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

static ຈຳນວນເຕັມ coladvance2(pos_T *pos, ຈຳນວນເຕັມ addspaces, ຈຳນວນເຕັມ finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    ຈຳນວນເຕັມ
virtual_active(void)
{
    unsigned ຈຳນວນເຕັມ cur_ve_flags = get_ve_flags();

    ຖ້າ (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_INSERT) && (State & MODE_INSERT)))
	ສົ່ງຄືນ TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    ຖ້າ (virtual_op != MAYBE)
	ສົ່ງຄືນ virtual_op;
    ສົ່ງຄືນ (cur_ve_flags & VE_BLOCK) && VIsual_active && VIsual_mode == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    ຈຳນວນເຕັມ
getviscol(void)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, NULL, NULL, GETVCOL_NO_ABOVE);
    ສົ່ງຄືນ (ຈຳນວນເຕັມ)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    ຈຳນວນເຕັມ
coladvance_force(colnr_T wcol)
{
    ຈຳນວນເຕັມ rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    ຖ້າ (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ບໍ່ດັ່ງນັ້ນ
	// Virtcol is valid
	set_valid_virtcol(curwin, wcol);
    ສົ່ງຄືນ rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    ຈຳນວນເຕັມ
getviscol2(colnr_T col, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = col;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, NULL, NULL, GETVCOL_NO_ABOVE);
    ສົ່ງຄືນ (ຈຳນວນເຕັມ)x;
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
    ຈຳນວນເຕັມ
coladvance(colnr_T wantcol)
{
    ຈຳນວນເຕັມ rc = getvpos(&curwin->w_cursor, wantcol);

    ຖ້າ (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	set_valid_virtcol(curwin, wantcol);
    ສົ່ງຄືນ rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    ຈຳນວນເຕັມ
getvpos(pos_T *pos, colnr_T wantcol)
{
    ສົ່ງຄືນ coladvance2(pos, FALSE, virtual_active(), wantcol);
}

    static ຈຳນວນເຕັມ
coladvance2(
    pos_T	*pos,
    ຈຳນວນເຕັມ		addspaces,	// change the text to achieve our goal?
    ຈຳນວນເຕັມ		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    ຈຳນວນເຕັມ		idx;
    char_u	*line;
    ຈຳນວນເຕັມ		linelen;
    colnr_T	col = 0;
    ຈຳນວນເຕັມ		csize = 0;
    ຈຳນວນເຕັມ		one_more;
#ifdef FEAT_LINEBREAK
    ຈຳນວນເຕັມ		head = 0;
#endif

    one_more = (State & MODE_INSERT)
		    || restart_edit != NUL
		    || (VIsual_active && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    line = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    ຖ້າ (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_more;
	    col = wcol;

	    ຖ້າ ((addspaces || finetune) && !VIsual_active)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_more;
		ຖ້າ (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຈຳນວນເຕັມ		width = curwin->w_width - win_col_off(curwin);
	chartabsize_T	cts;

	ຖ້າ (finetune
		&& curwin->w_p_wrap
		&& curwin->w_width != 0
		&& wcol >= (colnr_T)width
		&& width > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    ຖ້າ (csize > 0)
		csize--;

	    ຖ້າ (wcol / width > (colnr_T)csize / width
		    && ((State & MODE_INSERT) == 0 || (ຈຳນວນເຕັມ)wcol > csize + 1))
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
	ໃນຂະນະທີ່ (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ifdef FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &head, NULL);
	    MB_PTR_ADV(cts.cts_ptr);
#ບໍ່ດັ່ງນັ້ນ
	    csize = lbr_chartabsize_adv(&cts);
#endif
	    cts.cts_vcol += csize;
	}
	col = cts.cts_vcol;
	idx = (ຈຳນວນເຕັມ)(cts.cts_ptr - line);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	ຖ້າ (col > wcol || (!virtual_active() && one_more == 0))
	{
	    idx -= 1;
#ifdef FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= head;
#endif
	    col -= csize;
	}

	ຖ້າ (virtual_active()
		&& addspaces
		&& wcol >= 0
		&& ((col != wcol && col != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    ຖ້າ (line[idx] == NUL)
	    {
		// Append spaces
		ຈຳນວນເຕັມ	correct = wcol - col;
		char_u	*newline = alloc(idx + correct + 1);
		ຈຳນວນເຕັມ	t;

		ຖ້າ (newline == NULL)
		    ສົ່ງຄືນ FAIL;

		ສຳລັບ (t = 0; t < idx; ++t)
		    newline[t] = line[t];

		ສຳລັບ (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, (colnr_T)idx);
		idx += correct;
		col = wcol;
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		// Break a tab
		ຈຳນວນເຕັມ	correct = wcol - col - csize + 1; // negative!!
		char_u	*newline;
		ຈຳນວນເຕັມ	t, s = 0;
		ຈຳນວນເຕັມ	v;

		ຖ້າ (-correct > csize)
		    ສົ່ງຄືນ FAIL;

		newline = alloc(linelen + csize);
		ຖ້າ (newline == NULL)
		    ສົ່ງຄືນ FAIL;

		ສຳລັບ (t = 0; t < linelen; t++)
		{
		    ຖ້າ (t != idx)
			newline[s++] = line[t];
		    ບໍ່ດັ່ງນັ້ນ
			ສຳລັບ (v = 0; v < csize; v++)
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

    ຖ້າ (idx < 0)
	pos->col = 0;
    ບໍ່ດັ່ງນັ້ນ
	pos->col = idx;

    pos->coladd = 0;

    ຖ້າ (finetune)
    {
	ຖ້າ (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    ຖ້າ (!one_more)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, NULL, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຈຳນວນເຕັມ b = (ຈຳນວນເຕັມ)wcol - (ຈຳນວນເຕັມ)col;

	    // The difference between wcol and col is used to set coladd.
	    ຖ້າ (b > 0 && b < (MAXCOL - 2 * curwin->w_width))
		pos->coladd = b;

	    col += b;
	}
    }

    // prevent from moving onto a trail byte
    ຖ້າ (has_mbyte)
	mb_adjustpos(curbuf, pos);

    ຖ້າ (wcol < 0 || col < wcol)
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    ຈຳນວນເຕັມ
inc_cursor(void)
{
    ສົ່ງຄືນ inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    ຈຳນວນເຕັມ
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    ຖ້າ (lp->col != MAXCOL)
    {
	p = ml_get_pos(lp);
	ຖ້າ (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    ຖ້າ (has_mbyte)
	    {
		ຈຳນວນເຕັມ l = (*mb_ptr2len)(p);

		lp->col += l;
		ສົ່ງຄືນ ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->col++;
	    lp->coladd = 0;
	    ສົ່ງຄືນ ((p[1] != NUL) ? 0 : 2);
	}
    }
    ຖ້າ (lp->lnum != curbuf->b_ml.ml_line_count)     // there is a next line
    {
	lp->col = 0;
	lp->lnum++;
	lp->coladd = 0;
	ສົ່ງຄືນ 1;
    }
    ສົ່ງຄືນ -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    ຈຳນວນເຕັມ
incl(pos_T *lp)
{
    ຈຳນວນເຕັມ	    r;

    ຖ້າ ((r = inc(lp)) >= 1 && lp->col)
	r = inc(lp);
    ສົ່ງຄືນ r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    ຈຳນວນເຕັມ
dec_cursor(void)
{
    ສົ່ງຄືນ dec(&curwin->w_cursor);
}

    ຈຳນວນເຕັມ
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    ຖ້າ (lp->col == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	ຖ້າ (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	ສົ່ງຄືນ 0;
    }

    ຖ້າ (lp->col > 0)
    {
	// still within line
	lp->col--;
	ຖ້າ (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	}
	ສົ່ງຄືນ 0;
    }

    ຖ້າ (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	ຖ້າ (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	ສົ່ງຄືນ 1;
    }

    // at start of file
    ສົ່ງຄືນ -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    ຈຳນວນເຕັມ
decl(pos_T *lp)
{
    ຈຳນວນເຕັມ	    r;

    ຖ້າ ((r = dec(lp)) == 1 && lp->col)
	r = dec(lp);
    ສົ່ງຄືນ r;
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
    ຖ້າ (hasAnyFolding(wp))
    {
	ຖ້າ (lnum > cursor)
	{
	    ໃນຂະນະທີ່ (lnum > cursor)
	    {
		(void)hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		ຖ້າ (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (lnum < cursor)
	{
	    ໃນຂະນະທີ່ (lnum < cursor)
	    {
		(void)hasFoldingWin(wp, lnum, NULL, &lnum, TRUE, NULL);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		ຖ້າ (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    ບໍ່ດັ່ງນັ້ນ
#endif
	retval = lnum - cursor;

    ສົ່ງຄືນ retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    void
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    ຖ້າ (pos->lnum > buf->b_ml.ml_line_count)
	pos->lnum = buf->b_ml.ml_line_count;

    ຖ້າ (pos->col > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	ຖ້າ (pos->col > len)
	    pos->col = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    void
check_cursor_lnum(void)
{
    ຖ້າ (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
#ifdef FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	ຖ້າ (!hasFolding(curbuf->b_ml.ml_line_count,
						&curwin->w_cursor.lnum, NULL))
#endif
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    }
    ຖ້າ (curwin->w_cursor.lnum <= 0)
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
    unsigned ຈຳນວນເຕັມ cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_buffer, win->w_cursor.lnum);
    ຖ້າ (len == 0)
	win->w_cursor.col = 0;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (win->w_cursor.col >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	ຖ້າ ((State & MODE_INSERT) || restart_edit
		|| (VIsual_active && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_active())
	    win->w_cursor.col = len;
	ບໍ່ດັ່ງນັ້ນ
	{
	    win->w_cursor.col = len - 1;
	    // Move the cursor to the head byte.
	    ຖ້າ (has_mbyte)
		mb_adjustpos(win->w_buffer, &win->w_cursor);
	}
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (win->w_cursor.col < 0)
	win->w_cursor.col = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    ຖ້າ (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (cur_ve_flags == VE_ALL)
    {
	ຖ້າ (oldcoladd > win->w_cursor.col)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.col;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    ຖ້າ (win->w_cursor.col + 1 < len)
	    {
		ຈຳນວນເຕັມ cs, ce;

		getvcol(win, &win->w_cursor, &cs, NULL, &ce, 0);
		ຖ້າ (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
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
    ຖ້າ (VIsual.lnum > curbuf->b_ml.ml_line_count)
    {
	VIsual.lnum = curbuf->b_ml.ml_line_count;
	VIsual.col = 0;
	VIsual.coladd = 0;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຈຳນວນເຕັມ len = ml_get_len(VIsual.lnum);

	ຖ້າ (VIsual.col > len)
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
    ຖ້າ (curwin->w_cursor.col > 0
	    && (!VIsual_active || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    ຈຳນວນເຕັມ
set_leftcol(colnr_T leftcol)
{
    ຈຳນວນເຕັມ		retval = FALSE;

    // Return quickly when there is no change.
    ຖ້າ (curwin->w_leftcol == leftcol)
	ສົ່ງຄືນ FALSE;
    curwin->w_leftcol = leftcol;

    changed_cline_bef_curs();
    long lastcol = curwin->w_leftcol + curwin->w_width - curwin_col_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    long siso = get_sidescrolloff_value();
    ຖ້າ (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(void)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, NULL, &e, 0);
    ຖ້າ (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (s < curwin->w_leftcol)
    {
	retval = TRUE;
	ຖ້າ (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    changed_cline_bef_curs();
	}
    }

    ຖ້າ (retval)
	curwin->w_set_curswant = true;
    redraw_later(UPD_NOT_VALID);
    ສົ່ງຄືນ retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    ຈຳນວນເຕັມ
copy_option_part(
    char_u	**option,
    char_u	*buf,
    ຈຳນວນເຕັມ		maxlen,
    var_na	*sep_chars)
{
    ຈຳນວນເຕັມ	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    ຖ້າ (*p == '.')
	buf[len++] = *p++;
    ໃນຂະນະທີ່ (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == NULL)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	ຖ້າ (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != NULL)
	    ++p;
	ຖ້າ (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    ຖ້າ (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_to_option_part(p);	// p points to next file name

    *option = p;
    ສົ່ງຄືນ len;
}

#ຖ້າ !defined(HAVE_MEMSET) && !defined(PROTO)
    void *
vim_memset(void *ptr, ຈຳນວນເຕັມ c, size_t size)
{
    var_na *p = ptr;

    ໃນຂະນະທີ່ (size-- > 0)
	*p++ = c;
    ສົ່ງຄືນ ptr;
}
#endif

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    ຈຳນວນເຕັມ
vim_isspace(ຈຳນວນເຕັມ x)
{
    ສົ່ງຄືນ ((x >= 9 && x <= 13) || x == ' ');
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
#ຖ້າ defined(MACOS_X) || defined(FEAT_GUI_GTK)
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
    MOD_MASK_CTRL,  KS_EXTRA, (ຈຳນວນເຕັມ)KE_C_END,	'@', '7',	// end
    MOD_MASK_SHIFT, '*', '9',			'@', '9',	// exit
    MOD_MASK_SHIFT, '*', '0',			'@', '0',	// find
    MOD_MASK_SHIFT, '#', '1',			'%', '1',	// help
    MOD_MASK_SHIFT, '#', '2',			'k', 'h',	// home
    MOD_MASK_CTRL,  KS_EXTRA, (ຈຳນວນເຕັມ)KE_C_HOME,	'k', 'h',	// home
    MOD_MASK_SHIFT, '#', '3',			'k', 'I',	// insert
    MOD_MASK_SHIFT, '#', '4',			'k', 'l',	// left arrow
    MOD_MASK_CTRL,  KS_EXTRA, (ຈຳນວນເຕັມ)KE_C_LEFT,	'k', 'l',	// left arrow
    MOD_MASK_SHIFT, '%', 'a',			'%', '3',	// message
    MOD_MASK_SHIFT, '%', 'b',			'%', '4',	// move
    MOD_MASK_SHIFT, '%', 'c',			'%', '5',	// next
    MOD_MASK_SHIFT, '%', 'd',			'%', '7',	// options
    MOD_MASK_SHIFT, '%', 'e',			'%', '8',	// previous
    MOD_MASK_SHIFT, '%', 'f',			'%', '9',	// print
    MOD_MASK_SHIFT, '%', 'g',			'%', '0',	// redo
    MOD_MASK_SHIFT, '%', 'h',			'&', '3',	// replace
    MOD_MASK_SHIFT, '%', 'i',			'k', 'r',	// right arr.
    MOD_MASK_CTRL,  KS_EXTRA, (ຈຳນວນເຕັມ)KE_C_RIGHT,	'k', 'r',	// right arr.
    MOD_MASK_SHIFT, '%', 'j',			'&', '5',	// resume
    MOD_MASK_SHIFT, '!', '1',			'&', '6',	// save
    MOD_MASK_SHIFT, '!', '2',			'&', '7',	// suspend
    MOD_MASK_SHIFT, '!', '3',			'&', '8',	// undo
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_UP,	'k', 'u',	// up arrow
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_DOWN,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_XF1,	KS_EXTRA, (ຈຳນວນເຕັມ)KE_XF1,
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_XF2,	KS_EXTRA, (ຈຳນວນເຕັມ)KE_XF2,
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_XF3,	KS_EXTRA, (ຈຳນວນເຕັມ)KE_XF3,
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_XF4,	KS_EXTRA, (ຈຳນວນເຕັມ)KE_XF4,

    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F1,	'k', '1',	// F1
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F2,	'k', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F3,	'k', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F4,	'k', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F5,	'k', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F6,	'k', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F7,	'k', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F8,	'k', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F9,	'k', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F10,	'k', ';',	// F10

    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F11,	'F', '1',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F12,	'F', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F13,	'F', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F14,	'F', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F15,	'F', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F16,	'F', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F17,	'F', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F18,	'F', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F19,	'F', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F20,	'F', 'A',

    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F21,	'F', 'B',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F22,	'F', 'C',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F23,	'F', 'D',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F24,	'F', 'E',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F25,	'F', 'F',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F26,	'F', 'G',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F27,	'F', 'H',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F28,	'F', 'I',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F29,	'F', 'J',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F30,	'F', 'K',

    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F31,	'F', 'L',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F32,	'F', 'M',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F33,	'F', 'N',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F34,	'F', 'O',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F35,	'F', 'P',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F36,	'F', 'Q',
    MOD_MASK_SHIFT, KS_EXTRA, (ຈຳນວນເຕັມ)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_MASK_SHIFT, 'k', 'B',			KS_EXTRA, (ຈຳນວນເຕັມ)KE_TAB,

    NUL
};

static sa_nracanaa key_name_entry
{
    ຈຳນວນເຕັມ		enabled;	    // is this entry available (TRUE/FALSE)?
    ຈຳນວນເຕັມ		key;		    // special key code or ascii value
    string_T	name;		    // name of key
    ຈຳນວນເຕັມ		is_alt;		    // is an alternative name
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
#ບໍ່ດັ່ງນັ້ນ
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
#ບໍ່ດັ່ງນັ້ນ
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
#ບໍ່ດັ່ງນັ້ນ
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
#ບໍ່ດັ່ງນັ້ນ
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
#ບໍ່ດັ່ງນັ້ນ
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
#ບໍ່ດັ່ງນັ້ນ
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
    static ຈຳນວນເຕັມ
name_to_mod_mask(ຈຳນວນເຕັມ c)
{
    ຈຳນວນເຕັມ	    i;

    c = TOUPPER_ASC(c);
    ສຳລັບ (i = 0; mod_mask_table[i].mod_mask != 0; i++)
	ຖ້າ (c == mod_mask_table[i].name)
	    ສົ່ງຄືນ mod_mask_table[i].mod_flag;
    ສົ່ງຄືນ 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    ຈຳນວນເຕັມ
simplify_key(ຈຳນວນເຕັມ key, ຈຳນວນເຕັມ *modifiers)
{
    ຈຳນວນເຕັມ	    i;
    ຈຳນວນເຕັມ	    key0;
    ຈຳນວນເຕັມ	    key1;

    ຖ້າ (!(*modifiers & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	ສົ່ງຄືນ key;

    // TAB is a special case
    ຖ້າ (key == TAB && (*modifiers & MOD_MASK_SHIFT))
    {
	*modifiers &= ~MOD_MASK_SHIFT;
	ສົ່ງຄືນ K_S_TAB;
    }
    key0 = KEY2TERMCAP0(key);
    key1 = KEY2TERMCAP1(key);
    ສຳລັບ (i = 0; modifier_keys_table[i] != NUL; i += MOD_KEYS_ENTRY_SIZE)
    {
	ຖ້າ (key0 == modifier_keys_table[i + 3]
		&& key1 == modifier_keys_table[i + 4]
		&& (*modifiers & modifier_keys_table[i]))
	{
	    *modifiers &= ~modifier_keys_table[i];
	    ສົ່ງຄືນ TERMCAP2KEY(modifier_keys_table[i + 1],
		    modifier_keys_table[i + 2]);
	}
    }
    ສົ່ງຄືນ key;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    ຈຳນວນເຕັມ
handle_x_keys(ຈຳນວນເຕັມ key)
{
    switch (key)
    {
	pak_sa K_XUP:	ສົ່ງຄືນ K_UP;
	pak_sa K_XDOWN:	ສົ່ງຄືນ K_DOWN;
	pak_sa K_XLEFT:	ສົ່ງຄືນ K_LEFT;
	pak_sa K_XRIGHT:	ສົ່ງຄືນ K_RIGHT;
	pak_sa K_XHOME:	ສົ່ງຄືນ K_HOME;
	pak_sa K_ZHOME:	ສົ່ງຄືນ K_HOME;
	pak_sa K_XEND:	ສົ່ງຄືນ K_END;
	pak_sa K_ZEND:	ສົ່ງຄືນ K_END;
	pak_sa K_XF1:	ສົ່ງຄືນ K_F1;
	pak_sa K_XF2:	ສົ່ງຄືນ K_F2;
	pak_sa K_XF3:	ສົ່ງຄືນ K_F3;
	pak_sa K_XF4:	ສົ່ງຄືນ K_F4;
	pak_sa K_S_XF1:	ສົ່ງຄືນ K_S_F1;
	pak_sa K_S_XF2:	ສົ່ງຄືນ K_S_F2;
	pak_sa K_S_XF3:	ສົ່ງຄືນ K_S_F3;
	pak_sa K_S_XF4:	ສົ່ງຄືນ K_S_F4;
    }
    ສົ່ງຄືນ key;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_key_name(ຈຳນວນເຕັມ c, ຈຳນວນເຕັມ modifiers)
{
    static char_u string[MAX_KEY_NAME_LEN + 1];
    ຈຳນວນເຕັມ	    i, idx, len;
    ຈຳນວນເຕັມ	    table_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    ຖ້າ (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_KEY)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    ຖ້າ (IS_SPECIAL(c))
    {
	ສຳລັບ (i = 0; modifier_keys_table[i] != 0; i += MOD_KEYS_ENTRY_SIZE)
	    ຖ້າ (       KEY2TERMCAP0(c) == (ຈຳນວນເຕັມ)modifier_keys_table[i + 1]
		    && (ຈຳນວນເຕັມ)KEY2TERMCAP1(c) == (ຈຳນວນເຕັມ)modifier_keys_table[i + 2])
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
    ຖ້າ (c > 0 && (*mb_char2len)(c) == 1)
    {
	ຖ້າ (table_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_MASK_ALT;
	    // try again, to find the un-alted key in the special key table
	    table_idx = find_special_key_in_table(c);
	}
	ຖ້າ (table_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_MASK_CTRL;
	}
    }

    // translate the modifier into a string
    ສຳລັບ (i = 0; mod_mask_table[i].name != 'A'; i++)
	ຖ້າ ((modifiers & mod_mask_table[i].mod_mask)
						== mod_mask_table[i].mod_flag)
	{
	    string[idx++] = mod_mask_table[i].name;
	    string[idx++] = (char_u)'-';
	}

    ຖ້າ (table_idx < 0)		// unknown special key, may output t_xx
    {
	ຖ້າ (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	ບໍ່ດັ່ງນັ້ນ
	{
	    len = (*mb_char2len)(c);
	    ຖ້າ (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    ບໍ່ດັ່ງນັ້ນ
	    {
		char_u	*s = transchar(c);
		ໃນຂະນະທີ່ (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    ບໍ່ດັ່ງນັ້ນ		// use name of special key
    {
	string_T    *s;

	s = &key_names_table[table_idx].name;

	ຖ້າ (s->length + idx + 2 <= MAX_KEY_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (ຈຳນວນເຕັມ)s->length;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    ສົ່ງຄືນ string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    ຈຳນວນເຕັມ
trans_special(
    char_u	**srcp,
    char_u	*dst,
    ຈຳນວນເຕັມ		flags,		// FSK_ values
    ຈຳນວນເຕັມ		escape_ks,	// escape K_SPECIAL bytes in the character
    ຈຳນວນເຕັມ		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    ຈຳນວນເຕັມ		modifiers = 0;
    ຈຳນວນເຕັມ		key;

    key = find_special_key(srcp, &modifiers, flags, did_simplify);
    ຖ້າ (key == 0)
	ສົ່ງຄືນ 0;

    ສົ່ງຄືນ special_to_buf(key, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    ຈຳນວນເຕັມ
special_to_buf(ຈຳນວນເຕັມ key, ຈຳນວນເຕັມ modifiers, ຈຳນວນເຕັມ escape_ks, char_u *dst)
{
    ຈຳນວນເຕັມ		dlen = 0;

    // Put the appropriate modifier in a string
    ຖ້າ (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    ຖ້າ (IS_SPECIAL(key))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(key);
	dst[dlen++] = KEY2TERMCAP1(key);
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (escape_ks)
	dlen = (ຈຳນວນເຕັມ)(add_char2buf(key, dst + dlen) - dst);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (has_mbyte)
	dlen += (*mb_char2bytes)(key, dst + dlen);
    ບໍ່ດັ່ງນັ້ນ
	dst[dlen++] = key;

    ສົ່ງຄືນ dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    ຈຳນວນເຕັມ
find_special_key(
    char_u	**srcp,
    ຈຳນວນເຕັມ		*modp,
    ຈຳນວນເຕັມ		flags,		// FSK_ values
    ຈຳນວນເຕັມ		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*last_dash;
    char_u	*end_of_name;
    char_u	*src;
    char_u	*bp;
    ຈຳນວນເຕັມ		in_string = flags & FSK_IN_STRING;
    ຈຳນວນເຕັມ		modifiers;
    ຈຳນວນເຕັມ		bit;
    ຈຳນວນເຕັມ		key;
    uvarnumber_T	n;
    ຈຳນວນເຕັມ		l;

    src = *srcp;
    ຖ້າ (src[0] != '<')
	ສົ່ງຄືນ 0;
    ຖ້າ (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    last_dash = src;
    ສຳລັບ (bp = src + 1; *bp == '-' || vim_isNormalIDc(*bp); bp++)
    {
	ຖ້າ (*bp == '-')
	{
	    last_dash = bp;
	    ຖ້າ (bp[1] != NUL)
	    {
		ຖ້າ (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		ບໍ່ດັ່ງນັ້ນ
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		ຖ້າ (!(in_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	ຖ້າ (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, NULL, &l, STR2NR_ALL, NULL, NULL, 0, TRUE, NULL);
	    ຖ້າ (l == 0)
	    {
		emsg(_(e_invalid_argument));
		ສົ່ງຄືນ 0;
	    }
	    bp += l + 5;
	    bha_nga;
	}
    }

    ຖ້າ (*bp == '>')	// found matching '>'
    {
	end_of_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	ສຳລັບ (bp = src + 1; bp < last_dash; bp++)
	{
	    ຖ້າ (*bp != '-')
	    {
		bit = name_to_mod_mask(*bp);
		ຖ້າ (bit == 0x0)
		    bha_nga;	// Illegal modifier name
		modifiers |= bit;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	ຖ້າ (bp >= last_dash)
	{
	    ຖ້າ (STRNICMP(last_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(last_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(last_dash + 6, NULL, &l, STR2NR_ALL, NULL,
							    &n, 0, TRUE, NULL);
		ຖ້າ (l == 0)
		{
		    emsg(_(e_invalid_argument));
		    ສົ່ງຄືນ 0;
		}
		key = (ຈຳນວນເຕັມ)n;
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		ຈຳນວນເຕັມ off = 1;

		// Modifier with single letter, or special key name.
		ຖ້າ (in_string && last_dash[1] == '\\' && last_dash[2] == '"')
		    off = 2;
		ຖ້າ (has_mbyte)
		    l = mb_ptr2len(last_dash + off);
		ບໍ່ດັ່ງນັ້ນ
		    l = 1;
		ຖ້າ (modifiers != 0 && last_dash[l + off] == '>')
		    key = PTR2CHAR(last_dash + off);
		ບໍ່ດັ່ງນັ້ນ
		{
		    key = get_special_key_code(last_dash + off);
		    ຖ້າ (!(flags & FSK_KEEP_X_KEY))
			key = handle_x_keys(key);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    ຖ້າ (key != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		key = simplify_key(key, &modifiers);

		ຖ້າ ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    ຖ້າ (key == K_BS)
			key = BS;
		    ບໍ່ດັ່ງນັ້ນ ຖ້າ (key == K_DEL || key == K_KDEL)
			key = DEL;
		}
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (key == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    ຖ້າ ((flags & FSK_SIMPLIFY) != 0)
		    {
			ຖ້າ (did_simplify != NULL)
			    *did_simplify = TRUE;
		    }
		    ບໍ່ດັ່ງນັ້ນ
			key = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		ຖ້າ (!IS_SPECIAL(key))
		    key = extract_modifiers(key, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = end_of_name;
		ສົ່ງຄືນ key;
	    }
	}
    }
    ສົ່ງຄືນ 0;
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
    ຈຳນວນເຕັມ
may_adjust_key_for_ctrl(ຈຳນວນເຕັມ modifiers, ຈຳນວນເຕັມ key)
{
    ຖ້າ ((modifiers & MOD_MASK_CTRL) == 0)
	ສົ່ງຄືນ key;

    ຖ້າ (ASCII_ISALPHA(key))
    {
#ifdef FEAT_TERMINAL
	check_no_reduce_keys();  // may update the no_reduce_keys flag
#endif
	ສົ່ງຄືນ no_reduce_keys == 0 ? TOUPPER_ASC(key) : key;
    }
    ຖ້າ (key == '2')
	ສົ່ງຄືນ '@';
    ຖ້າ (key == '6')
	ສົ່ງຄືນ '^';
    ຖ້າ (key == '-')
	ສົ່ງຄືນ '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    ຖ້າ (key == '$' && (modifiers & MOD_MASK_SHIFT) == 0)
	ສົ່ງຄືນ ']';

    ສົ່ງຄືນ key;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    ຈຳນວນເຕັມ
may_remove_shift_modifier(ຈຳນວນເຕັມ modifiers, ຈຳນວນເຕັມ key)
{
    ຖ້າ ((modifiers == MOD_MASK_SHIFT
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
	ສົ່ງຄືນ modifiers & ~MOD_MASK_SHIFT;

    ຖ້າ (modifiers == (MOD_MASK_SHIFT | MOD_MASK_CTRL)
		&& (key == '{' || key == '}' || key == '|'))
	ສົ່ງຄືນ modifiers & ~MOD_MASK_SHIFT;

    ສົ່ງຄືນ modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    ຈຳນວນເຕັມ
extract_modifiers(ຈຳນວນເຕັມ key, ຈຳນວນເຕັມ *modp, ຈຳນວນເຕັມ simplify, ຈຳນວນເຕັມ *did_simplify)
{
    ຈຳນວນເຕັມ	modifiers = *modp;

#ifdef MACOS_X
    // Command-key really special, no fancynest
    ຖ້າ (!(modifiers & MOD_MASK_CMD))
#endif
    ຖ້າ ((modifiers & MOD_MASK_SHIFT) && ASCII_ISALPHA(key))
    {
	key = TOUPPER_ASC(key);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	ຖ້າ (simplify || modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    modifiers &= ~MOD_MASK_SHIFT;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    ຖ້າ ((modifiers & MOD_MASK_CTRL) && ASCII_ISALPHA(key))
	key = TOUPPER_ASC(key);

    ຖ້າ (simplify && (modifiers & MOD_MASK_CTRL)
	    && ((key >= '?' && key <= '_') || ASCII_ISALPHA(key)))
    {
	key = Ctrl_chr(key);
	modifiers &= ~MOD_MASK_CTRL;
	// <C-@> is <Nul>
	ຖ້າ (key == NUL)
	    key = K_ZERO;
	ຖ້າ (did_simplify != NULL)
	    *did_simplify = TRUE;
    }

#ifdef MACOS_X
    // Command-key really special, no fancynest
    ຖ້າ (!(modifiers & MOD_MASK_CMD))
#endif
    ຖ້າ (simplify && (modifiers & MOD_MASK_ALT) && key < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	key |= 0x80;
	modifiers &= ~MOD_MASK_ALT;	// remove the META modifier
	ຖ້າ (did_simplify != NULL)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    ສົ່ງຄືນ key;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    ຈຳນວນເຕັມ
find_special_key_in_table(ຈຳນວນເຕັມ c)
{
    ຈຳນວນເຕັມ	    i;

    ສຳລັບ (i = 0; i < (ຈຳນວນເຕັມ)ARRAY_LENGTH(key_names_table); i++)
	ຖ້າ (c == key_names_table[i].key && !key_names_table[i].is_alt)
	    ສົ່ງຄືນ key_names_table[i].enabled ? i : -1;

    ສົ່ງຄືນ -1;
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
    static ຈຳນວນເຕັມ
cmp_key_name_entry(const void *a, const void *b)
{
    char_u  *p1 = ((sa_nracanaa key_name_entry *)a)->name.string;
    char_u  *p2 = ((sa_nracanaa key_name_entry *)b)->name.string;
    ຈຳນວນເຕັມ	    result = 0;

    ຖ້າ (p1 == p2)
	ສົ່ງຄືນ 0;

    ໃນຂະນະທີ່ (vim_isNormalIDc(*p1) && *p2 != NUL)
    {
	ຖ້າ ((result = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    bha_nga;
	++p1;
	++p2;
    }

    ຖ້າ (result == 0)
    {
	ຖ້າ (*p2 == NUL)
	{
	    ຖ້າ (vim_isNormalIDc(*p1))
		result = 1;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    result = -1;
	}
    }

    ສົ່ງຄືນ result;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    ຈຳນວນເຕັມ
get_special_key_code(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    ຖ້າ (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	ຖ້າ (add_termcap_entry(string, FALSE) == OK)
	    ສົ່ງຄືນ TERMCAP2KEY(name[2], name[3]);
    }
    ບໍ່ດັ່ງນັ້ນ
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
	ຖ້າ (entry != NULL && entry->enabled)
	{
	    ຈຳນວນເຕັມ key = entry->key;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    ສົ່ງຄືນ key == K_TAB ? TAB : key;
	}
    }

    ສົ່ງຄືນ 0;
}

    char_u *
get_key_name(ຈຳນວນເຕັມ i)
{
    ຖ້າ (i < 0 || i >= (ຈຳນວນເຕັມ)ARRAY_LENGTH(key_names_table))
	ສົ່ງຄືນ NULL;

    ສົ່ງຄືນ key_names_table[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    ຈຳນວນເຕັມ
get_fileformat(buf_T *buf)
{
    ຈຳນວນເຕັມ		c = *buf->b_p_ff;

    ຖ້າ (buf->b_p_bin || c == 'u')
	ສົ່ງຄືນ EOL_UNIX;
    ຖ້າ (c == 'm')
	ສົ່ງຄືນ EOL_MAC;
    ສົ່ງຄືນ EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    ຈຳນວນເຕັມ
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    ຈຳນວນເຕັມ		c;

    ຖ້າ (eap != NULL && eap->force_ff != 0)
	c = eap->force_ff;
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ ((eap != NULL && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    ສົ່ງຄືນ EOL_UNIX;
	c = *buf->b_p_ff;
    }
    ຖ້າ (c == 'u')
	ສົ່ງຄືນ EOL_UNIX;
    ຖ້າ (c == 'm')
	ສົ່ງຄືນ EOL_MAC;
    ສົ່ງຄືນ EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    void
set_fileformat(
    ຈຳນວນເຕັມ		t,
    ຈຳນວນເຕັມ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
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
    ຖ້າ (p != NULL)
	set_string_option_direct((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_status(curbuf);
    redraw_tabline = TRUE;
#ຖ້າ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    ຈຳນວນເຕັມ
default_fileformat(void)
{
    switch (*p_ffs)
    {
	pak_sa 'm':   ສົ່ງຄືນ EOL_MAC;
	pak_sa 'd':   ສົ່ງຄືນ EOL_DOS;
    }
    ສົ່ງຄືນ EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    ຈຳນວນເຕັມ
call_shell(char_u *cmd, ຈຳນວນເຕັມ opt)
{
    ຈຳນວນເຕັມ		retval;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif

    ຖ້າ (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == NULL ? p_sh : cmd);
	msg_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ifdef FEAT_PROFILE
    ຖ້າ (do_profiling == PROF_YES)
	prof_child_enter(&wait_time);
#endif

    ຖ້າ (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_empty));
	retval = -1;
    }
    ບໍ່ດັ່ງນັ້ນ
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

	ຖ້າ (cmd == NULL || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	ບໍ່ດັ່ງນັ້ນ
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    ຖ້າ (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		ຖ້າ (ecmd == NULL)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    ຖ້າ (ncmd != NULL)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((var_na *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    ບໍ່ດັ່ງນັ້ນ
		retval = -1;
	    ຖ້າ (ecmd != cmd)
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
    ຖ້າ (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif

    ສົ່ງຄືນ retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    ຈຳນວນເຕັມ
get_real_state(void)
{
    ຖ້າ (State & MODE_NORMAL)
    {
	ຖ້າ (VIsual_active)
	{
	    ຖ້າ (VIsual_select)
		ສົ່ງຄືນ MODE_SELECT;
	    ສົ່ງຄືນ MODE_VISUAL;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (finish_op)
	    ສົ່ງຄືນ MODE_OP_PENDING;
    }
    ສົ່ງຄືນ State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    ຈຳນວນເຕັມ
after_pathsep(char_u *b, char_u *p)
{
    ສົ່ງຄືນ p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_head_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    ຈຳນວນເຕັມ
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    ຖ້າ (f1 == NULL || f2 == NULL)
	ສົ່ງຄືນ FALSE;

    (void)vim_FullName(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    ສົ່ງຄືນ (t1 - ffname == t2 - f2
	     && pathcmp((var_na *)ffname, (var_na *)f2, (ຈຳນວນເຕັມ)(t1 - ffname)) == 0);
}

#ຖ້າ defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    ຈຳນວນເຕັມ
vim_chdirfile(char_u *fname, var_na *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    ຖ້າ (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    ຖ້າ (pathcmp((var_na *)old_dir, (var_na *)new_dir, -1) == 0)
	// nothing to do
	ສົ່ງຄືນ OK;

    ຖ້າ (trigger_autocmd != NULL)
	trigger_DirChangedPre((char_u *)trigger_autocmd, new_dir);

    ຖ້າ (mch_chdir((var_na *)new_dir) != 0)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (trigger_autocmd != NULL)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    ສົ່ງຄືນ OK;
}
#endif

#ຖ້າ defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    static ຈຳນວນເຕັມ
illegal_slash(const var_na *name)
{
    ຖ້າ (name[0] == NUL)
	ສົ່ງຄືນ FALSE;	    // no file name is not illegal
    ຖ້າ (name[strlen(name) - 1] != '/')
	ສົ່ງຄືນ FALSE;	    // no trailing slash
    ຖ້າ (mch_isdir((char_u *)name))
	ສົ່ງຄືນ FALSE;	    // trailing slash for a directory
    ສົ່ງຄືນ TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    ຈຳນວນເຕັມ
vim_stat(const var_na *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    ສົ່ງຄືນ illegal_slash(name) ? -1 : stat(name, stp);
}
#endif

#ຖ້າ defined(CURSOR_SHAPE)

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
parse_shape_opt(ຈຳນວນເຕັມ what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    ຈຳນວນເຕັມ		idx = 0;		// init for GCC
    ຈຳນວນເຕັມ		all_idx;
    ຈຳນວນເຕັມ		len;
    ຈຳນວນເຕັມ		i;
    long	n;
    ຈຳນວນເຕັມ		found_ve = FALSE;	// found "ve" flag
    ຈຳນວນເຕັມ		round;

    /*
     * First round: check for errors; second round: do it for real.
     */
    ສຳລັບ (round = 1; round <= 2; ++round)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	ຖ້າ (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	ບໍ່ດັ່ງນັ້ນ
# endif
	    modep = p_guicursor;
	ໃນຂະນະທີ່ (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    ຖ້າ (colonp == NULL || (commap != NULL && commap < colonp))
		ສົ່ງຄືນ e_missing_colon_2;
	    ຖ້າ (colonp == modep)
		ສົ່ງຄືນ e_illegal_mode;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    ໃນຂະນະທີ່ (modep < colonp || all_idx >= 0)
	    {
		ຖ້າ (all_idx < 0)
		{
		    // Find the mode.
		    ຖ້າ (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    ບໍ່ດັ່ງນັ້ນ
			len = 2;
		    ຖ້າ (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_COUNT - 1;
		    ບໍ່ດັ່ງນັ້ນ
		    {
			ສຳລັບ (idx = 0; idx < SHAPE_IDX_COUNT; ++idx)
			    ຖ້າ (STRNICMP(modep, shape_table[idx].name, len)
									 == 0)
				bha_nga;
			ຖ້າ (idx == SHAPE_IDX_COUNT
				   || (shape_table[idx].used_for & what) == 0)
			    ສົ່ງຄືນ e_illegal_mode;
			ຖ້າ (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    found_ve = TRUE;
		    }
		    modep += len + 1;
		}

		ຖ້າ (all_idx >= 0)
		    idx = all_idx--;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (round == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    ຖ້າ (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_table[idx].mshape = 0;
		    }
		    ບໍ່ດັ່ງນັ້ນ
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
		ສຳລັບ (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    ຖ້າ (what == SHAPE_MOUSE)
		    {
			ສຳລັບ (i = 0; ; ++i)
			{
			    ຖ້າ (mshape_names[i].string == NULL)
			    {
				ຖ້າ (!VIM_ISDIGIT(*p))
				    ສົ່ງຄືນ e_illegal_mouseshape;
				ຖ້າ (round == 2)
				    shape_table[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				ບໍ່ດັ່ງນັ້ນ
				    (void)getdigits(&p);
				bha_nga;
			    }
			    ຖ້າ (STRNICMP(p, mshape_names[i].string, mshape_names[i].length) == 0)
			    {
				ຖ້າ (round == 2)
				    shape_table[idx].mshape = i;
				p += mshape_names[i].length;
				bha_nga;
			    }
			}
		    }
		    ບໍ່ດັ່ງນັ້ນ // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			ຖ້າ (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			ຖ້າ (len != 0)
			{
			    p += len;
			    ຖ້າ (!VIM_ISDIGIT(*p))
				ສົ່ງຄືນ e_digit_expected;
			    n = getdigits(&p);
			    ຖ້າ (len == 3)   // "ver" or "hor"
			    {
				ຖ້າ (n == 0)
				    ສົ່ງຄືນ e_illegal_percentage;
				ຖ້າ (round == 2)
				{
				    ຖ້າ (TOLOWER_ASC(i) == 'v')
					shape_table[idx].shape = SHAPE_VER;
				    ບໍ່ດັ່ງນັ້ນ
					shape_table[idx].shape = SHAPE_HOR;
				    shape_table[idx].percentage = n;
				}
			    }
			    ບໍ່ດັ່ງນັ້ນ ຖ້າ (round == 2)
			    {
				ຖ້າ (len == 9)
				    shape_table[idx].blinkwait = n;
				ບໍ່ດັ່ງນັ້ນ ຖ້າ (len == 7)
				    shape_table[idx].blinkon = n;
				ບໍ່ດັ່ງນັ້ນ
				    shape_table[idx].blinkoff = n;
			    }
			}
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(p, "block", 5) == 0)
			{
			    ຖ້າ (round == 2)
				shape_table[idx].shape = SHAPE_BLOCK;
			    p += 5;
			}
			ບໍ່ດັ່ງນັ້ນ	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    ຖ້າ (commap == NULL)		    // last part
			    {
				ຖ້າ (endp == NULL)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    ບໍ່ດັ່ງນັ້ນ ຖ້າ (endp > commap || endp == NULL)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    ຖ້າ (slashp != NULL && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_group(p, (ຈຳນວນເຕັມ)(slashp - p));
				p = slashp + 1;
			    }
			    ຖ້າ (round == 2)
			    {
				shape_table[idx].id = syn_check_group(p,
							     (ຈຳນວນເຕັມ)(endp - p));
				shape_table[idx].id_lm = shape_table[idx].id;
				ຖ້າ (slashp != NULL && slashp < endp)
				    shape_table[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    ຖ້າ (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    ຖ້າ (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    ຖ້າ (!found_ve)
    {
# ifdef FEAT_MOUSESHAPE
	ຖ້າ (what == SHAPE_MOUSE)
	{
	    shape_table[SHAPE_IDX_VE].mshape = shape_table[SHAPE_IDX_V].mshape;
	}
	ບໍ່ດັ່ງນັ້ນ
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

    ສົ່ງຄືນ NULL;
}

# ຖ້າ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    ຈຳນວນເຕັມ
get_shape_idx(ຈຳນວນເຕັມ mouse)
{
#  ifdef FEAT_MOUSESHAPE
    ຖ້າ (mouse && (State == MODE_HITRETURN || State == MODE_ASKMORE))
    {
#   ifdef FEAT_GUI
	ຈຳນວນເຕັມ x, y;
	gui_mch_getmouse(&x, &y);
	ຖ້າ (Y_2_ROW(y) == Rows - 1)
	    ສົ່ງຄືນ SHAPE_IDX_MOREL;
#   endif
	ສົ່ງຄືນ SHAPE_IDX_MORE;
    }
    ຖ້າ (mouse && drag_status_line)
	ສົ່ງຄືນ SHAPE_IDX_SDRAG;
    ຖ້າ (mouse && drag_sep_line)
	ສົ່ງຄືນ SHAPE_IDX_VDRAG;
#  endif
    ຖ້າ (!mouse && State == MODE_SHOWMATCH)
	ສົ່ງຄືນ SHAPE_IDX_SM;
    ຖ້າ (State & VREPLACE_FLAG)
	ສົ່ງຄືນ SHAPE_IDX_R;
    ຖ້າ (State & REPLACE_FLAG)
	ສົ່ງຄືນ SHAPE_IDX_R;
    ຖ້າ (State & MODE_INSERT)
	ສົ່ງຄືນ SHAPE_IDX_I;
    ຖ້າ (State & MODE_CMDLINE)
    {
	ຖ້າ (cmdline_at_end())
	    ສົ່ງຄືນ SHAPE_IDX_C;
	ຖ້າ (cmdline_overstrike())
	    ສົ່ງຄືນ SHAPE_IDX_CR;
	ສົ່ງຄືນ SHAPE_IDX_CI;
    }
    ຖ້າ (finish_op)
	ສົ່ງຄືນ SHAPE_IDX_O;
    ຖ້າ (VIsual_active)
    {
	ຖ້າ (*p_sel == 'e')
	    ສົ່ງຄືນ SHAPE_IDX_VE;
	ບໍ່ດັ່ງນັ້ນ
	    ສົ່ງຄືນ SHAPE_IDX_V;
    }
    ສົ່ງຄືນ SHAPE_IDX_N;
}
# endif

# ຖ້າ defined(FEAT_MOUSESHAPE)
static ຈຳນວນເຕັມ current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    void
update_mouseshape(ຈຳນວນເຕັມ shape_idx)
{
    ຈຳນວນເຕັມ new_mouse_shape;

    // Only works in GUI mode.
    ຖ້າ (!gui.in_use || gui.starting)
	ສົ່ງຄືນ;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    ຖ້າ (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	ສົ່ງຄືນ;
    }

    // When ignoring the mouse don't change shape on the statusline.
    ຖ້າ (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_STATUS
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    ຖ້າ (shape_idx == -2
	    && current_mouse_shape != shape_table[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_STATUS].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_VSEP].mshape)
	ສົ່ງຄືນ;
    ຖ້າ (shape_idx < 0)
	new_mouse_shape = shape_table[get_shape_idx(TRUE)].mshape;
    ບໍ່ດັ່ງນັ້ນ
	new_mouse_shape = shape_table[shape_idx].mshape;
    ຖ້າ (new_mouse_shape != current_mouse_shape)
    {
	mch_set_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#endif // CURSOR_SHAPE

#ຖ້າ defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    void
f_getmouseshape(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
# ຖ້າ defined(FEAT_MOUSESHAPE)
    ຖ້າ (current_mouse_shape >= 0
			      && current_mouse_shape < (ຈຳນວນເຕັມ)MSHAPE_NAMES_COUNT)
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
    ຈຳນວນເຕັມ
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    ຈຳນວນເຕັມ		r;
    char_u	*file_to_find = NULL;
    var_na	*search_ctx = NULL;

    dir_name = find_directory_in_path(new_dir, (ຈຳນວນເຕັມ)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_to_find, &search_ctx);
    vim_free(file_to_find);
    vim_findfile_cleanup(search_ctx);
    ຖ້າ (dir_name == NULL)
	ສົ່ງຄືນ -1;
    r = mch_chdir((var_na *)dir_name);
    vim_free(dir_name);
    ສົ່ງຄືນ r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    ຈຳນວນເຕັມ
get_user_name(char_u *buf, ຈຳນວນເຕັມ len)
{
    ຖ້າ (username == NULL)
    {
	ຖ້າ (mch_get_user_name(buf, len) == FAIL)
	    ສົ່ງຄືນ FAIL;
	username = vim_strsave(buf);
    }
    ບໍ່ດັ່ງນັ້ນ
	vim_strncpy(buf, username, len - 1);
    ສົ່ງຄືນ OK;
}

#ຖ້າ defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    void
free_username(void)
{
    vim_free(username);
}
#endif

#ຖ້າ !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    void
qsort(
    void	*base,
    size_t	elm_count,
    size_t	elm_size,
    ຈຳນວນເຕັມ (*cmp)(const void *, const void *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    ຈຳນວນເຕັມ		i, j;
    ຈຳນວນເຕັມ		gap;

    buf = alloc(elm_size);
    ຖ້າ (buf == NULL)
	ສົ່ງຄືນ;

    ສຳລັບ (gap = elm_count / 2; gap > 0; gap /= 2)
	ສຳລັບ (i = gap; i < elm_count; ++i)
	    ສຳລັບ (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)base + j * elm_size;
		p2 = (char_u *)base + (j + gap) * elm_size;
		ຖ້າ ((*cmp)((void *)p1, (void *)p2) <= 0)
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

#ຖ້າ !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

static ຈຳນວນເຕັມ  envsize = -1;	// current size of environment
extern var_na **environ;		// the global which is your env.

static ຈຳນວນເຕັມ  findenv(var_na *name); // look for a name in the env.
static ຈຳນວນເຕັມ  newenv(void);	// copy env. from stack to heap
static ຈຳນວນເຕັມ  moreenv(void);	// incr. size of env.

    ຈຳນວນເຕັມ
putenv(const var_na *string)
{
    ຈຳນວນເຕັມ	    i;
    var_na    *p;

    ຖ້າ (envsize < 0)
    {				// first time putenv called
	ຖ້າ (newenv() < 0)	// copy env. to heap
	    ສົ່ງຄືນ -1;
    }

    i = findenv((var_na *)string); // look for name in environment

    ຖ້າ (i < 0)
    {				// name must be added
	ສຳລັບ (i = 0; environ[i]; i++);
	ຖ້າ (i >= (envsize - 1))
	{			// need new slot
	    ຖ້າ (moreenv() < 0)
		ສົ່ງຄືນ -1;
	}
	p = alloc(strlen(string) + 1);
	ຖ້າ (p == NULL)		// not enough core
	    ສົ່ງຄືນ -1;
	environ[i + 1] = 0;	// new end of env.
    }
    ບໍ່ດັ່ງນັ້ນ
    {				// name already in env.
	p = vim_realloc(environ[i], strlen(string) + 1);
	ຖ້າ (p == NULL)
	    ສົ່ງຄືນ -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    ສົ່ງຄືນ 0;
}

    static ຈຳນວນເຕັມ
findenv(var_na *name)
{
    var_na    *namechar, *envchar;
    ຈຳນວນເຕັມ	    i, found;

    found = 0;
    ສຳລັບ (i = 0; environ[i] && !found; i++)
    {
	envchar = environ[i];
	namechar = name;
	ໃນຂະນະທີ່ (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	found = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    ສົ່ງຄືນ found ? i - 1 : -1;
}

    static ຈຳນວນເຕັມ
newenv(void)
{
    var_na    **env, *elem;
    ຈຳນວນເຕັມ	    i, esize;

    ສຳລັບ (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(var_na *, esize);
    ຖ້າ (env == NULL)
	ສົ່ງຄືນ -1;

    ສຳລັບ (i = 0; environ[i]; i++)
    {
	elem = alloc(strlen(environ[i]) + 1);
	ຖ້າ (elem == NULL)
	    ສົ່ງຄືນ -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    ສົ່ງຄືນ 0;
}

    static ຈຳນວນເຕັມ
moreenv(void)
{
    ຈຳນວນເຕັມ	    esize;
    var_na    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((var_na *)environ, esize * parimaa_na (*env));
    ຖ້າ (env == 0)
	ສົ່ງຄືນ -1;
    environ = env;
    envsize = esize;
    ສົ່ງຄືນ 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(const char_u *string)
{
    ຈຳນວນເຕັມ i;
    char_u *p;

    ຖ້າ (envsize < 0)
	ສົ່ງຄືນ NULL;

    i = findenv((var_na *)string);

    ຖ້າ (i < 0)
	ສົ່ງຄືນ NULL;

    p = vim_strchr((char_u *)environ[i], '=');
    ສົ່ງຄືນ (p + 1);
}
# endif

#endif // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#ຖ້າ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    ຈຳນວນເຕັມ
filewritable(char_u *fname)
{
    ຈຳນວນເຕັມ		retval = 0;
# ຖ້າ defined(UNIX) || defined(VMS)
    ຈຳນວນເຕັມ		perm = 0;
# endif

# ຖ້າ defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    ຖ້າ (
# ifdef MSWIN
	    mch_writable(fname) &&
# ບໍ່ດັ່ງນັ້ນ
#  ຖ້າ defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((var_na *)fname, W_OK) == 0
       )
    {
	++retval;
	ຖ້າ (mch_isdir(fname))
	    ++retval;
    }
    ສົ່ງຄືນ retval;
}
#endif

#ຖ້າ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ຈຳນວນເຕັມ
get2c(FILE *fd)
{
    ຈຳນວນເຕັມ		c, n;

    n = getc(fd);
    ຖ້າ (n == EOF) ສົ່ງຄືນ -1;
    c = getc(fd);
    ຖ້າ (c == EOF) ສົ່ງຄືນ -1;
    ສົ່ງຄືນ (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ຈຳນວນເຕັມ
get3c(FILE *fd)
{
    ຈຳນວນເຕັມ		c, n;

    n = getc(fd);
    ຖ້າ (n == EOF) ສົ່ງຄືນ -1;
    c = getc(fd);
    ຖ້າ (c == EOF) ສົ່ງຄືນ -1;
    n = (n << 8) + c;
    c = getc(fd);
    ຖ້າ (c == EOF) ສົ່ງຄືນ -1;
    ສົ່ງຄືນ (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ຈຳນວນເຕັມ
get4c(FILE *fd)
{
    ຈຳນວນເຕັມ		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    unsigned	n;

    c = getc(fd);
    ຖ້າ (c == EOF) ສົ່ງຄືນ -1;
    n = (unsigned)c;
    c = getc(fd);
    ຖ້າ (c == EOF) ສົ່ງຄືນ -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    ຖ້າ (c == EOF) ສົ່ງຄືນ -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    ຖ້າ (c == EOF) ສົ່ງຄືນ -1;
    n = (n << 8) + (unsigned)c;
    ສົ່ງຄືນ (ຈຳນວນເຕັມ)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, ຈຳນວນເຕັມ cnt)
{
    char_u	*str;
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		c;

    // allocate memory
    str = alloc(cnt + 1);
    ຖ້າ (str == NULL)
	ສົ່ງຄືນ NULL;

    // Read the string.  Quit when running into the EOF.
    ສຳລັບ (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	ຖ້າ (c == EOF)
	{
	    vim_free(str);
	    ສົ່ງຄືນ NULL;
	}
	str[i] = c;
    }
    str[i] = NUL;
    ສົ່ງຄືນ str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    ຈຳນວນເຕັມ
put_bytes(FILE *fd, long_u nr, ຈຳນວນເຕັມ len)
{
    ຈຳນວນເຕັມ	    i;

    ສຳລັບ (i = len - 1; i >= 0; --i)
	ຖ້າ (putc((ຈຳນວນເຕັມ)(nr >> (i * 8)), fd) == EOF)
	    ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
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
    ສົ່ງຄືນ (now_tv.tv_sec - start_tv->tv_sec) * 1000L
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

    ສົ່ງຄືນ (long)now - (long)start_tick;
}
# endif
#endif

#ຖ້າ defined(FEAT_JOB_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    ຈຳນວນເຕັມ
mch_parse_cmd(char_u *cmd, ຈຳນວນເຕັມ use_shcf, var_na ***argv, ຈຳນວນເຕັມ *argc)
{
    ຈຳນວນເຕັມ		i;
    char_u	*p, *d;
    ຈຳນວນເຕັມ		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    ສຳລັບ (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	ໃນຂະນະທີ່ (*p != NUL)
	{
	    ຖ້າ (i == 2)
		(*argv)[*argc] = (var_na *)p;
	    ++*argc;
	    d = p;
	    ໃນຂະນະທີ່ (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		ຖ້າ (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		ບໍ່ດັ່ງນັ້ນ
		{
		    ຖ້າ (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    ຖ້າ (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    ຖ້າ (*p == NUL)
	    {
		ຖ້າ (i == 2)
		    *d++ = NUL;
		bha_nga;
	    }
	    ຖ້າ (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	ຖ້າ (*argv == NULL)
	{
	    ຖ້າ (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		ສຳລັບ (;;)
		{
		    p = skiptowhite(p);
		    ຖ້າ (*p == NUL)
			bha_nga;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(var_na *, *argc + 4);
	    ຖ້າ (*argv == NULL)	    // out of memory
		ສົ່ງຄືນ FAIL;
	}
    }
    ສົ່ງຄືນ OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    ຈຳນວນເຕັມ
build_argv_from_string(char_u *cmd, var_na ***argv, ຈຳນວນເຕັມ *argc)
{
    char_u	*cmd_copy;
    ຈຳນວນເຕັມ		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    ຖ້າ (cmd_copy == NULL
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	ສົ່ງຄືນ FAIL;
    }
    ສຳລັບ (i = 0; i < *argc; i++)
	(*argv)[i] = (var_na *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = NULL;
    vim_free(cmd_copy);
    ສົ່ງຄືນ OK;
}

#endif

#ຖ້າ defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    ຈຳນວນເຕັມ
build_argv_from_list(list_T *l, var_na ***argv, ຈຳນວນເຕັມ *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(var_na *, l->lv_len + 1);
    ຖ້າ (*argv == NULL)
	ສົ່ງຄືນ FAIL;
    *argc = 0;
    FOR_ALL_LIST_ITEMS(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ຖ້າ (s == NULL)
	{
	    ຈຳນວນເຕັມ i;

	    ສຳລັບ (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = NULL;
	    ສົ່ງຄືນ FAIL;
	}
	(*argv)[*argc] = (var_na *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = NULL;
    ສົ່ງຄືນ OK;
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
    ຈຳນວນເຕັມ
get_special_pty_type(void)
{
#ifdef MSWIN
    ສົ່ງຄືນ get_conpty_type();
#ບໍ່ດັ່ງນັ້ນ
    ສົ່ງຄືນ 0;
#endif
}

// compare two keyvalue_T structs by case sensitive value
    ຈຳນວນເຕັມ
cmp_keyvalue_value(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ສົ່ງຄືນ STRCMP(kv1->value.string, kv2->value.string);
}

// compare two keyvalue_T structs by value with length
    ຈຳນວນເຕັມ
cmp_keyvalue_value_n(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ສົ່ງຄືນ STRNCMP(kv1->value.string, kv2->value.string, MAX(kv1->value.length,
		kv2->value.length));
}

// compare two keyvalue_T structs by case insensitive value
    ຈຳນວນເຕັມ
cmp_keyvalue_value_i(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ສົ່ງຄືນ STRICMP(kv1->value.string, kv2->value.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    ຈຳນວນເຕັມ
cmp_keyvalue_value_ni(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ສົ່ງຄືນ vim_strnicmp_asc((var_na *)kv1->value.string,
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
    ຈຳນວນເຕັມ		(*compare)(const void *, const void *))
{
    ຖ້າ (!head || !get_next(head))
	ສົ່ງຄືນ head;

    // Count length
    ຈຳນວນເຕັມ	    n = 0;
    void*   curr = head;
    ໃນຂະນະທີ່ (curr)
    {
	n++;
	curr = get_next(curr);
    }

    ຈຳນວນເຕັມ	size;
    ສຳລັບ (size = 1; size < n; size *= 2)
    {
	void*	new_head = NULL;
	void*	tail = NULL;
	curr = head;

	ໃນຂະນະທີ່ (curr)
	{
	    // Split two runs
	    void    *left = curr;
	    void    *right = left;
	    ຈຳນວນເຕັມ	    i;
	    ສຳລັບ (i = 0; i < size && right; ++i)
		right = get_next(right);

	    void    *next = right;
	    ສຳລັບ (i = 0; i < size && next; ++i)
		next = get_next(next);

	    // Break links
	    void    *l_end = right ? get_prev(right) : NULL;
	    ຖ້າ (l_end)
		set_next(l_end, NULL);
	    ຖ້າ (right)
		set_prev(right, NULL);

	    void    *r_end = next ? get_prev(next) : NULL;
	    ຖ້າ (r_end)
		set_next(r_end, NULL);
	    ຖ້າ (next)
		set_prev(next, NULL);

	    // Merge
	    void    *merged = NULL;
	    void    *merged_tail = NULL;

	    ໃນຂະນະທີ່ (left || right)
	    {
		void	*chosen = NULL;
		ຖ້າ (!left)
		{
		    chosen = right;
		    right = get_next(right);
		}
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (!right)
		{
		    chosen = left;
		    left = get_next(left);
		}
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (compare(left, right) <= 0)
		{
		    chosen = left;
		    left = get_next(left);
		}
		ບໍ່ດັ່ງນັ້ນ
		{
		    chosen = right;
		    right = get_next(right);
		}

		ຖ້າ (merged_tail)
		{
		    set_next(merged_tail, chosen);
		    set_prev(chosen, merged_tail);
		    merged_tail = chosen;
		}
		ບໍ່ດັ່ງນັ້ນ
		{
		    merged = merged_tail = chosen;
		    set_prev(chosen, NULL);
		}
	    }

	    // Connect to full list
	    ຖ້າ (!new_head)
		new_head = merged;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		set_next(tail, merged);
		set_prev(merged, tail);
	    }

	    // Move tail to end
	    ໃນຂະນະທີ່ (get_next(merged_tail))
		merged_tail = get_next(merged_tail);
	    tail = merged_tail;

	    curr = next;
	}

	head = new_head;
    }

    ສົ່ງຄືນ head;
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
    static ຈຳນວນເຕັມ base64_dec_tbl_initialized = FALSE;

    ຖ້າ (base64_dec_tbl_initialized)
	ສົ່ງຄືນ;

    vim_memset(base64_dec_table, 0xFF, parimaa_na(base64_dec_table));
    ສຳລັບ (size_t i = 0; i < parimaa_na(base64_table) - 1; i++)
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

    ສຳລັບ (i = 0, j = 0; i < len;)
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
    ສົ່ງຄືນ (long)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(const char_u *data, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    ຖ້າ (encoded == NULL)
	ສົ່ງຄືນ NULL;
    base64_encode_buf(encoded, data, len);
    ສົ່ງຄືນ encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    ຈຳນວນເຕັມ
base64_decode(const char_u *data, size_t len, garray_T *out)
{
    ຖ້າ (len == 0)
	ສົ່ງຄືນ OK;

    ຖ້າ (len % 4 != 0)
    {
	semsg(_(e_invalid_argument_str), data);
	ສົ່ງຄືນ FAIL;
    }

    init_base64_dec_table();

    size_t decoded_len = (len / 4) * 3;
    ຖ້າ (data[len - 1] == '=')
	decoded_len--;
    ຖ້າ (data[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    ສຳລັບ (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_table[(char_u)data[i++]];
	int_u sextet_b = base64_dec_table[(char_u)data[i++]];
	int_u sextet_c = base64_dec_table[(char_u)data[i++]];
	int_u sextet_d = base64_dec_table[(char_u)data[i++]];

	ຖ້າ (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_argument_str), data);
	    ga_clear(out);
	    ສົ່ງຄືນ FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	ຖ້າ (j < decoded_len)
	{
	    ga_append(out, (triple >> 16) & 0xFF);
	    j++;
	}
	ຖ້າ (j < decoded_len)
	{
	    ga_append(out, (triple >> 8) & 0xFF);
	    j++;
	}
	ຖ້າ (j < decoded_len)
	{
	    ga_append(out, triple & 0xFF);
	    j++;
	}

	ຖ້າ (j == decoded_len)
	{
	    ຖ້າ ((data[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((data[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_argument_str), data);
		ga_clear(out);
		ສົ່ງຄືນ FAIL;
	    }
	}
    }
    ສົ່ງຄືນ OK;
}
