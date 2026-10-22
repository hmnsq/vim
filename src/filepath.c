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

#ಅನ್ತರ್ಭಾವ "vim.h"

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    ಪೂರ್ಣಾಙ್ಕ		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * ಪರಿಮಾಣ(*newbuf));
    ಯದಿ (newbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    wfname = enc_to_utf16(*fnamep, ರಿಕ್ತ);
    ಯದಿ (wfname == ರಿಕ್ತ)
    {
	vim_free(newbuf);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    ಯದಿ (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * ಪರಿಮಾಣ(*newbuf));
	ಯದಿ (newbuf == ರಿಕ್ತ)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    ಯದಿ (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, ರಿಕ್ತ);

	ಯದಿ (p != ರಿಕ್ತ)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	ಅನ್ಯಥಾ
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = ರಿಕ್ತ;
    char_u	*short_fname = ರಿಕ್ತ;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    ಯದಿ (save_fname == ರಿಕ್ತ)
    {
	retval = FAIL;
	ಗಚ್ಛ theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    ಪ್ರತಿ (;;)
    {
	// go back one path-separator
	ಯಾವತ್ (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	ಯದಿ (endp <= save_fname)
	    ಭಙ್ಗ;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	ಯದಿ (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    ಗಚ್ಛ theend;
	}
	*endp = ch;	// preserve the string

	ಯದಿ (len > 0)
	    ಭಙ್ಗ;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    ಯದಿ (len > 0)
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
	ಯದಿ (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    ಯದಿ (*fname == ರಿಕ್ತ)
	    {
		retval = FAIL;
		ಗಚ್ಛ theend;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    ಯದಿ (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = ರಿಕ್ತ;
	}

	// concat the not-shortened part of the path
	ಯದಿ ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    ಪೂರ್ಣಾಙ್ಕ		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    ಪೂರ್ಣಾಙ್ಕ		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    ಪ್ರತಿ (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	ಯದಿ (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    ಯದಿ (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    ಅನ್ಯಥಾ
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    ಯದಿ (tfname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    len = tflen = STRLEN(tfname);

    ಯದಿ (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	ಯದಿ (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    ಪ್ರತಿ (p = tfname + len - 1; p >= tfname; --p)
    {
	ಯದಿ (has_mbyte)
	    p -= mb_head_off(tfname, p);
	ಯದಿ (vim_ispathsep(*p))
	{
	    ಯದಿ (sepcount == 0 || (hasTilde && sepcount == 1))
		ಭಙ್ಗ;
	    ಅನ್ಯಥಾ
		sepcount --;
	}
    }
    ಯದಿ (hasTilde)
    {
	--p;
	ಯದಿ (p >= tfname)
	    *p = '~';
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    ಪೂರ್ಣಾಙ್ಕ
modify_fname(
    char_u	*src,		// string with modifiers
    ಪೂರ್ಣಾಙ್ಕ		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    ಪೂರ್ಣಾಙ್ಕ		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		has_fullname = 0;
    ಪೂರ್ಣಾಙ್ಕ		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    ಪೂರ್ಣಾಙ್ಕ		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    ಯದಿ (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	ಯದಿ ((*fnamep)[0] == '~'
#ಯದಿ !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    ಯದಿ (*fnamep == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	ಪ್ರತಿ (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    ಯದಿ (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		ಭಙ್ಗ;
	}

	// FullName_save() is slow, don't use it when not needed.
	ಯದಿ (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ಯದಿ (*fnamep == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ -1;
	}

#ifdef MSWIN
# ಯದಿ _WIN32_WINNT >= 0x0500
	ಯದಿ (vim_strchr(*fnamep, '~') != ರಿಕ್ತ)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, ರಿಕ್ತ);
	    WCHAR buf[_MAX_PATH];

	    ಯದಿ (wfname != ರಿಕ್ತ)
	    {
		ಯದಿ (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, ರಿಕ್ತ);

		    ಯದಿ (q != ರಿಕ್ತ)
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
	ಯದಿ (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ಯದಿ (*fnamep == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    ಯಾವತ್ (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	ಯದಿ (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    ಅನುವರ್ತ;
	}
	pbuf = ರಿಕ್ತ;
	// Need full path first (use expand_env() to remove a "~/")
	ಯದಿ (!has_fullname && !has_homerelative)
	{
	    ಯದಿ (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    ಅನ್ಯಥಾ
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	ಅನ್ಯಥಾ
	    p = *fnamep;

	has_fullname = 0;

	ಯದಿ (p != ರಿಕ್ತ)
	{
	    size_t  dirnamelen = 0;

	    ಯದಿ (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		ಯದಿ (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    ಯದಿ (s != ರಿಕ್ತ)
		    {
			dirnamelen = home_replace(ರಿಕ್ತ, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		ಯದಿ (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		ಯದಿ (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    ಯದಿ (vim_ispathsep(*p))
		    {
			ಯಾವತ್ (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			ಯದಿ (pbuf != ರಿಕ್ತ)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = ರಿಕ್ತ;
			}
		    }
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		dirnamelen = home_replace(ರಿಕ್ತ, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		ಯದಿ (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    ಯದಿ (s != ರಿಕ್ತ)
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
    ಯಾವತ್ (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	ಯಾವತ್ (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	ಯದಿ (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	ಯದಿ (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    ಯದಿ (p == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	ಅನ್ಯಥಾ
	{
	    ಯಾವತ್ (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    ಯದಿ (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    ಯದಿ (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	ಯದಿ (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    ಯದಿ (p == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	ಯದಿ (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    ಯದಿ (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    ಯದಿ (get_short_pathname(fnamep, bufp, &l) == FAIL)
		ಪ್ರತ್ಯಾಗಮ -1;

	    ಯದಿ (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		ಯದಿ (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    ಯದಿ (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    ಯಾವತ್ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	ಯದಿ (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	ಅನ್ಯಥಾ
	    s = *fnamep + *fnamelen - 1;
	ಪ್ರತಿ ( ; s > tail; --s)
	    ಯದಿ (s[0] == '.')
		ಭಙ್ಗ;
	ಯದಿ (src[*usedlen + 1] == 'e')		// :e
	{
	    ಯದಿ (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		ಪ್ರತಿ ( ; s > *fnamep; --s)
		    ಯದಿ (s[0] == ';')
			ಭಙ್ಗ;
		ಯದಿ (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*fnamep <= tail)
		*fnamelen = 0;
	}
	ಅನ್ಯಥಾ				// :r
	{
	    char_u *limit = *fnamep;

	    ಯದಿ (limit < tail)
		limit = tail;
	    ಯದಿ (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    ಯದಿ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	ಪೂರ್ಣಾಙ್ಕ	    sep;
	char_u	    *flags;
	ಪೂರ್ಣಾಙ್ಕ	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	ಯದಿ (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	ಯದಿ (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    ಯದಿ (p != ರಿಕ್ತ)
	    {
		pat = vim_strnsave(s, p - s);
		ಯದಿ (pat != ರಿಕ್ತ)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    ಯದಿ (p != ರಿಕ್ತ)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			ಯದಿ (sub != ರಿಕ್ತ && str != ರಿಕ್ತ)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, ರಿಕ್ತ, flags, &slen);
			    ಯದಿ (s != ರಿಕ್ತ)
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
	    ಯದಿ (didit)
		ಗಚ್ಛ repeat;
	}
    }

    ಯದಿ (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	ಯದಿ (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	ಯದಿ (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    ಪ್ರತ್ಯಾಗಮ valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
shorten_dir_len(char_u *str, ಪೂರ್ಣಾಙ್ಕ trim_len)
{
    char_u	*tail, *s, *d;
    ಪೂರ್ಣಾಙ್ಕ		skip = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    ಪ್ರತಿ (s = str; ; ++s)
    {
	ಯದಿ (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    ಯದಿ (*s == NUL)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (!skip)
	{
	    *d++ = *s;			// copy next char
	    ಯದಿ (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		ಯದಿ (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    ಯದಿ (has_mbyte)
	    {
		ಪೂರ್ಣಾಙ್ಕ l = mb_ptr2len(s);

		ಯಾವತ್ (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    ಶೂನ್ಯ
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    ಪೂರ್ಣಾಙ್ಕ
file_is_readable(char_u *fname)
{
    ಪೂರ್ಣಾಙ್ಕ		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    ಯದಿ (*fname == NUL || mch_isdir_or_missing(fname))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ ((fd = mch_open((ವರ್ಣ *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    ಶೂನ್ಯ
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	ಯದಿ (in_vim9script())
	    (ಶೂನ್ಯ) check_for_string_arg(argvars, 0);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    ಯದಿ (cwd != ರಿಕ್ತ)
    {
	ಯದಿ (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	ಯದಿ (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "scope", s);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_localdir != ರಿಕ್ತ)
	scope = CDSCOPE_WINDOW;
    ಅನ್ಯಥಾ ಯದಿ (curtab->tp_localdir != ರಿಕ್ತ)
	scope = CDSCOPE_TABPAGE;

    ಯದಿ (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    ಶೂನ್ಯ
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);
    ಯದಿ (name == ರಿಕ್ತ || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    ಅನ್ಯಥಾ
	flags = (char_u *)"";

    ಯದಿ (*flags == NUL)
	// delete a file
	rettv->vval.v_ಸಂಖ್ಯೆ = mch_remove(name) == 0 ? 0 : -1;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_ಸಂಖ್ಯೆ = mch_rmdir(name) == 0 ? 0 : -1;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_ಸಂಖ್ಯೆ = delete_recursive(name);
    ಅನ್ಯಥಾ
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    ಶೂನ್ಯ
f_executable(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_ಸಂಖ್ಯೆ = mch_can_exe(tv_get_string(&argvars[0]), ರಿಕ್ತ, TRUE);
}

/*
 * "exepath()" function
 */
    ಶೂನ್ಯ
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = ರಿಕ್ತ;

    ಯದಿ (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    (ಶೂನ್ಯ)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    ಶೂನ್ಯ
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    rettv->vval.v_ಸಂಖ್ಯೆ = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    ಶೂನ್ಯ
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    rettv->vval.v_ಸಂಖ್ಯೆ = filewritable(tv_get_string(&argvars[0]));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    ಪೂರ್ಣಾಙ್ಕ		find_what)
{
    char_u	*fname;
    char_u	*fresult = ರಿಕ್ತ;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 1;
    ಪೂರ್ಣಾಙ್ಕ		first = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    rettv->vval.v_string = ರಿಕ್ತ;
    rettv->v_type = VAR_STRING;
    ಯದಿ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    fname = tv_get_string(&argvars[0]);

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	ಯದಿ (p == ರಿಕ್ತ)
	    error = TRUE;
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (*p != NUL)
		path = p;

	    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
		ಎಣಿಕೆ = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	}
    }

    ಯದಿ (ಎಣಿಕೆ < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    ಯದಿ (*fname != NUL && !error)
    {
	char_u	*file_to_find = ರಿಕ್ತ;
	ವರ್ಣ	*ಹುಡುಕು_ctx = ರಿಕ್ತ;

	ಕುರು
	{
	    ಯದಿ (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : ರಿಕ್ತ,
					       first ? (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &ಹುಡುಕು_ctx);
	    first = FALSE;

	    ಯದಿ (fresult != ರಿಕ್ತ && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} ಯಾವತ್ ((rettv->v_type == VAR_LIST || --ಎಣಿಕೆ > 0) && fresult != ರಿಕ್ತ);

	vim_free(file_to_find);
	vim_findfile_cleanup(ಹುಡುಕು_ctx);
    }

    ಯದಿ (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    ಶೂನ್ಯ
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    ಶೂನ್ಯ
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    ಶೂನ್ಯ
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = ರಿಕ್ತ;
    char_u	buf[NUMBUFLEN];

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    ಯದಿ (mods == ರಿಕ್ತ || fname == ರಿಕ್ತ)
	fname = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	len = STRLEN(fname);
	ಯದಿ (mods != ರಿಕ್ತ && *mods != NUL)
	    (ಶೂನ್ಯ)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    ಯದಿ (fname == ರಿಕ್ತ)
	rettv->vval.v_string = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = ರಿಕ್ತ;
    tabpage_T	*tp = ರಿಕ್ತ;
    char_u	*cwd;
    ಪೂರ್ಣಾಙ್ಕ		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ
	    && argvars[0].vval.v_ಸಂಖ್ಯೆ == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    ಅನ್ಯಥಾ
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    ಯದಿ (wp != ರಿಕ್ತ && wp->w_localdir != ರಿಕ್ತ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    ಅನ್ಯಥಾ ಯದಿ (tp != ರಿಕ್ತ && tp->tp_localdir != ರಿಕ್ತ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    ಅನ್ಯಥಾ ಯದಿ (wp != ರಿಕ್ತ || tp != ರಿಕ್ತ || global)
    {
	ಯದಿ (globaldir != ರಿಕ್ತ && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	ಅನ್ಯಥಾ
	{
	    cwd = alloc(MAXPATHL);
	    ಯದಿ (cwd != ರಿಕ್ತ)
	    {
		ಯದಿ (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    ಯದಿ (rettv->vval.v_string != ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ		    i;

    ಪ್ರತಿ (i = 0; i < 9; i++)
    {
	ಯದಿ (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	ಅನ್ಯಥಾ
	    perm[i] = '-';
    }
    ಪ್ರತ್ಯಾಗಮ perm;
}

/*
 * "getfperm({fname})" function
 */
    ಶೂನ್ಯ
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = ರಿಕ್ತ;
    char_u	permbuf[] = "---------";

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    ಶೂನ್ಯ
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    fname = tv_get_string(&argvars[0]);
    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st) >= 0)
    {
	ಯದಿ (S_ISDIR(st.st_mode))
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	ಅನ್ಯಥಾ
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    ಯದಿ ((off_T)rettv->vval.v_ಸಂಖ್ಯೆ != (off_T)st.st_size)
		rettv->vval.v_ಸಂಖ್ಯೆ = -2;
	}
    }
    ಅನ್ಯಥಾ
	  rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}

/*
 * "getftime({fname})" function
 */
    ಶೂನ್ಯ
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    fname = tv_get_string(&argvars[0]);
    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st) >= 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)st.st_mtime;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    ವರ್ಣ    *t;

    ಯದಿ (S_ISREG(st->st_mode))
	t = "file";
    ಅನ್ಯಥಾ ಯದಿ (S_ISDIR(st->st_mode))
	t = "dir";
    ಅನ್ಯಥಾ ಯದಿ (S_ISLNK(st->st_mode))
	t = "link";
    ಅನ್ಯಥಾ ಯದಿ (S_ISBLK(st->st_mode))
	t = "bdev";
    ಅನ್ಯಥಾ ಯದಿ (S_ISCHR(st->st_mode))
	t = "cdev";
    ಅನ್ಯಥಾ ಯದಿ (S_ISFIFO(st->st_mode))
	t = "fifo";
    ಅನ್ಯಥಾ ಯದಿ (S_ISSOCK(st->st_mode))
	t = "socket";
    ಅನ್ಯಥಾ
	t = "other";
    ಪ್ರತ್ಯಾಗಮ (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    ಶೂನ್ಯ
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = ರಿಕ್ತ;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ಯದಿ (mch_lstat((ವರ್ಣ *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    ಶೂನ್ಯ
f_glob(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	ಪ್ರತ್ಯಾಗಮ;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ಯದಿ (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, ರಿಕ್ತ);
	    ಯದಿ (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    ಯದಿ (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	ಯದಿ (p_wic)
	    options += WILD_ICASE;
	ಯದಿ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     ರಿಕ್ತ, options, WILD_ALL);
	ಅನ್ಯಥಾ ಯದಿ (rettv_list_alloc(rettv) == OK)
	{
	  ಪೂರ್ಣಾಙ್ಕ i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						ರಿಕ್ತ, options, WILD_ALL_KEEP);
	  ಪ್ರತಿ (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    ಅನ್ಯಥಾ
	rettv->vval.v_string = ರಿಕ್ತ;
}

/*
 * "glob2regpat()" function
 */
    ಶೂನ್ಯ
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == ರಿಕ್ತ)
			 ? ರಿಕ್ತ : file_pat_to_reg_pat(pat, ರಿಕ್ತ, ರಿಕ್ತ, FALSE);
}

/*
 * "globpath()" function
 */
    ಶೂನ್ಯ
f_globpath(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	ಪ್ರತ್ಯಾಗಮ;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	ಯದಿ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    ಯದಿ (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, ರಿಕ್ತ);
	    ಯದಿ (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    ಯದಿ (file != ರಿಕ್ತ && !error)
    {
	ga_init2(&ga, ಪರಿಮಾಣ(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	ಯದಿ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	ಅನ್ಯಥಾ ಯದಿ (rettv_list_alloc(rettv) == OK)
	    ಪ್ರತಿ (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    ಅನ್ಯಥಾ
	rettv->vval.v_string = ರಿಕ್ತ;
}

/*
 * "isdirectory()" function
 */
    ಶೂನ್ಯ
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    ಶೂನ್ಯ
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mkdir_recurse(char_u *dir, ಪೂರ್ಣಾಙ್ಕ prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    ಪೂರ್ಣಾಙ್ಕ		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    ಯದಿ (p <= get_past_head(dir))
	ಪ್ರತ್ಯಾಗಮ OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    ಯದಿ (updir == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (mch_isdir(updir))
	r = OK;
    ಅನ್ಯಥಾ ಯದಿ (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	ಯದಿ (r == OK && created != ರಿಕ್ತ && *created == ರಿಕ್ತ)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * "mkdir()" function
 */
    ಶೂನ್ಯ
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		prot = 0755;
    ಪೂರ್ಣಾಙ್ಕ		defer = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		defer_recurse = FALSE;
    char_u	*created = ರಿಕ್ತ;

    rettv->vval.v_ಸಂಖ್ಯೆ = FAIL;
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    dir = tv_get_string_buf(&argvars[0], buf);
    ಯದಿ (*dir == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], ರಿಕ್ತ);
	    ಯದಿ (prot == -1)
		ಪ್ರತ್ಯಾಗಮ;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != ರಿಕ್ತ;
	defer_recurse = vim_strchr(arg2, 'R') != ರಿಕ್ತ;
	ಯದಿ ((defer || defer_recurse) && !can_add_defer())
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (vim_strchr(arg2, 'p') != ರಿಕ್ತ)
	{
	    ಯದಿ (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_ಸಂಖ್ಯೆ = OK;
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : ರಿಕ್ತ);
	}
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    ಯದಿ (rettv->vval.v_ಸಂಖ್ಯೆ == OK
				&& created == ರಿಕ್ತ && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    ಯದಿ (created != ರಿಕ್ತ)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	ಯದಿ (tv[0].vval.v_string == ರಿಕ್ತ || tv[1].vval.v_string == ರಿಕ್ತ
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
    ಶೂನ್ಯ
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		trim_len = 1;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	ಯದಿ (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    ಯದಿ (p == ರಿಕ್ತ)
	rettv->vval.v_string = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	ಯದಿ (p != ರಿಕ್ತ)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
checkitem_common(ಶೂನ್ಯ *context, char_u *name, dict_T *dict)
{
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    ಯದಿ (name != ರಿಕ್ತ)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    ಅನ್ಯಥಾ
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    ಯದಿ (eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, FALSE, argv, 1, ರಿಕ್ತ, &rettv) == FAIL)
	ಗಚ್ಛ theend;

    // We want to use -1, but also true/false should be allowed.
    ಯದಿ (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_ಸಂಖ್ಯೆ;
	rettv.vval.v_ಸಂಖ್ಯೆ = rettv.vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE;
    }
    retval = tv_get_ಸಂಖ್ಯೆ_chk(&rettv, &error);
    ಯದಿ (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    ಯದಿ (name != ರಿಕ್ತ)
	set_vim_var_string(VV_VAL, ರಿಕ್ತ, 0);
    ಅನ್ಯಥಾ
	set_vim_var_dict(VV_VAL, ರಿಕ್ತ);
    restore_vimvar(VV_VAL, &save_val);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
readdir_checkitem(ಶೂನ್ಯ *context, ಶೂನ್ಯ *item)
{
    char_u	*name = (char_u *)item;

    ಪ್ರತ್ಯಾಗಮ checkitem_common(context, name, ರಿಕ್ತ);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
readdirex_dict_arg(typval_T *argvars, ಪೂರ್ಣಾಙ್ಕ *cmp)
{
    char_u     *compare;

    ಯದಿ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (dict_has_ಕೀಲಿ(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    ಅನ್ಯಥಾ
    {
	semsg(_(e_dictionary_ಕೀಲಿ_str_required), "sort");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * "readdir()" function
 */
    ಶೂನ್ಯ
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		sort = READDIR_SORT_BYTE;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    path = tv_get_string(&argvars[0]);
    ಅಭಿವ್ಯಕ್ತಿ = &argvars[1];

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ret = readdir_core(&ga, path, FALSE, (ಶೂನ್ಯ *)ಅಭಿವ್ಯಕ್ತಿ,
	    (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_UNKNOWN) ? ರಿಕ್ತ : readdir_checkitem, sort);
    ಯದಿ (ret == OK)
    {
	ಪ್ರತಿ (i = 0; i < ga.ga_len; i++)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
readdirex_checkitem(ಶೂನ್ಯ *context, ಶೂನ್ಯ *item)
{
    dict_T	*dict = (dict_T*)item;

    ಪ್ರತ್ಯಾಗಮ checkitem_common(context, ರಿಕ್ತ, dict);
}

/*
 * "readdirex()" function
 */
    ಶೂನ್ಯ
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    char_u	*path;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		sort = READDIR_SORT_BYTE;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    path = tv_get_string(&argvars[0]);
    ಅಭಿವ್ಯಕ್ತಿ = &argvars[1];

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ret = readdir_core(&ga, path, TRUE, (ಶೂನ್ಯ *)ಅಭಿವ್ಯಕ್ತಿ,
	    (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_UNKNOWN) ? ರಿಕ್ತ : readdirex_checkitem, sort);
    ಯದಿ (ret == OK)
    {
	ಪ್ರತಿ (i = 0; i < ga.ga_len; i++)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
read_file_or_blob(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ always_blob)
{
    ಪೂರ್ಣಾಙ್ಕ		ದ್ವಿಮಾನ = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		blob = always_blob;
    ಪೂರ್ಣಾಙ್ಕ		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    ಪೂರ್ಣಾಙ್ಕ		io_size = ಪರಿಮಾಣ(buf);
    ಪೂರ್ಣಾಙ್ಕ		readlen;		// size of last fread()
    char_u	*prev	 = ರಿಕ್ತ;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (always_blob)
	{
	    offset = (off_T)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		ದ್ವಿಮಾನ = TRUE;
	    ಯದಿ (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
	}
    }

    ಯದಿ ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    ಯದಿ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (*fname == NUL || (fd = mch_fopen((ವರ್ಣ *)fname, READBIN)) == ರಿಕ್ತ)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (blob)
    {
	ಯದಿ (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯಾವತ್ (cnt < maxline || maxline < 0)
    {
	readlen = (ಪೂರ್ಣಾಙ್ಕ)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	ಪ್ರತಿ (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || ದ್ವಿಮಾನ));
		++p)
	{
	    ಯದಿ (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= ರಿಕ್ತ;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		ಯದಿ (readlen > 0 && !ದ್ವಿಮಾನ)
		{
		    ಯಾವತ್ (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    ಯದಿ (len == 0)
			ಯಾವತ್ (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		ಯದಿ (prevlen == 0)
		    s = vim_strnsave(start, len);
		ಅನ್ಯಥಾ
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    ಯದಿ ((s = vim_realloc(prev, prevlen + len + 1)) != ರಿಕ್ತ)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = ರಿಕ್ತ; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		ಯದಿ (s == ರಿಕ್ತ)
		{
		    do_outofmem_ಸಂದೇಶ((long_u) prevlen + len + 1);
		    failed = TRUE;
		    ಭಙ್ಗ;
		}

		ಯದಿ ((li = listitem_alloc()) == ರಿಕ್ತ)
		{
		    vim_free(s);
		    failed = TRUE;
		    ಭಙ್ಗ;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		ಯದಿ ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    ಅನ್ಯಥಾ ಯದಿ (*p == 0xbf && enc_utf8 && !ದ್ವಿಮಾನ)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		ಯದಿ (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    ಯದಿ (start == dest)
			start = p + 1;
		    ಅನ್ಯಥಾ
		    {
			// have to shuffle buf to close gap
			ಪೂರ್ಣಾಙ್ಕ adjust_prevlen = 0;

			ಯದಿ (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (ಪೂರ್ಣಾಙ್ಕ)(buf - dest);
			    dest = buf;
			}
			ಯದಿ (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	ಯದಿ (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    ಭಙ್ಗ;
	ಯದಿ (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    ಯದಿ (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		ಯದಿ (prevsize == 0)
		    prevsize = (long)(p - start);
		ಅನ್ಯಥಾ
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		ಯದಿ (newprev == ರಿಕ್ತ)
		{
		    do_outofmem_ಸಂದೇಶ((long_u)prevsize);
		    failed = TRUE;
		    ಭಙ್ಗ;
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
    ಯದಿ (!failed && maxline < 0)
	ಯಾವತ್ (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    ಯದಿ (failed)
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
    ಶೂನ್ಯ
f_readblob(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    ಶೂನ್ಯ
f_readfile(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    ಶೂನ್ಯ
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = ರಿಕ್ತ;
# endif

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = ರಿಕ್ತ;

	v = mch_resolve_path(p, TRUE);
	ಯದಿ (v != ರಿಕ್ತ)
	    rettv->vval.v_string = v;
	ಅನ್ಯಥಾ
	    rettv->vval.v_string = vim_strsave(p);
    }
# ಅನ್ಯಥಾ
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	ಪೂರ್ಣಾಙ್ಕ	len;
	char_u	*remain = ರಿಕ್ತ;
	char_u	*q;
	ಪೂರ್ಣಾಙ್ಕ	is_relative_to_current = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	has_trailing_pathsep = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	limit = 100;

	p = vim_strsave(p);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಗಚ್ಛ fail;
	ಯದಿ (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	ಯದಿ (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	ಯದಿ (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    vim_free(p);
	    vim_free(remain);
	    ಗಚ್ಛ fail;
	}

	ಪ್ರತಿ (;;)
	{
	    ಪ್ರತಿ (;;)
	    {
		len = readlink((ವರ್ಣ *)p, (ವರ್ಣ *)buf, MAXPATHL);
		ಯದಿ (len <= 0)
		    ಭಙ್ಗ;
		buf[len] = NUL;

		ಯದಿ (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = ರಿಕ್ತ;
		    ಗಚ್ಛ fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		ಯದಿ (remain == ರಿಕ್ತ && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		ಯದಿ (*q != NUL)
		{
		    ಯದಿ (remain == ರಿಕ್ತ)
			remain = vim_strsave(q - 1);
		    ಅನ್ಯಥಾ
		    {
			cpy = concat_str(q - 1, remain);
			ಯದಿ (cpy != ರಿಕ್ತ)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		ಯದಿ (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		ಯದಿ (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    ಯದಿ (cpy != ರಿಕ್ತ)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    ಯದಿ (remain == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    ಯದಿ (cpy != ರಿಕ್ತ)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    ಯದಿ (*q != NUL)
		STRMOVE(remain, q - 1);
	    ಅನ್ಯಥಾ
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	ಯದಿ (!vim_ispathsep(*p))
	{
	    ಯದಿ (is_relative_to_current
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
		ಯದಿ (cpy != ರಿಕ್ತ)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		ಯಾವತ್ (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		ಯದಿ (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	ಯದಿ (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    ಯದಿ (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    ಕುರು
    {
	ಯದಿ (x == 'Z')
	    x = '0';
	ಅನ್ಯಥಾ ಯದಿ (x == '9')
	    x = 'A';
	ಅನ್ಯಥಾ
	    ++x;
    } ಯಾವತ್ (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    ಶೂನ್ಯ
f_writefile(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		ದ್ವಿಮಾನ = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		append = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		defer = FALSE;
# ifdef HAVE_FSYNC
    ಪೂರ್ಣಾಙ್ಕ		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    ಪೂರ್ಣಾಙ್ಕ		ret = 0;
    listitem_T	*li;
    list_T	*list = ರಿಕ್ತ;
    blob_T	*blob = ರಿಕ್ತ;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	ಯದಿ (list == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
	    ಯದಿ (tv_get_string_chk(&li->li_tv) == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	ಯದಿ (blob == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	ಯದಿ (arg2 == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (vim_strchr(arg2, 'b') != ರಿಕ್ತ)
	    ದ್ವಿಮಾನ = TRUE;
	ಯದಿ (vim_strchr(arg2, 'a') != ರಿಕ್ತ)
	    append = TRUE;
	ಯದಿ (vim_strchr(arg2, 'D') != ರಿಕ್ತ)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	ಯದಿ (vim_strchr(arg2, 's') != ರಿಕ್ತ)
	    do_fsync = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr(arg2, 'S') != ರಿಕ್ತ)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (defer && !can_add_defer())
	ಪ್ರತ್ಯಾಗಮ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    ಯದಿ (*fname == NUL || (fd = mch_fopen((ವರ್ಣ *)fname,
				      append ? APPENDBIN : WRITEBIN)) == ರಿಕ್ತ)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    ಯದಿ (tv.vval.v_string == ರಿಕ್ತ
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(ಶೂನ್ಯ)mch_remove(fname);
	    }
	}

	ಯದಿ (ret == 0)
	{
	    ಯದಿ (blob)
	    {
		ಯದಿ (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (write_list(fd, list, ದ್ವಿಮಾನ) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    ಯದಿ (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = ret;
}

#endif // FEAT_EVAL

#ಯದಿ defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    ಪೂರ್ಣಾಙ್ಕ		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    ಸ್ಥಿರ char_u	*last_dir = ರಿಕ್ತ;    // last used directory
    char_u		*tofree = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    ಯದಿ (title == ರಿಕ್ತ || *title == NUL)
    {
	ಯದಿ (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	ಅನ್ಯಥಾ ಯದಿ (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	ಅನ್ಯಥಾ
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    ಯದಿ ((initdir == ರಿಕ್ತ || *initdir == NUL) && dflt != ರಿಕ್ತ && *dflt != NUL)
    {
	ಯದಿ (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    ಯದಿ (tofree != ರಿಕ್ತ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    ಯದಿ (initdir == ರಿಕ್ತ || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	ಯದಿ (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	ಅನ್ಯಥಾ ಯದಿ (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != ರಿಕ್ತ && buf->b_ffname != ರಿಕ್ತ)
	{
	    ಯದಿ (dflt == ರಿಕ್ತ || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    ಯದಿ (tofree != ರಿಕ್ತ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	ಅನ್ಯಥಾ ಯದಿ (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    ಯದಿ (gui.in_use)		// when this changes, also adjust f_has()!
    {
	ಯದಿ (filter == ರಿಕ್ತ
#  ifdef FEAT_EVAL
		&& (filter = get_var_ಮೌಲ್ಯ((char_u *)"b:browsefilter")) == ರಿಕ್ತ
		&& (filter = get_var_ಮೌಲ್ಯ((char_u *)"g:browsefilter")) == ರಿಕ್ತ
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	ಯದಿ (flags & BROWSE_DIR)
	{
#  ಯದಿ defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  ಅನ್ಯಥಾ
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  ಯದಿ !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    ಯದಿ (fname != ರಿಕ್ತ && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		ಯದಿ (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	ಅನ್ಯಥಾ
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    ಅನ್ಯಥಾ
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = ರಿಕ್ತ;
    }

    // keep the directory for next time
    ಯದಿ (fname != ರಿಕ್ತ)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	ಯದಿ (last_dir != ರಿಕ್ತ && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    ಯದಿ (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		ಯದಿ (last_dir != ರಿಕ್ತ)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    ಪ್ರತ್ಯಾಗಮ fname;
}
#endif

#ಯದಿ defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    ಶೂನ್ಯ
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    ಪೂರ್ಣಾಙ್ಕ		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    save = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    ಯದಿ (error || title == ರಿಕ್ತ || initdir == ರಿಕ್ತ || defname == ರಿಕ್ತ)
	rettv->vval.v_string = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, ರಿಕ್ತ, initdir, ರಿಕ್ತ, curbuf);
# ಅನ್ಯಥಾ
    rettv->vval.v_string = ರಿಕ್ತ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    ಶೂನ್ಯ
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    ಯದಿ (title == ರಿಕ್ತ || initdir == ರಿಕ್ತ)
	rettv->vval.v_string = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, ರಿಕ್ತ, ರಿಕ್ತ, initdir, ರಿಕ್ತ, curbuf);
# ಅನ್ಯಥಾ
    rettv->vval.v_string = ರಿಕ್ತ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    ಶೂನ್ಯ
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;

    ಯದಿ (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    from = tv_get_string(&argvars[0]);

    ಯದಿ (mch_lstat((ವರ್ಣ *)from, &st) >= 0
	&& (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)))
	rettv->vval.v_ಸಂಖ್ಯೆ = vim_copyfile(
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
    ಪೂರ್ಣಾಙ್ಕ		dstlen,	// maximum length of the result
    ಪೂರ್ಣಾಙ್ಕ		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    ಯದಿ (src == ರಿಕ್ತ)
    {
	*dst = NUL;
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    ಯದಿ (buf != ರಿಕ್ತ && buf->b_help)
	ಪ್ರತ್ಯಾಗಮ vim_snprintf_safelen((ವರ್ಣ *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    ಯದಿ (homedir != ರಿಕ್ತ)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#ಅನ್ಯಥಾ
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    ಯದಿ (homedir_env == ರಿಕ್ತ)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    ಯದಿ (homedir_env != ರಿಕ್ತ && *homedir_env == NUL)
	homedir_env = ರಿಕ್ತ;

    ಯದಿ (homedir_env != ರಿಕ್ತ && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = ರಿಕ್ತ;

	flen = STRLEN(homedir_env);
	(ಶೂನ್ಯ)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	ಯದಿ (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    ಯದಿ (homedir_env != ರಿಕ್ತ)
	envlen = STRLEN(homedir_env);

    ಯದಿ (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    ಯಾವತ್ (*src && dstlen > 0)
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
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		ಯದಿ (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		ಭಙ್ಗ;
	    }
	    ಯದಿ (p == homedir_env)
		ಭಙ್ಗ;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	ಯಾವತ್ (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	ಯಾವತ್ ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    ಯದಿ (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    ಪ್ರತ್ಯಾಗಮ (size_t)(dst - dst_start);
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
    ಯದಿ (src != ರಿಕ್ತ)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    ಯದಿ (dst != ರಿಕ್ತ)
	home_replace(buf, src, dst, len, TRUE);
    ಪ್ರತ್ಯಾಗಮ dst;
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
    ಪೂರ್ಣಾಙ್ಕ
fullpathcmp(
    char_u *s1,
    char_u *s2,
    ಪೂರ್ಣಾಙ್ಕ	    checkname,		// when both don't exist, check file names
    ಪೂರ್ಣಾಙ್ಕ	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    ಪೂರ್ಣಾಙ್ಕ		    r1, r2;

    ಯದಿ (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    ಅನ್ಯಥಾ
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((ವರ್ಣ *)exp1, &st1);
    r2 = mch_stat((ವರ್ಣ *)s2, &st2);
    ಯದಿ (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	ಯದಿ (checkname)
	{
	    ಯದಿ (fnamecmp(exp1, s2) == 0)
		ಪ್ರತ್ಯಾಗಮ FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    ಯದಿ (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		ಪ್ರತ್ಯಾಗಮ FPC_SAMEX;
	}
	ಪ್ರತ್ಯಾಗಮ FPC_NOTX;
    }
    ಯದಿ (r1 != 0 || r2 != 0)
	ಪ್ರತ್ಯಾಗಮ FPC_DIFFX;
    ಯದಿ (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	ಪ್ರತ್ಯಾಗಮ FPC_SAME;
    ಪ್ರತ್ಯಾಗಮ FPC_DIFF;
#ಅನ್ಯಥಾ
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    ಪೂರ್ಣಾಙ್ಕ	    retval = FPC_DIFF;
    ಪೂರ್ಣಾಙ್ಕ	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    ಯದಿ ((exp1 = alloc(MAXPATHL * 3)) != ರಿಕ್ತ)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	ಯದಿ (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	ಅನ್ಯಥಾ
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	ಯದಿ (r1 != OK && r2 != OK)
	{
	    ಯದಿ (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    ಅನ್ಯಥಾ
		retval = FPC_NOTX;
	}
	ಅನ್ಯಥಾ ಯದಿ (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	ಅನ್ಯಥಾ ಯದಿ (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	ಅನ್ಯಥಾ
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    ಪ್ರತ್ಯಾಗಮ retval;
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

    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"";
    ಪ್ರತಿ (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	ಯದಿ (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    ಪ್ರತ್ಯಾಗಮ p1;
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
    ಯಾವತ್ (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    ಪ್ರತ್ಯಾಗಮ t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    ಯಾವತ್ (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    ಯದಿ (*fname)
	++fname;
    ಪ್ರತ್ಯಾಗಮ fname;
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

#ಯದಿ defined(MSWIN)
    // may skip "c:"
    ಯದಿ (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    ಅನ್ಯಥಾ
	retval = path;
#ಅನ್ಯಥಾ
# ಯದಿ defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    ಯದಿ (retval == ರಿಕ್ತ)
	retval = path;
# ಅನ್ಯಥಾ	// Unix
    retval = path;
# endif
#endif

    ಯಾವತ್ (vim_ispathsep(*retval))
	++retval;

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_ispathsep(ಪೂರ್ಣಾಙ್ಕ c)
{
#ifdef UNIX
    ಪ್ರತ್ಯಾಗಮ (c == '/');	    // UNIX has ':' inside file names
#ಅನ್ಯಥಾ
# ifdef BACKSLASH_IN_FILENAME
    ಪ್ರತ್ಯಾಗಮ (c == ':' || c == '/' || c == '\\');
# ಅನ್ಯಥಾ
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    ಪ್ರತ್ಯಾಗಮ (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_ispathsep_nocolon(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ vim_ispathsep(c)
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
    ಪೂರ್ಣಾಙ್ಕ
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		retval;

    p = gettail_sep(fname);
    ಯದಿ (p == fname)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    ಪ್ರತ್ಯಾಗಮ vim_fnamencmp(x, y, MAXPATHL);
#ಅನ್ಯಥಾ
    ಯದಿ (p_fic)
	ಪ್ರತ್ಯಾಗಮ MB_STRICMP(x, y);
    ಪ್ರತ್ಯಾಗಮ STRCMP(x, y);
#endif
}

    ಪೂರ್ಣಾಙ್ಕ
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    ಪೂರ್ಣಾಙ್ಕ		cx = NUL;
    ಪೂರ್ಣಾಙ್ಕ		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	ಯದಿ ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    ಯದಿ (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    ಯಾವತ್ (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	ಯದಿ (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    ಭಙ್ಗ;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ (cx - cy);
#ಅನ್ಯಥಾ
    ಯದಿ (p_fic)
	ಪ್ರತ್ಯಾಗಮ MB_STRNICMP(x, y, len);
    ಪ್ರತ್ಯಾಗಮ STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, ಪೂರ್ಣಾಙ್ಕ sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? ಪರಿಮಾಣ(PATHSEP) : 0) + fname2len + 1);
    ಯದಿ (ret->string == ರಿಕ್ತ)
	ret->length = 0;
    ಅನ್ಯಥಾ
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	ಯದಿ (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += ಪರಿಮಾಣ(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    ಪ್ರತ್ಯಾಗಮ ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    ಶೂನ್ಯ
add_pathsep(char_u *p)
{
    size_t  plen;

    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    plen = STRLEN(p);
    ಯದಿ (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    ಪೂರ್ಣಾಙ್ಕ		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = ರಿಕ್ತ;

    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    buf = alloc(MAXPATHL);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    ಅನ್ಯಥಾ
	new_fname = vim_strsave(fname);
    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_fexists(char_u *fname)
{
    stat_T st;

    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    ಪೂರ್ಣಾಙ್ಕ		  *ಅಂಕೆ_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ಪೂರ್ಣಾಙ್ಕ		   flags)	// EW_DIR, etc.
{
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
    char_u	*eval_pat = ರಿಕ್ತ;
    char_u	*exp_pat = *pat;
    ವರ್ಣ	*ignored_ಸಂದೇಶ;
    size_t	usedlen;
    ಪೂರ್ಣಾಙ್ಕ		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    ಪೂರ್ಣಾಙ್ಕ		star_follows = FALSE;

    ಯದಿ (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       ರಿಕ್ತ, &ignored_ಸಂದೇಶ, ರಿಕ್ತ, TRUE);
	--emsg_off;
	ಯದಿ (eval_pat != ರಿಕ್ತ)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    ಯದಿ (exp_pat != ರಿಕ್ತ)
	ret = expand_wildcards(1, &exp_pat, ಅಂಕೆ_file, file, flags);

    ಯದಿ (eval_pat != ರಿಕ್ತ)
    {
	ಯದಿ (*ಅಂಕೆ_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    ಯದಿ (*file != ರಿಕ್ತ)
	    {
		**file = eval_pat;
		eval_pat = ರಿಕ್ತ;
		*ಅಂಕೆ_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_wildcards(
    ಪೂರ್ಣಾಙ್ಕ		   ಅಂಕೆ_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    ಪೂರ್ಣಾಙ್ಕ		  *ಅಂಕೆ_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    ಪೂರ್ಣಾಙ್ಕ		   flags)	// EW_DIR, etc.
{
    ಪೂರ್ಣಾಙ್ಕ		retval;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(ಅಂಕೆ_pat, pat, ಅಂಕೆ_files, files, flags);

    // When keeping all matches, return here
    ಯದಿ ((flags & EW_KEEPALL) || retval == FAIL)
	ಪ್ರತ್ಯಾಗಮ retval;

    /*
     * Remove names that match 'wildignore'.
     */
    ಯದಿ (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	ಪ್ರತಿ (i = 0; i < *ಅಂಕೆ_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    ಯದಿ (ffname == ರಿಕ್ತ)		// out of memory
		ಭಙ್ಗ;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    ಯದಿ (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		ಪ್ರತಿ (j = i; j + 1 < *ಅಂಕೆ_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*ಅಂಕೆ_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	ಯದಿ (*ಅಂಕೆ_files == 0)
	{
	    VIM_CLEAR(*files);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    ಯದಿ (*ಅಂಕೆ_files > 1 && !got_int)
    {
	non_suf_match = 0;
	ಪ್ರತಿ (i = 0; i < *ಅಂಕೆ_files; ++i)
	{
	    ಯದಿ (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		ಪ್ರತಿ (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    ಪೂರ್ಣಾಙ್ಕ
match_suffix(char_u *fname)
{
    ಪೂರ್ಣಾಙ್ಕ		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fname);
    setsuflen = 0;
    ಪ್ರತಿ (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	ಯದಿ (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    ಯದಿ (vim_strchr(tail, '.') == ರಿಕ್ತ)
	    {
		setsuflen = 1;
		ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		ಭಙ್ಗ;
	    setsuflen = 0;
	}
    }
    ಪ್ರತ್ಯಾಗಮ (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vim_backtick(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    ಪೂರ್ಣಾಙ್ಕ		cnt = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    ಯದಿ (cmd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

#ifdef FEAT_EVAL
    ಯದಿ (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    ಅನ್ಯಥಾ
#endif
	buffer = get_cmd_output(cmd, ರಿಕ್ತ,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, ರಿಕ್ತ);
    vim_free(cmd);
    ಯದಿ (buffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    cmd = buffer;
    ಯಾವತ್ (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	ಯಾವತ್ (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	ಯದಿ (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	ಯಾವತ್ (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    ಪ್ರತ್ಯಾಗಮ cnt;
}

#ಯದಿ defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pstrcmp(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    ಪ್ರತ್ಯಾಗಮ (pathcmp(*(ವರ್ಣ **)a, *(ವರ್ಣ **)b, -1));
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
    ಪೂರ್ಣಾಙ್ಕ
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ಪೂರ್ಣಾಙ್ಕ		flags,		// EW_* flags
    ಪೂರ್ಣಾಙ್ಕ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ಪೂರ್ಣಾಙ್ಕ		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		starts_with_dot;
    ಪೂರ್ಣಾಙ್ಕ		matches;
    size_t	buflen;
    size_t	len;
    ಪೂರ್ಣಾಙ್ಕ		starstar = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_ಮೌಲ್ಯ;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = ರಿಕ್ತ;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    ಪೂರ್ಣಾಙ್ಕ			ok;
    char_u		*p_alt;
    ಪೂರ್ಣಾಙ್ಕ			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    ಯದಿ (stardepth > 0)
    {
	ui_breakcheck();
	ಯದಿ (got_int)
	    ಪ್ರತ್ಯಾಗಮ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ರಿಕ್ತ;
    path_end = path;
    ಯಾವತ್ (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ಯದಿ (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	ಅನ್ಯಥಾ ಯದಿ (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    ಯದಿ (e != ರಿಕ್ತ)
		ಭಙ್ಗ;
	    s = p + 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != ರಿಕ್ತ)
	    e = p;
	ಯದಿ (has_mbyte)
	{
	    ಪೂರ್ಣಾಙ್ಕ charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	ಅನ್ಯಥಾ
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    ಪ್ರತಿ (p = buf + wildoff; p < s; ++p)
	ಯದಿ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    ಪ್ರತಿ (p = s; p < e; ++p)
	ಯದಿ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ರಿಕ್ತ, FALSE);
    ಯದಿ (pat == ರಿಕ್ತ)
    {
	vim_free(buf);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    // compile the regexp into a program
    ಯದಿ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ಯದಿ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ಯದಿ (regmatch.regprog == ರಿಕ್ತ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != ರಿಕ್ತ;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ಯದಿ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((ವರ್ಣ *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(ಶೂನ್ಯ)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((ವರ್ಣ *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, ರಿಕ್ತ);
    ಯದಿ (wn != ರಿಕ್ತ)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_ಮೌಲ್ಯ);

    ಯಾವತ್ (ok)
    {
	p = utf16_to_enc(wfb.cFileName, ರಿಕ್ತ);   // p is allocated here

	ಯದಿ (p == ರಿಕ್ತ)
	    ಭಙ್ಗ;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	ಯದಿ (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, ರಿಕ್ತ);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	ಯದಿ ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == ರಿಕ್ತ
		  || (regmatch.regprog != ರಿಕ್ತ
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != ರಿಕ್ತ
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((ವರ್ಣ *)s, buflen - len, "%s", p);
	    ಯದಿ (len + 1 < buflen)
	    {
		ಯದಿ (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((ವರ್ಣ *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (ಶೂನ್ಯ)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((ವರ್ಣ *)buf + len, buflen - len, "%s", path_end);
		ಯದಿ (mch_has_exp_wildcard(path_end))
		{
		    ಯದಿ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ಶೂನ್ಯ)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ಯದಿ (*path_end != 0)
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
    ಯದಿ (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   ಪರಿಮಾಣ(char_u *), pstrcmp);
    ಪ್ರತ್ಯಾಗಮ matches;
}
#endif // MSWIN

#ಯದಿ (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pstrcmp(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    ಪ್ರತ್ಯಾಗಮ (pathcmp(*(ವರ್ಣ **)a, *(ವರ್ಣ **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    ಪೂರ್ಣಾಙ್ಕ
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ಪೂರ್ಣಾಙ್ಕ		flags,		// EW_* flags
    ಪೂರ್ಣಾಙ್ಕ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ಪೂರ್ಣಾಙ್ಕ		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		starts_with_dot;
    ಪೂರ್ಣಾಙ್ಕ		matches;
    size_t	buflen;
    size_t	len;
    ಪೂರ್ಣಾಙ್ಕ		starstar = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    ಸಂರಚನಾ dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    ಯದಿ (stardepth > 0)
    {
	ui_breakcheck();
	ಯದಿ (got_int)
	    ಪ್ರತ್ಯಾಗಮ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ರಿಕ್ತ;
    path_end = path;
    ಯಾವತ್ (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ಯದಿ (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	ಅನ್ಯಥಾ ಯದಿ (*path_end == '/')
	{
	    ಯದಿ (e != ರಿಕ್ತ)
		ಭಙ್ಗ;
	    s = p + 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != ರಿಕ್ತ
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	ಯದಿ (has_mbyte)
	{
	    ಪೂರ್ಣಾಙ್ಕ charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	ಅನ್ಯಥಾ
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    ಪ್ರತಿ (p = buf + wildoff; p < s; ++p)
	ಯದಿ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    ಪ್ರತಿ (p = s; p < e; ++p)
	ಯದಿ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ರಿಕ್ತ, FALSE);
    ಯದಿ (pat == ರಿಕ್ತ)
    {
	vim_free(buf);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    // compile the regexp into a program
    ಯದಿ (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    ಅನ್ಯಥಾ
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ಯದಿ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ಯದಿ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ಯದಿ (regmatch.regprog == ರಿಕ್ತ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ಯದಿ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((ವರ್ಣ *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(ಶೂನ್ಯ)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (ವರ್ಣ *)buf);

    // Find all matching entries
    ಯದಿ (dirp != ರಿಕ್ತ)
    {
	ಯಾವತ್ (!got_int)
	{
	    dp = readdir(dirp);
	    ಯದಿ (dp == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    len = (size_t)(s - buf);
	    ಯದಿ ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != ರಿಕ್ತ && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((ವರ್ಣ *)s, buflen - len, "%s", dp->d_name);
		ಯದಿ (len + 1 >= buflen)
		    ಅನುವರ್ತ;

		ಯದಿ (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((ವರ್ಣ *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (ಶೂನ್ಯ)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((ವರ್ಣ *)buf + len, buflen - len, "%s", path_end);
		ಯದಿ (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    ಯದಿ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ಶೂನ್ಯ)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ಯದಿ (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    ಯದಿ ((flags & EW_ALLLINKS) ? mch_lstat((ವರ್ಣ *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			ಯದಿ (precomp_buf)
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
    ಯದಿ (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   ಪರಿಮಾಣ(char_u *), pstrcmp);
    ಪ್ರತ್ಯಾಗಮ matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
has_env_var(char_u *p)
{
    ಪ್ರತಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಯದಿ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr((char_u *)
#ಯದಿ defined(MSWIN)
				    "$%"
#ಅನ್ಯಥಾ
				    "$"
#endif
					, *p) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
has_special_wildchar(char_u *p)
{
    ಪ್ರತಿ ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	ಯದಿ (*p == '\r' || *p == '\n')
	    ಭಙ್ಗ;
	// Allow for escaping.
	ಯದಿ (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != ರಿಕ್ತ)
	{
	    // A { must be followed by a matching }.
	    ಯದಿ (*p == '{' && vim_strchr(p, '}') == ರಿಕ್ತ)
		ಅನುವರ್ತ;
	    // A quote and backtick must be followed by another one.
	    ಯದಿ ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == ರಿಕ್ತ)
		ಅನುವರ್ತ;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಪೂರ್ಣಾಙ್ಕ
gen_expand_wildcards(
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ಪೂರ್ಣಾಙ್ಕ		flags)		// EW_* flags
{
    ಪೂರ್ಣಾಙ್ಕ			i;
    garray_T		ga;
    char_u		*p;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		recursive = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			add_pat;
    ಪೂರ್ಣಾಙ್ಕ			retval = OK;
    ಪೂರ್ಣಾಙ್ಕ			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    ಯದಿ (recursive)
#ifdef SPECIAL_WILDCHAR
	ಪ್ರತ್ಯಾಗಮ mch_expand_wildcards(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file, flags);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_pat; i++)
    {
	ಯದಿ (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    ಪ್ರತ್ಯಾಗಮ mch_expand_wildcards(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, ಪರಿಮಾಣ(char_u *), 30);

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	ಯದಿ (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    ಯದಿ (add_pat == -1)
		retval = FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    ಯದಿ ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		ಯದಿ (p == ರಿಕ್ತ)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		ಅನ್ಯಥಾ ಯದಿ (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    ಪ್ರತ್ಯಾಗಮ i;
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
	    ಯದಿ (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		ಯದಿ ((flags & (EW_PATH | EW_CDPATH))
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
		ಅನ್ಯಥಾ
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	ಯದಿ (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    ಯದಿ (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    ಅನ್ಯಥಾ
		addfile(&ga, t, flags);

	    ಯದಿ (t != p)
		vim_free(t);
	}

	ಯದಿ (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	ಯದಿ (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    ಯದಿ (retval == FAIL)
	ga_clear_strings(&ga);

    *ಅಂಕೆ_file = ga.ga_len;
    *file = (ga.ga_data != ರಿಕ್ತ) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    ಪ್ರತ್ಯಾಗಮ ((flags & EW_EMPTYOK) || ga.ga_data != ರಿಕ್ತ) ? retval : FAIL;
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
    ಶೂನ್ಯ
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		isdir;
    stat_T	sb;

#ifdef MSWIN
    ಯದಿ (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	ಯದಿ (perm < 0)
	    ಪ್ರತ್ಯಾಗಮ;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    ಅನ್ಯಥಾ
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	ಯದಿ (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((ವರ್ಣ *)f, &sb) < 0 : mch_getperm(f) < 0))
	    ಪ್ರತ್ಯಾಗಮ;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    ಯದಿ (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
#endif

    ಯದಿ ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	ಪ್ರತ್ಯಾಗಮ;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    ಯದಿ (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, ರಿಕ್ತ, !(flags & EW_SHELLCMD)))
	ಪ್ರತ್ಯಾಗಮ;

    // Make room for another item in the file list.
    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    p = alloc(STRLEN(f) + 1 + isdir);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    ಯದಿ (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    ಶೂನ್ಯ
FreeWild(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, char_u **files)
{
    ಯದಿ (ಎಣಿಕೆ <= 0 || files == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯಾವತ್ (ಎಣಿಕೆ--)
	vim_free(files[ಎಣಿಕೆ]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    ಪೂರ್ಣಾಙ್ಕ
pathcmp(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *q, ಪೂರ್ಣಾಙ್ಕ maxlen)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		c1, c2;
    ಧ್ರುವ ವರ್ಣ	*s = ರಿಕ್ತ;

    ಪ್ರತಿ (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	ಯದಿ (c1 == NUL)
	{
	    ಯದಿ (c2 == NUL)  // full match
		ಪ್ರತ್ಯಾಗಮ 0;
	    s = q;
	    i = j;
	    ಭಙ್ಗ;
	}

	// End of "q": check if "p" just has a slash.
	ಯದಿ (c2 == NUL)
	{
	    s = p;
	    ಭಙ್ಗ;
	}

	ಯದಿ ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    ಯದಿ (vim_ispathsep(c1))
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಯದಿ (vim_ispathsep(c2))
		ಪ್ರತ್ಯಾಗಮ 1;
	    ಪ್ರತ್ಯಾಗಮ p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    ಯದಿ (s == ರಿಕ್ತ)	// "i" or "j" ran into "maxlen"
	ಪ್ರತ್ಯಾಗಮ 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    ಯದಿ (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#ಅನ್ಯಥಾ
	    && c1 == '/'
#endif
       )
	ಪ್ರತ್ಯಾಗಮ 0;   // match with trailing slash
    ಯದಿ (s == q)
	ಪ್ರತ್ಯಾಗಮ -1;	    // no match
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_isAbsName(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		force)	    // force expansion even when already absolute
{
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    ಪೂರ್ಣಾಙ್ಕ		url;

    *buf = NUL;
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    url = path_with_url(fname);
    ಯದಿ (!url)
	retval = mch_FullName(fname, buf, len, force);
    ಯದಿ (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#ಯದಿ defined(MSWIN)
    slash_adjust(buf);
#endif
    ಪ್ರತ್ಯಾಗಮ retval;
}
