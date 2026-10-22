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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    char_u	*vir_લીટી;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    પૂર્ણાંક		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

પ્રકારવ્યાખ્યા ગણના {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

પ્રકારવ્યાખ્યા સંરચના {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    પૂર્ણાંક		bv_len;		// length of bv_string
    પૂર્ણાંક		bv_allocated;	// bv_string was allocated
} bval_T;

#જો defined(FEAT_VIMINFO)

સ્થિર પૂર્ણાંક  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    સ્થિર char_u *
find_viminfo_parameter(પૂર્ણાંક type)
{
    char_u  *p;

    માટે (p = p_viminfo; *p; ++p)
    {
	જો (*p == type)
	    પાછા p + 1;
	જો (*p == 'n')		    // 'n' is always the last one
	    તોડો;
	p = vim_strchr(p, ',');	    // skip until next ','
	જો (p == ખાલી)		    // hit the end without finding parameter
	    તોડો;
    }
    પાછા ખાલી;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    પૂર્ણાંક
get_viminfo_parameter(પૂર્ણાંક type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    જો (p != ખાલી && VIM_ISDIGIT(*p))
	પાછા atoi((અક્ષર *)p);
    પાછા -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    સ્થિર char_u *
viminfo_filename(char_u *file)
{
    જો (file == ખાલી || *file == NUL)
    {
	size_t  len;

	જો (*p_viminfofile != NUL)
	    file = p_viminfofile;
	નહીંતર જો ((file = find_viminfo_parameter('n')) == ખાલી || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    જો (mch_getenv((char_u *)"SYS$LOGIN") == ખાલી)
# નહીંતર
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    જો (STRCMP(vim_getenv((char_u *)"HOME", ખાલી), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == ખાલી)
#  નહીંતર
	    જો (mch_getenv((char_u *)"HOME") == ખાલી)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		જો (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		નહીંતર
		    file = (char_u *)VIMINFO_FILE;
	    }
	    નહીંતર
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	પાછા vim_strnsave(file, len);
    }

    પાછા vim_strsave(file);
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
    સ્થિર શૂન્ય
viminfo_writestring(FILE *fd, char_u *p)
{
    પૂર્ણાંક		c;
    char_u	*s;
    પૂર્ણાંક		len = 0;

    માટે (s = p; *s != NUL; ++s)
    {
	જો (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    જો (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    જ્યારે ((c = *p++) != NUL)
    {
	જો (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    જો (c == '\n')
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
    સ્થિર પૂર્ણાંક
barline_writestring(FILE *fd, char_u *s, પૂર્ણાંક remaining_start)
{
    char_u *p;
    પૂર્ણાંક	    remaining = remaining_start;
    પૂર્ણાંક	    len = 2;

    // Count the number of characters produced, including quotes.
    માટે (p = s; *p != NUL; ++p)
    {
	જો (*p == NL)
	    len += 2;
	નહીંતર જો (*p == '"' || *p == '\\')
	    len += 2;
	નહીંતર
	    ++len;
    }
    જો (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    માટે (p = s; *p != NUL; ++p)
    {
	જો (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	નહીંતર જો (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	નહીંતર
	    putc(*p, fd);
	--remaining;

	જો (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    પાછા remaining - 2;
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
    સ્થિર char_u *
viminfo_readstring(
    vir_T	*virp,
    પૂર્ણાંક		off,		    // offset for virp->vir_line
    પૂર્ણાંક		convert)	    // convert the string
{
    char_u	*retval = ખાલી;
    char_u	*s, *d;
    long	len;

    જો (virp->vir_લીટી[off] == Ctrl_V && vim_isdigit(virp->vir_લીટી[off + 1]))
    {
	len = atol((અક્ષર *)virp->vir_લીટી + off + 1);
	જો (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	જો (retval == ખાલી)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (શૂન્ય)vim_fgets(virp->vir_લીટી, 10, virp->vir_fd);
	    પાછા ખાલી;
	}
	(શૂન્ય)vim_fgets(retval, (પૂર્ણાંક)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    નહીંતર
    {
	retval = vim_strsave(virp->vir_લીટી + off);
	જો (retval == ખાલી)
	    પાછા ખાલી;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    જ્યારે (*s != NUL && *s != '\n')
    {
	જો (s[0] == Ctrl_V && s[1] != NUL)
	{
	    જો (s[1] == 'n')
		*d++ = '\n';
	    નહીંતર
		*d++ = Ctrl_V;
	    s += 2;
	}
	નહીંતર
	    *d++ = *s++;
    }
    *d = NUL;

    જો (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, ખાલી);
	જો (d != ખાલી)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    પાછા retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    સ્થિર પૂર્ણાંક
viminfo_readline(vir_T *virp)
{
    પાછા vim_fgets(virp->vir_લીટી, LSIZE, virp->vir_fd);
}

    સ્થિર પૂર્ણાંક
read_viminfo_bufferlist(
    vir_T	*virp,
    પૂર્ણાંક		writing)
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
    જો (xline != ખાલી && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != ખાલી)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	જો (tab != ખાલી)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((અક્ષર *)tab);
	    tab = vim_strrchr(xline, '\t');
	    જો (tab != ખાલી)
	    {
		*tab++ = '\0';
		lnum = atol((અક્ષર *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	જો (buf != ખાલી)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    પાછા viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    સ્થિર પૂર્ણાંક
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    પૂર્ણાંક	    part_len;
    પૂર્ણાંક	    retval = FALSE;

    name = home_replace_save(ખાલી, name);
    જો (name == ખાલી)
	પાછા FALSE;
    માટે (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, કદ(part), ", ");
	જો (part[0] == 'r')
	{
	    જો (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		તોડો;
	    }
	}
    }
    vim_free(name);
    પાછા retval;
}

    સ્થિર શૂન્ય
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*લીટી;
    પૂર્ણાંક		max_buffers;

    જો (find_viminfo_parameter('%') == ખાલી)
	પાછા;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define લીટી_BUF_LEN (MAXPATHL + 40)
    લીટી = alloc(લીટી_BUF_LEN);
    જો (લીટી == ખાલી)
	પાછા;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	જો (buf->b_fname == ખાલી
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    ચાલુ;

	જો (max_buffers-- == 0)
	    તોડો;
	putc('%', fp);
	linelen = home_replace(ખાલી, buf->b_ffname, લીટી, MAXPATHL, TRUE);
	vim_snprintf((અક્ષર *)લીટી + linelen, લીટી_BUF_LEN - linelen, "\t%ld\t%d",
			(long)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, લીટી);
    }
    vim_free(લીટી);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
સ્થિર histentry_T *viminfo_history[HIST_ગણતરી] =
					       {ખાલી, ખાલી, ખાલી, ખાલી, ખાલી};
સ્થિર પૂર્ણાંક	viminfo_hisidx[HIST_ગણતરી] = {0, 0, 0, 0, 0};
સ્થિર પૂર્ણાંક	viminfo_hislen[HIST_ગણતરી] = {0, 0, 0, 0, 0};
સ્થિર પૂર્ણાંક	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    સ્થિર પૂર્ણાંક
hist_type2char(
    પૂર્ણાંક	    type,
    પૂર્ણાંક	    use_question)	    // use '?' instead of '/'
{
    જો (type == HIST_CMD)
	પાછા ':';
    જો (type == HIST_શોધ)
    {
	જો (use_question)
	    પાછા '?';
	નહીંતર
	    પાછા '/';
    }
    જો (type == HIST_પદાવલિ)
	પાછા '=';
    પાછા '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    સ્થિર શૂન્ય
prepare_viminfo_history(પૂર્ણાંક asklen, પૂર્ણાંક writing)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    અંક;
    પૂર્ણાંક	    type;
    પૂર્ણાંક	    len;
    પૂર્ણાંક	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    જો (asklen > hislen)
	asklen = hislen;

    માટે (type = 0; type < HIST_ગણતરી; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	માટે (i = 0, અંક = 0; i < hislen; i++)
	    જો (histentry[i].hisstr == ખાલી || histentry[i].viminfo)
		અંક++;
	len = asklen;
	જો (અંક > len)
	    len = અંક;
	જો (len <= 0)
	    viminfo_history[type] = ખાલી;
	નહીંતર
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	જો (viminfo_history[type] == ખાલી)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    સ્થિર પૂર્ણાંક
read_viminfo_history(vir_T *virp, પૂર્ણાંક writing)
{
    પૂર્ણાંક		type;
    long_u	len;
    char_u	*val = ખાલી;
    char_u	*p;

    type = hist_char2type(virp->vir_લીટી[0]);
    જો (viminfo_hisidx[type] >= viminfo_hislen[type])
	જાઓ done;

    val = viminfo_readstring(virp, 1, TRUE);
    જો (val == ખાલી || *val == NUL)
	જાઓ done;

    પૂર્ણાંક sep = (*val == ' ' ? NUL : *val);

    જો (in_history(type, val + (type == HIST_શોધ), viminfo_add_at_front,
								sep, writing))
	જાઓ done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    જો (type == HIST_શોધ)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	જો (p == ખાલી)
	    જાઓ done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    નહીંતર
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	જો (p == ખાલી)
	    જાઓ done;

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
    પાછા viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    સ્થિર શૂન્ય
handle_viminfo_history(
	garray_T    *કિંમતો,
	પૂર્ણાંક	    writing)
{
    પૂર્ણાંક		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)કિંમતો->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    જો (કિંમતો->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	પાછા;

    type = vp[0].bv_nr;
    જો (type >= HIST_ગણતરી)
	પાછા;

    જો (viminfo_hisidx[type] >= viminfo_hislen[type])
	પાછા;

    val = vp[3].bv_string;
    જો (val == ખાલી || *val == NUL)
	પાછા;

    પૂર્ણાંક sep = type == HIST_શોધ && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    પૂર્ણાંક idx;
    પૂર્ણાંક overwrite = FALSE;

    જો (in_history(type, val, viminfo_add_at_front, sep, writing))
	પાછા;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    માટે (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	જો (STRCMP(val, p) == 0
		&& (type != HIST_શોધ || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    તોડો;
	}
    }

    જો (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    નહીંતર
	len = 0; // for picky compilers
    જો (p != ખાલી)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	જો (!overwrite)
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
    સ્થિર શૂન્ય
concat_history(પૂર્ણાંક type)
{
    પૂર્ણાંક		idx;
    પૂર્ણાંક		i;
    પૂર્ણાંક		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    પૂર્ણાંક		*hisidx = get_hisidx(type);
    પૂર્ણાંક		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    જો (idx >= hislen)
	idx -= hislen;
    નહીંતર જો (idx < 0)
	idx = hislen - 1;
    જો (viminfo_add_at_front)
	*hisidx = idx;
    નહીંતર
    {
	જો (*hisidx == -1)
	    *hisidx = hislen - 1;
	કરો
	{
	    જો (histentry[idx].hisstr != ખાલી || histentry[idx].viminfo)
		તોડો;
	    જો (++idx == hislen)
		idx = 0;
	} જ્યારે (idx != *hisidx);
	જો (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    માટે (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	જો (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    માટે (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    સ્થિર પૂર્ણાંક
sort_hist(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    જો (p1->time_set < p2->time_set) પાછા -1;
    જો (p1->time_set > p2->time_set) પાછા 1;
    પાછા 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    સ્થિર શૂન્ય
merge_history(પૂર્ણાંક type)
{
    પૂર્ણાંક		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    પૂર્ણાંક		i;
    પૂર્ણાંક		len;
    પૂર્ણાંક		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    પૂર્ણાંક		*hisidx = get_hisidx(type);
    પૂર્ણાંક		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    જો (tot_hist == ખાલી || new_hist == ખાલી)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	પાછા;
    }
    માટે (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    માટે (i = 0; i < hislen; i++)
	જો (histentry[i].hisstr != ખાલી)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((શૂન્ય *)tot_hist, (size_t)len, કદ(histentry_T *), sort_hist);

    // Keep the newest ones.
    માટે (i = 0; i < hislen; i++)
    {
	જો (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = ખાલી;
	    tot_hist[i]->hisstrlen = 0;
	    જો (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	નહીંતર
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    માટે (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    માટે (i = 0; i < hislen; i++)
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
    સ્થિર શૂન્ય
finish_viminfo_history(vir_T *virp)
{
    પૂર્ણાંક	type;
    પૂર્ણાંક merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    માટે (type = 0; type < HIST_ગણતરી; ++type)
    {
	જો (get_histentry(type) == ખાલી)
	    ચાલુ;

	જો (merge)
	    merge_history(type);
	નહીંતર
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
    સ્થિર શૂન્ય
write_viminfo_history(FILE *fp, પૂર્ણાંક merge)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    type;
    પૂર્ણાંક	    અંક_saved;
    પૂર્ણાંક     round;
    પૂર્ણાંક	    hislen;

    init_history();
    hislen = get_hislen();
    જો (hislen == 0)
	પાછા;
    માટે (type = 0; type < HIST_ગણતરી; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	પૂર્ણાંક	    *hisidx = get_hisidx(type);

	અંક_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	જો (અંક_saved == 0)
	    ચાલુ;
	જો (અંક_saved < 0)  // Use default
	    અંક_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_શોધ ? _("Search String") :
			    type == HIST_પદાવલિ ? _("Expression") :
			    type == HIST_આગત ? _("Input Line") :
					_("Debug Line"));
	જો (અંક_saved > hislen)
	    અંક_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	માટે (round = 1; round <= 2; ++round)
	{
	    જો (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    નહીંતર જો (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    નહીંતર
		// empty list
		i = -1;
	    જો (i >= 0)
		જ્યારે (અંક_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    પૂર્ણાંક	    c = NUL;

		    જો (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    નહીંતર
		    {
			જો (viminfo_history[type] == ખાલી)
			{
			    p = ખાલી;
			    plen = 0;
			    timestamp = 0;
			}
			નહીંતર
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    જો (p != ખાલી && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--અંક_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			જો (type == HIST_શોધ)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    અક્ષર    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    જો (c == NUL)
				cbuf[0] = NUL;
			    નહીંતર
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    જો (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			જો (--i < 0)
			    i = hislen - 1;
			જો (i == *hisidx)
			    તોડો;
		    }
		    નહીંતર
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	માટે (i = 0; i < viminfo_hisidx[type]; ++i)
	    જો (viminfo_history[type] != ખાલી)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    સ્થિર શૂન્ય
write_viminfo_barlines(vir_T *virp, FILE *fp_out)
{
    પૂર્ણાંક		i;
    garray_T	*gap = &virp->vir_barlines;
    પૂર્ણાંક		seen_useful = FALSE;
    અક્ષર	*લીટી;

    જો (gap->ga_len <= 0)
	પાછા;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    માટે (i = 0; i < gap->ga_len; ++i)
    {
	લીટી = ((અક્ષર **)(gap->ga_data))[i];
	જો (seen_useful || લીટી[1] != '<')
	{
	    fputs(લીટી, fp_out);
	    seen_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    સ્થિર પૂર્ણાંક
barline_parse(vir_T *virp, char_u *text, garray_T *કિંમતો)
{
    char_u  *p = text;
    char_u  *nextp = ખાલી;
    char_u  *buf = ખાલી;
    bval_T  *કિંમત;
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    allocated = FALSE;
    પૂર્ણાંક	    eof;
    char_u  *sconv;
    પૂર્ણાંક	    converted;

    જ્યારે (*p == ',')
    {
	++p;
	જો (ga_grow(કિંમતો, 1) == FAIL)
	    તોડો;
	કિંમત = (bval_T *)(કિંમતો->ga_data) + કિંમતો->ga_len;

	જો (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    જો (!allocated)
	    {
		માટે (i = 0; i < કિંમતો->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(કિંમતો->ga_data) + i;

		    જો (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    જો (vim_isdigit(p[1]))
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
		જો (buf == ખાલી)
		    પાછા TRUE;
		p = buf;
		માટે (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    જો (eof || virp->vir_લીટી[0] != '|'
						  || virp->vir_લીટી[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			પાછા eof || virp->vir_લીટી[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_લીટી);
		    જ્યારે (n > 2 && (virp->vir_લીટી[n - 1] == NL
					     || virp->vir_લીટી[n - 1] == CAR))
			--n;
		    n -= 2;
		    જો (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_લીટી + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_લીટી + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    નહીંતર
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		જો (eof || virp->vir_લીટી[0] != '|'
					      || virp->vir_લીટી[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    પાછા eof || virp->vir_લીટી[0] == '|';
		p = virp->vir_લીટી + 2;
	    }
	}

	જો (SAFE_isdigit(*p))
	{
	    કિંમત->bv_type = BVAL_NR;
	    કિંમત->bv_nr = getdigits(&p);
	    ++કિંમતો->ga_len;
	}
	નહીંતર જો (*p == '"')
	{
	    પૂર્ણાંક	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    જ્યારે (*p != '"')
	    {
		જો (*p == NL || *p == NUL)
		    પાછા TRUE;  // syntax error, drop the value
		જો (*p == '\\')
		{
		    ++p;
		    જો (*p == 'n')
			s[len++] = '\n';
		    નહીંતર
			s[len++] = *p;
		    ++p;
		}
		નહીંતર
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    કિંમત->bv_tofree = ખાલી;
	    જો (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, ખાલી);
		જો (sconv != ખાલી)
		{
		    જો (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			કિંમત->bv_tofree = buf;
		    s = sconv;
		    len = (પૂર્ણાંક)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    જો (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    કિંમત->bv_string = s;
	    કિંમત->bv_type = BVAL_STRING;
	    કિંમત->bv_len = len;
	    કિંમત->bv_allocated = allocated || converted;
	    ++કિંમતો->ga_len;
	    જો (nextp != ખાલી)
	    {
		// values following a long string
		p = nextp;
		nextp = ખાલી;
	    }
	}
	નહીંતર જો (*p == ',')
	{
	    કિંમત->bv_type = BVAL_EMPTY;
	    ++કિંમતો->ga_len;
	}
	નહીંતર
	    તોડો;
    }
    પાછા TRUE;
}

    સ્થિર શૂન્ય
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    સ્થિર પૂર્ણાંક
no_viminfo(શૂન્ય)
{
    // "vim -i NONE" does not read or write a viminfo file
    પાછા STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    સ્થિર પૂર્ણાંક
viminfo_error(અક્ષર *errnum, અક્ષર *message, char_u *લીટી)
{
    vim_snprintf((અક્ષર *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, લીટી, IOSIZE - STRLEN(IObuff) - 1);
    જો (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((અક્ષર *)IObuff);
    જો (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    સ્થિર પૂર્ણાંક
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    પૂર્ણાંક		i;

    જો (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_લીટી, '=');
	જો (p != ખાલી)
	{
	    // remove trailing newline
	    ++p;
	    માટે (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    પાછા viminfo_readline(virp);
}

#જો defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    સ્થિર પૂર્ણાંક
read_viminfo_varlist(vir_T *virp, પૂર્ણાંક writing)
{
    char_u	*tab;
    પૂર્ણાંક		type = VAR_સંખ્યા;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    જો (!writing && (find_viminfo_parameter('!') != ખાલી))
    {
	tab = vim_strchr(virp->vir_લીટી + 1, '\t');
	જો (tab != ખાલી)
	{
	    *tab++ = '\0';	// isolate the variable name
	    પસંદગી (*tab)
	    {
		સ્થિતિ 'S': type = VAR_STRING; તોડો;
		સ્થિતિ 'F': type = VAR_FLOAT; તોડો;
		સ્થિતિ 'D': type = VAR_DICT; તોડો;
		સ્થિતિ 'L': type = VAR_LIST; તોડો;
		સ્થિતિ 'B': type = VAR_BLOB; તોડો;
		સ્થિતિ 'X': type = VAR_SPECIAL; તોડો;
		સ્થિતિ 'T': type = VAR_TUPLE; તોડો;
	    }

	    tab = vim_strchr(tab, '\t');
	    જો (tab != ખાલી)
	    {
		tv.v_type = type;
		જો (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (પૂર્ણાંક)(tab - virp->vir_લીટી + 1), TRUE);
		નહીંતર જો (type == VAR_FLOAT)
		    (શૂન્ય)string2float(tab + 1, &tv.vval.v_float, FALSE);
		નહીંતર
		{
		    tv.vval.v_સંખ્યા = atol((અક્ષર *)tab + 1);
		    જો (type == VAR_SPECIAL && (tv.vval.v_સંખ્યા == VVAL_FALSE
					     || tv.vval.v_સંખ્યા == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		જો (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_પદાવલિ(tv.vval.v_string, ખાલી);

		    જો (etv == ખાલી)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    નહીંતર
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		નહીંતર જો (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    જો (blob == ખાલી)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    નહીંતર
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_entry);
		set_var(virp->vir_લીટી + 1, &tv, FALSE);
		restore_funccal();

		જો (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		નહીંતર જો (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    પાછા viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    સ્થિર શૂન્ય
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    પૂર્ણાંક		todo;
    અક્ષર	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    જો (find_viminfo_parameter('!') == ખાલી)
	પાછા;

    fputs(_("\n# global variables:\n"), fp);

    todo = (પૂર્ણાંક)gvht->ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(gvht, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    જો (var_flavour(this_var->di_ચાવી) == VAR_FLAVOUR_VIMINFO)
	    {
		પસંદગી (this_var->di_tv.v_type)
		{
		    સ્થિતિ VAR_STRING:  s = "STR"; તોડો;
		    સ્થિતિ VAR_સંખ્યા:  s = "NUM"; તોડો;
		    સ્થિતિ VAR_FLOAT:   s = "FLO"; તોડો;
		    સ્થિતિ VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      પૂર્ણાંક	copyID = get_copyID();

			      s = "DIC";
			      જો (di != ખાલી && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, ખાલી, ખાલી)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ચાલુ;
			      તોડો;
			  }
		    સ્થિતિ VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      પૂર્ણાંક	copyID = get_copyID();

			      s = "LIS";
			      જો (l != ખાલી && !set_ref_in_list_વસ્તુઓ(
						       l, copyID, ખાલી, ખાલી)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ચાલુ;
			      તોડો;
			  }
		    સ્થિતિ VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      પૂર્ણાંક	copyID = get_copyID();

			      s = "TUP";
			      જો (tuple != ખાલી && !set_ref_in_tuple_વસ્તુઓ(
					       tuple, copyID, ખાલી, ખાલી)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ચાલુ;
			      તોડો;
			  }
		    સ્થિતિ VAR_BLOB:    s = "BLO"; તોડો;
		    સ્થિતિ VAR_BOOL:    s = "XPL"; તોડો;  // backwards compat.
		    સ્થિતિ VAR_SPECIAL: s = "XPL"; તોડો;

		    સ્થિતિ VAR_UNKNOWN:
		    સ્થિતિ VAR_ANY:
		    સ્થિતિ VAR_VOID:
		    સ્થિતિ VAR_FUNC:
		    સ્થિતિ VAR_PARTIAL:
		    સ્થિતિ VAR_JOB:
		    સ્થિતિ VAR_CHANNEL:
		    સ્થિતિ VAR_INSTR:
		    સ્થિતિ VAR_CLASS:
		    સ્થિતિ VAR_OBJECT:
		    સ્થિતિ VAR_TYPEALIAS:
				      ચાલુ;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_ચાવી, s);
		જો (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((અક્ષર *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_સંખ્યા);
		    p = numbuf;
		    tofree = ખાલી;
		}
		નહીંતર
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		જો (p != ખાલી)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    સ્થિર પૂર્ણાંક
read_viminfo_sub_string(vir_T *virp, પૂર્ણાંક force)
{
    જો (force || get_old_sub() == ખાલી)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    પાછા viminfo_readline(virp);
}

    સ્થિર શૂન્ય
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    જો (get_viminfo_parameter('/') == 0 || old_sub == ખાલી)
	પાછા;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    સ્થિર પૂર્ણાંક
read_viminfo_શોધ_pattern(vir_T *virp, પૂર્ણાંક force, પૂર્ણાંક writing)
{
    char_u	*lp;
    પૂર્ણાંક		idx = -1;
    પૂર્ણાંક		magic = FALSE;
    પૂર્ણાંક		no_scs = FALSE;
    પૂર્ણાંક		off_લીટી = FALSE;
    પૂર્ણાંક		off_end = 0;
    long	off = 0;
    પૂર્ણાંક		setlast = FALSE;
#ifdef FEAT_શોધ_EXTRA
    સ્થિર પૂર્ણાંક	hlsearch_on = FALSE;
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
    lp = virp->vir_લીટી;
    જો (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	જો (lp[1] == 'M')		// magic on
	    magic = TRUE;
	જો (lp[2] == 's')
	    no_scs = TRUE;
	જો (lp[3] == 'L')
	    off_લીટી = TRUE;
	જો (lp[4] == 'E')
	    off_end = શોધ_END;
	lp += 5;
	off = getdigits(&lp);
    }
    જો (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    જો (lp[0] == '/')
	idx = RE_શોધ;
    નહીંતર જો (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_શોધ_EXTRA
    નહીંતર જો (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    નહીંતર જો (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    જો (idx >= 0)
    {
	spat = get_spat(idx);
	જો (force || spat->pat == ખાલી)
	{
	    val = viminfo_readstring(virp, (પૂર્ણાંક)(lp - virp->vir_લીટી + 1),
									TRUE);
	    જો (val != ખાલી)
	    {
		જો (!writing)
		{
		    set_last_શોધ_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.લીટી = off_લીટી;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_શોધ_EXTRA
		    જો (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    પાછા viminfo_readline(virp);
}

    સ્થિર શૂન્ય
wvsp_one(
    FILE	*fp,	// file to write to
    પૂર્ણાંક		idx,	// spats[] index
    અક્ષર	*s,	// search pat
    પૂર્ણાંક		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    જો (spat->pat == ખાલી)
	પાછા;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.લીટી ? 'L' : 'l',	// line offset
	    spat->off.end  ? 'E' : 'e',	// offset from end
	    spat->off.off,			// offset
	    get_spat_last_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    સ્થિર શૂન્ય
write_viminfo_શોધ_pattern(FILE *fp)
{
    જો (get_viminfo_parameter('/') == 0)
	પાછા;

#ifdef FEAT_શોધ_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != ખાલી) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_શોધ, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

સ્થિર yankreg_T *y_read_regs = ખાલી;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    સ્થિર શૂન્ય
prepare_viminfo_registers(શૂન્ય)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, અંક_REGISTERS);
}

    સ્થિર શૂન્ય
finish_viminfo_registers(શૂન્ય)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		j;

    જો (y_read_regs == ખાલી)
	પાછા;

    માટે (i = 0; i < અંક_REGISTERS; ++i)
	જો (y_read_regs[i].y_array != ખાલી)
	{
	    માટે (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    સ્થિર પૂર્ણાંક
read_viminfo_register(vir_T *virp, પૂર્ણાંક force)
{
    પૂર્ણાંક		eof;
    પૂર્ણાંક		do_it = TRUE;
    પૂર્ણાંક		size;
    પૂર્ણાંક		limit;
    પૂર્ણાંક		i;
    પૂર્ણાંક		set_prev = FALSE;
    char_u	*str;
    string_T	*array = ખાલી;
    પૂર્ણાંક		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_લીટી + 1;

    // If the line starts with "" this is the y_previous register.
    જો (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    જો (!ASCII_ISALNUM(*str) && *str != '-')
    {
	જો (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_લીટી))
	    પાછા TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    જો (!force && y_current_p->y_array != ખાલી)
	do_it = FALSE;

    જો (*str == '@')
    {
	// "x@: register x used for @@
	જો (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    જો (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	જો (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	જો (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	નહીંતર જો (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	નહીંતર
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    જ્યારે (!(eof = viminfo_readline(virp))
		    && (virp->vir_લીટી[0] == TAB || virp->vir_લીટી[0] == '<'))
    {
	જો (do_it)
	{
	    જો (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * કદ(string_T));

		જો (new_array == ખાલી)
		{
		    do_it = FALSE;
		    તોડો;
		}
		માટે (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    જો (str != ખાલી)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    નહીંતર
		// error, don't store the result
		do_it = FALSE;
	}
    }

    જો (do_it)
    {
	// free y_array[]
	માટે (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	જો (size == 0)
	{
	    y_current_p->y_array = ખાલી;
	}
	નહીંતર
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    માટે (i = 0; i < size; i++)
	    {
		જો (y_current_p->y_array == ખાલી)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		નહીંતર
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    નહીંતર
    {
	// Free array[] if it was filled.
	માટે (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    પાછા eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    સ્થિર શૂન્ય
handle_viminfo_register(garray_T *કિંમતો, પૂર્ણાંક force)
{
    bval_T	*vp = (bval_T *)કિંમતો->ga_data;
    પૂર્ણાંક		flags;
    પૂર્ણાંક		name;
    પૂર્ણાંક		type;
    પૂર્ણાંક		linecount;
    પૂર્ણાંક		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    પૂર્ણાંક		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    જો (કિંમતો->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	પાછા;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    જો (name < 0 || name >= અંક_REGISTERS)
	પાછા;
    type = vp[2].bv_nr;
    જો (type != MCHAR && type != MLINE && type != MBLOCK)
	પાછા;
    linecount = vp[3].bv_nr;
    જો (કિંમતો->ga_len < 6 + linecount)
	પાછા;
    width = vp[4].bv_nr;
    જો (width < 0)
	પાછા;

    જો (y_read_regs != ખાલી)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    નહીંતર
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    જો (y_ptr->y_array != ખાલી && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	પાછા;

    જો (y_ptr->y_array != ખાલી)
	માટે (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    જો (y_read_regs == ખાલી)
    {
	જો (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	જો ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    જો (linecount == 0)
    {
	y_ptr->y_array = ખાલી;
	પાછા;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    જો (y_ptr->y_array == ખાલી)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	પાછા;
    }
    માટે (i = 0; i < linecount; i++)
    {
	જો (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = ખાલી;
	}
	નહીંતર જો (vp[i + 6].bv_type != BVAL_STRING)
	{
	    મુક્ત(y_ptr->y_array);
	    y_ptr->y_array = ખાલી;
	}
	નહીંતર
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    સ્થિર શૂન્ય
write_viminfo_registers(FILE *fp)
{
    પૂર્ણાંક		i, j;
    char_u	*type;
    char_u	c;
    પૂર્ણાંક		અંક_લીટીઓ;
    પૂર્ણાંક		max_અંક_લીટીઓ;
    પૂર્ણાંક		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_અંક_લીટીઓ = get_viminfo_parameter('<');
    જો (max_અંક_લીટીઓ < 0)
	max_અંક_લીટીઓ = get_viminfo_parameter('"');
    જો (max_અંક_લીટીઓ == 0)
	પાછા;
    max_kbyte = get_viminfo_parameter('s');
    જો (max_kbyte == 0)
	પાછા;

    માટે (i = 0; i < અંક_REGISTERS; i++)
    {
#જો defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	જો (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    ચાલુ;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	જો (i == TILDE_REGISTER)
	    ચાલુ;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	જો (y_read_regs != ખાલી
		&& y_read_regs[i].y_array != ખાલી
		&& (y_regs_p[i].y_array == ખાલી ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	નહીંતર જો (y_regs_p[i].y_array == ખાલી)
	    ચાલુ;
	નહીંતર
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	અંક_લીટીઓ = y_ptr->y_size;
	જો (અંક_લીટીઓ == 0
		|| (અંક_લીટીઓ == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    ચાલુ;

	જો (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    માટે (j = 0; j < અંક_લીટીઓ; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    જો (len > (long)max_kbyte * 1024L)
		ચાલુ;
	}

	પસંદગી (y_ptr->y_type)
	{
	    સ્થિતિ MLINE:
		type = (char_u *)"LINE";
		તોડો;
	    સ્થિતિ MCHAR:
		type = (char_u *)"CHAR";
		તોડો;
	    સ્થિતિ MBLOCK:
		type = (char_u *)"BLOCK";
		તોડો;
	    મૂળભૂત:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		તોડો;
	}
	જો (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	જો (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (પૂર્ણાંક)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	જો (max_અંક_લીટીઓ > 0 && અંક_લીટીઓ > max_અંક_લીટીઓ)
	    અંક_લીટીઓ = max_અંક_લીટીઓ;
	માટે (j = 0; j < અંક_લીટીઓ; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    પૂર્ણાંક	    flags = 0;
	    પૂર્ણાંક	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    જો (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    જો (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, અંક_લીટીઓ, (પૂર્ણાંક)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    માટે (j = 0; j < અંક_લીટીઓ; j++)
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

સ્થિર xfmark_T *vi_namedfm = ખાલી;
સ્થિર xfmark_T *vi_jumplist = ખાલી;
સ્થિર પૂર્ણાંક vi_jumplist_len = 0;

    સ્થિર શૂન્ય
write_one_mark(FILE *fp_out, પૂર્ણાંક c, pos_T *pos)
{
    જો (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (પૂર્ણાંક)pos->col);
}

    સ્થિર શૂન્ય
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    પૂર્ણાંક		i;
    pos_T	pos;

    home_replace(ખાલી, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_ઉમેરો);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    માટે (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	જો (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    માટે (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    સ્થિર પૂર્ણાંક
skip_for_viminfo(buf_T *buf)
{
    પાછા bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    સ્થિર શૂન્ય
write_viminfo_marks(FILE *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    પૂર્ણાંક		is_mark_set;
    પૂર્ણાંક		i;
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
	જો (buf->b_marks_read)
	{
	    જો (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    નહીંતર
	    {
		is_mark_set = FALSE;
		માટે (i = 0; i < NMARKS; i++)
		    જો (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			તોડો;
		    }
	    }
	    જો (is_mark_set && buf->b_ffname != ખાલી
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		જો (buflist == ખાલી)
		    write_buffer_marks(buf, fp_out);
		નહીંતર જો (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    સ્થિર શૂન્ય
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    પૂર્ણાંક		c1,
    પૂર્ણાંક		c2)
{
    char_u	*name;

    જો (fm->fmark.mark.lnum == 0)	// not set
	પાછા;

    જો (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    નહીંતર
	name = fm->fname;		// use name from .viminfo
    જો (name != ખાલી && *name != NUL)
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

    જો (fm->fmark.fnum != 0)
	vim_free(name);
}

    સ્થિર શૂન્ય
write_viminfo_filemarks(FILE *fp)
{
    પૂર્ણાંક		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    પૂર્ણાંક		vi_idx;
    પૂર્ણાંક		idx;

    જો (get_viminfo_parameter('f') == 0)
	પાછા;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    માટે (i = 0; i < NMARKS; i++)
    {
	જો (vi_namedfm != ખાલી
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	નહીંતર
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    જો (curbuf->b_ffname != ખાલી && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	માટે (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    જો (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == ખાલી
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != ખાલી
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		તોડો;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	માટે ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = ખાલી;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    માટે (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != ખાલી ? &vi_namedfm[vi_idx] : ખાલી;

	જો (vi_fm != ખાલી
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	નહીંતર
	{
	    fm = &namedfm_p[idx++];
	    જો (vi_fm != ખાલી
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != ખાલી
			  && fm->fname != ખાલી
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
    માટે (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : ખાલી;
	vi_fm = (vi_jumplist != ખાલી && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : ખાલી;
	જો (fm == ખાલી && vi_fm == ખાલી)
	    તોડો;
	જો (fm == ખાલી || (vi_fm != ખાલી && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	નહીંતર
	    --idx;
	જો (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != ખાલી
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    પૂર્ણાંક
buf_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    જો (buf1->b_last_used == buf2->b_last_used)
	પાછા 0;
    પાછા buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    સ્થિર શૂન્ય
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_out,
    garray_T	*buflist,
    પૂર્ણાંક		eof,
    પૂર્ણાંક		flags)
{
    char_u	*લીટી = virp->vir_લીટી;
    buf_T	*buf;
    પૂર્ણાંક		અંક_marked_files;
    પૂર્ણાંક		load_marks;
    પૂર્ણાંક		copy_marks_out;
    char_u	*str;
    પૂર્ણાંક		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = ખાલી;
#endif
    પૂર્ણાંક		ગણતરી = 0;
    પૂર્ણાંક		buflist_used = 0;
    buf_T	*buflist_buf = ખાલી;

    જો ((name_buf = alloc(LSIZE)) == ખાલી)
	પાછા;
    *name_buf = NUL;

    જો (fp_out != ખાલી && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						કદ(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    જો (fp_out == ખાલી && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	જો (list != ખાલી)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    અંક_marked_files = get_viminfo_parameter('\'');
    જ્યારે (!eof && (ગણતરી < અંક_marked_files || fp_out == ખાલી))
    {
	જો (લીટી[0] != '>')
	{
	    જો (લીટી[0] != '\n' && લીટી[0] != '\r' && લીટી[0] != '#')
	    {
		જો (viminfo_error("E576: ", _(e_nonr_missing_gt), લીટી))
		    તોડો;	// too many errors, return now
	    }
	    eof = vim_fgets(લીટી, LSIZE, virp->vir_fd);
	    ચાલુ;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(લીટી + 1);
	str = viminfo_readstring(virp, (પૂર્ણાંક)(str - virp->vir_લીટી), FALSE);
	જો (str == ખાલી)
	    ચાલુ;
	p = str + STRLEN(str);
	જ્યારે (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	જો (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	જો (list != ખાલી)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	જો (fp_out == ખાલી)
	{
	    જો ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != ખાલી)
	    {
		જો (*name_buf == NUL)	    // only need to do this once
		    home_replace(ખાલી, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		જો (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	નહીંતર // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		જો (buf->b_ffname != ખાલી)
		{
		    home_replace(ખાલી, buf->b_ffname, name_buf, LSIZE, TRUE);
		    જો (fnamecmp(str, name_buf) == 0)
			તોડો;
		}

	    // Copy marks if the buffer has not been loaded.
	    જો (buf == ખાલી || !buf->b_marks_read)
	    {
		પૂર્ણાંક	did_read_લીટી = FALSE;

		જો (buflist_buf != ખાલી)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    જો (!viminfo_readline(virp) && લીટી[0] == TAB)
		    {
			did_read_લીટી = TRUE;
			જો (લીટી[1] == '*')
			{
			    long	ltime;

			    sscanf((અક્ષર *)લીટી + 2, "%ld ", &ltime);
			    જ્યારે ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				જો (++ગણતરી >= અંક_marked_files)
				    તોડો;
				જો (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = ખાલી;
				    તોડો;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			નહીંતર
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    જ્યારે (ગણતરી < અંક_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++ગણતરી;
			    }
			    buflist_buf = ખાલી;
			}

			જો (ગણતરી >= અંક_marked_files)
			{
			    vim_free(str);
			    તોડો;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		જો (did_read_લીટી)
		    fputs((અક્ષર *)લીટી, fp_out);

		ગણતરી++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	જ્યારે (!(eof = viminfo_readline(virp)) && લીટી[0] == TAB)
	{
	    જો (load_marks)
	    {
		જો (લીટી[1] != NUL)
		{
		    unsigned u;

		    sscanf((અક્ષર *)લીટી + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    પસંદગી (લીટી[1])
		    {
			સ્થિતિ '"': curbuf->b_last_cursor = pos; તોડો;
			સ્થિતિ '^': curbuf->b_last_ઉમેરો = pos; તોડો;
			સ્થિતિ '.': curbuf->b_last_change = pos; તોડો;
			સ્થિતિ '+':
				  // changelist positions are stored oldest
				  // first
				  જો (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    કદ(pos_T) * (JUMPLISTSIZE - 1));
				  નહીંતર
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  તોડો;

				  // Using the line number for the last-used
				  // timestamp.
			સ્થિતિ '*': curbuf->b_last_used = pos.lnum; તોડો;

			મૂળભૂત:  જો ((i = લીટી[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    નહીંતર જો (copy_marks_out)
		fputs((અક્ષર *)લીટી, fp_out);
	}

	જો (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		જો (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    જો (flags & VIF_ONLY_CURBUF)
		તોડો;
	}
    }

    જો (fp_out != ખાલી)
	// Write any remaining entries from buflist.
	જ્યારે (ગણતરી < અંક_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++ગણતરી;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    શૂન્ય
check_marks_read(શૂન્ય)
{
    જો (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != ખાલી)
	read_viminfo(ખાલી, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    સ્થિર પૂર્ણાંક
read_viminfo_filemark(vir_T *virp, પૂર્ણાંક force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    પૂર્ણાંક		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_લીટી + 1;
    જો (*str <= 127
	    && ((*virp->vir_લીટી == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_લીટી == '-' && *str == '\'')))
    {
	જો (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    જો (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = ખાલી;
	    નહીંતર
	    {
		માટે (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = ખાલી;
	    }
	}
	નહીંતર જો (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	નહીંતર
	    fm = &namedfm_p[*str - 'A'];
	જો (fm != ખાલી && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (પૂર્ણાંક)(str - virp->vir_લીટી),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    પાછા vim_fgets(virp->vir_લીટી, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    સ્થિર શૂન્ય
prepare_viminfo_marks(શૂન્ય)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    સ્થિર શૂન્ય
finish_viminfo_marks(શૂન્ય)
{
    પૂર્ણાંક		i;

    જો (vi_namedfm != ખાલી)
    {
	માટે (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    જો (vi_jumplist != ખાલી)
    {
	માટે (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    સ્થિર શૂન્ય
handle_viminfo_mark(garray_T *કિંમતો, પૂર્ણાંક force)
{
    bval_T	*vp = (bval_T *)કિંમતો->ga_data;
    પૂર્ણાંક		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = ખાલી;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    જો (કિંમતો->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	પાછા;

    name = vp[0].bv_nr;
    જો (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	પાછા;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    જો (lnum <= 0 || col < 0)
	પાછા;
    timestamp = (time_t)vp[3].bv_nr;

    જો (name == '\'')
    {
	જો (vi_jumplist != ખાલી)
	{
	    જો (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	નહીંતર
	{
	    પૂર્ણાંક idx;
	    પૂર્ણાંક i;

	    // If we have a timestamp insert it in the right place.
	    જો (timestamp != 0)
	    {
		માટે (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    જો (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			તોડો;
		    }
		// idx cannot be zero now
		જો (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    નહીંતર જો (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    નહીંતર
		idx = -1;

	    જો (idx >= 0)
	    {
		જો (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    માટે (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		નહીંતર
		{
		    // Move newer entries forward.
		    માટે (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = ખાલી;
		fm->time_set = 0;
	    }
	}
    }
    નહીંતર
    {
	પૂર્ણાંક		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	જો (VIM_ISDIGIT(name))
	{
	    જો (vi_namedfm != ખાલી)
		idx = name - '0' + NMARKS;
	    નહીંતર
	    {
		પૂર્ણાંક i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		માટે (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    જો (namedfm_p[idx].time_set < timestamp)
			તોડો;
		જો (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    પાછા;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		માટે ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = ખાલી;
	    }
	}
	નહીંતર
	    idx = name - 'A';
	જો (vi_namedfm != ખાલી)
	    fm = &vi_namedfm[idx];
	નહીંતર
	    fm = &namedfm_p[idx];
    }

    જો (fm != ખાલી)
    {
	જો (vi_namedfm != ખાલી || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    જો (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = ખાલી;
	    }
	    નહીંતર
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    સ્થિર પૂર્ણાંક
read_viminfo_barline(vir_T *virp, પૂર્ણાંક got_encoding, પૂર્ણાંક force, પૂર્ણાંક writing)
{
    char_u	*p = virp->vir_લીટી + 1;
    પૂર્ણાંક		bartype;
    garray_T	કિંમતો;
    bval_T	*vp;
    પૂર્ણાંક		i;
    પૂર્ણાંક		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    જો (*p == '<')
    {
	// Continuation line of an unrecognized item.
	જો (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_લીટી);
    }
    નહીંતર
    {
	ga_init2(&કિંમતો, કદ(bval_T), 20);
	bartype = getdigits(&p);
	પસંદગી (bartype)
	{
	    સ્થિતિ BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		જો (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &કિંમતો);
		    vp = (bval_T *)કિંમતો.ga_data;
		    જો (કિંમતો.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		તોડો;

	    સ્થિતિ BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &કિંમતો);
		handle_viminfo_history(&કિંમતો, writing);
		તોડો;

	    સ્થિતિ BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &કિંમતો);
		handle_viminfo_register(&કિંમતો, force);
		તોડો;

	    સ્થિતિ BARTYPE_MARK:
		read_next = barline_parse(virp, p, &કિંમતો);
		handle_viminfo_mark(&કિંમતો, force);
		તોડો;

	    મૂળભૂત:
		// copy unrecognized line (for future use)
		જો (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_લીટી);
	}
	માટે (i = 0; i < કિંમતો.ga_len; ++i)
	{
	    vp = (bval_T *)કિંમતો.ga_data + i;
	    જો (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&કિંમતો);
    }

    જો (read_next)
	પાછા viminfo_readline(virp);
    પાછા FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    સ્થિર પૂર્ણાંક
read_viminfo_up_to_marks(
    vir_T	*virp,
    પૂર્ણાંક		forceit,
    પૂર્ણાંક		writing)
{
    પૂર્ણાંક		eof;
    buf_T	*buf;
    પૂર્ણાંક		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    જ્યારે (!eof && virp->vir_લીટી[0] != '>')
    {
	પસંદગી (virp->vir_લીટી[0])
	{
		// Characters reserved for future expansion, ignored now
	    સ્થિતિ '+': // "+40 /path/dir file", for running vim without args
	    સ્થિતિ '^': // to be defined
	    સ્થિતિ '<': // long line - ignored
		// A comment or empty line.
	    સ્થિતિ NUL:
	    સ્થિતિ '\r':
	    સ્થિતિ '\n':
	    સ્થિતિ '#':
		eof = viminfo_readline(virp);
		તોડો;
	    સ્થિતિ '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		તોડો;
	    સ્થિતિ '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		તોડો;
	    સ્થિતિ '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#નહીંતર
		eof = viminfo_readline(virp);
#endif
		તોડો;
	    સ્થિતિ '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		તોડો;
	    સ્થિતિ '"':
		// When registers are in bar lines skip the old style register
		// lines.
		જો (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		નહીંતર
		    કરો
		    {
			eof = viminfo_readline(virp);
		    } જ્યારે (!eof && (virp->vir_લીટી[0] == TAB
						|| virp->vir_લીટી[0] == '<'));
		તોડો;
	    સ્થિતિ '/':	    // Search string
	    સ્થિતિ '&':	    // Substitute search string
	    સ્થિતિ '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_શોધ_pattern(virp, forceit, writing);
		તોડો;
	    સ્થિતિ '$':
		eof = read_viminfo_sub_string(virp, forceit);
		તોડો;
	    સ્થિતિ ':':
	    સ્થિતિ '?':
	    સ્થિતિ '=':
	    સ્થિતિ '@':
		// When history is in bar lines skip the old style history
		// lines.
		જો (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		નહીંતર
		    eof = viminfo_readline(virp);
		તોડો;
	    સ્થિતિ '-':
	    સ્થિતિ '\'':
		// When file marks are in bar lines skip the old style lines.
		જો (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		નહીંતર
		    eof = viminfo_readline(virp);
		તોડો;
	    મૂળભૂત:
		જો (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_લીટી))
		    eof = TRUE;
		નહીંતર
		    eof = viminfo_readline(virp);
		તોડો;
	}
    }

    // Finish reading history items.
    જો (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    પાછા eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    સ્થિર શૂન્ય
do_viminfo(FILE *fp_in, FILE *fp_out, પૂર્ણાંક flags)
{
    પૂર્ણાંક		eof = FALSE;
    vir_T	vir;
    પૂર્ણાંક		merge = FALSE;
    પૂર્ણાંક		do_copy_marks = FALSE;
    garray_T	buflist;

    જો ((vir.vir_લીટી = alloc(LSIZE)) == ખાલી)
	પાછા;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, કદ(char_u *), 100);
    vir.vir_version = -1;

    જો (fp_in != ખાલી)
    {
	જો (flags & VIF_WANT_INFO)
	{
	    જો (fp_out != ખાલી)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != ખાલી);
	    merge = TRUE;
	}
	નહીંતર જો (flags != 0)
	    // Skip info, find start of marks
	    જ્યારે (!(eof = viminfo_readline(&vir))
		    && vir.vir_લીટી[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    જો (fp_out != ખાલી)
    {
	// Write the info:
	fprintf(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_out);
	fprintf(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_શોધ_pattern(fp_out);
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

	જો (do_copy_marks)
	    ga_init2(&buflist, કદ(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : ખાલી);
    }

    જો (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	જો (fp_out != ખાલી)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_લીટી);
    જો (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, ખાલી, ખાલી);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    પૂર્ણાંક
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    પૂર્ણાંક		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    જો (no_viminfo())
	પાછા FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    જો (fname == ખાલી)
	પાછા FAIL;
    fp = mch_fopen((અક્ષર *)fname, READBIN);

    જો (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == ખાલી ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    જો (fp == ખાલી)
	પાછા FAIL;
    જો (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	પાછા FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, ખાલી, flags);

    fclose(fp);
    પાછા OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    શૂન્ય
write_viminfo(char_u *file, પૂર્ણાંક forceit)
{
    char_u	*fname;
    FILE	*fp_in = ખાલી;	// input viminfo file, if any
    FILE	*fp_out = ખાલી;	// output viminfo file
    char_u	*tempname = ખાલી;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#જો defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    પૂર્ણાંક		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    પૂર્ણાંક		hidden = FALSE;
#endif

    જો (no_viminfo())
	પાછા;

    fname = viminfo_filename(file);	// may set to default if NULL
    જો (fname == ખાલી)
	પાછા;

    fp_in = mch_fopen((અક્ષર *)fname, READBIN);
    જો (fp_in == ખાલી)
    {
	પૂર્ણાંક fd;

	// if it does exist, but we can't read it, don't try writing
	જો (mch_stat((અક્ષર *)fname, &st_new) == 0)
	    જાઓ end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((અક્ષર *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	જો (fd < 0)
	    જાઓ end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    નહીંતર
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	જો (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != મૂળ_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    પૂર્ણાંક	tt = સંદેશ_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    સંદેશ_didany = tt;
	    fclose(fp_in);
	    જાઓ end;
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
	માટે (;;)
	{
	    પૂર્ણાંક		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#નહીંતર
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#નહીંતર
				    (char_u *)".tmp",
#endif
				    FALSE);
	    જો (tempname == ખાલી)		// out of memory
		તોડો;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    જો (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    માટે (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		જો (mch_stat((અક્ષર *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    જો (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			તોડો;
		    }
#endif
		}
		નહીંતર
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((અક્ષર *)tempname, WRITEBIN);
		    (શૂન્ય)umask(umask_save);
#નહીંતર
		    પૂર્ણાંક	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((અક્ષર *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(પૂર્ણાંક)((st_old.st_mode & 0777) | 0600));
		    (શૂન્ય)umask(umask_save);
# નહીંતર
		    fd = mch_open((અક્ષર *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    જો (fd < 0)
		    {
			fp_out = ખાલી;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			જો (errno != EEXIST)
			    તોડો;
# endif
		    }
		    નહીંતર
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    જો (fp_out != ખાલી)
			તોડો;
		}

		// Assume file exists, try again with another name.
		જો (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    તોડો;
		}
		*wp = next_char;
		--next_char;
	    }

	    જો (tempname != ખાલી)
		તોડો;
	    // continue if shortname was set
	}

#જો defined(UNIX) && defined(HAVE_FCHOWN)
	જો (tempname != ખાલી && fp_out != ખાલી)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    જો (mch_stat((અક્ષર *)tempname, &tmp_st) >= 0)
	    {
		જો (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		જો (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (શૂન્ય)mch_setperm(tempname, 0600);
	    }
	    નહીંતર
		// can't stat the file, set conservative permissions
		(શૂન્ય)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    જો (fp_out == ખાલી)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == ખાલી || tempname == ખાલી) ? fname : tempname);
	જો (fp_in != ખાલી)
	    fclose(fp_in);
	જાઓ end;
    }

    જો (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    જો (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    જો (fp_in != ખાલી)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	જો (viminfo_errcnt == 0)
	{
	    જો (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    નહીંતર જો (hidden)
		mch_hide(fname);
#endif
	}
	જો (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    શૂન્ય
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    જો (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    જો (eap->cmdidx == CMD_rviminfo)
    {
	જો (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    નહીંતર
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
