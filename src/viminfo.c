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
قسمتعریف ساخت
{
    char_u	*vir_سطر;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    صحیح		vir_نسخہ;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

قسمتعریف شمار {
    BVAL_NR,
    BVAL_STRING,
    BVAL_خالی_خانے
} btype_T;

قسمتعریف ساخت {
    btype_T	bv_type;
    طویل	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    صحیح		bv_len;		// length of bv_string
    صحیح		bv_allocated;	// bv_string was allocated
} bval_T;

#اگر defined(FEAT_VIMINFO)

ساکن صحیح  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    ساکن char_u *
ڈھونڈو_viminfo_parameter(صحیح type)
{
    char_u  *p;

    برائے (p = p_viminfo; *p; ++p)
    {
	اگر (*p == type)
	    واپس p + 1;
	اگر (*p == 'n')		    // 'n' is always the last one
	    توڑو;
	p = vim_strchr(p, ',');	    // skip until next ','
	اگر (p == تہی)		    // hit the end without finding parameter
	    توڑو;
    }
    واپس تہی;
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

    p = ڈھونڈو_viminfo_parameter(type);
    اگر (p != تہی && VIM_ISDIGIT(*p))
	واپس atoi((حرف *)p);
    واپس -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    ساکن char_u *
viminfo_filename(char_u *file)
{
    اگر (file == تہی || *file == NUL)
    {
	size_t  len;

	اگر (*p_viminfofile != NUL)
	    file = p_viminfofile;
	ورنہ اگر ((file = ڈھونڈو_viminfo_parameter('n')) == تہی || *file == NUL)
	{
#اگر_تعریف_شدہ VIMINFO_FILE2
# ifdef VMS
	    اگر (mch_getenv((char_u *)"SYS$LOGIN") == تہی)
# ورنہ
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    اگر (STRCMP(vim_getenv((char_u *)"HOME", تہی), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == تہی)
#  ورنہ
	    اگر (mch_getenv((char_u *)"HOME") == تہی)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		اگر (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		ورنہ
		    file = (char_u *)VIMINFO_FILE;
	    }
	    ورنہ
#اگر_ختم
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	واپس vim_strnsave(file, len);
    }

    واپس vim_strsave(file);
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
    ساکن خالی
viminfo_writestring(FILE *fd, char_u *p)
{
    صحیح		c;
    char_u	*s;
    صحیح		len = 0;

    برائے (s = p; *s != NUL; ++s)
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

    جب ((c = *p++) != NUL)
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
    ساکن صحیح
barline_writestring(FILE *fd, char_u *s, صحیح بقایا_آغاز)
{
    char_u *p;
    صحیح	    بقایا = بقایا_آغاز;
    صحیح	    len = 2;

    // Count the number of characters produced, including quotes.
    برائے (p = s; *p != NUL; ++p)
    {
	اگر (*p == NL)
	    len += 2;
	ورنہ اگر (*p == '"' || *p == '\\')
	    len += 2;
	ورنہ
	    ++len;
    }
    اگر (len > بقایا - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	بقایا = LSIZE - 20;
    }

    putc('"', fd);
    برائے (p = s; *p != NUL; ++p)
    {
	اگر (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --بقایا;
	}
	ورنہ اگر (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --بقایا;
	}
	ورنہ
	    putc(*p, fd);
	--بقایا;

	اگر (بقایا < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    بقایا = LSIZE - 20;
	}
    }
    putc('"', fd);
    واپس بقایا - 2;
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
    ساکن char_u *
viminfo_readstring(
    vir_T	*virp,
    صحیح		off,		    // offset for virp->vir_line
    صحیح		convert)	    // convert the string
{
    char_u	*retval = تہی;
    char_u	*s, *d;
    طویل	len;

    اگر (virp->vir_سطر[off] == Ctrl_V && vim_isdigit(virp->vir_سطر[off + 1]))
    {
	len = atol((حرف *)virp->vir_سطر + off + 1);
	اگر (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	اگر (retval == تہی)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (خالی)vim_fgets(virp->vir_سطر, 10, virp->vir_fd);
	    واپس تہی;
	}
	(خالی)vim_fgets(retval, (صحیح)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    ورنہ
    {
	retval = vim_strsave(virp->vir_سطر + off);
	اگر (retval == تہی)
	    واپس تہی;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    جب (*s != NUL && *s != '\n')
    {
	اگر (s[0] == Ctrl_V && s[1] != NUL)
	{
	    اگر (s[1] == 'n')
		*d++ = '\n';
	    ورنہ
		*d++ = Ctrl_V;
	    s += 2;
	}
	ورنہ
	    *d++ = *s++;
    }
    *d = NUL;

    اگر (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, تہی);
	اگر (d != تہی)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    واپس retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    ساکن صحیح
viminfo_readline(vir_T *virp)
{
    واپس vim_fgets(virp->vir_سطر, LSIZE, virp->vir_fd);
}

    ساکن صحیح
read_viminfo_bufferlist(
    vir_T	*virp,
    صحیح		writing)
{
    char_u	*tab;
    linenr_T	lnum;
    colnr_T	ستون;
    buf_T	*buf;
    char_u	*sfname;
    char_u	*xline;

    // Handle long line and escaped characters.
    xline = viminfo_readstring(virp, 1, FALSE);

    // don't read in if there are files on the command-line or if writing:
    اگر (xline != تہی && !writing && ARGCOUNT == 0
				       && ڈھونڈو_viminfo_parameter('%') != تہی)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	ستون = 0;
	tab = vim_strrchr(xline, '\t');
	اگر (tab != تہی)
	{
	    *tab++ = '\0';
	    ستون = (colnr_T)atoi((حرف *)tab);
	    tab = vim_strrchr(xline, '\t');
	    اگر (tab != تہی)
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
	اگر (buf != تہی)	// just in case...
	{
	    buf->b_آخری_cursor.lnum = lnum;
	    buf->b_آخری_cursor.ستون = ستون;
	    buflist_setfpos(buf, curwin, lnum, ستون, FALSE);
	}
    }
    vim_free(xline);

    واپس viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    ساکن صحیح
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    صحیح	    part_len;
    صحیح	    retval = FALSE;

    name = home_replace_save(تہی, name);
    اگر (name == تہی)
	واپس FALSE;
    برائے (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, حجم(part), ", ");
	اگر (part[0] == 'r')
	{
	    اگر (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		توڑو;
	    }
	}
    }
    vim_free(name);
    واپس retval;
}

    ساکن خالی
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*سطر;
    صحیح		max_buffers;

    اگر (ڈھونڈو_viminfo_parameter('%') == تہی)
	واپس;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#تعریف_کرو سطر_BUF_LEN (MAXPATHL + 40)
    سطر = alloc(سطر_BUF_LEN);
    اگر (سطر == تہی)
	واپس;

    FOR_ALL_TAB_WINDOWS(tp, win)
	سیٹ_آخری_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	اگر (buf->b_fname == تہی
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    جاری;

	اگر (max_buffers-- == 0)
	    توڑو;
	putc('%', fp);
	linelen = home_replace(تہی, buf->b_ffname, سطر, MAXPATHL, TRUE);
	vim_snprintf((حرف *)سطر + linelen, سطر_BUF_LEN - linelen, "\t%ld\t%d",
			(طویل)buf->b_آخری_cursor.lnum,
			buf->b_آخری_cursor.ستون);
	viminfo_writestring(fp, سطر);
    }
    vim_free(سطر);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
ساکن histentry_T *viminfo_history[HIST_گنتی] =
					       {تہی, تہی, تہی, تہی, تہی};
ساکن صحیح	viminfo_hisidx[HIST_گنتی] = {0, 0, 0, 0, 0};
ساکن صحیح	viminfo_hislen[HIST_گنتی] = {0, 0, 0, 0, 0};
ساکن صحیح	viminfo_جوڑو_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    ساکن صحیح
hist_type2char(
    صحیح	    type,
    صحیح	    use_question)	    // use '?' instead of '/'
{
    اگر (type == HIST_CMD)
	واپس ':';
    اگر (type == HIST_تلاش)
    {
	اگر (use_question)
	    واپس '?';
	ورنہ
	    واپس '/';
    }
    اگر (type == HIST_اظہاریہ)
	واپس '=';
    واپس '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    ساکن خالی
prepare_viminfo_history(صحیح asklen, صحیح writing)
{
    صحیح	    i;
    صحیح	    عدد;
    صحیح	    type;
    صحیح	    len;
    صحیح	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_جوڑو_at_front = (asklen != 0 && !writing);
    اگر (asklen > hislen)
	asklen = hislen;

    برائے (type = 0; type < HIST_گنتی; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	برائے (i = 0, عدد = 0; i < hislen; i++)
	    اگر (histentry[i].hisstr == تہی || histentry[i].viminfo)
		عدد++;
	len = asklen;
	اگر (عدد > len)
	    len = عدد;
	اگر (len <= 0)
	    viminfo_history[type] = تہی;
	ورنہ
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	اگر (viminfo_history[type] == تہی)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    ساکن صحیح
read_viminfo_history(vir_T *virp, صحیح writing)
{
    صحیح		type;
    long_u	len;
    char_u	*val = تہی;
    char_u	*p;

    type = hist_char2type(virp->vir_سطر[0]);
    اگر (viminfo_hisidx[type] >= viminfo_hislen[type])
	جاؤ مکمل;

    val = viminfo_readstring(virp, 1, TRUE);
    اگر (val == تہی || *val == NUL)
	جاؤ مکمل;

    صحیح sep = (*val == ' ' ? NUL : *val);

    اگر (اندر_history(type, val + (type == HIST_تلاش), viminfo_جوڑو_at_front,
								sep, writing))
	جاؤ مکمل;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    اگر (type == HIST_تلاش)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	اگر (p == تہی)
	    جاؤ مکمل;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    ورنہ
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	اگر (p == تہی)
	    جاؤ مکمل;

	// Not a search entry: No separator in the viminfo
	// file, add a NUL separator.
	mch_memmove(p, val, (size_t)len + 1);	// +1 to include the NUL
	p[len + 1] = NUL;			// put the separator *after* the string's NUL
    }
    viminfo_history[type][viminfo_hisidx[type]].hisstr = p;
    viminfo_history[type][viminfo_hisidx[type]].hisstrlen = (size_t)len;
    viminfo_history[type][viminfo_hisidx[type]].وقت_سیٹ = 0;
    viminfo_history[type][viminfo_hisidx[type]].viminfo = TRUE;
    viminfo_history[type][viminfo_hisidx[type]].hisnum = 0;
    viminfo_hisidx[type]++;

مکمل:
    vim_free(val);
    واپس viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    ساکن خالی
handle_viminfo_history(
	garray_T    *اقدار,
	صحیح	    writing)
{
    صحیح		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)اقدار->ga_ڈیٹا;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    اگر (اقدار->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_خالی_خانے)
	    || vp[3].bv_type != BVAL_STRING)
	واپس;

    type = vp[0].bv_nr;
    اگر (type >= HIST_گنتی)
	واپس;

    اگر (viminfo_hisidx[type] >= viminfo_hislen[type])
	واپس;

    val = vp[3].bv_string;
    اگر (val == تہی || *val == NUL)
	واپس;

    صحیح sep = type == HIST_تلاش && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    صحیح idx;
    صحیح overwrite = FALSE;

    اگر (اندر_history(type, val, viminfo_جوڑو_at_front, sep, writing))
	واپس;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    برائے (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	اگر (STRCMP(val, p) == 0
		&& (type != HIST_تلاش || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    توڑو;
	}
    }

    اگر (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    ورنہ
	len = 0; // for picky compilers
    اگر (p != تہی)
    {
	viminfo_history[type][idx].وقت_سیٹ = vp[1].bv_nr;
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
    ساکن خالی
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
    ورنہ اگر (idx < 0)
	idx = hislen - 1;
    اگر (viminfo_جوڑو_at_front)
	*hisidx = idx;
    ورنہ
    {
	اگر (*hisidx == -1)
	    *hisidx = hislen - 1;
	کرو
	{
	    اگر (histentry[idx].hisstr != تہی || histentry[idx].viminfo)
		توڑو;
	    اگر (++idx == hislen)
		idx = 0;
	} جب (idx != *hisidx);
	اگر (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    برائے (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].وقت_سیٹ = viminfo_history[type][i].وقت_سیٹ;
	اگر (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    برائے (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    ساکن صحیح
ترتیب_دو_hist(ثابت خالی *s1, ثابت خالی *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    اگر (p1->وقت_سیٹ < p2->وقت_سیٹ) واپس -1;
    اگر (p1->وقت_سیٹ > p2->وقت_سیٹ) واپس 1;
    واپس 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    ساکن خالی
ملاؤ_history(صحیح type)
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
    اگر (tot_hist == تہی || new_hist == تہی)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	واپس;
    }
    برائے (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    برائے (i = 0; i < hislen; i++)
	اگر (histentry[i].hisstr != تہی)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((خالی *)tot_hist, (size_t)len, حجم(histentry_T *), ترتیب_دو_hist);

    // Keep the newest ones.
    برائے (i = 0; i < hislen; i++)
    {
	اگر (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = تہی;
	    tot_hist[i]->hisstrlen = 0;
	    اگر (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	ورنہ
	    clear_hist_اندراج(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    برائے (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    برائے (i = 0; i < hislen; i++)
    {
	vim_free(histentry[i].hisstr);
	histentry[i].hisstrlen = 0;
    }
    vim_free(histentry);
    سیٹ_histentry(type, new_hist);
    vim_free(tot_hist);
}

/*
 * Finish reading history lines from viminfo.  Not used when writing viminfo.
 */
    ساکن خالی
اختتام_viminfo_history(vir_T *virp)
{
    صحیح	type;
    صحیح ملاؤ = virp->vir_نسخہ >= VIMINFO_نسخہ_WITH_HISTORY;

    برائے (type = 0; type < HIST_گنتی; ++type)
    {
	اگر (get_histentry(type) == تہی)
	    جاری;

	اگر (ملاؤ)
	    ملاؤ_history(type);
	ورنہ
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
    ساکن خالی
write_viminfo_history(FILE *fp, صحیح ملاؤ)
{
    صحیح	    i;
    صحیح	    type;
    صحیح	    عدد_saved;
    صحیح     باری;
    صحیح	    hislen;

    init_history();
    hislen = get_hislen();
    اگر (hislen == 0)
	واپس;
    برائے (type = 0; type < HIST_گنتی; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	صحیح	    *hisidx = get_hisidx(type);

	عدد_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	اگر (عدد_saved == 0)
	    جاری;
	اگر (عدد_saved < 0)  // Use default
	    عدد_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_تلاش ? _("Search String") :
			    type == HIST_اظہاریہ ? _("Expression") :
			    type == HIST_داخلہ ? _("Input Line") :
					_("Debug Line"));
	اگر (عدد_saved > hislen)
	    عدد_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	برائے (باری = 1; باری <= 2; ++باری)
	{
	    اگر (باری == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    ورنہ اگر (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    ورنہ
		// empty list
		i = -1;
	    اگر (i >= 0)
		جب (عدد_saved > 0
			&& !(باری == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    صحیح	    c = NUL;

		    اگر (باری == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].وقت_سیٹ;
		    }
		    ورنہ
		    {
			اگر (viminfo_history[type] == تہی)
			{
			    p = تہی;
			    plen = 0;
			    timestamp = 0;
			}
			ورنہ
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].وقت_سیٹ;
			}
		    }

		    اگر (p != تہی && (باری == 2
				       || !ملاؤ
				       || !histentry[i].viminfo))
		    {
			--عدد_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			اگر (type == HIST_تلاش)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    حرف    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    اگر (c == NUL)
				cbuf[0] = NUL;
			    ورنہ
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (طویل)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    اگر (باری == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			اگر (--i < 0)
			    i = hislen - 1;
			اگر (i == *hisidx)
			    توڑو;
		    }
		    ورنہ
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	برائے (i = 0; i < viminfo_hisidx[type]; ++i)
	    اگر (viminfo_history[type] != تہی)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    ساکن خالی
write_viminfo_barlines(vir_T *virp, FILE *fp_برآمد)
{
    صحیح		i;
    garray_T	*gap = &virp->vir_barlines;
    صحیح		نظر_آیا_useful = FALSE;
    حرف	*سطر;

    اگر (gap->ga_len <= 0)
	واپس;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_برآمد);

    // Skip over continuation lines until seeing a useful line.
    برائے (i = 0; i < gap->ga_len; ++i)
    {
	سطر = ((حرف **)(gap->ga_ڈیٹا))[i];
	اگر (نظر_آیا_useful || سطر[1] != '<')
	{
	    fputs(سطر, fp_برآمد);
	    نظر_آیا_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    ساکن صحیح
barline_parse(vir_T *virp, char_u *متن, garray_T *اقدار)
{
    char_u  *p = متن;
    char_u  *nextp = تہی;
    char_u  *buf = تہی;
    bval_T  *قدر;
    صحیح	    i;
    صحیح	    allocated = FALSE;
    صحیح	    eof;
    char_u  *sconv;
    صحیح	    converted;

    جب (*p == ',')
    {
	++p;
	اگر (ga_grow(اقدار, 1) == FAIL)
	    توڑو;
	قدر = (bval_T *)(اقدار->ga_ڈیٹا) + اقدار->ga_len;

	اگر (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    اگر (!allocated)
	    {
		برائے (i = 0; i < اقدار->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(اقدار->ga_ڈیٹا) + i;

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
		اگر (buf == تہی)
		    واپس TRUE;
		p = buf;
		برائے (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    اگر (eof || virp->vir_سطر[0] != '|'
						  || virp->vir_سطر[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			واپس eof || virp->vir_سطر[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_سطر);
		    جب (n > 2 && (virp->vir_سطر[n - 1] == NL
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
	    ورنہ
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		اگر (eof || virp->vir_سطر[0] != '|'
					      || virp->vir_سطر[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    واپس eof || virp->vir_سطر[0] == '|';
		p = virp->vir_سطر + 2;
	    }
	}

	اگر (محفوظ_isdigit(*p))
	{
	    قدر->bv_type = BVAL_NR;
	    قدر->bv_nr = getdigits(&p);
	    ++اقدار->ga_len;
	}
	ورنہ اگر (*p == '"')
	{
	    صحیح	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    جب (*p != '"')
	    {
		اگر (*p == NL || *p == NUL)
		    واپس TRUE;  // syntax error, drop the value
		اگر (*p == '\\')
		{
		    ++p;
		    اگر (*p == 'n')
			s[len++] = '\n';
		    ورنہ
			s[len++] = *p;
		    ++p;
		}
		ورنہ
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    قدر->bv_tofree = تہی;
	    اگر (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, تہی);
		اگر (sconv != تہی)
		{
		    اگر (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			قدر->bv_tofree = buf;
		    s = sconv;
		    len = (صحیح)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    اگر (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    قدر->bv_string = s;
	    قدر->bv_type = BVAL_STRING;
	    قدر->bv_len = len;
	    قدر->bv_allocated = allocated || converted;
	    ++اقدار->ga_len;
	    اگر (nextp != تہی)
	    {
		// values following a long string
		p = nextp;
		nextp = تہی;
	    }
	}
	ورنہ اگر (*p == ',')
	{
	    قدر->bv_type = BVAL_خالی_خانے;
	    ++اقدار->ga_len;
	}
	ورنہ
	    توڑو;
    }
    واپس TRUE;
}

    ساکن خالی
write_viminfo_نسخہ(FILE *fp_برآمد)
{
    fprintf(fp_برآمد, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_نسخہ, VIMINFO_نسخہ);
}

    ساکن صحیح
no_viminfo(خالی)
{
    // "vim -i NONE" does not read or write a viminfo file
    واپس STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    ساکن صحیح
viminfo_error(حرف *errnum, حرف *پیغام_2, char_u *سطر)
{
    vim_snprintf((حرف *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, پیغام_2);
    STRNCAT(IObuff, سطر, IOSIZE - STRLEN(IObuff) - 1);
    اگر (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((حرف *)IObuff);
    اگر (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_کا_file));
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    ساکن صحیح
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    صحیح		i;

    اگر (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_سطر, '=');
	اگر (p != تہی)
	{
	    // remove trailing newline
	    ++p;
	    برائے (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_تیاری(&virp->vir_conv, p, p_enc);
	}
    }
    واپس viminfo_readline(virp);
}

#اگر defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    ساکن صحیح
read_viminfo_varlist(vir_T *virp, صحیح writing)
{
    char_u	*tab;
    صحیح		type = VAR_نمبر;
    typval_T	tv;
    funccal_اندراج_T funccal_اندراج;

    اگر (!writing && (ڈھونڈو_viminfo_parameter('!') != تہی))
    {
	tab = vim_strchr(virp->vir_سطر + 1, '\t');
	اگر (tab != تہی)
	{
	    *tab++ = '\0';	// isolate the variable name
	    انتخاب (*tab)
	    {
		حالت 'S': type = VAR_STRING; توڑو;
		حالت 'F': type = VAR_FLOAT; توڑو;
		حالت 'D': type = VAR_DICT; توڑو;
		حالت 'L': type = VAR_LIST; توڑو;
		حالت 'B': type = VAR_BLOB; توڑو;
		حالت 'X': type = VAR_SPECIAL; توڑو;
		حالت 'T': type = VAR_TUPLE; توڑو;
	    }

	    tab = vim_strchr(tab, '\t');
	    اگر (tab != تہی)
	    {
		tv.v_type = type;
		اگر (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (صحیح)(tab - virp->vir_سطر + 1), TRUE);
		ورنہ اگر (type == VAR_FLOAT)
		    (خالی)string2float(tab + 1, &tv.vval.v_float, FALSE);
		ورنہ
		{
		    tv.vval.v_نمبر = atol((حرف *)tab + 1);
		    اگر (type == VAR_SPECIAL && (tv.vval.v_نمبر == VVAL_FALSE
					     || tv.vval.v_نمبر == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		اگر (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_اظہاریہ(tv.vval.v_string, تہی);

		    اگر (etv == تہی)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    ورنہ
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		ورنہ اگر (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    اگر (blob == تہی)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    ورنہ
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_اندراج);
		سیٹ_var(virp->vir_سطر + 1, &tv, FALSE);
		restore_funccal();

		اگر (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		ورنہ اگر (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    واپس viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    ساکن خالی
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    صحیح		todo;
    حرف	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    اگر (ڈھونڈو_viminfo_parameter('!') == تہی)
	واپس;

    fputs(_("\n# global variables:\n"), fp);

    todo = (صحیح)gvht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(gvht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    اگر (var_flavour(this_var->di_کلید) == VAR_FLAVOUR_VIMINFO)
	    {
		انتخاب (this_var->di_tv.v_type)
		{
		    حالت VAR_STRING:  s = "STR"; توڑو;
		    حالت VAR_نمبر:  s = "NUM"; توڑو;
		    حالت VAR_FLOAT:   s = "FLO"; توڑو;
		    حالت VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      صحیح	copyID = get_copyID();

			      s = "DIC";
			      اگر (di != تہی && !سیٹ_ref_اندر_ht(
					 &di->dv_hashtab, copyID, تہی, تہی)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  جاری;
			      توڑو;
			  }
		    حالت VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      صحیح	copyID = get_copyID();

			      s = "LIS";
			      اگر (l != تہی && !سیٹ_ref_اندر_list_اشیاء(
						       l, copyID, تہی, تہی)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  جاری;
			      توڑو;
			  }
		    حالت VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      صحیح	copyID = get_copyID();

			      s = "TUP";
			      اگر (tuple != تہی && !سیٹ_ref_اندر_tuple_اشیاء(
					       tuple, copyID, تہی, تہی)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  جاری;
			      توڑو;
			  }
		    حالت VAR_BLOB:    s = "BLO"; توڑو;
		    حالت VAR_BOOL:    s = "XPL"; توڑو;  // backwards compat.
		    حالت VAR_SPECIAL: s = "XPL"; توڑو;

		    حالت VAR_UNKNOWN:
		    حالت VAR_ANY:
		    حالت VAR_VOID:
		    حالت VAR_FUNC:
		    حالت VAR_PARTIAL:
		    حالت VAR_کام:
		    حالت VAR_CHANNEL:
		    حالت VAR_INSTR:
		    حالت VAR_CLASS:
		    حالت VAR_OBJECT:
		    حالت VAR_TYPEALIAS:
				      جاری;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_کلید, s);
		اگر (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((حرف *)numbuf, "%ld",
					  (طویل)this_var->di_tv.vval.v_نمبر);
		    p = numbuf;
		    tofree = تہی;
		}
		ورنہ
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		اگر (p != تہی)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#اگر_ختم // FEAT_EVAL

    ساکن صحیح
read_viminfo_sub_string(vir_T *virp, صحیح force)
{
    اگر (force || get_old_sub() == تہی)
	سیٹ_old_sub(viminfo_readstring(virp, 1, TRUE));
    واپس viminfo_readline(virp);
}

    ساکن خالی
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    اگر (get_viminfo_parameter('/') == 0 || old_sub == تہی)
	واپس;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    ساکن صحیح
read_viminfo_تلاش_نمونہ(vir_T *virp, صحیح force, صحیح writing)
{
    char_u	*lp;
    صحیح		idx = -1;
    صحیح		magic = FALSE;
    صحیح		no_scs = FALSE;
    صحیح		off_سطر = FALSE;
    صحیح		off_انتہا = 0;
    طویل	off = 0;
    صحیح		setlast = FALSE;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    ساکن صحیح	hlsearch_on = FALSE;
#اگر_ختم
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
	    off_انتہا = تلاش_انتہا;
	lp += 5;
	off = getdigits(&lp);
    }
    اگر (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    اگر (lp[0] == '/')
	idx = RE_تلاش;
    ورنہ اگر (lp[0] == '&')
	idx = RE_SUBST;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    ورنہ اگر (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    ورنہ اگر (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#اگر_ختم
    اگر (idx >= 0)
    {
	spat = get_spat(idx);
	اگر (force || spat->pat == تہی)
	{
	    val = viminfo_readstring(virp, (صحیح)(lp - virp->vir_سطر + 1),
									TRUE);
	    اگر (val != تہی)
	    {
		اگر (!writing)
		{
		    سیٹ_آخری_تلاش_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.سطر = off_سطر;
		    spat->off.انتہا = off_انتہا;
		    spat->off.off = off;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		    اگر (setlast)
			سیٹ_no_hlsearch(!hlsearch_on);
#اگر_ختم
		}
		vim_free(val);
	    }
	}
    }
    واپس viminfo_readline(virp);
}

    ساکن خالی
wvsp_one(
    FILE	*fp,	// file to write to
    صحیح		idx,	// spats[] index
    حرف	*s,	// search pat
    صحیح		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    اگر (spat->pat == تہی)
	واپس;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.سطر ? 'L' : 'l',	// line offset
	    spat->off.انتہا  ? 'E' : 'e',	// offset from end
	    spat->off.off,			//انحراف
	    get_spat_آخری_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    ساکن خالی
write_viminfo_تلاش_نمونہ(FILE *fp)
{
    اگر (get_viminfo_parameter('/') == 0)
	واپس;

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || ڈھونڈو_viminfo_parameter('h') != تہی) ? 'h' : 'H');
#اگر_ختم
    wvsp_one(fp, RE_تلاش, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

ساکن yankreg_T *y_read_regs = تہی;

#تعریف_کرو REG_پچھلا 1
#تعریف_کرو REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    ساکن خالی
prepare_viminfo_registers(خالی)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, عدد_REGISTERS);
}

    ساکن خالی
اختتام_viminfo_registers(خالی)
{
    صحیح		i;
    صحیح		j;

    اگر (y_read_regs == تہی)
	واپس;

    برائے (i = 0; i < عدد_REGISTERS; ++i)
	اگر (y_read_regs[i].y_array != تہی)
	{
	    برائے (j = 0; j < y_read_regs[i].y_سائز; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    ساکن صحیح
read_viminfo_رجسٹر(vir_T *virp, صحیح force)
{
    صحیح		eof;
    صحیح		do_it = TRUE;
    صحیح		سائز;
    صحیح		حد_بندی;
    صحیح		i;
    صحیح		سیٹ_پچھلا = FALSE;
    char_u	*str;
    string_T	*array = تہی;
    صحیح		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_چوڑائی = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_سطر + 1;

    // If the line starts with "" this is the y_previous register.
    اگر (*str == '"')
    {
	سیٹ_پچھلا = TRUE;
	str++;
    }

    اگر (!ASCII_ISALNUM(*str) && *str != '-')
    {
	اگر (viminfo_error("E577: ", _(e_illegal_رجسٹر_name), virp->vir_سطر))
	    واپس TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_رجسٹر(*str++, FALSE);
    y_current_p = get_y_current();
    اگر (!force && y_current_p->y_array != تہی)
	do_it = FALSE;

    اگر (*str == '@')
    {
	// "x@: register x used for @@
	اگر (force || get_execreg_lastc() == NUL)
	    سیٹ_execreg_lastc(str[-1]);
    }

    سائز = 0;
    حد_بندی = 100;	// Optimized for registers containing <= 100 lines
    اگر (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	اگر (سیٹ_پچھلا)
	    سیٹ_y_پچھلا(y_current_p);
	array = ALLOC_MULT(string_T, حد_بندی);
	str = skipwhite(skiptowhite(str));
	اگر (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	ورنہ اگر (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	ورنہ
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_چوڑائی = getdigits(&str);
    }

    جب (!(eof = viminfo_readline(virp))
		    && (virp->vir_سطر[0] == TAB || virp->vir_سطر[0] == '<'))
    {
	اگر (do_it)
	{
	    اگر (سائز == حد_بندی)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)حد_بندی * 2 * حجم(string_T));

		اگر (new_array == تہی)
		{
		    do_it = FALSE;
		    توڑو;
		}
		برائے (i = 0; i < حد_بندی; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		حد_بندی *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    اگر (str != تہی)
	    {
		array[سائز].string = str;
		array[سائز].لمبائی = STRLEN(str);
		++سائز;
	    }
	    ورنہ
		// error, don't store the result
		do_it = FALSE;
	}
    }

    اگر (do_it)
    {
	// free y_array[]
	برائے (i = 0; i < y_current_p->y_سائز; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_چوڑائی = new_چوڑائی;
	y_current_p->y_سائز = سائز;
	y_current_p->y_وقت_سیٹ = 0;
	اگر (سائز == 0)
	{
	    y_current_p->y_array = تہی;
	}
	ورنہ
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, سائز);
	    برائے (i = 0; i < سائز; i++)
	    {
		اگر (y_current_p->y_array == تہی)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		ورنہ
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].لمبائی = array[i].لمبائی;
		}
	    }
	}
    }
    ورنہ
    {
	// Free array[] if it was filled.
	برائے (i = 0; i < سائز; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    واپس eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    ساکن خالی
handle_viminfo_رجسٹر(garray_T *اقدار, صحیح force)
{
    bval_T	*vp = (bval_T *)اقدار->ga_ڈیٹا;
    صحیح		flags;
    صحیح		name;
    صحیح		type;
    صحیح		linecount;
    صحیح		چوڑائی;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    صحیح		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    اگر (اقدار->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	واپس;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    اگر (name < 0 || name >= عدد_REGISTERS)
	واپس;
    type = vp[2].bv_nr;
    اگر (type != MCHAR && type != MLINE && type != MBLOCK)
	واپس;
    linecount = vp[3].bv_nr;
    اگر (اقدار->ga_len < 6 + linecount)
	واپس;
    چوڑائی = vp[4].bv_nr;
    اگر (چوڑائی < 0)
	واپس;

    اگر (y_read_regs != تہی)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    ورنہ
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    اگر (y_ptr->y_array != تہی && !force
			 && (timestamp == 0 || y_ptr->y_وقت_سیٹ > timestamp))
	واپس;

    اگر (y_ptr->y_array != تہی)
	برائے (i = 0; i < y_ptr->y_سائز; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    اگر (y_read_regs == تہی)
    {
	اگر (flags & REG_پچھلا)
	    سیٹ_y_پچھلا(y_ptr);
	اگر ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    سیٹ_execreg_lastc(get_رجسٹر_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_چوڑائی = چوڑائی;
    y_ptr->y_سائز = linecount;
    y_ptr->y_وقت_سیٹ = timestamp;
    اگر (linecount == 0)
    {
	y_ptr->y_array = تہی;
	واپس;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    اگر (y_ptr->y_array == تہی)
    {
	y_ptr->y_سائز = 0; // ensure object state is consistent
	واپس;
    }
    برائے (i = 0; i < linecount; i++)
    {
	اگر (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].لمبائی = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = تہی;
	}
	ورنہ اگر (vp[i + 6].bv_type != BVAL_STRING)
	{
	    آزاد(y_ptr->y_array);
	    y_ptr->y_array = تہی;
	}
	ورنہ
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].لمبائی = vp[i + 6].bv_len;
	}
    }
}

    ساکن خالی
write_viminfo_registers(FILE *fp)
{
    صحیح		i, j;
    char_u	*type;
    char_u	c;
    صحیح		عدد_سطریں;
    صحیح		max_عدد_سطریں;
    صحیح		max_kbyte;
    طویل	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_عدد_سطریں = get_viminfo_parameter('<');
    اگر (max_عدد_سطریں < 0)
	max_عدد_سطریں = get_viminfo_parameter('"');
    اگر (max_عدد_سطریں == 0)
	واپس;
    max_kbyte = get_viminfo_parameter('s');
    اگر (max_kbyte == 0)
	واپس;

    برائے (i = 0; i < عدد_REGISTERS; i++)
    {
#اگر defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	اگر (i == ستارہ_رجسٹر || i == PLUS_رجسٹر || i == REAL_PLUS_رجسٹر)
	    جاری;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DND
	// Neither do we want the '~' register
	اگر (i == TILDE_رجسٹر)
	    جاری;
#اگر_ختم
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	اگر (y_read_regs != تہی
		&& y_read_regs[i].y_array != تہی
		&& (y_regs_p[i].y_array == تہی ||
			    y_read_regs[i].y_وقت_سیٹ > y_regs_p[i].y_وقت_سیٹ))
	    y_ptr = &y_read_regs[i];
	ورنہ اگر (y_regs_p[i].y_array == تہی)
	    جاری;
	ورنہ
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	عدد_سطریں = y_ptr->y_سائز;
	اگر (عدد_سطریں == 0
		|| (عدد_سطریں == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    جاری;

	اگر (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    برائے (j = 0; j < عدد_سطریں; j++)
		len += (طویل)y_ptr->y_array[j].لمبائی + 1L;
	    اگر (len > (طویل)max_kbyte * 1024L)
		جاری;
	}

	انتخاب (y_ptr->y_type)
	{
	    حالت MLINE:
		type = (char_u *)"LINE";
		توڑو;
	    حالت MCHAR:
		type = (char_u *)"CHAR";
		توڑو;
	    حالت MBLOCK:
		type = (char_u *)"BLOCK";
		توڑو;
	    طےشدہ:
		semsg(_(e_unknown_رجسٹر_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		توڑو;
	}
	اگر (get_y_پچھلا() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_رجسٹر_name(i);
	fprintf(fp, "\"%c", c);
	اگر (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (صحیح)y_ptr->y_چوڑائی);

	// If max_num_lines < 0, then we save ALL the lines in the register
	اگر (max_عدد_سطریں > 0 && عدد_سطریں > max_عدد_سطریں)
	    عدد_سطریں = max_عدد_سطریں;
	برائے (j = 0; j < عدد_سطریں; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    صحیح	    flags = 0;
	    صحیح	    بقایا;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    اگر (get_y_پچھلا() == &y_regs_p[i])
		flags |= REG_پچھلا;
	    اگر (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_رجسٹر, flags,
		    i, y_ptr->y_type, عدد_سطریں, (صحیح)y_ptr->y_چوڑائی,
		    (طویل)y_ptr->y_وقت_سیٹ);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    بقایا = LSIZE - 71;
	    برائے (j = 0; j < عدد_سطریں; j++)
	    {
		putc(',', fp);
		--بقایا;
		بقایا = barline_writestring(fp, y_ptr->y_array[j].string,
								   بقایا);
	    }
	    putc('\n', fp);
	}
    }
}

/*
 * Functions relating to reading/writing marks from viminfo
 */

ساکن xfmark_T *vi_namedfm = تہی;
ساکن xfmark_T *vi_jumplist = تہی;
ساکن صحیح vi_jumplist_len = 0;

    ساکن خالی
write_one_mark(FILE *fp_برآمد, صحیح c, pos_T *pos)
{
    اگر (pos->lnum != 0)
	fprintf(fp_برآمد, "\t%c\t%ld\t%d\n", c, (طویل)pos->lnum, (صحیح)pos->ستون);
}

    ساکن خالی
write_بفر_marks(buf_T *buf, FILE *fp_برآمد)
{
    صحیح		i;
    pos_T	pos;

    home_replace(تہی, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_برآمد, "\n> ");
    viminfo_writestring(fp_برآمد, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_آخری_مستعمل;
    pos.ستون = 0;
    write_one_mark(fp_برآمد, '*', &pos);

    write_one_mark(fp_برآمد, '"', &buf->b_آخری_cursor);
    write_one_mark(fp_برآمد, '^', &buf->b_آخری_داخل_کرو);
    write_one_mark(fp_برآمد, '.', &buf->b_آخری_change);
    // changelist positions are stored oldest first
    برائے (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	اگر (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_برآمد, '+', &buf->b_changelist[i]);
    }
    برائے (i = 0; i < NMARKS; i++)
	write_one_mark(fp_برآمد, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    ساکن صحیح
skip_for_viminfo(buf_T *buf)
{
    واپس bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    ساکن خالی
write_viminfo_marks(FILE *fp_برآمد, garray_T *buflist)
{
    buf_T	*buf;
    صحیح		is_mark_سیٹ;
    صحیح		i;
    win_T	*win;
    tabpage_T	*tp;

    // Set b_last_cursor for the all buffers that have a window.
    FOR_ALL_TAB_WINDOWS(tp, win)
	سیٹ_آخری_cursor(win);

    fputs(_("\n# History of marks within files (newest to oldest):\n"), fp_برآمد);
    FOR_ALL_BUFFERS(buf)
    {
	// Only write something if buffer has been loaded and at least one
	// mark is set.
	اگر (buf->b_marks_read)
	{
	    اگر (buf->b_آخری_cursor.lnum != 0)
		is_mark_سیٹ = TRUE;
	    ورنہ
	    {
		is_mark_سیٹ = FALSE;
		برائے (i = 0; i < NMARKS; i++)
		    اگر (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_سیٹ = TRUE;
			توڑو;
		    }
	    }
	    اگر (is_mark_سیٹ && buf->b_ffname != تہی
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		اگر (buflist == تہی)
		    write_بفر_marks(buf, fp_برآمد);
		ورنہ اگر (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_ڈیٹا)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    ساکن خالی
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    صحیح		c1,
    صحیح		c2)
{
    char_u	*name;

    اگر (fm->fmark.mark.lnum == 0)	// not set
	واپس;

    اگر (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    ورنہ
	name = fm->fname;		// use name from .viminfo
    اگر (name != تہی && *name != NUL)
    {
	fprintf(fp, "%c%c  %ld  %ld  ", c1, c2, (طویل)fm->fmark.mark.lnum,
						    (طویل)fm->fmark.mark.ستون);
	viminfo_writestring(fp, name);

	// Barline: |{bartype},{name},{lnum},{col},{timestamp},{filename}
	// size up to filename: 8 + 3 * 20
	fprintf(fp, "|%d,%d,%ld,%ld,%ld,", BARTYPE_MARK, c2,
		(طویل)fm->fmark.mark.lnum, (طویل)fm->fmark.mark.ستون,
		(طویل)fm->وقت_سیٹ);
	barline_writestring(fp, name, LSIZE - 70);
	putc('\n', fp);
    }

    اگر (fm->fmark.fnum != 0)
	vim_free(name);
}

    ساکن خالی
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
	واپس;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    برائے (i = 0; i < NMARKS; i++)
    {
	اگر (vi_namedfm != تہی
			&& (vi_namedfm[i].وقت_سیٹ > namedfm_p[i].وقت_سیٹ))
	    fm = &vi_namedfm[i];
	ورنہ
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    اگر (curbuf->b_ffname != تہی && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	برائے (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    اگر (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == تہی
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != تہی
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		توڑو;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	برائے ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = تہی;
	namedfm_p[NMARKS].وقت_سیٹ = vim_وقت();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    برائے (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != تہی ? &vi_namedfm[vi_idx] : تہی;

	اگر (vi_fm != تہی
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->وقت_سیٹ > namedfm_p[idx].وقت_سیٹ
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ورنہ
	{
	    fm = &namedfm_p[idx++];
	    اگر (vi_fm != تہی
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->وقت_سیٹ == fm->وقت_سیٹ
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != تہی
			  && fm->fname != تہی
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
    برائے (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : تہی;
	vi_fm = (vi_jumplist != تہی && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : تہی;
	اگر (fm == تہی && vi_fm == تہی)
	    توڑو;
	اگر (fm == تہی || (vi_fm != تہی && fm->وقت_سیٹ < vi_fm->وقت_سیٹ))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ورنہ
	    --idx;
	اگر (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != تہی
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    صحیح
buf_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    اگر (buf1->b_آخری_مستعمل == buf2->b_آخری_مستعمل)
	واپس 0;
    واپس buf1->b_آخری_مستعمل > buf2->b_آخری_مستعمل ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    ساکن خالی
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_برآمد,
    garray_T	*buflist,
    صحیح		eof,
    صحیح		flags)
{
    char_u	*سطر = virp->vir_سطر;
    buf_T	*buf;
    صحیح		عدد_marked_files;
    صحیح		بھرو_marks;
    صحیح		copy_marks_برآمد;
    char_u	*str;
    صحیح		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#اگر_تعریف_شدہ FEAT_EVAL
    list_T	*list = تہی;
#اگر_ختم
    صحیح		گنتی = 0;
    صحیح		buflist_مستعمل = 0;
    buf_T	*buflist_buf = تہی;

    اگر ((name_buf = alloc(LSIZE)) == تہی)
	واپس;
    *name_buf = NUL;

    اگر (fp_برآمد != تہی && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_ڈیٹا, (size_t)buflist->ga_len,
						حجم(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_ڈیٹا)[0];
    }

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (fp_برآمد == تہی && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	اگر (list != تہی)
	    سیٹ_vim_var_list(VV_OLDFILES, list);
    }
#اگر_ختم

    عدد_marked_files = get_viminfo_parameter('\'');
    جب (!eof && (گنتی < عدد_marked_files || fp_برآمد == تہی))
    {
	اگر (سطر[0] != '>')
	{
	    اگر (سطر[0] != '\n' && سطر[0] != '\r' && سطر[0] != '#')
	    {
		اگر (viminfo_error("E576: ", _(e_nonr_missing_gt), سطر))
		    توڑو;	// too many errors, return now
	    }
	    eof = vim_fgets(سطر, LSIZE, virp->vir_fd);
	    جاری;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(سطر + 1);
	str = viminfo_readstring(virp, (صحیح)(str - virp->vir_سطر), FALSE);
	اگر (str == تہی)
	    جاری;
	p = str + STRLEN(str);
	جب (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	اگر (*p)
	    p++;
	*p = NUL;

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (list != تہی)
	    list_append_string(list, str, -1);
#اگر_ختم

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	بھرو_marks = copy_marks_برآمد = FALSE;
	اگر (fp_برآمد == تہی)
	{
	    اگر ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != تہی)
	    {
		اگر (*name_buf == NUL)	    // only need to do this once
		    home_replace(تہی, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		اگر (fnamecmp(str, name_buf) == 0)
		    بھرو_marks = TRUE;
	    }
	}
	ورنہ // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		اگر (buf->b_ffname != تہی)
		{
		    home_replace(تہی, buf->b_ffname, name_buf, LSIZE, TRUE);
		    اگر (fnamecmp(str, name_buf) == 0)
			توڑو;
		}

	    // Copy marks if the buffer has not been loaded.
	    اگر (buf == تہی || !buf->b_marks_read)
	    {
		صحیح	did_read_سطر = FALSE;

		اگر (buflist_buf != تہی)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    اگر (!viminfo_readline(virp) && سطر[0] == TAB)
		    {
			did_read_سطر = TRUE;
			اگر (سطر[1] == '*')
			{
			    طویل	ltime;

			    لڑی_سے_پڑھو((حرف *)سطر + 2, "%ld ", &ltime);
			    جب ((وقت_T)ltime < buflist_buf->b_آخری_مستعمل)
			    {
				write_بفر_marks(buflist_buf, fp_برآمد);
				اگر (++گنتی >= عدد_marked_files)
				    توڑو;
				اگر (++buflist_مستعمل == buflist->ga_len)
				{
				    buflist_buf = تہی;
				    توڑو;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_ڈیٹا)[buflist_مستعمل];
			    }
			}
			ورنہ
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    جب (گنتی < عدد_marked_files
					    && buflist_مستعمل < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_ڈیٹا)
							     [buflist_مستعمل++];
				write_بفر_marks(buflist_buf, fp_برآمد);
				++گنتی;
			    }
			    buflist_buf = تہی;
			}

			اگر (گنتی >= عدد_marked_files)
			{
			    vim_free(str);
			    توڑو;
			}
		    }
		}

		fputs("\n> ", fp_برآمد);
		viminfo_writestring(fp_برآمد, str);
		اگر (did_read_سطر)
		    fputs((حرف *)سطر, fp_برآمد);

		گنتی++;
		copy_marks_برآمد = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	جب (!(eof = viminfo_readline(virp)) && سطر[0] == TAB)
	{
	    اگر (بھرو_marks)
	    {
		اگر (سطر[1] != NUL)
		{
		    غیرعلامتی u;

		    لڑی_سے_پڑھو((حرف *)سطر + 2, "%ld %u", &pos.lnum, &u);
		    pos.ستون = u;
		    انتخاب (سطر[1])
		    {
			حالت '"': curbuf->b_آخری_cursor = pos; توڑو;
			حالت '^': curbuf->b_آخری_داخل_کرو = pos; توڑو;
			حالت '.': curbuf->b_آخری_change = pos; توڑو;
			حالت '+':
				  // changelist positions are stored oldest
				  //پہلا
				  اگر (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    حجم(pos_T) * (JUMPLISTSIZE - 1));
				  ورنہ
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  توڑو;

				  // Using the line number for the last-used
				  // timestamp.
			حالت '*': curbuf->b_آخری_مستعمل = pos.lnum; توڑو;

			طےشدہ:  اگر ((i = سطر[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    ورنہ اگر (copy_marks_برآمد)
		fputs((حرف *)سطر, fp_برآمد);
	}

	اگر (بھرو_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		اگر (wp->w_بفر == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    اگر (flags & VIF_ONLY_CURBUF)
		توڑو;
	}
    }

    اگر (fp_برآمد != تہی)
	// Write any remaining entries from buflist.
	جب (گنتی < عدد_marked_files && buflist_مستعمل < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_ڈیٹا)[buflist_مستعمل++];
	    write_بفر_marks(buflist_buf, fp_برآمد);
	    ++گنتی;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    خالی
check_marks_read(خالی)
{
    اگر (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != تہی)
	read_viminfo(تہی, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    ساکن صحیح
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
				       && (VIM_ISDIGIT(*str) || محفوظ_isupper(*str)))
	     || (*virp->vir_سطر == '-' && *str == '\'')))
    {
	اگر (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    اگر (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = تہی;
	    ورنہ
	    {
		برائے (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = تہی;
	    }
	}
	ورنہ اگر (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	ورنہ
	    fm = &namedfm_p[*str - 'A'];
	اگر (fm != تہی && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.ستون = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (صحیح)(str - virp->vir_سطر),
								       FALSE);
	    fm->وقت_سیٹ = 0;
	}
    }
    واپس vim_fgets(virp->vir_سطر, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    ساکن خالی
prepare_viminfo_marks(خالی)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    ساکن خالی
اختتام_viminfo_marks(خالی)
{
    صحیح		i;

    اگر (vi_namedfm != تہی)
    {
	برائے (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    اگر (vi_jumplist != تہی)
    {
	برائے (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    ساکن خالی
handle_viminfo_mark(garray_T *اقدار, صحیح force)
{
    bval_T	*vp = (bval_T *)اقدار->ga_ڈیٹا;
    صحیح		name;
    linenr_T	lnum;
    colnr_T	ستون;
    time_t	timestamp;
    xfmark_T	*fm = تہی;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    اگر (اقدار->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	واپس;

    name = vp[0].bv_nr;
    اگر (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	واپس;
    lnum = vp[1].bv_nr;
    ستون = vp[2].bv_nr;
    اگر (lnum <= 0 || ستون < 0)
	واپس;
    timestamp = (time_t)vp[3].bv_nr;

    اگر (name == '\'')
    {
	اگر (vi_jumplist != تہی)
	{
	    اگر (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	ورنہ
	{
	    صحیح idx;
	    صحیح i;

	    // If we have a timestamp insert it in the right place.
	    اگر (timestamp != 0)
	    {
		برائے (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    اگر (curwin->w_jumplist[idx].وقت_سیٹ < timestamp)
		    {
			++idx;
			توڑو;
		    }
		// idx cannot be zero now
		اگر (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    ورنہ اگر (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    ورنہ
		idx = -1;

	    اگر (idx >= 0)
	    {
		اگر (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    برائے (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		ورنہ
		{
		    // Move newer entries forward.
		    برائے (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = تہی;
		fm->وقت_سیٹ = 0;
	    }
	}
    }
    ورنہ
    {
	صحیح		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	اگر (VIM_ISDIGIT(name))
	{
	    اگر (vi_namedfm != تہی)
		idx = name - '0' + NMARKS;
	    ورنہ
	    {
		صحیح i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		برائے (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    اگر (namedfm_p[idx].وقت_سیٹ < timestamp)
			توڑو;
		اگر (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    واپس;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		برائے ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = تہی;
	    }
	}
	ورنہ
	    idx = name - 'A';
	اگر (vi_namedfm != تہی)
	    fm = &vi_namedfm[idx];
	ورنہ
	    fm = &namedfm_p[idx];
    }

    اگر (fm != تہی)
    {
	اگر (vi_namedfm != تہی || fm->fmark.mark.lnum == 0
					  || fm->وقت_سیٹ < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.ستون = ستون;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    اگر (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = تہی;
	    }
	    ورنہ
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->وقت_سیٹ = timestamp;
	}
    }
}

    ساکن صحیح
read_viminfo_barline(vir_T *virp, صحیح got_encoding, صحیح force, صحیح writing)
{
    char_u	*p = virp->vir_سطر + 1;
    صحیح		bartype;
    garray_T	اقدار;
    bval_T	*vp;
    صحیح		i;
    صحیح		read_اگلا = TRUE;

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
    ورنہ
    {
	ga_init2(&اقدار, حجم(bval_T), 20);
	bartype = getdigits(&p);
	انتخاب (bartype)
	{
	    حالت BARTYPE_نسخہ:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		اگر (!got_encoding)
		{
		    read_اگلا = barline_parse(virp, p, &اقدار);
		    vp = (bval_T *)اقدار.ga_ڈیٹا;
		    اگر (اقدار.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_نسخہ = vp->bv_nr;
		}
		توڑو;

	    حالت BARTYPE_HISTORY:
		read_اگلا = barline_parse(virp, p, &اقدار);
		handle_viminfo_history(&اقدار, writing);
		توڑو;

	    حالت BARTYPE_رجسٹر:
		read_اگلا = barline_parse(virp, p, &اقدار);
		handle_viminfo_رجسٹر(&اقدار, force);
		توڑو;

	    حالت BARTYPE_MARK:
		read_اگلا = barline_parse(virp, p, &اقدار);
		handle_viminfo_mark(&اقدار, force);
		توڑو;

	    طےشدہ:
		// copy unrecognized line (for future use)
		اگر (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_سطر);
	}
	برائے (i = 0; i < اقدار.ga_len; ++i)
	{
	    vp = (bval_T *)اقدار.ga_ڈیٹا + i;
	    اگر (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&اقدار);
    }

    اگر (read_اگلا)
	واپس viminfo_readline(virp);
    واپس FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    ساکن صحیح
read_viminfo_اوپر_مقصد_marks(
    vir_T	*virp,
    صحیح		forceit,
    صحیح		writing)
{
    صحیح		eof;
    buf_T	*buf;
    صحیح		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    جب (!eof && virp->vir_سطر[0] != '>')
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
		توڑو;
	    حالت '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		توڑو;
	    حالت '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		توڑو;
	    حالت '!': // global variable
#اگر_تعریف_شدہ FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#ورنہ
		eof = viminfo_readline(virp);
#اگر_ختم
		توڑو;
	    حالت '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		توڑو;
	    حالت '"':
		// When registers are in bar lines skip the old style register
		// lines.
		اگر (virp->vir_نسخہ < VIMINFO_نسخہ_WITH_REGISTERS)
		    eof = read_viminfo_رجسٹر(virp, forceit);
		ورنہ
		    کرو
		    {
			eof = viminfo_readline(virp);
		    } جب (!eof && (virp->vir_سطر[0] == TAB
						|| virp->vir_سطر[0] == '<'));
		توڑو;
	    حالت '/':	    // Search string
	    حالت '&':	    // Substitute search string
	    حالت '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_تلاش_نمونہ(virp, forceit, writing);
		توڑو;
	    حالت '$':
		eof = read_viminfo_sub_string(virp, forceit);
		توڑو;
	    حالت ':':
	    حالت '?':
	    حالت '=':
	    حالت '@':
		// When history is in bar lines skip the old style history
		// lines.
		اگر (virp->vir_نسخہ < VIMINFO_نسخہ_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		ورنہ
		    eof = viminfo_readline(virp);
		توڑو;
	    حالت '-':
	    حالت '\'':
		// When file marks are in bar lines skip the old style lines.
		اگر (virp->vir_نسخہ < VIMINFO_نسخہ_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		ورنہ
		    eof = viminfo_readline(virp);
		توڑو;
	    طےشدہ:
		اگر (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_سطر))
		    eof = TRUE;
		ورنہ
		    eof = viminfo_readline(virp);
		توڑو;
	}
    }

    // Finish reading history items.
    اگر (!writing)
	اختتام_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_نام(buf);

    واپس eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    ساکن خالی
do_viminfo(FILE *fp_اندر, FILE *fp_برآمد, صحیح flags)
{
    صحیح		eof = FALSE;
    vir_T	vir;
    صحیح		ملاؤ = FALSE;
    صحیح		do_copy_marks = FALSE;
    garray_T	buflist;

    اگر ((vir.vir_سطر = alloc(LSIZE)) == تہی)
	واپس;
    vir.vir_fd = fp_اندر;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, حجم(char_u *), 100);
    vir.vir_نسخہ = -1;

    اگر (fp_اندر != تہی)
    {
	اگر (flags & VIF_WANT_INFO)
	{
	    اگر (fp_برآمد != تہی)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_اوپر_مقصد_marks(&vir,
					 flags & VIF_FORCEIT, fp_برآمد != تہی);
	    ملاؤ = TRUE;
	}
	ورنہ اگر (flags != 0)
	    // Skip info, find start of marks
	    جب (!(eof = viminfo_readline(&vir))
		    && vir.vir_سطر[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    اگر (fp_برآمد != تہی)
    {
	// Write the info:
	fprintf(fp_برآمد, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_نسخہ_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_برآمد);
	write_viminfo_نسخہ(fp_برآمد);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_برآمد);
	fprintf(fp_برآمد, "*encoding=%s\n\n", p_enc);
	write_viminfo_تلاش_نمونہ(fp_برآمد);
	write_viminfo_sub_string(fp_برآمد);
	write_viminfo_history(fp_برآمد, ملاؤ);
	write_viminfo_registers(fp_برآمد);
	اختتام_viminfo_registers();
#اگر_تعریف_شدہ FEAT_EVAL
	write_viminfo_varlist(fp_برآمد);
#اگر_ختم
	write_viminfo_filemarks(fp_برآمد);
	اختتام_viminfo_marks();
	write_viminfo_bufferlist(fp_برآمد);
	write_viminfo_barlines(&vir, fp_برآمد);

	اگر (do_copy_marks)
	    ga_init2(&buflist, حجم(buf_T *), 50);
	write_viminfo_marks(fp_برآمد, do_copy_marks ? &buflist : تہی);
    }

    اگر (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_برآمد, &buflist, eof, flags);
	اگر (fp_برآمد != تہی)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_سطر);
    اگر (vir.vir_conv.vc_type != CONV_NONE)
	convert_تیاری(&vir.vir_conv, تہی, تہی);
    ga_clear_لڑیاں(&vir.vir_barlines);
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
	واپس FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    اگر (fname == تہی)
	واپس FAIL;
    fp = mch_fopen((حرف *)fname, READBIN);

    اگر (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == تہی ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    اگر (fp == تہی)
	واپس FAIL;
    اگر (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_منوال))
    {
	fclose(fp);
	واپس FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, تہی, flags);

    fclose(fp);
    واپس OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    خالی
write_viminfo(char_u *file, صحیح forceit)
{
    char_u	*fname;
    FILE	*fp_اندر = تہی;	// input viminfo file, if any
    FILE	*fp_برآمد = تہی;	// output viminfo file
    char_u	*tempname = تہی;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#اگر defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#اگر_ختم
#اگر_تعریف_شدہ UNIX
    صحیح		shortname = FALSE;	// use 8.3 file name
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    صحیح		hidden = FALSE;
#اگر_ختم

    اگر (no_viminfo())
	واپس;

    fname = viminfo_filename(file);	// may set to default if NULL
    اگر (fname == تہی)
	واپس;

    fp_اندر = mch_fopen((حرف *)fname, READBIN);
    اگر (fp_اندر == تہی)
    {
	صحیح fd;

	// if it does exist, but we can't read it, don't try writing
	اگر (mch_stat((حرف *)fname, &st_new) == 0)
	    جاؤ انتہا;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((حرف *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	اگر (fd < 0)
	    جاؤ انتہا;
	fp_برآمد = fdopen(fd, WRITEBIN);
    }
    ورنہ
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	اگر (mch_fstat(fileno(fp_اندر), &st_old) < 0
		|| S_ISDIR(st_old.st_منوال)
#اگر_تعریف_شدہ UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != جڑ_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_منوال & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_منوال & 0020)
				    : (st_old.st_منوال & 0002))))
#اگر_ختم
		)
	{
	    صحیح	tt = پیغام_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    پیغام_didany = tt;
	    fclose(fp_اندر);
	    جاؤ انتہا;
	}
#اگر_تعریف_شدہ MSWIN
	// Get the file attributes of the existing viminfo file.
	hidden = mch_ishidden(fname);
#اگر_ختم

	// Make tempname, find one that does not exist yet.
	// Beware of a race condition: If someone logs out and all Vim
	// instances exit at the same time a temp file might be created between
	// stat() and open().  Use mch_open() with O_EXCL to avoid that.
	// May try twice: Once normal and once with shortname set, just in
	// case somebody puts his viminfo file in an 8.3 filesystem.
	برائے (;;)
	{
	    صحیح		اگلا_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#اگر_تعریف_شدہ UNIX
				    shortname,
#ورنہ
				    FALSE,
#اگر_ختم
				    fname,
#اگر_تعریف_شدہ VMS
				    (char_u *)"-tmp",
#ورنہ
				    (char_u *)".tmp",
#اگر_ختم
				    FALSE);
	    اگر (tempname == تہی)		// out of memory
		توڑو;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    اگر (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    برائے (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		اگر (mch_stat((حرف *)tempname, &st_new) == 0)
		{
#اگر_تعریف_شدہ UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    اگر (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			توڑو;
		    }
#اگر_ختم
		}
		ورنہ
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#اگر_تعریف_شدہ VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_برآمد = mch_fopen((حرف *)tempname, WRITEBIN);
		    (خالی)umask(umask_save);
#ورنہ
		    صحیح	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((حرف *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(صحیح)((st_old.st_منوال & 0777) | 0600));
		    (خالی)umask(umask_save);
# ورنہ
		    fd = mch_open((حرف *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    اگر (fd < 0)
		    {
			fp_برآمد = تہی;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			اگر (errno != EEXIST)
			    توڑو;
# endif
		    }
		    ورنہ
			fp_برآمد = fdopen(fd, WRITEBIN);
#اگر_ختم // VMS
		    اگر (fp_برآمد != تہی)
			توڑو;
		}

		// Assume file exists, try again with another name.
		اگر (اگلا_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_عارضی_files_like_str), tempname);
		    توڑو;
		}
		*wp = اگلا_char;
		--اگلا_char;
	    }

	    اگر (tempname != تہی)
		توڑو;
	    // continue if shortname was set
	}

#اگر defined(UNIX) && defined(HAVE_FCHOWN)
	اگر (tempname != تہی && fp_برآمد != تہی)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    اگر (mch_stat((حرف *)tempname, &tmp_st) >= 0)
	    {
		اگر (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_برآمد), st_old.st_uid, -1);
		اگر (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_برآمد), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (خالی)mch_setperm(tempname, 0600);
	    }
	    ورنہ
		// can't stat the file, set conservative permissions
		(خالی)mch_setperm(tempname, 0600);
	}
#اگر_ختم
    }

    // Check if the new viminfo file can be written to.
    اگر (fp_برآمد == تہی)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_اندر == تہی || tempname == تہی) ? fname : tempname);
	اگر (fp_اندر != تہی)
	    fclose(fp_اندر);
	جاؤ انتہا;
    }

    اگر (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_اندر, fp_برآمد, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    اگر (fclose(fp_برآمد) == EOF)
	++viminfo_errcnt;

    اگر (fp_اندر != تہی)
    {
	fclose(fp_اندر);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	اگر (viminfo_errcnt == 0)
	{
	    اگر (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_مقصد_str), fname);
	    }
#اگر_تعریف_شدہ MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    ورنہ اگر (hidden)
		mch_hide(fname);
#اگر_ختم
	}
	اگر (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

انتہا:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    خالی
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
    ورنہ
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#اگر_ختم // FEAT_VIMINFO
