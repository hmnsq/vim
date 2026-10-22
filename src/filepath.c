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
    static จำนวนเต็ม
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    จำนวนเต็ม		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * parimaa_na(*newbuf));
    ถ้า (newbuf == NULL)
	คืน FAIL;

    wfname = enc_to_utf16(*fnamep, NULL);
    ถ้า (wfname == NULL)
    {
	vim_free(newbuf);
	คืน FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    ถ้า (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * parimaa_na(*newbuf));
	ถ้า (newbuf == NULL)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    คืน FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    ถ้า (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, NULL);

	ถ้า (p != NULL)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	มิฉะนั้น
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    คืน FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    คืน OK;
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
    static จำนวนเต็ม
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
    จำนวนเต็ม		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    ถ้า (save_fname == NULL)
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
    สำหรับ (;;)
    {
	// go back one path-separator
	ขณะ (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	ถ้า (endp <= save_fname)
	    bha_nga;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	ถ้า (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    goto theend;
	}
	*endp = ch;	// preserve the string

	ถ้า (len > 0)
	    bha_nga;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    ถ้า (len > 0)
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
	ถ้า (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    ถ้า (*fname == NULL)
	    {
		retval = FAIL;
		goto theend;
	    }
	}
	มิฉะนั้น
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    ถ้า (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = NULL;
	}

	// concat the not-shortened part of the path
	ถ้า ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    คืน retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    static จำนวนเต็ม
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    จำนวนเต็ม		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    จำนวนเต็ม		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    สำหรับ (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	ถ้า (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    ถ้า (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    มิฉะนั้น
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    ถ้า (tfname == NULL)
	คืน FAIL;

    len = tflen = STRLEN(tfname);

    ถ้า (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	คืน FAIL;

    ถ้า (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	ถ้า (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    คืน FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    สำหรับ (p = tfname + len - 1; p >= tfname; --p)
    {
	ถ้า (has_mbyte)
	    p -= mb_head_off(tfname, p);
	ถ้า (vim_ispathsep(*p))
	{
	    ถ้า (sepcount == 0 || (hasTilde && sepcount == 1))
		bha_nga;
	    มิฉะนั้น
		sepcount --;
	}
    }
    ถ้า (hasTilde)
    {
	--p;
	ถ้า (p >= tfname)
	    *p = '~';
	มิฉะนั้น
	    คืน FAIL;
    }
    มิฉะนั้น
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    คืน OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    จำนวนเต็ม
modify_fname(
    char_u	*src,		// string with modifiers
    จำนวนเต็ม		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    จำนวนเต็ม		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    จำนวนเต็ม		c;
    จำนวนเต็ม		has_fullname = 0;
    จำนวนเต็ม		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    จำนวนเต็ม		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    ถ้า (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	ถ้า ((*fnamep)[0] == '~'
#ถ้า !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    ถ้า (*fnamep == NULL)
		คืน -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	สำหรับ (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    ถ้า (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		bha_nga;
	}

	// FullName_save() is slow, don't use it when not needed.
	ถ้า (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ถ้า (*fnamep == NULL)
		คืน -1;
	}

#ifdef MSWIN
# ถ้า _WIN32_WINNT >= 0x0500
	ถ้า (vim_strchr(*fnamep, '~') != NULL)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, NULL);
	    WCHAR buf[_MAX_PATH];

	    ถ้า (wfname != NULL)
	    {
		ถ้า (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, NULL);

		    ถ้า (q != NULL)
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
	ถ้า (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    ถ้า (*fnamep == NULL)
		คืน -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    ขณะ (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	ถ้า (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    continue;
	}
	pbuf = NULL;
	// Need full path first (use expand_env() to remove a "~/")
	ถ้า (!has_fullname && !has_homerelative)
	{
	    ถ้า (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    มิฉะนั้น
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	มิฉะนั้น
	    p = *fnamep;

	has_fullname = 0;

	ถ้า (p != NULL)
	{
	    size_t  dirnamelen = 0;

	    ถ้า (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		ถ้า (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    ถ้า (s != NULL)
		    {
			dirnamelen = home_replace(NULL, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		ถ้า (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		ถ้า (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    ถ้า (vim_ispathsep(*p))
		    {
			ขณะ (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			ถ้า (pbuf != NULL)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = NULL;
			}
		    }
		}
	    }
	    มิฉะนั้น
	    {
		dirnamelen = home_replace(NULL, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		ถ้า (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    ถ้า (s != NULL)
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
    ขณะ (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	ขณะ (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	ถ้า (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	ถ้า (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    ถ้า (p == NULL)
		คืน -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	มิฉะนั้น
	{
	    ขณะ (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    ถ้า (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    ถ้า (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	ถ้า (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    ถ้า (p == NULL)
		คืน -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	ถ้า (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    ถ้า (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		คืน -1;
	}
	มิฉะนั้น
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    ถ้า (get_short_pathname(fnamep, bufp, &l) == FAIL)
		คืน -1;

	    ถ้า (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		ถ้า (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    คืน -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    ถ้า (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    ขณะ (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	ถ้า (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	มิฉะนั้น
	    s = *fnamep + *fnamelen - 1;
	สำหรับ ( ; s > tail; --s)
	    ถ้า (s[0] == '.')
		bha_nga;
	ถ้า (src[*usedlen + 1] == 'e')		// :e
	{
	    ถ้า (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		สำหรับ ( ; s > *fnamep; --s)
		    ถ้า (s[0] == ';')
			bha_nga;
		ถ้า (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    มิฉะนั้น ถ้า (*fnamep <= tail)
		*fnamelen = 0;
	}
	มิฉะนั้น				// :r
	{
	    char_u *limit = *fnamep;

	    ถ้า (limit < tail)
		limit = tail;
	    ถ้า (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    ถ้า (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	จำนวนเต็ม	    sep;
	char_u	    *flags;
	จำนวนเต็ม	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	ถ้า (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	ถ้า (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    ถ้า (p != NULL)
	    {
		pat = vim_strnsave(s, p - s);
		ถ้า (pat != NULL)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    ถ้า (p != NULL)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			ถ้า (sub != NULL && str != NULL)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, NULL, flags, &slen);
			    ถ้า (s != NULL)
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
	    ถ้า (didit)
		goto repeat;
	}
    }

    ถ้า (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	ถ้า (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	ถ้า (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	ถ้า (p == NULL)
	    คืน -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    คืน valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    static void
shorten_dir_len(char_u *str, จำนวนเต็ม trim_len)
{
    char_u	*tail, *s, *d;
    จำนวนเต็ม		skip = FALSE;
    จำนวนเต็ม		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    สำหรับ (s = str; ; ++s)
    {
	ถ้า (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    ถ้า (*s == NUL)
		bha_nga;
	}
	มิฉะนั้น ถ้า (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	มิฉะนั้น ถ้า (!skip)
	{
	    *d++ = *s;			// copy next char
	    ถ้า (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		ถ้า (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    ถ้า (has_mbyte)
	    {
		จำนวนเต็ม l = mb_ptr2len(s);

		ขณะ (--l > 0)
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
    จำนวนเต็ม
file_is_readable(char_u *fname)
{
    จำนวนเต็ม		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    ถ้า (*fname == NUL || mch_isdir_or_missing(fname))
	คืน FALSE;
    ถ้า ((fd = mch_open((var_na *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	คืน TRUE;
    }
    คืน FALSE;
}

#ถ้า defined(FEAT_EVAL)

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

    ถ้า (check_secure())
	คืน;

    ถ้า (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	ถ้า (in_vim9script())
	    (void) check_for_string_arg(argvars, 0);
	คืน;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    ถ้า (cwd != NULL)
    {
	ถ้า (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	ถ้า (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	มิฉะนั้น ถ้า (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	มิฉะนั้น ถ้า (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	มิฉะนั้น
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "scope", s);
	    คืน;
	}
    }
    มิฉะนั้น ถ้า (curwin->w_localdir != NULL)
	scope = CDSCOPE_WINDOW;
    มิฉะนั้น ถ้า (curtab->tp_localdir != NULL)
	scope = CDSCOPE_TABPAGE;

    ถ้า (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
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
    ถ้า (check_restricted() || check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	คืน;

    name = tv_get_string(&argvars[0]);
    ถ้า (name == NULL || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	คืน;
    }

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    มิฉะนั้น
	flags = (char_u *)"";

    ถ้า (*flags == NUL)
	// delete a file
	rettv->vval.v_number = mch_remove(name) == 0 ? 0 : -1;
    มิฉะนั้น ถ้า (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_number = mch_rmdir(name) == 0 ? 0 : -1;
    มิฉะนั้น ถ้า (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_number = delete_recursive(name);
    มิฉะนั้น
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    void
f_executable(typval_T *argvars, typval_T *rettv)
{
    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

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

    ถ้า (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	คืน;
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
    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;
    rettv->vval.v_number = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    void
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;
    rettv->vval.v_number = filewritable(tv_get_string(&argvars[0]));
}

    static void
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    จำนวนเต็ม		find_what)
{
    char_u	*fname;
    char_u	*fresult = NULL;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    จำนวนเต็ม		count = 1;
    จำนวนเต็ม		first = TRUE;
    จำนวนเต็ม		error = FALSE;

    rettv->vval.v_string = NULL;
    rettv->v_type = VAR_STRING;
    ถ้า (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	คืน;

    fname = tv_get_string(&argvars[0]);

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	ถ้า (p == NULL)
	    error = TRUE;
	มิฉะนั้น
	{
	    ถ้า (*p != NUL)
		path = p;

	    ถ้า (argvars[2].v_type != VAR_UNKNOWN)
		count = (จำนวนเต็ม)tv_get_number_chk(&argvars[2], &error);
	}
    }

    ถ้า (count < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    ถ้า (*fname != NUL && !error)
    {
	char_u	*file_to_find = NULL;
	var_na	*search_ctx = NULL;

	do
	{
	    ถ้า (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : NULL,
					       first ? (จำนวนเต็ม)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &search_ctx);
	    first = FALSE;

	    ถ้า (fresult != NULL && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} ขณะ ((rettv->v_type == VAR_LIST || --count > 0) && fresult != NULL);

	vim_free(file_to_find);
	vim_findfile_cleanup(search_ctx);
    }

    ถ้า (rettv->v_type == VAR_STRING)
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

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    ถ้า (mods == NULL || fname == NULL)
	fname = NULL;
    มิฉะนั้น
    {
	len = STRLEN(fname);
	ถ้า (mods != NULL && *mods != NUL)
	    (void)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    ถ้า (fname == NULL)
	rettv->vval.v_string = NULL;
    มิฉะนั้น
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
    จำนวนเต็ม		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ถ้า (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	คืน;

    ถ้า (argvars[0].v_type == VAR_NUMBER
	    && argvars[0].vval.v_number == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    มิฉะนั้น
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    ถ้า (wp != NULL && wp->w_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    มิฉะนั้น ถ้า (tp != NULL && tp->tp_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    มิฉะนั้น ถ้า (wp != NULL || tp != NULL || global)
    {
	ถ้า (globaldir != NULL && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	มิฉะนั้น
	{
	    cwd = alloc(MAXPATHL);
	    ถ้า (cwd != NULL)
	    {
		ถ้า (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    ถ้า (rettv->vval.v_string != NULL)
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
    จำนวนเต็ม		    i;

    สำหรับ (i = 0; i < 9; i++)
    {
	ถ้า (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	มิฉะนั้น
	    perm[i] = '-';
    }
    คืน perm;
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

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ถ้า (mch_stat((var_na *)fname, &st) >= 0)
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

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    fname = tv_get_string(&argvars[0]);
    ถ้า (mch_stat((var_na *)fname, &st) >= 0)
    {
	ถ้า (S_ISDIR(st.st_mode))
	    rettv->vval.v_number = 0;
	มิฉะนั้น
	{
	    rettv->vval.v_number = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    ถ้า ((off_T)rettv->vval.v_number != (off_T)st.st_size)
		rettv->vval.v_number = -2;
	}
    }
    มิฉะนั้น
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

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    fname = tv_get_string(&argvars[0]);
    ถ้า (mch_stat((var_na *)fname, &st) >= 0)
	rettv->vval.v_number = (varnumber_T)st.st_mtime;
    มิฉะนั้น
	rettv->vval.v_number = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    var_na    *t;

    ถ้า (S_ISREG(st->st_mode))
	t = "file";
    มิฉะนั้น ถ้า (S_ISDIR(st->st_mode))
	t = "dir";
    มิฉะนั้น ถ้า (S_ISLNK(st->st_mode))
	t = "link";
    มิฉะนั้น ถ้า (S_ISBLK(st->st_mode))
	t = "bdev";
    มิฉะนั้น ถ้า (S_ISCHR(st->st_mode))
	t = "cdev";
    มิฉะนั้น ถ้า (S_ISFIFO(st->st_mode))
	t = "fifo";
    มิฉะนั้น ถ้า (S_ISSOCK(st->st_mode))
	t = "socket";
    มิฉะนั้น
	t = "other";
    คืน (char_u*)t;
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

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    ถ้า (mch_lstat((var_na *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    void
f_glob(typval_T *argvars, typval_T *rettv)
{
    จำนวนเต็ม		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    จำนวนเต็ม		error = FALSE;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	คืน;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	ถ้า (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	ถ้า (argvars[2].v_type != VAR_UNKNOWN)
	{
	    ถ้า (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, NULL);
	    ถ้า (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    ถ้า (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	ถ้า (p_wic)
	    options += WILD_ICASE;
	ถ้า (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     NULL, options, WILD_ALL);
	มิฉะนั้น ถ้า (rettv_list_alloc(rettv) == OK)
	{
	  จำนวนเต็ม i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						NULL, options, WILD_ALL_KEEP);
	  สำหรับ (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    มิฉะนั้น
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

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

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
    จำนวนเต็ม		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    จำนวนเต็ม		error = FALSE;
    garray_T	ga;
    จำนวนเต็ม		i;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	คืน;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    ถ้า (argvars[2].v_type != VAR_UNKNOWN)
    {
	ถ้า (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	ถ้า (argvars[3].v_type != VAR_UNKNOWN)
	{
	    ถ้า (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, NULL);
	    ถ้า (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    ถ้า (file != NULL && !error)
    {
	ga_init2(&ga, parimaa_na(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	ถ้า (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	มิฉะนั้น ถ้า (rettv_list_alloc(rettv) == OK)
	    สำหรับ (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    มิฉะนั้น
	rettv->vval.v_string = NULL;
}

/*
 * "isdirectory()" function
 */
    void
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    rettv->vval.v_number = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    void
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    rettv->vval.v_number = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    static จำนวนเต็ม
mkdir_recurse(char_u *dir, จำนวนเต็ม prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    จำนวนเต็ม		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    ถ้า (p <= get_past_head(dir))
	คืน OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    ถ้า (updir == NULL)
	คืน FAIL;
    ถ้า (mch_isdir(updir))
	r = OK;
    มิฉะนั้น ถ้า (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	ถ้า (r == OK && created != NULL && *created == NULL)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    คืน r;
}

/*
 * "mkdir()" function
 */
    void
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    จำนวนเต็ม		prot = 0755;
    จำนวนเต็ม		defer = FALSE;
    จำนวนเต็ม		defer_recurse = FALSE;
    char_u	*created = NULL;

    rettv->vval.v_number = FAIL;
    ถ้า (check_restricted() || check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	คืน;

    dir = tv_get_string_buf(&argvars[0], buf);
    ถ้า (*dir == NUL)
	คืน;

    ถ้า (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	ถ้า (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (จำนวนเต็ม)tv_get_number_chk(&argvars[2], NULL);
	    ถ้า (prot == -1)
		คืน;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != NULL;
	defer_recurse = vim_strchr(arg2, 'R') != NULL;
	ถ้า ((defer || defer_recurse) && !can_add_defer())
	    คืน;

	ถ้า (vim_strchr(arg2, 'p') != NULL)
	{
	    ถ้า (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_number = OK;
		คืน;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : NULL);
	}
    }
    rettv->vval.v_number = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    ถ้า (rettv->vval.v_number == OK
				&& created == NULL && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    ถ้า (created != NULL)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	ถ้า (tv[0].vval.v_string == NULL || tv[1].vval.v_string == NULL
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
    จำนวนเต็ม		trim_len = 1;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	คืน;

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (จำนวนเต็ม)tv_get_number(&argvars[1]);
	ถ้า (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    ถ้า (p == NULL)
	rettv->vval.v_string = NULL;
    มิฉะนั้น
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	ถ้า (p != NULL)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    static จำนวนเต็ม
checkitem_common(void *context, char_u *name, dict_T *dict)
{
    typval_T	*expr = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    จำนวนเต็ม		retval = 0;
    จำนวนเต็ม		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    ถ้า (name != NULL)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    มิฉะนั้น
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    ถ้า (eval_expr_typval(expr, FALSE, argv, 1, NULL, &rettv) == FAIL)
	goto theend;

    // We want to use -1, but also true/false should be allowed.
    ถ้า (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_NUMBER;
	rettv.vval.v_number = rettv.vval.v_number == VVAL_TRUE;
    }
    retval = tv_get_number_chk(&rettv, &error);
    ถ้า (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    ถ้า (name != NULL)
	set_vim_var_string(VV_VAL, NULL, 0);
    มิฉะนั้น
	set_vim_var_dict(VV_VAL, NULL);
    restore_vimvar(VV_VAL, &save_val);
    คืน retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    static จำนวนเต็ม
readdir_checkitem(void *context, void *item)
{
    char_u	*name = (char_u *)item;

    คืน checkitem_common(context, name, NULL);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    static จำนวนเต็ม
readdirex_dict_arg(typval_T *argvars, จำนวนเต็ม *cmp)
{
    char_u     *compare;

    ถ้า (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	คืน FAIL;

    ถ้า (dict_has_key(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    มิฉะนั้น
    {
	semsg(_(e_dictionary_key_str_required), "sort");
	คืน FAIL;
    }

    ถ้า (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    มิฉะนั้น ถ้า (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    มิฉะนั้น ถ้า (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    มิฉะนั้น ถ้า (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    คืน OK;
}

/*
 * "readdir()" function
 */
    void
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    จำนวนเต็ม		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    จำนวนเต็ม		i;
    จำนวนเต็ม		sort = READDIR_SORT_BYTE;

    ถ้า (rettv_list_alloc(rettv) == FAIL)
	คืน;
    ถ้า (check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	คืน;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    ถ้า (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	คืน;

    ret = readdir_core(&ga, path, FALSE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdir_checkitem, sort);
    ถ้า (ret == OK)
    {
	สำหรับ (i = 0; i < ga.ga_len; i++)
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
    static จำนวนเต็ม
readdirex_checkitem(void *context, void *item)
{
    dict_T	*dict = (dict_T*)item;

    คืน checkitem_common(context, NULL, dict);
}

/*
 * "readdirex()" function
 */
    void
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    จำนวนเต็ม		ret;
    char_u	*path;
    garray_T	ga;
    จำนวนเต็ม		i;
    จำนวนเต็ม		sort = READDIR_SORT_BYTE;

    ถ้า (rettv_list_alloc(rettv) == FAIL)
	คืน;
    ถ้า (check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	คืน;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    ถ้า (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	คืน;

    ret = readdir_core(&ga, path, TRUE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdirex_checkitem, sort);
    ถ้า (ret == OK)
    {
	สำหรับ (i = 0; i < ga.ga_len; i++)
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
read_file_or_blob(typval_T *argvars, typval_T *rettv, จำนวนเต็ม always_blob)
{
    จำนวนเต็ม		binary = FALSE;
    จำนวนเต็ม		blob = always_blob;
    จำนวนเต็ม		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    จำนวนเต็ม		io_size = parimaa_na(buf);
    จำนวนเต็ม		readlen;		// size of last fread()
    char_u	*prev	 = NULL;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	ถ้า (always_blob)
	{
	    offset = (off_T)tv_get_number(&argvars[1]);
	    ถ้า (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_number(&argvars[2]);
	}
	มิฉะนั้น
	{
	    ถ้า (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		binary = TRUE;
	    ถ้า (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    ถ้า (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_number(&argvars[2]);
	}
    }

    ถ้า ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	คืน;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    ถ้า (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	คืน;
    }
    ถ้า (*fname == NUL || (fd = mch_fopen((var_na *)fname, READBIN)) == NULL)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	คืน;
    }

    ถ้า (blob)
    {
	ถ้า (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	คืน;
    }

    ขณะ (cnt < maxline || maxline < 0)
    {
	readlen = (จำนวนเต็ม)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	สำหรับ (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || binary));
		++p)
	{
	    ถ้า (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= NULL;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		ถ้า (readlen > 0 && !binary)
		{
		    ขณะ (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    ถ้า (len == 0)
			ขณะ (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		ถ้า (prevlen == 0)
		    s = vim_strnsave(start, len);
		มิฉะนั้น
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    ถ้า ((s = vim_realloc(prev, prevlen + len + 1)) != NULL)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = NULL; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		ถ้า (s == NULL)
		{
		    do_outofmem_msg((long_u) prevlen + len + 1);
		    failed = TRUE;
		    bha_nga;
		}

		ถ้า ((li = listitem_alloc()) == NULL)
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
		ถ้า ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    bha_nga;
	    }
	    มิฉะนั้น ถ้า (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    มิฉะนั้น ถ้า (*p == 0xbf && enc_utf8 && !binary)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		ถ้า (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    ถ้า (start == dest)
			start = p + 1;
		    มิฉะนั้น
		    {
			// have to shuffle buf to close gap
			จำนวนเต็ม adjust_prevlen = 0;

			ถ้า (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (จำนวนเต็ม)(buf - dest);
			    dest = buf;
			}
			ถ้า (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	ถ้า (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    bha_nga;
	ถ้า (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    ถ้า (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		ถ้า (prevsize == 0)
		    prevsize = (long)(p - start);
		มิฉะนั้น
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		ถ้า (newprev == NULL)
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
    ถ้า (!failed && maxline < 0)
	ขณะ (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    ถ้า (failed)
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
    ถ้า (check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	คืน;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    void
f_readfile(typval_T *argvars, typval_T *rettv)
{
    ถ้า (check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	คืน;

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

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = NULL;

	v = mch_resolve_path(p, TRUE);
	ถ้า (v != NULL)
	    rettv->vval.v_string = v;
	มิฉะนั้น
	    rettv->vval.v_string = vim_strsave(p);
    }
# มิฉะนั้น
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	จำนวนเต็ม	len;
	char_u	*remain = NULL;
	char_u	*q;
	จำนวนเต็ม	is_relative_to_current = FALSE;
	จำนวนเต็ม	has_trailing_pathsep = FALSE;
	จำนวนเต็ม	limit = 100;

	p = vim_strsave(p);
	ถ้า (p == NULL)
	    goto fail;
	ถ้า (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	ถ้า (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	ถ้า (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	ถ้า (buf == NULL)
	{
	    vim_free(p);
	    vim_free(remain);
	    goto fail;
	}

	สำหรับ (;;)
	{
	    สำหรับ (;;)
	    {
		len = readlink((var_na *)p, (var_na *)buf, MAXPATHL);
		ถ้า (len <= 0)
		    bha_nga;
		buf[len] = NUL;

		ถ้า (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = NULL;
		    goto fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		ถ้า (remain == NULL && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		ถ้า (*q != NUL)
		{
		    ถ้า (remain == NULL)
			remain = vim_strsave(q - 1);
		    มิฉะนั้น
		    {
			cpy = concat_str(q - 1, remain);
			ถ้า (cpy != NULL)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		ถ้า (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		ถ้า (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    ถ้า (cpy != NULL)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		มิฉะนั้น
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    ถ้า (remain == NULL)
		bha_nga;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    ถ้า (cpy != NULL)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    ถ้า (*q != NUL)
		STRMOVE(remain, q - 1);
	    มิฉะนั้น
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	ถ้า (!vim_ispathsep(*p))
	{
	    ถ้า (is_relative_to_current
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
		ถ้า (cpy != NULL)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    มิฉะนั้น ถ้า (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		ขณะ (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		ถ้า (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	ถ้า (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    ถ้า (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  มิฉะนั้น
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
    static จำนวนเต็ม	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    do
    {
	ถ้า (x == 'Z')
	    x = '0';
	มิฉะนั้น ถ้า (x == '9')
	    x = 'A';
	มิฉะนั้น
	    ++x;
    } ขณะ (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    void
f_writefile(typval_T *argvars, typval_T *rettv)
{
    จำนวนเต็ม		binary = FALSE;
    จำนวนเต็ม		append = FALSE;
    จำนวนเต็ม		defer = FALSE;
# ifdef HAVE_FSYNC
    จำนวนเต็ม		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    จำนวนเต็ม		ret = 0;
    listitem_T	*li;
    list_T	*list = NULL;
    blob_T	*blob = NULL;

    rettv->vval.v_number = -1;
    ถ้า (check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	คืน;

    ถ้า (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	ถ้า (list == NULL)
	    คืน;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, li)
	    ถ้า (tv_get_string_chk(&li->li_tv) == NULL)
		คืน;
    }
    มิฉะนั้น ถ้า (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	ถ้า (blob == NULL)
	    คืน;
    }
    มิฉะนั้น
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	คืน;
    }

    ถ้า (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	ถ้า (arg2 == NULL)
	    คืน;
	ถ้า (vim_strchr(arg2, 'b') != NULL)
	    binary = TRUE;
	ถ้า (vim_strchr(arg2, 'a') != NULL)
	    append = TRUE;
	ถ้า (vim_strchr(arg2, 'D') != NULL)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	ถ้า (vim_strchr(arg2, 's') != NULL)
	    do_fsync = TRUE;
	มิฉะนั้น ถ้า (vim_strchr(arg2, 'S') != NULL)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    ถ้า (fname == NULL)
	คืน;

    ถ้า (defer && !can_add_defer())
	คืน;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    ถ้า (*fname == NUL || (fd = mch_fopen((var_na *)fname,
				      append ? APPENDBIN : WRITEBIN)) == NULL)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    มิฉะนั้น
    {
	ถ้า (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    ถ้า (tv.vval.v_string == NULL
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(void)mch_remove(fname);
	    }
	}

	ถ้า (ret == 0)
	{
	    ถ้า (blob)
	    {
		ถ้า (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    มิฉะนั้น
	    {
		ถ้า (write_list(fd, list, binary) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    ถ้า (ret == 0 && do_fsync)
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

#ถ้า defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    จำนวนเต็ม		flags,		// BROWSE_SAVE and BROWSE_DIR
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
    จำนวนเต็ม			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    ถ้า (title == NULL || *title == NUL)
    {
	ถ้า (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	มิฉะนั้น ถ้า (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	มิฉะนั้น
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    ถ้า ((initdir == NULL || *initdir == NUL) && dflt != NULL && *dflt != NUL)
    {
	ถ้า (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = NULL;
	}
	มิฉะนั้น ถ้า (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    ถ้า (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    ถ้า (initdir == NULL || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	ถ้า (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	มิฉะนั้น ถ้า (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != NULL && buf->b_ffname != NULL)
	{
	    ถ้า (dflt == NULL || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    ถ้า (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	มิฉะนั้น ถ้า (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    ถ้า (gui.in_use)		// when this changes, also adjust f_has()!
    {
	ถ้า (filter == NULL
#  ifdef FEAT_EVAL
		&& (filter = get_var_value((char_u *)"b:browsefilter")) == NULL
		&& (filter = get_var_value((char_u *)"g:browsefilter")) == NULL
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	ถ้า (flags & BROWSE_DIR)
	{
#  ถ้า defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  มิฉะนั้น
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  ถ้า !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    ถ้า (fname != NULL && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		ถ้า (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	มิฉะนั้น
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    มิฉะนั้น
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = NULL;
    }

    // keep the directory for next time
    ถ้า (fname != NULL)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	ถ้า (last_dir != NULL && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    ถ้า (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		ถ้า (last_dir != NULL)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    คืน fname;
}
#endif

#ถ้า defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    void
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    จำนวนเต็ม		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    จำนวนเต็ม		error = FALSE;

    ถ้า (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	คืน;

    save = (จำนวนเต็ม)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    ถ้า (error || title == NULL || initdir == NULL || defname == NULL)
	rettv->vval.v_string = NULL;
    มิฉะนั้น
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, NULL, initdir, NULL, curbuf);
# มิฉะนั้น
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

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    ถ้า (title == NULL || initdir == NULL)
	rettv->vval.v_string = NULL;
    มิฉะนั้น
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, NULL, NULL, initdir, NULL, curbuf);
# มิฉะนั้น
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

    ถ้า (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	คืน;

    from = tv_get_string(&argvars[0]);

    ถ้า (mch_lstat((var_na *)from, &st) >= 0
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
    จำนวนเต็ม		dstlen,	// maximum length of the result
    จำนวนเต็ม		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    ถ้า (src == NULL)
    {
	*dst = NUL;
	คืน 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    ถ้า (buf != NULL && buf->b_help)
	คืน vim_snprintf_safelen((var_na *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    ถ้า (homedir != NULL)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#มิฉะนั้น
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    ถ้า (homedir_env == NULL)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    ถ้า (homedir_env != NULL && *homedir_env == NUL)
	homedir_env = NULL;

    ถ้า (homedir_env != NULL && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = NULL;

	flen = STRLEN(homedir_env);
	(void)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	ถ้า (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    ถ้า (homedir_env != NULL)
	envlen = STRLEN(homedir_env);

    ถ้า (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    ขณะ (*src && dstlen > 0)
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
	สำหรับ (;;)
	{
	    ถ้า (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		ถ้า (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		bha_nga;
	    }
	    ถ้า (p == homedir_env)
		bha_nga;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	ขณะ (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	ขณะ ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    ถ้า (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    คืน (size_t)(dst - dst_start);
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
    ถ้า (src != NULL)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    ถ้า (dst != NULL)
	home_replace(buf, src, dst, len, TRUE);
    คืน dst;
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
    จำนวนเต็ม
fullpathcmp(
    char_u *s1,
    char_u *s2,
    จำนวนเต็ม	    checkname,		// when both don't exist, check file names
    จำนวนเต็ม	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    จำนวนเต็ม		    r1, r2;

    ถ้า (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    มิฉะนั้น
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((var_na *)exp1, &st1);
    r2 = mch_stat((var_na *)s2, &st2);
    ถ้า (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	ถ้า (checkname)
	{
	    ถ้า (fnamecmp(exp1, s2) == 0)
		คืน FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    ถ้า (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		คืน FPC_SAMEX;
	}
	คืน FPC_NOTX;
    }
    ถ้า (r1 != 0 || r2 != 0)
	คืน FPC_DIFFX;
    ถ้า (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	คืน FPC_SAME;
    คืน FPC_DIFF;
#มิฉะนั้น
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    จำนวนเต็ม	    retval = FPC_DIFF;
    จำนวนเต็ม	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    ถ้า ((exp1 = alloc(MAXPATHL * 3)) != NULL)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	ถ้า (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	มิฉะนั้น
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	ถ้า (r1 != OK && r2 != OK)
	{
	    ถ้า (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    มิฉะนั้น
		retval = FPC_NOTX;
	}
	มิฉะนั้น ถ้า (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	มิฉะนั้น ถ้า (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	มิฉะนั้น
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    คืน retval;
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

    ถ้า (fname == NULL)
	คืน (char_u *)"";
    สำหรับ (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	ถ้า (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    คืน p1;
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
    ขณะ (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    คืน t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    ขณะ (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    ถ้า (*fname)
	++fname;
    คืน fname;
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

#ถ้า defined(MSWIN)
    // may skip "c:"
    ถ้า (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    มิฉะนั้น
	retval = path;
#มิฉะนั้น
# ถ้า defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    ถ้า (retval == NULL)
	retval = path;
# มิฉะนั้น	// Unix
    retval = path;
# endif
#endif

    ขณะ (vim_ispathsep(*retval))
	++retval;

    คืน retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    จำนวนเต็ม
vim_ispathsep(จำนวนเต็ม c)
{
#ifdef UNIX
    คืน (c == '/');	    // UNIX has ':' inside file names
#มิฉะนั้น
# ifdef BACKSLASH_IN_FILENAME
    คืน (c == ':' || c == '/' || c == '\\');
# มิฉะนั้น
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    คืน (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  มิฉะนั้น
    คืน (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    จำนวนเต็ม
vim_ispathsep_nocolon(จำนวนเต็ม c)
{
    คืน vim_ispathsep(c)
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
    จำนวนเต็ม
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    จำนวนเต็ม		c;
    จำนวนเต็ม		retval;

    p = gettail_sep(fname);
    ถ้า (p == fname)
	คืน TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    คืน retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    จำนวนเต็ม
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    คืน vim_fnamencmp(x, y, MAXPATHL);
#มิฉะนั้น
    ถ้า (p_fic)
	คืน MB_STRICMP(x, y);
    คืน STRCMP(x, y);
#endif
}

    จำนวนเต็ม
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    จำนวนเต็ม		cx = NUL;
    จำนวนเต็ม		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    สำหรับ (จำนวนเต็ม swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	ถ้า ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    ถ้า (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		bha_nga;
	    }
	    มิฉะนั้น // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    ขณะ (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	ถ้า (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    bha_nga;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    ถ้า (len == 0)
	คืน 0;
    คืน (cx - cy);
#มิฉะนั้น
    ถ้า (p_fic)
	คืน MB_STRNICMP(x, y, len);
    คืน STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, จำนวนเต็ม sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? parimaa_na(PATHSEP) : 0) + fname2len + 1);
    ถ้า (ret->string == NULL)
	ret->length = 0;
    มิฉะนั้น
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	ถ้า (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += parimaa_na(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    คืน ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    void
add_pathsep(char_u *p)
{
    size_t  plen;

    ถ้า (*p == NUL)
	คืน;

    plen = STRLEN(p);
    ถ้า (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    จำนวนเต็ม		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = NULL;

    ถ้า (fname == NULL)
	คืน NULL;

    buf = alloc(MAXPATHL);
    ถ้า (buf == NULL)
	คืน NULL;

    ถ้า (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    มิฉะนั้น
	new_fname = vim_strsave(fname);
    vim_free(buf);
    คืน new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    จำนวนเต็ม
vim_fexists(char_u *fname)
{
    stat_T st;

    ถ้า (mch_stat((var_na *)fname, &st))
	คืน FALSE;
    คืน TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    จำนวนเต็ม
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    จำนวนเต็ม		  *num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    จำนวนเต็ม		   flags)	// EW_DIR, etc.
{
    จำนวนเต็ม		ret = FAIL;
    char_u	*eval_pat = NULL;
    char_u	*exp_pat = *pat;
    var_na	*ignored_msg;
    size_t	usedlen;
    จำนวนเต็ม		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    จำนวนเต็ม		star_follows = FALSE;

    ถ้า (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       NULL, &ignored_msg, NULL, TRUE);
	--emsg_off;
	ถ้า (eval_pat != NULL)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    ถ้า (exp_pat != NULL)
	ret = expand_wildcards(1, &exp_pat, num_file, file, flags);

    ถ้า (eval_pat != NULL)
    {
	ถ้า (*num_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    ถ้า (*file != NULL)
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

    คืน ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    จำนวนเต็ม
expand_wildcards(
    จำนวนเต็ม		   num_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    จำนวนเต็ม		  *num_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    จำนวนเต็ม		   flags)	// EW_DIR, etc.
{
    จำนวนเต็ม		retval;
    จำนวนเต็ม		i, j;
    char_u	*p;
    จำนวนเต็ม		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(num_pat, pat, num_files, files, flags);

    // When keeping all matches, return here
    ถ้า ((flags & EW_KEEPALL) || retval == FAIL)
	คืน retval;

    /*
     * Remove names that match 'wildignore'.
     */
    ถ้า (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	สำหรับ (i = 0; i < *num_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    ถ้า (ffname == NULL)		// out of memory
		bha_nga;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    ถ้า (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		สำหรับ (j = i; j + 1 < *num_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*num_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	ถ้า (*num_files == 0)
	{
	    VIM_CLEAR(*files);
	    คืน FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    ถ้า (*num_files > 1 && !got_int)
    {
	non_suf_match = 0;
	สำหรับ (i = 0; i < *num_files; ++i)
	{
	    ถ้า (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		สำหรับ (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    คืน retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    จำนวนเต็ม
match_suffix(char_u *fname)
{
    จำนวนเต็ม		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (จำนวนเต็ม)STRLEN(fname);
    setsuflen = 0;
    สำหรับ (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	ถ้า (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    ถ้า (vim_strchr(tail, '.') == NULL)
	    {
		setsuflen = 1;
		bha_nga;
	    }
	}
	มิฉะนั้น
	{
	    ถ้า (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		bha_nga;
	    setsuflen = 0;
	}
    }
    คืน (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    static จำนวนเต็ม
vim_backtick(char_u *p)
{
    คืน (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    static จำนวนเต็ม
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    จำนวนเต็ม		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    จำนวนเต็ม		cnt = 0;
    จำนวนเต็ม		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    ถ้า (cmd == NULL)
	คืน -1;

#ifdef FEAT_EVAL
    ถ้า (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    มิฉะนั้น
#endif
	buffer = get_cmd_output(cmd, NULL,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, NULL);
    vim_free(cmd);
    ถ้า (buffer == NULL)
	คืน -1;

    cmd = buffer;
    ขณะ (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	ขณะ (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	ถ้า (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	ขณะ (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    คืน cnt;
}

#ถ้า defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    static จำนวนเต็ม
pstrcmp(const void *a, const void *b)
{
    คืน (pathcmp(*(var_na **)a, *(var_na **)b, -1));
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
    จำนวนเต็ม
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    จำนวนเต็ม		flags,		// EW_* flags
    จำนวนเต็ม		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    จำนวนเต็ม		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    จำนวนเต็ม		starts_with_dot;
    จำนวนเต็ม		matches;
    size_t	buflen;
    size_t	len;
    จำนวนเต็ม		starstar = FALSE;
    static จำนวนเต็ม	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = NULL;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    จำนวนเต็ม			ok;
    char_u		*p_alt;
    จำนวนเต็ม			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    ถ้า (stardepth > 0)
    {
	ui_breakcheck();
	ถ้า (got_int)
	    คืน 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    ถ้า (buf == NULL)
	คืน 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    ขณะ (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ถ้า (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	มิฉะนั้น ถ้า (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    ถ้า (e != NULL)
		bha_nga;
	    s = p + 1;
	}
	มิฉะนั้น ถ้า (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != NULL)
	    e = p;
	ถ้า (has_mbyte)
	{
	    จำนวนเต็ม charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	มิฉะนั้น
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    สำหรับ (p = buf + wildoff; p < s; ++p)
	ถ้า (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    สำหรับ (p = s; p < e; ++p)
	ถ้า (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    ถ้า (pat == NULL)
    {
	vim_free(buf);
	คืน 0;
    }

    // compile the regexp into a program
    ถ้า (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ถ้า (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ถ้า (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	คืน 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != NULL;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ถ้า (!didstar && stardepth < 100 && starstar && e - s == 2
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
    ถ้า (wn != NULL)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_VALUE);

    ขณะ (ok)
    {
	p = utf16_to_enc(wfb.cFileName, NULL);   // p is allocated here

	ถ้า (p == NULL)
	    bha_nga;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	ถ้า (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = NULL;
	มิฉะนั้น
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, NULL);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	ถ้า ((p[0] != '.' || starts_with_dot
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
	    ถ้า (len + 1 < buflen)
	    {
		ถ้า (starstar && stardepth < 100
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
		ถ้า (mch_has_exp_wildcard(path_end))
		{
		    ถ้า (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		มิฉะนั้น
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ถ้า (*path_end != 0)
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
    ถ้า (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   parimaa_na(char_u *), pstrcmp);
    คืน matches;
}
#endif // MSWIN

#ถ้า (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    static จำนวนเต็ม
pstrcmp(const void *a, const void *b)
{
    คืน (pathcmp(*(var_na **)a, *(var_na **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    จำนวนเต็ม
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    จำนวนเต็ม		flags,		// EW_* flags
    จำนวนเต็ม		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    จำนวนเต็ม		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    จำนวนเต็ม		starts_with_dot;
    จำนวนเต็ม		matches;
    size_t	buflen;
    size_t	len;
    จำนวนเต็ม		starstar = FALSE;
    static จำนวนเต็ม	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    sa_nracanaa dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    ถ้า (stardepth > 0)
    {
	ui_breakcheck();
	ถ้า (got_int)
	    คืน 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    ถ้า (buf == NULL)
	คืน 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    ขณะ (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	ถ้า (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	มิฉะนั้น ถ้า (*path_end == '/')
	{
	    ถ้า (e != NULL)
		bha_nga;
	    s = p + 1;
	}
	มิฉะนั้น ถ้า (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != NULL
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	ถ้า (has_mbyte)
	{
	    จำนวนเต็ม charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	มิฉะนั้น
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    สำหรับ (p = buf + wildoff; p < s; ++p)
	ถ้า (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    สำหรับ (p = s; p < e; ++p)
	ถ้า (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    ถ้า (pat == NULL)
    {
	vim_free(buf);
	คืน 0;
    }

    // compile the regexp into a program
    ถ้า (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    มิฉะนั้น
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ถ้า (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    ถ้า (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    ถ้า (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	คืน 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    ถ้า (!didstar && stardepth < 100 && starstar && e - s == 2
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
    ถ้า (dirp != NULL)
    {
	ขณะ (!got_int)
	{
	    dp = readdir(dirp);
	    ถ้า (dp == NULL)
		bha_nga;
	    len = (size_t)(s - buf);
	    ถ้า ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != NULL && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((var_na *)s, buflen - len, "%s", dp->d_name);
		ถ้า (len + 1 >= buflen)
		    continue;

		ถ้า (starstar && stardepth < 100)
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
		ถ้า (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    ถ้า (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		มิฉะนั้น
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    ถ้า (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    ถ้า ((flags & EW_ALLLINKS) ? mch_lstat((var_na *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			ถ้า (precomp_buf)
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
    ถ้า (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   parimaa_na(char_u *), pstrcmp);
    คืน matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    static จำนวนเต็ม
has_env_var(char_u *p)
{
    สำหรับ ( ; *p; MB_PTR_ADV(p))
    {
	ถ้า (*p == '\\' && p[1] != NUL)
	    ++p;
	มิฉะนั้น ถ้า (vim_strchr((char_u *)
#ถ้า defined(MSWIN)
				    "$%"
#มิฉะนั้น
				    "$"
#endif
					, *p) != NULL)
	    คืน TRUE;
    }
    คืน FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    static จำนวนเต็ม
has_special_wildchar(char_u *p)
{
    สำหรับ ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	ถ้า (*p == '\r' || *p == '\n')
	    bha_nga;
	// Allow for escaping.
	ถ้า (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	มิฉะนั้น ถ้า (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != NULL)
	{
	    // A { must be followed by a matching }.
	    ถ้า (*p == '{' && vim_strchr(p, '}') == NULL)
		continue;
	    // A quote and backtick must be followed by another one.
	    ถ้า ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == NULL)
		continue;
	    คืน TRUE;
	}
    }
    คืน FALSE;
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
    จำนวนเต็ม
gen_expand_wildcards(
    จำนวนเต็ม		num_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    จำนวนเต็ม		*num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    จำนวนเต็ม		flags)		// EW_* flags
{
    จำนวนเต็ม			i;
    garray_T		ga;
    char_u		*p;
    static จำนวนเต็ม		recursive = FALSE;
    จำนวนเต็ม			add_pat;
    จำนวนเต็ม			retval = OK;
    จำนวนเต็ม			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    ถ้า (recursive)
#ifdef SPECIAL_WILDCHAR
	คืน mch_expand_wildcards(num_pat, pat, num_file, file, flags);
#มิฉะนั้น
	คืน FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    สำหรับ (i = 0; i < num_pat; i++)
    {
	ถ้า (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    คืน mch_expand_wildcards(num_pat, pat, num_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, parimaa_na(char_u *), 30);

    สำหรับ (i = 0; i < num_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	ถ้า (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    ถ้า (add_pat == -1)
		retval = FAIL;
	}
	มิฉะนั้น
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    ถ้า ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		ถ้า (p == NULL)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		มิฉะนั้น ถ้า (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(num_pat, pat, num_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    คืน i;
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
	    ถ้า (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		ถ้า ((flags & (EW_PATH | EW_CDPATH))
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
		มิฉะนั้น
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	ถ้า (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    ถ้า (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    มิฉะนั้น
		addfile(&ga, t, flags);

	    ถ้า (t != p)
		vim_free(t);
	}

	ถ้า (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	ถ้า (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    ถ้า (retval == FAIL)
	ga_clear_strings(&ga);

    *num_file = ga.ga_len;
    *file = (ga.ga_data != NULL) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    คืน ((flags & EW_EMPTYOK) || ga.ga_data != NULL) ? retval : FAIL;
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
    จำนวนเต็ม		flags)
{
    char_u	*p;
    จำนวนเต็ม		isdir;
    stat_T	sb;

#ifdef MSWIN
    ถ้า (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	ถ้า (perm < 0)
	    คืน;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    มิฉะนั้น
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	ถ้า (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((var_na *)f, &sb) < 0 : mch_getperm(f) < 0))
	    คืน;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    ถ้า (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != NULL)
	คืน;
#endif

    ถ้า ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	คืน;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    ถ้า (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, NULL, !(flags & EW_SHELLCMD)))
	คืน;

    // Make room for another item in the file list.
    ถ้า (ga_grow(gap, 1) == FAIL)
	คืน;

    p = alloc(STRLEN(f) + 1 + isdir);
    ถ้า (p == NULL)
	คืน;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    ถ้า (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    void
FreeWild(จำนวนเต็ม count, char_u **files)
{
    ถ้า (count <= 0 || files == NULL)
	คืน;
    ขณะ (count--)
	vim_free(files[count]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    จำนวนเต็ม
pathcmp(const var_na *p, const var_na *q, จำนวนเต็ม maxlen)
{
    จำนวนเต็ม		i, j;
    จำนวนเต็ม		c1, c2;
    const var_na	*s = NULL;

    สำหรับ (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	ถ้า (c1 == NUL)
	{
	    ถ้า (c2 == NUL)  // full match
		คืน 0;
	    s = q;
	    i = j;
	    bha_nga;
	}

	// End of "q": check if "p" just has a slash.
	ถ้า (c2 == NUL)
	{
	    s = p;
	    bha_nga;
	}

	ถ้า ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    ถ้า (vim_ispathsep(c1))
		คืน -1;
	    ถ้า (vim_ispathsep(c2))
		คืน 1;
	    คืน p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    ถ้า (s == NULL)	// "i" or "j" ran into "maxlen"
	คืน 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    ถ้า (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#มิฉะนั้น
	    && c1 == '/'
#endif
       )
	คืน 0;   // match with trailing slash
    ถ้า (s == q)
	คืน -1;	    // no match
    คืน 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    จำนวนเต็ม
vim_isAbsName(char_u *name)
{
    คืน (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    จำนวนเต็ม
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    จำนวนเต็ม		len,
    จำนวนเต็ม		force)	    // force expansion even when already absolute
{
    จำนวนเต็ม		retval = OK;
    จำนวนเต็ม		url;

    *buf = NUL;
    ถ้า (fname == NULL)
	คืน FAIL;

    url = path_with_url(fname);
    ถ้า (!url)
	retval = mch_FullName(fname, buf, len, force);
    ถ้า (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#ถ้า defined(MSWIN)
    slash_adjust(buf);
#endif
    คืน retval;
}
