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

#ضمن "vim.h"
#ضمن "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
تعريفنوع بنية
{
    char_u	*vir_سطر;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    صحيح		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

تعريفنوع تعداد {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

تعريفنوع بنية {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    صحيح		bv_len;		// length of bv_string
    صحيح		bv_allocated;	// bv_string was allocated
} bval_T;

#إذا defined(FEAT_VIMINFO)

ساكن صحيح  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    ساكن char_u *
find_viminfo_parameter(صحيح type)
{
    char_u  *p;

    لكل (p = p_viminfo; *p; ++p)
    {
	إذا (*p == type)
	    أرجع p + 1;
	إذا (*p == 'n')		    // 'n' is always the last one
	    اكسر;
	p = vim_strchr(p, ',');	    // skip until next ','
	إذا (p == عدم)		    // hit the end without finding parameter
	    اكسر;
    }
    أرجع عدم;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    صحيح
get_viminfo_parameter(صحيح type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    إذا (p != عدم && VIM_ISDIGIT(*p))
	أرجع atoi((حرف *)p);
    أرجع -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    ساكن char_u *
viminfo_filename(char_u *file)
{
    إذا (file == عدم || *file == NUL)
    {
	size_t  len;

	إذا (*p_viminfofile != NUL)
	    file = p_viminfofile;
	وإلا إذا ((file = find_viminfo_parameter('n')) == عدم || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    إذا (mch_getenv((char_u *)"SYS$LOGIN") == عدم)
# وإلا
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    إذا (STRCMP(vim_getenv((char_u *)"HOME", عدم), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == عدم)
#  وإلا
	    إذا (mch_getenv((char_u *)"HOME") == عدم)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		إذا (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		وإلا
		    file = (char_u *)VIMINFO_FILE;
	    }
	    وإلا
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	أرجع vim_strnsave(file, len);
    }

    أرجع vim_strsave(file);
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
    ساكن فراغ
viminfo_writestring(FILE *fd, char_u *p)
{
    صحيح		c;
    char_u	*s;
    صحيح		len = 0;

    لكل (s = p; *s != NUL; ++s)
    {
	إذا (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    إذا (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    طالما ((c = *p++) != NUL)
    {
	إذا (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    إذا (c == '\n')
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
    ساكن صحيح
barline_writestring(FILE *fd, char_u *s, صحيح remaining_start)
{
    char_u *p;
    صحيح	    remaining = remaining_start;
    صحيح	    len = 2;

    // Count the number of characters produced, including quotes.
    لكل (p = s; *p != NUL; ++p)
    {
	إذا (*p == NL)
	    len += 2;
	وإلا إذا (*p == '"' || *p == '\\')
	    len += 2;
	وإلا
	    ++len;
    }
    إذا (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    لكل (p = s; *p != NUL; ++p)
    {
	إذا (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	وإلا إذا (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	وإلا
	    putc(*p, fd);
	--remaining;

	إذا (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    أرجع remaining - 2;
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
    ساكن char_u *
viminfo_readstring(
    vir_T	*virp,
    صحيح		off,		    // offset for virp->vir_line
    صحيح		convert)	    // convert the string
{
    char_u	*retval = عدم;
    char_u	*s, *d;
    long	len;

    إذا (virp->vir_سطر[off] == Ctrl_V && vim_isdigit(virp->vir_سطر[off + 1]))
    {
	len = atol((حرف *)virp->vir_سطر + off + 1);
	إذا (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	إذا (retval == عدم)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (فراغ)vim_fgets(virp->vir_سطر, 10, virp->vir_fd);
	    أرجع عدم;
	}
	(فراغ)vim_fgets(retval, (صحيح)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    وإلا
    {
	retval = vim_strsave(virp->vir_سطر + off);
	إذا (retval == عدم)
	    أرجع عدم;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    طالما (*s != NUL && *s != '\n')
    {
	إذا (s[0] == Ctrl_V && s[1] != NUL)
	{
	    إذا (s[1] == 'n')
		*d++ = '\n';
	    وإلا
		*d++ = Ctrl_V;
	    s += 2;
	}
	وإلا
	    *d++ = *s++;
    }
    *d = NUL;

    إذا (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, عدم);
	إذا (d != عدم)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    أرجع retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    ساكن صحيح
viminfo_readline(vir_T *virp)
{
    أرجع vim_fgets(virp->vir_سطر, LSIZE, virp->vir_fd);
}

    ساكن صحيح
read_viminfo_bufferlist(
    vir_T	*virp,
    صحيح		writing)
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
    إذا (xline != عدم && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != عدم)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	إذا (tab != عدم)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((حرف *)tab);
	    tab = vim_strrchr(xline, '\t');
	    إذا (tab != عدم)
	    {
		*tab++ = '\0';
		lnum = atol((حرف *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	إذا (buf != عدم)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    أرجع viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    ساكن صحيح
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    صحيح	    part_len;
    صحيح	    retval = FALSE;

    name = home_replace_save(عدم, name);
    إذا (name == عدم)
	أرجع FALSE;
    لكل (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, حجم(part), ", ");
	إذا (part[0] == 'r')
	{
	    إذا (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		اكسر;
	    }
	}
    }
    vim_free(name);
    أرجع retval;
}

    ساكن فراغ
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*سطر;
    صحيح		max_buffers;

    إذا (find_viminfo_parameter('%') == عدم)
	أرجع;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define سطر_BUF_LEN (MAXPATHL + 40)
    سطر = alloc(سطر_BUF_LEN);
    إذا (سطر == عدم)
	أرجع;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	إذا (buf->b_fname == عدم
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    استمر;

	إذا (max_buffers-- == 0)
	    اكسر;
	putc('%', fp);
	linelen = home_replace(عدم, buf->b_ffname, سطر, MAXPATHL, TRUE);
	vim_snprintf((حرف *)سطر + linelen, سطر_BUF_LEN - linelen, "\t%ld\t%d",
			(long)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, سطر);
    }
    vim_free(سطر);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
ساكن histentry_T *viminfo_history[HIST_گنتی] =
					       {عدم, عدم, عدم, عدم, عدم};
ساكن صحيح	viminfo_hisidx[HIST_گنتی] = {0, 0, 0, 0, 0};
ساكن صحيح	viminfo_hislen[HIST_گنتی] = {0, 0, 0, 0, 0};
ساكن صحيح	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    ساكن صحيح
hist_type2char(
    صحيح	    type,
    صحيح	    use_question)	    // use '?' instead of '/'
{
    إذا (type == HIST_CMD)
	أرجع ':';
    إذا (type == HIST_تلاش)
    {
	إذا (use_question)
	    أرجع '?';
	وإلا
	    أرجع '/';
    }
    إذا (type == HIST_اظہاریہ)
	أرجع '=';
    أرجع '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    ساكن فراغ
prepare_viminfo_history(صحيح asklen, صحيح writing)
{
    صحيح	    i;
    صحيح	    عدد;
    صحيح	    type;
    صحيح	    len;
    صحيح	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    إذا (asklen > hislen)
	asklen = hislen;

    لكل (type = 0; type < HIST_گنتی; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	لكل (i = 0, عدد = 0; i < hislen; i++)
	    إذا (histentry[i].hisstr == عدم || histentry[i].viminfo)
		عدد++;
	len = asklen;
	إذا (عدد > len)
	    len = عدد;
	إذا (len <= 0)
	    viminfo_history[type] = عدم;
	وإلا
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	إذا (viminfo_history[type] == عدم)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    ساكن صحيح
read_viminfo_history(vir_T *virp, صحيح writing)
{
    صحيح		type;
    long_u	len;
    char_u	*val = عدم;
    char_u	*p;

    type = hist_char2type(virp->vir_سطر[0]);
    إذا (viminfo_hisidx[type] >= viminfo_hislen[type])
	اذهب done;

    val = viminfo_readstring(virp, 1, TRUE);
    إذا (val == عدم || *val == NUL)
	اذهب done;

    صحيح sep = (*val == ' ' ? NUL : *val);

    إذا (in_history(type, val + (type == HIST_تلاش), viminfo_add_at_front,
								sep, writing))
	اذهب done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    إذا (type == HIST_تلاش)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	إذا (p == عدم)
	    اذهب done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    وإلا
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	إذا (p == عدم)
	    اذهب done;

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
    أرجع viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    ساكن فراغ
handle_viminfo_history(
	garray_T    *اقدار,
	صحيح	    writing)
{
    صحيح		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)اقدار->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    إذا (اقدار->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	أرجع;

    type = vp[0].bv_nr;
    إذا (type >= HIST_گنتی)
	أرجع;

    إذا (viminfo_hisidx[type] >= viminfo_hislen[type])
	أرجع;

    val = vp[3].bv_string;
    إذا (val == عدم || *val == NUL)
	أرجع;

    صحيح sep = type == HIST_تلاش && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    صحيح idx;
    صحيح overwrite = FALSE;

    إذا (in_history(type, val, viminfo_add_at_front, sep, writing))
	أرجع;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    لكل (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	إذا (STRCMP(val, p) == 0
		&& (type != HIST_تلاش || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    اكسر;
	}
    }

    إذا (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    وإلا
	len = 0; // for picky compilers
    إذا (p != عدم)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	إذا (!overwrite)
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
    ساكن فراغ
concat_history(صحيح type)
{
    صحيح		idx;
    صحيح		i;
    صحيح		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    صحيح		*hisidx = get_hisidx(type);
    صحيح		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    إذا (idx >= hislen)
	idx -= hislen;
    وإلا إذا (idx < 0)
	idx = hislen - 1;
    إذا (viminfo_add_at_front)
	*hisidx = idx;
    وإلا
    {
	إذا (*hisidx == -1)
	    *hisidx = hislen - 1;
	افعل
	{
	    إذا (histentry[idx].hisstr != عدم || histentry[idx].viminfo)
		اكسر;
	    إذا (++idx == hislen)
		idx = 0;
	} طالما (idx != *hisidx);
	إذا (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    لكل (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	إذا (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    لكل (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    ساكن صحيح
sort_hist(ثابت فراغ *s1, ثابت فراغ *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    إذا (p1->time_set < p2->time_set) أرجع -1;
    إذا (p1->time_set > p2->time_set) أرجع 1;
    أرجع 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    ساكن فراغ
merge_history(صحيح type)
{
    صحيح		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    صحيح		i;
    صحيح		len;
    صحيح		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    صحيح		*hisidx = get_hisidx(type);
    صحيح		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    إذا (tot_hist == عدم || new_hist == عدم)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	أرجع;
    }
    لكل (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    لكل (i = 0; i < hislen; i++)
	إذا (histentry[i].hisstr != عدم)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((فراغ *)tot_hist, (size_t)len, حجم(histentry_T *), sort_hist);

    // Keep the newest ones.
    لكل (i = 0; i < hislen; i++)
    {
	إذا (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = عدم;
	    tot_hist[i]->hisstrlen = 0;
	    إذا (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	وإلا
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    لكل (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    لكل (i = 0; i < hislen; i++)
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
    ساكن فراغ
finish_viminfo_history(vir_T *virp)
{
    صحيح	type;
    صحيح merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    لكل (type = 0; type < HIST_گنتی; ++type)
    {
	إذا (get_histentry(type) == عدم)
	    استمر;

	إذا (merge)
	    merge_history(type);
	وإلا
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
    ساكن فراغ
write_viminfo_history(FILE *fp, صحيح merge)
{
    صحيح	    i;
    صحيح	    type;
    صحيح	    عدد_saved;
    صحيح     round;
    صحيح	    hislen;

    init_history();
    hislen = get_hislen();
    إذا (hislen == 0)
	أرجع;
    لكل (type = 0; type < HIST_گنتی; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	صحيح	    *hisidx = get_hisidx(type);

	عدد_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	إذا (عدد_saved == 0)
	    استمر;
	إذا (عدد_saved < 0)  // Use default
	    عدد_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_تلاش ? _("Search String") :
			    type == HIST_اظہاریہ ? _("Expression") :
			    type == HIST_داخلہ ? _("Input Line") :
					_("Debug Line"));
	إذا (عدد_saved > hislen)
	    عدد_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	لكل (round = 1; round <= 2; ++round)
	{
	    إذا (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    وإلا إذا (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    وإلا
		// empty list
		i = -1;
	    إذا (i >= 0)
		طالما (عدد_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    صحيح	    c = NUL;

		    إذا (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    وإلا
		    {
			إذا (viminfo_history[type] == عدم)
			{
			    p = عدم;
			    plen = 0;
			    timestamp = 0;
			}
			وإلا
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    إذا (p != عدم && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--عدد_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			إذا (type == HIST_تلاش)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    حرف    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    إذا (c == NUL)
				cbuf[0] = NUL;
			    وإلا
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    إذا (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			إذا (--i < 0)
			    i = hislen - 1;
			إذا (i == *hisidx)
			    اكسر;
		    }
		    وإلا
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	لكل (i = 0; i < viminfo_hisidx[type]; ++i)
	    إذا (viminfo_history[type] != عدم)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    ساكن فراغ
write_viminfo_barlines(vir_T *virp, FILE *fp_out)
{
    صحيح		i;
    garray_T	*gap = &virp->vir_barlines;
    صحيح		seen_useful = FALSE;
    حرف	*سطر;

    إذا (gap->ga_len <= 0)
	أرجع;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    لكل (i = 0; i < gap->ga_len; ++i)
    {
	سطر = ((حرف **)(gap->ga_data))[i];
	إذا (seen_useful || سطر[1] != '<')
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
    ساكن صحيح
barline_parse(vir_T *virp, char_u *text, garray_T *اقدار)
{
    char_u  *p = text;
    char_u  *nextp = عدم;
    char_u  *buf = عدم;
    bval_T  *قدر;
    صحيح	    i;
    صحيح	    allocated = FALSE;
    صحيح	    eof;
    char_u  *sconv;
    صحيح	    converted;

    طالما (*p == ',')
    {
	++p;
	إذا (ga_grow(اقدار, 1) == FAIL)
	    اكسر;
	قدر = (bval_T *)(اقدار->ga_data) + اقدار->ga_len;

	إذا (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    إذا (!allocated)
	    {
		لكل (i = 0; i < اقدار->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(اقدار->ga_data) + i;

		    إذا (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    إذا (vim_isdigit(p[1]))
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
		إذا (buf == عدم)
		    أرجع TRUE;
		p = buf;
		لكل (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    إذا (eof || virp->vir_سطر[0] != '|'
						  || virp->vir_سطر[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			أرجع eof || virp->vir_سطر[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_سطر);
		    طالما (n > 2 && (virp->vir_سطر[n - 1] == NL
					     || virp->vir_سطر[n - 1] == CAR))
			--n;
		    n -= 2;
		    إذا (n > todo)
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
	    وإلا
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		إذا (eof || virp->vir_سطر[0] != '|'
					      || virp->vir_سطر[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    أرجع eof || virp->vir_سطر[0] == '|';
		p = virp->vir_سطر + 2;
	    }
	}

	إذا (SAFE_isdigit(*p))
	{
	    قدر->bv_type = BVAL_NR;
	    قدر->bv_nr = getdigits(&p);
	    ++اقدار->ga_len;
	}
	وإلا إذا (*p == '"')
	{
	    صحيح	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    طالما (*p != '"')
	    {
		إذا (*p == NL || *p == NUL)
		    أرجع TRUE;  // syntax error, drop the value
		إذا (*p == '\\')
		{
		    ++p;
		    إذا (*p == 'n')
			s[len++] = '\n';
		    وإلا
			s[len++] = *p;
		    ++p;
		}
		وإلا
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    قدر->bv_tofree = عدم;
	    إذا (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, عدم);
		إذا (sconv != عدم)
		{
		    إذا (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			قدر->bv_tofree = buf;
		    s = sconv;
		    len = (صحيح)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    إذا (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    قدر->bv_string = s;
	    قدر->bv_type = BVAL_STRING;
	    قدر->bv_len = len;
	    قدر->bv_allocated = allocated || converted;
	    ++اقدار->ga_len;
	    إذا (nextp != عدم)
	    {
		// values following a long string
		p = nextp;
		nextp = عدم;
	    }
	}
	وإلا إذا (*p == ',')
	{
	    قدر->bv_type = BVAL_EMPTY;
	    ++اقدار->ga_len;
	}
	وإلا
	    اكسر;
    }
    أرجع TRUE;
}

    ساكن فراغ
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    ساكن صحيح
no_viminfo(فراغ)
{
    // "vim -i NONE" does not read or write a viminfo file
    أرجع STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    ساكن صحيح
viminfo_error(حرف *errnum, حرف *message, char_u *سطر)
{
    vim_snprintf((حرف *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, سطر, IOSIZE - STRLEN(IObuff) - 1);
    إذا (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((حرف *)IObuff);
    إذا (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    ساكن صحيح
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    صحيح		i;

    إذا (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_سطر, '=');
	إذا (p != عدم)
	{
	    // remove trailing newline
	    ++p;
	    لكل (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    أرجع viminfo_readline(virp);
}

#إذا defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    ساكن صحيح
read_viminfo_varlist(vir_T *virp, صحيح writing)
{
    char_u	*tab;
    صحيح		type = VAR_نمبر;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    إذا (!writing && (find_viminfo_parameter('!') != عدم))
    {
	tab = vim_strchr(virp->vir_سطر + 1, '\t');
	إذا (tab != عدم)
	{
	    *tab++ = '\0';	// isolate the variable name
	    بدل (*tab)
	    {
		حالة 'S': type = VAR_STRING; اكسر;
		حالة 'F': type = VAR_FLOAT; اكسر;
		حالة 'D': type = VAR_DICT; اكسر;
		حالة 'L': type = VAR_LIST; اكسر;
		حالة 'B': type = VAR_BLOB; اكسر;
		حالة 'X': type = VAR_SPECIAL; اكسر;
		حالة 'T': type = VAR_TUPLE; اكسر;
	    }

	    tab = vim_strchr(tab, '\t');
	    إذا (tab != عدم)
	    {
		tv.v_type = type;
		إذا (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (صحيح)(tab - virp->vir_سطر + 1), TRUE);
		وإلا إذا (type == VAR_FLOAT)
		    (فراغ)string2float(tab + 1, &tv.vval.v_float, FALSE);
		وإلا
		{
		    tv.vval.v_نمبر = atol((حرف *)tab + 1);
		    إذا (type == VAR_SPECIAL && (tv.vval.v_نمبر == VVAL_FALSE
					     || tv.vval.v_نمبر == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		إذا (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_اظہاریہ(tv.vval.v_string, عدم);

		    إذا (etv == عدم)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    وإلا
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		وإلا إذا (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    إذا (blob == عدم)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    وإلا
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

		إذا (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		وإلا إذا (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    أرجع viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    ساكن فراغ
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    صحيح		todo;
    حرف	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    إذا (find_viminfo_parameter('!') == عدم)
	أرجع;

    fputs(_("\n# global variables:\n"), fp);

    todo = (صحيح)gvht->ht_used;
    FOR_ALL_HASHTAB_اشیاء(gvht, hi, todo)
    {
	إذا (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    إذا (var_flavour(this_var->di_کلید) == VAR_FLAVOUR_VIMINFO)
	    {
		بدل (this_var->di_tv.v_type)
		{
		    حالة VAR_STRING:  s = "STR"; اكسر;
		    حالة VAR_نمبر:  s = "NUM"; اكسر;
		    حالة VAR_FLOAT:   s = "FLO"; اكسر;
		    حالة VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      صحيح	copyID = get_copyID();

			      s = "DIC";
			      إذا (di != عدم && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, عدم, عدم)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  استمر;
			      اكسر;
			  }
		    حالة VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      صحيح	copyID = get_copyID();

			      s = "LIS";
			      إذا (l != عدم && !set_ref_in_list_اشیاء(
						       l, copyID, عدم, عدم)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  استمر;
			      اكسر;
			  }
		    حالة VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      صحيح	copyID = get_copyID();

			      s = "TUP";
			      إذا (tuple != عدم && !set_ref_in_tuple_اشیاء(
					       tuple, copyID, عدم, عدم)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  استمر;
			      اكسر;
			  }
		    حالة VAR_BLOB:    s = "BLO"; اكسر;
		    حالة VAR_BOOL:    s = "XPL"; اكسر;  // backwards compat.
		    حالة VAR_SPECIAL: s = "XPL"; اكسر;

		    حالة VAR_UNKNOWN:
		    حالة VAR_ANY:
		    حالة VAR_VOID:
		    حالة VAR_FUNC:
		    حالة VAR_PARTIAL:
		    حالة VAR_JOB:
		    حالة VAR_CHANNEL:
		    حالة VAR_INSTR:
		    حالة VAR_CLASS:
		    حالة VAR_OBJECT:
		    حالة VAR_TYPEALIAS:
				      استمر;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_کلید, s);
		إذا (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((حرف *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_نمبر);
		    p = numbuf;
		    tofree = عدم;
		}
		وإلا
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		إذا (p != عدم)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    ساكن صحيح
read_viminfo_sub_string(vir_T *virp, صحيح force)
{
    إذا (force || get_old_sub() == عدم)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    أرجع viminfo_readline(virp);
}

    ساكن فراغ
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    إذا (get_viminfo_parameter('/') == 0 || old_sub == عدم)
	أرجع;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    ساكن صحيح
read_viminfo_تلاش_pattern(vir_T *virp, صحيح force, صحيح writing)
{
    char_u	*lp;
    صحيح		idx = -1;
    صحيح		magic = FALSE;
    صحيح		no_scs = FALSE;
    صحيح		off_سطر = FALSE;
    صحيح		off_end = 0;
    long	off = 0;
    صحيح		setlast = FALSE;
#ifdef FEAT_تلاش_EXTRA
    ساكن صحيح	hlsearch_on = FALSE;
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
    إذا (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	إذا (lp[1] == 'M')		// magic on
	    magic = TRUE;
	إذا (lp[2] == 's')
	    no_scs = TRUE;
	إذا (lp[3] == 'L')
	    off_سطر = TRUE;
	إذا (lp[4] == 'E')
	    off_end = تلاش_END;
	lp += 5;
	off = getdigits(&lp);
    }
    إذا (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    إذا (lp[0] == '/')
	idx = RE_تلاش;
    وإلا إذا (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_تلاش_EXTRA
    وإلا إذا (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    وإلا إذا (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    إذا (idx >= 0)
    {
	spat = get_spat(idx);
	إذا (force || spat->pat == عدم)
	{
	    val = viminfo_readstring(virp, (صحيح)(lp - virp->vir_سطر + 1),
									TRUE);
	    إذا (val != عدم)
	    {
		إذا (!writing)
		{
		    set_last_تلاش_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.سطر = off_سطر;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_تلاش_EXTRA
		    إذا (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    أرجع viminfo_readline(virp);
}

    ساكن فراغ
wvsp_one(
    FILE	*fp,	// file to write to
    صحيح		idx,	// spats[] index
    حرف	*s,	// search pat
    صحيح		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    إذا (spat->pat == عدم)
	أرجع;

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

    ساكن فراغ
write_viminfo_تلاش_pattern(FILE *fp)
{
    إذا (get_viminfo_parameter('/') == 0)
	أرجع;

#ifdef FEAT_تلاش_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != عدم) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_تلاش, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

ساكن yankreg_T *y_read_regs = عدم;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    ساكن فراغ
prepare_viminfo_registers(فراغ)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, عدد_REGISTERS);
}

    ساكن فراغ
finish_viminfo_registers(فراغ)
{
    صحيح		i;
    صحيح		j;

    إذا (y_read_regs == عدم)
	أرجع;

    لكل (i = 0; i < عدد_REGISTERS; ++i)
	إذا (y_read_regs[i].y_array != عدم)
	{
	    لكل (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    ساكن صحيح
read_viminfo_register(vir_T *virp, صحيح force)
{
    صحيح		eof;
    صحيح		do_it = TRUE;
    صحيح		size;
    صحيح		limit;
    صحيح		i;
    صحيح		set_prev = FALSE;
    char_u	*str;
    string_T	*array = عدم;
    صحيح		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_سطر + 1;

    // If the line starts with "" this is the y_previous register.
    إذا (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    إذا (!ASCII_ISALNUM(*str) && *str != '-')
    {
	إذا (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_سطر))
	    أرجع TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    إذا (!force && y_current_p->y_array != عدم)
	do_it = FALSE;

    إذا (*str == '@')
    {
	// "x@: register x used for @@
	إذا (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    إذا (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	إذا (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	إذا (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	وإلا إذا (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	وإلا
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    طالما (!(eof = viminfo_readline(virp))
		    && (virp->vir_سطر[0] == TAB || virp->vir_سطر[0] == '<'))
    {
	إذا (do_it)
	{
	    إذا (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * حجم(string_T));

		إذا (new_array == عدم)
		{
		    do_it = FALSE;
		    اكسر;
		}
		لكل (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    إذا (str != عدم)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    وإلا
		// error, don't store the result
		do_it = FALSE;
	}
    }

    إذا (do_it)
    {
	// free y_array[]
	لكل (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	إذا (size == 0)
	{
	    y_current_p->y_array = عدم;
	}
	وإلا
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    لكل (i = 0; i < size; i++)
	    {
		إذا (y_current_p->y_array == عدم)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		وإلا
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    وإلا
    {
	// Free array[] if it was filled.
	لكل (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    أرجع eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    ساكن فراغ
handle_viminfo_register(garray_T *اقدار, صحيح force)
{
    bval_T	*vp = (bval_T *)اقدار->ga_data;
    صحيح		flags;
    صحيح		name;
    صحيح		type;
    صحيح		linecount;
    صحيح		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    صحيح		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    إذا (اقدار->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	أرجع;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    إذا (name < 0 || name >= عدد_REGISTERS)
	أرجع;
    type = vp[2].bv_nr;
    إذا (type != MCHAR && type != MLINE && type != MBLOCK)
	أرجع;
    linecount = vp[3].bv_nr;
    إذا (اقدار->ga_len < 6 + linecount)
	أرجع;
    width = vp[4].bv_nr;
    إذا (width < 0)
	أرجع;

    إذا (y_read_regs != عدم)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    وإلا
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    إذا (y_ptr->y_array != عدم && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	أرجع;

    إذا (y_ptr->y_array != عدم)
	لكل (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    إذا (y_read_regs == عدم)
    {
	إذا (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	إذا ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    إذا (linecount == 0)
    {
	y_ptr->y_array = عدم;
	أرجع;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    إذا (y_ptr->y_array == عدم)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	أرجع;
    }
    لكل (i = 0; i < linecount; i++)
    {
	إذا (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = عدم;
	}
	وإلا إذا (vp[i + 6].bv_type != BVAL_STRING)
	{
	    حرر(y_ptr->y_array);
	    y_ptr->y_array = عدم;
	}
	وإلا
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    ساكن فراغ
write_viminfo_registers(FILE *fp)
{
    صحيح		i, j;
    char_u	*type;
    char_u	c;
    صحيح		عدد_سطریں;
    صحيح		max_عدد_سطریں;
    صحيح		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_عدد_سطریں = get_viminfo_parameter('<');
    إذا (max_عدد_سطریں < 0)
	max_عدد_سطریں = get_viminfo_parameter('"');
    إذا (max_عدد_سطریں == 0)
	أرجع;
    max_kbyte = get_viminfo_parameter('s');
    إذا (max_kbyte == 0)
	أرجع;

    لكل (i = 0; i < عدد_REGISTERS; i++)
    {
#إذا defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	إذا (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    استمر;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	إذا (i == TILDE_REGISTER)
	    استمر;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	إذا (y_read_regs != عدم
		&& y_read_regs[i].y_array != عدم
		&& (y_regs_p[i].y_array == عدم ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	وإلا إذا (y_regs_p[i].y_array == عدم)
	    استمر;
	وإلا
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	عدد_سطریں = y_ptr->y_size;
	إذا (عدد_سطریں == 0
		|| (عدد_سطریں == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    استمر;

	إذا (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    لكل (j = 0; j < عدد_سطریں; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    إذا (len > (long)max_kbyte * 1024L)
		استمر;
	}

	بدل (y_ptr->y_type)
	{
	    حالة MLINE:
		type = (char_u *)"LINE";
		اكسر;
	    حالة MCHAR:
		type = (char_u *)"CHAR";
		اكسر;
	    حالة MBLOCK:
		type = (char_u *)"BLOCK";
		اكسر;
	    افتراضي:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		اكسر;
	}
	إذا (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	إذا (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (صحيح)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	إذا (max_عدد_سطریں > 0 && عدد_سطریں > max_عدد_سطریں)
	    عدد_سطریں = max_عدد_سطریں;
	لكل (j = 0; j < عدد_سطریں; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    صحيح	    flags = 0;
	    صحيح	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    إذا (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    إذا (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, عدد_سطریں, (صحيح)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    لكل (j = 0; j < عدد_سطریں; j++)
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

ساكن xfmark_T *vi_namedfm = عدم;
ساكن xfmark_T *vi_jumplist = عدم;
ساكن صحيح vi_jumplist_len = 0;

    ساكن فراغ
write_one_mark(FILE *fp_out, صحيح c, pos_T *pos)
{
    إذا (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (صحيح)pos->col);
}

    ساكن فراغ
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    صحيح		i;
    pos_T	pos;

    home_replace(عدم, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_داخل_کرو);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    لكل (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	إذا (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    لكل (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    ساكن صحيح
skip_for_viminfo(buf_T *buf)
{
    أرجع bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    ساكن فراغ
write_viminfo_marks(FILE *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    صحيح		is_mark_set;
    صحيح		i;
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
	إذا (buf->b_marks_read)
	{
	    إذا (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    وإلا
	    {
		is_mark_set = FALSE;
		لكل (i = 0; i < NMARKS; i++)
		    إذا (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			اكسر;
		    }
	    }
	    إذا (is_mark_set && buf->b_ffname != عدم
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		إذا (buflist == عدم)
		    write_buffer_marks(buf, fp_out);
		وإلا إذا (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    ساكن فراغ
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    صحيح		c1,
    صحيح		c2)
{
    char_u	*name;

    إذا (fm->fmark.mark.lnum == 0)	// not set
	أرجع;

    إذا (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    وإلا
	name = fm->fname;		// use name from .viminfo
    إذا (name != عدم && *name != NUL)
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

    إذا (fm->fmark.fnum != 0)
	vim_free(name);
}

    ساكن فراغ
write_viminfo_filemarks(FILE *fp)
{
    صحيح		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    صحيح		vi_idx;
    صحيح		idx;

    إذا (get_viminfo_parameter('f') == 0)
	أرجع;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    لكل (i = 0; i < NMARKS; i++)
    {
	إذا (vi_namedfm != عدم
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	وإلا
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    إذا (curbuf->b_ffname != عدم && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	لكل (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    إذا (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == عدم
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != عدم
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		اكسر;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	لكل ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = عدم;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    لكل (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != عدم ? &vi_namedfm[vi_idx] : عدم;

	إذا (vi_fm != عدم
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	وإلا
	{
	    fm = &namedfm_p[idx++];
	    إذا (vi_fm != عدم
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != عدم
			  && fm->fname != عدم
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
    لكل (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : عدم;
	vi_fm = (vi_jumplist != عدم && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : عدم;
	إذا (fm == عدم && vi_fm == عدم)
	    اكسر;
	إذا (fm == عدم || (vi_fm != عدم && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	وإلا
	    --idx;
	إذا (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != عدم
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    صحيح
buf_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    إذا (buf1->b_last_used == buf2->b_last_used)
	أرجع 0;
    أرجع buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    ساكن فراغ
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_out,
    garray_T	*buflist,
    صحيح		eof,
    صحيح		flags)
{
    char_u	*سطر = virp->vir_سطر;
    buf_T	*buf;
    صحيح		عدد_marked_files;
    صحيح		load_marks;
    صحيح		copy_marks_out;
    char_u	*str;
    صحيح		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = عدم;
#endif
    صحيح		گنتی = 0;
    صحيح		buflist_used = 0;
    buf_T	*buflist_buf = عدم;

    إذا ((name_buf = alloc(LSIZE)) == عدم)
	أرجع;
    *name_buf = NUL;

    إذا (fp_out != عدم && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						حجم(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    إذا (fp_out == عدم && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	إذا (list != عدم)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    عدد_marked_files = get_viminfo_parameter('\'');
    طالما (!eof && (گنتی < عدد_marked_files || fp_out == عدم))
    {
	إذا (سطر[0] != '>')
	{
	    إذا (سطر[0] != '\n' && سطر[0] != '\r' && سطر[0] != '#')
	    {
		إذا (viminfo_error("E576: ", _(e_nonr_missing_gt), سطر))
		    اكسر;	// too many errors, return now
	    }
	    eof = vim_fgets(سطر, LSIZE, virp->vir_fd);
	    استمر;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(سطر + 1);
	str = viminfo_readstring(virp, (صحيح)(str - virp->vir_سطر), FALSE);
	إذا (str == عدم)
	    استمر;
	p = str + STRLEN(str);
	طالما (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	إذا (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	إذا (list != عدم)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	إذا (fp_out == عدم)
	{
	    إذا ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != عدم)
	    {
		إذا (*name_buf == NUL)	    // only need to do this once
		    home_replace(عدم, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		إذا (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	وإلا // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		إذا (buf->b_ffname != عدم)
		{
		    home_replace(عدم, buf->b_ffname, name_buf, LSIZE, TRUE);
		    إذا (fnamecmp(str, name_buf) == 0)
			اكسر;
		}

	    // Copy marks if the buffer has not been loaded.
	    إذا (buf == عدم || !buf->b_marks_read)
	    {
		صحيح	did_read_سطر = FALSE;

		إذا (buflist_buf != عدم)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    إذا (!viminfo_readline(virp) && سطر[0] == TAB)
		    {
			did_read_سطر = TRUE;
			إذا (سطر[1] == '*')
			{
			    long	ltime;

			    sscanf((حرف *)سطر + 2, "%ld ", &ltime);
			    طالما ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				إذا (++گنتی >= عدد_marked_files)
				    اكسر;
				إذا (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = عدم;
				    اكسر;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			وإلا
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    طالما (گنتی < عدد_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++گنتی;
			    }
			    buflist_buf = عدم;
			}

			إذا (گنتی >= عدد_marked_files)
			{
			    vim_free(str);
			    اكسر;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		إذا (did_read_سطر)
		    fputs((حرف *)سطر, fp_out);

		گنتی++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	طالما (!(eof = viminfo_readline(virp)) && سطر[0] == TAB)
	{
	    إذا (load_marks)
	    {
		إذا (سطر[1] != NUL)
		{
		    unsigned u;

		    sscanf((حرف *)سطر + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    بدل (سطر[1])
		    {
			حالة '"': curbuf->b_last_cursor = pos; اكسر;
			حالة '^': curbuf->b_last_داخل_کرو = pos; اكسر;
			حالة '.': curbuf->b_last_change = pos; اكسر;
			حالة '+':
				  // changelist positions are stored oldest
				  // first
				  إذا (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    حجم(pos_T) * (JUMPLISTSIZE - 1));
				  وإلا
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  اكسر;

				  // Using the line number for the last-used
				  // timestamp.
			حالة '*': curbuf->b_last_used = pos.lnum; اكسر;

			افتراضي:  إذا ((i = سطر[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    وإلا إذا (copy_marks_out)
		fputs((حرف *)سطر, fp_out);
	}

	إذا (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		إذا (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    إذا (flags & VIF_ONLY_CURBUF)
		اكسر;
	}
    }

    إذا (fp_out != عدم)
	// Write any remaining entries from buflist.
	طالما (گنتی < عدد_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++گنتی;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    فراغ
check_marks_read(فراغ)
{
    إذا (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != عدم)
	read_viminfo(عدم, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    ساكن صحيح
read_viminfo_filemark(vir_T *virp, صحيح force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    صحيح		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_سطر + 1;
    إذا (*str <= 127
	    && ((*virp->vir_سطر == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_سطر == '-' && *str == '\'')))
    {
	إذا (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    إذا (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = عدم;
	    وإلا
	    {
		لكل (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = عدم;
	    }
	}
	وإلا إذا (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	وإلا
	    fm = &namedfm_p[*str - 'A'];
	إذا (fm != عدم && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (صحيح)(str - virp->vir_سطر),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    أرجع vim_fgets(virp->vir_سطر, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    ساكن فراغ
prepare_viminfo_marks(فراغ)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    ساكن فراغ
finish_viminfo_marks(فراغ)
{
    صحيح		i;

    إذا (vi_namedfm != عدم)
    {
	لكل (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    إذا (vi_jumplist != عدم)
    {
	لكل (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    ساكن فراغ
handle_viminfo_mark(garray_T *اقدار, صحيح force)
{
    bval_T	*vp = (bval_T *)اقدار->ga_data;
    صحيح		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = عدم;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    إذا (اقدار->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	أرجع;

    name = vp[0].bv_nr;
    إذا (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	أرجع;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    إذا (lnum <= 0 || col < 0)
	أرجع;
    timestamp = (time_t)vp[3].bv_nr;

    إذا (name == '\'')
    {
	إذا (vi_jumplist != عدم)
	{
	    إذا (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	وإلا
	{
	    صحيح idx;
	    صحيح i;

	    // If we have a timestamp insert it in the right place.
	    إذا (timestamp != 0)
	    {
		لكل (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    إذا (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			اكسر;
		    }
		// idx cannot be zero now
		إذا (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    وإلا إذا (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    وإلا
		idx = -1;

	    إذا (idx >= 0)
	    {
		إذا (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    لكل (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		وإلا
		{
		    // Move newer entries forward.
		    لكل (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = عدم;
		fm->time_set = 0;
	    }
	}
    }
    وإلا
    {
	صحيح		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	إذا (VIM_ISDIGIT(name))
	{
	    إذا (vi_namedfm != عدم)
		idx = name - '0' + NMARKS;
	    وإلا
	    {
		صحيح i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		لكل (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    إذا (namedfm_p[idx].time_set < timestamp)
			اكسر;
		إذا (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    أرجع;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		لكل ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = عدم;
	    }
	}
	وإلا
	    idx = name - 'A';
	إذا (vi_namedfm != عدم)
	    fm = &vi_namedfm[idx];
	وإلا
	    fm = &namedfm_p[idx];
    }

    إذا (fm != عدم)
    {
	إذا (vi_namedfm != عدم || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    إذا (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = عدم;
	    }
	    وإلا
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    ساكن صحيح
read_viminfo_barline(vir_T *virp, صحيح got_encoding, صحيح force, صحيح writing)
{
    char_u	*p = virp->vir_سطر + 1;
    صحيح		bartype;
    garray_T	اقدار;
    bval_T	*vp;
    صحيح		i;
    صحيح		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    إذا (*p == '<')
    {
	// Continuation line of an unrecognized item.
	إذا (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_سطر);
    }
    وإلا
    {
	ga_init2(&اقدار, حجم(bval_T), 20);
	bartype = getdigits(&p);
	بدل (bartype)
	{
	    حالة BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		إذا (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &اقدار);
		    vp = (bval_T *)اقدار.ga_data;
		    إذا (اقدار.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		اكسر;

	    حالة BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &اقدار);
		handle_viminfo_history(&اقدار, writing);
		اكسر;

	    حالة BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &اقدار);
		handle_viminfo_register(&اقدار, force);
		اكسر;

	    حالة BARTYPE_MARK:
		read_next = barline_parse(virp, p, &اقدار);
		handle_viminfo_mark(&اقدار, force);
		اكسر;

	    افتراضي:
		// copy unrecognized line (for future use)
		إذا (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_سطر);
	}
	لكل (i = 0; i < اقدار.ga_len; ++i)
	{
	    vp = (bval_T *)اقدار.ga_data + i;
	    إذا (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&اقدار);
    }

    إذا (read_next)
	أرجع viminfo_readline(virp);
    أرجع FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    ساكن صحيح
read_viminfo_up_to_marks(
    vir_T	*virp,
    صحيح		forceit,
    صحيح		writing)
{
    صحيح		eof;
    buf_T	*buf;
    صحيح		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    طالما (!eof && virp->vir_سطر[0] != '>')
    {
	بدل (virp->vir_سطر[0])
	{
		// Characters reserved for future expansion, ignored now
	    حالة '+': // "+40 /path/dir file", for running vim without args
	    حالة '^': // to be defined
	    حالة '<': // long line - ignored
		// A comment or empty line.
	    حالة NUL:
	    حالة '\r':
	    حالة '\n':
	    حالة '#':
		eof = viminfo_readline(virp);
		اكسر;
	    حالة '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		اكسر;
	    حالة '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		اكسر;
	    حالة '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#وإلا
		eof = viminfo_readline(virp);
#endif
		اكسر;
	    حالة '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		اكسر;
	    حالة '"':
		// When registers are in bar lines skip the old style register
		// lines.
		إذا (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		وإلا
		    افعل
		    {
			eof = viminfo_readline(virp);
		    } طالما (!eof && (virp->vir_سطر[0] == TAB
						|| virp->vir_سطر[0] == '<'));
		اكسر;
	    حالة '/':	    // Search string
	    حالة '&':	    // Substitute search string
	    حالة '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_تلاش_pattern(virp, forceit, writing);
		اكسر;
	    حالة '$':
		eof = read_viminfo_sub_string(virp, forceit);
		اكسر;
	    حالة ':':
	    حالة '?':
	    حالة '=':
	    حالة '@':
		// When history is in bar lines skip the old style history
		// lines.
		إذا (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		وإلا
		    eof = viminfo_readline(virp);
		اكسر;
	    حالة '-':
	    حالة '\'':
		// When file marks are in bar lines skip the old style lines.
		إذا (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		وإلا
		    eof = viminfo_readline(virp);
		اكسر;
	    افتراضي:
		إذا (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_سطر))
		    eof = TRUE;
		وإلا
		    eof = viminfo_readline(virp);
		اكسر;
	}
    }

    // Finish reading history items.
    إذا (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    أرجع eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    ساكن فراغ
do_viminfo(FILE *fp_in, FILE *fp_out, صحيح flags)
{
    صحيح		eof = FALSE;
    vir_T	vir;
    صحيح		merge = FALSE;
    صحيح		do_copy_marks = FALSE;
    garray_T	buflist;

    إذا ((vir.vir_سطر = alloc(LSIZE)) == عدم)
	أرجع;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, حجم(char_u *), 100);
    vir.vir_version = -1;

    إذا (fp_in != عدم)
    {
	إذا (flags & VIF_WANT_INFO)
	{
	    إذا (fp_out != عدم)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != عدم);
	    merge = TRUE;
	}
	وإلا إذا (flags != 0)
	    // Skip info, find start of marks
	    طالما (!(eof = viminfo_readline(&vir))
		    && vir.vir_سطر[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    إذا (fp_out != عدم)
    {
	// Write the info:
	fprintf(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_out);
	fprintf(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_تلاش_pattern(fp_out);
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

	إذا (do_copy_marks)
	    ga_init2(&buflist, حجم(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : عدم);
    }

    إذا (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	إذا (fp_out != عدم)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_سطر);
    إذا (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, عدم, عدم);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    صحيح
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    صحيح		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    إذا (no_viminfo())
	أرجع FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    إذا (fname == عدم)
	أرجع FAIL;
    fp = mch_fopen((حرف *)fname, READBIN);

    إذا (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == عدم ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    إذا (fp == عدم)
	أرجع FAIL;
    إذا (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	أرجع FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, عدم, flags);

    fclose(fp);
    أرجع OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    فراغ
write_viminfo(char_u *file, صحيح forceit)
{
    char_u	*fname;
    FILE	*fp_in = عدم;	// input viminfo file, if any
    FILE	*fp_out = عدم;	// output viminfo file
    char_u	*tempname = عدم;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#إذا defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    صحيح		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    صحيح		hidden = FALSE;
#endif

    إذا (no_viminfo())
	أرجع;

    fname = viminfo_filename(file);	// may set to default if NULL
    إذا (fname == عدم)
	أرجع;

    fp_in = mch_fopen((حرف *)fname, READBIN);
    إذا (fp_in == عدم)
    {
	صحيح fd;

	// if it does exist, but we can't read it, don't try writing
	إذا (mch_stat((حرف *)fname, &st_new) == 0)
	    اذهب end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((حرف *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	إذا (fd < 0)
	    اذهب end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    وإلا
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	إذا (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != جڑ_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    صحيح	tt = پیغام_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    پیغام_didany = tt;
	    fclose(fp_in);
	    اذهب end;
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
	لكل (;;)
	{
	    صحيح		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#وإلا
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#وإلا
				    (char_u *)".tmp",
#endif
				    FALSE);
	    إذا (tempname == عدم)		// out of memory
		اكسر;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    إذا (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    لكل (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		إذا (mch_stat((حرف *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    إذا (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			اكسر;
		    }
#endif
		}
		وإلا
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((حرف *)tempname, WRITEBIN);
		    (فراغ)umask(umask_save);
#وإلا
		    صحيح	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((حرف *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(صحيح)((st_old.st_mode & 0777) | 0600));
		    (فراغ)umask(umask_save);
# وإلا
		    fd = mch_open((حرف *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    إذا (fd < 0)
		    {
			fp_out = عدم;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			إذا (errno != EEXIST)
			    اكسر;
# endif
		    }
		    وإلا
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    إذا (fp_out != عدم)
			اكسر;
		}

		// Assume file exists, try again with another name.
		إذا (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    اكسر;
		}
		*wp = next_char;
		--next_char;
	    }

	    إذا (tempname != عدم)
		اكسر;
	    // continue if shortname was set
	}

#إذا defined(UNIX) && defined(HAVE_FCHOWN)
	إذا (tempname != عدم && fp_out != عدم)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    إذا (mch_stat((حرف *)tempname, &tmp_st) >= 0)
	    {
		إذا (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		إذا (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (فراغ)mch_setperm(tempname, 0600);
	    }
	    وإلا
		// can't stat the file, set conservative permissions
		(فراغ)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    إذا (fp_out == عدم)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == عدم || tempname == عدم) ? fname : tempname);
	إذا (fp_in != عدم)
	    fclose(fp_in);
	اذهب end;
    }

    إذا (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    إذا (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    إذا (fp_in != عدم)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	إذا (viminfo_errcnt == 0)
	{
	    إذا (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    وإلا إذا (hidden)
		mch_hide(fname);
#endif
	}
	إذا (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    فراغ
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    إذا (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    إذا (eap->cmdidx == CMD_rviminfo)
    {
	إذا (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    وإلا
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
