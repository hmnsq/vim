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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*vir_ಸಾಲು;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    ಪೂರ್ಣಾಂಕ		vir_ಆವೃತ್ತಿ;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    BVAL_NR,
    BVAL_STRING,
    BVAL_ಖಾಲಿಗಳು
} btype_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    btype_T	bv_type;
    ದೀರ್ಘ	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    ಪೂರ್ಣಾಂಕ		bv_len;		// length of bv_string
    ಪೂರ್ಣಾಂಕ		bv_allocated;	// bv_string was allocated
} bval_T;

#ಆದರೆ defined(FEAT_VIMINFO)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_viminfo_parameter(ಪೂರ್ಣಾಂಕ type)
{
    char_u  *p;

    ಗಾಗಿ (p = p_viminfo; *p; ++p)
    {
	ಆದರೆ (*p == type)
	    ಹಿಂತಿರುಗಿ p + 1;
	ಆದರೆ (*p == 'n')		    // 'n' is always the last one
	    ನಿಲ್ಲಿಸು;
	p = vim_strchr(p, ',');	    // skip until next ','
	ಆದರೆ (p == ಖಾಲಿ)		    // hit the end without finding parameter
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    ಪೂರ್ಣಾಂಕ
get_viminfo_parameter(ಪೂರ್ಣಾಂಕ type)
{
    char_u  *p;

    p = ಕಂಡುಹಿಡಿ_viminfo_parameter(type);
    ಆದರೆ (p != ಖಾಲಿ && VIM_ISDIGIT(*p))
	ಹಿಂತಿರುಗಿ atoi((ಅಕ್ಷರ *)p);
    ಹಿಂತಿರುಗಿ -1;
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
    ಆದರೆ (file == ಖಾಲಿ || *file == NUL)
    {
	size_t  len;

	ಆದರೆ (*p_viminfofile != NUL)
	    file = p_viminfofile;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((file = ಕಂಡುಹಿಡಿ_viminfo_parameter('n')) == ಖಾಲಿ || *file == NUL)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMINFO_FILE2
# ifdef VMS
	    ಆದರೆ (mch_getenv((char_u *)"SYS$LOGIN") == ಖಾಲಿ)
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    ಆದರೆ (STRCMP(vim_getenv((char_u *)"HOME", ಖಾಲಿ), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == ಖಾಲಿ)
#  ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (mch_getenv((char_u *)"HOME") == ಖಾಲಿ)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		ಆದರೆ (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		ಇಲ್ಲದಿದ್ದರೆ
		    file = (char_u *)VIMINFO_FILE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	ಹಿಂತಿರುಗಿ vim_strnsave(file, len);
    }

    ಹಿಂತಿರುಗಿ vim_strsave(file);
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
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		len = 0;

    ಗಾಗಿ (s = p; *s != NUL; ++s)
    {
	ಆದರೆ (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    ಆದರೆ (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    ಆಗ ((c = *p++) != NUL)
    {
	ಆದರೆ (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    ಆದರೆ (c == '\n')
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
barline_writestring(FILE *fd, char_u *s, ಪೂರ್ಣಾಂಕ ಉಳಿದ_ಆರಂಭ)
{
    char_u *p;
    ಪೂರ್ಣಾಂಕ	    ಉಳಿದ = ಉಳಿದ_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ	    len = 2;

    // Count the number of characters produced, including quotes.
    ಗಾಗಿ (p = s; *p != NUL; ++p)
    {
	ಆದರೆ (*p == NL)
	    len += 2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '"' || *p == '\\')
	    len += 2;
	ಇಲ್ಲದಿದ್ದರೆ
	    ++len;
    }
    ಆದರೆ (len > ಉಳಿದ - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	ಉಳಿದ = LSIZE - 20;
    }

    putc('"', fd);
    ಗಾಗಿ (p = s; *p != NUL; ++p)
    {
	ಆದರೆ (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --ಉಳಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --ಉಳಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    putc(*p, fd);
	--ಉಳಿದ;

	ಆದರೆ (ಉಳಿದ < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    ಉಳಿದ = LSIZE - 20;
	}
    }
    putc('"', fd);
    ಹಿಂತಿರುಗಿ ಉಳಿದ - 2;
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
    ಪೂರ್ಣಾಂಕ		off,		    // offset for virp->vir_line
    ಪೂರ್ಣಾಂಕ		convert)	    // convert the string
{
    char_u	*retval = ಖಾಲಿ;
    char_u	*s, *d;
    ದೀರ್ಘ	len;

    ಆದರೆ (virp->vir_ಸಾಲು[off] == Ctrl_V && vim_isdigit(virp->vir_ಸಾಲು[off + 1]))
    {
	len = atol((ಅಕ್ಷರ *)virp->vir_ಸಾಲು + off + 1);
	ಆದರೆ (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	ಆದರೆ (retval == ಖಾಲಿ)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (ಶೂನ್ಯ)vim_fgets(virp->vir_ಸಾಲು, 10, virp->vir_fd);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	(ಶೂನ್ಯ)vim_fgets(retval, (ಪೂರ್ಣಾಂಕ)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	retval = vim_strsave(virp->vir_ಸಾಲು + off);
	ಆದರೆ (retval == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    ಆಗ (*s != NUL && *s != '\n')
    {
	ಆದರೆ (s[0] == Ctrl_V && s[1] != NUL)
	{
	    ಆದರೆ (s[1] == 'n')
		*d++ = '\n';
	    ಇಲ್ಲದಿದ್ದರೆ
		*d++ = Ctrl_V;
	    s += 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *d++ = *s++;
    }
    *d = NUL;

    ಆದರೆ (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, ಖಾಲಿ);
	ಆದರೆ (d != ಖಾಲಿ)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
viminfo_readline(vir_T *virp)
{
    ಹಿಂತಿರುಗಿ vim_fgets(virp->vir_ಸಾಲು, LSIZE, virp->vir_fd);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_viminfo_bufferlist(
    vir_T	*virp,
    ಪೂರ್ಣಾಂಕ		writing)
{
    char_u	*tab;
    linenr_T	lnum;
    colnr_T	ಲಂಬಸಾಲು;
    buf_T	*buf;
    char_u	*sfname;
    char_u	*xline;

    // Handle long line and escaped characters.
    xline = viminfo_readstring(virp, 1, FALSE);

    // don't read in if there are files on the command-line or if writing:
    ಆದರೆ (xline != ಖಾಲಿ && !writing && ARGCOUNT == 0
				       && ಕಂಡುಹಿಡಿ_viminfo_parameter('%') != ಖಾಲಿ)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	ಲಂಬಸಾಲು = 0;
	tab = vim_strrchr(xline, '\t');
	ಆದರೆ (tab != ಖಾಲಿ)
	{
	    *tab++ = '\0';
	    ಲಂಬಸಾಲು = (colnr_T)atoi((ಅಕ್ಷರ *)tab);
	    tab = vim_strrchr(xline, '\t');
	    ಆದರೆ (tab != ಖಾಲಿ)
	    {
		*tab++ = '\0';
		lnum = atol((ಅಕ್ಷರ *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	ಆದರೆ (buf != ಖಾಲಿ)	// just in case...
	{
	    buf->b_ಕೊನೆಯ_cursor.lnum = lnum;
	    buf->b_ಕೊನೆಯ_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	    buflist_setfpos(buf, curwin, lnum, ಲಂಬಸಾಲು, FALSE);
	}
    }
    vim_free(xline);

    ಹಿಂತಿರುಗಿ viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    ಪೂರ್ಣಾಂಕ	    part_len;
    ಪೂರ್ಣಾಂಕ	    retval = FALSE;

    name = home_replace_save(ಖಾಲಿ, name);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, ಗಾತ್ರ(part), ", ");
	ಆದರೆ (part[0] == 'r')
	{
	    ಆದರೆ (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }
    vim_free(name);
    ಹಿಂತಿರುಗಿ retval;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		max_buffers;

    ಆದರೆ (ಕಂಡುಹಿಡಿ_viminfo_parameter('%') == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಾಲು_BUF_LEN (MAXPATHL + 40)
    ಸಾಲು = alloc(ಸಾಲು_BUF_LEN);
    ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_TAB_WINDOWS(tp, win)
	ಗಣ_ಕೊನೆಯ_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	ಆದರೆ (buf->b_fname == ಖಾಲಿ
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    ಮುಂದುವರಿ;

	ಆದರೆ (max_buffers-- == 0)
	    ನಿಲ್ಲಿಸು;
	putc('%', fp);
	linelen = home_replace(ಖಾಲಿ, buf->b_ffname, ಸಾಲು, MAXPATHL, TRUE);
	vim_snprintf((ಅಕ್ಷರ *)ಸಾಲು + linelen, ಸಾಲು_BUF_LEN - linelen, "\t%ld\t%d",
			(ದೀರ್ಘ)buf->b_ಕೊನೆಯ_cursor.lnum,
			buf->b_ಕೊನೆಯ_cursor.ಲಂಬಸಾಲು);
	viminfo_writestring(fp, ಸಾಲು);
    }
    vim_free(ಸಾಲು);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
ಸ್ಥಿರ histentry_T *viminfo_history[HIST_ಎಣಿಕೆ] =
					       {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	viminfo_hisidx[HIST_ಎಣಿಕೆ] = {0, 0, 0, 0, 0};
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	viminfo_hislen[HIST_ಎಣಿಕೆ] = {0, 0, 0, 0, 0};
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	viminfo_ಕೂಡಿಸು_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
hist_type2char(
    ಪೂರ್ಣಾಂಕ	    type,
    ಪೂರ್ಣಾಂಕ	    use_question)	    // use '?' instead of '/'
{
    ಆದರೆ (type == HIST_CMD)
	ಹಿಂತಿರುಗಿ ':';
    ಆದರೆ (type == HIST_ಹುಡುಕು)
    {
	ಆದರೆ (use_question)
	    ಹಿಂತಿರುಗಿ '?';
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ '/';
    }
    ಆದರೆ (type == HIST_ಅಭಿವ್ಯಕ್ತಿ)
	ಹಿಂತಿರುಗಿ '=';
    ಹಿಂತಿರುಗಿ '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_viminfo_history(ಪೂರ್ಣಾಂಕ asklen, ಪೂರ್ಣಾಂಕ writing)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ;
    ಪೂರ್ಣಾಂಕ	    type;
    ಪೂರ್ಣಾಂಕ	    len;
    ಪೂರ್ಣಾಂಕ	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_ಕೂಡಿಸು_at_front = (asklen != 0 && !writing);
    ಆದರೆ (asklen > hislen)
	asklen = hislen;

    ಗಾಗಿ (type = 0; type < HIST_ಎಣಿಕೆ; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	ಗಾಗಿ (i = 0, ಅಂಕೆ = 0; i < hislen; i++)
	    ಆದರೆ (histentry[i].hisstr == ಖಾಲಿ || histentry[i].viminfo)
		ಅಂಕೆ++;
	len = asklen;
	ಆದರೆ (ಅಂಕೆ > len)
	    len = ಅಂಕೆ;
	ಆದರೆ (len <= 0)
	    viminfo_history[type] = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	ಆದರೆ (viminfo_history[type] == ಖಾಲಿ)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_viminfo_history(vir_T *virp, ಪೂರ್ಣಾಂಕ writing)
{
    ಪೂರ್ಣಾಂಕ		type;
    long_u	len;
    char_u	*val = ಖಾಲಿ;
    char_u	*p;

    type = hist_char2type(virp->vir_ಸಾಲು[0]);
    ಆದರೆ (viminfo_hisidx[type] >= viminfo_hislen[type])
	ಹೋಗು ಮುಗಿದ;

    val = viminfo_readstring(virp, 1, TRUE);
    ಆದರೆ (val == ಖಾಲಿ || *val == NUL)
	ಹೋಗು ಮುಗಿದ;

    ಪೂರ್ಣಾಂಕ sep = (*val == ' ' ? NUL : *val);

    ಆದರೆ (ಒಳ_history(type, val + (type == HIST_ಹುಡುಕು), viminfo_ಕೂಡಿಸು_at_front,
								sep, writing))
	ಹೋಗು ಮುಗಿದ;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    ಆದರೆ (type == HIST_ಹುಡುಕು)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹೋಗು ಮುಗಿದ;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹೋಗು ಮುಗಿದ;

	// Not a search entry: No separator in the viminfo
	// file, add a NUL separator.
	mch_memmove(p, val, (size_t)len + 1);	// +1 to include the NUL
	p[len + 1] = NUL;			// put the separator *after* the string's NUL
    }
    viminfo_history[type][viminfo_hisidx[type]].hisstr = p;
    viminfo_history[type][viminfo_hisidx[type]].hisstrlen = (size_t)len;
    viminfo_history[type][viminfo_hisidx[type]].ಸಮಯ_ಗಣ = 0;
    viminfo_history[type][viminfo_hisidx[type]].viminfo = TRUE;
    viminfo_history[type][viminfo_hisidx[type]].hisnum = 0;
    viminfo_hisidx[type]++;

ಮುಗಿದ:
    vim_free(val);
    ಹಿಂತಿರುಗಿ viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_viminfo_history(
	garray_T    *ಮೌಲ್ಯಗಳು,
	ಪೂರ್ಣಾಂಕ	    writing)
{
    ಪೂರ್ಣಾಂಕ		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)ಮೌಲ್ಯಗಳು->ga_ದತ್ತಾಂಶ;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    ಆದರೆ (ಮೌಲ್ಯಗಳು->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_ಖಾಲಿಗಳು)
	    || vp[3].bv_type != BVAL_STRING)
	ಹಿಂತಿರುಗಿ;

    type = vp[0].bv_nr;
    ಆದರೆ (type >= HIST_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (viminfo_hisidx[type] >= viminfo_hislen[type])
	ಹಿಂತಿರುಗಿ;

    val = vp[3].bv_string;
    ಆದರೆ (val == ಖಾಲಿ || *val == NUL)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ sep = type == HIST_ಹುಡುಕು && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    ಪೂರ್ಣಾಂಕ idx;
    ಪೂರ್ಣಾಂಕ overwrite = FALSE;

    ಆದರೆ (ಒಳ_history(type, val, viminfo_ಕೂಡಿಸು_at_front, sep, writing))
	ಹಿಂತಿರುಗಿ;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    ಗಾಗಿ (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	ಆದರೆ (STRCMP(val, p) == 0
		&& (type != HIST_ಹುಡುಕು || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	len = 0; // for picky compilers
    ಆದರೆ (p != ಖಾಲಿ)
    {
	viminfo_history[type][idx].ಸಮಯ_ಗಣ = vp[1].bv_nr;
	ಆದರೆ (!overwrite)
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
concat_history(ಪೂರ್ಣಾಂಕ type)
{
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    ಪೂರ್ಣಾಂಕ		*hisidx = get_hisidx(type);
    ಪೂರ್ಣಾಂಕ		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    ಆದರೆ (idx >= hislen)
	idx -= hislen;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx < 0)
	idx = hislen - 1;
    ಆದರೆ (viminfo_ಕೂಡಿಸು_at_front)
	*hisidx = idx;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (*hisidx == -1)
	    *hisidx = hislen - 1;
	ಮಾಡು
	{
	    ಆದರೆ (histentry[idx].hisstr != ಖಾಲಿ || histentry[idx].viminfo)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (++idx == hislen)
		idx = 0;
	} ಆಗ (idx != *hisidx);
	ಆದರೆ (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    ಗಾಗಿ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].ಸಮಯ_ಗಣ = viminfo_history[type][i].ಸಮಯ_ಗಣ;
	ಆದರೆ (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    ಗಾಗಿ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ವಿಂಗಡಿಸು_hist(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    ಆದರೆ (p1->ಸಮಯ_ಗಣ < p2->ಸಮಯ_ಗಣ) ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (p1->ಸಮಯ_ಗಣ > p2->ಸಮಯ_ಗಣ) ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ವಿಲೀನ_history(ಪೂರ್ಣಾಂಕ type)
{
    ಪೂರ್ಣಾಂಕ		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    ಪೂರ್ಣಾಂಕ		*hisidx = get_hisidx(type);
    ಪೂರ್ಣಾಂಕ		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    ಆದರೆ (tot_hist == ಖಾಲಿ || new_hist == ಖಾಲಿ)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    ಗಾಗಿ (i = 0; i < hislen; i++)
	ಆದರೆ (histentry[i].hisstr != ಖಾಲಿ)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((ಶೂನ್ಯ *)tot_hist, (size_t)len, ಗಾತ್ರ(histentry_T *), ವಿಂಗಡಿಸು_hist);

    // Keep the newest ones.
    ಗಾಗಿ (i = 0; i < hislen; i++)
    {
	ಆದರೆ (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = ಖಾಲಿ;
	    tot_hist[i]->hisstrlen = 0;
	    ಆದರೆ (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    clear_hist_ನಮೂದು(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    ಗಾಗಿ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    ಗಾಗಿ (i = 0; i < hislen; i++)
    {
	vim_free(histentry[i].hisstr);
	histentry[i].hisstrlen = 0;
    }
    vim_free(histentry);
    ಗಣ_histentry(type, new_hist);
    vim_free(tot_hist);
}

/*
 * Finish reading history lines from viminfo.  Not used when writing viminfo.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಮುಕ್ತಾಯ_viminfo_history(vir_T *virp)
{
    ಪೂರ್ಣಾಂಕ	type;
    ಪೂರ್ಣಾಂಕ ವಿಲೀನ = virp->vir_ಆವೃತ್ತಿ >= VIMINFO_ಆವೃತ್ತಿ_WITH_HISTORY;

    ಗಾಗಿ (type = 0; type < HIST_ಎಣಿಕೆ; ++type)
    {
	ಆದರೆ (get_histentry(type) == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	ಆದರೆ (ವಿಲೀನ)
	    ವಿಲೀನ_history(type);
	ಇಲ್ಲದಿದ್ದರೆ
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
write_viminfo_history(FILE *fp, ಪೂರ್ಣಾಂಕ ವಿಲೀನ)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    type;
    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ_saved;
    ಪೂರ್ಣಾಂಕ     ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ	    hislen;

    init_history();
    hislen = get_hislen();
    ಆದರೆ (hislen == 0)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (type = 0; type < HIST_ಎಣಿಕೆ; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	ಪೂರ್ಣಾಂಕ	    *hisidx = get_hisidx(type);

	ಅಂಕೆ_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	ಆದರೆ (ಅಂಕೆ_saved == 0)
	    ಮುಂದುವರಿ;
	ಆದರೆ (ಅಂಕೆ_saved < 0)  // Use default
	    ಅಂಕೆ_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_ಹುಡುಕು ? _("Search String") :
			    type == HIST_ಅಭಿವ್ಯಕ್ತಿ ? _("Expression") :
			    type == HIST_ಒಳಹರಿವು ? _("Input Line") :
					_("Debug Line"));
	ಆದರೆ (ಅಂಕೆ_saved > hislen)
	    ಅಂಕೆ_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
	{
	    ಆದರೆ (ಸುತ್ತು == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		// empty list
		i = -1;
	    ಆದರೆ (i >= 0)
		ಆಗ (ಅಂಕೆ_saved > 0
			&& !(ಸುತ್ತು == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    ಪೂರ್ಣಾಂಕ	    c = NUL;

		    ಆದರೆ (ಸುತ್ತು == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].ಸಮಯ_ಗಣ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (viminfo_history[type] == ಖಾಲಿ)
			{
			    p = ಖಾಲಿ;
			    plen = 0;
			    timestamp = 0;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].ಸಮಯ_ಗಣ;
			}
		    }

		    ಆದರೆ (p != ಖಾಲಿ && (ಸುತ್ತು == 2
				       || !ವಿಲೀನ
				       || !histentry[i].viminfo))
		    {
			--ಅಂಕೆ_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			ಆದರೆ (type == HIST_ಹುಡುಕು)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    ಅಕ್ಷರ    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    ಆದರೆ (c == NUL)
				cbuf[0] = NUL;
			    ಇಲ್ಲದಿದ್ದರೆ
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (ದೀರ್ಘ)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    ಆದರೆ (ಸುತ್ತು == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			ಆದರೆ (--i < 0)
			    i = hislen - 1;
			ಆದರೆ (i == *hisidx)
			    ನಿಲ್ಲಿಸು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	ಗಾಗಿ (i = 0; i < viminfo_hisidx[type]; ++i)
	    ಆದರೆ (viminfo_history[type] != ಖಾಲಿ)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_barlines(vir_T *virp, FILE *fp_ಹೊರಹೋಗು)
{
    ಪೂರ್ಣಾಂಕ		i;
    garray_T	*gap = &virp->vir_barlines;
    ಪೂರ್ಣಾಂಕ		ನೋಡಿದ_useful = FALSE;
    ಅಕ್ಷರ	*ಸಾಲು;

    ಆದರೆ (gap->ga_len <= 0)
	ಹಿಂತಿರುಗಿ;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_ಹೊರಹೋಗು);

    // Skip over continuation lines until seeing a useful line.
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
    {
	ಸಾಲು = ((ಅಕ್ಷರ **)(gap->ga_ದತ್ತಾಂಶ))[i];
	ಆದರೆ (ನೋಡಿದ_useful || ಸಾಲು[1] != '<')
	{
	    fputs(ಸಾಲು, fp_ಹೊರಹೋಗು);
	    ನೋಡಿದ_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
barline_parse(vir_T *virp, char_u *ಪಠ್ಯ, garray_T *ಮೌಲ್ಯಗಳು)
{
    char_u  *p = ಪಠ್ಯ;
    char_u  *nextp = ಖಾಲಿ;
    char_u  *buf = ಖಾಲಿ;
    bval_T  *ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    allocated = FALSE;
    ಪೂರ್ಣಾಂಕ	    eof;
    char_u  *sconv;
    ಪೂರ್ಣಾಂಕ	    converted;

    ಆಗ (*p == ',')
    {
	++p;
	ಆದರೆ (ga_grow(ಮೌಲ್ಯಗಳು, 1) == FAIL)
	    ನಿಲ್ಲಿಸು;
	ಮೌಲ್ಯ = (bval_T *)(ಮೌಲ್ಯಗಳು->ga_ದತ್ತಾಂಶ) + ಮೌಲ್ಯಗಳು->ga_len;

	ಆದರೆ (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    ಆದರೆ (!allocated)
	    {
		ಗಾಗಿ (i = 0; i < ಮೌಲ್ಯಗಳು->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(ಮೌಲ್ಯಗಳು->ga_ದತ್ತಾಂಶ) + i;

		    ಆದರೆ (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    ಆದರೆ (vim_isdigit(p[1]))
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
		ಆದರೆ (buf == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ TRUE;
		p = buf;
		ಗಾಗಿ (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    ಆದರೆ (eof || virp->vir_ಸಾಲು[0] != '|'
						  || virp->vir_ಸಾಲು[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			ಹಿಂತಿರುಗಿ eof || virp->vir_ಸಾಲು[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_ಸಾಲು);
		    ಆಗ (n > 2 && (virp->vir_ಸಾಲು[n - 1] == NL
					     || virp->vir_ಸಾಲು[n - 1] == CAR))
			--n;
		    n -= 2;
		    ಆದರೆ (n > todo)
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
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		ಆದರೆ (eof || virp->vir_ಸಾಲು[0] != '|'
					      || virp->vir_ಸಾಲು[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    ಹಿಂತಿರುಗಿ eof || virp->vir_ಸಾಲು[0] == '|';
		p = virp->vir_ಸಾಲು + 2;
	    }
	}

	ಆದರೆ (ಸುರಕ್ಷಿತ_isdigit(*p))
	{
	    ಮೌಲ್ಯ->bv_type = BVAL_NR;
	    ಮೌಲ್ಯ->bv_nr = getdigits(&p);
	    ++ಮೌಲ್ಯಗಳು->ga_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '"')
	{
	    ಪೂರ್ಣಾಂಕ	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    ಆಗ (*p != '"')
	    {
		ಆದರೆ (*p == NL || *p == NUL)
		    ಹಿಂತಿರುಗಿ TRUE;  // syntax error, drop the value
		ಆದರೆ (*p == '\\')
		{
		    ++p;
		    ಆದರೆ (*p == 'n')
			s[len++] = '\n';
		    ಇಲ್ಲದಿದ್ದರೆ
			s[len++] = *p;
		    ++p;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    ಮೌಲ್ಯ->bv_tofree = ಖಾಲಿ;
	    ಆದರೆ (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, ಖಾಲಿ);
		ಆದರೆ (sconv != ಖಾಲಿ)
		{
		    ಆದರೆ (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			ಮೌಲ್ಯ->bv_tofree = buf;
		    s = sconv;
		    len = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    ಆದರೆ (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    ಮೌಲ್ಯ->bv_string = s;
	    ಮೌಲ್ಯ->bv_type = BVAL_STRING;
	    ಮೌಲ್ಯ->bv_len = len;
	    ಮೌಲ್ಯ->bv_allocated = allocated || converted;
	    ++ಮೌಲ್ಯಗಳು->ga_len;
	    ಆದರೆ (nextp != ಖಾಲಿ)
	    {
		// values following a long string
		p = nextp;
		nextp = ಖಾಲಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == ',')
	{
	    ಮೌಲ್ಯ->bv_type = BVAL_ಖಾಲಿಗಳು;
	    ++ಮೌಲ್ಯಗಳು->ga_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_ಆವೃತ್ತಿ(FILE *fp_ಹೊರಹೋಗು)
{
    fprintf(fp_ಹೊರಹೋಗು, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_ಆವೃತ್ತಿ, VIMINFO_ಆವೃತ್ತಿ);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
no_viminfo(ಶೂನ್ಯ)
{
    // "vim -i NONE" does not read or write a viminfo file
    ಹಿಂತಿರುಗಿ STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
viminfo_error(ಅಕ್ಷರ *errnum, ಅಕ್ಷರ *ಸಂದೇಶ_2, char_u *ಸಾಲು)
{
    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, ಸಂದೇಶ_2);
    STRNCAT(IObuff, ಸಾಲು, IOSIZE - STRLEN(IObuff) - 1);
    ಆದರೆ (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((ಅಕ್ಷರ *)IObuff);
    ಆದರೆ (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_ದ_file));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_ಸಾಲು, '=');
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    // remove trailing newline
	    ++p;
	    ಗಾಗಿ (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_ಸಿದ್ಧತೆ(&virp->vir_conv, p, p_enc);
	}
    }
    ಹಿಂತಿರುಗಿ viminfo_readline(virp);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_viminfo_varlist(vir_T *virp, ಪೂರ್ಣಾಂಕ writing)
{
    char_u	*tab;
    ಪೂರ್ಣಾಂಕ		type = VAR_ಸಂಖ್ಯೆ;
    typval_T	tv;
    funccal_ನಮೂದು_T funccal_ನಮೂದು;

    ಆದರೆ (!writing && (ಕಂಡುಹಿಡಿ_viminfo_parameter('!') != ಖಾಲಿ))
    {
	tab = vim_strchr(virp->vir_ಸಾಲು + 1, '\t');
	ಆದರೆ (tab != ಖಾಲಿ)
	{
	    *tab++ = '\0';	// isolate the variable name
	    ಆಯ್ಕೆ (*tab)
	    {
		ಸ್ಥಿತಿ 'S': type = VAR_STRING; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'F': type = VAR_FLOAT; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'D': type = VAR_DICT; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'L': type = VAR_LIST; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'B': type = VAR_BLOB; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'X': type = VAR_SPECIAL; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'T': type = VAR_TUPLE; ನಿಲ್ಲಿಸು;
	    }

	    tab = vim_strchr(tab, '\t');
	    ಆದರೆ (tab != ಖಾಲಿ)
	    {
		tv.v_type = type;
		ಆದರೆ (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (ಪೂರ್ಣಾಂಕ)(tab - virp->vir_ಸಾಲು + 1), TRUE);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == VAR_FLOAT)
		    (ಶೂನ್ಯ)string2float(tab + 1, &tv.vval.v_float, FALSE);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    tv.vval.v_ಸಂಖ್ಯೆ = atol((ಅಕ್ಷರ *)tab + 1);
		    ಆದರೆ (type == VAR_SPECIAL && (tv.vval.v_ಸಂಖ್ಯೆ == VVAL_FALSE
					     || tv.vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		ಆದರೆ (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_ಅಭಿವ್ಯಕ್ತಿ(tv.vval.v_string, ಖಾಲಿ);

		    ಆದರೆ (etv == ಖಾಲಿ)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    ಆದರೆ (blob == ಖಾಲಿ)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_ನಮೂದು);
		ಗಣ_var(virp->vir_ಸಾಲು + 1, &tv, FALSE);
		restore_funccal();

		ಆದರೆ (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    ಹಿಂತಿರುಗಿ viminfo_readline(virp);
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
    ಪೂರ್ಣಾಂಕ		todo;
    ಅಕ್ಷರ	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ಆದರೆ (ಕಂಡುಹಿಡಿ_viminfo_parameter('!') == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    fputs(_("\n# global variables:\n"), fp);

    todo = (ಪೂರ್ಣಾಂಕ)gvht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(gvht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ಆದರೆ (var_flavour(this_var->di_ಕೀಲಿ) == VAR_FLAVOUR_VIMINFO)
	    {
		ಆಯ್ಕೆ (this_var->di_tv.v_type)
		{
		    ಸ್ಥಿತಿ VAR_STRING:  s = "STR"; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:  s = "NUM"; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ VAR_FLOAT:   s = "FLO"; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      ಪೂರ್ಣಾಂಕ	copyID = get_copyID();

			      s = "DIC";
			      ಆದರೆ (di != ಖಾಲಿ && !ಗಣ_ref_ಒಳ_ht(
					 &di->dv_hashtab, copyID, ಖಾಲಿ, ಖಾಲಿ)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ಮುಂದುವರಿ;
			      ನಿಲ್ಲಿಸು;
			  }
		    ಸ್ಥಿತಿ VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      ಪೂರ್ಣಾಂಕ	copyID = get_copyID();

			      s = "LIS";
			      ಆದರೆ (l != ಖಾಲಿ && !ಗಣ_ref_ಒಳ_list_ವಸ್ತುಗಳು(
						       l, copyID, ಖಾಲಿ, ಖಾಲಿ)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ಮುಂದುವರಿ;
			      ನಿಲ್ಲಿಸು;
			  }
		    ಸ್ಥಿತಿ VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      ಪೂರ್ಣಾಂಕ	copyID = get_copyID();

			      s = "TUP";
			      ಆದರೆ (tuple != ಖಾಲಿ && !ಗಣ_ref_ಒಳ_tuple_ವಸ್ತುಗಳು(
					       tuple, copyID, ಖಾಲಿ, ಖಾಲಿ)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ಮುಂದುವರಿ;
			      ನಿಲ್ಲಿಸು;
			  }
		    ಸ್ಥಿತಿ VAR_BLOB:    s = "BLO"; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ VAR_BOOL:    s = "XPL"; ನಿಲ್ಲಿಸು;  // backwards compat.
		    ಸ್ಥಿತಿ VAR_SPECIAL: s = "XPL"; ನಿಲ್ಲಿಸು;

		    ಸ್ಥಿತಿ VAR_UNKNOWN:
		    ಸ್ಥಿತಿ VAR_ANY:
		    ಸ್ಥಿತಿ VAR_VOID:
		    ಸ್ಥಿತಿ VAR_FUNC:
		    ಸ್ಥಿತಿ VAR_PARTIAL:
		    ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
		    ಸ್ಥಿತಿ VAR_CHANNEL:
		    ಸ್ಥಿತಿ VAR_INSTR:
		    ಸ್ಥಿತಿ VAR_CLASS:
		    ಸ್ಥಿತಿ VAR_OBJECT:
		    ಸ್ಥಿತಿ VAR_TYPEALIAS:
				      ಮುಂದುವರಿ;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_ಕೀಲಿ, s);
		ಆದರೆ (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((ಅಕ್ಷರ *)numbuf, "%ld",
					  (ದೀರ್ಘ)this_var->di_tv.vval.v_ಸಂಖ್ಯೆ);
		    p = numbuf;
		    tofree = ಖಾಲಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		ಆದರೆ (p != ಖಾಲಿ)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_viminfo_sub_string(vir_T *virp, ಪೂರ್ಣಾಂಕ force)
{
    ಆದರೆ (force || get_old_sub() == ಖಾಲಿ)
	ಗಣ_old_sub(viminfo_readstring(virp, 1, TRUE));
    ಹಿಂತಿರುಗಿ viminfo_readline(virp);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    ಆದರೆ (get_viminfo_parameter('/') == 0 || old_sub == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_viminfo_ಹುಡುಕು_ನಮೂನೆ(vir_T *virp, ಪೂರ್ಣಾಂಕ force, ಪೂರ್ಣಾಂಕ writing)
{
    char_u	*lp;
    ಪೂರ್ಣಾಂಕ		idx = -1;
    ಪೂರ್ಣಾಂಕ		magic = FALSE;
    ಪೂರ್ಣಾಂಕ		no_scs = FALSE;
    ಪೂರ್ಣಾಂಕ		off_ಸಾಲು = FALSE;
    ಪೂರ್ಣಾಂಕ		off_ಕೊನೆ = 0;
    ದೀರ್ಘ	off = 0;
    ಪೂರ್ಣಾಂಕ		setlast = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	hlsearch_on = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
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
    ಆದರೆ (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	ಆದರೆ (lp[1] == 'M')		// magic on
	    magic = TRUE;
	ಆದರೆ (lp[2] == 's')
	    no_scs = TRUE;
	ಆದರೆ (lp[3] == 'L')
	    off_ಸಾಲು = TRUE;
	ಆದರೆ (lp[4] == 'E')
	    off_ಕೊನೆ = ಹುಡುಕು_ಕೊನೆ;
	lp += 5;
	off = getdigits(&lp);
    }
    ಆದರೆ (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    ಆದರೆ (lp[0] == '/')
	idx = RE_ಹುಡುಕು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp[0] == '&')
	idx = RE_SUBST;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (idx >= 0)
    {
	spat = get_spat(idx);
	ಆದರೆ (force || spat->pat == ಖಾಲಿ)
	{
	    val = viminfo_readstring(virp, (ಪೂರ್ಣಾಂಕ)(lp - virp->vir_ಸಾಲು + 1),
									TRUE);
	    ಆದರೆ (val != ಖಾಲಿ)
	    {
		ಆದರೆ (!writing)
		{
		    ಗಣ_ಕೊನೆಯ_ಹುಡುಕು_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.ಸಾಲು = off_ಸಾಲು;
		    spat->off.ಕೊನೆ = off_ಕೊನೆ;
		    spat->off.off = off;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		    ಆದರೆ (setlast)
			ಗಣ_no_hlsearch(!hlsearch_on);
#ಆದರೆ_ಅಂತ್ಯ
		}
		vim_free(val);
	    }
	}
    }
    ಹಿಂತಿರುಗಿ viminfo_readline(virp);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
wvsp_one(
    FILE	*fp,	// file to write to
    ಪೂರ್ಣಾಂಕ		idx,	// spats[] index
    ಅಕ್ಷರ	*s,	// search pat
    ಪೂರ್ಣಾಂಕ		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    ಆದರೆ (spat->pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.ಸಾಲು ? 'L' : 'l',	// line offset
	    spat->off.ಕೊನೆ  ? 'E' : 'e',	// offset from end
	    spat->off.off,			//ಸ್ಥಾನಾಂತರ
	    get_spat_ಕೊನೆಯ_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_ಹುಡುಕು_ನಮೂನೆ(FILE *fp)
{
    ಆದರೆ (get_viminfo_parameter('/') == 0)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || ಕಂಡುಹಿಡಿ_viminfo_parameter('h') != ಖಾಲಿ) ? 'h' : 'H');
#ಆದರೆ_ಅಂತ್ಯ
    wvsp_one(fp, RE_ಹುಡುಕು, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

ಸ್ಥಿರ yankreg_T *y_read_regs = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿಸು REG_ಹಿಂದಿನ 1
#ವ್ಯಾಖ್ಯಾನಿಸು REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_viminfo_registers(ಶೂನ್ಯ)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, ಅಂಕೆ_REGISTERS);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಮುಕ್ತಾಯ_viminfo_registers(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j;

    ಆದರೆ (y_read_regs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_REGISTERS; ++i)
	ಆದರೆ (y_read_regs[i].y_array != ಖಾಲಿ)
	{
	    ಗಾಗಿ (j = 0; j < y_read_regs[i].y_ಗಾತ್ರ; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_viminfo_ರಿಜಿಸ್ಟರ್(vir_T *virp, ಪೂರ್ಣಾಂಕ force)
{
    ಪೂರ್ಣಾಂಕ		eof;
    ಪೂರ್ಣಾಂಕ		do_it = TRUE;
    ಪೂರ್ಣಾಂಕ		ಗಾತ್ರ_2;
    ಪೂರ್ಣಾಂಕ		ಎಲ್ಲೆ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಗಣ_prev = FALSE;
    char_u	*str;
    string_T	*array = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_ಅಗಲ = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_ಸಾಲು + 1;

    // If the line starts with "" this is the y_previous register.
    ಆದರೆ (*str == '"')
    {
	ಗಣ_prev = TRUE;
	str++;
    }

    ಆದರೆ (!ASCII_ISALNUM(*str) && *str != '-')
    {
	ಆದರೆ (viminfo_error("E577: ", _(e_illegal_ರಿಜಿಸ್ಟರ್_name), virp->vir_ಸಾಲು))
	    ಹಿಂತಿರುಗಿ TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_ರಿಜಿಸ್ಟರ್(*str++, FALSE);
    y_current_p = get_y_current();
    ಆದರೆ (!force && y_current_p->y_array != ಖಾಲಿ)
	do_it = FALSE;

    ಆದರೆ (*str == '@')
    {
	// "x@: register x used for @@
	ಆದರೆ (force || get_execreg_lastc() == NUL)
	    ಗಣ_execreg_lastc(str[-1]);
    }

    ಗಾತ್ರ_2 = 0;
    ಎಲ್ಲೆ = 100;	// Optimized for registers containing <= 100 lines
    ಆದರೆ (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	ಆದರೆ (ಗಣ_prev)
	    ಗಣ_y_ಹಿಂದಿನ(y_current_p);
	array = ALLOC_MULT(string_T, ಎಲ್ಲೆ);
	str = skipwhite(skiptowhite(str));
	ಆದರೆ (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	ಇಲ್ಲದಿದ್ದರೆ
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_ಅಗಲ = getdigits(&str);
    }

    ಆಗ (!(eof = viminfo_readline(virp))
		    && (virp->vir_ಸಾಲು[0] == TAB || virp->vir_ಸಾಲು[0] == '<'))
    {
	ಆದರೆ (do_it)
	{
	    ಆದರೆ (ಗಾತ್ರ_2 == ಎಲ್ಲೆ)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)ಎಲ್ಲೆ * 2 * ಗಾತ್ರ(string_T));

		ಆದರೆ (new_array == ಖಾಲಿ)
		{
		    do_it = FALSE;
		    ನಿಲ್ಲಿಸು;
		}
		ಗಾಗಿ (i = 0; i < ಎಲ್ಲೆ; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		ಎಲ್ಲೆ *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    ಆದರೆ (str != ಖಾಲಿ)
	    {
		array[ಗಾತ್ರ_2].string = str;
		array[ಗಾತ್ರ_2].ಉದ್ದಳತೆ = STRLEN(str);
		++ಗಾತ್ರ_2;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// error, don't store the result
		do_it = FALSE;
	}
    }

    ಆದರೆ (do_it)
    {
	// free y_array[]
	ಗಾಗಿ (i = 0; i < y_current_p->y_ಗಾತ್ರ; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_ಅಗಲ = new_ಅಗಲ;
	y_current_p->y_ಗಾತ್ರ = ಗಾತ್ರ_2;
	y_current_p->y_ಸಮಯ_ಗಣ = 0;
	ಆದರೆ (ಗಾತ್ರ_2 == 0)
	{
	    y_current_p->y_array = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, ಗಾತ್ರ_2);
	    ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i++)
	    {
		ಆದರೆ (y_current_p->y_array == ಖಾಲಿ)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].ಉದ್ದಳತೆ = array[i].ಉದ್ದಳತೆ;
		}
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Free array[] if it was filled.
	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    ಹಿಂತಿರುಗಿ eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_viminfo_ರಿಜಿಸ್ಟರ್(garray_T *ಮೌಲ್ಯಗಳು, ಪೂರ್ಣಾಂಕ force)
{
    bval_T	*vp = (bval_T *)ಮೌಲ್ಯಗಳು->ga_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		flags;
    ಪೂರ್ಣಾಂಕ		name;
    ಪೂರ್ಣಾಂಕ		type;
    ಪೂರ್ಣಾಂಕ		linecount;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    ಪೂರ್ಣಾಂಕ		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    ಆದರೆ (ಮೌಲ್ಯಗಳು->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	ಹಿಂತಿರುಗಿ;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    ಆದರೆ (name < 0 || name >= ಅಂಕೆ_REGISTERS)
	ಹಿಂತಿರುಗಿ;
    type = vp[2].bv_nr;
    ಆದರೆ (type != MCHAR && type != MLINE && type != MBLOCK)
	ಹಿಂತಿರುಗಿ;
    linecount = vp[3].bv_nr;
    ಆದರೆ (ಮೌಲ್ಯಗಳು->ga_len < 6 + linecount)
	ಹಿಂತಿರುಗಿ;
    ಅಗಲ = vp[4].bv_nr;
    ಆದರೆ (ಅಗಲ < 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (y_read_regs != ಖಾಲಿ)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    ಇಲ್ಲದಿದ್ದರೆ
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    ಆದರೆ (y_ptr->y_array != ಖಾಲಿ && !force
			 && (timestamp == 0 || y_ptr->y_ಸಮಯ_ಗಣ > timestamp))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (y_ptr->y_array != ಖಾಲಿ)
	ಗಾಗಿ (i = 0; i < y_ptr->y_ಗಾತ್ರ; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    ಆದರೆ (y_read_regs == ಖಾಲಿ)
    {
	ಆದರೆ (flags & REG_ಹಿಂದಿನ)
	    ಗಣ_y_ಹಿಂದಿನ(y_ptr);
	ಆದರೆ ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    ಗಣ_execreg_lastc(get_ರಿಜಿಸ್ಟರ್_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_ಅಗಲ = ಅಗಲ;
    y_ptr->y_ಗಾತ್ರ = linecount;
    y_ptr->y_ಸಮಯ_ಗಣ = timestamp;
    ಆದರೆ (linecount == 0)
    {
	y_ptr->y_array = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    ಆದರೆ (y_ptr->y_array == ಖಾಲಿ)
    {
	y_ptr->y_ಗಾತ್ರ = 0; // ensure object state is consistent
	ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (i = 0; i < linecount; i++)
    {
	ಆದರೆ (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].ಉದ್ದಳತೆ = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vp[i + 6].bv_type != BVAL_STRING)
	{
	    ಬಿಡುಗಡೆ(y_ptr->y_array);
	    y_ptr->y_array = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].ಉದ್ದಳತೆ = vp[i + 6].bv_len;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_registers(FILE *fp)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    char_u	*type;
    char_u	c;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		max_ಅಂಕೆ_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		max_kbyte;
    ದೀರ್ಘ	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_ಅಂಕೆ_ಸಾಲುಗಳು = get_viminfo_parameter('<');
    ಆದರೆ (max_ಅಂಕೆ_ಸಾಲುಗಳು < 0)
	max_ಅಂಕೆ_ಸಾಲುಗಳು = get_viminfo_parameter('"');
    ಆದರೆ (max_ಅಂಕೆ_ಸಾಲುಗಳು == 0)
	ಹಿಂತಿರುಗಿ;
    max_kbyte = get_viminfo_parameter('s');
    ಆದರೆ (max_kbyte == 0)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_REGISTERS; i++)
    {
#ಆದರೆ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	ಆದರೆ (i == ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್ || i == PLUS_ರಿಜಿಸ್ಟರ್ || i == REAL_PLUS_ರಿಜಿಸ್ಟರ್)
	    ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
	// Neither do we want the '~' register
	ಆದರೆ (i == TILDE_ರಿಜಿಸ್ಟರ್)
	    ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	ಆದರೆ (y_read_regs != ಖಾಲಿ
		&& y_read_regs[i].y_array != ಖಾಲಿ
		&& (y_regs_p[i].y_array == ಖಾಲಿ ||
			    y_read_regs[i].y_ಸಮಯ_ಗಣ > y_regs_p[i].y_ಸಮಯ_ಗಣ))
	    y_ptr = &y_read_regs[i];
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (y_regs_p[i].y_array == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	ಅಂಕೆ_ಸಾಲುಗಳು = y_ptr->y_ಗಾತ್ರ;
	ಆದರೆ (ಅಂಕೆ_ಸಾಲುಗಳು == 0
		|| (ಅಂಕೆ_ಸಾಲುಗಳು == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    ಮುಂದುವರಿ;

	ಆದರೆ (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    ಗಾಗಿ (j = 0; j < ಅಂಕೆ_ಸಾಲುಗಳು; j++)
		len += (ದೀರ್ಘ)y_ptr->y_array[j].ಉದ್ದಳತೆ + 1L;
	    ಆದರೆ (len > (ದೀರ್ಘ)max_kbyte * 1024L)
		ಮುಂದುವರಿ;
	}

	ಆಯ್ಕೆ (y_ptr->y_type)
	{
	    ಸ್ಥಿತಿ MLINE:
		type = (char_u *)"LINE";
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MCHAR:
		type = (char_u *)"CHAR";
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MBLOCK:
		type = (char_u *)"BLOCK";
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		semsg(_(e_unknown_ರಿಜಿಸ್ಟರ್_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (get_y_ಹಿಂದಿನ() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_ರಿಜಿಸ್ಟರ್_name(i);
	fprintf(fp, "\"%c", c);
	ಆದರೆ (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (ಪೂರ್ಣಾಂಕ)y_ptr->y_ಅಗಲ);

	// If max_num_lines < 0, then we save ALL the lines in the register
	ಆದರೆ (max_ಅಂಕೆ_ಸಾಲುಗಳು > 0 && ಅಂಕೆ_ಸಾಲುಗಳು > max_ಅಂಕೆ_ಸಾಲುಗಳು)
	    ಅಂಕೆ_ಸಾಲುಗಳು = max_ಅಂಕೆ_ಸಾಲುಗಳು;
	ಗಾಗಿ (j = 0; j < ಅಂಕೆ_ಸಾಲುಗಳು; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    ಪೂರ್ಣಾಂಕ	    flags = 0;
	    ಪೂರ್ಣಾಂಕ	    ಉಳಿದ;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    ಆದರೆ (get_y_ಹಿಂದಿನ() == &y_regs_p[i])
		flags |= REG_ಹಿಂದಿನ;
	    ಆದರೆ (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_ರಿಜಿಸ್ಟರ್, flags,
		    i, y_ptr->y_type, ಅಂಕೆ_ಸಾಲುಗಳು, (ಪೂರ್ಣಾಂಕ)y_ptr->y_ಅಗಲ,
		    (ದೀರ್ಘ)y_ptr->y_ಸಮಯ_ಗಣ);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    ಉಳಿದ = LSIZE - 71;
	    ಗಾಗಿ (j = 0; j < ಅಂಕೆ_ಸಾಲುಗಳು; j++)
	    {
		putc(',', fp);
		--ಉಳಿದ;
		ಉಳಿದ = barline_writestring(fp, y_ptr->y_array[j].string,
								   ಉಳಿದ);
	    }
	    putc('\n', fp);
	}
    }
}

/*
 * Functions relating to reading/writing marks from viminfo
 */

ಸ್ಥಿರ xfmark_T *vi_namedfm = ಖಾಲಿ;
ಸ್ಥಿರ xfmark_T *vi_jumplist = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vi_jumplist_len = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
write_one_mark(FILE *fp_ಹೊರಹೋಗು, ಪೂರ್ಣಾಂಕ c, pos_T *pos)
{
    ಆದರೆ (pos->lnum != 0)
	fprintf(fp_ಹೊರಹೋಗು, "\t%c\t%ld\t%d\n", c, (ದೀರ್ಘ)pos->lnum, (ಪೂರ್ಣಾಂಕ)pos->ಲಂಬಸಾಲು);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_ಬಫರ್_marks(buf_T *buf, FILE *fp_ಹೊರಹೋಗು)
{
    ಪೂರ್ಣಾಂಕ		i;
    pos_T	pos;

    home_replace(ಖಾಲಿ, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_ಹೊರಹೋಗು, "\n> ");
    viminfo_writestring(fp_ಹೊರಹೋಗು, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_ಕೊನೆಯ_ಬಳಸಿದ;
    pos.ಲಂಬಸಾಲು = 0;
    write_one_mark(fp_ಹೊರಹೋಗು, '*', &pos);

    write_one_mark(fp_ಹೊರಹೋಗು, '"', &buf->b_ಕೊನೆಯ_cursor);
    write_one_mark(fp_ಹೊರಹೋಗು, '^', &buf->b_ಕೊನೆಯ_ಸೇರಿಸು);
    write_one_mark(fp_ಹೊರಹೋಗು, '.', &buf->b_ಕೊನೆಯ_change);
    // changelist positions are stored oldest first
    ಗಾಗಿ (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	ಆದರೆ (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_ಹೊರಹೋಗು, '+', &buf->b_changelist[i]);
    }
    ಗಾಗಿ (i = 0; i < NMARKS; i++)
	write_one_mark(fp_ಹೊರಹೋಗು, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
skip_for_viminfo(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_marks(FILE *fp_ಹೊರಹೋಗು, garray_T *buflist)
{
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		is_mark_ಗಣ;
    ಪೂರ್ಣಾಂಕ		i;
    win_T	*win;
    tabpage_T	*tp;

    // Set b_last_cursor for the all buffers that have a window.
    FOR_ALL_TAB_WINDOWS(tp, win)
	ಗಣ_ಕೊನೆಯ_cursor(win);

    fputs(_("\n# History of marks within files (newest to oldest):\n"), fp_ಹೊರಹೋಗು);
    FOR_ALL_BUFFERS(buf)
    {
	// Only write something if buffer has been loaded and at least one
	// mark is set.
	ಆದರೆ (buf->b_marks_read)
	{
	    ಆದರೆ (buf->b_ಕೊನೆಯ_cursor.lnum != 0)
		is_mark_ಗಣ = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		is_mark_ಗಣ = FALSE;
		ಗಾಗಿ (i = 0; i < NMARKS; i++)
		    ಆದರೆ (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_ಗಣ = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
	    }
	    ಆದರೆ (is_mark_ಗಣ && buf->b_ffname != ಖಾಲಿ
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		ಆದರೆ (buflist == ಖಾಲಿ)
		    write_ಬಫರ್_marks(buf, fp_ಹೊರಹೋಗು);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_ದತ್ತಾಂಶ)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    ಪೂರ್ಣಾಂಕ		c1,
    ಪೂರ್ಣಾಂಕ		c2)
{
    char_u	*name;

    ಆದರೆ (fm->fmark.mark.lnum == 0)	// not set
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
	name = fm->fname;		// use name from .viminfo
    ಆದರೆ (name != ಖಾಲಿ && *name != NUL)
    {
	fprintf(fp, "%c%c  %ld  %ld  ", c1, c2, (ದೀರ್ಘ)fm->fmark.mark.lnum,
						    (ದೀರ್ಘ)fm->fmark.mark.ಲಂಬಸಾಲು);
	viminfo_writestring(fp, name);

	// Barline: |{bartype},{name},{lnum},{col},{timestamp},{filename}
	// size up to filename: 8 + 3 * 20
	fprintf(fp, "|%d,%d,%ld,%ld,%ld,", BARTYPE_MARK, c2,
		(ದೀರ್ಘ)fm->fmark.mark.lnum, (ದೀರ್ಘ)fm->fmark.mark.ಲಂಬಸಾಲು,
		(ದೀರ್ಘ)fm->ಸಮಯ_ಗಣ);
	barline_writestring(fp, name, LSIZE - 70);
	putc('\n', fp);
    }

    ಆದರೆ (fm->fmark.fnum != 0)
	vim_free(name);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_viminfo_filemarks(FILE *fp)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    ಪೂರ್ಣಾಂಕ		vi_idx;
    ಪೂರ್ಣಾಂಕ		idx;

    ಆದರೆ (get_viminfo_parameter('f') == 0)
	ಹಿಂತಿರುಗಿ;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    ಗಾಗಿ (i = 0; i < NMARKS; i++)
    {
	ಆದರೆ (vi_namedfm != ಖಾಲಿ
			&& (vi_namedfm[i].ಸಮಯ_ಗಣ > namedfm_p[i].ಸಮಯ_ಗಣ))
	    fm = &vi_namedfm[i];
	ಇಲ್ಲದಿದ್ದರೆ
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    ಆದರೆ (curbuf->b_ffname != ಖಾಲಿ && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	ಗಾಗಿ (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    ಆದರೆ (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == ಖಾಲಿ
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != ಖಾಲಿ
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		ನಿಲ್ಲಿಸು;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	ಗಾಗಿ ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = ಖಾಲಿ;
	namedfm_p[NMARKS].ಸಮಯ_ಗಣ = vim_ಸಮಯ();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    ಗಾಗಿ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != ಖಾಲಿ ? &vi_namedfm[vi_idx] : ಖಾಲಿ;

	ಆದರೆ (vi_fm != ಖಾಲಿ
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->ಸಮಯ_ಗಣ > namedfm_p[idx].ಸಮಯ_ಗಣ
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    fm = &namedfm_p[idx++];
	    ಆದರೆ (vi_fm != ಖಾಲಿ
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->ಸಮಯ_ಗಣ == fm->ಸಮಯ_ಗಣ
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != ಖಾಲಿ
			  && fm->fname != ಖಾಲಿ
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
    ಗಾಗಿ (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : ಖಾಲಿ;
	vi_fm = (vi_jumplist != ಖಾಲಿ && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : ಖಾಲಿ;
	ಆದರೆ (fm == ಖಾಲಿ && vi_fm == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (fm == ಖಾಲಿ || (vi_fm != ಖಾಲಿ && fm->ಸಮಯ_ಗಣ < vi_fm->ಸಮಯ_ಗಣ))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    --idx;
	ಆದರೆ (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != ಖಾಲಿ
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    ಪೂರ್ಣಾಂಕ
buf_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    ಆದರೆ (buf1->b_ಕೊನೆಯ_ಬಳಸಿದ == buf2->b_ಕೊನೆಯ_ಬಳಸಿದ)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ buf1->b_ಕೊನೆಯ_ಬಳಸಿದ > buf2->b_ಕೊನೆಯ_ಬಳಸಿದ ? -1 : 1;
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
    FILE	*fp_ಹೊರಹೋಗು,
    garray_T	*buflist,
    ಪೂರ್ಣಾಂಕ		eof,
    ಪೂರ್ಣಾಂಕ		flags)
{
    char_u	*ಸಾಲು = virp->vir_ಸಾಲು;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_marked_files;
    ಪೂರ್ಣಾಂಕ		ತುಂಬು_marks;
    ಪೂರ್ಣಾಂಕ		copy_marks_ಹೊರಹೋಗು;
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    list_T	*list = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		buflist_ಬಳಸಿದ = 0;
    buf_T	*buflist_buf = ಖಾಲಿ;

    ಆದರೆ ((name_buf = alloc(LSIZE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    *name_buf = NUL;

    ಆದರೆ (fp_ಹೊರಹೋಗು != ಖಾಲಿ && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_ದತ್ತಾಂಶ, (size_t)buflist->ga_len,
						ಗಾತ್ರ(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_ದತ್ತಾಂಶ)[0];
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (fp_ಹೊರಹೋಗು == ಖಾಲಿ && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	ಆದರೆ (list != ಖಾಲಿ)
	    ಗಣ_vim_var_list(VV_OLDFILES, list);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಅಂಕೆ_marked_files = get_viminfo_parameter('\'');
    ಆಗ (!eof && (ಎಣಿಕೆ_2 < ಅಂಕೆ_marked_files || fp_ಹೊರಹೋಗು == ಖಾಲಿ))
    {
	ಆದರೆ (ಸಾಲು[0] != '>')
	{
	    ಆದರೆ (ಸಾಲು[0] != '\n' && ಸಾಲು[0] != '\r' && ಸಾಲು[0] != '#')
	    {
		ಆದರೆ (viminfo_error("E576: ", _(e_nonr_missing_gt), ಸಾಲು))
		    ನಿಲ್ಲಿಸು;	// too many errors, return now
	    }
	    eof = vim_fgets(ಸಾಲು, LSIZE, virp->vir_fd);
	    ಮುಂದುವರಿ;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(ಸಾಲು + 1);
	str = viminfo_readstring(virp, (ಪೂರ್ಣಾಂಕ)(str - virp->vir_ಸಾಲು), FALSE);
	ಆದರೆ (str == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	p = str + STRLEN(str);
	ಆಗ (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	ಆದರೆ (*p)
	    p++;
	*p = NUL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (list != ಖಾಲಿ)
	    list_append_string(list, str, -1);
#ಆದರೆ_ಅಂತ್ಯ

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	ತುಂಬು_marks = copy_marks_ಹೊರಹೋಗು = FALSE;
	ಆದರೆ (fp_ಹೊರಹೋಗು == ಖಾಲಿ)
	{
	    ಆದರೆ ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != ಖಾಲಿ)
	    {
		ಆದರೆ (*name_buf == NUL)	    // only need to do this once
		    home_replace(ಖಾಲಿ, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		ಆದರೆ (fnamecmp(str, name_buf) == 0)
		    ತುಂಬು_marks = TRUE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		ಆದರೆ (buf->b_ffname != ಖಾಲಿ)
		{
		    home_replace(ಖಾಲಿ, buf->b_ffname, name_buf, LSIZE, TRUE);
		    ಆದರೆ (fnamecmp(str, name_buf) == 0)
			ನಿಲ್ಲಿಸು;
		}

	    // Copy marks if the buffer has not been loaded.
	    ಆದರೆ (buf == ಖಾಲಿ || !buf->b_marks_read)
	    {
		ಪೂರ್ಣಾಂಕ	did_read_ಸಾಲು = FALSE;

		ಆದರೆ (buflist_buf != ಖಾಲಿ)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    ಆದರೆ (!viminfo_readline(virp) && ಸಾಲು[0] == TAB)
		    {
			did_read_ಸಾಲು = TRUE;
			ಆದರೆ (ಸಾಲು[1] == '*')
			{
			    ದೀರ್ಘ	ltime;

			    ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)ಸಾಲು + 2, "%ld ", &ltime);
			    ಆಗ ((ಸಮಯ_T)ltime < buflist_buf->b_ಕೊನೆಯ_ಬಳಸಿದ)
			    {
				write_ಬಫರ್_marks(buflist_buf, fp_ಹೊರಹೋಗು);
				ಆದರೆ (++ಎಣಿಕೆ_2 >= ಅಂಕೆ_marked_files)
				    ನಿಲ್ಲಿಸು;
				ಆದರೆ (++buflist_ಬಳಸಿದ == buflist->ga_len)
				{
				    buflist_buf = ಖಾಲಿ;
				    ನಿಲ್ಲಿಸು;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_ದತ್ತಾಂಶ)[buflist_ಬಳಸಿದ];
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    ಆಗ (ಎಣಿಕೆ_2 < ಅಂಕೆ_marked_files
					    && buflist_ಬಳಸಿದ < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_ದತ್ತಾಂಶ)
							     [buflist_ಬಳಸಿದ++];
				write_ಬಫರ್_marks(buflist_buf, fp_ಹೊರಹೋಗು);
				++ಎಣಿಕೆ_2;
			    }
			    buflist_buf = ಖಾಲಿ;
			}

			ಆದರೆ (ಎಣಿಕೆ_2 >= ಅಂಕೆ_marked_files)
			{
			    vim_free(str);
			    ನಿಲ್ಲಿಸು;
			}
		    }
		}

		fputs("\n> ", fp_ಹೊರಹೋಗು);
		viminfo_writestring(fp_ಹೊರಹೋಗು, str);
		ಆದರೆ (did_read_ಸಾಲು)
		    fputs((ಅಕ್ಷರ *)ಸಾಲು, fp_ಹೊರಹೋಗು);

		ಎಣಿಕೆ_2++;
		copy_marks_ಹೊರಹೋಗು = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	ಆಗ (!(eof = viminfo_readline(virp)) && ಸಾಲು[0] == TAB)
	{
	    ಆದರೆ (ತುಂಬು_marks)
	    {
		ಆದರೆ (ಸಾಲು[1] != NUL)
		{
		    ಚಿಹ್ನೆರಹಿತ u;

		    ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)ಸಾಲು + 2, "%ld %u", &pos.lnum, &u);
		    pos.ಲಂಬಸಾಲು = u;
		    ಆಯ್ಕೆ (ಸಾಲು[1])
		    {
			ಸ್ಥಿತಿ '"': curbuf->b_ಕೊನೆಯ_cursor = pos; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ '^': curbuf->b_ಕೊನೆಯ_ಸೇರಿಸು = pos; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ '.': curbuf->b_ಕೊನೆಯ_change = pos; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ '+':
				  // changelist positions are stored oldest
				  //ಮೊದಲ
				  ಆದರೆ (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    ಗಾತ್ರ(pos_T) * (JUMPLISTSIZE - 1));
				  ಇಲ್ಲದಿದ್ದರೆ
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  ನಿಲ್ಲಿಸು;

				  // Using the line number for the last-used
				  // timestamp.
			ಸ್ಥಿತಿ '*': curbuf->b_ಕೊನೆಯ_ಬಳಸಿದ = pos.lnum; ನಿಲ್ಲಿಸು;

			ಪೂರ್ವನಿಯೋಜಿತ:  ಆದರೆ ((i = ಸಾಲು[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copy_marks_ಹೊರಹೋಗು)
		fputs((ಅಕ್ಷರ *)ಸಾಲು, fp_ಹೊರಹೋಗು);
	}

	ಆದರೆ (ತುಂಬು_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		ಆದರೆ (wp->w_ಬಫರ್ == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    ಆದರೆ (flags & VIF_ONLY_CURBUF)
		ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (fp_ಹೊರಹೋಗು != ಖಾಲಿ)
	// Write any remaining entries from buflist.
	ಆಗ (ಎಣಿಕೆ_2 < ಅಂಕೆ_marked_files && buflist_ಬಳಸಿದ < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_ದತ್ತಾಂಶ)[buflist_ಬಳಸಿದ++];
	    write_ಬಫರ್_marks(buflist_buf, fp_ಹೊರಹೋಗು);
	    ++ಎಣಿಕೆ_2;
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
    ಆದರೆ (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != ಖಾಲಿ)
	read_viminfo(ಖಾಲಿ, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_viminfo_filemark(vir_T *virp, ಪೂರ್ಣಾಂಕ force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    ಪೂರ್ಣಾಂಕ		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_ಸಾಲು + 1;
    ಆದರೆ (*str <= 127
	    && ((*virp->vir_ಸಾಲು == '\''
				       && (VIM_ISDIGIT(*str) || ಸುರಕ್ಷಿತ_isupper(*str)))
	     || (*virp->vir_ಸಾಲು == '-' && *str == '\'')))
    {
	ಆದರೆ (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    ಆದರೆ (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಾಗಿ (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = ಖಾಲಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	ಇಲ್ಲದಿದ್ದರೆ
	    fm = &namedfm_p[*str - 'A'];
	ಆದರೆ (fm != ಖಾಲಿ && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.ಲಂಬಸಾಲು = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (ಪೂರ್ಣಾಂಕ)(str - virp->vir_ಸಾಲು),
								       FALSE);
	    fm->ಸಮಯ_ಗಣ = 0;
	}
    }
    ಹಿಂತಿರುಗಿ vim_fgets(virp->vir_ಸಾಲು, LSIZE, virp->vir_fd);
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
ಮುಕ್ತಾಯ_viminfo_marks(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (vi_namedfm != ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    ಆದರೆ (vi_jumplist != ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_viminfo_mark(garray_T *ಮೌಲ್ಯಗಳು, ಪೂರ್ಣಾಂಕ force)
{
    bval_T	*vp = (bval_T *)ಮೌಲ್ಯಗಳು->ga_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		name;
    linenr_T	lnum;
    colnr_T	ಲಂಬಸಾಲು;
    time_t	timestamp;
    xfmark_T	*fm = ಖಾಲಿ;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    ಆದರೆ (ಮೌಲ್ಯಗಳು->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	ಹಿಂತಿರುಗಿ;

    name = vp[0].bv_nr;
    ಆದರೆ (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	ಹಿಂತಿರುಗಿ;
    lnum = vp[1].bv_nr;
    ಲಂಬಸಾಲು = vp[2].bv_nr;
    ಆದರೆ (lnum <= 0 || ಲಂಬಸಾಲು < 0)
	ಹಿಂತಿರುಗಿ;
    timestamp = (time_t)vp[3].bv_nr;

    ಆದರೆ (name == '\'')
    {
	ಆದರೆ (vi_jumplist != ಖಾಲಿ)
	{
	    ಆದರೆ (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ idx;
	    ಪೂರ್ಣಾಂಕ i;

	    // If we have a timestamp insert it in the right place.
	    ಆದರೆ (timestamp != 0)
	    {
		ಗಾಗಿ (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    ಆದರೆ (curwin->w_jumplist[idx].ಸಮಯ_ಗಣ < timestamp)
		    {
			++idx;
			ನಿಲ್ಲಿಸು;
		    }
		// idx cannot be zero now
		ಆದರೆ (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		idx = -1;

	    ಆದರೆ (idx >= 0)
	    {
		ಆದರೆ (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    ಗಾಗಿ (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Move newer entries forward.
		    ಗಾಗಿ (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = ಖಾಲಿ;
		fm->ಸಮಯ_ಗಣ = 0;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	ಆದರೆ (VIM_ISDIGIT(name))
	{
	    ಆದರೆ (vi_namedfm != ಖಾಲಿ)
		idx = name - '0' + NMARKS;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		ಗಾಗಿ (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    ಆದರೆ (namedfm_p[idx].ಸಮಯ_ಗಣ < timestamp)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    ಹಿಂತಿರುಗಿ;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		ಗಾಗಿ ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = ಖಾಲಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    idx = name - 'A';
	ಆದರೆ (vi_namedfm != ಖಾಲಿ)
	    fm = &vi_namedfm[idx];
	ಇಲ್ಲದಿದ್ದರೆ
	    fm = &namedfm_p[idx];
    }

    ಆದರೆ (fm != ಖಾಲಿ)
    {
	ಆದರೆ (vi_namedfm != ಖಾಲಿ || fm->fmark.mark.lnum == 0
					  || fm->ಸಮಯ_ಗಣ < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    ಆದರೆ (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->ಸಮಯ_ಗಣ = timestamp;
	}
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_viminfo_barline(vir_T *virp, ಪೂರ್ಣಾಂಕ got_encoding, ಪೂರ್ಣಾಂಕ force, ಪೂರ್ಣಾಂಕ writing)
{
    char_u	*p = virp->vir_ಸಾಲು + 1;
    ಪೂರ್ಣಾಂಕ		bartype;
    garray_T	ಮೌಲ್ಯಗಳು;
    bval_T	*vp;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		read_ಮುಂದಿನ = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    ಆದರೆ (*p == '<')
    {
	// Continuation line of an unrecognized item.
	ಆದರೆ (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_ಸಾಲು);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ga_init2(&ಮೌಲ್ಯಗಳು, ಗಾತ್ರ(bval_T), 20);
	bartype = getdigits(&p);
	ಆಯ್ಕೆ (bartype)
	{
	    ಸ್ಥಿತಿ BARTYPE_ಆವೃತ್ತಿ:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		ಆದರೆ (!got_encoding)
		{
		    read_ಮುಂದಿನ = barline_parse(virp, p, &ಮೌಲ್ಯಗಳು);
		    vp = (bval_T *)ಮೌಲ್ಯಗಳು.ga_ದತ್ತಾಂಶ;
		    ಆದರೆ (ಮೌಲ್ಯಗಳು.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_ಆವೃತ್ತಿ = vp->bv_nr;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ BARTYPE_HISTORY:
		read_ಮುಂದಿನ = barline_parse(virp, p, &ಮೌಲ್ಯಗಳು);
		handle_viminfo_history(&ಮೌಲ್ಯಗಳು, writing);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ BARTYPE_ರಿಜಿಸ್ಟರ್:
		read_ಮುಂದಿನ = barline_parse(virp, p, &ಮೌಲ್ಯಗಳು);
		handle_viminfo_ರಿಜಿಸ್ಟರ್(&ಮೌಲ್ಯಗಳು, force);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ BARTYPE_MARK:
		read_ಮುಂದಿನ = barline_parse(virp, p, &ಮೌಲ್ಯಗಳು);
		handle_viminfo_mark(&ಮೌಲ್ಯಗಳು, force);
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:
		// copy unrecognized line (for future use)
		ಆದರೆ (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_ಸಾಲು);
	}
	ಗಾಗಿ (i = 0; i < ಮೌಲ್ಯಗಳು.ga_len; ++i)
	{
	    vp = (bval_T *)ಮೌಲ್ಯಗಳು.ga_ದತ್ತಾಂಶ + i;
	    ಆದರೆ (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&ಮೌಲ್ಯಗಳು);
    }

    ಆದರೆ (read_ಮುಂದಿನ)
	ಹಿಂತಿರುಗಿ viminfo_readline(virp);
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_viminfo_ಮೇಲೆ_ಗಮ್ಯ_marks(
    vir_T	*virp,
    ಪೂರ್ಣಾಂಕ		forceit,
    ಪೂರ್ಣಾಂಕ		writing)
{
    ಪೂರ್ಣಾಂಕ		eof;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    ಆಗ (!eof && virp->vir_ಸಾಲು[0] != '>')
    {
	ಆಯ್ಕೆ (virp->vir_ಸಾಲು[0])
	{
		// Characters reserved for future expansion, ignored now
	    ಸ್ಥಿತಿ '+': // "+40 /path/dir file", for running vim without args
	    ಸ್ಥಿತಿ '^': // to be defined
	    ಸ್ಥಿತಿ '<': // long line - ignored
		// A comment or empty line.
	    ಸ್ಥಿತಿ NUL:
	    ಸ್ಥಿತಿ '\r':
	    ಸ್ಥಿತಿ '\n':
	    ಸ್ಥಿತಿ '#':
		eof = viminfo_readline(virp);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '!': // global variable
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#ಇಲ್ಲದಿದ್ದರೆ
		eof = viminfo_readline(virp);
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '"':
		// When registers are in bar lines skip the old style register
		// lines.
		ಆದರೆ (virp->vir_ಆವೃತ್ತಿ < VIMINFO_ಆವೃತ್ತಿ_WITH_REGISTERS)
		    eof = read_viminfo_ರಿಜಿಸ್ಟರ್(virp, forceit);
		ಇಲ್ಲದಿದ್ದರೆ
		    ಮಾಡು
		    {
			eof = viminfo_readline(virp);
		    } ಆಗ (!eof && (virp->vir_ಸಾಲು[0] == TAB
						|| virp->vir_ಸಾಲು[0] == '<'));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '/':	    // Search string
	    ಸ್ಥಿತಿ '&':	    // Substitute search string
	    ಸ್ಥಿತಿ '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_ಹುಡುಕು_ನಮೂನೆ(virp, forceit, writing);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '$':
		eof = read_viminfo_sub_string(virp, forceit);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ':':
	    ಸ್ಥಿತಿ '?':
	    ಸ್ಥಿತಿ '=':
	    ಸ್ಥಿತಿ '@':
		// When history is in bar lines skip the old style history
		// lines.
		ಆದರೆ (virp->vir_ಆವೃತ್ತಿ < VIMINFO_ಆವೃತ್ತಿ_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		ಇಲ್ಲದಿದ್ದರೆ
		    eof = viminfo_readline(virp);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '-':
	    ಸ್ಥಿತಿ '\'':
		// When file marks are in bar lines skip the old style lines.
		ಆದರೆ (virp->vir_ಆವೃತ್ತಿ < VIMINFO_ಆವೃತ್ತಿ_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		ಇಲ್ಲದಿದ್ದರೆ
		    eof = viminfo_readline(virp);
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಆದರೆ (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_ಸಾಲು))
		    eof = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    eof = viminfo_readline(virp);
		ನಿಲ್ಲಿಸು;
	}
    }

    // Finish reading history items.
    ಆದರೆ (!writing)
	ಮುಕ್ತಾಯ_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_ಹೆಸರುಗಳು(buf);

    ಹಿಂತಿರುಗಿ eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_viminfo(FILE *fp_ಒಳ, FILE *fp_ಹೊರಹೋಗು, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ		eof = FALSE;
    vir_T	vir;
    ಪೂರ್ಣಾಂಕ		ವಿಲೀನ = FALSE;
    ಪೂರ್ಣಾಂಕ		do_copy_marks = FALSE;
    garray_T	buflist;

    ಆದರೆ ((vir.vir_ಸಾಲು = alloc(LSIZE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    vir.vir_fd = fp_ಒಳ;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, ಗಾತ್ರ(char_u *), 100);
    vir.vir_ಆವೃತ್ತಿ = -1;

    ಆದರೆ (fp_ಒಳ != ಖಾಲಿ)
    {
	ಆದರೆ (flags & VIF_WANT_INFO)
	{
	    ಆದರೆ (fp_ಹೊರಹೋಗು != ಖಾಲಿ)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_ಮೇಲೆ_ಗಮ್ಯ_marks(&vir,
					 flags & VIF_FORCEIT, fp_ಹೊರಹೋಗು != ಖಾಲಿ);
	    ವಿಲೀನ = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags != 0)
	    // Skip info, find start of marks
	    ಆಗ (!(eof = viminfo_readline(&vir))
		    && vir.vir_ಸಾಲು[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    ಆದರೆ (fp_ಹೊರಹೋಗು != ಖಾಲಿ)
    {
	// Write the info:
	fprintf(fp_ಹೊರಹೋಗು, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_ಆವೃತ್ತಿ_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_ಹೊರಹೋಗು);
	write_viminfo_ಆವೃತ್ತಿ(fp_ಹೊರಹೋಗು);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_ಹೊರಹೋಗು);
	fprintf(fp_ಹೊರಹೋಗು, "*encoding=%s\n\n", p_enc);
	write_viminfo_ಹುಡುಕು_ನಮೂನೆ(fp_ಹೊರಹೋಗು);
	write_viminfo_sub_string(fp_ಹೊರಹೋಗು);
	write_viminfo_history(fp_ಹೊರಹೋಗು, ವಿಲೀನ);
	write_viminfo_registers(fp_ಹೊರಹೋಗು);
	ಮುಕ್ತಾಯ_viminfo_registers();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	write_viminfo_varlist(fp_ಹೊರಹೋಗು);
#ಆದರೆ_ಅಂತ್ಯ
	write_viminfo_filemarks(fp_ಹೊರಹೋಗು);
	ಮುಕ್ತಾಯ_viminfo_marks();
	write_viminfo_bufferlist(fp_ಹೊರಹೋಗು);
	write_viminfo_barlines(&vir, fp_ಹೊರಹೋಗು);

	ಆದರೆ (do_copy_marks)
	    ga_init2(&buflist, ಗಾತ್ರ(buf_T *), 50);
	write_viminfo_marks(fp_ಹೊರಹೋಗು, do_copy_marks ? &buflist : ಖಾಲಿ);
    }

    ಆದರೆ (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_ಹೊರಹೋಗು, &buflist, eof, flags);
	ಆದರೆ (fp_ಹೊರಹೋಗು != ಖಾಲಿ)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_ಸಾಲು);
    ಆದರೆ (vir.vir_conv.vc_type != CONV_NONE)
	convert_ಸಿದ್ಧತೆ(&vir.vir_conv, ಖಾಲಿ, ಖಾಲಿ);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    ಪೂರ್ಣಾಂಕ
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    ಪೂರ್ಣಾಂಕ		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    ಆದರೆ (no_viminfo())
	ಹಿಂತಿರುಗಿ FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    fp = mch_fopen((ಅಕ್ಷರ *)fname, READBIN);

    ಆದರೆ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == ಖಾಲಿ ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    ಆದರೆ (fp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_ಬಹುಲಕ))
    {
	fclose(fp);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, ಖಾಲಿ, flags);

    fclose(fp);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    ಶೂನ್ಯ
write_viminfo(char_u *file, ಪೂರ್ಣಾಂಕ forceit)
{
    char_u	*fname;
    FILE	*fp_ಒಳ = ಖಾಲಿ;	// input viminfo file, if any
    FILE	*fp_ಹೊರಹೋಗು = ಖಾಲಿ;	// output viminfo file
    char_u	*tempname = ಖಾಲಿ;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#ಆದರೆ defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಪೂರ್ಣಾಂಕ		shortname = FALSE;	// use 8.3 file name
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಪೂರ್ಣಾಂಕ		hidden = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (no_viminfo())
	ಹಿಂತಿರುಗಿ;

    fname = viminfo_filename(file);	// may set to default if NULL
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    fp_ಒಳ = mch_fopen((ಅಕ್ಷರ *)fname, READBIN);
    ಆದರೆ (fp_ಒಳ == ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ fd;

	// if it does exist, but we can't read it, don't try writing
	ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st_new) == 0)
	    ಹೋಗು ಕೊನೆ;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((ಅಕ್ಷರ *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	ಆದರೆ (fd < 0)
	    ಹೋಗು ಕೊನೆ;
	fp_ಹೊರಹೋಗು = fdopen(fd, WRITEBIN);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	ಆದರೆ (mch_fstat(fileno(fp_ಒಳ), &st_old) < 0
		|| S_ISDIR(st_old.st_ಬಹುಲಕ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != ಬೇರು_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_ಬಹುಲಕ & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_ಬಹುಲಕ & 0020)
				    : (st_old.st_ಬಹುಲಕ & 0002))))
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    ಪೂರ್ಣಾಂಕ	tt = ಸಂದೇಶ_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    ಸಂದೇಶ_didany = tt;
	    fclose(fp_ಒಳ);
	    ಹೋಗು ಕೊನೆ;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	// Get the file attributes of the existing viminfo file.
	hidden = mch_ishidden(fname);
#ಆದರೆ_ಅಂತ್ಯ

	// Make tempname, find one that does not exist yet.
	// Beware of a race condition: If someone logs out and all Vim
	// instances exit at the same time a temp file might be created between
	// stat() and open().  Use mch_open() with O_EXCL to avoid that.
	// May try twice: Once normal and once with shortname set, just in
	// case somebody puts his viminfo file in an 8.3 filesystem.
	ಗಾಗಿ (;;)
	{
	    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
				    shortname,
#ಇಲ್ಲದಿದ್ದರೆ
				    FALSE,
#ಆದರೆ_ಅಂತ್ಯ
				    fname,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
				    (char_u *)"-tmp",
#ಇಲ್ಲದಿದ್ದರೆ
				    (char_u *)".tmp",
#ಆದರೆ_ಅಂತ್ಯ
				    FALSE);
	    ಆದರೆ (tempname == ಖಾಲಿ)		// out of memory
		ನಿಲ್ಲಿಸು;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    ಆದರೆ (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    ಗಾಗಿ (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		ಆದರೆ (mch_stat((ಅಕ್ಷರ *)tempname, &st_new) == 0)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    ಆದರೆ (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_ಹೊರಹೋಗು = mch_fopen((ಅಕ್ಷರ *)tempname, WRITEBIN);
		    (ಶೂನ್ಯ)umask(umask_save);
#ಇಲ್ಲದಿದ್ದರೆ
		    ಪೂರ್ಣಾಂಕ	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((ಅಕ್ಷರ *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(ಪೂರ್ಣಾಂಕ)((st_old.st_ಬಹುಲಕ & 0777) | 0600));
		    (ಶೂನ್ಯ)umask(umask_save);
# ಇಲ್ಲದಿದ್ದರೆ
		    fd = mch_open((ಅಕ್ಷರ *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    ಆದರೆ (fd < 0)
		    {
			fp_ಹೊರಹೋಗು = ಖಾಲಿ;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			ಆದರೆ (errno != EEXIST)
			    ನಿಲ್ಲಿಸು;
# endif
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			fp_ಹೊರಹೋಗು = fdopen(fd, WRITEBIN);
#ಆದರೆ_ಅಂತ್ಯ // VMS
		    ಆದರೆ (fp_ಹೊರಹೋಗು != ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;
		}

		// Assume file exists, try again with another name.
		ಆದರೆ (ಮುಂದಿನ_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_ತಾತ್ಕಾಲಿಕ_files_like_str), tempname);
		    ನಿಲ್ಲಿಸು;
		}
		*wp = ಮುಂದಿನ_char;
		--ಮುಂದಿನ_char;
	    }

	    ಆದರೆ (tempname != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    // continue if shortname was set
	}

#ಆದರೆ defined(UNIX) && defined(HAVE_FCHOWN)
	ಆದರೆ (tempname != ಖಾಲಿ && fp_ಹೊರಹೋಗು != ಖಾಲಿ)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)tempname, &tmp_st) >= 0)
	    {
		ಆದರೆ (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_ಹೊರಹೋಗು), st_old.st_uid, -1);
		ಆದರೆ (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_ಹೊರಹೋಗು), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (ಶೂನ್ಯ)mch_setperm(tempname, 0600);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// can't stat the file, set conservative permissions
		(ಶೂನ್ಯ)mch_setperm(tempname, 0600);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Check if the new viminfo file can be written to.
    ಆದರೆ (fp_ಹೊರಹೋಗು == ಖಾಲಿ)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_ಒಳ == ಖಾಲಿ || tempname == ಖಾಲಿ) ? fname : tempname);
	ಆದರೆ (fp_ಒಳ != ಖಾಲಿ)
	    fclose(fp_ಒಳ);
	ಹೋಗು ಕೊನೆ;
    }

    ಆದರೆ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_ಒಳ, fp_ಹೊರಹೋಗು, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    ಆದರೆ (fclose(fp_ಹೊರಹೋಗು) == EOF)
	++viminfo_errcnt;

    ಆದರೆ (fp_ಒಳ != ಖಾಲಿ)
    {
	fclose(fp_ಒಳ);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	ಆದರೆ (viminfo_errcnt == 0)
	{
	    ಆದರೆ (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_ಗಮ್ಯ_str), fname);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hidden)
		mch_hide(fname);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

ಕೊನೆ:
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
    ಆದರೆ (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    ಆದರೆ (eap->cmdidx == CMD_rviminfo)
    {
	ಆದರೆ (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_VIMINFO
