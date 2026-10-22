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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    char_u	*vir_ಸಾಲು;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    ಪೂರ್ಣಾಙ್ಕ		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    ಪೂರ್ಣಾಙ್ಕ		bv_len;		// length of bv_string
    ಪೂರ್ಣಾಙ್ಕ		bv_allocated;	// bv_string was allocated
} bval_T;

#ಯದಿ defined(FEAT_VIMINFO)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    ಸ್ಥಿರ char_u *
find_viminfo_parameter(ಪೂರ್ಣಾಙ್ಕ type)
{
    char_u  *p;

    ಪ್ರತಿ (p = p_viminfo; *p; ++p)
    {
	ಯದಿ (*p == type)
	    ಪ್ರತ್ಯಾಗಮ p + 1;
	ಯದಿ (*p == 'n')		    // 'n' is always the last one
	    ಭಙ್ಗ;
	p = vim_strchr(p, ',');	    // skip until next ','
	ಯದಿ (p == ರಿಕ್ತ)		    // hit the end without finding parameter
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_viminfo_parameter(ಪೂರ್ಣಾಙ್ಕ type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    ಯದಿ (p != ರಿಕ್ತ && VIM_ISDIGIT(*p))
	ಪ್ರತ್ಯಾಗಮ atoi((ವರ್ಣ *)p);
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    ಸ್ಥಿರ char_u *
viminfo_filename(char_u *file)
{
    ಯದಿ (file == ರಿಕ್ತ || *file == NUL)
    {
	size_t  len;

	ಯದಿ (*p_viminfofile != NUL)
	    file = p_viminfofile;
	ಅನ್ಯಥಾ ಯದಿ ((file = find_viminfo_parameter('n')) == ರಿಕ್ತ || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    ಯದಿ (mch_getenv((char_u *)"SYS$LOGIN") == ರಿಕ್ತ)
# ಅನ್ಯಥಾ
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    ಯದಿ (STRCMP(vim_getenv((char_u *)"HOME", ರಿಕ್ತ), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == ರಿಕ್ತ)
#  ಅನ್ಯಥಾ
	    ಯದಿ (mch_getenv((char_u *)"HOME") == ರಿಕ್ತ)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		ಯದಿ (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		ಅನ್ಯಥಾ
		    file = (char_u *)VIMINFO_FILE;
	    }
	    ಅನ್ಯಥಾ
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	ಪ್ರತ್ಯಾಗಮ vim_strnsave(file, len);
    }

    ಪ್ರತ್ಯಾಗಮ vim_strsave(file);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
viminfo_writestring(FILE *fd, char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;

    ಪ್ರತಿ (s = p; *s != NUL; ++s)
    {
	ಯದಿ (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    ಯದಿ (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    ಯಾವತ್ ((c = *p++) != NUL)
    {
	ಯದಿ (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    ಯದಿ (c == '\n')
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
barline_writestring(FILE *fd, char_u *s, ಪೂರ್ಣಾಙ್ಕ remaining_start)
{
    char_u *p;
    ಪೂರ್ಣಾಙ್ಕ	    remaining = remaining_start;
    ಪೂರ್ಣಾಙ್ಕ	    len = 2;

    // Count the number of characters produced, including quotes.
    ಪ್ರತಿ (p = s; *p != NUL; ++p)
    {
	ಯದಿ (*p == NL)
	    len += 2;
	ಅನ್ಯಥಾ ಯದಿ (*p == '"' || *p == '\\')
	    len += 2;
	ಅನ್ಯಥಾ
	    ++len;
    }
    ಯದಿ (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    ಪ್ರತಿ (p = s; *p != NUL; ++p)
    {
	ಯದಿ (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	ಅನ್ಯಥಾ
	    putc(*p, fd);
	--remaining;

	ಯದಿ (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    ಪ್ರತ್ಯಾಗಮ remaining - 2;
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
    ಸ್ಥಿರ char_u *
viminfo_readstring(
    vir_T	*virp,
    ಪೂರ್ಣಾಙ್ಕ		off,		    // offset for virp->vir_line
    ಪೂರ್ಣಾಙ್ಕ		convert)	    // convert the string
{
    char_u	*retval = ರಿಕ್ತ;
    char_u	*s, *d;
    long	len;

    ಯದಿ (virp->vir_ಸಾಲು[off] == Ctrl_V && vim_isdigit(virp->vir_ಸಾಲು[off + 1]))
    {
	len = atol((ವರ್ಣ *)virp->vir_ಸಾಲು + off + 1);
	ಯದಿ (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	ಯದಿ (retval == ರಿಕ್ತ)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (ಶೂನ್ಯ)vim_fgets(virp->vir_ಸಾಲು, 10, virp->vir_fd);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	(ಶೂನ್ಯ)vim_fgets(retval, (ಪೂರ್ಣಾಙ್ಕ)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    ಅನ್ಯಥಾ
    {
	retval = vim_strsave(virp->vir_ಸಾಲು + off);
	ಯದಿ (retval == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    ಯಾವತ್ (*s != NUL && *s != '\n')
    {
	ಯದಿ (s[0] == Ctrl_V && s[1] != NUL)
	{
	    ಯದಿ (s[1] == 'n')
		*d++ = '\n';
	    ಅನ್ಯಥಾ
		*d++ = Ctrl_V;
	    s += 2;
	}
	ಅನ್ಯಥಾ
	    *d++ = *s++;
    }
    *d = NUL;

    ಯದಿ (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, ರಿಕ್ತ);
	ಯದಿ (d != ರಿಕ್ತ)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
viminfo_readline(vir_T *virp)
{
    ಪ್ರತ್ಯಾಗಮ vim_fgets(virp->vir_ಸಾಲು, LSIZE, virp->vir_fd);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_viminfo_bufferlist(
    vir_T	*virp,
    ಪೂರ್ಣಾಙ್ಕ		writing)
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
    ಯದಿ (xline != ರಿಕ್ತ && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != ರಿಕ್ತ)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	ಯದಿ (tab != ರಿಕ್ತ)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((ವರ್ಣ *)tab);
	    tab = vim_strrchr(xline, '\t');
	    ಯದಿ (tab != ರಿಕ್ತ)
	    {
		*tab++ = '\0';
		lnum = atol((ವರ್ಣ *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	ಯದಿ (buf != ರಿಕ್ತ)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    ಪ್ರತ್ಯಾಗಮ viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    ಪೂರ್ಣಾಙ್ಕ	    part_len;
    ಪೂರ್ಣಾಙ್ಕ	    retval = FALSE;

    name = home_replace_save(ರಿಕ್ತ, name);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, ಪರಿಮಾಣ(part), ", ");
	ಯದಿ (part[0] == 'r')
	{
	    ಯದಿ (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		ಭಙ್ಗ;
	    }
	}
    }
    vim_free(name);
    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		max_buffers;

    ಯದಿ (find_viminfo_parameter('%') == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define ಸಾಲು_BUF_LEN (MAXPATHL + 40)
    ಸಾಲು = alloc(ಸಾಲು_BUF_LEN);
    ಯದಿ (ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	ಯದಿ (buf->b_fname == ರಿಕ್ತ
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    ಅನುವರ್ತ;

	ಯದಿ (max_buffers-- == 0)
	    ಭಙ್ಗ;
	putc('%', fp);
	linelen = home_replace(ರಿಕ್ತ, buf->b_ffname, ಸಾಲು, MAXPATHL, TRUE);
	vim_snprintf((ವರ್ಣ *)ಸಾಲು + linelen, ಸಾಲು_BUF_LEN - linelen, "\t%ld\t%d",
			(long)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, ಸಾಲು);
    }
    vim_free(ಸಾಲು);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
ಸ್ಥಿರ histentry_T *viminfo_history[HIST_ಎಣಿಕೆ] =
					       {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	viminfo_hisidx[HIST_ಎಣಿಕೆ] = {0, 0, 0, 0, 0};
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	viminfo_hislen[HIST_ಎಣಿಕೆ] = {0, 0, 0, 0, 0};
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
hist_type2char(
    ಪೂರ್ಣಾಙ್ಕ	    type,
    ಪೂರ್ಣಾಙ್ಕ	    use_question)	    // use '?' instead of '/'
{
    ಯದಿ (type == HIST_CMD)
	ಪ್ರತ್ಯಾಗಮ ':';
    ಯದಿ (type == HIST_ಹುಡುಕು)
    {
	ಯದಿ (use_question)
	    ಪ್ರತ್ಯಾಗಮ '?';
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ '/';
    }
    ಯದಿ (type == HIST_ಅಭಿವ್ಯಕ್ತಿ)
	ಪ್ರತ್ಯಾಗಮ '=';
    ಪ್ರತ್ಯಾಗಮ '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_viminfo_history(ಪೂರ್ಣಾಙ್ಕ asklen, ಪೂರ್ಣಾಙ್ಕ writing)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    ಅಂಕೆ;
    ಪೂರ್ಣಾಙ್ಕ	    type;
    ಪೂರ್ಣಾಙ್ಕ	    len;
    ಪೂರ್ಣಾಙ್ಕ	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    ಯದಿ (asklen > hislen)
	asklen = hislen;

    ಪ್ರತಿ (type = 0; type < HIST_ಎಣಿಕೆ; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	ಪ್ರತಿ (i = 0, ಅಂಕೆ = 0; i < hislen; i++)
	    ಯದಿ (histentry[i].hisstr == ರಿಕ್ತ || histentry[i].viminfo)
		ಅಂಕೆ++;
	len = asklen;
	ಯದಿ (ಅಂಕೆ > len)
	    len = ಅಂಕೆ;
	ಯದಿ (len <= 0)
	    viminfo_history[type] = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	ಯದಿ (viminfo_history[type] == ರಿಕ್ತ)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_viminfo_history(vir_T *virp, ಪೂರ್ಣಾಙ್ಕ writing)
{
    ಪೂರ್ಣಾಙ್ಕ		type;
    long_u	len;
    char_u	*val = ರಿಕ್ತ;
    char_u	*p;

    type = hist_char2type(virp->vir_ಸಾಲು[0]);
    ಯದಿ (viminfo_hisidx[type] >= viminfo_hislen[type])
	ಗಚ್ಛ done;

    val = viminfo_readstring(virp, 1, TRUE);
    ಯದಿ (val == ರಿಕ್ತ || *val == NUL)
	ಗಚ್ಛ done;

    ಪೂರ್ಣಾಙ್ಕ sep = (*val == ' ' ? NUL : *val);

    ಯದಿ (in_history(type, val + (type == HIST_ಹುಡುಕು), viminfo_add_at_front,
								sep, writing))
	ಗಚ್ಛ done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    ಯದಿ (type == HIST_ಹುಡುಕು)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	ಯದಿ (p == ರಿಕ್ತ)
	    ಗಚ್ಛ done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    ಅನ್ಯಥಾ
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	ಯದಿ (p == ರಿಕ್ತ)
	    ಗಚ್ಛ done;

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
    ಪ್ರತ್ಯಾಗಮ viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_viminfo_history(
	garray_T    *ಮೌಲ್ಯಗಳು,
	ಪೂರ್ಣಾಙ್ಕ	    writing)
{
    ಪೂರ್ಣಾಙ್ಕ		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)ಮೌಲ್ಯಗಳು->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    ಯದಿ (ಮೌಲ್ಯಗಳು->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	ಪ್ರತ್ಯಾಗಮ;

    type = vp[0].bv_nr;
    ಯದಿ (type >= HIST_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (viminfo_hisidx[type] >= viminfo_hislen[type])
	ಪ್ರತ್ಯಾಗಮ;

    val = vp[3].bv_string;
    ಯದಿ (val == ರಿಕ್ತ || *val == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ sep = type == HIST_ಹುಡುಕು && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    ಪೂರ್ಣಾಙ್ಕ idx;
    ಪೂರ್ಣಾಙ್ಕ overwrite = FALSE;

    ಯದಿ (in_history(type, val, viminfo_add_at_front, sep, writing))
	ಪ್ರತ್ಯಾಗಮ;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    ಪ್ರತಿ (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	ಯದಿ (STRCMP(val, p) == 0
		&& (type != HIST_ಹುಡುಕು || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    ಅನ್ಯಥಾ
	len = 0; // for picky compilers
    ಯದಿ (p != ರಿಕ್ತ)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	ಯದಿ (!overwrite)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
concat_history(ಪೂರ್ಣಾಙ್ಕ type)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    ಪೂರ್ಣಾಙ್ಕ		*hisidx = get_hisidx(type);
    ಪೂರ್ಣಾಙ್ಕ		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    ಯದಿ (idx >= hislen)
	idx -= hislen;
    ಅನ್ಯಥಾ ಯದಿ (idx < 0)
	idx = hislen - 1;
    ಯದಿ (viminfo_add_at_front)
	*hisidx = idx;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (*hisidx == -1)
	    *hisidx = hislen - 1;
	ಕುರು
	{
	    ಯದಿ (histentry[idx].hisstr != ರಿಕ್ತ || histentry[idx].viminfo)
		ಭಙ್ಗ;
	    ಯದಿ (++idx == hislen)
		idx = 0;
	} ಯಾವತ್ (idx != *hisidx);
	ಯದಿ (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    ಪ್ರತಿ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	ಯದಿ (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    ಪ್ರತಿ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sort_hist(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    ಯದಿ (p1->time_set < p2->time_set) ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (p1->time_set > p2->time_set) ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
merge_history(ಪೂರ್ಣಾಙ್ಕ type)
{
    ಪೂರ್ಣಾಙ್ಕ		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    ಪೂರ್ಣಾಙ್ಕ		*hisidx = get_hisidx(type);
    ಪೂರ್ಣಾಙ್ಕ		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    ಯದಿ (tot_hist == ರಿಕ್ತ || new_hist == ರಿಕ್ತ)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    ಪ್ರತಿ (i = 0; i < hislen; i++)
	ಯದಿ (histentry[i].hisstr != ರಿಕ್ತ)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((ಶೂನ್ಯ *)tot_hist, (size_t)len, ಪರಿಮಾಣ(histentry_T *), sort_hist);

    // Keep the newest ones.
    ಪ್ರತಿ (i = 0; i < hislen; i++)
    {
	ಯದಿ (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = ರಿಕ್ತ;
	    tot_hist[i]->hisstrlen = 0;
	    ಯದಿ (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	ಅನ್ಯಥಾ
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    ಪ್ರತಿ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    ಪ್ರತಿ (i = 0; i < hislen; i++)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
finish_viminfo_history(vir_T *virp)
{
    ಪೂರ್ಣಾಙ್ಕ	type;
    ಪೂರ್ಣಾಙ್ಕ merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    ಪ್ರತಿ (type = 0; type < HIST_ಎಣಿಕೆ; ++type)
    {
	ಯದಿ (get_histentry(type) == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	ಯದಿ (merge)
	    merge_history(type);
	ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_history(FILE *fp, ಪೂರ್ಣಾಙ್ಕ merge)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    type;
    ಪೂರ್ಣಾಙ್ಕ	    ಅಂಕೆ_saved;
    ಪೂರ್ಣಾಙ್ಕ     round;
    ಪೂರ್ಣಾಙ್ಕ	    hislen;

    init_history();
    hislen = get_hislen();
    ಯದಿ (hislen == 0)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (type = 0; type < HIST_ಎಣಿಕೆ; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	ಪೂರ್ಣಾಙ್ಕ	    *hisidx = get_hisidx(type);

	ಅಂಕೆ_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	ಯದಿ (ಅಂಕೆ_saved == 0)
	    ಅನುವರ್ತ;
	ಯದಿ (ಅಂಕೆ_saved < 0)  // Use default
	    ಅಂಕೆ_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_ಹುಡುಕು ? _("Search String") :
			    type == HIST_ಅಭಿವ್ಯಕ್ತಿ ? _("Expression") :
			    type == HIST_ಒಳಹರಿವು ? _("Input Line") :
					_("Debug Line"));
	ಯದಿ (ಅಂಕೆ_saved > hislen)
	    ಅಂಕೆ_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	ಪ್ರತಿ (round = 1; round <= 2; ++round)
	{
	    ಯದಿ (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    ಅನ್ಯಥಾ ಯದಿ (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    ಅನ್ಯಥಾ
		// empty list
		i = -1;
	    ಯದಿ (i >= 0)
		ಯಾವತ್ (ಅಂಕೆ_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    ಪೂರ್ಣಾಙ್ಕ	    c = NUL;

		    ಯದಿ (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (viminfo_history[type] == ರಿಕ್ತ)
			{
			    p = ರಿಕ್ತ;
			    plen = 0;
			    timestamp = 0;
			}
			ಅನ್ಯಥಾ
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    ಯದಿ (p != ರಿಕ್ತ && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--ಅಂಕೆ_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			ಯದಿ (type == HIST_ಹುಡುಕು)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    ವರ್ಣ    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    ಯದಿ (c == NUL)
				cbuf[0] = NUL;
			    ಅನ್ಯಥಾ
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    ಯದಿ (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			ಯದಿ (--i < 0)
			    i = hislen - 1;
			ಯದಿ (i == *hisidx)
			    ಭಙ್ಗ;
		    }
		    ಅನ್ಯಥಾ
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	ಪ್ರತಿ (i = 0; i < viminfo_hisidx[type]; ++i)
	    ಯದಿ (viminfo_history[type] != ರಿಕ್ತ)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_barlines(vir_T *virp, FILE *fp_out)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    garray_T	*gap = &virp->vir_barlines;
    ಪೂರ್ಣಾಙ್ಕ		seen_useful = FALSE;
    ವರ್ಣ	*ಸಾಲು;

    ಯದಿ (gap->ga_len <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	ಸಾಲು = ((ವರ್ಣ **)(gap->ga_data))[i];
	ಯದಿ (seen_useful || ಸಾಲು[1] != '<')
	{
	    fputs(ಸಾಲು, fp_out);
	    seen_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
barline_parse(vir_T *virp, char_u *text, garray_T *ಮೌಲ್ಯಗಳು)
{
    char_u  *p = text;
    char_u  *nextp = ರಿಕ್ತ;
    char_u  *buf = ರಿಕ್ತ;
    bval_T  *ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    allocated = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    eof;
    char_u  *sconv;
    ಪೂರ್ಣಾಙ್ಕ	    converted;

    ಯಾವತ್ (*p == ',')
    {
	++p;
	ಯದಿ (ga_grow(ಮೌಲ್ಯಗಳು, 1) == FAIL)
	    ಭಙ್ಗ;
	ಮೌಲ್ಯ = (bval_T *)(ಮೌಲ್ಯಗಳು->ga_data) + ಮೌಲ್ಯಗಳು->ga_len;

	ಯದಿ (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    ಯದಿ (!allocated)
	    {
		ಪ್ರತಿ (i = 0; i < ಮೌಲ್ಯಗಳು->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(ಮೌಲ್ಯಗಳು->ga_data) + i;

		    ಯದಿ (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    ಯದಿ (vim_isdigit(p[1]))
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
		ಯದಿ (buf == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ TRUE;
		p = buf;
		ಪ್ರತಿ (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    ಯದಿ (eof || virp->vir_ಸಾಲು[0] != '|'
						  || virp->vir_ಸಾಲು[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			ಪ್ರತ್ಯಾಗಮ eof || virp->vir_ಸಾಲು[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_ಸಾಲು);
		    ಯಾವತ್ (n > 2 && (virp->vir_ಸಾಲು[n - 1] == NL
					     || virp->vir_ಸಾಲು[n - 1] == CAR))
			--n;
		    n -= 2;
		    ಯದಿ (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_ಸಾಲು + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_ಸಾಲು + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		ಯದಿ (eof || virp->vir_ಸಾಲು[0] != '|'
					      || virp->vir_ಸಾಲು[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    ಪ್ರತ್ಯಾಗಮ eof || virp->vir_ಸಾಲು[0] == '|';
		p = virp->vir_ಸಾಲು + 2;
	    }
	}

	ಯದಿ (SAFE_isdigit(*p))
	{
	    ಮೌಲ್ಯ->bv_type = BVAL_NR;
	    ಮೌಲ್ಯ->bv_nr = getdigits(&p);
	    ++ಮೌಲ್ಯಗಳು->ga_len;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '"')
	{
	    ಪೂರ್ಣಾಙ್ಕ	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    ಯಾವತ್ (*p != '"')
	    {
		ಯದಿ (*p == NL || *p == NUL)
		    ಪ್ರತ್ಯಾಗಮ TRUE;  // syntax error, drop the value
		ಯದಿ (*p == '\\')
		{
		    ++p;
		    ಯದಿ (*p == 'n')
			s[len++] = '\n';
		    ಅನ್ಯಥಾ
			s[len++] = *p;
		    ++p;
		}
		ಅನ್ಯಥಾ
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    ಮೌಲ್ಯ->bv_tofree = ರಿಕ್ತ;
	    ಯದಿ (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, ರಿಕ್ತ);
		ಯದಿ (sconv != ರಿಕ್ತ)
		{
		    ಯದಿ (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			ಮೌಲ್ಯ->bv_tofree = buf;
		    s = sconv;
		    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    ಯದಿ (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    ಮೌಲ್ಯ->bv_string = s;
	    ಮೌಲ್ಯ->bv_type = BVAL_STRING;
	    ಮೌಲ್ಯ->bv_len = len;
	    ಮೌಲ್ಯ->bv_allocated = allocated || converted;
	    ++ಮೌಲ್ಯಗಳು->ga_len;
	    ಯದಿ (nextp != ರಿಕ್ತ)
	    {
		// values following a long string
		p = nextp;
		nextp = ರಿಕ್ತ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == ',')
	{
	    ಮೌಲ್ಯ->bv_type = BVAL_EMPTY;
	    ++ಮೌಲ್ಯಗಳು->ga_len;
	}
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
no_viminfo(ಶೂನ್ಯ)
{
    // "vim -i NONE" does not read or write a viminfo file
    ಪ್ರತ್ಯಾಗಮ STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
viminfo_error(ವರ್ಣ *errnum, ವರ್ಣ *message, char_u *ಸಾಲು)
{
    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, ಸಾಲು, IOSIZE - STRLEN(IObuff) - 1);
    ಯದಿ (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((ವರ್ಣ *)IObuff);
    ಯದಿ (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_ಸಾಲು, '=');
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    // remove trailing newline
	    ++p;
	    ಪ್ರತಿ (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    ಪ್ರತ್ಯಾಗಮ viminfo_readline(virp);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_viminfo_varlist(vir_T *virp, ಪೂರ್ಣಾಙ್ಕ writing)
{
    char_u	*tab;
    ಪೂರ್ಣಾಙ್ಕ		type = VAR_ಸಂಖ್ಯೆ;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    ಯದಿ (!writing && (find_viminfo_parameter('!') != ರಿಕ್ತ))
    {
	tab = vim_strchr(virp->vir_ಸಾಲು + 1, '\t');
	ಯದಿ (tab != ರಿಕ್ತ)
	{
	    *tab++ = '\0';	// isolate the variable name
	    ವಿಕಲ್ಪ (*tab)
	    {
		ಪಕ್ಷ 'S': type = VAR_STRING; ಭಙ್ಗ;
		ಪಕ್ಷ 'F': type = VAR_FLOAT; ಭಙ್ಗ;
		ಪಕ್ಷ 'D': type = VAR_DICT; ಭಙ್ಗ;
		ಪಕ್ಷ 'L': type = VAR_LIST; ಭಙ್ಗ;
		ಪಕ್ಷ 'B': type = VAR_BLOB; ಭಙ್ಗ;
		ಪಕ್ಷ 'X': type = VAR_SPECIAL; ಭಙ್ಗ;
		ಪಕ್ಷ 'T': type = VAR_TUPLE; ಭಙ್ಗ;
	    }

	    tab = vim_strchr(tab, '\t');
	    ಯದಿ (tab != ರಿಕ್ತ)
	    {
		tv.v_type = type;
		ಯದಿ (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (ಪೂರ್ಣಾಙ್ಕ)(tab - virp->vir_ಸಾಲು + 1), TRUE);
		ಅನ್ಯಥಾ ಯದಿ (type == VAR_FLOAT)
		    (ಶೂನ್ಯ)string2float(tab + 1, &tv.vval.v_float, FALSE);
		ಅನ್ಯಥಾ
		{
		    tv.vval.v_ಸಂಖ್ಯೆ = atol((ವರ್ಣ *)tab + 1);
		    ಯದಿ (type == VAR_SPECIAL && (tv.vval.v_ಸಂಖ್ಯೆ == VVAL_FALSE
					     || tv.vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		ಯದಿ (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_ಅಭಿವ್ಯಕ್ತಿ(tv.vval.v_string, ರಿಕ್ತ);

		    ಯದಿ (etv == ರಿಕ್ತ)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    ಅನ್ಯಥಾ
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    ಯದಿ (blob == ರಿಕ್ತ)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    ಅನ್ಯಥಾ
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_entry);
		set_var(virp->vir_ಸಾಲು + 1, &tv, FALSE);
		restore_funccal();

		ಯದಿ (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		ಅನ್ಯಥಾ ಯದಿ (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    ವರ್ಣ	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ಯದಿ (find_viminfo_parameter('!') == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    fputs(_("\n# global variables:\n"), fp);

    todo = (ಪೂರ್ಣಾಙ್ಕ)gvht->ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(gvht, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ಯದಿ (var_flavour(this_var->di_ಕೀಲಿ) == VAR_FLAVOUR_VIMINFO)
	    {
		ವಿಕಲ್ಪ (this_var->di_tv.v_type)
		{
		    ಪಕ್ಷ VAR_STRING:  s = "STR"; ಭಙ್ಗ;
		    ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:  s = "NUM"; ಭಙ್ಗ;
		    ಪಕ್ಷ VAR_FLOAT:   s = "FLO"; ಭಙ್ಗ;
		    ಪಕ್ಷ VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      ಪೂರ್ಣಾಙ್ಕ	copyID = get_copyID();

			      s = "DIC";
			      ಯದಿ (di != ರಿಕ್ತ && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, ರಿಕ್ತ, ರಿಕ್ತ)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ಅನುವರ್ತ;
			      ಭಙ್ಗ;
			  }
		    ಪಕ್ಷ VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      ಪೂರ್ಣಾಙ್ಕ	copyID = get_copyID();

			      s = "LIS";
			      ಯದಿ (l != ರಿಕ್ತ && !set_ref_in_list_ವಸ್ತುಗಳು(
						       l, copyID, ರಿಕ್ತ, ರಿಕ್ತ)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ಅನುವರ್ತ;
			      ಭಙ್ಗ;
			  }
		    ಪಕ್ಷ VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      ಪೂರ್ಣಾಙ್ಕ	copyID = get_copyID();

			      s = "TUP";
			      ಯದಿ (tuple != ರಿಕ್ತ && !set_ref_in_tuple_ವಸ್ತುಗಳು(
					       tuple, copyID, ರಿಕ್ತ, ರಿಕ್ತ)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ಅನುವರ್ತ;
			      ಭಙ್ಗ;
			  }
		    ಪಕ್ಷ VAR_BLOB:    s = "BLO"; ಭಙ್ಗ;
		    ಪಕ್ಷ VAR_BOOL:    s = "XPL"; ಭಙ್ಗ;  // backwards compat.
		    ಪಕ್ಷ VAR_SPECIAL: s = "XPL"; ಭಙ್ಗ;

		    ಪಕ್ಷ VAR_UNKNOWN:
		    ಪಕ್ಷ VAR_ANY:
		    ಪಕ್ಷ VAR_VOID:
		    ಪಕ್ಷ VAR_FUNC:
		    ಪಕ್ಷ VAR_PARTIAL:
		    ಪಕ್ಷ VAR_JOB:
		    ಪಕ್ಷ VAR_CHANNEL:
		    ಪಕ್ಷ VAR_INSTR:
		    ಪಕ್ಷ VAR_CLASS:
		    ಪಕ್ಷ VAR_OBJECT:
		    ಪಕ್ಷ VAR_TYPEALIAS:
				      ಅನುವರ್ತ;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_ಕೀಲಿ, s);
		ಯದಿ (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((ವರ್ಣ *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_ಸಂಖ್ಯೆ);
		    p = numbuf;
		    tofree = ರಿಕ್ತ;
		}
		ಅನ್ಯಥಾ
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		ಯದಿ (p != ರಿಕ್ತ)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_viminfo_sub_string(vir_T *virp, ಪೂರ್ಣಾಙ್ಕ force)
{
    ಯದಿ (force || get_old_sub() == ರಿಕ್ತ)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    ಪ್ರತ್ಯಾಗಮ viminfo_readline(virp);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    ಯದಿ (get_viminfo_parameter('/') == 0 || old_sub == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_viminfo_ಹುಡುಕು_pattern(vir_T *virp, ಪೂರ್ಣಾಙ್ಕ force, ಪೂರ್ಣಾಙ್ಕ writing)
{
    char_u	*lp;
    ಪೂರ್ಣಾಙ್ಕ		idx = -1;
    ಪೂರ್ಣಾಙ್ಕ		magic = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		no_scs = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		off_ಸಾಲು = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		off_end = 0;
    long	off = 0;
    ಪೂರ್ಣಾಙ್ಕ		setlast = FALSE;
#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	hlsearch_on = FALSE;
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
    lp = virp->vir_ಸಾಲು;
    ಯದಿ (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	ಯದಿ (lp[1] == 'M')		// magic on
	    magic = TRUE;
	ಯದಿ (lp[2] == 's')
	    no_scs = TRUE;
	ಯದಿ (lp[3] == 'L')
	    off_ಸಾಲು = TRUE;
	ಯದಿ (lp[4] == 'E')
	    off_end = ಹುಡುಕು_END;
	lp += 5;
	off = getdigits(&lp);
    }
    ಯದಿ (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    ಯದಿ (lp[0] == '/')
	idx = RE_ಹುಡುಕು;
    ಅನ್ಯಥಾ ಯದಿ (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಅನ್ಯಥಾ ಯದಿ (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    ಯದಿ (idx >= 0)
    {
	spat = get_spat(idx);
	ಯದಿ (force || spat->pat == ರಿಕ್ತ)
	{
	    val = viminfo_readstring(virp, (ಪೂರ್ಣಾಙ್ಕ)(lp - virp->vir_ಸಾಲು + 1),
									TRUE);
	    ಯದಿ (val != ರಿಕ್ತ)
	    {
		ಯದಿ (!writing)
		{
		    set_last_ಹುಡುಕು_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.ಸಾಲು = off_ಸಾಲು;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_ಹುಡುಕು_EXTRA
		    ಯದಿ (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ viminfo_readline(virp);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
wvsp_one(
    FILE	*fp,	// file to write to
    ಪೂರ್ಣಾಙ್ಕ		idx,	// spats[] index
    ವರ್ಣ	*s,	// search pat
    ಪೂರ್ಣಾಙ್ಕ		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    ಯದಿ (spat->pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.ಸಾಲು ? 'L' : 'l',	// line offset
	    spat->off.end  ? 'E' : 'e',	// offset from end
	    spat->off.off,			// offset
	    get_spat_last_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_ಹುಡುಕು_pattern(FILE *fp)
{
    ಯದಿ (get_viminfo_parameter('/') == 0)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_ಹುಡುಕು_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != ರಿಕ್ತ) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_ಹುಡುಕು, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

ಸ್ಥಿರ yankreg_T *y_read_regs = ರಿಕ್ತ;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_viminfo_registers(ಶೂನ್ಯ)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, ಅಂಕೆ_REGISTERS);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
finish_viminfo_registers(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		j;

    ಯದಿ (y_read_regs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_REGISTERS; ++i)
	ಯದಿ (y_read_regs[i].y_array != ರಿಕ್ತ)
	{
	    ಪ್ರತಿ (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_viminfo_register(vir_T *virp, ಪೂರ್ಣಾಙ್ಕ force)
{
    ಪೂರ್ಣಾಙ್ಕ		eof;
    ಪೂರ್ಣಾಙ್ಕ		do_it = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		size;
    ಪೂರ್ಣಾಙ್ಕ		limit;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		set_prev = FALSE;
    char_u	*str;
    string_T	*array = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_ಸಾಲು + 1;

    // If the line starts with "" this is the y_previous register.
    ಯದಿ (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    ಯದಿ (!ASCII_ISALNUM(*str) && *str != '-')
    {
	ಯದಿ (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_ಸಾಲು))
	    ಪ್ರತ್ಯಾಗಮ TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    ಯದಿ (!force && y_current_p->y_array != ರಿಕ್ತ)
	do_it = FALSE;

    ಯದಿ (*str == '@')
    {
	// "x@: register x used for @@
	ಯದಿ (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    ಯದಿ (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	ಯದಿ (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	ಯದಿ (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	ಅನ್ಯಥಾ
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    ಯಾವತ್ (!(eof = viminfo_readline(virp))
		    && (virp->vir_ಸಾಲು[0] == TAB || virp->vir_ಸಾಲು[0] == '<'))
    {
	ಯದಿ (do_it)
	{
	    ಯದಿ (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * ಪರಿಮಾಣ(string_T));

		ಯದಿ (new_array == ರಿಕ್ತ)
		{
		    do_it = FALSE;
		    ಭಙ್ಗ;
		}
		ಪ್ರತಿ (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    ಯದಿ (str != ರಿಕ್ತ)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    ಅನ್ಯಥಾ
		// error, don't store the result
		do_it = FALSE;
	}
    }

    ಯದಿ (do_it)
    {
	// free y_array[]
	ಪ್ರತಿ (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	ಯದಿ (size == 0)
	{
	    y_current_p->y_array = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    ಪ್ರತಿ (i = 0; i < size; i++)
	    {
		ಯದಿ (y_current_p->y_array == ರಿಕ್ತ)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		ಅನ್ಯಥಾ
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	// Free array[] if it was filled.
	ಪ್ರತಿ (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    ಪ್ರತ್ಯಾಗಮ eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_viminfo_register(garray_T *ಮೌಲ್ಯಗಳು, ಪೂರ್ಣಾಙ್ಕ force)
{
    bval_T	*vp = (bval_T *)ಮೌಲ್ಯಗಳು->ga_data;
    ಪೂರ್ಣಾಙ್ಕ		flags;
    ಪೂರ್ಣಾಙ್ಕ		name;
    ಪೂರ್ಣಾಙ್ಕ		type;
    ಪೂರ್ಣಾಙ್ಕ		linecount;
    ಪೂರ್ಣಾಙ್ಕ		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    ಯದಿ (ಮೌಲ್ಯಗಳು->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	ಪ್ರತ್ಯಾಗಮ;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    ಯದಿ (name < 0 || name >= ಅಂಕೆ_REGISTERS)
	ಪ್ರತ್ಯಾಗಮ;
    type = vp[2].bv_nr;
    ಯದಿ (type != MCHAR && type != MLINE && type != MBLOCK)
	ಪ್ರತ್ಯಾಗಮ;
    linecount = vp[3].bv_nr;
    ಯದಿ (ಮೌಲ್ಯಗಳು->ga_len < 6 + linecount)
	ಪ್ರತ್ಯಾಗಮ;
    width = vp[4].bv_nr;
    ಯದಿ (width < 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (y_read_regs != ರಿಕ್ತ)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    ಅನ್ಯಥಾ
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    ಯದಿ (y_ptr->y_array != ರಿಕ್ತ && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (y_ptr->y_array != ರಿಕ್ತ)
	ಪ್ರತಿ (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    ಯದಿ (y_read_regs == ರಿಕ್ತ)
    {
	ಯದಿ (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	ಯದಿ ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    ಯದಿ (linecount == 0)
    {
	y_ptr->y_array = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    ಯದಿ (y_ptr->y_array == ರಿಕ್ತ)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (i = 0; i < linecount; i++)
    {
	ಯದಿ (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (vp[i + 6].bv_type != BVAL_STRING)
	{
	    ಮುಕ್ತ(y_ptr->y_array);
	    y_ptr->y_array = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_registers(FILE *fp)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    char_u	*type;
    char_u	c;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಙ್ಕ		max_ಅಂಕೆ_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಙ್ಕ		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_ಅಂಕೆ_ಸಾಲುಗಳು = get_viminfo_parameter('<');
    ಯದಿ (max_ಅಂಕೆ_ಸಾಲುಗಳು < 0)
	max_ಅಂಕೆ_ಸಾಲುಗಳು = get_viminfo_parameter('"');
    ಯದಿ (max_ಅಂಕೆ_ಸಾಲುಗಳು == 0)
	ಪ್ರತ್ಯಾಗಮ;
    max_kbyte = get_viminfo_parameter('s');
    ಯದಿ (max_kbyte == 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_REGISTERS; i++)
    {
#ಯದಿ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	ಯದಿ (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    ಅನುವರ್ತ;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	ಯದಿ (i == TILDE_REGISTER)
	    ಅನುವರ್ತ;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	ಯದಿ (y_read_regs != ರಿಕ್ತ
		&& y_read_regs[i].y_array != ರಿಕ್ತ
		&& (y_regs_p[i].y_array == ರಿಕ್ತ ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	ಅನ್ಯಥಾ ಯದಿ (y_regs_p[i].y_array == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	ಅನ್ಯಥಾ
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	ಅಂಕೆ_ಸಾಲುಗಳು = y_ptr->y_size;
	ಯದಿ (ಅಂಕೆ_ಸಾಲುಗಳು == 0
		|| (ಅಂಕೆ_ಸಾಲುಗಳು == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    ಅನುವರ್ತ;

	ಯದಿ (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    ಪ್ರತಿ (j = 0; j < ಅಂಕೆ_ಸಾಲುಗಳು; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    ಯದಿ (len > (long)max_kbyte * 1024L)
		ಅನುವರ್ತ;
	}

	ವಿಕಲ್ಪ (y_ptr->y_type)
	{
	    ಪಕ್ಷ MLINE:
		type = (char_u *)"LINE";
		ಭಙ್ಗ;
	    ಪಕ್ಷ MCHAR:
		type = (char_u *)"CHAR";
		ಭಙ್ಗ;
	    ಪಕ್ಷ MBLOCK:
		type = (char_u *)"BLOCK";
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		ಭಙ್ಗ;
	}
	ಯದಿ (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	ಯದಿ (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (ಪೂರ್ಣಾಙ್ಕ)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	ಯದಿ (max_ಅಂಕೆ_ಸಾಲುಗಳು > 0 && ಅಂಕೆ_ಸಾಲುಗಳು > max_ಅಂಕೆ_ಸಾಲುಗಳು)
	    ಅಂಕೆ_ಸಾಲುಗಳು = max_ಅಂಕೆ_ಸಾಲುಗಳು;
	ಪ್ರತಿ (j = 0; j < ಅಂಕೆ_ಸಾಲುಗಳು; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    ಪೂರ್ಣಾಙ್ಕ	    flags = 0;
	    ಪೂರ್ಣಾಙ್ಕ	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    ಯದಿ (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    ಯದಿ (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, ಅಂಕೆ_ಸಾಲುಗಳು, (ಪೂರ್ಣಾಙ್ಕ)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    ಪ್ರತಿ (j = 0; j < ಅಂಕೆ_ಸಾಲುಗಳು; j++)
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

ಸ್ಥಿರ xfmark_T *vi_namedfm = ರಿಕ್ತ;
ಸ್ಥಿರ xfmark_T *vi_jumplist = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vi_jumplist_len = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
write_one_mark(FILE *fp_out, ಪೂರ್ಣಾಙ್ಕ c, pos_T *pos)
{
    ಯದಿ (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (ಪೂರ್ಣಾಙ್ಕ)pos->col);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    pos_T	pos;

    home_replace(ರಿಕ್ತ, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_ಸೇರಿಸು);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    ಪ್ರತಿ (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	ಯದಿ (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    ಪ್ರತಿ (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
skip_for_viminfo(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_marks(FILE *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		is_mark_set;
    ಪೂರ್ಣಾಙ್ಕ		i;
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
	ಯದಿ (buf->b_marks_read)
	{
	    ಯದಿ (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    ಅನ್ಯಥಾ
	    {
		is_mark_set = FALSE;
		ಪ್ರತಿ (i = 0; i < NMARKS; i++)
		    ಯದಿ (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			ಭಙ್ಗ;
		    }
	    }
	    ಯದಿ (is_mark_set && buf->b_ffname != ರಿಕ್ತ
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		ಯದಿ (buflist == ರಿಕ್ತ)
		    write_buffer_marks(buf, fp_out);
		ಅನ್ಯಥಾ ಯದಿ (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    ಪೂರ್ಣಾಙ್ಕ		c1,
    ಪೂರ್ಣಾಙ್ಕ		c2)
{
    char_u	*name;

    ಯದಿ (fm->fmark.mark.lnum == 0)	// not set
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    ಅನ್ಯಥಾ
	name = fm->fname;		// use name from .viminfo
    ಯದಿ (name != ರಿಕ್ತ && *name != NUL)
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

    ಯದಿ (fm->fmark.fnum != 0)
	vim_free(name);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_filemarks(FILE *fp)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    ಪೂರ್ಣಾಙ್ಕ		vi_idx;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    ಯದಿ (get_viminfo_parameter('f') == 0)
	ಪ್ರತ್ಯಾಗಮ;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    ಪ್ರತಿ (i = 0; i < NMARKS; i++)
    {
	ಯದಿ (vi_namedfm != ರಿಕ್ತ
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	ಅನ್ಯಥಾ
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    ಯದಿ (curbuf->b_ffname != ರಿಕ್ತ && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	ಪ್ರತಿ (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    ಯದಿ (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == ರಿಕ್ತ
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != ರಿಕ್ತ
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		ಭಙ್ಗ;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	ಪ್ರತಿ ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = ರಿಕ್ತ;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    ಪ್ರತಿ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != ರಿಕ್ತ ? &vi_namedfm[vi_idx] : ರಿಕ್ತ;

	ಯದಿ (vi_fm != ರಿಕ್ತ
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ಅನ್ಯಥಾ
	{
	    fm = &namedfm_p[idx++];
	    ಯದಿ (vi_fm != ರಿಕ್ತ
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != ರಿಕ್ತ
			  && fm->fname != ರಿಕ್ತ
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
    ಪ್ರತಿ (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : ರಿಕ್ತ;
	vi_fm = (vi_jumplist != ರಿಕ್ತ && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : ರಿಕ್ತ;
	ಯದಿ (fm == ರಿಕ್ತ && vi_fm == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ (fm == ರಿಕ್ತ || (vi_fm != ರಿಕ್ತ && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ಅನ್ಯಥಾ
	    --idx;
	ಯದಿ (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != ರಿಕ್ತ
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    ಪೂರ್ಣಾಙ್ಕ
buf_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    ಯದಿ (buf1->b_last_used == buf2->b_last_used)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_out,
    garray_T	*buflist,
    ಪೂರ್ಣಾಙ್ಕ		eof,
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    char_u	*ಸಾಲು = virp->vir_ಸಾಲು;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_marked_files;
    ಪೂರ್ಣಾಙ್ಕ		load_marks;
    ಪೂರ್ಣಾಙ್ಕ		copy_marks_out;
    char_u	*str;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = ರಿಕ್ತ;
#endif
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		buflist_used = 0;
    buf_T	*buflist_buf = ರಿಕ್ತ;

    ಯದಿ ((name_buf = alloc(LSIZE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    *name_buf = NUL;

    ಯದಿ (fp_out != ರಿಕ್ತ && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						ಪರಿಮಾಣ(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    ಯದಿ (fp_out == ರಿಕ್ತ && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	ಯದಿ (list != ರಿಕ್ತ)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    ಅಂಕೆ_marked_files = get_viminfo_parameter('\'');
    ಯಾವತ್ (!eof && (ಎಣಿಕೆ < ಅಂಕೆ_marked_files || fp_out == ರಿಕ್ತ))
    {
	ಯದಿ (ಸಾಲು[0] != '>')
	{
	    ಯದಿ (ಸಾಲು[0] != '\n' && ಸಾಲು[0] != '\r' && ಸಾಲು[0] != '#')
	    {
		ಯದಿ (viminfo_error("E576: ", _(e_nonr_missing_gt), ಸಾಲು))
		    ಭಙ್ಗ;	// too many errors, return now
	    }
	    eof = vim_fgets(ಸಾಲು, LSIZE, virp->vir_fd);
	    ಅನುವರ್ತ;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(ಸಾಲು + 1);
	str = viminfo_readstring(virp, (ಪೂರ್ಣಾಙ್ಕ)(str - virp->vir_ಸಾಲು), FALSE);
	ಯದಿ (str == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	p = str + STRLEN(str);
	ಯಾವತ್ (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	ಯದಿ (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	ಯದಿ (list != ರಿಕ್ತ)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	ಯದಿ (fp_out == ರಿಕ್ತ)
	{
	    ಯದಿ ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != ರಿಕ್ತ)
	    {
		ಯದಿ (*name_buf == NUL)	    // only need to do this once
		    home_replace(ರಿಕ್ತ, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		ಯದಿ (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	ಅನ್ಯಥಾ // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		ಯದಿ (buf->b_ffname != ರಿಕ್ತ)
		{
		    home_replace(ರಿಕ್ತ, buf->b_ffname, name_buf, LSIZE, TRUE);
		    ಯದಿ (fnamecmp(str, name_buf) == 0)
			ಭಙ್ಗ;
		}

	    // Copy marks if the buffer has not been loaded.
	    ಯದಿ (buf == ರಿಕ್ತ || !buf->b_marks_read)
	    {
		ಪೂರ್ಣಾಙ್ಕ	did_read_ಸಾಲು = FALSE;

		ಯದಿ (buflist_buf != ರಿಕ್ತ)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    ಯದಿ (!viminfo_readline(virp) && ಸಾಲು[0] == TAB)
		    {
			did_read_ಸಾಲು = TRUE;
			ಯದಿ (ಸಾಲು[1] == '*')
			{
			    long	ltime;

			    sscanf((ವರ್ಣ *)ಸಾಲು + 2, "%ld ", &ltime);
			    ಯಾವತ್ ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				ಯದಿ (++ಎಣಿಕೆ >= ಅಂಕೆ_marked_files)
				    ಭಙ್ಗ;
				ಯದಿ (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = ರಿಕ್ತ;
				    ಭಙ್ಗ;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			ಅನ್ಯಥಾ
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    ಯಾವತ್ (ಎಣಿಕೆ < ಅಂಕೆ_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++ಎಣಿಕೆ;
			    }
			    buflist_buf = ರಿಕ್ತ;
			}

			ಯದಿ (ಎಣಿಕೆ >= ಅಂಕೆ_marked_files)
			{
			    vim_free(str);
			    ಭಙ್ಗ;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		ಯದಿ (did_read_ಸಾಲು)
		    fputs((ವರ್ಣ *)ಸಾಲು, fp_out);

		ಎಣಿಕೆ++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	ಯಾವತ್ (!(eof = viminfo_readline(virp)) && ಸಾಲು[0] == TAB)
	{
	    ಯದಿ (load_marks)
	    {
		ಯದಿ (ಸಾಲು[1] != NUL)
		{
		    unsigned u;

		    sscanf((ವರ್ಣ *)ಸಾಲು + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    ವಿಕಲ್ಪ (ಸಾಲು[1])
		    {
			ಪಕ್ಷ '"': curbuf->b_last_cursor = pos; ಭಙ್ಗ;
			ಪಕ್ಷ '^': curbuf->b_last_ಸೇರಿಸು = pos; ಭಙ್ಗ;
			ಪಕ್ಷ '.': curbuf->b_last_change = pos; ಭಙ್ಗ;
			ಪಕ್ಷ '+':
				  // changelist positions are stored oldest
				  // first
				  ಯದಿ (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    ಪರಿಮಾಣ(pos_T) * (JUMPLISTSIZE - 1));
				  ಅನ್ಯಥಾ
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  ಭಙ್ಗ;

				  // Using the line number for the last-used
				  // timestamp.
			ಪಕ್ಷ '*': curbuf->b_last_used = pos.lnum; ಭಙ್ಗ;

			ಸ್ವಭಾವ:  ಯದಿ ((i = ಸಾಲು[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (copy_marks_out)
		fputs((ವರ್ಣ *)ಸಾಲು, fp_out);
	}

	ಯದಿ (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		ಯದಿ (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    ಯದಿ (flags & VIF_ONLY_CURBUF)
		ಭಙ್ಗ;
	}
    }

    ಯದಿ (fp_out != ರಿಕ್ತ)
	// Write any remaining entries from buflist.
	ಯಾವತ್ (ಎಣಿಕೆ < ಅಂಕೆ_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++ಎಣಿಕೆ;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    ಶೂನ್ಯ
check_marks_read(ಶೂನ್ಯ)
{
    ಯದಿ (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != ರಿಕ್ತ)
	read_viminfo(ರಿಕ್ತ, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_viminfo_filemark(vir_T *virp, ಪೂರ್ಣಾಙ್ಕ force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    ಪೂರ್ಣಾಙ್ಕ		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_ಸಾಲು + 1;
    ಯದಿ (*str <= 127
	    && ((*virp->vir_ಸಾಲು == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_ಸಾಲು == '-' && *str == '\'')))
    {
	ಯದಿ (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    ಯದಿ (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		ಪ್ರತಿ (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = ರಿಕ್ತ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	ಅನ್ಯಥಾ
	    fm = &namedfm_p[*str - 'A'];
	ಯದಿ (fm != ರಿಕ್ತ && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (ಪೂರ್ಣಾಙ್ಕ)(str - virp->vir_ಸಾಲು),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    ಪ್ರತ್ಯಾಗಮ vim_fgets(virp->vir_ಸಾಲು, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_viminfo_marks(ಶೂನ್ಯ)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
finish_viminfo_marks(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (vi_namedfm != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    ಯದಿ (vi_jumplist != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_viminfo_mark(garray_T *ಮೌಲ್ಯಗಳು, ಪೂರ್ಣಾಙ್ಕ force)
{
    bval_T	*vp = (bval_T *)ಮೌಲ್ಯಗಳು->ga_data;
    ಪೂರ್ಣಾಙ್ಕ		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = ರಿಕ್ತ;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    ಯದಿ (ಮೌಲ್ಯಗಳು->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	ಪ್ರತ್ಯಾಗಮ;

    name = vp[0].bv_nr;
    ಯದಿ (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	ಪ್ರತ್ಯಾಗಮ;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    ಯದಿ (lnum <= 0 || col < 0)
	ಪ್ರತ್ಯಾಗಮ;
    timestamp = (time_t)vp[3].bv_nr;

    ಯದಿ (name == '\'')
    {
	ಯದಿ (vi_jumplist != ರಿಕ್ತ)
	{
	    ಯದಿ (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ idx;
	    ಪೂರ್ಣಾಙ್ಕ i;

	    // If we have a timestamp insert it in the right place.
	    ಯದಿ (timestamp != 0)
	    {
		ಪ್ರತಿ (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    ಯದಿ (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			ಭಙ್ಗ;
		    }
		// idx cannot be zero now
		ಯದಿ (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    ಅನ್ಯಥಾ
		idx = -1;

	    ಯದಿ (idx >= 0)
	    {
		ಯದಿ (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    ಪ್ರತಿ (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		ಅನ್ಯಥಾ
		{
		    // Move newer entries forward.
		    ಪ್ರತಿ (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = ರಿಕ್ತ;
		fm->time_set = 0;
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	ಯದಿ (VIM_ISDIGIT(name))
	{
	    ಯದಿ (vi_namedfm != ರಿಕ್ತ)
		idx = name - '0' + NMARKS;
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		ಪ್ರತಿ (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    ಯದಿ (namedfm_p[idx].time_set < timestamp)
			ಭಙ್ಗ;
		ಯದಿ (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    ಪ್ರತ್ಯಾಗಮ;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		ಪ್ರತಿ ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = ರಿಕ್ತ;
	    }
	}
	ಅನ್ಯಥಾ
	    idx = name - 'A';
	ಯದಿ (vi_namedfm != ರಿಕ್ತ)
	    fm = &vi_namedfm[idx];
	ಅನ್ಯಥಾ
	    fm = &namedfm_p[idx];
    }

    ಯದಿ (fm != ರಿಕ್ತ)
    {
	ಯದಿ (vi_namedfm != ರಿಕ್ತ || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    ಯದಿ (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_viminfo_barline(vir_T *virp, ಪೂರ್ಣಾಙ್ಕ got_encoding, ಪೂರ್ಣಾಙ್ಕ force, ಪೂರ್ಣಾಙ್ಕ writing)
{
    char_u	*p = virp->vir_ಸಾಲು + 1;
    ಪೂರ್ಣಾಙ್ಕ		bartype;
    garray_T	ಮೌಲ್ಯಗಳು;
    bval_T	*vp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    ಯದಿ (*p == '<')
    {
	// Continuation line of an unrecognized item.
	ಯದಿ (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_ಸಾಲು);
    }
    ಅನ್ಯಥಾ
    {
	ga_init2(&ಮೌಲ್ಯಗಳು, ಪರಿಮಾಣ(bval_T), 20);
	bartype = getdigits(&p);
	ವಿಕಲ್ಪ (bartype)
	{
	    ಪಕ್ಷ BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		ಯದಿ (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &ಮೌಲ್ಯಗಳು);
		    vp = (bval_T *)ಮೌಲ್ಯಗಳು.ga_data;
		    ಯದಿ (ಮೌಲ್ಯಗಳು.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &ಮೌಲ್ಯಗಳು);
		handle_viminfo_history(&ಮೌಲ್ಯಗಳು, writing);
		ಭಙ್ಗ;

	    ಪಕ್ಷ BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &ಮೌಲ್ಯಗಳು);
		handle_viminfo_register(&ಮೌಲ್ಯಗಳು, force);
		ಭಙ್ಗ;

	    ಪಕ್ಷ BARTYPE_MARK:
		read_next = barline_parse(virp, p, &ಮೌಲ್ಯಗಳು);
		handle_viminfo_mark(&ಮೌಲ್ಯಗಳು, force);
		ಭಙ್ಗ;

	    ಸ್ವಭಾವ:
		// copy unrecognized line (for future use)
		ಯದಿ (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_ಸಾಲು);
	}
	ಪ್ರತಿ (i = 0; i < ಮೌಲ್ಯಗಳು.ga_len; ++i)
	{
	    vp = (bval_T *)ಮೌಲ್ಯಗಳು.ga_data + i;
	    ಯದಿ (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&ಮೌಲ್ಯಗಳು);
    }

    ಯದಿ (read_next)
	ಪ್ರತ್ಯಾಗಮ viminfo_readline(virp);
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_viminfo_up_to_marks(
    vir_T	*virp,
    ಪೂರ್ಣಾಙ್ಕ		forceit,
    ಪೂರ್ಣಾಙ್ಕ		writing)
{
    ಪೂರ್ಣಾಙ್ಕ		eof;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    ಯಾವತ್ (!eof && virp->vir_ಸಾಲು[0] != '>')
    {
	ವಿಕಲ್ಪ (virp->vir_ಸಾಲು[0])
	{
		// Characters reserved for future expansion, ignored now
	    ಪಕ್ಷ '+': // "+40 /path/dir file", for running vim without args
	    ಪಕ್ಷ '^': // to be defined
	    ಪಕ್ಷ '<': // long line - ignored
		// A comment or empty line.
	    ಪಕ್ಷ NUL:
	    ಪಕ್ಷ '\r':
	    ಪಕ್ಷ '\n':
	    ಪಕ್ಷ '#':
		eof = viminfo_readline(virp);
		ಭಙ್ಗ;
	    ಪಕ್ಷ '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		ಭಙ್ಗ;
	    ಪಕ್ಷ '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		ಭಙ್ಗ;
	    ಪಕ್ಷ '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#ಅನ್ಯಥಾ
		eof = viminfo_readline(virp);
#endif
		ಭಙ್ಗ;
	    ಪಕ್ಷ '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		ಭಙ್ಗ;
	    ಪಕ್ಷ '"':
		// When registers are in bar lines skip the old style register
		// lines.
		ಯದಿ (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		ಅನ್ಯಥಾ
		    ಕುರು
		    {
			eof = viminfo_readline(virp);
		    } ಯಾವತ್ (!eof && (virp->vir_ಸಾಲು[0] == TAB
						|| virp->vir_ಸಾಲು[0] == '<'));
		ಭಙ್ಗ;
	    ಪಕ್ಷ '/':	    // Search string
	    ಪಕ್ಷ '&':	    // Substitute search string
	    ಪಕ್ಷ '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_ಹುಡುಕು_pattern(virp, forceit, writing);
		ಭಙ್ಗ;
	    ಪಕ್ಷ '$':
		eof = read_viminfo_sub_string(virp, forceit);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ':':
	    ಪಕ್ಷ '?':
	    ಪಕ್ಷ '=':
	    ಪಕ್ಷ '@':
		// When history is in bar lines skip the old style history
		// lines.
		ಯದಿ (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		ಅನ್ಯಥಾ
		    eof = viminfo_readline(virp);
		ಭಙ್ಗ;
	    ಪಕ್ಷ '-':
	    ಪಕ್ಷ '\'':
		// When file marks are in bar lines skip the old style lines.
		ಯದಿ (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		ಅನ್ಯಥಾ
		    eof = viminfo_readline(virp);
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		ಯದಿ (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_ಸಾಲು))
		    eof = TRUE;
		ಅನ್ಯಥಾ
		    eof = viminfo_readline(virp);
		ಭಙ್ಗ;
	}
    }

    // Finish reading history items.
    ಯದಿ (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    ಪ್ರತ್ಯಾಗಮ eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_viminfo(FILE *fp_in, FILE *fp_out, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ		eof = FALSE;
    vir_T	vir;
    ಪೂರ್ಣಾಙ್ಕ		merge = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		do_copy_marks = FALSE;
    garray_T	buflist;

    ಯದಿ ((vir.vir_ಸಾಲು = alloc(LSIZE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, ಪರಿಮಾಣ(char_u *), 100);
    vir.vir_version = -1;

    ಯದಿ (fp_in != ರಿಕ್ತ)
    {
	ಯದಿ (flags & VIF_WANT_INFO)
	{
	    ಯದಿ (fp_out != ರಿಕ್ತ)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != ರಿಕ್ತ);
	    merge = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (flags != 0)
	    // Skip info, find start of marks
	    ಯಾವತ್ (!(eof = viminfo_readline(&vir))
		    && vir.vir_ಸಾಲು[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    ಯದಿ (fp_out != ರಿಕ್ತ)
    {
	// Write the info:
	fprintf(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_out);
	fprintf(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_ಹುಡುಕು_pattern(fp_out);
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

	ಯದಿ (do_copy_marks)
	    ga_init2(&buflist, ಪರಿಮಾಣ(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : ರಿಕ್ತ);
    }

    ಯದಿ (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	ಯದಿ (fp_out != ರಿಕ್ತ)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_ಸಾಲು);
    ಯದಿ (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, ರಿಕ್ತ, ರಿಕ್ತ);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    ಪೂರ್ಣಾಙ್ಕ
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    ಪೂರ್ಣಾಙ್ಕ		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    ಯದಿ (no_viminfo())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    fp = mch_fopen((ವರ್ಣ *)fname, READBIN);

    ಯದಿ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == ರಿಕ್ತ ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    ಯದಿ (fp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, ರಿಕ್ತ, flags);

    fclose(fp);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    ಶೂನ್ಯ
write_viminfo(char_u *file, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    char_u	*fname;
    FILE	*fp_in = ರಿಕ್ತ;	// input viminfo file, if any
    FILE	*fp_out = ರಿಕ್ತ;	// output viminfo file
    char_u	*tempname = ರಿಕ್ತ;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#ಯದಿ defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    ಪೂರ್ಣಾಙ್ಕ		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    ಪೂರ್ಣಾಙ್ಕ		hidden = FALSE;
#endif

    ಯದಿ (no_viminfo())
	ಪ್ರತ್ಯಾಗಮ;

    fname = viminfo_filename(file);	// may set to default if NULL
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    fp_in = mch_fopen((ವರ್ಣ *)fname, READBIN);
    ಯದಿ (fp_in == ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ fd;

	// if it does exist, but we can't read it, don't try writing
	ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st_new) == 0)
	    ಗಚ್ಛ end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((ವರ್ಣ *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	ಯದಿ (fd < 0)
	    ಗಚ್ಛ end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    ಅನ್ಯಥಾ
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	ಯದಿ (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != ಬೇರು_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    ಪೂರ್ಣಾಙ್ಕ	tt = ಸಂದೇಶ_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    ಸಂದೇಶ_didany = tt;
	    fclose(fp_in);
	    ಗಚ್ಛ end;
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
	ಪ್ರತಿ (;;)
	{
	    ಪೂರ್ಣಾಙ್ಕ		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#ಅನ್ಯಥಾ
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#ಅನ್ಯಥಾ
				    (char_u *)".tmp",
#endif
				    FALSE);
	    ಯದಿ (tempname == ರಿಕ್ತ)		// out of memory
		ಭಙ್ಗ;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    ಯದಿ (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    ಪ್ರತಿ (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		ಯದಿ (mch_stat((ವರ್ಣ *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    ಯದಿ (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			ಭಙ್ಗ;
		    }
#endif
		}
		ಅನ್ಯಥಾ
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((ವರ್ಣ *)tempname, WRITEBIN);
		    (ಶೂನ್ಯ)umask(umask_save);
#ಅನ್ಯಥಾ
		    ಪೂರ್ಣಾಙ್ಕ	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((ವರ್ಣ *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(ಪೂರ್ಣಾಙ್ಕ)((st_old.st_mode & 0777) | 0600));
		    (ಶೂನ್ಯ)umask(umask_save);
# ಅನ್ಯಥಾ
		    fd = mch_open((ವರ್ಣ *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    ಯದಿ (fd < 0)
		    {
			fp_out = ರಿಕ್ತ;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			ಯದಿ (errno != EEXIST)
			    ಭಙ್ಗ;
# endif
		    }
		    ಅನ್ಯಥಾ
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    ಯದಿ (fp_out != ರಿಕ್ತ)
			ಭಙ್ಗ;
		}

		// Assume file exists, try again with another name.
		ಯದಿ (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    ಭಙ್ಗ;
		}
		*wp = next_char;
		--next_char;
	    }

	    ಯದಿ (tempname != ರಿಕ್ತ)
		ಭಙ್ಗ;
	    // continue if shortname was set
	}

#ಯದಿ defined(UNIX) && defined(HAVE_FCHOWN)
	ಯದಿ (tempname != ರಿಕ್ತ && fp_out != ರಿಕ್ತ)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    ಯದಿ (mch_stat((ವರ್ಣ *)tempname, &tmp_st) >= 0)
	    {
		ಯದಿ (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		ಯದಿ (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (ಶೂನ್ಯ)mch_setperm(tempname, 0600);
	    }
	    ಅನ್ಯಥಾ
		// can't stat the file, set conservative permissions
		(ಶೂನ್ಯ)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    ಯದಿ (fp_out == ರಿಕ್ತ)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == ರಿಕ್ತ || tempname == ರಿಕ್ತ) ? fname : tempname);
	ಯದಿ (fp_in != ರಿಕ್ತ)
	    fclose(fp_in);
	ಗಚ್ಛ end;
    }

    ಯದಿ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    ಯದಿ (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    ಯದಿ (fp_in != ರಿಕ್ತ)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	ಯದಿ (viminfo_errcnt == 0)
	{
	    ಯದಿ (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    ಅನ್ಯಥಾ ಯದಿ (hidden)
		mch_hide(fname);
#endif
	}
	ಯದಿ (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    ಶೂನ್ಯ
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    ಯದಿ (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    ಯದಿ (eap->cmdidx == CMD_rviminfo)
    {
	ಯದಿ (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    ಅನ್ಯಥಾ
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
