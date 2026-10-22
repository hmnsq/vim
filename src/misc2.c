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
#অন্তর্ভাব "vim.h"

স্থির char_u	*username = রিক্ত; // cached result of mch_get_user_name()

স্থির পূর্ণাঙ্ক coladvance2(pos_T *pos, পূর্ণাঙ্ক addspaces, পূর্ণাঙ্ক finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    পূর্ণাঙ্ক
virtual_active(শূন্য)
{
    unsigned পূর্ণাঙ্ক cur_ve_flags = get_ve_flags();

    যদি (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_ঢোকাও) && (State & MODE_ঢোকাও)))
	প্রত্যাগম TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    যদি (virtual_op != MAYBE)
	প্রত্যাগম virtual_op;
    প্রত্যাগম (cur_ve_flags & VE_BLOCK) && VIsual_active && VIsual_mode == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    পূর্ণাঙ্ক
getviscol(শূন্য)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, রিক্ত, রিক্ত, GETVCOL_NO_ABOVE);
    প্রত্যাগম (পূর্ণাঙ্ক)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    পূর্ণাঙ্ক
coladvance_force(colnr_T wcol)
{
    পূর্ণাঙ্ক rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    যদি (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    অন্যথা
	// Virtcol is valid
	set_valid_virtcol(curwin, wcol);
    প্রত্যাগম rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    পূর্ণাঙ্ক
getviscol2(colnr_T col, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = col;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, রিক্ত, রিক্ত, GETVCOL_NO_ABOVE);
    প্রত্যাগম (পূর্ণাঙ্ক)x;
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
    পূর্ণাঙ্ক
coladvance(colnr_T wantcol)
{
    পূর্ণাঙ্ক rc = getvpos(&curwin->w_cursor, wantcol);

    যদি (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    অন্যথা যদি (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	set_valid_virtcol(curwin, wantcol);
    প্রত্যাগম rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    পূর্ণাঙ্ক
getvpos(pos_T *pos, colnr_T wantcol)
{
    প্রত্যাগম coladvance2(pos, FALSE, virtual_active(), wantcol);
}

    স্থির পূর্ণাঙ্ক
coladvance2(
    pos_T	*pos,
    পূর্ণাঙ্ক		addspaces,	// change the text to achieve our goal?
    পূর্ণাঙ্ক		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    পূর্ণাঙ্ক		idx;
    char_u	*সারি;
    পূর্ণাঙ্ক		linelen;
    colnr_T	col = 0;
    পূর্ণাঙ্ক		csize = 0;
    পূর্ণাঙ্ক		one_more;
#ifdef FEAT_LINEBREAK
    পূর্ণাঙ্ক		head = 0;
#endif

    one_more = (State & MODE_ঢোকাও)
		    || restart_edit != NUL
		    || (VIsual_active && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    সারি = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    যদি (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_more;
	    col = wcol;

	    যদি ((addspaces || finetune) && !VIsual_active)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_more;
		যদি (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    অন্যথা
    {
	পূর্ণাঙ্ক		width = curwin->w_width - win_col_off(curwin);
	chartabsize_T	cts;

	যদি (finetune
		&& curwin->w_p_wrap
		&& curwin->w_width != 0
		&& wcol >= (colnr_T)width
		&& width > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    যদি (csize > 0)
		csize--;

	    যদি (wcol / width > (colnr_T)csize / width
		    && ((State & MODE_ঢোকাও) == 0 || (পূর্ণাঙ্ক)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / width + 1) * width - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, সারি, সারি);
#ifdef FEAT_PROP_POPUP
	cts.cts_no_above = true;
#endif
	যাবত্ (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ifdef FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &head, রিক্ত);
	    MB_PTR_ADV(cts.cts_ptr);
#অন্যথা
	    csize = lbr_chartabsize_adv(&cts);
#endif
	    cts.cts_vcol += csize;
	}
	col = cts.cts_vcol;
	idx = (পূর্ণাঙ্ক)(cts.cts_ptr - সারি);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	যদি (col > wcol || (!virtual_active() && one_more == 0))
	{
	    idx -= 1;
#ifdef FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= head;
#endif
	    col -= csize;
	}

	যদি (virtual_active()
		&& addspaces
		&& wcol >= 0
		&& ((col != wcol && col != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    যদি (সারি[idx] == NUL)
	    {
		// Append spaces
		পূর্ণাঙ্ক	correct = wcol - col;
		char_u	*newline = alloc(idx + correct + 1);
		পূর্ণাঙ্ক	t;

		যদি (newline == রিক্ত)
		    প্রত্যাগম FAIL;

		প্রতি (t = 0; t < idx; ++t)
		    newline[t] = সারি[t];

		প্রতি (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, (colnr_T)idx);
		idx += correct;
		col = wcol;
	    }
	    অন্যথা
	    {
		// Break a tab
		পূর্ণাঙ্ক	correct = wcol - col - csize + 1; // negative!!
		char_u	*newline;
		পূর্ণাঙ্ক	t, s = 0;
		পূর্ণাঙ্ক	v;

		যদি (-correct > csize)
		    প্রত্যাগম FAIL;

		newline = alloc(linelen + csize);
		যদি (newline == রিক্ত)
		    প্রত্যাগম FAIL;

		প্রতি (t = 0; t < linelen; t++)
		{
		    যদি (t != idx)
			newline[s++] = সারি[t];
		    অন্যথা
			প্রতি (v = 0; v < csize; v++)
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

    যদি (idx < 0)
	pos->col = 0;
    অন্যথা
	pos->col = idx;

    pos->coladd = 0;

    যদি (finetune)
    {
	যদি (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    যদি (!one_more)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, রিক্ত, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক b = (পূর্ণাঙ্ক)wcol - (পূর্ণাঙ্ক)col;

	    // The difference between wcol and col is used to set coladd.
	    যদি (b > 0 && b < (MAXCOL - 2 * curwin->w_width))
		pos->coladd = b;

	    col += b;
	}
    }

    // prevent from moving onto a trail byte
    যদি (has_mbyte)
	mb_adjustpos(curbuf, pos);

    যদি (wcol < 0 || col < wcol)
	প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    পূর্ণাঙ্ক
inc_cursor(শূন্য)
{
    প্রত্যাগম inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    পূর্ণাঙ্ক
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    যদি (lp->col != MAXCOL)
    {
	p = ml_get_pos(lp);
	যদি (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    যদি (has_mbyte)
	    {
		পূর্ণাঙ্ক l = (*mb_ptr2len)(p);

		lp->col += l;
		প্রত্যাগম ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->col++;
	    lp->coladd = 0;
	    প্রত্যাগম ((p[1] != NUL) ? 0 : 2);
	}
    }
    যদি (lp->lnum != curbuf->b_ml.ml_সারি_গুনতি)     // there is a next line
    {
	lp->col = 0;
	lp->lnum++;
	lp->coladd = 0;
	প্রত্যাগম 1;
    }
    প্রত্যাগম -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    পূর্ণাঙ্ক
incl(pos_T *lp)
{
    পূর্ণাঙ্ক	    r;

    যদি ((r = inc(lp)) >= 1 && lp->col)
	r = inc(lp);
    প্রত্যাগম r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    পূর্ণাঙ্ক
dec_cursor(শূন্য)
{
    প্রত্যাগম dec(&curwin->w_cursor);
}

    পূর্ণাঙ্ক
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    যদি (lp->col == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	যদি (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	প্রত্যাগম 0;
    }

    যদি (lp->col > 0)
    {
	// still within line
	lp->col--;
	যদি (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	}
	প্রত্যাগম 0;
    }

    যদি (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	যদি (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	প্রত্যাগম 1;
    }

    // at start of file
    প্রত্যাগম -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    পূর্ণাঙ্ক
decl(pos_T *lp)
{
    পূর্ণাঙ্ক	    r;

    যদি ((r = dec(lp)) == 1 && lp->col)
	r = dec(lp);
    প্রত্যাগম r;
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
    যদি (hasAnyFolding(wp))
    {
	যদি (lnum > cursor)
	{
	    যাবত্ (lnum > cursor)
	    {
		(শূন্য)hasFoldingWin(wp, lnum, &lnum, রিক্ত, TRUE, রিক্ত);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		যদি (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	অন্যথা যদি (lnum < cursor)
	{
	    যাবত্ (lnum < cursor)
	    {
		(শূন্য)hasFoldingWin(wp, lnum, রিক্ত, &lnum, TRUE, রিক্ত);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		যদি (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    অন্যথা
#endif
	retval = lnum - cursor;

    প্রত্যাগম retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    শূন্য
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    যদি (pos->lnum > buf->b_ml.ml_সারি_গুনতি)
	pos->lnum = buf->b_ml.ml_সারি_গুনতি;

    যদি (pos->col > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	যদি (pos->col > len)
	    pos->col = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    শূন্য
check_cursor_lnum(শূন্য)
{
    যদি (curwin->w_cursor.lnum > curbuf->b_ml.ml_সারি_গুনতি)
    {
#ifdef FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	যদি (!hasFolding(curbuf->b_ml.ml_সারি_গুনতি,
						&curwin->w_cursor.lnum, রিক্ত))
#endif
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_সারি_গুনতি;
    }
    যদি (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    শূন্য
check_cursor_col(শূন্য)
{
    check_cursor_col_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    শূন্য
check_cursor_col_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.col;
    colnr_T      oldcoladd = win->w_cursor.col + win->w_cursor.coladd;
    unsigned পূর্ণাঙ্ক cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_buffer, win->w_cursor.lnum);
    যদি (len == 0)
	win->w_cursor.col = 0;
    অন্যথা যদি (win->w_cursor.col >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	যদি ((State & MODE_ঢোকাও) || restart_edit
		|| (VIsual_active && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_active())
	    win->w_cursor.col = len;
	অন্যথা
	{
	    win->w_cursor.col = len - 1;
	    // Move the cursor to the head byte.
	    যদি (has_mbyte)
		mb_adjustpos(win->w_buffer, &win->w_cursor);
	}
    }
    অন্যথা যদি (win->w_cursor.col < 0)
	win->w_cursor.col = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    যদি (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    অন্যথা যদি (cur_ve_flags == VE_ALL)
    {
	যদি (oldcoladd > win->w_cursor.col)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.col;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    যদি (win->w_cursor.col + 1 < len)
	    {
		পূর্ণাঙ্ক cs, ce;

		getvcol(win, &win->w_cursor, &cs, রিক্ত, &ce, 0);
		যদি (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	অন্যথা
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    শূন্য
check_cursor(শূন্য)
{
    check_cursor_lnum();
    check_cursor_col();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    শূন্য
check_visual_pos(শূন্য)
{
    যদি (VIsual.lnum > curbuf->b_ml.ml_সারি_গুনতি)
    {
	VIsual.lnum = curbuf->b_ml.ml_সারি_গুনতি;
	VIsual.col = 0;
	VIsual.coladd = 0;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক len = ml_get_len(VIsual.lnum);

	যদি (VIsual.col > len)
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
    শূন্য
adjust_cursor_col(শূন্য)
{
    যদি (curwin->w_cursor.col > 0
	    && (!VIsual_active || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    পূর্ণাঙ্ক
set_leftcol(colnr_T leftcol)
{
    পূর্ণাঙ্ক		retval = FALSE;

    // Return quickly when there is no change.
    যদি (curwin->w_leftcol == leftcol)
	প্রত্যাগম FALSE;
    curwin->w_leftcol = leftcol;

    changed_cline_bef_curs();
    long lastcol = curwin->w_leftcol + curwin->w_width - curwin_col_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    long siso = get_sidescrolloff_মান();
    যদি (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    অন্যথা যদি (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(শূন্য)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, রিক্ত, &e, 0);
    যদি (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    অন্যথা যদি (s < curwin->w_leftcol)
    {
	retval = TRUE;
	যদি (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    changed_cline_bef_curs();
	}
    }

    যদি (retval)
	curwin->w_set_curswant = true;
    redraw_later(UPD_NOT_VALID);
    প্রত্যাগম retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    পূর্ণাঙ্ক
copy_option_part(
    char_u	**option,
    char_u	*buf,
    পূর্ণাঙ্ক		maxlen,
    বর্ণ	*sep_chars)
{
    পূর্ণাঙ্ক	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    যদি (*p == '.')
	buf[len++] = *p++;
    যাবত্ (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == রিক্ত)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	যদি (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != রিক্ত)
	    ++p;
	যদি (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    যদি (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_to_option_part(p);	// p points to next file name

    *option = p;
    প্রত্যাগম len;
}

#যদি !defined(HAVE_MEMSET) && !defined(PROTO)
    শূন্য *
vim_memset(শূন্য *ptr, পূর্ণাঙ্ক c, size_t size)
{
    বর্ণ *p = ptr;

    যাবত্ (size-- > 0)
	*p++ = c;
    প্রত্যাগম ptr;
}
#endif

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    পূর্ণাঙ্ক
vim_isspace(পূর্ণাঙ্ক x)
{
    প্রত্যাগম ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

স্থির সংরচনা modmasktable
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
#যদি defined(MACOS_X) || defined(FEAT_GUI_GTK)
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
#define MOD_চাবিগুলি_ENTRY_SIZE 5

স্থির char_u modifier_চাবিগুলি_table[] =
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
    MOD_MASK_CTRL,  KS_EXTRA, (পূর্ণাঙ্ক)KE_C_END,	'@', '7',	// end
    MOD_MASK_SHIFT, '*', '9',			'@', '9',	// exit
    MOD_MASK_SHIFT, '*', '0',			'@', '0',	// find
    MOD_MASK_SHIFT, '#', '1',			'%', '1',	// help
    MOD_MASK_SHIFT, '#', '2',			'k', 'h',	// home
    MOD_MASK_CTRL,  KS_EXTRA, (পূর্ণাঙ্ক)KE_C_HOME,	'k', 'h',	// home
    MOD_MASK_SHIFT, '#', '3',			'k', 'I',	// insert
    MOD_MASK_SHIFT, '#', '4',			'k', 'l',	// left arrow
    MOD_MASK_CTRL,  KS_EXTRA, (পূর্ণাঙ্ক)KE_C_বাম,	'k', 'l',	// left arrow
    MOD_MASK_SHIFT, '%', 'a',			'%', '3',	// message
    MOD_MASK_SHIFT, '%', 'b',			'%', '4',	// move
    MOD_MASK_SHIFT, '%', 'c',			'%', '5',	// next
    MOD_MASK_SHIFT, '%', 'd',			'%', '7',	// options
    MOD_MASK_SHIFT, '%', 'e',			'%', '8',	// previous
    MOD_MASK_SHIFT, '%', 'f',			'%', '9',	// print
    MOD_MASK_SHIFT, '%', 'g',			'%', '0',	// redo
    MOD_MASK_SHIFT, '%', 'h',			'&', '3',	// replace
    MOD_MASK_SHIFT, '%', 'i',			'k', 'r',	// right arr.
    MOD_MASK_CTRL,  KS_EXTRA, (পূর্ণাঙ্ক)KE_C_ডান,	'k', 'r',	// right arr.
    MOD_MASK_SHIFT, '%', 'j',			'&', '5',	// resume
    MOD_MASK_SHIFT, '!', '1',			'&', '6',	// save
    MOD_MASK_SHIFT, '!', '2',			'&', '7',	// suspend
    MOD_MASK_SHIFT, '!', '3',			'&', '8',	// undo
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_UP,	'k', 'u',	// up arrow
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_DOWN,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_XF1,	KS_EXTRA, (পূর্ণাঙ্ক)KE_XF1,
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_XF2,	KS_EXTRA, (পূর্ণাঙ্ক)KE_XF2,
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_XF3,	KS_EXTRA, (পূর্ণাঙ্ক)KE_XF3,
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_XF4,	KS_EXTRA, (পূর্ণাঙ্ক)KE_XF4,

    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F1,	'k', '1',	// F1
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F2,	'k', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F3,	'k', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F4,	'k', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F5,	'k', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F6,	'k', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F7,	'k', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F8,	'k', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F9,	'k', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F10,	'k', ';',	// F10

    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F11,	'F', '1',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F12,	'F', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F13,	'F', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F14,	'F', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F15,	'F', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F16,	'F', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F17,	'F', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F18,	'F', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F19,	'F', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F20,	'F', 'A',

    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F21,	'F', 'B',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F22,	'F', 'C',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F23,	'F', 'D',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F24,	'F', 'E',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F25,	'F', 'F',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F26,	'F', 'G',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F27,	'F', 'H',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F28,	'F', 'I',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F29,	'F', 'J',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F30,	'F', 'K',

    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F31,	'F', 'L',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F32,	'F', 'M',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F33,	'F', 'N',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F34,	'F', 'O',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F35,	'F', 'P',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F36,	'F', 'Q',
    MOD_MASK_SHIFT, KS_EXTRA, (পূর্ণাঙ্ক)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_MASK_SHIFT, 'k', 'B',			KS_EXTRA, (পূর্ণাঙ্ক)KE_TAB,

    NUL
};

স্থির সংরচনা চাবি_name_entry
{
    পূর্ণাঙ্ক		enabled;	    // is this entry available (TRUE/FALSE)?
    পূর্ণাঙ্ক		চাবি;		    // special key code or ascii value
    string_T	name;		    // name of key
    পূর্ণাঙ্ক		is_alt;		    // is an alternative name
} চাবি_names_table[] =
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
#অন্যথা
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
#অন্যথা
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
    {TRUE, K_বাম, STR_LITERAL_INIT("Left"), FALSE},
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
#অন্যথা
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
#অন্যথা
    FALSE,
#endif
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_ডান, STR_LITERAL_INIT("Right"), FALSE},
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
#অন্যথা
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
#অন্যথা
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
    স্থির পূর্ণাঙ্ক
name_to_mod_mask(পূর্ণাঙ্ক c)
{
    পূর্ণাঙ্ক	    i;

    c = TOUPPER_ASC(c);
    প্রতি (i = 0; mod_mask_table[i].mod_mask != 0; i++)
	যদি (c == mod_mask_table[i].name)
	    প্রত্যাগম mod_mask_table[i].mod_flag;
    প্রত্যাগম 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    পূর্ণাঙ্ক
simplify_চাবি(পূর্ণাঙ্ক চাবি, পূর্ণাঙ্ক *modifiers)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    key0;
    পূর্ণাঙ্ক	    key1;

    যদি (!(*modifiers & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	প্রত্যাগম চাবি;

    // TAB is a special case
    যদি (চাবি == TAB && (*modifiers & MOD_MASK_SHIFT))
    {
	*modifiers &= ~MOD_MASK_SHIFT;
	প্রত্যাগম K_S_TAB;
    }
    key0 = KEY2TERMCAP0(চাবি);
    key1 = KEY2TERMCAP1(চাবি);
    প্রতি (i = 0; modifier_চাবিগুলি_table[i] != NUL; i += MOD_চাবিগুলি_ENTRY_SIZE)
    {
	যদি (key0 == modifier_চাবিগুলি_table[i + 3]
		&& key1 == modifier_চাবিগুলি_table[i + 4]
		&& (*modifiers & modifier_চাবিগুলি_table[i]))
	{
	    *modifiers &= ~modifier_চাবিগুলি_table[i];
	    প্রত্যাগম TERMCAP2_চাবি(modifier_চাবিগুলি_table[i + 1],
		    modifier_চাবিগুলি_table[i + 2]);
	}
    }
    প্রত্যাগম চাবি;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    পূর্ণাঙ্ক
handle_x_চাবিগুলি(পূর্ণাঙ্ক চাবি)
{
    বিকল্প (চাবি)
    {
	পক্ষ K_XUP:	প্রত্যাগম K_UP;
	পক্ষ K_XDOWN:	প্রত্যাগম K_DOWN;
	পক্ষ K_XLEFT:	প্রত্যাগম K_বাম;
	পক্ষ K_XRIGHT:	প্রত্যাগম K_ডান;
	পক্ষ K_XHOME:	প্রত্যাগম K_HOME;
	পক্ষ K_ZHOME:	প্রত্যাগম K_HOME;
	পক্ষ K_XEND:	প্রত্যাগম K_END;
	পক্ষ K_ZEND:	প্রত্যাগম K_END;
	পক্ষ K_XF1:	প্রত্যাগম K_F1;
	পক্ষ K_XF2:	প্রত্যাগম K_F2;
	পক্ষ K_XF3:	প্রত্যাগম K_F3;
	পক্ষ K_XF4:	প্রত্যাগম K_F4;
	পক্ষ K_S_XF1:	প্রত্যাগম K_S_F1;
	পক্ষ K_S_XF2:	প্রত্যাগম K_S_F2;
	পক্ষ K_S_XF3:	প্রত্যাগম K_S_F3;
	পক্ষ K_S_XF4:	প্রত্যাগম K_S_F4;
    }
    প্রত্যাগম চাবি;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_চাবি_name(পূর্ণাঙ্ক c, পূর্ণাঙ্ক modifiers)
{
    স্থির char_u string[MAX_চাবি_NAME_LEN + 1];
    পূর্ণাঙ্ক	    i, idx, len;
    পূর্ণাঙ্ক	    table_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    যদি (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_চাবি)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    যদি (IS_SPECIAL(c))
    {
	প্রতি (i = 0; modifier_চাবিগুলি_table[i] != 0; i += MOD_চাবিগুলি_ENTRY_SIZE)
	    যদি (       KEY2TERMCAP0(c) == (পূর্ণাঙ্ক)modifier_চাবিগুলি_table[i + 1]
		    && (পূর্ণাঙ্ক)KEY2TERMCAP1(c) == (পূর্ণাঙ্ক)modifier_চাবিগুলি_table[i + 2])
	    {
		modifiers |= modifier_চাবিগুলি_table[i];
		c = TERMCAP2_চাবি(modifier_চাবিগুলি_table[i + 3],
						   modifier_চাবিগুলি_table[i + 4]);
		ভঙ্গ;
	    }
    }

    // try to find the key in the special key table
    table_idx = find_special_চাবি_in_table(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    যদি (c > 0 && (*mb_char2len)(c) == 1)
    {
	যদি (table_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_MASK_ALT;
	    // try again, to find the un-alted key in the special key table
	    table_idx = find_special_চাবি_in_table(c);
	}
	যদি (table_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_MASK_CTRL;
	}
    }

    // translate the modifier into a string
    প্রতি (i = 0; mod_mask_table[i].name != 'A'; i++)
	যদি ((modifiers & mod_mask_table[i].mod_mask)
						== mod_mask_table[i].mod_flag)
	{
	    string[idx++] = mod_mask_table[i].name;
	    string[idx++] = (char_u)'-';
	}

    যদি (table_idx < 0)		// unknown special key, may output t_xx
    {
	যদি (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	অন্যথা
	{
	    len = (*mb_char2len)(c);
	    যদি (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    অন্যথা যদি (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    অন্যথা
	    {
		char_u	*s = transchar(c);
		যাবত্ (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    অন্যথা		// use name of special key
    {
	string_T    *s;

	s = &চাবি_names_table[table_idx].name;

	যদি (s->length + idx + 2 <= MAX_চাবি_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (পূর্ণাঙ্ক)s->length;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    প্রত্যাগম string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    পূর্ণাঙ্ক
trans_special(
    char_u	**srcp,
    char_u	*dst,
    পূর্ণাঙ্ক		flags,		// FSK_ values
    পূর্ণাঙ্ক		escape_ks,	// escape K_SPECIAL bytes in the character
    পূর্ণাঙ্ক		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    পূর্ণাঙ্ক		modifiers = 0;
    পূর্ণাঙ্ক		চাবি;

    চাবি = find_special_চাবি(srcp, &modifiers, flags, did_simplify);
    যদি (চাবি == 0)
	প্রত্যাগম 0;

    প্রত্যাগম special_to_buf(চাবি, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    পূর্ণাঙ্ক
special_to_buf(পূর্ণাঙ্ক চাবি, পূর্ণাঙ্ক modifiers, পূর্ণাঙ্ক escape_ks, char_u *dst)
{
    পূর্ণাঙ্ক		dlen = 0;

    // Put the appropriate modifier in a string
    যদি (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    যদি (IS_SPECIAL(চাবি))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(চাবি);
	dst[dlen++] = KEY2TERMCAP1(চাবি);
    }
    অন্যথা যদি (escape_ks)
	dlen = (পূর্ণাঙ্ক)(add_char2buf(চাবি, dst + dlen) - dst);
    অন্যথা যদি (has_mbyte)
	dlen += (*mb_char2bytes)(চাবি, dst + dlen);
    অন্যথা
	dst[dlen++] = চাবি;

    প্রত্যাগম dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    পূর্ণাঙ্ক
find_special_চাবি(
    char_u	**srcp,
    পূর্ণাঙ্ক		*modp,
    পূর্ণাঙ্ক		flags,		// FSK_ values
    পূর্ণাঙ্ক		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*last_dash;
    char_u	*end_of_name;
    char_u	*src;
    char_u	*bp;
    পূর্ণাঙ্ক		in_string = flags & FSK_IN_STRING;
    পূর্ণাঙ্ক		modifiers;
    পূর্ণাঙ্ক		bit;
    পূর্ণাঙ্ক		চাবি;
    uvarnumber_T	n;
    পূর্ণাঙ্ক		l;

    src = *srcp;
    যদি (src[0] != '<')
	প্রত্যাগম 0;
    যদি (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    last_dash = src;
    প্রতি (bp = src + 1; *bp == '-' || vim_isNormalIDc(*bp); bp++)
    {
	যদি (*bp == '-')
	{
	    last_dash = bp;
	    যদি (bp[1] != NUL)
	    {
		যদি (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		অন্যথা
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		যদি (!(in_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		অন্যথা যদি (in_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	যদি (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	অন্যথা যদি (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, রিক্ত, &l, STR2NR_ALL, রিক্ত, রিক্ত, 0, TRUE, রিক্ত);
	    যদি (l == 0)
	    {
		emsg(_(e_invalid_argument));
		প্রত্যাগম 0;
	    }
	    bp += l + 5;
	    ভঙ্গ;
	}
    }

    যদি (*bp == '>')	// found matching '>'
    {
	end_of_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	প্রতি (bp = src + 1; bp < last_dash; bp++)
	{
	    যদি (*bp != '-')
	    {
		bit = name_to_mod_mask(*bp);
		যদি (bit == 0x0)
		    ভঙ্গ;	// Illegal modifier name
		modifiers |= bit;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	যদি (bp >= last_dash)
	{
	    যদি (STRNICMP(last_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(last_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(last_dash + 6, রিক্ত, &l, STR2NR_ALL, রিক্ত,
							    &n, 0, TRUE, রিক্ত);
		যদি (l == 0)
		{
		    emsg(_(e_invalid_argument));
		    প্রত্যাগম 0;
		}
		চাবি = (পূর্ণাঙ্ক)n;
	    }
	    অন্যথা
	    {
		পূর্ণাঙ্ক off = 1;

		// Modifier with single letter, or special key name.
		যদি (in_string && last_dash[1] == '\\' && last_dash[2] == '"')
		    off = 2;
		যদি (has_mbyte)
		    l = mb_ptr2len(last_dash + off);
		অন্যথা
		    l = 1;
		যদি (modifiers != 0 && last_dash[l + off] == '>')
		    চাবি = PTR2CHAR(last_dash + off);
		অন্যথা
		{
		    চাবি = get_special_চাবি_code(last_dash + off);
		    যদি (!(flags & FSK_KEEP_X_চাবি))
			চাবি = handle_x_চাবিগুলি(চাবি);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    যদি (চাবি != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		চাবি = simplify_চাবি(চাবি, &modifiers);

		যদি ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    যদি (চাবি == K_BS)
			চাবি = BS;
		    অন্যথা যদি (চাবি == K_DEL || চাবি == K_KDEL)
			চাবি = DEL;
		}
		অন্যথা যদি (চাবি == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    যদি ((flags & FSK_SIMPLIFY) != 0)
		    {
			যদি (did_simplify != রিক্ত)
			    *did_simplify = TRUE;
		    }
		    অন্যথা
			চাবি = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		যদি (!IS_SPECIAL(চাবি))
		    চাবি = extract_modifiers(চাবি, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = end_of_name;
		প্রত্যাগম চাবি;
	    }
	}
    }
    প্রত্যাগম 0;
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
    পূর্ণাঙ্ক
may_adjust_চাবি_for_ctrl(পূর্ণাঙ্ক modifiers, পূর্ণাঙ্ক চাবি)
{
    যদি ((modifiers & MOD_MASK_CTRL) == 0)
	প্রত্যাগম চাবি;

    যদি (ASCII_ISALPHA(চাবি))
    {
#ifdef FEAT_TERMINAL
	check_no_reduce_চাবিগুলি();  // may update the no_reduce_keys flag
#endif
	প্রত্যাগম no_reduce_চাবিগুলি == 0 ? TOUPPER_ASC(চাবি) : চাবি;
    }
    যদি (চাবি == '2')
	প্রত্যাগম '@';
    যদি (চাবি == '6')
	প্রত্যাগম '^';
    যদি (চাবি == '-')
	প্রত্যাগম '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    যদি (চাবি == '$' && (modifiers & MOD_MASK_SHIFT) == 0)
	প্রত্যাগম ']';

    প্রত্যাগম চাবি;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    পূর্ণাঙ্ক
may_remove_shift_modifier(পূর্ণাঙ্ক modifiers, পূর্ণাঙ্ক চাবি)
{
    যদি ((modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
#ifdef FEAT_GUI_GTK
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_CMD)
#endif
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    && ((চাবি >= '!' && চাবি <= '/')
		|| (চাবি >= ':' && চাবি <= 'Z')
		|| vim_isdigit(চাবি)
		|| (চাবি >= '[' && চাবি <= '`')
		|| (চাবি >= '{' && চাবি <= '~')))
	প্রত্যাগম modifiers & ~MOD_MASK_SHIFT;

    যদি (modifiers == (MOD_MASK_SHIFT | MOD_MASK_CTRL)
		&& (চাবি == '{' || চাবি == '}' || চাবি == '|'))
	প্রত্যাগম modifiers & ~MOD_MASK_SHIFT;

    প্রত্যাগম modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    পূর্ণাঙ্ক
extract_modifiers(পূর্ণাঙ্ক চাবি, পূর্ণাঙ্ক *modp, পূর্ণাঙ্ক simplify, পূর্ণাঙ্ক *did_simplify)
{
    পূর্ণাঙ্ক	modifiers = *modp;

#ifdef MACOS_X
    // Command-key really special, no fancynest
    যদি (!(modifiers & MOD_MASK_CMD))
#endif
    যদি ((modifiers & MOD_MASK_SHIFT) && ASCII_ISALPHA(চাবি))
    {
	চাবি = TOUPPER_ASC(চাবি);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	যদি (simplify || modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    modifiers &= ~MOD_MASK_SHIFT;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    যদি ((modifiers & MOD_MASK_CTRL) && ASCII_ISALPHA(চাবি))
	চাবি = TOUPPER_ASC(চাবি);

    যদি (simplify && (modifiers & MOD_MASK_CTRL)
	    && ((চাবি >= '?' && চাবি <= '_') || ASCII_ISALPHA(চাবি)))
    {
	চাবি = Ctrl_chr(চাবি);
	modifiers &= ~MOD_MASK_CTRL;
	// <C-@> is <Nul>
	যদি (চাবি == NUL)
	    চাবি = K_ZERO;
	যদি (did_simplify != রিক্ত)
	    *did_simplify = TRUE;
    }

#ifdef MACOS_X
    // Command-key really special, no fancynest
    যদি (!(modifiers & MOD_MASK_CMD))
#endif
    যদি (simplify && (modifiers & MOD_MASK_ALT) && চাবি < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	চাবি |= 0x80;
	modifiers &= ~MOD_MASK_ALT;	// remove the META modifier
	যদি (did_simplify != রিক্ত)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    প্রত্যাগম চাবি;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    পূর্ণাঙ্ক
find_special_চাবি_in_table(পূর্ণাঙ্ক c)
{
    পূর্ণাঙ্ক	    i;

    প্রতি (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(চাবি_names_table); i++)
	যদি (c == চাবি_names_table[i].চাবি && !চাবি_names_table[i].is_alt)
	    প্রত্যাগম চাবি_names_table[i].enabled ? i : -1;

    প্রত্যাগম -1;
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
    স্থির পূর্ণাঙ্ক
cmp_চাবি_name_entry(ধ্রুব শূন্য *a, ধ্রুব শূন্য *b)
{
    char_u  *p1 = ((সংরচনা চাবি_name_entry *)a)->name.string;
    char_u  *p2 = ((সংরচনা চাবি_name_entry *)b)->name.string;
    পূর্ণাঙ্ক	    result = 0;

    যদি (p1 == p2)
	প্রত্যাগম 0;

    যাবত্ (vim_isNormalIDc(*p1) && *p2 != NUL)
    {
	যদি ((result = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    ভঙ্গ;
	++p1;
	++p2;
    }

    যদি (result == 0)
    {
	যদি (*p2 == NUL)
	{
	    যদি (vim_isNormalIDc(*p1))
		result = 1;
	}
	অন্যথা
	{
	    result = -1;
	}
    }

    প্রত্যাগম result;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    পূর্ণাঙ্ক
get_special_চাবি_code(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    যদি (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	যদি (add_termcap_entry(string, FALSE) == OK)
	    প্রত্যাগম TERMCAP2_চাবি(name[2], name[3]);
    }
    অন্যথা
    {
	সংরচনা চাবি_name_entry	target;
	সংরচনা চাবি_name_entry	*entry;

	target.enabled = TRUE;
	target.চাবি = 0;
	target.name.string = name;
	target.name.length = 0;

	entry = (সংরচনা চাবি_name_entry *)bsearch(
	    &target,
	    &চাবি_names_table,
	    ARRAY_LENGTH(চাবি_names_table),
	    পরিমাণ(চাবি_names_table[0]),
	    cmp_চাবি_name_entry);
	যদি (entry != রিক্ত && entry->enabled)
	{
	    পূর্ণাঙ্ক চাবি = entry->চাবি;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    প্রত্যাগম চাবি == K_TAB ? TAB : চাবি;
	}
    }

    প্রত্যাগম 0;
}

    char_u *
get_চাবি_name(পূর্ণাঙ্ক i)
{
    যদি (i < 0 || i >= (পূর্ণাঙ্ক)ARRAY_LENGTH(চাবি_names_table))
	প্রত্যাগম রিক্ত;

    প্রত্যাগম চাবি_names_table[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    পূর্ণাঙ্ক
get_fileformat(buf_T *buf)
{
    পূর্ণাঙ্ক		c = *buf->b_p_ff;

    যদি (buf->b_p_bin || c == 'u')
	প্রত্যাগম EOL_UNIX;
    যদি (c == 'm')
	প্রত্যাগম EOL_MAC;
    প্রত্যাগম EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    পূর্ণাঙ্ক
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    পূর্ণাঙ্ক		c;

    যদি (eap != রিক্ত && eap->force_ff != 0)
	c = eap->force_ff;
    অন্যথা
    {
	যদি ((eap != রিক্ত && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    প্রত্যাগম EOL_UNIX;
	c = *buf->b_p_ff;
    }
    যদি (c == 'u')
	প্রত্যাগম EOL_UNIX;
    যদি (c == 'm')
	প্রত্যাগম EOL_MAC;
    প্রত্যাগম EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    শূন্য
set_fileformat(
    পূর্ণাঙ্ক		t,
    পূর্ণাঙ্ক		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    বর্ণ	*p = রিক্ত;

    বিকল্প (t)
    {
    পক্ষ EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	ভঙ্গ;
    পক্ষ EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	ভঙ্গ;
    পক্ষ EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	ভঙ্গ;
    }
    যদি (p != রিক্ত)
	set_string_option_direct((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_status(curbuf);
    redraw_tabline = TRUE;
#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    পূর্ণাঙ্ক
default_fileformat(শূন্য)
{
    বিকল্প (*p_ffs)
    {
	পক্ষ 'm':   প্রত্যাগম EOL_MAC;
	পক্ষ 'd':   প্রত্যাগম EOL_DOS;
    }
    প্রত্যাগম EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    পূর্ণাঙ্ক
call_shell(char_u *cmd, পূর্ণাঙ্ক opt)
{
    পূর্ণাঙ্ক		retval;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif

    যদি (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == রিক্ত ? p_sh : cmd);
	বার্তা_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
	prof_child_enter(&wait_time);
#endif

    যদি (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_empty));
	retval = -1;
    }
    অন্যথা
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

	যদি (cmd == রিক্ত || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	অন্যথা
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    যদি (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		যদি (ecmd == রিক্ত)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    যদি (ncmd != রিক্ত)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((বর্ণ *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    অন্যথা
		retval = -1;
	    যদি (ecmd != cmd)
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
    যদি (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif

    প্রত্যাগম retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    পূর্ণাঙ্ক
get_real_state(শূন্য)
{
    যদি (State & MODE_NORMAL)
    {
	যদি (VIsual_active)
	{
	    যদি (VIsual_select)
		প্রত্যাগম MODE_SELECT;
	    প্রত্যাগম MODE_VISUAL;
	}
	অন্যথা যদি (finish_op)
	    প্রত্যাগম MODE_OP_PENDING;
    }
    প্রত্যাগম State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    পূর্ণাঙ্ক
after_pathsep(char_u *b, char_u *p)
{
    প্রত্যাগম p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_head_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    পূর্ণাঙ্ক
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    যদি (f1 == রিক্ত || f2 == রিক্ত)
	প্রত্যাগম FALSE;

    (শূন্য)vim_FullName(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    প্রত্যাগম (t1 - ffname == t2 - f2
	     && pathcmp((বর্ণ *)ffname, (বর্ণ *)f2, (পূর্ণাঙ্ক)(t1 - ffname)) == 0);
}

#যদি defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    পূর্ণাঙ্ক
vim_chdirfile(char_u *fname, বর্ণ *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    যদি (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    যদি (pathcmp((বর্ণ *)old_dir, (বর্ণ *)new_dir, -1) == 0)
	// nothing to do
	প্রত্যাগম OK;

    যদি (trigger_autocmd != রিক্ত)
	trigger_DirChangedPre((char_u *)trigger_autocmd, new_dir);

    যদি (mch_chdir((বর্ণ *)new_dir) != 0)
	প্রত্যাগম FAIL;

    যদি (trigger_autocmd != রিক্ত)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    প্রত্যাগম OK;
}
#endif

#যদি defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    স্থির পূর্ণাঙ্ক
illegal_slash(ধ্রুব বর্ণ *name)
{
    যদি (name[0] == NUL)
	প্রত্যাগম FALSE;	    // no file name is not illegal
    যদি (name[strlen(name) - 1] != '/')
	প্রত্যাগম FALSE;	    // no trailing slash
    যদি (mch_isdir((char_u *)name))
	প্রত্যাগম FALSE;	    // trailing slash for a directory
    প্রত্যাগম TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    পূর্ণাঙ্ক
vim_stat(ধ্রুব বর্ণ *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    প্রত্যাগম illegal_slash(name) ? -1 : stat(name, stp);
}
#endif

#যদি defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_table[SHAPE_IDX_গুনতি] =
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
স্থির string_T mshape_names[] =
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
    {রিক্ত, 0}
};

#  define MSHAPE_NAMES_গুনতি  (ARRAY_LENGTH(mshape_names) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    বর্ণ *
parse_shape_opt(পূর্ণাঙ্ক what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    পূর্ণাঙ্ক		idx = 0;		// init for GCC
    পূর্ণাঙ্ক		all_idx;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		i;
    long	n;
    পূর্ণাঙ্ক		found_ve = FALSE;	// found "ve" flag
    পূর্ণাঙ্ক		round;

    /*
     * First round: check for errors; second round: do it for real.
     */
    প্রতি (round = 1; round <= 2; ++round)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	যদি (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	অন্যথা
# endif
	    modep = p_guicursor;
	যাবত্ (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    যদি (colonp == রিক্ত || (commap != রিক্ত && commap < colonp))
		প্রত্যাগম e_missing_colon_2;
	    যদি (colonp == modep)
		প্রত্যাগম e_illegal_mode;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    যাবত্ (modep < colonp || all_idx >= 0)
	    {
		যদি (all_idx < 0)
		{
		    // Find the mode.
		    যদি (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    অন্যথা
			len = 2;
		    যদি (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_গুনতি - 1;
		    অন্যথা
		    {
			প্রতি (idx = 0; idx < SHAPE_IDX_গুনতি; ++idx)
			    যদি (STRNICMP(modep, shape_table[idx].name, len)
									 == 0)
				ভঙ্গ;
			যদি (idx == SHAPE_IDX_গুনতি
				   || (shape_table[idx].used_for & what) == 0)
			    প্রত্যাগম e_illegal_mode;
			যদি (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    found_ve = TRUE;
		    }
		    modep += len + 1;
		}

		যদি (all_idx >= 0)
		    idx = all_idx--;
		অন্যথা যদি (round == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    যদি (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_table[idx].mshape = 0;
		    }
		    অন্যথা
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
		প্রতি (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    যদি (what == SHAPE_MOUSE)
		    {
			প্রতি (i = 0; ; ++i)
			{
			    যদি (mshape_names[i].string == রিক্ত)
			    {
				যদি (!VIM_ISDIGIT(*p))
				    প্রত্যাগম e_illegal_mouseshape;
				যদি (round == 2)
				    shape_table[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				অন্যথা
				    (শূন্য)getdigits(&p);
				ভঙ্গ;
			    }
			    যদি (STRNICMP(p, mshape_names[i].string, mshape_names[i].length) == 0)
			    {
				যদি (round == 2)
				    shape_table[idx].mshape = i;
				p += mshape_names[i].length;
				ভঙ্গ;
			    }
			}
		    }
		    অন্যথা // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			যদি (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			অন্যথা যদি (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			অন্যথা যদি (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			অন্যথা যদি (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			অন্যথা যদি (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			যদি (len != 0)
			{
			    p += len;
			    যদি (!VIM_ISDIGIT(*p))
				প্রত্যাগম e_digit_expected;
			    n = getdigits(&p);
			    যদি (len == 3)   // "ver" or "hor"
			    {
				যদি (n == 0)
				    প্রত্যাগম e_illegal_percentage;
				যদি (round == 2)
				{
				    যদি (TOLOWER_ASC(i) == 'v')
					shape_table[idx].shape = SHAPE_VER;
				    অন্যথা
					shape_table[idx].shape = SHAPE_HOR;
				    shape_table[idx].percentage = n;
				}
			    }
			    অন্যথা যদি (round == 2)
			    {
				যদি (len == 9)
				    shape_table[idx].blinkwait = n;
				অন্যথা যদি (len == 7)
				    shape_table[idx].blinkon = n;
				অন্যথা
				    shape_table[idx].blinkoff = n;
			    }
			}
			অন্যথা যদি (STRNICMP(p, "block", 5) == 0)
			{
			    যদি (round == 2)
				shape_table[idx].shape = SHAPE_BLOCK;
			    p += 5;
			}
			অন্যথা	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    যদি (commap == রিক্ত)		    // last part
			    {
				যদি (endp == রিক্ত)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    অন্যথা যদি (endp > commap || endp == রিক্ত)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    যদি (slashp != রিক্ত && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_group(p, (পূর্ণাঙ্ক)(slashp - p));
				p = slashp + 1;
			    }
			    যদি (round == 2)
			    {
				shape_table[idx].id = syn_check_group(p,
							     (পূর্ণাঙ্ক)(endp - p));
				shape_table[idx].id_lm = shape_table[idx].id;
				যদি (slashp != রিক্ত && slashp < endp)
				    shape_table[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    যদি (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    যদি (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    যদি (!found_ve)
    {
# ifdef FEAT_MOUSESHAPE
	যদি (what == SHAPE_MOUSE)
	{
	    shape_table[SHAPE_IDX_VE].mshape = shape_table[SHAPE_IDX_V].mshape;
	}
	অন্যথা
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

    প্রত্যাগম রিক্ত;
}

# যদি defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    পূর্ণাঙ্ক
get_shape_idx(পূর্ণাঙ্ক mouse)
{
#  ifdef FEAT_MOUSESHAPE
    যদি (mouse && (State == MODE_HITRETURN || State == MODE_ASKMORE))
    {
#   ifdef FEAT_GUI
	পূর্ণাঙ্ক x, y;
	gui_mch_getmouse(&x, &y);
	যদি (Y_2_ROW(y) == Rows - 1)
	    প্রত্যাগম SHAPE_IDX_MOREL;
#   endif
	প্রত্যাগম SHAPE_IDX_MORE;
    }
    যদি (mouse && drag_status_সারি)
	প্রত্যাগম SHAPE_IDX_SDRAG;
    যদি (mouse && drag_sep_সারি)
	প্রত্যাগম SHAPE_IDX_VDRAG;
#  endif
    যদি (!mouse && State == MODE_SHOWMATCH)
	প্রত্যাগম SHAPE_IDX_SM;
    যদি (State & VREPLACE_FLAG)
	প্রত্যাগম SHAPE_IDX_R;
    যদি (State & REPLACE_FLAG)
	প্রত্যাগম SHAPE_IDX_R;
    যদি (State & MODE_ঢোকাও)
	প্রত্যাগম SHAPE_IDX_I;
    যদি (State & MODE_CMDLINE)
    {
	যদি (cmdline_at_end())
	    প্রত্যাগম SHAPE_IDX_C;
	যদি (cmdline_overstrike())
	    প্রত্যাগম SHAPE_IDX_CR;
	প্রত্যাগম SHAPE_IDX_CI;
    }
    যদি (finish_op)
	প্রত্যাগম SHAPE_IDX_O;
    যদি (VIsual_active)
    {
	যদি (*p_sel == 'e')
	    প্রত্যাগম SHAPE_IDX_VE;
	অন্যথা
	    প্রত্যাগম SHAPE_IDX_V;
    }
    প্রত্যাগম SHAPE_IDX_N;
}
# endif

# যদি defined(FEAT_MOUSESHAPE)
স্থির পূর্ণাঙ্ক current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    শূন্য
update_mouseshape(পূর্ণাঙ্ক shape_idx)
{
    পূর্ণাঙ্ক new_mouse_shape;

    // Only works in GUI mode.
    যদি (!gui.in_use || gui.starting)
	প্রত্যাগম;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    যদি (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	প্রত্যাগম;
    }

    // When ignoring the mouse don't change shape on the statusline.
    যদি (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_STATUS
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    যদি (shape_idx == -2
	    && current_mouse_shape != shape_table[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_STATUS].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_VSEP].mshape)
	প্রত্যাগম;
    যদি (shape_idx < 0)
	new_mouse_shape = shape_table[get_shape_idx(TRUE)].mshape;
    অন্যথা
	new_mouse_shape = shape_table[shape_idx].mshape;
    যদি (new_mouse_shape != current_mouse_shape)
    {
	mch_set_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#endif // CURSOR_SHAPE

#যদি defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    শূন্য
f_getmouseshape(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;
# যদি defined(FEAT_MOUSESHAPE)
    যদি (current_mouse_shape >= 0
			      && current_mouse_shape < (পূর্ণাঙ্ক)MSHAPE_NAMES_গুনতি)
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
    পূর্ণাঙ্ক
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    পূর্ণাঙ্ক		r;
    char_u	*file_to_find = রিক্ত;
    বর্ণ	*অনুসন্ধান_ctx = রিক্ত;

    dir_name = find_directory_in_path(new_dir, (পূর্ণাঙ্ক)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_to_find, &অনুসন্ধান_ctx);
    vim_free(file_to_find);
    vim_findfile_cleanup(অনুসন্ধান_ctx);
    যদি (dir_name == রিক্ত)
	প্রত্যাগম -1;
    r = mch_chdir((বর্ণ *)dir_name);
    vim_free(dir_name);
    প্রত্যাগম r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
get_user_name(char_u *buf, পূর্ণাঙ্ক len)
{
    যদি (username == রিক্ত)
    {
	যদি (mch_get_user_name(buf, len) == FAIL)
	    প্রত্যাগম FAIL;
	username = vim_strsave(buf);
    }
    অন্যথা
	vim_strncpy(buf, username, len - 1);
    প্রত্যাগম OK;
}

#যদি defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    শূন্য
free_username(শূন্য)
{
    vim_free(username);
}
#endif

#যদি !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    শূন্য
qsort(
    শূন্য	*base,
    size_t	elm_গুনতি,
    size_t	elm_size,
    পূর্ণাঙ্ক (*cmp)(ধ্রুব শূন্য *, ধ্রুব শূন্য *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    পূর্ণাঙ্ক		i, j;
    পূর্ণাঙ্ক		gap;

    buf = alloc(elm_size);
    যদি (buf == রিক্ত)
	প্রত্যাগম;

    প্রতি (gap = elm_গুনতি / 2; gap > 0; gap /= 2)
	প্রতি (i = gap; i < elm_গুনতি; ++i)
	    প্রতি (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)base + j * elm_size;
		p2 = (char_u *)base + (j + gap) * elm_size;
		যদি ((*cmp)((শূন্য *)p1, (শূন্য *)p2) <= 0)
		    ভঙ্গ;
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

#যদি !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

স্থির পূর্ণাঙ্ক  envsize = -1;	// current size of environment
বাহ্য বর্ণ **environ;		// the global which is your env.

স্থির পূর্ণাঙ্ক  findenv(বর্ণ *name); // look for a name in the env.
স্থির পূর্ণাঙ্ক  newenv(শূন্য);	// copy env. from stack to heap
স্থির পূর্ণাঙ্ক  moreenv(শূন্য);	// incr. size of env.

    পূর্ণাঙ্ক
putenv(ধ্রুব বর্ণ *string)
{
    পূর্ণাঙ্ক	    i;
    বর্ণ    *p;

    যদি (envsize < 0)
    {				// first time putenv called
	যদি (newenv() < 0)	// copy env. to heap
	    প্রত্যাগম -1;
    }

    i = findenv((বর্ণ *)string); // look for name in environment

    যদি (i < 0)
    {				// name must be added
	প্রতি (i = 0; environ[i]; i++);
	যদি (i >= (envsize - 1))
	{			// need new slot
	    যদি (moreenv() < 0)
		প্রত্যাগম -1;
	}
	p = alloc(strlen(string) + 1);
	যদি (p == রিক্ত)		// not enough core
	    প্রত্যাগম -1;
	environ[i + 1] = 0;	// new end of env.
    }
    অন্যথা
    {				// name already in env.
	p = vim_realloc(environ[i], strlen(string) + 1);
	যদি (p == রিক্ত)
	    প্রত্যাগম -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
findenv(বর্ণ *name)
{
    বর্ণ    *namechar, *envchar;
    পূর্ণাঙ্ক	    i, found;

    found = 0;
    প্রতি (i = 0; environ[i] && !found; i++)
    {
	envchar = environ[i];
	namechar = name;
	যাবত্ (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	found = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    প্রত্যাগম found ? i - 1 : -1;
}

    স্থির পূর্ণাঙ্ক
newenv(শূন্য)
{
    বর্ণ    **env, *elem;
    পূর্ণাঙ্ক	    i, esize;

    প্রতি (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(বর্ণ *, esize);
    যদি (env == রিক্ত)
	প্রত্যাগম -1;

    প্রতি (i = 0; environ[i]; i++)
    {
	elem = alloc(strlen(environ[i]) + 1);
	যদি (elem == রিক্ত)
	    প্রত্যাগম -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
moreenv(শূন্য)
{
    পূর্ণাঙ্ক	    esize;
    বর্ণ    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((বর্ণ *)environ, esize * পরিমাণ (*env));
    যদি (env == 0)
	প্রত্যাগম -1;
    environ = env;
    envsize = esize;
    প্রত্যাগম 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(ধ্রুব char_u *string)
{
    পূর্ণাঙ্ক i;
    char_u *p;

    যদি (envsize < 0)
	প্রত্যাগম রিক্ত;

    i = findenv((বর্ণ *)string);

    যদি (i < 0)
	প্রত্যাগম রিক্ত;

    p = vim_strchr((char_u *)environ[i], '=');
    প্রত্যাগম (p + 1);
}
# endif

#endif // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#যদি defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    পূর্ণাঙ্ক
filewritable(char_u *fname)
{
    পূর্ণাঙ্ক		retval = 0;
# যদি defined(UNIX) || defined(VMS)
    পূর্ণাঙ্ক		perm = 0;
# endif

# যদি defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    যদি (
# ifdef MSWIN
	    mch_writable(fname) &&
# অন্যথা
#  যদি defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((বর্ণ *)fname, W_OK) == 0
       )
    {
	++retval;
	যদি (mch_isdir(fname))
	    ++retval;
    }
    প্রত্যাগম retval;
}
#endif

#যদি defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    পূর্ণাঙ্ক
get2c(FILE *fd)
{
    পূর্ণাঙ্ক		c, n;

    n = getc(fd);
    যদি (n == EOF) প্রত্যাগম -1;
    c = getc(fd);
    যদি (c == EOF) প্রত্যাগম -1;
    প্রত্যাগম (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    পূর্ণাঙ্ক
get3c(FILE *fd)
{
    পূর্ণাঙ্ক		c, n;

    n = getc(fd);
    যদি (n == EOF) প্রত্যাগম -1;
    c = getc(fd);
    যদি (c == EOF) প্রত্যাগম -1;
    n = (n << 8) + c;
    c = getc(fd);
    যদি (c == EOF) প্রত্যাগম -1;
    প্রত্যাগম (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    পূর্ণাঙ্ক
get4c(FILE *fd)
{
    পূর্ণাঙ্ক		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    unsigned	n;

    c = getc(fd);
    যদি (c == EOF) প্রত্যাগম -1;
    n = (unsigned)c;
    c = getc(fd);
    যদি (c == EOF) প্রত্যাগম -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    যদি (c == EOF) প্রত্যাগম -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    যদি (c == EOF) প্রত্যাগম -1;
    n = (n << 8) + (unsigned)c;
    প্রত্যাগম (পূর্ণাঙ্ক)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, পূর্ণাঙ্ক cnt)
{
    char_u	*str;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c;

    // allocate memory
    str = alloc(cnt + 1);
    যদি (str == রিক্ত)
	প্রত্যাগম রিক্ত;

    // Read the string.  Quit when running into the EOF.
    প্রতি (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	যদি (c == EOF)
	{
	    vim_free(str);
	    প্রত্যাগম রিক্ত;
	}
	str[i] = c;
    }
    str[i] = NUL;
    প্রত্যাগম str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    পূর্ণাঙ্ক
put_bytes(FILE *fd, long_u nr, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক	    i;

    প্রতি (i = len - 1; i >= 0; --i)
	যদি (putc((পূর্ণাঙ্ক)(nr >> (i * 8)), fd) == EOF)
	    প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}

#endif

#ifndef PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    long
elapsed(সংরচনা timeval *start_tv)
{
    সংরচনা timeval  now_tv;

    gettimeofday(&now_tv, রিক্ত);
    প্রত্যাগম (now_tv.tv_sec - start_tv->tv_sec) * 1000L
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
    DWORD	now = Get_Tick_গুনতি();

    প্রত্যাগম (long)now - (long)start_tick;
}
# endif
#endif

#যদি defined(FEAT_JOB_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    পূর্ণাঙ্ক
mch_parse_cmd(char_u *cmd, পূর্ণাঙ্ক use_shcf, বর্ণ ***argv, পূর্ণাঙ্ক *argc)
{
    পূর্ণাঙ্ক		i;
    char_u	*p, *d;
    পূর্ণাঙ্ক		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    প্রতি (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	যাবত্ (*p != NUL)
	{
	    যদি (i == 2)
		(*argv)[*argc] = (বর্ণ *)p;
	    ++*argc;
	    d = p;
	    যাবত্ (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		যদি (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		অন্যথা
		{
		    যদি (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    যদি (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    যদি (*p == NUL)
	    {
		যদি (i == 2)
		    *d++ = NUL;
		ভঙ্গ;
	    }
	    যদি (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	যদি (*argv == রিক্ত)
	{
	    যদি (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		প্রতি (;;)
		{
		    p = skiptowhite(p);
		    যদি (*p == NUL)
			ভঙ্গ;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(বর্ণ *, *argc + 4);
	    যদি (*argv == রিক্ত)	    // out of memory
		প্রত্যাগম FAIL;
	}
    }
    প্রত্যাগম OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    পূর্ণাঙ্ক
build_argv_from_string(char_u *cmd, বর্ণ ***argv, পূর্ণাঙ্ক *argc)
{
    char_u	*cmd_copy;
    পূর্ণাঙ্ক		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    যদি (cmd_copy == রিক্ত
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	প্রত্যাগম FAIL;
    }
    প্রতি (i = 0; i < *argc; i++)
	(*argv)[i] = (বর্ণ *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = রিক্ত;
    vim_free(cmd_copy);
    প্রত্যাগম OK;
}

#endif

#যদি defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    পূর্ণাঙ্ক
build_argv_from_list(list_T *l, বর্ণ ***argv, পূর্ণাঙ্ক *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(বর্ণ *, l->lv_len + 1);
    যদি (*argv == রিক্ত)
	প্রত্যাগম FAIL;
    *argc = 0;
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	যদি (s == রিক্ত)
	{
	    পূর্ণাঙ্ক i;

	    প্রতি (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = রিক্ত;
	    প্রত্যাগম FAIL;
	}
	(*argv)[*argc] = (বর্ণ *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = রিক্ত;
    প্রত্যাগম OK;
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
    পূর্ণাঙ্ক
get_special_pty_type(শূন্য)
{
#ifdef MSWIN
    প্রত্যাগম get_conpty_type();
#অন্যথা
    প্রত্যাগম 0;
#endif
}

// compare two keyvalue_T structs by case sensitive value
    পূর্ণাঙ্ক
cmp_keyvalue_মান(ধ্রুব শূন্য *a, ধ্রুব শূন্য *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    প্রত্যাগম STRCMP(kv1->মান.string, kv2->মান.string);
}

// compare two keyvalue_T structs by value with length
    পূর্ণাঙ্ক
cmp_keyvalue_মান_n(ধ্রুব শূন্য *a, ধ্রুব শূন্য *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    প্রত্যাগম STRNCMP(kv1->মান.string, kv2->মান.string, MAX(kv1->মান.length,
		kv2->মান.length));
}

// compare two keyvalue_T structs by case insensitive value
    পূর্ণাঙ্ক
cmp_keyvalue_মান_i(ধ্রুব শূন্য *a, ধ্রুব শূন্য *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    প্রত্যাগম STRICMP(kv1->মান.string, kv2->মান.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    পূর্ণাঙ্ক
cmp_keyvalue_মান_ni(ধ্রুব শূন্য *a, ধ্রুব শূন্য *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    প্রত্যাগম vim_strnicmp_asc((বর্ণ *)kv1->মান.string,
	    (বর্ণ *)kv2->মান.string, MAX(kv1->মান.length,
		    kv2->মান.length));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    শূন্য *
mergesort_list(
    শূন্য	*head,
    শূন্য	*(*get_next)(শূন্য *),
    শূন্য	(*set_next)(শূন্য *, শূন্য *),
    শূন্য	*(*get_prev)(শূন্য *),
    শূন্য	(*set_prev)(শূন্য *, শূন্য *),
    পূর্ণাঙ্ক		(*compare)(ধ্রুব শূন্য *, ধ্রুব শূন্য *))
{
    যদি (!head || !get_next(head))
	প্রত্যাগম head;

    // Count length
    পূর্ণাঙ্ক	    n = 0;
    শূন্য*   curr = head;
    যাবত্ (curr)
    {
	n++;
	curr = get_next(curr);
    }

    পূর্ণাঙ্ক	size;
    প্রতি (size = 1; size < n; size *= 2)
    {
	শূন্য*	new_head = রিক্ত;
	শূন্য*	tail = রিক্ত;
	curr = head;

	যাবত্ (curr)
	{
	    // Split two runs
	    শূন্য    *বাম = curr;
	    শূন্য    *ডান = বাম;
	    পূর্ণাঙ্ক	    i;
	    প্রতি (i = 0; i < size && ডান; ++i)
		ডান = get_next(ডান);

	    শূন্য    *next = ডান;
	    প্রতি (i = 0; i < size && next; ++i)
		next = get_next(next);

	    // Break links
	    শূন্য    *l_end = ডান ? get_prev(ডান) : রিক্ত;
	    যদি (l_end)
		set_next(l_end, রিক্ত);
	    যদি (ডান)
		set_prev(ডান, রিক্ত);

	    শূন্য    *r_end = next ? get_prev(next) : রিক্ত;
	    যদি (r_end)
		set_next(r_end, রিক্ত);
	    যদি (next)
		set_prev(next, রিক্ত);

	    // Merge
	    শূন্য    *merged = রিক্ত;
	    শূন্য    *merged_tail = রিক্ত;

	    যাবত্ (বাম || ডান)
	    {
		শূন্য	*chosen = রিক্ত;
		যদি (!বাম)
		{
		    chosen = ডান;
		    ডান = get_next(ডান);
		}
		অন্যথা যদি (!ডান)
		{
		    chosen = বাম;
		    বাম = get_next(বাম);
		}
		অন্যথা যদি (compare(বাম, ডান) <= 0)
		{
		    chosen = বাম;
		    বাম = get_next(বাম);
		}
		অন্যথা
		{
		    chosen = ডান;
		    ডান = get_next(ডান);
		}

		যদি (merged_tail)
		{
		    set_next(merged_tail, chosen);
		    set_prev(chosen, merged_tail);
		    merged_tail = chosen;
		}
		অন্যথা
		{
		    merged = merged_tail = chosen;
		    set_prev(chosen, রিক্ত);
		}
	    }

	    // Connect to full list
	    যদি (!new_head)
		new_head = merged;
	    অন্যথা
	    {
		set_next(tail, merged);
		set_prev(merged, tail);
	    }

	    // Move tail to end
	    যাবত্ (get_next(merged_tail))
		merged_tail = get_next(merged_tail);
	    tail = merged_tail;

	    curr = next;
	}

	head = new_head;
    }

    প্রত্যাগম head;
}

স্থির ধ্রুব char_u base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

স্থির char_u base64_dec_table[256];

#define BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    স্থির শূন্য
init_base64_dec_table(শূন্য)
{
    স্থির পূর্ণাঙ্ক base64_dec_tbl_initialized = FALSE;

    যদি (base64_dec_tbl_initialized)
	প্রত্যাগম;

    vim_memset(base64_dec_table, 0xFF, পরিমাণ(base64_dec_table));
    প্রতি (size_t i = 0; i < পরিমাণ(base64_table) - 1; i++)
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
base64_encode_buf(char_u *dst, ধ্রুব char_u *src, size_t len)
{
    size_t i, j;

    প্রতি (i = 0, j = 0; i < len;)
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
    প্রত্যাগম (long)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(ধ্রুব char_u *data, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    যদি (encoded == রিক্ত)
	প্রত্যাগম রিক্ত;
    base64_encode_buf(encoded, data, len);
    প্রত্যাগম encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    পূর্ণাঙ্ক
base64_decode(ধ্রুব char_u *data, size_t len, garray_T *out)
{
    যদি (len == 0)
	প্রত্যাগম OK;

    যদি (len % 4 != 0)
    {
	semsg(_(e_invalid_argument_str), data);
	প্রত্যাগম FAIL;
    }

    init_base64_dec_table();

    size_t decoded_len = (len / 4) * 3;
    যদি (data[len - 1] == '=')
	decoded_len--;
    যদি (data[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    প্রতি (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_table[(char_u)data[i++]];
	int_u sextet_b = base64_dec_table[(char_u)data[i++]];
	int_u sextet_c = base64_dec_table[(char_u)data[i++]];
	int_u sextet_d = base64_dec_table[(char_u)data[i++]];

	যদি (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_argument_str), data);
	    ga_clear(out);
	    প্রত্যাগম FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	যদি (j < decoded_len)
	{
	    ga_append(out, (triple >> 16) & 0xFF);
	    j++;
	}
	যদি (j < decoded_len)
	{
	    ga_append(out, (triple >> 8) & 0xFF);
	    j++;
	}
	যদি (j < decoded_len)
	{
	    ga_append(out, triple & 0xFF);
	    j++;
	}

	যদি (j == decoded_len)
	{
	    যদি ((data[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((data[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_argument_str), data);
		ga_clear(out);
		প্রত্যাগম FAIL;
	    }
	}
    }
    প্রত্যাগম OK;
}
