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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * ଆକାର(*newbuf));
    ଯଦି (newbuf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    wfname = enc_to_utf16(*fnamep, ଖାଲି);
    ଯଦି (wfname == ଖାଲି)
    {
	vim_free(newbuf);
	ଫେରନ୍ତୁ FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    ଯଦି (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * ଆକାର(*newbuf));
	ଯଦି (newbuf == ଖାଲି)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    ଫେରନ୍ତୁ FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    ଯଦି (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, ଖାଲି);

	ଯଦି (p != ଖାଲି)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	ଅନ୍ୟଥା
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_unused = ଖାଲି;
    char_u	*short_fname = ଖାଲି;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    ଯଦି (save_fname == ଖାଲି)
    {
	retval = FAIL;
	ଯାଅ theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    ପାଇଁ (;;)
    {
	// go back one path-separator
	ଯେତେବେଳେ (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	ଯଦି (endp <= save_fname)
	    ଭାଙ୍ଗ;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	ଯଦି (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    ଯାଅ theend;
	}
	*endp = ch;	// preserve the string

	ଯଦି (len > 0)
	    ଭାଙ୍ଗ;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    ଯଦି (len > 0)
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
	ଯଦି (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    ଯଦି (*fname == ଖାଲି)
	    {
		retval = FAIL;
		ଯାଅ theend;
	    }
	}
	ଅନ୍ୟଥା
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    ଯଦି (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = ଖାଲି;
	}

	// concat the not-shortened part of the path
	ଯଦି ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    ଫେରନ୍ତୁ retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    ପାଇଁ (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	ଯଦି (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    ଯଦି (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    ଅନ୍ୟଥା
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    ଯଦି (tfname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    len = tflen = STRLEN(tfname);

    ଯଦି (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	ଯଦି (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    ପାଇଁ (p = tfname + len - 1; p >= tfname; --p)
    {
	ଯଦି (has_mbyte)
	    p -= mb_head_off(tfname, p);
	ଯଦି (vim_ispathsep(*p))
	{
	    ଯଦି (sepcount == 0 || (hasTilde && sepcount == 1))
		ଭାଙ୍ଗ;
	    ଅନ୍ୟଥା
		sepcount --;
	}
    }
    ଯଦି (hasTilde)
    {
	--p;
	ଯଦି (p >= tfname)
	    *p = '~';
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    ଫେରନ୍ତୁ OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
modify_fname(
    char_u	*src,		// string with modifiers
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_fullname = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    ଯଦି (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	ଯଦି ((*fnamep)[0] == '~'
#ଯଦି !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    ଯଦି (*fnamep == ଖାଲି)
		ଫେରନ୍ତୁ -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	ପାଇଁ (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    ଯଦି (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		ଭାଙ୍ଗ;
	}

	// FullName_save() is slow, don't use it when not needed.
	ଯଦି (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ଯଦି (*fnamep == ଖାଲି)
		ଫେରନ୍ତୁ -1;
	}

#ifdef MSWIN
# ଯଦି _WIN32_WINNT >= 0x0500
	ଯଦି (vim_strchr(*fnamep, '~') != ଖାଲି)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, ଖାଲି);
	    WCHAR buf[_MAX_PATH];

	    ଯଦି (wfname != ଖାଲି)
	    {
		ଯଦି (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, ଖାଲି);

		    ଯଦି (q != ଖାଲି)
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
	ଯଦି (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ଯଦି (*fnamep == ଖାଲି)
		ଫେରନ୍ତୁ -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    ଯେତେବେଳେ (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	ଯଦି (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    ଜାରି;
	}
	pbuf = ଖାଲି;
	// Need full path first (use expand_env() to remove a "~/")
	ଯଦି (!has_fullname && !has_homerelative)
	{
	    ଯଦି (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    ଅନ୍ୟଥା
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	ଅନ୍ୟଥା
	    p = *fnamep;

	has_fullname = 0;

	ଯଦି (p != ଖାଲି)
	{
	    size_t  dirnamelen = 0;

	    ଯଦି (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		ଯଦି (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    ଯଦି (s != ଖାଲି)
		    {
			dirnamelen = home_replace(ଖାଲି, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		ଯଦି (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		ଯଦି (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    ଯଦି (vim_ispathsep(*p))
		    {
			ଯେତେବେଳେ (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			ଯଦି (pbuf != ଖାଲି)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = ଖାଲି;
			}
		    }
		}
	    }
	    ଅନ୍ୟଥା
	    {
		dirnamelen = home_replace(ଖାଲି, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		ଯଦି (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    ଯଦି (s != ଖାଲି)
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
    ଯେତେବେଳେ (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	ଯେତେବେଳେ (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	ଯଦି (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	ଯଦି (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    ଯଦି (p == ଖାଲି)
		ଫେରନ୍ତୁ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	ଅନ୍ୟଥା
	{
	    ଯେତେବେଳେ (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    ଯଦି (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    ଯଦି (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	ଯଦି (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    ଯଦି (p == ଖାଲି)
		ଫେରନ୍ତୁ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	ଯଦି (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    ଯଦି (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    ଯଦି (get_short_pathname(fnamep, bufp, &l) == FAIL)
		ଫେରନ୍ତୁ -1;

	    ଯଦି (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		ଯଦି (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    ଫେରନ୍ତୁ -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    ଯଦି (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    ଯେତେବେଳେ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	ଯଦି (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	ଅନ୍ୟଥା
	    s = *fnamep + *fnamelen - 1;
	ପାଇଁ ( ; s > tail; --s)
	    ଯଦି (s[0] == '.')
		ଭାଙ୍ଗ;
	ଯଦି (src[*usedlen + 1] == 'e')		// :e
	{
	    ଯଦି (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		ପାଇଁ ( ; s > *fnamep; --s)
		    ଯଦି (s[0] == ';')
			ଭାଙ୍ଗ;
		ଯଦି (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    ଅନ୍ୟଥା ଯଦି (*fnamep <= tail)
		*fnamelen = 0;
	}
	ଅନ୍ୟଥା				// :r
	{
	    char_u *limit = *fnamep;

	    ଯଦି (limit < tail)
		limit = tail;
	    ଯଦି (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    ଯଦି (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sep;
	char_u	    *flags;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	ଯଦି (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	ଯଦି (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    ଯଦି (p != ଖାଲି)
	    {
		pat = vim_strnsave(s, p - s);
		ଯଦି (pat != ଖାଲି)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    ଯଦି (p != ଖାଲି)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			ଯଦି (sub != ଖାଲି && str != ଖାଲି)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, ଖାଲି, flags, &slen);
			    ଯଦି (s != ଖାଲି)
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
	    ଯଦି (didit)
		ଯାଅ repeat;
	}
    }

    ଯଦି (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	ଯଦି (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	ଯଦି (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    ଫେରନ୍ତୁ valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
shorten_dir_len(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା trim_len)
{
    char_u	*tail, *s, *d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    ପାଇଁ (s = str; ; ++s)
    {
	ଯଦି (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    ଯଦି (*s == NUL)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	ଅନ୍ୟଥା ଯଦି (!skip)
	{
	    *d++ = *s;			// copy next char
	    ଯଦି (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		ଯଦି (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    ଯଦି (has_mbyte)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = mb_ptr2len(s);

		ଯେତେବେଳେ (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    ଶୂନ୍ୟ
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
file_is_readable(char_u *fname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    ଯଦି (*fname == NUL || mch_isdir_or_missing(fname))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି ((fd = mch_open((ଅକ୍ଷର *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

#ଯଦି defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    ଶୂନ୍ୟ
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	ଯଦି (in_vim9script())
	    (ଶୂନ୍ୟ) check_for_string_arg(argvars, 0);
	ଫେରନ୍ତୁ;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    ଯଦି (cwd != ଖାଲି)
    {
	ଯଦି (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	ଯଦି (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	ଅନ୍ୟଥା ଯଦି (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	ଅନ୍ୟଥା ଯଦି (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	ଅନ୍ୟଥା
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "scope", s);
	    ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_localdir != ଖାଲି)
	scope = CDSCOPE_WINDOW;
    ଅନ୍ୟଥା ଯଦି (curtab->tp_localdir != ଖାଲି)
	scope = CDSCOPE_TABPAGE;

    ଯଦି (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    ଶୂନ୍ୟ
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);
    ଯଦି (name == ଖାଲି || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    ଅନ୍ୟଥା
	flags = (char_u *)"";

    ଯଦି (*flags == NUL)
	// delete a file
	rettv->vval.v_ସଂଖ୍ୟା = mch_remove(name) == 0 ? 0 : -1;
    ଅନ୍ୟଥା ଯଦି (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_ସଂଖ୍ୟା = mch_rmdir(name) == 0 ? 0 : -1;
    ଅନ୍ୟଥା ଯଦି (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_ସଂଖ୍ୟା = delete_recursive(name);
    ଅନ୍ୟଥା
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    ଶୂନ୍ୟ
f_executable(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_ସଂଖ୍ୟା = mch_can_exe(tv_get_string(&argvars[0]), ଖାଲି, TRUE);
}

/*
 * "exepath()" function
 */
    ଶୂନ୍ୟ
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = ଖାଲି;

    ଯଦି (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;
    (ଶୂନ୍ୟ)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    ଶୂନ୍ୟ
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;
    rettv->vval.v_ସଂଖ୍ୟା = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    ଶୂନ୍ୟ
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;
    rettv->vval.v_ସଂଖ୍ୟା = filewritable(tv_get_string(&argvars[0]));
}

    ସ୍ଥିର ଶୂନ୍ୟ
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		find_what)
{
    char_u	*fname;
    char_u	*fresult = ଖାଲି;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    rettv->vval.v_string = ଖାଲି;
    rettv->v_type = VAR_STRING;
    ଯଦି (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    fname = tv_get_string(&argvars[0]);

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	ଯଦି (p == ଖାଲି)
	    error = TRUE;
	ଅନ୍ୟଥା
	{
	    ଯଦି (*p != NUL)
		path = p;

	    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
		ଗଣନା_2 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	}
    }

    ଯଦି (ଗଣନା_2 < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    ଯଦି (*fname != NUL && !error)
    {
	char_u	*file_to_find = ଖାଲି;
	ଅକ୍ଷର	*ସନ୍ଧାନ_ctx = ଖାଲି;

	କର
	{
	    ଯଦି (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : ଖାଲି,
					       first ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &ସନ୍ଧାନ_ctx);
	    first = FALSE;

	    ଯଦି (fresult != ଖାଲି && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} ଯେତେବେଳେ ((rettv->v_type == VAR_LIST || --ଗଣନା_2 > 0) && fresult != ଖାଲି);

	vim_free(file_to_find);
	vim_findfile_cleanup(ସନ୍ଧାନ_ctx);
    }

    ଯଦି (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    ଶୂନ୍ୟ
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    ଶୂନ୍ୟ
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    ଶୂନ୍ୟ
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = ଖାଲି;
    char_u	buf[NUMBUFLEN];

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    ଯଦି (mods == ଖାଲି || fname == ଖାଲି)
	fname = ଖାଲି;
    ଅନ୍ୟଥା
    {
	len = STRLEN(fname);
	ଯଦି (mods != ଖାଲି && *mods != NUL)
	    (ଶୂନ୍ୟ)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    ଯଦି (fname == ଖାଲି)
	rettv->vval.v_string = ଖାଲି;
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = ଖାଲି;
    tabpage_T	*tp = ଖାଲି;
    char_u	*cwd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_ସଂଖ୍ୟା
	    && argvars[0].vval.v_ସଂଖ୍ୟା == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    ଅନ୍ୟଥା
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    ଯଦି (wp != ଖାଲି && wp->w_localdir != ଖାଲି
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    ଅନ୍ୟଥା ଯଦି (tp != ଖାଲି && tp->tp_localdir != ଖାଲି
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    ଅନ୍ୟଥା ଯଦି (wp != ଖାଲି || tp != ଖାଲି || global)
    {
	ଯଦି (globaldir != ଖାଲି && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	ଅନ୍ୟଥା
	{
	    cwd = alloc(MAXPATHL);
	    ଯଦି (cwd != ଖାଲି)
	    {
		ଯଦି (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    ଯଦି (rettv->vval.v_string != ଖାଲି)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;

    ପାଇଁ (i = 0; i < 9; i++)
    {
	ଯଦି (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	ଅନ୍ୟଥା
	    perm[i] = '-';
    }
    ଫେରନ୍ତୁ perm;
}

/*
 * "getfperm({fname})" function
 */
    ଶୂନ୍ୟ
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = ଖାଲି;
    char_u	permbuf[] = "---------";

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ଯଦି (mch_stat((ଅକ୍ଷର *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    ଶୂନ୍ୟ
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    fname = tv_get_string(&argvars[0]);
    ଯଦି (mch_stat((ଅକ୍ଷର *)fname, &st) >= 0)
    {
	ଯଦି (S_ISDIR(st.st_mode))
	    rettv->vval.v_ସଂଖ୍ୟା = 0;
	ଅନ୍ୟଥା
	{
	    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    ଯଦି ((off_T)rettv->vval.v_ସଂଖ୍ୟା != (off_T)st.st_size)
		rettv->vval.v_ସଂଖ୍ୟା = -2;
	}
    }
    ଅନ୍ୟଥା
	  rettv->vval.v_ସଂଖ୍ୟା = -1;
}

/*
 * "getftime({fname})" function
 */
    ଶୂନ୍ୟ
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    fname = tv_get_string(&argvars[0]);
    ଯଦି (mch_stat((ଅକ୍ଷର *)fname, &st) >= 0)
	rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)st.st_mtime;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    ଅକ୍ଷର    *t;

    ଯଦି (S_ISREG(st->st_mode))
	t = "file";
    ଅନ୍ୟଥା ଯଦି (S_ISDIR(st->st_mode))
	t = "dir";
    ଅନ୍ୟଥା ଯଦି (S_ISLNK(st->st_mode))
	t = "link";
    ଅନ୍ୟଥା ଯଦି (S_ISBLK(st->st_mode))
	t = "bdev";
    ଅନ୍ୟଥା ଯଦି (S_ISCHR(st->st_mode))
	t = "cdev";
    ଅନ୍ୟଥା ଯଦି (S_ISFIFO(st->st_mode))
	t = "fifo";
    ଅନ୍ୟଥା ଯଦି (S_ISSOCK(st->st_mode))
	t = "socket";
    ଅନ୍ୟଥା
	t = "other";
    ଫେରନ୍ତୁ (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    ଶୂନ୍ୟ
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = ଖାଲି;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ଯଦି (mch_lstat((ଅକ୍ଷର *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    ଶୂନ୍ୟ
f_glob(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	ଫେରନ୍ତୁ;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	ଯଦି (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ଯଦି (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, ଖାଲି);
	    ଯଦି (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    ଯଦି (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	ଯଦି (p_wic)
	    options += WILD_ICASE;
	ଯଦି (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     ଖାଲି, options, WILD_ALL);
	ଅନ୍ୟଥା ଯଦି (rettv_list_alloc(rettv) == OK)
	{
	  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						ଖାଲି, options, WILD_ALL_KEEP);
	  ପାଇଁ (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    ଅନ୍ୟଥା
	rettv->vval.v_string = ଖାଲି;
}

/*
 * "glob2regpat()" function
 */
    ଶୂନ୍ୟ
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, in_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == ଖାଲି)
			 ? ଖାଲି : file_pat_to_reg_pat(pat, ଖାଲି, ଖାଲି, FALSE);
}

/*
 * "globpath()" function
 */
    ଶୂନ୍ୟ
f_globpath(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	ଫେରନ୍ତୁ;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	ଯଦି (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	ଯଦି (argvars[3].v_type != VAR_UNKNOWN)
	{
	    ଯଦି (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, ଖାଲି);
	    ଯଦି (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    ଯଦି (file != ଖାଲି && !error)
    {
	ga_init2(&ga, ଆକାର(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	ଯଦି (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	ଅନ୍ୟଥା ଯଦି (rettv_list_alloc(rettv) == OK)
	    ପାଇଁ (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    ଅନ୍ୟଥା
	rettv->vval.v_string = ଖାଲି;
}

/*
 * "isdirectory()" function
 */
    ଶୂନ୍ୟ
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    ଶୂନ୍ୟ
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mkdir_recurse(char_u *dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    ଯଦି (p <= get_past_head(dir))
	ଫେରନ୍ତୁ OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    ଯଦି (updir == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (mch_isdir(updir))
	r = OK;
    ଅନ୍ୟଥା ଯଦି (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	ଯଦି (r == OK && created != ଖାଲି && *created == ଖାଲି)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    ଫେରନ୍ତୁ r;
}

/*
 * "mkdir()" function
 */
    ଶୂନ୍ୟ
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prot = 0755;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		defer = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		defer_recurse = FALSE;
    char_u	*created = ଖାଲି;

    rettv->vval.v_ସଂଖ୍ୟା = FAIL;
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    dir = tv_get_string_buf(&argvars[0], buf);
    ଯଦି (*dir == NUL)
	ଫେରନ୍ତୁ;

    ଯଦି (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], ଖାଲି);
	    ଯଦି (prot == -1)
		ଫେରନ୍ତୁ;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != ଖାଲି;
	defer_recurse = vim_strchr(arg2, 'R') != ଖାଲି;
	ଯଦି ((defer || defer_recurse) && !can_add_defer())
	    ଫେରନ୍ତୁ;

	ଯଦି (vim_strchr(arg2, 'p') != ଖାଲି)
	{
	    ଯଦି (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_ସଂଖ୍ୟା = OK;
		ଫେରନ୍ତୁ;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : ଖାଲି);
	}
    }
    rettv->vval.v_ସଂଖ୍ୟା = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    ଯଦି (rettv->vval.v_ସଂଖ୍ୟା == OK
				&& created == ଖାଲି && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    ଯଦି (created != ଖାଲି)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	ଯଦି (tv[0].vval.v_string == ଖାଲି || tv[1].vval.v_string == ଖାଲି
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
    ଶୂନ୍ୟ
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trim_len = 1;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[1]);
	ଯଦି (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    ଯଦି (p == ଖାଲି)
	rettv->vval.v_string = ଖାଲି;
    ଅନ୍ୟଥା
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	ଯଦି (p != ଖାଲି)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
checkitem_common(ଶୂନ୍ୟ *context, char_u *name, dict_T *dict)
{
    typval_T	*ପଦସମଷ୍ଟି = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    ଯଦି (name != ଖାଲି)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    ଅନ୍ୟଥା
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    ଯଦି (eval_ପଦସମଷ୍ଟି_typval(ପଦସମଷ୍ଟି, FALSE, argv, 1, ଖାଲି, &rettv) == FAIL)
	ଯାଅ theend;

    // We want to use -1, but also true/false should be allowed.
    ଯଦି (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_ସଂଖ୍ୟା;
	rettv.vval.v_ସଂଖ୍ୟା = rettv.vval.v_ସଂଖ୍ୟା == VVAL_TRUE;
    }
    retval = tv_get_ସଂଖ୍ୟା_chk(&rettv, &error);
    ଯଦି (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    ଯଦି (name != ଖାଲି)
	set_vim_var_string(VV_VAL, ଖାଲି, 0);
    ଅନ୍ୟଥା
	set_vim_var_dict(VV_VAL, ଖାଲି);
    restore_vimvar(VV_VAL, &save_val);
    ଫେରନ୍ତୁ retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
readdir_checkitem(ଶୂନ୍ୟ *context, ଶୂନ୍ୟ *item)
{
    char_u	*name = (char_u *)item;

    ଫେରନ୍ତୁ checkitem_common(context, name, ଖାଲି);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
readdirex_dict_arg(typval_T *argvars, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cmp)
{
    char_u     *compare;

    ଯଦି (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (dict_has_ଚାବି(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    ଅନ୍ୟଥା
    {
	semsg(_(e_dictionary_ଚାବି_str_required), "sort");
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    ଅନ୍ୟଥା ଯଦି (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    ଅନ୍ୟଥା ଯଦି (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    ଅନ୍ୟଥା ଯଦି (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    ଫେରନ୍ତୁ OK;
}

/*
 * "readdir()" function
 */
    ଶୂନ୍ୟ
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*ପଦସମଷ୍ଟି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sort = READDIR_SORT_BYTE;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    path = tv_get_string(&argvars[0]);
    ପଦସମଷ୍ଟି = &argvars[1];

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ଫେରନ୍ତୁ;

    ret = readdir_core(&ga, path, FALSE, (ଶୂନ୍ୟ *)ପଦସମଷ୍ଟି,
	    (ପଦସମଷ୍ଟି->v_type == VAR_UNKNOWN) ? ଖାଲି : readdir_checkitem, sort);
    ଯଦି (ret == OK)
    {
	ପାଇଁ (i = 0; i < ga.ga_len; i++)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
readdirex_checkitem(ଶୂନ୍ୟ *context, ଶୂନ୍ୟ *item)
{
    dict_T	*dict = (dict_T*)item;

    ଫେରନ୍ତୁ checkitem_common(context, ଖାଲି, dict);
}

/*
 * "readdirex()" function
 */
    ଶୂନ୍ୟ
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*ପଦସମଷ୍ଟି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    char_u	*path;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sort = READDIR_SORT_BYTE;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    path = tv_get_string(&argvars[0]);
    ପଦସମଷ୍ଟି = &argvars[1];

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ଫେରନ୍ତୁ;

    ret = readdir_core(&ga, path, TRUE, (ଶୂନ୍ୟ *)ପଦସମଷ୍ଟି,
	    (ପଦସମଷ୍ଟି->v_type == VAR_UNKNOWN) ? ଖାଲି : readdirex_checkitem, sort);
    ଯଦି (ret == OK)
    {
	ପାଇଁ (i = 0; i < ga.ga_len; i++)
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
    ସ୍ଥିର ଶୂନ୍ୟ
read_file_or_blob(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା always_blob)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଦ୍ୱିମିକ = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		blob = always_blob;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		io_size = ଆକାର(buf);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		readlen;		// size of last fread()
    char_u	*prev	 = ଖାଲି;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	ଯଦି (always_blob)
	{
	    offset = (off_T)tv_get_ସଂଖ୍ୟା(&argvars[1]);
	    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_ସଂଖ୍ୟା(&argvars[2]);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		ଦ୍ୱିମିକ = TRUE;
	    ଯଦି (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_ସଂଖ୍ୟା(&argvars[2]);
	}
    }

    ଯଦି ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	ଫେରନ୍ତୁ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    ଯଦି (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (*fname == NUL || (fd = mch_fopen((ଅକ୍ଷର *)fname, READBIN)) == ଖାଲି)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (blob)
    {
	ଯଦି (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	ଫେରନ୍ତୁ;
    }

    ଯେତେବେଳେ (cnt < maxline || maxline < 0)
    {
	readlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	ପାଇଁ (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || ଦ୍ୱିମିକ));
		++p)
	{
	    ଯଦି (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= ଖାଲି;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		ଯଦି (readlen > 0 && !ଦ୍ୱିମିକ)
		{
		    ଯେତେବେଳେ (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    ଯଦି (len == 0)
			ଯେତେବେଳେ (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		ଯଦି (prevlen == 0)
		    s = vim_strnsave(start, len);
		ଅନ୍ୟଥା
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    ଯଦି ((s = vim_realloc(prev, prevlen + len + 1)) != ଖାଲି)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = ଖାଲି; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		ଯଦି (s == ଖାଲି)
		{
		    do_outofmem_ସନ୍ଦେଶ((long_u) prevlen + len + 1);
		    failed = TRUE;
		    ଭାଙ୍ଗ;
		}

		ଯଦି ((li = listitem_alloc()) == ଖାଲି)
		{
		    vim_free(s);
		    failed = TRUE;
		    ଭାଙ୍ଗ;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		ଯଦି ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    ଅନ୍ୟଥା ଯଦି (*p == 0xbf && enc_utf8 && !ଦ୍ୱିମିକ)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		ଯଦି (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    ଯଦି (start == dest)
			start = p + 1;
		    ଅନ୍ୟଥା
		    {
			// have to shuffle buf to close gap
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା adjust_prevlen = 0;

			ଯଦି (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(buf - dest);
			    dest = buf;
			}
			ଯଦି (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	ଯଦି (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    ଭାଙ୍ଗ;
	ଯଦି (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    ଯଦି (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		ଯଦି (prevsize == 0)
		    prevsize = (long)(p - start);
		ଅନ୍ୟଥା
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		ଯଦି (newprev == ଖାଲି)
		{
		    do_outofmem_ସନ୍ଦେଶ((long_u)prevsize);
		    failed = TRUE;
		    ଭାଙ୍ଗ;
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
    ଯଦି (!failed && maxline < 0)
	ଯେତେବେଳେ (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    ଯଦି (failed)
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
    ଶୂନ୍ୟ
f_readblob(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    ଶୂନ୍ୟ
f_readfile(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)))
	ଫେରନ୍ତୁ;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    ଶୂନ୍ୟ
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = ଖାଲି;
# endif

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = ଖାଲି;

	v = mch_resolve_path(p, TRUE);
	ଯଦି (v != ଖାଲି)
	    rettv->vval.v_string = v;
	ଅନ୍ୟଥା
	    rettv->vval.v_string = vim_strsave(p);
    }
# ଅନ୍ୟଥା
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;
	char_u	*remain = ଖାଲି;
	char_u	*q;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_relative_to_current = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_trailing_pathsep = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	limit = 100;

	p = vim_strsave(p);
	ଯଦି (p == ଖାଲି)
	    ଯାଅ fail;
	ଯଦି (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	ଯଦି (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	ଯଦି (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	ଯଦି (buf == ଖାଲି)
	{
	    vim_free(p);
	    vim_free(remain);
	    ଯାଅ fail;
	}

	ପାଇଁ (;;)
	{
	    ପାଇଁ (;;)
	    {
		len = readlink((ଅକ୍ଷର *)p, (ଅକ୍ଷର *)buf, MAXPATHL);
		ଯଦି (len <= 0)
		    ଭାଙ୍ଗ;
		buf[len] = NUL;

		ଯଦି (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = ଖାଲି;
		    ଯାଅ fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		ଯଦି (remain == ଖାଲି && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		ଯଦି (*q != NUL)
		{
		    ଯଦି (remain == ଖାଲି)
			remain = vim_strsave(q - 1);
		    ଅନ୍ୟଥା
		    {
			cpy = concat_str(q - 1, remain);
			ଯଦି (cpy != ଖାଲି)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		ଯଦି (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		ଯଦି (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    ଯଦି (cpy != ଖାଲି)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		ଅନ୍ୟଥା
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    ଯଦି (remain == ଖାଲି)
		ଭାଙ୍ଗ;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    ଯଦି (cpy != ଖାଲି)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    ଯଦି (*q != NUL)
		STRMOVE(remain, q - 1);
	    ଅନ୍ୟଥା
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	ଯଦି (!vim_ispathsep(*p))
	{
	    ଯଦି (is_relative_to_current
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
		ଯଦି (cpy != ଖାଲି)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		ଯେତେବେଳେ (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		ଯଦି (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	ଯଦି (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    ଯଦି (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
f_tempname(typval_T *argvars UNUSED, typval_T *rettv)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    କର
    {
	ଯଦି (x == 'Z')
	    x = '0';
	ଅନ୍ୟଥା ଯଦି (x == '9')
	    x = 'A';
	ଅନ୍ୟଥା
	    ++x;
    } ଯେତେବେଳେ (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    ଶୂନ୍ୟ
f_writefile(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଦ୍ୱିମିକ = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		append = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		defer = FALSE;
# ifdef HAVE_FSYNC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = 0;
    listitem_T	*li;
    list_T	*list = ଖାଲି;
    blob_T	*blob = ଖାଲି;

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	ଯଦି (list == ଖାଲି)
	    ଫେରନ୍ତୁ;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(list, li)
	    ଯଦି (tv_get_string_chk(&li->li_tv) == ଖାଲି)
		ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	ଯଦି (blob == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	ଯଦି (arg2 == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି (vim_strchr(arg2, 'b') != ଖାଲି)
	    ଦ୍ୱିମିକ = TRUE;
	ଯଦି (vim_strchr(arg2, 'a') != ଖାଲି)
	    append = TRUE;
	ଯଦି (vim_strchr(arg2, 'D') != ଖାଲି)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	ଯଦି (vim_strchr(arg2, 's') != ଖାଲି)
	    do_fsync = TRUE;
	ଅନ୍ୟଥା ଯଦି (vim_strchr(arg2, 'S') != ଖାଲି)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (defer && !can_add_defer())
	ଫେରନ୍ତୁ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    ଯଦି (*fname == NUL || (fd = mch_fopen((ଅକ୍ଷର *)fname,
				      append ? APPENDBIN : WRITEBIN)) == ଖାଲି)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    ଯଦି (tv.vval.v_string == ଖାଲି
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(ଶୂନ୍ୟ)mch_remove(fname);
	    }
	}

	ଯଦି (ret == 0)
	{
	    ଯଦି (blob)
	    {
		ଯଦି (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (write_list(fd, list, ଦ୍ୱିମିକ) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    ଯଦି (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_ସଂଖ୍ୟା = ret;
}

#endif // FEAT_EVAL

#ଯଦି defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    ସ୍ଥିର char_u	*last_dir = ଖାଲି;    // last used directory
    char_u		*tofree = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    ଯଦି (title == ଖାଲି || *title == NUL)
    {
	ଯଦି (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	ଅନ୍ୟଥା ଯଦି (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	ଅନ୍ୟଥା
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    ଯଦି ((initdir == ଖାଲି || *initdir == NUL) && dflt != ଖାଲି && *dflt != NUL)
    {
	ଯଦି (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    ଯଦି (tofree != ଖାଲି)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    ଯଦି (initdir == ଖାଲି || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	ଯଦି (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	ଅନ୍ୟଥା ଯଦି (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != ଖାଲି && buf->b_ffname != ଖାଲି)
	{
	    ଯଦି (dflt == ଖାଲି || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    ଯଦି (tofree != ଖାଲି)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	ଅନ୍ୟଥା ଯଦି (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    ଯଦି (gui.in_use)		// when this changes, also adjust f_has()!
    {
	ଯଦି (filter == ଖାଲି
#  ifdef FEAT_EVAL
		&& (filter = get_var_ମୂଲ୍ୟ((char_u *)"b:browsefilter")) == ଖାଲି
		&& (filter = get_var_ମୂଲ୍ୟ((char_u *)"g:browsefilter")) == ଖାଲି
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	ଯଦି (flags & BROWSE_DIR)
	{
#  ଯଦି defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  ଅନ୍ୟଥା
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  ଯଦି !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    ଯଦି (fname != ଖାଲି && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		ଯଦି (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	ଅନ୍ୟଥା
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    ଅନ୍ୟଥା
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = ଖାଲି;
    }

    // keep the directory for next time
    ଯଦି (fname != ଖାଲି)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	ଯଦି (last_dir != ଖାଲି && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    ଯଦି (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		ଯଦି (last_dir != ଖାଲି)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    ଫେରନ୍ତୁ fname;
}
#endif

#ଯଦି defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    ଶୂନ୍ୟ
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ଫେରନ୍ତୁ;

    save = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    ଯଦି (error || title == ଖାଲି || initdir == ଖାଲି || defname == ଖାଲି)
	rettv->vval.v_string = ଖାଲି;
    ଅନ୍ୟଥା
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, ଖାଲି, initdir, ଖାଲି, curbuf);
# ଅନ୍ୟଥା
    rettv->vval.v_string = ଖାଲି;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    ଶୂନ୍ୟ
f_browsedir(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    ଯଦି (title == ଖାଲି || initdir == ଖାଲି)
	rettv->vval.v_string = ଖାଲି;
    ଅନ୍ୟଥା
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, ଖାଲି, ଖାଲି, initdir, ଖାଲି, curbuf);
# ଅନ୍ୟଥା
    rettv->vval.v_string = ଖାଲି;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    ଶୂନ୍ୟ
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_ସଂଖ୍ୟା = FALSE;

    ଯଦି (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    from = tv_get_string(&argvars[0]);

    ଯଦି (mch_lstat((ଅକ୍ଷର *)from, &st) >= 0
	&& (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)))
	rettv->vval.v_ସଂଖ୍ୟା = vim_copyfile(
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dstlen,	// maximum length of the result
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    ଯଦି (src == ଖାଲି)
    {
	*dst = NUL;
	ଫେରନ୍ତୁ 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    ଯଦି (buf != ଖାଲି && buf->b_help)
	ଫେରନ୍ତୁ vim_snprintf_safelen((ଅକ୍ଷର *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    ଯଦି (homedir != ଖାଲି)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#ଅନ୍ୟଥା
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    ଯଦି (homedir_env == ଖାଲି)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    ଯଦି (homedir_env != ଖାଲି && *homedir_env == NUL)
	homedir_env = ଖାଲି;

    ଯଦି (homedir_env != ଖାଲି && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = ଖାଲି;

	flen = STRLEN(homedir_env);
	(ଶୂନ୍ୟ)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	ଯଦି (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    ଯଦି (homedir_env != ଖାଲି)
	envlen = STRLEN(homedir_env);

    ଯଦି (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    ଯେତେବେଳେ (*src && dstlen > 0)
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
	ପାଇଁ (;;)
	{
	    ଯଦି (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		ଯଦି (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (p == homedir_env)
		ଭାଙ୍ଗ;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	ଯେତେବେଳେ (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	ଯେତେବେଳେ ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    ଯଦି (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    ଫେରନ୍ତୁ (size_t)(dst - dst_start);
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
    ଯଦି (src != ଖାଲି)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    ଯଦି (dst != ଖାଲି)
	home_replace(buf, src, dst, len, TRUE);
    ଫେରନ୍ତୁ dst;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fullpathcmp(
    char_u *s1,
    char_u *s2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    checkname,		// when both don't exist, check file names
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    r1, r2;

    ଯଦି (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    ଅନ୍ୟଥା
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((ଅକ୍ଷର *)exp1, &st1);
    r2 = mch_stat((ଅକ୍ଷର *)s2, &st2);
    ଯଦି (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	ଯଦି (checkname)
	{
	    ଯଦି (fnamecmp(exp1, s2) == 0)
		ଫେରନ୍ତୁ FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    ଯଦି (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		ଫେରନ୍ତୁ FPC_SAMEX;
	}
	ଫେରନ୍ତୁ FPC_NOTX;
    }
    ଯଦି (r1 != 0 || r2 != 0)
	ଫେରନ୍ତୁ FPC_DIFFX;
    ଯଦି (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	ଫେରନ୍ତୁ FPC_SAME;
    ଫେରନ୍ତୁ FPC_DIFF;
#ଅନ୍ୟଥା
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval = FPC_DIFF;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    ଯଦି ((exp1 = alloc(MAXPATHL * 3)) != ଖାଲି)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	ଯଦି (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	ଅନ୍ୟଥା
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	ଯଦି (r1 != OK && r2 != OK)
	{
	    ଯଦି (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    ଅନ୍ୟଥା
		retval = FPC_NOTX;
	}
	ଅନ୍ୟଥା ଯଦି (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	ଅନ୍ୟଥା ଯଦି (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	ଅନ୍ୟଥା
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    ଫେରନ୍ତୁ retval;
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

    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ (char_u *)"";
    ପାଇଁ (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	ଯଦି (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    ଫେରନ୍ତୁ p1;
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
    ଯେତେବେଳେ (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    ଫେରନ୍ତୁ t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    ଯେତେବେଳେ (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    ଯଦି (*fname)
	++fname;
    ଫେରନ୍ତୁ fname;
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

#ଯଦି defined(MSWIN)
    // may skip "c:"
    ଯଦି (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    ଅନ୍ୟଥା
	retval = path;
#ଅନ୍ୟଥା
# ଯଦି defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    ଯଦି (retval == ଖାଲି)
	retval = path;
# ଅନ୍ୟଥା	// Unix
    retval = path;
# endif
#endif

    ଯେତେବେଳେ (vim_ispathsep(*retval))
	++retval;

    ଫେରନ୍ତୁ retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_ispathsep(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
#ifdef UNIX
    ଫେରନ୍ତୁ (c == '/');	    // UNIX has ':' inside file names
#ଅନ୍ୟଥା
# ifdef BACKSLASH_IN_FILENAME
    ଫେରନ୍ତୁ (c == ':' || c == '/' || c == '\\');
# ଅନ୍ୟଥା
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    ଫେରନ୍ତୁ (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_ispathsep_nocolon(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ vim_ispathsep(c)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;

    p = gettail_sep(fname);
    ଯଦି (p == fname)
	ଫେରନ୍ତୁ TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    ଫେରନ୍ତୁ retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    ଫେରନ୍ତୁ vim_fnamencmp(x, y, MAXPATHL);
#ଅନ୍ୟଥା
    ଯଦି (p_fic)
	ଫେରନ୍ତୁ MB_STRICMP(x, y);
    ଫେରନ୍ତୁ STRCMP(x, y);
#endif
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cx = NUL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	ଯଦି ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    ଯଦି (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    ଯେତେବେଳେ (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	ଯଦି (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    ଭାଙ୍ଗ;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    ଯଦି (len == 0)
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ (cx - cy);
#ଅନ୍ୟଥା
    ଯଦି (p_fic)
	ଫେରନ୍ତୁ MB_STRNICMP(x, y, len);
    ଫେରନ୍ତୁ STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? ଆକାର(PATHSEP) : 0) + fname2len + 1);
    ଯଦି (ret->string == ଖାଲି)
	ret->length = 0;
    ଅନ୍ୟଥା
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	ଯଦି (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += ଆକାର(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    ଫେରନ୍ତୁ ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    ଶୂନ୍ୟ
add_pathsep(char_u *p)
{
    size_t  plen;

    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ;

    plen = STRLEN(p);
    ଯଦି (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = ଖାଲି;

    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    buf = alloc(MAXPATHL);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    ଅନ୍ୟଥା
	new_fname = vim_strsave(fname);
    vim_free(buf);
    ଫେରନ୍ତୁ new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_fexists(char_u *fname)
{
    stat_T st;

    ଯଦି (mch_stat((ଅକ୍ଷର *)fname, &st))
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		  *ଅଙ୍କ_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		   flags)	// EW_DIR, etc.
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    char_u	*eval_pat = ଖାଲି;
    char_u	*exp_pat = *pat;
    ଅକ୍ଷର	*ignored_ସନ୍ଦେଶ;
    size_t	usedlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		star_follows = FALSE;

    ଯଦି (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       ଖାଲି, &ignored_ସନ୍ଦେଶ, ଖାଲି, TRUE);
	--emsg_off;
	ଯଦି (eval_pat != ଖାଲି)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    ଯଦି (exp_pat != ଖାଲି)
	ret = expand_wildcards(1, &exp_pat, ଅଙ୍କ_file, file, flags);

    ଯଦି (eval_pat != ଖାଲି)
    {
	ଯଦି (*ଅଙ୍କ_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    ଯଦି (*file != ଖାଲି)
	    {
		**file = eval_pat;
		eval_pat = ଖାଲି;
		*ଅଙ୍କ_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_wildcards(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		   ଅଙ୍କ_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		  *ଅଙ୍କ_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		   flags)	// EW_DIR, etc.
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(ଅଙ୍କ_pat, pat, ଅଙ୍କ_files, files, flags);

    // When keeping all matches, return here
    ଯଦି ((flags & EW_KEEPALL) || retval == FAIL)
	ଫେରନ୍ତୁ retval;

    /*
     * Remove names that match 'wildignore'.
     */
    ଯଦି (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	ପାଇଁ (i = 0; i < *ଅଙ୍କ_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    ଯଦି (ffname == ଖାଲି)		// out of memory
		ଭାଙ୍ଗ;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    ଯଦି (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		ପାଇଁ (j = i; j + 1 < *ଅଙ୍କ_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*ଅଙ୍କ_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	ଯଦି (*ଅଙ୍କ_files == 0)
	{
	    VIM_CLEAR(*files);
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    ଯଦି (*ଅଙ୍କ_files > 1 && !got_int)
    {
	non_suf_match = 0;
	ପାଇଁ (i = 0; i < *ଅଙ୍କ_files; ++i)
	{
	    ଯଦି (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		ପାଇଁ (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_suffix(char_u *fname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(fname);
    setsuflen = 0;
    ପାଇଁ (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	ଯଦି (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    ଯଦି (vim_strchr(tail, '.') == ଖାଲି)
	    {
		setsuflen = 1;
		ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		ଭାଙ୍ଗ;
	    setsuflen = 0;
	}
    }
    ଫେରନ୍ତୁ (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_backtick(char_u *p)
{
    ଫେରନ୍ତୁ (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    ଯଦି (cmd == ଖାଲି)
	ଫେରନ୍ତୁ -1;

#ifdef FEAT_EVAL
    ଯଦି (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    ଅନ୍ୟଥା
#endif
	buffer = get_cmd_output(cmd, ଖାଲି,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, ଖାଲି);
    vim_free(cmd);
    ଯଦି (buffer == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    cmd = buffer;
    ଯେତେବେଳେ (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	ଯେତେବେଳେ (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	ଯଦି (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	ଯେତେବେଳେ (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    ଫେରନ୍ତୁ cnt;
}

#ଯଦି defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pstrcmp(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b)
{
    ଫେରନ୍ତୁ (pathcmp(*(ଅକ୍ଷର **)a, *(ଅକ୍ଷର **)b, -1));
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// EW_* flags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		starts_with_dot;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matches;
    size_t	buflen;
    size_t	len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		starstar = FALSE;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_ମୂଲ୍ୟ;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = ଖାଲି;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ok;
    char_u		*p_alt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    ଯଦି (stardepth > 0)
    {
	ui_breakcheck();
	ଯଦି (got_int)
	    ଫେରନ୍ତୁ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ଖାଲି;
    path_end = path;
    ଯେତେବେଳେ (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ଯଦି (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	ଅନ୍ୟଥା ଯଦି (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    ଯଦି (e != ଖାଲି)
		ଭାଙ୍ଗ;
	    s = p + 1;
	}
	ଅନ୍ୟଥା ଯଦି (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != ଖାଲି)
	    e = p;
	ଯଦି (has_mbyte)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	ଅନ୍ୟଥା
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    ପାଇଁ (p = buf + wildoff; p < s; ++p)
	ଯଦି (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    ପାଇଁ (p = s; p < e; ++p)
	ଯଦି (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ଖାଲି, FALSE);
    ଯଦି (pat == ଖାଲି)
    {
	vim_free(buf);
	ଫେରନ୍ତୁ 0;
    }

    // compile the regexp into a program
    ଯଦି (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ଯଦି (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ଯଦି (regmatch.regprog == ଖାଲି && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ଫେରନ୍ତୁ 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != ଖାଲି;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ଯଦି (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((ଅକ୍ଷର *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(ଶୂନ୍ୟ)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((ଅକ୍ଷର *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, ଖାଲି);
    ଯଦି (wn != ଖାଲି)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_ମୂଲ୍ୟ);

    ଯେତେବେଳେ (ok)
    {
	p = utf16_to_enc(wfb.cFileName, ଖାଲି);   // p is allocated here

	ଯଦି (p == ଖାଲି)
	    ଭାଙ୍ଗ;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	ଯଦି (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = ଖାଲି;
	ଅନ୍ୟଥା
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, ଖାଲି);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	ଯଦି ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == ଖାଲି
		  || (regmatch.regprog != ଖାଲି
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != ଖାଲି
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((ଅକ୍ଷର *)s, buflen - len, "%s", p);
	    ଯଦି (len + 1 < buflen)
	    {
		ଯଦି (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((ଅକ୍ଷର *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (ଶୂନ୍ୟ)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((ଅକ୍ଷର *)buf + len, buflen - len, "%s", path_end);
		ଯଦି (mch_has_exp_wildcard(path_end))
		{
		    ଯଦି (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ଶୂନ୍ୟ)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ଅନ୍ୟଥା
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ଯଦି (*path_end != 0)
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
    ଯଦି (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   ଆକାର(char_u *), pstrcmp);
    ଫେରନ୍ତୁ matches;
}
#endif // MSWIN

#ଯଦି (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pstrcmp(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b)
{
    ଫେରନ୍ତୁ (pathcmp(*(ଅକ୍ଷର **)a, *(ଅକ୍ଷର **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// EW_* flags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		starts_with_dot;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matches;
    size_t	buflen;
    size_t	len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		starstar = FALSE;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    ସଂରଚନା dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    ଯଦି (stardepth > 0)
    {
	ui_breakcheck();
	ଯଦି (got_int)
	    ଫେରନ୍ତୁ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ଖାଲି;
    path_end = path;
    ଯେତେବେଳେ (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ଯଦି (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	ଅନ୍ୟଥା ଯଦି (*path_end == '/')
	{
	    ଯଦି (e != ଖାଲି)
		ଭାଙ୍ଗ;
	    s = p + 1;
	}
	ଅନ୍ୟଥା ଯଦି (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != ଖାଲି
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	ଯଦି (has_mbyte)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	ଅନ୍ୟଥା
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    ପାଇଁ (p = buf + wildoff; p < s; ++p)
	ଯଦି (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    ପାଇଁ (p = s; p < e; ++p)
	ଯଦି (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, ଖାଲି, FALSE);
    ଯଦି (pat == ଖାଲି)
    {
	vim_free(buf);
	ଫେରନ୍ତୁ 0;
    }

    // compile the regexp into a program
    ଯଦି (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    ଅନ୍ୟଥା
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ଯଦି (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ଯଦି (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ଯଦି (regmatch.regprog == ଖାଲି && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ଫେରନ୍ତୁ 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ଯଦି (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((ଅକ୍ଷର *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(ଶୂନ୍ୟ)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (ଅକ୍ଷର *)buf);

    // Find all matching entries
    ଯଦି (dirp != ଖାଲି)
    {
	ଯେତେବେଳେ (!got_int)
	{
	    dp = readdir(dirp);
	    ଯଦି (dp == ଖାଲି)
		ଭାଙ୍ଗ;
	    len = (size_t)(s - buf);
	    ଯଦି ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != ଖାଲି && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((ଅକ୍ଷର *)s, buflen - len, "%s", dp->d_name);
		ଯଦି (len + 1 >= buflen)
		    ଜାରି;

		ଯଦି (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((ଅକ୍ଷର *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (ଶୂନ୍ୟ)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((ଅକ୍ଷର *)buf + len, buflen - len, "%s", path_end);
		ଯଦି (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    ଯଦି (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ଶୂନ୍ୟ)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ଅନ୍ୟଥା
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ଯଦି (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    ଯଦି ((flags & EW_ALLLINKS) ? mch_lstat((ଅକ୍ଷର *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			ଯଦି (precomp_buf)
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
    ଯଦି (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   ଆକାର(char_u *), pstrcmp);
    ଫେରନ୍ତୁ matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_env_var(char_u *p)
{
    ପାଇଁ ( ; *p; MB_PTR_ADV(p))
    {
	ଯଦି (*p == '\\' && p[1] != NUL)
	    ++p;
	ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)
#ଯଦି defined(MSWIN)
				    "$%"
#ଅନ୍ୟଥା
				    "$"
#endif
					, *p) != ଖାଲି)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_special_wildchar(char_u *p)
{
    ପାଇଁ ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	ଯଦି (*p == '\r' || *p == '\n')
	    ଭାଙ୍ଗ;
	// Allow for escaping.
	ଯଦି (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != ଖାଲି)
	{
	    // A { must be followed by a matching }.
	    ଯଦି (*p == '{' && vim_strchr(p, '}') == ଖାଲି)
		ଜାରି;
	    // A quote and backtick must be followed by another one.
	    ଯଦି ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == ଖାଲି)
		ଜାରି;
	    ଫେରନ୍ତୁ TRUE;
	}
    }
    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gen_expand_wildcards(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// EW_* flags
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    garray_T		ga;
    char_u		*p;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		recursive = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			add_pat;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			retval = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    ଯଦି (recursive)
#ifdef SPECIAL_WILDCHAR
	ଫେରନ୍ତୁ mch_expand_wildcards(ଅଙ୍କ_pat, pat, ଅଙ୍କ_file, file, flags);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    ପାଇଁ (i = 0; i < ଅଙ୍କ_pat; i++)
    {
	ଯଦି (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    ଫେରନ୍ତୁ mch_expand_wildcards(ଅଙ୍କ_pat, pat, ଅଙ୍କ_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, ଆକାର(char_u *), 30);

    ପାଇଁ (i = 0; i < ଅଙ୍କ_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	ଯଦି (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    ଯଦି (add_pat == -1)
		retval = FAIL;
	}
	ଅନ୍ୟଥା
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    ଯଦି ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		ଯଦି (p == ଖାଲି)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		ଅନ୍ୟଥା ଯଦି (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(ଅଙ୍କ_pat, pat, ଅଙ୍କ_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    ଫେରନ୍ତୁ i;
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
	    ଯଦି (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		ଯଦି ((flags & (EW_PATH | EW_CDPATH))
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
		ଅନ୍ୟଥା
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	ଯଦି (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    ଯଦି (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    ଅନ୍ୟଥା
		addfile(&ga, t, flags);

	    ଯଦି (t != p)
		vim_free(t);
	}

	ଯଦି (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	ଯଦି (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    ଯଦି (retval == FAIL)
	ga_clear_strings(&ga);

    *ଅଙ୍କ_file = ga.ga_len;
    *file = (ga.ga_data != ଖାଲି) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    ଫେରନ୍ତୁ ((flags & EW_EMPTYOK) || ga.ga_data != ଖାଲି) ? retval : FAIL;
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
    ଶୂନ୍ୟ
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		isdir;
    stat_T	sb;

#ifdef MSWIN
    ଯଦି (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	ଯଦି (perm < 0)
	    ଫେରନ୍ତୁ;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    ଅନ୍ୟଥା
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	ଯଦି (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((ଅକ୍ଷର *)f, &sb) < 0 : mch_getperm(f) < 0))
	    ଫେରନ୍ତୁ;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    ଯଦି (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != ଖାଲି)
	ଫେରନ୍ତୁ;
#endif

    ଯଦି ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	ଫେରନ୍ତୁ;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    ଯଦି (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, ଖାଲି, !(flags & EW_SHELLCMD)))
	ଫେରନ୍ତୁ;

    // Make room for another item in the file list.
    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ;

    p = alloc(STRLEN(f) + 1 + isdir);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    ଯଦି (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    ଶୂନ୍ୟ
FreeWild(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, char_u **files)
{
    ଯଦି (ଗଣନା_2 <= 0 || files == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯେତେବେଳେ (ଗଣନା_2--)
	vim_free(files[ଗଣନା_2]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pathcmp(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *q, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1, c2;
    ଅଚଳ ଅକ୍ଷର	*s = ଖାଲି;

    ପାଇଁ (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	ଯଦି (c1 == NUL)
	{
	    ଯଦି (c2 == NUL)  // full match
		ଫେରନ୍ତୁ 0;
	    s = q;
	    i = j;
	    ଭାଙ୍ଗ;
	}

	// End of "q": check if "p" just has a slash.
	ଯଦି (c2 == NUL)
	{
	    s = p;
	    ଭାଙ୍ଗ;
	}

	ଯଦି ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    ଯଦି (vim_ispathsep(c1))
		ଫେରନ୍ତୁ -1;
	    ଯଦି (vim_ispathsep(c2))
		ଫେରନ୍ତୁ 1;
	    ଫେରନ୍ତୁ p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    ଯଦି (s == ଖାଲି)	// "i" or "j" ran into "maxlen"
	ଫେରନ୍ତୁ 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    ଯଦି (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#ଅନ୍ୟଥା
	    && c1 == '/'
#endif
       )
	ଫେରନ୍ତୁ 0;   // match with trailing slash
    ଯଦି (s == q)
	ଫେରନ୍ତୁ -1;	    // no match
    ଫେରନ୍ତୁ 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_isAbsName(char_u *name)
{
    ଫେରନ୍ତୁ (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force)	    // force expansion even when already absolute
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		url;

    *buf = NUL;
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    url = path_with_url(fname);
    ଯଦି (!url)
	retval = mch_FullName(fname, buf, len, force);
    ଯଦି (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#ଯଦି defined(MSWIN)
    slash_adjust(buf);
#endif
    ଫେରନ୍ତୁ retval;
}
