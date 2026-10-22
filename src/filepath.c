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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
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
    ਸਥਿਰ ਪੂਰਨ
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    ਪੂਰਨ		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(len * ਆਕਾਰ(*newbuf));
    ਜੇ (newbuf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    wfname = enc_ਮੰਜ਼ਿਲ_utf16(*fnamep, ਨਿਰਾ);
    ਜੇ (wfname == ਨਿਰਾ)
    {
	vim_free(newbuf);
	ਵਾਪਸ FAIL;
    }

    l = Get_Short_ਰਸਤਾ_Name_W(wfname, newbuf, len);
    ਜੇ (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * ਆਕਾਰ(*newbuf));
	ਜੇ (newbuf == ਨਿਰਾ)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    ਵਾਪਸ FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = Get_Short_ਰਸਤਾ_Name_W(wfname, newbuf, l+1);
    }
    ਜੇ (l != 0)
    {
	char_u *p = utf16_ਮੰਜ਼ਿਲ_enc(newbuf, ਨਿਰਾ);

	ਜੇ (p != ਨਿਰਾ)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	ਨਹੀਂਤਾਂ
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    ਵਾਪਸ FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_ਅਣਵਰਤਿਆ = ਨਿਰਾ;
    char_u	*short_fname = ਨਿਰਾ;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    ਪੂਰਨ		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    ਜੇ (save_fname == ਨਿਰਾ)
    {
	retval = FAIL;
	ਜਾਓ theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    ਲਈ (;;)
    {
	// go back one path-separator
	ਜਦੋਂ (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	ਜੇ (endp <= save_fname)
	    ਰੋਕੋ;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	ਜੇ (get_short_pathname(&short_fname, &pbuf_ਅਣਵਰਤਿਆ, &len) == FAIL)
	{
	    retval = FAIL;
	    ਜਾਓ theend;
	}
	*endp = ch;	// preserve the string

	ਜੇ (len > 0)
	    ਰੋਕੋ;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    ਜੇ (len > 0)
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
	ਜੇ (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    ਜੇ (*fname == ਨਿਰਾ)
	    {
		retval = FAIL;
		ਜਾਓ theend;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    ਜੇ (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = ਨਿਰਾ;
	}

	// concat the not-shortened part of the path
	ਜੇ ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_ਅਣਵਰਤਿਆ);
    vim_free(save_fname);

    ਵਾਪਸ retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    ਪੂਰਨ		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    ਪੂਰਨ		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    ਲਈ (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	ਜੇ (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    ਜੇ (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    ਨਹੀਂਤਾਂ
	pbuf = tfname = ਭਰਿਆ_Name_save(*fnamep, FALSE);
    ਜੇ (tfname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    len = tflen = STRLEN(tfname);

    ਜੇ (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	ਜੇ (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    ਲਈ (p = tfname + len - 1; p >= tfname; --p)
    {
	ਜੇ (has_mbyte)
	    p -= mb_ਸਿਰ_off(tfname, p);
	ਜੇ (vim_ispathsep(*p))
	{
	    ਜੇ (sepcount == 0 || (hasTilde && sepcount == 1))
		ਰੋਕੋ;
	    ਨਹੀਂਤਾਂ
		sepcount --;
	}
    }
    ਜੇ (hasTilde)
    {
	--p;
	ਜੇ (p >= tfname)
	    *p = '~';
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    ਪੂਰਨ
modify_fname(
    char_u	*src,		// string with modifiers
    ਪੂਰਨ		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    ਪੂਰਨ		valid = 0;
    char_u	*ਪੂਛ;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    ਪੂਰਨ		c;
    ਪੂਰਨ		has_fullname = 0;
    ਪੂਰਨ		has_homerelative = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    char_u	*fname_ਸ਼ੁਰੂ = *fnamep;
    ਪੂਰਨ		has_shortname = 0;
#ਜੇ_ਅੰਤ

repeat:
    // ":p" - full path/file_name
    ਜੇ (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_ਰਸਤਾ;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	ਜੇ ((*fnamep)[0] == '~'
#ਜੇ !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
		&& ((*fnamep)[1] == '/'
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
		    || (*fnamep)[1] == '\\'
# endif
		    || (*fnamep)[1] == NUL)
#ਜੇ_ਅੰਤ
		&& !(tilde_file && (*fnamep)[1] == NUL)
	   )
	{
	    *fnamep = expand_env_save(*fnamep);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ਜੇ (*fnamep == ਨਿਰਾ)
		ਵਾਪਸ -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	ਲਈ (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    ਜੇ (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		ਰੋਕੋ;
	}

	// FullName_save() is slow, don't use it when not needed.
	ਜੇ (*p != NUL || !vim_isAbsName(*fnamep)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#ਜੇ_ਅੰਤ
	)
	{
	    *fnamep = ਭਰਿਆ_Name_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ਜੇ (*fnamep == ਨਿਰਾ)
		ਵਾਪਸ -1;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# ਜੇ _WIN32_WINNT >= 0x0500
	ਜੇ (vim_strchr(*fnamep, '~') != ਨਿਰਾ)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_ਮੰਜ਼ਿਲ_utf16(*fnamep, ਨਿਰਾ);
	    WCHAR buf[MAX_ਰਸਤਾ_2];

	    ਜੇ (wfname != ਨਿਰਾ)
	    {
		ਜੇ (Get_Long_ਰਸਤਾ_Name_W(wfname, buf, MAX_ਰਸਤਾ_2))
		{
		    char_u *q = utf16_ਮੰਜ਼ਿਲ_enc(buf, ਨਿਰਾ);

		    ਜੇ (q != ਨਿਰਾ)
		    {
			vim_free(*bufp);    // free any allocated file name
			*bufp = *fnamep = q;
		    }
		}
		vim_free(wfname);
	    }
	}
# endif
#ਜੇ_ਅੰਤ
	// Append a path separator to a directory.
	ਜੇ (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ਜੇ (*fnamep == ਨਿਰਾ)
		ਵਾਪਸ -1;
	    ਜੋੜੋ_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    ਜਦੋਂ (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	ਜੇ (c == '8')
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    has_shortname = 1; // Postpone this.
#ਜੇ_ਅੰਤ
	    ਜਾਰੀ;
	}
	pbuf = ਨਿਰਾ;
	// Need full path first (use expand_env() to remove a "~/")
	ਜੇ (!has_fullname && !has_homerelative)
	{
	    ਜੇ (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    ਨਹੀਂਤਾਂ
		p = pbuf = ਭਰਿਆ_Name_save(*fnamep, FALSE);
	}
	ਨਹੀਂਤਾਂ
	    p = *fnamep;

	has_fullname = 0;

	ਜੇ (p != ਨਿਰਾ)
	{
	    size_t  dirnamelen = 0;

	    ਜੇ (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		ਜੇ (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    ਜੇ (s != ਨਿਰਾ)
		    {
			dirnamelen = home_replace(ਨਿਰਾ, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		ਜੇ (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		ਜੇ (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    ਜੇ (vim_ispathsep(*p))
		    {
			ਜਦੋਂ (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			ਜੇ (pbuf != ਨਿਰਾ)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = ਨਿਰਾ;
			}
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		dirnamelen = home_replace(ਨਿਰਾ, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		ਜੇ (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    ਜੇ (s != ਨਿਰਾ)
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

    ਪੂਛ = gettail(*fnamep);
    *fnamelen = STRLEN(*fnamep);

    // ":h" - head, remove "/file_name", can be repeated
    // Don't remove the first "/" or "c:\"
    ਜਦੋਂ (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_ਸਿਰ;
	*usedlen += 2;
	s = get_past_ਸਿਰ(*fnamep);
	ਜਦੋਂ (ਪੂਛ > s && after_pathsep(s, ਪੂਛ))
	    MB_PTR_ਪਿੱਠ(*fnamep, ਪੂਛ);
	*fnamelen = ਪੂਛ - *fnamep;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
	ਜੇ (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#ਜੇ_ਅੰਤ
	ਜੇ (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    ਜੇ (p == ਨਿਰਾ)
		ਵਾਪਸ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = ਪੂਛ = p;
	    *fnamelen = 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜਦੋਂ (ਪੂਛ > s && !after_pathsep(s, ਪੂਛ))
		MB_PTR_ਪਿੱਠ(*fnamep, ਪੂਛ);
	}
    }

    // ":8" - shortname
    ਜੇ (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
    {
	*usedlen += 2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	has_shortname = 1;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    /*
     * Handle ":8" after we have done 'heads' and before we do 'tails'.
     */
    ਜੇ (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	ਜੇ (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_ਸ਼ੁਰੂ)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    ਜੇ (p == ਨਿਰਾ)
		ਵਾਪਸ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	ਜੇ (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    ਜੇ (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    ਜੇ (get_short_pathname(fnamep, bufp, &l) == FAIL)
		ਵਾਪਸ -1;

	    ਜੇ (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		ਜੇ (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    ਵਾਪਸ -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	ਪੂਛ = gettail(*fnamep);
    }
#ਜੇ_ਅੰਤ // MSWIN

    // ":t" - tail, just the basename
    ਜੇ (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= ਪੂਛ - *fnamep;
	*fnamep = ਪੂਛ;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    ਜਦੋਂ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	ਜੇ (src[*usedlen + 1] == 'e' && *fnamep > ਪੂਛ)
	    s = *fnamep - 2;
	ਨਹੀਂਤਾਂ
	    s = *fnamep + *fnamelen - 1;
	ਲਈ ( ; s > ਪੂਛ; --s)
	    ਜੇ (s[0] == '.')
		ਰੋਕੋ;
	ਜੇ (src[*usedlen + 1] == 'e')		// :e
	{
	    ਜੇ (s > ਪੂਛ)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		ਲਈ ( ; s > *fnamep; --s)
		    ਜੇ (s[0] == ';')
			ਰੋਕੋ;
		ਜੇ (s > *fnamep)
		    *fnamelen = s - *fnamep;
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*fnamep <= ਪੂਛ)
		*fnamelen = 0;
	}
	ਨਹੀਂਤਾਂ				// :r
	{
	    char_u *ਹੱਦਬੰਦੀ = *fnamep;

	    ਜੇ (ਹੱਦਬੰਦੀ < ਪੂਛ)
		ਹੱਦਬੰਦੀ = ਪੂਛ;
	    ਜੇ (s > ਹੱਦਬੰਦੀ)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    ਜੇ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	ਪੂਰਨ	    sep;
	char_u	    *flags;
	ਪੂਰਨ	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	ਜੇ (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	ਜੇ (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    ਜੇ (p != ਨਿਰਾ)
	    {
		pat = vim_strnsave(s, p - s);
		ਜੇ (pat != ਨਿਰਾ)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    ਜੇ (p != ਨਿਰਾ)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			ਜੇ (sub != ਨਿਰਾ && str != ਨਿਰਾ)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, ਨਿਰਾ, flags, &slen);
			    ਜੇ (s != ਨਿਰਾ)
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
	    ਜੇ (didit)
		ਜਾਓ repeat;
	}
    }

    ਜੇ (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	ਜੇ (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	ਜੇ (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    ਵਾਪਸ valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    ਸਥਿਰ ਖਾਲੀ
shorten_dir_len(char_u *str, ਪੂਰਨ trim_len)
{
    char_u	*ਪੂਛ, *s, *d;
    ਪੂਰਨ		skip = FALSE;
    ਪੂਰਨ		dirchunk_len = 0;

    ਪੂਛ = gettail(str);
    d = str;
    ਲਈ (s = str; ; ++s)
    {
	ਜੇ (s >= ਪੂਛ)		    // copy the whole tail
	{
	    *d++ = *s;
	    ਜੇ (*s == NUL)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!skip)
	{
	    *d++ = *s;			// copy next char
	    ਜੇ (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		ਜੇ (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    ਜੇ (has_mbyte)
	    {
		ਪੂਰਨ l = mb_ptr2len(s);

		ਜਦੋਂ (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    ਖਾਲੀ
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    ਪੂਰਨ
file_is_readable(char_u *fname)
{
    ਪੂਰਨ		fd;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ O_NONBLOCK
# define O_NONBLOCK 0
#ਜੇ_ਅੰਤ
    // Opening a file that does not exist is slow.
    ਜੇ (*fname == NUL || mch_isdir_or_missing(fname))
	ਵਾਪਸ FALSE;
    ਜੇ ((fd = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

#ਜੇ defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    ਖਾਲੀ
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	ਜੇ (ਵਿੱਚ_vim9script())
	    (ਖਾਲੀ) check_for_string_arg(argvars, 0);
	ਵਾਪਸ;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    ਜੇ (cwd != ਨਿਰਾ)
    {
	ਜੇ (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	ਜੇ (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_ਖਿੜਕੀ;
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "scope", s);
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_localdir != ਨਿਰਾ)
	scope = CDSCOPE_ਖਿੜਕੀ;
    ਨਹੀਂਤਾਂ ਜੇ (curtab->tp_localdir != ਨਿਰਾ)
	scope = CDSCOPE_TABPAGE;

    ਜੇ (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    ਖਾਲੀ
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);
    ਜੇ (name == ਨਿਰਾ || *name == NUL)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ;
    }

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    ਨਹੀਂਤਾਂ
	flags = (char_u *)"";

    ਜੇ (*flags == NUL)
	// delete a file
	rettv->vval.v_ਸੰਖਿਆ = mch_remove(name) == 0 ? 0 : -1;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_ਸੰਖਿਆ = mch_rmdir(name) == 0 ? 0 : -1;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_ਸੰਖਿਆ = delete_recursive(name);
    ਨਹੀਂਤਾਂ
	semsg(_(e_invalid_ਵਿਅੰਜਕ_str), flags);
}

/*
 * "executable()" function
 */
    ਖਾਲੀ
f_executable(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_ਸੰਖਿਆ = mch_can_exe(tv_get_string(&argvars[0]), ਨਿਰਾ, TRUE);
}

/*
 * "exepath()" function
 */
    ਖਾਲੀ
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;
    (ਖਾਲੀ)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    ਖਾਲੀ
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;
    rettv->vval.v_ਸੰਖਿਆ = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    ਖਾਲੀ
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;
    rettv->vval.v_ਸੰਖਿਆ = filewritable(tv_get_string(&argvars[0]));
}

    ਸਥਿਰ ਖਾਲੀ
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    ਪੂਰਨ		ਲੱਭੋ_what)
{
    char_u	*fname;
    char_u	*fresult = ਨਿਰਾ;
    char_u	*ਰਸਤਾ = *curbuf->b_p_ਰਸਤਾ == NUL ? p_ਰਸਤਾ : curbuf->b_p_ਰਸਤਾ;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 1;
    ਪੂਰਨ		ਪਹਿਲਾ = TRUE;
    ਪੂਰਨ		error = FALSE;

    rettv->vval.v_string = ਨਿਰਾ;
    rettv->v_type = VAR_STRING;
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    fname = tv_get_string(&argvars[0]);

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	ਜੇ (p == ਨਿਰਾ)
	    error = TRUE;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*p != NUL)
		ਰਸਤਾ = p;

	    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
		ਗਿਣਤੀ_2 = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	}
    }

    ਜੇ (ਗਿਣਤੀ_2 < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    ਜੇ (*fname != NUL && !error)
    {
	char_u	*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ = ਨਿਰਾ;
	ਅੱਖਰ	*ਖੋਜ_ctx = ਨਿਰਾ;

	ਕਰੋ
	{
	    ਜੇ (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ_option(ਪਹਿਲਾ ? fname : ਨਿਰਾ,
					       ਪਹਿਲਾ ? (ਪੂਰਨ)STRLEN(fname) : 0,
					0, ਪਹਿਲਾ, ਰਸਤਾ,
					ਲੱਭੋ_what,
					curbuf->b_ffname,
					ਲੱਭੋ_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, &ਖੋਜ_ctx);
	    ਪਹਿਲਾ = FALSE;

	    ਜੇ (fresult != ਨਿਰਾ && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} ਜਦੋਂ ((rettv->v_type == VAR_LIST || --ਗਿਣਤੀ_2 > 0) && fresult != ਨਿਰਾ);

	vim_free(file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
	vim_findfile_cleanup(ਖੋਜ_ctx);
    }

    ਜੇ (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    ਖਾਲੀ
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    ਖਾਲੀ
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    ਖਾਲੀ
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = ਨਿਰਾ;
    char_u	buf[NUMBUFLEN];

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    ਜੇ (mods == ਨਿਰਾ || fname == ਨਿਰਾ)
	fname = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	len = STRLEN(fname);
	ਜੇ (mods != ਨਿਰਾ && *mods != NUL)
	    (ਖਾਲੀ)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    ਜੇ (fname == ਨਿਰਾ)
	rettv->vval.v_string = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = ਨਿਰਾ;
    tabpage_T	*tp = ਨਿਰਾ;
    char_u	*cwd;
    ਪੂਰਨ		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_ਸੰਖਿਆ
	    && argvars[0].vval.v_ਸੰਖਿਆ == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    ਨਹੀਂਤਾਂ
	wp = ਲੱਭੋ_tabwin(&argvars[0], &argvars[1], &tp);

    ਜੇ (wp != ਨਿਰਾ && wp->w_localdir != ਨਿਰਾ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    ਨਹੀਂਤਾਂ ਜੇ (tp != ਨਿਰਾ && tp->tp_localdir != ਨਿਰਾ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    ਨਹੀਂਤਾਂ ਜੇ (wp != ਨਿਰਾ || tp != ਨਿਰਾ || global)
    {
	ਜੇ (globaldir != ਨਿਰਾ && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	ਨਹੀਂਤਾਂ
	{
	    cwd = alloc(MAXPATHL);
	    ਜੇ (cwd != ਨਿਰਾ)
	    {
		ਜੇ (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
    ਜੇ (rettv->vval.v_string != ਨਿਰਾ)
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
    ਪੂਰਨ		    i;

    ਲਈ (i = 0; i < 9; i++)
    {
	ਜੇ (st->st_ਬਹੁਲਕ & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	ਨਹੀਂਤਾਂ
	    perm[i] = '-';
    }
    ਵਾਪਸ perm;
}

/*
 * "getfperm({fname})" function
 */
    ਖਾਲੀ
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = ਨਿਰਾ;
    char_u	permbuf[] = "---------";

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    ਖਾਲੀ
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    fname = tv_get_string(&argvars[0]);
    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) >= 0)
    {
	ਜੇ (S_ISDIR(st.st_ਬਹੁਲਕ))
	    rettv->vval.v_ਸੰਖਿਆ = 0;
	ਨਹੀਂਤਾਂ
	{
	    rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    ਜੇ ((off_T)rettv->vval.v_ਸੰਖਿਆ != (off_T)st.st_size)
		rettv->vval.v_ਸੰਖਿਆ = -2;
	}
    }
    ਨਹੀਂਤਾਂ
	  rettv->vval.v_ਸੰਖਿਆ = -1;
}

/*
 * "getftime({fname})" function
 */
    ਖਾਲੀ
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    fname = tv_get_string(&argvars[0]);
    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) >= 0)
	rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)st.st_mtime;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    ਅੱਖਰ    *t;

    ਜੇ (S_ISREG(st->st_ਬਹੁਲਕ))
	t = "file";
    ਨਹੀਂਤਾਂ ਜੇ (S_ISDIR(st->st_ਬਹੁਲਕ))
	t = "dir";
    ਨਹੀਂਤਾਂ ਜੇ (S_ISLNK(st->st_ਬਹੁਲਕ))
	t = "link";
    ਨਹੀਂਤਾਂ ਜੇ (S_ISBLK(st->st_ਬਹੁਲਕ))
	t = "bdev";
    ਨਹੀਂਤਾਂ ਜੇ (S_ISCHR(st->st_ਬਹੁਲਕ))
	t = "cdev";
    ਨਹੀਂਤਾਂ ਜੇ (S_ISFIFO(st->st_ਬਹੁਲਕ))
	t = "fifo";
    ਨਹੀਂਤਾਂ ਜੇ (S_ISSOCK(st->st_ਬਹੁਲਕ))
	t = "socket";
    ਨਹੀਂਤਾਂ
	t = "other";
    ਵਾਪਸ (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    ਖਾਲੀ
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ਜੇ (mch_lstat((ਅੱਖਰ *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    ਖਾਲੀ
f_glob(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    ਪੂਰਨ		error = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	ਵਾਪਸ;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ਜੇ (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ਜੇ (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_ਸੈੱਟ(rettv, ਨਿਰਾ);
	    ਜੇ (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    ਜੇ (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	ਜੇ (p_wic)
	    options += WILD_ICASE;
	ਜੇ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     ਨਿਰਾ, options, WILD_ALL);
	ਨਹੀਂਤਾਂ ਜੇ (rettv_list_alloc(rettv) == OK)
	{
	  ਪੂਰਨ i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						ਨਿਰਾ, options, WILD_ALL_KEEP);
	  ਲਈ (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = ਨਿਰਾ;
}

/*
 * "glob2regpat()" function
 */
    ਖਾਲੀ
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, ਵਿੱਚ_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == ਨਿਰਾ)
			 ? ਨਿਰਾ : file_pat_ਮੰਜ਼ਿਲ_reg_pat(pat, ਨਿਰਾ, ਨਿਰਾ, FALSE);
}

/*
 * "globpath()" function
 */
    ਖਾਲੀ
f_globpath(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    ਪੂਰਨ		error = FALSE;
    garray_T	ga;
    ਪੂਰਨ		i;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	ਵਾਪਸ;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ਜੇ (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	ਜੇ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    ਜੇ (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_ਸੈੱਟ(rettv, ਨਿਰਾ);
	    ਜੇ (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    ਜੇ (file != ਨਿਰਾ && !error)
    {
	ga_init2(&ga, ਆਕਾਰ(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	ਜੇ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_ਲੜੀਆਂ(&ga, "\n");
	ਨਹੀਂਤਾਂ ਜੇ (rettv_list_alloc(rettv) == OK)
	    ਲਈ (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_ਅੰਕੜੇ))[i], -1);
	ga_clear_ਲੜੀਆਂ(&ga);
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = ਨਿਰਾ;
}

/*
 * "isdirectory()" function
 */
    ਖਾਲੀ
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    ਖਾਲੀ
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = mch_is_ਭਰਿਆ_Name(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
mkdir_recurse(char_u *dir, ਪੂਰਨ prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    ਪੂਰਨ		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    ਜੇ (p <= get_past_ਸਿਰ(dir))
	ਵਾਪਸ OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    ਜੇ (updir == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (mch_isdir(updir))
	r = OK;
    ਨਹੀਂਤਾਂ ਜੇ (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	ਜੇ (r == OK && created != ਨਿਰਾ && *created == ਨਿਰਾ)
	    *created = ਭਰਿਆ_Name_save(updir, FALSE);
    }
    vim_free(updir);
    ਵਾਪਸ r;
}

/*
 * "mkdir()" function
 */
    ਖਾਲੀ
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    ਪੂਰਨ		prot = 0755;
    ਪੂਰਨ		defer = FALSE;
    ਪੂਰਨ		defer_recurse = FALSE;
    char_u	*created = ਨਿਰਾ;

    rettv->vval.v_ਸੰਖਿਆ = FAIL;
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    dir = tv_get_string_buf(&argvars[0], buf);
    ਜੇ (*dir == NUL)
	ਵਾਪਸ;

    ਜੇ (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], ਨਿਰਾ);
	    ਜੇ (prot == -1)
		ਵਾਪਸ;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != ਨਿਰਾ;
	defer_recurse = vim_strchr(arg2, 'R') != ਨਿਰਾ;
	ਜੇ ((defer || defer_recurse) && !can_ਜੋੜੋ_defer())
	    ਵਾਪਸ;

	ਜੇ (vim_strchr(arg2, 'p') != ਨਿਰਾ)
	{
	    ਜੇ (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_ਸੰਖਿਆ = OK;
		ਵਾਪਸ;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : ਨਿਰਾ);
	}
    }
    rettv->vval.v_ਸੰਖਿਆ = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    ਜੇ (rettv->vval.v_ਸੰਖਿਆ == OK
				&& created == ਨਿਰਾ && (defer || defer_recurse))
	created = ਭਰਿਆ_Name_save(dir, FALSE);
    ਜੇ (created != ਨਿਰਾ)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_ਤਾਲਾ = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_ਤਾਲਾ = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	ਜੇ (tv[0].vval.v_string == ਨਿਰਾ || tv[1].vval.v_string == ਨਿਰਾ
		|| ਜੋੜੋ_defer((char_u *)"delete", 2, tv) == FAIL)
	{
	    vim_free(tv[0].vval.v_string);
	    vim_free(tv[1].vval.v_string);
	}
    }
}

/*
 * "pathshorten()" function
 */
    ਖਾਲੀ
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    ਪੂਰਨ		trim_len = 1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[1]);
	ਜੇ (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    ਜੇ (p == ਨਿਰਾ)
	rettv->vval.v_string = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	ਜੇ (p != ਨਿਰਾ)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    ਸਥਿਰ ਪੂਰਨ
checkitem_common(ਖਾਲੀ *context, char_u *name, dict_T *dict)
{
    typval_T	*ਵਿਅੰਜਕ = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    ਪੂਰਨ		retval = 0;
    ਪੂਰਨ		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    ਜੇ (name != ਨਿਰਾ)
    {
	ਸੈੱਟ_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    ਜੇ (eval_ਵਿਅੰਜਕ_typval(ਵਿਅੰਜਕ, FALSE, argv, 1, ਨਿਰਾ, &rettv) == FAIL)
	ਜਾਓ theend;

    // We want to use -1, but also true/false should be allowed.
    ਜੇ (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_ਸੰਖਿਆ;
	rettv.vval.v_ਸੰਖਿਆ = rettv.vval.v_ਸੰਖਿਆ == VVAL_TRUE;
    }
    retval = tv_get_ਸੰਖਿਆ_chk(&rettv, &error);
    ਜੇ (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    ਜੇ (name != ਨਿਰਾ)
	ਸੈੱਟ_vim_var_string(VV_VAL, ਨਿਰਾ, 0);
    ਨਹੀਂਤਾਂ
	ਸੈੱਟ_vim_var_dict(VV_VAL, ਨਿਰਾ);
    restore_vimvar(VV_VAL, &save_val);
    ਵਾਪਸ retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    ਸਥਿਰ ਪੂਰਨ
readdir_checkitem(ਖਾਲੀ *context, ਖਾਲੀ *ਵਸਤੂ)
{
    char_u	*name = (char_u *)ਵਸਤੂ;

    ਵਾਪਸ checkitem_common(context, name, ਨਿਰਾ);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    ਸਥਿਰ ਪੂਰਨ
readdirex_dict_arg(typval_T *argvars, ਪੂਰਨ *cmp)
{
    char_u     *compare;

    ਜੇ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (dict_has_ਕੁੰਜੀ(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_dictionary_ਕੁੰਜੀ_str_required), "sort");
	ਵਾਪਸ FAIL;
    }

    ਜੇ (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_ਛਾਂਟੋ_NONE;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_ਛਾਂਟੋ_BYTE;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_ਛਾਂਟੋ_IC;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_ਛਾਂਟੋ_COLLATE;
    ਵਾਪਸ OK;
}

/*
 * "readdir()" function
 */
    ਖਾਲੀ
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*ਵਿਅੰਜਕ;
    ਪੂਰਨ		ret;
    char_u	*ਰਸਤਾ;
    char_u	*p;
    garray_T	ga;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਛਾਂਟੋ = READDIR_ਛਾਂਟੋ_BYTE;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    ਰਸਤਾ = tv_get_string(&argvars[0]);
    ਵਿਅੰਜਕ = &argvars[1];

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &ਛਾਂਟੋ) == FAIL)
	ਵਾਪਸ;

    ret = readdir_core(&ga, ਰਸਤਾ, FALSE, (ਖਾਲੀ *)ਵਿਅੰਜਕ,
	    (ਵਿਅੰਜਕ->v_type == VAR_UNKNOWN) ? ਨਿਰਾ : readdir_checkitem, ਛਾਂਟੋ);
    ਜੇ (ret == OK)
    {
	ਲਈ (i = 0; i < ga.ga_len; i++)
	{
	    p = ((char_u **)ga.ga_ਅੰਕੜੇ)[i];
	    list_append_string(rettv->vval.v_list, p, -1);
	}
    }
    ga_clear_ਲੜੀਆਂ(&ga);
}

/*
 * Evaluate "expr" (= "context") for readdirex().
 */
    ਸਥਿਰ ਪੂਰਨ
readdirex_checkitem(ਖਾਲੀ *context, ਖਾਲੀ *ਵਸਤੂ)
{
    dict_T	*dict = (dict_T*)ਵਸਤੂ;

    ਵਾਪਸ checkitem_common(context, ਨਿਰਾ, dict);
}

/*
 * "readdirex()" function
 */
    ਖਾਲੀ
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*ਵਿਅੰਜਕ;
    ਪੂਰਨ		ret;
    char_u	*ਰਸਤਾ;
    garray_T	ga;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਛਾਂਟੋ = READDIR_ਛਾਂਟੋ_BYTE;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    ਰਸਤਾ = tv_get_string(&argvars[0]);
    ਵਿਅੰਜਕ = &argvars[1];

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &ਛਾਂਟੋ) == FAIL)
	ਵਾਪਸ;

    ret = readdir_core(&ga, ਰਸਤਾ, TRUE, (ਖਾਲੀ *)ਵਿਅੰਜਕ,
	    (ਵਿਅੰਜਕ->v_type == VAR_UNKNOWN) ? ਨਿਰਾ : readdirex_checkitem, ਛਾਂਟੋ);
    ਜੇ (ret == OK)
    {
	ਲਈ (i = 0; i < ga.ga_len; i++)
	{
	    dict_T  *dict = ((dict_T**)ga.ga_ਅੰਕੜੇ)[i];
	    list_append_dict(rettv->vval.v_list, dict);
	    dict_unref(dict);
	}
    }
    ga_clear(&ga);
}

/*
 * "readfile()" function
 */
    ਸਥਿਰ ਖਾਲੀ
read_file_or_blob(typval_T *argvars, typval_T *rettv, ਪੂਰਨ always_blob)
{
    ਪੂਰਨ		ਦੁਆਧਾਰੀ = FALSE;
    ਪੂਰਨ		blob = always_blob;
    ਪੂਰਨ		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    ਪੂਰਨ		io_ਮਾਪ = ਆਕਾਰ(buf);
    ਪੂਰਨ		readlen;		// size of last fread()
    char_u	*prev	 = ਨਿਰਾ;	// previously read bytes, if any
    ਲੰਮਾ	prevlen  = 0;		// length of data in prev
    ਲੰਮਾ	prevsize = 0;		// size of prev buffer
    ਲੰਮਾ	maxline  = MAXLNUM;
    ਲੰਮਾ	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*ਸ਼ੁਰੂ;			// start of current line
    off_T	ਵਿੱਥ = 0;
    off_T	ਮਾਪ = -1;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ਜੇ (always_blob)
	{
	    ਵਿੱਥ = (off_T)tv_get_ਸੰਖਿਆ(&argvars[1]);
	    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
		ਮਾਪ = (off_T)tv_get_ਸੰਖਿਆ(&argvars[2]);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		ਦੁਆਧਾਰੀ = TRUE;
	    ਜੇ (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ(&argvars[2]);
	}
    }

    ਜੇ ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	ਵਾਪਸ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    ਜੇ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ਵਾਪਸ;
    }
    ਜੇ (*fname == NUL || (fd = mch_fopen((ਅੱਖਰ *)fname, READBIN)) == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ਵਾਪਸ;
    }

    ਜੇ (blob)
    {
	ਜੇ (read_blob(fd, rettv, ਵਿੱਥ, ਮਾਪ) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	ਵਾਪਸ;
    }

    ਜਦੋਂ (cnt < maxline || maxline < 0)
    {
	readlen = (ਪੂਰਨ)fread(buf, 1, io_ਮਾਪ, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	ਲਈ (p = buf, ਸ਼ੁਰੂ = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || ਦੁਆਧਾਰੀ));
		++p)
	{
	    ਜੇ (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= ਨਿਰਾ;
		long_u	    len = p - ਸ਼ੁਰੂ;

		// Finished a line.  Remove CRs before NL.
		ਜੇ (readlen > 0 && !ਦੁਆਧਾਰੀ)
		{
		    ਜਦੋਂ (len > 0 && ਸ਼ੁਰੂ[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    ਜੇ (len == 0)
			ਜਦੋਂ (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		ਜੇ (prevlen == 0)
		    s = vim_strnsave(ਸ਼ੁਰੂ, len);
		ਨਹੀਂਤਾਂ
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    ਜੇ ((s = vim_realloc(prev, prevlen + len + 1)) != ਨਿਰਾ)
		    {
			mch_memmove(s + prevlen, ਸ਼ੁਰੂ, len);
			s[prevlen + len] = NUL;
			prev = ਨਿਰਾ; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		ਜੇ (s == ਨਿਰਾ)
		{
		    do_outofmem_ਸੁਨੇਹਾ((long_u) prevlen + len + 1);
		    failed = TRUE;
		    ਰੋਕੋ;
		}

		ਜੇ ((li = listitem_alloc()) == ਨਿਰਾ)
		{
		    vim_free(s);
		    failed = TRUE;
		    ਰੋਕੋ;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_ਤਾਲਾ = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		ਸ਼ੁਰੂ = p + 1; // step over newline
		ਜੇ ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    ਨਹੀਂਤਾਂ ਜੇ (*p == 0xbf && enc_utf8 && !ਦੁਆਧਾਰੀ)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		ਜੇ (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    ਜੇ (ਸ਼ੁਰੂ == dest)
			ਸ਼ੁਰੂ = p + 1;
		    ਨਹੀਂਤਾਂ
		    {
			// have to shuffle buf to close gap
			ਪੂਰਨ adjust_prevlen = 0;

			ਜੇ (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (ਪੂਰਨ)(buf - dest);
			    dest = buf;
			}
			ਜੇ (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	ਜੇ (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    ਰੋਕੋ;
	ਜੇ (ਸ਼ੁਰੂ < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    ਜੇ (p - ਸ਼ੁਰੂ + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		ਜੇ (prevsize == 0)
		    prevsize = (ਲੰਮਾ)(p - ਸ਼ੁਰੂ);
		ਨਹੀਂਤਾਂ
		{
		    ਲੰਮਾ grow50pc = (prevsize * 3) / 2;
		    ਲੰਮਾ growmin  = (ਲੰਮਾ)((p - ਸ਼ੁਰੂ) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		ਜੇ (newprev == ਨਿਰਾ)
		{
		    do_outofmem_ਸੁਨੇਹਾ((long_u)prevsize);
		    failed = TRUE;
		    ਰੋਕੋ;
		}
		prev = newprev;
	    }
	    // Add the line part to end of "prev".
	    mch_memmove(prev + prevlen, ਸ਼ੁਰੂ, p - ਸ਼ੁਰੂ);
	    prevlen += (ਲੰਮਾ)(p - ਸ਼ੁਰੂ);
	}
    } // while

    // For a negative line count use only the lines at the end of the file,
    // free the rest.
    ਜੇ (!failed && maxline < 0)
	ਜਦੋਂ (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_ਪਹਿਲਾ);
	    --cnt;
	}

    ਜੇ (failed)
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
    ਖਾਲੀ
f_readblob(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    ਖਾਲੀ
f_readfile(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    ਖਾਲੀ
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = ਨਿਰਾ;
# endif

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = ਨਿਰਾ;

	v = mch_resolve_ਰਸਤਾ(p, TRUE);
	ਜੇ (v != ਨਿਰਾ)
	    rettv->vval.v_string = v;
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_string = vim_strsave(p);
    }
# ਨਹੀਂਤਾਂ
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	ਪੂਰਨ	len;
	char_u	*remain = ਨਿਰਾ;
	char_u	*q;
	ਪੂਰਨ	is_relative_ਮੰਜ਼ਿਲ_current = FALSE;
	ਪੂਰਨ	has_trailing_pathsep = FALSE;
	ਪੂਰਨ	ਹੱਦਬੰਦੀ = 100;

	p = vim_strsave(p);
	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਓ fail;
	ਜੇ (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_ਮੰਜ਼ਿਲ_current = TRUE;

	len = STRLEN(p);
	ਜੇ (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	ਜੇ (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	ਜੇ (buf == ਨਿਰਾ)
	{
	    vim_free(p);
	    vim_free(remain);
	    ਜਾਓ fail;
	}

	ਲਈ (;;)
	{
	    ਲਈ (;;)
	    {
		len = readlink((ਅੱਖਰ *)p, (ਅੱਖਰ *)buf, MAXPATHL);
		ਜੇ (len <= 0)
		    ਰੋਕੋ;
		buf[len] = NUL;

		ਜੇ (ਹੱਦਬੰਦੀ-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_ਚੱਕਰ));
		    rettv->vval.v_string = ਨਿਰਾ;
		    ਜਾਓ fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		ਜੇ (remain == ਨਿਰਾ && has_trailing_pathsep)
		    ਜੋੜੋ_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		ਜੇ (*q != NUL)
		{
		    ਜੇ (remain == ਨਿਰਾ)
			remain = vim_strsave(q - 1);
		    ਨਹੀਂਤਾਂ
		    {
			cpy = concat_str(q - 1, remain);
			ਜੇ (cpy != ਨਿਰਾ)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		ਜੇ (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		ਜੇ (q > p && !mch_is_ਭਰਿਆ_Name(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    ਜੇ (cpy != ਨਿਰਾ)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    ਜੇ (remain == ਨਿਰਾ)
		ਰੋਕੋ;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    ਜੇ (cpy != ਨਿਰਾ)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    ਜੇ (*q != NUL)
		STRMOVE(remain, q - 1);
	    ਨਹੀਂਤਾਂ
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	ਜੇ (!vim_ispathsep(*p))
	{
	    ਜੇ (is_relative_ਮੰਜ਼ਿਲ_current
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
		ਜੇ (cpy != ਨਿਰਾ)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!is_relative_ਮੰਜ਼ਿਲ_current)
	    {
		// Strip leading "./".
		q = p;
		ਜਦੋਂ (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		ਜੇ (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	ਜੇ (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    ਜੇ (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
f_tempname(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਸਥਿਰ ਪੂਰਨ	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    ਕਰੋ
    {
	ਜੇ (x == 'Z')
	    x = '0';
	ਨਹੀਂਤਾਂ ਜੇ (x == '9')
	    x = 'A';
	ਨਹੀਂਤਾਂ
	    ++x;
    } ਜਦੋਂ (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    ਖਾਲੀ
f_writefile(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		ਦੁਆਧਾਰੀ = FALSE;
    ਪੂਰਨ		append = FALSE;
    ਪੂਰਨ		defer = FALSE;
# ifdef HAVE_FSYNC
    ਪੂਰਨ		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    ਪੂਰਨ		ret = 0;
    listitem_T	*li;
    list_T	*list = ਨਿਰਾ;
    blob_T	*blob = ਨਿਰਾ;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	ਜੇ (list == ਨਿਰਾ)
	    ਵਾਪਸ;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ਚੀਜ਼ਾਂ(list, li)
	    ਜੇ (tv_get_string_chk(&li->li_tv) == ਨਿਰਾ)
		ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	ਜੇ (blob == ਨਿਰਾ)
	    ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਦਲੀਲ_str),
		_("writefile() first argument must be a List or a Blob"));
	ਵਾਪਸ;
    }

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	ਜੇ (arg2 == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ (vim_strchr(arg2, 'b') != ਨਿਰਾ)
	    ਦੁਆਧਾਰੀ = TRUE;
	ਜੇ (vim_strchr(arg2, 'a') != ਨਿਰਾ)
	    append = TRUE;
	ਜੇ (vim_strchr(arg2, 'D') != ਨਿਰਾ)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	ਜੇ (vim_strchr(arg2, 's') != ਨਿਰਾ)
	    do_fsync = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr(arg2, 'S') != ਨਿਰਾ)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (defer && !can_ਜੋੜੋ_defer())
	ਵਾਪਸ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    ਜੇ (*fname == NUL || (fd = mch_fopen((ਅੱਖਰ *)fname,
				      append ? APPENDBIN : WRITEBIN)) == ਨਿਰਾ)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_ਤਾਲਾ = 0;
	    tv.vval.v_string = ਭਰਿਆ_Name_save(fname, FALSE);
	    ਜੇ (tv.vval.v_string == ਨਿਰਾ
		    || ਜੋੜੋ_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(ਖਾਲੀ)mch_remove(fname);
	    }
	}

	ਜੇ (ret == 0)
	{
	    ਜੇ (blob)
	    {
		ਜੇ (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (write_list(fd, list, ਦੁਆਧਾਰੀ) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    ਜੇ (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_ਸੰਖਿਆ = ret;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL

#ਜੇ defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    ਪੂਰਨ		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    ਸਥਿਰ char_u	*ਆਖਰੀ_dir = ਨਿਰਾ;    // last used directory
    char_u		*tofree = ਨਿਰਾ;
    ਪੂਰਨ			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    ਜੇ (title == ਨਿਰਾ || *title == NUL)
    {
	ਜੇ (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	ਨਹੀਂਤਾਂ ਜੇ (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	ਨਹੀਂਤਾਂ
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    ਜੇ ((initdir == ਨਿਰਾ || *initdir == NUL) && dflt != ਨਿਰਾ && *dflt != NUL)
    {
	ਜੇ (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    ਜੇ (tofree != ਨਿਰਾ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    ਜੇ (initdir == ਨਿਰਾ || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	ਜੇ (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	ਨਹੀਂਤਾਂ ਜੇ (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != ਨਿਰਾ && buf->b_ffname != ਨਿਰਾ)
	{
	    ਜੇ (dflt == ਨਿਰਾ || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    ਜੇ (tofree != ਨਿਰਾ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	ਨਹੀਂਤਾਂ ਜੇ (*p_bsdir == 'l')
	    initdir = ਆਖਰੀ_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)		// when this changes, also adjust f_has()!
    {
	ਜੇ (filter == ਨਿਰਾ
#  ifdef FEAT_EVAL
		&& (filter = get_var_ਮੁੱਲ((char_u *)"b:browsefilter")) == ਨਿਰਾ
		&& (filter = get_var_ਮੁੱਲ((char_u *)"g:browsefilter")) == ਨਿਰਾ
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	ਜੇ (flags & BROWSE_DIR)
	{
#  ਜੇ defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  ਨਹੀਂਤਾਂ
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  ਜੇ !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    ਜੇ (fname != ਨਿਰਾ && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *ਪੂਛ = gettail_sep(fname);

		ਜੇ (ਪੂਛ == fname)
		    *ਪੂਛ++ = '.';	// use current dir
		*ਪੂਛ = NUL;
	    }
#  endif
	}
	ਨਹੀਂਤਾਂ
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_ਵਿੱਚ_console_ਬਹੁਲਕ));
	fname = ਨਿਰਾ;
    }

    // keep the directory for next time
    ਜੇ (fname != ਨਿਰਾ)
    {
	vim_free(ਆਖਰੀ_dir);
	ਆਖਰੀ_dir = vim_strsave(fname);
	ਜੇ (ਆਖਰੀ_dir != ਨਿਰਾ && !(flags & BROWSE_DIR))
	{
	    *gettail(ਆਖਰੀ_dir) = NUL;
	    ਜੇ (*ਆਖਰੀ_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(ਆਖਰੀ_dir);
		ਆਖਰੀ_dir = alloc(MAXPATHL);
		ਜੇ (ਆਖਰੀ_dir != ਨਿਰਾ)
		    mch_dirname(ਆਖਰੀ_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    ਵਾਪਸ fname;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    ਖਾਲੀ
f_browse(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    ਪੂਰਨ		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ਪੂਰਨ		error = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ਵਾਪਸ;

    save = (ਪੂਰਨ)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    ਜੇ (error || title == ਨਿਰਾ || initdir == ਨਿਰਾ || defname == ਨਿਰਾ)
	rettv->vval.v_string = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, ਨਿਰਾ, initdir, ਨਿਰਾ, curbuf);
# ਨਹੀਂਤਾਂ
    rettv->vval.v_string = ਨਿਰਾ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    ਖਾਲੀ
f_browsedir(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    ਜੇ (title == ਨਿਰਾ || initdir == ਨਿਰਾ)
	rettv->vval.v_string = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, ਨਿਰਾ, ਨਿਰਾ, initdir, ਨਿਰਾ, curbuf);
# ਨਹੀਂਤਾਂ
    rettv->vval.v_string = ਨਿਰਾ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    ਖਾਲੀ
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_ਸੰਖਿਆ = FALSE;

    ਜੇ (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    from = tv_get_string(&argvars[0]);

    ਜੇ (mch_lstat((ਅੱਖਰ *)from, &st) >= 0
	&& (S_ISREG(st.st_ਬਹੁਲਕ) || S_ISLNK(st.st_ਬਹੁਲਕ)))
	rettv->vval.v_ਸੰਖਿਆ = vim_copyfile(
	    tv_get_string(&argvars[0]),
	    tv_get_string(&argvars[1])) == OK ? TRUE : FALSE;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL

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
    ਪੂਰਨ		dstlen,	// maximum length of the result
    ਪੂਰਨ		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_ਸ਼ੁਰੂ;

    ਜੇ (src == ਨਿਰਾ)
    {
	*dst = NUL;
	ਵਾਪਸ 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    ਜੇ (buf != ਨਿਰਾ && buf->b_help)
	ਵਾਪਸ vim_snprintf_safelen((ਅੱਖਰ *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    ਜੇ (homedir != ਨਿਰਾ)
	dirlen = STRLEN(homedir);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#ਨਹੀਂਤਾਂ
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (homedir_env == ਨਿਰਾ)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#ਜੇ_ਅੰਤ
    // Empty is the same as not set.
    ਜੇ (homedir_env != ਨਿਰਾ && *homedir_env == NUL)
	homedir_env = ਨਿਰਾ;

    ਜੇ (homedir_env != ਨਿਰਾ && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = ਨਿਰਾ;

	flen = STRLEN(homedir_env);
	(ਖਾਲੀ)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	ਜੇ (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    ਜੇ (homedir_env != ਨਿਰਾ)
	envlen = STRLEN(homedir_env);

    ਜੇ (!one)
	src = skipwhite(src);
    dst_ਸ਼ੁਰੂ = dst;		// remember the start
    ਜਦੋਂ (*src && dstlen > 0)
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
	ਲਈ (;;)
	{
	    ਜੇ (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		ਜੇ (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		ਰੋਕੋ;
	    }
	    ਜੇ (p == homedir_env)
		ਰੋਕੋ;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	ਜਦੋਂ (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	ਜਦੋਂ ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    ਜੇ (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    ਵਾਪਸ (size_t)(dst - dst_ਸ਼ੁਰੂ);
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
    ਬਿਨਾਨਿਸ਼ਾਨੀ	len;

    len = 3;			// space for "~/" and trailing NUL
    ਜੇ (src != ਨਿਰਾ)		// just in case
	len += (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(src);
    dst = alloc(len);
    ਜੇ (dst != ਨਿਰਾ)
	home_replace(buf, src, dst, len, TRUE);
    ਵਾਪਸ dst;
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
    ਪੂਰਨ
fullpathcmp(
    char_u *s1,
    char_u *s2,
    ਪੂਰਨ	    checkname,		// when both don't exist, check file names
    ਪੂਰਨ	    expandenv)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    ਪੂਰਨ		    r1, r2;

    ਜੇ (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    ਨਹੀਂਤਾਂ
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((ਅੱਖਰ *)exp1, &st1);
    r2 = mch_stat((ਅੱਖਰ *)s2, &st2);
    ਜੇ (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	ਜੇ (checkname)
	{
	    ਜੇ (fnamecmp(exp1, s2) == 0)
		ਵਾਪਸ FPC_SAMEX;
	    r1 = vim_ਭਰਿਆ_Name(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_ਭਰਿਆ_Name(s2, full2, MAXPATHL, FALSE);
	    ਜੇ (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		ਵਾਪਸ FPC_SAMEX;
	}
	ਵਾਪਸ FPC_NOTX;
    }
    ਜੇ (r1 != 0 || r2 != 0)
	ਵਾਪਸ FPC_DIFFX;
    ਜੇ (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	ਵਾਪਸ FPC_SAME;
    ਵਾਪਸ FPC_DIFF;
#ਨਹੀਂਤਾਂ
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    ਪੂਰਨ	    retval = FPC_DIFF;
    ਪੂਰਨ	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    ਜੇ ((exp1 = alloc(MAXPATHL * 3)) != ਨਿਰਾ)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	ਜੇ (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	ਨਹੀਂਤਾਂ
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_ਭਰਿਆ_Name(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_ਭਰਿਆ_Name(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	ਜੇ (r1 != OK && r2 != OK)
	{
	    ਜੇ (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    ਨਹੀਂਤਾਂ
		retval = FPC_NOTX;
	}
	ਨਹੀਂਤਾਂ ਜੇ (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	ਨਹੀਂਤਾਂ ਜੇ (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	ਨਹੀਂਤਾਂ
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    ਵਾਪਸ retval;
#ਜੇ_ਅੰਤ
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

    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ (char_u *)"";
    ਲਈ (p1 = p2 = get_past_ਸਿਰ(fname); *p2; )	// find last part of path
    {
	ਜੇ (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    ਵਾਪਸ p1;
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

    p = get_past_ਸਿਰ(fname);	// don't remove the '/' from "c:/file"
    t = gettail(fname);
    ਜਦੋਂ (t > p && after_pathsep(fname, t))
	--t;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    // path separator is part of the path
    ++t;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    ਜਦੋਂ (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    ਜੇ (*fname)
	++fname;
    ਵਾਪਸ fname;
}

/*
 * Get a pointer to one character past the head of a path name.
 * Unix: after "/"; DOS: after "c:\"; Amiga: after "disk:/"; Mac: no head.
 * If there is no head, path is returned.
 */
    char_u *
get_past_ਸਿਰ(char_u *ਰਸਤਾ)
{
    char_u  *retval;

#ਜੇ defined(MSWIN)
    // may skip "c:"
    ਜੇ (ਸੁਰੱਖਿਅਤ_isalpha(ਰਸਤਾ[0]) && ਰਸਤਾ[1] == ':')
	retval = ਰਸਤਾ + 2;
    ਨਹੀਂਤਾਂ
	retval = ਰਸਤਾ;
#ਨਹੀਂਤਾਂ
# ਜੇ defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(ਰਸਤਾ, ':');
    ਜੇ (retval == ਨਿਰਾ)
	retval = ਰਸਤਾ;
# ਨਹੀਂਤਾਂ	// Unix
    retval = ਰਸਤਾ;
# endif
#ਜੇ_ਅੰਤ

    ਜਦੋਂ (vim_ispathsep(*retval))
	++retval;

    ਵਾਪਸ retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    ਪੂਰਨ
vim_ispathsep(ਪੂਰਨ c)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਵਾਪਸ (c == '/');	    // UNIX has ':' inside file names
#ਨਹੀਂਤਾਂ
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
    ਵਾਪਸ (c == ':' || c == '/' || c == '\\');
# ਨਹੀਂਤਾਂ
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    ਵਾਪਸ (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  ਨਹੀਂਤਾਂ
    ਵਾਪਸ (c == ':' || c == '/');
#  endif // VMS
# endif
#ਜੇ_ਅੰਤ
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    ਪੂਰਨ
vim_ispathsep_nocolon(ਪੂਰਨ c)
{
    ਵਾਪਸ vim_ispathsep(c)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	&& c != ':'
#ਜੇ_ਅੰਤ
	;
}

/*
 * Return TRUE if the directory of "fname" exists, FALSE otherwise.
 * Also returns TRUE if there is no directory name.
 * "fname" must be writable!.
 */
    ਪੂਰਨ
dir_ਦਾ_file_exists(char_u *fname)
{
    char_u	*p;
    ਪੂਰਨ		c;
    ਪੂਰਨ		retval;

    p = gettail_sep(fname);
    ਜੇ (p == fname)
	ਵਾਪਸ TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    ਵਾਪਸ retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    ਪੂਰਨ
vim_fnamecmp(char_u *x, char_u *y)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    ਵਾਪਸ vim_fnamencmp(x, y, MAXPATHL);
#ਨਹੀਂਤਾਂ
    ਜੇ (p_fic)
	ਵਾਪਸ MB_STRICMP(x, y);
    ਵਾਪਸ STRCMP(x, y);
#ਜੇ_ਅੰਤ
}

    ਪੂਰਨ
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    ਪੂਰਨ		cx = NUL;
    ਪੂਰਨ		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    ਲਈ (ਪੂਰਨ ਅਦਲਾ_ਬਦਲੀ = 0, drive = NUL; ਅਦਲਾ_ਬਦਲੀ < 2; ++ਅਦਲਾ_ਬਦਲੀ)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	ਜੇ ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    ਜੇ (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    ਜਦੋਂ (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	ਜੇ (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    ਰੋਕੋ;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    ਜੇ (len == 0)
	ਵਾਪਸ 0;
    ਵਾਪਸ (cx - cy);
#ਨਹੀਂਤਾਂ
    ਜੇ (p_fic)
	ਵਾਪਸ MB_STRNICMP(x, y, len);
    ਵਾਪਸ STRNCMP(x, y, len);
#ਜੇ_ਅੰਤ
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, ਪੂਰਨ sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? ਆਕਾਰ(PATHSEP) : 0) + fname2len + 1);
    ਜੇ (ret->string == ਨਿਰਾ)
	ret->ਲੰਮਾਈ = 0;
    ਨਹੀਂਤਾਂ
    {
	STRCPY(ret->string, fname1);
	ret->ਲੰਮਾਈ = fname1len;
	ਜੇ (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->ਲੰਮਾਈ))
	{
	    STRCPY(ret->string + ret->ਲੰਮਾਈ, PATHSEPSTR);
	    ret->ਲੰਮਾਈ += ਆਕਾਰ(PATHSEP);
	}
	STRCPY(ret->string + ret->ਲੰਮਾਈ, fname2);
	ret->ਲੰਮਾਈ += fname2len;
    }

    ਵਾਪਸ ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    ਖਾਲੀ
ਜੋੜੋ_pathsep(char_u *p)
{
    size_t  plen;

    ਜੇ (*p == NUL)
	ਵਾਪਸ;

    plen = STRLEN(p);
    ਜੇ (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
ਭਰਿਆ_Name_save(
    char_u	*fname,
    ਪੂਰਨ		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = ਨਿਰਾ;

    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    buf = alloc(MAXPATHL);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (vim_ਭਰਿਆ_Name(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    ਨਹੀਂਤਾਂ
	new_fname = vim_strsave(fname);
    vim_free(buf);
    ਵਾਪਸ new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    ਪੂਰਨ
vim_fexists(char_u *fname)
{
    stat_T st;

    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st))
	ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    ਪੂਰਨ		  *ਅੰਕ_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ਪੂਰਨ		   flags)	// EW_DIR, etc.
{
    ਪੂਰਨ		ret = FAIL;
    char_u	*eval_pat = ਨਿਰਾ;
    char_u	*exp_pat = *pat;
    ਅੱਖਰ	*ignored_ਸੁਨੇਹਾ;
    size_t	usedlen;
    ਪੂਰਨ		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    ਪੂਰਨ		ਤਾਰਾ_follows = FALSE;

    ਜੇ (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       ਨਿਰਾ, &ignored_ਸੁਨੇਹਾ, ਨਿਰਾ, TRUE);
	--emsg_off;
	ਜੇ (eval_pat != ਨਿਰਾ)
	{
	    ਤਾਰਾ_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    ਜੇ (exp_pat != ਨਿਰਾ)
	ret = expand_wildcards(1, &exp_pat, ਅੰਕ_file, file, flags);

    ਜੇ (eval_pat != ਨਿਰਾ)
    {
	ਜੇ (*ਅੰਕ_file == 0 && is_cur_alt_file && ਤਾਰਾ_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    ਜੇ (*file != ਨਿਰਾ)
	    {
		**file = eval_pat;
		eval_pat = ਨਿਰਾ;
		*ਅੰਕ_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    ਵਾਪਸ ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    ਪੂਰਨ
expand_wildcards(
    ਪੂਰਨ		   ਅੰਕ_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    ਪੂਰਨ		  *ਅੰਕ_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    ਪੂਰਨ		   flags)	// EW_DIR, etc.
{
    ਪੂਰਨ		retval;
    ਪੂਰਨ		i, j;
    char_u	*p;
    ਪੂਰਨ		non_suf_ਮੇਲ;	// number without matching suffix

    retval = gen_expand_wildcards(ਅੰਕ_pat, pat, ਅੰਕ_files, files, flags);

    // When keeping all matches, return here
    ਜੇ ((flags & EW_KEEPALL) || retval == FAIL)
	ਵਾਪਸ retval;

    /*
     * Remove names that match 'wildignore'.
     */
    ਜੇ (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	ਲਈ (i = 0; i < *ਅੰਕ_files; ++i)
	{
	    ffname = ਭਰਿਆ_Name_save((*files)[i], FALSE);
	    ਜੇ (ffname == ਨਿਰਾ)		// out of memory
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
	    vms_remove_ਸੰਸਕਰਣ(ffname);
#ਜੇ_ਅੰਤ
	    ਜੇ (ਮੇਲ_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		ਲਈ (j = i; j + 1 < *ਅੰਕ_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*ਅੰਕ_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	ਜੇ (*ਅੰਕ_files == 0)
	{
	    VIM_CLEAR(*files);
	    ਵਾਪਸ FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    ਜੇ (*ਅੰਕ_files > 1 && !got_int)
    {
	non_suf_ਮੇਲ = 0;
	ਲਈ (i = 0; i < *ਅੰਕ_files; ++i)
	{
	    ਜੇ (!ਮੇਲ_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		ਲਈ (j = i; j > non_suf_ਮੇਲ; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_ਮੇਲ++] = p;
	    }
	}
    }

    ਵਾਪਸ retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    ਪੂਰਨ
ਮੇਲ_suffix(char_u *fname)
{
    ਪੂਰਨ		fnamelen, setsuflen;
    char_u	*setsuf;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (ਪੂਰਨ)STRLEN(fname);
    setsuflen = 0;
    ਲਈ (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	ਜੇ (setsuflen == 0)
	{
	    char_u *ਪੂਛ = gettail(fname);

	    // empty entry: match name without a '.'
	    ਜੇ (vim_strchr(ਪੂਛ, '.') == ਨਿਰਾ)
	    {
		setsuflen = 1;
		ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		ਰੋਕੋ;
	    setsuflen = 0;
	}
    }
    ਵਾਪਸ (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    ਸਥਿਰ ਪੂਰਨ
vim_backtick(char_u *p)
{
    ਵਾਪਸ (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    ਸਥਿਰ ਪੂਰਨ
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    ਪੂਰਨ		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*ਬਫਰ;
    ਪੂਰਨ		cnt = 0;
    ਪੂਰਨ		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    ਜੇ (cmd == ਨਿਰਾ)
	ਵਾਪਸ -1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (*cmd == '=')	    // `={expr}`: Expand expression
	ਬਫਰ = eval_ਮੰਜ਼ਿਲ_string(cmd + 1, TRUE, FALSE);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਬਫਰ = get_cmd_output(cmd, ਨਿਰਾ,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, ਨਿਰਾ);
    vim_free(cmd);
    ਜੇ (ਬਫਰ == ਨਿਰਾ)
	ਵਾਪਸ -1;

    cmd = ਬਫਰ;
    ਜਦੋਂ (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	ਜਦੋਂ (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	ਜੇ (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	ਜਦੋਂ (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(ਬਫਰ);
    ਵਾਪਸ cnt;
}

#ਜੇ defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    ਸਥਿਰ ਪੂਰਨ
pstrcmp(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    ਵਾਪਸ (pathcmp(*(ਅੱਖਰ **)a, *(ਅੱਖਰ **)b, -1));
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
    ਪੂਰਨ
dos_expandpath(
    garray_T	*gap,
    char_u	*ਰਸਤਾ,
    size_t	wildoff,
    ਪੂਰਨ		flags,		// EW_* flags
    ਪੂਰਨ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*ਰਸਤਾ_ਸਿਰਾ;
    char_u	*p, *s, *e;
    ਪੂਰਨ		ਸ਼ੁਰੂ_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ਪੂਰਨ		starts_with_dot;
    ਪੂਰਨ		matches;
    size_t	buflen;
    size_t	len;
    ਪੂਰਨ		starstar = FALSE;
    ਸਥਿਰ ਪੂਰਨ	stardepth = 0;	    // depth for "**" expansion
    HANDLE		h_ਲੱਭੋ = INVALID_HANDLE_ਮੁੱਲ;
    WIN32_ਲੱਭੋ_DATAW    wfb;
    WCHAR		*wn = ਨਿਰਾ;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    ਪੂਰਨ			ok;
    char_u		*p_alt;
    ਪੂਰਨ			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    ਜੇ (stardepth > 0)
    {
	ui_breakcheck();
	ਜੇ (got_int)
	    ਵਾਪਸ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(ਰਸਤਾ) + MAXPATHL;
    buf = alloc(buflen);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ਨਿਰਾ;
    ਰਸਤਾ_ਸਿਰਾ = ਰਸਤਾ;
    ਜਦੋਂ (*ਰਸਤਾ_ਸਿਰਾ != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ਜੇ (ਰਸਤਾ_ਸਿਰਾ >= ਰਸਤਾ + wildoff && rem_backslash(ਰਸਤਾ_ਸਿਰਾ))
	    *p++ = *ਰਸਤਾ_ਸਿਰਾ++;
	ਨਹੀਂਤਾਂ ਜੇ (*ਰਸਤਾ_ਸਿਰਾ == '\\' || *ਰਸਤਾ_ਸਿਰਾ == ':' || *ਰਸਤਾ_ਸਿਰਾ == '/')
	{
	    ਜੇ (e != ਨਿਰਾ)
		ਰੋਕੋ;
	    s = p + 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਰਸਤਾ_ਸਿਰਾ >= ਰਸਤਾ + wildoff
			 && vim_strchr((char_u *)"*?[~", *ਰਸਤਾ_ਸਿਰਾ) != ਨਿਰਾ)
	    e = p;
	ਜੇ (has_mbyte)
	{
	    ਪੂਰਨ charlen = (*mb_ptr2len)(ਰਸਤਾ_ਸਿਰਾ);

	    STRNCPY(p, ਰਸਤਾ_ਸਿਰਾ, (size_t)charlen);
	    p += charlen;
	    ਰਸਤਾ_ਸਿਰਾ += charlen;
	}
	ਨਹੀਂਤਾਂ
	    *p++ = *ਰਸਤਾ_ਸਿਰਾ++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    ਲਈ (p = buf + wildoff; p < s; ++p)
	ਜੇ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    ਲਈ (p = s; p < e; ++p)
	ਜੇ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_ਮੰਜ਼ਿਲ_reg_pat(s, e, ਨਿਰਾ, FALSE);
    ਜੇ (pat == ਨਿਰਾ)
    {
	vim_free(buf);
	ਵਾਪਸ 0;
    }

    // compile the regexp into a program
    ਜੇ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ਜੇ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ਜੇ (regmatch.regprog == ਨਿਰਾ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ਵਾਪਸ 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != ਨਿਰਾ;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ਜੇ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *ਰਸਤਾ_ਸਿਰਾ == '/')
    {
	vim_snprintf((ਅੱਖਰ *)s, buflen - len, "%s", ਰਸਤਾ_ਸਿਰਾ + 1);
	++stardepth;
	(ਖਾਲੀ)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((ਅੱਖਰ *)s, buflen - len, "*.*");
    wn = enc_ਮੰਜ਼ਿਲ_utf16(buf, ਨਿਰਾ);
    ਜੇ (wn != ਨਿਰਾ)
	h_ਲੱਭੋ = ਲੱਭੋ_ਪਹਿਲਾ_File_W(wn, &wfb);
    ok = (h_ਲੱਭੋ != INVALID_HANDLE_ਮੁੱਲ);

    ਜਦੋਂ (ok)
    {
	p = utf16_ਮੰਜ਼ਿਲ_enc(wfb.cFileName, ਨਿਰਾ);   // p is allocated here

	ਜੇ (p == ਨਿਰਾ)
	    ਰੋਕੋ;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	ਜੇ (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    p_alt = utf16_ਮੰਜ਼ਿਲ_enc(wfb.cAlternateFileName, ਨਿਰਾ);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	ਜੇ ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == ਨਿਰਾ
		  || (regmatch.regprog != ਨਿਰਾ
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != ਨਿਰਾ
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(ਰਸਤਾ + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((ਅੱਖਰ *)s, buflen - len, "%s", p);
	    ਜੇ (len + 1 < buflen)
	    {
		ਜੇ (starstar && stardepth < 100
			  && (wfb.dw_File_ਗੁਣ & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((ਅੱਖਰ *)buf + len, buflen - len,
							    "/**%s", ਰਸਤਾ_ਸਿਰਾ);
		    ++stardepth;
		    (ਖਾਲੀ)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((ਅੱਖਰ *)buf + len, buflen - len, "%s", ਰਸਤਾ_ਸਿਰਾ);
		ਜੇ (mch_has_exp_wildcard(ਰਸਤਾ_ਸਿਰਾ))
		{
		    ਜੇ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ਖਾਲੀ)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ਜੇ (*ਰਸਤਾ_ਸਿਰਾ != 0)
			backslash_halve(buf + len + 1);
		    // addfile() checks existence; do not stat the same path twice.
		    // Even with EW_NOTFOUND, a literal suffix must exist.
		    addfile(gap, buf, flags & ~EW_NOTFOUND);
		}
	    }
	}

	vim_free(p_alt);
	vim_free(p);
	ok = ਲੱਭੋ_ਅਗਲਾ_File_W(h_ਲੱਭੋ, &wfb);
    }

    ਲੱਭੋ_Close(h_ਲੱਭੋ);
    vim_free(wn);
    vim_free(buf);
    vim_regfree(regmatch.regprog);
    vim_free(matchname);

    matches = gap->ga_len - ਸ਼ੁਰੂ_len;
    ਜੇ (matches > 0)
	qsort(((char_u **)gap->ga_ਅੰਕੜੇ) + ਸ਼ੁਰੂ_len, (size_t)matches,
						   ਆਕਾਰ(char_u *), pstrcmp);
    ਵਾਪਸ matches;
}
#ਜੇ_ਅੰਤ // MSWIN

#ਜੇ (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    ਸਥਿਰ ਪੂਰਨ
pstrcmp(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    ਵਾਪਸ (pathcmp(*(ਅੱਖਰ **)a, *(ਅੱਖਰ **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    ਪੂਰਨ
unix_expandpath(
    garray_T	*gap,
    char_u	*ਰਸਤਾ,
    size_t	wildoff,
    ਪੂਰਨ		flags,		// EW_* flags
    ਪੂਰਨ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*ਰਸਤਾ_ਸਿਰਾ;
    char_u	*p, *s, *e;
    ਪੂਰਨ		ਸ਼ੁਰੂ_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ਪੂਰਨ		starts_with_dot;
    ਪੂਰਨ		matches;
    size_t	buflen;
    size_t	len;
    ਪੂਰਨ		starstar = FALSE;
    ਸਥਿਰ ਪੂਰਨ	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    ਬਣਤਰ dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    ਜੇ (stardepth > 0)
    {
	ui_breakcheck();
	ਜੇ (got_int)
	    ਵਾਪਸ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(ਰਸਤਾ) + MAXPATHL;
    buf = alloc(buflen);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ਨਿਰਾ;
    ਰਸਤਾ_ਸਿਰਾ = ਰਸਤਾ;
    ਜਦੋਂ (*ਰਸਤਾ_ਸਿਰਾ != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ਜੇ (ਰਸਤਾ_ਸਿਰਾ >= ਰਸਤਾ + wildoff && rem_backslash(ਰਸਤਾ_ਸਿਰਾ))
	    *p++ = *ਰਸਤਾ_ਸਿਰਾ++;
	ਨਹੀਂਤਾਂ ਜੇ (*ਰਸਤਾ_ਸਿਰਾ == '/')
	{
	    ਜੇ (e != ਨਿਰਾ)
		ਰੋਕੋ;
	    s = p + 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਰਸਤਾ_ਸਿਰਾ >= ਰਸਤਾ + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *ਰਸਤਾ_ਸਿਰਾ) != ਨਿਰਾ
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(ਰਸਤਾ_ਸਿਰਾ)))))
	    e = p;
	ਜੇ (has_mbyte)
	{
	    ਪੂਰਨ charlen = (*mb_ptr2len)(ਰਸਤਾ_ਸਿਰਾ);

	    STRNCPY(p, ਰਸਤਾ_ਸਿਰਾ, (size_t)charlen);
	    p += charlen;
	    ਰਸਤਾ_ਸਿਰਾ += charlen;
	}
	ਨਹੀਂਤਾਂ
	    *p++ = *ਰਸਤਾ_ਸਿਰਾ++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    ਲਈ (p = buf + wildoff; p < s; ++p)
	ਜੇ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    ਲਈ (p = s; p < e; ++p)
	ਜੇ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_ਮੰਜ਼ਿਲ_reg_pat(s, e, ਨਿਰਾ, FALSE);
    ਜੇ (pat == ਨਿਰਾ)
    {
	vim_free(buf);
	ਵਾਪਸ 0;
    }

    // compile the regexp into a program
    ਜੇ (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    ਨਹੀਂਤਾਂ
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ਜੇ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ਜੇ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ਜੇ (regmatch.regprog == ਨਿਰਾ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ਵਾਪਸ 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ਜੇ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *ਰਸਤਾ_ਸਿਰਾ == '/')
    {
	vim_snprintf((ਅੱਖਰ *)s, buflen - len, "%s", ਰਸਤਾ_ਸਿਰਾ + 1);
	++stardepth;
	(ਖਾਲੀ)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (ਅੱਖਰ *)buf);

    // Find all matching entries
    ਜੇ (dirp != ਨਿਰਾ)
    {
	ਜਦੋਂ (!got_int)
	{
	    dp = readdir(dirp);
	    ਜੇ (dp == ਨਿਰਾ)
		ਰੋਕੋ;
	    len = (size_t)(s - buf);
	    ਜੇ ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != ਨਿਰਾ && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(ਰਸਤਾ + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((ਅੱਖਰ *)s, buflen - len, "%s", dp->d_name);
		ਜੇ (len + 1 >= buflen)
		    ਜਾਰੀ;

		ਜੇ (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((ਅੱਖਰ *)buf + len, buflen - len,
							    "/**%s", ਰਸਤਾ_ਸਿਰਾ);
		    ++stardepth;
		    (ਖਾਲੀ)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((ਅੱਖਰ *)buf + len, buflen - len, "%s", ਰਸਤਾ_ਸਿਰਾ);
		ਜੇ (mch_has_exp_wildcard(ਰਸਤਾ_ਸਿਰਾ)) // handle more wildcards
		{
		    ਜੇ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ਖਾਲੀ)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ਜੇ (*ਰਸਤਾ_ਸਿਰਾ != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    ਜੇ ((flags & EW_ALLLINKS) ? mch_lstat((ਅੱਖਰ *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_ਰਸਤਾ(buf, precomp_len, &precomp_len);

			ਜੇ (precomp_buf)
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
    matches = gap->ga_len - ਸ਼ੁਰੂ_len;
    ਜੇ (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_ਅੰਕੜੇ) + ਸ਼ੁਰੂ_len, matches,
						   ਆਕਾਰ(char_u *), pstrcmp);
    ਵਾਪਸ matches;
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    ਸਥਿਰ ਪੂਰਨ
has_env_var(char_u *p)
{
    ਲਈ ( ; *p; MB_PTR_ADV(p))
    {
	ਜੇ (*p == '\\' && p[1] != NUL)
	    ++p;
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)
#ਜੇ defined(MSWIN)
				    "$%"
#ਨਹੀਂਤਾਂ
				    "$"
#ਜੇ_ਅੰਤ
					, *p) != ਨਿਰਾ)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    ਸਥਿਰ ਪੂਰਨ
has_special_wildchar(char_u *p)
{
    ਲਈ ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	ਜੇ (*p == '\r' || *p == '\n')
	    ਰੋਕੋ;
	// Allow for escaping.
	ਜੇ (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != ਨਿਰਾ)
	{
	    // A { must be followed by a matching }.
	    ਜੇ (*p == '{' && vim_strchr(p, '}') == ਨਿਰਾ)
		ਜਾਰੀ;
	    // A quote and backtick must be followed by another one.
	    ਜੇ ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == ਨਿਰਾ)
		ਜਾਰੀ;
	    ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
gen_expand_wildcards(
    ਪੂਰਨ		ਅੰਕ_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    ਪੂਰਨ		*ਅੰਕ_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ਪੂਰਨ		flags)		// EW_* flags
{
    ਪੂਰਨ			i;
    garray_T		ga;
    char_u		*p;
    ਸਥਿਰ ਪੂਰਨ		recursive = FALSE;
    ਪੂਰਨ			ਜੋੜੋ_pat;
    ਪੂਰਨ			retval = OK;
    ਪੂਰਨ			did_expand_ਵਿੱਚ_ਰਸਤਾ = FALSE;
    char_u		*ਰਸਤਾ_option = *curbuf->b_p_ਰਸਤਾ == NUL ?
					p_ਰਸਤਾ : curbuf->b_p_ਰਸਤਾ;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    ਜੇ (recursive)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPECIAL_WILDCHAR
	ਵਾਪਸ mch_expand_wildcards(ਅੰਕ_pat, pat, ਅੰਕ_file, file, flags);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    ਲਈ (i = 0; i < ਅੰਕ_pat; i++)
    {
	ਜੇ (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    ਵਾਪਸ mch_expand_wildcards(ਅੰਕ_pat, pat, ਅੰਕ_file, file, flags);
    }
#ਜੇ_ਅੰਤ

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, ਆਕਾਰ(char_u *), 30);

    ਲਈ (i = 0; i < ਅੰਕ_pat && !got_int; ++i)
    {
	ਜੋੜੋ_pat = -1;
	p = pat[i];

	ਜੇ (vim_backtick(p))
	{
	    ਜੋੜੋ_pat = expand_backtick(&ga, p, flags);
	    ਜੇ (ਜੋੜੋ_pat == -1)
		retval = FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    ਜੇ ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)ਰਸਤਾ_ESC_WILDCARDS);
		ਜੇ (p == ਨਿਰਾ)
		    p = pat[i];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		ਨਹੀਂਤਾਂ ਜੇ (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_ਲੜੀਆਂ(&ga);
		    i = mch_expand_wildcards(ਅੰਕ_pat, pat, ਅੰਕ_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    ਵਾਪਸ i;
		}
#ਜੇ_ਅੰਤ
	    }

	    /*
	     * If there are wildcards or case-insensitive expansion is
	     * required: Expand file names and add each match to the list.  If
	     * there is no match, and EW_NOTFOUND is given, add the pattern.
	     * Otherwise: Add the file name if it exists or when EW_NOTFOUND is
	     * given.
	     */
	    ਜੇ (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		ਜੇ ((flags & (EW_ਰਸਤਾ | EW_CDPATH))
			&& !mch_is_ਭਰਿਆ_Name(p)
			&& !(p[0] == '.'
			    && (vim_ispathsep(p[1])
				|| (p[1] == '.' && vim_ispathsep(p[2]))))
		   )
		{
		    // :find completion where 'path' is used.
		    // Recursiveness is OK here.
		    recursive = FALSE;
		    ਜੋੜੋ_pat = expand_ਵਿੱਚ_ਰਸਤਾ(&ga, p, flags);
		    recursive = TRUE;
		    did_expand_ਵਿੱਚ_ਰਸਤਾ = TRUE;
		}
		ਨਹੀਂਤਾਂ
		    ਜੋੜੋ_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	ਜੇ (ਜੋੜੋ_pat == -1 || (ਜੋੜੋ_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    ਜੇ (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    ਨਹੀਂਤਾਂ
		addfile(&ga, t, flags);

	    ਜੇ (t != p)
		vim_free(t);
	}

	ਜੇ (did_expand_ਵਿੱਚ_ਰਸਤਾ && ga.ga_len > 0 && (flags & (EW_ਰਸਤਾ | EW_CDPATH)))
	    uniquefy_paths(&ga, p, ਰਸਤਾ_option);
	ਜੇ (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    ਜੇ (retval == FAIL)
	ga_clear_ਲੜੀਆਂ(&ga);

    *ਅੰਕ_file = ga.ga_len;
    *file = (ga.ga_ਅੰਕੜੇ != ਨਿਰਾ) ? (char_u **)ga.ga_ਅੰਕੜੇ
						  : (char_u **)_("no matches");

    recursive = FALSE;

    ਵਾਪਸ ((flags & EW_EMPTYOK) || ga.ga_ਅੰਕੜੇ != ਨਿਰਾ) ? retval : FAIL;
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
    ਖਾਲੀ
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    ਪੂਰਨ		flags)
{
    char_u	*p;
    ਪੂਰਨ		isdir;
    stat_T	sb;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	ਲੰਮਾ perm = mch_getperm(f);

	ਜੇ (perm < 0)
	    ਵਾਪਸ;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	// if the file/dir/link doesn't exist, may not add it
	ਜੇ (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((ਅੱਖਰ *)f, &sb) < 0 : mch_getperm(f) < 0))
	    ਵਾਪਸ;
	isdir = mch_isdir(f);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    ਜੇ (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != ਨਿਰਾ)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    ਜੇ ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	ਵਾਪਸ;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    ਜੇ (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, ਨਿਰਾ, !(flags & EW_SHELLCMD)))
	ਵਾਪਸ;

    // Make room for another item in the file list.
    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ;

    p = alloc(STRLEN(f) + 1 + isdir);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    STRCPY(p, f);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    slash_adjust(p);
#ਜੇ_ਅੰਤ
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    ਜੇ (isdir && (flags & EW_ADDSLASH))
	ਜੋੜੋ_pathsep(p);
    ((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    ਖਾਲੀ
FreeWild(ਪੂਰਨ ਗਿਣਤੀ_2, char_u **files)
{
    ਜੇ (ਗਿਣਤੀ_2 <= 0 || files == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜਦੋਂ (ਗਿਣਤੀ_2--)
	vim_free(files[ਗਿਣਤੀ_2]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    ਪੂਰਨ
pathcmp(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *q, ਪੂਰਨ maxlen)
{
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		c1, c2;
    ਅਚਲ ਅੱਖਰ	*s = ਨਿਰਾ;

    ਲਈ (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	ਜੇ (c1 == NUL)
	{
	    ਜੇ (c2 == NUL)  // full match
		ਵਾਪਸ 0;
	    s = q;
	    i = j;
	    ਰੋਕੋ;
	}

	// End of "q": check if "p" just has a slash.
	ਜੇ (c2 == NUL)
	{
	    s = p;
	    ਰੋਕੋ;
	}

	ਜੇ ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#ਜੇ_ਅੰਤ
		)
	{
	    ਜੇ (vim_ispathsep(c1))
		ਵਾਪਸ -1;
	    ਜੇ (vim_ispathsep(c2))
		ਵਾਪਸ 1;
	    ਵਾਪਸ p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    ਜੇ (s == ਨਿਰਾ)	// "i" or "j" ran into "maxlen"
	ਵਾਪਸ 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    ਜੇ (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    && (c1 == '/' || c1 == '\\')
#ਨਹੀਂਤਾਂ
	    && c1 == '/'
#ਜੇ_ਅੰਤ
       )
	ਵਾਪਸ 0;   // match with trailing slash
    ਜੇ (s == q)
	ਵਾਪਸ -1;	    // no match
    ਵਾਪਸ 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    ਪੂਰਨ
vim_isAbsName(char_u *name)
{
    ਵਾਪਸ (ਰਸਤਾ_with_url(name) != 0 || mch_is_ਭਰਿਆ_Name(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
vim_ਭਰਿਆ_Name(
    char_u	*fname,
    char_u	*buf,
    ਪੂਰਨ		len,
    ਪੂਰਨ		force)	    // force expansion even when already absolute
{
    ਪੂਰਨ		retval = OK;
    ਪੂਰਨ		url;

    *buf = NUL;
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    url = ਰਸਤਾ_with_url(fname);
    ਜੇ (!url)
	retval = mch_ਭਰਿਆ_Name(fname, buf, len, force);
    ਜੇ (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#ਜੇ defined(MSWIN)
    slash_adjust(buf);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ retval;
}
