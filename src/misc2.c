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
#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ char_u	*username = ರಿಕ್ತ; // cached result of mch_get_user_name()

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ coladvance2(pos_T *pos, ಪೂರ್ಣಾಙ್ಕ addspaces, ಪೂರ್ಣಾಙ್ಕ finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    ಪೂರ್ಣಾಙ್ಕ
virtual_active(ಶೂನ್ಯ)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ cur_ve_flags = get_ve_flags();

    ಯದಿ (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_ಸೇರಿಸು) && (State & MODE_ಸೇರಿಸು)))
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    ಯದಿ (virtual_op != MAYBE)
	ಪ್ರತ್ಯಾಗಮ virtual_op;
    ಪ್ರತ್ಯಾಗಮ (cur_ve_flags & VE_BLOCK) && VIsual_active && VIsual_mode == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    ಪೂರ್ಣಾಙ್ಕ
getviscol(ಶೂನ್ಯ)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, ರಿಕ್ತ, ರಿಕ್ತ, GETVCOL_NO_ABOVE);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    ಪೂರ್ಣಾಙ್ಕ
coladvance_force(colnr_T wcol)
{
    ಪೂರ್ಣಾಙ್ಕ rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    ಯದಿ (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ಅನ್ಯಥಾ
	// Virtcol is valid
	set_valid_virtcol(curwin, wcol);
    ಪ್ರತ್ಯಾಗಮ rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    ಪೂರ್ಣಾಙ್ಕ
getviscol2(colnr_T col, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = col;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, ರಿಕ್ತ, ರಿಕ್ತ, GETVCOL_NO_ABOVE);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)x;
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
    ಪೂರ್ಣಾಙ್ಕ
coladvance(colnr_T wantcol)
{
    ಪೂರ್ಣಾಙ್ಕ rc = getvpos(&curwin->w_cursor, wantcol);

    ಯದಿ (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ಅನ್ಯಥಾ ಯದಿ (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	set_valid_virtcol(curwin, wantcol);
    ಪ್ರತ್ಯಾಗಮ rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    ಪೂರ್ಣಾಙ್ಕ
getvpos(pos_T *pos, colnr_T wantcol)
{
    ಪ್ರತ್ಯಾಗಮ coladvance2(pos, FALSE, virtual_active(), wantcol);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
coladvance2(
    pos_T	*pos,
    ಪೂರ್ಣಾಙ್ಕ		addspaces,	// change the text to achieve our goal?
    ಪೂರ್ಣಾಙ್ಕ		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		linelen;
    colnr_T	col = 0;
    ಪೂರ್ಣಾಙ್ಕ		csize = 0;
    ಪೂರ್ಣಾಙ್ಕ		one_more;
#ifdef FEAT_LINEBREAK
    ಪೂರ್ಣಾಙ್ಕ		head = 0;
#endif

    one_more = (State & MODE_ಸೇರಿಸು)
		    || restart_edit != NUL
		    || (VIsual_active && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    ಸಾಲು = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    ಯದಿ (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_more;
	    col = wcol;

	    ಯದಿ ((addspaces || finetune) && !VIsual_active)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_more;
		ಯದಿ (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ		width = curwin->w_width - win_col_off(curwin);
	chartabsize_T	cts;

	ಯದಿ (finetune
		&& curwin->w_p_wrap
		&& curwin->w_width != 0
		&& wcol >= (colnr_T)width
		&& width > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    ಯದಿ (csize > 0)
		csize--;

	    ಯದಿ (wcol / width > (colnr_T)csize / width
		    && ((State & MODE_ಸೇರಿಸು) == 0 || (ಪೂರ್ಣಾಙ್ಕ)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / width + 1) * width - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, ಸಾಲು, ಸಾಲು);
#ifdef FEAT_PROP_POPUP
	cts.cts_no_above = true;
#endif
	ಯಾವತ್ (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ifdef FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &head, ರಿಕ್ತ);
	    MB_PTR_ADV(cts.cts_ptr);
#ಅನ್ಯಥಾ
	    csize = lbr_chartabsize_adv(&cts);
#endif
	    cts.cts_vcol += csize;
	}
	col = cts.cts_vcol;
	idx = (ಪೂರ್ಣಾಙ್ಕ)(cts.cts_ptr - ಸಾಲು);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	ಯದಿ (col > wcol || (!virtual_active() && one_more == 0))
	{
	    idx -= 1;
#ifdef FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= head;
#endif
	    col -= csize;
	}

	ಯದಿ (virtual_active()
		&& addspaces
		&& wcol >= 0
		&& ((col != wcol && col != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    ಯದಿ (ಸಾಲು[idx] == NUL)
	    {
		// Append spaces
		ಪೂರ್ಣಾಙ್ಕ	correct = wcol - col;
		char_u	*newline = alloc(idx + correct + 1);
		ಪೂರ್ಣಾಙ್ಕ	t;

		ಯದಿ (newline == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		ಪ್ರತಿ (t = 0; t < idx; ++t)
		    newline[t] = ಸಾಲು[t];

		ಪ್ರತಿ (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, (colnr_T)idx);
		idx += correct;
		col = wcol;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Break a tab
		ಪೂರ್ಣಾಙ್ಕ	correct = wcol - col - csize + 1; // negative!!
		char_u	*newline;
		ಪೂರ್ಣಾಙ್ಕ	t, s = 0;
		ಪೂರ್ಣಾಙ್ಕ	v;

		ಯದಿ (-correct > csize)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		newline = alloc(linelen + csize);
		ಯದಿ (newline == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		ಪ್ರತಿ (t = 0; t < linelen; t++)
		{
		    ಯದಿ (t != idx)
			newline[s++] = ಸಾಲು[t];
		    ಅನ್ಯಥಾ
			ಪ್ರತಿ (v = 0; v < csize; v++)
			    newline[s++] = ' ';
		}

		newline[linelen + csize - 1] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, idx);
		idx += (csize - 1 + correct);
		col += correct;
	    }
	}
    }

    ಯದಿ (idx < 0)
	pos->col = 0;
    ಅನ್ಯಥಾ
	pos->col = idx;

    pos->coladd = 0;

    ಯದಿ (finetune)
    {
	ಯದಿ (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    ಯದಿ (!one_more)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, ರಿಕ್ತ, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ b = (ಪೂರ್ಣಾಙ್ಕ)wcol - (ಪೂರ್ಣಾಙ್ಕ)col;

	    // The difference between wcol and col is used to set coladd.
	    ಯದಿ (b > 0 && b < (MAXCOL - 2 * curwin->w_width))
		pos->coladd = b;

	    col += b;
	}
    }

    // prevent from moving onto a trail byte
    ಯದಿ (has_mbyte)
	mb_adjustpos(curbuf, pos);

    ಯದಿ (wcol < 0 || col < wcol)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    ಪೂರ್ಣಾಙ್ಕ
inc_cursor(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    ಯದಿ (lp->col != MAXCOL)
    {
	p = ml_get_pos(lp);
	ಯದಿ (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    ಯದಿ (has_mbyte)
	    {
		ಪೂರ್ಣಾಙ್ಕ l = (*mb_ptr2len)(p);

		lp->col += l;
		ಪ್ರತ್ಯಾಗಮ ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->col++;
	    lp->coladd = 0;
	    ಪ್ರತ್ಯಾಗಮ ((p[1] != NUL) ? 0 : 2);
	}
    }
    ಯದಿ (lp->lnum != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)     // there is a next line
    {
	lp->col = 0;
	lp->lnum++;
	lp->coladd = 0;
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    ಪೂರ್ಣಾಙ್ಕ
incl(pos_T *lp)
{
    ಪೂರ್ಣಾಙ್ಕ	    r;

    ಯದಿ ((r = inc(lp)) >= 1 && lp->col)
	r = inc(lp);
    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
dec_cursor(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ dec(&curwin->w_cursor);
}

    ಪೂರ್ಣಾಙ್ಕ
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    ಯದಿ (lp->col == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	ಯದಿ (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (lp->col > 0)
    {
	// still within line
	lp->col--;
	ಯದಿ (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	}
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	ಯದಿ (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	ಪ್ರತ್ಯಾಗಮ 1;
    }

    // at start of file
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    ಪೂರ್ಣಾಙ್ಕ
decl(pos_T *lp)
{
    ಪೂರ್ಣಾಙ್ಕ	    r;

    ಯದಿ ((r = dec(lp)) == 1 && lp->col)
	r = dec(lp);
    ಪ್ರತ್ಯಾಗಮ r;
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

#ifdef FEAT_FOLDING
    ಯದಿ (hasAnyFolding(wp))
    {
	ಯದಿ (lnum > cursor)
	{
	    ಯಾವತ್ (lnum > cursor)
	    {
		(ಶೂನ್ಯ)hasFoldingWin(wp, lnum, &lnum, ರಿಕ್ತ, TRUE, ರಿಕ್ತ);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		ಯದಿ (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (lnum < cursor)
	{
	    ಯಾವತ್ (lnum < cursor)
	    {
		(ಶೂನ್ಯ)hasFoldingWin(wp, lnum, ರಿಕ್ತ, &lnum, TRUE, ರಿಕ್ತ);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		ಯದಿ (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    ಅನ್ಯಥಾ
#endif
	retval = lnum - cursor;

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    ಶೂನ್ಯ
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    ಯದಿ (pos->lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	pos->lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಯದಿ (pos->col > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	ಯದಿ (pos->col > len)
	    pos->col = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    ಶೂನ್ಯ
check_cursor_lnum(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
#ifdef FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	ಯದಿ (!hasFolding(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
						&curwin->w_cursor.lnum, ರಿಕ್ತ))
#endif
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    }
    ಯದಿ (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    ಶೂನ್ಯ
check_cursor_col(ಶೂನ್ಯ)
{
    check_cursor_col_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    ಶೂನ್ಯ
check_cursor_col_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.col;
    colnr_T      oldcoladd = win->w_cursor.col + win->w_cursor.coladd;
    unsigned ಪೂರ್ಣಾಙ್ಕ cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_buffer, win->w_cursor.lnum);
    ಯದಿ (len == 0)
	win->w_cursor.col = 0;
    ಅನ್ಯಥಾ ಯದಿ (win->w_cursor.col >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	ಯದಿ ((State & MODE_ಸೇರಿಸು) || restart_edit
		|| (VIsual_active && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_active())
	    win->w_cursor.col = len;
	ಅನ್ಯಥಾ
	{
	    win->w_cursor.col = len - 1;
	    // Move the cursor to the head byte.
	    ಯದಿ (has_mbyte)
		mb_adjustpos(win->w_buffer, &win->w_cursor);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (win->w_cursor.col < 0)
	win->w_cursor.col = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    ಯದಿ (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    ಅನ್ಯಥಾ ಯದಿ (cur_ve_flags == VE_ALL)
    {
	ಯದಿ (oldcoladd > win->w_cursor.col)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.col;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    ಯದಿ (win->w_cursor.col + 1 < len)
	    {
		ಪೂರ್ಣಾಙ್ಕ cs, ce;

		getvcol(win, &win->w_cursor, &cs, ರಿಕ್ತ, &ce, 0);
		ಯದಿ (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	ಅನ್ಯಥಾ
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    ಶೂನ್ಯ
check_cursor(ಶೂನ್ಯ)
{
    check_cursor_lnum();
    check_cursor_col();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    ಶೂನ್ಯ
check_visual_pos(ಶೂನ್ಯ)
{
    ಯದಿ (VIsual.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	VIsual.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	VIsual.col = 0;
	VIsual.coladd = 0;
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ len = ml_get_len(VIsual.lnum);

	ಯದಿ (VIsual.col > len)
	{
	    VIsual.col = len;
	    VIsual.coladd = 0;
	}
    }
}

/*
 * Make sure curwin->w_cursor is not on the NUL at the end of the line.
 * Allow it when in Visual mode and 'selection' is not "old".
 */
    ಶೂನ್ಯ
adjust_cursor_col(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_cursor.col > 0
	    && (!VIsual_active || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_leftcol(colnr_T leftcol)
{
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;

    // Return quickly when there is no change.
    ಯದಿ (curwin->w_leftcol == leftcol)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    curwin->w_leftcol = leftcol;

    changed_cline_bef_curs();
    long lastcol = curwin->w_leftcol + curwin->w_width - curwin_col_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    long siso = get_sidescrolloff_ಮೌಲ್ಯ();
    ಯದಿ (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(ಶೂನ್ಯ)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, ರಿಕ್ತ, &e, 0);
    ಯದಿ (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    ಅನ್ಯಥಾ ಯದಿ (s < curwin->w_leftcol)
    {
	retval = TRUE;
	ಯದಿ (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    changed_cline_bef_curs();
	}
    }

    ಯದಿ (retval)
	curwin->w_set_curswant = true;
    redraw_later(UPD_NOT_VALID);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    ಪೂರ್ಣಾಙ್ಕ
copy_option_part(
    char_u	**option,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		maxlen,
    ವರ್ಣ	*sep_chars)
{
    ಪೂರ್ಣಾಙ್ಕ	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    ಯದಿ (*p == '.')
	buf[len++] = *p++;
    ಯಾವತ್ (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == ರಿಕ್ತ)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	ಯದಿ (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != ರಿಕ್ತ)
	    ++p;
	ಯದಿ (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    ಯದಿ (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_to_option_part(p);	// p points to next file name

    *option = p;
    ಪ್ರತ್ಯಾಗಮ len;
}

#ಯದಿ !defined(HAVE_MEMSET) && !defined(PROTO)
    ಶೂನ್ಯ *
vim_memset(ಶೂನ್ಯ *ptr, ಪೂರ್ಣಾಙ್ಕ c, size_t size)
{
    ವರ್ಣ *p = ptr;

    ಯಾವತ್ (size-- > 0)
	*p++ = c;
    ಪ್ರತ್ಯಾಗಮ ptr;
}
#endif

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_isspace(ಪೂರ್ಣಾಙ್ಕ x)
{
    ಪ್ರತ್ಯಾಗಮ ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

ಸ್ಥಿರ ಸಂರಚನಾ modmasktable
{
    short	mod_mask;	// Bit-mask for particular key modifier
    short	mod_flag;	// Bit(s) for particular key modifier
    char_u	name;		// Single letter name of modifier
} mod_mask_table[] =
{
    {MOD_MASK_ALT,		MOD_MASK_ALT,		(char_u)'M'},
    {MOD_MASK_META,		MOD_MASK_META,		(char_u)'T'},
    {MOD_MASK_CTRL,		MOD_MASK_CTRL,		(char_u)'C'},
    {MOD_MASK_SHIFT,		MOD_MASK_SHIFT,		(char_u)'S'},
    {MOD_MASK_MULTI_CLICK,	MOD_MASK_2CLICK,	(char_u)'2'},
    {MOD_MASK_MULTI_CLICK,	MOD_MASK_3CLICK,	(char_u)'3'},
    {MOD_MASK_MULTI_CLICK,	MOD_MASK_4CLICK,	(char_u)'4'},
#ಯದಿ defined(MACOS_X) || defined(FEAT_GUI_GTK)
    {MOD_MASK_CMD,		MOD_MASK_CMD,		(char_u)'D'},
#endif
    // 'A' must be the last one
    {MOD_MASK_ALT,		MOD_MASK_ALT,		(char_u)'A'},
    {0, 0, NUL}
    // NOTE: when adding an entry, update MAX_KEY_NAME_LEN!
};

/*
 * Shifted key terminal codes and their unshifted equivalent.
 * Don't add mouse codes here, they are handled separately!
 */
#define MOD_ಕೀಲಿಗಳು_ENTRY_SIZE 5

ಸ್ಥಿರ char_u modifier_ಕೀಲಿಗಳು_table[] =
{
//  mod mask	    with modifier		without modifier
    MOD_MASK_SHIFT, '&', '9',			'@', '1',	// begin
    MOD_MASK_SHIFT, '&', '0',			'@', '2',	// cancel
    MOD_MASK_SHIFT, '*', '1',			'@', '4',	// command
    MOD_MASK_SHIFT, '*', '2',			'@', '5',	// copy
    MOD_MASK_SHIFT, '*', '3',			'@', '6',	// create
    MOD_MASK_SHIFT, '*', '4',			'k', 'D',	// delete char
    MOD_MASK_SHIFT, '*', '5',			'k', 'L',	// delete line
    MOD_MASK_SHIFT, '*', '7',			'@', '7',	// end
    MOD_MASK_CTRL,  KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_C_END,	'@', '7',	// end
    MOD_MASK_SHIFT, '*', '9',			'@', '9',	// exit
    MOD_MASK_SHIFT, '*', '0',			'@', '0',	// find
    MOD_MASK_SHIFT, '#', '1',			'%', '1',	// help
    MOD_MASK_SHIFT, '#', '2',			'k', 'h',	// home
    MOD_MASK_CTRL,  KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_C_HOME,	'k', 'h',	// home
    MOD_MASK_SHIFT, '#', '3',			'k', 'I',	// insert
    MOD_MASK_SHIFT, '#', '4',			'k', 'l',	// left arrow
    MOD_MASK_CTRL,  KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_C_ಎಡ,	'k', 'l',	// left arrow
    MOD_MASK_SHIFT, '%', 'a',			'%', '3',	// message
    MOD_MASK_SHIFT, '%', 'b',			'%', '4',	// move
    MOD_MASK_SHIFT, '%', 'c',			'%', '5',	// next
    MOD_MASK_SHIFT, '%', 'd',			'%', '7',	// options
    MOD_MASK_SHIFT, '%', 'e',			'%', '8',	// previous
    MOD_MASK_SHIFT, '%', 'f',			'%', '9',	// print
    MOD_MASK_SHIFT, '%', 'g',			'%', '0',	// redo
    MOD_MASK_SHIFT, '%', 'h',			'&', '3',	// replace
    MOD_MASK_SHIFT, '%', 'i',			'k', 'r',	// right arr.
    MOD_MASK_CTRL,  KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_C_ಬಲ,	'k', 'r',	// right arr.
    MOD_MASK_SHIFT, '%', 'j',			'&', '5',	// resume
    MOD_MASK_SHIFT, '!', '1',			'&', '6',	// save
    MOD_MASK_SHIFT, '!', '2',			'&', '7',	// suspend
    MOD_MASK_SHIFT, '!', '3',			'&', '8',	// undo
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_UP,	'k', 'u',	// up arrow
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_DOWN,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_XF1,	KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_XF1,
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_XF2,	KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_XF2,
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_XF3,	KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_XF3,
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_XF4,	KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_XF4,

    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F1,	'k', '1',	// F1
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F2,	'k', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F3,	'k', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F4,	'k', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F5,	'k', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F6,	'k', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F7,	'k', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F8,	'k', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F9,	'k', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F10,	'k', ';',	// F10

    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F11,	'F', '1',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F12,	'F', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F13,	'F', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F14,	'F', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F15,	'F', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F16,	'F', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F17,	'F', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F18,	'F', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F19,	'F', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F20,	'F', 'A',

    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F21,	'F', 'B',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F22,	'F', 'C',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F23,	'F', 'D',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F24,	'F', 'E',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F25,	'F', 'F',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F26,	'F', 'G',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F27,	'F', 'H',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F28,	'F', 'I',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F29,	'F', 'J',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F30,	'F', 'K',

    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F31,	'F', 'L',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F32,	'F', 'M',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F33,	'F', 'N',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F34,	'F', 'O',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F35,	'F', 'P',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F36,	'F', 'Q',
    MOD_MASK_SHIFT, KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_MASK_SHIFT, 'k', 'B',			KS_EXTRA, (ಪೂರ್ಣಾಙ್ಕ)KE_TAB,

    NUL
};

ಸ್ಥಿರ ಸಂರಚನಾ ಕೀಲಿ_name_entry
{
    ಪೂರ್ಣಾಙ್ಕ		enabled;	    // is this entry available (TRUE/FALSE)?
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ;		    // special key code or ascii value
    string_T	name;		    // name of key
    ಪೂರ್ಣಾಙ್ಕ		is_alt;		    // is an alternative name
} ಕೀಲಿ_names_table[] =
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
#ifdef FEAT_MOUSE_DEC
    TRUE,
#ಅನ್ಯಥಾ
    FALSE,
#endif
	K_DEC_MOUSE, STR_LITERAL_INIT("DecMouse"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Del"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Delete"), TRUE},
    {TRUE, K_DOWN, STR_LITERAL_INIT("Down"), FALSE},
    {TRUE, K_DROP, STR_LITERAL_INIT("Drop"), FALSE},
    {TRUE, K_END, STR_LITERAL_INIT("End"), FALSE},
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
#ifdef FEAT_MOUSE_JSB
    TRUE,
#ಅನ್ಯಥಾ
    FALSE,
#endif
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
    {TRUE, K_ಎಡ, STR_LITERAL_INIT("Left"), FALSE},
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
#ifdef FEAT_MOUSE_NET
    TRUE,
#ಅನ್ಯಥಾ
    FALSE,
#endif
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
#ifdef FEAT_MOUSE_PTERM
    TRUE,
#ಅನ್ಯಥಾ
    FALSE,
#endif
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_ಬಲ, STR_LITERAL_INIT("Right"), FALSE},
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
#ifdef FEAT_EVAL
    TRUE,
#ಅನ್ಯಥಾ
    FALSE,
#endif
	K_SNR, STR_LITERAL_INIT("SNR"), FALSE},
    {TRUE, ' ', STR_LITERAL_INIT("Space"), FALSE},
    {TRUE, TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_UNDO, STR_LITERAL_INIT("Undo"), FALSE},
    {TRUE, K_UP, STR_LITERAL_INIT("Up"), FALSE},
    {
#ifdef FEAT_MOUSE_URXVT
    TRUE,
#ಅನ್ಯಥಾ
    FALSE,
#endif
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
name_to_mod_mask(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    c = TOUPPER_ASC(c);
    ಪ್ರತಿ (i = 0; mod_mask_table[i].mod_mask != 0; i++)
	ಯದಿ (c == mod_mask_table[i].name)
	    ಪ್ರತ್ಯಾಗಮ mod_mask_table[i].mod_flag;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    ಪೂರ್ಣಾಙ್ಕ
simplify_ಕೀಲಿ(ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ *modifiers)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    key0;
    ಪೂರ್ಣಾಙ್ಕ	    key1;

    ಯದಿ (!(*modifiers & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;

    // TAB is a special case
    ಯದಿ (ಕೀಲಿ == TAB && (*modifiers & MOD_MASK_SHIFT))
    {
	*modifiers &= ~MOD_MASK_SHIFT;
	ಪ್ರತ್ಯಾಗಮ K_S_TAB;
    }
    key0 = KEY2TERMCAP0(ಕೀಲಿ);
    key1 = KEY2TERMCAP1(ಕೀಲಿ);
    ಪ್ರತಿ (i = 0; modifier_ಕೀಲಿಗಳು_table[i] != NUL; i += MOD_ಕೀಲಿಗಳು_ENTRY_SIZE)
    {
	ಯದಿ (key0 == modifier_ಕೀಲಿಗಳು_table[i + 3]
		&& key1 == modifier_ಕೀಲಿಗಳು_table[i + 4]
		&& (*modifiers & modifier_ಕೀಲಿಗಳು_table[i]))
	{
	    *modifiers &= ~modifier_ಕೀಲಿಗಳು_table[i];
	    ಪ್ರತ್ಯಾಗಮ TERMCAP2_ಕೀಲಿ(modifier_ಕೀಲಿಗಳು_table[i + 1],
		    modifier_ಕೀಲಿಗಳು_table[i + 2]);
	}
    }
    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    ಪೂರ್ಣಾಙ್ಕ
handle_x_ಕೀಲಿಗಳು(ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ)
{
    ವಿಕಲ್ಪ (ಕೀಲಿ)
    {
	ಪಕ್ಷ K_XUP:	ಪ್ರತ್ಯಾಗಮ K_UP;
	ಪಕ್ಷ K_XDOWN:	ಪ್ರತ್ಯಾಗಮ K_DOWN;
	ಪಕ್ಷ K_XLEFT:	ಪ್ರತ್ಯಾಗಮ K_ಎಡ;
	ಪಕ್ಷ K_XRIGHT:	ಪ್ರತ್ಯಾಗಮ K_ಬಲ;
	ಪಕ್ಷ K_XHOME:	ಪ್ರತ್ಯಾಗಮ K_HOME;
	ಪಕ್ಷ K_ZHOME:	ಪ್ರತ್ಯಾಗಮ K_HOME;
	ಪಕ್ಷ K_XEND:	ಪ್ರತ್ಯಾಗಮ K_END;
	ಪಕ್ಷ K_ZEND:	ಪ್ರತ್ಯಾಗಮ K_END;
	ಪಕ್ಷ K_XF1:	ಪ್ರತ್ಯಾಗಮ K_F1;
	ಪಕ್ಷ K_XF2:	ಪ್ರತ್ಯಾಗಮ K_F2;
	ಪಕ್ಷ K_XF3:	ಪ್ರತ್ಯಾಗಮ K_F3;
	ಪಕ್ಷ K_XF4:	ಪ್ರತ್ಯಾಗಮ K_F4;
	ಪಕ್ಷ K_S_XF1:	ಪ್ರತ್ಯಾಗಮ K_S_F1;
	ಪಕ್ಷ K_S_XF2:	ಪ್ರತ್ಯಾಗಮ K_S_F2;
	ಪಕ್ಷ K_S_XF3:	ಪ್ರತ್ಯಾಗಮ K_S_F3;
	ಪಕ್ಷ K_S_XF4:	ಪ್ರತ್ಯಾಗಮ K_S_F4;
    }
    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_ಕೀಲಿ_name(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ modifiers)
{
    ಸ್ಥಿರ char_u string[MAX_ಕೀಲಿ_NAME_LEN + 1];
    ಪೂರ್ಣಾಙ್ಕ	    i, idx, len;
    ಪೂರ್ಣಾಙ್ಕ	    table_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    ಯದಿ (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_ಕೀಲಿ)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    ಯದಿ (IS_SPECIAL(c))
    {
	ಪ್ರತಿ (i = 0; modifier_ಕೀಲಿಗಳು_table[i] != 0; i += MOD_ಕೀಲಿಗಳು_ENTRY_SIZE)
	    ಯದಿ (       KEY2TERMCAP0(c) == (ಪೂರ್ಣಾಙ್ಕ)modifier_ಕೀಲಿಗಳು_table[i + 1]
		    && (ಪೂರ್ಣಾಙ್ಕ)KEY2TERMCAP1(c) == (ಪೂರ್ಣಾಙ್ಕ)modifier_ಕೀಲಿಗಳು_table[i + 2])
	    {
		modifiers |= modifier_ಕೀಲಿಗಳು_table[i];
		c = TERMCAP2_ಕೀಲಿ(modifier_ಕೀಲಿಗಳು_table[i + 3],
						   modifier_ಕೀಲಿಗಳು_table[i + 4]);
		ಭಙ್ಗ;
	    }
    }

    // try to find the key in the special key table
    table_idx = find_special_ಕೀಲಿ_in_table(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    ಯದಿ (c > 0 && (*mb_char2len)(c) == 1)
    {
	ಯದಿ (table_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_MASK_ALT;
	    // try again, to find the un-alted key in the special key table
	    table_idx = find_special_ಕೀಲಿ_in_table(c);
	}
	ಯದಿ (table_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_MASK_CTRL;
	}
    }

    // translate the modifier into a string
    ಪ್ರತಿ (i = 0; mod_mask_table[i].name != 'A'; i++)
	ಯದಿ ((modifiers & mod_mask_table[i].mod_mask)
						== mod_mask_table[i].mod_flag)
	{
	    string[idx++] = mod_mask_table[i].name;
	    string[idx++] = (char_u)'-';
	}

    ಯದಿ (table_idx < 0)		// unknown special key, may output t_xx
    {
	ಯದಿ (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	ಅನ್ಯಥಾ
	{
	    len = (*mb_char2len)(c);
	    ಯದಿ (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    ಅನ್ಯಥಾ
	    {
		char_u	*s = transchar(c);
		ಯಾವತ್ (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    ಅನ್ಯಥಾ		// use name of special key
    {
	string_T    *s;

	s = &ಕೀಲಿ_names_table[table_idx].name;

	ಯದಿ (s->length + idx + 2 <= MAX_ಕೀಲಿ_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (ಪೂರ್ಣಾಙ್ಕ)s->length;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    ಪ್ರತ್ಯಾಗಮ string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    ಪೂರ್ಣಾಙ್ಕ
trans_special(
    char_u	**srcp,
    char_u	*dst,
    ಪೂರ್ಣಾಙ್ಕ		flags,		// FSK_ values
    ಪೂರ್ಣಾಙ್ಕ		escape_ks,	// escape K_SPECIAL bytes in the character
    ಪೂರ್ಣಾಙ್ಕ		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    ಪೂರ್ಣಾಙ್ಕ		modifiers = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ;

    ಕೀಲಿ = find_special_ಕೀಲಿ(srcp, &modifiers, flags, did_simplify);
    ಯದಿ (ಕೀಲಿ == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತ್ಯಾಗಮ special_to_buf(ಕೀಲಿ, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    ಪೂರ್ಣಾಙ್ಕ
special_to_buf(ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ modifiers, ಪೂರ್ಣಾಙ್ಕ escape_ks, char_u *dst)
{
    ಪೂರ್ಣಾಙ್ಕ		dlen = 0;

    // Put the appropriate modifier in a string
    ಯದಿ (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    ಯದಿ (IS_SPECIAL(ಕೀಲಿ))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(ಕೀಲಿ);
	dst[dlen++] = KEY2TERMCAP1(ಕೀಲಿ);
    }
    ಅನ್ಯಥಾ ಯದಿ (escape_ks)
	dlen = (ಪೂರ್ಣಾಙ್ಕ)(add_char2buf(ಕೀಲಿ, dst + dlen) - dst);
    ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	dlen += (*mb_char2bytes)(ಕೀಲಿ, dst + dlen);
    ಅನ್ಯಥಾ
	dst[dlen++] = ಕೀಲಿ;

    ಪ್ರತ್ಯಾಗಮ dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    ಪೂರ್ಣಾಙ್ಕ
find_special_ಕೀಲಿ(
    char_u	**srcp,
    ಪೂರ್ಣಾಙ್ಕ		*modp,
    ಪೂರ್ಣಾಙ್ಕ		flags,		// FSK_ values
    ಪೂರ್ಣಾಙ್ಕ		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*last_dash;
    char_u	*end_of_name;
    char_u	*src;
    char_u	*bp;
    ಪೂರ್ಣಾಙ್ಕ		in_string = flags & FSK_IN_STRING;
    ಪೂರ್ಣಾಙ್ಕ		modifiers;
    ಪೂರ್ಣಾಙ್ಕ		bit;
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ;
    uvarnumber_T	n;
    ಪೂರ್ಣಾಙ್ಕ		l;

    src = *srcp;
    ಯದಿ (src[0] != '<')
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    last_dash = src;
    ಪ್ರತಿ (bp = src + 1; *bp == '-' || vim_isNormalIDc(*bp); bp++)
    {
	ಯದಿ (*bp == '-')
	{
	    last_dash = bp;
	    ಯದಿ (bp[1] != NUL)
	    {
		ಯದಿ (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		ಅನ್ಯಥಾ
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		ಯದಿ (!(in_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		ಅನ್ಯಥಾ ಯದಿ (in_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	ಯದಿ (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, ರಿಕ್ತ, &l, STR2NR_ALL, ರಿಕ್ತ, ರಿಕ್ತ, 0, TRUE, ರಿಕ್ತ);
	    ಯದಿ (l == 0)
	    {
		emsg(_(e_invalid_argument));
		ಪ್ರತ್ಯಾಗಮ 0;
	    }
	    bp += l + 5;
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (*bp == '>')	// found matching '>'
    {
	end_of_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	ಪ್ರತಿ (bp = src + 1; bp < last_dash; bp++)
	{
	    ಯದಿ (*bp != '-')
	    {
		bit = name_to_mod_mask(*bp);
		ಯದಿ (bit == 0x0)
		    ಭಙ್ಗ;	// Illegal modifier name
		modifiers |= bit;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	ಯದಿ (bp >= last_dash)
	{
	    ಯದಿ (STRNICMP(last_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(last_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(last_dash + 6, ರಿಕ್ತ, &l, STR2NR_ALL, ರಿಕ್ತ,
							    &n, 0, TRUE, ರಿಕ್ತ);
		ಯದಿ (l == 0)
		{
		    emsg(_(e_invalid_argument));
		    ಪ್ರತ್ಯಾಗಮ 0;
		}
		ಕೀಲಿ = (ಪೂರ್ಣಾಙ್ಕ)n;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ off = 1;

		// Modifier with single letter, or special key name.
		ಯದಿ (in_string && last_dash[1] == '\\' && last_dash[2] == '"')
		    off = 2;
		ಯದಿ (has_mbyte)
		    l = mb_ptr2len(last_dash + off);
		ಅನ್ಯಥಾ
		    l = 1;
		ಯದಿ (modifiers != 0 && last_dash[l + off] == '>')
		    ಕೀಲಿ = PTR2CHAR(last_dash + off);
		ಅನ್ಯಥಾ
		{
		    ಕೀಲಿ = get_special_ಕೀಲಿ_code(last_dash + off);
		    ಯದಿ (!(flags & FSK_KEEP_X_ಕೀಲಿ))
			ಕೀಲಿ = handle_x_ಕೀಲಿಗಳು(ಕೀಲಿ);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    ಯದಿ (ಕೀಲಿ != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);

		ಯದಿ ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    ಯದಿ (ಕೀಲಿ == K_BS)
			ಕೀಲಿ = BS;
		    ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ == K_DEL || ಕೀಲಿ == K_KDEL)
			ಕೀಲಿ = DEL;
		}
		ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    ಯದಿ ((flags & FSK_SIMPLIFY) != 0)
		    {
			ಯದಿ (did_simplify != ರಿಕ್ತ)
			    *did_simplify = TRUE;
		    }
		    ಅನ್ಯಥಾ
			ಕೀಲಿ = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		ಯದಿ (!IS_SPECIAL(ಕೀಲಿ))
		    ಕೀಲಿ = extract_modifiers(ಕೀಲಿ, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = end_of_name;
		ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;
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
    ಪೂರ್ಣಾಙ್ಕ
may_adjust_ಕೀಲಿ_for_ctrl(ಪೂರ್ಣಾಙ್ಕ modifiers, ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ)
{
    ಯದಿ ((modifiers & MOD_MASK_CTRL) == 0)
	ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;

    ಯದಿ (ASCII_ISALPHA(ಕೀಲಿ))
    {
#ifdef FEAT_TERMINAL
	check_no_reduce_ಕೀಲಿಗಳು();  // may update the no_reduce_keys flag
#endif
	ಪ್ರತ್ಯಾಗಮ no_reduce_ಕೀಲಿಗಳು == 0 ? TOUPPER_ASC(ಕೀಲಿ) : ಕೀಲಿ;
    }
    ಯದಿ (ಕೀಲಿ == '2')
	ಪ್ರತ್ಯಾಗಮ '@';
    ಯದಿ (ಕೀಲಿ == '6')
	ಪ್ರತ್ಯಾಗಮ '^';
    ಯದಿ (ಕೀಲಿ == '-')
	ಪ್ರತ್ಯಾಗಮ '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    ಯದಿ (ಕೀಲಿ == '$' && (modifiers & MOD_MASK_SHIFT) == 0)
	ಪ್ರತ್ಯಾಗಮ ']';

    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    ಪೂರ್ಣಾಙ್ಕ
may_remove_shift_modifier(ಪೂರ್ಣಾಙ್ಕ modifiers, ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ)
{
    ಯದಿ ((modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
#ifdef FEAT_GUI_GTK
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_CMD)
#endif
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    && ((ಕೀಲಿ >= '!' && ಕೀಲಿ <= '/')
		|| (ಕೀಲಿ >= ':' && ಕೀಲಿ <= 'Z')
		|| vim_isdigit(ಕೀಲಿ)
		|| (ಕೀಲಿ >= '[' && ಕೀಲಿ <= '`')
		|| (ಕೀಲಿ >= '{' && ಕೀಲಿ <= '~')))
	ಪ್ರತ್ಯಾಗಮ modifiers & ~MOD_MASK_SHIFT;

    ಯದಿ (modifiers == (MOD_MASK_SHIFT | MOD_MASK_CTRL)
		&& (ಕೀಲಿ == '{' || ಕೀಲಿ == '}' || ಕೀಲಿ == '|'))
	ಪ್ರತ್ಯಾಗಮ modifiers & ~MOD_MASK_SHIFT;

    ಪ್ರತ್ಯಾಗಮ modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    ಪೂರ್ಣಾಙ್ಕ
extract_modifiers(ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ *modp, ಪೂರ್ಣಾಙ್ಕ simplify, ಪೂರ್ಣಾಙ್ಕ *did_simplify)
{
    ಪೂರ್ಣಾಙ್ಕ	modifiers = *modp;

#ifdef MACOS_X
    // Command-key really special, no fancynest
    ಯದಿ (!(modifiers & MOD_MASK_CMD))
#endif
    ಯದಿ ((modifiers & MOD_MASK_SHIFT) && ASCII_ISALPHA(ಕೀಲಿ))
    {
	ಕೀಲಿ = TOUPPER_ASC(ಕೀಲಿ);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	ಯದಿ (simplify || modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    modifiers &= ~MOD_MASK_SHIFT;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    ಯದಿ ((modifiers & MOD_MASK_CTRL) && ASCII_ISALPHA(ಕೀಲಿ))
	ಕೀಲಿ = TOUPPER_ASC(ಕೀಲಿ);

    ಯದಿ (simplify && (modifiers & MOD_MASK_CTRL)
	    && ((ಕೀಲಿ >= '?' && ಕೀಲಿ <= '_') || ASCII_ISALPHA(ಕೀಲಿ)))
    {
	ಕೀಲಿ = Ctrl_chr(ಕೀಲಿ);
	modifiers &= ~MOD_MASK_CTRL;
	// <C-@> is <Nul>
	ಯದಿ (ಕೀಲಿ == NUL)
	    ಕೀಲಿ = K_ZERO;
	ಯದಿ (did_simplify != ರಿಕ್ತ)
	    *did_simplify = TRUE;
    }

#ifdef MACOS_X
    // Command-key really special, no fancynest
    ಯದಿ (!(modifiers & MOD_MASK_CMD))
#endif
    ಯದಿ (simplify && (modifiers & MOD_MASK_ALT) && ಕೀಲಿ < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	ಕೀಲಿ |= 0x80;
	modifiers &= ~MOD_MASK_ALT;	// remove the META modifier
	ಯದಿ (did_simplify != ರಿಕ್ತ)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
find_special_ಕೀಲಿ_in_table(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(ಕೀಲಿ_names_table); i++)
	ಯದಿ (c == ಕೀಲಿ_names_table[i].ಕೀಲಿ && !ಕೀಲಿ_names_table[i].is_alt)
	    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ_names_table[i].enabled ? i : -1;

    ಪ್ರತ್ಯಾಗಮ -1;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cmp_ಕೀಲಿ_name_entry(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    char_u  *p1 = ((ಸಂರಚನಾ ಕೀಲಿ_name_entry *)a)->name.string;
    char_u  *p2 = ((ಸಂರಚನಾ ಕೀಲಿ_name_entry *)b)->name.string;
    ಪೂರ್ಣಾಙ್ಕ	    result = 0;

    ಯದಿ (p1 == p2)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯಾವತ್ (vim_isNormalIDc(*p1) && *p2 != NUL)
    {
	ಯದಿ ((result = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    ಭಙ್ಗ;
	++p1;
	++p2;
    }

    ಯದಿ (result == 0)
    {
	ಯದಿ (*p2 == NUL)
	{
	    ಯದಿ (vim_isNormalIDc(*p1))
		result = 1;
	}
	ಅನ್ಯಥಾ
	{
	    result = -1;
	}
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_special_ಕೀಲಿ_code(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    ಯದಿ (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	ಯದಿ (add_termcap_entry(string, FALSE) == OK)
	    ಪ್ರತ್ಯಾಗಮ TERMCAP2_ಕೀಲಿ(name[2], name[3]);
    }
    ಅನ್ಯಥಾ
    {
	ಸಂರಚನಾ ಕೀಲಿ_name_entry	target;
	ಸಂರಚನಾ ಕೀಲಿ_name_entry	*entry;

	target.enabled = TRUE;
	target.ಕೀಲಿ = 0;
	target.name.string = name;
	target.name.length = 0;

	entry = (ಸಂರಚನಾ ಕೀಲಿ_name_entry *)bsearch(
	    &target,
	    &ಕೀಲಿ_names_table,
	    ARRAY_LENGTH(ಕೀಲಿ_names_table),
	    ಪರಿಮಾಣ(ಕೀಲಿ_names_table[0]),
	    cmp_ಕೀಲಿ_name_entry);
	ಯದಿ (entry != ರಿಕ್ತ && entry->enabled)
	{
	    ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ = entry->ಕೀಲಿ;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ == K_TAB ? TAB : ಕೀಲಿ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

    char_u *
get_ಕೀಲಿ_name(ಪೂರ್ಣಾಙ್ಕ i)
{
    ಯದಿ (i < 0 || i >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(ಕೀಲಿ_names_table))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ_names_table[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_fileformat(buf_T *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		c = *buf->b_p_ff;

    ಯದಿ (buf->b_p_bin || c == 'u')
	ಪ್ರತ್ಯಾಗಮ EOL_UNIX;
    ಯದಿ (c == 'm')
	ಪ್ರತ್ಯಾಗಮ EOL_MAC;
    ಪ್ರತ್ಯಾಗಮ EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (eap != ರಿಕ್ತ && eap->force_ff != 0)
	c = eap->force_ff;
    ಅನ್ಯಥಾ
    {
	ಯದಿ ((eap != ರಿಕ್ತ && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    ಪ್ರತ್ಯಾಗಮ EOL_UNIX;
	c = *buf->b_p_ff;
    }
    ಯದಿ (c == 'u')
	ಪ್ರತ್ಯಾಗಮ EOL_UNIX;
    ಯದಿ (c == 'm')
	ಪ್ರತ್ಯಾಗಮ EOL_MAC;
    ಪ್ರತ್ಯಾಗಮ EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    ಶೂನ್ಯ
set_fileformat(
    ಪೂರ್ಣಾಙ್ಕ		t,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ವರ್ಣ	*p = ರಿಕ್ತ;

    ವಿಕಲ್ಪ (t)
    {
    ಪಕ್ಷ EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	ಭಙ್ಗ;
    ಪಕ್ಷ EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	ಭಙ್ಗ;
    ಪಕ್ಷ EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	ಭಙ್ಗ;
    }
    ಯದಿ (p != ರಿಕ್ತ)
	set_string_option_direct((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_status(curbuf);
    redraw_tabline = TRUE;
#ಯದಿ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    ಪೂರ್ಣಾಙ್ಕ
default_fileformat(ಶೂನ್ಯ)
{
    ವಿಕಲ್ಪ (*p_ffs)
    {
	ಪಕ್ಷ 'm':   ಪ್ರತ್ಯಾಗಮ EOL_MAC;
	ಪಕ್ಷ 'd':   ಪ್ರತ್ಯಾಗಮ EOL_DOS;
    }
    ಪ್ರತ್ಯಾಗಮ EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    ಪೂರ್ಣಾಙ್ಕ
call_shell(char_u *cmd, ಪೂರ್ಣಾಙ್ಕ opt)
{
    ಪೂರ್ಣಾಙ್ಕ		retval;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif

    ಯದಿ (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == ರಿಕ್ತ ? p_sh : cmd);
	ಸಂದೇಶ_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES)
	prof_child_enter(&wait_time);
#endif

    ಯದಿ (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_empty));
	retval = -1;
    }
    ಅನ್ಯಥಾ
    {
#ifdef FEAT_GUI_MSWIN
	// Don't hide the pointer while executing a shell command.
	gui_mch_mousehide(FALSE);
#endif
#ifdef FEAT_GUI
	++hold_gui_events;
#endif
	// The external command may update a tags file, clear cached tags.
	tag_freematch();

	ಯದಿ (cmd == ರಿಕ್ತ || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	ಅನ್ಯಥಾ
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    ಯದಿ (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		ಯದಿ (ecmd == ರಿಕ್ತ)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    ಯದಿ (ncmd != ರಿಕ್ತ)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((ವರ್ಣ *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    ಅನ್ಯಥಾ
		retval = -1;
	    ಯದಿ (ecmd != cmd)
		vim_free(ecmd);
	}
#ifdef FEAT_GUI
	--hold_gui_events;
#endif
	/*
	 * Check the window size, in case it changed while executing the
	 * external command.
	 */
	shell_resized_check();
    }

#ifdef FEAT_EVAL
    set_vim_var_nr(VV_SHELL_ERROR, (long)retval);
# ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_real_state(ಶೂನ್ಯ)
{
    ಯದಿ (State & MODE_NORMAL)
    {
	ಯದಿ (VIsual_active)
	{
	    ಯದಿ (VIsual_select)
		ಪ್ರತ್ಯಾಗಮ MODE_SELECT;
	    ಪ್ರತ್ಯಾಗಮ MODE_VISUAL;
	}
	ಅನ್ಯಥಾ ಯದಿ (finish_op)
	    ಪ್ರತ್ಯಾಗಮ MODE_OP_PENDING;
    }
    ಪ್ರತ್ಯಾಗಮ State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    ಪೂರ್ಣಾಙ್ಕ
after_pathsep(char_u *b, char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_head_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    ಪೂರ್ಣಾಙ್ಕ
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    ಯದಿ (f1 == ರಿಕ್ತ || f2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    (ಶೂನ್ಯ)vim_FullName(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    ಪ್ರತ್ಯಾಗಮ (t1 - ffname == t2 - f2
	     && pathcmp((ವರ್ಣ *)ffname, (ವರ್ಣ *)f2, (ಪೂರ್ಣಾಙ್ಕ)(t1 - ffname)) == 0);
}

#ಯದಿ defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_chdirfile(char_u *fname, ವರ್ಣ *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    ಯದಿ (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    ಯದಿ (pathcmp((ವರ್ಣ *)old_dir, (ವರ್ಣ *)new_dir, -1) == 0)
	// nothing to do
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (trigger_autocmd != ರಿಕ್ತ)
	trigger_DirChangedPre((char_u *)trigger_autocmd, new_dir);

    ಯದಿ (mch_chdir((ವರ್ಣ *)new_dir) != 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (trigger_autocmd != ರಿಕ್ತ)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

#ಯದಿ defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
illegal_slash(ಧ್ರುವ ವರ್ಣ *name)
{
    ಯದಿ (name[0] == NUL)
	ಪ್ರತ್ಯಾಗಮ FALSE;	    // no file name is not illegal
    ಯದಿ (name[strlen(name) - 1] != '/')
	ಪ್ರತ್ಯಾಗಮ FALSE;	    // no trailing slash
    ಯದಿ (mch_isdir((char_u *)name))
	ಪ್ರತ್ಯಾಗಮ FALSE;	    // trailing slash for a directory
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_stat(ಧ್ರುವ ವರ್ಣ *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    ಪ್ರತ್ಯಾಗಮ illegal_slash(name) ? -1 : stat(name, stp);
}
#endif

#ಯದಿ defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_table[SHAPE_IDX_ಎಣಿಕೆ] =
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
ಸ್ಥಿರ string_T mshape_names[] =
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
    {ರಿಕ್ತ, 0}
};

#  define MSHAPE_NAMES_ಎಣಿಕೆ  (ARRAY_LENGTH(mshape_names) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    ವರ್ಣ *
parse_shape_opt(ಪೂರ್ಣಾಙ್ಕ what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;		// init for GCC
    ಪೂರ್ಣಾಙ್ಕ		all_idx;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;
    long	n;
    ಪೂರ್ಣಾಙ್ಕ		found_ve = FALSE;	// found "ve" flag
    ಪೂರ್ಣಾಙ್ಕ		round;

    /*
     * First round: check for errors; second round: do it for real.
     */
    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	ಯದಿ (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	ಅನ್ಯಥಾ
# endif
	    modep = p_guicursor;
	ಯಾವತ್ (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    ಯದಿ (colonp == ರಿಕ್ತ || (commap != ರಿಕ್ತ && commap < colonp))
		ಪ್ರತ್ಯಾಗಮ e_missing_colon_2;
	    ಯದಿ (colonp == modep)
		ಪ್ರತ್ಯಾಗಮ e_illegal_mode;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    ಯಾವತ್ (modep < colonp || all_idx >= 0)
	    {
		ಯದಿ (all_idx < 0)
		{
		    // Find the mode.
		    ಯದಿ (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    ಅನ್ಯಥಾ
			len = 2;
		    ಯದಿ (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_ಎಣಿಕೆ - 1;
		    ಅನ್ಯಥಾ
		    {
			ಪ್ರತಿ (idx = 0; idx < SHAPE_IDX_ಎಣಿಕೆ; ++idx)
			    ಯದಿ (STRNICMP(modep, shape_table[idx].name, len)
									 == 0)
				ಭಙ್ಗ;
			ಯದಿ (idx == SHAPE_IDX_ಎಣಿಕೆ
				   || (shape_table[idx].used_for & what) == 0)
			    ಪ್ರತ್ಯಾಗಮ e_illegal_mode;
			ಯದಿ (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    found_ve = TRUE;
		    }
		    modep += len + 1;
		}

		ಯದಿ (all_idx >= 0)
		    idx = all_idx--;
		ಅನ್ಯಥಾ ಯದಿ (round == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    ಯದಿ (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_table[idx].mshape = 0;
		    }
		    ಅನ್ಯಥಾ
# endif
		    {
			// Set the defaults, for the missing parts
			shape_table[idx].shape = SHAPE_BLOCK;
			shape_table[idx].blinkwait = 700L;
			shape_table[idx].blinkon = 400L;
			shape_table[idx].blinkoff = 250L;
		    }
		}

		// Parse the part after the colon
		ಪ್ರತಿ (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    ಯದಿ (what == SHAPE_MOUSE)
		    {
			ಪ್ರತಿ (i = 0; ; ++i)
			{
			    ಯದಿ (mshape_names[i].string == ರಿಕ್ತ)
			    {
				ಯದಿ (!VIM_ISDIGIT(*p))
				    ಪ್ರತ್ಯಾಗಮ e_illegal_mouseshape;
				ಯದಿ (round == 2)
				    shape_table[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				ಅನ್ಯಥಾ
				    (ಶೂನ್ಯ)getdigits(&p);
				ಭಙ್ಗ;
			    }
			    ಯದಿ (STRNICMP(p, mshape_names[i].string, mshape_names[i].length) == 0)
			    {
				ಯದಿ (round == 2)
				    shape_table[idx].mshape = i;
				p += mshape_names[i].length;
				ಭಙ್ಗ;
			    }
			}
		    }
		    ಅನ್ಯಥಾ // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			ಯದಿ (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			ಯದಿ (len != 0)
			{
			    p += len;
			    ಯದಿ (!VIM_ISDIGIT(*p))
				ಪ್ರತ್ಯಾಗಮ e_digit_expected;
			    n = getdigits(&p);
			    ಯದಿ (len == 3)   // "ver" or "hor"
			    {
				ಯದಿ (n == 0)
				    ಪ್ರತ್ಯಾಗಮ e_illegal_percentage;
				ಯದಿ (round == 2)
				{
				    ಯದಿ (TOLOWER_ASC(i) == 'v')
					shape_table[idx].shape = SHAPE_VER;
				    ಅನ್ಯಥಾ
					shape_table[idx].shape = SHAPE_HOR;
				    shape_table[idx].percentage = n;
				}
			    }
			    ಅನ್ಯಥಾ ಯದಿ (round == 2)
			    {
				ಯದಿ (len == 9)
				    shape_table[idx].blinkwait = n;
				ಅನ್ಯಥಾ ಯದಿ (len == 7)
				    shape_table[idx].blinkon = n;
				ಅನ್ಯಥಾ
				    shape_table[idx].blinkoff = n;
			    }
			}
			ಅನ್ಯಥಾ ಯದಿ (STRNICMP(p, "block", 5) == 0)
			{
			    ಯದಿ (round == 2)
				shape_table[idx].shape = SHAPE_BLOCK;
			    p += 5;
			}
			ಅನ್ಯಥಾ	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    ಯದಿ (commap == ರಿಕ್ತ)		    // last part
			    {
				ಯದಿ (endp == ರಿಕ್ತ)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    ಅನ್ಯಥಾ ಯದಿ (endp > commap || endp == ರಿಕ್ತ)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    ಯದಿ (slashp != ರಿಕ್ತ && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_group(p, (ಪೂರ್ಣಾಙ್ಕ)(slashp - p));
				p = slashp + 1;
			    }
			    ಯದಿ (round == 2)
			    {
				shape_table[idx].id = syn_check_group(p,
							     (ಪೂರ್ಣಾಙ್ಕ)(endp - p));
				shape_table[idx].id_lm = shape_table[idx].id;
				ಯದಿ (slashp != ರಿಕ್ತ && slashp < endp)
				    shape_table[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    ಯದಿ (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    ಯದಿ (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    ಯದಿ (!found_ve)
    {
# ifdef FEAT_MOUSESHAPE
	ಯದಿ (what == SHAPE_MOUSE)
	{
	    shape_table[SHAPE_IDX_VE].mshape = shape_table[SHAPE_IDX_V].mshape;
	}
	ಅನ್ಯಥಾ
# endif
	{
	    shape_table[SHAPE_IDX_VE].shape = shape_table[SHAPE_IDX_V].shape;
	    shape_table[SHAPE_IDX_VE].percentage =
					 shape_table[SHAPE_IDX_V].percentage;
	    shape_table[SHAPE_IDX_VE].blinkwait =
					  shape_table[SHAPE_IDX_V].blinkwait;
	    shape_table[SHAPE_IDX_VE].blinkon =
					    shape_table[SHAPE_IDX_V].blinkon;
	    shape_table[SHAPE_IDX_VE].blinkoff =
					   shape_table[SHAPE_IDX_V].blinkoff;
	    shape_table[SHAPE_IDX_VE].id = shape_table[SHAPE_IDX_V].id;
	    shape_table[SHAPE_IDX_VE].id_lm = shape_table[SHAPE_IDX_V].id_lm;
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

# ಯದಿ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_shape_idx(ಪೂರ್ಣಾಙ್ಕ mouse)
{
#  ifdef FEAT_MOUSESHAPE
    ಯದಿ (mouse && (State == MODE_HITRETURN || State == MODE_ASKMORE))
    {
#   ifdef FEAT_GUI
	ಪೂರ್ಣಾಙ್ಕ x, y;
	gui_mch_getmouse(&x, &y);
	ಯದಿ (Y_2_ROW(y) == Rows - 1)
	    ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_MOREL;
#   endif
	ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_MORE;
    }
    ಯದಿ (mouse && drag_status_ಸಾಲು)
	ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_SDRAG;
    ಯದಿ (mouse && drag_sep_ಸಾಲು)
	ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_VDRAG;
#  endif
    ಯದಿ (!mouse && State == MODE_SHOWMATCH)
	ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_SM;
    ಯದಿ (State & VREPLACE_FLAG)
	ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_R;
    ಯದಿ (State & REPLACE_FLAG)
	ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_R;
    ಯದಿ (State & MODE_ಸೇರಿಸು)
	ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_I;
    ಯದಿ (State & MODE_CMDLINE)
    {
	ಯದಿ (cmdline_at_end())
	    ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_C;
	ಯದಿ (cmdline_overstrike())
	    ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_CR;
	ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_CI;
    }
    ಯದಿ (finish_op)
	ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_O;
    ಯದಿ (VIsual_active)
    {
	ಯದಿ (*p_sel == 'e')
	    ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_VE;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_V;
    }
    ಪ್ರತ್ಯಾಗಮ SHAPE_IDX_N;
}
# endif

# ಯದಿ defined(FEAT_MOUSESHAPE)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    ಶೂನ್ಯ
update_mouseshape(ಪೂರ್ಣಾಙ್ಕ shape_idx)
{
    ಪೂರ್ಣಾಙ್ಕ new_mouse_shape;

    // Only works in GUI mode.
    ಯದಿ (!gui.in_use || gui.starting)
	ಪ್ರತ್ಯಾಗಮ;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    ಯದಿ (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // When ignoring the mouse don't change shape on the statusline.
    ಯದಿ (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_STATUS
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    ಯದಿ (shape_idx == -2
	    && current_mouse_shape != shape_table[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_STATUS].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_VSEP].mshape)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (shape_idx < 0)
	new_mouse_shape = shape_table[get_shape_idx(TRUE)].mshape;
    ಅನ್ಯಥಾ
	new_mouse_shape = shape_table[shape_idx].mshape;
    ಯದಿ (new_mouse_shape != current_mouse_shape)
    {
	mch_set_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#endif // CURSOR_SHAPE

#ಯದಿ defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    ಶೂನ್ಯ
f_getmouseshape(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;
# ಯದಿ defined(FEAT_MOUSESHAPE)
    ಯದಿ (current_mouse_shape >= 0
			      && current_mouse_shape < (ಪೂರ್ಣಾಙ್ಕ)MSHAPE_NAMES_ಎಣಿಕೆ)
	rettv->vval.v_string = vim_strnsave(
				  mshape_names[current_mouse_shape].string,
				  mshape_names[current_mouse_shape].length);
# endif
}
#endif



/*
 * Change directory to "new_dir".  Search 'cdpath' for relative directory
 * names.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    ಪೂರ್ಣಾಙ್ಕ		r;
    char_u	*file_to_find = ರಿಕ್ತ;
    ವರ್ಣ	*ಹುಡುಕು_ctx = ರಿಕ್ತ;

    dir_name = find_directory_in_path(new_dir, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_to_find, &ಹುಡುಕು_ctx);
    vim_free(file_to_find);
    vim_findfile_cleanup(ಹುಡುಕು_ctx);
    ಯದಿ (dir_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    r = mch_chdir((ವರ್ಣ *)dir_name);
    vim_free(dir_name);
    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_user_name(char_u *buf, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಯದಿ (username == ರಿಕ್ತ)
    {
	ಯದಿ (mch_get_user_name(buf, len) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	username = vim_strsave(buf);
    }
    ಅನ್ಯಥಾ
	vim_strncpy(buf, username, len - 1);
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    ಶೂನ್ಯ
free_username(ಶೂನ್ಯ)
{
    vim_free(username);
}
#endif

#ಯದಿ !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    ಶೂನ್ಯ
qsort(
    ಶೂನ್ಯ	*base,
    size_t	elm_ಎಣಿಕೆ,
    size_t	elm_size,
    ಪೂರ್ಣಾಙ್ಕ (*cmp)(ಧ್ರುವ ಶೂನ್ಯ *, ಧ್ರುವ ಶೂನ್ಯ *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		gap;

    buf = alloc(elm_size);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (gap = elm_ಎಣಿಕೆ / 2; gap > 0; gap /= 2)
	ಪ್ರತಿ (i = gap; i < elm_ಎಣಿಕೆ; ++i)
	    ಪ್ರತಿ (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)base + j * elm_size;
		p2 = (char_u *)base + (j + gap) * elm_size;
		ಯದಿ ((*cmp)((ಶೂನ್ಯ *)p1, (ಶೂನ್ಯ *)p2) <= 0)
		    ಭಙ್ಗ;
		// Exchange the elements.
		mch_memmove(buf, p1, elm_size);
		mch_memmove(p1, p2, elm_size);
		mch_memmove(p2, buf, elm_size);
	    }

    vim_free(buf);
}
#endif

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

#ಯದಿ !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  envsize = -1;	// current size of environment
ಬಾಹ್ಯ ವರ್ಣ **environ;		// the global which is your env.

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  findenv(ವರ್ಣ *name); // look for a name in the env.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  newenv(ಶೂನ್ಯ);	// copy env. from stack to heap
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  moreenv(ಶೂನ್ಯ);	// incr. size of env.

    ಪೂರ್ಣಾಙ್ಕ
putenv(ಧ್ರುವ ವರ್ಣ *string)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ವರ್ಣ    *p;

    ಯದಿ (envsize < 0)
    {				// first time putenv called
	ಯದಿ (newenv() < 0)	// copy env. to heap
	    ಪ್ರತ್ಯಾಗಮ -1;
    }

    i = findenv((ವರ್ಣ *)string); // look for name in environment

    ಯದಿ (i < 0)
    {				// name must be added
	ಪ್ರತಿ (i = 0; environ[i]; i++);
	ಯದಿ (i >= (envsize - 1))
	{			// need new slot
	    ಯದಿ (moreenv() < 0)
		ಪ್ರತ್ಯಾಗಮ -1;
	}
	p = alloc(strlen(string) + 1);
	ಯದಿ (p == ರಿಕ್ತ)		// not enough core
	    ಪ್ರತ್ಯಾಗಮ -1;
	environ[i + 1] = 0;	// new end of env.
    }
    ಅನ್ಯಥಾ
    {				// name already in env.
	p = vim_realloc(environ[i], strlen(string) + 1);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
findenv(ವರ್ಣ *name)
{
    ವರ್ಣ    *namechar, *envchar;
    ಪೂರ್ಣಾಙ್ಕ	    i, found;

    found = 0;
    ಪ್ರತಿ (i = 0; environ[i] && !found; i++)
    {
	envchar = environ[i];
	namechar = name;
	ಯಾವತ್ (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	found = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    ಪ್ರತ್ಯಾಗಮ found ? i - 1 : -1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
newenv(ಶೂನ್ಯ)
{
    ವರ್ಣ    **env, *elem;
    ಪೂರ್ಣಾಙ್ಕ	    i, esize;

    ಪ್ರತಿ (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(ವರ್ಣ *, esize);
    ಯದಿ (env == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಪ್ರತಿ (i = 0; environ[i]; i++)
    {
	elem = alloc(strlen(environ[i]) + 1);
	ಯದಿ (elem == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
moreenv(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    esize;
    ವರ್ಣ    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((ವರ್ಣ *)environ, esize * ಪರಿಮಾಣ (*env));
    ಯದಿ (env == 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    environ = env;
    envsize = esize;
    ಪ್ರತ್ಯಾಗಮ 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(ಧ್ರುವ char_u *string)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    char_u *p;

    ಯದಿ (envsize < 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    i = findenv((ವರ್ಣ *)string);

    ಯದಿ (i < 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    p = vim_strchr((char_u *)environ[i], '=');
    ಪ್ರತ್ಯಾಗಮ (p + 1);
}
# endif

#endif // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    ಪೂರ್ಣಾಙ್ಕ
filewritable(char_u *fname)
{
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;
# ಯದಿ defined(UNIX) || defined(VMS)
    ಪೂರ್ಣಾಙ್ಕ		perm = 0;
# endif

# ಯದಿ defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    ಯದಿ (
# ifdef MSWIN
	    mch_writable(fname) &&
# ಅನ್ಯಥಾ
#  ಯದಿ defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((ವರ್ಣ *)fname, W_OK) == 0
       )
    {
	++retval;
	ಯದಿ (mch_isdir(fname))
	    ++retval;
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}
#endif

#ಯದಿ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ಪೂರ್ಣಾಙ್ಕ
get2c(FILE *fd)
{
    ಪೂರ್ಣಾಙ್ಕ		c, n;

    n = getc(fd);
    ಯದಿ (n == EOF) ಪ್ರತ್ಯಾಗಮ -1;
    c = getc(fd);
    ಯದಿ (c == EOF) ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ಪೂರ್ಣಾಙ್ಕ
get3c(FILE *fd)
{
    ಪೂರ್ಣಾಙ್ಕ		c, n;

    n = getc(fd);
    ಯದಿ (n == EOF) ಪ್ರತ್ಯಾಗಮ -1;
    c = getc(fd);
    ಯದಿ (c == EOF) ಪ್ರತ್ಯಾಗಮ -1;
    n = (n << 8) + c;
    c = getc(fd);
    ಯದಿ (c == EOF) ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ಪೂರ್ಣಾಙ್ಕ
get4c(FILE *fd)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    unsigned	n;

    c = getc(fd);
    ಯದಿ (c == EOF) ಪ್ರತ್ಯಾಗಮ -1;
    n = (unsigned)c;
    c = getc(fd);
    ಯದಿ (c == EOF) ಪ್ರತ್ಯಾಗಮ -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    ಯದಿ (c == EOF) ಪ್ರತ್ಯಾಗಮ -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    ಯದಿ (c == EOF) ಪ್ರತ್ಯಾಗಮ -1;
    n = (n << 8) + (unsigned)c;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, ಪೂರ್ಣಾಙ್ಕ cnt)
{
    char_u	*str;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		c;

    // allocate memory
    str = alloc(cnt + 1);
    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Read the string.  Quit when running into the EOF.
    ಪ್ರತಿ (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	ಯದಿ (c == EOF)
	{
	    vim_free(str);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	str[i] = c;
    }
    str[i] = NUL;
    ಪ್ರತ್ಯಾಗಮ str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    ಪೂರ್ಣಾಙ್ಕ
put_bytes(FILE *fd, long_u nr, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = len - 1; i >= 0; --i)
	ಯದಿ (putc((ಪೂರ್ಣಾಙ್ಕ)(nr >> (i * 8)), fd) == EOF)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

#endif

#ifndef PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    long
elapsed(ಸಂರಚನಾ timeval *start_tv)
{
    ಸಂರಚನಾ timeval  now_tv;

    gettimeofday(&now_tv, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ (now_tv.tv_sec - start_tv->tv_sec) * 1000L
	 + (now_tv.tv_usec - start_tv->tv_usec) / 1000L;
}
# endif

# ifdef ELAPSED_TICKCOUNT
/*
 * Return time in msec since "start_tick".
 */
    long
elapsed(DWORD start_tick)
{
    DWORD	now = Get_Tick_ಎಣಿಕೆ();

    ಪ್ರತ್ಯಾಗಮ (long)now - (long)start_tick;
}
# endif
#endif

#ಯದಿ defined(FEAT_JOB_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_parse_cmd(char_u *cmd, ಪೂರ್ಣಾಙ್ಕ use_shcf, ವರ್ಣ ***argv, ಪೂರ್ಣಾಙ್ಕ *argc)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p, *d;
    ಪೂರ್ಣಾಙ್ಕ		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    ಪ್ರತಿ (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	ಯಾವತ್ (*p != NUL)
	{
	    ಯದಿ (i == 2)
		(*argv)[*argc] = (ವರ್ಣ *)p;
	    ++*argc;
	    d = p;
	    ಯಾವತ್ (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		ಯದಿ (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    ಯದಿ (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    ಯದಿ (*p == NUL)
	    {
		ಯದಿ (i == 2)
		    *d++ = NUL;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	ಯದಿ (*argv == ರಿಕ್ತ)
	{
	    ಯದಿ (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		ಪ್ರತಿ (;;)
		{
		    p = skiptowhite(p);
		    ಯದಿ (*p == NUL)
			ಭಙ್ಗ;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(ವರ್ಣ *, *argc + 4);
	    ಯದಿ (*argv == ರಿಕ್ತ)	    // out of memory
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
build_argv_from_string(char_u *cmd, ವರ್ಣ ***argv, ಪೂರ್ಣಾಙ್ಕ *argc)
{
    char_u	*cmd_copy;
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    ಯದಿ (cmd_copy == ರಿಕ್ತ
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತಿ (i = 0; i < *argc; i++)
	(*argv)[i] = (ವರ್ಣ *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = ರಿಕ್ತ;
    vim_free(cmd_copy);
    ಪ್ರತ್ಯಾಗಮ OK;
}

#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
build_argv_from_list(list_T *l, ವರ್ಣ ***argv, ಪೂರ್ಣಾಙ್ಕ *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(ವರ್ಣ *, l->lv_len + 1);
    ಯದಿ (*argv == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    *argc = 0;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ಯದಿ (s == ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ i;

	    ಪ್ರತಿ (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	(*argv)[*argc] = (ವರ್ಣ *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Change the behavior of vterm.
 * 0: As usual.
 * 1: Windows 10 version 1809
 *      The bug causes unstable handling of ambiguous width character.
 * 2: Windows 10 version 1903 & 1909
 *      Use the wrong result because each result is different.
 * 3: Windows 10 insider preview (current latest logic)
 */
    ಪೂರ್ಣಾಙ್ಕ
get_special_pty_type(ಶೂನ್ಯ)
{
#ifdef MSWIN
    ಪ್ರತ್ಯಾಗಮ get_conpty_type();
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

// compare two keyvalue_T structs by case sensitive value
    ಪೂರ್ಣಾಙ್ಕ
cmp_keyvalue_ಮೌಲ್ಯ(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ಪ್ರತ್ಯಾಗಮ STRCMP(kv1->ಮೌಲ್ಯ.string, kv2->ಮೌಲ್ಯ.string);
}

// compare two keyvalue_T structs by value with length
    ಪೂರ್ಣಾಙ್ಕ
cmp_keyvalue_ಮೌಲ್ಯ_n(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ಪ್ರತ್ಯಾಗಮ STRNCMP(kv1->ಮೌಲ್ಯ.string, kv2->ಮೌಲ್ಯ.string, MAX(kv1->ಮೌಲ್ಯ.length,
		kv2->ಮೌಲ್ಯ.length));
}

// compare two keyvalue_T structs by case insensitive value
    ಪೂರ್ಣಾಙ್ಕ
cmp_keyvalue_ಮೌಲ್ಯ_i(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ಪ್ರತ್ಯಾಗಮ STRICMP(kv1->ಮೌಲ್ಯ.string, kv2->ಮೌಲ್ಯ.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    ಪೂರ್ಣಾಙ್ಕ
cmp_keyvalue_ಮೌಲ್ಯ_ni(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ಪ್ರತ್ಯಾಗಮ vim_strnicmp_asc((ವರ್ಣ *)kv1->ಮೌಲ್ಯ.string,
	    (ವರ್ಣ *)kv2->ಮೌಲ್ಯ.string, MAX(kv1->ಮೌಲ್ಯ.length,
		    kv2->ಮೌಲ್ಯ.length));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    ಶೂನ್ಯ *
mergesort_list(
    ಶೂನ್ಯ	*head,
    ಶೂನ್ಯ	*(*get_next)(ಶೂನ್ಯ *),
    ಶೂನ್ಯ	(*set_next)(ಶೂನ್ಯ *, ಶೂನ್ಯ *),
    ಶೂನ್ಯ	*(*get_prev)(ಶೂನ್ಯ *),
    ಶೂನ್ಯ	(*set_prev)(ಶೂನ್ಯ *, ಶೂನ್ಯ *),
    ಪೂರ್ಣಾಙ್ಕ		(*compare)(ಧ್ರುವ ಶೂನ್ಯ *, ಧ್ರುವ ಶೂನ್ಯ *))
{
    ಯದಿ (!head || !get_next(head))
	ಪ್ರತ್ಯಾಗಮ head;

    // Count length
    ಪೂರ್ಣಾಙ್ಕ	    n = 0;
    ಶೂನ್ಯ*   curr = head;
    ಯಾವತ್ (curr)
    {
	n++;
	curr = get_next(curr);
    }

    ಪೂರ್ಣಾಙ್ಕ	size;
    ಪ್ರತಿ (size = 1; size < n; size *= 2)
    {
	ಶೂನ್ಯ*	new_head = ರಿಕ್ತ;
	ಶೂನ್ಯ*	tail = ರಿಕ್ತ;
	curr = head;

	ಯಾವತ್ (curr)
	{
	    // Split two runs
	    ಶೂನ್ಯ    *ಎಡ = curr;
	    ಶೂನ್ಯ    *ಬಲ = ಎಡ;
	    ಪೂರ್ಣಾಙ್ಕ	    i;
	    ಪ್ರತಿ (i = 0; i < size && ಬಲ; ++i)
		ಬಲ = get_next(ಬಲ);

	    ಶೂನ್ಯ    *next = ಬಲ;
	    ಪ್ರತಿ (i = 0; i < size && next; ++i)
		next = get_next(next);

	    // Break links
	    ಶೂನ್ಯ    *l_end = ಬಲ ? get_prev(ಬಲ) : ರಿಕ್ತ;
	    ಯದಿ (l_end)
		set_next(l_end, ರಿಕ್ತ);
	    ಯದಿ (ಬಲ)
		set_prev(ಬಲ, ರಿಕ್ತ);

	    ಶೂನ್ಯ    *r_end = next ? get_prev(next) : ರಿಕ್ತ;
	    ಯದಿ (r_end)
		set_next(r_end, ರಿಕ್ತ);
	    ಯದಿ (next)
		set_prev(next, ರಿಕ್ತ);

	    // Merge
	    ಶೂನ್ಯ    *merged = ರಿಕ್ತ;
	    ಶೂನ್ಯ    *merged_tail = ರಿಕ್ತ;

	    ಯಾವತ್ (ಎಡ || ಬಲ)
	    {
		ಶೂನ್ಯ	*chosen = ರಿಕ್ತ;
		ಯದಿ (!ಎಡ)
		{
		    chosen = ಬಲ;
		    ಬಲ = get_next(ಬಲ);
		}
		ಅನ್ಯಥಾ ಯದಿ (!ಬಲ)
		{
		    chosen = ಎಡ;
		    ಎಡ = get_next(ಎಡ);
		}
		ಅನ್ಯಥಾ ಯದಿ (compare(ಎಡ, ಬಲ) <= 0)
		{
		    chosen = ಎಡ;
		    ಎಡ = get_next(ಎಡ);
		}
		ಅನ್ಯಥಾ
		{
		    chosen = ಬಲ;
		    ಬಲ = get_next(ಬಲ);
		}

		ಯದಿ (merged_tail)
		{
		    set_next(merged_tail, chosen);
		    set_prev(chosen, merged_tail);
		    merged_tail = chosen;
		}
		ಅನ್ಯಥಾ
		{
		    merged = merged_tail = chosen;
		    set_prev(chosen, ರಿಕ್ತ);
		}
	    }

	    // Connect to full list
	    ಯದಿ (!new_head)
		new_head = merged;
	    ಅನ್ಯಥಾ
	    {
		set_next(tail, merged);
		set_prev(merged, tail);
	    }

	    // Move tail to end
	    ಯಾವತ್ (get_next(merged_tail))
		merged_tail = get_next(merged_tail);
	    tail = merged_tail;

	    curr = next;
	}

	head = new_head;
    }

    ಪ್ರತ್ಯಾಗಮ head;
}

ಸ್ಥಿರ ಧ್ರುವ char_u base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

ಸ್ಥಿರ char_u base64_dec_table[256];

#define BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_base64_dec_table(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ base64_dec_tbl_initialized = FALSE;

    ಯದಿ (base64_dec_tbl_initialized)
	ಪ್ರತ್ಯಾಗಮ;

    vim_memset(base64_dec_table, 0xFF, ಪರಿಮಾಣ(base64_dec_table));
    ಪ್ರತಿ (size_t i = 0; i < ಪರಿಮಾಣ(base64_table) - 1; i++)
	base64_dec_table[(char_u)base64_table[i]] = (char_u)i;
    base64_dec_table['='] = 0;

    base64_dec_tbl_initialized = TRUE;
}

/*
 * Base64 encode "src[len]" into caller-supplied "dst", which must have room for
 * at least BASE64_ENCODED_LEN(len) bytes (NUL terminated). Returns the number
 * of bytes written, excluding the NUL.
 */
    long
base64_encode_buf(char_u *dst, ಧ್ರುವ char_u *src, size_t len)
{
    size_t i, j;

    ಪ್ರತಿ (i = 0, j = 0; i < len;)
    {
	size_t remaining = len - i;

	int_u octet_a = src[i++];
	int_u octet_b = remaining > 1 ? src[i++] : 0;
	int_u octet_c = remaining > 2 ? src[i++] : 0;

	int_u triple = (octet_a << 16) | (octet_b << 8) | octet_c;

	dst[j++] = base64_table[(triple >> 18) & 0x3F];
	dst[j++] = base64_table[(triple >> 12) & 0x3F];
	dst[j++] = remaining > 1
	    ? base64_table[(triple >> 6) & 0x3F]
	    : '=';
	dst[j++] = remaining > 2
	    ? base64_table[triple & 0x3F]
	    : '=';
    }

    dst[j] = NUL;
    ಪ್ರತ್ಯಾಗಮ (long)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(ಧ್ರುವ char_u *data, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    ಯದಿ (encoded == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    base64_encode_buf(encoded, data, len);
    ಪ್ರತ್ಯಾಗಮ encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
base64_decode(ಧ್ರುವ char_u *data, size_t len, garray_T *out)
{
    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (len % 4 != 0)
    {
	semsg(_(e_invalid_argument_str), data);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    init_base64_dec_table();

    size_t decoded_len = (len / 4) * 3;
    ಯದಿ (data[len - 1] == '=')
	decoded_len--;
    ಯದಿ (data[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    ಪ್ರತಿ (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_table[(char_u)data[i++]];
	int_u sextet_b = base64_dec_table[(char_u)data[i++]];
	int_u sextet_c = base64_dec_table[(char_u)data[i++]];
	int_u sextet_d = base64_dec_table[(char_u)data[i++]];

	ಯದಿ (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_argument_str), data);
	    ga_clear(out);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	ಯದಿ (j < decoded_len)
	{
	    ga_append(out, (triple >> 16) & 0xFF);
	    j++;
	}
	ಯದಿ (j < decoded_len)
	{
	    ga_append(out, (triple >> 8) & 0xFF);
	    j++;
	}
	ಯದಿ (j < decoded_len)
	{
	    ga_append(out, triple & 0xFF);
	    j++;
	}

	ಯದಿ (j == decoded_len)
	{
	    ಯದಿ ((data[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((data[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_argument_str), data);
		ga_clear(out);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}
