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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
প্রকারনির্দেশ সংরচনা
{
    char_u	*vir_সারি;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    পূর্ণাঙ্ক		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

প্রকারনির্দেশ গণনা {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

প্রকারনির্দেশ সংরচনা {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    পূর্ণাঙ্ক		bv_len;		// length of bv_string
    পূর্ণাঙ্ক		bv_allocated;	// bv_string was allocated
} bval_T;

#যদি defined(FEAT_VIMINFO)

স্থির পূর্ণাঙ্ক  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    স্থির char_u *
find_viminfo_parameter(পূর্ণাঙ্ক type)
{
    char_u  *p;

    প্রতি (p = p_viminfo; *p; ++p)
    {
	যদি (*p == type)
	    প্রত্যাগম p + 1;
	যদি (*p == 'n')		    // 'n' is always the last one
	    ভঙ্গ;
	p = vim_strchr(p, ',');	    // skip until next ','
	যদি (p == রিক্ত)		    // hit the end without finding parameter
	    ভঙ্গ;
    }
    প্রত্যাগম রিক্ত;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    পূর্ণাঙ্ক
get_viminfo_parameter(পূর্ণাঙ্ক type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    যদি (p != রিক্ত && VIM_ISDIGIT(*p))
	প্রত্যাগম atoi((বর্ণ *)p);
    প্রত্যাগম -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    স্থির char_u *
viminfo_filename(char_u *file)
{
    যদি (file == রিক্ত || *file == NUL)
    {
	size_t  len;

	যদি (*p_viminfofile != NUL)
	    file = p_viminfofile;
	অন্যথা যদি ((file = find_viminfo_parameter('n')) == রিক্ত || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    যদি (mch_getenv((char_u *)"SYS$LOGIN") == রিক্ত)
# অন্যথা
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    যদি (STRCMP(vim_getenv((char_u *)"HOME", রিক্ত), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == রিক্ত)
#  অন্যথা
	    যদি (mch_getenv((char_u *)"HOME") == রিক্ত)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		যদি (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		অন্যথা
		    file = (char_u *)VIMINFO_FILE;
	    }
	    অন্যথা
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	প্রত্যাগম vim_strnsave(file, len);
    }

    প্রত্যাগম vim_strsave(file);
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
    স্থির শূন্য
viminfo_writestring(FILE *fd, char_u *p)
{
    পূর্ণাঙ্ক		c;
    char_u	*s;
    পূর্ণাঙ্ক		len = 0;

    প্রতি (s = p; *s != NUL; ++s)
    {
	যদি (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    যদি (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    যাবত্ ((c = *p++) != NUL)
    {
	যদি (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    যদি (c == '\n')
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
    স্থির পূর্ণাঙ্ক
barline_writestring(FILE *fd, char_u *s, পূর্ণাঙ্ক remaining_start)
{
    char_u *p;
    পূর্ণাঙ্ক	    remaining = remaining_start;
    পূর্ণাঙ্ক	    len = 2;

    // Count the number of characters produced, including quotes.
    প্রতি (p = s; *p != NUL; ++p)
    {
	যদি (*p == NL)
	    len += 2;
	অন্যথা যদি (*p == '"' || *p == '\\')
	    len += 2;
	অন্যথা
	    ++len;
    }
    যদি (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    প্রতি (p = s; *p != NUL; ++p)
    {
	যদি (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	অন্যথা যদি (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	অন্যথা
	    putc(*p, fd);
	--remaining;

	যদি (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    প্রত্যাগম remaining - 2;
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
    স্থির char_u *
viminfo_readstring(
    vir_T	*virp,
    পূর্ণাঙ্ক		off,		    // offset for virp->vir_line
    পূর্ণাঙ্ক		convert)	    // convert the string
{
    char_u	*retval = রিক্ত;
    char_u	*s, *d;
    long	len;

    যদি (virp->vir_সারি[off] == Ctrl_V && vim_isdigit(virp->vir_সারি[off + 1]))
    {
	len = atol((বর্ণ *)virp->vir_সারি + off + 1);
	যদি (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	যদি (retval == রিক্ত)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (শূন্য)vim_fgets(virp->vir_সারি, 10, virp->vir_fd);
	    প্রত্যাগম রিক্ত;
	}
	(শূন্য)vim_fgets(retval, (পূর্ণাঙ্ক)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    অন্যথা
    {
	retval = vim_strsave(virp->vir_সারি + off);
	যদি (retval == রিক্ত)
	    প্রত্যাগম রিক্ত;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    যাবত্ (*s != NUL && *s != '\n')
    {
	যদি (s[0] == Ctrl_V && s[1] != NUL)
	{
	    যদি (s[1] == 'n')
		*d++ = '\n';
	    অন্যথা
		*d++ = Ctrl_V;
	    s += 2;
	}
	অন্যথা
	    *d++ = *s++;
    }
    *d = NUL;

    যদি (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, রিক্ত);
	যদি (d != রিক্ত)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    প্রত্যাগম retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    স্থির পূর্ণাঙ্ক
viminfo_readline(vir_T *virp)
{
    প্রত্যাগম vim_fgets(virp->vir_সারি, LSIZE, virp->vir_fd);
}

    স্থির পূর্ণাঙ্ক
read_viminfo_bufferlist(
    vir_T	*virp,
    পূর্ণাঙ্ক		writing)
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
    যদি (xline != রিক্ত && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != রিক্ত)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	যদি (tab != রিক্ত)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((বর্ণ *)tab);
	    tab = vim_strrchr(xline, '\t');
	    যদি (tab != রিক্ত)
	    {
		*tab++ = '\0';
		lnum = atol((বর্ণ *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	যদি (buf != রিক্ত)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    প্রত্যাগম viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    স্থির পূর্ণাঙ্ক
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    পূর্ণাঙ্ক	    part_len;
    পূর্ণাঙ্ক	    retval = FALSE;

    name = home_replace_save(রিক্ত, name);
    যদি (name == রিক্ত)
	প্রত্যাগম FALSE;
    প্রতি (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, পরিমাণ(part), ", ");
	যদি (part[0] == 'r')
	{
	    যদি (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		ভঙ্গ;
	    }
	}
    }
    vim_free(name);
    প্রত্যাগম retval;
}

    স্থির শূন্য
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*সারি;
    পূর্ণাঙ্ক		max_buffers;

    যদি (find_viminfo_parameter('%') == রিক্ত)
	প্রত্যাগম;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define সারি_BUF_LEN (MAXPATHL + 40)
    সারি = alloc(সারি_BUF_LEN);
    যদি (সারি == রিক্ত)
	প্রত্যাগম;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	যদি (buf->b_fname == রিক্ত
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    অনুবর্ত;

	যদি (max_buffers-- == 0)
	    ভঙ্গ;
	putc('%', fp);
	linelen = home_replace(রিক্ত, buf->b_ffname, সারি, MAXPATHL, TRUE);
	vim_snprintf((বর্ণ *)সারি + linelen, সারি_BUF_LEN - linelen, "\t%ld\t%d",
			(long)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, সারি);
    }
    vim_free(সারি);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
স্থির histentry_T *viminfo_history[HIST_গুনতি] =
					       {রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত};
স্থির পূর্ণাঙ্ক	viminfo_hisidx[HIST_গুনতি] = {0, 0, 0, 0, 0};
স্থির পূর্ণাঙ্ক	viminfo_hislen[HIST_গুনতি] = {0, 0, 0, 0, 0};
স্থির পূর্ণাঙ্ক	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    স্থির পূর্ণাঙ্ক
hist_type2char(
    পূর্ণাঙ্ক	    type,
    পূর্ণাঙ্ক	    use_question)	    // use '?' instead of '/'
{
    যদি (type == HIST_CMD)
	প্রত্যাগম ':';
    যদি (type == HIST_অনুসন্ধান)
    {
	যদি (use_question)
	    প্রত্যাগম '?';
	অন্যথা
	    প্রত্যাগম '/';
    }
    যদি (type == HIST_রাশিমালা)
	প্রত্যাগম '=';
    প্রত্যাগম '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    স্থির শূন্য
prepare_viminfo_history(পূর্ণাঙ্ক asklen, পূর্ণাঙ্ক writing)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    অঙ্ক;
    পূর্ণাঙ্ক	    type;
    পূর্ণাঙ্ক	    len;
    পূর্ণাঙ্ক	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    যদি (asklen > hislen)
	asklen = hislen;

    প্রতি (type = 0; type < HIST_গুনতি; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	প্রতি (i = 0, অঙ্ক = 0; i < hislen; i++)
	    যদি (histentry[i].hisstr == রিক্ত || histentry[i].viminfo)
		অঙ্ক++;
	len = asklen;
	যদি (অঙ্ক > len)
	    len = অঙ্ক;
	যদি (len <= 0)
	    viminfo_history[type] = রিক্ত;
	অন্যথা
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	যদি (viminfo_history[type] == রিক্ত)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    স্থির পূর্ণাঙ্ক
read_viminfo_history(vir_T *virp, পূর্ণাঙ্ক writing)
{
    পূর্ণাঙ্ক		type;
    long_u	len;
    char_u	*val = রিক্ত;
    char_u	*p;

    type = hist_char2type(virp->vir_সারি[0]);
    যদি (viminfo_hisidx[type] >= viminfo_hislen[type])
	গচ্ছ done;

    val = viminfo_readstring(virp, 1, TRUE);
    যদি (val == রিক্ত || *val == NUL)
	গচ্ছ done;

    পূর্ণাঙ্ক sep = (*val == ' ' ? NUL : *val);

    যদি (in_history(type, val + (type == HIST_অনুসন্ধান), viminfo_add_at_front,
								sep, writing))
	গচ্ছ done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    যদি (type == HIST_অনুসন্ধান)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	যদি (p == রিক্ত)
	    গচ্ছ done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    অন্যথা
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	যদি (p == রিক্ত)
	    গচ্ছ done;

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
    প্রত্যাগম viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    স্থির শূন্য
handle_viminfo_history(
	garray_T    *মানসমূহ,
	পূর্ণাঙ্ক	    writing)
{
    পূর্ণাঙ্ক		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)মানসমূহ->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    যদি (মানসমূহ->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	প্রত্যাগম;

    type = vp[0].bv_nr;
    যদি (type >= HIST_গুনতি)
	প্রত্যাগম;

    যদি (viminfo_hisidx[type] >= viminfo_hislen[type])
	প্রত্যাগম;

    val = vp[3].bv_string;
    যদি (val == রিক্ত || *val == NUL)
	প্রত্যাগম;

    পূর্ণাঙ্ক sep = type == HIST_অনুসন্ধান && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    পূর্ণাঙ্ক idx;
    পূর্ণাঙ্ক overwrite = FALSE;

    যদি (in_history(type, val, viminfo_add_at_front, sep, writing))
	প্রত্যাগম;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    প্রতি (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	যদি (STRCMP(val, p) == 0
		&& (type != HIST_অনুসন্ধান || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    ভঙ্গ;
	}
    }

    যদি (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    অন্যথা
	len = 0; // for picky compilers
    যদি (p != রিক্ত)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	যদি (!overwrite)
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
    স্থির শূন্য
concat_history(পূর্ণাঙ্ক type)
{
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    পূর্ণাঙ্ক		*hisidx = get_hisidx(type);
    পূর্ণাঙ্ক		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    যদি (idx >= hislen)
	idx -= hislen;
    অন্যথা যদি (idx < 0)
	idx = hislen - 1;
    যদি (viminfo_add_at_front)
	*hisidx = idx;
    অন্যথা
    {
	যদি (*hisidx == -1)
	    *hisidx = hislen - 1;
	কুরু
	{
	    যদি (histentry[idx].hisstr != রিক্ত || histentry[idx].viminfo)
		ভঙ্গ;
	    যদি (++idx == hislen)
		idx = 0;
	} যাবত্ (idx != *hisidx);
	যদি (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    প্রতি (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	যদি (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    প্রতি (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    স্থির পূর্ণাঙ্ক
sort_hist(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    যদি (p1->time_set < p2->time_set) প্রত্যাগম -1;
    যদি (p1->time_set > p2->time_set) প্রত্যাগম 1;
    প্রত্যাগম 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    স্থির শূন্য
merge_history(পূর্ণাঙ্ক type)
{
    পূর্ণাঙ্ক		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    পূর্ণাঙ্ক		*hisidx = get_hisidx(type);
    পূর্ণাঙ্ক		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    যদি (tot_hist == রিক্ত || new_hist == রিক্ত)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	প্রত্যাগম;
    }
    প্রতি (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    প্রতি (i = 0; i < hislen; i++)
	যদি (histentry[i].hisstr != রিক্ত)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((শূন্য *)tot_hist, (size_t)len, পরিমাণ(histentry_T *), sort_hist);

    // Keep the newest ones.
    প্রতি (i = 0; i < hislen; i++)
    {
	যদি (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = রিক্ত;
	    tot_hist[i]->hisstrlen = 0;
	    যদি (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	অন্যথা
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    প্রতি (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    প্রতি (i = 0; i < hislen; i++)
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
    স্থির শূন্য
finish_viminfo_history(vir_T *virp)
{
    পূর্ণাঙ্ক	type;
    পূর্ণাঙ্ক merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    প্রতি (type = 0; type < HIST_গুনতি; ++type)
    {
	যদি (get_histentry(type) == রিক্ত)
	    অনুবর্ত;

	যদি (merge)
	    merge_history(type);
	অন্যথা
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
    স্থির শূন্য
write_viminfo_history(FILE *fp, পূর্ণাঙ্ক merge)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    type;
    পূর্ণাঙ্ক	    অঙ্ক_saved;
    পূর্ণাঙ্ক     round;
    পূর্ণাঙ্ক	    hislen;

    init_history();
    hislen = get_hislen();
    যদি (hislen == 0)
	প্রত্যাগম;
    প্রতি (type = 0; type < HIST_গুনতি; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	পূর্ণাঙ্ক	    *hisidx = get_hisidx(type);

	অঙ্ক_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	যদি (অঙ্ক_saved == 0)
	    অনুবর্ত;
	যদি (অঙ্ক_saved < 0)  // Use default
	    অঙ্ক_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_অনুসন্ধান ? _("Search String") :
			    type == HIST_রাশিমালা ? _("Expression") :
			    type == HIST_প্রবেশ ? _("Input Line") :
					_("Debug Line"));
	যদি (অঙ্ক_saved > hislen)
	    অঙ্ক_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	প্রতি (round = 1; round <= 2; ++round)
	{
	    যদি (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    অন্যথা যদি (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    অন্যথা
		// empty list
		i = -1;
	    যদি (i >= 0)
		যাবত্ (অঙ্ক_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    পূর্ণাঙ্ক	    c = NUL;

		    যদি (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    অন্যথা
		    {
			যদি (viminfo_history[type] == রিক্ত)
			{
			    p = রিক্ত;
			    plen = 0;
			    timestamp = 0;
			}
			অন্যথা
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    যদি (p != রিক্ত && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--অঙ্ক_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			যদি (type == HIST_অনুসন্ধান)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    বর্ণ    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    যদি (c == NUL)
				cbuf[0] = NUL;
			    অন্যথা
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    যদি (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			যদি (--i < 0)
			    i = hislen - 1;
			যদি (i == *hisidx)
			    ভঙ্গ;
		    }
		    অন্যথা
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	প্রতি (i = 0; i < viminfo_hisidx[type]; ++i)
	    যদি (viminfo_history[type] != রিক্ত)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    স্থির শূন্য
write_viminfo_barlines(vir_T *virp, FILE *fp_out)
{
    পূর্ণাঙ্ক		i;
    garray_T	*gap = &virp->vir_barlines;
    পূর্ণাঙ্ক		seen_useful = FALSE;
    বর্ণ	*সারি;

    যদি (gap->ga_len <= 0)
	প্রত্যাগম;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    প্রতি (i = 0; i < gap->ga_len; ++i)
    {
	সারি = ((বর্ণ **)(gap->ga_data))[i];
	যদি (seen_useful || সারি[1] != '<')
	{
	    fputs(সারি, fp_out);
	    seen_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    স্থির পূর্ণাঙ্ক
barline_parse(vir_T *virp, char_u *text, garray_T *মানসমূহ)
{
    char_u  *p = text;
    char_u  *nextp = রিক্ত;
    char_u  *buf = রিক্ত;
    bval_T  *মান;
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    allocated = FALSE;
    পূর্ণাঙ্ক	    eof;
    char_u  *sconv;
    পূর্ণাঙ্ক	    converted;

    যাবত্ (*p == ',')
    {
	++p;
	যদি (ga_grow(মানসমূহ, 1) == FAIL)
	    ভঙ্গ;
	মান = (bval_T *)(মানসমূহ->ga_data) + মানসমূহ->ga_len;

	যদি (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    যদি (!allocated)
	    {
		প্রতি (i = 0; i < মানসমূহ->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(মানসমূহ->ga_data) + i;

		    যদি (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    যদি (vim_isdigit(p[1]))
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
		যদি (buf == রিক্ত)
		    প্রত্যাগম TRUE;
		p = buf;
		প্রতি (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    যদি (eof || virp->vir_সারি[0] != '|'
						  || virp->vir_সারি[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			প্রত্যাগম eof || virp->vir_সারি[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_সারি);
		    যাবত্ (n > 2 && (virp->vir_সারি[n - 1] == NL
					     || virp->vir_সারি[n - 1] == CAR))
			--n;
		    n -= 2;
		    যদি (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_সারি + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_সারি + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    অন্যথা
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		যদি (eof || virp->vir_সারি[0] != '|'
					      || virp->vir_সারি[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    প্রত্যাগম eof || virp->vir_সারি[0] == '|';
		p = virp->vir_সারি + 2;
	    }
	}

	যদি (SAFE_isdigit(*p))
	{
	    মান->bv_type = BVAL_NR;
	    মান->bv_nr = getdigits(&p);
	    ++মানসমূহ->ga_len;
	}
	অন্যথা যদি (*p == '"')
	{
	    পূর্ণাঙ্ক	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    যাবত্ (*p != '"')
	    {
		যদি (*p == NL || *p == NUL)
		    প্রত্যাগম TRUE;  // syntax error, drop the value
		যদি (*p == '\\')
		{
		    ++p;
		    যদি (*p == 'n')
			s[len++] = '\n';
		    অন্যথা
			s[len++] = *p;
		    ++p;
		}
		অন্যথা
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    মান->bv_tofree = রিক্ত;
	    যদি (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, রিক্ত);
		যদি (sconv != রিক্ত)
		{
		    যদি (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			মান->bv_tofree = buf;
		    s = sconv;
		    len = (পূর্ণাঙ্ক)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    যদি (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    মান->bv_string = s;
	    মান->bv_type = BVAL_STRING;
	    মান->bv_len = len;
	    মান->bv_allocated = allocated || converted;
	    ++মানসমূহ->ga_len;
	    যদি (nextp != রিক্ত)
	    {
		// values following a long string
		p = nextp;
		nextp = রিক্ত;
	    }
	}
	অন্যথা যদি (*p == ',')
	{
	    মান->bv_type = BVAL_EMPTY;
	    ++মানসমূহ->ga_len;
	}
	অন্যথা
	    ভঙ্গ;
    }
    প্রত্যাগম TRUE;
}

    স্থির শূন্য
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    স্থির পূর্ণাঙ্ক
no_viminfo(শূন্য)
{
    // "vim -i NONE" does not read or write a viminfo file
    প্রত্যাগম STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    স্থির পূর্ণাঙ্ক
viminfo_error(বর্ণ *errnum, বর্ণ *message, char_u *সারি)
{
    vim_snprintf((বর্ণ *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, সারি, IOSIZE - STRLEN(IObuff) - 1);
    যদি (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((বর্ণ *)IObuff);
    যদি (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    স্থির পূর্ণাঙ্ক
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    পূর্ণাঙ্ক		i;

    যদি (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_সারি, '=');
	যদি (p != রিক্ত)
	{
	    // remove trailing newline
	    ++p;
	    প্রতি (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    প্রত্যাগম viminfo_readline(virp);
}

#যদি defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    স্থির পূর্ণাঙ্ক
read_viminfo_varlist(vir_T *virp, পূর্ণাঙ্ক writing)
{
    char_u	*tab;
    পূর্ণাঙ্ক		type = VAR_সংখ্যা;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    যদি (!writing && (find_viminfo_parameter('!') != রিক্ত))
    {
	tab = vim_strchr(virp->vir_সারি + 1, '\t');
	যদি (tab != রিক্ত)
	{
	    *tab++ = '\0';	// isolate the variable name
	    বিকল্প (*tab)
	    {
		পক্ষ 'S': type = VAR_STRING; ভঙ্গ;
		পক্ষ 'F': type = VAR_FLOAT; ভঙ্গ;
		পক্ষ 'D': type = VAR_DICT; ভঙ্গ;
		পক্ষ 'L': type = VAR_LIST; ভঙ্গ;
		পক্ষ 'B': type = VAR_BLOB; ভঙ্গ;
		পক্ষ 'X': type = VAR_SPECIAL; ভঙ্গ;
		পক্ষ 'T': type = VAR_TUPLE; ভঙ্গ;
	    }

	    tab = vim_strchr(tab, '\t');
	    যদি (tab != রিক্ত)
	    {
		tv.v_type = type;
		যদি (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (পূর্ণাঙ্ক)(tab - virp->vir_সারি + 1), TRUE);
		অন্যথা যদি (type == VAR_FLOAT)
		    (শূন্য)string2float(tab + 1, &tv.vval.v_float, FALSE);
		অন্যথা
		{
		    tv.vval.v_সংখ্যা = atol((বর্ণ *)tab + 1);
		    যদি (type == VAR_SPECIAL && (tv.vval.v_সংখ্যা == VVAL_FALSE
					     || tv.vval.v_সংখ্যা == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		যদি (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_রাশিমালা(tv.vval.v_string, রিক্ত);

		    যদি (etv == রিক্ত)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    অন্যথা
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		অন্যথা যদি (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    যদি (blob == রিক্ত)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    অন্যথা
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_entry);
		set_var(virp->vir_সারি + 1, &tv, FALSE);
		restore_funccal();

		যদি (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		অন্যথা যদি (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    প্রত্যাগম viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    স্থির শূন্য
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    পূর্ণাঙ্ক		todo;
    বর্ণ	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    যদি (find_viminfo_parameter('!') == রিক্ত)
	প্রত্যাগম;

    fputs(_("\n# global variables:\n"), fp);

    todo = (পূর্ণাঙ্ক)gvht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(gvht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    যদি (var_flavour(this_var->di_চাবি) == VAR_FLAVOUR_VIMINFO)
	    {
		বিকল্প (this_var->di_tv.v_type)
		{
		    পক্ষ VAR_STRING:  s = "STR"; ভঙ্গ;
		    পক্ষ VAR_সংখ্যা:  s = "NUM"; ভঙ্গ;
		    পক্ষ VAR_FLOAT:   s = "FLO"; ভঙ্গ;
		    পক্ষ VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      পূর্ণাঙ্ক	copyID = get_copyID();

			      s = "DIC";
			      যদি (di != রিক্ত && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, রিক্ত, রিক্ত)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  অনুবর্ত;
			      ভঙ্গ;
			  }
		    পক্ষ VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      পূর্ণাঙ্ক	copyID = get_copyID();

			      s = "LIS";
			      যদি (l != রিক্ত && !set_ref_in_list_বস্তুসমূহ(
						       l, copyID, রিক্ত, রিক্ত)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  অনুবর্ত;
			      ভঙ্গ;
			  }
		    পক্ষ VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      পূর্ণাঙ্ক	copyID = get_copyID();

			      s = "TUP";
			      যদি (tuple != রিক্ত && !set_ref_in_tuple_বস্তুসমূহ(
					       tuple, copyID, রিক্ত, রিক্ত)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  অনুবর্ত;
			      ভঙ্গ;
			  }
		    পক্ষ VAR_BLOB:    s = "BLO"; ভঙ্গ;
		    পক্ষ VAR_BOOL:    s = "XPL"; ভঙ্গ;  // backwards compat.
		    পক্ষ VAR_SPECIAL: s = "XPL"; ভঙ্গ;

		    পক্ষ VAR_UNKNOWN:
		    পক্ষ VAR_ANY:
		    পক্ষ VAR_VOID:
		    পক্ষ VAR_FUNC:
		    পক্ষ VAR_PARTIAL:
		    পক্ষ VAR_JOB:
		    পক্ষ VAR_CHANNEL:
		    পক্ষ VAR_INSTR:
		    পক্ষ VAR_CLASS:
		    পক্ষ VAR_OBJECT:
		    পক্ষ VAR_TYPEALIAS:
				      অনুবর্ত;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_চাবি, s);
		যদি (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((বর্ণ *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_সংখ্যা);
		    p = numbuf;
		    tofree = রিক্ত;
		}
		অন্যথা
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		যদি (p != রিক্ত)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    স্থির পূর্ণাঙ্ক
read_viminfo_sub_string(vir_T *virp, পূর্ণাঙ্ক force)
{
    যদি (force || get_old_sub() == রিক্ত)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    প্রত্যাগম viminfo_readline(virp);
}

    স্থির শূন্য
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    যদি (get_viminfo_parameter('/') == 0 || old_sub == রিক্ত)
	প্রত্যাগম;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    স্থির পূর্ণাঙ্ক
read_viminfo_অনুসন্ধান_pattern(vir_T *virp, পূর্ণাঙ্ক force, পূর্ণাঙ্ক writing)
{
    char_u	*lp;
    পূর্ণাঙ্ক		idx = -1;
    পূর্ণাঙ্ক		magic = FALSE;
    পূর্ণাঙ্ক		no_scs = FALSE;
    পূর্ণাঙ্ক		off_সারি = FALSE;
    পূর্ণাঙ্ক		off_end = 0;
    long	off = 0;
    পূর্ণাঙ্ক		setlast = FALSE;
#ifdef FEAT_অনুসন্ধান_EXTRA
    স্থির পূর্ণাঙ্ক	hlsearch_on = FALSE;
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
    lp = virp->vir_সারি;
    যদি (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	যদি (lp[1] == 'M')		// magic on
	    magic = TRUE;
	যদি (lp[2] == 's')
	    no_scs = TRUE;
	যদি (lp[3] == 'L')
	    off_সারি = TRUE;
	যদি (lp[4] == 'E')
	    off_end = অনুসন্ধান_END;
	lp += 5;
	off = getdigits(&lp);
    }
    যদি (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    যদি (lp[0] == '/')
	idx = RE_অনুসন্ধান;
    অন্যথা যদি (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_অনুসন্ধান_EXTRA
    অন্যথা যদি (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    অন্যথা যদি (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    যদি (idx >= 0)
    {
	spat = get_spat(idx);
	যদি (force || spat->pat == রিক্ত)
	{
	    val = viminfo_readstring(virp, (পূর্ণাঙ্ক)(lp - virp->vir_সারি + 1),
									TRUE);
	    যদি (val != রিক্ত)
	    {
		যদি (!writing)
		{
		    set_last_অনুসন্ধান_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.সারি = off_সারি;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_অনুসন্ধান_EXTRA
		    যদি (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    প্রত্যাগম viminfo_readline(virp);
}

    স্থির শূন্য
wvsp_one(
    FILE	*fp,	// file to write to
    পূর্ণাঙ্ক		idx,	// spats[] index
    বর্ণ	*s,	// search pat
    পূর্ণাঙ্ক		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    যদি (spat->pat == রিক্ত)
	প্রত্যাগম;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.সারি ? 'L' : 'l',	// line offset
	    spat->off.end  ? 'E' : 'e',	// offset from end
	    spat->off.off,			// offset
	    get_spat_last_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    স্থির শূন্য
write_viminfo_অনুসন্ধান_pattern(FILE *fp)
{
    যদি (get_viminfo_parameter('/') == 0)
	প্রত্যাগম;

#ifdef FEAT_অনুসন্ধান_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != রিক্ত) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_অনুসন্ধান, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

স্থির yankreg_T *y_read_regs = রিক্ত;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    স্থির শূন্য
prepare_viminfo_registers(শূন্য)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, অঙ্ক_REGISTERS);
}

    স্থির শূন্য
finish_viminfo_registers(শূন্য)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		j;

    যদি (y_read_regs == রিক্ত)
	প্রত্যাগম;

    প্রতি (i = 0; i < অঙ্ক_REGISTERS; ++i)
	যদি (y_read_regs[i].y_array != রিক্ত)
	{
	    প্রতি (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    স্থির পূর্ণাঙ্ক
read_viminfo_register(vir_T *virp, পূর্ণাঙ্ক force)
{
    পূর্ণাঙ্ক		eof;
    পূর্ণাঙ্ক		do_it = TRUE;
    পূর্ণাঙ্ক		size;
    পূর্ণাঙ্ক		limit;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		set_prev = FALSE;
    char_u	*str;
    string_T	*array = রিক্ত;
    পূর্ণাঙ্ক		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_সারি + 1;

    // If the line starts with "" this is the y_previous register.
    যদি (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    যদি (!ASCII_ISALNUM(*str) && *str != '-')
    {
	যদি (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_সারি))
	    প্রত্যাগম TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    যদি (!force && y_current_p->y_array != রিক্ত)
	do_it = FALSE;

    যদি (*str == '@')
    {
	// "x@: register x used for @@
	যদি (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    যদি (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	যদি (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	যদি (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	অন্যথা যদি (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	অন্যথা
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    যাবত্ (!(eof = viminfo_readline(virp))
		    && (virp->vir_সারি[0] == TAB || virp->vir_সারি[0] == '<'))
    {
	যদি (do_it)
	{
	    যদি (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * পরিমাণ(string_T));

		যদি (new_array == রিক্ত)
		{
		    do_it = FALSE;
		    ভঙ্গ;
		}
		প্রতি (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    যদি (str != রিক্ত)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    অন্যথা
		// error, don't store the result
		do_it = FALSE;
	}
    }

    যদি (do_it)
    {
	// free y_array[]
	প্রতি (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	যদি (size == 0)
	{
	    y_current_p->y_array = রিক্ত;
	}
	অন্যথা
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    প্রতি (i = 0; i < size; i++)
	    {
		যদি (y_current_p->y_array == রিক্ত)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		অন্যথা
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    অন্যথা
    {
	// Free array[] if it was filled.
	প্রতি (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    প্রত্যাগম eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    স্থির শূন্য
handle_viminfo_register(garray_T *মানসমূহ, পূর্ণাঙ্ক force)
{
    bval_T	*vp = (bval_T *)মানসমূহ->ga_data;
    পূর্ণাঙ্ক		flags;
    পূর্ণাঙ্ক		name;
    পূর্ণাঙ্ক		type;
    পূর্ণাঙ্ক		linecount;
    পূর্ণাঙ্ক		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    পূর্ণাঙ্ক		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    যদি (মানসমূহ->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	প্রত্যাগম;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    যদি (name < 0 || name >= অঙ্ক_REGISTERS)
	প্রত্যাগম;
    type = vp[2].bv_nr;
    যদি (type != MCHAR && type != MLINE && type != MBLOCK)
	প্রত্যাগম;
    linecount = vp[3].bv_nr;
    যদি (মানসমূহ->ga_len < 6 + linecount)
	প্রত্যাগম;
    width = vp[4].bv_nr;
    যদি (width < 0)
	প্রত্যাগম;

    যদি (y_read_regs != রিক্ত)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    অন্যথা
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    যদি (y_ptr->y_array != রিক্ত && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	প্রত্যাগম;

    যদি (y_ptr->y_array != রিক্ত)
	প্রতি (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    যদি (y_read_regs == রিক্ত)
    {
	যদি (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	যদি ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    যদি (linecount == 0)
    {
	y_ptr->y_array = রিক্ত;
	প্রত্যাগম;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    যদি (y_ptr->y_array == রিক্ত)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	প্রত্যাগম;
    }
    প্রতি (i = 0; i < linecount; i++)
    {
	যদি (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = রিক্ত;
	}
	অন্যথা যদি (vp[i + 6].bv_type != BVAL_STRING)
	{
	    মুক্ত(y_ptr->y_array);
	    y_ptr->y_array = রিক্ত;
	}
	অন্যথা
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    স্থির শূন্য
write_viminfo_registers(FILE *fp)
{
    পূর্ণাঙ্ক		i, j;
    char_u	*type;
    char_u	c;
    পূর্ণাঙ্ক		অঙ্ক_সারিগুলি;
    পূর্ণাঙ্ক		max_অঙ্ক_সারিগুলি;
    পূর্ণাঙ্ক		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_অঙ্ক_সারিগুলি = get_viminfo_parameter('<');
    যদি (max_অঙ্ক_সারিগুলি < 0)
	max_অঙ্ক_সারিগুলি = get_viminfo_parameter('"');
    যদি (max_অঙ্ক_সারিগুলি == 0)
	প্রত্যাগম;
    max_kbyte = get_viminfo_parameter('s');
    যদি (max_kbyte == 0)
	প্রত্যাগম;

    প্রতি (i = 0; i < অঙ্ক_REGISTERS; i++)
    {
#যদি defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	যদি (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    অনুবর্ত;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	যদি (i == TILDE_REGISTER)
	    অনুবর্ত;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	যদি (y_read_regs != রিক্ত
		&& y_read_regs[i].y_array != রিক্ত
		&& (y_regs_p[i].y_array == রিক্ত ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	অন্যথা যদি (y_regs_p[i].y_array == রিক্ত)
	    অনুবর্ত;
	অন্যথা
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	অঙ্ক_সারিগুলি = y_ptr->y_size;
	যদি (অঙ্ক_সারিগুলি == 0
		|| (অঙ্ক_সারিগুলি == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    অনুবর্ত;

	যদি (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    প্রতি (j = 0; j < অঙ্ক_সারিগুলি; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    যদি (len > (long)max_kbyte * 1024L)
		অনুবর্ত;
	}

	বিকল্প (y_ptr->y_type)
	{
	    পক্ষ MLINE:
		type = (char_u *)"LINE";
		ভঙ্গ;
	    পক্ষ MCHAR:
		type = (char_u *)"CHAR";
		ভঙ্গ;
	    পক্ষ MBLOCK:
		type = (char_u *)"BLOCK";
		ভঙ্গ;
	    স্বভাব:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		ভঙ্গ;
	}
	যদি (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	যদি (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (পূর্ণাঙ্ক)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	যদি (max_অঙ্ক_সারিগুলি > 0 && অঙ্ক_সারিগুলি > max_অঙ্ক_সারিগুলি)
	    অঙ্ক_সারিগুলি = max_অঙ্ক_সারিগুলি;
	প্রতি (j = 0; j < অঙ্ক_সারিগুলি; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    পূর্ণাঙ্ক	    flags = 0;
	    পূর্ণাঙ্ক	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    যদি (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    যদি (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, অঙ্ক_সারিগুলি, (পূর্ণাঙ্ক)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    প্রতি (j = 0; j < অঙ্ক_সারিগুলি; j++)
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

স্থির xfmark_T *vi_namedfm = রিক্ত;
স্থির xfmark_T *vi_jumplist = রিক্ত;
স্থির পূর্ণাঙ্ক vi_jumplist_len = 0;

    স্থির শূন্য
write_one_mark(FILE *fp_out, পূর্ণাঙ্ক c, pos_T *pos)
{
    যদি (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (পূর্ণাঙ্ক)pos->col);
}

    স্থির শূন্য
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    পূর্ণাঙ্ক		i;
    pos_T	pos;

    home_replace(রিক্ত, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_ঢোকাও);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    প্রতি (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	যদি (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    প্রতি (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    স্থির পূর্ণাঙ্ক
skip_for_viminfo(buf_T *buf)
{
    প্রত্যাগম bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    স্থির শূন্য
write_viminfo_marks(FILE *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    পূর্ণাঙ্ক		is_mark_set;
    পূর্ণাঙ্ক		i;
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
	যদি (buf->b_marks_read)
	{
	    যদি (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    অন্যথা
	    {
		is_mark_set = FALSE;
		প্রতি (i = 0; i < NMARKS; i++)
		    যদি (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			ভঙ্গ;
		    }
	    }
	    যদি (is_mark_set && buf->b_ffname != রিক্ত
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		যদি (buflist == রিক্ত)
		    write_buffer_marks(buf, fp_out);
		অন্যথা যদি (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    স্থির শূন্য
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    পূর্ণাঙ্ক		c1,
    পূর্ণাঙ্ক		c2)
{
    char_u	*name;

    যদি (fm->fmark.mark.lnum == 0)	// not set
	প্রত্যাগম;

    যদি (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    অন্যথা
	name = fm->fname;		// use name from .viminfo
    যদি (name != রিক্ত && *name != NUL)
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

    যদি (fm->fmark.fnum != 0)
	vim_free(name);
}

    স্থির শূন্য
write_viminfo_filemarks(FILE *fp)
{
    পূর্ণাঙ্ক		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    পূর্ণাঙ্ক		vi_idx;
    পূর্ণাঙ্ক		idx;

    যদি (get_viminfo_parameter('f') == 0)
	প্রত্যাগম;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    প্রতি (i = 0; i < NMARKS; i++)
    {
	যদি (vi_namedfm != রিক্ত
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	অন্যথা
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    যদি (curbuf->b_ffname != রিক্ত && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	প্রতি (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    যদি (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == রিক্ত
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != রিক্ত
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		ভঙ্গ;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	প্রতি ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = রিক্ত;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    প্রতি (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != রিক্ত ? &vi_namedfm[vi_idx] : রিক্ত;

	যদি (vi_fm != রিক্ত
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	অন্যথা
	{
	    fm = &namedfm_p[idx++];
	    যদি (vi_fm != রিক্ত
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != রিক্ত
			  && fm->fname != রিক্ত
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
    প্রতি (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : রিক্ত;
	vi_fm = (vi_jumplist != রিক্ত && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : রিক্ত;
	যদি (fm == রিক্ত && vi_fm == রিক্ত)
	    ভঙ্গ;
	যদি (fm == রিক্ত || (vi_fm != রিক্ত && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	অন্যথা
	    --idx;
	যদি (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != রিক্ত
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    পূর্ণাঙ্ক
buf_compare(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    যদি (buf1->b_last_used == buf2->b_last_used)
	প্রত্যাগম 0;
    প্রত্যাগম buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    স্থির শূন্য
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_out,
    garray_T	*buflist,
    পূর্ণাঙ্ক		eof,
    পূর্ণাঙ্ক		flags)
{
    char_u	*সারি = virp->vir_সারি;
    buf_T	*buf;
    পূর্ণাঙ্ক		অঙ্ক_marked_files;
    পূর্ণাঙ্ক		load_marks;
    পূর্ণাঙ্ক		copy_marks_out;
    char_u	*str;
    পূর্ণাঙ্ক		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = রিক্ত;
#endif
    পূর্ণাঙ্ক		গুনতি = 0;
    পূর্ণাঙ্ক		buflist_used = 0;
    buf_T	*buflist_buf = রিক্ত;

    যদি ((name_buf = alloc(LSIZE)) == রিক্ত)
	প্রত্যাগম;
    *name_buf = NUL;

    যদি (fp_out != রিক্ত && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						পরিমাণ(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    যদি (fp_out == রিক্ত && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	যদি (list != রিক্ত)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    অঙ্ক_marked_files = get_viminfo_parameter('\'');
    যাবত্ (!eof && (গুনতি < অঙ্ক_marked_files || fp_out == রিক্ত))
    {
	যদি (সারি[0] != '>')
	{
	    যদি (সারি[0] != '\n' && সারি[0] != '\r' && সারি[0] != '#')
	    {
		যদি (viminfo_error("E576: ", _(e_nonr_missing_gt), সারি))
		    ভঙ্গ;	// too many errors, return now
	    }
	    eof = vim_fgets(সারি, LSIZE, virp->vir_fd);
	    অনুবর্ত;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(সারি + 1);
	str = viminfo_readstring(virp, (পূর্ণাঙ্ক)(str - virp->vir_সারি), FALSE);
	যদি (str == রিক্ত)
	    অনুবর্ত;
	p = str + STRLEN(str);
	যাবত্ (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	যদি (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	যদি (list != রিক্ত)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	যদি (fp_out == রিক্ত)
	{
	    যদি ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != রিক্ত)
	    {
		যদি (*name_buf == NUL)	    // only need to do this once
		    home_replace(রিক্ত, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		যদি (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	অন্যথা // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		যদি (buf->b_ffname != রিক্ত)
		{
		    home_replace(রিক্ত, buf->b_ffname, name_buf, LSIZE, TRUE);
		    যদি (fnamecmp(str, name_buf) == 0)
			ভঙ্গ;
		}

	    // Copy marks if the buffer has not been loaded.
	    যদি (buf == রিক্ত || !buf->b_marks_read)
	    {
		পূর্ণাঙ্ক	did_read_সারি = FALSE;

		যদি (buflist_buf != রিক্ত)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    যদি (!viminfo_readline(virp) && সারি[0] == TAB)
		    {
			did_read_সারি = TRUE;
			যদি (সারি[1] == '*')
			{
			    long	ltime;

			    sscanf((বর্ণ *)সারি + 2, "%ld ", &ltime);
			    যাবত্ ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				যদি (++গুনতি >= অঙ্ক_marked_files)
				    ভঙ্গ;
				যদি (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = রিক্ত;
				    ভঙ্গ;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			অন্যথা
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    যাবত্ (গুনতি < অঙ্ক_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++গুনতি;
			    }
			    buflist_buf = রিক্ত;
			}

			যদি (গুনতি >= অঙ্ক_marked_files)
			{
			    vim_free(str);
			    ভঙ্গ;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		যদি (did_read_সারি)
		    fputs((বর্ণ *)সারি, fp_out);

		গুনতি++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	যাবত্ (!(eof = viminfo_readline(virp)) && সারি[0] == TAB)
	{
	    যদি (load_marks)
	    {
		যদি (সারি[1] != NUL)
		{
		    unsigned u;

		    sscanf((বর্ণ *)সারি + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    বিকল্প (সারি[1])
		    {
			পক্ষ '"': curbuf->b_last_cursor = pos; ভঙ্গ;
			পক্ষ '^': curbuf->b_last_ঢোকাও = pos; ভঙ্গ;
			পক্ষ '.': curbuf->b_last_change = pos; ভঙ্গ;
			পক্ষ '+':
				  // changelist positions are stored oldest
				  // first
				  যদি (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    পরিমাণ(pos_T) * (JUMPLISTSIZE - 1));
				  অন্যথা
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  ভঙ্গ;

				  // Using the line number for the last-used
				  // timestamp.
			পক্ষ '*': curbuf->b_last_used = pos.lnum; ভঙ্গ;

			স্বভাব:  যদি ((i = সারি[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    অন্যথা যদি (copy_marks_out)
		fputs((বর্ণ *)সারি, fp_out);
	}

	যদি (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		যদি (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    যদি (flags & VIF_ONLY_CURBUF)
		ভঙ্গ;
	}
    }

    যদি (fp_out != রিক্ত)
	// Write any remaining entries from buflist.
	যাবত্ (গুনতি < অঙ্ক_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++গুনতি;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    শূন্য
check_marks_read(শূন্য)
{
    যদি (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != রিক্ত)
	read_viminfo(রিক্ত, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    স্থির পূর্ণাঙ্ক
read_viminfo_filemark(vir_T *virp, পূর্ণাঙ্ক force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    পূর্ণাঙ্ক		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_সারি + 1;
    যদি (*str <= 127
	    && ((*virp->vir_সারি == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_সারি == '-' && *str == '\'')))
    {
	যদি (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    যদি (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = রিক্ত;
	    অন্যথা
	    {
		প্রতি (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = রিক্ত;
	    }
	}
	অন্যথা যদি (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	অন্যথা
	    fm = &namedfm_p[*str - 'A'];
	যদি (fm != রিক্ত && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (পূর্ণাঙ্ক)(str - virp->vir_সারি),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    প্রত্যাগম vim_fgets(virp->vir_সারি, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    স্থির শূন্য
prepare_viminfo_marks(শূন্য)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    স্থির শূন্য
finish_viminfo_marks(শূন্য)
{
    পূর্ণাঙ্ক		i;

    যদি (vi_namedfm != রিক্ত)
    {
	প্রতি (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    যদি (vi_jumplist != রিক্ত)
    {
	প্রতি (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    স্থির শূন্য
handle_viminfo_mark(garray_T *মানসমূহ, পূর্ণাঙ্ক force)
{
    bval_T	*vp = (bval_T *)মানসমূহ->ga_data;
    পূর্ণাঙ্ক		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = রিক্ত;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    যদি (মানসমূহ->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	প্রত্যাগম;

    name = vp[0].bv_nr;
    যদি (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	প্রত্যাগম;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    যদি (lnum <= 0 || col < 0)
	প্রত্যাগম;
    timestamp = (time_t)vp[3].bv_nr;

    যদি (name == '\'')
    {
	যদি (vi_jumplist != রিক্ত)
	{
	    যদি (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক idx;
	    পূর্ণাঙ্ক i;

	    // If we have a timestamp insert it in the right place.
	    যদি (timestamp != 0)
	    {
		প্রতি (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    যদি (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			ভঙ্গ;
		    }
		// idx cannot be zero now
		যদি (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    অন্যথা যদি (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    অন্যথা
		idx = -1;

	    যদি (idx >= 0)
	    {
		যদি (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    প্রতি (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		অন্যথা
		{
		    // Move newer entries forward.
		    প্রতি (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = রিক্ত;
		fm->time_set = 0;
	    }
	}
    }
    অন্যথা
    {
	পূর্ণাঙ্ক		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	যদি (VIM_ISDIGIT(name))
	{
	    যদি (vi_namedfm != রিক্ত)
		idx = name - '0' + NMARKS;
	    অন্যথা
	    {
		পূর্ণাঙ্ক i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		প্রতি (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    যদি (namedfm_p[idx].time_set < timestamp)
			ভঙ্গ;
		যদি (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    প্রত্যাগম;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		প্রতি ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = রিক্ত;
	    }
	}
	অন্যথা
	    idx = name - 'A';
	যদি (vi_namedfm != রিক্ত)
	    fm = &vi_namedfm[idx];
	অন্যথা
	    fm = &namedfm_p[idx];
    }

    যদি (fm != রিক্ত)
    {
	যদি (vi_namedfm != রিক্ত || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    যদি (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = রিক্ত;
	    }
	    অন্যথা
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    স্থির পূর্ণাঙ্ক
read_viminfo_barline(vir_T *virp, পূর্ণাঙ্ক got_encoding, পূর্ণাঙ্ক force, পূর্ণাঙ্ক writing)
{
    char_u	*p = virp->vir_সারি + 1;
    পূর্ণাঙ্ক		bartype;
    garray_T	মানসমূহ;
    bval_T	*vp;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    যদি (*p == '<')
    {
	// Continuation line of an unrecognized item.
	যদি (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_সারি);
    }
    অন্যথা
    {
	ga_init2(&মানসমূহ, পরিমাণ(bval_T), 20);
	bartype = getdigits(&p);
	বিকল্প (bartype)
	{
	    পক্ষ BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		যদি (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &মানসমূহ);
		    vp = (bval_T *)মানসমূহ.ga_data;
		    যদি (মানসমূহ.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		ভঙ্গ;

	    পক্ষ BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &মানসমূহ);
		handle_viminfo_history(&মানসমূহ, writing);
		ভঙ্গ;

	    পক্ষ BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &মানসমূহ);
		handle_viminfo_register(&মানসমূহ, force);
		ভঙ্গ;

	    পক্ষ BARTYPE_MARK:
		read_next = barline_parse(virp, p, &মানসমূহ);
		handle_viminfo_mark(&মানসমূহ, force);
		ভঙ্গ;

	    স্বভাব:
		// copy unrecognized line (for future use)
		যদি (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_সারি);
	}
	প্রতি (i = 0; i < মানসমূহ.ga_len; ++i)
	{
	    vp = (bval_T *)মানসমূহ.ga_data + i;
	    যদি (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&মানসমূহ);
    }

    যদি (read_next)
	প্রত্যাগম viminfo_readline(virp);
    প্রত্যাগম FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    স্থির পূর্ণাঙ্ক
read_viminfo_up_to_marks(
    vir_T	*virp,
    পূর্ণাঙ্ক		forceit,
    পূর্ণাঙ্ক		writing)
{
    পূর্ণাঙ্ক		eof;
    buf_T	*buf;
    পূর্ণাঙ্ক		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    যাবত্ (!eof && virp->vir_সারি[0] != '>')
    {
	বিকল্প (virp->vir_সারি[0])
	{
		// Characters reserved for future expansion, ignored now
	    পক্ষ '+': // "+40 /path/dir file", for running vim without args
	    পক্ষ '^': // to be defined
	    পক্ষ '<': // long line - ignored
		// A comment or empty line.
	    পক্ষ NUL:
	    পক্ষ '\r':
	    পক্ষ '\n':
	    পক্ষ '#':
		eof = viminfo_readline(virp);
		ভঙ্গ;
	    পক্ষ '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		ভঙ্গ;
	    পক্ষ '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		ভঙ্গ;
	    পক্ষ '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#অন্যথা
		eof = viminfo_readline(virp);
#endif
		ভঙ্গ;
	    পক্ষ '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		ভঙ্গ;
	    পক্ষ '"':
		// When registers are in bar lines skip the old style register
		// lines.
		যদি (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		অন্যথা
		    কুরু
		    {
			eof = viminfo_readline(virp);
		    } যাবত্ (!eof && (virp->vir_সারি[0] == TAB
						|| virp->vir_সারি[0] == '<'));
		ভঙ্গ;
	    পক্ষ '/':	    // Search string
	    পক্ষ '&':	    // Substitute search string
	    পক্ষ '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_অনুসন্ধান_pattern(virp, forceit, writing);
		ভঙ্গ;
	    পক্ষ '$':
		eof = read_viminfo_sub_string(virp, forceit);
		ভঙ্গ;
	    পক্ষ ':':
	    পক্ষ '?':
	    পক্ষ '=':
	    পক্ষ '@':
		// When history is in bar lines skip the old style history
		// lines.
		যদি (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		অন্যথা
		    eof = viminfo_readline(virp);
		ভঙ্গ;
	    পক্ষ '-':
	    পক্ষ '\'':
		// When file marks are in bar lines skip the old style lines.
		যদি (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		অন্যথা
		    eof = viminfo_readline(virp);
		ভঙ্গ;
	    স্বভাব:
		যদি (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_সারি))
		    eof = TRUE;
		অন্যথা
		    eof = viminfo_readline(virp);
		ভঙ্গ;
	}
    }

    // Finish reading history items.
    যদি (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    প্রত্যাগম eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    স্থির শূন্য
do_viminfo(FILE *fp_in, FILE *fp_out, পূর্ণাঙ্ক flags)
{
    পূর্ণাঙ্ক		eof = FALSE;
    vir_T	vir;
    পূর্ণাঙ্ক		merge = FALSE;
    পূর্ণাঙ্ক		do_copy_marks = FALSE;
    garray_T	buflist;

    যদি ((vir.vir_সারি = alloc(LSIZE)) == রিক্ত)
	প্রত্যাগম;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, পরিমাণ(char_u *), 100);
    vir.vir_version = -1;

    যদি (fp_in != রিক্ত)
    {
	যদি (flags & VIF_WANT_INFO)
	{
	    যদি (fp_out != রিক্ত)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != রিক্ত);
	    merge = TRUE;
	}
	অন্যথা যদি (flags != 0)
	    // Skip info, find start of marks
	    যাবত্ (!(eof = viminfo_readline(&vir))
		    && vir.vir_সারি[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    যদি (fp_out != রিক্ত)
    {
	// Write the info:
	fprintf(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_out);
	fprintf(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_অনুসন্ধান_pattern(fp_out);
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

	যদি (do_copy_marks)
	    ga_init2(&buflist, পরিমাণ(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : রিক্ত);
    }

    যদি (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	যদি (fp_out != রিক্ত)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_সারি);
    যদি (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, রিক্ত, রিক্ত);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    পূর্ণাঙ্ক
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    পূর্ণাঙ্ক		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    যদি (no_viminfo())
	প্রত্যাগম FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    যদি (fname == রিক্ত)
	প্রত্যাগম FAIL;
    fp = mch_fopen((বর্ণ *)fname, READBIN);

    যদি (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == রিক্ত ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    যদি (fp == রিক্ত)
	প্রত্যাগম FAIL;
    যদি (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	প্রত্যাগম FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, রিক্ত, flags);

    fclose(fp);
    প্রত্যাগম OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    শূন্য
write_viminfo(char_u *file, পূর্ণাঙ্ক forceit)
{
    char_u	*fname;
    FILE	*fp_in = রিক্ত;	// input viminfo file, if any
    FILE	*fp_out = রিক্ত;	// output viminfo file
    char_u	*tempname = রিক্ত;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#যদি defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    পূর্ণাঙ্ক		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    পূর্ণাঙ্ক		hidden = FALSE;
#endif

    যদি (no_viminfo())
	প্রত্যাগম;

    fname = viminfo_filename(file);	// may set to default if NULL
    যদি (fname == রিক্ত)
	প্রত্যাগম;

    fp_in = mch_fopen((বর্ণ *)fname, READBIN);
    যদি (fp_in == রিক্ত)
    {
	পূর্ণাঙ্ক fd;

	// if it does exist, but we can't read it, don't try writing
	যদি (mch_stat((বর্ণ *)fname, &st_new) == 0)
	    গচ্ছ end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((বর্ণ *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	যদি (fd < 0)
	    গচ্ছ end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    অন্যথা
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	যদি (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != মূল_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    পূর্ণাঙ্ক	tt = বার্তা_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    বার্তা_didany = tt;
	    fclose(fp_in);
	    গচ্ছ end;
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
	প্রতি (;;)
	{
	    পূর্ণাঙ্ক		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#অন্যথা
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#অন্যথা
				    (char_u *)".tmp",
#endif
				    FALSE);
	    যদি (tempname == রিক্ত)		// out of memory
		ভঙ্গ;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    যদি (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    প্রতি (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		যদি (mch_stat((বর্ণ *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    যদি (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			ভঙ্গ;
		    }
#endif
		}
		অন্যথা
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((বর্ণ *)tempname, WRITEBIN);
		    (শূন্য)umask(umask_save);
#অন্যথা
		    পূর্ণাঙ্ক	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((বর্ণ *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(পূর্ণাঙ্ক)((st_old.st_mode & 0777) | 0600));
		    (শূন্য)umask(umask_save);
# অন্যথা
		    fd = mch_open((বর্ণ *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    যদি (fd < 0)
		    {
			fp_out = রিক্ত;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			যদি (errno != EEXIST)
			    ভঙ্গ;
# endif
		    }
		    অন্যথা
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    যদি (fp_out != রিক্ত)
			ভঙ্গ;
		}

		// Assume file exists, try again with another name.
		যদি (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    ভঙ্গ;
		}
		*wp = next_char;
		--next_char;
	    }

	    যদি (tempname != রিক্ত)
		ভঙ্গ;
	    // continue if shortname was set
	}

#যদি defined(UNIX) && defined(HAVE_FCHOWN)
	যদি (tempname != রিক্ত && fp_out != রিক্ত)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    যদি (mch_stat((বর্ণ *)tempname, &tmp_st) >= 0)
	    {
		যদি (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		যদি (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (শূন্য)mch_setperm(tempname, 0600);
	    }
	    অন্যথা
		// can't stat the file, set conservative permissions
		(শূন্য)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    যদি (fp_out == রিক্ত)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == রিক্ত || tempname == রিক্ত) ? fname : tempname);
	যদি (fp_in != রিক্ত)
	    fclose(fp_in);
	গচ্ছ end;
    }

    যদি (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    যদি (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    যদি (fp_in != রিক্ত)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	যদি (viminfo_errcnt == 0)
	{
	    যদি (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    অন্যথা যদি (hidden)
		mch_hide(fname);
#endif
	}
	যদি (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    শূন্য
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    যদি (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    যদি (eap->cmdidx == CMD_rviminfo)
    {
	যদি (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    অন্যথা
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
