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

#include "vim.h"
#include "version.h"

/*
 * Structure used for reading from the viminfo file.
 */
typedef sa_nracanaa
{
    char_u	*vir_line;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    puur_naa_nka		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

typedef ga_nanaa {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

typedef sa_nracanaa {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    puur_naa_nka		bv_len;		// length of bv_string
    puur_naa_nka		bv_allocated;	// bv_string was allocated
} bval_T;

#ᬬᬾᬦ᭄ defined(FEAT_VIMINFO)

static puur_naa_nka  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    static char_u *
find_viminfo_parameter(puur_naa_nka type)
{
    char_u  *p;

    for (p = p_viminfo; *p; ++p)
    {
	ᬬᬾᬦ᭄ (*p == type)
	    ᬯᬮᬶ p + 1;
	ᬬᬾᬦ᭄ (*p == 'n')		    // 'n' is always the last one
	    bha_nga;
	p = vim_strchr(p, ',');	    // skip until next ','
	ᬬᬾᬦ᭄ (p == NULL)		    // hit the end without finding parameter
	    bha_nga;
    }
    ᬯᬮᬶ NULL;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    puur_naa_nka
get_viminfo_parameter(puur_naa_nka type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    ᬬᬾᬦ᭄ (p != NULL && VIM_ISDIGIT(*p))
	ᬯᬮᬶ atoi((var_na *)p);
    ᬯᬮᬶ -1;
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 * Returns an allocated string.  NULL when out of memory.
 */
    static char_u *
viminfo_filename(char_u *file)
{
    ᬬᬾᬦ᭄ (file == NULL || *file == NUL)
    {
	size_t  len;

	ᬬᬾᬦ᭄ (*p_viminfofile != NUL)
	    file = p_viminfofile;
	ᬮᬶᬬ ᬬᬾᬦ᭄ ((file = find_viminfo_parameter('n')) == NULL || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    ᬬᬾᬦ᭄ (mch_getenv((char_u *)"SYS$LOGIN") == NULL)
# ᬮᬶᬬ
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    ᬬᬾᬦ᭄ (STRCMP(vim_getenv((char_u *)"HOME", NULL), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == NULL)
#  ᬮᬶᬬ
	    ᬬᬾᬦ᭄ (mch_getenv((char_u *)"HOME") == NULL)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		ᬬᬾᬦ᭄ (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		ᬮᬶᬬ
		    file = (char_u *)VIMINFO_FILE;
	    }
	    ᬮᬶᬬ
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	ᬯᬮᬶ vim_strnsave(file, len);
    }

    ᬯᬮᬶ vim_strsave(file);
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
    static void
viminfo_writestring(FILE *fd, char_u *p)
{
    puur_naa_nka		c;
    char_u	*s;
    puur_naa_nka		len = 0;

    for (s = p; *s != NUL; ++s)
    {
	ᬬᬾᬦ᭄ (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    ᬬᬾᬦ᭄ (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    ᬲᭂᬤᭂᬮ ((c = *p++) != NUL)
    {
	ᬬᬾᬦ᭄ (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    ᬬᬾᬦ᭄ (c == '\n')
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
    static puur_naa_nka
barline_writestring(FILE *fd, char_u *s, puur_naa_nka remaining_start)
{
    char_u *p;
    puur_naa_nka	    remaining = remaining_start;
    puur_naa_nka	    len = 2;

    // Count the number of characters produced, including quotes.
    for (p = s; *p != NUL; ++p)
    {
	ᬬᬾᬦ᭄ (*p == NL)
	    len += 2;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*p == '"' || *p == '\\')
	    len += 2;
	ᬮᬶᬬ
	    ++len;
    }
    ᬬᬾᬦ᭄ (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    for (p = s; *p != NUL; ++p)
    {
	ᬬᬾᬦ᭄ (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	ᬮᬶᬬ
	    putc(*p, fd);
	--remaining;

	ᬬᬾᬦ᭄ (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    ᬯᬮᬶ remaining - 2;
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
    static char_u *
viminfo_readstring(
    vir_T	*virp,
    puur_naa_nka		off,		    // offset for virp->vir_line
    puur_naa_nka		convert)	    // convert the string
{
    char_u	*retval = NULL;
    char_u	*s, *d;
    long	len;

    ᬬᬾᬦ᭄ (virp->vir_line[off] == Ctrl_V && vim_isdigit(virp->vir_line[off + 1]))
    {
	len = atol((var_na *)virp->vir_line + off + 1);
	ᬬᬾᬦ᭄ (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	ᬬᬾᬦ᭄ (retval == NULL)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (void)vim_fgets(virp->vir_line, 10, virp->vir_fd);
	    ᬯᬮᬶ NULL;
	}
	(void)vim_fgets(retval, (puur_naa_nka)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    ᬮᬶᬬ
    {
	retval = vim_strsave(virp->vir_line + off);
	ᬬᬾᬦ᭄ (retval == NULL)
	    ᬯᬮᬶ NULL;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    ᬲᭂᬤᭂᬮ (*s != NUL && *s != '\n')
    {
	ᬬᬾᬦ᭄ (s[0] == Ctrl_V && s[1] != NUL)
	{
	    ᬬᬾᬦ᭄ (s[1] == 'n')
		*d++ = '\n';
	    ᬮᬶᬬ
		*d++ = Ctrl_V;
	    s += 2;
	}
	ᬮᬶᬬ
	    *d++ = *s++;
    }
    *d = NUL;

    ᬬᬾᬦ᭄ (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, NULL);
	ᬬᬾᬦ᭄ (d != NULL)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    ᬯᬮᬶ retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    static puur_naa_nka
viminfo_readline(vir_T *virp)
{
    ᬯᬮᬶ vim_fgets(virp->vir_line, LSIZE, virp->vir_fd);
}

    static puur_naa_nka
read_viminfo_bufferlist(
    vir_T	*virp,
    puur_naa_nka		writing)
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
    ᬬᬾᬦ᭄ (xline != NULL && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != NULL)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	ᬬᬾᬦ᭄ (tab != NULL)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((var_na *)tab);
	    tab = vim_strrchr(xline, '\t');
	    ᬬᬾᬦ᭄ (tab != NULL)
	    {
		*tab++ = '\0';
		lnum = atol((var_na *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	ᬬᬾᬦ᭄ (buf != NULL)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    ᬯᬮᬶ viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    static puur_naa_nka
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    puur_naa_nka	    part_len;
    puur_naa_nka	    retval = FALSE;

    name = home_replace_save(NULL, name);
    ᬬᬾᬦ᭄ (name == NULL)
	ᬯᬮᬶ FALSE;
    for (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, parimaa_na(part), ", ");
	ᬬᬾᬦ᭄ (part[0] == 'r')
	{
	    ᬬᬾᬦ᭄ (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		bha_nga;
	    }
	}
    }
    vim_free(name);
    ᬯᬮᬶ retval;
}

    static void
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*line;
    puur_naa_nka		max_buffers;

    ᬬᬾᬦ᭄ (find_viminfo_parameter('%') == NULL)
	ᬯᬮᬶ;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define LINE_BUF_LEN (MAXPATHL + 40)
    line = alloc(LINE_BUF_LEN);
    ᬬᬾᬦ᭄ (line == NULL)
	ᬯᬮᬶ;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	ᬬᬾᬦ᭄ (buf->b_fname == NULL
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    continue;

	ᬬᬾᬦ᭄ (max_buffers-- == 0)
	    bha_nga;
	putc('%', fp);
	linelen = home_replace(NULL, buf->b_ffname, line, MAXPATHL, TRUE);
	vim_snprintf((var_na *)line + linelen, LINE_BUF_LEN - linelen, "\t%ld\t%d",
			(long)buf->b_last_cursor.lnum,
			buf->b_last_cursor.col);
	viminfo_writestring(fp, line);
    }
    vim_free(line);
}

/*
 * Buffers for history read from a viminfo file.  Only valid while reading.
 */
static histentry_T *viminfo_history[HIST_COUNT] =
					       {NULL, NULL, NULL, NULL, NULL};
static puur_naa_nka	viminfo_hisidx[HIST_COUNT] = {0, 0, 0, 0, 0};
static puur_naa_nka	viminfo_hislen[HIST_COUNT] = {0, 0, 0, 0, 0};
static puur_naa_nka	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    static puur_naa_nka
hist_type2char(
    puur_naa_nka	    type,
    puur_naa_nka	    use_question)	    // use '?' instead of '/'
{
    ᬬᬾᬦ᭄ (type == HIST_CMD)
	ᬯᬮᬶ ':';
    ᬬᬾᬦ᭄ (type == HIST_SEARCH)
    {
	ᬬᬾᬦ᭄ (use_question)
	    ᬯᬮᬶ '?';
	ᬮᬶᬬ
	    ᬯᬮᬶ '/';
    }
    ᬬᬾᬦ᭄ (type == HIST_EXPR)
	ᬯᬮᬶ '=';
    ᬯᬮᬶ '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    static void
prepare_viminfo_history(puur_naa_nka asklen, puur_naa_nka writing)
{
    puur_naa_nka	    i;
    puur_naa_nka	    num;
    puur_naa_nka	    type;
    puur_naa_nka	    len;
    puur_naa_nka	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    ᬬᬾᬦ᭄ (asklen > hislen)
	asklen = hislen;

    for (type = 0; type < HIST_COUNT; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	for (i = 0, num = 0; i < hislen; i++)
	    ᬬᬾᬦ᭄ (histentry[i].hisstr == NULL || histentry[i].viminfo)
		num++;
	len = asklen;
	ᬬᬾᬦ᭄ (num > len)
	    len = num;
	ᬬᬾᬦ᭄ (len <= 0)
	    viminfo_history[type] = NULL;
	ᬮᬶᬬ
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	ᬬᬾᬦ᭄ (viminfo_history[type] == NULL)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    static puur_naa_nka
read_viminfo_history(vir_T *virp, puur_naa_nka writing)
{
    puur_naa_nka		type;
    long_u	len;
    char_u	*val = NULL;
    char_u	*p;

    type = hist_char2type(virp->vir_line[0]);
    ᬬᬾᬦ᭄ (viminfo_hisidx[type] >= viminfo_hislen[type])
	goto done;

    val = viminfo_readstring(virp, 1, TRUE);
    ᬬᬾᬦ᭄ (val == NULL || *val == NUL)
	goto done;

    puur_naa_nka sep = (*val == ' ' ? NUL : *val);

    ᬬᬾᬦ᭄ (in_history(type, val + (type == HIST_SEARCH), viminfo_add_at_front,
								sep, writing))
	goto done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    ᬬᬾᬦ᭄ (type == HIST_SEARCH)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	ᬬᬾᬦ᭄ (p == NULL)
	    goto done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    ᬮᬶᬬ
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	ᬬᬾᬦ᭄ (p == NULL)
	    goto done;

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
    ᬯᬮᬶ viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    static void
handle_viminfo_history(
	garray_T    *values,
	puur_naa_nka	    writing)
{
    puur_naa_nka		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)values->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    ᬬᬾᬦ᭄ (values->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	ᬯᬮᬶ;

    type = vp[0].bv_nr;
    ᬬᬾᬦ᭄ (type >= HIST_COUNT)
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (viminfo_hisidx[type] >= viminfo_hislen[type])
	ᬯᬮᬶ;

    val = vp[3].bv_string;
    ᬬᬾᬦ᭄ (val == NULL || *val == NUL)
	ᬯᬮᬶ;

    puur_naa_nka sep = type == HIST_SEARCH && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    puur_naa_nka idx;
    puur_naa_nka overwrite = FALSE;

    ᬬᬾᬦ᭄ (in_history(type, val, viminfo_add_at_front, sep, writing))
	ᬯᬮᬶ;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    for (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	ᬬᬾᬦ᭄ (STRCMP(val, p) == 0
		&& (type != HIST_SEARCH || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    bha_nga;
	}
    }

    ᬬᬾᬦ᭄ (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    ᬮᬶᬬ
	len = 0; // for picky compilers
    ᬬᬾᬦ᭄ (p != NULL)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	ᬬᬾᬦ᭄ (!overwrite)
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
    static void
concat_history(puur_naa_nka type)
{
    puur_naa_nka		idx;
    puur_naa_nka		i;
    puur_naa_nka		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    puur_naa_nka		*hisidx = get_hisidx(type);
    puur_naa_nka		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    ᬬᬾᬦ᭄ (idx >= hislen)
	idx -= hislen;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (idx < 0)
	idx = hislen - 1;
    ᬬᬾᬦ᭄ (viminfo_add_at_front)
	*hisidx = idx;
    ᬮᬶᬬ
    {
	ᬬᬾᬦ᭄ (*hisidx == -1)
	    *hisidx = hislen - 1;
	do
	{
	    ᬬᬾᬦ᭄ (histentry[idx].hisstr != NULL || histentry[idx].viminfo)
		bha_nga;
	    ᬬᬾᬦ᭄ (++idx == hislen)
		idx = 0;
	} ᬲᭂᬤᭂᬮ (idx != *hisidx);
	ᬬᬾᬦ᭄ (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    for (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	ᬬᬾᬦ᭄ (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    for (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    static puur_naa_nka
sort_hist(const void *s1, const void *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    ᬬᬾᬦ᭄ (p1->time_set < p2->time_set) ᬯᬮᬶ -1;
    ᬬᬾᬦ᭄ (p1->time_set > p2->time_set) ᬯᬮᬶ 1;
    ᬯᬮᬶ 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    static void
merge_history(puur_naa_nka type)
{
    puur_naa_nka		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    puur_naa_nka		i;
    puur_naa_nka		len;
    puur_naa_nka		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    puur_naa_nka		*hisidx = get_hisidx(type);
    puur_naa_nka		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    ᬬᬾᬦ᭄ (tot_hist == NULL || new_hist == NULL)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	ᬯᬮᬶ;
    }
    for (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    for (i = 0; i < hislen; i++)
	ᬬᬾᬦ᭄ (histentry[i].hisstr != NULL)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((void *)tot_hist, (size_t)len, parimaa_na(histentry_T *), sort_hist);

    // Keep the newest ones.
    for (i = 0; i < hislen; i++)
    {
	ᬬᬾᬦ᭄ (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = NULL;
	    tot_hist[i]->hisstrlen = 0;
	    ᬬᬾᬦ᭄ (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	ᬮᬶᬬ
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    for (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    for (i = 0; i < hislen; i++)
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
    static void
finish_viminfo_history(vir_T *virp)
{
    puur_naa_nka	type;
    puur_naa_nka merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    for (type = 0; type < HIST_COUNT; ++type)
    {
	ᬬᬾᬦ᭄ (get_histentry(type) == NULL)
	    continue;

	ᬬᬾᬦ᭄ (merge)
	    merge_history(type);
	ᬮᬶᬬ
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
    static void
write_viminfo_history(FILE *fp, puur_naa_nka merge)
{
    puur_naa_nka	    i;
    puur_naa_nka	    type;
    puur_naa_nka	    num_saved;
    puur_naa_nka     round;
    puur_naa_nka	    hislen;

    init_history();
    hislen = get_hislen();
    ᬬᬾᬦ᭄ (hislen == 0)
	ᬯᬮᬶ;
    for (type = 0; type < HIST_COUNT; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	puur_naa_nka	    *hisidx = get_hisidx(type);

	num_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	ᬬᬾᬦ᭄ (num_saved == 0)
	    continue;
	ᬬᬾᬦ᭄ (num_saved < 0)  // Use default
	    num_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_SEARCH ? _("Search String") :
			    type == HIST_EXPR ? _("Expression") :
			    type == HIST_INPUT ? _("Input Line") :
					_("Debug Line"));
	ᬬᬾᬦ᭄ (num_saved > hislen)
	    num_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	for (round = 1; round <= 2; ++round)
	{
	    ᬬᬾᬦ᭄ (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    ᬮᬶᬬ
		// empty list
		i = -1;
	    ᬬᬾᬦ᭄ (i >= 0)
		ᬲᭂᬤᭂᬮ (num_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    puur_naa_nka	    c = NUL;

		    ᬬᬾᬦ᭄ (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    ᬮᬶᬬ
		    {
			ᬬᬾᬦ᭄ (viminfo_history[type] == NULL)
			{
			    p = NULL;
			    plen = 0;
			    timestamp = 0;
			}
			ᬮᬶᬬ
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    ᬬᬾᬦ᭄ (p != NULL && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--num_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			ᬬᬾᬦ᭄ (type == HIST_SEARCH)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    var_na    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    ᬬᬾᬦ᭄ (c == NUL)
				cbuf[0] = NUL;
			    ᬮᬶᬬ
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    ᬬᬾᬦ᭄ (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			ᬬᬾᬦ᭄ (--i < 0)
			    i = hislen - 1;
			ᬬᬾᬦ᭄ (i == *hisidx)
			    bha_nga;
		    }
		    ᬮᬶᬬ
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	for (i = 0; i < viminfo_hisidx[type]; ++i)
	    ᬬᬾᬦ᭄ (viminfo_history[type] != NULL)
	    {
		vim_free(viminfo_history[type][i].hisstr);
		viminfo_history[type][i].hisstrlen = 0;
	    }
	VIM_CLEAR(viminfo_history[type]);
	viminfo_hisidx[type] = 0;
    }
}

    static void
write_viminfo_barlines(vir_T *virp, FILE *fp_out)
{
    puur_naa_nka		i;
    garray_T	*gap = &virp->vir_barlines;
    puur_naa_nka		seen_useful = FALSE;
    var_na	*line;

    ᬬᬾᬦ᭄ (gap->ga_len <= 0)
	ᬯᬮᬶ;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    for (i = 0; i < gap->ga_len; ++i)
    {
	line = ((var_na **)(gap->ga_data))[i];
	ᬬᬾᬦ᭄ (seen_useful || line[1] != '<')
	{
	    fputs(line, fp_out);
	    seen_useful = TRUE;
	}
    }
}

/*
 * Parse a viminfo line starting with '|'.
 * Add each decoded value to "values".
 * Returns TRUE if the next line is to be read after using the parsed values.
 */
    static puur_naa_nka
barline_parse(vir_T *virp, char_u *text, garray_T *values)
{
    char_u  *p = text;
    char_u  *nextp = NULL;
    char_u  *buf = NULL;
    bval_T  *value;
    puur_naa_nka	    i;
    puur_naa_nka	    allocated = FALSE;
    puur_naa_nka	    eof;
    char_u  *sconv;
    puur_naa_nka	    converted;

    ᬲᭂᬤᭂᬮ (*p == ',')
    {
	++p;
	ᬬᬾᬦ᭄ (ga_grow(values, 1) == FAIL)
	    bha_nga;
	value = (bval_T *)(values->ga_data) + values->ga_len;

	ᬬᬾᬦ᭄ (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    ᬬᬾᬦ᭄ (!allocated)
	    {
		for (i = 0; i < values->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(values->ga_data) + i;

		    ᬬᬾᬦ᭄ (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    ᬬᬾᬦ᭄ (vim_isdigit(p[1]))
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
		ᬬᬾᬦ᭄ (buf == NULL)
		    ᬯᬮᬶ TRUE;
		p = buf;
		for (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    ᬬᬾᬦ᭄ (eof || virp->vir_line[0] != '|'
						  || virp->vir_line[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			ᬯᬮᬶ eof || virp->vir_line[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_line);
		    ᬲᭂᬤᭂᬮ (n > 2 && (virp->vir_line[n - 1] == NL
					     || virp->vir_line[n - 1] == CAR))
			--n;
		    n -= 2;
		    ᬬᬾᬦ᭄ (n > todo)
		    {
			// more values follow after the string
			nextp = virp->vir_line + 2 + todo;
			n = todo;
		    }
		    mch_memmove(p, virp->vir_line + 2, n);
		    p += n;
		}
		*p = NUL;
		p = buf;
	    }
	    ᬮᬶᬬ
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		ᬬᬾᬦ᭄ (eof || virp->vir_line[0] != '|'
					      || virp->vir_line[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    ᬯᬮᬶ eof || virp->vir_line[0] == '|';
		p = virp->vir_line + 2;
	    }
	}

	ᬬᬾᬦ᭄ (SAFE_isdigit(*p))
	{
	    value->bv_type = BVAL_NR;
	    value->bv_nr = getdigits(&p);
	    ++values->ga_len;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*p == '"')
	{
	    puur_naa_nka	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    ᬲᭂᬤᭂᬮ (*p != '"')
	    {
		ᬬᬾᬦ᭄ (*p == NL || *p == NUL)
		    ᬯᬮᬶ TRUE;  // syntax error, drop the value
		ᬬᬾᬦ᭄ (*p == '\\')
		{
		    ++p;
		    ᬬᬾᬦ᭄ (*p == 'n')
			s[len++] = '\n';
		    ᬮᬶᬬ
			s[len++] = *p;
		    ++p;
		}
		ᬮᬶᬬ
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    value->bv_tofree = NULL;
	    ᬬᬾᬦ᭄ (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, NULL);
		ᬬᬾᬦ᭄ (sconv != NULL)
		{
		    ᬬᬾᬦ᭄ (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			value->bv_tofree = buf;
		    s = sconv;
		    len = (puur_naa_nka)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    ᬬᬾᬦ᭄ (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    value->bv_string = s;
	    value->bv_type = BVAL_STRING;
	    value->bv_len = len;
	    value->bv_allocated = allocated || converted;
	    ++values->ga_len;
	    ᬬᬾᬦ᭄ (nextp != NULL)
	    {
		// values following a long string
		p = nextp;
		nextp = NULL;
	    }
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*p == ',')
	{
	    value->bv_type = BVAL_EMPTY;
	    ++values->ga_len;
	}
	ᬮᬶᬬ
	    bha_nga;
    }
    ᬯᬮᬶ TRUE;
}

    static void
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    static puur_naa_nka
no_viminfo(void)
{
    // "vim -i NONE" does not read or write a viminfo file
    ᬯᬮᬶ STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    static puur_naa_nka
viminfo_error(var_na *errnum, var_na *message, char_u *line)
{
    vim_snprintf((var_na *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, line, IOSIZE - STRLEN(IObuff) - 1);
    ᬬᬾᬦ᭄ (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((var_na *)IObuff);
    ᬬᬾᬦ᭄ (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	ᬯᬮᬶ TRUE;
    }
    ᬯᬮᬶ FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    static puur_naa_nka
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    puur_naa_nka		i;

    ᬬᬾᬦ᭄ (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_line, '=');
	ᬬᬾᬦ᭄ (p != NULL)
	{
	    // remove trailing newline
	    ++p;
	    for (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    ᬯᬮᬶ viminfo_readline(virp);
}

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    static puur_naa_nka
read_viminfo_varlist(vir_T *virp, puur_naa_nka writing)
{
    char_u	*tab;
    puur_naa_nka		type = VAR_NUMBER;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    ᬬᬾᬦ᭄ (!writing && (find_viminfo_parameter('!') != NULL))
    {
	tab = vim_strchr(virp->vir_line + 1, '\t');
	ᬬᬾᬦ᭄ (tab != NULL)
	{
	    *tab++ = '\0';	// isolate the variable name
	    switch (*tab)
	    {
		pak_sa 'S': type = VAR_STRING; bha_nga;
		pak_sa 'F': type = VAR_FLOAT; bha_nga;
		pak_sa 'D': type = VAR_DICT; bha_nga;
		pak_sa 'L': type = VAR_LIST; bha_nga;
		pak_sa 'B': type = VAR_BLOB; bha_nga;
		pak_sa 'X': type = VAR_SPECIAL; bha_nga;
		pak_sa 'T': type = VAR_TUPLE; bha_nga;
	    }

	    tab = vim_strchr(tab, '\t');
	    ᬬᬾᬦ᭄ (tab != NULL)
	    {
		tv.v_type = type;
		ᬬᬾᬦ᭄ (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (puur_naa_nka)(tab - virp->vir_line + 1), TRUE);
		ᬮᬶᬬ ᬬᬾᬦ᭄ (type == VAR_FLOAT)
		    (void)string2float(tab + 1, &tv.vval.v_float, FALSE);
		ᬮᬶᬬ
		{
		    tv.vval.v_number = atol((var_na *)tab + 1);
		    ᬬᬾᬦ᭄ (type == VAR_SPECIAL && (tv.vval.v_number == VVAL_FALSE
					     || tv.vval.v_number == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		ᬬᬾᬦ᭄ (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_expr(tv.vval.v_string, NULL);

		    ᬬᬾᬦ᭄ (etv == NULL)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    ᬮᬶᬬ
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		ᬮᬶᬬ ᬬᬾᬦ᭄ (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    ᬬᬾᬦ᭄ (blob == NULL)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    ᬮᬶᬬ
		    {
			vim_free(tv.vval.v_string);
			tv.v_type = VAR_BLOB;
			tv.vval.v_blob = blob;
		    }
		}

		// when in a function use global variables
		save_funccal(&funccal_entry);
		set_var(virp->vir_line + 1, &tv, FALSE);
		restore_funccal();

		ᬬᬾᬦ᭄ (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		ᬮᬶᬬ ᬬᬾᬦ᭄ (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    ᬯᬮᬶ viminfo_readline(virp);
}

/*
 * Write global vars that start with a capital to the viminfo file
 */
    static void
write_viminfo_varlist(FILE *fp)
{
    hashtab_T	*gvht = get_globvar_ht();
    hashitem_T	*hi;
    dictitem_T	*this_var;
    puur_naa_nka		todo;
    var_na	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    ᬬᬾᬦ᭄ (find_viminfo_parameter('!') == NULL)
	ᬯᬮᬶ;

    fputs(_("\n# global variables:\n"), fp);

    todo = (puur_naa_nka)gvht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(gvht, hi, todo)
    {
	ᬬᬾᬦ᭄ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    ᬬᬾᬦ᭄ (var_flavour(this_var->di_key) == VAR_FLAVOUR_VIMINFO)
	    {
		switch (this_var->di_tv.v_type)
		{
		    pak_sa VAR_STRING:  s = "STR"; bha_nga;
		    pak_sa VAR_NUMBER:  s = "NUM"; bha_nga;
		    pak_sa VAR_FLOAT:   s = "FLO"; bha_nga;
		    pak_sa VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      puur_naa_nka	copyID = get_copyID();

			      s = "DIC";
			      ᬬᬾᬦ᭄ (di != NULL && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, NULL, NULL)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  continue;
			      bha_nga;
			  }
		    pak_sa VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      puur_naa_nka	copyID = get_copyID();

			      s = "LIS";
			      ᬬᬾᬦ᭄ (l != NULL && !set_ref_in_list_items(
						       l, copyID, NULL, NULL)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  continue;
			      bha_nga;
			  }
		    pak_sa VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      puur_naa_nka	copyID = get_copyID();

			      s = "TUP";
			      ᬬᬾᬦ᭄ (tuple != NULL && !set_ref_in_tuple_items(
					       tuple, copyID, NULL, NULL)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  continue;
			      bha_nga;
			  }
		    pak_sa VAR_BLOB:    s = "BLO"; bha_nga;
		    pak_sa VAR_BOOL:    s = "XPL"; bha_nga;  // backwards compat.
		    pak_sa VAR_SPECIAL: s = "XPL"; bha_nga;

		    pak_sa VAR_UNKNOWN:
		    pak_sa VAR_ANY:
		    pak_sa VAR_VOID:
		    pak_sa VAR_FUNC:
		    pak_sa VAR_PARTIAL:
		    pak_sa VAR_JOB:
		    pak_sa VAR_CHANNEL:
		    pak_sa VAR_INSTR:
		    pak_sa VAR_CLASS:
		    pak_sa VAR_OBJECT:
		    pak_sa VAR_TYPEALIAS:
				      continue;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_key, s);
		ᬬᬾᬦ᭄ (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((var_na *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_number);
		    p = numbuf;
		    tofree = NULL;
		}
		ᬮᬶᬬ
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		ᬬᬾᬦ᭄ (p != NULL)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    static puur_naa_nka
read_viminfo_sub_string(vir_T *virp, puur_naa_nka force)
{
    ᬬᬾᬦ᭄ (force || get_old_sub() == NULL)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    ᬯᬮᬶ viminfo_readline(virp);
}

    static void
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    ᬬᬾᬦ᭄ (get_viminfo_parameter('/') == 0 || old_sub == NULL)
	ᬯᬮᬶ;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    static puur_naa_nka
read_viminfo_search_pattern(vir_T *virp, puur_naa_nka force, puur_naa_nka writing)
{
    char_u	*lp;
    puur_naa_nka		idx = -1;
    puur_naa_nka		magic = FALSE;
    puur_naa_nka		no_scs = FALSE;
    puur_naa_nka		off_line = FALSE;
    puur_naa_nka		off_end = 0;
    long	off = 0;
    puur_naa_nka		setlast = FALSE;
#ifdef FEAT_SEARCH_EXTRA
    static puur_naa_nka	hlsearch_on = FALSE;
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
    lp = virp->vir_line;
    ᬬᬾᬦ᭄ (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	ᬬᬾᬦ᭄ (lp[1] == 'M')		// magic on
	    magic = TRUE;
	ᬬᬾᬦ᭄ (lp[2] == 's')
	    no_scs = TRUE;
	ᬬᬾᬦ᭄ (lp[3] == 'L')
	    off_line = TRUE;
	ᬬᬾᬦ᭄ (lp[4] == 'E')
	    off_end = SEARCH_END;
	lp += 5;
	off = getdigits(&lp);
    }
    ᬬᬾᬦ᭄ (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    ᬬᬾᬦ᭄ (lp[0] == '/')
	idx = RE_SEARCH;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_SEARCH_EXTRA
    ᬮᬶᬬ ᬬᬾᬦ᭄ (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    ᬬᬾᬦ᭄ (idx >= 0)
    {
	spat = get_spat(idx);
	ᬬᬾᬦ᭄ (force || spat->pat == NULL)
	{
	    val = viminfo_readstring(virp, (puur_naa_nka)(lp - virp->vir_line + 1),
									TRUE);
	    ᬬᬾᬦ᭄ (val != NULL)
	    {
		ᬬᬾᬦ᭄ (!writing)
		{
		    set_last_search_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.line = off_line;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_SEARCH_EXTRA
		    ᬬᬾᬦ᭄ (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    ᬯᬮᬶ viminfo_readline(virp);
}

    static void
wvsp_one(
    FILE	*fp,	// file to write to
    puur_naa_nka		idx,	// spats[] index
    var_na	*s,	// search pat
    puur_naa_nka		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    ᬬᬾᬦ᭄ (spat->pat == NULL)
	ᬯᬮᬶ;

    fprintf(fp, _("\n# Last %sSearch Pattern:\n~"), s);
    // off.dir is not stored, it's reset to forward
    fprintf(fp, "%c%c%c%c%ld%s%c",
	    spat->magic    ? 'M' : 'm',	// magic
	    spat->no_scs   ? 's' : 'S',	// smartcase
	    spat->off.line ? 'L' : 'l',	// line offset
	    spat->off.end  ? 'E' : 'e',	// offset from end
	    spat->off.off,			// offset
	    get_spat_last_idx() == idx ? "~" : "",	// last used pat
	    sc);
    viminfo_writestring(fp, spat->pat);
}

    static void
write_viminfo_search_pattern(FILE *fp)
{
    ᬬᬾᬦ᭄ (get_viminfo_parameter('/') == 0)
	ᬯᬮᬶ;

#ifdef FEAT_SEARCH_EXTRA
    fprintf(fp, "\n# hlsearch on (H) or off (h):\n~%c",
	    (no_hlsearch || find_viminfo_parameter('h') != NULL) ? 'h' : 'H');
#endif
    wvsp_one(fp, RE_SEARCH, "", '/');
    wvsp_one(fp, RE_SUBST, _("Substitute "), '&');
}

/*
 * Functions relating to reading/writing registers from viminfo
 */

static yankreg_T *y_read_regs = NULL;

#define REG_PREVIOUS 1
#define REG_EXEC 2

/*
 * Prepare for reading viminfo registers when writing viminfo later.
 */
    static void
prepare_viminfo_registers(void)
{
    y_read_regs = ALLOC_CLEAR_MULT(yankreg_T, NUM_REGISTERS);
}

    static void
finish_viminfo_registers(void)
{
    puur_naa_nka		i;
    puur_naa_nka		j;

    ᬬᬾᬦ᭄ (y_read_regs == NULL)
	ᬯᬮᬶ;

    for (i = 0; i < NUM_REGISTERS; ++i)
	ᬬᬾᬦ᭄ (y_read_regs[i].y_array != NULL)
	{
	    for (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    static puur_naa_nka
read_viminfo_register(vir_T *virp, puur_naa_nka force)
{
    puur_naa_nka		eof;
    puur_naa_nka		do_it = TRUE;
    puur_naa_nka		size;
    puur_naa_nka		limit;
    puur_naa_nka		i;
    puur_naa_nka		set_prev = FALSE;
    char_u	*str;
    string_T	*array = NULL;
    puur_naa_nka		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_line + 1;

    // If the line starts with "" this is the y_previous register.
    ᬬᬾᬦ᭄ (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    ᬬᬾᬦ᭄ (!ASCII_ISALNUM(*str) && *str != '-')
    {
	ᬬᬾᬦ᭄ (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_line))
	    ᬯᬮᬶ TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    ᬬᬾᬦ᭄ (!force && y_current_p->y_array != NULL)
	do_it = FALSE;

    ᬬᬾᬦ᭄ (*str == '@')
    {
	// "x@: register x used for @@
	ᬬᬾᬦ᭄ (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    ᬬᬾᬦ᭄ (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	ᬬᬾᬦ᭄ (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	ᬬᬾᬦ᭄ (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	ᬮᬶᬬ
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    ᬲᭂᬤᭂᬮ (!(eof = viminfo_readline(virp))
		    && (virp->vir_line[0] == TAB || virp->vir_line[0] == '<'))
    {
	ᬬᬾᬦ᭄ (do_it)
	{
	    ᬬᬾᬦ᭄ (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * parimaa_na(string_T));

		ᬬᬾᬦ᭄ (new_array == NULL)
		{
		    do_it = FALSE;
		    bha_nga;
		}
		for (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    ᬬᬾᬦ᭄ (str != NULL)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    ᬮᬶᬬ
		// error, don't store the result
		do_it = FALSE;
	}
    }

    ᬬᬾᬦ᭄ (do_it)
    {
	// free y_array[]
	for (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	ᬬᬾᬦ᭄ (size == 0)
	{
	    y_current_p->y_array = NULL;
	}
	ᬮᬶᬬ
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    for (i = 0; i < size; i++)
	    {
		ᬬᬾᬦ᭄ (y_current_p->y_array == NULL)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		ᬮᬶᬬ
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    ᬮᬶᬬ
    {
	// Free array[] if it was filled.
	for (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    ᬯᬮᬶ eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    static void
handle_viminfo_register(garray_T *values, puur_naa_nka force)
{
    bval_T	*vp = (bval_T *)values->ga_data;
    puur_naa_nka		flags;
    puur_naa_nka		name;
    puur_naa_nka		type;
    puur_naa_nka		linecount;
    puur_naa_nka		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    puur_naa_nka		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    ᬬᬾᬦ᭄ (values->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	ᬯᬮᬶ;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    ᬬᬾᬦ᭄ (name < 0 || name >= NUM_REGISTERS)
	ᬯᬮᬶ;
    type = vp[2].bv_nr;
    ᬬᬾᬦ᭄ (type != MCHAR && type != MLINE && type != MBLOCK)
	ᬯᬮᬶ;
    linecount = vp[3].bv_nr;
    ᬬᬾᬦ᭄ (values->ga_len < 6 + linecount)
	ᬯᬮᬶ;
    width = vp[4].bv_nr;
    ᬬᬾᬦ᭄ (width < 0)
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (y_read_regs != NULL)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    ᬮᬶᬬ
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    ᬬᬾᬦ᭄ (y_ptr->y_array != NULL && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (y_ptr->y_array != NULL)
	for (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    ᬬᬾᬦ᭄ (y_read_regs == NULL)
    {
	ᬬᬾᬦ᭄ (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	ᬬᬾᬦ᭄ ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    ᬬᬾᬦ᭄ (linecount == 0)
    {
	y_ptr->y_array = NULL;
	ᬯᬮᬶ;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    ᬬᬾᬦ᭄ (y_ptr->y_array == NULL)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	ᬯᬮᬶ;
    }
    for (i = 0; i < linecount; i++)
    {
	ᬬᬾᬦ᭄ (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = NULL;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (vp[i + 6].bv_type != BVAL_STRING)
	{
	    free(y_ptr->y_array);
	    y_ptr->y_array = NULL;
	}
	ᬮᬶᬬ
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    static void
write_viminfo_registers(FILE *fp)
{
    puur_naa_nka		i, j;
    char_u	*type;
    char_u	c;
    puur_naa_nka		num_lines;
    puur_naa_nka		max_num_lines;
    puur_naa_nka		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_num_lines = get_viminfo_parameter('<');
    ᬬᬾᬦ᭄ (max_num_lines < 0)
	max_num_lines = get_viminfo_parameter('"');
    ᬬᬾᬦ᭄ (max_num_lines == 0)
	ᬯᬮᬶ;
    max_kbyte = get_viminfo_parameter('s');
    ᬬᬾᬦ᭄ (max_kbyte == 0)
	ᬯᬮᬶ;

    for (i = 0; i < NUM_REGISTERS; i++)
    {
#ᬬᬾᬦ᭄ defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	ᬬᬾᬦ᭄ (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    continue;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	ᬬᬾᬦ᭄ (i == TILDE_REGISTER)
	    continue;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	ᬬᬾᬦ᭄ (y_read_regs != NULL
		&& y_read_regs[i].y_array != NULL
		&& (y_regs_p[i].y_array == NULL ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	ᬮᬶᬬ ᬬᬾᬦ᭄ (y_regs_p[i].y_array == NULL)
	    continue;
	ᬮᬶᬬ
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	num_lines = y_ptr->y_size;
	ᬬᬾᬦ᭄ (num_lines == 0
		|| (num_lines == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    continue;

	ᬬᬾᬦ᭄ (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    for (j = 0; j < num_lines; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    ᬬᬾᬦ᭄ (len > (long)max_kbyte * 1024L)
		continue;
	}

	switch (y_ptr->y_type)
	{
	    pak_sa MLINE:
		type = (char_u *)"LINE";
		bha_nga;
	    pak_sa MCHAR:
		type = (char_u *)"CHAR";
		bha_nga;
	    pak_sa MBLOCK:
		type = (char_u *)"BLOCK";
		bha_nga;
	    default:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		bha_nga;
	}
	ᬬᬾᬦ᭄ (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	ᬬᬾᬦ᭄ (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (puur_naa_nka)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	ᬬᬾᬦ᭄ (max_num_lines > 0 && num_lines > max_num_lines)
	    num_lines = max_num_lines;
	for (j = 0; j < num_lines; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    puur_naa_nka	    flags = 0;
	    puur_naa_nka	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    ᬬᬾᬦ᭄ (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    ᬬᬾᬦ᭄ (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, num_lines, (puur_naa_nka)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    for (j = 0; j < num_lines; j++)
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

static xfmark_T *vi_namedfm = NULL;
static xfmark_T *vi_jumplist = NULL;
static puur_naa_nka vi_jumplist_len = 0;

    static void
write_one_mark(FILE *fp_out, puur_naa_nka c, pos_T *pos)
{
    ᬬᬾᬦ᭄ (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (puur_naa_nka)pos->col);
}

    static void
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    puur_naa_nka		i;
    pos_T	pos;

    home_replace(NULL, buf->b_ffname, IObuff, IOSIZE, TRUE);
    fprintf(fp_out, "\n> ");
    viminfo_writestring(fp_out, IObuff);

    // Write the last used timestamp as the lnum of the non-existing mark '*'.
    // Older Vims will ignore it and/or copy it.
    pos.lnum = (linenr_T)buf->b_last_used;
    pos.col = 0;
    write_one_mark(fp_out, '*', &pos);

    write_one_mark(fp_out, '"', &buf->b_last_cursor);
    write_one_mark(fp_out, '^', &buf->b_last_insert);
    write_one_mark(fp_out, '.', &buf->b_last_change);
    // changelist positions are stored oldest first
    for (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	ᬬᬾᬦ᭄ (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    for (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    static puur_naa_nka
skip_for_viminfo(buf_T *buf)
{
    ᬯᬮᬶ bt_terminal(buf) || removable(buf->b_ffname);
}

/*
 * Write all the named marks for all buffers.
 * When "buflist" is not NULL fill it with the buffers for which marks are to
 * be written.
 */
    static void
write_viminfo_marks(FILE *fp_out, garray_T *buflist)
{
    buf_T	*buf;
    puur_naa_nka		is_mark_set;
    puur_naa_nka		i;
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
	ᬬᬾᬦ᭄ (buf->b_marks_read)
	{
	    ᬬᬾᬦ᭄ (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    ᬮᬶᬬ
	    {
		is_mark_set = FALSE;
		for (i = 0; i < NMARKS; i++)
		    ᬬᬾᬦ᭄ (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			bha_nga;
		    }
	    }
	    ᬬᬾᬦ᭄ (is_mark_set && buf->b_ffname != NULL
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		ᬬᬾᬦ᭄ (buflist == NULL)
		    write_buffer_marks(buf, fp_out);
		ᬮᬶᬬ ᬬᬾᬦ᭄ (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    static void
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    puur_naa_nka		c1,
    puur_naa_nka		c2)
{
    char_u	*name;

    ᬬᬾᬦ᭄ (fm->fmark.mark.lnum == 0)	// not set
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    ᬮᬶᬬ
	name = fm->fname;		// use name from .viminfo
    ᬬᬾᬦ᭄ (name != NULL && *name != NUL)
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

    ᬬᬾᬦ᭄ (fm->fmark.fnum != 0)
	vim_free(name);
}

    static void
write_viminfo_filemarks(FILE *fp)
{
    puur_naa_nka		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    puur_naa_nka		vi_idx;
    puur_naa_nka		idx;

    ᬬᬾᬦ᭄ (get_viminfo_parameter('f') == 0)
	ᬯᬮᬶ;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    for (i = 0; i < NMARKS; i++)
    {
	ᬬᬾᬦ᭄ (vi_namedfm != NULL
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	ᬮᬶᬬ
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    ᬬᬾᬦ᭄ (curbuf->b_ffname != NULL && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	for (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    ᬬᬾᬦ᭄ (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == NULL
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != NULL
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		bha_nga;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	for ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = NULL;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    for (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != NULL ? &vi_namedfm[vi_idx] : NULL;

	ᬬᬾᬦ᭄ (vi_fm != NULL
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ᬮᬶᬬ
	{
	    fm = &namedfm_p[idx++];
	    ᬬᬾᬦ᭄ (vi_fm != NULL
		  && vi_fm->fmark.mark.lnum == fm->fmark.mark.lnum
		  && vi_fm->time_set == fm->time_set
		  && ((vi_fm->fmark.fnum != 0
			  && vi_fm->fmark.fnum == fm->fmark.fnum)
		      || (vi_fm->fname != NULL
			  && fm->fname != NULL
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
    for (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : NULL;
	vi_fm = (vi_jumplist != NULL && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : NULL;
	ᬬᬾᬦ᭄ (fm == NULL && vi_fm == NULL)
	    bha_nga;
	ᬬᬾᬦ᭄ (fm == NULL || (vi_fm != NULL && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	ᬮᬶᬬ
	    --idx;
	ᬬᬾᬦ᭄ (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != NULL
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    puur_naa_nka
buf_compare(const void *s1, const void *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    ᬬᬾᬦ᭄ (buf1->b_last_used == buf2->b_last_used)
	ᬯᬮᬶ 0;
    ᬯᬮᬶ buf1->b_last_used > buf2->b_last_used ? -1 : 1;
}

/*
 * Handle marks in the viminfo file:
 * fp_out != NULL: copy marks, in time order with buffers in "buflist".
 * fp_out == NULL && (flags & VIF_WANT_MARKS): read marks for curbuf
 * fp_out == NULL && (flags & VIF_ONLY_CURBUF): bail out after curbuf marks
 * fp_out == NULL && (flags & VIF_GET_OLDFILES | VIF_FORCEIT): fill v:oldfiles
 */
    static void
copy_viminfo_marks(
    vir_T	*virp,
    FILE	*fp_out,
    garray_T	*buflist,
    puur_naa_nka		eof,
    puur_naa_nka		flags)
{
    char_u	*line = virp->vir_line;
    buf_T	*buf;
    puur_naa_nka		num_marked_files;
    puur_naa_nka		load_marks;
    puur_naa_nka		copy_marks_out;
    char_u	*str;
    puur_naa_nka		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = NULL;
#endif
    puur_naa_nka		count = 0;
    puur_naa_nka		buflist_used = 0;
    buf_T	*buflist_buf = NULL;

    ᬬᬾᬦ᭄ ((name_buf = alloc(LSIZE)) == NULL)
	ᬯᬮᬶ;
    *name_buf = NUL;

    ᬬᬾᬦ᭄ (fp_out != NULL && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						parimaa_na(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    ᬬᬾᬦ᭄ (fp_out == NULL && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	ᬬᬾᬦ᭄ (list != NULL)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    num_marked_files = get_viminfo_parameter('\'');
    ᬲᭂᬤᭂᬮ (!eof && (count < num_marked_files || fp_out == NULL))
    {
	ᬬᬾᬦ᭄ (line[0] != '>')
	{
	    ᬬᬾᬦ᭄ (line[0] != '\n' && line[0] != '\r' && line[0] != '#')
	    {
		ᬬᬾᬦ᭄ (viminfo_error("E576: ", _(e_nonr_missing_gt), line))
		    bha_nga;	// too many errors, return now
	    }
	    eof = vim_fgets(line, LSIZE, virp->vir_fd);
	    continue;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(line + 1);
	str = viminfo_readstring(virp, (puur_naa_nka)(str - virp->vir_line), FALSE);
	ᬬᬾᬦ᭄ (str == NULL)
	    continue;
	p = str + STRLEN(str);
	ᬲᭂᬤᭂᬮ (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	ᬬᬾᬦ᭄ (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	ᬬᬾᬦ᭄ (list != NULL)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	ᬬᬾᬦ᭄ (fp_out == NULL)
	{
	    ᬬᬾᬦ᭄ ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != NULL)
	    {
		ᬬᬾᬦ᭄ (*name_buf == NUL)	    // only need to do this once
		    home_replace(NULL, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		ᬬᬾᬦ᭄ (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	ᬮᬶᬬ // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		ᬬᬾᬦ᭄ (buf->b_ffname != NULL)
		{
		    home_replace(NULL, buf->b_ffname, name_buf, LSIZE, TRUE);
		    ᬬᬾᬦ᭄ (fnamecmp(str, name_buf) == 0)
			bha_nga;
		}

	    // Copy marks if the buffer has not been loaded.
	    ᬬᬾᬦ᭄ (buf == NULL || !buf->b_marks_read)
	    {
		puur_naa_nka	did_read_line = FALSE;

		ᬬᬾᬦ᭄ (buflist_buf != NULL)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    ᬬᬾᬦ᭄ (!viminfo_readline(virp) && line[0] == TAB)
		    {
			did_read_line = TRUE;
			ᬬᬾᬦ᭄ (line[1] == '*')
			{
			    long	ltime;

			    sscanf((var_na *)line + 2, "%ld ", &ltime);
			    ᬲᭂᬤᭂᬮ ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				ᬬᬾᬦ᭄ (++count >= num_marked_files)
				    bha_nga;
				ᬬᬾᬦ᭄ (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = NULL;
				    bha_nga;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			ᬮᬶᬬ
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    ᬲᭂᬤᭂᬮ (count < num_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++count;
			    }
			    buflist_buf = NULL;
			}

			ᬬᬾᬦ᭄ (count >= num_marked_files)
			{
			    vim_free(str);
			    bha_nga;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		ᬬᬾᬦ᭄ (did_read_line)
		    fputs((var_na *)line, fp_out);

		count++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	ᬲᭂᬤᭂᬮ (!(eof = viminfo_readline(virp)) && line[0] == TAB)
	{
	    ᬬᬾᬦ᭄ (load_marks)
	    {
		ᬬᬾᬦ᭄ (line[1] != NUL)
		{
		    unsigned u;

		    sscanf((var_na *)line + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    switch (line[1])
		    {
			pak_sa '"': curbuf->b_last_cursor = pos; bha_nga;
			pak_sa '^': curbuf->b_last_insert = pos; bha_nga;
			pak_sa '.': curbuf->b_last_change = pos; bha_nga;
			pak_sa '+':
				  // changelist positions are stored oldest
				  // first
				  ᬬᬾᬦ᭄ (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    parimaa_na(pos_T) * (JUMPLISTSIZE - 1));
				  ᬮᬶᬬ
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  bha_nga;

				  // Using the line number for the last-used
				  // timestamp.
			pak_sa '*': curbuf->b_last_used = pos.lnum; bha_nga;

			default:  ᬬᬾᬦ᭄ ((i = line[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (copy_marks_out)
		fputs((var_na *)line, fp_out);
	}

	ᬬᬾᬦ᭄ (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		ᬬᬾᬦ᭄ (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    ᬬᬾᬦ᭄ (flags & VIF_ONLY_CURBUF)
		bha_nga;
	}
    }

    ᬬᬾᬦ᭄ (fp_out != NULL)
	// Write any remaining entries from buflist.
	ᬲᭂᬤᭂᬮ (count < num_marked_files && buflist_used < buflist->ga_len)
	{
	    buflist_buf = ((buf_T **)buflist->ga_data)[buflist_used++];
	    write_buffer_marks(buflist_buf, fp_out);
	    ++count;
	}

    vim_free(name_buf);
}

/*
 * Read marks for the current buffer from the viminfo file, when we support
 * buffer marks and the buffer has a name.
 */
    void
check_marks_read(void)
{
    ᬬᬾᬦ᭄ (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != NULL)
	read_viminfo(NULL, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    static puur_naa_nka
read_viminfo_filemark(vir_T *virp, puur_naa_nka force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    puur_naa_nka		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_line + 1;
    ᬬᬾᬦ᭄ (*str <= 127
	    && ((*virp->vir_line == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_line == '-' && *str == '\'')))
    {
	ᬬᬾᬦ᭄ (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    ᬬᬾᬦ᭄ (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = NULL;
	    ᬮᬶᬬ
	    {
		for (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = NULL;
	    }
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	ᬮᬶᬬ
	    fm = &namedfm_p[*str - 'A'];
	ᬬᬾᬦ᭄ (fm != NULL && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (puur_naa_nka)(str - virp->vir_line),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    ᬯᬮᬶ vim_fgets(virp->vir_line, LSIZE, virp->vir_fd);
}

/*
 * Prepare for reading viminfo marks when writing viminfo later.
 */
    static void
prepare_viminfo_marks(void)
{
    vi_namedfm = ALLOC_CLEAR_MULT(xfmark_T, NMARKS + EXTRA_MARKS);
    vi_jumplist = ALLOC_CLEAR_MULT(xfmark_T, JUMPLISTSIZE);
    vi_jumplist_len = 0;
}

    static void
finish_viminfo_marks(void)
{
    puur_naa_nka		i;

    ᬬᬾᬦ᭄ (vi_namedfm != NULL)
    {
	for (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    ᬬᬾᬦ᭄ (vi_jumplist != NULL)
    {
	for (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    static void
handle_viminfo_mark(garray_T *values, puur_naa_nka force)
{
    bval_T	*vp = (bval_T *)values->ga_data;
    puur_naa_nka		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = NULL;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    ᬬᬾᬦ᭄ (values->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	ᬯᬮᬶ;

    name = vp[0].bv_nr;
    ᬬᬾᬦ᭄ (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	ᬯᬮᬶ;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    ᬬᬾᬦ᭄ (lnum <= 0 || col < 0)
	ᬯᬮᬶ;
    timestamp = (time_t)vp[3].bv_nr;

    ᬬᬾᬦ᭄ (name == '\'')
    {
	ᬬᬾᬦ᭄ (vi_jumplist != NULL)
	{
	    ᬬᬾᬦ᭄ (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	ᬮᬶᬬ
	{
	    puur_naa_nka idx;
	    puur_naa_nka i;

	    // If we have a timestamp insert it in the right place.
	    ᬬᬾᬦ᭄ (timestamp != 0)
	    {
		for (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    ᬬᬾᬦ᭄ (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			bha_nga;
		    }
		// idx cannot be zero now
		ᬬᬾᬦ᭄ (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    ᬮᬶᬬ
		idx = -1;

	    ᬬᬾᬦ᭄ (idx >= 0)
	    {
		ᬬᬾᬦ᭄ (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    for (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		ᬮᬶᬬ
		{
		    // Move newer entries forward.
		    for (i = curwin->w_jumplistlen; i > idx; --i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		    ++curwin->w_jumplistidx;
		    ++curwin->w_jumplistlen;
		}
		fm = &curwin->w_jumplist[idx];
		fm->fmark.mark.lnum = 0;
		fm->fname = NULL;
		fm->time_set = 0;
	    }
	}
    }
    ᬮᬶᬬ
    {
	puur_naa_nka		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	ᬬᬾᬦ᭄ (VIM_ISDIGIT(name))
	{
	    ᬬᬾᬦ᭄ (vi_namedfm != NULL)
		idx = name - '0' + NMARKS;
	    ᬮᬶᬬ
	    {
		puur_naa_nka i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		for (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    ᬬᬾᬦ᭄ (namedfm_p[idx].time_set < timestamp)
			bha_nga;
		ᬬᬾᬦ᭄ (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    ᬯᬮᬶ;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		for ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = NULL;
	    }
	}
	ᬮᬶᬬ
	    idx = name - 'A';
	ᬬᬾᬦ᭄ (vi_namedfm != NULL)
	    fm = &vi_namedfm[idx];
	ᬮᬶᬬ
	    fm = &namedfm_p[idx];
    }

    ᬬᬾᬦ᭄ (fm != NULL)
    {
	ᬬᬾᬦ᭄ (vi_namedfm != NULL || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    ᬬᬾᬦ᭄ (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = NULL;
	    }
	    ᬮᬶᬬ
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    static puur_naa_nka
read_viminfo_barline(vir_T *virp, puur_naa_nka got_encoding, puur_naa_nka force, puur_naa_nka writing)
{
    char_u	*p = virp->vir_line + 1;
    puur_naa_nka		bartype;
    garray_T	values;
    bval_T	*vp;
    puur_naa_nka		i;
    puur_naa_nka		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    ᬬᬾᬦ᭄ (*p == '<')
    {
	// Continuation line of an unrecognized item.
	ᬬᬾᬦ᭄ (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_line);
    }
    ᬮᬶᬬ
    {
	ga_init2(&values, parimaa_na(bval_T), 20);
	bartype = getdigits(&p);
	switch (bartype)
	{
	    pak_sa BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		ᬬᬾᬦ᭄ (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &values);
		    vp = (bval_T *)values.ga_data;
		    ᬬᬾᬦ᭄ (values.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		bha_nga;

	    pak_sa BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &values);
		handle_viminfo_history(&values, writing);
		bha_nga;

	    pak_sa BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &values);
		handle_viminfo_register(&values, force);
		bha_nga;

	    pak_sa BARTYPE_MARK:
		read_next = barline_parse(virp, p, &values);
		handle_viminfo_mark(&values, force);
		bha_nga;

	    default:
		// copy unrecognized line (for future use)
		ᬬᬾᬦ᭄ (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_line);
	}
	for (i = 0; i < values.ga_len; ++i)
	{
	    vp = (bval_T *)values.ga_data + i;
	    ᬬᬾᬦ᭄ (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&values);
    }

    ᬬᬾᬦ᭄ (read_next)
	ᬯᬮᬶ viminfo_readline(virp);
    ᬯᬮᬶ FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    static puur_naa_nka
read_viminfo_up_to_marks(
    vir_T	*virp,
    puur_naa_nka		forceit,
    puur_naa_nka		writing)
{
    puur_naa_nka		eof;
    buf_T	*buf;
    puur_naa_nka		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    ᬲᭂᬤᭂᬮ (!eof && virp->vir_line[0] != '>')
    {
	switch (virp->vir_line[0])
	{
		// Characters reserved for future expansion, ignored now
	    pak_sa '+': // "+40 /path/dir file", for running vim without args
	    pak_sa '^': // to be defined
	    pak_sa '<': // long line - ignored
		// A comment or empty line.
	    pak_sa NUL:
	    pak_sa '\r':
	    pak_sa '\n':
	    pak_sa '#':
		eof = viminfo_readline(virp);
		bha_nga;
	    pak_sa '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		bha_nga;
	    pak_sa '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		bha_nga;
	    pak_sa '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#ᬮᬶᬬ
		eof = viminfo_readline(virp);
#endif
		bha_nga;
	    pak_sa '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		bha_nga;
	    pak_sa '"':
		// When registers are in bar lines skip the old style register
		// lines.
		ᬬᬾᬦ᭄ (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		ᬮᬶᬬ
		    do
		    {
			eof = viminfo_readline(virp);
		    } ᬲᭂᬤᭂᬮ (!eof && (virp->vir_line[0] == TAB
						|| virp->vir_line[0] == '<'));
		bha_nga;
	    pak_sa '/':	    // Search string
	    pak_sa '&':	    // Substitute search string
	    pak_sa '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_search_pattern(virp, forceit, writing);
		bha_nga;
	    pak_sa '$':
		eof = read_viminfo_sub_string(virp, forceit);
		bha_nga;
	    pak_sa ':':
	    pak_sa '?':
	    pak_sa '=':
	    pak_sa '@':
		// When history is in bar lines skip the old style history
		// lines.
		ᬬᬾᬦ᭄ (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		ᬮᬶᬬ
		    eof = viminfo_readline(virp);
		bha_nga;
	    pak_sa '-':
	    pak_sa '\'':
		// When file marks are in bar lines skip the old style lines.
		ᬬᬾᬦ᭄ (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		ᬮᬶᬬ
		    eof = viminfo_readline(virp);
		bha_nga;
	    default:
		ᬬᬾᬦ᭄ (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_line))
		    eof = TRUE;
		ᬮᬶᬬ
		    eof = viminfo_readline(virp);
		bha_nga;
	}
    }

    // Finish reading history items.
    ᬬᬾᬦ᭄ (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    ᬯᬮᬶ eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    static void
do_viminfo(FILE *fp_in, FILE *fp_out, puur_naa_nka flags)
{
    puur_naa_nka		eof = FALSE;
    vir_T	vir;
    puur_naa_nka		merge = FALSE;
    puur_naa_nka		do_copy_marks = FALSE;
    garray_T	buflist;

    ᬬᬾᬦ᭄ ((vir.vir_line = alloc(LSIZE)) == NULL)
	ᬯᬮᬶ;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, parimaa_na(char_u *), 100);
    vir.vir_version = -1;

    ᬬᬾᬦ᭄ (fp_in != NULL)
    {
	ᬬᬾᬦ᭄ (flags & VIF_WANT_INFO)
	{
	    ᬬᬾᬦ᭄ (fp_out != NULL)
	    {
		// Registers and marks are read and kept separate from what
		// this Vim is using.  They are merged when writing.
		prepare_viminfo_registers();
		prepare_viminfo_marks();
	    }

	    eof = read_viminfo_up_to_marks(&vir,
					 flags & VIF_FORCEIT, fp_out != NULL);
	    merge = TRUE;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (flags != 0)
	    // Skip info, find start of marks
	    ᬲᭂᬤᭂᬮ (!(eof = viminfo_readline(&vir))
		    && vir.vir_line[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    ᬬᬾᬦ᭄ (fp_out != NULL)
    {
	// Write the info:
	fprintf(fp_out, _("# This viminfo file was generated by Vim %s.\n"),
							  VIM_VERSION_MEDIUM);
	fputs(_("# You may edit it if you're careful!\n\n"), fp_out);
	write_viminfo_version(fp_out);
	fputs(_("# Value of 'encoding' when this file was written\n"), fp_out);
	fprintf(fp_out, "*encoding=%s\n\n", p_enc);
	write_viminfo_search_pattern(fp_out);
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

	ᬬᬾᬦ᭄ (do_copy_marks)
	    ga_init2(&buflist, parimaa_na(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : NULL);
    }

    ᬬᬾᬦ᭄ (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	ᬬᬾᬦ᭄ (fp_out != NULL)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_line);
    ᬬᬾᬦ᭄ (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, NULL, NULL);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    puur_naa_nka
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    puur_naa_nka		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    ᬬᬾᬦ᭄ (no_viminfo())
	ᬯᬮᬶ FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    ᬬᬾᬦ᭄ (fname == NULL)
	ᬯᬮᬶ FAIL;
    fp = mch_fopen((var_na *)fname, READBIN);

    ᬬᬾᬦ᭄ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading viminfo file \"%s\"%s%s%s%s"),
		fname,
		(flags & VIF_WANT_INFO) ? _(" info") : "",
		(flags & VIF_WANT_MARKS) ? _(" marks") : "",
		(flags & VIF_GET_OLDFILES) ? _(" oldfiles") : "",
		fp == NULL ? _(" FAILED") : "");
	verbose_leave();
    }

    vim_free(fname);
    ᬬᬾᬦ᭄ (fp == NULL)
	ᬯᬮᬶ FAIL;
    ᬬᬾᬦ᭄ (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	ᬯᬮᬶ FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, NULL, flags);

    fclose(fp);
    ᬯᬮᬶ OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    void
write_viminfo(char_u *file, puur_naa_nka forceit)
{
    char_u	*fname;
    FILE	*fp_in = NULL;	// input viminfo file, if any
    FILE	*fp_out = NULL;	// output viminfo file
    char_u	*tempname = NULL;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#ᬬᬾᬦ᭄ defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    puur_naa_nka		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    puur_naa_nka		hidden = FALSE;
#endif

    ᬬᬾᬦ᭄ (no_viminfo())
	ᬯᬮᬶ;

    fname = viminfo_filename(file);	// may set to default if NULL
    ᬬᬾᬦ᭄ (fname == NULL)
	ᬯᬮᬶ;

    fp_in = mch_fopen((var_na *)fname, READBIN);
    ᬬᬾᬦ᭄ (fp_in == NULL)
    {
	puur_naa_nka fd;

	// if it does exist, but we can't read it, don't try writing
	ᬬᬾᬦ᭄ (mch_stat((var_na *)fname, &st_new) == 0)
	    goto end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((var_na *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	ᬬᬾᬦ᭄ (fd < 0)
	    goto end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    ᬮᬶᬬ
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	ᬬᬾᬦ᭄ (mch_fstat(fileno(fp_in), &st_old) < 0
		|| S_ISDIR(st_old.st_mode)
#ifdef UNIX
		// For Unix we check the owner of the file.  It's not very nice
		// to overwrite a user's viminfo file after a "su root", with a
		// viminfo file that the user can't read.
		|| (getuid() != ROOT_UID
		    && !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
#endif
		)
	{
	    puur_naa_nka	tt = msg_didany;

	    // avoid a wait_return() for this message, it's annoying
	    semsg(_(e_viminfo_file_is_not_writable_str), fname);
	    msg_didany = tt;
	    fclose(fp_in);
	    goto end;
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
	for (;;)
	{
	    puur_naa_nka		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#ᬮᬶᬬ
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#ᬮᬶᬬ
				    (char_u *)".tmp",
#endif
				    FALSE);
	    ᬬᬾᬦ᭄ (tempname == NULL)		// out of memory
		bha_nga;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    ᬬᬾᬦ᭄ (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    for (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		ᬬᬾᬦ᭄ (mch_stat((var_na *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    ᬬᬾᬦ᭄ (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			bha_nga;
		    }
#endif
		}
		ᬮᬶᬬ
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((var_na *)tempname, WRITEBIN);
		    (void)umask(umask_save);
#ᬮᬶᬬ
		    puur_naa_nka	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((var_na *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(puur_naa_nka)((st_old.st_mode & 0777) | 0600));
		    (void)umask(umask_save);
# ᬮᬶᬬ
		    fd = mch_open((var_na *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    ᬬᬾᬦ᭄ (fd < 0)
		    {
			fp_out = NULL;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			ᬬᬾᬦ᭄ (errno != EEXIST)
			    bha_nga;
# endif
		    }
		    ᬮᬶᬬ
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    ᬬᬾᬦ᭄ (fp_out != NULL)
			bha_nga;
		}

		// Assume file exists, try again with another name.
		ᬬᬾᬦ᭄ (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    bha_nga;
		}
		*wp = next_char;
		--next_char;
	    }

	    ᬬᬾᬦ᭄ (tempname != NULL)
		bha_nga;
	    // continue if shortname was set
	}

#ᬬᬾᬦ᭄ defined(UNIX) && defined(HAVE_FCHOWN)
	ᬬᬾᬦ᭄ (tempname != NULL && fp_out != NULL)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    ᬬᬾᬦ᭄ (mch_stat((var_na *)tempname, &tmp_st) >= 0)
	    {
		ᬬᬾᬦ᭄ (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		ᬬᬾᬦ᭄ (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (void)mch_setperm(tempname, 0600);
	    }
	    ᬮᬶᬬ
		// can't stat the file, set conservative permissions
		(void)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    ᬬᬾᬦ᭄ (fp_out == NULL)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == NULL || tempname == NULL) ? fname : tempname);
	ᬬᬾᬦ᭄ (fp_in != NULL)
	    fclose(fp_in);
	goto end;
    }

    ᬬᬾᬦ᭄ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    ᬬᬾᬦ᭄ (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    ᬬᬾᬦ᭄ (fp_in != NULL)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	ᬬᬾᬦ᭄ (viminfo_errcnt == 0)
	{
	    ᬬᬾᬦ᭄ (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (hidden)
		mch_hide(fname);
#endif
	}
	ᬬᬾᬦ᭄ (viminfo_errcnt > 0)
	    mch_remove(tempname);
    }

end:
    vim_free(fname);
    vim_free(tempname);
}

/*
 * ":rviminfo" and ":wviminfo".
 */
    void
ex_viminfo(
    exarg_T	*eap)
{
    char_u	*save_viminfo;

    save_viminfo = p_viminfo;
    ᬬᬾᬦ᭄ (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    ᬬᬾᬦ᭄ (eap->cmdidx == CMD_rviminfo)
    {
	ᬬᬾᬦ᭄ (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    ᬮᬶᬬ
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
