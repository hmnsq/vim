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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
வகைவரை கட்டமைப்பு
{
    char_u	*vir_வரி;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    முழு		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

வகைவரை எண்ணிக்கை {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

வகைவரை கட்டமைப்பு {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    முழு		bv_len;		// length of bv_string
    முழு		bv_allocated;	// bv_string was allocated
} bval_T;

#எனில் defined(FEAT_VIMINFO)

நிலையான முழு  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    நிலையான char_u *
find_viminfo_parameter(முழு type)
{
    char_u  *p;

    க்கு (p = p_viminfo; *p; ++p)
    {
	எனில் (*p == type)
	    திரும்பு p + 1;
	எனில் (*p == 'n')		    // 'n' is always the last one
	    நிறுத்து;
	p = vim_strchr(p, ',');	    // skip until next ','
	எனில் (p == வெற்று)		    // hit the end without finding parameter
	    நிறுத்து;
    }
    திரும்பு வெற்று;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    முழு
get_viminfo_parameter(முழு type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    எனில் (p != வெற்று && VIM_ISDIGIT(*p))
	திரும்பு atoi((எழுத்து *)p);
    திரும்பு -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    நிலையான char_u *
viminfo_filename(char_u *file)
{
    எனில் (file == வெற்று || *file == NUL)
    {
	size_t  len;

	எனில் (*p_viminfofile != NUL)
	    file = p_viminfofile;
	இல்லையெனில் எனில் ((file = find_viminfo_parameter('n')) == வெற்று || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    எனில் (mch_getenv((char_u *)"SYS$LOGIN") == வெற்று)
# இல்லையெனில்
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    எனில் (STRCMP(vim_getenv((char_u *)"HOME", வெற்று), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == வெற்று)
#  இல்லையெனில்
	    எனில் (mch_getenv((char_u *)"HOME") == வெற்று)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		எனில் (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		இல்லையெனில்
		    file = (char_u *)VIMINFO_FILE;
	    }
	    இல்லையெனில்
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	திரும்பு vim_strnsave(file, len);
    }

    திரும்பு vim_strsave(file);
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
    நிலையான வெறுமை
viminfo_writestring(FILE *fd, char_u *p)
{
    முழு		c;
    char_u	*s;
    முழு		len = 0;

    க்கு (s = p; *s != NUL; ++s)
    {
	எனில் (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    எனில் (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    போது ((c = *p++) != NUL)
    {
	எனில் (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    எனில் (c == '\n')
		c = 'n';
	}
	putc(c, fd);
    }
    putc('\n', fd);
}

/*
 * Write a string in quotes that barline_parse() can read back.
 * Breaks the line in less than LSIZE pieces when needed.
 * Returns remaining characters in the line.
 */
    நிலையான முழு
barline_writestring(FILE *fd, char_u *s, முழு remaining_start)
{
    char_u *p;
    முழு	    remaining = remaining_start;
    முழு	    len = 2;

    // Count the number of characters produced, including quotes.
    க்கு (p = s; *p != NUL; ++p)
    {
	எனில் (*p == NL)
	    len += 2;
	இல்லையெனில் எனில் (*p == '"' || *p == '\\')
	    len += 2;
	இல்லையெனில்
	    ++len;
    }
    எனில் (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    க்கு (p = s; *p != NUL; ++p)
    {
	எனில் (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	இல்லையெனில் எனில் (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	இல்லையெனில்
	    putc(*p, fd);
	--remaining;

	எனில் (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    திரும்பு remaining - 2;
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
    நிலையான char_u *
viminfo_readstring(
    vir_T	*virp,
    முழு		off,		    // offset for virp->vir_line
    முழு		convert)	    // convert the string
{
    char_u	*retval = வெற்று;
    char_u	*s, *d;
    long	len;

    எனில் (virp->vir_வரி[off] == Ctrl_V && vim_isdigit(virp->vir_வரி[off + 1]))
    {
	len = atol((எழுத்து *)virp->vir_வரி + off + 1);
	எனில் (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	எனில் (retval == வெற்று)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (வெறுமை)vim_fgets(virp->vir_வரி, 10, virp->vir_fd);
	    திரும்பு வெற்று;
	}
	(வெறுமை)vim_fgets(retval, (முழு)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    இல்லையெனில்
    {
	retval = vim_strsave(virp->vir_வரி + off);
	எனில் (retval == வெற்று)
	    திரும்பு வெற்று;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    போது (*s != NUL && *s != '\n')
    {
	எனில் (s[0] == Ctrl_V && s[1] != NUL)
	{
	    எனில் (s[1] == 'n')
		*d++ = '\n';
	    இல்லையெனில்
		*d++ = Ctrl_V;
	    s += 2;
	}
	இல்லையெனில்
	    *d++ = *s++;
    }
    *d = NUL;

    எனில் (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, வெற்று);
	எனில் (d != வெற்று)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    திரும்பு retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    நிலையான முழு
viminfo_readline(vir_T *virp)
{
    திரும்பு vim_fgets(virp->vir_வரி, LSIZE, virp->vir_fd);
}

    நிலையான முழு
read_viminfo_bufferlist(
    vir_T	*virp,
    முழு		writing)
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
    எனில் (xline != வெற்று && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != வெற்று)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	எனில் (tab != வெற்று)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((எழுத்து *)tab);
	    tab = vim_strrchr(xline, '\t');
	    எனில் (tab != வெற்று)
	    {
		*tab++ = '\0';
		lnum = atol((எழுத்து *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	எனில் (buf != வெற்று)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    திரும்பு viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    நிலையான முழு
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    முழு	    part_len;
    முழு	    retval = FALSE;

    name = home_replace_save(வெற்று, name);
    எனில் (name == வெற்று)
	திரும்பு FALSE;
    க்கு (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, அளவு(part), ", ");
	எனில் (part[0] == 'r')
	{
	    எனில் (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		நிறுத்து;
	    }
	}
    }
    vim_free(name);
    திரும்பு retval;
}

    நிலையான வெறுமை
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*வரி;
    முழு		max_buffers;

    எனில் (find_viminfo_parameter('%') == வெற்று)
	திரும்பு;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define வரி_BUF_LEN (MAXPATHL + 40)
    வரி = alloc(வரி_BUF_LEN);
    எனில் (வரி == வெற்று)
	திரும்பு;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	எனில் (buf->b_fname == வெற்று
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    தொடர்;

	எனில் (max_buffers-- == 0)
	    நிறுத்து;
	putc('%', fp);
	linelen = home_replace(வெற்று, buf->b_ffname, வரி, MAXPATHL, TRUE);
	vim_snprintf((எழுத்து *)வரி + linelen, வரி_BUF_LEN - linelen, "\t%ld\t%d",
			(long)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, வரி);
    }
    vim_free(வரி);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
நிலையான histentry_T *viminfo_history[HIST_மொத்தம்] =
					       {வெற்று, வெற்று, வெற்று, வெற்று, வெற்று};
நிலையான முழு	viminfo_hisidx[HIST_மொத்தம்] = {0, 0, 0, 0, 0};
நிலையான முழு	viminfo_hislen[HIST_மொத்தம்] = {0, 0, 0, 0, 0};
நிலையான முழு	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    நிலையான முழு
hist_type2char(
    முழு	    type,
    முழு	    use_question)	    // use '?' instead of '/'
{
    எனில் (type == HIST_CMD)
	திரும்பு ':';
    எனில் (type == HIST_தேடல்)
    {
	எனில் (use_question)
	    திரும்பு '?';
	இல்லையெனில்
	    திரும்பு '/';
    }
    எனில் (type == HIST_கோவை)
	திரும்பு '=';
    திரும்பு '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    நிலையான வெறுமை
prepare_viminfo_history(முழு asklen, முழு writing)
{
    முழு	    i;
    முழு	    இலக்கம்;
    முழு	    type;
    முழு	    len;
    முழு	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    எனில் (asklen > hislen)
	asklen = hislen;

    க்கு (type = 0; type < HIST_மொத்தம்; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	க்கு (i = 0, இலக்கம் = 0; i < hislen; i++)
	    எனில் (histentry[i].hisstr == வெற்று || histentry[i].viminfo)
		இலக்கம்++;
	len = asklen;
	எனில் (இலக்கம் > len)
	    len = இலக்கம்;
	எனில் (len <= 0)
	    viminfo_history[type] = வெற்று;
	இல்லையெனில்
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	எனில் (viminfo_history[type] == வெற்று)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    நிலையான முழு
read_viminfo_history(vir_T *virp, முழு writing)
{
    முழு		type;
    long_u	len;
    char_u	*val = வெற்று;
    char_u	*p;

    type = hist_char2type(virp->vir_வரி[0]);
    எனில் (viminfo_hisidx[type] >= viminfo_hislen[type])
	செல் done;

    val = viminfo_readstring(virp, 1, TRUE);
    எனில் (val == வெற்று || *val == NUL)
	செல் done;

    முழு sep = (*val == ' ' ? NUL : *val);

    எனில் (in_history(type, val + (type == HIST_தேடல்), viminfo_add_at_front,
								sep, writing))
	செல் done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    எனில் (type == HIST_தேடல்)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	எனில் (p == வெற்று)
	    செல் done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    இல்லையெனில்
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	எனில் (p == வெற்று)
	    செல் done;

	// Not a search entry: No separator in the viminfo
	// file, add a NUL separator.
	mch_memmove(p, val, (size_t)len + 1);	// +1 to include the NUL
	p[len + 1] = NUL;			// put the separator *after* the string's NUL
    }
    viminfo_history[type][viminfo_hisidx[type]].hisstr = p;
    viminfo_history[type][viminfo_hisidx[type]].hisstrlen = (size_t)len;
    viminfo_history[type][viminfo_hisidx[type]].time_set = 0;
    viminfo_history[type][viminfo_hisidx[type]].viminfo = TRUE;
    viminfo_history[type][viminfo_hisidx[type]].hisnum = 0;
    viminfo_hisidx[type]++;

done:
    vim_free(val);
    திரும்பு viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    நிலையான வெறுமை
handle_viminfo_history(
	garray_T    *மதிப்புகள்,
	முழு	    writing)
{
    முழு		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)மதிப்புகள்->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    எனில் (மதிப்புகள்->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	திரும்பு;

    type = vp[0].bv_nr;
    எனில் (type >= HIST_மொத்தம்)
	திரும்பு;

    எனில் (viminfo_hisidx[type] >= viminfo_hislen[type])
	திரும்பு;

    val = vp[3].bv_string;
    எனில் (val == வெற்று || *val == NUL)
	திரும்பு;

    முழு sep = type == HIST_தேடல் && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    முழு idx;
    முழு overwrite = FALSE;

    எனில் (in_history(type, val, viminfo_add_at_front, sep, writing))
	திரும்பு;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    க்கு (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	எனில் (STRCMP(val, p) == 0
		&& (type != HIST_தேடல் || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    நிறுத்து;
	}
    }

    எனில் (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    இல்லையெனில்
	len = 0; // for picky compilers
    எனில் (p != வெற்று)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	எனில் (!overwrite)
	{
	    mch_memmove(p, val, (size_t)len + 1);
	    // Put the separator after the NUL.
	    p[len + 1] = sep;
	    viminfo_history[type][idx].hisstr = p;
	    viminfo_history[type][idx].hisstrlen = (size_t)len;
	    viminfo_history[type][idx].hisnum = 0;
	    viminfo_history[type][idx].viminfo = TRUE;
	    viminfo_hisidx[type]++;
	}
    }
}

/*
 * Concatenate history lines from viminfo after the lines typed in this Vim.
 */
    நிலையான வெறுமை
concat_history(முழு type)
{
    முழு		idx;
    முழு		i;
    முழு		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    முழு		*hisidx = get_hisidx(type);
    முழு		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    எனில் (idx >= hislen)
	idx -= hislen;
    இல்லையெனில் எனில் (idx < 0)
	idx = hislen - 1;
    எனில் (viminfo_add_at_front)
	*hisidx = idx;
    இல்லையெனில்
    {
	எனில் (*hisidx == -1)
	    *hisidx = hislen - 1;
	செய்
	{
	    எனில் (histentry[idx].hisstr != வெற்று || histentry[idx].viminfo)
		நிறுத்து;
	    எனில் (++idx == hislen)
		idx = 0;
	} போது (idx != *hisidx);
	எனில் (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    க்கு (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	எனில் (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    க்கு (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    நிலையான முழு
sort_hist(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    எனில் (p1->time_set < p2->time_set) திரும்பு -1;
    எனில் (p1->time_set > p2->time_set) திரும்பு 1;
    திரும்பு 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    நிலையான வெறுமை
merge_history(முழு type)
{
    முழு		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    முழு		i;
    முழு		len;
    முழு		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    முழு		*hisidx = get_hisidx(type);
    முழு		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    எனில் (tot_hist == வெற்று || new_hist == வெற்று)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	திரும்பு;
    }
    க்கு (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    க்கு (i = 0; i < hislen; i++)
	எனில் (histentry[i].hisstr != வெற்று)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((வெறுமை *)tot_hist, (size_t)len, அளவு(histentry_T *), sort_hist);

    // Keep the newest ones.
    க்கு (i = 0; i < hislen; i++)
    {
	எனில் (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = வெற்று;
	    tot_hist[i]->hisstrlen = 0;
	    எனில் (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	இல்லையெனில்
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    க்கு (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    க்கு (i = 0; i < hislen; i++)
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
    நிலையான வெறுமை
finish_viminfo_history(vir_T *virp)
{
    முழு	type;
    முழு merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    க்கு (type = 0; type < HIST_மொத்தம்; ++type)
    {
	எனில் (get_histentry(type) == வெற்று)
	    தொடர்;

	எனில் (merge)
	    merge_history(type);
	இல்லையெனில்
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
    நிலையான வெறுமை
write_viminfo_history(FILE *fp, முழு merge)
{
    முழு	    i;
    முழு	    type;
    முழு	    இலக்கம்_saved;
    முழு     round;
    முழு	    hislen;

    init_history();
    hislen = get_hislen();
    எனில் (hislen == 0)
	திரும்பு;
    க்கு (type = 0; type < HIST_மொத்தம்; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	முழு	    *hisidx = get_hisidx(type);

	இலக்கம்_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	எனில் (இலக்கம்_saved == 0)
	    தொடர்;
	எனில் (இலக்கம்_saved < 0)  // Use default
	    இலக்கம்_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_தேடல் ? _("Search String") :
			    type == HIST_கோவை ? _("Expression") :
			    type == HIST_உள்ளீடு ? _("Input Line") :
					_("Debug Line"));
	எனில் (இலக்கம்_saved > hislen)
	    இலக்கம்_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	க்கு (round = 1; round <= 2; ++round)
	{
	    எனில் (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    இல்லையெனில் எனில் (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    இல்லையெனில்
		// empty list
		i = -1;
	    எனில் (i >= 0)
		போது (இலக்கம்_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    முழு	    c = NUL;

		    எனில் (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    இல்லையெனில்
		    {
			எனில் (viminfo_history[type] == வெற்று)
			{
			    p = வெற்று;
			    plen = 0;
			    timestamp = 0;
			}
			இல்லையெனில்
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    எனில் (p != வெற்று && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--இலக்கம்_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			எனில் (type == HIST_தேடல்)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    எழுத்து    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    எனில் (c == NUL)
				cbuf[0] = NUL;
			    இல்லையெனில்
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    எனில் (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			எனில் (--i < 0)
			    i = hislen - 1;
			எனில் (i == *hisidx)
			    நிறுத்து;
		    }
		    இல்லையெனில்
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	க்கு (i = 0; i < viminfo_hisidx[type]; ++i)
	    எனில் (viminfo_history[type] != வெற்று)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    நிலையான வெறுமை
write_viminfo_barlines(vir_T *virp, FILE *fp_out)
{
    முழு		i;
    garray_T	*gap = &virp->vir_barlines;
    முழு		seen_useful = FALSE;
    எழுத்து	*வரி;

    எனில் (gap->ga_len <= 0)
	திரும்பு;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    க்கு (i = 0; i < gap->ga_len; ++i)
    {
	வரி = ((எழுத்து **)(gap->ga_data))[i];
	எனில் (seen_useful || வரி[1] != '<')
	{
	    fputs(வரி, fp_out);
	    seen_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    நிலையான முழு
barline_parse(vir_T *virp, char_u *text, garray_T *மதிப்புகள்)
{
    char_u  *p = text;
    char_u  *nextp = வெற்று;
    char_u  *buf = வெற்று;
    bval_T  *மதிப்பு;
    முழு	    i;
    முழு	    allocated = FALSE;
    முழு	    eof;
    char_u  *sconv;
    முழு	    converted;

    போது (*p == ',')
    {
	++p;
	எனில் (ga_grow(மதிப்புகள், 1) == FAIL)
	    நிறுத்து;
	மதிப்பு = (bval_T *)(மதிப்புகள்->ga_data) + மதிப்புகள்->ga_len;

	எனில் (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    எனில் (!allocated)
	    {
		க்கு (i = 0; i < மதிப்புகள்->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(மதிப்புகள்->ga_data) + i;

		    எனில் (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    எனில் (vim_isdigit(p[1]))
	    {
		size_t len;
		size_t todo;
		size_t n;

		// String value was split into lines that are each shorter
		// than LSIZE:
		//     |{bartype},>{length of "{text}{text2}"}
		//     |<"{text1}
		//     |<{text2}",{value}
		// Length includes the quotes.
		++p;
		len = getdigits(&p);
		buf = alloc(len + 1);
		எனில் (buf == வெற்று)
		    திரும்பு TRUE;
		p = buf;
		க்கு (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    எனில் (eof || virp->vir_வரி[0] != '|'
						  || virp->vir_வரி[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			திரும்பு eof || virp->vir_வரி[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_வரி);
		    போது (n > 2 && (virp->vir_வரி[n - 1] == NL
					     || virp->vir_வரி[n - 1] == CAR))
			--n;
		    n -= 2;
		    எனில் (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_வரி + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_வரி + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    இல்லையெனில்
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		எனில் (eof || virp->vir_வரி[0] != '|'
					      || virp->vir_வரி[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    திரும்பு eof || virp->vir_வரி[0] == '|';
		p = virp->vir_வரி + 2;
	    }
	}

	எனில் (SAFE_isdigit(*p))
	{
	    மதிப்பு->bv_type = BVAL_NR;
	    மதிப்பு->bv_nr = getdigits(&p);
	    ++மதிப்புகள்->ga_len;
	}
	இல்லையெனில் எனில் (*p == '"')
	{
	    முழு	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    போது (*p != '"')
	    {
		எனில் (*p == NL || *p == NUL)
		    திரும்பு TRUE;  // syntax error, drop the value
		எனில் (*p == '\\')
		{
		    ++p;
		    எனில் (*p == 'n')
			s[len++] = '\n';
		    இல்லையெனில்
			s[len++] = *p;
		    ++p;
		}
		இல்லையெனில்
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    மதிப்பு->bv_tofree = வெற்று;
	    எனில் (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, வெற்று);
		எனில் (sconv != வெற்று)
		{
		    எனில் (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			மதிப்பு->bv_tofree = buf;
		    s = sconv;
		    len = (முழு)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    எனில் (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    மதிப்பு->bv_string = s;
	    மதிப்பு->bv_type = BVAL_STRING;
	    மதிப்பு->bv_len = len;
	    மதிப்பு->bv_allocated = allocated || converted;
	    ++மதிப்புகள்->ga_len;
	    எனில் (nextp != வெற்று)
	    {
		// values following a long string
		p = nextp;
		nextp = வெற்று;
	    }
	}
	இல்லையெனில் எனில் (*p == ',')
	{
	    மதிப்பு->bv_type = BVAL_EMPTY;
	    ++மதிப்புகள்->ga_len;
	}
	இல்லையெனில்
	    நிறுத்து;
    }
    திரும்பு TRUE;
}

    நிலையான வெறுமை
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    நிலையான முழு
no_viminfo(வெறுமை)
{
    // "vim -i NONE" does not read or write a viminfo file
    திரும்பு STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    நிலையான முழு
viminfo_error(எழுத்து *errnum, எழுத்து *message, char_u *வரி)
{
    vim_snprintf((எழுத்து *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, வரி, IOSIZE - STRLEN(IObuff) - 1);
    எனில் (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((எழுத்து *)IObuff);
    எனில் (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    நிலையான முழு
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    முழு		i;

    எனில் (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_வரி, '=');
	எனில் (p != வெற்று)
	{
	    // remove trailing newline
	    ++p;
	    க்கு (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    திரும்பு viminfo_readline(virp);
}

#எனில் defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    நிலையான முழு
read_viminfo_varlist(vir_T *virp, முழு writing)
{
    char_u	*tab;
    முழு		type = VAR_எண்;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    எனில் (!writing && (find_viminfo_parameter('!') != வெற்று))
    {
	tab = vim_strchr(virp->vir_வரி + 1, '\t');
	எனில் (tab != வெற்று)
	{
	    *tab++ = '\0';	// isolate the variable name
	    தேர்வு (*tab)
	    {
		நிலை 'S': type = VAR_STRING; நிறுத்து;
		நிலை 'F': type = VAR_FLOAT; நிறுத்து;
		நிலை 'D': type = VAR_DICT; நிறுத்து;
		நிலை 'L': type = VAR_LIST; நிறுத்து;
		நிலை 'B': type = VAR_BLOB; நிறுத்து;
		நிலை 'X': type = VAR_SPECIAL; நிறுத்து;
		நிலை 'T': type = VAR_TUPLE; நிறுத்து;
	    }

	    tab = vim_strchr(tab, '\t');
	    எனில் (tab != வெற்று)
	    {
		tv.v_type = type;
		எனில் (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (முழு)(tab - virp->vir_வரி + 1), TRUE);
		இல்லையெனில் எனில் (type == VAR_FLOAT)
		    (வெறுமை)string2float(tab + 1, &tv.vval.v_float, FALSE);
		இல்லையெனில்
		{
		    tv.vval.v_எண் = atol((எழுத்து *)tab + 1);
		    எனில் (type == VAR_SPECIAL && (tv.vval.v_எண் == VVAL_FALSE
					     || tv.vval.v_எண் == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		எனில் (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_கோவை(tv.vval.v_string, வெற்று);

		    எனில் (etv == வெற்று)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    இல்லையெனில்
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		இல்லையெனில் எனில் (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    எனில் (blob == வெற்று)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    இல்லையெனில்
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_entry);
		set_var(virp->vir_வரி + 1, &tv, FALSE);
		restore_funccal();

		எனில் (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		இல்லையெனில் எனில் (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    திரும்பு viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    நிலையான வெறுமை
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    முழு		todo;
    எழுத்து	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    எனில் (find_viminfo_parameter('!') == வெற்று)
	திரும்பு;

    fputs(_("\n# global variables:\n"), fp);

    todo = (முழு)gvht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(gvht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    எனில் (var_flavour(this_var->di_சாவி) == VAR_FLAVOUR_VIMINFO)
	    {
		தேர்வு (this_var->di_tv.v_type)
		{
		    நிலை VAR_STRING:  s = "STR"; நிறுத்து;
		    நிலை VAR_எண்:  s = "NUM"; நிறுத்து;
		    நிலை VAR_FLOAT:   s = "FLO"; நிறுத்து;
		    நிலை VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      முழு	copyID = get_copyID();

			      s = "DIC";
			      எனில் (di != வெற்று && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, வெற்று, வெற்று)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  தொடர்;
			      நிறுத்து;
			  }
		    நிலை VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      முழு	copyID = get_copyID();

			      s = "LIS";
			      எனில் (l != வெற்று && !set_ref_in_list_பொருட்கள்(
						       l, copyID, வெற்று, வெற்று)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  தொடர்;
			      நிறுத்து;
			  }
		    நிலை VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      முழு	copyID = get_copyID();

			      s = "TUP";
			      எனில் (tuple != வெற்று && !set_ref_in_tuple_பொருட்கள்(
					       tuple, copyID, வெற்று, வெற்று)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  தொடர்;
			      நிறுத்து;
			  }
		    நிலை VAR_BLOB:    s = "BLO"; நிறுத்து;
		    நிலை VAR_BOOL:    s = "XPL"; நிறுத்து;  // backwards compat.
		    நிலை VAR_SPECIAL: s = "XPL"; நிறுத்து;

		    நிலை VAR_UNKNOWN:
		    நிலை VAR_ANY:
		    நிலை VAR_VOID:
		    நிலை VAR_FUNC:
		    நிலை VAR_PARTIAL:
		    நிலை VAR_JOB:
		    நிலை VAR_CHANNEL:
		    நிலை VAR_INSTR:
		    நிலை VAR_CLASS:
		    நிலை VAR_OBJECT:
		    நிலை VAR_TYPEALIAS:
				      தொடர்;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_சாவி, s);
		எனில் (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((எழுத்து *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_எண்);
		    p = numbuf;
		    tofree = வெற்று;
		}
		இல்லையெனில்
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		எனில் (p != வெற்று)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    நிலையான முழு
read_viminfo_sub_string(vir_T *virp, முழு force)
{
    எனில் (force || get_old_sub() == வெற்று)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    திரும்பு viminfo_readline(virp);
}

    நிலையான வெறுமை
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    எனில் (get_viminfo_parameter('/') == 0 || old_sub == வெற்று)
	திரும்பு;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    நிலையான முழு
read_viminfo_தேடல்_pattern(vir_T *virp, முழு force, முழு writing)
{
    char_u	*lp;
    முழு		idx = -1;
    முழு		magic = FALSE;
    முழு		no_scs = FALSE;
    முழு		off_வரி = FALSE;
    முழு		off_end = 0;
    long	off = 0;
    முழு		setlast = FALSE;
#ifdef FEAT_தேடல்_EXTRA
    நிலையான முழு	hlsearch_on = FALSE;
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
    lp = virp->vir_வரி;
    எனில் (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	எனில் (lp[1] == 'M')		// magic on
	    magic = TRUE;
	எனில் (lp[2] == 's')
	    no_scs = TRUE;
	எனில் (lp[3] == 'L')
	    off_வரி = TRUE;
	எனில் (lp[4] == 'E')
	    off_end = தேடல்_END;
	lp += 5;
	off = getdigits(&lp);
    }
    எனில் (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    எனில் (lp[0] == '/')
	idx = RE_தேடல்;
    இல்லையெனில் எனில் (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_தேடல்_EXTRA
    இல்லையெனில் எனில் (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    இல்லையெனில் எனில் (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    எனில் (idx >= 0)
    {
	spat = get_spat(idx);
	எனில் (force || spat->pat == வெற்று)
	{
	    val = viminfo_readstring(virp, (முழு)(lp - virp->vir_வரி + 1),
									TRUE);
	    எனில் (val != வெற்று)
	    {
		எனில் (!writing)
		{
		    set_last_தேடல்_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.வரி = off_வரி;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_தேடல்_EXTRA
		    எனில் (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    திரும்பு viminfo_readline(virp);
}

    நிலையான வெறுமை
wvsp_one(
    FILE	*fp,	// file to write to
    முழு		idx,	// spats[] index
    எழுத்து	*s,	// search pat
    முழு		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    எனில் (spat->pat == வெற்று)
	திரும்பு;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.வரி ? 'L' : 'l',	// line offset
	    spat->off.end  ? 'E' : 'e',	// offset from end
	    spat->off.off,			// offset
	    get_spat_last_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    நிலையான வெறுமை
write_viminfo_தேடல்_pattern(FILE *fp)
{
    எனில் (get_viminfo_parameter('/') == 0)
	திரும்பு;

#ifdef FEAT_தேடல்_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != வெற்று) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_தேடல், "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

நிலையான yankreg_T *y_read_regs = வெற்று;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    நிலையான வெறுமை
prepare_viminfo_registers(வெறுமை)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, இலக்கம்_REGISTERS);
}

    நிலையான வெறுமை
finish_viminfo_registers(வெறுமை)
{
    முழு		i;
    முழு		j;

    எனில் (y_read_regs == வெற்று)
	திரும்பு;

    க்கு (i = 0; i < இலக்கம்_REGISTERS; ++i)
	எனில் (y_read_regs[i].y_array != வெற்று)
	{
	    க்கு (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    நிலையான முழு
read_viminfo_register(vir_T *virp, முழு force)
{
    முழு		eof;
    முழு		do_it = TRUE;
    முழு		size;
    முழு		limit;
    முழு		i;
    முழு		set_prev = FALSE;
    char_u	*str;
    string_T	*array = வெற்று;
    முழு		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_வரி + 1;

    // If the line starts with "" this is the y_previous register.
    எனில் (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    எனில் (!ASCII_ISALNUM(*str) && *str != '-')
    {
	எனில் (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_வரி))
	    திரும்பு TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    எனில் (!force && y_current_p->y_array != வெற்று)
	do_it = FALSE;

    எனில் (*str == '@')
    {
	// "x@: register x used for @@
	எனில் (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    எனில் (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	எனில் (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	எனில் (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	இல்லையெனில் எனில் (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	இல்லையெனில்
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    போது (!(eof = viminfo_readline(virp))
		    && (virp->vir_வரி[0] == TAB || virp->vir_வரி[0] == '<'))
    {
	எனில் (do_it)
	{
	    எனில் (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * அளவு(string_T));

		எனில் (new_array == வெற்று)
		{
		    do_it = FALSE;
		    நிறுத்து;
		}
		க்கு (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    எனில் (str != வெற்று)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    இல்லையெனில்
		// error, don't store the result
		do_it = FALSE;
	}
    }

    எனில் (do_it)
    {
	// free y_array[]
	க்கு (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	எனில் (size == 0)
	{
	    y_current_p->y_array = வெற்று;
	}
	இல்லையெனில்
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    க்கு (i = 0; i < size; i++)
	    {
		எனில் (y_current_p->y_array == வெற்று)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		இல்லையெனில்
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    இல்லையெனில்
    {
	// Free array[] if it was filled.
	க்கு (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    திரும்பு eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    நிலையான வெறுமை
handle_viminfo_register(garray_T *மதிப்புகள், முழு force)
{
    bval_T	*vp = (bval_T *)மதிப்புகள்->ga_data;
    முழு		flags;
    முழு		name;
    முழு		type;
    முழு		linecount;
    முழு		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    முழு		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    எனில் (மதிப்புகள்->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	திரும்பு;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    எனில் (name < 0 || name >= இலக்கம்_REGISTERS)
	திரும்பு;
    type = vp[2].bv_nr;
    எனில் (type != MCHAR && type != MLINE && type != MBLOCK)
	திரும்பு;
    linecount = vp[3].bv_nr;
    எனில் (மதிப்புகள்->ga_len < 6 + linecount)
	திரும்பு;
    width = vp[4].bv_nr;
    எனில் (width < 0)
	திரும்பு;

    எனில் (y_read_regs != வெற்று)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    இல்லையெனில்
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    எனில் (y_ptr->y_array != வெற்று && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	திரும்பு;

    எனில் (y_ptr->y_array != வெற்று)
	க்கு (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    எனில் (y_read_regs == வெற்று)
    {
	எனில் (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	எனில் ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    எனில் (linecount == 0)
    {
	y_ptr->y_array = வெற்று;
	திரும்பு;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    எனில் (y_ptr->y_array == வெற்று)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	திரும்பு;
    }
    க்கு (i = 0; i < linecount; i++)
    {
	எனில் (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = வெற்று;
	}
	இல்லையெனில் எனில் (vp[i + 6].bv_type != BVAL_STRING)
	{
	    விடுவி(y_ptr->y_array);
	    y_ptr->y_array = வெற்று;
	}
	இல்லையெனில்
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    நிலையான வெறுமை
write_viminfo_registers(FILE *fp)
{
    முழு		i, j;
    char_u	*type;
    char_u	c;
    முழு		இலக்கம்_வரிகள்;
    முழு		max_இலக்கம்_வரிகள்;
    முழு		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_இலக்கம்_வரிகள் = get_viminfo_parameter('<');
    எனில் (max_இலக்கம்_வரிகள் < 0)
	max_இலக்கம்_வரிகள் = get_viminfo_parameter('"');
    எனில் (max_இலக்கம்_வரிகள் == 0)
	திரும்பு;
    max_kbyte = get_viminfo_parameter('s');
    எனில் (max_kbyte == 0)
	திரும்பு;

    க்கு (i = 0; i < இலக்கம்_REGISTERS; i++)
    {
#எனில் defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	எனில் (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    தொடர்;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	எனில் (i == TILDE_REGISTER)
	    தொடர்;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	எனில் (y_read_regs != வெற்று
		&& y_read_regs[i].y_array != வெற்று
		&& (y_regs_p[i].y_array == வெற்று ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	இல்லையெனில் எனில் (y_regs_p[i].y_array == வெற்று)
	    தொடர்;
	இல்லையெனில்
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	இலக்கம்_வரிகள் = y_ptr->y_size;
	எனில் (இலக்கம்_வரிகள் == 0
		|| (இலக்கம்_வரிகள் == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    தொடர்;

	எனில் (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    க்கு (j = 0; j < இலக்கம்_வரிகள்; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    எனில் (len > (long)max_kbyte * 1024L)
		தொடர்;
	}

	தேர்வு (y_ptr->y_type)
	{
	    நிலை MLINE:
		type = (char_u *)"LINE";
		நிறுத்து;
	    நிலை MCHAR:
		type = (char_u *)"CHAR";
		நிறுத்து;
	    நிலை MBLOCK:
		type = (char_u *)"BLOCK";
		நிறுத்து;
	    இயல்பு:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		நிறுத்து;
	}
	எனில் (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	எனில் (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (முழு)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	எனில் (max_இலக்கம்_வரிகள் > 0 && இலக்கம்_வரிகள் > max_இலக்கம்_வரிகள்)
	    இலக்கம்_வரிகள் = max_இலக்கம்_வரிகள்;
	க்கு (j = 0; j < இலக்கம்_வரிகள்; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    முழு	    flags = 0;
	    முழு	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    எனில் (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    எனில் (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, இலக்கம்_வரிகள், (முழு)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    க்கு (j = 0; j < இலக்கம்_வரிகள்; j++)
	    {
		putc(',', fp);
		--remaining;
		remaining = barline_writestring(fp, y_ptr->y_array[j].string,
								   remaining);
	    }
	    putc('\n', fp);
	}
    }
}

/*
 * Functions relating to reading/writing marks from viminfo
 */

நிலையான xfmark_T *vi_namedfm = வெற்று;
நிலையான xfmark_T *vi_jumplist = வெற்று;
நிலையான முழு vi_jumplist_len = 0;

    நிலையான வெறுமை
write_one_mark(FILE *fp_out, முழு c, pos_T *pos)
{
    எனில் (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (முழு)pos->col);
}

    நிலையான வெறுமை
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    முழு		i;
    pos_T	pos;

    home_replace(வெற்று, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_செருகு);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    க்கு (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	எனில் (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    க்கு (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    நிலையான முழு
skip_for_viminfo(buf_T *buf)
{
    திரும்பு bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    நிலையான வெறுமை
write_viminfo_marks(FILE *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    முழு		is_mark_set;
    முழு		i;
    win_T	*win;
    tabpage_T	*tp;

    // Set b_last_cursor for the all buffers that have a window.
    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# History of marks within files (newest to oldest):\n"), fp_out);
    FOR_ALL_BUFFERS(buf)
    {
	// Only write something if buffer has been loaded and at least one
	// mark is set.
	எனில் (buf->b_marks_read)
	{
	    எனில் (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    இல்லையெனில்
	    {
		is_mark_set = FALSE;
		க்கு (i = 0; i < NMARKS; i++)
		    எனில் (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			நிறுத்து;
		    }
	    }
	    எனில் (is_mark_set && buf->b_ffname != வெற்று
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		எனில் (buflist == வெற்று)
		    write_buffer_marks(buf, fp_out);
		இல்லையெனில் எனில் (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    நிலையான வெறுமை
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    முழு		c1,
    முழு		c2)
{
    char_u	*name;

    எனில் (fm->fmark.mark.lnum == 0)	// not set
	திரும்பு;

    எனில் (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    இல்லையெனில்
	name = fm->fname;		// use name from .viminfo
    எனில் (name != வெற்று && *name != NUL)
    {
	fprintf(fp, "%c%c  %ld  %ld  ", c1, c2, (long)fm->fmark.mark.lnum,
						    (long)fm->fmark.mark.col);
	viminfo_writestring(fp, name);

	// Barline: |{bartype},{name},{lnum},{col},{timestamp},{filename}
	// size up to filename: 8 + 3 * 20
	fprintf(fp, "|%d,%d,%ld,%ld,%ld,", BARTYPE_MARK, c2,
		(long)fm->fmark.mark.lnum, (long)fm->fmark.mark.col,
		(long)fm->time_set);
	barline_writestring(fp, name, LSIZE - 70);
	putc('\n', fp);
    }

    எனில் (fm->fmark.fnum != 0)
	vim_free(name);
}

    நிலையான வெறுமை
write_viminfo_filemarks(FILE *fp)
{
    முழு		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    முழு		vi_idx;
    முழு		idx;

    எனில் (get_viminfo_parameter('f') == 0)
	திரும்பு;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    க்கு (i = 0; i < NMARKS; i++)
    {
	எனில் (vi_namedfm != வெற்று
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	இல்லையெனில்
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    எனில் (curbuf->b_ffname != வெற்று && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	க்கு (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    எனில் (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == வெற்று
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != வெற்று
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		நிறுத்து;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	க்கு ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = வெற்று;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    க்கு (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != வெற்று ? &vi_namedfm[vi_idx] : வெற்று;

	எனில் (vi_fm != வெற்று
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	இல்லையெனில்
	{
	    fm = &namedfm_p[idx++];
	    எனில் (vi_fm != வெற்று
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != வெற்று
			  && fm->fname != வெற்று
			  && STRCMP(vi_fm->fname, fm->fname) == 0)))
		++vi_idx;  // skip duplicate
	}
	write_one_filemark(fp, fm, '\'', i - NMARKS + '0');
    }

    // Write the jumplist with -'
    fputs(_("\n# Jumplist (newest first):\n"), fp);
    setpcmark();	// add current cursor position
    cleanup_jumplist(curwin, FALSE);
    vi_idx = 0;
    idx = curwin->w_jumplistlen - 1;
    க்கு (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : வெற்று;
	vi_fm = (vi_jumplist != வெற்று && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : வெற்று;
	எனில் (fm == வெற்று && vi_fm == வெற்று)
	    நிறுத்து;
	எனில் (fm == வெற்று || (vi_fm != வெற்று && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	இல்லையெனில்
	    --idx;
	எனில் (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != வெற்று
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    முழு
buf_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    எனில் (buf1->b_last_used == buf2->b_last_used)
	திரும்பு 0;
    திரும்பு buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    நிலையான வெறுமை
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_out,
    garray_T	*buflist,
    முழு		eof,
    முழு		flags)
{
    char_u	*வரி = virp->vir_வரி;
    buf_T	*buf;
    முழு		இலக்கம்_marked_files;
    முழு		load_marks;
    முழு		copy_marks_out;
    char_u	*str;
    முழு		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = வெற்று;
#endif
    முழு		மொத்தம் = 0;
    முழு		buflist_used = 0;
    buf_T	*buflist_buf = வெற்று;

    எனில் ((name_buf = alloc(LSIZE)) == வெற்று)
	திரும்பு;
    *name_buf = NUL;

    எனில் (fp_out != வெற்று && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						அளவு(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    எனில் (fp_out == வெற்று && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	எனில் (list != வெற்று)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    இலக்கம்_marked_files = get_viminfo_parameter('\'');
    போது (!eof && (மொத்தம் < இலக்கம்_marked_files || fp_out == வெற்று))
    {
	எனில் (வரி[0] != '>')
	{
	    எனில் (வரி[0] != '\n' && வரி[0] != '\r' && வரி[0] != '#')
	    {
		எனில் (viminfo_error("E576: ", _(e_nonr_missing_gt), வரி))
		    நிறுத்து;	// too many errors, return now
	    }
	    eof = vim_fgets(வரி, LSIZE, virp->vir_fd);
	    தொடர்;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(வரி + 1);
	str = viminfo_readstring(virp, (முழு)(str - virp->vir_வரி), FALSE);
	எனில் (str == வெற்று)
	    தொடர்;
	p = str + STRLEN(str);
	போது (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	எனில் (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	எனில் (list != வெற்று)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	எனில் (fp_out == வெற்று)
	{
	    எனில் ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != வெற்று)
	    {
		எனில் (*name_buf == NUL)	    // only need to do this once
		    home_replace(வெற்று, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		எனில் (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	இல்லையெனில் // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		எனில் (buf->b_ffname != வெற்று)
		{
		    home_replace(வெற்று, buf->b_ffname, name_buf, LSIZE, TRUE);
		    எனில் (fnamecmp(str, name_buf) == 0)
			நிறுத்து;
		}

	    // Copy marks if the buffer has not been loaded.
	    எனில் (buf == வெற்று || !buf->b_marks_read)
	    {
		முழு	did_read_வரி = FALSE;

		எனில் (buflist_buf != வெற்று)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    எனில் (!viminfo_readline(virp) && வரி[0] == TAB)
		    {
			did_read_வரி = TRUE;
			எனில் (வரி[1] == '*')
			{
			    long	ltime;

			    sscanf((எழுத்து *)வரி + 2, "%ld ", &ltime);
			    போது ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				எனில் (++மொத்தம் >= இலக்கம்_marked_files)
				    நிறுத்து;
				எனில் (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = வெற்று;
				    நிறுத்து;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			இல்லையெனில்
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    போது (மொத்தம் < இலக்கம்_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++மொத்தம்;
			    }
			    buflist_buf = வெற்று;
			}

			எனில் (மொத்தம் >= இலக்கம்_marked_files)
			{
			    vim_free(str);
			    நிறுத்து;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		எனில் (did_read_வரி)
		    fputs((எழுத்து *)வரி, fp_out);

		மொத்தம்++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	போது (!(eof = viminfo_readline(virp)) && வரி[0] == TAB)
	{
	    எனில் (load_marks)
	    {
		எனில் (வரி[1] != NUL)
		{
		    unsigned u;

		    sscanf((எழுத்து *)வரி + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    தேர்வு (வரி[1])
		    {
			நிலை '"': curbuf->b_last_cursor = pos; நிறுத்து;
			நிலை '^': curbuf->b_last_செருகு = pos; நிறுத்து;
			நிலை '.': curbuf->b_last_change = pos; நிறுத்து;
			நிலை '+':
				  // changelist positions are stored oldest
				  // first
				  எனில் (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    அளவு(pos_T) * (JUMPLISTSIZE - 1));
				  இல்லையெனில்
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  நிறுத்து;

				  // Using the line number for the last-used
				  // timestamp.
			நிலை '*': curbuf->b_last_used = pos.lnum; நிறுத்து;

			இயல்பு:  எனில் ((i = வரி[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    இல்லையெனில் எனில் (copy_marks_out)
		fputs((எழுத்து *)வரி, fp_out);
	}

	எனில் (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		எனில் (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    எனில் (flags & VIF_ONLY_CURBUF)
		நிறுத்து;
	}
    }

    எனில் (fp_out != வெற்று)
	// Write any remaining entries from buflist.
	போது (மொத்தம் < இலக்கம்_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++மொத்தம்;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    வெறுமை
check_marks_read(வெறுமை)
{
    எனில் (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != வெற்று)
	read_viminfo(வெற்று, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    நிலையான முழு
read_viminfo_filemark(vir_T *virp, முழு force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    முழு		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_வரி + 1;
    எனில் (*str <= 127
	    && ((*virp->vir_வரி == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_வரி == '-' && *str == '\'')))
    {
	எனில் (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    எனில் (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = வெற்று;
	    இல்லையெனில்
	    {
		க்கு (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = வெற்று;
	    }
	}
	இல்லையெனில் எனில் (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	இல்லையெனில்
	    fm = &namedfm_p[*str - 'A'];
	எனில் (fm != வெற்று && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (முழு)(str - virp->vir_வரி),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    திரும்பு vim_fgets(virp->vir_வரி, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    நிலையான வெறுமை
prepare_viminfo_marks(வெறுமை)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    நிலையான வெறுமை
finish_viminfo_marks(வெறுமை)
{
    முழு		i;

    எனில் (vi_namedfm != வெற்று)
    {
	க்கு (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    எனில் (vi_jumplist != வெற்று)
    {
	க்கு (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    நிலையான வெறுமை
handle_viminfo_mark(garray_T *மதிப்புகள், முழு force)
{
    bval_T	*vp = (bval_T *)மதிப்புகள்->ga_data;
    முழு		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = வெற்று;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    எனில் (மதிப்புகள்->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	திரும்பு;

    name = vp[0].bv_nr;
    எனில் (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	திரும்பு;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    எனில் (lnum <= 0 || col < 0)
	திரும்பு;
    timestamp = (time_t)vp[3].bv_nr;

    எனில் (name == '\'')
    {
	எனில் (vi_jumplist != வெற்று)
	{
	    எனில் (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	இல்லையெனில்
	{
	    முழு idx;
	    முழு i;

	    // If we have a timestamp insert it in the right place.
	    எனில் (timestamp != 0)
	    {
		க்கு (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    எனில் (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			நிறுத்து;
		    }
		// idx cannot be zero now
		எனில் (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    இல்லையெனில் எனில் (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    இல்லையெனில்
		idx = -1;

	    எனில் (idx >= 0)
	    {
		எனில் (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    க்கு (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		இல்லையெனில்
		{
		    // Move newer entries forward.
		    க்கு (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = வெற்று;
		fm->time_set = 0;
	    }
	}
    }
    இல்லையெனில்
    {
	முழு		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	எனில் (VIM_ISDIGIT(name))
	{
	    எனில் (vi_namedfm != வெற்று)
		idx = name - '0' + NMARKS;
	    இல்லையெனில்
	    {
		முழு i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		க்கு (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    எனில் (namedfm_p[idx].time_set < timestamp)
			நிறுத்து;
		எனில் (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    திரும்பு;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		க்கு ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = வெற்று;
	    }
	}
	இல்லையெனில்
	    idx = name - 'A';
	எனில் (vi_namedfm != வெற்று)
	    fm = &vi_namedfm[idx];
	இல்லையெனில்
	    fm = &namedfm_p[idx];
    }

    எனில் (fm != வெற்று)
    {
	எனில் (vi_namedfm != வெற்று || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    எனில் (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = வெற்று;
	    }
	    இல்லையெனில்
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    நிலையான முழு
read_viminfo_barline(vir_T *virp, முழு got_encoding, முழு force, முழு writing)
{
    char_u	*p = virp->vir_வரி + 1;
    முழு		bartype;
    garray_T	மதிப்புகள்;
    bval_T	*vp;
    முழு		i;
    முழு		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    எனில் (*p == '<')
    {
	// Continuation line of an unrecognized item.
	எனில் (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_வரி);
    }
    இல்லையெனில்
    {
	ga_init2(&மதிப்புகள், அளவு(bval_T), 20);
	bartype = getdigits(&p);
	தேர்வு (bartype)
	{
	    நிலை BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		எனில் (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &மதிப்புகள்);
		    vp = (bval_T *)மதிப்புகள்.ga_data;
		    எனில் (மதிப்புகள்.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		நிறுத்து;

	    நிலை BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &மதிப்புகள்);
		handle_viminfo_history(&மதிப்புகள், writing);
		நிறுத்து;

	    நிலை BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &மதிப்புகள்);
		handle_viminfo_register(&மதிப்புகள், force);
		நிறுத்து;

	    நிலை BARTYPE_MARK:
		read_next = barline_parse(virp, p, &மதிப்புகள்);
		handle_viminfo_mark(&மதிப்புகள், force);
		நிறுத்து;

	    இயல்பு:
		// copy unrecognized line (for future use)
		எனில் (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_வரி);
	}
	க்கு (i = 0; i < மதிப்புகள்.ga_len; ++i)
	{
	    vp = (bval_T *)மதிப்புகள்.ga_data + i;
	    எனில் (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&மதிப்புகள்);
    }

    எனில் (read_next)
	திரும்பு viminfo_readline(virp);
    திரும்பு FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    நிலையான முழு
read_viminfo_up_to_marks(
    vir_T	*virp,
    முழு		forceit,
    முழு		writing)
{
    முழு		eof;
    buf_T	*buf;
    முழு		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    போது (!eof && virp->vir_வரி[0] != '>')
    {
	தேர்வு (virp->vir_வரி[0])
	{
		// Characters reserved for future expansion, ignored now
	    நிலை '+': // "+40 /path/dir file", for running vim without args
	    நிலை '^': // to be defined
	    நிலை '<': // long line - ignored
		// A comment or empty line.
	    நிலை NUL:
	    நிலை '\r':
	    நிலை '\n':
	    நிலை '#':
		eof = viminfo_readline(virp);
		நிறுத்து;
	    நிலை '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		நிறுத்து;
	    நிலை '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		நிறுத்து;
	    நிலை '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#இல்லையெனில்
		eof = viminfo_readline(virp);
#endif
		நிறுத்து;
	    நிலை '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		நிறுத்து;
	    நிலை '"':
		// When registers are in bar lines skip the old style register
		// lines.
		எனில் (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		இல்லையெனில்
		    செய்
		    {
			eof = viminfo_readline(virp);
		    } போது (!eof && (virp->vir_வரி[0] == TAB
						|| virp->vir_வரி[0] == '<'));
		நிறுத்து;
	    நிலை '/':	    // Search string
	    நிலை '&':	    // Substitute search string
	    நிலை '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_தேடல்_pattern(virp, forceit, writing);
		நிறுத்து;
	    நிலை '$':
		eof = read_viminfo_sub_string(virp, forceit);
		நிறுத்து;
	    நிலை ':':
	    நிலை '?':
	    நிலை '=':
	    நிலை '@':
		// When history is in bar lines skip the old style history
		// lines.
		எனில் (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		இல்லையெனில்
		    eof = viminfo_readline(virp);
		நிறுத்து;
	    நிலை '-':
	    நிலை '\'':
		// When file marks are in bar lines skip the old style lines.
		எனில் (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		இல்லையெனில்
		    eof = viminfo_readline(virp);
		நிறுத்து;
	    இயல்பு:
		எனில் (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_வரி))
		    eof = TRUE;
		இல்லையெனில்
		    eof = viminfo_readline(virp);
		நிறுத்து;
	}
    }

    // Finish reading history items.
    எனில் (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    திரும்பு eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    நிலையான வெறுமை
do_viminfo(FILE *fp_in, FILE *fp_out, முழு flags)
{
    முழு		eof = FALSE;
    vir_T	vir;
    முழு		merge = FALSE;
    முழு		do_copy_marks = FALSE;
    garray_T	buflist;

    எனில் ((vir.vir_வரி = alloc(LSIZE)) == வெற்று)
	திரும்பு;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, அளவு(char_u *), 100);
    vir.vir_version = -1;

    எனில் (fp_in != வெற்று)
    {
	எனில் (flags & VIF_WANT_INFO)
	{
	    எனில் (fp_out != வெற்று)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != வெற்று);
	    merge = TRUE;
	}
	இல்லையெனில் எனில் (flags != 0)
	    // Skip info, find start of marks
	    போது (!(eof = viminfo_readline(&vir))
		    && vir.vir_வரி[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    எனில் (fp_out != வெற்று)
    {
	// Write the info:
	fprintf(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_out);
	fprintf(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_தேடல்_pattern(fp_out);
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

	எனில் (do_copy_marks)
	    ga_init2(&buflist, அளவு(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : வெற்று);
    }

    எனில் (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	எனில் (fp_out != வெற்று)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_வரி);
    எனில் (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, வெற்று, வெற்று);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    முழு
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    முழு		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    எனில் (no_viminfo())
	திரும்பு FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    எனில் (fname == வெற்று)
	திரும்பு FAIL;
    fp = mch_fopen((எழுத்து *)fname, READBIN);

    எனில் (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == வெற்று ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    எனில் (fp == வெற்று)
	திரும்பு FAIL;
    எனில் (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	திரும்பு FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, வெற்று, flags);

    fclose(fp);
    திரும்பு OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    வெறுமை
write_viminfo(char_u *file, முழு forceit)
{
    char_u	*fname;
    FILE	*fp_in = வெற்று;	// input viminfo file, if any
    FILE	*fp_out = வெற்று;	// output viminfo file
    char_u	*tempname = வெற்று;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#எனில் defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    முழு		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    முழு		hidden = FALSE;
#endif

    எனில் (no_viminfo())
	திரும்பு;

    fname = viminfo_filename(file);	// may set to default if NULL
    எனில் (fname == வெற்று)
	திரும்பு;

    fp_in = mch_fopen((எழுத்து *)fname, READBIN);
    எனில் (fp_in == வெற்று)
    {
	முழு fd;

	// if it does exist, but we can't read it, don't try writing
	எனில் (mch_stat((எழுத்து *)fname, &st_new) == 0)
	    செல் end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((எழுத்து *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	எனில் (fd < 0)
	    செல் end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    இல்லையெனில்
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	எனில் (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != வேர்_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    முழு	tt = செய்தி_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    செய்தி_didany = tt;
	    fclose(fp_in);
	    செல் end;
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
	க்கு (;;)
	{
	    முழு		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#இல்லையெனில்
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#இல்லையெனில்
				    (char_u *)".tmp",
#endif
				    FALSE);
	    எனில் (tempname == வெற்று)		// out of memory
		நிறுத்து;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    எனில் (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    க்கு (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		எனில் (mch_stat((எழுத்து *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    எனில் (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			நிறுத்து;
		    }
#endif
		}
		இல்லையெனில்
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((எழுத்து *)tempname, WRITEBIN);
		    (வெறுமை)umask(umask_save);
#இல்லையெனில்
		    முழு	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((எழுத்து *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(முழு)((st_old.st_mode & 0777) | 0600));
		    (வெறுமை)umask(umask_save);
# இல்லையெனில்
		    fd = mch_open((எழுத்து *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    எனில் (fd < 0)
		    {
			fp_out = வெற்று;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			எனில் (errno != EEXIST)
			    நிறுத்து;
# endif
		    }
		    இல்லையெனில்
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    எனில் (fp_out != வெற்று)
			நிறுத்து;
		}

		// Assume file exists, try again with another name.
		எனில் (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    நிறுத்து;
		}
		*wp = next_char;
		--next_char;
	    }

	    எனில் (tempname != வெற்று)
		நிறுத்து;
	    // continue if shortname was set
	}

#எனில் defined(UNIX) && defined(HAVE_FCHOWN)
	எனில் (tempname != வெற்று && fp_out != வெற்று)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    எனில் (mch_stat((எழுத்து *)tempname, &tmp_st) >= 0)
	    {
		எனில் (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		எனில் (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (வெறுமை)mch_setperm(tempname, 0600);
	    }
	    இல்லையெனில்
		// can't stat the file, set conservative permissions
		(வெறுமை)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    எனில் (fp_out == வெற்று)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == வெற்று || tempname == வெற்று) ? fname : tempname);
	எனில் (fp_in != வெற்று)
	    fclose(fp_in);
	செல் end;
    }

    எனில் (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    எனில் (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    எனில் (fp_in != வெற்று)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	எனில் (viminfo_errcnt == 0)
	{
	    எனில் (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    இல்லையெனில் எனில் (hidden)
		mch_hide(fname);
#endif
	}
	எனில் (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    வெறுமை
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    எனில் (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    எனில் (eap->cmdidx == CMD_rviminfo)
    {
	எனில் (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    இல்லையெனில்
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
