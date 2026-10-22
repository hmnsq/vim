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

#شامل "vim.h"

#اگر_تعریف_شدہ MSWIN
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
    ساکن صحیح
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    صحیح		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = حافظہ_دو(len * حجم(*newbuf));
    اگر (newbuf == تہی)
	واپس FAIL;

    wfname = enc_مقصد_utf16(*fnamep, تہی);
    اگر (wfname == تہی)
    {
	vim_free(newbuf);
	واپس FAIL;
    }

    l = Get_Short_راستہ_Name_W(wfname, newbuf, len);
    اگر (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * حجم(*newbuf));
	اگر (newbuf == تہی)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    واپس FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = Get_Short_راستہ_Name_W(wfname, newbuf, l+1);
    }
    اگر (l != 0)
    {
	char_u *p = utf16_مقصد_enc(newbuf, تہی);

	اگر (p != تہی)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	ورنہ
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    واپس FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    واپس OK;
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
    ساکن صحیح
shortpath_for_invalid_fname(
    char_u	**fname,
    char_u	**bufp,
    size_t	*fnamelen)
{
    char_u	*save_fname;
    char_u	*pbuf_غیر_مستعمل = تہی;
    char_u	*short_fname = تہی;
    char_u	*endp, *save_endp;
    char_u	ch;
    size_t	old_len;
    size_t	len;
    size_t	new_len, sfx_len;
    صحیح		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    اگر (save_fname == تہی)
    {
	retval = FAIL;
	جاؤ theend;
    }

    endp = save_fname + old_len - 1; // Find the end of the copy
    save_endp = endp;

    /*
     * Try shortening the supplied path till it succeeds by removing one
     * directory at a time from the tail of the path.
     */
    len = 0;
    برائے (;;)
    {
	// go back one path-separator
	جب (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	اگر (endp <= save_fname)
	    توڑو;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	اگر (get_short_pathname(&short_fname, &pbuf_غیر_مستعمل, &len) == FAIL)
	{
	    retval = FAIL;
	    جاؤ theend;
	}
	*endp = ch;	// preserve the string

	اگر (len > 0)
	    توڑو;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    اگر (len > 0)
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
	اگر (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    اگر (*fname == تہی)
	    {
		retval = FAIL;
		جاؤ theend;
	    }
	}
	ورنہ
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    اگر (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = تہی;
	}

	// concat the not-shortened part of the path
	اگر ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_غیر_مستعمل);
    vim_free(save_fname);

    واپس retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    ساکن صحیح
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    صحیح		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    صحیح		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    برائے (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	اگر (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    اگر (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    ورنہ
	pbuf = tfname = بھرا_Name_save(*fnamep, FALSE);
    اگر (tfname == تہی)
	واپس FAIL;

    len = tflen = STRLEN(tfname);

    اگر (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	واپس FAIL;

    اگر (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	اگر (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    واپس FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    برائے (p = tfname + len - 1; p >= tfname; --p)
    {
	اگر (has_mbyte)
	    p -= mb_سر_off(tfname, p);
	اگر (vim_ispathsep(*p))
	{
	    اگر (sepcount == 0 || (hasTilde && sepcount == 1))
		توڑو;
	    ورنہ
		sepcount --;
	}
    }
    اگر (hasTilde)
    {
	--p;
	اگر (p >= tfname)
	    *p = '~';
	ورنہ
	    واپس FAIL;
    }
    ورنہ
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    واپس OK;
}
#اگر_ختم // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    صحیح
modify_fname(
    char_u	*src,		// string with modifiers
    صحیح		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    صحیح		valid = 0;
    char_u	*دم;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    صحیح		c;
    صحیح		has_fullname = 0;
    صحیح		has_homerelative = 0;
#اگر_تعریف_شدہ MSWIN
    char_u	*fname_آغاز = *fnamep;
    صحیح		has_shortname = 0;
#اگر_ختم

repeat:
    // ":p" - full path/file_name
    اگر (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_راستہ;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	اگر ((*fnamep)[0] == '~'
#اگر !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
		&& ((*fnamep)[1] == '/'
# ifdef BACKSLASH_اندر_FILENAME
		    || (*fnamep)[1] == '\\'
# endif
		    || (*fnamep)[1] == NUL)
#اگر_ختم
		&& !(tilde_file && (*fnamep)[1] == NUL)
	   )
	{
	    *fnamep = expand_env_save(*fnamep);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    اگر (*fnamep == تہی)
		واپس -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	برائے (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    اگر (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		توڑو;
	}

	// FullName_save() is slow, don't use it when not needed.
	اگر (*p != NUL || !vim_isAbsName(*fnamep)
#اگر_تعریف_شدہ MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#اگر_ختم
	)
	{
	    *fnamep = بھرا_Name_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    اگر (*fnamep == تہی)
		واپس -1;
	}

#اگر_تعریف_شدہ MSWIN
# اگر _WIN32_WINNT >= 0x0500
	اگر (vim_strchr(*fnamep, '~') != تہی)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_مقصد_utf16(*fnamep, تہی);
	    WCHAR buf[MAX_راستہ_2];

	    اگر (wfname != تہی)
	    {
		اگر (Get_Long_راستہ_Name_W(wfname, buf, MAX_راستہ_2))
		{
		    char_u *q = utf16_مقصد_enc(buf, تہی);

		    اگر (q != تہی)
		    {
			vim_free(*bufp);    // free any allocated file name
			*bufp = *fnamep = q;
		    }
		}
		vim_free(wfname);
	    }
	}
# endif
#اگر_ختم
	// Append a path separator to a directory.
	اگر (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    اگر (*fnamep == تہی)
		واپس -1;
	    جوڑو_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    جب (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	اگر (c == '8')
	{
#اگر_تعریف_شدہ MSWIN
	    has_shortname = 1; // Postpone this.
#اگر_ختم
	    جاری;
	}
	pbuf = تہی;
	// Need full path first (use expand_env() to remove a "~/")
	اگر (!has_fullname && !has_homerelative)
	{
	    اگر (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    ورنہ
		p = pbuf = بھرا_Name_save(*fnamep, FALSE);
	}
	ورنہ
	    p = *fnamep;

	has_fullname = 0;

	اگر (p != تہی)
	{
	    size_t  dirnamelen = 0;

	    اگر (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		اگر (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    اگر (s != تہی)
		    {
			dirnamelen = home_replace(تہی, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		اگر (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		اگر (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    اگر (vim_ispathsep(*p))
		    {
			جب (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			اگر (pbuf != تہی)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = تہی;
			}
		    }
		}
	    }
	    ورنہ
	    {
		dirnamelen = home_replace(تہی, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		اگر (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    اگر (s != تہی)
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

    دم = gettail(*fnamep);
    *fnamelen = STRLEN(*fnamep);

    // ":h" - head, remove "/file_name", can be repeated
    // Don't remove the first "/" or "c:\"
    جب (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_سر;
	*usedlen += 2;
	s = get_past_سر(*fnamep);
	جب (دم > s && after_pathsep(s, دم))
	    MB_PTR_عقب(*fnamep, دم);
	*fnamelen = دم - *fnamep;
#اگر_تعریف_شدہ VMS
	اگر (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#اگر_ختم
	اگر (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    اگر (p == تہی)
		واپس -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = دم = p;
	    *fnamelen = 1;
	}
	ورنہ
	{
	    جب (دم > s && !after_pathsep(s, دم))
		MB_PTR_عقب(*fnamep, دم);
	}
    }

    // ":8" - shortname
    اگر (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
    {
	*usedlen += 2;
#اگر_تعریف_شدہ MSWIN
	has_shortname = 1;
#اگر_ختم
    }

#اگر_تعریف_شدہ MSWIN
    /*
     * Handle ":8" after we have done 'heads' and before we do 'tails'.
     */
    اگر (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	اگر (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_آغاز)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    اگر (p == تہی)
		واپس -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	اگر (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    اگر (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		واپس -1;
	}
	ورنہ
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    اگر (get_short_pathname(fnamep, bufp, &l) == FAIL)
		واپس -1;

	    اگر (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		اگر (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    واپس -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	دم = gettail(*fnamep);
    }
#اگر_ختم // MSWIN

    // ":t" - tail, just the basename
    اگر (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= دم - *fnamep;
	*fnamep = دم;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    جب (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	اگر (src[*usedlen + 1] == 'e' && *fnamep > دم)
	    s = *fnamep - 2;
	ورنہ
	    s = *fnamep + *fnamelen - 1;
	برائے ( ; s > دم; --s)
	    اگر (s[0] == '.')
		توڑو;
	اگر (src[*usedlen + 1] == 'e')		// :e
	{
	    اگر (s > دم)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#اگر_تعریف_شدہ VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		برائے ( ; s > *fnamep; --s)
		    اگر (s[0] == ';')
			توڑو;
		اگر (s > *fnamep)
		    *fnamelen = s - *fnamep;
#اگر_ختم
	    }
	    ورنہ اگر (*fnamep <= دم)
		*fnamelen = 0;
	}
	ورنہ				// :r
	{
	    char_u *حد_بندی = *fnamep;

	    اگر (حد_بندی < دم)
		حد_بندی = دم;
	    اگر (s > حد_بندی)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    اگر (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	صحیح	    sep;
	char_u	    *flags;
	صحیح	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	اگر (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	اگر (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    اگر (p != تہی)
	    {
		pat = vim_strnsave(s, p - s);
		اگر (pat != تہی)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    اگر (p != تہی)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			اگر (sub != تہی && str != تہی)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, تہی, flags, &slen);
			    اگر (s != تہی)
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
	    اگر (didit)
		جاؤ repeat;
	}
    }

    اگر (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	اگر (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	اگر (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	اگر (p == تہی)
	    واپس -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    واپس valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    ساکن خالی
shorten_dir_len(char_u *str, صحیح trim_len)
{
    char_u	*دم, *s, *d;
    صحیح		skip = FALSE;
    صحیح		dirchunk_len = 0;

    دم = gettail(str);
    d = str;
    برائے (s = str; ; ++s)
    {
	اگر (s >= دم)		    // copy the whole tail
	{
	    *d++ = *s;
	    اگر (*s == NUL)
		توڑو;
	}
	ورنہ اگر (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	ورنہ اگر (!skip)
	{
	    *d++ = *s;			// copy next char
	    اگر (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		اگر (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    اگر (has_mbyte)
	    {
		صحیح l = mb_ptr2len(s);

		جب (--l > 0)
		    *d++ = *++s;
	    }
	}
    }
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * It's done in-place.
 */
    خالی
shorten_dir(char_u *str)
{
    shorten_dir_len(str, 1);
}

/*
 * Return TRUE if "fname" is a readable file.
 */
    صحیح
file_is_readable(char_u *fname)
{
    صحیح		fd;

#اگر_غیر_تعریف_شدہ O_NONBLOCK
# define O_NONBLOCK 0
#اگر_ختم
    // Opening a file that does not exist is slow.
    اگر (*fname == NUL || mch_isdir_or_missing(fname))
	واپس FALSE;
    اگر ((fd = mch_open((حرف *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	واپس TRUE;
    }
    واپس FALSE;
}

#اگر defined(FEAT_EVAL)

/*
 * "chdir(dir)" function
 */
    خالی
f_chdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*cwd;
    cdscope_T	scope = CDSCOPE_GLOBAL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (check_secure())
	واپس;

    اگر (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	اگر (اندر_vim9script())
	    (خالی) check_for_string_arg(argvars, 0);
	واپس;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    اگر (cwd != تہی)
    {
	اگر (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_اندر_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	اگر (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	ورنہ اگر (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	ورنہ اگر (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_کھڑکی;
	ورنہ
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str_str), "scope", s);
	    واپس;
	}
    }
    ورنہ اگر (curwin->w_localdir != تہی)
	scope = CDSCOPE_کھڑکی;
    ورنہ اگر (curtab->tp_localdir != تہی)
	scope = CDSCOPE_TABPAGE;

    اگر (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
	// Directory change failed
	VIM_CLEAR(rettv->vval.v_string);
}

/*
 * "delete()" function
 */
    خالی
f_delete(typval_T *argvars, typval_T *rettv)
{
    char_u	nbuf[NUMBUFLEN];
    char_u	*name;
    char_u	*flags;

    rettv->vval.v_نمبر = -1;
    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    name = tv_get_string(&argvars[0]);
    اگر (name == تہی || *name == NUL)
    {
	emsg(_(e_invalid_دلیل));
	واپس;
    }

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    ورنہ
	flags = (char_u *)"";

    اگر (*flags == NUL)
	// delete a file
	rettv->vval.v_نمبر = mch_remove(name) == 0 ? 0 : -1;
    ورنہ اگر (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_نمبر = mch_rmdir(name) == 0 ? 0 : -1;
    ورنہ اگر (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_نمبر = delete_recursive(name);
    ورنہ
	semsg(_(e_invalid_عبارت_str), flags);
}

/*
 * "executable()" function
 */
    خالی
f_executable(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    // Check in $PATH and also check directly if there is a directory name.
    rettv->vval.v_نمبر = mch_can_exe(tv_get_string(&argvars[0]), تہی, TRUE);
}

/*
 * "exepath()" function
 */
    خالی
f_exepath(typval_T *argvars, typval_T *rettv)
{
    char_u *p = تہی;

    اگر (اندر_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	واپس;
    (خالی)mch_can_exe(tv_get_string(&argvars[0]), &p, TRUE);
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p;
}

/*
 * "filereadable()" function
 */
    خالی
f_filereadable(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;
    rettv->vval.v_نمبر = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    خالی
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;
    rettv->vval.v_نمبر = filewritable(tv_get_string(&argvars[0]));
}

    ساکن خالی
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		ڈھونڈو_what)
{
    char_u	*fname;
    char_u	*fresult = تہی;
    char_u	*راستہ = *curbuf->b_p_راستہ == NUL ? p_راستہ : curbuf->b_p_راستہ;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    صحیح		گنتی = 1;
    صحیح		پہلا = TRUE;
    صحیح		error = FALSE;

    rettv->vval.v_string = تہی;
    rettv->v_type = VAR_STRING;
    اگر (اندر_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 2) == FAIL)))
	واپس;

    fname = tv_get_string(&argvars[0]);

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	اگر (p == تہی)
	    error = TRUE;
	ورنہ
	{
	    اگر (*p != NUL)
		راستہ = p;

	    اگر (argvars[2].v_type != VAR_UNKNOWN)
		گنتی = (صحیح)tv_get_نمبر_chk(&argvars[2], &error);
	}
    }

    اگر (گنتی < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    اگر (*fname != NUL && !error)
    {
	char_u	*file_مقصد_ڈھونڈو = تہی;
	حرف	*تلاش_ctx = تہی;

	کرو
	{
	    اگر (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = ڈھونڈو_file_اندر_راستہ_option(پہلا ? fname : تہی,
					       پہلا ? (صحیح)STRLEN(fname) : 0,
					0, پہلا, راستہ,
					ڈھونڈو_what,
					curbuf->b_ffname,
					ڈھونڈو_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_مقصد_ڈھونڈو, &تلاش_ctx);
	    پہلا = FALSE;

	    اگر (fresult != تہی && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} جب ((rettv->v_type == VAR_LIST || --گنتی > 0) && fresult != تہی);

	vim_free(file_مقصد_ڈھونڈو);
	vim_findfile_cleanup(تلاش_ctx);
    }

    اگر (rettv->v_type == VAR_STRING)
	rettv->vval.v_string = fresult;
}

/*
 * "finddir({fname}[, {path}[, {count}]])" function
 */
    خالی
f_finddir(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_DIR);
}

/*
 * "findfile({fname}[, {path}[, {count}]])" function
 */
    خالی
f_findfile(typval_T *argvars, typval_T *rettv)
{
    findfilendir(argvars, rettv, FINDFILE_FILE);
}

/*
 * "fnamemodify({fname}, {mods})" function
 */
    خالی
f_fnamemodify(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    char_u	*mods;
    size_t	usedlen = 0;
    size_t	len = 0;
    char_u	*fbuf = تہی;
    char_u	buf[NUMBUFLEN];

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    اگر (mods == تہی || fname == تہی)
	fname = تہی;
    ورنہ
    {
	len = STRLEN(fname);
	اگر (mods != تہی && *mods != NUL)
	    (خالی)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    اگر (fname == تہی)
	rettv->vval.v_string = تہی;
    ورنہ
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
    خالی
f_getcwd(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = تہی;
    tabpage_T	*tp = تہی;
    char_u	*cwd;
    صحیح		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script()
	    && (check_for_opt_نمبر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 1) == FAIL)))
	واپس;

    اگر (argvars[0].v_type == VAR_نمبر
	    && argvars[0].vval.v_نمبر == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    ورنہ
	wp = ڈھونڈو_tabwin(&argvars[0], &argvars[1], &tp);

    اگر (wp != تہی && wp->w_localdir != تہی
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    ورنہ اگر (tp != تہی && tp->tp_localdir != تہی
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    ورنہ اگر (wp != تہی || tp != تہی || global)
    {
	اگر (globaldir != تہی && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	ورنہ
	{
	    cwd = alloc(MAXPATHL);
	    اگر (cwd != تہی)
	    {
		اگر (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_اندر_FILENAME
    اگر (rettv->vval.v_string != تہی)
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
    صحیح		    i;

    برائے (i = 0; i < 9; i++)
    {
	اگر (st->st_منوال & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	ورنہ
	    perm[i] = '-';
    }
    واپس perm;
}

/*
 * "getfperm({fname})" function
 */
    خالی
f_getfperm(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*perm = تہی;
    char_u	permbuf[] = "---------";

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    اگر (mch_stat((حرف *)fname, &st) >= 0)
	perm = vim_strsave(getfpermst(&st, permbuf));
    rettv->vval.v_string = perm;
}

/*
 * "getfsize({fname})" function
 */
    خالی
f_getfsize(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    fname = tv_get_string(&argvars[0]);
    اگر (mch_stat((حرف *)fname, &st) >= 0)
    {
	اگر (S_ISDIR(st.st_منوال))
	    rettv->vval.v_نمبر = 0;
	ورنہ
	{
	    rettv->vval.v_نمبر = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    اگر ((off_T)rettv->vval.v_نمبر != (off_T)st.st_size)
		rettv->vval.v_نمبر = -2;
	}
    }
    ورنہ
	  rettv->vval.v_نمبر = -1;
}

/*
 * "getftime({fname})" function
 */
    خالی
f_getftime(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    fname = tv_get_string(&argvars[0]);
    اگر (mch_stat((حرف *)fname, &st) >= 0)
	rettv->vval.v_نمبر = (varnumber_T)st.st_mtime;
    ورنہ
	rettv->vval.v_نمبر = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    حرف    *t;

    اگر (S_ISREG(st->st_منوال))
	t = "file";
    ورنہ اگر (S_ISDIR(st->st_منوال))
	t = "dir";
    ورنہ اگر (S_ISLNK(st->st_منوال))
	t = "link";
    ورنہ اگر (S_ISBLK(st->st_منوال))
	t = "bdev";
    ورنہ اگر (S_ISCHR(st->st_منوال))
	t = "cdev";
    ورنہ اگر (S_ISFIFO(st->st_منوال))
	t = "fifo";
    ورنہ اگر (S_ISSOCK(st->st_منوال))
	t = "socket";
    ورنہ
	t = "other";
    واپس (char_u*)t;
}

/*
 * "getftype({fname})" function
 */
    خالی
f_getftype(typval_T *argvars, typval_T *rettv)
{
    char_u	*fname;
    stat_T	st;
    char_u	*type = تہی;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    اگر (mch_lstat((حرف *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    خالی
f_glob(typval_T *argvars, typval_T *rettv)
{
    صحیح		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    صحیح		error = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	واپس;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    اگر (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_سیٹ(rettv, تہی);
	    اگر (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    اگر (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	اگر (p_wic)
	    options += WILD_ICASE;
	اگر (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     تہی, options, WILD_ALL);
	ورنہ اگر (rettv_list_alloc(rettv) == OK)
	{
	  صحیح i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						تہی, options, WILD_ALL_KEEP);
	  برائے (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    ورنہ
	rettv->vval.v_string = تہی;
}

/*
 * "glob2regpat()" function
 */
    خالی
f_glob2regpat(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[NUMBUFLEN];
    char_u	*pat;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    pat = tv_get_string_buf_chk_strict(&argvars[0], buf, اندر_vim9script());
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = (pat == تہی)
			 ? تہی : file_pat_مقصد_reg_pat(pat, تہی, تہی, FALSE);
}

/*
 * "globpath()" function
 */
    خالی
f_globpath(typval_T *argvars, typval_T *rettv)
{
    صحیح		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    صحیح		error = FALSE;
    garray_T	ga;
    صحیح		i;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	واپس;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	اگر (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	اگر (argvars[3].v_type != VAR_UNKNOWN)
	{
	    اگر (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_سیٹ(rettv, تہی);
	    اگر (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    اگر (file != تہی && !error)
    {
	ga_init2(&ga, حجم(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	اگر (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_لڑیاں(&ga, "\n");
	ورنہ اگر (rettv_list_alloc(rettv) == OK)
	    برائے (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_ڈیٹا))[i], -1);
	ga_clear_لڑیاں(&ga);
    }
    ورنہ
	rettv->vval.v_string = تہی;
}

/*
 * "isdirectory()" function
 */
    خالی
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    خالی
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = mch_is_بھرا_Name(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    ساکن صحیح
mkdir_recurse(char_u *dir, صحیح prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    صحیح		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    اگر (p <= get_past_سر(dir))
	واپس OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    اگر (updir == تہی)
	واپس FAIL;
    اگر (mch_isdir(updir))
	r = OK;
    ورنہ اگر (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	اگر (r == OK && created != تہی && *created == تہی)
	    *created = بھرا_Name_save(updir, FALSE);
    }
    vim_free(updir);
    واپس r;
}

/*
 * "mkdir()" function
 */
    خالی
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    صحیح		prot = 0755;
    صحیح		defer = FALSE;
    صحیح		defer_recurse = FALSE;
    char_u	*created = تہی;

    rettv->vval.v_نمبر = FAIL;
    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 2) == FAIL)))
	واپس;

    dir = tv_get_string_buf(&argvars[0], buf);
    اگر (*dir == NUL)
	واپس;

    اگر (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (صحیح)tv_get_نمبر_chk(&argvars[2], تہی);
	    اگر (prot == -1)
		واپس;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != تہی;
	defer_recurse = vim_strchr(arg2, 'R') != تہی;
	اگر ((defer || defer_recurse) && !can_جوڑو_defer())
	    واپس;

	اگر (vim_strchr(arg2, 'p') != تہی)
	{
	    اگر (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_نمبر = OK;
		واپس;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : تہی);
	}
    }
    rettv->vval.v_نمبر = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    اگر (rettv->vval.v_نمبر == OK
				&& created == تہی && (defer || defer_recurse))
	created = بھرا_Name_save(dir, FALSE);
    اگر (created != تہی)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_تالا = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_تالا = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	اگر (tv[0].vval.v_string == تہی || tv[1].vval.v_string == تہی
		|| جوڑو_defer((char_u *)"delete", 2, tv) == FAIL)
	{
	    vim_free(tv[0].vval.v_string);
	    vim_free(tv[1].vval.v_string);
	}
    }
}

/*
 * "pathshorten()" function
 */
    خالی
f_pathshorten(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
    صحیح		trim_len = 1;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (صحیح)tv_get_نمبر(&argvars[1]);
	اگر (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    اگر (p == تہی)
	rettv->vval.v_string = تہی;
    ورنہ
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	اگر (p != تہی)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    ساکن صحیح
checkitem_common(خالی *context, char_u *name, dict_T *dict)
{
    typval_T	*اظہاریہ = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    صحیح		retval = 0;
    صحیح		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    اگر (name != تہی)
    {
	سیٹ_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    ورنہ
    {
	سیٹ_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    اگر (eval_اظہاریہ_typval(اظہاریہ, FALSE, argv, 1, تہی, &rettv) == FAIL)
	جاؤ theend;

    // We want to use -1, but also true/false should be allowed.
    اگر (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_نمبر;
	rettv.vval.v_نمبر = rettv.vval.v_نمبر == VVAL_TRUE;
    }
    retval = tv_get_نمبر_chk(&rettv, &error);
    اگر (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    اگر (name != تہی)
	سیٹ_vim_var_string(VV_VAL, تہی, 0);
    ورنہ
	سیٹ_vim_var_dict(VV_VAL, تہی);
    restore_vimvar(VV_VAL, &save_val);
    واپس retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    ساکن صحیح
readdir_checkitem(خالی *context, خالی *شے)
{
    char_u	*name = (char_u *)شے;

    واپس checkitem_common(context, name, تہی);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    ساکن صحیح
readdirex_dict_arg(typval_T *argvars, صحیح *cmp)
{
    char_u     *compare;

    اگر (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	واپس FAIL;

    اگر (dict_has_کلید(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    ورنہ
    {
	semsg(_(e_dictionary_کلید_str_required), "sort");
	واپس FAIL;
    }

    اگر (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_ترتیب_دو_NONE;
    ورنہ اگر (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_ترتیب_دو_BYTE;
    ورنہ اگر (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_ترتیب_دو_IC;
    ورنہ اگر (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_ترتیب_دو_COLLATE;
    واپس OK;
}

/*
 * "readdir()" function
 */
    خالی
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*اظہاریہ;
    صحیح		ret;
    char_u	*راستہ;
    char_u	*p;
    garray_T	ga;
    صحیح		i;
    صحیح		ترتیب_دو = READDIR_ترتیب_دو_BYTE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;
    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	واپس;

    راستہ = tv_get_string(&argvars[0]);
    اظہاریہ = &argvars[1];

    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &ترتیب_دو) == FAIL)
	واپس;

    ret = readdir_core(&ga, راستہ, FALSE, (خالی *)اظہاریہ,
	    (اظہاریہ->v_type == VAR_UNKNOWN) ? تہی : readdir_checkitem, ترتیب_دو);
    اگر (ret == OK)
    {
	برائے (i = 0; i < ga.ga_len; i++)
	{
	    p = ((char_u **)ga.ga_ڈیٹا)[i];
	    list_append_string(rettv->vval.v_list, p, -1);
	}
    }
    ga_clear_لڑیاں(&ga);
}

/*
 * Evaluate "expr" (= "context") for readdirex().
 */
    ساکن صحیح
readdirex_checkitem(خالی *context, خالی *شے)
{
    dict_T	*dict = (dict_T*)شے;

    واپس checkitem_common(context, تہی, dict);
}

/*
 * "readdirex()" function
 */
    خالی
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*اظہاریہ;
    صحیح		ret;
    char_u	*راستہ;
    garray_T	ga;
    صحیح		i;
    صحیح		ترتیب_دو = READDIR_ترتیب_دو_BYTE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;
    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	واپس;

    راستہ = tv_get_string(&argvars[0]);
    اظہاریہ = &argvars[1];

    اگر (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &ترتیب_دو) == FAIL)
	واپس;

    ret = readdir_core(&ga, راستہ, TRUE, (خالی *)اظہاریہ,
	    (اظہاریہ->v_type == VAR_UNKNOWN) ? تہی : readdirex_checkitem, ترتیب_دو);
    اگر (ret == OK)
    {
	برائے (i = 0; i < ga.ga_len; i++)
	{
	    dict_T  *dict = ((dict_T**)ga.ga_ڈیٹا)[i];
	    list_append_dict(rettv->vval.v_list, dict);
	    dict_unref(dict);
	}
    }
    ga_clear(&ga);
}

/*
 * "readfile()" function
 */
    ساکن خالی
read_file_or_blob(typval_T *argvars, typval_T *rettv, صحیح always_blob)
{
    صحیح		ثنائی = FALSE;
    صحیح		blob = always_blob;
    صحیح		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    صحیح		io_سائز = حجم(buf);
    صحیح		readlen;		// size of last fread()
    char_u	*پچھلا	 = تہی;	// previously read bytes, if any
    طویل	prevlen  = 0;		// length of data in prev
    طویل	prevsize = 0;		// size of prev buffer
    طویل	maxline  = MAXLNUM;
    طویل	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*آغاز;			// start of current line
    off_T	انحراف = 0;
    off_T	سائز = -1;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (always_blob)
	{
	    انحراف = (off_T)tv_get_نمبر(&argvars[1]);
	    اگر (argvars[2].v_type != VAR_UNKNOWN)
		سائز = (off_T)tv_get_نمبر(&argvars[2]);
	}
	ورنہ
	{
	    اگر (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		ثنائی = TRUE;
	    اگر (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    اگر (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (طویل)tv_get_نمبر(&argvars[2]);
	}
    }

    اگر ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	واپس;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    اگر (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	واپس;
    }
    اگر (*fname == NUL || (fd = mch_fopen((حرف *)fname, READBIN)) == تہی)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	واپس;
    }

    اگر (blob)
    {
	اگر (read_blob(fd, rettv, انحراف, سائز) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	واپس;
    }

    جب (cnt < maxline || maxline < 0)
    {
	readlen = (صحیح)fread(buf, 1, io_سائز, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	برائے (p = buf, آغاز = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || ثنائی));
		++p)
	{
	    اگر (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= تہی;
		long_u	    len = p - آغاز;

		// Finished a line.  Remove CRs before NL.
		اگر (readlen > 0 && !ثنائی)
		{
		    جب (len > 0 && آغاز[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    اگر (len == 0)
			جب (prevlen > 0 && پچھلا[prevlen - 1] == '\r')
			    --prevlen;
		}
		اگر (prevlen == 0)
		    s = vim_strnsave(آغاز, len);
		ورنہ
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    اگر ((s = vim_realloc(پچھلا, prevlen + len + 1)) != تہی)
		    {
			mch_memmove(s + prevlen, آغاز, len);
			s[prevlen + len] = NUL;
			پچھلا = تہی; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		اگر (s == تہی)
		{
		    do_outofmem_پیغام((long_u) prevlen + len + 1);
		    failed = TRUE;
		    توڑو;
		}

		اگر ((li = listitem_alloc()) == تہی)
		{
		    vim_free(s);
		    failed = TRUE;
		    توڑو;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_تالا = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		آغاز = p + 1; // step over newline
		اگر ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    توڑو;
	    }
	    ورنہ اگر (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    ورنہ اگر (*p == 0xbf && enc_utf8 && !ثنائی)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? پچھلا[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? پچھلا[prevlen - 1]
			  : prevlen >= 2 ? پچھلا[prevlen - 2] : NUL;

		اگر (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    اگر (آغاز == dest)
			آغاز = p + 1;
		    ورنہ
		    {
			// have to shuffle buf to close gap
			صحیح adjust_prevlen = 0;

			اگر (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (صحیح)(buf - dest);
			    dest = buf;
			}
			اگر (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	اگر (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    توڑو;
	اگر (آغاز < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    اگر (p - آغاز + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		اگر (prevsize == 0)
		    prevsize = (طویل)(p - آغاز);
		ورنہ
		{
		    طویل grow50pc = (prevsize * 3) / 2;
		    طویل growmin  = (طویل)((p - آغاز) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(پچھلا, prevsize);
		اگر (newprev == تہی)
		{
		    do_outofmem_پیغام((long_u)prevsize);
		    failed = TRUE;
		    توڑو;
		}
		پچھلا = newprev;
	    }
	    // Add the line part to end of "prev".
	    mch_memmove(پچھلا + prevlen, آغاز, p - آغاز);
	    prevlen += (طویل)(p - آغاز);
	}
    } // while

    // For a negative line count use only the lines at the end of the file,
    // free the rest.
    اگر (!failed && maxline < 0)
	جب (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_پہلا);
	    --cnt;
	}

    اگر (failed)
    {
	// an empty list is returned on error
	list_free(rettv->vval.v_list);
	rettv_list_alloc(rettv);
    }

    vim_free(پچھلا);
    fclose(fd);
}

/*
 * "readblob()" function
 */
    خالی
f_readblob(typval_T *argvars, typval_T *rettv)
{
    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 2) == FAIL)))
	واپس;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    خالی
f_readfile(typval_T *argvars, typval_T *rettv)
{
    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 2) == FAIL)))
	واپس;

    read_file_or_blob(argvars, rettv, FALSE);
}

/*
 * "resolve()" function
 */
    خالی
f_resolve(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;
# ifdef HAVE_READLINK
    char_u	*buf = تہی;
# endif

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = تہی;

	v = mch_resolve_راستہ(p, TRUE);
	اگر (v != تہی)
	    rettv->vval.v_string = v;
	ورنہ
	    rettv->vval.v_string = vim_strsave(p);
    }
# ورنہ
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	صحیح	len;
	char_u	*remain = تہی;
	char_u	*q;
	صحیح	is_relative_مقصد_current = FALSE;
	صحیح	has_trailing_pathsep = FALSE;
	صحیح	حد_بندی = 100;

	p = vim_strsave(p);
	اگر (p == تہی)
	    جاؤ fail;
	اگر (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_مقصد_current = TRUE;

	len = STRLEN(p);
	اگر (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	اگر (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	اگر (buf == تہی)
	{
	    vim_free(p);
	    vim_free(remain);
	    جاؤ fail;
	}

	برائے (;;)
	{
	    برائے (;;)
	    {
		len = readlink((حرف *)p, (حرف *)buf, MAXPATHL);
		اگر (len <= 0)
		    توڑو;
		buf[len] = NUL;

		اگر (حد_بندی-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_چکر));
		    rettv->vval.v_string = تہی;
		    جاؤ fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		اگر (remain == تہی && has_trailing_pathsep)
		    جوڑو_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		اگر (*q != NUL)
		{
		    اگر (remain == تہی)
			remain = vim_strsave(q - 1);
		    ورنہ
		    {
			cpy = concat_str(q - 1, remain);
			اگر (cpy != تہی)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		اگر (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		اگر (q > p && !mch_is_بھرا_Name(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    اگر (cpy != تہی)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		ورنہ
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    اگر (remain == تہی)
		توڑو;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    اگر (cpy != تہی)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    اگر (*q != NUL)
		STRMOVE(remain, q - 1);
	    ورنہ
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	اگر (!vim_ispathsep(*p))
	{
	    اگر (is_relative_مقصد_current
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
		اگر (cpy != تہی)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    ورنہ اگر (!is_relative_مقصد_current)
	    {
		// Strip leading "./".
		q = p;
		جب (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		اگر (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	اگر (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    اگر (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  ورنہ
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
    خالی
f_tempname(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    ساکن صحیح	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    کرو
    {
	اگر (x == 'Z')
	    x = '0';
	ورنہ اگر (x == '9')
	    x = 'A';
	ورنہ
	    ++x;
    } جب (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    خالی
f_writefile(typval_T *argvars, typval_T *rettv)
{
    صحیح		ثنائی = FALSE;
    صحیح		append = FALSE;
    صحیح		defer = FALSE;
# ifdef HAVE_FSYNC
    صحیح		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    صحیح		ret = 0;
    listitem_T	*li;
    list_T	*list = تہی;
    blob_T	*blob = تہی;

    rettv->vval.v_نمبر = -1;
    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	واپس;

    اگر (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	اگر (list == تہی)
	    واپس;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_اشیاء(list, li)
	    اگر (tv_get_string_chk(&li->li_tv) == تہی)
		واپس;
    }
    ورنہ اگر (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	اگر (blob == تہی)
	    واپس;
    }
    ورنہ
    {
	semsg(_(e_invalid_دلیل_str),
		_("writefile() first argument must be a List or a Blob"));
	واپس;
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	اگر (arg2 == تہی)
	    واپس;
	اگر (vim_strchr(arg2, 'b') != تہی)
	    ثنائی = TRUE;
	اگر (vim_strchr(arg2, 'a') != تہی)
	    append = TRUE;
	اگر (vim_strchr(arg2, 'D') != تہی)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	اگر (vim_strchr(arg2, 's') != تہی)
	    do_fsync = TRUE;
	ورنہ اگر (vim_strchr(arg2, 'S') != تہی)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    اگر (fname == تہی)
	واپس;

    اگر (defer && !can_جوڑو_defer())
	واپس;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    اگر (*fname == NUL || (fd = mch_fopen((حرف *)fname,
				      append ? APPENDBIN : WRITEBIN)) == تہی)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    ورنہ
    {
	اگر (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_تالا = 0;
	    tv.vval.v_string = بھرا_Name_save(fname, FALSE);
	    اگر (tv.vval.v_string == تہی
		    || جوڑو_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(خالی)mch_remove(fname);
	    }
	}

	اگر (ret == 0)
	{
	    اگر (blob)
	    {
		اگر (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    ورنہ
	    {
		اگر (write_list(fd, list, ثنائی) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    اگر (ret == 0 && do_fsync)
		// Ignore the error, the user wouldn't know what to do about
		// it.  May happen for a device.
		vim_ignored = vim_fsync(fileno(fd));
# endif
	    fclose(fd);
	}
    }

    rettv->vval.v_نمبر = ret;
}

#اگر_ختم // FEAT_EVAL

#اگر defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    صحیح		flags,		// BROWSE_SAVE and BROWSE_DIR
    char_u	*title,		// title for the window
    char_u	*dflt,		// default file name (may include directory)
    char_u	*ext,		// extension added
    char_u	*initdir,	// initial directory, NULL for current dir or
				// when using path from "dflt"
    char_u	*filter,	// file name filter
    buf_T	*buf)		// buffer to read/write for
{
    char_u		*fname;
    ساکن char_u	*آخری_dir = تہی;    // last used directory
    char_u		*tofree = تہی;
    صحیح			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    اگر (title == تہی || *title == NUL)
    {
	اگر (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	ورنہ اگر (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	ورنہ
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    اگر ((initdir == تہی || *initdir == NUL) && dflt != تہی && *dflt != NUL)
    {
	اگر (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = تہی;
	}
	ورنہ اگر (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    اگر (tofree != تہی)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    اگر (initdir == تہی || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	اگر (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	ورنہ اگر (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != تہی && buf->b_ffname != تہی)
	{
	    اگر (dflt == تہی || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    اگر (tofree != تہی)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	ورنہ اگر (*p_bsdir == 'l')
	    initdir = آخری_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    اگر (gui.اندر_use)		// when this changes, also adjust f_has()!
    {
	اگر (filter == تہی
#  ifdef FEAT_EVAL
		&& (filter = get_var_قدر((char_u *)"b:browsefilter")) == تہی
		&& (filter = get_var_قدر((char_u *)"g:browsefilter")) == تہی
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	اگر (flags & BROWSE_DIR)
	{
#  اگر defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  ورنہ
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  اگر !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    اگر (fname != تہی && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *دم = gettail_sep(fname);

		اگر (دم == fname)
		    *دم++ = '.';	// use current dir
		*دم = NUL;
	    }
#  endif
	}
	ورنہ
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    ورنہ
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_اندر_console_منوال));
	fname = تہی;
    }

    // keep the directory for next time
    اگر (fname != تہی)
    {
	vim_free(آخری_dir);
	آخری_dir = vim_strsave(fname);
	اگر (آخری_dir != تہی && !(flags & BROWSE_DIR))
	{
	    *gettail(آخری_dir) = NUL;
	    اگر (*آخری_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(آخری_dir);
		آخری_dir = alloc(MAXPATHL);
		اگر (آخری_dir != تہی)
		    mch_dirname(آخری_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    واپس fname;
}
#اگر_ختم

#اگر defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    خالی
f_browse(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    صحیح		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    صحیح		error = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	واپس;

    save = (صحیح)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    اگر (error || title == تہی || initdir == تہی || defname == تہی)
	rettv->vval.v_string = تہی;
    ورنہ
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, تہی, initdir, تہی, curbuf);
# ورنہ
    rettv->vval.v_string = تہی;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "browsedir(title, initdir)" function
 */
    خالی
f_browsedir(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    char_u	*title;
    char_u	*initdir;
    char_u	buf[NUMBUFLEN];

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    اگر (title == تہی || initdir == تہی)
	rettv->vval.v_string = تہی;
    ورنہ
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, تہی, تہی, initdir, تہی, curbuf);
# ورنہ
    rettv->vval.v_string = تہی;
# endif
    rettv->v_type = VAR_STRING;
}

/*
 * "filecopy()" function
 */
    خالی
f_filecopy(typval_T *argvars, typval_T *rettv)
{
    char_u	*from;
    stat_T	st;

    rettv->vval.v_نمبر = FALSE;

    اگر (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	واپس;

    from = tv_get_string(&argvars[0]);

    اگر (mch_lstat((حرف *)from, &st) >= 0
	&& (S_ISREG(st.st_منوال) || S_ISLNK(st.st_منوال)))
	rettv->vval.v_نمبر = vim_copyfile(
	    tv_get_string(&argvars[0]),
	    tv_get_string(&argvars[1])) == OK ? TRUE : FALSE;
}

#اگر_ختم // FEAT_EVAL

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
    صحیح		dstlen,	// maximum length of the result
    صحیح		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_آغاز;

    اگر (src == تہی)
    {
	*dst = NUL;
	واپس 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    اگر (buf != تہی && buf->b_help)
	واپس vim_snprintf_safelen((حرف *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    اگر (homedir != تہی)
	dirlen = STRLEN(homedir);

#اگر_تعریف_شدہ VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#ورنہ
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    اگر (homedir_env == تہی)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#اگر_ختم
    // Empty is the same as not set.
    اگر (homedir_env != تہی && *homedir_env == NUL)
	homedir_env = تہی;

    اگر (homedir_env != تہی && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = تہی;

	flen = STRLEN(homedir_env);
	(خالی)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	اگر (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    اگر (homedir_env != تہی)
	envlen = STRLEN(homedir_env);

    اگر (!one)
	src = skipwhite(src);
    dst_آغاز = dst;		// remember the start
    جب (*src && dstlen > 0)
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
	برائے (;;)
	{
	    اگر (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		اگر (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		توڑو;
	    }
	    اگر (p == homedir_env)
		توڑو;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	جب (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	جب ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    اگر (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    واپس (size_t)(dst - dst_آغاز);
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
    غیرعلامتی	len;

    len = 3;			// space for "~/" and trailing NUL
    اگر (src != تہی)		// just in case
	len += (غیرعلامتی)STRLEN(src);
    dst = alloc(len);
    اگر (dst != تہی)
	home_replace(buf, src, dst, len, TRUE);
    واپس dst;
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
    صحیح
fullpathcmp(
    char_u *s1,
    char_u *s2,
    صحیح	    checkname,		// when both don't exist, check file names
    صحیح	    expandenv)
{
#اگر_تعریف_شدہ UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    صحیح		    r1, r2;

    اگر (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    ورنہ
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((حرف *)exp1, &st1);
    r2 = mch_stat((حرف *)s2, &st2);
    اگر (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	اگر (checkname)
	{
	    اگر (fnamecmp(exp1, s2) == 0)
		واپس FPC_SAMEX;
	    r1 = vim_بھرا_Name(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_بھرا_Name(s2, full2, MAXPATHL, FALSE);
	    اگر (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		واپس FPC_SAMEX;
	}
	واپس FPC_NOTX;
    }
    اگر (r1 != 0 || r2 != 0)
	واپس FPC_DIFFX;
    اگر (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	واپس FPC_SAME;
    واپس FPC_DIFF;
#ورنہ
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    صحیح	    retval = FPC_DIFF;
    صحیح	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    اگر ((exp1 = alloc(MAXPATHL * 3)) != تہی)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	اگر (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	ورنہ
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_بھرا_Name(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_بھرا_Name(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	اگر (r1 != OK && r2 != OK)
	{
	    اگر (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    ورنہ
		retval = FPC_NOTX;
	}
	ورنہ اگر (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	ورنہ اگر (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	ورنہ
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    واپس retval;
#اگر_ختم
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

    اگر (fname == تہی)
	واپس (char_u *)"";
    برائے (p1 = p2 = get_past_سر(fname); *p2; )	// find last part of path
    {
	اگر (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    واپس p1;
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

    p = get_past_سر(fname);	// don't remove the '/' from "c:/file"
    t = gettail(fname);
    جب (t > p && after_pathsep(fname, t))
	--t;
#اگر_تعریف_شدہ VMS
    // path separator is part of the path
    ++t;
#اگر_ختم
    واپس t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    جب (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    اگر (*fname)
	++fname;
    واپس fname;
}

/*
 * Get a pointer to one character past the head of a path name.
 * Unix: after "/"; DOS: after "c:\"; Amiga: after "disk:/"; Mac: no head.
 * If there is no head, path is returned.
 */
    char_u *
get_past_سر(char_u *راستہ)
{
    char_u  *retval;

#اگر defined(MSWIN)
    // may skip "c:"
    اگر (محفوظ_isalpha(راستہ[0]) && راستہ[1] == ':')
	retval = راستہ + 2;
    ورنہ
	retval = راستہ;
#ورنہ
# اگر defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(راستہ, ':');
    اگر (retval == تہی)
	retval = راستہ;
# ورنہ	// Unix
    retval = راستہ;
# endif
#اگر_ختم

    جب (vim_ispathsep(*retval))
	++retval;

    واپس retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    صحیح
vim_ispathsep(صحیح c)
{
#اگر_تعریف_شدہ UNIX
    واپس (c == '/');	    // UNIX has ':' inside file names
#ورنہ
# ifdef BACKSLASH_اندر_FILENAME
    واپس (c == ':' || c == '/' || c == '\\');
# ورنہ
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    واپس (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  ورنہ
    واپس (c == ':' || c == '/');
#  endif // VMS
# endif
#اگر_ختم
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    صحیح
vim_ispathsep_nocolon(صحیح c)
{
    واپس vim_ispathsep(c)
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	&& c != ':'
#اگر_ختم
	;
}

/*
 * Return TRUE if the directory of "fname" exists, FALSE otherwise.
 * Also returns TRUE if there is no directory name.
 * "fname" must be writable!.
 */
    صحیح
dir_کا_file_exists(char_u *fname)
{
    char_u	*p;
    صحیح		c;
    صحیح		retval;

    p = gettail_sep(fname);
    اگر (p == fname)
	واپس TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    واپس retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    صحیح
vim_fnamecmp(char_u *x, char_u *y)
{
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    واپس vim_fnamencmp(x, y, MAXPATHL);
#ورنہ
    اگر (p_fic)
	واپس MB_STRICMP(x, y);
    واپس STRCMP(x, y);
#اگر_ختم
}

    صحیح
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    صحیح		cx = NUL;
    صحیح		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    برائے (صحیح تبادلہ = 0, drive = NUL; تبادلہ < 2; ++تبادلہ)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	اگر ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    اگر (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		توڑو;
	    }
	    ورنہ // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    جب (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	اگر (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    توڑو;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    اگر (len == 0)
	واپس 0;
    واپس (cx - cy);
#ورنہ
    اگر (p_fic)
	واپس MB_STRNICMP(x, y, len);
    واپس STRNCMP(x, y, len);
#اگر_ختم
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, صحیح sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? حجم(PATHSEP) : 0) + fname2len + 1);
    اگر (ret->string == تہی)
	ret->لمبائی = 0;
    ورنہ
    {
	STRCPY(ret->string, fname1);
	ret->لمبائی = fname1len;
	اگر (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->لمبائی))
	{
	    STRCPY(ret->string + ret->لمبائی, PATHSEPSTR);
	    ret->لمبائی += حجم(PATHSEP);
	}
	STRCPY(ret->string + ret->لمبائی, fname2);
	ret->لمبائی += fname2len;
    }

    واپس ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    خالی
جوڑو_pathsep(char_u *p)
{
    size_t  plen;

    اگر (*p == NUL)
	واپس;

    plen = STRLEN(p);
    اگر (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
بھرا_Name_save(
    char_u	*fname,
    صحیح		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = تہی;

    اگر (fname == تہی)
	واپس تہی;

    buf = alloc(MAXPATHL);
    اگر (buf == تہی)
	واپس تہی;

    اگر (vim_بھرا_Name(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    ورنہ
	new_fname = vim_strsave(fname);
    vim_free(buf);
    واپس new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    صحیح
vim_fexists(char_u *fname)
{
    stat_T st;

    اگر (mch_stat((حرف *)fname, &st))
	واپس FALSE;
    واپس TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    صحیح
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    صحیح		  *عدد_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    صحیح		   flags)	// EW_DIR, etc.
{
    صحیح		ret = FAIL;
    char_u	*eval_pat = تہی;
    char_u	*exp_pat = *pat;
    حرف	*ignored_پیغام;
    size_t	usedlen;
    صحیح		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    صحیح		ستارہ_follows = FALSE;

    اگر (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       تہی, &ignored_پیغام, تہی, TRUE);
	--emsg_off;
	اگر (eval_pat != تہی)
	{
	    ستارہ_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    اگر (exp_pat != تہی)
	ret = expand_wildcards(1, &exp_pat, عدد_file, file, flags);

    اگر (eval_pat != تہی)
    {
	اگر (*عدد_file == 0 && is_cur_alt_file && ستارہ_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    اگر (*file != تہی)
	    {
		**file = eval_pat;
		eval_pat = تہی;
		*عدد_file = 1;
		ret = OK;
	    }
	}
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    واپس ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    صحیح
expand_wildcards(
    صحیح		   عدد_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    صحیح		  *عدد_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    صحیح		   flags)	// EW_DIR, etc.
{
    صحیح		retval;
    صحیح		i, j;
    char_u	*p;
    صحیح		non_suf_مطابقت;	// number without matching suffix

    retval = gen_expand_wildcards(عدد_pat, pat, عدد_files, files, flags);

    // When keeping all matches, return here
    اگر ((flags & EW_KEEPALL) || retval == FAIL)
	واپس retval;

    /*
     * Remove names that match 'wildignore'.
     */
    اگر (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	برائے (i = 0; i < *عدد_files; ++i)
	{
	    ffname = بھرا_Name_save((*files)[i], FALSE);
	    اگر (ffname == تہی)		// out of memory
		توڑو;
#اگر_تعریف_شدہ VMS
	    vms_remove_نسخہ(ffname);
#اگر_ختم
	    اگر (مطابقت_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		برائے (j = i; j + 1 < *عدد_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*عدد_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	اگر (*عدد_files == 0)
	{
	    VIM_CLEAR(*files);
	    واپس FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    اگر (*عدد_files > 1 && !got_int)
    {
	non_suf_مطابقت = 0;
	برائے (i = 0; i < *عدد_files; ++i)
	{
	    اگر (!مطابقت_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		برائے (j = i; j > non_suf_مطابقت; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_مطابقت++] = p;
	    }
	}
    }

    واپس retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    صحیح
مطابقت_suffix(char_u *fname)
{
    صحیح		fnamelen, setsuflen;
    char_u	*setsuf;
#تعریف_کرو MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (صحیح)STRLEN(fname);
    setsuflen = 0;
    برائے (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	اگر (setsuflen == 0)
	{
	    char_u *دم = gettail(fname);

	    // empty entry: match name without a '.'
	    اگر (vim_strchr(دم, '.') == تہی)
	    {
		setsuflen = 1;
		توڑو;
	    }
	}
	ورنہ
	{
	    اگر (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		توڑو;
	    setsuflen = 0;
	}
    }
    واپس (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    ساکن صحیح
vim_backtick(char_u *p)
{
    واپس (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    ساکن صحیح
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    صحیح		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*بفر;
    صحیح		cnt = 0;
    صحیح		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    اگر (cmd == تہی)
	واپس -1;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (*cmd == '=')	    // `={expr}`: Expand expression
	بفر = eval_مقصد_string(cmd + 1, TRUE, FALSE);
    ورنہ
#اگر_ختم
	بفر = get_cmd_output(cmd, تہی,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, تہی);
    vim_free(cmd);
    اگر (بفر == تہی)
	واپس -1;

    cmd = بفر;
    جب (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	جب (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	اگر (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	جب (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(بفر);
    واپس cnt;
}

#اگر defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    ساکن صحیح
pstrcmp(ثابت خالی *a, ثابت خالی *b)
{
    واپس (pathcmp(*(حرف **)a, *(حرف **)b, -1));
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
    صحیح
dos_expandpath(
    garray_T	*gap,
    char_u	*راستہ,
    size_t	wildoff,
    صحیح		flags,		// EW_* flags
    صحیح		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*راستہ_انتہا;
    char_u	*p, *s, *e;
    صحیح		آغاز_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    صحیح		starts_with_dot;
    صحیح		matches;
    size_t	buflen;
    size_t	len;
    صحیح		starstar = FALSE;
    ساکن صحیح	stardepth = 0;	    // depth for "**" expansion
    HANDLE		h_ڈھونڈو = INVALID_HANDLE_قدر;
    WIN32_ڈھونڈو_DATAW    wfb;
    WCHAR		*wn = تہی;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    صحیح			ok;
    char_u		*p_alt;
    صحیح			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    اگر (stardepth > 0)
    {
	ui_breakcheck();
	اگر (got_int)
	    واپس 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(راستہ) + MAXPATHL;
    buf = alloc(buflen);
    اگر (buf == تہی)
	واپس 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = تہی;
    راستہ_انتہا = راستہ;
    جب (*راستہ_انتہا != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	اگر (راستہ_انتہا >= راستہ + wildoff && rem_backslash(راستہ_انتہا))
	    *p++ = *راستہ_انتہا++;
	ورنہ اگر (*راستہ_انتہا == '\\' || *راستہ_انتہا == ':' || *راستہ_انتہا == '/')
	{
	    اگر (e != تہی)
		توڑو;
	    s = p + 1;
	}
	ورنہ اگر (راستہ_انتہا >= راستہ + wildoff
			 && vim_strchr((char_u *)"*?[~", *راستہ_انتہا) != تہی)
	    e = p;
	اگر (has_mbyte)
	{
	    صحیح charlen = (*mb_ptr2len)(راستہ_انتہا);

	    STRNCPY(p, راستہ_انتہا, (size_t)charlen);
	    p += charlen;
	    راستہ_انتہا += charlen;
	}
	ورنہ
	    *p++ = *راستہ_انتہا++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    برائے (p = buf + wildoff; p < s; ++p)
	اگر (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    برائے (p = s; p < e; ++p)
	اگر (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_مقصد_reg_pat(s, e, تہی, FALSE);
    اگر (pat == تہی)
    {
	vim_free(buf);
	واپس 0;
    }

    // compile the regexp into a program
    اگر (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    اگر (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    اگر (regmatch.regprog == تہی && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	واپس 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != تہی;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    اگر (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *راستہ_انتہا == '/')
    {
	vim_snprintf((حرف *)s, buflen - len, "%s", راستہ_انتہا + 1);
	++stardepth;
	(خالی)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((حرف *)s, buflen - len, "*.*");
    wn = enc_مقصد_utf16(buf, تہی);
    اگر (wn != تہی)
	h_ڈھونڈو = ڈھونڈو_پہلا_File_W(wn, &wfb);
    ok = (h_ڈھونڈو != INVALID_HANDLE_قدر);

    جب (ok)
    {
	p = utf16_مقصد_enc(wfb.cFileName, تہی);   // p is allocated here

	اگر (p == تہی)
	    توڑو;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	اگر (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = تہی;
	ورنہ
	    p_alt = utf16_مقصد_enc(wfb.cAlternateFileName, تہی);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	اگر ((p[0] != '.' || starts_with_dot
			 || ((flags & EW_DODOT)
			     && p[1] != NUL && (p[1] != '.' || p[2] != NUL)))
		&& (matchname == تہی
		  || (regmatch.regprog != تہی
		      && (vim_regexec(&regmatch, p, (colnr_T)0)
			 || (p_alt != تہی
				&& vim_regexec(&regmatch, p_alt, (colnr_T)0))))
		  || ((flags & EW_NOTWILD)
		     && fnamencmp(راستہ + len, p, e - s) == 0)))
	{
	    len += vim_snprintf((حرف *)s, buflen - len, "%s", p);
	    اگر (len + 1 < buflen)
	    {
		اگر (starstar && stardepth < 100
			  && (wfb.dw_File_اوصاف & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((حرف *)buf + len, buflen - len,
							    "/**%s", راستہ_انتہا);
		    ++stardepth;
		    (خالی)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((حرف *)buf + len, buflen - len, "%s", راستہ_انتہا);
		اگر (mch_has_exp_wildcard(راستہ_انتہا))
		{
		    اگر (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(خالی)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ورنہ
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    اگر (*راستہ_انتہا != 0)
			backslash_halve(buf + len + 1);
		    // addfile() checks existence; do not stat the same path twice.
		    // Even with EW_NOTFOUND, a literal suffix must exist.
		    addfile(gap, buf, flags & ~EW_NOTFOUND);
		}
	    }
	}

	vim_free(p_alt);
	vim_free(p);
	ok = ڈھونڈو_اگلا_File_W(h_ڈھونڈو, &wfb);
    }

    ڈھونڈو_Close(h_ڈھونڈو);
    vim_free(wn);
    vim_free(buf);
    vim_regfree(regmatch.regprog);
    vim_free(matchname);

    matches = gap->ga_len - آغاز_len;
    اگر (matches > 0)
	qsort(((char_u **)gap->ga_ڈیٹا) + آغاز_len, (size_t)matches,
						   حجم(char_u *), pstrcmp);
    واپس matches;
}
#اگر_ختم // MSWIN

#اگر (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    ساکن صحیح
pstrcmp(ثابت خالی *a, ثابت خالی *b)
{
    واپس (pathcmp(*(حرف **)a, *(حرف **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    صحیح
unix_expandpath(
    garray_T	*gap,
    char_u	*راستہ,
    size_t	wildoff,
    صحیح		flags,		// EW_* flags
    صحیح		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*راستہ_انتہا;
    char_u	*p, *s, *e;
    صحیح		آغاز_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    صحیح		starts_with_dot;
    صحیح		matches;
    size_t	buflen;
    size_t	len;
    صحیح		starstar = FALSE;
    ساکن صحیح	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    ساخت dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    اگر (stardepth > 0)
    {
	ui_breakcheck();
	اگر (got_int)
	    واپس 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(راستہ) + MAXPATHL;
    buf = alloc(buflen);
    اگر (buf == تہی)
	واپس 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = تہی;
    راستہ_انتہا = راستہ;
    جب (*راستہ_انتہا != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	اگر (راستہ_انتہا >= راستہ + wildoff && rem_backslash(راستہ_انتہا))
	    *p++ = *راستہ_انتہا++;
	ورنہ اگر (*راستہ_انتہا == '/')
	{
	    اگر (e != تہی)
		توڑو;
	    s = p + 1;
	}
	ورنہ اگر (راستہ_انتہا >= راستہ + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *راستہ_انتہا) != تہی
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(راستہ_انتہا)))))
	    e = p;
	اگر (has_mbyte)
	{
	    صحیح charlen = (*mb_ptr2len)(راستہ_انتہا);

	    STRNCPY(p, راستہ_انتہا, (size_t)charlen);
	    p += charlen;
	    راستہ_انتہا += charlen;
	}
	ورنہ
	    *p++ = *راستہ_انتہا++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    برائے (p = buf + wildoff; p < s; ++p)
	اگر (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    برائے (p = s; p < e; ++p)
	اگر (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_مقصد_reg_pat(s, e, تہی, FALSE);
    اگر (pat == تہی)
    {
	vim_free(buf);
	واپس 0;
    }

    // compile the regexp into a program
    اگر (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    ورنہ
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    اگر (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    اگر (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    اگر (regmatch.regprog == تہی && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	واپس 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    اگر (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *راستہ_انتہا == '/')
    {
	vim_snprintf((حرف *)s, buflen - len, "%s", راستہ_انتہا + 1);
	++stardepth;
	(خالی)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (حرف *)buf);

    // Find all matching entries
    اگر (dirp != تہی)
    {
	جب (!got_int)
	{
	    dp = readdir(dirp);
	    اگر (dp == تہی)
		توڑو;
	    len = (size_t)(s - buf);
	    اگر ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != تہی && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(راستہ + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((حرف *)s, buflen - len, "%s", dp->d_name);
		اگر (len + 1 >= buflen)
		    جاری;

		اگر (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((حرف *)buf + len, buflen - len,
							    "/**%s", راستہ_انتہا);
		    ++stardepth;
		    (خالی)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((حرف *)buf + len, buflen - len, "%s", راستہ_انتہا);
		اگر (mch_has_exp_wildcard(راستہ_انتہا)) // handle more wildcards
		{
		    اگر (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(خالی)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		ورنہ
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    اگر (*راستہ_انتہا != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    اگر ((flags & EW_ALLLINKS) ? mch_lstat((حرف *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_راستہ(buf, precomp_len, &precomp_len);

			اگر (precomp_buf)
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
    matches = gap->ga_len - آغاز_len;
    اگر (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_ڈیٹا) + آغاز_len, matches,
						   حجم(char_u *), pstrcmp);
    واپس matches;
}
#اگر_ختم

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    ساکن صحیح
has_env_var(char_u *p)
{
    برائے ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	ورنہ اگر (vim_strchr((char_u *)
#اگر defined(MSWIN)
				    "$%"
#ورنہ
				    "$"
#اگر_ختم
					, *p) != تہی)
	    واپس TRUE;
    }
    واپس FALSE;
}

#اگر_تعریف_شدہ SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    ساکن صحیح
has_special_wildchar(char_u *p)
{
    برائے ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	اگر (*p == '\r' || *p == '\n')
	    توڑو;
	// Allow for escaping.
	اگر (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	ورنہ اگر (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != تہی)
	{
	    // A { must be followed by a matching }.
	    اگر (*p == '{' && vim_strchr(p, '}') == تہی)
		جاری;
	    // A quote and backtick must be followed by another one.
	    اگر ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == تہی)
		جاری;
	    واپس TRUE;
	}
    }
    واپس FALSE;
}
#اگر_ختم

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
    صحیح
gen_expand_wildcards(
    صحیح		عدد_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    صحیح		*عدد_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    صحیح		flags)		// EW_* flags
{
    صحیح			i;
    garray_T		ga;
    char_u		*p;
    ساکن صحیح		recursive = FALSE;
    صحیح			جوڑو_pat;
    صحیح			retval = OK;
    صحیح			did_expand_اندر_راستہ = FALSE;
    char_u		*راستہ_option = *curbuf->b_p_راستہ == NUL ?
					p_راستہ : curbuf->b_p_راستہ;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    اگر (recursive)
#اگر_تعریف_شدہ SPECIAL_WILDCHAR
	واپس mch_expand_wildcards(عدد_pat, pat, عدد_file, file, flags);
#ورنہ
	واپس FAIL;
#اگر_ختم

#اگر_تعریف_شدہ SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    برائے (i = 0; i < عدد_pat; i++)
    {
	اگر (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    واپس mch_expand_wildcards(عدد_pat, pat, عدد_file, file, flags);
    }
#اگر_ختم

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, حجم(char_u *), 30);

    برائے (i = 0; i < عدد_pat && !got_int; ++i)
    {
	جوڑو_pat = -1;
	p = pat[i];

	اگر (vim_backtick(p))
	{
	    جوڑو_pat = expand_backtick(&ga, p, flags);
	    اگر (جوڑو_pat == -1)
		retval = FAIL;
	}
	ورنہ
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    اگر ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)راستہ_ESC_WILDCARDS);
		اگر (p == تہی)
		    p = pat[i];
#اگر_تعریف_شدہ UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		ورنہ اگر (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_لڑیاں(&ga);
		    i = mch_expand_wildcards(عدد_pat, pat, عدد_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    واپس i;
		}
#اگر_ختم
	    }

	    /*
	     * If there are wildcards or case-insensitive expansion is
	     * required: Expand file names and add each match to the list.  If
	     * there is no match, and EW_NOTFOUND is given, add the pattern.
	     * Otherwise: Add the file name if it exists or when EW_NOTFOUND is
	     * given.
	     */
	    اگر (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		اگر ((flags & (EW_راستہ | EW_CDPATH))
			&& !mch_is_بھرا_Name(p)
			&& !(p[0] == '.'
			    && (vim_ispathsep(p[1])
				|| (p[1] == '.' && vim_ispathsep(p[2]))))
		   )
		{
		    // :find completion where 'path' is used.
		    // Recursiveness is OK here.
		    recursive = FALSE;
		    جوڑو_pat = expand_اندر_راستہ(&ga, p, flags);
		    recursive = TRUE;
		    did_expand_اندر_راستہ = TRUE;
		}
		ورنہ
		    جوڑو_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	اگر (جوڑو_pat == -1 || (جوڑو_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    اگر (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    ورنہ
		addfile(&ga, t, flags);

	    اگر (t != p)
		vim_free(t);
	}

	اگر (did_expand_اندر_راستہ && ga.ga_len > 0 && (flags & (EW_راستہ | EW_CDPATH)))
	    uniquefy_paths(&ga, p, راستہ_option);
	اگر (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    اگر (retval == FAIL)
	ga_clear_لڑیاں(&ga);

    *عدد_file = ga.ga_len;
    *file = (ga.ga_ڈیٹا != تہی) ? (char_u **)ga.ga_ڈیٹا
						  : (char_u **)_("no matches");

    recursive = FALSE;

    واپس ((flags & EW_EMPTYOK) || ga.ga_ڈیٹا != تہی) ? retval : FAIL;
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
    خالی
addfile(
    garray_T	*gap,
    char_u	*f,	// filename
    صحیح		flags)
{
    char_u	*p;
    صحیح		isdir;
    stat_T	sb;

#اگر_تعریف_شدہ MSWIN
    اگر (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	طویل perm = mch_getperm(f);

	اگر (perm < 0)
	    واپس;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    ورنہ
#اگر_ختم
    {
	// if the file/dir/link doesn't exist, may not add it
	اگر (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((حرف *)f, &sb) < 0 : mch_getperm(f) < 0))
	    واپس;
	isdir = mch_isdir(f);
    }

#اگر_تعریف_شدہ FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    اگر (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != تہی)
	واپس;
#اگر_ختم

    اگر ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	واپس;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    اگر (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, تہی, !(flags & EW_SHELLCMD)))
	واپس;

    // Make room for another item in the file list.
    اگر (ga_grow(gap, 1) == FAIL)
	واپس;

    p = alloc(STRLEN(f) + 1 + isdir);
    اگر (p == تہی)
	واپس;

    STRCPY(p, f);
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    slash_adjust(p);
#اگر_ختم
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    اگر (isdir && (flags & EW_ADDSLASH))
	جوڑو_pathsep(p);
    ((char_u **)gap->ga_ڈیٹا)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    خالی
FreeWild(صحیح گنتی, char_u **files)
{
    اگر (گنتی <= 0 || files == تہی)
	واپس;
    جب (گنتی--)
	vim_free(files[گنتی]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    صحیح
pathcmp(ثابت حرف *p, ثابت حرف *q, صحیح maxlen)
{
    صحیح		i, j;
    صحیح		c1, c2;
    ثابت حرف	*s = تہی;

    برائے (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	اگر (c1 == NUL)
	{
	    اگر (c2 == NUL)  // full match
		واپس 0;
	    s = q;
	    i = j;
	    توڑو;
	}

	// End of "q": check if "p" just has a slash.
	اگر (c2 == NUL)
	{
	    s = p;
	    توڑو;
	}

	اگر ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#اگر_ختم
		)
	{
	    اگر (vim_ispathsep(c1))
		واپس -1;
	    اگر (vim_ispathsep(c2))
		واپس 1;
	    واپس p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    اگر (s == تہی)	// "i" or "j" ran into "maxlen"
	واپس 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    اگر (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    && (c1 == '/' || c1 == '\\')
#ورنہ
	    && c1 == '/'
#اگر_ختم
       )
	واپس 0;   // match with trailing slash
    اگر (s == q)
	واپس -1;	    // no match
    واپس 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    صحیح
vim_isAbsName(char_u *name)
{
    واپس (راستہ_with_url(name) != 0 || mch_is_بھرا_Name(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
vim_بھرا_Name(
    char_u	*fname,
    char_u	*buf,
    صحیح		len,
    صحیح		force)	    // force expansion even when already absolute
{
    صحیح		retval = OK;
    صحیح		url;

    *buf = NUL;
    اگر (fname == تہی)
	واپس FAIL;

    url = راستہ_with_url(fname);
    اگر (!url)
	retval = mch_بھرا_Name(fname, buf, len, force);
    اگر (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#اگر defined(MSWIN)
    slash_adjust(buf);
#اگر_ختم
    واپس retval;
}
