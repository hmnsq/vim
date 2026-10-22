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
static char_u *mark_line(pos_T *mp, ціле lead_len);
static void show_one_mark(ціле, char_u *, pos_T *, char_u *, ціле current);
static void mark_adjust_internal(linenr_T line1, linenr_T line2, long amount,
    long amount_after, ціле adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    ціле
setmark(ціле c)
{
    повернути setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    ціле
setmark_pos(ціле c, pos_T *pos, ціле fnum)
{
    ціле		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    якщо (c < 0)
	повернути FAIL;

    якщо (c == '\'' || c == '`')
    {
	якщо (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	інакше
	    curwin->w_pcmark = *pos;
	повернути OK;
    }

    buf = buflist_findnr(fnum);
    якщо (buf == NULL)
	повернути FAIL;

    якщо (c == '"')
    {
	buf->b_last_cursor = *pos;
	повернути OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    якщо (c == '[')
    {
	buf->b_op_start = *pos;
	повернути OK;
    }
    якщо (c == ']')
    {
	buf->b_op_end = *pos;
	повернути OK;
    }

    якщо (c == '<' || c == '>')
    {
	якщо (c == '<')
	    buf->b_visual.vi_start = *pos;
	інакше
	    buf->b_visual.vi_end = *pos;
	якщо (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	повернути OK;
    }

    якщо (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	повернути OK;
    }
    якщо (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	якщо (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	інакше
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	повернути OK;
    }
    повернути FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    void
mark_forget_file(win_T *wp, ціле fnum)
{
    ціле		i;

    для (i = wp->w_jumplistlen - 1; i >= 0; --i)
	якщо (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    якщо (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * sizeof(wp->w_jumplist[i]));
	}

    для (i = wp->w_tagstacklen - 1; i >= 0; --i)
	якщо (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    якщо (wp->w_tagstackidx > i)
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
    ціле		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    якщо (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	повернути;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    якщо (jop_flags & JOP_STACK)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	якщо (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    якщо (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	для (i = 1; i < JUMPLISTSIZE; ++i)
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
    якщо (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(ціле count)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    якщо (curwin->w_jumplistlen == 0)	    // nothing to jump to
	повернути (pos_T *)NULL;

    для (;;)
    {
	якщо (curwin->w_jumplistidx + count < 0
		|| curwin->w_jumplistidx + count >= curwin->w_jumplistlen)
	    повернути (pos_T *)NULL;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	якщо (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    якщо (curwin->w_jumplistidx + count < 0)
		повернути (pos_T *)NULL;
	}

	curwin->w_jumplistidx += count;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	якщо (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	якщо (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    якщо (buflist_findnr(fmark.fnum) == NULL)
	    {					     // Skip this one ..
		count += count < 0 ? -1 : 1;
		continue;
	    }
	    якщо (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		повернути (pos_T *)NULL;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	інакше
	    pos = &(jmp->fmark.mark);
	повернути pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(ціле count)
{
    ціле		n;

    якщо (curbuf->b_changelistlen == 0)	    // nothing to jump to
	повернути (pos_T *)NULL;

    n = curwin->w_changelistidx;
    якщо (n + count < 0)
    {
	якщо (n == 0)
	    повернути (pos_T *)NULL;
	n = 0;
    }
    інакше якщо (n + count >= curbuf->b_changelistlen)
    {
	якщо (n == curbuf->b_changelistlen - 1)
	    повернути (pos_T *)NULL;
	n = curbuf->b_changelistlen - 1;
    }
    інакше
	n += count;
    curwin->w_changelistidx = n;
    повернути curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, ціле c, ціле changefile)
{
    повернути getmark_buf_fnum(buf, c, changefile, NULL);
}

    pos_T *
getmark(ціле c, ціле changefile)
{
    повернути getmark_buf_fnum(curbuf, c, changefile, NULL);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    ціле		c,
    ціле		changefile,
    ціле		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    static pos_T	pos_copy;

    posp = NULL;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    якщо (c < 0)
	повернути posp;
    якщо (c > '~')			// check for islower()/isupper()
	;
    інакше якщо (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    інакше якщо (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    інакше якщо (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_insert);
    інакше якщо (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    інакше якщо (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    інакше якщо (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    інакше якщо (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	ціле	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	якщо (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    інакше якщо (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	ціле	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	якщо (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    інакше якщо (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	якщо (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	інакше
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	якщо (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    якщо (c == '<')
		pos_copy.col = 0;
	    інакше
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    інакше якщо (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    інакше якщо (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	якщо (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	інакше
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	якщо (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	якщо (fnum != NULL)
	    *fnum = namedfm[c].fmark.fnum;
	інакше якщо (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    якщо (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		якщо (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    повернути (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    інакше
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    повернути posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    ціле		dir,	// direction for search
    ціле		begin_line)
{
    ціле		i;
    pos_T	*result = NULL;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    якщо (dir == BACKWARD && begin_line)
	pos.col = 0;
    інакше якщо (dir == FORWARD && begin_line)
	pos.col = MAXCOL;

    для (i = 0; i < NMARKS; i++)
    {
	якщо (curbuf->b_namedm[i].lnum > 0)
	{
	    якщо (dir == FORWARD)
	    {
		якщо ((result == NULL || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    інакше
	    {
		якщо ((result == NULL || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    повернути result;
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

    якщо (fm->fname == NULL)
	повернути;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    якщо (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    інакше
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
    ціле		i;
    win_T	*wp;

    якщо (buf->b_ffname == NULL)
	повернути;

    name = home_replace_save(buf, buf->b_ffname);
    якщо (name == NULL)
	повернути;

    для (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	для (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    static void
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    якщо (fm->fmark.fnum == 0
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
    ціле
check_mark(pos_T *pos)
{
    якщо (pos == NULL)
    {
	emsg(_(e_unknown_mark));
	повернути FAIL;
    }
    якщо (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	якщо (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	повернути FAIL;
    }
    якщо (pos->lnum > curbuf->b_ml.ml_line_count)
    {
	emsg(_(e_mark_has_invalid_line_number));
	повернути FAIL;
    }
    повернути OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    void
clrallmarks(buf_T *buf)
{
    static ціле		i = -1;

    якщо (i == -1)	// first call ever: initialize
	для (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = NULL;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    для (i = 0; i < NMARKS; i++)
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
fm_getname(fmark_T *fmark, ціле lead_len)
{
    якщо (fmark->fnum == curbuf->b_fnum)		    // current buffer
	повернути mark_line(&(fmark->mark), lead_len);
    повернути buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    static char_u *
mark_line(pos_T *mp, ціле lead_len)
{
    char_u	*s, *p;
    ціле		len;

    якщо (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_line_count)
	повернути vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    якщо (s == NULL)
	повернути NULL;
    // Truncate the line to fit it in the window.
    len = 0;
    для (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	якщо (len >= Columns - lead_len)
	    break;
    }
    *p = NUL;
    повернути s;
}

/*
 * print the marks
 */
    void
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ціле		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    якщо (arg != NULL && *arg == NUL)
	arg = NULL;

    show_one_mark('\'', arg, &curwin->w_pcmark, NULL, TRUE);
    для (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], NULL, TRUE);
    для (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	якщо (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	інакше
	    name = namedfm[i].fname;
	якщо (name != NULL)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    якщо (namedfm[i].fmark.fnum != 0)
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
    якщо ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    інакше
	posp = endp;
    show_one_mark('<', arg, posp, NULL, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, NULL, TRUE);

    show_one_mark(-1, arg, NULL, NULL, FALSE);
}

    static void
show_one_mark(
    ціле		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    ціле		current)	// in current file
{
    static ціле	did_title = FALSE;
    ціле		mustfree = FALSE;
    char_u	*name = name_arg;

    якщо (c == -1)			    // finish up
    {
	якщо (did_title)
	    did_title = FALSE;
	інакше
	{
	    якщо (arg == NULL)
		msg(_("No marks set"));
	    інакше
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    інакше якщо (!got_int
	    && (arg == NULL || vim_strchr(arg, c) != NULL)
	    && p->lnum != 0)
    {
	якщо (name == NULL && current)
	{
	    name = mark_line(p, 15);
	    якщо (name == NULL)
	    {
		emsg(_(e_out_of_memory));
		повернути;
	    }
	    mustfree = TRUE;
	}
	якщо (!message_filtered(name))
	{
	    якщо (!did_title)
	    {
		// Highlight title
		msg_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    msg_putchar('\n');
	    якщо (!got_int)
	    {
		sprintf((char *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		msg_outtrans(IObuff);
		якщо (name != NULL)
		{
		    msg_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	якщо (mustfree)
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
    ціле		from, to;
    ціле		i;
    ціле		lower;
    ціле		digit;
    ціле		n;

    якщо (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    інакше якщо (eap->forceit)
	emsg(_(e_invalid_argument));
    інакше якщо (*eap->arg == NUL)
	emsg(_(e_argument_required));
    інакше
    {
	// clear specified marks only
	для (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    якщо (lower || digit || ASCII_ISUPPER(*p))
	    {
		якщо (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    якщо (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			повернути;
		    }
		    p += 2;
		}
		інакше
		    // clear one lower case mark
		    from = to = *p;

		для (i = from; i <= to; ++i)
		{
		    якщо (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    інакше
		    {
			якщо (digit)
			    n = i - '0' + NMARKS;
			інакше
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
	    інакше
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
			      повернути;
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
    ціле		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    msg_puts_title(_("\n jump line  col file/text"));
    для (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	якщо (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    якщо (name == NULL && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    якщо (name == NULL || message_filtered(name))
	    {
		vim_free(name);
		continue;
	    }

	    msg_putchar('\n');
	    якщо (got_int)
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
    якщо (curwin->w_jumplistidx == curwin->w_jumplistlen)
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
    ціле		i;
    char_u	*name;

    // Highlight title
    msg_puts_title(_("\nchange line  col text"));

    для (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	якщо (curbuf->b_changelist[i].lnum != 0)
	{
	    msg_putchar('\n');
	    якщо (got_int)
		break;
	    sprintf((char *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (long)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    msg_outtrans(IObuff);
	    name = mark_line(&curbuf->b_changelist[i], 17);
	    якщо (name == NULL)
		break;
	    msg_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    якщо (curwin->w_changelistidx == curbuf->b_changelistlen)
	msg_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	якщо (*lp >= line1 && *lp <= line2) \
	{ \
	    якщо (amount == MAXLNUM) \
		*lp = 0; \
	    інакше \
		*lp += amount; \
	} \
	інакше якщо (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	якщо (*lp >= line1 && *lp <= line2) \
	{ \
	    якщо (amount == MAXLNUM) \
		*lp = line1; \
	    інакше \
		*lp += amount; \
	} \
	інакше якщо (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	якщо (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    якщо (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    інакше /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	інакше якщо (amount_after && posp->lnum > line2) \
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
    ціле		adjust_folds UNUSED)
{
    ціле		i;
    ціле		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    static pos_T initpos = {1, 0, 0};

    якщо (line2 < line1 && amount_after == 0L)	    // nothing to do
	повернути;

    якщо ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	для (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    якщо (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	для (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    якщо (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_insert.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	якщо (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	для (i = 0; i < curbuf->b_changelistlen; ++i)
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
    якщо (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	якщо ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    для (i = 0; i < win->w_jumplistlen; ++i)
		якщо (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	якщо (win->w_buffer == curbuf)
	{
	    якщо ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		для (i = 0; i < win->w_tagstacklen; i++)
		    якщо (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    якщо (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    якщо (win != curwin)
	    {
		якщо (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    якщо (amount == MAXLNUM)	    // topline is deleted
		    {
			якщо (line1 <= 1)
			    win->w_topline = 1;
			інакше
			    win->w_topline = line1 - 1;
		    }
		    інакше якщо (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		інакше якщо (amount_after && win->w_topline > line2)
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
	    якщо (adjust_folds)
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
	якщо (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    якщо (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    інакше якщо (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    інакше \
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
    ціле		spaces_removed)
{
    ціле		i;
    ціле		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    якщо ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	повернути; // nothing to do

    // named marks, lower case and upper case
    для (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	якщо (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    для (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	якщо (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_insert));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    для (i = 0; i < curbuf->b_changelistlen; ++i)
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
	для (i = 0; i < win->w_jumplistlen; ++i)
	    якщо (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	якщо (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    для (i = 0; i < win->w_tagstacklen; i++)
		якщо (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    якщо (win != curwin)
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
cleanup_jumplist(win_T *wp, ціле loadfiles)
{
    ціле	    i;
    ціле	    from, to;
    ціле	    mustfree;

    якщо (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	для (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    якщо ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    для (from = 0; from < wp->w_jumplistlen; ++from)
    {
	якщо (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	для (i = from + 1; i < wp->w_jumplistlen; ++i)
	    якщо (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		break;
	якщо (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	інакше якщо (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_STACK);
	інакше				// adjacent duplicate
	    mustfree = TRUE;

	якщо (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	інакше
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    якщо (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    void
copy_jumplist(win_T *from, win_T *to)
{
    ціле		i;

    для (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	якщо (from->w_jumplist[i].fname != NULL)
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
    ціле		i;

    для (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    void
set_last_cursor(win_T *win)
{
    якщо (win->w_buffer != NULL)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#якщо defined(EXITFREE)
    void
free_all_marks(void)
{
    ціле		i;

    для (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	якщо (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#якщо defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(void)
{
    повернути namedfm;
}
#endif

#якщо defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    static ціле
add_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, ціле bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    якщо (pos->lnum <= 0)
	повернути OK;

    d = dict_alloc();
    якщо (d == NULL)
	повернути FAIL;

    якщо (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	повернути FAIL;
    }

    lpos = list_alloc();
    якщо (lpos == NULL)
	повернути FAIL;

    list_append_number(lpos, bufnr);
    list_append_number(lpos, pos->lnum);
    list_append_number(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_number(lpos, pos->coladd);

    якщо (dict_add_string_len(d, "mark", mname, (ціле)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != NULL && dict_add_string(d, "file", fname) == FAIL))
    {
	якщо (lpos->lv_refcount == 0)
	    list_free(lpos);
	повернути FAIL;
    }

    повернути OK;
}

/*
 * Get information about marks local to a buffer.
 */
    static void
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ціле		i;

    // Marks 'a' to 'z'
    для (i = 0; i < NMARKS; ++i)
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
    ціле		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    для (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	якщо (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	інакше
	    name = namedfm[i].fname;
	якщо (name != NULL)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    якщо (namedfm[i].fmark.fnum != 0)
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

    якщо (rettv_list_alloc(rettv) == FAIL)
	повернути;

    якщо (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	повернути;

    якщо (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	повернути;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    якщо (buf == NULL)
	повернути;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
