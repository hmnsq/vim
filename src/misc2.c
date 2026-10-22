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
#ضمن "vim.h"

ساكن char_u	*username = عدم; // cached result of mch_get_user_name()

ساكن صحيح coladvance2(pos_T *pos, صحيح addspaces, صحيح finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    صحيح
virtual_سرگرم(فراغ)
{
    بنانشانی صحيح cur_ve_flags = get_ve_flags();

    إذا (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_داخل_کرو) && (State & منوال_داخل_کرو)))
	أرجع TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    إذا (virtual_op != MAYBE)
	أرجع virtual_op;
    أرجع (cur_ve_flags & VE_بلاک) && VIsual_سرگرم && VIsual_منوال == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    صحيح
getviscol(فراغ)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, عدم, عدم, GETVCOL_NO_ABOVE);
    أرجع (صحيح)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    صحيح
coladvance_force(colnr_T wcol)
{
    صحيح rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    إذا (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    وإلا
	// Virtcol is valid
	سیٹ_valid_virtcol(curwin, wcol);
    أرجع rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    صحيح
getviscol2(colnr_T خانہ, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.خانہ = خانہ;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, عدم, عدم, GETVCOL_NO_ABOVE);
    أرجع (صحيح)x;
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
    صحيح
coladvance(colnr_T wantcol)
{
    صحيح rc = getvpos(&curwin->w_cursor, wantcol);

    إذا (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    وإلا إذا (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	سیٹ_valid_virtcol(curwin, wantcol);
    أرجع rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    صحيح
getvpos(pos_T *pos, colnr_T wantcol)
{
    أرجع coladvance2(pos, FALSE, virtual_سرگرم(), wantcol);
}

    ساكن صحيح
coladvance2(
    pos_T	*pos,
    صحيح		addspaces,	// change the text to achieve our goal?
    صحيح		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    صحيح		idx;
    char_u	*سطر;
    صحيح		linelen;
    colnr_T	خانہ = 0;
    صحيح		csize = 0;
    صحيح		one_ہور;
#جے_تعریف_شدہ FEAT_LINEBREAK
    صحيح		سر = 0;
#جے_انت

    one_ہور = (State & منوال_داخل_کرو)
		    || restart_edit != NUL
		    || (VIsual_سرگرم && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    سطر = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    إذا (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_ہور;
	    خانہ = wcol;

	    إذا ((addspaces || finetune) && !VIsual_سرگرم)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_ہور;
		إذا (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    وإلا
    {
	صحيح		چوڑائی = curwin->w_چوڑائی - win_خانہ_off(curwin);
	chartabsize_T	cts;

	إذا (finetune
		&& curwin->w_p_wrap
		&& curwin->w_چوڑائی != 0
		&& wcol >= (colnr_T)چوڑائی
		&& چوڑائی > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    إذا (csize > 0)
		csize--;

	    إذا (wcol / چوڑائی > (colnr_T)csize / چوڑائی
		    && ((State & منوال_داخل_کرو) == 0 || (صحيح)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / چوڑائی + 1) * چوڑائی - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, سطر, سطر);
#جے_تعریف_شدہ FEAT_PROP_POPUP
	cts.cts_no_above = true;
#جے_انت
	طالما (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#جے_تعریف_شدہ FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &سر, عدم);
	    MB_PTR_ADV(cts.cts_ptr);
#وإلا
	    csize = lbr_chartabsize_adv(&cts);
#جے_انت
	    cts.cts_vcol += csize;
	}
	خانہ = cts.cts_vcol;
	idx = (صحيح)(cts.cts_ptr - سطر);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	إذا (خانہ > wcol || (!virtual_سرگرم() && one_ہور == 0))
	{
	    idx -= 1;
#جے_تعریف_شدہ FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= سر;
#جے_انت
	    خانہ -= csize;
	}

	إذا (virtual_سرگرم()
		&& addspaces
		&& wcol >= 0
		&& ((خانہ != wcol && خانہ != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    إذا (سطر[idx] == NUL)
	    {
		// Append spaces
		صحيح	correct = wcol - خانہ;
		char_u	*newline = alloc(idx + correct + 1);
		صحيح	t;

		إذا (newline == عدم)
		    أرجع FAIL;

		لكل (t = 0; t < idx; ++t)
		    newline[t] = سطر[t];

		لكل (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		بدلیا_بائٹاں(pos->lnum, (colnr_T)idx);
		idx += correct;
		خانہ = wcol;
	    }
	    وإلا
	    {
		// Break a tab
		صحيح	correct = wcol - خانہ - csize + 1; // negative!!
		char_u	*newline;
		صحيح	t, s = 0;
		صحيح	v;

		إذا (-correct > csize)
		    أرجع FAIL;

		newline = alloc(linelen + csize);
		إذا (newline == عدم)
		    أرجع FAIL;

		لكل (t = 0; t < linelen; t++)
		{
		    إذا (t != idx)
			newline[s++] = سطر[t];
		    وإلا
			لكل (v = 0; v < csize; v++)
			    newline[s++] = ' ';
		}

		newline[linelen + csize - 1] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		بدلیا_بائٹاں(pos->lnum, idx);
		idx += (csize - 1 + correct);
		خانہ += correct;
	    }
	}
    }

    إذا (idx < 0)
	pos->خانہ = 0;
    وإلا
	pos->خانہ = idx;

    pos->coladd = 0;

    إذا (finetune)
    {
	إذا (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    إذا (!one_ہور)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, عدم, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	وإلا
	{
	    صحيح b = (صحيح)wcol - (صحيح)خانہ;

	    // The difference between wcol and col is used to set coladd.
	    إذا (b > 0 && b < (MAXCOL - 2 * curwin->w_چوڑائی))
		pos->coladd = b;

	    خانہ += b;
	}
    }

    // prevent from moving onto a trail byte
    إذا (has_mbyte)
	mb_adjustpos(curbuf, pos);

    إذا (wcol < 0 || خانہ < wcol)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    صحيح
inc_cursor(فراغ)
{
    أرجع inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    صحيح
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    إذا (lp->خانہ != MAXCOL)
    {
	p = ml_get_pos(lp);
	إذا (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    إذا (has_mbyte)
	    {
		صحيح l = (*mb_ptr2len)(p);

		lp->خانہ += l;
		أرجع ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->خانہ++;
	    lp->coladd = 0;
	    أرجع ((p[1] != NUL) ? 0 : 2);
	}
    }
    إذا (lp->lnum != curbuf->b_ml.ml_سطر_گنتی)     // there is a next line
    {
	lp->خانہ = 0;
	lp->lnum++;
	lp->coladd = 0;
	أرجع 1;
    }
    أرجع -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    صحيح
incl(pos_T *lp)
{
    صحيح	    r;

    إذا ((r = inc(lp)) >= 1 && lp->خانہ)
	r = inc(lp);
    أرجع r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    صحيح
dec_cursor(فراغ)
{
    أرجع dec(&curwin->w_cursor);
}

    صحيح
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    إذا (lp->خانہ == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->خانہ = ml_get_len(lp->lnum);
	إذا (has_mbyte)
	    lp->خانہ -= (*mb_سر_off)(p, p + lp->خانہ);
	أرجع 0;
    }

    إذا (lp->خانہ > 0)
    {
	// still within line
	lp->خانہ--;
	إذا (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->خانہ -= (*mb_سر_off)(p, p + lp->خانہ);
	}
	أرجع 0;
    }

    إذا (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->خانہ = ml_get_len(lp->lnum);
	إذا (has_mbyte)
	    lp->خانہ -= (*mb_سر_off)(p, p + lp->خانہ);
	أرجع 1;
    }

    // at start of file
    أرجع -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    صحيح
decl(pos_T *lp)
{
    صحيح	    r;

    إذا ((r = dec(lp)) == 1 && lp->خانہ)
	r = dec(lp);
    أرجع r;
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

#جے_تعریف_شدہ FEAT_FOLDING
    إذا (hasAnyFolding(wp))
    {
	إذا (lnum > cursor)
	{
	    طالما (lnum > cursor)
	    {
		(فراغ)hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		إذا (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	وإلا إذا (lnum < cursor)
	{
	    طالما (lnum < cursor)
	    {
		(فراغ)hasFoldingWin(wp, lnum, عدم, &lnum, TRUE, عدم);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		إذا (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    وإلا
#جے_انت
	retval = lnum - cursor;

    أرجع retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    فراغ
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    إذا (pos->lnum > buf->b_ml.ml_سطر_گنتی)
	pos->lnum = buf->b_ml.ml_سطر_گنتی;

    إذا (pos->خانہ > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	إذا (pos->خانہ > len)
	    pos->خانہ = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    فراغ
check_cursor_lnum(فراغ)
{
    إذا (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
#جے_تعریف_شدہ FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	إذا (!hasFolding(curbuf->b_ml.ml_سطر_گنتی,
						&curwin->w_cursor.lnum, عدم))
#جے_انت
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
    }
    إذا (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    فراغ
check_cursor_خانہ(فراغ)
{
    check_cursor_خانہ_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    فراغ
check_cursor_خانہ_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.خانہ;
    colnr_T      oldcoladd = win->w_cursor.خانہ + win->w_cursor.coladd;
    بنانشانی صحيح cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_بفر, win->w_cursor.lnum);
    إذا (len == 0)
	win->w_cursor.خانہ = 0;
    وإلا إذا (win->w_cursor.خانہ >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	إذا ((State & منوال_داخل_کرو) || restart_edit
		|| (VIsual_سرگرم && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_سرگرم())
	    win->w_cursor.خانہ = len;
	وإلا
	{
	    win->w_cursor.خانہ = len - 1;
	    // Move the cursor to the head byte.
	    إذا (has_mbyte)
		mb_adjustpos(win->w_بفر, &win->w_cursor);
	}
    }
    وإلا إذا (win->w_cursor.خانہ < 0)
	win->w_cursor.خانہ = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    إذا (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    وإلا إذا (cur_ve_flags == VE_ALL)
    {
	إذا (oldcoladd > win->w_cursor.خانہ)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.خانہ;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    إذا (win->w_cursor.خانہ + 1 < len)
	    {
		صحيح cs, ce;

		getvcol(win, &win->w_cursor, &cs, عدم, &ce, 0);
		إذا (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	وإلا
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    فراغ
check_cursor(فراغ)
{
    check_cursor_lnum();
    check_cursor_خانہ();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    فراغ
check_visual_pos(فراغ)
{
    إذا (VIsual.lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	VIsual.lnum = curbuf->b_ml.ml_سطر_گنتی;
	VIsual.خانہ = 0;
	VIsual.coladd = 0;
    }
    وإلا
    {
	صحيح len = ml_get_len(VIsual.lnum);

	إذا (VIsual.خانہ > len)
	{
	    VIsual.خانہ = len;
	    VIsual.coladd = 0;
	}
    }
}

/*
 * Make sure curwin->w_cursor is not on the NUL at the end of the line.
 * Allow it when in Visual mode and 'selection' is not "old".
 */
    فراغ
adjust_cursor_خانہ(فراغ)
{
    إذا (curwin->w_cursor.خانہ > 0
	    && (!VIsual_سرگرم || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.خانہ;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    صحيح
سیٹ_leftcol(colnr_T leftcol)
{
    صحيح		retval = FALSE;

    // Return quickly when there is no change.
    إذا (curwin->w_leftcol == leftcol)
	أرجع FALSE;
    curwin->w_leftcol = leftcol;

    بدلیا_cline_bef_curs();
    لما lastcol = curwin->w_leftcol + curwin->w_چوڑائی - curwin_خانہ_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    لما siso = get_sidescrolloff_قدر();
    إذا (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    وإلا إذا (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(فراغ)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, عدم, &e, 0);
    إذا (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    وإلا إذا (s < curwin->w_leftcol)
    {
	retval = TRUE;
	إذا (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    بدلیا_cline_bef_curs();
	}
    }

    إذا (retval)
	curwin->w_سیٹ_curswant = true;
    redraw_later(UPD_NOT_VALID);
    أرجع retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    صحيح
copy_option_part(
    char_u	**option,
    char_u	*buf,
    صحيح		maxlen,
    حرف	*sep_chars)
{
    صحيح	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    إذا (*p == '.')
	buf[len++] = *p++;
    طالما (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == عدم)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	إذا (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != عدم)
	    ++p;
	إذا (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    إذا (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_منزل_option_part(p);	// p points to next file name

    *option = p;
    أرجع len;
}

#إذا !defined(HAVE_MEMSET) && !defined(PROTO)
    فراغ *
vim_memset(فراغ *ptr, صحيح c, size_t ناپ)
{
    حرف *p = ptr;

    طالما (ناپ-- > 0)
	*p++ = c;
    أرجع ptr;
}
#جے_انت

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    صحيح
vim_isspace(صحيح x)
{
    أرجع ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

ساكن بنية modmasktable
{
    چھوٹا	mod_نقاب;	// Bit-mask for particular key modifier
    چھوٹا	mod_flag;	// Bit(s) for particular key modifier
    char_u	name;		// Single letter name of modifier
} mod_نقاب_جدول[] =
{
    {MOD_نقاب_ALT,		MOD_نقاب_ALT,		(char_u)'M'},
    {MOD_نقاب_META,		MOD_نقاب_META,		(char_u)'T'},
    {MOD_نقاب_CTRL,		MOD_نقاب_CTRL,		(char_u)'C'},
    {MOD_نقاب_کھسکاؤ,		MOD_نقاب_کھسکاؤ,		(char_u)'S'},
    {MOD_نقاب_MULTI_CLICK,	MOD_نقاب_2_CLICK,	(char_u)'2'},
    {MOD_نقاب_MULTI_CLICK,	MOD_نقاب_3_CLICK,	(char_u)'3'},
    {MOD_نقاب_MULTI_CLICK,	MOD_نقاب_4_CLICK,	(char_u)'4'},
#إذا defined(MACOS_X) || defined(FEAT_GUI_GTK)
    {MOD_نقاب_CMD,		MOD_نقاب_CMD,		(char_u)'D'},
#جے_انت
    // 'A' must be the last one
    {MOD_نقاب_ALT,		MOD_نقاب_ALT,		(char_u)'A'},
    {0, 0, NUL}
    // NOTE: when adding an entry, update MAX_KEY_NAME_LEN!
};

/*
 * Shifted key terminal codes and their unshifted equivalent.
 * Don't add mouse codes here, they are handled separately!
 */
#تعریف_کرو MOD_کلیدیں_اندراج_ناپ 5

ساكن char_u modifier_کلیدیں_جدول[] =
{
//  mod mask	    with modifier		without modifier
    MOD_نقاب_کھسکاؤ, '&', '9',			'@', '1',	// begin
    MOD_نقاب_کھسکاؤ, '&', '0',			'@', '2',	// cancel
    MOD_نقاب_کھسکاؤ, '*', '1',			'@', '4',	// command
    MOD_نقاب_کھسکاؤ, '*', '2',			'@', '5',	// copy
    MOD_نقاب_کھسکاؤ, '*', '3',			'@', '6',	// create
    MOD_نقاب_کھسکاؤ, '*', '4',			'k', 'D',	// delete char
    MOD_نقاب_کھسکاؤ, '*', '5',			'k', 'L',	// delete line
    MOD_نقاب_کھسکاؤ, '*', '7',			'@', '7',	// end
    MOD_نقاب_CTRL,  KS_EXTRA, (صحيح)KE_C_سرا,	'@', '7',	// end
    MOD_نقاب_کھسکاؤ, '*', '9',			'@', '9',	// exit
    MOD_نقاب_کھسکاؤ, '*', '0',			'@', '0',	//لبھو
    MOD_نقاب_کھسکاؤ, '#', '1',			'%', '1',	// help
    MOD_نقاب_کھسکاؤ, '#', '2',			'k', 'h',	// home
    MOD_نقاب_CTRL,  KS_EXTRA, (صحيح)KE_C_HOME,	'k', 'h',	// home
    MOD_نقاب_کھسکاؤ, '#', '3',			'k', 'I',	// insert
    MOD_نقاب_کھسکاؤ, '#', '4',			'k', 'l',	// left arrow
    MOD_نقاب_CTRL,  KS_EXTRA, (صحيح)KE_C_بایاں,	'k', 'l',	// left arrow
    MOD_نقاب_کھسکاؤ, '%', 'a',			'%', '3',	//سنیہا
    MOD_نقاب_کھسکاؤ, '%', 'b',			'%', '4',	// move
    MOD_نقاب_کھسکاؤ, '%', 'c',			'%', '5',	//اگلا
    MOD_نقاب_کھسکاؤ, '%', 'd',			'%', '7',	// options
    MOD_نقاب_کھسکاؤ, '%', 'e',			'%', '8',	//پچھلا
    MOD_نقاب_کھسکاؤ, '%', 'f',			'%', '9',	// print
    MOD_نقاب_کھسکاؤ, '%', 'g',			'%', '0',	// redo
    MOD_نقاب_کھسکاؤ, '%', 'h',			'&', '3',	// replace
    MOD_نقاب_کھسکاؤ, '%', 'i',			'k', 'r',	// right arr.
    MOD_نقاب_CTRL,  KS_EXTRA, (صحيح)KE_C_دایاں,	'k', 'r',	// right arr.
    MOD_نقاب_کھسکاؤ, '%', 'j',			'&', '5',	// resume
    MOD_نقاب_کھسکاؤ, '!', '1',			'&', '6',	// save
    MOD_نقاب_کھسکاؤ, '!', '2',			'&', '7',	// suspend
    MOD_نقاب_کھسکاؤ, '!', '3',			'&', '8',	// undo
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_اتے,	'k', 'u',	// up arrow
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_تھلے,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_XF1,	KS_EXTRA, (صحيح)KE_XF1,
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_XF2,	KS_EXTRA, (صحيح)KE_XF2,
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_XF3,	KS_EXTRA, (صحيح)KE_XF3,
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_XF4,	KS_EXTRA, (صحيح)KE_XF4,

    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F1,	'k', '1',	// F1
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F2,	'k', '2',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F3,	'k', '3',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F4,	'k', '4',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F5,	'k', '5',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F6,	'k', '6',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F7,	'k', '7',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F8,	'k', '8',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F9,	'k', '9',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F10,	'k', ';',	// F10

    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F11,	'F', '1',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F12,	'F', '2',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F13,	'F', '3',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F14,	'F', '4',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F15,	'F', '5',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F16,	'F', '6',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F17,	'F', '7',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F18,	'F', '8',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F19,	'F', '9',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F20,	'F', 'A',

    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F21,	'F', 'B',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F22,	'F', 'C',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F23,	'F', 'D',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F24,	'F', 'E',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F25,	'F', 'F',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F26,	'F', 'G',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F27,	'F', 'H',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F28,	'F', 'I',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F29,	'F', 'J',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F30,	'F', 'K',

    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F31,	'F', 'L',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F32,	'F', 'M',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F33,	'F', 'N',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F34,	'F', 'O',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F35,	'F', 'P',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F36,	'F', 'Q',
    MOD_نقاب_کھسکاؤ, KS_EXTRA, (صحيح)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_نقاب_کھسکاؤ, 'k', 'B',			KS_EXTRA, (صحيح)KE_TAB,

    NUL
};

ساكن بنية کلید_name_اندراج
{
    صحيح		enabled;	    // is this entry available (TRUE/FALSE)?
    صحيح		کلید;		    // special key code or ascii value
    string_T	name;		    // name of key
    صحيح		is_alt;		    // is an alternative name
} کلید_ناں_جدول[] =
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
#جے_تعریف_شدہ FEAT_MOUSE_DEC
    TRUE,
#وإلا
    FALSE,
#جے_انت
	K_DEC_MOUSE, STR_LITERAL_INIT("DecMouse"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Del"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Delete"), TRUE},
    {TRUE, K_تھلے, STR_LITERAL_INIT("Down"), FALSE},
    {TRUE, K_DROP, STR_LITERAL_INIT("Drop"), FALSE},
    {TRUE, K_سرا, STR_LITERAL_INIT("End"), FALSE},
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
#جے_تعریف_شدہ FEAT_MOUSE_JSB
    TRUE,
#وإلا
    FALSE,
#جے_انت
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
    {TRUE, K_بایاں, STR_LITERAL_INIT("Left"), FALSE},
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
#جے_تعریف_شدہ FEAT_MOUSE_NET
    TRUE,
#وإلا
    FALSE,
#جے_انت
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
#جے_تعریف_شدہ FEAT_MOUSE_PTERM
    TRUE,
#وإلا
    FALSE,
#جے_انت
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_دایاں, STR_LITERAL_INIT("Right"), FALSE},
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
#جے_تعریف_شدہ FEAT_EVAL
    TRUE,
#وإلا
    FALSE,
#جے_انت
	K_SNR, STR_LITERAL_INIT("SNR"), FALSE},
    {TRUE, ' ', STR_LITERAL_INIT("Space"), FALSE},
    {TRUE, TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_UNDO, STR_LITERAL_INIT("Undo"), FALSE},
    {TRUE, K_اتے, STR_LITERAL_INIT("Up"), FALSE},
    {
#جے_تعریف_شدہ FEAT_MOUSE_URXVT
    TRUE,
#وإلا
    FALSE,
#جے_انت
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
    ساكن صحيح
name_منزل_mod_نقاب(صحيح c)
{
    صحيح	    i;

    c = TOUPPER_ASC(c);
    لكل (i = 0; mod_نقاب_جدول[i].mod_نقاب != 0; i++)
	إذا (c == mod_نقاب_جدول[i].name)
	    أرجع mod_نقاب_جدول[i].mod_flag;
    أرجع 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    صحيح
simplify_کلید(صحيح کلید, صحيح *modifiers)
{
    صحيح	    i;
    صحيح	    key0;
    صحيح	    key1;

    إذا (!(*modifiers & (MOD_نقاب_کھسکاؤ | MOD_نقاب_CTRL)))
	أرجع کلید;

    // TAB is a special case
    إذا (کلید == TAB && (*modifiers & MOD_نقاب_کھسکاؤ))
    {
	*modifiers &= ~MOD_نقاب_کھسکاؤ;
	أرجع K_S_TAB;
    }
    key0 = KEY2TERMCAP0(کلید);
    key1 = KEY2TERMCAP1(کلید);
    لكل (i = 0; modifier_کلیدیں_جدول[i] != NUL; i += MOD_کلیدیں_اندراج_ناپ)
    {
	إذا (key0 == modifier_کلیدیں_جدول[i + 3]
		&& key1 == modifier_کلیدیں_جدول[i + 4]
		&& (*modifiers & modifier_کلیدیں_جدول[i]))
	{
	    *modifiers &= ~modifier_کلیدیں_جدول[i];
	    أرجع TERMCAP2_کلید(modifier_کلیدیں_جدول[i + 1],
		    modifier_کلیدیں_جدول[i + 2]);
	}
    }
    أرجع کلید;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    صحيح
handle_x_کلیدیں(صحيح کلید)
{
    بدل (کلید)
    {
	حالة K_XUP:	أرجع K_اتے;
	حالة K_XDOWN:	أرجع K_تھلے;
	حالة K_XLEFT:	أرجع K_بایاں;
	حالة K_XRIGHT:	أرجع K_دایاں;
	حالة K_XHOME:	أرجع K_HOME;
	حالة K_ZHOME:	أرجع K_HOME;
	حالة K_XEND:	أرجع K_سرا;
	حالة K_ZEND:	أرجع K_سرا;
	حالة K_XF1:	أرجع K_F1;
	حالة K_XF2:	أرجع K_F2;
	حالة K_XF3:	أرجع K_F3;
	حالة K_XF4:	أرجع K_F4;
	حالة K_S_XF1:	أرجع K_S_F1;
	حالة K_S_XF2:	أرجع K_S_F2;
	حالة K_S_XF3:	أرجع K_S_F3;
	حالة K_S_XF4:	أرجع K_S_F4;
    }
    أرجع کلید;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_کلید_name(صحيح c, صحيح modifiers)
{
    ساكن char_u string[MAX_کلید_NAME_LEN + 1];
    صحيح	    i, idx, len;
    صحيح	    جدول_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    إذا (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_کلید)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    إذا (IS_SPECIAL(c))
    {
	لكل (i = 0; modifier_کلیدیں_جدول[i] != 0; i += MOD_کلیدیں_اندراج_ناپ)
	    إذا (       KEY2TERMCAP0(c) == (صحيح)modifier_کلیدیں_جدول[i + 1]
		    && (صحيح)KEY2TERMCAP1(c) == (صحيح)modifier_کلیدیں_جدول[i + 2])
	    {
		modifiers |= modifier_کلیدیں_جدول[i];
		c = TERMCAP2_کلید(modifier_کلیدیں_جدول[i + 3],
						   modifier_کلیدیں_جدول[i + 4]);
		اكسر;
	    }
    }

    // try to find the key in the special key table
    جدول_idx = لبھو_special_کلید_وچ_جدول(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    إذا (c > 0 && (*mb_char2len)(c) == 1)
    {
	إذا (جدول_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_نقاب_ALT;
	    // try again, to find the un-alted key in the special key table
	    جدول_idx = لبھو_special_کلید_وچ_جدول(c);
	}
	إذا (جدول_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_نقاب_CTRL;
	}
    }

    // translate the modifier into a string
    لكل (i = 0; mod_نقاب_جدول[i].name != 'A'; i++)
	إذا ((modifiers & mod_نقاب_جدول[i].mod_نقاب)
						== mod_نقاب_جدول[i].mod_flag)
	{
	    string[idx++] = mod_نقاب_جدول[i].name;
	    string[idx++] = (char_u)'-';
	}

    إذا (جدول_idx < 0)		// unknown special key, may output t_xx
    {
	إذا (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	وإلا
	{
	    len = (*mb_char2len)(c);
	    إذا (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    وإلا إذا (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    وإلا
	    {
		char_u	*s = transchar(c);
		طالما (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    وإلا		// use name of special key
    {
	string_T    *s;

	s = &کلید_ناں_جدول[جدول_idx].name;

	إذا (s->لمائی + idx + 2 <= MAX_کلید_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (صحيح)s->لمائی;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    أرجع string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    صحيح
trans_special(
    char_u	**srcp,
    char_u	*dst,
    صحيح		flags,		// FSK_ values
    صحيح		escape_ks,	// escape K_SPECIAL bytes in the character
    صحيح		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    صحيح		modifiers = 0;
    صحيح		کلید;

    کلید = لبھو_special_کلید(srcp, &modifiers, flags, did_simplify);
    إذا (کلید == 0)
	أرجع 0;

    أرجع special_منزل_buf(کلید, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    صحيح
special_منزل_buf(صحيح کلید, صحيح modifiers, صحيح escape_ks, char_u *dst)
{
    صحيح		dlen = 0;

    // Put the appropriate modifier in a string
    إذا (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    إذا (IS_SPECIAL(کلید))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(کلید);
	dst[dlen++] = KEY2TERMCAP1(کلید);
    }
    وإلا إذا (escape_ks)
	dlen = (صحيح)(جوڑو_char2buf(کلید, dst + dlen) - dst);
    وإلا إذا (has_mbyte)
	dlen += (*mb_char2bytes)(کلید, dst + dlen);
    وإلا
	dst[dlen++] = کلید;

    أرجع dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    صحيح
لبھو_special_کلید(
    char_u	**srcp,
    صحيح		*modp,
    صحيح		flags,		// FSK_ values
    صحيح		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*آخری_dash;
    char_u	*سرا_دا_name;
    char_u	*src;
    char_u	*bp;
    صحيح		وچ_string = flags & FSK_وچ_STRING;
    صحيح		modifiers;
    صحيح		ثنائی_ہندسہ;
    صحيح		کلید;
    uvarnumber_T	n;
    صحيح		l;

    src = *srcp;
    إذا (src[0] != '<')
	أرجع 0;
    إذا (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    آخری_dash = src;
    لكل (bp = src + 1; *bp == '-' || vim_is_عام_IDc(*bp); bp++)
    {
	إذا (*bp == '-')
	{
	    آخری_dash = bp;
	    إذا (bp[1] != NUL)
	    {
		إذا (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		وإلا
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		إذا (!(وچ_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		وإلا إذا (وچ_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	إذا (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	وإلا إذا (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, عدم, &l, STR2NR_ALL, عدم, عدم, 0, TRUE, عدم);
	    إذا (l == 0)
	    {
		emsg(_(e_invalid_دلیل));
		أرجع 0;
	    }
	    bp += l + 5;
	    اكسر;
	}
    }

    إذا (*bp == '>')	// found matching '>'
    {
	سرا_دا_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	لكل (bp = src + 1; bp < آخری_dash; bp++)
	{
	    إذا (*bp != '-')
	    {
		ثنائی_ہندسہ = name_منزل_mod_نقاب(*bp);
		إذا (ثنائی_ہندسہ == 0x0)
		    اكسر;	// Illegal modifier name
		modifiers |= ثنائی_ہندسہ;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	إذا (bp >= آخری_dash)
	{
	    إذا (STRNICMP(آخری_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(آخری_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(آخری_dash + 6, عدم, &l, STR2NR_ALL, عدم,
							    &n, 0, TRUE, عدم);
		إذا (l == 0)
		{
		    emsg(_(e_invalid_دلیل));
		    أرجع 0;
		}
		کلید = (صحيح)n;
	    }
	    وإلا
	    {
		صحيح off = 1;

		// Modifier with single letter, or special key name.
		إذا (وچ_string && آخری_dash[1] == '\\' && آخری_dash[2] == '"')
		    off = 2;
		إذا (has_mbyte)
		    l = mb_ptr2len(آخری_dash + off);
		وإلا
		    l = 1;
		إذا (modifiers != 0 && آخری_dash[l + off] == '>')
		    کلید = PTR2CHAR(آخری_dash + off);
		وإلا
		{
		    کلید = get_special_کلید_کوڈ(آخری_dash + off);
		    إذا (!(flags & FSK_KEEP_X_کلید))
			کلید = handle_x_کلیدیں(کلید);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    إذا (کلید != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		کلید = simplify_کلید(کلید, &modifiers);

		إذا ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    إذا (کلید == K_BS)
			کلید = BS;
		    وإلا إذا (کلید == K_DEL || کلید == K_KDEL)
			کلید = DEL;
		}
		وإلا إذا (کلید == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    إذا ((flags & FSK_SIMPLIFY) != 0)
		    {
			إذا (did_simplify != عدم)
			    *did_simplify = TRUE;
		    }
		    وإلا
			کلید = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		إذا (!IS_SPECIAL(کلید))
		    کلید = کڈھو_modifiers(کلید, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = سرا_دا_name;
		أرجع کلید;
	    }
	}
    }
    أرجع 0;
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
    صحيح
may_adjust_کلید_for_ctrl(صحيح modifiers, صحيح کلید)
{
    إذا ((modifiers & MOD_نقاب_CTRL) == 0)
	أرجع کلید;

    إذا (ASCII_ISALPHA(کلید))
    {
#جے_تعریف_شدہ FEAT_TERMINAL
	check_no_reduce_کلیدیں();  // may update the no_reduce_keys flag
#جے_انت
	أرجع no_reduce_کلیدیں == 0 ? TOUPPER_ASC(کلید) : کلید;
    }
    إذا (کلید == '2')
	أرجع '@';
    إذا (کلید == '6')
	أرجع '^';
    إذا (کلید == '-')
	أرجع '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    إذا (کلید == '$' && (modifiers & MOD_نقاب_کھسکاؤ) == 0)
	أرجع ']';

    أرجع کلید;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    صحيح
may_remove_کھسکاؤ_modifier(صحيح modifiers, صحيح کلید)
{
    إذا ((modifiers == MOD_نقاب_کھسکاؤ
		|| modifiers == (MOD_نقاب_کھسکاؤ | MOD_نقاب_ALT)
#جے_تعریف_شدہ FEAT_GUI_GTK
		|| modifiers == (MOD_نقاب_کھسکاؤ | MOD_نقاب_CMD)
#جے_انت
		|| modifiers == (MOD_نقاب_کھسکاؤ | MOD_نقاب_META))
	    && ((کلید >= '!' && کلید <= '/')
		|| (کلید >= ':' && کلید <= 'Z')
		|| vim_isdigit(کلید)
		|| (کلید >= '[' && کلید <= '`')
		|| (کلید >= '{' && کلید <= '~')))
	أرجع modifiers & ~MOD_نقاب_کھسکاؤ;

    إذا (modifiers == (MOD_نقاب_کھسکاؤ | MOD_نقاب_CTRL)
		&& (کلید == '{' || کلید == '}' || کلید == '|'))
	أرجع modifiers & ~MOD_نقاب_کھسکاؤ;

    أرجع modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    صحيح
کڈھو_modifiers(صحيح کلید, صحيح *modp, صحيح simplify, صحيح *did_simplify)
{
    صحيح	modifiers = *modp;

#جے_تعریف_شدہ MACOS_X
    // Command-key really special, no fancynest
    إذا (!(modifiers & MOD_نقاب_CMD))
#جے_انت
    إذا ((modifiers & MOD_نقاب_کھسکاؤ) && ASCII_ISALPHA(کلید))
    {
	کلید = TOUPPER_ASC(کلید);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	إذا (simplify || modifiers == MOD_نقاب_کھسکاؤ
		|| modifiers == (MOD_نقاب_کھسکاؤ | MOD_نقاب_ALT)
		|| modifiers == (MOD_نقاب_کھسکاؤ | MOD_نقاب_META))
	    modifiers &= ~MOD_نقاب_کھسکاؤ;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    إذا ((modifiers & MOD_نقاب_CTRL) && ASCII_ISALPHA(کلید))
	کلید = TOUPPER_ASC(کلید);

    إذا (simplify && (modifiers & MOD_نقاب_CTRL)
	    && ((کلید >= '?' && کلید <= '_') || ASCII_ISALPHA(کلید)))
    {
	کلید = Ctrl_chr(کلید);
	modifiers &= ~MOD_نقاب_CTRL;
	// <C-@> is <Nul>
	إذا (کلید == NUL)
	    کلید = K_ZERO;
	إذا (did_simplify != عدم)
	    *did_simplify = TRUE;
    }

#جے_تعریف_شدہ MACOS_X
    // Command-key really special, no fancynest
    إذا (!(modifiers & MOD_نقاب_CMD))
#جے_انت
    إذا (simplify && (modifiers & MOD_نقاب_ALT) && کلید < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	کلید |= 0x80;
	modifiers &= ~MOD_نقاب_ALT;	// remove the META modifier
	إذا (did_simplify != عدم)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    أرجع کلید;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    صحيح
لبھو_special_کلید_وچ_جدول(صحيح c)
{
    صحيح	    i;

    لكل (i = 0; i < (صحيح)ARRAY_لمائی(کلید_ناں_جدول); i++)
	إذا (c == کلید_ناں_جدول[i].کلید && !کلید_ناں_جدول[i].is_alt)
	    أرجع کلید_ناں_جدول[i].enabled ? i : -1;

    أرجع -1;
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
    ساكن صحيح
cmp_کلید_name_اندراج(ثابت فراغ *a, ثابت فراغ *b)
{
    char_u  *p1 = ((بنية کلید_name_اندراج *)a)->name.string;
    char_u  *p2 = ((بنية کلید_name_اندراج *)b)->name.string;
    صحيح	    نتیجہ = 0;

    إذا (p1 == p2)
	أرجع 0;

    طالما (vim_is_عام_IDc(*p1) && *p2 != NUL)
    {
	إذا ((نتیجہ = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    اكسر;
	++p1;
	++p2;
    }

    إذا (نتیجہ == 0)
    {
	إذا (*p2 == NUL)
	{
	    إذا (vim_is_عام_IDc(*p1))
		نتیجہ = 1;
	}
	وإلا
	{
	    نتیجہ = -1;
	}
    }

    أرجع نتیجہ;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    صحيح
get_special_کلید_کوڈ(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    إذا (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	إذا (جوڑو_termcap_اندراج(string, FALSE) == OK)
	    أرجع TERMCAP2_کلید(name[2], name[3]);
    }
    وإلا
    {
	بنية کلید_name_اندراج	نشانہ;
	بنية کلید_name_اندراج	*اندراج;

	نشانہ.enabled = TRUE;
	نشانہ.کلید = 0;
	نشانہ.name.string = name;
	نشانہ.name.لمائی = 0;

	اندراج = (بنية کلید_name_اندراج *)bsearch(
	    &نشانہ,
	    &کلید_ناں_جدول,
	    ARRAY_لمائی(کلید_ناں_جدول),
	    حجم(کلید_ناں_جدول[0]),
	    cmp_کلید_name_اندراج);
	إذا (اندراج != عدم && اندراج->enabled)
	{
	    صحيح کلید = اندراج->کلید;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    أرجع کلید == K_TAB ? TAB : کلید;
	}
    }

    أرجع 0;
}

    char_u *
get_کلید_name(صحيح i)
{
    إذا (i < 0 || i >= (صحيح)ARRAY_لمائی(کلید_ناں_جدول))
	أرجع عدم;

    أرجع کلید_ناں_جدول[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    صحيح
get_fileformat(buf_T *buf)
{
    صحيح		c = *buf->b_p_ff;

    إذا (buf->b_p_bin || c == 'u')
	أرجع EOL_UNIX;
    إذا (c == 'm')
	أرجع EOL_MAC;
    أرجع EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    صحيح
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    صحيح		c;

    إذا (eap != عدم && eap->force_ff != 0)
	c = eap->force_ff;
    وإلا
    {
	إذا ((eap != عدم && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    أرجع EOL_UNIX;
	c = *buf->b_p_ff;
    }
    إذا (c == 'u')
	أرجع EOL_UNIX;
    إذا (c == 'm')
	أرجع EOL_MAC;
    أرجع EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    فراغ
سیٹ_fileformat(
    صحيح		t,
    صحيح		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    حرف	*p = عدم;

    بدل (t)
    {
    حالة EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	اكسر;
    حالة EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	اكسر;
    حالة EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	اكسر;
    }
    إذا (p != عدم)
	سیٹ_string_option_سدھا((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_حالت(curbuf);
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#جے_انت
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    صحيح
default_fileformat(فراغ)
{
    بدل (*p_ffs)
    {
	حالة 'm':   أرجع EOL_MAC;
	حالة 'd':   أرجع EOL_DOS;
    }
    أرجع EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    صحيح
call_shell(char_u *cmd, صحيح opt)
{
    صحيح		retval;
#جے_تعریف_شدہ FEAT_PROFILE
    proftime_T	wait_ویلا;
#جے_انت

    إذا (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == عدم ? p_sh : cmd);
	پیغام_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#جے_تعریف_شدہ FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_بچہ_enter(&wait_ویلا);
#جے_انت

    إذا (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_خالی_تھاواں));
	retval = -1;
    }
    وإلا
    {
#جے_تعریف_شدہ FEAT_GUI_MSWIN
	// Don't hide the pointer while executing a shell command.
	gui_mch_mousehide(FALSE);
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
	++hold_gui_events;
#جے_انت
	// The external command may update a tags file, clear cached tags.
	ٹیگ_freematch();

	إذا (cmd == عدم || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	وإلا
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    إذا (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		إذا (ecmd == عدم)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    إذا (ncmd != عدم)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((حرف *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    وإلا
		retval = -1;
	    إذا (ecmd != cmd)
		vim_free(ecmd);
	}
#جے_تعریف_شدہ FEAT_GUI
	--hold_gui_events;
#جے_انت
	/*
	 * Check the window size, in case it changed while executing the
	 * external command.
	 */
	shell_resized_check();
    }

#جے_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_nr(VV_SHELL_ERROR, (لما)retval);
# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_بچہ_exit(&wait_ویلا);
# endif
#جے_انت

    أرجع retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    صحيح
get_real_state(فراغ)
{
    إذا (State & منوال_عام)
    {
	إذا (VIsual_سرگرم)
	{
	    إذا (VIsual_select)
		أرجع منوال_SELECT;
	    أرجع منوال_VISUAL;
	}
	وإلا إذا (انت_op)
	    أرجع منوال_OP_PENDING;
    }
    أرجع State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    صحيح
after_pathsep(char_u *b, char_u *p)
{
    أرجع p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_سر_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    صحيح
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    إذا (f1 == عدم || f2 == عدم)
	أرجع FALSE;

    (فراغ)vim_بھریا_Name(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    أرجع (t1 - ffname == t2 - f2
	     && pathcmp((حرف *)ffname, (حرف *)f2, (صحيح)(t1 - ffname)) == 0);
}

#إذا defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    صحيح
vim_chdirfile(char_u *fname, حرف *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    إذا (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    إذا (pathcmp((حرف *)old_dir, (حرف *)new_dir, -1) == 0)
	// nothing to do
	أرجع OK;

    إذا (trigger_autocmd != عدم)
	trigger_Dir_بدلیا_Pre((char_u *)trigger_autocmd, new_dir);

    إذا (mch_chdir((حرف *)new_dir) != 0)
	أرجع FAIL;

    إذا (trigger_autocmd != عدم)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    أرجع OK;
}
#جے_انت

#إذا defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    ساكن صحيح
illegal_slash(ثابت حرف *name)
{
    إذا (name[0] == NUL)
	أرجع FALSE;	    // no file name is not illegal
    إذا (name[لڑی_لمبائی(name) - 1] != '/')
	أرجع FALSE;	    // no trailing slash
    إذا (mch_isdir((char_u *)name))
	أرجع FALSE;	    // trailing slash for a directory
    أرجع TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    صحيح
vim_stat(ثابت حرف *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    أرجع illegal_slash(name) ? -1 : stat(name, stp);
}
#جے_انت

#إذا defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_جدول[SHAPE_IDX_گنتی] =
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
ساكن string_T mshape_ناں[] =
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
    {عدم, 0}
};

#  define MSHAPE_ناں_گنتی  (ARRAY_لمائی(mshape_ناں) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    حرف *
parse_shape_opt(صحيح what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    صحيح		idx = 0;		// init for GCC
    صحيح		all_idx;
    صحيح		len;
    صحيح		i;
    لما	n;
    صحيح		لبھیا_ve = FALSE;	// found "ve" flag
    صحيح		گیڑا;

    /*
     * First round: check for errors; second round: do it for real.
     */
    لكل (گیڑا = 1; گیڑا <= 2; ++گیڑا)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	إذا (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	وإلا
# endif
	    modep = p_guicursor;
	طالما (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    إذا (colonp == عدم || (commap != عدم && commap < colonp))
		أرجع e_missing_colon_2;
	    إذا (colonp == modep)
		أرجع e_illegal_منوال;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    طالما (modep < colonp || all_idx >= 0)
	    {
		إذا (all_idx < 0)
		{
		    // Find the mode.
		    إذا (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    وإلا
			len = 2;
		    إذا (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_گنتی - 1;
		    وإلا
		    {
			لكل (idx = 0; idx < SHAPE_IDX_گنتی; ++idx)
			    إذا (STRNICMP(modep, shape_جدول[idx].name, len)
									 == 0)
				اكسر;
			إذا (idx == SHAPE_IDX_گنتی
				   || (shape_جدول[idx].ورتیا_for & what) == 0)
			    أرجع e_illegal_منوال;
			إذا (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    لبھیا_ve = TRUE;
		    }
		    modep += len + 1;
		}

		إذا (all_idx >= 0)
		    idx = all_idx--;
		وإلا إذا (گیڑا == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    إذا (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_جدول[idx].mshape = 0;
		    }
		    وإلا
# endif
		    {
			// Set the defaults, for the missing parts
			shape_جدول[idx].shape = SHAPE_بلاک;
			shape_جدول[idx].blinkwait = 700L;
			shape_جدول[idx].blinkon = 400L;
			shape_جدول[idx].blinkoff = 250L;
		    }
		}

		// Parse the part after the colon
		لكل (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    إذا (what == SHAPE_MOUSE)
		    {
			لكل (i = 0; ; ++i)
			{
			    إذا (mshape_ناں[i].string == عدم)
			    {
				إذا (!VIM_ISDIGIT(*p))
				    أرجع e_illegal_mouseshape;
				إذا (گیڑا == 2)
				    shape_جدول[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				وإلا
				    (فراغ)getdigits(&p);
				اكسر;
			    }
			    إذا (STRNICMP(p, mshape_ناں[i].string, mshape_ناں[i].لمائی) == 0)
			    {
				إذا (گیڑا == 2)
				    shape_جدول[idx].mshape = i;
				p += mshape_ناں[i].لمائی;
				اكسر;
			    }
			}
		    }
		    وإلا // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			إذا (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			وإلا إذا (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			وإلا إذا (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			وإلا إذا (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			وإلا إذا (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			إذا (len != 0)
			{
			    p += len;
			    إذا (!VIM_ISDIGIT(*p))
				أرجع e_digit_expected;
			    n = getdigits(&p);
			    إذا (len == 3)   // "ver" or "hor"
			    {
				إذا (n == 0)
				    أرجع e_illegal_percentage;
				إذا (گیڑا == 2)
				{
				    إذا (TOLOWER_ASC(i) == 'v')
					shape_جدول[idx].shape = SHAPE_VER;
				    وإلا
					shape_جدول[idx].shape = SHAPE_HOR;
				    shape_جدول[idx].percentage = n;
				}
			    }
			    وإلا إذا (گیڑا == 2)
			    {
				إذا (len == 9)
				    shape_جدول[idx].blinkwait = n;
				وإلا إذا (len == 7)
				    shape_جدول[idx].blinkon = n;
				وإلا
				    shape_جدول[idx].blinkoff = n;
			    }
			}
			وإلا إذا (STRNICMP(p, "block", 5) == 0)
			{
			    إذا (گیڑا == 2)
				shape_جدول[idx].shape = SHAPE_بلاک;
			    p += 5;
			}
			وإلا	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    إذا (commap == عدم)		    // last part
			    {
				إذا (endp == عدم)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    وإلا إذا (endp > commap || endp == عدم)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    إذا (slashp != عدم && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_گروہ(p, (صحيح)(slashp - p));
				p = slashp + 1;
			    }
			    إذا (گیڑا == 2)
			    {
				shape_جدول[idx].id = syn_check_گروہ(p,
							     (صحيح)(endp - p));
				shape_جدول[idx].id_lm = shape_جدول[idx].id;
				إذا (slashp != عدم && slashp < endp)
				    shape_جدول[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    إذا (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    إذا (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    إذا (!لبھیا_ve)
    {
# ifdef FEAT_MOUSESHAPE
	إذا (what == SHAPE_MOUSE)
	{
	    shape_جدول[SHAPE_IDX_VE].mshape = shape_جدول[SHAPE_IDX_V].mshape;
	}
	وإلا
# endif
	{
	    shape_جدول[SHAPE_IDX_VE].shape = shape_جدول[SHAPE_IDX_V].shape;
	    shape_جدول[SHAPE_IDX_VE].percentage =
					 shape_جدول[SHAPE_IDX_V].percentage;
	    shape_جدول[SHAPE_IDX_VE].blinkwait =
					  shape_جدول[SHAPE_IDX_V].blinkwait;
	    shape_جدول[SHAPE_IDX_VE].blinkon =
					    shape_جدول[SHAPE_IDX_V].blinkon;
	    shape_جدول[SHAPE_IDX_VE].blinkoff =
					   shape_جدول[SHAPE_IDX_V].blinkoff;
	    shape_جدول[SHAPE_IDX_VE].id = shape_جدول[SHAPE_IDX_V].id;
	    shape_جدول[SHAPE_IDX_VE].id_lm = shape_جدول[SHAPE_IDX_V].id_lm;
	}
    }

    أرجع عدم;
}

# إذا defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    صحيح
get_shape_idx(صحيح mouse)
{
#  ifdef FEAT_MOUSESHAPE
    إذا (mouse && (State == منوال_HITRETURN || State == منوال_ASKMORE))
    {
#   ifdef FEAT_GUI
	صحيح x, y;
	gui_mch_getmouse(&x, &y);
	إذا (Y_2_صف(y) == صفاں - 1)
	    أرجع SHAPE_IDX_MOREL;
#   endif
	أرجع SHAPE_IDX_ہور;
    }
    إذا (mouse && drag_حالت_سطر)
	أرجع SHAPE_IDX_SDRAG;
    إذا (mouse && drag_sep_سطر)
	أرجع SHAPE_IDX_VDRAG;
#  endif
    إذا (!mouse && State == منوال_SHOWMATCH)
	أرجع SHAPE_IDX_SM;
    إذا (State & VREPLACE_FLAG)
	أرجع SHAPE_IDX_R;
    إذا (State & REPLACE_FLAG)
	أرجع SHAPE_IDX_R;
    إذا (State & منوال_داخل_کرو)
	أرجع SHAPE_IDX_I;
    إذا (State & منوال_CMDLINE)
    {
	إذا (cmdline_at_سرا())
	    أرجع SHAPE_IDX_C;
	إذا (cmdline_overstrike())
	    أرجع SHAPE_IDX_CR;
	أرجع SHAPE_IDX_CI;
    }
    إذا (انت_op)
	أرجع SHAPE_IDX_O;
    إذا (VIsual_سرگرم)
    {
	إذا (*p_sel == 'e')
	    أرجع SHAPE_IDX_VE;
	وإلا
	    أرجع SHAPE_IDX_V;
    }
    أرجع SHAPE_IDX_N;
}
# endif

# إذا defined(FEAT_MOUSESHAPE)
ساكن صحيح current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    فراغ
update_mouseshape(صحيح shape_idx)
{
    صحيح new_mouse_shape;

    // Only works in GUI mode.
    إذا (!gui.وچ_use || gui.starting)
	أرجع;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    إذا (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	أرجع;
    }

    // When ignoring the mouse don't change shape on the statusline.
    إذا (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_حالت
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    إذا (shape_idx == -2
	    && current_mouse_shape != shape_جدول[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_جدول[SHAPE_IDX_حالت].mshape
	    && current_mouse_shape != shape_جدول[SHAPE_IDX_VSEP].mshape)
	أرجع;
    إذا (shape_idx < 0)
	new_mouse_shape = shape_جدول[get_shape_idx(TRUE)].mshape;
    وإلا
	new_mouse_shape = shape_جدول[shape_idx].mshape;
    إذا (new_mouse_shape != current_mouse_shape)
    {
	mch_سیٹ_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#جے_انت // CURSOR_SHAPE

#إذا defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    فراغ
f_getmouseshape(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
# إذا defined(FEAT_MOUSESHAPE)
    إذا (current_mouse_shape >= 0
			      && current_mouse_shape < (صحيح)MSHAPE_ناں_گنتی)
	rettv->vval.v_string = vim_strnsave(
				  mshape_ناں[current_mouse_shape].string,
				  mshape_ناں[current_mouse_shape].لمائی);
# endif
}
#جے_انت



/*
 * Change directory to "new_dir".  Search 'cdpath' for relative directory
 * names.
 */
    صحيح
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    صحيح		r;
    char_u	*file_منزل_لبھو = عدم;
    حرف	*تلاش_ctx = عدم;

    dir_name = لبھو_directory_وچ_رستہ(new_dir, (صحيح)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_منزل_لبھو, &تلاش_ctx);
    vim_free(file_منزل_لبھو);
    vim_findfile_cleanup(تلاش_ctx);
    إذا (dir_name == عدم)
	أرجع -1;
    r = mch_chdir((حرف *)dir_name);
    vim_free(dir_name);
    أرجع r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    صحيح
get_user_name(char_u *buf, صحيح len)
{
    إذا (username == عدم)
    {
	إذا (mch_get_user_name(buf, len) == FAIL)
	    أرجع FAIL;
	username = vim_strsave(buf);
    }
    وإلا
	vim_strncpy(buf, username, len - 1);
    أرجع OK;
}

#إذا defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    فراغ
free_username(فراغ)
{
    vim_free(username);
}
#جے_انت

#إذا !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    فراغ
qsort(
    فراغ	*ادھار,
    size_t	elm_گنتی,
    size_t	elm_ناپ,
    صحيح (*cmp)(ثابت فراغ *, ثابت فراغ *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    صحيح		i, j;
    صحيح		gap;

    buf = alloc(elm_ناپ);
    إذا (buf == عدم)
	أرجع;

    لكل (gap = elm_گنتی / 2; gap > 0; gap /= 2)
	لكل (i = gap; i < elm_گنتی; ++i)
	    لكل (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)ادھار + j * elm_ناپ;
		p2 = (char_u *)ادھار + (j + gap) * elm_ناپ;
		إذا ((*cmp)((فراغ *)p1, (فراغ *)p2) <= 0)
		    اكسر;
		// Exchange the elements.
		mch_memmove(buf, p1, elm_ناپ);
		mch_memmove(p1, p2, elm_ناپ);
		mch_memmove(p2, buf, elm_ناپ);
	    }

    vim_free(buf);
}
#جے_انت

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

#إذا !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

ساكن صحيح  envsize = -1;	// current size of environment
خارجي حرف **environ;		// the global which is your env.

ساكن صحيح  findenv(حرف *name); // look for a name in the env.
ساكن صحيح  newenv(فراغ);	// copy env. from stack to heap
ساكن صحيح  moreenv(فراغ);	// incr. size of env.

    صحيح
putenv(ثابت حرف *string)
{
    صحيح	    i;
    حرف    *p;

    إذا (envsize < 0)
    {				// first time putenv called
	إذا (newenv() < 0)	// copy env. to heap
	    أرجع -1;
    }

    i = findenv((حرف *)string); // look for name in environment

    إذا (i < 0)
    {				// name must be added
	لكل (i = 0; environ[i]; i++);
	إذا (i >= (envsize - 1))
	{			// need new slot
	    إذا (moreenv() < 0)
		أرجع -1;
	}
	p = alloc(لڑی_لمبائی(string) + 1);
	إذا (p == عدم)		// not enough core
	    أرجع -1;
	environ[i + 1] = 0;	// new end of env.
    }
    وإلا
    {				// name already in env.
	p = vim_realloc(environ[i], لڑی_لمبائی(string) + 1);
	إذا (p == عدم)
	    أرجع -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    أرجع 0;
}

    ساكن صحيح
findenv(حرف *name)
{
    حرف    *namechar, *envchar;
    صحيح	    i, لبھیا;

    لبھیا = 0;
    لكل (i = 0; environ[i] && !لبھیا; i++)
    {
	envchar = environ[i];
	namechar = name;
	طالما (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	لبھیا = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    أرجع لبھیا ? i - 1 : -1;
}

    ساكن صحيح
newenv(فراغ)
{
    حرف    **env, *elem;
    صحيح	    i, esize;

    لكل (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(حرف *, esize);
    إذا (env == عدم)
	أرجع -1;

    لكل (i = 0; environ[i]; i++)
    {
	elem = alloc(لڑی_لمبائی(environ[i]) + 1);
	إذا (elem == عدم)
	    أرجع -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    أرجع 0;
}

    ساكن صحيح
moreenv(فراغ)
{
    صحيح	    esize;
    حرف    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((حرف *)environ, esize * حجم (*env));
    إذا (env == 0)
	أرجع -1;
    environ = env;
    envsize = esize;
    أرجع 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(ثابت char_u *string)
{
    صحيح i;
    char_u *p;

    إذا (envsize < 0)
	أرجع عدم;

    i = findenv((حرف *)string);

    إذا (i < 0)
	أرجع عدم;

    p = vim_strchr((char_u *)environ[i], '=');
    أرجع (p + 1);
}
# endif

#جے_انت // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#إذا defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    صحيح
filewritable(char_u *fname)
{
    صحيح		retval = 0;
# إذا defined(UNIX) || defined(VMS)
    صحيح		perm = 0;
# endif

# إذا defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    إذا (
# ifdef MSWIN
	    mch_writable(fname) &&
# وإلا
#  إذا defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((حرف *)fname, W_OK) == 0
       )
    {
	++retval;
	إذا (mch_isdir(fname))
	    ++retval;
    }
    أرجع retval;
}
#جے_انت

#إذا defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    صحيح
get2c(FILE *fd)
{
    صحيح		c, n;

    n = getc(fd);
    إذا (n == EOF) أرجع -1;
    c = getc(fd);
    إذا (c == EOF) أرجع -1;
    أرجع (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    صحيح
get3c(FILE *fd)
{
    صحيح		c, n;

    n = getc(fd);
    إذا (n == EOF) أرجع -1;
    c = getc(fd);
    إذا (c == EOF) أرجع -1;
    n = (n << 8) + c;
    c = getc(fd);
    إذا (c == EOF) أرجع -1;
    أرجع (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    صحيح
get4c(FILE *fd)
{
    صحيح		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    بنانشانی	n;

    c = getc(fd);
    إذا (c == EOF) أرجع -1;
    n = (بنانشانی)c;
    c = getc(fd);
    إذا (c == EOF) أرجع -1;
    n = (n << 8) + (بنانشانی)c;
    c = getc(fd);
    إذا (c == EOF) أرجع -1;
    n = (n << 8) + (بنانشانی)c;
    c = getc(fd);
    إذا (c == EOF) أرجع -1;
    n = (n << 8) + (بنانشانی)c;
    أرجع (صحيح)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, صحيح cnt)
{
    char_u	*str;
    صحيح		i;
    صحيح		c;

    // allocate memory
    str = alloc(cnt + 1);
    إذا (str == عدم)
	أرجع عدم;

    // Read the string.  Quit when running into the EOF.
    لكل (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	إذا (c == EOF)
	{
	    vim_free(str);
	    أرجع عدم;
	}
	str[i] = c;
    }
    str[i] = NUL;
    أرجع str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    صحيح
put_بائٹاں(FILE *fd, long_u nr, صحيح len)
{
    صحيح	    i;

    لكل (i = len - 1; i >= 0; --i)
	إذا (putc((صحيح)(nr >> (i * 8)), fd) == EOF)
	    أرجع FAIL;
    أرجع OK;
}

#جے_انت

#جے_بنا_تعریف PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    لما
elapsed(بنية timeval *شروع_tv)
{
    بنية timeval  now_tv;

    gettimeofday(&now_tv, عدم);
    أرجع (now_tv.tv_sec - شروع_tv->tv_sec) * 1000L
	 + (now_tv.tv_usec - شروع_tv->tv_usec) / 1000L;
}
# endif

# ifdef ELAPSED_TICKCOUNT
/*
 * Return time in msec since "start_tick".
 */
    لما
elapsed(DWORD شروع_tick)
{
    DWORD	now = Get_Tick_گنتی();

    أرجع (لما)now - (لما)شروع_tick;
}
# endif
#جے_انت

#إذا defined(FEAT_کم_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    صحيح
mch_parse_cmd(char_u *cmd, صحيح use_shcf, حرف ***argv, صحيح *argc)
{
    صحيح		i;
    char_u	*p, *d;
    صحيح		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    لكل (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	طالما (*p != NUL)
	{
	    إذا (i == 2)
		(*argv)[*argc] = (حرف *)p;
	    ++*argc;
	    d = p;
	    طالما (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		إذا (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		وإلا
		{
		    إذا (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    إذا (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    إذا (*p == NUL)
	    {
		إذا (i == 2)
		    *d++ = NUL;
		اكسر;
	    }
	    إذا (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	إذا (*argv == عدم)
	{
	    إذا (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		لكل (;;)
		{
		    p = skiptowhite(p);
		    إذا (*p == NUL)
			اكسر;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(حرف *, *argc + 4);
	    إذا (*argv == عدم)	    // out of memory
		أرجع FAIL;
	}
    }
    أرجع OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    صحيح
build_argv_from_string(char_u *cmd, حرف ***argv, صحيح *argc)
{
    char_u	*cmd_copy;
    صحيح		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    إذا (cmd_copy == عدم
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	أرجع FAIL;
    }
    لكل (i = 0; i < *argc; i++)
	(*argv)[i] = (حرف *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = عدم;
    vim_free(cmd_copy);
    أرجع OK;
}

#جے_انت

#إذا defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    صحيح
build_argv_from_list(list_T *l, حرف ***argv, صحيح *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(حرف *, l->lv_len + 1);
    إذا (*argv == عدم)
	أرجع FAIL;
    *argc = 0;
    FOR_ALL_LIST_اشیاء(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	إذا (s == عدم)
	{
	    صحيح i;

	    لكل (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = عدم;
	    أرجع FAIL;
	}
	(*argv)[*argc] = (حرف *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = عدم;
    أرجع OK;
}
#جے_انت

/*
 * Change the behavior of vterm.
 * 0: As usual.
 * 1: Windows 10 version 1809
 *      The bug causes unstable handling of ambiguous width character.
 * 2: Windows 10 version 1903 & 1909
 *      Use the wrong result because each result is different.
 * 3: Windows 10 insider preview (current latest logic)
 */
    صحيح
get_special_pty_type(فراغ)
{
#جے_تعریف_شدہ MSWIN
    أرجع get_conpty_type();
#وإلا
    أرجع 0;
#جے_انت
}

// compare two keyvalue_T structs by case sensitive value
    صحيح
cmp_keyvalue_قدر(ثابت فراغ *a, ثابت فراغ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    أرجع STRCMP(kv1->قدر.string, kv2->قدر.string);
}

// compare two keyvalue_T structs by value with length
    صحيح
cmp_keyvalue_قدر_n(ثابت فراغ *a, ثابت فراغ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    أرجع STRNCMP(kv1->قدر.string, kv2->قدر.string, MAX(kv1->قدر.لمائی,
		kv2->قدر.لمائی));
}

// compare two keyvalue_T structs by case insensitive value
    صحيح
cmp_keyvalue_قدر_i(ثابت فراغ *a, ثابت فراغ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    أرجع STRICMP(kv1->قدر.string, kv2->قدر.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    صحيح
cmp_keyvalue_قدر_ni(ثابت فراغ *a, ثابت فراغ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    أرجع vim_strnicmp_asc((حرف *)kv1->قدر.string,
	    (حرف *)kv2->قدر.string, MAX(kv1->قدر.لمائی,
		    kv2->قدر.لمائی));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    فراغ *
mergesort_list(
    فراغ	*سر,
    فراغ	*(*get_اگلا)(فراغ *),
    فراغ	(*سیٹ_اگلا)(فراغ *, فراغ *),
    فراغ	*(*get_prev)(فراغ *),
    فراغ	(*سیٹ_prev)(فراغ *, فراغ *),
    صحيح		(*compare)(ثابت فراغ *, ثابت فراغ *))
{
    إذا (!سر || !get_اگلا(سر))
	أرجع سر;

    // Count length
    صحيح	    n = 0;
    فراغ*   curr = سر;
    طالما (curr)
    {
	n++;
	curr = get_اگلا(curr);
    }

    صحيح	ناپ;
    لكل (ناپ = 1; ناپ < n; ناپ *= 2)
    {
	فراغ*	new_سر = عدم;
	فراغ*	پوچھ = عدم;
	curr = سر;

	طالما (curr)
	{
	    // Split two runs
	    فراغ    *بایاں = curr;
	    فراغ    *دایاں = بایاں;
	    صحيح	    i;
	    لكل (i = 0; i < ناپ && دایاں; ++i)
		دایاں = get_اگلا(دایاں);

	    فراغ    *اگلا = دایاں;
	    لكل (i = 0; i < ناپ && اگلا; ++i)
		اگلا = get_اگلا(اگلا);

	    // Break links
	    فراغ    *l_سرا = دایاں ? get_prev(دایاں) : عدم;
	    إذا (l_سرا)
		سیٹ_اگلا(l_سرا, عدم);
	    إذا (دایاں)
		سیٹ_prev(دایاں, عدم);

	    فراغ    *r_سرا = اگلا ? get_prev(اگلا) : عدم;
	    إذا (r_سرا)
		سیٹ_اگلا(r_سرا, عدم);
	    إذا (اگلا)
		سیٹ_prev(اگلا, عدم);

	    //ملاؤ
	    فراغ    *merged = عدم;
	    فراغ    *merged_پوچھ = عدم;

	    طالما (بایاں || دایاں)
	    {
		فراغ	*chosen = عدم;
		إذا (!بایاں)
		{
		    chosen = دایاں;
		    دایاں = get_اگلا(دایاں);
		}
		وإلا إذا (!دایاں)
		{
		    chosen = بایاں;
		    بایاں = get_اگلا(بایاں);
		}
		وإلا إذا (compare(بایاں, دایاں) <= 0)
		{
		    chosen = بایاں;
		    بایاں = get_اگلا(بایاں);
		}
		وإلا
		{
		    chosen = دایاں;
		    دایاں = get_اگلا(دایاں);
		}

		إذا (merged_پوچھ)
		{
		    سیٹ_اگلا(merged_پوچھ, chosen);
		    سیٹ_prev(chosen, merged_پوچھ);
		    merged_پوچھ = chosen;
		}
		وإلا
		{
		    merged = merged_پوچھ = chosen;
		    سیٹ_prev(chosen, عدم);
		}
	    }

	    // Connect to full list
	    إذا (!new_سر)
		new_سر = merged;
	    وإلا
	    {
		سیٹ_اگلا(پوچھ, merged);
		سیٹ_prev(merged, پوچھ);
	    }

	    // Move tail to end
	    طالما (get_اگلا(merged_پوچھ))
		merged_پوچھ = get_اگلا(merged_پوچھ);
	    پوچھ = merged_پوچھ;

	    curr = اگلا;
	}

	سر = new_سر;
    }

    أرجع سر;
}

ساكن ثابت char_u base64_جدول[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

ساكن char_u base64_dec_جدول[256];

#تعریف_کرو BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    ساكن فراغ
init_base64_dec_جدول(فراغ)
{
    ساكن صحيح base64_dec_tbl_initialized = FALSE;

    إذا (base64_dec_tbl_initialized)
	أرجع;

    vim_memset(base64_dec_جدول, 0xFF, حجم(base64_dec_جدول));
    لكل (size_t i = 0; i < حجم(base64_جدول) - 1; i++)
	base64_dec_جدول[(char_u)base64_جدول[i]] = (char_u)i;
    base64_dec_جدول['='] = 0;

    base64_dec_tbl_initialized = TRUE;
}

/*
 * Base64 encode "src[len]" into caller-supplied "dst", which must have room for
 * at least BASE64_ENCODED_LEN(len) bytes (NUL terminated). Returns the number
 * of bytes written, excluding the NUL.
 */
    لما
base64_encode_buf(char_u *dst, ثابت char_u *src, size_t len)
{
    size_t i, j;

    لكل (i = 0, j = 0; i < len;)
    {
	size_t بچیا = len - i;

	int_u octet_a = src[i++];
	int_u octet_b = بچیا > 1 ? src[i++] : 0;
	int_u octet_c = بچیا > 2 ? src[i++] : 0;

	int_u triple = (octet_a << 16) | (octet_b << 8) | octet_c;

	dst[j++] = base64_جدول[(triple >> 18) & 0x3F];
	dst[j++] = base64_جدول[(triple >> 12) & 0x3F];
	dst[j++] = بچیا > 1
	    ? base64_جدول[(triple >> 6) & 0x3F]
	    : '=';
	dst[j++] = بچیا > 2
	    ? base64_جدول[triple & 0x3F]
	    : '=';
    }

    dst[j] = NUL;
    أرجع (لما)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(ثابت char_u *انکڑے, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    إذا (encoded == عدم)
	أرجع عدم;
    base64_encode_buf(encoded, انکڑے, len);
    أرجع encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    صحيح
base64_decode(ثابت char_u *انکڑے, size_t len, garray_T *باہرلا)
{
    إذا (len == 0)
	أرجع OK;

    إذا (len % 4 != 0)
    {
	semsg(_(e_invalid_دلیل_str), انکڑے);
	أرجع FAIL;
    }

    init_base64_dec_جدول();

    size_t decoded_len = (len / 4) * 3;
    إذا (انکڑے[len - 1] == '=')
	decoded_len--;
    إذا (انکڑے[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    لكل (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_جدول[(char_u)انکڑے[i++]];
	int_u sextet_b = base64_dec_جدول[(char_u)انکڑے[i++]];
	int_u sextet_c = base64_dec_جدول[(char_u)انکڑے[i++]];
	int_u sextet_d = base64_dec_جدول[(char_u)انکڑے[i++]];

	إذا (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_دلیل_str), انکڑے);
	    ga_clear(باہرلا);
	    أرجع FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	إذا (j < decoded_len)
	{
	    ga_append(باہرلا, (triple >> 16) & 0xFF);
	    j++;
	}
	إذا (j < decoded_len)
	{
	    ga_append(باہرلا, (triple >> 8) & 0xFF);
	    j++;
	}
	إذا (j < decoded_len)
	{
	    ga_append(باہرلا, triple & 0xFF);
	    j++;
	}

	إذا (j == decoded_len)
	{
	    إذا ((انکڑے[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((انکڑے[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_دلیل_str), انکڑے);
		ga_clear(باہرلا);
		أرجع FAIL;
	    }
	}
    }
    أرجع OK;
}
