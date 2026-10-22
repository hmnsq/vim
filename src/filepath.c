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
    static số_nguyên
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    số_nguyên		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * sizeof(*newbuf));
    nếu (newbuf == NULL)
	trả_về FAIL;

    wfname = enc_to_utf16(*fnamep, NULL);
    nếu (wfname == NULL)
    {
	vim_free(newbuf);
	trả_về FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    nếu (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * sizeof(*newbuf));
	nếu (newbuf == NULL)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    trả_về FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    nếu (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, NULL);

	nếu (p != NULL)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	không_thì
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    trả_về FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    trả_về OK;
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
    static số_nguyên
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
    số_nguyên		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    nếu (save_fname == NULL)
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
    cho (;;)
    {
	// go back one path-separator
	trong_khi (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	nếu (endp <= save_fname)
	    break;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	nếu (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    goto theend;
	}
	*endp = ch;	// preserve the string

	nếu (len > 0)
	    break;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    nếu (len > 0)
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
	nếu (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    nếu (*fname == NULL)
	    {
		retval = FAIL;
		goto theend;
	    }
	}
	không_thì
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    nếu (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = NULL;
	}

	// concat the not-shortened part of the path
	nếu ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    trả_về retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    static số_nguyên
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    số_nguyên		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    số_nguyên		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    cho (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	nếu (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    nếu (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    không_thì
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    nếu (tfname == NULL)
	trả_về FAIL;

    len = tflen = STRLEN(tfname);

    nếu (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	trả_về FAIL;

    nếu (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	nếu (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    trả_về FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    cho (p = tfname + len - 1; p >= tfname; --p)
    {
	nếu (has_mbyte)
	    p -= mb_head_off(tfname, p);
	nếu (vim_ispathsep(*p))
	{
	    nếu (sepcount == 0 || (hasTilde && sepcount == 1))
		break;
	    không_thì
		sepcount --;
	}
    }
    nếu (hasTilde)
    {
	--p;
	nếu (p >= tfname)
	    *p = '~';
	không_thì
	    trả_về FAIL;
    }
    không_thì
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    trả_về OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    số_nguyên
modify_fname(
    char_u	*src,		// string with modifiers
    số_nguyên		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    số_nguyên		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    số_nguyên		c;
    số_nguyên		has_fullname = 0;
    số_nguyên		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    số_nguyên		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    nếu (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	nếu ((*fnamep)[0] == '~'
#nếu !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    nếu (*fnamep == NULL)
		trả_về -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	cho (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    nếu (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		break;
	}

	// FullName_save() is slow, don't use it when not needed.
	nếu (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    nếu (*fnamep == NULL)
		trả_về -1;
	}

#ifdef MSWIN
# nếu _WIN32_WINNT >= 0x0500
	nếu (vim_strchr(*fnamep, '~') != NULL)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, NULL);
	    WCHAR buf[_MAX_PATH];

	    nếu (wfname != NULL)
	    {
		nếu (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, NULL);

		    nếu (q != NULL)
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
	nếu (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    nếu (*fnamep == NULL)
		trả_về -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    trong_khi (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	nếu (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    continue;
	}
	pbuf = NULL;
	// Need full path first (use expand_env() to remove a "~/")
	nếu (!has_fullname && !has_homerelative)
	{
	    nếu (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    không_thì
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	không_thì
	    p = *fnamep;

	has_fullname = 0;

	nếu (p != NULL)
	{
	    size_t  dirnamelen = 0;

	    nếu (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		nếu (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    nếu (s != NULL)
		    {
			dirnamelen = home_replace(NULL, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		nếu (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		nếu (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    nếu (vim_ispathsep(*p))
		    {
			trong_khi (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			nếu (pbuf != NULL)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = NULL;
			}
		    }
		}
	    }
	    không_thì
	    {
		dirnamelen = home_replace(NULL, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		nếu (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    nếu (s != NULL)
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
    trong_khi (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	trong_khi (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	nếu (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	nếu (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    nếu (p == NULL)
		trả_về -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	không_thì
	{
	    trong_khi (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    nếu (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    nếu (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	nếu (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    nếu (p == NULL)
		trả_về -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	nếu (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    nếu (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		trả_về -1;
	}
	không_thì
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    nếu (get_short_pathname(fnamep, bufp, &l) == FAIL)
		trả_về -1;

	    nếu (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		nếu (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    trả_về -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    nếu (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    trong_khi (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	nếu (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	không_thì
	    s = *fnamep + *fnamelen - 1;
	cho ( ; s > tail; --s)
	    nếu (s[0] == '.')
		break;
	nếu (src[*usedlen + 1] == 'e')		// :e
	{
	    nếu (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		cho ( ; s > *fnamep; --s)
		    nếu (s[0] == ';')
			break;
		nếu (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    không_thì nếu (*fnamep <= tail)
		*fnamelen = 0;
	}
	không_thì				// :r
	{
	    char_u *limit = *fnamep;

	    nếu (limit < tail)
		limit = tail;
	    nếu (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    nếu (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	số_nguyên	    sep;
	char_u	    *flags;
	số_nguyên	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	nếu (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	nếu (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    nếu (p != NULL)
	    {
		pat = vim_strnsave(s, p - s);
		nếu (pat != NULL)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    nếu (p != NULL)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			nếu (sub != NULL && str != NULL)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, NULL, flags, &slen);
			    nếu (s != NULL)
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
	    nếu (didit)
		goto repeat;
	}
    }

    nếu (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	nếu (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	nếu (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	nếu (p == NULL)
	    trả_về -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    trả_về valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    static void
shorten_dir_len(char_u *str, số_nguyên trim_len)
{
    char_u	*tail, *s, *d;
    số_nguyên		skip = FALSE;
    số_nguyên		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    cho (s = str; ; ++s)
    {
	nếu (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    nếu (*s == NUL)
		break;
	}
	không_thì nếu (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	không_thì nếu (!skip)
	{
	    *d++ = *s;			// copy next char
	    nếu (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		nếu (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    nếu (has_mbyte)
	    {
		số_nguyên l = mb_ptr2len(s);

		trong_khi (--l > 0)
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
    số_nguyên
file_is_readable(char_u *fname)
{
    số_nguyên		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    nếu (*fname == NUL || mch_isdir_or_missing(fname))
	trả_về FALSE;
    nếu ((fd = mch_open((char *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	trả_về TRUE;
    }
    trả_về FALSE;
}

#nếu defined(FEAT_EVAL)

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

    nếu (check_secure())
	trả_về;

    nếu (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	nếu (in_vim9script())
	    (void) check_for_string_arg(argvars, 0);
	trả_về;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    nếu (cwd != NULL)
    {
	nếu (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	nếu (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	không_thì nếu (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	không_thì nếu (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	không_thì
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "scope", s);
	    trả_về;
	}
    }
    không_thì nếu (curwin->w_localdir != NULL)
	scope = CDSCOPE_WINDOW;
    không_thì nếu (curtab->tp_localdir != NULL)
	scope = CDSCOPE_TABPAGE;

    nếu (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
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
    nếu (check_restricted() || check_secure())
	trả_về;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	trả_về;

    name = tv_get_string(&argvars[0]);
    nếu (name == NULL || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	trả_về;
    }

    nếu (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    không_thì
	flags = (char_u *)"";

    nếu (*flags == NUL)
	// delete a file
	rettv->vval.v_number = mch_remove(name) == 0 ? 0 : -1;
    không_thì nếu (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_number = mch_rmdir(name) == 0 ? 0 : -1;
    không_thì nếu (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_number = delete_recursive(name);
    không_thì
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    void
f_executable(typval_T *argvars, typval_T *rettv)
{
    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

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

    nếu (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	trả_về;
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
    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;
    rettv->vval.v_number = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    void
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;
    rettv->vval.v_number = filewritable(tv_get_string(&argvars[0]));
}

    static void
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    số_nguyên		find_what)
{
    char_u	*fname;
    char_u	*fresult = NULL;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    số_nguyên		count = 1;
    số_nguyên		first = TRUE;
    số_nguyên		error = FALSE;

    rettv->vval.v_string = NULL;
    rettv->v_type = VAR_STRING;
    nếu (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	trả_về;

    fname = tv_get_string(&argvars[0]);

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	nếu (p == NULL)
	    error = TRUE;
	không_thì
	{
	    nếu (*p != NUL)
		path = p;

	    nếu (argvars[2].v_type != VAR_UNKNOWN)
		count = (số_nguyên)tv_get_number_chk(&argvars[2], &error);
	}
    }

    nếu (count < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    nếu (*fname != NUL && !error)
    {
	char_u	*file_to_find = NULL;
	char	*search_ctx = NULL;

	do
	{
	    nếu (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : NULL,
					       first ? (số_nguyên)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &search_ctx);
	    first = FALSE;

	    nếu (fresult != NULL && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} trong_khi ((rettv->v_type == VAR_LIST || --count > 0) && fresult != NULL);

	vim_free(file_to_find);
	vim_findfile_cleanup(search_ctx);
    }

    nếu (rettv->v_type == VAR_STRING)
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

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	trả_về;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    nếu (mods == NULL || fname == NULL)
	fname = NULL;
    không_thì
    {
	len = STRLEN(fname);
	nếu (mods != NULL && *mods != NUL)
	    (void)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    nếu (fname == NULL)
	rettv->vval.v_string = NULL;
    không_thì
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
    số_nguyên		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    nếu (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	trả_về;

    nếu (argvars[0].v_type == VAR_NUMBER
	    && argvars[0].vval.v_number == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    không_thì
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    nếu (wp != NULL && wp->w_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    không_thì nếu (tp != NULL && tp->tp_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    không_thì nếu (wp != NULL || tp != NULL || global)
    {
	nếu (globaldir != NULL && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	không_thì
	{
	    cwd = alloc(MAXPATHL);
	    nếu (cwd != NULL)
	    {
		nếu (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    nếu (rettv->vval.v_string != NULL)
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
    số_nguyên		    i;

    cho (i = 0; i < 9; i++)
    {
	nếu (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	không_thì
	    perm[i] = '-';
    }
    trả_về perm;
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

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    nếu (mch_stat((char *)fname, &st) >= 0)
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

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    fname = tv_get_string(&argvars[0]);
    nếu (mch_stat((char *)fname, &st) >= 0)
    {
	nếu (S_ISDIR(st.st_mode))
	    rettv->vval.v_number = 0;
	không_thì
	{
	    rettv->vval.v_number = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    nếu ((off_T)rettv->vval.v_number != (off_T)st.st_size)
		rettv->vval.v_number = -2;
	}
    }
    không_thì
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

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    fname = tv_get_string(&argvars[0]);
    nếu (mch_stat((char *)fname, &st) >= 0)
	rettv->vval.v_number = (varnumber_T)st.st_mtime;
    không_thì
	rettv->vval.v_number = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    char    *t;

    nếu (S_ISREG(st->st_mode))
	t = "file";
    không_thì nếu (S_ISDIR(st->st_mode))
	t = "dir";
    không_thì nếu (S_ISLNK(st->st_mode))
	t = "link";
    không_thì nếu (S_ISBLK(st->st_mode))
	t = "bdev";
    không_thì nếu (S_ISCHR(st->st_mode))
	t = "cdev";
    không_thì nếu (S_ISFIFO(st->st_mode))
	t = "fifo";
    không_thì nếu (S_ISSOCK(st->st_mode))
	t = "socket";
    không_thì
	t = "other";
    trả_về (char_u*)t;
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

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    nếu (mch_lstat((char *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    void
f_glob(typval_T *argvars, typval_T *rettv)
{
    số_nguyên		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    số_nguyên		error = FALSE;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	trả_về;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	nếu (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	nếu (argvars[2].v_type != VAR_UNKNOWN)
	{
	    nếu (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, NULL);
	    nếu (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    nếu (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	nếu (p_wic)
	    options += WILD_ICASE;
	nếu (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     NULL, options, WILD_ALL);
	không_thì nếu (rettv_list_alloc(rettv) == OK)
	{
	  số_nguyên i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						NULL, options, WILD_ALL_KEEP);
	  cho (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    không_thì
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

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

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
    số_nguyên		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    số_nguyên		error = FALSE;
    garray_T	ga;
    số_nguyên		i;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	trả_về;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    nếu (argvars[2].v_type != VAR_UNKNOWN)
    {
	nếu (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	nếu (argvars[3].v_type != VAR_UNKNOWN)
	{
	    nếu (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, NULL);
	    nếu (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    nếu (file != NULL && !error)
    {
	ga_init2(&ga, sizeof(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	nếu (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	không_thì nếu (rettv_list_alloc(rettv) == OK)
	    cho (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    không_thì
	rettv->vval.v_string = NULL;
}

/*
 * "isdirectory()" function
 */
    void
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    rettv->vval.v_number = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    void
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    rettv->vval.v_number = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    static số_nguyên
mkdir_recurse(char_u *dir, số_nguyên prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    số_nguyên		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    nếu (p <= get_past_head(dir))
	trả_về OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    nếu (updir == NULL)
	trả_về FAIL;
    nếu (mch_isdir(updir))
	r = OK;
    không_thì nếu (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	nếu (r == OK && created != NULL && *created == NULL)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    trả_về r;
}

/*
 * "mkdir()" function
 */
    void
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    số_nguyên		prot = 0755;
    số_nguyên		defer = FALSE;
    số_nguyên		defer_recurse = FALSE;
    char_u	*created = NULL;

    rettv->vval.v_number = FAIL;
    nếu (check_restricted() || check_secure())
	trả_về;

    nếu (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	trả_về;

    dir = tv_get_string_buf(&argvars[0], buf);
    nếu (*dir == NUL)
	trả_về;

    nếu (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	nếu (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (số_nguyên)tv_get_number_chk(&argvars[2], NULL);
	    nếu (prot == -1)
		trả_về;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != NULL;
	defer_recurse = vim_strchr(arg2, 'R') != NULL;
	nếu ((defer || defer_recurse) && !can_add_defer())
	    trả_về;

	nếu (vim_strchr(arg2, 'p') != NULL)
	{
	    nếu (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_number = OK;
		trả_về;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : NULL);
	}
    }
    rettv->vval.v_number = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    nếu (rettv->vval.v_number == OK
				&& created == NULL && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    nếu (created != NULL)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	nếu (tv[0].vval.v_string == NULL || tv[1].vval.v_string == NULL
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
    số_nguyên		trim_len = 1;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	trả_về;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (số_nguyên)tv_get_number(&argvars[1]);
	nếu (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    nếu (p == NULL)
	rettv->vval.v_string = NULL;
    không_thì
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	nếu (p != NULL)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    static số_nguyên
checkitem_common(void *context, char_u *name, dict_T *dict)
{
    typval_T	*expr = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    số_nguyên		retval = 0;
    số_nguyên		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    nếu (name != NULL)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    không_thì
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    nếu (eval_expr_typval(expr, FALSE, argv, 1, NULL, &rettv) == FAIL)
	goto theend;

    // We want to use -1, but also true/false should be allowed.
    nếu (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_NUMBER;
	rettv.vval.v_number = rettv.vval.v_number == VVAL_TRUE;
    }
    retval = tv_get_number_chk(&rettv, &error);
    nếu (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    nếu (name != NULL)
	set_vim_var_string(VV_VAL, NULL, 0);
    không_thì
	set_vim_var_dict(VV_VAL, NULL);
    restore_vimvar(VV_VAL, &save_val);
    trả_về retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    static số_nguyên
readdir_checkitem(void *context, void *item)
{
    char_u	*name = (char_u *)item;

    trả_về checkitem_common(context, name, NULL);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    static số_nguyên
readdirex_dict_arg(typval_T *argvars, số_nguyên *cmp)
{
    char_u     *compare;

    nếu (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	trả_về FAIL;

    nếu (dict_has_key(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    không_thì
    {
	semsg(_(e_dictionary_key_str_required), "sort");
	trả_về FAIL;
    }

    nếu (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    không_thì nếu (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    không_thì nếu (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    không_thì nếu (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    trả_về OK;
}

/*
 * "readdir()" function
 */
    void
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    số_nguyên		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    số_nguyên		i;
    số_nguyên		sort = READDIR_SORT_BYTE;

    nếu (rettv_list_alloc(rettv) == FAIL)
	trả_về;
    nếu (check_secure())
	trả_về;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	trả_về;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    nếu (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	trả_về;

    ret = readdir_core(&ga, path, FALSE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdir_checkitem, sort);
    nếu (ret == OK)
    {
	cho (i = 0; i < ga.ga_len; i++)
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
    static số_nguyên
readdirex_checkitem(void *context, void *item)
{
    dict_T	*dict = (dict_T*)item;

    trả_về checkitem_common(context, NULL, dict);
}

/*
 * "readdirex()" function
 */
    void
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    số_nguyên		ret;
    char_u	*path;
    garray_T	ga;
    số_nguyên		i;
    số_nguyên		sort = READDIR_SORT_BYTE;

    nếu (rettv_list_alloc(rettv) == FAIL)
	trả_về;
    nếu (check_secure())
	trả_về;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	trả_về;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    nếu (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	trả_về;

    ret = readdir_core(&ga, path, TRUE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdirex_checkitem, sort);
    nếu (ret == OK)
    {
	cho (i = 0; i < ga.ga_len; i++)
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
read_file_or_blob(typval_T *argvars, typval_T *rettv, số_nguyên always_blob)
{
    số_nguyên		binary = FALSE;
    số_nguyên		blob = always_blob;
    số_nguyên		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    số_nguyên		io_size = sizeof(buf);
    số_nguyên		readlen;		// size of last fread()
    char_u	*prev	 = NULL;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    nếu (argvars[1].v_type != VAR_UNKNOWN)
    {
	nếu (always_blob)
	{
	    offset = (off_T)tv_get_number(&argvars[1]);
	    nếu (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_number(&argvars[2]);
	}
	không_thì
	{
	    nếu (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		binary = TRUE;
	    nếu (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    nếu (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_number(&argvars[2]);
	}
    }

    nếu ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	trả_về;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    nếu (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	trả_về;
    }
    nếu (*fname == NUL || (fd = mch_fopen((char *)fname, READBIN)) == NULL)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	trả_về;
    }

    nếu (blob)
    {
	nếu (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	trả_về;
    }

    trong_khi (cnt < maxline || maxline < 0)
    {
	readlen = (số_nguyên)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	cho (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || binary));
		++p)
	{
	    nếu (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= NULL;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		nếu (readlen > 0 && !binary)
		{
		    trong_khi (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    nếu (len == 0)
			trong_khi (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		nếu (prevlen == 0)
		    s = vim_strnsave(start, len);
		không_thì
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    nếu ((s = vim_realloc(prev, prevlen + len + 1)) != NULL)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = NULL; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		nếu (s == NULL)
		{
		    do_outofmem_msg((long_u) prevlen + len + 1);
		    failed = TRUE;
		    break;
		}

		nếu ((li = listitem_alloc()) == NULL)
		{
		    vim_free(s);
		    failed = TRUE;
		    break;
		}
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = s;
		list_append(rettv->vval.v_list, li);

		start = p + 1; // step over newline
		nếu ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    break;
	    }
	    không_thì nếu (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    không_thì nếu (*p == 0xbf && enc_utf8 && !binary)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		nếu (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    nếu (start == dest)
			start = p + 1;
		    không_thì
		    {
			// have to shuffle buf to close gap
			số_nguyên adjust_prevlen = 0;

			nếu (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (số_nguyên)(buf - dest);
			    dest = buf;
			}
			nếu (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	nếu (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    break;
	nếu (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    nếu (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		nếu (prevsize == 0)
		    prevsize = (long)(p - start);
		không_thì
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		nếu (newprev == NULL)
		{
		    do_outofmem_msg((long_u)prevsize);
		    failed = TRUE;
		    break;
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
    nếu (!failed && maxline < 0)
	trong_khi (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    nếu (failed)
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
    nếu (check_secure())
	trả_về;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	trả_về;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    void
f_readfile(typval_T *argvars, typval_T *rettv)
{
    nếu (check_secure())
	trả_về;

    nếu (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	trả_về;

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

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = NULL;

	v = mch_resolve_path(p, TRUE);
	nếu (v != NULL)
	    rettv->vval.v_string = v;
	không_thì
	    rettv->vval.v_string = vim_strsave(p);
    }
# không_thì
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	số_nguyên	len;
	char_u	*remain = NULL;
	char_u	*q;
	số_nguyên	is_relative_to_current = FALSE;
	số_nguyên	has_trailing_pathsep = FALSE;
	số_nguyên	limit = 100;

	p = vim_strsave(p);
	nếu (p == NULL)
	    goto fail;
	nếu (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	nếu (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	nếu (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	nếu (buf == NULL)
	{
	    vim_free(p);
	    vim_free(remain);
	    goto fail;
	}

	cho (;;)
	{
	    cho (;;)
	    {
		len = readlink((char *)p, (char *)buf, MAXPATHL);
		nếu (len <= 0)
		    break;
		buf[len] = NUL;

		nếu (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = NULL;
		    goto fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		nếu (remain == NULL && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		nếu (*q != NUL)
		{
		    nếu (remain == NULL)
			remain = vim_strsave(q - 1);
		    không_thì
		    {
			cpy = concat_str(q - 1, remain);
			nếu (cpy != NULL)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		nếu (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		nếu (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    nếu (cpy != NULL)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		không_thì
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    nếu (remain == NULL)
		break;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    nếu (cpy != NULL)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    nếu (*q != NUL)
		STRMOVE(remain, q - 1);
	    không_thì
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	nếu (!vim_ispathsep(*p))
	{
	    nếu (is_relative_to_current
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
		nếu (cpy != NULL)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    không_thì nếu (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		trong_khi (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		nếu (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	nếu (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    nếu (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  không_thì
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
    static số_nguyên	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    do
    {
	nếu (x == 'Z')
	    x = '0';
	không_thì nếu (x == '9')
	    x = 'A';
	không_thì
	    ++x;
    } trong_khi (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    void
f_writefile(typval_T *argvars, typval_T *rettv)
{
    số_nguyên		binary = FALSE;
    số_nguyên		append = FALSE;
    số_nguyên		defer = FALSE;
# ifdef HAVE_FSYNC
    số_nguyên		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    số_nguyên		ret = 0;
    listitem_T	*li;
    list_T	*list = NULL;
    blob_T	*blob = NULL;

    rettv->vval.v_number = -1;
    nếu (check_secure())
	trả_về;

    nếu (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	trả_về;

    nếu (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	nếu (list == NULL)
	    trả_về;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, li)
	    nếu (tv_get_string_chk(&li->li_tv) == NULL)
		trả_về;
    }
    không_thì nếu (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	nếu (blob == NULL)
	    trả_về;
    }
    không_thì
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	trả_về;
    }

    nếu (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	nếu (arg2 == NULL)
	    trả_về;
	nếu (vim_strchr(arg2, 'b') != NULL)
	    binary = TRUE;
	nếu (vim_strchr(arg2, 'a') != NULL)
	    append = TRUE;
	nếu (vim_strchr(arg2, 'D') != NULL)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	nếu (vim_strchr(arg2, 's') != NULL)
	    do_fsync = TRUE;
	không_thì nếu (vim_strchr(arg2, 'S') != NULL)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    nếu (fname == NULL)
	trả_về;

    nếu (defer && !can_add_defer())
	trả_về;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    nếu (*fname == NUL || (fd = mch_fopen((char *)fname,
				      append ? APPENDBIN : WRITEBIN)) == NULL)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    không_thì
    {
	nếu (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    nếu (tv.vval.v_string == NULL
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(void)mch_remove(fname);
	    }
	}

	nếu (ret == 0)
	{
	    nếu (blob)
	    {
		nếu (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    không_thì
	    {
		nếu (write_list(fd, list, binary) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    nếu (ret == 0 && do_fsync)
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

#nếu defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    số_nguyên		flags,		// BROWSE_SAVE and BROWSE_DIR
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
    số_nguyên			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    nếu (title == NULL || *title == NUL)
    {
	nếu (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	không_thì nếu (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	không_thì
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    nếu ((initdir == NULL || *initdir == NUL) && dflt != NULL && *dflt != NUL)
    {
	nếu (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = NULL;
	}
	không_thì nếu (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    nếu (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    nếu (initdir == NULL || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	nếu (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	không_thì nếu (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != NULL && buf->b_ffname != NULL)
	{
	    nếu (dflt == NULL || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    nếu (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	không_thì nếu (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    nếu (gui.in_use)		// when this changes, also adjust f_has()!
    {
	nếu (filter == NULL
#  ifdef FEAT_EVAL
		&& (filter = get_var_value((char_u *)"b:browsefilter")) == NULL
		&& (filter = get_var_value((char_u *)"g:browsefilter")) == NULL
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	nếu (flags & BROWSE_DIR)
	{
#  nếu defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  không_thì
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  nếu !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    nếu (fname != NULL && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		nếu (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	không_thì
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    không_thì
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = NULL;
    }

    // keep the directory for next time
    nếu (fname != NULL)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	nếu (last_dir != NULL && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    nếu (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		nếu (last_dir != NULL)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    trả_về fname;
}
#endif

#nếu defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    void
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    số_nguyên		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    số_nguyên		error = FALSE;

    nếu (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	trả_về;

    save = (số_nguyên)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    nếu (error || title == NULL || initdir == NULL || defname == NULL)
	rettv->vval.v_string = NULL;
    không_thì
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, NULL, initdir, NULL, curbuf);
# không_thì
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

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	trả_về;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    nếu (title == NULL || initdir == NULL)
	rettv->vval.v_string = NULL;
    không_thì
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, NULL, NULL, initdir, NULL, curbuf);
# không_thì
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

    nếu (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	trả_về;

    from = tv_get_string(&argvars[0]);

    nếu (mch_lstat((char *)from, &st) >= 0
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
    số_nguyên		dstlen,	// maximum length of the result
    số_nguyên		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    nếu (src == NULL)
    {
	*dst = NUL;
	trả_về 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    nếu (buf != NULL && buf->b_help)
	trả_về vim_snprintf_safelen((char *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    nếu (homedir != NULL)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#không_thì
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    nếu (homedir_env == NULL)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    nếu (homedir_env != NULL && *homedir_env == NUL)
	homedir_env = NULL;

    nếu (homedir_env != NULL && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = NULL;

	flen = STRLEN(homedir_env);
	(void)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	nếu (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    nếu (homedir_env != NULL)
	envlen = STRLEN(homedir_env);

    nếu (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    trong_khi (*src && dstlen > 0)
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
	cho (;;)
	{
	    nếu (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		nếu (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		break;
	    }
	    nếu (p == homedir_env)
		break;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	trong_khi (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	trong_khi ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    nếu (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    trả_về (size_t)(dst - dst_start);
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
    nếu (src != NULL)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    nếu (dst != NULL)
	home_replace(buf, src, dst, len, TRUE);
    trả_về dst;
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
    số_nguyên
fullpathcmp(
    char_u *s1,
    char_u *s2,
    số_nguyên	    checkname,		// when both don't exist, check file names
    số_nguyên	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    số_nguyên		    r1, r2;

    nếu (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    không_thì
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((char *)exp1, &st1);
    r2 = mch_stat((char *)s2, &st2);
    nếu (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	nếu (checkname)
	{
	    nếu (fnamecmp(exp1, s2) == 0)
		trả_về FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    nếu (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		trả_về FPC_SAMEX;
	}
	trả_về FPC_NOTX;
    }
    nếu (r1 != 0 || r2 != 0)
	trả_về FPC_DIFFX;
    nếu (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	trả_về FPC_SAME;
    trả_về FPC_DIFF;
#không_thì
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    số_nguyên	    retval = FPC_DIFF;
    số_nguyên	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    nếu ((exp1 = alloc(MAXPATHL * 3)) != NULL)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	nếu (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	không_thì
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	nếu (r1 != OK && r2 != OK)
	{
	    nếu (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    không_thì
		retval = FPC_NOTX;
	}
	không_thì nếu (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	không_thì nếu (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	không_thì
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    trả_về retval;
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

    nếu (fname == NULL)
	trả_về (char_u *)"";
    cho (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	nếu (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    trả_về p1;
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
    trong_khi (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    trả_về t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    trong_khi (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    nếu (*fname)
	++fname;
    trả_về fname;
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

#nếu defined(MSWIN)
    // may skip "c:"
    nếu (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    không_thì
	retval = path;
#không_thì
# nếu defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    nếu (retval == NULL)
	retval = path;
# không_thì	// Unix
    retval = path;
# endif
#endif

    trong_khi (vim_ispathsep(*retval))
	++retval;

    trả_về retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    số_nguyên
vim_ispathsep(số_nguyên c)
{
#ifdef UNIX
    trả_về (c == '/');	    // UNIX has ':' inside file names
#không_thì
# ifdef BACKSLASH_IN_FILENAME
    trả_về (c == ':' || c == '/' || c == '\\');
# không_thì
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    trả_về (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  không_thì
    trả_về (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    số_nguyên
vim_ispathsep_nocolon(số_nguyên c)
{
    trả_về vim_ispathsep(c)
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
    số_nguyên
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    số_nguyên		c;
    số_nguyên		retval;

    p = gettail_sep(fname);
    nếu (p == fname)
	trả_về TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    trả_về retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    số_nguyên
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    trả_về vim_fnamencmp(x, y, MAXPATHL);
#không_thì
    nếu (p_fic)
	trả_về MB_STRICMP(x, y);
    trả_về STRCMP(x, y);
#endif
}

    số_nguyên
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    số_nguyên		cx = NUL;
    số_nguyên		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    cho (số_nguyên swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	nếu ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    nếu (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		break;
	    }
	    không_thì // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    trong_khi (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	nếu (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    break;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    nếu (len == 0)
	trả_về 0;
    trả_về (cx - cy);
#không_thì
    nếu (p_fic)
	trả_về MB_STRNICMP(x, y, len);
    trả_về STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, số_nguyên sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? sizeof(PATHSEP) : 0) + fname2len + 1);
    nếu (ret->string == NULL)
	ret->length = 0;
    không_thì
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	nếu (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += sizeof(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    trả_về ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    void
add_pathsep(char_u *p)
{
    size_t  plen;

    nếu (*p == NUL)
	trả_về;

    plen = STRLEN(p);
    nếu (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    số_nguyên		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = NULL;

    nếu (fname == NULL)
	trả_về NULL;

    buf = alloc(MAXPATHL);
    nếu (buf == NULL)
	trả_về NULL;

    nếu (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    không_thì
	new_fname = vim_strsave(fname);
    vim_free(buf);
    trả_về new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    số_nguyên
vim_fexists(char_u *fname)
{
    stat_T st;

    nếu (mch_stat((char *)fname, &st))
	trả_về FALSE;
    trả_về TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    số_nguyên
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    số_nguyên		  *num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    số_nguyên		   flags)	// EW_DIR, etc.
{
    số_nguyên		ret = FAIL;
    char_u	*eval_pat = NULL;
    char_u	*exp_pat = *pat;
    char	*ignored_msg;
    size_t	usedlen;
    số_nguyên		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    số_nguyên		star_follows = FALSE;

    nếu (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       NULL, &ignored_msg, NULL, TRUE);
	--emsg_off;
	nếu (eval_pat != NULL)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    nếu (exp_pat != NULL)
	ret = expand_wildcards(1, &exp_pat, num_file, file, flags);

    nếu (eval_pat != NULL)
    {
	nếu (*num_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    nếu (*file != NULL)
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

    trả_về ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    số_nguyên
expand_wildcards(
    số_nguyên		   num_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    số_nguyên		  *num_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    số_nguyên		   flags)	// EW_DIR, etc.
{
    số_nguyên		retval;
    số_nguyên		i, j;
    char_u	*p;
    số_nguyên		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(num_pat, pat, num_files, files, flags);

    // When keeping all matches, return here
    nếu ((flags & EW_KEEPALL) || retval == FAIL)
	trả_về retval;

    /*
     * Remove names that match 'wildignore'.
     */
    nếu (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	cho (i = 0; i < *num_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    nếu (ffname == NULL)		// out of memory
		break;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    nếu (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		cho (j = i; j + 1 < *num_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*num_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	nếu (*num_files == 0)
	{
	    VIM_CLEAR(*files);
	    trả_về FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    nếu (*num_files > 1 && !got_int)
    {
	non_suf_match = 0;
	cho (i = 0; i < *num_files; ++i)
	{
	    nếu (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		cho (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    trả_về retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    số_nguyên
match_suffix(char_u *fname)
{
    số_nguyên		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (số_nguyên)STRLEN(fname);
    setsuflen = 0;
    cho (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	nếu (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    nếu (vim_strchr(tail, '.') == NULL)
	    {
		setsuflen = 1;
		break;
	    }
	}
	không_thì
	{
	    nếu (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		break;
	    setsuflen = 0;
	}
    }
    trả_về (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    static số_nguyên
vim_backtick(char_u *p)
{
    trả_về (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    static số_nguyên
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    số_nguyên		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    số_nguyên		cnt = 0;
    số_nguyên		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    nếu (cmd == NULL)
	trả_về -1;

#ifdef FEAT_EVAL
    nếu (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    không_thì
#endif
	buffer = get_cmd_output(cmd, NULL,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, NULL);
    vim_free(cmd);
    nếu (buffer == NULL)
	trả_về -1;

    cmd = buffer;
    trong_khi (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	trong_khi (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	nếu (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	trong_khi (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    trả_về cnt;
}

#nếu defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    static số_nguyên
pstrcmp(const void *a, const void *b)
{
    trả_về (pathcmp(*(char **)a, *(char **)b, -1));
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
    số_nguyên
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    số_nguyên		flags,		// EW_* flags
    số_nguyên		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    số_nguyên		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    số_nguyên		starts_with_dot;
    số_nguyên		matches;
    size_t	buflen;
    size_t	len;
    số_nguyên		starstar = FALSE;
    static số_nguyên	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = NULL;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    số_nguyên			ok;
    char_u		*p_alt;
    số_nguyên			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    nếu (stardepth > 0)
    {
	ui_breakcheck();
	nếu (got_int)
	    trả_về 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    nếu (buf == NULL)
	trả_về 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    trong_khi (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	nếu (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	không_thì nếu (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    nếu (e != NULL)
		break;
	    s = p + 1;
	}
	không_thì nếu (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != NULL)
	    e = p;
	nếu (has_mbyte)
	{
	    số_nguyên charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	không_thì
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    cho (p = buf + wildoff; p < s; ++p)
	nếu (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    cho (p = s; p < e; ++p)
	nếu (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    nếu (pat == NULL)
    {
	vim_free(buf);
	trả_về 0;
    }

    // compile the regexp into a program
    nếu (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    nếu (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    nếu (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	trả_về 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != NULL;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    nếu (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((char *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(void)dos_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // Scan all files in the directory with "dir/ *.*"
    vim_snprintf((char *)s, buflen - len, "*.*");
    wn = enc_to_utf16(buf, NULL);
    nếu (wn != NULL)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_VALUE);

    trong_khi (ok)
    {
	p = utf16_to_enc(wfb.cFileName, NULL);   // p is allocated here

	nếu (p == NULL)
	    break;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	nếu (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = NULL;
	không_thì
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, NULL);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	nếu ((p[0] != '.' || starts_with_dot
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
	    len += vim_snprintf((char *)s, buflen - len, "%s", p);
	    nếu (len + 1 < buflen)
	    {
		nếu (starstar && stardepth < 100
			  && (wfb.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((char *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (void)dos_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((char *)buf + len, buflen - len, "%s", path_end);
		nếu (mch_has_exp_wildcard(path_end))
		{
		    nếu (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		không_thì
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    nếu (*path_end != 0)
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
    nếu (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   sizeof(char_u *), pstrcmp);
    trả_về matches;
}
#endif // MSWIN

#nếu (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    static số_nguyên
pstrcmp(const void *a, const void *b)
{
    trả_về (pathcmp(*(char **)a, *(char **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    số_nguyên
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    số_nguyên		flags,		// EW_* flags
    số_nguyên		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    số_nguyên		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    số_nguyên		starts_with_dot;
    số_nguyên		matches;
    size_t	buflen;
    size_t	len;
    số_nguyên		starstar = FALSE;
    static số_nguyên	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    struct dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    nếu (stardepth > 0)
    {
	ui_breakcheck();
	nếu (got_int)
	    trả_về 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    nếu (buf == NULL)
	trả_về 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    trong_khi (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	nếu (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	không_thì nếu (*path_end == '/')
	{
	    nếu (e != NULL)
		break;
	    s = p + 1;
	}
	không_thì nếu (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != NULL
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	nếu (has_mbyte)
	{
	    số_nguyên charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	không_thì
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    cho (p = buf + wildoff; p < s; ++p)
	nếu (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    cho (p = s; p < e; ++p)
	nếu (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    nếu (pat == NULL)
    {
	vim_free(buf);
	trả_về 0;
    }

    // compile the regexp into a program
    nếu (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    không_thì
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    nếu (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    nếu (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    nếu (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	trả_về 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    nếu (!didstar && stardepth < 100 && starstar && e - s == 2
							  && *path_end == '/')
    {
	vim_snprintf((char *)s, buflen - len, "%s", path_end + 1);
	++stardepth;
	(void)unix_expandpath(gap, buf, len, flags, TRUE);
	--stardepth;
    }

    // open the directory for scanning
    *s = NUL;
    dirp = opendir(*buf == NUL ? "." : (char *)buf);

    // Find all matching entries
    nếu (dirp != NULL)
    {
	trong_khi (!got_int)
	{
	    dp = readdir(dirp);
	    nếu (dp == NULL)
		break;
	    len = (size_t)(s - buf);
	    nếu ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != NULL && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((char *)s, buflen - len, "%s", dp->d_name);
		nếu (len + 1 >= buflen)
		    continue;

		nếu (starstar && stardepth < 100)
		{
		    // For "**" in the pattern first go deeper in the tree to
		    // find matches.
		    vim_snprintf((char *)buf + len, buflen - len,
							    "/**%s", path_end);
		    ++stardepth;
		    (void)unix_expandpath(gap, buf, len + 1, flags, TRUE);
		    --stardepth;
		}

		vim_snprintf((char *)buf + len, buflen - len, "%s", path_end);
		nếu (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    nếu (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		không_thì
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    nếu (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    nếu ((flags & EW_ALLLINKS) ? mch_lstat((char *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			nếu (precomp_buf)
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
    nếu (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   sizeof(char_u *), pstrcmp);
    trả_về matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    static số_nguyên
has_env_var(char_u *p)
{
    cho ( ; *p; MB_PTR_ADV(p))
    {
	nếu (*p == '\\' && p[1] != NUL)
	    ++p;
	không_thì nếu (vim_strchr((char_u *)
#nếu defined(MSWIN)
				    "$%"
#không_thì
				    "$"
#endif
					, *p) != NULL)
	    trả_về TRUE;
    }
    trả_về FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    static số_nguyên
has_special_wildchar(char_u *p)
{
    cho ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	nếu (*p == '\r' || *p == '\n')
	    break;
	// Allow for escaping.
	nếu (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	không_thì nếu (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != NULL)
	{
	    // A { must be followed by a matching }.
	    nếu (*p == '{' && vim_strchr(p, '}') == NULL)
		continue;
	    // A quote and backtick must be followed by another one.
	    nếu ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == NULL)
		continue;
	    trả_về TRUE;
	}
    }
    trả_về FALSE;
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
    số_nguyên
gen_expand_wildcards(
    số_nguyên		num_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    số_nguyên		*num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    số_nguyên		flags)		// EW_* flags
{
    số_nguyên			i;
    garray_T		ga;
    char_u		*p;
    static số_nguyên		recursive = FALSE;
    số_nguyên			add_pat;
    số_nguyên			retval = OK;
    số_nguyên			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    nếu (recursive)
#ifdef SPECIAL_WILDCHAR
	trả_về mch_expand_wildcards(num_pat, pat, num_file, file, flags);
#không_thì
	trả_về FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    cho (i = 0; i < num_pat; i++)
    {
	nếu (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    trả_về mch_expand_wildcards(num_pat, pat, num_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, sizeof(char_u *), 30);

    cho (i = 0; i < num_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	nếu (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    nếu (add_pat == -1)
		retval = FAIL;
	}
	không_thì
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    nếu ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		nếu (p == NULL)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		không_thì nếu (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(num_pat, pat, num_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    trả_về i;
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
	    nếu (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		nếu ((flags & (EW_PATH | EW_CDPATH))
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
		không_thì
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	nếu (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    nếu (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    không_thì
		addfile(&ga, t, flags);

	    nếu (t != p)
		vim_free(t);
	}

	nếu (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	nếu (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    nếu (retval == FAIL)
	ga_clear_strings(&ga);

    *num_file = ga.ga_len;
    *file = (ga.ga_data != NULL) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    trả_về ((flags & EW_EMPTYOK) || ga.ga_data != NULL) ? retval : FAIL;
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
    số_nguyên		flags)
{
    char_u	*p;
    số_nguyên		isdir;
    stat_T	sb;

#ifdef MSWIN
    nếu (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	nếu (perm < 0)
	    trả_về;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    không_thì
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	nếu (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((char *)f, &sb) < 0 : mch_getperm(f) < 0))
	    trả_về;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    nếu (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != NULL)
	trả_về;
#endif

    nếu ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	trả_về;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    nếu (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, NULL, !(flags & EW_SHELLCMD)))
	trả_về;

    // Make room for another item in the file list.
    nếu (ga_grow(gap, 1) == FAIL)
	trả_về;

    p = alloc(STRLEN(f) + 1 + isdir);
    nếu (p == NULL)
	trả_về;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    nếu (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    void
FreeWild(số_nguyên count, char_u **files)
{
    nếu (count <= 0 || files == NULL)
	trả_về;
    trong_khi (count--)
	vim_free(files[count]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    số_nguyên
pathcmp(const char *p, const char *q, số_nguyên maxlen)
{
    số_nguyên		i, j;
    số_nguyên		c1, c2;
    const char	*s = NULL;

    cho (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	nếu (c1 == NUL)
	{
	    nếu (c2 == NUL)  // full match
		trả_về 0;
	    s = q;
	    i = j;
	    break;
	}

	// End of "q": check if "p" just has a slash.
	nếu (c2 == NUL)
	{
	    s = p;
	    break;
	}

	nếu ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    nếu (vim_ispathsep(c1))
		trả_về -1;
	    nếu (vim_ispathsep(c2))
		trả_về 1;
	    trả_về p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    nếu (s == NULL)	// "i" or "j" ran into "maxlen"
	trả_về 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    nếu (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#không_thì
	    && c1 == '/'
#endif
       )
	trả_về 0;   // match with trailing slash
    nếu (s == q)
	trả_về -1;	    // no match
    trả_về 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    số_nguyên
vim_isAbsName(char_u *name)
{
    trả_về (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    số_nguyên
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    số_nguyên		len,
    số_nguyên		force)	    // force expansion even when already absolute
{
    số_nguyên		retval = OK;
    số_nguyên		url;

    *buf = NUL;
    nếu (fname == NULL)
	trả_về FAIL;

    url = path_with_url(fname);
    nếu (!url)
	retval = mch_FullName(fname, buf, len, force);
    nếu (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#nếu defined(MSWIN)
    slash_adjust(buf);
#endif
    trả_về retval;
}
