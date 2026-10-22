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
#include "vim.h"

static char_u	*username = NULL; // cached result of mch_get_user_name()

static puur_naa_nka coladvance2(pos_T *pos, puur_naa_nka addspaces, puur_naa_nka finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    puur_naa_nka
virtual_active(void)
{
    unsigned puur_naa_nka cur_ve_flags = get_ve_flags();

    ᮚᮦᮔ᮪ (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_INSERT) && (State & MODE_INSERT)))
	ᮝᮜᮤ TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    ᮚᮦᮔ᮪ (virtual_op != MAYBE)
	ᮝᮜᮤ virtual_op;
    ᮝᮜᮤ (cur_ve_flags & VE_BLOCK) && VIsual_active && VIsual_mode == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    puur_naa_nka
getviscol(void)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, NULL, NULL, GETVCOL_NO_ABOVE);
    ᮝᮜᮤ (puur_naa_nka)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    puur_naa_nka
coladvance_force(colnr_T wcol)
{
    puur_naa_nka rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    ᮚᮦᮔ᮪ (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ᮜᮤᮚ
	// Virtcol is valid
	set_valid_virtcol(curwin, wcol);
    ᮝᮜᮤ rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    puur_naa_nka
getviscol2(colnr_T col, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = col;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, NULL, NULL, GETVCOL_NO_ABOVE);
    ᮝᮜᮤ (puur_naa_nka)x;
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
    puur_naa_nka
coladvance(colnr_T wantcol)
{
    puur_naa_nka rc = getvpos(&curwin->w_cursor, wantcol);

    ᮚᮦᮔ᮪ (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	set_valid_virtcol(curwin, wantcol);
    ᮝᮜᮤ rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    puur_naa_nka
getvpos(pos_T *pos, colnr_T wantcol)
{
    ᮝᮜᮤ coladvance2(pos, FALSE, virtual_active(), wantcol);
}

    static puur_naa_nka
coladvance2(
    pos_T	*pos,
    puur_naa_nka		addspaces,	// change the text to achieve our goal?
    puur_naa_nka		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    puur_naa_nka		idx;
    char_u	*line;
    puur_naa_nka		linelen;
    colnr_T	col = 0;
    puur_naa_nka		csize = 0;
    puur_naa_nka		one_more;
#ifdef FEAT_LINEBREAK
    puur_naa_nka		head = 0;
#endif

    one_more = (State & MODE_INSERT)
		    || restart_edit != NUL
		    || (VIsual_active && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    line = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    ᮚᮦᮔ᮪ (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_more;
	    col = wcol;

	    ᮚᮦᮔ᮪ ((addspaces || finetune) && !VIsual_active)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_more;
		ᮚᮦᮔ᮪ (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    ᮜᮤᮚ
    {
	puur_naa_nka		width = curwin->w_width - win_col_off(curwin);
	chartabsize_T	cts;

	ᮚᮦᮔ᮪ (finetune
		&& curwin->w_p_wrap
		&& curwin->w_width != 0
		&& wcol >= (colnr_T)width
		&& width > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    ᮚᮦᮔ᮪ (csize > 0)
		csize--;

	    ᮚᮦᮔ᮪ (wcol / width > (colnr_T)csize / width
		    && ((State & MODE_INSERT) == 0 || (puur_naa_nka)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / width + 1) * width - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, line, line);
#ifdef FEAT_PROP_POPUP
	cts.cts_no_above = true;
#endif
	ᮞᮨᮓᮨᮜ (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ifdef FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &head, NULL);
	    MB_PTR_ADV(cts.cts_ptr);
#ᮜᮤᮚ
	    csize = lbr_chartabsize_adv(&cts);
#endif
	    cts.cts_vcol += csize;
	}
	col = cts.cts_vcol;
	idx = (puur_naa_nka)(cts.cts_ptr - line);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	ᮚᮦᮔ᮪ (col > wcol || (!virtual_active() && one_more == 0))
	{
	    idx -= 1;
#ifdef FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= head;
#endif
	    col -= csize;
	}

	ᮚᮦᮔ᮪ (virtual_active()
		&& addspaces
		&& wcol >= 0
		&& ((col != wcol && col != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    ᮚᮦᮔ᮪ (line[idx] == NUL)
	    {
		// Append spaces
		puur_naa_nka	correct = wcol - col;
		char_u	*newline = alloc(idx + correct + 1);
		puur_naa_nka	t;

		ᮚᮦᮔ᮪ (newline == NULL)
		    ᮝᮜᮤ FAIL;

		for (t = 0; t < idx; ++t)
		    newline[t] = line[t];

		for (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, (colnr_T)idx);
		idx += correct;
		col = wcol;
	    }
	    ᮜᮤᮚ
	    {
		// Break a tab
		puur_naa_nka	correct = wcol - col - csize + 1; // negative!!
		char_u	*newline;
		puur_naa_nka	t, s = 0;
		puur_naa_nka	v;

		ᮚᮦᮔ᮪ (-correct > csize)
		    ᮝᮜᮤ FAIL;

		newline = alloc(linelen + csize);
		ᮚᮦᮔ᮪ (newline == NULL)
		    ᮝᮜᮤ FAIL;

		for (t = 0; t < linelen; t++)
		{
		    ᮚᮦᮔ᮪ (t != idx)
			newline[s++] = line[t];
		    ᮜᮤᮚ
			for (v = 0; v < csize; v++)
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

    ᮚᮦᮔ᮪ (idx < 0)
	pos->col = 0;
    ᮜᮤᮚ
	pos->col = idx;

    pos->coladd = 0;

    ᮚᮦᮔ᮪ (finetune)
    {
	ᮚᮦᮔ᮪ (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    ᮚᮦᮔ᮪ (!one_more)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, NULL, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	ᮜᮤᮚ
	{
	    puur_naa_nka b = (puur_naa_nka)wcol - (puur_naa_nka)col;

	    // The difference between wcol and col is used to set coladd.
	    ᮚᮦᮔ᮪ (b > 0 && b < (MAXCOL - 2 * curwin->w_width))
		pos->coladd = b;

	    col += b;
	}
    }

    // prevent from moving onto a trail byte
    ᮚᮦᮔ᮪ (has_mbyte)
	mb_adjustpos(curbuf, pos);

    ᮚᮦᮔ᮪ (wcol < 0 || col < wcol)
	ᮝᮜᮤ FAIL;
    ᮝᮜᮤ OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    puur_naa_nka
inc_cursor(void)
{
    ᮝᮜᮤ inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    puur_naa_nka
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    ᮚᮦᮔ᮪ (lp->col != MAXCOL)
    {
	p = ml_get_pos(lp);
	ᮚᮦᮔ᮪ (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    ᮚᮦᮔ᮪ (has_mbyte)
	    {
		puur_naa_nka l = (*mb_ptr2len)(p);

		lp->col += l;
		ᮝᮜᮤ ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->col++;
	    lp->coladd = 0;
	    ᮝᮜᮤ ((p[1] != NUL) ? 0 : 2);
	}
    }
    ᮚᮦᮔ᮪ (lp->lnum != curbuf->b_ml.ml_line_count)     // there is a next line
    {
	lp->col = 0;
	lp->lnum++;
	lp->coladd = 0;
	ᮝᮜᮤ 1;
    }
    ᮝᮜᮤ -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    puur_naa_nka
incl(pos_T *lp)
{
    puur_naa_nka	    r;

    ᮚᮦᮔ᮪ ((r = inc(lp)) >= 1 && lp->col)
	r = inc(lp);
    ᮝᮜᮤ r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    puur_naa_nka
dec_cursor(void)
{
    ᮝᮜᮤ dec(&curwin->w_cursor);
}

    puur_naa_nka
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    ᮚᮦᮔ᮪ (lp->col == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	ᮚᮦᮔ᮪ (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	ᮝᮜᮤ 0;
    }

    ᮚᮦᮔ᮪ (lp->col > 0)
    {
	// still within line
	lp->col--;
	ᮚᮦᮔ᮪ (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	}
	ᮝᮜᮤ 0;
    }

    ᮚᮦᮔ᮪ (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	ᮚᮦᮔ᮪ (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	ᮝᮜᮤ 1;
    }

    // at start of file
    ᮝᮜᮤ -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    puur_naa_nka
decl(pos_T *lp)
{
    puur_naa_nka	    r;

    ᮚᮦᮔ᮪ ((r = dec(lp)) == 1 && lp->col)
	r = dec(lp);
    ᮝᮜᮤ r;
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
    ᮚᮦᮔ᮪ (hasAnyFolding(wp))
    {
	ᮚᮦᮔ᮪ (lnum > cursor)
	{
	    ᮞᮨᮓᮨᮜ (lnum > cursor)
	    {
		(void)hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		ᮚᮦᮔ᮪ (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (lnum < cursor)
	{
	    ᮞᮨᮓᮨᮜ (lnum < cursor)
	    {
		(void)hasFoldingWin(wp, lnum, NULL, &lnum, TRUE, NULL);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		ᮚᮦᮔ᮪ (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    ᮜᮤᮚ
#endif
	retval = lnum - cursor;

    ᮝᮜᮤ retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    void
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    ᮚᮦᮔ᮪ (pos->lnum > buf->b_ml.ml_line_count)
	pos->lnum = buf->b_ml.ml_line_count;

    ᮚᮦᮔ᮪ (pos->col > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	ᮚᮦᮔ᮪ (pos->col > len)
	    pos->col = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    void
check_cursor_lnum(void)
{
    ᮚᮦᮔ᮪ (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
#ifdef FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	ᮚᮦᮔ᮪ (!hasFolding(curbuf->b_ml.ml_line_count,
						&curwin->w_cursor.lnum, NULL))
#endif
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    }
    ᮚᮦᮔ᮪ (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    void
check_cursor_col(void)
{
    check_cursor_col_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    void
check_cursor_col_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.col;
    colnr_T      oldcoladd = win->w_cursor.col + win->w_cursor.coladd;
    unsigned puur_naa_nka cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_buffer, win->w_cursor.lnum);
    ᮚᮦᮔ᮪ (len == 0)
	win->w_cursor.col = 0;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (win->w_cursor.col >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	ᮚᮦᮔ᮪ ((State & MODE_INSERT) || restart_edit
		|| (VIsual_active && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_active())
	    win->w_cursor.col = len;
	ᮜᮤᮚ
	{
	    win->w_cursor.col = len - 1;
	    // Move the cursor to the head byte.
	    ᮚᮦᮔ᮪ (has_mbyte)
		mb_adjustpos(win->w_buffer, &win->w_cursor);
	}
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (win->w_cursor.col < 0)
	win->w_cursor.col = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    ᮚᮦᮔ᮪ (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (cur_ve_flags == VE_ALL)
    {
	ᮚᮦᮔ᮪ (oldcoladd > win->w_cursor.col)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.col;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    ᮚᮦᮔ᮪ (win->w_cursor.col + 1 < len)
	    {
		puur_naa_nka cs, ce;

		getvcol(win, &win->w_cursor, &cs, NULL, &ce, 0);
		ᮚᮦᮔ᮪ (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	ᮜᮤᮚ
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    void
check_cursor(void)
{
    check_cursor_lnum();
    check_cursor_col();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    void
check_visual_pos(void)
{
    ᮚᮦᮔ᮪ (VIsual.lnum > curbuf->b_ml.ml_line_count)
    {
	VIsual.lnum = curbuf->b_ml.ml_line_count;
	VIsual.col = 0;
	VIsual.coladd = 0;
    }
    ᮜᮤᮚ
    {
	puur_naa_nka len = ml_get_len(VIsual.lnum);

	ᮚᮦᮔ᮪ (VIsual.col > len)
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
    void
adjust_cursor_col(void)
{
    ᮚᮦᮔ᮪ (curwin->w_cursor.col > 0
	    && (!VIsual_active || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    puur_naa_nka
set_leftcol(colnr_T leftcol)
{
    puur_naa_nka		retval = FALSE;

    // Return quickly when there is no change.
    ᮚᮦᮔ᮪ (curwin->w_leftcol == leftcol)
	ᮝᮜᮤ FALSE;
    curwin->w_leftcol = leftcol;

    changed_cline_bef_curs();
    long lastcol = curwin->w_leftcol + curwin->w_width - curwin_col_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    long siso = get_sidescrolloff_value();
    ᮚᮦᮔ᮪ (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(void)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, NULL, &e, 0);
    ᮚᮦᮔ᮪ (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (s < curwin->w_leftcol)
    {
	retval = TRUE;
	ᮚᮦᮔ᮪ (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    changed_cline_bef_curs();
	}
    }

    ᮚᮦᮔ᮪ (retval)
	curwin->w_set_curswant = true;
    redraw_later(UPD_NOT_VALID);
    ᮝᮜᮤ retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    puur_naa_nka
copy_option_part(
    char_u	**option,
    char_u	*buf,
    puur_naa_nka		maxlen,
    var_na	*sep_chars)
{
    puur_naa_nka	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    ᮚᮦᮔ᮪ (*p == '.')
	buf[len++] = *p++;
    ᮞᮨᮓᮨᮜ (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == NULL)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	ᮚᮦᮔ᮪ (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != NULL)
	    ++p;
	ᮚᮦᮔ᮪ (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    ᮚᮦᮔ᮪ (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_to_option_part(p);	// p points to next file name

    *option = p;
    ᮝᮜᮤ len;
}

#ᮚᮦᮔ᮪ !defined(HAVE_MEMSET) && !defined(PROTO)
    void *
vim_memset(void *ptr, puur_naa_nka c, size_t size)
{
    var_na *p = ptr;

    ᮞᮨᮓᮨᮜ (size-- > 0)
	*p++ = c;
    ᮝᮜᮤ ptr;
}
#endif

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    puur_naa_nka
vim_isspace(puur_naa_nka x)
{
    ᮝᮜᮤ ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

static sa_nracanaa modmasktable
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
#ᮚᮦᮔ᮪ defined(MACOS_X) || defined(FEAT_GUI_GTK)
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
#define MOD_KEYS_ENTRY_SIZE 5

static char_u modifier_keys_table[] =
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
    MOD_MASK_CTRL,  KS_EXTRA, (puur_naa_nka)KE_C_END,	'@', '7',	// end
    MOD_MASK_SHIFT, '*', '9',			'@', '9',	// exit
    MOD_MASK_SHIFT, '*', '0',			'@', '0',	// find
    MOD_MASK_SHIFT, '#', '1',			'%', '1',	// help
    MOD_MASK_SHIFT, '#', '2',			'k', 'h',	// home
    MOD_MASK_CTRL,  KS_EXTRA, (puur_naa_nka)KE_C_HOME,	'k', 'h',	// home
    MOD_MASK_SHIFT, '#', '3',			'k', 'I',	// insert
    MOD_MASK_SHIFT, '#', '4',			'k', 'l',	// left arrow
    MOD_MASK_CTRL,  KS_EXTRA, (puur_naa_nka)KE_C_LEFT,	'k', 'l',	// left arrow
    MOD_MASK_SHIFT, '%', 'a',			'%', '3',	// message
    MOD_MASK_SHIFT, '%', 'b',			'%', '4',	// move
    MOD_MASK_SHIFT, '%', 'c',			'%', '5',	// next
    MOD_MASK_SHIFT, '%', 'd',			'%', '7',	// options
    MOD_MASK_SHIFT, '%', 'e',			'%', '8',	// previous
    MOD_MASK_SHIFT, '%', 'f',			'%', '9',	// print
    MOD_MASK_SHIFT, '%', 'g',			'%', '0',	// redo
    MOD_MASK_SHIFT, '%', 'h',			'&', '3',	// replace
    MOD_MASK_SHIFT, '%', 'i',			'k', 'r',	// right arr.
    MOD_MASK_CTRL,  KS_EXTRA, (puur_naa_nka)KE_C_RIGHT,	'k', 'r',	// right arr.
    MOD_MASK_SHIFT, '%', 'j',			'&', '5',	// resume
    MOD_MASK_SHIFT, '!', '1',			'&', '6',	// save
    MOD_MASK_SHIFT, '!', '2',			'&', '7',	// suspend
    MOD_MASK_SHIFT, '!', '3',			'&', '8',	// undo
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_UP,	'k', 'u',	// up arrow
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_DOWN,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_XF1,	KS_EXTRA, (puur_naa_nka)KE_XF1,
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_XF2,	KS_EXTRA, (puur_naa_nka)KE_XF2,
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_XF3,	KS_EXTRA, (puur_naa_nka)KE_XF3,
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_XF4,	KS_EXTRA, (puur_naa_nka)KE_XF4,

    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F1,	'k', '1',	// F1
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F2,	'k', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F3,	'k', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F4,	'k', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F5,	'k', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F6,	'k', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F7,	'k', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F8,	'k', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F9,	'k', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F10,	'k', ';',	// F10

    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F11,	'F', '1',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F12,	'F', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F13,	'F', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F14,	'F', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F15,	'F', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F16,	'F', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F17,	'F', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F18,	'F', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F19,	'F', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F20,	'F', 'A',

    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F21,	'F', 'B',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F22,	'F', 'C',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F23,	'F', 'D',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F24,	'F', 'E',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F25,	'F', 'F',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F26,	'F', 'G',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F27,	'F', 'H',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F28,	'F', 'I',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F29,	'F', 'J',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F30,	'F', 'K',

    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F31,	'F', 'L',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F32,	'F', 'M',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F33,	'F', 'N',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F34,	'F', 'O',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F35,	'F', 'P',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F36,	'F', 'Q',
    MOD_MASK_SHIFT, KS_EXTRA, (puur_naa_nka)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_MASK_SHIFT, 'k', 'B',			KS_EXTRA, (puur_naa_nka)KE_TAB,

    NUL
};

static sa_nracanaa key_name_entry
{
    puur_naa_nka		enabled;	    // is this entry available (TRUE/FALSE)?
    puur_naa_nka		key;		    // special key code or ascii value
    string_T	name;		    // name of key
    puur_naa_nka		is_alt;		    // is an alternative name
} key_names_table[] =
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
#ᮜᮤᮚ
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
#ᮜᮤᮚ
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
    {TRUE, K_LEFT, STR_LITERAL_INIT("Left"), FALSE},
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
#ᮜᮤᮚ
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
#ᮜᮤᮚ
    FALSE,
#endif
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_RIGHT, STR_LITERAL_INIT("Right"), FALSE},
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
#ᮜᮤᮚ
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
#ᮜᮤᮚ
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
    static puur_naa_nka
name_to_mod_mask(puur_naa_nka c)
{
    puur_naa_nka	    i;

    c = TOUPPER_ASC(c);
    for (i = 0; mod_mask_table[i].mod_mask != 0; i++)
	ᮚᮦᮔ᮪ (c == mod_mask_table[i].name)
	    ᮝᮜᮤ mod_mask_table[i].mod_flag;
    ᮝᮜᮤ 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    puur_naa_nka
simplify_key(puur_naa_nka key, puur_naa_nka *modifiers)
{
    puur_naa_nka	    i;
    puur_naa_nka	    key0;
    puur_naa_nka	    key1;

    ᮚᮦᮔ᮪ (!(*modifiers & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	ᮝᮜᮤ key;

    // TAB is a special case
    ᮚᮦᮔ᮪ (key == TAB && (*modifiers & MOD_MASK_SHIFT))
    {
	*modifiers &= ~MOD_MASK_SHIFT;
	ᮝᮜᮤ K_S_TAB;
    }
    key0 = KEY2TERMCAP0(key);
    key1 = KEY2TERMCAP1(key);
    for (i = 0; modifier_keys_table[i] != NUL; i += MOD_KEYS_ENTRY_SIZE)
    {
	ᮚᮦᮔ᮪ (key0 == modifier_keys_table[i + 3]
		&& key1 == modifier_keys_table[i + 4]
		&& (*modifiers & modifier_keys_table[i]))
	{
	    *modifiers &= ~modifier_keys_table[i];
	    ᮝᮜᮤ TERMCAP2KEY(modifier_keys_table[i + 1],
		    modifier_keys_table[i + 2]);
	}
    }
    ᮝᮜᮤ key;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    puur_naa_nka
handle_x_keys(puur_naa_nka key)
{
    switch (key)
    {
	pak_sa K_XUP:	ᮝᮜᮤ K_UP;
	pak_sa K_XDOWN:	ᮝᮜᮤ K_DOWN;
	pak_sa K_XLEFT:	ᮝᮜᮤ K_LEFT;
	pak_sa K_XRIGHT:	ᮝᮜᮤ K_RIGHT;
	pak_sa K_XHOME:	ᮝᮜᮤ K_HOME;
	pak_sa K_ZHOME:	ᮝᮜᮤ K_HOME;
	pak_sa K_XEND:	ᮝᮜᮤ K_END;
	pak_sa K_ZEND:	ᮝᮜᮤ K_END;
	pak_sa K_XF1:	ᮝᮜᮤ K_F1;
	pak_sa K_XF2:	ᮝᮜᮤ K_F2;
	pak_sa K_XF3:	ᮝᮜᮤ K_F3;
	pak_sa K_XF4:	ᮝᮜᮤ K_F4;
	pak_sa K_S_XF1:	ᮝᮜᮤ K_S_F1;
	pak_sa K_S_XF2:	ᮝᮜᮤ K_S_F2;
	pak_sa K_S_XF3:	ᮝᮜᮤ K_S_F3;
	pak_sa K_S_XF4:	ᮝᮜᮤ K_S_F4;
    }
    ᮝᮜᮤ key;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_key_name(puur_naa_nka c, puur_naa_nka modifiers)
{
    static char_u string[MAX_KEY_NAME_LEN + 1];
    puur_naa_nka	    i, idx, len;
    puur_naa_nka	    table_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    ᮚᮦᮔ᮪ (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_KEY)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    ᮚᮦᮔ᮪ (IS_SPECIAL(c))
    {
	for (i = 0; modifier_keys_table[i] != 0; i += MOD_KEYS_ENTRY_SIZE)
	    ᮚᮦᮔ᮪ (       KEY2TERMCAP0(c) == (puur_naa_nka)modifier_keys_table[i + 1]
		    && (puur_naa_nka)KEY2TERMCAP1(c) == (puur_naa_nka)modifier_keys_table[i + 2])
	    {
		modifiers |= modifier_keys_table[i];
		c = TERMCAP2KEY(modifier_keys_table[i + 3],
						   modifier_keys_table[i + 4]);
		bha_nga;
	    }
    }

    // try to find the key in the special key table
    table_idx = find_special_key_in_table(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    ᮚᮦᮔ᮪ (c > 0 && (*mb_char2len)(c) == 1)
    {
	ᮚᮦᮔ᮪ (table_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_MASK_ALT;
	    // try again, to find the un-alted key in the special key table
	    table_idx = find_special_key_in_table(c);
	}
	ᮚᮦᮔ᮪ (table_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_MASK_CTRL;
	}
    }

    // translate the modifier into a string
    for (i = 0; mod_mask_table[i].name != 'A'; i++)
	ᮚᮦᮔ᮪ ((modifiers & mod_mask_table[i].mod_mask)
						== mod_mask_table[i].mod_flag)
	{
	    string[idx++] = mod_mask_table[i].name;
	    string[idx++] = (char_u)'-';
	}

    ᮚᮦᮔ᮪ (table_idx < 0)		// unknown special key, may output t_xx
    {
	ᮚᮦᮔ᮪ (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	ᮜᮤᮚ
	{
	    len = (*mb_char2len)(c);
	    ᮚᮦᮔ᮪ (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    ᮜᮤᮚ
	    {
		char_u	*s = transchar(c);
		ᮞᮨᮓᮨᮜ (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    ᮜᮤᮚ		// use name of special key
    {
	string_T    *s;

	s = &key_names_table[table_idx].name;

	ᮚᮦᮔ᮪ (s->length + idx + 2 <= MAX_KEY_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (puur_naa_nka)s->length;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    ᮝᮜᮤ string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    puur_naa_nka
trans_special(
    char_u	**srcp,
    char_u	*dst,
    puur_naa_nka		flags,		// FSK_ values
    puur_naa_nka		escape_ks,	// escape K_SPECIAL bytes in the character
    puur_naa_nka		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    puur_naa_nka		modifiers = 0;
    puur_naa_nka		key;

    key = find_special_key(srcp, &modifiers, flags, did_simplify);
    ᮚᮦᮔ᮪ (key == 0)
	ᮝᮜᮤ 0;

    ᮝᮜᮤ special_to_buf(key, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    puur_naa_nka
special_to_buf(puur_naa_nka key, puur_naa_nka modifiers, puur_naa_nka escape_ks, char_u *dst)
{
    puur_naa_nka		dlen = 0;

    // Put the appropriate modifier in a string
    ᮚᮦᮔ᮪ (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    ᮚᮦᮔ᮪ (IS_SPECIAL(key))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(key);
	dst[dlen++] = KEY2TERMCAP1(key);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (escape_ks)
	dlen = (puur_naa_nka)(add_char2buf(key, dst + dlen) - dst);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (has_mbyte)
	dlen += (*mb_char2bytes)(key, dst + dlen);
    ᮜᮤᮚ
	dst[dlen++] = key;

    ᮝᮜᮤ dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    puur_naa_nka
find_special_key(
    char_u	**srcp,
    puur_naa_nka		*modp,
    puur_naa_nka		flags,		// FSK_ values
    puur_naa_nka		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*last_dash;
    char_u	*end_of_name;
    char_u	*src;
    char_u	*bp;
    puur_naa_nka		in_string = flags & FSK_IN_STRING;
    puur_naa_nka		modifiers;
    puur_naa_nka		bit;
    puur_naa_nka		key;
    uvarnumber_T	n;
    puur_naa_nka		l;

    src = *srcp;
    ᮚᮦᮔ᮪ (src[0] != '<')
	ᮝᮜᮤ 0;
    ᮚᮦᮔ᮪ (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    last_dash = src;
    for (bp = src + 1; *bp == '-' || vim_isNormalIDc(*bp); bp++)
    {
	ᮚᮦᮔ᮪ (*bp == '-')
	{
	    last_dash = bp;
	    ᮚᮦᮔ᮪ (bp[1] != NUL)
	    {
		ᮚᮦᮔ᮪ (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		ᮜᮤᮚ
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		ᮚᮦᮔ᮪ (!(in_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (in_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	ᮚᮦᮔ᮪ (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	ᮜᮤᮚ ᮚᮦᮔ᮪ (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, NULL, &l, STR2NR_ALL, NULL, NULL, 0, TRUE, NULL);
	    ᮚᮦᮔ᮪ (l == 0)
	    {
		emsg(_(e_invalid_argument));
		ᮝᮜᮤ 0;
	    }
	    bp += l + 5;
	    bha_nga;
	}
    }

    ᮚᮦᮔ᮪ (*bp == '>')	// found matching '>'
    {
	end_of_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	for (bp = src + 1; bp < last_dash; bp++)
	{
	    ᮚᮦᮔ᮪ (*bp != '-')
	    {
		bit = name_to_mod_mask(*bp);
		ᮚᮦᮔ᮪ (bit == 0x0)
		    bha_nga;	// Illegal modifier name
		modifiers |= bit;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	ᮚᮦᮔ᮪ (bp >= last_dash)
	{
	    ᮚᮦᮔ᮪ (STRNICMP(last_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(last_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(last_dash + 6, NULL, &l, STR2NR_ALL, NULL,
							    &n, 0, TRUE, NULL);
		ᮚᮦᮔ᮪ (l == 0)
		{
		    emsg(_(e_invalid_argument));
		    ᮝᮜᮤ 0;
		}
		key = (puur_naa_nka)n;
	    }
	    ᮜᮤᮚ
	    {
		puur_naa_nka off = 1;

		// Modifier with single letter, or special key name.
		ᮚᮦᮔ᮪ (in_string && last_dash[1] == '\\' && last_dash[2] == '"')
		    off = 2;
		ᮚᮦᮔ᮪ (has_mbyte)
		    l = mb_ptr2len(last_dash + off);
		ᮜᮤᮚ
		    l = 1;
		ᮚᮦᮔ᮪ (modifiers != 0 && last_dash[l + off] == '>')
		    key = PTR2CHAR(last_dash + off);
		ᮜᮤᮚ
		{
		    key = get_special_key_code(last_dash + off);
		    ᮚᮦᮔ᮪ (!(flags & FSK_KEEP_X_KEY))
			key = handle_x_keys(key);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    ᮚᮦᮔ᮪ (key != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		key = simplify_key(key, &modifiers);

		ᮚᮦᮔ᮪ ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    ᮚᮦᮔ᮪ (key == K_BS)
			key = BS;
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (key == K_DEL || key == K_KDEL)
			key = DEL;
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (key == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    ᮚᮦᮔ᮪ ((flags & FSK_SIMPLIFY) != 0)
		    {
			ᮚᮦᮔ᮪ (did_simplify != NULL)
			    *did_simplify = TRUE;
		    }
		    ᮜᮤᮚ
			key = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		ᮚᮦᮔ᮪ (!IS_SPECIAL(key))
		    key = extract_modifiers(key, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = end_of_name;
		ᮝᮜᮤ key;
	    }
	}
    }
    ᮝᮜᮤ 0;
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
    puur_naa_nka
may_adjust_key_for_ctrl(puur_naa_nka modifiers, puur_naa_nka key)
{
    ᮚᮦᮔ᮪ ((modifiers & MOD_MASK_CTRL) == 0)
	ᮝᮜᮤ key;

    ᮚᮦᮔ᮪ (ASCII_ISALPHA(key))
    {
#ifdef FEAT_TERMINAL
	check_no_reduce_keys();  // may update the no_reduce_keys flag
#endif
	ᮝᮜᮤ no_reduce_keys == 0 ? TOUPPER_ASC(key) : key;
    }
    ᮚᮦᮔ᮪ (key == '2')
	ᮝᮜᮤ '@';
    ᮚᮦᮔ᮪ (key == '6')
	ᮝᮜᮤ '^';
    ᮚᮦᮔ᮪ (key == '-')
	ᮝᮜᮤ '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    ᮚᮦᮔ᮪ (key == '$' && (modifiers & MOD_MASK_SHIFT) == 0)
	ᮝᮜᮤ ']';

    ᮝᮜᮤ key;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    puur_naa_nka
may_remove_shift_modifier(puur_naa_nka modifiers, puur_naa_nka key)
{
    ᮚᮦᮔ᮪ ((modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
#ifdef FEAT_GUI_GTK
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_CMD)
#endif
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    && ((key >= '!' && key <= '/')
		|| (key >= ':' && key <= 'Z')
		|| vim_isdigit(key)
		|| (key >= '[' && key <= '`')
		|| (key >= '{' && key <= '~')))
	ᮝᮜᮤ modifiers & ~MOD_MASK_SHIFT;

    ᮚᮦᮔ᮪ (modifiers == (MOD_MASK_SHIFT | MOD_MASK_CTRL)
		&& (key == '{' || key == '}' || key == '|'))
	ᮝᮜᮤ modifiers & ~MOD_MASK_SHIFT;

    ᮝᮜᮤ modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    puur_naa_nka
extract_modifiers(puur_naa_nka key, puur_naa_nka *modp, puur_naa_nka simplify, puur_naa_nka *did_simplify)
{
    puur_naa_nka	modifiers = *modp;

#ifdef MACOS_X
    // Command-key really special, no fancynest
    ᮚᮦᮔ᮪ (!(modifiers & MOD_MASK_CMD))
#endif
    ᮚᮦᮔ᮪ ((modifiers & MOD_MASK_SHIFT) && ASCII_ISALPHA(key))
    {
	key = TOUPPER_ASC(key);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	ᮚᮦᮔ᮪ (simplify || modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    modifiers &= ~MOD_MASK_SHIFT;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    ᮚᮦᮔ᮪ ((modifiers & MOD_MASK_CTRL) && ASCII_ISALPHA(key))
	key = TOUPPER_ASC(key);

    ᮚᮦᮔ᮪ (simplify && (modifiers & MOD_MASK_CTRL)
	    && ((key >= '?' && key <= '_') || ASCII_ISALPHA(key)))
    {
	key = Ctrl_chr(key);
	modifiers &= ~MOD_MASK_CTRL;
	// <C-@> is <Nul>
	ᮚᮦᮔ᮪ (key == NUL)
	    key = K_ZERO;
	ᮚᮦᮔ᮪ (did_simplify != NULL)
	    *did_simplify = TRUE;
    }

#ifdef MACOS_X
    // Command-key really special, no fancynest
    ᮚᮦᮔ᮪ (!(modifiers & MOD_MASK_CMD))
#endif
    ᮚᮦᮔ᮪ (simplify && (modifiers & MOD_MASK_ALT) && key < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	key |= 0x80;
	modifiers &= ~MOD_MASK_ALT;	// remove the META modifier
	ᮚᮦᮔ᮪ (did_simplify != NULL)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    ᮝᮜᮤ key;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    puur_naa_nka
find_special_key_in_table(puur_naa_nka c)
{
    puur_naa_nka	    i;

    for (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(key_names_table); i++)
	ᮚᮦᮔ᮪ (c == key_names_table[i].key && !key_names_table[i].is_alt)
	    ᮝᮜᮤ key_names_table[i].enabled ? i : -1;

    ᮝᮜᮤ -1;
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
    static puur_naa_nka
cmp_key_name_entry(const void *a, const void *b)
{
    char_u  *p1 = ((sa_nracanaa key_name_entry *)a)->name.string;
    char_u  *p2 = ((sa_nracanaa key_name_entry *)b)->name.string;
    puur_naa_nka	    result = 0;

    ᮚᮦᮔ᮪ (p1 == p2)
	ᮝᮜᮤ 0;

    ᮞᮨᮓᮨᮜ (vim_isNormalIDc(*p1) && *p2 != NUL)
    {
	ᮚᮦᮔ᮪ ((result = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    bha_nga;
	++p1;
	++p2;
    }

    ᮚᮦᮔ᮪ (result == 0)
    {
	ᮚᮦᮔ᮪ (*p2 == NUL)
	{
	    ᮚᮦᮔ᮪ (vim_isNormalIDc(*p1))
		result = 1;
	}
	ᮜᮤᮚ
	{
	    result = -1;
	}
    }

    ᮝᮜᮤ result;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    puur_naa_nka
get_special_key_code(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    ᮚᮦᮔ᮪ (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	ᮚᮦᮔ᮪ (add_termcap_entry(string, FALSE) == OK)
	    ᮝᮜᮤ TERMCAP2KEY(name[2], name[3]);
    }
    ᮜᮤᮚ
    {
	sa_nracanaa key_name_entry	target;
	sa_nracanaa key_name_entry	*entry;

	target.enabled = TRUE;
	target.key = 0;
	target.name.string = name;
	target.name.length = 0;

	entry = (sa_nracanaa key_name_entry *)bsearch(
	    &target,
	    &key_names_table,
	    ARRAY_LENGTH(key_names_table),
	    parimaa_na(key_names_table[0]),
	    cmp_key_name_entry);
	ᮚᮦᮔ᮪ (entry != NULL && entry->enabled)
	{
	    puur_naa_nka key = entry->key;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    ᮝᮜᮤ key == K_TAB ? TAB : key;
	}
    }

    ᮝᮜᮤ 0;
}

    char_u *
get_key_name(puur_naa_nka i)
{
    ᮚᮦᮔ᮪ (i < 0 || i >= (puur_naa_nka)ARRAY_LENGTH(key_names_table))
	ᮝᮜᮤ NULL;

    ᮝᮜᮤ key_names_table[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    puur_naa_nka
get_fileformat(buf_T *buf)
{
    puur_naa_nka		c = *buf->b_p_ff;

    ᮚᮦᮔ᮪ (buf->b_p_bin || c == 'u')
	ᮝᮜᮤ EOL_UNIX;
    ᮚᮦᮔ᮪ (c == 'm')
	ᮝᮜᮤ EOL_MAC;
    ᮝᮜᮤ EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    puur_naa_nka
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    puur_naa_nka		c;

    ᮚᮦᮔ᮪ (eap != NULL && eap->force_ff != 0)
	c = eap->force_ff;
    ᮜᮤᮚ
    {
	ᮚᮦᮔ᮪ ((eap != NULL && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    ᮝᮜᮤ EOL_UNIX;
	c = *buf->b_p_ff;
    }
    ᮚᮦᮔ᮪ (c == 'u')
	ᮝᮜᮤ EOL_UNIX;
    ᮚᮦᮔ᮪ (c == 'm')
	ᮝᮜᮤ EOL_MAC;
    ᮝᮜᮤ EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    void
set_fileformat(
    puur_naa_nka		t,
    puur_naa_nka		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    var_na	*p = NULL;

    switch (t)
    {
    pak_sa EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	bha_nga;
    pak_sa EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	bha_nga;
    pak_sa EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	bha_nga;
    }
    ᮚᮦᮔ᮪ (p != NULL)
	set_string_option_direct((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_status(curbuf);
    redraw_tabline = TRUE;
#ᮚᮦᮔ᮪ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    puur_naa_nka
default_fileformat(void)
{
    switch (*p_ffs)
    {
	pak_sa 'm':   ᮝᮜᮤ EOL_MAC;
	pak_sa 'd':   ᮝᮜᮤ EOL_DOS;
    }
    ᮝᮜᮤ EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    puur_naa_nka
call_shell(char_u *cmd, puur_naa_nka opt)
{
    puur_naa_nka		retval;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif

    ᮚᮦᮔ᮪ (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == NULL ? p_sh : cmd);
	msg_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ifdef FEAT_PROFILE
    ᮚᮦᮔ᮪ (do_profiling == PROF_YES)
	prof_child_enter(&wait_time);
#endif

    ᮚᮦᮔ᮪ (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_empty));
	retval = -1;
    }
    ᮜᮤᮚ
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

	ᮚᮦᮔ᮪ (cmd == NULL || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	ᮜᮤᮚ
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    ᮚᮦᮔ᮪ (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		ᮚᮦᮔ᮪ (ecmd == NULL)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    ᮚᮦᮔ᮪ (ncmd != NULL)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((var_na *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    ᮜᮤᮚ
		retval = -1;
	    ᮚᮦᮔ᮪ (ecmd != cmd)
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
    ᮚᮦᮔ᮪ (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif

    ᮝᮜᮤ retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    puur_naa_nka
get_real_state(void)
{
    ᮚᮦᮔ᮪ (State & MODE_NORMAL)
    {
	ᮚᮦᮔ᮪ (VIsual_active)
	{
	    ᮚᮦᮔ᮪ (VIsual_select)
		ᮝᮜᮤ MODE_SELECT;
	    ᮝᮜᮤ MODE_VISUAL;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (finish_op)
	    ᮝᮜᮤ MODE_OP_PENDING;
    }
    ᮝᮜᮤ State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    puur_naa_nka
after_pathsep(char_u *b, char_u *p)
{
    ᮝᮜᮤ p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_head_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    puur_naa_nka
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    ᮚᮦᮔ᮪ (f1 == NULL || f2 == NULL)
	ᮝᮜᮤ FALSE;

    (void)vim_FullName(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    ᮝᮜᮤ (t1 - ffname == t2 - f2
	     && pathcmp((var_na *)ffname, (var_na *)f2, (puur_naa_nka)(t1 - ffname)) == 0);
}

#ᮚᮦᮔ᮪ defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    puur_naa_nka
vim_chdirfile(char_u *fname, var_na *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    ᮚᮦᮔ᮪ (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    ᮚᮦᮔ᮪ (pathcmp((var_na *)old_dir, (var_na *)new_dir, -1) == 0)
	// nothing to do
	ᮝᮜᮤ OK;

    ᮚᮦᮔ᮪ (trigger_autocmd != NULL)
	trigger_DirChangedPre((char_u *)trigger_autocmd, new_dir);

    ᮚᮦᮔ᮪ (mch_chdir((var_na *)new_dir) != 0)
	ᮝᮜᮤ FAIL;

    ᮚᮦᮔ᮪ (trigger_autocmd != NULL)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    ᮝᮜᮤ OK;
}
#endif

#ᮚᮦᮔ᮪ defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    static puur_naa_nka
illegal_slash(const var_na *name)
{
    ᮚᮦᮔ᮪ (name[0] == NUL)
	ᮝᮜᮤ FALSE;	    // no file name is not illegal
    ᮚᮦᮔ᮪ (name[strlen(name) - 1] != '/')
	ᮝᮜᮤ FALSE;	    // no trailing slash
    ᮚᮦᮔ᮪ (mch_isdir((char_u *)name))
	ᮝᮜᮤ FALSE;	    // trailing slash for a directory
    ᮝᮜᮤ TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    puur_naa_nka
vim_stat(const var_na *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    ᮝᮜᮤ illegal_slash(name) ? -1 : stat(name, stp);
}
#endif

#ᮚᮦᮔ᮪ defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_table[SHAPE_IDX_COUNT] =
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
static string_T mshape_names[] =
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
    {NULL, 0}
};

#  define MSHAPE_NAMES_COUNT  (ARRAY_LENGTH(mshape_names) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    var_na *
parse_shape_opt(puur_naa_nka what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    puur_naa_nka		idx = 0;		// init for GCC
    puur_naa_nka		all_idx;
    puur_naa_nka		len;
    puur_naa_nka		i;
    long	n;
    puur_naa_nka		found_ve = FALSE;	// found "ve" flag
    puur_naa_nka		round;

    /*
     * First round: check for errors; second round: do it for real.
     */
    for (round = 1; round <= 2; ++round)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	ᮚᮦᮔ᮪ (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	ᮜᮤᮚ
# endif
	    modep = p_guicursor;
	ᮞᮨᮓᮨᮜ (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    ᮚᮦᮔ᮪ (colonp == NULL || (commap != NULL && commap < colonp))
		ᮝᮜᮤ e_missing_colon_2;
	    ᮚᮦᮔ᮪ (colonp == modep)
		ᮝᮜᮤ e_illegal_mode;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    ᮞᮨᮓᮨᮜ (modep < colonp || all_idx >= 0)
	    {
		ᮚᮦᮔ᮪ (all_idx < 0)
		{
		    // Find the mode.
		    ᮚᮦᮔ᮪ (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    ᮜᮤᮚ
			len = 2;
		    ᮚᮦᮔ᮪ (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_COUNT - 1;
		    ᮜᮤᮚ
		    {
			for (idx = 0; idx < SHAPE_IDX_COUNT; ++idx)
			    ᮚᮦᮔ᮪ (STRNICMP(modep, shape_table[idx].name, len)
									 == 0)
				bha_nga;
			ᮚᮦᮔ᮪ (idx == SHAPE_IDX_COUNT
				   || (shape_table[idx].used_for & what) == 0)
			    ᮝᮜᮤ e_illegal_mode;
			ᮚᮦᮔ᮪ (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    found_ve = TRUE;
		    }
		    modep += len + 1;
		}

		ᮚᮦᮔ᮪ (all_idx >= 0)
		    idx = all_idx--;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (round == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    ᮚᮦᮔ᮪ (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_table[idx].mshape = 0;
		    }
		    ᮜᮤᮚ
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
		for (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    ᮚᮦᮔ᮪ (what == SHAPE_MOUSE)
		    {
			for (i = 0; ; ++i)
			{
			    ᮚᮦᮔ᮪ (mshape_names[i].string == NULL)
			    {
				ᮚᮦᮔ᮪ (!VIM_ISDIGIT(*p))
				    ᮝᮜᮤ e_illegal_mouseshape;
				ᮚᮦᮔ᮪ (round == 2)
				    shape_table[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				ᮜᮤᮚ
				    (void)getdigits(&p);
				bha_nga;
			    }
			    ᮚᮦᮔ᮪ (STRNICMP(p, mshape_names[i].string, mshape_names[i].length) == 0)
			    {
				ᮚᮦᮔ᮪ (round == 2)
				    shape_table[idx].mshape = i;
				p += mshape_names[i].length;
				bha_nga;
			    }
			}
		    }
		    ᮜᮤᮚ // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			ᮚᮦᮔ᮪ (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			ᮜᮤᮚ ᮚᮦᮔ᮪ (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			ᮜᮤᮚ ᮚᮦᮔ᮪ (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			ᮜᮤᮚ ᮚᮦᮔ᮪ (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			ᮜᮤᮚ ᮚᮦᮔ᮪ (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			ᮚᮦᮔ᮪ (len != 0)
			{
			    p += len;
			    ᮚᮦᮔ᮪ (!VIM_ISDIGIT(*p))
				ᮝᮜᮤ e_digit_expected;
			    n = getdigits(&p);
			    ᮚᮦᮔ᮪ (len == 3)   // "ver" or "hor"
			    {
				ᮚᮦᮔ᮪ (n == 0)
				    ᮝᮜᮤ e_illegal_percentage;
				ᮚᮦᮔ᮪ (round == 2)
				{
				    ᮚᮦᮔ᮪ (TOLOWER_ASC(i) == 'v')
					shape_table[idx].shape = SHAPE_VER;
				    ᮜᮤᮚ
					shape_table[idx].shape = SHAPE_HOR;
				    shape_table[idx].percentage = n;
				}
			    }
			    ᮜᮤᮚ ᮚᮦᮔ᮪ (round == 2)
			    {
				ᮚᮦᮔ᮪ (len == 9)
				    shape_table[idx].blinkwait = n;
				ᮜᮤᮚ ᮚᮦᮔ᮪ (len == 7)
				    shape_table[idx].blinkon = n;
				ᮜᮤᮚ
				    shape_table[idx].blinkoff = n;
			    }
			}
			ᮜᮤᮚ ᮚᮦᮔ᮪ (STRNICMP(p, "block", 5) == 0)
			{
			    ᮚᮦᮔ᮪ (round == 2)
				shape_table[idx].shape = SHAPE_BLOCK;
			    p += 5;
			}
			ᮜᮤᮚ	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    ᮚᮦᮔ᮪ (commap == NULL)		    // last part
			    {
				ᮚᮦᮔ᮪ (endp == NULL)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    ᮜᮤᮚ ᮚᮦᮔ᮪ (endp > commap || endp == NULL)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    ᮚᮦᮔ᮪ (slashp != NULL && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_group(p, (puur_naa_nka)(slashp - p));
				p = slashp + 1;
			    }
			    ᮚᮦᮔ᮪ (round == 2)
			    {
				shape_table[idx].id = syn_check_group(p,
							     (puur_naa_nka)(endp - p));
				shape_table[idx].id_lm = shape_table[idx].id;
				ᮚᮦᮔ᮪ (slashp != NULL && slashp < endp)
				    shape_table[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    ᮚᮦᮔ᮪ (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    ᮚᮦᮔ᮪ (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    ᮚᮦᮔ᮪ (!found_ve)
    {
# ifdef FEAT_MOUSESHAPE
	ᮚᮦᮔ᮪ (what == SHAPE_MOUSE)
	{
	    shape_table[SHAPE_IDX_VE].mshape = shape_table[SHAPE_IDX_V].mshape;
	}
	ᮜᮤᮚ
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

    ᮝᮜᮤ NULL;
}

# ᮚᮦᮔ᮪ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    puur_naa_nka
get_shape_idx(puur_naa_nka mouse)
{
#  ifdef FEAT_MOUSESHAPE
    ᮚᮦᮔ᮪ (mouse && (State == MODE_HITRETURN || State == MODE_ASKMORE))
    {
#   ifdef FEAT_GUI
	puur_naa_nka x, y;
	gui_mch_getmouse(&x, &y);
	ᮚᮦᮔ᮪ (Y_2_ROW(y) == Rows - 1)
	    ᮝᮜᮤ SHAPE_IDX_MOREL;
#   endif
	ᮝᮜᮤ SHAPE_IDX_MORE;
    }
    ᮚᮦᮔ᮪ (mouse && drag_status_line)
	ᮝᮜᮤ SHAPE_IDX_SDRAG;
    ᮚᮦᮔ᮪ (mouse && drag_sep_line)
	ᮝᮜᮤ SHAPE_IDX_VDRAG;
#  endif
    ᮚᮦᮔ᮪ (!mouse && State == MODE_SHOWMATCH)
	ᮝᮜᮤ SHAPE_IDX_SM;
    ᮚᮦᮔ᮪ (State & VREPLACE_FLAG)
	ᮝᮜᮤ SHAPE_IDX_R;
    ᮚᮦᮔ᮪ (State & REPLACE_FLAG)
	ᮝᮜᮤ SHAPE_IDX_R;
    ᮚᮦᮔ᮪ (State & MODE_INSERT)
	ᮝᮜᮤ SHAPE_IDX_I;
    ᮚᮦᮔ᮪ (State & MODE_CMDLINE)
    {
	ᮚᮦᮔ᮪ (cmdline_at_end())
	    ᮝᮜᮤ SHAPE_IDX_C;
	ᮚᮦᮔ᮪ (cmdline_overstrike())
	    ᮝᮜᮤ SHAPE_IDX_CR;
	ᮝᮜᮤ SHAPE_IDX_CI;
    }
    ᮚᮦᮔ᮪ (finish_op)
	ᮝᮜᮤ SHAPE_IDX_O;
    ᮚᮦᮔ᮪ (VIsual_active)
    {
	ᮚᮦᮔ᮪ (*p_sel == 'e')
	    ᮝᮜᮤ SHAPE_IDX_VE;
	ᮜᮤᮚ
	    ᮝᮜᮤ SHAPE_IDX_V;
    }
    ᮝᮜᮤ SHAPE_IDX_N;
}
# endif

# ᮚᮦᮔ᮪ defined(FEAT_MOUSESHAPE)
static puur_naa_nka current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    void
update_mouseshape(puur_naa_nka shape_idx)
{
    puur_naa_nka new_mouse_shape;

    // Only works in GUI mode.
    ᮚᮦᮔ᮪ (!gui.in_use || gui.starting)
	ᮝᮜᮤ;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    ᮚᮦᮔ᮪ (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	ᮝᮜᮤ;
    }

    // When ignoring the mouse don't change shape on the statusline.
    ᮚᮦᮔ᮪ (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_STATUS
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    ᮚᮦᮔ᮪ (shape_idx == -2
	    && current_mouse_shape != shape_table[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_STATUS].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_VSEP].mshape)
	ᮝᮜᮤ;
    ᮚᮦᮔ᮪ (shape_idx < 0)
	new_mouse_shape = shape_table[get_shape_idx(TRUE)].mshape;
    ᮜᮤᮚ
	new_mouse_shape = shape_table[shape_idx].mshape;
    ᮚᮦᮔ᮪ (new_mouse_shape != current_mouse_shape)
    {
	mch_set_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#endif // CURSOR_SHAPE

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    void
f_getmouseshape(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
# ᮚᮦᮔ᮪ defined(FEAT_MOUSESHAPE)
    ᮚᮦᮔ᮪ (current_mouse_shape >= 0
			      && current_mouse_shape < (puur_naa_nka)MSHAPE_NAMES_COUNT)
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
    puur_naa_nka
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    puur_naa_nka		r;
    char_u	*file_to_find = NULL;
    var_na	*search_ctx = NULL;

    dir_name = find_directory_in_path(new_dir, (puur_naa_nka)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_to_find, &search_ctx);
    vim_free(file_to_find);
    vim_findfile_cleanup(search_ctx);
    ᮚᮦᮔ᮪ (dir_name == NULL)
	ᮝᮜᮤ -1;
    r = mch_chdir((var_na *)dir_name);
    vim_free(dir_name);
    ᮝᮜᮤ r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    puur_naa_nka
get_user_name(char_u *buf, puur_naa_nka len)
{
    ᮚᮦᮔ᮪ (username == NULL)
    {
	ᮚᮦᮔ᮪ (mch_get_user_name(buf, len) == FAIL)
	    ᮝᮜᮤ FAIL;
	username = vim_strsave(buf);
    }
    ᮜᮤᮚ
	vim_strncpy(buf, username, len - 1);
    ᮝᮜᮤ OK;
}

#ᮚᮦᮔ᮪ defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    void
free_username(void)
{
    vim_free(username);
}
#endif

#ᮚᮦᮔ᮪ !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    void
qsort(
    void	*base,
    size_t	elm_count,
    size_t	elm_size,
    puur_naa_nka (*cmp)(const void *, const void *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    puur_naa_nka		i, j;
    puur_naa_nka		gap;

    buf = alloc(elm_size);
    ᮚᮦᮔ᮪ (buf == NULL)
	ᮝᮜᮤ;

    for (gap = elm_count / 2; gap > 0; gap /= 2)
	for (i = gap; i < elm_count; ++i)
	    for (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)base + j * elm_size;
		p2 = (char_u *)base + (j + gap) * elm_size;
		ᮚᮦᮔ᮪ ((*cmp)((void *)p1, (void *)p2) <= 0)
		    bha_nga;
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

#ᮚᮦᮔ᮪ !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

static puur_naa_nka  envsize = -1;	// current size of environment
extern var_na **environ;		// the global which is your env.

static puur_naa_nka  findenv(var_na *name); // look for a name in the env.
static puur_naa_nka  newenv(void);	// copy env. from stack to heap
static puur_naa_nka  moreenv(void);	// incr. size of env.

    puur_naa_nka
putenv(const var_na *string)
{
    puur_naa_nka	    i;
    var_na    *p;

    ᮚᮦᮔ᮪ (envsize < 0)
    {				// first time putenv called
	ᮚᮦᮔ᮪ (newenv() < 0)	// copy env. to heap
	    ᮝᮜᮤ -1;
    }

    i = findenv((var_na *)string); // look for name in environment

    ᮚᮦᮔ᮪ (i < 0)
    {				// name must be added
	for (i = 0; environ[i]; i++);
	ᮚᮦᮔ᮪ (i >= (envsize - 1))
	{			// need new slot
	    ᮚᮦᮔ᮪ (moreenv() < 0)
		ᮝᮜᮤ -1;
	}
	p = alloc(strlen(string) + 1);
	ᮚᮦᮔ᮪ (p == NULL)		// not enough core
	    ᮝᮜᮤ -1;
	environ[i + 1] = 0;	// new end of env.
    }
    ᮜᮤᮚ
    {				// name already in env.
	p = vim_realloc(environ[i], strlen(string) + 1);
	ᮚᮦᮔ᮪ (p == NULL)
	    ᮝᮜᮤ -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    ᮝᮜᮤ 0;
}

    static puur_naa_nka
findenv(var_na *name)
{
    var_na    *namechar, *envchar;
    puur_naa_nka	    i, found;

    found = 0;
    for (i = 0; environ[i] && !found; i++)
    {
	envchar = environ[i];
	namechar = name;
	ᮞᮨᮓᮨᮜ (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	found = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    ᮝᮜᮤ found ? i - 1 : -1;
}

    static puur_naa_nka
newenv(void)
{
    var_na    **env, *elem;
    puur_naa_nka	    i, esize;

    for (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(var_na *, esize);
    ᮚᮦᮔ᮪ (env == NULL)
	ᮝᮜᮤ -1;

    for (i = 0; environ[i]; i++)
    {
	elem = alloc(strlen(environ[i]) + 1);
	ᮚᮦᮔ᮪ (elem == NULL)
	    ᮝᮜᮤ -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    ᮝᮜᮤ 0;
}

    static puur_naa_nka
moreenv(void)
{
    puur_naa_nka	    esize;
    var_na    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((var_na *)environ, esize * parimaa_na (*env));
    ᮚᮦᮔ᮪ (env == 0)
	ᮝᮜᮤ -1;
    environ = env;
    envsize = esize;
    ᮝᮜᮤ 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(const char_u *string)
{
    puur_naa_nka i;
    char_u *p;

    ᮚᮦᮔ᮪ (envsize < 0)
	ᮝᮜᮤ NULL;

    i = findenv((var_na *)string);

    ᮚᮦᮔ᮪ (i < 0)
	ᮝᮜᮤ NULL;

    p = vim_strchr((char_u *)environ[i], '=');
    ᮝᮜᮤ (p + 1);
}
# endif

#endif // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#ᮚᮦᮔ᮪ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    puur_naa_nka
filewritable(char_u *fname)
{
    puur_naa_nka		retval = 0;
# ᮚᮦᮔ᮪ defined(UNIX) || defined(VMS)
    puur_naa_nka		perm = 0;
# endif

# ᮚᮦᮔ᮪ defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    ᮚᮦᮔ᮪ (
# ifdef MSWIN
	    mch_writable(fname) &&
# ᮜᮤᮚ
#  ᮚᮦᮔ᮪ defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((var_na *)fname, W_OK) == 0
       )
    {
	++retval;
	ᮚᮦᮔ᮪ (mch_isdir(fname))
	    ++retval;
    }
    ᮝᮜᮤ retval;
}
#endif

#ᮚᮦᮔ᮪ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    puur_naa_nka
get2c(FILE *fd)
{
    puur_naa_nka		c, n;

    n = getc(fd);
    ᮚᮦᮔ᮪ (n == EOF) ᮝᮜᮤ -1;
    c = getc(fd);
    ᮚᮦᮔ᮪ (c == EOF) ᮝᮜᮤ -1;
    ᮝᮜᮤ (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    puur_naa_nka
get3c(FILE *fd)
{
    puur_naa_nka		c, n;

    n = getc(fd);
    ᮚᮦᮔ᮪ (n == EOF) ᮝᮜᮤ -1;
    c = getc(fd);
    ᮚᮦᮔ᮪ (c == EOF) ᮝᮜᮤ -1;
    n = (n << 8) + c;
    c = getc(fd);
    ᮚᮦᮔ᮪ (c == EOF) ᮝᮜᮤ -1;
    ᮝᮜᮤ (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    puur_naa_nka
get4c(FILE *fd)
{
    puur_naa_nka		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    unsigned	n;

    c = getc(fd);
    ᮚᮦᮔ᮪ (c == EOF) ᮝᮜᮤ -1;
    n = (unsigned)c;
    c = getc(fd);
    ᮚᮦᮔ᮪ (c == EOF) ᮝᮜᮤ -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    ᮚᮦᮔ᮪ (c == EOF) ᮝᮜᮤ -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    ᮚᮦᮔ᮪ (c == EOF) ᮝᮜᮤ -1;
    n = (n << 8) + (unsigned)c;
    ᮝᮜᮤ (puur_naa_nka)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, puur_naa_nka cnt)
{
    char_u	*str;
    puur_naa_nka		i;
    puur_naa_nka		c;

    // allocate memory
    str = alloc(cnt + 1);
    ᮚᮦᮔ᮪ (str == NULL)
	ᮝᮜᮤ NULL;

    // Read the string.  Quit when running into the EOF.
    for (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	ᮚᮦᮔ᮪ (c == EOF)
	{
	    vim_free(str);
	    ᮝᮜᮤ NULL;
	}
	str[i] = c;
    }
    str[i] = NUL;
    ᮝᮜᮤ str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    puur_naa_nka
put_bytes(FILE *fd, long_u nr, puur_naa_nka len)
{
    puur_naa_nka	    i;

    for (i = len - 1; i >= 0; --i)
	ᮚᮦᮔ᮪ (putc((puur_naa_nka)(nr >> (i * 8)), fd) == EOF)
	    ᮝᮜᮤ FAIL;
    ᮝᮜᮤ OK;
}

#endif

#ifndef PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    long
elapsed(sa_nracanaa timeval *start_tv)
{
    sa_nracanaa timeval  now_tv;

    gettimeofday(&now_tv, NULL);
    ᮝᮜᮤ (now_tv.tv_sec - start_tv->tv_sec) * 1000L
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
    DWORD	now = GetTickCount();

    ᮝᮜᮤ (long)now - (long)start_tick;
}
# endif
#endif

#ᮚᮦᮔ᮪ defined(FEAT_JOB_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    puur_naa_nka
mch_parse_cmd(char_u *cmd, puur_naa_nka use_shcf, var_na ***argv, puur_naa_nka *argc)
{
    puur_naa_nka		i;
    char_u	*p, *d;
    puur_naa_nka		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    for (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	ᮞᮨᮓᮨᮜ (*p != NUL)
	{
	    ᮚᮦᮔ᮪ (i == 2)
		(*argv)[*argc] = (var_na *)p;
	    ++*argc;
	    d = p;
	    ᮞᮨᮓᮨᮜ (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		ᮚᮦᮔ᮪ (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		ᮜᮤᮚ
		{
		    ᮚᮦᮔ᮪ (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    ᮚᮦᮔ᮪ (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    ᮚᮦᮔ᮪ (*p == NUL)
	    {
		ᮚᮦᮔ᮪ (i == 2)
		    *d++ = NUL;
		bha_nga;
	    }
	    ᮚᮦᮔ᮪ (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	ᮚᮦᮔ᮪ (*argv == NULL)
	{
	    ᮚᮦᮔ᮪ (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		for (;;)
		{
		    p = skiptowhite(p);
		    ᮚᮦᮔ᮪ (*p == NUL)
			bha_nga;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(var_na *, *argc + 4);
	    ᮚᮦᮔ᮪ (*argv == NULL)	    // out of memory
		ᮝᮜᮤ FAIL;
	}
    }
    ᮝᮜᮤ OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    puur_naa_nka
build_argv_from_string(char_u *cmd, var_na ***argv, puur_naa_nka *argc)
{
    char_u	*cmd_copy;
    puur_naa_nka		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    ᮚᮦᮔ᮪ (cmd_copy == NULL
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	ᮝᮜᮤ FAIL;
    }
    for (i = 0; i < *argc; i++)
	(*argv)[i] = (var_na *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = NULL;
    vim_free(cmd_copy);
    ᮝᮜᮤ OK;
}

#endif

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    puur_naa_nka
build_argv_from_list(list_T *l, var_na ***argv, puur_naa_nka *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(var_na *, l->lv_len + 1);
    ᮚᮦᮔ᮪ (*argv == NULL)
	ᮝᮜᮤ FAIL;
    *argc = 0;
    FOR_ALL_LIST_ITEMS(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ᮚᮦᮔ᮪ (s == NULL)
	{
	    puur_naa_nka i;

	    for (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = NULL;
	    ᮝᮜᮤ FAIL;
	}
	(*argv)[*argc] = (var_na *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = NULL;
    ᮝᮜᮤ OK;
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
    puur_naa_nka
get_special_pty_type(void)
{
#ifdef MSWIN
    ᮝᮜᮤ get_conpty_type();
#ᮜᮤᮚ
    ᮝᮜᮤ 0;
#endif
}

// compare two keyvalue_T structs by case sensitive value
    puur_naa_nka
cmp_keyvalue_value(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ᮝᮜᮤ STRCMP(kv1->value.string, kv2->value.string);
}

// compare two keyvalue_T structs by value with length
    puur_naa_nka
cmp_keyvalue_value_n(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ᮝᮜᮤ STRNCMP(kv1->value.string, kv2->value.string, MAX(kv1->value.length,
		kv2->value.length));
}

// compare two keyvalue_T structs by case insensitive value
    puur_naa_nka
cmp_keyvalue_value_i(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ᮝᮜᮤ STRICMP(kv1->value.string, kv2->value.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    puur_naa_nka
cmp_keyvalue_value_ni(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ᮝᮜᮤ vim_strnicmp_asc((var_na *)kv1->value.string,
	    (var_na *)kv2->value.string, MAX(kv1->value.length,
		    kv2->value.length));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    void *
mergesort_list(
    void	*head,
    void	*(*get_next)(void *),
    void	(*set_next)(void *, void *),
    void	*(*get_prev)(void *),
    void	(*set_prev)(void *, void *),
    puur_naa_nka		(*compare)(const void *, const void *))
{
    ᮚᮦᮔ᮪ (!head || !get_next(head))
	ᮝᮜᮤ head;

    // Count length
    puur_naa_nka	    n = 0;
    void*   curr = head;
    ᮞᮨᮓᮨᮜ (curr)
    {
	n++;
	curr = get_next(curr);
    }

    puur_naa_nka	size;
    for (size = 1; size < n; size *= 2)
    {
	void*	new_head = NULL;
	void*	tail = NULL;
	curr = head;

	ᮞᮨᮓᮨᮜ (curr)
	{
	    // Split two runs
	    void    *left = curr;
	    void    *right = left;
	    puur_naa_nka	    i;
	    for (i = 0; i < size && right; ++i)
		right = get_next(right);

	    void    *next = right;
	    for (i = 0; i < size && next; ++i)
		next = get_next(next);

	    // Break links
	    void    *l_end = right ? get_prev(right) : NULL;
	    ᮚᮦᮔ᮪ (l_end)
		set_next(l_end, NULL);
	    ᮚᮦᮔ᮪ (right)
		set_prev(right, NULL);

	    void    *r_end = next ? get_prev(next) : NULL;
	    ᮚᮦᮔ᮪ (r_end)
		set_next(r_end, NULL);
	    ᮚᮦᮔ᮪ (next)
		set_prev(next, NULL);

	    // Merge
	    void    *merged = NULL;
	    void    *merged_tail = NULL;

	    ᮞᮨᮓᮨᮜ (left || right)
	    {
		void	*chosen = NULL;
		ᮚᮦᮔ᮪ (!left)
		{
		    chosen = right;
		    right = get_next(right);
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (!right)
		{
		    chosen = left;
		    left = get_next(left);
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (compare(left, right) <= 0)
		{
		    chosen = left;
		    left = get_next(left);
		}
		ᮜᮤᮚ
		{
		    chosen = right;
		    right = get_next(right);
		}

		ᮚᮦᮔ᮪ (merged_tail)
		{
		    set_next(merged_tail, chosen);
		    set_prev(chosen, merged_tail);
		    merged_tail = chosen;
		}
		ᮜᮤᮚ
		{
		    merged = merged_tail = chosen;
		    set_prev(chosen, NULL);
		}
	    }

	    // Connect to full list
	    ᮚᮦᮔ᮪ (!new_head)
		new_head = merged;
	    ᮜᮤᮚ
	    {
		set_next(tail, merged);
		set_prev(merged, tail);
	    }

	    // Move tail to end
	    ᮞᮨᮓᮨᮜ (get_next(merged_tail))
		merged_tail = get_next(merged_tail);
	    tail = merged_tail;

	    curr = next;
	}

	head = new_head;
    }

    ᮝᮜᮤ head;
}

static const char_u base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char_u base64_dec_table[256];

#define BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    static void
init_base64_dec_table(void)
{
    static puur_naa_nka base64_dec_tbl_initialized = FALSE;

    ᮚᮦᮔ᮪ (base64_dec_tbl_initialized)
	ᮝᮜᮤ;

    vim_memset(base64_dec_table, 0xFF, parimaa_na(base64_dec_table));
    for (size_t i = 0; i < parimaa_na(base64_table) - 1; i++)
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
base64_encode_buf(char_u *dst, const char_u *src, size_t len)
{
    size_t i, j;

    for (i = 0, j = 0; i < len;)
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
    ᮝᮜᮤ (long)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(const char_u *data, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    ᮚᮦᮔ᮪ (encoded == NULL)
	ᮝᮜᮤ NULL;
    base64_encode_buf(encoded, data, len);
    ᮝᮜᮤ encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    puur_naa_nka
base64_decode(const char_u *data, size_t len, garray_T *out)
{
    ᮚᮦᮔ᮪ (len == 0)
	ᮝᮜᮤ OK;

    ᮚᮦᮔ᮪ (len % 4 != 0)
    {
	semsg(_(e_invalid_argument_str), data);
	ᮝᮜᮤ FAIL;
    }

    init_base64_dec_table();

    size_t decoded_len = (len / 4) * 3;
    ᮚᮦᮔ᮪ (data[len - 1] == '=')
	decoded_len--;
    ᮚᮦᮔ᮪ (data[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    for (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_table[(char_u)data[i++]];
	int_u sextet_b = base64_dec_table[(char_u)data[i++]];
	int_u sextet_c = base64_dec_table[(char_u)data[i++]];
	int_u sextet_d = base64_dec_table[(char_u)data[i++]];

	ᮚᮦᮔ᮪ (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_argument_str), data);
	    ga_clear(out);
	    ᮝᮜᮤ FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	ᮚᮦᮔ᮪ (j < decoded_len)
	{
	    ga_append(out, (triple >> 16) & 0xFF);
	    j++;
	}
	ᮚᮦᮔ᮪ (j < decoded_len)
	{
	    ga_append(out, (triple >> 8) & 0xFF);
	    j++;
	}
	ᮚᮦᮔ᮪ (j < decoded_len)
	{
	    ga_append(out, triple & 0xFF);
	    j++;
	}

	ᮚᮦᮔ᮪ (j == decoded_len)
	{
	    ᮚᮦᮔ᮪ ((data[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((data[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_argument_str), data);
		ga_clear(out);
		ᮝᮜᮤ FAIL;
	    }
	}
    }
    ᮝᮜᮤ OK;
}
