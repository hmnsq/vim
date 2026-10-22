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
    static ཧྲིལ
get_short_pathname(char_u **fnamep, char_u **bufp, size_t *fnamelen)
{
    ཧྲིལ		l, len;
    WCHAR	*newbuf;
    WCHAR	*wfname;

    len = MAXPATHL;
    newbuf = malloc(len * parimaa_na(*newbuf));
    གལ་སྲིད (newbuf == NULL)
	ལོག FAIL;

    wfname = enc_to_utf16(*fnamep, NULL);
    གལ་སྲིད (wfname == NULL)
    {
	vim_free(newbuf);
	ལོག FAIL;
    }

    l = GetShortPathNameW(wfname, newbuf, len);
    གལ་སྲིད (l > len - 1)
    {
	// If that doesn't work (not enough space), then save the string
	// and try again with a new buffer big enough.
	WCHAR *newbuf_t = newbuf;
	newbuf = vim_realloc(newbuf, (l + 1) * parimaa_na(*newbuf));
	གལ་སྲིད (newbuf == NULL)
	{
	    vim_free(wfname);
	    vim_free(newbuf_t);
	    ལོག FAIL;
	}
	// Really should always succeed, as the buffer is big enough.
	l = GetShortPathNameW(wfname, newbuf, l+1);
    }
    གལ་སྲིད (l != 0)
    {
	char_u *p = utf16_to_enc(newbuf, NULL);

	གལ་སྲིད (p != NULL)
	{
	    vim_free(*bufp);
	    *fnamep = *bufp = p;
	}
	དེ་མིན
	{
	    vim_free(wfname);
	    vim_free(newbuf);
	    ལོག FAIL;
	}
    }
    vim_free(wfname);
    vim_free(newbuf);

    *fnamelen = l == 0 ? l : STRLEN(*bufp);
    ལོག OK;
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
    static ཧྲིལ
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
    ཧྲིལ		retval = OK;

    // Make a copy
    old_len = *fnamelen;
    save_fname = vim_strnsave(*fname, old_len);
    གལ་སྲིད (save_fname == NULL)
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
    དོན་ལུ (;;)
    {
	// go back one path-separator
	སྐབས (endp > save_fname && !after_pathsep(save_fname, endp + 1))
	    --endp;
	གལ་སྲིད (endp <= save_fname)
	    bha_nga;		// processed the complete path

	/*
	 * Replace the path separator with a NUL and try to shorten the
	 * resulting path.
	 */
	ch = *endp;
	*endp = 0;
	short_fname = save_fname;
	len = STRLEN(short_fname) + 1;
	གལ་སྲིད (get_short_pathname(&short_fname, &pbuf_unused, &len) == FAIL)
	{
	    retval = FAIL;
	    goto theend;
	}
	*endp = ch;	// preserve the string

	གལ་སྲིད (len > 0)
	    bha_nga;	// successfully shortened the path

	// failed to shorten the path. Skip the path separator
	--endp;
    }

    གལ་སྲིད (len > 0)
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
	གལ་སྲིད (new_len > old_len)
	{
	    // There is not enough space in the currently allocated string,
	    // copy it to a buffer big enough.
	    *fname = *bufp = vim_strnsave(short_fname, new_len);
	    གལ་སྲིད (*fname == NULL)
	    {
		retval = FAIL;
		goto theend;
	    }
	}
	དེ་མིན
	{
	    // Transfer short_fname to the main buffer (it's big enough),
	    // unless get_short_pathname() did its work in-place.
	    *fname = *bufp = save_fname;
	    གལ་སྲིད (short_fname != save_fname)
		STRNCPY(save_fname, short_fname, len);
	    save_fname = NULL;
	}

	// concat the not-shortened part of the path
	གལ་སྲིད ((*fname + len) != endp)
	    vim_strncpy(*fname + len, endp, sfx_len);
	(*fname)[new_len] = NUL;
    }

theend:
    vim_free(pbuf_unused);
    vim_free(save_fname);

    ལོག retval;
}

/*
 * Get a pathname for a partial path.
 * Returns OK for success, FAIL for failure.
 */
    static ཧྲིལ
shortpath_for_partial(
    char_u	**fnamep,
    char_u	**bufp,
    size_t	*fnamelen)
{
    ཧྲིལ		sepcount;
    size_t	len, tflen;
    char_u	*p;
    char_u	*pbuf, *tfname;
    ཧྲིལ		hasTilde;

    // Count up the path separators from the RHS.. so we know which part
    // of the path to return.
    sepcount = 0;
    དོན་ལུ (p = *fnamep; p < *fnamep + *fnamelen; MB_PTR_ADV(p))
	གལ་སྲིད (vim_ispathsep(*p))
	    ++sepcount;

    // Need full path first (use expand_env() to remove a "~/")
    hasTilde = (**fnamep == '~');
    གལ་སྲིད (hasTilde)
	pbuf = tfname = expand_env_save(*fnamep);
    དེ་མིན
	pbuf = tfname = FullName_save(*fnamep, FALSE);
    གལ་སྲིད (tfname == NULL)
	ལོག FAIL;

    len = tflen = STRLEN(tfname);

    གལ་སྲིད (get_short_pathname(&tfname, &pbuf, &len) == FAIL)
	ལོག FAIL;

    གལ་སྲིད (len == 0)
    {
	// Don't have a valid filename, so shorten the rest of the
	// path if we can. This CAN give us invalid 8.3 filenames, but
	// there's not a lot of point in guessing what it might be.
	len = tflen;
	གལ་སྲིད (shortpath_for_invalid_fname(&tfname, &pbuf, &len) == FAIL)
	    ལོག FAIL;
    }

    // Count the paths backward to find the beginning of the desired string.
    དོན་ལུ (p = tfname + len - 1; p >= tfname; --p)
    {
	གལ་སྲིད (has_mbyte)
	    p -= mb_head_off(tfname, p);
	གལ་སྲིད (vim_ispathsep(*p))
	{
	    གལ་སྲིད (sepcount == 0 || (hasTilde && sepcount == 1))
		bha_nga;
	    དེ་མིན
		sepcount --;
	}
    }
    གལ་སྲིད (hasTilde)
    {
	--p;
	གལ་སྲིད (p >= tfname)
	    *p = '~';
	དེ་མིན
	    ལོག FAIL;
    }
    དེ་མིན
	++p;

    // Copy in the string - p indexes into tfname - allocated at pbuf
    vim_free(*bufp);
    *fnamelen = STRLEN(p);
    *bufp = pbuf;
    *fnamep = p;

    ལོག OK;
}
#endif // MSWIN

/*
 * Adjust a filename, according to a string of modifiers.
 * *fnamep must be NUL terminated when called.  When returning, the length is
 * determined by *fnamelen.
 * Returns VALID_ flags or -1 for failure.
 * When there is an error, *fnamep is set to NULL.
 */
    ཧྲིལ
modify_fname(
    char_u	*src,		// string with modifiers
    ཧྲིལ		tilde_file,	// "~" is a file name, not $HOME
    size_t	*usedlen,	// characters after src that are used
    char_u	**fnamep,	// file name so far
    char_u	**bufp,		// buffer for allocated file name or NULL
    size_t	*fnamelen)	// length of fnamep
{
    ཧྲིལ		valid = 0;
    char_u	*tail;
    char_u	*s, *p, *pbuf;
    char_u	dirname[MAXPATHL];
    ཧྲིལ		c;
    ཧྲིལ		has_fullname = 0;
    ཧྲིལ		has_homerelative = 0;
#ifdef MSWIN
    char_u	*fname_start = *fnamep;
    ཧྲིལ		has_shortname = 0;
#endif

repeat:
    // ":p" - full path/file_name
    གལ་སྲིད (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
    {
	has_fullname = 1;

	valid |= VALID_PATH;
	*usedlen += 2;

	// Expand "~/path" for all systems and "~user/path" for Unix and VMS
	གལ་སྲིད ((*fnamep)[0] == '~'
#གལ་སྲིད !defined(UNIX) && !(defined(VMS) && defined(USER_HOME))
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
	    གལ་སྲིད (*fnamep == NULL)
		ལོག -1;
	}

	// When "/." or "/.." is used: force expansion to get rid of it.
	དོན་ལུ (p = *fnamep; *p != NUL; MB_PTR_ADV(p))
	{
	    གལ་སྲིད (vim_ispathsep(*p)
		    && p[1] == '.'
		    && (p[2] == NUL
			|| vim_ispathsep(p[2])
			|| (p[2] == '.'
			    && (p[3] == NUL || vim_ispathsep(p[3])))))
		bha_nga;
	}

	// FullName_save() is slow, don't use it when not needed.
	གལ་སྲིད (*p != NUL || !vim_isAbsName(*fnamep)
#ifdef MSWIN	// enforce drive letter on Windows paths
		|| **fnamep == '/' || **fnamep == '\\'
#endif
	)
	{
	    *fnamep = FullName_save(*fnamep, *p != NUL);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    གལ་སྲིད (*fnamep == NULL)
		ལོག -1;
	}

#ifdef MSWIN
# གལ་སྲིད _WIN32_WINNT >= 0x0500
	གལ་སྲིད (vim_strchr(*fnamep, '~') != NULL)
	{
	    // Expand 8.3 filename to full path.  Needed to make sure the same
	    // file does not have two different names.
	    // Note: problem does not occur if _WIN32_WINNT < 0x0500.
	    WCHAR *wfname = enc_to_utf16(*fnamep, NULL);
	    WCHAR buf[_MAX_PATH];

	    གལ་སྲིད (wfname != NULL)
	    {
		གལ་སྲིད (GetLongPathNameW(wfname, buf, _MAX_PATH))
		{
		    char_u *q = utf16_to_enc(buf, NULL);

		    གལ་སྲིད (q != NULL)
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
	གལ་སྲིད (mch_isdir(*fnamep))
	{
	    // Make room for one or two extra characters.
	    *fnamep = vim_strnsave(*fnamep, STRLEN(*fnamep) + 2);
	    vim_free(*bufp);	// free any allocated file name
	    *bufp = *fnamep;
	    གལ་སྲིད (*fnamep == NULL)
		ལོག -1;
	    add_pathsep(*fnamep);
	}
    }

    // ":." - path relative to the current directory
    // ":~" - path relative to the home directory
    // ":8" - shortname path - postponed till after
    སྐབས (src[*usedlen] == ':'
		  && ((c = src[*usedlen + 1]) == '.' || c == '~' || c == '8'))
    {
	*usedlen += 2;
	གལ་སྲིད (c == '8')
	{
#ifdef MSWIN
	    has_shortname = 1; // Postpone this.
#endif
	    continue;
	}
	pbuf = NULL;
	// Need full path first (use expand_env() to remove a "~/")
	གལ་སྲིད (!has_fullname && !has_homerelative)
	{
	    གལ་སྲིད (**fnamep == '~')
		p = pbuf = expand_env_save(*fnamep);
	    དེ་མིན
		p = pbuf = FullName_save(*fnamep, FALSE);
	}
	དེ་མིན
	    p = *fnamep;

	has_fullname = 0;

	གལ་སྲིད (p != NULL)
	{
	    size_t  dirnamelen = 0;

	    གལ་སྲིད (c == '.')
	    {
		mch_dirname(dirname, MAXPATHL);
		གལ་སྲིད (has_homerelative)
		{
		    s = vim_strsave(dirname);
		    གལ་སྲིད (s != NULL)
		    {
			dirnamelen = home_replace(NULL, s, dirname, MAXPATHL, TRUE);
			vim_free(s);
		    }
		}

		གལ་སྲིད (dirnamelen == 0)
		    dirnamelen = STRLEN(dirname);

		// Do not call shorten_fname() here since it removes the prefix
		// even though the path does not have a prefix.
		གལ་སྲིད (fnamencmp(p, dirname, dirnamelen) == 0)
		{
		    p += dirnamelen;
		    གལ་སྲིད (vim_ispathsep(*p))
		    {
			སྐབས (*p && vim_ispathsep(*p))
			    ++p;
			*fnamep = p;
			གལ་སྲིད (pbuf != NULL)
			{
			    // free any allocated file name
			    vim_free(*bufp);
			    *bufp = pbuf;
			    pbuf = NULL;
			}
		    }
		}
	    }
	    དེ་མིན
	    {
		dirnamelen = home_replace(NULL, p, dirname, MAXPATHL, TRUE);
		// Only replace it when it starts with '~'
		གལ་སྲིད (*dirname == '~')
		{
		    s = vim_strnsave(dirname, dirnamelen);
		    གལ་སྲིད (s != NULL)
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
    སྐབས (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
    {
	valid |= VALID_HEAD;
	*usedlen += 2;
	s = get_past_head(*fnamep);
	སྐབས (tail > s && after_pathsep(s, tail))
	    MB_PTR_BACK(*fnamep, tail);
	*fnamelen = tail - *fnamep;
#ifdef VMS
	གལ་སྲིད (*fnamelen > 0)
	    *fnamelen += 1; // the path separator is part of the path
#endif
	གལ་སྲིད (*fnamelen == 0)
	{
	    // Result is empty.  Turn it into "." to make ":cd %:h" work.
	    p = vim_strsave((char_u *)".");
	    གལ་སྲིད (p == NULL)
		ལོག -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = tail = p;
	    *fnamelen = 1;
	}
	དེ་མིན
	{
	    སྐབས (tail > s && !after_pathsep(s, tail))
		MB_PTR_BACK(*fnamep, tail);
	}
    }

    // ":8" - shortname
    གལ་སྲིད (src[*usedlen] == ':' && src[*usedlen + 1] == '8')
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
    གལ་སྲིད (has_shortname)
    {
	// Copy the string if it is shortened by :h and when it wasn't copied
	// yet, because we are going to change it in place.  Avoids changing
	// the buffer name for "%:8".
	གལ་སྲིད (*fnamelen < STRLEN(*fnamep) || *fnamep == fname_start)
	{
	    p = vim_strnsave(*fnamep, *fnamelen);
	    གལ་སྲིད (p == NULL)
		ལོག -1;
	    vim_free(*bufp);
	    *bufp = *fnamep = p;
	}

	// Split into two implementations - makes it easier.  First is where
	// there isn't a full name already, second is where there is.
	གལ་སྲིད (!has_fullname && !vim_isAbsName(*fnamep))
	{
	    གལ་སྲིད (shortpath_for_partial(fnamep, bufp, fnamelen) == FAIL)
		ལོག -1;
	}
	དེ་མིན
	{
	    size_t	l = *fnamelen;

	    // Simple case, already have the full-name.
	    // Nearly always shorter, so try first time.
	    གལ་སྲིད (get_short_pathname(fnamep, bufp, &l) == FAIL)
		ལོག -1;

	    གལ་སྲིད (l == 0)
	    {
		// Couldn't find the filename, search the paths.
		l = *fnamelen;
		གལ་སྲིད (shortpath_for_invalid_fname(fnamep, bufp, &l) == FAIL)
		    ལོག -1;
	    }
	    *fnamelen = l;
	}

	// The name was replaced by the short name, "tail" points into the
	// previous name.
	tail = gettail(*fnamep);
    }
#endif // MSWIN

    // ":t" - tail, just the basename
    གལ་སྲིད (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
    {
	*usedlen += 2;
	*fnamelen -= tail - *fnamep;
	*fnamep = tail;
    }

    // ":e" - extension, can be repeated
    // ":r" - root, without extension, can be repeated
    སྐབས (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
    {
	// find a '.' in the tail:
	// - for second :e: before the current fname
	// - otherwise: The last '.'
	གལ་སྲིད (src[*usedlen + 1] == 'e' && *fnamep > tail)
	    s = *fnamep - 2;
	དེ་མིན
	    s = *fnamep + *fnamelen - 1;
	དོན་ལུ ( ; s > tail; --s)
	    གལ་སྲིད (s[0] == '.')
		bha_nga;
	གལ་སྲིད (src[*usedlen + 1] == 'e')		// :e
	{
	    གལ་སྲིད (s > tail)
	    {
		*fnamelen += (*fnamep - (s + 1));
		*fnamep = s + 1;
#ifdef VMS
		// cut version from the extension
		s = *fnamep + *fnamelen - 1;
		དོན་ལུ ( ; s > *fnamep; --s)
		    གལ་སྲིད (s[0] == ';')
			bha_nga;
		གལ་སྲིད (s > *fnamep)
		    *fnamelen = s - *fnamep;
#endif
	    }
	    དེ་མིན གལ་སྲིད (*fnamep <= tail)
		*fnamelen = 0;
	}
	དེ་མིན				// :r
	{
	    char_u *limit = *fnamep;

	    གལ་སྲིད (limit < tail)
		limit = tail;
	    གལ་སྲིད (s > limit)	// remove one extension
		*fnamelen = s - *fnamep;
	}
	*usedlen += 2;
    }

    // ":s?pat?foo?" - substitute
    // ":gs?pat?foo?" - global substitute
    གལ་སྲིད (src[*usedlen] == ':'
	    && (src[*usedlen + 1] == 's'
		|| (src[*usedlen + 1] == 'g' && src[*usedlen + 2] == 's')))
    {
	char_u	    *str;
	char_u	    *pat;
	char_u	    *sub;
	ཧྲིལ	    sep;
	char_u	    *flags;
	ཧྲིལ	    didit = FALSE;

	flags = (char_u *)"";
	s = src + *usedlen + 2;
	གལ་སྲིད (src[*usedlen + 1] == 'g')
	{
	    flags = (char_u *)"g";
	    ++s;
	}

	sep = *s++;
	གལ་སྲིད (sep)
	{
	    // find end of pattern
	    p = vim_strchr(s, sep);
	    གལ་སྲིད (p != NULL)
	    {
		pat = vim_strnsave(s, p - s);
		གལ་སྲིད (pat != NULL)
		{
		    s = p + 1;
		    // find end of substitution
		    p = vim_strchr(s, sep);
		    གལ་སྲིད (p != NULL)
		    {
			sub = vim_strnsave(s, p - s);
			str = vim_strnsave(*fnamep, *fnamelen);
			གལ་སྲིད (sub != NULL && str != NULL)
			{
			    size_t slen;

			    *usedlen = p + 1 - src;
			    s = do_string_sub(str, *fnamelen, pat, sub, NULL, flags, &slen);
			    གལ་སྲིད (s != NULL)
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
	    གལ་སྲིད (didit)
		goto repeat;
	}
    }

    གལ་སྲིད (src[*usedlen] == ':' && src[*usedlen + 1] == 'S')
    {
	// vim_strsave_shellescape() needs a NUL terminated string.
	c = (*fnamep)[*fnamelen];
	གལ་སྲིད (c != NUL)
	    (*fnamep)[*fnamelen] = NUL;
	p = vim_strsave_shellescape(*fnamep, FALSE, FALSE);
	གལ་སྲིད (c != NUL)
	    (*fnamep)[*fnamelen] = c;
	གལ་སྲིད (p == NULL)
	    ལོག -1;
	vim_free(*bufp);
	*bufp = *fnamep = p;
	*fnamelen = STRLEN(p);
	*usedlen += 2;
    }

    ལོག valid;
}

/*
 * Shorten the path of a file from "~/foo/../.bar/fname" to "~/f/../.b/fname"
 * "trim_len" specifies how many characters to keep for each directory.
 * Must be 1 or more.
 * It's done in-place.
 */
    static void
shorten_dir_len(char_u *str, ཧྲིལ trim_len)
{
    char_u	*tail, *s, *d;
    ཧྲིལ		skip = FALSE;
    ཧྲིལ		dirchunk_len = 0;

    tail = gettail(str);
    d = str;
    དོན་ལུ (s = str; ; ++s)
    {
	གལ་སྲིད (s >= tail)		    // copy the whole tail
	{
	    *d++ = *s;
	    གལ་སྲིད (*s == NUL)
		bha_nga;
	}
	དེ་མིན གལ་སྲིད (vim_ispathsep(*s))	    // copy '/' and next char
	{
	    *d++ = *s;
	    skip = FALSE;
	    dirchunk_len = 0;
	}
	དེ་མིན གལ་སྲིད (!skip)
	{
	    *d++ = *s;			// copy next char
	    གལ་སྲིད (*s != '~' && *s != '.') // and leading "~" and "."
	    {
		++dirchunk_len; // only count word chars for the size

		// keep copying chars until we have our preferred length (or
		// until the above if/else branches move us along)
		གལ་སྲིད (dirchunk_len >= trim_len)
		    skip = TRUE;
	    }

	    གལ་སྲིད (has_mbyte)
	    {
		ཧྲིལ l = mb_ptr2len(s);

		སྐབས (--l > 0)
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
    ཧྲིལ
file_is_readable(char_u *fname)
{
    ཧྲིལ		fd;

#ifndef O_NONBLOCK
# define O_NONBLOCK 0
#endif
    // Opening a file that does not exist is slow.
    གལ་སྲིད (*fname == NUL || mch_isdir_or_missing(fname))
	ལོག FALSE;
    གལ་སྲིད ((fd = mch_open((var_na *)fname, O_RDONLY | O_NONBLOCK, 0)) >= 0)
    {
	close(fd);
	ལོག TRUE;
    }
    ལོག FALSE;
}

#གལ་སྲིད defined(FEAT_EVAL)

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

    གལ་སྲིད (check_secure())
	ལོག;

    གལ་སྲིད (argvars[0].v_type != VAR_STRING)
    {
	// Returning an empty string means it failed.
	// No error message, for historic reasons.
	གལ་སྲིད (in_vim9script())
	    (void) check_for_string_arg(argvars, 0);
	ལོག;
    }

    // Return the current directory
    cwd = alloc(MAXPATHL);
    གལ་སྲིད (cwd != NULL)
    {
	གལ་སྲིད (mch_dirname(cwd, MAXPATHL) != FAIL)
	{
# ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(cwd);
# endif
	    rettv->vval.v_string = vim_strsave(cwd);
	}
	vim_free(cwd);
    }

    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *s = tv_get_string(&argvars[1]);
	གལ་སྲིད (STRCMP(s, "global") == 0)
	    scope = CDSCOPE_GLOBAL;
	དེ་མིན གལ་སྲིད (STRCMP(s, "tabpage") == 0)
	    scope = CDSCOPE_TABPAGE;
	དེ་མིན གལ་སྲིད (STRCMP(s, "window") == 0)
	    scope = CDSCOPE_WINDOW;
	དེ་མིན
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "scope", s);
	    ལོག;
	}
    }
    དེ་མིན གལ་སྲིད (curwin->w_localdir != NULL)
	scope = CDSCOPE_WINDOW;
    དེ་མིན གལ་སྲིད (curtab->tp_localdir != NULL)
	scope = CDSCOPE_TABPAGE;

    གལ་སྲིད (!changedir_func(argvars[0].vval.v_string, TRUE, scope))
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
    གལ་སྲིད (check_restricted() || check_secure())
	ལོག;

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ལོག;

    name = tv_get_string(&argvars[0]);
    གལ་སྲིད (name == NULL || *name == NUL)
    {
	emsg(_(e_invalid_argument));
	ལོག;
    }

    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN)
	flags = tv_get_string_buf(&argvars[1], nbuf);
    དེ་མིན
	flags = (char_u *)"";

    གལ་སྲིད (*flags == NUL)
	// delete a file
	rettv->vval.v_number = mch_remove(name) == 0 ? 0 : -1;
    དེ་མིན གལ་སྲིད (STRCMP(flags, "d") == 0)
	// delete an empty directory
	rettv->vval.v_number = mch_rmdir(name) == 0 ? 0 : -1;
    དེ་མིན གལ་སྲིད (STRCMP(flags, "rf") == 0)
	// delete a directory recursively
	rettv->vval.v_number = delete_recursive(name);
    དེ་མིན
	semsg(_(e_invalid_expression_str), flags);
}

/*
 * "executable()" function
 */
    void
f_executable(typval_T *argvars, typval_T *rettv)
{
    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

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

    གལ་སྲིད (in_vim9script() && check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ལོག;
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
    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;
    rettv->vval.v_number = file_is_readable(tv_get_string(&argvars[0]));
}

/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    void
f_filewritable(typval_T *argvars, typval_T *rettv)
{
    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;
    rettv->vval.v_number = filewritable(tv_get_string(&argvars[0]));
}

    static void
findfilendir(
    typval_T	*argvars,
    typval_T	*rettv,
    ཧྲིལ		find_what)
{
    char_u	*fname;
    char_u	*fresult = NULL;
    char_u	*path = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;
    char_u	*p;
    char_u	pathbuf[NUMBUFLEN];
    ཧྲིལ		count = 1;
    ཧྲིལ		first = TRUE;
    ཧྲིལ		error = FALSE;

    rettv->vval.v_string = NULL;
    rettv->v_type = VAR_STRING;
    གལ་སྲིད (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ལོག;

    fname = tv_get_string(&argvars[0]);

    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN)
    {
	p = tv_get_string_buf_chk(&argvars[1], pathbuf);
	གལ་སྲིད (p == NULL)
	    error = TRUE;
	དེ་མིན
	{
	    གལ་སྲིད (*p != NUL)
		path = p;

	    གལ་སྲིད (argvars[2].v_type != VAR_UNKNOWN)
		count = (ཧྲིལ)tv_get_number_chk(&argvars[2], &error);
	}
    }

    གལ་སྲིད (count < 0 && rettv_list_alloc(rettv) == FAIL)
	error = TRUE;

    གལ་སྲིད (*fname != NUL && !error)
    {
	char_u	*file_to_find = NULL;
	var_na	*search_ctx = NULL;

	do
	{
	    གལ་སྲིད (rettv->v_type == VAR_STRING || rettv->v_type == VAR_LIST)
		vim_free(fresult);
	    fresult = find_file_in_path_option(first ? fname : NULL,
					       first ? (ཧྲིལ)STRLEN(fname) : 0,
					0, first, path,
					find_what,
					curbuf->b_ffname,
					find_what == FINDFILE_DIR
					    ? (char_u *)"" : curbuf->b_p_sua,
					    &file_to_find, &search_ctx);
	    first = FALSE;

	    གལ་སྲིད (fresult != NULL && rettv->v_type == VAR_LIST)
		list_append_string(rettv->vval.v_list, fresult, -1);

	} སྐབས ((rettv->v_type == VAR_LIST || --count > 0) && fresult != NULL);

	vim_free(file_to_find);
	vim_findfile_cleanup(search_ctx);
    }

    གལ་སྲིད (rettv->v_type == VAR_STRING)
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

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ལོག;

    fname = tv_get_string_chk(&argvars[0]);
    mods = tv_get_string_buf_chk(&argvars[1], buf);
    གལ་སྲིད (mods == NULL || fname == NULL)
	fname = NULL;
    དེ་མིན
    {
	len = STRLEN(fname);
	གལ་སྲིད (mods != NULL && *mods != NUL)
	    (void)modify_fname(mods, FALSE, &usedlen, &fname, &fbuf, &len);
    }

    rettv->v_type = VAR_STRING;
    གལ་སྲིད (fname == NULL)
	rettv->vval.v_string = NULL;
    དེ་མིན
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
    ཧྲིལ		global = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    གལ་སྲིད (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	ལོག;

    གལ་སྲིད (argvars[0].v_type == VAR_NUMBER
	    && argvars[0].vval.v_number == -1
	    && argvars[1].v_type == VAR_UNKNOWN)
	global = TRUE;
    དེ་མིན
	wp = find_tabwin(&argvars[0], &argvars[1], &tp);

    གལ་སྲིད (wp != NULL && wp->w_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(wp->w_localdir);
    དེ་མིན གལ་སྲིད (tp != NULL && tp->tp_localdir != NULL
					   && argvars[0].v_type != VAR_UNKNOWN)
	rettv->vval.v_string = vim_strsave(tp->tp_localdir);
    དེ་མིན གལ་སྲིད (wp != NULL || tp != NULL || global)
    {
	གལ་སྲིད (globaldir != NULL && argvars[0].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(globaldir);
	དེ་མིན
	{
	    cwd = alloc(MAXPATHL);
	    གལ་སྲིད (cwd != NULL)
	    {
		གལ་སྲིད (mch_dirname(cwd, MAXPATHL) != FAIL)
		    rettv->vval.v_string = vim_strsave(cwd);
		vim_free(cwd);
	    }
	}
    }
# ifdef BACKSLASH_IN_FILENAME
    གལ་སྲིད (rettv->vval.v_string != NULL)
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
    ཧྲིལ		    i;

    དོན་ལུ (i = 0; i < 9; i++)
    {
	གལ་སྲིད (st->st_mode & (1 << (8 - i)))
	    perm[i] = flags[i % 3];
	དེ་མིན
	    perm[i] = '-';
    }
    ལོག perm;
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

    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    གལ་སྲིད (mch_stat((var_na *)fname, &st) >= 0)
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

    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    fname = tv_get_string(&argvars[0]);
    གལ་སྲིད (mch_stat((var_na *)fname, &st) >= 0)
    {
	གལ་སྲིད (S_ISDIR(st.st_mode))
	    rettv->vval.v_number = 0;
	དེ་མིན
	{
	    rettv->vval.v_number = (varnumber_T)st.st_size;

	    // non-perfect check for overflow
	    གལ་སྲིད ((off_T)rettv->vval.v_number != (off_T)st.st_size)
		rettv->vval.v_number = -2;
	}
    }
    དེ་མིན
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

    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    fname = tv_get_string(&argvars[0]);
    གལ་སྲིད (mch_stat((var_na *)fname, &st) >= 0)
	rettv->vval.v_number = (varnumber_T)st.st_mtime;
    དེ་མིན
	rettv->vval.v_number = -1;
}

/*
 * Convert "st" to file type string.
 */
    char_u *
getftypest(stat_T *st)
{
    var_na    *t;

    གལ་སྲིད (S_ISREG(st->st_mode))
	t = "file";
    དེ་མིན གལ་སྲིད (S_ISDIR(st->st_mode))
	t = "dir";
    དེ་མིན གལ་སྲིད (S_ISLNK(st->st_mode))
	t = "link";
    དེ་མིན གལ་སྲིད (S_ISBLK(st->st_mode))
	t = "bdev";
    དེ་མིན གལ་སྲིད (S_ISCHR(st->st_mode))
	t = "cdev";
    དེ་མིན གལ་སྲིད (S_ISFIFO(st->st_mode))
	t = "fifo";
    དེ་མིན གལ་སྲིད (S_ISSOCK(st->st_mode))
	t = "socket";
    དེ་མིན
	t = "other";
    ལོག (char_u*)t;
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

    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    fname = tv_get_string(&argvars[0]);

    rettv->v_type = VAR_STRING;
    གལ་སྲིད (mch_lstat((var_na *)fname, &st) >= 0)
	type = vim_strsave(getftypest(&st));
    rettv->vval.v_string = type;
}

/*
 * "glob()" function
 */
    void
f_glob(typval_T *argvars, typval_T *rettv)
{
    ཧྲིལ		options = WILD_SILENT|WILD_USE_NL;
    expand_T	xpc;
    ཧྲིལ		error = FALSE;

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
	ལོག;

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN)
    {
	གལ་སྲིད (tv_get_bool_chk(&argvars[1], &error))
	    options |= WILD_KEEP_ALL;
	གལ་སྲིད (argvars[2].v_type != VAR_UNKNOWN)
	{
	    གལ་སྲིད (tv_get_bool_chk(&argvars[2], &error))
		rettv_list_set(rettv, NULL);
	    གལ་སྲིད (argvars[3].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[3], &error))
		options |= WILD_ALLLINKS;
	}
    }
    གལ་སྲིད (!error)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	གལ་སྲིད (p_wic)
	    options += WILD_ICASE;
	གལ་སྲིད (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ExpandOne(&xpc, tv_get_string(&argvars[0]),
						     NULL, options, WILD_ALL);
	དེ་མིན གལ་སྲིད (rettv_list_alloc(rettv) == OK)
	{
	  ཧྲིལ i;

	  ExpandOne(&xpc, tv_get_string(&argvars[0]),
						NULL, options, WILD_ALL_KEEP);
	  དོན་ལུ (i = 0; i < xpc.xp_numfiles; i++)
	      list_append_string(rettv->vval.v_list, xpc.xp_files[i], -1);

	  ExpandCleanup(&xpc);
	}
    }
    དེ་མིན
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

    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

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
    ཧྲིལ		flags = WILD_IGNORE_COMPLETESLASH;
    char_u	buf1[NUMBUFLEN];
    char_u	*file;
    ཧྲིལ		error = FALSE;
    garray_T	ga;
    ཧྲིལ		i;

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 3) == FAIL
			|| (argvars[3].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 4) == FAIL)))))
	ལོག;

    file = tv_get_string_buf_chk(&argvars[1], buf1);

    // When the optional second argument is non-zero, don't remove matches
    // for 'wildignore' and don't put matches for 'suffixes' at the end.
    rettv->v_type = VAR_STRING;
    གལ་སྲིད (argvars[2].v_type != VAR_UNKNOWN)
    {
	གལ་སྲིད (tv_get_bool_chk(&argvars[2], &error))
	    flags |= WILD_KEEP_ALL;
	གལ་སྲིད (argvars[3].v_type != VAR_UNKNOWN)
	{
	    གལ་སྲིད (tv_get_bool_chk(&argvars[3], &error))
		rettv_list_set(rettv, NULL);
	    གལ་སྲིད (argvars[4].v_type != VAR_UNKNOWN
				    && tv_get_bool_chk(&argvars[4], &error))
		flags |= WILD_ALLLINKS;
	}
    }
    གལ་སྲིད (file != NULL && !error)
    {
	ga_init2(&ga, parimaa_na(char_u *), 10);
	globpath(tv_get_string(&argvars[0]), file, &ga, flags, FALSE);
	གལ་སྲིད (rettv->v_type == VAR_STRING)
	    rettv->vval.v_string = ga_concat_strings(&ga, "\n");
	དེ་མིན གལ་སྲིད (rettv_list_alloc(rettv) == OK)
	    དོན་ལུ (i = 0; i < ga.ga_len; ++i)
		list_append_string(rettv->vval.v_list,
					    ((char_u **)(ga.ga_data))[i], -1);
	ga_clear_strings(&ga);
    }
    དེ་མིན
	rettv->vval.v_string = NULL;
}

/*
 * "isdirectory()" function
 */
    void
f_isdirectory(typval_T *argvars, typval_T *rettv)
{
    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->vval.v_number = mch_isdir(tv_get_string(&argvars[0]));
}

/*
 * "isabsolutepath()" function
 */
    void
f_isabsolutepath(typval_T *argvars, typval_T *rettv)
{
    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->vval.v_number = mch_isFullName(tv_get_string_strict(&argvars[0]));
}

/*
 * Create the directory in which "dir" is located, and higher levels when
 * needed.
 * Set "created" to the full name of the first created directory.  It will be
 * NULL until that happens.
 * Return OK or FAIL.
 */
    static ཧྲིལ
mkdir_recurse(char_u *dir, ཧྲིལ prot, char_u **created)
{
    char_u	*p;
    char_u	*updir;
    ཧྲིལ		r = FAIL;

    // Get end of directory name in "dir".
    // We're done when it's "/" or "c:/".
    p = gettail_sep(dir);
    གལ་སྲིད (p <= get_past_head(dir))
	ལོག OK;

    // If the directory exists we're done.  Otherwise: create it.
    updir = vim_strnsave(dir, p - dir);
    གལ་སྲིད (updir == NULL)
	ལོག FAIL;
    གལ་སྲིད (mch_isdir(updir))
	r = OK;
    དེ་མིན གལ་སྲིད (mkdir_recurse(updir, prot, created) == OK)
    {
	r = vim_mkdir_emsg(updir, prot);
	གལ་སྲིད (r == OK && created != NULL && *created == NULL)
	    *created = FullName_save(updir, FALSE);
    }
    vim_free(updir);
    ལོག r;
}

/*
 * "mkdir()" function
 */
    void
f_mkdir(typval_T *argvars, typval_T *rettv)
{
    char_u	*dir;
    char_u	buf[NUMBUFLEN];
    ཧྲིལ		prot = 0755;
    ཧྲིལ		defer = FALSE;
    ཧྲིལ		defer_recurse = FALSE;
    char_u	*created = NULL;

    rettv->vval.v_number = FAIL;
    གལ་སྲིད (check_restricted() || check_secure())
	ལོག;

    གལ་སྲིད (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ལོག;

    dir = tv_get_string_buf(&argvars[0], buf);
    གལ་སྲིད (*dir == NUL)
	ལོག;

    གལ་སྲིད (*gettail(dir) == NUL)
	// remove trailing slashes
	*gettail_sep(dir) = NUL;

    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN)
    {
	char_u *arg2;

	གལ་སྲིད (argvars[2].v_type != VAR_UNKNOWN)
	{
	    prot = (ཧྲིལ)tv_get_number_chk(&argvars[2], NULL);
	    གལ་སྲིད (prot == -1)
		ལོག;
	}
	arg2 = tv_get_string(&argvars[1]);
	defer = vim_strchr(arg2, 'D') != NULL;
	defer_recurse = vim_strchr(arg2, 'R') != NULL;
	གལ་སྲིད ((defer || defer_recurse) && !can_add_defer())
	    ལོག;

	གལ་སྲིད (vim_strchr(arg2, 'p') != NULL)
	{
	    གལ་སྲིད (mch_isdir(dir))
	    {
		// With the "p" flag it's OK if the dir already exists.
		rettv->vval.v_number = OK;
		ལོག;
	    }
	    mkdir_recurse(dir, prot, defer || defer_recurse ? &created : NULL);
	}
    }
    rettv->vval.v_number = vim_mkdir_emsg(dir, prot);

    // Handle "D" and "R": deferred deletion of the created directory.
    གལ་སྲིད (rettv->vval.v_number == OK
				&& created == NULL && (defer || defer_recurse))
	created = FullName_save(dir, FALSE);
    གལ་སྲིད (created != NULL)
    {
	typval_T tv[2];

	tv[0].v_type = VAR_STRING;
	tv[0].v_lock = 0;
	tv[0].vval.v_string = created;
	tv[1].v_type = VAR_STRING;
	tv[1].v_lock = 0;
	tv[1].vval.v_string = vim_strsave(
				       (char_u *)(defer_recurse ? "rf" : "d"));
	གལ་སྲིད (tv[0].vval.v_string == NULL || tv[1].vval.v_string == NULL
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
    ཧྲིལ		trim_len = 1;

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ལོག;

    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN)
    {
	trim_len = (ཧྲིལ)tv_get_number(&argvars[1]);
	གལ་སྲིད (trim_len < 1)
	    trim_len = 1;
    }

    rettv->v_type = VAR_STRING;
    p = tv_get_string_chk(&argvars[0]);

    གལ་སྲིད (p == NULL)
	rettv->vval.v_string = NULL;
    དེ་མིན
    {
	p = vim_strsave(p);
	rettv->vval.v_string = p;
	གལ་སྲིད (p != NULL)
	    shorten_dir_len(p, trim_len);
    }
}

/*
 * Common code for readdir_checkitem() and readdirex_checkitem().
 * Either "name" or "dict" is NULL.
 */
    static ཧྲིལ
checkitem_common(void *context, char_u *name, dict_T *dict)
{
    typval_T	*expr = (typval_T *)context;
    typval_T	save_val;
    typval_T	rettv;
    typval_T	argv[2];
    ཧྲིལ		retval = 0;
    ཧྲིལ		error = FALSE;

    prepare_vimvar(VV_VAL, &save_val);
    གལ་སྲིད (name != NULL)
    {
	set_vim_var_string(VV_VAL, name, -1);
	argv[0].v_type = VAR_STRING;
	argv[0].vval.v_string = name;
    }
    དེ་མིན
    {
	set_vim_var_dict(VV_VAL, dict);
	argv[0].v_type = VAR_DICT;
	argv[0].vval.v_dict = dict;
    }

    གལ་སྲིད (eval_expr_typval(expr, FALSE, argv, 1, NULL, &rettv) == FAIL)
	goto theend;

    // We want to use -1, but also true/false should be allowed.
    གལ་སྲིད (rettv.v_type == VAR_SPECIAL || rettv.v_type == VAR_BOOL)
    {
	rettv.v_type = VAR_NUMBER;
	rettv.vval.v_number = rettv.vval.v_number == VVAL_TRUE;
    }
    retval = tv_get_number_chk(&rettv, &error);
    གལ་སྲིད (error)
	retval = -1;
    clear_tv(&rettv);

theend:
    གལ་སྲིད (name != NULL)
	set_vim_var_string(VV_VAL, NULL, 0);
    དེ་མིན
	set_vim_var_dict(VV_VAL, NULL);
    restore_vimvar(VV_VAL, &save_val);
    ལོག retval;
}

/*
 * Evaluate "expr" (= "context") for readdir().
 */
    static ཧྲིལ
readdir_checkitem(void *context, void *item)
{
    char_u	*name = (char_u *)item;

    ལོག checkitem_common(context, name, NULL);
}

/*
 * Process the keys in the Dict argument to the readdir() and readdirex()
 * functions.  Assumes the Dict argument is the 3rd argument.
 */
    static ཧྲིལ
readdirex_dict_arg(typval_T *argvars, ཧྲིལ *cmp)
{
    char_u     *compare;

    གལ་སྲིད (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	ལོག FAIL;

    གལ་སྲིད (dict_has_key(argvars[2].vval.v_dict, "sort"))
	compare = dict_get_string(argvars[2].vval.v_dict, "sort", FALSE);
    དེ་མིན
    {
	semsg(_(e_dictionary_key_str_required), "sort");
	ལོག FAIL;
    }

    གལ་སྲིད (STRCMP(compare, (char_u *) "none") == 0)
	*cmp = READDIR_SORT_NONE;
    དེ་མིན གལ་སྲིད (STRCMP(compare, (char_u *) "case") == 0)
	*cmp = READDIR_SORT_BYTE;
    དེ་མིན གལ་སྲིད (STRCMP(compare, (char_u *) "icase") == 0)
	*cmp = READDIR_SORT_IC;
    དེ་མིན གལ་སྲིད (STRCMP(compare, (char_u *) "collate") == 0)
	*cmp = READDIR_SORT_COLLATE;
    ལོག OK;
}

/*
 * "readdir()" function
 */
    void
f_readdir(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    ཧྲིལ		ret;
    char_u	*path;
    char_u	*p;
    garray_T	ga;
    ཧྲིལ		i;
    ཧྲིལ		sort = READDIR_SORT_BYTE;

    གལ་སྲིད (rettv_list_alloc(rettv) == FAIL)
	ལོག;
    གལ་སྲིད (check_secure())
	ལོག;

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ལོག;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ལོག;

    ret = readdir_core(&ga, path, FALSE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdir_checkitem, sort);
    གལ་སྲིད (ret == OK)
    {
	དོན་ལུ (i = 0; i < ga.ga_len; i++)
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
    static ཧྲིལ
readdirex_checkitem(void *context, void *item)
{
    dict_T	*dict = (dict_T*)item;

    ལོག checkitem_common(context, NULL, dict);
}

/*
 * "readdirex()" function
 */
    void
f_readdirex(typval_T *argvars, typval_T *rettv)
{
    typval_T	*expr;
    ཧྲིལ		ret;
    char_u	*path;
    garray_T	ga;
    ཧྲིལ		i;
    ཧྲིལ		sort = READDIR_SORT_BYTE;

    གལ་སྲིད (rettv_list_alloc(rettv) == FAIL)
	ལོག;
    གལ་སྲིད (check_secure())
	ལོག;

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ལོག;

    path = tv_get_string(&argvars[0]);
    expr = &argvars[1];

    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN && argvars[2].v_type != VAR_UNKNOWN &&
	    readdirex_dict_arg(argvars, &sort) == FAIL)
	ལོག;

    ret = readdir_core(&ga, path, TRUE, (void *)expr,
	    (expr->v_type == VAR_UNKNOWN) ? NULL : readdirex_checkitem, sort);
    གལ་སྲིད (ret == OK)
    {
	དོན་ལུ (i = 0; i < ga.ga_len; i++)
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
read_file_or_blob(typval_T *argvars, typval_T *rettv, ཧྲིལ always_blob)
{
    ཧྲིལ		binary = FALSE;
    ཧྲིལ		blob = always_blob;
    ཧྲིལ		failed = FALSE;
    char_u	*fname;
    FILE	*fd;
    char_u	buf[(IOSIZE/256)*256];	// rounded to avoid odd + 1
    ཧྲིལ		io_size = parimaa_na(buf);
    ཧྲིལ		readlen;		// size of last fread()
    char_u	*prev	 = NULL;	// previously read bytes, if any
    long	prevlen  = 0;		// length of data in prev
    long	prevsize = 0;		// size of prev buffer
    long	maxline  = MAXLNUM;
    long	cnt	 = 0;
    char_u	*p;			// position in buf
    char_u	*start;			// start of current line
    off_T	offset = 0;
    off_T	size = -1;

    གལ་སྲིད (argvars[1].v_type != VAR_UNKNOWN)
    {
	གལ་སྲིད (always_blob)
	{
	    offset = (off_T)tv_get_number(&argvars[1]);
	    གལ་སྲིད (argvars[2].v_type != VAR_UNKNOWN)
		size = (off_T)tv_get_number(&argvars[2]);
	}
	དེ་མིན
	{
	    གལ་སྲིད (STRCMP(tv_get_string(&argvars[1]), "b") == 0)
		binary = TRUE;
	    གལ་སྲིད (STRCMP(tv_get_string(&argvars[1]), "B") == 0)
		blob = TRUE;

	    གལ་སྲིད (argvars[2].v_type != VAR_UNKNOWN)
		maxline = (long)tv_get_number(&argvars[2]);
	}
    }

    གལ་སྲིད ((blob ? rettv_blob_alloc(rettv) : rettv_list_alloc(rettv)) == FAIL)
	ལོག;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    fname = tv_get_string(&argvars[0]);

    གལ་སྲིད (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ལོག;
    }
    གལ་སྲིད (*fname == NUL || (fd = mch_fopen((var_na *)fname, READBIN)) == NULL)
    {
	semsg(_(e_cant_open_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ལོག;
    }

    གལ་སྲིད (blob)
    {
	གལ་སྲིད (read_blob(fd, rettv, offset, size) == FAIL)
	    semsg(_(e_cant_read_file_str), fname);
	fclose(fd);
	ལོག;
    }

    སྐབས (cnt < maxline || maxline < 0)
    {
	readlen = (ཧྲིལ)fread(buf, 1, io_size, fd);

	// This for loop processes what was read, but is also entered at end
	// of file so that either:
	// - an incomplete line gets written
	// - a "binary" file gets an empty line at the end if it ends in a
	//   newline.
	དོན་ལུ (p = buf, start = buf;
		p < buf + readlen || (readlen <= 0 && (prevlen > 0 || binary));
		++p)
	{
	    གལ་སྲིད (readlen <= 0 || *p == '\n')
	    {
		listitem_T  *li;
		char_u	    *s	= NULL;
		long_u	    len = p - start;

		// Finished a line.  Remove CRs before NL.
		གལ་སྲིད (readlen > 0 && !binary)
		{
		    སྐབས (len > 0 && start[len - 1] == '\r')
			--len;
		    // removal may cross back to the "prev" string
		    གལ་སྲིད (len == 0)
			སྐབས (prevlen > 0 && prev[prevlen - 1] == '\r')
			    --prevlen;
		}
		གལ་སྲིད (prevlen == 0)
		    s = vim_strnsave(start, len);
		དེ་མིན
		{
		    // Change "prev" buffer to be the right size.  This way
		    // the bytes are only copied once, and very long lines are
		    // allocated only once.
		    གལ་སྲིད ((s = vim_realloc(prev, prevlen + len + 1)) != NULL)
		    {
			mch_memmove(s + prevlen, start, len);
			s[prevlen + len] = NUL;
			prev = NULL; // the list will own the string
			prevlen = prevsize = 0;
		    }
		}
		གལ་སྲིད (s == NULL)
		{
		    do_outofmem_msg((long_u) prevlen + len + 1);
		    failed = TRUE;
		    bha_nga;
		}

		གལ་སྲིད ((li = listitem_alloc()) == NULL)
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
		གལ་སྲིད ((++cnt >= maxline && maxline >= 0) || readlen <= 0)
		    bha_nga;
	    }
	    དེ་མིན གལ་སྲིད (*p == NUL)
		*p = '\n';
	    // Check for utf8 "bom"; U+FEFF is encoded as EF BB BF.  Do this
	    // when finding the BF and check the previous two bytes.
	    དེ་མིན གལ་སྲིད (*p == 0xbf && enc_utf8 && !binary)
	    {
		// Find the two bytes before the 0xbf.	If p is at buf, or buf
		// + 1, these may be in the "prev" string.
		char_u back1 = p >= buf + 1 ? p[-1]
				     : prevlen >= 1 ? prev[prevlen - 1] : NUL;
		char_u back2 = p >= buf + 2 ? p[-2]
			  : p == buf + 1 && prevlen >= 1 ? prev[prevlen - 1]
			  : prevlen >= 2 ? prev[prevlen - 2] : NUL;

		གལ་སྲིད (back2 == 0xef && back1 == 0xbb)
		{
		    char_u *dest = p - 2;

		    // Usually a BOM is at the beginning of a file, and so at
		    // the beginning of a line; then we can just step over it.
		    གལ་སྲིད (start == dest)
			start = p + 1;
		    དེ་མིན
		    {
			// have to shuffle buf to close gap
			ཧྲིལ adjust_prevlen = 0;

			གལ་སྲིད (dest < buf)
			{
			    // must be 1 or 2
			    adjust_prevlen = (ཧྲིལ)(buf - dest);
			    dest = buf;
			}
			གལ་སྲིད (readlen > p - buf + 1)
			    mch_memmove(dest, p + 1, readlen - (p - buf) - 1);
			readlen -= 3 - adjust_prevlen;
			prevlen -= adjust_prevlen;
			p = dest - 1;
		    }
		}
	    }
	} // for

	གལ་སྲིད (failed || (cnt >= maxline && maxline >= 0) || readlen <= 0)
	    bha_nga;
	གལ་སྲིད (start < p)
	{
	    // There's part of a line in buf, store it in "prev".
	    གལ་སྲིད (p - start + prevlen >= prevsize)
	    {
		// need bigger "prev" buffer
		char_u *newprev;

		// A common use case is ordinary text files and "prev" gets a
		// fragment of a line, so the first allocation is made
		// small, to avoid repeatedly 'allocing' large and
		// 'reallocing' small.
		གལ་སྲིད (prevsize == 0)
		    prevsize = (long)(p - start);
		དེ་མིན
		{
		    long grow50pc = (prevsize * 3) / 2;
		    long growmin  = (long)((p - start) * 2 + prevlen);
		    prevsize = grow50pc > growmin ? grow50pc : growmin;
		}
		newprev = vim_realloc(prev, prevsize);
		གལ་སྲིད (newprev == NULL)
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
    གལ་སྲིད (!failed && maxline < 0)
	སྐབས (cnt > -maxline)
	{
	    listitem_remove(rettv->vval.v_list, rettv->vval.v_list->lv_first);
	    --cnt;
	}

    གལ་སྲིད (failed)
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
    གལ་སྲིད (check_secure())
	ལོག;

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ལོག;

    read_file_or_blob(argvars, rettv, TRUE);
}

/*
 * "readfile()" function
 */
    void
f_readfile(typval_T *argvars, typval_T *rettv)
{
    གལ་སྲིད (check_secure())
	ལོག;

    གལ་སྲིད (in_vim9script()
	    && (check_for_nonempty_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 2) == FAIL)))
	ལོག;

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

    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    p = tv_get_string(&argvars[0]);
# ifdef FEAT_SHORTCUT
    {
	char_u	*v = NULL;

	v = mch_resolve_path(p, TRUE);
	གལ་སྲིད (v != NULL)
	    rettv->vval.v_string = v;
	དེ་མིན
	    rettv->vval.v_string = vim_strsave(p);
    }
# དེ་མིན
#  ifdef HAVE_READLINK
    {
	char_u	*cpy;
	ཧྲིལ	len;
	char_u	*remain = NULL;
	char_u	*q;
	ཧྲིལ	is_relative_to_current = FALSE;
	ཧྲིལ	has_trailing_pathsep = FALSE;
	ཧྲིལ	limit = 100;

	p = vim_strsave(p);
	གལ་སྲིད (p == NULL)
	    goto fail;
	གལ་སྲིད (p[0] == '.' && (vim_ispathsep(p[1])
				   || (p[1] == '.' && (vim_ispathsep(p[2])))))
	    is_relative_to_current = TRUE;

	len = STRLEN(p);
	གལ་སྲིད (len > 1 && after_pathsep(p, p + len))
	{
	    has_trailing_pathsep = TRUE;
	    p[len - 1] = NUL; // the trailing slash breaks readlink()
	}

	q = getnextcomp(p);
	གལ་སྲིད (*q != NUL)
	{
	    // Separate the first path component in "p", and keep the
	    // remainder (beginning with the path separator).
	    remain = vim_strsave(q - 1);
	    q[-1] = NUL;
	}

	buf = alloc(MAXPATHL + 1);
	གལ་སྲིད (buf == NULL)
	{
	    vim_free(p);
	    vim_free(remain);
	    goto fail;
	}

	དོན་ལུ (;;)
	{
	    དོན་ལུ (;;)
	    {
		len = readlink((var_na *)p, (var_na *)buf, MAXPATHL);
		གལ་སྲིད (len <= 0)
		    bha_nga;
		buf[len] = NUL;

		གལ་སྲིད (limit-- == 0)
		{
		    vim_free(p);
		    vim_free(remain);
		    emsg(_(e_too_many_symbolic_links_cycle));
		    rettv->vval.v_string = NULL;
		    goto fail;
		}

		// Ensure that the result will have a trailing path separator
		// if the argument has one.
		གལ་སྲིད (remain == NULL && has_trailing_pathsep)
		    add_pathsep(buf);

		// Separate the first path component in the link value and
		// concatenate the remainders.
		q = getnextcomp(vim_ispathsep(*buf) ? buf + 1 : buf);
		གལ་སྲིད (*q != NUL)
		{
		    གལ་སྲིད (remain == NULL)
			remain = vim_strsave(q - 1);
		    དེ་མིན
		    {
			cpy = concat_str(q - 1, remain);
			གལ་སྲིད (cpy != NULL)
			{
			    vim_free(remain);
			    remain = cpy;
			}
		    }
		    q[-1] = NUL;
		}

		q = gettail(p);
		གལ་སྲིད (q > p && *q == NUL)
		{
		    // Ignore trailing path separator.
		    p[q - p - 1] = NUL;
		    q = gettail(p);
		}
		གལ་སྲིད (q > p && !mch_isFullName(buf))
		{
		    // symlink is relative to directory of argument
		    cpy = alloc(STRLEN(p) + STRLEN(buf) + 1);
		    གལ་སྲིད (cpy != NULL)
		    {
			STRCPY(cpy, p);
			STRCPY(gettail(cpy), buf);
			vim_free(p);
			p = cpy;
		    }
		}
		དེ་མིན
		{
		    vim_free(p);
		    p = vim_strsave(buf);
		}
	    }

	    གལ་སྲིད (remain == NULL)
		bha_nga;

	    // Append the first path component of "remain" to "p".
	    q = getnextcomp(remain + 1);
	    len = q - remain - (*q != NUL);
	    cpy = vim_strnsave(p, STRLEN(p) + len);
	    གལ་སྲིད (cpy != NULL)
	    {
		STRNCAT(cpy, remain, len);
		vim_free(p);
		p = cpy;
	    }
	    // Shorten "remain".
	    གལ་སྲིད (*q != NUL)
		STRMOVE(remain, q - 1);
	    དེ་མིན
		VIM_CLEAR(remain);
	}

	// If the result is a relative path name, make it explicitly relative to
	// the current directory if and only if the argument had this form.
	གལ་སྲིད (!vim_ispathsep(*p))
	{
	    གལ་སྲིད (is_relative_to_current
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
		གལ་སྲིད (cpy != NULL)
		{
		    vim_free(p);
		    p = cpy;
		}
	    }
	    དེ་མིན གལ་སྲིད (!is_relative_to_current)
	    {
		// Strip leading "./".
		q = p;
		སྐབས (q[0] == '.' && vim_ispathsep(q[1]))
		    q += 2;
		གལ་སྲིད (q > p)
		    STRMOVE(p, p + 2);
	    }
	}

	// Ensure that the result will have no trailing path separator
	// if the argument had none.  But keep "/" or "//".
	གལ་སྲིད (!has_trailing_pathsep)
	{
	    q = p + STRLEN(p);
	    གལ་སྲིད (after_pathsep(p, q))
		*gettail_sep(p) = NUL;
	}

	rettv->vval.v_string = p;
    }
#  དེ་མིན
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
    static ཧྲིལ	x = 'A';

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = vim_tempname(x, FALSE);

    // Advance 'x' to use A-Z and 0-9, so that there are at least 34 different
    // names.  Skip 'I' and 'O', they are used for shell redirection.
    do
    {
	གལ་སྲིད (x == 'Z')
	    x = '0';
	དེ་མིན གལ་སྲིད (x == '9')
	    x = 'A';
	དེ་མིན
	    ++x;
    } སྐབས (x == 'I' || x == 'O');
}

/*
 * "writefile()" function
 */
    void
f_writefile(typval_T *argvars, typval_T *rettv)
{
    ཧྲིལ		binary = FALSE;
    ཧྲིལ		append = FALSE;
    ཧྲིལ		defer = FALSE;
# ifdef HAVE_FSYNC
    ཧྲིལ		do_fsync = p_fs;
# endif
    char_u	*fname;
    FILE	*fd;
    ཧྲིལ		ret = 0;
    listitem_T	*li;
    list_T	*list = NULL;
    blob_T	*blob = NULL;

    rettv->vval.v_number = -1;
    གལ་སྲིད (check_secure())
	ལོག;

    གལ་སྲིད (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ལོག;

    གལ་སྲིད (argvars[0].v_type == VAR_LIST)
    {
	list = argvars[0].vval.v_list;
	གལ་སྲིད (list == NULL)
	    ལོག;
	CHECK_LIST_MATERIALIZE(list);
	FOR_ALL_LIST_ITEMS(list, li)
	    གལ་སྲིད (tv_get_string_chk(&li->li_tv) == NULL)
		ལོག;
    }
    དེ་མིན གལ་སྲིད (argvars[0].v_type == VAR_BLOB)
    {
	blob = argvars[0].vval.v_blob;
	གལ་སྲིད (blob == NULL)
	    ལོག;
    }
    དེ་མིན
    {
	semsg(_(e_invalid_argument_str),
		_("writefile() first argument must be a List or a Blob"));
	ལོག;
    }

    གལ་སྲིད (argvars[2].v_type != VAR_UNKNOWN)
    {
	char_u *arg2 = tv_get_string_chk(&argvars[2]);

	གལ་སྲིད (arg2 == NULL)
	    ལོག;
	གལ་སྲིད (vim_strchr(arg2, 'b') != NULL)
	    binary = TRUE;
	གལ་སྲིད (vim_strchr(arg2, 'a') != NULL)
	    append = TRUE;
	གལ་སྲིད (vim_strchr(arg2, 'D') != NULL)
	    defer = TRUE;
# ifdef HAVE_FSYNC
	གལ་སྲིད (vim_strchr(arg2, 's') != NULL)
	    do_fsync = TRUE;
	དེ་མིན གལ་སྲིད (vim_strchr(arg2, 'S') != NULL)
	    do_fsync = FALSE;
# endif
    }

    fname = tv_get_string_chk(&argvars[1]);
    གལ་སྲིད (fname == NULL)
	ལོག;

    གལ་སྲིད (defer && !can_add_defer())
	ལོག;

    // Always open the file in binary mode, library functions have a mind of
    // their own about CR-LF conversion.
    གལ་སྲིད (*fname == NUL || (fd = mch_fopen((var_na *)fname,
				      append ? APPENDBIN : WRITEBIN)) == NULL)
    {
	semsg(_(e_cant_create_file_str),
			       *fname == NUL ? (char_u *)_("<empty>") : fname);
	ret = -1;
    }
    དེ་མིན
    {
	གལ་སྲིད (defer)
	{
	    typval_T tv;

	    tv.v_type = VAR_STRING;
	    tv.v_lock = 0;
	    tv.vval.v_string = FullName_save(fname, FALSE);
	    གལ་སྲིད (tv.vval.v_string == NULL
		    || add_defer((char_u *)"delete", 1, &tv) == FAIL)
	    {
		ret = -1;
		fclose(fd);
		(void)mch_remove(fname);
	    }
	}

	གལ་སྲིད (ret == 0)
	{
	    གལ་སྲིད (blob)
	    {
		གལ་སྲིད (write_blob(fd, blob) == FAIL)
		    ret = -1;
	    }
	    དེ་མིན
	    {
		གལ་སྲིད (write_list(fd, list, binary) == FAIL)
		    ret = -1;
	    }
# ifdef HAVE_FSYNC
	    གལ་སྲིད (ret == 0 && do_fsync)
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

#གལ་སྲིད defined(FEAT_BROWSE)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(
    ཧྲིལ		flags,		// BROWSE_SAVE and BROWSE_DIR
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
    ཧྲིལ			save_cmod_flags = cmdmod.cmod_flags;

    // Must turn off browse to avoid that autocommands will get the
    // flag too!
    cmdmod.cmod_flags &= ~CMOD_BROWSE;

    གལ་སྲིད (title == NULL || *title == NUL)
    {
	གལ་སྲིད (flags & BROWSE_DIR)
	    title = (char_u *)_("Select Directory dialog");
	དེ་མིན གལ་སྲིད (flags & BROWSE_SAVE)
	    title = (char_u *)_("Save File dialog");
	དེ་མིན
	    title = (char_u *)_("Open File dialog");
    }

    // When no directory specified, use default file name, default dir, buffer
    // dir, last dir or current dir
    གལ་སྲིད ((initdir == NULL || *initdir == NUL) && dflt != NULL && *dflt != NUL)
    {
	གལ་སྲིད (mch_isdir(dflt))		// default file name is a directory
	{
	    initdir = dflt;
	    dflt = NULL;
	}
	དེ་མིན གལ་སྲིད (gettail(dflt) != dflt)	// default file name includes a path
	{
	    tofree = vim_strsave(dflt);
	    གལ་སྲིད (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
		dflt = gettail(dflt);
	    }
	}
    }

    གལ་སྲིད (initdir == NULL || *initdir == NUL)
    {
	// When 'browsedir' is a directory, use it
	གལ་སྲིད (STRCMP(p_bsdir, "last") != 0
		&& STRCMP(p_bsdir, "buffer") != 0
		&& STRCMP(p_bsdir, "current") != 0
		&& mch_isdir(p_bsdir))
	    initdir = p_bsdir;
	// When saving or 'browsedir' is "buffer", use buffer fname
	དེ་མིན གལ་སྲིད (((flags & BROWSE_SAVE) || *p_bsdir == 'b')
		&& buf != NULL && buf->b_ffname != NULL)
	{
	    གལ་སྲིད (dflt == NULL || *dflt == NUL)
		dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    གལ་སྲིད (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	// When 'browsedir' is "last", use dir from last browse
	དེ་མིན གལ་སྲིད (*p_bsdir == 'l')
	    initdir = last_dir;
	// When 'browsedir is "current", use current directory.  This is the
	// default already, leave initdir empty.
    }

# ifdef FEAT_GUI
    གལ་སྲིད (gui.in_use)		// when this changes, also adjust f_has()!
    {
	གལ་སྲིད (filter == NULL
#  ifdef FEAT_EVAL
		&& (filter = get_var_value((char_u *)"b:browsefilter")) == NULL
		&& (filter = get_var_value((char_u *)"g:browsefilter")) == NULL
#  endif
	)
	    filter = BROWSE_FILTER_DEFAULT;
	གལ་སྲིད (flags & BROWSE_DIR)
	{
#  གལ་སྲིད defined(FEAT_GUI_GTK) || defined(MSWIN)
	    // For systems that have a directory dialog.
	    fname = gui_mch_browsedir(title, initdir);
#  དེ་མིན
	    // Generic solution for selecting a directory: select a file and
	    // remove the file name.
	    fname = gui_mch_browse(0, title, dflt, ext, initdir, (char_u *)"");
#  endif
#  གལ་སྲིད !defined(FEAT_GUI_GTK)
	    // Win32 adds a dummy file name, others return an arbitrary file
	    // name.  GTK+ 2 returns only the directory,
	    གལ་སྲིད (fname != NULL && *fname != NUL && !mch_isdir(fname))
	    {
		// Remove the file name.
		char_u	    *tail = gettail_sep(fname);

		གལ་སྲིད (tail == fname)
		    *tail++ = '.';	// use current dir
		*tail = NUL;
	    }
#  endif
	}
	དེ་མིན
	    fname = gui_mch_browse(flags & BROWSE_SAVE,
			       title, dflt, ext, initdir, (char_u *)_(filter));

	// We hang around in the dialog for a while, the user might do some
	// things to our files.  The Win32 dialog allows deleting or renaming
	// a file, check timestamps.
	need_check_timestamps = TRUE;
	did_check_timestamps = FALSE;
    }
    དེ་མིན
# endif
    {
	// TODO: non-GUI file selector here
	emsg(_(e_sorry_no_file_browser_in_console_mode));
	fname = NULL;
    }

    // keep the directory for next time
    གལ་སྲིད (fname != NULL)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	གལ་སྲིད (last_dir != NULL && !(flags & BROWSE_DIR))
	{
	    *gettail(last_dir) = NUL;
	    གལ་སྲིད (*last_dir == NUL)
	    {
		// filename only returned, must be in current dir
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		གལ་སྲིད (last_dir != NULL)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);
    cmdmod.cmod_flags = save_cmod_flags;

    ལོག fname;
}
#endif

#གལ་སྲིད defined(FEAT_EVAL)

/*
 * "browse(save, title, initdir, default)" function
 */
    void
f_browse(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_BROWSE
    ཧྲིལ		save;
    char_u	*title;
    char_u	*initdir;
    char_u	*defname;
    char_u	buf[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    ཧྲིལ		error = FALSE;

    གལ་སྲིད (in_vim9script()
	    && (check_for_bool_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL
		|| check_for_string_arg(argvars, 3) == FAIL))
	ལོག;

    save = (ཧྲིལ)tv_get_bool_chk(&argvars[0], &error);
    title = tv_get_string_chk(&argvars[1]);
    initdir = tv_get_string_buf_chk(&argvars[2], buf);
    defname = tv_get_string_buf_chk(&argvars[3], buf2);

    གལ་སྲིད (error || title == NULL || initdir == NULL || defname == NULL)
	rettv->vval.v_string = NULL;
    དེ་མིན
	rettv->vval.v_string =
		 do_browse(save ? BROWSE_SAVE : 0,
				 title, defname, NULL, initdir, NULL, curbuf);
# དེ་མིན
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

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ལོག;

    title = tv_get_string_chk(&argvars[0]);
    initdir = tv_get_string_buf_chk(&argvars[1], buf);

    གལ་སྲིད (title == NULL || initdir == NULL)
	rettv->vval.v_string = NULL;
    དེ་མིན
	rettv->vval.v_string = do_browse(BROWSE_DIR,
				    title, NULL, NULL, initdir, NULL, curbuf);
# དེ་མིན
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

    གལ་སྲིད (check_restricted() || check_secure()
	|| check_for_string_arg(argvars, 0) == FAIL
	|| check_for_string_arg(argvars, 1) == FAIL)
	ལོག;

    from = tv_get_string(&argvars[0]);

    གལ་སྲིད (mch_lstat((var_na *)from, &st) >= 0
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
    ཧྲིལ		dstlen,	// maximum length of the result
    ཧྲིལ		one)	// if TRUE, only replace one file name, include
			// spaces and commas in the file name.
{
    size_t	dirlen = 0, envlen = 0;
    size_t	len;
    char_u	*homedir_env, *homedir_env_orig;
    char_u	*p;
    char_u	*dst_start;

    གལ་སྲིད (src == NULL)
    {
	*dst = NUL;
	ལོག 0;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    གལ་སྲིད (buf != NULL && buf->b_help)
	ལོག vim_snprintf_safelen((var_na *)dst, dstlen, "%s", gettail(src));

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    གལ་སྲིད (homedir != NULL)
	dirlen = STRLEN(homedir);

#ifdef VMS
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"SYS$LOGIN");
#དེ་མིན
    homedir_env_orig = homedir_env = mch_getenv((char_u *)"HOME");
#endif
#ifdef MSWIN
    གལ་སྲིད (homedir_env == NULL)
	homedir_env_orig = homedir_env = mch_getenv((char_u *)"USERPROFILE");
#endif
    // Empty is the same as not set.
    གལ་སྲིད (homedir_env != NULL && *homedir_env == NUL)
	homedir_env = NULL;

    གལ་སྲིད (homedir_env != NULL && *homedir_env == '~')
    {
	size_t	usedlen = 0;
	size_t	flen;
	char_u	*fbuf = NULL;

	flen = STRLEN(homedir_env);
	(void)modify_fname((char_u *)":p", FALSE, &usedlen,
						  &homedir_env, &fbuf, &flen);
	flen = STRLEN(homedir_env);
	གལ་སྲིད (flen > 0 && vim_ispathsep(homedir_env[flen - 1]))
	    // Remove the trailing / that is added to a directory.
	    homedir_env[flen - 1] = NUL;
    }

    གལ་སྲིད (homedir_env != NULL)
	envlen = STRLEN(homedir_env);

    གལ་སྲིད (!one)
	src = skipwhite(src);
    dst_start = dst;		// remember the start
    སྐབས (*src && dstlen > 0)
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
	དོན་ལུ (;;)
	{
	    གལ་སྲིད (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || (!one && (src[len] == ',' || src[len] == ' '))
		    || src[len] == NUL))
	    {
		src += len;
		གལ་སྲིད (--dstlen > 0)
		    *dst++ = '~';

		// Do not add directory separator into dst, because dst is
		// expected to just return the directory name without the
		// directory separator '/'.
		bha_nga;
	    }
	    གལ་སྲིད (p == homedir_env)
		bha_nga;
	    p = homedir_env;
	    len = envlen;
	}

	// if (!one) skip to separator: space or comma
	སྐབས (*src && (one || (*src != ',' && *src != ' ')) && --dstlen > 0)
	    *dst++ = *src++;
	// skip separator
	སྐབས ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    // if (dstlen == 0) out of space, what to do???

    *dst = NUL;

    གལ་སྲིད (homedir_env != homedir_env_orig)
	vim_free(homedir_env);

    ལོག (size_t)(dst - dst_start);
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
    གལ་སྲིད (src != NULL)		// just in case
	len += (unsigned)STRLEN(src);
    dst = alloc(len);
    གལ་སྲིད (dst != NULL)
	home_replace(buf, src, dst, len, TRUE);
    ལོག dst;
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
    ཧྲིལ
fullpathcmp(
    char_u *s1,
    char_u *s2,
    ཧྲིལ	    checkname,		// when both don't exist, check file names
    ཧྲིལ	    expandenv)
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    stat_T	    st1, st2;
    ཧྲིལ		    r1, r2;

    གལ་སྲིད (expandenv)
	expand_env(s1, exp1, MAXPATHL);
    དེ་མིན
	vim_strncpy(exp1, s1, MAXPATHL - 1);
    r1 = mch_stat((var_na *)exp1, &st1);
    r2 = mch_stat((var_na *)s2, &st2);
    གལ་སྲིད (r1 != 0 && r2 != 0)
    {
	// if mch_stat() doesn't work, may compare the names
	གལ་སྲིད (checkname)
	{
	    གལ་སྲིད (fnamecmp(exp1, s2) == 0)
		ལོག FPC_SAMEX;
	    r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);
	    གལ་སྲིད (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		ལོག FPC_SAMEX;
	}
	ལོག FPC_NOTX;
    }
    གལ་སྲིད (r1 != 0 || r2 != 0)
	ལོག FPC_DIFFX;
    གལ་སྲིད (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	ལོག FPC_SAME;
    ལོག FPC_DIFF;
#དེ་མིན
    char_u  *exp1;		// expanded s1
    char_u  *full1;		// full path of s1
    char_u  *full2;		// full path of s2
    ཧྲིལ	    retval = FPC_DIFF;
    ཧྲིལ	    r1, r2;

    // allocate one buffer to store three paths (alloc()/free() is slow!)
    གལ་སྲིད ((exp1 = alloc(MAXPATHL * 3)) != NULL)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	གལ་སྲིད (expandenv)
	    expand_env(s1, exp1, MAXPATHL);
	དེ་མིན
	    vim_strncpy(exp1, s1, MAXPATHL - 1);
	r1 = vim_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = vim_FullName(s2, full2, MAXPATHL, FALSE);

	// If vim_FullName() fails, the file probably doesn't exist.
	གལ་སྲིད (r1 != OK && r2 != OK)
	{
	    གལ་སྲིད (checkname && fnamecmp(exp1, s2) == 0)
		retval = FPC_SAMEX;
	    དེ་མིན
		retval = FPC_NOTX;
	}
	དེ་མིན གལ་སྲིད (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	དེ་མིན གལ་སྲིད (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	དེ་མིན
	    retval = FPC_SAME;
	vim_free(exp1);
    }
    ལོག retval;
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

    གལ་སྲིད (fname == NULL)
	ལོག (char_u *)"";
    དོན་ལུ (p1 = p2 = get_past_head(fname); *p2; )	// find last part of path
    {
	གལ་སྲིད (vim_ispathsep_nocolon(*p2))
	    p1 = p2 + 1;
	MB_PTR_ADV(p2);
    }
    ལོག p1;
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
    སྐབས (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    ལོག t;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(char_u *fname)
{
    སྐབས (*fname && !vim_ispathsep(*fname))
	MB_PTR_ADV(fname);
    གལ་སྲིད (*fname)
	++fname;
    ལོག fname;
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

#གལ་སྲིད defined(MSWIN)
    // may skip "c:"
    གལ་སྲིད (SAFE_isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    དེ་མིན
	retval = path;
#དེ་མིན
# གལ་སྲིད defined(AMIGA)
    // may skip "label:"
    retval = vim_strchr(path, ':');
    གལ་སྲིད (retval == NULL)
	retval = path;
# དེ་མིན	// Unix
    retval = path;
# endif
#endif

    སྐབས (vim_ispathsep(*retval))
	++retval;

    ལོག retval;
}

/*
 * Return TRUE if 'c' is a path separator.
 * Note that for MS-Windows this includes the colon.
 */
    ཧྲིལ
vim_ispathsep(ཧྲིལ c)
{
#ifdef UNIX
    ལོག (c == '/');	    // UNIX has ':' inside file names
#དེ་མིན
# ifdef BACKSLASH_IN_FILENAME
    ལོག (c == ':' || c == '/' || c == '\\');
# དེ་མིན
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    ལོག (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  དེ་མིན
    ལོག (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}

/*
 * Like vim_ispathsep(c), but exclude the colon for MS-Windows.
 */
    ཧྲིལ
vim_ispathsep_nocolon(ཧྲིལ c)
{
    ལོག vim_ispathsep(c)
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
    ཧྲིལ
dir_of_file_exists(char_u *fname)
{
    char_u	*p;
    ཧྲིལ		c;
    ཧྲིལ		retval;

    p = gettail_sep(fname);
    གལ་སྲིད (p == fname)
	ལོག TRUE;
    c = *p;
    *p = NUL;
    retval = mch_isdir(fname);
    *p = c;
    ལོག retval;
}

/*
 * Versions of fnamecmp() and fnamencmp() that handle '/' and '\' equally
 * and deal with 'fileignorecase'.
 */
    ཧྲིལ
vim_fnamecmp(char_u *x, char_u *y)
{
#ifdef BACKSLASH_IN_FILENAME
    ལོག vim_fnamencmp(x, y, MAXPATHL);
#དེ་མིན
    གལ་སྲིད (p_fic)
	ལོག MB_STRICMP(x, y);
    ལོག STRCMP(x, y);
#endif
}

    ཧྲིལ
vim_fnamencmp(char_u *x, char_u *y, size_t len)
{
#ifdef BACKSLASH_IN_FILENAME
    char_u	*px = x;
    char_u	*py = y;
    ཧྲིལ		cx = NUL;
    ཧྲིལ		cy = NUL;

# ifdef MSWIN
    /*
     * To allow proper comparison of absolute paths:
     *	 - one with explicit drive letter C:\xxx
     *	 - another with implicit drive letter \xxx
     * advance the pointer, of the explicit one, to skip the drive
     */
    དོན་ལུ (ཧྲིལ swap = 0, drive = NUL; swap < 2; ++swap)
    {
	// Handle absolute paths with implicit drive letter
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);

	གལ་སྲིད ((cx == '/' || cx == '\\') && ASCII_ISALPHA(cy))
	{
	    drive = MB_TOUPPER(cy) - 'A' + 1;

	    // Check for the colon
	    py += mb_ptr2len(py);
	    cy = PTR2CHAR(py);
	    གལ་སྲིད (cy == ':' && drive == _getdrive())
	    { // skip the drive for comparison
		py += mb_ptr2len(py);
		bha_nga;
	    }
	    དེ་མིན // ignore
		py -= mb_ptr2len(py);
	}

	// swap pointers
	char_u *tmp = px;
	px = py;
	py = tmp;
    }
# endif

    སྐབས (len > 0)
    {
	cx = PTR2CHAR(px);
	cy = PTR2CHAR(py);
	གལ་སྲིད (cx == NUL || cy == NUL
	    || ((p_fic ? MB_TOLOWER(cx) != MB_TOLOWER(cy) : cx != cy)
		&& !(cx == '/' && cy == '\\')
		&& !(cx == '\\' && cy == '/')))
	    bha_nga;
	len -= mb_ptr2len(px);
	px += mb_ptr2len(px);
	py += mb_ptr2len(py);
    }
    གལ་སྲིད (len == 0)
	ལོག 0;
    ལོག (cx - cy);
#དེ་མིན
    གལ་སྲིད (p_fic)
	ལོག MB_STRNICMP(x, y, len);
    ལོག STRNCMP(x, y, len);
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is necessary.
 */
    char_u  *
concat_fnames(char_u *fname1, size_t fname1len, char_u *fname2, size_t fname2len, ཧྲིལ sep, string_T *ret)
{
    ret->string = alloc(fname1len + (sep ? parimaa_na(PATHSEP) : 0) + fname2len + 1);
    གལ་སྲིད (ret->string == NULL)
	ret->length = 0;
    དེ་མིན
    {
	STRCPY(ret->string, fname1);
	ret->length = fname1len;
	གལ་སྲིད (sep && *ret->string != NUL && !after_pathsep(ret->string, ret->string + ret->length))
	{
	    STRCPY(ret->string + ret->length, PATHSEPSTR);
	    ret->length += parimaa_na(PATHSEP);
	}
	STRCPY(ret->string + ret->length, fname2);
	ret->length += fname2len;
    }

    ལོག ret->string;
}

/*
 * Add a path separator to a file name, unless it already ends in a path
 * separator.
 */
    void
add_pathsep(char_u *p)
{
    size_t  plen;

    གལ་སྲིད (*p == NUL)
	ལོག;

    plen = STRLEN(p);
    གལ་སྲིད (!after_pathsep(p, p + plen))
	STRCPY(p + plen, PATHSEPSTR);
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(
    char_u	*fname,
    ཧྲིལ		force)		// force expansion, even when it already looks
				// like a full path name
{
    char_u	*buf;
    char_u	*new_fname = NULL;

    གལ་སྲིད (fname == NULL)
	ལོག NULL;

    buf = alloc(MAXPATHL);
    གལ་སྲིད (buf == NULL)
	ལོག NULL;

    གལ་སྲིད (vim_FullName(fname, buf, MAXPATHL, force) != FAIL)
	new_fname = vim_strsave(buf);
    དེ་མིན
	new_fname = vim_strsave(fname);
    vim_free(buf);
    ལོག new_fname;
}

/*
 * return TRUE if "fname" exists.
 */
    ཧྲིལ
vim_fexists(char_u *fname)
{
    stat_T st;

    གལ་སྲིད (mch_stat((var_na *)fname, &st))
	ལོག FALSE;
    ལོག TRUE;
}

/*
 * Invoke expand_wildcards() for one pattern.
 * Expand items like "%:h" before the expansion.
 * Returns OK or FAIL.
 */
    ཧྲིལ
expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    ཧྲིལ		  *num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ཧྲིལ		   flags)	// EW_DIR, etc.
{
    ཧྲིལ		ret = FAIL;
    char_u	*eval_pat = NULL;
    char_u	*exp_pat = *pat;
    var_na	*ignored_msg;
    size_t	usedlen;
    ཧྲིལ		is_cur_alt_file = *exp_pat == '%' || *exp_pat == '#';
    ཧྲིལ		star_follows = FALSE;

    གལ་སྲིད (is_cur_alt_file || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
					       NULL, &ignored_msg, NULL, TRUE);
	--emsg_off;
	གལ་སྲིད (eval_pat != NULL)
	{
	    star_follows = STRCMP(exp_pat + usedlen, "*") == 0;
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
	}
    }

    གལ་སྲིད (exp_pat != NULL)
	ret = expand_wildcards(1, &exp_pat, num_file, file, flags);

    གལ་སྲིད (eval_pat != NULL)
    {
	གལ་སྲིད (*num_file == 0 && is_cur_alt_file && star_follows)
	{
	    // Expanding "%" or "#" and the file does not exist: Add the
	    // pattern anyway (without the star) so that this works for remote
	    // files and non-file buffer names.
	    *file = ALLOC_ONE(char_u *);
	    གལ་སྲིད (*file != NULL)
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

    ལོག ret;
}

/*
 * Expand wildcards.  Calls gen_expand_wildcards() and removes files matching
 * 'wildignore'.
 * Returns OK or FAIL.  When FAIL then "num_files" won't be set.
 */
    ཧྲིལ
expand_wildcards(
    ཧྲིལ		   num_pat,	// number of input patterns
    char_u	 **pat,		// array of input patterns
    ཧྲིལ		  *num_files,	// resulting number of files
    char_u	***files,	// array of resulting files
    ཧྲིལ		   flags)	// EW_DIR, etc.
{
    ཧྲིལ		retval;
    ཧྲིལ		i, j;
    char_u	*p;
    ཧྲིལ		non_suf_match;	// number without matching suffix

    retval = gen_expand_wildcards(num_pat, pat, num_files, files, flags);

    // When keeping all matches, return here
    གལ་སྲིད ((flags & EW_KEEPALL) || retval == FAIL)
	ལོག retval;

    /*
     * Remove names that match 'wildignore'.
     */
    གལ་སྲིད (*p_wig)
    {
	char_u	*ffname;

	// check all files in (*files)[]
	དོན་ལུ (i = 0; i < *num_files; ++i)
	{
	    ffname = FullName_save((*files)[i], FALSE);
	    གལ་སྲིད (ffname == NULL)		// out of memory
		bha_nga;
#ifdef VMS
	    vms_remove_version(ffname);
#endif
	    གལ་སྲིད (match_file_list(p_wig, (*files)[i], ffname))
	    {
		// remove this matching file from the list
		vim_free((*files)[i]);
		དོན་ལུ (j = i; j + 1 < *num_files; ++j)
		    (*files)[j] = (*files)[j + 1];
		--*num_files;
		--i;
	    }
	    vim_free(ffname);
	}

	// If the number of matches is now zero, we fail.
	གལ་སྲིད (*num_files == 0)
	{
	    VIM_CLEAR(*files);
	    ལོག FAIL;
	}
    }

    /*
     * Move the names where 'suffixes' match to the end.
     * Skip when interrupted, the result probably won't be used.
     */
    གལ་སྲིད (*num_files > 1 && !got_int)
    {
	non_suf_match = 0;
	དོན་ལུ (i = 0; i < *num_files; ++i)
	{
	    གལ་སྲིད (!match_suffix((*files)[i]))
	    {
		/*
		 * Move the name without matching suffix to the front
		 * of the list.
		 */
		p = (*files)[i];
		དོན་ལུ (j = i; j > non_suf_match; --j)
		    (*files)[j] = (*files)[j - 1];
		(*files)[non_suf_match++] = p;
	    }
	}
    }

    ལོག retval;
}

/*
 * Return TRUE if "fname" matches with an entry in 'suffixes'.
 */
    ཧྲིལ
match_suffix(char_u *fname)
{
    ཧྲིལ		fnamelen, setsuflen;
    char_u	*setsuf;
#define MAXSUFLEN 30	    // maximum length of a file suffix
    char_u	suf_buf[MAXSUFLEN];

    fnamelen = (ཧྲིལ)STRLEN(fname);
    setsuflen = 0;
    དོན་ལུ (setsuf = p_su; *setsuf; )
    {
	setsuflen = copy_option_part(&setsuf, suf_buf, MAXSUFLEN, ".,");
	གལ་སྲིད (setsuflen == 0)
	{
	    char_u *tail = gettail(fname);

	    // empty entry: match name without a '.'
	    གལ་སྲིད (vim_strchr(tail, '.') == NULL)
	    {
		setsuflen = 1;
		bha_nga;
	    }
	}
	དེ་མིན
	{
	    གལ་སྲིད (fnamelen >= setsuflen
		    && fnamencmp(suf_buf, fname + fnamelen - setsuflen,
						  (size_t)setsuflen) == 0)
		bha_nga;
	    setsuflen = 0;
	}
    }
    ལོག (setsuflen != 0);
}

/*
 * Return TRUE if we can expand this backtick thing here.
 */
    static ཧྲིལ
vim_backtick(char_u *p)
{
    ལོག (*p == '`' && *(p + 1) != NUL && *(p + STRLEN(p) - 1) == '`');
}

/*
 * Expand an item in `backticks` by executing it as a command.
 * Currently only works when pat[] starts and ends with a `.
 * Returns number of file names found, -1 if an error is encountered.
 */
    static ཧྲིལ
expand_backtick(
    garray_T	*gap,
    char_u	*pat,
    ཧྲིལ		flags)	// EW_* flags
{
    char_u	*p;
    char_u	*cmd;
    char_u	*buffer;
    ཧྲིལ		cnt = 0;
    ཧྲིལ		i;

    // Create the command: lop off the backticks.
    cmd = vim_strnsave(pat + 1, STRLEN(pat) - 2);
    གལ་སྲིད (cmd == NULL)
	ལོག -1;

#ifdef FEAT_EVAL
    གལ་སྲིད (*cmd == '=')	    // `={expr}`: Expand expression
	buffer = eval_to_string(cmd + 1, TRUE, FALSE);
    དེ་མིན
#endif
	buffer = get_cmd_output(cmd, NULL,
				(flags & EW_SILENT) ? SHELL_SILENT : 0, NULL);
    vim_free(cmd);
    གལ་སྲིད (buffer == NULL)
	ལོག -1;

    cmd = buffer;
    སྐབས (*cmd != NUL)
    {
	cmd = skipwhite(cmd);		// skip over white space
	p = cmd;
	སྐབས (*p != NUL && *p != '\r' && *p != '\n') // skip over entry
	    ++p;
	// add an entry if it is not empty
	གལ་སྲིད (p > cmd)
	{
	    i = *p;
	    *p = NUL;
	    addfile(gap, cmd, flags);
	    *p = i;
	    ++cnt;
	}
	cmd = p;
	སྐབས (*cmd != NUL && (*cmd == '\r' || *cmd == '\n'))
	    ++cmd;
    }

    vim_free(buffer);
    ལོག cnt;
}

#གལ་སྲིད defined(MSWIN)
/*
 * File name expansion code for MS-DOS, Win16 and Win32.  It's here because
 * it's shared between these systems.
 */

/*
 * comparison function for qsort in dos_expandpath()
 */
    static ཧྲིལ
pstrcmp(const void *a, const void *b)
{
    ལོག (pathcmp(*(var_na **)a, *(var_na **)b, -1));
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
    ཧྲིལ
dos_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ཧྲིལ		flags,		// EW_* flags
    ཧྲིལ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ཧྲིལ		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ཧྲིལ		starts_with_dot;
    ཧྲིལ		matches;
    size_t	buflen;
    size_t	len;
    ཧྲིལ		starstar = FALSE;
    static ཧྲིལ	stardepth = 0;	    // depth for "**" expansion
    HANDLE		hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    wfb;
    WCHAR		*wn = NULL;	// UCS-2 name, NULL when not used.
    char_u		*matchname;
    ཧྲིལ			ok;
    char_u		*p_alt;
    ཧྲིལ			use_short_name;

    // Expanding "**" may take a long time, check for CTRL-C.
    གལ་སྲིད (stardepth > 0)
    {
	ui_breakcheck();
	གལ་སྲིད (got_int)
	    ལོག 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    གལ་སྲིད (buf == NULL)
	ལོག 0;

    /*
     * Find the first part in the path name that contains a wildcard or a ~1.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    སྐབས (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	གལ་སྲིད (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	དེ་མིན གལ་སྲིད (*path_end == '\\' || *path_end == ':' || *path_end == '/')
	{
	    གལ་སྲིད (e != NULL)
		bha_nga;
	    s = p + 1;
	}
	དེ་མིན གལ་སྲིད (path_end >= path + wildoff
			 && vim_strchr((char_u *)"*?[~", *path_end) != NULL)
	    e = p;
	གལ་སྲིད (has_mbyte)
	{
	    ཧྲིལ charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	དེ་མིན
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // now we have one wildcard component between s and e
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    དོན་ལུ (p = buf + wildoff; p < s; ++p)
	གལ་སྲིད (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    དོན་ལུ (p = s; p < e; ++p)
	གལ་སྲིད (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    གལ་སྲིད (pat == NULL)
    {
	vim_free(buf);
	ལོག 0;
    }

    // compile the regexp into a program
    གལ་སྲིད (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.rm_ic = TRUE;		// Always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    གལ་སྲིད (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    གལ་སྲིད (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ལོག 0;
    }

    // remember the pattern or file name being looked for
    matchname = vim_strsave(s);

    // Only match against the alternate (8.3) file name when the pattern looks
    // like a short name, it contains a '~'.
    use_short_name = vim_strchr(s, '~') != NULL;

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    གལ་སྲིད (!didstar && stardepth < 100 && starstar && e - s == 2
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
    གལ་སྲིད (wn != NULL)
	hFind = FindFirstFileW(wn, &wfb);
    ok = (hFind != INVALID_HANDLE_VALUE);

    སྐབས (ok)
    {
	p = utf16_to_enc(wfb.cFileName, NULL);   // p is allocated here

	གལ་སྲིད (p == NULL)
	    bha_nga;  // out of memory

	// Do not use the alternate filename when the file name ends in '~',
	// because it picks up backup files: short name for "foo.vim~" is
	// "foo~1.vim", which matches "*.vim".
	གལ་སྲིད (!use_short_name || *wfb.cAlternateFileName == NUL
						  || p[STRLEN(p) - 1] == '~')
	    p_alt = NULL;
	དེ་མིན
	    p_alt = utf16_to_enc(wfb.cAlternateFileName, NULL);

	len = (size_t)(s - buf);
	// Ignore entries starting with a dot, unless when asked for.  Accept
	// all entries found with "matchname".
	གལ་སྲིད ((p[0] != '.' || starts_with_dot
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
	    གལ་སྲིད (len + 1 < buflen)
	    {
		གལ་སྲིད (starstar && stardepth < 100
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
		གལ་སྲིད (mch_has_exp_wildcard(path_end))
		{
		    གལ་སྲིད (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)dos_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		དེ་མིན
		{
		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    གལ་སྲིད (*path_end != 0)
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
    གལ་སྲིད (matches > 0)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   parimaa_na(char_u *), pstrcmp);
    ལོག matches;
}
#endif // MSWIN

#གལ་སྲིད (defined(UNIX) && !defined(VMS)) || defined(USE_UNIXFILENAME)
/*
 * Unix style wildcard expansion code.
 * It's here because it's used both for Unix and Mac.
 */
    static ཧྲིལ
pstrcmp(const void *a, const void *b)
{
    ལོག (pathcmp(*(var_na **)a, *(var_na **)b, -1));
}

/*
 * Recursively expand one path component into all matching files and/or
 * directories.  Adds matches to "gap".  Handles "*", "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "path + wildoff".
 * Return the number of matches found.
 * NOTE: much of this is identical to dos_expandpath(), keep in sync!
 */
    ཧྲིལ
unix_expandpath(
    garray_T	*gap,
    char_u	*path,
    size_t	wildoff,
    ཧྲིལ		flags,		// EW_* flags
    ཧྲིལ		didstar)	// expanded "**" once already
{
    char_u	*buf;
    char_u	*path_end;
    char_u	*p, *s, *e;
    ཧྲིལ		start_len = gap->ga_len;
    char_u	*pat;
    regmatch_T	regmatch;
    ཧྲིལ		starts_with_dot;
    ཧྲིལ		matches;
    size_t	buflen;
    size_t	len;
    ཧྲིལ		starstar = FALSE;
    static ཧྲིལ	stardepth = 0;	    // depth for "**" expansion

    DIR		*dirp;
    sa_nracanaa dirent *dp;

    // Expanding "**" may take a long time, check for CTRL-C.
    གལ་སྲིད (stardepth > 0)
    {
	ui_breakcheck();
	གལ་སྲིད (got_int)
	    ལོག 0;
    }

    // Make room for file name (a bit too much to stay on the safe side).
    buflen = STRLEN(path) + MAXPATHL;
    buf = alloc(buflen);
    གལ་སྲིད (buf == NULL)
	ལོག 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * When EW_ICASE is set every letter is considered to be a wildcard.
     * Copy it into "buf", including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    path_end = path;
    སྐབས (*path_end != NUL)
    {
	// May ignore a wildcard that has a backslash before it; it will
	// be removed by rem_backslash() or file_pat_to_reg_pat() below.
	གལ་སྲིད (path_end >= path + wildoff && rem_backslash(path_end))
	    *p++ = *path_end++;
	དེ་མིན གལ་སྲིད (*path_end == '/')
	{
	    གལ་སྲིད (e != NULL)
		bha_nga;
	    s = p + 1;
	}
	དེ་མིན གལ་སྲིད (path_end >= path + wildoff
			 && (vim_strchr((char_u *)"*?[{~$", *path_end) != NULL
			     || (!p_fic && (flags & EW_ICASE)
					  && vim_isalpha(PTR2CHAR(path_end)))))
	    e = p;
	གལ་སྲིད (has_mbyte)
	{
	    ཧྲིལ charlen = (*mb_ptr2len)(path_end);

	    STRNCPY(p, path_end, (size_t)charlen);
	    p += charlen;
	    path_end += charlen;
	}
	དེ་མིན
	    *p++ = *path_end++;
    }
    e = p;
    *e = NUL;

    // Now we have one wildcard component between "s" and "e".
    // Remove backslashes between "wildoff" and the start of the wildcard
    // component.
    དོན་ལུ (p = buf + wildoff; p < s; ++p)
	གལ་སྲིད (rem_backslash(p))
	{
	    STRMOVE(p, p + 1);
	    --e;
	    --s;
	}

    // Check for "**" between "s" and "e".
    དོན་ལུ (p = s; p < e; ++p)
	གལ་སྲིད (p[0] == '*' && p[1] == '*')
	    starstar = TRUE;

    // convert the file pattern to a regexp pattern
    starts_with_dot = *s == '.';
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    གལ་སྲིད (pat == NULL)
    {
	vim_free(buf);
	ལོག 0;
    }

    // compile the regexp into a program
    གལ་སྲིད (flags & EW_ICASE)
	regmatch.rm_ic = TRUE;		// 'wildignorecase' set
    དེ་མིན
	regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    གལ་སྲིད (flags & (EW_NOERROR | EW_NOTWILD))
	++emsg_silent;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC);
    གལ་སྲིད (flags & (EW_NOERROR | EW_NOTWILD))
	--emsg_silent;
    vim_free(pat);

    གལ་སྲིད (regmatch.regprog == NULL && (flags & EW_NOTWILD) == 0)
    {
	vim_free(buf);
	ལོག 0;
    }

    len = (size_t)(s - buf);
    // If "**" is by itself, this is the first time we encounter it and more
    // is following then find matches without any directory.
    གལ་སྲིད (!didstar && stardepth < 100 && starstar && e - s == 2
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
    གལ་སྲིད (dirp != NULL)
    {
	སྐབས (!got_int)
	{
	    dp = readdir(dirp);
	    གལ་སྲིད (dp == NULL)
		bha_nga;
	    len = (size_t)(s - buf);
	    གལ་སྲིད ((dp->d_name[0] != '.' || starts_with_dot
			|| ((flags & EW_DODOT)
			    && dp->d_name[1] != NUL
			    && (dp->d_name[1] != '.' || dp->d_name[2] != NUL)))
		 && ((regmatch.regprog != NULL && vim_regexec(&regmatch,
					     (char_u *)dp->d_name, (colnr_T)0))
		   || ((flags & EW_NOTWILD)
		     && fnamencmp(path + len, dp->d_name, e - s) == 0)))
	    {
		len += vim_snprintf((var_na *)s, buflen - len, "%s", dp->d_name);
		གལ་སྲིད (len + 1 >= buflen)
		    continue;

		གལ་སྲིད (starstar && stardepth < 100)
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
		གལ་སྲིད (mch_has_exp_wildcard(path_end)) // handle more wildcards
		{
		    གལ་སྲིད (stardepth < 100)
		    {
			// need to expand another component of the path
			// remove backslashes for the remaining components only
			++stardepth;
			(void)unix_expandpath(gap, buf, len + 1, flags, FALSE);
			--stardepth;
		    }
		}
		དེ་མིན
		{
		    stat_T  sb;

		    // no more wildcards, check if there is a match
		    // remove backslashes for the remaining components only
		    གལ་སྲིད (*path_end != NUL)
			backslash_halve(buf + len + 1);
		    // add existing file or symbolic link
		    གལ་སྲིད ((flags & EW_ALLLINKS) ? mch_lstat((var_na *)buf, &sb) >= 0
						      : mch_getperm(buf) >= 0)
		    {
# ifdef MACOS_CONVERT
			size_t precomp_len = STRLEN(buf)+1;
			char_u *precomp_buf =
			    mac_precompose_path(buf, precomp_len, &precomp_len);

			གལ་སྲིད (precomp_buf)
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
    གལ་སྲིད (matches > 0 && !got_int)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   parimaa_na(char_u *), pstrcmp);
    ལོག matches;
}
#endif

/*
 * Return TRUE if "p" contains what looks like an environment variable.
 * Allowing for escaping.
 */
    static ཧྲིལ
has_env_var(char_u *p)
{
    དོན་ལུ ( ; *p; MB_PTR_ADV(p))
    {
	གལ་སྲིད (*p == '\\' && p[1] != NUL)
	    ++p;
	དེ་མིན གལ་སྲིད (vim_strchr((char_u *)
#གལ་སྲིད defined(MSWIN)
				    "$%"
#དེ་མིན
				    "$"
#endif
					, *p) != NULL)
	    ལོག TRUE;
    }
    ལོག FALSE;
}

#ifdef SPECIAL_WILDCHAR
/*
 * Return TRUE if "p" contains a special wildcard character, one that Vim
 * cannot expand, requires using a shell.
 */
    static ཧྲིལ
has_special_wildchar(char_u *p)
{
    དོན་ལུ ( ; *p; MB_PTR_ADV(p))
    {
	// Disallow line break characters.
	གལ་སྲིད (*p == '\r' || *p == '\n')
	    bha_nga;
	// Allow for escaping.
	གལ་སྲིད (*p == '\\' && p[1] != NUL && p[1] != '\r' && p[1] != '\n')
	    ++p;
	དེ་མིན གལ་སྲིད (vim_strchr((char_u *)SPECIAL_WILDCHAR, *p) != NULL)
	{
	    // A { must be followed by a matching }.
	    གལ་སྲིད (*p == '{' && vim_strchr(p, '}') == NULL)
		continue;
	    // A quote and backtick must be followed by another one.
	    གལ་སྲིད ((*p == '`' || *p == '\'') && vim_strchr(p, *p) == NULL)
		continue;
	    ལོག TRUE;
	}
    }
    ལོག FALSE;
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
    ཧྲིལ
gen_expand_wildcards(
    ཧྲིལ		num_pat,	// number of input patterns
    char_u	**pat,		// array of input patterns
    ཧྲིལ		*num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    ཧྲིལ		flags)		// EW_* flags
{
    ཧྲིལ			i;
    garray_T		ga;
    char_u		*p;
    static ཧྲིལ		recursive = FALSE;
    ཧྲིལ			add_pat;
    ཧྲིལ			retval = OK;
    ཧྲིལ			did_expand_in_path = FALSE;
    char_u		*path_option = *curbuf->b_p_path == NUL ?
					p_path : curbuf->b_p_path;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    གལ་སྲིད (recursive)
#ifdef SPECIAL_WILDCHAR
	ལོག mch_expand_wildcards(num_pat, pat, num_file, file, flags);
#དེ་མིན
	ལོག FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     * For `=expr` do use the internal function.
     */
    དོན་ལུ (i = 0; i < num_pat; i++)
    {
	གལ་སྲིད (has_special_wildchar(pat[i])
		&& !(vim_backtick(pat[i]) && pat[i][1] == '=')
	   )
	    ལོག mch_expand_wildcards(num_pat, pat, num_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init2(&ga, parimaa_na(char_u *), 30);

    དོན་ལུ (i = 0; i < num_pat && !got_int; ++i)
    {
	add_pat = -1;
	p = pat[i];

	གལ་སྲིད (vim_backtick(p))
	{
	    add_pat = expand_backtick(&ga, p, flags);
	    གལ་སྲིད (add_pat == -1)
		retval = FAIL;
	}
	དེ་མིན
	{
	    /*
	     * First expand environment variables, "~/" and "~user/".
	     */
	    གལ་སྲིད ((has_env_var(p) && !(flags & EW_NOTENV)) || *p == '~')
	    {
		p = expand_env_save_opt(p, TRUE, (char_u *)PATH_ESC_WILDCARDS);
		གལ་སྲིད (p == NULL)
		    p = pat[i];
#ifdef UNIX
		/*
		 * On Unix, if expand_env() can't expand an environment
		 * variable, use the shell to do that.  Discard previously
		 * found file names and start all over again.
		 */
		དེ་མིན གལ་སྲིད (has_env_var(p) || *p == '~')
		{
		    vim_free(p);
		    ga_clear_strings(&ga);
		    i = mch_expand_wildcards(num_pat, pat, num_file, file,
							 flags|EW_KEEPDOLLAR);
		    recursive = FALSE;
		    ལོག i;
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
	    གལ་སྲིད (mch_has_exp_wildcard(p) || (flags & EW_ICASE))
	    {
		གལ་སྲིད ((flags & (EW_PATH | EW_CDPATH))
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
		དེ་མིན
		    add_pat = mch_expandpath(&ga, p, flags);
	    }
	}

	གལ་སྲིད (add_pat == -1 || (add_pat == 0 && (flags & EW_NOTFOUND)))
	{
	    char_u	*t = backslash_halve_save(p);

	    // When EW_NOTFOUND is used, always add files and dirs.  Makes
	    // "vim c:/" work.
	    གལ་སྲིད (flags & EW_NOTFOUND)
		addfile(&ga, t, flags | EW_DIR | EW_FILE);
	    དེ་མིན
		addfile(&ga, t, flags);

	    གལ་སྲིད (t != p)
		vim_free(t);
	}

	གལ་སྲིད (did_expand_in_path && ga.ga_len > 0 && (flags & (EW_PATH | EW_CDPATH)))
	    uniquefy_paths(&ga, p, path_option);
	གལ་སྲིད (p != pat[i])
	    vim_free(p);
    }

    // When returning FAIL the array must be freed here.
    གལ་སྲིད (retval == FAIL)
	ga_clear_strings(&ga);

    *num_file = ga.ga_len;
    *file = (ga.ga_data != NULL) ? (char_u **)ga.ga_data
						  : (char_u **)_("no matches");

    recursive = FALSE;

    ལོག ((flags & EW_EMPTYOK) || ga.ga_data != NULL) ? retval : FAIL;
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
    ཧྲིལ		flags)
{
    char_u	*p;
    ཧྲིལ		isdir;
    stat_T	sb;

#ifdef MSWIN
    གལ་སྲིད (!(flags & (EW_NOTFOUND | EW_ALLLINKS)))
    {
	long perm = mch_getperm(f);

	གལ་སྲིད (perm < 0)
	    ལོག;
	// The stat result already tells us whether this is a directory.
	isdir = (perm & S_IFMT) == S_IFDIR;
    }
    དེ་མིན
#endif
    {
	// if the file/dir/link doesn't exist, may not add it
	གལ་སྲིད (!(flags & EW_NOTFOUND) && ((flags & EW_ALLLINKS)
		    ? mch_lstat((var_na *)f, &sb) < 0 : mch_getperm(f) < 0))
	    ལོག;
	isdir = mch_isdir(f);
    }

#ifdef FNAME_ILLEGAL
    // if the file/dir contains illegal characters, don't add it
    གལ་སྲིད (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != NULL)
	ལོག;
#endif

    གལ་སྲིད ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	ལོག;

    // If the file isn't executable, may not add it.  Do accept directories.
    // When invoked from expand_shellcmd() do not use $PATH.
    གལ་སྲིད (!isdir && (flags & EW_EXEC)
			     && !mch_can_exe(f, NULL, !(flags & EW_SHELLCMD)))
	ལོག;

    // Make room for another item in the file list.
    གལ་སྲིད (ga_grow(gap, 1) == FAIL)
	ལོག;

    p = alloc(STRLEN(f) + 1 + isdir);
    གལ་སྲིད (p == NULL)
	ལོག;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names if none is present.
     */
    གལ་སྲིད (isdir && (flags & EW_ADDSLASH))
	add_pathsep(p);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
}

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    void
FreeWild(ཧྲིལ count, char_u **files)
{
    གལ་སྲིད (count <= 0 || files == NULL)
	ལོག;
    སྐབས (count--)
	vim_free(files[count]);
    vim_free(files);
}

/*
 * Compare path "p[]" to "q[]".
 * If "maxlen" >= 0 compare "p[maxlen]" to "q[maxlen]"
 * Return value like strcmp(p, q), but consider path separators.
 */
    ཧྲིལ
pathcmp(const var_na *p, const var_na *q, ཧྲིལ maxlen)
{
    ཧྲིལ		i, j;
    ཧྲིལ		c1, c2;
    const var_na	*s = NULL;

    དོན་ལུ (i = 0, j = 0; maxlen < 0 || (i < maxlen && j < maxlen);)
    {
	c1 = PTR2CHAR((char_u *)p + i);
	c2 = PTR2CHAR((char_u *)q + j);

	// End of "p": check if "q" also ends or just has a slash.
	གལ་སྲིད (c1 == NUL)
	{
	    གལ་སྲིད (c2 == NUL)  // full match
		ལོག 0;
	    s = q;
	    i = j;
	    bha_nga;
	}

	// End of "q": check if "p" just has a slash.
	གལ་སྲིད (c2 == NUL)
	{
	    s = p;
	    bha_nga;
	}

	གལ་སྲིད ((p_fic ? MB_TOUPPER(c1) != MB_TOUPPER(c2) : c1 != c2)
#ifdef BACKSLASH_IN_FILENAME
		// consider '/' and '\\' to be equal
		&& !((c1 == '/' && c2 == '\\')
		    || (c1 == '\\' && c2 == '/'))
#endif
		)
	{
	    གལ་སྲིད (vim_ispathsep(c1))
		ལོག -1;
	    གལ་སྲིད (vim_ispathsep(c2))
		ལོག 1;
	    ལོག p_fic ? MB_TOUPPER(c1) - MB_TOUPPER(c2)
		    : c1 - c2;  // no match
	}

	i += mb_ptr2len((char_u *)p + i);
	j += mb_ptr2len((char_u *)q + j);
    }
    གལ་སྲིད (s == NULL)	// "i" or "j" ran into "maxlen"
	ལོག 0;

    c1 = PTR2CHAR((char_u *)s + i);
    c2 = PTR2CHAR((char_u *)s + i + mb_ptr2len((char_u *)s + i));
    // ignore a trailing slash, but not "//" or ":/"
    གལ་སྲིད (c2 == NUL
	    && i > 0
	    && !after_pathsep((char_u *)s, (char_u *)s + i)
#ifdef BACKSLASH_IN_FILENAME
	    && (c1 == '/' || c1 == '\\')
#དེ་མིན
	    && c1 == '/'
#endif
       )
	ལོག 0;   // match with trailing slash
    གལ་སྲིད (s == q)
	ལོག -1;	    // no match
    ལོག 1;
}

/*
 * Return TRUE if "name" is a full (absolute) path name or URL.
 */
    ཧྲིལ
vim_isAbsName(char_u *name)
{
    ལོག (path_with_url(name) != 0 || mch_isFullName(name));
}

/*
 * Get absolute file name into buffer "buf[len]".
 *
 * return FAIL for failure, OK otherwise
 */
    ཧྲིལ
vim_FullName(
    char_u	*fname,
    char_u	*buf,
    ཧྲིལ		len,
    ཧྲིལ		force)	    // force expansion even when already absolute
{
    ཧྲིལ		retval = OK;
    ཧྲིལ		url;

    *buf = NUL;
    གལ་སྲིད (fname == NULL)
	ལོག FAIL;

    url = path_with_url(fname);
    གལ་སྲིད (!url)
	retval = mch_FullName(fname, buf, len, force);
    གལ་སྲིད (url || retval == FAIL)
    {
	// something failed; use the file name (truncate when too long)
	vim_strncpy(buf, fname, len - 1);
    }
#གལ་སྲིད defined(MSWIN)
    slash_adjust(buf);
#endif
    ལོག retval;
}
