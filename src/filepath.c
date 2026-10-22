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

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే MSWIN
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
    స్థిర పూర్ణాంకం
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    పూర్ణాంకం		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = జ్ఞాపకం_ఇవ్వు(len * పరిమాణం(*newbuf));
    ఒకవేళ (newbuf == ఖాళీ)
	తిరిగి FAIL;

    wfname = enc_గమ్యం_utf16(*fnamep, ఖాళీ);
    ఒకవేళ (wfname == ఖాళీ)
    {
	vim_free(newbuf);
	తిరిగి FAIL;
    }

    l = Get_Short_మార్గం_Name_W(wfname, newbuf, len);
    ఒకవేళ (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * పరిమాణం(*newbuf));
	ఒకవేళ (newbuf == ఖాళీ)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    తిరిగి FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = Get_Short_మార్గం_Name_W(wfname, newbuf, l+1);
    }
    ఒకవేళ (l != 0)
    {
	char_u *p = utf16_గమ్యం_enc(newbuf, ఖాళీ);

	ఒకవేళ (p != ఖాళీ)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	లేకపోతే
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    తిరిగి FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_వాడని = ఖాళీ;
    char_u	*short_fname = ఖాళీ;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    పూర్ణాంకం		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    ఒకవేళ (save_fname == ఖాళీ)
    {
	retval = FAIL;
	వెళ్ళు theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    కొరకు (;;)
    {
	// go back one path-separator
	అయితే (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	ఒకవేళ (endp <= save_fname)
	    ఆపు;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	ఒకవేళ (get_short_pathname(&short_fname, &pbuf_వాడని, &len) == FAIL)
	{
	    retval = FAIL;
	    వెళ్ళు theend;
	}
	*endp = ch;	// preserve the string

	ఒకవేళ (len > 0)
	    ఆపు;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    ఒకవేళ (len > 0)
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
	ఒకవేళ (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    ఒకవేళ (*fname == ఖాళీ)
	    {
		retval = FAIL;
		వెళ్ళు theend;
	    }
	}
	లేకపోతే
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    ఒకవేళ (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = ఖాళీ;
	}

	// concat the not-shortened part of the path
	ఒకవేళ ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_వాడని);
    vim_free(save_fname);

    తిరిగి retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    స్థిర పూర్ణాంకం
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    పూర్ణాంకం		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    పూర్ణాంకం		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    కొరకు (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	ఒకవేళ (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    ఒకవేళ (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    లేకపోతే
	pbuf = tfname = నిండిన_Name_save(*fnamep, FALSE);
    ఒకవేళ (tfname == ఖాళీ)
	తిరిగి FAIL;

    len = tflen = STRLEN(tfname);

    ఒకవేళ (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	ఒకవేళ (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    తిరిగి FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    కొరకు (p = tfname + len - 1; p >= tfname; --p)
    {
	ఒకవేళ (has_mbyte)
	    p -= mb_తల_off(tfname, p);
	ఒకవేళ (vim_ispathsep(*p))
	{
	    ఒకవేళ (sepcount == 0 || (hasTilde && sepcount == 1))
		ఆపు;
	    లేకపోతే
		sepcount --;
	}
    }
    ఒకవేళ (hasTilde)
    {
	--p;
	ఒకవేళ (p >= tfname)
	    *p = '~';
	లేకపోతే
	    తిరిగి FAIL;
    }
    లేకపోతే
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    తిరిగి OK;
}
#అయితే_ముగింపు // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    పూర్ణాంకం
modify_fname(
    char_u	*src,		// string with modifiers
    పూర్ణాంకం		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    పూర్ణాంకం		valid = 0;
    char_u	*తోక;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    పూర్ణాంకం		c;
    పూర్ణాంకం		has_fullname = 0;
    పూర్ణాంకం		has_homerelative = 0;
#నిర్వచితమైతే MSWIN
    char_u	*fname_ప్రారంభం = *fnamep;
    పూర్ణాంకం		has_shortname = 0;
#అయితే_ముగింపు

repeat:
    // ":p" - full path/file_name
    ఒకవేళ (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_మార్గం;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	ఒకవేళ ((*fnamep)[0] == '~'
#ఒకవేళ !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
		&& ((*fnamep)[1] == '/'
# ifdef BACKSLASH_లోపల_FILENAME
		    || (*fnamep)[1] == '\\'
# endif
		    || (*fnamep)[1] == NUL)
#అయితే_ముగింపు
		&& !(tilde_file && (*fnamep)[1] == NUL)
	   )
	{
	    *fnamep = expand_env_save(*fnamep);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ఒకవేళ (*fnamep == ఖాళీ)
		తిరిగి -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	కొరకు (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    ఒకవేళ (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		ఆపు;
	}

	// FullName_save() is slow, don't use it when not needed.
	ఒకవేళ (*p != NUL || !vim_isAbsName(*fnamep)
#నిర్వచితమైతే MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#అయితే_ముగింపు
	)
	{
	    *fnamep = నిండిన_Name_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ఒకవేళ (*fnamep == ఖాళీ)
		తిరిగి -1;
	}

#నిర్వచితమైతే MSWIN
# ఒకవేళ _WIN32_WINNT >= 0x0500
	ఒకవేళ (vim_strchr(*fnamep, '~') != ఖాళీ)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_గమ్యం_utf16(*fnamep, ఖాళీ);
	    WCHAR buf[MAX_మార్గం_2];

	    ఒకవేళ (wfname != ఖాళీ)
	    {
		ఒకవేళ (Get_Long_మార్గం_Name_W(wfname, buf, MAX_మార్గం_2))
		{
		    char_u *q = utf16_గమ్యం_enc(buf, ఖాళీ);

		    ఒకవేళ (q != ఖాళీ)
		    {
			vim_free(*bufp);    // free any allocated file name
			*bufp = *fnamep = q;
		    }
		}
		vim_free(wfname);
	    }
	}
# endif
#అయితే_ముగింపు
	// Append a path separator to a directory.
	ఒకవేళ (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ఒకవేళ (*fnamep == ఖాళీ)
		తిరిగి -1;
	    కూడు_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    అయితే (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	ఒకవేళ (c == '8')
	{
#నిర్వచితమైతే MSWIN
	    has_shortname = 1; // Postpone this.
#అయితే_ముగింపు
	    కొనసాగు;
	}
	pbuf = ఖాళీ;
	// Need full path first (use expand_env() to remove a "~/")
	ఒకవేళ (!has_fullname && !has_homerelative)
	{
	    ఒకవేళ (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    లేకపోతే
		p = pbuf = నిండిన_Name_save(*fnamep, FALSE);
	}
	లేకపోతే
	    p = *fnamep;

	has_fullname = 0;

	ఒకవేళ (p != ఖాళీ)
	{
	    size_t  dirnamelen = 0;

	    ఒకవేళ (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		ఒకవేళ (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    ఒకవేళ (s != ఖాళీ)
		    {
			dirnamelen = home_replace(ఖాళీ, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		ఒకవేళ (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		ఒకవేళ (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    ఒకవేళ (vim_ispathsep(*p))
		    {
			అయితే (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			ఒకవేళ (pbuf != ఖాళీ)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = ఖాళీ;
			}
		    }
		}
	    }
	    లేకపోతే
	    {
		dirnamelen = home_replace(ఖాళీ, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		ఒకవేళ (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    ఒకవేళ (s != ఖాళీ)
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

    తోక = gettail(*fnamep);
    *fnamelen = STRLEN(*fnamep);

    // ":h" - head, remove "/file_name", can be repeated
    // Don't remove the first "/" or "c:\"
    అయితే (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_తల;
	*usedlen += 2;
	s = get_past_తల(*fnamep);
	అయితే (తోక > s && after_pathsep(s, తోక))
	    MB_PTR_వెనుక(*fnamep, తోక);
	*fnamelen = తోక - *fnamep;
#నిర్వచితమైతే VMS
	ఒకవేళ (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#అయితే_ముగింపు
	ఒకవేళ (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    ఒకవేళ (p == ఖాళీ)
		తిరిగి -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = తోక = p;
	    *fnamelen = 1;
	}
	లేకపోతే
	{
	    అయితే (తోక > s && !after_pathsep(s, తోక))
		MB_PTR_వెనుక(*fnamep, తోక);
	}
    }

    // ":8" - shortname
    ఒకవేళ (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
    {
	*usedlen += 2;
#నిర్వచితమైతే MSWIN
	has_shortname = 1;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే MSWIN
    /*
     * Handle ":8" after we have done 'heads' and before we do 'tails'.
     */
    ఒకవేళ (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	ఒకవేళ (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_ప్రారంభం)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    ఒకవేళ (p == ఖాళీ)
		తిరిగి -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	ఒకవేళ (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    ఒకవేళ (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		తిరిగి -1;
	}
	లేకపోతే
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    ఒకవేళ (get_short_pathname(fnamep, bufp, &l) == FAIL)
		తిరిగి -1;

	    ఒకవేళ (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		ఒకవేళ (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    తిరిగి -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	తోక = gettail(*fnamep);
    }
#అయితే_ముగింపు // MSWIN

    // ":t" - tail, just the basename
    ఒకవేళ (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= తోక - *fnamep;
	*fnamep = తోక;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    అయితే (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	ఒకవేళ (src[*usedlen + 1] == 'e' && *fnamep > తోక)
	    s = *fnamep - 2;
	లేకపోతే
	    s = *fnamep + *fnamelen - 1;
	కొరకు ( ; s > తోక; --s)
	    ఒకవేళ (s[0] == '.')
		ఆపు;
	ఒకవేళ (src[*usedlen + 1] == 'e')		// :e
	{
	    ఒకవేళ (s > తోక)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#నిర్వచితమైతే VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		కొరకు ( ; s > *fnamep; --s)
		    ఒకవేళ (s[0] == ';')
			ఆపు;
		ఒకవేళ (s > *fnamep)
		    *fnamelen = s - *fnamep;
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ (*fnamep <= తోక)
		*fnamelen = 0;
	}
	లేకపోతే				// :r
	{
	    char_u *హద్దు = *fnamep;

	    ఒకవేళ (హద్దు < తోక)
		హద్దు = తోక;
	    ఒకవేళ (s > హద్దు)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    ఒకవేళ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	పూర్ణాంకం	    sep;
	char_u	    *flags;
	పూర్ణాంకం	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	ఒకవేళ (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	ఒకవేళ (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    ఒకవేళ (p != ఖాళీ)
	    {
		pat = vim_strnsave(s, p - s);
		ఒకవేళ (pat != ఖాళీ)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    ఒకవేళ (p != ఖాళీ)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			ఒకవేళ (sub != ఖాళీ && str != ఖాళీ)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, ఖాళీ, flags, &slen);
			    ఒకవేళ (s != ఖాళీ)
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
	    ఒకవేళ (didit)
		వెళ్ళు repeat;
	}
    }

    ఒకవేళ (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	ఒకవేళ (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	ఒకవేళ (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    తిరిగి valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    స్థిర శూన్య
shorten_dir_len(char_u *str, పూర్ణాంకం trim_len)
{
    char_u	*తోక, *s, *d;
    పూర్ణాంకం		skip = FALSE;
    పూర్ణాంకం		dirchunk_len = 0;

    తోక = gettail(str);
    d = str;
    కొరకు (s = str; ; ++s)
    {
	ఒకవేళ (s >= తోక)		    // copy the whole tail
	{
	    *d++ = *s;
	    ఒకవేళ (*s == NUL)
		ఆపు;
	}
	లేకపోతే ఒకవేళ (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	లేకపోతే ఒకవేళ (!skip)
	{
	    *d++ = *s;			// copy next char
	    ఒకవేళ (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		ఒకవేళ (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    ఒకవేళ (has_mbyte)
	    {
		పూర్ణాంకం l = mb_ptr2len(s);

		అయితే (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    శూన్య
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    పూర్ణాంకం
file_is_readable(char_u *fname)
{
    పూర్ణాంకం		fd;

#నిర్వచితం_కాకపోతే O_NONBLOCK
# define O_NONBLOCK 0
#అయితే_ముగింపు
    // Opening a file that does not exist is slow.
    ఒకవేళ (*fname == NUL || mch_isdir_or_missing(fname))
	తిరిగి FALSE;
    ఒకవేళ ((fd = mch_open((అక్షరం *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

#ఒకవేళ defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    శూన్య
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	ఒకవేళ (లోపల_vim9script())
	    (శూన్య) check_for_string_arg(argvars, 0);
	తిరిగి;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    ఒకవేళ (cwd != ఖాళీ)
    {
	ఒకవేళ (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_లోపల_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	ఒకవేళ (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	లేకపోతే ఒకవేళ (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	లేకపోతే ఒకవేళ (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_కిటికీ;
	లేకపోతే
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str_str), "scope", s);
	    తిరిగి;
	}
    }
    లేకపోతే ఒకవేళ (curwin->w_localdir != ఖాళీ)
	scope = CDSCOPE_కిటికీ;
    లేకపోతే ఒకవేళ (curtab->tp_localdir != ఖాళీ)
	scope = CDSCOPE_TABPAGE;

    ఒకవేళ (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    శూన్య
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    name = tv_get_string(&argvars[0]);
    ఒకవేళ (name == ఖాళీ || *name == NUL)
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి;
    }

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    లేకపోతే
	flags = (char_u *)"";

    ఒకవేళ (*flags == NUL)
	// delete a file
	rettv->vval.v_సంఖ్య = mch_remove(name) == 0 ? 0 : -1;
    లేకపోతే ఒకవేళ (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_సంఖ్య = mch_rmdir(name) == 0 ? 0 : -1;
    లేకపోతే ఒకవేళ (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_సంఖ్య = delete_recursive(name);
    లేకపోతే
	semsg(_(e_invalid_వ్యక్తీకరణ_str), flags);
}

/*
 * "executable()" function
 */
    శూన్య
f_executable(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_సంఖ్య = mch_can_exe(tv_get_string(&argvars[0]), ఖాళీ, TRUE);
}

/*
 * "exepath()" function
 */
    శూన్య
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = ఖాళీ;

    ఒకవేళ (లోపల_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	తిరిగి;
    (శూన్య)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    శూన్య
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;
    rettv->vval.v_సంఖ్య = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    శూన్య
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;
    rettv->vval.v_సంఖ్య = filewritable(tv_get_string(&argvars[0]));
}

    స్థిర శూన్య
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    పూర్ణాంకం		కనుగొను_what)
{
    char_u	*fname;
    char_u	*fresult = ఖాళీ;
    char_u	*మార్గం = *curbuf->b_p_మార్గం == NUL ? p_మార్గం : curbuf->b_p_మార్గం;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    పూర్ణాంకం		లెక్క = 1;
    పూర్ణాంకం		మొదటి = TRUE;
    పూర్ణాంకం		error = FALSE;

    rettv->vval.v_string = ఖాళీ;
    rettv->v_type = VAR_STRING;
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL)))
	తిరిగి;

    fname = tv_get_string(&argvars[0]);

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	ఒకవేళ (p == ఖాళీ)
	    error = TRUE;
	లేకపోతే
	{
	    ఒకవేళ (*p != NUL)
		మార్గం = p;

	    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
		లెక్క = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[2], &error);
	}
    }

    ఒకవేళ (లెక్క < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    ఒకవేళ (*fname != NUL && !error)
    {
	char_u	*file_గమ్యం_కనుగొను = ఖాళీ;
	అక్షరం	*శోధన_ctx = ఖాళీ;

	చేయి
	{
	    ఒకవేళ (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = కనుగొను_file_లోపల_మార్గం_option(మొదటి ? fname : ఖాళీ,
					       మొదటి ? (పూర్ణాంకం)STRLEN(fname) : 0,
					0, మొదటి, మార్గం,
					కనుగొను_what,
					curbuf->b_ffname,
					కనుగొను_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_గమ్యం_కనుగొను, &శోధన_ctx);
	    మొదటి = FALSE;

	    ఒకవేళ (fresult != ఖాళీ && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} అయితే ((rettv->v_type == VAR_LIST || --లెక్క > 0) && fresult != ఖాళీ);

	vim_free(file_గమ్యం_కనుగొను);
	vim_findfile_cleanup(శోధన_ctx);
    }

    ఒకవేళ (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    శూన్య
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    శూన్య
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    శూన్య
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = ఖాళీ;
    char_u	buf[NUMBUFLEN];

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    ఒకవేళ (mods == ఖాళీ || fname == ఖాళీ)
	fname = ఖాళీ;
    లేకపోతే
    {
	len = STRLEN(fname);
	ఒకవేళ (mods != ఖాళీ && *mods != NUL)
	    (శూన్య)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    ఒకవేళ (fname == ఖాళీ)
	rettv->vval.v_string = ఖాళీ;
    లేకపోతే
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
    శూన్య
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = ఖాళీ;
    tabpage_T	*tp = ఖాళీ;
    char_u	*cwd;
    పూర్ణాంకం		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_సంఖ్య
	    && argvars[0].vval.v_సంఖ్య == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    లేకపోతే
	wp = కనుగొను_tabwin(&argvars[0], &argvars[1], &tp);

    ఒకవేళ (wp != ఖాళీ && wp->w_localdir != ఖాళీ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    లేకపోతే ఒకవేళ (tp != ఖాళీ && tp->tp_localdir != ఖాళీ
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    లేకపోతే ఒకవేళ (wp != ఖాళీ || tp != ఖాళీ || global)
    {
	ఒకవేళ (globaldir != ఖాళీ && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	లేకపోతే
	{
	    cwd = alloc(MAXPATHL);
	    ఒకవేళ (cwd != ఖాళీ)
	    {
		ఒకవేళ (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_లోపల_FILENAME
    ఒకవేళ (rettv->vval.v_string != ఖాళీ)
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
    పూర్ణాంకం		    i;

    కొరకు (i = 0; i < 9; i++)
    {
	ఒకవేళ (st->st_బహుళకం & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	లేకపోతే
	    perm[i] = '-';
    }
    తిరిగి perm;
}

/*
 * "getfperm({fname})" function
 */
    శూన్య
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = ఖాళీ;
    char_u	permbuf[] = "---------";

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ఒకవేళ (mch_stat((అక్షరం *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    శూన్య
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    fname = tv_get_string(&argvars[0]);
    ఒకవేళ (mch_stat((అక్షరం *)fname, &st) >= 0)
    {
	ఒకవేళ (S_ISDIR(st.st_బహుళకం))
	    rettv->vval.v_సంఖ్య = 0;
	లేకపోతే
	{
	    rettv->vval.v_సంఖ్య = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    ఒకవేళ ((off_T)rettv->vval.v_సంఖ్య != (off_T)st.st_size)
		rettv->vval.v_సంఖ్య = -2;
	}
    }
    లేకపోతే
	  rettv->vval.v_సంఖ్య = -1;
}

/*
 * "getftime({fname})" function
 */
    శూన్య
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    fname = tv_get_string(&argvars[0]);
    ఒకవేళ (mch_stat((అక్షరం *)fname, &st) >= 0)
	rettv->vval.v_సంఖ్య = (varnumber_T)st.st_mtime;
    లేకపోతే
	rettv->vval.v_సంఖ్య = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    అక్షరం    *t;

    ఒకవేళ (S_ISREG(st->st_బహుళకం))
	t = "file";
    లేకపోతే ఒకవేళ (S_ISDIR(st->st_బహుళకం))
	t = "dir";
    లేకపోతే ఒకవేళ (S_ISLNK(st->st_బహుళకం))
	t = "link";
    లేకపోతే ఒకవేళ (S_ISBLK(st->st_బహుళకం))
	t = "bdev";
    లేకపోతే ఒకవేళ (S_ISCHR(st->st_బహుళకం))
	t = "cdev";
    లేకపోతే ఒకవేళ (S_ISFIFO(st->st_బహుళకం))
	t = "fifo";
    లేకపోతే ఒకవేళ (S_ISSOCK(st->st_బహుళకం))
	t = "socket";
    లేకపోతే
	t = "other";
    తిరిగి (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    శూన్య
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = ఖాళీ;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ఒకవేళ (mch_lstat((అక్షరం *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    శూన్య
f_glob(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	తిరిగి;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ఒకవేళ (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_సమితి(rettv, ఖాళీ);
	    ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    ఒకవేళ (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	ఒకవేళ (p_wic)
	    options += WILD_ICASE;
	ఒకవేళ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     ఖాళీ, options, WILD_ALL);
	లేకపోతే ఒకవేళ (rettv_list_alloc(rettv) == OK)
	{
	  పూర్ణాంకం i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						ఖాళీ, options, WILD_ALL_KEEP);
	  కొరకు (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    లేకపోతే
	rettv->vval.v_string = ఖాళీ;
}

/*
 * "glob2regpat()" function
 */
    శూన్య
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, లోపల_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == ఖాళీ)
			 ? ఖాళీ : file_pat_గమ్యం_reg_pat(pat, ఖాళీ, ఖాళీ, FALSE);
}

/*
 * "globpath()" function
 */
    శూన్య
f_globpath(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    పూర్ణాంకం		error = FALSE;
    garray_T	ga;
    పూర్ణాంకం		i;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	తిరిగి;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	ఒకవేళ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    ఒకవేళ (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_సమితి(rettv, ఖాళీ);
	    ఒకవేళ (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    ఒకవేళ (file != ఖాళీ && !error)
    {
	ga_init2(&ga, పరిమాణం(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	ఒకవేళ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_అక్షరశ్రేణులు(&ga, "\n");
	లేకపోతే ఒకవేళ (rettv_list_alloc(rettv) == OK)
	    కొరకు (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_దత్తాంశం))[i], -1);
	ga_clear_అక్షరశ్రేణులు(&ga);
    }
    లేకపోతే
	rettv->vval.v_string = ఖాళీ;
}

/*
 * "isdirectory()" function
 */
    శూన్య
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    శూన్య
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = mch_is_నిండిన_Name(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
mkdir_recurse(char_u *dir, పూర్ణాంకం prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    పూర్ణాంకం		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    ఒకవేళ (p <= get_past_తల(dir))
	తిరిగి OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    ఒకవేళ (updir == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (mch_isdir(updir))
	r = OK;
    లేకపోతే ఒకవేళ (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	ఒకవేళ (r == OK && created != ఖాళీ && *created == ఖాళీ)
	    *created = నిండిన_Name_save(updir, FALSE);
    }
    vim_free(updir);
    తిరిగి r;
}

/*
 * "mkdir()" function
 */
    శూన్య
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    పూర్ణాంకం		prot = 0755;
    పూర్ణాంకం		defer = FALSE;
    పూర్ణాంకం		defer_recurse = FALSE;
    char_u	*created = ఖాళీ;

    rettv->vval.v_సంఖ్య = FAIL;
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL)))
	తిరిగి;

    dir = tv_get_string_buf(&argvars[0], buf);
    ఒకవేళ (*dir == NUL)
	తిరిగి;

    ఒకవేళ (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[2], ఖాళీ);
	    ఒకవేళ (prot == -1)
		తిరిగి;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != ఖాళీ;
	defer_recurse = vim_strchr(arg2, 'R') != ఖాళీ;
	ఒకవేళ ((defer || defer_recurse) && !can_కూడు_defer())
	    తిరిగి;

	ఒకవేళ (vim_strchr(arg2, 'p') != ఖాళీ)
	{
	    ఒకవేళ (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_సంఖ్య = OK;
		తిరిగి;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : ఖాళీ);
	}
    }
    rettv->vval.v_సంఖ్య = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    ఒకవేళ (rettv->vval.v_సంఖ్య == OK
				&& created == ఖాళీ && (defer || defer_recurse))
	created = నిండిన_Name_save(dir, FALSE);
    ఒకవేళ (created != ఖాళీ)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_తాళం = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_తాళం = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	ఒకవేళ (tv[0].vval.v_string == ఖాళీ || tv[1].vval.v_string == ఖాళీ
		|| కూడు_defer((char_u *)"delete", 2, tv) == FAIL)
	{
	    vim_free(tv[0].vval.v_string);
	    vim_free(tv[1].vval.v_string);
	}
    }
}

/*
 * "pathshorten()" function
 */
    శూన్య
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    పూర్ణాంకం		trim_len = 1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[1]);
	ఒకవేళ (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    ఒకవేళ (p == ఖాళీ)
	rettv->vval.v_string = ఖాళీ;
    లేకపోతే
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	ఒకవేళ (p != ఖాళీ)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    స్థిర పూర్ణాంకం
checkitem_common(శూన్య *context, char_u *name, dict_T *dict)
{
    typval_T	*సమాసం = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    పూర్ణాంకం		retval = 0;
    పూర్ణాంకం		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    ఒకవేళ (name != ఖాళీ)
    {
	సమితి_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    లేకపోతే
    {
	సమితి_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    ఒకవేళ (eval_సమాసం_typval(సమాసం, FALSE, argv, 1, ఖాళీ, &rettv) == FAIL)
	వెళ్ళు theend;

    // We want to use -1, but also true/false should be allowed.
    ఒకవేళ (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_సంఖ్య;
	rettv.vval.v_సంఖ్య = rettv.vval.v_సంఖ్య == VVAL_TRUE;
    }
    retval = tv_get_సంఖ్య_chk(&rettv, &error);
    ఒకవేళ (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    ఒకవేళ (name != ఖాళీ)
	సమితి_vim_var_string(VV_VAL, ఖాళీ, 0);
    లేకపోతే
	సమితి_vim_var_dict(VV_VAL, ఖాళీ);
    restore_vimvar(VV_VAL, &save_val);
    తిరిగి retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    స్థిర పూర్ణాంకం
readdir_checkitem(శూన్య *context, శూన్య *అంశం)
{
    char_u	*name = (char_u *)అంశం;

    తిరిగి checkitem_common(context, name, ఖాళీ);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    స్థిర పూర్ణాంకం
readdirex_dict_arg(typval_T *argvars, పూర్ణాంకం *cmp)
{
    char_u     *compare;

    ఒకవేళ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (dict_has_కీలకం(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    లేకపోతే
    {
	semsg(_(e_dictionary_కీలకం_str_required), "sort");
	తిరిగి FAIL;
    }

    ఒకవేళ (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_క్రమీకరించు_NONE;
    లేకపోతే ఒకవేళ (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_క్రమీకరించు_BYTE;
    లేకపోతే ఒకవేళ (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_క్రమీకరించు_IC;
    లేకపోతే ఒకవేళ (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_క్రమీకరించు_COLLATE;
    తిరిగి OK;
}

/*
 * "readdir()" function
 */
    శూన్య
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*సమాసం;
    పూర్ణాంకం		ret;
    char_u	*మార్గం;
    char_u	*p;
    garray_T	ga;
    పూర్ణాంకం		i;
    పూర్ణాంకం		క్రమీకరించు = READDIR_క్రమీకరించు_BYTE;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;
    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	తిరిగి;

    మార్గం = tv_get_string(&argvars[0]);
    సమాసం = &argvars[1];

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &క్రమీకరించు) == FAIL)
	తిరిగి;

    ret = readdir_core(&ga, మార్గం, FALSE, (శూన్య *)సమాసం,
	    (సమాసం->v_type == VAR_UNKNOWN) ? ఖాళీ : readdir_checkitem, క్రమీకరించు);
    ఒకవేళ (ret == OK)
    {
	కొరకు (i = 0; i < ga.ga_len; i++)
	{
	    p = ((char_u **)ga.ga_దత్తాంశం)[i];
	    list_append_string(rettv->vval.v_list, p, -1);
	}
    }
    ga_clear_అక్షరశ్రేణులు(&ga);
}

/*
 * Evaluate "expr" (= "context") for readdirex().
 */
    స్థిర పూర్ణాంకం
readdirex_checkitem(శూన్య *context, శూన్య *అంశం)
{
    dict_T	*dict = (dict_T*)అంశం;

    తిరిగి checkitem_common(context, ఖాళీ, dict);
}

/*
 * "readdirex()" function
 */
    శూన్య
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*సమాసం;
    పూర్ణాంకం		ret;
    char_u	*మార్గం;
    garray_T	ga;
    పూర్ణాంకం		i;
    పూర్ణాంకం		క్రమీకరించు = READDIR_క్రమీకరించు_BYTE;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;
    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	తిరిగి;

    మార్గం = tv_get_string(&argvars[0]);
    సమాసం = &argvars[1];

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &క్రమీకరించు) == FAIL)
	తిరిగి;

    ret = readdir_core(&ga, మార్గం, TRUE, (శూన్య *)సమాసం,
	    (సమాసం->v_type == VAR_UNKNOWN) ? ఖాళీ : readdirex_checkitem, క్రమీకరించు);
    ఒకవేళ (ret == OK)
    {
	కొరకు (i = 0; i < ga.ga_len; i++)
	{
	    dict_T  *dict = ((dict_T**)ga.ga_దత్తాంశం)[i];
	    list_append_dict(rettv->vval.v_list, dict);
	    dict_unref(dict);
	}
    }
    ga_clear(&ga);
}

/*
 * "readfile()" function
 */
    స్థిర శూన్య
read_file_or_blob(typval_T *argvars, typval_T *rettv, పూర్ణాంకం always_blob)
{
    పూర్ణాంకం		ద్విమాన = FALSE;
    పూర్ణాంకం		blob = always_blob;
    పూర్ణాంకం		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    పూర్ణాంకం		io_కొలత = పరిమాణం(buf);
    పూర్ణాంకం		readlen;		// size of last fread()
    char_u	*prev	 = ఖాళీ;	// previously read bytes, if any
    దీర్ఘ	prevlen  = 0;		// length of data in prev
    దీర్ఘ	prevsize = 0;		// size of prev buffer
    దీర్ఘ	maxline  = MAXLNUM;
    దీర్ఘ	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*ప్రారంభం;			// start of current line
    off_T	స్థానభ్రంశం = 0;
    off_T	కొలత = -1;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (always_blob)
	{
	    స్థానభ్రంశం = (off_T)tv_get_సంఖ్య(&argvars[1]);
	    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
		కొలత = (off_T)tv_get_సంఖ్య(&argvars[2]);
	}
	లేకపోతే
	{
	    ఒకవేళ (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		ద్విమాన = TRUE;
	    ఒకవేళ (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (దీర్ఘ)tv_get_సంఖ్య(&argvars[2]);
	}
    }

    ఒకవేళ ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	తిరిగి;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    ఒకవేళ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	తిరిగి;
    }
    ఒకవేళ (*fname == NUL || (fd = mch_fopen((అక్షరం *)fname, READBIN)) == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	తిరిగి;
    }

    ఒకవేళ (blob)
    {
	ఒకవేళ (read_blob(fd, rettv, స్థానభ్రంశం, కొలత) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	తిరిగి;
    }

    అయితే (cnt < maxline || maxline < 0)
    {
	readlen = (పూర్ణాంకం)fread(buf, 1, io_కొలత, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	కొరకు (p = buf, ప్రారంభం = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || ద్విమాన));
		++p)
	{
	    ఒకవేళ (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= ఖాళీ;
		long_u	    len = p - ప్రారంభం;

		// Finished a line.  Remove CRs before NL.
		ఒకవేళ (readlen > 0 && !ద్విమాన)
		{
		    అయితే (len > 0 && ప్రారంభం[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    ఒకవేళ (len == 0)
			అయితే (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		ఒకవేళ (prevlen == 0)
		    s = vim_strnsave(ప్రారంభం, len);
		లేకపోతే
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    ఒకవేళ ((s = vim_realloc(prev, prevlen + len + 1)) != ఖాళీ)
		    {
			mch_memmove(s + prevlen, ప్రారంభం, len);
			s[prevlen + len] = NUL;
			prev = ఖాళీ; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		ఒకవేళ (s == ఖాళీ)
		{
		    do_outofmem_సందేశం((long_u) prevlen + len + 1);
		    failed = TRUE;
		    ఆపు;
		}

		ఒకవేళ ((li = listitem_alloc()) == ఖాళీ)
		{
		    vim_free(s);
		    failed = TRUE;
		    ఆపు;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_తాళం = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		ప్రారంభం = p + 1; // step over newline
		ఒకవేళ ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    లేకపోతే ఒకవేళ (*p == 0xbf && enc_utf8 && !ద్విమాన)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		ఒకవేళ (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    ఒకవేళ (ప్రారంభం == dest)
			ప్రారంభం = p + 1;
		    లేకపోతే
		    {
			// have to shuffle buf to close gap
			పూర్ణాంకం adjust_prevlen = 0;

			ఒకవేళ (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (పూర్ణాంకం)(buf - dest);
			    dest = buf;
			}
			ఒకవేళ (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	ఒకవేళ (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    ఆపు;
	ఒకవేళ (ప్రారంభం < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    ఒకవేళ (p - ప్రారంభం + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		ఒకవేళ (prevsize == 0)
		    prevsize = (దీర్ఘ)(p - ప్రారంభం);
		లేకపోతే
		{
		    దీర్ఘ grow50pc = (prevsize * 3) / 2;
		    దీర్ఘ growmin  = (దీర్ఘ)((p - ప్రారంభం) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		ఒకవేళ (newprev == ఖాళీ)
		{
		    do_outofmem_సందేశం((long_u)prevsize);
		    failed = TRUE;
		    ఆపు;
		}
		prev = newprev;
	    }
	    // Add the line part to end of "prev".
	    mch_memmove(prev + prevlen, ప్రారంభం, p - ప్రారంభం);
	    prevlen += (దీర్ఘ)(p - ప్రారంభం);
	}
    } // while

    // For a negative line count use only the lines at the end of the file,
    // free the rest.
    ఒకవేళ (!failed && maxline < 0)
	అయితే (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_మొదటి);
	    --cnt;
	}

    ఒకవేళ (failed)
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
    శూన్య
f_readblob(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL)))
	తిరిగి;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    శూన్య
f_readfile(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL)))
	తిరిగి;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    శూన్య
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = ఖాళీ;
# endif

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = ఖాళీ;

	v = mch_resolve_మార్గం(p, TRUE);
	ఒకవేళ (v != ఖాళీ)
	    rettv->vval.v_string = v;
	లేకపోతే
	    rettv->vval.v_string = vim_strsave(p);
    }
# లేకపోతే
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	పూర్ణాంకం	len;
	char_u	*remain = ఖాళీ;
	char_u	*q;
	పూర్ణాంకం	is_relative_గమ్యం_current = FALSE;
	పూర్ణాంకం	has_trailing_pathsep = FALSE;
	పూర్ణాంకం	హద్దు = 100;

	p = vim_strsave(p);
	ఒకవేళ (p == ఖాళీ)
	    వెళ్ళు fail;
	ఒకవేళ (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_గమ్యం_current = TRUE;

	len = STRLEN(p);
	ఒకవేళ (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	ఒకవేళ (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	ఒకవేళ (buf == ఖాళీ)
	{
	    vim_free(p);
	    vim_free(remain);
	    వెళ్ళు fail;
	}

	కొరకు (;;)
	{
	    కొరకు (;;)
	    {
		len = readlink((అక్షరం *)p, (అక్షరం *)buf, MAXPATHL);
		ఒకవేళ (len <= 0)
		    ఆపు;
		buf[len] = NUL;

		ఒకవేళ (హద్దు-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_చక్రం));
		    rettv->vval.v_string = ఖాళీ;
		    వెళ్ళు fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		ఒకవేళ (remain == ఖాళీ && has_trailing_pathsep)
		    కూడు_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		ఒకవేళ (*q != NUL)
		{
		    ఒకవేళ (remain == ఖాళీ)
			remain = vim_strsave(q - 1);
		    లేకపోతే
		    {
			cpy = concat_str(q - 1, remain);
			ఒకవేళ (cpy != ఖాళీ)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		ఒకవేళ (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		ఒకవేళ (q > p && !mch_is_నిండిన_Name(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    ఒకవేళ (cpy != ఖాళీ)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		లేకపోతే
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    ఒకవేళ (remain == ఖాళీ)
		ఆపు;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    ఒకవేళ (cpy != ఖాళీ)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    ఒకవేళ (*q != NUL)
		STRMOVE(remain, q - 1);
	    లేకపోతే
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	ఒకవేళ (!vim_ispathsep(*p))
	{
	    ఒకవేళ (is_relative_గమ్యం_current
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
		ఒకవేళ (cpy != ఖాళీ)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    లేకపోతే ఒకవేళ (!is_relative_గమ్యం_current)
	    {
		// Strip leading "./".
		q = p;
		అయితే (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		ఒకవేళ (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	ఒకవేళ (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    ఒకవేళ (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  లేకపోతే
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
    శూన్య
f_tempname(typval_T *argvars వాడని, typval_T *rettv)
{
    స్థిర పూర్ణాంకం	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    చేయి
    {
	ఒకవేళ (x == 'Z')
	    x = '0';
	లేకపోతే ఒకవేళ (x == '9')
	    x = 'A';
	లేకపోతే
	    ++x;
    } అయితే (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    శూన్య
f_writefile(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		ద్విమాన = FALSE;
    పూర్ణాంకం		append = FALSE;
    పూర్ణాంకం		defer = FALSE;
# ifdef HAVE_FSYNC
    పూర్ణాంకం		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    పూర్ణాంకం		ret = 0;
    listitem_T	*li;
    list_T	*list = ఖాళీ;
    blob_T	*blob = ఖాళీ;

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	ఒకవేళ (list == ఖాళీ)
	    తిరిగి;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_వస్తువులు(list, li)
	    ఒకవేళ (tv_get_string_chk(&li->li_tv) == ఖాళీ)
		తిరిగి;
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	ఒకవేళ (blob == ఖాళీ)
	    తిరిగి;
    }
    లేకపోతే
    {
	semsg(_(e_invalid_వాదం_str),
		_("writefile() first argument must be a List or a Blob"));
	తిరిగి;
    }

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	ఒకవేళ (arg2 == ఖాళీ)
	    తిరిగి;
	ఒకవేళ (vim_strchr(arg2, 'b') != ఖాళీ)
	    ద్విమాన = TRUE;
	ఒకవేళ (vim_strchr(arg2, 'a') != ఖాళీ)
	    append = TRUE;
	ఒకవేళ (vim_strchr(arg2, 'D') != ఖాళీ)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	ఒకవేళ (vim_strchr(arg2, 's') != ఖాళీ)
	    do_fsync = TRUE;
	లేకపోతే ఒకవేళ (vim_strchr(arg2, 'S') != ఖాళీ)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి;

    ఒకవేళ (defer && !can_కూడు_defer())
	తిరిగి;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    ఒకవేళ (*fname == NUL || (fd = mch_fopen((అక్షరం *)fname,
				      append ? APPENDBIN : WRITEBIN)) == ఖాళీ)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    లేకపోతే
    {
	ఒకవేళ (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_తాళం = 0;
	    tv.vval.v_string = నిండిన_Name_save(fname, FALSE);
	    ఒకవేళ (tv.vval.v_string == ఖాళీ
		    || కూడు_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(శూన్య)mch_remove(fname);
	    }
	}

	ఒకవేళ (ret == 0)
	{
	    ఒకవేళ (blob)
	    {
		ఒకవేళ (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (write_list(fd, list, ద్విమాన) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    ఒకవేళ (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_సంఖ్య = ret;
}

#అయితే_ముగింపు // FEAT_EVAL

#ఒకవేళ defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    పూర్ణాంకం		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    స్థిర char_u	*చివరి_dir = ఖాళీ;    // last used directory
    char_u		*tofree = ఖాళీ;
    పూర్ణాంకం			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    ఒకవేళ (title == ఖాళీ || *title == NUL)
    {
	ఒకవేళ (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	లేకపోతే ఒకవేళ (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	లేకపోతే
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    ఒకవేళ ((initdir == ఖాళీ || *initdir == NUL) && dflt != ఖాళీ && *dflt != NUL)
    {
	ఒకవేళ (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = ఖాళీ;
	}
	లేకపోతే ఒకవేళ (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    ఒకవేళ (tofree != ఖాళీ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    ఒకవేళ (initdir == ఖాళీ || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	ఒకవేళ (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	లేకపోతే ఒకవేళ (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != ఖాళీ && buf->b_ffname != ఖాళీ)
	{
	    ఒకవేళ (dflt == ఖాళీ || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    ఒకవేళ (tofree != ఖాళీ)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	లేకపోతే ఒకవేళ (*p_bsdir == 'l')
	    initdir = చివరి_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)		// when this changes, also adjust f_has()!
    {
	ఒకవేళ (filter == ఖాళీ
#  ifdef FEAT_EVAL
		&& (filter = get_var_విలువ((char_u *)"b:browsefilter")) == ఖాళీ
		&& (filter = get_var_విలువ((char_u *)"g:browsefilter")) == ఖాళీ
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	ఒకవేళ (flags & BROWSE_DIR)
	{
#  ఒకవేళ defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  లేకపోతే
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  ఒకవేళ !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    ఒకవేళ (fname != ఖాళీ && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *తోక = gettail_sep(fname);

		ఒకవేళ (తోక == fname)
		    *తోక++ = '.';	// use current dir
		*తోక = NUL;
	    }
#  endif
	}
	లేకపోతే
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    లేకపోతే
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_లోపల_console_బహుళకం));
	fname = ఖాళీ;
    }

    // keep the directory for next time
    ఒకవేళ (fname != ఖాళీ)
    {
	vim_free(చివరి_dir);
	చివరి_dir = vim_strsave(fname);
	ఒకవేళ (చివరి_dir != ఖాళీ && !(flags & BROWSE_DIR))
	{
	    *gettail(చివరి_dir) = NUL;
	    ఒకవేళ (*చివరి_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(చివరి_dir);
		చివరి_dir = alloc(MAXPATHL);
		ఒకవేళ (చివరి_dir != ఖాళీ)
		    mch_dirname(చివరి_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    తిరిగి fname;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    శూన్య
f_browse(typval_T *argvars వాడని, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    పూర్ణాంకం		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	తిరిగి;

    save = (పూర్ణాంకం)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    ఒకవేళ (error || title == ఖాళీ || initdir == ఖాళీ || defname == ఖాళీ)
	rettv->vval.v_string = ఖాళీ;
    లేకపోతే
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, ఖాళీ, initdir, ఖాళీ, curbuf);
# లేకపోతే
    rettv->vval.v_string = ఖాళీ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    శూన్య
f_browsedir(typval_T *argvars వాడని, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    ఒకవేళ (title == ఖాళీ || initdir == ఖాళీ)
	rettv->vval.v_string = ఖాళీ;
    లేకపోతే
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, ఖాళీ, ఖాళీ, initdir, ఖాళీ, curbuf);
# లేకపోతే
    rettv->vval.v_string = ఖాళీ;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    శూన్య
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_సంఖ్య = FALSE;

    ఒకవేళ (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	తిరిగి;

    from = tv_get_string(&argvars[0]);

    ఒకవేళ (mch_lstat((అక్షరం *)from, &st) >= 0
	&& (S_ISREG(st.st_బహుళకం) || S_ISLNK(st.st_బహుళకం)))
	rettv->vval.v_సంఖ్య = vim_copyfile(
	    tv_get_string(&argvars[0]),
	    tv_get_string(&argvars[1])) == OK ? TRUE : FALSE;
}

#అయితే_ముగింపు // FEAT_EVAL

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
    పూర్ణాంకం		dstlen,	// maximum length of the result
    పూర్ణాంకం		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_ప్రారంభం;

    ఒకవేళ (src == ఖాళీ)
    {
	*dst = NUL;
	తిరిగి 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    ఒకవేళ (buf != ఖాళీ && buf->b_help)
	తిరిగి vim_snprintf_safelen((అక్షరం *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    ఒకవేళ (homedir != ఖాళీ)
	dirlen = STRLEN(homedir);

#నిర్వచితమైతే VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#లేకపోతే
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    ఒకవేళ (homedir_env == ఖాళీ)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#అయితే_ముగింపు
    // Empty is the same as not set.
    ఒకవేళ (homedir_env != ఖాళీ && *homedir_env == NUL)
	homedir_env = ఖాళీ;

    ఒకవేళ (homedir_env != ఖాళీ && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = ఖాళీ;

	flen = STRLEN(homedir_env);
	(శూన్య)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	ఒకవేళ (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    ఒకవేళ (homedir_env != ఖాళీ)
	envlen = STRLEN(homedir_env);

    ఒకవేళ (!one)
	src = skipwhite(src);
    dst_ప్రారంభం = dst;		// remember the start
    అయితే (*src && dstlen > 0)
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
	కొరకు (;;)
	{
	    ఒకవేళ (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		ఒకవేళ (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		ఆపు;
	    }
	    ఒకవేళ (p == homedir_env)
		ఆపు;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	అయితే (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	అయితే ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    ఒకవేళ (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    తిరిగి (size_t)(dst - dst_ప్రారంభం);
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
    గుర్తులేని	len;

    len = 3;			// space for "~/" and trailing NUL
    ఒకవేళ (src != ఖాళీ)		// just in case
	len += (గుర్తులేని)STRLEN(src);
    dst = alloc(len);
    ఒకవేళ (dst != ఖాళీ)
	home_replace(buf, src, dst, len, TRUE);
    తిరిగి dst;
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
    పూర్ణాంకం
fullpathcmp(
    char_u *s1,
    char_u *s2,
    పూర్ణాంకం	    checkname,		// when both don't exist, check file names
    పూర్ణాంకం	    expandenv)
{
#నిర్వచితమైతే UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    పూర్ణాంకం		    r1, r2;

    ఒకవేళ (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    లేకపోతే
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((అక్షరం *)exp1, &st1);
    r2 = mch_stat((అక్షరం *)s2, &st2);
    ఒకవేళ (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	ఒకవేళ (checkname)
	{
	    ఒకవేళ (fnamecmp(exp1, s2) == 0)
		తిరిగి FPC_SAMEX;
	    r1 = vim_నిండిన_Name(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_నిండిన_Name(s2, full2, MAXPATHL, FALSE);
	    ఒకవేళ (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		తిరిగి FPC_SAMEX;
	}
	తిరిగి FPC_NOTX;
    }
    ఒకవేళ (r1 != 0 || r2 != 0)
	తిరిగి FPC_DIFFX;
    ఒకవేళ (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	తిరిగి FPC_SAME;
    తిరిగి FPC_DIFF;
#లేకపోతే
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    పూర్ణాంకం	    retval = FPC_DIFF;
    పూర్ణాంకం	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    ఒకవేళ ((exp1 = alloc(MAXPATHL * 3)) != ఖాళీ)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	ఒకవేళ (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	లేకపోతే
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_నిండిన_Name(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_నిండిన_Name(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	ఒకవేళ (r1 != OK && r2 != OK)
	{
	    ఒకవేళ (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    లేకపోతే
		retval = FPC_NOTX;
	}
	లేకపోతే ఒకవేళ (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	లేకపోతే ఒకవేళ (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	లేకపోతే
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    తిరిగి retval;
#అయితే_ముగింపు
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

    ఒకవేళ (fname == ఖాళీ)
	తిరిగి (char_u *)"";
    కొరకు (p1 = p2 = get_past_తల(fname); *p2; )	// find last part of path
    {
	ఒకవేళ (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    తిరిగి p1;
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

    p = get_past_తల(fname);	// don't remove the '/' from "c:/file"
    t = gettail(fname);
    అయితే (t > p && after_pathsep(fname, t))
	--t;
#నిర్వచితమైతే VMS
    // path separator is part of the path
    ++t;
#అయితే_ముగింపు
    తిరిగి t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    అయితే (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    ఒకవేళ (*fname)
	++fname;
    తిరిగి fname;
}

/*
 * Get a pointer to one character past the head of a path name.
 * Unix: after "/"; DOS: after "c:\"; Amiga: after "disk:/"; Mac: no head.
 * If there is no head, path is returned.
 */
    char_u *
get_past_తల(char_u *మార్గం)
{
    char_u  *retval;

#ఒకవేళ defined(MSWIN)
    // may skip "c:"
    ఒకవేళ (సురక్షిత_isalpha(మార్గం[0]) && మార్గం[1] == ':')
	retval = మార్గం + 2;
    లేకపోతే
	retval = మార్గం;
#లేకపోతే
# ఒకవేళ defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(మార్గం, ':');
    ఒకవేళ (retval == ఖాళీ)
	retval = మార్గం;
# లేకపోతే	// Unix
    retval = మార్గం;
# endif
#అయితే_ముగింపు

    అయితే (vim_ispathsep(*retval))
	++retval;

    తిరిగి retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    పూర్ణాంకం
vim_ispathsep(పూర్ణాంకం c)
{
#నిర్వచితమైతే UNIX
    తిరిగి (c == '/');	    // UNIX has ':' inside file names
#లేకపోతే
# ifdef BACKSLASH_లోపల_FILENAME
    తిరిగి (c == ':' || c == '/' || c == '\\');
# లేకపోతే
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    తిరిగి (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  లేకపోతే
    తిరిగి (c == ':' || c == '/');
#  endif // VMS
# endif
#అయితే_ముగింపు
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    పూర్ణాంకం
vim_ispathsep_nocolon(పూర్ణాంకం c)
{
    తిరిగి vim_ispathsep(c)
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	&& c != ':'
#అయితే_ముగింపు
	;
}

/*
 * Return TRUE if the directory of "fname" exists, FALSE otherwise.
 * Also returns TRUE if there is no directory name.
 * "fname" must be writable!.
 */
    పూర్ణాంకం
dir_యొక్క_file_exists(char_u *fname)
{
    char_u	*p;
    పూర్ణాంకం		c;
    పూర్ణాంకం		retval;

    p = gettail_sep(fname);
    ఒకవేళ (p == fname)
	తిరిగి TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    తిరిగి retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    పూర్ణాంకం
vim_fnamecmp(char_u *x, char_u *y)
{
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    తిరిగి vim_fnamencmp(x, y, MAXPATHL);
#లేకపోతే
    ఒకవేళ (p_fic)
	తిరిగి MB_STRICMP(x, y);
    తిరిగి STRCMP(x, y);
#అయితే_ముగింపు
}

    పూర్ణాంకం
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    పూర్ణాంకం		cx = NUL;
    పూర్ణాంకం		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    కొరకు (పూర్ణాంకం మార్పిడి = 0, drive = NUL; మార్పిడి < 2; ++మార్పిడి)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	ఒకవేళ ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    ఒకవేళ (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		ఆపు;
	    }
	    లేకపోతే // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    అయితే (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	ఒకవేళ (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    ఆపు;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    ఒకవేళ (len == 0)
	తిరిగి 0;
    తిరిగి (cx - cy);
#లేకపోతే
    ఒకవేళ (p_fic)
	తిరిగి MB_STRNICMP(x, y, len);
    తిరిగి STRNCMP(x, y, len);
#అయితే_ముగింపు
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, పూర్ణాంకం sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? పరిమాణం(PATHSEP) : 0) + fname2len + 1);
    ఒకవేళ (ret->string == ఖాళీ)
	ret->నిడివి = 0;
    లేకపోతే
    {
	STRCPY(ret->string, fname1);
	ret->నిడివి = fname1len;
	ఒకవేళ (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->నిడివి))
	{
	    STRCPY(ret->string + ret->నిడివి, PATHSEPSTR);
	    ret->నిడివి += పరిమాణం(PATHSEP);
	}
	STRCPY(ret->string + ret->నిడివి, fname2);
	ret->నిడివి += fname2len;
    }

    తిరిగి ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    శూన్య
కూడు_pathsep(char_u *p)
{
    size_t  plen;

    ఒకవేళ (*p == NUL)
	తిరిగి;

    plen = STRLEN(p);
    ఒకవేళ (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
నిండిన_Name_save(
    char_u	*fname,
    పూర్ణాంకం		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = ఖాళీ;

    ఒకవేళ (fname == ఖాళీ)
	తిరిగి ఖాళీ;

    buf = alloc(MAXPATHL);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (vim_నిండిన_Name(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    లేకపోతే
	new_fname = vim_strsave(fname);
    vim_free(buf);
    తిరిగి new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    పూర్ణాంకం
vim_fexists(char_u *fname)
{
    stat_T st;

    ఒకవేళ (mch_stat((అక్షరం *)fname, &st))
	తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    పూర్ణాంకం		  *అంకె_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    పూర్ణాంకం		   flags)	// EW_DIR, etc.
{
    పూర్ణాంకం		ret = FAIL;
    char_u	*eval_pat = ఖాళీ;
    char_u	*exp_pat = *pat;
    అక్షరం	*ignored_సందేశం;
    size_t	usedlen;
    పూర్ణాంకం		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    పూర్ణాంకం		నక్షత్రం_follows = FALSE;

    ఒకవేళ (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       ఖాళీ, &ignored_సందేశం, ఖాళీ, TRUE);
	--emsg_off;
	ఒకవేళ (eval_pat != ఖాళీ)
	{
	    నక్షత్రం_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    ఒకవేళ (exp_pat != ఖాళీ)
	ret = expand_wildcards(1, &exp_pat, అంకె_file, file, flags);

    ఒకవేళ (eval_pat != ఖాళీ)
    {
	ఒకవేళ (*అంకె_file == 0 && is_cur_alt_file && నక్షత్రం_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    ఒకవేళ (*file != ఖాళీ)
	    {
		**file = eval_pat;
		eval_pat = ఖాళీ;
		*అంకె_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    తిరిగి ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    పూర్ణాంకం
expand_wildcards(
    పూర్ణాంకం		   అంకె_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    పూర్ణాంకం		  *అంకె_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    పూర్ణాంకం		   flags)	// EW_DIR, etc.
{
    పూర్ణాంకం		retval;
    పూర్ణాంకం		i, j;
    char_u	*p;
    పూర్ణాంకం		non_suf_సరిపోలిక;	// number without matching suffix

    retval = gen_expand_wildcards(అంకె_pat, pat, అంకె_files, files, flags);

    // When keeping all matches, return here
    ఒకవేళ ((flags & EW_KEEPALL) || retval == FAIL)
	తిరిగి retval;

    /*
     * Remove names that match 'wildignore'.
     */
    ఒకవేళ (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	కొరకు (i = 0; i < *అంకె_files; ++i)
	{
	    ffname = నిండిన_Name_save((*files)[i], FALSE);
	    ఒకవేళ (ffname == ఖాళీ)		// out of memory
		ఆపు;
#నిర్వచితమైతే VMS
	    vms_remove_సంస్కరణ(ffname);
#అయితే_ముగింపు
	    ఒకవేళ (సరిపోలిక_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		కొరకు (j = i; j + 1 < *అంకె_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*అంకె_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	ఒకవేళ (*అంకె_files == 0)
	{
	    VIM_CLEAR(*files);
	    తిరిగి FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    ఒకవేళ (*అంకె_files > 1 && !got_int)
    {
	non_suf_సరిపోలిక = 0;
	కొరకు (i = 0; i < *అంకె_files; ++i)
	{
	    ఒకవేళ (!సరిపోలిక_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		కొరకు (j = i; j > non_suf_సరిపోలిక; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_సరిపోలిక++] = p;
	    }
	}
    }

    తిరిగి retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    పూర్ణాంకం
సరిపోలిక_suffix(char_u *fname)
{
    పూర్ణాంకం		fnamelen, setsuflen;
    char_u	*setsuf;
#నిర్వచించు MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (పూర్ణాంకం)STRLEN(fname);
    setsuflen = 0;
    కొరకు (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	ఒకవేళ (setsuflen == 0)
	{
	    char_u *తోక = gettail(fname);

	    // empty entry: match name without a '.'
	    ఒకవేళ (vim_strchr(తోక, '.') == ఖాళీ)
	    {
		setsuflen = 1;
		ఆపు;
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		ఆపు;
	    setsuflen = 0;
	}
    }
    తిరిగి (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    స్థిర పూర్ణాంకం
vim_backtick(char_u *p)
{
    తిరిగి (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    స్థిర పూర్ణాంకం
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    పూర్ణాంకం		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*బఫరు;
    పూర్ణాంకం		cnt = 0;
    పూర్ణాంకం		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    ఒకవేళ (cmd == ఖాళీ)
	తిరిగి -1;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (*cmd == '=')	    // `={expr}`: Expand expression
	బఫరు = eval_గమ్యం_string(cmd + 1, TRUE, FALSE);
    లేకపోతే
#అయితే_ముగింపు
	బఫరు = get_cmd_output(cmd, ఖాళీ,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, ఖాళీ);
    vim_free(cmd);
    ఒకవేళ (బఫరు == ఖాళీ)
	తిరిగి -1;

    cmd = బఫరు;
    అయితే (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	అయితే (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	ఒకవేళ (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	అయితే (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(బఫరు);
    తిరిగి cnt;
}

#ఒకవేళ defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    స్థిర పూర్ణాంకం
pstrcmp(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    తిరిగి (pathcmp(*(అక్షరం **)a, *(అక్షరం **)b, -1));
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
    పూర్ణాంకం
dos_expandpath(
    garray_T	*gap,
    char_u	*మార్గం,
    size_t	wildoff,
    పూర్ణాంకం		flags,		// EW_* flags
    పూర్ణాంకం		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*మార్గం_చివర;
    char_u	*p, *s, *e;
    పూర్ణాంకం		ప్రారంభం_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    పూర్ణాంకం		starts_with_dot;
    పూర్ణాంకం		matches;
    size_t	buflen;
    size_t	len;
    పూర్ణాంకం		starstar = FALSE;
    స్థిర పూర్ణాంకం	stardepth = 0;	    // depth for "**" expansion
    HANDLE		h_కనుగొను = INVALID_HANDLE_విలువ;
    WIN32_కనుగొను_DATAW    wfb;
    WCHAR		*wn = ఖాళీ;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    పూర్ణాంకం			ok;
    char_u		*p_alt;
    పూర్ణాంకం			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    ఒకవేళ (stardepth > 0)
    {
	ui_breakcheck();
	ఒకవేళ (got_int)
	    తిరిగి 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(మార్గం) + MAXPATHL;
    buf = alloc(buflen);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ఖాళీ;
    మార్గం_చివర = మార్గం;
    అయితే (*మార్గం_చివర != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ఒకవేళ (మార్గం_చివర >= మార్గం + wildoff && rem_backslash(మార్గం_చివర))
	    *p++ = *మార్గం_చివర++;
	లేకపోతే ఒకవేళ (*మార్గం_చివర == '\\' || *మార్గం_చివర == ':' || *మార్గం_చివర == '/')
	{
	    ఒకవేళ (e != ఖాళీ)
		ఆపు;
	    s = p + 1;
	}
	లేకపోతే ఒకవేళ (మార్గం_చివర >= మార్గం + wildoff
			 && vim_strchr((char_u *)"*?[~", *మార్గం_చివర) != ఖాళీ)
	    e = p;
	ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం charlen = (*mb_ptr2len)(మార్గం_చివర);

	    STRNCPY(p, మార్గం_చివర, (size_t)charlen);
	    p += charlen;
	    మార్గం_చివర += charlen;
	}
	లేకపోతే
	    *p++ = *మార్గం_చివర++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    కొరకు (p = buf + wildoff; p < s; ++p)
	ఒకవేళ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    కొరకు (p = s; p < e; ++p)
	ఒకవేళ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_గమ్యం_reg_pat(s, e, ఖాళీ, FALSE);
    ఒకవేళ (pat == ఖాళీ)
    {
	vim_free(buf);
	తిరిగి 0;
    }

    // compile the regexp into a program
    ఒకవేళ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ఒకవేళ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ఒకవేళ (regmatch.regprog == ఖాళీ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	తిరిగి 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != ఖాళీ;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ఒకవేళ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *మార్గం_చివర == '/')
    {
	vim_snprintf((అక్షరం *)s, buflen - len, "%s", మార్గం_చివర + 1);
	++stardepth;
	(శూన్య)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((అక్షరం *)s, buflen - len, "*.*");
    wn = enc_గమ్యం_utf16(buf, ఖాళీ);
    ఒకవేళ (wn != ఖాళీ)
	h_కనుగొను = కనుగొను_మొదటి_File_W(wn, &wfb);
    ok = (h_కనుగొను != INVALID_HANDLE_విలువ);

    అయితే (ok)
    {
	p = utf16_గమ్యం_enc(wfb.cFileName, ఖాళీ);   // p is allocated here

	ఒకవేళ (p == ఖాళీ)
	    ఆపు;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	ఒకవేళ (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = ఖాళీ;
	లేకపోతే
	    p_alt = utf16_గమ్యం_enc(wfb.cAlternateFileName, ఖాళీ);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	ఒకవేళ ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == ఖాళీ
		  || (regmatch.regprog != ఖాళీ
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != ఖాళీ
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(మార్గం + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((అక్షరం *)s, buflen - len, "%s", p);
	    ఒకవేళ (len + 1 < buflen)
	    {
		ఒకవేళ (starstar && stardepth < 100
			  && (wfb.dw_File_లక్షణాలు & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((అక్షరం *)buf + len, buflen - len,
							    "/**%s", మార్గం_చివర);
		    ++stardepth;
		    (శూన్య)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((అక్షరం *)buf + len, buflen - len, "%s", మార్గం_చివర);
		ఒకవేళ (mch_has_exp_wildcard(మార్గం_చివర))
		{
		    ఒకవేళ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(శూన్య)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		లేకపోతే
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ఒకవేళ (*మార్గం_చివర != 0)
			backslash_halve(buf + len + 1);
		    // addfile() checks existence; do not stat the same path twice.
		    // Even with EW_NOTFOUND, a literal suffix must exist.
		    addfile(gap, buf, flags & ~EW_NOTFOUND);
		}
	    }
	}

	vim_free(p_alt);
	vim_free(p);
	ok = కనుగొను_తదుపరి_File_W(h_కనుగొను, &wfb);
    }

    కనుగొను_Close(h_కనుగొను);
    vim_free(wn);
    vim_free(buf);
    vim_regfree(regmatch.regprog);
    vim_free(matchname);

    matches = gap->ga_len - ప్రారంభం_len;
    ఒకవేళ (matches > 0)
	qsort(((char_u **)gap->ga_దత్తాంశం) + ప్రారంభం_len, (size_t)matches,
						   పరిమాణం(char_u *), pstrcmp);
    తిరిగి matches;
}
#అయితే_ముగింపు // MSWIN

#ఒకవేళ (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    స్థిర పూర్ణాంకం
pstrcmp(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    తిరిగి (pathcmp(*(అక్షరం **)a, *(అక్షరం **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    పూర్ణాంకం
unix_expandpath(
    garray_T	*gap,
    char_u	*మార్గం,
    size_t	wildoff,
    పూర్ణాంకం		flags,		// EW_* flags
    పూర్ణాంకం		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*మార్గం_చివర;
    char_u	*p, *s, *e;
    పూర్ణాంకం		ప్రారంభం_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    పూర్ణాంకం		starts_with_dot;
    పూర్ణాంకం		matches;
    size_t	buflen;
    size_t	len;
    పూర్ణాంకం		starstar = FALSE;
    స్థిర పూర్ణాంకం	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    నిర్మాణం dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    ఒకవేళ (stardepth > 0)
    {
	ui_breakcheck();
	ఒకవేళ (got_int)
	    తిరిగి 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(మార్గం) + MAXPATHL;
    buf = alloc(buflen);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = ఖాళీ;
    మార్గం_చివర = మార్గం;
    అయితే (*మార్గం_చివర != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ఒకవేళ (మార్గం_చివర >= మార్గం + wildoff && rem_backslash(మార్గం_చివర))
	    *p++ = *మార్గం_చివర++;
	లేకపోతే ఒకవేళ (*మార్గం_చివర == '/')
	{
	    ఒకవేళ (e != ఖాళీ)
		ఆపు;
	    s = p + 1;
	}
	లేకపోతే ఒకవేళ (మార్గం_చివర >= మార్గం + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *మార్గం_చివర) != ఖాళీ
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(మార్గం_చివర)))))
	    e = p;
	ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం charlen = (*mb_ptr2len)(మార్గం_చివర);

	    STRNCPY(p, మార్గం_చివర, (size_t)charlen);
	    p += charlen;
	    మార్గం_చివర += charlen;
	}
	లేకపోతే
	    *p++ = *మార్గం_చివర++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    కొరకు (p = buf + wildoff; p < s; ++p)
	ఒకవేళ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    కొరకు (p = s; p < e; ++p)
	ఒకవేళ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_గమ్యం_reg_pat(s, e, ఖాళీ, FALSE);
    ఒకవేళ (pat == ఖాళీ)
    {
	vim_free(buf);
	తిరిగి 0;
    }

    // compile the regexp into a program
    ఒకవేళ (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    లేకపోతే
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ఒకవేళ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ఒకవేళ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ఒకవేళ (regmatch.regprog == ఖాళీ && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	తిరిగి 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ఒకవేళ (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *మార్గం_చివర == '/')
    {
	vim_snprintf((అక్షరం *)s, buflen - len, "%s", మార్గం_చివర + 1);
	++stardepth;
	(శూన్య)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (అక్షరం *)buf);

    // Find all matching entries
    ఒకవేళ (dirp != ఖాళీ)
    {
	అయితే (!got_int)
	{
	    dp = readdir(dirp);
	    ఒకవేళ (dp == ఖాళీ)
		ఆపు;
	    len = (size_t)(s - buf);
	    ఒకవేళ ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != ఖాళీ && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(మార్గం + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((అక్షరం *)s, buflen - len, "%s", dp->d_name);
		ఒకవేళ (len + 1 >= buflen)
		    కొనసాగు;

		ఒకవేళ (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((అక్షరం *)buf + len, buflen - len,
							    "/**%s", మార్గం_చివర);
		    ++stardepth;
		    (శూన్య)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((అక్షరం *)buf + len, buflen - len, "%s", మార్గం_చివర);
		ఒకవేళ (mch_has_exp_wildcard(మార్గం_చివర)) // handle more wildcards
		{
		    ఒకవేళ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(శూన్య)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		లేకపోతే
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ఒకవేళ (*మార్గం_చివర != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    ఒకవేళ ((flags & EW_ALLLINKS) ? mch_lstat((అక్షరం *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_మార్గం(buf, precomp_len, &precomp_len);

			ఒకవేళ (precomp_buf)
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
    matches = gap->ga_len - ప్రారంభం_len;
    ఒకవేళ (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_దత్తాంశం) + ప్రారంభం_len, matches,
						   పరిమాణం(char_u *), pstrcmp);
    తిరిగి matches;
}
#అయితే_ముగింపు

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    స్థిర పూర్ణాంకం
has_env_var(char_u *p)
{
    కొరకు ( ; *p; MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == '\\' && p[1] != NUL)
	    ++p;
	లేకపోతే ఒకవేళ (vim_strchr((char_u *)
#ఒకవేళ defined(MSWIN)
				    "$%"
#లేకపోతే
				    "$"
#అయితే_ముగింపు
					, *p) != ఖాళీ)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

#నిర్వచితమైతే SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    స్థిర పూర్ణాంకం
has_special_wildchar(char_u *p)
{
    కొరకు ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	ఒకవేళ (*p == '\r' || *p == '\n')
	    ఆపు;
	// Allow for escaping.
	ఒకవేళ (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	లేకపోతే ఒకవేళ (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != ఖాళీ)
	{
	    // A { must be followed by a matching }.
	    ఒకవేళ (*p == '{' && vim_strchr(p, '}') == ఖాళీ)
		కొనసాగు;
	    // A quote and backtick must be followed by another one.
	    ఒకవేళ ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == ఖాళీ)
		కొనసాగు;
	    తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
gen_expand_wildcards(
    పూర్ణాంకం		అంకె_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    పూర్ణాంకం		*అంకె_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    పూర్ణాంకం		flags)		// EW_* flags
{
    పూర్ణాంకం			i;
    garray_T		ga;
    char_u		*p;
    స్థిర పూర్ణాంకం		recursive = FALSE;
    పూర్ణాంకం			కూడు_pat;
    పూర్ణాంకం			retval = OK;
    పూర్ణాంకం			did_expand_లోపల_మార్గం = FALSE;
    char_u		*మార్గం_option = *curbuf->b_p_మార్గం == NUL ?
					p_మార్గం : curbuf->b_p_మార్గం;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    ఒకవేళ (recursive)
#నిర్వచితమైతే SPECIAL_WILDCHAR
	తిరిగి mch_expand_wildcards(అంకె_pat, pat, అంకె_file, file, flags);
#లేకపోతే
	తిరిగి FAIL;
#అయితే_ముగింపు

#నిర్వచితమైతే SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    కొరకు (i = 0; i < అంకె_pat; i++)
    {
	ఒకవేళ (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    తిరిగి mch_expand_wildcards(అంకె_pat, pat, అంకె_file, file, flags);
    }
#అయితే_ముగింపు

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, పరిమాణం(char_u *), 30);

    కొరకు (i = 0; i < అంకె_pat && !got_int; ++i)
    {
	కూడు_pat = -1;
	p = pat[i];

	ఒకవేళ (vim_backtick(p))
	{
	    కూడు_pat = expand_backtick(&ga, p, flags);
	    ఒకవేళ (కూడు_pat == -1)
		retval = FAIL;
	}
	లేకపోతే
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    ఒకవేళ ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)మార్గం_ESC_WILDCARDS);
		ఒకవేళ (p == ఖాళీ)
		    p = pat[i];
#నిర్వచితమైతే UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		లేకపోతే ఒకవేళ (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_అక్షరశ్రేణులు(&ga);
		    i = mch_expand_wildcards(అంకె_pat, pat, అంకె_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    తిరిగి i;
		}
#అయితే_ముగింపు
	    }

	    /*
	     * If there are wildcards or case-insensitive expansion is
	     * required: Expand file names and add each match to the list.  If
	     * there is no match, and EW_NOTFOUND is given, add the pattern.
	     * Otherwise: Add the file name if it exists or when EW_NOTFOUND is
	     * given.
	     */
	    ఒకవేళ (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		ఒకవేళ ((flags & (EW_మార్గం | EW_CDPATH))
			&& !mch_is_నిండిన_Name(p)
			&& !(p[0] == '.'
			    && (vim_ispathsep(p[1])
				|| (p[1] == '.' && vim_ispathsep(p[2]))))
		   )
		{
		    // :find completion where 'path' is used.
		    // Recursiveness is OK here.
		    recursive = FALSE;
		    కూడు_pat = expand_లోపల_మార్గం(&ga, p, flags);
		    recursive = TRUE;
		    did_expand_లోపల_మార్గం = TRUE;
		}
		లేకపోతే
		    కూడు_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	ఒకవేళ (కూడు_pat == -1 || (కూడు_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    ఒకవేళ (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    లేకపోతే
		addfile(&ga, t, flags);

	    ఒకవేళ (t != p)
		vim_free(t);
	}

	ఒకవేళ (did_expand_లోపల_మార్గం && ga.ga_len > 0 && (flags & (EW_మార్గం | EW_CDPATH)))
	    uniquefy_paths(&ga, p, మార్గం_option);
	ఒకవేళ (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    ఒకవేళ (retval == FAIL)
	ga_clear_అక్షరశ్రేణులు(&ga);

    *అంకె_file = ga.ga_len;
    *file = (ga.ga_దత్తాంశం != ఖాళీ) ? (char_u **)ga.ga_దత్తాంశం
						  : (char_u **)_("no matches");

    recursive = FALSE;

    తిరిగి ((flags & EW_EMPTYOK) || ga.ga_దత్తాంశం != ఖాళీ) ? retval : FAIL;
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
    శూన్య
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    పూర్ణాంకం		flags)
{
    char_u	*p;
    పూర్ణాంకం		isdir;
    stat_T	sb;

#నిర్వచితమైతే MSWIN
    ఒకవేళ (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	దీర్ఘ perm = mch_getperm(f);

	ఒకవేళ (perm < 0)
	    తిరిగి;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	// if the file/dir/link doesn't exist, may not add it
	ఒకవేళ (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((అక్షరం *)f, &sb) < 0 : mch_getperm(f) < 0))
	    తిరిగి;
	isdir = mch_isdir(f);
    }

#నిర్వచితమైతే FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    ఒకవేళ (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != ఖాళీ)
	తిరిగి;
#అయితే_ముగింపు

    ఒకవేళ ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	తిరిగి;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    ఒకవేళ (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, ఖాళీ, !(flags & EW_SHELLCMD)))
	తిరిగి;

    // Make room for another item in the file list.
    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి;

    p = alloc(STRLEN(f) + 1 + isdir);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    STRCPY(p, f);
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    slash_adjust(p);
#అయితే_ముగింపు
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    ఒకవేళ (isdir && (flags & EW_ADDSLASH))
	కూడు_pathsep(p);
    ((char_u **)gap->ga_దత్తాంశం)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    శూన్య
FreeWild(పూర్ణాంకం లెక్క, char_u **files)
{
    ఒకవేళ (లెక్క <= 0 || files == ఖాళీ)
	తిరిగి;
    అయితే (లెక్క--)
	vim_free(files[లెక్క]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    పూర్ణాంకం
pathcmp(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *q, పూర్ణాంకం maxlen)
{
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		c1, c2;
    స్థిరరాశి అక్షరం	*s = ఖాళీ;

    కొరకు (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	ఒకవేళ (c1 == NUL)
	{
	    ఒకవేళ (c2 == NUL)  // full match
		తిరిగి 0;
	    s = q;
	    i = j;
	    ఆపు;
	}

	// End of "q": check if "p" just has a slash.
	ఒకవేళ (c2 == NUL)
	{
	    s = p;
	    ఆపు;
	}

	ఒకవేళ ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#అయితే_ముగింపు
		)
	{
	    ఒకవేళ (vim_ispathsep(c1))
		తిరిగి -1;
	    ఒకవేళ (vim_ispathsep(c2))
		తిరిగి 1;
	    తిరిగి p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    ఒకవేళ (s == ఖాళీ)	// "i" or "j" ran into "maxlen"
	తిరిగి 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    ఒకవేళ (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    && (c1 == '/' || c1 == '\\')
#లేకపోతే
	    && c1 == '/'
#అయితే_ముగింపు
       )
	తిరిగి 0;   // match with trailing slash
    ఒకవేళ (s == q)
	తిరిగి -1;	    // no match
    తిరిగి 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    పూర్ణాంకం
vim_isAbsName(char_u *name)
{
    తిరిగి (మార్గం_with_url(name) != 0 || mch_is_నిండిన_Name(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
vim_నిండిన_Name(
    char_u	*fname,
    char_u	*buf,
    పూర్ణాంకం		len,
    పూర్ణాంకం		force)	    // force expansion even when already absolute
{
    పూర్ణాంకం		retval = OK;
    పూర్ణాంకం		url;

    *buf = NUL;
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి FAIL;

    url = మార్గం_with_url(fname);
    ఒకవేళ (!url)
	retval = mch_నిండిన_Name(fname, buf, len, force);
    ఒకవేళ (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#ఒకవేళ defined(MSWIN)
    slash_adjust(buf);
#అయితే_ముగింపు
    తిరిగి retval;
}
