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

#شامل "vim.h"

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
ساکن xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

ساکن خالی fname2fnum(xfmark_T *fm);
ساکن خالی fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
ساکن char_u *mark_سطر(pos_T *mp, صحیح lead_len);
ساکن خالی show_one_mark(صحیح, char_u *, pos_T *, char_u *, صحیح current);
ساکن خالی mark_adjust_اندرونی(linenr_T line1, linenr_T line2, طویل amount,
    طویل amount_after, صحیح adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    صحیح
setmark(صحیح c)
{
    واپس setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    صحیح
setmark_pos(صحیح c, pos_T *pos, صحیح fnum)
{
    صحیح		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    اگر (c < 0)
	واپس FAIL;

    اگر (c == '\'' || c == '`')
    {
	اگر (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_پچھلا_pcmark = curwin->w_pcmark;
	}
	ورنہ
	    curwin->w_pcmark = *pos;
	واپس OK;
    }

    buf = buflist_findnr(fnum);
    اگر (buf == تہی)
	واپس FAIL;

    اگر (c == '"')
    {
	buf->b_آخری_cursor = *pos;
	واپس OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    اگر (c == '[')
    {
	buf->b_op_آغاز = *pos;
	واپس OK;
    }
    اگر (c == ']')
    {
	buf->b_op_انتہا = *pos;
	واپس OK;
    }

    اگر (c == '<' || c == '>')
    {
	اگر (c == '<')
	    buf->b_visual.vi_آغاز = *pos;
	ورنہ
	    buf->b_visual.vi_انتہا = *pos;
	اگر (buf->b_visual.vi_منوال == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_منوال = 'v';
	واپس OK;
    }

    اگر (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	واپس OK;
    }
    اگر (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	اگر (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	ورنہ
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#اگر_تعریف_شدہ FEAT_VIMINFO
	namedfm[i].وقت_سیٹ = vim_وقت();
#اگر_ختم
	واپس OK;
    }
    واپس FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    خالی
mark_forget_file(win_T *wp, صحیح fnum)
{
    صحیح		i;

    برائے (i = wp->w_jumplistlen - 1; i >= 0; --i)
	اگر (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    اگر (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * حجم(wp->w_jumplist[i]));
	}

    برائے (i = wp->w_tagstacklen - 1; i >= 0; --i)
	اگر (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_اندراج(&wp->w_tagstack[i]);
	    اگر (wp->w_tagstackidx > i)
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
    خالی
setpcmark(خالی)
{
    صحیح		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    اگر (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	واپس;

    curwin->w_پچھلا_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    اگر (jop_flags & JOP_ڈھیر)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	اگر (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    اگر (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	برائے (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = تہی;
#اگر_تعریف_شدہ FEAT_VIMINFO
    fm->وقت_سیٹ = vim_وقت();
#اگر_ختم
}

/*
 * To change context, call setpcmark(), then move the current position to
 * where ever, then call checkpcmark().  This ensures that the previous
 * context will only be changed if the cursor moved to a different line.
 * If pcmark was deleted (with "dG") the previous mark is restored.
 */
    خالی
checkpcmark(خالی)
{
    اگر (curwin->w_پچھلا_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_پچھلا_pcmark;
    curwin->w_پچھلا_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(صحیح گنتی)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    اگر (curwin->w_jumplistlen == 0)	    // nothing to jump to
	واپس (pos_T *)تہی;

    برائے (;;)
    {
	اگر (curwin->w_jumplistidx + گنتی < 0
		|| curwin->w_jumplistidx + گنتی >= curwin->w_jumplistlen)
	    واپس (pos_T *)تہی;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	اگر (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    اگر (curwin->w_jumplistidx + گنتی < 0)
		واپس (pos_T *)تہی;
	}

	curwin->w_jumplistidx += گنتی;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	اگر (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	اگر (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    اگر (buflist_findnr(fmark.fnum) == تہی)
	    {					     // Skip this one ..
		گنتی += گنتی < 0 ? -1 : 1;
		جاری;
	    }
	    اگر (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		واپس (pos_T *)تہی;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	ورنہ
	    pos = &(jmp->fmark.mark);
	واپس pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(صحیح گنتی)
{
    صحیح		n;

    اگر (curbuf->b_changelistlen == 0)	    // nothing to jump to
	واپس (pos_T *)تہی;

    n = curwin->w_changelistidx;
    اگر (n + گنتی < 0)
    {
	اگر (n == 0)
	    واپس (pos_T *)تہی;
	n = 0;
    }
    ورنہ اگر (n + گنتی >= curbuf->b_changelistlen)
    {
	اگر (n == curbuf->b_changelistlen - 1)
	    واپس (pos_T *)تہی;
	n = curbuf->b_changelistlen - 1;
    }
    ورنہ
	n += گنتی;
    curwin->w_changelistidx = n;
    واپس curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, صحیح c, صحیح changefile)
{
    واپس getmark_buf_fnum(buf, c, changefile, تہی);
}

    pos_T *
getmark(صحیح c, صحیح changefile)
{
    واپس getmark_buf_fnum(curbuf, c, changefile, تہی);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    صحیح		c,
    صحیح		changefile,
    صحیح		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    ساکن pos_T	pos_copy;

    posp = تہی;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    اگر (c < 0)
	واپس posp;
    اگر (c > '~')			// check for islower()/isupper()
	;
    ورنہ اگر (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    ورنہ اگر (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_آخری_cursor);
    ورنہ اگر (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_آخری_داخل_کرو);
    ورنہ اگر (c == '.')			// to where last change was made
	posp = &(buf->b_آخری_change);
    ورنہ اگر (c == '[')			// to start of previous operator
	posp = &(buf->b_op_آغاز);
    ورنہ اگر (c == ']')			// to end of previous operator
	posp = &(buf->b_op_انتہا);
    ورنہ اگر (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	صحیح	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	اگر (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ورنہ اگر (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	صحیح	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	اگر (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ورنہ اگر (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_آغاز;
	endp = &buf->b_visual.vi_انتہا;
	اگر (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	ورنہ
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	اگر (buf->b_visual.vi_منوال == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    اگر (c == '<')
		pos_copy.ستون = 0;
	    ورنہ
		pos_copy.ستون = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    ورنہ اگر (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    ورنہ اگر (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	اگر (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	ورنہ
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	اگر (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	اگر (fnum != تہی)
	    *fnum = namedfm[c].fmark.fnum;
	ورنہ اگر (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    اگر (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		اگر (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    واپس (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    ورنہ
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    واپس posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    صحیح		dir,	// direction for search
    صحیح		begin_سطر)
{
    صحیح		i;
    pos_T	*نتیجہ = تہی;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    اگر (dir == BACKWARD && begin_سطر)
	pos.ستون = 0;
    ورنہ اگر (dir == FORWARD && begin_سطر)
	pos.ستون = MAXCOL;

    برائے (i = 0; i < NMARKS; i++)
    {
	اگر (curbuf->b_namedm[i].lnum > 0)
	{
	    اگر (dir == FORWARD)
	    {
		اگر ((نتیجہ == تہی || LT_POS(curbuf->b_namedm[i], *نتیجہ))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    نتیجہ = &curbuf->b_namedm[i];
	    }
	    ورنہ
	    {
		اگر ((نتیجہ == تہی || LT_POS(*نتیجہ, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    نتیجہ = &curbuf->b_namedm[i];
	    }
	}
    }

    واپس نتیجہ;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    ساکن خالی
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    اگر (fm->fname == تہی)
	واپس;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    اگر (fm->fname[0] == '~' && (fm->fname[1] == '/'
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
		|| fm->fname[1] == '\\'
#اگر_ختم
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    ورنہ
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (خالی)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    خالی
fmarks_check_نام(buf_T *buf)
{
    char_u	*name;
    صحیح		i;
    win_T	*wp;

    اگر (buf->b_ffname == تہی)
	واپس;

    name = home_replace_save(buf, buf->b_ffname);
    اگر (name == تہی)
	واپس;

    برائے (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	برائے (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    ساکن خالی
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    اگر (fm->fmark.fnum == 0
	    && fm->fname != تہی
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
    صحیح
check_mark(pos_T *pos)
{
    اگر (pos == تہی)
    {
	emsg(_(e_unknown_mark));
	واپس FAIL;
    }
    اگر (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	اگر (pos->lnum == 0)
	    emsg(_(e_mark_not_سیٹ));
	واپس FAIL;
    }
    اگر (pos->lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	emsg(_(e_mark_has_invalid_سطر_نمبر));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    خالی
clrallmarks(buf_T *buf)
{
    ساکن صحیح		i = -1;

    اگر (i == -1)	// first call ever: initialize
	برائے (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = تہی;
#اگر_تعریف_شدہ FEAT_VIMINFO
	    namedfm[i].وقت_سیٹ = 0;
#اگر_ختم
	}

    برائے (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_آغاز.lnum = 0;		// start/end op mark cleared
    buf->b_op_انتہا.lnum = 0;
    buf->b_آخری_cursor.lnum = 1;	// '" mark cleared
    buf->b_آخری_cursor.ستون = 0;
    buf->b_آخری_cursor.coladd = 0;
    buf->b_آخری_داخل_کرو.lnum = 0;	// '^ mark cleared
    buf->b_آخری_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, صحیح lead_len)
{
    اگر (fmark->fnum == curbuf->b_fnum)		    // current buffer
	واپس mark_سطر(&(fmark->mark), lead_len);
    واپس buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    ساکن char_u *
mark_سطر(pos_T *mp, صحیح lead_len)
{
    char_u	*s, *p;
    صحیح		len;

    اگر (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_سطر_گنتی)
	واپس vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    اگر (s == تہی)
	واپس تہی;
    // Truncate the line to fit it in the window.
    len = 0;
    برائے (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	اگر (len >= Columns - lead_len)
	    توڑو;
    }
    *p = NUL;
    واپس s;
}

/*
 * print the marks
 */
    خالی
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحیح		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    اگر (arg != تہی && *arg == NUL)
	arg = تہی;

    show_one_mark('\'', arg, &curwin->w_pcmark, تہی, TRUE);
    برائے (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], تہی, TRUE);
    برائے (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	اگر (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	ورنہ
	    name = namedfm[i].fname;
	اگر (name != تہی)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    اگر (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_آخری_cursor, تہی, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_آغاز, تہی, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_انتہا, تہی, TRUE);
    show_one_mark('^', arg, &curbuf->b_آخری_داخل_کرو, تہی, TRUE);
    show_one_mark('.', arg, &curbuf->b_آخری_change, تہی, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_آغاز;
    endp = &curbuf->b_visual.vi_انتہا;
    اگر ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    ورنہ
	posp = endp;
    show_one_mark('<', arg, posp, تہی, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, تہی, TRUE);

    show_one_mark(-1, arg, تہی, تہی, FALSE);
}

    ساکن خالی
show_one_mark(
    صحیح		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    صحیح		current)	// in current file
{
    ساکن صحیح	did_title = FALSE;
    صحیح		mustfree = FALSE;
    char_u	*name = name_arg;

    اگر (c == -1)			    // finish up
    {
	اگر (did_title)
	    did_title = FALSE;
	ورنہ
	{
	    اگر (arg == تہی)
		پیغام(_("No marks set"));
	    ورنہ
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    ورنہ اگر (!got_int
	    && (arg == تہی || vim_strchr(arg, c) != تہی)
	    && p->lnum != 0)
    {
	اگر (name == تہی && current)
	{
	    name = mark_سطر(p, 15);
	    اگر (name == تہی)
	    {
		emsg(_(e_برآمد_کا_حافظہ));
		واپس;
	    }
	    mustfree = TRUE;
	}
	اگر (!پیغام_filtered(name))
	{
	    اگر (!did_title)
	    {
		// Highlight title
		پیغام_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    پیغام_putchar('\n');
	    اگر (!got_int)
	    {
		sprintf((حرف *)IObuff, " %c %6ld %4d ", c, p->lnum, p->ستون);
		پیغام_outtrans(IObuff);
		اگر (name != تہی)
		{
		    پیغام_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    برآمد_flush();		    // show one line at a time
	}
	اگر (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    خالی
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    صحیح		from, مقصد;
    صحیح		i;
    صحیح		زیریں;
    صحیح		digit;
    صحیح		n;

    اگر (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    ورنہ اگر (eap->forceit)
	emsg(_(e_invalid_دلیل));
    ورنہ اگر (*eap->arg == NUL)
	emsg(_(e_دلیل_required));
    ورنہ
    {
	// clear specified marks only
	برائے (p = eap->arg; *p != NUL; ++p)
	{
	    زیریں = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    اگر (زیریں || digit || ASCII_ISUPPER(*p))
	    {
		اگر (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    مقصد = p[2];
		    اگر (!(زیریں ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || مقصد < from)
		    {
			semsg(_(e_invalid_دلیل_str), p);
			واپس;
		    }
		    p += 2;
		}
		ورنہ
		    // clear one lower case mark
		    from = مقصد = *p;

		برائے (i = from; i <= مقصد; ++i)
		{
		    اگر (زیریں)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    ورنہ
		    {
			اگر (digit)
			    n = i - '0' + NMARKS;
			ورنہ
			    n = i - 'A';
			namedfm[n].fmark.mark.lnum = 0;
			namedfm[n].fmark.fnum = 0;
			VIM_CLEAR(namedfm[n].fname);
#اگر_تعریف_شدہ FEAT_VIMINFO
			namedfm[n].وقت_سیٹ = digit ? 0 : vim_وقت();
#اگر_ختم
		    }
		}
	    }
	    ورنہ
		انتخاب (*p)
		{
		    حالت '"': curbuf->b_آخری_cursor.lnum = 0; توڑو;
		    حالت '^': curbuf->b_آخری_داخل_کرو.lnum = 0; توڑو;
		    حالت '.': curbuf->b_آخری_change.lnum = 0; توڑو;
		    حالت '[': curbuf->b_op_آغاز.lnum    = 0; توڑو;
		    حالت ']': curbuf->b_op_انتہا.lnum      = 0; توڑو;
		    حالت '<': curbuf->b_visual.vi_آغاز.lnum = 0; توڑو;
		    حالت '>': curbuf->b_visual.vi_انتہا.lnum   = 0; توڑو;
		    حالت ' ': توڑو;
		    طےشدہ:  semsg(_(e_invalid_دلیل_str), p);
			      واپس;
		}
	}
    }
}

/*
 * print the jumplist
 */
    خالی
ex_jumps(exarg_T *eap غیر_مستعمل)
{
    صحیح		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    پیغام_puts_title(_("\n jump line  col file/text"));
    برائے (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	اگر (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    اگر (name == تہی && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    اگر (name == تہی || پیغام_filtered(name))
	    {
		vim_free(name);
		جاری;
	    }

	    پیغام_putchar('\n');
	    اگر (got_int)
	    {
		vim_free(name);
		توڑو;
	    }
	    sprintf((حرف *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.ستون);
	    پیغام_outtrans(IObuff);
	    پیغام_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	برآمد_flush();
    }
    اگر (curwin->w_jumplistidx == curwin->w_jumplistlen)
	پیغام_puts("\n>");
}

    خالی
ex_clearjumps(exarg_T *eap غیر_مستعمل)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    خالی
ex_changes(exarg_T *eap غیر_مستعمل)
{
    صحیح		i;
    char_u	*name;

    // Highlight title
    پیغام_puts_title(_("\nchange line  col text"));

    برائے (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	اگر (curbuf->b_changelist[i].lnum != 0)
	{
	    پیغام_putchar('\n');
	    اگر (got_int)
		توڑو;
	    sprintf((حرف *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (طویل)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].ستون);
	    پیغام_outtrans(IObuff);
	    name = mark_سطر(&curbuf->b_changelist[i], 17);
	    اگر (name == تہی)
		توڑو;
	    پیغام_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	برآمد_flush();
    }
    اگر (curwin->w_changelistidx == curbuf->b_changelistlen)
	پیغام_puts("\n>");
}

#تعریف_کرو one_adjust(جوڑو) \
    { \
	lp = جوڑو; \
	اگر (*lp >= line1 && *lp <= line2) \
	{ \
	    اگر (amount == MAXLNUM) \
		*lp = 0; \
	    ورنہ \
		*lp += amount; \
	} \
	ورنہ اگر (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#تعریف_کرو one_adjust_nodel(جوڑو) \
    { \
	lp = جوڑو; \
	اگر (*lp >= line1 && *lp <= line2) \
	{ \
	    اگر (amount == MAXLNUM) \
		*lp = line1; \
	    ورنہ \
		*lp += amount; \
	} \
	ورنہ اگر (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#تعریف_کرو one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	اگر (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    اگر (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->ستون = 0; \
	    } \
	    ورنہ /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	ورنہ اگر (amount_after && posp->lnum > line2) \
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
    خالی
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    طویل	amount,
    طویل	amount_after)
{
    mark_adjust_اندرونی(line1, line2, amount, amount_after, TRUE);
}

    خالی
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    طویل	amount,
    طویل	amount_after)
{
    mark_adjust_اندرونی(line1, line2, amount, amount_after, FALSE);
}

    ساکن خالی
mark_adjust_اندرونی(
    linenr_T	line1,
    linenr_T	line2,
    طویل	amount,
    طویل	amount_after,
    صحیح		adjust_folds غیر_مستعمل)
{
    صحیح		i;
    صحیح		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    ساکن pos_T initpos = {1, 0, 0};

    اگر (line2 < line1 && amount_after == 0L)	    // nothing to do
	واپس;

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	برائے (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    اگر (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	برائے (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    اگر (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_آخری_داخل_کرو.lnum));

	// last change position
	one_adjust(&(curbuf->b_آخری_change.lnum));

	// last cursor position, if it was set
	اگر (!EQUAL_POS(curbuf->b_آخری_cursor, initpos))
	    one_adjust(&(curbuf->b_آخری_cursor.lnum));


	// list of change positions
	برائے (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_آغاز.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_انتہا.lnum));

#اگر_تعریف_شدہ FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(تہی, line1, line2, amount, amount_after);
	// location lists
	FOR_ALL_TAB_WINDOWS(tab, win)
	    qf_mark_adjust(win, line1, line2, amount, amount_after);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SIGNS
	sign_mark_adjust(line1, line2, amount, amount_after);
#اگر_ختم
    }

    // previous context mark
    one_adjust(&(curwin->w_pcmark.lnum));

    // previous pcmark
    one_adjust(&(curwin->w_پچھلا_pcmark.lnum));

    // saved cursor for formatting
    اگر (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    برائے (i = 0; i < win->w_jumplistlen; ++i)
		اگر (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	اگر (win->w_بفر == curbuf)
	{
	    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		برائے (i = 0; i < win->w_tagstacklen; i++)
		    اگر (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    اگر (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    اگر (win != curwin)
	    {
		اگر (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    اگر (amount == MAXLNUM)	    // topline is deleted
		    {
			اگر (line1 <= 1)
			    win->w_topline = 1;
			ورنہ
			    win->w_topline = line1 - 1;
		    }
		    ورنہ اگر (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#اگر_تعریف_شدہ FEAT_DIFF
		    win->w_topfill = 0;
#اگر_ختم
		}
		ورنہ اگر (amount_after && win->w_topline > line2)
		{
		    win->w_topline += amount_after;
#اگر_تعریف_شدہ FEAT_DIFF
		    win->w_topfill = 0;
#اگر_ختم
		}
		one_adjust_cursor(&(win->w_cursor));
	    }

#اگر_تعریف_شدہ FEAT_FOLDING
	    // adjust folds
	    اگر (adjust_folds)
		foldMarkAdjust(win, line1, line2, amount, amount_after);
#اگر_ختم
	}
    }

#اگر_تعریف_شدہ FEAT_DIFF
    // adjust diffs
    diff_mark_adjust(line1, line2, amount, amount_after);
#اگر_ختم

    // adjust per-window "last cursor" positions
    FOR_ALL_BUF_WININFO(curbuf, wip)
	one_adjust_cursor(&(wip->wi_fpos));
}

// This code is used often, needs to be fast.
#تعریف_کرو ستون_adjust(pp) \
    { \
	posp = pp; \
	اگر (posp->lnum == lnum && posp->ستون >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    اگر (ستون_amount < 0 && posp->ستون <= (colnr_T)-ستون_amount) \
		posp->ستون = 0; \
	    ورنہ اگر (posp->ستون < spaces_removed) \
		posp->ستون = ستون_amount + spaces_removed; \
	    ورنہ \
		posp->ستون += ستون_amount; \
	} \
    }

/*
 * Adjust marks in line "lnum" at column "mincol" and further: add
 * "lnum_amount" to the line number and add "col_amount" to the column
 * position.
 * "spaces_removed" is the number of spaces that were removed, matters when the
 * cursor is inside them.
 */
    خالی
mark_ستون_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    طویل	lnum_amount,
    طویل	ستون_amount,
    صحیح		spaces_removed)
{
    صحیح		i;
    صحیح		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    اگر ((ستون_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	واپس; // nothing to do

    // named marks, lower case and upper case
    برائے (i = 0; i < NMARKS; i++)
    {
	ستون_adjust(&(curbuf->b_namedm[i]));
	اگر (namedfm[i].fmark.fnum == fnum)
	    ستون_adjust(&(namedfm[i].fmark.mark));
    }
    برائے (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	اگر (namedfm[i].fmark.fnum == fnum)
	    ستون_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    ستون_adjust(&(curbuf->b_آخری_داخل_کرو));

    // last change position
    ستون_adjust(&(curbuf->b_آخری_change));

    // list of change positions
    برائے (i = 0; i < curbuf->b_changelistlen; ++i)
	ستون_adjust(&(curbuf->b_changelist[i]));

    // Visual area
    ستون_adjust(&(curbuf->b_visual.vi_آغاز));
    ستون_adjust(&(curbuf->b_visual.vi_انتہا));

    // previous context mark
    ستون_adjust(&(curwin->w_pcmark));

    // previous pcmark
    ستون_adjust(&(curwin->w_پچھلا_pcmark));

    // saved cursor for formatting
    ستون_adjust(&saved_cursor);

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_WINDOWS(win)
    {
	// marks in the jumplist
	برائے (i = 0; i < win->w_jumplistlen; ++i)
	    اگر (win->w_jumplist[i].fmark.fnum == fnum)
		ستون_adjust(&(win->w_jumplist[i].fmark.mark));

	اگر (win->w_بفر == curbuf)
	{
	    // marks in the tag stack
	    برائے (i = 0; i < win->w_tagstacklen; i++)
		اگر (win->w_tagstack[i].fmark.fnum == fnum)
		    ستون_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    اگر (win != curwin)
		ستون_adjust(&win->w_cursor);
	}
    }
}

/*
 * When deleting lines, this may create duplicate marks in the
 * jumplist. They will be removed here for the specified window.
 * When "loadfiles" is TRUE first ensure entries have the "fnum" field set
 * (this may be a bit slow).
 */
    خالی
cleanup_jumplist(win_T *wp, صحیح loadfiles)
{
    صحیح	    i;
    صحیح	    from, مقصد;
    صحیح	    mustfree;

    اگر (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	برائے (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    اگر ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    مقصد = 0;
    برائے (from = 0; from < wp->w_jumplistlen; ++from)
    {
	اگر (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = مقصد;
	برائے (i = from + 1; i < wp->w_jumplistlen; ++i)
	    اگر (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		توڑو;
	اگر (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	ورنہ اگر (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_ڈھیر);
	ورنہ				// adjacent duplicate
	    mustfree = TRUE;

	اگر (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	ورنہ
	    wp->w_jumplist[مقصد++] = wp->w_jumplist[from];
    }
    اگر (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = مقصد;
    wp->w_jumplistlen = مقصد;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    خالی
copy_jumplist(win_T *from, win_T *مقصد)
{
    صحیح		i;

    برائے (i = 0; i < from->w_jumplistlen; ++i)
    {
	مقصد->w_jumplist[i] = from->w_jumplist[i];
	اگر (from->w_jumplist[i].fname != تہی)
	    مقصد->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    مقصد->w_jumplistlen = from->w_jumplistlen;
    مقصد->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    خالی
free_jumplist(win_T *wp)
{
    صحیح		i;

    برائے (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    خالی
سیٹ_آخری_cursor(win_T *win)
{
    اگر (win->w_بفر != تہی)
	win->w_بفر->b_آخری_cursor = win->w_cursor;
}

#اگر defined(EXITFREE)
    خالی
free_all_marks(خالی)
{
    صحیح		i;

    برائے (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	اگر (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#اگر_ختم

#اگر defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(خالی)
{
    واپس namedfm;
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    ساکن صحیح
جوڑو_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, صحیح bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    اگر (pos->lnum <= 0)
	واپس OK;

    d = dict_alloc();
    اگر (d == تہی)
	واپس FAIL;

    اگر (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	واپس FAIL;
    }

    lpos = list_alloc();
    اگر (lpos == تہی)
	واپس FAIL;

    list_append_نمبر(lpos, bufnr);
    list_append_نمبر(lpos, pos->lnum);
    list_append_نمبر(lpos, pos->ستون < MAXCOL ? pos->ستون + 1 : MAXCOL);
    list_append_نمبر(lpos, pos->coladd);

    اگر (dict_جوڑو_string_len(d, "mark", mname, (صحیح)mnamelen) == FAIL
	    || dict_جوڑو_list(d, "pos", lpos) == FAIL
	    || (fname != تہی && dict_جوڑو_string(d, "file", fname) == FAIL))
    {
	اگر (lpos->lv_refcount == 0)
	    list_free(lpos);
	واپس FAIL;
    }

    واپس OK;
}

/*
 * Get information about marks local to a buffer.
 */
    ساکن خالی
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    صحیح		i;

    // Marks 'a' to 'z'
    برائے (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	جوڑو_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, تہی);
    }

    // Mark '' is a window local mark and not a buffer local mark
    جوڑو_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, تہی);

    جوڑو_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_آخری_cursor, buf->b_fnum, تہی);
    جوڑو_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_آغاز, buf->b_fnum, تہی);
    جوڑو_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_انتہا, buf->b_fnum, تہی);
    جوڑو_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_آخری_داخل_کرو, buf->b_fnum, تہی);
    جوڑو_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_آخری_change, buf->b_fnum, تہی);
    جوڑو_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_آغاز, buf->b_fnum, تہی);
    جوڑو_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_انتہا, buf->b_fnum, تہی);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    ساکن خالی
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    صحیح		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    برائے (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	اگر (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	ورنہ
	    name = namedfm[i].fname;
	اگر (name != تہی)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    جوڑو_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    اگر (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    خالی
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = تہی;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_بفر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	واپس;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == تہی)
	واپس;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#اگر_ختم
