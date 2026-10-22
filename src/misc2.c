/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * misc2.c: Various functions.
 */
#అన్తర్భావ "vim.h"

స్థిర char_u	*username = ఖాళీ; // cached result of mch_get_user_name()

స్థిర పూర్ణాంకం coladvance2(pos_T *pos, పూర్ణాంకం addspaces, పూర్ణాంకం finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    పూర్ణాంకం
virtual_చురుకైన(శూన్య)
{
    గుర్తులేని పూర్ణాంకం cur_ve_flags = get_ve_flags();

    ఒకవేళ (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_చొప్పించు) && (State & బహుళకం_చొప్పించు)))
	తిరిగి TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    ఒకవేళ (virtual_op != MAYBE)
	తిరిగి virtual_op;
    తిరిగి (cur_ve_flags & VE_ఖండం) && VIsual_చురుకైన && VIsual_బహుళకం == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    పూర్ణాంకం
getviscol(శూన్య)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, ఖాళీ, ఖాళీ, GETVCOL_NO_ABOVE);
    తిరిగి (పూర్ణాంకం)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    పూర్ణాంకం
coladvance_force(colnr_T wcol)
{
    పూర్ణాంకం rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    ఒకవేళ (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    లేకపోతే
	// Virtcol is valid
	సమితి_valid_virtcol(curwin, wcol);
    తిరిగి rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    పూర్ణాంకం
getviscol2(colnr_T నిలువు, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.నిలువు = నిలువు;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, ఖాళీ, ఖాళీ, GETVCOL_NO_ABOVE);
    తిరిగి (పూర్ణాంకం)x;
}

/*
 * Try to advance the Cursor to the specified screen column "wantcol".
 * If virtual editing: fine tune the cursor position.
 * Note that all virtual positions off the end of a line should share
 * a curwin->w_cursor.col value (n.b. this is equal to STRLEN(line)),
 * beginning at coladd 0.
 *
 * return OK if desired column is reached, FAIL if not
 */
    పూర్ణాంకం
coladvance(colnr_T wantcol)
{
    పూర్ణాంకం rc = getvpos(&curwin->w_cursor, wantcol);

    ఒకవేళ (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    లేకపోతే ఒకవేళ (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	సమితి_valid_virtcol(curwin, wantcol);
    తిరిగి rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    పూర్ణాంకం
getvpos(pos_T *pos, colnr_T wantcol)
{
    తిరిగి coladvance2(pos, FALSE, virtual_చురుకైన(), wantcol);
}

    స్థిర పూర్ణాంకం
coladvance2(
    pos_T	*pos,
    పూర్ణాంకం		addspaces,	// change the text to achieve our goal?
    పూర్ణాంకం		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    పూర్ణాంకం		idx;
    char_u	*పంక్తి;
    పూర్ణాంకం		linelen;
    colnr_T	నిలువు = 0;
    పూర్ణాంకం		csize = 0;
    పూర్ణాంకం		one_మరిన్ని;
#నిర్వచితమైతే FEAT_LINEBREAK
    పూర్ణాంకం		తల = 0;
#అయితే_ముగింపు

    one_మరిన్ని = (State & బహుళకం_చొప్పించు)
		    || restart_edit != NUL
		    || (VIsual_చురుకైన && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    పంక్తి = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    ఒకవేళ (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_మరిన్ని;
	    నిలువు = wcol;

	    ఒకవేళ ((addspaces || finetune) && !VIsual_చురుకైన)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_మరిన్ని;
		ఒకవేళ (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    లేకపోతే
    {
	పూర్ణాంకం		వెడల్పు = curwin->w_వెడల్పు - win_నిలువు_off(curwin);
	chartabsize_T	cts;

	ఒకవేళ (finetune
		&& curwin->w_p_wrap
		&& curwin->w_వెడల్పు != 0
		&& wcol >= (colnr_T)వెడల్పు
		&& వెడల్పు > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    ఒకవేళ (csize > 0)
		csize--;

	    ఒకవేళ (wcol / వెడల్పు > (colnr_T)csize / వెడల్పు
		    && ((State & బహుళకం_చొప్పించు) == 0 || (పూర్ణాంకం)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / వెడల్పు + 1) * వెడల్పు - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, పంక్తి, పంక్తి);
#నిర్వచితమైతే FEAT_PROP_POPUP
	cts.cts_no_above = true;
#అయితే_ముగింపు
	అయితే (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#నిర్వచితమైతే FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &తల, ఖాళీ);
	    MB_PTR_ADV(cts.cts_ptr);
#లేకపోతే
	    csize = lbr_chartabsize_adv(&cts);
#అయితే_ముగింపు
	    cts.cts_vcol += csize;
	}
	నిలువు = cts.cts_vcol;
	idx = (పూర్ణాంకం)(cts.cts_ptr - పంక్తి);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	ఒకవేళ (నిలువు > wcol || (!virtual_చురుకైన() && one_మరిన్ని == 0))
	{
	    idx -= 1;
#నిర్వచితమైతే FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= తల;
#అయితే_ముగింపు
	    నిలువు -= csize;
	}

	ఒకవేళ (virtual_చురుకైన()
		&& addspaces
		&& wcol >= 0
		&& ((నిలువు != wcol && నిలువు != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    ఒకవేళ (పంక్తి[idx] == NUL)
	    {
		// Append spaces
		పూర్ణాంకం	correct = wcol - నిలువు;
		char_u	*newline = alloc(idx + correct + 1);
		పూర్ణాంకం	t;

		ఒకవేళ (newline == ఖాళీ)
		    తిరిగి FAIL;

		కొరకు (t = 0; t < idx; ++t)
		    newline[t] = పంక్తి[t];

		కొరకు (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		మారింది_బైట్లు(pos->lnum, (colnr_T)idx);
		idx += correct;
		నిలువు = wcol;
	    }
	    లేకపోతే
	    {
		// Break a tab
		పూర్ణాంకం	correct = wcol - నిలువు - csize + 1; // negative!!
		char_u	*newline;
		పూర్ణాంకం	t, s = 0;
		పూర్ణాంకం	v;

		ఒకవేళ (-correct > csize)
		    తిరిగి FAIL;

		newline = alloc(linelen + csize);
		ఒకవేళ (newline == ఖాళీ)
		    తిరిగి FAIL;

		కొరకు (t = 0; t < linelen; t++)
		{
		    ఒకవేళ (t != idx)
			newline[s++] = పంక్తి[t];
		    లేకపోతే
			కొరకు (v = 0; v < csize; v++)
			    newline[s++] = ' ';
		}

		newline[linelen + csize - 1] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		మారింది_బైట్లు(pos->lnum, idx);
		idx += (csize - 1 + correct);
		నిలువు += correct;
	    }
	}
    }

    ఒకవేళ (idx < 0)
	pos->నిలువు = 0;
    లేకపోతే
	pos->నిలువు = idx;

    pos->coladd = 0;

    ఒకవేళ (finetune)
    {
	ఒకవేళ (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    ఒకవేళ (!one_మరిన్ని)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, ఖాళీ, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	లేకపోతే
	{
	    పూర్ణాంకం b = (పూర్ణాంకం)wcol - (పూర్ణాంకం)నిలువు;

	    // The difference between wcol and col is used to set coladd.
	    ఒకవేళ (b > 0 && b < (MAXCOL - 2 * curwin->w_వెడల్పు))
		pos->coladd = b;

	    నిలువు += b;
	}
    }

    // prevent from moving onto a trail byte
    ఒకవేళ (has_mbyte)
	mb_adjustpos(curbuf, pos);

    ఒకవేళ (wcol < 0 || నిలువు < wcol)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    పూర్ణాంకం
inc_cursor(శూన్య)
{
    తిరిగి inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    పూర్ణాంకం
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    ఒకవేళ (lp->నిలువు != MAXCOL)
    {
	p = ml_get_pos(lp);
	ఒకవేళ (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    ఒకవేళ (has_mbyte)
	    {
		పూర్ణాంకం l = (*mb_ptr2len)(p);

		lp->నిలువు += l;
		తిరిగి ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->నిలువు++;
	    lp->coladd = 0;
	    తిరిగి ((p[1] != NUL) ? 0 : 2);
	}
    }
    ఒకవేళ (lp->lnum != curbuf->b_ml.ml_పంక్తి_లెక్క)     // there is a next line
    {
	lp->నిలువు = 0;
	lp->lnum++;
	lp->coladd = 0;
	తిరిగి 1;
    }
    తిరిగి -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    పూర్ణాంకం
incl(pos_T *lp)
{
    పూర్ణాంకం	    r;

    ఒకవేళ ((r = inc(lp)) >= 1 && lp->నిలువు)
	r = inc(lp);
    తిరిగి r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    పూర్ణాంకం
dec_cursor(శూన్య)
{
    తిరిగి dec(&curwin->w_cursor);
}

    పూర్ణాంకం
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    ఒకవేళ (lp->నిలువు == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->నిలువు = ml_get_len(lp->lnum);
	ఒకవేళ (has_mbyte)
	    lp->నిలువు -= (*mb_తల_off)(p, p + lp->నిలువు);
	తిరిగి 0;
    }

    ఒకవేళ (lp->నిలువు > 0)
    {
	// still within line
	lp->నిలువు--;
	ఒకవేళ (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->నిలువు -= (*mb_తల_off)(p, p + lp->నిలువు);
	}
	తిరిగి 0;
    }

    ఒకవేళ (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->నిలువు = ml_get_len(lp->lnum);
	ఒకవేళ (has_mbyte)
	    lp->నిలువు -= (*mb_తల_off)(p, p + lp->నిలువు);
	తిరిగి 1;
    }

    // at start of file
    తిరిగి -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    పూర్ణాంకం
decl(pos_T *lp)
{
    పూర్ణాంకం	    r;

    ఒకవేళ ((r = dec(lp)) == 1 && lp->నిలువు)
	r = dec(lp);
    తిరిగి r;
}

/*
 * Get the line number relative to the current cursor position, i.e. the
 * difference between line number and cursor position. Only look for lines that
 * can be visible, folded lines don't count.
 */
    linenr_T
get_cursor_rel_lnum(
    win_T	*wp,
    linenr_T	lnum)		    // line number to get the result for
{
    linenr_T	cursor = wp->w_cursor.lnum;
    linenr_T	retval = 0;

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (hasAnyFolding(wp))
    {
	ఒకవేళ (lnum > cursor)
	{
	    అయితే (lnum > cursor)
	    {
		(శూన్య)hasFoldingWin(wp, lnum, &lnum, ఖాళీ, TRUE, ఖాళీ);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		ఒకవేళ (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	లేకపోతే ఒకవేళ (lnum < cursor)
	{
	    అయితే (lnum < cursor)
	    {
		(శూన్య)hasFoldingWin(wp, lnum, ఖాళీ, &lnum, TRUE, ఖాళీ);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		ఒకవేళ (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    లేకపోతే
#అయితే_ముగింపు
	retval = lnum - cursor;

    తిరిగి retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    శూన్య
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    ఒకవేళ (pos->lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	pos->lnum = buf->b_ml.ml_పంక్తి_లెక్క;

    ఒకవేళ (pos->నిలువు > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	ఒకవేళ (pos->నిలువు > len)
	    pos->నిలువు = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    శూన్య
check_cursor_lnum(శూన్య)
{
    ఒకవేళ (curwin->w_cursor.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
#నిర్వచితమైతే FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	ఒకవేళ (!hasFolding(curbuf->b_ml.ml_పంక్తి_లెక్క,
						&curwin->w_cursor.lnum, ఖాళీ))
#అయితే_ముగింపు
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
    }
    ఒకవేళ (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    శూన్య
check_cursor_నిలువు(శూన్య)
{
    check_cursor_నిలువు_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    శూన్య
check_cursor_నిలువు_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.నిలువు;
    colnr_T      oldcoladd = win->w_cursor.నిలువు + win->w_cursor.coladd;
    గుర్తులేని పూర్ణాంకం cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_బఫరు, win->w_cursor.lnum);
    ఒకవేళ (len == 0)
	win->w_cursor.నిలువు = 0;
    లేకపోతే ఒకవేళ (win->w_cursor.నిలువు >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	ఒకవేళ ((State & బహుళకం_చొప్పించు) || restart_edit
		|| (VIsual_చురుకైన && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_చురుకైన())
	    win->w_cursor.నిలువు = len;
	లేకపోతే
	{
	    win->w_cursor.నిలువు = len - 1;
	    // Move the cursor to the head byte.
	    ఒకవేళ (has_mbyte)
		mb_adjustpos(win->w_బఫరు, &win->w_cursor);
	}
    }
    లేకపోతే ఒకవేళ (win->w_cursor.నిలువు < 0)
	win->w_cursor.నిలువు = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    ఒకవేళ (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    లేకపోతే ఒకవేళ (cur_ve_flags == VE_ALL)
    {
	ఒకవేళ (oldcoladd > win->w_cursor.నిలువు)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.నిలువు;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    ఒకవేళ (win->w_cursor.నిలువు + 1 < len)
	    {
		పూర్ణాంకం cs, ce;

		getvcol(win, &win->w_cursor, &cs, ఖాళీ, &ce, 0);
		ఒకవేళ (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	లేకపోతే
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    శూన్య
check_cursor(శూన్య)
{
    check_cursor_lnum();
    check_cursor_నిలువు();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    శూన్య
check_visual_pos(శూన్య)
{
    ఒకవేళ (VIsual.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	VIsual.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	VIsual.నిలువు = 0;
	VIsual.coladd = 0;
    }
    లేకపోతే
    {
	పూర్ణాంకం len = ml_get_len(VIsual.lnum);

	ఒకవేళ (VIsual.నిలువు > len)
	{
	    VIsual.నిలువు = len;
	    VIsual.coladd = 0;
	}
    }
}

/*
 * Make sure curwin->w_cursor is not on the NUL at the end of the line.
 * Allow it when in Visual mode and 'selection' is not "old".
 */
    శూన్య
adjust_cursor_నిలువు(శూన్య)
{
    ఒకవేళ (curwin->w_cursor.నిలువు > 0
	    && (!VIsual_చురుకైన || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.నిలువు;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    పూర్ణాంకం
సమితి_leftcol(colnr_T leftcol)
{
    పూర్ణాంకం		retval = FALSE;

    // Return quickly when there is no change.
    ఒకవేళ (curwin->w_leftcol == leftcol)
	తిరిగి FALSE;
    curwin->w_leftcol = leftcol;

    మారింది_cline_bef_curs();
    దీర్ఘ lastcol = curwin->w_leftcol + curwin->w_వెడల్పు - curwin_నిలువు_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    దీర్ఘ siso = get_sidescrolloff_విలువ();
    ఒకవేళ (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    లేకపోతే ఒకవేళ (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(శూన్య)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, ఖాళీ, &e, 0);
    ఒకవేళ (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    లేకపోతే ఒకవేళ (s < curwin->w_leftcol)
    {
	retval = TRUE;
	ఒకవేళ (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    మారింది_cline_bef_curs();
	}
    }

    ఒకవేళ (retval)
	curwin->w_సమితి_curswant = true;
    redraw_later(UPD_NOT_VALID);
    తిరిగి retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    పూర్ణాంకం
copy_option_part(
    char_u	**option,
    char_u	*buf,
    పూర్ణాంకం		maxlen,
    అక్షరం	*sep_chars)
{
    పూర్ణాంకం	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    ఒకవేళ (*p == '.')
	buf[len++] = *p++;
    అయితే (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == ఖాళీ)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	ఒకవేళ (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != ఖాళీ)
	    ++p;
	ఒకవేళ (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    ఒకవేళ (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_గమ్యం_option_part(p);	// p points to next file name

    *option = p;
    తిరిగి len;
}

#ఒకవేళ !defined(HAVE_MEMSET) && !defined(PROTO)
    శూన్య *
vim_memset(శూన్య *ptr, పూర్ణాంకం c, size_t కొలత)
{
    అక్షరం *p = ptr;

    అయితే (కొలత-- > 0)
	*p++ = c;
    తిరిగి ptr;
}
#అయితే_ముగింపు

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    పూర్ణాంకం
vim_isspace(పూర్ణాంకం x)
{
    తిరిగి ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

స్థిర నిర్మాణం modmasktable
{
    పొట్టి	mod_ముసుగు;	// Bit-mask for particular key modifier
    పొట్టి	mod_flag;	// Bit(s) for particular key modifier
    char_u	name;		// Single letter name of modifier
} mod_ముసుగు_పట్టిక[] =
{
    {MOD_ముసుగు_ALT,		MOD_ముసుగు_ALT,		(char_u)'M'},
    {MOD_ముసుగు_META,		MOD_ముసుగు_META,		(char_u)'T'},
    {MOD_ముసుగు_CTRL,		MOD_ముసుగు_CTRL,		(char_u)'C'},
    {MOD_ముసుగు_జరుపు,		MOD_ముసుగు_జరుపు,		(char_u)'S'},
    {MOD_ముసుగు_MULTI_CLICK,	MOD_ముసుగు_2_CLICK,	(char_u)'2'},
    {MOD_ముసుగు_MULTI_CLICK,	MOD_ముసుగు_3_CLICK,	(char_u)'3'},
    {MOD_ముసుగు_MULTI_CLICK,	MOD_ముసుగు_4_CLICK,	(char_u)'4'},
#ఒకవేళ defined(MACOS_X) || defined(FEAT_GUI_GTK)
    {MOD_ముసుగు_CMD,		MOD_ముసుగు_CMD,		(char_u)'D'},
#అయితే_ముగింపు
    // 'A' must be the last one
    {MOD_ముసుగు_ALT,		MOD_ముసుగు_ALT,		(char_u)'A'},
    {0, 0, NUL}
    // NOTE: when adding an entry, update MAX_KEY_NAME_LEN!
};

/*
 * Shifted key terminal codes and their unshifted equivalent.
 * Don't add mouse codes here, they are handled separately!
 */
#నిర్వచించు MOD_కీలకాలు_నమోదు_కొలత 5

స్థిర char_u modifier_కీలకాలు_పట్టిక[] =
{
//  mod mask	    with modifier		without modifier
    MOD_ముసుగు_జరుపు, '&', '9',			'@', '1',	// begin
    MOD_ముసుగు_జరుపు, '&', '0',			'@', '2',	// cancel
    MOD_ముసుగు_జరుపు, '*', '1',			'@', '4',	// command
    MOD_ముసుగు_జరుపు, '*', '2',			'@', '5',	// copy
    MOD_ముసుగు_జరుపు, '*', '3',			'@', '6',	// create
    MOD_ముసుగు_జరుపు, '*', '4',			'k', 'D',	// delete char
    MOD_ముసుగు_జరుపు, '*', '5',			'k', 'L',	// delete line
    MOD_ముసుగు_జరుపు, '*', '7',			'@', '7',	// end
    MOD_ముసుగు_CTRL,  KS_EXTRA, (పూర్ణాంకం)KE_C_చివర,	'@', '7',	// end
    MOD_ముసుగు_జరుపు, '*', '9',			'@', '9',	// exit
    MOD_ముసుగు_జరుపు, '*', '0',			'@', '0',	//కనుగొను
    MOD_ముసుగు_జరుపు, '#', '1',			'%', '1',	// help
    MOD_ముసుగు_జరుపు, '#', '2',			'k', 'h',	// home
    MOD_ముసుగు_CTRL,  KS_EXTRA, (పూర్ణాంకం)KE_C_HOME,	'k', 'h',	// home
    MOD_ముసుగు_జరుపు, '#', '3',			'k', 'I',	// insert
    MOD_ముసుగు_జరుపు, '#', '4',			'k', 'l',	// left arrow
    MOD_ముసుగు_CTRL,  KS_EXTRA, (పూర్ణాంకం)KE_C_ఎడమ,	'k', 'l',	// left arrow
    MOD_ముసుగు_జరుపు, '%', 'a',			'%', '3',	//సందేశం
    MOD_ముసుగు_జరుపు, '%', 'b',			'%', '4',	// move
    MOD_ముసుగు_జరుపు, '%', 'c',			'%', '5',	//తదుపరి
    MOD_ముసుగు_జరుపు, '%', 'd',			'%', '7',	// options
    MOD_ముసుగు_జరుపు, '%', 'e',			'%', '8',	//మునుపటి
    MOD_ముసుగు_జరుపు, '%', 'f',			'%', '9',	// print
    MOD_ముసుగు_జరుపు, '%', 'g',			'%', '0',	// redo
    MOD_ముసుగు_జరుపు, '%', 'h',			'&', '3',	// replace
    MOD_ముసుగు_జరుపు, '%', 'i',			'k', 'r',	// right arr.
    MOD_ముసుగు_CTRL,  KS_EXTRA, (పూర్ణాంకం)KE_C_కుడి,	'k', 'r',	// right arr.
    MOD_ముసుగు_జరుపు, '%', 'j',			'&', '5',	// resume
    MOD_ముసుగు_జరుపు, '!', '1',			'&', '6',	// save
    MOD_ముసుగు_జరుపు, '!', '2',			'&', '7',	// suspend
    MOD_ముసుగు_జరుపు, '!', '3',			'&', '8',	// undo
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_పైకి,	'k', 'u',	// up arrow
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_కిందికి,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_XF1,	KS_EXTRA, (పూర్ణాంకం)KE_XF1,
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_XF2,	KS_EXTRA, (పూర్ణాంకం)KE_XF2,
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_XF3,	KS_EXTRA, (పూర్ణాంకం)KE_XF3,
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_XF4,	KS_EXTRA, (పూర్ణాంకం)KE_XF4,

    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F1,	'k', '1',	// F1
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F2,	'k', '2',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F3,	'k', '3',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F4,	'k', '4',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F5,	'k', '5',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F6,	'k', '6',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F7,	'k', '7',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F8,	'k', '8',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F9,	'k', '9',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F10,	'k', ';',	// F10

    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F11,	'F', '1',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F12,	'F', '2',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F13,	'F', '3',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F14,	'F', '4',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F15,	'F', '5',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F16,	'F', '6',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F17,	'F', '7',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F18,	'F', '8',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F19,	'F', '9',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F20,	'F', 'A',

    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F21,	'F', 'B',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F22,	'F', 'C',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F23,	'F', 'D',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F24,	'F', 'E',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F25,	'F', 'F',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F26,	'F', 'G',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F27,	'F', 'H',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F28,	'F', 'I',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F29,	'F', 'J',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F30,	'F', 'K',

    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F31,	'F', 'L',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F32,	'F', 'M',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F33,	'F', 'N',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F34,	'F', 'O',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F35,	'F', 'P',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F36,	'F', 'Q',
    MOD_ముసుగు_జరుపు, KS_EXTRA, (పూర్ణాంకం)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_ముసుగు_జరుపు, 'k', 'B',			KS_EXTRA, (పూర్ణాంకం)KE_TAB,

    NUL
};

స్థిర నిర్మాణం కీలకం_name_నమోదు
{
    పూర్ణాంకం		enabled;	    // is this entry available (TRUE/FALSE)?
    పూర్ణాంకం		కీలకం;		    // special key code or ascii value
    string_T	name;		    // name of key
    పూర్ణాంకం		is_alt;		    // is an alternative name
} కీలకం_పేర్లు_పట్టిక[] =
// Must be sorted by the 'name.string' field in ascending order because it is used by bsearch()!
{
    {TRUE, K_BS, STR_LITERAL_INIT("BackSpace"), TRUE},
    {TRUE, '|', STR_LITERAL_INIT("Bar"), FALSE},
    {TRUE, K_BS, STR_LITERAL_INIT("BS"), FALSE},
    {TRUE, '\\', STR_LITERAL_INIT("Bslash"), FALSE},
    {TRUE, K_COMMAND, STR_LITERAL_INIT("Cmd"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("CR"), FALSE},
    {TRUE, CSI, STR_LITERAL_INIT("CSI"), FALSE},
    {TRUE, K_CURSORHOLD, STR_LITERAL_INIT("CursorHold"), FALSE},
    {
#నిర్వచితమైతే FEAT_MOUSE_DEC
    TRUE,
#లేకపోతే
    FALSE,
#అయితే_ముగింపు
	K_DEC_MOUSE, STR_LITERAL_INIT("DecMouse"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Del"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Delete"), TRUE},
    {TRUE, K_కిందికి, STR_LITERAL_INIT("Down"), FALSE},
    {TRUE, K_DROP, STR_LITERAL_INIT("Drop"), FALSE},
    {TRUE, K_చివర, STR_LITERAL_INIT("End"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Enter"), TRUE},
    {TRUE, ESC, STR_LITERAL_INIT("Esc"), FALSE},

    {TRUE, K_F1, STR_LITERAL_INIT("F1"), FALSE},
    {TRUE, K_F10, STR_LITERAL_INIT("F10"), FALSE},
    {TRUE, K_F11, STR_LITERAL_INIT("F11"), FALSE},
    {TRUE, K_F12, STR_LITERAL_INIT("F12"), FALSE},
    {TRUE, K_F13, STR_LITERAL_INIT("F13"), FALSE},
    {TRUE, K_F14, STR_LITERAL_INIT("F14"), FALSE},
    {TRUE, K_F15, STR_LITERAL_INIT("F15"), FALSE},
    {TRUE, K_F16, STR_LITERAL_INIT("F16"), FALSE},
    {TRUE, K_F17, STR_LITERAL_INIT("F17"), FALSE},
    {TRUE, K_F18, STR_LITERAL_INIT("F18"), FALSE},
    {TRUE, K_F19, STR_LITERAL_INIT("F19"), FALSE},

    {TRUE, K_F2, STR_LITERAL_INIT("F2"), FALSE},
    {TRUE, K_F20, STR_LITERAL_INIT("F20"), FALSE},
    {TRUE, K_F21, STR_LITERAL_INIT("F21"), FALSE},
    {TRUE, K_F22, STR_LITERAL_INIT("F22"), FALSE},
    {TRUE, K_F23, STR_LITERAL_INIT("F23"), FALSE},
    {TRUE, K_F24, STR_LITERAL_INIT("F24"), FALSE},
    {TRUE, K_F25, STR_LITERAL_INIT("F25"), FALSE},
    {TRUE, K_F26, STR_LITERAL_INIT("F26"), FALSE},
    {TRUE, K_F27, STR_LITERAL_INIT("F27"), FALSE},
    {TRUE, K_F28, STR_LITERAL_INIT("F28"), FALSE},
    {TRUE, K_F29, STR_LITERAL_INIT("F29"), FALSE},

    {TRUE, K_F3, STR_LITERAL_INIT("F3"), FALSE},
    {TRUE, K_F30, STR_LITERAL_INIT("F30"), FALSE},
    {TRUE, K_F31, STR_LITERAL_INIT("F31"), FALSE},
    {TRUE, K_F32, STR_LITERAL_INIT("F32"), FALSE},
    {TRUE, K_F33, STR_LITERAL_INIT("F33"), FALSE},
    {TRUE, K_F34, STR_LITERAL_INIT("F34"), FALSE},
    {TRUE, K_F35, STR_LITERAL_INIT("F35"), FALSE},
    {TRUE, K_F36, STR_LITERAL_INIT("F36"), FALSE},
    {TRUE, K_F37, STR_LITERAL_INIT("F37"), FALSE},

    {TRUE, K_F4, STR_LITERAL_INIT("F4"), FALSE},
    {TRUE, K_F5, STR_LITERAL_INIT("F5"), FALSE},
    {TRUE, K_F6, STR_LITERAL_INIT("F6"), FALSE},
    {TRUE, K_F7, STR_LITERAL_INIT("F7"), FALSE},
    {TRUE, K_F8, STR_LITERAL_INIT("F8"), FALSE},
    {TRUE, K_F9, STR_LITERAL_INIT("F9"), FALSE},

    {TRUE, K_FOCUSGAINED, STR_LITERAL_INIT("FocusGained"), FALSE},
    {TRUE, K_FOCUSLOST, STR_LITERAL_INIT("FocusLost"), FALSE},
    {TRUE, K_HELP, STR_LITERAL_INIT("Help"), FALSE},
    {TRUE, K_HOME, STR_LITERAL_INIT("Home"), FALSE},
    {TRUE, K_IGNORE, STR_LITERAL_INIT("Ignore"), FALSE},
    {TRUE, K_INS, STR_LITERAL_INIT("Ins"), TRUE},
    {TRUE, K_INS, STR_LITERAL_INIT("Insert"), FALSE},
    {
#నిర్వచితమైతే FEAT_MOUSE_JSB
    TRUE,
#లేకపోతే
    FALSE,
#అయితే_ముగింపు
	K_JSBTERM_MOUSE, STR_LITERAL_INIT("JsbMouse"), FALSE},
    {TRUE, K_K0, STR_LITERAL_INIT("k0"), FALSE},
    {TRUE, K_K1, STR_LITERAL_INIT("k1"), FALSE},
    {TRUE, K_K2, STR_LITERAL_INIT("k2"), FALSE},
    {TRUE, K_K3, STR_LITERAL_INIT("k3"), FALSE},
    {TRUE, K_K4, STR_LITERAL_INIT("k4"), FALSE},
    {TRUE, K_K5, STR_LITERAL_INIT("k5"), FALSE},
    {TRUE, K_K6, STR_LITERAL_INIT("k6"), FALSE},
    {TRUE, K_K7, STR_LITERAL_INIT("k7"), FALSE},
    {TRUE, K_K8, STR_LITERAL_INIT("k8"), FALSE},
    {TRUE, K_K9, STR_LITERAL_INIT("k9"), FALSE},

    {TRUE, K_KDEL, STR_LITERAL_INIT("kDel"), FALSE},
    {TRUE, K_KDIVIDE, STR_LITERAL_INIT("kDivide"), FALSE},
    {TRUE, K_KEND, STR_LITERAL_INIT("kEnd"), FALSE},
    {TRUE, K_KENTER, STR_LITERAL_INIT("kEnter"), FALSE},
    {TRUE, K_KHOME, STR_LITERAL_INIT("kHome"), FALSE},
    {TRUE, K_KINS, STR_LITERAL_INIT("kInsert"), FALSE},
    {TRUE, K_KMINUS, STR_LITERAL_INIT("kMinus"), FALSE},
    {TRUE, K_KMULTIPLY, STR_LITERAL_INIT("kMultiply"), FALSE},
    {TRUE, K_KPAGEDOWN, STR_LITERAL_INIT("kPageDown"), FALSE},
    {TRUE, K_KPAGEUP, STR_LITERAL_INIT("kPageUp"), FALSE},
    {TRUE, K_KPLUS, STR_LITERAL_INIT("kPlus"), FALSE},
    {TRUE, K_KPOINT, STR_LITERAL_INIT("kPoint"), FALSE},
    {TRUE, K_ఎడమ, STR_LITERAL_INIT("Left"), FALSE},
    {TRUE, K_LEFTDRAG, STR_LITERAL_INIT("LeftDrag"), FALSE},
    {TRUE, K_LEFTMOUSE, STR_LITERAL_INIT("LeftMouse"), FALSE},
    {TRUE, K_LEFTMOUSE_NM, STR_LITERAL_INIT("LeftMouseNM"), FALSE},
    {TRUE, K_LEFTRELEASE, STR_LITERAL_INIT("LeftRelease"), FALSE},
    {TRUE, K_LEFTRELEASE_NM, STR_LITERAL_INIT("LeftReleaseNM"), FALSE},
    {TRUE, NL, STR_LITERAL_INIT("LF"), TRUE},
    {TRUE, NL, STR_LITERAL_INIT("LineFeed"), TRUE},
    {TRUE, '<', STR_LITERAL_INIT("lt"), FALSE},
    {TRUE, K_MIDDLEDRAG, STR_LITERAL_INIT("MiddleDrag"), FALSE},
    {TRUE, K_MIDDLEMOUSE, STR_LITERAL_INIT("MiddleMouse"), FALSE},
    {TRUE, K_MIDDLERELEASE, STR_LITERAL_INIT("MiddleRelease"), FALSE},
    {TRUE, K_MOUSE, STR_LITERAL_INIT("Mouse"), FALSE},
    {TRUE, K_MOUSEDOWN, STR_LITERAL_INIT("MouseDown"), TRUE},
    {TRUE, K_MOUSEMOVE, STR_LITERAL_INIT("MouseMove"), FALSE},
    {TRUE, K_MOUSEUP, STR_LITERAL_INIT("MouseUp"), TRUE},
    {
#నిర్వచితమైతే FEAT_MOUSE_NET
    TRUE,
#లేకపోతే
    FALSE,
#అయితే_ముగింపు
	K_NETTERM_MOUSE, STR_LITERAL_INIT("NetMouse"), FALSE},
    {TRUE, NL, STR_LITERAL_INIT("NewLine"), TRUE},
    {TRUE, NL, STR_LITERAL_INIT("NL"), FALSE},
    {TRUE, K_ZERO, STR_LITERAL_INIT("Nul"), FALSE},
    {TRUE, OSC, STR_LITERAL_INIT("OSC"), FALSE},
    {TRUE, K_PAGEDOWN, STR_LITERAL_INIT("PageDown"), FALSE},
    {TRUE, K_PAGEUP, STR_LITERAL_INIT("PageUp"), FALSE},
    {TRUE, K_PE, STR_LITERAL_INIT("PasteEnd"), FALSE},
    {TRUE, K_PS, STR_LITERAL_INIT("PasteStart"), FALSE},
    {TRUE, K_PLUG, STR_LITERAL_INIT("Plug"), FALSE},
    {
#నిర్వచితమైతే FEAT_MOUSE_PTERM
    TRUE,
#లేకపోతే
    FALSE,
#అయితే_ముగింపు
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_కుడి, STR_LITERAL_INIT("Right"), FALSE},
    {TRUE, K_RIGHTDRAG, STR_LITERAL_INIT("RightDrag"), FALSE},
    {TRUE, K_RIGHTMOUSE, STR_LITERAL_INIT("RightMouse"), FALSE},
    {TRUE, K_RIGHTRELEASE, STR_LITERAL_INIT("RightRelease"), FALSE},
    {TRUE, K_SCRIPT_COMMAND, STR_LITERAL_INIT("ScriptCmd"), FALSE},
    {TRUE, K_MOUSEUP, STR_LITERAL_INIT("ScrollWheelDown"), FALSE},
    {TRUE, K_MOUSERIGHT, STR_LITERAL_INIT("ScrollWheelLeft"), FALSE},
    {TRUE, K_MOUSELEFT, STR_LITERAL_INIT("ScrollWheelRight"), FALSE},
    {TRUE, K_MOUSEDOWN, STR_LITERAL_INIT("ScrollWheelUp"), FALSE},
    {TRUE, K_SGR_MOUSE, STR_LITERAL_INIT("SgrMouse"), FALSE},
    {TRUE, K_SGR_MOUSERELEASE, STR_LITERAL_INIT("SgrMouseRelease"), FALSE},
    {
#నిర్వచితమైతే FEAT_EVAL
    TRUE,
#లేకపోతే
    FALSE,
#అయితే_ముగింపు
	K_SNR, STR_LITERAL_INIT("SNR"), FALSE},
    {TRUE, ' ', STR_LITERAL_INIT("Space"), FALSE},
    {TRUE, TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_UNDO, STR_LITERAL_INIT("Undo"), FALSE},
    {TRUE, K_పైకి, STR_LITERAL_INIT("Up"), FALSE},
    {
#నిర్వచితమైతే FEAT_MOUSE_URXVT
    TRUE,
#లేకపోతే
    FALSE,
#అయితే_ముగింపు
	K_URXVT_MOUSE, STR_LITERAL_INIT("UrxvtMouse"), FALSE},
    {TRUE, K_X1DRAG, STR_LITERAL_INIT("X1Drag"), FALSE},
    {TRUE, K_X1MOUSE, STR_LITERAL_INIT("X1Mouse"), FALSE},
    {TRUE, K_X1RELEASE, STR_LITERAL_INIT("X1Release"), FALSE},
    {TRUE, K_X2DRAG, STR_LITERAL_INIT("X2Drag"), FALSE},
    {TRUE, K_X2MOUSE, STR_LITERAL_INIT("X2Mouse"), FALSE},
    {TRUE, K_X2RELEASE, STR_LITERAL_INIT("X2Release"), FALSE},
    {TRUE, K_CSI, STR_LITERAL_INIT("xCSI"), FALSE},
    {TRUE, K_XDOWN, STR_LITERAL_INIT("xDown"), FALSE},
    {TRUE, K_XEND, STR_LITERAL_INIT("xEnd"), FALSE},
    {TRUE, K_XF1, STR_LITERAL_INIT("xF1"), FALSE},
    {TRUE, K_XF2, STR_LITERAL_INIT("xF2"), FALSE},
    {TRUE, K_XF3, STR_LITERAL_INIT("xF3"), FALSE},
    {TRUE, K_XF4, STR_LITERAL_INIT("xF4"), FALSE},
    {TRUE, K_XHOME, STR_LITERAL_INIT("xHome"), FALSE},
    {TRUE, K_XLEFT, STR_LITERAL_INIT("xLeft"), FALSE},
    {TRUE, K_OSC, STR_LITERAL_INIT("xOSC"), FALSE},
    {TRUE, K_XRIGHT, STR_LITERAL_INIT("xRight"), FALSE},
    {TRUE, K_XUP, STR_LITERAL_INIT("xUp"), FALSE},
    {TRUE, K_ZEND, STR_LITERAL_INIT("zEnd"), FALSE},
    {TRUE, K_ZHOME, STR_LITERAL_INIT("zHome"), FALSE}
    // NOTE: When adding a long name update MAX_KEY_NAME_LEN.
};

/*
 * Return the modifier mask bit (MOD_MASK_*) which corresponds to the given
 * modifier name ('S' for Shift, 'C' for Ctrl etc).
 */
    స్థిర పూర్ణాంకం
name_గమ్యం_mod_ముసుగు(పూర్ణాంకం c)
{
    పూర్ణాంకం	    i;

    c = TOUPPER_ASC(c);
    కొరకు (i = 0; mod_ముసుగు_పట్టిక[i].mod_ముసుగు != 0; i++)
	ఒకవేళ (c == mod_ముసుగు_పట్టిక[i].name)
	    తిరిగి mod_ముసుగు_పట్టిక[i].mod_flag;
    తిరిగి 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    పూర్ణాంకం
simplify_కీలకం(పూర్ణాంకం కీలకం, పూర్ణాంకం *modifiers)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    key0;
    పూర్ణాంకం	    key1;

    ఒకవేళ (!(*modifiers & (MOD_ముసుగు_జరుపు | MOD_ముసుగు_CTRL)))
	తిరిగి కీలకం;

    // TAB is a special case
    ఒకవేళ (కీలకం == TAB && (*modifiers & MOD_ముసుగు_జరుపు))
    {
	*modifiers &= ~MOD_ముసుగు_జరుపు;
	తిరిగి K_S_TAB;
    }
    key0 = KEY2TERMCAP0(కీలకం);
    key1 = KEY2TERMCAP1(కీలకం);
    కొరకు (i = 0; modifier_కీలకాలు_పట్టిక[i] != NUL; i += MOD_కీలకాలు_నమోదు_కొలత)
    {
	ఒకవేళ (key0 == modifier_కీలకాలు_పట్టిక[i + 3]
		&& key1 == modifier_కీలకాలు_పట్టిక[i + 4]
		&& (*modifiers & modifier_కీలకాలు_పట్టిక[i]))
	{
	    *modifiers &= ~modifier_కీలకాలు_పట్టిక[i];
	    తిరిగి TERMCAP2_కీలకం(modifier_కీలకాలు_పట్టిక[i + 1],
		    modifier_కీలకాలు_పట్టిక[i + 2]);
	}
    }
    తిరిగి కీలకం;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    పూర్ణాంకం
handle_x_కీలకాలు(పూర్ణాంకం కీలకం)
{
    ఎంపిక (కీలకం)
    {
	స్థితి K_XUP:	తిరిగి K_పైకి;
	స్థితి K_XDOWN:	తిరిగి K_కిందికి;
	స్థితి K_XLEFT:	తిరిగి K_ఎడమ;
	స్థితి K_XRIGHT:	తిరిగి K_కుడి;
	స్థితి K_XHOME:	తిరిగి K_HOME;
	స్థితి K_ZHOME:	తిరిగి K_HOME;
	స్థితి K_XEND:	తిరిగి K_చివర;
	స్థితి K_ZEND:	తిరిగి K_చివర;
	స్థితి K_XF1:	తిరిగి K_F1;
	స్థితి K_XF2:	తిరిగి K_F2;
	స్థితి K_XF3:	తిరిగి K_F3;
	స్థితి K_XF4:	తిరిగి K_F4;
	స్థితి K_S_XF1:	తిరిగి K_S_F1;
	స్థితి K_S_XF2:	తిరిగి K_S_F2;
	స్థితి K_S_XF3:	తిరిగి K_S_F3;
	స్థితి K_S_XF4:	తిరిగి K_S_F4;
    }
    తిరిగి కీలకం;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_కీలకం_name(పూర్ణాంకం c, పూర్ణాంకం modifiers)
{
    స్థిర char_u string[MAX_కీలకం_NAME_LEN + 1];
    పూర్ణాంకం	    i, idx, len;
    పూర్ణాంకం	    పట్టిక_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    ఒకవేళ (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_కీలకం)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    ఒకవేళ (IS_SPECIAL(c))
    {
	కొరకు (i = 0; modifier_కీలకాలు_పట్టిక[i] != 0; i += MOD_కీలకాలు_నమోదు_కొలత)
	    ఒకవేళ (       KEY2TERMCAP0(c) == (పూర్ణాంకం)modifier_కీలకాలు_పట్టిక[i + 1]
		    && (పూర్ణాంకం)KEY2TERMCAP1(c) == (పూర్ణాంకం)modifier_కీలకాలు_పట్టిక[i + 2])
	    {
		modifiers |= modifier_కీలకాలు_పట్టిక[i];
		c = TERMCAP2_కీలకం(modifier_కీలకాలు_పట్టిక[i + 3],
						   modifier_కీలకాలు_పట్టిక[i + 4]);
		ఆపు;
	    }
    }

    // try to find the key in the special key table
    పట్టిక_idx = కనుగొను_special_కీలకం_లోపల_పట్టిక(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    ఒకవేళ (c > 0 && (*mb_char2len)(c) == 1)
    {
	ఒకవేళ (పట్టిక_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_ముసుగు_ALT;
	    // try again, to find the un-alted key in the special key table
	    పట్టిక_idx = కనుగొను_special_కీలకం_లోపల_పట్టిక(c);
	}
	ఒకవేళ (పట్టిక_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_ముసుగు_CTRL;
	}
    }

    // translate the modifier into a string
    కొరకు (i = 0; mod_ముసుగు_పట్టిక[i].name != 'A'; i++)
	ఒకవేళ ((modifiers & mod_ముసుగు_పట్టిక[i].mod_ముసుగు)
						== mod_ముసుగు_పట్టిక[i].mod_flag)
	{
	    string[idx++] = mod_ముసుగు_పట్టిక[i].name;
	    string[idx++] = (char_u)'-';
	}

    ఒకవేళ (పట్టిక_idx < 0)		// unknown special key, may output t_xx
    {
	ఒకవేళ (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	లేకపోతే
	{
	    len = (*mb_char2len)(c);
	    ఒకవేళ (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    లేకపోతే ఒకవేళ (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    లేకపోతే
	    {
		char_u	*s = transchar(c);
		అయితే (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    లేకపోతే		// use name of special key
    {
	string_T    *s;

	s = &కీలకం_పేర్లు_పట్టిక[పట్టిక_idx].name;

	ఒకవేళ (s->నిడివి + idx + 2 <= MAX_కీలకం_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (పూర్ణాంకం)s->నిడివి;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    తిరిగి string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    పూర్ణాంకం
trans_special(
    char_u	**srcp,
    char_u	*dst,
    పూర్ణాంకం		flags,		// FSK_ values
    పూర్ణాంకం		escape_ks,	// escape K_SPECIAL bytes in the character
    పూర్ణాంకం		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    పూర్ణాంకం		modifiers = 0;
    పూర్ణాంకం		కీలకం;

    కీలకం = కనుగొను_special_కీలకం(srcp, &modifiers, flags, did_simplify);
    ఒకవేళ (కీలకం == 0)
	తిరిగి 0;

    తిరిగి special_గమ్యం_buf(కీలకం, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    పూర్ణాంకం
special_గమ్యం_buf(పూర్ణాంకం కీలకం, పూర్ణాంకం modifiers, పూర్ణాంకం escape_ks, char_u *dst)
{
    పూర్ణాంకం		dlen = 0;

    // Put the appropriate modifier in a string
    ఒకవేళ (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    ఒకవేళ (IS_SPECIAL(కీలకం))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(కీలకం);
	dst[dlen++] = KEY2TERMCAP1(కీలకం);
    }
    లేకపోతే ఒకవేళ (escape_ks)
	dlen = (పూర్ణాంకం)(కూడు_char2buf(కీలకం, dst + dlen) - dst);
    లేకపోతే ఒకవేళ (has_mbyte)
	dlen += (*mb_char2bytes)(కీలకం, dst + dlen);
    లేకపోతే
	dst[dlen++] = కీలకం;

    తిరిగి dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    పూర్ణాంకం
కనుగొను_special_కీలకం(
    char_u	**srcp,
    పూర్ణాంకం		*modp,
    పూర్ణాంకం		flags,		// FSK_ values
    పూర్ణాంకం		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*చివరి_dash;
    char_u	*చివర_యొక్క_name;
    char_u	*src;
    char_u	*bp;
    పూర్ణాంకం		లోపల_string = flags & FSK_లోపల_STRING;
    పూర్ణాంకం		modifiers;
    పూర్ణాంకం		బిట్టు;
    పూర్ణాంకం		కీలకం;
    uvarnumber_T	n;
    పూర్ణాంకం		l;

    src = *srcp;
    ఒకవేళ (src[0] != '<')
	తిరిగి 0;
    ఒకవేళ (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    చివరి_dash = src;
    కొరకు (bp = src + 1; *bp == '-' || vim_is_సాధారణ_IDc(*bp); bp++)
    {
	ఒకవేళ (*bp == '-')
	{
	    చివరి_dash = bp;
	    ఒకవేళ (bp[1] != NUL)
	    {
		ఒకవేళ (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		లేకపోతే
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		ఒకవేళ (!(లోపల_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		లేకపోతే ఒకవేళ (లోపల_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	ఒకవేళ (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	లేకపోతే ఒకవేళ (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, ఖాళీ, &l, STR2NR_ALL, ఖాళీ, ఖాళీ, 0, TRUE, ఖాళీ);
	    ఒకవేళ (l == 0)
	    {
		emsg(_(e_invalid_వాదం));
		తిరిగి 0;
	    }
	    bp += l + 5;
	    ఆపు;
	}
    }

    ఒకవేళ (*bp == '>')	// found matching '>'
    {
	చివర_యొక్క_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	కొరకు (bp = src + 1; bp < చివరి_dash; bp++)
	{
	    ఒకవేళ (*bp != '-')
	    {
		బిట్టు = name_గమ్యం_mod_ముసుగు(*bp);
		ఒకవేళ (బిట్టు == 0x0)
		    ఆపు;	// Illegal modifier name
		modifiers |= బిట్టు;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	ఒకవేళ (bp >= చివరి_dash)
	{
	    ఒకవేళ (STRNICMP(చివరి_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(చివరి_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(చివరి_dash + 6, ఖాళీ, &l, STR2NR_ALL, ఖాళీ,
							    &n, 0, TRUE, ఖాళీ);
		ఒకవేళ (l == 0)
		{
		    emsg(_(e_invalid_వాదం));
		    తిరిగి 0;
		}
		కీలకం = (పూర్ణాంకం)n;
	    }
	    లేకపోతే
	    {
		పూర్ణాంకం off = 1;

		// Modifier with single letter, or special key name.
		ఒకవేళ (లోపల_string && చివరి_dash[1] == '\\' && చివరి_dash[2] == '"')
		    off = 2;
		ఒకవేళ (has_mbyte)
		    l = mb_ptr2len(చివరి_dash + off);
		లేకపోతే
		    l = 1;
		ఒకవేళ (modifiers != 0 && చివరి_dash[l + off] == '>')
		    కీలకం = PTR2CHAR(చివరి_dash + off);
		లేకపోతే
		{
		    కీలకం = get_special_కీలకం_సంకేతం(చివరి_dash + off);
		    ఒకవేళ (!(flags & FSK_KEEP_X_కీలకం))
			కీలకం = handle_x_కీలకాలు(కీలకం);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    ఒకవేళ (కీలకం != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		కీలకం = simplify_కీలకం(కీలకం, &modifiers);

		ఒకవేళ ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    ఒకవేళ (కీలకం == K_BS)
			కీలకం = BS;
		    లేకపోతే ఒకవేళ (కీలకం == K_DEL || కీలకం == K_KDEL)
			కీలకం = DEL;
		}
		లేకపోతే ఒకవేళ (కీలకం == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    ఒకవేళ ((flags & FSK_SIMPLIFY) != 0)
		    {
			ఒకవేళ (did_simplify != ఖాళీ)
			    *did_simplify = TRUE;
		    }
		    లేకపోతే
			కీలకం = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		ఒకవేళ (!IS_SPECIAL(కీలకం))
		    కీలకం = వెలికితీయి_modifiers(కీలకం, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = చివర_యొక్క_name;
		తిరిగి కీలకం;
	    }
	}
    }
    తిరిగి 0;
}


/*
 * Some keys are used with Ctrl without Shift and are still expected to be
 * mapped as if Shift was pressed:
 * CTRL-2 is CTRL-@
 * CTRL-6 is CTRL-^
 * CTRL-- is CTRL-_
 * Also, unless no_reduce_keys is set then <C-H> and <C-h> mean the same thing,
 * use "H".
 * Returns the possibly adjusted key.
 */
    పూర్ణాంకం
may_adjust_కీలకం_for_ctrl(పూర్ణాంకం modifiers, పూర్ణాంకం కీలకం)
{
    ఒకవేళ ((modifiers & MOD_ముసుగు_CTRL) == 0)
	తిరిగి కీలకం;

    ఒకవేళ (ASCII_ISALPHA(కీలకం))
    {
#నిర్వచితమైతే FEAT_TERMINAL
	check_no_reduce_కీలకాలు();  // may update the no_reduce_keys flag
#అయితే_ముగింపు
	తిరిగి no_reduce_కీలకాలు == 0 ? TOUPPER_ASC(కీలకం) : కీలకం;
    }
    ఒకవేళ (కీలకం == '2')
	తిరిగి '@';
    ఒకవేళ (కీలకం == '6')
	తిరిగి '^';
    ఒకవేళ (కీలకం == '-')
	తిరిగి '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    ఒకవేళ (కీలకం == '$' && (modifiers & MOD_ముసుగు_జరుపు) == 0)
	తిరిగి ']';

    తిరిగి కీలకం;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    పూర్ణాంకం
may_remove_జరుపు_modifier(పూర్ణాంకం modifiers, పూర్ణాంకం కీలకం)
{
    ఒకవేళ ((modifiers == MOD_ముసుగు_జరుపు
		|| modifiers == (MOD_ముసుగు_జరుపు | MOD_ముసుగు_ALT)
#నిర్వచితమైతే FEAT_GUI_GTK
		|| modifiers == (MOD_ముసుగు_జరుపు | MOD_ముసుగు_CMD)
#అయితే_ముగింపు
		|| modifiers == (MOD_ముసుగు_జరుపు | MOD_ముసుగు_META))
	    && ((కీలకం >= '!' && కీలకం <= '/')
		|| (కీలకం >= ':' && కీలకం <= 'Z')
		|| vim_isdigit(కీలకం)
		|| (కీలకం >= '[' && కీలకం <= '`')
		|| (కీలకం >= '{' && కీలకం <= '~')))
	తిరిగి modifiers & ~MOD_ముసుగు_జరుపు;

    ఒకవేళ (modifiers == (MOD_ముసుగు_జరుపు | MOD_ముసుగు_CTRL)
		&& (కీలకం == '{' || కీలకం == '}' || కీలకం == '|'))
	తిరిగి modifiers & ~MOD_ముసుగు_జరుపు;

    తిరిగి modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    పూర్ణాంకం
వెలికితీయి_modifiers(పూర్ణాంకం కీలకం, పూర్ణాంకం *modp, పూర్ణాంకం simplify, పూర్ణాంకం *did_simplify)
{
    పూర్ణాంకం	modifiers = *modp;

#నిర్వచితమైతే MACOS_X
    // Command-key really special, no fancynest
    ఒకవేళ (!(modifiers & MOD_ముసుగు_CMD))
#అయితే_ముగింపు
    ఒకవేళ ((modifiers & MOD_ముసుగు_జరుపు) && ASCII_ISALPHA(కీలకం))
    {
	కీలకం = TOUPPER_ASC(కీలకం);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	ఒకవేళ (simplify || modifiers == MOD_ముసుగు_జరుపు
		|| modifiers == (MOD_ముసుగు_జరుపు | MOD_ముసుగు_ALT)
		|| modifiers == (MOD_ముసుగు_జరుపు | MOD_ముసుగు_META))
	    modifiers &= ~MOD_ముసుగు_జరుపు;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    ఒకవేళ ((modifiers & MOD_ముసుగు_CTRL) && ASCII_ISALPHA(కీలకం))
	కీలకం = TOUPPER_ASC(కీలకం);

    ఒకవేళ (simplify && (modifiers & MOD_ముసుగు_CTRL)
	    && ((కీలకం >= '?' && కీలకం <= '_') || ASCII_ISALPHA(కీలకం)))
    {
	కీలకం = Ctrl_chr(కీలకం);
	modifiers &= ~MOD_ముసుగు_CTRL;
	// <C-@> is <Nul>
	ఒకవేళ (కీలకం == NUL)
	    కీలకం = K_ZERO;
	ఒకవేళ (did_simplify != ఖాళీ)
	    *did_simplify = TRUE;
    }

#నిర్వచితమైతే MACOS_X
    // Command-key really special, no fancynest
    ఒకవేళ (!(modifiers & MOD_ముసుగు_CMD))
#అయితే_ముగింపు
    ఒకవేళ (simplify && (modifiers & MOD_ముసుగు_ALT) && కీలకం < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	కీలకం |= 0x80;
	modifiers &= ~MOD_ముసుగు_ALT;	// remove the META modifier
	ఒకవేళ (did_simplify != ఖాళీ)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    తిరిగి కీలకం;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    పూర్ణాంకం
కనుగొను_special_కీలకం_లోపల_పట్టిక(పూర్ణాంకం c)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(కీలకం_పేర్లు_పట్టిక); i++)
	ఒకవేళ (c == కీలకం_పేర్లు_పట్టిక[i].కీలకం && !కీలకం_పేర్లు_పట్టిక[i].is_alt)
	    తిరిగి కీలకం_పేర్లు_పట్టిక[i].enabled ? i : -1;

    తిరిగి -1;
}


/*
 * Compare two 'struct key_name_entry' structures.
 * Note that the target string (p1) may contain additional trailing characters
 * that should not factor into the comparison. Example:
 * 'LeftMouse>", "<LeftMouse>"] ...'
 * should match with
 * 'LeftMouse'.
 * These characters are identified by vim_isNormalIDc().
 */
    స్థిర పూర్ణాంకం
cmp_కీలకం_name_నమోదు(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    char_u  *p1 = ((నిర్మాణం కీలకం_name_నమోదు *)a)->name.string;
    char_u  *p2 = ((నిర్మాణం కీలకం_name_నమోదు *)b)->name.string;
    పూర్ణాంకం	    ఫలితం = 0;

    ఒకవేళ (p1 == p2)
	తిరిగి 0;

    అయితే (vim_is_సాధారణ_IDc(*p1) && *p2 != NUL)
    {
	ఒకవేళ ((ఫలితం = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    ఆపు;
	++p1;
	++p2;
    }

    ఒకవేళ (ఫలితం == 0)
    {
	ఒకవేళ (*p2 == NUL)
	{
	    ఒకవేళ (vim_is_సాధారణ_IDc(*p1))
		ఫలితం = 1;
	}
	లేకపోతే
	{
	    ఫలితం = -1;
	}
    }

    తిరిగి ఫలితం;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    పూర్ణాంకం
get_special_కీలకం_సంకేతం(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    ఒకవేళ (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	ఒకవేళ (కూడు_termcap_నమోదు(string, FALSE) == OK)
	    తిరిగి TERMCAP2_కీలకం(name[2], name[3]);
    }
    లేకపోతే
    {
	నిర్మాణం కీలకం_name_నమోదు	లక్ష్యం;
	నిర్మాణం కీలకం_name_నమోదు	*నమోదు_2;

	లక్ష్యం.enabled = TRUE;
	లక్ష్యం.కీలకం = 0;
	లక్ష్యం.name.string = name;
	లక్ష్యం.name.నిడివి = 0;

	నమోదు_2 = (నిర్మాణం కీలకం_name_నమోదు *)bsearch(
	    &లక్ష్యం,
	    &కీలకం_పేర్లు_పట్టిక,
	    ARRAY_నిడివి(కీలకం_పేర్లు_పట్టిక),
	    పరిమాణం(కీలకం_పేర్లు_పట్టిక[0]),
	    cmp_కీలకం_name_నమోదు);
	ఒకవేళ (నమోదు_2 != ఖాళీ && నమోదు_2->enabled)
	{
	    పూర్ణాంకం కీలకం = నమోదు_2->కీలకం;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    తిరిగి కీలకం == K_TAB ? TAB : కీలకం;
	}
    }

    తిరిగి 0;
}

    char_u *
get_కీలకం_name(పూర్ణాంకం i)
{
    ఒకవేళ (i < 0 || i >= (పూర్ణాంకం)ARRAY_నిడివి(కీలకం_పేర్లు_పట్టిక))
	తిరిగి ఖాళీ;

    తిరిగి కీలకం_పేర్లు_పట్టిక[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    పూర్ణాంకం
get_fileformat(buf_T *buf)
{
    పూర్ణాంకం		c = *buf->b_p_ff;

    ఒకవేళ (buf->b_p_bin || c == 'u')
	తిరిగి EOL_UNIX;
    ఒకవేళ (c == 'm')
	తిరిగి EOL_MAC;
    తిరిగి EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    పూర్ణాంకం
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    పూర్ణాంకం		c;

    ఒకవేళ (eap != ఖాళీ && eap->force_ff != 0)
	c = eap->force_ff;
    లేకపోతే
    {
	ఒకవేళ ((eap != ఖాళీ && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    తిరిగి EOL_UNIX;
	c = *buf->b_p_ff;
    }
    ఒకవేళ (c == 'u')
	తిరిగి EOL_UNIX;
    ఒకవేళ (c == 'm')
	తిరిగి EOL_MAC;
    తిరిగి EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    శూన్య
సమితి_fileformat(
    పూర్ణాంకం		t,
    పూర్ణాంకం		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    అక్షరం	*p = ఖాళీ;

    ఎంపిక (t)
    {
    స్థితి EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	ఆపు;
    స్థితి EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	ఆపు;
    స్థితి EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	ఆపు;
    }
    ఒకవేళ (p != ఖాళీ)
	సమితి_string_option_ప్రత్యక్ష((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_స్థితి(curbuf);
    redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    పూర్ణాంకం
default_fileformat(శూన్య)
{
    ఎంపిక (*p_ffs)
    {
	స్థితి 'm':   తిరిగి EOL_MAC;
	స్థితి 'd':   తిరిగి EOL_DOS;
    }
    తిరిగి EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    పూర్ణాంకం
call_shell(char_u *cmd, పూర్ణాంకం opt)
{
    పూర్ణాంకం		retval;
#నిర్వచితమైతే FEAT_PROFILE
    proftime_T	wait_సమయం;
#అయితే_ముగింపు

    ఒకవేళ (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == ఖాళీ ? p_sh : cmd);
	సందేశం_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	prof_బిడ్డ_enter(&wait_సమయం);
#అయితే_ముగింపు

    ఒకవేళ (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_ఖాళీలు));
	retval = -1;
    }
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_GUI_MSWIN
	// Don't hide the pointer while executing a shell command.
	gui_mch_mousehide(FALSE);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
	++hold_gui_events;
#అయితే_ముగింపు
	// The external command may update a tags file, clear cached tags.
	ట్యాగ్_freematch();

	ఒకవేళ (cmd == ఖాళీ || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	లేకపోతే
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    ఒకవేళ (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		ఒకవేళ (ecmd == ఖాళీ)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    ఒకవేళ (ncmd != ఖాళీ)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((అక్షరం *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    లేకపోతే
		retval = -1;
	    ఒకవేళ (ecmd != cmd)
		vim_free(ecmd);
	}
#నిర్వచితమైతే FEAT_GUI
	--hold_gui_events;
#అయితే_ముగింపు
	/*
	 * Check the window size, in case it changed while executing the
	 * external command.
	 */
	shell_resized_check();
    }

#నిర్వచితమైతే FEAT_EVAL
    సమితి_vim_var_nr(VV_SHELL_ERROR, (దీర్ఘ)retval);
# ifdef FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	prof_బిడ్డ_exit(&wait_సమయం);
# endif
#అయితే_ముగింపు

    తిరిగి retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    పూర్ణాంకం
get_real_state(శూన్య)
{
    ఒకవేళ (State & బహుళకం_సాధారణ)
    {
	ఒకవేళ (VIsual_చురుకైన)
	{
	    ఒకవేళ (VIsual_select)
		తిరిగి బహుళకం_SELECT;
	    తిరిగి బహుళకం_VISUAL;
	}
	లేకపోతే ఒకవేళ (ముగింపు_op)
	    తిరిగి బహుళకం_OP_PENDING;
    }
    తిరిగి State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    పూర్ణాంకం
after_pathsep(char_u *b, char_u *p)
{
    తిరిగి p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_తల_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    పూర్ణాంకం
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    ఒకవేళ (f1 == ఖాళీ || f2 == ఖాళీ)
	తిరిగి FALSE;

    (శూన్య)vim_నిండిన_Name(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    తిరిగి (t1 - ffname == t2 - f2
	     && pathcmp((అక్షరం *)ffname, (అక్షరం *)f2, (పూర్ణాంకం)(t1 - ffname)) == 0);
}

#ఒకవేళ defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    పూర్ణాంకం
vim_chdirfile(char_u *fname, అక్షరం *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    ఒకవేళ (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    ఒకవేళ (pathcmp((అక్షరం *)old_dir, (అక్షరం *)new_dir, -1) == 0)
	// nothing to do
	తిరిగి OK;

    ఒకవేళ (trigger_autocmd != ఖాళీ)
	trigger_Dir_మారింది_Pre((char_u *)trigger_autocmd, new_dir);

    ఒకవేళ (mch_chdir((అక్షరం *)new_dir) != 0)
	తిరిగి FAIL;

    ఒకవేళ (trigger_autocmd != ఖాళీ)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    తిరిగి OK;
}
#అయితే_ముగింపు

#ఒకవేళ defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    స్థిర పూర్ణాంకం
illegal_slash(స్థిరరాశి అక్షరం *name)
{
    ఒకవేళ (name[0] == NUL)
	తిరిగి FALSE;	    // no file name is not illegal
    ఒకవేళ (name[అక్షరశ్రేణి_పొడవు(name) - 1] != '/')
	తిరిగి FALSE;	    // no trailing slash
    ఒకవేళ (mch_isdir((char_u *)name))
	తిరిగి FALSE;	    // trailing slash for a directory
    తిరిగి TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    పూర్ణాంకం
vim_stat(స్థిరరాశి అక్షరం *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    తిరిగి illegal_slash(name) ? -1 : stat(name, stp);
}
#అయితే_ముగింపు

#ఒకవేళ defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_పట్టిక[SHAPE_IDX_లెక్క] =
{
    // The values will be filled in from the 'guicursor' and 'mouseshape'
    // defaults when Vim starts.
    // Adjust the SHAPE_IDX_ defines when making changes!
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "n", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "v", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "i", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "r", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "c", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "ci", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "cr", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "o", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "ve", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "e", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "s", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "sd", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "vs", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "vd", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "m", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "ml", SHAPE_MOUSE},
    {0,	0, 0, 100L, 100L, 100L, 0, 0, "sm", SHAPE_CURSOR},
};

# ifdef FEAT_MOUSESHAPE
/*
 * Table with names for mouse shapes.  Keep in sync with all the tables for
 * mch_set_mouse_shape()!.
 */
స్థిర string_T mshape_పేర్లు[] =
{
    STR_LITERAL_INIT("arrow"),	// default, must be the first one
    STR_LITERAL_INIT("blank"),	// hidden
    STR_LITERAL_INIT("beam"),
    STR_LITERAL_INIT("updown"),
    STR_LITERAL_INIT("udsizing"),
    STR_LITERAL_INIT("leftright"),
    STR_LITERAL_INIT("lrsizing"),
    STR_LITERAL_INIT("busy"),
    STR_LITERAL_INIT("no"),
    STR_LITERAL_INIT("crosshair"),
    STR_LITERAL_INIT("hand1"),
    STR_LITERAL_INIT("hand2"),
    STR_LITERAL_INIT("pencil"),
    STR_LITERAL_INIT("question"),
    STR_LITERAL_INIT("rightup-arrow"),
    STR_LITERAL_INIT("up-arrow"),
    {ఖాళీ, 0}
};

#  define MSHAPE_పేర్లు_లెక్క  (ARRAY_నిడివి(mshape_పేర్లు) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    అక్షరం *
parse_shape_opt(పూర్ణాంకం what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    పూర్ణాంకం		idx = 0;		// init for GCC
    పూర్ణాంకం		all_idx;
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    దీర్ఘ	n;
    పూర్ణాంకం		దొరికింది_ve = FALSE;	// found "ve" flag
    పూర్ణాంకం		విడత;

    /*
     * First round: check for errors; second round: do it for real.
     */
    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	ఒకవేళ (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	లేకపోతే
# endif
	    modep = p_guicursor;
	అయితే (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    ఒకవేళ (colonp == ఖాళీ || (commap != ఖాళీ && commap < colonp))
		తిరిగి e_missing_colon_2;
	    ఒకవేళ (colonp == modep)
		తిరిగి e_illegal_బహుళకం;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    అయితే (modep < colonp || all_idx >= 0)
	    {
		ఒకవేళ (all_idx < 0)
		{
		    // Find the mode.
		    ఒకవేళ (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    లేకపోతే
			len = 2;
		    ఒకవేళ (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_లెక్క - 1;
		    లేకపోతే
		    {
			కొరకు (idx = 0; idx < SHAPE_IDX_లెక్క; ++idx)
			    ఒకవేళ (STRNICMP(modep, shape_పట్టిక[idx].name, len)
									 == 0)
				ఆపు;
			ఒకవేళ (idx == SHAPE_IDX_లెక్క
				   || (shape_పట్టిక[idx].వాడిన_for & what) == 0)
			    తిరిగి e_illegal_బహుళకం;
			ఒకవేళ (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    దొరికింది_ve = TRUE;
		    }
		    modep += len + 1;
		}

		ఒకవేళ (all_idx >= 0)
		    idx = all_idx--;
		లేకపోతే ఒకవేళ (విడత == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    ఒకవేళ (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_పట్టిక[idx].mshape = 0;
		    }
		    లేకపోతే
# endif
		    {
			// Set the defaults, for the missing parts
			shape_పట్టిక[idx].shape = SHAPE_ఖండం;
			shape_పట్టిక[idx].blinkwait = 700L;
			shape_పట్టిక[idx].blinkon = 400L;
			shape_పట్టిక[idx].blinkoff = 250L;
		    }
		}

		// Parse the part after the colon
		కొరకు (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    ఒకవేళ (what == SHAPE_MOUSE)
		    {
			కొరకు (i = 0; ; ++i)
			{
			    ఒకవేళ (mshape_పేర్లు[i].string == ఖాళీ)
			    {
				ఒకవేళ (!VIM_ISDIGIT(*p))
				    తిరిగి e_illegal_mouseshape;
				ఒకవేళ (విడత == 2)
				    shape_పట్టిక[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				లేకపోతే
				    (శూన్య)getdigits(&p);
				ఆపు;
			    }
			    ఒకవేళ (STRNICMP(p, mshape_పేర్లు[i].string, mshape_పేర్లు[i].నిడివి) == 0)
			    {
				ఒకవేళ (విడత == 2)
				    shape_పట్టిక[idx].mshape = i;
				p += mshape_పేర్లు[i].నిడివి;
				ఆపు;
			    }
			}
		    }
		    లేకపోతే // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			ఒకవేళ (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			లేకపోతే ఒకవేళ (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			లేకపోతే ఒకవేళ (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			లేకపోతే ఒకవేళ (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			లేకపోతే ఒకవేళ (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			ఒకవేళ (len != 0)
			{
			    p += len;
			    ఒకవేళ (!VIM_ISDIGIT(*p))
				తిరిగి e_digit_expected;
			    n = getdigits(&p);
			    ఒకవేళ (len == 3)   // "ver" or "hor"
			    {
				ఒకవేళ (n == 0)
				    తిరిగి e_illegal_percentage;
				ఒకవేళ (విడత == 2)
				{
				    ఒకవేళ (TOLOWER_ASC(i) == 'v')
					shape_పట్టిక[idx].shape = SHAPE_VER;
				    లేకపోతే
					shape_పట్టిక[idx].shape = SHAPE_HOR;
				    shape_పట్టిక[idx].percentage = n;
				}
			    }
			    లేకపోతే ఒకవేళ (విడత == 2)
			    {
				ఒకవేళ (len == 9)
				    shape_పట్టిక[idx].blinkwait = n;
				లేకపోతే ఒకవేళ (len == 7)
				    shape_పట్టిక[idx].blinkon = n;
				లేకపోతే
				    shape_పట్టిక[idx].blinkoff = n;
			    }
			}
			లేకపోతే ఒకవేళ (STRNICMP(p, "block", 5) == 0)
			{
			    ఒకవేళ (విడత == 2)
				shape_పట్టిక[idx].shape = SHAPE_ఖండం;
			    p += 5;
			}
			లేకపోతే	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    ఒకవేళ (commap == ఖాళీ)		    // last part
			    {
				ఒకవేళ (endp == ఖాళీ)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    లేకపోతే ఒకవేళ (endp > commap || endp == ఖాళీ)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    ఒకవేళ (slashp != ఖాళీ && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_సమూహం(p, (పూర్ణాంకం)(slashp - p));
				p = slashp + 1;
			    }
			    ఒకవేళ (విడత == 2)
			    {
				shape_పట్టిక[idx].id = syn_check_సమూహం(p,
							     (పూర్ణాంకం)(endp - p));
				shape_పట్టిక[idx].id_lm = shape_పట్టిక[idx].id;
				ఒకవేళ (slashp != ఖాళీ && slashp < endp)
				    shape_పట్టిక[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    ఒకవేళ (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    ఒకవేళ (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    ఒకవేళ (!దొరికింది_ve)
    {
# ifdef FEAT_MOUSESHAPE
	ఒకవేళ (what == SHAPE_MOUSE)
	{
	    shape_పట్టిక[SHAPE_IDX_VE].mshape = shape_పట్టిక[SHAPE_IDX_V].mshape;
	}
	లేకపోతే
# endif
	{
	    shape_పట్టిక[SHAPE_IDX_VE].shape = shape_పట్టిక[SHAPE_IDX_V].shape;
	    shape_పట్టిక[SHAPE_IDX_VE].percentage =
					 shape_పట్టిక[SHAPE_IDX_V].percentage;
	    shape_పట్టిక[SHAPE_IDX_VE].blinkwait =
					  shape_పట్టిక[SHAPE_IDX_V].blinkwait;
	    shape_పట్టిక[SHAPE_IDX_VE].blinkon =
					    shape_పట్టిక[SHAPE_IDX_V].blinkon;
	    shape_పట్టిక[SHAPE_IDX_VE].blinkoff =
					   shape_పట్టిక[SHAPE_IDX_V].blinkoff;
	    shape_పట్టిక[SHAPE_IDX_VE].id = shape_పట్టిక[SHAPE_IDX_V].id;
	    shape_పట్టిక[SHAPE_IDX_VE].id_lm = shape_పట్టిక[SHAPE_IDX_V].id_lm;
	}
    }

    తిరిగి ఖాళీ;
}

# ఒకవేళ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    పూర్ణాంకం
get_shape_idx(పూర్ణాంకం mouse)
{
#  ifdef FEAT_MOUSESHAPE
    ఒకవేళ (mouse && (State == బహుళకం_HITRETURN || State == బహుళకం_ASKMORE))
    {
#   ifdef FEAT_GUI
	పూర్ణాంకం x, y;
	gui_mch_getmouse(&x, &y);
	ఒకవేళ (Y_2_అడ్డువరుస(y) == అడ్డువరుసలు - 1)
	    తిరిగి SHAPE_IDX_MOREL;
#   endif
	తిరిగి SHAPE_IDX_మరిన్ని;
    }
    ఒకవేళ (mouse && drag_స్థితి_పంక్తి)
	తిరిగి SHAPE_IDX_SDRAG;
    ఒకవేళ (mouse && drag_sep_పంక్తి)
	తిరిగి SHAPE_IDX_VDRAG;
#  endif
    ఒకవేళ (!mouse && State == బహుళకం_SHOWMATCH)
	తిరిగి SHAPE_IDX_SM;
    ఒకవేళ (State & VREPLACE_FLAG)
	తిరిగి SHAPE_IDX_R;
    ఒకవేళ (State & REPLACE_FLAG)
	తిరిగి SHAPE_IDX_R;
    ఒకవేళ (State & బహుళకం_చొప్పించు)
	తిరిగి SHAPE_IDX_I;
    ఒకవేళ (State & బహుళకం_CMDLINE)
    {
	ఒకవేళ (cmdline_at_చివర())
	    తిరిగి SHAPE_IDX_C;
	ఒకవేళ (cmdline_overstrike())
	    తిరిగి SHAPE_IDX_CR;
	తిరిగి SHAPE_IDX_CI;
    }
    ఒకవేళ (ముగింపు_op)
	తిరిగి SHAPE_IDX_O;
    ఒకవేళ (VIsual_చురుకైన)
    {
	ఒకవేళ (*p_sel == 'e')
	    తిరిగి SHAPE_IDX_VE;
	లేకపోతే
	    తిరిగి SHAPE_IDX_V;
    }
    తిరిగి SHAPE_IDX_N;
}
# endif

# ఒకవేళ defined(FEAT_MOUSESHAPE)
స్థిర పూర్ణాంకం current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    శూన్య
update_mouseshape(పూర్ణాంకం shape_idx)
{
    పూర్ణాంకం new_mouse_shape;

    // Only works in GUI mode.
    ఒకవేళ (!gui.లోపల_use || gui.starting)
	తిరిగి;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    ఒకవేళ (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	తిరిగి;
    }

    // When ignoring the mouse don't change shape on the statusline.
    ఒకవేళ (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_స్థితి
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    ఒకవేళ (shape_idx == -2
	    && current_mouse_shape != shape_పట్టిక[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_పట్టిక[SHAPE_IDX_స్థితి].mshape
	    && current_mouse_shape != shape_పట్టిక[SHAPE_IDX_VSEP].mshape)
	తిరిగి;
    ఒకవేళ (shape_idx < 0)
	new_mouse_shape = shape_పట్టిక[get_shape_idx(TRUE)].mshape;
    లేకపోతే
	new_mouse_shape = shape_పట్టిక[shape_idx].mshape;
    ఒకవేళ (new_mouse_shape != current_mouse_shape)
    {
	mch_సమితి_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#అయితే_ముగింపు // CURSOR_SHAPE

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    శూన్య
f_getmouseshape(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
# ఒకవేళ defined(FEAT_MOUSESHAPE)
    ఒకవేళ (current_mouse_shape >= 0
			      && current_mouse_shape < (పూర్ణాంకం)MSHAPE_పేర్లు_లెక్క)
	rettv->vval.v_string = vim_strnsave(
				  mshape_పేర్లు[current_mouse_shape].string,
				  mshape_పేర్లు[current_mouse_shape].నిడివి);
# endif
}
#అయితే_ముగింపు



/*
 * Change directory to "new_dir".  Search 'cdpath' for relative directory
 * names.
 */
    పూర్ణాంకం
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    పూర్ణాంకం		r;
    char_u	*file_గమ్యం_కనుగొను = ఖాళీ;
    అక్షరం	*శోధన_ctx = ఖాళీ;

    dir_name = కనుగొను_directory_లోపల_మార్గం(new_dir, (పూర్ణాంకం)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_గమ్యం_కనుగొను, &శోధన_ctx);
    vim_free(file_గమ్యం_కనుగొను);
    vim_findfile_cleanup(శోధన_ctx);
    ఒకవేళ (dir_name == ఖాళీ)
	తిరిగి -1;
    r = mch_chdir((అక్షరం *)dir_name);
    vim_free(dir_name);
    తిరిగి r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
get_user_name(char_u *buf, పూర్ణాంకం len)
{
    ఒకవేళ (username == ఖాళీ)
    {
	ఒకవేళ (mch_get_user_name(buf, len) == FAIL)
	    తిరిగి FAIL;
	username = vim_strsave(buf);
    }
    లేకపోతే
	vim_strncpy(buf, username, len - 1);
    తిరిగి OK;
}

#ఒకవేళ defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    శూన్య
free_username(శూన్య)
{
    vim_free(username);
}
#అయితే_ముగింపు

#ఒకవేళ !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    శూన్య
qsort(
    శూన్య	*పునాది,
    size_t	elm_లెక్క,
    size_t	elm_కొలత,
    పూర్ణాంకం (*cmp)(స్థిరరాశి శూన్య *, స్థిరరాశి శూన్య *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		gap;

    buf = alloc(elm_కొలత);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    కొరకు (gap = elm_లెక్క / 2; gap > 0; gap /= 2)
	కొరకు (i = gap; i < elm_లెక్క; ++i)
	    కొరకు (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)పునాది + j * elm_కొలత;
		p2 = (char_u *)పునాది + (j + gap) * elm_కొలత;
		ఒకవేళ ((*cmp)((శూన్య *)p1, (శూన్య *)p2) <= 0)
		    ఆపు;
		// Exchange the elements.
		mch_memmove(buf, p1, elm_కొలత);
		mch_memmove(p1, p2, elm_కొలత);
		mch_memmove(p2, buf, elm_కొలత);
	    }

    vim_free(buf);
}
#అయితే_ముగింపు

/*
 * The putenv() implementation below comes from the "screen" program.
 * Included with permission from Juergen Weigert.
 * See pty.c for the copyright notice.
 */

/*
 *  putenv  --	put value into environment
 *
 *  Usage:  i = putenv (string)
 *    int i;
 *    char  *string;
 *
 *  where string is of the form <name>=<value>.
 *  Putenv returns 0 normally, -1 on error (not enough core for malloc).
 *
 *  Putenv may need to add a new name into the environment, or to
 *  associate a value longer than the current value with a particular
 *  name.  So, to make life simpler, putenv() copies your entire
 *  environment into the heap (i.e. malloc()) from the stack
 *  (i.e. where it resides when your process is initiated) the first
 *  time you call it.
 *
 *  (history removed, not very interesting.  See the "screen" sources.)
 */

#ఒకవేళ !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

స్థిర పూర్ణాంకం  envsize = -1;	// current size of environment
బాహ్య అక్షరం **environ;		// the global which is your env.

స్థిర పూర్ణాంకం  findenv(అక్షరం *name); // look for a name in the env.
స్థిర పూర్ణాంకం  newenv(శూన్య);	// copy env. from stack to heap
స్థిర పూర్ణాంకం  moreenv(శూన్య);	// incr. size of env.

    పూర్ణాంకం
putenv(స్థిరరాశి అక్షరం *string)
{
    పూర్ణాంకం	    i;
    అక్షరం    *p;

    ఒకవేళ (envsize < 0)
    {				// first time putenv called
	ఒకవేళ (newenv() < 0)	// copy env. to heap
	    తిరిగి -1;
    }

    i = findenv((అక్షరం *)string); // look for name in environment

    ఒకవేళ (i < 0)
    {				// name must be added
	కొరకు (i = 0; environ[i]; i++);
	ఒకవేళ (i >= (envsize - 1))
	{			// need new slot
	    ఒకవేళ (moreenv() < 0)
		తిరిగి -1;
	}
	p = alloc(అక్షరశ్రేణి_పొడవు(string) + 1);
	ఒకవేళ (p == ఖాళీ)		// not enough core
	    తిరిగి -1;
	environ[i + 1] = 0;	// new end of env.
    }
    లేకపోతే
    {				// name already in env.
	p = vim_realloc(environ[i], అక్షరశ్రేణి_పొడవు(string) + 1);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
findenv(అక్షరం *name)
{
    అక్షరం    *namechar, *envchar;
    పూర్ణాంకం	    i, దొరికింది;

    దొరికింది = 0;
    కొరకు (i = 0; environ[i] && !దొరికింది; i++)
    {
	envchar = environ[i];
	namechar = name;
	అయితే (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	దొరికింది = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    తిరిగి దొరికింది ? i - 1 : -1;
}

    స్థిర పూర్ణాంకం
newenv(శూన్య)
{
    అక్షరం    **env, *elem;
    పూర్ణాంకం	    i, esize;

    కొరకు (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(అక్షరం *, esize);
    ఒకవేళ (env == ఖాళీ)
	తిరిగి -1;

    కొరకు (i = 0; environ[i]; i++)
    {
	elem = alloc(అక్షరశ్రేణి_పొడవు(environ[i]) + 1);
	ఒకవేళ (elem == ఖాళీ)
	    తిరిగి -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
moreenv(శూన్య)
{
    పూర్ణాంకం	    esize;
    అక్షరం    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((అక్షరం *)environ, esize * పరిమాణం (*env));
    ఒకవేళ (env == 0)
	తిరిగి -1;
    environ = env;
    envsize = esize;
    తిరిగి 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(స్థిరరాశి char_u *string)
{
    పూర్ణాంకం i;
    char_u *p;

    ఒకవేళ (envsize < 0)
	తిరిగి ఖాళీ;

    i = findenv((అక్షరం *)string);

    ఒకవేళ (i < 0)
	తిరిగి ఖాళీ;

    p = vim_strchr((char_u *)environ[i], '=');
    తిరిగి (p + 1);
}
# endif

#అయితే_ముగింపు // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    పూర్ణాంకం
filewritable(char_u *fname)
{
    పూర్ణాంకం		retval = 0;
# ఒకవేళ defined(UNIX) || defined(VMS)
    పూర్ణాంకం		perm = 0;
# endif

# ఒకవేళ defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    ఒకవేళ (
# ifdef MSWIN
	    mch_writable(fname) &&
# లేకపోతే
#  ఒకవేళ defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((అక్షరం *)fname, W_OK) == 0
       )
    {
	++retval;
	ఒకవేళ (mch_isdir(fname))
	    ++retval;
    }
    తిరిగి retval;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    పూర్ణాంకం
get2c(FILE *fd)
{
    పూర్ణాంకం		c, n;

    n = getc(fd);
    ఒకవేళ (n == EOF) తిరిగి -1;
    c = getc(fd);
    ఒకవేళ (c == EOF) తిరిగి -1;
    తిరిగి (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    పూర్ణాంకం
get3c(FILE *fd)
{
    పూర్ణాంకం		c, n;

    n = getc(fd);
    ఒకవేళ (n == EOF) తిరిగి -1;
    c = getc(fd);
    ఒకవేళ (c == EOF) తిరిగి -1;
    n = (n << 8) + c;
    c = getc(fd);
    ఒకవేళ (c == EOF) తిరిగి -1;
    తిరిగి (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    పూర్ణాంకం
get4c(FILE *fd)
{
    పూర్ణాంకం		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    గుర్తులేని	n;

    c = getc(fd);
    ఒకవేళ (c == EOF) తిరిగి -1;
    n = (గుర్తులేని)c;
    c = getc(fd);
    ఒకవేళ (c == EOF) తిరిగి -1;
    n = (n << 8) + (గుర్తులేని)c;
    c = getc(fd);
    ఒకవేళ (c == EOF) తిరిగి -1;
    n = (n << 8) + (గుర్తులేని)c;
    c = getc(fd);
    ఒకవేళ (c == EOF) తిరిగి -1;
    n = (n << 8) + (గుర్తులేని)c;
    తిరిగి (పూర్ణాంకం)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, పూర్ణాంకం cnt)
{
    char_u	*str;
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;

    // allocate memory
    str = alloc(cnt + 1);
    ఒకవేళ (str == ఖాళీ)
	తిరిగి ఖాళీ;

    // Read the string.  Quit when running into the EOF.
    కొరకు (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	ఒకవేళ (c == EOF)
	{
	    vim_free(str);
	    తిరిగి ఖాళీ;
	}
	str[i] = c;
    }
    str[i] = NUL;
    తిరిగి str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    పూర్ణాంకం
put_బైట్లు(FILE *fd, long_u nr, పూర్ణాంకం len)
{
    పూర్ణాంకం	    i;

    కొరకు (i = len - 1; i >= 0; --i)
	ఒకవేళ (putc((పూర్ణాంకం)(nr >> (i * 8)), fd) == EOF)
	    తిరిగి FAIL;
    తిరిగి OK;
}

#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    దీర్ఘ
elapsed(నిర్మాణం timeval *ప్రారంభం_tv)
{
    నిర్మాణం timeval  now_tv;

    gettimeofday(&now_tv, ఖాళీ);
    తిరిగి (now_tv.tv_sec - ప్రారంభం_tv->tv_sec) * 1000L
	 + (now_tv.tv_usec - ప్రారంభం_tv->tv_usec) / 1000L;
}
# endif

# ifdef ELAPSED_TICKCOUNT
/*
 * Return time in msec since "start_tick".
 */
    దీర్ఘ
elapsed(DWORD ప్రారంభం_tick)
{
    DWORD	now = Get_Tick_లెక్క();

    తిరిగి (దీర్ఘ)now - (దీర్ఘ)ప్రారంభం_tick;
}
# endif
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_పని_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    పూర్ణాంకం
mch_parse_cmd(char_u *cmd, పూర్ణాంకం use_shcf, అక్షరం ***argv, పూర్ణాంకం *argc)
{
    పూర్ణాంకం		i;
    char_u	*p, *d;
    పూర్ణాంకం		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    కొరకు (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	అయితే (*p != NUL)
	{
	    ఒకవేళ (i == 2)
		(*argv)[*argc] = (అక్షరం *)p;
	    ++*argc;
	    d = p;
	    అయితే (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		ఒకవేళ (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		లేకపోతే
		{
		    ఒకవేళ (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    ఒకవేళ (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    ఒకవేళ (*p == NUL)
	    {
		ఒకవేళ (i == 2)
		    *d++ = NUL;
		ఆపు;
	    }
	    ఒకవేళ (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	ఒకవేళ (*argv == ఖాళీ)
	{
	    ఒకవేళ (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		కొరకు (;;)
		{
		    p = skiptowhite(p);
		    ఒకవేళ (*p == NUL)
			ఆపు;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(అక్షరం *, *argc + 4);
	    ఒకవేళ (*argv == ఖాళీ)	    // out of memory
		తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    పూర్ణాంకం
build_argv_from_string(char_u *cmd, అక్షరం ***argv, పూర్ణాంకం *argc)
{
    char_u	*cmd_copy;
    పూర్ణాంకం		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    ఒకవేళ (cmd_copy == ఖాళీ
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	తిరిగి FAIL;
    }
    కొరకు (i = 0; i < *argc; i++)
	(*argv)[i] = (అక్షరం *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = ఖాళీ;
    vim_free(cmd_copy);
    తిరిగి OK;
}

#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    పూర్ణాంకం
build_argv_from_list(list_T *l, అక్షరం ***argv, పూర్ణాంకం *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(అక్షరం *, l->lv_len + 1);
    ఒకవేళ (*argv == ఖాళీ)
	తిరిగి FAIL;
    *argc = 0;
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ఒకవేళ (s == ఖాళీ)
	{
	    పూర్ణాంకం i;

	    కొరకు (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = ఖాళీ;
	    తిరిగి FAIL;
	}
	(*argv)[*argc] = (అక్షరం *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = ఖాళీ;
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Change the behavior of vterm.
 * 0: As usual.
 * 1: Windows 10 version 1809
 *      The bug causes unstable handling of ambiguous width character.
 * 2: Windows 10 version 1903 & 1909
 *      Use the wrong result because each result is different.
 * 3: Windows 10 insider preview (current latest logic)
 */
    పూర్ణాంకం
get_special_pty_type(శూన్య)
{
#నిర్వచితమైతే MSWIN
    తిరిగి get_conpty_type();
#లేకపోతే
    తిరిగి 0;
#అయితే_ముగింపు
}

// compare two keyvalue_T structs by case sensitive value
    పూర్ణాంకం
cmp_keyvalue_విలువ(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    తిరిగి STRCMP(kv1->విలువ.string, kv2->విలువ.string);
}

// compare two keyvalue_T structs by value with length
    పూర్ణాంకం
cmp_keyvalue_విలువ_n(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    తిరిగి STRNCMP(kv1->విలువ.string, kv2->విలువ.string, MAX(kv1->విలువ.నిడివి,
		kv2->విలువ.నిడివి));
}

// compare two keyvalue_T structs by case insensitive value
    పూర్ణాంకం
cmp_keyvalue_విలువ_i(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    తిరిగి STRICMP(kv1->విలువ.string, kv2->విలువ.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    పూర్ణాంకం
cmp_keyvalue_విలువ_ni(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    తిరిగి vim_strnicmp_asc((అక్షరం *)kv1->విలువ.string,
	    (అక్షరం *)kv2->విలువ.string, MAX(kv1->విలువ.నిడివి,
		    kv2->విలువ.నిడివి));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    శూన్య *
mergesort_list(
    శూన్య	*తల,
    శూన్య	*(*get_తదుపరి)(శూన్య *),
    శూన్య	(*సమితి_తదుపరి)(శూన్య *, శూన్య *),
    శూన్య	*(*get_prev)(శూన్య *),
    శూన్య	(*సమితి_prev)(శూన్య *, శూన్య *),
    పూర్ణాంకం		(*compare)(స్థిరరాశి శూన్య *, స్థిరరాశి శూన్య *))
{
    ఒకవేళ (!తల || !get_తదుపరి(తల))
	తిరిగి తల;

    // Count length
    పూర్ణాంకం	    n = 0;
    శూన్య*   curr = తల;
    అయితే (curr)
    {
	n++;
	curr = get_తదుపరి(curr);
    }

    పూర్ణాంకం	కొలత;
    కొరకు (కొలత = 1; కొలత < n; కొలత *= 2)
    {
	శూన్య*	new_తల = ఖాళీ;
	శూన్య*	తోక = ఖాళీ;
	curr = తల;

	అయితే (curr)
	{
	    // Split two runs
	    శూన్య    *ఎడమ_2 = curr;
	    శూన్య    *కుడి_2 = ఎడమ_2;
	    పూర్ణాంకం	    i;
	    కొరకు (i = 0; i < కొలత && కుడి_2; ++i)
		కుడి_2 = get_తదుపరి(కుడి_2);

	    శూన్య    *తదుపరి = కుడి_2;
	    కొరకు (i = 0; i < కొలత && తదుపరి; ++i)
		తదుపరి = get_తదుపరి(తదుపరి);

	    // Break links
	    శూన్య    *l_చివర = కుడి_2 ? get_prev(కుడి_2) : ఖాళీ;
	    ఒకవేళ (l_చివర)
		సమితి_తదుపరి(l_చివర, ఖాళీ);
	    ఒకవేళ (కుడి_2)
		సమితి_prev(కుడి_2, ఖాళీ);

	    శూన్య    *r_చివర = తదుపరి ? get_prev(తదుపరి) : ఖాళీ;
	    ఒకవేళ (r_చివర)
		సమితి_తదుపరి(r_చివర, ఖాళీ);
	    ఒకవేళ (తదుపరి)
		సమితి_prev(తదుపరి, ఖాళీ);

	    //విలీనం
	    శూన్య    *merged = ఖాళీ;
	    శూన్య    *merged_తోక = ఖాళీ;

	    అయితే (ఎడమ_2 || కుడి_2)
	    {
		శూన్య	*chosen = ఖాళీ;
		ఒకవేళ (!ఎడమ_2)
		{
		    chosen = కుడి_2;
		    కుడి_2 = get_తదుపరి(కుడి_2);
		}
		లేకపోతే ఒకవేళ (!కుడి_2)
		{
		    chosen = ఎడమ_2;
		    ఎడమ_2 = get_తదుపరి(ఎడమ_2);
		}
		లేకపోతే ఒకవేళ (compare(ఎడమ_2, కుడి_2) <= 0)
		{
		    chosen = ఎడమ_2;
		    ఎడమ_2 = get_తదుపరి(ఎడమ_2);
		}
		లేకపోతే
		{
		    chosen = కుడి_2;
		    కుడి_2 = get_తదుపరి(కుడి_2);
		}

		ఒకవేళ (merged_తోక)
		{
		    సమితి_తదుపరి(merged_తోక, chosen);
		    సమితి_prev(chosen, merged_తోక);
		    merged_తోక = chosen;
		}
		లేకపోతే
		{
		    merged = merged_తోక = chosen;
		    సమితి_prev(chosen, ఖాళీ);
		}
	    }

	    // Connect to full list
	    ఒకవేళ (!new_తల)
		new_తల = merged;
	    లేకపోతే
	    {
		సమితి_తదుపరి(తోక, merged);
		సమితి_prev(merged, తోక);
	    }

	    // Move tail to end
	    అయితే (get_తదుపరి(merged_తోక))
		merged_తోక = get_తదుపరి(merged_తోక);
	    తోక = merged_తోక;

	    curr = తదుపరి;
	}

	తల = new_తల;
    }

    తిరిగి తల;
}

స్థిర స్థిరరాశి char_u base64_పట్టిక[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

స్థిర char_u base64_dec_పట్టిక[256];

#నిర్వచించు BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    స్థిర శూన్య
init_base64_dec_పట్టిక(శూన్య)
{
    స్థిర పూర్ణాంకం base64_dec_tbl_initialized = FALSE;

    ఒకవేళ (base64_dec_tbl_initialized)
	తిరిగి;

    vim_memset(base64_dec_పట్టిక, 0xFF, పరిమాణం(base64_dec_పట్టిక));
    కొరకు (size_t i = 0; i < పరిమాణం(base64_పట్టిక) - 1; i++)
	base64_dec_పట్టిక[(char_u)base64_పట్టిక[i]] = (char_u)i;
    base64_dec_పట్టిక['='] = 0;

    base64_dec_tbl_initialized = TRUE;
}

/*
 * Base64 encode "src[len]" into caller-supplied "dst", which must have room for
 * at least BASE64_ENCODED_LEN(len) bytes (NUL terminated). Returns the number
 * of bytes written, excluding the NUL.
 */
    దీర్ఘ
base64_encode_buf(char_u *dst, స్థిరరాశి char_u *src, size_t len)
{
    size_t i, j;

    కొరకు (i = 0, j = 0; i < len;)
    {
	size_t మిగిలిన = len - i;

	int_u octet_a = src[i++];
	int_u octet_b = మిగిలిన > 1 ? src[i++] : 0;
	int_u octet_c = మిగిలిన > 2 ? src[i++] : 0;

	int_u triple = (octet_a << 16) | (octet_b << 8) | octet_c;

	dst[j++] = base64_పట్టిక[(triple >> 18) & 0x3F];
	dst[j++] = base64_పట్టిక[(triple >> 12) & 0x3F];
	dst[j++] = మిగిలిన > 1
	    ? base64_పట్టిక[(triple >> 6) & 0x3F]
	    : '=';
	dst[j++] = మిగిలిన > 2
	    ? base64_పట్టిక[triple & 0x3F]
	    : '=';
    }

    dst[j] = NUL;
    తిరిగి (దీర్ఘ)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(స్థిరరాశి char_u *దత్తాంశం, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    ఒకవేళ (encoded == ఖాళీ)
	తిరిగి ఖాళీ;
    base64_encode_buf(encoded, దత్తాంశం, len);
    తిరిగి encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    పూర్ణాంకం
base64_decode(స్థిరరాశి char_u *దత్తాంశం, size_t len, garray_T *బహిర్గతం)
{
    ఒకవేళ (len == 0)
	తిరిగి OK;

    ఒకవేళ (len % 4 != 0)
    {
	semsg(_(e_invalid_వాదం_str), దత్తాంశం);
	తిరిగి FAIL;
    }

    init_base64_dec_పట్టిక();

    size_t decoded_len = (len / 4) * 3;
    ఒకవేళ (దత్తాంశం[len - 1] == '=')
	decoded_len--;
    ఒకవేళ (దత్తాంశం[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    కొరకు (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_పట్టిక[(char_u)దత్తాంశం[i++]];
	int_u sextet_b = base64_dec_పట్టిక[(char_u)దత్తాంశం[i++]];
	int_u sextet_c = base64_dec_పట్టిక[(char_u)దత్తాంశం[i++]];
	int_u sextet_d = base64_dec_పట్టిక[(char_u)దత్తాంశం[i++]];

	ఒకవేళ (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_వాదం_str), దత్తాంశం);
	    ga_clear(బహిర్గతం);
	    తిరిగి FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	ఒకవేళ (j < decoded_len)
	{
	    ga_append(బహిర్గతం, (triple >> 16) & 0xFF);
	    j++;
	}
	ఒకవేళ (j < decoded_len)
	{
	    ga_append(బహిర్గతం, (triple >> 8) & 0xFF);
	    j++;
	}
	ఒకవేళ (j < decoded_len)
	{
	    ga_append(బహిర్గతం, triple & 0xFF);
	    j++;
	}

	ఒకవేళ (j == decoded_len)
	{
	    ఒకవేళ ((దత్తాంశం[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((దత్తాంశం[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_వాదం_str), దత్తాంశం);
		ga_clear(బహిర్గతం);
		తిరిగి FAIL;
	    }
	}
    }
    తిరిగి OK;
}
