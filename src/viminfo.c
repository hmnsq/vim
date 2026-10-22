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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*vir_పంక్తి;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    పూర్ణాంకం		vir_సంస్కరణ;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

రకనిర్వచనం గణన {
    BVAL_NR,
    BVAL_STRING,
    BVAL_ఖాళీలు
} btype_T;

రకనిర్వచనం నిర్మాణం {
    btype_T	bv_type;
    దీర్ఘ	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    పూర్ణాంకం		bv_len;		// length of bv_string
    పూర్ణాంకం		bv_allocated;	// bv_string was allocated
} bval_T;

#ఒకవేళ defined(FEAT_VIMINFO)

స్థిర పూర్ణాంకం  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    స్థిర char_u *
కనుగొను_viminfo_parameter(పూర్ణాంకం type)
{
    char_u  *p;

    కొరకు (p = p_viminfo; *p; ++p)
    {
	ఒకవేళ (*p == type)
	    తిరిగి p + 1;
	ఒకవేళ (*p == 'n')		    // 'n' is always the last one
	    ఆపు;
	p = vim_strchr(p, ',');	    // skip until next ','
	ఒకవేళ (p == ఖాళీ)		    // hit the end without finding parameter
	    ఆపు;
    }
    తిరిగి ఖాళీ;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    పూర్ణాంకం
get_viminfo_parameter(పూర్ణాంకం type)
{
    char_u  *p;

    p = కనుగొను_viminfo_parameter(type);
    ఒకవేళ (p != ఖాళీ && VIM_ISDIGIT(*p))
	తిరిగి atoi((అక్షరం *)p);
    తిరిగి -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    స్థిర char_u *
viminfo_filename(char_u *file)
{
    ఒకవేళ (file == ఖాళీ || *file == NUL)
    {
	size_t  len;

	ఒకవేళ (*p_viminfofile != NUL)
	    file = p_viminfofile;
	లేకపోతే ఒకవేళ ((file = కనుగొను_viminfo_parameter('n')) == ఖాళీ || *file == NUL)
	{
#నిర్వచితమైతే VIMINFO_FILE2
# ifdef VMS
	    ఒకవేళ (mch_getenv((char_u *)"SYS$LOGIN") == ఖాళీ)
# లేకపోతే
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    ఒకవేళ (STRCMP(vim_getenv((char_u *)"HOME", ఖాళీ), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == ఖాళీ)
#  లేకపోతే
	    ఒకవేళ (mch_getenv((char_u *)"HOME") == ఖాళీ)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		ఒకవేళ (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		లేకపోతే
		    file = (char_u *)VIMINFO_FILE;
	    }
	    లేకపోతే
#అయితే_ముగింపు
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	తిరిగి vim_strnsave(file, len);
    }

    తిరిగి vim_strsave(file);
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
    స్థిర శూన్య
viminfo_writestring(FILE *fd, char_u *p)
{
    పూర్ణాంకం		c;
    char_u	*s;
    పూర్ణాంకం		len = 0;

    కొరకు (s = p; *s != NUL; ++s)
    {
	ఒకవేళ (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    ఒకవేళ (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    అయితే ((c = *p++) != NUL)
    {
	ఒకవేళ (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    ఒకవేళ (c == '\n')
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
    స్థిర పూర్ణాంకం
barline_writestring(FILE *fd, char_u *s, పూర్ణాంకం మిగిలిన_ప్రారంభం)
{
    char_u *p;
    పూర్ణాంకం	    మిగిలిన = మిగిలిన_ప్రారంభం;
    పూర్ణాంకం	    len = 2;

    // Count the number of characters produced, including quotes.
    కొరకు (p = s; *p != NUL; ++p)
    {
	ఒకవేళ (*p == NL)
	    len += 2;
	లేకపోతే ఒకవేళ (*p == '"' || *p == '\\')
	    len += 2;
	లేకపోతే
	    ++len;
    }
    ఒకవేళ (len > మిగిలిన - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	మిగిలిన = LSIZE - 20;
    }

    putc('"', fd);
    కొరకు (p = s; *p != NUL; ++p)
    {
	ఒకవేళ (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --మిగిలిన;
	}
	లేకపోతే ఒకవేళ (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --మిగిలిన;
	}
	లేకపోతే
	    putc(*p, fd);
	--మిగిలిన;

	ఒకవేళ (మిగిలిన < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    మిగిలిన = LSIZE - 20;
	}
    }
    putc('"', fd);
    తిరిగి మిగిలిన - 2;
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
    స్థిర char_u *
viminfo_readstring(
    vir_T	*virp,
    పూర్ణాంకం		off,		    // offset for virp->vir_line
    పూర్ణాంకం		convert)	    // convert the string
{
    char_u	*retval = ఖాళీ;
    char_u	*s, *d;
    దీర్ఘ	len;

    ఒకవేళ (virp->vir_పంక్తి[off] == Ctrl_V && vim_isdigit(virp->vir_పంక్తి[off + 1]))
    {
	len = atol((అక్షరం *)virp->vir_పంక్తి + off + 1);
	ఒకవేళ (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	ఒకవేళ (retval == ఖాళీ)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (శూన్య)vim_fgets(virp->vir_పంక్తి, 10, virp->vir_fd);
	    తిరిగి ఖాళీ;
	}
	(శూన్య)vim_fgets(retval, (పూర్ణాంకం)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    లేకపోతే
    {
	retval = vim_strsave(virp->vir_పంక్తి + off);
	ఒకవేళ (retval == ఖాళీ)
	    తిరిగి ఖాళీ;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    అయితే (*s != NUL && *s != '\n')
    {
	ఒకవేళ (s[0] == Ctrl_V && s[1] != NUL)
	{
	    ఒకవేళ (s[1] == 'n')
		*d++ = '\n';
	    లేకపోతే
		*d++ = Ctrl_V;
	    s += 2;
	}
	లేకపోతే
	    *d++ = *s++;
    }
    *d = NUL;

    ఒకవేళ (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, ఖాళీ);
	ఒకవేళ (d != ఖాళీ)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    తిరిగి retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    స్థిర పూర్ణాంకం
viminfo_readline(vir_T *virp)
{
    తిరిగి vim_fgets(virp->vir_పంక్తి, LSIZE, virp->vir_fd);
}

    స్థిర పూర్ణాంకం
read_viminfo_bufferlist(
    vir_T	*virp,
    పూర్ణాంకం		writing)
{
    char_u	*tab;
    linenr_T	lnum;
    colnr_T	నిలువు;
    buf_T	*buf;
    char_u	*sfname;
    char_u	*xline;

    // Handle long line and escaped characters.
    xline = viminfo_readstring(virp, 1, FALSE);

    // don't read in if there are files on the command-line or if writing:
    ఒకవేళ (xline != ఖాళీ && !writing && ARGCOUNT == 0
				       && కనుగొను_viminfo_parameter('%') != ఖాళీ)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	నిలువు = 0;
	tab = vim_strrchr(xline, '\t');
	ఒకవేళ (tab != ఖాళీ)
	{
	    *tab++ = '\0';
	    నిలువు = (colnr_T)atoi((అక్షరం *)tab);
	    tab = vim_strrchr(xline, '\t');
	    ఒకవేళ (tab != ఖాళీ)
	    {
		*tab++ = '\0';
		lnum = atol((అక్షరం *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	ఒకవేళ (buf != ఖాళీ)	// just in case...
	{
	    buf->b_చివరి_cursor.lnum = lnum;
	    buf->b_చివరి_cursor.నిలువు = నిలువు;
	    buflist_setfpos(buf, curwin, lnum, నిలువు, FALSE);
	}
    }
    vim_free(xline);

    తిరిగి viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    స్థిర పూర్ణాంకం
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    పూర్ణాంకం	    part_len;
    పూర్ణాంకం	    retval = FALSE;

    name = home_replace_save(ఖాళీ, name);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి FALSE;
    కొరకు (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, పరిమాణం(part), ", ");
	ఒకవేళ (part[0] == 'r')
	{
	    ఒకవేళ (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		ఆపు;
	    }
	}
    }
    vim_free(name);
    తిరిగి retval;
}

    స్థిర శూన్య
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*పంక్తి;
    పూర్ణాంకం		max_buffers;

    ఒకవేళ (కనుగొను_viminfo_parameter('%') == ఖాళీ)
	తిరిగి;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#నిర్వచించు పంక్తి_BUF_LEN (MAXPATHL + 40)
    పంక్తి = alloc(పంక్తి_BUF_LEN);
    ఒకవేళ (పంక్తి == ఖాళీ)
	తిరిగి;

    FOR_ALL_TAB_WINDOWS(tp, win)
	సమితి_చివరి_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	ఒకవేళ (buf->b_fname == ఖాళీ
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    కొనసాగు;

	ఒకవేళ (max_buffers-- == 0)
	    ఆపు;
	putc('%', fp);
	linelen = home_replace(ఖాళీ, buf->b_ffname, పంక్తి, MAXPATHL, TRUE);
	vim_snprintf((అక్షరం *)పంక్తి + linelen, పంక్తి_BUF_LEN - linelen, "\t%ld\t%d",
			(దీర్ఘ)buf->b_చివరి_cursor.lnum,
			buf->b_చివరి_cursor.నిలువు);
	viminfo_writestring(fp, పంక్తి);
    }
    vim_free(పంక్తి);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
స్థిర histentry_T *viminfo_history[HIST_లెక్క] =
					       {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};
స్థిర పూర్ణాంకం	viminfo_hisidx[HIST_లెక్క] = {0, 0, 0, 0, 0};
స్థిర పూర్ణాంకం	viminfo_hislen[HIST_లెక్క] = {0, 0, 0, 0, 0};
స్థిర పూర్ణాంకం	viminfo_కూడు_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    స్థిర పూర్ణాంకం
hist_type2char(
    పూర్ణాంకం	    type,
    పూర్ణాంకం	    use_question)	    // use '?' instead of '/'
{
    ఒకవేళ (type == HIST_CMD)
	తిరిగి ':';
    ఒకవేళ (type == HIST_శోధన)
    {
	ఒకవేళ (use_question)
	    తిరిగి '?';
	లేకపోతే
	    తిరిగి '/';
    }
    ఒకవేళ (type == HIST_సమాసం)
	తిరిగి '=';
    తిరిగి '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    స్థిర శూన్య
prepare_viminfo_history(పూర్ణాంకం asklen, పూర్ణాంకం writing)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    అంకె;
    పూర్ణాంకం	    type;
    పూర్ణాంకం	    len;
    పూర్ణాంకం	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_కూడు_at_front = (asklen != 0 && !writing);
    ఒకవేళ (asklen > hislen)
	asklen = hislen;

    కొరకు (type = 0; type < HIST_లెక్క; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	కొరకు (i = 0, అంకె = 0; i < hislen; i++)
	    ఒకవేళ (histentry[i].hisstr == ఖాళీ || histentry[i].viminfo)
		అంకె++;
	len = asklen;
	ఒకవేళ (అంకె > len)
	    len = అంకె;
	ఒకవేళ (len <= 0)
	    viminfo_history[type] = ఖాళీ;
	లేకపోతే
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	ఒకవేళ (viminfo_history[type] == ఖాళీ)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    స్థిర పూర్ణాంకం
read_viminfo_history(vir_T *virp, పూర్ణాంకం writing)
{
    పూర్ణాంకం		type;
    long_u	len;
    char_u	*val = ఖాళీ;
    char_u	*p;

    type = hist_char2type(virp->vir_పంక్తి[0]);
    ఒకవేళ (viminfo_hisidx[type] >= viminfo_hislen[type])
	వెళ్ళు పూర్తి;

    val = viminfo_readstring(virp, 1, TRUE);
    ఒకవేళ (val == ఖాళీ || *val == NUL)
	వెళ్ళు పూర్తి;

    పూర్ణాంకం sep = (*val == ' ' ? NUL : *val);

    ఒకవేళ (లోపల_history(type, val + (type == HIST_శోధన), viminfo_కూడు_at_front,
								sep, writing))
	వెళ్ళు పూర్తి;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    ఒకవేళ (type == HIST_శోధన)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	ఒకవేళ (p == ఖాళీ)
	    వెళ్ళు పూర్తి;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    లేకపోతే
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	ఒకవేళ (p == ఖాళీ)
	    వెళ్ళు పూర్తి;

	// Not a search entry: No separator in the viminfo
	// file, add a NUL separator.
	mch_memmove(p, val, (size_t)len + 1);	// +1 to include the NUL
	p[len + 1] = NUL;			// put the separator *after* the string's NUL
    }
    viminfo_history[type][viminfo_hisidx[type]].hisstr = p;
    viminfo_history[type][viminfo_hisidx[type]].hisstrlen = (size_t)len;
    viminfo_history[type][viminfo_hisidx[type]].సమయం_సమితి = 0;
    viminfo_history[type][viminfo_hisidx[type]].viminfo = TRUE;
    viminfo_history[type][viminfo_hisidx[type]].hisnum = 0;
    viminfo_hisidx[type]++;

పూర్తి:
    vim_free(val);
    తిరిగి viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    స్థిర శూన్య
handle_viminfo_history(
	garray_T    *విలువలు,
	పూర్ణాంకం	    writing)
{
    పూర్ణాంకం		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)విలువలు->ga_దత్తాంశం;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    ఒకవేళ (విలువలు->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_ఖాళీలు)
	    || vp[3].bv_type != BVAL_STRING)
	తిరిగి;

    type = vp[0].bv_nr;
    ఒకవేళ (type >= HIST_లెక్క)
	తిరిగి;

    ఒకవేళ (viminfo_hisidx[type] >= viminfo_hislen[type])
	తిరిగి;

    val = vp[3].bv_string;
    ఒకవేళ (val == ఖాళీ || *val == NUL)
	తిరిగి;

    పూర్ణాంకం sep = type == HIST_శోధన && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    పూర్ణాంకం idx;
    పూర్ణాంకం overwrite = FALSE;

    ఒకవేళ (లోపల_history(type, val, viminfo_కూడు_at_front, sep, writing))
	తిరిగి;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    కొరకు (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	ఒకవేళ (STRCMP(val, p) == 0
		&& (type != HIST_శోధన || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    ఆపు;
	}
    }

    ఒకవేళ (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    లేకపోతే
	len = 0; // for picky compilers
    ఒకవేళ (p != ఖాళీ)
    {
	viminfo_history[type][idx].సమయం_సమితి = vp[1].bv_nr;
	ఒకవేళ (!overwrite)
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
    స్థిర శూన్య
concat_history(పూర్ణాంకం type)
{
    పూర్ణాంకం		idx;
    పూర్ణాంకం		i;
    పూర్ణాంకం		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    పూర్ణాంకం		*hisidx = get_hisidx(type);
    పూర్ణాంకం		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    ఒకవేళ (idx >= hislen)
	idx -= hislen;
    లేకపోతే ఒకవేళ (idx < 0)
	idx = hislen - 1;
    ఒకవేళ (viminfo_కూడు_at_front)
	*hisidx = idx;
    లేకపోతే
    {
	ఒకవేళ (*hisidx == -1)
	    *hisidx = hislen - 1;
	చేయి
	{
	    ఒకవేళ (histentry[idx].hisstr != ఖాళీ || histentry[idx].viminfo)
		ఆపు;
	    ఒకవేళ (++idx == hislen)
		idx = 0;
	} అయితే (idx != *hisidx);
	ఒకవేళ (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    కొరకు (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].సమయం_సమితి = viminfo_history[type][i].సమయం_సమితి;
	ఒకవేళ (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    కొరకు (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    స్థిర పూర్ణాంకం
క్రమీకరించు_hist(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    ఒకవేళ (p1->సమయం_సమితి < p2->సమయం_సమితి) తిరిగి -1;
    ఒకవేళ (p1->సమయం_సమితి > p2->సమయం_సమితి) తిరిగి 1;
    తిరిగి 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    స్థిర శూన్య
విలీనం_history(పూర్ణాంకం type)
{
    పూర్ణాంకం		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    పూర్ణాంకం		i;
    పూర్ణాంకం		len;
    పూర్ణాంకం		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    పూర్ణాంకం		*hisidx = get_hisidx(type);
    పూర్ణాంకం		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    ఒకవేళ (tot_hist == ఖాళీ || new_hist == ఖాళీ)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	తిరిగి;
    }
    కొరకు (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    కొరకు (i = 0; i < hislen; i++)
	ఒకవేళ (histentry[i].hisstr != ఖాళీ)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((శూన్య *)tot_hist, (size_t)len, పరిమాణం(histentry_T *), క్రమీకరించు_hist);

    // Keep the newest ones.
    కొరకు (i = 0; i < hislen; i++)
    {
	ఒకవేళ (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = ఖాళీ;
	    tot_hist[i]->hisstrlen = 0;
	    ఒకవేళ (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	లేకపోతే
	    clear_hist_నమోదు(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    కొరకు (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    కొరకు (i = 0; i < hislen; i++)
    {
	vim_free(histentry[i].hisstr);
	histentry[i].hisstrlen = 0;
    }
    vim_free(histentry);
    సమితి_histentry(type, new_hist);
    vim_free(tot_hist);
}

/*
 * Finish reading history lines from viminfo.  Not used when writing viminfo.
 */
    స్థిర శూన్య
ముగింపు_viminfo_history(vir_T *virp)
{
    పూర్ణాంకం	type;
    పూర్ణాంకం విలీనం = virp->vir_సంస్కరణ >= VIMINFO_సంస్కరణ_WITH_HISTORY;

    కొరకు (type = 0; type < HIST_లెక్క; ++type)
    {
	ఒకవేళ (get_histentry(type) == ఖాళీ)
	    కొనసాగు;

	ఒకవేళ (విలీనం)
	    విలీనం_history(type);
	లేకపోతే
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
    స్థిర శూన్య
write_viminfo_history(FILE *fp, పూర్ణాంకం విలీనం)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    type;
    పూర్ణాంకం	    అంకె_saved;
    పూర్ణాంకం     విడత;
    పూర్ణాంకం	    hislen;

    init_history();
    hislen = get_hislen();
    ఒకవేళ (hislen == 0)
	తిరిగి;
    కొరకు (type = 0; type < HIST_లెక్క; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	పూర్ణాంకం	    *hisidx = get_hisidx(type);

	అంకె_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	ఒకవేళ (అంకె_saved == 0)
	    కొనసాగు;
	ఒకవేళ (అంకె_saved < 0)  // Use default
	    అంకె_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_శోధన ? _("Search String") :
			    type == HIST_సమాసం ? _("Expression") :
			    type == HIST_ఆదానం ? _("Input Line") :
					_("Debug Line"));
	ఒకవేళ (అంకె_saved > hislen)
	    అంకె_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	కొరకు (విడత = 1; విడత <= 2; ++విడత)
	{
	    ఒకవేళ (విడత == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    లేకపోతే ఒకవేళ (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    లేకపోతే
		// empty list
		i = -1;
	    ఒకవేళ (i >= 0)
		అయితే (అంకె_saved > 0
			&& !(విడత == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    పూర్ణాంకం	    c = NUL;

		    ఒకవేళ (విడత == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].సమయం_సమితి;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (viminfo_history[type] == ఖాళీ)
			{
			    p = ఖాళీ;
			    plen = 0;
			    timestamp = 0;
			}
			లేకపోతే
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].సమయం_సమితి;
			}
		    }

		    ఒకవేళ (p != ఖాళీ && (విడత == 2
				       || !విలీనం
				       || !histentry[i].viminfo))
		    {
			--అంకె_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			ఒకవేళ (type == HIST_శోధన)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    అక్షరం    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    ఒకవేళ (c == NUL)
				cbuf[0] = NUL;
			    లేకపోతే
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (దీర్ఘ)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    ఒకవేళ (విడత == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			ఒకవేళ (--i < 0)
			    i = hislen - 1;
			ఒకవేళ (i == *hisidx)
			    ఆపు;
		    }
		    లేకపోతే
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	కొరకు (i = 0; i < viminfo_hisidx[type]; ++i)
	    ఒకవేళ (viminfo_history[type] != ఖాళీ)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    స్థిర శూన్య
write_viminfo_barlines(vir_T *virp, FILE *fp_బహిర్గతం)
{
    పూర్ణాంకం		i;
    garray_T	*gap = &virp->vir_barlines;
    పూర్ణాంకం		చూసిన_useful = FALSE;
    అక్షరం	*పంక్తి;

    ఒకవేళ (gap->ga_len <= 0)
	తిరిగి;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_బహిర్గతం);

    // Skip over continuation lines until seeing a useful line.
    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	పంక్తి = ((అక్షరం **)(gap->ga_దత్తాంశం))[i];
	ఒకవేళ (చూసిన_useful || పంక్తి[1] != '<')
	{
	    fputs(పంక్తి, fp_బహిర్గతం);
	    చూసిన_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    స్థిర పూర్ణాంకం
barline_parse(vir_T *virp, char_u *పాఠ్యం, garray_T *విలువలు)
{
    char_u  *p = పాఠ్యం;
    char_u  *nextp = ఖాళీ;
    char_u  *buf = ఖాళీ;
    bval_T  *విలువ;
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    allocated = FALSE;
    పూర్ణాంకం	    eof;
    char_u  *sconv;
    పూర్ణాంకం	    converted;

    అయితే (*p == ',')
    {
	++p;
	ఒకవేళ (ga_grow(విలువలు, 1) == FAIL)
	    ఆపు;
	విలువ = (bval_T *)(విలువలు->ga_దత్తాంశం) + విలువలు->ga_len;

	ఒకవేళ (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    ఒకవేళ (!allocated)
	    {
		కొరకు (i = 0; i < విలువలు->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(విలువలు->ga_దత్తాంశం) + i;

		    ఒకవేళ (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    ఒకవేళ (vim_isdigit(p[1]))
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
		ఒకవేళ (buf == ఖాళీ)
		    తిరిగి TRUE;
		p = buf;
		కొరకు (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    ఒకవేళ (eof || virp->vir_పంక్తి[0] != '|'
						  || virp->vir_పంక్తి[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			తిరిగి eof || virp->vir_పంక్తి[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_పంక్తి);
		    అయితే (n > 2 && (virp->vir_పంక్తి[n - 1] == NL
					     || virp->vir_పంక్తి[n - 1] == CAR))
			--n;
		    n -= 2;
		    ఒకవేళ (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_పంక్తి + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_పంక్తి + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    లేకపోతే
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		ఒకవేళ (eof || virp->vir_పంక్తి[0] != '|'
					      || virp->vir_పంక్తి[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    తిరిగి eof || virp->vir_పంక్తి[0] == '|';
		p = virp->vir_పంక్తి + 2;
	    }
	}

	ఒకవేళ (సురక్షిత_isdigit(*p))
	{
	    విలువ->bv_type = BVAL_NR;
	    విలువ->bv_nr = getdigits(&p);
	    ++విలువలు->ga_len;
	}
	లేకపోతే ఒకవేళ (*p == '"')
	{
	    పూర్ణాంకం	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    అయితే (*p != '"')
	    {
		ఒకవేళ (*p == NL || *p == NUL)
		    తిరిగి TRUE;  // syntax error, drop the value
		ఒకవేళ (*p == '\\')
		{
		    ++p;
		    ఒకవేళ (*p == 'n')
			s[len++] = '\n';
		    లేకపోతే
			s[len++] = *p;
		    ++p;
		}
		లేకపోతే
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    విలువ->bv_tofree = ఖాళీ;
	    ఒకవేళ (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, ఖాళీ);
		ఒకవేళ (sconv != ఖాళీ)
		{
		    ఒకవేళ (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			విలువ->bv_tofree = buf;
		    s = sconv;
		    len = (పూర్ణాంకం)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    ఒకవేళ (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    విలువ->bv_string = s;
	    విలువ->bv_type = BVAL_STRING;
	    విలువ->bv_len = len;
	    విలువ->bv_allocated = allocated || converted;
	    ++విలువలు->ga_len;
	    ఒకవేళ (nextp != ఖాళీ)
	    {
		// values following a long string
		p = nextp;
		nextp = ఖాళీ;
	    }
	}
	లేకపోతే ఒకవేళ (*p == ',')
	{
	    విలువ->bv_type = BVAL_ఖాళీలు;
	    ++విలువలు->ga_len;
	}
	లేకపోతే
	    ఆపు;
    }
    తిరిగి TRUE;
}

    స్థిర శూన్య
write_viminfo_సంస్కరణ(FILE *fp_బహిర్గతం)
{
    fprintf(fp_బహిర్గతం, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_సంస్కరణ, VIMINFO_సంస్కరణ);
}

    స్థిర పూర్ణాంకం
no_viminfo(శూన్య)
{
    // "vim -i NONE" does not read or write a viminfo file
    తిరిగి STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    స్థిర పూర్ణాంకం
viminfo_error(అక్షరం *errnum, అక్షరం *సందేశం_2, char_u *పంక్తి)
{
    vim_snprintf((అక్షరం *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, సందేశం_2);
    STRNCAT(IObuff, పంక్తి, IOSIZE - STRLEN(IObuff) - 1);
    ఒకవేళ (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((అక్షరం *)IObuff);
    ఒకవేళ (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_యొక్క_file));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    స్థిర పూర్ణాంకం
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    పూర్ణాంకం		i;

    ఒకవేళ (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_పంక్తి, '=');
	ఒకవేళ (p != ఖాళీ)
	{
	    // remove trailing newline
	    ++p;
	    కొరకు (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_అమరిక(&virp->vir_conv, p, p_enc);
	}
    }
    తిరిగి viminfo_readline(virp);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    స్థిర పూర్ణాంకం
read_viminfo_varlist(vir_T *virp, పూర్ణాంకం writing)
{
    char_u	*tab;
    పూర్ణాంకం		type = VAR_సంఖ్య;
    typval_T	tv;
    funccal_నమోదు_T funccal_నమోదు;

    ఒకవేళ (!writing && (కనుగొను_viminfo_parameter('!') != ఖాళీ))
    {
	tab = vim_strchr(virp->vir_పంక్తి + 1, '\t');
	ఒకవేళ (tab != ఖాళీ)
	{
	    *tab++ = '\0';	// isolate the variable name
	    ఎంపిక (*tab)
	    {
		స్థితి 'S': type = VAR_STRING; ఆపు;
		స్థితి 'F': type = VAR_FLOAT; ఆపు;
		స్థితి 'D': type = VAR_DICT; ఆపు;
		స్థితి 'L': type = VAR_LIST; ఆపు;
		స్థితి 'B': type = VAR_BLOB; ఆపు;
		స్థితి 'X': type = VAR_SPECIAL; ఆపు;
		స్థితి 'T': type = VAR_TUPLE; ఆపు;
	    }

	    tab = vim_strchr(tab, '\t');
	    ఒకవేళ (tab != ఖాళీ)
	    {
		tv.v_type = type;
		ఒకవేళ (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (పూర్ణాంకం)(tab - virp->vir_పంక్తి + 1), TRUE);
		లేకపోతే ఒకవేళ (type == VAR_FLOAT)
		    (శూన్య)string2float(tab + 1, &tv.vval.v_float, FALSE);
		లేకపోతే
		{
		    tv.vval.v_సంఖ్య = atol((అక్షరం *)tab + 1);
		    ఒకవేళ (type == VAR_SPECIAL && (tv.vval.v_సంఖ్య == VVAL_FALSE
					     || tv.vval.v_సంఖ్య == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		ఒకవేళ (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_సమాసం(tv.vval.v_string, ఖాళీ);

		    ఒకవేళ (etv == ఖాళీ)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    లేకపోతే
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		లేకపోతే ఒకవేళ (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    ఒకవేళ (blob == ఖాళీ)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    లేకపోతే
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_నమోదు);
		సమితి_var(virp->vir_పంక్తి + 1, &tv, FALSE);
		restore_funccal();

		ఒకవేళ (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		లేకపోతే ఒకవేళ (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    తిరిగి viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    స్థిర శూన్య
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    పూర్ణాంకం		todo;
    అక్షరం	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ఒకవేళ (కనుగొను_viminfo_parameter('!') == ఖాళీ)
	తిరిగి;

    fputs(_("\n# global variables:\n"), fp);

    todo = (పూర్ణాంకం)gvht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(gvht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ఒకవేళ (var_flavour(this_var->di_కీలకం) == VAR_FLAVOUR_VIMINFO)
	    {
		ఎంపిక (this_var->di_tv.v_type)
		{
		    స్థితి VAR_STRING:  s = "STR"; ఆపు;
		    స్థితి VAR_సంఖ్య:  s = "NUM"; ఆపు;
		    స్థితి VAR_FLOAT:   s = "FLO"; ఆపు;
		    స్థితి VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      పూర్ణాంకం	copyID = get_copyID();

			      s = "DIC";
			      ఒకవేళ (di != ఖాళీ && !సమితి_ref_లోపల_ht(
					 &di->dv_hashtab, copyID, ఖాళీ, ఖాళీ)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  కొనసాగు;
			      ఆపు;
			  }
		    స్థితి VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      పూర్ణాంకం	copyID = get_copyID();

			      s = "LIS";
			      ఒకవేళ (l != ఖాళీ && !సమితి_ref_లోపల_list_వస్తువులు(
						       l, copyID, ఖాళీ, ఖాళీ)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  కొనసాగు;
			      ఆపు;
			  }
		    స్థితి VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      పూర్ణాంకం	copyID = get_copyID();

			      s = "TUP";
			      ఒకవేళ (tuple != ఖాళీ && !సమితి_ref_లోపల_tuple_వస్తువులు(
					       tuple, copyID, ఖాళీ, ఖాళీ)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  కొనసాగు;
			      ఆపు;
			  }
		    స్థితి VAR_BLOB:    s = "BLO"; ఆపు;
		    స్థితి VAR_BOOL:    s = "XPL"; ఆపు;  // backwards compat.
		    స్థితి VAR_SPECIAL: s = "XPL"; ఆపు;

		    స్థితి VAR_UNKNOWN:
		    స్థితి VAR_ANY:
		    స్థితి VAR_VOID:
		    స్థితి VAR_FUNC:
		    స్థితి VAR_PARTIAL:
		    స్థితి VAR_పని:
		    స్థితి VAR_CHANNEL:
		    స్థితి VAR_INSTR:
		    స్థితి VAR_CLASS:
		    స్థితి VAR_OBJECT:
		    స్థితి VAR_TYPEALIAS:
				      కొనసాగు;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_కీలకం, s);
		ఒకవేళ (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((అక్షరం *)numbuf, "%ld",
					  (దీర్ఘ)this_var->di_tv.vval.v_సంఖ్య);
		    p = numbuf;
		    tofree = ఖాళీ;
		}
		లేకపోతే
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		ఒకవేళ (p != ఖాళీ)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#అయితే_ముగింపు // FEAT_EVAL

    స్థిర పూర్ణాంకం
read_viminfo_sub_string(vir_T *virp, పూర్ణాంకం force)
{
    ఒకవేళ (force || get_old_sub() == ఖాళీ)
	సమితి_old_sub(viminfo_readstring(virp, 1, TRUE));
    తిరిగి viminfo_readline(virp);
}

    స్థిర శూన్య
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    ఒకవేళ (get_viminfo_parameter('/') == 0 || old_sub == ఖాళీ)
	తిరిగి;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    స్థిర పూర్ణాంకం
read_viminfo_శోధన_నమూనా(vir_T *virp, పూర్ణాంకం force, పూర్ణాంకం writing)
{
    char_u	*lp;
    పూర్ణాంకం		idx = -1;
    పూర్ణాంకం		magic = FALSE;
    పూర్ణాంకం		no_scs = FALSE;
    పూర్ణాంకం		off_పంక్తి = FALSE;
    పూర్ణాంకం		off_చివర = 0;
    దీర్ఘ	off = 0;
    పూర్ణాంకం		setlast = FALSE;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    స్థిర పూర్ణాంకం	hlsearch_on = FALSE;
#అయితే_ముగింపు
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
    lp = virp->vir_పంక్తి;
    ఒకవేళ (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	ఒకవేళ (lp[1] == 'M')		// magic on
	    magic = TRUE;
	ఒకవేళ (lp[2] == 's')
	    no_scs = TRUE;
	ఒకవేళ (lp[3] == 'L')
	    off_పంక్తి = TRUE;
	ఒకవేళ (lp[4] == 'E')
	    off_చివర = శోధన_చివర;
	lp += 5;
	off = getdigits(&lp);
    }
    ఒకవేళ (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    ఒకవేళ (lp[0] == '/')
	idx = RE_శోధన;
    లేకపోతే ఒకవేళ (lp[0] == '&')
	idx = RE_SUBST;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    లేకపోతే ఒకవేళ (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    లేకపోతే ఒకవేళ (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#అయితే_ముగింపు
    ఒకవేళ (idx >= 0)
    {
	spat = get_spat(idx);
	ఒకవేళ (force || spat->pat == ఖాళీ)
	{
	    val = viminfo_readstring(virp, (పూర్ణాంకం)(lp - virp->vir_పంక్తి + 1),
									TRUE);
	    ఒకవేళ (val != ఖాళీ)
	    {
		ఒకవేళ (!writing)
		{
		    సమితి_చివరి_శోధన_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.పంక్తి = off_పంక్తి;
		    spat->off.చివర = off_చివర;
		    spat->off.off = off;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		    ఒకవేళ (setlast)
			సమితి_no_hlsearch(!hlsearch_on);
#అయితే_ముగింపు
		}
		vim_free(val);
	    }
	}
    }
    తిరిగి viminfo_readline(virp);
}

    స్థిర శూన్య
wvsp_one(
    FILE	*fp,	// file to write to
    పూర్ణాంకం		idx,	// spats[] index
    అక్షరం	*s,	// search pat
    పూర్ణాంకం		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    ఒకవేళ (spat->pat == ఖాళీ)
	తిరిగి;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.పంక్తి ? 'L' : 'l',	// line offset
	    spat->off.చివర  ? 'E' : 'e',	// offset from end
	    spat->off.off,			//స్థానభ్రంశం
	    get_spat_చివరి_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    స్థిర శూన్య
write_viminfo_శోధన_నమూనా(FILE *fp)
{
    ఒకవేళ (get_viminfo_parameter('/') == 0)
	తిరిగి;

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || కనుగొను_viminfo_parameter('h') != ఖాళీ) ? 'h' : 'H');
#అయితే_ముగింపు
    wvsp_one(fp, RE_శోధన, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

స్థిర yankreg_T *y_read_regs = ఖాళీ;

#నిర్వచించు REG_మునుపటి 1
#నిర్వచించు REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    స్థిర శూన్య
prepare_viminfo_registers(శూన్య)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, అంకె_REGISTERS);
}

    స్థిర శూన్య
ముగింపు_viminfo_registers(శూన్య)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		j;

    ఒకవేళ (y_read_regs == ఖాళీ)
	తిరిగి;

    కొరకు (i = 0; i < అంకె_REGISTERS; ++i)
	ఒకవేళ (y_read_regs[i].y_array != ఖాళీ)
	{
	    కొరకు (j = 0; j < y_read_regs[i].y_కొలత; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    స్థిర పూర్ణాంకం
read_viminfo_రిజిస్టరు(vir_T *virp, పూర్ణాంకం force)
{
    పూర్ణాంకం		eof;
    పూర్ణాంకం		do_it = TRUE;
    పూర్ణాంకం		కొలత;
    పూర్ణాంకం		హద్దు;
    పూర్ణాంకం		i;
    పూర్ణాంకం		సమితి_prev = FALSE;
    char_u	*str;
    string_T	*array = ఖాళీ;
    పూర్ణాంకం		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_వెడల్పు = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_పంక్తి + 1;

    // If the line starts with "" this is the y_previous register.
    ఒకవేళ (*str == '"')
    {
	సమితి_prev = TRUE;
	str++;
    }

    ఒకవేళ (!ASCII_ISALNUM(*str) && *str != '-')
    {
	ఒకవేళ (viminfo_error("E577: ", _(e_illegal_రిజిస్టరు_name), virp->vir_పంక్తి))
	    తిరిగి TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_రిజిస్టరు(*str++, FALSE);
    y_current_p = get_y_current();
    ఒకవేళ (!force && y_current_p->y_array != ఖాళీ)
	do_it = FALSE;

    ఒకవేళ (*str == '@')
    {
	// "x@: register x used for @@
	ఒకవేళ (force || get_execreg_lastc() == NUL)
	    సమితి_execreg_lastc(str[-1]);
    }

    కొలత = 0;
    హద్దు = 100;	// Optimized for registers containing <= 100 lines
    ఒకవేళ (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	ఒకవేళ (సమితి_prev)
	    సమితి_y_మునుపటి(y_current_p);
	array = ALLOC_MULT(string_T, హద్దు);
	str = skipwhite(skiptowhite(str));
	ఒకవేళ (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	లేకపోతే ఒకవేళ (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	లేకపోతే
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_వెడల్పు = getdigits(&str);
    }

    అయితే (!(eof = viminfo_readline(virp))
		    && (virp->vir_పంక్తి[0] == TAB || virp->vir_పంక్తి[0] == '<'))
    {
	ఒకవేళ (do_it)
	{
	    ఒకవేళ (కొలత == హద్దు)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)హద్దు * 2 * పరిమాణం(string_T));

		ఒకవేళ (new_array == ఖాళీ)
		{
		    do_it = FALSE;
		    ఆపు;
		}
		కొరకు (i = 0; i < హద్దు; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		హద్దు *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    ఒకవేళ (str != ఖాళీ)
	    {
		array[కొలత].string = str;
		array[కొలత].నిడివి = STRLEN(str);
		++కొలత;
	    }
	    లేకపోతే
		// error, don't store the result
		do_it = FALSE;
	}
    }

    ఒకవేళ (do_it)
    {
	// free y_array[]
	కొరకు (i = 0; i < y_current_p->y_కొలత; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_వెడల్పు = new_వెడల్పు;
	y_current_p->y_కొలత = కొలత;
	y_current_p->y_సమయం_సమితి = 0;
	ఒకవేళ (కొలత == 0)
	{
	    y_current_p->y_array = ఖాళీ;
	}
	లేకపోతే
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, కొలత);
	    కొరకు (i = 0; i < కొలత; i++)
	    {
		ఒకవేళ (y_current_p->y_array == ఖాళీ)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		లేకపోతే
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].నిడివి = array[i].నిడివి;
		}
	    }
	}
    }
    లేకపోతే
    {
	// Free array[] if it was filled.
	కొరకు (i = 0; i < కొలత; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    తిరిగి eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    స్థిర శూన్య
handle_viminfo_రిజిస్టరు(garray_T *విలువలు, పూర్ణాంకం force)
{
    bval_T	*vp = (bval_T *)విలువలు->ga_దత్తాంశం;
    పూర్ణాంకం		flags;
    పూర్ణాంకం		name;
    పూర్ణాంకం		type;
    పూర్ణాంకం		linecount;
    పూర్ణాంకం		వెడల్పు;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    పూర్ణాంకం		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    ఒకవేళ (విలువలు->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	తిరిగి;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    ఒకవేళ (name < 0 || name >= అంకె_REGISTERS)
	తిరిగి;
    type = vp[2].bv_nr;
    ఒకవేళ (type != MCHAR && type != MLINE && type != MBLOCK)
	తిరిగి;
    linecount = vp[3].bv_nr;
    ఒకవేళ (విలువలు->ga_len < 6 + linecount)
	తిరిగి;
    వెడల్పు = vp[4].bv_nr;
    ఒకవేళ (వెడల్పు < 0)
	తిరిగి;

    ఒకవేళ (y_read_regs != ఖాళీ)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    లేకపోతే
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    ఒకవేళ (y_ptr->y_array != ఖాళీ && !force
			 && (timestamp == 0 || y_ptr->y_సమయం_సమితి > timestamp))
	తిరిగి;

    ఒకవేళ (y_ptr->y_array != ఖాళీ)
	కొరకు (i = 0; i < y_ptr->y_కొలత; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    ఒకవేళ (y_read_regs == ఖాళీ)
    {
	ఒకవేళ (flags & REG_మునుపటి)
	    సమితి_y_మునుపటి(y_ptr);
	ఒకవేళ ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    సమితి_execreg_lastc(get_రిజిస్టరు_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_వెడల్పు = వెడల్పు;
    y_ptr->y_కొలత = linecount;
    y_ptr->y_సమయం_సమితి = timestamp;
    ఒకవేళ (linecount == 0)
    {
	y_ptr->y_array = ఖాళీ;
	తిరిగి;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    ఒకవేళ (y_ptr->y_array == ఖాళీ)
    {
	y_ptr->y_కొలత = 0; // ensure object state is consistent
	తిరిగి;
    }
    కొరకు (i = 0; i < linecount; i++)
    {
	ఒకవేళ (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].నిడివి = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = ఖాళీ;
	}
	లేకపోతే ఒకవేళ (vp[i + 6].bv_type != BVAL_STRING)
	{
	    విడుదల(y_ptr->y_array);
	    y_ptr->y_array = ఖాళీ;
	}
	లేకపోతే
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].నిడివి = vp[i + 6].bv_len;
	}
    }
}

    స్థిర శూన్య
write_viminfo_registers(FILE *fp)
{
    పూర్ణాంకం		i, j;
    char_u	*type;
    char_u	c;
    పూర్ణాంకం		అంకె_పంక్తులు;
    పూర్ణాంకం		max_అంకె_పంక్తులు;
    పూర్ణాంకం		max_kbyte;
    దీర్ఘ	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_అంకె_పంక్తులు = get_viminfo_parameter('<');
    ఒకవేళ (max_అంకె_పంక్తులు < 0)
	max_అంకె_పంక్తులు = get_viminfo_parameter('"');
    ఒకవేళ (max_అంకె_పంక్తులు == 0)
	తిరిగి;
    max_kbyte = get_viminfo_parameter('s');
    ఒకవేళ (max_kbyte == 0)
	తిరిగి;

    కొరకు (i = 0; i < అంకె_REGISTERS; i++)
    {
#ఒకవేళ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	ఒకవేళ (i == నక్షత్రం_రిజిస్టరు || i == PLUS_రిజిస్టరు || i == REAL_PLUS_రిజిస్టరు)
	    కొనసాగు;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DND
	// Neither do we want the '~' register
	ఒకవేళ (i == TILDE_రిజిస్టరు)
	    కొనసాగు;
#అయితే_ముగింపు
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	ఒకవేళ (y_read_regs != ఖాళీ
		&& y_read_regs[i].y_array != ఖాళీ
		&& (y_regs_p[i].y_array == ఖాళీ ||
			    y_read_regs[i].y_సమయం_సమితి > y_regs_p[i].y_సమయం_సమితి))
	    y_ptr = &y_read_regs[i];
	లేకపోతే ఒకవేళ (y_regs_p[i].y_array == ఖాళీ)
	    కొనసాగు;
	లేకపోతే
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	అంకె_పంక్తులు = y_ptr->y_కొలత;
	ఒకవేళ (అంకె_పంక్తులు == 0
		|| (అంకె_పంక్తులు == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    కొనసాగు;

	ఒకవేళ (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    కొరకు (j = 0; j < అంకె_పంక్తులు; j++)
		len += (దీర్ఘ)y_ptr->y_array[j].నిడివి + 1L;
	    ఒకవేళ (len > (దీర్ఘ)max_kbyte * 1024L)
		కొనసాగు;
	}

	ఎంపిక (y_ptr->y_type)
	{
	    స్థితి MLINE:
		type = (char_u *)"LINE";
		ఆపు;
	    స్థితి MCHAR:
		type = (char_u *)"CHAR";
		ఆపు;
	    స్థితి MBLOCK:
		type = (char_u *)"BLOCK";
		ఆపు;
	    అప్రమేయ:
		semsg(_(e_unknown_రిజిస్టరు_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		ఆపు;
	}
	ఒకవేళ (get_y_మునుపటి() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_రిజిస్టరు_name(i);
	fprintf(fp, "\"%c", c);
	ఒకవేళ (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (పూర్ణాంకం)y_ptr->y_వెడల్పు);

	// If max_num_lines < 0, then we save ALL the lines in the register
	ఒకవేళ (max_అంకె_పంక్తులు > 0 && అంకె_పంక్తులు > max_అంకె_పంక్తులు)
	    అంకె_పంక్తులు = max_అంకె_పంక్తులు;
	కొరకు (j = 0; j < అంకె_పంక్తులు; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    పూర్ణాంకం	    flags = 0;
	    పూర్ణాంకం	    మిగిలిన;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    ఒకవేళ (get_y_మునుపటి() == &y_regs_p[i])
		flags |= REG_మునుపటి;
	    ఒకవేళ (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_రిజిస్టరు, flags,
		    i, y_ptr->y_type, అంకె_పంక్తులు, (పూర్ణాంకం)y_ptr->y_వెడల్పు,
		    (దీర్ఘ)y_ptr->y_సమయం_సమితి);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    మిగిలిన = LSIZE - 71;
	    కొరకు (j = 0; j < అంకె_పంక్తులు; j++)
	    {
		putc(',', fp);
		--మిగిలిన;
		మిగిలిన = barline_writestring(fp, y_ptr->y_array[j].string,
								   మిగిలిన);
	    }
	    putc('\n', fp);
	}
    }
}

/*
 * Functions relating to reading/writing marks from viminfo
 */

స్థిర xfmark_T *vi_namedfm = ఖాళీ;
స్థిర xfmark_T *vi_jumplist = ఖాళీ;
స్థిర పూర్ణాంకం vi_jumplist_len = 0;

    స్థిర శూన్య
write_one_mark(FILE *fp_బహిర్గతం, పూర్ణాంకం c, pos_T *pos)
{
    ఒకవేళ (pos->lnum != 0)
	fprintf(fp_బహిర్గతం, "\t%c\t%ld\t%d\n", c, (దీర్ఘ)pos->lnum, (పూర్ణాంకం)pos->నిలువు);
}

    స్థిర శూన్య
write_బఫరు_marks(buf_T *buf, FILE *fp_బహిర్గతం)
{
    పూర్ణాంకం		i;
    pos_T	pos;

    home_replace(ఖాళీ, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_బహిర్గతం, "\n> ");
    viminfo_writestring(fp_బహిర్గతం, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_చివరి_వాడిన;
    pos.నిలువు = 0;
    write_one_mark(fp_బహిర్గతం, '*', &pos);

    write_one_mark(fp_బహిర్గతం, '"', &buf->b_చివరి_cursor);
    write_one_mark(fp_బహిర్గతం, '^', &buf->b_చివరి_చొప్పించు);
    write_one_mark(fp_బహిర్గతం, '.', &buf->b_చివరి_change);
    // changelist positions are stored oldest first
    కొరకు (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	ఒకవేళ (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_బహిర్గతం, '+', &buf->b_changelist[i]);
    }
    కొరకు (i = 0; i < NMARKS; i++)
	write_one_mark(fp_బహిర్గతం, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    స్థిర పూర్ణాంకం
skip_for_viminfo(buf_T *buf)
{
    తిరిగి bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    స్థిర శూన్య
write_viminfo_marks(FILE *fp_బహిర్గతం, garray_T *buflist)
{
    buf_T	*buf;
    పూర్ణాంకం		is_mark_సమితి;
    పూర్ణాంకం		i;
    win_T	*win;
    tabpage_T	*tp;

    // Set b_last_cursor for the all buffers that have a window.
    FOR_ALL_TAB_WINDOWS(tp, win)
	సమితి_చివరి_cursor(win);

    fputs(_("\n# History of marks within files (newest to oldest):\n"), fp_బహిర్గతం);
    FOR_ALL_BUFFERS(buf)
    {
	// Only write something if buffer has been loaded and at least one
	// mark is set.
	ఒకవేళ (buf->b_marks_read)
	{
	    ఒకవేళ (buf->b_చివరి_cursor.lnum != 0)
		is_mark_సమితి = TRUE;
	    లేకపోతే
	    {
		is_mark_సమితి = FALSE;
		కొరకు (i = 0; i < NMARKS; i++)
		    ఒకవేళ (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_సమితి = TRUE;
			ఆపు;
		    }
	    }
	    ఒకవేళ (is_mark_సమితి && buf->b_ffname != ఖాళీ
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		ఒకవేళ (buflist == ఖాళీ)
		    write_బఫరు_marks(buf, fp_బహిర్గతం);
		లేకపోతే ఒకవేళ (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_దత్తాంశం)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    స్థిర శూన్య
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    పూర్ణాంకం		c1,
    పూర్ణాంకం		c2)
{
    char_u	*name;

    ఒకవేళ (fm->fmark.mark.lnum == 0)	// not set
	తిరిగి;

    ఒకవేళ (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    లేకపోతే
	name = fm->fname;		// use name from .viminfo
    ఒకవేళ (name != ఖాళీ && *name != NUL)
    {
	fprintf(fp, "%c%c  %ld  %ld  ", c1, c2, (దీర్ఘ)fm->fmark.mark.lnum,
						    (దీర్ఘ)fm->fmark.mark.నిలువు);
	viminfo_writestring(fp, name);

	// Barline: |{bartype},{name},{lnum},{col},{timestamp},{filename}
	// size up to filename: 8 + 3 * 20
	fprintf(fp, "|%d,%d,%ld,%ld,%ld,", BARTYPE_MARK, c2,
		(దీర్ఘ)fm->fmark.mark.lnum, (దీర్ఘ)fm->fmark.mark.నిలువు,
		(దీర్ఘ)fm->సమయం_సమితి);
	barline_writestring(fp, name, LSIZE - 70);
	putc('\n', fp);
    }

    ఒకవేళ (fm->fmark.fnum != 0)
	vim_free(name);
}

    స్థిర శూన్య
write_viminfo_filemarks(FILE *fp)
{
    పూర్ణాంకం		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    పూర్ణాంకం		vi_idx;
    పూర్ణాంకం		idx;

    ఒకవేళ (get_viminfo_parameter('f') == 0)
	తిరిగి;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    కొరకు (i = 0; i < NMARKS; i++)
    {
	ఒకవేళ (vi_namedfm != ఖాళీ
			&& (vi_namedfm[i].సమయం_సమితి > namedfm_p[i].సమయం_సమితి))
	    fm = &vi_namedfm[i];
	లేకపోతే
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    ఒకవేళ (curbuf->b_ffname != ఖాళీ && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	కొరకు (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    ఒకవేళ (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == ఖాళీ
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != ఖాళీ
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		ఆపు;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	కొరకు ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = ఖాళీ;
	namedfm_p[NMARKS].సమయం_సమితి = vim_సమయం();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    కొరకు (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != ఖాళీ ? &vi_namedfm[vi_idx] : ఖాళీ;

	ఒకవేళ (vi_fm != ఖాళీ
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->సమయం_సమితి > namedfm_p[idx].సమయం_సమితి
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	లేకపోతే
	{
	    fm = &namedfm_p[idx++];
	    ఒకవేళ (vi_fm != ఖాళీ
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->సమయం_సమితి == fm->సమయం_సమితి
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != ఖాళీ
			  && fm->fname != ఖాళీ
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
    కొరకు (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : ఖాళీ;
	vi_fm = (vi_jumplist != ఖాళీ && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : ఖాళీ;
	ఒకవేళ (fm == ఖాళీ && vi_fm == ఖాళీ)
	    ఆపు;
	ఒకవేళ (fm == ఖాళీ || (vi_fm != ఖాళీ && fm->సమయం_సమితి < vi_fm->సమయం_సమితి))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	లేకపోతే
	    --idx;
	ఒకవేళ (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != ఖాళీ
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    పూర్ణాంకం
buf_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    ఒకవేళ (buf1->b_చివరి_వాడిన == buf2->b_చివరి_వాడిన)
	తిరిగి 0;
    తిరిగి buf1->b_చివరి_వాడిన > buf2->b_చివరి_వాడిన ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    స్థిర శూన్య
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_బహిర్గతం,
    garray_T	*buflist,
    పూర్ణాంకం		eof,
    పూర్ణాంకం		flags)
{
    char_u	*పంక్తి = virp->vir_పంక్తి;
    buf_T	*buf;
    పూర్ణాంకం		అంకె_marked_files;
    పూర్ణాంకం		ఎక్కించు_marks;
    పూర్ణాంకం		copy_marks_బహిర్గతం;
    char_u	*str;
    పూర్ణాంకం		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#నిర్వచితమైతే FEAT_EVAL
    list_T	*list = ఖాళీ;
#అయితే_ముగింపు
    పూర్ణాంకం		లెక్క = 0;
    పూర్ణాంకం		buflist_వాడిన = 0;
    buf_T	*buflist_buf = ఖాళీ;

    ఒకవేళ ((name_buf = alloc(LSIZE)) == ఖాళీ)
	తిరిగి;
    *name_buf = NUL;

    ఒకవేళ (fp_బహిర్గతం != ఖాళీ && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_దత్తాంశం, (size_t)buflist->ga_len,
						పరిమాణం(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_దత్తాంశం)[0];
    }

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (fp_బహిర్గతం == ఖాళీ && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	ఒకవేళ (list != ఖాళీ)
	    సమితి_vim_var_list(VV_OLDFILES, list);
    }
#అయితే_ముగింపు

    అంకె_marked_files = get_viminfo_parameter('\'');
    అయితే (!eof && (లెక్క < అంకె_marked_files || fp_బహిర్గతం == ఖాళీ))
    {
	ఒకవేళ (పంక్తి[0] != '>')
	{
	    ఒకవేళ (పంక్తి[0] != '\n' && పంక్తి[0] != '\r' && పంక్తి[0] != '#')
	    {
		ఒకవేళ (viminfo_error("E576: ", _(e_nonr_missing_gt), పంక్తి))
		    ఆపు;	// too many errors, return now
	    }
	    eof = vim_fgets(పంక్తి, LSIZE, virp->vir_fd);
	    కొనసాగు;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(పంక్తి + 1);
	str = viminfo_readstring(virp, (పూర్ణాంకం)(str - virp->vir_పంక్తి), FALSE);
	ఒకవేళ (str == ఖాళీ)
	    కొనసాగు;
	p = str + STRLEN(str);
	అయితే (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	ఒకవేళ (*p)
	    p++;
	*p = NUL;

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (list != ఖాళీ)
	    list_append_string(list, str, -1);
#అయితే_ముగింపు

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	ఎక్కించు_marks = copy_marks_బహిర్గతం = FALSE;
	ఒకవేళ (fp_బహిర్గతం == ఖాళీ)
	{
	    ఒకవేళ ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != ఖాళీ)
	    {
		ఒకవేళ (*name_buf == NUL)	    // only need to do this once
		    home_replace(ఖాళీ, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		ఒకవేళ (fnamecmp(str, name_buf) == 0)
		    ఎక్కించు_marks = TRUE;
	    }
	}
	లేకపోతే // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		ఒకవేళ (buf->b_ffname != ఖాళీ)
		{
		    home_replace(ఖాళీ, buf->b_ffname, name_buf, LSIZE, TRUE);
		    ఒకవేళ (fnamecmp(str, name_buf) == 0)
			ఆపు;
		}

	    // Copy marks if the buffer has not been loaded.
	    ఒకవేళ (buf == ఖాళీ || !buf->b_marks_read)
	    {
		పూర్ణాంకం	did_read_పంక్తి = FALSE;

		ఒకవేళ (buflist_buf != ఖాళీ)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    ఒకవేళ (!viminfo_readline(virp) && పంక్తి[0] == TAB)
		    {
			did_read_పంక్తి = TRUE;
			ఒకవేళ (పంక్తి[1] == '*')
			{
			    దీర్ఘ	ltime;

			    అక్షరశ్రేణి_చదువు((అక్షరం *)పంక్తి + 2, "%ld ", &ltime);
			    అయితే ((సమయం_T)ltime < buflist_buf->b_చివరి_వాడిన)
			    {
				write_బఫరు_marks(buflist_buf, fp_బహిర్గతం);
				ఒకవేళ (++లెక్క >= అంకె_marked_files)
				    ఆపు;
				ఒకవేళ (++buflist_వాడిన == buflist->ga_len)
				{
				    buflist_buf = ఖాళీ;
				    ఆపు;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_దత్తాంశం)[buflist_వాడిన];
			    }
			}
			లేకపోతే
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    అయితే (లెక్క < అంకె_marked_files
					    && buflist_వాడిన < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_దత్తాంశం)
							     [buflist_వాడిన++];
				write_బఫరు_marks(buflist_buf, fp_బహిర్గతం);
				++లెక్క;
			    }
			    buflist_buf = ఖాళీ;
			}

			ఒకవేళ (లెక్క >= అంకె_marked_files)
			{
			    vim_free(str);
			    ఆపు;
			}
		    }
		}

		fputs("\n> ", fp_బహిర్గతం);
		viminfo_writestring(fp_బహిర్గతం, str);
		ఒకవేళ (did_read_పంక్తి)
		    fputs((అక్షరం *)పంక్తి, fp_బహిర్గతం);

		లెక్క++;
		copy_marks_బహిర్గతం = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	అయితే (!(eof = viminfo_readline(virp)) && పంక్తి[0] == TAB)
	{
	    ఒకవేళ (ఎక్కించు_marks)
	    {
		ఒకవేళ (పంక్తి[1] != NUL)
		{
		    గుర్తులేని u;

		    అక్షరశ్రేణి_చదువు((అక్షరం *)పంక్తి + 2, "%ld %u", &pos.lnum, &u);
		    pos.నిలువు = u;
		    ఎంపిక (పంక్తి[1])
		    {
			స్థితి '"': curbuf->b_చివరి_cursor = pos; ఆపు;
			స్థితి '^': curbuf->b_చివరి_చొప్పించు = pos; ఆపు;
			స్థితి '.': curbuf->b_చివరి_change = pos; ఆపు;
			స్థితి '+':
				  // changelist positions are stored oldest
				  //మొదటి
				  ఒకవేళ (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    పరిమాణం(pos_T) * (JUMPLISTSIZE - 1));
				  లేకపోతే
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  ఆపు;

				  // Using the line number for the last-used
				  // timestamp.
			స్థితి '*': curbuf->b_చివరి_వాడిన = pos.lnum; ఆపు;

			అప్రమేయ:  ఒకవేళ ((i = పంక్తి[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    లేకపోతే ఒకవేళ (copy_marks_బహిర్గతం)
		fputs((అక్షరం *)పంక్తి, fp_బహిర్గతం);
	}

	ఒకవేళ (ఎక్కించు_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		ఒకవేళ (wp->w_బఫరు == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    ఒకవేళ (flags & VIF_ONLY_CURBUF)
		ఆపు;
	}
    }

    ఒకవేళ (fp_బహిర్గతం != ఖాళీ)
	// Write any remaining entries from buflist.
	అయితే (లెక్క < అంకె_marked_files && buflist_వాడిన < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_దత్తాంశం)[buflist_వాడిన++];
	    write_బఫరు_marks(buflist_buf, fp_బహిర్గతం);
	    ++లెక్క;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    శూన్య
check_marks_read(శూన్య)
{
    ఒకవేళ (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != ఖాళీ)
	read_viminfo(ఖాళీ, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    స్థిర పూర్ణాంకం
read_viminfo_filemark(vir_T *virp, పూర్ణాంకం force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    పూర్ణాంకం		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_పంక్తి + 1;
    ఒకవేళ (*str <= 127
	    && ((*virp->vir_పంక్తి == '\''
				       && (VIM_ISDIGIT(*str) || సురక్షిత_isupper(*str)))
	     || (*virp->vir_పంక్తి == '-' && *str == '\'')))
    {
	ఒకవేళ (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    ఒకవేళ (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = ఖాళీ;
	    లేకపోతే
	    {
		కొరకు (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = ఖాళీ;
	    }
	}
	లేకపోతే ఒకవేళ (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	లేకపోతే
	    fm = &namedfm_p[*str - 'A'];
	ఒకవేళ (fm != ఖాళీ && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.నిలువు = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (పూర్ణాంకం)(str - virp->vir_పంక్తి),
								       FALSE);
	    fm->సమయం_సమితి = 0;
	}
    }
    తిరిగి vim_fgets(virp->vir_పంక్తి, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    స్థిర శూన్య
prepare_viminfo_marks(శూన్య)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    స్థిర శూన్య
ముగింపు_viminfo_marks(శూన్య)
{
    పూర్ణాంకం		i;

    ఒకవేళ (vi_namedfm != ఖాళీ)
    {
	కొరకు (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    ఒకవేళ (vi_jumplist != ఖాళీ)
    {
	కొరకు (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    స్థిర శూన్య
handle_viminfo_mark(garray_T *విలువలు, పూర్ణాంకం force)
{
    bval_T	*vp = (bval_T *)విలువలు->ga_దత్తాంశం;
    పూర్ణాంకం		name;
    linenr_T	lnum;
    colnr_T	నిలువు;
    time_t	timestamp;
    xfmark_T	*fm = ఖాళీ;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    ఒకవేళ (విలువలు->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	తిరిగి;

    name = vp[0].bv_nr;
    ఒకవేళ (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	తిరిగి;
    lnum = vp[1].bv_nr;
    నిలువు = vp[2].bv_nr;
    ఒకవేళ (lnum <= 0 || నిలువు < 0)
	తిరిగి;
    timestamp = (time_t)vp[3].bv_nr;

    ఒకవేళ (name == '\'')
    {
	ఒకవేళ (vi_jumplist != ఖాళీ)
	{
	    ఒకవేళ (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	లేకపోతే
	{
	    పూర్ణాంకం idx;
	    పూర్ణాంకం i;

	    // If we have a timestamp insert it in the right place.
	    ఒకవేళ (timestamp != 0)
	    {
		కొరకు (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    ఒకవేళ (curwin->w_jumplist[idx].సమయం_సమితి < timestamp)
		    {
			++idx;
			ఆపు;
		    }
		// idx cannot be zero now
		ఒకవేళ (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    లేకపోతే ఒకవేళ (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    లేకపోతే
		idx = -1;

	    ఒకవేళ (idx >= 0)
	    {
		ఒకవేళ (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    కొరకు (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		లేకపోతే
		{
		    // Move newer entries forward.
		    కొరకు (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = ఖాళీ;
		fm->సమయం_సమితి = 0;
	    }
	}
    }
    లేకపోతే
    {
	పూర్ణాంకం		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	ఒకవేళ (VIM_ISDIGIT(name))
	{
	    ఒకవేళ (vi_namedfm != ఖాళీ)
		idx = name - '0' + NMARKS;
	    లేకపోతే
	    {
		పూర్ణాంకం i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		కొరకు (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    ఒకవేళ (namedfm_p[idx].సమయం_సమితి < timestamp)
			ఆపు;
		ఒకవేళ (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    తిరిగి;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		కొరకు ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = ఖాళీ;
	    }
	}
	లేకపోతే
	    idx = name - 'A';
	ఒకవేళ (vi_namedfm != ఖాళీ)
	    fm = &vi_namedfm[idx];
	లేకపోతే
	    fm = &namedfm_p[idx];
    }

    ఒకవేళ (fm != ఖాళీ)
    {
	ఒకవేళ (vi_namedfm != ఖాళీ || fm->fmark.mark.lnum == 0
					  || fm->సమయం_సమితి < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.నిలువు = నిలువు;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    ఒకవేళ (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = ఖాళీ;
	    }
	    లేకపోతే
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->సమయం_సమితి = timestamp;
	}
    }
}

    స్థిర పూర్ణాంకం
read_viminfo_barline(vir_T *virp, పూర్ణాంకం got_encoding, పూర్ణాంకం force, పూర్ణాంకం writing)
{
    char_u	*p = virp->vir_పంక్తి + 1;
    పూర్ణాంకం		bartype;
    garray_T	విలువలు;
    bval_T	*vp;
    పూర్ణాంకం		i;
    పూర్ణాంకం		read_తదుపరి = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    ఒకవేళ (*p == '<')
    {
	// Continuation line of an unrecognized item.
	ఒకవేళ (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_పంక్తి);
    }
    లేకపోతే
    {
	ga_init2(&విలువలు, పరిమాణం(bval_T), 20);
	bartype = getdigits(&p);
	ఎంపిక (bartype)
	{
	    స్థితి BARTYPE_సంస్కరణ:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		ఒకవేళ (!got_encoding)
		{
		    read_తదుపరి = barline_parse(virp, p, &విలువలు);
		    vp = (bval_T *)విలువలు.ga_దత్తాంశం;
		    ఒకవేళ (విలువలు.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_సంస్కరణ = vp->bv_nr;
		}
		ఆపు;

	    స్థితి BARTYPE_HISTORY:
		read_తదుపరి = barline_parse(virp, p, &విలువలు);
		handle_viminfo_history(&విలువలు, writing);
		ఆపు;

	    స్థితి BARTYPE_రిజిస్టరు:
		read_తదుపరి = barline_parse(virp, p, &విలువలు);
		handle_viminfo_రిజిస్టరు(&విలువలు, force);
		ఆపు;

	    స్థితి BARTYPE_MARK:
		read_తదుపరి = barline_parse(virp, p, &విలువలు);
		handle_viminfo_mark(&విలువలు, force);
		ఆపు;

	    అప్రమేయ:
		// copy unrecognized line (for future use)
		ఒకవేళ (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_పంక్తి);
	}
	కొరకు (i = 0; i < విలువలు.ga_len; ++i)
	{
	    vp = (bval_T *)విలువలు.ga_దత్తాంశం + i;
	    ఒకవేళ (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&విలువలు);
    }

    ఒకవేళ (read_తదుపరి)
	తిరిగి viminfo_readline(virp);
    తిరిగి FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    స్థిర పూర్ణాంకం
read_viminfo_పైకి_గమ్యం_marks(
    vir_T	*virp,
    పూర్ణాంకం		forceit,
    పూర్ణాంకం		writing)
{
    పూర్ణాంకం		eof;
    buf_T	*buf;
    పూర్ణాంకం		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    అయితే (!eof && virp->vir_పంక్తి[0] != '>')
    {
	ఎంపిక (virp->vir_పంక్తి[0])
	{
		// Characters reserved for future expansion, ignored now
	    స్థితి '+': // "+40 /path/dir file", for running vim without args
	    స్థితి '^': // to be defined
	    స్థితి '<': // long line - ignored
		// A comment or empty line.
	    స్థితి NUL:
	    స్థితి '\r':
	    స్థితి '\n':
	    స్థితి '#':
		eof = viminfo_readline(virp);
		ఆపు;
	    స్థితి '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		ఆపు;
	    స్థితి '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		ఆపు;
	    స్థితి '!': // global variable
#నిర్వచితమైతే FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#లేకపోతే
		eof = viminfo_readline(virp);
#అయితే_ముగింపు
		ఆపు;
	    స్థితి '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		ఆపు;
	    స్థితి '"':
		// When registers are in bar lines skip the old style register
		// lines.
		ఒకవేళ (virp->vir_సంస్కరణ < VIMINFO_సంస్కరణ_WITH_REGISTERS)
		    eof = read_viminfo_రిజిస్టరు(virp, forceit);
		లేకపోతే
		    చేయి
		    {
			eof = viminfo_readline(virp);
		    } అయితే (!eof && (virp->vir_పంక్తి[0] == TAB
						|| virp->vir_పంక్తి[0] == '<'));
		ఆపు;
	    స్థితి '/':	    // Search string
	    స్థితి '&':	    // Substitute search string
	    స్థితి '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_శోధన_నమూనా(virp, forceit, writing);
		ఆపు;
	    స్థితి '$':
		eof = read_viminfo_sub_string(virp, forceit);
		ఆపు;
	    స్థితి ':':
	    స్థితి '?':
	    స్థితి '=':
	    స్థితి '@':
		// When history is in bar lines skip the old style history
		// lines.
		ఒకవేళ (virp->vir_సంస్కరణ < VIMINFO_సంస్కరణ_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		లేకపోతే
		    eof = viminfo_readline(virp);
		ఆపు;
	    స్థితి '-':
	    స్థితి '\'':
		// When file marks are in bar lines skip the old style lines.
		ఒకవేళ (virp->vir_సంస్కరణ < VIMINFO_సంస్కరణ_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		లేకపోతే
		    eof = viminfo_readline(virp);
		ఆపు;
	    అప్రమేయ:
		ఒకవేళ (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_పంక్తి))
		    eof = TRUE;
		లేకపోతే
		    eof = viminfo_readline(virp);
		ఆపు;
	}
    }

    // Finish reading history items.
    ఒకవేళ (!writing)
	ముగింపు_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_పేర్లు(buf);

    తిరిగి eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    స్థిర శూన్య
do_viminfo(FILE *fp_లోపల, FILE *fp_బహిర్గతం, పూర్ణాంకం flags)
{
    పూర్ణాంకం		eof = FALSE;
    vir_T	vir;
    పూర్ణాంకం		విలీనం = FALSE;
    పూర్ణాంకం		do_copy_marks = FALSE;
    garray_T	buflist;

    ఒకవేళ ((vir.vir_పంక్తి = alloc(LSIZE)) == ఖాళీ)
	తిరిగి;
    vir.vir_fd = fp_లోపల;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, పరిమాణం(char_u *), 100);
    vir.vir_సంస్కరణ = -1;

    ఒకవేళ (fp_లోపల != ఖాళీ)
    {
	ఒకవేళ (flags & VIF_WANT_INFO)
	{
	    ఒకవేళ (fp_బహిర్గతం != ఖాళీ)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_పైకి_గమ్యం_marks(&vir,
					 flags & VIF_FORCEIT, fp_బహిర్గతం != ఖాళీ);
	    విలీనం = TRUE;
	}
	లేకపోతే ఒకవేళ (flags != 0)
	    // Skip info, find start of marks
	    అయితే (!(eof = viminfo_readline(&vir))
		    && vir.vir_పంక్తి[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    ఒకవేళ (fp_బహిర్గతం != ఖాళీ)
    {
	// Write the info:
	fprintf(fp_బహిర్గతం, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_సంస్కరణ_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_బహిర్గతం);
	write_viminfo_సంస్కరణ(fp_బహిర్గతం);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_బహిర్గతం);
	fprintf(fp_బహిర్గతం, "*encoding=%s\n\n", p_enc);
	write_viminfo_శోధన_నమూనా(fp_బహిర్గతం);
	write_viminfo_sub_string(fp_బహిర్గతం);
	write_viminfo_history(fp_బహిర్గతం, విలీనం);
	write_viminfo_registers(fp_బహిర్గతం);
	ముగింపు_viminfo_registers();
#నిర్వచితమైతే FEAT_EVAL
	write_viminfo_varlist(fp_బహిర్గతం);
#అయితే_ముగింపు
	write_viminfo_filemarks(fp_బహిర్గతం);
	ముగింపు_viminfo_marks();
	write_viminfo_bufferlist(fp_బహిర్గతం);
	write_viminfo_barlines(&vir, fp_బహిర్గతం);

	ఒకవేళ (do_copy_marks)
	    ga_init2(&buflist, పరిమాణం(buf_T *), 50);
	write_viminfo_marks(fp_బహిర్గతం, do_copy_marks ? &buflist : ఖాళీ);
    }

    ఒకవేళ (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_బహిర్గతం, &buflist, eof, flags);
	ఒకవేళ (fp_బహిర్గతం != ఖాళీ)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_పంక్తి);
    ఒకవేళ (vir.vir_conv.vc_type != CONV_NONE)
	convert_అమరిక(&vir.vir_conv, ఖాళీ, ఖాళీ);
    ga_clear_అక్షరశ్రేణులు(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    పూర్ణాంకం
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    పూర్ణాంకం		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    ఒకవేళ (no_viminfo())
	తిరిగి FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి FAIL;
    fp = mch_fopen((అక్షరం *)fname, READBIN);

    ఒకవేళ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == ఖాళీ ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    ఒకవేళ (fp == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_బహుళకం))
    {
	fclose(fp);
	తిరిగి FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, ఖాళీ, flags);

    fclose(fp);
    తిరిగి OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    శూన్య
write_viminfo(char_u *file, పూర్ణాంకం forceit)
{
    char_u	*fname;
    FILE	*fp_లోపల = ఖాళీ;	// input viminfo file, if any
    FILE	*fp_బహిర్గతం = ఖాళీ;	// output viminfo file
    char_u	*tempname = ఖాళీ;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#ఒకవేళ defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#అయితే_ముగింపు
#నిర్వచితమైతే UNIX
    పూర్ణాంకం		shortname = FALSE;	// use 8.3 file name
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    పూర్ణాంకం		hidden = FALSE;
#అయితే_ముగింపు

    ఒకవేళ (no_viminfo())
	తిరిగి;

    fname = viminfo_filename(file);	// may set to default if NULL
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి;

    fp_లోపల = mch_fopen((అక్షరం *)fname, READBIN);
    ఒకవేళ (fp_లోపల == ఖాళీ)
    {
	పూర్ణాంకం fd;

	// if it does exist, but we can't read it, don't try writing
	ఒకవేళ (mch_stat((అక్షరం *)fname, &st_new) == 0)
	    వెళ్ళు చివర;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((అక్షరం *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	ఒకవేళ (fd < 0)
	    వెళ్ళు చివర;
	fp_బహిర్గతం = fdopen(fd, WRITEBIN);
    }
    లేకపోతే
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	ఒకవేళ (mch_fstat(fileno(fp_లోపల), &st_old) < 0
		|| S_ISDIR(st_old.st_బహుళకం)
#నిర్వచితమైతే UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != మూలం_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_బహుళకం & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_బహుళకం & 0020)
				    : (st_old.st_బహుళకం & 0002))))
#అయితే_ముగింపు
		)
	{
	    పూర్ణాంకం	tt = సందేశం_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    సందేశం_didany = tt;
	    fclose(fp_లోపల);
	    వెళ్ళు చివర;
	}
#నిర్వచితమైతే MSWIN
	// Get the file attributes of the existing viminfo file.
	hidden = mch_ishidden(fname);
#అయితే_ముగింపు

	// Make tempname, find one that does not exist yet.
	// Beware of a race condition: If someone logs out and all Vim
	// instances exit at the same time a temp file might be created between
	// stat() and open().  Use mch_open() with O_EXCL to avoid that.
	// May try twice: Once normal and once with shortname set, just in
	// case somebody puts his viminfo file in an 8.3 filesystem.
	కొరకు (;;)
	{
	    పూర్ణాంకం		తదుపరి_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#నిర్వచితమైతే UNIX
				    shortname,
#లేకపోతే
				    FALSE,
#అయితే_ముగింపు
				    fname,
#నిర్వచితమైతే VMS
				    (char_u *)"-tmp",
#లేకపోతే
				    (char_u *)".tmp",
#అయితే_ముగింపు
				    FALSE);
	    ఒకవేళ (tempname == ఖాళీ)		// out of memory
		ఆపు;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    ఒకవేళ (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    కొరకు (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		ఒకవేళ (mch_stat((అక్షరం *)tempname, &st_new) == 0)
		{
#నిర్వచితమైతే UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    ఒకవేళ (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			ఆపు;
		    }
#అయితే_ముగింపు
		}
		లేకపోతే
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#నిర్వచితమైతే VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_బహిర్గతం = mch_fopen((అక్షరం *)tempname, WRITEBIN);
		    (శూన్య)umask(umask_save);
#లేకపోతే
		    పూర్ణాంకం	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((అక్షరం *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(పూర్ణాంకం)((st_old.st_బహుళకం & 0777) | 0600));
		    (శూన్య)umask(umask_save);
# లేకపోతే
		    fd = mch_open((అక్షరం *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    ఒకవేళ (fd < 0)
		    {
			fp_బహిర్గతం = ఖాళీ;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			ఒకవేళ (errno != EEXIST)
			    ఆపు;
# endif
		    }
		    లేకపోతే
			fp_బహిర్గతం = fdopen(fd, WRITEBIN);
#అయితే_ముగింపు // VMS
		    ఒకవేళ (fp_బహిర్గతం != ఖాళీ)
			ఆపు;
		}

		// Assume file exists, try again with another name.
		ఒకవేళ (తదుపరి_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_తాత్కాలిక_files_like_str), tempname);
		    ఆపు;
		}
		*wp = తదుపరి_char;
		--తదుపరి_char;
	    }

	    ఒకవేళ (tempname != ఖాళీ)
		ఆపు;
	    // continue if shortname was set
	}

#ఒకవేళ defined(UNIX) && defined(HAVE_FCHOWN)
	ఒకవేళ (tempname != ఖాళీ && fp_బహిర్గతం != ఖాళీ)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    ఒకవేళ (mch_stat((అక్షరం *)tempname, &tmp_st) >= 0)
	    {
		ఒకవేళ (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_బహిర్గతం), st_old.st_uid, -1);
		ఒకవేళ (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_బహిర్గతం), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (శూన్య)mch_setperm(tempname, 0600);
	    }
	    లేకపోతే
		// can't stat the file, set conservative permissions
		(శూన్య)mch_setperm(tempname, 0600);
	}
#అయితే_ముగింపు
    }

    // Check if the new viminfo file can be written to.
    ఒకవేళ (fp_బహిర్గతం == ఖాళీ)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_లోపల == ఖాళీ || tempname == ఖాళీ) ? fname : tempname);
	ఒకవేళ (fp_లోపల != ఖాళీ)
	    fclose(fp_లోపల);
	వెళ్ళు చివర;
    }

    ఒకవేళ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_లోపల, fp_బహిర్గతం, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    ఒకవేళ (fclose(fp_బహిర్గతం) == EOF)
	++viminfo_errcnt;

    ఒకవేళ (fp_లోపల != ఖాళీ)
    {
	fclose(fp_లోపల);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	ఒకవేళ (viminfo_errcnt == 0)
	{
	    ఒకవేళ (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_గమ్యం_str), fname);
	    }
#నిర్వచితమైతే MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    లేకపోతే ఒకవేళ (hidden)
		mch_hide(fname);
#అయితే_ముగింపు
	}
	ఒకవేళ (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

చివర:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    శూన్య
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    ఒకవేళ (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    ఒకవేళ (eap->cmdidx == CMD_rviminfo)
    {
	ఒకవేళ (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    లేకపోతే
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#అయితే_ముగింపు // FEAT_VIMINFO
