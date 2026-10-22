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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u	*vir_ଧାଡ଼ି;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bv_len;		// length of bv_string
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bv_allocated;	// bv_string was allocated
} bval_T;

#ଯଦି defined(FEAT_VIMINFO)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    ସ୍ଥିର char_u *
find_viminfo_parameter(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    char_u  *p;

    ପାଇଁ (p = p_viminfo; *p; ++p)
    {
	ଯଦି (*p == type)
	    ଫେରନ୍ତୁ p + 1;
	ଯଦି (*p == 'n')		    // 'n' is always the last one
	    ଭାଙ୍ଗ;
	p = vim_strchr(p, ',');	    // skip until next ','
	ଯଦି (p == ଖାଲି)		    // hit the end without finding parameter
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_viminfo_parameter(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    ଯଦି (p != ଖାଲି && VIM_ISDIGIT(*p))
	ଫେରନ୍ତୁ atoi((ଅକ୍ଷର *)p);
    ଫେରନ୍ତୁ -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    ସ୍ଥିର char_u *
viminfo_filename(char_u *file)
{
    ଯଦି (file == ଖାଲି || *file == NUL)
    {
	size_t  len;

	ଯଦି (*p_viminfofile != NUL)
	    file = p_viminfofile;
	ଅନ୍ୟଥା ଯଦି ((file = find_viminfo_parameter('n')) == ଖାଲି || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    ଯଦି (mch_getenv((char_u *)"SYS$LOGIN") == ଖାଲି)
# ଅନ୍ୟଥା
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    ଯଦି (STRCMP(vim_getenv((char_u *)"HOME", ଖାଲି), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == ଖାଲି)
#  ଅନ୍ୟଥା
	    ଯଦି (mch_getenv((char_u *)"HOME") == ଖାଲି)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		ଯଦି (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		ଅନ୍ୟଥା
		    file = (char_u *)VIMINFO_FILE;
	    }
	    ଅନ୍ୟଥା
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	ଫେରନ୍ତୁ vim_strnsave(file, len);
    }

    ଫେରନ୍ତୁ vim_strsave(file);
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
    ସ୍ଥିର ଶୂନ୍ୟ
viminfo_writestring(FILE *fd, char_u *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;

    ପାଇଁ (s = p; *s != NUL; ++s)
    {
	ଯଦି (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    ଯଦି (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    ଯେତେବେଳେ ((c = *p++) != NUL)
    {
	ଯଦି (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    ଯଦି (c == '\n')
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
barline_writestring(FILE *fd, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା remaining_start)
{
    char_u *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    remaining = remaining_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = 2;

    // Count the number of characters produced, including quotes.
    ପାଇଁ (p = s; *p != NUL; ++p)
    {
	ଯଦି (*p == NL)
	    len += 2;
	ଅନ୍ୟଥା ଯଦି (*p == '"' || *p == '\\')
	    len += 2;
	ଅନ୍ୟଥା
	    ++len;
    }
    ଯଦି (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    ପାଇଁ (p = s; *p != NUL; ++p)
    {
	ଯଦି (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	ଅନ୍ୟଥା
	    putc(*p, fd);
	--remaining;

	ଯଦି (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    ଫେରନ୍ତୁ remaining - 2;
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
    ସ୍ଥିର char_u *
viminfo_readstring(
    vir_T	*virp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off,		    // offset for virp->vir_line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		convert)	    // convert the string
{
    char_u	*retval = ଖାଲି;
    char_u	*s, *d;
    long	len;

    ଯଦି (virp->vir_ଧାଡ଼ି[off] == Ctrl_V && vim_isdigit(virp->vir_ଧାଡ଼ି[off + 1]))
    {
	len = atol((ଅକ୍ଷର *)virp->vir_ଧାଡ଼ି + off + 1);
	ଯଦି (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	ଯଦି (retval == ଖାଲି)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (ଶୂନ୍ୟ)vim_fgets(virp->vir_ଧାଡ଼ି, 10, virp->vir_fd);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	(ଶୂନ୍ୟ)vim_fgets(retval, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    ଅନ୍ୟଥା
    {
	retval = vim_strsave(virp->vir_ଧାଡ଼ି + off);
	ଯଦି (retval == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    ଯେତେବେଳେ (*s != NUL && *s != '\n')
    {
	ଯଦି (s[0] == Ctrl_V && s[1] != NUL)
	{
	    ଯଦି (s[1] == 'n')
		*d++ = '\n';
	    ଅନ୍ୟଥା
		*d++ = Ctrl_V;
	    s += 2;
	}
	ଅନ୍ୟଥା
	    *d++ = *s++;
    }
    *d = NUL;

    ଯଦି (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, ଖାଲି);
	ଯଦି (d != ଖାଲି)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
viminfo_readline(vir_T *virp)
{
    ଫେରନ୍ତୁ vim_fgets(virp->vir_ଧାଡ଼ି, LSIZE, virp->vir_fd);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo_bufferlist(
    vir_T	*virp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		writing)
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
    ଯଦି (xline != ଖାଲି && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != ଖାଲି)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	ଯଦି (tab != ଖାଲି)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((ଅକ୍ଷର *)tab);
	    tab = vim_strrchr(xline, '\t');
	    ଯଦି (tab != ଖାଲି)
	    {
		*tab++ = '\0';
		lnum = atol((ଅକ୍ଷର *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	ଯଦି (buf != ଖାଲି)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    ଫେରନ୍ତୁ viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    part_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval = FALSE;

    name = home_replace_save(ଖାଲି, name);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, ଆକାର(part), ", ");
	ଯଦି (part[0] == 'r')
	{
	    ଯଦି (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
    }
    vim_free(name);
    ଫେରନ୍ତୁ retval;
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_buffers;

    ଯଦି (find_viminfo_parameter('%') == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define ଧାଡ଼ି_BUF_LEN (MAXPATHL + 40)
    ଧାଡ଼ି = alloc(ଧାଡ଼ି_BUF_LEN);
    ଯଦି (ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	ଯଦି (buf->b_fname == ଖାଲି
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    ଜାରି;

	ଯଦି (max_buffers-- == 0)
	    ଭାଙ୍ଗ;
	putc('%', fp);
	linelen = home_replace(ଖାଲି, buf->b_ffname, ଧାଡ଼ି, MAXPATHL, TRUE);
	vim_snprintf((ଅକ୍ଷର *)ଧାଡ଼ି + linelen, ଧାଡ଼ି_BUF_LEN - linelen, "\t%ld\t%d",
			(long)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, ଧାଡ଼ି);
    }
    vim_free(ଧାଡ଼ି);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
ସ୍ଥିର histentry_T *viminfo_history[HIST_ଗଣନା] =
					       {ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି};
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	viminfo_hisidx[HIST_ଗଣନା] = {0, 0, 0, 0, 0};
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	viminfo_hislen[HIST_ଗଣନା] = {0, 0, 0, 0, 0};
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hist_type2char(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    use_question)	    // use '?' instead of '/'
{
    ଯଦି (type == HIST_CMD)
	ଫେରନ୍ତୁ ':';
    ଯଦି (type == HIST_ସନ୍ଧାନ)
    {
	ଯଦି (use_question)
	    ଫେରନ୍ତୁ '?';
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ '/';
    }
    ଯଦି (type == HIST_ପଦସମଷ୍ଟି)
	ଫେରନ୍ତୁ '=';
    ଫେରନ୍ତୁ '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prepare_viminfo_history(ପୂର୍ଣ୍ଣସଂଖ୍ୟା asklen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writing)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    ଯଦି (asklen > hislen)
	asklen = hislen;

    ପାଇଁ (type = 0; type < HIST_ଗଣନା; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	ପାଇଁ (i = 0, ଅଙ୍କ = 0; i < hislen; i++)
	    ଯଦି (histentry[i].hisstr == ଖାଲି || histentry[i].viminfo)
		ଅଙ୍କ++;
	len = asklen;
	ଯଦି (ଅଙ୍କ > len)
	    len = ଅଙ୍କ;
	ଯଦି (len <= 0)
	    viminfo_history[type] = ଖାଲି;
	ଅନ୍ୟଥା
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	ଯଦି (viminfo_history[type] == ଖାଲି)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo_history(vir_T *virp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writing)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;
    long_u	len;
    char_u	*val = ଖାଲି;
    char_u	*p;

    type = hist_char2type(virp->vir_ଧାଡ଼ି[0]);
    ଯଦି (viminfo_hisidx[type] >= viminfo_hislen[type])
	ଯାଅ done;

    val = viminfo_readstring(virp, 1, TRUE);
    ଯଦି (val == ଖାଲି || *val == NUL)
	ଯାଅ done;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sep = (*val == ' ' ? NUL : *val);

    ଯଦି (in_history(type, val + (type == HIST_ସନ୍ଧାନ), viminfo_add_at_front,
								sep, writing))
	ଯାଅ done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    ଯଦି (type == HIST_ସନ୍ଧାନ)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	ଯଦି (p == ଖାଲି)
	    ଯାଅ done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    ଅନ୍ୟଥା
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	ଯଦି (p == ଖାଲି)
	    ଯାଅ done;

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
    ଫେରନ୍ତୁ viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_viminfo_history(
	garray_T    *ମୂଲ୍ୟଗୁଡ଼ିକ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    writing)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)ମୂଲ୍ୟଗୁଡ଼ିକ->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	ଫେରନ୍ତୁ;

    type = vp[0].bv_nr;
    ଯଦି (type >= HIST_ଗଣନା)
	ଫେରନ୍ତୁ;

    ଯଦି (viminfo_hisidx[type] >= viminfo_hislen[type])
	ଫେରନ୍ତୁ;

    val = vp[3].bv_string;
    ଯଦି (val == ଖାଲି || *val == NUL)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sep = type == HIST_ସନ୍ଧାନ && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା overwrite = FALSE;

    ଯଦି (in_history(type, val, viminfo_add_at_front, sep, writing))
	ଫେରନ୍ତୁ;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    ପାଇଁ (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	ଯଦି (STRCMP(val, p) == 0
		&& (type != HIST_ସନ୍ଧାନ || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    ଭାଙ୍ଗ;
	}
    }

    ଯଦି (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    ଅନ୍ୟଥା
	len = 0; // for picky compilers
    ଯଦି (p != ଖାଲି)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	ଯଦି (!overwrite)
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
    ସ୍ଥିର ଶୂନ୍ୟ
concat_history(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*hisidx = get_hisidx(type);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    ଯଦି (idx >= hislen)
	idx -= hislen;
    ଅନ୍ୟଥା ଯଦି (idx < 0)
	idx = hislen - 1;
    ଯଦି (viminfo_add_at_front)
	*hisidx = idx;
    ଅନ୍ୟଥା
    {
	ଯଦି (*hisidx == -1)
	    *hisidx = hislen - 1;
	କର
	{
	    ଯଦି (histentry[idx].hisstr != ଖାଲି || histentry[idx].viminfo)
		ଭାଙ୍ଗ;
	    ଯଦି (++idx == hislen)
		idx = 0;
	} ଯେତେବେଳେ (idx != *hisidx);
	ଯଦି (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    ପାଇଁ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	ଯଦି (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    ପାଇଁ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sort_hist(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    ଯଦି (p1->time_set < p2->time_set) ଫେରନ୍ତୁ -1;
    ଯଦି (p1->time_set > p2->time_set) ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    ସ୍ଥିର ଶୂନ୍ୟ
merge_history(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*hisidx = get_hisidx(type);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    ଯଦି (tot_hist == ଖାଲି || new_hist == ଖାଲି)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	ଫେରନ୍ତୁ;
    }
    ପାଇଁ (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    ପାଇଁ (i = 0; i < hislen; i++)
	ଯଦି (histentry[i].hisstr != ଖାଲି)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((ଶୂନ୍ୟ *)tot_hist, (size_t)len, ଆକାର(histentry_T *), sort_hist);

    // Keep the newest ones.
    ପାଇଁ (i = 0; i < hislen; i++)
    {
	ଯଦି (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = ଖାଲି;
	    tot_hist[i]->hisstrlen = 0;
	    ଯଦି (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	ଅନ୍ୟଥା
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    ପାଇଁ (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    ପାଇଁ (i = 0; i < hislen; i++)
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
    ସ୍ଥିର ଶୂନ୍ୟ
finish_viminfo_history(vir_T *virp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    ପାଇଁ (type = 0; type < HIST_ଗଣନା; ++type)
    {
	ଯଦି (get_histentry(type) == ଖାଲି)
	    ଜାରି;

	ଯଦି (merge)
	    merge_history(type);
	ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_history(FILE *fp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା merge)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ_saved;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     round;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    hislen;

    init_history();
    hislen = get_hislen();
    ଯଦି (hislen == 0)
	ଫେରନ୍ତୁ;
    ପାଇଁ (type = 0; type < HIST_ଗଣନା; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *hisidx = get_hisidx(type);

	ଅଙ୍କ_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	ଯଦି (ଅଙ୍କ_saved == 0)
	    ଜାରି;
	ଯଦି (ଅଙ୍କ_saved < 0)  // Use default
	    ଅଙ୍କ_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_ସନ୍ଧାନ ? _("Search String") :
			    type == HIST_ପଦସମଷ୍ଟି ? _("Expression") :
			    type == HIST_ନିବେଶ ? _("Input Line") :
					_("Debug Line"));
	ଯଦି (ଅଙ୍କ_saved > hislen)
	    ଅଙ୍କ_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	ପାଇଁ (round = 1; round <= 2; ++round)
	{
	    ଯଦି (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    ଅନ୍ୟଥା ଯଦି (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    ଅନ୍ୟଥା
		// empty list
		i = -1;
	    ଯଦି (i >= 0)
		ଯେତେବେଳେ (ଅଙ୍କ_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c = NUL;

		    ଯଦି (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (viminfo_history[type] == ଖାଲି)
			{
			    p = ଖାଲି;
			    plen = 0;
			    timestamp = 0;
			}
			ଅନ୍ୟଥା
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    ଯଦି (p != ଖାଲି && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--ଅଙ୍କ_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			ଯଦି (type == HIST_ସନ୍ଧାନ)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    ଅକ୍ଷର    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    ଯଦି (c == NUL)
				cbuf[0] = NUL;
			    ଅନ୍ୟଥା
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    ଯଦି (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			ଯଦି (--i < 0)
			    i = hislen - 1;
			ଯଦି (i == *hisidx)
			    ଭାଙ୍ଗ;
		    }
		    ଅନ୍ୟଥା
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	ପାଇଁ (i = 0; i < viminfo_hisidx[type]; ++i)
	    ଯଦି (viminfo_history[type] != ଖାଲି)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_barlines(vir_T *virp, FILE *fp_out)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    garray_T	*gap = &virp->vir_barlines;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seen_useful = FALSE;
    ଅକ୍ଷର	*ଧାଡ଼ି;

    ଯଦି (gap->ga_len <= 0)
	ଫେରନ୍ତୁ;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	ଧାଡ଼ି = ((ଅକ୍ଷର **)(gap->ga_data))[i];
	ଯଦି (seen_useful || ଧାଡ଼ି[1] != '<')
	{
	    fputs(ଧାଡ଼ି, fp_out);
	    seen_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
barline_parse(vir_T *virp, char_u *text, garray_T *ମୂଲ୍ୟଗୁଡ଼ିକ)
{
    char_u  *p = text;
    char_u  *nextp = ଖାଲି;
    char_u  *buf = ଖାଲି;
    bval_T  *ମୂଲ୍ୟ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    allocated = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    eof;
    char_u  *sconv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    converted;

    ଯେତେବେଳେ (*p == ',')
    {
	++p;
	ଯଦି (ga_grow(ମୂଲ୍ୟଗୁଡ଼ିକ, 1) == FAIL)
	    ଭାଙ୍ଗ;
	ମୂଲ୍ୟ = (bval_T *)(ମୂଲ୍ୟଗୁଡ଼ିକ->ga_data) + ମୂଲ୍ୟଗୁଡ଼ିକ->ga_len;

	ଯଦି (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    ଯଦି (!allocated)
	    {
		ପାଇଁ (i = 0; i < ମୂଲ୍ୟଗୁଡ଼ିକ->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(ମୂଲ୍ୟଗୁଡ଼ିକ->ga_data) + i;

		    ଯଦି (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    ଯଦି (vim_isdigit(p[1]))
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
		ଯଦି (buf == ଖାଲି)
		    ଫେରନ୍ତୁ TRUE;
		p = buf;
		ପାଇଁ (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    ଯଦି (eof || virp->vir_ଧାଡ଼ି[0] != '|'
						  || virp->vir_ଧାଡ଼ି[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			ଫେରନ୍ତୁ eof || virp->vir_ଧାଡ଼ି[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_ଧାଡ଼ି);
		    ଯେତେବେଳେ (n > 2 && (virp->vir_ଧାଡ଼ି[n - 1] == NL
					     || virp->vir_ଧାଡ଼ି[n - 1] == CAR))
			--n;
		    n -= 2;
		    ଯଦି (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_ଧାଡ଼ି + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_ଧାଡ଼ି + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    ଅନ୍ୟଥା
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		ଯଦି (eof || virp->vir_ଧାଡ଼ି[0] != '|'
					      || virp->vir_ଧାଡ଼ି[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    ଫେରନ୍ତୁ eof || virp->vir_ଧାଡ଼ି[0] == '|';
		p = virp->vir_ଧାଡ଼ି + 2;
	    }
	}

	ଯଦି (SAFE_isdigit(*p))
	{
	    ମୂଲ୍ୟ->bv_type = BVAL_NR;
	    ମୂଲ୍ୟ->bv_nr = getdigits(&p);
	    ++ମୂଲ୍ୟଗୁଡ଼ିକ->ga_len;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '"')
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    ଯେତେବେଳେ (*p != '"')
	    {
		ଯଦି (*p == NL || *p == NUL)
		    ଫେରନ୍ତୁ TRUE;  // syntax error, drop the value
		ଯଦି (*p == '\\')
		{
		    ++p;
		    ଯଦି (*p == 'n')
			s[len++] = '\n';
		    ଅନ୍ୟଥା
			s[len++] = *p;
		    ++p;
		}
		ଅନ୍ୟଥା
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    ମୂଲ୍ୟ->bv_tofree = ଖାଲି;
	    ଯଦି (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, ଖାଲି);
		ଯଦି (sconv != ଖାଲି)
		{
		    ଯଦି (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			ମୂଲ୍ୟ->bv_tofree = buf;
		    s = sconv;
		    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    ଯଦି (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    ମୂଲ୍ୟ->bv_string = s;
	    ମୂଲ୍ୟ->bv_type = BVAL_STRING;
	    ମୂଲ୍ୟ->bv_len = len;
	    ମୂଲ୍ୟ->bv_allocated = allocated || converted;
	    ++ମୂଲ୍ୟଗୁଡ଼ିକ->ga_len;
	    ଯଦି (nextp != ଖାଲି)
	    {
		// values following a long string
		p = nextp;
		nextp = ଖାଲି;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (*p == ',')
	{
	    ମୂଲ୍ୟ->bv_type = BVAL_EMPTY;
	    ++ମୂଲ୍ୟଗୁଡ଼ିକ->ga_len;
	}
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
no_viminfo(ଶୂନ୍ୟ)
{
    // "vim -i NONE" does not read or write a viminfo file
    ଫେରନ୍ତୁ STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
viminfo_error(ଅକ୍ଷର *errnum, ଅକ୍ଷର *message, char_u *ଧାଡ଼ି)
{
    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, ଧାଡ଼ି, IOSIZE - STRLEN(IObuff) - 1);
    ଯଦି (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((ଅକ୍ଷର *)IObuff);
    ଯଦି (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_ଧାଡ଼ି, '=');
	ଯଦି (p != ଖାଲି)
	{
	    // remove trailing newline
	    ++p;
	    ପାଇଁ (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    ଫେରନ୍ତୁ viminfo_readline(virp);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo_varlist(vir_T *virp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writing)
{
    char_u	*tab;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type = VAR_ସଂଖ୍ୟା;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    ଯଦି (!writing && (find_viminfo_parameter('!') != ଖାଲି))
    {
	tab = vim_strchr(virp->vir_ଧାଡ଼ି + 1, '\t');
	ଯଦି (tab != ଖାଲି)
	{
	    *tab++ = '\0';	// isolate the variable name
	    ବାଛ (*tab)
	    {
		ସ୍ଥିତି 'S': type = VAR_STRING; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'F': type = VAR_FLOAT; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'D': type = VAR_DICT; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'L': type = VAR_LIST; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'B': type = VAR_BLOB; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'X': type = VAR_SPECIAL; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'T': type = VAR_TUPLE; ଭାଙ୍ଗ;
	    }

	    tab = vim_strchr(tab, '\t');
	    ଯଦି (tab != ଖାଲି)
	    {
		tv.v_type = type;
		ଯଦି (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tab - virp->vir_ଧାଡ଼ି + 1), TRUE);
		ଅନ୍ୟଥା ଯଦି (type == VAR_FLOAT)
		    (ଶୂନ୍ୟ)string2float(tab + 1, &tv.vval.v_float, FALSE);
		ଅନ୍ୟଥା
		{
		    tv.vval.v_ସଂଖ୍ୟା = atol((ଅକ୍ଷର *)tab + 1);
		    ଯଦି (type == VAR_SPECIAL && (tv.vval.v_ସଂଖ୍ୟା == VVAL_FALSE
					     || tv.vval.v_ସଂଖ୍ୟା == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		ଯଦି (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_ପଦସମଷ୍ଟି(tv.vval.v_string, ଖାଲି);

		    ଯଦି (etv == ଖାଲି)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    ଅନ୍ୟଥା
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		ଅନ୍ୟଥା ଯଦି (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    ଯଦି (blob == ଖାଲି)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    ଅନ୍ୟଥା
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_entry);
		set_var(virp->vir_ଧାଡ଼ି + 1, &tv, FALSE);
		restore_funccal();

		ଯଦି (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		ଅନ୍ୟଥା ଯଦି (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    ଫେରନ୍ତୁ viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    ଅକ୍ଷର	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ଯଦି (find_viminfo_parameter('!') == ଖାଲି)
	ଫେରନ୍ତୁ;

    fputs(_("\n# global variables:\n"), fp);

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)gvht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(gvht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ଯଦି (var_flavour(this_var->di_ଚାବି) == VAR_FLAVOUR_VIMINFO)
	    {
		ବାଛ (this_var->di_tv.v_type)
		{
		    ସ୍ଥିତି VAR_STRING:  s = "STR"; ଭାଙ୍ଗ;
		    ସ୍ଥିତି VAR_ସଂଖ୍ୟା:  s = "NUM"; ଭାଙ୍ଗ;
		    ସ୍ଥିତି VAR_FLOAT:   s = "FLO"; ଭାଙ୍ଗ;
		    ସ୍ଥିତି VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା	copyID = get_copyID();

			      s = "DIC";
			      ଯଦି (di != ଖାଲି && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, ଖାଲି, ଖାଲି)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ଜାରି;
			      ଭାଙ୍ଗ;
			  }
		    ସ୍ଥିତି VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା	copyID = get_copyID();

			      s = "LIS";
			      ଯଦି (l != ଖାଲି && !set_ref_in_list_ବସ୍ତୁଗୁଡ଼ିକ(
						       l, copyID, ଖାଲି, ଖାଲି)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ଜାରି;
			      ଭାଙ୍ଗ;
			  }
		    ସ୍ଥିତି VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା	copyID = get_copyID();

			      s = "TUP";
			      ଯଦି (tuple != ଖାଲି && !set_ref_in_tuple_ବସ୍ତୁଗୁଡ଼ିକ(
					       tuple, copyID, ଖାଲି, ଖାଲି)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  ଜାରି;
			      ଭାଙ୍ଗ;
			  }
		    ସ୍ଥିତି VAR_BLOB:    s = "BLO"; ଭାଙ୍ଗ;
		    ସ୍ଥିତି VAR_BOOL:    s = "XPL"; ଭାଙ୍ଗ;  // backwards compat.
		    ସ୍ଥିତି VAR_SPECIAL: s = "XPL"; ଭାଙ୍ଗ;

		    ସ୍ଥିତି VAR_UNKNOWN:
		    ସ୍ଥିତି VAR_ANY:
		    ସ୍ଥିତି VAR_VOID:
		    ସ୍ଥିତି VAR_FUNC:
		    ସ୍ଥିତି VAR_PARTIAL:
		    ସ୍ଥିତି VAR_JOB:
		    ସ୍ଥିତି VAR_CHANNEL:
		    ସ୍ଥିତି VAR_INSTR:
		    ସ୍ଥିତି VAR_CLASS:
		    ସ୍ଥିତି VAR_OBJECT:
		    ସ୍ଥିତି VAR_TYPEALIAS:
				      ଜାରି;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_ଚାବି, s);
		ଯଦି (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((ଅକ୍ଷର *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_ସଂଖ୍ୟା);
		    p = numbuf;
		    tofree = ଖାଲି;
		}
		ଅନ୍ୟଥା
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		ଯଦି (p != ଖାଲି)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo_sub_string(vir_T *virp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    ଯଦି (force || get_old_sub() == ଖାଲି)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    ଫେରନ୍ତୁ viminfo_readline(virp);
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    ଯଦି (get_viminfo_parameter('/') == 0 || old_sub == ଖାଲି)
	ଫେରନ୍ତୁ;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo_ସନ୍ଧାନ_pattern(vir_T *virp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writing)
{
    char_u	*lp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		magic = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no_scs = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off_ଧାଡ଼ି = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off_end = 0;
    long	off = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		setlast = FALSE;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	hlsearch_on = FALSE;
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
    lp = virp->vir_ଧାଡ଼ି;
    ଯଦି (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	ଯଦି (lp[1] == 'M')		// magic on
	    magic = TRUE;
	ଯଦି (lp[2] == 's')
	    no_scs = TRUE;
	ଯଦି (lp[3] == 'L')
	    off_ଧାଡ଼ି = TRUE;
	ଯଦି (lp[4] == 'E')
	    off_end = ସନ୍ଧାନ_END;
	lp += 5;
	off = getdigits(&lp);
    }
    ଯଦି (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    ଯଦି (lp[0] == '/')
	idx = RE_ସନ୍ଧାନ;
    ଅନ୍ୟଥା ଯଦି (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ଅନ୍ୟଥା ଯଦି (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    ଅନ୍ୟଥା ଯଦି (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    ଯଦି (idx >= 0)
    {
	spat = get_spat(idx);
	ଯଦି (force || spat->pat == ଖାଲି)
	{
	    val = viminfo_readstring(virp, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(lp - virp->vir_ଧାଡ଼ି + 1),
									TRUE);
	    ଯଦି (val != ଖାଲି)
	    {
		ଯଦି (!writing)
		{
		    set_last_ସନ୍ଧାନ_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.ଧାଡ଼ି = off_ଧାଡ଼ି;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
		    ଯଦି (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    ଫେରନ୍ତୁ viminfo_readline(virp);
}

    ସ୍ଥିର ଶୂନ୍ୟ
wvsp_one(
    FILE	*fp,	// file to write to
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,	// spats[] index
    ଅକ୍ଷର	*s,	// search pat
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    ଯଦି (spat->pat == ଖାଲି)
	ଫେରନ୍ତୁ;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.ଧାଡ଼ି ? 'L' : 'l',	// line offset
	    spat->off.end  ? 'E' : 'e',	// offset from end
	    spat->off.off,			// offset
	    get_spat_last_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_ସନ୍ଧାନ_pattern(FILE *fp)
{
    ଯଦି (get_viminfo_parameter('/') == 0)
	ଫେରନ୍ତୁ;

#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != ଖାଲି) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_ସନ୍ଧାନ, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

ସ୍ଥିର yankreg_T *y_read_regs = ଖାଲି;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prepare_viminfo_registers(ଶୂନ୍ୟ)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, ଅଙ୍କ_REGISTERS);
}

    ସ୍ଥିର ଶୂନ୍ୟ
finish_viminfo_registers(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;

    ଯଦି (y_read_regs == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_REGISTERS; ++i)
	ଯଦି (y_read_regs[i].y_array != ଖାଲି)
	{
	    ପାଇଁ (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo_register(vir_T *virp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eof;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_it = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		limit;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_prev = FALSE;
    char_u	*str;
    string_T	*array = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_ଧାଡ଼ି + 1;

    // If the line starts with "" this is the y_previous register.
    ଯଦି (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    ଯଦି (!ASCII_ISALNUM(*str) && *str != '-')
    {
	ଯଦି (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_ଧାଡ଼ି))
	    ଫେରନ୍ତୁ TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    ଯଦି (!force && y_current_p->y_array != ଖାଲି)
	do_it = FALSE;

    ଯଦି (*str == '@')
    {
	// "x@: register x used for @@
	ଯଦି (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    ଯଦି (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	ଯଦି (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	ଯଦି (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	ଅନ୍ୟଥା ଯଦି (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	ଅନ୍ୟଥା
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    ଯେତେବେଳେ (!(eof = viminfo_readline(virp))
		    && (virp->vir_ଧାଡ଼ି[0] == TAB || virp->vir_ଧାଡ଼ି[0] == '<'))
    {
	ଯଦି (do_it)
	{
	    ଯଦି (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * ଆକାର(string_T));

		ଯଦି (new_array == ଖାଲି)
		{
		    do_it = FALSE;
		    ଭାଙ୍ଗ;
		}
		ପାଇଁ (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    ଯଦି (str != ଖାଲି)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    ଅନ୍ୟଥା
		// error, don't store the result
		do_it = FALSE;
	}
    }

    ଯଦି (do_it)
    {
	// free y_array[]
	ପାଇଁ (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	ଯଦି (size == 0)
	{
	    y_current_p->y_array = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    ପାଇଁ (i = 0; i < size; i++)
	    {
		ଯଦି (y_current_p->y_array == ଖାଲି)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		ଅନ୍ୟଥା
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	// Free array[] if it was filled.
	ପାଇଁ (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    ଫେରନ୍ତୁ eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_viminfo_register(garray_T *ମୂଲ୍ୟଗୁଡ଼ିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    bval_T	*vp = (bval_T *)ମୂଲ୍ୟଗୁଡ଼ିକ->ga_data;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linecount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	ଫେରନ୍ତୁ;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    ଯଦି (name < 0 || name >= ଅଙ୍କ_REGISTERS)
	ଫେରନ୍ତୁ;
    type = vp[2].bv_nr;
    ଯଦି (type != MCHAR && type != MLINE && type != MBLOCK)
	ଫେରନ୍ତୁ;
    linecount = vp[3].bv_nr;
    ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ->ga_len < 6 + linecount)
	ଫେରନ୍ତୁ;
    width = vp[4].bv_nr;
    ଯଦି (width < 0)
	ଫେରନ୍ତୁ;

    ଯଦି (y_read_regs != ଖାଲି)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    ଅନ୍ୟଥା
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    ଯଦି (y_ptr->y_array != ଖାଲି && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	ଫେରନ୍ତୁ;

    ଯଦି (y_ptr->y_array != ଖାଲି)
	ପାଇଁ (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    ଯଦି (y_read_regs == ଖାଲି)
    {
	ଯଦି (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	ଯଦି ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    ଯଦି (linecount == 0)
    {
	y_ptr->y_array = ଖାଲି;
	ଫେରନ୍ତୁ;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    ଯଦି (y_ptr->y_array == ଖାଲି)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	ଫେରନ୍ତୁ;
    }
    ପାଇଁ (i = 0; i < linecount; i++)
    {
	ଯଦି (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (vp[i + 6].bv_type != BVAL_STRING)
	{
	    ମୁକ୍ତ(y_ptr->y_array);
	    y_ptr->y_array = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_registers(FILE *fp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    char_u	*type;
    char_u	c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ = get_viminfo_parameter('<');
    ଯଦି (max_ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ < 0)
	max_ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ = get_viminfo_parameter('"');
    ଯଦି (max_ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ == 0)
	ଫେରନ୍ତୁ;
    max_kbyte = get_viminfo_parameter('s');
    ଯଦି (max_kbyte == 0)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_REGISTERS; i++)
    {
#ଯଦି defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	ଯଦି (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    ଜାରି;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	ଯଦି (i == TILDE_REGISTER)
	    ଜାରି;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	ଯଦି (y_read_regs != ଖାଲି
		&& y_read_regs[i].y_array != ଖାଲି
		&& (y_regs_p[i].y_array == ଖାଲି ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	ଅନ୍ୟଥା ଯଦି (y_regs_p[i].y_array == ଖାଲି)
	    ଜାରି;
	ଅନ୍ୟଥା
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ = y_ptr->y_size;
	ଯଦି (ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ == 0
		|| (ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    ଜାରି;

	ଯଦି (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    ପାଇଁ (j = 0; j < ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    ଯଦି (len > (long)max_kbyte * 1024L)
		ଜାରି;
	}

	ବାଛ (y_ptr->y_type)
	{
	    ସ୍ଥିତି MLINE:
		type = (char_u *)"LINE";
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MCHAR:
		type = (char_u *)"CHAR";
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MBLOCK:
		type = (char_u *)"BLOCK";
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		ଭାଙ୍ଗ;
	}
	ଯଦି (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	ଯଦି (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	ଯଦି (max_ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ > 0 && ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ > max_ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
	    ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ = max_ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ;
	ପାଇଁ (j = 0; j < ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    ଯଦି (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    ଯଦି (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    ପାଇଁ (j = 0; j < ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ; j++)
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

ସ୍ଥିର xfmark_T *vi_namedfm = ଖାଲି;
ସ୍ଥିର xfmark_T *vi_jumplist = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vi_jumplist_len = 0;

    ସ୍ଥିର ଶୂନ୍ୟ
write_one_mark(FILE *fp_out, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, pos_T *pos)
{
    ଯଦି (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pos->col);
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    pos_T	pos;

    home_replace(ଖାଲି, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_ଭର୍ତ୍ତି);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    ପାଇଁ (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	ଯଦି (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    ପାଇଁ (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
skip_for_viminfo(buf_T *buf)
{
    ଫେରନ୍ତୁ bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_marks(FILE *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_mark_set;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
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
	ଯଦି (buf->b_marks_read)
	{
	    ଯଦି (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    ଅନ୍ୟଥା
	    {
		is_mark_set = FALSE;
		ପାଇଁ (i = 0; i < NMARKS; i++)
		    ଯଦି (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			ଭାଙ୍ଗ;
		    }
	    }
	    ଯଦି (is_mark_set && buf->b_ffname != ଖାଲି
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		ଯଦି (buflist == ଖାଲି)
		    write_buffer_marks(buf, fp_out);
		ଅନ୍ୟଥା ଯଦି (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c2)
{
    char_u	*name;

    ଯଦି (fm->fmark.mark.lnum == 0)	// not set
	ଫେରନ୍ତୁ;

    ଯଦି (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    ଅନ୍ୟଥା
	name = fm->fname;		// use name from .viminfo
    ଯଦି (name != ଖାଲି && *name != NUL)
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

    ଯଦି (fm->fmark.fnum != 0)
	vim_free(name);
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_viminfo_filemarks(FILE *fp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vi_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    ଯଦି (get_viminfo_parameter('f') == 0)
	ଫେରନ୍ତୁ;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    ପାଇଁ (i = 0; i < NMARKS; i++)
    {
	ଯଦି (vi_namedfm != ଖାଲି
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	ଅନ୍ୟଥା
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    ଯଦି (curbuf->b_ffname != ଖାଲି && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	ପାଇଁ (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    ଯଦି (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == ଖାଲି
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != ଖାଲି
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		ଭାଙ୍ଗ;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	ପାଇଁ ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = ଖାଲି;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    ପାଇଁ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != ଖାଲି ? &vi_namedfm[vi_idx] : ଖାଲି;

	ଯଦି (vi_fm != ଖାଲି
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ଅନ୍ୟଥା
	{
	    fm = &namedfm_p[idx++];
	    ଯଦି (vi_fm != ଖାଲି
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != ଖାଲି
			  && fm->fname != ଖାଲି
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
    ପାଇଁ (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : ଖାଲି;
	vi_fm = (vi_jumplist != ଖାଲି && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : ଖାଲି;
	ଯଦି (fm == ଖାଲି && vi_fm == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି (fm == ଖାଲି || (vi_fm != ଖାଲି && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ଅନ୍ୟଥା
	    --idx;
	ଯଦି (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != ଖାଲି
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    ଯଦି (buf1->b_last_used == buf2->b_last_used)
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    ସ୍ଥିର ଶୂନ୍ୟ
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_out,
    garray_T	*buflist,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eof,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    char_u	*ଧାଡ଼ି = virp->vir_ଧାଡ଼ି;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_marked_files;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		load_marks;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy_marks_out;
    char_u	*str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = ଖାଲି;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflist_used = 0;
    buf_T	*buflist_buf = ଖାଲି;

    ଯଦି ((name_buf = alloc(LSIZE)) == ଖାଲି)
	ଫେରନ୍ତୁ;
    *name_buf = NUL;

    ଯଦି (fp_out != ଖାଲି && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						ଆକାର(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    ଯଦି (fp_out == ଖାଲି && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	ଯଦି (list != ଖାଲି)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    ଅଙ୍କ_marked_files = get_viminfo_parameter('\'');
    ଯେତେବେଳେ (!eof && (ଗଣନା_2 < ଅଙ୍କ_marked_files || fp_out == ଖାଲି))
    {
	ଯଦି (ଧାଡ଼ି[0] != '>')
	{
	    ଯଦି (ଧାଡ଼ି[0] != '\n' && ଧାଡ଼ି[0] != '\r' && ଧାଡ଼ି[0] != '#')
	    {
		ଯଦି (viminfo_error("E576: ", _(e_nonr_missing_gt), ଧାଡ଼ି))
		    ଭାଙ୍ଗ;	// too many errors, return now
	    }
	    eof = vim_fgets(ଧାଡ଼ି, LSIZE, virp->vir_fd);
	    ଜାରି;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(ଧାଡ଼ି + 1);
	str = viminfo_readstring(virp, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(str - virp->vir_ଧାଡ଼ି), FALSE);
	ଯଦି (str == ଖାଲି)
	    ଜାରି;
	p = str + STRLEN(str);
	ଯେତେବେଳେ (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	ଯଦି (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	ଯଦି (list != ଖାଲି)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	ଯଦି (fp_out == ଖାଲି)
	{
	    ଯଦି ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != ଖାଲି)
	    {
		ଯଦି (*name_buf == NUL)	    // only need to do this once
		    home_replace(ଖାଲି, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		ଯଦି (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	ଅନ୍ୟଥା // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		ଯଦି (buf->b_ffname != ଖାଲି)
		{
		    home_replace(ଖାଲି, buf->b_ffname, name_buf, LSIZE, TRUE);
		    ଯଦି (fnamecmp(str, name_buf) == 0)
			ଭାଙ୍ଗ;
		}

	    // Copy marks if the buffer has not been loaded.
	    ଯଦି (buf == ଖାଲି || !buf->b_marks_read)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_read_ଧାଡ଼ି = FALSE;

		ଯଦି (buflist_buf != ଖାଲି)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    ଯଦି (!viminfo_readline(virp) && ଧାଡ଼ି[0] == TAB)
		    {
			did_read_ଧାଡ଼ି = TRUE;
			ଯଦି (ଧାଡ଼ି[1] == '*')
			{
			    long	ltime;

			    sscanf((ଅକ୍ଷର *)ଧାଡ଼ି + 2, "%ld ", &ltime);
			    ଯେତେବେଳେ ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				ଯଦି (++ଗଣନା_2 >= ଅଙ୍କ_marked_files)
				    ଭାଙ୍ଗ;
				ଯଦି (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = ଖାଲି;
				    ଭାଙ୍ଗ;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			ଅନ୍ୟଥା
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    ଯେତେବେଳେ (ଗଣନା_2 < ଅଙ୍କ_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++ଗଣନା_2;
			    }
			    buflist_buf = ଖାଲି;
			}

			ଯଦି (ଗଣନା_2 >= ଅଙ୍କ_marked_files)
			{
			    vim_free(str);
			    ଭାଙ୍ଗ;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		ଯଦି (did_read_ଧାଡ଼ି)
		    fputs((ଅକ୍ଷର *)ଧାଡ଼ି, fp_out);

		ଗଣନା_2++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	ଯେତେବେଳେ (!(eof = viminfo_readline(virp)) && ଧାଡ଼ି[0] == TAB)
	{
	    ଯଦି (load_marks)
	    {
		ଯଦି (ଧାଡ଼ି[1] != NUL)
		{
		    unsigned u;

		    sscanf((ଅକ୍ଷର *)ଧାଡ଼ି + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    ବାଛ (ଧାଡ଼ି[1])
		    {
			ସ୍ଥିତି '"': curbuf->b_last_cursor = pos; ଭାଙ୍ଗ;
			ସ୍ଥିତି '^': curbuf->b_last_ଭର୍ତ୍ତି = pos; ଭାଙ୍ଗ;
			ସ୍ଥିତି '.': curbuf->b_last_change = pos; ଭାଙ୍ଗ;
			ସ୍ଥିତି '+':
				  // changelist positions are stored oldest
				  // first
				  ଯଦି (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    ଆକାର(pos_T) * (JUMPLISTSIZE - 1));
				  ଅନ୍ୟଥା
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  ଭାଙ୍ଗ;

				  // Using the line number for the last-used
				  // timestamp.
			ସ୍ଥିତି '*': curbuf->b_last_used = pos.lnum; ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଯଦି ((i = ଧାଡ଼ି[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (copy_marks_out)
		fputs((ଅକ୍ଷର *)ଧାଡ଼ି, fp_out);
	}

	ଯଦି (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		ଯଦି (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    ଯଦି (flags & VIF_ONLY_CURBUF)
		ଭାଙ୍ଗ;
	}
    }

    ଯଦି (fp_out != ଖାଲି)
	// Write any remaining entries from buflist.
	ଯେତେବେଳେ (ଗଣନା_2 < ଅଙ୍କ_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++ଗଣନା_2;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    ଶୂନ୍ୟ
check_marks_read(ଶୂନ୍ୟ)
{
    ଯଦି (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != ଖାଲି)
	read_viminfo(ଖାଲି, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo_filemark(vir_T *virp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_ଧାଡ଼ି + 1;
    ଯଦି (*str <= 127
	    && ((*virp->vir_ଧାଡ଼ି == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_ଧାଡ଼ି == '-' && *str == '\'')))
    {
	ଯଦି (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    ଯଦି (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		ପାଇଁ (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = ଖାଲି;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	ଅନ୍ୟଥା
	    fm = &namedfm_p[*str - 'A'];
	ଯଦି (fm != ଖାଲି && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(str - virp->vir_ଧାଡ଼ି),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    ଫେରନ୍ତୁ vim_fgets(virp->vir_ଧାଡ଼ି, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prepare_viminfo_marks(ଶୂନ୍ୟ)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    ସ୍ଥିର ଶୂନ୍ୟ
finish_viminfo_marks(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (vi_namedfm != ଖାଲି)
    {
	ପାଇଁ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    ଯଦି (vi_jumplist != ଖାଲି)
    {
	ପାଇଁ (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
handle_viminfo_mark(garray_T *ମୂଲ୍ୟଗୁଡ଼ିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    bval_T	*vp = (bval_T *)ମୂଲ୍ୟଗୁଡ଼ିକ->ga_data;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = ଖାଲି;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	ଫେରନ୍ତୁ;

    name = vp[0].bv_nr;
    ଯଦି (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	ଫେରନ୍ତୁ;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    ଯଦି (lnum <= 0 || col < 0)
	ଫେରନ୍ତୁ;
    timestamp = (time_t)vp[3].bv_nr;

    ଯଦି (name == '\'')
    {
	ଯଦି (vi_jumplist != ଖାଲି)
	{
	    ଯଦି (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    // If we have a timestamp insert it in the right place.
	    ଯଦି (timestamp != 0)
	    {
		ପାଇଁ (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    ଯଦି (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			ଭାଙ୍ଗ;
		    }
		// idx cannot be zero now
		ଯଦି (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    ଅନ୍ୟଥା ଯଦି (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    ଅନ୍ୟଥା
		idx = -1;

	    ଯଦି (idx >= 0)
	    {
		ଯଦି (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    ପାଇଁ (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		ଅନ୍ୟଥା
		{
		    // Move newer entries forward.
		    ପାଇଁ (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = ଖାଲି;
		fm->time_set = 0;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	ଯଦି (VIM_ISDIGIT(name))
	{
	    ଯଦି (vi_namedfm != ଖାଲି)
		idx = name - '0' + NMARKS;
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		ପାଇଁ (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    ଯଦି (namedfm_p[idx].time_set < timestamp)
			ଭାଙ୍ଗ;
		ଯଦି (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    ଫେରନ୍ତୁ;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		ପାଇଁ ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = ଖାଲି;
	    }
	}
	ଅନ୍ୟଥା
	    idx = name - 'A';
	ଯଦି (vi_namedfm != ଖାଲି)
	    fm = &vi_namedfm[idx];
	ଅନ୍ୟଥା
	    fm = &namedfm_p[idx];
    }

    ଯଦି (fm != ଖାଲି)
    {
	ଯଦି (vi_namedfm != ଖାଲି || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    ଯଦି (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = ଖାଲି;
	    }
	    ଅନ୍ୟଥା
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo_barline(vir_T *virp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା got_encoding, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writing)
{
    char_u	*p = virp->vir_ଧାଡ଼ି + 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bartype;
    garray_T	ମୂଲ୍ୟଗୁଡ଼ିକ;
    bval_T	*vp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    ଯଦି (*p == '<')
    {
	// Continuation line of an unrecognized item.
	ଯଦି (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_ଧାଡ଼ି);
    }
    ଅନ୍ୟଥା
    {
	ga_init2(&ମୂଲ୍ୟଗୁଡ଼ିକ, ଆକାର(bval_T), 20);
	bartype = getdigits(&p);
	ବାଛ (bartype)
	{
	    ସ୍ଥିତି BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		ଯଦି (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &ମୂଲ୍ୟଗୁଡ଼ିକ);
		    vp = (bval_T *)ମୂଲ୍ୟଗୁଡ଼ିକ.ga_data;
		    ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &ମୂଲ୍ୟଗୁଡ଼ିକ);
		handle_viminfo_history(&ମୂଲ୍ୟଗୁଡ଼ିକ, writing);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &ମୂଲ୍ୟଗୁଡ଼ିକ);
		handle_viminfo_register(&ମୂଲ୍ୟଗୁଡ଼ିକ, force);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି BARTYPE_MARK:
		read_next = barline_parse(virp, p, &ମୂଲ୍ୟଗୁଡ଼ିକ);
		handle_viminfo_mark(&ମୂଲ୍ୟଗୁଡ଼ିକ, force);
		ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		// copy unrecognized line (for future use)
		ଯଦି (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_ଧାଡ଼ି);
	}
	ପାଇଁ (i = 0; i < ମୂଲ୍ୟଗୁଡ଼ିକ.ga_len; ++i)
	{
	    vp = (bval_T *)ମୂଲ୍ୟଗୁଡ଼ିକ.ga_data + i;
	    ଯଦି (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&ମୂଲ୍ୟଗୁଡ଼ିକ);
    }

    ଯଦି (read_next)
	ଫେରନ୍ତୁ viminfo_readline(virp);
    ଫେରନ୍ତୁ FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo_up_to_marks(
    vir_T	*virp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		writing)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eof;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    ଯେତେବେଳେ (!eof && virp->vir_ଧାଡ଼ି[0] != '>')
    {
	ବାଛ (virp->vir_ଧାଡ଼ି[0])
	{
		// Characters reserved for future expansion, ignored now
	    ସ୍ଥିତି '+': // "+40 /path/dir file", for running vim without args
	    ସ୍ଥିତି '^': // to be defined
	    ସ୍ଥିତି '<': // long line - ignored
		// A comment or empty line.
	    ସ୍ଥିତି NUL:
	    ସ୍ଥିତି '\r':
	    ସ୍ଥିତି '\n':
	    ସ୍ଥିତି '#':
		eof = viminfo_readline(virp);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#ଅନ୍ୟଥା
		eof = viminfo_readline(virp);
#endif
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '"':
		// When registers are in bar lines skip the old style register
		// lines.
		ଯଦି (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		ଅନ୍ୟଥା
		    କର
		    {
			eof = viminfo_readline(virp);
		    } ଯେତେବେଳେ (!eof && (virp->vir_ଧାଡ଼ି[0] == TAB
						|| virp->vir_ଧାଡ଼ି[0] == '<'));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '/':	    // Search string
	    ସ୍ଥିତି '&':	    // Substitute search string
	    ସ୍ଥିତି '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_ସନ୍ଧାନ_pattern(virp, forceit, writing);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '$':
		eof = read_viminfo_sub_string(virp, forceit);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ':':
	    ସ୍ଥିତି '?':
	    ସ୍ଥିତି '=':
	    ସ୍ଥିତି '@':
		// When history is in bar lines skip the old style history
		// lines.
		ଯଦି (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		ଅନ୍ୟଥା
		    eof = viminfo_readline(virp);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '-':
	    ସ୍ଥିତି '\'':
		// When file marks are in bar lines skip the old style lines.
		ଯଦି (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		ଅନ୍ୟଥା
		    eof = viminfo_readline(virp);
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଯଦି (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_ଧାଡ଼ି))
		    eof = TRUE;
		ଅନ୍ୟଥା
		    eof = viminfo_readline(virp);
		ଭାଙ୍ଗ;
	}
    }

    // Finish reading history items.
    ଯଦି (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    ଫେରନ୍ତୁ eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_viminfo(FILE *fp_in, FILE *fp_out, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eof = FALSE;
    vir_T	vir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		merge = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_copy_marks = FALSE;
    garray_T	buflist;

    ଯଦି ((vir.vir_ଧାଡ଼ି = alloc(LSIZE)) == ଖାଲି)
	ଫେରନ୍ତୁ;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, ଆକାର(char_u *), 100);
    vir.vir_version = -1;

    ଯଦି (fp_in != ଖାଲି)
    {
	ଯଦି (flags & VIF_WANT_INFO)
	{
	    ଯଦି (fp_out != ଖାଲି)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != ଖାଲି);
	    merge = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (flags != 0)
	    // Skip info, find start of marks
	    ଯେତେବେଳେ (!(eof = viminfo_readline(&vir))
		    && vir.vir_ଧାଡ଼ି[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    ଯଦି (fp_out != ଖାଲି)
    {
	// Write the info:
	fprintf(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_out);
	fprintf(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_ସନ୍ଧାନ_pattern(fp_out);
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

	ଯଦି (do_copy_marks)
	    ga_init2(&buflist, ଆକାର(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : ଖାଲି);
    }

    ଯଦି (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	ଯଦି (fp_out != ଖାଲି)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_ଧାଡ଼ି);
    ଯଦି (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, ଖାଲି, ଖାଲି);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    ଯଦି (no_viminfo())
	ଫେରନ୍ତୁ FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    fp = mch_fopen((ଅକ୍ଷର *)fname, READBIN);

    ଯଦି (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == ଖାଲି ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    ଯଦି (fp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	ଫେରନ୍ତୁ FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, ଖାଲି, flags);

    fclose(fp);
    ଫେରନ୍ତୁ OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    ଶୂନ୍ୟ
write_viminfo(char_u *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    char_u	*fname;
    FILE	*fp_in = ଖାଲି;	// input viminfo file, if any
    FILE	*fp_out = ଖାଲି;	// output viminfo file
    char_u	*tempname = ଖାଲି;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#ଯଦି defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hidden = FALSE;
#endif

    ଯଦି (no_viminfo())
	ଫେରନ୍ତୁ;

    fname = viminfo_filename(file);	// may set to default if NULL
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    fp_in = mch_fopen((ଅକ୍ଷର *)fname, READBIN);
    ଯଦି (fp_in == ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

	// if it does exist, but we can't read it, don't try writing
	ଯଦି (mch_stat((ଅକ୍ଷର *)fname, &st_new) == 0)
	    ଯାଅ end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((ଅକ୍ଷର *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	ଯଦି (fd < 0)
	    ଯାଅ end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    ଅନ୍ୟଥା
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	ଯଦି (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != ମୂଳ_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	tt = ସନ୍ଦେଶ_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    ସନ୍ଦେଶ_didany = tt;
	    fclose(fp_in);
	    ଯାଅ end;
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
	ପାଇଁ (;;)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#ଅନ୍ୟଥା
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#ଅନ୍ୟଥା
				    (char_u *)".tmp",
#endif
				    FALSE);
	    ଯଦି (tempname == ଖାଲି)		// out of memory
		ଭାଙ୍ଗ;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    ଯଦି (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    ପାଇଁ (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		ଯଦି (mch_stat((ଅକ୍ଷର *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    ଯଦି (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			ଭାଙ୍ଗ;
		    }
#endif
		}
		ଅନ୍ୟଥା
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((ଅକ୍ଷର *)tempname, WRITEBIN);
		    (ଶୂନ୍ୟ)umask(umask_save);
#ଅନ୍ୟଥା
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((ଅକ୍ଷର *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((st_old.st_mode & 0777) | 0600));
		    (ଶୂନ୍ୟ)umask(umask_save);
# ଅନ୍ୟଥା
		    fd = mch_open((ଅକ୍ଷର *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    ଯଦି (fd < 0)
		    {
			fp_out = ଖାଲି;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			ଯଦି (errno != EEXIST)
			    ଭାଙ୍ଗ;
# endif
		    }
		    ଅନ୍ୟଥା
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    ଯଦି (fp_out != ଖାଲି)
			ଭାଙ୍ଗ;
		}

		// Assume file exists, try again with another name.
		ଯଦି (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    ଭାଙ୍ଗ;
		}
		*wp = next_char;
		--next_char;
	    }

	    ଯଦି (tempname != ଖାଲି)
		ଭାଙ୍ଗ;
	    // continue if shortname was set
	}

#ଯଦି defined(UNIX) && defined(HAVE_FCHOWN)
	ଯଦି (tempname != ଖାଲି && fp_out != ଖାଲି)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    ଯଦି (mch_stat((ଅକ୍ଷର *)tempname, &tmp_st) >= 0)
	    {
		ଯଦି (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		ଯଦି (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (ଶୂନ୍ୟ)mch_setperm(tempname, 0600);
	    }
	    ଅନ୍ୟଥା
		// can't stat the file, set conservative permissions
		(ଶୂନ୍ୟ)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    ଯଦି (fp_out == ଖାଲି)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == ଖାଲି || tempname == ଖାଲି) ? fname : tempname);
	ଯଦି (fp_in != ଖାଲି)
	    fclose(fp_in);
	ଯାଅ end;
    }

    ଯଦି (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    ଯଦି (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    ଯଦି (fp_in != ଖାଲି)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	ଯଦି (viminfo_errcnt == 0)
	{
	    ଯଦି (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    ଅନ୍ୟଥା ଯଦି (hidden)
		mch_hide(fname);
#endif
	}
	ଯଦି (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    ଶୂନ୍ୟ
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    ଯଦି (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    ଯଦି (eap->cmdidx == CMD_rviminfo)
    {
	ଯଦି (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    ଅନ୍ୟଥା
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
