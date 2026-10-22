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

#ضمن "vim.h"

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
    ساكن صحيح
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    صحيح		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * حجم(*newbuf));
    𐡄𐡍 (newbuf == عدم)
	أرجع FAIL;

    wfname = enc_to_utf16(*fnamep, عدم);
    𐡄𐡍 (wfname == عدم)
    {
	vim_free(newbuf);
	أرجع FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    𐡄𐡍 (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * حجم(*newbuf));
	𐡄𐡍 (newbuf == عدم)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    أرجع FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    𐡄𐡍 (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, عدم);

	𐡄𐡍 (p != عدم)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	وإلا
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    أرجع FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    أرجع OK;
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
    ساكن صحيح
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = عدم;
    char_u	*short_fname = عدم;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    صحيح		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    𐡄𐡍 (save_fname == عدم)
    {
	retval = FAIL;
	اذهب theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    لكل (;;)
    {
	// go back one path-separator
	طالما (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	𐡄𐡍 (endp <= save_fname)
	    اكسر;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	𐡄𐡍 (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    اذهب theend;
	}
	*endp = ch;	// preserve the string

	𐡄𐡍 (len > 0)
	    اكسر;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    𐡄𐡍 (len > 0)
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
	𐡄𐡍 (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    𐡄𐡍 (*fname == عدم)
	    {
		retval = FAIL;
		اذهب theend;
	    }
	}
	وإلا
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    𐡄𐡍 (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = عدم;
	}

	// concat the not-shortened part of the path
	𐡄𐡍 ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    أرجع retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    ساكن صحيح
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    صحيح		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    صحيح		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    لكل (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	𐡄𐡍 (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    𐡄𐡍 (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    وإلا
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    𐡄𐡍 (tfname == عدم)
	أرجع FAIL;

    len = tflen = STRLEN(tfname);

    𐡄𐡍 (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	أرجع FAIL;

    𐡄𐡍 (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	𐡄𐡍 (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    أرجع FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    لكل (p = tfname + len - 1; p >= tfname; --p)
    {
	𐡄𐡍 (has_mbyte)
	    p -= mb_head_off(tfname, p);
	𐡄𐡍 (vim_ispathsep(*p))
	{
	    𐡄𐡍 (sepcount == 0 || (hasTilde && sepcount == 1))
		اكسر;
	    وإلا
		sepcount --;
	}
    }
    𐡄𐡍 (hasTilde)
    {
	--p;
	𐡄𐡍 (p >= tfname)
	    *p = '~';
	وإلا
	    أرجع FAIL;
    }
    وإلا
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    أرجع OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    صحيح
modify_fname(
    char_u	*src,		// string with modifiers
    صحيح		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    صحيح		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    صحيح		c;
    صحيح		has_fullname = 0;
    صحيح		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    صحيح		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    𐡄𐡍 (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	𐡄𐡍 ((*fnamep)[0] == '~'
#𐡄𐡍 !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    𐡄𐡍 (*fnamep == عدم)
		أرجع -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	لكل (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    𐡄𐡍 (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		اكسر;
	}

	// FullName_save() is slow, don't use it when not needed.
	𐡄𐡍 (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    𐡄𐡍 (*fnamep == عدم)
		أرجع -1;
	}

#ifdef MSWIN
# 𐡄𐡍 _WIN32_WINNT >= 0x0500
	𐡄𐡍 (vim_strchr(*fnamep, '~') != عدم)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, عدم);
	    WCHAR buf[_MAX_PATH];

	    𐡄𐡍 (wfname != عدم)
	    {
		𐡄𐡍 (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, عدم);

		    𐡄𐡍 (q != عدم)
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
	𐡄𐡍 (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    𐡄𐡍 (*fnamep == عدم)
		أرجع -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    طالما (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	𐡄𐡍 (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    استمر;
	}
	pbuf = عدم;
	// Need full path first (use expand_env() to remove a "~/")
	𐡄𐡍 (!has_fullname && !has_homerelative)
	{
	    𐡄𐡍 (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    وإلا
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	وإلا
	    p = *fnamep;

	has_fullname = 0;

	𐡄𐡍 (p != عدم)
	{
	    size_t  dirnamelen = 0;

	    𐡄𐡍 (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		𐡄𐡍 (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    𐡄𐡍 (s != عدم)
		    {
			dirnamelen = home_replace(عدم, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		𐡄𐡍 (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		𐡄𐡍 (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    𐡄𐡍 (vim_ispathsep(*p))
		    {
			طالما (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			𐡄𐡍 (pbuf != عدم)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = عدم;
			}
		    }
		}
	    }
	    وإلا
	    {
		dirnamelen = home_replace(عدم, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		𐡄𐡍 (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    𐡄𐡍 (s != عدم)
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
    طالما (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	طالما (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	𐡄𐡍 (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	𐡄𐡍 (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    𐡄𐡍 (p == عدم)
		أرجع -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	وإلا
	{
	    طالما (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    𐡄𐡍 (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    𐡄𐡍 (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	𐡄𐡍 (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    𐡄𐡍 (p == عدم)
		أرجع -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	𐡄𐡍 (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    𐡄𐡍 (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		أرجع -1;
	}
	وإلا
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    𐡄𐡍 (get_short_pathname(fnamep, bufp, &l) == FAIL)
		أرجع -1;

	    𐡄𐡍 (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		𐡄𐡍 (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    أرجع -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    𐡄𐡍 (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    طالما (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	𐡄𐡍 (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	وإلا
	    s = *fnamep + *fnamelen - 1;
	لكل ( ; s > tail; --s)
	    𐡄𐡍 (s[0] == '.')
		اكسر;
	𐡄𐡍 (src[*usedlen + 1] == 'e')		// :e
	{
	    𐡄𐡍 (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		لكل ( ; s > *fnamep; --s)
		    𐡄𐡍 (s[0] == ';')
			اكسر;
		𐡄𐡍 (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    وإلا 𐡄𐡍 (*fnamep <= tail)
		*fnamelen = 0;
	}
	وإلا				// :r
	{
	    char_u *limit = *fnamep;

	    𐡄𐡍 (limit < tail)
		limit = tail;
	    𐡄𐡍 (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    𐡄𐡍 (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	صحيح	    sep;
	char_u	    *flags;
	صحيح	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	𐡄𐡍 (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	𐡄𐡍 (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    𐡄𐡍 (p != عدم)
	    {
		pat = vim_strnsave(s, p - s);
		𐡄𐡍 (pat != عدم)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    𐡄𐡍 (p != عدم)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			𐡄𐡍 (sub != عدم && str != عدم)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, عدم, flags, &slen);
			    𐡄𐡍 (s != عدم)
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
	    𐡄𐡍 (didit)
		اذهب repeat;
	}
    }

    𐡄𐡍 (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	𐡄𐡍 (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	𐡄𐡍 (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	𐡄𐡍 (p == عدم)
	    أرجع -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    أرجع valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    ساكن فراغ
shorten_dir_len(char_u *str, صحيح trim_len)
{
    char_u	*tail, *s, *d;
    صحيح		skip = FALSE;
    صحيح		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    لكل (s = str; ; ++s)
    {
	𐡄𐡍 (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    𐡄𐡍 (*s == NUL)
		اكسر;
	}
	وإلا 𐡄𐡍 (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	وإلا 𐡄𐡍 (!skip)
	{
	    *d++ = *s;			// copy next char
	    𐡄𐡍 (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		𐡄𐡍 (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    𐡄𐡍 (has_mbyte)
	    {
		صحيح l = mb_ptr2len(s);

		طالما (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    فراغ
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    صحيح
file_is_readable(char_u *fname)
{
    صحيح		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    𐡄𐡍 (*fname == NUL || mch_isdir_or_missing(fname))
	أرجع FALSE;
    𐡄𐡍 ((fd = mch_open((حرف *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	أرجع TRUE;
    }
    أرجع FALSE;
}

#𐡄𐡍 defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    فراغ
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    𐡄𐡍 (check_secure())
	أرجع;

    𐡄𐡍 (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	𐡄𐡍 (in_vim9script())
	    (فراغ) check_for_string_arg(argvars, 0);
	أرجع;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    𐡄𐡍 (cwd != عدم)
    {
	𐡄𐡍 (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	𐡄𐡍 (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	وإلا 𐡄𐡍 (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	وإلا 𐡄𐡍 (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	وإلا
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "scope", s);
	    أرجع;
	}
    }
    وإلا 𐡄𐡍 (curwin->w_localdir != عدم)
	scope = CDSCOPE_WINDOW;
    وإلا 𐡄𐡍 (curtab->tp_localdir != عدم)
	scope = CDSCOPE_TABPAGE;

    𐡄𐡍 (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    فراغ
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_number = -1;
    𐡄𐡍 (check_restricted() || check_secure())
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    name = tv_get_string(&argvars[0]);
    𐡄𐡍 (name == عدم || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	أرجع;
    }

    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    وإلا
	flags = (char_u *)"";

    𐡄𐡍 (*flags == NUL)
	// delete a file
	rettv->vval.v_number = mch_remove(name) == 0 ? 0 : -1;
    وإلا 𐡄𐡍 (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_number = mch_rmdir(name) == 0 ? 0 : -1;
    وإلا 𐡄𐡍 (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_number = delete_recursive(name);
    وإلا
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    فراغ
f_executable(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_number = mch_can_exe(tv_get_string(&argvars[0]), عدم, TRUE);
}

/*
 * "exepath()" function
 */
    فراغ
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = عدم;

    𐡄𐡍 (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	أرجع;
    (فراغ)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    فراغ
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;
    rettv->vval.v_number = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    فراغ
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;
    rettv->vval.v_number = filewritable(tv_get_string(&argvars[0]));
}

    ساكن فراغ
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    صحيح		find_what)
{
    char_u	*fname;
    char_u	*fresult = عدم;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    صحيح		count = 1;
    صحيح		first = TRUE;
    صحيح		error = FALSE;

    rettv->vval.v_string = عدم;
    rettv->v_type = VAR_STRING;
    𐡄𐡍 (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	أرجع;

    fname = tv_get_string(&argvars[0]);

    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	𐡄𐡍 (p == عدم)
	    error = TRUE;
	وإلا
	{
	    𐡄𐡍 (*p != NUL)
		path = p;

	    𐡄𐡍 (argvars[2].v_type != VAR_UNKNOWN)
		count = (صحيح)tv_get_number_chk(&argvars[2], &error);
	}
    }

    𐡄𐡍 (count < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    𐡄𐡍 (*fname != NUL && !error)
    {
	char_u	*file_to_find = عدم;
	حرف	*search_ctx = عدم;

	𐡏𐡁𐡃
	{
	    𐡄𐡍 (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : عدم,
					       first ? (صحيح)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &search_ctx);
	    first = FALSE;

	    𐡄𐡍 (fresult != عدم && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} طالما ((rettv->v_type == VAR_LIST || --count > 0) && fresult != عدم);

	vim_free(file_to_find);
	vim_findfile_cleanup(search_ctx);
    }

    𐡄𐡍 (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    فراغ
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    فراغ
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    فراغ
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = عدم;
    char_u	buf[NUMBUFLEN];

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    𐡄𐡍 (mods == عدم || fname == عدم)
	fname = عدم;
    وإلا
    {
	len = STRLEN(fname);
	𐡄𐡍 (mods != عدم && *mods != NUL)
	    (فراغ)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    𐡄𐡍 (fname == عدم)
	rettv->vval.v_string = عدم;
    وإلا
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
    فراغ
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = عدم;
    tabpage_T	*tp = عدم;
    char_u	*cwd;
    صحيح		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    𐡄𐡍 (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	أرجع;

    𐡄𐡍 (argvars[0].v_type == VAR_NUMBER
	    && argvars[0].vval.v_number == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    وإلا
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    𐡄𐡍 (wp != عدم && wp->w_localdir != عدم
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    وإلا 𐡄𐡍 (tp != عدم && tp->tp_localdir != عدم
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    وإلا 𐡄𐡍 (wp != عدم || tp != عدم || global)
    {
	𐡄𐡍 (globaldir != عدم && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	وإلا
	{
	    cwd = alloc(MAXPATHL);
	    𐡄𐡍 (cwd != عدم)
	    {
		𐡄𐡍 (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    𐡄𐡍 (rettv->vval.v_string != عدم)
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
    صحيح		    i;

    لكل (i = 0; i < 9; i++)
    {
	𐡄𐡍 (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	وإلا
	    perm[i] = '-';
    }
    أرجع perm;
}

/*
 * "getfperm({fname})" function
 */
    فراغ
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = عدم;
    char_u	permbuf[] = "---------";

    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    𐡄𐡍 (mch_stat((حرف *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    فراغ
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    fname = tv_get_string(&argvars[0]);
    𐡄𐡍 (mch_stat((حرف *)fname, &st) >= 0)
    {
	𐡄𐡍 (S_ISDIR(st.st_mode))
	    rettv->vval.v_number = 0;
	وإلا
	{
	    rettv->vval.v_number = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    𐡄𐡍 ((off_T)rettv->vval.v_number != (off_T)st.st_size)
		rettv->vval.v_number = -2;
	}
    }
    وإلا
	  rettv->vval.v_number = -1;
}

/*
 * "getftime({fname})" function
 */
    فراغ
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    fname = tv_get_string(&argvars[0]);
    𐡄𐡍 (mch_stat((حرف *)fname, &st) >= 0)
	rettv->vval.v_number = (varnumber_T)st.st_mtime;
    وإلا
	rettv->vval.v_number = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    حرف    *t;

    𐡄𐡍 (S_ISREG(st->st_mode))
	t = "file";
    وإلا 𐡄𐡍 (S_ISDIR(st->st_mode))
	t = "dir";
    وإلا 𐡄𐡍 (S_ISLNK(st->st_mode))
	t = "link";
    وإلا 𐡄𐡍 (S_ISBLK(st->st_mode))
	t = "bdev";
    وإلا 𐡄𐡍 (S_ISCHR(st->st_mode))
	t = "cdev";
    وإلا 𐡄𐡍 (S_ISFIFO(st->st_mode))
	t = "fifo";
    وإلا 𐡄𐡍 (S_ISSOCK(st->st_mode))
	t = "socket";
    وإلا
	t = "other";
    أرجع (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    فراغ
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = عدم;

    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    𐡄𐡍 (mch_lstat((حرف *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    فراغ
f_glob(typval_T *argvars, typval_T *rettv)
{
    صحيح		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    صحيح		error = FALSE;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	أرجع;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
    {
	𐡄𐡍 (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	𐡄𐡍 (argvars[2].v_type != VAR_UNKNOWN)
	{
	    𐡄𐡍 (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, عدم);
	    𐡄𐡍 (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    𐡄𐡍 (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	𐡄𐡍 (p_wic)
	    options += WILD_ICASE;
	𐡄𐡍 (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     عدم, options, WILD_ALL);
	وإلا 𐡄𐡍 (rettv_list_alloc(rettv) == OK)
	{
	  صحيح i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						عدم, options, WILD_ALL_KEEP);
	  لكل (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    وإلا
	rettv->vval.v_string = عدم;
}

/*
 * "glob2regpat()" function
 */
    فراغ
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == عدم)
			 ? عدم : file_pat_to_reg_pat(pat, عدم, عدم, FALSE);
}

/*
 * "globpath()" function
 */
    فراغ
f_globpath(typval_T *argvars, typval_T *rettv)
{
    صحيح		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    صحيح		error = FALSE;
    garray_T	ga;
    صحيح		i;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	أرجع;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    𐡄𐡍 (argvars[2].v_type != VAR_UNKNOWN)
    {
	𐡄𐡍 (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	𐡄𐡍 (argvars[3].v_type != VAR_UNKNOWN)
	{
	    𐡄𐡍 (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, عدم);
	    𐡄𐡍 (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    𐡄𐡍 (file != عدم && !error)
    {
	ga_init2(&ga, حجم(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	𐡄𐡍 (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	وإلا 𐡄𐡍 (rettv_list_alloc(rettv) == OK)
	    لكل (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    وإلا
	rettv->vval.v_string = عدم;
}

/*
 * "isdirectory()" function
 */
    فراغ
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_number = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    فراغ
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_number = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    ساكن صحيح
mkdir_recurse(char_u *dir, صحيح prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    صحيح		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    𐡄𐡍 (p <= get_past_head(dir))
	أرجع OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    𐡄𐡍 (updir == عدم)
	أرجع FAIL;
    𐡄𐡍 (mch_isdir(updir))
	r = OK;
    وإلا 𐡄𐡍 (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	𐡄𐡍 (r == OK && created != عدم && *created == عدم)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    أرجع r;
}

/*
 * "mkdir()" function
 */
    فراغ
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    صحيح		prot = 0755;
    صحيح		defer = FALSE;
    صحيح		defer_recurse = FALSE;
    char_u	*created = عدم;

    rettv->vval.v_number = FAIL;
    𐡄𐡍 (check_restricted() || check_secure())
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	أرجع;

    dir = tv_get_string_buf(&argvars[0], buf);
    𐡄𐡍 (*dir == NUL)
	أرجع;

    𐡄𐡍 (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	𐡄𐡍 (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (صحيح)tv_get_number_chk(&argvars[2], عدم);
	    𐡄𐡍 (prot == -1)
		أرجع;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != عدم;
	defer_recurse = vim_strchr(arg2, 'R') != عدم;
	𐡄𐡍 ((defer || defer_recurse) && !can_add_defer())
	    أرجع;

	𐡄𐡍 (vim_strchr(arg2, 'p') != عدم)
	{
	    𐡄𐡍 (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_number = OK;
		أرجع;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : عدم);
	}
    }
    rettv->vval.v_number = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    𐡄𐡍 (rettv->vval.v_number == OK
				&& created == عدم && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    𐡄𐡍 (created != عدم)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	𐡄𐡍 (tv[0].vval.v_string == عدم || tv[1].vval.v_string == عدم
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
    فراغ
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحيح		trim_len = 1;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	أرجع;

    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (صحيح)tv_get_number(&argvars[1]);
	𐡄𐡍 (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    𐡄𐡍 (p == عدم)
	rettv->vval.v_string = عدم;
    وإلا
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	𐡄𐡍 (p != عدم)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    ساكن صحيح
checkitem_common(فراغ *context, char_u *name, dict_T *dict)
{
    typval_T	*expr = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    صحيح		retval = 0;
    صحيح		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    𐡄𐡍 (name != عدم)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    وإلا
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    𐡄𐡍 (eval_expr_typval(expr, FALSE, argv, 1, عدم, &rettv) == FAIL)
	اذهب theend;

    // We want to use -1, but also true/false should be allowed.
    𐡄𐡍 (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_NUMBER;
	rettv.vval.v_number = rettv.vval.v_number == VVAL_TRUE;
    }
    retval = tv_get_number_chk(&rettv, &error);
    𐡄𐡍 (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    𐡄𐡍 (name != عدم)
	set_vim_var_string(VV_VAL, عدم, 0);
    وإلا
	set_vim_var_dict(VV_VAL, عدم);
    restore_vimvar(VV_VAL, &save_val);
    أرجع retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    ساكن صحيح
readdir_checkitem(فراغ *context, فراغ *item)
{
    char_u	*name = (char_u *)item;

    أرجع checkitem_common(context, name, عدم);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    ساكن صحيح
readdirex_dict_arg(typval_T *argvars, صحيح *cmp)
{
    char_u     *compare;

    𐡄𐡍 (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	أرجع FAIL;

    𐡄𐡍 (dict_has_key(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    وإلا
    {
	semsg(_(e_dictionary_key_str_required), "sort");
	أرجع FAIL;
    }

    𐡄𐡍 (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    وإلا 𐡄𐡍 (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    وإلا 𐡄𐡍 (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    وإلا 𐡄𐡍 (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    أرجع OK;
}

/*
 * "readdir()" function
 */
    فراغ
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    صحيح		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    صحيح		i;
    صحيح		sort = READDIR_SORT_BYTE;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    𐡄𐡍 (check_secure())
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	أرجع;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	أرجع;

    ret = readdir_core(&ga, path, FALSE, (فراغ *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? عدم : readdir_checkitem, sort);
    𐡄𐡍 (ret == OK)
    {
	لكل (i = 0; i < ga.ga_len; i++)
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
    ساكن صحيح
readdirex_checkitem(فراغ *context, فراغ *item)
{
    dict_T	*dict = (dict_T*)item;

    أرجع checkitem_common(context, عدم, dict);
}

/*
 * "readdirex()" function
 */
    فراغ
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    صحيح		ret;
    char_u	*path;
    garray_T	ga;
    صحيح		i;
    صحيح		sort = READDIR_SORT_BYTE;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    𐡄𐡍 (check_secure())
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	أرجع;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	أرجع;

    ret = readdir_core(&ga, path, TRUE, (فراغ *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? عدم : readdirex_checkitem, sort);
    𐡄𐡍 (ret == OK)
    {
	لكل (i = 0; i < ga.ga_len; i++)
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
    ساكن فراغ
read_file_or_blob(typval_T *argvars, typval_T *rettv, صحيح always_blob)
{
    صحيح		binary = FALSE;
    صحيح		blob = always_blob;
    صحيح		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    صحيح		io_size = حجم(buf);
    صحيح		readlen;		// size of last fread()
    char_u	*prev	 = عدم;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    𐡄𐡍 (argvars[1].v_type != VAR_UNKNOWN)
    {
	𐡄𐡍 (always_blob)
	{
	    offset = (off_T)tv_get_number(&argvars[1]);
	    𐡄𐡍 (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_number(&argvars[2]);
	}
	وإلا
	{
	    𐡄𐡍 (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		binary = TRUE;
	    𐡄𐡍 (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    𐡄𐡍 (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_number(&argvars[2]);
	}
    }

    𐡄𐡍 ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	أرجع;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    𐡄𐡍 (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	أرجع;
    }
    𐡄𐡍 (*fname == NUL || (fd = mch_fopen((حرف *)fname, READBIN)) == عدم)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	أرجع;
    }

    𐡄𐡍 (blob)
    {
	𐡄𐡍 (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	أرجع;
    }

    طالما (cnt < maxline || maxline < 0)
    {
	readlen = (صحيح)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	لكل (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || binary));
		++p)
	{
	    𐡄𐡍 (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= عدم;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		𐡄𐡍 (readlen > 0 && !binary)
		{
		    طالما (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    𐡄𐡍 (len == 0)
			طالما (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		𐡄𐡍 (prevlen == 0)
		    s = vim_strnsave(start, len);
		وإلا
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    𐡄𐡍 ((s = vim_realloc(prev, prevlen + len + 1)) != عدم)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = عدم; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		𐡄𐡍 (s == عدم)
		{
		    do_outofmem_msg((long_u) prevlen + len + 1);
		    failed = TRUE;
		    اكسر;
		}

		𐡄𐡍 ((li = listitem_alloc()) == عدم)
		{
		    vim_free(s);
		    failed = TRUE;
		    اكسر;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		𐡄𐡍 ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    اكسر;
	    }
	    وإلا 𐡄𐡍 (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    وإلا 𐡄𐡍 (*p == 0xbf && enc_utf8 && !binary)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		𐡄𐡍 (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    𐡄𐡍 (start == dest)
			start = p + 1;
		    وإلا
		    {
			// have to shuffle buf to close gap
			صحيح adjust_prevlen = 0;

			𐡄𐡍 (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (صحيح)(buf - dest);
			    dest = buf;
			}
			𐡄𐡍 (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	𐡄𐡍 (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    اكسر;
	𐡄𐡍 (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    𐡄𐡍 (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		𐡄𐡍 (prevsize == 0)
		    prevsize = (long)(p - start);
		وإلا
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		𐡄𐡍 (newprev == عدم)
		{
		    do_outofmem_msg((long_u)prevsize);
		    failed = TRUE;
		    اكسر;
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
    𐡄𐡍 (!failed && maxline < 0)
	طالما (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    𐡄𐡍 (failed)
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
    فراغ
f_readblob(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (check_secure())
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	أرجع;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    فراغ
f_readfile(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (check_secure())
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	أرجع;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    فراغ
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = عدم;
# endif

    𐡄𐡍 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = عدم;

	v = mch_resolve_path(p, TRUE);
	𐡄𐡍 (v != عدم)
	    rettv->vval.v_string = v;
	وإلا
	    rettv->vval.v_string = vim_strsave(p);
    }
# وإلا
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	صحيح	len;
	char_u	*remain = عدم;
	char_u	*q;
	صحيح	is_relative_to_current = FALSE;
	صحيح	has_trailing_pathsep = FALSE;
	صحيح	limit = 100;

	p = vim_strsave(p);
	𐡄𐡍 (p == عدم)
	    اذهب fail;
	𐡄𐡍 (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	𐡄𐡍 (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	𐡄𐡍 (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	𐡄𐡍 (buf == عدم)
	{
	    vim_free(p);
	    vim_free(remain);
	    اذهب fail;
	}

	لكل (;;)
	{
	    لكل (;;)
	    {
		len = readlink((حرف *)p, (حرف *)buf, MAXPATHL);
		𐡄𐡍 (len <= 0)
		    اكسر;
		buf[len] = NUL;

		𐡄𐡍 (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = عدم;
		    اذهب fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		𐡄𐡍 (remain == عدم && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		𐡄𐡍 (*q != NUL)
		{
		    𐡄𐡍 (remain == عدم)
			remain = vim_strsave(q - 1);
		    وإلا
		    {
			cpy = concat_str(q - 1, remain);
			𐡄𐡍 (cpy != عدم)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		𐡄𐡍 (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		𐡄𐡍 (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    𐡄𐡍 (cpy != عدم)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		وإلا
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    𐡄𐡍 (remain == عدم)
		اكسر;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    𐡄𐡍 (cpy != عدم)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    𐡄𐡍 (*q != NUL)
		STRMOVE(remain, q - 1);
	    وإلا
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	𐡄𐡍 (!vim_ispathsep(*p))
	{
	    𐡄𐡍 (is_relative_to_current
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
		𐡄𐡍 (cpy != عدم)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    وإلا 𐡄𐡍 (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		طالما (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		𐡄𐡍 (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	𐡄𐡍 (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    𐡄𐡍 (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  وإلا
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
    فراغ
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    ساكن صحيح	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    𐡏𐡁𐡃
    {
	𐡄𐡍 (x == 'Z')
	    x = '0';
	وإلا 𐡄𐡍 (x == '9')
	    x = 'A';
	وإلا
	    ++x;
    } طالما (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    فراغ
f_writefile(typval_T *argvars, typval_T *rettv)
{
    صحيح		binary = FALSE;
    صحيح		append = FALSE;
    صحيح		defer = FALSE;
# ifdef HAVE_FSYNC
    صحيح		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    صحيح		ret = 0;
    listitem_T	*li;
    list_T	*list = عدم;
    blob_T	*blob = عدم;

    rettv->vval.v_number = -1;
    𐡄𐡍 (check_secure())
	أرجع;

    𐡄𐡍 (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	أرجع;

    𐡄𐡍 (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	𐡄𐡍 (list == عدم)
	    أرجع;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, li)
	    𐡄𐡍 (tv_get_string_chk(&li->li_tv) == عدم)
		أرجع;
    }
    وإلا 𐡄𐡍 (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	𐡄𐡍 (blob == عدم)
	    أرجع;
    }
    وإلا
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	أرجع;
    }

    𐡄𐡍 (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	𐡄𐡍 (arg2 == عدم)
	    أرجع;
	𐡄𐡍 (vim_strchr(arg2, 'b') != عدم)
	    binary = TRUE;
	𐡄𐡍 (vim_strchr(arg2, 'a') != عدم)
	    append = TRUE;
	𐡄𐡍 (vim_strchr(arg2, 'D') != عدم)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	𐡄𐡍 (vim_strchr(arg2, 's') != عدم)
	    do_fsync = TRUE;
	وإلا 𐡄𐡍 (vim_strchr(arg2, 'S') != عدم)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    𐡄𐡍 (fname == عدم)
	أرجع;

    𐡄𐡍 (defer && !can_add_defer())
	أرجع;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    𐡄𐡍 (*fname == NUL || (fd = mch_fopen((حرف *)fname,
				      append ? APPENDBIN : WRITEBIN)) == عدم)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    وإلا
    {
	𐡄𐡍 (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    𐡄𐡍 (tv.vval.v_string == عدم
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(فراغ)mch_remove(fname);
	    }
	}

	𐡄𐡍 (ret == 0)
	{
	    𐡄𐡍 (blob)
	    {
		𐡄𐡍 (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    وإلا
	    {
		𐡄𐡍 (write_list(fd, list, binary) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    𐡄𐡍 (ret == 0 && do_fsync)
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

#𐡄𐡍 defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    صحيح		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    ساكن char_u	*last_dir = عدم;    // last used directory
    char_u		*tofree = عدم;
    صحيح			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    𐡄𐡍 (title == عدم || *title == NUL)
    {
	𐡄𐡍 (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	وإلا 𐡄𐡍 (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	وإلا
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    𐡄𐡍 ((initdir == عدم || *initdir == NUL) && dflt != عدم && *dflt != NUL)
    {
	𐡄𐡍 (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = عدم;
	}
	وإلا 𐡄𐡍 (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    𐡄𐡍 (tofree != عدم)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    𐡄𐡍 (initdir == عدم || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	𐡄𐡍 (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	وإلا 𐡄𐡍 (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != عدم && buf->b_ffname != عدم)
	{
	    𐡄𐡍 (dflt == عدم || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    𐡄𐡍 (tofree != عدم)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	وإلا 𐡄𐡍 (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    𐡄𐡍 (gui.in_use)		// when this changes, also adjust f_has()!
    {
	𐡄𐡍 (filter == عدم
#  ifdef FEAT_EVAL
		&& (filter = get_var_value((char_u *)"b:browsefilter")) == عدم
		&& (filter = get_var_value((char_u *)"g:browsefilter")) == عدم
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	𐡄𐡍 (flags & BROWSE_DIR)
	{
#  𐡄𐡍 defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  وإلا
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  𐡄𐡍 !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    𐡄𐡍 (fname != عدم && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		𐡄𐡍 (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	وإلا
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    وإلا
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = عدم;
    }

    // keep the directory for next time
    𐡄𐡍 (fname != عدم)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	𐡄𐡍 (last_dir != عدم && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    𐡄𐡍 (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		𐡄𐡍 (last_dir != عدم)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    أرجع fname;
}
#endif

#𐡄𐡍 defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    فراغ
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    صحيح		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    صحيح		error = FALSE;

    𐡄𐡍 (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	أرجع;

    save = (صحيح)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    𐡄𐡍 (error || title == عدم || initdir == عدم || defname == عدم)
	rettv->vval.v_string = عدم;
    وإلا
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, عدم, initdir, عدم, curbuf);
# وإلا
    rettv->vval.v_string = عدم;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    فراغ
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    𐡄𐡍 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    𐡄𐡍 (title == عدم || initdir == عدم)
	rettv->vval.v_string = عدم;
    وإلا
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, عدم, عدم, initdir, عدم, curbuf);
# وإلا
    rettv->vval.v_string = عدم;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    فراغ
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_number = FALSE;

    𐡄𐡍 (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	أرجع;

    from = tv_get_string(&argvars[0]);

    𐡄𐡍 (mch_lstat((حرف *)from, &st) >= 0
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
    صحيح		dstlen,	// maximum length of the result
    صحيح		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    𐡄𐡍 (src == عدم)
    {
	*dst = NUL;
	أرجع 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    𐡄𐡍 (buf != عدم && buf->b_help)
	أرجع vim_snprintf_safelen((حرف *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    𐡄𐡍 (homedir != عدم)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#وإلا
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    𐡄𐡍 (homedir_env == عدم)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    𐡄𐡍 (homedir_env != عدم && *homedir_env == NUL)
	homedir_env = عدم;

    𐡄𐡍 (homedir_env != عدم && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = عدم;

	flen = STRLEN(homedir_env);
	(فراغ)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	𐡄𐡍 (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    𐡄𐡍 (homedir_env != عدم)
	envlen = STRLEN(homedir_env);

    𐡄𐡍 (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    طالما (*src && dstlen > 0)
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
	لكل (;;)
	{
	    𐡄𐡍 (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		𐡄𐡍 (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		اكسر;
	    }
	    𐡄𐡍 (p == homedir_env)
		اكسر;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	طالما (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	طالما ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    𐡄𐡍 (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    أرجع (size_t)(dst - dst_start);
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
    𐡄𐡍 (src != عدم)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    𐡄𐡍 (dst != عدم)
	home_replace(buf, src, dst, len, TRUE);
    أرجع dst;
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
    صحيح
fullpathcmp(
    char_u *s1,
    char_u *s2,
    صحيح	    checkname,		// when both don't exist, check file names
    صحيح	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    صحيح		    r1, r2;

    𐡄𐡍 (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    وإلا
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((حرف *)exp1, &st1);
    r2 = mch_stat((حرف *)s2, &st2);
    𐡄𐡍 (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	𐡄𐡍 (checkname)
	{
	    𐡄𐡍 (fnamecmp(exp1, s2) == 0)
		أرجع FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    𐡄𐡍 (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		أرجع FPC_SAMEX;
	}
	أرجع FPC_NOTX;
    }
    𐡄𐡍 (r1 != 0 || r2 != 0)
	أرجع FPC_DIFFX;
    𐡄𐡍 (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	أرجع FPC_SAME;
    أرجع FPC_DIFF;
#وإلا
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    صحيح	    retval = FPC_DIFF;
    صحيح	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    𐡄𐡍 ((exp1 = alloc(MAXPATHL * 3)) != عدم)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	𐡄𐡍 (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	وإلا
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	𐡄𐡍 (r1 != OK && r2 != OK)
	{
	    𐡄𐡍 (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    وإلا
		retval = FPC_NOTX;
	}
	وإلا 𐡄𐡍 (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	وإلا 𐡄𐡍 (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	وإلا
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    أرجع retval;
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

    𐡄𐡍 (fname == عدم)
	أرجع (char_u *)"";
    لكل (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	𐡄𐡍 (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    أرجع p1;
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
    طالما (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    أرجع t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    طالما (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    𐡄𐡍 (*fname)
	++fname;
    أرجع fname;
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

#𐡄𐡍 defined(MSWIN)
    // may skip "c:"
    𐡄𐡍 (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    وإلا
	retval = path;
#وإلا
# 𐡄𐡍 defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    𐡄𐡍 (retval == عدم)
	retval = path;
# وإلا	// Unix
    retval = path;
# endif
#endif

    طالما (vim_ispathsep(*retval))
	++retval;

    أرجع retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    صحيح
vim_ispathsep(صحيح c)
{
#ifdef UNIX
    أرجع (c == '/');	    // UNIX has ':' inside file names
#وإلا
# ifdef BACKSLASH_IN_FILENAME
    أرجع (c == ':' || c == '/' || c == '\\');
# وإلا
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    أرجع (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  وإلا
    أرجع (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    صحيح
vim_ispathsep_nocolon(صحيح c)
{
    أرجع vim_ispathsep(c)
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
    صحيح
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    صحيح		c;
    صحيح		retval;

    p = gettail_sep(fname);
    𐡄𐡍 (p == fname)
	أرجع TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    أرجع retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    صحيح
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    أرجع vim_fnamencmp(x, y, MAXPATHL);
#وإلا
    𐡄𐡍 (p_fic)
	أرجع MB_STRICMP(x, y);
    أرجع STRCMP(x, y);
#endif
}

    صحيح
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    صحيح		cx = NUL;
    صحيح		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    لكل (صحيح swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	𐡄𐡍 ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    𐡄𐡍 (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		اكسر;
	    }
	    وإلا // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    طالما (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	𐡄𐡍 (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    اكسر;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    𐡄𐡍 (len == 0)
	أرجع 0;
    أرجع (cx - cy);
#وإلا
    𐡄𐡍 (p_fic)
	أرجع MB_STRNICMP(x, y, len);
    أرجع STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, صحيح sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? حجم(PATHSEP) : 0) + fname2len + 1);
    𐡄𐡍 (ret->string == عدم)
	ret->length = 0;
    وإلا
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	𐡄𐡍 (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += حجم(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    أرجع ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    فراغ
add_pathsep(char_u *p)
{
    size_t  plen;

    𐡄𐡍 (*p == NUL)
	أرجع;

    plen = STRLEN(p);
    𐡄𐡍 (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    صحيح		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = عدم;

    𐡄𐡍 (fname == عدم)
	أرجع عدم;

    buf = alloc(MAXPATHL);
    𐡄𐡍 (buf == عدم)
	أرجع عدم;

    𐡄𐡍 (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    وإلا
	new_fname = vim_strsave(fname);
    vim_free(buf);
    أرجع new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    صحيح
vim_fexists(char_u *fname)
{
    stat_T st;

    𐡄𐡍 (mch_stat((حرف *)fname, &st))
	أرجع FALSE;
    أرجع TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    صحيح
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    صحيح		  *num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    صحيح		   flags)	// EW_DIR, etc.
{
    صحيح		ret = FAIL;
    char_u	*eval_pat = عدم;
    char_u	*exp_pat = *pat;
    حرف	*ignored_msg;
    size_t	usedlen;
    صحيح		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    صحيح		star_follows = FALSE;

    𐡄𐡍 (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       عدم, &ignored_msg, عدم, TRUE);
	--emsg_off;
	𐡄𐡍 (eval_pat != عدم)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    𐡄𐡍 (exp_pat != عدم)
	ret = expand_wildcards(1, &exp_pat, num_file, file, flags);

    𐡄𐡍 (eval_pat != عدم)
    {
	𐡄𐡍 (*num_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    𐡄𐡍 (*file != عدم)
	    {
		**file = eval_pat;
		eval_pat = عدم;
		*num_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    أرجع ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    صحيح
expand_wildcards(
    صحيح		   num_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    صحيح		  *num_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    صحيح		   flags)	// EW_DIR, etc.
{
    صحيح		retval;
    صحيح		i, j;
    char_u	*p;
    صحيح		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(num_pat, pat, num_files, files, flags);

    // When keeping all matches, return here
    𐡄𐡍 ((flags & EW_KEEPALL) || retval == FAIL)
	أرجع retval;

    /*
     * Remove names that match 'wildignore'.
     */
    𐡄𐡍 (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	لكل (i = 0; i < *num_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    𐡄𐡍 (ffname == عدم)		// out of memory
		اكسر;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    𐡄𐡍 (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		لكل (j = i; j + 1 < *num_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*num_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	𐡄𐡍 (*num_files == 0)
	{
	    VIM_CLEAR(*files);
	    أرجع FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    𐡄𐡍 (*num_files > 1 && !got_int)
    {
	non_suf_match = 0;
	لكل (i = 0; i < *num_files; ++i)
	{
	    𐡄𐡍 (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		لكل (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    أرجع retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    صحيح
match_suffix(char_u *fname)
{
    صحيح		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (صحيح)STRLEN(fname);
    setsuflen = 0;
    لكل (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	𐡄𐡍 (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    𐡄𐡍 (vim_strchr(tail, '.') == عدم)
	    {
		setsuflen = 1;
		اكسر;
	    }
	}
	وإلا
	{
	    𐡄𐡍 (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		اكسر;
	    setsuflen = 0;
	}
    }
    أرجع (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    ساكن صحيح
vim_backtick(char_u *p)
{
    أرجع (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    ساكن صحيح
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    صحيح		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    صحيح		cnt = 0;
    صحيح		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    𐡄𐡍 (cmd == عدم)
	أرجع -1;

#ifdef FEAT_EVAL
    𐡄𐡍 (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    وإلا
#endif
	buffer = get_cmd_output(cmd, عدم,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, عدم);
    vim_free(cmd);
    𐡄𐡍 (buffer == عدم)
	أرجع -1;

    cmd = buffer;
    طالما (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	طالما (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	𐡄𐡍 (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	طالما (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    أرجع cnt;
}

#𐡄𐡍 defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    ساكن صحيح
pstrcmp(ثابت فراغ *a, ثابت فراغ *b)
{
    أرجع (pathcmp(*(حرف **)a, *(حرف **)b, -1));
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
    صحيح
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    صحيح		flags,		// EW_* flags
    صحيح		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    صحيح		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    صحيح		starts_with_dot;
    صحيح		matches;
    size_t	buflen;
    size_t	len;
    صحيح		starstar = FALSE;
    ساكن صحيح	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = عدم;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    صحيح			ok;
    char_u		*p_alt;
    صحيح			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    𐡄𐡍 (stardepth > 0)
    {
	ui_breakcheck();
	𐡄𐡍 (got_int)
	    أرجع 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    𐡄𐡍 (buf == عدم)
	أرجع 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = عدم;
    path_end = path;
    طالما (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	𐡄𐡍 (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	وإلا 𐡄𐡍 (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    𐡄𐡍 (e != عدم)
		اكسر;
	    s = p + 1;
	}
	وإلا 𐡄𐡍 (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != عدم)
	    e = p;
	𐡄𐡍 (has_mbyte)
	{
	    صحيح charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	وإلا
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    لكل (p = buf + wildoff; p < s; ++p)
	𐡄𐡍 (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    لكل (p = s; p < e; ++p)
	𐡄𐡍 (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, عدم, FALSE);
    𐡄𐡍 (pat == عدم)
    {
	vim_free(buf);
	أرجع 0;
    }

    // compile the regexp into a program
    𐡄𐡍 (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    𐡄𐡍 (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    𐡄𐡍 (regmatch.regprog == عدم && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	أرجع 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != عدم;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    𐡄𐡍 (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((حرف *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(فراغ)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((حرف *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, عدم);
    𐡄𐡍 (wn != عدم)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_VALUE);

    طالما (ok)
    {
	p = utf16_to_enc(wfb.cFileName, عدم);   // p is allocated here

	𐡄𐡍 (p == عدم)
	    اكسر;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	𐡄𐡍 (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = عدم;
	وإلا
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, عدم);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	𐡄𐡍 ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == عدم
		  || (regmatch.regprog != عدم
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != عدم
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((حرف *)s, buflen - len, "%s", p);
	    𐡄𐡍 (len + 1 < buflen)
	    {
		𐡄𐡍 (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((حرف *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (فراغ)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((حرف *)buf + len, buflen - len, "%s", path_end);
		𐡄𐡍 (mch_has_exp_wildcard(path_end))
		{
		    𐡄𐡍 (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(فراغ)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		وإلا
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    𐡄𐡍 (*path_end != 0)
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
    𐡄𐡍 (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   حجم(char_u *), pstrcmp);
    أرجع matches;
}
#endif // MSWIN

#𐡄𐡍 (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    ساكن صحيح
pstrcmp(ثابت فراغ *a, ثابت فراغ *b)
{
    أرجع (pathcmp(*(حرف **)a, *(حرف **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    صحيح
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    صحيح		flags,		// EW_* flags
    صحيح		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    صحيح		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    صحيح		starts_with_dot;
    صحيح		matches;
    size_t	buflen;
    size_t	len;
    صحيح		starstar = FALSE;
    ساكن صحيح	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    بنية dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    𐡄𐡍 (stardepth > 0)
    {
	ui_breakcheck();
	𐡄𐡍 (got_int)
	    أرجع 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    𐡄𐡍 (buf == عدم)
	أرجع 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = عدم;
    path_end = path;
    طالما (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	𐡄𐡍 (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	وإلا 𐡄𐡍 (*path_end == '/')
	{
	    𐡄𐡍 (e != عدم)
		اكسر;
	    s = p + 1;
	}
	وإلا 𐡄𐡍 (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != عدم
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	𐡄𐡍 (has_mbyte)
	{
	    صحيح charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	وإلا
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    لكل (p = buf + wildoff; p < s; ++p)
	𐡄𐡍 (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    لكل (p = s; p < e; ++p)
	𐡄𐡍 (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, عدم, FALSE);
    𐡄𐡍 (pat == عدم)
    {
	vim_free(buf);
	أرجع 0;
    }

    // compile the regexp into a program
    𐡄𐡍 (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    وإلا
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    𐡄𐡍 (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    𐡄𐡍 (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    𐡄𐡍 (regmatch.regprog == عدم && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	أرجع 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    𐡄𐡍 (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((حرف *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(فراغ)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (حرف *)buf);

    // Find all matching entries
    𐡄𐡍 (dirp != عدم)
    {
	طالما (!got_int)
	{
	    dp = readdir(dirp);
	    𐡄𐡍 (dp == عدم)
		اكسر;
	    len = (size_t)(s - buf);
	    𐡄𐡍 ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != عدم && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((حرف *)s, buflen - len, "%s", dp->d_name);
		𐡄𐡍 (len + 1 >= buflen)
		    استمر;

		𐡄𐡍 (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((حرف *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (فراغ)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((حرف *)buf + len, buflen - len, "%s", path_end);
		𐡄𐡍 (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    𐡄𐡍 (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(فراغ)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		وإلا
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    𐡄𐡍 (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    𐡄𐡍 ((flags & EW_ALLLINKS) ? mch_lstat((حرف *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			𐡄𐡍 (precomp_buf)
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
    𐡄𐡍 (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   حجم(char_u *), pstrcmp);
    أرجع matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    ساكن صحيح
has_env_var(char_u *p)
{
    لكل ( ; *p; MB_PTR_ADV(p))
    {
	𐡄𐡍 (*p == '\\' && p[1] != NUL)
	    ++p;
	وإلا 𐡄𐡍 (vim_strchr((char_u *)
#𐡄𐡍 defined(MSWIN)
				    "$%"
#وإلا
				    "$"
#endif
					, *p) != عدم)
	    أرجع TRUE;
    }
    أرجع FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    ساكن صحيح
has_special_wildchar(char_u *p)
{
    لكل ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	𐡄𐡍 (*p == '\r' || *p == '\n')
	    اكسر;
	// Allow for escaping.
	𐡄𐡍 (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	وإلا 𐡄𐡍 (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != عدم)
	{
	    // A { must be followed by a matching }.
	    𐡄𐡍 (*p == '{' && vim_strchr(p, '}') == عدم)
		استمر;
	    // A quote and backtick must be followed by another one.
	    𐡄𐡍 ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == عدم)
		استمر;
	    أرجع TRUE;
	}
    }
    أرجع FALSE;
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
    صحيح
gen_expand_wildcards(
    صحيح		num_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    صحيح		*num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    صحيح		flags)		// EW_* flags
{
    صحيح			i;
    garray_T		ga;
    char_u		*p;
    ساكن صحيح		recursive = FALSE;
    صحيح			add_pat;
    صحيح			retval = OK;
    صحيح			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    𐡄𐡍 (recursive)
#ifdef SPECIAL_WILDCHAR
	أرجع mch_expand_wildcards(num_pat, pat, num_file, file, flags);
#وإلا
	أرجع FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    لكل (i = 0; i < num_pat; i++)
    {
	𐡄𐡍 (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    أرجع mch_expand_wildcards(num_pat, pat, num_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, حجم(char_u *), 30);

    لكل (i = 0; i < num_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	𐡄𐡍 (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    𐡄𐡍 (add_pat == -1)
		retval = FAIL;
	}
	وإلا
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    𐡄𐡍 ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		𐡄𐡍 (p == عدم)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		وإلا 𐡄𐡍 (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(num_pat, pat, num_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    أرجع i;
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
	    𐡄𐡍 (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		𐡄𐡍 ((flags & (EW_PATH | EW_CDPATH))
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
		وإلا
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	𐡄𐡍 (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    𐡄𐡍 (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    وإلا
		addfile(&ga, t, flags);

	    𐡄𐡍 (t != p)
		vim_free(t);
	}

	𐡄𐡍 (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	𐡄𐡍 (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    𐡄𐡍 (retval == FAIL)
	ga_clear_strings(&ga);

    *num_file = ga.ga_len;
    *file = (ga.ga_data != عدم) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    أرجع ((flags & EW_EMPTYOK) || ga.ga_data != عدم) ? retval : FAIL;
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
    فراغ
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    صحيح		flags)
{
    char_u	*p;
    صحيح		isdir;
    stat_T	sb;

#ifdef MSWIN
    𐡄𐡍 (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	𐡄𐡍 (perm < 0)
	    أرجع;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    وإلا
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	𐡄𐡍 (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((حرف *)f, &sb) < 0 : mch_getperm(f) < 0))
	    أرجع;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    𐡄𐡍 (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != عدم)
	أرجع;
#endif

    𐡄𐡍 ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	أرجع;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    𐡄𐡍 (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, عدم, !(flags & EW_SHELLCMD)))
	أرجع;

    // Make room for another item in the file list.
    𐡄𐡍 (ga_grow(gap, 1) == FAIL)
	أرجع;

    p = alloc(STRLEN(f) + 1 + isdir);
    𐡄𐡍 (p == عدم)
	أرجع;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    𐡄𐡍 (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    فراغ
FreeWild(صحيح count, char_u **files)
{
    𐡄𐡍 (count <= 0 || files == عدم)
	أرجع;
    طالما (count--)
	vim_free(files[count]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    صحيح
pathcmp(ثابت حرف *p, ثابت حرف *q, صحيح maxlen)
{
    صحيح		i, j;
    صحيح		c1, c2;
    ثابت حرف	*s = عدم;

    لكل (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	𐡄𐡍 (c1 == NUL)
	{
	    𐡄𐡍 (c2 == NUL)  // full match
		أرجع 0;
	    s = q;
	    i = j;
	    اكسر;
	}

	// End of "q": check if "p" just has a slash.
	𐡄𐡍 (c2 == NUL)
	{
	    s = p;
	    اكسر;
	}

	𐡄𐡍 ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    𐡄𐡍 (vim_ispathsep(c1))
		أرجع -1;
	    𐡄𐡍 (vim_ispathsep(c2))
		أرجع 1;
	    أرجع p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    𐡄𐡍 (s == عدم)	// "i" or "j" ran into "maxlen"
	أرجع 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    𐡄𐡍 (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#وإلا
	    && c1 == '/'
#endif
       )
	أرجع 0;   // match with trailing slash
    𐡄𐡍 (s == q)
	أرجع -1;	    // no match
    أرجع 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    صحيح
vim_isAbsName(char_u *name)
{
    أرجع (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    صحيح		len,
    صحيح		force)	    // force expansion even when already absolute
{
    صحيح		retval = OK;
    صحيح		url;

    *buf = NUL;
    𐡄𐡍 (fname == عدم)
	أرجع FAIL;

    url = path_with_url(fname);
    𐡄𐡍 (!url)
	retval = mch_FullName(fname, buf, len, force);
    𐡄𐡍 (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#𐡄𐡍 defined(MSWIN)
    slash_adjust(buf);
#endif
    أرجع retval;
}
