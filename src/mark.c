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

#అన్తర్భావ "vim.h"

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
స్థిర xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

స్థిర శూన్య fname2fnum(xfmark_T *fm);
స్థిర శూన్య fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
స్థిర char_u *mark_పంక్తి(pos_T *mp, పూర్ణాంకం lead_len);
స్థిర శూన్య show_one_mark(పూర్ణాంకం, char_u *, pos_T *, char_u *, పూర్ణాంకం current);
స్థిర శూన్య mark_adjust_అంతర్గత(linenr_T line1, linenr_T line2, దీర్ఘ amount,
    దీర్ఘ amount_after, పూర్ణాంకం adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    పూర్ణాంకం
setmark(పూర్ణాంకం c)
{
    తిరిగి setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    పూర్ణాంకం
setmark_pos(పూర్ణాంకం c, pos_T *pos, పూర్ణాంకం fnum)
{
    పూర్ణాంకం		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    ఒకవేళ (c < 0)
	తిరిగి FAIL;

    ఒకవేళ (c == '\'' || c == '`')
    {
	ఒకవేళ (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	లేకపోతే
	    curwin->w_pcmark = *pos;
	తిరిగి OK;
    }

    buf = buflist_findnr(fnum);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (c == '"')
    {
	buf->b_చివరి_cursor = *pos;
	తిరిగి OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    ఒకవేళ (c == '[')
    {
	buf->b_op_ప్రారంభం = *pos;
	తిరిగి OK;
    }
    ఒకవేళ (c == ']')
    {
	buf->b_op_చివర = *pos;
	తిరిగి OK;
    }

    ఒకవేళ (c == '<' || c == '>')
    {
	ఒకవేళ (c == '<')
	    buf->b_visual.vi_ప్రారంభం = *pos;
	లేకపోతే
	    buf->b_visual.vi_చివర = *pos;
	ఒకవేళ (buf->b_visual.vi_బహుళకం == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_బహుళకం = 'v';
	తిరిగి OK;
    }

    ఒకవేళ (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	తిరిగి OK;
    }
    ఒకవేళ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	ఒకవేళ (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	లేకపోతే
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#నిర్వచితమైతే FEAT_VIMINFO
	namedfm[i].సమయం_సమితి = vim_సమయం();
#అయితే_ముగింపు
	తిరిగి OK;
    }
    తిరిగి FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    శూన్య
mark_forget_file(win_T *wp, పూర్ణాంకం fnum)
{
    పూర్ణాంకం		i;

    కొరకు (i = wp->w_jumplistlen - 1; i >= 0; --i)
	ఒకవేళ (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    ఒకవేళ (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * పరిమాణం(wp->w_jumplist[i]));
	}

    కొరకు (i = wp->w_tagstacklen - 1; i >= 0; --i)
	ఒకవేళ (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_నమోదు(&wp->w_tagstack[i]);
	    ఒకవేళ (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * పరిమాణం(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    శూన్య
setpcmark(శూన్య)
{
    పూర్ణాంకం		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    ఒకవేళ (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	తిరిగి;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    ఒకవేళ (jop_flags & JOP_దొంతర)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	ఒకవేళ (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    ఒకవేళ (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	కొరకు (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = ఖాళీ;
#నిర్వచితమైతే FEAT_VIMINFO
    fm->సమయం_సమితి = vim_సమయం();
#అయితే_ముగింపు
}

/*
 * To change context, call setpcmark(), then move the current position to
 * where ever, then call checkpcmark().  This ensures that the previous
 * context will only be changed if the cursor moved to a different line.
 * If pcmark was deleted (with "dG") the previous mark is restored.
 */
    శూన్య
checkpcmark(శూన్య)
{
    ఒకవేళ (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(పూర్ణాంకం లెక్క)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    ఒకవేళ (curwin->w_jumplistlen == 0)	    // nothing to jump to
	తిరిగి (pos_T *)ఖాళీ;

    కొరకు (;;)
    {
	ఒకవేళ (curwin->w_jumplistidx + లెక్క < 0
		|| curwin->w_jumplistidx + లెక్క >= curwin->w_jumplistlen)
	    తిరిగి (pos_T *)ఖాళీ;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	ఒకవేళ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    ఒకవేళ (curwin->w_jumplistidx + లెక్క < 0)
		తిరిగి (pos_T *)ఖాళీ;
	}

	curwin->w_jumplistidx += లెక్క;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	ఒకవేళ (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	ఒకవేళ (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    ఒకవేళ (buflist_findnr(fmark.fnum) == ఖాళీ)
	    {					     // Skip this one ..
		లెక్క += లెక్క < 0 ? -1 : 1;
		కొనసాగు;
	    }
	    ఒకవేళ (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		తిరిగి (pos_T *)ఖాళీ;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	లేకపోతే
	    pos = &(jmp->fmark.mark);
	తిరిగి pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(పూర్ణాంకం లెక్క)
{
    పూర్ణాంకం		n;

    ఒకవేళ (curbuf->b_changelistlen == 0)	    // nothing to jump to
	తిరిగి (pos_T *)ఖాళీ;

    n = curwin->w_changelistidx;
    ఒకవేళ (n + లెక్క < 0)
    {
	ఒకవేళ (n == 0)
	    తిరిగి (pos_T *)ఖాళీ;
	n = 0;
    }
    లేకపోతే ఒకవేళ (n + లెక్క >= curbuf->b_changelistlen)
    {
	ఒకవేళ (n == curbuf->b_changelistlen - 1)
	    తిరిగి (pos_T *)ఖాళీ;
	n = curbuf->b_changelistlen - 1;
    }
    లేకపోతే
	n += లెక్క;
    curwin->w_changelistidx = n;
    తిరిగి curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, పూర్ణాంకం c, పూర్ణాంకం changefile)
{
    తిరిగి getmark_buf_fnum(buf, c, changefile, ఖాళీ);
}

    pos_T *
getmark(పూర్ణాంకం c, పూర్ణాంకం changefile)
{
    తిరిగి getmark_buf_fnum(curbuf, c, changefile, ఖాళీ);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    పూర్ణాంకం		c,
    పూర్ణాంకం		changefile,
    పూర్ణాంకం		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    స్థిర pos_T	pos_copy;

    posp = ఖాళీ;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    ఒకవేళ (c < 0)
	తిరిగి posp;
    ఒకవేళ (c > '~')			// check for islower()/isupper()
	;
    లేకపోతే ఒకవేళ (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    లేకపోతే ఒకవేళ (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_చివరి_cursor);
    లేకపోతే ఒకవేళ (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_చివరి_చొప్పించు);
    లేకపోతే ఒకవేళ (c == '.')			// to where last change was made
	posp = &(buf->b_చివరి_change);
    లేకపోతే ఒకవేళ (c == '[')			// to start of previous operator
	posp = &(buf->b_op_ప్రారంభం);
    లేకపోతే ఒకవేళ (c == ']')			// to end of previous operator
	posp = &(buf->b_op_చివర);
    లేకపోతే ఒకవేళ (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	పూర్ణాంకం	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ఒకవేళ (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    లేకపోతే ఒకవేళ (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	పూర్ణాంకం	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ఒకవేళ (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    లేకపోతే ఒకవేళ (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_ప్రారంభం;
	endp = &buf->b_visual.vi_చివర;
	ఒకవేళ (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	లేకపోతే
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	ఒకవేళ (buf->b_visual.vi_బహుళకం == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    ఒకవేళ (c == '<')
		pos_copy.నిలువు = 0;
	    లేకపోతే
		pos_copy.నిలువు = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    లేకపోతే ఒకవేళ (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    లేకపోతే ఒకవేళ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	ఒకవేళ (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	లేకపోతే
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	ఒకవేళ (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	ఒకవేళ (fnum != ఖాళీ)
	    *fnum = namedfm[c].fmark.fnum;
	లేకపోతే ఒకవేళ (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    ఒకవేళ (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		ఒకవేళ (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    తిరిగి (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    లేకపోతే
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    తిరిగి posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    పూర్ణాంకం		dir,	// direction for search
    పూర్ణాంకం		begin_పంక్తి)
{
    పూర్ణాంకం		i;
    pos_T	*ఫలితం = ఖాళీ;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    ఒకవేళ (dir == BACKWARD && begin_పంక్తి)
	pos.నిలువు = 0;
    లేకపోతే ఒకవేళ (dir == FORWARD && begin_పంక్తి)
	pos.నిలువు = MAXCOL;

    కొరకు (i = 0; i < NMARKS; i++)
    {
	ఒకవేళ (curbuf->b_namedm[i].lnum > 0)
	{
	    ఒకవేళ (dir == FORWARD)
	    {
		ఒకవేళ ((ఫలితం == ఖాళీ || LT_POS(curbuf->b_namedm[i], *ఫలితం))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    ఫలితం = &curbuf->b_namedm[i];
	    }
	    లేకపోతే
	    {
		ఒకవేళ ((ఫలితం == ఖాళీ || LT_POS(*ఫలితం, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    ఫలితం = &curbuf->b_namedm[i];
	    }
	}
    }

    తిరిగి ఫలితం;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    స్థిర శూన్య
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    ఒకవేళ (fm->fname == ఖాళీ)
	తిరిగి;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    ఒకవేళ (fm->fname[0] == '~' && (fm->fname[1] == '/'
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
		|| fm->fname[1] == '\\'
#అయితే_ముగింపు
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    లేకపోతే
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (శూన్య)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    శూన్య
fmarks_check_పేర్లు(buf_T *buf)
{
    char_u	*name;
    పూర్ణాంకం		i;
    win_T	*wp;

    ఒకవేళ (buf->b_ffname == ఖాళీ)
	తిరిగి;

    name = home_replace_save(buf, buf->b_ffname);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి;

    కొరకు (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	కొరకు (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    స్థిర శూన్య
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    ఒకవేళ (fm->fmark.fnum == 0
	    && fm->fname != ఖాళీ
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
    పూర్ణాంకం
check_mark(pos_T *pos)
{
    ఒకవేళ (pos == ఖాళీ)
    {
	emsg(_(e_unknown_mark));
	తిరిగి FAIL;
    }
    ఒకవేళ (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	ఒకవేళ (pos->lnum == 0)
	    emsg(_(e_mark_not_సమితి));
	తిరిగి FAIL;
    }
    ఒకవేళ (pos->lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	emsg(_(e_mark_has_invalid_పంక్తి_సంఖ్య));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    శూన్య
clrallmarks(buf_T *buf)
{
    స్థిర పూర్ణాంకం		i = -1;

    ఒకవేళ (i == -1)	// first call ever: initialize
	కొరకు (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = ఖాళీ;
#నిర్వచితమైతే FEAT_VIMINFO
	    namedfm[i].సమయం_సమితి = 0;
#అయితే_ముగింపు
	}

    కొరకు (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_ప్రారంభం.lnum = 0;		// start/end op mark cleared
    buf->b_op_చివర.lnum = 0;
    buf->b_చివరి_cursor.lnum = 1;	// '" mark cleared
    buf->b_చివరి_cursor.నిలువు = 0;
    buf->b_చివరి_cursor.coladd = 0;
    buf->b_చివరి_చొప్పించు.lnum = 0;	// '^ mark cleared
    buf->b_చివరి_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, పూర్ణాంకం lead_len)
{
    ఒకవేళ (fmark->fnum == curbuf->b_fnum)		    // current buffer
	తిరిగి mark_పంక్తి(&(fmark->mark), lead_len);
    తిరిగి buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    స్థిర char_u *
mark_పంక్తి(pos_T *mp, పూర్ణాంకం lead_len)
{
    char_u	*s, *p;
    పూర్ణాంకం		len;

    ఒకవేళ (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    ఒకవేళ (s == ఖాళీ)
	తిరిగి ఖాళీ;
    // Truncate the line to fit it in the window.
    len = 0;
    కొరకు (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	ఒకవేళ (len >= Columns - lead_len)
	    ఆపు;
    }
    *p = NUL;
    తిరిగి s;
}

/*
 * print the marks
 */
    శూన్య
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    పూర్ణాంకం		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    ఒకవేళ (arg != ఖాళీ && *arg == NUL)
	arg = ఖాళీ;

    show_one_mark('\'', arg, &curwin->w_pcmark, ఖాళీ, TRUE);
    కొరకు (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], ఖాళీ, TRUE);
    కొరకు (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ఒకవేళ (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	లేకపోతే
	    name = namedfm[i].fname;
	ఒకవేళ (name != ఖాళీ)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    ఒకవేళ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_చివరి_cursor, ఖాళీ, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_ప్రారంభం, ఖాళీ, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_చివర, ఖాళీ, TRUE);
    show_one_mark('^', arg, &curbuf->b_చివరి_చొప్పించు, ఖాళీ, TRUE);
    show_one_mark('.', arg, &curbuf->b_చివరి_change, ఖాళీ, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_ప్రారంభం;
    endp = &curbuf->b_visual.vi_చివర;
    ఒకవేళ ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    లేకపోతే
	posp = endp;
    show_one_mark('<', arg, posp, ఖాళీ, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, ఖాళీ, TRUE);

    show_one_mark(-1, arg, ఖాళీ, ఖాళీ, FALSE);
}

    స్థిర శూన్య
show_one_mark(
    పూర్ణాంకం		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    పూర్ణాంకం		current)	// in current file
{
    స్థిర పూర్ణాంకం	did_title = FALSE;
    పూర్ణాంకం		mustfree = FALSE;
    char_u	*name = name_arg;

    ఒకవేళ (c == -1)			    // finish up
    {
	ఒకవేళ (did_title)
	    did_title = FALSE;
	లేకపోతే
	{
	    ఒకవేళ (arg == ఖాళీ)
		సందేశం(_("No marks set"));
	    లేకపోతే
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    లేకపోతే ఒకవేళ (!got_int
	    && (arg == ఖాళీ || vim_strchr(arg, c) != ఖాళీ)
	    && p->lnum != 0)
    {
	ఒకవేళ (name == ఖాళీ && current)
	{
	    name = mark_పంక్తి(p, 15);
	    ఒకవేళ (name == ఖాళీ)
	    {
		emsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం));
		తిరిగి;
	    }
	    mustfree = TRUE;
	}
	ఒకవేళ (!సందేశం_filtered(name))
	{
	    ఒకవేళ (!did_title)
	    {
		// Highlight title
		సందేశం_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    సందేశం_putchar('\n');
	    ఒకవేళ (!got_int)
	    {
		sprintf((అక్షరం *)IObuff, " %c %6ld %4d ", c, p->lnum, p->నిలువు);
		సందేశం_outtrans(IObuff);
		ఒకవేళ (name != ఖాళీ)
		{
		    సందేశం_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    బహిర్గతం_flush();		    // show one line at a time
	}
	ఒకవేళ (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    శూన్య
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    పూర్ణాంకం		from, గమ్యం;
    పూర్ణాంకం		i;
    పూర్ణాంకం		క్రింది;
    పూర్ణాంకం		digit;
    పూర్ణాంకం		n;

    ఒకవేళ (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    లేకపోతే ఒకవేళ (eap->forceit)
	emsg(_(e_invalid_వాదం));
    లేకపోతే ఒకవేళ (*eap->arg == NUL)
	emsg(_(e_వాదం_required));
    లేకపోతే
    {
	// clear specified marks only
	కొరకు (p = eap->arg; *p != NUL; ++p)
	{
	    క్రింది = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    ఒకవేళ (క్రింది || digit || ASCII_ISUPPER(*p))
	    {
		ఒకవేళ (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    గమ్యం = p[2];
		    ఒకవేళ (!(క్రింది ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || గమ్యం < from)
		    {
			semsg(_(e_invalid_వాదం_str), p);
			తిరిగి;
		    }
		    p += 2;
		}
		లేకపోతే
		    // clear one lower case mark
		    from = గమ్యం = *p;

		కొరకు (i = from; i <= గమ్యం; ++i)
		{
		    ఒకవేళ (క్రింది)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    లేకపోతే
		    {
			ఒకవేళ (digit)
			    n = i - '0' + NMARKS;
			లేకపోతే
			    n = i - 'A';
			namedfm[n].fmark.mark.lnum = 0;
			namedfm[n].fmark.fnum = 0;
			VIM_CLEAR(namedfm[n].fname);
#నిర్వచితమైతే FEAT_VIMINFO
			namedfm[n].సమయం_సమితి = digit ? 0 : vim_సమయం();
#అయితే_ముగింపు
		    }
		}
	    }
	    లేకపోతే
		ఎంపిక (*p)
		{
		    స్థితి '"': curbuf->b_చివరి_cursor.lnum = 0; ఆపు;
		    స్థితి '^': curbuf->b_చివరి_చొప్పించు.lnum = 0; ఆపు;
		    స్థితి '.': curbuf->b_చివరి_change.lnum = 0; ఆపు;
		    స్థితి '[': curbuf->b_op_ప్రారంభం.lnum    = 0; ఆపు;
		    స్థితి ']': curbuf->b_op_చివర.lnum      = 0; ఆపు;
		    స్థితి '<': curbuf->b_visual.vi_ప్రారంభం.lnum = 0; ఆపు;
		    స్థితి '>': curbuf->b_visual.vi_చివర.lnum   = 0; ఆపు;
		    స్థితి ' ': ఆపు;
		    అప్రమేయ:  semsg(_(e_invalid_వాదం_str), p);
			      తిరిగి;
		}
	}
    }
}

/*
 * print the jumplist
 */
    శూన్య
ex_jumps(exarg_T *eap వాడని)
{
    పూర్ణాంకం		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    సందేశం_puts_title(_("\n jump line  col file/text"));
    కొరకు (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	ఒకవేళ (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    ఒకవేళ (name == ఖాళీ && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    ఒకవేళ (name == ఖాళీ || సందేశం_filtered(name))
	    {
		vim_free(name);
		కొనసాగు;
	    }

	    సందేశం_putchar('\n');
	    ఒకవేళ (got_int)
	    {
		vim_free(name);
		ఆపు;
	    }
	    sprintf((అక్షరం *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.నిలువు);
	    సందేశం_outtrans(IObuff);
	    సందేశం_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	బహిర్గతం_flush();
    }
    ఒకవేళ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	సందేశం_puts("\n>");
}

    శూన్య
ex_clearjumps(exarg_T *eap వాడని)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    శూన్య
ex_changes(exarg_T *eap వాడని)
{
    పూర్ణాంకం		i;
    char_u	*name;

    // Highlight title
    సందేశం_puts_title(_("\nchange line  col text"));

    కొరకు (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	ఒకవేళ (curbuf->b_changelist[i].lnum != 0)
	{
	    సందేశం_putchar('\n');
	    ఒకవేళ (got_int)
		ఆపు;
	    sprintf((అక్షరం *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (దీర్ఘ)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].నిలువు);
	    సందేశం_outtrans(IObuff);
	    name = mark_పంక్తి(&curbuf->b_changelist[i], 17);
	    ఒకవేళ (name == ఖాళీ)
		ఆపు;
	    సందేశం_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	బహిర్గతం_flush();
    }
    ఒకవేళ (curwin->w_changelistidx == curbuf->b_changelistlen)
	సందేశం_puts("\n>");
}

#నిర్వచించు one_adjust(కూడు) \
    { \
	lp = కూడు; \
	ఒకవేళ (*lp >= line1 && *lp <= line2) \
	{ \
	    ఒకవేళ (amount == MAXLNUM) \
		*lp = 0; \
	    లేకపోతే \
		*lp += amount; \
	} \
	లేకపోతే ఒకవేళ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#నిర్వచించు one_adjust_nodel(కూడు) \
    { \
	lp = కూడు; \
	ఒకవేళ (*lp >= line1 && *lp <= line2) \
	{ \
	    ఒకవేళ (amount == MAXLNUM) \
		*lp = line1; \
	    లేకపోతే \
		*lp += amount; \
	} \
	లేకపోతే ఒకవేళ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#నిర్వచించు one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	ఒకవేళ (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    ఒకవేళ (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->నిలువు = 0; \
	    } \
	    లేకపోతే /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	లేకపోతే ఒకవేళ (amount_after && posp->lnum > line2) \
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
    శూన్య
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    దీర్ఘ	amount,
    దీర్ఘ	amount_after)
{
    mark_adjust_అంతర్గత(line1, line2, amount, amount_after, TRUE);
}

    శూన్య
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    దీర్ఘ	amount,
    దీర్ఘ	amount_after)
{
    mark_adjust_అంతర్గత(line1, line2, amount, amount_after, FALSE);
}

    స్థిర శూన్య
mark_adjust_అంతర్గత(
    linenr_T	line1,
    linenr_T	line2,
    దీర్ఘ	amount,
    దీర్ఘ	amount_after,
    పూర్ణాంకం		adjust_folds వాడని)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    స్థిర pos_T initpos = {1, 0, 0};

    ఒకవేళ (line2 < line1 && amount_after == 0L)	    // nothing to do
	తిరిగి;

    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	కొరకు (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    ఒకవేళ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	కొరకు (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    ఒకవేళ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_చివరి_చొప్పించు.lnum));

	// last change position
	one_adjust(&(curbuf->b_చివరి_change.lnum));

	// last cursor position, if it was set
	ఒకవేళ (!EQUAL_POS(curbuf->b_చివరి_cursor, initpos))
	    one_adjust(&(curbuf->b_చివరి_cursor.lnum));


	// list of change positions
	కొరకు (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_ప్రారంభం.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_చివర.lnum));

#నిర్వచితమైతే FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(ఖాళీ, line1, line2, amount, amount_after);
	// location lists
	FOR_ALL_TAB_WINDOWS(tab, win)
	    qf_mark_adjust(win, line1, line2, amount, amount_after);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SIGNS
	sign_mark_adjust(line1, line2, amount, amount_after);
#అయితే_ముగింపు
    }

    // previous context mark
    one_adjust(&(curwin->w_pcmark.lnum));

    // previous pcmark
    one_adjust(&(curwin->w_prev_pcmark.lnum));

    // saved cursor for formatting
    ఒకవేళ (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    కొరకు (i = 0; i < win->w_jumplistlen; ++i)
		ఒకవేళ (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	ఒకవేళ (win->w_బఫరు == curbuf)
	{
	    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		కొరకు (i = 0; i < win->w_tagstacklen; i++)
		    ఒకవేళ (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    ఒకవేళ (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    ఒకవేళ (win != curwin)
	    {
		ఒకవేళ (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    ఒకవేళ (amount == MAXLNUM)	    // topline is deleted
		    {
			ఒకవేళ (line1 <= 1)
			    win->w_topline = 1;
			లేకపోతే
			    win->w_topline = line1 - 1;
		    }
		    లేకపోతే ఒకవేళ (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#నిర్వచితమైతే FEAT_DIFF
		    win->w_topfill = 0;
#అయితే_ముగింపు
		}
		లేకపోతే ఒకవేళ (amount_after && win->w_topline > line2)
		{
		    win->w_topline += amount_after;
#నిర్వచితమైతే FEAT_DIFF
		    win->w_topfill = 0;
#అయితే_ముగింపు
		}
		one_adjust_cursor(&(win->w_cursor));
	    }

#నిర్వచితమైతే FEAT_FOLDING
	    // adjust folds
	    ఒకవేళ (adjust_folds)
		foldMarkAdjust(win, line1, line2, amount, amount_after);
#అయితే_ముగింపు
	}
    }

#నిర్వచితమైతే FEAT_DIFF
    // adjust diffs
    diff_mark_adjust(line1, line2, amount, amount_after);
#అయితే_ముగింపు

    // adjust per-window "last cursor" positions
    FOR_ALL_BUF_WININFO(curbuf, wip)
	one_adjust_cursor(&(wip->wi_fpos));
}

// This code is used often, needs to be fast.
#నిర్వచించు నిలువు_adjust(pp) \
    { \
	posp = pp; \
	ఒకవేళ (posp->lnum == lnum && posp->నిలువు >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    ఒకవేళ (నిలువు_amount < 0 && posp->నిలువు <= (colnr_T)-నిలువు_amount) \
		posp->నిలువు = 0; \
	    లేకపోతే ఒకవేళ (posp->నిలువు < spaces_removed) \
		posp->నిలువు = నిలువు_amount + spaces_removed; \
	    లేకపోతే \
		posp->నిలువు += నిలువు_amount; \
	} \
    }

/*
 * Adjust marks in line "lnum" at column "mincol" and further: add
 * "lnum_amount" to the line number and add "col_amount" to the column
 * position.
 * "spaces_removed" is the number of spaces that were removed, matters when the
 * cursor is inside them.
 */
    శూన్య
mark_నిలువు_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    దీర్ఘ	lnum_amount,
    దీర్ఘ	నిలువు_amount,
    పూర్ణాంకం		spaces_removed)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    ఒకవేళ ((నిలువు_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	తిరిగి; // nothing to do

    // named marks, lower case and upper case
    కొరకు (i = 0; i < NMARKS; i++)
    {
	నిలువు_adjust(&(curbuf->b_namedm[i]));
	ఒకవేళ (namedfm[i].fmark.fnum == fnum)
	    నిలువు_adjust(&(namedfm[i].fmark.mark));
    }
    కొరకు (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	ఒకవేళ (namedfm[i].fmark.fnum == fnum)
	    నిలువు_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    నిలువు_adjust(&(curbuf->b_చివరి_చొప్పించు));

    // last change position
    నిలువు_adjust(&(curbuf->b_చివరి_change));

    // list of change positions
    కొరకు (i = 0; i < curbuf->b_changelistlen; ++i)
	నిలువు_adjust(&(curbuf->b_changelist[i]));

    // Visual area
    నిలువు_adjust(&(curbuf->b_visual.vi_ప్రారంభం));
    నిలువు_adjust(&(curbuf->b_visual.vi_చివర));

    // previous context mark
    నిలువు_adjust(&(curwin->w_pcmark));

    // previous pcmark
    నిలువు_adjust(&(curwin->w_prev_pcmark));

    // saved cursor for formatting
    నిలువు_adjust(&saved_cursor);

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_WINDOWS(win)
    {
	// marks in the jumplist
	కొరకు (i = 0; i < win->w_jumplistlen; ++i)
	    ఒకవేళ (win->w_jumplist[i].fmark.fnum == fnum)
		నిలువు_adjust(&(win->w_jumplist[i].fmark.mark));

	ఒకవేళ (win->w_బఫరు == curbuf)
	{
	    // marks in the tag stack
	    కొరకు (i = 0; i < win->w_tagstacklen; i++)
		ఒకవేళ (win->w_tagstack[i].fmark.fnum == fnum)
		    నిలువు_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    ఒకవేళ (win != curwin)
		నిలువు_adjust(&win->w_cursor);
	}
    }
}

/*
 * When deleting lines, this may create duplicate marks in the
 * jumplist. They will be removed here for the specified window.
 * When "loadfiles" is TRUE first ensure entries have the "fnum" field set
 * (this may be a bit slow).
 */
    శూన్య
cleanup_jumplist(win_T *wp, పూర్ణాంకం loadfiles)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    from, గమ్యం;
    పూర్ణాంకం	    mustfree;

    ఒకవేళ (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	కొరకు (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    ఒకవేళ ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    గమ్యం = 0;
    కొరకు (from = 0; from < wp->w_jumplistlen; ++from)
    {
	ఒకవేళ (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = గమ్యం;
	కొరకు (i = from + 1; i < wp->w_jumplistlen; ++i)
	    ఒకవేళ (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		ఆపు;
	ఒకవేళ (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	లేకపోతే ఒకవేళ (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_దొంతర);
	లేకపోతే				// adjacent duplicate
	    mustfree = TRUE;

	ఒకవేళ (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	లేకపోతే
	    wp->w_jumplist[గమ్యం++] = wp->w_jumplist[from];
    }
    ఒకవేళ (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = గమ్యం;
    wp->w_jumplistlen = గమ్యం;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    శూన్య
copy_jumplist(win_T *from, win_T *గమ్యం)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < from->w_jumplistlen; ++i)
    {
	గమ్యం->w_jumplist[i] = from->w_jumplist[i];
	ఒకవేళ (from->w_jumplist[i].fname != ఖాళీ)
	    గమ్యం->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    గమ్యం->w_jumplistlen = from->w_jumplistlen;
    గమ్యం->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    శూన్య
free_jumplist(win_T *wp)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    శూన్య
సమితి_చివరి_cursor(win_T *win)
{
    ఒకవేళ (win->w_బఫరు != ఖాళీ)
	win->w_బఫరు->b_చివరి_cursor = win->w_cursor;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_all_marks(శూన్య)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	ఒకవేళ (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(శూన్య)
{
    తిరిగి namedfm;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    స్థిర పూర్ణాంకం
కూడు_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, పూర్ణాంకం bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    ఒకవేళ (pos->lnum <= 0)
	తిరిగి OK;

    d = dict_alloc();
    ఒకవేళ (d == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	తిరిగి FAIL;
    }

    lpos = list_alloc();
    ఒకవేళ (lpos == ఖాళీ)
	తిరిగి FAIL;

    list_append_సంఖ్య(lpos, bufnr);
    list_append_సంఖ్య(lpos, pos->lnum);
    list_append_సంఖ్య(lpos, pos->నిలువు < MAXCOL ? pos->నిలువు + 1 : MAXCOL);
    list_append_సంఖ్య(lpos, pos->coladd);

    ఒకవేళ (dict_కూడు_string_len(d, "mark", mname, (పూర్ణాంకం)mnamelen) == FAIL
	    || dict_కూడు_list(d, "pos", lpos) == FAIL
	    || (fname != ఖాళీ && dict_కూడు_string(d, "file", fname) == FAIL))
    {
	ఒకవేళ (lpos->lv_refcount == 0)
	    list_free(lpos);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Get information about marks local to a buffer.
 */
    స్థిర శూన్య
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    పూర్ణాంకం		i;

    // Marks 'a' to 'z'
    కొరకు (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	కూడు_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, ఖాళీ);
    }

    // Mark '' is a window local mark and not a buffer local mark
    కూడు_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, ఖాళీ);

    కూడు_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_చివరి_cursor, buf->b_fnum, ఖాళీ);
    కూడు_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_ప్రారంభం, buf->b_fnum, ఖాళీ);
    కూడు_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_చివర, buf->b_fnum, ఖాళీ);
    కూడు_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_చివరి_చొప్పించు, buf->b_fnum, ఖాళీ);
    కూడు_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_చివరి_change, buf->b_fnum, ఖాళీ);
    కూడు_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_ప్రారంభం, buf->b_fnum, ఖాళీ);
    కూడు_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_చివర, buf->b_fnum, ఖాళీ);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    స్థిర శూన్య
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    పూర్ణాంకం		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    కొరకు (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ఒకవేళ (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	లేకపోతే
	    name = namedfm[i].fname;
	ఒకవేళ (name != ఖాళీ)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    కూడు_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    ఒకవేళ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    శూన్య
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ఖాళీ;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	తిరిగి;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#అయితే_ముగింపు
