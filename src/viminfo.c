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

#شامل "vim.h"
#شامل "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
تعریفنوع ساختار
{
    char_u	*vir_سطر;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    صحیح		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

تعریفنوع شمارش {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

تعریفنوع ساختار {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    صحیح		bv_len;		// length of bv_string
    صحیح		bv_allocated;	// bv_string was allocated
} bval_T;

#اگر defined(FEAT_VIMINFO)

ایستا صحیح  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    ایستا char_u *
find_viminfo_parameter(صحیح type)
{
    char_u  *p;

    برای (p = p_viminfo; *p; ++p)
    {
	اگر (*p == type)
	    بازگشت p + 1;
	اگر (*p == 'n')		    // 'n' is always the last one
	    بشکن;
	p = vim_strchr(p, ',');	    // skip until next ','
	اگر (p == پوچ)		    // hit the end without finding parameter
	    بشکن;
    }
    بازگشت پوچ;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    صحیح
get_viminfo_parameter(صحیح type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    اگر (p != پوچ && VIM_ISDIGIT(*p))
	بازگشت atoi((نویسه *)p);
    بازگشت -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    ایستا char_u *
viminfo_filename(char_u *file)
{
    اگر (file == پوچ || *file == NUL)
    {
	size_t  len;

	اگر (*p_viminfofile != NUL)
	    file = p_viminfofile;
	وگرنه اگر ((file = find_viminfo_parameter('n')) == پوچ || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    اگر (mch_getenv((char_u *)"SYS$LOGIN") == پوچ)
# وگرنه
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    اگر (STRCMP(vim_getenv((char_u *)"HOME", پوچ), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == پوچ)
#  وگرنه
	    اگر (mch_getenv((char_u *)"HOME") == پوچ)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		اگر (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		وگرنه
		    file = (char_u *)VIMINFO_FILE;
	    }
	    وگرنه
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	بازگشت vim_strnsave(file, len);
    }

    بازگشت vim_strsave(file);
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
    ایستا تهی
viminfo_writestring(FILE *fd, char_u *p)
{
    صحیح		c;
    char_u	*s;
    صحیح		len = 0;

    برای (s = p; *s != NUL; ++s)
    {
	اگر (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    اگر (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    وقتی ((c = *p++) != NUL)
    {
	اگر (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    اگر (c == '\n')
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
    ایستا صحیح
barline_writestring(FILE *fd, char_u *s, صحیح remaining_start)
{
    char_u *p;
    صحیح	    remaining = remaining_start;
    صحیح	    len = 2;

    // Count the number of characters produced, including quotes.
    برای (p = s; *p != NUL; ++p)
    {
	اگر (*p == NL)
	    len += 2;
	وگرنه اگر (*p == '"' || *p == '\\')
	    len += 2;
	وگرنه
	    ++len;
    }
    اگر (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    برای (p = s; *p != NUL; ++p)
    {
	اگر (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	وگرنه اگر (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	وگرنه
	    putc(*p, fd);
	--remaining;

	اگر (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    بازگشت remaining - 2;
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
    ایستا char_u *
viminfo_readstring(
    vir_T	*virp,
    صحیح		off,		    // offset for virp->vir_line
    صحیح		convert)	    // convert the string
{
    char_u	*retval = پوچ;
    char_u	*s, *d;
    long	len;

    اگر (virp->vir_سطر[off] == Ctrl_V && vim_isdigit(virp->vir_سطر[off + 1]))
    {
	len = atol((نویسه *)virp->vir_سطر + off + 1);
	اگر (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	اگر (retval == پوچ)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (تهی)vim_fgets(virp->vir_سطر, 10, virp->vir_fd);
	    بازگشت پوچ;
	}
	(تهی)vim_fgets(retval, (صحیح)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    وگرنه
    {
	retval = vim_strsave(virp->vir_سطر + off);
	اگر (retval == پوچ)
	    بازگشت پوچ;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    وقتی (*s != NUL && *s != '\n')
    {
	اگر (s[0] == Ctrl_V && s[1] != NUL)
	{
	    اگر (s[1] == 'n')
		*d++ = '\n';
	    وگرنه
		*d++ = Ctrl_V;
	    s += 2;
	}
	وگرنه
	    *d++ = *s++;
    }
    *d = NUL;

    اگر (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, پوچ);
	اگر (d != پوچ)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    بازگشت retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    ایستا صحیح
viminfo_readline(vir_T *virp)
{
    بازگشت vim_fgets(virp->vir_سطر, LSIZE, virp->vir_fd);
}

    ایستا صحیح
read_viminfo_bufferlist(
    vir_T	*virp,
    صحیح		writing)
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
    اگر (xline != پوچ && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != پوچ)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	اگر (tab != پوچ)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((نویسه *)tab);
	    tab = vim_strrchr(xline, '\t');
	    اگر (tab != پوچ)
	    {
		*tab++ = '\0';
		lnum = atol((نویسه *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	اگر (buf != پوچ)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    بازگشت viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    ایستا صحیح
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    صحیح	    part_len;
    صحیح	    retval = FALSE;

    name = home_replace_save(پوچ, name);
    اگر (name == پوچ)
	بازگشت FALSE;
    برای (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, اندازه(part), ", ");
	اگر (part[0] == 'r')
	{
	    اگر (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		بشکن;
	    }
	}
    }
    vim_free(name);
    بازگشت retval;
}

    ایستا تهی
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*سطر;
    صحیح		max_buffers;

    اگر (find_viminfo_parameter('%') == پوچ)
	بازگشت;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define سطر_BUF_LEN (MAXPATHL + 40)
    سطر = alloc(سطر_BUF_LEN);
    اگر (سطر == پوچ)
	بازگشت;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	اگر (buf->b_fname == پوچ
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    ادامه;

	اگر (max_buffers-- == 0)
	    بشکن;
	putc('%', fp);
	linelen = home_replace(پوچ, buf->b_ffname, سطر, MAXPATHL, TRUE);
	vim_snprintf((نویسه *)سطر + linelen, سطر_BUF_LEN - linelen, "\t%ld\t%d",
			(long)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, سطر);
    }
    vim_free(سطر);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
ایستا histentry_T *viminfo_history[HIST_شمار] =
					       {پوچ, پوچ, پوچ, پوچ, پوچ};
ایستا صحیح	viminfo_hisidx[HIST_شمار] = {0, 0, 0, 0, 0};
ایستا صحیح	viminfo_hislen[HIST_شمار] = {0, 0, 0, 0, 0};
ایستا صحیح	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    ایستا صحیح
hist_type2char(
    صحیح	    type,
    صحیح	    use_question)	    // use '?' instead of '/'
{
    اگر (type == HIST_CMD)
	بازگشت ':';
    اگر (type == HIST_جستجو)
    {
	اگر (use_question)
	    بازگشت '?';
	وگرنه
	    بازگشت '/';
    }
    اگر (type == HIST_عبارت)
	بازگشت '=';
    بازگشت '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    ایستا تهی
prepare_viminfo_history(صحیح asklen, صحیح writing)
{
    صحیح	    i;
    صحیح	    رقم;
    صحیح	    type;
    صحیح	    len;
    صحیح	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    اگر (asklen > hislen)
	asklen = hislen;

    برای (type = 0; type < HIST_شمار; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	برای (i = 0, رقم = 0; i < hislen; i++)
	    اگر (histentry[i].hisstr == پوچ || histentry[i].viminfo)
		رقم++;
	len = asklen;
	اگر (رقم > len)
	    len = رقم;
	اگر (len <= 0)
	    viminfo_history[type] = پوچ;
	وگرنه
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	اگر (viminfo_history[type] == پوچ)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    ایستا صحیح
read_viminfo_history(vir_T *virp, صحیح writing)
{
    صحیح		type;
    long_u	len;
    char_u	*val = پوچ;
    char_u	*p;

    type = hist_char2type(virp->vir_سطر[0]);
    اگر (viminfo_hisidx[type] >= viminfo_hislen[type])
	برو done;

    val = viminfo_readstring(virp, 1, TRUE);
    اگر (val == پوچ || *val == NUL)
	برو done;

    صحیح sep = (*val == ' ' ? NUL : *val);

    اگر (in_history(type, val + (type == HIST_جستجو), viminfo_add_at_front,
								sep, writing))
	برو done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    اگر (type == HIST_جستجو)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	اگر (p == پوچ)
	    برو done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    وگرنه
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	اگر (p == پوچ)
	    برو done;

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
    بازگشت viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    ایستا تهی
handle_viminfo_history(
	garray_T    *مقادیر,
	صحیح	    writing)
{
    صحیح		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)مقادیر->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    اگر (مقادیر->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	بازگشت;

    type = vp[0].bv_nr;
    اگر (type >= HIST_شمار)
	بازگشت;

    اگر (viminfo_hisidx[type] >= viminfo_hislen[type])
	بازگشت;

    val = vp[3].bv_string;
    اگر (val == پوچ || *val == NUL)
	بازگشت;

    صحیح sep = type == HIST_جستجو && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    صحیح idx;
    صحیح overwrite = FALSE;

    اگر (in_history(type, val, viminfo_add_at_front, sep, writing))
	بازگشت;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    برای (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	اگر (STRCMP(val, p) == 0
		&& (type != HIST_جستجو || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    بشکن;
	}
    }

    اگر (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    وگرنه
	len = 0; // for picky compilers
    اگر (p != پوچ)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	اگر (!overwrite)
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
    ایستا تهی
concat_history(صحیح type)
{
    صحیح		idx;
    صحیح		i;
    صحیح		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    صحیح		*hisidx = get_hisidx(type);
    صحیح		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    اگر (idx >= hislen)
	idx -= hislen;
    وگرنه اگر (idx < 0)
	idx = hislen - 1;
    اگر (viminfo_add_at_front)
	*hisidx = idx;
    وگرنه
    {
	اگر (*hisidx == -1)
	    *hisidx = hislen - 1;
	انجامبده
	{
	    اگر (histentry[idx].hisstr != پوچ || histentry[idx].viminfo)
		بشکن;
	    اگر (++idx == hislen)
		idx = 0;
	} وقتی (idx != *hisidx);
	اگر (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    برای (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	اگر (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    برای (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    ایستا صحیح
sort_hist(ثابت تهی *s1, ثابت تهی *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    اگر (p1->time_set < p2->time_set) بازگشت -1;
    اگر (p1->time_set > p2->time_set) بازگشت 1;
    بازگشت 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    ایستا تهی
merge_history(صحیح type)
{
    صحیح		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    صحیح		i;
    صحیح		len;
    صحیح		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    صحیح		*hisidx = get_hisidx(type);
    صحیح		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    اگر (tot_hist == پوچ || new_hist == پوچ)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	بازگشت;
    }
    برای (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    برای (i = 0; i < hislen; i++)
	اگر (histentry[i].hisstr != پوچ)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((تهی *)tot_hist, (size_t)len, اندازه(histentry_T *), sort_hist);

    // Keep the newest ones.
    برای (i = 0; i < hislen; i++)
    {
	اگر (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = پوچ;
	    tot_hist[i]->hisstrlen = 0;
	    اگر (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	وگرنه
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    برای (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    برای (i = 0; i < hislen; i++)
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
    ایستا تهی
finish_viminfo_history(vir_T *virp)
{
    صحیح	type;
    صحیح merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    برای (type = 0; type < HIST_شمار; ++type)
    {
	اگر (get_histentry(type) == پوچ)
	    ادامه;

	اگر (merge)
	    merge_history(type);
	وگرنه
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
    ایستا تهی
write_viminfo_history(FILE *fp, صحیح merge)
{
    صحیح	    i;
    صحیح	    type;
    صحیح	    رقم_saved;
    صحیح     round;
    صحیح	    hislen;

    init_history();
    hislen = get_hislen();
    اگر (hislen == 0)
	بازگشت;
    برای (type = 0; type < HIST_شمار; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	صحیح	    *hisidx = get_hisidx(type);

	رقم_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	اگر (رقم_saved == 0)
	    ادامه;
	اگر (رقم_saved < 0)  // Use default
	    رقم_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_جستجو ? _("Search String") :
			    type == HIST_عبارت ? _("Expression") :
			    type == HIST_درون_داد ? _("Input Line") :
					_("Debug Line"));
	اگر (رقم_saved > hislen)
	    رقم_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	برای (round = 1; round <= 2; ++round)
	{
	    اگر (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    وگرنه اگر (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    وگرنه
		// empty list
		i = -1;
	    اگر (i >= 0)
		وقتی (رقم_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    صحیح	    c = NUL;

		    اگر (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    وگرنه
		    {
			اگر (viminfo_history[type] == پوچ)
			{
			    p = پوچ;
			    plen = 0;
			    timestamp = 0;
			}
			وگرنه
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    اگر (p != پوچ && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--رقم_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			اگر (type == HIST_جستجو)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    نویسه    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    اگر (c == NUL)
				cbuf[0] = NUL;
			    وگرنه
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    اگر (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			اگر (--i < 0)
			    i = hislen - 1;
			اگر (i == *hisidx)
			    بشکن;
		    }
		    وگرنه
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	برای (i = 0; i < viminfo_hisidx[type]; ++i)
	    اگر (viminfo_history[type] != پوچ)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    ایستا تهی
write_viminfo_barlines(vir_T *virp, FILE *fp_out)
{
    صحیح		i;
    garray_T	*gap = &virp->vir_barlines;
    صحیح		seen_useful = FALSE;
    نویسه	*سطر;

    اگر (gap->ga_len <= 0)
	بازگشت;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    برای (i = 0; i < gap->ga_len; ++i)
    {
	سطر = ((نویسه **)(gap->ga_data))[i];
	اگر (seen_useful || سطر[1] != '<')
	{
	    fputs(سطر, fp_out);
	    seen_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    ایستا صحیح
barline_parse(vir_T *virp, char_u *text, garray_T *مقادیر)
{
    char_u  *p = text;
    char_u  *nextp = پوچ;
    char_u  *buf = پوچ;
    bval_T  *مقدار;
    صحیح	    i;
    صحیح	    allocated = FALSE;
    صحیح	    eof;
    char_u  *sconv;
    صحیح	    converted;

    وقتی (*p == ',')
    {
	++p;
	اگر (ga_grow(مقادیر, 1) == FAIL)
	    بشکن;
	مقدار = (bval_T *)(مقادیر->ga_data) + مقادیر->ga_len;

	اگر (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    اگر (!allocated)
	    {
		برای (i = 0; i < مقادیر->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(مقادیر->ga_data) + i;

		    اگر (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    اگر (vim_isdigit(p[1]))
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
		اگر (buf == پوچ)
		    بازگشت TRUE;
		p = buf;
		برای (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    اگر (eof || virp->vir_سطر[0] != '|'
						  || virp->vir_سطر[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			بازگشت eof || virp->vir_سطر[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_سطر);
		    وقتی (n > 2 && (virp->vir_سطر[n - 1] == NL
					     || virp->vir_سطر[n - 1] == CAR))
			--n;
		    n -= 2;
		    اگر (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_سطر + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_سطر + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    وگرنه
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		اگر (eof || virp->vir_سطر[0] != '|'
					      || virp->vir_سطر[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    بازگشت eof || virp->vir_سطر[0] == '|';
		p = virp->vir_سطر + 2;
	    }
	}

	اگر (SAFE_isdigit(*p))
	{
	    مقدار->bv_type = BVAL_NR;
	    مقدار->bv_nr = getdigits(&p);
	    ++مقادیر->ga_len;
	}
	وگرنه اگر (*p == '"')
	{
	    صحیح	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    وقتی (*p != '"')
	    {
		اگر (*p == NL || *p == NUL)
		    بازگشت TRUE;  // syntax error, drop the value
		اگر (*p == '\\')
		{
		    ++p;
		    اگر (*p == 'n')
			s[len++] = '\n';
		    وگرنه
			s[len++] = *p;
		    ++p;
		}
		وگرنه
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    مقدار->bv_tofree = پوچ;
	    اگر (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, پوچ);
		اگر (sconv != پوچ)
		{
		    اگر (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			مقدار->bv_tofree = buf;
		    s = sconv;
		    len = (صحیح)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    اگر (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    مقدار->bv_string = s;
	    مقدار->bv_type = BVAL_STRING;
	    مقدار->bv_len = len;
	    مقدار->bv_allocated = allocated || converted;
	    ++مقادیر->ga_len;
	    اگر (nextp != پوچ)
	    {
		// values following a long string
		p = nextp;
		nextp = پوچ;
	    }
	}
	وگرنه اگر (*p == ',')
	{
	    مقدار->bv_type = BVAL_EMPTY;
	    ++مقادیر->ga_len;
	}
	وگرنه
	    بشکن;
    }
    بازگشت TRUE;
}

    ایستا تهی
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    ایستا صحیح
no_viminfo(تهی)
{
    // "vim -i NONE" does not read or write a viminfo file
    بازگشت STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    ایستا صحیح
viminfo_error(نویسه *errnum, نویسه *message, char_u *سطر)
{
    vim_snprintf((نویسه *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, سطر, IOSIZE - STRLEN(IObuff) - 1);
    اگر (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((نویسه *)IObuff);
    اگر (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    ایستا صحیح
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    صحیح		i;

    اگر (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_سطر, '=');
	اگر (p != پوچ)
	{
	    // remove trailing newline
	    ++p;
	    برای (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    بازگشت viminfo_readline(virp);
}

#اگر defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    ایستا صحیح
read_viminfo_varlist(vir_T *virp, صحیح writing)
{
    char_u	*tab;
    صحیح		type = VAR_عدد;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    اگر (!writing && (find_viminfo_parameter('!') != پوچ))
    {
	tab = vim_strchr(virp->vir_سطر + 1, '\t');
	اگر (tab != پوچ)
	{
	    *tab++ = '\0';	// isolate the variable name
	    انتخاب (*tab)
	    {
		حالت 'S': type = VAR_STRING; بشکن;
		حالت 'F': type = VAR_FLOAT; بشکن;
		حالت 'D': type = VAR_DICT; بشکن;
		حالت 'L': type = VAR_LIST; بشکن;
		حالت 'B': type = VAR_BLOB; بشکن;
		حالت 'X': type = VAR_SPECIAL; بشکن;
		حالت 'T': type = VAR_TUPLE; بشکن;
	    }

	    tab = vim_strchr(tab, '\t');
	    اگر (tab != پوچ)
	    {
		tv.v_type = type;
		اگر (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (صحیح)(tab - virp->vir_سطر + 1), TRUE);
		وگرنه اگر (type == VAR_FLOAT)
		    (تهی)string2float(tab + 1, &tv.vval.v_float, FALSE);
		وگرنه
		{
		    tv.vval.v_عدد = atol((نویسه *)tab + 1);
		    اگر (type == VAR_SPECIAL && (tv.vval.v_عدد == VVAL_FALSE
					     || tv.vval.v_عدد == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		اگر (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_عبارت(tv.vval.v_string, پوچ);

		    اگر (etv == پوچ)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    وگرنه
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		وگرنه اگر (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    اگر (blob == پوچ)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    وگرنه
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_entry);
		set_var(virp->vir_سطر + 1, &tv, FALSE);
		restore_funccal();

		اگر (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		وگرنه اگر (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    بازگشت viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    ایستا تهی
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    صحیح		todo;
    نویسه	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    اگر (find_viminfo_parameter('!') == پوچ)
	بازگشت;

    fputs(_("\n# global variables:\n"), fp);

    todo = (صحیح)gvht->ht_used;
    FOR_ALL_HASHTAB_اقلام(gvht, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    اگر (var_flavour(this_var->di_کلید) == VAR_FLAVOUR_VIMINFO)
	    {
		انتخاب (this_var->di_tv.v_type)
		{
		    حالت VAR_STRING:  s = "STR"; بشکن;
		    حالت VAR_عدد:  s = "NUM"; بشکن;
		    حالت VAR_FLOAT:   s = "FLO"; بشکن;
		    حالت VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      صحیح	copyID = get_copyID();

			      s = "DIC";
			      اگر (di != پوچ && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, پوچ, پوچ)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ادامه;
			      بشکن;
			  }
		    حالت VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      صحیح	copyID = get_copyID();

			      s = "LIS";
			      اگر (l != پوچ && !set_ref_in_list_اقلام(
						       l, copyID, پوچ, پوچ)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ادامه;
			      بشکن;
			  }
		    حالت VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      صحیح	copyID = get_copyID();

			      s = "TUP";
			      اگر (tuple != پوچ && !set_ref_in_tuple_اقلام(
					       tuple, copyID, پوچ, پوچ)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ادامه;
			      بشکن;
			  }
		    حالت VAR_BLOB:    s = "BLO"; بشکن;
		    حالت VAR_BOOL:    s = "XPL"; بشکن;  // backwards compat.
		    حالت VAR_SPECIAL: s = "XPL"; بشکن;

		    حالت VAR_UNKNOWN:
		    حالت VAR_ANY:
		    حالت VAR_VOID:
		    حالت VAR_FUNC:
		    حالت VAR_PARTIAL:
		    حالت VAR_JOB:
		    حالت VAR_CHANNEL:
		    حالت VAR_INSTR:
		    حالت VAR_CLASS:
		    حالت VAR_OBJECT:
		    حالت VAR_TYPEALIAS:
				      ادامه;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_کلید, s);
		اگر (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((نویسه *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_عدد);
		    p = numbuf;
		    tofree = پوچ;
		}
		وگرنه
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		اگر (p != پوچ)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    ایستا صحیح
read_viminfo_sub_string(vir_T *virp, صحیح force)
{
    اگر (force || get_old_sub() == پوچ)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    بازگشت viminfo_readline(virp);
}

    ایستا تهی
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    اگر (get_viminfo_parameter('/') == 0 || old_sub == پوچ)
	بازگشت;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    ایستا صحیح
read_viminfo_جستجو_pattern(vir_T *virp, صحیح force, صحیح writing)
{
    char_u	*lp;
    صحیح		idx = -1;
    صحیح		magic = FALSE;
    صحیح		no_scs = FALSE;
    صحیح		off_سطر = FALSE;
    صحیح		off_end = 0;
    long	off = 0;
    صحیح		setlast = FALSE;
#ifdef FEAT_جستجو_EXTRA
    ایستا صحیح	hlsearch_on = FALSE;
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
    lp = virp->vir_سطر;
    اگر (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	اگر (lp[1] == 'M')		// magic on
	    magic = TRUE;
	اگر (lp[2] == 's')
	    no_scs = TRUE;
	اگر (lp[3] == 'L')
	    off_سطر = TRUE;
	اگر (lp[4] == 'E')
	    off_end = جستجو_END;
	lp += 5;
	off = getdigits(&lp);
    }
    اگر (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    اگر (lp[0] == '/')
	idx = RE_جستجو;
    وگرنه اگر (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_جستجو_EXTRA
    وگرنه اگر (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    وگرنه اگر (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    اگر (idx >= 0)
    {
	spat = get_spat(idx);
	اگر (force || spat->pat == پوچ)
	{
	    val = viminfo_readstring(virp, (صحیح)(lp - virp->vir_سطر + 1),
									TRUE);
	    اگر (val != پوچ)
	    {
		اگر (!writing)
		{
		    set_last_جستجو_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.سطر = off_سطر;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_جستجو_EXTRA
		    اگر (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    بازگشت viminfo_readline(virp);
}

    ایستا تهی
wvsp_one(
    FILE	*fp,	// file to write to
    صحیح		idx,	// spats[] index
    نویسه	*s,	// search pat
    صحیح		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    اگر (spat->pat == پوچ)
	بازگشت;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.سطر ? 'L' : 'l',	// line offset
	    spat->off.end  ? 'E' : 'e',	// offset from end
	    spat->off.off,			// offset
	    get_spat_last_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    ایستا تهی
write_viminfo_جستجو_pattern(FILE *fp)
{
    اگر (get_viminfo_parameter('/') == 0)
	بازگشت;

#ifdef FEAT_جستجو_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != پوچ) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_جستجو, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

ایستا yankreg_T *y_read_regs = پوچ;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    ایستا تهی
prepare_viminfo_registers(تهی)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, رقم_REGISTERS);
}

    ایستا تهی
finish_viminfo_registers(تهی)
{
    صحیح		i;
    صحیح		j;

    اگر (y_read_regs == پوچ)
	بازگشت;

    برای (i = 0; i < رقم_REGISTERS; ++i)
	اگر (y_read_regs[i].y_array != پوچ)
	{
	    برای (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    ایستا صحیح
read_viminfo_register(vir_T *virp, صحیح force)
{
    صحیح		eof;
    صحیح		do_it = TRUE;
    صحیح		size;
    صحیح		limit;
    صحیح		i;
    صحیح		set_prev = FALSE;
    char_u	*str;
    string_T	*array = پوچ;
    صحیح		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_سطر + 1;

    // If the line starts with "" this is the y_previous register.
    اگر (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    اگر (!ASCII_ISALNUM(*str) && *str != '-')
    {
	اگر (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_سطر))
	    بازگشت TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    اگر (!force && y_current_p->y_array != پوچ)
	do_it = FALSE;

    اگر (*str == '@')
    {
	// "x@: register x used for @@
	اگر (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    اگر (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	اگر (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	اگر (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	وگرنه اگر (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	وگرنه
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    وقتی (!(eof = viminfo_readline(virp))
		    && (virp->vir_سطر[0] == TAB || virp->vir_سطر[0] == '<'))
    {
	اگر (do_it)
	{
	    اگر (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * اندازه(string_T));

		اگر (new_array == پوچ)
		{
		    do_it = FALSE;
		    بشکن;
		}
		برای (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    اگر (str != پوچ)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    وگرنه
		// error, don't store the result
		do_it = FALSE;
	}
    }

    اگر (do_it)
    {
	// free y_array[]
	برای (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	اگر (size == 0)
	{
	    y_current_p->y_array = پوچ;
	}
	وگرنه
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    برای (i = 0; i < size; i++)
	    {
		اگر (y_current_p->y_array == پوچ)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		وگرنه
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    وگرنه
    {
	// Free array[] if it was filled.
	برای (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    بازگشت eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    ایستا تهی
handle_viminfo_register(garray_T *مقادیر, صحیح force)
{
    bval_T	*vp = (bval_T *)مقادیر->ga_data;
    صحیح		flags;
    صحیح		name;
    صحیح		type;
    صحیح		linecount;
    صحیح		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    صحیح		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    اگر (مقادیر->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	بازگشت;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    اگر (name < 0 || name >= رقم_REGISTERS)
	بازگشت;
    type = vp[2].bv_nr;
    اگر (type != MCHAR && type != MLINE && type != MBLOCK)
	بازگشت;
    linecount = vp[3].bv_nr;
    اگر (مقادیر->ga_len < 6 + linecount)
	بازگشت;
    width = vp[4].bv_nr;
    اگر (width < 0)
	بازگشت;

    اگر (y_read_regs != پوچ)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    وگرنه
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    اگر (y_ptr->y_array != پوچ && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	بازگشت;

    اگر (y_ptr->y_array != پوچ)
	برای (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    اگر (y_read_regs == پوچ)
    {
	اگر (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	اگر ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    اگر (linecount == 0)
    {
	y_ptr->y_array = پوچ;
	بازگشت;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    اگر (y_ptr->y_array == پوچ)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	بازگشت;
    }
    برای (i = 0; i < linecount; i++)
    {
	اگر (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = پوچ;
	}
	وگرنه اگر (vp[i + 6].bv_type != BVAL_STRING)
	{
	    آزاد(y_ptr->y_array);
	    y_ptr->y_array = پوچ;
	}
	وگرنه
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    ایستا تهی
write_viminfo_registers(FILE *fp)
{
    صحیح		i, j;
    char_u	*type;
    char_u	c;
    صحیح		رقم_سطرها;
    صحیح		max_رقم_سطرها;
    صحیح		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_رقم_سطرها = get_viminfo_parameter('<');
    اگر (max_رقم_سطرها < 0)
	max_رقم_سطرها = get_viminfo_parameter('"');
    اگر (max_رقم_سطرها == 0)
	بازگشت;
    max_kbyte = get_viminfo_parameter('s');
    اگر (max_kbyte == 0)
	بازگشت;

    برای (i = 0; i < رقم_REGISTERS; i++)
    {
#اگر defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	اگر (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    ادامه;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	اگر (i == TILDE_REGISTER)
	    ادامه;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	اگر (y_read_regs != پوچ
		&& y_read_regs[i].y_array != پوچ
		&& (y_regs_p[i].y_array == پوچ ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	وگرنه اگر (y_regs_p[i].y_array == پوچ)
	    ادامه;
	وگرنه
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	رقم_سطرها = y_ptr->y_size;
	اگر (رقم_سطرها == 0
		|| (رقم_سطرها == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    ادامه;

	اگر (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    برای (j = 0; j < رقم_سطرها; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    اگر (len > (long)max_kbyte * 1024L)
		ادامه;
	}

	انتخاب (y_ptr->y_type)
	{
	    حالت MLINE:
		type = (char_u *)"LINE";
		بشکن;
	    حالت MCHAR:
		type = (char_u *)"CHAR";
		بشکن;
	    حالت MBLOCK:
		type = (char_u *)"BLOCK";
		بشکن;
	    پیشفرض:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		بشکن;
	}
	اگر (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	اگر (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (صحیح)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	اگر (max_رقم_سطرها > 0 && رقم_سطرها > max_رقم_سطرها)
	    رقم_سطرها = max_رقم_سطرها;
	برای (j = 0; j < رقم_سطرها; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    صحیح	    flags = 0;
	    صحیح	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    اگر (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    اگر (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, رقم_سطرها, (صحیح)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    برای (j = 0; j < رقم_سطرها; j++)
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

ایستا xfmark_T *vi_namedfm = پوچ;
ایستا xfmark_T *vi_jumplist = پوچ;
ایستا صحیح vi_jumplist_len = 0;

    ایستا تهی
write_one_mark(FILE *fp_out, صحیح c, pos_T *pos)
{
    اگر (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (صحیح)pos->col);
}

    ایستا تهی
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    صحیح		i;
    pos_T	pos;

    home_replace(پوچ, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_درج);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    برای (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	اگر (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    برای (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    ایستا صحیح
skip_for_viminfo(buf_T *buf)
{
    بازگشت bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    ایستا تهی
write_viminfo_marks(FILE *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    صحیح		is_mark_set;
    صحیح		i;
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
	اگر (buf->b_marks_read)
	{
	    اگر (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    وگرنه
	    {
		is_mark_set = FALSE;
		برای (i = 0; i < NMARKS; i++)
		    اگر (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			بشکن;
		    }
	    }
	    اگر (is_mark_set && buf->b_ffname != پوچ
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		اگر (buflist == پوچ)
		    write_buffer_marks(buf, fp_out);
		وگرنه اگر (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    ایستا تهی
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    صحیح		c1,
    صحیح		c2)
{
    char_u	*name;

    اگر (fm->fmark.mark.lnum == 0)	// not set
	بازگشت;

    اگر (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    وگرنه
	name = fm->fname;		// use name from .viminfo
    اگر (name != پوچ && *name != NUL)
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

    اگر (fm->fmark.fnum != 0)
	vim_free(name);
}

    ایستا تهی
write_viminfo_filemarks(FILE *fp)
{
    صحیح		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    صحیح		vi_idx;
    صحیح		idx;

    اگر (get_viminfo_parameter('f') == 0)
	بازگشت;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    برای (i = 0; i < NMARKS; i++)
    {
	اگر (vi_namedfm != پوچ
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	وگرنه
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    اگر (curbuf->b_ffname != پوچ && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	برای (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    اگر (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == پوچ
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != پوچ
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		بشکن;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	برای ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = پوچ;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    برای (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != پوچ ? &vi_namedfm[vi_idx] : پوچ;

	اگر (vi_fm != پوچ
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	وگرنه
	{
	    fm = &namedfm_p[idx++];
	    اگر (vi_fm != پوچ
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != پوچ
			  && fm->fname != پوچ
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
    برای (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : پوچ;
	vi_fm = (vi_jumplist != پوچ && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : پوچ;
	اگر (fm == پوچ && vi_fm == پوچ)
	    بشکن;
	اگر (fm == پوچ || (vi_fm != پوچ && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	وگرنه
	    --idx;
	اگر (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != پوچ
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    صحیح
buf_compare(ثابت تهی *s1, ثابت تهی *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    اگر (buf1->b_last_used == buf2->b_last_used)
	بازگشت 0;
    بازگشت buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    ایستا تهی
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_out,
    garray_T	*buflist,
    صحیح		eof,
    صحیح		flags)
{
    char_u	*سطر = virp->vir_سطر;
    buf_T	*buf;
    صحیح		رقم_marked_files;
    صحیح		load_marks;
    صحیح		copy_marks_out;
    char_u	*str;
    صحیح		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = پوچ;
#endif
    صحیح		شمار = 0;
    صحیح		buflist_used = 0;
    buf_T	*buflist_buf = پوچ;

    اگر ((name_buf = alloc(LSIZE)) == پوچ)
	بازگشت;
    *name_buf = NUL;

    اگر (fp_out != پوچ && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						اندازه(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    اگر (fp_out == پوچ && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	اگر (list != پوچ)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    رقم_marked_files = get_viminfo_parameter('\'');
    وقتی (!eof && (شمار < رقم_marked_files || fp_out == پوچ))
    {
	اگر (سطر[0] != '>')
	{
	    اگر (سطر[0] != '\n' && سطر[0] != '\r' && سطر[0] != '#')
	    {
		اگر (viminfo_error("E576: ", _(e_nonr_missing_gt), سطر))
		    بشکن;	// too many errors, return now
	    }
	    eof = vim_fgets(سطر, LSIZE, virp->vir_fd);
	    ادامه;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(سطر + 1);
	str = viminfo_readstring(virp, (صحیح)(str - virp->vir_سطر), FALSE);
	اگر (str == پوچ)
	    ادامه;
	p = str + STRLEN(str);
	وقتی (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	اگر (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	اگر (list != پوچ)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	اگر (fp_out == پوچ)
	{
	    اگر ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != پوچ)
	    {
		اگر (*name_buf == NUL)	    // only need to do this once
		    home_replace(پوچ, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		اگر (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	وگرنه // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		اگر (buf->b_ffname != پوچ)
		{
		    home_replace(پوچ, buf->b_ffname, name_buf, LSIZE, TRUE);
		    اگر (fnamecmp(str, name_buf) == 0)
			بشکن;
		}

	    // Copy marks if the buffer has not been loaded.
	    اگر (buf == پوچ || !buf->b_marks_read)
	    {
		صحیح	did_read_سطر = FALSE;

		اگر (buflist_buf != پوچ)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    اگر (!viminfo_readline(virp) && سطر[0] == TAB)
		    {
			did_read_سطر = TRUE;
			اگر (سطر[1] == '*')
			{
			    long	ltime;

			    sscanf((نویسه *)سطر + 2, "%ld ", &ltime);
			    وقتی ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				اگر (++شمار >= رقم_marked_files)
				    بشکن;
				اگر (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = پوچ;
				    بشکن;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			وگرنه
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    وقتی (شمار < رقم_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++شمار;
			    }
			    buflist_buf = پوچ;
			}

			اگر (شمار >= رقم_marked_files)
			{
			    vim_free(str);
			    بشکن;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		اگر (did_read_سطر)
		    fputs((نویسه *)سطر, fp_out);

		شمار++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	وقتی (!(eof = viminfo_readline(virp)) && سطر[0] == TAB)
	{
	    اگر (load_marks)
	    {
		اگر (سطر[1] != NUL)
		{
		    unsigned u;

		    sscanf((نویسه *)سطر + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    انتخاب (سطر[1])
		    {
			حالت '"': curbuf->b_last_cursor = pos; بشکن;
			حالت '^': curbuf->b_last_درج = pos; بشکن;
			حالت '.': curbuf->b_last_change = pos; بشکن;
			حالت '+':
				  // changelist positions are stored oldest
				  // first
				  اگر (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    اندازه(pos_T) * (JUMPLISTSIZE - 1));
				  وگرنه
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  بشکن;

				  // Using the line number for the last-used
				  // timestamp.
			حالت '*': curbuf->b_last_used = pos.lnum; بشکن;

			پیشفرض:  اگر ((i = سطر[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    وگرنه اگر (copy_marks_out)
		fputs((نویسه *)سطر, fp_out);
	}

	اگر (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		اگر (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    اگر (flags & VIF_ONLY_CURBUF)
		بشکن;
	}
    }

    اگر (fp_out != پوچ)
	// Write any remaining entries from buflist.
	وقتی (شمار < رقم_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++شمار;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    تهی
check_marks_read(تهی)
{
    اگر (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != پوچ)
	read_viminfo(پوچ, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    ایستا صحیح
read_viminfo_filemark(vir_T *virp, صحیح force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    صحیح		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_سطر + 1;
    اگر (*str <= 127
	    && ((*virp->vir_سطر == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_سطر == '-' && *str == '\'')))
    {
	اگر (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    اگر (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = پوچ;
	    وگرنه
	    {
		برای (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = پوچ;
	    }
	}
	وگرنه اگر (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	وگرنه
	    fm = &namedfm_p[*str - 'A'];
	اگر (fm != پوچ && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (صحیح)(str - virp->vir_سطر),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    بازگشت vim_fgets(virp->vir_سطر, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    ایستا تهی
prepare_viminfo_marks(تهی)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    ایستا تهی
finish_viminfo_marks(تهی)
{
    صحیح		i;

    اگر (vi_namedfm != پوچ)
    {
	برای (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    اگر (vi_jumplist != پوچ)
    {
	برای (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    ایستا تهی
handle_viminfo_mark(garray_T *مقادیر, صحیح force)
{
    bval_T	*vp = (bval_T *)مقادیر->ga_data;
    صحیح		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = پوچ;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    اگر (مقادیر->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	بازگشت;

    name = vp[0].bv_nr;
    اگر (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	بازگشت;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    اگر (lnum <= 0 || col < 0)
	بازگشت;
    timestamp = (time_t)vp[3].bv_nr;

    اگر (name == '\'')
    {
	اگر (vi_jumplist != پوچ)
	{
	    اگر (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	وگرنه
	{
	    صحیح idx;
	    صحیح i;

	    // If we have a timestamp insert it in the right place.
	    اگر (timestamp != 0)
	    {
		برای (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    اگر (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			بشکن;
		    }
		// idx cannot be zero now
		اگر (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    وگرنه اگر (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    وگرنه
		idx = -1;

	    اگر (idx >= 0)
	    {
		اگر (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    برای (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		وگرنه
		{
		    // Move newer entries forward.
		    برای (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = پوچ;
		fm->time_set = 0;
	    }
	}
    }
    وگرنه
    {
	صحیح		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	اگر (VIM_ISDIGIT(name))
	{
	    اگر (vi_namedfm != پوچ)
		idx = name - '0' + NMARKS;
	    وگرنه
	    {
		صحیح i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		برای (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    اگر (namedfm_p[idx].time_set < timestamp)
			بشکن;
		اگر (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    بازگشت;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		برای ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = پوچ;
	    }
	}
	وگرنه
	    idx = name - 'A';
	اگر (vi_namedfm != پوچ)
	    fm = &vi_namedfm[idx];
	وگرنه
	    fm = &namedfm_p[idx];
    }

    اگر (fm != پوچ)
    {
	اگر (vi_namedfm != پوچ || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    اگر (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = پوچ;
	    }
	    وگرنه
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    ایستا صحیح
read_viminfo_barline(vir_T *virp, صحیح got_encoding, صحیح force, صحیح writing)
{
    char_u	*p = virp->vir_سطر + 1;
    صحیح		bartype;
    garray_T	مقادیر;
    bval_T	*vp;
    صحیح		i;
    صحیح		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    اگر (*p == '<')
    {
	// Continuation line of an unrecognized item.
	اگر (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_سطر);
    }
    وگرنه
    {
	ga_init2(&مقادیر, اندازه(bval_T), 20);
	bartype = getdigits(&p);
	انتخاب (bartype)
	{
	    حالت BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		اگر (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &مقادیر);
		    vp = (bval_T *)مقادیر.ga_data;
		    اگر (مقادیر.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		بشکن;

	    حالت BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &مقادیر);
		handle_viminfo_history(&مقادیر, writing);
		بشکن;

	    حالت BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &مقادیر);
		handle_viminfo_register(&مقادیر, force);
		بشکن;

	    حالت BARTYPE_MARK:
		read_next = barline_parse(virp, p, &مقادیر);
		handle_viminfo_mark(&مقادیر, force);
		بشکن;

	    پیشفرض:
		// copy unrecognized line (for future use)
		اگر (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_سطر);
	}
	برای (i = 0; i < مقادیر.ga_len; ++i)
	{
	    vp = (bval_T *)مقادیر.ga_data + i;
	    اگر (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&مقادیر);
    }

    اگر (read_next)
	بازگشت viminfo_readline(virp);
    بازگشت FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    ایستا صحیح
read_viminfo_up_to_marks(
    vir_T	*virp,
    صحیح		forceit,
    صحیح		writing)
{
    صحیح		eof;
    buf_T	*buf;
    صحیح		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    وقتی (!eof && virp->vir_سطر[0] != '>')
    {
	انتخاب (virp->vir_سطر[0])
	{
		// Characters reserved for future expansion, ignored now
	    حالت '+': // "+40 /path/dir file", for running vim without args
	    حالت '^': // to be defined
	    حالت '<': // long line - ignored
		// A comment or empty line.
	    حالت NUL:
	    حالت '\r':
	    حالت '\n':
	    حالت '#':
		eof = viminfo_readline(virp);
		بشکن;
	    حالت '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		بشکن;
	    حالت '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		بشکن;
	    حالت '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#وگرنه
		eof = viminfo_readline(virp);
#endif
		بشکن;
	    حالت '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		بشکن;
	    حالت '"':
		// When registers are in bar lines skip the old style register
		// lines.
		اگر (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		وگرنه
		    انجامبده
		    {
			eof = viminfo_readline(virp);
		    } وقتی (!eof && (virp->vir_سطر[0] == TAB
						|| virp->vir_سطر[0] == '<'));
		بشکن;
	    حالت '/':	    // Search string
	    حالت '&':	    // Substitute search string
	    حالت '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_جستجو_pattern(virp, forceit, writing);
		بشکن;
	    حالت '$':
		eof = read_viminfo_sub_string(virp, forceit);
		بشکن;
	    حالت ':':
	    حالت '?':
	    حالت '=':
	    حالت '@':
		// When history is in bar lines skip the old style history
		// lines.
		اگر (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		وگرنه
		    eof = viminfo_readline(virp);
		بشکن;
	    حالت '-':
	    حالت '\'':
		// When file marks are in bar lines skip the old style lines.
		اگر (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		وگرنه
		    eof = viminfo_readline(virp);
		بشکن;
	    پیشفرض:
		اگر (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_سطر))
		    eof = TRUE;
		وگرنه
		    eof = viminfo_readline(virp);
		بشکن;
	}
    }

    // Finish reading history items.
    اگر (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    بازگشت eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    ایستا تهی
do_viminfo(FILE *fp_in, FILE *fp_out, صحیح flags)
{
    صحیح		eof = FALSE;
    vir_T	vir;
    صحیح		merge = FALSE;
    صحیح		do_copy_marks = FALSE;
    garray_T	buflist;

    اگر ((vir.vir_سطر = alloc(LSIZE)) == پوچ)
	بازگشت;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, اندازه(char_u *), 100);
    vir.vir_version = -1;

    اگر (fp_in != پوچ)
    {
	اگر (flags & VIF_WANT_INFO)
	{
	    اگر (fp_out != پوچ)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != پوچ);
	    merge = TRUE;
	}
	وگرنه اگر (flags != 0)
	    // Skip info, find start of marks
	    وقتی (!(eof = viminfo_readline(&vir))
		    && vir.vir_سطر[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    اگر (fp_out != پوچ)
    {
	// Write the info:
	fprintf(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_out);
	fprintf(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_جستجو_pattern(fp_out);
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

	اگر (do_copy_marks)
	    ga_init2(&buflist, اندازه(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : پوچ);
    }

    اگر (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	اگر (fp_out != پوچ)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_سطر);
    اگر (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, پوچ, پوچ);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    صحیح
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    صحیح		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    اگر (no_viminfo())
	بازگشت FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    اگر (fname == پوچ)
	بازگشت FAIL;
    fp = mch_fopen((نویسه *)fname, READBIN);

    اگر (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == پوچ ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    اگر (fp == پوچ)
	بازگشت FAIL;
    اگر (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	بازگشت FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, پوچ, flags);

    fclose(fp);
    بازگشت OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    تهی
write_viminfo(char_u *file, صحیح forceit)
{
    char_u	*fname;
    FILE	*fp_in = پوچ;	// input viminfo file, if any
    FILE	*fp_out = پوچ;	// output viminfo file
    char_u	*tempname = پوچ;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#اگر defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    صحیح		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    صحیح		hidden = FALSE;
#endif

    اگر (no_viminfo())
	بازگشت;

    fname = viminfo_filename(file);	// may set to default if NULL
    اگر (fname == پوچ)
	بازگشت;

    fp_in = mch_fopen((نویسه *)fname, READBIN);
    اگر (fp_in == پوچ)
    {
	صحیح fd;

	// if it does exist, but we can't read it, don't try writing
	اگر (mch_stat((نویسه *)fname, &st_new) == 0)
	    برو end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((نویسه *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	اگر (fd < 0)
	    برو end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    وگرنه
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	اگر (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != ریشه_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    صحیح	tt = پیام_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    پیام_didany = tt;
	    fclose(fp_in);
	    برو end;
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
	برای (;;)
	{
	    صحیح		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#وگرنه
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#وگرنه
				    (char_u *)".tmp",
#endif
				    FALSE);
	    اگر (tempname == پوچ)		// out of memory
		بشکن;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    اگر (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    برای (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		اگر (mch_stat((نویسه *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    اگر (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			بشکن;
		    }
#endif
		}
		وگرنه
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((نویسه *)tempname, WRITEBIN);
		    (تهی)umask(umask_save);
#وگرنه
		    صحیح	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((نویسه *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(صحیح)((st_old.st_mode & 0777) | 0600));
		    (تهی)umask(umask_save);
# وگرنه
		    fd = mch_open((نویسه *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    اگر (fd < 0)
		    {
			fp_out = پوچ;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			اگر (errno != EEXIST)
			    بشکن;
# endif
		    }
		    وگرنه
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    اگر (fp_out != پوچ)
			بشکن;
		}

		// Assume file exists, try again with another name.
		اگر (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    بشکن;
		}
		*wp = next_char;
		--next_char;
	    }

	    اگر (tempname != پوچ)
		بشکن;
	    // continue if shortname was set
	}

#اگر defined(UNIX) && defined(HAVE_FCHOWN)
	اگر (tempname != پوچ && fp_out != پوچ)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    اگر (mch_stat((نویسه *)tempname, &tmp_st) >= 0)
	    {
		اگر (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		اگر (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (تهی)mch_setperm(tempname, 0600);
	    }
	    وگرنه
		// can't stat the file, set conservative permissions
		(تهی)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    اگر (fp_out == پوچ)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == پوچ || tempname == پوچ) ? fname : tempname);
	اگر (fp_in != پوچ)
	    fclose(fp_in);
	برو end;
    }

    اگر (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    اگر (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    اگر (fp_in != پوچ)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	اگر (viminfo_errcnt == 0)
	{
	    اگر (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    وگرنه اگر (hidden)
		mch_hide(fname);
#endif
	}
	اگر (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    تهی
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    اگر (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    اگر (eap->cmdidx == CMD_rviminfo)
    {
	اگر (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    وگرنه
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
