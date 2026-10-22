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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
തരനിർവചനം ഘടന
{
    char_u	*vir_വരി;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    പൂർണ്ണസംഖ്യ		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

തരനിർവചനം എണ്ണൽ {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

തരനിർവചനം ഘടന {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    പൂർണ്ണസംഖ്യ		bv_len;		// length of bv_string
    പൂർണ്ണസംഖ്യ		bv_allocated;	// bv_string was allocated
} bval_T;

#എങ്കിൽ defined(FEAT_VIMINFO)

സ്ഥിതം പൂർണ്ണസംഖ്യ  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    സ്ഥിതം char_u *
find_viminfo_parameter(പൂർണ്ണസംഖ്യ type)
{
    char_u  *p;

    വേണ്ടി (p = p_viminfo; *p; ++p)
    {
	എങ്കിൽ (*p == type)
	    തിരികെ p + 1;
	എങ്കിൽ (*p == 'n')		    // 'n' is always the last one
	    നിർത്തുക;
	p = vim_strchr(p, ',');	    // skip until next ','
	എങ്കിൽ (p == ശൂന്യ)		    // hit the end without finding parameter
	    നിർത്തുക;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    പൂർണ്ണസംഖ്യ
get_viminfo_parameter(പൂർണ്ണസംഖ്യ type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    എങ്കിൽ (p != ശൂന്യ && VIM_ISDIGIT(*p))
	തിരികെ atoi((അക്ഷരം *)p);
    തിരികെ -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    സ്ഥിതം char_u *
viminfo_filename(char_u *file)
{
    എങ്കിൽ (file == ശൂന്യ || *file == NUL)
    {
	size_t  len;

	എങ്കിൽ (*p_viminfofile != NUL)
	    file = p_viminfofile;
	അല്ലെങ്കിൽ എങ്കിൽ ((file = find_viminfo_parameter('n')) == ശൂന്യ || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    എങ്കിൽ (mch_getenv((char_u *)"SYS$LOGIN") == ശൂന്യ)
# അല്ലെങ്കിൽ
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    എങ്കിൽ (STRCMP(vim_getenv((char_u *)"HOME", ശൂന്യ), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == ശൂന്യ)
#  അല്ലെങ്കിൽ
	    എങ്കിൽ (mch_getenv((char_u *)"HOME") == ശൂന്യ)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		എങ്കിൽ (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		അല്ലെങ്കിൽ
		    file = (char_u *)VIMINFO_FILE;
	    }
	    അല്ലെങ്കിൽ
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	തിരികെ vim_strnsave(file, len);
    }

    തിരികെ vim_strsave(file);
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
    സ്ഥിതം ശൂന്യം
viminfo_writestring(FILE *fd, char_u *p)
{
    പൂർണ്ണസംഖ്യ		c;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		len = 0;

    വേണ്ടി (s = p; *s != NUL; ++s)
    {
	എങ്കിൽ (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    എങ്കിൽ (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    ആകുമ്പോൾ ((c = *p++) != NUL)
    {
	എങ്കിൽ (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    എങ്കിൽ (c == '\n')
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
barline_writestring(FILE *fd, char_u *s, പൂർണ്ണസംഖ്യ remaining_start)
{
    char_u *p;
    പൂർണ്ണസംഖ്യ	    remaining = remaining_start;
    പൂർണ്ണസംഖ്യ	    len = 2;

    // Count the number of characters produced, including quotes.
    വേണ്ടി (p = s; *p != NUL; ++p)
    {
	എങ്കിൽ (*p == NL)
	    len += 2;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '"' || *p == '\\')
	    len += 2;
	അല്ലെങ്കിൽ
	    ++len;
    }
    എങ്കിൽ (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    വേണ്ടി (p = s; *p != NUL; ++p)
    {
	എങ്കിൽ (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	അല്ലെങ്കിൽ
	    putc(*p, fd);
	--remaining;

	എങ്കിൽ (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    തിരികെ remaining - 2;
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
    സ്ഥിതം char_u *
viminfo_readstring(
    vir_T	*virp,
    പൂർണ്ണസംഖ്യ		off,		    // offset for virp->vir_line
    പൂർണ്ണസംഖ്യ		convert)	    // convert the string
{
    char_u	*retval = ശൂന്യ;
    char_u	*s, *d;
    long	len;

    എങ്കിൽ (virp->vir_വരി[off] == Ctrl_V && vim_isdigit(virp->vir_വരി[off + 1]))
    {
	len = atol((അക്ഷരം *)virp->vir_വരി + off + 1);
	എങ്കിൽ (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	എങ്കിൽ (retval == ശൂന്യ)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (ശൂന്യം)vim_fgets(virp->vir_വരി, 10, virp->vir_fd);
	    തിരികെ ശൂന്യ;
	}
	(ശൂന്യം)vim_fgets(retval, (പൂർണ്ണസംഖ്യ)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    അല്ലെങ്കിൽ
    {
	retval = vim_strsave(virp->vir_വരി + off);
	എങ്കിൽ (retval == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    ആകുമ്പോൾ (*s != NUL && *s != '\n')
    {
	എങ്കിൽ (s[0] == Ctrl_V && s[1] != NUL)
	{
	    എങ്കിൽ (s[1] == 'n')
		*d++ = '\n';
	    അല്ലെങ്കിൽ
		*d++ = Ctrl_V;
	    s += 2;
	}
	അല്ലെങ്കിൽ
	    *d++ = *s++;
    }
    *d = NUL;

    എങ്കിൽ (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, ശൂന്യ);
	എങ്കിൽ (d != ശൂന്യ)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    തിരികെ retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
viminfo_readline(vir_T *virp)
{
    തിരികെ vim_fgets(virp->vir_വരി, LSIZE, virp->vir_fd);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_viminfo_bufferlist(
    vir_T	*virp,
    പൂർണ്ണസംഖ്യ		writing)
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
    എങ്കിൽ (xline != ശൂന്യ && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != ശൂന്യ)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	എങ്കിൽ (tab != ശൂന്യ)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((അക്ഷരം *)tab);
	    tab = vim_strrchr(xline, '\t');
	    എങ്കിൽ (tab != ശൂന്യ)
	    {
		*tab++ = '\0';
		lnum = atol((അക്ഷരം *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	എങ്കിൽ (buf != ശൂന്യ)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    തിരികെ viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    പൂർണ്ണസംഖ്യ	    part_len;
    പൂർണ്ണസംഖ്യ	    retval = FALSE;

    name = home_replace_save(ശൂന്യ, name);
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ FALSE;
    വേണ്ടി (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, വലുപ്പം(part), ", ");
	എങ്കിൽ (part[0] == 'r')
	{
	    എങ്കിൽ (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		നിർത്തുക;
	    }
	}
    }
    vim_free(name);
    തിരികെ retval;
}

    സ്ഥിതം ശൂന്യം
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*വരി;
    പൂർണ്ണസംഖ്യ		max_buffers;

    എങ്കിൽ (find_viminfo_parameter('%') == ശൂന്യ)
	തിരികെ;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define വരി_BUF_LEN (MAXPATHL + 40)
    വരി = alloc(വരി_BUF_LEN);
    എങ്കിൽ (വരി == ശൂന്യ)
	തിരികെ;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	എങ്കിൽ (buf->b_fname == ശൂന്യ
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    തുടരുക;

	എങ്കിൽ (max_buffers-- == 0)
	    നിർത്തുക;
	putc('%', fp);
	linelen = home_replace(ശൂന്യ, buf->b_ffname, വരി, MAXPATHL, TRUE);
	vim_snprintf((അക്ഷരം *)വരി + linelen, വരി_BUF_LEN - linelen, "\t%ld\t%d",
			(long)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, വരി);
    }
    vim_free(വരി);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
സ്ഥിതം histentry_T *viminfo_history[HIST_എണ്ണം] =
					       {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};
സ്ഥിതം പൂർണ്ണസംഖ്യ	viminfo_hisidx[HIST_എണ്ണം] = {0, 0, 0, 0, 0};
സ്ഥിതം പൂർണ്ണസംഖ്യ	viminfo_hislen[HIST_എണ്ണം] = {0, 0, 0, 0, 0};
സ്ഥിതം പൂർണ്ണസംഖ്യ	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
hist_type2char(
    പൂർണ്ണസംഖ്യ	    type,
    പൂർണ്ണസംഖ്യ	    use_question)	    // use '?' instead of '/'
{
    എങ്കിൽ (type == HIST_CMD)
	തിരികെ ':';
    എങ്കിൽ (type == HIST_തിരയൽ)
    {
	എങ്കിൽ (use_question)
	    തിരികെ '?';
	അല്ലെങ്കിൽ
	    തിരികെ '/';
    }
    എങ്കിൽ (type == HIST_വ്യഞ്ജകം)
	തിരികെ '=';
    തിരികെ '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    സ്ഥിതം ശൂന്യം
prepare_viminfo_history(പൂർണ്ണസംഖ്യ asklen, പൂർണ്ണസംഖ്യ writing)
{
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    അക്കം;
    പൂർണ്ണസംഖ്യ	    type;
    പൂർണ്ണസംഖ്യ	    len;
    പൂർണ്ണസംഖ്യ	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    എങ്കിൽ (asklen > hislen)
	asklen = hislen;

    വേണ്ടി (type = 0; type < HIST_എണ്ണം; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	വേണ്ടി (i = 0, അക്കം = 0; i < hislen; i++)
	    എങ്കിൽ (histentry[i].hisstr == ശൂന്യ || histentry[i].viminfo)
		അക്കം++;
	len = asklen;
	എങ്കിൽ (അക്കം > len)
	    len = അക്കം;
	എങ്കിൽ (len <= 0)
	    viminfo_history[type] = ശൂന്യ;
	അല്ലെങ്കിൽ
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	എങ്കിൽ (viminfo_history[type] == ശൂന്യ)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_viminfo_history(vir_T *virp, പൂർണ്ണസംഖ്യ writing)
{
    പൂർണ്ണസംഖ്യ		type;
    long_u	len;
    char_u	*val = ശൂന്യ;
    char_u	*p;

    type = hist_char2type(virp->vir_വരി[0]);
    എങ്കിൽ (viminfo_hisidx[type] >= viminfo_hislen[type])
	പോകുക done;

    val = viminfo_readstring(virp, 1, TRUE);
    എങ്കിൽ (val == ശൂന്യ || *val == NUL)
	പോകുക done;

    പൂർണ്ണസംഖ്യ sep = (*val == ' ' ? NUL : *val);

    എങ്കിൽ (in_history(type, val + (type == HIST_തിരയൽ), viminfo_add_at_front,
								sep, writing))
	പോകുക done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    എങ്കിൽ (type == HIST_തിരയൽ)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	എങ്കിൽ (p == ശൂന്യ)
	    പോകുക done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    അല്ലെങ്കിൽ
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	എങ്കിൽ (p == ശൂന്യ)
	    പോകുക done;

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
    തിരികെ viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    സ്ഥിതം ശൂന്യം
handle_viminfo_history(
	garray_T    *മൂല്യങ്ങൾ,
	പൂർണ്ണസംഖ്യ	    writing)
{
    പൂർണ്ണസംഖ്യ		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)മൂല്യങ്ങൾ->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    എങ്കിൽ (മൂല്യങ്ങൾ->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	തിരികെ;

    type = vp[0].bv_nr;
    എങ്കിൽ (type >= HIST_എണ്ണം)
	തിരികെ;

    എങ്കിൽ (viminfo_hisidx[type] >= viminfo_hislen[type])
	തിരികെ;

    val = vp[3].bv_string;
    എങ്കിൽ (val == ശൂന്യ || *val == NUL)
	തിരികെ;

    പൂർണ്ണസംഖ്യ sep = type == HIST_തിരയൽ && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    പൂർണ്ണസംഖ്യ idx;
    പൂർണ്ണസംഖ്യ overwrite = FALSE;

    എങ്കിൽ (in_history(type, val, viminfo_add_at_front, sep, writing))
	തിരികെ;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    വേണ്ടി (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	എങ്കിൽ (STRCMP(val, p) == 0
		&& (type != HIST_തിരയൽ || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    നിർത്തുക;
	}
    }

    എങ്കിൽ (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    അല്ലെങ്കിൽ
	len = 0; // for picky compilers
    എങ്കിൽ (p != ശൂന്യ)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	എങ്കിൽ (!overwrite)
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
    സ്ഥിതം ശൂന്യം
concat_history(പൂർണ്ണസംഖ്യ type)
{
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    പൂർണ്ണസംഖ്യ		*hisidx = get_hisidx(type);
    പൂർണ്ണസംഖ്യ		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    എങ്കിൽ (idx >= hislen)
	idx -= hislen;
    അല്ലെങ്കിൽ എങ്കിൽ (idx < 0)
	idx = hislen - 1;
    എങ്കിൽ (viminfo_add_at_front)
	*hisidx = idx;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (*hisidx == -1)
	    *hisidx = hislen - 1;
	ചെയ്യുക
	{
	    എങ്കിൽ (histentry[idx].hisstr != ശൂന്യ || histentry[idx].viminfo)
		നിർത്തുക;
	    എങ്കിൽ (++idx == hislen)
		idx = 0;
	} ആകുമ്പോൾ (idx != *hisidx);
	എങ്കിൽ (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    വേണ്ടി (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	എങ്കിൽ (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    വേണ്ടി (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
sort_hist(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    എങ്കിൽ (p1->time_set < p2->time_set) തിരികെ -1;
    എങ്കിൽ (p1->time_set > p2->time_set) തിരികെ 1;
    തിരികെ 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    സ്ഥിതം ശൂന്യം
merge_history(പൂർണ്ണസംഖ്യ type)
{
    പൂർണ്ണസംഖ്യ		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    പൂർണ്ണസംഖ്യ		*hisidx = get_hisidx(type);
    പൂർണ്ണസംഖ്യ		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    എങ്കിൽ (tot_hist == ശൂന്യ || new_hist == ശൂന്യ)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	തിരികെ;
    }
    വേണ്ടി (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    വേണ്ടി (i = 0; i < hislen; i++)
	എങ്കിൽ (histentry[i].hisstr != ശൂന്യ)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((ശൂന്യം *)tot_hist, (size_t)len, വലുപ്പം(histentry_T *), sort_hist);

    // Keep the newest ones.
    വേണ്ടി (i = 0; i < hislen; i++)
    {
	എങ്കിൽ (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = ശൂന്യ;
	    tot_hist[i]->hisstrlen = 0;
	    എങ്കിൽ (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	അല്ലെങ്കിൽ
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    വേണ്ടി (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    വേണ്ടി (i = 0; i < hislen; i++)
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
    സ്ഥിതം ശൂന്യം
finish_viminfo_history(vir_T *virp)
{
    പൂർണ്ണസംഖ്യ	type;
    പൂർണ്ണസംഖ്യ merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    വേണ്ടി (type = 0; type < HIST_എണ്ണം; ++type)
    {
	എങ്കിൽ (get_histentry(type) == ശൂന്യ)
	    തുടരുക;

	എങ്കിൽ (merge)
	    merge_history(type);
	അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
write_viminfo_history(FILE *fp, പൂർണ്ണസംഖ്യ merge)
{
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    type;
    പൂർണ്ണസംഖ്യ	    അക്കം_saved;
    പൂർണ്ണസംഖ്യ     round;
    പൂർണ്ണസംഖ്യ	    hislen;

    init_history();
    hislen = get_hislen();
    എങ്കിൽ (hislen == 0)
	തിരികെ;
    വേണ്ടി (type = 0; type < HIST_എണ്ണം; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	പൂർണ്ണസംഖ്യ	    *hisidx = get_hisidx(type);

	അക്കം_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	എങ്കിൽ (അക്കം_saved == 0)
	    തുടരുക;
	എങ്കിൽ (അക്കം_saved < 0)  // Use default
	    അക്കം_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_തിരയൽ ? _("Search String") :
			    type == HIST_വ്യഞ്ജകം ? _("Expression") :
			    type == HIST_നിവേശം ? _("Input Line") :
					_("Debug Line"));
	എങ്കിൽ (അക്കം_saved > hislen)
	    അക്കം_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	വേണ്ടി (round = 1; round <= 2; ++round)
	{
	    എങ്കിൽ (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    അല്ലെങ്കിൽ എങ്കിൽ (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    അല്ലെങ്കിൽ
		// empty list
		i = -1;
	    എങ്കിൽ (i >= 0)
		ആകുമ്പോൾ (അക്കം_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    പൂർണ്ണസംഖ്യ	    c = NUL;

		    എങ്കിൽ (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (viminfo_history[type] == ശൂന്യ)
			{
			    p = ശൂന്യ;
			    plen = 0;
			    timestamp = 0;
			}
			അല്ലെങ്കിൽ
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    എങ്കിൽ (p != ശൂന്യ && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--അക്കം_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			എങ്കിൽ (type == HIST_തിരയൽ)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    അക്ഷരം    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    എങ്കിൽ (c == NUL)
				cbuf[0] = NUL;
			    അല്ലെങ്കിൽ
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    എങ്കിൽ (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			എങ്കിൽ (--i < 0)
			    i = hislen - 1;
			എങ്കിൽ (i == *hisidx)
			    നിർത്തുക;
		    }
		    അല്ലെങ്കിൽ
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	വേണ്ടി (i = 0; i < viminfo_hisidx[type]; ++i)
	    എങ്കിൽ (viminfo_history[type] != ശൂന്യ)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    സ്ഥിതം ശൂന്യം
write_viminfo_barlines(vir_T *virp, FILE *fp_out)
{
    പൂർണ്ണസംഖ്യ		i;
    garray_T	*gap = &virp->vir_barlines;
    പൂർണ്ണസംഖ്യ		seen_useful = FALSE;
    അക്ഷരം	*വരി;

    എങ്കിൽ (gap->ga_len <= 0)
	തിരികെ;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
    {
	വരി = ((അക്ഷരം **)(gap->ga_data))[i];
	എങ്കിൽ (seen_useful || വരി[1] != '<')
	{
	    fputs(വരി, fp_out);
	    seen_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
barline_parse(vir_T *virp, char_u *text, garray_T *മൂല്യങ്ങൾ)
{
    char_u  *p = text;
    char_u  *nextp = ശൂന്യ;
    char_u  *buf = ശൂന്യ;
    bval_T  *മൂല്യം;
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    allocated = FALSE;
    പൂർണ്ണസംഖ്യ	    eof;
    char_u  *sconv;
    പൂർണ്ണസംഖ്യ	    converted;

    ആകുമ്പോൾ (*p == ',')
    {
	++p;
	എങ്കിൽ (ga_grow(മൂല്യങ്ങൾ, 1) == FAIL)
	    നിർത്തുക;
	മൂല്യം = (bval_T *)(മൂല്യങ്ങൾ->ga_data) + മൂല്യങ്ങൾ->ga_len;

	എങ്കിൽ (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    എങ്കിൽ (!allocated)
	    {
		വേണ്ടി (i = 0; i < മൂല്യങ്ങൾ->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(മൂല്യങ്ങൾ->ga_data) + i;

		    എങ്കിൽ (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    എങ്കിൽ (vim_isdigit(p[1]))
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
		എങ്കിൽ (buf == ശൂന്യ)
		    തിരികെ TRUE;
		p = buf;
		വേണ്ടി (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    എങ്കിൽ (eof || virp->vir_വരി[0] != '|'
						  || virp->vir_വരി[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			തിരികെ eof || virp->vir_വരി[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_വരി);
		    ആകുമ്പോൾ (n > 2 && (virp->vir_വരി[n - 1] == NL
					     || virp->vir_വരി[n - 1] == CAR))
			--n;
		    n -= 2;
		    എങ്കിൽ (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_വരി + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_വരി + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		എങ്കിൽ (eof || virp->vir_വരി[0] != '|'
					      || virp->vir_വരി[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    തിരികെ eof || virp->vir_വരി[0] == '|';
		p = virp->vir_വരി + 2;
	    }
	}

	എങ്കിൽ (SAFE_isdigit(*p))
	{
	    മൂല്യം->bv_type = BVAL_NR;
	    മൂല്യം->bv_nr = getdigits(&p);
	    ++മൂല്യങ്ങൾ->ga_len;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '"')
	{
	    പൂർണ്ണസംഖ്യ	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    ആകുമ്പോൾ (*p != '"')
	    {
		എങ്കിൽ (*p == NL || *p == NUL)
		    തിരികെ TRUE;  // syntax error, drop the value
		എങ്കിൽ (*p == '\\')
		{
		    ++p;
		    എങ്കിൽ (*p == 'n')
			s[len++] = '\n';
		    അല്ലെങ്കിൽ
			s[len++] = *p;
		    ++p;
		}
		അല്ലെങ്കിൽ
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    മൂല്യം->bv_tofree = ശൂന്യ;
	    എങ്കിൽ (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, ശൂന്യ);
		എങ്കിൽ (sconv != ശൂന്യ)
		{
		    എങ്കിൽ (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			മൂല്യം->bv_tofree = buf;
		    s = sconv;
		    len = (പൂർണ്ണസംഖ്യ)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    എങ്കിൽ (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    മൂല്യം->bv_string = s;
	    മൂല്യം->bv_type = BVAL_STRING;
	    മൂല്യം->bv_len = len;
	    മൂല്യം->bv_allocated = allocated || converted;
	    ++മൂല്യങ്ങൾ->ga_len;
	    എങ്കിൽ (nextp != ശൂന്യ)
	    {
		// values following a long string
		p = nextp;
		nextp = ശൂന്യ;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == ',')
	{
	    മൂല്യം->bv_type = BVAL_EMPTY;
	    ++മൂല്യങ്ങൾ->ga_len;
	}
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }
    തിരികെ TRUE;
}

    സ്ഥിതം ശൂന്യം
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
no_viminfo(ശൂന്യം)
{
    // "vim -i NONE" does not read or write a viminfo file
    തിരികെ STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
viminfo_error(അക്ഷരം *errnum, അക്ഷരം *message, char_u *വരി)
{
    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, വരി, IOSIZE - STRLEN(IObuff) - 1);
    എങ്കിൽ (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((അക്ഷരം *)IObuff);
    എങ്കിൽ (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_വരി, '=');
	എങ്കിൽ (p != ശൂന്യ)
	{
	    // remove trailing newline
	    ++p;
	    വേണ്ടി (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    തിരികെ viminfo_readline(virp);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_viminfo_varlist(vir_T *virp, പൂർണ്ണസംഖ്യ writing)
{
    char_u	*tab;
    പൂർണ്ണസംഖ്യ		type = VAR_സംഖ്യ;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    എങ്കിൽ (!writing && (find_viminfo_parameter('!') != ശൂന്യ))
    {
	tab = vim_strchr(virp->vir_വരി + 1, '\t');
	എങ്കിൽ (tab != ശൂന്യ)
	{
	    *tab++ = '\0';	// isolate the variable name
	    തിരഞ്ഞെടുപ്പ് (*tab)
	    {
		അവസ്ഥ 'S': type = VAR_STRING; നിർത്തുക;
		അവസ്ഥ 'F': type = VAR_FLOAT; നിർത്തുക;
		അവസ്ഥ 'D': type = VAR_DICT; നിർത്തുക;
		അവസ്ഥ 'L': type = VAR_LIST; നിർത്തുക;
		അവസ്ഥ 'B': type = VAR_BLOB; നിർത്തുക;
		അവസ്ഥ 'X': type = VAR_SPECIAL; നിർത്തുക;
		അവസ്ഥ 'T': type = VAR_TUPLE; നിർത്തുക;
	    }

	    tab = vim_strchr(tab, '\t');
	    എങ്കിൽ (tab != ശൂന്യ)
	    {
		tv.v_type = type;
		എങ്കിൽ (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (പൂർണ്ണസംഖ്യ)(tab - virp->vir_വരി + 1), TRUE);
		അല്ലെങ്കിൽ എങ്കിൽ (type == VAR_FLOAT)
		    (ശൂന്യം)string2float(tab + 1, &tv.vval.v_float, FALSE);
		അല്ലെങ്കിൽ
		{
		    tv.vval.v_സംഖ്യ = atol((അക്ഷരം *)tab + 1);
		    എങ്കിൽ (type == VAR_SPECIAL && (tv.vval.v_സംഖ്യ == VVAL_FALSE
					     || tv.vval.v_സംഖ്യ == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		എങ്കിൽ (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_വ്യഞ്ജകം(tv.vval.v_string, ശൂന്യ);

		    എങ്കിൽ (etv == ശൂന്യ)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    അല്ലെങ്കിൽ
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    എങ്കിൽ (blob == ശൂന്യ)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    അല്ലെങ്കിൽ
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_entry);
		set_var(virp->vir_വരി + 1, &tv, FALSE);
		restore_funccal();

		എങ്കിൽ (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		അല്ലെങ്കിൽ എങ്കിൽ (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    തിരികെ viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    സ്ഥിതം ശൂന്യം
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    പൂർണ്ണസംഖ്യ		todo;
    അക്ഷരം	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    എങ്കിൽ (find_viminfo_parameter('!') == ശൂന്യ)
	തിരികെ;

    fputs(_("\n# global variables:\n"), fp);

    todo = (പൂർണ്ണസംഖ്യ)gvht->ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(gvht, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    എങ്കിൽ (var_flavour(this_var->di_താക്കോൽ) == VAR_FLAVOUR_VIMINFO)
	    {
		തിരഞ്ഞെടുപ്പ് (this_var->di_tv.v_type)
		{
		    അവസ്ഥ VAR_STRING:  s = "STR"; നിർത്തുക;
		    അവസ്ഥ VAR_സംഖ്യ:  s = "NUM"; നിർത്തുക;
		    അവസ്ഥ VAR_FLOAT:   s = "FLO"; നിർത്തുക;
		    അവസ്ഥ VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      പൂർണ്ണസംഖ്യ	copyID = get_copyID();

			      s = "DIC";
			      എങ്കിൽ (di != ശൂന്യ && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, ശൂന്യ, ശൂന്യ)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  തുടരുക;
			      നിർത്തുക;
			  }
		    അവസ്ഥ VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      പൂർണ്ണസംഖ്യ	copyID = get_copyID();

			      s = "LIS";
			      എങ്കിൽ (l != ശൂന്യ && !set_ref_in_list_ഇനങ്ങൾ(
						       l, copyID, ശൂന്യ, ശൂന്യ)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  തുടരുക;
			      നിർത്തുക;
			  }
		    അവസ്ഥ VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      പൂർണ്ണസംഖ്യ	copyID = get_copyID();

			      s = "TUP";
			      എങ്കിൽ (tuple != ശൂന്യ && !set_ref_in_tuple_ഇനങ്ങൾ(
					       tuple, copyID, ശൂന്യ, ശൂന്യ)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  തുടരുക;
			      നിർത്തുക;
			  }
		    അവസ്ഥ VAR_BLOB:    s = "BLO"; നിർത്തുക;
		    അവസ്ഥ VAR_BOOL:    s = "XPL"; നിർത്തുക;  // backwards compat.
		    അവസ്ഥ VAR_SPECIAL: s = "XPL"; നിർത്തുക;

		    അവസ്ഥ VAR_UNKNOWN:
		    അവസ്ഥ VAR_ANY:
		    അവസ്ഥ VAR_VOID:
		    അവസ്ഥ VAR_FUNC:
		    അവസ്ഥ VAR_PARTIAL:
		    അവസ്ഥ VAR_JOB:
		    അവസ്ഥ VAR_CHANNEL:
		    അവസ്ഥ VAR_INSTR:
		    അവസ്ഥ VAR_CLASS:
		    അവസ്ഥ VAR_OBJECT:
		    അവസ്ഥ VAR_TYPEALIAS:
				      തുടരുക;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_താക്കോൽ, s);
		എങ്കിൽ (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((അക്ഷരം *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_സംഖ്യ);
		    p = numbuf;
		    tofree = ശൂന്യ;
		}
		അല്ലെങ്കിൽ
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		എങ്കിൽ (p != ശൂന്യ)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_viminfo_sub_string(vir_T *virp, പൂർണ്ണസംഖ്യ force)
{
    എങ്കിൽ (force || get_old_sub() == ശൂന്യ)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    തിരികെ viminfo_readline(virp);
}

    സ്ഥിതം ശൂന്യം
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    എങ്കിൽ (get_viminfo_parameter('/') == 0 || old_sub == ശൂന്യ)
	തിരികെ;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_viminfo_തിരയൽ_pattern(vir_T *virp, പൂർണ്ണസംഖ്യ force, പൂർണ്ണസംഖ്യ writing)
{
    char_u	*lp;
    പൂർണ്ണസംഖ്യ		idx = -1;
    പൂർണ്ണസംഖ്യ		magic = FALSE;
    പൂർണ്ണസംഖ്യ		no_scs = FALSE;
    പൂർണ്ണസംഖ്യ		off_വരി = FALSE;
    പൂർണ്ണസംഖ്യ		off_end = 0;
    long	off = 0;
    പൂർണ്ണസംഖ്യ		setlast = FALSE;
#ifdef FEAT_തിരയൽ_EXTRA
    സ്ഥിതം പൂർണ്ണസംഖ്യ	hlsearch_on = FALSE;
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
    lp = virp->vir_വരി;
    എങ്കിൽ (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	എങ്കിൽ (lp[1] == 'M')		// magic on
	    magic = TRUE;
	എങ്കിൽ (lp[2] == 's')
	    no_scs = TRUE;
	എങ്കിൽ (lp[3] == 'L')
	    off_വരി = TRUE;
	എങ്കിൽ (lp[4] == 'E')
	    off_end = തിരയൽ_END;
	lp += 5;
	off = getdigits(&lp);
    }
    എങ്കിൽ (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    എങ്കിൽ (lp[0] == '/')
	idx = RE_തിരയൽ;
    അല്ലെങ്കിൽ എങ്കിൽ (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_തിരയൽ_EXTRA
    അല്ലെങ്കിൽ എങ്കിൽ (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    എങ്കിൽ (idx >= 0)
    {
	spat = get_spat(idx);
	എങ്കിൽ (force || spat->pat == ശൂന്യ)
	{
	    val = viminfo_readstring(virp, (പൂർണ്ണസംഖ്യ)(lp - virp->vir_വരി + 1),
									TRUE);
	    എങ്കിൽ (val != ശൂന്യ)
	    {
		എങ്കിൽ (!writing)
		{
		    set_last_തിരയൽ_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.വരി = off_വരി;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_തിരയൽ_EXTRA
		    എങ്കിൽ (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    തിരികെ viminfo_readline(virp);
}

    സ്ഥിതം ശൂന്യം
wvsp_one(
    FILE	*fp,	// file to write to
    പൂർണ്ണസംഖ്യ		idx,	// spats[] index
    അക്ഷരം	*s,	// search pat
    പൂർണ്ണസംഖ്യ		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    എങ്കിൽ (spat->pat == ശൂന്യ)
	തിരികെ;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.വരി ? 'L' : 'l',	// line offset
	    spat->off.end  ? 'E' : 'e',	// offset from end
	    spat->off.off,			// offset
	    get_spat_last_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    സ്ഥിതം ശൂന്യം
write_viminfo_തിരയൽ_pattern(FILE *fp)
{
    എങ്കിൽ (get_viminfo_parameter('/') == 0)
	തിരികെ;

#ifdef FEAT_തിരയൽ_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != ശൂന്യ) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_തിരയൽ, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

സ്ഥിതം yankreg_T *y_read_regs = ശൂന്യ;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    സ്ഥിതം ശൂന്യം
prepare_viminfo_registers(ശൂന്യം)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, അക്കം_REGISTERS);
}

    സ്ഥിതം ശൂന്യം
finish_viminfo_registers(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		j;

    എങ്കിൽ (y_read_regs == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (i = 0; i < അക്കം_REGISTERS; ++i)
	എങ്കിൽ (y_read_regs[i].y_array != ശൂന്യ)
	{
	    വേണ്ടി (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_viminfo_register(vir_T *virp, പൂർണ്ണസംഖ്യ force)
{
    പൂർണ്ണസംഖ്യ		eof;
    പൂർണ്ണസംഖ്യ		do_it = TRUE;
    പൂർണ്ണസംഖ്യ		size;
    പൂർണ്ണസംഖ്യ		limit;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		set_prev = FALSE;
    char_u	*str;
    string_T	*array = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_വരി + 1;

    // If the line starts with "" this is the y_previous register.
    എങ്കിൽ (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    എങ്കിൽ (!ASCII_ISALNUM(*str) && *str != '-')
    {
	എങ്കിൽ (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_വരി))
	    തിരികെ TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    എങ്കിൽ (!force && y_current_p->y_array != ശൂന്യ)
	do_it = FALSE;

    എങ്കിൽ (*str == '@')
    {
	// "x@: register x used for @@
	എങ്കിൽ (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    എങ്കിൽ (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	എങ്കിൽ (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	എങ്കിൽ (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	അല്ലെങ്കിൽ
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    ആകുമ്പോൾ (!(eof = viminfo_readline(virp))
		    && (virp->vir_വരി[0] == TAB || virp->vir_വരി[0] == '<'))
    {
	എങ്കിൽ (do_it)
	{
	    എങ്കിൽ (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * വലുപ്പം(string_T));

		എങ്കിൽ (new_array == ശൂന്യ)
		{
		    do_it = FALSE;
		    നിർത്തുക;
		}
		വേണ്ടി (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    എങ്കിൽ (str != ശൂന്യ)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    അല്ലെങ്കിൽ
		// error, don't store the result
		do_it = FALSE;
	}
    }

    എങ്കിൽ (do_it)
    {
	// free y_array[]
	വേണ്ടി (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	എങ്കിൽ (size == 0)
	{
	    y_current_p->y_array = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    വേണ്ടി (i = 0; i < size; i++)
	    {
		എങ്കിൽ (y_current_p->y_array == ശൂന്യ)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		അല്ലെങ്കിൽ
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	// Free array[] if it was filled.
	വേണ്ടി (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    തിരികെ eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    സ്ഥിതം ശൂന്യം
handle_viminfo_register(garray_T *മൂല്യങ്ങൾ, പൂർണ്ണസംഖ്യ force)
{
    bval_T	*vp = (bval_T *)മൂല്യങ്ങൾ->ga_data;
    പൂർണ്ണസംഖ്യ		flags;
    പൂർണ്ണസംഖ്യ		name;
    പൂർണ്ണസംഖ്യ		type;
    പൂർണ്ണസംഖ്യ		linecount;
    പൂർണ്ണസംഖ്യ		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    പൂർണ്ണസംഖ്യ		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    എങ്കിൽ (മൂല്യങ്ങൾ->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	തിരികെ;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    എങ്കിൽ (name < 0 || name >= അക്കം_REGISTERS)
	തിരികെ;
    type = vp[2].bv_nr;
    എങ്കിൽ (type != MCHAR && type != MLINE && type != MBLOCK)
	തിരികെ;
    linecount = vp[3].bv_nr;
    എങ്കിൽ (മൂല്യങ്ങൾ->ga_len < 6 + linecount)
	തിരികെ;
    width = vp[4].bv_nr;
    എങ്കിൽ (width < 0)
	തിരികെ;

    എങ്കിൽ (y_read_regs != ശൂന്യ)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    അല്ലെങ്കിൽ
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    എങ്കിൽ (y_ptr->y_array != ശൂന്യ && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	തിരികെ;

    എങ്കിൽ (y_ptr->y_array != ശൂന്യ)
	വേണ്ടി (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    എങ്കിൽ (y_read_regs == ശൂന്യ)
    {
	എങ്കിൽ (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	എങ്കിൽ ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    എങ്കിൽ (linecount == 0)
    {
	y_ptr->y_array = ശൂന്യ;
	തിരികെ;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    എങ്കിൽ (y_ptr->y_array == ശൂന്യ)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	തിരികെ;
    }
    വേണ്ടി (i = 0; i < linecount; i++)
    {
	എങ്കിൽ (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = ശൂന്യ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vp[i + 6].bv_type != BVAL_STRING)
	{
	    സ്വതന്ത്രം(y_ptr->y_array);
	    y_ptr->y_array = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    സ്ഥിതം ശൂന്യം
write_viminfo_registers(FILE *fp)
{
    പൂർണ്ണസംഖ്യ		i, j;
    char_u	*type;
    char_u	c;
    പൂർണ്ണസംഖ്യ		അക്കം_വരികൾ;
    പൂർണ്ണസംഖ്യ		max_അക്കം_വരികൾ;
    പൂർണ്ണസംഖ്യ		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_അക്കം_വരികൾ = get_viminfo_parameter('<');
    എങ്കിൽ (max_അക്കം_വരികൾ < 0)
	max_അക്കം_വരികൾ = get_viminfo_parameter('"');
    എങ്കിൽ (max_അക്കം_വരികൾ == 0)
	തിരികെ;
    max_kbyte = get_viminfo_parameter('s');
    എങ്കിൽ (max_kbyte == 0)
	തിരികെ;

    വേണ്ടി (i = 0; i < അക്കം_REGISTERS; i++)
    {
#എങ്കിൽ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	എങ്കിൽ (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    തുടരുക;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	എങ്കിൽ (i == TILDE_REGISTER)
	    തുടരുക;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	എങ്കിൽ (y_read_regs != ശൂന്യ
		&& y_read_regs[i].y_array != ശൂന്യ
		&& (y_regs_p[i].y_array == ശൂന്യ ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	അല്ലെങ്കിൽ എങ്കിൽ (y_regs_p[i].y_array == ശൂന്യ)
	    തുടരുക;
	അല്ലെങ്കിൽ
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	അക്കം_വരികൾ = y_ptr->y_size;
	എങ്കിൽ (അക്കം_വരികൾ == 0
		|| (അക്കം_വരികൾ == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    തുടരുക;

	എങ്കിൽ (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    വേണ്ടി (j = 0; j < അക്കം_വരികൾ; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    എങ്കിൽ (len > (long)max_kbyte * 1024L)
		തുടരുക;
	}

	തിരഞ്ഞെടുപ്പ് (y_ptr->y_type)
	{
	    അവസ്ഥ MLINE:
		type = (char_u *)"LINE";
		നിർത്തുക;
	    അവസ്ഥ MCHAR:
		type = (char_u *)"CHAR";
		നിർത്തുക;
	    അവസ്ഥ MBLOCK:
		type = (char_u *)"BLOCK";
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		നിർത്തുക;
	}
	എങ്കിൽ (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	എങ്കിൽ (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (പൂർണ്ണസംഖ്യ)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	എങ്കിൽ (max_അക്കം_വരികൾ > 0 && അക്കം_വരികൾ > max_അക്കം_വരികൾ)
	    അക്കം_വരികൾ = max_അക്കം_വരികൾ;
	വേണ്ടി (j = 0; j < അക്കം_വരികൾ; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    പൂർണ്ണസംഖ്യ	    flags = 0;
	    പൂർണ്ണസംഖ്യ	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    എങ്കിൽ (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    എങ്കിൽ (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, അക്കം_വരികൾ, (പൂർണ്ണസംഖ്യ)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    വേണ്ടി (j = 0; j < അക്കം_വരികൾ; j++)
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

സ്ഥിതം xfmark_T *vi_namedfm = ശൂന്യ;
സ്ഥിതം xfmark_T *vi_jumplist = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ vi_jumplist_len = 0;

    സ്ഥിതം ശൂന്യം
write_one_mark(FILE *fp_out, പൂർണ്ണസംഖ്യ c, pos_T *pos)
{
    എങ്കിൽ (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (പൂർണ്ണസംഖ്യ)pos->col);
}

    സ്ഥിതം ശൂന്യം
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    പൂർണ്ണസംഖ്യ		i;
    pos_T	pos;

    home_replace(ശൂന്യ, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_തിരുകുക);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    വേണ്ടി (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	എങ്കിൽ (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    വേണ്ടി (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
skip_for_viminfo(buf_T *buf)
{
    തിരികെ bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    സ്ഥിതം ശൂന്യം
write_viminfo_marks(FILE *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		is_mark_set;
    പൂർണ്ണസംഖ്യ		i;
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
	എങ്കിൽ (buf->b_marks_read)
	{
	    എങ്കിൽ (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    അല്ലെങ്കിൽ
	    {
		is_mark_set = FALSE;
		വേണ്ടി (i = 0; i < NMARKS; i++)
		    എങ്കിൽ (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			നിർത്തുക;
		    }
	    }
	    എങ്കിൽ (is_mark_set && buf->b_ffname != ശൂന്യ
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		എങ്കിൽ (buflist == ശൂന്യ)
		    write_buffer_marks(buf, fp_out);
		അല്ലെങ്കിൽ എങ്കിൽ (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    സ്ഥിതം ശൂന്യം
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    പൂർണ്ണസംഖ്യ		c1,
    പൂർണ്ണസംഖ്യ		c2)
{
    char_u	*name;

    എങ്കിൽ (fm->fmark.mark.lnum == 0)	// not set
	തിരികെ;

    എങ്കിൽ (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    അല്ലെങ്കിൽ
	name = fm->fname;		// use name from .viminfo
    എങ്കിൽ (name != ശൂന്യ && *name != NUL)
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

    എങ്കിൽ (fm->fmark.fnum != 0)
	vim_free(name);
}

    സ്ഥിതം ശൂന്യം
write_viminfo_filemarks(FILE *fp)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    പൂർണ്ണസംഖ്യ		vi_idx;
    പൂർണ്ണസംഖ്യ		idx;

    എങ്കിൽ (get_viminfo_parameter('f') == 0)
	തിരികെ;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    വേണ്ടി (i = 0; i < NMARKS; i++)
    {
	എങ്കിൽ (vi_namedfm != ശൂന്യ
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	അല്ലെങ്കിൽ
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    എങ്കിൽ (curbuf->b_ffname != ശൂന്യ && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	വേണ്ടി (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    എങ്കിൽ (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == ശൂന്യ
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != ശൂന്യ
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		നിർത്തുക;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	വേണ്ടി ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = ശൂന്യ;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    വേണ്ടി (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != ശൂന്യ ? &vi_namedfm[vi_idx] : ശൂന്യ;

	എങ്കിൽ (vi_fm != ശൂന്യ
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	അല്ലെങ്കിൽ
	{
	    fm = &namedfm_p[idx++];
	    എങ്കിൽ (vi_fm != ശൂന്യ
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != ശൂന്യ
			  && fm->fname != ശൂന്യ
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
    വേണ്ടി (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : ശൂന്യ;
	vi_fm = (vi_jumplist != ശൂന്യ && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : ശൂന്യ;
	എങ്കിൽ (fm == ശൂന്യ && vi_fm == ശൂന്യ)
	    നിർത്തുക;
	എങ്കിൽ (fm == ശൂന്യ || (vi_fm != ശൂന്യ && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	അല്ലെങ്കിൽ
	    --idx;
	എങ്കിൽ (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != ശൂന്യ
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    പൂർണ്ണസംഖ്യ
buf_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    എങ്കിൽ (buf1->b_last_used == buf2->b_last_used)
	തിരികെ 0;
    തിരികെ buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    സ്ഥിതം ശൂന്യം
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_out,
    garray_T	*buflist,
    പൂർണ്ണസംഖ്യ		eof,
    പൂർണ്ണസംഖ്യ		flags)
{
    char_u	*വരി = virp->vir_വരി;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		അക്കം_marked_files;
    പൂർണ്ണസംഖ്യ		load_marks;
    പൂർണ്ണസംഖ്യ		copy_marks_out;
    char_u	*str;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = ശൂന്യ;
#endif
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		buflist_used = 0;
    buf_T	*buflist_buf = ശൂന്യ;

    എങ്കിൽ ((name_buf = alloc(LSIZE)) == ശൂന്യ)
	തിരികെ;
    *name_buf = NUL;

    എങ്കിൽ (fp_out != ശൂന്യ && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						വലുപ്പം(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    എങ്കിൽ (fp_out == ശൂന്യ && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	എങ്കിൽ (list != ശൂന്യ)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    അക്കം_marked_files = get_viminfo_parameter('\'');
    ആകുമ്പോൾ (!eof && (എണ്ണം < അക്കം_marked_files || fp_out == ശൂന്യ))
    {
	എങ്കിൽ (വരി[0] != '>')
	{
	    എങ്കിൽ (വരി[0] != '\n' && വരി[0] != '\r' && വരി[0] != '#')
	    {
		എങ്കിൽ (viminfo_error("E576: ", _(e_nonr_missing_gt), വരി))
		    നിർത്തുക;	// too many errors, return now
	    }
	    eof = vim_fgets(വരി, LSIZE, virp->vir_fd);
	    തുടരുക;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(വരി + 1);
	str = viminfo_readstring(virp, (പൂർണ്ണസംഖ്യ)(str - virp->vir_വരി), FALSE);
	എങ്കിൽ (str == ശൂന്യ)
	    തുടരുക;
	p = str + STRLEN(str);
	ആകുമ്പോൾ (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	എങ്കിൽ (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	എങ്കിൽ (list != ശൂന്യ)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	എങ്കിൽ (fp_out == ശൂന്യ)
	{
	    എങ്കിൽ ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != ശൂന്യ)
	    {
		എങ്കിൽ (*name_buf == NUL)	    // only need to do this once
		    home_replace(ശൂന്യ, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		എങ്കിൽ (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	അല്ലെങ്കിൽ // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		എങ്കിൽ (buf->b_ffname != ശൂന്യ)
		{
		    home_replace(ശൂന്യ, buf->b_ffname, name_buf, LSIZE, TRUE);
		    എങ്കിൽ (fnamecmp(str, name_buf) == 0)
			നിർത്തുക;
		}

	    // Copy marks if the buffer has not been loaded.
	    എങ്കിൽ (buf == ശൂന്യ || !buf->b_marks_read)
	    {
		പൂർണ്ണസംഖ്യ	did_read_വരി = FALSE;

		എങ്കിൽ (buflist_buf != ശൂന്യ)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    എങ്കിൽ (!viminfo_readline(virp) && വരി[0] == TAB)
		    {
			did_read_വരി = TRUE;
			എങ്കിൽ (വരി[1] == '*')
			{
			    long	ltime;

			    sscanf((അക്ഷരം *)വരി + 2, "%ld ", &ltime);
			    ആകുമ്പോൾ ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				എങ്കിൽ (++എണ്ണം >= അക്കം_marked_files)
				    നിർത്തുക;
				എങ്കിൽ (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = ശൂന്യ;
				    നിർത്തുക;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			അല്ലെങ്കിൽ
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    ആകുമ്പോൾ (എണ്ണം < അക്കം_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++എണ്ണം;
			    }
			    buflist_buf = ശൂന്യ;
			}

			എങ്കിൽ (എണ്ണം >= അക്കം_marked_files)
			{
			    vim_free(str);
			    നിർത്തുക;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		എങ്കിൽ (did_read_വരി)
		    fputs((അക്ഷരം *)വരി, fp_out);

		എണ്ണം++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	ആകുമ്പോൾ (!(eof = viminfo_readline(virp)) && വരി[0] == TAB)
	{
	    എങ്കിൽ (load_marks)
	    {
		എങ്കിൽ (വരി[1] != NUL)
		{
		    unsigned u;

		    sscanf((അക്ഷരം *)വരി + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    തിരഞ്ഞെടുപ്പ് (വരി[1])
		    {
			അവസ്ഥ '"': curbuf->b_last_cursor = pos; നിർത്തുക;
			അവസ്ഥ '^': curbuf->b_last_തിരുകുക = pos; നിർത്തുക;
			അവസ്ഥ '.': curbuf->b_last_change = pos; നിർത്തുക;
			അവസ്ഥ '+':
				  // changelist positions are stored oldest
				  // first
				  എങ്കിൽ (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    വലുപ്പം(pos_T) * (JUMPLISTSIZE - 1));
				  അല്ലെങ്കിൽ
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  നിർത്തുക;

				  // Using the line number for the last-used
				  // timestamp.
			അവസ്ഥ '*': curbuf->b_last_used = pos.lnum; നിർത്തുക;

			സ്ഥിരസ്ഥിതി:  എങ്കിൽ ((i = വരി[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (copy_marks_out)
		fputs((അക്ഷരം *)വരി, fp_out);
	}

	എങ്കിൽ (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		എങ്കിൽ (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    എങ്കിൽ (flags & VIF_ONLY_CURBUF)
		നിർത്തുക;
	}
    }

    എങ്കിൽ (fp_out != ശൂന്യ)
	// Write any remaining entries from buflist.
	ആകുമ്പോൾ (എണ്ണം < അക്കം_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++എണ്ണം;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    ശൂന്യം
check_marks_read(ശൂന്യം)
{
    എങ്കിൽ (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != ശൂന്യ)
	read_viminfo(ശൂന്യ, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_viminfo_filemark(vir_T *virp, പൂർണ്ണസംഖ്യ force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    പൂർണ്ണസംഖ്യ		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_വരി + 1;
    എങ്കിൽ (*str <= 127
	    && ((*virp->vir_വരി == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_വരി == '-' && *str == '\'')))
    {
	എങ്കിൽ (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    എങ്കിൽ (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		വേണ്ടി (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = ശൂന്യ;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	അല്ലെങ്കിൽ
	    fm = &namedfm_p[*str - 'A'];
	എങ്കിൽ (fm != ശൂന്യ && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (പൂർണ്ണസംഖ്യ)(str - virp->vir_വരി),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    തിരികെ vim_fgets(virp->vir_വരി, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    സ്ഥിതം ശൂന്യം
prepare_viminfo_marks(ശൂന്യം)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    സ്ഥിതം ശൂന്യം
finish_viminfo_marks(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (vi_namedfm != ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    എങ്കിൽ (vi_jumplist != ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    സ്ഥിതം ശൂന്യം
handle_viminfo_mark(garray_T *മൂല്യങ്ങൾ, പൂർണ്ണസംഖ്യ force)
{
    bval_T	*vp = (bval_T *)മൂല്യങ്ങൾ->ga_data;
    പൂർണ്ണസംഖ്യ		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = ശൂന്യ;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    എങ്കിൽ (മൂല്യങ്ങൾ->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	തിരികെ;

    name = vp[0].bv_nr;
    എങ്കിൽ (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	തിരികെ;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    എങ്കിൽ (lnum <= 0 || col < 0)
	തിരികെ;
    timestamp = (time_t)vp[3].bv_nr;

    എങ്കിൽ (name == '\'')
    {
	എങ്കിൽ (vi_jumplist != ശൂന്യ)
	{
	    എങ്കിൽ (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ idx;
	    പൂർണ്ണസംഖ്യ i;

	    // If we have a timestamp insert it in the right place.
	    എങ്കിൽ (timestamp != 0)
	    {
		വേണ്ടി (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    എങ്കിൽ (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			നിർത്തുക;
		    }
		// idx cannot be zero now
		എങ്കിൽ (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    അല്ലെങ്കിൽ
		idx = -1;

	    എങ്കിൽ (idx >= 0)
	    {
		എങ്കിൽ (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    വേണ്ടി (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		അല്ലെങ്കിൽ
		{
		    // Move newer entries forward.
		    വേണ്ടി (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = ശൂന്യ;
		fm->time_set = 0;
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	എങ്കിൽ (VIM_ISDIGIT(name))
	{
	    എങ്കിൽ (vi_namedfm != ശൂന്യ)
		idx = name - '0' + NMARKS;
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		വേണ്ടി (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    എങ്കിൽ (namedfm_p[idx].time_set < timestamp)
			നിർത്തുക;
		എങ്കിൽ (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    തിരികെ;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		വേണ്ടി ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = ശൂന്യ;
	    }
	}
	അല്ലെങ്കിൽ
	    idx = name - 'A';
	എങ്കിൽ (vi_namedfm != ശൂന്യ)
	    fm = &vi_namedfm[idx];
	അല്ലെങ്കിൽ
	    fm = &namedfm_p[idx];
    }

    എങ്കിൽ (fm != ശൂന്യ)
    {
	എങ്കിൽ (vi_namedfm != ശൂന്യ || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    എങ്കിൽ (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_viminfo_barline(vir_T *virp, പൂർണ്ണസംഖ്യ got_encoding, പൂർണ്ണസംഖ്യ force, പൂർണ്ണസംഖ്യ writing)
{
    char_u	*p = virp->vir_വരി + 1;
    പൂർണ്ണസംഖ്യ		bartype;
    garray_T	മൂല്യങ്ങൾ;
    bval_T	*vp;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    എങ്കിൽ (*p == '<')
    {
	// Continuation line of an unrecognized item.
	എങ്കിൽ (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_വരി);
    }
    അല്ലെങ്കിൽ
    {
	ga_init2(&മൂല്യങ്ങൾ, വലുപ്പം(bval_T), 20);
	bartype = getdigits(&p);
	തിരഞ്ഞെടുപ്പ് (bartype)
	{
	    അവസ്ഥ BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		എങ്കിൽ (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &മൂല്യങ്ങൾ);
		    vp = (bval_T *)മൂല്യങ്ങൾ.ga_data;
		    എങ്കിൽ (മൂല്യങ്ങൾ.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		നിർത്തുക;

	    അവസ്ഥ BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &മൂല്യങ്ങൾ);
		handle_viminfo_history(&മൂല്യങ്ങൾ, writing);
		നിർത്തുക;

	    അവസ്ഥ BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &മൂല്യങ്ങൾ);
		handle_viminfo_register(&മൂല്യങ്ങൾ, force);
		നിർത്തുക;

	    അവസ്ഥ BARTYPE_MARK:
		read_next = barline_parse(virp, p, &മൂല്യങ്ങൾ);
		handle_viminfo_mark(&മൂല്യങ്ങൾ, force);
		നിർത്തുക;

	    സ്ഥിരസ്ഥിതി:
		// copy unrecognized line (for future use)
		എങ്കിൽ (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_വരി);
	}
	വേണ്ടി (i = 0; i < മൂല്യങ്ങൾ.ga_len; ++i)
	{
	    vp = (bval_T *)മൂല്യങ്ങൾ.ga_data + i;
	    എങ്കിൽ (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&മൂല്യങ്ങൾ);
    }

    എങ്കിൽ (read_next)
	തിരികെ viminfo_readline(virp);
    തിരികെ FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_viminfo_up_to_marks(
    vir_T	*virp,
    പൂർണ്ണസംഖ്യ		forceit,
    പൂർണ്ണസംഖ്യ		writing)
{
    പൂർണ്ണസംഖ്യ		eof;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    ആകുമ്പോൾ (!eof && virp->vir_വരി[0] != '>')
    {
	തിരഞ്ഞെടുപ്പ് (virp->vir_വരി[0])
	{
		// Characters reserved for future expansion, ignored now
	    അവസ്ഥ '+': // "+40 /path/dir file", for running vim without args
	    അവസ്ഥ '^': // to be defined
	    അവസ്ഥ '<': // long line - ignored
		// A comment or empty line.
	    അവസ്ഥ NUL:
	    അവസ്ഥ '\r':
	    അവസ്ഥ '\n':
	    അവസ്ഥ '#':
		eof = viminfo_readline(virp);
		നിർത്തുക;
	    അവസ്ഥ '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		നിർത്തുക;
	    അവസ്ഥ '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		നിർത്തുക;
	    അവസ്ഥ '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#അല്ലെങ്കിൽ
		eof = viminfo_readline(virp);
#endif
		നിർത്തുക;
	    അവസ്ഥ '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		നിർത്തുക;
	    അവസ്ഥ '"':
		// When registers are in bar lines skip the old style register
		// lines.
		എങ്കിൽ (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		അല്ലെങ്കിൽ
		    ചെയ്യുക
		    {
			eof = viminfo_readline(virp);
		    } ആകുമ്പോൾ (!eof && (virp->vir_വരി[0] == TAB
						|| virp->vir_വരി[0] == '<'));
		നിർത്തുക;
	    അവസ്ഥ '/':	    // Search string
	    അവസ്ഥ '&':	    // Substitute search string
	    അവസ്ഥ '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_തിരയൽ_pattern(virp, forceit, writing);
		നിർത്തുക;
	    അവസ്ഥ '$':
		eof = read_viminfo_sub_string(virp, forceit);
		നിർത്തുക;
	    അവസ്ഥ ':':
	    അവസ്ഥ '?':
	    അവസ്ഥ '=':
	    അവസ്ഥ '@':
		// When history is in bar lines skip the old style history
		// lines.
		എങ്കിൽ (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		അല്ലെങ്കിൽ
		    eof = viminfo_readline(virp);
		നിർത്തുക;
	    അവസ്ഥ '-':
	    അവസ്ഥ '\'':
		// When file marks are in bar lines skip the old style lines.
		എങ്കിൽ (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		അല്ലെങ്കിൽ
		    eof = viminfo_readline(virp);
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		എങ്കിൽ (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_വരി))
		    eof = TRUE;
		അല്ലെങ്കിൽ
		    eof = viminfo_readline(virp);
		നിർത്തുക;
	}
    }

    // Finish reading history items.
    എങ്കിൽ (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    തിരികെ eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    സ്ഥിതം ശൂന്യം
do_viminfo(FILE *fp_in, FILE *fp_out, പൂർണ്ണസംഖ്യ flags)
{
    പൂർണ്ണസംഖ്യ		eof = FALSE;
    vir_T	vir;
    പൂർണ്ണസംഖ്യ		merge = FALSE;
    പൂർണ്ണസംഖ്യ		do_copy_marks = FALSE;
    garray_T	buflist;

    എങ്കിൽ ((vir.vir_വരി = alloc(LSIZE)) == ശൂന്യ)
	തിരികെ;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, വലുപ്പം(char_u *), 100);
    vir.vir_version = -1;

    എങ്കിൽ (fp_in != ശൂന്യ)
    {
	എങ്കിൽ (flags & VIF_WANT_INFO)
	{
	    എങ്കിൽ (fp_out != ശൂന്യ)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != ശൂന്യ);
	    merge = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (flags != 0)
	    // Skip info, find start of marks
	    ആകുമ്പോൾ (!(eof = viminfo_readline(&vir))
		    && vir.vir_വരി[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    എങ്കിൽ (fp_out != ശൂന്യ)
    {
	// Write the info:
	fprintf(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_out);
	fprintf(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_തിരയൽ_pattern(fp_out);
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

	എങ്കിൽ (do_copy_marks)
	    ga_init2(&buflist, വലുപ്പം(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : ശൂന്യ);
    }

    എങ്കിൽ (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	എങ്കിൽ (fp_out != ശൂന്യ)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_വരി);
    എങ്കിൽ (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, ശൂന്യ, ശൂന്യ);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    പൂർണ്ണസംഖ്യ
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    പൂർണ്ണസംഖ്യ		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    എങ്കിൽ (no_viminfo())
	തിരികെ FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ FAIL;
    fp = mch_fopen((അക്ഷരം *)fname, READBIN);

    എങ്കിൽ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == ശൂന്യ ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    എങ്കിൽ (fp == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	തിരികെ FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, ശൂന്യ, flags);

    fclose(fp);
    തിരികെ OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    ശൂന്യം
write_viminfo(char_u *file, പൂർണ്ണസംഖ്യ forceit)
{
    char_u	*fname;
    FILE	*fp_in = ശൂന്യ;	// input viminfo file, if any
    FILE	*fp_out = ശൂന്യ;	// output viminfo file
    char_u	*tempname = ശൂന്യ;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#എങ്കിൽ defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    പൂർണ്ണസംഖ്യ		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    പൂർണ്ണസംഖ്യ		hidden = FALSE;
#endif

    എങ്കിൽ (no_viminfo())
	തിരികെ;

    fname = viminfo_filename(file);	// may set to default if NULL
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ;

    fp_in = mch_fopen((അക്ഷരം *)fname, READBIN);
    എങ്കിൽ (fp_in == ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ fd;

	// if it does exist, but we can't read it, don't try writing
	എങ്കിൽ (mch_stat((അക്ഷരം *)fname, &st_new) == 0)
	    പോകുക end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((അക്ഷരം *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	എങ്കിൽ (fd < 0)
	    പോകുക end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    അല്ലെങ്കിൽ
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	എങ്കിൽ (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != വേര്_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    പൂർണ്ണസംഖ്യ	tt = സന്ദേശം_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    സന്ദേശം_didany = tt;
	    fclose(fp_in);
	    പോകുക end;
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
	വേണ്ടി (;;)
	{
	    പൂർണ്ണസംഖ്യ		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#അല്ലെങ്കിൽ
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#അല്ലെങ്കിൽ
				    (char_u *)".tmp",
#endif
				    FALSE);
	    എങ്കിൽ (tempname == ശൂന്യ)		// out of memory
		നിർത്തുക;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    എങ്കിൽ (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    വേണ്ടി (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		എങ്കിൽ (mch_stat((അക്ഷരം *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    എങ്കിൽ (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			നിർത്തുക;
		    }
#endif
		}
		അല്ലെങ്കിൽ
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((അക്ഷരം *)tempname, WRITEBIN);
		    (ശൂന്യം)umask(umask_save);
#അല്ലെങ്കിൽ
		    പൂർണ്ണസംഖ്യ	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((അക്ഷരം *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(പൂർണ്ണസംഖ്യ)((st_old.st_mode & 0777) | 0600));
		    (ശൂന്യം)umask(umask_save);
# അല്ലെങ്കിൽ
		    fd = mch_open((അക്ഷരം *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    എങ്കിൽ (fd < 0)
		    {
			fp_out = ശൂന്യ;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			എങ്കിൽ (errno != EEXIST)
			    നിർത്തുക;
# endif
		    }
		    അല്ലെങ്കിൽ
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    എങ്കിൽ (fp_out != ശൂന്യ)
			നിർത്തുക;
		}

		// Assume file exists, try again with another name.
		എങ്കിൽ (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    നിർത്തുക;
		}
		*wp = next_char;
		--next_char;
	    }

	    എങ്കിൽ (tempname != ശൂന്യ)
		നിർത്തുക;
	    // continue if shortname was set
	}

#എങ്കിൽ defined(UNIX) && defined(HAVE_FCHOWN)
	എങ്കിൽ (tempname != ശൂന്യ && fp_out != ശൂന്യ)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    എങ്കിൽ (mch_stat((അക്ഷരം *)tempname, &tmp_st) >= 0)
	    {
		എങ്കിൽ (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		എങ്കിൽ (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (ശൂന്യം)mch_setperm(tempname, 0600);
	    }
	    അല്ലെങ്കിൽ
		// can't stat the file, set conservative permissions
		(ശൂന്യം)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    എങ്കിൽ (fp_out == ശൂന്യ)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == ശൂന്യ || tempname == ശൂന്യ) ? fname : tempname);
	എങ്കിൽ (fp_in != ശൂന്യ)
	    fclose(fp_in);
	പോകുക end;
    }

    എങ്കിൽ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    എങ്കിൽ (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    എങ്കിൽ (fp_in != ശൂന്യ)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	എങ്കിൽ (viminfo_errcnt == 0)
	{
	    എങ്കിൽ (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    അല്ലെങ്കിൽ എങ്കിൽ (hidden)
		mch_hide(fname);
#endif
	}
	എങ്കിൽ (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    ശൂന്യം
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    എങ്കിൽ (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    എങ്കിൽ (eap->cmdidx == CMD_rviminfo)
    {
	എങ്കിൽ (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    അല്ലെങ്കിൽ
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
