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

#অন্তর্ভাব "vim.h"

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
    স্থির পূর্ণসংখ্যা
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    পূর্ণসংখ্যা		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * আকার(*newbuf));
    যদি (newbuf == ফাঁকা)
	ফেরত FAIL;

    wfname = enc_to_utf16(*fnamep, ফাঁকা);
    যদি (wfname == ফাঁকা)
    {
	vim_free(newbuf);
	ফেরত FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    যদি (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * আকার(*newbuf));
	যদি (newbuf == ফাঁকা)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    ফেরত FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    যদি (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, ফাঁকা);

	যদি (p != ফাঁকা)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	অন্যথা
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    ফেরত FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = ফাঁকা;
    char_u	*short_fname = ফাঁকা;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    পূর্ণসংখ্যা		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    যদি (save_fname == ফাঁকা)
    {
	retval = FAIL;
	যাও theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    জন্য (;;)
    {
	// go back one path-separator
	যখন (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	যদি (endp <= save_fname)
	    বিরতি;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	যদি (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    যাও theend;
	}
	*endp = ch;	// preserve the string

	যদি (len > 0)
	    বিরতি;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    যদি (len > 0)
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
	যদি (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    যদি (*fname == ফাঁকা)
	    {
		retval = FAIL;
		যাও theend;
	    }
	}
	অন্যথা
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    যদি (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = ফাঁকা;
	}

	// concat the not-shortened part of the path
	যদি ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    ফেরত retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    স্থির পূর্ণসংখ্যা
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    পূর্ণসংখ্যা		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    পূর্ণসংখ্যা		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    জন্য (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	যদি (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    যদি (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    অন্যথা
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    যদি (tfname == ফাঁকা)
	ফেরত FAIL;

    len = tflen = STRLEN(tfname);

    যদি (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	ফেরত FAIL;

    যদি (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	যদি (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    ফেরত FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    জন্য (p = tfname + len - 1; p >= tfname; --p)
    {
	যদি (has_mbyte)
	    p -= mb_শীর্ষ_off(tfname, p);
	যদি (vim_ispathsep(*p))
	{
	    যদি (sepcount == 0 || (hasTilde && sepcount == 1))
		বিরতি;
	    অন্যথা
		sepcount --;
	}
    }
    যদি (hasTilde)
    {
	--p;
	যদি (p >= tfname)
	    *p = '~';
	অন্যথা
	    ফেরত FAIL;
    }
    অন্যথা
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    ফেরত OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    পূর্ণসংখ্যা
modify_fname(
    char_u	*src,		// string with modifiers
    পূর্ণসংখ্যা		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    পূর্ণসংখ্যা		valid = 0;
    char_u	*লেজ;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		has_fullname = 0;
    পূর্ণসংখ্যা		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    পূর্ণসংখ্যা		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    যদি (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	যদি ((*fnamep)[0] == '~'
#যদি !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    যদি (*fnamep == ফাঁকা)
		ফেরত -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	জন্য (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    যদি (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		বিরতি;
	}

	// FullName_save() is slow, don't use it when not needed.
	যদি (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    যদি (*fnamep == ফাঁকা)
		ফেরত -1;
	}

#ifdef MSWIN
# যদি _WIN32_WINNT >= 0x0500
	যদি (vim_strchr(*fnamep, '~') != ফাঁকা)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, ফাঁকা);
	    WCHAR buf[_MAX_PATH];

	    যদি (wfname != ফাঁকা)
	    {
		যদি (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, ফাঁকা);

		    যদি (q != ফাঁকা)
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
	যদি (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    যদি (*fnamep == ফাঁকা)
		ফেরত -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    যখন (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	যদি (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    চালু;
	}
	pbuf = ফাঁকা;
	// Need full path first (use expand_env() to remove a "~/")
	যদি (!has_fullname && !has_homerelative)
	{
	    যদি (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    অন্যথা
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	অন্যথা
	    p = *fnamep;

	has_fullname = 0;

	যদি (p != ফাঁকা)
	{
	    size_t  dirnamelen = 0;

	    যদি (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		যদি (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    যদি (s != ফাঁকা)
		    {
			dirnamelen = home_replace(ফাঁকা, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		যদি (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		যদি (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    যদি (vim_ispathsep(*p))
		    {
			যখন (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			যদি (pbuf != ফাঁকা)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = ফাঁকা;
			}
		    }
		}
	    }
	    অন্যথা
	    {
		dirnamelen = home_replace(ফাঁকা, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		যদি (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    যদি (s != ফাঁকা)
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

    লেজ = gettail(*fnamep);
    *fnamelen = STRLEN(*fnamep);

    // ":h" - head, remove "/file_name", can be repeated
    // Don't remove the first "/" or "c:\"
    যখন (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_শীর্ষ;
	*usedlen += 2;
	s = get_past_শীর্ষ(*fnamep);
	যখন (লেজ > s && after_pathsep(s, লেজ))
	    MB_PTR_BACK(*fnamep, লেজ);
	*fnamelen = লেজ - *fnamep;
#ifdef VMS
	যদি (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	যদি (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    যদি (p == ফাঁকা)
		ফেরত -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = লেজ = p;
	    *fnamelen = 1;
	}
	অন্যথা
	{
	    যখন (লেজ > s && !after_pathsep(s, লেজ))
		MB_PTR_BACK(*fnamep, লেজ);
	}
    }

    // ":8" - shortname
    যদি (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    যদি (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	যদি (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    যদি (p == ফাঁকা)
		ফেরত -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	যদি (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    যদি (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		ফেরত -1;
	}
	অন্যথা
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    যদি (get_short_pathname(fnamep, bufp, &l) == FAIL)
		ফেরত -1;

	    যদি (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		যদি (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    ফেরত -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	লেজ = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    যদি (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= লেজ - *fnamep;
	*fnamep = লেজ;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    যখন (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	যদি (src[*usedlen + 1] == 'e' && *fnamep > লেজ)
	    s = *fnamep - 2;
	অন্যথা
	    s = *fnamep + *fnamelen - 1;
	জন্য ( ; s > লেজ; --s)
	    যদি (s[0] == '.')
		বিরতি;
	যদি (src[*usedlen + 1] == 'e')		// :e
	{
	    যদি (s > লেজ)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		জন্য ( ; s > *fnamep; --s)
		    যদি (s[0] == ';')
			বিরতি;
		যদি (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    অন্যথা যদি (*fnamep <= লেজ)
		*fnamelen = 0;
	}
	অন্যথা				// :r
	{
	    char_u *limit = *fnamep;

	    যদি (limit < লেজ)
		limit = লেজ;
	    যদি (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    যদি (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	পূর্ণসংখ্যা	    sep;
	char_u	    *flags;
	পূর্ণসংখ্যা	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	যদি (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	যদি (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    যদি (p != ফাঁকা)
	    {
		pat = vim_strnsave(s, p - s);
		যদি (pat != ফাঁকা)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    যদি (p != ফাঁকা)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			যদি (sub != ফাঁকা && str != ফাঁকা)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, ফাঁকা, flags, &slen);
			    যদি (s != ফাঁকা)
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
	    যদি (didit)
		যাও repeat;
	}
    }

    যদি (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	যদি (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	যদি (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	যদি (p == ফাঁকা)
	    ফেরত -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    ফেরত valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    স্থির শূন্য
shorten_dir_len(char_u *str, পূর্ণসংখ্যা trim_len)
{
    char_u	*লেজ, *s, *d;
    পূর্ণসংখ্যা		skip = FALSE;
    পূর্ণসংখ্যা		dirchunk_len = 0;

    লেজ = gettail(str);
    d = str;
    জন্য (s = str; ; ++s)
    {
	যদি (s >= লেজ)		    // copy the whole tail
	{
	    *d++ = *s;
	    যদি (*s == NUL)
		বিরতি;
	}
	অন্যথা যদি (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	অন্যথা যদি (!skip)
	{
	    *d++ = *s;			// copy next char
	    যদি (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		যদি (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    যদি (has_mbyte)
	    {
		পূর্ণসংখ্যা l = mb_ptr2len(s);

		যখন (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    শূন্য
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    পূর্ণসংখ্যা
file_is_readable(char_u *fname)
{
    পূর্ণসংখ্যা		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    যদি (*fname == NUL || mch_isdir_or_missing(fname))
	ফেরত FALSE;
    যদি ((fd = mch_open((অক্ষর *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

#যদি defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    শূন্য
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (check_secure())
	ফেরত;

    যদি (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	যদি (in_vim9script())
	    (শূন্য) check_for_string_arg(argvars, 0);
	ফেরত;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    যদি (cwd != ফাঁকা)
    {
	যদি (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	যদি (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	অন্যথা যদি (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	অন্যথা যদি (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	অন্যথা
	{
	    semsg(_(e_invalid_মান_for_argument_str_str), "scope", s);
	    ফেরত;
	}
    }
    অন্যথা যদি (curwin->w_localdir != ফাঁকা)
	scope = CDSCOPE_WINDOW;
    অন্যথা যদি (curtab->tp_localdir != ফাঁকা)
	scope = CDSCOPE_TABPAGE;

    যদি (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    শূন্য
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_সংখ্যা = -1;
    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ফেরত;

    name = tv_get_string(&argvars[0]);
    যদি (name == ফাঁকা || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	ফেরত;
    }

    যদি (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    অন্যথা
	flags = (char_u *)"";

    যদি (*flags == NUL)
	// delete a file
	rettv->vval.v_সংখ্যা = mch_remove(name) == 0 ? 0 : -1;
    অন্যথা যদি (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_সংখ্যা = mch_rmdir(name) == 0 ? 0 : -1;
    অন্যথা যদি (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_সংখ্যা = delete_recursive(name);
    অন্যথা
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    শূন্য
f_executable(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_সংখ্যা = mch_can_exe(tv_get_string(&argvars[0]), ফাঁকা, TRUE);
}

/*
 * "exepath()" function
 */
    শূন্য
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = ফাঁকা;

    যদি (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ফেরত;
    (শূন্য)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    শূন্য
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;
    rettv->vval.v_সংখ্যা = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    শূন্য
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;
    rettv->vval.v_সংখ্যা = filewritable(tv_get_string(&argvars[0]));
}

    স্থির শূন্য
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    পূর্ণসংখ্যা		find_what)
{
    char_u	*fname;
    char_u	*fresult = ফাঁকা;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    পূর্ণসংখ্যা		গুনতি = 1;
    পূর্ণসংখ্যা		first = TRUE;
    পূর্ণসংখ্যা		error = FALSE;

    rettv->vval.v_string = ফাঁকা;
    rettv->v_type = VAR_STRING;
    যদি (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ফেরত;

    fname = tv_get_string(&argvars[0]);

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	যদি (p == ফাঁকা)
	    error = TRUE;
	অন্যথা
	{
	    যদি (*p != NUL)
		path = p;

	    যদি (argvars[2].v_type != VAR_UNKNOWN)
		গুনতি = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[2], &error);
	}
    }

    যদি (গুনতি < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    যদি (*fname != NUL && !error)
    {
	char_u	*file_to_find = ফাঁকা;
	অক্ষর	*অনুসন্ধান_ctx = ফাঁকা;

	করো
	{
	    যদি (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : ফাঁকা,
					       first ? (পূর্ণসংখ্যা)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &অনুসন্ধান_ctx);
	    first = FALSE;

	    যদি (fresult != ফাঁকা && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} যখন ((rettv->v_type == VAR_LIST || --গুনতি > 0) && fresult != ফাঁকা);

	vim_free(file_to_find);
	vim_findfile_cleanup(অনুসন্ধান_ctx);
    }

    যদি (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    শূন্য
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    শূন্য
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    শূন্য
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = ফাঁকা;
    char_u	buf[NUMBUFLEN];

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    যদি (mods == ফাঁকা || fname == ফাঁকা)
	fname = ফাঁকা;
    অন্যথা
    {
	len = STRLEN(fname);
	যদি (mods != ফাঁকা && *mods != NUL)
	    (শূন্য)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    যদি (fname == ফাঁকা)
	rettv->vval.v_string = ফাঁকা;
    অন্যথা
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
    শূন্য
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = ফাঁকা;
    tabpage_T	*tp = ফাঁকা;
    char_u	*cwd;
    পূর্ণসংখ্যা		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL)))
	ফেরত;

    যদি (argvars[0].v_type == VAR_সংখ্যা
	    && argvars[0].vval.v_সংখ্যা == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    অন্যথা
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    যদি (wp != ফাঁকা && wp->w_localdir != ফাঁকা
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    অন্যথা যদি (tp != ফাঁকা && tp->tp_localdir != ফাঁকা
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    অন্যথা যদি (wp != ফাঁকা || tp != ফাঁকা || global)
    {
	যদি (globaldir != ফাঁকা && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	অন্যথা
	{
	    cwd = alloc(MAXPATHL);
	    যদি (cwd != ফাঁকা)
	    {
		যদি (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    যদি (rettv->vval.v_string != ফাঁকা)
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
    পূর্ণসংখ্যা		    i;

    জন্য (i = 0; i < 9; i++)
    {
	যদি (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	অন্যথা
	    perm[i] = '-';
    }
    ফেরত perm;
}

/*
 * "getfperm({fname})" function
 */
    শূন্য
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = ফাঁকা;
    char_u	permbuf[] = "---------";

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    যদি (mch_stat((অক্ষর *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    শূন্য
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    fname = tv_get_string(&argvars[0]);
    যদি (mch_stat((অক্ষর *)fname, &st) >= 0)
    {
	যদি (S_ISDIR(st.st_mode))
	    rettv->vval.v_সংখ্যা = 0;
	অন্যথা
	{
	    rettv->vval.v_সংখ্যা = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    যদি ((off_T)rettv->vval.v_সংখ্যা != (off_T)st.st_size)
		rettv->vval.v_সংখ্যা = -2;
	}
    }
    অন্যথা
	  rettv->vval.v_সংখ্যা = -1;
}

/*
 * "getftime({fname})" function
 */
    শূন্য
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    fname = tv_get_string(&argvars[0]);
    যদি (mch_stat((অক্ষর *)fname, &st) >= 0)
	rettv->vval.v_সংখ্যা = (varnumber_T)st.st_mtime;
    অন্যথা
	rettv->vval.v_সংখ্যা = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    অক্ষর    *t;

    যদি (S_ISREG(st->st_mode))
	t = "file";
    অন্যথা যদি (S_ISDIR(st->st_mode))
	t = "dir";
    অন্যথা যদি (S_ISLNK(st->st_mode))
	t = "link";
    অন্যথা যদি (S_ISBLK(st->st_mode))
	t = "bdev";
    অন্যথা যদি (S_ISCHR(st->st_mode))
	t = "cdev";
    অন্যথা যদি (S_ISFIFO(st->st_mode))
	t = "fifo";
    অন্যথা যদি (S_ISSOCK(st->st_mode))
	t = "socket";
    অন্যথা
	t = "other";
    ফেরত (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    শূন্য
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = ফাঁকা;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    যদি (mch_lstat((অক্ষর *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    শূন্য
f_glob(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	ফেরত;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	যদি (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    যদি (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, ফাঁকা);
	    যদি (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    যদি (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	যদি (p_wic)
	    options += WILD_ICASE;
	যদি (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     ফাঁকা, options, WILD_ALL);
	অন্যথা যদি (rettv_list_alloc(rettv) == OK)
	{
	  পূর্ণসংখ্যা i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						ফাঁকা, options, WILD_ALL_KEEP);
	  জন্য (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    অন্যথা
	rettv->vval.v_string = ফাঁকা;
}

/*
 * "glob2regpat()" function
 */
    শূন্য
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == ফাঁকা)
			 ? ফাঁকা : file_pat_to_reg_pat(pat, ফাঁকা, ফাঁকা, FALSE);
}

/*
 * "globpath()" function
 */
    শূন্য
f_globpath(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    পূর্ণসংখ্যা		error = FALSE;
    garray_T	ga;
    পূর্ণসংখ্যা		i;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	ফেরত;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	যদি (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	যদি (argvars[3].v_type != VAR_UNKNOWN)
	{
	    যদি (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, ফাঁকা);
	    যদি (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    যদি (file != ফাঁকা && !error)
    {
	ga_init2(&ga, আকার(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	যদি (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	অন্যথা যদি (rettv_list_alloc(rettv) == OK)
	    জন্য (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_তথ্য))[i], -1);
	ga_clear_strings(&ga);
    }
    অন্যথা
	rettv->vval.v_string = ফাঁকা;
}

/*
 * "isdirectory()" function
 */
    শূন্য
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    শূন্য
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
mkdir_recurse(char_u *dir, পূর্ণসংখ্যা prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    পূর্ণসংখ্যা		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    যদি (p <= get_past_শীর্ষ(dir))
	ফেরত OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    যদি (updir == ফাঁকা)
	ফেরত FAIL;
    যদি (mch_isdir(updir))
	r = OK;
    অন্যথা যদি (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	যদি (r == OK && created != ফাঁকা && *created == ফাঁকা)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    ফেরত r;
}

/*
 * "mkdir()" function
 */
    শূন্য
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    পূর্ণসংখ্যা		prot = 0755;
    পূর্ণসংখ্যা		defer = FALSE;
    পূর্ণসংখ্যা		defer_recurse = FALSE;
    char_u	*created = ফাঁকা;

    rettv->vval.v_সংখ্যা = FAIL;
    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ফেরত;

    dir = tv_get_string_buf(&argvars[0], buf);
    যদি (*dir == NUL)
	ফেরত;

    যদি (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[2], ফাঁকা);
	    যদি (prot == -1)
		ফেরত;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != ফাঁকা;
	defer_recurse = vim_strchr(arg2, 'R') != ফাঁকা;
	যদি ((defer || defer_recurse) && !can_add_defer())
	    ফেরত;

	যদি (vim_strchr(arg2, 'p') != ফাঁকা)
	{
	    যদি (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_সংখ্যা = OK;
		ফেরত;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : ফাঁকা);
	}
    }
    rettv->vval.v_সংখ্যা = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    যদি (rettv->vval.v_সংখ্যা == OK
				&& created == ফাঁকা && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    যদি (created != ফাঁকা)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	যদি (tv[0].vval.v_string == ফাঁকা || tv[1].vval.v_string == ফাঁকা
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
    শূন্য
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    পূর্ণসংখ্যা		trim_len = 1;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[1]);
	যদি (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    যদি (p == ফাঁকা)
	rettv->vval.v_string = ফাঁকা;
    অন্যথা
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	যদি (p != ফাঁকা)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    স্থির পূর্ণসংখ্যা
checkitem_common(শূন্য *context, char_u *name, dict_T *dict)
{
    typval_T	*রাশিমালা = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    পূর্ণসংখ্যা		retval = 0;
    পূর্ণসংখ্যা		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    যদি (name != ফাঁকা)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    অন্যথা
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    যদি (eval_রাশিমালা_typval(রাশিমালা, FALSE, argv, 1, ফাঁকা, &rettv) == FAIL)
	যাও theend;

    // We want to use -1, but also true/false should be allowed.
    যদি (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_সংখ্যা;
	rettv.vval.v_সংখ্যা = rettv.vval.v_সংখ্যা == VVAL_TRUE;
    }
    retval = tv_get_সংখ্যা_chk(&rettv, &error);
    যদি (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    যদি (name != ফাঁকা)
	set_vim_var_string(VV_VAL, ফাঁকা, 0);
    অন্যথা
	set_vim_var_dict(VV_VAL, ফাঁকা);
    restore_vimvar(VV_VAL, &save_val);
    ফেরত retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    স্থির পূর্ণসংখ্যা
readdir_checkitem(শূন্য *context, শূন্য *item)
{
    char_u	*name = (char_u *)item;

    ফেরত checkitem_common(context, name, ফাঁকা);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    স্থির পূর্ণসংখ্যা
readdirex_dict_arg(typval_T *argvars, পূর্ণসংখ্যা *cmp)
{
    char_u     *compare;

    যদি (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	ফেরত FAIL;

    যদি (dict_has_চাবি(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    অন্যথা
    {
	semsg(_(e_dictionary_চাবি_str_required), "sort");
	ফেরত FAIL;
    }

    যদি (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    অন্যথা যদি (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    অন্যথা যদি (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    অন্যথা যদি (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    ফেরত OK;
}

/*
 * "readdir()" function
 */
    শূন্য
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*রাশিমালা;
    পূর্ণসংখ্যা		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		sort = READDIR_SORT_BYTE;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;
    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ফেরত;

    path = tv_get_string(&argvars[0]);
    রাশিমালা = &argvars[1];

    যদি (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ফেরত;

    ret = readdir_core(&ga, path, FALSE, (শূন্য *)রাশিমালা,
	    (রাশিমালা->v_type == VAR_UNKNOWN) ? ফাঁকা : readdir_checkitem, sort);
    যদি (ret == OK)
    {
	জন্য (i = 0; i < ga.ga_len; i++)
	{
	    p = ((char_u **)ga.ga_তথ্য)[i];
	    list_append_string(rettv->vval.v_list, p, -1);
	}
    }
    ga_clear_strings(&ga);
}

/*
 * Evaluate "expr" (= "context") for readdirex().
 */
    স্থির পূর্ণসংখ্যা
readdirex_checkitem(শূন্য *context, শূন্য *item)
{
    dict_T	*dict = (dict_T*)item;

    ফেরত checkitem_common(context, ফাঁকা, dict);
}

/*
 * "readdirex()" function
 */
    শূন্য
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*রাশিমালা;
    পূর্ণসংখ্যা		ret;
    char_u	*path;
    garray_T	ga;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		sort = READDIR_SORT_BYTE;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;
    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ফেরত;

    path = tv_get_string(&argvars[0]);
    রাশিমালা = &argvars[1];

    যদি (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ফেরত;

    ret = readdir_core(&ga, path, TRUE, (শূন্য *)রাশিমালা,
	    (রাশিমালা->v_type == VAR_UNKNOWN) ? ফাঁকা : readdirex_checkitem, sort);
    যদি (ret == OK)
    {
	জন্য (i = 0; i < ga.ga_len; i++)
	{
	    dict_T  *dict = ((dict_T**)ga.ga_তথ্য)[i];
	    list_append_dict(rettv->vval.v_list, dict);
	    dict_unref(dict);
	}
    }
    ga_clear(&ga);
}

/*
 * "readfile()" function
 */
    স্থির শূন্য
read_file_or_blob(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা always_blob)
{
    পূর্ণসংখ্যা		দ্বিমিক = FALSE;
    পূর্ণসংখ্যা		blob = always_blob;
    পূর্ণসংখ্যা		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    পূর্ণসংখ্যা		io_আকার = আকার(buf);
    পূর্ণসংখ্যা		readlen;		// size of last fread()
    char_u	*পূর্ব	 = ফাঁকা;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	আকার_2 = -1;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	যদি (always_blob)
	{
	    offset = (off_T)tv_get_সংখ্যা(&argvars[1]);
	    যদি (argvars[2].v_type != VAR_UNKNOWN)
		আকার_2 = (off_T)tv_get_সংখ্যা(&argvars[2]);
	}
	অন্যথা
	{
	    যদি (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		দ্বিমিক = TRUE;
	    যদি (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    যদি (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_সংখ্যা(&argvars[2]);
	}
    }

    যদি ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	ফেরত;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    যদি (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ফেরত;
    }
    যদি (*fname == NUL || (fd = mch_fopen((অক্ষর *)fname, READBIN)) == ফাঁকা)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ফেরত;
    }

    যদি (blob)
    {
	যদি (read_blob(fd, rettv, offset, আকার_2) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	ফেরত;
    }

    যখন (cnt < maxline || maxline < 0)
    {
	readlen = (পূর্ণসংখ্যা)fread(buf, 1, io_আকার, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	জন্য (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || দ্বিমিক));
		++p)
	{
	    যদি (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= ফাঁকা;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		যদি (readlen > 0 && !দ্বিমিক)
		{
		    যখন (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    যদি (len == 0)
			যখন (prevlen > 0 && পূর্ব[prevlen - 1] == '\r')
			    --prevlen;
		}
		যদি (prevlen == 0)
		    s = vim_strnsave(start, len);
		অন্যথা
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    যদি ((s = vim_realloc(পূর্ব, prevlen + len + 1)) != ফাঁকা)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			পূর্ব = ফাঁকা; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		যদি (s == ফাঁকা)
		{
		    do_outofmem_বার্তা((long_u) prevlen + len + 1);
		    failed = TRUE;
		    বিরতি;
		}

		যদি ((li = listitem_alloc()) == ফাঁকা)
		{
		    vim_free(s);
		    failed = TRUE;
		    বিরতি;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		যদি ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    বিরতি;
	    }
	    অন্যথা যদি (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    অন্যথা যদি (*p == 0xbf && enc_utf8 && !দ্বিমিক)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? পূর্ব[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? পূর্ব[prevlen - 1]
			  : prevlen >= 2 ? পূর্ব[prevlen - 2] : NUL;

		যদি (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    যদি (start == dest)
			start = p + 1;
		    অন্যথা
		    {
			// have to shuffle buf to close gap
			পূর্ণসংখ্যা adjust_prevlen = 0;

			যদি (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (পূর্ণসংখ্যা)(buf - dest);
			    dest = buf;
			}
			যদি (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	যদি (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    বিরতি;
	যদি (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    যদি (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		যদি (prevsize == 0)
		    prevsize = (long)(p - start);
		অন্যথা
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(পূর্ব, prevsize);
		যদি (newprev == ফাঁকা)
		{
		    do_outofmem_বার্তা((long_u)prevsize);
		    failed = TRUE;
		    বিরতি;
		}
		পূর্ব = newprev;
	    }
	    // Add the line part to end of "prev".
	    mch_memmove(পূর্ব + prevlen, start, p - start);
	    prevlen += (long)(p - start);
	}
    } // while

    // For a negative line count use only the lines at the end of the file,
    // free the rest.
    যদি (!failed && maxline < 0)
	যখন (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    যদি (failed)
    {
	// an empty list is returned on error
	list_free(rettv->vval.v_list);
	rettv_list_alloc(rettv);
    }

    vim_free(পূর্ব);
    fclose(fd);
}

/*
 * "readblob()" function
 */
    শূন্য
f_readblob(typval_T *argvars, typval_T *rettv)
{
    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ফেরত;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    শূন্য
f_readfile(typval_T *argvars, typval_T *rettv)
{
    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL)))
	ফেরত;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    শূন্য
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = ফাঁকা;
# endif

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = ফাঁকা;

	v = mch_resolve_path(p, TRUE);
	যদি (v != ফাঁকা)
	    rettv->vval.v_string = v;
	অন্যথা
	    rettv->vval.v_string = vim_strsave(p);
    }
# অন্যথা
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	পূর্ণসংখ্যা	len;
	char_u	*remain = ফাঁকা;
	char_u	*q;
	পূর্ণসংখ্যা	is_relative_to_current = FALSE;
	পূর্ণসংখ্যা	has_trailing_pathsep = FALSE;
	পূর্ণসংখ্যা	limit = 100;

	p = vim_strsave(p);
	যদি (p == ফাঁকা)
	    যাও fail;
	যদি (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	যদি (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	যদি (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	যদি (buf == ফাঁকা)
	{
	    vim_free(p);
	    vim_free(remain);
	    যাও fail;
	}

	জন্য (;;)
	{
	    জন্য (;;)
	    {
		len = readlink((অক্ষর *)p, (অক্ষর *)buf, MAXPATHL);
		যদি (len <= 0)
		    বিরতি;
		buf[len] = NUL;

		যদি (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = ফাঁকা;
		    যাও fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		যদি (remain == ফাঁকা && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		যদি (*q != NUL)
		{
		    যদি (remain == ফাঁকা)
			remain = vim_strsave(q - 1);
		    অন্যথা
		    {
			cpy = concat_str(q - 1, remain);
			যদি (cpy != ফাঁকা)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		যদি (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		যদি (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    যদি (cpy != ফাঁকা)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		অন্যথা
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    যদি (remain == ফাঁকা)
		বিরতি;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    যদি (cpy != ফাঁকা)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    যদি (*q != NUL)
		STRMOVE(remain, q - 1);
	    অন্যথা
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	যদি (!vim_ispathsep(*p))
	{
	    যদি (is_relative_to_current
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
		যদি (cpy != ফাঁকা)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    অন্যথা যদি (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		যখন (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		যদি (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	যদি (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    যদি (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  অন্যথা
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
    শূন্য
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    স্থির পূর্ণসংখ্যা	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    করো
    {
	যদি (x == 'Z')
	    x = '0';
	অন্যথা যদি (x == '9')
	    x = 'A';
	অন্যথা
	    ++x;
    } যখন (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    শূন্য
f_writefile(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		দ্বিমিক = FALSE;
    পূর্ণসংখ্যা		append = FALSE;
    পূর্ণসংখ্যা		defer = FALSE;
# ifdef HAVE_FSYNC
    পূর্ণসংখ্যা		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    পূর্ণসংখ্যা		ret = 0;
    listitem_T	*li;
    list_T	*list = ফাঁকা;
    blob_T	*blob = ফাঁকা;

    rettv->vval.v_সংখ্যা = -1;
    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ফেরত;

    যদি (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	যদি (list == ফাঁকা)
	    ফেরত;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_বস্তুসমূহ(list, li)
	    যদি (tv_get_string_chk(&li->li_tv) == ফাঁকা)
		ফেরত;
    }
    অন্যথা যদি (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	যদি (blob == ফাঁকা)
	    ফেরত;
    }
    অন্যথা
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	ফেরত;
    }

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	যদি (arg2 == ফাঁকা)
	    ফেরত;
	যদি (vim_strchr(arg2, 'b') != ফাঁকা)
	    দ্বিমিক = TRUE;
	যদি (vim_strchr(arg2, 'a') != ফাঁকা)
	    append = TRUE;
	যদি (vim_strchr(arg2, 'D') != ফাঁকা)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	যদি (vim_strchr(arg2, 's') != ফাঁকা)
	    do_fsync = TRUE;
	অন্যথা যদি (vim_strchr(arg2, 'S') != ফাঁকা)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    যদি (fname == ফাঁকা)
	ফেরত;

    যদি (defer && !can_add_defer())
	ফেরত;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    যদি (*fname == NUL || (fd = mch_fopen((অক্ষর *)fname,
				      append ? APPENDBIN : WRITEBIN)) == ফাঁকা)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    অন্যথা
    {
	যদি (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    যদি (tv.vval.v_string == ফাঁকা
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(শূন্য)mch_remove(fname);
	    }
	}

	যদি (ret == 0)
	{
	    যদি (blob)
	    {
		যদি (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    অন্যথা
	    {
		যদি (write_list(fd, list, দ্বিমিক) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    যদি (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_সংখ্যা = ret;
}

#endif // FEAT_EVAL

#যদি defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    পূর্ণসংখ্যা		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    স্থির char_u	*last_dir = ফাঁকা;    // last used directory
    char_u		*tofree = ফাঁকা;
    পূর্ণসংখ্যা			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    যদি (title == ফাঁকা || *title == NUL)
    {
	যদি (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	অন্যথা যদি (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	অন্যথা
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    যদি ((initdir == ফাঁকা || *initdir == NUL) && dflt != ফাঁকা && *dflt != NUL)
    {
	যদি (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = ফাঁকা;
	}
	অন্যথা যদি (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    যদি (tofree != ফাঁকা)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    যদি (initdir == ফাঁকা || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	যদি (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	অন্যথা যদি (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != ফাঁকা && buf->b_ffname != ফাঁকা)
	{
	    যদি (dflt == ফাঁকা || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    যদি (tofree != ফাঁকা)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	অন্যথা যদি (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    যদি (gui.in_use)		// when this changes, also adjust f_has()!
    {
	যদি (filter == ফাঁকা
#  ifdef FEAT_EVAL
		&& (filter = get_var_মান((char_u *)"b:browsefilter")) == ফাঁকা
		&& (filter = get_var_মান((char_u *)"g:browsefilter")) == ফাঁকা
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	যদি (flags & BROWSE_DIR)
	{
#  যদি defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  অন্যথা
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  যদি !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    যদি (fname != ফাঁকা && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *লেজ = gettail_sep(fname);

		যদি (লেজ == fname)
		    *লেজ++ = '.';	// use current dir
		*লেজ = NUL;
	    }
#  endif
	}
	অন্যথা
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    অন্যথা
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = ফাঁকা;
    }

    // keep the directory for next time
    যদি (fname != ফাঁকা)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	যদি (last_dir != ফাঁকা && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    যদি (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		যদি (last_dir != ফাঁকা)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    ফেরত fname;
}
#endif

#যদি defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    শূন্য
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    পূর্ণসংখ্যা		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    পূর্ণসংখ্যা		error = FALSE;

    যদি (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ফেরত;

    save = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    যদি (error || title == ফাঁকা || initdir == ফাঁকা || defname == ফাঁকা)
	rettv->vval.v_string = ফাঁকা;
    অন্যথা
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, ফাঁকা, initdir, ফাঁকা, curbuf);
# অন্যথা
    rettv->vval.v_string = ফাঁকা;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    শূন্য
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    যদি (title == ফাঁকা || initdir == ফাঁকা)
	rettv->vval.v_string = ফাঁকা;
    অন্যথা
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, ফাঁকা, ফাঁকা, initdir, ফাঁকা, curbuf);
# অন্যথা
    rettv->vval.v_string = ফাঁকা;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    শূন্য
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_সংখ্যা = FALSE;

    যদি (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	ফেরত;

    from = tv_get_string(&argvars[0]);

    যদি (mch_lstat((অক্ষর *)from, &st) >= 0
	&& (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)))
	rettv->vval.v_সংখ্যা = vim_copyfile(
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
    পূর্ণসংখ্যা		dstlen,	// maximum length of the result
    পূর্ণসংখ্যা		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    যদি (src == ফাঁকা)
    {
	*dst = NUL;
	ফেরত 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    যদি (buf != ফাঁকা && buf->b_help)
	ফেরত vim_snprintf_safelen((অক্ষর *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    যদি (homedir != ফাঁকা)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#অন্যথা
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    যদি (homedir_env == ফাঁকা)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    যদি (homedir_env != ফাঁকা && *homedir_env == NUL)
	homedir_env = ফাঁকা;

    যদি (homedir_env != ফাঁকা && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = ফাঁকা;

	flen = STRLEN(homedir_env);
	(শূন্য)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	যদি (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    যদি (homedir_env != ফাঁকা)
	envlen = STRLEN(homedir_env);

    যদি (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    যখন (*src && dstlen > 0)
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
	জন্য (;;)
	{
	    যদি (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		যদি (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		বিরতি;
	    }
	    যদি (p == homedir_env)
		বিরতি;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	যখন (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	যখন ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    যদি (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    ফেরত (size_t)(dst - dst_start);
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
    যদি (src != ফাঁকা)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    যদি (dst != ফাঁকা)
	home_replace(buf, src, dst, len, TRUE);
    ফেরত dst;
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
    পূর্ণসংখ্যা
fullpathcmp(
    char_u *s1,
    char_u *s2,
    পূর্ণসংখ্যা	    checkname,		// when both don't exist, check file names
    পূর্ণসংখ্যা	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    পূর্ণসংখ্যা		    r1, r2;

    যদি (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    অন্যথা
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((অক্ষর *)exp1, &st1);
    r2 = mch_stat((অক্ষর *)s2, &st2);
    যদি (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	যদি (checkname)
	{
	    যদি (fnamecmp(exp1, s2) == 0)
		ফেরত FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    যদি (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		ফেরত FPC_SAMEX;
	}
	ফেরত FPC_NOTX;
    }
    যদি (r1 != 0 || r2 != 0)
	ফেরত FPC_DIFFX;
    যদি (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	ফেরত FPC_SAME;
    ফেরত FPC_DIFF;
#অন্যথা
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    পূর্ণসংখ্যা	    retval = FPC_DIFF;
    পূর্ণসংখ্যা	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    যদি ((exp1 = alloc(MAXPATHL * 3)) != ফাঁকা)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	যদি (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	অন্যথা
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	যদি (r1 != OK && r2 != OK)
	{
	    যদি (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    অন্যথা
		retval = FPC_NOTX;
	}
	অন্যথা যদি (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	অন্যথা যদি (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	অন্যথা
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    ফেরত retval;
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

    যদি (fname == ফাঁকা)
	ফেরত (char_u *)"";
    জন্য (p1 = p2 = get_past_শীর্ষ(fname); *p2; )	// find last part of path
    {
	যদি (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    ফেরত p1;
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

    p = get_past_শীর্ষ(fname);	// don't remove the '/' from "c:/file"
    t = gettail(fname);
    যখন (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    ফেরত t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    যখন (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    যদি (*fname)
	++fname;
    ফেরত fname;
}

/*
 * Get a pointer to one character past the head of a path name.
 * Unix: after "/"; DOS: after "c:\"; Amiga: after "disk:/"; Mac: no head.
 * If there is no head, path is returned.
 */
    char_u *
get_past_শীর্ষ(char_u *path)
{
    char_u  *retval;

#যদি defined(MSWIN)
    // may skip "c:"
    যদি (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    অন্যথা
	retval = path;
#অন্যথা
# যদি defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    যদি (retval == ফাঁকা)
	retval = path;
# অন্যথা	// Unix
    retval = path;
# endif
#endif

    যখন (vim_ispathsep(*retval))
	++retval;

    ফেরত retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    পূর্ণসংখ্যা
vim_ispathsep(পূর্ণসংখ্যা c)
{
#ifdef UNIX
    ফেরত (c == '/');	    // UNIX has ':' inside file names
#অন্যথা
# ifdef BACKSLASH_IN_FILENAME
    ফেরত (c == ':' || c == '/' || c == '\\');
# অন্যথা
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    ফেরত (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  অন্যথা
    ফেরত (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    পূর্ণসংখ্যা
vim_ispathsep_nocolon(পূর্ণসংখ্যা c)
{
    ফেরত vim_ispathsep(c)
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
    পূর্ণসংখ্যা
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		retval;

    p = gettail_sep(fname);
    যদি (p == fname)
	ফেরত TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    ফেরত retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    পূর্ণসংখ্যা
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    ফেরত vim_fnamencmp(x, y, MAXPATHL);
#অন্যথা
    যদি (p_fic)
	ফেরত MB_STRICMP(x, y);
    ফেরত STRCMP(x, y);
#endif
}

    পূর্ণসংখ্যা
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    পূর্ণসংখ্যা		cx = NUL;
    পূর্ণসংখ্যা		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    জন্য (পূর্ণসংখ্যা swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	যদি ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    যদি (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		বিরতি;
	    }
	    অন্যথা // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    যখন (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	যদি (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    বিরতি;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    যদি (len == 0)
	ফেরত 0;
    ফেরত (cx - cy);
#অন্যথা
    যদি (p_fic)
	ফেরত MB_STRNICMP(x, y, len);
    ফেরত STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, পূর্ণসংখ্যা sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? আকার(PATHSEP) : 0) + fname2len + 1);
    যদি (ret->string == ফাঁকা)
	ret->দৈর্ঘ্য = 0;
    অন্যথা
    {
	STRCPY(ret->string, fname1);
	ret->দৈর্ঘ্য = fname1len;
	যদি (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->দৈর্ঘ্য))
	{
	    STRCPY(ret->string + ret->দৈর্ঘ্য, PATHSEPSTR);
	    ret->দৈর্ঘ্য += আকার(PATHSEP);
	}
	STRCPY(ret->string + ret->দৈর্ঘ্য, fname2);
	ret->দৈর্ঘ্য += fname2len;
    }

    ফেরত ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    শূন্য
add_pathsep(char_u *p)
{
    size_t  plen;

    যদি (*p == NUL)
	ফেরত;

    plen = STRLEN(p);
    যদি (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    পূর্ণসংখ্যা		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = ফাঁকা;

    যদি (fname == ফাঁকা)
	ফেরত ফাঁকা;

    buf = alloc(MAXPATHL);
    যদি (buf == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    অন্যথা
	new_fname = vim_strsave(fname);
    vim_free(buf);
    ফেরত new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    পূর্ণসংখ্যা
vim_fexists(char_u *fname)
{
    stat_T st;

    যদি (mch_stat((অক্ষর *)fname, &st))
	ফেরত FALSE;
    ফেরত TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    পূর্ণসংখ্যা		  *অঙ্ক_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    পূর্ণসংখ্যা		   flags)	// EW_DIR, etc.
{
    পূর্ণসংখ্যা		ret = FAIL;
    char_u	*eval_pat = ফাঁকা;
    char_u	*exp_pat = *pat;
    অক্ষর	*ignored_বার্তা;
    size_t	usedlen;
    পূর্ণসংখ্যা		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    পূর্ণসংখ্যা		star_follows = FALSE;

    যদি (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       ফাঁকা, &ignored_বার্তা, ফাঁকা, TRUE);
	--emsg_off;
	যদি (eval_pat != ফাঁকা)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    যদি (exp_pat != ফাঁকা)
	ret = expand_wildcards(1, &exp_pat, অঙ্ক_file, file, flags);

    যদি (eval_pat != ফাঁকা)
    {
	যদি (*অঙ্ক_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    যদি (*file != ফাঁকা)
	    {
		**file = eval_pat;
		eval_pat = ফাঁকা;
		*অঙ্ক_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    ফেরত ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    পূর্ণসংখ্যা
expand_wildcards(
    পূর্ণসংখ্যা		   অঙ্ক_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    পূর্ণসংখ্যা		  *অঙ্ক_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    পূর্ণসংখ্যা		   flags)	// EW_DIR, etc.
{
    পূর্ণসংখ্যা		retval;
    পূর্ণসংখ্যা		i, j;
    char_u	*p;
    পূর্ণসংখ্যা		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(অঙ্ক_pat, pat, অঙ্ক_files, files, flags);

    // When keeping all matches, return here
    যদি ((flags & EW_KEEPALL) || retval == FAIL)
	ফেরত retval;

    /*
     * Remove names that match 'wildignore'.
     */
    যদি (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	জন্য (i = 0; i < *অঙ্ক_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    যদি (ffname == ফাঁকা)		// out of memory
		বিরতি;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    যদি (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		জন্য (j = i; j + 1 < *অঙ্ক_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*অঙ্ক_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	যদি (*অঙ্ক_files == 0)
	{
	    VIM_CLEAR(*files);
	    ফেরত FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    যদি (*অঙ্ক_files > 1 && !got_int)
    {
	non_suf_match = 0;
	জন্য (i = 0; i < *অঙ্ক_files; ++i)
	{
	    যদি (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		জন্য (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    ফেরত retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    পূর্ণসংখ্যা
match_suffix(char_u *fname)
{
    পূর্ণসংখ্যা		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (পূর্ণসংখ্যা)STRLEN(fname);
    setsuflen = 0;
    জন্য (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	যদি (setsuflen == 0)
	{
	    char_u *লেজ = gettail(fname);

	    // empty entry: match name without a '.'
	    যদি (vim_strchr(লেজ, '.') == ফাঁকা)
	    {
		setsuflen = 1;
		বিরতি;
	    }
	}
	অন্যথা
	{
	    যদি (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		বিরতি;
	    setsuflen = 0;
	}
    }
    ফেরত (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    স্থির পূর্ণসংখ্যা
vim_backtick(char_u *p)
{
    ফেরত (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    স্থির পূর্ণসংখ্যা
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    পূর্ণসংখ্যা		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*বাফার;
    পূর্ণসংখ্যা		cnt = 0;
    পূর্ণসংখ্যা		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    যদি (cmd == ফাঁকা)
	ফেরত -1;

#ifdef FEAT_EVAL
    যদি (*cmd == '=')	    // `={expr}`: Expand expression
	বাফার = eval_to_string(cmd + 1, TRUE, FALSE);
    অন্যথা
#endif
	বাফার = get_cmd_output(cmd, ফাঁকা,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, ফাঁকা);
    vim_free(cmd);
    যদি (বাফার == ফাঁকা)
	ফেরত -1;

    cmd = বাফার;
    যখন (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	যখন (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	যদি (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	যখন (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(বাফার);
    ফেরত cnt;
}

#যদি defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    স্থির পূর্ণসংখ্যা
pstrcmp(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b)
{
    ফেরত (pathcmp(*(অক্ষর **)a, *(অক্ষর **)b, -1));
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
    পূর্ণসংখ্যা
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    পূর্ণসংখ্যা		flags,		// EW_* flags
    পূর্ণসংখ্যা		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    পূর্ণসংখ্যা		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    পূর্ণসংখ্যা		starts_with_dot;
    পূর্ণসংখ্যা		matches;
    size_t	buflen;
    size_t	len;
    পূর্ণসংখ্যা		starstar = FALSE;
    স্থির পূর্ণসংখ্যা	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_মান;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = ফাঁকা;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    পূর্ণসংখ্যা			ok;
    char_u		*p_alt;
    পূর্ণসংখ্যা			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    যদি (stardepth > 0)
    {
	ui_breakcheck();
	যদি (got_int)
	    ফেরত 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    যদি (buf == ফাঁকা)
	ফেরত 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ফাঁকা;
    path_end = path;
    যখন (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	যদি (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	অন্যথা যদি (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    যদি (e != ফাঁকা)
		বিরতি;
	    s = p + 1;
	}
	অন্যথা যদি (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != ফাঁকা)
	    e = p;
	যদি (has_mbyte)
	{
	    পূর্ণসংখ্যা charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	অন্যথা
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    জন্য (p = buf + wildoff; p < s; ++p)
	যদি (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    জন্য (p = s; p < e; ++p)
	যদি (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ফাঁকা, FALSE);
    যদি (pat == ফাঁকা)
    {
	vim_free(buf);
	ফেরত 0;
    }

    // compile the regexp into a program
    যদি (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    যদি (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    যদি (regmatch.regprog == ফাঁকা && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ফেরত 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != ফাঁকা;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    যদি (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((অক্ষর *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(শূন্য)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((অক্ষর *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, ফাঁকা);
    যদি (wn != ফাঁকা)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_মান);

    যখন (ok)
    {
	p = utf16_to_enc(wfb.cFileName, ফাঁকা);   // p is allocated here

	যদি (p == ফাঁকা)
	    বিরতি;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	যদি (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = ফাঁকা;
	অন্যথা
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, ফাঁকা);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	যদি ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == ফাঁকা
		  || (regmatch.regprog != ফাঁকা
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != ফাঁকা
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((অক্ষর *)s, buflen - len, "%s", p);
	    যদি (len + 1 < buflen)
	    {
		যদি (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((অক্ষর *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (শূন্য)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((অক্ষর *)buf + len, buflen - len, "%s", path_end);
		যদি (mch_has_exp_wildcard(path_end))
		{
		    যদি (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(শূন্য)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		অন্যথা
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    যদি (*path_end != 0)
			backslash_halve(buf + len + 1);
		    // addfile() checks existence; do not stat the same path twice.
		    // Even with EW_NOTFOUND, a literal suffix must exist.
		    addfile(gap, buf, flags & ~EW_NOTFOUND);
		}
	    }
	}

	vim_free(p_alt);
	vim_free(p);
	ok = Find_পরবর্তী_File_W(hFind, &wfb);
    }

    FindClose(hFind);
    vim_free(wn);
    vim_free(buf);
    vim_regfree(regmatch.regprog);
    vim_free(matchname);

    matches = gap->ga_len - start_len;
    যদি (matches > 0)
	qsort(((char_u **)gap->ga_তথ্য) + start_len, (size_t)matches,
						   আকার(char_u *), pstrcmp);
    ফেরত matches;
}
#endif // MSWIN

#যদি (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    স্থির পূর্ণসংখ্যা
pstrcmp(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b)
{
    ফেরত (pathcmp(*(অক্ষর **)a, *(অক্ষর **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    পূর্ণসংখ্যা
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    পূর্ণসংখ্যা		flags,		// EW_* flags
    পূর্ণসংখ্যা		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    পূর্ণসংখ্যা		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    পূর্ণসংখ্যা		starts_with_dot;
    পূর্ণসংখ্যা		matches;
    size_t	buflen;
    size_t	len;
    পূর্ণসংখ্যা		starstar = FALSE;
    স্থির পূর্ণসংখ্যা	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    কাঠামো dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    যদি (stardepth > 0)
    {
	ui_breakcheck();
	যদি (got_int)
	    ফেরত 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    যদি (buf == ফাঁকা)
	ফেরত 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ফাঁকা;
    path_end = path;
    যখন (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	যদি (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	অন্যথা যদি (*path_end == '/')
	{
	    যদি (e != ফাঁকা)
		বিরতি;
	    s = p + 1;
	}
	অন্যথা যদি (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != ফাঁকা
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	যদি (has_mbyte)
	{
	    পূর্ণসংখ্যা charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	অন্যথা
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    জন্য (p = buf + wildoff; p < s; ++p)
	যদি (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    জন্য (p = s; p < e; ++p)
	যদি (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ফাঁকা, FALSE);
    যদি (pat == ফাঁকা)
    {
	vim_free(buf);
	ফেরত 0;
    }

    // compile the regexp into a program
    যদি (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    অন্যথা
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    যদি (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    যদি (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    যদি (regmatch.regprog == ফাঁকা && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ফেরত 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    যদি (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((অক্ষর *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(শূন্য)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (অক্ষর *)buf);

    // Find all matching entries
    যদি (dirp != ফাঁকা)
    {
	যখন (!got_int)
	{
	    dp = readdir(dirp);
	    যদি (dp == ফাঁকা)
		বিরতি;
	    len = (size_t)(s - buf);
	    যদি ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != ফাঁকা && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((অক্ষর *)s, buflen - len, "%s", dp->d_name);
		যদি (len + 1 >= buflen)
		    চালু;

		যদি (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((অক্ষর *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (শূন্য)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((অক্ষর *)buf + len, buflen - len, "%s", path_end);
		যদি (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    যদি (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(শূন্য)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		অন্যথা
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    যদি (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    যদি ((flags & EW_ALLLINKS) ? mch_lstat((অক্ষর *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			যদি (precomp_buf)
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
    যদি (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_তথ্য) + start_len, matches,
						   আকার(char_u *), pstrcmp);
    ফেরত matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    স্থির পূর্ণসংখ্যা
has_env_var(char_u *p)
{
    জন্য ( ; *p; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' && p[1] != NUL)
	    ++p;
	অন্যথা যদি (vim_strchr((char_u *)
#যদি defined(MSWIN)
				    "$%"
#অন্যথা
				    "$"
#endif
					, *p) != ফাঁকা)
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    স্থির পূর্ণসংখ্যা
has_special_wildchar(char_u *p)
{
    জন্য ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	যদি (*p == '\r' || *p == '\n')
	    বিরতি;
	// Allow for escaping.
	যদি (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	অন্যথা যদি (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != ফাঁকা)
	{
	    // A { must be followed by a matching }.
	    যদি (*p == '{' && vim_strchr(p, '}') == ফাঁকা)
		চালু;
	    // A quote and backtick must be followed by another one.
	    যদি ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == ফাঁকা)
		চালু;
	    ফেরত TRUE;
	}
    }
    ফেরত FALSE;
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
    পূর্ণসংখ্যা
gen_expand_wildcards(
    পূর্ণসংখ্যা		অঙ্ক_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    পূর্ণসংখ্যা		*অঙ্ক_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    পূর্ণসংখ্যা		flags)		// EW_* flags
{
    পূর্ণসংখ্যা			i;
    garray_T		ga;
    char_u		*p;
    স্থির পূর্ণসংখ্যা		recursive = FALSE;
    পূর্ণসংখ্যা			add_pat;
    পূর্ণসংখ্যা			retval = OK;
    পূর্ণসংখ্যা			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    যদি (recursive)
#ifdef SPECIAL_WILDCHAR
	ফেরত mch_expand_wildcards(অঙ্ক_pat, pat, অঙ্ক_file, file, flags);
#অন্যথা
	ফেরত FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    জন্য (i = 0; i < অঙ্ক_pat; i++)
    {
	যদি (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    ফেরত mch_expand_wildcards(অঙ্ক_pat, pat, অঙ্ক_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, আকার(char_u *), 30);

    জন্য (i = 0; i < অঙ্ক_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	যদি (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    যদি (add_pat == -1)
		retval = FAIL;
	}
	অন্যথা
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    যদি ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		যদি (p == ফাঁকা)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		অন্যথা যদি (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(অঙ্ক_pat, pat, অঙ্ক_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    ফেরত i;
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
	    যদি (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		যদি ((flags & (EW_PATH | EW_CDPATH))
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
		অন্যথা
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	যদি (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    যদি (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    অন্যথা
		addfile(&ga, t, flags);

	    যদি (t != p)
		vim_free(t);
	}

	যদি (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	যদি (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    যদি (retval == FAIL)
	ga_clear_strings(&ga);

    *অঙ্ক_file = ga.ga_len;
    *file = (ga.ga_তথ্য != ফাঁকা) ? (char_u **)ga.ga_তথ্য
						  : (char_u **)_("no matches");

    recursive = FALSE;

    ফেরত ((flags & EW_EMPTYOK) || ga.ga_তথ্য != ফাঁকা) ? retval : FAIL;
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
    শূন্য
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    পূর্ণসংখ্যা		flags)
{
    char_u	*p;
    পূর্ণসংখ্যা		isdir;
    stat_T	sb;

#ifdef MSWIN
    যদি (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	যদি (perm < 0)
	    ফেরত;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    অন্যথা
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	যদি (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((অক্ষর *)f, &sb) < 0 : mch_getperm(f) < 0))
	    ফেরত;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    যদি (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != ফাঁকা)
	ফেরত;
#endif

    যদি ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	ফেরত;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    যদি (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, ফাঁকা, !(flags & EW_SHELLCMD)))
	ফেরত;

    // Make room for another item in the file list.
    যদি (ga_grow(gap, 1) == FAIL)
	ফেরত;

    p = alloc(STRLEN(f) + 1 + isdir);
    যদি (p == ফাঁকা)
	ফেরত;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    যদি (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_তথ্য)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    শূন্য
FreeWild(পূর্ণসংখ্যা গুনতি, char_u **files)
{
    যদি (গুনতি <= 0 || files == ফাঁকা)
	ফেরত;
    যখন (গুনতি--)
	vim_free(files[গুনতি]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    পূর্ণসংখ্যা
pathcmp(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *q, পূর্ণসংখ্যা maxlen)
{
    পূর্ণসংখ্যা		i, j;
    পূর্ণসংখ্যা		c1, c2;
    ধ্রুবক অক্ষর	*s = ফাঁকা;

    জন্য (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	যদি (c1 == NUL)
	{
	    যদি (c2 == NUL)  // full match
		ফেরত 0;
	    s = q;
	    i = j;
	    বিরতি;
	}

	// End of "q": check if "p" just has a slash.
	যদি (c2 == NUL)
	{
	    s = p;
	    বিরতি;
	}

	যদি ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    যদি (vim_ispathsep(c1))
		ফেরত -1;
	    যদি (vim_ispathsep(c2))
		ফেরত 1;
	    ফেরত p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    যদি (s == ফাঁকা)	// "i" or "j" ran into "maxlen"
	ফেরত 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    যদি (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#অন্যথা
	    && c1 == '/'
#endif
       )
	ফেরত 0;   // match with trailing slash
    যদি (s == q)
	ফেরত -1;	    // no match
    ফেরত 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    পূর্ণসংখ্যা
vim_isAbsName(char_u *name)
{
    ফেরত (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    পূর্ণসংখ্যা		len,
    পূর্ণসংখ্যা		force)	    // force expansion even when already absolute
{
    পূর্ণসংখ্যা		retval = OK;
    পূর্ণসংখ্যা		url;

    *buf = NUL;
    যদি (fname == ফাঁকা)
	ফেরত FAIL;

    url = path_with_url(fname);
    যদি (!url)
	retval = mch_FullName(fname, buf, len, force);
    যদি (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#যদি defined(MSWIN)
    slash_adjust(buf);
#endif
    ফেরত retval;
}
