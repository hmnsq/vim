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
#अन्तर्भाव "vim.h"

स्थिर char_u	*username = रिक्त; // cached result of mch_get_user_name()

स्थिर पूर्णाङ्क coladvance2(pos_T *pos, पूर्णाङ्क addspaces, पूर्णाङ्क finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    पूर्णाङ्क
virtual_active(शून्य)
{
    unsigned पूर्णाङ्क cur_ve_flags = get_ve_flags();

    सचे (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_निवेशय) && (State & MODE_निवेशय)))
	पटिनिवत्त TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    सचे (virtual_op != MAYBE)
	पटिनिवत्त virtual_op;
    पटिनिवत्त (cur_ve_flags & VE_BLOCK) && VIsual_active && VIsual_mode == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    पूर्णाङ्क
getviscol(शून्य)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, रिक्त, रिक्त, GETVCOL_NO_ABOVE);
    पटिनिवत्त (पूर्णाङ्क)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    पूर्णाङ्क
coladvance_force(colnr_T wcol)
{
    पूर्णाङ्क rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    सचे (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    अञ्ञथा
	// Virtcol is valid
	set_valid_virtcol(curwin, wcol);
    पटिनिवत्त rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    पूर्णाङ्क
getviscol2(colnr_T col, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = col;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, रिक्त, रिक्त, GETVCOL_NO_ABOVE);
    पटिनिवत्त (पूर्णाङ्क)x;
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
    पूर्णाङ्क
coladvance(colnr_T wantcol)
{
    पूर्णाङ्क rc = getvpos(&curwin->w_cursor, wantcol);

    सचे (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    अञ्ञथा सचे (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	set_valid_virtcol(curwin, wantcol);
    पटिनिवत्त rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    पूर्णाङ्क
getvpos(pos_T *pos, colnr_T wantcol)
{
    पटिनिवत्त coladvance2(pos, FALSE, virtual_active(), wantcol);
}

    स्थिर पूर्णाङ्क
coladvance2(
    pos_T	*pos,
    पूर्णाङ्क		addspaces,	// change the text to achieve our goal?
    पूर्णाङ्क		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    पूर्णाङ्क		idx;
    char_u	*पङ्क्तिः;
    पूर्णाङ्क		linelen;
    colnr_T	col = 0;
    पूर्णाङ्क		csize = 0;
    पूर्णाङ्क		one_more;
#ifdef FEAT_LINEBREAK
    पूर्णाङ्क		head = 0;
#endif

    one_more = (State & MODE_निवेशय)
		    || restart_edit != NUL
		    || (VIsual_active && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    पङ्क्तिः = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    सचे (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_more;
	    col = wcol;

	    सचे ((addspaces || finetune) && !VIsual_active)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_more;
		सचे (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    अञ्ञथा
    {
	पूर्णाङ्क		width = curwin->w_width - win_col_off(curwin);
	chartabsize_T	cts;

	सचे (finetune
		&& curwin->w_p_wrap
		&& curwin->w_width != 0
		&& wcol >= (colnr_T)width
		&& width > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    सचे (csize > 0)
		csize--;

	    सचे (wcol / width > (colnr_T)csize / width
		    && ((State & MODE_निवेशय) == 0 || (पूर्णाङ्क)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / width + 1) * width - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, पङ्क्तिः, पङ्क्तिः);
#ifdef FEAT_PROP_POPUP
	cts.cts_no_above = true;
#endif
	याव (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ifdef FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &head, रिक्त);
	    MB_PTR_ADV(cts.cts_ptr);
#अञ्ञथा
	    csize = lbr_chartabsize_adv(&cts);
#endif
	    cts.cts_vcol += csize;
	}
	col = cts.cts_vcol;
	idx = (पूर्णाङ्क)(cts.cts_ptr - पङ्क्तिः);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	सचे (col > wcol || (!virtual_active() && one_more == 0))
	{
	    idx -= 1;
#ifdef FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= head;
#endif
	    col -= csize;
	}

	सचे (virtual_active()
		&& addspaces
		&& wcol >= 0
		&& ((col != wcol && col != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    सचे (पङ्क्तिः[idx] == NUL)
	    {
		// Append spaces
		पूर्णाङ्क	correct = wcol - col;
		char_u	*newline = alloc(idx + correct + 1);
		पूर्णाङ्क	t;

		सचे (newline == रिक्त)
		    पटिनिवत्त FAIL;

		अत्थाय (t = 0; t < idx; ++t)
		    newline[t] = पङ्क्तिः[t];

		अत्थाय (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, (colnr_T)idx);
		idx += correct;
		col = wcol;
	    }
	    अञ्ञथा
	    {
		// Break a tab
		पूर्णाङ्क	correct = wcol - col - csize + 1; // negative!!
		char_u	*newline;
		पूर्णाङ्क	t, s = 0;
		पूर्णाङ्क	v;

		सचे (-correct > csize)
		    पटिनिवत्त FAIL;

		newline = alloc(linelen + csize);
		सचे (newline == रिक्त)
		    पटिनिवत्त FAIL;

		अत्थाय (t = 0; t < linelen; t++)
		{
		    सचे (t != idx)
			newline[s++] = पङ्क्तिः[t];
		    अञ्ञथा
			अत्थाय (v = 0; v < csize; v++)
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

    सचे (idx < 0)
	pos->col = 0;
    अञ्ञथा
	pos->col = idx;

    pos->coladd = 0;

    सचे (finetune)
    {
	सचे (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    सचे (!one_more)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, रिक्त, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	अञ्ञथा
	{
	    पूर्णाङ्क b = (पूर्णाङ्क)wcol - (पूर्णाङ्क)col;

	    // The difference between wcol and col is used to set coladd.
	    सचे (b > 0 && b < (MAXCOL - 2 * curwin->w_width))
		pos->coladd = b;

	    col += b;
	}
    }

    // prevent from moving onto a trail byte
    सचे (has_mbyte)
	mb_adjustpos(curbuf, pos);

    सचे (wcol < 0 || col < wcol)
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    पूर्णाङ्क
inc_cursor(शून्य)
{
    पटिनिवत्त inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    पूर्णाङ्क
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    सचे (lp->col != MAXCOL)
    {
	p = ml_get_pos(lp);
	सचे (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    सचे (has_mbyte)
	    {
		पूर्णाङ्क l = (*mb_ptr2len)(p);

		lp->col += l;
		पटिनिवत्त ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->col++;
	    lp->coladd = 0;
	    पटिनिवत्त ((p[1] != NUL) ? 0 : 2);
	}
    }
    सचे (lp->lnum != curbuf->b_ml.ml_पङ्क्तिः_गणनम्)     // there is a next line
    {
	lp->col = 0;
	lp->lnum++;
	lp->coladd = 0;
	पटिनिवत्त 1;
    }
    पटिनिवत्त -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    पूर्णाङ्क
incl(pos_T *lp)
{
    पूर्णाङ्क	    r;

    सचे ((r = inc(lp)) >= 1 && lp->col)
	r = inc(lp);
    पटिनिवत्त r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    पूर्णाङ्क
dec_cursor(शून्य)
{
    पटिनिवत्त dec(&curwin->w_cursor);
}

    पूर्णाङ्क
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    सचे (lp->col == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	सचे (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	पटिनिवत्त 0;
    }

    सचे (lp->col > 0)
    {
	// still within line
	lp->col--;
	सचे (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	}
	पटिनिवत्त 0;
    }

    सचे (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	सचे (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	पटिनिवत्त 1;
    }

    // at start of file
    पटिनिवत्त -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    पूर्णाङ्क
decl(pos_T *lp)
{
    पूर्णाङ्क	    r;

    सचे ((r = dec(lp)) == 1 && lp->col)
	r = dec(lp);
    पटिनिवत्त r;
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
    सचे (hasAnyFolding(wp))
    {
	सचे (lnum > cursor)
	{
	    याव (lnum > cursor)
	    {
		(शून्य)hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		सचे (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	अञ्ञथा सचे (lnum < cursor)
	{
	    याव (lnum < cursor)
	    {
		(शून्य)hasFoldingWin(wp, lnum, रिक्त, &lnum, TRUE, रिक्त);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		सचे (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    अञ्ञथा
#endif
	retval = lnum - cursor;

    पटिनिवत्त retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    शून्य
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    सचे (pos->lnum > buf->b_ml.ml_पङ्क्तिः_गणनम्)
	pos->lnum = buf->b_ml.ml_पङ्क्तिः_गणनम्;

    सचे (pos->col > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	सचे (pos->col > len)
	    pos->col = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    शून्य
check_cursor_lnum(शून्य)
{
    सचे (curwin->w_cursor.lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
    {
#ifdef FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	सचे (!hasFolding(curbuf->b_ml.ml_पङ्क्तिः_गणनम्,
						&curwin->w_cursor.lnum, रिक्त))
#endif
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
    }
    सचे (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    शून्य
check_cursor_col(शून्य)
{
    check_cursor_col_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    शून्य
check_cursor_col_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.col;
    colnr_T      oldcoladd = win->w_cursor.col + win->w_cursor.coladd;
    unsigned पूर्णाङ्क cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_buffer, win->w_cursor.lnum);
    सचे (len == 0)
	win->w_cursor.col = 0;
    अञ्ञथा सचे (win->w_cursor.col >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	सचे ((State & MODE_निवेशय) || restart_edit
		|| (VIsual_active && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_active())
	    win->w_cursor.col = len;
	अञ्ञथा
	{
	    win->w_cursor.col = len - 1;
	    // Move the cursor to the head byte.
	    सचे (has_mbyte)
		mb_adjustpos(win->w_buffer, &win->w_cursor);
	}
    }
    अञ्ञथा सचे (win->w_cursor.col < 0)
	win->w_cursor.col = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    सचे (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    अञ्ञथा सचे (cur_ve_flags == VE_ALL)
    {
	सचे (oldcoladd > win->w_cursor.col)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.col;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    सचे (win->w_cursor.col + 1 < len)
	    {
		पूर्णाङ्क cs, ce;

		getvcol(win, &win->w_cursor, &cs, रिक्त, &ce, 0);
		सचे (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	अञ्ञथा
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    शून्य
check_cursor(शून्य)
{
    check_cursor_lnum();
    check_cursor_col();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    शून्य
check_visual_pos(शून्य)
{
    सचे (VIsual.lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	VIsual.lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	VIsual.col = 0;
	VIsual.coladd = 0;
    }
    अञ्ञथा
    {
	पूर्णाङ्क len = ml_get_len(VIsual.lnum);

	सचे (VIsual.col > len)
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
    शून्य
adjust_cursor_col(शून्य)
{
    सचे (curwin->w_cursor.col > 0
	    && (!VIsual_active || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    पूर्णाङ्क
set_leftcol(colnr_T leftcol)
{
    पूर्णाङ्क		retval = FALSE;

    // Return quickly when there is no change.
    सचे (curwin->w_leftcol == leftcol)
	पटिनिवत्त FALSE;
    curwin->w_leftcol = leftcol;

    changed_cline_bef_curs();
    long lastcol = curwin->w_leftcol + curwin->w_width - curwin_col_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    long siso = get_sidescrolloff_मानम्();
    सचे (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    अञ्ञथा सचे (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(शून्य)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, रिक्त, &e, 0);
    सचे (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    अञ्ञथा सचे (s < curwin->w_leftcol)
    {
	retval = TRUE;
	सचे (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    changed_cline_bef_curs();
	}
    }

    सचे (retval)
	curwin->w_set_curswant = true;
    redraw_later(UPD_NOT_VALID);
    पटिनिवत्त retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    पूर्णाङ्क
copy_option_part(
    char_u	**option,
    char_u	*buf,
    पूर्णाङ्क		maxlen,
    वर्ण	*sep_chars)
{
    पूर्णाङ्क	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    सचे (*p == '.')
	buf[len++] = *p++;
    याव (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == रिक्त)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	सचे (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != रिक्त)
	    ++p;
	सचे (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    सचे (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_to_option_part(p);	// p points to next file name

    *option = p;
    पटिनिवत्त len;
}

#सचे !defined(HAVE_MEMSET) && !defined(PROTO)
    शून्य *
vim_memset(शून्य *ptr, पूर्णाङ्क c, size_t size)
{
    वर्ण *p = ptr;

    याव (size-- > 0)
	*p++ = c;
    पटिनिवत्त ptr;
}
#endif

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    पूर्णाङ्क
vim_isspace(पूर्णाङ्क x)
{
    पटिनिवत्त ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

स्थिर संरचना modmasktable
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
#सचे defined(MACOS_X) || defined(FEAT_GUI_GTK)
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
#define MOD_कुञ्चिकाः_ENTRY_SIZE 5

स्थिर char_u modifier_कुञ्चिकाः_table[] =
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
    MOD_MASK_CTRL,  KS_EXTRA, (पूर्णाङ्क)KE_C_END,	'@', '7',	// end
    MOD_MASK_SHIFT, '*', '9',			'@', '9',	// exit
    MOD_MASK_SHIFT, '*', '0',			'@', '0',	// find
    MOD_MASK_SHIFT, '#', '1',			'%', '1',	// help
    MOD_MASK_SHIFT, '#', '2',			'k', 'h',	// home
    MOD_MASK_CTRL,  KS_EXTRA, (पूर्णाङ्क)KE_C_HOME,	'k', 'h',	// home
    MOD_MASK_SHIFT, '#', '3',			'k', 'I',	// insert
    MOD_MASK_SHIFT, '#', '4',			'k', 'l',	// left arrow
    MOD_MASK_CTRL,  KS_EXTRA, (पूर्णाङ्क)KE_C_वामः,	'k', 'l',	// left arrow
    MOD_MASK_SHIFT, '%', 'a',			'%', '3',	// message
    MOD_MASK_SHIFT, '%', 'b',			'%', '4',	// move
    MOD_MASK_SHIFT, '%', 'c',			'%', '5',	// next
    MOD_MASK_SHIFT, '%', 'd',			'%', '7',	// options
    MOD_MASK_SHIFT, '%', 'e',			'%', '8',	// previous
    MOD_MASK_SHIFT, '%', 'f',			'%', '9',	// print
    MOD_MASK_SHIFT, '%', 'g',			'%', '0',	// redo
    MOD_MASK_SHIFT, '%', 'h',			'&', '3',	// replace
    MOD_MASK_SHIFT, '%', 'i',			'k', 'r',	// right arr.
    MOD_MASK_CTRL,  KS_EXTRA, (पूर्णाङ्क)KE_C_दक्षिणः,	'k', 'r',	// right arr.
    MOD_MASK_SHIFT, '%', 'j',			'&', '5',	// resume
    MOD_MASK_SHIFT, '!', '1',			'&', '6',	// save
    MOD_MASK_SHIFT, '!', '2',			'&', '7',	// suspend
    MOD_MASK_SHIFT, '!', '3',			'&', '8',	// undo
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_UP,	'k', 'u',	// up arrow
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_DOWN,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_XF1,	KS_EXTRA, (पूर्णाङ्क)KE_XF1,
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_XF2,	KS_EXTRA, (पूर्णाङ्क)KE_XF2,
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_XF3,	KS_EXTRA, (पूर्णाङ्क)KE_XF3,
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_XF4,	KS_EXTRA, (पूर्णाङ्क)KE_XF4,

    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F1,	'k', '1',	// F1
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F2,	'k', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F3,	'k', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F4,	'k', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F5,	'k', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F6,	'k', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F7,	'k', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F8,	'k', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F9,	'k', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F10,	'k', ';',	// F10

    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F11,	'F', '1',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F12,	'F', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F13,	'F', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F14,	'F', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F15,	'F', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F16,	'F', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F17,	'F', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F18,	'F', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F19,	'F', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F20,	'F', 'A',

    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F21,	'F', 'B',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F22,	'F', 'C',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F23,	'F', 'D',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F24,	'F', 'E',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F25,	'F', 'F',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F26,	'F', 'G',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F27,	'F', 'H',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F28,	'F', 'I',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F29,	'F', 'J',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F30,	'F', 'K',

    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F31,	'F', 'L',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F32,	'F', 'M',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F33,	'F', 'N',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F34,	'F', 'O',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F35,	'F', 'P',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F36,	'F', 'Q',
    MOD_MASK_SHIFT, KS_EXTRA, (पूर्णाङ्क)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_MASK_SHIFT, 'k', 'B',			KS_EXTRA, (पूर्णाङ्क)KE_TAB,

    NUL
};

स्थिर संरचना कुञ्चिका_name_entry
{
    पूर्णाङ्क		enabled;	    // is this entry available (TRUE/FALSE)?
    पूर्णाङ्क		कुञ्चिका;		    // special key code or ascii value
    string_T	name;		    // name of key
    पूर्णाङ्क		is_alt;		    // is an alternative name
} कुञ्चिका_names_table[] =
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
#अञ्ञथा
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
#अञ्ञथा
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
    {TRUE, K_वामः, STR_LITERAL_INIT("Left"), FALSE},
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
#अञ्ञथा
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
#अञ्ञथा
    FALSE,
#endif
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_दक्षिणः, STR_LITERAL_INIT("Right"), FALSE},
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
#अञ्ञथा
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
#अञ्ञथा
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
    स्थिर पूर्णाङ्क
name_to_mod_mask(पूर्णाङ्क c)
{
    पूर्णाङ्क	    i;

    c = TOUPPER_ASC(c);
    अत्थाय (i = 0; mod_mask_table[i].mod_mask != 0; i++)
	सचे (c == mod_mask_table[i].name)
	    पटिनिवत्त mod_mask_table[i].mod_flag;
    पटिनिवत्त 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    पूर्णाङ्क
simplify_कुञ्चिका(पूर्णाङ्क कुञ्चिका, पूर्णाङ्क *modifiers)
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    key0;
    पूर्णाङ्क	    key1;

    सचे (!(*modifiers & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	पटिनिवत्त कुञ्चिका;

    // TAB is a special case
    सचे (कुञ्चिका == TAB && (*modifiers & MOD_MASK_SHIFT))
    {
	*modifiers &= ~MOD_MASK_SHIFT;
	पटिनिवत्त K_S_TAB;
    }
    key0 = KEY2TERMCAP0(कुञ्चिका);
    key1 = KEY2TERMCAP1(कुञ्चिका);
    अत्थाय (i = 0; modifier_कुञ्चिकाः_table[i] != NUL; i += MOD_कुञ्चिकाः_ENTRY_SIZE)
    {
	सचे (key0 == modifier_कुञ्चिकाः_table[i + 3]
		&& key1 == modifier_कुञ्चिकाः_table[i + 4]
		&& (*modifiers & modifier_कुञ्चिकाः_table[i]))
	{
	    *modifiers &= ~modifier_कुञ्चिकाः_table[i];
	    पटिनिवत्त TERMCAP2_कुञ्चिका(modifier_कुञ्चिकाः_table[i + 1],
		    modifier_कुञ्चिकाः_table[i + 2]);
	}
    }
    पटिनिवत्त कुञ्चिका;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    पूर्णाङ्क
handle_x_कुञ्चिकाः(पूर्णाङ्क कुञ्चिका)
{
    विकल्प (कुञ्चिका)
    {
	पक्ष K_XUP:	पटिनिवत्त K_UP;
	पक्ष K_XDOWN:	पटिनिवत्त K_DOWN;
	पक्ष K_XLEFT:	पटिनिवत्त K_वामः;
	पक्ष K_XRIGHT:	पटिनिवत्त K_दक्षिणः;
	पक्ष K_XHOME:	पटिनिवत्त K_HOME;
	पक्ष K_ZHOME:	पटिनिवत्त K_HOME;
	पक्ष K_XEND:	पटिनिवत्त K_END;
	पक्ष K_ZEND:	पटिनिवत्त K_END;
	पक्ष K_XF1:	पटिनिवत्त K_F1;
	पक्ष K_XF2:	पटिनिवत्त K_F2;
	पक्ष K_XF3:	पटिनिवत्त K_F3;
	पक्ष K_XF4:	पटिनिवत्त K_F4;
	पक्ष K_S_XF1:	पटिनिवत्त K_S_F1;
	पक्ष K_S_XF2:	पटिनिवत्त K_S_F2;
	पक्ष K_S_XF3:	पटिनिवत्त K_S_F3;
	पक्ष K_S_XF4:	पटिनिवत्त K_S_F4;
    }
    पटिनिवत्त कुञ्चिका;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_कुञ्चिका_name(पूर्णाङ्क c, पूर्णाङ्क modifiers)
{
    स्थिर char_u string[MAX_कुञ्चिका_NAME_LEN + 1];
    पूर्णाङ्क	    i, idx, len;
    पूर्णाङ्क	    table_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    सचे (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_कुञ्चिका)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    सचे (IS_SPECIAL(c))
    {
	अत्थाय (i = 0; modifier_कुञ्चिकाः_table[i] != 0; i += MOD_कुञ्चिकाः_ENTRY_SIZE)
	    सचे (       KEY2TERMCAP0(c) == (पूर्णाङ्क)modifier_कुञ्चिकाः_table[i + 1]
		    && (पूर्णाङ्क)KEY2TERMCAP1(c) == (पूर्णाङ्क)modifier_कुञ्चिकाः_table[i + 2])
	    {
		modifiers |= modifier_कुञ्चिकाः_table[i];
		c = TERMCAP2_कुञ्चिका(modifier_कुञ्चिकाः_table[i + 3],
						   modifier_कुञ्चिकाः_table[i + 4]);
		भङ्ग;
	    }
    }

    // try to find the key in the special key table
    table_idx = find_special_कुञ्चिका_in_table(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    सचे (c > 0 && (*mb_char2len)(c) == 1)
    {
	सचे (table_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_MASK_ALT;
	    // try again, to find the un-alted key in the special key table
	    table_idx = find_special_कुञ्चिका_in_table(c);
	}
	सचे (table_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_MASK_CTRL;
	}
    }

    // translate the modifier into a string
    अत्थाय (i = 0; mod_mask_table[i].name != 'A'; i++)
	सचे ((modifiers & mod_mask_table[i].mod_mask)
						== mod_mask_table[i].mod_flag)
	{
	    string[idx++] = mod_mask_table[i].name;
	    string[idx++] = (char_u)'-';
	}

    सचे (table_idx < 0)		// unknown special key, may output t_xx
    {
	सचे (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	अञ्ञथा
	{
	    len = (*mb_char2len)(c);
	    सचे (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    अञ्ञथा सचे (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    अञ्ञथा
	    {
		char_u	*s = transchar(c);
		याव (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    अञ्ञथा		// use name of special key
    {
	string_T    *s;

	s = &कुञ्चिका_names_table[table_idx].name;

	सचे (s->length + idx + 2 <= MAX_कुञ्चिका_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (पूर्णाङ्क)s->length;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    पटिनिवत्त string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    पूर्णाङ्क
trans_special(
    char_u	**srcp,
    char_u	*dst,
    पूर्णाङ्क		flags,		// FSK_ values
    पूर्णाङ्क		escape_ks,	// escape K_SPECIAL bytes in the character
    पूर्णाङ्क		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    पूर्णाङ्क		modifiers = 0;
    पूर्णाङ्क		कुञ्चिका;

    कुञ्चिका = find_special_कुञ्चिका(srcp, &modifiers, flags, did_simplify);
    सचे (कुञ्चिका == 0)
	पटिनिवत्त 0;

    पटिनिवत्त special_to_buf(कुञ्चिका, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    पूर्णाङ्क
special_to_buf(पूर्णाङ्क कुञ्चिका, पूर्णाङ्क modifiers, पूर्णाङ्क escape_ks, char_u *dst)
{
    पूर्णाङ्क		dlen = 0;

    // Put the appropriate modifier in a string
    सचे (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    सचे (IS_SPECIAL(कुञ्चिका))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(कुञ्चिका);
	dst[dlen++] = KEY2TERMCAP1(कुञ्चिका);
    }
    अञ्ञथा सचे (escape_ks)
	dlen = (पूर्णाङ्क)(add_char2buf(कुञ्चिका, dst + dlen) - dst);
    अञ्ञथा सचे (has_mbyte)
	dlen += (*mb_char2bytes)(कुञ्चिका, dst + dlen);
    अञ्ञथा
	dst[dlen++] = कुञ्चिका;

    पटिनिवत्त dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    पूर्णाङ्क
find_special_कुञ्चिका(
    char_u	**srcp,
    पूर्णाङ्क		*modp,
    पूर्णाङ्क		flags,		// FSK_ values
    पूर्णाङ्क		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*last_dash;
    char_u	*end_of_name;
    char_u	*src;
    char_u	*bp;
    पूर्णाङ्क		in_string = flags & FSK_IN_STRING;
    पूर्णाङ्क		modifiers;
    पूर्णाङ्क		bit;
    पूर्णाङ्क		कुञ्चिका;
    uvarnumber_T	n;
    पूर्णाङ्क		l;

    src = *srcp;
    सचे (src[0] != '<')
	पटिनिवत्त 0;
    सचे (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    last_dash = src;
    अत्थाय (bp = src + 1; *bp == '-' || vim_isNormalIDc(*bp); bp++)
    {
	सचे (*bp == '-')
	{
	    last_dash = bp;
	    सचे (bp[1] != NUL)
	    {
		सचे (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		अञ्ञथा
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		सचे (!(in_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		अञ्ञथा सचे (in_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	सचे (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	अञ्ञथा सचे (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, रिक्त, &l, STR2NR_ALL, रिक्त, रिक्त, 0, TRUE, रिक्त);
	    सचे (l == 0)
	    {
		emsg(_(e_invalid_argument));
		पटिनिवत्त 0;
	    }
	    bp += l + 5;
	    भङ्ग;
	}
    }

    सचे (*bp == '>')	// found matching '>'
    {
	end_of_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	अत्थाय (bp = src + 1; bp < last_dash; bp++)
	{
	    सचे (*bp != '-')
	    {
		bit = name_to_mod_mask(*bp);
		सचे (bit == 0x0)
		    भङ्ग;	// Illegal modifier name
		modifiers |= bit;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	सचे (bp >= last_dash)
	{
	    सचे (STRNICMP(last_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(last_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(last_dash + 6, रिक्त, &l, STR2NR_ALL, रिक्त,
							    &n, 0, TRUE, रिक्त);
		सचे (l == 0)
		{
		    emsg(_(e_invalid_argument));
		    पटिनिवत्त 0;
		}
		कुञ्चिका = (पूर्णाङ्क)n;
	    }
	    अञ्ञथा
	    {
		पूर्णाङ्क off = 1;

		// Modifier with single letter, or special key name.
		सचे (in_string && last_dash[1] == '\\' && last_dash[2] == '"')
		    off = 2;
		सचे (has_mbyte)
		    l = mb_ptr2len(last_dash + off);
		अञ्ञथा
		    l = 1;
		सचे (modifiers != 0 && last_dash[l + off] == '>')
		    कुञ्चिका = PTR2CHAR(last_dash + off);
		अञ्ञथा
		{
		    कुञ्चिका = get_special_कुञ्चिका_code(last_dash + off);
		    सचे (!(flags & FSK_KEEP_X_कुञ्चिका))
			कुञ्चिका = handle_x_कुञ्चिकाः(कुञ्चिका);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    सचे (कुञ्चिका != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		कुञ्चिका = simplify_कुञ्चिका(कुञ्चिका, &modifiers);

		सचे ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    सचे (कुञ्चिका == K_BS)
			कुञ्चिका = BS;
		    अञ्ञथा सचे (कुञ्चिका == K_DEL || कुञ्चिका == K_KDEL)
			कुञ्चिका = DEL;
		}
		अञ्ञथा सचे (कुञ्चिका == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    सचे ((flags & FSK_SIMPLIFY) != 0)
		    {
			सचे (did_simplify != रिक्त)
			    *did_simplify = TRUE;
		    }
		    अञ्ञथा
			कुञ्चिका = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		सचे (!IS_SPECIAL(कुञ्चिका))
		    कुञ्चिका = extract_modifiers(कुञ्चिका, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = end_of_name;
		पटिनिवत्त कुञ्चिका;
	    }
	}
    }
    पटिनिवत्त 0;
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
    पूर्णाङ्क
may_adjust_कुञ्चिका_for_ctrl(पूर्णाङ्क modifiers, पूर्णाङ्क कुञ्चिका)
{
    सचे ((modifiers & MOD_MASK_CTRL) == 0)
	पटिनिवत्त कुञ्चिका;

    सचे (ASCII_ISALPHA(कुञ्चिका))
    {
#ifdef FEAT_TERMINAL
	check_no_reduce_कुञ्चिकाः();  // may update the no_reduce_keys flag
#endif
	पटिनिवत्त no_reduce_कुञ्चिकाः == 0 ? TOUPPER_ASC(कुञ्चिका) : कुञ्चिका;
    }
    सचे (कुञ्चिका == '2')
	पटिनिवत्त '@';
    सचे (कुञ्चिका == '6')
	पटिनिवत्त '^';
    सचे (कुञ्चिका == '-')
	पटिनिवत्त '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    सचे (कुञ्चिका == '$' && (modifiers & MOD_MASK_SHIFT) == 0)
	पटिनिवत्त ']';

    पटिनिवत्त कुञ्चिका;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    पूर्णाङ्क
may_remove_shift_modifier(पूर्णाङ्क modifiers, पूर्णाङ्क कुञ्चिका)
{
    सचे ((modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
#ifdef FEAT_GUI_GTK
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_CMD)
#endif
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    && ((कुञ्चिका >= '!' && कुञ्चिका <= '/')
		|| (कुञ्चिका >= ':' && कुञ्चिका <= 'Z')
		|| vim_isdigit(कुञ्चिका)
		|| (कुञ्चिका >= '[' && कुञ्चिका <= '`')
		|| (कुञ्चिका >= '{' && कुञ्चिका <= '~')))
	पटिनिवत्त modifiers & ~MOD_MASK_SHIFT;

    सचे (modifiers == (MOD_MASK_SHIFT | MOD_MASK_CTRL)
		&& (कुञ्चिका == '{' || कुञ्चिका == '}' || कुञ्चिका == '|'))
	पटिनिवत्त modifiers & ~MOD_MASK_SHIFT;

    पटिनिवत्त modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    पूर्णाङ्क
extract_modifiers(पूर्णाङ्क कुञ्चिका, पूर्णाङ्क *modp, पूर्णाङ्क simplify, पूर्णाङ्क *did_simplify)
{
    पूर्णाङ्क	modifiers = *modp;

#ifdef MACOS_X
    // Command-key really special, no fancynest
    सचे (!(modifiers & MOD_MASK_CMD))
#endif
    सचे ((modifiers & MOD_MASK_SHIFT) && ASCII_ISALPHA(कुञ्चिका))
    {
	कुञ्चिका = TOUPPER_ASC(कुञ्चिका);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	सचे (simplify || modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    modifiers &= ~MOD_MASK_SHIFT;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    सचे ((modifiers & MOD_MASK_CTRL) && ASCII_ISALPHA(कुञ्चिका))
	कुञ्चिका = TOUPPER_ASC(कुञ्चिका);

    सचे (simplify && (modifiers & MOD_MASK_CTRL)
	    && ((कुञ्चिका >= '?' && कुञ्चिका <= '_') || ASCII_ISALPHA(कुञ्चिका)))
    {
	कुञ्चिका = Ctrl_chr(कुञ्चिका);
	modifiers &= ~MOD_MASK_CTRL;
	// <C-@> is <Nul>
	सचे (कुञ्चिका == NUL)
	    कुञ्चिका = K_ZERO;
	सचे (did_simplify != रिक्त)
	    *did_simplify = TRUE;
    }

#ifdef MACOS_X
    // Command-key really special, no fancynest
    सचे (!(modifiers & MOD_MASK_CMD))
#endif
    सचे (simplify && (modifiers & MOD_MASK_ALT) && कुञ्चिका < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	कुञ्चिका |= 0x80;
	modifiers &= ~MOD_MASK_ALT;	// remove the META modifier
	सचे (did_simplify != रिक्त)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    पटिनिवत्त कुञ्चिका;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    पूर्णाङ्क
find_special_कुञ्चिका_in_table(पूर्णाङ्क c)
{
    पूर्णाङ्क	    i;

    अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(कुञ्चिका_names_table); i++)
	सचे (c == कुञ्चिका_names_table[i].कुञ्चिका && !कुञ्चिका_names_table[i].is_alt)
	    पटिनिवत्त कुञ्चिका_names_table[i].enabled ? i : -1;

    पटिनिवत्त -1;
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
    स्थिर पूर्णाङ्क
cmp_कुञ्चिका_name_entry(ध्रुव शून्य *a, ध्रुव शून्य *b)
{
    char_u  *p1 = ((संरचना कुञ्चिका_name_entry *)a)->name.string;
    char_u  *p2 = ((संरचना कुञ्चिका_name_entry *)b)->name.string;
    पूर्णाङ्क	    result = 0;

    सचे (p1 == p2)
	पटिनिवत्त 0;

    याव (vim_isNormalIDc(*p1) && *p2 != NUL)
    {
	सचे ((result = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    भङ्ग;
	++p1;
	++p2;
    }

    सचे (result == 0)
    {
	सचे (*p2 == NUL)
	{
	    सचे (vim_isNormalIDc(*p1))
		result = 1;
	}
	अञ्ञथा
	{
	    result = -1;
	}
    }

    पटिनिवत्त result;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    पूर्णाङ्क
get_special_कुञ्चिका_code(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    सचे (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	सचे (add_termcap_entry(string, FALSE) == OK)
	    पटिनिवत्त TERMCAP2_कुञ्चिका(name[2], name[3]);
    }
    अञ्ञथा
    {
	संरचना कुञ्चिका_name_entry	target;
	संरचना कुञ्चिका_name_entry	*entry;

	target.enabled = TRUE;
	target.कुञ्चिका = 0;
	target.name.string = name;
	target.name.length = 0;

	entry = (संरचना कुञ्चिका_name_entry *)bsearch(
	    &target,
	    &कुञ्चिका_names_table,
	    ARRAY_LENGTH(कुञ्चिका_names_table),
	    परिमाण(कुञ्चिका_names_table[0]),
	    cmp_कुञ्चिका_name_entry);
	सचे (entry != रिक्त && entry->enabled)
	{
	    पूर्णाङ्क कुञ्चिका = entry->कुञ्चिका;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    पटिनिवत्त कुञ्चिका == K_TAB ? TAB : कुञ्चिका;
	}
    }

    पटिनिवत्त 0;
}

    char_u *
get_कुञ्चिका_name(पूर्णाङ्क i)
{
    सचे (i < 0 || i >= (पूर्णाङ्क)ARRAY_LENGTH(कुञ्चिका_names_table))
	पटिनिवत्त रिक्त;

    पटिनिवत्त कुञ्चिका_names_table[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    पूर्णाङ्क
get_fileformat(buf_T *buf)
{
    पूर्णाङ्क		c = *buf->b_p_ff;

    सचे (buf->b_p_bin || c == 'u')
	पटिनिवत्त EOL_UNIX;
    सचे (c == 'm')
	पटिनिवत्त EOL_MAC;
    पटिनिवत्त EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    पूर्णाङ्क
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    पूर्णाङ्क		c;

    सचे (eap != रिक्त && eap->force_ff != 0)
	c = eap->force_ff;
    अञ्ञथा
    {
	सचे ((eap != रिक्त && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    पटिनिवत्त EOL_UNIX;
	c = *buf->b_p_ff;
    }
    सचे (c == 'u')
	पटिनिवत्त EOL_UNIX;
    सचे (c == 'm')
	पटिनिवत्त EOL_MAC;
    पटिनिवत्त EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    शून्य
set_fileformat(
    पूर्णाङ्क		t,
    पूर्णाङ्क		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    वर्ण	*p = रिक्त;

    विकल्प (t)
    {
    पक्ष EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	भङ्ग;
    पक्ष EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	भङ्ग;
    पक्ष EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	भङ्ग;
    }
    सचे (p != रिक्त)
	set_string_option_direct((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_status(curbuf);
    redraw_tabline = TRUE;
#सचे defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    पूर्णाङ्क
default_fileformat(शून्य)
{
    विकल्प (*p_ffs)
    {
	पक्ष 'm':   पटिनिवत्त EOL_MAC;
	पक्ष 'd':   पटिनिवत्त EOL_DOS;
    }
    पटिनिवत्त EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    पूर्णाङ्क
call_shell(char_u *cmd, पूर्णाङ्क opt)
{
    पूर्णाङ्क		retval;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif

    सचे (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == रिक्त ? p_sh : cmd);
	सन्देशः_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ifdef FEAT_PROFILE
    सचे (do_profiling == PROF_YES)
	prof_child_enter(&wait_time);
#endif

    सचे (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_empty));
	retval = -1;
    }
    अञ्ञथा
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

	सचे (cmd == रिक्त || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	अञ्ञथा
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    सचे (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		सचे (ecmd == रिक्त)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    सचे (ncmd != रिक्त)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((वर्ण *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    अञ्ञथा
		retval = -1;
	    सचे (ecmd != cmd)
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
    सचे (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif

    पटिनिवत्त retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    पूर्णाङ्क
get_real_state(शून्य)
{
    सचे (State & MODE_NORMAL)
    {
	सचे (VIsual_active)
	{
	    सचे (VIsual_select)
		पटिनिवत्त MODE_SELECT;
	    पटिनिवत्त MODE_VISUAL;
	}
	अञ्ञथा सचे (finish_op)
	    पटिनिवत्त MODE_OP_PENDING;
    }
    पटिनिवत्त State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    पूर्णाङ्क
after_pathsep(char_u *b, char_u *p)
{
    पटिनिवत्त p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_head_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    पूर्णाङ्क
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    सचे (f1 == रिक्त || f2 == रिक्त)
	पटिनिवत्त FALSE;

    (शून्य)vim_FullName(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    पटिनिवत्त (t1 - ffname == t2 - f2
	     && pathcmp((वर्ण *)ffname, (वर्ण *)f2, (पूर्णाङ्क)(t1 - ffname)) == 0);
}

#सचे defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    पूर्णाङ्क
vim_chdirfile(char_u *fname, वर्ण *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    सचे (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    सचे (pathcmp((वर्ण *)old_dir, (वर्ण *)new_dir, -1) == 0)
	// nothing to do
	पटिनिवत्त OK;

    सचे (trigger_autocmd != रिक्त)
	trigger_DirChangedPre((char_u *)trigger_autocmd, new_dir);

    सचे (mch_chdir((वर्ण *)new_dir) != 0)
	पटिनिवत्त FAIL;

    सचे (trigger_autocmd != रिक्त)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    पटिनिवत्त OK;
}
#endif

#सचे defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    स्थिर पूर्णाङ्क
illegal_slash(ध्रुव वर्ण *name)
{
    सचे (name[0] == NUL)
	पटिनिवत्त FALSE;	    // no file name is not illegal
    सचे (name[strlen(name) - 1] != '/')
	पटिनिवत्त FALSE;	    // no trailing slash
    सचे (mch_isdir((char_u *)name))
	पटिनिवत्त FALSE;	    // trailing slash for a directory
    पटिनिवत्त TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    पूर्णाङ्क
vim_stat(ध्रुव वर्ण *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    पटिनिवत्त illegal_slash(name) ? -1 : stat(name, stp);
}
#endif

#सचे defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_table[SHAPE_IDX_गणनम्] =
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
स्थिर string_T mshape_names[] =
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
    {रिक्त, 0}
};

#  define MSHAPE_NAMES_गणनम्  (ARRAY_LENGTH(mshape_names) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    वर्ण *
parse_shape_opt(पूर्णाङ्क what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    पूर्णाङ्क		idx = 0;		// init for GCC
    पूर्णाङ्क		all_idx;
    पूर्णाङ्क		len;
    पूर्णाङ्क		i;
    long	n;
    पूर्णाङ्क		found_ve = FALSE;	// found "ve" flag
    पूर्णाङ्क		round;

    /*
     * First round: check for errors; second round: do it for real.
     */
    अत्थाय (round = 1; round <= 2; ++round)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	सचे (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	अञ्ञथा
# endif
	    modep = p_guicursor;
	याव (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    सचे (colonp == रिक्त || (commap != रिक्त && commap < colonp))
		पटिनिवत्त e_missing_colon_2;
	    सचे (colonp == modep)
		पटिनिवत्त e_illegal_mode;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    याव (modep < colonp || all_idx >= 0)
	    {
		सचे (all_idx < 0)
		{
		    // Find the mode.
		    सचे (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    अञ्ञथा
			len = 2;
		    सचे (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_गणनम् - 1;
		    अञ्ञथा
		    {
			अत्थाय (idx = 0; idx < SHAPE_IDX_गणनम्; ++idx)
			    सचे (STRNICMP(modep, shape_table[idx].name, len)
									 == 0)
				भङ्ग;
			सचे (idx == SHAPE_IDX_गणनम्
				   || (shape_table[idx].used_for & what) == 0)
			    पटिनिवत्त e_illegal_mode;
			सचे (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    found_ve = TRUE;
		    }
		    modep += len + 1;
		}

		सचे (all_idx >= 0)
		    idx = all_idx--;
		अञ्ञथा सचे (round == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    सचे (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_table[idx].mshape = 0;
		    }
		    अञ्ञथा
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
		अत्थाय (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    सचे (what == SHAPE_MOUSE)
		    {
			अत्थाय (i = 0; ; ++i)
			{
			    सचे (mshape_names[i].string == रिक्त)
			    {
				सचे (!VIM_ISDIGIT(*p))
				    पटिनिवत्त e_illegal_mouseshape;
				सचे (round == 2)
				    shape_table[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				अञ्ञथा
				    (शून्य)getdigits(&p);
				भङ्ग;
			    }
			    सचे (STRNICMP(p, mshape_names[i].string, mshape_names[i].length) == 0)
			    {
				सचे (round == 2)
				    shape_table[idx].mshape = i;
				p += mshape_names[i].length;
				भङ्ग;
			    }
			}
		    }
		    अञ्ञथा // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			सचे (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			अञ्ञथा सचे (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			अञ्ञथा सचे (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			अञ्ञथा सचे (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			अञ्ञथा सचे (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			सचे (len != 0)
			{
			    p += len;
			    सचे (!VIM_ISDIGIT(*p))
				पटिनिवत्त e_digit_expected;
			    n = getdigits(&p);
			    सचे (len == 3)   // "ver" or "hor"
			    {
				सचे (n == 0)
				    पटिनिवत्त e_illegal_percentage;
				सचे (round == 2)
				{
				    सचे (TOLOWER_ASC(i) == 'v')
					shape_table[idx].shape = SHAPE_VER;
				    अञ्ञथा
					shape_table[idx].shape = SHAPE_HOR;
				    shape_table[idx].percentage = n;
				}
			    }
			    अञ्ञथा सचे (round == 2)
			    {
				सचे (len == 9)
				    shape_table[idx].blinkwait = n;
				अञ्ञथा सचे (len == 7)
				    shape_table[idx].blinkon = n;
				अञ्ञथा
				    shape_table[idx].blinkoff = n;
			    }
			}
			अञ्ञथा सचे (STRNICMP(p, "block", 5) == 0)
			{
			    सचे (round == 2)
				shape_table[idx].shape = SHAPE_BLOCK;
			    p += 5;
			}
			अञ्ञथा	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    सचे (commap == रिक्त)		    // last part
			    {
				सचे (endp == रिक्त)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    अञ्ञथा सचे (endp > commap || endp == रिक्त)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    सचे (slashp != रिक्त && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_group(p, (पूर्णाङ्क)(slashp - p));
				p = slashp + 1;
			    }
			    सचे (round == 2)
			    {
				shape_table[idx].id = syn_check_group(p,
							     (पूर्णाङ्क)(endp - p));
				shape_table[idx].id_lm = shape_table[idx].id;
				सचे (slashp != रिक्त && slashp < endp)
				    shape_table[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    सचे (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    सचे (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    सचे (!found_ve)
    {
# ifdef FEAT_MOUSESHAPE
	सचे (what == SHAPE_MOUSE)
	{
	    shape_table[SHAPE_IDX_VE].mshape = shape_table[SHAPE_IDX_V].mshape;
	}
	अञ्ञथा
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

    पटिनिवत्त रिक्त;
}

# सचे defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    पूर्णाङ्क
get_shape_idx(पूर्णाङ्क mouse)
{
#  ifdef FEAT_MOUSESHAPE
    सचे (mouse && (State == MODE_HITRETURN || State == MODE_ASKMORE))
    {
#   ifdef FEAT_GUI
	पूर्णाङ्क x, y;
	gui_mch_getmouse(&x, &y);
	सचे (Y_2_ROW(y) == Rows - 1)
	    पटिनिवत्त SHAPE_IDX_MOREL;
#   endif
	पटिनिवत्त SHAPE_IDX_MORE;
    }
    सचे (mouse && drag_status_पङ्क्तिः)
	पटिनिवत्त SHAPE_IDX_SDRAG;
    सचे (mouse && drag_sep_पङ्क्तिः)
	पटिनिवत्त SHAPE_IDX_VDRAG;
#  endif
    सचे (!mouse && State == MODE_SHOWMATCH)
	पटिनिवत्त SHAPE_IDX_SM;
    सचे (State & VREPLACE_FLAG)
	पटिनिवत्त SHAPE_IDX_R;
    सचे (State & REPLACE_FLAG)
	पटिनिवत्त SHAPE_IDX_R;
    सचे (State & MODE_निवेशय)
	पटिनिवत्त SHAPE_IDX_I;
    सचे (State & MODE_CMDLINE)
    {
	सचे (cmdline_at_end())
	    पटिनिवत्त SHAPE_IDX_C;
	सचे (cmdline_overstrike())
	    पटिनिवत्त SHAPE_IDX_CR;
	पटिनिवत्त SHAPE_IDX_CI;
    }
    सचे (finish_op)
	पटिनिवत्त SHAPE_IDX_O;
    सचे (VIsual_active)
    {
	सचे (*p_sel == 'e')
	    पटिनिवत्त SHAPE_IDX_VE;
	अञ्ञथा
	    पटिनिवत्त SHAPE_IDX_V;
    }
    पटिनिवत्त SHAPE_IDX_N;
}
# endif

# सचे defined(FEAT_MOUSESHAPE)
स्थिर पूर्णाङ्क current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    शून्य
update_mouseshape(पूर्णाङ्क shape_idx)
{
    पूर्णाङ्क new_mouse_shape;

    // Only works in GUI mode.
    सचे (!gui.in_use || gui.starting)
	पटिनिवत्त;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    सचे (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	पटिनिवत्त;
    }

    // When ignoring the mouse don't change shape on the statusline.
    सचे (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_STATUS
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    सचे (shape_idx == -2
	    && current_mouse_shape != shape_table[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_STATUS].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_VSEP].mshape)
	पटिनिवत्त;
    सचे (shape_idx < 0)
	new_mouse_shape = shape_table[get_shape_idx(TRUE)].mshape;
    अञ्ञथा
	new_mouse_shape = shape_table[shape_idx].mshape;
    सचे (new_mouse_shape != current_mouse_shape)
    {
	mch_set_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#endif // CURSOR_SHAPE

#सचे defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    शून्य
f_getmouseshape(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
# सचे defined(FEAT_MOUSESHAPE)
    सचे (current_mouse_shape >= 0
			      && current_mouse_shape < (पूर्णाङ्क)MSHAPE_NAMES_गणनम्)
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
    पूर्णाङ्क
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    पूर्णाङ्क		r;
    char_u	*file_to_find = रिक्त;
    वर्ण	*अन्वेषणम्_ctx = रिक्त;

    dir_name = find_directory_in_path(new_dir, (पूर्णाङ्क)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_to_find, &अन्वेषणम्_ctx);
    vim_free(file_to_find);
    vim_findfile_cleanup(अन्वेषणम्_ctx);
    सचे (dir_name == रिक्त)
	पटिनिवत्त -1;
    r = mch_chdir((वर्ण *)dir_name);
    vim_free(dir_name);
    पटिनिवत्त r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
get_user_name(char_u *buf, पूर्णाङ्क len)
{
    सचे (username == रिक्त)
    {
	सचे (mch_get_user_name(buf, len) == FAIL)
	    पटिनिवत्त FAIL;
	username = vim_strsave(buf);
    }
    अञ्ञथा
	vim_strncpy(buf, username, len - 1);
    पटिनिवत्त OK;
}

#सचे defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    शून्य
free_username(शून्य)
{
    vim_free(username);
}
#endif

#सचे !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    शून्य
qsort(
    शून्य	*base,
    size_t	elm_गणनम्,
    size_t	elm_size,
    पूर्णाङ्क (*cmp)(ध्रुव शून्य *, ध्रुव शून्य *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    पूर्णाङ्क		i, j;
    पूर्णाङ्क		gap;

    buf = alloc(elm_size);
    सचे (buf == रिक्त)
	पटिनिवत्त;

    अत्थाय (gap = elm_गणनम् / 2; gap > 0; gap /= 2)
	अत्थाय (i = gap; i < elm_गणनम्; ++i)
	    अत्थाय (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)base + j * elm_size;
		p2 = (char_u *)base + (j + gap) * elm_size;
		सचे ((*cmp)((शून्य *)p1, (शून्य *)p2) <= 0)
		    भङ्ग;
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

#सचे !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

स्थिर पूर्णाङ्क  envsize = -1;	// current size of environment
बाह्य वर्ण **environ;		// the global which is your env.

स्थिर पूर्णाङ्क  findenv(वर्ण *name); // look for a name in the env.
स्थिर पूर्णाङ्क  newenv(शून्य);	// copy env. from stack to heap
स्थिर पूर्णाङ्क  moreenv(शून्य);	// incr. size of env.

    पूर्णाङ्क
putenv(ध्रुव वर्ण *string)
{
    पूर्णाङ्क	    i;
    वर्ण    *p;

    सचे (envsize < 0)
    {				// first time putenv called
	सचे (newenv() < 0)	// copy env. to heap
	    पटिनिवत्त -1;
    }

    i = findenv((वर्ण *)string); // look for name in environment

    सचे (i < 0)
    {				// name must be added
	अत्थाय (i = 0; environ[i]; i++);
	सचे (i >= (envsize - 1))
	{			// need new slot
	    सचे (moreenv() < 0)
		पटिनिवत्त -1;
	}
	p = alloc(strlen(string) + 1);
	सचे (p == रिक्त)		// not enough core
	    पटिनिवत्त -1;
	environ[i + 1] = 0;	// new end of env.
    }
    अञ्ञथा
    {				// name already in env.
	p = vim_realloc(environ[i], strlen(string) + 1);
	सचे (p == रिक्त)
	    पटिनिवत्त -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
findenv(वर्ण *name)
{
    वर्ण    *namechar, *envchar;
    पूर्णाङ्क	    i, found;

    found = 0;
    अत्थाय (i = 0; environ[i] && !found; i++)
    {
	envchar = environ[i];
	namechar = name;
	याव (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	found = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    पटिनिवत्त found ? i - 1 : -1;
}

    स्थिर पूर्णाङ्क
newenv(शून्य)
{
    वर्ण    **env, *elem;
    पूर्णाङ्क	    i, esize;

    अत्थाय (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(वर्ण *, esize);
    सचे (env == रिक्त)
	पटिनिवत्त -1;

    अत्थाय (i = 0; environ[i]; i++)
    {
	elem = alloc(strlen(environ[i]) + 1);
	सचे (elem == रिक्त)
	    पटिनिवत्त -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    पटिनिवत्त 0;
}

    स्थिर पूर्णाङ्क
moreenv(शून्य)
{
    पूर्णाङ्क	    esize;
    वर्ण    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((वर्ण *)environ, esize * परिमाण (*env));
    सचे (env == 0)
	पटिनिवत्त -1;
    environ = env;
    envsize = esize;
    पटिनिवत्त 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(ध्रुव char_u *string)
{
    पूर्णाङ्क i;
    char_u *p;

    सचे (envsize < 0)
	पटिनिवत्त रिक्त;

    i = findenv((वर्ण *)string);

    सचे (i < 0)
	पटिनिवत्त रिक्त;

    p = vim_strchr((char_u *)environ[i], '=');
    पटिनिवत्त (p + 1);
}
# endif

#endif // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#सचे defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    पूर्णाङ्क
filewritable(char_u *fname)
{
    पूर्णाङ्क		retval = 0;
# सचे defined(UNIX) || defined(VMS)
    पूर्णाङ्क		perm = 0;
# endif

# सचे defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    सचे (
# ifdef MSWIN
	    mch_writable(fname) &&
# अञ्ञथा
#  सचे defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((वर्ण *)fname, W_OK) == 0
       )
    {
	++retval;
	सचे (mch_isdir(fname))
	    ++retval;
    }
    पटिनिवत्त retval;
}
#endif

#सचे defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    पूर्णाङ्क
get2c(FILE *fd)
{
    पूर्णाङ्क		c, n;

    n = getc(fd);
    सचे (n == EOF) पटिनिवत्त -1;
    c = getc(fd);
    सचे (c == EOF) पटिनिवत्त -1;
    पटिनिवत्त (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    पूर्णाङ्क
get3c(FILE *fd)
{
    पूर्णाङ्क		c, n;

    n = getc(fd);
    सचे (n == EOF) पटिनिवत्त -1;
    c = getc(fd);
    सचे (c == EOF) पटिनिवत्त -1;
    n = (n << 8) + c;
    c = getc(fd);
    सचे (c == EOF) पटिनिवत्त -1;
    पटिनिवत्त (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    पूर्णाङ्क
get4c(FILE *fd)
{
    पूर्णाङ्क		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    unsigned	n;

    c = getc(fd);
    सचे (c == EOF) पटिनिवत्त -1;
    n = (unsigned)c;
    c = getc(fd);
    सचे (c == EOF) पटिनिवत्त -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    सचे (c == EOF) पटिनिवत्त -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    सचे (c == EOF) पटिनिवत्त -1;
    n = (n << 8) + (unsigned)c;
    पटिनिवत्त (पूर्णाङ्क)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, पूर्णाङ्क cnt)
{
    char_u	*str;
    पूर्णाङ्क		i;
    पूर्णाङ्क		c;

    // allocate memory
    str = alloc(cnt + 1);
    सचे (str == रिक्त)
	पटिनिवत्त रिक्त;

    // Read the string.  Quit when running into the EOF.
    अत्थाय (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	सचे (c == EOF)
	{
	    vim_free(str);
	    पटिनिवत्त रिक्त;
	}
	str[i] = c;
    }
    str[i] = NUL;
    पटिनिवत्त str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    पूर्णाङ्क
put_bytes(FILE *fd, long_u nr, पूर्णाङ्क len)
{
    पूर्णाङ्क	    i;

    अत्थाय (i = len - 1; i >= 0; --i)
	सचे (putc((पूर्णाङ्क)(nr >> (i * 8)), fd) == EOF)
	    पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

#endif

#ifndef PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    long
elapsed(संरचना timeval *start_tv)
{
    संरचना timeval  now_tv;

    gettimeofday(&now_tv, रिक्त);
    पटिनिवत्त (now_tv.tv_sec - start_tv->tv_sec) * 1000L
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
    DWORD	now = Get_Tick_गणनम्();

    पटिनिवत्त (long)now - (long)start_tick;
}
# endif
#endif

#सचे defined(FEAT_JOB_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    पूर्णाङ्क
mch_parse_cmd(char_u *cmd, पूर्णाङ्क use_shcf, वर्ण ***argv, पूर्णाङ्क *argc)
{
    पूर्णाङ्क		i;
    char_u	*p, *d;
    पूर्णाङ्क		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    अत्थाय (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	याव (*p != NUL)
	{
	    सचे (i == 2)
		(*argv)[*argc] = (वर्ण *)p;
	    ++*argc;
	    d = p;
	    याव (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		सचे (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		अञ्ञथा
		{
		    सचे (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    सचे (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    सचे (*p == NUL)
	    {
		सचे (i == 2)
		    *d++ = NUL;
		भङ्ग;
	    }
	    सचे (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	सचे (*argv == रिक्त)
	{
	    सचे (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		अत्थाय (;;)
		{
		    p = skiptowhite(p);
		    सचे (*p == NUL)
			भङ्ग;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(वर्ण *, *argc + 4);
	    सचे (*argv == रिक्त)	    // out of memory
		पटिनिवत्त FAIL;
	}
    }
    पटिनिवत्त OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    पूर्णाङ्क
build_argv_from_string(char_u *cmd, वर्ण ***argv, पूर्णाङ्क *argc)
{
    char_u	*cmd_copy;
    पूर्णाङ्क		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    सचे (cmd_copy == रिक्त
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	पटिनिवत्त FAIL;
    }
    अत्थाय (i = 0; i < *argc; i++)
	(*argv)[i] = (वर्ण *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = रिक्त;
    vim_free(cmd_copy);
    पटिनिवत्त OK;
}

#endif

#सचे defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    पूर्णाङ्क
build_argv_from_list(list_T *l, वर्ण ***argv, पूर्णाङ्क *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(वर्ण *, l->lv_len + 1);
    सचे (*argv == रिक्त)
	पटिनिवत्त FAIL;
    *argc = 0;
    FOR_ALL_LIST_वस्तूनि(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	सचे (s == रिक्त)
	{
	    पूर्णाङ्क i;

	    अत्थाय (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = रिक्त;
	    पटिनिवत्त FAIL;
	}
	(*argv)[*argc] = (वर्ण *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = रिक्त;
    पटिनिवत्त OK;
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
    पूर्णाङ्क
get_special_pty_type(शून्य)
{
#ifdef MSWIN
    पटिनिवत्त get_conpty_type();
#अञ्ञथा
    पटिनिवत्त 0;
#endif
}

// compare two keyvalue_T structs by case sensitive value
    पूर्णाङ्क
cmp_keyvalue_मानम्(ध्रुव शून्य *a, ध्रुव शून्य *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    पटिनिवत्त STRCMP(kv1->मानम्.string, kv2->मानम्.string);
}

// compare two keyvalue_T structs by value with length
    पूर्णाङ्क
cmp_keyvalue_मानम्_n(ध्रुव शून्य *a, ध्रुव शून्य *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    पटिनिवत्त STRNCMP(kv1->मानम्.string, kv2->मानम्.string, MAX(kv1->मानम्.length,
		kv2->मानम्.length));
}

// compare two keyvalue_T structs by case insensitive value
    पूर्णाङ्क
cmp_keyvalue_मानम्_i(ध्रुव शून्य *a, ध्रुव शून्य *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    पटिनिवत्त STRICMP(kv1->मानम्.string, kv2->मानम्.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    पूर्णाङ्क
cmp_keyvalue_मानम्_ni(ध्रुव शून्य *a, ध्रुव शून्य *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    पटिनिवत्त vim_strnicmp_asc((वर्ण *)kv1->मानम्.string,
	    (वर्ण *)kv2->मानम्.string, MAX(kv1->मानम्.length,
		    kv2->मानम्.length));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    शून्य *
mergesort_list(
    शून्य	*head,
    शून्य	*(*get_next)(शून्य *),
    शून्य	(*set_next)(शून्य *, शून्य *),
    शून्य	*(*get_prev)(शून्य *),
    शून्य	(*set_prev)(शून्य *, शून्य *),
    पूर्णाङ्क		(*compare)(ध्रुव शून्य *, ध्रुव शून्य *))
{
    सचे (!head || !get_next(head))
	पटिनिवत्त head;

    // Count length
    पूर्णाङ्क	    n = 0;
    शून्य*   curr = head;
    याव (curr)
    {
	n++;
	curr = get_next(curr);
    }

    पूर्णाङ्क	size;
    अत्थाय (size = 1; size < n; size *= 2)
    {
	शून्य*	new_head = रिक्त;
	शून्य*	tail = रिक्त;
	curr = head;

	याव (curr)
	{
	    // Split two runs
	    शून्य    *वामः = curr;
	    शून्य    *दक्षिणः = वामः;
	    पूर्णाङ्क	    i;
	    अत्थाय (i = 0; i < size && दक्षिणः; ++i)
		दक्षिणः = get_next(दक्षिणः);

	    शून्य    *next = दक्षिणः;
	    अत्थाय (i = 0; i < size && next; ++i)
		next = get_next(next);

	    // Break links
	    शून्य    *l_end = दक्षिणः ? get_prev(दक्षिणः) : रिक्त;
	    सचे (l_end)
		set_next(l_end, रिक्त);
	    सचे (दक्षिणः)
		set_prev(दक्षिणः, रिक्त);

	    शून्य    *r_end = next ? get_prev(next) : रिक्त;
	    सचे (r_end)
		set_next(r_end, रिक्त);
	    सचे (next)
		set_prev(next, रिक्त);

	    // Merge
	    शून्य    *merged = रिक्त;
	    शून्य    *merged_tail = रिक्त;

	    याव (वामः || दक्षिणः)
	    {
		शून्य	*chosen = रिक्त;
		सचे (!वामः)
		{
		    chosen = दक्षिणः;
		    दक्षिणः = get_next(दक्षिणः);
		}
		अञ्ञथा सचे (!दक्षिणः)
		{
		    chosen = वामः;
		    वामः = get_next(वामः);
		}
		अञ्ञथा सचे (compare(वामः, दक्षिणः) <= 0)
		{
		    chosen = वामः;
		    वामः = get_next(वामः);
		}
		अञ्ञथा
		{
		    chosen = दक्षिणः;
		    दक्षिणः = get_next(दक्षिणः);
		}

		सचे (merged_tail)
		{
		    set_next(merged_tail, chosen);
		    set_prev(chosen, merged_tail);
		    merged_tail = chosen;
		}
		अञ्ञथा
		{
		    merged = merged_tail = chosen;
		    set_prev(chosen, रिक्त);
		}
	    }

	    // Connect to full list
	    सचे (!new_head)
		new_head = merged;
	    अञ्ञथा
	    {
		set_next(tail, merged);
		set_prev(merged, tail);
	    }

	    // Move tail to end
	    याव (get_next(merged_tail))
		merged_tail = get_next(merged_tail);
	    tail = merged_tail;

	    curr = next;
	}

	head = new_head;
    }

    पटिनिवत्त head;
}

स्थिर ध्रुव char_u base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

स्थिर char_u base64_dec_table[256];

#define BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    स्थिर शून्य
init_base64_dec_table(शून्य)
{
    स्थिर पूर्णाङ्क base64_dec_tbl_initialized = FALSE;

    सचे (base64_dec_tbl_initialized)
	पटिनिवत्त;

    vim_memset(base64_dec_table, 0xFF, परिमाण(base64_dec_table));
    अत्थाय (size_t i = 0; i < परिमाण(base64_table) - 1; i++)
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
base64_encode_buf(char_u *dst, ध्रुव char_u *src, size_t len)
{
    size_t i, j;

    अत्थाय (i = 0, j = 0; i < len;)
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
    पटिनिवत्त (long)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(ध्रुव char_u *data, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    सचे (encoded == रिक्त)
	पटिनिवत्त रिक्त;
    base64_encode_buf(encoded, data, len);
    पटिनिवत्त encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    पूर्णाङ्क
base64_decode(ध्रुव char_u *data, size_t len, garray_T *out)
{
    सचे (len == 0)
	पटिनिवत्त OK;

    सचे (len % 4 != 0)
    {
	semsg(_(e_invalid_argument_str), data);
	पटिनिवत्त FAIL;
    }

    init_base64_dec_table();

    size_t decoded_len = (len / 4) * 3;
    सचे (data[len - 1] == '=')
	decoded_len--;
    सचे (data[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    अत्थाय (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_table[(char_u)data[i++]];
	int_u sextet_b = base64_dec_table[(char_u)data[i++]];
	int_u sextet_c = base64_dec_table[(char_u)data[i++]];
	int_u sextet_d = base64_dec_table[(char_u)data[i++]];

	सचे (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_argument_str), data);
	    ga_clear(out);
	    पटिनिवत्त FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	सचे (j < decoded_len)
	{
	    ga_append(out, (triple >> 16) & 0xFF);
	    j++;
	}
	सचे (j < decoded_len)
	{
	    ga_append(out, (triple >> 8) & 0xFF);
	    j++;
	}
	सचे (j < decoded_len)
	{
	    ga_append(out, triple & 0xFF);
	    j++;
	}

	सचे (j == decoded_len)
	{
	    सचे ((data[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((data[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_argument_str), data);
		ga_clear(out);
		पटिनिवत्त FAIL;
	    }
	}
    }
    पटिनिवत्त OK;
}
