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
typedef struct
{
    char_u	*vir_line;	// text of the current line
    FILE	*vir_fd;	// file descriptor
    vimconv_T	vir_conv;	// encoding conversion
    정수		vir_version;	// viminfo version detected or -1
    garray_T	vir_barlines;	// lines starting with |
} vir_T;

typedef enum {
    BVAL_NR,
    BVAL_STRING,
    BVAL_EMPTY
} btype_T;

typedef struct {
    btype_T	bv_type;
    long	bv_nr;
    char_u	*bv_string;
    char_u	*bv_tofree;	// free later when not NULL
    정수		bv_len;		// length of bv_string
    정수		bv_allocated;	// bv_string was allocated
} bval_T;

#만약 defined(FEAT_VIMINFO)

static 정수  viminfo_errcnt;

/*
 * Find the parameter represented by the given character (eg ''', ':', '"', or
 * '/') in the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    static char_u *
find_viminfo_parameter(정수 type)
{
    char_u  *p;

    위해 (p = p_viminfo; *p; ++p)
    {
	만약 (*p == type)
	    반환 p + 1;
	만약 (*p == 'n')		    // 'n' is always the last one
	    break;
	p = vim_strchr(p, ',');	    // skip until next ','
	만약 (p == NULL)		    // hit the end without finding parameter
	    break;
    }
    반환 NULL;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string or there is no following
 * number, return -1.
 */
    정수
get_viminfo_parameter(정수 type)
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    만약 (p != NULL && VIM_ISDIGIT(*p))
	반환 atoi((char *)p);
    반환 -1;
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
    만약 (file == NULL || *file == NUL)
    {
	size_t  len;

	만약 (*p_viminfofile != NUL)
	    file = p_viminfofile;
	아니면 만약 ((file = find_viminfo_parameter('n')) == NULL || *file == NUL)
	{
#ifdef VIMINFO_FILE2
# ifdef VMS
	    만약 (mch_getenv((char_u *)"SYS$LOGIN") == NULL)
# 아니면
#  ifdef MSWIN
	    // Use $VIM only if $HOME is the default "C:/".
	    만약 (STRCMP(vim_getenv((char_u *)"HOME", NULL), "C:/") == 0
		    && mch_getenv((char_u *)"HOME") == NULL)
#  아니면
	    만약 (mch_getenv((char_u *)"HOME") == NULL)
#  endif
# endif
	    {
		// don't use $VIM when not available.
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		만약 (STRCMP("$VIM", NameBuff) != 0)  // $VIM was expanded
		    file = (char_u *)VIMINFO_FILE2;
		아니면
		    file = (char_u *)VIMINFO_FILE;
	    }
	    아니면
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	len = expand_env(file, NameBuff, MAXPATHL);
	file = NameBuff;

	반환 vim_strnsave(file, len);
    }

    반환 vim_strsave(file);
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
    정수		c;
    char_u	*s;
    정수		len = 0;

    위해 (s = p; *s != NUL; ++s)
    {
	만약 (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    // If the string will be too long, write its length and put it in the next
    // line.  Take into account that some room is needed for what comes before
    // the string (e.g., variable name).  Add something to the length for the
    // '<', NL and trailing NUL.
    만약 (len > LSIZE / 2)
	fprintf(fd, "\026%d\n<", len + 3);

    동안 ((c = *p++) != NUL)
    {
	만약 (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    만약 (c == '\n')
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
    static 정수
barline_writestring(FILE *fd, char_u *s, 정수 remaining_start)
{
    char_u *p;
    정수	    remaining = remaining_start;
    정수	    len = 2;

    // Count the number of characters produced, including quotes.
    위해 (p = s; *p != NUL; ++p)
    {
	만약 (*p == NL)
	    len += 2;
	아니면 만약 (*p == '"' || *p == '\\')
	    len += 2;
	아니면
	    ++len;
    }
    만약 (len > remaining - 2)
    {
	fprintf(fd, ">%d\n|<", len);
	remaining = LSIZE - 20;
    }

    putc('"', fd);
    위해 (p = s; *p != NUL; ++p)
    {
	만약 (*p == NL)
	{
	    putc('\\', fd);
	    putc('n', fd);
	    --remaining;
	}
	아니면 만약 (*p == '"' || *p == '\\')
	{
	    putc('\\', fd);
	    putc(*p, fd);
	    --remaining;
	}
	아니면
	    putc(*p, fd);
	--remaining;

	만약 (remaining < 3)
	{
	    putc('\n', fd);
	    putc('|', fd);
	    putc('<', fd);
	    // Leave enough space for another continuation.
	    remaining = LSIZE - 20;
	}
    }
    putc('"', fd);
    반환 remaining - 2;
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
    정수		off,		    // offset for virp->vir_line
    정수		convert)	    // convert the string
{
    char_u	*retval = NULL;
    char_u	*s, *d;
    long	len;

    만약 (virp->vir_line[off] == Ctrl_V && vim_isdigit(virp->vir_line[off + 1]))
    {
	len = atol((char *)virp->vir_line + off + 1);
	만약 (len > 1 && len < 1000000)
	    retval = lalloc(len, TRUE);
	만약 (retval == NULL)
	{
	    // Invalid length, line too long, out of memory?  Skip next line.
	    (void)vim_fgets(virp->vir_line, 10, virp->vir_fd);
	    반환 NULL;
	}
	(void)vim_fgets(retval, (정수)len, virp->vir_fd);
	s = retval + 1;	    // Skip the leading '<'
    }
    아니면
    {
	retval = vim_strsave(virp->vir_line + off);
	만약 (retval == NULL)
	    반환 NULL;
	s = retval;
    }

    // Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place.
    d = retval;
    동안 (*s != NUL && *s != '\n')
    {
	만약 (s[0] == Ctrl_V && s[1] != NUL)
	{
	    만약 (s[1] == 'n')
		*d++ = '\n';
	    아니면
		*d++ = Ctrl_V;
	    s += 2;
	}
	아니면
	    *d++ = *s++;
    }
    *d = NUL;

    만약 (convert && virp->vir_conv.vc_type != CONV_NONE && *retval != NUL)
    {
	d = string_convert(&virp->vir_conv, retval, NULL);
	만약 (d != NULL)
	{
	    vim_free(retval);
	    retval = d;
	}
    }

    반환 retval;
}

/*
 * Read a line from the viminfo file.
 * Returns TRUE for end-of-file;
 */
    static 정수
viminfo_readline(vir_T *virp)
{
    반환 vim_fgets(virp->vir_line, LSIZE, virp->vir_fd);
}

    static 정수
read_viminfo_bufferlist(
    vir_T	*virp,
    정수		writing)
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
    만약 (xline != NULL && !writing && ARGCOUNT == 0
				       && find_viminfo_parameter('%') != NULL)
    {
	// Format is: <fname> Tab <lnum> Tab <col>.
	// Watch out for a Tab in the file name, work from the end.
	lnum = 0;
	col = 0;
	tab = vim_strrchr(xline, '\t');
	만약 (tab != NULL)
	{
	    *tab++ = '\0';
	    col = (colnr_T)atoi((char *)tab);
	    tab = vim_strrchr(xline, '\t');
	    만약 (tab != NULL)
	    {
		*tab++ = '\0';
		lnum = atol((char *)tab);
	    }
	}

	// Expand "~/" in the file name at "line + 1" to a full path.
	// Then try shortening it by comparing with the current directory
	expand_env(xline, NameBuff, MAXPATHL);
	sfname = shorten_fname1(NameBuff);

	buf = buflist_new(NameBuff, sfname, (linenr_T)0, BLN_LISTED);
	만약 (buf != NULL)	// just in case...
	{
	    buf->b_last_cursor.lnum = lnum;
	    buf->b_last_cursor.col = col;
	    buflist_setfpos(buf, curwin, lnum, col, FALSE);
	}
    }
    vim_free(xline);

    반환 viminfo_readline(virp);
}

/*
 * Return TRUE if "name" is on removable media (depending on 'viminfo').
 */
    static 정수
removable(char_u *name)
{
    char_u  *p;
    char_u  part[51];
    정수	    part_len;
    정수	    retval = FALSE;

    name = home_replace_save(NULL, name);
    만약 (name == NULL)
	반환 FALSE;
    위해 (p = p_viminfo; *p; )
    {
	part_len = copy_option_part(&p, part, sizeof(part), ", ");
	만약 (part[0] == 'r')
	{
	    만약 (MB_STRNICMP(part + 1, name, part_len - 1) == 0)
	    {
		retval = TRUE;
		break;
	    }
	}
    }
    vim_free(name);
    반환 retval;
}

    static void
write_viminfo_bufferlist(FILE *fp)
{
    buf_T	*buf;
    win_T	*win;
    tabpage_T	*tp;
    char_u	*line;
    정수		max_buffers;

    만약 (find_viminfo_parameter('%') == NULL)
	반환;

    // Without a number -1 is returned: do all buffers.
    max_buffers = get_viminfo_parameter('%');

    // Allocate room for the file name, lnum and col.
#define LINE_BUF_LEN (MAXPATHL + 40)
    line = alloc(LINE_BUF_LEN);
    만약 (line == NULL)
	반환;

    FOR_ALL_TAB_WINDOWS(tp, win)
	set_last_cursor(win);

    fputs(_("\n# Buffer list:\n"), fp);
    FOR_ALL_BUFFERS(buf)
    {
	size_t	linelen;

	만약 (buf->b_fname == NULL
		|| !buf->b_p_bl
		|| bt_quickfix(buf)
		|| bt_terminal(buf)
		|| removable(buf->b_ffname))
	    continue;

	만약 (max_buffers-- == 0)
	    break;
	putc('%', fp);
	linelen = home_replace(NULL, buf->b_ffname, line, MAXPATHL, TRUE);
	vim_snprintf((char *)line + linelen, LINE_BUF_LEN - linelen, "\t%ld\t%d",
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
static 정수	viminfo_hisidx[HIST_COUNT] = {0, 0, 0, 0, 0};
static 정수	viminfo_hislen[HIST_COUNT] = {0, 0, 0, 0, 0};
static 정수	viminfo_add_at_front = FALSE;

/*
 * Translate a history type number to the associated character.
 */
    static 정수
hist_type2char(
    정수	    type,
    정수	    use_question)	    // use '?' instead of '/'
{
    만약 (type == HIST_CMD)
	반환 ':';
    만약 (type == HIST_SEARCH)
    {
	만약 (use_question)
	    반환 '?';
	아니면
	    반환 '/';
    }
    만약 (type == HIST_EXPR)
	반환 '=';
    반환 '@';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    static void
prepare_viminfo_history(정수 asklen, 정수 writing)
{
    정수	    i;
    정수	    num;
    정수	    type;
    정수	    len;
    정수	    hislen;

    init_history();
    hislen = get_hislen();
    viminfo_add_at_front = (asklen != 0 && !writing);
    만약 (asklen > hislen)
	asklen = hislen;

    위해 (type = 0; type < HIST_COUNT; ++type)
    {
	histentry_T *histentry = get_histentry(type);

	// Count the number of empty spaces in the history list.  Entries read
	// from viminfo previously are also considered empty.  If there are
	// more spaces available than we request, then fill them up.
	위해 (i = 0, num = 0; i < hislen; i++)
	    만약 (histentry[i].hisstr == NULL || histentry[i].viminfo)
		num++;
	len = asklen;
	만약 (num > len)
	    len = num;
	만약 (len <= 0)
	    viminfo_history[type] = NULL;
	아니면
	    viminfo_history[type] = LALLOC_MULT(histentry_T, len);
	만약 (viminfo_history[type] == NULL)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    static 정수
read_viminfo_history(vir_T *virp, 정수 writing)
{
    정수		type;
    long_u	len;
    char_u	*val = NULL;
    char_u	*p;

    type = hist_char2type(virp->vir_line[0]);
    만약 (viminfo_hisidx[type] >= viminfo_hislen[type])
	goto done;

    val = viminfo_readstring(virp, 1, TRUE);
    만약 (val == NULL || *val == NUL)
	goto done;

    정수 sep = (*val == ' ' ? NUL : *val);

    만약 (in_history(type, val + (type == HIST_SEARCH), viminfo_add_at_front,
								sep, writing))
	goto done;

    // Need to re-allocate to append the separator byte.
    len = STRLEN(val);
    만약 (type == HIST_SEARCH)
    {
	p = alloc((size_t)len + 1);	    // +1 for the NUL. val already
					    // includes the separator.
	만약 (p == NULL)
	    goto done;

	// Search entry: Move the separator from the first
	// column to after the NUL.
	mch_memmove(p, val + 1, (size_t)len);
	p[len] = sep;
	--len;				    // take into account the shortened string
    }
    아니면
    {
	p = alloc((size_t)len + 2);	    // +1 for NUL and +1 for separator
	만약 (p == NULL)
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
    반환 viminfo_readline(virp);
}

/*
 * Accept a new style history line from the viminfo, store it in the history
 * array when it's new.
 */
    static void
handle_viminfo_history(
	garray_T    *values,
	정수	    writing)
{
    정수		type;
    long_u	len;
    char_u	*val;
    char_u	*p;
    bval_T	*vp = (bval_T *)values->ga_data;

    // Check the format:
    // |{bartype},{histtype},{timestamp},{separator},"text"
    만약 (values->ga_len < 4
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || (vp[2].bv_type != BVAL_NR && vp[2].bv_type != BVAL_EMPTY)
	    || vp[3].bv_type != BVAL_STRING)
	반환;

    type = vp[0].bv_nr;
    만약 (type >= HIST_COUNT)
	반환;

    만약 (viminfo_hisidx[type] >= viminfo_hislen[type])
	반환;

    val = vp[3].bv_string;
    만약 (val == NULL || *val == NUL)
	반환;

    정수 sep = type == HIST_SEARCH && vp[2].bv_type == BVAL_NR
							? vp[2].bv_nr : NUL;
    정수 idx;
    정수 overwrite = FALSE;

    만약 (in_history(type, val, viminfo_add_at_front, sep, writing))
	반환;

    // If lines were written by an older Vim we need to avoid
    // getting duplicates. See if the entry already exists.
    위해 (idx = 0; idx < viminfo_hisidx[type]; ++idx)
    {
	p = viminfo_history[type][idx].hisstr;
	len = viminfo_history[type][idx].hisstrlen;
	만약 (STRCMP(val, p) == 0
		&& (type != HIST_SEARCH || sep == p[len + 1]))
	{
	    overwrite = TRUE;
	    break;
	}
    }

    만약 (!overwrite)
    {
	// Need to re-allocate to append the separator byte.
	len = vp[3].bv_len;
	p = alloc(len + 2);
    }
    아니면
	len = 0; // for picky compilers
    만약 (p != NULL)
    {
	viminfo_history[type][idx].time_set = vp[1].bv_nr;
	만약 (!overwrite)
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
concat_history(정수 type)
{
    정수		idx;
    정수		i;
    정수		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    정수		*hisidx = get_hisidx(type);
    정수		*hisnum = get_hisnum(type);

    idx = *hisidx + viminfo_hisidx[type];
    만약 (idx >= hislen)
	idx -= hislen;
    아니면 만약 (idx < 0)
	idx = hislen - 1;
    만약 (viminfo_add_at_front)
	*hisidx = idx;
    아니면
    {
	만약 (*hisidx == -1)
	    *hisidx = hislen - 1;
	do
	{
	    만약 (histentry[idx].hisstr != NULL || histentry[idx].viminfo)
		break;
	    만약 (++idx == hislen)
		idx = 0;
	} 동안 (idx != *hisidx);
	만약 (idx != *hisidx && --idx < 0)
	    idx = hislen - 1;
    }
    위해 (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(histentry[idx].hisstr);
	histentry[idx].hisstr = viminfo_history[type][i].hisstr;
	histentry[idx].hisstrlen = viminfo_history[type][i].hisstrlen;
	histentry[idx].viminfo = TRUE;
	histentry[idx].time_set = viminfo_history[type][i].time_set;
	만약 (--idx < 0)
	    idx = hislen - 1;
    }
    idx += 1;
    idx %= hislen;
    위해 (i = 0; i < viminfo_hisidx[type]; i++)
    {
	histentry[idx++].hisnum = ++*hisnum;
	idx %= hislen;
    }
}

    static 정수
sort_hist(const void *s1, const void *s2)
{
    histentry_T *p1 = *(histentry_T **)s1;
    histentry_T *p2 = *(histentry_T **)s2;

    만약 (p1->time_set < p2->time_set) 반환 -1;
    만약 (p1->time_set > p2->time_set) 반환 1;
    반환 0;
}

/*
 * Merge history lines from viminfo and lines typed in this Vim based on the
 * timestamp;
 */
    static void
merge_history(정수 type)
{
    정수		max_len;
    histentry_T **tot_hist;
    histentry_T *new_hist;
    정수		i;
    정수		len;
    정수		hislen = get_hislen();
    histentry_T *histentry = get_histentry(type);
    정수		*hisidx = get_hisidx(type);
    정수		*hisnum = get_hisnum(type);

    // Make one long list with all entries.
    max_len = hislen + viminfo_hisidx[type];
    tot_hist = ALLOC_MULT(histentry_T *, max_len);
    new_hist = ALLOC_MULT(histentry_T, hislen);
    만약 (tot_hist == NULL || new_hist == NULL)
    {
	vim_free(tot_hist);
	vim_free(new_hist);
	반환;
    }
    위해 (i = 0; i < viminfo_hisidx[type]; i++)
	tot_hist[i] = &viminfo_history[type][i];
    len = i;
    위해 (i = 0; i < hislen; i++)
	만약 (histentry[i].hisstr != NULL)
	    tot_hist[len++] = &histentry[i];

    // Sort the list on timestamp.
    qsort((void *)tot_hist, (size_t)len, sizeof(histentry_T *), sort_hist);

    // Keep the newest ones.
    위해 (i = 0; i < hislen; i++)
    {
	만약 (i < len)
	{
	    new_hist[i] = *tot_hist[i];
	    tot_hist[i]->hisstr = NULL;
	    tot_hist[i]->hisstrlen = 0;
	    만약 (new_hist[i].hisnum == 0)
		new_hist[i].hisnum = ++*hisnum;
	}
	아니면
	    clear_hist_entry(&new_hist[i]);
    }
    *hisidx = (i < len ? i : len) - 1;

    // Free what is not kept.
    위해 (i = 0; i < viminfo_hisidx[type]; i++)
    {
	vim_free(viminfo_history[type][i].hisstr);
	viminfo_history[type][i].hisstrlen = 0;
    }
    위해 (i = 0; i < hislen; i++)
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
    정수	type;
    정수 merge = virp->vir_version >= VIMINFO_VERSION_WITH_HISTORY;

    위해 (type = 0; type < HIST_COUNT; ++type)
    {
	만약 (get_histentry(type) == NULL)
	    continue;

	만약 (merge)
	    merge_history(type);
	아니면
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
write_viminfo_history(FILE *fp, 정수 merge)
{
    정수	    i;
    정수	    type;
    정수	    num_saved;
    정수     round;
    정수	    hislen;

    init_history();
    hislen = get_hislen();
    만약 (hislen == 0)
	반환;
    위해 (type = 0; type < HIST_COUNT; ++type)
    {
	histentry_T *histentry = get_histentry(type);
	정수	    *hisidx = get_hisidx(type);

	num_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	만약 (num_saved == 0)
	    continue;
	만약 (num_saved < 0)  // Use default
	    num_saved = hislen;
	fprintf(fp, _("\n# %s History (newest to oldest):\n"),
			    type == HIST_CMD ? _("Command Line") :
			    type == HIST_SEARCH ? _("Search String") :
			    type == HIST_EXPR ? _("Expression") :
			    type == HIST_INPUT ? _("Input Line") :
					_("Debug Line"));
	만약 (num_saved > hislen)
	    num_saved = hislen;

	// Merge typed and viminfo history:
	// round 1: history of typed commands.
	// round 2: history from recently read viminfo.
	위해 (round = 1; round <= 2; ++round)
	{
	    만약 (round == 1)
		// start at newest entry, somewhere in the list
		i = *hisidx;
	    아니면 만약 (viminfo_hisidx[type] > 0)
		// start at newest entry, first in the list
		i = 0;
	    아니면
		// empty list
		i = -1;
	    만약 (i >= 0)
		동안 (num_saved > 0
			&& !(round == 2 && i >= viminfo_hisidx[type]))
		{
		    char_u  *p;
		    size_t  plen;
		    time_t  timestamp;
		    정수	    c = NUL;

		    만약 (round == 1)
		    {
			p = histentry[i].hisstr;
			plen = histentry[i].hisstrlen;
			timestamp = histentry[i].time_set;
		    }
		    아니면
		    {
			만약 (viminfo_history[type] == NULL)
			{
			    p = NULL;
			    plen = 0;
			    timestamp = 0;
			}
			아니면
			{
			    p = viminfo_history[type][i].hisstr;
			    plen = viminfo_history[type][i].hisstrlen;
			    timestamp = viminfo_history[type][i].time_set;
			}
		    }

		    만약 (p != NULL && (round == 2
				       || !merge
				       || !histentry[i].viminfo))
		    {
			--num_saved;
			fputc(hist_type2char(type, TRUE), fp);
			// For the search history: put the separator in the
			// second column; use a space if there isn't one.
			만약 (type == HIST_SEARCH)
			{
			    c = p[plen + 1];
			    putc(c == NUL ? ' ' : c, fp);
			}
			viminfo_writestring(fp, p);

			{
			    char    cbuf[NUMBUFLEN];

			    // New style history with a bar line. Format:
			    // |{bartype},{histtype},{timestamp},{separator},"text"
			    만약 (c == NUL)
				cbuf[0] = NUL;
			    아니면
				sprintf(cbuf, "%d", c);
			    fprintf(fp, "|%d,%d,%ld,%s,", BARTYPE_HISTORY,
						 type, (long)timestamp, cbuf);
			    barline_writestring(fp, p, LSIZE - 20);
			    putc('\n', fp);
			}
		    }
		    만약 (round == 1)
		    {
			// Decrement index, loop around and stop when back at
			// the start.
			만약 (--i < 0)
			    i = hislen - 1;
			만약 (i == *hisidx)
			    break;
		    }
		    아니면
		    {
			// Increment index. Stop at the end in the while.
			++i;
		    }
		}
	}
	위해 (i = 0; i < viminfo_hisidx[type]; ++i)
	    만약 (viminfo_history[type] != NULL)
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
    정수		i;
    garray_T	*gap = &virp->vir_barlines;
    정수		seen_useful = FALSE;
    char	*line;

    만약 (gap->ga_len <= 0)
	반환;

    fputs(_("\n# Bar lines, copied verbatim:\n"), fp_out);

    // Skip over continuation lines until seeing a useful line.
    위해 (i = 0; i < gap->ga_len; ++i)
    {
	line = ((char **)(gap->ga_data))[i];
	만약 (seen_useful || line[1] != '<')
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
    static 정수
barline_parse(vir_T *virp, char_u *text, garray_T *values)
{
    char_u  *p = text;
    char_u  *nextp = NULL;
    char_u  *buf = NULL;
    bval_T  *value;
    정수	    i;
    정수	    allocated = FALSE;
    정수	    eof;
    char_u  *sconv;
    정수	    converted;

    동안 (*p == ',')
    {
	++p;
	만약 (ga_grow(values, 1) == FAIL)
	    break;
	value = (bval_T *)(values->ga_data) + values->ga_len;

	만약 (*p == '>')
	{
	    // Need to read a continuation line.  Put strings in allocated
	    // memory, because virp->vir_line is overwritten.
	    만약 (!allocated)
	    {
		위해 (i = 0; i < values->ga_len; ++i)
		{
		    bval_T  *vp = (bval_T *)(values->ga_data) + i;

		    만약 (vp->bv_type == BVAL_STRING && !vp->bv_allocated)
		    {
			vp->bv_string = vim_strnsave(vp->bv_string, vp->bv_len);
			vp->bv_allocated = TRUE;
		    }
		}
		allocated = TRUE;
	    }

	    만약 (vim_isdigit(p[1]))
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
		만약 (buf == NULL)
		    반환 TRUE;
		p = buf;
		위해 (todo = len; todo > 0; todo -= n)
		{
		    eof = viminfo_readline(virp);
		    만약 (eof || virp->vir_line[0] != '|'
						  || virp->vir_line[1] != '<')
		    {
			// File was truncated or garbled. Read another line if
			// this one starts with '|'.
			vim_free(buf);
			반환 eof || virp->vir_line[0] == '|';
		    }
		    // Get length of text, excluding |< and NL chars.
		    n = STRLEN(virp->vir_line);
		    동안 (n > 2 && (virp->vir_line[n - 1] == NL
					     || virp->vir_line[n - 1] == CAR))
			--n;
		    n -= 2;
		    만약 (n > todo)
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
	    아니면
	    {
		// Line ending in ">" continues in the next line:
		//     |{bartype},{lots of values},>
		//     |<{value},{value}
		eof = viminfo_readline(virp);
		만약 (eof || virp->vir_line[0] != '|'
					      || virp->vir_line[1] != '<')
		    // File was truncated or garbled. Read another line if
		    // this one starts with '|'.
		    반환 eof || virp->vir_line[0] == '|';
		p = virp->vir_line + 2;
	    }
	}

	만약 (SAFE_isdigit(*p))
	{
	    value->bv_type = BVAL_NR;
	    value->bv_nr = getdigits(&p);
	    ++values->ga_len;
	}
	아니면 만약 (*p == '"')
	{
	    정수	    len = 0;
	    char_u  *s = p;

	    // Unescape special characters in-place.
	    ++p;
	    동안 (*p != '"')
	    {
		만약 (*p == NL || *p == NUL)
		    반환 TRUE;  // syntax error, drop the value
		만약 (*p == '\\')
		{
		    ++p;
		    만약 (*p == 'n')
			s[len++] = '\n';
		    아니면
			s[len++] = *p;
		    ++p;
		}
		아니면
		    s[len++] = *p++;
	    }
	    ++p;
	    s[len] = NUL;

	    converted = FALSE;
	    value->bv_tofree = NULL;
	    만약 (virp->vir_conv.vc_type != CONV_NONE && *s != NUL)
	    {
		sconv = string_convert(&virp->vir_conv, s, NULL);
		만약 (sconv != NULL)
		{
		    만약 (s == buf)
			// the converted string is stored in bv_string and
			// freed later, also need to free "buf" later
			value->bv_tofree = buf;
		    s = sconv;
		    len = (정수)STRLEN(s);
		    converted = TRUE;
		}
	    }

	    // Need to copy in allocated memory if the string wasn't allocated
	    // above and we did allocate before, thus vir_line may change.
	    만약 (s != buf && allocated && !converted)
		s = vim_strnsave(s, len);
	    value->bv_string = s;
	    value->bv_type = BVAL_STRING;
	    value->bv_len = len;
	    value->bv_allocated = allocated || converted;
	    ++values->ga_len;
	    만약 (nextp != NULL)
	    {
		// values following a long string
		p = nextp;
		nextp = NULL;
	    }
	}
	아니면 만약 (*p == ',')
	{
	    value->bv_type = BVAL_EMPTY;
	    ++values->ga_len;
	}
	아니면
	    break;
    }
    반환 TRUE;
}

    static void
write_viminfo_version(FILE *fp_out)
{
    fprintf(fp_out, "# Viminfo version\n|%d,%d\n\n",
					    BARTYPE_VERSION, VIMINFO_VERSION);
}

    static 정수
no_viminfo(void)
{
    // "vim -i NONE" does not read or write a viminfo file
    반환 STRCMP(p_viminfofile, "NONE") == 0;
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    static 정수
viminfo_error(char *errnum, char *message, char_u *line)
{
    vim_snprintf((char *)IObuff, IOSIZE, _("%sviminfo: %s in line: "),
							     errnum, message);
    STRNCAT(IObuff, line, IOSIZE - STRLEN(IObuff) - 1);
    만약 (IObuff[STRLEN(IObuff) - 1] == '\n')
	IObuff[STRLEN(IObuff) - 1] = NUL;
    emsg((char *)IObuff);
    만약 (++viminfo_errcnt >= 10)
    {
	emsg(_(e_viminfo_too_many_errors_skipping_rest_of_file));
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Compare the 'encoding' value in the viminfo file with the current value of
 * 'encoding'.  If different and the 'c' flag is in 'viminfo', setup for
 * conversion of text with iconv() in viminfo_readstring().
 */
    static 정수
viminfo_encoding(vir_T *virp)
{
    char_u	*p;
    정수		i;

    만약 (get_viminfo_parameter('c') != 0)
    {
	p = vim_strchr(virp->vir_line, '=');
	만약 (p != NULL)
	{
	    // remove trailing newline
	    ++p;
	    위해 (i = 0; vim_isprintc(p[i]); ++i)
		;
	    p[i] = NUL;

	    convert_setup(&virp->vir_conv, p, p_enc);
	}
    }
    반환 viminfo_readline(virp);
}

#만약 defined(FEAT_EVAL)
/*
 * Restore global vars that start with a capital from the viminfo file
 */
    static 정수
read_viminfo_varlist(vir_T *virp, 정수 writing)
{
    char_u	*tab;
    정수		type = VAR_NUMBER;
    typval_T	tv;
    funccal_entry_T funccal_entry;

    만약 (!writing && (find_viminfo_parameter('!') != NULL))
    {
	tab = vim_strchr(virp->vir_line + 1, '\t');
	만약 (tab != NULL)
	{
	    *tab++ = '\0';	// isolate the variable name
	    switch (*tab)
	    {
		case 'S': type = VAR_STRING; break;
		case 'F': type = VAR_FLOAT; break;
		case 'D': type = VAR_DICT; break;
		case 'L': type = VAR_LIST; break;
		case 'B': type = VAR_BLOB; break;
		case 'X': type = VAR_SPECIAL; break;
		case 'T': type = VAR_TUPLE; break;
	    }

	    tab = vim_strchr(tab, '\t');
	    만약 (tab != NULL)
	    {
		tv.v_type = type;
		만약 (type == VAR_STRING || type == VAR_DICT
			|| type == VAR_LIST || type == VAR_BLOB
			|| type == VAR_TUPLE)
		    tv.vval.v_string = viminfo_readstring(virp,
				       (정수)(tab - virp->vir_line + 1), TRUE);
		아니면 만약 (type == VAR_FLOAT)
		    (void)string2float(tab + 1, &tv.vval.v_float, FALSE);
		아니면
		{
		    tv.vval.v_number = atol((char *)tab + 1);
		    만약 (type == VAR_SPECIAL && (tv.vval.v_number == VVAL_FALSE
					     || tv.vval.v_number == VVAL_TRUE))
			tv.v_type = VAR_BOOL;
		}
		만약 (type == VAR_DICT || type == VAR_LIST || type == VAR_TUPLE)
		{
		    typval_T *etv = eval_expr(tv.vval.v_string, NULL);

		    만약 (etv == NULL)
			// Failed to parse back the dict or list, use it as a
			// string.
			tv.v_type = VAR_STRING;
		    아니면
		    {
			vim_free(tv.vval.v_string);
			tv = *etv;
			vim_free(etv);
		    }
		}
		아니면 만약 (type == VAR_BLOB)
		{
		    blob_T *blob = string2blob(tv.vval.v_string);

		    만약 (blob == NULL)
			// Failed to parse back the blob, use it as a string.
			tv.v_type = VAR_STRING;
		    아니면
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

		만약 (tv.v_type == VAR_STRING)
		    vim_free(tv.vval.v_string);
		아니면 만약 (tv.v_type == VAR_DICT || tv.v_type == VAR_LIST ||
			tv.v_type == VAR_BLOB)
		    clear_tv(&tv);
	    }
	}
    }

    반환 viminfo_readline(virp);
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
    정수		todo;
    char	*s = "";
    char_u	*p;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];

    만약 (find_viminfo_parameter('!') == NULL)
	반환;

    fputs(_("\n# global variables:\n"), fp);

    todo = (정수)gvht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(gvht, hi, todo)
    {
	만약 (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    this_var = HI2DI(hi);
	    만약 (var_flavour(this_var->di_key) == VAR_FLAVOUR_VIMINFO)
	    {
		switch (this_var->di_tv.v_type)
		{
		    case VAR_STRING:  s = "STR"; break;
		    case VAR_NUMBER:  s = "NUM"; break;
		    case VAR_FLOAT:   s = "FLO"; break;
		    case VAR_DICT:
			  {
			      dict_T	*di = this_var->di_tv.vval.v_dict;
			      정수	copyID = get_copyID();

			      s = "DIC";
			      만약 (di != NULL && !set_ref_in_ht(
					 &di->dv_hashtab, copyID, NULL, NULL)
				      && di->dv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  continue;
			      break;
			  }
		    case VAR_LIST:
			  {
			      list_T	*l = this_var->di_tv.vval.v_list;
			      정수	copyID = get_copyID();

			      s = "LIS";
			      만약 (l != NULL && !set_ref_in_list_items(
						       l, copyID, NULL, NULL)
				      && l->lv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  continue;
			      break;
			  }
		    case VAR_TUPLE:
			  {
			      tuple_T	*tuple = this_var->di_tv.vval.v_tuple;
			      정수	copyID = get_copyID();

			      s = "TUP";
			      만약 (tuple != NULL && !set_ref_in_tuple_items(
					       tuple, copyID, NULL, NULL)
				      && tuple->tv_copyID == copyID)
				  // has a circular reference, can't turn the
				  // value into a string
				  continue;
			      break;
			  }
		    case VAR_BLOB:    s = "BLO"; break;
		    case VAR_BOOL:    s = "XPL"; break;  // backwards compat.
		    case VAR_SPECIAL: s = "XPL"; break;

		    case VAR_UNKNOWN:
		    case VAR_ANY:
		    case VAR_VOID:
		    case VAR_FUNC:
		    case VAR_PARTIAL:
		    case VAR_JOB:
		    case VAR_CHANNEL:
		    case VAR_INSTR:
		    case VAR_CLASS:
		    case VAR_OBJECT:
		    case VAR_TYPEALIAS:
				      continue;
		}
		fprintf(fp, "!%s\t%s\t", this_var->di_key, s);
		만약 (this_var->di_tv.v_type == VAR_BOOL
				      || this_var->di_tv.v_type == VAR_SPECIAL)
		{
		    // do not use "v:true" but "1"
		    sprintf((char *)numbuf, "%ld",
					  (long)this_var->di_tv.vval.v_number);
		    p = numbuf;
		    tofree = NULL;
		}
		아니면
		    p = echo_string(&this_var->di_tv, &tofree, numbuf, 0);
		만약 (p != NULL)
		    viminfo_writestring(fp, p);
		vim_free(tofree);
	    }
	}
    }
}
#endif // FEAT_EVAL

    static 정수
read_viminfo_sub_string(vir_T *virp, 정수 force)
{
    만약 (force || get_old_sub() == NULL)
	set_old_sub(viminfo_readstring(virp, 1, TRUE));
    반환 viminfo_readline(virp);
}

    static void
write_viminfo_sub_string(FILE *fp)
{
    char_u *old_sub = get_old_sub();

    만약 (get_viminfo_parameter('/') == 0 || old_sub == NULL)
	반환;

    fputs(_("\n# Last Substitute String:\n$"), fp);
    viminfo_writestring(fp, old_sub);
}

/*
 * Functions relating to reading/writing the search pattern from viminfo
 */

    static 정수
read_viminfo_search_pattern(vir_T *virp, 정수 force, 정수 writing)
{
    char_u	*lp;
    정수		idx = -1;
    정수		magic = FALSE;
    정수		no_scs = FALSE;
    정수		off_line = FALSE;
    정수		off_end = 0;
    long	off = 0;
    정수		setlast = FALSE;
#ifdef FEAT_SEARCH_EXTRA
    static 정수	hlsearch_on = FALSE;
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
    만약 (lp[0] == '~' && (lp[1] == 'm' || lp[1] == 'M'))	// new line type
    {
	만약 (lp[1] == 'M')		// magic on
	    magic = TRUE;
	만약 (lp[2] == 's')
	    no_scs = TRUE;
	만약 (lp[3] == 'L')
	    off_line = TRUE;
	만약 (lp[4] == 'E')
	    off_end = SEARCH_END;
	lp += 5;
	off = getdigits(&lp);
    }
    만약 (lp[0] == '~')		// use this pattern for last-used pattern
    {
	setlast = TRUE;
	lp++;
    }
    만약 (lp[0] == '/')
	idx = RE_SEARCH;
    아니면 만약 (lp[0] == '&')
	idx = RE_SUBST;
#ifdef FEAT_SEARCH_EXTRA
    아니면 만약 (lp[0] == 'h')	// ~h: 'hlsearch' highlighting off
	hlsearch_on = FALSE;
    아니면 만약 (lp[0] == 'H')	// ~H: 'hlsearch' highlighting on
	hlsearch_on = TRUE;
#endif
    만약 (idx >= 0)
    {
	spat = get_spat(idx);
	만약 (force || spat->pat == NULL)
	{
	    val = viminfo_readstring(virp, (정수)(lp - virp->vir_line + 1),
									TRUE);
	    만약 (val != NULL)
	    {
		만약 (!writing)
		{
		    set_last_search_pat(val, idx, magic, setlast);
		    spat->no_scs = no_scs;
		    spat->off.line = off_line;
		    spat->off.end = off_end;
		    spat->off.off = off;
#ifdef FEAT_SEARCH_EXTRA
		    만약 (setlast)
			set_no_hlsearch(!hlsearch_on);
#endif
		}
		vim_free(val);
	    }
	}
    }
    반환 viminfo_readline(virp);
}

    static void
wvsp_one(
    FILE	*fp,	// file to write to
    정수		idx,	// spats[] index
    char	*s,	// search pat
    정수		sc)	// dir char
{
    spat_T	*spat = get_spat(idx);
    만약 (spat->pat == NULL)
	반환;

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
    만약 (get_viminfo_parameter('/') == 0)
	반환;

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
    정수		i;
    정수		j;

    만약 (y_read_regs == NULL)
	반환;

    위해 (i = 0; i < NUM_REGISTERS; ++i)
	만약 (y_read_regs[i].y_array != NULL)
	{
	    위해 (j = 0; j < y_read_regs[i].y_size; j++)
		vim_free(y_read_regs[i].y_array[j].string);
	    vim_free(y_read_regs[i].y_array);
	}
    VIM_CLEAR(y_read_regs);
}

    static 정수
read_viminfo_register(vir_T *virp, 정수 force)
{
    정수		eof;
    정수		do_it = TRUE;
    정수		size;
    정수		limit;
    정수		i;
    정수		set_prev = FALSE;
    char_u	*str;
    string_T	*array = NULL;
    정수		new_type = MCHAR; // init to shut up compiler
    colnr_T	new_width = 0; // init to shut up compiler
    yankreg_T	*y_current_p;

    // We only get here (hopefully) if line[0] == '"'
    str = virp->vir_line + 1;

    // If the line starts with "" this is the y_previous register.
    만약 (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }

    만약 (!ASCII_ISALNUM(*str) && *str != '-')
    {
	만약 (viminfo_error("E577: ", _(e_illegal_register_name), virp->vir_line))
	    반환 TRUE;	// too many errors, pretend end-of-file
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    y_current_p = get_y_current();
    만약 (!force && y_current_p->y_array != NULL)
	do_it = FALSE;

    만약 (*str == '@')
    {
	// "x@: register x used for @@
	만약 (force || get_execreg_lastc() == NUL)
	    set_execreg_lastc(str[-1]);
    }

    size = 0;
    limit = 100;	// Optimized for registers containing <= 100 lines
    만약 (do_it)
    {
	// Build the new register in array[].
	// y_array is kept as-is until done.
	// The "do_it" flag is reset when something is wrong, in which case
	// array[] needs to be freed.
	만약 (set_prev)
	    set_y_previous(y_current_p);
	array = ALLOC_MULT(string_T, limit);
	str = skipwhite(skiptowhite(str));
	만약 (STRNCMP(str, "CHAR", 4) == 0)
	    new_type = MCHAR;
	아니면 만약 (STRNCMP(str, "BLOCK", 5) == 0)
	    new_type = MBLOCK;
	아니면
	    new_type = MLINE;
	// get the block width; if it's missing we get a zero, which is OK
	str = skipwhite(skiptowhite(str));
	new_width = getdigits(&str);
    }

    동안 (!(eof = viminfo_readline(virp))
		    && (virp->vir_line[0] == TAB || virp->vir_line[0] == '<'))
    {
	만약 (do_it)
	{
	    만약 (size == limit)
	    {
		string_T *new_array = (string_T *)
				    alloc((size_t)limit * 2 * sizeof(string_T));

		만약 (new_array == NULL)
		{
		    do_it = FALSE;
		    break;
		}
		위해 (i = 0; i < limit; i++)
		    new_array[i] = array[i];
		vim_free(array);
		array = new_array;
		limit *= 2;
	    }
	    str = viminfo_readstring(virp, 1, TRUE);
	    만약 (str != NULL)
	    {
		array[size].string = str;
		array[size].length = STRLEN(str);
		++size;
	    }
	    아니면
		// error, don't store the result
		do_it = FALSE;
	}
    }

    만약 (do_it)
    {
	// free y_array[]
	위해 (i = 0; i < y_current_p->y_size; i++)
	    vim_free(y_current_p->y_array[i].string);
	vim_free(y_current_p->y_array);

	y_current_p->y_type = new_type;
	y_current_p->y_width = new_width;
	y_current_p->y_size = size;
	y_current_p->y_time_set = 0;
	만약 (size == 0)
	{
	    y_current_p->y_array = NULL;
	}
	아니면
	{
	    // Move the lines from array[] to y_array[].
	    y_current_p->y_array = ALLOC_MULT(string_T, size);
	    위해 (i = 0; i < size; i++)
	    {
		만약 (y_current_p->y_array == NULL)
		{
		    VIM_CLEAR_STRING(array[i]);
		}
		아니면
		{
		    y_current_p->y_array[i].string = array[i].string;
		    y_current_p->y_array[i].length = array[i].length;
		}
	    }
	}
    }
    아니면
    {
	// Free array[] if it was filled.
	위해 (i = 0; i < size; i++)
	    vim_free(array[i].string);
    }
    vim_free(array);

    반환 eof;
}

/*
 * Accept a new style register line from the viminfo, store it when it's new.
 */
    static void
handle_viminfo_register(garray_T *values, 정수 force)
{
    bval_T	*vp = (bval_T *)values->ga_data;
    정수		flags;
    정수		name;
    정수		type;
    정수		linecount;
    정수		width;
    time_t	timestamp;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();
    정수		i;

    // Check the format:
    // |{bartype},{flags},{name},{type},
    //      {linecount},{width},{timestamp},"line1","line2"
    만약 (values->ga_len < 6
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_NR
	    || vp[5].bv_type != BVAL_NR)
	반환;
    flags = vp[0].bv_nr;
    name = vp[1].bv_nr;
    만약 (name < 0 || name >= NUM_REGISTERS)
	반환;
    type = vp[2].bv_nr;
    만약 (type != MCHAR && type != MLINE && type != MBLOCK)
	반환;
    linecount = vp[3].bv_nr;
    만약 (values->ga_len < 6 + linecount)
	반환;
    width = vp[4].bv_nr;
    만약 (width < 0)
	반환;

    만약 (y_read_regs != NULL)
	// Reading viminfo for merging and writing.  Store the register
	// content, don't update the current registers.
	y_ptr = &y_read_regs[name];
    아니면
	y_ptr = &y_regs_p[name];

    // Do not overwrite unless forced or the timestamp is newer.
    timestamp = (time_t)vp[5].bv_nr;
    만약 (y_ptr->y_array != NULL && !force
			 && (timestamp == 0 || y_ptr->y_time_set > timestamp))
	반환;

    만약 (y_ptr->y_array != NULL)
	위해 (i = 0; i < y_ptr->y_size; i++)
	    vim_free(y_ptr->y_array[i].string);
    vim_free(y_ptr->y_array);

    만약 (y_read_regs == NULL)
    {
	만약 (flags & REG_PREVIOUS)
	    set_y_previous(y_ptr);
	만약 ((flags & REG_EXEC) && (force || get_execreg_lastc() == NUL))
	    set_execreg_lastc(get_register_name(name));
    }
    y_ptr->y_type = type;
    y_ptr->y_width = width;
    y_ptr->y_size = linecount;
    y_ptr->y_time_set = timestamp;
    만약 (linecount == 0)
    {
	y_ptr->y_array = NULL;
	반환;
    }
    y_ptr->y_array = ALLOC_MULT(string_T, linecount);
    만약 (y_ptr->y_array == NULL)
    {
	y_ptr->y_size = 0; // ensure object state is consistent
	반환;
    }
    위해 (i = 0; i < linecount; i++)
    {
	만약 (vp[i + 6].bv_allocated)
	{
	    y_ptr->y_array[i].string = vp[i + 6].bv_string;
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	    vp[i + 6].bv_string = NULL;
	}
	아니면 만약 (vp[i + 6].bv_type != BVAL_STRING)
	{
	    free(y_ptr->y_array);
	    y_ptr->y_array = NULL;
	}
	아니면
	{
	    y_ptr->y_array[i].string = vim_strnsave(vp[i + 6].bv_string, vp[i + 6].bv_len);
	    y_ptr->y_array[i].length = vp[i + 6].bv_len;
	}
    }
}

    static void
write_viminfo_registers(FILE *fp)
{
    정수		i, j;
    char_u	*type;
    char_u	c;
    정수		num_lines;
    정수		max_num_lines;
    정수		max_kbyte;
    long	len;
    yankreg_T	*y_ptr;
    yankreg_T	*y_regs_p = get_y_regs();

    fputs(_("\n# Registers:\n"), fp);

    // Get '<' value, use old '"' value if '<' is not found.
    max_num_lines = get_viminfo_parameter('<');
    만약 (max_num_lines < 0)
	max_num_lines = get_viminfo_parameter('"');
    만약 (max_num_lines == 0)
	반환;
    max_kbyte = get_viminfo_parameter('s');
    만약 (max_kbyte == 0)
	반환;

    위해 (i = 0; i < NUM_REGISTERS; i++)
    {
#만약 defined(FEAT_CLIPBOARD) || defined(FEAT_CLIPBOARD_PROVIDER)
	// Skip '*'/'+' register, we don't want them back next time
	만약 (i == STAR_REGISTER || i == PLUS_REGISTER || i == REAL_PLUS_REGISTER)
	    continue;
#endif
#ifdef FEAT_DND
	// Neither do we want the '~' register
	만약 (i == TILDE_REGISTER)
	    continue;
#endif
	// When reading viminfo for merging and writing: Use the register from
	// viminfo if it's newer.
	만약 (y_read_regs != NULL
		&& y_read_regs[i].y_array != NULL
		&& (y_regs_p[i].y_array == NULL ||
			    y_read_regs[i].y_time_set > y_regs_p[i].y_time_set))
	    y_ptr = &y_read_regs[i];
	아니면 만약 (y_regs_p[i].y_array == NULL)
	    continue;
	아니면
	    y_ptr = &y_regs_p[i];

	// Skip empty registers.
	num_lines = y_ptr->y_size;
	만약 (num_lines == 0
		|| (num_lines == 1 && y_ptr->y_type == MCHAR
					&& *y_ptr->y_array[0].string == NUL))
	    continue;

	만약 (max_kbyte > 0)
	{
	    // Skip register if there is more text than the maximum size.
	    len = 0;
	    위해 (j = 0; j < num_lines; j++)
		len += (long)y_ptr->y_array[j].length + 1L;
	    만약 (len > (long)max_kbyte * 1024L)
		continue;
	}

	switch (y_ptr->y_type)
	{
	    case MLINE:
		type = (char_u *)"LINE";
		break;
	    case MCHAR:
		type = (char_u *)"CHAR";
		break;
	    case MBLOCK:
		type = (char_u *)"BLOCK";
		break;
	    default:
		semsg(_(e_unknown_register_type_nr), y_ptr->y_type);
		type = (char_u *)"LINE";
		break;
	}
	만약 (get_y_previous() == &y_regs_p[i])
	    fprintf(fp, "\"");
	c = get_register_name(i);
	fprintf(fp, "\"%c", c);
	만약 (c == get_execreg_lastc())
	    fprintf(fp, "@");
	fprintf(fp, "\t%s\t%d\n", type, (정수)y_ptr->y_width);

	// If max_num_lines < 0, then we save ALL the lines in the register
	만약 (max_num_lines > 0 && num_lines > max_num_lines)
	    num_lines = max_num_lines;
	위해 (j = 0; j < num_lines; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_ptr->y_array[j].string);
	}

	{
	    정수	    flags = 0;
	    정수	    remaining;

	    // New style with a bar line. Format:
	    // |{bartype},{flags},{name},{type},
	    //      {linecount},{width},{timestamp},"line1","line2"
	    // flags: REG_PREVIOUS - register is y_previous
	    //	      REG_EXEC - used for @@
	    만약 (get_y_previous() == &y_regs_p[i])
		flags |= REG_PREVIOUS;
	    만약 (c == get_execreg_lastc())
		flags |= REG_EXEC;
	    fprintf(fp, "|%d,%d,%d,%d,%d,%d,%ld", BARTYPE_REGISTER, flags,
		    i, y_ptr->y_type, num_lines, (정수)y_ptr->y_width,
		    (long)y_ptr->y_time_set);
	    // 11 chars for type/flags/name/type, 3 * 20 for numbers
	    remaining = LSIZE - 71;
	    위해 (j = 0; j < num_lines; j++)
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
static 정수 vi_jumplist_len = 0;

    static void
write_one_mark(FILE *fp_out, 정수 c, pos_T *pos)
{
    만약 (pos->lnum != 0)
	fprintf(fp_out, "\t%c\t%ld\t%d\n", c, (long)pos->lnum, (정수)pos->col);
}

    static void
write_buffer_marks(buf_T *buf, FILE *fp_out)
{
    정수		i;
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
    위해 (i = 0; i < buf->b_changelistlen; ++i)
    {
	// skip duplicates
	만약 (i == 0 || !EQUAL_POS(buf->b_changelist[i - 1],
							 buf->b_changelist[i]))
	    write_one_mark(fp_out, '+', &buf->b_changelist[i]);
    }
    위해 (i = 0; i < NMARKS; i++)
	write_one_mark(fp_out, 'a' + i, &buf->b_namedm[i]);
}

/*
 * Return TRUE if marks for "buf" should not be written.
 */
    static 정수
skip_for_viminfo(buf_T *buf)
{
    반환 bt_terminal(buf) || removable(buf->b_ffname);
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
    정수		is_mark_set;
    정수		i;
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
	만약 (buf->b_marks_read)
	{
	    만약 (buf->b_last_cursor.lnum != 0)
		is_mark_set = TRUE;
	    아니면
	    {
		is_mark_set = FALSE;
		위해 (i = 0; i < NMARKS; i++)
		    만약 (buf->b_namedm[i].lnum != 0)
		    {
			is_mark_set = TRUE;
			break;
		    }
	    }
	    만약 (is_mark_set && buf->b_ffname != NULL
		      && buf->b_ffname[0] != NUL
		      && !skip_for_viminfo(buf))
	    {
		만약 (buflist == NULL)
		    write_buffer_marks(buf, fp_out);
		아니면 만약 (ga_grow(buflist, 1) == OK)
		    ((buf_T **)buflist->ga_data)[buflist->ga_len++] = buf;
	    }
	}
    }
}

    static void
write_one_filemark(
    FILE	*fp,
    xfmark_T	*fm,
    정수		c1,
    정수		c2)
{
    char_u	*name;

    만약 (fm->fmark.mark.lnum == 0)	// not set
	반환;

    만약 (fm->fmark.fnum != 0)		// there is a buffer
	name = buflist_nr2name(fm->fmark.fnum, TRUE, FALSE);
    아니면
	name = fm->fname;		// use name from .viminfo
    만약 (name != NULL && *name != NUL)
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

    만약 (fm->fmark.fnum != 0)
	vim_free(name);
}

    static void
write_viminfo_filemarks(FILE *fp)
{
    정수		i;
    char_u	*name;
    buf_T	*buf;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    정수		vi_idx;
    정수		idx;

    만약 (get_viminfo_parameter('f') == 0)
	반환;

    fputs(_("\n# File marks:\n"), fp);

    // Write the filemarks 'A - 'Z
    위해 (i = 0; i < NMARKS; i++)
    {
	만약 (vi_namedfm != NULL
			&& (vi_namedfm[i].time_set > namedfm_p[i].time_set))
	    fm = &vi_namedfm[i];
	아니면
	    fm = &namedfm_p[i];
	write_one_filemark(fp, fm, '\'', i + 'A');
    }

    // Find a mark that is the same file and position as the cursor.
    // That one, or else the last one is deleted.
    // Move '0 to '1, '1 to '2, etc. until the matching one or '9
    // Set the '0 mark to current cursor position.
    만약 (curbuf->b_ffname != NULL && !skip_for_viminfo(curbuf))
    {
	name = buflist_nr2name(curbuf->b_fnum, TRUE, FALSE);
	위해 (i = NMARKS; i < NMARKS + EXTRA_MARKS - 1; ++i)
	    만약 (namedfm_p[i].fmark.mark.lnum == curwin->w_cursor.lnum
		    && (namedfm_p[i].fname == NULL
			    ? namedfm_p[i].fmark.fnum == curbuf->b_fnum
			    : (name != NULL
				    && STRCMP(name, namedfm_p[i].fname) == 0)))
		break;
	vim_free(name);

	vim_free(namedfm_p[i].fname);
	위해 ( ; i > NMARKS; --i)
	    namedfm_p[i] = namedfm_p[i - 1];
	namedfm_p[NMARKS].fmark.mark = curwin->w_cursor;
	namedfm_p[NMARKS].fmark.fnum = curbuf->b_fnum;
	namedfm_p[NMARKS].fname = NULL;
	namedfm_p[NMARKS].time_set = vim_time();
    }

    // Write the filemarks '0 - '9.  Newest (highest timestamp) first.
    vi_idx = NMARKS;
    idx = NMARKS;
    위해 (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	xfmark_T *vi_fm = vi_namedfm != NULL ? &vi_namedfm[vi_idx] : NULL;

	만약 (vi_fm != NULL
		&& vi_fm->fmark.mark.lnum != 0
		&& (vi_fm->time_set > namedfm_p[idx].time_set
		    || namedfm_p[idx].fmark.mark.lnum == 0))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	아니면
	{
	    fm = &namedfm_p[idx++];
	    만약 (vi_fm != NULL
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
    위해 (i = 0; i < JUMPLISTSIZE; ++i)
    {
	xfmark_T	*vi_fm;

	fm = idx >= 0 ? &curwin->w_jumplist[idx] : NULL;
	vi_fm = (vi_jumplist != NULL && vi_idx < vi_jumplist_len)
					? &vi_jumplist[vi_idx] : NULL;
	만약 (fm == NULL && vi_fm == NULL)
	    break;
	만약 (fm == NULL || (vi_fm != NULL && fm->time_set < vi_fm->time_set))
	{
	    fm = vi_fm;
	    ++vi_idx;
	}
	아니면
	    --idx;
	만약 (fm->fmark.fnum == 0
		|| ((buf = buflist_findnr(fm->fmark.fnum)) != NULL
		    && !skip_for_viminfo(buf)))
	    write_one_filemark(fp, fm, '-', '\'');
    }
}

/*
 * Compare functions for qsort() below, that compares b_last_used.
 */
    정수
buf_compare(const void *s1, const void *s2)
{
    buf_T *buf1 = *(buf_T **)s1;
    buf_T *buf2 = *(buf_T **)s2;

    만약 (buf1->b_last_used == buf2->b_last_used)
	반환 0;
    반환 buf1->b_last_used > buf2->b_last_used ? -1 : 1;
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
    정수		eof,
    정수		flags)
{
    char_u	*line = virp->vir_line;
    buf_T	*buf;
    정수		num_marked_files;
    정수		load_marks;
    정수		copy_marks_out;
    char_u	*str;
    정수		i;
    char_u	*p;
    char_u	*name_buf;
    pos_T	pos;
#ifdef FEAT_EVAL
    list_T	*list = NULL;
#endif
    정수		count = 0;
    정수		buflist_used = 0;
    buf_T	*buflist_buf = NULL;

    만약 ((name_buf = alloc(LSIZE)) == NULL)
	반환;
    *name_buf = NUL;

    만약 (fp_out != NULL && buflist->ga_len > 0)
    {
	// Sort the list of buffers on b_last_used.
	qsort(buflist->ga_data, (size_t)buflist->ga_len,
						sizeof(buf_T *), buf_compare);
	buflist_buf = ((buf_T **)buflist->ga_data)[0];
    }

#ifdef FEAT_EVAL
    만약 (fp_out == NULL && (flags & (VIF_GET_OLDFILES | VIF_FORCEIT)))
    {
	list = list_alloc();
	만약 (list != NULL)
	    set_vim_var_list(VV_OLDFILES, list);
    }
#endif

    num_marked_files = get_viminfo_parameter('\'');
    동안 (!eof && (count < num_marked_files || fp_out == NULL))
    {
	만약 (line[0] != '>')
	{
	    만약 (line[0] != '\n' && line[0] != '\r' && line[0] != '#')
	    {
		만약 (viminfo_error("E576: ", _(e_nonr_missing_gt), line))
		    break;	// too many errors, return now
	    }
	    eof = vim_fgets(line, LSIZE, virp->vir_fd);
	    continue;		// Skip this dud line
	}

	// Handle long line and translate escaped characters.
	// Find file name, set str to start.
	// Ignore leading and trailing white space.
	str = skipwhite(line + 1);
	str = viminfo_readstring(virp, (정수)(str - virp->vir_line), FALSE);
	만약 (str == NULL)
	    continue;
	p = str + STRLEN(str);
	동안 (p != str && (*p == NUL || vim_isspace(*p)))
	    p--;
	만약 (*p)
	    p++;
	*p = NUL;

#ifdef FEAT_EVAL
	만약 (list != NULL)
	    list_append_string(list, str, -1);
#endif

	// If fp_out == NULL, load marks for current buffer.
	// If fp_out != NULL, copy marks for buffers not in buflist.
	load_marks = copy_marks_out = FALSE;
	만약 (fp_out == NULL)
	{
	    만약 ((flags & VIF_WANT_MARKS) && curbuf->b_ffname != NULL)
	    {
		만약 (*name_buf == NUL)	    // only need to do this once
		    home_replace(NULL, curbuf->b_ffname, name_buf, LSIZE, TRUE);
		만약 (fnamecmp(str, name_buf) == 0)
		    load_marks = TRUE;
	    }
	}
	아니면 // fp_out != NULL
	{
	    // This is slow if there are many buffers!!
	    FOR_ALL_BUFFERS(buf)
		만약 (buf->b_ffname != NULL)
		{
		    home_replace(NULL, buf->b_ffname, name_buf, LSIZE, TRUE);
		    만약 (fnamecmp(str, name_buf) == 0)
			break;
		}

	    // Copy marks if the buffer has not been loaded.
	    만약 (buf == NULL || !buf->b_marks_read)
	    {
		정수	did_read_line = FALSE;

		만약 (buflist_buf != NULL)
		{
		    // Read the next line.  If it has the "*" mark compare the
		    // time stamps.  Write entries from "buflist" that are
		    // newer.
		    만약 (!viminfo_readline(virp) && line[0] == TAB)
		    {
			did_read_line = TRUE;
			만약 (line[1] == '*')
			{
			    long	ltime;

			    sscanf((char *)line + 2, "%ld ", &ltime);
			    동안 ((time_T)ltime < buflist_buf->b_last_used)
			    {
				write_buffer_marks(buflist_buf, fp_out);
				만약 (++count >= num_marked_files)
				    break;
				만약 (++buflist_used == buflist->ga_len)
				{
				    buflist_buf = NULL;
				    break;
				}
				buflist_buf =
				   ((buf_T **)buflist->ga_data)[buflist_used];
			    }
			}
			아니면
			{
			    // No timestamp, must be written by an older Vim.
			    // Assume all remaining buffers are older than
			    // ours.
			    동안 (count < num_marked_files
					    && buflist_used < buflist->ga_len)
			    {
				buflist_buf = ((buf_T **)buflist->ga_data)
							     [buflist_used++];
				write_buffer_marks(buflist_buf, fp_out);
				++count;
			    }
			    buflist_buf = NULL;
			}

			만약 (count >= num_marked_files)
			{
			    vim_free(str);
			    break;
			}
		    }
		}

		fputs("\n> ", fp_out);
		viminfo_writestring(fp_out, str);
		만약 (did_read_line)
		    fputs((char *)line, fp_out);

		count++;
		copy_marks_out = TRUE;
	    }
	}
	vim_free(str);

	pos.coladd = 0;
	동안 (!(eof = viminfo_readline(virp)) && line[0] == TAB)
	{
	    만약 (load_marks)
	    {
		만약 (line[1] != NUL)
		{
		    unsigned u;

		    sscanf((char *)line + 2, "%ld %u", &pos.lnum, &u);
		    pos.col = u;
		    switch (line[1])
		    {
			case '"': curbuf->b_last_cursor = pos; break;
			case '^': curbuf->b_last_insert = pos; break;
			case '.': curbuf->b_last_change = pos; break;
			case '+':
				  // changelist positions are stored oldest
				  // first
				  만약 (curbuf->b_changelistlen == JUMPLISTSIZE)
				      // list is full, remove oldest entry
				      mch_memmove(curbuf->b_changelist,
					    curbuf->b_changelist + 1,
					    sizeof(pos_T) * (JUMPLISTSIZE - 1));
				  아니면
				      ++curbuf->b_changelistlen;
				  curbuf->b_changelist[
					   curbuf->b_changelistlen - 1] = pos;
				  break;

				  // Using the line number for the last-used
				  // timestamp.
			case '*': curbuf->b_last_used = pos.lnum; break;

			default:  만약 ((i = line[1] - 'a') >= 0 && i < NMARKS)
				      curbuf->b_namedm[i] = pos;
		    }
		}
	    }
	    아니면 만약 (copy_marks_out)
		fputs((char *)line, fp_out);
	}

	만약 (load_marks)
	{
	    win_T	*wp;

	    FOR_ALL_WINDOWS(wp)
	    {
		만약 (wp->w_buffer == curbuf)
		    wp->w_changelistidx = curbuf->b_changelistlen;
	    }
	    만약 (flags & VIF_ONLY_CURBUF)
		break;
	}
    }

    만약 (fp_out != NULL)
	// Write any remaining entries from buflist.
	동안 (count < num_marked_files && buflist_used < buflist->ga_len)
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
    만약 (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != NULL)
	read_viminfo(NULL, VIF_WANT_MARKS | VIF_ONLY_CURBUF);

    // Always set b_marks_read; needed when 'viminfo' is changed to include
    // the ' parameter after opening a buffer.
    curbuf->b_marks_read = true;
}

    static 정수
read_viminfo_filemark(vir_T *virp, 정수 force)
{
    char_u	*str;
    xfmark_T	*namedfm_p = get_namedfm();
    xfmark_T	*fm;
    정수		i;

    // We only get here if line[0] == '\'' or '-'.
    // Illegal mark names are ignored (for future expansion).
    str = virp->vir_line + 1;
    만약 (*str <= 127
	    && ((*virp->vir_line == '\''
				       && (VIM_ISDIGIT(*str) || SAFE_isupper(*str)))
	     || (*virp->vir_line == '-' && *str == '\'')))
    {
	만약 (*str == '\'')
	{
	    // If the jumplist isn't full insert fmark as oldest entry
	    만약 (curwin->w_jumplistlen == JUMPLISTSIZE)
		fm = NULL;
	    아니면
	    {
		위해 (i = curwin->w_jumplistlen; i > 0; --i)
		    curwin->w_jumplist[i] = curwin->w_jumplist[i - 1];
		++curwin->w_jumplistidx;
		++curwin->w_jumplistlen;
		fm = &curwin->w_jumplist[0];
		fm->fmark.mark.lnum = 0;
		fm->fname = NULL;
	    }
	}
	아니면 만약 (VIM_ISDIGIT(*str))
	    fm = &namedfm_p[*str - '0' + NMARKS];
	아니면
	    fm = &namedfm_p[*str - 'A'];
	만약 (fm != NULL && (fm->fmark.mark.lnum == 0 || force))
	{
	    str = skipwhite(str + 1);
	    fm->fmark.mark.lnum = getdigits(&str);
	    str = skipwhite(str);
	    fm->fmark.mark.col = getdigits(&str);
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    str = skipwhite(str);
	    vim_free(fm->fname);
	    fm->fname = viminfo_readstring(virp, (정수)(str - virp->vir_line),
								       FALSE);
	    fm->time_set = 0;
	}
    }
    반환 vim_fgets(virp->vir_line, LSIZE, virp->vir_fd);
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
    정수		i;

    만약 (vi_namedfm != NULL)
    {
	위해 (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	    vim_free(vi_namedfm[i].fname);
	VIM_CLEAR(vi_namedfm);
    }
    만약 (vi_jumplist != NULL)
    {
	위해 (i = 0; i < vi_jumplist_len; ++i)
	    vim_free(vi_jumplist[i].fname);
	VIM_CLEAR(vi_jumplist);
    }
}

/*
 * Accept a new style mark line from the viminfo, store it when it's new.
 */
    static void
handle_viminfo_mark(garray_T *values, 정수 force)
{
    bval_T	*vp = (bval_T *)values->ga_data;
    정수		name;
    linenr_T	lnum;
    colnr_T	col;
    time_t	timestamp;
    xfmark_T	*fm = NULL;

    // Check the format:
    // |{bartype},{name},{lnum},{col},{timestamp},{filename}
    만약 (values->ga_len < 5
	    || vp[0].bv_type != BVAL_NR
	    || vp[1].bv_type != BVAL_NR
	    || vp[2].bv_type != BVAL_NR
	    || vp[3].bv_type != BVAL_NR
	    || vp[4].bv_type != BVAL_STRING)
	반환;

    name = vp[0].bv_nr;
    만약 (name != '\'' && !VIM_ISDIGIT(name) && !ASCII_ISUPPER(name))
	반환;
    lnum = vp[1].bv_nr;
    col = vp[2].bv_nr;
    만약 (lnum <= 0 || col < 0)
	반환;
    timestamp = (time_t)vp[3].bv_nr;

    만약 (name == '\'')
    {
	만약 (vi_jumplist != NULL)
	{
	    만약 (vi_jumplist_len < JUMPLISTSIZE)
		fm = &vi_jumplist[vi_jumplist_len++];
	}
	아니면
	{
	    정수 idx;
	    정수 i;

	    // If we have a timestamp insert it in the right place.
	    만약 (timestamp != 0)
	    {
		위해 (idx = curwin->w_jumplistlen - 1; idx >= 0; --idx)
		    만약 (curwin->w_jumplist[idx].time_set < timestamp)
		    {
			++idx;
			break;
		    }
		// idx cannot be zero now
		만약 (idx < 0 && curwin->w_jumplistlen < JUMPLISTSIZE)
		    // insert as the oldest entry
		    idx = 0;
	    }
	    아니면 만약 (curwin->w_jumplistlen < JUMPLISTSIZE)
		// insert as oldest entry
		idx = 0;
	    아니면
		idx = -1;

	    만약 (idx >= 0)
	    {
		만약 (curwin->w_jumplistlen == JUMPLISTSIZE)
		{
		    // Drop the oldest entry.
		    --idx;
		    vim_free(curwin->w_jumplist[0].fname);
		    위해 (i = 0; i < idx; ++i)
			curwin->w_jumplist[i] = curwin->w_jumplist[i + 1];
		}
		아니면
		{
		    // Move newer entries forward.
		    위해 (i = curwin->w_jumplistlen; i > idx; --i)
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
    아니면
    {
	정수		idx;
	xfmark_T	*namedfm_p = get_namedfm();

	만약 (VIM_ISDIGIT(name))
	{
	    만약 (vi_namedfm != NULL)
		idx = name - '0' + NMARKS;
	    아니면
	    {
		정수 i;

		// Do not use the name from the viminfo file, insert in time
		// order.
		위해 (idx = NMARKS; idx < NMARKS + EXTRA_MARKS; ++idx)
		    만약 (namedfm_p[idx].time_set < timestamp)
			break;
		만약 (idx == NMARKS + EXTRA_MARKS)
		    // All existing entries are newer.
		    반환;
		i = NMARKS + EXTRA_MARKS - 1;

		vim_free(namedfm_p[i].fname);
		위해 ( ; i > idx; --i)
		    namedfm_p[i] = namedfm_p[i - 1];
		namedfm_p[idx].fname = NULL;
	    }
	}
	아니면
	    idx = name - 'A';
	만약 (vi_namedfm != NULL)
	    fm = &vi_namedfm[idx];
	아니면
	    fm = &namedfm_p[idx];
    }

    만약 (fm != NULL)
    {
	만약 (vi_namedfm != NULL || fm->fmark.mark.lnum == 0
					  || fm->time_set < timestamp || force)
	{
	    fm->fmark.mark.lnum = lnum;
	    fm->fmark.mark.col = col;
	    fm->fmark.mark.coladd = 0;
	    fm->fmark.fnum = 0;
	    vim_free(fm->fname);
	    만약 (vp[4].bv_allocated)
	    {
		fm->fname = vp[4].bv_string;
		vp[4].bv_string = NULL;
	    }
	    아니면
		fm->fname = vim_strsave(vp[4].bv_string);
	    fm->time_set = timestamp;
	}
    }
}

    static 정수
read_viminfo_barline(vir_T *virp, 정수 got_encoding, 정수 force, 정수 writing)
{
    char_u	*p = virp->vir_line + 1;
    정수		bartype;
    garray_T	values;
    bval_T	*vp;
    정수		i;
    정수		read_next = TRUE;

    // The format is: |{bartype},{value},...
    // For a very long string:
    //     |{bartype},>{length of "{text}{text2}"}
    //     |<{text1}
    //     |<{text2},{value}
    // For a long line not using a string
    //     |{bartype},{lots of values},>
    //     |<{value},{value}
    만약 (*p == '<')
    {
	// Continuation line of an unrecognized item.
	만약 (writing)
	    ga_copy_string(&virp->vir_barlines, virp->vir_line);
    }
    아니면
    {
	ga_init2(&values, sizeof(bval_T), 20);
	bartype = getdigits(&p);
	switch (bartype)
	{
	    case BARTYPE_VERSION:
		// Only use the version when it comes before the encoding.
		// If it comes later it was copied by a Vim version that
		// doesn't understand the version.
		만약 (!got_encoding)
		{
		    read_next = barline_parse(virp, p, &values);
		    vp = (bval_T *)values.ga_data;
		    만약 (values.ga_len > 0 && vp->bv_type == BVAL_NR)
			virp->vir_version = vp->bv_nr;
		}
		break;

	    case BARTYPE_HISTORY:
		read_next = barline_parse(virp, p, &values);
		handle_viminfo_history(&values, writing);
		break;

	    case BARTYPE_REGISTER:
		read_next = barline_parse(virp, p, &values);
		handle_viminfo_register(&values, force);
		break;

	    case BARTYPE_MARK:
		read_next = barline_parse(virp, p, &values);
		handle_viminfo_mark(&values, force);
		break;

	    default:
		// copy unrecognized line (for future use)
		만약 (writing)
		    ga_copy_string(&virp->vir_barlines, virp->vir_line);
	}
	위해 (i = 0; i < values.ga_len; ++i)
	{
	    vp = (bval_T *)values.ga_data + i;
	    만약 (vp->bv_type == BVAL_STRING && vp->bv_allocated)
		vim_free(vp->bv_string);
	    vim_free(vp->bv_tofree);
	}
	ga_clear(&values);
    }

    만약 (read_next)
	반환 viminfo_readline(virp);
    반환 FALSE;
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    static 정수
read_viminfo_up_to_marks(
    vir_T	*virp,
    정수		forceit,
    정수		writing)
{
    정수		eof;
    buf_T	*buf;
    정수		got_encoding = FALSE;

    prepare_viminfo_history(forceit ? 9999 : 0, writing);

    eof = viminfo_readline(virp);
    동안 (!eof && virp->vir_line[0] != '>')
    {
	switch (virp->vir_line[0])
	{
		// Characters reserved for future expansion, ignored now
	    case '+': // "+40 /path/dir file", for running vim without args
	    case '^': // to be defined
	    case '<': // long line - ignored
		// A comment or empty line.
	    case NUL:
	    case '\r':
	    case '\n':
	    case '#':
		eof = viminfo_readline(virp);
		break;
	    case '|':
		eof = read_viminfo_barline(virp, got_encoding,
							    forceit, writing);
		break;
	    case '*': // "*encoding=value"
		got_encoding = TRUE;
		eof = viminfo_encoding(virp);
		break;
	    case '!': // global variable
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(virp, writing);
#아니면
		eof = viminfo_readline(virp);
#endif
		break;
	    case '%': // entry for buffer list
		eof = read_viminfo_bufferlist(virp, writing);
		break;
	    case '"':
		// When registers are in bar lines skip the old style register
		// lines.
		만약 (virp->vir_version < VIMINFO_VERSION_WITH_REGISTERS)
		    eof = read_viminfo_register(virp, forceit);
		아니면
		    do
		    {
			eof = viminfo_readline(virp);
		    } 동안 (!eof && (virp->vir_line[0] == TAB
						|| virp->vir_line[0] == '<'));
		break;
	    case '/':	    // Search string
	    case '&':	    // Substitute search string
	    case '~':	    // Last search string, followed by '/' or '&'
		eof = read_viminfo_search_pattern(virp, forceit, writing);
		break;
	    case '$':
		eof = read_viminfo_sub_string(virp, forceit);
		break;
	    case ':':
	    case '?':
	    case '=':
	    case '@':
		// When history is in bar lines skip the old style history
		// lines.
		만약 (virp->vir_version < VIMINFO_VERSION_WITH_HISTORY)
		    eof = read_viminfo_history(virp, writing);
		아니면
		    eof = viminfo_readline(virp);
		break;
	    case '-':
	    case '\'':
		// When file marks are in bar lines skip the old style lines.
		만약 (virp->vir_version < VIMINFO_VERSION_WITH_MARKS)
		    eof = read_viminfo_filemark(virp, forceit);
		아니면
		    eof = viminfo_readline(virp);
		break;
	    default:
		만약 (viminfo_error("E575: ", _(e_illegal_starting_char),
			    virp->vir_line))
		    eof = TRUE;
		아니면
		    eof = viminfo_readline(virp);
		break;
	}
    }

    // Finish reading history items.
    만약 (!writing)
	finish_viminfo_history(virp);

    // Change file names to buffer numbers for fmarks.
    FOR_ALL_BUFFERS(buf)
	fmarks_check_names(buf);

    반환 eof;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    static void
do_viminfo(FILE *fp_in, FILE *fp_out, 정수 flags)
{
    정수		eof = FALSE;
    vir_T	vir;
    정수		merge = FALSE;
    정수		do_copy_marks = FALSE;
    garray_T	buflist;

    만약 ((vir.vir_line = alloc(LSIZE)) == NULL)
	반환;
    vir.vir_fd = fp_in;
    vir.vir_conv.vc_type = CONV_NONE;
    ga_init2(&vir.vir_barlines, sizeof(char_u *), 100);
    vir.vir_version = -1;

    만약 (fp_in != NULL)
    {
	만약 (flags & VIF_WANT_INFO)
	{
	    만약 (fp_out != NULL)
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
	아니면 만약 (flags != 0)
	    // Skip info, find start of marks
	    동안 (!(eof = viminfo_readline(&vir))
		    && vir.vir_line[0] != '>')
		;

	do_copy_marks = (flags & (VIF_WANT_MARKS | VIF_ONLY_CURBUF
					    | VIF_GET_OLDFILES | VIF_FORCEIT));
    }

    만약 (fp_out != NULL)
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

	만약 (do_copy_marks)
	    ga_init2(&buflist, sizeof(buf_T *), 50);
	write_viminfo_marks(fp_out, do_copy_marks ? &buflist : NULL);
    }

    만약 (do_copy_marks)
    {
	copy_viminfo_marks(&vir, fp_out, &buflist, eof, flags);
	만약 (fp_out != NULL)
	    ga_clear(&buflist);
    }

    vim_free(vir.vir_line);
    만약 (vir.vir_conv.vc_type != CONV_NONE)
	convert_setup(&vir.vir_conv, NULL, NULL);
    ga_clear_strings(&vir.vir_barlines);
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless "flags" includes VIF_FORCEIT. -- webb
 */
    정수
read_viminfo(
    char_u	*file,	    // file name or NULL to use default name
    정수		flags)	    // VIF_WANT_INFO et al.
{
    FILE	*fp;
    char_u	*fname;
    stat_T	st;		// mch_stat() of existing viminfo file

    만약 (no_viminfo())
	반환 FAIL;

    fname = viminfo_filename(file);	// get file name in allocated buffer
    만약 (fname == NULL)
	반환 FAIL;
    fp = mch_fopen((char *)fname, READBIN);

    만약 (p_verbose > 0)
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
    만약 (fp == NULL)
	반환 FAIL;
    만약 (mch_fstat(fileno(fp), &st) < 0 || S_ISDIR(st.st_mode))
    {
	fclose(fp);
	반환 FAIL;
    }

    viminfo_errcnt = 0;
    do_viminfo(fp, NULL, flags);

    fclose(fp);
    반환 OK;
}

/*
 * Write the viminfo file.  The old one is read in first so that effectively a
 * merge of current info and old info is done.  This allows multiple vims to
 * run simultaneously, without losing any marks etc.
 * If "forceit" is TRUE, then the old file is not read in, and only internal
 * info is written to the file.
 */
    void
write_viminfo(char_u *file, 정수 forceit)
{
    char_u	*fname;
    FILE	*fp_in = NULL;	// input viminfo file, if any
    FILE	*fp_out = NULL;	// output viminfo file
    char_u	*tempname = NULL;	// name of temp viminfo file
    stat_T	st_new;		// mch_stat() of potential new file
    stat_T	st_old;		// mch_stat() of existing viminfo file
#만약 defined(UNIX) || defined(VMS)
    mode_t	umask_save;
#endif
#ifdef UNIX
    정수		shortname = FALSE;	// use 8.3 file name
#endif
#ifdef MSWIN
    정수		hidden = FALSE;
#endif

    만약 (no_viminfo())
	반환;

    fname = viminfo_filename(file);	// may set to default if NULL
    만약 (fname == NULL)
	반환;

    fp_in = mch_fopen((char *)fname, READBIN);
    만약 (fp_in == NULL)
    {
	정수 fd;

	// if it does exist, but we can't read it, don't try writing
	만약 (mch_stat((char *)fname, &st_new) == 0)
	    goto end;

	// Create the new .viminfo non-accessible for others, because it may
	// contain text from non-accessible documents. It is up to the user to
	// widen access (e.g. to a group). This may also fail if there is a
	// race condition, then just give up.
	fd = mch_open((char *)fname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
	만약 (fd < 0)
	    goto end;
	fp_out = fdopen(fd, WRITEBIN);
    }
    아니면
    {
	// There is an existing viminfo file.  Create a temporary file to
	// write the new viminfo into, in the same directory as the
	// existing viminfo file, which will be renamed once all writing is
	// successful.
	만약 (mch_fstat(fileno(fp_in), &st_old) < 0
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
	    정수	tt = msg_didany;

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
	위해 (;;)
	{
	    정수		next_char = 'z';
	    char_u	*wp;

	    tempname = buf_modname(
#ifdef UNIX
				    shortname,
#아니면
				    FALSE,
#endif
				    fname,
#ifdef VMS
				    (char_u *)"-tmp",
#아니면
				    (char_u *)".tmp",
#endif
				    FALSE);
	    만약 (tempname == NULL)		// out of memory
		break;

	    // Try a series of names.  Change one character, just before
	    // the extension.  This should also work for an 8.3
	    // file name, when after adding the extension it still is
	    // the same file as the original.
	    wp = tempname + STRLEN(tempname) - 5;
	    만약 (wp < gettail(tempname))	    // empty file name?
		wp = gettail(tempname);
	    위해 (;;)
	    {
		// Check if tempfile already exists.  Never overwrite an
		// existing file!
		만약 (mch_stat((char *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    // Check if tempfile is same as original file.  May happen
		    // when modname() gave the same file back.  E.g.  silly
		    // link, or file name-length reached.  Try again with
		    // shortname set.
		    만약 (!shortname && st_new.st_dev == st_old.st_dev
						&& st_new.st_ino == st_old.st_ino)
		    {
			VIM_CLEAR(tempname);
			shortname = TRUE;
			break;
		    }
#endif
		}
		아니면
		{
		    // Try creating the file exclusively.  This may fail if
		    // another Vim tries to do it at the same time.
#ifdef VMS
		    // fdopen() fails for some reason
		    umask_save = umask(077);
		    fp_out = mch_fopen((char *)tempname, WRITEBIN);
		    (void)umask(umask_save);
#아니면
		    정수	fd;

		    // Use mch_open() to be able to use O_NOFOLLOW and set file
		    // protection:
		    // Unix: same as original file, but strip s-bit.  Reset
		    // umask to avoid it getting in the way.
		    // Others: r&w for user only.
# ifdef UNIX
		    umask_save = umask(0);
		    fd = mch_open((char *)tempname,
			    O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW,
					(정수)((st_old.st_mode & 0777) | 0600));
		    (void)umask(umask_save);
# 아니면
		    fd = mch_open((char *)tempname,
			     O_CREAT|O_EXTRA|O_EXCL|O_WRONLY|O_NOFOLLOW, 0600);
# endif
		    만약 (fd < 0)
		    {
			fp_out = NULL;
# ifdef EEXIST
			// Avoid trying lots of names while the problem is lack
			// of permission, only retry if the file already
			// exists.
			만약 (errno != EEXIST)
			    break;
# endif
		    }
		    아니면
			fp_out = fdopen(fd, WRITEBIN);
#endif // VMS
		    만약 (fp_out != NULL)
			break;
		}

		// Assume file exists, try again with another name.
		만약 (next_char == 'a' - 1)
		{
		    // They all exist?  Must be something wrong! Don't write
		    // the viminfo file then.
		    semsg(_(e_too_many_viminfo_temp_files_like_str), tempname);
		    break;
		}
		*wp = next_char;
		--next_char;
	    }

	    만약 (tempname != NULL)
		break;
	    // continue if shortname was set
	}

#만약 defined(UNIX) && defined(HAVE_FCHOWN)
	만약 (tempname != NULL && fp_out != NULL)
	{
		stat_T	tmp_st;

	    // Make sure the original owner can read/write the tempfile and
	    // otherwise preserve permissions, making sure the group matches.
	    만약 (mch_stat((char *)tempname, &tmp_st) >= 0)
	    {
		만약 (st_old.st_uid != tmp_st.st_uid)
		    // Changing the owner might fail, in which case the
		    // file will now be owned by the current user, oh well.
		    vim_ignored = fchown(fileno(fp_out), st_old.st_uid, -1);
		만약 (st_old.st_gid != tmp_st.st_gid
			&& fchown(fileno(fp_out), -1, st_old.st_gid) == -1)
		    // can't set the group to what it should be, remove
		    // group permissions
		    (void)mch_setperm(tempname, 0600);
	    }
	    아니면
		// can't stat the file, set conservative permissions
		(void)mch_setperm(tempname, 0600);
	}
#endif
    }

    // Check if the new viminfo file can be written to.
    만약 (fp_out == NULL)
    {
	semsg(_(e_cant_write_viminfo_file_str),
		       (fp_in == NULL || tempname == NULL) ? fname : tempname);
	만약 (fp_in != NULL)
	    fclose(fp_in);
	goto end;
    }

    만약 (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing viminfo file \"%s\""), fname);
	verbose_leave();
    }

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, forceit ? 0 : (VIF_WANT_INFO | VIF_WANT_MARKS));

    만약 (fclose(fp_out) == EOF)
	++viminfo_errcnt;

    만약 (fp_in != NULL)
    {
	fclose(fp_in);

	// In case of an error keep the original viminfo file.  Otherwise
	// rename the newly written file.  Give an error if that fails.
	만약 (viminfo_errcnt == 0)
	{
	    만약 (vim_rename(tempname, fname) == -1)
	    {
		++viminfo_errcnt;
		semsg(_(e_cant_rename_viminfo_file_to_str), fname);
	    }
#ifdef MSWIN
	    // If the viminfo file was hidden then also hide the new file.
	    아니면 만약 (hidden)
		mch_hide(fname);
#endif
	}
	만약 (viminfo_errcnt > 0)
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
    만약 (*p_viminfo == NUL)
	p_viminfo = (char_u *)"'100";
    만약 (eap->cmdidx == CMD_rviminfo)
    {
	만약 (read_viminfo(eap->arg, VIF_WANT_INFO | VIF_WANT_MARKS
				  | (eap->forceit ? VIF_FORCEIT : 0)) == FAIL)
	    emsg(_(e_cannot_open_viminfo_file_for_reading));
    }
    아니면
	write_viminfo(eap->arg, eap->forceit);
    p_viminfo = save_viminfo;
}

#endif // FEAT_VIMINFO
