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

#شامل "vim.h"

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
    ایستا صحیح
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    صحیح		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * اندازه(*newbuf));
    اگر (newbuf == پوچ)
	بازگشت FAIL;

    wfname = enc_to_utf16(*fnamep, پوچ);
    اگر (wfname == پوچ)
    {
	vim_free(newbuf);
	بازگشت FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    اگر (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * اندازه(*newbuf));
	اگر (newbuf == پوچ)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    بازگشت FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    اگر (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, پوچ);

	اگر (p != پوچ)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	وگرنه
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    بازگشت FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    بازگشت OK;
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
    ایستا صحیح
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = پوچ;
    char_u	*short_fname = پوچ;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    صحیح		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    اگر (save_fname == پوچ)
    {
	retval = FAIL;
	برو theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    برای (;;)
    {
	// go back one path-separator
	وقتی (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	اگر (endp <= save_fname)
	    بشکن;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	اگر (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    برو theend;
	}
	*endp = ch;	// preserve the string

	اگر (len > 0)
	    بشکن;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    اگر (len > 0)
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
	اگر (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    اگر (*fname == پوچ)
	    {
		retval = FAIL;
		برو theend;
	    }
	}
	وگرنه
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    اگر (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = پوچ;
	}

	// concat the not-shortened part of the path
	اگر ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    بازگشت retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    ایستا صحیح
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    صحیح		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    صحیح		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    برای (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	اگر (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    اگر (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    وگرنه
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    اگر (tfname == پوچ)
	بازگشت FAIL;

    len = tflen = STRLEN(tfname);

    اگر (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	بازگشت FAIL;

    اگر (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	اگر (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    بازگشت FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    برای (p = tfname + len - 1; p >= tfname; --p)
    {
	اگر (has_mbyte)
	    p -= mb_head_off(tfname, p);
	اگر (vim_ispathsep(*p))
	{
	    اگر (sepcount == 0 || (hasTilde && sepcount == 1))
		بشکن;
	    وگرنه
		sepcount --;
	}
    }
    اگر (hasTilde)
    {
	--p;
	اگر (p >= tfname)
	    *p = '~';
	وگرنه
	    بازگشت FAIL;
    }
    وگرنه
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    بازگشت OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    صحیح
modify_fname(
    char_u	*src,		// string with modifiers
    صحیح		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    صحیح		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    صحیح		c;
    صحیح		has_fullname = 0;
    صحیح		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    صحیح		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    اگر (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	اگر ((*fnamep)[0] == '~'
#اگر !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    اگر (*fnamep == پوچ)
		بازگشت -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	برای (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    اگر (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		بشکن;
	}

	// FullName_save() is slow, don't use it when not needed.
	اگر (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    اگر (*fnamep == پوچ)
		بازگشت -1;
	}

#ifdef MSWIN
# اگر _WIN32_WINNT >= 0x0500
	اگر (vim_strchr(*fnamep, '~') != پوچ)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, پوچ);
	    WCHAR buf[_MAX_PATH];

	    اگر (wfname != پوچ)
	    {
		اگر (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, پوچ);

		    اگر (q != پوچ)
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
	اگر (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    اگر (*fnamep == پوچ)
		بازگشت -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    وقتی (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	اگر (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    ادامه;
	}
	pbuf = پوچ;
	// Need full path first (use expand_env() to remove a "~/")
	اگر (!has_fullname && !has_homerelative)
	{
	    اگر (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    وگرنه
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	وگرنه
	    p = *fnamep;

	has_fullname = 0;

	اگر (p != پوچ)
	{
	    size_t  dirnamelen = 0;

	    اگر (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		اگر (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    اگر (s != پوچ)
		    {
			dirnamelen = home_replace(پوچ, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		اگر (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		اگر (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    اگر (vim_ispathsep(*p))
		    {
			وقتی (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			اگر (pbuf != پوچ)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = پوچ;
			}
		    }
		}
	    }
	    وگرنه
	    {
		dirnamelen = home_replace(پوچ, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		اگر (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    اگر (s != پوچ)
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
    وقتی (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	وقتی (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	اگر (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	اگر (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    اگر (p == پوچ)
		بازگشت -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	وگرنه
	{
	    وقتی (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    اگر (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    اگر (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	اگر (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    اگر (p == پوچ)
		بازگشت -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	اگر (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    اگر (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		بازگشت -1;
	}
	وگرنه
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    اگر (get_short_pathname(fnamep, bufp, &l) == FAIL)
		بازگشت -1;

	    اگر (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		اگر (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    بازگشت -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    اگر (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    وقتی (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	اگر (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	وگرنه
	    s = *fnamep + *fnamelen - 1;
	برای ( ; s > tail; --s)
	    اگر (s[0] == '.')
		بشکن;
	اگر (src[*usedlen + 1] == 'e')		// :e
	{
	    اگر (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		برای ( ; s > *fnamep; --s)
		    اگر (s[0] == ';')
			بشکن;
		اگر (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    وگرنه اگر (*fnamep <= tail)
		*fnamelen = 0;
	}
	وگرنه				// :r
	{
	    char_u *limit = *fnamep;

	    اگر (limit < tail)
		limit = tail;
	    اگر (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    اگر (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	صحیح	    sep;
	char_u	    *flags;
	صحیح	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	اگر (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	اگر (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    اگر (p != پوچ)
	    {
		pat = vim_strnsave(s, p - s);
		اگر (pat != پوچ)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    اگر (p != پوچ)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			اگر (sub != پوچ && str != پوچ)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, پوچ, flags, &slen);
			    اگر (s != پوچ)
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
	    اگر (didit)
		برو repeat;
	}
    }

    اگر (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	اگر (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	اگر (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	اگر (p == پوچ)
	    بازگشت -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    بازگشت valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    ایستا تهی
shorten_dir_len(char_u *str, صحیح trim_len)
{
    char_u	*tail, *s, *d;
    صحیح		skip = FALSE;
    صحیح		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    برای (s = str; ; ++s)
    {
	اگر (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    اگر (*s == NUL)
		بشکن;
	}
	وگرنه اگر (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	وگرنه اگر (!skip)
	{
	    *d++ = *s;			// copy next char
	    اگر (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		اگر (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    اگر (has_mbyte)
	    {
		صحیح l = mb_ptr2len(s);

		وقتی (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    تهی
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    صحیح
file_is_readable(char_u *fname)
{
    صحیح		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    اگر (*fname == NUL || mch_isdir_or_missing(fname))
	بازگشت FALSE;
    اگر ((fd = mch_open((نویسه *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

#اگر defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    تهی
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (check_secure())
	بازگشت;

    اگر (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	اگر (in_vim9script())
	    (تهی) check_for_string_arg(argvars, 0);
	بازگشت;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    اگر (cwd != پوچ)
    {
	اگر (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	اگر (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	وگرنه اگر (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	وگرنه اگر (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	وگرنه
	{
	    semsg(_(e_invalid_مقدار_for_argument_str_str), "scope", s);
	    بازگشت;
	}
    }
    وگرنه اگر (curwin->w_localdir != پوچ)
	scope = CDSCOPE_WINDOW;
    وگرنه اگر (curtab->tp_localdir != پوچ)
	scope = CDSCOPE_TABPAGE;

    اگر (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    تهی
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_عدد = -1;
    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    name = tv_get_string(&argvars[0]);
    اگر (name == پوچ || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	بازگشت;
    }

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    وگرنه
	flags = (char_u *)"";

    اگر (*flags == NUL)
	// delete a file
	rettv->vval.v_عدد = mch_remove(name) == 0 ? 0 : -1;
    وگرنه اگر (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_عدد = mch_rmdir(name) == 0 ? 0 : -1;
    وگرنه اگر (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_عدد = delete_recursive(name);
    وگرنه
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    تهی
f_executable(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_عدد = mch_can_exe(tv_get_string(&argvars[0]), پوچ, TRUE);
}

/*
 * "exepath()" function
 */
    تهی
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = پوچ;

    اگر (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	بازگشت;
    (تهی)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    تهی
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;
    rettv->vval.v_عدد = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    تهی
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;
    rettv->vval.v_عدد = filewritable(tv_get_string(&argvars[0]));
}

    ایستا تهی
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		find_what)
{
    char_u	*fname;
    char_u	*fresult = پوچ;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    صحیح		شمار = 1;
    صحیح		first = TRUE;
    صحیح		error = FALSE;

    rettv->vval.v_string = پوچ;
    rettv->v_type = VAR_STRING;
    اگر (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 2) == FAIL)))
	بازگشت;

    fname = tv_get_string(&argvars[0]);

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	اگر (p == پوچ)
	    error = TRUE;
	وگرنه
	{
	    اگر (*p != NUL)
		path = p;

	    اگر (argvars[2].v_type != VAR_UNKNOWN)
		شمار = (صحیح)tv_get_عدد_chk(&argvars[2], &error);
	}
    }

    اگر (شمار < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    اگر (*fname != NUL && !error)
    {
	char_u	*file_to_find = پوچ;
	نویسه	*جستجو_ctx = پوچ;

	انجامبده
	{
	    اگر (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : پوچ,
					       first ? (صحیح)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &جستجو_ctx);
	    first = FALSE;

	    اگر (fresult != پوچ && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} وقتی ((rettv->v_type == VAR_LIST || --شمار > 0) && fresult != پوچ);

	vim_free(file_to_find);
	vim_findfile_cleanup(جستجو_ctx);
    }

    اگر (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    تهی
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    تهی
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    تهی
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = پوچ;
    char_u	buf[NUMBUFLEN];

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    اگر (mods == پوچ || fname == پوچ)
	fname = پوچ;
    وگرنه
    {
	len = STRLEN(fname);
	اگر (mods != پوچ && *mods != NUL)
	    (تهی)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    اگر (fname == پوچ)
	rettv->vval.v_string = پوچ;
    وگرنه
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
    تهی
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = پوچ;
    tabpage_T	*tp = پوچ;
    char_u	*cwd;
    صحیح		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script()
	    && (check_for_opt_عدد_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 1) == FAIL)))
	بازگشت;

    اگر (argvars[0].v_type == VAR_عدد
	    && argvars[0].vval.v_عدد == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    وگرنه
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    اگر (wp != پوچ && wp->w_localdir != پوچ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    وگرنه اگر (tp != پوچ && tp->tp_localdir != پوچ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    وگرنه اگر (wp != پوچ || tp != پوچ || global)
    {
	اگر (globaldir != پوچ && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	وگرنه
	{
	    cwd = alloc(MAXPATHL);
	    اگر (cwd != پوچ)
	    {
		اگر (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    اگر (rettv->vval.v_string != پوچ)
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
    صحیح		    i;

    برای (i = 0; i < 9; i++)
    {
	اگر (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	وگرنه
	    perm[i] = '-';
    }
    بازگشت perm;
}

/*
 * "getfperm({fname})" function
 */
    تهی
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = پوچ;
    char_u	permbuf[] = "---------";

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    اگر (mch_stat((نویسه *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    تهی
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    fname = tv_get_string(&argvars[0]);
    اگر (mch_stat((نویسه *)fname, &st) >= 0)
    {
	اگر (S_ISDIR(st.st_mode))
	    rettv->vval.v_عدد = 0;
	وگرنه
	{
	    rettv->vval.v_عدد = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    اگر ((off_T)rettv->vval.v_عدد != (off_T)st.st_size)
		rettv->vval.v_عدد = -2;
	}
    }
    وگرنه
	  rettv->vval.v_عدد = -1;
}

/*
 * "getftime({fname})" function
 */
    تهی
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    fname = tv_get_string(&argvars[0]);
    اگر (mch_stat((نویسه *)fname, &st) >= 0)
	rettv->vval.v_عدد = (varnumber_T)st.st_mtime;
    وگرنه
	rettv->vval.v_عدد = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    نویسه    *t;

    اگر (S_ISREG(st->st_mode))
	t = "file";
    وگرنه اگر (S_ISDIR(st->st_mode))
	t = "dir";
    وگرنه اگر (S_ISLNK(st->st_mode))
	t = "link";
    وگرنه اگر (S_ISBLK(st->st_mode))
	t = "bdev";
    وگرنه اگر (S_ISCHR(st->st_mode))
	t = "cdev";
    وگرنه اگر (S_ISFIFO(st->st_mode))
	t = "fifo";
    وگرنه اگر (S_ISSOCK(st->st_mode))
	t = "socket";
    وگرنه
	t = "other";
    بازگشت (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    تهی
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = پوچ;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    اگر (mch_lstat((نویسه *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    تهی
f_glob(typval_T *argvars, typval_T *rettv)
{
    صحیح		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    صحیح		error = FALSE;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	بازگشت;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    اگر (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, پوچ);
	    اگر (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    اگر (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	اگر (p_wic)
	    options += WILD_ICASE;
	اگر (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     پوچ, options, WILD_ALL);
	وگرنه اگر (rettv_list_alloc(rettv) == OK)
	{
	  صحیح i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						پوچ, options, WILD_ALL_KEEP);
	  برای (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    وگرنه
	rettv->vval.v_string = پوچ;
}

/*
 * "glob2regpat()" function
 */
    تهی
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == پوچ)
			 ? پوچ : file_pat_to_reg_pat(pat, پوچ, پوچ, FALSE);
}

/*
 * "globpath()" function
 */
    تهی
f_globpath(typval_T *argvars, typval_T *rettv)
{
    صحیح		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    صحیح		error = FALSE;
    garray_T	ga;
    صحیح		i;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	بازگشت;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	اگر (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	{
	    اگر (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, پوچ);
	    اگر (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    اگر (file != پوچ && !error)
    {
	ga_init2(&ga, اندازه(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	اگر (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	وگرنه اگر (rettv_list_alloc(rettv) == OK)
	    برای (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    وگرنه
	rettv->vval.v_string = پوچ;
}

/*
 * "isdirectory()" function
 */
    تهی
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    تهی
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    ایستا صحیح
mkdir_recurse(char_u *dir, صحیح prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    صحیح		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    اگر (p <= get_past_head(dir))
	بازگشت OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    اگر (updir == پوچ)
	بازگشت FAIL;
    اگر (mch_isdir(updir))
	r = OK;
    وگرنه اگر (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	اگر (r == OK && created != پوچ && *created == پوچ)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    بازگشت r;
}

/*
 * "mkdir()" function
 */
    تهی
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    صحیح		prot = 0755;
    صحیح		defer = FALSE;
    صحیح		defer_recurse = FALSE;
    char_u	*created = پوچ;

    rettv->vval.v_عدد = FAIL;
    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 2) == FAIL)))
	بازگشت;

    dir = tv_get_string_buf(&argvars[0], buf);
    اگر (*dir == NUL)
	بازگشت;

    اگر (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (صحیح)tv_get_عدد_chk(&argvars[2], پوچ);
	    اگر (prot == -1)
		بازگشت;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != پوچ;
	defer_recurse = vim_strchr(arg2, 'R') != پوچ;
	اگر ((defer || defer_recurse) && !can_add_defer())
	    بازگشت;

	اگر (vim_strchr(arg2, 'p') != پوچ)
	{
	    اگر (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_عدد = OK;
		بازگشت;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : پوچ);
	}
    }
    rettv->vval.v_عدد = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    اگر (rettv->vval.v_عدد == OK
				&& created == پوچ && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    اگر (created != پوچ)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	اگر (tv[0].vval.v_string == پوچ || tv[1].vval.v_string == پوچ
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
    تهی
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		trim_len = 1;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (صحیح)tv_get_عدد(&argvars[1]);
	اگر (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    اگر (p == پوچ)
	rettv->vval.v_string = پوچ;
    وگرنه
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	اگر (p != پوچ)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    ایستا صحیح
checkitem_common(تهی *context, char_u *name, dict_T *dict)
{
    typval_T	*عبارت = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    صحیح		retval = 0;
    صحیح		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    اگر (name != پوچ)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    وگرنه
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    اگر (eval_عبارت_typval(عبارت, FALSE, argv, 1, پوچ, &rettv) == FAIL)
	برو theend;

    // We want to use -1, but also true/false should be allowed.
    اگر (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_عدد;
	rettv.vval.v_عدد = rettv.vval.v_عدد == VVAL_TRUE;
    }
    retval = tv_get_عدد_chk(&rettv, &error);
    اگر (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    اگر (name != پوچ)
	set_vim_var_string(VV_VAL, پوچ, 0);
    وگرنه
	set_vim_var_dict(VV_VAL, پوچ);
    restore_vimvar(VV_VAL, &save_val);
    بازگشت retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    ایستا صحیح
readdir_checkitem(تهی *context, تهی *item)
{
    char_u	*name = (char_u *)item;

    بازگشت checkitem_common(context, name, پوچ);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    ایستا صحیح
readdirex_dict_arg(typval_T *argvars, صحیح *cmp)
{
    char_u     *compare;

    اگر (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	بازگشت FAIL;

    اگر (dict_has_کلید(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    وگرنه
    {
	semsg(_(e_dictionary_کلید_str_required), "sort");
	بازگشت FAIL;
    }

    اگر (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    وگرنه اگر (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    وگرنه اگر (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    وگرنه اگر (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    بازگشت OK;
}

/*
 * "readdir()" function
 */
    تهی
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*عبارت;
    صحیح		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    صحیح		i;
    صحیح		sort = READDIR_SORT_BYTE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;
    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	بازگشت;

    path = tv_get_string(&argvars[0]);
    عبارت = &argvars[1];

    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	بازگشت;

    ret = readdir_core(&ga, path, FALSE, (تهی *)عبارت,
	    (عبارت->v_type == VAR_UNKNOWN) ? پوچ : readdir_checkitem, sort);
    اگر (ret == OK)
    {
	برای (i = 0; i < ga.ga_len; i++)
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
    ایستا صحیح
readdirex_checkitem(تهی *context, تهی *item)
{
    dict_T	*dict = (dict_T*)item;

    بازگشت checkitem_common(context, پوچ, dict);
}

/*
 * "readdirex()" function
 */
    تهی
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*عبارت;
    صحیح		ret;
    char_u	*path;
    garray_T	ga;
    صحیح		i;
    صحیح		sort = READDIR_SORT_BYTE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;
    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	بازگشت;

    path = tv_get_string(&argvars[0]);
    عبارت = &argvars[1];

    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	بازگشت;

    ret = readdir_core(&ga, path, TRUE, (تهی *)عبارت,
	    (عبارت->v_type == VAR_UNKNOWN) ? پوچ : readdirex_checkitem, sort);
    اگر (ret == OK)
    {
	برای (i = 0; i < ga.ga_len; i++)
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
    ایستا تهی
read_file_or_blob(typval_T *argvars, typval_T *rettv, صحیح always_blob)
{
    صحیح		دودویی = FALSE;
    صحیح		blob = always_blob;
    صحیح		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    صحیح		io_size = اندازه(buf);
    صحیح		readlen;		// size of last fread()
    char_u	*prev	 = پوچ;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (always_blob)
	{
	    offset = (off_T)tv_get_عدد(&argvars[1]);
	    اگر (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_عدد(&argvars[2]);
	}
	وگرنه
	{
	    اگر (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		دودویی = TRUE;
	    اگر (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    اگر (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_عدد(&argvars[2]);
	}
    }

    اگر ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	بازگشت;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    اگر (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	بازگشت;
    }
    اگر (*fname == NUL || (fd = mch_fopen((نویسه *)fname, READBIN)) == پوچ)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	بازگشت;
    }

    اگر (blob)
    {
	اگر (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	بازگشت;
    }

    وقتی (cnt < maxline || maxline < 0)
    {
	readlen = (صحیح)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	برای (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || دودویی));
		++p)
	{
	    اگر (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= پوچ;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		اگر (readlen > 0 && !دودویی)
		{
		    وقتی (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    اگر (len == 0)
			وقتی (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		اگر (prevlen == 0)
		    s = vim_strnsave(start, len);
		وگرنه
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    اگر ((s = vim_realloc(prev, prevlen + len + 1)) != پوچ)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = پوچ; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		اگر (s == پوچ)
		{
		    do_outofmem_پیام((long_u) prevlen + len + 1);
		    failed = TRUE;
		    بشکن;
		}

		اگر ((li = listitem_alloc()) == پوچ)
		{
		    vim_free(s);
		    failed = TRUE;
		    بشکن;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		اگر ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    بشکن;
	    }
	    وگرنه اگر (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    وگرنه اگر (*p == 0xbf && enc_utf8 && !دودویی)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		اگر (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    اگر (start == dest)
			start = p + 1;
		    وگرنه
		    {
			// have to shuffle buf to close gap
			صحیح adjust_prevlen = 0;

			اگر (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (صحیح)(buf - dest);
			    dest = buf;
			}
			اگر (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	اگر (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    بشکن;
	اگر (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    اگر (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		اگر (prevsize == 0)
		    prevsize = (long)(p - start);
		وگرنه
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		اگر (newprev == پوچ)
		{
		    do_outofmem_پیام((long_u)prevsize);
		    failed = TRUE;
		    بشکن;
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
    اگر (!failed && maxline < 0)
	وقتی (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    اگر (failed)
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
    تهی
f_readblob(typval_T *argvars, typval_T *rettv)
{
    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 2) == FAIL)))
	بازگشت;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    تهی
f_readfile(typval_T *argvars, typval_T *rettv)
{
    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 2) == FAIL)))
	بازگشت;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    تهی
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = پوچ;
# endif

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = پوچ;

	v = mch_resolve_path(p, TRUE);
	اگر (v != پوچ)
	    rettv->vval.v_string = v;
	وگرنه
	    rettv->vval.v_string = vim_strsave(p);
    }
# وگرنه
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	صحیح	len;
	char_u	*remain = پوچ;
	char_u	*q;
	صحیح	is_relative_to_current = FALSE;
	صحیح	has_trailing_pathsep = FALSE;
	صحیح	limit = 100;

	p = vim_strsave(p);
	اگر (p == پوچ)
	    برو fail;
	اگر (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	اگر (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	اگر (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	اگر (buf == پوچ)
	{
	    vim_free(p);
	    vim_free(remain);
	    برو fail;
	}

	برای (;;)
	{
	    برای (;;)
	    {
		len = readlink((نویسه *)p, (نویسه *)buf, MAXPATHL);
		اگر (len <= 0)
		    بشکن;
		buf[len] = NUL;

		اگر (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = پوچ;
		    برو fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		اگر (remain == پوچ && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		اگر (*q != NUL)
		{
		    اگر (remain == پوچ)
			remain = vim_strsave(q - 1);
		    وگرنه
		    {
			cpy = concat_str(q - 1, remain);
			اگر (cpy != پوچ)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		اگر (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		اگر (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    اگر (cpy != پوچ)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		وگرنه
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    اگر (remain == پوچ)
		بشکن;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    اگر (cpy != پوچ)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    اگر (*q != NUL)
		STRMOVE(remain, q - 1);
	    وگرنه
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	اگر (!vim_ispathsep(*p))
	{
	    اگر (is_relative_to_current
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
		اگر (cpy != پوچ)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    وگرنه اگر (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		وقتی (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		اگر (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	اگر (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    اگر (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  وگرنه
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
    تهی
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    ایستا صحیح	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    انجامبده
    {
	اگر (x == 'Z')
	    x = '0';
	وگرنه اگر (x == '9')
	    x = 'A';
	وگرنه
	    ++x;
    } وقتی (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    تهی
f_writefile(typval_T *argvars, typval_T *rettv)
{
    صحیح		دودویی = FALSE;
    صحیح		append = FALSE;
    صحیح		defer = FALSE;
# ifdef HAVE_FSYNC
    صحیح		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    صحیح		ret = 0;
    listitem_T	*li;
    list_T	*list = پوچ;
    blob_T	*blob = پوچ;

    rettv->vval.v_عدد = -1;
    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	بازگشت;

    اگر (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	اگر (list == پوچ)
	    بازگشت;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_اقلام(list, li)
	    اگر (tv_get_string_chk(&li->li_tv) == پوچ)
		بازگشت;
    }
    وگرنه اگر (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	اگر (blob == پوچ)
	    بازگشت;
    }
    وگرنه
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	بازگشت;
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	اگر (arg2 == پوچ)
	    بازگشت;
	اگر (vim_strchr(arg2, 'b') != پوچ)
	    دودویی = TRUE;
	اگر (vim_strchr(arg2, 'a') != پوچ)
	    append = TRUE;
	اگر (vim_strchr(arg2, 'D') != پوچ)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	اگر (vim_strchr(arg2, 's') != پوچ)
	    do_fsync = TRUE;
	وگرنه اگر (vim_strchr(arg2, 'S') != پوچ)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    اگر (fname == پوچ)
	بازگشت;

    اگر (defer && !can_add_defer())
	بازگشت;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    اگر (*fname == NUL || (fd = mch_fopen((نویسه *)fname,
				      append ? APPENDBIN : WRITEBIN)) == پوچ)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    وگرنه
    {
	اگر (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    اگر (tv.vval.v_string == پوچ
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(تهی)mch_remove(fname);
	    }
	}

	اگر (ret == 0)
	{
	    اگر (blob)
	    {
		اگر (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    وگرنه
	    {
		اگر (write_list(fd, list, دودویی) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    اگر (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_عدد = ret;
}

#endif // FEAT_EVAL

#اگر defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    صحیح		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    ایستا char_u	*last_dir = پوچ;    // last used directory
    char_u		*tofree = پوچ;
    صحیح			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    اگر (title == پوچ || *title == NUL)
    {
	اگر (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	وگرنه اگر (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	وگرنه
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    اگر ((initdir == پوچ || *initdir == NUL) && dflt != پوچ && *dflt != NUL)
    {
	اگر (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = پوچ;
	}
	وگرنه اگر (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    اگر (tofree != پوچ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    اگر (initdir == پوچ || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	اگر (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	وگرنه اگر (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != پوچ && buf->b_ffname != پوچ)
	{
	    اگر (dflt == پوچ || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    اگر (tofree != پوچ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	وگرنه اگر (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    اگر (gui.in_use)		// when this changes, also adjust f_has()!
    {
	اگر (filter == پوچ
#  ifdef FEAT_EVAL
		&& (filter = get_var_مقدار((char_u *)"b:browsefilter")) == پوچ
		&& (filter = get_var_مقدار((char_u *)"g:browsefilter")) == پوچ
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	اگر (flags & BROWSE_DIR)
	{
#  اگر defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  وگرنه
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  اگر !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    اگر (fname != پوچ && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		اگر (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	وگرنه
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    وگرنه
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = پوچ;
    }

    // keep the directory for next time
    اگر (fname != پوچ)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	اگر (last_dir != پوچ && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    اگر (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		اگر (last_dir != پوچ)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    بازگشت fname;
}
#endif

#اگر defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    تهی
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    صحیح		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    صحیح		error = FALSE;

    اگر (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	بازگشت;

    save = (صحیح)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    اگر (error || title == پوچ || initdir == پوچ || defname == پوچ)
	rettv->vval.v_string = پوچ;
    وگرنه
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, پوچ, initdir, پوچ, curbuf);
# وگرنه
    rettv->vval.v_string = پوچ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    تهی
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    اگر (title == پوچ || initdir == پوچ)
	rettv->vval.v_string = پوچ;
    وگرنه
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, پوچ, پوچ, initdir, پوچ, curbuf);
# وگرنه
    rettv->vval.v_string = پوچ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    تهی
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_عدد = FALSE;

    اگر (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	بازگشت;

    from = tv_get_string(&argvars[0]);

    اگر (mch_lstat((نویسه *)from, &st) >= 0
	&& (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)))
	rettv->vval.v_عدد = vim_copyfile(
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
    صحیح		dstlen,	// maximum length of the result
    صحیح		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    اگر (src == پوچ)
    {
	*dst = NUL;
	بازگشت 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    اگر (buf != پوچ && buf->b_help)
	بازگشت vim_snprintf_safelen((نویسه *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    اگر (homedir != پوچ)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#وگرنه
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    اگر (homedir_env == پوچ)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    اگر (homedir_env != پوچ && *homedir_env == NUL)
	homedir_env = پوچ;

    اگر (homedir_env != پوچ && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = پوچ;

	flen = STRLEN(homedir_env);
	(تهی)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	اگر (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    اگر (homedir_env != پوچ)
	envlen = STRLEN(homedir_env);

    اگر (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    وقتی (*src && dstlen > 0)
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
	برای (;;)
	{
	    اگر (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		اگر (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		بشکن;
	    }
	    اگر (p == homedir_env)
		بشکن;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	وقتی (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	وقتی ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    اگر (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    بازگشت (size_t)(dst - dst_start);
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
    اگر (src != پوچ)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    اگر (dst != پوچ)
	home_replace(buf, src, dst, len, TRUE);
    بازگشت dst;
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
    صحیح
fullpathcmp(
    char_u *s1,
    char_u *s2,
    صحیح	    checkname,		// when both don't exist, check file names
    صحیح	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    صحیح		    r1, r2;

    اگر (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    وگرنه
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((نویسه *)exp1, &st1);
    r2 = mch_stat((نویسه *)s2, &st2);
    اگر (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	اگر (checkname)
	{
	    اگر (fnamecmp(exp1, s2) == 0)
		بازگشت FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    اگر (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		بازگشت FPC_SAMEX;
	}
	بازگشت FPC_NOTX;
    }
    اگر (r1 != 0 || r2 != 0)
	بازگشت FPC_DIFFX;
    اگر (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	بازگشت FPC_SAME;
    بازگشت FPC_DIFF;
#وگرنه
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    صحیح	    retval = FPC_DIFF;
    صحیح	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    اگر ((exp1 = alloc(MAXPATHL * 3)) != پوچ)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	اگر (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	وگرنه
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	اگر (r1 != OK && r2 != OK)
	{
	    اگر (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    وگرنه
		retval = FPC_NOTX;
	}
	وگرنه اگر (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	وگرنه اگر (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	وگرنه
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    بازگشت retval;
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

    اگر (fname == پوچ)
	بازگشت (char_u *)"";
    برای (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	اگر (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    بازگشت p1;
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
    وقتی (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    بازگشت t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    وقتی (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    اگر (*fname)
	++fname;
    بازگشت fname;
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

#اگر defined(MSWIN)
    // may skip "c:"
    اگر (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    وگرنه
	retval = path;
#وگرنه
# اگر defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    اگر (retval == پوچ)
	retval = path;
# وگرنه	// Unix
    retval = path;
# endif
#endif

    وقتی (vim_ispathsep(*retval))
	++retval;

    بازگشت retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    صحیح
vim_ispathsep(صحیح c)
{
#ifdef UNIX
    بازگشت (c == '/');	    // UNIX has ':' inside file names
#وگرنه
# ifdef BACKSLASH_IN_FILENAME
    بازگشت (c == ':' || c == '/' || c == '\\');
# وگرنه
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    بازگشت (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  وگرنه
    بازگشت (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    صحیح
vim_ispathsep_nocolon(صحیح c)
{
    بازگشت vim_ispathsep(c)
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
    صحیح
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    صحیح		c;
    صحیح		retval;

    p = gettail_sep(fname);
    اگر (p == fname)
	بازگشت TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    بازگشت retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    صحیح
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    بازگشت vim_fnamencmp(x, y, MAXPATHL);
#وگرنه
    اگر (p_fic)
	بازگشت MB_STRICMP(x, y);
    بازگشت STRCMP(x, y);
#endif
}

    صحیح
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    صحیح		cx = NUL;
    صحیح		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    برای (صحیح swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	اگر ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    اگر (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		بشکن;
	    }
	    وگرنه // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    وقتی (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	اگر (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    بشکن;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    اگر (len == 0)
	بازگشت 0;
    بازگشت (cx - cy);
#وگرنه
    اگر (p_fic)
	بازگشت MB_STRNICMP(x, y, len);
    بازگشت STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, صحیح sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? اندازه(PATHSEP) : 0) + fname2len + 1);
    اگر (ret->string == پوچ)
	ret->length = 0;
    وگرنه
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	اگر (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += اندازه(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    بازگشت ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    تهی
add_pathsep(char_u *p)
{
    size_t  plen;

    اگر (*p == NUL)
	بازگشت;

    plen = STRLEN(p);
    اگر (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    صحیح		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = پوچ;

    اگر (fname == پوچ)
	بازگشت پوچ;

    buf = alloc(MAXPATHL);
    اگر (buf == پوچ)
	بازگشت پوچ;

    اگر (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    وگرنه
	new_fname = vim_strsave(fname);
    vim_free(buf);
    بازگشت new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    صحیح
vim_fexists(char_u *fname)
{
    stat_T st;

    اگر (mch_stat((نویسه *)fname, &st))
	بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    صحیح
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    صحیح		  *رقم_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    صحیح		   flags)	// EW_DIR, etc.
{
    صحیح		ret = FAIL;
    char_u	*eval_pat = پوچ;
    char_u	*exp_pat = *pat;
    نویسه	*ignored_پیام;
    size_t	usedlen;
    صحیح		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    صحیح		star_follows = FALSE;

    اگر (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       پوچ, &ignored_پیام, پوچ, TRUE);
	--emsg_off;
	اگر (eval_pat != پوچ)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    اگر (exp_pat != پوچ)
	ret = expand_wildcards(1, &exp_pat, رقم_file, file, flags);

    اگر (eval_pat != پوچ)
    {
	اگر (*رقم_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    اگر (*file != پوچ)
	    {
		**file = eval_pat;
		eval_pat = پوچ;
		*رقم_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    بازگشت ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    صحیح
expand_wildcards(
    صحیح		   رقم_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    صحیح		  *رقم_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    صحیح		   flags)	// EW_DIR, etc.
{
    صحیح		retval;
    صحیح		i, j;
    char_u	*p;
    صحیح		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(رقم_pat, pat, رقم_files, files, flags);

    // When keeping all matches, return here
    اگر ((flags & EW_KEEPALL) || retval == FAIL)
	بازگشت retval;

    /*
     * Remove names that match 'wildignore'.
     */
    اگر (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	برای (i = 0; i < *رقم_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    اگر (ffname == پوچ)		// out of memory
		بشکن;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    اگر (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		برای (j = i; j + 1 < *رقم_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*رقم_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	اگر (*رقم_files == 0)
	{
	    VIM_CLEAR(*files);
	    بازگشت FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    اگر (*رقم_files > 1 && !got_int)
    {
	non_suf_match = 0;
	برای (i = 0; i < *رقم_files; ++i)
	{
	    اگر (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		برای (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    بازگشت retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    صحیح
match_suffix(char_u *fname)
{
    صحیح		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (صحیح)STRLEN(fname);
    setsuflen = 0;
    برای (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	اگر (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    اگر (vim_strchr(tail, '.') == پوچ)
	    {
		setsuflen = 1;
		بشکن;
	    }
	}
	وگرنه
	{
	    اگر (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		بشکن;
	    setsuflen = 0;
	}
    }
    بازگشت (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    ایستا صحیح
vim_backtick(char_u *p)
{
    بازگشت (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    ایستا صحیح
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    صحیح		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    صحیح		cnt = 0;
    صحیح		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    اگر (cmd == پوچ)
	بازگشت -1;

#ifdef FEAT_EVAL
    اگر (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    وگرنه
#endif
	buffer = get_cmd_output(cmd, پوچ,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, پوچ);
    vim_free(cmd);
    اگر (buffer == پوچ)
	بازگشت -1;

    cmd = buffer;
    وقتی (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	وقتی (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	اگر (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	وقتی (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    بازگشت cnt;
}

#اگر defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    ایستا صحیح
pstrcmp(ثابت تهی *a, ثابت تهی *b)
{
    بازگشت (pathcmp(*(نویسه **)a, *(نویسه **)b, -1));
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
    صحیح
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    صحیح		flags,		// EW_* flags
    صحیح		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    صحیح		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    صحیح		starts_with_dot;
    صحیح		matches;
    size_t	buflen;
    size_t	len;
    صحیح		starstar = FALSE;
    ایستا صحیح	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_مقدار;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = پوچ;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    صحیح			ok;
    char_u		*p_alt;
    صحیح			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    اگر (stardepth > 0)
    {
	ui_breakcheck();
	اگر (got_int)
	    بازگشت 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    اگر (buf == پوچ)
	بازگشت 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = پوچ;
    path_end = path;
    وقتی (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	اگر (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	وگرنه اگر (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    اگر (e != پوچ)
		بشکن;
	    s = p + 1;
	}
	وگرنه اگر (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != پوچ)
	    e = p;
	اگر (has_mbyte)
	{
	    صحیح charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	وگرنه
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    برای (p = buf + wildoff; p < s; ++p)
	اگر (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    برای (p = s; p < e; ++p)
	اگر (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, پوچ, FALSE);
    اگر (pat == پوچ)
    {
	vim_free(buf);
	بازگشت 0;
    }

    // compile the regexp into a program
    اگر (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    اگر (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    اگر (regmatch.regprog == پوچ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	بازگشت 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != پوچ;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    اگر (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((نویسه *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(تهی)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((نویسه *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, پوچ);
    اگر (wn != پوچ)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_مقدار);

    وقتی (ok)
    {
	p = utf16_to_enc(wfb.cFileName, پوچ);   // p is allocated here

	اگر (p == پوچ)
	    بشکن;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	اگر (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = پوچ;
	وگرنه
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, پوچ);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	اگر ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == پوچ
		  || (regmatch.regprog != پوچ
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != پوچ
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((نویسه *)s, buflen - len, "%s", p);
	    اگر (len + 1 < buflen)
	    {
		اگر (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((نویسه *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (تهی)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((نویسه *)buf + len, buflen - len, "%s", path_end);
		اگر (mch_has_exp_wildcard(path_end))
		{
		    اگر (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(تهی)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		وگرنه
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    اگر (*path_end != 0)
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
    اگر (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   اندازه(char_u *), pstrcmp);
    بازگشت matches;
}
#endif // MSWIN

#اگر (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    ایستا صحیح
pstrcmp(ثابت تهی *a, ثابت تهی *b)
{
    بازگشت (pathcmp(*(نویسه **)a, *(نویسه **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    صحیح
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    صحیح		flags,		// EW_* flags
    صحیح		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    صحیح		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    صحیح		starts_with_dot;
    صحیح		matches;
    size_t	buflen;
    size_t	len;
    صحیح		starstar = FALSE;
    ایستا صحیح	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    ساختار dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    اگر (stardepth > 0)
    {
	ui_breakcheck();
	اگر (got_int)
	    بازگشت 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    اگر (buf == پوچ)
	بازگشت 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = پوچ;
    path_end = path;
    وقتی (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	اگر (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	وگرنه اگر (*path_end == '/')
	{
	    اگر (e != پوچ)
		بشکن;
	    s = p + 1;
	}
	وگرنه اگر (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != پوچ
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	اگر (has_mbyte)
	{
	    صحیح charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	وگرنه
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    برای (p = buf + wildoff; p < s; ++p)
	اگر (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    برای (p = s; p < e; ++p)
	اگر (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, پوچ, FALSE);
    اگر (pat == پوچ)
    {
	vim_free(buf);
	بازگشت 0;
    }

    // compile the regexp into a program
    اگر (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    وگرنه
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    اگر (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    اگر (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    اگر (regmatch.regprog == پوچ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	بازگشت 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    اگر (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((نویسه *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(تهی)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (نویسه *)buf);

    // Find all matching entries
    اگر (dirp != پوچ)
    {
	وقتی (!got_int)
	{
	    dp = readdir(dirp);
	    اگر (dp == پوچ)
		بشکن;
	    len = (size_t)(s - buf);
	    اگر ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != پوچ && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((نویسه *)s, buflen - len, "%s", dp->d_name);
		اگر (len + 1 >= buflen)
		    ادامه;

		اگر (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((نویسه *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (تهی)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((نویسه *)buf + len, buflen - len, "%s", path_end);
		اگر (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    اگر (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(تهی)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		وگرنه
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    اگر (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    اگر ((flags & EW_ALLLINKS) ? mch_lstat((نویسه *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			اگر (precomp_buf)
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
    اگر (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   اندازه(char_u *), pstrcmp);
    بازگشت matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    ایستا صحیح
has_env_var(char_u *p)
{
    برای ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	وگرنه اگر (vim_strchr((char_u *)
#اگر defined(MSWIN)
				    "$%"
#وگرنه
				    "$"
#endif
					, *p) != پوچ)
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    ایستا صحیح
has_special_wildchar(char_u *p)
{
    برای ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	اگر (*p == '\r' || *p == '\n')
	    بشکن;
	// Allow for escaping.
	اگر (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	وگرنه اگر (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != پوچ)
	{
	    // A { must be followed by a matching }.
	    اگر (*p == '{' && vim_strchr(p, '}') == پوچ)
		ادامه;
	    // A quote and backtick must be followed by another one.
	    اگر ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == پوچ)
		ادامه;
	    بازگشت TRUE;
	}
    }
    بازگشت FALSE;
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
    صحیح
gen_expand_wildcards(
    صحیح		رقم_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    صحیح		*رقم_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    صحیح		flags)		// EW_* flags
{
    صحیح			i;
    garray_T		ga;
    char_u		*p;
    ایستا صحیح		recursive = FALSE;
    صحیح			add_pat;
    صحیح			retval = OK;
    صحیح			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    اگر (recursive)
#ifdef SPECIAL_WILDCHAR
	بازگشت mch_expand_wildcards(رقم_pat, pat, رقم_file, file, flags);
#وگرنه
	بازگشت FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    برای (i = 0; i < رقم_pat; i++)
    {
	اگر (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    بازگشت mch_expand_wildcards(رقم_pat, pat, رقم_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, اندازه(char_u *), 30);

    برای (i = 0; i < رقم_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	اگر (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    اگر (add_pat == -1)
		retval = FAIL;
	}
	وگرنه
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    اگر ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		اگر (p == پوچ)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		وگرنه اگر (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(رقم_pat, pat, رقم_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    بازگشت i;
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
	    اگر (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		اگر ((flags & (EW_PATH | EW_CDPATH))
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
		وگرنه
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	اگر (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    اگر (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    وگرنه
		addfile(&ga, t, flags);

	    اگر (t != p)
		vim_free(t);
	}

	اگر (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	اگر (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    اگر (retval == FAIL)
	ga_clear_strings(&ga);

    *رقم_file = ga.ga_len;
    *file = (ga.ga_data != پوچ) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    بازگشت ((flags & EW_EMPTYOK) || ga.ga_data != پوچ) ? retval : FAIL;
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
    تهی
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    صحیح		flags)
{
    char_u	*p;
    صحیح		isdir;
    stat_T	sb;

#ifdef MSWIN
    اگر (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	اگر (perm < 0)
	    بازگشت;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    وگرنه
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	اگر (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((نویسه *)f, &sb) < 0 : mch_getperm(f) < 0))
	    بازگشت;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    اگر (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != پوچ)
	بازگشت;
#endif

    اگر ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	بازگشت;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    اگر (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, پوچ, !(flags & EW_SHELLCMD)))
	بازگشت;

    // Make room for another item in the file list.
    اگر (ga_grow(gap, 1) == FAIL)
	بازگشت;

    p = alloc(STRLEN(f) + 1 + isdir);
    اگر (p == پوچ)
	بازگشت;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    اگر (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    تهی
FreeWild(صحیح شمار, char_u **files)
{
    اگر (شمار <= 0 || files == پوچ)
	بازگشت;
    وقتی (شمار--)
	vim_free(files[شمار]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    صحیح
pathcmp(ثابت نویسه *p, ثابت نویسه *q, صحیح maxlen)
{
    صحیح		i, j;
    صحیح		c1, c2;
    ثابت نویسه	*s = پوچ;

    برای (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	اگر (c1 == NUL)
	{
	    اگر (c2 == NUL)  // full match
		بازگشت 0;
	    s = q;
	    i = j;
	    بشکن;
	}

	// End of "q": check if "p" just has a slash.
	اگر (c2 == NUL)
	{
	    s = p;
	    بشکن;
	}

	اگر ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    اگر (vim_ispathsep(c1))
		بازگشت -1;
	    اگر (vim_ispathsep(c2))
		بازگشت 1;
	    بازگشت p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    اگر (s == پوچ)	// "i" or "j" ran into "maxlen"
	بازگشت 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    اگر (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#وگرنه
	    && c1 == '/'
#endif
       )
	بازگشت 0;   // match with trailing slash
    اگر (s == q)
	بازگشت -1;	    // no match
    بازگشت 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    صحیح
vim_isAbsName(char_u *name)
{
    بازگشت (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    صحیح		len,
    صحیح		force)	    // force expansion even when already absolute
{
    صحیح		retval = OK;
    صحیح		url;

    *buf = NUL;
    اگر (fname == پوچ)
	بازگشت FAIL;

    url = path_with_url(fname);
    اگر (!url)
	retval = mch_FullName(fname, buf, len, force);
    اگر (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#اگر defined(MSWIN)
    slash_adjust(buf);
#endif
    بازگشت retval;
}
