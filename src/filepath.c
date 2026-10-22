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

#അന്തര്ഭാവ "vim.h"

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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    പൂർണ്ണസംഖ്യ		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * വലുപ്പം(*newbuf));
    എങ്കിൽ (newbuf == ശൂന്യ)
	തിരികെ FAIL;

    wfname = enc_to_utf16(*fnamep, ശൂന്യ);
    എങ്കിൽ (wfname == ശൂന്യ)
    {
	vim_free(newbuf);
	തിരികെ FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    എങ്കിൽ (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * വലുപ്പം(*newbuf));
	എങ്കിൽ (newbuf == ശൂന്യ)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    തിരികെ FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    എങ്കിൽ (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, ശൂന്യ);

	എങ്കിൽ (p != ശൂന്യ)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	അല്ലെങ്കിൽ
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    തിരികെ FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = ശൂന്യ;
    char_u	*short_fname = ശൂന്യ;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    പൂർണ്ണസംഖ്യ		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    എങ്കിൽ (save_fname == ശൂന്യ)
    {
	retval = FAIL;
	പോകുക theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    വേണ്ടി (;;)
    {
	// go back one path-separator
	ആകുമ്പോൾ (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	എങ്കിൽ (endp <= save_fname)
	    നിർത്തുക;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	എങ്കിൽ (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    പോകുക theend;
	}
	*endp = ch;	// preserve the string

	എങ്കിൽ (len > 0)
	    നിർത്തുക;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    എങ്കിൽ (len > 0)
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
	എങ്കിൽ (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    എങ്കിൽ (*fname == ശൂന്യ)
	    {
		retval = FAIL;
		പോകുക theend;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    എങ്കിൽ (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = ശൂന്യ;
	}

	// concat the not-shortened part of the path
	എങ്കിൽ ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    തിരികെ retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    പൂർണ്ണസംഖ്യ		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    പൂർണ്ണസംഖ്യ		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    വേണ്ടി (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	എങ്കിൽ (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    എങ്കിൽ (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    അല്ലെങ്കിൽ
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    എങ്കിൽ (tfname == ശൂന്യ)
	തിരികെ FAIL;

    len = tflen = STRLEN(tfname);

    എങ്കിൽ (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	എങ്കിൽ (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    തിരികെ FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    വേണ്ടി (p = tfname + len - 1; p >= tfname; --p)
    {
	എങ്കിൽ (has_mbyte)
	    p -= mb_head_off(tfname, p);
	എങ്കിൽ (vim_ispathsep(*p))
	{
	    എങ്കിൽ (sepcount == 0 || (hasTilde && sepcount == 1))
		നിർത്തുക;
	    അല്ലെങ്കിൽ
		sepcount --;
	}
    }
    എങ്കിൽ (hasTilde)
    {
	--p;
	എങ്കിൽ (p >= tfname)
	    *p = '~';
	അല്ലെങ്കിൽ
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    തിരികെ OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    പൂർണ്ണസംഖ്യ
modify_fname(
    char_u	*src,		// string with modifiers
    പൂർണ്ണസംഖ്യ		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    പൂർണ്ണസംഖ്യ		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		has_fullname = 0;
    പൂർണ്ണസംഖ്യ		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    പൂർണ്ണസംഖ്യ		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    എങ്കിൽ (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	എങ്കിൽ ((*fnamep)[0] == '~'
#എങ്കിൽ !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    എങ്കിൽ (*fnamep == ശൂന്യ)
		തിരികെ -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	വേണ്ടി (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    എങ്കിൽ (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		നിർത്തുക;
	}

	// FullName_save() is slow, don't use it when not needed.
	എങ്കിൽ (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    എങ്കിൽ (*fnamep == ശൂന്യ)
		തിരികെ -1;
	}

#ifdef MSWIN
# എങ്കിൽ _WIN32_WINNT >= 0x0500
	എങ്കിൽ (vim_strchr(*fnamep, '~') != ശൂന്യ)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, ശൂന്യ);
	    WCHAR buf[_MAX_PATH];

	    എങ്കിൽ (wfname != ശൂന്യ)
	    {
		എങ്കിൽ (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, ശൂന്യ);

		    എങ്കിൽ (q != ശൂന്യ)
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
	എങ്കിൽ (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    എങ്കിൽ (*fnamep == ശൂന്യ)
		തിരികെ -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    ആകുമ്പോൾ (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	എങ്കിൽ (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    തുടരുക;
	}
	pbuf = ശൂന്യ;
	// Need full path first (use expand_env() to remove a "~/")
	എങ്കിൽ (!has_fullname && !has_homerelative)
	{
	    എങ്കിൽ (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    അല്ലെങ്കിൽ
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	അല്ലെങ്കിൽ
	    p = *fnamep;

	has_fullname = 0;

	എങ്കിൽ (p != ശൂന്യ)
	{
	    size_t  dirnamelen = 0;

	    എങ്കിൽ (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		എങ്കിൽ (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    എങ്കിൽ (s != ശൂന്യ)
		    {
			dirnamelen = home_replace(ശൂന്യ, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		എങ്കിൽ (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		എങ്കിൽ (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    എങ്കിൽ (vim_ispathsep(*p))
		    {
			ആകുമ്പോൾ (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			എങ്കിൽ (pbuf != ശൂന്യ)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = ശൂന്യ;
			}
		    }
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		dirnamelen = home_replace(ശൂന്യ, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		എങ്കിൽ (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    എങ്കിൽ (s != ശൂന്യ)
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
    ആകുമ്പോൾ (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	ആകുമ്പോൾ (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	എങ്കിൽ (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	എങ്കിൽ (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    എങ്കിൽ (p == ശൂന്യ)
		തിരികെ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	അല്ലെങ്കിൽ
	{
	    ആകുമ്പോൾ (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    എങ്കിൽ (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    എങ്കിൽ (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	എങ്കിൽ (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    എങ്കിൽ (p == ശൂന്യ)
		തിരികെ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	എങ്കിൽ (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    എങ്കിൽ (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		തിരികെ -1;
	}
	അല്ലെങ്കിൽ
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    എങ്കിൽ (get_short_pathname(fnamep, bufp, &l) == FAIL)
		തിരികെ -1;

	    എങ്കിൽ (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		എങ്കിൽ (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    തിരികെ -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    എങ്കിൽ (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    ആകുമ്പോൾ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	എങ്കിൽ (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	അല്ലെങ്കിൽ
	    s = *fnamep + *fnamelen - 1;
	വേണ്ടി ( ; s > tail; --s)
	    എങ്കിൽ (s[0] == '.')
		നിർത്തുക;
	എങ്കിൽ (src[*usedlen + 1] == 'e')		// :e
	{
	    എങ്കിൽ (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		വേണ്ടി ( ; s > *fnamep; --s)
		    എങ്കിൽ (s[0] == ';')
			നിർത്തുക;
		എങ്കിൽ (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*fnamep <= tail)
		*fnamelen = 0;
	}
	അല്ലെങ്കിൽ				// :r
	{
	    char_u *limit = *fnamep;

	    എങ്കിൽ (limit < tail)
		limit = tail;
	    എങ്കിൽ (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    എങ്കിൽ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	പൂർണ്ണസംഖ്യ	    sep;
	char_u	    *flags;
	പൂർണ്ണസംഖ്യ	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	എങ്കിൽ (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	എങ്കിൽ (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    എങ്കിൽ (p != ശൂന്യ)
	    {
		pat = vim_strnsave(s, p - s);
		എങ്കിൽ (pat != ശൂന്യ)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    എങ്കിൽ (p != ശൂന്യ)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			എങ്കിൽ (sub != ശൂന്യ && str != ശൂന്യ)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, ശൂന്യ, flags, &slen);
			    എങ്കിൽ (s != ശൂന്യ)
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
	    എങ്കിൽ (didit)
		പോകുക repeat;
	}
    }

    എങ്കിൽ (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	എങ്കിൽ (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	എങ്കിൽ (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    തിരികെ valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    സ്ഥിതം ശൂന്യം
shorten_dir_len(char_u *str, പൂർണ്ണസംഖ്യ trim_len)
{
    char_u	*tail, *s, *d;
    പൂർണ്ണസംഖ്യ		skip = FALSE;
    പൂർണ്ണസംഖ്യ		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    വേണ്ടി (s = str; ; ++s)
    {
	എങ്കിൽ (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    എങ്കിൽ (*s == NUL)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!skip)
	{
	    *d++ = *s;			// copy next char
	    എങ്കിൽ (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		എങ്കിൽ (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    എങ്കിൽ (has_mbyte)
	    {
		പൂർണ്ണസംഖ്യ l = mb_ptr2len(s);

		ആകുമ്പോൾ (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    ശൂന്യം
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    പൂർണ്ണസംഖ്യ
file_is_readable(char_u *fname)
{
    പൂർണ്ണസംഖ്യ		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    എങ്കിൽ (*fname == NUL || mch_isdir_or_missing(fname))
	തിരികെ FALSE;
    എങ്കിൽ ((fd = mch_open((അക്ഷരം *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    ശൂന്യം
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	എങ്കിൽ (in_vim9script())
	    (ശൂന്യം) check_for_string_arg(argvars, 0);
	തിരികെ;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    എങ്കിൽ (cwd != ശൂന്യ)
    {
	എങ്കിൽ (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	എങ്കിൽ (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "scope", s);
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_localdir != ശൂന്യ)
	scope = CDSCOPE_WINDOW;
    അല്ലെങ്കിൽ എങ്കിൽ (curtab->tp_localdir != ശൂന്യ)
	scope = CDSCOPE_TABPAGE;

    എങ്കിൽ (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    ശൂന്യം
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    name = tv_get_string(&argvars[0]);
    എങ്കിൽ (name == ശൂന്യ || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	തിരികെ;
    }

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    അല്ലെങ്കിൽ
	flags = (char_u *)"";

    എങ്കിൽ (*flags == NUL)
	// delete a file
	rettv->vval.v_സംഖ്യ = mch_remove(name) == 0 ? 0 : -1;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_സംഖ്യ = mch_rmdir(name) == 0 ? 0 : -1;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_സംഖ്യ = delete_recursive(name);
    അല്ലെങ്കിൽ
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    ശൂന്യം
f_executable(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_സംഖ്യ = mch_can_exe(tv_get_string(&argvars[0]), ശൂന്യ, TRUE);
}

/*
 * "exepath()" function
 */
    ശൂന്യം
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = ശൂന്യ;

    എങ്കിൽ (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	തിരികെ;
    (ശൂന്യം)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    ശൂന്യം
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;
    rettv->vval.v_സംഖ്യ = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    ശൂന്യം
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;
    rettv->vval.v_സംഖ്യ = filewritable(tv_get_string(&argvars[0]));
}

    സ്ഥിതം ശൂന്യം
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    പൂർണ്ണസംഖ്യ		find_what)
{
    char_u	*fname;
    char_u	*fresult = ശൂന്യ;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		എണ്ണം = 1;
    പൂർണ്ണസംഖ്യ		first = TRUE;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    rettv->vval.v_string = ശൂന്യ;
    rettv->v_type = VAR_STRING;
    എങ്കിൽ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL)))
	തിരികെ;

    fname = tv_get_string(&argvars[0]);

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	എങ്കിൽ (p == ശൂന്യ)
	    error = TRUE;
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*p != NUL)
		path = p;

	    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
		എണ്ണം = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	}
    }

    എങ്കിൽ (എണ്ണം < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    എങ്കിൽ (*fname != NUL && !error)
    {
	char_u	*file_to_find = ശൂന്യ;
	അക്ഷരം	*തിരയൽ_ctx = ശൂന്യ;

	ചെയ്യുക
	{
	    എങ്കിൽ (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : ശൂന്യ,
					       first ? (പൂർണ്ണസംഖ്യ)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &തിരയൽ_ctx);
	    first = FALSE;

	    എങ്കിൽ (fresult != ശൂന്യ && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} ആകുമ്പോൾ ((rettv->v_type == VAR_LIST || --എണ്ണം > 0) && fresult != ശൂന്യ);

	vim_free(file_to_find);
	vim_findfile_cleanup(തിരയൽ_ctx);
    }

    എങ്കിൽ (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    ശൂന്യം
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    ശൂന്യം
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    ശൂന്യം
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = ശൂന്യ;
    char_u	buf[NUMBUFLEN];

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    എങ്കിൽ (mods == ശൂന്യ || fname == ശൂന്യ)
	fname = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	len = STRLEN(fname);
	എങ്കിൽ (mods != ശൂന്യ && *mods != NUL)
	    (ശൂന്യം)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    എങ്കിൽ (fname == ശൂന്യ)
	rettv->vval.v_string = ശൂന്യ;
    അല്ലെങ്കിൽ
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
    ശൂന്യം
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = ശൂന്യ;
    tabpage_T	*tp = ശൂന്യ;
    char_u	*cwd;
    പൂർണ്ണസംഖ്യ		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL)))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_സംഖ്യ
	    && argvars[0].vval.v_സംഖ്യ == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    അല്ലെങ്കിൽ
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    എങ്കിൽ (wp != ശൂന്യ && wp->w_localdir != ശൂന്യ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    അല്ലെങ്കിൽ എങ്കിൽ (tp != ശൂന്യ && tp->tp_localdir != ശൂന്യ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    അല്ലെങ്കിൽ എങ്കിൽ (wp != ശൂന്യ || tp != ശൂന്യ || global)
    {
	എങ്കിൽ (globaldir != ശൂന്യ && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	അല്ലെങ്കിൽ
	{
	    cwd = alloc(MAXPATHL);
	    എങ്കിൽ (cwd != ശൂന്യ)
	    {
		എങ്കിൽ (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    എങ്കിൽ (rettv->vval.v_string != ശൂന്യ)
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
    പൂർണ്ണസംഖ്യ		    i;

    വേണ്ടി (i = 0; i < 9; i++)
    {
	എങ്കിൽ (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	അല്ലെങ്കിൽ
	    perm[i] = '-';
    }
    തിരികെ perm;
}

/*
 * "getfperm({fname})" function
 */
    ശൂന്യം
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = ശൂന്യ;
    char_u	permbuf[] = "---------";

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    എങ്കിൽ (mch_stat((അക്ഷരം *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    ശൂന്യം
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    fname = tv_get_string(&argvars[0]);
    എങ്കിൽ (mch_stat((അക്ഷരം *)fname, &st) >= 0)
    {
	എങ്കിൽ (S_ISDIR(st.st_mode))
	    rettv->vval.v_സംഖ്യ = 0;
	അല്ലെങ്കിൽ
	{
	    rettv->vval.v_സംഖ്യ = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    എങ്കിൽ ((off_T)rettv->vval.v_സംഖ്യ != (off_T)st.st_size)
		rettv->vval.v_സംഖ്യ = -2;
	}
    }
    അല്ലെങ്കിൽ
	  rettv->vval.v_സംഖ്യ = -1;
}

/*
 * "getftime({fname})" function
 */
    ശൂന്യം
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    fname = tv_get_string(&argvars[0]);
    എങ്കിൽ (mch_stat((അക്ഷരം *)fname, &st) >= 0)
	rettv->vval.v_സംഖ്യ = (varnumber_T)st.st_mtime;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    അക്ഷരം    *t;

    എങ്കിൽ (S_ISREG(st->st_mode))
	t = "file";
    അല്ലെങ്കിൽ എങ്കിൽ (S_ISDIR(st->st_mode))
	t = "dir";
    അല്ലെങ്കിൽ എങ്കിൽ (S_ISLNK(st->st_mode))
	t = "link";
    അല്ലെങ്കിൽ എങ്കിൽ (S_ISBLK(st->st_mode))
	t = "bdev";
    അല്ലെങ്കിൽ എങ്കിൽ (S_ISCHR(st->st_mode))
	t = "cdev";
    അല്ലെങ്കിൽ എങ്കിൽ (S_ISFIFO(st->st_mode))
	t = "fifo";
    അല്ലെങ്കിൽ എങ്കിൽ (S_ISSOCK(st->st_mode))
	t = "socket";
    അല്ലെങ്കിൽ
	t = "other";
    തിരികെ (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    ശൂന്യം
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = ശൂന്യ;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    എങ്കിൽ (mch_lstat((അക്ഷരം *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    ശൂന്യം
f_glob(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	തിരികെ;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    എങ്കിൽ (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, ശൂന്യ);
	    എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    എങ്കിൽ (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	എങ്കിൽ (p_wic)
	    options += WILD_ICASE;
	എങ്കിൽ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     ശൂന്യ, options, WILD_ALL);
	അല്ലെങ്കിൽ എങ്കിൽ (rettv_list_alloc(rettv) == OK)
	{
	  പൂർണ്ണസംഖ്യ i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						ശൂന്യ, options, WILD_ALL_KEEP);
	  വേണ്ടി (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_string = ശൂന്യ;
}

/*
 * "glob2regpat()" function
 */
    ശൂന്യം
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == ശൂന്യ)
			 ? ശൂന്യ : file_pat_to_reg_pat(pat, ശൂന്യ, ശൂന്യ, FALSE);
}

/*
 * "globpath()" function
 */
    ശൂന്യം
f_globpath(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	തിരികെ;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    എങ്കിൽ (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, ശൂന്യ);
	    എങ്കിൽ (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    എങ്കിൽ (file != ശൂന്യ && !error)
    {
	ga_init2(&ga, വലുപ്പം(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	എങ്കിൽ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	അല്ലെങ്കിൽ എങ്കിൽ (rettv_list_alloc(rettv) == OK)
	    വേണ്ടി (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_string = ശൂന്യ;
}

/*
 * "isdirectory()" function
 */
    ശൂന്യം
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    ശൂന്യം
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mkdir_recurse(char_u *dir, പൂർണ്ണസംഖ്യ prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    പൂർണ്ണസംഖ്യ		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    എങ്കിൽ (p <= get_past_head(dir))
	തിരികെ OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    എങ്കിൽ (updir == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (mch_isdir(updir))
	r = OK;
    അല്ലെങ്കിൽ എങ്കിൽ (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	എങ്കിൽ (r == OK && created != ശൂന്യ && *created == ശൂന്യ)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    തിരികെ r;
}

/*
 * "mkdir()" function
 */
    ശൂന്യം
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		prot = 0755;
    പൂർണ്ണസംഖ്യ		defer = FALSE;
    പൂർണ്ണസംഖ്യ		defer_recurse = FALSE;
    char_u	*created = ശൂന്യ;

    rettv->vval.v_സംഖ്യ = FAIL;
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL)))
	തിരികെ;

    dir = tv_get_string_buf(&argvars[0], buf);
    എങ്കിൽ (*dir == NUL)
	തിരികെ;

    എങ്കിൽ (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[2], ശൂന്യ);
	    എങ്കിൽ (prot == -1)
		തിരികെ;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != ശൂന്യ;
	defer_recurse = vim_strchr(arg2, 'R') != ശൂന്യ;
	എങ്കിൽ ((defer || defer_recurse) && !can_add_defer())
	    തിരികെ;

	എങ്കിൽ (vim_strchr(arg2, 'p') != ശൂന്യ)
	{
	    എങ്കിൽ (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_സംഖ്യ = OK;
		തിരികെ;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : ശൂന്യ);
	}
    }
    rettv->vval.v_സംഖ്യ = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    എങ്കിൽ (rettv->vval.v_സംഖ്യ == OK
				&& created == ശൂന്യ && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    എങ്കിൽ (created != ശൂന്യ)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	എങ്കിൽ (tv[0].vval.v_string == ശൂന്യ || tv[1].vval.v_string == ശൂന്യ
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
    ശൂന്യം
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		trim_len = 1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[1]);
	എങ്കിൽ (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    എങ്കിൽ (p == ശൂന്യ)
	rettv->vval.v_string = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	എങ്കിൽ (p != ശൂന്യ)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
checkitem_common(ശൂന്യം *context, char_u *name, dict_T *dict)
{
    typval_T	*വ്യഞ്ജകം = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    പൂർണ്ണസംഖ്യ		retval = 0;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    എങ്കിൽ (name != ശൂന്യ)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    അല്ലെങ്കിൽ
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    എങ്കിൽ (eval_വ്യഞ്ജകം_typval(വ്യഞ്ജകം, FALSE, argv, 1, ശൂന്യ, &rettv) == FAIL)
	പോകുക theend;

    // We want to use -1, but also true/false should be allowed.
    എങ്കിൽ (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_സംഖ്യ;
	rettv.vval.v_സംഖ്യ = rettv.vval.v_സംഖ്യ == VVAL_TRUE;
    }
    retval = tv_get_സംഖ്യ_chk(&rettv, &error);
    എങ്കിൽ (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    എങ്കിൽ (name != ശൂന്യ)
	set_vim_var_string(VV_VAL, ശൂന്യ, 0);
    അല്ലെങ്കിൽ
	set_vim_var_dict(VV_VAL, ശൂന്യ);
    restore_vimvar(VV_VAL, &save_val);
    തിരികെ retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
readdir_checkitem(ശൂന്യം *context, ശൂന്യം *item)
{
    char_u	*name = (char_u *)item;

    തിരികെ checkitem_common(context, name, ശൂന്യ);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
readdirex_dict_arg(typval_T *argvars, പൂർണ്ണസംഖ്യ *cmp)
{
    char_u     *compare;

    എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (dict_has_താക്കോൽ(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    അല്ലെങ്കിൽ
    {
	semsg(_(e_dictionary_താക്കോൽ_str_required), "sort");
	തിരികെ FAIL;
    }

    എങ്കിൽ (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    തിരികെ OK;
}

/*
 * "readdir()" function
 */
    ശൂന്യം
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*വ്യഞ്ജകം;
    പൂർണ്ണസംഖ്യ		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		sort = READDIR_SORT_BYTE;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;
    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	തിരികെ;

    path = tv_get_string(&argvars[0]);
    വ്യഞ്ജകം = &argvars[1];

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	തിരികെ;

    ret = readdir_core(&ga, path, FALSE, (ശൂന്യം *)വ്യഞ്ജകം,
	    (വ്യഞ്ജകം->v_type == VAR_UNKNOWN) ? ശൂന്യ : readdir_checkitem, sort);
    എങ്കിൽ (ret == OK)
    {
	വേണ്ടി (i = 0; i < ga.ga_len; i++)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
readdirex_checkitem(ശൂന്യം *context, ശൂന്യം *item)
{
    dict_T	*dict = (dict_T*)item;

    തിരികെ checkitem_common(context, ശൂന്യ, dict);
}

/*
 * "readdirex()" function
 */
    ശൂന്യം
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*വ്യഞ്ജകം;
    പൂർണ്ണസംഖ്യ		ret;
    char_u	*path;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		sort = READDIR_SORT_BYTE;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;
    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	തിരികെ;

    path = tv_get_string(&argvars[0]);
    വ്യഞ്ജകം = &argvars[1];

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	തിരികെ;

    ret = readdir_core(&ga, path, TRUE, (ശൂന്യം *)വ്യഞ്ജകം,
	    (വ്യഞ്ജകം->v_type == VAR_UNKNOWN) ? ശൂന്യ : readdirex_checkitem, sort);
    എങ്കിൽ (ret == OK)
    {
	വേണ്ടി (i = 0; i < ga.ga_len; i++)
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
    സ്ഥിതം ശൂന്യം
read_file_or_blob(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ always_blob)
{
    പൂർണ്ണസംഖ്യ		ദ്വിമാന = FALSE;
    പൂർണ്ണസംഖ്യ		blob = always_blob;
    പൂർണ്ണസംഖ്യ		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    പൂർണ്ണസംഖ്യ		io_size = വലുപ്പം(buf);
    പൂർണ്ണസംഖ്യ		readlen;		// size of last fread()
    char_u	*prev	 = ശൂന്യ;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (always_blob)
	{
	    offset = (off_T)tv_get_സംഖ്യ(&argvars[1]);
	    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_സംഖ്യ(&argvars[2]);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		ദ്വിമാന = TRUE;
	    എങ്കിൽ (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_സംഖ്യ(&argvars[2]);
	}
    }

    എങ്കിൽ ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	തിരികെ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    എങ്കിൽ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	തിരികെ;
    }
    എങ്കിൽ (*fname == NUL || (fd = mch_fopen((അക്ഷരം *)fname, READBIN)) == ശൂന്യ)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	തിരികെ;
    }

    എങ്കിൽ (blob)
    {
	എങ്കിൽ (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	തിരികെ;
    }

    ആകുമ്പോൾ (cnt < maxline || maxline < 0)
    {
	readlen = (പൂർണ്ണസംഖ്യ)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	വേണ്ടി (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || ദ്വിമാന));
		++p)
	{
	    എങ്കിൽ (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= ശൂന്യ;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		എങ്കിൽ (readlen > 0 && !ദ്വിമാന)
		{
		    ആകുമ്പോൾ (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    എങ്കിൽ (len == 0)
			ആകുമ്പോൾ (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		എങ്കിൽ (prevlen == 0)
		    s = vim_strnsave(start, len);
		അല്ലെങ്കിൽ
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    എങ്കിൽ ((s = vim_realloc(prev, prevlen + len + 1)) != ശൂന്യ)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = ശൂന്യ; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		എങ്കിൽ (s == ശൂന്യ)
		{
		    do_outofmem_സന്ദേശം((long_u) prevlen + len + 1);
		    failed = TRUE;
		    നിർത്തുക;
		}

		എങ്കിൽ ((li = listitem_alloc()) == ശൂന്യ)
		{
		    vim_free(s);
		    failed = TRUE;
		    നിർത്തുക;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		എങ്കിൽ ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == 0xbf && enc_utf8 && !ദ്വിമാന)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		എങ്കിൽ (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    എങ്കിൽ (start == dest)
			start = p + 1;
		    അല്ലെങ്കിൽ
		    {
			// have to shuffle buf to close gap
			പൂർണ്ണസംഖ്യ adjust_prevlen = 0;

			എങ്കിൽ (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (പൂർണ്ണസംഖ്യ)(buf - dest);
			    dest = buf;
			}
			എങ്കിൽ (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	എങ്കിൽ (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    നിർത്തുക;
	എങ്കിൽ (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    എങ്കിൽ (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		എങ്കിൽ (prevsize == 0)
		    prevsize = (long)(p - start);
		അല്ലെങ്കിൽ
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		എങ്കിൽ (newprev == ശൂന്യ)
		{
		    do_outofmem_സന്ദേശം((long_u)prevsize);
		    failed = TRUE;
		    നിർത്തുക;
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
    എങ്കിൽ (!failed && maxline < 0)
	ആകുമ്പോൾ (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    എങ്കിൽ (failed)
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
    ശൂന്യം
f_readblob(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL)))
	തിരികെ;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    ശൂന്യം
f_readfile(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL)))
	തിരികെ;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    ശൂന്യം
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = ശൂന്യ;
# endif

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = ശൂന്യ;

	v = mch_resolve_path(p, TRUE);
	എങ്കിൽ (v != ശൂന്യ)
	    rettv->vval.v_string = v;
	അല്ലെങ്കിൽ
	    rettv->vval.v_string = vim_strsave(p);
    }
# അല്ലെങ്കിൽ
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	പൂർണ്ണസംഖ്യ	len;
	char_u	*remain = ശൂന്യ;
	char_u	*q;
	പൂർണ്ണസംഖ്യ	is_relative_to_current = FALSE;
	പൂർണ്ണസംഖ്യ	has_trailing_pathsep = FALSE;
	പൂർണ്ണസംഖ്യ	limit = 100;

	p = vim_strsave(p);
	എങ്കിൽ (p == ശൂന്യ)
	    പോകുക fail;
	എങ്കിൽ (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	എങ്കിൽ (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	എങ്കിൽ (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	എങ്കിൽ (buf == ശൂന്യ)
	{
	    vim_free(p);
	    vim_free(remain);
	    പോകുക fail;
	}

	വേണ്ടി (;;)
	{
	    വേണ്ടി (;;)
	    {
		len = readlink((അക്ഷരം *)p, (അക്ഷരം *)buf, MAXPATHL);
		എങ്കിൽ (len <= 0)
		    നിർത്തുക;
		buf[len] = NUL;

		എങ്കിൽ (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = ശൂന്യ;
		    പോകുക fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		എങ്കിൽ (remain == ശൂന്യ && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		എങ്കിൽ (*q != NUL)
		{
		    എങ്കിൽ (remain == ശൂന്യ)
			remain = vim_strsave(q - 1);
		    അല്ലെങ്കിൽ
		    {
			cpy = concat_str(q - 1, remain);
			എങ്കിൽ (cpy != ശൂന്യ)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		എങ്കിൽ (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		എങ്കിൽ (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    എങ്കിൽ (cpy != ശൂന്യ)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    എങ്കിൽ (remain == ശൂന്യ)
		നിർത്തുക;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    എങ്കിൽ (cpy != ശൂന്യ)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    എങ്കിൽ (*q != NUL)
		STRMOVE(remain, q - 1);
	    അല്ലെങ്കിൽ
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	എങ്കിൽ (!vim_ispathsep(*p))
	{
	    എങ്കിൽ (is_relative_to_current
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
		എങ്കിൽ (cpy != ശൂന്യ)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		ആകുമ്പോൾ (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		എങ്കിൽ (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	എങ്കിൽ (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    എങ്കിൽ (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  അല്ലെങ്കിൽ
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
    ശൂന്യം
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    ചെയ്യുക
    {
	എങ്കിൽ (x == 'Z')
	    x = '0';
	അല്ലെങ്കിൽ എങ്കിൽ (x == '9')
	    x = 'A';
	അല്ലെങ്കിൽ
	    ++x;
    } ആകുമ്പോൾ (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    ശൂന്യം
f_writefile(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		ദ്വിമാന = FALSE;
    പൂർണ്ണസംഖ്യ		append = FALSE;
    പൂർണ്ണസംഖ്യ		defer = FALSE;
# ifdef HAVE_FSYNC
    പൂർണ്ണസംഖ്യ		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    പൂർണ്ണസംഖ്യ		ret = 0;
    listitem_T	*li;
    list_T	*list = ശൂന്യ;
    blob_T	*blob = ശൂന്യ;

    rettv->vval.v_സംഖ്യ = -1;
    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	എങ്കിൽ (list == ശൂന്യ)
	    തിരികെ;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ഇനങ്ങൾ(list, li)
	    എങ്കിൽ (tv_get_string_chk(&li->li_tv) == ശൂന്യ)
		തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	എങ്കിൽ (blob == ശൂന്യ)
	    തിരികെ;
    }
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	തിരികെ;
    }

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	എങ്കിൽ (arg2 == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ (vim_strchr(arg2, 'b') != ശൂന്യ)
	    ദ്വിമാന = TRUE;
	എങ്കിൽ (vim_strchr(arg2, 'a') != ശൂന്യ)
	    append = TRUE;
	എങ്കിൽ (vim_strchr(arg2, 'D') != ശൂന്യ)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	എങ്കിൽ (vim_strchr(arg2, 's') != ശൂന്യ)
	    do_fsync = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr(arg2, 'S') != ശൂന്യ)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (defer && !can_add_defer())
	തിരികെ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    എങ്കിൽ (*fname == NUL || (fd = mch_fopen((അക്ഷരം *)fname,
				      append ? APPENDBIN : WRITEBIN)) == ശൂന്യ)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    എങ്കിൽ (tv.vval.v_string == ശൂന്യ
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(ശൂന്യം)mch_remove(fname);
	    }
	}

	എങ്കിൽ (ret == 0)
	{
	    എങ്കിൽ (blob)
	    {
		എങ്കിൽ (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (write_list(fd, list, ദ്വിമാന) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    എങ്കിൽ (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_സംഖ്യ = ret;
}

#endif // FEAT_EVAL

#എങ്കിൽ defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    പൂർണ്ണസംഖ്യ		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    സ്ഥിതം char_u	*last_dir = ശൂന്യ;    // last used directory
    char_u		*tofree = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    എങ്കിൽ (title == ശൂന്യ || *title == NUL)
    {
	എങ്കിൽ (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	അല്ലെങ്കിൽ എങ്കിൽ (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	അല്ലെങ്കിൽ
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    എങ്കിൽ ((initdir == ശൂന്യ || *initdir == NUL) && dflt != ശൂന്യ && *dflt != NUL)
    {
	എങ്കിൽ (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = ശൂന്യ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    എങ്കിൽ (tofree != ശൂന്യ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    എങ്കിൽ (initdir == ശൂന്യ || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	എങ്കിൽ (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	അല്ലെങ്കിൽ എങ്കിൽ (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != ശൂന്യ && buf->b_ffname != ശൂന്യ)
	{
	    എങ്കിൽ (dflt == ശൂന്യ || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    എങ്കിൽ (tofree != ശൂന്യ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	അല്ലെങ്കിൽ എങ്കിൽ (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)		// when this changes, also adjust f_has()!
    {
	എങ്കിൽ (filter == ശൂന്യ
#  ifdef FEAT_EVAL
		&& (filter = get_var_മൂല്യം((char_u *)"b:browsefilter")) == ശൂന്യ
		&& (filter = get_var_മൂല്യം((char_u *)"g:browsefilter")) == ശൂന്യ
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	എങ്കിൽ (flags & BROWSE_DIR)
	{
#  എങ്കിൽ defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  അല്ലെങ്കിൽ
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  എങ്കിൽ !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    എങ്കിൽ (fname != ശൂന്യ && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		എങ്കിൽ (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	അല്ലെങ്കിൽ
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    അല്ലെങ്കിൽ
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = ശൂന്യ;
    }

    // keep the directory for next time
    എങ്കിൽ (fname != ശൂന്യ)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	എങ്കിൽ (last_dir != ശൂന്യ && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    എങ്കിൽ (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		എങ്കിൽ (last_dir != ശൂന്യ)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    തിരികെ fname;
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    ശൂന്യം
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    പൂർണ്ണസംഖ്യ		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	തിരികെ;

    save = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    എങ്കിൽ (error || title == ശൂന്യ || initdir == ശൂന്യ || defname == ശൂന്യ)
	rettv->vval.v_string = ശൂന്യ;
    അല്ലെങ്കിൽ
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, ശൂന്യ, initdir, ശൂന്യ, curbuf);
# അല്ലെങ്കിൽ
    rettv->vval.v_string = ശൂന്യ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    ശൂന്യം
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    എങ്കിൽ (title == ശൂന്യ || initdir == ശൂന്യ)
	rettv->vval.v_string = ശൂന്യ;
    അല്ലെങ്കിൽ
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, ശൂന്യ, ശൂന്യ, initdir, ശൂന്യ, curbuf);
# അല്ലെങ്കിൽ
    rettv->vval.v_string = ശൂന്യ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    ശൂന്യം
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_സംഖ്യ = FALSE;

    എങ്കിൽ (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	തിരികെ;

    from = tv_get_string(&argvars[0]);

    എങ്കിൽ (mch_lstat((അക്ഷരം *)from, &st) >= 0
	&& (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)))
	rettv->vval.v_സംഖ്യ = vim_copyfile(
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
    പൂർണ്ണസംഖ്യ		dstlen,	// maximum length of the result
    പൂർണ്ണസംഖ്യ		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    എങ്കിൽ (src == ശൂന്യ)
    {
	*dst = NUL;
	തിരികെ 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    എങ്കിൽ (buf != ശൂന്യ && buf->b_help)
	തിരികെ vim_snprintf_safelen((അക്ഷരം *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    എങ്കിൽ (homedir != ശൂന്യ)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#അല്ലെങ്കിൽ
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    എങ്കിൽ (homedir_env == ശൂന്യ)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    എങ്കിൽ (homedir_env != ശൂന്യ && *homedir_env == NUL)
	homedir_env = ശൂന്യ;

    എങ്കിൽ (homedir_env != ശൂന്യ && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = ശൂന്യ;

	flen = STRLEN(homedir_env);
	(ശൂന്യം)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	എങ്കിൽ (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    എങ്കിൽ (homedir_env != ശൂന്യ)
	envlen = STRLEN(homedir_env);

    എങ്കിൽ (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    ആകുമ്പോൾ (*src && dstlen > 0)
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
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		എങ്കിൽ (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		നിർത്തുക;
	    }
	    എങ്കിൽ (p == homedir_env)
		നിർത്തുക;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	ആകുമ്പോൾ (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	ആകുമ്പോൾ ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    എങ്കിൽ (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    തിരികെ (size_t)(dst - dst_start);
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
    എങ്കിൽ (src != ശൂന്യ)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    എങ്കിൽ (dst != ശൂന്യ)
	home_replace(buf, src, dst, len, TRUE);
    തിരികെ dst;
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
    പൂർണ്ണസംഖ്യ
fullpathcmp(
    char_u *s1,
    char_u *s2,
    പൂർണ്ണസംഖ്യ	    checkname,		// when both don't exist, check file names
    പൂർണ്ണസംഖ്യ	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    പൂർണ്ണസംഖ്യ		    r1, r2;

    എങ്കിൽ (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    അല്ലെങ്കിൽ
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((അക്ഷരം *)exp1, &st1);
    r2 = mch_stat((അക്ഷരം *)s2, &st2);
    എങ്കിൽ (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	എങ്കിൽ (checkname)
	{
	    എങ്കിൽ (fnamecmp(exp1, s2) == 0)
		തിരികെ FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    എങ്കിൽ (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		തിരികെ FPC_SAMEX;
	}
	തിരികെ FPC_NOTX;
    }
    എങ്കിൽ (r1 != 0 || r2 != 0)
	തിരികെ FPC_DIFFX;
    എങ്കിൽ (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	തിരികെ FPC_SAME;
    തിരികെ FPC_DIFF;
#അല്ലെങ്കിൽ
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    പൂർണ്ണസംഖ്യ	    retval = FPC_DIFF;
    പൂർണ്ണസംഖ്യ	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    എങ്കിൽ ((exp1 = alloc(MAXPATHL * 3)) != ശൂന്യ)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	എങ്കിൽ (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	അല്ലെങ്കിൽ
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	എങ്കിൽ (r1 != OK && r2 != OK)
	{
	    എങ്കിൽ (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    അല്ലെങ്കിൽ
		retval = FPC_NOTX;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	അല്ലെങ്കിൽ എങ്കിൽ (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	അല്ലെങ്കിൽ
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    തിരികെ retval;
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

    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ (char_u *)"";
    വേണ്ടി (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	എങ്കിൽ (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    തിരികെ p1;
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
    ആകുമ്പോൾ (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    തിരികെ t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    ആകുമ്പോൾ (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    എങ്കിൽ (*fname)
	++fname;
    തിരികെ fname;
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

#എങ്കിൽ defined(MSWIN)
    // may skip "c:"
    എങ്കിൽ (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    അല്ലെങ്കിൽ
	retval = path;
#അല്ലെങ്കിൽ
# എങ്കിൽ defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    എങ്കിൽ (retval == ശൂന്യ)
	retval = path;
# അല്ലെങ്കിൽ	// Unix
    retval = path;
# endif
#endif

    ആകുമ്പോൾ (vim_ispathsep(*retval))
	++retval;

    തിരികെ retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    പൂർണ്ണസംഖ്യ
vim_ispathsep(പൂർണ്ണസംഖ്യ c)
{
#ifdef UNIX
    തിരികെ (c == '/');	    // UNIX has ':' inside file names
#അല്ലെങ്കിൽ
# ifdef BACKSLASH_IN_FILENAME
    തിരികെ (c == ':' || c == '/' || c == '\\');
# അല്ലെങ്കിൽ
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    തിരികെ (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  അല്ലെങ്കിൽ
    തിരികെ (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    പൂർണ്ണസംഖ്യ
vim_ispathsep_nocolon(പൂർണ്ണസംഖ്യ c)
{
    തിരികെ vim_ispathsep(c)
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
    പൂർണ്ണസംഖ്യ
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		retval;

    p = gettail_sep(fname);
    എങ്കിൽ (p == fname)
	തിരികെ TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    തിരികെ retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    പൂർണ്ണസംഖ്യ
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    തിരികെ vim_fnamencmp(x, y, MAXPATHL);
#അല്ലെങ്കിൽ
    എങ്കിൽ (p_fic)
	തിരികെ MB_STRICMP(x, y);
    തിരികെ STRCMP(x, y);
#endif
}

    പൂർണ്ണസംഖ്യ
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    പൂർണ്ണസംഖ്യ		cx = NUL;
    പൂർണ്ണസംഖ്യ		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    വേണ്ടി (പൂർണ്ണസംഖ്യ swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	എങ്കിൽ ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    എങ്കിൽ (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    ആകുമ്പോൾ (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	എങ്കിൽ (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    നിർത്തുക;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    എങ്കിൽ (len == 0)
	തിരികെ 0;
    തിരികെ (cx - cy);
#അല്ലെങ്കിൽ
    എങ്കിൽ (p_fic)
	തിരികെ MB_STRNICMP(x, y, len);
    തിരികെ STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, പൂർണ്ണസംഖ്യ sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? വലുപ്പം(PATHSEP) : 0) + fname2len + 1);
    എങ്കിൽ (ret->string == ശൂന്യ)
	ret->length = 0;
    അല്ലെങ്കിൽ
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	എങ്കിൽ (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += വലുപ്പം(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    തിരികെ ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    ശൂന്യം
add_pathsep(char_u *p)
{
    size_t  plen;

    എങ്കിൽ (*p == NUL)
	തിരികെ;

    plen = STRLEN(p);
    എങ്കിൽ (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    പൂർണ്ണസംഖ്യ		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = ശൂന്യ;

    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ ശൂന്യ;

    buf = alloc(MAXPATHL);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    അല്ലെങ്കിൽ
	new_fname = vim_strsave(fname);
    vim_free(buf);
    തിരികെ new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    പൂർണ്ണസംഖ്യ
vim_fexists(char_u *fname)
{
    stat_T st;

    എങ്കിൽ (mch_stat((അക്ഷരം *)fname, &st))
	തിരികെ FALSE;
    തിരികെ TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    പൂർണ്ണസംഖ്യ		  *അക്കം_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    പൂർണ്ണസംഖ്യ		   flags)	// EW_DIR, etc.
{
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    char_u	*eval_pat = ശൂന്യ;
    char_u	*exp_pat = *pat;
    അക്ഷരം	*ignored_സന്ദേശം;
    size_t	usedlen;
    പൂർണ്ണസംഖ്യ		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    പൂർണ്ണസംഖ്യ		star_follows = FALSE;

    എങ്കിൽ (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       ശൂന്യ, &ignored_സന്ദേശം, ശൂന്യ, TRUE);
	--emsg_off;
	എങ്കിൽ (eval_pat != ശൂന്യ)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    എങ്കിൽ (exp_pat != ശൂന്യ)
	ret = expand_wildcards(1, &exp_pat, അക്കം_file, file, flags);

    എങ്കിൽ (eval_pat != ശൂന്യ)
    {
	എങ്കിൽ (*അക്കം_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    എങ്കിൽ (*file != ശൂന്യ)
	    {
		**file = eval_pat;
		eval_pat = ശൂന്യ;
		*അക്കം_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    തിരികെ ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    പൂർണ്ണസംഖ്യ
expand_wildcards(
    പൂർണ്ണസംഖ്യ		   അക്കം_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    പൂർണ്ണസംഖ്യ		  *അക്കം_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    പൂർണ്ണസംഖ്യ		   flags)	// EW_DIR, etc.
{
    പൂർണ്ണസംഖ്യ		retval;
    പൂർണ്ണസംഖ്യ		i, j;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(അക്കം_pat, pat, അക്കം_files, files, flags);

    // When keeping all matches, return here
    എങ്കിൽ ((flags & EW_KEEPALL) || retval == FAIL)
	തിരികെ retval;

    /*
     * Remove names that match 'wildignore'.
     */
    എങ്കിൽ (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	വേണ്ടി (i = 0; i < *അക്കം_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    എങ്കിൽ (ffname == ശൂന്യ)		// out of memory
		നിർത്തുക;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    എങ്കിൽ (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		വേണ്ടി (j = i; j + 1 < *അക്കം_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*അക്കം_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	എങ്കിൽ (*അക്കം_files == 0)
	{
	    VIM_CLEAR(*files);
	    തിരികെ FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    എങ്കിൽ (*അക്കം_files > 1 && !got_int)
    {
	non_suf_match = 0;
	വേണ്ടി (i = 0; i < *അക്കം_files; ++i)
	{
	    എങ്കിൽ (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		വേണ്ടി (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    തിരികെ retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    പൂർണ്ണസംഖ്യ
match_suffix(char_u *fname)
{
    പൂർണ്ണസംഖ്യ		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (പൂർണ്ണസംഖ്യ)STRLEN(fname);
    setsuflen = 0;
    വേണ്ടി (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	എങ്കിൽ (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    എങ്കിൽ (vim_strchr(tail, '.') == ശൂന്യ)
	    {
		setsuflen = 1;
		നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		നിർത്തുക;
	    setsuflen = 0;
	}
    }
    തിരികെ (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vim_backtick(char_u *p)
{
    തിരികെ (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    പൂർണ്ണസംഖ്യ		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    പൂർണ്ണസംഖ്യ		cnt = 0;
    പൂർണ്ണസംഖ്യ		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    എങ്കിൽ (cmd == ശൂന്യ)
	തിരികെ -1;

#ifdef FEAT_EVAL
    എങ്കിൽ (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    അല്ലെങ്കിൽ
#endif
	buffer = get_cmd_output(cmd, ശൂന്യ,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, ശൂന്യ);
    vim_free(cmd);
    എങ്കിൽ (buffer == ശൂന്യ)
	തിരികെ -1;

    cmd = buffer;
    ആകുമ്പോൾ (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	ആകുമ്പോൾ (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	എങ്കിൽ (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	ആകുമ്പോൾ (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    തിരികെ cnt;
}

#എങ്കിൽ defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
pstrcmp(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b)
{
    തിരികെ (pathcmp(*(അക്ഷരം **)a, *(അക്ഷരം **)b, -1));
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
    പൂർണ്ണസംഖ്യ
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    പൂർണ്ണസംഖ്യ		flags,		// EW_* flags
    പൂർണ്ണസംഖ്യ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    പൂർണ്ണസംഖ്യ		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		starts_with_dot;
    പൂർണ്ണസംഖ്യ		matches;
    size_t	buflen;
    size_t	len;
    പൂർണ്ണസംഖ്യ		starstar = FALSE;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_മൂല്യം;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = ശൂന്യ;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    പൂർണ്ണസംഖ്യ			ok;
    char_u		*p_alt;
    പൂർണ്ണസംഖ്യ			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    എങ്കിൽ (stardepth > 0)
    {
	ui_breakcheck();
	എങ്കിൽ (got_int)
	    തിരികെ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ശൂന്യ;
    path_end = path;
    ആകുമ്പോൾ (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	എങ്കിൽ (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	അല്ലെങ്കിൽ എങ്കിൽ (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    എങ്കിൽ (e != ശൂന്യ)
		നിർത്തുക;
	    s = p + 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != ശൂന്യ)
	    e = p;
	എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	അല്ലെങ്കിൽ
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    വേണ്ടി (p = buf + wildoff; p < s; ++p)
	എങ്കിൽ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    വേണ്ടി (p = s; p < e; ++p)
	എങ്കിൽ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ശൂന്യ, FALSE);
    എങ്കിൽ (pat == ശൂന്യ)
    {
	vim_free(buf);
	തിരികെ 0;
    }

    // compile the regexp into a program
    എങ്കിൽ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    എങ്കിൽ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    എങ്കിൽ (regmatch.regprog == ശൂന്യ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	തിരികെ 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != ശൂന്യ;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    എങ്കിൽ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((അക്ഷരം *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(ശൂന്യം)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((അക്ഷരം *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, ശൂന്യ);
    എങ്കിൽ (wn != ശൂന്യ)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_മൂല്യം);

    ആകുമ്പോൾ (ok)
    {
	p = utf16_to_enc(wfb.cFileName, ശൂന്യ);   // p is allocated here

	എങ്കിൽ (p == ശൂന്യ)
	    നിർത്തുക;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	എങ്കിൽ (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = ശൂന്യ;
	അല്ലെങ്കിൽ
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, ശൂന്യ);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	എങ്കിൽ ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == ശൂന്യ
		  || (regmatch.regprog != ശൂന്യ
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != ശൂന്യ
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((അക്ഷരം *)s, buflen - len, "%s", p);
	    എങ്കിൽ (len + 1 < buflen)
	    {
		എങ്കിൽ (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((അക്ഷരം *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (ശൂന്യം)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((അക്ഷരം *)buf + len, buflen - len, "%s", path_end);
		എങ്കിൽ (mch_has_exp_wildcard(path_end))
		{
		    എങ്കിൽ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ശൂന്യം)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    എങ്കിൽ (*path_end != 0)
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
    എങ്കിൽ (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   വലുപ്പം(char_u *), pstrcmp);
    തിരികെ matches;
}
#endif // MSWIN

#എങ്കിൽ (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
pstrcmp(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b)
{
    തിരികെ (pathcmp(*(അക്ഷരം **)a, *(അക്ഷരം **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    പൂർണ്ണസംഖ്യ
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    പൂർണ്ണസംഖ്യ		flags,		// EW_* flags
    പൂർണ്ണസംഖ്യ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    പൂർണ്ണസംഖ്യ		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		starts_with_dot;
    പൂർണ്ണസംഖ്യ		matches;
    size_t	buflen;
    size_t	len;
    പൂർണ്ണസംഖ്യ		starstar = FALSE;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    ഘടന dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    എങ്കിൽ (stardepth > 0)
    {
	ui_breakcheck();
	എങ്കിൽ (got_int)
	    തിരികെ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ശൂന്യ;
    path_end = path;
    ആകുമ്പോൾ (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	എങ്കിൽ (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	അല്ലെങ്കിൽ എങ്കിൽ (*path_end == '/')
	{
	    എങ്കിൽ (e != ശൂന്യ)
		നിർത്തുക;
	    s = p + 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != ശൂന്യ
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	അല്ലെങ്കിൽ
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    വേണ്ടി (p = buf + wildoff; p < s; ++p)
	എങ്കിൽ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    വേണ്ടി (p = s; p < e; ++p)
	എങ്കിൽ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ശൂന്യ, FALSE);
    എങ്കിൽ (pat == ശൂന്യ)
    {
	vim_free(buf);
	തിരികെ 0;
    }

    // compile the regexp into a program
    എങ്കിൽ (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    അല്ലെങ്കിൽ
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    എങ്കിൽ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    എങ്കിൽ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    എങ്കിൽ (regmatch.regprog == ശൂന്യ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	തിരികെ 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    എങ്കിൽ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((അക്ഷരം *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(ശൂന്യം)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (അക്ഷരം *)buf);

    // Find all matching entries
    എങ്കിൽ (dirp != ശൂന്യ)
    {
	ആകുമ്പോൾ (!got_int)
	{
	    dp = readdir(dirp);
	    എങ്കിൽ (dp == ശൂന്യ)
		നിർത്തുക;
	    len = (size_t)(s - buf);
	    എങ്കിൽ ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != ശൂന്യ && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((അക്ഷരം *)s, buflen - len, "%s", dp->d_name);
		എങ്കിൽ (len + 1 >= buflen)
		    തുടരുക;

		എങ്കിൽ (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((അക്ഷരം *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (ശൂന്യം)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((അക്ഷരം *)buf + len, buflen - len, "%s", path_end);
		എങ്കിൽ (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    എങ്കിൽ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ശൂന്യം)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    എങ്കിൽ (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    എങ്കിൽ ((flags & EW_ALLLINKS) ? mch_lstat((അക്ഷരം *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			എങ്കിൽ (precomp_buf)
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
    എങ്കിൽ (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   വലുപ്പം(char_u *), pstrcmp);
    തിരികെ matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
has_env_var(char_u *p)
{
    വേണ്ടി ( ; *p; MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == '\\' && p[1] != NUL)
	    ++p;
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)
#എങ്കിൽ defined(MSWIN)
				    "$%"
#അല്ലെങ്കിൽ
				    "$"
#endif
					, *p) != ശൂന്യ)
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
has_special_wildchar(char_u *p)
{
    വേണ്ടി ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	എങ്കിൽ (*p == '\r' || *p == '\n')
	    നിർത്തുക;
	// Allow for escaping.
	എങ്കിൽ (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != ശൂന്യ)
	{
	    // A { must be followed by a matching }.
	    എങ്കിൽ (*p == '{' && vim_strchr(p, '}') == ശൂന്യ)
		തുടരുക;
	    // A quote and backtick must be followed by another one.
	    എങ്കിൽ ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == ശൂന്യ)
		തുടരുക;
	    തിരികെ TRUE;
	}
    }
    തിരികെ FALSE;
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
    പൂർണ്ണസംഖ്യ
gen_expand_wildcards(
    പൂർണ്ണസംഖ്യ		അക്കം_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    പൂർണ്ണസംഖ്യ		*അക്കം_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    പൂർണ്ണസംഖ്യ		flags)		// EW_* flags
{
    പൂർണ്ണസംഖ്യ			i;
    garray_T		ga;
    char_u		*p;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		recursive = FALSE;
    പൂർണ്ണസംഖ്യ			add_pat;
    പൂർണ്ണസംഖ്യ			retval = OK;
    പൂർണ്ണസംഖ്യ			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    എങ്കിൽ (recursive)
#ifdef SPECIAL_WILDCHAR
	തിരികെ mch_expand_wildcards(അക്കം_pat, pat, അക്കം_file, file, flags);
#അല്ലെങ്കിൽ
	തിരികെ FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    വേണ്ടി (i = 0; i < അക്കം_pat; i++)
    {
	എങ്കിൽ (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    തിരികെ mch_expand_wildcards(അക്കം_pat, pat, അക്കം_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, വലുപ്പം(char_u *), 30);

    വേണ്ടി (i = 0; i < അക്കം_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	എങ്കിൽ (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    എങ്കിൽ (add_pat == -1)
		retval = FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    എങ്കിൽ ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		എങ്കിൽ (p == ശൂന്യ)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		അല്ലെങ്കിൽ എങ്കിൽ (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(അക്കം_pat, pat, അക്കം_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    തിരികെ i;
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
	    എങ്കിൽ (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		എങ്കിൽ ((flags & (EW_PATH | EW_CDPATH))
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
		അല്ലെങ്കിൽ
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	എങ്കിൽ (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    എങ്കിൽ (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    അല്ലെങ്കിൽ
		addfile(&ga, t, flags);

	    എങ്കിൽ (t != p)
		vim_free(t);
	}

	എങ്കിൽ (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	എങ്കിൽ (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    എങ്കിൽ (retval == FAIL)
	ga_clear_strings(&ga);

    *അക്കം_file = ga.ga_len;
    *file = (ga.ga_data != ശൂന്യ) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    തിരികെ ((flags & EW_EMPTYOK) || ga.ga_data != ശൂന്യ) ? retval : FAIL;
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
    ശൂന്യം
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    പൂർണ്ണസംഖ്യ		flags)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		isdir;
    stat_T	sb;

#ifdef MSWIN
    എങ്കിൽ (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	എങ്കിൽ (perm < 0)
	    തിരികെ;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    അല്ലെങ്കിൽ
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	എങ്കിൽ (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((അക്ഷരം *)f, &sb) < 0 : mch_getperm(f) < 0))
	    തിരികെ;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    എങ്കിൽ (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != ശൂന്യ)
	തിരികെ;
#endif

    എങ്കിൽ ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	തിരികെ;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    എങ്കിൽ (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, ശൂന്യ, !(flags & EW_SHELLCMD)))
	തിരികെ;

    // Make room for another item in the file list.
    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ;

    p = alloc(STRLEN(f) + 1 + isdir);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    എങ്കിൽ (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    ശൂന്യം
FreeWild(പൂർണ്ണസംഖ്യ എണ്ണം, char_u **files)
{
    എങ്കിൽ (എണ്ണം <= 0 || files == ശൂന്യ)
	തിരികെ;
    ആകുമ്പോൾ (എണ്ണം--)
	vim_free(files[എണ്ണം]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    പൂർണ്ണസംഖ്യ
pathcmp(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *q, പൂർണ്ണസംഖ്യ maxlen)
{
    പൂർണ്ണസംഖ്യ		i, j;
    പൂർണ്ണസംഖ്യ		c1, c2;
    സ്ഥിരം അക്ഷരം	*s = ശൂന്യ;

    വേണ്ടി (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	എങ്കിൽ (c1 == NUL)
	{
	    എങ്കിൽ (c2 == NUL)  // full match
		തിരികെ 0;
	    s = q;
	    i = j;
	    നിർത്തുക;
	}

	// End of "q": check if "p" just has a slash.
	എങ്കിൽ (c2 == NUL)
	{
	    s = p;
	    നിർത്തുക;
	}

	എങ്കിൽ ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    എങ്കിൽ (vim_ispathsep(c1))
		തിരികെ -1;
	    എങ്കിൽ (vim_ispathsep(c2))
		തിരികെ 1;
	    തിരികെ p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    എങ്കിൽ (s == ശൂന്യ)	// "i" or "j" ran into "maxlen"
	തിരികെ 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    എങ്കിൽ (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#അല്ലെങ്കിൽ
	    && c1 == '/'
#endif
       )
	തിരികെ 0;   // match with trailing slash
    എങ്കിൽ (s == q)
	തിരികെ -1;	    // no match
    തിരികെ 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    പൂർണ്ണസംഖ്യ
vim_isAbsName(char_u *name)
{
    തിരികെ (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		force)	    // force expansion even when already absolute
{
    പൂർണ്ണസംഖ്യ		retval = OK;
    പൂർണ്ണസംഖ്യ		url;

    *buf = NUL;
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ FAIL;

    url = path_with_url(fname);
    എങ്കിൽ (!url)
	retval = mch_FullName(fname, buf, len, force);
    എങ്കിൽ (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#എങ്കിൽ defined(MSWIN)
    slash_adjust(buf);
#endif
    തിരികെ retval;
}
