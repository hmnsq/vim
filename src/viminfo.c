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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*vir_ਸਤਰ;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    ਪੂਰਨ		vir_ਸੰਸਕਰਣ;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    BVAL_NR,
    BVAL_STRING,
    BVAL_ਖਾਲੀ_ਥਾਵਾਂ
} btype_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    btype_T	bv_type;
    ਲੰਮਾ	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    ਪੂਰਨ		bv_len;		// length of bv_string
    ਪੂਰਨ		bv_allocated;	// bv_string was allocated
} bval_T;

#ਜੇ defined(FEAT_VIMINFO)

ਸਥਿਰ ਪੂਰਨ  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_viminfo_parameter(ਪੂਰਨ type)
{
    char_u  *p;

    ਲਈ (p = p_viminfo; *p; ++p)
    {
	ਜੇ (*p == type)
	    ਵਾਪਸ p + 1;
	ਜੇ (*p == 'n')		    // 'n' is always the last one
	    ਰੋਕੋ;
	p = vim_strchr(p, ',');	    // skip until next ','
	ਜੇ (p == ਨਿਰਾ)		    // hit the end without finding parameter
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    ਪੂਰਨ
get_viminfo_parameter(ਪੂਰਨ type)
{
    char_u  *p;

    p = ਲੱਭੋ_viminfo_parameter(type);
    ਜੇ (p != ਨਿਰਾ && VIM_ISDIGIT(*p))
	ਵਾਪਸ atoi((ਅੱਖਰ *)p);
    ਵਾਪਸ -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    ਸਥਿਰ char_u *
viminfo_filename(char_u *file)
{
    ਜੇ (file == ਨਿਰਾ || *file == NUL)
    {
	size_t  len;

	ਜੇ (*p_viminfofile != NUL)
	    file = p_viminfofile;
	ਨਹੀਂਤਾਂ ਜੇ ((file = ਲੱਭੋ_viminfo_parameter('n')) == ਨਿਰਾ || *file == NUL)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMINFO_FILE2
# ifdef VMS
	    ਜੇ (mch_getenv((char_u *)"SYS$LOGIN") == ਨਿਰਾ)
# ਨਹੀਂਤਾਂ
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    ਜੇ (STRCMP(vim_getenv((char_u *)"HOME", ਨਿਰਾ), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == ਨਿਰਾ)
#  ਨਹੀਂਤਾਂ
	    ਜੇ (mch_getenv((char_u *)"HOME") == ਨਿਰਾ)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		ਜੇ (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		ਨਹੀਂਤਾਂ
		    file = (char_u *)VIMINFO_FILE;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	ਵਾਪਸ vim_strnsave(file, len);
    }

    ਵਾਪਸ vim_strsave(file);
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
    ਸਥਿਰ ਖਾਲੀ
viminfo_writestring(FILE *fd, char_u *p)
{
    ਪੂਰਨ		c;
    char_u	*s;
    ਪੂਰਨ		len = 0;

    ਲਈ (s = p; *s != NUL; ++s)
    {
	ਜੇ (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    ਜੇ (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    ਜਦੋਂ ((c = *p++) != NUL)
    {
	ਜੇ (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    ਜੇ (c == '\n')
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
    ਸਥਿਰ ਪੂਰਨ
barline_writestring(FILE *fd, char_u *s, ਪੂਰਨ ਬਚਿਆ_ਸ਼ੁਰੂ)
{
    char_u *p;
    ਪੂਰਨ	    ਬਚਿਆ = ਬਚਿਆ_ਸ਼ੁਰੂ;
    ਪੂਰਨ	    len = 2;

    // Count the number of characters produced, including quotes.
    ਲਈ (p = s; *p != NUL; ++p)
    {
	ਜੇ (*p == NL)
	    len += 2;
	ਨਹੀਂਤਾਂ ਜੇ (*p == '"' || *p == '\\')
	    len += 2;
	ਨਹੀਂਤਾਂ
	    ++len;
    }
    ਜੇ (len > ਬਚਿਆ - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	ਬਚਿਆ = LSIZE - 20;
    }

    putc('"', fd);
    ਲਈ (p = s; *p != NUL; ++p)
    {
	ਜੇ (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --ਬਚਿਆ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --ਬਚਿਆ;
	}
	ਨਹੀਂਤਾਂ
	    putc(*p, fd);
	--ਬਚਿਆ;

	ਜੇ (ਬਚਿਆ < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    ਬਚਿਆ = LSIZE - 20;
	}
    }
    putc('"', fd);
    ਵਾਪਸ ਬਚਿਆ - 2;
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
    ਸਥਿਰ char_u *
viminfo_readstring(
    vir_T	*virp,
    ਪੂਰਨ		off,		    // offset for virp->vir_line
    ਪੂਰਨ		convert)	    // convert the string
{
    char_u	*retval = ਨਿਰਾ;
    char_u	*s, *d;
    ਲੰਮਾ	len;

    ਜੇ (virp->vir_ਸਤਰ[off] == Ctrl_V && vim_isdigit(virp->vir_ਸਤਰ[off + 1]))
    {
	len = atol((ਅੱਖਰ *)virp->vir_ਸਤਰ + off + 1);
	ਜੇ (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	ਜੇ (retval == ਨਿਰਾ)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (ਖਾਲੀ)vim_fgets(virp->vir_ਸਤਰ, 10, virp->vir_fd);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	(ਖਾਲੀ)vim_fgets(retval, (ਪੂਰਨ)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    ਨਹੀਂਤਾਂ
    {
	retval = vim_strsave(virp->vir_ਸਤਰ + off);
	ਜੇ (retval == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    ਜਦੋਂ (*s != NUL && *s != '\n')
    {
	ਜੇ (s[0] == Ctrl_V && s[1] != NUL)
	{
	    ਜੇ (s[1] == 'n')
		*d++ = '\n';
	    ਨਹੀਂਤਾਂ
		*d++ = Ctrl_V;
	    s += 2;
	}
	ਨਹੀਂਤਾਂ
	    *d++ = *s++;
    }
    *d = NUL;

    ਜੇ (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, ਨਿਰਾ);
	ਜੇ (d != ਨਿਰਾ)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    ਵਾਪਸ retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    ਸਥਿਰ ਪੂਰਨ
viminfo_readline(vir_T *virp)
{
    ਵਾਪਸ vim_fgets(virp->vir_ਸਤਰ, LSIZE, virp->vir_fd);
}

    ਸਥਿਰ ਪੂਰਨ
read_viminfo_bufferlist(
    vir_T	*virp,
    ਪੂਰਨ		writing)
{
    char_u	*tab;
    linenr_T	lnum;
    colnr_T	ਖਾਨਾ;
    buf_T	*buf;
    char_u	*sfname;
    char_u	*xline;

    // Handle long line and escaped characters.
    xline = viminfo_readstring(virp, 1, FALSE);

    // don't read in if there are files on the command-line or if writing:
    ਜੇ (xline != ਨਿਰਾ && !writing && ARGCOUNT == 0
				       && ਲੱਭੋ_viminfo_parameter('%') != ਨਿਰਾ)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	ਖਾਨਾ = 0;
	tab = vim_strrchr(xline, '\t');
	ਜੇ (tab != ਨਿਰਾ)
	{
	    *tab++ = '\0';
	    ਖਾਨਾ = (colnr_T)atoi((ਅੱਖਰ *)tab);
	    tab = vim_strrchr(xline, '\t');
	    ਜੇ (tab != ਨਿਰਾ)
	    {
		*tab++ = '\0';
		lnum = atol((ਅੱਖਰ *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	ਜੇ (buf != ਨਿਰਾ)	// just in case...
	{
	    buf->b_ਆਖਰੀ_cursor.lnum = lnum;
	    buf->b_ਆਖਰੀ_cursor.ਖਾਨਾ = ਖਾਨਾ;
	    buflist_setfpos(buf, curwin, lnum, ਖਾਨਾ, FALSE);
	}
    }
    vim_free(xline);

    ਵਾਪਸ viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    ਸਥਿਰ ਪੂਰਨ
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    ਪੂਰਨ	    part_len;
    ਪੂਰਨ	    retval = FALSE;

    name = home_replace_save(ਨਿਰਾ, name);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਲਈ (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, ਆਕਾਰ(part), ", ");
	ਜੇ (part[0] == 'r')
	{
	    ਜੇ (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		ਰੋਕੋ;
	    }
	}
    }
    vim_free(name);
    ਵਾਪਸ retval;
}

    ਸਥਿਰ ਖਾਲੀ
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*ਸਤਰ;
    ਪੂਰਨ		max_buffers;

    ਜੇ (ਲੱਭੋ_viminfo_parameter('%') == ਨਿਰਾ)
	ਵਾਪਸ;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਤਰ_BUF_LEN (MAXPATHL + 40)
    ਸਤਰ = alloc(ਸਤਰ_BUF_LEN);
    ਜੇ (ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    FOR_ALL_TAB_WINDOWS(tp, win)
	ਸੈੱਟ_ਆਖਰੀ_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	ਜੇ (buf->b_fname == ਨਿਰਾ
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    ਜਾਰੀ;

	ਜੇ (max_buffers-- == 0)
	    ਰੋਕੋ;
	putc('%', fp);
	linelen = home_replace(ਨਿਰਾ, buf->b_ffname, ਸਤਰ, MAXPATHL, TRUE);
	vim_snprintf((ਅੱਖਰ *)ਸਤਰ + linelen, ਸਤਰ_BUF_LEN - linelen, "\t%ld\t%d",
			(ਲੰਮਾ)buf->b_ਆਖਰੀ_cursor.lnum,
			buf->b_ਆਖਰੀ_cursor.ਖਾਨਾ);
	viminfo_writestring(fp, ਸਤਰ);
    }
    vim_free(ਸਤਰ);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
ਸਥਿਰ histentry_T *viminfo_history[HIST_ਗਿਣਤੀ] =
					       {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};
ਸਥਿਰ ਪੂਰਨ	viminfo_hisidx[HIST_ਗਿਣਤੀ] = {0, 0, 0, 0, 0};
ਸਥਿਰ ਪੂਰਨ	viminfo_hislen[HIST_ਗਿਣਤੀ] = {0, 0, 0, 0, 0};
ਸਥਿਰ ਪੂਰਨ	viminfo_ਜੋੜੋ_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    ਸਥਿਰ ਪੂਰਨ
hist_type2char(
    ਪੂਰਨ	    type,
    ਪੂਰਨ	    use_question)	    // use '?' instead of '/'
{
    ਜੇ (type == HIST_CMD)
	ਵਾਪਸ ':';
    ਜੇ (type == HIST_ਖੋਜ)
    {
	ਜੇ (use_question)
	    ਵਾਪਸ '?';
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ '/';
    }
    ਜੇ (type == HIST_ਵਿਅੰਜਕ)
	ਵਾਪਸ '=';
    ਵਾਪਸ '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    ਸਥਿਰ ਖਾਲੀ
prepare_viminfo_history(ਪੂਰਨ asklen, ਪੂਰਨ writing)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    ਅੰਕ;
    ਪੂਰਨ	    type;
    ਪੂਰਨ	    len;
    ਪੂਰਨ	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_ਜੋੜੋ_at_front = (asklen != 0 && !writing);
    ਜੇ (asklen > hislen)
	asklen = hislen;

    ਲਈ (type = 0; type < HIST_ਗਿਣਤੀ; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	ਲਈ (i = 0, ਅੰਕ = 0; i < hislen; i++)
	    ਜੇ (histentry[i].hisstr == ਨਿਰਾ || histentry[i].viminfo)
		ਅੰਕ++;
	len = asklen;
	ਜੇ (ਅੰਕ > len)
	    len = ਅੰਕ;
	ਜੇ (len <= 0)
	    viminfo_history[type] = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	ਜੇ (viminfo_history[type] == ਨਿਰਾ)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    ਸਥਿਰ ਪੂਰਨ
read_viminfo_history(vir_T *virp, ਪੂਰਨ writing)
{
    ਪੂਰਨ		type;
    long_u	len;
    char_u	*val = ਨਿਰਾ;
    char_u	*p;

    type = hist_char2type(virp->vir_ਸਤਰ[0]);
    ਜੇ (viminfo_hisidx[type] >= viminfo_hislen[type])
	ਜਾਓ ਪੂਰਾ;

    val = viminfo_readstring(virp, 1, TRUE);
    ਜੇ (val == ਨਿਰਾ || *val == NUL)
	ਜਾਓ ਪੂਰਾ;

    ਪੂਰਨ sep = (*val == ' ' ? NUL : *val);

    ਜੇ (ਵਿੱਚ_history(type, val + (type == HIST_ਖੋਜ), viminfo_ਜੋੜੋ_at_front,
								sep, writing))
	ਜਾਓ ਪੂਰਾ;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    ਜੇ (type == HIST_ਖੋਜ)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਓ ਪੂਰਾ;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    ਨਹੀਂਤਾਂ
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਓ ਪੂਰਾ;

	// Not a search entry: No separator in the viminfo
	// file, add a NUL separator.
	mch_memmove(p, val, (size_t)len + 1);	// +1 to include the NUL
	p[len + 1] = NUL;			// put the separator *after* the string's NUL
    }
    viminfo_history[type][viminfo_hisidx[type]].hisstr = p;
    viminfo_history[type][viminfo_hisidx[type]].hisstrlen = (size_t)len;
    viminfo_history[type][viminfo_hisidx[type]].ਸਮਾਂ_ਸੈੱਟ = 0;
    viminfo_history[type][viminfo_hisidx[type]].viminfo = TRUE;
    viminfo_history[type][viminfo_hisidx[type]].hisnum = 0;
    viminfo_hisidx[type]++;

ਪੂਰਾ:
    vim_free(val);
    ਵਾਪਸ viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_viminfo_history(
	garray_T    *ਮੁੱਲਾਂ,
	ਪੂਰਨ	    writing)
{
    ਪੂਰਨ		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)ਮੁੱਲਾਂ->ga_ਅੰਕੜੇ;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    ਜੇ (ਮੁੱਲਾਂ->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_ਖਾਲੀ_ਥਾਵਾਂ)
	    || vp[3].bv_type != BVAL_STRING)
	ਵਾਪਸ;

    type = vp[0].bv_nr;
    ਜੇ (type >= HIST_ਗਿਣਤੀ)
	ਵਾਪਸ;

    ਜੇ (viminfo_hisidx[type] >= viminfo_hislen[type])
	ਵਾਪਸ;

    val = vp[3].bv_string;
    ਜੇ (val == ਨਿਰਾ || *val == NUL)
	ਵਾਪਸ;

    ਪੂਰਨ sep = type == HIST_ਖੋਜ && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    ਪੂਰਨ idx;
    ਪੂਰਨ overwrite = FALSE;

    ਜੇ (ਵਿੱਚ_history(type, val, viminfo_ਜੋੜੋ_at_front, sep, writing))
	ਵਾਪਸ;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    ਲਈ (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	ਜੇ (STRCMP(val, p) == 0
		&& (type != HIST_ਖੋਜ || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    ਰੋਕੋ;
	}
    }

    ਜੇ (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    ਨਹੀਂਤਾਂ
	len = 0; // for picky compilers
    ਜੇ (p != ਨਿਰਾ)
    {
	viminfo_history[type][idx].ਸਮਾਂ_ਸੈੱਟ = vp[1].bv_nr;
	ਜੇ (!overwrite)
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
    ਸਥਿਰ ਖਾਲੀ
concat_history(ਪੂਰਨ type)
{
    ਪੂਰਨ		idx;
    ਪੂਰਨ		i;
    ਪੂਰਨ		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    ਪੂਰਨ		*hisidx = get_hisidx(type);
    ਪੂਰਨ		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    ਜੇ (idx >= hislen)
	idx -= hislen;
    ਨਹੀਂਤਾਂ ਜੇ (idx < 0)
	idx = hislen - 1;
    ਜੇ (viminfo_ਜੋੜੋ_at_front)
	*hisidx = idx;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (*hisidx == -1)
	    *hisidx = hislen - 1;
	ਕਰੋ
	{
	    ਜੇ (histentry[idx].hisstr != ਨਿਰਾ || histentry[idx].viminfo)
		ਰੋਕੋ;
	    ਜੇ (++idx == hislen)
		idx = 0;
	} ਜਦੋਂ (idx != *hisidx);
	ਜੇ (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    ਲਈ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].ਸਮਾਂ_ਸੈੱਟ = viminfo_history[type][i].ਸਮਾਂ_ਸੈੱਟ;
	ਜੇ (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    ਲਈ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    ਸਥਿਰ ਪੂਰਨ
ਛਾਂਟੋ_hist(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    ਜੇ (p1->ਸਮਾਂ_ਸੈੱਟ < p2->ਸਮਾਂ_ਸੈੱਟ) ਵਾਪਸ -1;
    ਜੇ (p1->ਸਮਾਂ_ਸੈੱਟ > p2->ਸਮਾਂ_ਸੈੱਟ) ਵਾਪਸ 1;
    ਵਾਪਸ 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    ਸਥਿਰ ਖਾਲੀ
ਮਿਲਾਓ_history(ਪੂਰਨ type)
{
    ਪੂਰਨ		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    ਪੂਰਨ		i;
    ਪੂਰਨ		len;
    ਪੂਰਨ		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    ਪੂਰਨ		*hisidx = get_hisidx(type);
    ਪੂਰਨ		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    ਜੇ (tot_hist == ਨਿਰਾ || new_hist == ਨਿਰਾ)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	ਵਾਪਸ;
    }
    ਲਈ (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    ਲਈ (i = 0; i < hislen; i++)
	ਜੇ (histentry[i].hisstr != ਨਿਰਾ)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((ਖਾਲੀ *)tot_hist, (size_t)len, ਆਕਾਰ(histentry_T *), ਛਾਂਟੋ_hist);

    // Keep the newest ones.
    ਲਈ (i = 0; i < hislen; i++)
    {
	ਜੇ (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = ਨਿਰਾ;
	    tot_hist[i]->hisstrlen = 0;
	    ਜੇ (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	ਨਹੀਂਤਾਂ
	    clear_hist_ਇੰਦਰਾਜ(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    ਲਈ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    ਲਈ (i = 0; i < hislen; i++)
    {
	vim_free(histentry[i].hisstr);
	histentry[i].hisstrlen = 0;
    }
    vim_free(histentry);
    ਸੈੱਟ_histentry(type, new_hist);
    vim_free(tot_hist);
}

/*
 * Finish reading history lines from viminfo.  Not used when writing viminfo.
 */
    ਸਥਿਰ ਖਾਲੀ
ਅੰਤ_viminfo_history(vir_T *virp)
{
    ਪੂਰਨ	type;
    ਪੂਰਨ ਮਿਲਾਓ = virp->vir_ਸੰਸਕਰਣ >= VIMINFO_ਸੰਸਕਰਣ_WITH_HISTORY;

    ਲਈ (type = 0; type < HIST_ਗਿਣਤੀ; ++type)
    {
	ਜੇ (get_histentry(type) == ਨਿਰਾ)
	    ਜਾਰੀ;

	ਜੇ (ਮਿਲਾਓ)
	    ਮਿਲਾਓ_history(type);
	ਨਹੀਂਤਾਂ
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
    ਸਥਿਰ ਖਾਲੀ
write_viminfo_history(FILE *fp, ਪੂਰਨ ਮਿਲਾਓ)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    type;
    ਪੂਰਨ	    ਅੰਕ_saved;
    ਪੂਰਨ     ਗੇੜਾ;
    ਪੂਰਨ	    hislen;

    init_history();
    hislen = get_hislen();
    ਜੇ (hislen == 0)
	ਵਾਪਸ;
    ਲਈ (type = 0; type < HIST_ਗਿਣਤੀ; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	ਪੂਰਨ	    *hisidx = get_hisidx(type);

	ਅੰਕ_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	ਜੇ (ਅੰਕ_saved == 0)
	    ਜਾਰੀ;
	ਜੇ (ਅੰਕ_saved < 0)  // Use default
	    ਅੰਕ_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_ਖੋਜ ? _("Search String") :
			    type == HIST_ਵਿਅੰਜਕ ? _("Expression") :
			    type == HIST_ਨਿਵੇਸ਼ ? _("Input Line") :
					_("Debug Line"));
	ਜੇ (ਅੰਕ_saved > hislen)
	    ਅੰਕ_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
	{
	    ਜੇ (ਗੇੜਾ == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    ਨਹੀਂਤਾਂ ਜੇ (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    ਨਹੀਂਤਾਂ
		// empty list
		i = -1;
	    ਜੇ (i >= 0)
		ਜਦੋਂ (ਅੰਕ_saved > 0
			&& !(ਗੇੜਾ == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    ਪੂਰਨ	    c = NUL;

		    ਜੇ (ਗੇੜਾ == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].ਸਮਾਂ_ਸੈੱਟ;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (viminfo_history[type] == ਨਿਰਾ)
			{
			    p = ਨਿਰਾ;
			    plen = 0;
			    timestamp = 0;
			}
			ਨਹੀਂਤਾਂ
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].ਸਮਾਂ_ਸੈੱਟ;
			}
		    }

		    ਜੇ (p != ਨਿਰਾ && (ਗੇੜਾ == 2
				       || !ਮਿਲਾਓ
				       || !histentry[i].viminfo))
		    {
			--ਅੰਕ_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			ਜੇ (type == HIST_ਖੋਜ)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    ਅੱਖਰ    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    ਜੇ (c == NUL)
				cbuf[0] = NUL;
			    ਨਹੀਂਤਾਂ
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (ਲੰਮਾ)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    ਜੇ (ਗੇੜਾ == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			ਜੇ (--i < 0)
			    i = hislen - 1;
			ਜੇ (i == *hisidx)
			    ਰੋਕੋ;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	ਲਈ (i = 0; i < viminfo_hisidx[type]; ++i)
	    ਜੇ (viminfo_history[type] != ਨਿਰਾ)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    ਸਥਿਰ ਖਾਲੀ
write_viminfo_barlines(vir_T *virp, FILE *fp_ਬਾਹਰਲਾ)
{
    ਪੂਰਨ		i;
    garray_T	*gap = &virp->vir_barlines;
    ਪੂਰਨ		ਵੇਖਿਆ_useful = FALSE;
    ਅੱਖਰ	*ਸਤਰ;

    ਜੇ (gap->ga_len <= 0)
	ਵਾਪਸ;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_ਬਾਹਰਲਾ);

    // Skip over continuation lines until seeing a useful line.
    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	ਸਤਰ = ((ਅੱਖਰ **)(gap->ga_ਅੰਕੜੇ))[i];
	ਜੇ (ਵੇਖਿਆ_useful || ਸਤਰ[1] != '<')
	{
	    fputs(ਸਤਰ, fp_ਬਾਹਰਲਾ);
	    ਵੇਖਿਆ_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    ਸਥਿਰ ਪੂਰਨ
barline_parse(vir_T *virp, char_u *ਪਾਠ, garray_T *ਮੁੱਲਾਂ)
{
    char_u  *p = ਪਾਠ;
    char_u  *nextp = ਨਿਰਾ;
    char_u  *buf = ਨਿਰਾ;
    bval_T  *ਮੁੱਲ;
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    allocated = FALSE;
    ਪੂਰਨ	    eof;
    char_u  *sconv;
    ਪੂਰਨ	    converted;

    ਜਦੋਂ (*p == ',')
    {
	++p;
	ਜੇ (ga_grow(ਮੁੱਲਾਂ, 1) == FAIL)
	    ਰੋਕੋ;
	ਮੁੱਲ = (bval_T *)(ਮੁੱਲਾਂ->ga_ਅੰਕੜੇ) + ਮੁੱਲਾਂ->ga_len;

	ਜੇ (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    ਜੇ (!allocated)
	    {
		ਲਈ (i = 0; i < ਮੁੱਲਾਂ->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(ਮੁੱਲਾਂ->ga_ਅੰਕੜੇ) + i;

		    ਜੇ (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    ਜੇ (vim_isdigit(p[1]))
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
		ਜੇ (buf == ਨਿਰਾ)
		    ਵਾਪਸ TRUE;
		p = buf;
		ਲਈ (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    ਜੇ (eof || virp->vir_ਸਤਰ[0] != '|'
						  || virp->vir_ਸਤਰ[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			ਵਾਪਸ eof || virp->vir_ਸਤਰ[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_ਸਤਰ);
		    ਜਦੋਂ (n > 2 && (virp->vir_ਸਤਰ[n - 1] == NL
					     || virp->vir_ਸਤਰ[n - 1] == CAR))
			--n;
		    n -= 2;
		    ਜੇ (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_ਸਤਰ + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_ਸਤਰ + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		ਜੇ (eof || virp->vir_ਸਤਰ[0] != '|'
					      || virp->vir_ਸਤਰ[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    ਵਾਪਸ eof || virp->vir_ਸਤਰ[0] == '|';
		p = virp->vir_ਸਤਰ + 2;
	    }
	}

	ਜੇ (ਸੁਰੱਖਿਅਤ_isdigit(*p))
	{
	    ਮੁੱਲ->bv_type = BVAL_NR;
	    ਮੁੱਲ->bv_nr = getdigits(&p);
	    ++ਮੁੱਲਾਂ->ga_len;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '"')
	{
	    ਪੂਰਨ	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    ਜਦੋਂ (*p != '"')
	    {
		ਜੇ (*p == NL || *p == NUL)
		    ਵਾਪਸ TRUE;  // syntax error, drop the value
		ਜੇ (*p == '\\')
		{
		    ++p;
		    ਜੇ (*p == 'n')
			s[len++] = '\n';
		    ਨਹੀਂਤਾਂ
			s[len++] = *p;
		    ++p;
		}
		ਨਹੀਂਤਾਂ
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    ਮੁੱਲ->bv_tofree = ਨਿਰਾ;
	    ਜੇ (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, ਨਿਰਾ);
		ਜੇ (sconv != ਨਿਰਾ)
		{
		    ਜੇ (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			ਮੁੱਲ->bv_tofree = buf;
		    s = sconv;
		    len = (ਪੂਰਨ)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    ਜੇ (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    ਮੁੱਲ->bv_string = s;
	    ਮੁੱਲ->bv_type = BVAL_STRING;
	    ਮੁੱਲ->bv_len = len;
	    ਮੁੱਲ->bv_allocated = allocated || converted;
	    ++ਮੁੱਲਾਂ->ga_len;
	    ਜੇ (nextp != ਨਿਰਾ)
	    {
		// values following a long string
		p = nextp;
		nextp = ਨਿਰਾ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == ',')
	{
	    ਮੁੱਲ->bv_type = BVAL_ਖਾਲੀ_ਥਾਵਾਂ;
	    ++ਮੁੱਲਾਂ->ga_len;
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }
    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
write_viminfo_ਸੰਸਕਰਣ(FILE *fp_ਬਾਹਰਲਾ)
{
    fprintf(fp_ਬਾਹਰਲਾ, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_ਸੰਸਕਰਣ, VIMINFO_ਸੰਸਕਰਣ);
}

    ਸਥਿਰ ਪੂਰਨ
no_viminfo(ਖਾਲੀ)
{
    // "vim -i NONE" does not read or write a viminfo file
    ਵਾਪਸ STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    ਸਥਿਰ ਪੂਰਨ
viminfo_error(ਅੱਖਰ *errnum, ਅੱਖਰ *ਸੁਨੇਹਾ_2, char_u *ਸਤਰ)
{
    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, ਸੁਨੇਹਾ_2);
    STRNCAT(IObuff, ਸਤਰ, IOSIZE - STRLEN(IObuff) - 1);
    ਜੇ (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((ਅੱਖਰ *)IObuff);
    ਜੇ (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_ਦਾ_file));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    ਸਥਿਰ ਪੂਰਨ
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    ਪੂਰਨ		i;

    ਜੇ (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_ਸਤਰ, '=');
	ਜੇ (p != ਨਿਰਾ)
	{
	    // remove trailing newline
	    ++p;
	    ਲਈ (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_ਤਿਆਰੀ(&virp->vir_conv, p, p_enc);
	}
    }
    ਵਾਪਸ viminfo_readline(virp);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    ਸਥਿਰ ਪੂਰਨ
read_viminfo_varlist(vir_T *virp, ਪੂਰਨ writing)
{
    char_u	*tab;
    ਪੂਰਨ		type = VAR_ਸੰਖਿਆ;
    typval_T	tv;
    funccal_ਇੰਦਰਾਜ_T funccal_ਇੰਦਰਾਜ;

    ਜੇ (!writing && (ਲੱਭੋ_viminfo_parameter('!') != ਨਿਰਾ))
    {
	tab = vim_strchr(virp->vir_ਸਤਰ + 1, '\t');
	ਜੇ (tab != ਨਿਰਾ)
	{
	    *tab++ = '\0';	// isolate the variable name
	    ਚੋਣ (*tab)
	    {
		ਹਾਲਤ 'S': type = VAR_STRING; ਰੋਕੋ;
		ਹਾਲਤ 'F': type = VAR_FLOAT; ਰੋਕੋ;
		ਹਾਲਤ 'D': type = VAR_DICT; ਰੋਕੋ;
		ਹਾਲਤ 'L': type = VAR_LIST; ਰੋਕੋ;
		ਹਾਲਤ 'B': type = VAR_BLOB; ਰੋਕੋ;
		ਹਾਲਤ 'X': type = VAR_SPECIAL; ਰੋਕੋ;
		ਹਾਲਤ 'T': type = VAR_TUPLE; ਰੋਕੋ;
	    }

	    tab = vim_strchr(tab, '\t');
	    ਜੇ (tab != ਨਿਰਾ)
	    {
		tv.v_type = type;
		ਜੇ (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (ਪੂਰਨ)(tab - virp->vir_ਸਤਰ + 1), TRUE);
		ਨਹੀਂਤਾਂ ਜੇ (type == VAR_FLOAT)
		    (ਖਾਲੀ)string2float(tab + 1, &tv.vval.v_float, FALSE);
		ਨਹੀਂਤਾਂ
		{
		    tv.vval.v_ਸੰਖਿਆ = atol((ਅੱਖਰ *)tab + 1);
		    ਜੇ (type == VAR_SPECIAL && (tv.vval.v_ਸੰਖਿਆ == VVAL_FALSE
					     || tv.vval.v_ਸੰਖਿਆ == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		ਜੇ (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_ਵਿਅੰਜਕ(tv.vval.v_string, ਨਿਰਾ);

		    ਜੇ (etv == ਨਿਰਾ)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    ਨਹੀਂਤਾਂ
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    ਜੇ (blob == ਨਿਰਾ)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    ਨਹੀਂਤਾਂ
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_ਇੰਦਰਾਜ);
		ਸੈੱਟ_var(virp->vir_ਸਤਰ + 1, &tv, FALSE);
		restore_funccal();

		ਜੇ (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		ਨਹੀਂਤਾਂ ਜੇ (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    ਵਾਪਸ viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    ਸਥਿਰ ਖਾਲੀ
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    ਪੂਰਨ		todo;
    ਅੱਖਰ	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ਜੇ (ਲੱਭੋ_viminfo_parameter('!') == ਨਿਰਾ)
	ਵਾਪਸ;

    fputs(_("\n# global variables:\n"), fp);

    todo = (ਪੂਰਨ)gvht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(gvht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ਜੇ (var_flavour(this_var->di_ਕੁੰਜੀ) == VAR_FLAVOUR_VIMINFO)
	    {
		ਚੋਣ (this_var->di_tv.v_type)
		{
		    ਹਾਲਤ VAR_STRING:  s = "STR"; ਰੋਕੋ;
		    ਹਾਲਤ VAR_ਸੰਖਿਆ:  s = "NUM"; ਰੋਕੋ;
		    ਹਾਲਤ VAR_FLOAT:   s = "FLO"; ਰੋਕੋ;
		    ਹਾਲਤ VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      ਪੂਰਨ	copyID = get_copyID();

			      s = "DIC";
			      ਜੇ (di != ਨਿਰਾ && !ਸੈੱਟ_ref_ਵਿੱਚ_ht(
					 &di->dv_hashtab, copyID, ਨਿਰਾ, ਨਿਰਾ)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ਜਾਰੀ;
			      ਰੋਕੋ;
			  }
		    ਹਾਲਤ VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      ਪੂਰਨ	copyID = get_copyID();

			      s = "LIS";
			      ਜੇ (l != ਨਿਰਾ && !ਸੈੱਟ_ref_ਵਿੱਚ_list_ਚੀਜ਼ਾਂ(
						       l, copyID, ਨਿਰਾ, ਨਿਰਾ)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ਜਾਰੀ;
			      ਰੋਕੋ;
			  }
		    ਹਾਲਤ VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      ਪੂਰਨ	copyID = get_copyID();

			      s = "TUP";
			      ਜੇ (tuple != ਨਿਰਾ && !ਸੈੱਟ_ref_ਵਿੱਚ_tuple_ਚੀਜ਼ਾਂ(
					       tuple, copyID, ਨਿਰਾ, ਨਿਰਾ)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ਜਾਰੀ;
			      ਰੋਕੋ;
			  }
		    ਹਾਲਤ VAR_BLOB:    s = "BLO"; ਰੋਕੋ;
		    ਹਾਲਤ VAR_BOOL:    s = "XPL"; ਰੋਕੋ;  // backwards compat.
		    ਹਾਲਤ VAR_SPECIAL: s = "XPL"; ਰੋਕੋ;

		    ਹਾਲਤ VAR_UNKNOWN:
		    ਹਾਲਤ VAR_ANY:
		    ਹਾਲਤ VAR_VOID:
		    ਹਾਲਤ VAR_FUNC:
		    ਹਾਲਤ VAR_PARTIAL:
		    ਹਾਲਤ VAR_ਕੰਮ:
		    ਹਾਲਤ VAR_CHANNEL:
		    ਹਾਲਤ VAR_INSTR:
		    ਹਾਲਤ VAR_CLASS:
		    ਹਾਲਤ VAR_OBJECT:
		    ਹਾਲਤ VAR_TYPEALIAS:
				      ਜਾਰੀ;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_ਕੁੰਜੀ, s);
		ਜੇ (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((ਅੱਖਰ *)numbuf, "%ld",
					  (ਲੰਮਾ)this_var->di_tv.vval.v_ਸੰਖਿਆ);
		    p = numbuf;
		    tofree = ਨਿਰਾ;
		}
		ਨਹੀਂਤਾਂ
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		ਜੇ (p != ਨਿਰਾ)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#ਜੇ_ਅੰਤ // FEAT_EVAL

    ਸਥਿਰ ਪੂਰਨ
read_viminfo_sub_string(vir_T *virp, ਪੂਰਨ force)
{
    ਜੇ (force || get_old_sub() == ਨਿਰਾ)
	ਸੈੱਟ_old_sub(viminfo_readstring(virp, 1, TRUE));
    ਵਾਪਸ viminfo_readline(virp);
}

    ਸਥਿਰ ਖਾਲੀ
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    ਜੇ (get_viminfo_parameter('/') == 0 || old_sub == ਨਿਰਾ)
	ਵਾਪਸ;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    ਸਥਿਰ ਪੂਰਨ
read_viminfo_ਖੋਜ_ਨਮੂਨਾ(vir_T *virp, ਪੂਰਨ force, ਪੂਰਨ writing)
{
    char_u	*lp;
    ਪੂਰਨ		idx = -1;
    ਪੂਰਨ		magic = FALSE;
    ਪੂਰਨ		no_scs = FALSE;
    ਪੂਰਨ		off_ਸਤਰ = FALSE;
    ਪੂਰਨ		off_ਸਿਰਾ = 0;
    ਲੰਮਾ	off = 0;
    ਪੂਰਨ		setlast = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਸਥਿਰ ਪੂਰਨ	hlsearch_on = FALSE;
#ਜੇ_ਅੰਤ
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
    lp = virp->vir_ਸਤਰ;
    ਜੇ (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	ਜੇ (lp[1] == 'M')		// magic on
	    magic = TRUE;
	ਜੇ (lp[2] == 's')
	    no_scs = TRUE;
	ਜੇ (lp[3] == 'L')
	    off_ਸਤਰ = TRUE;
	ਜੇ (lp[4] == 'E')
	    off_ਸਿਰਾ = ਖੋਜ_ਸਿਰਾ;
	lp += 5;
	off = getdigits(&lp);
    }
    ਜੇ (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    ਜੇ (lp[0] == '/')
	idx = RE_ਖੋਜ;
    ਨਹੀਂਤਾਂ ਜੇ (lp[0] == '&')
	idx = RE_SUBST;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਨਹੀਂਤਾਂ ਜੇ (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#ਜੇ_ਅੰਤ
    ਜੇ (idx >= 0)
    {
	spat = get_spat(idx);
	ਜੇ (force || spat->pat == ਨਿਰਾ)
	{
	    val = viminfo_readstring(virp, (ਪੂਰਨ)(lp - virp->vir_ਸਤਰ + 1),
									TRUE);
	    ਜੇ (val != ਨਿਰਾ)
	    {
		ਜੇ (!writing)
		{
		    ਸੈੱਟ_ਆਖਰੀ_ਖੋਜ_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.ਸਤਰ = off_ਸਤਰ;
		    spat->off.ਸਿਰਾ = off_ਸਿਰਾ;
		    spat->off.off = off;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		    ਜੇ (setlast)
			ਸੈੱਟ_no_hlsearch(!hlsearch_on);
#ਜੇ_ਅੰਤ
		}
		vim_free(val);
	    }
	}
    }
    ਵਾਪਸ viminfo_readline(virp);
}

    ਸਥਿਰ ਖਾਲੀ
wvsp_one(
    FILE	*fp,	// file to write to
    ਪੂਰਨ		idx,	// spats[] index
    ਅੱਖਰ	*s,	// search pat
    ਪੂਰਨ		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    ਜੇ (spat->pat == ਨਿਰਾ)
	ਵਾਪਸ;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.ਸਤਰ ? 'L' : 'l',	// line offset
	    spat->off.ਸਿਰਾ  ? 'E' : 'e',	// offset from end
	    spat->off.off,			//ਵਿੱਥ
	    get_spat_ਆਖਰੀ_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    ਸਥਿਰ ਖਾਲੀ
write_viminfo_ਖੋਜ_ਨਮੂਨਾ(FILE *fp)
{
    ਜੇ (get_viminfo_parameter('/') == 0)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || ਲੱਭੋ_viminfo_parameter('h') != ਨਿਰਾ) ? 'h' : 'H');
#ਜੇ_ਅੰਤ
    wvsp_one(fp, RE_ਖੋਜ, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

ਸਥਿਰ yankreg_T *y_read_regs = ਨਿਰਾ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_ਪਿਛਲਾ 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    ਸਥਿਰ ਖਾਲੀ
prepare_viminfo_registers(ਖਾਲੀ)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, ਅੰਕ_REGISTERS);
}

    ਸਥਿਰ ਖਾਲੀ
ਅੰਤ_viminfo_registers(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		j;

    ਜੇ (y_read_regs == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < ਅੰਕ_REGISTERS; ++i)
	ਜੇ (y_read_regs[i].y_array != ਨਿਰਾ)
	{
	    ਲਈ (j = 0; j < y_read_regs[i].y_ਮਾਪ; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    ਸਥਿਰ ਪੂਰਨ
read_viminfo_ਰਜਿਸਟਰ(vir_T *virp, ਪੂਰਨ force)
{
    ਪੂਰਨ		eof;
    ਪੂਰਨ		do_it = TRUE;
    ਪੂਰਨ		ਮਾਪ;
    ਪੂਰਨ		ਹੱਦਬੰਦੀ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਸੈੱਟ_prev = FALSE;
    char_u	*str;
    string_T	*array = ਨਿਰਾ;
    ਪੂਰਨ		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_ਚੌੜਾਈ = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_ਸਤਰ + 1;

    // If the line starts with "" this is the y_previous register.
    ਜੇ (*str == '"')
    {
	ਸੈੱਟ_prev = TRUE;
	str++;
    }

    ਜੇ (!ASCII_ISALNUM(*str) && *str != '-')
    {
	ਜੇ (viminfo_error("E577: ", _(e_illegal_ਰਜਿਸਟਰ_name), virp->vir_ਸਤਰ))
	    ਵਾਪਸ TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_ਰਜਿਸਟਰ(*str++, FALSE);
    y_current_p = get_y_current();
    ਜੇ (!force && y_current_p->y_array != ਨਿਰਾ)
	do_it = FALSE;

    ਜੇ (*str == '@')
    {
	// "x@: register x used for @@
	ਜੇ (force || get_execreg_lastc() == NUL)
	    ਸੈੱਟ_execreg_lastc(str[-1]);
    }

    ਮਾਪ = 0;
    ਹੱਦਬੰਦੀ = 100;	// Optimized for registers containing <= 100 lines
    ਜੇ (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	ਜੇ (ਸੈੱਟ_prev)
	    ਸੈੱਟ_y_ਪਿਛਲਾ(y_current_p);
	array = ALLOC_MULT(string_T, ਹੱਦਬੰਦੀ);
	str = skipwhite(skiptowhite(str));
	ਜੇ (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	ਨਹੀਂਤਾਂ
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_ਚੌੜਾਈ = getdigits(&str);
    }

    ਜਦੋਂ (!(eof = viminfo_readline(virp))
		    && (virp->vir_ਸਤਰ[0] == TAB || virp->vir_ਸਤਰ[0] == '<'))
    {
	ਜੇ (do_it)
	{
	    ਜੇ (ਮਾਪ == ਹੱਦਬੰਦੀ)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)ਹੱਦਬੰਦੀ * 2 * ਆਕਾਰ(string_T));

		ਜੇ (new_array == ਨਿਰਾ)
		{
		    do_it = FALSE;
		    ਰੋਕੋ;
		}
		ਲਈ (i = 0; i < ਹੱਦਬੰਦੀ; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		ਹੱਦਬੰਦੀ *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    ਜੇ (str != ਨਿਰਾ)
	    {
		array[ਮਾਪ].string = str;
		array[ਮਾਪ].ਲੰਮਾਈ = STRLEN(str);
		++ਮਾਪ;
	    }
	    ਨਹੀਂਤਾਂ
		// error, don't store the result
		do_it = FALSE;
	}
    }

    ਜੇ (do_it)
    {
	// free y_array[]
	ਲਈ (i = 0; i < y_current_p->y_ਮਾਪ; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_ਚੌੜਾਈ = new_ਚੌੜਾਈ;
	y_current_p->y_ਮਾਪ = ਮਾਪ;
	y_current_p->y_ਸਮਾਂ_ਸੈੱਟ = 0;
	ਜੇ (ਮਾਪ == 0)
	{
	    y_current_p->y_array = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, ਮਾਪ);
	    ਲਈ (i = 0; i < ਮਾਪ; i++)
	    {
		ਜੇ (y_current_p->y_array == ਨਿਰਾ)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		ਨਹੀਂਤਾਂ
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].ਲੰਮਾਈ = array[i].ਲੰਮਾਈ;
		}
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Free array[] if it was filled.
	ਲਈ (i = 0; i < ਮਾਪ; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    ਵਾਪਸ eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_viminfo_ਰਜਿਸਟਰ(garray_T *ਮੁੱਲਾਂ, ਪੂਰਨ force)
{
    bval_T	*vp = (bval_T *)ਮੁੱਲਾਂ->ga_ਅੰਕੜੇ;
    ਪੂਰਨ		flags;
    ਪੂਰਨ		name;
    ਪੂਰਨ		type;
    ਪੂਰਨ		linecount;
    ਪੂਰਨ		ਚੌੜਾਈ;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    ਪੂਰਨ		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    ਜੇ (ਮੁੱਲਾਂ->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	ਵਾਪਸ;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    ਜੇ (name < 0 || name >= ਅੰਕ_REGISTERS)
	ਵਾਪਸ;
    type = vp[2].bv_nr;
    ਜੇ (type != MCHAR && type != MLINE && type != MBLOCK)
	ਵਾਪਸ;
    linecount = vp[3].bv_nr;
    ਜੇ (ਮੁੱਲਾਂ->ga_len < 6 + linecount)
	ਵਾਪਸ;
    ਚੌੜਾਈ = vp[4].bv_nr;
    ਜੇ (ਚੌੜਾਈ < 0)
	ਵਾਪਸ;

    ਜੇ (y_read_regs != ਨਿਰਾ)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    ਨਹੀਂਤਾਂ
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    ਜੇ (y_ptr->y_array != ਨਿਰਾ && !force
			 && (timestamp == 0 || y_ptr->y_ਸਮਾਂ_ਸੈੱਟ > timestamp))
	ਵਾਪਸ;

    ਜੇ (y_ptr->y_array != ਨਿਰਾ)
	ਲਈ (i = 0; i < y_ptr->y_ਮਾਪ; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    ਜੇ (y_read_regs == ਨਿਰਾ)
    {
	ਜੇ (flags & REG_ਪਿਛਲਾ)
	    ਸੈੱਟ_y_ਪਿਛਲਾ(y_ptr);
	ਜੇ ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    ਸੈੱਟ_execreg_lastc(get_ਰਜਿਸਟਰ_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_ਚੌੜਾਈ = ਚੌੜਾਈ;
    y_ptr->y_ਮਾਪ = linecount;
    y_ptr->y_ਸਮਾਂ_ਸੈੱਟ = timestamp;
    ਜੇ (linecount == 0)
    {
	y_ptr->y_array = ਨਿਰਾ;
	ਵਾਪਸ;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    ਜੇ (y_ptr->y_array == ਨਿਰਾ)
    {
	y_ptr->y_ਮਾਪ = 0; // ensure object state is consistent
	ਵਾਪਸ;
    }
    ਲਈ (i = 0; i < linecount; i++)
    {
	ਜੇ (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].ਲੰਮਾਈ = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vp[i + 6].bv_type != BVAL_STRING)
	{
	    ਮੁਕਤ(y_ptr->y_array);
	    y_ptr->y_array = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].ਲੰਮਾਈ = vp[i + 6].bv_len;
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
write_viminfo_registers(FILE *fp)
{
    ਪੂਰਨ		i, j;
    char_u	*type;
    char_u	c;
    ਪੂਰਨ		ਅੰਕ_ਸਤਰਾਂ;
    ਪੂਰਨ		max_ਅੰਕ_ਸਤਰਾਂ;
    ਪੂਰਨ		max_kbyte;
    ਲੰਮਾ	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_ਅੰਕ_ਸਤਰਾਂ = get_viminfo_parameter('<');
    ਜੇ (max_ਅੰਕ_ਸਤਰਾਂ < 0)
	max_ਅੰਕ_ਸਤਰਾਂ = get_viminfo_parameter('"');
    ਜੇ (max_ਅੰਕ_ਸਤਰਾਂ == 0)
	ਵਾਪਸ;
    max_kbyte = get_viminfo_parameter('s');
    ਜੇ (max_kbyte == 0)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < ਅੰਕ_REGISTERS; i++)
    {
#ਜੇ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	ਜੇ (i == ਤਾਰਾ_ਰਜਿਸਟਰ || i == PLUS_ਰਜਿਸਟਰ || i == REAL_PLUS_ਰਜਿਸਟਰ)
	    ਜਾਰੀ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
	// Neither do we want the '~' register
	ਜੇ (i == TILDE_ਰਜਿਸਟਰ)
	    ਜਾਰੀ;
#ਜੇ_ਅੰਤ
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	ਜੇ (y_read_regs != ਨਿਰਾ
		&& y_read_regs[i].y_array != ਨਿਰਾ
		&& (y_regs_p[i].y_array == ਨਿਰਾ ||
			    y_read_regs[i].y_ਸਮਾਂ_ਸੈੱਟ > y_regs_p[i].y_ਸਮਾਂ_ਸੈੱਟ))
	    y_ptr = &y_read_regs[i];
	ਨਹੀਂਤਾਂ ਜੇ (y_regs_p[i].y_array == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਨਹੀਂਤਾਂ
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	ਅੰਕ_ਸਤਰਾਂ = y_ptr->y_ਮਾਪ;
	ਜੇ (ਅੰਕ_ਸਤਰਾਂ == 0
		|| (ਅੰਕ_ਸਤਰਾਂ == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    ਜਾਰੀ;

	ਜੇ (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    ਲਈ (j = 0; j < ਅੰਕ_ਸਤਰਾਂ; j++)
		len += (ਲੰਮਾ)y_ptr->y_array[j].ਲੰਮਾਈ + 1L;
	    ਜੇ (len > (ਲੰਮਾ)max_kbyte * 1024L)
		ਜਾਰੀ;
	}

	ਚੋਣ (y_ptr->y_type)
	{
	    ਹਾਲਤ MLINE:
		type = (char_u *)"LINE";
		ਰੋਕੋ;
	    ਹਾਲਤ MCHAR:
		type = (char_u *)"CHAR";
		ਰੋਕੋ;
	    ਹਾਲਤ MBLOCK:
		type = (char_u *)"BLOCK";
		ਰੋਕੋ;
	    ਮੂਲ:
		semsg(_(e_unknown_ਰਜਿਸਟਰ_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		ਰੋਕੋ;
	}
	ਜੇ (get_y_ਪਿਛਲਾ() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_ਰਜਿਸਟਰ_name(i);
	fprintf(fp, "\"%c", c);
	ਜੇ (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (ਪੂਰਨ)y_ptr->y_ਚੌੜਾਈ);

	// If max_num_lines < 0, then we save ALL the lines in the register
	ਜੇ (max_ਅੰਕ_ਸਤਰਾਂ > 0 && ਅੰਕ_ਸਤਰਾਂ > max_ਅੰਕ_ਸਤਰਾਂ)
	    ਅੰਕ_ਸਤਰਾਂ = max_ਅੰਕ_ਸਤਰਾਂ;
	ਲਈ (j = 0; j < ਅੰਕ_ਸਤਰਾਂ; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    ਪੂਰਨ	    flags = 0;
	    ਪੂਰਨ	    ਬਚਿਆ;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    ਜੇ (get_y_ਪਿਛਲਾ() == &y_regs_p[i])
		flags |= REG_ਪਿਛਲਾ;
	    ਜੇ (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_ਰਜਿਸਟਰ, flags,
		    i, y_ptr->y_type, ਅੰਕ_ਸਤਰਾਂ, (ਪੂਰਨ)y_ptr->y_ਚੌੜਾਈ,
		    (ਲੰਮਾ)y_ptr->y_ਸਮਾਂ_ਸੈੱਟ);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    ਬਚਿਆ = LSIZE - 71;
	    ਲਈ (j = 0; j < ਅੰਕ_ਸਤਰਾਂ; j++)
	    {
		putc(',', fp);
		--ਬਚਿਆ;
		ਬਚਿਆ = barline_writestring(fp, y_ptr->y_array[j].string,
								   ਬਚਿਆ);
	    }
	    putc('\n', fp);
	}
    }
}

/*
 * Functions relating to reading/writing marks from viminfo
 */

ਸਥਿਰ xfmark_T *vi_namedfm = ਨਿਰਾ;
ਸਥਿਰ xfmark_T *vi_jumplist = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ vi_jumplist_len = 0;

    ਸਥਿਰ ਖਾਲੀ
write_one_mark(FILE *fp_ਬਾਹਰਲਾ, ਪੂਰਨ c, pos_T *pos)
{
    ਜੇ (pos->lnum != 0)
	fprintf(fp_ਬਾਹਰਲਾ, "\t%c\t%ld\t%d\n", c, (ਲੰਮਾ)pos->lnum, (ਪੂਰਨ)pos->ਖਾਨਾ);
}

    ਸਥਿਰ ਖਾਲੀ
write_ਬਫਰ_marks(buf_T *buf, FILE *fp_ਬਾਹਰਲਾ)
{
    ਪੂਰਨ		i;
    pos_T	pos;

    home_replace(ਨਿਰਾ, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_ਬਾਹਰਲਾ, "\n> ");
    viminfo_writestring(fp_ਬਾਹਰਲਾ, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_ਆਖਰੀ_ਵਰਤਿਆ;
    pos.ਖਾਨਾ = 0;
    write_one_mark(fp_ਬਾਹਰਲਾ, '*', &pos);

    write_one_mark(fp_ਬਾਹਰਲਾ, '"', &buf->b_ਆਖਰੀ_cursor);
    write_one_mark(fp_ਬਾਹਰਲਾ, '^', &buf->b_ਆਖਰੀ_ਪਾਓ);
    write_one_mark(fp_ਬਾਹਰਲਾ, '.', &buf->b_ਆਖਰੀ_change);
    // changelist positions are stored oldest first
    ਲਈ (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	ਜੇ (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_ਬਾਹਰਲਾ, '+', &buf->b_changelist[i]);
    }
    ਲਈ (i = 0; i < NMARKS; i++)
	write_one_mark(fp_ਬਾਹਰਲਾ, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    ਸਥਿਰ ਪੂਰਨ
skip_for_viminfo(buf_T *buf)
{
    ਵਾਪਸ bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    ਸਥਿਰ ਖਾਲੀ
write_viminfo_marks(FILE *fp_ਬਾਹਰਲਾ, garray_T *buflist)
{
    buf_T	*buf;
    ਪੂਰਨ		is_mark_ਸੈੱਟ;
    ਪੂਰਨ		i;
    win_T	*win;
    tabpage_T	*tp;

    // Set b_last_cursor for the all buffers that have a window.
    FOR_ALL_TAB_WINDOWS(tp, win)
	ਸੈੱਟ_ਆਖਰੀ_cursor(win);

    fputs(_("\n# History of marks within files (newest to oldest):\n"), fp_ਬਾਹਰਲਾ);
    FOR_ALL_BUFFERS(buf)
    {
	// Only write something if buffer has been loaded and at least one
	// mark is set.
	ਜੇ (buf->b_marks_read)
	{
	    ਜੇ (buf->b_ਆਖਰੀ_cursor.lnum != 0)
		is_mark_ਸੈੱਟ = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		is_mark_ਸੈੱਟ = FALSE;
		ਲਈ (i = 0; i < NMARKS; i++)
		    ਜੇ (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_ਸੈੱਟ = TRUE;
			ਰੋਕੋ;
		    }
	    }
	    ਜੇ (is_mark_ਸੈੱਟ && buf->b_ffname != ਨਿਰਾ
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		ਜੇ (buflist == ਨਿਰਾ)
		    write_ਬਫਰ_marks(buf, fp_ਬਾਹਰਲਾ);
		ਨਹੀਂਤਾਂ ਜੇ (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_ਅੰਕੜੇ)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    ਪੂਰਨ		c1,
    ਪੂਰਨ		c2)
{
    char_u	*name;

    ਜੇ (fm->fmark.mark.lnum == 0)	// not set
	ਵਾਪਸ;

    ਜੇ (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    ਨਹੀਂਤਾਂ
	name = fm->fname;		// use name from .viminfo
    ਜੇ (name != ਨਿਰਾ && *name != NUL)
    {
	fprintf(fp, "%c%c  %ld  %ld  ", c1, c2, (ਲੰਮਾ)fm->fmark.mark.lnum,
						    (ਲੰਮਾ)fm->fmark.mark.ਖਾਨਾ);
	viminfo_writestring(fp, name);

	// Barline: |{bartype},{name},{lnum},{col},{timestamp},{filename}
	// size up to filename: 8 + 3 * 20
	fprintf(fp, "|%d,%d,%ld,%ld,%ld,", BARTYPE_MARK, c2,
		(ਲੰਮਾ)fm->fmark.mark.lnum, (ਲੰਮਾ)fm->fmark.mark.ਖਾਨਾ,
		(ਲੰਮਾ)fm->ਸਮਾਂ_ਸੈੱਟ);
	barline_writestring(fp, name, LSIZE - 70);
	putc('\n', fp);
    }

    ਜੇ (fm->fmark.fnum != 0)
	vim_free(name);
}

    ਸਥਿਰ ਖਾਲੀ
write_viminfo_filemarks(FILE *fp)
{
    ਪੂਰਨ		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    ਪੂਰਨ		vi_idx;
    ਪੂਰਨ		idx;

    ਜੇ (get_viminfo_parameter('f') == 0)
	ਵਾਪਸ;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    ਲਈ (i = 0; i < NMARKS; i++)
    {
	ਜੇ (vi_namedfm != ਨਿਰਾ
			&& (vi_namedfm[i].ਸਮਾਂ_ਸੈੱਟ > namedfm_p[i].ਸਮਾਂ_ਸੈੱਟ))
	    fm = &vi_namedfm[i];
	ਨਹੀਂਤਾਂ
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    ਜੇ (curbuf->b_ffname != ਨਿਰਾ && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	ਲਈ (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    ਜੇ (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == ਨਿਰਾ
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != ਨਿਰਾ
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		ਰੋਕੋ;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	ਲਈ ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = ਨਿਰਾ;
	namedfm_p[NMARKS].ਸਮਾਂ_ਸੈੱਟ = vim_ਸਮਾਂ();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    ਲਈ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != ਨਿਰਾ ? &vi_namedfm[vi_idx] : ਨਿਰਾ;

	ਜੇ (vi_fm != ਨਿਰਾ
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->ਸਮਾਂ_ਸੈੱਟ > namedfm_p[idx].ਸਮਾਂ_ਸੈੱਟ
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ਨਹੀਂਤਾਂ
	{
	    fm = &namedfm_p[idx++];
	    ਜੇ (vi_fm != ਨਿਰਾ
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->ਸਮਾਂ_ਸੈੱਟ == fm->ਸਮਾਂ_ਸੈੱਟ
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != ਨਿਰਾ
			  && fm->fname != ਨਿਰਾ
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
    ਲਈ (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : ਨਿਰਾ;
	vi_fm = (vi_jumplist != ਨਿਰਾ && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : ਨਿਰਾ;
	ਜੇ (fm == ਨਿਰਾ && vi_fm == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ (fm == ਨਿਰਾ || (vi_fm != ਨਿਰਾ && fm->ਸਮਾਂ_ਸੈੱਟ < vi_fm->ਸਮਾਂ_ਸੈੱਟ))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ਨਹੀਂਤਾਂ
	    --idx;
	ਜੇ (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != ਨਿਰਾ
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    ਪੂਰਨ
buf_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    ਜੇ (buf1->b_ਆਖਰੀ_ਵਰਤਿਆ == buf2->b_ਆਖਰੀ_ਵਰਤਿਆ)
	ਵਾਪਸ 0;
    ਵਾਪਸ buf1->b_ਆਖਰੀ_ਵਰਤਿਆ > buf2->b_ਆਖਰੀ_ਵਰਤਿਆ ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    ਸਥਿਰ ਖਾਲੀ
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_ਬਾਹਰਲਾ,
    garray_T	*buflist,
    ਪੂਰਨ		eof,
    ਪੂਰਨ		flags)
{
    char_u	*ਸਤਰ = virp->vir_ਸਤਰ;
    buf_T	*buf;
    ਪੂਰਨ		ਅੰਕ_marked_files;
    ਪੂਰਨ		ਲੱਦੋ_marks;
    ਪੂਰਨ		copy_marks_ਬਾਹਰਲਾ;
    char_u	*str;
    ਪੂਰਨ		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    list_T	*list = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ		buflist_ਵਰਤਿਆ = 0;
    buf_T	*buflist_buf = ਨਿਰਾ;

    ਜੇ ((name_buf = alloc(LSIZE)) == ਨਿਰਾ)
	ਵਾਪਸ;
    *name_buf = NUL;

    ਜੇ (fp_ਬਾਹਰਲਾ != ਨਿਰਾ && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_ਅੰਕੜੇ, (size_t)buflist->ga_len,
						ਆਕਾਰ(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_ਅੰਕੜੇ)[0];
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (fp_ਬਾਹਰਲਾ == ਨਿਰਾ && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	ਜੇ (list != ਨਿਰਾ)
	    ਸੈੱਟ_vim_var_list(VV_OLDFILES, list);
    }
#ਜੇ_ਅੰਤ

    ਅੰਕ_marked_files = get_viminfo_parameter('\'');
    ਜਦੋਂ (!eof && (ਗਿਣਤੀ_2 < ਅੰਕ_marked_files || fp_ਬਾਹਰਲਾ == ਨਿਰਾ))
    {
	ਜੇ (ਸਤਰ[0] != '>')
	{
	    ਜੇ (ਸਤਰ[0] != '\n' && ਸਤਰ[0] != '\r' && ਸਤਰ[0] != '#')
	    {
		ਜੇ (viminfo_error("E576: ", _(e_nonr_missing_gt), ਸਤਰ))
		    ਰੋਕੋ;	// too many errors, return now
	    }
	    eof = vim_fgets(ਸਤਰ, LSIZE, virp->vir_fd);
	    ਜਾਰੀ;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(ਸਤਰ + 1);
	str = viminfo_readstring(virp, (ਪੂਰਨ)(str - virp->vir_ਸਤਰ), FALSE);
	ਜੇ (str == ਨਿਰਾ)
	    ਜਾਰੀ;
	p = str + STRLEN(str);
	ਜਦੋਂ (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	ਜੇ (*p)
	    p++;
	*p = NUL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (list != ਨਿਰਾ)
	    list_append_string(list, str, -1);
#ਜੇ_ਅੰਤ

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	ਲੱਦੋ_marks = copy_marks_ਬਾਹਰਲਾ = FALSE;
	ਜੇ (fp_ਬਾਹਰਲਾ == ਨਿਰਾ)
	{
	    ਜੇ ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != ਨਿਰਾ)
	    {
		ਜੇ (*name_buf == NUL)	    // only need to do this once
		    home_replace(ਨਿਰਾ, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		ਜੇ (fnamecmp(str, name_buf) == 0)
		    ਲੱਦੋ_marks = TRUE;
	    }
	}
	ਨਹੀਂਤਾਂ // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		ਜੇ (buf->b_ffname != ਨਿਰਾ)
		{
		    home_replace(ਨਿਰਾ, buf->b_ffname, name_buf, LSIZE, TRUE);
		    ਜੇ (fnamecmp(str, name_buf) == 0)
			ਰੋਕੋ;
		}

	    // Copy marks if the buffer has not been loaded.
	    ਜੇ (buf == ਨਿਰਾ || !buf->b_marks_read)
	    {
		ਪੂਰਨ	did_read_ਸਤਰ = FALSE;

		ਜੇ (buflist_buf != ਨਿਰਾ)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    ਜੇ (!viminfo_readline(virp) && ਸਤਰ[0] == TAB)
		    {
			did_read_ਸਤਰ = TRUE;
			ਜੇ (ਸਤਰ[1] == '*')
			{
			    ਲੰਮਾ	ltime;

			    ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)ਸਤਰ + 2, "%ld ", &ltime);
			    ਜਦੋਂ ((ਸਮਾਂ_T)ltime < buflist_buf->b_ਆਖਰੀ_ਵਰਤਿਆ)
			    {
				write_ਬਫਰ_marks(buflist_buf, fp_ਬਾਹਰਲਾ);
				ਜੇ (++ਗਿਣਤੀ_2 >= ਅੰਕ_marked_files)
				    ਰੋਕੋ;
				ਜੇ (++buflist_ਵਰਤਿਆ == buflist->ga_len)
				{
				    buflist_buf = ਨਿਰਾ;
				    ਰੋਕੋ;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_ਅੰਕੜੇ)[buflist_ਵਰਤਿਆ];
			    }
			}
			ਨਹੀਂਤਾਂ
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    ਜਦੋਂ (ਗਿਣਤੀ_2 < ਅੰਕ_marked_files
					    && buflist_ਵਰਤਿਆ < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_ਅੰਕੜੇ)
							     [buflist_ਵਰਤਿਆ++];
				write_ਬਫਰ_marks(buflist_buf, fp_ਬਾਹਰਲਾ);
				++ਗਿਣਤੀ_2;
			    }
			    buflist_buf = ਨਿਰਾ;
			}

			ਜੇ (ਗਿਣਤੀ_2 >= ਅੰਕ_marked_files)
			{
			    vim_free(str);
			    ਰੋਕੋ;
			}
		    }
		}

		fputs("\n> ", fp_ਬਾਹਰਲਾ);
		viminfo_writestring(fp_ਬਾਹਰਲਾ, str);
		ਜੇ (did_read_ਸਤਰ)
		    fputs((ਅੱਖਰ *)ਸਤਰ, fp_ਬਾਹਰਲਾ);

		ਗਿਣਤੀ_2++;
		copy_marks_ਬਾਹਰਲਾ = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	ਜਦੋਂ (!(eof = viminfo_readline(virp)) && ਸਤਰ[0] == TAB)
	{
	    ਜੇ (ਲੱਦੋ_marks)
	    {
		ਜੇ (ਸਤਰ[1] != NUL)
		{
		    ਬਿਨਾਨਿਸ਼ਾਨੀ u;

		    ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)ਸਤਰ + 2, "%ld %u", &pos.lnum, &u);
		    pos.ਖਾਨਾ = u;
		    ਚੋਣ (ਸਤਰ[1])
		    {
			ਹਾਲਤ '"': curbuf->b_ਆਖਰੀ_cursor = pos; ਰੋਕੋ;
			ਹਾਲਤ '^': curbuf->b_ਆਖਰੀ_ਪਾਓ = pos; ਰੋਕੋ;
			ਹਾਲਤ '.': curbuf->b_ਆਖਰੀ_change = pos; ਰੋਕੋ;
			ਹਾਲਤ '+':
				  // changelist positions are stored oldest
				  //ਪਹਿਲਾ
				  ਜੇ (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    ਆਕਾਰ(pos_T) * (JUMPLISTSIZE - 1));
				  ਨਹੀਂਤਾਂ
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  ਰੋਕੋ;

				  // Using the line number for the last-used
				  // timestamp.
			ਹਾਲਤ '*': curbuf->b_ਆਖਰੀ_ਵਰਤਿਆ = pos.lnum; ਰੋਕੋ;

			ਮੂਲ:  ਜੇ ((i = ਸਤਰ[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (copy_marks_ਬਾਹਰਲਾ)
		fputs((ਅੱਖਰ *)ਸਤਰ, fp_ਬਾਹਰਲਾ);
	}

	ਜੇ (ਲੱਦੋ_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		ਜੇ (wp->w_ਬਫਰ == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    ਜੇ (flags & VIF_ONLY_CURBUF)
		ਰੋਕੋ;
	}
    }

    ਜੇ (fp_ਬਾਹਰਲਾ != ਨਿਰਾ)
	// Write any remaining entries from buflist.
	ਜਦੋਂ (ਗਿਣਤੀ_2 < ਅੰਕ_marked_files && buflist_ਵਰਤਿਆ < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_ਅੰਕੜੇ)[buflist_ਵਰਤਿਆ++];
	    write_ਬਫਰ_marks(buflist_buf, fp_ਬਾਹਰਲਾ);
	    ++ਗਿਣਤੀ_2;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    ਖਾਲੀ
check_marks_read(ਖਾਲੀ)
{
    ਜੇ (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != ਨਿਰਾ)
	read_viminfo(ਨਿਰਾ, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    ਸਥਿਰ ਪੂਰਨ
read_viminfo_filemark(vir_T *virp, ਪੂਰਨ force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    ਪੂਰਨ		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_ਸਤਰ + 1;
    ਜੇ (*str <= 127
	    && ((*virp->vir_ਸਤਰ == '\''
				       && (VIM_ISDIGIT(*str) || ਸੁਰੱਖਿਅਤ_isupper(*str)))
	     || (*virp->vir_ਸਤਰ == '-' && *str == '\'')))
    {
	ਜੇ (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    ਜੇ (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਲਈ (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = ਨਿਰਾ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	ਨਹੀਂਤਾਂ
	    fm = &namedfm_p[*str - 'A'];
	ਜੇ (fm != ਨਿਰਾ && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.ਖਾਨਾ = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (ਪੂਰਨ)(str - virp->vir_ਸਤਰ),
								       FALSE);
	    fm->ਸਮਾਂ_ਸੈੱਟ = 0;
	}
    }
    ਵਾਪਸ vim_fgets(virp->vir_ਸਤਰ, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    ਸਥਿਰ ਖਾਲੀ
prepare_viminfo_marks(ਖਾਲੀ)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    ਸਥਿਰ ਖਾਲੀ
ਅੰਤ_viminfo_marks(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    ਜੇ (vi_namedfm != ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    ਜੇ (vi_jumplist != ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_viminfo_mark(garray_T *ਮੁੱਲਾਂ, ਪੂਰਨ force)
{
    bval_T	*vp = (bval_T *)ਮੁੱਲਾਂ->ga_ਅੰਕੜੇ;
    ਪੂਰਨ		name;
    linenr_T	lnum;
    colnr_T	ਖਾਨਾ;
    time_t	timestamp;
    xfmark_T	*fm = ਨਿਰਾ;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    ਜੇ (ਮੁੱਲਾਂ->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	ਵਾਪਸ;

    name = vp[0].bv_nr;
    ਜੇ (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	ਵਾਪਸ;
    lnum = vp[1].bv_nr;
    ਖਾਨਾ = vp[2].bv_nr;
    ਜੇ (lnum <= 0 || ਖਾਨਾ < 0)
	ਵਾਪਸ;
    timestamp = (time_t)vp[3].bv_nr;

    ਜੇ (name == '\'')
    {
	ਜੇ (vi_jumplist != ਨਿਰਾ)
	{
	    ਜੇ (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ idx;
	    ਪੂਰਨ i;

	    // If we have a timestamp insert it in the right place.
	    ਜੇ (timestamp != 0)
	    {
		ਲਈ (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    ਜੇ (curwin->w_jumplist[idx].ਸਮਾਂ_ਸੈੱਟ < timestamp)
		    {
			++idx;
			ਰੋਕੋ;
		    }
		// idx cannot be zero now
		ਜੇ (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    ਨਹੀਂਤਾਂ
		idx = -1;

	    ਜੇ (idx >= 0)
	    {
		ਜੇ (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    ਲਈ (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		ਨਹੀਂਤਾਂ
		{
		    // Move newer entries forward.
		    ਲਈ (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = ਨਿਰਾ;
		fm->ਸਮਾਂ_ਸੈੱਟ = 0;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	ਜੇ (VIM_ISDIGIT(name))
	{
	    ਜੇ (vi_namedfm != ਨਿਰਾ)
		idx = name - '0' + NMARKS;
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		ਲਈ (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    ਜੇ (namedfm_p[idx].ਸਮਾਂ_ਸੈੱਟ < timestamp)
			ਰੋਕੋ;
		ਜੇ (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    ਵਾਪਸ;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		ਲਈ ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = ਨਿਰਾ;
	    }
	}
	ਨਹੀਂਤਾਂ
	    idx = name - 'A';
	ਜੇ (vi_namedfm != ਨਿਰਾ)
	    fm = &vi_namedfm[idx];
	ਨਹੀਂਤਾਂ
	    fm = &namedfm_p[idx];
    }

    ਜੇ (fm != ਨਿਰਾ)
    {
	ਜੇ (vi_namedfm != ਨਿਰਾ || fm->fmark.mark.lnum == 0
					  || fm->ਸਮਾਂ_ਸੈੱਟ < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.ਖਾਨਾ = ਖਾਨਾ;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    ਜੇ (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->ਸਮਾਂ_ਸੈੱਟ = timestamp;
	}
    }
}

    ਸਥਿਰ ਪੂਰਨ
read_viminfo_barline(vir_T *virp, ਪੂਰਨ got_encoding, ਪੂਰਨ force, ਪੂਰਨ writing)
{
    char_u	*p = virp->vir_ਸਤਰ + 1;
    ਪੂਰਨ		bartype;
    garray_T	ਮੁੱਲਾਂ;
    bval_T	*vp;
    ਪੂਰਨ		i;
    ਪੂਰਨ		read_ਅਗਲਾ = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    ਜੇ (*p == '<')
    {
	// Continuation line of an unrecognized item.
	ਜੇ (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_ਸਤਰ);
    }
    ਨਹੀਂਤਾਂ
    {
	ga_init2(&ਮੁੱਲਾਂ, ਆਕਾਰ(bval_T), 20);
	bartype = getdigits(&p);
	ਚੋਣ (bartype)
	{
	    ਹਾਲਤ BARTYPE_ਸੰਸਕਰਣ:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		ਜੇ (!got_encoding)
		{
		    read_ਅਗਲਾ = barline_parse(virp, p, &ਮੁੱਲਾਂ);
		    vp = (bval_T *)ਮੁੱਲਾਂ.ga_ਅੰਕੜੇ;
		    ਜੇ (ਮੁੱਲਾਂ.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_ਸੰਸਕਰਣ = vp->bv_nr;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ BARTYPE_HISTORY:
		read_ਅਗਲਾ = barline_parse(virp, p, &ਮੁੱਲਾਂ);
		handle_viminfo_history(&ਮੁੱਲਾਂ, writing);
		ਰੋਕੋ;

	    ਹਾਲਤ BARTYPE_ਰਜਿਸਟਰ:
		read_ਅਗਲਾ = barline_parse(virp, p, &ਮੁੱਲਾਂ);
		handle_viminfo_ਰਜਿਸਟਰ(&ਮੁੱਲਾਂ, force);
		ਰੋਕੋ;

	    ਹਾਲਤ BARTYPE_MARK:
		read_ਅਗਲਾ = barline_parse(virp, p, &ਮੁੱਲਾਂ);
		handle_viminfo_mark(&ਮੁੱਲਾਂ, force);
		ਰੋਕੋ;

	    ਮੂਲ:
		// copy unrecognized line (for future use)
		ਜੇ (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_ਸਤਰ);
	}
	ਲਈ (i = 0; i < ਮੁੱਲਾਂ.ga_len; ++i)
	{
	    vp = (bval_T *)ਮੁੱਲਾਂ.ga_ਅੰਕੜੇ + i;
	    ਜੇ (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&ਮੁੱਲਾਂ);
    }

    ਜੇ (read_ਅਗਲਾ)
	ਵਾਪਸ viminfo_readline(virp);
    ਵਾਪਸ FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    ਸਥਿਰ ਪੂਰਨ
read_viminfo_ਉੱਪਰ_ਮੰਜ਼ਿਲ_marks(
    vir_T	*virp,
    ਪੂਰਨ		forceit,
    ਪੂਰਨ		writing)
{
    ਪੂਰਨ		eof;
    buf_T	*buf;
    ਪੂਰਨ		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    ਜਦੋਂ (!eof && virp->vir_ਸਤਰ[0] != '>')
    {
	ਚੋਣ (virp->vir_ਸਤਰ[0])
	{
		// Characters reserved for future expansion, ignored now
	    ਹਾਲਤ '+': // "+40 /path/dir file", for running vim without args
	    ਹਾਲਤ '^': // to be defined
	    ਹਾਲਤ '<': // long line - ignored
		// A comment or empty line.
	    ਹਾਲਤ NUL:
	    ਹਾਲਤ '\r':
	    ਹਾਲਤ '\n':
	    ਹਾਲਤ '#':
		eof = viminfo_readline(virp);
		ਰੋਕੋ;
	    ਹਾਲਤ '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		ਰੋਕੋ;
	    ਹਾਲਤ '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		ਰੋਕੋ;
	    ਹਾਲਤ '!': // global variable
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#ਨਹੀਂਤਾਂ
		eof = viminfo_readline(virp);
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    ਹਾਲਤ '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		ਰੋਕੋ;
	    ਹਾਲਤ '"':
		// When registers are in bar lines skip the old style register
		// lines.
		ਜੇ (virp->vir_ਸੰਸਕਰਣ < VIMINFO_ਸੰਸਕਰਣ_WITH_REGISTERS)
		    eof = read_viminfo_ਰਜਿਸਟਰ(virp, forceit);
		ਨਹੀਂਤਾਂ
		    ਕਰੋ
		    {
			eof = viminfo_readline(virp);
		    } ਜਦੋਂ (!eof && (virp->vir_ਸਤਰ[0] == TAB
						|| virp->vir_ਸਤਰ[0] == '<'));
		ਰੋਕੋ;
	    ਹਾਲਤ '/':	    // Search string
	    ਹਾਲਤ '&':	    // Substitute search string
	    ਹਾਲਤ '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_ਖੋਜ_ਨਮੂਨਾ(virp, forceit, writing);
		ਰੋਕੋ;
	    ਹਾਲਤ '$':
		eof = read_viminfo_sub_string(virp, forceit);
		ਰੋਕੋ;
	    ਹਾਲਤ ':':
	    ਹਾਲਤ '?':
	    ਹਾਲਤ '=':
	    ਹਾਲਤ '@':
		// When history is in bar lines skip the old style history
		// lines.
		ਜੇ (virp->vir_ਸੰਸਕਰਣ < VIMINFO_ਸੰਸਕਰਣ_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		ਨਹੀਂਤਾਂ
		    eof = viminfo_readline(virp);
		ਰੋਕੋ;
	    ਹਾਲਤ '-':
	    ਹਾਲਤ '\'':
		// When file marks are in bar lines skip the old style lines.
		ਜੇ (virp->vir_ਸੰਸਕਰਣ < VIMINFO_ਸੰਸਕਰਣ_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		ਨਹੀਂਤਾਂ
		    eof = viminfo_readline(virp);
		ਰੋਕੋ;
	    ਮੂਲ:
		ਜੇ (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_ਸਤਰ))
		    eof = TRUE;
		ਨਹੀਂਤਾਂ
		    eof = viminfo_readline(virp);
		ਰੋਕੋ;
	}
    }

    // Finish reading history items.
    ਜੇ (!writing)
	ਅੰਤ_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_ਨਾਂ(buf);

    ਵਾਪਸ eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    ਸਥਿਰ ਖਾਲੀ
do_viminfo(FILE *fp_ਵਿੱਚ, FILE *fp_ਬਾਹਰਲਾ, ਪੂਰਨ flags)
{
    ਪੂਰਨ		eof = FALSE;
    vir_T	vir;
    ਪੂਰਨ		ਮਿਲਾਓ = FALSE;
    ਪੂਰਨ		do_copy_marks = FALSE;
    garray_T	buflist;

    ਜੇ ((vir.vir_ਸਤਰ = alloc(LSIZE)) == ਨਿਰਾ)
	ਵਾਪਸ;
    vir.vir_fd = fp_ਵਿੱਚ;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, ਆਕਾਰ(char_u *), 100);
    vir.vir_ਸੰਸਕਰਣ = -1;

    ਜੇ (fp_ਵਿੱਚ != ਨਿਰਾ)
    {
	ਜੇ (flags & VIF_WANT_INFO)
	{
	    ਜੇ (fp_ਬਾਹਰਲਾ != ਨਿਰਾ)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_ਉੱਪਰ_ਮੰਜ਼ਿਲ_marks(&vir,
					 flags & VIF_FORCEIT, fp_ਬਾਹਰਲਾ != ਨਿਰਾ);
	    ਮਿਲਾਓ = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (flags != 0)
	    // Skip info, find start of marks
	    ਜਦੋਂ (!(eof = viminfo_readline(&vir))
		    && vir.vir_ਸਤਰ[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    ਜੇ (fp_ਬਾਹਰਲਾ != ਨਿਰਾ)
    {
	// Write the info:
	fprintf(fp_ਬਾਹਰਲਾ, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_ਸੰਸਕਰਣ_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_ਬਾਹਰਲਾ);
	write_viminfo_ਸੰਸਕਰਣ(fp_ਬਾਹਰਲਾ);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_ਬਾਹਰਲਾ);
	fprintf(fp_ਬਾਹਰਲਾ, "*encoding=%s\n\n", p_enc);
	write_viminfo_ਖੋਜ_ਨਮੂਨਾ(fp_ਬਾਹਰਲਾ);
	write_viminfo_sub_string(fp_ਬਾਹਰਲਾ);
	write_viminfo_history(fp_ਬਾਹਰਲਾ, ਮਿਲਾਓ);
	write_viminfo_registers(fp_ਬਾਹਰਲਾ);
	ਅੰਤ_viminfo_registers();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	write_viminfo_varlist(fp_ਬਾਹਰਲਾ);
#ਜੇ_ਅੰਤ
	write_viminfo_filemarks(fp_ਬਾਹਰਲਾ);
	ਅੰਤ_viminfo_marks();
	write_viminfo_bufferlist(fp_ਬਾਹਰਲਾ);
	write_viminfo_barlines(&vir, fp_ਬਾਹਰਲਾ);

	ਜੇ (do_copy_marks)
	    ga_init2(&buflist, ਆਕਾਰ(buf_T *), 50);
	write_viminfo_marks(fp_ਬਾਹਰਲਾ, do_copy_marks ? &buflist : ਨਿਰਾ);
    }

    ਜੇ (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_ਬਾਹਰਲਾ, &buflist, eof, flags);
	ਜੇ (fp_ਬਾਹਰਲਾ != ਨਿਰਾ)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_ਸਤਰ);
    ਜੇ (vir.vir_conv.vc_type != CONV_NONE)
	convert_ਤਿਆਰੀ(&vir.vir_conv, ਨਿਰਾ, ਨਿਰਾ);
    ga_clear_ਲੜੀਆਂ(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    ਪੂਰਨ
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    ਪੂਰਨ		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    ਜੇ (no_viminfo())
	ਵਾਪਸ FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    fp = mch_fopen((ਅੱਖਰ *)fname, READBIN);

    ਜੇ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == ਨਿਰਾ ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    ਜੇ (fp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_ਬਹੁਲਕ))
    {
	fclose(fp);
	ਵਾਪਸ FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, ਨਿਰਾ, flags);

    fclose(fp);
    ਵਾਪਸ OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    ਖਾਲੀ
write_viminfo(char_u *file, ਪੂਰਨ forceit)
{
    char_u	*fname;
    FILE	*fp_ਵਿੱਚ = ਨਿਰਾ;	// input viminfo file, if any
    FILE	*fp_ਬਾਹਰਲਾ = ਨਿਰਾ;	// output viminfo file
    char_u	*tempname = ਨਿਰਾ;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#ਜੇ defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਪੂਰਨ		shortname = FALSE;	// use 8.3 file name
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਪੂਰਨ		hidden = FALSE;
#ਜੇ_ਅੰਤ

    ਜੇ (no_viminfo())
	ਵਾਪਸ;

    fname = viminfo_filename(file);	// may set to default if NULL
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ;

    fp_ਵਿੱਚ = mch_fopen((ਅੱਖਰ *)fname, READBIN);
    ਜੇ (fp_ਵਿੱਚ == ਨਿਰਾ)
    {
	ਪੂਰਨ fd;

	// if it does exist, but we can't read it, don't try writing
	ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st_new) == 0)
	    ਜਾਓ ਸਿਰਾ;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((ਅੱਖਰ *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	ਜੇ (fd < 0)
	    ਜਾਓ ਸਿਰਾ;
	fp_ਬਾਹਰਲਾ = fdopen(fd, WRITEBIN);
    }
    ਨਹੀਂਤਾਂ
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	ਜੇ (mch_fstat(fileno(fp_ਵਿੱਚ), &st_old) < 0
		|| S_ISDIR(st_old.st_ਬਹੁਲਕ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != ਜੜ੍ਹ_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_ਬਹੁਲਕ & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_ਬਹੁਲਕ & 0020)
				    : (st_old.st_ਬਹੁਲਕ & 0002))))
#ਜੇ_ਅੰਤ
		)
	{
	    ਪੂਰਨ	tt = ਸੁਨੇਹਾ_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    ਸੁਨੇਹਾ_didany = tt;
	    fclose(fp_ਵਿੱਚ);
	    ਜਾਓ ਸਿਰਾ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	// Get the file attributes of the existing viminfo file.
	hidden = mch_ishidden(fname);
#ਜੇ_ਅੰਤ

	// Make tempname, find one that does not exist yet.
	// Beware of a race condition: If someone logs out and all Vim
	// instances exit at the same time a temp file might be created between
	// stat() and open().  Use mch_open() with O_EXCL to avoid that.
	// May try twice: Once normal and once with shortname set, just in
	// case somebody puts his viminfo file in an 8.3 filesystem.
	ਲਈ (;;)
	{
	    ਪੂਰਨ		ਅਗਲਾ_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
				    shortname,
#ਨਹੀਂਤਾਂ
				    FALSE,
#ਜੇ_ਅੰਤ
				    fname,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
				    (char_u *)"-tmp",
#ਨਹੀਂਤਾਂ
				    (char_u *)".tmp",
#ਜੇ_ਅੰਤ
				    FALSE);
	    ਜੇ (tempname == ਨਿਰਾ)		// out of memory
		ਰੋਕੋ;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    ਜੇ (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    ਲਈ (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		ਜੇ (mch_stat((ਅੱਖਰ *)tempname, &st_new) == 0)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    ਜੇ (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			ਰੋਕੋ;
		    }
#ਜੇ_ਅੰਤ
		}
		ਨਹੀਂਤਾਂ
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_ਬਾਹਰਲਾ = mch_fopen((ਅੱਖਰ *)tempname, WRITEBIN);
		    (ਖਾਲੀ)umask(umask_save);
#ਨਹੀਂਤਾਂ
		    ਪੂਰਨ	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((ਅੱਖਰ *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(ਪੂਰਨ)((st_old.st_ਬਹੁਲਕ & 0777) | 0600));
		    (ਖਾਲੀ)umask(umask_save);
# ਨਹੀਂਤਾਂ
		    fd = mch_open((ਅੱਖਰ *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    ਜੇ (fd < 0)
		    {
			fp_ਬਾਹਰਲਾ = ਨਿਰਾ;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			ਜੇ (errno != EEXIST)
			    ਰੋਕੋ;
# endif
		    }
		    ਨਹੀਂਤਾਂ
			fp_ਬਾਹਰਲਾ = fdopen(fd, WRITEBIN);
#ਜੇ_ਅੰਤ // VMS
		    ਜੇ (fp_ਬਾਹਰਲਾ != ਨਿਰਾ)
			ਰੋਕੋ;
		}

		// Assume file exists, try again with another name.
		ਜੇ (ਅਗਲਾ_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_ਆਰਜ਼ੀ_files_like_str), tempname);
		    ਰੋਕੋ;
		}
		*wp = ਅਗਲਾ_char;
		--ਅਗਲਾ_char;
	    }

	    ਜੇ (tempname != ਨਿਰਾ)
		ਰੋਕੋ;
	    // continue if shortname was set
	}

#ਜੇ defined(UNIX) && defined(HAVE_FCHOWN)
	ਜੇ (tempname != ਨਿਰਾ && fp_ਬਾਹਰਲਾ != ਨਿਰਾ)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    ਜੇ (mch_stat((ਅੱਖਰ *)tempname, &tmp_st) >= 0)
	    {
		ਜੇ (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_ਬਾਹਰਲਾ), st_old.st_uid, -1);
		ਜੇ (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_ਬਾਹਰਲਾ), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (ਖਾਲੀ)mch_setperm(tempname, 0600);
	    }
	    ਨਹੀਂਤਾਂ
		// can't stat the file, set conservative permissions
		(ਖਾਲੀ)mch_setperm(tempname, 0600);
	}
#ਜੇ_ਅੰਤ
    }

    // Check if the new viminfo file can be written to.
    ਜੇ (fp_ਬਾਹਰਲਾ == ਨਿਰਾ)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_ਵਿੱਚ == ਨਿਰਾ || tempname == ਨਿਰਾ) ? fname : tempname);
	ਜੇ (fp_ਵਿੱਚ != ਨਿਰਾ)
	    fclose(fp_ਵਿੱਚ);
	ਜਾਓ ਸਿਰਾ;
    }

    ਜੇ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_ਵਿੱਚ, fp_ਬਾਹਰਲਾ, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    ਜੇ (fclose(fp_ਬਾਹਰਲਾ) == EOF)
	++viminfo_errcnt;

    ਜੇ (fp_ਵਿੱਚ != ਨਿਰਾ)
    {
	fclose(fp_ਵਿੱਚ);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	ਜੇ (viminfo_errcnt == 0)
	{
	    ਜੇ (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_ਮੰਜ਼ਿਲ_str), fname);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    ਨਹੀਂਤਾਂ ਜੇ (hidden)
		mch_hide(fname);
#ਜੇ_ਅੰਤ
	}
	ਜੇ (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

ਸਿਰਾ:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    ਖਾਲੀ
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    ਜੇ (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    ਜੇ (eap->cmdidx == CMD_rviminfo)
    {
	ਜੇ (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    ਨਹੀਂਤਾਂ
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#ਜੇ_ਅੰਤ // FEAT_VIMINFO
