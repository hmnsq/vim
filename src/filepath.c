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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    ಪೂರ್ಣಾಂಕ		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = ಸ್ಮರಣೆ_ಕೊಡು(len * ಗಾತ್ರ(*newbuf));
    ಆದರೆ (newbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    wfname = enc_ಗಮ್ಯ_utf16(*fnamep, ಖಾಲಿ);
    ಆದರೆ (wfname == ಖಾಲಿ)
    {
	vim_free(newbuf);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    l = Get_Short_ಮಾರ್ಗ_Name_W(wfname, newbuf, len);
    ಆದರೆ (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * ಗಾತ್ರ(*newbuf));
	ಆದರೆ (newbuf == ಖಾಲಿ)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = Get_Short_ಮಾರ್ಗ_Name_W(wfname, newbuf, l+1);
    }
    ಆದರೆ (l != 0)
    {
	char_u *p = utf16_ಗಮ್ಯ_enc(newbuf, ಖಾಲಿ);

	ಆದರೆ (p != ಖಾಲಿ)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_ಬಳಸದ = ಖಾಲಿ;
    char_u	*short_fname = ಖಾಲಿ;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    ಪೂರ್ಣಾಂಕ		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    ಆದರೆ (save_fname == ಖಾಲಿ)
    {
	retval = FAIL;
	ಹೋಗು theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    ಗಾಗಿ (;;)
    {
	// go back one path-separator
	ಆಗ (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	ಆದರೆ (endp <= save_fname)
	    ನಿಲ್ಲಿಸು;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	ಆದರೆ (get_short_pathname(&short_fname, &pbuf_ಬಳಸದ, &len) == FAIL)
	{
	    retval = FAIL;
	    ಹೋಗು theend;
	}
	*endp = ch;	// preserve the string

	ಆದರೆ (len > 0)
	    ನಿಲ್ಲಿಸು;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    ಆದರೆ (len > 0)
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
	ಆದರೆ (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    ಆದರೆ (*fname == ಖಾಲಿ)
	    {
		retval = FAIL;
		ಹೋಗು theend;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    ಆದರೆ (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = ಖಾಲಿ;
	}

	// concat the not-shortened part of the path
	ಆದರೆ ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_ಬಳಸದ);
    vim_free(save_fname);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    ಪೂರ್ಣಾಂಕ		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    ಪೂರ್ಣಾಂಕ		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    ಗಾಗಿ (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	ಆದರೆ (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    ಆದರೆ (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    ಇಲ್ಲದಿದ್ದರೆ
	pbuf = tfname = ತುಂಬಿದ_Name_save(*fnamep, FALSE);
    ಆದರೆ (tfname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    len = tflen = STRLEN(tfname);

    ಆದರೆ (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	ಆದರೆ (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    ಗಾಗಿ (p = tfname + len - 1; p >= tfname; --p)
    {
	ಆದರೆ (has_mbyte)
	    p -= mb_ತಲೆ_off(tfname, p);
	ಆದರೆ (vim_ispathsep(*p))
	{
	    ಆದರೆ (sepcount == 0 || (hasTilde && sepcount == 1))
		ನಿಲ್ಲಿಸು;
	    ಇಲ್ಲದಿದ್ದರೆ
		sepcount --;
	}
    }
    ಆದರೆ (hasTilde)
    {
	--p;
	ಆದರೆ (p >= tfname)
	    *p = '~';
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    ಪೂರ್ಣಾಂಕ
modify_fname(
    char_u	*src,		// string with modifiers
    ಪೂರ್ಣಾಂಕ		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    ಪೂರ್ಣಾಂಕ		valid = 0;
    char_u	*ಬಾಲ;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		has_fullname = 0;
    ಪೂರ್ಣಾಂಕ		has_homerelative = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    char_u	*fname_ಆರಂಭ = *fnamep;
    ಪೂರ್ಣಾಂಕ		has_shortname = 0;
#ಆದರೆ_ಅಂತ್ಯ

repeat:
    // ":p" - full path/file_name
    ಆದರೆ (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_ಮಾರ್ಗ;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	ಆದರೆ ((*fnamep)[0] == '~'
#ಆದರೆ !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
		&& ((*fnamep)[1] == '/'
# ifdef BACKSLASH_ಒಳ_FILENAME
		    || (*fnamep)[1] == '\\'
# endif
		    || (*fnamep)[1] == NUL)
#ಆದರೆ_ಅಂತ್ಯ
		&& !(tilde_file && (*fnamep)[1] == NUL)
	   )
	{
	    *fnamep = expand_env_save(*fnamep);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ಆದರೆ (*fnamep == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	ಗಾಗಿ (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    ಆದರೆ (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		ನಿಲ್ಲಿಸು;
	}

	// FullName_save() is slow, don't use it when not needed.
	ಆದರೆ (*p != NUL || !vim_isAbsName(*fnamep)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#ಆದರೆ_ಅಂತ್ಯ
	)
	{
	    *fnamep = ತುಂಬಿದ_Name_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ಆದರೆ (*fnamep == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ -1;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# ಆದರೆ _WIN32_WINNT >= 0x0500
	ಆದರೆ (vim_strchr(*fnamep, '~') != ಖಾಲಿ)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_ಗಮ್ಯ_utf16(*fnamep, ಖಾಲಿ);
	    WCHAR buf[MAX_ಮಾರ್ಗ_2];

	    ಆದರೆ (wfname != ಖಾಲಿ)
	    {
		ಆದರೆ (Get_Long_ಮಾರ್ಗ_Name_W(wfname, buf, MAX_ಮಾರ್ಗ_2))
		{
		    char_u *q = utf16_ಗಮ್ಯ_enc(buf, ಖಾಲಿ);

		    ಆದರೆ (q != ಖಾಲಿ)
		    {
			vim_free(*bufp);    // free any allocated file name
			*bufp = *fnamep = q;
		    }
		}
		vim_free(wfname);
	    }
	}
# endif
#ಆದರೆ_ಅಂತ್ಯ
	// Append a path separator to a directory.
	ಆದರೆ (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ಆದರೆ (*fnamep == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ -1;
	    ಕೂಡಿಸು_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    ಆಗ (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	ಆದರೆ (c == '8')
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    has_shortname = 1; // Postpone this.
#ಆದರೆ_ಅಂತ್ಯ
	    ಮುಂದುವರಿ;
	}
	pbuf = ಖಾಲಿ;
	// Need full path first (use expand_env() to remove a "~/")
	ಆದರೆ (!has_fullname && !has_homerelative)
	{
	    ಆದರೆ (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    ಇಲ್ಲದಿದ್ದರೆ
		p = pbuf = ತುಂಬಿದ_Name_save(*fnamep, FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    p = *fnamep;

	has_fullname = 0;

	ಆದರೆ (p != ಖಾಲಿ)
	{
	    size_t  dirnamelen = 0;

	    ಆದರೆ (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		ಆದರೆ (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    ಆದರೆ (s != ಖಾಲಿ)
		    {
			dirnamelen = home_replace(ಖಾಲಿ, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		ಆದರೆ (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		ಆದರೆ (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    ಆದರೆ (vim_ispathsep(*p))
		    {
			ಆಗ (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			ಆದರೆ (pbuf != ಖಾಲಿ)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = ಖಾಲಿ;
			}
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		dirnamelen = home_replace(ಖಾಲಿ, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		ಆದರೆ (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    ಆದರೆ (s != ಖಾಲಿ)
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

    ಬಾಲ = gettail(*fnamep);
    *fnamelen = STRLEN(*fnamep);

    // ":h" - head, remove "/file_name", can be repeated
    // Don't remove the first "/" or "c:\"
    ಆಗ (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_ತಲೆ;
	*usedlen += 2;
	s = get_past_ತಲೆ(*fnamep);
	ಆಗ (ಬಾಲ > s && after_pathsep(s, ಬಾಲ))
	    MB_PTR_ಹಿಂಭಾಗ(*fnamep, ಬಾಲ);
	*fnamelen = ಬಾಲ - *fnamep;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	ಆದರೆ (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    ಆದರೆ (p == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = ಬಾಲ = p;
	    *fnamelen = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆಗ (ಬಾಲ > s && !after_pathsep(s, ಬಾಲ))
		MB_PTR_ಹಿಂಭಾಗ(*fnamep, ಬಾಲ);
	}
    }

    // ":8" - shortname
    ಆದರೆ (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
    {
	*usedlen += 2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	has_shortname = 1;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    /*
     * Handle ":8" after we have done 'heads' and before we do 'tails'.
     */
    ಆದರೆ (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	ಆದರೆ (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_ಆರಂಭ)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    ಆದರೆ (p == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	ಆದರೆ (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    ಆದರೆ (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    ಆದರೆ (get_short_pathname(fnamep, bufp, &l) == FAIL)
		ಹಿಂತಿರುಗಿ -1;

	    ಆದರೆ (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		ಆದರೆ (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    ಹಿಂತಿರುಗಿ -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	ಬಾಲ = gettail(*fnamep);
    }
#ಆದರೆ_ಅಂತ್ಯ // MSWIN

    // ":t" - tail, just the basename
    ಆದರೆ (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= ಬಾಲ - *fnamep;
	*fnamep = ಬಾಲ;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    ಆಗ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	ಆದರೆ (src[*usedlen + 1] == 'e' && *fnamep > ಬಾಲ)
	    s = *fnamep - 2;
	ಇಲ್ಲದಿದ್ದರೆ
	    s = *fnamep + *fnamelen - 1;
	ಗಾಗಿ ( ; s > ಬಾಲ; --s)
	    ಆದರೆ (s[0] == '.')
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (src[*usedlen + 1] == 'e')		// :e
	{
	    ಆದರೆ (s > ಬಾಲ)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		ಗಾಗಿ ( ; s > *fnamep; --s)
		    ಆದರೆ (s[0] == ';')
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (s > *fnamep)
		    *fnamelen = s - *fnamep;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*fnamep <= ಬಾಲ)
		*fnamelen = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ				// :r
	{
	    char_u *ಎಲ್ಲೆ = *fnamep;

	    ಆದರೆ (ಎಲ್ಲೆ < ಬಾಲ)
		ಎಲ್ಲೆ = ಬಾಲ;
	    ಆದರೆ (s > ಎಲ್ಲೆ)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    ಆದರೆ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	ಪೂರ್ಣಾಂಕ	    sep;
	char_u	    *flags;
	ಪೂರ್ಣಾಂಕ	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	ಆದರೆ (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	ಆದರೆ (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		pat = vim_strnsave(s, p - s);
		ಆದರೆ (pat != ಖಾಲಿ)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			ಆದರೆ (sub != ಖಾಲಿ && str != ಖಾಲಿ)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, ಖಾಲಿ, flags, &slen);
			    ಆದರೆ (s != ಖಾಲಿ)
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
	    ಆದರೆ (didit)
		ಹೋಗು repeat;
	}
    }

    ಆದರೆ (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	ಆದರೆ (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	ಆದರೆ (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    ಹಿಂತಿರುಗಿ valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
shorten_dir_len(char_u *str, ಪೂರ್ಣಾಂಕ trim_len)
{
    char_u	*ಬಾಲ, *s, *d;
    ಪೂರ್ಣಾಂಕ		skip = FALSE;
    ಪೂರ್ಣಾಂಕ		dirchunk_len = 0;

    ಬಾಲ = gettail(str);
    d = str;
    ಗಾಗಿ (s = str; ; ++s)
    {
	ಆದರೆ (s >= ಬಾಲ)		    // copy the whole tail
	{
	    *d++ = *s;
	    ಆದರೆ (*s == NUL)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!skip)
	{
	    *d++ = *s;			// copy next char
	    ಆದರೆ (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		ಆದರೆ (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    ಆದರೆ (has_mbyte)
	    {
		ಪೂರ್ಣಾಂಕ l = mb_ptr2len(s);

		ಆಗ (--l > 0)
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
    ಪೂರ್ಣಾಂಕ
file_is_readable(char_u *fname)
{
    ಪೂರ್ಣಾಂಕ		fd;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ O_NONBLOCK
# define O_NONBLOCK 0
#ಆದರೆ_ಅಂತ್ಯ
    // Opening a file that does not exist is slow.
    ಆದರೆ (*fname == NUL || mch_isdir_or_missing(fname))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ ((fd = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    ಶೂನ್ಯ
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	ಆದರೆ (ಒಳ_vim9script())
	    (ಶೂನ್ಯ) check_for_string_arg(argvars, 0);
	ಹಿಂತಿರುಗಿ;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    ಆದರೆ (cwd != ಖಾಲಿ)
    {
	ಆದರೆ (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_ಒಳ_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	ಆದರೆ (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_ಕಿಟಕಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "scope", s);
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_localdir != ಖಾಲಿ)
	scope = CDSCOPE_ಕಿಟಕಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curtab->tp_localdir != ಖಾಲಿ)
	scope = CDSCOPE_TABPAGE;

    ಆದರೆ (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
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
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);
    ಆದರೆ (name == ಖಾಲಿ || *name == NUL)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    ಇಲ್ಲದಿದ್ದರೆ
	flags = (char_u *)"";

    ಆದರೆ (*flags == NUL)
	// delete a file
	rettv->vval.v_ಸಂಖ್ಯೆ = mch_remove(name) == 0 ? 0 : -1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_ಸಂಖ್ಯೆ = mch_rmdir(name) == 0 ? 0 : -1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_ಸಂಖ್ಯೆ = delete_recursive(name);
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), flags);
}

/*
 * "executable()" function
 */
    ಶೂನ್ಯ
f_executable(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_ಸಂಖ್ಯೆ = mch_can_exe(tv_get_string(&argvars[0]), ಖಾಲಿ, TRUE);
}

/*
 * "exepath()" function
 */
    ಶೂನ್ಯ
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;
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
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;
    rettv->vval.v_ಸಂಖ್ಯೆ = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    ಶೂನ್ಯ
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;
    rettv->vval.v_ಸಂಖ್ಯೆ = filewritable(tv_get_string(&argvars[0]));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    ಪೂರ್ಣಾಂಕ		ಕಂಡುಹಿಡಿ_what)
{
    char_u	*fname;
    char_u	*fresult = ಖಾಲಿ;
    char_u	*ಮಾರ್ಗ = *curbuf->b_p_ಮಾರ್ಗ == NUL ? p_ಮಾರ್ಗ : curbuf->b_p_ಮಾರ್ಗ;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 1;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = TRUE;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    rettv->vval.v_string = ಖಾಲಿ;
    rettv->v_type = VAR_STRING;
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    fname = tv_get_string(&argvars[0]);

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	ಆದರೆ (p == ಖಾಲಿ)
	    error = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*p != NUL)
		ಮಾರ್ಗ = p;

	    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
		ಎಣಿಕೆ_2 = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	}
    }

    ಆದರೆ (ಎಣಿಕೆ_2 < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    ಆದರೆ (*fname != NUL && !error)
    {
	char_u	*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ = ಖಾಲಿ;
	ಅಕ್ಷರ	*ಹುಡುಕು_ctx = ಖಾಲಿ;

	ಮಾಡು
	{
	    ಆದರೆ (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ_option(ಮೊದಲ ? fname : ಖಾಲಿ,
					       ಮೊದಲ ? (ಪೂರ್ಣಾಂಕ)STRLEN(fname) : 0,
					0, ಮೊದಲ, ಮಾರ್ಗ,
					ಕಂಡುಹಿಡಿ_what,
					curbuf->b_ffname,
					ಕಂಡುಹಿಡಿ_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, &ಹುಡುಕು_ctx);
	    ಮೊದಲ = FALSE;

	    ಆದರೆ (fresult != ಖಾಲಿ && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} ಆಗ ((rettv->v_type == VAR_LIST || --ಎಣಿಕೆ_2 > 0) && fresult != ಖಾಲಿ);

	vim_free(file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
	vim_findfile_cleanup(ಹುಡುಕು_ctx);
    }

    ಆದರೆ (rettv->v_type == VAR_STRING)
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
    char_u	*fbuf = ಖಾಲಿ;
    char_u	buf[NUMBUFLEN];

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    ಆದರೆ (mods == ಖಾಲಿ || fname == ಖಾಲಿ)
	fname = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	len = STRLEN(fname);
	ಆದರೆ (mods != ಖಾಲಿ && *mods != NUL)
	    (ಶೂನ್ಯ)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    ಆದರೆ (fname == ಖಾಲಿ)
	rettv->vval.v_string = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
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
    win_T	*wp = ಖಾಲಿ;
    tabpage_T	*tp = ಖಾಲಿ;
    char_u	*cwd;
    ಪೂರ್ಣಾಂಕ		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ
	    && argvars[0].vval.v_ಸಂಖ್ಯೆ == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	wp = ಕಂಡುಹಿಡಿ_tabwin(&argvars[0], &argvars[1], &tp);

    ಆದರೆ (wp != ಖಾಲಿ && wp->w_localdir != ಖಾಲಿ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tp != ಖಾಲಿ && tp->tp_localdir != ಖಾಲಿ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp != ಖಾಲಿ || tp != ಖಾಲಿ || global)
    {
	ಆದರೆ (globaldir != ಖಾಲಿ && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cwd = alloc(MAXPATHL);
	    ಆದರೆ (cwd != ಖಾಲಿ)
	    {
		ಆದರೆ (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_ಒಳ_FILENAME
    ಆದರೆ (rettv->vval.v_string != ಖಾಲಿ)
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
    ಪೂರ್ಣಾಂಕ		    i;

    ಗಾಗಿ (i = 0; i < 9; i++)
    {
	ಆದರೆ (st->st_ಬಹುಲಕ & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	ಇಲ್ಲದಿದ್ದರೆ
	    perm[i] = '-';
    }
    ಹಿಂತಿರುಗಿ perm;
}

/*
 * "getfperm({fname})" function
 */
    ಶೂನ್ಯ
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = ಖಾಲಿ;
    char_u	permbuf[] = "---------";

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) >= 0)
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

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    fname = tv_get_string(&argvars[0]);
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) >= 0)
    {
	ಆದರೆ (S_ISDIR(st.st_ಬಹುಲಕ))
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    ಆದರೆ ((off_T)rettv->vval.v_ಸಂಖ್ಯೆ != (off_T)st.st_size)
		rettv->vval.v_ಸಂಖ್ಯೆ = -2;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
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

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    fname = tv_get_string(&argvars[0]);
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) >= 0)
	rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)st.st_mtime;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    ಅಕ್ಷರ    *t;

    ಆದರೆ (S_ISREG(st->st_ಬಹುಲಕ))
	t = "file";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISDIR(st->st_ಬಹುಲಕ))
	t = "dir";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISLNK(st->st_ಬಹುಲಕ))
	t = "link";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISBLK(st->st_ಬಹುಲಕ))
	t = "bdev";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISCHR(st->st_ಬಹುಲಕ))
	t = "cdev";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISFIFO(st->st_ಬಹುಲಕ))
	t = "fifo";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISSOCK(st->st_ಬಹುಲಕ))
	t = "socket";
    ಇಲ್ಲದಿದ್ದರೆ
	t = "other";
    ಹಿಂತಿರುಗಿ (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    ಶೂನ್ಯ
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ಆದರೆ (mch_lstat((ಅಕ್ಷರ *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    ಶೂನ್ಯ
f_glob(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	ಹಿಂತಿರುಗಿ;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ಆದರೆ (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_ಗಣ(rettv, ಖಾಲಿ);
	    ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    ಆದರೆ (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	ಆದರೆ (p_wic)
	    options += WILD_ICASE;
	ಆದರೆ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     ಖಾಲಿ, options, WILD_ALL);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv_list_alloc(rettv) == OK)
	{
	  ಪೂರ್ಣಾಂಕ i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						ಖಾಲಿ, options, WILD_ALL_KEEP);
	  ಗಾಗಿ (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = ಖಾಲಿ;
}

/*
 * "glob2regpat()" function
 */
    ಶೂನ್ಯ
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, ಒಳ_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == ಖಾಲಿ)
			 ? ಖಾಲಿ : file_pat_ಗಮ್ಯ_reg_pat(pat, ಖಾಲಿ, ಖಾಲಿ, FALSE);
}

/*
 * "globpath()" function
 */
    ಶೂನ್ಯ
f_globpath(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	ಹಿಂತಿರುಗಿ;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	ಆದರೆ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    ಆದರೆ (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_ಗಣ(rettv, ಖಾಲಿ);
	    ಆದರೆ (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    ಆದರೆ (file != ಖಾಲಿ && !error)
    {
	ga_init2(&ga, ಗಾತ್ರ(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	ಆದರೆ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_ಅಕ್ಷರಸರಣಿಗಳು(&ga, "\n");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv_list_alloc(rettv) == OK)
	    ಗಾಗಿ (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_ದತ್ತಾಂಶ))[i], -1);
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = ಖಾಲಿ;
}

/*
 * "isdirectory()" function
 */
    ಶೂನ್ಯ
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    ಶೂನ್ಯ
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = mch_is_ತುಂಬಿದ_Name(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mkdir_recurse(char_u *dir, ಪೂರ್ಣಾಂಕ prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    ಪೂರ್ಣಾಂಕ		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    ಆದರೆ (p <= get_past_ತಲೆ(dir))
	ಹಿಂತಿರುಗಿ OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    ಆದರೆ (updir == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (mch_isdir(updir))
	r = OK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	ಆದರೆ (r == OK && created != ಖಾಲಿ && *created == ಖಾಲಿ)
	    *created = ತುಂಬಿದ_Name_save(updir, FALSE);
    }
    vim_free(updir);
    ಹಿಂತಿರುಗಿ r;
}

/*
 * "mkdir()" function
 */
    ಶೂನ್ಯ
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		prot = 0755;
    ಪೂರ್ಣಾಂಕ		defer = FALSE;
    ಪೂರ್ಣಾಂಕ		defer_recurse = FALSE;
    char_u	*created = ಖಾಲಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = FAIL;
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    dir = tv_get_string_buf(&argvars[0], buf);
    ಆದರೆ (*dir == NUL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], ಖಾಲಿ);
	    ಆದರೆ (prot == -1)
		ಹಿಂತಿರುಗಿ;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != ಖಾಲಿ;
	defer_recurse = vim_strchr(arg2, 'R') != ಖಾಲಿ;
	ಆದರೆ ((defer || defer_recurse) && !can_ಕೂಡಿಸು_defer())
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (vim_strchr(arg2, 'p') != ಖಾಲಿ)
	{
	    ಆದರೆ (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_ಸಂಖ್ಯೆ = OK;
		ಹಿಂತಿರುಗಿ;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : ಖಾಲಿ);
	}
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    ಆದರೆ (rettv->vval.v_ಸಂಖ್ಯೆ == OK
				&& created == ಖಾಲಿ && (defer || defer_recurse))
	created = ತುಂಬಿದ_Name_save(dir, FALSE);
    ಆದರೆ (created != ಖಾಲಿ)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_ಬೀಗ = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_ಬೀಗ = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	ಆದರೆ (tv[0].vval.v_string == ಖಾಲಿ || tv[1].vval.v_string == ಖಾಲಿ
		|| ಕೂಡಿಸು_defer((char_u *)"delete", 2, tv) == FAIL)
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
    ಪೂರ್ಣಾಂಕ		trim_len = 1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	ಆದರೆ (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    ಆದರೆ (p == ಖಾಲಿ)
	rettv->vval.v_string = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	ಆದರೆ (p != ಖಾಲಿ)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
checkitem_common(ಶೂನ್ಯ *context, char_u *name, dict_T *dict)
{
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    ಪೂರ್ಣಾಂಕ		retval = 0;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    ಆದರೆ (name != ಖಾಲಿ)
    {
	ಗಣ_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಣ_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    ಆದರೆ (eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, FALSE, argv, 1, ಖಾಲಿ, &rettv) == FAIL)
	ಹೋಗು theend;

    // We want to use -1, but also true/false should be allowed.
    ಆದರೆ (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_ಸಂಖ್ಯೆ;
	rettv.vval.v_ಸಂಖ್ಯೆ = rettv.vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE;
    }
    retval = tv_get_ಸಂಖ್ಯೆ_chk(&rettv, &error);
    ಆದರೆ (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    ಆದರೆ (name != ಖಾಲಿ)
	ಗಣ_vim_var_string(VV_VAL, ಖಾಲಿ, 0);
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಣ_vim_var_dict(VV_VAL, ಖಾಲಿ);
    restore_vimvar(VV_VAL, &save_val);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
readdir_checkitem(ಶೂನ್ಯ *context, ಶೂನ್ಯ *ಅಂಶ)
{
    char_u	*name = (char_u *)ಅಂಶ;

    ಹಿಂತಿರುಗಿ checkitem_common(context, name, ಖಾಲಿ);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
readdirex_dict_arg(typval_T *argvars, ಪೂರ್ಣಾಂಕ *cmp)
{
    char_u     *compare;

    ಆದರೆ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (dict_has_ಕೀಲಿ(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_dictionary_ಕೀಲಿ_str_required), "sort");
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_ವಿಂಗಡಿಸು_NONE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_ವಿಂಗಡಿಸು_BYTE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_ವಿಂಗಡಿಸು_IC;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_ವಿಂಗಡಿಸು_COLLATE;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * "readdir()" function
 */
    ಶೂನ್ಯ
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ;
    ಪೂರ್ಣಾಂಕ		ret;
    char_u	*ಮಾರ್ಗ;
    char_u	*p;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ವಿಂಗಡಿಸು = READDIR_ವಿಂಗಡಿಸು_BYTE;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಮಾರ್ಗ = tv_get_string(&argvars[0]);
    ಅಭಿವ್ಯಕ್ತಿ = &argvars[1];

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &ವಿಂಗಡಿಸು) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ret = readdir_core(&ga, ಮಾರ್ಗ, FALSE, (ಶೂನ್ಯ *)ಅಭಿವ್ಯಕ್ತಿ,
	    (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_UNKNOWN) ? ಖಾಲಿ : readdir_checkitem, ವಿಂಗಡಿಸು);
    ಆದರೆ (ret == OK)
    {
	ಗಾಗಿ (i = 0; i < ga.ga_len; i++)
	{
	    p = ((char_u **)ga.ga_ದತ್ತಾಂಶ)[i];
	    list_append_string(rettv->vval.v_list, p, -1);
	}
    }
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
}

/*
 * Evaluate "expr" (= "context") for readdirex().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
readdirex_checkitem(ಶೂನ್ಯ *context, ಶೂನ್ಯ *ಅಂಶ)
{
    dict_T	*dict = (dict_T*)ಅಂಶ;

    ಹಿಂತಿರುಗಿ checkitem_common(context, ಖಾಲಿ, dict);
}

/*
 * "readdirex()" function
 */
    ಶೂನ್ಯ
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ;
    ಪೂರ್ಣಾಂಕ		ret;
    char_u	*ಮಾರ್ಗ;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ವಿಂಗಡಿಸು = READDIR_ವಿಂಗಡಿಸು_BYTE;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಮಾರ್ಗ = tv_get_string(&argvars[0]);
    ಅಭಿವ್ಯಕ್ತಿ = &argvars[1];

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &ವಿಂಗಡಿಸು) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ret = readdir_core(&ga, ಮಾರ್ಗ, TRUE, (ಶೂನ್ಯ *)ಅಭಿವ್ಯಕ್ತಿ,
	    (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_UNKNOWN) ? ಖಾಲಿ : readdirex_checkitem, ವಿಂಗಡಿಸು);
    ಆದರೆ (ret == OK)
    {
	ಗಾಗಿ (i = 0; i < ga.ga_len; i++)
	{
	    dict_T  *dict = ((dict_T**)ga.ga_ದತ್ತಾಂಶ)[i];
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
read_file_or_blob(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ always_blob)
{
    ಪೂರ್ಣಾಂಕ		ದ್ವಿಮಾನ = FALSE;
    ಪೂರ್ಣಾಂಕ		blob = always_blob;
    ಪೂರ್ಣಾಂಕ		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    ಪೂರ್ಣಾಂಕ		io_ಗಾತ್ರ = ಗಾತ್ರ(buf);
    ಪೂರ್ಣಾಂಕ		readlen;		// size of last fread()
    char_u	*prev	 = ಖಾಲಿ;	// previously read bytes, if any
    ದೀರ್ಘ	prevlen  = 0;		// length of data in prev
    ದೀರ್ಘ	prevsize = 0;		// size of prev buffer
    ದೀರ್ಘ	maxline  = MAXLNUM;
    ದೀರ್ಘ	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*ಆರಂಭ;			// start of current line
    off_T	ಸ್ಥಾನಾಂತರ = 0;
    off_T	ಗಾತ್ರ_2 = -1;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (always_blob)
	{
	    ಸ್ಥಾನಾಂತರ = (off_T)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
		ಗಾತ್ರ_2 = (off_T)tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		ದ್ವಿಮಾನ = TRUE;
	    ಆದರೆ (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
	}
    }

    ಆದರೆ ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    ಆದರೆ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (*fname == NUL || (fd = mch_fopen((ಅಕ್ಷರ *)fname, READBIN)) == ಖಾಲಿ)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (blob)
    {
	ಆದರೆ (read_blob(fd, rettv, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	ಹಿಂತಿರುಗಿ;
    }

    ಆಗ (cnt < maxline || maxline < 0)
    {
	readlen = (ಪೂರ್ಣಾಂಕ)fread(buf, 1, io_ಗಾತ್ರ, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	ಗಾಗಿ (p = buf, ಆರಂಭ = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || ದ್ವಿಮಾನ));
		++p)
	{
	    ಆದರೆ (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= ಖಾಲಿ;
		long_u	    len = p - ಆರಂಭ;

		// Finished a line.  Remove CRs before NL.
		ಆದರೆ (readlen > 0 && !ದ್ವಿಮಾನ)
		{
		    ಆಗ (len > 0 && ಆರಂಭ[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    ಆದರೆ (len == 0)
			ಆಗ (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		ಆದರೆ (prevlen == 0)
		    s = vim_strnsave(ಆರಂಭ, len);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    ಆದರೆ ((s = vim_realloc(prev, prevlen + len + 1)) != ಖಾಲಿ)
		    {
			mch_memmove(s + prevlen, ಆರಂಭ, len);
			s[prevlen + len] = NUL;
			prev = ಖಾಲಿ; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		ಆದರೆ (s == ಖಾಲಿ)
		{
		    do_outofmem_ಸಂದೇಶ((long_u) prevlen + len + 1);
		    failed = TRUE;
		    ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ ((li = listitem_alloc()) == ಖಾಲಿ)
		{
		    vim_free(s);
		    failed = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_ಬೀಗ = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		ಆರಂಭ = p + 1; // step over newline
		ಆದರೆ ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 0xbf && enc_utf8 && !ದ್ವಿಮಾನ)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		ಆದರೆ (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    ಆದರೆ (ಆರಂಭ == dest)
			ಆರಂಭ = p + 1;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// have to shuffle buf to close gap
			ಪೂರ್ಣಾಂಕ adjust_prevlen = 0;

			ಆದರೆ (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (ಪೂರ್ಣಾಂಕ)(buf - dest);
			    dest = buf;
			}
			ಆದರೆ (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	ಆದರೆ (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ಆರಂಭ < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    ಆದರೆ (p - ಆರಂಭ + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		ಆದರೆ (prevsize == 0)
		    prevsize = (ದೀರ್ಘ)(p - ಆರಂಭ);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ದೀರ್ಘ grow50pc = (prevsize * 3) / 2;
		    ದೀರ್ಘ growmin  = (ದೀರ್ಘ)((p - ಆರಂಭ) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		ಆದರೆ (newprev == ಖಾಲಿ)
		{
		    do_outofmem_ಸಂದೇಶ((long_u)prevsize);
		    failed = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
		prev = newprev;
	    }
	    // Add the line part to end of "prev".
	    mch_memmove(prev + prevlen, ಆರಂಭ, p - ಆರಂಭ);
	    prevlen += (ದೀರ್ಘ)(p - ಆರಂಭ);
	}
    } // while

    // For a negative line count use only the lines at the end of the file,
    // free the rest.
    ಆದರೆ (!failed && maxline < 0)
	ಆಗ (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_ಮೊದಲ);
	    --cnt;
	}

    ಆದರೆ (failed)
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
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    ಶೂನ್ಯ
f_readfile(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

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
    char_u	*buf = ಖಾಲಿ;
# endif

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = ಖಾಲಿ;

	v = mch_resolve_ಮಾರ್ಗ(p, TRUE);
	ಆದರೆ (v != ಖಾಲಿ)
	    rettv->vval.v_string = v;
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_string = vim_strsave(p);
    }
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	ಪೂರ್ಣಾಂಕ	len;
	char_u	*remain = ಖಾಲಿ;
	char_u	*q;
	ಪೂರ್ಣಾಂಕ	is_relative_ಗಮ್ಯ_current = FALSE;
	ಪೂರ್ಣಾಂಕ	has_trailing_pathsep = FALSE;
	ಪೂರ್ಣಾಂಕ	ಎಲ್ಲೆ = 100;

	p = vim_strsave(p);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹೋಗು fail;
	ಆದರೆ (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_ಗಮ್ಯ_current = TRUE;

	len = STRLEN(p);
	ಆದರೆ (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	ಆದರೆ (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    vim_free(p);
	    vim_free(remain);
	    ಹೋಗು fail;
	}

	ಗಾಗಿ (;;)
	{
	    ಗಾಗಿ (;;)
	    {
		len = readlink((ಅಕ್ಷರ *)p, (ಅಕ್ಷರ *)buf, MAXPATHL);
		ಆದರೆ (len <= 0)
		    ನಿಲ್ಲಿಸು;
		buf[len] = NUL;

		ಆದರೆ (ಎಲ್ಲೆ-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_ಚಕ್ರ));
		    rettv->vval.v_string = ಖಾಲಿ;
		    ಹೋಗು fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		ಆದರೆ (remain == ಖಾಲಿ && has_trailing_pathsep)
		    ಕೂಡಿಸು_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		ಆದರೆ (*q != NUL)
		{
		    ಆದರೆ (remain == ಖಾಲಿ)
			remain = vim_strsave(q - 1);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			cpy = concat_str(q - 1, remain);
			ಆದರೆ (cpy != ಖಾಲಿ)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		ಆದರೆ (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		ಆದರೆ (q > p && !mch_is_ತುಂಬಿದ_Name(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    ಆದರೆ (cpy != ಖಾಲಿ)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    ಆದರೆ (remain == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    ಆದರೆ (cpy != ಖಾಲಿ)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    ಆದರೆ (*q != NUL)
		STRMOVE(remain, q - 1);
	    ಇಲ್ಲದಿದ್ದರೆ
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	ಆದರೆ (!vim_ispathsep(*p))
	{
	    ಆದರೆ (is_relative_ಗಮ್ಯ_current
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
		ಆದರೆ (cpy != ಖಾಲಿ)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!is_relative_ಗಮ್ಯ_current)
	    {
		// Strip leading "./".
		q = p;
		ಆಗ (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		ಆದರೆ (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	ಆದರೆ (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    ಆದರೆ (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  ಇಲ್ಲದಿದ್ದರೆ
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
f_tempname(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    ಮಾಡು
    {
	ಆದರೆ (x == 'Z')
	    x = '0';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (x == '9')
	    x = 'A';
	ಇಲ್ಲದಿದ್ದರೆ
	    ++x;
    } ಆಗ (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    ಶೂನ್ಯ
f_writefile(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		ದ್ವಿಮಾನ = FALSE;
    ಪೂರ್ಣಾಂಕ		append = FALSE;
    ಪೂರ್ಣಾಂಕ		defer = FALSE;
# ifdef HAVE_FSYNC
    ಪೂರ್ಣಾಂಕ		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    ಪೂರ್ಣಾಂಕ		ret = 0;
    listitem_T	*li;
    list_T	*list = ಖಾಲಿ;
    blob_T	*blob = ಖಾಲಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	ಆದರೆ (list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
	    ಆದರೆ (tv_get_string_chk(&li->li_tv) == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	ಆದರೆ (blob == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ವಾದ_str),
		_("writefile() first argument must be a List or a Blob"));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	ಆದರೆ (arg2 == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (vim_strchr(arg2, 'b') != ಖಾಲಿ)
	    ದ್ವಿಮಾನ = TRUE;
	ಆದರೆ (vim_strchr(arg2, 'a') != ಖಾಲಿ)
	    append = TRUE;
	ಆದರೆ (vim_strchr(arg2, 'D') != ಖಾಲಿ)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	ಆದರೆ (vim_strchr(arg2, 's') != ಖಾಲಿ)
	    do_fsync = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr(arg2, 'S') != ಖಾಲಿ)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (defer && !can_ಕೂಡಿಸು_defer())
	ಹಿಂತಿರುಗಿ;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    ಆದರೆ (*fname == NUL || (fd = mch_fopen((ಅಕ್ಷರ *)fname,
				      append ? APPENDBIN : WRITEBIN)) == ಖಾಲಿ)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_ಬೀಗ = 0;
	    tv.vval.v_string = ತುಂಬಿದ_Name_save(fname, FALSE);
	    ಆದರೆ (tv.vval.v_string == ಖಾಲಿ
		    || ಕೂಡಿಸು_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(ಶೂನ್ಯ)mch_remove(fname);
	    }
	}

	ಆದರೆ (ret == 0)
	{
	    ಆದರೆ (blob)
	    {
		ಆದರೆ (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (write_list(fd, list, ದ್ವಿಮಾನ) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    ಆದರೆ (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = ret;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

#ಆದರೆ defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    ಪೂರ್ಣಾಂಕ		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    ಸ್ಥಿರ char_u	*ಕೊನೆಯ_dir = ಖಾಲಿ;    // last used directory
    char_u		*tofree = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    ಆದರೆ (title == ಖಾಲಿ || *title == NUL)
    {
	ಆದರೆ (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	ಇಲ್ಲದಿದ್ದರೆ
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    ಆದರೆ ((initdir == ಖಾಲಿ || *initdir == NUL) && dflt != ಖಾಲಿ && *dflt != NUL)
    {
	ಆದರೆ (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    ಆದರೆ (tofree != ಖಾಲಿ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    ಆದರೆ (initdir == ಖಾಲಿ || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	ಆದರೆ (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != ಖಾಲಿ && buf->b_ffname != ಖಾಲಿ)
	{
	    ಆದರೆ (dflt == ಖಾಲಿ || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    ಆದರೆ (tofree != ಖಾಲಿ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_bsdir == 'l')
	    initdir = ಕೊನೆಯ_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)		// when this changes, also adjust f_has()!
    {
	ಆದರೆ (filter == ಖಾಲಿ
#  ifdef FEAT_EVAL
		&& (filter = get_var_ಮೌಲ್ಯ((char_u *)"b:browsefilter")) == ಖಾಲಿ
		&& (filter = get_var_ಮೌಲ್ಯ((char_u *)"g:browsefilter")) == ಖಾಲಿ
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	ಆದರೆ (flags & BROWSE_DIR)
	{
#  ಆದರೆ defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  ಇಲ್ಲದಿದ್ದರೆ
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  ಆದರೆ !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    ಆದರೆ (fname != ಖಾಲಿ && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *ಬಾಲ = gettail_sep(fname);

		ಆದರೆ (ಬಾಲ == fname)
		    *ಬಾಲ++ = '.';	// use current dir
		*ಬಾಲ = NUL;
	    }
#  endif
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_ಒಳ_console_ಬಹುಲಕ));
	fname = ಖಾಲಿ;
    }

    // keep the directory for next time
    ಆದರೆ (fname != ಖಾಲಿ)
    {
	vim_free(ಕೊನೆಯ_dir);
	ಕೊನೆಯ_dir = vim_strsave(fname);
	ಆದರೆ (ಕೊನೆಯ_dir != ಖಾಲಿ && !(flags & BROWSE_DIR))
	{
	    *gettail(ಕೊನೆಯ_dir) = NUL;
	    ಆದರೆ (*ಕೊನೆಯ_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(ಕೊನೆಯ_dir);
		ಕೊನೆಯ_dir = alloc(MAXPATHL);
		ಆದರೆ (ಕೊನೆಯ_dir != ಖಾಲಿ)
		    mch_dirname(ಕೊನೆಯ_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    ಹಿಂತಿರುಗಿ fname;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    ಶೂನ್ಯ
f_browse(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    ಪೂರ್ಣಾಂಕ		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ಹಿಂತಿರುಗಿ;

    save = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    ಆದರೆ (error || title == ಖಾಲಿ || initdir == ಖಾಲಿ || defname == ಖಾಲಿ)
	rettv->vval.v_string = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, ಖಾಲಿ, initdir, ಖಾಲಿ, curbuf);
# ಇಲ್ಲದಿದ್ದರೆ
    rettv->vval.v_string = ಖಾಲಿ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    ಶೂನ್ಯ
f_browsedir(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    ಆದರೆ (title == ಖಾಲಿ || initdir == ಖಾಲಿ)
	rettv->vval.v_string = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, ಖಾಲಿ, ಖಾಲಿ, initdir, ಖಾಲಿ, curbuf);
# ಇಲ್ಲದಿದ್ದರೆ
    rettv->vval.v_string = ಖಾಲಿ;
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

    ಆದರೆ (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    from = tv_get_string(&argvars[0]);

    ಆದರೆ (mch_lstat((ಅಕ್ಷರ *)from, &st) >= 0
	&& (S_ISREG(st.st_ಬಹುಲಕ) || S_ISLNK(st.st_ಬಹುಲಕ)))
	rettv->vval.v_ಸಂಖ್ಯೆ = vim_copyfile(
	    tv_get_string(&argvars[0]),
	    tv_get_string(&argvars[1])) == OK ? TRUE : FALSE;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

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
    ಪೂರ್ಣಾಂಕ		dstlen,	// maximum length of the result
    ಪೂರ್ಣಾಂಕ		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_ಆರಂಭ;

    ಆದರೆ (src == ಖಾಲಿ)
    {
	*dst = NUL;
	ಹಿಂತಿರುಗಿ 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    ಆದರೆ (buf != ಖಾಲಿ && buf->b_help)
	ಹಿಂತಿರುಗಿ vim_snprintf_safelen((ಅಕ್ಷರ *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    ಆದರೆ (homedir != ಖಾಲಿ)
	dirlen = STRLEN(homedir);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#ಇಲ್ಲದಿದ್ದರೆ
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (homedir_env == ಖಾಲಿ)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#ಆದರೆ_ಅಂತ್ಯ
    // Empty is the same as not set.
    ಆದರೆ (homedir_env != ಖಾಲಿ && *homedir_env == NUL)
	homedir_env = ಖಾಲಿ;

    ಆದರೆ (homedir_env != ಖಾಲಿ && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = ಖಾಲಿ;

	flen = STRLEN(homedir_env);
	(ಶೂನ್ಯ)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	ಆದರೆ (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    ಆದರೆ (homedir_env != ಖಾಲಿ)
	envlen = STRLEN(homedir_env);

    ಆದರೆ (!one)
	src = skipwhite(src);
    dst_ಆರಂಭ = dst;		// remember the start
    ಆಗ (*src && dstlen > 0)
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
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		ಆದರೆ (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (p == homedir_env)
		ನಿಲ್ಲಿಸು;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	ಆಗ (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	ಆಗ ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    ಆದರೆ (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    ಹಿಂತಿರುಗಿ (size_t)(dst - dst_ಆರಂಭ);
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
    ಚಿಹ್ನೆರಹಿತ	len;

    len = 3;			// space for "~/" and trailing NUL
    ಆದರೆ (src != ಖಾಲಿ)		// just in case
	len += (ಚಿಹ್ನೆರಹಿತ)STRLEN(src);
    dst = alloc(len);
    ಆದರೆ (dst != ಖಾಲಿ)
	home_replace(buf, src, dst, len, TRUE);
    ಹಿಂತಿರುಗಿ dst;
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
    ಪೂರ್ಣಾಂಕ
fullpathcmp(
    char_u *s1,
    char_u *s2,
    ಪೂರ್ಣಾಂಕ	    checkname,		// when both don't exist, check file names
    ಪೂರ್ಣಾಂಕ	    expandenv)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    ಪೂರ್ಣಾಂಕ		    r1, r2;

    ಆದರೆ (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((ಅಕ್ಷರ *)exp1, &st1);
    r2 = mch_stat((ಅಕ್ಷರ *)s2, &st2);
    ಆದರೆ (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	ಆದರೆ (checkname)
	{
	    ಆದರೆ (fnamecmp(exp1, s2) == 0)
		ಹಿಂತಿರುಗಿ FPC_SAMEX;
	    r1 = vim_ತುಂಬಿದ_Name(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_ತುಂಬಿದ_Name(s2, full2, MAXPATHL, FALSE);
	    ಆದರೆ (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		ಹಿಂತಿರುಗಿ FPC_SAMEX;
	}
	ಹಿಂತಿರುಗಿ FPC_NOTX;
    }
    ಆದರೆ (r1 != 0 || r2 != 0)
	ಹಿಂತಿರುಗಿ FPC_DIFFX;
    ಆದರೆ (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	ಹಿಂತಿರುಗಿ FPC_SAME;
    ಹಿಂತಿರುಗಿ FPC_DIFF;
#ಇಲ್ಲದಿದ್ದರೆ
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    ಪೂರ್ಣಾಂಕ	    retval = FPC_DIFF;
    ಪೂರ್ಣಾಂಕ	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    ಆದರೆ ((exp1 = alloc(MAXPATHL * 3)) != ಖಾಲಿ)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	ಆದರೆ (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_ತುಂಬಿದ_Name(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_ತುಂಬಿದ_Name(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	ಆದರೆ (r1 != OK && r2 != OK)
	{
	    ಆದರೆ (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    ಇಲ್ಲದಿದ್ದರೆ
		retval = FPC_NOTX;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    ಹಿಂತಿರುಗಿ retval;
#ಆದರೆ_ಅಂತ್ಯ
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

    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (char_u *)"";
    ಗಾಗಿ (p1 = p2 = get_past_ತಲೆ(fname); *p2; )	// find last part of path
    {
	ಆದರೆ (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    ಹಿಂತಿರುಗಿ p1;
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

    p = get_past_ತಲೆ(fname);	// don't remove the '/' from "c:/file"
    t = gettail(fname);
    ಆಗ (t > p && after_pathsep(fname, t))
	--t;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    // path separator is part of the path
    ++t;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    ಆಗ (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    ಆದರೆ (*fname)
	++fname;
    ಹಿಂತಿರುಗಿ fname;
}

/*
 * Get a pointer to one character past the head of a path name.
 * Unix: after "/"; DOS: after "c:\"; Amiga: after "disk:/"; Mac: no head.
 * If there is no head, path is returned.
 */
    char_u *
get_past_ತಲೆ(char_u *ಮಾರ್ಗ)
{
    char_u  *retval;

#ಆದರೆ defined(MSWIN)
    // may skip "c:"
    ಆದರೆ (ಸುರಕ್ಷಿತ_isalpha(ಮಾರ್ಗ[0]) && ಮಾರ್ಗ[1] == ':')
	retval = ಮಾರ್ಗ + 2;
    ಇಲ್ಲದಿದ್ದರೆ
	retval = ಮಾರ್ಗ;
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(ಮಾರ್ಗ, ':');
    ಆದರೆ (retval == ಖಾಲಿ)
	retval = ಮಾರ್ಗ;
# ಇಲ್ಲದಿದ್ದರೆ	// Unix
    retval = ಮಾರ್ಗ;
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ (vim_ispathsep(*retval))
	++retval;

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    ಪೂರ್ಣಾಂಕ
vim_ispathsep(ಪೂರ್ಣಾಂಕ c)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಹಿಂತಿರುಗಿ (c == '/');	    // UNIX has ':' inside file names
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef BACKSLASH_ಒಳ_FILENAME
    ಹಿಂತಿರುಗಿ (c == ':' || c == '/' || c == '\\');
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    ಹಿಂತಿರುಗಿ (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (c == ':' || c == '/');
#  endif // VMS
# endif
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    ಪೂರ್ಣಾಂಕ
vim_ispathsep_nocolon(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ vim_ispathsep(c)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	&& c != ':'
#ಆದರೆ_ಅಂತ್ಯ
	;
}

/*
 * Return TRUE if the directory of "fname" exists, FALSE otherwise.
 * Also returns TRUE if there is no directory name.
 * "fname" must be writable!.
 */
    ಪೂರ್ಣಾಂಕ
dir_ದ_file_exists(char_u *fname)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		retval;

    p = gettail_sep(fname);
    ಆದರೆ (p == fname)
	ಹಿಂತಿರುಗಿ TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    ಪೂರ್ಣಾಂಕ
vim_fnamecmp(char_u *x, char_u *y)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    ಹಿಂತಿರುಗಿ vim_fnamencmp(x, y, MAXPATHL);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (p_fic)
	ಹಿಂತಿರುಗಿ MB_STRICMP(x, y);
    ಹಿಂತಿರುಗಿ STRCMP(x, y);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಪೂರ್ಣಾಂಕ
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    ಪೂರ್ಣಾಂಕ		cx = NUL;
    ಪೂರ್ಣಾಂಕ		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಅದಲುಬದಲು = 0, drive = NUL; ಅದಲುಬದಲು < 2; ++ಅದಲುಬದಲು)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	ಆದರೆ ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    ಆದರೆ (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    ಆಗ (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	ಆದರೆ (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    ನಿಲ್ಲಿಸು;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ (cx - cy);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (p_fic)
	ಹಿಂತಿರುಗಿ MB_STRNICMP(x, y, len);
    ಹಿಂತಿರುಗಿ STRNCMP(x, y, len);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, ಪೂರ್ಣಾಂಕ sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? ಗಾತ್ರ(PATHSEP) : 0) + fname2len + 1);
    ಆದರೆ (ret->string == ಖಾಲಿ)
	ret->ಉದ್ದಳತೆ = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	STRCPY(ret->string, fname1);
	ret->ಉದ್ದಳತೆ = fname1len;
	ಆದರೆ (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->ಉದ್ದಳತೆ))
	{
	    STRCPY(ret->string + ret->ಉದ್ದಳತೆ, PATHSEPSTR);
	    ret->ಉದ್ದಳತೆ += ಗಾತ್ರ(PATHSEP);
	}
	STRCPY(ret->string + ret->ಉದ್ದಳತೆ, fname2);
	ret->ಉದ್ದಳತೆ += fname2len;
    }

    ಹಿಂತಿರುಗಿ ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_pathsep(char_u *p)
{
    size_t  plen;

    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ;

    plen = STRLEN(p);
    ಆದರೆ (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
ತುಂಬಿದ_Name_save(
    char_u	*fname,
    ಪೂರ್ಣಾಂಕ		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = ಖಾಲಿ;

    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    buf = alloc(MAXPATHL);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (vim_ತುಂಬಿದ_Name(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    ಇಲ್ಲದಿದ್ದರೆ
	new_fname = vim_strsave(fname);
    vim_free(buf);
    ಹಿಂತಿರುಗಿ new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    ಪೂರ್ಣಾಂಕ
vim_fexists(char_u *fname)
{
    stat_T st;

    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st))
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    ಪೂರ್ಣಾಂಕ		  *ಅಂಕೆ_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ಪೂರ್ಣಾಂಕ		   flags)	// EW_DIR, etc.
{
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    char_u	*eval_pat = ಖಾಲಿ;
    char_u	*exp_pat = *pat;
    ಅಕ್ಷರ	*ignored_ಸಂದೇಶ;
    size_t	usedlen;
    ಪೂರ್ಣಾಂಕ		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    ಪೂರ್ಣಾಂಕ		ನಕ್ಷತ್ರ_follows = FALSE;

    ಆದರೆ (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       ಖಾಲಿ, &ignored_ಸಂದೇಶ, ಖಾಲಿ, TRUE);
	--emsg_off;
	ಆದರೆ (eval_pat != ಖಾಲಿ)
	{
	    ನಕ್ಷತ್ರ_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    ಆದರೆ (exp_pat != ಖಾಲಿ)
	ret = expand_wildcards(1, &exp_pat, ಅಂಕೆ_file, file, flags);

    ಆದರೆ (eval_pat != ಖಾಲಿ)
    {
	ಆದರೆ (*ಅಂಕೆ_file == 0 && is_cur_alt_file && ನಕ್ಷತ್ರ_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    ಆದರೆ (*file != ಖಾಲಿ)
	    {
		**file = eval_pat;
		eval_pat = ಖಾಲಿ;
		*ಅಂಕೆ_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    ಪೂರ್ಣಾಂಕ
expand_wildcards(
    ಪೂರ್ಣಾಂಕ		   ಅಂಕೆ_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    ಪೂರ್ಣಾಂಕ		  *ಅಂಕೆ_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    ಪೂರ್ಣಾಂಕ		   flags)	// EW_DIR, etc.
{
    ಪೂರ್ಣಾಂಕ		retval;
    ಪೂರ್ಣಾಂಕ		i, j;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		non_suf_ಹೊಂದಿಕೆ;	// number without matching suffix

    retval = gen_expand_wildcards(ಅಂಕೆ_pat, pat, ಅಂಕೆ_files, files, flags);

    // When keeping all matches, return here
    ಆದರೆ ((flags & EW_KEEPALL) || retval == FAIL)
	ಹಿಂತಿರುಗಿ retval;

    /*
     * Remove names that match 'wildignore'.
     */
    ಆದರೆ (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	ಗಾಗಿ (i = 0; i < *ಅಂಕೆ_files; ++i)
	{
	    ffname = ತುಂಬಿದ_Name_save((*files)[i], FALSE);
	    ಆದರೆ (ffname == ಖಾಲಿ)		// out of memory
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	    vms_remove_ಆವೃತ್ತಿ(ffname);
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (ಹೊಂದಿಕೆ_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		ಗಾಗಿ (j = i; j + 1 < *ಅಂಕೆ_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*ಅಂಕೆ_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	ಆದರೆ (*ಅಂಕೆ_files == 0)
	{
	    VIM_CLEAR(*files);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    ಆದರೆ (*ಅಂಕೆ_files > 1 && !got_int)
    {
	non_suf_ಹೊಂದಿಕೆ = 0;
	ಗಾಗಿ (i = 0; i < *ಅಂಕೆ_files; ++i)
	{
	    ಆದರೆ (!ಹೊಂದಿಕೆ_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		ಗಾಗಿ (j = i; j > non_suf_ಹೊಂದಿಕೆ; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_ಹೊಂದಿಕೆ++] = p;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_suffix(char_u *fname)
{
    ಪೂರ್ಣಾಂಕ		fnamelen, setsuflen;
    char_u	*setsuf;
#ವ್ಯಾಖ್ಯಾನಿಸು MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (ಪೂರ್ಣಾಂಕ)STRLEN(fname);
    setsuflen = 0;
    ಗಾಗಿ (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	ಆದರೆ (setsuflen == 0)
	{
	    char_u *ಬಾಲ = gettail(fname);

	    // empty entry: match name without a '.'
	    ಆದರೆ (vim_strchr(ಬಾಲ, '.') == ಖಾಲಿ)
	    {
		setsuflen = 1;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		ನಿಲ್ಲಿಸು;
	    setsuflen = 0;
	}
    }
    ಹಿಂತಿರುಗಿ (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim_backtick(char_u *p)
{
    ಹಿಂತಿರುಗಿ (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*ಬಫರ್;
    ಪೂರ್ಣಾಂಕ		cnt = 0;
    ಪೂರ್ಣಾಂಕ		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    ಆದರೆ (cmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (*cmd == '=')	    // `={expr}`: Expand expression
	ಬಫರ್ = eval_ಗಮ್ಯ_string(cmd + 1, TRUE, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಬಫರ್ = get_cmd_output(cmd, ಖಾಲಿ,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, ಖಾಲಿ);
    vim_free(cmd);
    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    cmd = ಬಫರ್;
    ಆಗ (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	ಆಗ (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	ಆದರೆ (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	ಆಗ (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(ಬಫರ್);
    ಹಿಂತಿರುಗಿ cnt;
}

#ಆದರೆ defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pstrcmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    ಹಿಂತಿರುಗಿ (pathcmp(*(ಅಕ್ಷರ **)a, *(ಅಕ್ಷರ **)b, -1));
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
    ಪೂರ್ಣಾಂಕ
dos_expandpath(
    garray_T	*gap,
    char_u	*ಮಾರ್ಗ,
    size_t	wildoff,
    ಪೂರ್ಣಾಂಕ		flags,		// EW_* flags
    ಪೂರ್ಣಾಂಕ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*ಮಾರ್ಗ_ಕೊನೆ;
    char_u	*p, *s, *e;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		starts_with_dot;
    ಪೂರ್ಣಾಂಕ		matches;
    size_t	buflen;
    size_t	len;
    ಪೂರ್ಣಾಂಕ		starstar = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	stardepth = 0;	    // depth for "**" expansion
    HANDLE		h_ಕಂಡುಹಿಡಿ = INVALID_HANDLE_ಮೌಲ್ಯ;
    WIN32_ಕಂಡುಹಿಡಿ_DATAW    wfb;
    WCHAR		*wn = ಖಾಲಿ;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    ಪೂರ್ಣಾಂಕ			ok;
    char_u		*p_alt;
    ಪೂರ್ಣಾಂಕ			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    ಆದರೆ (stardepth > 0)
    {
	ui_breakcheck();
	ಆದರೆ (got_int)
	    ಹಿಂತಿರುಗಿ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(ಮಾರ್ಗ) + MAXPATHL;
    buf = alloc(buflen);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ಖಾಲಿ;
    ಮಾರ್ಗ_ಕೊನೆ = ಮಾರ್ಗ;
    ಆಗ (*ಮಾರ್ಗ_ಕೊನೆ != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ಆದರೆ (ಮಾರ್ಗ_ಕೊನೆ >= ಮಾರ್ಗ + wildoff && rem_backslash(ಮಾರ್ಗ_ಕೊನೆ))
	    *p++ = *ಮಾರ್ಗ_ಕೊನೆ++;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಮಾರ್ಗ_ಕೊನೆ == '\\' || *ಮಾರ್ಗ_ಕೊನೆ == ':' || *ಮಾರ್ಗ_ಕೊನೆ == '/')
	{
	    ಆದರೆ (e != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    s = p + 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮಾರ್ಗ_ಕೊನೆ >= ಮಾರ್ಗ + wildoff
			 && vim_strchr((char_u *)"*?[~", *ಮಾರ್ಗ_ಕೊನೆ) != ಖಾಲಿ)
	    e = p;
	ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ charlen = (*mb_ptr2len)(ಮಾರ್ಗ_ಕೊನೆ);

	    STRNCPY(p, ಮಾರ್ಗ_ಕೊನೆ, (size_t)charlen);
	    p += charlen;
	    ಮಾರ್ಗ_ಕೊನೆ += charlen;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *p++ = *ಮಾರ್ಗ_ಕೊನೆ++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    ಗಾಗಿ (p = buf + wildoff; p < s; ++p)
	ಆದರೆ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    ಗಾಗಿ (p = s; p < e; ++p)
	ಆದರೆ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_ಗಮ್ಯ_reg_pat(s, e, ಖಾಲಿ, FALSE);
    ಆದರೆ (pat == ಖಾಲಿ)
    {
	vim_free(buf);
	ಹಿಂತಿರುಗಿ 0;
    }

    // compile the regexp into a program
    ಆದರೆ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ಆದರೆ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ಆದರೆ (regmatch.regprog == ಖಾಲಿ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ಹಿಂತಿರುಗಿ 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != ಖಾಲಿ;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ಆದರೆ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *ಮಾರ್ಗ_ಕೊನೆ == '/')
    {
	vim_snprintf((ಅಕ್ಷರ *)s, buflen - len, "%s", ಮಾರ್ಗ_ಕೊನೆ + 1);
	++stardepth;
	(ಶೂನ್ಯ)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((ಅಕ್ಷರ *)s, buflen - len, "*.*");
    wn = enc_ಗಮ್ಯ_utf16(buf, ಖಾಲಿ);
    ಆದರೆ (wn != ಖಾಲಿ)
	h_ಕಂಡುಹಿಡಿ = ಕಂಡುಹಿಡಿ_ಮೊದಲ_File_W(wn, &wfb);
    ok = (h_ಕಂಡುಹಿಡಿ != INVALID_HANDLE_ಮೌಲ್ಯ);

    ಆಗ (ok)
    {
	p = utf16_ಗಮ್ಯ_enc(wfb.cFileName, ಖಾಲಿ);   // p is allocated here

	ಆದರೆ (p == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	ಆದರೆ (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    p_alt = utf16_ಗಮ್ಯ_enc(wfb.cAlternateFileName, ಖಾಲಿ);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	ಆದರೆ ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == ಖಾಲಿ
		  || (regmatch.regprog != ಖಾಲಿ
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != ಖಾಲಿ
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(ಮಾರ್ಗ + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((ಅಕ್ಷರ *)s, buflen - len, "%s", p);
	    ಆದರೆ (len + 1 < buflen)
	    {
		ಆದರೆ (starstar && stardepth < 100
			  && (wfb.dw_File_ಗುಣಲಕ್ಷಣಗಳು & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((ಅಕ್ಷರ *)buf + len, buflen - len,
							    "/**%s", ಮಾರ್ಗ_ಕೊನೆ);
		    ++stardepth;
		    (ಶೂನ್ಯ)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((ಅಕ್ಷರ *)buf + len, buflen - len, "%s", ಮಾರ್ಗ_ಕೊನೆ);
		ಆದರೆ (mch_has_exp_wildcard(ಮಾರ್ಗ_ಕೊನೆ))
		{
		    ಆದರೆ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ಶೂನ್ಯ)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ಆದರೆ (*ಮಾರ್ಗ_ಕೊನೆ != 0)
			backslash_halve(buf + len + 1);
		    // addfile() checks existence; do not stat the same path twice.
		    // Even with EW_NOTFOUND, a literal suffix must exist.
		    addfile(gap, buf, flags & ~EW_NOTFOUND);
		}
	    }
	}

	vim_free(p_alt);
	vim_free(p);
	ok = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_File_W(h_ಕಂಡುಹಿಡಿ, &wfb);
    }

    ಕಂಡುಹಿಡಿ_Close(h_ಕಂಡುಹಿಡಿ);
    vim_free(wn);
    vim_free(buf);
    vim_regfree(regmatch.regprog);
    vim_free(matchname);

    matches = gap->ga_len - ಆರಂಭ_len;
    ಆದರೆ (matches > 0)
	qsort(((char_u **)gap->ga_ದತ್ತಾಂಶ) + ಆರಂಭ_len, (size_t)matches,
						   ಗಾತ್ರ(char_u *), pstrcmp);
    ಹಿಂತಿರುಗಿ matches;
}
#ಆದರೆ_ಅಂತ್ಯ // MSWIN

#ಆದರೆ (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pstrcmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    ಹಿಂತಿರುಗಿ (pathcmp(*(ಅಕ್ಷರ **)a, *(ಅಕ್ಷರ **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    ಪೂರ್ಣಾಂಕ
unix_expandpath(
    garray_T	*gap,
    char_u	*ಮಾರ್ಗ,
    size_t	wildoff,
    ಪೂರ್ಣಾಂಕ		flags,		// EW_* flags
    ಪೂರ್ಣಾಂಕ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*ಮಾರ್ಗ_ಕೊನೆ;
    char_u	*p, *s, *e;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		starts_with_dot;
    ಪೂರ್ಣಾಂಕ		matches;
    size_t	buflen;
    size_t	len;
    ಪೂರ್ಣಾಂಕ		starstar = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    ರಚನೆ dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    ಆದರೆ (stardepth > 0)
    {
	ui_breakcheck();
	ಆದರೆ (got_int)
	    ಹಿಂತಿರುಗಿ 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(ಮಾರ್ಗ) + MAXPATHL;
    buf = alloc(buflen);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ಖಾಲಿ;
    ಮಾರ್ಗ_ಕೊನೆ = ಮಾರ್ಗ;
    ಆಗ (*ಮಾರ್ಗ_ಕೊನೆ != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ಆದರೆ (ಮಾರ್ಗ_ಕೊನೆ >= ಮಾರ್ಗ + wildoff && rem_backslash(ಮಾರ್ಗ_ಕೊನೆ))
	    *p++ = *ಮಾರ್ಗ_ಕೊನೆ++;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಮಾರ್ಗ_ಕೊನೆ == '/')
	{
	    ಆದರೆ (e != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    s = p + 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮಾರ್ಗ_ಕೊನೆ >= ಮಾರ್ಗ + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *ಮಾರ್ಗ_ಕೊನೆ) != ಖಾಲಿ
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(ಮಾರ್ಗ_ಕೊನೆ)))))
	    e = p;
	ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ charlen = (*mb_ptr2len)(ಮಾರ್ಗ_ಕೊನೆ);

	    STRNCPY(p, ಮಾರ್ಗ_ಕೊನೆ, (size_t)charlen);
	    p += charlen;
	    ಮಾರ್ಗ_ಕೊನೆ += charlen;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *p++ = *ಮಾರ್ಗ_ಕೊನೆ++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    ಗಾಗಿ (p = buf + wildoff; p < s; ++p)
	ಆದರೆ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    ಗಾಗಿ (p = s; p < e; ++p)
	ಆದರೆ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_ಗಮ್ಯ_reg_pat(s, e, ಖಾಲಿ, FALSE);
    ಆದರೆ (pat == ಖಾಲಿ)
    {
	vim_free(buf);
	ಹಿಂತಿರುಗಿ 0;
    }

    // compile the regexp into a program
    ಆದರೆ (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    ಇಲ್ಲದಿದ್ದರೆ
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ಆದರೆ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ಆದರೆ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ಆದರೆ (regmatch.regprog == ಖಾಲಿ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ಹಿಂತಿರುಗಿ 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ಆದರೆ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *ಮಾರ್ಗ_ಕೊನೆ == '/')
    {
	vim_snprintf((ಅಕ್ಷರ *)s, buflen - len, "%s", ಮಾರ್ಗ_ಕೊನೆ + 1);
	++stardepth;
	(ಶೂನ್ಯ)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (ಅಕ್ಷರ *)buf);

    // Find all matching entries
    ಆದರೆ (dirp != ಖಾಲಿ)
    {
	ಆಗ (!got_int)
	{
	    dp = readdir(dirp);
	    ಆದರೆ (dp == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    len = (size_t)(s - buf);
	    ಆದರೆ ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != ಖಾಲಿ && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(ಮಾರ್ಗ + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((ಅಕ್ಷರ *)s, buflen - len, "%s", dp->d_name);
		ಆದರೆ (len + 1 >= buflen)
		    ಮುಂದುವರಿ;

		ಆದರೆ (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((ಅಕ್ಷರ *)buf + len, buflen - len,
							    "/**%s", ಮಾರ್ಗ_ಕೊನೆ);
		    ++stardepth;
		    (ಶೂನ್ಯ)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((ಅಕ್ಷರ *)buf + len, buflen - len, "%s", ಮಾರ್ಗ_ಕೊನೆ);
		ಆದರೆ (mch_has_exp_wildcard(ಮಾರ್ಗ_ಕೊನೆ)) // handle more wildcards
		{
		    ಆದರೆ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(ಶೂನ್ಯ)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ಆದರೆ (*ಮಾರ್ಗ_ಕೊನೆ != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    ಆದರೆ ((flags & EW_ALLLINKS) ? mch_lstat((ಅಕ್ಷರ *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_ಮಾರ್ಗ(buf, precomp_len, &precomp_len);

			ಆದರೆ (precomp_buf)
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
    matches = gap->ga_len - ಆರಂಭ_len;
    ಆದರೆ (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_ದತ್ತಾಂಶ) + ಆರಂಭ_len, matches,
						   ಗಾತ್ರ(char_u *), pstrcmp);
    ಹಿಂತಿರುಗಿ matches;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
has_env_var(char_u *p)
{
    ಗಾಗಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)
#ಆದರೆ defined(MSWIN)
				    "$%"
#ಇಲ್ಲದಿದ್ದರೆ
				    "$"
#ಆದರೆ_ಅಂತ್ಯ
					, *p) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
has_special_wildchar(char_u *p)
{
    ಗಾಗಿ ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	ಆದರೆ (*p == '\r' || *p == '\n')
	    ನಿಲ್ಲಿಸು;
	// Allow for escaping.
	ಆದರೆ (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != ಖಾಲಿ)
	{
	    // A { must be followed by a matching }.
	    ಆದರೆ (*p == '{' && vim_strchr(p, '}') == ಖಾಲಿ)
		ಮುಂದುವರಿ;
	    // A quote and backtick must be followed by another one.
	    ಆದರೆ ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == ಖಾಲಿ)
		ಮುಂದುವರಿ;
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
gen_expand_wildcards(
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ಪೂರ್ಣಾಂಕ		flags)		// EW_* flags
{
    ಪೂರ್ಣಾಂಕ			i;
    garray_T		ga;
    char_u		*p;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		recursive = FALSE;
    ಪೂರ್ಣಾಂಕ			ಕೂಡಿಸು_pat;
    ಪೂರ್ಣಾಂಕ			retval = OK;
    ಪೂರ್ಣಾಂಕ			did_expand_ಒಳ_ಮಾರ್ಗ = FALSE;
    char_u		*ಮಾರ್ಗ_option = *curbuf->b_p_ಮಾರ್ಗ == NUL ?
					p_ಮಾರ್ಗ : curbuf->b_p_ಮಾರ್ಗ;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    ಆದರೆ (recursive)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SPECIAL_WILDCHAR
	ಹಿಂತಿರುಗಿ mch_expand_wildcards(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file, flags);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_pat; i++)
    {
	ಆದರೆ (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    ಹಿಂತಿರುಗಿ mch_expand_wildcards(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file, flags);
    }
#ಆದರೆ_ಅಂತ್ಯ

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, ಗಾತ್ರ(char_u *), 30);

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_pat && !got_int; ++i)
    {
	ಕೂಡಿಸು_pat = -1;
	p = pat[i];

	ಆದರೆ (vim_backtick(p))
	{
	    ಕೂಡಿಸು_pat = expand_backtick(&ga, p, flags);
	    ಆದರೆ (ಕೂಡಿಸು_pat == -1)
		retval = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    ಆದರೆ ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)ಮಾರ್ಗ_ESC_WILDCARDS);
		ಆದರೆ (p == ಖಾಲಿ)
		    p = pat[i];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
		    i = mch_expand_wildcards(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    ಹಿಂತಿರುಗಿ i;
		}
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    /*
	     * If there are wildcards or case-insensitive expansion is
	     * required: Expand file names and add each match to the list.  If
	     * there is no match, and EW_NOTFOUND is given, add the pattern.
	     * Otherwise: Add the file name if it exists or when EW_NOTFOUND is
	     * given.
	     */
	    ಆದರೆ (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		ಆದರೆ ((flags & (EW_ಮಾರ್ಗ | EW_CDPATH))
			&& !mch_is_ತುಂಬಿದ_Name(p)
			&& !(p[0] == '.'
			    && (vim_ispathsep(p[1])
				|| (p[1] == '.' && vim_ispathsep(p[2]))))
		   )
		{
		    // :find completion where 'path' is used.
		    // Recursiveness is OK here.
		    recursive = FALSE;
		    ಕೂಡಿಸು_pat = expand_ಒಳ_ಮಾರ್ಗ(&ga, p, flags);
		    recursive = TRUE;
		    did_expand_ಒಳ_ಮಾರ್ಗ = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಕೂಡಿಸು_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	ಆದರೆ (ಕೂಡಿಸು_pat == -1 || (ಕೂಡಿಸು_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    ಆದರೆ (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    ಇಲ್ಲದಿದ್ದರೆ
		addfile(&ga, t, flags);

	    ಆದರೆ (t != p)
		vim_free(t);
	}

	ಆದರೆ (did_expand_ಒಳ_ಮಾರ್ಗ && ga.ga_len > 0 && (flags & (EW_ಮಾರ್ಗ | EW_CDPATH)))
	    uniquefy_paths(&ga, p, ಮಾರ್ಗ_option);
	ಆದರೆ (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    ಆದರೆ (retval == FAIL)
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);

    *ಅಂಕೆ_file = ga.ga_len;
    *file = (ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ) ? (char_u **)ga.ga_ದತ್ತಾಂಶ
						  : (char_u **)_("no matches");

    recursive = FALSE;

    ಹಿಂತಿರುಗಿ ((flags & EW_EMPTYOK) || ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ) ? retval : FAIL;
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
    ಪೂರ್ಣಾಂಕ		flags)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		isdir;
    stat_T	sb;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	ದೀರ್ಘ perm = mch_getperm(f);

	ಆದರೆ (perm < 0)
	    ಹಿಂತಿರುಗಿ;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	// if the file/dir/link doesn't exist, may not add it
	ಆದರೆ (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((ಅಕ್ಷರ *)f, &sb) < 0 : mch_getperm(f) < 0))
	    ಹಿಂತಿರುಗಿ;
	isdir = mch_isdir(f);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    ಆದರೆ (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	ಹಿಂತಿರುಗಿ;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    ಆದರೆ (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, ಖಾಲಿ, !(flags & EW_SHELLCMD)))
	ಹಿಂತಿರುಗಿ;

    // Make room for another item in the file list.
    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    p = alloc(STRLEN(f) + 1 + isdir);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    STRCPY(p, f);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    slash_adjust(p);
#ಆದರೆ_ಅಂತ್ಯ
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    ಆದರೆ (isdir && (flags & EW_ADDSLASH))
	ಕೂಡಿಸು_pathsep(p);
    ((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    ಶೂನ್ಯ
FreeWild(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, char_u **files)
{
    ಆದರೆ (ಎಣಿಕೆ_2 <= 0 || files == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆಗ (ಎಣಿಕೆ_2--)
	vim_free(files[ಎಣಿಕೆ_2]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    ಪೂರ್ಣಾಂಕ
pathcmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q, ಪೂರ್ಣಾಂಕ maxlen)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		c1, c2;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*s = ಖಾಲಿ;

    ಗಾಗಿ (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	ಆದರೆ (c1 == NUL)
	{
	    ಆದರೆ (c2 == NUL)  // full match
		ಹಿಂತಿರುಗಿ 0;
	    s = q;
	    i = j;
	    ನಿಲ್ಲಿಸು;
	}

	// End of "q": check if "p" just has a slash.
	ಆದರೆ (c2 == NUL)
	{
	    s = p;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    ಆದರೆ (vim_ispathsep(c1))
		ಹಿಂತಿರುಗಿ -1;
	    ಆದರೆ (vim_ispathsep(c2))
		ಹಿಂತಿರುಗಿ 1;
	    ಹಿಂತಿರುಗಿ p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    ಆದರೆ (s == ಖಾಲಿ)	// "i" or "j" ran into "maxlen"
	ಹಿಂತಿರುಗಿ 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    ಆದರೆ (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    && (c1 == '/' || c1 == '\\')
#ಇಲ್ಲದಿದ್ದರೆ
	    && c1 == '/'
#ಆದರೆ_ಅಂತ್ಯ
       )
	ಹಿಂತಿರುಗಿ 0;   // match with trailing slash
    ಆದರೆ (s == q)
	ಹಿಂತಿರುಗಿ -1;	    // no match
    ಹಿಂತಿರುಗಿ 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    ಪೂರ್ಣಾಂಕ
vim_isAbsName(char_u *name)
{
    ಹಿಂತಿರುಗಿ (ಮಾರ್ಗ_with_url(name) != 0 || mch_is_ತುಂಬಿದ_Name(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
vim_ತುಂಬಿದ_Name(
    char_u	*fname,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		force)	    // force expansion even when already absolute
{
    ಪೂರ್ಣಾಂಕ		retval = OK;
    ಪೂರ್ಣಾಂಕ		url;

    *buf = NUL;
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    url = ಮಾರ್ಗ_with_url(fname);
    ಆದರೆ (!url)
	retval = mch_ತುಂಬಿದ_Name(fname, buf, len, force);
    ಆದರೆ (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#ಆದರೆ defined(MSWIN)
    slash_adjust(buf);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ retval;
}
