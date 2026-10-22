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
    static ကိန်းပြည့်
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    ကိန်းပြည့်		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * parimaa_na(*newbuf));
    အကယ်၍ (newbuf == NULL)
	ပြန်ပေး FAIL;

    wfname = enc_to_utf16(*fnamep, NULL);
    အကယ်၍ (wfname == NULL)
    {
	vim_free(newbuf);
	ပြန်ပေး FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    အကယ်၍ (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * parimaa_na(*newbuf));
	အကယ်၍ (newbuf == NULL)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    ပြန်ပေး FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    အကယ်၍ (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, NULL);

	အကယ်၍ (p != NULL)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	မဟုတ်ပါက
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    ပြန်ပေး FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    ပြန်ပေး OK;
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
    static ကိန်းပြည့်
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
    ကိန်းပြည့်		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    အကယ်၍ (save_fname == NULL)
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
    အတွက် (;;)
    {
	// go back one path-separator
	နေစဉ် (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	အကယ်၍ (endp <= save_fname)
	    bha_nga;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	အကယ်၍ (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    goto theend;
	}
	*endp = ch;	// preserve the string

	အကယ်၍ (len > 0)
	    bha_nga;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    အကယ်၍ (len > 0)
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
	အကယ်၍ (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    အကယ်၍ (*fname == NULL)
	    {
		retval = FAIL;
		goto theend;
	    }
	}
	မဟုတ်ပါက
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    အကယ်၍ (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = NULL;
	}

	// concat the not-shortened part of the path
	အကယ်၍ ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    ပြန်ပေး retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    static ကိန်းပြည့်
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    ကိန်းပြည့်		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    ကိန်းပြည့်		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    အတွက် (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	အကယ်၍ (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    အကယ်၍ (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    မဟုတ်ပါက
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    အကယ်၍ (tfname == NULL)
	ပြန်ပေး FAIL;

    len = tflen = STRLEN(tfname);

    အကယ်၍ (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	ပြန်ပေး FAIL;

    အကယ်၍ (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	အကယ်၍ (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    ပြန်ပေး FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    အတွက် (p = tfname + len - 1; p >= tfname; --p)
    {
	အကယ်၍ (has_mbyte)
	    p -= mb_head_off(tfname, p);
	အကယ်၍ (vim_ispathsep(*p))
	{
	    အကယ်၍ (sepcount == 0 || (hasTilde && sepcount == 1))
		bha_nga;
	    မဟုတ်ပါက
		sepcount --;
	}
    }
    အကယ်၍ (hasTilde)
    {
	--p;
	အကယ်၍ (p >= tfname)
	    *p = '~';
	မဟုတ်ပါက
	    ပြန်ပေး FAIL;
    }
    မဟုတ်ပါက
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    ပြန်ပေး OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    ကိန်းပြည့်
modify_fname(
    char_u	*src,		// string with modifiers
    ကိန်းပြည့်		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    ကိန်းပြည့်		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		has_fullname = 0;
    ကိန်းပြည့်		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    ကိန်းပြည့်		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    အကယ်၍ (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	အကယ်၍ ((*fnamep)[0] == '~'
#အကယ်၍ !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    အကယ်၍ (*fnamep == NULL)
		ပြန်ပေး -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	အတွက် (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    အကယ်၍ (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		bha_nga;
	}

	// FullName_save() is slow, don't use it when not needed.
	အကယ်၍ (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    အကယ်၍ (*fnamep == NULL)
		ပြန်ပေး -1;
	}

#ifdef MSWIN
# အကယ်၍ _WIN32_WINNT >= 0x0500
	အကယ်၍ (vim_strchr(*fnamep, '~') != NULL)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, NULL);
	    WCHAR buf[_MAX_PATH];

	    အကယ်၍ (wfname != NULL)
	    {
		အကယ်၍ (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, NULL);

		    အကယ်၍ (q != NULL)
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
	အကယ်၍ (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    အကယ်၍ (*fnamep == NULL)
		ပြန်ပေး -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    နေစဉ် (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	အကယ်၍ (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    continue;
	}
	pbuf = NULL;
	// Need full path first (use expand_env() to remove a "~/")
	အကယ်၍ (!has_fullname && !has_homerelative)
	{
	    အကယ်၍ (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    မဟုတ်ပါက
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	မဟုတ်ပါက
	    p = *fnamep;

	has_fullname = 0;

	အကယ်၍ (p != NULL)
	{
	    size_t  dirnamelen = 0;

	    အကယ်၍ (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		အကယ်၍ (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    အကယ်၍ (s != NULL)
		    {
			dirnamelen = home_replace(NULL, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		အကယ်၍ (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		အကယ်၍ (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    အကယ်၍ (vim_ispathsep(*p))
		    {
			နေစဉ် (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			အကယ်၍ (pbuf != NULL)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = NULL;
			}
		    }
		}
	    }
	    မဟုတ်ပါက
	    {
		dirnamelen = home_replace(NULL, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		အကယ်၍ (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    အကယ်၍ (s != NULL)
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
    နေစဉ် (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	နေစဉ် (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	အကယ်၍ (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	အကယ်၍ (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    အကယ်၍ (p == NULL)
		ပြန်ပေး -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	မဟုတ်ပါက
	{
	    နေစဉ် (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    အကယ်၍ (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    အကယ်၍ (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	အကယ်၍ (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    အကယ်၍ (p == NULL)
		ပြန်ပေး -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	အကယ်၍ (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    အကယ်၍ (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		ပြန်ပေး -1;
	}
	မဟုတ်ပါက
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    အကယ်၍ (get_short_pathname(fnamep, bufp, &l) == FAIL)
		ပြန်ပေး -1;

	    အကယ်၍ (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		အကယ်၍ (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    ပြန်ပေး -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    အကယ်၍ (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    နေစဉ် (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	အကယ်၍ (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	မဟုတ်ပါက
	    s = *fnamep + *fnamelen - 1;
	အတွက် ( ; s > tail; --s)
	    အကယ်၍ (s[0] == '.')
		bha_nga;
	အကယ်၍ (src[*usedlen + 1] == 'e')		// :e
	{
	    အကယ်၍ (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		အတွက် ( ; s > *fnamep; --s)
		    အကယ်၍ (s[0] == ';')
			bha_nga;
		အကယ်၍ (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    မဟုတ်ပါက အကယ်၍ (*fnamep <= tail)
		*fnamelen = 0;
	}
	မဟုတ်ပါက				// :r
	{
	    char_u *limit = *fnamep;

	    အကယ်၍ (limit < tail)
		limit = tail;
	    အကယ်၍ (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    အကယ်၍ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	ကိန်းပြည့်	    sep;
	char_u	    *flags;
	ကိန်းပြည့်	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	အကယ်၍ (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	အကယ်၍ (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    အကယ်၍ (p != NULL)
	    {
		pat = vim_strnsave(s, p - s);
		အကယ်၍ (pat != NULL)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    အကယ်၍ (p != NULL)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			အကယ်၍ (sub != NULL && str != NULL)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, NULL, flags, &slen);
			    အကယ်၍ (s != NULL)
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
	    အကယ်၍ (didit)
		goto repeat;
	}
    }

    အကယ်၍ (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	အကယ်၍ (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	အကယ်၍ (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	အကယ်၍ (p == NULL)
	    ပြန်ပေး -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    ပြန်ပေး valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    static void
shorten_dir_len(char_u *str, ကိန်းပြည့် trim_len)
{
    char_u	*tail, *s, *d;
    ကိန်းပြည့်		skip = FALSE;
    ကိန်းပြည့်		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    အတွက် (s = str; ; ++s)
    {
	အကယ်၍ (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    အကယ်၍ (*s == NUL)
		bha_nga;
	}
	မဟုတ်ပါက အကယ်၍ (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	မဟုတ်ပါက အကယ်၍ (!skip)
	{
	    *d++ = *s;			// copy next char
	    အကယ်၍ (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		အကယ်၍ (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    အကယ်၍ (has_mbyte)
	    {
		ကိန်းပြည့် l = mb_ptr2len(s);

		နေစဉ် (--l > 0)
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
    ကိန်းပြည့်
file_is_readable(char_u *fname)
{
    ကိန်းပြည့်		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    အကယ်၍ (*fname == NUL || mch_isdir_or_missing(fname))
	ပြန်ပေး FALSE;
    အကယ်၍ ((fd = mch_open((var_na *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
}

#အကယ်၍ defined(FEAT_EVAL)

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

    အကယ်၍ (check_secure())
	ပြန်ပေး;

    အကယ်၍ (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	အကယ်၍ (in_vim9script())
	    (void) check_for_string_arg(argvars, 0);
	ပြန်ပေး;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    အကယ်၍ (cwd != NULL)
    {
	အကယ်၍ (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	အကယ်၍ (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	မဟုတ်ပါက အကယ်၍ (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	မဟုတ်ပါက အကယ်၍ (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	မဟုတ်ပါက
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "scope", s);
	    ပြန်ပေး;
	}
    }
    မဟုတ်ပါက အကယ်၍ (curwin->w_localdir != NULL)
	scope = CDSCOPE_WINDOW;
    မဟုတ်ပါက အကယ်၍ (curtab->tp_localdir != NULL)
	scope = CDSCOPE_TABPAGE;

    အကယ်၍ (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
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
    အကယ်၍ (check_restricted() || check_secure())
	ပြန်ပေး;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ပြန်ပေး;

    name = tv_get_string(&argvars[0]);
    အကယ်၍ (name == NULL || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	ပြန်ပေး;
    }

    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    မဟုတ်ပါက
	flags = (char_u *)"";

    အကယ်၍ (*flags == NUL)
	// delete a file
	rettv->vval.v_number = mch_remove(name) == 0 ? 0 : -1;
    မဟုတ်ပါက အကယ်၍ (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_number = mch_rmdir(name) == 0 ? 0 : -1;
    မဟုတ်ပါက အကယ်၍ (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_number = delete_recursive(name);
    မဟုတ်ပါက
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    void
f_executable(typval_T *argvars, typval_T *rettv)
{
    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

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

    အကယ်၍ (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;
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
    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;
    rettv->vval.v_number = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    void
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;
    rettv->vval.v_number = filewritable(tv_get_string(&argvars[0]));
}

    static void
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    ကိန်းပြည့်		find_what)
{
    char_u	*fname;
    char_u	*fresult = NULL;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    ကိန်းပြည့်		count = 1;
    ကိန်းပြည့်		first = TRUE;
    ကိန်းပြည့်		error = FALSE;

    rettv->vval.v_string = NULL;
    rettv->v_type = VAR_STRING;
    အကယ်၍ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ပြန်ပေး;

    fname = tv_get_string(&argvars[0]);

    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	အကယ်၍ (p == NULL)
	    error = TRUE;
	မဟုတ်ပါက
	{
	    အကယ်၍ (*p != NUL)
		path = p;

	    အကယ်၍ (argvars[2].v_type != VAR_UNKNOWN)
		count = (ကိန်းပြည့်)tv_get_number_chk(&argvars[2], &error);
	}
    }

    အကယ်၍ (count < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    အကယ်၍ (*fname != NUL && !error)
    {
	char_u	*file_to_find = NULL;
	var_na	*search_ctx = NULL;

	do
	{
	    အကယ်၍ (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : NULL,
					       first ? (ကိန်းပြည့်)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &search_ctx);
	    first = FALSE;

	    အကယ်၍ (fresult != NULL && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} နေစဉ် ((rettv->v_type == VAR_LIST || --count > 0) && fresult != NULL);

	vim_free(file_to_find);
	vim_findfile_cleanup(search_ctx);
    }

    အကယ်၍ (rettv->v_type == VAR_STRING)
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

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ပြန်ပေး;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    အကယ်၍ (mods == NULL || fname == NULL)
	fname = NULL;
    မဟုတ်ပါက
    {
	len = STRLEN(fname);
	အကယ်၍ (mods != NULL && *mods != NUL)
	    (void)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    အကယ်၍ (fname == NULL)
	rettv->vval.v_string = NULL;
    မဟုတ်ပါက
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
    ကိန်းပြည့်		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    အကယ်၍ (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	ပြန်ပေး;

    အကယ်၍ (argvars[0].v_type == VAR_NUMBER
	    && argvars[0].vval.v_number == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    မဟုတ်ပါက
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    အကယ်၍ (wp != NULL && wp->w_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    မဟုတ်ပါက အကယ်၍ (tp != NULL && tp->tp_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    မဟုတ်ပါက အကယ်၍ (wp != NULL || tp != NULL || global)
    {
	အကယ်၍ (globaldir != NULL && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	မဟုတ်ပါက
	{
	    cwd = alloc(MAXPATHL);
	    အကယ်၍ (cwd != NULL)
	    {
		အကယ်၍ (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    အကယ်၍ (rettv->vval.v_string != NULL)
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
    ကိန်းပြည့်		    i;

    အတွက် (i = 0; i < 9; i++)
    {
	အကယ်၍ (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	မဟုတ်ပါက
	    perm[i] = '-';
    }
    ပြန်ပေး perm;
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

    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    အကယ်၍ (mch_stat((var_na *)fname, &st) >= 0)
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

    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    fname = tv_get_string(&argvars[0]);
    အကယ်၍ (mch_stat((var_na *)fname, &st) >= 0)
    {
	အကယ်၍ (S_ISDIR(st.st_mode))
	    rettv->vval.v_number = 0;
	မဟုတ်ပါက
	{
	    rettv->vval.v_number = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    အကယ်၍ ((off_T)rettv->vval.v_number != (off_T)st.st_size)
		rettv->vval.v_number = -2;
	}
    }
    မဟုတ်ပါက
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

    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    fname = tv_get_string(&argvars[0]);
    အကယ်၍ (mch_stat((var_na *)fname, &st) >= 0)
	rettv->vval.v_number = (varnumber_T)st.st_mtime;
    မဟုတ်ပါက
	rettv->vval.v_number = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    var_na    *t;

    အကယ်၍ (S_ISREG(st->st_mode))
	t = "file";
    မဟုတ်ပါက အကယ်၍ (S_ISDIR(st->st_mode))
	t = "dir";
    မဟုတ်ပါက အကယ်၍ (S_ISLNK(st->st_mode))
	t = "link";
    မဟုတ်ပါက အကယ်၍ (S_ISBLK(st->st_mode))
	t = "bdev";
    မဟုတ်ပါက အကယ်၍ (S_ISCHR(st->st_mode))
	t = "cdev";
    မဟုတ်ပါက အကယ်၍ (S_ISFIFO(st->st_mode))
	t = "fifo";
    မဟုတ်ပါက အကယ်၍ (S_ISSOCK(st->st_mode))
	t = "socket";
    မဟုတ်ပါက
	t = "other";
    ပြန်ပေး (char_u*)t;
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

    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    အကယ်၍ (mch_lstat((var_na *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    void
f_glob(typval_T *argvars, typval_T *rettv)
{
    ကိန်းပြည့်		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    ကိန်းပြည့်		error = FALSE;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	ပြန်ပေး;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
    {
	အကယ်၍ (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	အကယ်၍ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    အကယ်၍ (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, NULL);
	    အကယ်၍ (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    အကယ်၍ (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	အကယ်၍ (p_wic)
	    options += WILD_ICASE;
	အကယ်၍ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     NULL, options, WILD_ALL);
	မဟုတ်ပါက အကယ်၍ (rettv_list_alloc(rettv) == OK)
	{
	  ကိန်းပြည့် i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						NULL, options, WILD_ALL_KEEP);
	  အတွက် (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    မဟုတ်ပါက
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

    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

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
    ကိန်းပြည့်		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    ကိန်းပြည့်		error = FALSE;
    garray_T	ga;
    ကိန်းပြည့်		i;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	ပြန်ပေး;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    အကယ်၍ (argvars[2].v_type != VAR_UNKNOWN)
    {
	အကယ်၍ (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	အကယ်၍ (argvars[3].v_type != VAR_UNKNOWN)
	{
	    အကယ်၍ (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, NULL);
	    အကယ်၍ (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    အကယ်၍ (file != NULL && !error)
    {
	ga_init2(&ga, parimaa_na(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	အကယ်၍ (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	မဟုတ်ပါက အကယ်၍ (rettv_list_alloc(rettv) == OK)
	    အတွက် (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    မဟုတ်ပါက
	rettv->vval.v_string = NULL;
}

/*
 * "isdirectory()" function
 */
    void
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    rettv->vval.v_number = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    void
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    rettv->vval.v_number = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    static ကိန်းပြည့်
mkdir_recurse(char_u *dir, ကိန်းပြည့် prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    ကိန်းပြည့်		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    အကယ်၍ (p <= get_past_head(dir))
	ပြန်ပေး OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    အကယ်၍ (updir == NULL)
	ပြန်ပေး FAIL;
    အကယ်၍ (mch_isdir(updir))
	r = OK;
    မဟုတ်ပါက အကယ်၍ (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	အကယ်၍ (r == OK && created != NULL && *created == NULL)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    ပြန်ပေး r;
}

/*
 * "mkdir()" function
 */
    void
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    ကိန်းပြည့်		prot = 0755;
    ကိန်းပြည့်		defer = FALSE;
    ကိန်းပြည့်		defer_recurse = FALSE;
    char_u	*created = NULL;

    rettv->vval.v_number = FAIL;
    အကယ်၍ (check_restricted() || check_secure())
	ပြန်ပေး;

    အကယ်၍ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ပြန်ပေး;

    dir = tv_get_string_buf(&argvars[0], buf);
    အကယ်၍ (*dir == NUL)
	ပြန်ပေး;

    အကယ်၍ (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	အကယ်၍ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (ကိန်းပြည့်)tv_get_number_chk(&argvars[2], NULL);
	    အကယ်၍ (prot == -1)
		ပြန်ပေး;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != NULL;
	defer_recurse = vim_strchr(arg2, 'R') != NULL;
	အကယ်၍ ((defer || defer_recurse) && !can_add_defer())
	    ပြန်ပေး;

	အကယ်၍ (vim_strchr(arg2, 'p') != NULL)
	{
	    အကယ်၍ (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_number = OK;
		ပြန်ပေး;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : NULL);
	}
    }
    rettv->vval.v_number = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    အကယ်၍ (rettv->vval.v_number == OK
				&& created == NULL && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    အကယ်၍ (created != NULL)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	အကယ်၍ (tv[0].vval.v_string == NULL || tv[1].vval.v_string == NULL
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
    ကိန်းပြည့်		trim_len = 1;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ပြန်ပေး;

    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (ကိန်းပြည့်)tv_get_number(&argvars[1]);
	အကယ်၍ (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    အကယ်၍ (p == NULL)
	rettv->vval.v_string = NULL;
    မဟုတ်ပါက
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	အကယ်၍ (p != NULL)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    static ကိန်းပြည့်
checkitem_common(void *context, char_u *name, dict_T *dict)
{
    typval_T	*expr = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    ကိန်းပြည့်		retval = 0;
    ကိန်းပြည့်		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    အကယ်၍ (name != NULL)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    မဟုတ်ပါက
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    အကယ်၍ (eval_expr_typval(expr, FALSE, argv, 1, NULL, &rettv) == FAIL)
	goto theend;

    // We want to use -1, but also true/false should be allowed.
    အကယ်၍ (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_NUMBER;
	rettv.vval.v_number = rettv.vval.v_number == VVAL_TRUE;
    }
    retval = tv_get_number_chk(&rettv, &error);
    အကယ်၍ (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    အကယ်၍ (name != NULL)
	set_vim_var_string(VV_VAL, NULL, 0);
    မဟုတ်ပါက
	set_vim_var_dict(VV_VAL, NULL);
    restore_vimvar(VV_VAL, &save_val);
    ပြန်ပေး retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    static ကိန်းပြည့်
readdir_checkitem(void *context, void *item)
{
    char_u	*name = (char_u *)item;

    ပြန်ပေး checkitem_common(context, name, NULL);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    static ကိန်းပြည့်
readdirex_dict_arg(typval_T *argvars, ကိန်းပြည့် *cmp)
{
    char_u     *compare;

    အကယ်၍ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	ပြန်ပေး FAIL;

    အကယ်၍ (dict_has_key(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    မဟုတ်ပါက
    {
	semsg(_(e_dictionary_key_str_required), "sort");
	ပြန်ပေး FAIL;
    }

    အကယ်၍ (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    မဟုတ်ပါက အကယ်၍ (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    မဟုတ်ပါက အကယ်၍ (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    မဟုတ်ပါက အကယ်၍ (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    ပြန်ပေး OK;
}

/*
 * "readdir()" function
 */
    void
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    ကိန်းပြည့်		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		sort = READDIR_SORT_BYTE;

    အကယ်၍ (rettv_list_alloc(rettv) == FAIL)
	ပြန်ပေး;
    အကယ်၍ (check_secure())
	ပြန်ပေး;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ပြန်ပေး;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ပြန်ပေး;

    ret = readdir_core(&ga, path, FALSE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdir_checkitem, sort);
    အကယ်၍ (ret == OK)
    {
	အတွက် (i = 0; i < ga.ga_len; i++)
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
    static ကိန်းပြည့်
readdirex_checkitem(void *context, void *item)
{
    dict_T	*dict = (dict_T*)item;

    ပြန်ပေး checkitem_common(context, NULL, dict);
}

/*
 * "readdirex()" function
 */
    void
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    ကိန်းပြည့်		ret;
    char_u	*path;
    garray_T	ga;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		sort = READDIR_SORT_BYTE;

    အကယ်၍ (rettv_list_alloc(rettv) == FAIL)
	ပြန်ပေး;
    အကယ်၍ (check_secure())
	ပြန်ပေး;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ပြန်ပေး;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ပြန်ပေး;

    ret = readdir_core(&ga, path, TRUE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdirex_checkitem, sort);
    အကယ်၍ (ret == OK)
    {
	အတွက် (i = 0; i < ga.ga_len; i++)
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
read_file_or_blob(typval_T *argvars, typval_T *rettv, ကိန်းပြည့် always_blob)
{
    ကိန်းပြည့်		binary = FALSE;
    ကိန်းပြည့်		blob = always_blob;
    ကိန်းပြည့်		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    ကိန်းပြည့်		io_size = parimaa_na(buf);
    ကိန်းပြည့်		readlen;		// size of last fread()
    char_u	*prev	 = NULL;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
    {
	အကယ်၍ (always_blob)
	{
	    offset = (off_T)tv_get_number(&argvars[1]);
	    အကယ်၍ (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_number(&argvars[2]);
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		binary = TRUE;
	    အကယ်၍ (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    အကယ်၍ (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_number(&argvars[2]);
	}
    }

    အကယ်၍ ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	ပြန်ပေး;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    အကယ်၍ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ပြန်ပေး;
    }
    အကယ်၍ (*fname == NUL || (fd = mch_fopen((var_na *)fname, READBIN)) == NULL)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ပြန်ပေး;
    }

    အကယ်၍ (blob)
    {
	အကယ်၍ (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	ပြန်ပေး;
    }

    နေစဉ် (cnt < maxline || maxline < 0)
    {
	readlen = (ကိန်းပြည့်)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	အတွက် (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || binary));
		++p)
	{
	    အကယ်၍ (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= NULL;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		အကယ်၍ (readlen > 0 && !binary)
		{
		    နေစဉ် (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    အကယ်၍ (len == 0)
			နေစဉ် (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		အကယ်၍ (prevlen == 0)
		    s = vim_strnsave(start, len);
		မဟုတ်ပါက
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    အကယ်၍ ((s = vim_realloc(prev, prevlen + len + 1)) != NULL)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = NULL; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		အကယ်၍ (s == NULL)
		{
		    do_outofmem_msg((long_u) prevlen + len + 1);
		    failed = TRUE;
		    bha_nga;
		}

		အကယ်၍ ((li = listitem_alloc()) == NULL)
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
		အကယ်၍ ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    bha_nga;
	    }
	    မဟုတ်ပါက အကယ်၍ (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    မဟုတ်ပါက အကယ်၍ (*p == 0xbf && enc_utf8 && !binary)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		အကယ်၍ (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    အကယ်၍ (start == dest)
			start = p + 1;
		    မဟုတ်ပါက
		    {
			// have to shuffle buf to close gap
			ကိန်းပြည့် adjust_prevlen = 0;

			အကယ်၍ (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (ကိန်းပြည့်)(buf - dest);
			    dest = buf;
			}
			အကယ်၍ (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	အကယ်၍ (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    bha_nga;
	အကယ်၍ (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    အကယ်၍ (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		အကယ်၍ (prevsize == 0)
		    prevsize = (long)(p - start);
		မဟုတ်ပါက
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		အကယ်၍ (newprev == NULL)
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
    အကယ်၍ (!failed && maxline < 0)
	နေစဉ် (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    အကယ်၍ (failed)
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
    အကယ်၍ (check_secure())
	ပြန်ပေး;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ပြန်ပေး;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    void
f_readfile(typval_T *argvars, typval_T *rettv)
{
    အကယ်၍ (check_secure())
	ပြန်ပေး;

    အကယ်၍ (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ပြန်ပေး;

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

    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = NULL;

	v = mch_resolve_path(p, TRUE);
	အကယ်၍ (v != NULL)
	    rettv->vval.v_string = v;
	မဟုတ်ပါက
	    rettv->vval.v_string = vim_strsave(p);
    }
# မဟုတ်ပါက
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	ကိန်းပြည့်	len;
	char_u	*remain = NULL;
	char_u	*q;
	ကိန်းပြည့်	is_relative_to_current = FALSE;
	ကိန်းပြည့်	has_trailing_pathsep = FALSE;
	ကိန်းပြည့်	limit = 100;

	p = vim_strsave(p);
	အကယ်၍ (p == NULL)
	    goto fail;
	အကယ်၍ (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	အကယ်၍ (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	အကယ်၍ (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	အကယ်၍ (buf == NULL)
	{
	    vim_free(p);
	    vim_free(remain);
	    goto fail;
	}

	အတွက် (;;)
	{
	    အတွက် (;;)
	    {
		len = readlink((var_na *)p, (var_na *)buf, MAXPATHL);
		အကယ်၍ (len <= 0)
		    bha_nga;
		buf[len] = NUL;

		အကယ်၍ (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = NULL;
		    goto fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		အကယ်၍ (remain == NULL && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		အကယ်၍ (*q != NUL)
		{
		    အကယ်၍ (remain == NULL)
			remain = vim_strsave(q - 1);
		    မဟုတ်ပါက
		    {
			cpy = concat_str(q - 1, remain);
			အကယ်၍ (cpy != NULL)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		အကယ်၍ (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		အကယ်၍ (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    အကယ်၍ (cpy != NULL)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		မဟုတ်ပါက
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    အကယ်၍ (remain == NULL)
		bha_nga;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    အကယ်၍ (cpy != NULL)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    အကယ်၍ (*q != NUL)
		STRMOVE(remain, q - 1);
	    မဟုတ်ပါက
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	အကယ်၍ (!vim_ispathsep(*p))
	{
	    အကယ်၍ (is_relative_to_current
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
		အကယ်၍ (cpy != NULL)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    မဟုတ်ပါက အကယ်၍ (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		နေစဉ် (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		အကယ်၍ (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	အကယ်၍ (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    အကယ်၍ (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  မဟုတ်ပါက
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
    static ကိန်းပြည့်	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    do
    {
	အကယ်၍ (x == 'Z')
	    x = '0';
	မဟုတ်ပါက အကယ်၍ (x == '9')
	    x = 'A';
	မဟုတ်ပါက
	    ++x;
    } နေစဉ် (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    void
f_writefile(typval_T *argvars, typval_T *rettv)
{
    ကိန်းပြည့်		binary = FALSE;
    ကိန်းပြည့်		append = FALSE;
    ကိန်းပြည့်		defer = FALSE;
# ifdef HAVE_FSYNC
    ကိန်းပြည့်		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    ကိန်းပြည့်		ret = 0;
    listitem_T	*li;
    list_T	*list = NULL;
    blob_T	*blob = NULL;

    rettv->vval.v_number = -1;
    အကယ်၍ (check_secure())
	ပြန်ပေး;

    အကယ်၍ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ပြန်ပေး;

    အကယ်၍ (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	အကယ်၍ (list == NULL)
	    ပြန်ပေး;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, li)
	    အကယ်၍ (tv_get_string_chk(&li->li_tv) == NULL)
		ပြန်ပေး;
    }
    မဟုတ်ပါက အကယ်၍ (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	အကယ်၍ (blob == NULL)
	    ပြန်ပေး;
    }
    မဟုတ်ပါက
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	ပြန်ပေး;
    }

    အကယ်၍ (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	အကယ်၍ (arg2 == NULL)
	    ပြန်ပေး;
	အကယ်၍ (vim_strchr(arg2, 'b') != NULL)
	    binary = TRUE;
	အကယ်၍ (vim_strchr(arg2, 'a') != NULL)
	    append = TRUE;
	အကယ်၍ (vim_strchr(arg2, 'D') != NULL)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	အကယ်၍ (vim_strchr(arg2, 's') != NULL)
	    do_fsync = TRUE;
	မဟုတ်ပါက အကယ်၍ (vim_strchr(arg2, 'S') != NULL)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    အကယ်၍ (fname == NULL)
	ပြန်ပေး;

    အကယ်၍ (defer && !can_add_defer())
	ပြန်ပေး;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    အကယ်၍ (*fname == NUL || (fd = mch_fopen((var_na *)fname,
				      append ? APPENDBIN : WRITEBIN)) == NULL)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    မဟုတ်ပါက
    {
	အကယ်၍ (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    အကယ်၍ (tv.vval.v_string == NULL
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(void)mch_remove(fname);
	    }
	}

	အကယ်၍ (ret == 0)
	{
	    အကယ်၍ (blob)
	    {
		အကယ်၍ (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    မဟုတ်ပါက
	    {
		အကယ်၍ (write_list(fd, list, binary) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    အကယ်၍ (ret == 0 && do_fsync)
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

#အကယ်၍ defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    ကိန်းပြည့်		flags,		// BROWSE_SAVE and BROWSE_DIR
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
    ကိန်းပြည့်			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    အကယ်၍ (title == NULL || *title == NUL)
    {
	အကယ်၍ (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	မဟုတ်ပါက အကယ်၍ (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	မဟုတ်ပါက
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    အကယ်၍ ((initdir == NULL || *initdir == NUL) && dflt != NULL && *dflt != NUL)
    {
	အကယ်၍ (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = NULL;
	}
	မဟုတ်ပါက အကယ်၍ (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    အကယ်၍ (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    အကယ်၍ (initdir == NULL || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	အကယ်၍ (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	မဟုတ်ပါက အကယ်၍ (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != NULL && buf->b_ffname != NULL)
	{
	    အကယ်၍ (dflt == NULL || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    အကယ်၍ (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	မဟုတ်ပါက အကယ်၍ (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    အကယ်၍ (gui.in_use)		// when this changes, also adjust f_has()!
    {
	အကယ်၍ (filter == NULL
#  ifdef FEAT_EVAL
		&& (filter = get_var_value((char_u *)"b:browsefilter")) == NULL
		&& (filter = get_var_value((char_u *)"g:browsefilter")) == NULL
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	အကယ်၍ (flags & BROWSE_DIR)
	{
#  အကယ်၍ defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  မဟုတ်ပါက
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  အကယ်၍ !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    အကယ်၍ (fname != NULL && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		အကယ်၍ (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	မဟုတ်ပါက
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    မဟုတ်ပါက
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = NULL;
    }

    // keep the directory for next time
    အကယ်၍ (fname != NULL)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	အကယ်၍ (last_dir != NULL && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    အကယ်၍ (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		အကယ်၍ (last_dir != NULL)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    ပြန်ပေး fname;
}
#endif

#အကယ်၍ defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    void
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    ကိန်းပြည့်		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ကိန်းပြည့်		error = FALSE;

    အကယ်၍ (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ပြန်ပေး;

    save = (ကိန်းပြည့်)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    အကယ်၍ (error || title == NULL || initdir == NULL || defname == NULL)
	rettv->vval.v_string = NULL;
    မဟုတ်ပါက
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, NULL, initdir, NULL, curbuf);
# မဟုတ်ပါက
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

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ပြန်ပေး;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    အကယ်၍ (title == NULL || initdir == NULL)
	rettv->vval.v_string = NULL;
    မဟုတ်ပါက
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, NULL, NULL, initdir, NULL, curbuf);
# မဟုတ်ပါက
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

    အကယ်၍ (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	ပြန်ပေး;

    from = tv_get_string(&argvars[0]);

    အကယ်၍ (mch_lstat((var_na *)from, &st) >= 0
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
    ကိန်းပြည့်		dstlen,	// maximum length of the result
    ကိန်းပြည့်		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    အကယ်၍ (src == NULL)
    {
	*dst = NUL;
	ပြန်ပေး 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    အကယ်၍ (buf != NULL && buf->b_help)
	ပြန်ပေး vim_snprintf_safelen((var_na *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    အကယ်၍ (homedir != NULL)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#မဟုတ်ပါက
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    အကယ်၍ (homedir_env == NULL)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    အကယ်၍ (homedir_env != NULL && *homedir_env == NUL)
	homedir_env = NULL;

    အကယ်၍ (homedir_env != NULL && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = NULL;

	flen = STRLEN(homedir_env);
	(void)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	အကယ်၍ (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    အကယ်၍ (homedir_env != NULL)
	envlen = STRLEN(homedir_env);

    အကယ်၍ (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    နေစဉ် (*src && dstlen > 0)
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
	အတွက် (;;)
	{
	    အကယ်၍ (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		အကယ်၍ (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		bha_nga;
	    }
	    အကယ်၍ (p == homedir_env)
		bha_nga;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	နေစဉ် (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	နေစဉ် ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    အကယ်၍ (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    ပြန်ပေး (size_t)(dst - dst_start);
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
    အကယ်၍ (src != NULL)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    အကယ်၍ (dst != NULL)
	home_replace(buf, src, dst, len, TRUE);
    ပြန်ပေး dst;
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
    ကိန်းပြည့်
fullpathcmp(
    char_u *s1,
    char_u *s2,
    ကိန်းပြည့်	    checkname,		// when both don't exist, check file names
    ကိန်းပြည့်	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    ကိန်းပြည့်		    r1, r2;

    အကယ်၍ (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    မဟုတ်ပါက
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((var_na *)exp1, &st1);
    r2 = mch_stat((var_na *)s2, &st2);
    အကယ်၍ (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	အကယ်၍ (checkname)
	{
	    အကယ်၍ (fnamecmp(exp1, s2) == 0)
		ပြန်ပေး FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    အကယ်၍ (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		ပြန်ပေး FPC_SAMEX;
	}
	ပြန်ပေး FPC_NOTX;
    }
    အကယ်၍ (r1 != 0 || r2 != 0)
	ပြန်ပေး FPC_DIFFX;
    အကယ်၍ (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	ပြန်ပေး FPC_SAME;
    ပြန်ပေး FPC_DIFF;
#မဟုတ်ပါက
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    ကိန်းပြည့်	    retval = FPC_DIFF;
    ကိန်းပြည့်	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    အကယ်၍ ((exp1 = alloc(MAXPATHL * 3)) != NULL)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	အကယ်၍ (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	မဟုတ်ပါက
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	အကယ်၍ (r1 != OK && r2 != OK)
	{
	    အကယ်၍ (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    မဟုတ်ပါက
		retval = FPC_NOTX;
	}
	မဟုတ်ပါက အကယ်၍ (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	မဟုတ်ပါက အကယ်၍ (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	မဟုတ်ပါက
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    ပြန်ပေး retval;
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

    အကယ်၍ (fname == NULL)
	ပြန်ပေး (char_u *)"";
    အတွက် (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	အကယ်၍ (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    ပြန်ပေး p1;
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
    နေစဉ် (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    ပြန်ပေး t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    နေစဉ် (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    အကယ်၍ (*fname)
	++fname;
    ပြန်ပေး fname;
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

#အကယ်၍ defined(MSWIN)
    // may skip "c:"
    အကယ်၍ (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    မဟုတ်ပါက
	retval = path;
#မဟုတ်ပါက
# အကယ်၍ defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    အကယ်၍ (retval == NULL)
	retval = path;
# မဟုတ်ပါက	// Unix
    retval = path;
# endif
#endif

    နေစဉ် (vim_ispathsep(*retval))
	++retval;

    ပြန်ပေး retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    ကိန်းပြည့်
vim_ispathsep(ကိန်းပြည့် c)
{
#ifdef UNIX
    ပြန်ပေး (c == '/');	    // UNIX has ':' inside file names
#မဟုတ်ပါက
# ifdef BACKSLASH_IN_FILENAME
    ပြန်ပေး (c == ':' || c == '/' || c == '\\');
# မဟုတ်ပါက
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    ပြန်ပေး (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  မဟုတ်ပါက
    ပြန်ပေး (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    ကိန်းပြည့်
vim_ispathsep_nocolon(ကိန်းပြည့် c)
{
    ပြန်ပေး vim_ispathsep(c)
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
    ကိန်းပြည့်
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		retval;

    p = gettail_sep(fname);
    အကယ်၍ (p == fname)
	ပြန်ပေး TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    ပြန်ပေး retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    ကိန်းပြည့်
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    ပြန်ပေး vim_fnamencmp(x, y, MAXPATHL);
#မဟုတ်ပါက
    အကယ်၍ (p_fic)
	ပြန်ပေး MB_STRICMP(x, y);
    ပြန်ပေး STRCMP(x, y);
#endif
}

    ကိန်းပြည့်
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    ကိန်းပြည့်		cx = NUL;
    ကိန်းပြည့်		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    အတွက် (ကိန်းပြည့် swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	အကယ်၍ ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    အကယ်၍ (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		bha_nga;
	    }
	    မဟုတ်ပါက // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    နေစဉ် (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	အကယ်၍ (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    bha_nga;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    အကယ်၍ (len == 0)
	ပြန်ပေး 0;
    ပြန်ပေး (cx - cy);
#မဟုတ်ပါက
    အကယ်၍ (p_fic)
	ပြန်ပေး MB_STRNICMP(x, y, len);
    ပြန်ပေး STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, ကိန်းပြည့် sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? parimaa_na(PATHSEP) : 0) + fname2len + 1);
    အကယ်၍ (ret->string == NULL)
	ret->length = 0;
    မဟုတ်ပါက
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	အကယ်၍ (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += parimaa_na(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    ပြန်ပေး ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    void
add_pathsep(char_u *p)
{
    size_t  plen;

    အကယ်၍ (*p == NUL)
	ပြန်ပေး;

    plen = STRLEN(p);
    အကယ်၍ (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    ကိန်းပြည့်		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = NULL;

    အကယ်၍ (fname == NULL)
	ပြန်ပေး NULL;

    buf = alloc(MAXPATHL);
    အကယ်၍ (buf == NULL)
	ပြန်ပေး NULL;

    အကယ်၍ (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    မဟုတ်ပါက
	new_fname = vim_strsave(fname);
    vim_free(buf);
    ပြန်ပေး new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    ကိန်းပြည့်
vim_fexists(char_u *fname)
{
    stat_T st;

    အကယ်၍ (mch_stat((var_na *)fname, &st))
	ပြန်ပေး FALSE;
    ပြန်ပေး TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    ကိန်းပြည့်
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    ကိန်းပြည့်		  *num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ကိန်းပြည့်		   flags)	// EW_DIR, etc.
{
    ကိန်းပြည့်		ret = FAIL;
    char_u	*eval_pat = NULL;
    char_u	*exp_pat = *pat;
    var_na	*ignored_msg;
    size_t	usedlen;
    ကိန်းပြည့်		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    ကိန်းပြည့်		star_follows = FALSE;

    အကယ်၍ (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       NULL, &ignored_msg, NULL, TRUE);
	--emsg_off;
	အကယ်၍ (eval_pat != NULL)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    အကယ်၍ (exp_pat != NULL)
	ret = expand_wildcards(1, &exp_pat, num_file, file, flags);

    အကယ်၍ (eval_pat != NULL)
    {
	အကယ်၍ (*num_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    အကယ်၍ (*file != NULL)
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

    ပြန်ပေး ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    ကိန်းပြည့်
expand_wildcards(
    ကိန်းပြည့်		   num_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    ကိန်းပြည့်		  *num_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    ကိန်းပြည့်		   flags)	// EW_DIR, etc.
{
    ကိန်းပြည့်		retval;
    ကိန်းပြည့်		i, j;
    char_u	*p;
    ကိန်းပြည့်		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(num_pat, pat, num_files, files, flags);

    // When keeping all matches, return here
    အကယ်၍ ((flags & EW_KEEPALL) || retval == FAIL)
	ပြန်ပေး retval;

    /*
     * Remove names that match 'wildignore'.
     */
    အကယ်၍ (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	အတွက် (i = 0; i < *num_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    အကယ်၍ (ffname == NULL)		// out of memory
		bha_nga;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    အကယ်၍ (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		အတွက် (j = i; j + 1 < *num_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*num_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	အကယ်၍ (*num_files == 0)
	{
	    VIM_CLEAR(*files);
	    ပြန်ပေး FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    အကယ်၍ (*num_files > 1 && !got_int)
    {
	non_suf_match = 0;
	အတွက် (i = 0; i < *num_files; ++i)
	{
	    အကယ်၍ (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		အတွက် (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    ပြန်ပေး retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    ကိန်းပြည့်
match_suffix(char_u *fname)
{
    ကိန်းပြည့်		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (ကိန်းပြည့်)STRLEN(fname);
    setsuflen = 0;
    အတွက် (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	အကယ်၍ (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    အကယ်၍ (vim_strchr(tail, '.') == NULL)
	    {
		setsuflen = 1;
		bha_nga;
	    }
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		bha_nga;
	    setsuflen = 0;
	}
    }
    ပြန်ပေး (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    static ကိန်းပြည့်
vim_backtick(char_u *p)
{
    ပြန်ပေး (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    static ကိန်းပြည့်
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    ကိန်းပြည့်		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    ကိန်းပြည့်		cnt = 0;
    ကိန်းပြည့်		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    အကယ်၍ (cmd == NULL)
	ပြန်ပေး -1;

#ifdef FEAT_EVAL
    အကယ်၍ (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    မဟုတ်ပါက
#endif
	buffer = get_cmd_output(cmd, NULL,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, NULL);
    vim_free(cmd);
    အကယ်၍ (buffer == NULL)
	ပြန်ပေး -1;

    cmd = buffer;
    နေစဉ် (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	နေစဉ် (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	အကယ်၍ (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	နေစဉ် (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    ပြန်ပေး cnt;
}

#အကယ်၍ defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    static ကိန်းပြည့်
pstrcmp(const void *a, const void *b)
{
    ပြန်ပေး (pathcmp(*(var_na **)a, *(var_na **)b, -1));
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
    ကိန်းပြည့်
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ကိန်းပြည့်		flags,		// EW_* flags
    ကိန်းပြည့်		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ကိန်းပြည့်		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ကိန်းပြည့်		starts_with_dot;
    ကိန်းပြည့်		matches;
    size_t	buflen;
    size_t	len;
    ကိန်းပြည့်		starstar = FALSE;
    static ကိန်းပြည့်	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = NULL;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    ကိန်းပြည့်			ok;
    char_u		*p_alt;
    ကိန်းပြည့်			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    အကယ်၍ (stardepth > 0)
    {
	ui_breakcheck();
	အကယ်၍ (got_int)
	    ပြန်ပေး 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    အကယ်၍ (buf == NULL)
	ပြန်ပေး 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    နေစဉ် (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	အကယ်၍ (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	မဟုတ်ပါက အကယ်၍ (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    အကယ်၍ (e != NULL)
		bha_nga;
	    s = p + 1;
	}
	မဟုတ်ပါက အကယ်၍ (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != NULL)
	    e = p;
	အကယ်၍ (has_mbyte)
	{
	    ကိန်းပြည့် charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	မဟုတ်ပါက
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    အတွက် (p = buf + wildoff; p < s; ++p)
	အကယ်၍ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    အတွက် (p = s; p < e; ++p)
	အကယ်၍ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    အကယ်၍ (pat == NULL)
    {
	vim_free(buf);
	ပြန်ပေး 0;
    }

    // compile the regexp into a program
    အကယ်၍ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    အကယ်၍ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    အကယ်၍ (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ပြန်ပေး 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != NULL;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    အကယ်၍ (!didstar && stardepth < 100 && starstar && e - s == 2
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
    အကယ်၍ (wn != NULL)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_VALUE);

    နေစဉ် (ok)
    {
	p = utf16_to_enc(wfb.cFileName, NULL);   // p is allocated here

	အကယ်၍ (p == NULL)
	    bha_nga;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	အကယ်၍ (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = NULL;
	မဟုတ်ပါက
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, NULL);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	အကယ်၍ ((p[0] != '.' || starts_with_dot
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
	    အကယ်၍ (len + 1 < buflen)
	    {
		အကယ်၍ (starstar && stardepth < 100
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
		အကယ်၍ (mch_has_exp_wildcard(path_end))
		{
		    အကယ်၍ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		မဟုတ်ပါက
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    အကယ်၍ (*path_end != 0)
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
    အကယ်၍ (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   parimaa_na(char_u *), pstrcmp);
    ပြန်ပေး matches;
}
#endif // MSWIN

#အကယ်၍ (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    static ကိန်းပြည့်
pstrcmp(const void *a, const void *b)
{
    ပြန်ပေး (pathcmp(*(var_na **)a, *(var_na **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    ကိန်းပြည့်
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ကိန်းပြည့်		flags,		// EW_* flags
    ကိန်းပြည့်		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ကိန်းပြည့်		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ကိန်းပြည့်		starts_with_dot;
    ကိန်းပြည့်		matches;
    size_t	buflen;
    size_t	len;
    ကိန်းပြည့်		starstar = FALSE;
    static ကိန်းပြည့်	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    sa_nracanaa dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    အကယ်၍ (stardepth > 0)
    {
	ui_breakcheck();
	အကယ်၍ (got_int)
	    ပြန်ပေး 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    အကယ်၍ (buf == NULL)
	ပြန်ပေး 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    နေစဉ် (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	အကယ်၍ (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	မဟုတ်ပါက အကယ်၍ (*path_end == '/')
	{
	    အကယ်၍ (e != NULL)
		bha_nga;
	    s = p + 1;
	}
	မဟုတ်ပါက အကယ်၍ (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != NULL
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	အကယ်၍ (has_mbyte)
	{
	    ကိန်းပြည့် charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	မဟုတ်ပါက
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    အတွက် (p = buf + wildoff; p < s; ++p)
	အကယ်၍ (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    အတွက် (p = s; p < e; ++p)
	အကယ်၍ (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    အကယ်၍ (pat == NULL)
    {
	vim_free(buf);
	ပြန်ပေး 0;
    }

    // compile the regexp into a program
    အကယ်၍ (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    မဟုတ်ပါက
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    အကယ်၍ (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    အကယ်၍ (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    အကယ်၍ (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ပြန်ပေး 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    အကယ်၍ (!didstar && stardepth < 100 && starstar && e - s == 2
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
    အကယ်၍ (dirp != NULL)
    {
	နေစဉ် (!got_int)
	{
	    dp = readdir(dirp);
	    အကယ်၍ (dp == NULL)
		bha_nga;
	    len = (size_t)(s - buf);
	    အကယ်၍ ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != NULL && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((var_na *)s, buflen - len, "%s", dp->d_name);
		အကယ်၍ (len + 1 >= buflen)
		    continue;

		အကယ်၍ (starstar && stardepth < 100)
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
		အကယ်၍ (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    အကယ်၍ (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		မဟုတ်ပါက
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    အကယ်၍ (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    အကယ်၍ ((flags & EW_ALLLINKS) ? mch_lstat((var_na *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			အကယ်၍ (precomp_buf)
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
    အကယ်၍ (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   parimaa_na(char_u *), pstrcmp);
    ပြန်ပေး matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    static ကိန်းပြည့်
has_env_var(char_u *p)
{
    အတွက် ( ; *p; MB_PTR_ADV(p))
    {
	အကယ်၍ (*p == '\\' && p[1] != NUL)
	    ++p;
	မဟုတ်ပါက အကယ်၍ (vim_strchr((char_u *)
#အကယ်၍ defined(MSWIN)
				    "$%"
#မဟုတ်ပါက
				    "$"
#endif
					, *p) != NULL)
	    ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    static ကိန်းပြည့်
has_special_wildchar(char_u *p)
{
    အတွက် ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	အကယ်၍ (*p == '\r' || *p == '\n')
	    bha_nga;
	// Allow for escaping.
	အကယ်၍ (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	မဟုတ်ပါက အကယ်၍ (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != NULL)
	{
	    // A { must be followed by a matching }.
	    အကယ်၍ (*p == '{' && vim_strchr(p, '}') == NULL)
		continue;
	    // A quote and backtick must be followed by another one.
	    အကယ်၍ ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == NULL)
		continue;
	    ပြန်ပေး TRUE;
	}
    }
    ပြန်ပေး FALSE;
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
    ကိန်းပြည့်
gen_expand_wildcards(
    ကိန်းပြည့်		num_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    ကိန်းပြည့်		*num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ကိန်းပြည့်		flags)		// EW_* flags
{
    ကိန်းပြည့်			i;
    garray_T		ga;
    char_u		*p;
    static ကိန်းပြည့်		recursive = FALSE;
    ကိန်းပြည့်			add_pat;
    ကိန်းပြည့်			retval = OK;
    ကိန်းပြည့်			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    အကယ်၍ (recursive)
#ifdef SPECIAL_WILDCHAR
	ပြန်ပေး mch_expand_wildcards(num_pat, pat, num_file, file, flags);
#မဟုတ်ပါက
	ပြန်ပေး FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    အတွက် (i = 0; i < num_pat; i++)
    {
	အကယ်၍ (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    ပြန်ပေး mch_expand_wildcards(num_pat, pat, num_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, parimaa_na(char_u *), 30);

    အတွက် (i = 0; i < num_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	အကယ်၍ (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    အကယ်၍ (add_pat == -1)
		retval = FAIL;
	}
	မဟုတ်ပါက
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    အကယ်၍ ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		အကယ်၍ (p == NULL)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		မဟုတ်ပါက အကယ်၍ (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(num_pat, pat, num_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    ပြန်ပေး i;
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
	    အကယ်၍ (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		အကယ်၍ ((flags & (EW_PATH | EW_CDPATH))
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
		မဟုတ်ပါက
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	အကယ်၍ (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    အကယ်၍ (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    မဟုတ်ပါက
		addfile(&ga, t, flags);

	    အကယ်၍ (t != p)
		vim_free(t);
	}

	အကယ်၍ (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	အကယ်၍ (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    အကယ်၍ (retval == FAIL)
	ga_clear_strings(&ga);

    *num_file = ga.ga_len;
    *file = (ga.ga_data != NULL) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    ပြန်ပေး ((flags & EW_EMPTYOK) || ga.ga_data != NULL) ? retval : FAIL;
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
    ကိန်းပြည့်		flags)
{
    char_u	*p;
    ကိန်းပြည့်		isdir;
    stat_T	sb;

#ifdef MSWIN
    အကယ်၍ (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	အကယ်၍ (perm < 0)
	    ပြန်ပေး;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    မဟုတ်ပါက
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	အကယ်၍ (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((var_na *)f, &sb) < 0 : mch_getperm(f) < 0))
	    ပြန်ပေး;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    အကယ်၍ (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != NULL)
	ပြန်ပေး;
#endif

    အကယ်၍ ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	ပြန်ပေး;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    အကယ်၍ (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, NULL, !(flags & EW_SHELLCMD)))
	ပြန်ပေး;

    // Make room for another item in the file list.
    အကယ်၍ (ga_grow(gap, 1) == FAIL)
	ပြန်ပေး;

    p = alloc(STRLEN(f) + 1 + isdir);
    အကယ်၍ (p == NULL)
	ပြန်ပေး;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    အကယ်၍ (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    void
FreeWild(ကိန်းပြည့် count, char_u **files)
{
    အကယ်၍ (count <= 0 || files == NULL)
	ပြန်ပေး;
    နေစဉ် (count--)
	vim_free(files[count]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    ကိန်းပြည့်
pathcmp(const var_na *p, const var_na *q, ကိန်းပြည့် maxlen)
{
    ကိန်းပြည့်		i, j;
    ကိန်းပြည့်		c1, c2;
    const var_na	*s = NULL;

    အတွက် (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	အကယ်၍ (c1 == NUL)
	{
	    အကယ်၍ (c2 == NUL)  // full match
		ပြန်ပေး 0;
	    s = q;
	    i = j;
	    bha_nga;
	}

	// End of "q": check if "p" just has a slash.
	အကယ်၍ (c2 == NUL)
	{
	    s = p;
	    bha_nga;
	}

	အကယ်၍ ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    အကယ်၍ (vim_ispathsep(c1))
		ပြန်ပေး -1;
	    အကယ်၍ (vim_ispathsep(c2))
		ပြန်ပေး 1;
	    ပြန်ပေး p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    အကယ်၍ (s == NULL)	// "i" or "j" ran into "maxlen"
	ပြန်ပေး 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    အကယ်၍ (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#မဟုတ်ပါက
	    && c1 == '/'
#endif
       )
	ပြန်ပေး 0;   // match with trailing slash
    အကယ်၍ (s == q)
	ပြန်ပေး -1;	    // no match
    ပြန်ပေး 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    ကိန်းပြည့်
vim_isAbsName(char_u *name)
{
    ပြန်ပေး (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    ကိန်းပြည့်
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    ကိန်းပြည့်		len,
    ကိန်းပြည့်		force)	    // force expansion even when already absolute
{
    ကိန်းပြည့်		retval = OK;
    ကိန်းပြည့်		url;

    *buf = NUL;
    အကယ်၍ (fname == NULL)
	ပြန်ပေး FAIL;

    url = path_with_url(fname);
    အကယ်၍ (!url)
	retval = mch_FullName(fname, buf, len, force);
    အကယ်၍ (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#အကယ်၍ defined(MSWIN)
    slash_adjust(buf);
#endif
    ပြန်ပေး retval;
}
