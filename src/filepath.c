/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * filepath.c: dealing with file names and paths.
 */

#அந்தர்பாவ "vim.h"

#ifdef MSWIN
/*
 * Functions for ":8" filename modifier: get 8.3 version of a filename.
 */

/*
 * Get the short path (8.3) for the filename in "fnamep".
 * Only works for a valid file name.
 * When the path gets longer "fnamep" is changed and the allocated buffer
 * is put in "bufp".
 * *fnamelen is the length of "fnamep" and set to 0 for a nonexistent path.
 * Returns OK on success, FAIL on failure.
 */
    நிலையான முழு
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    முழு		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * அளவு(*newbuf));
    எனில் (newbuf == வெற்று)
	திரும்பு FAIL;

    wfname = enc_to_utf16(*fnamep, வெற்று);
    எனில் (wfname == வெற்று)
    {
	vim_free(newbuf);
	திரும்பு FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    எனில் (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * அளவு(*newbuf));
	எனில் (newbuf == வெற்று)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    திரும்பு FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    எனில் (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, வெற்று);

	எனில் (p != வெற்று)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	இல்லையெனில்
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    திரும்பு FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    திரும்பு OK;
}

/*
 * Get the short path (8.3) for the filename in "fname". The converted
 * path is returned in "bufp".
 *
 * Some of the directories specified in "fname" may not exist. This function
 * will shorten the existing directories at the beginning of the path and then
 * append the remaining non-existing path.
 *
 * fname - Pointer to the filename to shorten.  On return, contains the
 *	   pointer to the shortened pathname
 * bufp -  Pointer to an allocated buffer for the filename.
 * fnamelen - Length of the filename pointed to by fname
 *
 * Returns OK on success (or nothing done) and FAIL on failure (out of memory).
 */
    நிலையான முழு
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = வெற்று;
    char_u	*short_fname = வெற்று;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    முழு		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    எனில் (save_fname == வெற்று)
    {
	retval = FAIL;
	செல் theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    க்கு (;;)
    {
	// go back one path-separator
	போது (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	எனில் (endp <= save_fname)
	    நிறுத்து;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	எனில் (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    செல் theend;
	}
	*endp = ch;	// preserve the string

	எனில் (len > 0)
	    நிறுத்து;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    எனில் (len > 0)
    {
	/*
	 * Succeeded in shortening the path. Now concatenate the shortened
	 * path with the remaining path at the tail.
	 */

	// Compute the length of the new path.
	sfx_len = (save_endp - endp) + 1;
	new_len = len + sfx_len;

	*fnamelen = new_len;
	vim_free(*bufp);
	எனில் (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    எனில் (*fname == வெற்று)
	    {
		retval = FAIL;
		செல் theend;
	    }
	}
	இல்லையெனில்
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    எனில் (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = வெற்று;
	}

	// concat the not-shortened part of the path
	எனில் ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    திரும்பு retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    நிலையான முழு
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    முழு		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    முழு		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    க்கு (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	எனில் (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    எனில் (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    இல்லையெனில்
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    எனில் (tfname == வெற்று)
	திரும்பு FAIL;

    len = tflen = STRLEN(tfname);

    எனில் (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	திரும்பு FAIL;

    எனில் (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	எனில் (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    திரும்பு FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    க்கு (p = tfname + len - 1; p >= tfname; --p)
    {
	எனில் (has_mbyte)
	    p -= mb_head_off(tfname, p);
	எனில் (vim_ispathsep(*p))
	{
	    எனில் (sepcount == 0 || (hasTilde && sepcount == 1))
		நிறுத்து;
	    இல்லையெனில்
		sepcount --;
	}
    }
    எனில் (hasTilde)
    {
	--p;
	எனில் (p >= tfname)
	    *p = '~';
	இல்லையெனில்
	    திரும்பு FAIL;
    }
    இல்லையெனில்
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    திரும்பு OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    முழு
modify_fname(
    char_u	*src,		// string with modifiers
    முழு		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    முழு		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    முழு		c;
    முழு		has_fullname = 0;
    முழு		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    முழு		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    எனில் (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	எனில் ((*fnamep)[0] == '~'
#எனில் !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
		&& ((*fnamep)[1] == '/'
# ifdef BACKSLASH_IN_FILENAME
		    || (*fnamep)[1] == '\\'
# endif
		    || (*fnamep)[1] == NUL)
#endif
		&& !(tilde_file && (*fnamep)[1] == NUL)
	   )
	{
	    *fnamep = expand_env_save(*fnamep);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    எனில் (*fnamep == வெற்று)
		திரும்பு -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	க்கு (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    எனில் (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		நிறுத்து;
	}

	// FullName_save() is slow, don't use it when not needed.
	எனில் (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    எனில் (*fnamep == வெற்று)
		திரும்பு -1;
	}

#ifdef MSWIN
# எனில் _WIN32_WINNT >= 0x0500
	எனில் (vim_strchr(*fnamep, '~') != வெற்று)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, வெற்று);
	    WCHAR buf[_MAX_PATH];

	    எனில் (wfname != வெற்று)
	    {
		எனில் (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, வெற்று);

		    எனில் (q != வெற்று)
		    {
			vim_free(*bufp);    // free any allocated file name
			*bufp = *fnamep = q;
		    }
		}
		vim_free(wfname);
	    }
	}
# endif
#endif
	// Append a path separator to a directory.
	எனில் (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    எனில் (*fnamep == வெற்று)
		திரும்பு -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    போது (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	எனில் (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    தொடர்;
	}
	pbuf = வெற்று;
	// Need full path first (use expand_env() to remove a "~/")
	எனில் (!has_fullname && !has_homerelative)
	{
	    எனில் (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    இல்லையெனில்
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	இல்லையெனில்
	    p = *fnamep;

	has_fullname = 0;

	எனில் (p != வெற்று)
	{
	    size_t  dirnamelen = 0;

	    எனில் (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		எனில் (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    எனில் (s != வெற்று)
		    {
			dirnamelen = home_replace(வெற்று, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		எனில் (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		எனில் (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    எனில் (vim_ispathsep(*p))
		    {
			போது (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			எனில் (pbuf != வெற்று)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = வெற்று;
			}
		    }
		}
	    }
	    இல்லையெனில்
	    {
		dirnamelen = home_replace(வெற்று, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		எனில் (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    எனில் (s != வெற்று)
		    {
			*fnamep = s;
			vim_free(*bufp);
			*bufp = s;
			has_homerelative = TRUE;
		    }
		}
	    }
	    vim_free(pbuf);
	}
    }

    tail = gettail(*fnamep);
    *fnamelen = STRLEN(*fnamep);

    // ":h" - head, remove "/file_name", can be repeated
    // Don't remove the first "/" or "c:\"
    போது (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	போது (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	எனில் (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	எனில் (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    எனில் (p == வெற்று)
		திரும்பு -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	இல்லையெனில்
	{
	    போது (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    எனில் (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
    {
	*usedlen += 2;
#ifdef MSWIN
	has_shortname = 1;
#endif
    }

#ifdef MSWIN
    /*
     * Handle ":8" after we have done 'heads' and before we do 'tails'.
     */
    எனில் (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	எனில் (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    எனில் (p == வெற்று)
		திரும்பு -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	எனில் (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    எனில் (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		திரும்பு -1;
	}
	இல்லையெனில்
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    எனில் (get_short_pathname(fnamep, bufp, &l) == FAIL)
		திரும்பு -1;

	    எனில் (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		எனில் (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    திரும்பு -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    எனில் (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    போது (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	எனில் (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	இல்லையெனில்
	    s = *fnamep + *fnamelen - 1;
	க்கு ( ; s > tail; --s)
	    எனில் (s[0] == '.')
		நிறுத்து;
	எனில் (src[*usedlen + 1] == 'e')		// :e
	{
	    எனில் (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		க்கு ( ; s > *fnamep; --s)
		    எனில் (s[0] == ';')
			நிறுத்து;
		எனில் (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    இல்லையெனில் எனில் (*fnamep <= tail)
		*fnamelen = 0;
	}
	இல்லையெனில்				// :r
	{
	    char_u *limit = *fnamep;

	    எனில் (limit < tail)
		limit = tail;
	    எனில் (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    எனில் (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	முழு	    sep;
	char_u	    *flags;
	முழு	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	எனில் (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	எனில் (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    எனில் (p != வெற்று)
	    {
		pat = vim_strnsave(s, p - s);
		எனில் (pat != வெற்று)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    எனில் (p != வெற்று)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			எனில் (sub != வெற்று && str != வெற்று)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, வெற்று, flags, &slen);
			    எனில் (s != வெற்று)
			    {
				*fnamep = s;
				*fnamelen = slen;
				vim_free(*bufp);
				*bufp = s;
				didit = TRUE;
			    }
			}
			vim_free(sub);
			vim_free(str);
		    }
		    vim_free(pat);
		}
	    }
	    // after using ":s", repeat all the modifiers
	    எனில் (didit)
		செல் repeat;
	}
    }

    எனில் (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	எனில் (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	எனில் (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	எனில் (p == வெற்று)
	    திரும்பு -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    திரும்பு valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    நிலையான வெறுமை
shorten_dir_len(char_u *str, முழு trim_len)
{
    char_u	*tail, *s, *d;
    முழு		skip = FALSE;
    முழு		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    க்கு (s = str; ; ++s)
    {
	எனில் (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    எனில் (*s == NUL)
		நிறுத்து;
	}
	இல்லையெனில் எனில் (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	இல்லையெனில் எனில் (!skip)
	{
	    *d++ = *s;			// copy next char
	    எனில் (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		எனில் (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    எனில் (has_mbyte)
	    {
		முழு l = mb_ptr2len(s);

		போது (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    வெறுமை
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    முழு
file_is_readable(char_u *fname)
{
    முழு		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    எனில் (*fname == NUL || mch_isdir_or_missing(fname))
	திரும்பு FALSE;
    எனில் ((fd = mch_open((எழுத்து *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

#எனில் defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    வெறுமை
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (check_secure())
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	எனில் (in_vim9script())
	    (வெறுமை) check_for_string_arg(argvars, 0);
	திரும்பு;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    எனில் (cwd != வெற்று)
    {
	எனில் (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	எனில் (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	இல்லையெனில் எனில் (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	இல்லையெனில் எனில் (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	இல்லையெனில்
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "scope", s);
	    திரும்பு;
	}
    }
    இல்லையெனில் எனில் (curwin->w_localdir != வெற்று)
	scope = CDSCOPE_WINDOW;
    இல்லையெனில் எனில் (curtab->tp_localdir != வெற்று)
	scope = CDSCOPE_TABPAGE;

    எனில் (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    வெறுமை
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_எண் = -1;
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    name = tv_get_string(&argvars[0]);
    எனில் (name == வெற்று || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	திரும்பு;
    }

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    இல்லையெனில்
	flags = (char_u *)"";

    எனில் (*flags == NUL)
	// delete a file
	rettv->vval.v_எண் = mch_remove(name) == 0 ? 0 : -1;
    இல்லையெனில் எனில் (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_எண் = mch_rmdir(name) == 0 ? 0 : -1;
    இல்லையெனில் எனில் (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_எண் = delete_recursive(name);
    இல்லையெனில்
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    வெறுமை
f_executable(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_எண் = mch_can_exe(tv_get_string(&argvars[0]), வெற்று, TRUE);
}

/*
 * "exepath()" function
 */
    வெறுமை
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = வெற்று;

    எனில் (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	திரும்பு;
    (வெறுமை)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    வெறுமை
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;
    rettv->vval.v_எண் = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    வெறுமை
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;
    rettv->vval.v_எண் = filewritable(tv_get_string(&argvars[0]));
}

    நிலையான வெறுமை
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    முழு		find_what)
{
    char_u	*fname;
    char_u	*fresult = வெற்று;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    முழு		மொத்தம் = 1;
    முழு		first = TRUE;
    முழு		error = FALSE;

    rettv->vval.v_string = வெற்று;
    rettv->v_type = VAR_STRING;
    எனில் (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 2) == FAIL)))
	திரும்பு;

    fname = tv_get_string(&argvars[0]);

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	எனில் (p == வெற்று)
	    error = TRUE;
	இல்லையெனில்
	{
	    எனில் (*p != NUL)
		path = p;

	    எனில் (argvars[2].v_type != VAR_UNKNOWN)
		மொத்தம் = (முழு)tv_get_எண்_chk(&argvars[2], &error);
	}
    }

    எனில் (மொத்தம் < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    எனில் (*fname != NUL && !error)
    {
	char_u	*file_to_find = வெற்று;
	எழுத்து	*தேடல்_ctx = வெற்று;

	செய்
	{
	    எனில் (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : வெற்று,
					       first ? (முழு)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &தேடல்_ctx);
	    first = FALSE;

	    எனில் (fresult != வெற்று && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} போது ((rettv->v_type == VAR_LIST || --மொத்தம் > 0) && fresult != வெற்று);

	vim_free(file_to_find);
	vim_findfile_cleanup(தேடல்_ctx);
    }

    எனில் (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    வெறுமை
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    வெறுமை
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    வெறுமை
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = வெற்று;
    char_u	buf[NUMBUFLEN];

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    எனில் (mods == வெற்று || fname == வெற்று)
	fname = வெற்று;
    இல்லையெனில்
    {
	len = STRLEN(fname);
	எனில் (mods != வெற்று && *mods != NUL)
	    (வெறுமை)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    எனில் (fname == வெற்று)
	rettv->vval.v_string = வெற்று;
    இல்லையெனில்
	rettv->vval.v_string = vim_strnsave(fname, len);
    vim_free(fbuf);
}

/*
 * "getcwd()" function
 *
 * Return the current working directory of a window in a tab page.
 * First optional argument 'winnr' is the window number or -1 and the second
 * optional argument 'tabnr' is the tab page number.
 *
 * If no arguments are supplied, then return the directory of the current
 * window.
 * If only 'winnr' is specified and is not -1 or 0 then return the directory of
 * the specified window.
 * If 'winnr' is 0 then return the directory of the current window.
 * If both 'winnr and 'tabnr' are specified and 'winnr' is -1 then return the
 * directory of the specified tab page.  Otherwise return the directory of the
 * specified window in the specified tab page.
 * If the window or the tab page doesn't exist then return NULL.
 */
    வெறுமை
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = வெற்று;
    tabpage_T	*tp = வெற்று;
    char_u	*cwd;
    முழு		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_opt_எண்_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 1) == FAIL)))
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_எண்
	    && argvars[0].vval.v_எண் == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    இல்லையெனில்
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    எனில் (wp != வெற்று && wp->w_localdir != வெற்று
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    இல்லையெனில் எனில் (tp != வெற்று && tp->tp_localdir != வெற்று
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    இல்லையெனில் எனில் (wp != வெற்று || tp != வெற்று || global)
    {
	எனில் (globaldir != வெற்று && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	இல்லையெனில்
	{
	    cwd = alloc(MAXPATHL);
	    எனில் (cwd != வெற்று)
	    {
		எனில் (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    எனில் (rettv->vval.v_string != வெற்று)
	slash_adjust(rettv->vval.v_string);
# endif
}

/*
 * Convert "st" to file permission string.
 */
    char_u *
getfpermst(stat_T *st, char_u *perm)
{
    char_u	    flags[] = "rwx";
    முழு		    i;

    க்கு (i = 0; i < 9; i++)
    {
	எனில் (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	இல்லையெனில்
	    perm[i] = '-';
    }
    திரும்பு perm;
}

/*
 * "getfperm({fname})" function
 */
    வெறுமை
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = வெற்று;
    char_u	permbuf[] = "---------";

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    எனில் (mch_stat((எழுத்து *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    வெறுமை
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    fname = tv_get_string(&argvars[0]);
    எனில் (mch_stat((எழுத்து *)fname, &st) >= 0)
    {
	எனில் (S_ISDIR(st.st_mode))
	    rettv->vval.v_எண் = 0;
	இல்லையெனில்
	{
	    rettv->vval.v_எண் = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    எனில் ((off_T)rettv->vval.v_எண் != (off_T)st.st_size)
		rettv->vval.v_எண் = -2;
	}
    }
    இல்லையெனில்
	  rettv->vval.v_எண் = -1;
}

/*
 * "getftime({fname})" function
 */
    வெறுமை
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    fname = tv_get_string(&argvars[0]);
    எனில் (mch_stat((எழுத்து *)fname, &st) >= 0)
	rettv->vval.v_எண் = (varnumber_T)st.st_mtime;
    இல்லையெனில்
	rettv->vval.v_எண் = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    எழுத்து    *t;

    எனில் (S_ISREG(st->st_mode))
	t = "file";
    இல்லையெனில் எனில் (S_ISDIR(st->st_mode))
	t = "dir";
    இல்லையெனில் எனில் (S_ISLNK(st->st_mode))
	t = "link";
    இல்லையெனில் எனில் (S_ISBLK(st->st_mode))
	t = "bdev";
    இல்லையெனில் எனில் (S_ISCHR(st->st_mode))
	t = "cdev";
    இல்லையெனில் எனில் (S_ISFIFO(st->st_mode))
	t = "fifo";
    இல்லையெனில் எனில் (S_ISSOCK(st->st_mode))
	t = "socket";
    இல்லையெனில்
	t = "other";
    திரும்பு (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    வெறுமை
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = வெற்று;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    எனில் (mch_lstat((எழுத்து *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    வெறுமை
f_glob(typval_T *argvars, typval_T *rettv)
{
    முழு		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    முழு		error = FALSE;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	திரும்பு;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	எனில் (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    எனில் (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, வெற்று);
	    எனில் (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    எனில் (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	எனில் (p_wic)
	    options += WILD_ICASE;
	எனில் (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     வெற்று, options, WILD_ALL);
	இல்லையெனில் எனில் (rettv_list_alloc(rettv) == OK)
	{
	  முழு i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						வெற்று, options, WILD_ALL_KEEP);
	  க்கு (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    இல்லையெனில்
	rettv->vval.v_string = வெற்று;
}

/*
 * "glob2regpat()" function
 */
    வெறுமை
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == வெற்று)
			 ? வெற்று : file_pat_to_reg_pat(pat, வெற்று, வெற்று, FALSE);
}

/*
 * "globpath()" function
 */
    வெறுமை
f_globpath(typval_T *argvars, typval_T *rettv)
{
    முழு		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    முழு		error = FALSE;
    garray_T	ga;
    முழு		i;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	திரும்பு;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	எனில் (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	எனில் (argvars[3].v_type != VAR_UNKNOWN)
	{
	    எனில் (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, வெற்று);
	    எனில் (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    எனில் (file != வெற்று && !error)
    {
	ga_init2(&ga, அளவு(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	எனில் (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	இல்லையெனில் எனில் (rettv_list_alloc(rettv) == OK)
	    க்கு (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    இல்லையெனில்
	rettv->vval.v_string = வெற்று;
}

/*
 * "isdirectory()" function
 */
    வெறுமை
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    வெறுமை
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    நிலையான முழு
mkdir_recurse(char_u *dir, முழு prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    முழு		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    எனில் (p <= get_past_head(dir))
	திரும்பு OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    எனில் (updir == வெற்று)
	திரும்பு FAIL;
    எனில் (mch_isdir(updir))
	r = OK;
    இல்லையெனில் எனில் (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	எனில் (r == OK && created != வெற்று && *created == வெற்று)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    திரும்பு r;
}

/*
 * "mkdir()" function
 */
    வெறுமை
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    முழு		prot = 0755;
    முழு		defer = FALSE;
    முழு		defer_recurse = FALSE;
    char_u	*created = வெற்று;

    rettv->vval.v_எண் = FAIL;
    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 2) == FAIL)))
	திரும்பு;

    dir = tv_get_string_buf(&argvars[0], buf);
    எனில் (*dir == NUL)
	திரும்பு;

    எனில் (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (முழு)tv_get_எண்_chk(&argvars[2], வெற்று);
	    எனில் (prot == -1)
		திரும்பு;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != வெற்று;
	defer_recurse = vim_strchr(arg2, 'R') != வெற்று;
	எனில் ((defer || defer_recurse) && !can_add_defer())
	    திரும்பு;

	எனில் (vim_strchr(arg2, 'p') != வெற்று)
	{
	    எனில் (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_எண் = OK;
		திரும்பு;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : வெற்று);
	}
    }
    rettv->vval.v_எண் = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    எனில் (rettv->vval.v_எண் == OK
				&& created == வெற்று && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    எனில் (created != வெற்று)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	எனில் (tv[0].vval.v_string == வெற்று || tv[1].vval.v_string == வெற்று
		|| add_defer((char_u *)"delete", 2, tv) == FAIL)
	{
	    vim_free(tv[0].vval.v_string);
	    vim_free(tv[1].vval.v_string);
	}
    }
}

/*
 * "pathshorten()" function
 */
    வெறுமை
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    முழு		trim_len = 1;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (முழு)tv_get_எண்(&argvars[1]);
	எனில் (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    எனில் (p == வெற்று)
	rettv->vval.v_string = வெற்று;
    இல்லையெனில்
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	எனில் (p != வெற்று)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    நிலையான முழு
checkitem_common(வெறுமை *context, char_u *name, dict_T *dict)
{
    typval_T	*கோவை = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    முழு		retval = 0;
    முழு		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    எனில் (name != வெற்று)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    இல்லையெனில்
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    எனில் (eval_கோவை_typval(கோவை, FALSE, argv, 1, வெற்று, &rettv) == FAIL)
	செல் theend;

    // We want to use -1, but also true/false should be allowed.
    எனில் (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_எண்;
	rettv.vval.v_எண் = rettv.vval.v_எண் == VVAL_TRUE;
    }
    retval = tv_get_எண்_chk(&rettv, &error);
    எனில் (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    எனில் (name != வெற்று)
	set_vim_var_string(VV_VAL, வெற்று, 0);
    இல்லையெனில்
	set_vim_var_dict(VV_VAL, வெற்று);
    restore_vimvar(VV_VAL, &save_val);
    திரும்பு retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    நிலையான முழு
readdir_checkitem(வெறுமை *context, வெறுமை *item)
{
    char_u	*name = (char_u *)item;

    திரும்பு checkitem_common(context, name, வெற்று);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    நிலையான முழு
readdirex_dict_arg(typval_T *argvars, முழு *cmp)
{
    char_u     *compare;

    எனில் (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	திரும்பு FAIL;

    எனில் (dict_has_சாவி(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    இல்லையெனில்
    {
	semsg(_(e_dictionary_சாவி_str_required), "sort");
	திரும்பு FAIL;
    }

    எனில் (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    இல்லையெனில் எனில் (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    இல்லையெனில் எனில் (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    இல்லையெனில் எனில் (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    திரும்பு OK;
}

/*
 * "readdir()" function
 */
    வெறுமை
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*கோவை;
    முழு		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    முழு		i;
    முழு		sort = READDIR_SORT_BYTE;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;
    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	திரும்பு;

    path = tv_get_string(&argvars[0]);
    கோவை = &argvars[1];

    எனில் (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	திரும்பு;

    ret = readdir_core(&ga, path, FALSE, (வெறுமை *)கோவை,
	    (கோவை->v_type == VAR_UNKNOWN) ? வெற்று : readdir_checkitem, sort);
    எனில் (ret == OK)
    {
	க்கு (i = 0; i < ga.ga_len; i++)
	{
	    p = ((char_u **)ga.ga_data)[i];
	    list_append_string(rettv->vval.v_list, p, -1);
	}
    }
    ga_clear_strings(&ga);
}

/*
 * Evaluate "expr" (= "context") for readdirex().
 */
    நிலையான முழு
readdirex_checkitem(வெறுமை *context, வெறுமை *item)
{
    dict_T	*dict = (dict_T*)item;

    திரும்பு checkitem_common(context, வெற்று, dict);
}

/*
 * "readdirex()" function
 */
    வெறுமை
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*கோவை;
    முழு		ret;
    char_u	*path;
    garray_T	ga;
    முழு		i;
    முழு		sort = READDIR_SORT_BYTE;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;
    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	திரும்பு;

    path = tv_get_string(&argvars[0]);
    கோவை = &argvars[1];

    எனில் (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	திரும்பு;

    ret = readdir_core(&ga, path, TRUE, (வெறுமை *)கோவை,
	    (கோவை->v_type == VAR_UNKNOWN) ? வெற்று : readdirex_checkitem, sort);
    எனில் (ret == OK)
    {
	க்கு (i = 0; i < ga.ga_len; i++)
	{
	    dict_T  *dict = ((dict_T**)ga.ga_data)[i];
	    list_append_dict(rettv->vval.v_list, dict);
	    dict_unref(dict);
	}
    }
    ga_clear(&ga);
}

/*
 * "readfile()" function
 */
    நிலையான வெறுமை
read_file_or_blob(typval_T *argvars, typval_T *rettv, முழு always_blob)
{
    முழு		இருமம் = FALSE;
    முழு		blob = always_blob;
    முழு		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    முழு		io_size = அளவு(buf);
    முழு		readlen;		// size of last fread()
    char_u	*prev	 = வெற்று;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	எனில் (always_blob)
	{
	    offset = (off_T)tv_get_எண்(&argvars[1]);
	    எனில் (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_எண்(&argvars[2]);
	}
	இல்லையெனில்
	{
	    எனில் (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		இருமம் = TRUE;
	    எனில் (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    எனில் (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_எண்(&argvars[2]);
	}
    }

    எனில் ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	திரும்பு;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    எனில் (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	திரும்பு;
    }
    எனில் (*fname == NUL || (fd = mch_fopen((எழுத்து *)fname, READBIN)) == வெற்று)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	திரும்பு;
    }

    எனில் (blob)
    {
	எனில் (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	திரும்பு;
    }

    போது (cnt < maxline || maxline < 0)
    {
	readlen = (முழு)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	க்கு (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || இருமம்));
		++p)
	{
	    எனில் (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= வெற்று;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		எனில் (readlen > 0 && !இருமம்)
		{
		    போது (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    எனில் (len == 0)
			போது (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		எனில் (prevlen == 0)
		    s = vim_strnsave(start, len);
		இல்லையெனில்
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    எனில் ((s = vim_realloc(prev, prevlen + len + 1)) != வெற்று)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = வெற்று; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		எனில் (s == வெற்று)
		{
		    do_outofmem_செய்தி((long_u) prevlen + len + 1);
		    failed = TRUE;
		    நிறுத்து;
		}

		எனில் ((li = listitem_alloc()) == வெற்று)
		{
		    vim_free(s);
		    failed = TRUE;
		    நிறுத்து;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		எனில் ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    இல்லையெனில் எனில் (*p == 0xbf && enc_utf8 && !இருமம்)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		எனில் (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    எனில் (start == dest)
			start = p + 1;
		    இல்லையெனில்
		    {
			// have to shuffle buf to close gap
			முழு adjust_prevlen = 0;

			எனில் (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (முழு)(buf - dest);
			    dest = buf;
			}
			எனில் (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	எனில் (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    நிறுத்து;
	எனில் (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    எனில் (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		எனில் (prevsize == 0)
		    prevsize = (long)(p - start);
		இல்லையெனில்
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		எனில் (newprev == வெற்று)
		{
		    do_outofmem_செய்தி((long_u)prevsize);
		    failed = TRUE;
		    நிறுத்து;
		}
		prev = newprev;
	    }
	    // Add the line part to end of "prev".
	    mch_memmove(prev + prevlen, start, p - start);
	    prevlen += (long)(p - start);
	}
    } // while

    // For a negative line count use only the lines at the end of the file,
    // free the rest.
    எனில் (!failed && maxline < 0)
	போது (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    எனில் (failed)
    {
	// an empty list is returned on error
	list_free(rettv->vval.v_list);
	rettv_list_alloc(rettv);
    }

    vim_free(prev);
    fclose(fd);
}

/*
 * "readblob()" function
 */
    வெறுமை
f_readblob(typval_T *argvars, typval_T *rettv)
{
    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 2) == FAIL)))
	திரும்பு;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    வெறுமை
f_readfile(typval_T *argvars, typval_T *rettv)
{
    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 2) == FAIL)))
	திரும்பு;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    வெறுமை
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = வெற்று;
# endif

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = வெற்று;

	v = mch_resolve_path(p, TRUE);
	எனில் (v != வெற்று)
	    rettv->vval.v_string = v;
	இல்லையெனில்
	    rettv->vval.v_string = vim_strsave(p);
    }
# இல்லையெனில்
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	முழு	len;
	char_u	*remain = வெற்று;
	char_u	*q;
	முழு	is_relative_to_current = FALSE;
	முழு	has_trailing_pathsep = FALSE;
	முழு	limit = 100;

	p = vim_strsave(p);
	எனில் (p == வெற்று)
	    செல் fail;
	எனில் (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	எனில் (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	எனில் (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	எனில் (buf == வெற்று)
	{
	    vim_free(p);
	    vim_free(remain);
	    செல் fail;
	}

	க்கு (;;)
	{
	    க்கு (;;)
	    {
		len = readlink((எழுத்து *)p, (எழுத்து *)buf, MAXPATHL);
		எனில் (len <= 0)
		    நிறுத்து;
		buf[len] = NUL;

		எனில் (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = வெற்று;
		    செல் fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		எனில் (remain == வெற்று && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		எனில் (*q != NUL)
		{
		    எனில் (remain == வெற்று)
			remain = vim_strsave(q - 1);
		    இல்லையெனில்
		    {
			cpy = concat_str(q - 1, remain);
			எனில் (cpy != வெற்று)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		எனில் (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		எனில் (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    எனில் (cpy != வெற்று)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		இல்லையெனில்
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    எனில் (remain == வெற்று)
		நிறுத்து;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    எனில் (cpy != வெற்று)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    எனில் (*q != NUL)
		STRMOVE(remain, q - 1);
	    இல்லையெனில்
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	எனில் (!vim_ispathsep(*p))
	{
	    எனில் (is_relative_to_current
		    && *p != NUL
		    && !(p[0] == '.'
			&& (p[1] == NUL
			    || vim_ispathsep(p[1])
			    || (p[1] == '.'
				&& (p[2] == NUL
				    || vim_ispathsep(p[2]))))))
	    {
		// Prepend "./".
		cpy = concat_str((char_u *)"./", p);
		எனில் (cpy != வெற்று)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    இல்லையெனில் எனில் (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		போது (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		எனில் (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	எனில் (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    எனில் (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  இல்லையெனில்
    rettv->vval.v_string = vim_strsave(p);
#  endif
# endif

    simplify_filename(rettv->vval.v_string);

# ifdef HAVE_READLINK
fail:
    vim_free(buf);
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "tempname()" function
 */
    வெறுமை
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    நிலையான முழு	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    செய்
    {
	எனில் (x == 'Z')
	    x = '0';
	இல்லையெனில் எனில் (x == '9')
	    x = 'A';
	இல்லையெனில்
	    ++x;
    } போது (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    வெறுமை
f_writefile(typval_T *argvars, typval_T *rettv)
{
    முழு		இருமம் = FALSE;
    முழு		append = FALSE;
    முழு		defer = FALSE;
# ifdef HAVE_FSYNC
    முழு		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    முழு		ret = 0;
    listitem_T	*li;
    list_T	*list = வெற்று;
    blob_T	*blob = வெற்று;

    rettv->vval.v_எண் = -1;
    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	எனில் (list == வெற்று)
	    திரும்பு;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_பொருட்கள்(list, li)
	    எனில் (tv_get_string_chk(&li->li_tv) == வெற்று)
		திரும்பு;
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	எனில் (blob == வெற்று)
	    திரும்பு;
    }
    இல்லையெனில்
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	திரும்பு;
    }

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	எனில் (arg2 == வெற்று)
	    திரும்பு;
	எனில் (vim_strchr(arg2, 'b') != வெற்று)
	    இருமம் = TRUE;
	எனில் (vim_strchr(arg2, 'a') != வெற்று)
	    append = TRUE;
	எனில் (vim_strchr(arg2, 'D') != வெற்று)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	எனில் (vim_strchr(arg2, 's') != வெற்று)
	    do_fsync = TRUE;
	இல்லையெனில் எனில் (vim_strchr(arg2, 'S') != வெற்று)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    எனில் (fname == வெற்று)
	திரும்பு;

    எனில் (defer && !can_add_defer())
	திரும்பு;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    எனில் (*fname == NUL || (fd = mch_fopen((எழுத்து *)fname,
				      append ? APPENDBIN : WRITEBIN)) == வெற்று)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    இல்லையெனில்
    {
	எனில் (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    எனில் (tv.vval.v_string == வெற்று
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(வெறுமை)mch_remove(fname);
	    }
	}

	எனில் (ret == 0)
	{
	    எனில் (blob)
	    {
		எனில் (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    இல்லையெனில்
	    {
		எனில் (write_list(fd, list, இருமம்) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    எனில் (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_எண் = ret;
}

#endif // FEAT_EVAL

#எனில் defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    முழு		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    நிலையான char_u	*last_dir = வெற்று;    // last used directory
    char_u		*tofree = வெற்று;
    முழு			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    எனில் (title == வெற்று || *title == NUL)
    {
	எனில் (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	இல்லையெனில் எனில் (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	இல்லையெனில்
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    எனில் ((initdir == வெற்று || *initdir == NUL) && dflt != வெற்று && *dflt != NUL)
    {
	எனில் (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = வெற்று;
	}
	இல்லையெனில் எனில் (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    எனில் (tofree != வெற்று)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    எனில் (initdir == வெற்று || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	எனில் (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	இல்லையெனில் எனில் (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != வெற்று && buf->b_ffname != வெற்று)
	{
	    எனில் (dflt == வெற்று || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    எனில் (tofree != வெற்று)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	இல்லையெனில் எனில் (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    எனில் (gui.in_use)		// when this changes, also adjust f_has()!
    {
	எனில் (filter == வெற்று
#  ifdef FEAT_EVAL
		&& (filter = get_var_மதிப்பு((char_u *)"b:browsefilter")) == வெற்று
		&& (filter = get_var_மதிப்பு((char_u *)"g:browsefilter")) == வெற்று
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	எனில் (flags & BROWSE_DIR)
	{
#  எனில் defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  இல்லையெனில்
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  எனில் !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    எனில் (fname != வெற்று && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		எனில் (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	இல்லையெனில்
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    இல்லையெனில்
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = வெற்று;
    }

    // keep the directory for next time
    எனில் (fname != வெற்று)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	எனில் (last_dir != வெற்று && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    எனில் (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		எனில் (last_dir != வெற்று)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    திரும்பு fname;
}
#endif

#எனில் defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    வெறுமை
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    முழு		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    முழு		error = FALSE;

    எனில் (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	திரும்பு;

    save = (முழு)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    எனில் (error || title == வெற்று || initdir == வெற்று || defname == வெற்று)
	rettv->vval.v_string = வெற்று;
    இல்லையெனில்
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, வெற்று, initdir, வெற்று, curbuf);
# இல்லையெனில்
    rettv->vval.v_string = வெற்று;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    வெறுமை
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    எனில் (title == வெற்று || initdir == வெற்று)
	rettv->vval.v_string = வெற்று;
    இல்லையெனில்
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, வெற்று, வெற்று, initdir, வெற்று, curbuf);
# இல்லையெனில்
    rettv->vval.v_string = வெற்று;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    வெறுமை
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_எண் = FALSE;

    எனில் (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	திரும்பு;

    from = tv_get_string(&argvars[0]);

    எனில் (mch_lstat((எழுத்து *)from, &st) >= 0
	&& (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)))
	rettv->vval.v_எண் = vim_copyfile(
	    tv_get_string(&argvars[0]),
	    tv_get_string(&argvars[1])) == OK ? TRUE : FALSE;
}

#endif // FEAT_EVAL

/*
 * Replace home directory by "~" in each space or comma separated file name in
 * 'src'.
 * If anything fails (except when out of space) dst equals src.
 */
    size_t
home_replace(
    buf_T	*buf,	// when not NULL, check for help files
    char_u	*src,	// input file name
    char_u	*dst,	// where to put the result
    முழு		dstlen,	// maximum length of the result
    முழு		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    எனில் (src == வெற்று)
    {
	*dst = NUL;
	திரும்பு 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    எனில் (buf != வெற்று && buf->b_help)
	திரும்பு vim_snprintf_safelen((எழுத்து *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    எனில் (homedir != வெற்று)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#இல்லையெனில்
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    எனில் (homedir_env == வெற்று)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    எனில் (homedir_env != வெற்று && *homedir_env == NUL)
	homedir_env = வெற்று;

    எனில் (homedir_env != வெற்று && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = வெற்று;

	flen = STRLEN(homedir_env);
	(வெறுமை)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	எனில் (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    எனில் (homedir_env != வெற்று)
	envlen = STRLEN(homedir_env);

    எனில் (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    போது (*src && dstlen > 0)
    {
	/*
	 * Here we are at the beginning of a file name.
	 * First, check to see if the beginning of the file name matches
	 * $HOME or the "real" home directory. Check that there is a '/'
	 * after the match (so that if e.g. the file is "/home/pieter/bla",
	 * and the home directory is "/home/piet", the file does not end up
	 * as "~er/bla" (which would seem to indicate the file "bla" in user
	 * er's home directory)).
	 */
	p = homedir;
	len = dirlen;
	க்கு (;;)
	{
	    எனில் (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		எனில் (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		நிறுத்து;
	    }
	    எனில் (p == homedir_env)
		நிறுத்து;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	போது (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	போது ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    எனில் (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    திரும்பு (size_t)(dst - dst_start);
}

/*
 * Like home_replace, store the replaced string in allocated memory.
 * When something fails, NULL is returned.
 */
    char_u  *
home_replace_save(
    buf_T	*buf,	// when not NULL, check for help files
    char_u	*src)	// input file name
{
    char_u	*dst;
    unsigned	len;

    len = 3;			// space for "~/" and trailing NUL
    எனில் (src != வெற்று)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    எனில் (dst != வெற்று)
	home_replace(buf, src, dst, len, TRUE);
    திரும்பு dst;
}

/*
 * Compare two file names and return:
 * FPC_SAME   if they both exist and are the same file.
 * FPC_SAMEX  if they both don't exist and have the same file name.
 * FPC_DIFF   if they both exist and are different files.
 * FPC_NOTX   if they both don't exist.
 * FPC_DIFFX  if one of them doesn't exist.
 * For the first name environment variables are expanded if "expandenv" is
 * TRUE.
 */
    முழு
fullpathcmp(
    char_u *s1,
    char_u *s2,
    முழு	    checkname,		// when both don't exist, check file names
    முழு	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    முழு		    r1, r2;

    எனில் (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    இல்லையெனில்
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((எழுத்து *)exp1, &st1);
    r2 = mch_stat((எழுத்து *)s2, &st2);
    எனில் (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	எனில் (checkname)
	{
	    எனில் (fnamecmp(exp1, s2) == 0)
		திரும்பு FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    எனில் (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		திரும்பு FPC_SAMEX;
	}
	திரும்பு FPC_NOTX;
    }
    எனில் (r1 != 0 || r2 != 0)
	திரும்பு FPC_DIFFX;
    எனில் (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	திரும்பு FPC_SAME;
    திரும்பு FPC_DIFF;
#இல்லையெனில்
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    முழு	    retval = FPC_DIFF;
    முழு	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    எனில் ((exp1 = alloc(MAXPATHL * 3)) != வெற்று)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	எனில் (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	இல்லையெனில்
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	எனில் (r1 != OK && r2 != OK)
	{
	    எனில் (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    இல்லையெனில்
		retval = FPC_NOTX;
	}
	இல்லையெனில் எனில் (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	இல்லையெனில் எனில் (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	இல்லையெனில்
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    திரும்பு retval;
#endif
}

/*
 * Get the tail of a path: the file name.
 * When the path ends in a path separator the tail is the NUL after it.
 * Fail safe: never returns NULL.
 */
    char_u *
gettail(char_u *fname)
{
    char_u  *p1, *p2;

    எனில் (fname == வெற்று)
	திரும்பு (char_u *)"";
    க்கு (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	எனில் (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    திரும்பு p1;
}

/*
 * Get pointer to tail of "fname", including path separators.  Putting a NUL
 * here leaves the directory name.  Takes care of "c:/" and "//".
 * Always returns a valid pointer.
 */
    char_u *
gettail_sep(char_u *fname)
{
    char_u	*p;
    char_u	*t;

    p = get_past_head(fname);	// don't remove the '/' from "c:/file"
    t = gettail(fname);
    போது (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    திரும்பு t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    போது (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    எனில் (*fname)
	++fname;
    திரும்பு fname;
}

/*
 * Get a pointer to one character past the head of a path name.
 * Unix: after "/"; DOS: after "c:\"; Amiga: after "disk:/"; Mac: no head.
 * If there is no head, path is returned.
 */
    char_u *
get_past_head(char_u *path)
{
    char_u  *retval;

#எனில் defined(MSWIN)
    // may skip "c:"
    எனில் (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    இல்லையெனில்
	retval = path;
#இல்லையெனில்
# எனில் defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    எனில் (retval == வெற்று)
	retval = path;
# இல்லையெனில்	// Unix
    retval = path;
# endif
#endif

    போது (vim_ispathsep(*retval))
	++retval;

    திரும்பு retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    முழு
vim_ispathsep(முழு c)
{
#ifdef UNIX
    திரும்பு (c == '/');	    // UNIX has ':' inside file names
#இல்லையெனில்
# ifdef BACKSLASH_IN_FILENAME
    திரும்பு (c == ':' || c == '/' || c == '\\');
# இல்லையெனில்
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    திரும்பு (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  இல்லையெனில்
    திரும்பு (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    முழு
vim_ispathsep_nocolon(முழு c)
{
    திரும்பு vim_ispathsep(c)
#ifdef BACKSLASH_IN_FILENAME
	&& c != ':'
#endif
	;
}

/*
 * Return TRUE if the directory of "fname" exists, FALSE otherwise.
 * Also returns TRUE if there is no directory name.
 * "fname" must be writable!.
 */
    முழு
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    முழு		c;
    முழு		retval;

    p = gettail_sep(fname);
    எனில் (p == fname)
	திரும்பு TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    திரும்பு retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    முழு
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    திரும்பு vim_fnamencmp(x, y, MAXPATHL);
#இல்லையெனில்
    எனில் (p_fic)
	திரும்பு MB_STRICMP(x, y);
    திரும்பு STRCMP(x, y);
#endif
}

    முழு
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    முழு		cx = NUL;
    முழு		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    க்கு (முழு swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	எனில் ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    எனில் (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		நிறுத்து;
	    }
	    இல்லையெனில் // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    போது (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	எனில் (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    நிறுத்து;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    எனில் (len == 0)
	திரும்பு 0;
    திரும்பு (cx - cy);
#இல்லையெனில்
    எனில் (p_fic)
	திரும்பு MB_STRNICMP(x, y, len);
    திரும்பு STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, முழு sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? அளவு(PATHSEP) : 0) + fname2len + 1);
    எனில் (ret->string == வெற்று)
	ret->length = 0;
    இல்லையெனில்
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	எனில் (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += அளவு(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    திரும்பு ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    வெறுமை
add_pathsep(char_u *p)
{
    size_t  plen;

    எனில் (*p == NUL)
	திரும்பு;

    plen = STRLEN(p);
    எனில் (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    முழு		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = வெற்று;

    எனில் (fname == வெற்று)
	திரும்பு வெற்று;

    buf = alloc(MAXPATHL);
    எனில் (buf == வெற்று)
	திரும்பு வெற்று;

    எனில் (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    இல்லையெனில்
	new_fname = vim_strsave(fname);
    vim_free(buf);
    திரும்பு new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    முழு
vim_fexists(char_u *fname)
{
    stat_T st;

    எனில் (mch_stat((எழுத்து *)fname, &st))
	திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    முழு
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    முழு		  *இலக்கம்_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    முழு		   flags)	// EW_DIR, etc.
{
    முழு		ret = FAIL;
    char_u	*eval_pat = வெற்று;
    char_u	*exp_pat = *pat;
    எழுத்து	*ignored_செய்தி;
    size_t	usedlen;
    முழு		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    முழு		star_follows = FALSE;

    எனில் (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       வெற்று, &ignored_செய்தி, வெற்று, TRUE);
	--emsg_off;
	எனில் (eval_pat != வெற்று)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    எனில் (exp_pat != வெற்று)
	ret = expand_wildcards(1, &exp_pat, இலக்கம்_file, file, flags);

    எனில் (eval_pat != வெற்று)
    {
	எனில் (*இலக்கம்_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    எனில் (*file != வெற்று)
	    {
		**file = eval_pat;
		eval_pat = வெற்று;
		*இலக்கம்_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    திரும்பு ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    முழு
expand_wildcards(
    முழு		   இலக்கம்_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    முழு		  *இலக்கம்_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    முழு		   flags)	// EW_DIR, etc.
{
    முழு		retval;
    முழு		i, j;
    char_u	*p;
    முழு		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(இலக்கம்_pat, pat, இலக்கம்_files, files, flags);

    // When keeping all matches, return here
    எனில் ((flags & EW_KEEPALL) || retval == FAIL)
	திரும்பு retval;

    /*
     * Remove names that match 'wildignore'.
     */
    எனில் (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	க்கு (i = 0; i < *இலக்கம்_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    எனில் (ffname == வெற்று)		// out of memory
		நிறுத்து;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    எனில் (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		க்கு (j = i; j + 1 < *இலக்கம்_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*இலக்கம்_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	எனில் (*இலக்கம்_files == 0)
	{
	    VIM_CLEAR(*files);
	    திரும்பு FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    எனில் (*இலக்கம்_files > 1 && !got_int)
    {
	non_suf_match = 0;
	க்கு (i = 0; i < *இலக்கம்_files; ++i)
	{
	    எனில் (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		க்கு (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    திரும்பு retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    முழு
match_suffix(char_u *fname)
{
    முழு		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (முழு)STRLEN(fname);
    setsuflen = 0;
    க்கு (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	எனில் (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    எனில் (vim_strchr(tail, '.') == வெற்று)
	    {
		setsuflen = 1;
		நிறுத்து;
	    }
	}
	இல்லையெனில்
	{
	    எனில் (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		நிறுத்து;
	    setsuflen = 0;
	}
    }
    திரும்பு (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    நிலையான முழு
vim_backtick(char_u *p)
{
    திரும்பு (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    நிலையான முழு
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    முழு		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    முழு		cnt = 0;
    முழு		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    எனில் (cmd == வெற்று)
	திரும்பு -1;

#ifdef FEAT_EVAL
    எனில் (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    இல்லையெனில்
#endif
	buffer = get_cmd_output(cmd, வெற்று,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, வெற்று);
    vim_free(cmd);
    எனில் (buffer == வெற்று)
	திரும்பு -1;

    cmd = buffer;
    போது (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	போது (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	எனில் (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	போது (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    திரும்பு cnt;
}

#எனில் defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    நிலையான முழு
pstrcmp(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    திரும்பு (pathcmp(*(எழுத்து **)a, *(எழுத்து **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * Return the number of matches found.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path[wildoff]".
 * Return the number of matches found.
 * NOTE: much of this is identical to unix_expandpath(), keep in sync!
 */
    முழு
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    முழு		flags,		// EW_* flags
    முழு		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    முழு		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    முழு		starts_with_dot;
    முழு		matches;
    size_t	buflen;
    size_t	len;
    முழு		starstar = FALSE;
    நிலையான முழு	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_மதிப்பு;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = வெற்று;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    முழு			ok;
    char_u		*p_alt;
    முழு			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    எனில் (stardepth > 0)
    {
	ui_breakcheck();
	எனில் (got_int)
	    திரும்பு 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    எனில் (buf == வெற்று)
	திரும்பு 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = வெற்று;
    path_end = path;
    போது (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	எனில் (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	இல்லையெனில் எனில் (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    எனில் (e != வெற்று)
		நிறுத்து;
	    s = p + 1;
	}
	இல்லையெனில் எனில் (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != வெற்று)
	    e = p;
	எனில் (has_mbyte)
	{
	    முழு charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	இல்லையெனில்
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    க்கு (p = buf + wildoff; p < s; ++p)
	எனில் (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    க்கு (p = s; p < e; ++p)
	எனில் (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, வெற்று, FALSE);
    எனில் (pat == வெற்று)
    {
	vim_free(buf);
	திரும்பு 0;
    }

    // compile the regexp into a program
    எனில் (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    எனில் (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    எனில் (regmatch.regprog == வெற்று && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	திரும்பு 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != வெற்று;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    எனில் (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((எழுத்து *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(வெறுமை)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((எழுத்து *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, வெற்று);
    எனில் (wn != வெற்று)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_மதிப்பு);

    போது (ok)
    {
	p = utf16_to_enc(wfb.cFileName, வெற்று);   // p is allocated here

	எனில் (p == வெற்று)
	    நிறுத்து;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	எனில் (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = வெற்று;
	இல்லையெனில்
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, வெற்று);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	எனில் ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == வெற்று
		  || (regmatch.regprog != வெற்று
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != வெற்று
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((எழுத்து *)s, buflen - len, "%s", p);
	    எனில் (len + 1 < buflen)
	    {
		எனில் (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((எழுத்து *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (வெறுமை)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((எழுத்து *)buf + len, buflen - len, "%s", path_end);
		எனில் (mch_has_exp_wildcard(path_end))
		{
		    எனில் (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(வெறுமை)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		இல்லையெனில்
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    எனில் (*path_end != 0)
			backslash_halve(buf + len + 1);
		    // addfile() checks existence; do not stat the same path twice.
		    // Even with EW_NOTFOUND, a literal suffix must exist.
		    addfile(gap, buf, flags & ~EW_NOTFOUND);
		}
	    }
	}

	vim_free(p_alt);
	vim_free(p);
	ok = FindNextFileW(hFind, &wfb);
    }

    FindClose(hFind);
    vim_free(wn);
    vim_free(buf);
    vim_regfree(regmatch.regprog);
    vim_free(matchname);

    matches = gap->ga_len - start_len;
    எனில் (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   அளவு(char_u *), pstrcmp);
    திரும்பு matches;
}
#endif // MSWIN

#எனில் (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    நிலையான முழு
pstrcmp(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    திரும்பு (pathcmp(*(எழுத்து **)a, *(எழுத்து **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    முழு
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    முழு		flags,		// EW_* flags
    முழு		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    முழு		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    முழு		starts_with_dot;
    முழு		matches;
    size_t	buflen;
    size_t	len;
    முழு		starstar = FALSE;
    நிலையான முழு	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    கட்டமைப்பு dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    எனில் (stardepth > 0)
    {
	ui_breakcheck();
	எனில் (got_int)
	    திரும்பு 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    எனில் (buf == வெற்று)
	திரும்பு 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = வெற்று;
    path_end = path;
    போது (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	எனில் (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	இல்லையெனில் எனில் (*path_end == '/')
	{
	    எனில் (e != வெற்று)
		நிறுத்து;
	    s = p + 1;
	}
	இல்லையெனில் எனில் (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != வெற்று
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	எனில் (has_mbyte)
	{
	    முழு charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	இல்லையெனில்
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    க்கு (p = buf + wildoff; p < s; ++p)
	எனில் (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    க்கு (p = s; p < e; ++p)
	எனில் (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, வெற்று, FALSE);
    எனில் (pat == வெற்று)
    {
	vim_free(buf);
	திரும்பு 0;
    }

    // compile the regexp into a program
    எனில் (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    இல்லையெனில்
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    எனில் (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    எனில் (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    எனில் (regmatch.regprog == வெற்று && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	திரும்பு 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    எனில் (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((எழுத்து *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(வெறுமை)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (எழுத்து *)buf);

    // Find all matching entries
    எனில் (dirp != வெற்று)
    {
	போது (!got_int)
	{
	    dp = readdir(dirp);
	    எனில் (dp == வெற்று)
		நிறுத்து;
	    len = (size_t)(s - buf);
	    எனில் ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != வெற்று && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((எழுத்து *)s, buflen - len, "%s", dp->d_name);
		எனில் (len + 1 >= buflen)
		    தொடர்;

		எனில் (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((எழுத்து *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (வெறுமை)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((எழுத்து *)buf + len, buflen - len, "%s", path_end);
		எனில் (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    எனில் (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(வெறுமை)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		இல்லையெனில்
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    எனில் (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    எனில் ((flags & EW_ALLLINKS) ? mch_lstat((எழுத்து *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			எனில் (precomp_buf)
			{
			    mch_memmove(buf, precomp_buf, precomp_len);
			    vim_free(precomp_buf);
			}
# endif
			addfile(gap, buf, flags);
		    }
		}
	    }
	}

	closedir(dirp);
    }

    vim_free(buf);
    vim_regfree(regmatch.regprog);

    // When interrupted the matches probably won't be used and sorting can be
    // slow, thus skip it.
    matches = gap->ga_len - start_len;
    எனில் (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   அளவு(char_u *), pstrcmp);
    திரும்பு matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    நிலையான முழு
has_env_var(char_u *p)
{
    க்கு ( ; *p; MB_PTR_ADV(p))
    {
	எனில் (*p == '\\' && p[1] != NUL)
	    ++p;
	இல்லையெனில் எனில் (vim_strchr((char_u *)
#எனில் defined(MSWIN)
				    "$%"
#இல்லையெனில்
				    "$"
#endif
					, *p) != வெற்று)
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    நிலையான முழு
has_special_wildchar(char_u *p)
{
    க்கு ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	எனில் (*p == '\r' || *p == '\n')
	    நிறுத்து;
	// Allow for escaping.
	எனில் (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	இல்லையெனில் எனில் (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != வெற்று)
	{
	    // A { must be followed by a matching }.
	    எனில் (*p == '{' && vim_strchr(p, '}') == வெற்று)
		தொடர்;
	    // A quote and backtick must be followed by another one.
	    எனில் ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == வெற்று)
		தொடர்;
	    திரும்பு TRUE;
	}
    }
    திரும்பு FALSE;
}
#endif

/*
 * Generic wildcard expansion code.
 *
 * Characters in "pat" that should not be expanded must be preceded with a
 * backslash. E.g., "/path\ with\ spaces/my\*star*"
 *
 * Return FAIL when no single file was found.  In this case "num_file" is not
 * set, and "file" may contain an error message.
 * Return OK when some files found.  "num_file" is set to the number of
 * matches, "file" to the array of matches.  Call FreeWild() later.
 */
    முழு
gen_expand_wildcards(
    முழு		இலக்கம்_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    முழு		*இலக்கம்_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    முழு		flags)		// EW_* flags
{
    முழு			i;
    garray_T		ga;
    char_u		*p;
    நிலையான முழு		recursive = FALSE;
    முழு			add_pat;
    முழு			retval = OK;
    முழு			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    எனில் (recursive)
#ifdef SPECIAL_WILDCHAR
	திரும்பு mch_expand_wildcards(இலக்கம்_pat, pat, இலக்கம்_file, file, flags);
#இல்லையெனில்
	திரும்பு FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    க்கு (i = 0; i < இலக்கம்_pat; i++)
    {
	எனில் (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    திரும்பு mch_expand_wildcards(இலக்கம்_pat, pat, இலக்கம்_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, அளவு(char_u *), 30);

    க்கு (i = 0; i < இலக்கம்_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	எனில் (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    எனில் (add_pat == -1)
		retval = FAIL;
	}
	இல்லையெனில்
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    எனில் ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		எனில் (p == வெற்று)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		இல்லையெனில் எனில் (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(இலக்கம்_pat, pat, இலக்கம்_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    திரும்பு i;
		}
#endif
	    }

	    /*
	     * If there are wildcards or case-insensitive expansion is
	     * required: Expand file names and add each match to the list.  If
	     * there is no match, and EW_NOTFOUND is given, add the pattern.
	     * Otherwise: Add the file name if it exists or when EW_NOTFOUND is
	     * given.
	     */
	    எனில் (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		எனில் ((flags & (EW_PATH | EW_CDPATH))
			&& !mch_isFullName(p)
			&& !(p[0] == '.'
			    && (vim_ispathsep(p[1])
				|| (p[1] == '.' && vim_ispathsep(p[2]))))
		   )
		{
		    // :find completion where 'path' is used.
		    // Recursiveness is OK here.
		    recursive = FALSE;
		    add_pat = expand_in_path(&ga, p, flags);
		    recursive = TRUE;
		    did_expand_in_path = TRUE;
		}
		இல்லையெனில்
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	எனில் (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    எனில் (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    இல்லையெனில்
		addfile(&ga, t, flags);

	    எனில் (t != p)
		vim_free(t);
	}

	எனில் (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	எனில் (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    எனில் (retval == FAIL)
	ga_clear_strings(&ga);

    *இலக்கம்_file = ga.ga_len;
    *file = (ga.ga_data != வெற்று) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    திரும்பு ((flags & EW_EMPTYOK) || ga.ga_data != வெற்று) ? retval : FAIL;
}

/*
 * Add a file to a file list.  Accepted flags:
 * EW_DIR	add directories
 * EW_FILE	add files
 * EW_EXEC	add executable files
 * EW_NOTFOUND	add even when it doesn't exist
 * EW_ADDSLASH	add slash after directory name
 * EW_ALLLINKS	add symlink also when the referred file does not exist
 */
    வெறுமை
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    முழு		flags)
{
    char_u	*p;
    முழு		isdir;
    stat_T	sb;

#ifdef MSWIN
    எனில் (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	எனில் (perm < 0)
	    திரும்பு;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    இல்லையெனில்
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	எனில் (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((எழுத்து *)f, &sb) < 0 : mch_getperm(f) < 0))
	    திரும்பு;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    எனில் (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != வெற்று)
	திரும்பு;
#endif

    எனில் ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	திரும்பு;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    எனில் (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, வெற்று, !(flags & EW_SHELLCMD)))
	திரும்பு;

    // Make room for another item in the file list.
    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு;

    p = alloc(STRLEN(f) + 1 + isdir);
    எனில் (p == வெற்று)
	திரும்பு;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    எனில் (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    வெறுமை
FreeWild(முழு மொத்தம், char_u **files)
{
    எனில் (மொத்தம் <= 0 || files == வெற்று)
	திரும்பு;
    போது (மொத்தம்--)
	vim_free(files[மொத்தம்]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    முழு
pathcmp(மாறிலி எழுத்து *p, மாறிலி எழுத்து *q, முழு maxlen)
{
    முழு		i, j;
    முழு		c1, c2;
    மாறிலி எழுத்து	*s = வெற்று;

    க்கு (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	எனில் (c1 == NUL)
	{
	    எனில் (c2 == NUL)  // full match
		திரும்பு 0;
	    s = q;
	    i = j;
	    நிறுத்து;
	}

	// End of "q": check if "p" just has a slash.
	எனில் (c2 == NUL)
	{
	    s = p;
	    நிறுத்து;
	}

	எனில் ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    எனில் (vim_ispathsep(c1))
		திரும்பு -1;
	    எனில் (vim_ispathsep(c2))
		திரும்பு 1;
	    திரும்பு p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    எனில் (s == வெற்று)	// "i" or "j" ran into "maxlen"
	திரும்பு 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    எனில் (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#இல்லையெனில்
	    && c1 == '/'
#endif
       )
	திரும்பு 0;   // match with trailing slash
    எனில் (s == q)
	திரும்பு -1;	    // no match
    திரும்பு 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    முழு
vim_isAbsName(char_u *name)
{
    திரும்பு (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    முழு		len,
    முழு		force)	    // force expansion even when already absolute
{
    முழு		retval = OK;
    முழு		url;

    *buf = NUL;
    எனில் (fname == வெற்று)
	திரும்பு FAIL;

    url = path_with_url(fname);
    எனில் (!url)
	retval = mch_FullName(fname, buf, len, force);
    எனில் (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#எனில் defined(MSWIN)
    slash_adjust(buf);
#endif
    திரும்பு retval;
}
