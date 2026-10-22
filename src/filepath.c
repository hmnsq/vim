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

#અન્તર્ભાવ "vim.h"

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
    સ્થિર પૂર્ણાંક
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    પૂર્ણાંક		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * કદ(*newbuf));
    જો (newbuf == ખાલી)
	પાછા FAIL;

    wfname = enc_to_utf16(*fnamep, ખાલી);
    જો (wfname == ખાલી)
    {
	vim_free(newbuf);
	પાછા FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    જો (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * કદ(*newbuf));
	જો (newbuf == ખાલી)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    પાછા FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    જો (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, ખાલી);

	જો (p != ખાલી)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	નહીંતર
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    પાછા FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    પાછા OK;
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
    સ્થિર પૂર્ણાંક
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = ખાલી;
    char_u	*short_fname = ખાલી;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    પૂર્ણાંક		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    જો (save_fname == ખાલી)
    {
	retval = FAIL;
	જાઓ theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    માટે (;;)
    {
	// go back one path-separator
	જ્યારે (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	જો (endp <= save_fname)
	    તોડો;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	જો (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    જાઓ theend;
	}
	*endp = ch;	// preserve the string

	જો (len > 0)
	    તોડો;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    જો (len > 0)
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
	જો (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    જો (*fname == ખાલી)
	    {
		retval = FAIL;
		જાઓ theend;
	    }
	}
	નહીંતર
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    જો (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = ખાલી;
	}

	// concat the not-shortened part of the path
	જો ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    પાછા retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    સ્થિર પૂર્ણાંક
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    પૂર્ણાંક		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    પૂર્ણાંક		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    માટે (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	જો (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    જો (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    નહીંતર
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    જો (tfname == ખાલી)
	પાછા FAIL;

    len = tflen = STRLEN(tfname);

    જો (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	પાછા FAIL;

    જો (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	જો (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    પાછા FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    માટે (p = tfname + len - 1; p >= tfname; --p)
    {
	જો (has_mbyte)
	    p -= mb_head_off(tfname, p);
	જો (vim_ispathsep(*p))
	{
	    જો (sepcount == 0 || (hasTilde && sepcount == 1))
		તોડો;
	    નહીંતર
		sepcount --;
	}
    }
    જો (hasTilde)
    {
	--p;
	જો (p >= tfname)
	    *p = '~';
	નહીંતર
	    પાછા FAIL;
    }
    નહીંતર
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    પાછા OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    પૂર્ણાંક
modify_fname(
    char_u	*src,		// string with modifiers
    પૂર્ણાંક		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    પૂર્ણાંક		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    પૂર્ણાંક		c;
    પૂર્ણાંક		has_fullname = 0;
    પૂર્ણાંક		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    પૂર્ણાંક		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    જો (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	જો ((*fnamep)[0] == '~'
#જો !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    જો (*fnamep == ખાલી)
		પાછા -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	માટે (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    જો (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		તોડો;
	}

	// FullName_save() is slow, don't use it when not needed.
	જો (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    જો (*fnamep == ખાલી)
		પાછા -1;
	}

#ifdef MSWIN
# જો _WIN32_WINNT >= 0x0500
	જો (vim_strchr(*fnamep, '~') != ખાલી)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, ખાલી);
	    WCHAR buf[_MAX_PATH];

	    જો (wfname != ખાલી)
	    {
		જો (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, ખાલી);

		    જો (q != ખાલી)
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
	જો (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    જો (*fnamep == ખાલી)
		પાછા -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    જ્યારે (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	જો (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    ચાલુ;
	}
	pbuf = ખાલી;
	// Need full path first (use expand_env() to remove a "~/")
	જો (!has_fullname && !has_homerelative)
	{
	    જો (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    નહીંતર
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	નહીંતર
	    p = *fnamep;

	has_fullname = 0;

	જો (p != ખાલી)
	{
	    size_t  dirnamelen = 0;

	    જો (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		જો (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    જો (s != ખાલી)
		    {
			dirnamelen = home_replace(ખાલી, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		જો (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		જો (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    જો (vim_ispathsep(*p))
		    {
			જ્યારે (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			જો (pbuf != ખાલી)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = ખાલી;
			}
		    }
		}
	    }
	    નહીંતર
	    {
		dirnamelen = home_replace(ખાલી, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		જો (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    જો (s != ખાલી)
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
    જ્યારે (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	જ્યારે (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	જો (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	જો (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    જો (p == ખાલી)
		પાછા -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	નહીંતર
	{
	    જ્યારે (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    જો (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    જો (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	જો (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    જો (p == ખાલી)
		પાછા -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	જો (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    જો (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		પાછા -1;
	}
	નહીંતર
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    જો (get_short_pathname(fnamep, bufp, &l) == FAIL)
		પાછા -1;

	    જો (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		જો (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    પાછા -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    જો (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    જ્યારે (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	જો (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	નહીંતર
	    s = *fnamep + *fnamelen - 1;
	માટે ( ; s > tail; --s)
	    જો (s[0] == '.')
		તોડો;
	જો (src[*usedlen + 1] == 'e')		// :e
	{
	    જો (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		માટે ( ; s > *fnamep; --s)
		    જો (s[0] == ';')
			તોડો;
		જો (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    નહીંતર જો (*fnamep <= tail)
		*fnamelen = 0;
	}
	નહીંતર				// :r
	{
	    char_u *limit = *fnamep;

	    જો (limit < tail)
		limit = tail;
	    જો (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    જો (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	પૂર્ણાંક	    sep;
	char_u	    *flags;
	પૂર્ણાંક	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	જો (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	જો (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    જો (p != ખાલી)
	    {
		pat = vim_strnsave(s, p - s);
		જો (pat != ખાલી)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    જો (p != ખાલી)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			જો (sub != ખાલી && str != ખાલી)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, ખાલી, flags, &slen);
			    જો (s != ખાલી)
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
	    જો (didit)
		જાઓ repeat;
	}
    }

    જો (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	જો (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	જો (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	જો (p == ખાલી)
	    પાછા -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    પાછા valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    સ્થિર શૂન્ય
shorten_dir_len(char_u *str, પૂર્ણાંક trim_len)
{
    char_u	*tail, *s, *d;
    પૂર્ણાંક		skip = FALSE;
    પૂર્ણાંક		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    માટે (s = str; ; ++s)
    {
	જો (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    જો (*s == NUL)
		તોડો;
	}
	નહીંતર જો (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	નહીંતર જો (!skip)
	{
	    *d++ = *s;			// copy next char
	    જો (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		જો (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    જો (has_mbyte)
	    {
		પૂર્ણાંક l = mb_ptr2len(s);

		જ્યારે (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    શૂન્ય
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    પૂર્ણાંક
file_is_readable(char_u *fname)
{
    પૂર્ણાંક		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    જો (*fname == NUL || mch_isdir_or_missing(fname))
	પાછા FALSE;
    જો ((fd = mch_open((અક્ષર *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	પાછા TRUE;
    }
    પાછા FALSE;
}

#જો defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    શૂન્ય
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (check_secure())
	પાછા;

    જો (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	જો (in_vim9script())
	    (શૂન્ય) check_for_string_arg(argvars, 0);
	પાછા;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    જો (cwd != ખાલી)
    {
	જો (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	જો (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	નહીંતર જો (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	નહીંતર જો (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	નહીંતર
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str_str), "scope", s);
	    પાછા;
	}
    }
    નહીંતર જો (curwin->w_localdir != ખાલી)
	scope = CDSCOPE_WINDOW;
    નહીંતર જો (curtab->tp_localdir != ખાલી)
	scope = CDSCOPE_TABPAGE;

    જો (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    શૂન્ય
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_સંખ્યા = -1;
    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    name = tv_get_string(&argvars[0]);
    જો (name == ખાલી || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	પાછા;
    }

    જો (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    નહીંતર
	flags = (char_u *)"";

    જો (*flags == NUL)
	// delete a file
	rettv->vval.v_સંખ્યા = mch_remove(name) == 0 ? 0 : -1;
    નહીંતર જો (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_સંખ્યા = mch_rmdir(name) == 0 ? 0 : -1;
    નહીંતર જો (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_સંખ્યા = delete_recursive(name);
    નહીંતર
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    શૂન્ય
f_executable(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_સંખ્યા = mch_can_exe(tv_get_string(&argvars[0]), ખાલી, TRUE);
}

/*
 * "exepath()" function
 */
    શૂન્ય
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = ખાલી;

    જો (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	પાછા;
    (શૂન્ય)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    શૂન્ય
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;
    rettv->vval.v_સંખ્યા = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    શૂન્ય
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;
    rettv->vval.v_સંખ્યા = filewritable(tv_get_string(&argvars[0]));
}

    સ્થિર શૂન્ય
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    પૂર્ણાંક		find_what)
{
    char_u	*fname;
    char_u	*fresult = ખાલી;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    પૂર્ણાંક		ગણતરી = 1;
    પૂર્ણાંક		first = TRUE;
    પૂર્ણાંક		error = FALSE;

    rettv->vval.v_string = ખાલી;
    rettv->v_type = VAR_STRING;
    જો (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL)))
	પાછા;

    fname = tv_get_string(&argvars[0]);

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	જો (p == ખાલી)
	    error = TRUE;
	નહીંતર
	{
	    જો (*p != NUL)
		path = p;

	    જો (argvars[2].v_type != VAR_UNKNOWN)
		ગણતરી = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[2], &error);
	}
    }

    જો (ગણતરી < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    જો (*fname != NUL && !error)
    {
	char_u	*file_to_find = ખાલી;
	અક્ષર	*શોધ_ctx = ખાલી;

	કરો
	{
	    જો (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : ખાલી,
					       first ? (પૂર્ણાંક)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &શોધ_ctx);
	    first = FALSE;

	    જો (fresult != ખાલી && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} જ્યારે ((rettv->v_type == VAR_LIST || --ગણતરી > 0) && fresult != ખાલી);

	vim_free(file_to_find);
	vim_findfile_cleanup(શોધ_ctx);
    }

    જો (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    શૂન્ય
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    શૂન્ય
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    શૂન્ય
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = ખાલી;
    char_u	buf[NUMBUFLEN];

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    જો (mods == ખાલી || fname == ખાલી)
	fname = ખાલી;
    નહીંતર
    {
	len = STRLEN(fname);
	જો (mods != ખાલી && *mods != NUL)
	    (શૂન્ય)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    જો (fname == ખાલી)
	rettv->vval.v_string = ખાલી;
    નહીંતર
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
    શૂન્ય
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = ખાલી;
    tabpage_T	*tp = ખાલી;
    char_u	*cwd;
    પૂર્ણાંક		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script()
	    && (check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL)))
	પાછા;

    જો (argvars[0].v_type == VAR_સંખ્યા
	    && argvars[0].vval.v_સંખ્યા == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    નહીંતર
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    જો (wp != ખાલી && wp->w_localdir != ખાલી
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    નહીંતર જો (tp != ખાલી && tp->tp_localdir != ખાલી
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    નહીંતર જો (wp != ખાલી || tp != ખાલી || global)
    {
	જો (globaldir != ખાલી && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	નહીંતર
	{
	    cwd = alloc(MAXPATHL);
	    જો (cwd != ખાલી)
	    {
		જો (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    જો (rettv->vval.v_string != ખાલી)
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
    પૂર્ણાંક		    i;

    માટે (i = 0; i < 9; i++)
    {
	જો (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	નહીંતર
	    perm[i] = '-';
    }
    પાછા perm;
}

/*
 * "getfperm({fname})" function
 */
    શૂન્ય
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = ખાલી;
    char_u	permbuf[] = "---------";

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    જો (mch_stat((અક્ષર *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    શૂન્ય
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    fname = tv_get_string(&argvars[0]);
    જો (mch_stat((અક્ષર *)fname, &st) >= 0)
    {
	જો (S_ISDIR(st.st_mode))
	    rettv->vval.v_સંખ્યા = 0;
	નહીંતર
	{
	    rettv->vval.v_સંખ્યા = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    જો ((off_T)rettv->vval.v_સંખ્યા != (off_T)st.st_size)
		rettv->vval.v_સંખ્યા = -2;
	}
    }
    નહીંતર
	  rettv->vval.v_સંખ્યા = -1;
}

/*
 * "getftime({fname})" function
 */
    શૂન્ય
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    fname = tv_get_string(&argvars[0]);
    જો (mch_stat((અક્ષર *)fname, &st) >= 0)
	rettv->vval.v_સંખ્યા = (varnumber_T)st.st_mtime;
    નહીંતર
	rettv->vval.v_સંખ્યા = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    અક્ષર    *t;

    જો (S_ISREG(st->st_mode))
	t = "file";
    નહીંતર જો (S_ISDIR(st->st_mode))
	t = "dir";
    નહીંતર જો (S_ISLNK(st->st_mode))
	t = "link";
    નહીંતર જો (S_ISBLK(st->st_mode))
	t = "bdev";
    નહીંતર જો (S_ISCHR(st->st_mode))
	t = "cdev";
    નહીંતર જો (S_ISFIFO(st->st_mode))
	t = "fifo";
    નહીંતર જો (S_ISSOCK(st->st_mode))
	t = "socket";
    નહીંતર
	t = "other";
    પાછા (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    શૂન્ય
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = ખાલી;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    જો (mch_lstat((અક્ષર *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    શૂન્ય
f_glob(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    પૂર્ણાંક		error = FALSE;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	પાછા;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	જો (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    જો (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, ખાલી);
	    જો (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    જો (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	જો (p_wic)
	    options += WILD_ICASE;
	જો (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     ખાલી, options, WILD_ALL);
	નહીંતર જો (rettv_list_alloc(rettv) == OK)
	{
	  પૂર્ણાંક i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						ખાલી, options, WILD_ALL_KEEP);
	  માટે (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    નહીંતર
	rettv->vval.v_string = ખાલી;
}

/*
 * "glob2regpat()" function
 */
    શૂન્ય
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == ખાલી)
			 ? ખાલી : file_pat_to_reg_pat(pat, ખાલી, ખાલી, FALSE);
}

/*
 * "globpath()" function
 */
    શૂન્ય
f_globpath(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    પૂર્ણાંક		error = FALSE;
    garray_T	ga;
    પૂર્ણાંક		i;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	પાછા;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	જો (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	જો (argvars[3].v_type != VAR_UNKNOWN)
	{
	    જો (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, ખાલી);
	    જો (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    જો (file != ખાલી && !error)
    {
	ga_init2(&ga, કદ(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	જો (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	નહીંતર જો (rettv_list_alloc(rettv) == OK)
	    માટે (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    નહીંતર
	rettv->vval.v_string = ખાલી;
}

/*
 * "isdirectory()" function
 */
    શૂન્ય
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    શૂન્ય
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
mkdir_recurse(char_u *dir, પૂર્ણાંક prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    પૂર્ણાંક		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    જો (p <= get_past_head(dir))
	પાછા OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    જો (updir == ખાલી)
	પાછા FAIL;
    જો (mch_isdir(updir))
	r = OK;
    નહીંતર જો (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	જો (r == OK && created != ખાલી && *created == ખાલી)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    પાછા r;
}

/*
 * "mkdir()" function
 */
    શૂન્ય
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    પૂર્ણાંક		prot = 0755;
    પૂર્ણાંક		defer = FALSE;
    પૂર્ણાંક		defer_recurse = FALSE;
    char_u	*created = ખાલી;

    rettv->vval.v_સંખ્યા = FAIL;
    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL)))
	પાછા;

    dir = tv_get_string_buf(&argvars[0], buf);
    જો (*dir == NUL)
	પાછા;

    જો (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[2], ખાલી);
	    જો (prot == -1)
		પાછા;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != ખાલી;
	defer_recurse = vim_strchr(arg2, 'R') != ખાલી;
	જો ((defer || defer_recurse) && !can_add_defer())
	    પાછા;

	જો (vim_strchr(arg2, 'p') != ખાલી)
	{
	    જો (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_સંખ્યા = OK;
		પાછા;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : ખાલી);
	}
    }
    rettv->vval.v_સંખ્યા = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    જો (rettv->vval.v_સંખ્યા == OK
				&& created == ખાલી && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    જો (created != ખાલી)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	જો (tv[0].vval.v_string == ખાલી || tv[1].vval.v_string == ખાલી
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
    શૂન્ય
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    પૂર્ણાંક		trim_len = 1;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[1]);
	જો (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    જો (p == ખાલી)
	rettv->vval.v_string = ખાલી;
    નહીંતર
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	જો (p != ખાલી)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    સ્થિર પૂર્ણાંક
checkitem_common(શૂન્ય *context, char_u *name, dict_T *dict)
{
    typval_T	*પદાવલિ = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    પૂર્ણાંક		retval = 0;
    પૂર્ણાંક		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    જો (name != ખાલી)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    નહીંતર
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    જો (eval_પદાવલિ_typval(પદાવલિ, FALSE, argv, 1, ખાલી, &rettv) == FAIL)
	જાઓ theend;

    // We want to use -1, but also true/false should be allowed.
    જો (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_સંખ્યા;
	rettv.vval.v_સંખ્યા = rettv.vval.v_સંખ્યા == VVAL_TRUE;
    }
    retval = tv_get_સંખ્યા_chk(&rettv, &error);
    જો (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    જો (name != ખાલી)
	set_vim_var_string(VV_VAL, ખાલી, 0);
    નહીંતર
	set_vim_var_dict(VV_VAL, ખાલી);
    restore_vimvar(VV_VAL, &save_val);
    પાછા retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    સ્થિર પૂર્ણાંક
readdir_checkitem(શૂન્ય *context, શૂન્ય *item)
{
    char_u	*name = (char_u *)item;

    પાછા checkitem_common(context, name, ખાલી);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    સ્થિર પૂર્ણાંક
readdirex_dict_arg(typval_T *argvars, પૂર્ણાંક *cmp)
{
    char_u     *compare;

    જો (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	પાછા FAIL;

    જો (dict_has_ચાવી(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    નહીંતર
    {
	semsg(_(e_dictionary_ચાવી_str_required), "sort");
	પાછા FAIL;
    }

    જો (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    નહીંતર જો (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    નહીંતર જો (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    નહીંતર જો (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    પાછા OK;
}

/*
 * "readdir()" function
 */
    શૂન્ય
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*પદાવલિ;
    પૂર્ણાંક		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    પૂર્ણાંક		i;
    પૂર્ણાંક		sort = READDIR_SORT_BYTE;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;
    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	પાછા;

    path = tv_get_string(&argvars[0]);
    પદાવલિ = &argvars[1];

    જો (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	પાછા;

    ret = readdir_core(&ga, path, FALSE, (શૂન્ય *)પદાવલિ,
	    (પદાવલિ->v_type == VAR_UNKNOWN) ? ખાલી : readdir_checkitem, sort);
    જો (ret == OK)
    {
	માટે (i = 0; i < ga.ga_len; i++)
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
    સ્થિર પૂર્ણાંક
readdirex_checkitem(શૂન્ય *context, શૂન્ય *item)
{
    dict_T	*dict = (dict_T*)item;

    પાછા checkitem_common(context, ખાલી, dict);
}

/*
 * "readdirex()" function
 */
    શૂન્ય
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*પદાવલિ;
    પૂર્ણાંક		ret;
    char_u	*path;
    garray_T	ga;
    પૂર્ણાંક		i;
    પૂર્ણાંક		sort = READDIR_SORT_BYTE;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;
    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	પાછા;

    path = tv_get_string(&argvars[0]);
    પદાવલિ = &argvars[1];

    જો (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	પાછા;

    ret = readdir_core(&ga, path, TRUE, (શૂન્ય *)પદાવલિ,
	    (પદાવલિ->v_type == VAR_UNKNOWN) ? ખાલી : readdirex_checkitem, sort);
    જો (ret == OK)
    {
	માટે (i = 0; i < ga.ga_len; i++)
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
    સ્થિર શૂન્ય
read_file_or_blob(typval_T *argvars, typval_T *rettv, પૂર્ણાંક always_blob)
{
    પૂર્ણાંક		દ્વિઅંકી = FALSE;
    પૂર્ણાંક		blob = always_blob;
    પૂર્ણાંક		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    પૂર્ણાંક		io_size = કદ(buf);
    પૂર્ણાંક		readlen;		// size of last fread()
    char_u	*prev	 = ખાલી;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	જો (always_blob)
	{
	    offset = (off_T)tv_get_સંખ્યા(&argvars[1]);
	    જો (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_સંખ્યા(&argvars[2]);
	}
	નહીંતર
	{
	    જો (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		દ્વિઅંકી = TRUE;
	    જો (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    જો (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_સંખ્યા(&argvars[2]);
	}
    }

    જો ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	પાછા;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    જો (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	પાછા;
    }
    જો (*fname == NUL || (fd = mch_fopen((અક્ષર *)fname, READBIN)) == ખાલી)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	પાછા;
    }

    જો (blob)
    {
	જો (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	પાછા;
    }

    જ્યારે (cnt < maxline || maxline < 0)
    {
	readlen = (પૂર્ણાંક)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	માટે (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || દ્વિઅંકી));
		++p)
	{
	    જો (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= ખાલી;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		જો (readlen > 0 && !દ્વિઅંકી)
		{
		    જ્યારે (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    જો (len == 0)
			જ્યારે (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		જો (prevlen == 0)
		    s = vim_strnsave(start, len);
		નહીંતર
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    જો ((s = vim_realloc(prev, prevlen + len + 1)) != ખાલી)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = ખાલી; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		જો (s == ખાલી)
		{
		    do_outofmem_સંદેશ((long_u) prevlen + len + 1);
		    failed = TRUE;
		    તોડો;
		}

		જો ((li = listitem_alloc()) == ખાલી)
		{
		    vim_free(s);
		    failed = TRUE;
		    તોડો;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		જો ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    તોડો;
	    }
	    નહીંતર જો (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    નહીંતર જો (*p == 0xbf && enc_utf8 && !દ્વિઅંકી)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		જો (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    જો (start == dest)
			start = p + 1;
		    નહીંતર
		    {
			// have to shuffle buf to close gap
			પૂર્ણાંક adjust_prevlen = 0;

			જો (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (પૂર્ણાંક)(buf - dest);
			    dest = buf;
			}
			જો (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	જો (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    તોડો;
	જો (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    જો (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		જો (prevsize == 0)
		    prevsize = (long)(p - start);
		નહીંતર
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		જો (newprev == ખાલી)
		{
		    do_outofmem_સંદેશ((long_u)prevsize);
		    failed = TRUE;
		    તોડો;
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
    જો (!failed && maxline < 0)
	જ્યારે (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    જો (failed)
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
    શૂન્ય
f_readblob(typval_T *argvars, typval_T *rettv)
{
    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL)))
	પાછા;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    શૂન્ય
f_readfile(typval_T *argvars, typval_T *rettv)
{
    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL)))
	પાછા;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    શૂન્ય
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = ખાલી;
# endif

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = ખાલી;

	v = mch_resolve_path(p, TRUE);
	જો (v != ખાલી)
	    rettv->vval.v_string = v;
	નહીંતર
	    rettv->vval.v_string = vim_strsave(p);
    }
# નહીંતર
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	પૂર્ણાંક	len;
	char_u	*remain = ખાલી;
	char_u	*q;
	પૂર્ણાંક	is_relative_to_current = FALSE;
	પૂર્ણાંક	has_trailing_pathsep = FALSE;
	પૂર્ણાંક	limit = 100;

	p = vim_strsave(p);
	જો (p == ખાલી)
	    જાઓ fail;
	જો (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	જો (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	જો (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	જો (buf == ખાલી)
	{
	    vim_free(p);
	    vim_free(remain);
	    જાઓ fail;
	}

	માટે (;;)
	{
	    માટે (;;)
	    {
		len = readlink((અક્ષર *)p, (અક્ષર *)buf, MAXPATHL);
		જો (len <= 0)
		    તોડો;
		buf[len] = NUL;

		જો (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = ખાલી;
		    જાઓ fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		જો (remain == ખાલી && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		જો (*q != NUL)
		{
		    જો (remain == ખાલી)
			remain = vim_strsave(q - 1);
		    નહીંતર
		    {
			cpy = concat_str(q - 1, remain);
			જો (cpy != ખાલી)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		જો (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		જો (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    જો (cpy != ખાલી)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		નહીંતર
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    જો (remain == ખાલી)
		તોડો;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    જો (cpy != ખાલી)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    જો (*q != NUL)
		STRMOVE(remain, q - 1);
	    નહીંતર
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	જો (!vim_ispathsep(*p))
	{
	    જો (is_relative_to_current
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
		જો (cpy != ખાલી)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    નહીંતર જો (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		જ્યારે (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		જો (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	જો (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    જો (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  નહીંતર
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
    શૂન્ય
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    સ્થિર પૂર્ણાંક	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    કરો
    {
	જો (x == 'Z')
	    x = '0';
	નહીંતર જો (x == '9')
	    x = 'A';
	નહીંતર
	    ++x;
    } જ્યારે (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    શૂન્ય
f_writefile(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		દ્વિઅંકી = FALSE;
    પૂર્ણાંક		append = FALSE;
    પૂર્ણાંક		defer = FALSE;
# ifdef HAVE_FSYNC
    પૂર્ણાંક		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    પૂર્ણાંક		ret = 0;
    listitem_T	*li;
    list_T	*list = ખાલી;
    blob_T	*blob = ખાલી;

    rettv->vval.v_સંખ્યા = -1;
    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	પાછા;

    જો (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	જો (list == ખાલી)
	    પાછા;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_વસ્તુઓ(list, li)
	    જો (tv_get_string_chk(&li->li_tv) == ખાલી)
		પાછા;
    }
    નહીંતર જો (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	જો (blob == ખાલી)
	    પાછા;
    }
    નહીંતર
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	પાછા;
    }

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	જો (arg2 == ખાલી)
	    પાછા;
	જો (vim_strchr(arg2, 'b') != ખાલી)
	    દ્વિઅંકી = TRUE;
	જો (vim_strchr(arg2, 'a') != ખાલી)
	    append = TRUE;
	જો (vim_strchr(arg2, 'D') != ખાલી)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	જો (vim_strchr(arg2, 's') != ખાલી)
	    do_fsync = TRUE;
	નહીંતર જો (vim_strchr(arg2, 'S') != ખાલી)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    જો (fname == ખાલી)
	પાછા;

    જો (defer && !can_add_defer())
	પાછા;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    જો (*fname == NUL || (fd = mch_fopen((અક્ષર *)fname,
				      append ? APPENDBIN : WRITEBIN)) == ખાલી)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    નહીંતર
    {
	જો (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    જો (tv.vval.v_string == ખાલી
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(શૂન્ય)mch_remove(fname);
	    }
	}

	જો (ret == 0)
	{
	    જો (blob)
	    {
		જો (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    નહીંતર
	    {
		જો (write_list(fd, list, દ્વિઅંકી) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    જો (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_સંખ્યા = ret;
}

#endif // FEAT_EVAL

#જો defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    પૂર્ણાંક		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    સ્થિર char_u	*last_dir = ખાલી;    // last used directory
    char_u		*tofree = ખાલી;
    પૂર્ણાંક			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    જો (title == ખાલી || *title == NUL)
    {
	જો (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	નહીંતર જો (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	નહીંતર
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    જો ((initdir == ખાલી || *initdir == NUL) && dflt != ખાલી && *dflt != NUL)
    {
	જો (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = ખાલી;
	}
	નહીંતર જો (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    જો (tofree != ખાલી)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    જો (initdir == ખાલી || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	જો (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	નહીંતર જો (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != ખાલી && buf->b_ffname != ખાલી)
	{
	    જો (dflt == ખાલી || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    જો (tofree != ખાલી)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	નહીંતર જો (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    જો (gui.in_use)		// when this changes, also adjust f_has()!
    {
	જો (filter == ખાલી
#  ifdef FEAT_EVAL
		&& (filter = get_var_કિંમત((char_u *)"b:browsefilter")) == ખાલી
		&& (filter = get_var_કિંમત((char_u *)"g:browsefilter")) == ખાલી
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	જો (flags & BROWSE_DIR)
	{
#  જો defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  નહીંતર
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  જો !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    જો (fname != ખાલી && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		જો (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	નહીંતર
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    નહીંતર
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = ખાલી;
    }

    // keep the directory for next time
    જો (fname != ખાલી)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	જો (last_dir != ખાલી && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    જો (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		જો (last_dir != ખાલી)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    પાછા fname;
}
#endif

#જો defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    શૂન્ય
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    પૂર્ણાંક		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    પૂર્ણાંક		error = FALSE;

    જો (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	પાછા;

    save = (પૂર્ણાંક)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    જો (error || title == ખાલી || initdir == ખાલી || defname == ખાલી)
	rettv->vval.v_string = ખાલી;
    નહીંતર
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, ખાલી, initdir, ખાલી, curbuf);
# નહીંતર
    rettv->vval.v_string = ખાલી;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    શૂન્ય
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    જો (title == ખાલી || initdir == ખાલી)
	rettv->vval.v_string = ખાલી;
    નહીંતર
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, ખાલી, ખાલી, initdir, ખાલી, curbuf);
# નહીંતર
    rettv->vval.v_string = ખાલી;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    શૂન્ય
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_સંખ્યા = FALSE;

    જો (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	પાછા;

    from = tv_get_string(&argvars[0]);

    જો (mch_lstat((અક્ષર *)from, &st) >= 0
	&& (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)))
	rettv->vval.v_સંખ્યા = vim_copyfile(
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
    પૂર્ણાંક		dstlen,	// maximum length of the result
    પૂર્ણાંક		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    જો (src == ખાલી)
    {
	*dst = NUL;
	પાછા 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    જો (buf != ખાલી && buf->b_help)
	પાછા vim_snprintf_safelen((અક્ષર *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    જો (homedir != ખાલી)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#નહીંતર
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    જો (homedir_env == ખાલી)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    જો (homedir_env != ખાલી && *homedir_env == NUL)
	homedir_env = ખાલી;

    જો (homedir_env != ખાલી && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = ખાલી;

	flen = STRLEN(homedir_env);
	(શૂન્ય)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	જો (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    જો (homedir_env != ખાલી)
	envlen = STRLEN(homedir_env);

    જો (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    જ્યારે (*src && dstlen > 0)
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
	માટે (;;)
	{
	    જો (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		જો (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		તોડો;
	    }
	    જો (p == homedir_env)
		તોડો;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	જ્યારે (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	જ્યારે ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    જો (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    પાછા (size_t)(dst - dst_start);
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
    જો (src != ખાલી)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    જો (dst != ખાલી)
	home_replace(buf, src, dst, len, TRUE);
    પાછા dst;
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
    પૂર્ણાંક
fullpathcmp(
    char_u *s1,
    char_u *s2,
    પૂર્ણાંક	    checkname,		// when both don't exist, check file names
    પૂર્ણાંક	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    પૂર્ણાંક		    r1, r2;

    જો (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    નહીંતર
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((અક્ષર *)exp1, &st1);
    r2 = mch_stat((અક્ષર *)s2, &st2);
    જો (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	જો (checkname)
	{
	    જો (fnamecmp(exp1, s2) == 0)
		પાછા FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    જો (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		પાછા FPC_SAMEX;
	}
	પાછા FPC_NOTX;
    }
    જો (r1 != 0 || r2 != 0)
	પાછા FPC_DIFFX;
    જો (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	પાછા FPC_SAME;
    પાછા FPC_DIFF;
#નહીંતર
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    પૂર્ણાંક	    retval = FPC_DIFF;
    પૂર્ણાંક	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    જો ((exp1 = alloc(MAXPATHL * 3)) != ખાલી)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	જો (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	નહીંતર
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	જો (r1 != OK && r2 != OK)
	{
	    જો (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    નહીંતર
		retval = FPC_NOTX;
	}
	નહીંતર જો (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	નહીંતર જો (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	નહીંતર
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    પાછા retval;
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

    જો (fname == ખાલી)
	પાછા (char_u *)"";
    માટે (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	જો (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    પાછા p1;
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
    જ્યારે (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    પાછા t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    જ્યારે (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    જો (*fname)
	++fname;
    પાછા fname;
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

#જો defined(MSWIN)
    // may skip "c:"
    જો (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    નહીંતર
	retval = path;
#નહીંતર
# જો defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    જો (retval == ખાલી)
	retval = path;
# નહીંતર	// Unix
    retval = path;
# endif
#endif

    જ્યારે (vim_ispathsep(*retval))
	++retval;

    પાછા retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    પૂર્ણાંક
vim_ispathsep(પૂર્ણાંક c)
{
#ifdef UNIX
    પાછા (c == '/');	    // UNIX has ':' inside file names
#નહીંતર
# ifdef BACKSLASH_IN_FILENAME
    પાછા (c == ':' || c == '/' || c == '\\');
# નહીંતર
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    પાછા (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  નહીંતર
    પાછા (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    પૂર્ણાંક
vim_ispathsep_nocolon(પૂર્ણાંક c)
{
    પાછા vim_ispathsep(c)
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
    પૂર્ણાંક
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    પૂર્ણાંક		c;
    પૂર્ણાંક		retval;

    p = gettail_sep(fname);
    જો (p == fname)
	પાછા TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    પાછા retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    પૂર્ણાંક
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    પાછા vim_fnamencmp(x, y, MAXPATHL);
#નહીંતર
    જો (p_fic)
	પાછા MB_STRICMP(x, y);
    પાછા STRCMP(x, y);
#endif
}

    પૂર્ણાંક
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    પૂર્ણાંક		cx = NUL;
    પૂર્ણાંક		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    માટે (પૂર્ણાંક swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	જો ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    જો (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		તોડો;
	    }
	    નહીંતર // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    જ્યારે (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	જો (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    તોડો;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    જો (len == 0)
	પાછા 0;
    પાછા (cx - cy);
#નહીંતર
    જો (p_fic)
	પાછા MB_STRNICMP(x, y, len);
    પાછા STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, પૂર્ણાંક sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? કદ(PATHSEP) : 0) + fname2len + 1);
    જો (ret->string == ખાલી)
	ret->length = 0;
    નહીંતર
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	જો (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += કદ(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    પાછા ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    શૂન્ય
add_pathsep(char_u *p)
{
    size_t  plen;

    જો (*p == NUL)
	પાછા;

    plen = STRLEN(p);
    જો (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    પૂર્ણાંક		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = ખાલી;

    જો (fname == ખાલી)
	પાછા ખાલી;

    buf = alloc(MAXPATHL);
    જો (buf == ખાલી)
	પાછા ખાલી;

    જો (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    નહીંતર
	new_fname = vim_strsave(fname);
    vim_free(buf);
    પાછા new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    પૂર્ણાંક
vim_fexists(char_u *fname)
{
    stat_T st;

    જો (mch_stat((અક્ષર *)fname, &st))
	પાછા FALSE;
    પાછા TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    પૂર્ણાંક		  *અંક_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    પૂર્ણાંક		   flags)	// EW_DIR, etc.
{
    પૂર્ણાંક		ret = FAIL;
    char_u	*eval_pat = ખાલી;
    char_u	*exp_pat = *pat;
    અક્ષર	*ignored_સંદેશ;
    size_t	usedlen;
    પૂર્ણાંક		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    પૂર્ણાંક		star_follows = FALSE;

    જો (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       ખાલી, &ignored_સંદેશ, ખાલી, TRUE);
	--emsg_off;
	જો (eval_pat != ખાલી)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    જો (exp_pat != ખાલી)
	ret = expand_wildcards(1, &exp_pat, અંક_file, file, flags);

    જો (eval_pat != ખાલી)
    {
	જો (*અંક_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    જો (*file != ખાલી)
	    {
		**file = eval_pat;
		eval_pat = ખાલી;
		*અંક_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    પાછા ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    પૂર્ણાંક
expand_wildcards(
    પૂર્ણાંક		   અંક_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    પૂર્ણાંક		  *અંક_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    પૂર્ણાંક		   flags)	// EW_DIR, etc.
{
    પૂર્ણાંક		retval;
    પૂર્ણાંક		i, j;
    char_u	*p;
    પૂર્ણાંક		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(અંક_pat, pat, અંક_files, files, flags);

    // When keeping all matches, return here
    જો ((flags & EW_KEEPALL) || retval == FAIL)
	પાછા retval;

    /*
     * Remove names that match 'wildignore'.
     */
    જો (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	માટે (i = 0; i < *અંક_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    જો (ffname == ખાલી)		// out of memory
		તોડો;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    જો (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		માટે (j = i; j + 1 < *અંક_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*અંક_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	જો (*અંક_files == 0)
	{
	    VIM_CLEAR(*files);
	    પાછા FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    જો (*અંક_files > 1 && !got_int)
    {
	non_suf_match = 0;
	માટે (i = 0; i < *અંક_files; ++i)
	{
	    જો (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		માટે (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    પાછા retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    પૂર્ણાંક
match_suffix(char_u *fname)
{
    પૂર્ણાંક		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (પૂર્ણાંક)STRLEN(fname);
    setsuflen = 0;
    માટે (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	જો (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    જો (vim_strchr(tail, '.') == ખાલી)
	    {
		setsuflen = 1;
		તોડો;
	    }
	}
	નહીંતર
	{
	    જો (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		તોડો;
	    setsuflen = 0;
	}
    }
    પાછા (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    સ્થિર પૂર્ણાંક
vim_backtick(char_u *p)
{
    પાછા (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    સ્થિર પૂર્ણાંક
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    પૂર્ણાંક		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    પૂર્ણાંક		cnt = 0;
    પૂર્ણાંક		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    જો (cmd == ખાલી)
	પાછા -1;

#ifdef FEAT_EVAL
    જો (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    નહીંતર
#endif
	buffer = get_cmd_output(cmd, ખાલી,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, ખાલી);
    vim_free(cmd);
    જો (buffer == ખાલી)
	પાછા -1;

    cmd = buffer;
    જ્યારે (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	જ્યારે (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	જો (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	જ્યારે (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    પાછા cnt;
}

#જો defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    સ્થિર પૂર્ણાંક
pstrcmp(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    પાછા (pathcmp(*(અક્ષર **)a, *(અક્ષર **)b, -1));
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
    પૂર્ણાંક
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    પૂર્ણાંક		flags,		// EW_* flags
    પૂર્ણાંક		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    પૂર્ણાંક		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    પૂર્ણાંક		starts_with_dot;
    પૂર્ણાંક		matches;
    size_t	buflen;
    size_t	len;
    પૂર્ણાંક		starstar = FALSE;
    સ્થિર પૂર્ણાંક	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_કિંમત;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = ખાલી;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    પૂર્ણાંક			ok;
    char_u		*p_alt;
    પૂર્ણાંક			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    જો (stardepth > 0)
    {
	ui_breakcheck();
	જો (got_int)
	    પાછા 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    જો (buf == ખાલી)
	પાછા 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ખાલી;
    path_end = path;
    જ્યારે (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	જો (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	નહીંતર જો (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    જો (e != ખાલી)
		તોડો;
	    s = p + 1;
	}
	નહીંતર જો (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != ખાલી)
	    e = p;
	જો (has_mbyte)
	{
	    પૂર્ણાંક charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	નહીંતર
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    માટે (p = buf + wildoff; p < s; ++p)
	જો (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    માટે (p = s; p < e; ++p)
	જો (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ખાલી, FALSE);
    જો (pat == ખાલી)
    {
	vim_free(buf);
	પાછા 0;
    }

    // compile the regexp into a program
    જો (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    જો (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    જો (regmatch.regprog == ખાલી && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	પાછા 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != ખાલી;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    જો (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((અક્ષર *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(શૂન્ય)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((અક્ષર *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, ખાલી);
    જો (wn != ખાલી)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_કિંમત);

    જ્યારે (ok)
    {
	p = utf16_to_enc(wfb.cFileName, ખાલી);   // p is allocated here

	જો (p == ખાલી)
	    તોડો;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	જો (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = ખાલી;
	નહીંતર
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, ખાલી);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	જો ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == ખાલી
		  || (regmatch.regprog != ખાલી
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != ખાલી
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((અક્ષર *)s, buflen - len, "%s", p);
	    જો (len + 1 < buflen)
	    {
		જો (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((અક્ષર *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (શૂન્ય)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((અક્ષર *)buf + len, buflen - len, "%s", path_end);
		જો (mch_has_exp_wildcard(path_end))
		{
		    જો (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(શૂન્ય)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		નહીંતર
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    જો (*path_end != 0)
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
    જો (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   કદ(char_u *), pstrcmp);
    પાછા matches;
}
#endif // MSWIN

#જો (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    સ્થિર પૂર્ણાંક
pstrcmp(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    પાછા (pathcmp(*(અક્ષર **)a, *(અક્ષર **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    પૂર્ણાંક
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    પૂર્ણાંક		flags,		// EW_* flags
    પૂર્ણાંક		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    પૂર્ણાંક		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    પૂર્ણાંક		starts_with_dot;
    પૂર્ણાંક		matches;
    size_t	buflen;
    size_t	len;
    પૂર્ણાંક		starstar = FALSE;
    સ્થિર પૂર્ણાંક	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    સંરચના dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    જો (stardepth > 0)
    {
	ui_breakcheck();
	જો (got_int)
	    પાછા 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    જો (buf == ખાલી)
	પાછા 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ખાલી;
    path_end = path;
    જ્યારે (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	જો (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	નહીંતર જો (*path_end == '/')
	{
	    જો (e != ખાલી)
		તોડો;
	    s = p + 1;
	}
	નહીંતર જો (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != ખાલી
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	જો (has_mbyte)
	{
	    પૂર્ણાંક charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	નહીંતર
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    માટે (p = buf + wildoff; p < s; ++p)
	જો (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    માટે (p = s; p < e; ++p)
	જો (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ખાલી, FALSE);
    જો (pat == ખાલી)
    {
	vim_free(buf);
	પાછા 0;
    }

    // compile the regexp into a program
    જો (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    નહીંતર
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    જો (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    જો (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    જો (regmatch.regprog == ખાલી && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	પાછા 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    જો (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((અક્ષર *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(શૂન્ય)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (અક્ષર *)buf);

    // Find all matching entries
    જો (dirp != ખાલી)
    {
	જ્યારે (!got_int)
	{
	    dp = readdir(dirp);
	    જો (dp == ખાલી)
		તોડો;
	    len = (size_t)(s - buf);
	    જો ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != ખાલી && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((અક્ષર *)s, buflen - len, "%s", dp->d_name);
		જો (len + 1 >= buflen)
		    ચાલુ;

		જો (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((અક્ષર *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (શૂન્ય)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((અક્ષર *)buf + len, buflen - len, "%s", path_end);
		જો (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    જો (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(શૂન્ય)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		નહીંતર
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    જો (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    જો ((flags & EW_ALLLINKS) ? mch_lstat((અક્ષર *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			જો (precomp_buf)
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
    જો (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   કદ(char_u *), pstrcmp);
    પાછા matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    સ્થિર પૂર્ણાંક
has_env_var(char_u *p)
{
    માટે ( ; *p; MB_PTR_ADV(p))
    {
	જો (*p == '\\' && p[1] != NUL)
	    ++p;
	નહીંતર જો (vim_strchr((char_u *)
#જો defined(MSWIN)
				    "$%"
#નહીંતર
				    "$"
#endif
					, *p) != ખાલી)
	    પાછા TRUE;
    }
    પાછા FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    સ્થિર પૂર્ણાંક
has_special_wildchar(char_u *p)
{
    માટે ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	જો (*p == '\r' || *p == '\n')
	    તોડો;
	// Allow for escaping.
	જો (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	નહીંતર જો (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != ખાલી)
	{
	    // A { must be followed by a matching }.
	    જો (*p == '{' && vim_strchr(p, '}') == ખાલી)
		ચાલુ;
	    // A quote and backtick must be followed by another one.
	    જો ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == ખાલી)
		ચાલુ;
	    પાછા TRUE;
	}
    }
    પાછા FALSE;
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
    પૂર્ણાંક
gen_expand_wildcards(
    પૂર્ણાંક		અંક_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    પૂર્ણાંક		*અંક_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    પૂર્ણાંક		flags)		// EW_* flags
{
    પૂર્ણાંક			i;
    garray_T		ga;
    char_u		*p;
    સ્થિર પૂર્ણાંક		recursive = FALSE;
    પૂર્ણાંક			add_pat;
    પૂર્ણાંક			retval = OK;
    પૂર્ણાંક			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    જો (recursive)
#ifdef SPECIAL_WILDCHAR
	પાછા mch_expand_wildcards(અંક_pat, pat, અંક_file, file, flags);
#નહીંતર
	પાછા FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    માટે (i = 0; i < અંક_pat; i++)
    {
	જો (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    પાછા mch_expand_wildcards(અંક_pat, pat, અંક_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, કદ(char_u *), 30);

    માટે (i = 0; i < અંક_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	જો (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    જો (add_pat == -1)
		retval = FAIL;
	}
	નહીંતર
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    જો ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		જો (p == ખાલી)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		નહીંતર જો (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(અંક_pat, pat, અંક_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    પાછા i;
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
	    જો (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		જો ((flags & (EW_PATH | EW_CDPATH))
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
		નહીંતર
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	જો (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    જો (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    નહીંતર
		addfile(&ga, t, flags);

	    જો (t != p)
		vim_free(t);
	}

	જો (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	જો (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    જો (retval == FAIL)
	ga_clear_strings(&ga);

    *અંક_file = ga.ga_len;
    *file = (ga.ga_data != ખાલી) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    પાછા ((flags & EW_EMPTYOK) || ga.ga_data != ખાલી) ? retval : FAIL;
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
    શૂન્ય
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    પૂર્ણાંક		flags)
{
    char_u	*p;
    પૂર્ણાંક		isdir;
    stat_T	sb;

#ifdef MSWIN
    જો (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	જો (perm < 0)
	    પાછા;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    નહીંતર
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	જો (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((અક્ષર *)f, &sb) < 0 : mch_getperm(f) < 0))
	    પાછા;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    જો (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != ખાલી)
	પાછા;
#endif

    જો ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	પાછા;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    જો (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, ખાલી, !(flags & EW_SHELLCMD)))
	પાછા;

    // Make room for another item in the file list.
    જો (ga_grow(gap, 1) == FAIL)
	પાછા;

    p = alloc(STRLEN(f) + 1 + isdir);
    જો (p == ખાલી)
	પાછા;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    જો (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    શૂન્ય
FreeWild(પૂર્ણાંક ગણતરી, char_u **files)
{
    જો (ગણતરી <= 0 || files == ખાલી)
	પાછા;
    જ્યારે (ગણતરી--)
	vim_free(files[ગણતરી]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    પૂર્ણાંક
pathcmp(અચલ અક્ષર *p, અચલ અક્ષર *q, પૂર્ણાંક maxlen)
{
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		c1, c2;
    અચલ અક્ષર	*s = ખાલી;

    માટે (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	જો (c1 == NUL)
	{
	    જો (c2 == NUL)  // full match
		પાછા 0;
	    s = q;
	    i = j;
	    તોડો;
	}

	// End of "q": check if "p" just has a slash.
	જો (c2 == NUL)
	{
	    s = p;
	    તોડો;
	}

	જો ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    જો (vim_ispathsep(c1))
		પાછા -1;
	    જો (vim_ispathsep(c2))
		પાછા 1;
	    પાછા p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    જો (s == ખાલી)	// "i" or "j" ran into "maxlen"
	પાછા 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    જો (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#નહીંતર
	    && c1 == '/'
#endif
       )
	પાછા 0;   // match with trailing slash
    જો (s == q)
	પાછા -1;	    // no match
    પાછા 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    પૂર્ણાંક
vim_isAbsName(char_u *name)
{
    પાછા (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    પૂર્ણાંક		len,
    પૂર્ણાંક		force)	    // force expansion even when already absolute
{
    પૂર્ણાંક		retval = OK;
    પૂર્ણાંક		url;

    *buf = NUL;
    જો (fname == ખાલી)
	પાછા FAIL;

    url = path_with_url(fname);
    જો (!url)
	retval = mch_FullName(fname, buf, len, force);
    જો (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#જો defined(MSWIN)
    slash_adjust(buf);
#endif
    પાછા retval;
}
