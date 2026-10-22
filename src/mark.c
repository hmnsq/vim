/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * mark.c: functions for setting marks and jumping to them
 */

#include "vim.h"

/*
 * This file contains routines to maintain and manipulate marks.
 */

/*
 * If a named file mark's lnum is non-zero, it is valid.
 * If a named file mark's fnum is non-zero, it is for an existing buffer,
 * otherwise it is from .viminfo and namedfm[n].fname is the file name.
 * There are marks 'A - 'Z (set by user) and '0 to '9 (set when writing
 * viminfo).
 */
static xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

static void fname2fnum(xfmark_T *fm);
static void fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
static char_u *mark_line(pos_T *mp, 정수 lead_len);
static void show_one_mark(정수, char_u *, pos_T *, char_u *, 정수 current);
static void mark_adjust_internal(linenr_T line1, linenr_T line2, long amount,
    long amount_after, 정수 adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    정수
setmark(정수 c)
{
    반환 setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    정수
setmark_pos(정수 c, pos_T *pos, 정수 fnum)
{
    정수		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    만약 (c < 0)
	반환 FAIL;

    만약 (c == '\'' || c == '`')
    {
	만약 (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	아니면
	    curwin->w_pcmark = *pos;
	반환 OK;
    }

    buf = buflist_findnr(fnum);
    만약 (buf == NULL)
	반환 FAIL;

    만약 (c == '"')
    {
	buf->b_last_cursor = *pos;
	반환 OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    만약 (c == '[')
    {
	buf->b_op_start = *pos;
	반환 OK;
    }
    만약 (c == ']')
    {
	buf->b_op_end = *pos;
	반환 OK;
    }

    만약 (c == '<' || c == '>')
    {
	만약 (c == '<')
	    buf->b_visual.vi_start = *pos;
	아니면
	    buf->b_visual.vi_end = *pos;
	만약 (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	반환 OK;
    }

    만약 (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	반환 OK;
    }
    만약 (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	만약 (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	아니면
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	반환 OK;
    }
    반환 FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    void
mark_forget_file(win_T *wp, 정수 fnum)
{
    정수		i;

    위해 (i = wp->w_jumplistlen - 1; i >= 0; --i)
	만약 (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    만약 (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * sizeof(wp->w_jumplist[i]));
	}

    위해 (i = wp->w_tagstacklen - 1; i >= 0; --i)
	만약 (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    만약 (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * sizeof(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    void
setpcmark(void)
{
    정수		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    만약 (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	반환;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    만약 (jop_flags & JOP_STACK)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	만약 (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    만약 (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	위해 (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = NULL;
#ifdef FEAT_VIMINFO
    fm->time_set = vim_time();
#endif
}

/*
 * To change context, call setpcmark(), then move the current position to
 * where ever, then call checkpcmark().  This ensures that the previous
 * context will only be changed if the cursor moved to a different line.
 * If pcmark was deleted (with "dG") the previous mark is restored.
 */
    void
checkpcmark(void)
{
    만약 (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(정수 count)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    만약 (curwin->w_jumplistlen == 0)	    // nothing to jump to
	반환 (pos_T *)NULL;

    위해 (;;)
    {
	만약 (curwin->w_jumplistidx + count < 0
		|| curwin->w_jumplistidx + count >= curwin->w_jumplistlen)
	    반환 (pos_T *)NULL;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	만약 (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    만약 (curwin->w_jumplistidx + count < 0)
		반환 (pos_T *)NULL;
	}

	curwin->w_jumplistidx += count;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	만약 (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	만약 (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    만약 (buflist_findnr(fmark.fnum) == NULL)
	    {					     // Skip this one ..
		count += count < 0 ? -1 : 1;
		continue;
	    }
	    만약 (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		반환 (pos_T *)NULL;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	아니면
	    pos = &(jmp->fmark.mark);
	반환 pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(정수 count)
{
    정수		n;

    만약 (curbuf->b_changelistlen == 0)	    // nothing to jump to
	반환 (pos_T *)NULL;

    n = curwin->w_changelistidx;
    만약 (n + count < 0)
    {
	만약 (n == 0)
	    반환 (pos_T *)NULL;
	n = 0;
    }
    아니면 만약 (n + count >= curbuf->b_changelistlen)
    {
	만약 (n == curbuf->b_changelistlen - 1)
	    반환 (pos_T *)NULL;
	n = curbuf->b_changelistlen - 1;
    }
    아니면
	n += count;
    curwin->w_changelistidx = n;
    반환 curbuf->b_changelist + n;
}

/*
 * Find mark "c" in buffer pointed to by "buf".
 * If "changefile" is TRUE it's allowed to edit another file for '0, 'A, etc.
 * If "fnum" is not NULL store the fnum there for '0, 'A etc., don't edit
 * another file.
 * Returns:
 * - pointer to pos_T if found.  lnum is 0 when mark not set, -1 when mark is
 *   in another file which can't be gotten. (caller needs to check lnum!)
 * - NULL if there is no mark called 'c'.
 * - -1 if mark is in other file and jumped there (only if changefile is TRUE)
 */
    pos_T *
getmark_buf(buf_T *buf, 정수 c, 정수 changefile)
{
    반환 getmark_buf_fnum(buf, c, changefile, NULL);
}

    pos_T *
getmark(정수 c, 정수 changefile)
{
    반환 getmark_buf_fnum(curbuf, c, changefile, NULL);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    정수		c,
    정수		changefile,
    정수		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    static pos_T	pos_copy;

    posp = NULL;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    만약 (c < 0)
	반환 posp;
    만약 (c > '~')			// check for islower()/isupper()
	;
    아니면 만약 (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    아니면 만약 (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    아니면 만약 (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_insert);
    아니면 만약 (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    아니면 만약 (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    아니면 만약 (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    아니면 만약 (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	정수	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	만약 (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    아니면 만약 (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	정수	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	만약 (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    아니면 만약 (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	만약 (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	아니면
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	만약 (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    만약 (c == '<')
		pos_copy.col = 0;
	    아니면
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    아니면 만약 (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    아니면 만약 (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	만약 (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	아니면
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	만약 (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	만약 (fnum != NULL)
	    *fnum = namedfm[c].fmark.fnum;
	아니면 만약 (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    만약 (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		만약 (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    반환 (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    아니면
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    반환 posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    정수		dir,	// direction for search
    정수		begin_line)
{
    정수		i;
    pos_T	*result = NULL;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    만약 (dir == BACKWARD && begin_line)
	pos.col = 0;
    아니면 만약 (dir == FORWARD && begin_line)
	pos.col = MAXCOL;

    위해 (i = 0; i < NMARKS; i++)
    {
	만약 (curbuf->b_namedm[i].lnum > 0)
	{
	    만약 (dir == FORWARD)
	    {
		만약 ((result == NULL || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    아니면
	    {
		만약 ((result == NULL || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    반환 result;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    static void
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    만약 (fm->fname == NULL)
	반환;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    만약 (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    아니면
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (void)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    void
fmarks_check_names(buf_T *buf)
{
    char_u	*name;
    정수		i;
    win_T	*wp;

    만약 (buf->b_ffname == NULL)
	반환;

    name = home_replace_save(buf, buf->b_ffname);
    만약 (name == NULL)
	반환;

    위해 (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	위해 (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    static void
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    만약 (fm->fmark.fnum == 0
	    && fm->fname != NULL
	    && fnamecmp(name, fm->fname) == 0)
    {
	fm->fmark.fnum = buf->b_fnum;
	VIM_CLEAR(fm->fname);
    }
}

/*
 * Check a if a position from a mark is valid.
 * Give and error message and return FAIL if not.
 */
    정수
check_mark(pos_T *pos)
{
    만약 (pos == NULL)
    {
	emsg(_(e_unknown_mark));
	반환 FAIL;
    }
    만약 (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	만약 (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	반환 FAIL;
    }
    만약 (pos->lnum > curbuf->b_ml.ml_line_count)
    {
	emsg(_(e_mark_has_invalid_line_number));
	반환 FAIL;
    }
    반환 OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    void
clrallmarks(buf_T *buf)
{
    static 정수		i = -1;

    만약 (i == -1)	// first call ever: initialize
	위해 (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = NULL;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    위해 (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_start.lnum = 0;		// start/end op mark cleared
    buf->b_op_end.lnum = 0;
    buf->b_last_cursor.lnum = 1;	// '" mark cleared
    buf->b_last_cursor.col = 0;
    buf->b_last_cursor.coladd = 0;
    buf->b_last_insert.lnum = 0;	// '^ mark cleared
    buf->b_last_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, 정수 lead_len)
{
    만약 (fmark->fnum == curbuf->b_fnum)		    // current buffer
	반환 mark_line(&(fmark->mark), lead_len);
    반환 buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    static char_u *
mark_line(pos_T *mp, 정수 lead_len)
{
    char_u	*s, *p;
    정수		len;

    만약 (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_line_count)
	반환 vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    만약 (s == NULL)
	반환 NULL;
    // Truncate the line to fit it in the window.
    len = 0;
    위해 (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	만약 (len >= Columns - lead_len)
	    break;
    }
    *p = NUL;
    반환 s;
}

/*
 * print the marks
 */
    void
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    정수		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    만약 (arg != NULL && *arg == NUL)
	arg = NULL;

    show_one_mark('\'', arg, &curwin->w_pcmark, NULL, TRUE);
    위해 (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], NULL, TRUE);
    위해 (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	만약 (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	아니면
	    name = namedfm[i].fname;
	만약 (name != NULL)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    만약 (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_last_cursor, NULL, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_start, NULL, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_end, NULL, TRUE);
    show_one_mark('^', arg, &curbuf->b_last_insert, NULL, TRUE);
    show_one_mark('.', arg, &curbuf->b_last_change, NULL, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_start;
    endp = &curbuf->b_visual.vi_end;
    만약 ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    아니면
	posp = endp;
    show_one_mark('<', arg, posp, NULL, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, NULL, TRUE);

    show_one_mark(-1, arg, NULL, NULL, FALSE);
}

    static void
show_one_mark(
    정수		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    정수		current)	// in current file
{
    static 정수	did_title = FALSE;
    정수		mustfree = FALSE;
    char_u	*name = name_arg;

    만약 (c == -1)			    // finish up
    {
	만약 (did_title)
	    did_title = FALSE;
	아니면
	{
	    만약 (arg == NULL)
		msg(_("No marks set"));
	    아니면
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    아니면 만약 (!got_int
	    && (arg == NULL || vim_strchr(arg, c) != NULL)
	    && p->lnum != 0)
    {
	만약 (name == NULL && current)
	{
	    name = mark_line(p, 15);
	    만약 (name == NULL)
	    {
		emsg(_(e_out_of_memory));
		반환;
	    }
	    mustfree = TRUE;
	}
	만약 (!message_filtered(name))
	{
	    만약 (!did_title)
	    {
		// Highlight title
		msg_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    msg_putchar('\n');
	    만약 (!got_int)
	    {
		sprintf((char *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		msg_outtrans(IObuff);
		만약 (name != NULL)
		{
		    msg_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	만약 (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    void
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    정수		from, to;
    정수		i;
    정수		lower;
    정수		digit;
    정수		n;

    만약 (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    아니면 만약 (eap->forceit)
	emsg(_(e_invalid_argument));
    아니면 만약 (*eap->arg == NUL)
	emsg(_(e_argument_required));
    아니면
    {
	// clear specified marks only
	위해 (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    만약 (lower || digit || ASCII_ISUPPER(*p))
	    {
		만약 (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    만약 (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			반환;
		    }
		    p += 2;
		}
		아니면
		    // clear one lower case mark
		    from = to = *p;

		위해 (i = from; i <= to; ++i)
		{
		    만약 (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    아니면
		    {
			만약 (digit)
			    n = i - '0' + NMARKS;
			아니면
			    n = i - 'A';
			namedfm[n].fmark.mark.lnum = 0;
			namedfm[n].fmark.fnum = 0;
			VIM_CLEAR(namedfm[n].fname);
#ifdef FEAT_VIMINFO
			namedfm[n].time_set = digit ? 0 : vim_time();
#endif
		    }
		}
	    }
	    아니면
		switch (*p)
		{
		    case '"': curbuf->b_last_cursor.lnum = 0; break;
		    case '^': curbuf->b_last_insert.lnum = 0; break;
		    case '.': curbuf->b_last_change.lnum = 0; break;
		    case '[': curbuf->b_op_start.lnum    = 0; break;
		    case ']': curbuf->b_op_end.lnum      = 0; break;
		    case '<': curbuf->b_visual.vi_start.lnum = 0; break;
		    case '>': curbuf->b_visual.vi_end.lnum   = 0; break;
		    case ' ': break;
		    default:  semsg(_(e_invalid_argument_str), p);
			      반환;
		}
	}
    }
}

/*
 * print the jumplist
 */
    void
ex_jumps(exarg_T *eap UNUSED)
{
    정수		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    msg_puts_title(_("\n jump line  col file/text"));
    위해 (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	만약 (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    만약 (name == NULL && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    만약 (name == NULL || message_filtered(name))
	    {
		vim_free(name);
		continue;
	    }

	    msg_putchar('\n');
	    만약 (got_int)
	    {
		vim_free(name);
		break;
	    }
	    sprintf((char *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.col);
	    msg_outtrans(IObuff);
	    msg_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    만약 (curwin->w_jumplistidx == curwin->w_jumplistlen)
	msg_puts("\n>");
}

    void
ex_clearjumps(exarg_T *eap UNUSED)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    void
ex_changes(exarg_T *eap UNUSED)
{
    정수		i;
    char_u	*name;

    // Highlight title
    msg_puts_title(_("\nchange line  col text"));

    위해 (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	만약 (curbuf->b_changelist[i].lnum != 0)
	{
	    msg_putchar('\n');
	    만약 (got_int)
		break;
	    sprintf((char *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (long)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    msg_outtrans(IObuff);
	    name = mark_line(&curbuf->b_changelist[i], 17);
	    만약 (name == NULL)
		break;
	    msg_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    만약 (curwin->w_changelistidx == curbuf->b_changelistlen)
	msg_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	만약 (*lp >= line1 && *lp <= line2) \
	{ \
	    만약 (amount == MAXLNUM) \
		*lp = 0; \
	    아니면 \
		*lp += amount; \
	} \
	아니면 만약 (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	만약 (*lp >= line1 && *lp <= line2) \
	{ \
	    만약 (amount == MAXLNUM) \
		*lp = line1; \
	    아니면 \
		*lp += amount; \
	} \
	아니면 만약 (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	만약 (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    만약 (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    아니면 /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	아니면 만약 (amount_after && posp->lnum > line2) \
	    posp->lnum += amount_after; \
    }

/*
 * Adjust marks between "line1" and "line2" (inclusive) to move "amount" lines.
 * Must be called before changed_*(), appended_lines() or deleted_lines().
 * May be called before or after changing the text.
 * When deleting lines "line1" to "line2", use an "amount" of MAXLNUM: The
 * marks within this range are made invalid.
 * If "amount_after" is non-zero adjust marks after "line2".
 * Example: Delete lines 34 and 35: mark_adjust(34, 35, MAXLNUM, -2);
 * Example: Insert two lines below 55: mark_adjust(56, MAXLNUM, 2, 0);
 *				   or: mark_adjust(56, 55, MAXLNUM, 2);
 */
    void
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, TRUE);
}

    void
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, FALSE);
}

    static void
mark_adjust_internal(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after,
    정수		adjust_folds UNUSED)
{
    정수		i;
    정수		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    static pos_T initpos = {1, 0, 0};

    만약 (line2 < line1 && amount_after == 0L)	    // nothing to do
	반환;

    만약 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	위해 (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    만약 (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	위해 (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    만약 (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_insert.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	만약 (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	위해 (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_start.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_end.lnum));

#ifdef FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(NULL, line1, line2, amount, amount_after);
	// location lists
	FOR_ALL_TAB_WINDOWS(tab, win)
	    qf_mark_adjust(win, line1, line2, amount, amount_after);
#endif

#ifdef FEAT_SIGNS
	sign_mark_adjust(line1, line2, amount, amount_after);
#endif
    }

    // previous context mark
    one_adjust(&(curwin->w_pcmark.lnum));

    // previous pcmark
    one_adjust(&(curwin->w_prev_pcmark.lnum));

    // saved cursor for formatting
    만약 (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	만약 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    위해 (i = 0; i < win->w_jumplistlen; ++i)
		만약 (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	만약 (win->w_buffer == curbuf)
	{
	    만약 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		위해 (i = 0; i < win->w_tagstacklen; i++)
		    만약 (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    만약 (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    만약 (win != curwin)
	    {
		만약 (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    만약 (amount == MAXLNUM)	    // topline is deleted
		    {
			만약 (line1 <= 1)
			    win->w_topline = 1;
			아니면
			    win->w_topline = line1 - 1;
		    }
		    아니면 만약 (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		아니면 만약 (amount_after && win->w_topline > line2)
		{
		    win->w_topline += amount_after;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		one_adjust_cursor(&(win->w_cursor));
	    }

#ifdef FEAT_FOLDING
	    // adjust folds
	    만약 (adjust_folds)
		foldMarkAdjust(win, line1, line2, amount, amount_after);
#endif
	}
    }

#ifdef FEAT_DIFF
    // adjust diffs
    diff_mark_adjust(line1, line2, amount, amount_after);
#endif

    // adjust per-window "last cursor" positions
    FOR_ALL_BUF_WININFO(curbuf, wip)
	one_adjust_cursor(&(wip->wi_fpos));
}

// This code is used often, needs to be fast.
#define col_adjust(pp) \
    { \
	posp = pp; \
	만약 (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    만약 (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    아니면 만약 (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    아니면 \
		posp->col += col_amount; \
	} \
    }

/*
 * Adjust marks in line "lnum" at column "mincol" and further: add
 * "lnum_amount" to the line number and add "col_amount" to the column
 * position.
 * "spaces_removed" is the number of spaces that were removed, matters when the
 * cursor is inside them.
 */
    void
mark_col_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    long	lnum_amount,
    long	col_amount,
    정수		spaces_removed)
{
    정수		i;
    정수		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    만약 ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	반환; // nothing to do

    // named marks, lower case and upper case
    위해 (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	만약 (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    위해 (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	만약 (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_insert));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    위해 (i = 0; i < curbuf->b_changelistlen; ++i)
	col_adjust(&(curbuf->b_changelist[i]));

    // Visual area
    col_adjust(&(curbuf->b_visual.vi_start));
    col_adjust(&(curbuf->b_visual.vi_end));

    // previous context mark
    col_adjust(&(curwin->w_pcmark));

    // previous pcmark
    col_adjust(&(curwin->w_prev_pcmark));

    // saved cursor for formatting
    col_adjust(&saved_cursor);

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_WINDOWS(win)
    {
	// marks in the jumplist
	위해 (i = 0; i < win->w_jumplistlen; ++i)
	    만약 (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	만약 (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    위해 (i = 0; i < win->w_tagstacklen; i++)
		만약 (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    만약 (win != curwin)
		col_adjust(&win->w_cursor);
	}
    }
}

/*
 * When deleting lines, this may create duplicate marks in the
 * jumplist. They will be removed here for the specified window.
 * When "loadfiles" is TRUE first ensure entries have the "fnum" field set
 * (this may be a bit slow).
 */
    void
cleanup_jumplist(win_T *wp, 정수 loadfiles)
{
    정수	    i;
    정수	    from, to;
    정수	    mustfree;

    만약 (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	위해 (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    만약 ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    위해 (from = 0; from < wp->w_jumplistlen; ++from)
    {
	만약 (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	위해 (i = from + 1; i < wp->w_jumplistlen; ++i)
	    만약 (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		break;
	만약 (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	아니면 만약 (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_STACK);
	아니면				// adjacent duplicate
	    mustfree = TRUE;

	만약 (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	아니면
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    만약 (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    void
copy_jumplist(win_T *from, win_T *to)
{
    정수		i;

    위해 (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	만약 (from->w_jumplist[i].fname != NULL)
	    to->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    to->w_jumplistlen = from->w_jumplistlen;
    to->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    void
free_jumplist(win_T *wp)
{
    정수		i;

    위해 (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    void
set_last_cursor(win_T *win)
{
    만약 (win->w_buffer != NULL)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#만약 defined(EXITFREE)
    void
free_all_marks(void)
{
    정수		i;

    위해 (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	만약 (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#만약 defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(void)
{
    반환 namedfm;
}
#endif

#만약 defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    static 정수
add_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, 정수 bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    만약 (pos->lnum <= 0)
	반환 OK;

    d = dict_alloc();
    만약 (d == NULL)
	반환 FAIL;

    만약 (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	반환 FAIL;
    }

    lpos = list_alloc();
    만약 (lpos == NULL)
	반환 FAIL;

    list_append_number(lpos, bufnr);
    list_append_number(lpos, pos->lnum);
    list_append_number(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_number(lpos, pos->coladd);

    만약 (dict_add_string_len(d, "mark", mname, (정수)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != NULL && dict_add_string(d, "file", fname) == FAIL))
    {
	만약 (lpos->lv_refcount == 0)
	    list_free(lpos);
	반환 FAIL;
    }

    반환 OK;
}

/*
 * Get information about marks local to a buffer.
 */
    static void
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    정수		i;

    // Marks 'a' to 'z'
    위해 (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	add_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, NULL);
    }

    // Mark '' is a window local mark and not a buffer local mark
    add_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, NULL);

    add_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_last_cursor, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_start, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_end, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_last_insert, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_last_change, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_start, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_end, buf->b_fnum, NULL);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    static void
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    정수		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    위해 (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	만약 (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	아니면
	    name = namedfm[i].fname;
	만약 (name != NULL)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    만약 (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    void
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = NULL;

    만약 (rettv_list_alloc(rettv) == FAIL)
	반환;

    만약 (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	반환;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    만약 (buf == NULL)
	반환;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
