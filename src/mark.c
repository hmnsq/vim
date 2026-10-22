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

#ضمن "vim.h"

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
ساكن xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

ساكن فراغ fname2fnum(xfmark_T *fm);
ساكن فراغ fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
ساكن char_u *mark_هێڵ(pos_T *mp, صحيح lead_len);
ساكن فراغ show_one_mark(صحيح, char_u *, pos_T *, char_u *, صحيح current);
ساكن فراغ mark_adjust_internal(linenr_T line1, linenr_T line2, long amount,
    long amount_after, صحيح adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    صحيح
setmark(صحيح c)
{
    أرجع setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    صحيح
setmark_pos(صحيح c, pos_T *pos, صحيح fnum)
{
    صحيح		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    إذا (c < 0)
	أرجع FAIL;

    إذا (c == '\'' || c == '`')
    {
	إذا (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	وإلا
	    curwin->w_pcmark = *pos;
	أرجع OK;
    }

    buf = buflist_findnr(fnum);
    إذا (buf == عدم)
	أرجع FAIL;

    إذا (c == '"')
    {
	buf->b_last_cursor = *pos;
	أرجع OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    إذا (c == '[')
    {
	buf->b_op_start = *pos;
	أرجع OK;
    }
    إذا (c == ']')
    {
	buf->b_op_end = *pos;
	أرجع OK;
    }

    إذا (c == '<' || c == '>')
    {
	إذا (c == '<')
	    buf->b_visual.vi_start = *pos;
	وإلا
	    buf->b_visual.vi_end = *pos;
	إذا (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	أرجع OK;
    }

    إذا (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	أرجع OK;
    }
    إذا (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	إذا (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	وإلا
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	أرجع OK;
    }
    أرجع FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    فراغ
mark_forget_file(win_T *wp, صحيح fnum)
{
    صحيح		i;

    لكل (i = wp->w_jumplistlen - 1; i >= 0; --i)
	إذا (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    إذا (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * حجم(wp->w_jumplist[i]));
	}

    لكل (i = wp->w_tagstacklen - 1; i >= 0; --i)
	إذا (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    إذا (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * حجم(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    فراغ
setpcmark(فراغ)
{
    صحيح		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    إذا (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	أرجع;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    إذا (jop_flags & JOP_کەڵەکە)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	إذا (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    إذا (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	لكل (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = عدم;
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
    فراغ
checkpcmark(فراغ)
{
    إذا (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(صحيح ژماردن)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    إذا (curwin->w_jumplistlen == 0)	    // nothing to jump to
	أرجع (pos_T *)عدم;

    لكل (;;)
    {
	إذا (curwin->w_jumplistidx + ژماردن < 0
		|| curwin->w_jumplistidx + ژماردن >= curwin->w_jumplistlen)
	    أرجع (pos_T *)عدم;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	إذا (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    إذا (curwin->w_jumplistidx + ژماردن < 0)
		أرجع (pos_T *)عدم;
	}

	curwin->w_jumplistidx += ژماردن;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	إذا (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	إذا (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    إذا (buflist_findnr(fmark.fnum) == عدم)
	    {					     // Skip this one ..
		ژماردن += ژماردن < 0 ? -1 : 1;
		استمر;
	    }
	    إذا (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		أرجع (pos_T *)عدم;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	وإلا
	    pos = &(jmp->fmark.mark);
	أرجع pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(صحيح ژماردن)
{
    صحيح		n;

    إذا (curbuf->b_changelistlen == 0)	    // nothing to jump to
	أرجع (pos_T *)عدم;

    n = curwin->w_changelistidx;
    إذا (n + ژماردن < 0)
    {
	إذا (n == 0)
	    أرجع (pos_T *)عدم;
	n = 0;
    }
    وإلا إذا (n + ژماردن >= curbuf->b_changelistlen)
    {
	إذا (n == curbuf->b_changelistlen - 1)
	    أرجع (pos_T *)عدم;
	n = curbuf->b_changelistlen - 1;
    }
    وإلا
	n += ژماردن;
    curwin->w_changelistidx = n;
    أرجع curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, صحيح c, صحيح changefile)
{
    أرجع getmark_buf_fnum(buf, c, changefile, عدم);
}

    pos_T *
getmark(صحيح c, صحيح changefile)
{
    أرجع getmark_buf_fnum(curbuf, c, changefile, عدم);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    صحيح		c,
    صحيح		changefile,
    صحيح		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    ساكن pos_T	pos_copy;

    posp = عدم;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    إذا (c < 0)
	أرجع posp;
    إذا (c > '~')			// check for islower()/isupper()
	;
    وإلا إذا (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    وإلا إذا (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    وإلا إذا (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_زیادکردن);
    وإلا إذا (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    وإلا إذا (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    وإلا إذا (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    وإلا إذا (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	صحيح	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	إذا (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    وإلا إذا (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	صحيح	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	إذا (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    وإلا إذا (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	إذا (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	وإلا
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	إذا (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    إذا (c == '<')
		pos_copy.col = 0;
	    وإلا
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    وإلا إذا (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    وإلا إذا (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	إذا (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	وإلا
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	إذا (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	إذا (fnum != عدم)
	    *fnum = namedfm[c].fmark.fnum;
	وإلا إذا (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    إذا (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		إذا (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    أرجع (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    وإلا
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    أرجع posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    صحيح		dir,	// direction for search
    صحيح		begin_هێڵ)
{
    صحيح		i;
    pos_T	*result = عدم;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    إذا (dir == BACKWARD && begin_هێڵ)
	pos.col = 0;
    وإلا إذا (dir == FORWARD && begin_هێڵ)
	pos.col = MAXCOL;

    لكل (i = 0; i < NMARKS; i++)
    {
	إذا (curbuf->b_namedm[i].lnum > 0)
	{
	    إذا (dir == FORWARD)
	    {
		إذا ((result == عدم || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    وإلا
	    {
		إذا ((result == عدم || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    أرجع result;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    ساكن فراغ
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    إذا (fm->fname == عدم)
	أرجع;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    إذا (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    وإلا
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (فراغ)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    فراغ
fmarks_check_names(buf_T *buf)
{
    char_u	*name;
    صحيح		i;
    win_T	*wp;

    إذا (buf->b_ffname == عدم)
	أرجع;

    name = home_replace_save(buf, buf->b_ffname);
    إذا (name == عدم)
	أرجع;

    لكل (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	لكل (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    ساكن فراغ
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    إذا (fm->fmark.fnum == 0
	    && fm->fname != عدم
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
    صحيح
check_mark(pos_T *pos)
{
    إذا (pos == عدم)
    {
	emsg(_(e_unknown_mark));
	أرجع FAIL;
    }
    إذا (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	إذا (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	أرجع FAIL;
    }
    إذا (pos->lnum > curbuf->b_ml.ml_هێڵ_ژماردن)
    {
	emsg(_(e_mark_has_invalid_هێڵ_ژمار));
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    فراغ
clrallmarks(buf_T *buf)
{
    ساكن صحيح		i = -1;

    إذا (i == -1)	// first call ever: initialize
	لكل (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = عدم;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    لكل (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_start.lnum = 0;		// start/end op mark cleared
    buf->b_op_end.lnum = 0;
    buf->b_last_cursor.lnum = 1;	// '" mark cleared
    buf->b_last_cursor.col = 0;
    buf->b_last_cursor.coladd = 0;
    buf->b_last_زیادکردن.lnum = 0;	// '^ mark cleared
    buf->b_last_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, صحيح lead_len)
{
    إذا (fmark->fnum == curbuf->b_fnum)		    // current buffer
	أرجع mark_هێڵ(&(fmark->mark), lead_len);
    أرجع buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    ساكن char_u *
mark_هێڵ(pos_T *mp, صحيح lead_len)
{
    char_u	*s, *p;
    صحيح		len;

    إذا (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_هێڵ_ژماردن)
	أرجع vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    إذا (s == عدم)
	أرجع عدم;
    // Truncate the line to fit it in the window.
    len = 0;
    لكل (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	إذا (len >= Columns - lead_len)
	    اكسر;
    }
    *p = NUL;
    أرجع s;
}

/*
 * print the marks
 */
    فراغ
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحيح		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    إذا (arg != عدم && *arg == NUL)
	arg = عدم;

    show_one_mark('\'', arg, &curwin->w_pcmark, عدم, TRUE);
    لكل (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], عدم, TRUE);
    لكل (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	إذا (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	وإلا
	    name = namedfm[i].fname;
	إذا (name != عدم)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    إذا (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_last_cursor, عدم, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_start, عدم, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_end, عدم, TRUE);
    show_one_mark('^', arg, &curbuf->b_last_زیادکردن, عدم, TRUE);
    show_one_mark('.', arg, &curbuf->b_last_change, عدم, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_start;
    endp = &curbuf->b_visual.vi_end;
    إذا ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    وإلا
	posp = endp;
    show_one_mark('<', arg, posp, عدم, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, عدم, TRUE);

    show_one_mark(-1, arg, عدم, عدم, FALSE);
}

    ساكن فراغ
show_one_mark(
    صحيح		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    صحيح		current)	// in current file
{
    ساكن صحيح	did_title = FALSE;
    صحيح		mustfree = FALSE;
    char_u	*name = name_arg;

    إذا (c == -1)			    // finish up
    {
	إذا (did_title)
	    did_title = FALSE;
	وإلا
	{
	    إذا (arg == عدم)
		پەیام(_("No marks set"));
	    وإلا
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    وإلا إذا (!got_int
	    && (arg == عدم || vim_strchr(arg, c) != عدم)
	    && p->lnum != 0)
    {
	إذا (name == عدم && current)
	{
	    name = mark_هێڵ(p, 15);
	    إذا (name == عدم)
	    {
		emsg(_(e_out_of_memory));
		أرجع;
	    }
	    mustfree = TRUE;
	}
	إذا (!message_filtered(name))
	{
	    إذا (!did_title)
	    {
		// Highlight title
		پەیام_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    پەیام_putchar('\n');
	    إذا (!got_int)
	    {
		sprintf((حرف *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		پەیام_outtrans(IObuff);
		إذا (name != عدم)
		{
		    پەیام_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	إذا (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    فراغ
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    صحيح		from, to;
    صحيح		i;
    صحيح		lower;
    صحيح		digit;
    صحيح		n;

    إذا (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    وإلا إذا (eap->forceit)
	emsg(_(e_invalid_argument));
    وإلا إذا (*eap->arg == NUL)
	emsg(_(e_argument_required));
    وإلا
    {
	// clear specified marks only
	لكل (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    إذا (lower || digit || ASCII_ISUPPER(*p))
	    {
		إذا (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    إذا (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			أرجع;
		    }
		    p += 2;
		}
		وإلا
		    // clear one lower case mark
		    from = to = *p;

		لكل (i = from; i <= to; ++i)
		{
		    إذا (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    وإلا
		    {
			إذا (digit)
			    n = i - '0' + NMARKS;
			وإلا
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
	    وإلا
		بدل (*p)
		{
		    حالة '"': curbuf->b_last_cursor.lnum = 0; اكسر;
		    حالة '^': curbuf->b_last_زیادکردن.lnum = 0; اكسر;
		    حالة '.': curbuf->b_last_change.lnum = 0; اكسر;
		    حالة '[': curbuf->b_op_start.lnum    = 0; اكسر;
		    حالة ']': curbuf->b_op_end.lnum      = 0; اكسر;
		    حالة '<': curbuf->b_visual.vi_start.lnum = 0; اكسر;
		    حالة '>': curbuf->b_visual.vi_end.lnum   = 0; اكسر;
		    حالة ' ': اكسر;
		    افتراضي:  semsg(_(e_invalid_argument_str), p);
			      أرجع;
		}
	}
    }
}

/*
 * print the jumplist
 */
    فراغ
ex_jumps(exarg_T *eap UNUSED)
{
    صحيح		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    پەیام_puts_title(_("\n jump line  col file/text"));
    لكل (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	إذا (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    إذا (name == عدم && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    إذا (name == عدم || message_filtered(name))
	    {
		vim_free(name);
		استمر;
	    }

	    پەیام_putchar('\n');
	    إذا (got_int)
	    {
		vim_free(name);
		اكسر;
	    }
	    sprintf((حرف *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.col);
	    پەیام_outtrans(IObuff);
	    پەیام_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    إذا (curwin->w_jumplistidx == curwin->w_jumplistlen)
	پەیام_puts("\n>");
}

    فراغ
ex_clearjumps(exarg_T *eap UNUSED)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    فراغ
ex_changes(exarg_T *eap UNUSED)
{
    صحيح		i;
    char_u	*name;

    // Highlight title
    پەیام_puts_title(_("\nchange line  col text"));

    لكل (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	إذا (curbuf->b_changelist[i].lnum != 0)
	{
	    پەیام_putchar('\n');
	    إذا (got_int)
		اكسر;
	    sprintf((حرف *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (long)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    پەیام_outtrans(IObuff);
	    name = mark_هێڵ(&curbuf->b_changelist[i], 17);
	    إذا (name == عدم)
		اكسر;
	    پەیام_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    إذا (curwin->w_changelistidx == curbuf->b_changelistlen)
	پەیام_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	إذا (*lp >= line1 && *lp <= line2) \
	{ \
	    إذا (amount == MAXLNUM) \
		*lp = 0; \
	    وإلا \
		*lp += amount; \
	} \
	وإلا إذا (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	إذا (*lp >= line1 && *lp <= line2) \
	{ \
	    إذا (amount == MAXLNUM) \
		*lp = line1; \
	    وإلا \
		*lp += amount; \
	} \
	وإلا إذا (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	إذا (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    إذا (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    وإلا /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	وإلا إذا (amount_after && posp->lnum > line2) \
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
    فراغ
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, TRUE);
}

    فراغ
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, FALSE);
}

    ساكن فراغ
mark_adjust_internal(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after,
    صحيح		adjust_folds UNUSED)
{
    صحيح		i;
    صحيح		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    ساكن pos_T initpos = {1, 0, 0};

    إذا (line2 < line1 && amount_after == 0L)	    // nothing to do
	أرجع;

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	لكل (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    إذا (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	لكل (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    إذا (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_زیادکردن.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	إذا (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	لكل (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_start.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_end.lnum));

#ifdef FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(عدم, line1, line2, amount, amount_after);
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
    إذا (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    لكل (i = 0; i < win->w_jumplistlen; ++i)
		إذا (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	إذا (win->w_buffer == curbuf)
	{
	    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		لكل (i = 0; i < win->w_tagstacklen; i++)
		    إذا (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    إذا (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    إذا (win != curwin)
	    {
		إذا (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    إذا (amount == MAXLNUM)	    // topline is deleted
		    {
			إذا (line1 <= 1)
			    win->w_topline = 1;
			وإلا
			    win->w_topline = line1 - 1;
		    }
		    وإلا إذا (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		وإلا إذا (amount_after && win->w_topline > line2)
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
	    إذا (adjust_folds)
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
	إذا (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    إذا (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    وإلا إذا (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    وإلا \
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
    فراغ
mark_col_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    long	lnum_amount,
    long	col_amount,
    صحيح		spaces_removed)
{
    صحيح		i;
    صحيح		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    إذا ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	أرجع; // nothing to do

    // named marks, lower case and upper case
    لكل (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	إذا (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    لكل (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	إذا (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_زیادکردن));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    لكل (i = 0; i < curbuf->b_changelistlen; ++i)
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
	لكل (i = 0; i < win->w_jumplistlen; ++i)
	    إذا (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	إذا (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    لكل (i = 0; i < win->w_tagstacklen; i++)
		إذا (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    إذا (win != curwin)
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
    فراغ
cleanup_jumplist(win_T *wp, صحيح loadfiles)
{
    صحيح	    i;
    صحيح	    from, to;
    صحيح	    mustfree;

    إذا (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	لكل (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    إذا ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    لكل (from = 0; from < wp->w_jumplistlen; ++from)
    {
	إذا (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	لكل (i = from + 1; i < wp->w_jumplistlen; ++i)
	    إذا (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		اكسر;
	إذا (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	وإلا إذا (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_کەڵەکە);
	وإلا				// adjacent duplicate
	    mustfree = TRUE;

	إذا (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	وإلا
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    إذا (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    فراغ
copy_jumplist(win_T *from, win_T *to)
{
    صحيح		i;

    لكل (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	إذا (from->w_jumplist[i].fname != عدم)
	    to->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    to->w_jumplistlen = from->w_jumplistlen;
    to->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    فراغ
free_jumplist(win_T *wp)
{
    صحيح		i;

    لكل (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    فراغ
set_last_cursor(win_T *win)
{
    إذا (win->w_buffer != عدم)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#إذا defined(EXITFREE)
    فراغ
free_all_marks(فراغ)
{
    صحيح		i;

    لكل (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	إذا (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#إذا defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(فراغ)
{
    أرجع namedfm;
}
#endif

#إذا defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    ساكن صحيح
add_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, صحيح bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    إذا (pos->lnum <= 0)
	أرجع OK;

    d = dict_alloc();
    إذا (d == عدم)
	أرجع FAIL;

    إذا (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	أرجع FAIL;
    }

    lpos = list_alloc();
    إذا (lpos == عدم)
	أرجع FAIL;

    list_append_ژمار(lpos, bufnr);
    list_append_ژمار(lpos, pos->lnum);
    list_append_ژمار(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_ژمار(lpos, pos->coladd);

    إذا (dict_add_string_len(d, "mark", mname, (صحيح)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != عدم && dict_add_string(d, "file", fname) == FAIL))
    {
	إذا (lpos->lv_refcount == 0)
	    list_free(lpos);
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Get information about marks local to a buffer.
 */
    ساكن فراغ
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    صحيح		i;

    // Marks 'a' to 'z'
    لكل (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	add_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, عدم);
    }

    // Mark '' is a window local mark and not a buffer local mark
    add_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, عدم);

    add_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_last_cursor, buf->b_fnum, عدم);
    add_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_start, buf->b_fnum, عدم);
    add_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_end, buf->b_fnum, عدم);
    add_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_last_زیادکردن, buf->b_fnum, عدم);
    add_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_last_change, buf->b_fnum, عدم);
    add_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_start, buf->b_fnum, عدم);
    add_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_end, buf->b_fnum, عدم);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    ساكن فراغ
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    صحيح		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    لكل (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	إذا (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	وإلا
	    name = namedfm[i].fname;
	إذا (name != عدم)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    إذا (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    فراغ
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = عدم;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	أرجع;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
