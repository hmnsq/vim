/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * viminfo.c: viminfo related functions
 */

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
प्रकारनिर्देश संरचना
{
    char_u	*vir_पङ्क्ति;	// text of the current line
    खाता	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    पूर्णाङ्क		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

प्रकारनिर्देश गणना {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

प्रकारनिर्देश संरचना {
    btype_T	bv_type;
    दीर्घ	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    पूर्णाङ्क		bv_len;		// length of bv_string
    पूर्णाङ्क		bv_allocated;	// bv_string was allocated
} bval_T;

#यदि defined(FEAT_VIMINFO)

स्थिर पूर्णाङ्क  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    स्थिर char_u *
find_viminfo_parameter(पूर्णाङ्क type)
{
    char_u  *p;

    लागि (p = p_viminfo; *p; ++p)
    {
	यदि (*p == type)
	    फर्काऊ p + 1;
	यदि (*p == 'n')		    // 'n' is always the last one
	    रोक;
	p = vim_strchr(p, ',');	    // skip until next ','
	यदि (p == रिक्त)		    // hit the end without finding parameter
	    रोक;
    }
    फर्काऊ रिक्त;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    पूर्णाङ्क
get_viminfo_parameter(पूर्णाङ्क type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    यदि (p != रिक्त && VIM_ISDIGIT(*p))
	फर्काऊ म_से_प((वर्ण *)p);
    फर्काऊ -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    स्थिर char_u *
viminfo_filename(char_u *file)
{
    यदि (file == रिक्त || *file == NUL)
    {
	माप_प्रकार  len;

	यदि (*p_viminfofile != NUL)
	    file = p_viminfofile;
	अन्यथा यदि ((file = find_viminfo_parameter('n')) == रिक्त || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    यदि (mch_getenv((char_u *)"SYS$LOGIN") == रिक्त)
# अन्यथा
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    यदि (STRCMP(vim_getenv((char_u *)"HOME", रिक्त), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == रिक्त)
#  अन्यथा
	    यदि (mch_getenv((char_u *)"HOME") == रिक्त)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		यदि (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		अन्यथा
		    file = (char_u *)VIMINFO_FILE;
	    }
	    अन्यथा
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	फर्काऊ vim_strnsave(file, len);
    }

    फर्काऊ vim_strsave(file);
}

/*
 * write string to viminfo file
 * - replace CTRL-V with CTRL-V CTRL-V
 * - replace '\n'   with CTRL-V 'n'
 * - add a '\n' at the end
 *
 * For a long line:
 * - write " CTRL-V <length> \n " in first line
 * - write " < <string> \n "	  in second line
 */
    स्थिर शून्य
viminfo_writestring(खाता *fd, char_u *p)
{
    पूर्णाङ्क		c;
    char_u	*s;
    पूर्णाङ्क		len = 0;

    लागि (s = p; *s != NUL; ++s)
    {
	यदि (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    यदि (len > LSIZE / 2)
	ख_लिखो(fd, "\026%d\n<", len + 3);

    जबसम्म ((c = *p++) != NUL)
    {
	यदि (c == Ctrl_V || c == '\n')
	{
	    अ_दो(Ctrl_V, fd);
	    यदि (c == '\n')
		c = 'n';
	}
	अ_दो(c, fd);
    }
    अ_दो('\n', fd);
}

/*
 * Write a string in quotes that barline_parse() can read back.
 * Breaks the line in less than LSIZE pieces when needed.
 * Returns remaining characters in the line.
 */
    स्थिर पूर्णाङ्क
barline_writestring(खाता *fd, char_u *s, पूर्णाङ्क remaining_start)
{
    char_u *p;
    पूर्णाङ्क	    remaining = remaining_start;
    पूर्णाङ्क	    len = 2;

    // Count the number of characters produced, including quotes.
    लागि (p = s; *p != NUL; ++p)
    {
	यदि (*p == NL)
	    len += 2;
	अन्यथा यदि (*p == '"' || *p == '\\')
	    len += 2;
	अन्यथा
	    ++len;
    }
    यदि (len > remaining - 2)
    {
	ख_लिखो(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    अ_दो('"', fd);
    लागि (p = s; *p != NUL; ++p)
    {
	यदि (*p == NL)
	{
	    अ_दो('\\', fd);
	    अ_दो('n', fd);
	    --remaining;
	}
	अन्यथा यदि (*p == '"' || *p == '\\')
	{
	    अ_दो('\\', fd);
	    अ_दो(*p, fd);
	    --remaining;
	}
	अन्यथा
	    अ_दो(*p, fd);
	--remaining;

	यदि (remaining < 3)
	{
	    अ_दो('\n', fd);
	    अ_दो('|', fd);
	    अ_दो('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    अ_दो('"', fd);
    फर्काऊ remaining - 2;
}

/*
 * Check string read from viminfo file.
 * Remove '\n' at the end of the line.
 * - replace CTRL-V CTRL-V with CTRL-V
 * - replace CTRL-V 'n'    with '\n'
 *
 * Check for a long line as written by viminfo_writestring().
 *
 * Return the string in allocated memory (NULL when out of memory).
 */
    स्थिर char_u *
viminfo_readstring(
    vir_T	*virp,
    पूर्णाङ्क		off,		    // offset for virp->vir_line
    पूर्णाङ्क		convert)	    // convert the string
{
    char_u	*retval = रिक्त;
    char_u	*s, *d;
    दीर्घ	len;

    यदि (virp->vir_पङ्क्ति[off] == Ctrl_V && vim_isdigit(virp->vir_पङ्क्ति[off + 1]))
    {
	len = म_से_द((वर्ण *)virp->vir_पङ्क्ति + off + 1);
	यदि (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	यदि (retval == रिक्त)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (शून्य)vim_fgets(virp->vir_पङ्क्ति, 10, virp->vir_fd);
	    फर्काऊ रिक्त;
	}
	(शून्य)vim_fgets(retval, (पूर्णाङ्क)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    अन्यथा
    {
	retval = vim_strsave(virp->vir_पङ्क्ति + off);
	यदि (retval == रिक्त)
	    फर्काऊ रिक्त;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    जबसम्म (*s != NUL && *s != '\n')
    {
	यदि (s[0] == Ctrl_V && s[1] != NUL)
	{
	    यदि (s[1] == 'n')
		*d++ = '\n';
	    अन्यथा
		*d++ = Ctrl_V;
	    s += 2;
	}
	अन्यथा
	    *d++ = *s++;
    }
    *d = NUL;

    यदि (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, रिक्त);
	यदि (d != रिक्त)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    फर्काऊ retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    स्थिर पूर्णाङ्क
viminfo_readline(vir_T *virp)
{
    फर्काऊ vim_fgets(virp->vir_पङ्क्ति, LSIZE, virp->vir_fd);
}

    स्थिर पूर्णाङ्क
read_viminfo_bufferlist(
    vir_T	*virp,
    पूर्णाङ्क		writing)
{
    char_u	*tab;
    linenr_T	lnum;
    colnr_T	col;
    buf_T	*buf;
    char_u	*sfname;
    char_u	*xline;

    // Handle long line and escaped characters.
    xline = viminfo_readstring(virp, 1, FALSE);

    // don't read in if there are files on the command-line or if writing:
    यदि (xline != रिक्त && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != रिक्त)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	यदि (tab != रिक्त)
	{
	    *tab++ = '\0';
	    col = (colnr_T)म_से_प((वर्ण *)tab);
	    tab = vim_strrchr(xline, '\t');
	    यदि (tab != रिक्त)
	    {
		*tab++ = '\0';
		lnum = म_से_द((वर्ण *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	यदि (buf != रिक्त)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    फर्काऊ viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    स्थिर पूर्णाङ्क
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    पूर्णाङ्क	    part_len;
    पूर्णाङ्क	    retval = FALSE;

    name = home_replace_save(रिक्त, name);
    यदि (name == रिक्त)
	फर्काऊ FALSE;
    लागि (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, परिमाण(part), ", ");
	यदि (part[0] == 'r')
	{
	    यदि (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		रोक;
	    }
	}
    }
    vim_free(name);
    फर्काऊ retval;
}

    स्थिर शून्य
write_viminfo_bufferlist(खाता *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*पङ्क्ति;
    पूर्णाङ्क		max_buffers;

    यदि (find_viminfo_parameter('%') == रिक्त)
	फर्काऊ;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define पङ्क्ति_BUF_LEN (MAXPATHL + 40)
    पङ्क्ति = alloc(पङ्क्ति_BUF_LEN);
    यदि (पङ्क्ति == रिक्त)
	फर्काऊ;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    ख_माला_दो(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	माप_प्रकार	linelen;

	यदि (buf->b_fname == रिक्त
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    जारी;

	यदि (max_buffers-- == 0)
	    रोक;
	अ_दो('%', fp);
	linelen = home_replace(रिक्त, buf->b_ffname, पङ्क्ति, MAXPATHL, TRUE);
	vim_snprintf((वर्ण *)पङ्क्ति + linelen, पङ्क्ति_BUF_LEN - linelen, "\t%ld\t%d",
			(दीर्घ)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, पङ्क्ति);
    }
    vim_free(पङ्क्ति);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
स्थिर histentry_T *viminfo_history[HIST_गन्ती] =
					       {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};
स्थिर पूर्णाङ्क	viminfo_hisidx[HIST_गन्ती] = {0, 0, 0, 0, 0};
स्थिर पूर्णाङ्क	viminfo_hislen[HIST_गन्ती] = {0, 0, 0, 0, 0};
स्थिर पूर्णाङ्क	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    स्थिर पूर्णाङ्क
hist_type2char(
    पूर्णाङ्क	    type,
    पूर्णाङ्क	    use_question)	    // use '?' instead of '/'
{
    यदि (type == HIST_CMD)
	फर्काऊ ':';
    यदि (type == HIST_खोज)
    {
	यदि (use_question)
	    फर्काऊ '?';
	अन्यथा
	    फर्काऊ '/';
    }
    यदि (type == HIST_अभिव्यक्ति)
	फर्काऊ '=';
    फर्काऊ '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    स्थिर शून्य
prepare_viminfo_history(पूर्णाङ्क asklen, पूर्णाङ्क writing)
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    अङ्क;
    पूर्णाङ्क	    type;
    पूर्णाङ्क	    len;
    पूर्णाङ्क	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    यदि (asklen > hislen)
	asklen = hislen;

    लागि (type = 0; type < HIST_गन्ती; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	लागि (i = 0, अङ्क = 0; i < hislen; i++)
	    यदि (histentry[i].hisstr == रिक्त || histentry[i].viminfo)
		अङ्क++;
	len = asklen;
	यदि (अङ्क > len)
	    len = अङ्क;
	यदि (len <= 0)
	    viminfo_history[type] = रिक्त;
	अन्यथा
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	यदि (viminfo_history[type] == रिक्त)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    स्थिर पूर्णाङ्क
read_viminfo_history(vir_T *virp, पूर्णाङ्क writing)
{
    पूर्णाङ्क		type;
    long_u	len;
    char_u	*val = रिक्त;
    char_u	*p;

    type = hist_char2type(virp->vir_पङ्क्ति[0]);
    यदि (viminfo_hisidx[type] >= viminfo_hislen[type])
	गच्छ done;

    val = viminfo_readstring(virp, 1, TRUE);
    यदि (val == रिक्त || *val == NUL)
	गच्छ done;

    पूर्णाङ्क sep = (*val == ' ' ? NUL : *val);

    यदि (in_history(type, val + (type == HIST_खोज), viminfo_add_at_front,
								sep, writing))
	गच्छ done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    यदि (type == HIST_खोज)
    {
	p = alloc((माप_प्रकार)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	यदि (p == रिक्त)
	    गच्छ done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (माप_प्रकार)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    अन्यथा
    {
	p = alloc((माप_प्रकार)len + 2);	    // +1 for NUL and +1 for separator
	यदि (p == रिक्त)
	    गच्छ done;

	// Not a search entry: No separator in the viminfo
	// file, add a NUL separator.
	mch_memmove(p, val, (माप_प्रकार)len + 1);	// +1 to include the NUL
	p[len + 1] = NUL;			// put the separator *after* the string's NUL
    }
    viminfo_history[type][viminfo_hisidx[type]].hisstr = p;
    viminfo_history[type][viminfo_hisidx[type]].hisstrlen = (माप_प्रकार)len;
    viminfo_history[type][viminfo_hisidx[type]].time_set = 0;
    viminfo_history[type][viminfo_hisidx[type]].viminfo = TRUE;
    viminfo_history[type][viminfo_hisidx[type]].hisnum = 0;
    viminfo_hisidx[type]++;

done:
    vim_free(val);
    फर्काऊ viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    स्थिर शून्य
handle_viminfo_history(
	garray_T    *मानहरू,
	पूर्णाङ्क	    writing)
{
    पूर्णाङ्क		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)मानहरू->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    यदि (मानहरू->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	फर्काऊ;

    type = vp[0].bv_nr;
    यदि (type >= HIST_गन्ती)
	फर्काऊ;

    यदि (viminfo_hisidx[type] >= viminfo_hislen[type])
	फर्काऊ;

    val = vp[3].bv_string;
    यदि (val == रिक्त || *val == NUL)
	फर्काऊ;

    पूर्णाङ्क sep = type == HIST_खोज && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    पूर्णाङ्क idx;
    पूर्णाङ्क overwrite = FALSE;

    यदि (in_history(type, val, viminfo_add_at_front, sep, writing))
	फर्काऊ;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    लागि (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	यदि (STRCMP(val, p) == 0
		&& (type != HIST_खोज || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    रोक;
	}
    }

    यदि (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    अन्यथा
	len = 0; // for picky compilers
    यदि (p != रिक्त)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	यदि (!overwrite)
	{
	    mch_memmove(p, val, (माप_प्रकार)len + 1);
	    // Put the separator after the NUL.
	    p[len + 1] = sep;
	    viminfo_history[type][idx].hisstr = p;
	    viminfo_history[type][idx].hisstrlen = (माप_प्रकार)len;
	    viminfo_history[type][idx].hisnum = 0;
	    viminfo_history[type][idx].viminfo = TRUE;
	    viminfo_hisidx[type]++;
	}
    }
}

/*
 * Concatenate history lines from viminfo after the lines typed in this Vim.
 */
    स्थिर शून्य
concat_history(पूर्णाङ्क type)
{
    पूर्णाङ्क		idx;
    पूर्णाङ्क		i;
    पूर्णाङ्क		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    पूर्णाङ्क		*hisidx = get_hisidx(type);
    पूर्णाङ्क		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    यदि (idx >= hislen)
	idx -= hislen;
    अन्यथा यदि (idx < 0)
	idx = hislen - 1;
    यदि (viminfo_add_at_front)
	*hisidx = idx;
    अन्यथा
    {
	यदि (*hisidx == -1)
	    *hisidx = hislen - 1;
	गर
	{
	    यदि (histentry[idx].hisstr != रिक्त || histentry[idx].viminfo)
		रोक;
	    यदि (++idx == hislen)
		idx = 0;
	} जबसम्म (idx != *hisidx);
	यदि (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    लागि (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	यदि (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    लागि (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    स्थिर पूर्णाङ्क
sort_hist(अचर शून्य *s1, अचर शून्य *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    यदि (p1->time_set < p2->time_set) फर्काऊ -1;
    यदि (p1->time_set > p2->time_set) फर्काऊ 1;
    फर्काऊ 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    स्थिर शून्य
merge_history(पूर्णाङ्क type)
{
    पूर्णाङ्क		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    पूर्णाङ्क		i;
    पूर्णाङ्क		len;
    पूर्णाङ्क		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    पूर्णाङ्क		*hisidx = get_hisidx(type);
    पूर्णाङ्क		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    यदि (tot_hist == रिक्त || new_hist == रिक्त)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	फर्काऊ;
    }
    लागि (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    लागि (i = 0; i < hislen; i++)
	यदि (histentry[i].hisstr != रिक्त)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    क्विक((शून्य *)tot_hist, (माप_प्रकार)len, परिमाण(histentry_T *), sort_hist);

    // Keep the newest ones.
    लागि (i = 0; i < hislen; i++)
    {
	यदि (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = रिक्त;
	    tot_hist[i]->hisstrlen = 0;
	    यदि (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	अन्यथा
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    लागि (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    लागि (i = 0; i < hislen; i++)
    {
	vim_free(histentry[i].hisstr);
	histentry[i].hisstrlen = 0;
    }
    vim_free(histentry);
    set_histentry(type, new_hist);
    vim_free(tot_hist);
}

/*
 * Finish reading history lines from viminfo.  Not used when writing viminfo.
 */
    स्थिर शून्य
finish_viminfo_history(vir_T *virp)
{
    पूर्णाङ्क	type;
    पूर्णाङ्क merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    लागि (type = 0; type < HIST_गन्ती; ++type)
    {
	यदि (get_histentry(type) == रिक्त)
	    जारी;

	यदि (merge)
	    merge_history(type);
	अन्यथा
	    concat_history(type);

	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Write history to viminfo file in "fp".
 * When "merge" is TRUE merge history lines with a previously read viminfo
 * file, data is in viminfo_history[].
 * When "merge" is FALSE just write all history lines.  Used for ":wviminfo!".
 */
    स्थिर शून्य
write_viminfo_history(खाता *fp, पूर्णाङ्क merge)
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    type;
    पूर्णाङ्क	    अङ्क_saved;
    पूर्णाङ्क     round;
    पूर्णाङ्क	    hislen;

    init_history();
    hislen = get_hislen();
    यदि (hislen == 0)
	फर्काऊ;
    लागि (type = 0; type < HIST_गन्ती; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	पूर्णाङ्क	    *hisidx = get_hisidx(type);

	अङ्क_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	यदि (अङ्क_saved == 0)
	    जारी;
	यदि (अङ्क_saved < 0)  // Use default
	    अङ्क_saved = hislen;
	ख_लिखो(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_खोज ? _("Search String") :
			    type == HIST_अभिव्यक्ति ? _("Expression") :
			    type == HIST_आगत ? _("Input Line") :
					_("Debug Line"));
	यदि (अङ्क_saved > hislen)
	    अङ्क_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	लागि (round = 1; round <= 2; ++round)
	{
	    यदि (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    अन्यथा यदि (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    अन्यथा
		// empty list
		i = -1;
	    यदि (i >= 0)
		जबसम्म (अङ्क_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    माप_प्रकार  plen;
		    समय_प्रकार  timestamp;
		    पूर्णाङ्क	    c = NUL;

		    यदि (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    अन्यथा
		    {
			यदि (viminfo_history[type] == रिक्त)
			{
			    p = रिक्त;
			    plen = 0;
			    timestamp = 0;
			}
			अन्यथा
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    यदि (p != रिक्त && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--अङ्क_saved;
			ख_अक्षर_दो(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			यदि (type == HIST_खोज)
			{
			    c = p[plen + 1];
			    अ_दो(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    वर्ण    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    यदि (c == NUL)
				cbuf[0] = NUL;
			    अन्यथा
				प्र_लिखो(cbuf, "%d", c);
			    ख_लिखो(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (दीर्घ)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    अ_दो('\n', fp);
			}
		    }
		    यदि (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			यदि (--i < 0)
			    i = hislen - 1;
			यदि (i == *hisidx)
			    रोक;
		    }
		    अन्यथा
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	लागि (i = 0; i < viminfo_hisidx[type]; ++i)
	    यदि (viminfo_history[type] != रिक्त)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    स्थिर शून्य
write_viminfo_barlines(vir_T *virp, खाता *fp_out)
{
    पूर्णाङ्क		i;
    garray_T	*gap = &virp->vir_barlines;
    पूर्णाङ्क		seen_useful = FALSE;
    वर्ण	*पङ्क्ति;

    यदि (gap->ga_len <= 0)
	फर्काऊ;

    ख_माला_दो(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    लागि (i = 0; i < gap->ga_len; ++i)
    {
	पङ्क्ति = ((वर्ण **)(gap->ga_data))[i];
	यदि (seen_useful || पङ्क्ति[1] != '<')
	{
	    ख_माला_दो(पङ्क्ति, fp_out);
	    seen_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    स्थिर पूर्णाङ्क
barline_parse(vir_T *virp, char_u *text, garray_T *मानहरू)
{
    char_u  *p = text;
    char_u  *nextp = रिक्त;
    char_u  *buf = रिक्त;
    bval_T  *मान;
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    allocated = FALSE;
    पूर्णाङ्क	    eof;
    char_u  *sconv;
    पूर्णाङ्क	    converted;

    जबसम्म (*p == ',')
    {
	++p;
	यदि (ga_grow(मानहरू, 1) == FAIL)
	    रोक;
	मान = (bval_T *)(मानहरू->ga_data) + मानहरू->ga_len;

	यदि (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    यदि (!allocated)
	    {
		लागि (i = 0; i < मानहरू->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(मानहरू->ga_data) + i;

		    यदि (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    यदि (vim_isdigit(p[1]))
	    {
		माप_प्रकार len;
		माप_प्रकार todo;
		माप_प्रकार n;

		// String value was split into lines that are each shorter
		// than LSIZE:
		//     |{bartype},>{length of "{text}{text2}"}
		//     |<"{text1}
		//     |<{text2}",{value}
		// Length includes the quotes.
		++p;
		len = getdigits(&p);
		buf = alloc(len + 1);
		यदि (buf == रिक्त)
		    फर्काऊ TRUE;
		p = buf;
		लागि (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    यदि (eof || virp->vir_पङ्क्ति[0] != '|'
						  || virp->vir_पङ्क्ति[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			फर्काऊ eof || virp->vir_पङ्क्ति[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_पङ्क्ति);
		    जबसम्म (n > 2 && (virp->vir_पङ्क्ति[n - 1] == NL
					     || virp->vir_पङ्क्ति[n - 1] == CAR))
			--n;
		    n -= 2;
		    यदि (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_पङ्क्ति + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_पङ्क्ति + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    अन्यथा
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		यदि (eof || virp->vir_पङ्क्ति[0] != '|'
					      || virp->vir_पङ्क्ति[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    फर्काऊ eof || virp->vir_पङ्क्ति[0] == '|';
		p = virp->vir_पङ्क्ति + 2;
	    }
	}

	यदि (SAFE_isdigit(*p))
	{
	    मान->bv_type = BVAL_NR;
	    मान->bv_nr = getdigits(&p);
	    ++मानहरू->ga_len;
	}
	अन्यथा यदि (*p == '"')
	{
	    पूर्णाङ्क	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    जबसम्म (*p != '"')
	    {
		यदि (*p == NL || *p == NUL)
		    फर्काऊ TRUE;  // syntax error, drop the value
		यदि (*p == '\\')
		{
		    ++p;
		    यदि (*p == 'n')
			s[len++] = '\n';
		    अन्यथा
			s[len++] = *p;
		    ++p;
		}
		अन्यथा
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    मान->bv_tofree = रिक्त;
	    यदि (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, रिक्त);
		यदि (sconv != रिक्त)
		{
		    यदि (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			मान->bv_tofree = buf;
		    s = sconv;
		    len = (पूर्णाङ्क)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    यदि (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    मान->bv_string = s;
	    मान->bv_type = BVAL_STRING;
	    मान->bv_len = len;
	    मान->bv_allocated = allocated || converted;
	    ++मानहरू->ga_len;
	    यदि (nextp != रिक्त)
	    {
		// values following a long string
		p = nextp;
		nextp = रिक्त;
	    }
	}
	अन्यथा यदि (*p == ',')
	{
	    मान->bv_type = BVAL_EMPTY;
	    ++मानहरू->ga_len;
	}
	अन्यथा
	    रोक;
    }
    फर्काऊ TRUE;
}

    स्थिर शून्य
write_viminfo_version(खाता *fp_out)
{
    ख_लिखो(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    स्थिर पूर्णाङ्क
no_viminfo(शून्य)
{
    // "vim -i NONE" does not read or write a viminfo file
    फर्काऊ STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    स्थिर पूर्णाङ्क
viminfo_error(वर्ण *errnum, वर्ण *message, char_u *पङ्क्ति)
{
    vim_snprintf((वर्ण *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, पङ्क्ति, IOSIZE - STRLEN(IObuff) - 1);
    यदि (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((वर्ण *)IObuff);
    यदि (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	फर्काऊ TRUE;
    }
    फर्काऊ FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    स्थिर पूर्णाङ्क
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    पूर्णाङ्क		i;

    यदि (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_पङ्क्ति, '=');
	यदि (p != रिक्त)
	{
	    // remove trailing newline
	    ++p;
	    लागि (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    फर्काऊ viminfo_readline(virp);
}

#यदि defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    स्थिर पूर्णाङ्क
read_viminfo_varlist(vir_T *virp, पूर्णाङ्क writing)
{
    char_u	*tab;
    पूर्णाङ्क		type = VAR_सङ्ख्या;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    यदि (!writing && (find_viminfo_parameter('!') != रिक्त))
    {
	tab = vim_strchr(virp->vir_पङ्क्ति + 1, '\t');
	यदि (tab != रिक्त)
	{
	    *tab++ = '\0';	// isolate the variable name
	    विकल्प (*tab)
	    {
		पक्ष 'S': type = VAR_STRING; रोक;
		पक्ष 'F': type = VAR_FLOAT; रोक;
		पक्ष 'D': type = VAR_DICT; रोक;
		पक्ष 'L': type = VAR_LIST; रोक;
		पक्ष 'B': type = VAR_BLOB; रोक;
		पक्ष 'X': type = VAR_SPECIAL; रोक;
		पक्ष 'T': type = VAR_TUPLE; रोक;
	    }

	    tab = vim_strchr(tab, '\t');
	    यदि (tab != रिक्त)
	    {
		tv.v_type = type;
		यदि (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (पूर्णाङ्क)(tab - virp->vir_पङ्क्ति + 1), TRUE);
		अन्यथा यदि (type == VAR_FLOAT)
		    (शून्य)string2float(tab + 1, &tv.vval.v_float, FALSE);
		अन्यथा
		{
		    tv.vval.v_सङ्ख्या = म_से_द((वर्ण *)tab + 1);
		    यदि (type == VAR_SPECIAL && (tv.vval.v_सङ्ख्या == VVAL_FALSE
					     || tv.vval.v_सङ्ख्या == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		यदि (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_अभिव्यक्ति(tv.vval.v_string, रिक्त);

		    यदि (etv == रिक्त)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    अन्यथा
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		अन्यथा यदि (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    यदि (blob == रिक्त)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    अन्यथा
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_entry);
		set_var(virp->vir_पङ्क्ति + 1, &tv, FALSE);
		restore_funccal();

		यदि (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		अन्यथा यदि (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    फर्काऊ viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    स्थिर शून्य
write_viminfo_varlist(खाता *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    पूर्णाङ्क		todo;
    वर्ण	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    यदि (find_viminfo_parameter('!') == रिक्त)
	फर्काऊ;

    ख_माला_दो(_("\n# global variables:\n"), fp);

    todo = (पूर्णाङ्क)gvht->ht_used;
    FOR_ALL_HASHTAB_वस्तुहरू(gvht, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    यदि (var_flavour(this_var->di_साँचो) == VAR_FLAVOUR_VIMINFO)
	    {
		विकल्प (this_var->di_tv.v_type)
		{
		    पक्ष VAR_STRING:  s = "STR"; रोक;
		    पक्ष VAR_सङ्ख्या:  s = "NUM"; रोक;
		    पक्ष VAR_FLOAT:   s = "FLO"; रोक;
		    पक्ष VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      पूर्णाङ्क	copyID = get_copyID();

			      s = "DIC";
			      यदि (di != रिक्त && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, रिक्त, रिक्त)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  जारी;
			      रोक;
			  }
		    पक्ष VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      पूर्णाङ्क	copyID = get_copyID();

			      s = "LIS";
			      यदि (l != रिक्त && !set_ref_in_list_वस्तुहरू(
						       l, copyID, रिक्त, रिक्त)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  जारी;
			      रोक;
			  }
		    पक्ष VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      पूर्णाङ्क	copyID = get_copyID();

			      s = "TUP";
			      यदि (tuple != रिक्त && !set_ref_in_tuple_वस्तुहरू(
					       tuple, copyID, रिक्त, रिक्त)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  जारी;
			      रोक;
			  }
		    पक्ष VAR_BLOB:    s = "BLO"; रोक;
		    पक्ष VAR_BOOL:    s = "XPL"; रोक;  // backwards compat.
		    पक्ष VAR_SPECIAL: s = "XPL"; रोक;

		    पक्ष VAR_UNKNOWN:
		    पक्ष VAR_ANY:
		    पक्ष VAR_VOID:
		    पक्ष VAR_FUNC:
		    पक्ष VAR_PARTIAL:
		    पक्ष VAR_JOB:
		    पक्ष VAR_CHANNEL:
		    पक्ष VAR_INSTR:
		    पक्ष VAR_CLASS:
		    पक्ष VAR_OBJECT:
		    पक्ष VAR_TYPEALIAS:
				      जारी;
		}
		ख_लिखो(fp, "!%s\t%s\t", this_var->di_साँचो, s);
		यदि (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    प्र_लिखो((वर्ण *)numbuf, "%ld",
					  (दीर्घ)this_var->di_tv.vval.v_सङ्ख्या);
		    p = numbuf;
		    tofree = रिक्त;
		}
		अन्यथा
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		यदि (p != रिक्त)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    स्थिर पूर्णाङ्क
read_viminfo_sub_string(vir_T *virp, पूर्णाङ्क force)
{
    यदि (force || get_old_sub() == रिक्त)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    फर्काऊ viminfo_readline(virp);
}

    स्थिर शून्य
write_viminfo_sub_string(खाता *fp)
{
    char_u *old_sub = get_old_sub();

    यदि (get_viminfo_parameter('/') == 0 || old_sub == रिक्त)
	फर्काऊ;

    ख_माला_दो(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    स्थिर पूर्णाङ्क
read_viminfo_खोज_pattern(vir_T *virp, पूर्णाङ्क force, पूर्णाङ्क writing)
{
    char_u	*lp;
    पूर्णाङ्क		idx = -1;
    पूर्णाङ्क		magic = FALSE;
    पूर्णाङ्क		no_scs = FALSE;
    पूर्णाङ्क		off_पङ्क्ति = FALSE;
    पूर्णाङ्क		off_end = 0;
    दीर्घ	off = 0;
    पूर्णाङ्क		setlast = FALSE;
#ifdef FEAT_खोज_EXTRA
    स्थिर पूर्णाङ्क	hlsearch_on = FALSE;
#endif
    char_u	*val;
    spat_T	*spat;

    // Old line types:
    // "/pat", "&pat": search/subst. pat
    // "~/pat", "~&pat": last used search/subst. pat
    // New line types:
    // "~h", "~H": hlsearch highlighting off/on
    // "~<magic><smartcase><line><end><off><last><which>pat"
    // <magic>: 'm' off, 'M' on
    // <smartcase>: 's' off, 'S' on
    // <line>: 'L' line offset, 'l' char offset
    // <end>: 'E' from end, 'e' from start
    // <off>: decimal, offset
    // <last>: '~' last used pattern
    // <which>: '/' search pat, '&' subst. pat
    lp = virp->vir_पङ्क्ति;
    यदि (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	यदि (lp[1] == 'M')		// magic on
	    magic = TRUE;
	यदि (lp[2] == 's')
	    no_scs = TRUE;
	यदि (lp[3] == 'L')
	    off_पङ्क्ति = TRUE;
	यदि (lp[4] == 'E')
	    off_end = खोज_END;
	lp += 5;
	off = getdigits(&lp);
    }
    यदि (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    यदि (lp[0] == '/')
	idx = RE_खोज;
    अन्यथा यदि (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_खोज_EXTRA
    अन्यथा यदि (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    अन्यथा यदि (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    यदि (idx >= 0)
    {
	spat = get_spat(idx);
	यदि (force || spat->pat == रिक्त)
	{
	    val = viminfo_readstring(virp, (पूर्णाङ्क)(lp - virp->vir_पङ्क्ति + 1),
									TRUE);
	    यदि (val != रिक्त)
	    {
		यदि (!writing)
		{
		    set_last_खोज_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.पङ्क्ति = off_पङ्क्ति;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_खोज_EXTRA
		    यदि (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    फर्काऊ viminfo_readline(virp);
}

    स्थिर शून्य
wvsp_one(
    खाता	*fp,	// file to write to
    पूर्णाङ्क		idx,	// spats[] index
    वर्ण	*s,	// search pat
    पूर्णाङ्क		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    यदि (spat->pat == रिक्त)
	फर्काऊ;

    ख_लिखो(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    ख_लिखो(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.पङ्क्ति ? 'L' : 'l',	// line offset
	    spat->off.end  ? 'E' : 'e',	// offset from end
	    spat->off.off,			// offset
	    get_spat_last_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    स्थिर शून्य
write_viminfo_खोज_pattern(खाता *fp)
{
    यदि (get_viminfo_parameter('/') == 0)
	फर्काऊ;

#ifdef FEAT_खोज_EXTRA
    ख_लिखो(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != रिक्त) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_खोज, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

स्थिर yankreg_T *y_read_regs = रिक्त;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    स्थिर शून्य
prepare_viminfo_registers(शून्य)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, अङ्क_REGISTERS);
}

    स्थिर शून्य
finish_viminfo_registers(शून्य)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		j;

    यदि (y_read_regs == रिक्त)
	फर्काऊ;

    लागि (i = 0; i < अङ्क_REGISTERS; ++i)
	यदि (y_read_regs[i].y_array != रिक्त)
	{
	    लागि (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    स्थिर पूर्णाङ्क
read_viminfo_register(vir_T *virp, पूर्णाङ्क force)
{
    पूर्णाङ्क		eof;
    पूर्णाङ्क		do_it = TRUE;
    पूर्णाङ्क		size;
    पूर्णाङ्क		limit;
    पूर्णाङ्क		i;
    पूर्णाङ्क		set_prev = FALSE;
    char_u	*str;
    string_T	*array = रिक्त;
    पूर्णाङ्क		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_पङ्क्ति + 1;

    // If the line starts with "" this is the y_previous register.
    यदि (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    यदि (!ASCII_ISALNUM(*str) && *str != '-')
    {
	यदि (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_पङ्क्ति))
	    फर्काऊ TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    यदि (!force && y_current_p->y_array != रिक्त)
	do_it = FALSE;

    यदि (*str == '@')
    {
	// "x@: register x used for @@
	यदि (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    यदि (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	यदि (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	यदि (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	अन्यथा यदि (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	अन्यथा
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    जबसम्म (!(eof = viminfo_readline(virp))
		    && (virp->vir_पङ्क्ति[0] == TAB || virp->vir_पङ्क्ति[0] == '<'))
    {
	यदि (do_it)
	{
	    यदि (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((माप_प्रकार)limit * 2 * परिमाण(string_T));

		यदि (new_array == रिक्त)
		{
		    do_it = FALSE;
		    रोक;
		}
		लागि (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    यदि (str != रिक्त)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    अन्यथा
		// error, don't store the result
		do_it = FALSE;
	}
    }

    यदि (do_it)
    {
	// free y_array[]
	लागि (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	यदि (size == 0)
	{
	    y_current_p->y_array = रिक्त;
	}
	अन्यथा
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    लागि (i = 0; i < size; i++)
	    {
		यदि (y_current_p->y_array == रिक्त)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		अन्यथा
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    अन्यथा
    {
	// Free array[] if it was filled.
	लागि (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    फर्काऊ eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    स्थिर शून्य
handle_viminfo_register(garray_T *मानहरू, पूर्णाङ्क force)
{
    bval_T	*vp = (bval_T *)मानहरू->ga_data;
    पूर्णाङ्क		flags;
    पूर्णाङ्क		name;
    पूर्णाङ्क		type;
    पूर्णाङ्क		linecount;
    पूर्णाङ्क		width;
    समय_प्रकार	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    पूर्णाङ्क		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    यदि (मानहरू->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	फर्काऊ;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    यदि (name < 0 || name >= अङ्क_REGISTERS)
	फर्काऊ;
    type = vp[2].bv_nr;
    यदि (type != MCHAR && type != MLINE && type != MBLOCK)
	फर्काऊ;
    linecount = vp[3].bv_nr;
    यदि (मानहरू->ga_len < 6 + linecount)
	फर्काऊ;
    width = vp[4].bv_nr;
    यदि (width < 0)
	फर्काऊ;

    यदि (y_read_regs != रिक्त)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    अन्यथा
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (समय_प्रकार)vp[5].bv_nr;
    यदि (y_ptr->y_array != रिक्त && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	फर्काऊ;

    यदि (y_ptr->y_array != रिक्त)
	लागि (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    यदि (y_read_regs == रिक्त)
    {
	यदि (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	यदि ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    यदि (linecount == 0)
    {
	y_ptr->y_array = रिक्त;
	फर्काऊ;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    यदि (y_ptr->y_array == रिक्त)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	फर्काऊ;
    }
    लागि (i = 0; i < linecount; i++)
    {
	यदि (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = रिक्त;
	}
	अन्यथा यदि (vp[i + 6].bv_type != BVAL_STRING)
	{
	    मुक्त(y_ptr->y_array);
	    y_ptr->y_array = रिक्त;
	}
	अन्यथा
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    स्थिर शून्य
write_viminfo_registers(खाता *fp)
{
    पूर्णाङ्क		i, j;
    char_u	*type;
    char_u	c;
    पूर्णाङ्क		अङ्क_पङ्क्तिहरू;
    पूर्णाङ्क		max_अङ्क_पङ्क्तिहरू;
    पूर्णाङ्क		max_kbyte;
    दीर्घ	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    ख_माला_दो(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_अङ्क_पङ्क्तिहरू = get_viminfo_parameter('<');
    यदि (max_अङ्क_पङ्क्तिहरू < 0)
	max_अङ्क_पङ्क्तिहरू = get_viminfo_parameter('"');
    यदि (max_अङ्क_पङ्क्तिहरू == 0)
	फर्काऊ;
    max_kbyte = get_viminfo_parameter('s');
    यदि (max_kbyte == 0)
	फर्काऊ;

    लागि (i = 0; i < अङ्क_REGISTERS; i++)
    {
#यदि defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	यदि (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    जारी;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	यदि (i == TILDE_REGISTER)
	    जारी;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	यदि (y_read_regs != रिक्त
		&& y_read_regs[i].y_array != रिक्त
		&& (y_regs_p[i].y_array == रिक्त ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	अन्यथा यदि (y_regs_p[i].y_array == रिक्त)
	    जारी;
	अन्यथा
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	अङ्क_पङ्क्तिहरू = y_ptr->y_size;
	यदि (अङ्क_पङ्क्तिहरू == 0
		|| (अङ्क_पङ्क्तिहरू == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    जारी;

	यदि (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    लागि (j = 0; j < अङ्क_पङ्क्तिहरू; j++)
		len += (दीर्घ)y_ptr->y_array[j].length + 1L;
	    यदि (len > (दीर्घ)max_kbyte * 1024L)
		जारी;
	}

	विकल्प (y_ptr->y_type)
	{
	    पक्ष MLINE:
		type = (char_u *)"LINE";
		रोक;
	    पक्ष MCHAR:
		type = (char_u *)"CHAR";
		रोक;
	    पक्ष MBLOCK:
		type = (char_u *)"BLOCK";
		रोक;
	    स्वभाव:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		रोक;
	}
	यदि (get_y_previous() == &y_regs_p[i])
	    ख_लिखो(fp, "\"");
	c = get_register_name(i);
	ख_लिखो(fp, "\"%c", c);
	यदि (c == get_execreg_lastc())
	    ख_लिखो(fp, "@");
	ख_लिखो(fp, "\t%s\t%d\n", type, (पूर्णाङ्क)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	यदि (max_अङ्क_पङ्क्तिहरू > 0 && अङ्क_पङ्क्तिहरू > max_अङ्क_पङ्क्तिहरू)
	    अङ्क_पङ्क्तिहरू = max_अङ्क_पङ्क्तिहरू;
	लागि (j = 0; j < अङ्क_पङ्क्तिहरू; j++)
	{
	    अ_दो('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    पूर्णाङ्क	    flags = 0;
	    पूर्णाङ्क	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    यदि (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    यदि (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    ख_लिखो(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, अङ्क_पङ्क्तिहरू, (पूर्णाङ्क)y_ptr->y_width,
		    (दीर्घ)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    लागि (j = 0; j < अङ्क_पङ्क्तिहरू; j++)
	    {
		अ_दो(',', fp);
		--remaining;
		remaining = barline_writestring(fp, y_ptr->y_array[j].string,
								   remaining);
	    }
	    अ_दो('\n', fp);
	}
    }
}

/*
 * Functions relating to reading/writing marks from viminfo
 */

स्थिर xfmark_T *vi_namedfm = रिक्त;
स्थिर xfmark_T *vi_jumplist = रिक्त;
स्थिर पूर्णाङ्क vi_jumplist_len = 0;

    स्थिर शून्य
write_one_mark(खाता *fp_out, पूर्णाङ्क c, pos_T *pos)
{
    यदि (pos->lnum != 0)
	ख_लिखो(fp_out, "\t%c\t%ld\t%d\n", c, (दीर्घ)pos->lnum, (पूर्णाङ्क)pos->col);
}

    स्थिर शून्य
write_buffer_marks(buf_T *buf, खाता *fp_out)
{
    पूर्णाङ्क		i;
    pos_T	pos;

    home_replace(रिक्त, buf->b_ffname, IObuff, IOSIZE, TRUE);
    ख_लिखो(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_घुसाउ);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    लागि (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	यदि (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    लागि (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    स्थिर पूर्णाङ्क
skip_for_viminfo(buf_T *buf)
{
    फर्काऊ bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    स्थिर शून्य
write_viminfo_marks(खाता *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    पूर्णाङ्क		is_mark_set;
    पूर्णाङ्क		i;
    win_T	*win;
    tabpage_T	*tp;

    // Set b_last_cursor for the all buffers that have a window.
    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    ख_माला_दो(_("\n# History of marks within files (newest to oldest):\n"), fp_out);
    FOR_ALL_BUFFERS(buf)
    {
	// Only write something if buffer has been loaded and at least one
	// mark is set.
	यदि (buf->b_marks_read)
	{
	    यदि (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    अन्यथा
	    {
		is_mark_set = FALSE;
		लागि (i = 0; i < NMARKS; i++)
		    यदि (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			रोक;
		    }
	    }
	    यदि (is_mark_set && buf->b_ffname != रिक्त
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		यदि (buflist == रिक्त)
		    write_buffer_marks(buf, fp_out);
		अन्यथा यदि (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    स्थिर शून्य
write_one_filemark(
    खाता	*fp,
    xfmark_T	*fm,
    पूर्णाङ्क		c1,
    पूर्णाङ्क		c2)
{
    char_u	*name;

    यदि (fm->fmark.mark.lnum == 0)	// not set
	फर्काऊ;

    यदि (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    अन्यथा
	name = fm->fname;		// use name from .viminfo
    यदि (name != रिक्त && *name != NUL)
    {
	ख_लिखो(fp, "%c%c  %ld  %ld  ", c1, c2, (दीर्घ)fm->fmark.mark.lnum,
						    (दीर्घ)fm->fmark.mark.col);
	viminfo_writestring(fp, name);

	// Barline: |{bartype},{name},{lnum},{col},{timestamp},{filename}
	// size up to filename: 8 + 3 * 20
	ख_लिखो(fp, "|%d,%d,%ld,%ld,%ld,", BARTYPE_MARK, c2,
		(दीर्घ)fm->fmark.mark.lnum, (दीर्घ)fm->fmark.mark.col,
		(दीर्घ)fm->time_set);
	barline_writestring(fp, name, LSIZE - 70);
	अ_दो('\n', fp);
    }

    यदि (fm->fmark.fnum != 0)
	vim_free(name);
}

    स्थिर शून्य
write_viminfo_filemarks(खाता *fp)
{
    पूर्णाङ्क		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    पूर्णाङ्क		vi_idx;
    पूर्णाङ्क		idx;

    यदि (get_viminfo_parameter('f') == 0)
	फर्काऊ;

    ख_माला_दो(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    लागि (i = 0; i < NMARKS; i++)
    {
	यदि (vi_namedfm != रिक्त
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	अन्यथा
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    यदि (curbuf->b_ffname != रिक्त && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	लागि (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    यदि (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == रिक्त
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != रिक्त
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		रोक;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	लागि ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = रिक्त;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    लागि (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != रिक्त ? &vi_namedfm[vi_idx] : रिक्त;

	यदि (vi_fm != रिक्त
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	अन्यथा
	{
	    fm = &namedfm_p[idx++];
	    यदि (vi_fm != रिक्त
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != रिक्त
			  && fm->fname != रिक्त
			  && STRCMP(vi_fm->fname, fm->fname) == 0)))
		++vi_idx;  // skip duplicate
	}
	write_one_filemark(fp, fm, '\'', i - NMARKS + '0');
    }

    // Write the jumplist with -'
    ख_माला_दो(_("\n# Jumplist (newest first):\n"), fp);
    setpcmark();	// add current cursor position
    cleanup_jumplist(curwin, FALSE);
    vi_idx = 0;
    idx = curwin->w_jumplistlen - 1;
    लागि (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : रिक्त;
	vi_fm = (vi_jumplist != रिक्त && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : रिक्त;
	यदि (fm == रिक्त && vi_fm == रिक्त)
	    रोक;
	यदि (fm == रिक्त || (vi_fm != रिक्त && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	अन्यथा
	    --idx;
	यदि (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != रिक्त
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    पूर्णाङ्क
buf_compare(अचर शून्य *s1, अचर शून्य *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    यदि (buf1->b_last_used == buf2->b_last_used)
	फर्काऊ 0;
    फर्काऊ buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    स्थिर शून्य
copy_viminfo_marks(
    vir_T	*virp,
    खाता	*fp_out,
    garray_T	*buflist,
    पूर्णाङ्क		eof,
    पूर्णाङ्क		flags)
{
    char_u	*पङ्क्ति = virp->vir_पङ्क्ति;
    buf_T	*buf;
    पूर्णाङ्क		अङ्क_marked_files;
    पूर्णाङ्क		load_marks;
    पूर्णाङ्क		copy_marks_out;
    char_u	*str;
    पूर्णाङ्क		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = रिक्त;
#endif
    पूर्णाङ्क		गन्ती = 0;
    पूर्णाङ्क		buflist_used = 0;
    buf_T	*buflist_buf = रिक्त;

    यदि ((name_buf = alloc(LSIZE)) == रिक्त)
	फर्काऊ;
    *name_buf = NUL;

    यदि (fp_out != रिक्त && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	क्विक(buflist->ga_data, (माप_प्रकार)buflist->ga_len,
						परिमाण(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    यदि (fp_out == रिक्त && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	यदि (list != रिक्त)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    अङ्क_marked_files = get_viminfo_parameter('\'');
    जबसम्म (!eof && (गन्ती < अङ्क_marked_files || fp_out == रिक्त))
    {
	यदि (पङ्क्ति[0] != '>')
	{
	    यदि (पङ्क्ति[0] != '\n' && पङ्क्ति[0] != '\r' && पङ्क्ति[0] != '#')
	    {
		यदि (viminfo_error("E576: ", _(e_nonr_missing_gt), पङ्क्ति))
		    रोक;	// too many errors, return now
	    }
	    eof = vim_fgets(पङ्क्ति, LSIZE, virp->vir_fd);
	    जारी;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(पङ्क्ति + 1);
	str = viminfo_readstring(virp, (पूर्णाङ्क)(str - virp->vir_पङ्क्ति), FALSE);
	यदि (str == रिक्त)
	    जारी;
	p = str + STRLEN(str);
	जबसम्म (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	यदि (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	यदि (list != रिक्त)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	यदि (fp_out == रिक्त)
	{
	    यदि ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != रिक्त)
	    {
		यदि (*name_buf == NUL)	    // only need to do this once
		    home_replace(रिक्त, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		यदि (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	अन्यथा // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		यदि (buf->b_ffname != रिक्त)
		{
		    home_replace(रिक्त, buf->b_ffname, name_buf, LSIZE, TRUE);
		    यदि (fnamecmp(str, name_buf) == 0)
			रोक;
		}

	    // Copy marks if the buffer has not been loaded.
	    यदि (buf == रिक्त || !buf->b_marks_read)
	    {
		पूर्णाङ्क	did_read_पङ्क्ति = FALSE;

		यदि (buflist_buf != रिक्त)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    यदि (!viminfo_readline(virp) && पङ्क्ति[0] == TAB)
		    {
			did_read_पङ्क्ति = TRUE;
			यदि (पङ्क्ति[1] == '*')
			{
			    दीर्घ	ltime;

			    माला_पूछो((वर्ण *)पङ्क्ति + 2, "%ld ", &ltime);
			    जबसम्म ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				यदि (++गन्ती >= अङ्क_marked_files)
				    रोक;
				यदि (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = रिक्त;
				    रोक;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			अन्यथा
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    जबसम्म (गन्ती < अङ्क_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++गन्ती;
			    }
			    buflist_buf = रिक्त;
			}

			यदि (गन्ती >= अङ्क_marked_files)
			{
			    vim_free(str);
			    रोक;
			}
		    }
		}

		ख_माला_दो("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		यदि (did_read_पङ्क्ति)
		    ख_माला_दो((वर्ण *)पङ्क्ति, fp_out);

		गन्ती++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	जबसम्म (!(eof = viminfo_readline(virp)) && पङ्क्ति[0] == TAB)
	{
	    यदि (load_marks)
	    {
		यदि (पङ्क्ति[1] != NUL)
		{
		    अचिन्हित u;

		    माला_पूछो((वर्ण *)पङ्क्ति + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    विकल्प (पङ्क्ति[1])
		    {
			पक्ष '"': curbuf->b_last_cursor = pos; रोक;
			पक्ष '^': curbuf->b_last_घुसाउ = pos; रोक;
			पक्ष '.': curbuf->b_last_change = pos; रोक;
			पक्ष '+':
				  // changelist positions are stored oldest
				  // first
				  यदि (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    परिमाण(pos_T) * (JUMPLISTSIZE - 1));
				  अन्यथा
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  रोक;

				  // Using the line number for the last-used
				  // timestamp.
			पक्ष '*': curbuf->b_last_used = pos.lnum; रोक;

			स्वभाव:  यदि ((i = पङ्क्ति[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    अन्यथा यदि (copy_marks_out)
		ख_माला_दो((वर्ण *)पङ्क्ति, fp_out);
	}

	यदि (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		यदि (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    यदि (flags & VIF_ONLY_CURBUF)
		रोक;
	}
    }

    यदि (fp_out != रिक्त)
	// Write any remaining entries from buflist.
	जबसम्म (गन्ती < अङ्क_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++गन्ती;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    शून्य
check_marks_read(शून्य)
{
    यदि (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != रिक्त)
	read_viminfo(रिक्त, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    स्थिर पूर्णाङ्क
read_viminfo_filemark(vir_T *virp, पूर्णाङ्क force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    पूर्णाङ्क		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_पङ्क्ति + 1;
    यदि (*str <= 127
	    && ((*virp->vir_पङ्क्ति == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_पङ्क्ति == '-' && *str == '\'')))
    {
	यदि (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    यदि (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = रिक्त;
	    अन्यथा
	    {
		लागि (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = रिक्त;
	    }
	}
	अन्यथा यदि (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	अन्यथा
	    fm = &namedfm_p[*str - 'A'];
	यदि (fm != रिक्त && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (पूर्णाङ्क)(str - virp->vir_पङ्क्ति),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    फर्काऊ vim_fgets(virp->vir_पङ्क्ति, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    स्थिर शून्य
prepare_viminfo_marks(शून्य)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    स्थिर शून्य
finish_viminfo_marks(शून्य)
{
    पूर्णाङ्क		i;

    यदि (vi_namedfm != रिक्त)
    {
	लागि (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    यदि (vi_jumplist != रिक्त)
    {
	लागि (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    स्थिर शून्य
handle_viminfo_mark(garray_T *मानहरू, पूर्णाङ्क force)
{
    bval_T	*vp = (bval_T *)मानहरू->ga_data;
    पूर्णाङ्क		name;
    linenr_T	lnum;
    colnr_T	col;
    समय_प्रकार	timestamp;
    xfmark_T	*fm = रिक्त;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    यदि (मानहरू->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	फर्काऊ;

    name = vp[0].bv_nr;
    यदि (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	फर्काऊ;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    यदि (lnum <= 0 || col < 0)
	फर्काऊ;
    timestamp = (समय_प्रकार)vp[3].bv_nr;

    यदि (name == '\'')
    {
	यदि (vi_jumplist != रिक्त)
	{
	    यदि (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	अन्यथा
	{
	    पूर्णाङ्क idx;
	    पूर्णाङ्क i;

	    // If we have a timestamp insert it in the right place.
	    यदि (timestamp != 0)
	    {
		लागि (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    यदि (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			रोक;
		    }
		// idx cannot be zero now
		यदि (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    अन्यथा यदि (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    अन्यथा
		idx = -1;

	    यदि (idx >= 0)
	    {
		यदि (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    लागि (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		अन्यथा
		{
		    // Move newer entries forward.
		    लागि (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = रिक्त;
		fm->time_set = 0;
	    }
	}
    }
    अन्यथा
    {
	पूर्णाङ्क		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	यदि (VIM_ISDIGIT(name))
	{
	    यदि (vi_namedfm != रिक्त)
		idx = name - '0' + NMARKS;
	    अन्यथा
	    {
		पूर्णाङ्क i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		लागि (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    यदि (namedfm_p[idx].time_set < timestamp)
			रोक;
		यदि (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    फर्काऊ;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		लागि ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = रिक्त;
	    }
	}
	अन्यथा
	    idx = name - 'A';
	यदि (vi_namedfm != रिक्त)
	    fm = &vi_namedfm[idx];
	अन्यथा
	    fm = &namedfm_p[idx];
    }

    यदि (fm != रिक्त)
    {
	यदि (vi_namedfm != रिक्त || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    यदि (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = रिक्त;
	    }
	    अन्यथा
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    स्थिर पूर्णाङ्क
read_viminfo_barline(vir_T *virp, पूर्णाङ्क got_encoding, पूर्णाङ्क force, पूर्णाङ्क writing)
{
    char_u	*p = virp->vir_पङ्क्ति + 1;
    पूर्णाङ्क		bartype;
    garray_T	मानहरू;
    bval_T	*vp;
    पूर्णाङ्क		i;
    पूर्णाङ्क		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    यदि (*p == '<')
    {
	// Continuation line of an unrecognized item.
	यदि (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_पङ्क्ति);
    }
    अन्यथा
    {
	ga_init2(&मानहरू, परिमाण(bval_T), 20);
	bartype = getdigits(&p);
	विकल्प (bartype)
	{
	    पक्ष BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		यदि (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &मानहरू);
		    vp = (bval_T *)मानहरू.ga_data;
		    यदि (मानहरू.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		रोक;

	    पक्ष BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &मानहरू);
		handle_viminfo_history(&मानहरू, writing);
		रोक;

	    पक्ष BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &मानहरू);
		handle_viminfo_register(&मानहरू, force);
		रोक;

	    पक्ष BARTYPE_MARK:
		read_next = barline_parse(virp, p, &मानहरू);
		handle_viminfo_mark(&मानहरू, force);
		रोक;

	    स्वभाव:
		// copy unrecognized line (for future use)
		यदि (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_पङ्क्ति);
	}
	लागि (i = 0; i < मानहरू.ga_len; ++i)
	{
	    vp = (bval_T *)मानहरू.ga_data + i;
	    यदि (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&मानहरू);
    }

    यदि (read_next)
	फर्काऊ viminfo_readline(virp);
    फर्काऊ FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    स्थिर पूर्णाङ्क
read_viminfo_up_to_marks(
    vir_T	*virp,
    पूर्णाङ्क		forceit,
    पूर्णाङ्क		writing)
{
    पूर्णाङ्क		eof;
    buf_T	*buf;
    पूर्णाङ्क		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    जबसम्म (!eof && virp->vir_पङ्क्ति[0] != '>')
    {
	विकल्प (virp->vir_पङ्क्ति[0])
	{
		// Characters reserved for future expansion, ignored now
	    पक्ष '+': // "+40 /path/dir file", for running vim without args
	    पक्ष '^': // to be defined
	    पक्ष '<': // long line - ignored
		// A comment or empty line.
	    पक्ष NUL:
	    पक्ष '\r':
	    पक्ष '\n':
	    पक्ष '#':
		eof = viminfo_readline(virp);
		रोक;
	    पक्ष '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		रोक;
	    पक्ष '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		रोक;
	    पक्ष '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#अन्यथा
		eof = viminfo_readline(virp);
#endif
		रोक;
	    पक्ष '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		रोक;
	    पक्ष '"':
		// When registers are in bar lines skip the old style register
		// lines.
		यदि (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		अन्यथा
		    गर
		    {
			eof = viminfo_readline(virp);
		    } जबसम्म (!eof && (virp->vir_पङ्क्ति[0] == TAB
						|| virp->vir_पङ्क्ति[0] == '<'));
		रोक;
	    पक्ष '/':	    // Search string
	    पक्ष '&':	    // Substitute search string
	    पक्ष '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_खोज_pattern(virp, forceit, writing);
		रोक;
	    पक्ष '$':
		eof = read_viminfo_sub_string(virp, forceit);
		रोक;
	    पक्ष ':':
	    पक्ष '?':
	    पक्ष '=':
	    पक्ष '@':
		// When history is in bar lines skip the old style history
		// lines.
		यदि (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		अन्यथा
		    eof = viminfo_readline(virp);
		रोक;
	    पक्ष '-':
	    पक्ष '\'':
		// When file marks are in bar lines skip the old style lines.
		यदि (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		अन्यथा
		    eof = viminfo_readline(virp);
		रोक;
	    स्वभाव:
		यदि (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_पङ्क्ति))
		    eof = TRUE;
		अन्यथा
		    eof = viminfo_readline(virp);
		रोक;
	}
    }

    // Finish reading history items.
    यदि (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    फर्काऊ eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    स्थिर शून्य
do_viminfo(खाता *fp_in, खाता *fp_out, पूर्णाङ्क flags)
{
    पूर्णाङ्क		eof = FALSE;
    vir_T	vir;
    पूर्णाङ्क		merge = FALSE;
    पूर्णाङ्क		do_copy_marks = FALSE;
    garray_T	buflist;

    यदि ((vir.vir_पङ्क्ति = alloc(LSIZE)) == रिक्त)
	फर्काऊ;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, परिमाण(char_u *), 100);
    vir.vir_version = -1;

    यदि (fp_in != रिक्त)
    {
	यदि (flags & VIF_WANT_INFO)
	{
	    यदि (fp_out != रिक्त)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != रिक्त);
	    merge = TRUE;
	}
	अन्यथा यदि (flags != 0)
	    // Skip info, find start of marks
	    जबसम्म (!(eof = viminfo_readline(&vir))
		    && vir.vir_पङ्क्ति[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    यदि (fp_out != रिक्त)
    {
	// Write the info:
	ख_लिखो(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	ख_माला_दो(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	ख_माला_दो(_("# Value of 'encoding' when this file was written\n"), fp_out);
	ख_लिखो(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_खोज_pattern(fp_out);
	write_viminfo_sub_string(fp_out);
	write_viminfo_history(fp_out, merge);
	write_viminfo_registers(fp_out);
	finish_viminfo_registers();
#ifdef FEAT_EVAL
	write_viminfo_varlist(fp_out);
#endif
	write_viminfo_filemarks(fp_out);
	finish_viminfo_marks();
	write_viminfo_bufferlist(fp_out);
	write_viminfo_barlines(&vir, fp_out);

	यदि (do_copy_marks)
	    ga_init2(&buflist, परिमाण(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : रिक्त);
    }

    यदि (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	यदि (fp_out != रिक्त)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_पङ्क्ति);
    यदि (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, रिक्त, रिक्त);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    पूर्णाङ्क
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    पूर्णाङ्क		flags)	    // VIF_WANT_INFO et al.
{
    खाता	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    यदि (no_viminfo())
	फर्काऊ FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    यदि (fname == रिक्त)
	फर्काऊ FAIL;
    fp = mch_fopen((वर्ण *)fname, READBIN);

    यदि (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == रिक्त ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    यदि (fp == रिक्त)
	फर्काऊ FAIL;
    यदि (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	ख_बंद(fp);
	फर्काऊ FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, रिक्त, flags);

    ख_बंद(fp);
    फर्काऊ OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    शून्य
write_viminfo(char_u *file, पूर्णाङ्क forceit)
{
    char_u	*fname;
    खाता	*fp_in = रिक्त;	// input viminfo file, if any
    खाता	*fp_out = रिक्त;	// output viminfo file
    char_u	*tempname = रिक्त;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#यदि defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    पूर्णाङ्क		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    पूर्णाङ्क		hidden = FALSE;
#endif

    यदि (no_viminfo())
	फर्काऊ;

    fname = viminfo_filename(file);	// may set to default if NULL
    यदि (fname == रिक्त)
	फर्काऊ;

    fp_in = mch_fopen((वर्ण *)fname, READBIN);
    यदि (fp_in == रिक्त)
    {
	पूर्णाङ्क fd;

	// if it does exist, but we can't read it, don't try writing
	यदि (mch_stat((वर्ण *)fname, &st_new) == 0)
	    गच्छ end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((वर्ण *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	यदि (fd < 0)
	    गच्छ end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    अन्यथा
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	यदि (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != जरा_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    पूर्णाङ्क	tt = सन्देश_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    सन्देश_didany = tt;
	    ख_बंद(fp_in);
	    गच्छ end;
	}
#ifdef MSWIN
	// Get the file attributes of the existing viminfo file.
	hidden = mch_ishidden(fname);
#endif

	// Make tempname, find one that does not exist yet.
	// Beware of a race condition: If someone logs out and all Vim
	// instances exit at the same time a temp file might be created between
	// stat() and open().  Use mch_open() with O_EXCL to avoid that.
	// May try twice: Once normal and once with shortname set, just in
	// case somebody puts his viminfo file in an 8.3 filesystem.
	लागि (;;)
	{
	    पूर्णाङ्क		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#अन्यथा
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#अन्यथा
				    (char_u *)".tmp",
#endif
				    FALSE);
	    यदि (tempname == रिक्त)		// out of memory
		रोक;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    यदि (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    लागि (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		यदि (mch_stat((वर्ण *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    यदि (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			रोक;
		    }
#endif
		}
		अन्यथा
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((वर्ण *)tempname, WRITEBIN);
		    (शून्य)umask(umask_save);
#अन्यथा
		    पूर्णाङ्क	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((वर्ण *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(पूर्णाङ्क)((st_old.st_mode & 0777) | 0600));
		    (शून्य)umask(umask_save);
# अन्यथा
		    fd = mch_open((वर्ण *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    यदि (fd < 0)
		    {
			fp_out = रिक्त;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			यदि (त्रुटि_सं != EEXIST)
			    रोक;
# endif
		    }
		    अन्यथा
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    यदि (fp_out != रिक्त)
			रोक;
		}

		// Assume file exists, try again with another name.
		यदि (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    रोक;
		}
		*wp = next_char;
		--next_char;
	    }

	    यदि (tempname != रिक्त)
		रोक;
	    // continue if shortname was set
	}

#यदि defined(UNIX) && defined(HAVE_FCHOWN)
	यदि (tempname != रिक्त && fp_out != रिक्त)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    यदि (mch_stat((वर्ण *)tempname, &tmp_st) >= 0)
	    {
		यदि (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		यदि (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (शून्य)mch_setperm(tempname, 0600);
	    }
	    अन्यथा
		// can't stat the file, set conservative permissions
		(शून्य)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    यदि (fp_out == रिक्त)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == रिक्त || tempname == रिक्त) ? fname : tempname);
	यदि (fp_in != रिक्त)
	    ख_बंद(fp_in);
	गच्छ end;
    }

    यदि (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    यदि (ख_बंद(fp_out) == खातापूर्ण)
	++viminfo_errcnt;

    यदि (fp_in != रिक्त)
    {
	ख_बंद(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	यदि (viminfo_errcnt == 0)
	{
	    यदि (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    अन्यथा यदि (hidden)
		mch_hide(fname);
#endif
	}
	यदि (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    शून्य
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    यदि (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    यदि (eap->cmdidx == CMD_rviminfo)
    {
	यदि (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    अन्यथा
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
