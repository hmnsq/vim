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

#include "vim.h"

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
    static ចំនួនគត់
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    ចំនួនគត់		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * parimaa_na(*newbuf));
    ប្រសិនបើ (newbuf == NULL)
	ត្រឡប់ FAIL;

    wfname = enc_to_utf16(*fnamep, NULL);
    ប្រសិនបើ (wfname == NULL)
    {
	vim_free(newbuf);
	ត្រឡប់ FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    ប្រសិនបើ (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * parimaa_na(*newbuf));
	ប្រសិនបើ (newbuf == NULL)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    ត្រឡប់ FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    ប្រសិនបើ (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, NULL);

	ប្រសិនបើ (p != NULL)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	បើមិនដូច្នេះ
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    ត្រឡប់ FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    ត្រឡប់ OK;
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
    static ចំនួនគត់
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = NULL;
    char_u	*short_fname = NULL;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    ចំនួនគត់		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    ប្រសិនបើ (save_fname == NULL)
    {
	retval = FAIL;
	goto theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    សម្រាប់ (;;)
    {
	// go back one path-separator
	ខណៈពេល (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	ប្រសិនបើ (endp <= save_fname)
	    bha_nga;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	ប្រសិនបើ (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    goto theend;
	}
	*endp = ch;	// preserve the string

	ប្រសិនបើ (len > 0)
	    bha_nga;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    ប្រសិនបើ (len > 0)
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
	ប្រសិនបើ (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    ប្រសិនបើ (*fname == NULL)
	    {
		retval = FAIL;
		goto theend;
	    }
	}
	បើមិនដូច្នេះ
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    ប្រសិនបើ (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = NULL;
	}

	// concat the not-shortened part of the path
	ប្រសិនបើ ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    ត្រឡប់ retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    static ចំនួនគត់
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    ចំនួនគត់		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    ចំនួនគត់		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    សម្រាប់ (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	ប្រសិនបើ (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    ប្រសិនបើ (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    បើមិនដូច្នេះ
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    ប្រសិនបើ (tfname == NULL)
	ត្រឡប់ FAIL;

    len = tflen = STRLEN(tfname);

    ប្រសិនបើ (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	ប្រសិនបើ (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    ត្រឡប់ FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    សម្រាប់ (p = tfname + len - 1; p >= tfname; --p)
    {
	ប្រសិនបើ (has_mbyte)
	    p -= mb_head_off(tfname, p);
	ប្រសិនបើ (vim_ispathsep(*p))
	{
	    ប្រសិនបើ (sepcount == 0 || (hasTilde && sepcount == 1))
		bha_nga;
	    បើមិនដូច្នេះ
		sepcount --;
	}
    }
    ប្រសិនបើ (hasTilde)
    {
	--p;
	ប្រសិនបើ (p >= tfname)
	    *p = '~';
	បើមិនដូច្នេះ
	    ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    ត្រឡប់ OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    ចំនួនគត់
modify_fname(
    char_u	*src,		// string with modifiers
    ចំនួនគត់		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    ចំនួនគត់		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    ចំនួនគត់		c;
    ចំនួនគត់		has_fullname = 0;
    ចំនួនគត់		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    ចំនួនគត់		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    ប្រសិនបើ (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	ប្រសិនបើ ((*fnamep)[0] == '~'
#ប្រសិនបើ !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    ប្រសិនបើ (*fnamep == NULL)
		ត្រឡប់ -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	សម្រាប់ (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    ប្រសិនបើ (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		bha_nga;
	}

	// FullName_save() is slow, don't use it when not needed.
	ប្រសិនបើ (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ប្រសិនបើ (*fnamep == NULL)
		ត្រឡប់ -1;
	}

#ifdef MSWIN
# ប្រសិនបើ _WIN32_WINNT >= 0x0500
	ប្រសិនបើ (vim_strchr(*fnamep, '~') != NULL)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, NULL);
	    WCHAR buf[_MAX_PATH];

	    ប្រសិនបើ (wfname != NULL)
	    {
		ប្រសិនបើ (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, NULL);

		    ប្រសិនបើ (q != NULL)
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
	ប្រសិនបើ (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ប្រសិនបើ (*fnamep == NULL)
		ត្រឡប់ -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    ខណៈពេល (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	ប្រសិនបើ (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    continue;
	}
	pbuf = NULL;
	// Need full path first (use expand_env() to remove a "~/")
	ប្រសិនបើ (!has_fullname && !has_homerelative)
	{
	    ប្រសិនបើ (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    បើមិនដូច្នេះ
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	បើមិនដូច្នេះ
	    p = *fnamep;

	has_fullname = 0;

	ប្រសិនបើ (p != NULL)
	{
	    size_t  dirnamelen = 0;

	    ប្រសិនបើ (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		ប្រសិនបើ (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    ប្រសិនបើ (s != NULL)
		    {
			dirnamelen = home_replace(NULL, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		ប្រសិនបើ (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		ប្រសិនបើ (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    ប្រសិនបើ (vim_ispathsep(*p))
		    {
			ខណៈពេល (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			ប្រសិនបើ (pbuf != NULL)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = NULL;
			}
		    }
		}
	    }
	    បើមិនដូច្នេះ
	    {
		dirnamelen = home_replace(NULL, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		ប្រសិនបើ (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    ប្រសិនបើ (s != NULL)
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
    ខណៈពេល (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	ខណៈពេល (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	ប្រសិនបើ (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	ប្រសិនបើ (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    ប្រសិនបើ (p == NULL)
		ត្រឡប់ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	បើមិនដូច្នេះ
	{
	    ខណៈពេល (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    ប្រសិនបើ (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    ប្រសិនបើ (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	ប្រសិនបើ (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    ប្រសិនបើ (p == NULL)
		ត្រឡប់ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	ប្រសិនបើ (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    ប្រសិនបើ (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		ត្រឡប់ -1;
	}
	បើមិនដូច្នេះ
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    ប្រសិនបើ (get_short_pathname(fnamep, bufp, &l) == FAIL)
		ត្រឡប់ -1;

	    ប្រសិនបើ (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		ប្រសិនបើ (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    ត្រឡប់ -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    ប្រសិនបើ (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    ខណៈពេល (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	ប្រសិនបើ (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	បើមិនដូច្នេះ
	    s = *fnamep + *fnamelen - 1;
	សម្រាប់ ( ; s > tail; --s)
	    ប្រសិនបើ (s[0] == '.')
		bha_nga;
	ប្រសិនបើ (src[*usedlen + 1] == 'e')		// :e
	{
	    ប្រសិនបើ (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		សម្រាប់ ( ; s > *fnamep; --s)
		    ប្រសិនបើ (s[0] == ';')
			bha_nga;
		ប្រសិនបើ (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (*fnamep <= tail)
		*fnamelen = 0;
	}
	បើមិនដូច្នេះ				// :r
	{
	    char_u *limit = *fnamep;

	    ប្រសិនបើ (limit < tail)
		limit = tail;
	    ប្រសិនបើ (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    ប្រសិនបើ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	ចំនួនគត់	    sep;
	char_u	    *flags;
	ចំនួនគត់	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	ប្រសិនបើ (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	ប្រសិនបើ (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    ប្រសិនបើ (p != NULL)
	    {
		pat = vim_strnsave(s, p - s);
		ប្រសិនបើ (pat != NULL)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    ប្រសិនបើ (p != NULL)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			ប្រសិនបើ (sub != NULL && str != NULL)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, NULL, flags, &slen);
			    ប្រសិនបើ (s != NULL)
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
	    ប្រសិនបើ (didit)
		goto repeat;
	}
    }

    ប្រសិនបើ (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	ប្រសិនបើ (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	ប្រសិនបើ (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	ប្រសិនបើ (p == NULL)
	    ត្រឡប់ -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    ត្រឡប់ valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    static void
shorten_dir_len(char_u *str, ចំនួនគត់ trim_len)
{
    char_u	*tail, *s, *d;
    ចំនួនគត់		skip = FALSE;
    ចំនួនគត់		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    សម្រាប់ (s = str; ; ++s)
    {
	ប្រសិនបើ (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    ប្រសិនបើ (*s == NUL)
		bha_nga;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (!skip)
	{
	    *d++ = *s;			// copy next char
	    ប្រសិនបើ (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		ប្រសិនបើ (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    ប្រសិនបើ (has_mbyte)
	    {
		ចំនួនគត់ l = mb_ptr2len(s);

		ខណៈពេល (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    void
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    ចំនួនគត់
file_is_readable(char_u *fname)
{
    ចំនួនគត់		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    ប្រសិនបើ (*fname == NUL || mch_isdir_or_missing(fname))
	ត្រឡប់ FALSE;
    ប្រសិនបើ ((fd = mch_open((var_na *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	ត្រឡប់ TRUE;
    }
    ត្រឡប់ FALSE;
}

#ប្រសិនបើ defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    void
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ប្រសិនបើ (check_secure())
	ត្រឡប់;

    ប្រសិនបើ (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	ប្រសិនបើ (in_vim9script())
	    (void) check_for_string_arg(argvars, 0);
	ត្រឡប់;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    ប្រសិនបើ (cwd != NULL)
    {
	ប្រសិនបើ (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	ប្រសិនបើ (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	បើមិនដូច្នេះ
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "scope", s);
	    ត្រឡប់;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (curwin->w_localdir != NULL)
	scope = CDSCOPE_WINDOW;
    បើមិនដូច្នេះ ប្រសិនបើ (curtab->tp_localdir != NULL)
	scope = CDSCOPE_TABPAGE;

    ប្រសិនបើ (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    void
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_number = -1;
    ប្រសិនបើ (check_restricted() || check_secure())
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    name = tv_get_string(&argvars[0]);
    ប្រសិនបើ (name == NULL || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	ត្រឡប់;
    }

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    បើមិនដូច្នេះ
	flags = (char_u *)"";

    ប្រសិនបើ (*flags == NUL)
	// delete a file
	rettv->vval.v_number = mch_remove(name) == 0 ? 0 : -1;
    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_number = mch_rmdir(name) == 0 ? 0 : -1;
    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_number = delete_recursive(name);
    បើមិនដូច្នេះ
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    void
f_executable(typval_T *argvars, typval_T *rettv)
{
    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_number = mch_can_exe(tv_get_string(&argvars[0]), NULL, TRUE);
}

/*
 * "exepath()" function
 */
    void
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = NULL;

    ប្រសិនបើ (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;
    (void)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    void
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;
    rettv->vval.v_number = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    void
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;
    rettv->vval.v_number = filewritable(tv_get_string(&argvars[0]));
}

    static void
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    ចំនួនគត់		find_what)
{
    char_u	*fname;
    char_u	*fresult = NULL;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    ចំនួនគត់		count = 1;
    ចំនួនគត់		first = TRUE;
    ចំនួនគត់		error = FALSE;

    rettv->vval.v_string = NULL;
    rettv->v_type = VAR_STRING;
    ប្រសិនបើ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    fname = tv_get_string(&argvars[0]);

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	ប្រសិនបើ (p == NULL)
	    error = TRUE;
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (*p != NUL)
		path = p;

	    ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
		count = (ចំនួនគត់)tv_get_number_chk(&argvars[2], &error);
	}
    }

    ប្រសិនបើ (count < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    ប្រសិនបើ (*fname != NUL && !error)
    {
	char_u	*file_to_find = NULL;
	var_na	*search_ctx = NULL;

	do
	{
	    ប្រសិនបើ (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : NULL,
					       first ? (ចំនួនគត់)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &search_ctx);
	    first = FALSE;

	    ប្រសិនបើ (fresult != NULL && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} ខណៈពេល ((rettv->v_type == VAR_LIST || --count > 0) && fresult != NULL);

	vim_free(file_to_find);
	vim_findfile_cleanup(search_ctx);
    }

    ប្រសិនបើ (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    void
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    void
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    void
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = NULL;
    char_u	buf[NUMBUFLEN];

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    ប្រសិនបើ (mods == NULL || fname == NULL)
	fname = NULL;
    បើមិនដូច្នេះ
    {
	len = STRLEN(fname);
	ប្រសិនបើ (mods != NULL && *mods != NUL)
	    (void)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    ប្រសិនបើ (fname == NULL)
	rettv->vval.v_string = NULL;
    បើមិនដូច្នេះ
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
    void
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = NULL;
    tabpage_T	*tp = NULL;
    char_u	*cwd;
    ចំនួនគត់		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	ត្រឡប់;

    ប្រសិនបើ (argvars[0].v_type == VAR_NUMBER
	    && argvars[0].vval.v_number == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    បើមិនដូច្នេះ
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    ប្រសិនបើ (wp != NULL && wp->w_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    បើមិនដូច្នេះ ប្រសិនបើ (tp != NULL && tp->tp_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    បើមិនដូច្នេះ ប្រសិនបើ (wp != NULL || tp != NULL || global)
    {
	ប្រសិនបើ (globaldir != NULL && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	បើមិនដូច្នេះ
	{
	    cwd = alloc(MAXPATHL);
	    ប្រសិនបើ (cwd != NULL)
	    {
		ប្រសិនបើ (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    ប្រសិនបើ (rettv->vval.v_string != NULL)
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
    ចំនួនគត់		    i;

    សម្រាប់ (i = 0; i < 9; i++)
    {
	ប្រសិនបើ (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	បើមិនដូច្នេះ
	    perm[i] = '-';
    }
    ត្រឡប់ perm;
}

/*
 * "getfperm({fname})" function
 */
    void
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = NULL;
    char_u	permbuf[] = "---------";

    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ប្រសិនបើ (mch_stat((var_na *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    void
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    fname = tv_get_string(&argvars[0]);
    ប្រសិនបើ (mch_stat((var_na *)fname, &st) >= 0)
    {
	ប្រសិនបើ (S_ISDIR(st.st_mode))
	    rettv->vval.v_number = 0;
	បើមិនដូច្នេះ
	{
	    rettv->vval.v_number = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    ប្រសិនបើ ((off_T)rettv->vval.v_number != (off_T)st.st_size)
		rettv->vval.v_number = -2;
	}
    }
    បើមិនដូច្នេះ
	  rettv->vval.v_number = -1;
}

/*
 * "getftime({fname})" function
 */
    void
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    fname = tv_get_string(&argvars[0]);
    ប្រសិនបើ (mch_stat((var_na *)fname, &st) >= 0)
	rettv->vval.v_number = (varnumber_T)st.st_mtime;
    បើមិនដូច្នេះ
	rettv->vval.v_number = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    var_na    *t;

    ប្រសិនបើ (S_ISREG(st->st_mode))
	t = "file";
    បើមិនដូច្នេះ ប្រសិនបើ (S_ISDIR(st->st_mode))
	t = "dir";
    បើមិនដូច្នេះ ប្រសិនបើ (S_ISLNK(st->st_mode))
	t = "link";
    បើមិនដូច្នេះ ប្រសិនបើ (S_ISBLK(st->st_mode))
	t = "bdev";
    បើមិនដូច្នេះ ប្រសិនបើ (S_ISCHR(st->st_mode))
	t = "cdev";
    បើមិនដូច្នេះ ប្រសិនបើ (S_ISFIFO(st->st_mode))
	t = "fifo";
    បើមិនដូច្នេះ ប្រសិនបើ (S_ISSOCK(st->st_mode))
	t = "socket";
    បើមិនដូច្នេះ
	t = "other";
    ត្រឡប់ (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    void
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = NULL;

    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ប្រសិនបើ (mch_lstat((var_na *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    void
f_glob(typval_T *argvars, typval_T *rettv)
{
    ចំនួនគត់		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    ចំនួនគត់		error = FALSE;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	ត្រឡប់;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ប្រសិនបើ (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ប្រសិនបើ (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, NULL);
	    ប្រសិនបើ (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    ប្រសិនបើ (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	ប្រសិនបើ (p_wic)
	    options += WILD_ICASE;
	ប្រសិនបើ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     NULL, options, WILD_ALL);
	បើមិនដូច្នេះ ប្រសិនបើ (rettv_list_alloc(rettv) == OK)
	{
	  ចំនួនគត់ i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						NULL, options, WILD_ALL_KEEP);
	  សម្រាប់ (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    បើមិនដូច្នេះ
	rettv->vval.v_string = NULL;
}

/*
 * "glob2regpat()" function
 */
    void
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == NULL)
			 ? NULL : file_pat_to_reg_pat(pat, NULL, NULL, FALSE);
}

/*
 * "globpath()" function
 */
    void
f_globpath(typval_T *argvars, typval_T *rettv)
{
    ចំនួនគត់		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    ចំនួនគត់		error = FALSE;
    garray_T	ga;
    ចំនួនគត់		i;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	ត្រឡប់;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ប្រសិនបើ (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	ប្រសិនបើ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    ប្រសិនបើ (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, NULL);
	    ប្រសិនបើ (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    ប្រសិនបើ (file != NULL && !error)
    {
	ga_init2(&ga, parimaa_na(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	ប្រសិនបើ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	បើមិនដូច្នេះ ប្រសិនបើ (rettv_list_alloc(rettv) == OK)
	    សម្រាប់ (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    បើមិនដូច្នេះ
	rettv->vval.v_string = NULL;
}

/*
 * "isdirectory()" function
 */
    void
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    rettv->vval.v_number = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    void
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    rettv->vval.v_number = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    static ចំនួនគត់
mkdir_recurse(char_u *dir, ចំនួនគត់ prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    ចំនួនគត់		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    ប្រសិនបើ (p <= get_past_head(dir))
	ត្រឡប់ OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    ប្រសិនបើ (updir == NULL)
	ត្រឡប់ FAIL;
    ប្រសិនបើ (mch_isdir(updir))
	r = OK;
    បើមិនដូច្នេះ ប្រសិនបើ (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	ប្រសិនបើ (r == OK && created != NULL && *created == NULL)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    ត្រឡប់ r;
}

/*
 * "mkdir()" function
 */
    void
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    ចំនួនគត់		prot = 0755;
    ចំនួនគត់		defer = FALSE;
    ចំនួនគត់		defer_recurse = FALSE;
    char_u	*created = NULL;

    rettv->vval.v_number = FAIL;
    ប្រសិនបើ (check_restricted() || check_secure())
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    dir = tv_get_string_buf(&argvars[0], buf);
    ប្រសិនបើ (*dir == NUL)
	ត្រឡប់;

    ប្រសិនបើ (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (ចំនួនគត់)tv_get_number_chk(&argvars[2], NULL);
	    ប្រសិនបើ (prot == -1)
		ត្រឡប់;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != NULL;
	defer_recurse = vim_strchr(arg2, 'R') != NULL;
	ប្រសិនបើ ((defer || defer_recurse) && !can_add_defer())
	    ត្រឡប់;

	ប្រសិនបើ (vim_strchr(arg2, 'p') != NULL)
	{
	    ប្រសិនបើ (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_number = OK;
		ត្រឡប់;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : NULL);
	}
    }
    rettv->vval.v_number = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    ប្រសិនបើ (rettv->vval.v_number == OK
				&& created == NULL && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    ប្រសិនបើ (created != NULL)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	ប្រសិនបើ (tv[0].vval.v_string == NULL || tv[1].vval.v_string == NULL
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
    void
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ចំនួនគត់		trim_len = 1;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (ចំនួនគត់)tv_get_number(&argvars[1]);
	ប្រសិនបើ (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    ប្រសិនបើ (p == NULL)
	rettv->vval.v_string = NULL;
    បើមិនដូច្នេះ
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	ប្រសិនបើ (p != NULL)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    static ចំនួនគត់
checkitem_common(void *context, char_u *name, dict_T *dict)
{
    typval_T	*expr = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    ចំនួនគត់		retval = 0;
    ចំនួនគត់		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    ប្រសិនបើ (name != NULL)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    បើមិនដូច្នេះ
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    ប្រសិនបើ (eval_expr_typval(expr, FALSE, argv, 1, NULL, &rettv) == FAIL)
	goto theend;

    // We want to use -1, but also true/false should be allowed.
    ប្រសិនបើ (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_NUMBER;
	rettv.vval.v_number = rettv.vval.v_number == VVAL_TRUE;
    }
    retval = tv_get_number_chk(&rettv, &error);
    ប្រសិនបើ (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    ប្រសិនបើ (name != NULL)
	set_vim_var_string(VV_VAL, NULL, 0);
    បើមិនដូច្នេះ
	set_vim_var_dict(VV_VAL, NULL);
    restore_vimvar(VV_VAL, &save_val);
    ត្រឡប់ retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    static ចំនួនគត់
readdir_checkitem(void *context, void *item)
{
    char_u	*name = (char_u *)item;

    ត្រឡប់ checkitem_common(context, name, NULL);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    static ចំនួនគត់
readdirex_dict_arg(typval_T *argvars, ចំនួនគត់ *cmp)
{
    char_u     *compare;

    ប្រសិនបើ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (dict_has_key(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    បើមិនដូច្នេះ
    {
	semsg(_(e_dictionary_key_str_required), "sort");
	ត្រឡប់ FAIL;
    }

    ប្រសិនបើ (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    ត្រឡប់ OK;
}

/*
 * "readdir()" function
 */
    void
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    ចំនួនគត់		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    ចំនួនគត់		i;
    ចំនួនគត់		sort = READDIR_SORT_BYTE;

    ប្រសិនបើ (rettv_list_alloc(rettv) == FAIL)
	ត្រឡប់;
    ប្រសិនបើ (check_secure())
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ត្រឡប់;

    ret = readdir_core(&ga, path, FALSE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdir_checkitem, sort);
    ប្រសិនបើ (ret == OK)
    {
	សម្រាប់ (i = 0; i < ga.ga_len; i++)
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
    static ចំនួនគត់
readdirex_checkitem(void *context, void *item)
{
    dict_T	*dict = (dict_T*)item;

    ត្រឡប់ checkitem_common(context, NULL, dict);
}

/*
 * "readdirex()" function
 */
    void
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    ចំនួនគត់		ret;
    char_u	*path;
    garray_T	ga;
    ចំនួនគត់		i;
    ចំនួនគត់		sort = READDIR_SORT_BYTE;

    ប្រសិនបើ (rettv_list_alloc(rettv) == FAIL)
	ត្រឡប់;
    ប្រសិនបើ (check_secure())
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ត្រឡប់;

    ret = readdir_core(&ga, path, TRUE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdirex_checkitem, sort);
    ប្រសិនបើ (ret == OK)
    {
	សម្រាប់ (i = 0; i < ga.ga_len; i++)
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
    static void
read_file_or_blob(typval_T *argvars, typval_T *rettv, ចំនួនគត់ always_blob)
{
    ចំនួនគត់		binary = FALSE;
    ចំនួនគត់		blob = always_blob;
    ចំនួនគត់		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    ចំនួនគត់		io_size = parimaa_na(buf);
    ចំនួនគត់		readlen;		// size of last fread()
    char_u	*prev	 = NULL;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ប្រសិនបើ (always_blob)
	{
	    offset = (off_T)tv_get_number(&argvars[1]);
	    ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_number(&argvars[2]);
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		binary = TRUE;
	    ប្រសិនបើ (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_number(&argvars[2]);
	}
    }

    ប្រសិនបើ ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	ត្រឡប់;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    ប្រសិនបើ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ត្រឡប់;
    }
    ប្រសិនបើ (*fname == NUL || (fd = mch_fopen((var_na *)fname, READBIN)) == NULL)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ត្រឡប់;
    }

    ប្រសិនបើ (blob)
    {
	ប្រសិនបើ (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	ត្រឡប់;
    }

    ខណៈពេល (cnt < maxline || maxline < 0)
    {
	readlen = (ចំនួនគត់)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	សម្រាប់ (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || binary));
		++p)
	{
	    ប្រសិនបើ (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= NULL;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		ប្រសិនបើ (readlen > 0 && !binary)
		{
		    ខណៈពេល (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    ប្រសិនបើ (len == 0)
			ខណៈពេល (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		ប្រសិនបើ (prevlen == 0)
		    s = vim_strnsave(start, len);
		បើមិនដូច្នេះ
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    ប្រសិនបើ ((s = vim_realloc(prev, prevlen + len + 1)) != NULL)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = NULL; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		ប្រសិនបើ (s == NULL)
		{
		    do_outofmem_msg((long_u) prevlen + len + 1);
		    failed = TRUE;
		    bha_nga;
		}

		ប្រសិនបើ ((li = listitem_alloc()) == NULL)
		{
		    vim_free(s);
		    failed = TRUE;
		    bha_nga;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		ប្រសិនបើ ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    bha_nga;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    បើមិនដូច្នេះ ប្រសិនបើ (*p == 0xbf && enc_utf8 && !binary)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		ប្រសិនបើ (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    ប្រសិនបើ (start == dest)
			start = p + 1;
		    បើមិនដូច្នេះ
		    {
			// have to shuffle buf to close gap
			ចំនួនគត់ adjust_prevlen = 0;

			ប្រសិនបើ (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (ចំនួនគត់)(buf - dest);
			    dest = buf;
			}
			ប្រសិនបើ (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	ប្រសិនបើ (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    bha_nga;
	ប្រសិនបើ (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    ប្រសិនបើ (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		ប្រសិនបើ (prevsize == 0)
		    prevsize = (long)(p - start);
		បើមិនដូច្នេះ
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		ប្រសិនបើ (newprev == NULL)
		{
		    do_outofmem_msg((long_u)prevsize);
		    failed = TRUE;
		    bha_nga;
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
    ប្រសិនបើ (!failed && maxline < 0)
	ខណៈពេល (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    ប្រសិនបើ (failed)
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
    void
f_readblob(typval_T *argvars, typval_T *rettv)
{
    ប្រសិនបើ (check_secure())
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    void
f_readfile(typval_T *argvars, typval_T *rettv)
{
    ប្រសិនបើ (check_secure())
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    void
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = NULL;
# endif

    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = NULL;

	v = mch_resolve_path(p, TRUE);
	ប្រសិនបើ (v != NULL)
	    rettv->vval.v_string = v;
	បើមិនដូច្នេះ
	    rettv->vval.v_string = vim_strsave(p);
    }
# បើមិនដូច្នេះ
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	ចំនួនគត់	len;
	char_u	*remain = NULL;
	char_u	*q;
	ចំនួនគត់	is_relative_to_current = FALSE;
	ចំនួនគត់	has_trailing_pathsep = FALSE;
	ចំនួនគត់	limit = 100;

	p = vim_strsave(p);
	ប្រសិនបើ (p == NULL)
	    goto fail;
	ប្រសិនបើ (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	ប្រសិនបើ (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	ប្រសិនបើ (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	ប្រសិនបើ (buf == NULL)
	{
	    vim_free(p);
	    vim_free(remain);
	    goto fail;
	}

	សម្រាប់ (;;)
	{
	    សម្រាប់ (;;)
	    {
		len = readlink((var_na *)p, (var_na *)buf, MAXPATHL);
		ប្រសិនបើ (len <= 0)
		    bha_nga;
		buf[len] = NUL;

		ប្រសិនបើ (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = NULL;
		    goto fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		ប្រសិនបើ (remain == NULL && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		ប្រសិនបើ (*q != NUL)
		{
		    ប្រសិនបើ (remain == NULL)
			remain = vim_strsave(q - 1);
		    បើមិនដូច្នេះ
		    {
			cpy = concat_str(q - 1, remain);
			ប្រសិនបើ (cpy != NULL)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		ប្រសិនបើ (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		ប្រសិនបើ (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    ប្រសិនបើ (cpy != NULL)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		បើមិនដូច្នេះ
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    ប្រសិនបើ (remain == NULL)
		bha_nga;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    ប្រសិនបើ (cpy != NULL)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    ប្រសិនបើ (*q != NUL)
		STRMOVE(remain, q - 1);
	    បើមិនដូច្នេះ
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	ប្រសិនបើ (!vim_ispathsep(*p))
	{
	    ប្រសិនបើ (is_relative_to_current
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
		ប្រសិនបើ (cpy != NULL)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		ខណៈពេល (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		ប្រសិនបើ (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	ប្រសិនបើ (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    ប្រសិនបើ (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  បើមិនដូច្នេះ
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
    void
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    static ចំនួនគត់	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    do
    {
	ប្រសិនបើ (x == 'Z')
	    x = '0';
	បើមិនដូច្នេះ ប្រសិនបើ (x == '9')
	    x = 'A';
	បើមិនដូច្នេះ
	    ++x;
    } ខណៈពេល (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    void
f_writefile(typval_T *argvars, typval_T *rettv)
{
    ចំនួនគត់		binary = FALSE;
    ចំនួនគត់		append = FALSE;
    ចំនួនគត់		defer = FALSE;
# ifdef HAVE_FSYNC
    ចំនួនគត់		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    ចំនួនគត់		ret = 0;
    listitem_T	*li;
    list_T	*list = NULL;
    blob_T	*blob = NULL;

    rettv->vval.v_number = -1;
    ប្រសិនបើ (check_secure())
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ត្រឡប់;

    ប្រសិនបើ (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	ប្រសិនបើ (list == NULL)
	    ត្រឡប់;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, li)
	    ប្រសិនបើ (tv_get_string_chk(&li->li_tv) == NULL)
		ត្រឡប់;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	ប្រសិនបើ (blob == NULL)
	    ត្រឡប់;
    }
    បើមិនដូច្នេះ
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	ត្រឡប់;
    }

    ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	ប្រសិនបើ (arg2 == NULL)
	    ត្រឡប់;
	ប្រសិនបើ (vim_strchr(arg2, 'b') != NULL)
	    binary = TRUE;
	ប្រសិនបើ (vim_strchr(arg2, 'a') != NULL)
	    append = TRUE;
	ប្រសិនបើ (vim_strchr(arg2, 'D') != NULL)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	ប្រសិនបើ (vim_strchr(arg2, 's') != NULL)
	    do_fsync = TRUE;
	បើមិនដូច្នេះ ប្រសិនបើ (vim_strchr(arg2, 'S') != NULL)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    ប្រសិនបើ (fname == NULL)
	ត្រឡប់;

    ប្រសិនបើ (defer && !can_add_defer())
	ត្រឡប់;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    ប្រសិនបើ (*fname == NUL || (fd = mch_fopen((var_na *)fname,
				      append ? APPENDBIN : WRITEBIN)) == NULL)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    ប្រសិនបើ (tv.vval.v_string == NULL
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(void)mch_remove(fname);
	    }
	}

	ប្រសិនបើ (ret == 0)
	{
	    ប្រសិនបើ (blob)
	    {
		ប្រសិនបើ (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    បើមិនដូច្នេះ
	    {
		ប្រសិនបើ (write_list(fd, list, binary) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    ប្រសិនបើ (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_number = ret;
}

#endif // FEAT_EVAL

#ប្រសិនបើ defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    ចំនួនគត់		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    static char_u	*last_dir = NULL;    // last used directory
    char_u		*tofree = NULL;
    ចំនួនគត់			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    ប្រសិនបើ (title == NULL || *title == NUL)
    {
	ប្រសិនបើ (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	បើមិនដូច្នេះ ប្រសិនបើ (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	បើមិនដូច្នេះ
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    ប្រសិនបើ ((initdir == NULL || *initdir == NUL) && dflt != NULL && *dflt != NUL)
    {
	ប្រសិនបើ (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = NULL;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    ប្រសិនបើ (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    ប្រសិនបើ (initdir == NULL || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	ប្រសិនបើ (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	បើមិនដូច្នេះ ប្រសិនបើ (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != NULL && buf->b_ffname != NULL)
	{
	    ប្រសិនបើ (dflt == NULL || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    ប្រសិនបើ (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	បើមិនដូច្នេះ ប្រសិនបើ (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    ប្រសិនបើ (gui.in_use)		// when this changes, also adjust f_has()!
    {
	ប្រសិនបើ (filter == NULL
#  ifdef FEAT_EVAL
		&& (filter = get_var_value((char_u *)"b:browsefilter")) == NULL
		&& (filter = get_var_value((char_u *)"g:browsefilter")) == NULL
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	ប្រសិនបើ (flags & BROWSE_DIR)
	{
#  ប្រសិនបើ defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  បើមិនដូច្នេះ
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  ប្រសិនបើ !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    ប្រសិនបើ (fname != NULL && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		ប្រសិនបើ (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	បើមិនដូច្នេះ
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    បើមិនដូច្នេះ
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = NULL;
    }

    // keep the directory for next time
    ប្រសិនបើ (fname != NULL)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	ប្រសិនបើ (last_dir != NULL && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    ប្រសិនបើ (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		ប្រសិនបើ (last_dir != NULL)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    ត្រឡប់ fname;
}
#endif

#ប្រសិនបើ defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    void
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    ចំនួនគត់		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ចំនួនគត់		error = FALSE;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ត្រឡប់;

    save = (ចំនួនគត់)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    ប្រសិនបើ (error || title == NULL || initdir == NULL || defname == NULL)
	rettv->vval.v_string = NULL;
    បើមិនដូច្នេះ
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, NULL, initdir, NULL, curbuf);
# បើមិនដូច្នេះ
    rettv->vval.v_string = NULL;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    void
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    ប្រសិនបើ (title == NULL || initdir == NULL)
	rettv->vval.v_string = NULL;
    បើមិនដូច្នេះ
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, NULL, NULL, initdir, NULL, curbuf);
# បើមិនដូច្នេះ
    rettv->vval.v_string = NULL;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    void
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_number = FALSE;

    ប្រសិនបើ (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	ត្រឡប់;

    from = tv_get_string(&argvars[0]);

    ប្រសិនបើ (mch_lstat((var_na *)from, &st) >= 0
	&& (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)))
	rettv->vval.v_number = vim_copyfile(
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
    ចំនួនគត់		dstlen,	// maximum length of the result
    ចំនួនគត់		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    ប្រសិនបើ (src == NULL)
    {
	*dst = NUL;
	ត្រឡប់ 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    ប្រសិនបើ (buf != NULL && buf->b_help)
	ត្រឡប់ vim_snprintf_safelen((var_na *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    ប្រសិនបើ (homedir != NULL)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#បើមិនដូច្នេះ
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    ប្រសិនបើ (homedir_env == NULL)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    ប្រសិនបើ (homedir_env != NULL && *homedir_env == NUL)
	homedir_env = NULL;

    ប្រសិនបើ (homedir_env != NULL && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = NULL;

	flen = STRLEN(homedir_env);
	(void)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	ប្រសិនបើ (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    ប្រសិនបើ (homedir_env != NULL)
	envlen = STRLEN(homedir_env);

    ប្រសិនបើ (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    ខណៈពេល (*src && dstlen > 0)
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
	សម្រាប់ (;;)
	{
	    ប្រសិនបើ (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		ប្រសិនបើ (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		bha_nga;
	    }
	    ប្រសិនបើ (p == homedir_env)
		bha_nga;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	ខណៈពេល (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	ខណៈពេល ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    ប្រសិនបើ (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    ត្រឡប់ (size_t)(dst - dst_start);
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
    ប្រសិនបើ (src != NULL)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    ប្រសិនបើ (dst != NULL)
	home_replace(buf, src, dst, len, TRUE);
    ត្រឡប់ dst;
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
    ចំនួនគត់
fullpathcmp(
    char_u *s1,
    char_u *s2,
    ចំនួនគត់	    checkname,		// when both don't exist, check file names
    ចំនួនគត់	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    ចំនួនគត់		    r1, r2;

    ប្រសិនបើ (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    បើមិនដូច្នេះ
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((var_na *)exp1, &st1);
    r2 = mch_stat((var_na *)s2, &st2);
    ប្រសិនបើ (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	ប្រសិនបើ (checkname)
	{
	    ប្រសិនបើ (fnamecmp(exp1, s2) == 0)
		ត្រឡប់ FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    ប្រសិនបើ (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		ត្រឡប់ FPC_SAMEX;
	}
	ត្រឡប់ FPC_NOTX;
    }
    ប្រសិនបើ (r1 != 0 || r2 != 0)
	ត្រឡប់ FPC_DIFFX;
    ប្រសិនបើ (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	ត្រឡប់ FPC_SAME;
    ត្រឡប់ FPC_DIFF;
#បើមិនដូច្នេះ
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    ចំនួនគត់	    retval = FPC_DIFF;
    ចំនួនគត់	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    ប្រសិនបើ ((exp1 = alloc(MAXPATHL * 3)) != NULL)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	ប្រសិនបើ (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	បើមិនដូច្នេះ
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	ប្រសិនបើ (r1 != OK && r2 != OK)
	{
	    ប្រសិនបើ (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    បើមិនដូច្នេះ
		retval = FPC_NOTX;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	បើមិនដូច្នេះ ប្រសិនបើ (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	បើមិនដូច្នេះ
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    ត្រឡប់ retval;
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

    ប្រសិនបើ (fname == NULL)
	ត្រឡប់ (char_u *)"";
    សម្រាប់ (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	ប្រសិនបើ (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    ត្រឡប់ p1;
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
    ខណៈពេល (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    ត្រឡប់ t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    ខណៈពេល (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    ប្រសិនបើ (*fname)
	++fname;
    ត្រឡប់ fname;
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

#ប្រសិនបើ defined(MSWIN)
    // may skip "c:"
    ប្រសិនបើ (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    បើមិនដូច្នេះ
	retval = path;
#បើមិនដូច្នេះ
# ប្រសិនបើ defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    ប្រសិនបើ (retval == NULL)
	retval = path;
# បើមិនដូច្នេះ	// Unix
    retval = path;
# endif
#endif

    ខណៈពេល (vim_ispathsep(*retval))
	++retval;

    ត្រឡប់ retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    ចំនួនគត់
vim_ispathsep(ចំនួនគត់ c)
{
#ifdef UNIX
    ត្រឡប់ (c == '/');	    // UNIX has ':' inside file names
#បើមិនដូច្នេះ
# ifdef BACKSLASH_IN_FILENAME
    ត្រឡប់ (c == ':' || c == '/' || c == '\\');
# បើមិនដូច្នេះ
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    ត្រឡប់ (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  បើមិនដូច្នេះ
    ត្រឡប់ (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    ចំនួនគត់
vim_ispathsep_nocolon(ចំនួនគត់ c)
{
    ត្រឡប់ vim_ispathsep(c)
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
    ចំនួនគត់
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    ចំនួនគត់		c;
    ចំនួនគត់		retval;

    p = gettail_sep(fname);
    ប្រសិនបើ (p == fname)
	ត្រឡប់ TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    ត្រឡប់ retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    ចំនួនគត់
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    ត្រឡប់ vim_fnamencmp(x, y, MAXPATHL);
#បើមិនដូច្នេះ
    ប្រសិនបើ (p_fic)
	ត្រឡប់ MB_STRICMP(x, y);
    ត្រឡប់ STRCMP(x, y);
#endif
}

    ចំនួនគត់
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    ចំនួនគត់		cx = NUL;
    ចំនួនគត់		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    សម្រាប់ (ចំនួនគត់ swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	ប្រសិនបើ ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    ប្រសិនបើ (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		bha_nga;
	    }
	    បើមិនដូច្នេះ // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    ខណៈពេល (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	ប្រសិនបើ (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    bha_nga;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    ប្រសិនបើ (len == 0)
	ត្រឡប់ 0;
    ត្រឡប់ (cx - cy);
#បើមិនដូច្នេះ
    ប្រសិនបើ (p_fic)
	ត្រឡប់ MB_STRNICMP(x, y, len);
    ត្រឡប់ STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, ចំនួនគត់ sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? parimaa_na(PATHSEP) : 0) + fname2len + 1);
    ប្រសិនបើ (ret->string == NULL)
	ret->length = 0;
    បើមិនដូច្នេះ
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	ប្រសិនបើ (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += parimaa_na(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    ត្រឡប់ ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    void
add_pathsep(char_u *p)
{
    size_t  plen;

    ប្រសិនបើ (*p == NUL)
	ត្រឡប់;

    plen = STRLEN(p);
    ប្រសិនបើ (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    ចំនួនគត់		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = NULL;

    ប្រសិនបើ (fname == NULL)
	ត្រឡប់ NULL;

    buf = alloc(MAXPATHL);
    ប្រសិនបើ (buf == NULL)
	ត្រឡប់ NULL;

    ប្រសិនបើ (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    បើមិនដូច្នេះ
	new_fname = vim_strsave(fname);
    vim_free(buf);
    ត្រឡប់ new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    ចំនួនគត់
vim_fexists(char_u *fname)
{
    stat_T st;

    ប្រសិនបើ (mch_stat((var_na *)fname, &st))
	ត្រឡប់ FALSE;
    ត្រឡប់ TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    ចំនួនគត់
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    ចំនួនគត់		  *num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ចំនួនគត់		   flags)	// EW_DIR, etc.
{
    ចំនួនគត់		ret = FAIL;
    char_u	*eval_pat = NULL;
    char_u	*exp_pat = *pat;
    var_na	*ignored_msg;
    size_t	usedlen;
    ចំនួនគត់		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    ចំនួនគត់		star_follows = FALSE;

    ប្រសិនបើ (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       NULL, &ignored_msg, NULL, TRUE);
	--emsg_off;
	ប្រសិនបើ (eval_pat != NULL)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    ប្រសិនបើ (exp_pat != NULL)
	ret = expand_wildcards(1, &exp_pat, num_file, file, flags);

    ប្រសិនបើ (eval_pat != NULL)
    {
	ប្រសិនបើ (*num_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    ប្រសិនបើ (*file != NULL)
	    {
		**file = eval_pat;
		eval_pat = NULL;
		*num_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    ត្រឡប់ ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    ចំនួនគត់
expand_wildcards(
    ចំនួនគត់		   num_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    ចំនួនគត់		  *num_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    ចំនួនគត់		   flags)	// EW_DIR, etc.
{
    ចំនួនគត់		retval;
    ចំនួនគត់		i, j;
    char_u	*p;
    ចំនួនគត់		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(num_pat, pat, num_files, files, flags);

    // When keeping all matches, return here
    ប្រសិនបើ ((flags & EW_KEEPALL) || retval == FAIL)
	ត្រឡប់ retval;

    /*
     * Remove names that match 'wildignore'.
     */
    ប្រសិនបើ (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	សម្រាប់ (i = 0; i < *num_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    ប្រសិនបើ (ffname == NULL)		// out of memory
		bha_nga;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    ប្រសិនបើ (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		សម្រាប់ (j = i; j + 1 < *num_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*num_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	ប្រសិនបើ (*num_files == 0)
	{
	    VIM_CLEAR(*files);
	    ត្រឡប់ FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    ប្រសិនបើ (*num_files > 1 && !got_int)
    {
	non_suf_match = 0;
	សម្រាប់ (i = 0; i < *num_files; ++i)
	{
	    ប្រសិនបើ (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		សម្រាប់ (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    ត្រឡប់ retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    ចំនួនគត់
match_suffix(char_u *fname)
{
    ចំនួនគត់		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (ចំនួនគត់)STRLEN(fname);
    setsuflen = 0;
    សម្រាប់ (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	ប្រសិនបើ (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    ប្រសិនបើ (vim_strchr(tail, '.') == NULL)
	    {
		setsuflen = 1;
		bha_nga;
	    }
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		bha_nga;
	    setsuflen = 0;
	}
    }
    ត្រឡប់ (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    static ចំនួនគត់
vim_backtick(char_u *p)
{
    ត្រឡប់ (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    static ចំនួនគត់
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    ចំនួនគត់		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    ចំនួនគត់		cnt = 0;
    ចំនួនគត់		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    ប្រសិនបើ (cmd == NULL)
	ត្រឡប់ -1;

#ifdef FEAT_EVAL
    ប្រសិនបើ (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    បើមិនដូច្នេះ
#endif
	buffer = get_cmd_output(cmd, NULL,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, NULL);
    vim_free(cmd);
    ប្រសិនបើ (buffer == NULL)
	ត្រឡប់ -1;

    cmd = buffer;
    ខណៈពេល (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	ខណៈពេល (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	ប្រសិនបើ (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	ខណៈពេល (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    ត្រឡប់ cnt;
}

#ប្រសិនបើ defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    static ចំនួនគត់
pstrcmp(const void *a, const void *b)
{
    ត្រឡប់ (pathcmp(*(var_na **)a, *(var_na **)b, -1));
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
    ចំនួនគត់
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ចំនួនគត់		flags,		// EW_* flags
    ចំនួនគត់		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ចំនួនគត់		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ចំនួនគត់		starts_with_dot;
    ចំនួនគត់		matches;
    size_t	buflen;
    size_t	len;
    ចំនួនគត់		starstar = FALSE;
    static ចំនួនគត់	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = NULL;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    ចំនួនគត់			ok;
    char_u		*p_alt;
    ចំនួនគត់			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    ប្រសិនបើ (stardepth > 0)
    {
	ui_breakcheck();
	ប្រសិនបើ (got_int)
	    ត្រឡប់ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    ប្រសិនបើ (buf == NULL)
	ត្រឡប់ 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    ខណៈពេល (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ប្រសិនបើ (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	បើមិនដូច្នេះ ប្រសិនបើ (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    ប្រសិនបើ (e != NULL)
		bha_nga;
	    s = p + 1;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != NULL)
	    e = p;
	ប្រសិនបើ (has_mbyte)
	{
	    ចំនួនគត់ charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	បើមិនដូច្នេះ
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    សម្រាប់ (p = buf + wildoff; p < s; ++p)
	ប្រសិនបើ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    សម្រាប់ (p = s; p < e; ++p)
	ប្រសិនបើ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    ប្រសិនបើ (pat == NULL)
    {
	vim_free(buf);
	ត្រឡប់ 0;
    }

    // compile the regexp into a program
    ប្រសិនបើ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ប្រសិនបើ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ប្រសិនបើ (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ត្រឡប់ 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != NULL;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ប្រសិនបើ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((var_na *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(void)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((var_na *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, NULL);
    ប្រសិនបើ (wn != NULL)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_VALUE);

    ខណៈពេល (ok)
    {
	p = utf16_to_enc(wfb.cFileName, NULL);   // p is allocated here

	ប្រសិនបើ (p == NULL)
	    bha_nga;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	ប្រសិនបើ (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = NULL;
	បើមិនដូច្នេះ
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, NULL);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	ប្រសិនបើ ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == NULL
		  || (regmatch.regprog != NULL
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != NULL
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((var_na *)s, buflen - len, "%s", p);
	    ប្រសិនបើ (len + 1 < buflen)
	    {
		ប្រសិនបើ (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((var_na *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (void)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((var_na *)buf + len, buflen - len, "%s", path_end);
		ប្រសិនបើ (mch_has_exp_wildcard(path_end))
		{
		    ប្រសិនបើ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		បើមិនដូច្នេះ
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ប្រសិនបើ (*path_end != 0)
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
    ប្រសិនបើ (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   parimaa_na(char_u *), pstrcmp);
    ត្រឡប់ matches;
}
#endif // MSWIN

#ប្រសិនបើ (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    static ចំនួនគត់
pstrcmp(const void *a, const void *b)
{
    ត្រឡប់ (pathcmp(*(var_na **)a, *(var_na **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    ចំនួនគត់
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ចំនួនគត់		flags,		// EW_* flags
    ចំនួនគត់		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ចំនួនគត់		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ចំនួនគត់		starts_with_dot;
    ចំនួនគត់		matches;
    size_t	buflen;
    size_t	len;
    ចំនួនគត់		starstar = FALSE;
    static ចំនួនគត់	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    sa_nracanaa dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    ប្រសិនបើ (stardepth > 0)
    {
	ui_breakcheck();
	ប្រសិនបើ (got_int)
	    ត្រឡប់ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    ប្រសិនបើ (buf == NULL)
	ត្រឡប់ 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    ខណៈពេល (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ប្រសិនបើ (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	បើមិនដូច្នេះ ប្រសិនបើ (*path_end == '/')
	{
	    ប្រសិនបើ (e != NULL)
		bha_nga;
	    s = p + 1;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != NULL
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	ប្រសិនបើ (has_mbyte)
	{
	    ចំនួនគត់ charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	បើមិនដូច្នេះ
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    សម្រាប់ (p = buf + wildoff; p < s; ++p)
	ប្រសិនបើ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    សម្រាប់ (p = s; p < e; ++p)
	ប្រសិនបើ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    ប្រសិនបើ (pat == NULL)
    {
	vim_free(buf);
	ត្រឡប់ 0;
    }

    // compile the regexp into a program
    ប្រសិនបើ (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    បើមិនដូច្នេះ
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ប្រសិនបើ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ប្រសិនបើ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ប្រសិនបើ (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ត្រឡប់ 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ប្រសិនបើ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((var_na *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(void)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (var_na *)buf);

    // Find all matching entries
    ប្រសិនបើ (dirp != NULL)
    {
	ខណៈពេល (!got_int)
	{
	    dp = readdir(dirp);
	    ប្រសិនបើ (dp == NULL)
		bha_nga;
	    len = (size_t)(s - buf);
	    ប្រសិនបើ ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != NULL && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((var_na *)s, buflen - len, "%s", dp->d_name);
		ប្រសិនបើ (len + 1 >= buflen)
		    continue;

		ប្រសិនបើ (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((var_na *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (void)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((var_na *)buf + len, buflen - len, "%s", path_end);
		ប្រសិនបើ (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    ប្រសិនបើ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		បើមិនដូច្នេះ
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ប្រសិនបើ (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    ប្រសិនបើ ((flags & EW_ALLLINKS) ? mch_lstat((var_na *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			ប្រសិនបើ (precomp_buf)
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
    ប្រសិនបើ (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   parimaa_na(char_u *), pstrcmp);
    ត្រឡប់ matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    static ចំនួនគត់
has_env_var(char_u *p)
{
    សម្រាប់ ( ; *p; MB_PTR_ADV(p))
    {
	ប្រសិនបើ (*p == '\\' && p[1] != NUL)
	    ++p;
	បើមិនដូច្នេះ ប្រសិនបើ (vim_strchr((char_u *)
#ប្រសិនបើ defined(MSWIN)
				    "$%"
#បើមិនដូច្នេះ
				    "$"
#endif
					, *p) != NULL)
	    ត្រឡប់ TRUE;
    }
    ត្រឡប់ FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    static ចំនួនគត់
has_special_wildchar(char_u *p)
{
    សម្រាប់ ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	ប្រសិនបើ (*p == '\r' || *p == '\n')
	    bha_nga;
	// Allow for escaping.
	ប្រសិនបើ (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	បើមិនដូច្នេះ ប្រសិនបើ (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != NULL)
	{
	    // A { must be followed by a matching }.
	    ប្រសិនបើ (*p == '{' && vim_strchr(p, '}') == NULL)
		continue;
	    // A quote and backtick must be followed by another one.
	    ប្រសិនបើ ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == NULL)
		continue;
	    ត្រឡប់ TRUE;
	}
    }
    ត្រឡប់ FALSE;
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
    ចំនួនគត់
gen_expand_wildcards(
    ចំនួនគត់		num_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    ចំនួនគត់		*num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ចំនួនគត់		flags)		// EW_* flags
{
    ចំនួនគត់			i;
    garray_T		ga;
    char_u		*p;
    static ចំនួនគត់		recursive = FALSE;
    ចំនួនគត់			add_pat;
    ចំនួនគត់			retval = OK;
    ចំនួនគត់			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    ប្រសិនបើ (recursive)
#ifdef SPECIAL_WILDCHAR
	ត្រឡប់ mch_expand_wildcards(num_pat, pat, num_file, file, flags);
#បើមិនដូច្នេះ
	ត្រឡប់ FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    សម្រាប់ (i = 0; i < num_pat; i++)
    {
	ប្រសិនបើ (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    ត្រឡប់ mch_expand_wildcards(num_pat, pat, num_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, parimaa_na(char_u *), 30);

    សម្រាប់ (i = 0; i < num_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	ប្រសិនបើ (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    ប្រសិនបើ (add_pat == -1)
		retval = FAIL;
	}
	បើមិនដូច្នេះ
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    ប្រសិនបើ ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		ប្រសិនបើ (p == NULL)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		បើមិនដូច្នេះ ប្រសិនបើ (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(num_pat, pat, num_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    ត្រឡប់ i;
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
	    ប្រសិនបើ (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		ប្រសិនបើ ((flags & (EW_PATH | EW_CDPATH))
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
		បើមិនដូច្នេះ
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	ប្រសិនបើ (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    ប្រសិនបើ (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    បើមិនដូច្នេះ
		addfile(&ga, t, flags);

	    ប្រសិនបើ (t != p)
		vim_free(t);
	}

	ប្រសិនបើ (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	ប្រសិនបើ (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    ប្រសិនបើ (retval == FAIL)
	ga_clear_strings(&ga);

    *num_file = ga.ga_len;
    *file = (ga.ga_data != NULL) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    ត្រឡប់ ((flags & EW_EMPTYOK) || ga.ga_data != NULL) ? retval : FAIL;
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
    void
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    ចំនួនគត់		flags)
{
    char_u	*p;
    ចំនួនគត់		isdir;
    stat_T	sb;

#ifdef MSWIN
    ប្រសិនបើ (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	ប្រសិនបើ (perm < 0)
	    ត្រឡប់;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    បើមិនដូច្នេះ
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	ប្រសិនបើ (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((var_na *)f, &sb) < 0 : mch_getperm(f) < 0))
	    ត្រឡប់;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    ប្រសិនបើ (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != NULL)
	ត្រឡប់;
#endif

    ប្រសិនបើ ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	ត្រឡប់;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    ប្រសិនបើ (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, NULL, !(flags & EW_SHELLCMD)))
	ត្រឡប់;

    // Make room for another item in the file list.
    ប្រសិនបើ (ga_grow(gap, 1) == FAIL)
	ត្រឡប់;

    p = alloc(STRLEN(f) + 1 + isdir);
    ប្រសិនបើ (p == NULL)
	ត្រឡប់;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    ប្រសិនបើ (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    void
FreeWild(ចំនួនគត់ count, char_u **files)
{
    ប្រសិនបើ (count <= 0 || files == NULL)
	ត្រឡប់;
    ខណៈពេល (count--)
	vim_free(files[count]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    ចំនួនគត់
pathcmp(const var_na *p, const var_na *q, ចំនួនគត់ maxlen)
{
    ចំនួនគត់		i, j;
    ចំនួនគត់		c1, c2;
    const var_na	*s = NULL;

    សម្រាប់ (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	ប្រសិនបើ (c1 == NUL)
	{
	    ប្រសិនបើ (c2 == NUL)  // full match
		ត្រឡប់ 0;
	    s = q;
	    i = j;
	    bha_nga;
	}

	// End of "q": check if "p" just has a slash.
	ប្រសិនបើ (c2 == NUL)
	{
	    s = p;
	    bha_nga;
	}

	ប្រសិនបើ ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    ប្រសិនបើ (vim_ispathsep(c1))
		ត្រឡប់ -1;
	    ប្រសិនបើ (vim_ispathsep(c2))
		ត្រឡប់ 1;
	    ត្រឡប់ p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    ប្រសិនបើ (s == NULL)	// "i" or "j" ran into "maxlen"
	ត្រឡប់ 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    ប្រសិនបើ (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#បើមិនដូច្នេះ
	    && c1 == '/'
#endif
       )
	ត្រឡប់ 0;   // match with trailing slash
    ប្រសិនបើ (s == q)
	ត្រឡប់ -1;	    // no match
    ត្រឡប់ 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    ចំនួនគត់
vim_isAbsName(char_u *name)
{
    ត្រឡប់ (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    ចំនួនគត់
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    ចំនួនគត់		len,
    ចំនួនគត់		force)	    // force expansion even when already absolute
{
    ចំនួនគត់		retval = OK;
    ចំនួនគត់		url;

    *buf = NUL;
    ប្រសិនបើ (fname == NULL)
	ត្រឡប់ FAIL;

    url = path_with_url(fname);
    ប្រសិនបើ (!url)
	retval = mch_FullName(fname, buf, len, force);
    ប្រសិនបើ (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#ប្រសិនបើ defined(MSWIN)
    slash_adjust(buf);
#endif
    ត្រឡប់ retval;
}
