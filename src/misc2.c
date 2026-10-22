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
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ char_u	*username = ਨਿਰਾ; // cached result of mch_get_user_name()

ਸਥਿਰ ਪੂਰਨ coladvance2(pos_T *pos, ਪੂਰਨ addspaces, ਪੂਰਨ finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    ਪੂਰਨ
virtual_ਸਰਗਰਮ(ਖਾਲੀ)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cur_ve_flags = get_ve_flags();

    ਜੇ (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_ਪਾਓ) && (State & ਬਹੁਲਕ_ਪਾਓ)))
	ਵਾਪਸ TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    ਜੇ (virtual_op != MAYBE)
	ਵਾਪਸ virtual_op;
    ਵਾਪਸ (cur_ve_flags & VE_ਖੰਡ) && VIsual_ਸਰਗਰਮ && VIsual_ਬਹੁਲਕ == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    ਪੂਰਨ
getviscol(ਖਾਲੀ)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, ਨਿਰਾ, ਨਿਰਾ, GETVCOL_NO_ABOVE);
    ਵਾਪਸ (ਪੂਰਨ)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    ਪੂਰਨ
coladvance_force(colnr_T wcol)
{
    ਪੂਰਨ rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    ਜੇ (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ਨਹੀਂਤਾਂ
	// Virtcol is valid
	ਸੈੱਟ_valid_virtcol(curwin, wcol);
    ਵਾਪਸ rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    ਪੂਰਨ
getviscol2(colnr_T ਖਾਨਾ, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.ਖਾਨਾ = ਖਾਨਾ;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, ਨਿਰਾ, ਨਿਰਾ, GETVCOL_NO_ABOVE);
    ਵਾਪਸ (ਪੂਰਨ)x;
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
    ਪੂਰਨ
coladvance(colnr_T wantcol)
{
    ਪੂਰਨ rc = getvpos(&curwin->w_cursor, wantcol);

    ਜੇ (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ਨਹੀਂਤਾਂ ਜੇ (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	ਸੈੱਟ_valid_virtcol(curwin, wantcol);
    ਵਾਪਸ rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    ਪੂਰਨ
getvpos(pos_T *pos, colnr_T wantcol)
{
    ਵਾਪਸ coladvance2(pos, FALSE, virtual_ਸਰਗਰਮ(), wantcol);
}

    ਸਥਿਰ ਪੂਰਨ
coladvance2(
    pos_T	*pos,
    ਪੂਰਨ		addspaces,	// change the text to achieve our goal?
    ਪੂਰਨ		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    ਪੂਰਨ		idx;
    char_u	*ਸਤਰ;
    ਪੂਰਨ		linelen;
    colnr_T	ਖਾਨਾ = 0;
    ਪੂਰਨ		csize = 0;
    ਪੂਰਨ		one_ਹੋਰ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਪੂਰਨ		ਸਿਰ = 0;
#ਜੇ_ਅੰਤ

    one_ਹੋਰ = (State & ਬਹੁਲਕ_ਪਾਓ)
		    || restart_edit != NUL
		    || (VIsual_ਸਰਗਰਮ && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    ਸਤਰ = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    ਜੇ (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_ਹੋਰ;
	    ਖਾਨਾ = wcol;

	    ਜੇ ((addspaces || finetune) && !VIsual_ਸਰਗਰਮ)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_ਹੋਰ;
		ਜੇ (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ		ਚੌੜਾਈ = curwin->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(curwin);
	chartabsize_T	cts;

	ਜੇ (finetune
		&& curwin->w_p_wrap
		&& curwin->w_ਚੌੜਾਈ != 0
		&& wcol >= (colnr_T)ਚੌੜਾਈ
		&& ਚੌੜਾਈ > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    ਜੇ (csize > 0)
		csize--;

	    ਜੇ (wcol / ਚੌੜਾਈ > (colnr_T)csize / ਚੌੜਾਈ
		    && ((State & ਬਹੁਲਕ_ਪਾਓ) == 0 || (ਪੂਰਨ)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / ਚੌੜਾਈ + 1) * ਚੌੜਾਈ - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, ਸਤਰ, ਸਤਰ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	cts.cts_no_above = true;
#ਜੇ_ਅੰਤ
	ਜਦੋਂ (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &ਸਿਰ, ਨਿਰਾ);
	    MB_PTR_ADV(cts.cts_ptr);
#ਨਹੀਂਤਾਂ
	    csize = lbr_chartabsize_adv(&cts);
#ਜੇ_ਅੰਤ
	    cts.cts_vcol += csize;
	}
	ਖਾਨਾ = cts.cts_vcol;
	idx = (ਪੂਰਨ)(cts.cts_ptr - ਸਤਰ);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	ਜੇ (ਖਾਨਾ > wcol || (!virtual_ਸਰਗਰਮ() && one_ਹੋਰ == 0))
	{
	    idx -= 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= ਸਿਰ;
#ਜੇ_ਅੰਤ
	    ਖਾਨਾ -= csize;
	}

	ਜੇ (virtual_ਸਰਗਰਮ()
		&& addspaces
		&& wcol >= 0
		&& ((ਖਾਨਾ != wcol && ਖਾਨਾ != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    ਜੇ (ਸਤਰ[idx] == NUL)
	    {
		// Append spaces
		ਪੂਰਨ	correct = wcol - ਖਾਨਾ;
		char_u	*newline = alloc(idx + correct + 1);
		ਪੂਰਨ	t;

		ਜੇ (newline == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;

		ਲਈ (t = 0; t < idx; ++t)
		    newline[t] = ਸਤਰ[t];

		ਲਈ (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		ਬਦਲਿਆ_ਬਾਈਟਾਂ(pos->lnum, (colnr_T)idx);
		idx += correct;
		ਖਾਨਾ = wcol;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Break a tab
		ਪੂਰਨ	correct = wcol - ਖਾਨਾ - csize + 1; // negative!!
		char_u	*newline;
		ਪੂਰਨ	t, s = 0;
		ਪੂਰਨ	v;

		ਜੇ (-correct > csize)
		    ਵਾਪਸ FAIL;

		newline = alloc(linelen + csize);
		ਜੇ (newline == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;

		ਲਈ (t = 0; t < linelen; t++)
		{
		    ਜੇ (t != idx)
			newline[s++] = ਸਤਰ[t];
		    ਨਹੀਂਤਾਂ
			ਲਈ (v = 0; v < csize; v++)
			    newline[s++] = ' ';
		}

		newline[linelen + csize - 1] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		ਬਦਲਿਆ_ਬਾਈਟਾਂ(pos->lnum, idx);
		idx += (csize - 1 + correct);
		ਖਾਨਾ += correct;
	    }
	}
    }

    ਜੇ (idx < 0)
	pos->ਖਾਨਾ = 0;
    ਨਹੀਂਤਾਂ
	pos->ਖਾਨਾ = idx;

    pos->coladd = 0;

    ਜੇ (finetune)
    {
	ਜੇ (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    ਜੇ (!one_ਹੋਰ)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, ਨਿਰਾ, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ b = (ਪੂਰਨ)wcol - (ਪੂਰਨ)ਖਾਨਾ;

	    // The difference between wcol and col is used to set coladd.
	    ਜੇ (b > 0 && b < (MAXCOL - 2 * curwin->w_ਚੌੜਾਈ))
		pos->coladd = b;

	    ਖਾਨਾ += b;
	}
    }

    // prevent from moving onto a trail byte
    ਜੇ (has_mbyte)
	mb_adjustpos(curbuf, pos);

    ਜੇ (wcol < 0 || ਖਾਨਾ < wcol)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    ਪੂਰਨ
inc_cursor(ਖਾਲੀ)
{
    ਵਾਪਸ inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    ਪੂਰਨ
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    ਜੇ (lp->ਖਾਨਾ != MAXCOL)
    {
	p = ml_get_pos(lp);
	ਜੇ (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    ਜੇ (has_mbyte)
	    {
		ਪੂਰਨ l = (*mb_ptr2len)(p);

		lp->ਖਾਨਾ += l;
		ਵਾਪਸ ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->ਖਾਨਾ++;
	    lp->coladd = 0;
	    ਵਾਪਸ ((p[1] != NUL) ? 0 : 2);
	}
    }
    ਜੇ (lp->lnum != curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)     // there is a next line
    {
	lp->ਖਾਨਾ = 0;
	lp->lnum++;
	lp->coladd = 0;
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    ਪੂਰਨ
incl(pos_T *lp)
{
    ਪੂਰਨ	    r;

    ਜੇ ((r = inc(lp)) >= 1 && lp->ਖਾਨਾ)
	r = inc(lp);
    ਵਾਪਸ r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    ਪੂਰਨ
dec_cursor(ਖਾਲੀ)
{
    ਵਾਪਸ dec(&curwin->w_cursor);
}

    ਪੂਰਨ
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    ਜੇ (lp->ਖਾਨਾ == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->ਖਾਨਾ = ml_get_len(lp->lnum);
	ਜੇ (has_mbyte)
	    lp->ਖਾਨਾ -= (*mb_ਸਿਰ_off)(p, p + lp->ਖਾਨਾ);
	ਵਾਪਸ 0;
    }

    ਜੇ (lp->ਖਾਨਾ > 0)
    {
	// still within line
	lp->ਖਾਨਾ--;
	ਜੇ (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->ਖਾਨਾ -= (*mb_ਸਿਰ_off)(p, p + lp->ਖਾਨਾ);
	}
	ਵਾਪਸ 0;
    }

    ਜੇ (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->ਖਾਨਾ = ml_get_len(lp->lnum);
	ਜੇ (has_mbyte)
	    lp->ਖਾਨਾ -= (*mb_ਸਿਰ_off)(p, p + lp->ਖਾਨਾ);
	ਵਾਪਸ 1;
    }

    // at start of file
    ਵਾਪਸ -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    ਪੂਰਨ
decl(pos_T *lp)
{
    ਪੂਰਨ	    r;

    ਜੇ ((r = dec(lp)) == 1 && lp->ਖਾਨਾ)
	r = dec(lp);
    ਵਾਪਸ r;
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (hasAnyFolding(wp))
    {
	ਜੇ (lnum > cursor)
	{
	    ਜਦੋਂ (lnum > cursor)
	    {
		(ਖਾਲੀ)hasFoldingWin(wp, lnum, &lnum, ਨਿਰਾ, TRUE, ਨਿਰਾ);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		ਜੇ (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (lnum < cursor)
	{
	    ਜਦੋਂ (lnum < cursor)
	    {
		(ਖਾਲੀ)hasFoldingWin(wp, lnum, ਨਿਰਾ, &lnum, TRUE, ਨਿਰਾ);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		ਜੇ (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	retval = lnum - cursor;

    ਵਾਪਸ retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    ਖਾਲੀ
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    ਜੇ (pos->lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	pos->lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ (pos->ਖਾਨਾ > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	ਜੇ (pos->ਖਾਨਾ > len)
	    pos->ਖਾਨਾ = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    ਖਾਲੀ
check_cursor_lnum(ਖਾਲੀ)
{
    ਜੇ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	ਜੇ (!hasFolding(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
						&curwin->w_cursor.lnum, ਨਿਰਾ))
#ਜੇ_ਅੰਤ
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    }
    ਜੇ (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    ਖਾਲੀ
check_cursor_ਖਾਨਾ(ਖਾਲੀ)
{
    check_cursor_ਖਾਨਾ_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    ਖਾਲੀ
check_cursor_ਖਾਨਾ_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.ਖਾਨਾ;
    colnr_T      oldcoladd = win->w_cursor.ਖਾਨਾ + win->w_cursor.coladd;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_ਬਫਰ, win->w_cursor.lnum);
    ਜੇ (len == 0)
	win->w_cursor.ਖਾਨਾ = 0;
    ਨਹੀਂਤਾਂ ਜੇ (win->w_cursor.ਖਾਨਾ >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) || restart_edit
		|| (VIsual_ਸਰਗਰਮ && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_ਸਰਗਰਮ())
	    win->w_cursor.ਖਾਨਾ = len;
	ਨਹੀਂਤਾਂ
	{
	    win->w_cursor.ਖਾਨਾ = len - 1;
	    // Move the cursor to the head byte.
	    ਜੇ (has_mbyte)
		mb_adjustpos(win->w_ਬਫਰ, &win->w_cursor);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (win->w_cursor.ਖਾਨਾ < 0)
	win->w_cursor.ਖਾਨਾ = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    ਜੇ (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    ਨਹੀਂਤਾਂ ਜੇ (cur_ve_flags == VE_ALL)
    {
	ਜੇ (oldcoladd > win->w_cursor.ਖਾਨਾ)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.ਖਾਨਾ;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    ਜੇ (win->w_cursor.ਖਾਨਾ + 1 < len)
	    {
		ਪੂਰਨ cs, ce;

		getvcol(win, &win->w_cursor, &cs, ਨਿਰਾ, &ce, 0);
		ਜੇ (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	ਨਹੀਂਤਾਂ
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    ਖਾਲੀ
check_cursor(ਖਾਲੀ)
{
    check_cursor_lnum();
    check_cursor_ਖਾਨਾ();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    ਖਾਲੀ
check_visual_pos(ਖਾਲੀ)
{
    ਜੇ (VIsual.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	VIsual.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	VIsual.ਖਾਨਾ = 0;
	VIsual.coladd = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ len = ml_get_len(VIsual.lnum);

	ਜੇ (VIsual.ਖਾਨਾ > len)
	{
	    VIsual.ਖਾਨਾ = len;
	    VIsual.coladd = 0;
	}
    }
}

/*
 * Make sure curwin->w_cursor is not on the NUL at the end of the line.
 * Allow it when in Visual mode and 'selection' is not "old".
 */
    ਖਾਲੀ
adjust_cursor_ਖਾਨਾ(ਖਾਲੀ)
{
    ਜੇ (curwin->w_cursor.ਖਾਨਾ > 0
	    && (!VIsual_ਸਰਗਰਮ || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.ਖਾਨਾ;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    ਪੂਰਨ
ਸੈੱਟ_leftcol(colnr_T leftcol)
{
    ਪੂਰਨ		retval = FALSE;

    // Return quickly when there is no change.
    ਜੇ (curwin->w_leftcol == leftcol)
	ਵਾਪਸ FALSE;
    curwin->w_leftcol = leftcol;

    ਬਦਲਿਆ_cline_bef_curs();
    ਲੰਮਾ lastcol = curwin->w_leftcol + curwin->w_ਚੌੜਾਈ - curwin_ਖਾਨਾ_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    ਲੰਮਾ siso = get_sidescrolloff_ਮੁੱਲ();
    ਜੇ (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(ਖਾਲੀ)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, ਨਿਰਾ, &e, 0);
    ਜੇ (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    ਨਹੀਂਤਾਂ ਜੇ (s < curwin->w_leftcol)
    {
	retval = TRUE;
	ਜੇ (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    ਬਦਲਿਆ_cline_bef_curs();
	}
    }

    ਜੇ (retval)
	curwin->w_ਸੈੱਟ_curswant = true;
    redraw_later(UPD_NOT_VALID);
    ਵਾਪਸ retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    ਪੂਰਨ
copy_option_part(
    char_u	**option,
    char_u	*buf,
    ਪੂਰਨ		maxlen,
    ਅੱਖਰ	*sep_chars)
{
    ਪੂਰਨ	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    ਜੇ (*p == '.')
	buf[len++] = *p++;
    ਜਦੋਂ (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == ਨਿਰਾ)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	ਜੇ (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != ਨਿਰਾ)
	    ++p;
	ਜੇ (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    ਜੇ (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_ਮੰਜ਼ਿਲ_option_part(p);	// p points to next file name

    *option = p;
    ਵਾਪਸ len;
}

#ਜੇ !defined(HAVE_MEMSET) && !defined(PROTO)
    ਖਾਲੀ *
vim_memset(ਖਾਲੀ *ptr, ਪੂਰਨ c, size_t ਮਾਪ)
{
    ਅੱਖਰ *p = ptr;

    ਜਦੋਂ (ਮਾਪ-- > 0)
	*p++ = c;
    ਵਾਪਸ ptr;
}
#ਜੇ_ਅੰਤ

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    ਪੂਰਨ
vim_isspace(ਪੂਰਨ x)
{
    ਵਾਪਸ ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

ਸਥਿਰ ਬਣਤਰ modmasktable
{
    ਛੋਟਾ	mod_ਨਕਾਬ;	// Bit-mask for particular key modifier
    ਛੋਟਾ	mod_flag;	// Bit(s) for particular key modifier
    char_u	name;		// Single letter name of modifier
} mod_ਨਕਾਬ_ਸਾਰਣੀ[] =
{
    {MOD_ਨਕਾਬ_ALT,		MOD_ਨਕਾਬ_ALT,		(char_u)'M'},
    {MOD_ਨਕਾਬ_META,		MOD_ਨਕਾਬ_META,		(char_u)'T'},
    {MOD_ਨਕਾਬ_CTRL,		MOD_ਨਕਾਬ_CTRL,		(char_u)'C'},
    {MOD_ਨਕਾਬ_ਖਿਸਕਾਓ,		MOD_ਨਕਾਬ_ਖਿਸਕਾਓ,		(char_u)'S'},
    {MOD_ਨਕਾਬ_MULTI_CLICK,	MOD_ਨਕਾਬ_2_CLICK,	(char_u)'2'},
    {MOD_ਨਕਾਬ_MULTI_CLICK,	MOD_ਨਕਾਬ_3_CLICK,	(char_u)'3'},
    {MOD_ਨਕਾਬ_MULTI_CLICK,	MOD_ਨਕਾਬ_4_CLICK,	(char_u)'4'},
#ਜੇ defined(MACOS_X) || defined(FEAT_GUI_GTK)
    {MOD_ਨਕਾਬ_CMD,		MOD_ਨਕਾਬ_CMD,		(char_u)'D'},
#ਜੇ_ਅੰਤ
    // 'A' must be the last one
    {MOD_ਨਕਾਬ_ALT,		MOD_ਨਕਾਬ_ALT,		(char_u)'A'},
    {0, 0, NUL}
    // NOTE: when adding an entry, update MAX_KEY_NAME_LEN!
};

/*
 * Shifted key terminal codes and their unshifted equivalent.
 * Don't add mouse codes here, they are handled separately!
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD_ਕੁੰਜੀਆਂ_ਇੰਦਰਾਜ_ਮਾਪ 5

ਸਥਿਰ char_u modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[] =
{
//  mod mask	    with modifier		without modifier
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '&', '9',			'@', '1',	// begin
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '&', '0',			'@', '2',	// cancel
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '*', '1',			'@', '4',	// command
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '*', '2',			'@', '5',	// copy
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '*', '3',			'@', '6',	// create
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '*', '4',			'k', 'D',	// delete char
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '*', '5',			'k', 'L',	// delete line
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '*', '7',			'@', '7',	// end
    MOD_ਨਕਾਬ_CTRL,  KS_EXTRA, (ਪੂਰਨ)KE_C_ਸਿਰਾ,	'@', '7',	// end
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '*', '9',			'@', '9',	// exit
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '*', '0',			'@', '0',	//ਲੱਭੋ
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '#', '1',			'%', '1',	// help
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '#', '2',			'k', 'h',	// home
    MOD_ਨਕਾਬ_CTRL,  KS_EXTRA, (ਪੂਰਨ)KE_C_HOME,	'k', 'h',	// home
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '#', '3',			'k', 'I',	// insert
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '#', '4',			'k', 'l',	// left arrow
    MOD_ਨਕਾਬ_CTRL,  KS_EXTRA, (ਪੂਰਨ)KE_C_ਖੱਬਾ,	'k', 'l',	// left arrow
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'a',			'%', '3',	//ਸੁਨੇਹਾ
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'b',			'%', '4',	// move
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'c',			'%', '5',	//ਅਗਲਾ
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'd',			'%', '7',	// options
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'e',			'%', '8',	//ਪਿਛਲਾ
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'f',			'%', '9',	// print
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'g',			'%', '0',	// redo
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'h',			'&', '3',	// replace
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'i',			'k', 'r',	// right arr.
    MOD_ਨਕਾਬ_CTRL,  KS_EXTRA, (ਪੂਰਨ)KE_C_ਸੱਜਾ,	'k', 'r',	// right arr.
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '%', 'j',			'&', '5',	// resume
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '!', '1',			'&', '6',	// save
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '!', '2',			'&', '7',	// suspend
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, '!', '3',			'&', '8',	// undo
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_ਉੱਪਰ,	'k', 'u',	// up arrow
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_ਥੱਲੇ,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_XF1,	KS_EXTRA, (ਪੂਰਨ)KE_XF1,
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_XF2,	KS_EXTRA, (ਪੂਰਨ)KE_XF2,
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_XF3,	KS_EXTRA, (ਪੂਰਨ)KE_XF3,
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_XF4,	KS_EXTRA, (ਪੂਰਨ)KE_XF4,

    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F1,	'k', '1',	// F1
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F2,	'k', '2',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F3,	'k', '3',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F4,	'k', '4',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F5,	'k', '5',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F6,	'k', '6',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F7,	'k', '7',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F8,	'k', '8',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F9,	'k', '9',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F10,	'k', ';',	// F10

    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F11,	'F', '1',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F12,	'F', '2',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F13,	'F', '3',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F14,	'F', '4',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F15,	'F', '5',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F16,	'F', '6',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F17,	'F', '7',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F18,	'F', '8',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F19,	'F', '9',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F20,	'F', 'A',

    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F21,	'F', 'B',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F22,	'F', 'C',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F23,	'F', 'D',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F24,	'F', 'E',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F25,	'F', 'F',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F26,	'F', 'G',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F27,	'F', 'H',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F28,	'F', 'I',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F29,	'F', 'J',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F30,	'F', 'K',

    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F31,	'F', 'L',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F32,	'F', 'M',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F33,	'F', 'N',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F34,	'F', 'O',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F35,	'F', 'P',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F36,	'F', 'Q',
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, KS_EXTRA, (ਪੂਰਨ)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_ਨਕਾਬ_ਖਿਸਕਾਓ, 'k', 'B',			KS_EXTRA, (ਪੂਰਨ)KE_TAB,

    NUL
};

ਸਥਿਰ ਬਣਤਰ ਕੁੰਜੀ_name_ਇੰਦਰਾਜ
{
    ਪੂਰਨ		enabled;	    // is this entry available (TRUE/FALSE)?
    ਪੂਰਨ		ਕੁੰਜੀ;		    // special key code or ascii value
    string_T	name;		    // name of key
    ਪੂਰਨ		is_alt;		    // is an alternative name
} ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ[] =
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_DEC
    TRUE,
#ਨਹੀਂਤਾਂ
    FALSE,
#ਜੇ_ਅੰਤ
	K_DEC_MOUSE, STR_LITERAL_INIT("DecMouse"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Del"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Delete"), TRUE},
    {TRUE, K_ਥੱਲੇ, STR_LITERAL_INIT("Down"), FALSE},
    {TRUE, K_DROP, STR_LITERAL_INIT("Drop"), FALSE},
    {TRUE, K_ਸਿਰਾ, STR_LITERAL_INIT("End"), FALSE},
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_JSB
    TRUE,
#ਨਹੀਂਤਾਂ
    FALSE,
#ਜੇ_ਅੰਤ
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
    {TRUE, K_ਖੱਬਾ, STR_LITERAL_INIT("Left"), FALSE},
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_NET
    TRUE,
#ਨਹੀਂਤਾਂ
    FALSE,
#ਜੇ_ਅੰਤ
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_PTERM
    TRUE,
#ਨਹੀਂਤਾਂ
    FALSE,
#ਜੇ_ਅੰਤ
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_ਸੱਜਾ, STR_LITERAL_INIT("Right"), FALSE},
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    TRUE,
#ਨਹੀਂਤਾਂ
    FALSE,
#ਜੇ_ਅੰਤ
	K_SNR, STR_LITERAL_INIT("SNR"), FALSE},
    {TRUE, ' ', STR_LITERAL_INIT("Space"), FALSE},
    {TRUE, TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_UNDO, STR_LITERAL_INIT("Undo"), FALSE},
    {TRUE, K_ਉੱਪਰ, STR_LITERAL_INIT("Up"), FALSE},
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_URXVT
    TRUE,
#ਨਹੀਂਤਾਂ
    FALSE,
#ਜੇ_ਅੰਤ
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
    ਸਥਿਰ ਪੂਰਨ
name_ਮੰਜ਼ਿਲ_mod_ਨਕਾਬ(ਪੂਰਨ c)
{
    ਪੂਰਨ	    i;

    c = TOUPPER_ASC(c);
    ਲਈ (i = 0; mod_ਨਕਾਬ_ਸਾਰਣੀ[i].mod_ਨਕਾਬ != 0; i++)
	ਜੇ (c == mod_ਨਕਾਬ_ਸਾਰਣੀ[i].name)
	    ਵਾਪਸ mod_ਨਕਾਬ_ਸਾਰਣੀ[i].mod_flag;
    ਵਾਪਸ 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    ਪੂਰਨ
simplify_ਕੁੰਜੀ(ਪੂਰਨ ਕੁੰਜੀ, ਪੂਰਨ *modifiers)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    key0;
    ਪੂਰਨ	    key1;

    ਜੇ (!(*modifiers & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_CTRL)))
	ਵਾਪਸ ਕੁੰਜੀ;

    // TAB is a special case
    ਜੇ (ਕੁੰਜੀ == TAB && (*modifiers & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ))
    {
	*modifiers &= ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
	ਵਾਪਸ K_S_TAB;
    }
    key0 = KEY2TERMCAP0(ਕੁੰਜੀ);
    key1 = KEY2TERMCAP1(ਕੁੰਜੀ);
    ਲਈ (i = 0; modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i] != NUL; i += MOD_ਕੁੰਜੀਆਂ_ਇੰਦਰਾਜ_ਮਾਪ)
    {
	ਜੇ (key0 == modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i + 3]
		&& key1 == modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i + 4]
		&& (*modifiers & modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i]))
	{
	    *modifiers &= ~modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i];
	    ਵਾਪਸ TERMCAP2_ਕੁੰਜੀ(modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i + 1],
		    modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i + 2]);
	}
    }
    ਵਾਪਸ ਕੁੰਜੀ;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    ਪੂਰਨ
handle_x_ਕੁੰਜੀਆਂ(ਪੂਰਨ ਕੁੰਜੀ)
{
    ਚੋਣ (ਕੁੰਜੀ)
    {
	ਹਾਲਤ K_XUP:	ਵਾਪਸ K_ਉੱਪਰ;
	ਹਾਲਤ K_XDOWN:	ਵਾਪਸ K_ਥੱਲੇ;
	ਹਾਲਤ K_XLEFT:	ਵਾਪਸ K_ਖੱਬਾ;
	ਹਾਲਤ K_XRIGHT:	ਵਾਪਸ K_ਸੱਜਾ;
	ਹਾਲਤ K_XHOME:	ਵਾਪਸ K_HOME;
	ਹਾਲਤ K_ZHOME:	ਵਾਪਸ K_HOME;
	ਹਾਲਤ K_XEND:	ਵਾਪਸ K_ਸਿਰਾ;
	ਹਾਲਤ K_ZEND:	ਵਾਪਸ K_ਸਿਰਾ;
	ਹਾਲਤ K_XF1:	ਵਾਪਸ K_F1;
	ਹਾਲਤ K_XF2:	ਵਾਪਸ K_F2;
	ਹਾਲਤ K_XF3:	ਵਾਪਸ K_F3;
	ਹਾਲਤ K_XF4:	ਵਾਪਸ K_F4;
	ਹਾਲਤ K_S_XF1:	ਵਾਪਸ K_S_F1;
	ਹਾਲਤ K_S_XF2:	ਵਾਪਸ K_S_F2;
	ਹਾਲਤ K_S_XF3:	ਵਾਪਸ K_S_F3;
	ਹਾਲਤ K_S_XF4:	ਵਾਪਸ K_S_F4;
    }
    ਵਾਪਸ ਕੁੰਜੀ;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_ਕੁੰਜੀ_name(ਪੂਰਨ c, ਪੂਰਨ modifiers)
{
    ਸਥਿਰ char_u string[MAX_ਕੁੰਜੀ_NAME_LEN + 1];
    ਪੂਰਨ	    i, idx, len;
    ਪੂਰਨ	    ਸਾਰਣੀ_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    ਜੇ (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_ਕੁੰਜੀ)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    ਜੇ (IS_SPECIAL(c))
    {
	ਲਈ (i = 0; modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i] != 0; i += MOD_ਕੁੰਜੀਆਂ_ਇੰਦਰਾਜ_ਮਾਪ)
	    ਜੇ (       KEY2TERMCAP0(c) == (ਪੂਰਨ)modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i + 1]
		    && (ਪੂਰਨ)KEY2TERMCAP1(c) == (ਪੂਰਨ)modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i + 2])
	    {
		modifiers |= modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i];
		c = TERMCAP2_ਕੁੰਜੀ(modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i + 3],
						   modifier_ਕੁੰਜੀਆਂ_ਸਾਰਣੀ[i + 4]);
		ਰੋਕੋ;
	    }
    }

    // try to find the key in the special key table
    ਸਾਰਣੀ_idx = ਲੱਭੋ_special_ਕੁੰਜੀ_ਵਿੱਚ_ਸਾਰਣੀ(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    ਜੇ (c > 0 && (*mb_char2len)(c) == 1)
    {
	ਜੇ (ਸਾਰਣੀ_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_ਨਕਾਬ_ALT;
	    // try again, to find the un-alted key in the special key table
	    ਸਾਰਣੀ_idx = ਲੱਭੋ_special_ਕੁੰਜੀ_ਵਿੱਚ_ਸਾਰਣੀ(c);
	}
	ਜੇ (ਸਾਰਣੀ_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_ਨਕਾਬ_CTRL;
	}
    }

    // translate the modifier into a string
    ਲਈ (i = 0; mod_ਨਕਾਬ_ਸਾਰਣੀ[i].name != 'A'; i++)
	ਜੇ ((modifiers & mod_ਨਕਾਬ_ਸਾਰਣੀ[i].mod_ਨਕਾਬ)
						== mod_ਨਕਾਬ_ਸਾਰਣੀ[i].mod_flag)
	{
	    string[idx++] = mod_ਨਕਾਬ_ਸਾਰਣੀ[i].name;
	    string[idx++] = (char_u)'-';
	}

    ਜੇ (ਸਾਰਣੀ_idx < 0)		// unknown special key, may output t_xx
    {
	ਜੇ (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	ਨਹੀਂਤਾਂ
	{
	    len = (*mb_char2len)(c);
	    ਜੇ (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    ਨਹੀਂਤਾਂ
	    {
		char_u	*s = transchar(c);
		ਜਦੋਂ (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    ਨਹੀਂਤਾਂ		// use name of special key
    {
	string_T    *s;

	s = &ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ[ਸਾਰਣੀ_idx].name;

	ਜੇ (s->ਲੰਮਾਈ + idx + 2 <= MAX_ਕੁੰਜੀ_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (ਪੂਰਨ)s->ਲੰਮਾਈ;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    ਵਾਪਸ string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    ਪੂਰਨ
trans_special(
    char_u	**srcp,
    char_u	*dst,
    ਪੂਰਨ		flags,		// FSK_ values
    ਪੂਰਨ		escape_ks,	// escape K_SPECIAL bytes in the character
    ਪੂਰਨ		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    ਪੂਰਨ		modifiers = 0;
    ਪੂਰਨ		ਕੁੰਜੀ;

    ਕੁੰਜੀ = ਲੱਭੋ_special_ਕੁੰਜੀ(srcp, &modifiers, flags, did_simplify);
    ਜੇ (ਕੁੰਜੀ == 0)
	ਵਾਪਸ 0;

    ਵਾਪਸ special_ਮੰਜ਼ਿਲ_buf(ਕੁੰਜੀ, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    ਪੂਰਨ
special_ਮੰਜ਼ਿਲ_buf(ਪੂਰਨ ਕੁੰਜੀ, ਪੂਰਨ modifiers, ਪੂਰਨ escape_ks, char_u *dst)
{
    ਪੂਰਨ		dlen = 0;

    // Put the appropriate modifier in a string
    ਜੇ (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    ਜੇ (IS_SPECIAL(ਕੁੰਜੀ))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(ਕੁੰਜੀ);
	dst[dlen++] = KEY2TERMCAP1(ਕੁੰਜੀ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (escape_ks)
	dlen = (ਪੂਰਨ)(ਜੋੜੋ_char2buf(ਕੁੰਜੀ, dst + dlen) - dst);
    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	dlen += (*mb_char2bytes)(ਕੁੰਜੀ, dst + dlen);
    ਨਹੀਂਤਾਂ
	dst[dlen++] = ਕੁੰਜੀ;

    ਵਾਪਸ dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    ਪੂਰਨ
ਲੱਭੋ_special_ਕੁੰਜੀ(
    char_u	**srcp,
    ਪੂਰਨ		*modp,
    ਪੂਰਨ		flags,		// FSK_ values
    ਪੂਰਨ		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*ਆਖਰੀ_dash;
    char_u	*ਸਿਰਾ_ਦਾ_name;
    char_u	*src;
    char_u	*bp;
    ਪੂਰਨ		ਵਿੱਚ_string = flags & FSK_ਵਿੱਚ_STRING;
    ਪੂਰਨ		modifiers;
    ਪੂਰਨ		ਦੋਆਧਾਰੀ_ਅੰਕ;
    ਪੂਰਨ		ਕੁੰਜੀ;
    uvarnumber_T	n;
    ਪੂਰਨ		l;

    src = *srcp;
    ਜੇ (src[0] != '<')
	ਵਾਪਸ 0;
    ਜੇ (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    ਆਖਰੀ_dash = src;
    ਲਈ (bp = src + 1; *bp == '-' || vim_is_ਸਾਧਾਰਨ_IDc(*bp); bp++)
    {
	ਜੇ (*bp == '-')
	{
	    ਆਖਰੀ_dash = bp;
	    ਜੇ (bp[1] != NUL)
	    {
		ਜੇ (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		ਨਹੀਂਤਾਂ
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		ਜੇ (!(ਵਿੱਚ_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	ਜੇ (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, ਨਿਰਾ, &l, STR2NR_ALL, ਨਿਰਾ, ਨਿਰਾ, 0, TRUE, ਨਿਰਾ);
	    ਜੇ (l == 0)
	    {
		emsg(_(e_invalid_ਦਲੀਲ));
		ਵਾਪਸ 0;
	    }
	    bp += l + 5;
	    ਰੋਕੋ;
	}
    }

    ਜੇ (*bp == '>')	// found matching '>'
    {
	ਸਿਰਾ_ਦਾ_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	ਲਈ (bp = src + 1; bp < ਆਖਰੀ_dash; bp++)
	{
	    ਜੇ (*bp != '-')
	    {
		ਦੋਆਧਾਰੀ_ਅੰਕ = name_ਮੰਜ਼ਿਲ_mod_ਨਕਾਬ(*bp);
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == 0x0)
		    ਰੋਕੋ;	// Illegal modifier name
		modifiers |= ਦੋਆਧਾਰੀ_ਅੰਕ;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	ਜੇ (bp >= ਆਖਰੀ_dash)
	{
	    ਜੇ (STRNICMP(ਆਖਰੀ_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(ਆਖਰੀ_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(ਆਖਰੀ_dash + 6, ਨਿਰਾ, &l, STR2NR_ALL, ਨਿਰਾ,
							    &n, 0, TRUE, ਨਿਰਾ);
		ਜੇ (l == 0)
		{
		    emsg(_(e_invalid_ਦਲੀਲ));
		    ਵਾਪਸ 0;
		}
		ਕੁੰਜੀ = (ਪੂਰਨ)n;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ off = 1;

		// Modifier with single letter, or special key name.
		ਜੇ (ਵਿੱਚ_string && ਆਖਰੀ_dash[1] == '\\' && ਆਖਰੀ_dash[2] == '"')
		    off = 2;
		ਜੇ (has_mbyte)
		    l = mb_ptr2len(ਆਖਰੀ_dash + off);
		ਨਹੀਂਤਾਂ
		    l = 1;
		ਜੇ (modifiers != 0 && ਆਖਰੀ_dash[l + off] == '>')
		    ਕੁੰਜੀ = PTR2CHAR(ਆਖਰੀ_dash + off);
		ਨਹੀਂਤਾਂ
		{
		    ਕੁੰਜੀ = get_special_ਕੁੰਜੀ_ਕੋਡ(ਆਖਰੀ_dash + off);
		    ਜੇ (!(flags & FSK_KEEP_X_ਕੁੰਜੀ))
			ਕੁੰਜੀ = handle_x_ਕੁੰਜੀਆਂ(ਕੁੰਜੀ);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    ਜੇ (ਕੁੰਜੀ != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		ਕੁੰਜੀ = simplify_ਕੁੰਜੀ(ਕੁੰਜੀ, &modifiers);

		ਜੇ ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    ਜੇ (ਕੁੰਜੀ == K_BS)
			ਕੁੰਜੀ = BS;
		    ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ == K_DEL || ਕੁੰਜੀ == K_KDEL)
			ਕੁੰਜੀ = DEL;
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    ਜੇ ((flags & FSK_SIMPLIFY) != 0)
		    {
			ਜੇ (did_simplify != ਨਿਰਾ)
			    *did_simplify = TRUE;
		    }
		    ਨਹੀਂਤਾਂ
			ਕੁੰਜੀ = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		ਜੇ (!IS_SPECIAL(ਕੁੰਜੀ))
		    ਕੁੰਜੀ = ਕੱਢੋ_modifiers(ਕੁੰਜੀ, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = ਸਿਰਾ_ਦਾ_name;
		ਵਾਪਸ ਕੁੰਜੀ;
	    }
	}
    }
    ਵਾਪਸ 0;
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
    ਪੂਰਨ
may_adjust_ਕੁੰਜੀ_for_ctrl(ਪੂਰਨ modifiers, ਪੂਰਨ ਕੁੰਜੀ)
{
    ਜੇ ((modifiers & MOD_ਨਕਾਬ_CTRL) == 0)
	ਵਾਪਸ ਕੁੰਜੀ;

    ਜੇ (ASCII_ISALPHA(ਕੁੰਜੀ))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	check_no_reduce_ਕੁੰਜੀਆਂ();  // may update the no_reduce_keys flag
#ਜੇ_ਅੰਤ
	ਵਾਪਸ no_reduce_ਕੁੰਜੀਆਂ == 0 ? TOUPPER_ASC(ਕੁੰਜੀ) : ਕੁੰਜੀ;
    }
    ਜੇ (ਕੁੰਜੀ == '2')
	ਵਾਪਸ '@';
    ਜੇ (ਕੁੰਜੀ == '6')
	ਵਾਪਸ '^';
    ਜੇ (ਕੁੰਜੀ == '-')
	ਵਾਪਸ '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    ਜੇ (ਕੁੰਜੀ == '$' && (modifiers & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ) == 0)
	ਵਾਪਸ ']';

    ਵਾਪਸ ਕੁੰਜੀ;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    ਪੂਰਨ
may_remove_ਖਿਸਕਾਓ_modifier(ਪੂਰਨ modifiers, ਪੂਰਨ ਕੁੰਜੀ)
{
    ਜੇ ((modifiers == MOD_ਨਕਾਬ_ਖਿਸਕਾਓ
		|| modifiers == (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_ALT)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
		|| modifiers == (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_CMD)
#ਜੇ_ਅੰਤ
		|| modifiers == (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_META))
	    && ((ਕੁੰਜੀ >= '!' && ਕੁੰਜੀ <= '/')
		|| (ਕੁੰਜੀ >= ':' && ਕੁੰਜੀ <= 'Z')
		|| vim_isdigit(ਕੁੰਜੀ)
		|| (ਕੁੰਜੀ >= '[' && ਕੁੰਜੀ <= '`')
		|| (ਕੁੰਜੀ >= '{' && ਕੁੰਜੀ <= '~')))
	ਵਾਪਸ modifiers & ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;

    ਜੇ (modifiers == (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_CTRL)
		&& (ਕੁੰਜੀ == '{' || ਕੁੰਜੀ == '}' || ਕੁੰਜੀ == '|'))
	ਵਾਪਸ modifiers & ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;

    ਵਾਪਸ modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    ਪੂਰਨ
ਕੱਢੋ_modifiers(ਪੂਰਨ ਕੁੰਜੀ, ਪੂਰਨ *modp, ਪੂਰਨ simplify, ਪੂਰਨ *did_simplify)
{
    ਪੂਰਨ	modifiers = *modp;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_X
    // Command-key really special, no fancynest
    ਜੇ (!(modifiers & MOD_ਨਕਾਬ_CMD))
#ਜੇ_ਅੰਤ
    ਜੇ ((modifiers & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ) && ASCII_ISALPHA(ਕੁੰਜੀ))
    {
	ਕੁੰਜੀ = TOUPPER_ASC(ਕੁੰਜੀ);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	ਜੇ (simplify || modifiers == MOD_ਨਕਾਬ_ਖਿਸਕਾਓ
		|| modifiers == (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_ALT)
		|| modifiers == (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_META))
	    modifiers &= ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    ਜੇ ((modifiers & MOD_ਨਕਾਬ_CTRL) && ASCII_ISALPHA(ਕੁੰਜੀ))
	ਕੁੰਜੀ = TOUPPER_ASC(ਕੁੰਜੀ);

    ਜੇ (simplify && (modifiers & MOD_ਨਕਾਬ_CTRL)
	    && ((ਕੁੰਜੀ >= '?' && ਕੁੰਜੀ <= '_') || ASCII_ISALPHA(ਕੁੰਜੀ)))
    {
	ਕੁੰਜੀ = Ctrl_chr(ਕੁੰਜੀ);
	modifiers &= ~MOD_ਨਕਾਬ_CTRL;
	// <C-@> is <Nul>
	ਜੇ (ਕੁੰਜੀ == NUL)
	    ਕੁੰਜੀ = K_ZERO;
	ਜੇ (did_simplify != ਨਿਰਾ)
	    *did_simplify = TRUE;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_X
    // Command-key really special, no fancynest
    ਜੇ (!(modifiers & MOD_ਨਕਾਬ_CMD))
#ਜੇ_ਅੰਤ
    ਜੇ (simplify && (modifiers & MOD_ਨਕਾਬ_ALT) && ਕੁੰਜੀ < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	ਕੁੰਜੀ |= 0x80;
	modifiers &= ~MOD_ਨਕਾਬ_ALT;	// remove the META modifier
	ਜੇ (did_simplify != ਨਿਰਾ)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    ਵਾਪਸ ਕੁੰਜੀ;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    ਪੂਰਨ
ਲੱਭੋ_special_ਕੁੰਜੀ_ਵਿੱਚ_ਸਾਰਣੀ(ਪੂਰਨ c)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ); i++)
	ਜੇ (c == ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ[i].ਕੁੰਜੀ && !ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ[i].is_alt)
	    ਵਾਪਸ ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ[i].enabled ? i : -1;

    ਵਾਪਸ -1;
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
    ਸਥਿਰ ਪੂਰਨ
cmp_ਕੁੰਜੀ_name_ਇੰਦਰਾਜ(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    char_u  *p1 = ((ਬਣਤਰ ਕੁੰਜੀ_name_ਇੰਦਰਾਜ *)a)->name.string;
    char_u  *p2 = ((ਬਣਤਰ ਕੁੰਜੀ_name_ਇੰਦਰਾਜ *)b)->name.string;
    ਪੂਰਨ	    ਨਤੀਜਾ = 0;

    ਜੇ (p1 == p2)
	ਵਾਪਸ 0;

    ਜਦੋਂ (vim_is_ਸਾਧਾਰਨ_IDc(*p1) && *p2 != NUL)
    {
	ਜੇ ((ਨਤੀਜਾ = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    ਰੋਕੋ;
	++p1;
	++p2;
    }

    ਜੇ (ਨਤੀਜਾ == 0)
    {
	ਜੇ (*p2 == NUL)
	{
	    ਜੇ (vim_is_ਸਾਧਾਰਨ_IDc(*p1))
		ਨਤੀਜਾ = 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਨਤੀਜਾ = -1;
	}
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    ਪੂਰਨ
get_special_ਕੁੰਜੀ_ਕੋਡ(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    ਜੇ (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	ਜੇ (ਜੋੜੋ_termcap_ਇੰਦਰਾਜ(string, FALSE) == OK)
	    ਵਾਪਸ TERMCAP2_ਕੁੰਜੀ(name[2], name[3]);
    }
    ਨਹੀਂਤਾਂ
    {
	ਬਣਤਰ ਕੁੰਜੀ_name_ਇੰਦਰਾਜ	ਨਿਸ਼ਾਨਾ;
	ਬਣਤਰ ਕੁੰਜੀ_name_ਇੰਦਰਾਜ	*ਇੰਦਰਾਜ;

	ਨਿਸ਼ਾਨਾ.enabled = TRUE;
	ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
	ਨਿਸ਼ਾਨਾ.name.string = name;
	ਨਿਸ਼ਾਨਾ.name.ਲੰਮਾਈ = 0;

	ਇੰਦਰਾਜ = (ਬਣਤਰ ਕੁੰਜੀ_name_ਇੰਦਰਾਜ *)bsearch(
	    &ਨਿਸ਼ਾਨਾ,
	    &ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ,
	    ARRAY_ਲੰਮਾਈ(ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ),
	    ਆਕਾਰ(ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ[0]),
	    cmp_ਕੁੰਜੀ_name_ਇੰਦਰਾਜ);
	ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ && ਇੰਦਰਾਜ->enabled)
	{
	    ਪੂਰਨ ਕੁੰਜੀ = ਇੰਦਰਾਜ->ਕੁੰਜੀ;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    ਵਾਪਸ ਕੁੰਜੀ == K_TAB ? TAB : ਕੁੰਜੀ;
	}
    }

    ਵਾਪਸ 0;
}

    char_u *
get_ਕੁੰਜੀ_name(ਪੂਰਨ i)
{
    ਜੇ (i < 0 || i >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ))
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ ਕੁੰਜੀ_ਨਾਂ_ਸਾਰਣੀ[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    ਪੂਰਨ
get_fileformat(buf_T *buf)
{
    ਪੂਰਨ		c = *buf->b_p_ff;

    ਜੇ (buf->b_p_bin || c == 'u')
	ਵਾਪਸ EOL_UNIX;
    ਜੇ (c == 'm')
	ਵਾਪਸ EOL_MAC;
    ਵਾਪਸ EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    ਪੂਰਨ
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    ਪੂਰਨ		c;

    ਜੇ (eap != ਨਿਰਾ && eap->force_ff != 0)
	c = eap->force_ff;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ ((eap != ਨਿਰਾ && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    ਵਾਪਸ EOL_UNIX;
	c = *buf->b_p_ff;
    }
    ਜੇ (c == 'u')
	ਵਾਪਸ EOL_UNIX;
    ਜੇ (c == 'm')
	ਵਾਪਸ EOL_MAC;
    ਵਾਪਸ EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    ਖਾਲੀ
ਸੈੱਟ_fileformat(
    ਪੂਰਨ		t,
    ਪੂਰਨ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ਅੱਖਰ	*p = ਨਿਰਾ;

    ਚੋਣ (t)
    {
    ਹਾਲਤ EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	ਰੋਕੋ;
    ਹਾਲਤ EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	ਰੋਕੋ;
    ਹਾਲਤ EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	ਰੋਕੋ;
    }
    ਜੇ (p != ਨਿਰਾ)
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_ਸਥਿਤੀ(curbuf);
    redraw_tabline = TRUE;
#ਜੇ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    ਪੂਰਨ
default_fileformat(ਖਾਲੀ)
{
    ਚੋਣ (*p_ffs)
    {
	ਹਾਲਤ 'm':   ਵਾਪਸ EOL_MAC;
	ਹਾਲਤ 'd':   ਵਾਪਸ EOL_DOS;
    }
    ਵਾਪਸ EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    ਪੂਰਨ
call_shell(char_u *cmd, ਪੂਰਨ opt)
{
    ਪੂਰਨ		retval;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    proftime_T	wait_ਸਮਾਂ;
#ਜੇ_ਅੰਤ

    ਜੇ (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == ਨਿਰਾ ? p_sh : cmd);
	ਸੁਨੇਹਾ_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	prof_ਬੱਚਾ_enter(&wait_ਸਮਾਂ);
#ਜੇ_ਅੰਤ

    ਜੇ (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_ਖਾਲੀ_ਥਾਵਾਂ));
	retval = -1;
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
	// Don't hide the pointer while executing a shell command.
	gui_mch_mousehide(FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	++hold_gui_events;
#ਜੇ_ਅੰਤ
	// The external command may update a tags file, clear cached tags.
	ਟੈਗ_freematch();

	ਜੇ (cmd == ਨਿਰਾ || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	ਨਹੀਂਤਾਂ
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    ਜੇ (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		ਜੇ (ecmd == ਨਿਰਾ)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    ਜੇ (ncmd != ਨਿਰਾ)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((ਅੱਖਰ *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    ਨਹੀਂਤਾਂ
		retval = -1;
	    ਜੇ (ecmd != cmd)
		vim_free(ecmd);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	--hold_gui_events;
#ਜੇ_ਅੰਤ
	/*
	 * Check the window size, in case it changed while executing the
	 * external command.
	 */
	shell_resized_check();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸੈੱਟ_vim_var_nr(VV_SHELL_ERROR, (ਲੰਮਾ)retval);
# ifdef FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	prof_ਬੱਚਾ_exit(&wait_ਸਮਾਂ);
# endif
#ਜੇ_ਅੰਤ

    ਵਾਪਸ retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    ਪੂਰਨ
get_real_state(ਖਾਲੀ)
{
    ਜੇ (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ)
    {
	ਜੇ (VIsual_ਸਰਗਰਮ)
	{
	    ਜੇ (VIsual_select)
		ਵਾਪਸ ਬਹੁਲਕ_SELECT;
	    ਵਾਪਸ ਬਹੁਲਕ_VISUAL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਅੰਤ_op)
	    ਵਾਪਸ ਬਹੁਲਕ_OP_PENDING;
    }
    ਵਾਪਸ State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    ਪੂਰਨ
after_pathsep(char_u *b, char_u *p)
{
    ਵਾਪਸ p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_ਸਿਰ_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    ਪੂਰਨ
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    ਜੇ (f1 == ਨਿਰਾ || f2 == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    (ਖਾਲੀ)vim_ਭਰਿਆ_Name(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    ਵਾਪਸ (t1 - ffname == t2 - f2
	     && pathcmp((ਅੱਖਰ *)ffname, (ਅੱਖਰ *)f2, (ਪੂਰਨ)(t1 - ffname)) == 0);
}

#ਜੇ defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    ਪੂਰਨ
vim_chdirfile(char_u *fname, ਅੱਖਰ *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    ਜੇ (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    ਜੇ (pathcmp((ਅੱਖਰ *)old_dir, (ਅੱਖਰ *)new_dir, -1) == 0)
	// nothing to do
	ਵਾਪਸ OK;

    ਜੇ (trigger_autocmd != ਨਿਰਾ)
	trigger_Dir_ਬਦਲਿਆ_Pre((char_u *)trigger_autocmd, new_dir);

    ਜੇ (mch_chdir((ਅੱਖਰ *)new_dir) != 0)
	ਵਾਪਸ FAIL;

    ਜੇ (trigger_autocmd != ਨਿਰਾ)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    ਸਥਿਰ ਪੂਰਨ
illegal_slash(ਅਚਲ ਅੱਖਰ *name)
{
    ਜੇ (name[0] == NUL)
	ਵਾਪਸ FALSE;	    // no file name is not illegal
    ਜੇ (name[ਲੜੀ_ਲੰਬਾਈ(name) - 1] != '/')
	ਵਾਪਸ FALSE;	    // no trailing slash
    ਜੇ (mch_isdir((char_u *)name))
	ਵਾਪਸ FALSE;	    // trailing slash for a directory
    ਵਾਪਸ TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    ਪੂਰਨ
vim_stat(ਅਚਲ ਅੱਖਰ *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    ਵਾਪਸ illegal_slash(name) ? -1 : stat(name, stp);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_ਸਾਰਣੀ[SHAPE_IDX_ਗਿਣਤੀ] =
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
ਸਥਿਰ string_T mshape_ਨਾਂ[] =
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
    {ਨਿਰਾ, 0}
};

#  define MSHAPE_ਨਾਂ_ਗਿਣਤੀ  (ARRAY_ਲੰਮਾਈ(mshape_ਨਾਂ) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    ਅੱਖਰ *
parse_shape_opt(ਪੂਰਨ what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    ਪੂਰਨ		idx = 0;		// init for GCC
    ਪੂਰਨ		all_idx;
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    ਲੰਮਾ	n;
    ਪੂਰਨ		ਮਿਲਿਆ_ve = FALSE;	// found "ve" flag
    ਪੂਰਨ		ਗੇੜਾ;

    /*
     * First round: check for errors; second round: do it for real.
     */
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	ਜੇ (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	ਨਹੀਂਤਾਂ
# endif
	    modep = p_guicursor;
	ਜਦੋਂ (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    ਜੇ (colonp == ਨਿਰਾ || (commap != ਨਿਰਾ && commap < colonp))
		ਵਾਪਸ e_missing_colon_2;
	    ਜੇ (colonp == modep)
		ਵਾਪਸ e_illegal_ਬਹੁਲਕ;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    ਜਦੋਂ (modep < colonp || all_idx >= 0)
	    {
		ਜੇ (all_idx < 0)
		{
		    // Find the mode.
		    ਜੇ (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    ਨਹੀਂਤਾਂ
			len = 2;
		    ਜੇ (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_ਗਿਣਤੀ - 1;
		    ਨਹੀਂਤਾਂ
		    {
			ਲਈ (idx = 0; idx < SHAPE_IDX_ਗਿਣਤੀ; ++idx)
			    ਜੇ (STRNICMP(modep, shape_ਸਾਰਣੀ[idx].name, len)
									 == 0)
				ਰੋਕੋ;
			ਜੇ (idx == SHAPE_IDX_ਗਿਣਤੀ
				   || (shape_ਸਾਰਣੀ[idx].ਵਰਤਿਆ_for & what) == 0)
			    ਵਾਪਸ e_illegal_ਬਹੁਲਕ;
			ਜੇ (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    ਮਿਲਿਆ_ve = TRUE;
		    }
		    modep += len + 1;
		}

		ਜੇ (all_idx >= 0)
		    idx = all_idx--;
		ਨਹੀਂਤਾਂ ਜੇ (ਗੇੜਾ == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    ਜੇ (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_ਸਾਰਣੀ[idx].mshape = 0;
		    }
		    ਨਹੀਂਤਾਂ
# endif
		    {
			// Set the defaults, for the missing parts
			shape_ਸਾਰਣੀ[idx].shape = SHAPE_ਖੰਡ;
			shape_ਸਾਰਣੀ[idx].blinkwait = 700L;
			shape_ਸਾਰਣੀ[idx].blinkon = 400L;
			shape_ਸਾਰਣੀ[idx].blinkoff = 250L;
		    }
		}

		// Parse the part after the colon
		ਲਈ (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    ਜੇ (what == SHAPE_MOUSE)
		    {
			ਲਈ (i = 0; ; ++i)
			{
			    ਜੇ (mshape_ਨਾਂ[i].string == ਨਿਰਾ)
			    {
				ਜੇ (!VIM_ISDIGIT(*p))
				    ਵਾਪਸ e_illegal_mouseshape;
				ਜੇ (ਗੇੜਾ == 2)
				    shape_ਸਾਰਣੀ[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				ਨਹੀਂਤਾਂ
				    (ਖਾਲੀ)getdigits(&p);
				ਰੋਕੋ;
			    }
			    ਜੇ (STRNICMP(p, mshape_ਨਾਂ[i].string, mshape_ਨਾਂ[i].ਲੰਮਾਈ) == 0)
			    {
				ਜੇ (ਗੇੜਾ == 2)
				    shape_ਸਾਰਣੀ[idx].mshape = i;
				p += mshape_ਨਾਂ[i].ਲੰਮਾਈ;
				ਰੋਕੋ;
			    }
			}
		    }
		    ਨਹੀਂਤਾਂ // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			ਜੇ (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			ਜੇ (len != 0)
			{
			    p += len;
			    ਜੇ (!VIM_ISDIGIT(*p))
				ਵਾਪਸ e_digit_expected;
			    n = getdigits(&p);
			    ਜੇ (len == 3)   // "ver" or "hor"
			    {
				ਜੇ (n == 0)
				    ਵਾਪਸ e_illegal_percentage;
				ਜੇ (ਗੇੜਾ == 2)
				{
				    ਜੇ (TOLOWER_ASC(i) == 'v')
					shape_ਸਾਰਣੀ[idx].shape = SHAPE_VER;
				    ਨਹੀਂਤਾਂ
					shape_ਸਾਰਣੀ[idx].shape = SHAPE_HOR;
				    shape_ਸਾਰਣੀ[idx].percentage = n;
				}
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (ਗੇੜਾ == 2)
			    {
				ਜੇ (len == 9)
				    shape_ਸਾਰਣੀ[idx].blinkwait = n;
				ਨਹੀਂਤਾਂ ਜੇ (len == 7)
				    shape_ਸਾਰਣੀ[idx].blinkon = n;
				ਨਹੀਂਤਾਂ
				    shape_ਸਾਰਣੀ[idx].blinkoff = n;
			    }
			}
			ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "block", 5) == 0)
			{
			    ਜੇ (ਗੇੜਾ == 2)
				shape_ਸਾਰਣੀ[idx].shape = SHAPE_ਖੰਡ;
			    p += 5;
			}
			ਨਹੀਂਤਾਂ	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    ਜੇ (commap == ਨਿਰਾ)		    // last part
			    {
				ਜੇ (endp == ਨਿਰਾ)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (endp > commap || endp == ਨਿਰਾ)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    ਜੇ (slashp != ਨਿਰਾ && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_ਸਮੂਹ(p, (ਪੂਰਨ)(slashp - p));
				p = slashp + 1;
			    }
			    ਜੇ (ਗੇੜਾ == 2)
			    {
				shape_ਸਾਰਣੀ[idx].id = syn_check_ਸਮੂਹ(p,
							     (ਪੂਰਨ)(endp - p));
				shape_ਸਾਰਣੀ[idx].id_lm = shape_ਸਾਰਣੀ[idx].id;
				ਜੇ (slashp != ਨਿਰਾ && slashp < endp)
				    shape_ਸਾਰਣੀ[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    ਜੇ (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    ਜੇ (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    ਜੇ (!ਮਿਲਿਆ_ve)
    {
# ifdef FEAT_MOUSESHAPE
	ਜੇ (what == SHAPE_MOUSE)
	{
	    shape_ਸਾਰਣੀ[SHAPE_IDX_VE].mshape = shape_ਸਾਰਣੀ[SHAPE_IDX_V].mshape;
	}
	ਨਹੀਂਤਾਂ
# endif
	{
	    shape_ਸਾਰਣੀ[SHAPE_IDX_VE].shape = shape_ਸਾਰਣੀ[SHAPE_IDX_V].shape;
	    shape_ਸਾਰਣੀ[SHAPE_IDX_VE].percentage =
					 shape_ਸਾਰਣੀ[SHAPE_IDX_V].percentage;
	    shape_ਸਾਰਣੀ[SHAPE_IDX_VE].blinkwait =
					  shape_ਸਾਰਣੀ[SHAPE_IDX_V].blinkwait;
	    shape_ਸਾਰਣੀ[SHAPE_IDX_VE].blinkon =
					    shape_ਸਾਰਣੀ[SHAPE_IDX_V].blinkon;
	    shape_ਸਾਰਣੀ[SHAPE_IDX_VE].blinkoff =
					   shape_ਸਾਰਣੀ[SHAPE_IDX_V].blinkoff;
	    shape_ਸਾਰਣੀ[SHAPE_IDX_VE].id = shape_ਸਾਰਣੀ[SHAPE_IDX_V].id;
	    shape_ਸਾਰਣੀ[SHAPE_IDX_VE].id_lm = shape_ਸਾਰਣੀ[SHAPE_IDX_V].id_lm;
	}
    }

    ਵਾਪਸ ਨਿਰਾ;
}

# ਜੇ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    ਪੂਰਨ
get_shape_idx(ਪੂਰਨ mouse)
{
#  ifdef FEAT_MOUSESHAPE
    ਜੇ (mouse && (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_ASKMORE))
    {
#   ifdef FEAT_GUI
	ਪੂਰਨ x, y;
	gui_mch_getmouse(&x, &y);
	ਜੇ (Y_2_ਪੰਕਤੀ(y) == ਪੰਕਤੀਆਂ - 1)
	    ਵਾਪਸ SHAPE_IDX_MOREL;
#   endif
	ਵਾਪਸ SHAPE_IDX_ਹੋਰ;
    }
    ਜੇ (mouse && drag_ਸਥਿਤੀ_ਸਤਰ)
	ਵਾਪਸ SHAPE_IDX_SDRAG;
    ਜੇ (mouse && drag_sep_ਸਤਰ)
	ਵਾਪਸ SHAPE_IDX_VDRAG;
#  endif
    ਜੇ (!mouse && State == ਬਹੁਲਕ_SHOWMATCH)
	ਵਾਪਸ SHAPE_IDX_SM;
    ਜੇ (State & VREPLACE_FLAG)
	ਵਾਪਸ SHAPE_IDX_R;
    ਜੇ (State & REPLACE_FLAG)
	ਵਾਪਸ SHAPE_IDX_R;
    ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	ਵਾਪਸ SHAPE_IDX_I;
    ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
    {
	ਜੇ (cmdline_at_ਸਿਰਾ())
	    ਵਾਪਸ SHAPE_IDX_C;
	ਜੇ (cmdline_overstrike())
	    ਵਾਪਸ SHAPE_IDX_CR;
	ਵਾਪਸ SHAPE_IDX_CI;
    }
    ਜੇ (ਅੰਤ_op)
	ਵਾਪਸ SHAPE_IDX_O;
    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (*p_sel == 'e')
	    ਵਾਪਸ SHAPE_IDX_VE;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ SHAPE_IDX_V;
    }
    ਵਾਪਸ SHAPE_IDX_N;
}
# endif

# ਜੇ defined(FEAT_MOUSESHAPE)
ਸਥਿਰ ਪੂਰਨ current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    ਖਾਲੀ
update_mouseshape(ਪੂਰਨ shape_idx)
{
    ਪੂਰਨ new_mouse_shape;

    // Only works in GUI mode.
    ਜੇ (!gui.ਵਿੱਚ_use || gui.starting)
	ਵਾਪਸ;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    ਜੇ (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	ਵਾਪਸ;
    }

    // When ignoring the mouse don't change shape on the statusline.
    ਜੇ (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_ਸਥਿਤੀ
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    ਜੇ (shape_idx == -2
	    && current_mouse_shape != shape_ਸਾਰਣੀ[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_ਸਾਰਣੀ[SHAPE_IDX_ਸਥਿਤੀ].mshape
	    && current_mouse_shape != shape_ਸਾਰਣੀ[SHAPE_IDX_VSEP].mshape)
	ਵਾਪਸ;
    ਜੇ (shape_idx < 0)
	new_mouse_shape = shape_ਸਾਰਣੀ[get_shape_idx(TRUE)].mshape;
    ਨਹੀਂਤਾਂ
	new_mouse_shape = shape_ਸਾਰਣੀ[shape_idx].mshape;
    ਜੇ (new_mouse_shape != current_mouse_shape)
    {
	mch_ਸੈੱਟ_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#ਜੇ_ਅੰਤ // CURSOR_SHAPE

#ਜੇ defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    ਖਾਲੀ
f_getmouseshape(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;
# ਜੇ defined(FEAT_MOUSESHAPE)
    ਜੇ (current_mouse_shape >= 0
			      && current_mouse_shape < (ਪੂਰਨ)MSHAPE_ਨਾਂ_ਗਿਣਤੀ)
	rettv->vval.v_string = vim_strnsave(
				  mshape_ਨਾਂ[current_mouse_shape].string,
				  mshape_ਨਾਂ[current_mouse_shape].ਲੰਮਾਈ);
# endif
}
#ਜੇ_ਅੰਤ



/*
 * Change directory to "new_dir".  Search 'cdpath' for relative directory
 * names.
 */
    ਪੂਰਨ
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    ਪੂਰਨ		r;
    char_u	*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ = ਨਿਰਾ;
    ਅੱਖਰ	*ਖੋਜ_ctx = ਨਿਰਾ;

    dir_name = ਲੱਭੋ_directory_ਵਿੱਚ_ਰਸਤਾ(new_dir, (ਪੂਰਨ)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, &ਖੋਜ_ctx);
    vim_free(file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
    vim_findfile_cleanup(ਖੋਜ_ctx);
    ਜੇ (dir_name == ਨਿਰਾ)
	ਵਾਪਸ -1;
    r = mch_chdir((ਅੱਖਰ *)dir_name);
    vim_free(dir_name);
    ਵਾਪਸ r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
get_user_name(char_u *buf, ਪੂਰਨ len)
{
    ਜੇ (username == ਨਿਰਾ)
    {
	ਜੇ (mch_get_user_name(buf, len) == FAIL)
	    ਵਾਪਸ FAIL;
	username = vim_strsave(buf);
    }
    ਨਹੀਂਤਾਂ
	vim_strncpy(buf, username, len - 1);
    ਵਾਪਸ OK;
}

#ਜੇ defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    ਖਾਲੀ
free_username(ਖਾਲੀ)
{
    vim_free(username);
}
#ਜੇ_ਅੰਤ

#ਜੇ !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    ਖਾਲੀ
qsort(
    ਖਾਲੀ	*ਅਧਾਰ,
    size_t	elm_ਗਿਣਤੀ,
    size_t	elm_ਮਾਪ,
    ਪੂਰਨ (*cmp)(ਅਚਲ ਖਾਲੀ *, ਅਚਲ ਖਾਲੀ *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		gap;

    buf = alloc(elm_ਮਾਪ);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (gap = elm_ਗਿਣਤੀ / 2; gap > 0; gap /= 2)
	ਲਈ (i = gap; i < elm_ਗਿਣਤੀ; ++i)
	    ਲਈ (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)ਅਧਾਰ + j * elm_ਮਾਪ;
		p2 = (char_u *)ਅਧਾਰ + (j + gap) * elm_ਮਾਪ;
		ਜੇ ((*cmp)((ਖਾਲੀ *)p1, (ਖਾਲੀ *)p2) <= 0)
		    ਰੋਕੋ;
		// Exchange the elements.
		mch_memmove(buf, p1, elm_ਮਾਪ);
		mch_memmove(p1, p2, elm_ਮਾਪ);
		mch_memmove(p2, buf, elm_ਮਾਪ);
	    }

    vim_free(buf);
}
#ਜੇ_ਅੰਤ

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

#ਜੇ !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

ਸਥਿਰ ਪੂਰਨ  envsize = -1;	// current size of environment
ਬਾਹਰੀ ਅੱਖਰ **environ;		// the global which is your env.

ਸਥਿਰ ਪੂਰਨ  findenv(ਅੱਖਰ *name); // look for a name in the env.
ਸਥਿਰ ਪੂਰਨ  newenv(ਖਾਲੀ);	// copy env. from stack to heap
ਸਥਿਰ ਪੂਰਨ  moreenv(ਖਾਲੀ);	// incr. size of env.

    ਪੂਰਨ
putenv(ਅਚਲ ਅੱਖਰ *string)
{
    ਪੂਰਨ	    i;
    ਅੱਖਰ    *p;

    ਜੇ (envsize < 0)
    {				// first time putenv called
	ਜੇ (newenv() < 0)	// copy env. to heap
	    ਵਾਪਸ -1;
    }

    i = findenv((ਅੱਖਰ *)string); // look for name in environment

    ਜੇ (i < 0)
    {				// name must be added
	ਲਈ (i = 0; environ[i]; i++);
	ਜੇ (i >= (envsize - 1))
	{			// need new slot
	    ਜੇ (moreenv() < 0)
		ਵਾਪਸ -1;
	}
	p = alloc(ਲੜੀ_ਲੰਬਾਈ(string) + 1);
	ਜੇ (p == ਨਿਰਾ)		// not enough core
	    ਵਾਪਸ -1;
	environ[i + 1] = 0;	// new end of env.
    }
    ਨਹੀਂਤਾਂ
    {				// name already in env.
	p = vim_realloc(environ[i], ਲੜੀ_ਲੰਬਾਈ(string) + 1);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
findenv(ਅੱਖਰ *name)
{
    ਅੱਖਰ    *namechar, *envchar;
    ਪੂਰਨ	    i, ਮਿਲਿਆ;

    ਮਿਲਿਆ = 0;
    ਲਈ (i = 0; environ[i] && !ਮਿਲਿਆ; i++)
    {
	envchar = environ[i];
	namechar = name;
	ਜਦੋਂ (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	ਮਿਲਿਆ = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    ਵਾਪਸ ਮਿਲਿਆ ? i - 1 : -1;
}

    ਸਥਿਰ ਪੂਰਨ
newenv(ਖਾਲੀ)
{
    ਅੱਖਰ    **env, *elem;
    ਪੂਰਨ	    i, esize;

    ਲਈ (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(ਅੱਖਰ *, esize);
    ਜੇ (env == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਲਈ (i = 0; environ[i]; i++)
    {
	elem = alloc(ਲੜੀ_ਲੰਬਾਈ(environ[i]) + 1);
	ਜੇ (elem == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
moreenv(ਖਾਲੀ)
{
    ਪੂਰਨ	    esize;
    ਅੱਖਰ    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((ਅੱਖਰ *)environ, esize * ਆਕਾਰ (*env));
    ਜੇ (env == 0)
	ਵਾਪਸ -1;
    environ = env;
    envsize = esize;
    ਵਾਪਸ 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(ਅਚਲ char_u *string)
{
    ਪੂਰਨ i;
    char_u *p;

    ਜੇ (envsize < 0)
	ਵਾਪਸ ਨਿਰਾ;

    i = findenv((ਅੱਖਰ *)string);

    ਜੇ (i < 0)
	ਵਾਪਸ ਨਿਰਾ;

    p = vim_strchr((char_u *)environ[i], '=');
    ਵਾਪਸ (p + 1);
}
# endif

#ਜੇ_ਅੰਤ // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    ਪੂਰਨ
filewritable(char_u *fname)
{
    ਪੂਰਨ		retval = 0;
# ਜੇ defined(UNIX) || defined(VMS)
    ਪੂਰਨ		perm = 0;
# endif

# ਜੇ defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    ਜੇ (
# ifdef MSWIN
	    mch_writable(fname) &&
# ਨਹੀਂਤਾਂ
#  ਜੇ defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((ਅੱਖਰ *)fname, W_OK) == 0
       )
    {
	++retval;
	ਜੇ (mch_isdir(fname))
	    ++retval;
    }
    ਵਾਪਸ retval;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ਪੂਰਨ
get2c(FILE *fd)
{
    ਪੂਰਨ		c, n;

    n = getc(fd);
    ਜੇ (n == EOF) ਵਾਪਸ -1;
    c = getc(fd);
    ਜੇ (c == EOF) ਵਾਪਸ -1;
    ਵਾਪਸ (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ਪੂਰਨ
get3c(FILE *fd)
{
    ਪੂਰਨ		c, n;

    n = getc(fd);
    ਜੇ (n == EOF) ਵਾਪਸ -1;
    c = getc(fd);
    ਜੇ (c == EOF) ਵਾਪਸ -1;
    n = (n << 8) + c;
    c = getc(fd);
    ਜੇ (c == EOF) ਵਾਪਸ -1;
    ਵਾਪਸ (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ਪੂਰਨ
get4c(FILE *fd)
{
    ਪੂਰਨ		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    ਬਿਨਾਨਿਸ਼ਾਨੀ	n;

    c = getc(fd);
    ਜੇ (c == EOF) ਵਾਪਸ -1;
    n = (ਬਿਨਾਨਿਸ਼ਾਨੀ)c;
    c = getc(fd);
    ਜੇ (c == EOF) ਵਾਪਸ -1;
    n = (n << 8) + (ਬਿਨਾਨਿਸ਼ਾਨੀ)c;
    c = getc(fd);
    ਜੇ (c == EOF) ਵਾਪਸ -1;
    n = (n << 8) + (ਬਿਨਾਨਿਸ਼ਾਨੀ)c;
    c = getc(fd);
    ਜੇ (c == EOF) ਵਾਪਸ -1;
    n = (n << 8) + (ਬਿਨਾਨਿਸ਼ਾਨੀ)c;
    ਵਾਪਸ (ਪੂਰਨ)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, ਪੂਰਨ cnt)
{
    char_u	*str;
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;

    // allocate memory
    str = alloc(cnt + 1);
    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Read the string.  Quit when running into the EOF.
    ਲਈ (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	ਜੇ (c == EOF)
	{
	    vim_free(str);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	str[i] = c;
    }
    str[i] = NUL;
    ਵਾਪਸ str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    ਪੂਰਨ
put_ਬਾਈਟਾਂ(FILE *fd, long_u nr, ਪੂਰਨ len)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = len - 1; i >= 0; --i)
	ਜੇ (putc((ਪੂਰਨ)(nr >> (i * 8)), fd) == EOF)
	    ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    ਲੰਮਾ
elapsed(ਬਣਤਰ timeval *ਸ਼ੁਰੂ_tv)
{
    ਬਣਤਰ timeval  now_tv;

    gettimeofday(&now_tv, ਨਿਰਾ);
    ਵਾਪਸ (now_tv.tv_sec - ਸ਼ੁਰੂ_tv->tv_sec) * 1000L
	 + (now_tv.tv_usec - ਸ਼ੁਰੂ_tv->tv_usec) / 1000L;
}
# endif

# ifdef ELAPSED_TICKCOUNT
/*
 * Return time in msec since "start_tick".
 */
    ਲੰਮਾ
elapsed(DWORD ਸ਼ੁਰੂ_tick)
{
    DWORD	now = Get_Tick_ਗਿਣਤੀ();

    ਵਾਪਸ (ਲੰਮਾ)now - (ਲੰਮਾ)ਸ਼ੁਰੂ_tick;
}
# endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    ਪੂਰਨ
mch_parse_cmd(char_u *cmd, ਪੂਰਨ use_shcf, ਅੱਖਰ ***argv, ਪੂਰਨ *argc)
{
    ਪੂਰਨ		i;
    char_u	*p, *d;
    ਪੂਰਨ		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    ਲਈ (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	ਜਦੋਂ (*p != NUL)
	{
	    ਜੇ (i == 2)
		(*argv)[*argc] = (ਅੱਖਰ *)p;
	    ++*argc;
	    d = p;
	    ਜਦੋਂ (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		ਜੇ (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    ਜੇ (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    ਜੇ (*p == NUL)
	    {
		ਜੇ (i == 2)
		    *d++ = NUL;
		ਰੋਕੋ;
	    }
	    ਜੇ (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	ਜੇ (*argv == ਨਿਰਾ)
	{
	    ਜੇ (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		ਲਈ (;;)
		{
		    p = skiptowhite(p);
		    ਜੇ (*p == NUL)
			ਰੋਕੋ;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(ਅੱਖਰ *, *argc + 4);
	    ਜੇ (*argv == ਨਿਰਾ)	    // out of memory
		ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    ਪੂਰਨ
build_argv_from_string(char_u *cmd, ਅੱਖਰ ***argv, ਪੂਰਨ *argc)
{
    char_u	*cmd_copy;
    ਪੂਰਨ		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    ਜੇ (cmd_copy == ਨਿਰਾ
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	ਵਾਪਸ FAIL;
    }
    ਲਈ (i = 0; i < *argc; i++)
	(*argv)[i] = (ਅੱਖਰ *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = ਨਿਰਾ;
    vim_free(cmd_copy);
    ਵਾਪਸ OK;
}

#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    ਪੂਰਨ
build_argv_from_list(list_T *l, ਅੱਖਰ ***argv, ਪੂਰਨ *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(ਅੱਖਰ *, l->lv_len + 1);
    ਜੇ (*argv == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    *argc = 0;
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ਜੇ (s == ਨਿਰਾ)
	{
	    ਪੂਰਨ i;

	    ਲਈ (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = ਨਿਰਾ;
	    ਵਾਪਸ FAIL;
	}
	(*argv)[*argc] = (ਅੱਖਰ *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = ਨਿਰਾ;
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Change the behavior of vterm.
 * 0: As usual.
 * 1: Windows 10 version 1809
 *      The bug causes unstable handling of ambiguous width character.
 * 2: Windows 10 version 1903 & 1909
 *      Use the wrong result because each result is different.
 * 3: Windows 10 insider preview (current latest logic)
 */
    ਪੂਰਨ
get_special_pty_type(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਵਾਪਸ get_conpty_type();
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

// compare two keyvalue_T structs by case sensitive value
    ਪੂਰਨ
cmp_keyvalue_ਮੁੱਲ(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ਵਾਪਸ STRCMP(kv1->ਮੁੱਲ.string, kv2->ਮੁੱਲ.string);
}

// compare two keyvalue_T structs by value with length
    ਪੂਰਨ
cmp_keyvalue_ਮੁੱਲ_n(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ਵਾਪਸ STRNCMP(kv1->ਮੁੱਲ.string, kv2->ਮੁੱਲ.string, MAX(kv1->ਮੁੱਲ.ਲੰਮਾਈ,
		kv2->ਮੁੱਲ.ਲੰਮਾਈ));
}

// compare two keyvalue_T structs by case insensitive value
    ਪੂਰਨ
cmp_keyvalue_ਮੁੱਲ_i(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ਵਾਪਸ STRICMP(kv1->ਮੁੱਲ.string, kv2->ਮੁੱਲ.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    ਪੂਰਨ
cmp_keyvalue_ਮੁੱਲ_ni(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ਵਾਪਸ vim_strnicmp_asc((ਅੱਖਰ *)kv1->ਮੁੱਲ.string,
	    (ਅੱਖਰ *)kv2->ਮੁੱਲ.string, MAX(kv1->ਮੁੱਲ.ਲੰਮਾਈ,
		    kv2->ਮੁੱਲ.ਲੰਮਾਈ));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    ਖਾਲੀ *
mergesort_list(
    ਖਾਲੀ	*ਸਿਰ,
    ਖਾਲੀ	*(*get_ਅਗਲਾ)(ਖਾਲੀ *),
    ਖਾਲੀ	(*ਸੈੱਟ_ਅਗਲਾ)(ਖਾਲੀ *, ਖਾਲੀ *),
    ਖਾਲੀ	*(*get_prev)(ਖਾਲੀ *),
    ਖਾਲੀ	(*ਸੈੱਟ_prev)(ਖਾਲੀ *, ਖਾਲੀ *),
    ਪੂਰਨ		(*compare)(ਅਚਲ ਖਾਲੀ *, ਅਚਲ ਖਾਲੀ *))
{
    ਜੇ (!ਸਿਰ || !get_ਅਗਲਾ(ਸਿਰ))
	ਵਾਪਸ ਸਿਰ;

    // Count length
    ਪੂਰਨ	    n = 0;
    ਖਾਲੀ*   curr = ਸਿਰ;
    ਜਦੋਂ (curr)
    {
	n++;
	curr = get_ਅਗਲਾ(curr);
    }

    ਪੂਰਨ	ਮਾਪ;
    ਲਈ (ਮਾਪ = 1; ਮਾਪ < n; ਮਾਪ *= 2)
    {
	ਖਾਲੀ*	new_ਸਿਰ = ਨਿਰਾ;
	ਖਾਲੀ*	ਪੂਛ = ਨਿਰਾ;
	curr = ਸਿਰ;

	ਜਦੋਂ (curr)
	{
	    // Split two runs
	    ਖਾਲੀ    *ਖੱਬਾ = curr;
	    ਖਾਲੀ    *ਸੱਜਾ = ਖੱਬਾ;
	    ਪੂਰਨ	    i;
	    ਲਈ (i = 0; i < ਮਾਪ && ਸੱਜਾ; ++i)
		ਸੱਜਾ = get_ਅਗਲਾ(ਸੱਜਾ);

	    ਖਾਲੀ    *ਅਗਲਾ = ਸੱਜਾ;
	    ਲਈ (i = 0; i < ਮਾਪ && ਅਗਲਾ; ++i)
		ਅਗਲਾ = get_ਅਗਲਾ(ਅਗਲਾ);

	    // Break links
	    ਖਾਲੀ    *l_ਸਿਰਾ = ਸੱਜਾ ? get_prev(ਸੱਜਾ) : ਨਿਰਾ;
	    ਜੇ (l_ਸਿਰਾ)
		ਸੈੱਟ_ਅਗਲਾ(l_ਸਿਰਾ, ਨਿਰਾ);
	    ਜੇ (ਸੱਜਾ)
		ਸੈੱਟ_prev(ਸੱਜਾ, ਨਿਰਾ);

	    ਖਾਲੀ    *r_ਸਿਰਾ = ਅਗਲਾ ? get_prev(ਅਗਲਾ) : ਨਿਰਾ;
	    ਜੇ (r_ਸਿਰਾ)
		ਸੈੱਟ_ਅਗਲਾ(r_ਸਿਰਾ, ਨਿਰਾ);
	    ਜੇ (ਅਗਲਾ)
		ਸੈੱਟ_prev(ਅਗਲਾ, ਨਿਰਾ);

	    //ਮਿਲਾਓ
	    ਖਾਲੀ    *merged = ਨਿਰਾ;
	    ਖਾਲੀ    *merged_ਪੂਛ = ਨਿਰਾ;

	    ਜਦੋਂ (ਖੱਬਾ || ਸੱਜਾ)
	    {
		ਖਾਲੀ	*chosen = ਨਿਰਾ;
		ਜੇ (!ਖੱਬਾ)
		{
		    chosen = ਸੱਜਾ;
		    ਸੱਜਾ = get_ਅਗਲਾ(ਸੱਜਾ);
		}
		ਨਹੀਂਤਾਂ ਜੇ (!ਸੱਜਾ)
		{
		    chosen = ਖੱਬਾ;
		    ਖੱਬਾ = get_ਅਗਲਾ(ਖੱਬਾ);
		}
		ਨਹੀਂਤਾਂ ਜੇ (compare(ਖੱਬਾ, ਸੱਜਾ) <= 0)
		{
		    chosen = ਖੱਬਾ;
		    ਖੱਬਾ = get_ਅਗਲਾ(ਖੱਬਾ);
		}
		ਨਹੀਂਤਾਂ
		{
		    chosen = ਸੱਜਾ;
		    ਸੱਜਾ = get_ਅਗਲਾ(ਸੱਜਾ);
		}

		ਜੇ (merged_ਪੂਛ)
		{
		    ਸੈੱਟ_ਅਗਲਾ(merged_ਪੂਛ, chosen);
		    ਸੈੱਟ_prev(chosen, merged_ਪੂਛ);
		    merged_ਪੂਛ = chosen;
		}
		ਨਹੀਂਤਾਂ
		{
		    merged = merged_ਪੂਛ = chosen;
		    ਸੈੱਟ_prev(chosen, ਨਿਰਾ);
		}
	    }

	    // Connect to full list
	    ਜੇ (!new_ਸਿਰ)
		new_ਸਿਰ = merged;
	    ਨਹੀਂਤਾਂ
	    {
		ਸੈੱਟ_ਅਗਲਾ(ਪੂਛ, merged);
		ਸੈੱਟ_prev(merged, ਪੂਛ);
	    }

	    // Move tail to end
	    ਜਦੋਂ (get_ਅਗਲਾ(merged_ਪੂਛ))
		merged_ਪੂਛ = get_ਅਗਲਾ(merged_ਪੂਛ);
	    ਪੂਛ = merged_ਪੂਛ;

	    curr = ਅਗਲਾ;
	}

	ਸਿਰ = new_ਸਿਰ;
    }

    ਵਾਪਸ ਸਿਰ;
}

ਸਥਿਰ ਅਚਲ char_u base64_ਸਾਰਣੀ[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

ਸਥਿਰ char_u base64_dec_ਸਾਰਣੀ[256];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    ਸਥਿਰ ਖਾਲੀ
init_base64_dec_ਸਾਰਣੀ(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ base64_dec_tbl_initialized = FALSE;

    ਜੇ (base64_dec_tbl_initialized)
	ਵਾਪਸ;

    vim_memset(base64_dec_ਸਾਰਣੀ, 0xFF, ਆਕਾਰ(base64_dec_ਸਾਰਣੀ));
    ਲਈ (size_t i = 0; i < ਆਕਾਰ(base64_ਸਾਰਣੀ) - 1; i++)
	base64_dec_ਸਾਰਣੀ[(char_u)base64_ਸਾਰਣੀ[i]] = (char_u)i;
    base64_dec_ਸਾਰਣੀ['='] = 0;

    base64_dec_tbl_initialized = TRUE;
}

/*
 * Base64 encode "src[len]" into caller-supplied "dst", which must have room for
 * at least BASE64_ENCODED_LEN(len) bytes (NUL terminated). Returns the number
 * of bytes written, excluding the NUL.
 */
    ਲੰਮਾ
base64_encode_buf(char_u *dst, ਅਚਲ char_u *src, size_t len)
{
    size_t i, j;

    ਲਈ (i = 0, j = 0; i < len;)
    {
	size_t ਬਚਿਆ = len - i;

	int_u octet_a = src[i++];
	int_u octet_b = ਬਚਿਆ > 1 ? src[i++] : 0;
	int_u octet_c = ਬਚਿਆ > 2 ? src[i++] : 0;

	int_u triple = (octet_a << 16) | (octet_b << 8) | octet_c;

	dst[j++] = base64_ਸਾਰਣੀ[(triple >> 18) & 0x3F];
	dst[j++] = base64_ਸਾਰਣੀ[(triple >> 12) & 0x3F];
	dst[j++] = ਬਚਿਆ > 1
	    ? base64_ਸਾਰਣੀ[(triple >> 6) & 0x3F]
	    : '=';
	dst[j++] = ਬਚਿਆ > 2
	    ? base64_ਸਾਰਣੀ[triple & 0x3F]
	    : '=';
    }

    dst[j] = NUL;
    ਵਾਪਸ (ਲੰਮਾ)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(ਅਚਲ char_u *ਅੰਕੜੇ, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    ਜੇ (encoded == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    base64_encode_buf(encoded, ਅੰਕੜੇ, len);
    ਵਾਪਸ encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    ਪੂਰਨ
base64_decode(ਅਚਲ char_u *ਅੰਕੜੇ, size_t len, garray_T *ਬਾਹਰਲਾ)
{
    ਜੇ (len == 0)
	ਵਾਪਸ OK;

    ਜੇ (len % 4 != 0)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), ਅੰਕੜੇ);
	ਵਾਪਸ FAIL;
    }

    init_base64_dec_ਸਾਰਣੀ();

    size_t decoded_len = (len / 4) * 3;
    ਜੇ (ਅੰਕੜੇ[len - 1] == '=')
	decoded_len--;
    ਜੇ (ਅੰਕੜੇ[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    ਲਈ (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_ਸਾਰਣੀ[(char_u)ਅੰਕੜੇ[i++]];
	int_u sextet_b = base64_dec_ਸਾਰਣੀ[(char_u)ਅੰਕੜੇ[i++]];
	int_u sextet_c = base64_dec_ਸਾਰਣੀ[(char_u)ਅੰਕੜੇ[i++]];
	int_u sextet_d = base64_dec_ਸਾਰਣੀ[(char_u)ਅੰਕੜੇ[i++]];

	ਜੇ (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), ਅੰਕੜੇ);
	    ga_clear(ਬਾਹਰਲਾ);
	    ਵਾਪਸ FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	ਜੇ (j < decoded_len)
	{
	    ga_append(ਬਾਹਰਲਾ, (triple >> 16) & 0xFF);
	    j++;
	}
	ਜੇ (j < decoded_len)
	{
	    ga_append(ਬਾਹਰਲਾ, (triple >> 8) & 0xFF);
	    j++;
	}
	ਜੇ (j < decoded_len)
	{
	    ga_append(ਬਾਹਰਲਾ, triple & 0xFF);
	    j++;
	}

	ਜੇ (j == decoded_len)
	{
	    ਜੇ ((ਅੰਕੜੇ[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((ਅੰਕੜੇ[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_ਦਲੀਲ_str), ਅੰਕੜੇ);
		ga_clear(ਬਾਹਰਲਾ);
		ਵਾਪਸ FAIL;
	    }
	}
    }
    ਵਾਪਸ OK;
}
