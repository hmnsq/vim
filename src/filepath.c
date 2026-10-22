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

#अन्तर्भाव "vim.h"

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
    स्थिर पूर्णांक
get_short_pathname(char_u **fnamep, char_u **bufp, माप_प्रकार *fnamelen)
{
    पूर्णांक		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = दो_स्मृति(len * आकार(*newbuf));
    जर (newbuf == रिक्त)
	परत FAIL;

    wfname = enc_to_utf16(*fnamep, रिक्त);
    जर (wfname == रिक्त)
    {
	vim_free(newbuf);
	परत FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    जर (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * आकार(*newbuf));
	जर (newbuf == रिक्त)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    परत FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    जर (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, रिक्त);

	जर (p != रिक्त)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	अन्यथा
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    परत FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    परत OK;
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
    स्थिर पूर्णांक
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    माप_प्रकार	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = रिक्त;
    char_u	*short_fname = रिक्त;
    char_u	*endp, *save_endp;
    char_u	ch;
    माप_प्रकार	old_len;
    माप_प्रकार	len;
    माप_प्रकार	new_len, sfx_len;
    पूर्णांक		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    जर (save_fname == रिक्त)
    {
	retval = FAIL;
	जा theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    साठी (;;)
    {
	// go back one path-separator
	पर्यंत (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	जर (endp <= save_fname)
	    थांबा;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	जर (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    जा theend;
	}
	*endp = ch;	// preserve the string

	जर (len > 0)
	    थांबा;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    जर (len > 0)
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
	जर (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    जर (*fname == रिक्त)
	    {
		retval = FAIL;
		जा theend;
	    }
	}
	अन्यथा
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    जर (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = रिक्त;
	}

	// concat the not-shortened part of the path
	जर ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    परत retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    स्थिर पूर्णांक
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    माप_प्रकार	*fnamelen)
{
    पूर्णांक		sepcount;
    माप_प्रकार	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    पूर्णांक		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    साठी (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	जर (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    जर (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    अन्यथा
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    जर (tfname == रिक्त)
	परत FAIL;

    len = tflen = STRLEN(tfname);

    जर (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	परत FAIL;

    जर (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	जर (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    परत FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    साठी (p = tfname + len - 1; p >= tfname; --p)
    {
	जर (has_mbyte)
	    p -= mb_head_off(tfname, p);
	जर (vim_ispathsep(*p))
	{
	    जर (sepcount == 0 || (hasTilde && sepcount == 1))
		थांबा;
	    अन्यथा
		sepcount --;
	}
    }
    जर (hasTilde)
    {
	--p;
	जर (p >= tfname)
	    *p = '~';
	अन्यथा
	    परत FAIL;
    }
    अन्यथा
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    परत OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    पूर्णांक
modify_fname(
    char_u	*src,		// string with modifiers
    पूर्णांक		tilde_file,	// "~" is a file name, not $HOME
    माप_प्रकार	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    माप_प्रकार	*fnamelen)	// length of fnamep
{
    पूर्णांक		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	स_नाम[MAXPATHL];
    पूर्णांक		c;
    पूर्णांक		has_fullname = 0;
    पूर्णांक		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    पूर्णांक		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    जर (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	जर ((*fnamep)[0] == '~'
#जर !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    जर (*fnamep == रिक्त)
		परत -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	साठी (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    जर (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		थांबा;
	}

	// FullName_save() is slow, don't use it when not needed.
	जर (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    जर (*fnamep == रिक्त)
		परत -1;
	}

#ifdef MSWIN
# जर _WIN32_WINNT >= 0x0500
	जर (vim_strchr(*fnamep, '~') != रिक्त)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, रिक्त);
	    WCHAR buf[_MAX_PATH];

	    जर (wfname != रिक्त)
	    {
		जर (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, रिक्त);

		    जर (q != रिक्त)
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
	जर (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    जर (*fnamep == रिक्त)
		परत -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    पर्यंत (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	जर (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    सुरूठेवा;
	}
	pbuf = रिक्त;
	// Need full path first (use expand_env() to remove a "~/")
	जर (!has_fullname && !has_homerelative)
	{
	    जर (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    अन्यथा
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	अन्यथा
	    p = *fnamep;

	has_fullname = 0;

	जर (p != रिक्त)
	{
	    माप_प्रकार  dirnamelen = 0;

	    जर (c == '.')
	    {
		mch_dirname(स_नाम, MAXPATHL);
		जर (has_homerelative)
		{
		    s = vim_strsave(स_नाम);
		    जर (s != रिक्त)
		    {
			dirnamelen = home_replace(रिक्त, s, स_नाम, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		जर (dirnamelen == 0)
		    dirnamelen = STRLEN(स_नाम);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		जर (fnamencmp(p, स_नाम, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    जर (vim_ispathsep(*p))
		    {
			पर्यंत (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			जर (pbuf != रिक्त)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = रिक्त;
			}
		    }
		}
	    }
	    अन्यथा
	    {
		dirnamelen = home_replace(रिक्त, p, स_नाम, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		जर (*स_नाम == '~')
		{
		    s = vim_strnsave(स_नाम, dirnamelen);
		    जर (s != रिक्त)
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
    पर्यंत (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	पर्यंत (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	जर (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	जर (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    जर (p == रिक्त)
		परत -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	अन्यथा
	{
	    पर्यंत (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    जर (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    जर (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	जर (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    जर (p == रिक्त)
		परत -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	जर (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    जर (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		परत -1;
	}
	अन्यथा
	{
	    माप_प्रकार	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    जर (get_short_pathname(fnamep, bufp, &l) == FAIL)
		परत -1;

	    जर (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		जर (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    परत -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    जर (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    पर्यंत (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	जर (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	अन्यथा
	    s = *fnamep + *fnamelen - 1;
	साठी ( ; s > tail; --s)
	    जर (s[0] == '.')
		थांबा;
	जर (src[*usedlen + 1] == 'e')		// :e
	{
	    जर (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		साठी ( ; s > *fnamep; --s)
		    जर (s[0] == ';')
			थांबा;
		जर (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    अन्यथा जर (*fnamep <= tail)
		*fnamelen = 0;
	}
	अन्यथा				// :r
	{
	    char_u *limit = *fnamep;

	    जर (limit < tail)
		limit = tail;
	    जर (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    जर (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	पूर्णांक	    sep;
	char_u	    *flags;
	पूर्णांक	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	जर (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	जर (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    जर (p != रिक्त)
	    {
		pat = vim_strnsave(s, p - s);
		जर (pat != रिक्त)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    जर (p != रिक्त)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			जर (sub != रिक्त && str != रिक्त)
			{
			    माप_प्रकार slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, रिक्त, flags, &slen);
			    जर (s != रिक्त)
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
	    जर (didit)
		जा repeat;
	}
    }

    जर (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	जर (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	जर (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	जर (p == रिक्त)
	    परत -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    परत valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    स्थिर शून्य
shorten_dir_len(char_u *str, पूर्णांक trim_len)
{
    char_u	*tail, *s, *d;
    पूर्णांक		skip = FALSE;
    पूर्णांक		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    साठी (s = str; ; ++s)
    {
	जर (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    जर (*s == NUL)
		थांबा;
	}
	अन्यथा जर (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	अन्यथा जर (!skip)
	{
	    *d++ = *s;			// copy next char
	    जर (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		जर (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    जर (has_mbyte)
	    {
		पूर्णांक l = mb_ptr2len(s);

		पर्यंत (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    शून्य
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    पूर्णांक
file_is_readable(char_u *fname)
{
    पूर्णांक		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    जर (*fname == NUL || mch_isdir_or_missing(fname))
	परत FALSE;
    जर ((fd = mch_open((अक्षर *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	बंद(fd);
	परत TRUE;
    }
    परत FALSE;
}

#जर defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    शून्य
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (check_secure())
	परत;

    जर (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	जर (in_vim9script())
	    (शून्य) check_for_string_arg(argvars, 0);
	परत;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    जर (cwd != रिक्त)
    {
	जर (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	जर (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	अन्यथा जर (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	अन्यथा जर (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	अन्यथा
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str_str), "scope", s);
	    परत;
	}
    }
    अन्यथा जर (curwin->w_localdir != रिक्त)
	scope = CDSCOPE_WINDOW;
    अन्यथा जर (curtab->tp_localdir != रिक्त)
	scope = CDSCOPE_TABPAGE;

    जर (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    शून्य
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_संख्या = -1;
    जर (check_restricted() || check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	परत;

    name = tv_get_string(&argvars[0]);
    जर (name == रिक्त || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	परत;
    }

    जर (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    अन्यथा
	flags = (char_u *)"";

    जर (*flags == NUL)
	// delete a file
	rettv->vval.v_संख्या = mch_remove(name) == 0 ? 0 : -1;
    अन्यथा जर (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_संख्या = mch_rmdir(name) == 0 ? 0 : -1;
    अन्यथा जर (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_संख्या = delete_recursive(name);
    अन्यथा
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    शून्य
f_executable(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_संख्या = mch_can_exe(tv_get_string(&argvars[0]), रिक्त, TRUE);
}

/*
 * "exepath()" function
 */
    शून्य
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = रिक्त;

    जर (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	परत;
    (शून्य)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    शून्य
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;
    rettv->vval.v_संख्या = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    शून्य
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;
    rettv->vval.v_संख्या = filewritable(tv_get_string(&argvars[0]));
}

    स्थिर शून्य
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    पूर्णांक		find_what)
{
    char_u	*fname;
    char_u	*fresult = रिक्त;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    पूर्णांक		मोजणी = 1;
    पूर्णांक		first = TRUE;
    पूर्णांक		error = FALSE;

    rettv->vval.v_string = रिक्त;
    rettv->v_type = VAR_STRING;
    जर (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 2) == FAIL)))
	परत;

    fname = tv_get_string(&argvars[0]);

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	जर (p == रिक्त)
	    error = TRUE;
	अन्यथा
	{
	    जर (*p != NUL)
		path = p;

	    जर (argvars[2].v_type != VAR_UNKNOWN)
		मोजणी = (पूर्णांक)tv_get_संख्या_chk(&argvars[2], &error);
	}
    }

    जर (मोजणी < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    जर (*fname != NUL && !error)
    {
	char_u	*file_to_find = रिक्त;
	अक्षर	*शोध_ctx = रिक्त;

	करा
	{
	    जर (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : रिक्त,
					       first ? (पूर्णांक)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &शोध_ctx);
	    first = FALSE;

	    जर (fresult != रिक्त && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} पर्यंत ((rettv->v_type == VAR_LIST || --मोजणी > 0) && fresult != रिक्त);

	vim_free(file_to_find);
	vim_findfile_cleanup(शोध_ctx);
    }

    जर (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    शून्य
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    शून्य
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    शून्य
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    माप_प्रकार	usedlen = 0;
    माप_प्रकार	len = 0;
    char_u	*fbuf = रिक्त;
    char_u	buf[NUMBUFLEN];

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    जर (mods == रिक्त || fname == रिक्त)
	fname = रिक्त;
    अन्यथा
    {
	len = STRLEN(fname);
	जर (mods != रिक्त && *mods != NUL)
	    (शून्य)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    जर (fname == रिक्त)
	rettv->vval.v_string = रिक्त;
    अन्यथा
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
    शून्य
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = रिक्त;
    tabpage_T	*tp = रिक्त;
    char_u	*cwd;
    पूर्णांक		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (in_vim9script()
	    && (check_for_opt_संख्या_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 1) == FAIL)))
	परत;

    जर (argvars[0].v_type == VAR_संख्या
	    && argvars[0].vval.v_संख्या == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    अन्यथा
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    जर (wp != रिक्त && wp->w_localdir != रिक्त
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    अन्यथा जर (tp != रिक्त && tp->tp_localdir != रिक्त
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    अन्यथा जर (wp != रिक्त || tp != रिक्त || global)
    {
	जर (globaldir != रिक्त && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	अन्यथा
	{
	    cwd = alloc(MAXPATHL);
	    जर (cwd != रिक्त)
	    {
		जर (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    जर (rettv->vval.v_string != रिक्त)
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
    पूर्णांक		    i;

    साठी (i = 0; i < 9; i++)
    {
	जर (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	अन्यथा
	    perm[i] = '-';
    }
    परत perm;
}

/*
 * "getfperm({fname})" function
 */
    शून्य
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = रिक्त;
    char_u	permbuf[] = "---------";

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    जर (mch_stat((अक्षर *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    शून्य
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    fname = tv_get_string(&argvars[0]);
    जर (mch_stat((अक्षर *)fname, &st) >= 0)
    {
	जर (S_ISDIR(st.st_mode))
	    rettv->vval.v_संख्या = 0;
	अन्यथा
	{
	    rettv->vval.v_संख्या = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    जर ((off_T)rettv->vval.v_संख्या != (off_T)st.st_size)
		rettv->vval.v_संख्या = -2;
	}
    }
    अन्यथा
	  rettv->vval.v_संख्या = -1;
}

/*
 * "getftime({fname})" function
 */
    शून्य
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    fname = tv_get_string(&argvars[0]);
    जर (mch_stat((अक्षर *)fname, &st) >= 0)
	rettv->vval.v_संख्या = (varnumber_T)st.st_mtime;
    अन्यथा
	rettv->vval.v_संख्या = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    अक्षर    *t;

    जर (S_ISREG(st->st_mode))
	t = "file";
    अन्यथा जर (S_ISDIR(st->st_mode))
	t = "dir";
    अन्यथा जर (S_ISLNK(st->st_mode))
	t = "link";
    अन्यथा जर (S_ISBLK(st->st_mode))
	t = "bdev";
    अन्यथा जर (S_ISCHR(st->st_mode))
	t = "cdev";
    अन्यथा जर (S_ISFIFO(st->st_mode))
	t = "fifo";
    अन्यथा जर (S_ISSOCK(st->st_mode))
	t = "socket";
    अन्यथा
	t = "other";
    परत (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    शून्य
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = रिक्त;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    जर (mch_lstat((अक्षर *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    शून्य
f_glob(typval_T *argvars, typval_T *rettv)
{
    पूर्णांक		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    पूर्णांक		error = FALSE;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	परत;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	जर (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	जर (argvars[2].v_type != VAR_UNKNOWN)
	{
	    जर (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, रिक्त);
	    जर (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    जर (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	जर (p_wic)
	    options += WILD_ICASE;
	जर (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     रिक्त, options, WILD_ALL);
	अन्यथा जर (rettv_list_alloc(rettv) == OK)
	{
	  पूर्णांक i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						रिक्त, options, WILD_ALL_KEEP);
	  साठी (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    अन्यथा
	rettv->vval.v_string = रिक्त;
}

/*
 * "glob2regpat()" function
 */
    शून्य
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == रिक्त)
			 ? रिक्त : file_pat_to_reg_pat(pat, रिक्त, रिक्त, FALSE);
}

/*
 * "globpath()" function
 */
    शून्य
f_globpath(typval_T *argvars, typval_T *rettv)
{
    पूर्णांक		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    पूर्णांक		error = FALSE;
    garray_T	ga;
    पूर्णांक		i;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	परत;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	जर (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	जर (argvars[3].v_type != VAR_UNKNOWN)
	{
	    जर (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, रिक्त);
	    जर (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    जर (file != रिक्त && !error)
    {
	ga_init2(&ga, आकार(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	जर (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	अन्यथा जर (rettv_list_alloc(rettv) == OK)
	    साठी (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    अन्यथा
	rettv->vval.v_string = रिक्त;
}

/*
 * "isdirectory()" function
 */
    शून्य
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    rettv->vval.v_संख्या = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    शून्य
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    rettv->vval.v_संख्या = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    स्थिर पूर्णांक
mkdir_recurse(char_u *dir, पूर्णांक prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    पूर्णांक		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    जर (p <= get_past_head(dir))
	परत OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    जर (updir == रिक्त)
	परत FAIL;
    जर (mch_isdir(updir))
	r = OK;
    अन्यथा जर (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	जर (r == OK && created != रिक्त && *created == रिक्त)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    परत r;
}

/*
 * "mkdir()" function
 */
    शून्य
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    पूर्णांक		prot = 0755;
    पूर्णांक		defer = FALSE;
    पूर्णांक		defer_recurse = FALSE;
    char_u	*created = रिक्त;

    rettv->vval.v_संख्या = FAIL;
    जर (check_restricted() || check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 2) == FAIL)))
	परत;

    dir = tv_get_string_buf(&argvars[0], buf);
    जर (*dir == NUL)
	परत;

    जर (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	जर (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (पूर्णांक)tv_get_संख्या_chk(&argvars[2], रिक्त);
	    जर (prot == -1)
		परत;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != रिक्त;
	defer_recurse = vim_strchr(arg2, 'R') != रिक्त;
	जर ((defer || defer_recurse) && !can_add_defer())
	    परत;

	जर (vim_strchr(arg2, 'p') != रिक्त)
	{
	    जर (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_संख्या = OK;
		परत;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : रिक्त);
	}
    }
    rettv->vval.v_संख्या = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    जर (rettv->vval.v_संख्या == OK
				&& created == रिक्त && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    जर (created != रिक्त)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	जर (tv[0].vval.v_string == रिक्त || tv[1].vval.v_string == रिक्त
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
    शून्य
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    पूर्णांक		trim_len = 1;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (पूर्णांक)tv_get_संख्या(&argvars[1]);
	जर (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    जर (p == रिक्त)
	rettv->vval.v_string = रिक्त;
    अन्यथा
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	जर (p != रिक्त)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    स्थिर पूर्णांक
checkitem_common(शून्य *context, char_u *name, dict_T *dict)
{
    typval_T	*अभिव्यक्ती = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    पूर्णांक		retval = 0;
    पूर्णांक		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    जर (name != रिक्त)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    अन्यथा
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    जर (eval_अभिव्यक्ती_typval(अभिव्यक्ती, FALSE, argv, 1, रिक्त, &rettv) == FAIL)
	जा theend;

    // We want to use -1, but also true/false should be allowed.
    जर (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_संख्या;
	rettv.vval.v_संख्या = rettv.vval.v_संख्या == VVAL_TRUE;
    }
    retval = tv_get_संख्या_chk(&rettv, &error);
    जर (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    जर (name != रिक्त)
	set_vim_var_string(VV_VAL, रिक्त, 0);
    अन्यथा
	set_vim_var_dict(VV_VAL, रिक्त);
    restore_vimvar(VV_VAL, &save_val);
    परत retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    स्थिर पूर्णांक
readdir_checkitem(शून्य *context, शून्य *item)
{
    char_u	*name = (char_u *)item;

    परत checkitem_common(context, name, रिक्त);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    स्थिर पूर्णांक
readdirex_dict_arg(typval_T *argvars, पूर्णांक *cmp)
{
    char_u     *compare;

    जर (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	परत FAIL;

    जर (dict_has_किल्ली(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    अन्यथा
    {
	semsg(_(e_dictionary_किल्ली_str_required), "sort");
	परत FAIL;
    }

    जर (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    अन्यथा जर (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    अन्यथा जर (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    अन्यथा जर (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    परत OK;
}

/*
 * "readdir()" function
 */
    शून्य
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*अभिव्यक्ती;
    पूर्णांक		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    पूर्णांक		i;
    पूर्णांक		sort = READDIR_SORT_BYTE;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;
    जर (check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	परत;

    path = tv_get_string(&argvars[0]);
    अभिव्यक्ती = &argvars[1];

    जर (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	परत;

    ret = readdir_core(&ga, path, FALSE, (शून्य *)अभिव्यक्ती,
	    (अभिव्यक्ती->v_type == VAR_UNKNOWN) ? रिक्त : readdir_checkitem, sort);
    जर (ret == OK)
    {
	साठी (i = 0; i < ga.ga_len; i++)
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
    स्थिर पूर्णांक
readdirex_checkitem(शून्य *context, शून्य *item)
{
    dict_T	*dict = (dict_T*)item;

    परत checkitem_common(context, रिक्त, dict);
}

/*
 * "readdirex()" function
 */
    शून्य
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*अभिव्यक्ती;
    पूर्णांक		ret;
    char_u	*path;
    garray_T	ga;
    पूर्णांक		i;
    पूर्णांक		sort = READDIR_SORT_BYTE;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;
    जर (check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	परत;

    path = tv_get_string(&argvars[0]);
    अभिव्यक्ती = &argvars[1];

    जर (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	परत;

    ret = readdir_core(&ga, path, TRUE, (शून्य *)अभिव्यक्ती,
	    (अभिव्यक्ती->v_type == VAR_UNKNOWN) ? रिक्त : readdirex_checkitem, sort);
    जर (ret == OK)
    {
	साठी (i = 0; i < ga.ga_len; i++)
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
    स्थिर शून्य
read_file_or_blob(typval_T *argvars, typval_T *rettv, पूर्णांक always_blob)
{
    पूर्णांक		द्विमान = FALSE;
    पूर्णांक		blob = always_blob;
    पूर्णांक		failed = FALSE;
    char_u	*fname;
    खाता	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    पूर्णांक		io_size = आकार(buf);
    पूर्णांक		readlen;		// size of last fread()
    char_u	*prev	 = रिक्त;	// previously read bytes, if any
    दीर्घ	prevlen  = 0;		// length of data in prev
    दीर्घ	prevsize = 0;		// size of prev buffer
    दीर्घ	maxline  = MAXLNUM;
    दीर्घ	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	जर (always_blob)
	{
	    offset = (off_T)tv_get_संख्या(&argvars[1]);
	    जर (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_संख्या(&argvars[2]);
	}
	अन्यथा
	{
	    जर (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		द्विमान = TRUE;
	    जर (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    जर (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (दीर्घ)tv_get_संख्या(&argvars[2]);
	}
    }

    जर ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	परत;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    जर (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	परत;
    }
    जर (*fname == NUL || (fd = mch_fopen((अक्षर *)fname, READBIN)) == रिक्त)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	परत;
    }

    जर (blob)
    {
	जर (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	ख_बंद(fd);
	परत;
    }

    पर्यंत (cnt < maxline || maxline < 0)
    {
	readlen = (पूर्णांक)ख_पढ़ो(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	साठी (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || द्विमान));
		++p)
	{
	    जर (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= रिक्त;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		जर (readlen > 0 && !द्विमान)
		{
		    पर्यंत (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    जर (len == 0)
			पर्यंत (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		जर (prevlen == 0)
		    s = vim_strnsave(start, len);
		अन्यथा
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    जर ((s = vim_realloc(prev, prevlen + len + 1)) != रिक्त)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = रिक्त; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		जर (s == रिक्त)
		{
		    do_outofmem_संदेश((long_u) prevlen + len + 1);
		    failed = TRUE;
		    थांबा;
		}

		जर ((li = listitem_alloc()) == रिक्त)
		{
		    vim_free(s);
		    failed = TRUE;
		    थांबा;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		जर ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    थांबा;
	    }
	    अन्यथा जर (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    अन्यथा जर (*p == 0xbf && enc_utf8 && !द्विमान)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		जर (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    जर (start == dest)
			start = p + 1;
		    अन्यथा
		    {
			// have to shuffle buf to close gap
			पूर्णांक adjust_prevlen = 0;

			जर (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (पूर्णांक)(buf - dest);
			    dest = buf;
			}
			जर (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	जर (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    थांबा;
	जर (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    जर (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		जर (prevsize == 0)
		    prevsize = (दीर्घ)(p - start);
		अन्यथा
		{
		    दीर्घ grow50pc = (prevsize * 3) / 2;
		    दीर्घ growmin  = (दीर्घ)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		जर (newprev == रिक्त)
		{
		    do_outofmem_संदेश((long_u)prevsize);
		    failed = TRUE;
		    थांबा;
		}
		prev = newprev;
	    }
	    // Add the line part to end of "prev".
	    mch_memmove(prev + prevlen, start, p - start);
	    prevlen += (दीर्घ)(p - start);
	}
    } // while

    // For a negative line count use only the lines at the end of the file,
    // free the rest.
    जर (!failed && maxline < 0)
	पर्यंत (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    जर (failed)
    {
	// an empty list is returned on error
	list_free(rettv->vval.v_list);
	rettv_list_alloc(rettv);
    }

    vim_free(prev);
    ख_बंद(fd);
}

/*
 * "readblob()" function
 */
    शून्य
f_readblob(typval_T *argvars, typval_T *rettv)
{
    जर (check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 2) == FAIL)))
	परत;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    शून्य
f_readfile(typval_T *argvars, typval_T *rettv)
{
    जर (check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 2) == FAIL)))
	परत;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    शून्य
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = रिक्त;
# endif

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = रिक्त;

	v = mch_resolve_path(p, TRUE);
	जर (v != रिक्त)
	    rettv->vval.v_string = v;
	अन्यथा
	    rettv->vval.v_string = vim_strsave(p);
    }
# अन्यथा
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	पूर्णांक	len;
	char_u	*remain = रिक्त;
	char_u	*q;
	पूर्णांक	is_relative_to_current = FALSE;
	पूर्णांक	has_trailing_pathsep = FALSE;
	पूर्णांक	limit = 100;

	p = vim_strsave(p);
	जर (p == रिक्त)
	    जा fail;
	जर (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	जर (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	जर (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	जर (buf == रिक्त)
	{
	    vim_free(p);
	    vim_free(remain);
	    जा fail;
	}

	साठी (;;)
	{
	    साठी (;;)
	    {
		len = readlink((अक्षर *)p, (अक्षर *)buf, MAXPATHL);
		जर (len <= 0)
		    थांबा;
		buf[len] = NUL;

		जर (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = रिक्त;
		    जा fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		जर (remain == रिक्त && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		जर (*q != NUL)
		{
		    जर (remain == रिक्त)
			remain = vim_strsave(q - 1);
		    अन्यथा
		    {
			cpy = concat_str(q - 1, remain);
			जर (cpy != रिक्त)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		जर (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		जर (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    जर (cpy != रिक्त)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		अन्यथा
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    जर (remain == रिक्त)
		थांबा;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    जर (cpy != रिक्त)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    जर (*q != NUL)
		STRMOVE(remain, q - 1);
	    अन्यथा
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	जर (!vim_ispathsep(*p))
	{
	    जर (is_relative_to_current
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
		जर (cpy != रिक्त)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    अन्यथा जर (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		पर्यंत (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		जर (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	जर (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    जर (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  अन्यथा
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
    शून्य
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    स्थिर पूर्णांक	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    करा
    {
	जर (x == 'Z')
	    x = '0';
	अन्यथा जर (x == '9')
	    x = 'A';
	अन्यथा
	    ++x;
    } पर्यंत (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    शून्य
f_writefile(typval_T *argvars, typval_T *rettv)
{
    पूर्णांक		द्विमान = FALSE;
    पूर्णांक		append = FALSE;
    पूर्णांक		defer = FALSE;
# ifdef HAVE_FSYNC
    पूर्णांक		do_fsync = p_fs;
# endif
    char_u	*fname;
    खाता	*fd;
    पूर्णांक		ret = 0;
    listitem_T	*li;
    list_T	*list = रिक्त;
    blob_T	*blob = रिक्त;

    rettv->vval.v_संख्या = -1;
    जर (check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	परत;

    जर (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	जर (list == रिक्त)
	    परत;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_वस्तुसंच(list, li)
	    जर (tv_get_string_chk(&li->li_tv) == रिक्त)
		परत;
    }
    अन्यथा जर (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	जर (blob == रिक्त)
	    परत;
    }
    अन्यथा
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	परत;
    }

    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	जर (arg2 == रिक्त)
	    परत;
	जर (vim_strchr(arg2, 'b') != रिक्त)
	    द्विमान = TRUE;
	जर (vim_strchr(arg2, 'a') != रिक्त)
	    append = TRUE;
	जर (vim_strchr(arg2, 'D') != रिक्त)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	जर (vim_strchr(arg2, 's') != रिक्त)
	    do_fsync = TRUE;
	अन्यथा जर (vim_strchr(arg2, 'S') != रिक्त)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    जर (fname == रिक्त)
	परत;

    जर (defer && !can_add_defer())
	परत;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    जर (*fname == NUL || (fd = mch_fopen((अक्षर *)fname,
				      append ? APPENDBIN : WRITEBIN)) == रिक्त)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    अन्यथा
    {
	जर (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    जर (tv.vval.v_string == रिक्त
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		ख_बंद(fd);
		(शून्य)mch_remove(fname);
	    }
	}

	जर (ret == 0)
	{
	    जर (blob)
	    {
		जर (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    अन्यथा
	    {
		जर (write_list(fd, list, द्विमान) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    जर (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    ख_बंद(fd);
	}
    }

    rettv->vval.v_संख्या = ret;
}

#endif // FEAT_EVAL

#जर defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    पूर्णांक		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    स्थिर char_u	*last_dir = रिक्त;    // last used directory
    char_u		*tofree = रिक्त;
    पूर्णांक			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    जर (title == रिक्त || *title == NUL)
    {
	जर (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	अन्यथा जर (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	अन्यथा
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    जर ((initdir == रिक्त || *initdir == NUL) && dflt != रिक्त && *dflt != NUL)
    {
	जर (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = रिक्त;
	}
	अन्यथा जर (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    जर (tofree != रिक्त)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    जर (initdir == रिक्त || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	जर (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	अन्यथा जर (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != रिक्त && buf->b_ffname != रिक्त)
	{
	    जर (dflt == रिक्त || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    जर (tofree != रिक्त)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	अन्यथा जर (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    जर (gui.in_use)		// when this changes, also adjust f_has()!
    {
	जर (filter == रिक्त
#  ifdef FEAT_EVAL
		&& (filter = get_var_मूल्य((char_u *)"b:browsefilter")) == रिक्त
		&& (filter = get_var_मूल्य((char_u *)"g:browsefilter")) == रिक्त
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	जर (flags & BROWSE_DIR)
	{
#  जर defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  अन्यथा
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  जर !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    जर (fname != रिक्त && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		जर (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	अन्यथा
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    अन्यथा
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = रिक्त;
    }

    // keep the directory for next time
    जर (fname != रिक्त)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	जर (last_dir != रिक्त && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    जर (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		जर (last_dir != रिक्त)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    परत fname;
}
#endif

#जर defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    शून्य
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    पूर्णांक		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    पूर्णांक		error = FALSE;

    जर (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	परत;

    save = (पूर्णांक)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    जर (error || title == रिक्त || initdir == रिक्त || defname == रिक्त)
	rettv->vval.v_string = रिक्त;
    अन्यथा
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, रिक्त, initdir, रिक्त, curbuf);
# अन्यथा
    rettv->vval.v_string = रिक्त;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    शून्य
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    जर (title == रिक्त || initdir == रिक्त)
	rettv->vval.v_string = रिक्त;
    अन्यथा
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, रिक्त, रिक्त, initdir, रिक्त, curbuf);
# अन्यथा
    rettv->vval.v_string = रिक्त;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    शून्य
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_संख्या = FALSE;

    जर (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	परत;

    from = tv_get_string(&argvars[0]);

    जर (mch_lstat((अक्षर *)from, &st) >= 0
	&& (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)))
	rettv->vval.v_संख्या = vim_copyfile(
	    tv_get_string(&argvars[0]),
	    tv_get_string(&argvars[1])) == OK ? TRUE : FALSE;
}

#endif // FEAT_EVAL

/*
 * Replace home directory by "~" in each space or comma separated file name in
 * 'src'.
 * If anything fails (except when out of space) dst equals src.
 */
    माप_प्रकार
home_replace(
    buf_T	*buf,	// when not NULL, check for help files
    char_u	*src,	// input file name
    char_u	*dst,	// where to put the result
    पूर्णांक		dstlen,	// maximum length of the result
    पूर्णांक		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    माप_प्रकार	dirlen = 0, envlen = 0;
    माप_प्रकार	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    जर (src == रिक्त)
    {
	*dst = NUL;
	परत 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    जर (buf != रिक्त && buf->b_help)
	परत vim_snprintf_safelen((अक्षर *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    जर (homedir != रिक्त)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#अन्यथा
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    जर (homedir_env == रिक्त)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    जर (homedir_env != रिक्त && *homedir_env == NUL)
	homedir_env = रिक्त;

    जर (homedir_env != रिक्त && *homedir_env == '~')
    {
	माप_प्रकार	usedlen = 0;
	माप_प्रकार	flen;
	char_u	*fbuf = रिक्त;

	flen = STRLEN(homedir_env);
	(शून्य)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	जर (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    जर (homedir_env != रिक्त)
	envlen = STRLEN(homedir_env);

    जर (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    पर्यंत (*src && dstlen > 0)
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
	साठी (;;)
	{
	    जर (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		जर (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		थांबा;
	    }
	    जर (p == homedir_env)
		थांबा;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	पर्यंत (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	पर्यंत ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    जर (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    परत (माप_प्रकार)(dst - dst_start);
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
    अचिन्हित	len;

    len = 3;			// space for "~/" and trailing NUL
    जर (src != रिक्त)		// just in case
	len += (अचिन्हित)STRLEN(src);
    dst = alloc(len);
    जर (dst != रिक्त)
	home_replace(buf, src, dst, len, TRUE);
    परत dst;
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
    पूर्णांक
fullpathcmp(
    char_u *s1,
    char_u *s2,
    पूर्णांक	    checkname,		// when both don't exist, check file names
    पूर्णांक	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    पूर्णांक		    r1, r2;

    जर (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    अन्यथा
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((अक्षर *)exp1, &st1);
    r2 = mch_stat((अक्षर *)s2, &st2);
    जर (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	जर (checkname)
	{
	    जर (fnamecmp(exp1, s2) == 0)
		परत FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    जर (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		परत FPC_SAMEX;
	}
	परत FPC_NOTX;
    }
    जर (r1 != 0 || r2 != 0)
	परत FPC_DIFFX;
    जर (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	परत FPC_SAME;
    परत FPC_DIFF;
#अन्यथा
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    पूर्णांक	    retval = FPC_DIFF;
    पूर्णांक	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    जर ((exp1 = alloc(MAXPATHL * 3)) != रिक्त)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	जर (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	अन्यथा
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	जर (r1 != OK && r2 != OK)
	{
	    जर (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    अन्यथा
		retval = FPC_NOTX;
	}
	अन्यथा जर (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	अन्यथा जर (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	अन्यथा
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    परत retval;
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

    जर (fname == रिक्त)
	परत (char_u *)"";
    साठी (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	जर (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    परत p1;
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
    पर्यंत (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    परत t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    पर्यंत (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    जर (*fname)
	++fname;
    परत fname;
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

#जर defined(MSWIN)
    // may skip "c:"
    जर (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    अन्यथा
	retval = path;
#अन्यथा
# जर defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    जर (retval == रिक्त)
	retval = path;
# अन्यथा	// Unix
    retval = path;
# endif
#endif

    पर्यंत (vim_ispathsep(*retval))
	++retval;

    परत retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    पूर्णांक
vim_ispathsep(पूर्णांक c)
{
#ifdef UNIX
    परत (c == '/');	    // UNIX has ':' inside file names
#अन्यथा
# ifdef BACKSLASH_IN_FILENAME
    परत (c == ':' || c == '/' || c == '\\');
# अन्यथा
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    परत (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  अन्यथा
    परत (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    पूर्णांक
vim_ispathsep_nocolon(पूर्णांक c)
{
    परत vim_ispathsep(c)
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
    पूर्णांक
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    पूर्णांक		c;
    पूर्णांक		retval;

    p = gettail_sep(fname);
    जर (p == fname)
	परत TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    परत retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    पूर्णांक
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    परत vim_fnamencmp(x, y, MAXPATHL);
#अन्यथा
    जर (p_fic)
	परत MB_STRICMP(x, y);
    परत STRCMP(x, y);
#endif
}

    पूर्णांक
vim_fnamencmp(char_u *x, char_u *y, माप_प्रकार len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    पूर्णांक		cx = NUL;
    पूर्णांक		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    साठी (पूर्णांक swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	जर ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    जर (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		थांबा;
	    }
	    अन्यथा // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    पर्यंत (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	जर (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    थांबा;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    जर (len == 0)
	परत 0;
    परत (cx - cy);
#अन्यथा
    जर (p_fic)
	परत MB_STRNICMP(x, y, len);
    परत STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, माप_प्रकार fname1len, char_u *fname2, माप_प्रकार fname2len, पूर्णांक sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? आकार(PATHSEP) : 0) + fname2len + 1);
    जर (ret->string == रिक्त)
	ret->length = 0;
    अन्यथा
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	जर (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += आकार(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    परत ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    शून्य
add_pathsep(char_u *p)
{
    माप_प्रकार  plen;

    जर (*p == NUL)
	परत;

    plen = STRLEN(p);
    जर (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    पूर्णांक		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = रिक्त;

    जर (fname == रिक्त)
	परत रिक्त;

    buf = alloc(MAXPATHL);
    जर (buf == रिक्त)
	परत रिक्त;

    जर (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    अन्यथा
	new_fname = vim_strsave(fname);
    vim_free(buf);
    परत new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    पूर्णांक
vim_fexists(char_u *fname)
{
    stat_T st;

    जर (mch_stat((अक्षर *)fname, &st))
	परत FALSE;
    परत TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    पूर्णांक
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    पूर्णांक		  *अंक_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    पूर्णांक		   flags)	// EW_DIR, etc.
{
    पूर्णांक		ret = FAIL;
    char_u	*eval_pat = रिक्त;
    char_u	*exp_pat = *pat;
    अक्षर	*ignored_संदेश;
    माप_प्रकार	usedlen;
    पूर्णांक		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    पूर्णांक		star_follows = FALSE;

    जर (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       रिक्त, &ignored_संदेश, रिक्त, TRUE);
	--emsg_off;
	जर (eval_pat != रिक्त)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    जर (exp_pat != रिक्त)
	ret = expand_wildcards(1, &exp_pat, अंक_file, file, flags);

    जर (eval_pat != रिक्त)
    {
	जर (*अंक_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    जर (*file != रिक्त)
	    {
		**file = eval_pat;
		eval_pat = रिक्त;
		*अंक_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    परत ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    पूर्णांक
expand_wildcards(
    पूर्णांक		   अंक_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    पूर्णांक		  *अंक_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    पूर्णांक		   flags)	// EW_DIR, etc.
{
    पूर्णांक		retval;
    पूर्णांक		i, j;
    char_u	*p;
    पूर्णांक		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(अंक_pat, pat, अंक_files, files, flags);

    // When keeping all matches, return here
    जर ((flags & EW_KEEPALL) || retval == FAIL)
	परत retval;

    /*
     * Remove names that match 'wildignore'.
     */
    जर (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	साठी (i = 0; i < *अंक_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    जर (ffname == रिक्त)		// out of memory
		थांबा;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    जर (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		साठी (j = i; j + 1 < *अंक_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*अंक_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	जर (*अंक_files == 0)
	{
	    VIM_CLEAR(*files);
	    परत FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    जर (*अंक_files > 1 && !got_int)
    {
	non_suf_match = 0;
	साठी (i = 0; i < *अंक_files; ++i)
	{
	    जर (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		साठी (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    परत retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    पूर्णांक
match_suffix(char_u *fname)
{
    पूर्णांक		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (पूर्णांक)STRLEN(fname);
    setsuflen = 0;
    साठी (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	जर (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    जर (vim_strchr(tail, '.') == रिक्त)
	    {
		setsuflen = 1;
		थांबा;
	    }
	}
	अन्यथा
	{
	    जर (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (माप_प्रकार)setsuflen) == 0)
		थांबा;
	    setsuflen = 0;
	}
    }
    परत (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    स्थिर पूर्णांक
vim_backtick(char_u *p)
{
    परत (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    स्थिर पूर्णांक
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    पूर्णांक		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    पूर्णांक		cnt = 0;
    पूर्णांक		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    जर (cmd == रिक्त)
	परत -1;

#ifdef FEAT_EVAL
    जर (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    अन्यथा
#endif
	buffer = get_cmd_output(cmd, रिक्त,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, रिक्त);
    vim_free(cmd);
    जर (buffer == रिक्त)
	परत -1;

    cmd = buffer;
    पर्यंत (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	पर्यंत (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	जर (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	पर्यंत (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    परत cnt;
}

#जर defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    स्थिर पूर्णांक
pstrcmp(स्थिरांक शून्य *a, स्थिरांक शून्य *b)
{
    परत (pathcmp(*(अक्षर **)a, *(अक्षर **)b, -1));
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
    पूर्णांक
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    माप_प्रकार	wildoff,
    पूर्णांक		flags,		// EW_* flags
    पूर्णांक		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    पूर्णांक		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    पूर्णांक		starts_with_dot;
    पूर्णांक		matches;
    माप_प्रकार	buflen;
    माप_प्रकार	len;
    पूर्णांक		starstar = FALSE;
    स्थिर पूर्णांक	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_मूल्य;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = रिक्त;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    पूर्णांक			ok;
    char_u		*p_alt;
    पूर्णांक			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    जर (stardepth > 0)
    {
	ui_breakcheck();
	जर (got_int)
	    परत 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    जर (buf == रिक्त)
	परत 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = रिक्त;
    path_end = path;
    पर्यंत (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	जर (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	अन्यथा जर (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    जर (e != रिक्त)
		थांबा;
	    s = p + 1;
	}
	अन्यथा जर (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != रिक्त)
	    e = p;
	जर (has_mbyte)
	{
	    पूर्णांक charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (माप_प्रकार)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	अन्यथा
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    साठी (p = buf + wildoff; p < s; ++p)
	जर (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    साठी (p = s; p < e; ++p)
	जर (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, रिक्त, FALSE);
    जर (pat == रिक्त)
    {
	vim_free(buf);
	परत 0;
    }

    // compile the regexp into a program
    जर (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    जर (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    जर (regmatch.regprog == रिक्त && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	परत 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != रिक्त;

    len = (माप_प्रकार)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    जर (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((अक्षर *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(शून्य)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((अक्षर *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, रिक्त);
    जर (wn != रिक्त)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_मूल्य);

    पर्यंत (ok)
    {
	p = utf16_to_enc(wfb.cFileName, रिक्त);   // p is allocated here

	जर (p == रिक्त)
	    थांबा;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	जर (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = रिक्त;
	अन्यथा
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, रिक्त);

	len = (माप_प्रकार)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	जर ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == रिक्त
		  || (regmatch.regprog != रिक्त
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != रिक्त
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((अक्षर *)s, buflen - len, "%s", p);
	    जर (len + 1 < buflen)
	    {
		जर (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((अक्षर *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (शून्य)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((अक्षर *)buf + len, buflen - len, "%s", path_end);
		जर (mch_has_exp_wildcard(path_end))
		{
		    जर (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(शून्य)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		अन्यथा
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    जर (*path_end != 0)
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
    जर (matches > 0)
	क्विक(((char_u **)gap->ga_data) + start_len, (माप_प्रकार)matches,
						   आकार(char_u *), pstrcmp);
    परत matches;
}
#endif // MSWIN

#जर (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    स्थिर पूर्णांक
pstrcmp(स्थिरांक शून्य *a, स्थिरांक शून्य *b)
{
    परत (pathcmp(*(अक्षर **)a, *(अक्षर **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    पूर्णांक
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    माप_प्रकार	wildoff,
    पूर्णांक		flags,		// EW_* flags
    पूर्णांक		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    पूर्णांक		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    पूर्णांक		starts_with_dot;
    पूर्णांक		matches;
    माप_प्रकार	buflen;
    माप_प्रकार	len;
    पूर्णांक		starstar = FALSE;
    स्थिर पूर्णांक	stardepth = 0;	    // depth for "**" expansion

    सूची		*dirp;
    संरचना dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    जर (stardepth > 0)
    {
	ui_breakcheck();
	जर (got_int)
	    परत 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    जर (buf == रिक्त)
	परत 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = रिक्त;
    path_end = path;
    पर्यंत (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	जर (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	अन्यथा जर (*path_end == '/')
	{
	    जर (e != रिक्त)
		थांबा;
	    s = p + 1;
	}
	अन्यथा जर (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != रिक्त
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	जर (has_mbyte)
	{
	    पूर्णांक charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (माप_प्रकार)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	अन्यथा
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    साठी (p = buf + wildoff; p < s; ++p)
	जर (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    साठी (p = s; p < e; ++p)
	जर (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, रिक्त, FALSE);
    जर (pat == रिक्त)
    {
	vim_free(buf);
	परत 0;
    }

    // compile the regexp into a program
    जर (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    अन्यथा
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    जर (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    जर (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    जर (regmatch.regprog == रिक्त && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	परत 0;
    }

    len = (माप_प्रकार)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    जर (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((अक्षर *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(शून्य)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = सूची_खोलो(*buf == NUL ? "." : (अक्षर *)buf);

    // Find all matching entries
    जर (dirp != रिक्त)
    {
	पर्यंत (!got_int)
	{
	    dp = सूची_पढ़ो(dirp);
	    जर (dp == रिक्त)
		थांबा;
	    len = (माप_प्रकार)(s - buf);
	    जर ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != रिक्त && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((अक्षर *)s, buflen - len, "%s", dp->d_name);
		जर (len + 1 >= buflen)
		    सुरूठेवा;

		जर (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((अक्षर *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (शून्य)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((अक्षर *)buf + len, buflen - len, "%s", path_end);
		जर (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    जर (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(शून्य)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		अन्यथा
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    जर (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    जर ((flags & EW_ALLLINKS) ? mch_lstat((अक्षर *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			माप_प्रकार precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			जर (precomp_buf)
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

	बंद_सूची(dirp);
    }

    vim_free(buf);
    vim_regfree(regmatch.regprog);

    // When interrupted the matches probably won't be used and sorting can be
    // slow, thus skip it.
    matches = gap->ga_len - start_len;
    जर (matches > 0 && !got_int)
	क्विक(((char_u **)gap->ga_data) + start_len, matches,
						   आकार(char_u *), pstrcmp);
    परत matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    स्थिर पूर्णांक
has_env_var(char_u *p)
{
    साठी ( ; *p; MB_PTR_ADV(p))
    {
	जर (*p == '\\' && p[1] != NUL)
	    ++p;
	अन्यथा जर (vim_strchr((char_u *)
#जर defined(MSWIN)
				    "$%"
#अन्यथा
				    "$"
#endif
					, *p) != रिक्त)
	    परत TRUE;
    }
    परत FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    स्थिर पूर्णांक
has_special_wildchar(char_u *p)
{
    साठी ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	जर (*p == '\r' || *p == '\n')
	    थांबा;
	// Allow for escaping.
	जर (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	अन्यथा जर (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != रिक्त)
	{
	    // A { must be followed by a matching }.
	    जर (*p == '{' && vim_strchr(p, '}') == रिक्त)
		सुरूठेवा;
	    // A quote and backtick must be followed by another one.
	    जर ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == रिक्त)
		सुरूठेवा;
	    परत TRUE;
	}
    }
    परत FALSE;
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
    पूर्णांक
gen_expand_wildcards(
    पूर्णांक		अंक_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    पूर्णांक		*अंक_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    पूर्णांक		flags)		// EW_* flags
{
    पूर्णांक			i;
    garray_T		ga;
    char_u		*p;
    स्थिर पूर्णांक		recursive = FALSE;
    पूर्णांक			add_pat;
    पूर्णांक			retval = OK;
    पूर्णांक			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    जर (recursive)
#ifdef SPECIAL_WILDCHAR
	परत mch_expand_wildcards(अंक_pat, pat, अंक_file, file, flags);
#अन्यथा
	परत FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    साठी (i = 0; i < अंक_pat; i++)
    {
	जर (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    परत mch_expand_wildcards(अंक_pat, pat, अंक_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, आकार(char_u *), 30);

    साठी (i = 0; i < अंक_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	जर (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    जर (add_pat == -1)
		retval = FAIL;
	}
	अन्यथा
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    जर ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		जर (p == रिक्त)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		अन्यथा जर (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(अंक_pat, pat, अंक_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    परत i;
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
	    जर (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		जर ((flags & (EW_PATH | EW_CDPATH))
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
		अन्यथा
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	जर (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    जर (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    अन्यथा
		addfile(&ga, t, flags);

	    जर (t != p)
		vim_free(t);
	}

	जर (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	जर (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    जर (retval == FAIL)
	ga_clear_strings(&ga);

    *अंक_file = ga.ga_len;
    *file = (ga.ga_data != रिक्त) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    परत ((flags & EW_EMPTYOK) || ga.ga_data != रिक्त) ? retval : FAIL;
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
    शून्य
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    पूर्णांक		flags)
{
    char_u	*p;
    पूर्णांक		isdir;
    stat_T	sb;

#ifdef MSWIN
    जर (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	दीर्घ perm = mch_getperm(f);

	जर (perm < 0)
	    परत;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    अन्यथा
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	जर (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((अक्षर *)f, &sb) < 0 : mch_getperm(f) < 0))
	    परत;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    जर (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != रिक्त)
	परत;
#endif

    जर ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	परत;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    जर (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, रिक्त, !(flags & EW_SHELLCMD)))
	परत;

    // Make room for another item in the file list.
    जर (ga_grow(gap, 1) == FAIL)
	परत;

    p = alloc(STRLEN(f) + 1 + isdir);
    जर (p == रिक्त)
	परत;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    जर (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    शून्य
FreeWild(पूर्णांक मोजणी, char_u **files)
{
    जर (मोजणी <= 0 || files == रिक्त)
	परत;
    पर्यंत (मोजणी--)
	vim_free(files[मोजणी]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    पूर्णांक
pathcmp(स्थिरांक अक्षर *p, स्थिरांक अक्षर *q, पूर्णांक maxlen)
{
    पूर्णांक		i, j;
    पूर्णांक		c1, c2;
    स्थिरांक अक्षर	*s = रिक्त;

    साठी (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	जर (c1 == NUL)
	{
	    जर (c2 == NUL)  // full match
		परत 0;
	    s = q;
	    i = j;
	    थांबा;
	}

	// End of "q": check if "p" just has a slash.
	जर (c2 == NUL)
	{
	    s = p;
	    थांबा;
	}

	जर ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    जर (vim_ispathsep(c1))
		परत -1;
	    जर (vim_ispathsep(c2))
		परत 1;
	    परत p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    जर (s == रिक्त)	// "i" or "j" ran into "maxlen"
	परत 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    जर (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#अन्यथा
	    && c1 == '/'
#endif
       )
	परत 0;   // match with trailing slash
    जर (s == q)
	परत -1;	    // no match
    परत 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    पूर्णांक
vim_isAbsName(char_u *name)
{
    परत (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णांक
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    पूर्णांक		len,
    पूर्णांक		force)	    // force expansion even when already absolute
{
    पूर्णांक		retval = OK;
    पूर्णांक		url;

    *buf = NUL;
    जर (fname == रिक्त)
	परत FAIL;

    url = path_with_url(fname);
    जर (!url)
	retval = mch_FullName(fname, buf, len, force);
    जर (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#जर defined(MSWIN)
    slash_adjust(buf);
#endif
    परत retval;
}
