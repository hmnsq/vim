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
#شامل "vim.h"

ساکن char_u	*username = تہی; // cached result of mch_get_user_name()

ساکن صحیح coladvance2(pos_T *pos, صحیح addspaces, صحیح finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    صحیح
virtual_فعال(خالی)
{
    غیرعلامتی صحیح cur_ve_flags = get_ve_flags();

    اگر (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_داخل_کرو) && (State & منوال_داخل_کرو)))
	واپس TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    اگر (virtual_op != MAYBE)
	واپس virtual_op;
    واپس (cur_ve_flags & VE_بلاک) && VIsual_فعال && VIsual_منوال == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    صحیح
getviscol(خالی)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, تہی, تہی, GETVCOL_NO_ABOVE);
    واپس (صحیح)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    صحیح
coladvance_force(colnr_T wcol)
{
    صحیح rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    اگر (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ورنہ
	// Virtcol is valid
	سیٹ_valid_virtcol(curwin, wcol);
    واپس rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    صحیح
getviscol2(colnr_T ستون, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.ستون = ستون;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, تہی, تہی, GETVCOL_NO_ABOVE);
    واپس (صحیح)x;
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
    صحیح
coladvance(colnr_T wantcol)
{
    صحیح rc = getvpos(&curwin->w_cursor, wantcol);

    اگر (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ورنہ اگر (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	سیٹ_valid_virtcol(curwin, wantcol);
    واپس rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    صحیح
getvpos(pos_T *pos, colnr_T wantcol)
{
    واپس coladvance2(pos, FALSE, virtual_فعال(), wantcol);
}

    ساکن صحیح
coladvance2(
    pos_T	*pos,
    صحیح		addspaces,	// change the text to achieve our goal?
    صحیح		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    صحیح		idx;
    char_u	*سطر;
    صحیح		linelen;
    colnr_T	ستون = 0;
    صحیح		csize = 0;
    صحیح		one_مزید;
#اگر_تعریف_شدہ FEAT_LINEBREAK
    صحیح		سر = 0;
#اگر_ختم

    one_مزید = (State & منوال_داخل_کرو)
		    || restart_edit != NUL
		    || (VIsual_فعال && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    سطر = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    اگر (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_مزید;
	    ستون = wcol;

	    اگر ((addspaces || finetune) && !VIsual_فعال)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_مزید;
		اگر (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    ورنہ
    {
	صحیح		چوڑائی = curwin->w_چوڑائی - win_ستون_off(curwin);
	chartabsize_T	cts;

	اگر (finetune
		&& curwin->w_p_wrap
		&& curwin->w_چوڑائی != 0
		&& wcol >= (colnr_T)چوڑائی
		&& چوڑائی > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    اگر (csize > 0)
		csize--;

	    اگر (wcol / چوڑائی > (colnr_T)csize / چوڑائی
		    && ((State & منوال_داخل_کرو) == 0 || (صحیح)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / چوڑائی + 1) * چوڑائی - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, سطر, سطر);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	cts.cts_no_above = true;
#اگر_ختم
	جب (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#اگر_تعریف_شدہ FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &سر, تہی);
	    MB_PTR_ADV(cts.cts_ptr);
#ورنہ
	    csize = lbr_chartabsize_adv(&cts);
#اگر_ختم
	    cts.cts_vcol += csize;
	}
	ستون = cts.cts_vcol;
	idx = (صحیح)(cts.cts_ptr - سطر);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	اگر (ستون > wcol || (!virtual_فعال() && one_مزید == 0))
	{
	    idx -= 1;
#اگر_تعریف_شدہ FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= سر;
#اگر_ختم
	    ستون -= csize;
	}

	اگر (virtual_فعال()
		&& addspaces
		&& wcol >= 0
		&& ((ستون != wcol && ستون != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    اگر (سطر[idx] == NUL)
	    {
		// Append spaces
		صحیح	correct = wcol - ستون;
		char_u	*newline = alloc(idx + correct + 1);
		صحیح	t;

		اگر (newline == تہی)
		    واپس FAIL;

		برائے (t = 0; t < idx; ++t)
		    newline[t] = سطر[t];

		برائے (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		بدلا_بائٹیں(pos->lnum, (colnr_T)idx);
		idx += correct;
		ستون = wcol;
	    }
	    ورنہ
	    {
		// Break a tab
		صحیح	correct = wcol - ستون - csize + 1; // negative!!
		char_u	*newline;
		صحیح	t, s = 0;
		صحیح	v;

		اگر (-correct > csize)
		    واپس FAIL;

		newline = alloc(linelen + csize);
		اگر (newline == تہی)
		    واپس FAIL;

		برائے (t = 0; t < linelen; t++)
		{
		    اگر (t != idx)
			newline[s++] = سطر[t];
		    ورنہ
			برائے (v = 0; v < csize; v++)
			    newline[s++] = ' ';
		}

		newline[linelen + csize - 1] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		بدلا_بائٹیں(pos->lnum, idx);
		idx += (csize - 1 + correct);
		ستون += correct;
	    }
	}
    }

    اگر (idx < 0)
	pos->ستون = 0;
    ورنہ
	pos->ستون = idx;

    pos->coladd = 0;

    اگر (finetune)
    {
	اگر (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    اگر (!one_مزید)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, تہی, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	ورنہ
	{
	    صحیح b = (صحیح)wcol - (صحیح)ستون;

	    // The difference between wcol and col is used to set coladd.
	    اگر (b > 0 && b < (MAXCOL - 2 * curwin->w_چوڑائی))
		pos->coladd = b;

	    ستون += b;
	}
    }

    // prevent from moving onto a trail byte
    اگر (has_mbyte)
	mb_adjustpos(curbuf, pos);

    اگر (wcol < 0 || ستون < wcol)
	واپس FAIL;
    واپس OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    صحیح
inc_cursor(خالی)
{
    واپس inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    صحیح
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    اگر (lp->ستون != MAXCOL)
    {
	p = ml_get_pos(lp);
	اگر (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    اگر (has_mbyte)
	    {
		صحیح l = (*mb_ptr2len)(p);

		lp->ستون += l;
		واپس ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->ستون++;
	    lp->coladd = 0;
	    واپس ((p[1] != NUL) ? 0 : 2);
	}
    }
    اگر (lp->lnum != curbuf->b_ml.ml_سطر_گنتی)     // there is a next line
    {
	lp->ستون = 0;
	lp->lnum++;
	lp->coladd = 0;
	واپس 1;
    }
    واپس -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    صحیح
incl(pos_T *lp)
{
    صحیح	    r;

    اگر ((r = inc(lp)) >= 1 && lp->ستون)
	r = inc(lp);
    واپس r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    صحیح
dec_cursor(خالی)
{
    واپس dec(&curwin->w_cursor);
}

    صحیح
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    اگر (lp->ستون == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->ستون = ml_get_len(lp->lnum);
	اگر (has_mbyte)
	    lp->ستون -= (*mb_سر_off)(p, p + lp->ستون);
	واپس 0;
    }

    اگر (lp->ستون > 0)
    {
	// still within line
	lp->ستون--;
	اگر (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->ستون -= (*mb_سر_off)(p, p + lp->ستون);
	}
	واپس 0;
    }

    اگر (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->ستون = ml_get_len(lp->lnum);
	اگر (has_mbyte)
	    lp->ستون -= (*mb_سر_off)(p, p + lp->ستون);
	واپس 1;
    }

    // at start of file
    واپس -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    صحیح
decl(pos_T *lp)
{
    صحیح	    r;

    اگر ((r = dec(lp)) == 1 && lp->ستون)
	r = dec(lp);
    واپس r;
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

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (hasAnyFolding(wp))
    {
	اگر (lnum > cursor)
	{
	    جب (lnum > cursor)
	    {
		(خالی)hasFoldingWin(wp, lnum, &lnum, تہی, TRUE, تہی);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		اگر (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	ورنہ اگر (lnum < cursor)
	{
	    جب (lnum < cursor)
	    {
		(خالی)hasFoldingWin(wp, lnum, تہی, &lnum, TRUE, تہی);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		اگر (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    ورنہ
#اگر_ختم
	retval = lnum - cursor;

    واپس retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    خالی
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    اگر (pos->lnum > buf->b_ml.ml_سطر_گنتی)
	pos->lnum = buf->b_ml.ml_سطر_گنتی;

    اگر (pos->ستون > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	اگر (pos->ستون > len)
	    pos->ستون = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    خالی
check_cursor_lnum(خالی)
{
    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
#اگر_تعریف_شدہ FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	اگر (!hasFolding(curbuf->b_ml.ml_سطر_گنتی,
						&curwin->w_cursor.lnum, تہی))
#اگر_ختم
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
    }
    اگر (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    خالی
check_cursor_ستون(خالی)
{
    check_cursor_ستون_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    خالی
check_cursor_ستون_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.ستون;
    colnr_T      oldcoladd = win->w_cursor.ستون + win->w_cursor.coladd;
    غیرعلامتی صحیح cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_بفر, win->w_cursor.lnum);
    اگر (len == 0)
	win->w_cursor.ستون = 0;
    ورنہ اگر (win->w_cursor.ستون >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	اگر ((State & منوال_داخل_کرو) || restart_edit
		|| (VIsual_فعال && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_فعال())
	    win->w_cursor.ستون = len;
	ورنہ
	{
	    win->w_cursor.ستون = len - 1;
	    // Move the cursor to the head byte.
	    اگر (has_mbyte)
		mb_adjustpos(win->w_بفر, &win->w_cursor);
	}
    }
    ورنہ اگر (win->w_cursor.ستون < 0)
	win->w_cursor.ستون = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    اگر (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    ورنہ اگر (cur_ve_flags == VE_ALL)
    {
	اگر (oldcoladd > win->w_cursor.ستون)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.ستون;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    اگر (win->w_cursor.ستون + 1 < len)
	    {
		صحیح cs, ce;

		getvcol(win, &win->w_cursor, &cs, تہی, &ce, 0);
		اگر (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	ورنہ
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    خالی
check_cursor(خالی)
{
    check_cursor_lnum();
    check_cursor_ستون();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    خالی
check_visual_pos(خالی)
{
    اگر (VIsual.lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	VIsual.lnum = curbuf->b_ml.ml_سطر_گنتی;
	VIsual.ستون = 0;
	VIsual.coladd = 0;
    }
    ورنہ
    {
	صحیح len = ml_get_len(VIsual.lnum);

	اگر (VIsual.ستون > len)
	{
	    VIsual.ستون = len;
	    VIsual.coladd = 0;
	}
    }
}

/*
 * Make sure curwin->w_cursor is not on the NUL at the end of the line.
 * Allow it when in Visual mode and 'selection' is not "old".
 */
    خالی
adjust_cursor_ستون(خالی)
{
    اگر (curwin->w_cursor.ستون > 0
	    && (!VIsual_فعال || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.ستون;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    صحیح
سیٹ_leftcol(colnr_T leftcol)
{
    صحیح		retval = FALSE;

    // Return quickly when there is no change.
    اگر (curwin->w_leftcol == leftcol)
	واپس FALSE;
    curwin->w_leftcol = leftcol;

    بدلا_cline_bef_curs();
    طویل lastcol = curwin->w_leftcol + curwin->w_چوڑائی - curwin_ستون_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    طویل siso = get_sidescrolloff_قدر();
    اگر (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    ورنہ اگر (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(خالی)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, تہی, &e, 0);
    اگر (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    ورنہ اگر (s < curwin->w_leftcol)
    {
	retval = TRUE;
	اگر (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    بدلا_cline_bef_curs();
	}
    }

    اگر (retval)
	curwin->w_سیٹ_curswant = true;
    redraw_later(UPD_NOT_VALID);
    واپس retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    صحیح
copy_option_part(
    char_u	**option,
    char_u	*buf,
    صحیح		maxlen,
    حرف	*sep_chars)
{
    صحیح	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    اگر (*p == '.')
	buf[len++] = *p++;
    جب (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == تہی)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	اگر (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != تہی)
	    ++p;
	اگر (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    اگر (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_مقصد_option_part(p);	// p points to next file name

    *option = p;
    واپس len;
}

#اگر !defined(HAVE_MEMSET) && !defined(PROTO)
    خالی *
vim_memset(خالی *ptr, صحیح c, size_t سائز)
{
    حرف *p = ptr;

    جب (سائز-- > 0)
	*p++ = c;
    واپس ptr;
}
#اگر_ختم

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    صحیح
vim_isspace(صحیح x)
{
    واپس ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

ساکن ساخت modmasktable
{
    مختصر	mod_نقاب;	// Bit-mask for particular key modifier
    مختصر	mod_flag;	// Bit(s) for particular key modifier
    char_u	name;		// Single letter name of modifier
} mod_نقاب_جدول[] =
{
    {MOD_نقاب_ALT,		MOD_نقاب_ALT,		(char_u)'M'},
    {MOD_نقاب_META,		MOD_نقاب_META,		(char_u)'T'},
    {MOD_نقاب_CTRL,		MOD_نقاب_CTRL,		(char_u)'C'},
    {MOD_نقاب_سرکاؤ,		MOD_نقاب_سرکاؤ,		(char_u)'S'},
    {MOD_نقاب_MULTI_CLICK,	MOD_نقاب_2_CLICK,	(char_u)'2'},
    {MOD_نقاب_MULTI_CLICK,	MOD_نقاب_3_CLICK,	(char_u)'3'},
    {MOD_نقاب_MULTI_CLICK,	MOD_نقاب_4_CLICK,	(char_u)'4'},
#اگر defined(MACOS_X) || defined(FEAT_GUI_GTK)
    {MOD_نقاب_CMD,		MOD_نقاب_CMD,		(char_u)'D'},
#اگر_ختم
    // 'A' must be the last one
    {MOD_نقاب_ALT,		MOD_نقاب_ALT,		(char_u)'A'},
    {0, 0, NUL}
    // NOTE: when adding an entry, update MAX_KEY_NAME_LEN!
};

/*
 * Shifted key terminal codes and their unshifted equivalent.
 * Don't add mouse codes here, they are handled separately!
 */
#تعریف_کرو MOD_کلیدیں_اندراج_سائز 5

ساکن char_u modifier_کلیدیں_جدول[] =
{
//  mod mask	    with modifier		without modifier
    MOD_نقاب_سرکاؤ, '&', '9',			'@', '1',	// begin
    MOD_نقاب_سرکاؤ, '&', '0',			'@', '2',	// cancel
    MOD_نقاب_سرکاؤ, '*', '1',			'@', '4',	// command
    MOD_نقاب_سرکاؤ, '*', '2',			'@', '5',	// copy
    MOD_نقاب_سرکاؤ, '*', '3',			'@', '6',	// create
    MOD_نقاب_سرکاؤ, '*', '4',			'k', 'D',	// delete char
    MOD_نقاب_سرکاؤ, '*', '5',			'k', 'L',	// delete line
    MOD_نقاب_سرکاؤ, '*', '7',			'@', '7',	// end
    MOD_نقاب_CTRL,  KS_EXTRA, (صحیح)KE_C_انتہا,	'@', '7',	// end
    MOD_نقاب_سرکاؤ, '*', '9',			'@', '9',	// exit
    MOD_نقاب_سرکاؤ, '*', '0',			'@', '0',	//ڈھونڈو
    MOD_نقاب_سرکاؤ, '#', '1',			'%', '1',	// help
    MOD_نقاب_سرکاؤ, '#', '2',			'k', 'h',	// home
    MOD_نقاب_CTRL,  KS_EXTRA, (صحیح)KE_C_HOME,	'k', 'h',	// home
    MOD_نقاب_سرکاؤ, '#', '3',			'k', 'I',	// insert
    MOD_نقاب_سرکاؤ, '#', '4',			'k', 'l',	// left arrow
    MOD_نقاب_CTRL,  KS_EXTRA, (صحیح)KE_C_بایاں,	'k', 'l',	// left arrow
    MOD_نقاب_سرکاؤ, '%', 'a',			'%', '3',	//پیغام
    MOD_نقاب_سرکاؤ, '%', 'b',			'%', '4',	// move
    MOD_نقاب_سرکاؤ, '%', 'c',			'%', '5',	//اگلا
    MOD_نقاب_سرکاؤ, '%', 'd',			'%', '7',	// options
    MOD_نقاب_سرکاؤ, '%', 'e',			'%', '8',	//پچھلا
    MOD_نقاب_سرکاؤ, '%', 'f',			'%', '9',	// print
    MOD_نقاب_سرکاؤ, '%', 'g',			'%', '0',	// redo
    MOD_نقاب_سرکاؤ, '%', 'h',			'&', '3',	// replace
    MOD_نقاب_سرکاؤ, '%', 'i',			'k', 'r',	// right arr.
    MOD_نقاب_CTRL,  KS_EXTRA, (صحیح)KE_C_دایاں,	'k', 'r',	// right arr.
    MOD_نقاب_سرکاؤ, '%', 'j',			'&', '5',	// resume
    MOD_نقاب_سرکاؤ, '!', '1',			'&', '6',	// save
    MOD_نقاب_سرکاؤ, '!', '2',			'&', '7',	// suspend
    MOD_نقاب_سرکاؤ, '!', '3',			'&', '8',	// undo
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_اوپر,	'k', 'u',	// up arrow
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_نیچے_کو,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_XF1,	KS_EXTRA, (صحیح)KE_XF1,
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_XF2,	KS_EXTRA, (صحیح)KE_XF2,
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_XF3,	KS_EXTRA, (صحیح)KE_XF3,
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_XF4,	KS_EXTRA, (صحیح)KE_XF4,

    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F1,	'k', '1',	// F1
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F2,	'k', '2',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F3,	'k', '3',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F4,	'k', '4',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F5,	'k', '5',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F6,	'k', '6',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F7,	'k', '7',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F8,	'k', '8',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F9,	'k', '9',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F10,	'k', ';',	// F10

    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F11,	'F', '1',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F12,	'F', '2',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F13,	'F', '3',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F14,	'F', '4',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F15,	'F', '5',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F16,	'F', '6',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F17,	'F', '7',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F18,	'F', '8',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F19,	'F', '9',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F20,	'F', 'A',

    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F21,	'F', 'B',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F22,	'F', 'C',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F23,	'F', 'D',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F24,	'F', 'E',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F25,	'F', 'F',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F26,	'F', 'G',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F27,	'F', 'H',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F28,	'F', 'I',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F29,	'F', 'J',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F30,	'F', 'K',

    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F31,	'F', 'L',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F32,	'F', 'M',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F33,	'F', 'N',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F34,	'F', 'O',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F35,	'F', 'P',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F36,	'F', 'Q',
    MOD_نقاب_سرکاؤ, KS_EXTRA, (صحیح)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_نقاب_سرکاؤ, 'k', 'B',			KS_EXTRA, (صحیح)KE_TAB,

    NUL
};

ساکن ساخت کلید_name_اندراج
{
    صحیح		enabled;	    // is this entry available (TRUE/FALSE)?
    صحیح		کلید;		    // special key code or ascii value
    string_T	name;		    // name of key
    صحیح		is_alt;		    // is an alternative name
} کلید_نام_جدول[] =
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
#اگر_تعریف_شدہ FEAT_MOUSE_DEC
    TRUE,
#ورنہ
    FALSE,
#اگر_ختم
	K_DEC_MOUSE, STR_LITERAL_INIT("DecMouse"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Del"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Delete"), TRUE},
    {TRUE, K_نیچے_کو, STR_LITERAL_INIT("Down"), FALSE},
    {TRUE, K_DROP, STR_LITERAL_INIT("Drop"), FALSE},
    {TRUE, K_انتہا, STR_LITERAL_INIT("End"), FALSE},
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
#اگر_تعریف_شدہ FEAT_MOUSE_JSB
    TRUE,
#ورنہ
    FALSE,
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_MOUSE_NET
    TRUE,
#ورنہ
    FALSE,
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_MOUSE_PTERM
    TRUE,
#ورنہ
    FALSE,
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_EVAL
    TRUE,
#ورنہ
    FALSE,
#اگر_ختم
	K_SNR, STR_LITERAL_INIT("SNR"), FALSE},
    {TRUE, ' ', STR_LITERAL_INIT("Space"), FALSE},
    {TRUE, TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_UNDO, STR_LITERAL_INIT("Undo"), FALSE},
    {TRUE, K_اوپر, STR_LITERAL_INIT("Up"), FALSE},
    {
#اگر_تعریف_شدہ FEAT_MOUSE_URXVT
    TRUE,
#ورنہ
    FALSE,
#اگر_ختم
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
    ساکن صحیح
name_مقصد_mod_نقاب(صحیح c)
{
    صحیح	    i;

    c = TOUPPER_ASC(c);
    برائے (i = 0; mod_نقاب_جدول[i].mod_نقاب != 0; i++)
	اگر (c == mod_نقاب_جدول[i].name)
	    واپس mod_نقاب_جدول[i].mod_flag;
    واپس 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    صحیح
simplify_کلید(صحیح کلید, صحیح *modifiers)
{
    صحیح	    i;
    صحیح	    key0;
    صحیح	    key1;

    اگر (!(*modifiers & (MOD_نقاب_سرکاؤ | MOD_نقاب_CTRL)))
	واپس کلید;

    // TAB is a special case
    اگر (کلید == TAB && (*modifiers & MOD_نقاب_سرکاؤ))
    {
	*modifiers &= ~MOD_نقاب_سرکاؤ;
	واپس K_S_TAB;
    }
    key0 = KEY2TERMCAP0(کلید);
    key1 = KEY2TERMCAP1(کلید);
    برائے (i = 0; modifier_کلیدیں_جدول[i] != NUL; i += MOD_کلیدیں_اندراج_سائز)
    {
	اگر (key0 == modifier_کلیدیں_جدول[i + 3]
		&& key1 == modifier_کلیدیں_جدول[i + 4]
		&& (*modifiers & modifier_کلیدیں_جدول[i]))
	{
	    *modifiers &= ~modifier_کلیدیں_جدول[i];
	    واپس TERMCAP2_کلید(modifier_کلیدیں_جدول[i + 1],
		    modifier_کلیدیں_جدول[i + 2]);
	}
    }
    واپس کلید;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    صحیح
handle_x_کلیدیں(صحیح کلید)
{
    انتخاب (کلید)
    {
	حالت K_XUP:	واپس K_اوپر;
	حالت K_XDOWN:	واپس K_نیچے_کو;
	حالت K_XLEFT:	واپس K_بایاں;
	حالت K_XRIGHT:	واپس K_دایاں;
	حالت K_XHOME:	واپس K_HOME;
	حالت K_ZHOME:	واپس K_HOME;
	حالت K_XEND:	واپس K_انتہا;
	حالت K_ZEND:	واپس K_انتہا;
	حالت K_XF1:	واپس K_F1;
	حالت K_XF2:	واپس K_F2;
	حالت K_XF3:	واپس K_F3;
	حالت K_XF4:	واپس K_F4;
	حالت K_S_XF1:	واپس K_S_F1;
	حالت K_S_XF2:	واپس K_S_F2;
	حالت K_S_XF3:	واپس K_S_F3;
	حالت K_S_XF4:	واپس K_S_F4;
    }
    واپس کلید;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_کلید_name(صحیح c, صحیح modifiers)
{
    ساکن char_u string[MAX_کلید_NAME_LEN + 1];
    صحیح	    i, idx, len;
    صحیح	    جدول_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    اگر (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_کلید)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    اگر (IS_SPECIAL(c))
    {
	برائے (i = 0; modifier_کلیدیں_جدول[i] != 0; i += MOD_کلیدیں_اندراج_سائز)
	    اگر (       KEY2TERMCAP0(c) == (صحیح)modifier_کلیدیں_جدول[i + 1]
		    && (صحیح)KEY2TERMCAP1(c) == (صحیح)modifier_کلیدیں_جدول[i + 2])
	    {
		modifiers |= modifier_کلیدیں_جدول[i];
		c = TERMCAP2_کلید(modifier_کلیدیں_جدول[i + 3],
						   modifier_کلیدیں_جدول[i + 4]);
		توڑو;
	    }
    }

    // try to find the key in the special key table
    جدول_idx = ڈھونڈو_special_کلید_اندر_جدول(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    اگر (c > 0 && (*mb_char2len)(c) == 1)
    {
	اگر (جدول_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_نقاب_ALT;
	    // try again, to find the un-alted key in the special key table
	    جدول_idx = ڈھونڈو_special_کلید_اندر_جدول(c);
	}
	اگر (جدول_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_نقاب_CTRL;
	}
    }

    // translate the modifier into a string
    برائے (i = 0; mod_نقاب_جدول[i].name != 'A'; i++)
	اگر ((modifiers & mod_نقاب_جدول[i].mod_نقاب)
						== mod_نقاب_جدول[i].mod_flag)
	{
	    string[idx++] = mod_نقاب_جدول[i].name;
	    string[idx++] = (char_u)'-';
	}

    اگر (جدول_idx < 0)		// unknown special key, may output t_xx
    {
	اگر (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	ورنہ
	{
	    len = (*mb_char2len)(c);
	    اگر (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    ورنہ اگر (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    ورنہ
	    {
		char_u	*s = transchar(c);
		جب (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    ورنہ		// use name of special key
    {
	string_T    *s;

	s = &کلید_نام_جدول[جدول_idx].name;

	اگر (s->لمبائی + idx + 2 <= MAX_کلید_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (صحیح)s->لمبائی;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    واپس string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    صحیح
trans_special(
    char_u	**srcp,
    char_u	*dst,
    صحیح		flags,		// FSK_ values
    صحیح		escape_ks,	// escape K_SPECIAL bytes in the character
    صحیح		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    صحیح		modifiers = 0;
    صحیح		کلید;

    کلید = ڈھونڈو_special_کلید(srcp, &modifiers, flags, did_simplify);
    اگر (کلید == 0)
	واپس 0;

    واپس special_مقصد_buf(کلید, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    صحیح
special_مقصد_buf(صحیح کلید, صحیح modifiers, صحیح escape_ks, char_u *dst)
{
    صحیح		dlen = 0;

    // Put the appropriate modifier in a string
    اگر (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    اگر (IS_SPECIAL(کلید))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(کلید);
	dst[dlen++] = KEY2TERMCAP1(کلید);
    }
    ورنہ اگر (escape_ks)
	dlen = (صحیح)(جوڑو_char2buf(کلید, dst + dlen) - dst);
    ورنہ اگر (has_mbyte)
	dlen += (*mb_char2bytes)(کلید, dst + dlen);
    ورنہ
	dst[dlen++] = کلید;

    واپس dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    صحیح
ڈھونڈو_special_کلید(
    char_u	**srcp,
    صحیح		*modp,
    صحیح		flags,		// FSK_ values
    صحیح		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*آخری_dash;
    char_u	*انتہا_کا_name;
    char_u	*src;
    char_u	*bp;
    صحیح		اندر_string = flags & FSK_اندر_STRING;
    صحیح		modifiers;
    صحیح		ثنائی_ہندسہ;
    صحیح		کلید;
    uvarnumber_T	n;
    صحیح		l;

    src = *srcp;
    اگر (src[0] != '<')
	واپس 0;
    اگر (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    آخری_dash = src;
    برائے (bp = src + 1; *bp == '-' || vim_is_معمول_IDc(*bp); bp++)
    {
	اگر (*bp == '-')
	{
	    آخری_dash = bp;
	    اگر (bp[1] != NUL)
	    {
		اگر (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		ورنہ
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		اگر (!(اندر_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		ورنہ اگر (اندر_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	اگر (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	ورنہ اگر (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, تہی, &l, STR2NR_ALL, تہی, تہی, 0, TRUE, تہی);
	    اگر (l == 0)
	    {
		emsg(_(e_invalid_دلیل));
		واپس 0;
	    }
	    bp += l + 5;
	    توڑو;
	}
    }

    اگر (*bp == '>')	// found matching '>'
    {
	انتہا_کا_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	برائے (bp = src + 1; bp < آخری_dash; bp++)
	{
	    اگر (*bp != '-')
	    {
		ثنائی_ہندسہ = name_مقصد_mod_نقاب(*bp);
		اگر (ثنائی_ہندسہ == 0x0)
		    توڑو;	// Illegal modifier name
		modifiers |= ثنائی_ہندسہ;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	اگر (bp >= آخری_dash)
	{
	    اگر (STRNICMP(آخری_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(آخری_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(آخری_dash + 6, تہی, &l, STR2NR_ALL, تہی,
							    &n, 0, TRUE, تہی);
		اگر (l == 0)
		{
		    emsg(_(e_invalid_دلیل));
		    واپس 0;
		}
		کلید = (صحیح)n;
	    }
	    ورنہ
	    {
		صحیح off = 1;

		// Modifier with single letter, or special key name.
		اگر (اندر_string && آخری_dash[1] == '\\' && آخری_dash[2] == '"')
		    off = 2;
		اگر (has_mbyte)
		    l = mb_ptr2len(آخری_dash + off);
		ورنہ
		    l = 1;
		اگر (modifiers != 0 && آخری_dash[l + off] == '>')
		    کلید = PTR2CHAR(آخری_dash + off);
		ورنہ
		{
		    کلید = get_special_کلید_رمز(آخری_dash + off);
		    اگر (!(flags & FSK_KEEP_X_کلید))
			کلید = handle_x_کلیدیں(کلید);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    اگر (کلید != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		کلید = simplify_کلید(کلید, &modifiers);

		اگر ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    اگر (کلید == K_BS)
			کلید = BS;
		    ورنہ اگر (کلید == K_DEL || کلید == K_KDEL)
			کلید = DEL;
		}
		ورنہ اگر (کلید == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    اگر ((flags & FSK_SIMPLIFY) != 0)
		    {
			اگر (did_simplify != تہی)
			    *did_simplify = TRUE;
		    }
		    ورنہ
			کلید = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		اگر (!IS_SPECIAL(کلید))
		    کلید = نکالو_modifiers(کلید, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = انتہا_کا_name;
		واپس کلید;
	    }
	}
    }
    واپس 0;
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
    صحیح
may_adjust_کلید_for_ctrl(صحیح modifiers, صحیح کلید)
{
    اگر ((modifiers & MOD_نقاب_CTRL) == 0)
	واپس کلید;

    اگر (ASCII_ISALPHA(کلید))
    {
#اگر_تعریف_شدہ FEAT_TERMINAL
	check_no_reduce_کلیدیں();  // may update the no_reduce_keys flag
#اگر_ختم
	واپس no_reduce_کلیدیں == 0 ? TOUPPER_ASC(کلید) : کلید;
    }
    اگر (کلید == '2')
	واپس '@';
    اگر (کلید == '6')
	واپس '^';
    اگر (کلید == '-')
	واپس '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    اگر (کلید == '$' && (modifiers & MOD_نقاب_سرکاؤ) == 0)
	واپس ']';

    واپس کلید;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    صحیح
may_remove_سرکاؤ_modifier(صحیح modifiers, صحیح کلید)
{
    اگر ((modifiers == MOD_نقاب_سرکاؤ
		|| modifiers == (MOD_نقاب_سرکاؤ | MOD_نقاب_ALT)
#اگر_تعریف_شدہ FEAT_GUI_GTK
		|| modifiers == (MOD_نقاب_سرکاؤ | MOD_نقاب_CMD)
#اگر_ختم
		|| modifiers == (MOD_نقاب_سرکاؤ | MOD_نقاب_META))
	    && ((کلید >= '!' && کلید <= '/')
		|| (کلید >= ':' && کلید <= 'Z')
		|| vim_isdigit(کلید)
		|| (کلید >= '[' && کلید <= '`')
		|| (کلید >= '{' && کلید <= '~')))
	واپس modifiers & ~MOD_نقاب_سرکاؤ;

    اگر (modifiers == (MOD_نقاب_سرکاؤ | MOD_نقاب_CTRL)
		&& (کلید == '{' || کلید == '}' || کلید == '|'))
	واپس modifiers & ~MOD_نقاب_سرکاؤ;

    واپس modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    صحیح
نکالو_modifiers(صحیح کلید, صحیح *modp, صحیح simplify, صحیح *did_simplify)
{
    صحیح	modifiers = *modp;

#اگر_تعریف_شدہ MACOS_X
    // Command-key really special, no fancynest
    اگر (!(modifiers & MOD_نقاب_CMD))
#اگر_ختم
    اگر ((modifiers & MOD_نقاب_سرکاؤ) && ASCII_ISALPHA(کلید))
    {
	کلید = TOUPPER_ASC(کلید);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	اگر (simplify || modifiers == MOD_نقاب_سرکاؤ
		|| modifiers == (MOD_نقاب_سرکاؤ | MOD_نقاب_ALT)
		|| modifiers == (MOD_نقاب_سرکاؤ | MOD_نقاب_META))
	    modifiers &= ~MOD_نقاب_سرکاؤ;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    اگر ((modifiers & MOD_نقاب_CTRL) && ASCII_ISALPHA(کلید))
	کلید = TOUPPER_ASC(کلید);

    اگر (simplify && (modifiers & MOD_نقاب_CTRL)
	    && ((کلید >= '?' && کلید <= '_') || ASCII_ISALPHA(کلید)))
    {
	کلید = Ctrl_chr(کلید);
	modifiers &= ~MOD_نقاب_CTRL;
	// <C-@> is <Nul>
	اگر (کلید == NUL)
	    کلید = K_ZERO;
	اگر (did_simplify != تہی)
	    *did_simplify = TRUE;
    }

#اگر_تعریف_شدہ MACOS_X
    // Command-key really special, no fancynest
    اگر (!(modifiers & MOD_نقاب_CMD))
#اگر_ختم
    اگر (simplify && (modifiers & MOD_نقاب_ALT) && کلید < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	کلید |= 0x80;
	modifiers &= ~MOD_نقاب_ALT;	// remove the META modifier
	اگر (did_simplify != تہی)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    واپس کلید;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    صحیح
ڈھونڈو_special_کلید_اندر_جدول(صحیح c)
{
    صحیح	    i;

    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(کلید_نام_جدول); i++)
	اگر (c == کلید_نام_جدول[i].کلید && !کلید_نام_جدول[i].is_alt)
	    واپس کلید_نام_جدول[i].enabled ? i : -1;

    واپس -1;
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
    ساکن صحیح
cmp_کلید_name_اندراج(ثابت خالی *a, ثابت خالی *b)
{
    char_u  *p1 = ((ساخت کلید_name_اندراج *)a)->name.string;
    char_u  *p2 = ((ساخت کلید_name_اندراج *)b)->name.string;
    صحیح	    نتیجہ = 0;

    اگر (p1 == p2)
	واپس 0;

    جب (vim_is_معمول_IDc(*p1) && *p2 != NUL)
    {
	اگر ((نتیجہ = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    توڑو;
	++p1;
	++p2;
    }

    اگر (نتیجہ == 0)
    {
	اگر (*p2 == NUL)
	{
	    اگر (vim_is_معمول_IDc(*p1))
		نتیجہ = 1;
	}
	ورنہ
	{
	    نتیجہ = -1;
	}
    }

    واپس نتیجہ;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    صحیح
get_special_کلید_رمز(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    اگر (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	اگر (جوڑو_termcap_اندراج(string, FALSE) == OK)
	    واپس TERMCAP2_کلید(name[2], name[3]);
    }
    ورنہ
    {
	ساخت کلید_name_اندراج	ہدف;
	ساخت کلید_name_اندراج	*اندراج_2;

	ہدف.enabled = TRUE;
	ہدف.کلید = 0;
	ہدف.name.string = name;
	ہدف.name.لمبائی = 0;

	اندراج_2 = (ساخت کلید_name_اندراج *)bsearch(
	    &ہدف,
	    &کلید_نام_جدول,
	    ARRAY_لمبائی(کلید_نام_جدول),
	    حجم(کلید_نام_جدول[0]),
	    cmp_کلید_name_اندراج);
	اگر (اندراج_2 != تہی && اندراج_2->enabled)
	{
	    صحیح کلید = اندراج_2->کلید;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    واپس کلید == K_TAB ? TAB : کلید;
	}
    }

    واپس 0;
}

    char_u *
get_کلید_name(صحیح i)
{
    اگر (i < 0 || i >= (صحیح)ARRAY_لمبائی(کلید_نام_جدول))
	واپس تہی;

    واپس کلید_نام_جدول[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    صحیح
get_fileformat(buf_T *buf)
{
    صحیح		c = *buf->b_p_ff;

    اگر (buf->b_p_bin || c == 'u')
	واپس EOL_UNIX;
    اگر (c == 'm')
	واپس EOL_MAC;
    واپس EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    صحیح
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    صحیح		c;

    اگر (eap != تہی && eap->force_ff != 0)
	c = eap->force_ff;
    ورنہ
    {
	اگر ((eap != تہی && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    واپس EOL_UNIX;
	c = *buf->b_p_ff;
    }
    اگر (c == 'u')
	واپس EOL_UNIX;
    اگر (c == 'm')
	واپس EOL_MAC;
    واپس EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    خالی
سیٹ_fileformat(
    صحیح		t,
    صحیح		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    حرف	*p = تہی;

    انتخاب (t)
    {
    حالت EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	توڑو;
    حالت EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	توڑو;
    حالت EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	توڑو;
    }
    اگر (p != تہی)
	سیٹ_string_option_براہ_راست((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_حالت(curbuf);
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    صحیح
default_fileformat(خالی)
{
    انتخاب (*p_ffs)
    {
	حالت 'm':   واپس EOL_MAC;
	حالت 'd':   واپس EOL_DOS;
    }
    واپس EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    صحیح
call_shell(char_u *cmd, صحیح opt)
{
    صحیح		retval;
#اگر_تعریف_شدہ FEAT_PROFILE
    proftime_T	wait_وقت;
#اگر_ختم

    اگر (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == تہی ? p_sh : cmd);
	پیغام_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	prof_بچہ_enter(&wait_وقت);
#اگر_ختم

    اگر (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_خالی_خانے));
	retval = -1;
    }
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
	// Don't hide the pointer while executing a shell command.
	gui_mch_mousehide(FALSE);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
	++hold_gui_events;
#اگر_ختم
	// The external command may update a tags file, clear cached tags.
	لیبل_freematch();

	اگر (cmd == تہی || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	ورنہ
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    اگر (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		اگر (ecmd == تہی)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    اگر (ncmd != تہی)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((حرف *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    ورنہ
		retval = -1;
	    اگر (ecmd != cmd)
		vim_free(ecmd);
	}
#اگر_تعریف_شدہ FEAT_GUI
	--hold_gui_events;
#اگر_ختم
	/*
	 * Check the window size, in case it changed while executing the
	 * external command.
	 */
	shell_resized_check();
    }

#اگر_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_nr(VV_SHELL_ERROR, (طویل)retval);
# ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	prof_بچہ_exit(&wait_وقت);
# endif
#اگر_ختم

    واپس retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    صحیح
get_real_state(خالی)
{
    اگر (State & منوال_معمول)
    {
	اگر (VIsual_فعال)
	{
	    اگر (VIsual_select)
		واپس منوال_SELECT;
	    واپس منوال_VISUAL;
	}
	ورنہ اگر (اختتام_op)
	    واپس منوال_OP_PENDING;
    }
    واپس State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    صحیح
after_pathsep(char_u *b, char_u *p)
{
    واپس p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_سر_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    صحیح
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    اگر (f1 == تہی || f2 == تہی)
	واپس FALSE;

    (خالی)vim_بھرا_Name(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    واپس (t1 - ffname == t2 - f2
	     && pathcmp((حرف *)ffname, (حرف *)f2, (صحیح)(t1 - ffname)) == 0);
}

#اگر defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    صحیح
vim_chdirfile(char_u *fname, حرف *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    اگر (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    اگر (pathcmp((حرف *)old_dir, (حرف *)new_dir, -1) == 0)
	// nothing to do
	واپس OK;

    اگر (trigger_autocmd != تہی)
	trigger_Dir_بدلا_Pre((char_u *)trigger_autocmd, new_dir);

    اگر (mch_chdir((حرف *)new_dir) != 0)
	واپس FAIL;

    اگر (trigger_autocmd != تہی)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    واپس OK;
}
#اگر_ختم

#اگر defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    ساکن صحیح
illegal_slash(ثابت حرف *name)
{
    اگر (name[0] == NUL)
	واپس FALSE;	    // no file name is not illegal
    اگر (name[لڑی_لمبائی(name) - 1] != '/')
	واپس FALSE;	    // no trailing slash
    اگر (mch_isdir((char_u *)name))
	واپس FALSE;	    // trailing slash for a directory
    واپس TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    صحیح
vim_stat(ثابت حرف *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    واپس illegal_slash(name) ? -1 : stat(name, stp);
}
#اگر_ختم

#اگر defined(CURSOR_SHAPE)

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
ساکن string_T mshape_نام[] =
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
    {تہی, 0}
};

#  define MSHAPE_نام_گنتی  (ARRAY_لمبائی(mshape_نام) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    حرف *
parse_shape_opt(صحیح what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    صحیح		idx = 0;		// init for GCC
    صحیح		all_idx;
    صحیح		len;
    صحیح		i;
    طویل	n;
    صحیح		ملا_ve = FALSE;	// found "ve" flag
    صحیح		باری;

    /*
     * First round: check for errors; second round: do it for real.
     */
    برائے (باری = 1; باری <= 2; ++باری)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	اگر (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	ورنہ
# endif
	    modep = p_guicursor;
	جب (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    اگر (colonp == تہی || (commap != تہی && commap < colonp))
		واپس e_missing_colon_2;
	    اگر (colonp == modep)
		واپس e_illegal_منوال;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    جب (modep < colonp || all_idx >= 0)
	    {
		اگر (all_idx < 0)
		{
		    // Find the mode.
		    اگر (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    ورنہ
			len = 2;
		    اگر (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_گنتی - 1;
		    ورنہ
		    {
			برائے (idx = 0; idx < SHAPE_IDX_گنتی; ++idx)
			    اگر (STRNICMP(modep, shape_جدول[idx].name, len)
									 == 0)
				توڑو;
			اگر (idx == SHAPE_IDX_گنتی
				   || (shape_جدول[idx].مستعمل_for & what) == 0)
			    واپس e_illegal_منوال;
			اگر (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    ملا_ve = TRUE;
		    }
		    modep += len + 1;
		}

		اگر (all_idx >= 0)
		    idx = all_idx--;
		ورنہ اگر (باری == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    اگر (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_جدول[idx].mshape = 0;
		    }
		    ورنہ
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
		برائے (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    اگر (what == SHAPE_MOUSE)
		    {
			برائے (i = 0; ; ++i)
			{
			    اگر (mshape_نام[i].string == تہی)
			    {
				اگر (!VIM_ISDIGIT(*p))
				    واپس e_illegal_mouseshape;
				اگر (باری == 2)
				    shape_جدول[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				ورنہ
				    (خالی)getdigits(&p);
				توڑو;
			    }
			    اگر (STRNICMP(p, mshape_نام[i].string, mshape_نام[i].لمبائی) == 0)
			    {
				اگر (باری == 2)
				    shape_جدول[idx].mshape = i;
				p += mshape_نام[i].لمبائی;
				توڑو;
			    }
			}
		    }
		    ورنہ // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			اگر (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			ورنہ اگر (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			ورنہ اگر (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			ورنہ اگر (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			ورنہ اگر (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			اگر (len != 0)
			{
			    p += len;
			    اگر (!VIM_ISDIGIT(*p))
				واپس e_digit_expected;
			    n = getdigits(&p);
			    اگر (len == 3)   // "ver" or "hor"
			    {
				اگر (n == 0)
				    واپس e_illegal_percentage;
				اگر (باری == 2)
				{
				    اگر (TOLOWER_ASC(i) == 'v')
					shape_جدول[idx].shape = SHAPE_VER;
				    ورنہ
					shape_جدول[idx].shape = SHAPE_HOR;
				    shape_جدول[idx].percentage = n;
				}
			    }
			    ورنہ اگر (باری == 2)
			    {
				اگر (len == 9)
				    shape_جدول[idx].blinkwait = n;
				ورنہ اگر (len == 7)
				    shape_جدول[idx].blinkon = n;
				ورنہ
				    shape_جدول[idx].blinkoff = n;
			    }
			}
			ورنہ اگر (STRNICMP(p, "block", 5) == 0)
			{
			    اگر (باری == 2)
				shape_جدول[idx].shape = SHAPE_بلاک;
			    p += 5;
			}
			ورنہ	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    اگر (commap == تہی)		    // last part
			    {
				اگر (endp == تہی)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    ورنہ اگر (endp > commap || endp == تہی)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    اگر (slashp != تہی && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_گروہ(p, (صحیح)(slashp - p));
				p = slashp + 1;
			    }
			    اگر (باری == 2)
			    {
				shape_جدول[idx].id = syn_check_گروہ(p,
							     (صحیح)(endp - p));
				shape_جدول[idx].id_lm = shape_جدول[idx].id;
				اگر (slashp != تہی && slashp < endp)
				    shape_جدول[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    اگر (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    اگر (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    اگر (!ملا_ve)
    {
# ifdef FEAT_MOUSESHAPE
	اگر (what == SHAPE_MOUSE)
	{
	    shape_جدول[SHAPE_IDX_VE].mshape = shape_جدول[SHAPE_IDX_V].mshape;
	}
	ورنہ
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

    واپس تہی;
}

# اگر defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    صحیح
get_shape_idx(صحیح mouse)
{
#  ifdef FEAT_MOUSESHAPE
    اگر (mouse && (State == منوال_HITRETURN || State == منوال_ASKMORE))
    {
#   ifdef FEAT_GUI
	صحیح x, y;
	gui_mch_getmouse(&x, &y);
	اگر (Y_2_صف(y) == صفیں - 1)
	    واپس SHAPE_IDX_MOREL;
#   endif
	واپس SHAPE_IDX_مزید;
    }
    اگر (mouse && drag_حالت_سطر)
	واپس SHAPE_IDX_SDRAG;
    اگر (mouse && drag_sep_سطر)
	واپس SHAPE_IDX_VDRAG;
#  endif
    اگر (!mouse && State == منوال_SHOWMATCH)
	واپس SHAPE_IDX_SM;
    اگر (State & VREPLACE_FLAG)
	واپس SHAPE_IDX_R;
    اگر (State & REPLACE_FLAG)
	واپس SHAPE_IDX_R;
    اگر (State & منوال_داخل_کرو)
	واپس SHAPE_IDX_I;
    اگر (State & منوال_CMDLINE)
    {
	اگر (cmdline_at_انتہا())
	    واپس SHAPE_IDX_C;
	اگر (cmdline_overstrike())
	    واپس SHAPE_IDX_CR;
	واپس SHAPE_IDX_CI;
    }
    اگر (اختتام_op)
	واپس SHAPE_IDX_O;
    اگر (VIsual_فعال)
    {
	اگر (*p_sel == 'e')
	    واپس SHAPE_IDX_VE;
	ورنہ
	    واپس SHAPE_IDX_V;
    }
    واپس SHAPE_IDX_N;
}
# endif

# اگر defined(FEAT_MOUSESHAPE)
ساکن صحیح current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    خالی
update_mouseshape(صحیح shape_idx)
{
    صحیح new_mouse_shape;

    // Only works in GUI mode.
    اگر (!gui.اندر_use || gui.starting)
	واپس;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    اگر (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	واپس;
    }

    // When ignoring the mouse don't change shape on the statusline.
    اگر (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_حالت
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    اگر (shape_idx == -2
	    && current_mouse_shape != shape_جدول[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_جدول[SHAPE_IDX_حالت].mshape
	    && current_mouse_shape != shape_جدول[SHAPE_IDX_VSEP].mshape)
	واپس;
    اگر (shape_idx < 0)
	new_mouse_shape = shape_جدول[get_shape_idx(TRUE)].mshape;
    ورنہ
	new_mouse_shape = shape_جدول[shape_idx].mshape;
    اگر (new_mouse_shape != current_mouse_shape)
    {
	mch_سیٹ_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#اگر_ختم // CURSOR_SHAPE

#اگر defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    خالی
f_getmouseshape(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
# اگر defined(FEAT_MOUSESHAPE)
    اگر (current_mouse_shape >= 0
			      && current_mouse_shape < (صحیح)MSHAPE_نام_گنتی)
	rettv->vval.v_string = vim_strnsave(
				  mshape_نام[current_mouse_shape].string,
				  mshape_نام[current_mouse_shape].لمبائی);
# endif
}
#اگر_ختم



/*
 * Change directory to "new_dir".  Search 'cdpath' for relative directory
 * names.
 */
    صحیح
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    صحیح		r;
    char_u	*file_مقصد_ڈھونڈو = تہی;
    حرف	*تلاش_ctx = تہی;

    dir_name = ڈھونڈو_directory_اندر_راستہ(new_dir, (صحیح)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_مقصد_ڈھونڈو, &تلاش_ctx);
    vim_free(file_مقصد_ڈھونڈو);
    vim_findfile_cleanup(تلاش_ctx);
    اگر (dir_name == تہی)
	واپس -1;
    r = mch_chdir((حرف *)dir_name);
    vim_free(dir_name);
    واپس r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    صحیح
get_user_name(char_u *buf, صحیح len)
{
    اگر (username == تہی)
    {
	اگر (mch_get_user_name(buf, len) == FAIL)
	    واپس FAIL;
	username = vim_strsave(buf);
    }
    ورنہ
	vim_strncpy(buf, username, len - 1);
    واپس OK;
}

#اگر defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    خالی
free_username(خالی)
{
    vim_free(username);
}
#اگر_ختم

#اگر !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    خالی
qsort(
    خالی	*بنیاد,
    size_t	elm_گنتی,
    size_t	elm_سائز,
    صحیح (*cmp)(ثابت خالی *, ثابت خالی *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    صحیح		i, j;
    صحیح		gap;

    buf = alloc(elm_سائز);
    اگر (buf == تہی)
	واپس;

    برائے (gap = elm_گنتی / 2; gap > 0; gap /= 2)
	برائے (i = gap; i < elm_گنتی; ++i)
	    برائے (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)بنیاد + j * elm_سائز;
		p2 = (char_u *)بنیاد + (j + gap) * elm_سائز;
		اگر ((*cmp)((خالی *)p1, (خالی *)p2) <= 0)
		    توڑو;
		// Exchange the elements.
		mch_memmove(buf, p1, elm_سائز);
		mch_memmove(p1, p2, elm_سائز);
		mch_memmove(p2, buf, elm_سائز);
	    }

    vim_free(buf);
}
#اگر_ختم

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

#اگر !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

ساکن صحیح  envsize = -1;	// current size of environment
خارجی حرف **environ;		// the global which is your env.

ساکن صحیح  findenv(حرف *name); // look for a name in the env.
ساکن صحیح  newenv(خالی);	// copy env. from stack to heap
ساکن صحیح  moreenv(خالی);	// incr. size of env.

    صحیح
putenv(ثابت حرف *string)
{
    صحیح	    i;
    حرف    *p;

    اگر (envsize < 0)
    {				// first time putenv called
	اگر (newenv() < 0)	// copy env. to heap
	    واپس -1;
    }

    i = findenv((حرف *)string); // look for name in environment

    اگر (i < 0)
    {				// name must be added
	برائے (i = 0; environ[i]; i++);
	اگر (i >= (envsize - 1))
	{			// need new slot
	    اگر (moreenv() < 0)
		واپس -1;
	}
	p = alloc(لڑی_لمبائی(string) + 1);
	اگر (p == تہی)		// not enough core
	    واپس -1;
	environ[i + 1] = 0;	// new end of env.
    }
    ورنہ
    {				// name already in env.
	p = vim_realloc(environ[i], لڑی_لمبائی(string) + 1);
	اگر (p == تہی)
	    واپس -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    واپس 0;
}

    ساکن صحیح
findenv(حرف *name)
{
    حرف    *namechar, *envchar;
    صحیح	    i, ملا;

    ملا = 0;
    برائے (i = 0; environ[i] && !ملا; i++)
    {
	envchar = environ[i];
	namechar = name;
	جب (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	ملا = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    واپس ملا ? i - 1 : -1;
}

    ساکن صحیح
newenv(خالی)
{
    حرف    **env, *elem;
    صحیح	    i, esize;

    برائے (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(حرف *, esize);
    اگر (env == تہی)
	واپس -1;

    برائے (i = 0; environ[i]; i++)
    {
	elem = alloc(لڑی_لمبائی(environ[i]) + 1);
	اگر (elem == تہی)
	    واپس -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    واپس 0;
}

    ساکن صحیح
moreenv(خالی)
{
    صحیح	    esize;
    حرف    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((حرف *)environ, esize * حجم (*env));
    اگر (env == 0)
	واپس -1;
    environ = env;
    envsize = esize;
    واپس 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(ثابت char_u *string)
{
    صحیح i;
    char_u *p;

    اگر (envsize < 0)
	واپس تہی;

    i = findenv((حرف *)string);

    اگر (i < 0)
	واپس تہی;

    p = vim_strchr((char_u *)environ[i], '=');
    واپس (p + 1);
}
# endif

#اگر_ختم // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#اگر defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    صحیح
filewritable(char_u *fname)
{
    صحیح		retval = 0;
# اگر defined(UNIX) || defined(VMS)
    صحیح		perm = 0;
# endif

# اگر defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    اگر (
# ifdef MSWIN
	    mch_writable(fname) &&
# ورنہ
#  اگر defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((حرف *)fname, W_OK) == 0
       )
    {
	++retval;
	اگر (mch_isdir(fname))
	    ++retval;
    }
    واپس retval;
}
#اگر_ختم

#اگر defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    صحیح
get2c(FILE *fd)
{
    صحیح		c, n;

    n = getc(fd);
    اگر (n == EOF) واپس -1;
    c = getc(fd);
    اگر (c == EOF) واپس -1;
    واپس (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    صحیح
get3c(FILE *fd)
{
    صحیح		c, n;

    n = getc(fd);
    اگر (n == EOF) واپس -1;
    c = getc(fd);
    اگر (c == EOF) واپس -1;
    n = (n << 8) + c;
    c = getc(fd);
    اگر (c == EOF) واپس -1;
    واپس (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    صحیح
get4c(FILE *fd)
{
    صحیح		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    غیرعلامتی	n;

    c = getc(fd);
    اگر (c == EOF) واپس -1;
    n = (غیرعلامتی)c;
    c = getc(fd);
    اگر (c == EOF) واپس -1;
    n = (n << 8) + (غیرعلامتی)c;
    c = getc(fd);
    اگر (c == EOF) واپس -1;
    n = (n << 8) + (غیرعلامتی)c;
    c = getc(fd);
    اگر (c == EOF) واپس -1;
    n = (n << 8) + (غیرعلامتی)c;
    واپس (صحیح)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, صحیح cnt)
{
    char_u	*str;
    صحیح		i;
    صحیح		c;

    // allocate memory
    str = alloc(cnt + 1);
    اگر (str == تہی)
	واپس تہی;

    // Read the string.  Quit when running into the EOF.
    برائے (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	اگر (c == EOF)
	{
	    vim_free(str);
	    واپس تہی;
	}
	str[i] = c;
    }
    str[i] = NUL;
    واپس str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    صحیح
put_بائٹیں(FILE *fd, long_u nr, صحیح len)
{
    صحیح	    i;

    برائے (i = len - 1; i >= 0; --i)
	اگر (putc((صحیح)(nr >> (i * 8)), fd) == EOF)
	    واپس FAIL;
    واپس OK;
}

#اگر_ختم

#اگر_غیر_تعریف_شدہ PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    طویل
elapsed(ساخت timeval *آغاز_tv)
{
    ساخت timeval  now_tv;

    gettimeofday(&now_tv, تہی);
    واپس (now_tv.tv_sec - آغاز_tv->tv_sec) * 1000L
	 + (now_tv.tv_usec - آغاز_tv->tv_usec) / 1000L;
}
# endif

# ifdef ELAPSED_TICKCOUNT
/*
 * Return time in msec since "start_tick".
 */
    طویل
elapsed(DWORD آغاز_tick)
{
    DWORD	now = Get_Tick_گنتی();

    واپس (طویل)now - (طویل)آغاز_tick;
}
# endif
#اگر_ختم

#اگر defined(FEAT_کام_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    صحیح
mch_parse_cmd(char_u *cmd, صحیح use_shcf, حرف ***argv, صحیح *argc)
{
    صحیح		i;
    char_u	*p, *d;
    صحیح		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    برائے (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	جب (*p != NUL)
	{
	    اگر (i == 2)
		(*argv)[*argc] = (حرف *)p;
	    ++*argc;
	    d = p;
	    جب (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		اگر (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		ورنہ
		{
		    اگر (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    اگر (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    اگر (*p == NUL)
	    {
		اگر (i == 2)
		    *d++ = NUL;
		توڑو;
	    }
	    اگر (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	اگر (*argv == تہی)
	{
	    اگر (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		برائے (;;)
		{
		    p = skiptowhite(p);
		    اگر (*p == NUL)
			توڑو;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(حرف *, *argc + 4);
	    اگر (*argv == تہی)	    // out of memory
		واپس FAIL;
	}
    }
    واپس OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    صحیح
build_argv_from_string(char_u *cmd, حرف ***argv, صحیح *argc)
{
    char_u	*cmd_copy;
    صحیح		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    اگر (cmd_copy == تہی
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	واپس FAIL;
    }
    برائے (i = 0; i < *argc; i++)
	(*argv)[i] = (حرف *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = تہی;
    vim_free(cmd_copy);
    واپس OK;
}

#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    صحیح
build_argv_from_list(list_T *l, حرف ***argv, صحیح *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(حرف *, l->lv_len + 1);
    اگر (*argv == تہی)
	واپس FAIL;
    *argc = 0;
    FOR_ALL_LIST_اشیاء(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	اگر (s == تہی)
	{
	    صحیح i;

	    برائے (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = تہی;
	    واپس FAIL;
	}
	(*argv)[*argc] = (حرف *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = تہی;
    واپس OK;
}
#اگر_ختم

/*
 * Change the behavior of vterm.
 * 0: As usual.
 * 1: Windows 10 version 1809
 *      The bug causes unstable handling of ambiguous width character.
 * 2: Windows 10 version 1903 & 1909
 *      Use the wrong result because each result is different.
 * 3: Windows 10 insider preview (current latest logic)
 */
    صحیح
get_special_pty_type(خالی)
{
#اگر_تعریف_شدہ MSWIN
    واپس get_conpty_type();
#ورنہ
    واپس 0;
#اگر_ختم
}

// compare two keyvalue_T structs by case sensitive value
    صحیح
cmp_keyvalue_قدر(ثابت خالی *a, ثابت خالی *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    واپس STRCMP(kv1->قدر.string, kv2->قدر.string);
}

// compare two keyvalue_T structs by value with length
    صحیح
cmp_keyvalue_قدر_n(ثابت خالی *a, ثابت خالی *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    واپس STRNCMP(kv1->قدر.string, kv2->قدر.string, MAX(kv1->قدر.لمبائی,
		kv2->قدر.لمبائی));
}

// compare two keyvalue_T structs by case insensitive value
    صحیح
cmp_keyvalue_قدر_i(ثابت خالی *a, ثابت خالی *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    واپس STRICMP(kv1->قدر.string, kv2->قدر.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    صحیح
cmp_keyvalue_قدر_ni(ثابت خالی *a, ثابت خالی *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    واپس vim_strnicmp_asc((حرف *)kv1->قدر.string,
	    (حرف *)kv2->قدر.string, MAX(kv1->قدر.لمبائی,
		    kv2->قدر.لمبائی));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    خالی *
mergesort_list(
    خالی	*سر,
    خالی	*(*get_اگلا)(خالی *),
    خالی	(*سیٹ_اگلا)(خالی *, خالی *),
    خالی	*(*get_پچھلا)(خالی *),
    خالی	(*سیٹ_پچھلا)(خالی *, خالی *),
    صحیح		(*compare)(ثابت خالی *, ثابت خالی *))
{
    اگر (!سر || !get_اگلا(سر))
	واپس سر;

    // Count length
    صحیح	    n = 0;
    خالی*   curr = سر;
    جب (curr)
    {
	n++;
	curr = get_اگلا(curr);
    }

    صحیح	سائز;
    برائے (سائز = 1; سائز < n; سائز *= 2)
    {
	خالی*	new_سر = تہی;
	خالی*	دم = تہی;
	curr = سر;

	جب (curr)
	{
	    // Split two runs
	    خالی    *بایاں = curr;
	    خالی    *دایاں = بایاں;
	    صحیح	    i;
	    برائے (i = 0; i < سائز && دایاں; ++i)
		دایاں = get_اگلا(دایاں);

	    خالی    *اگلا = دایاں;
	    برائے (i = 0; i < سائز && اگلا; ++i)
		اگلا = get_اگلا(اگلا);

	    // Break links
	    خالی    *l_انتہا = دایاں ? get_پچھلا(دایاں) : تہی;
	    اگر (l_انتہا)
		سیٹ_اگلا(l_انتہا, تہی);
	    اگر (دایاں)
		سیٹ_پچھلا(دایاں, تہی);

	    خالی    *r_انتہا = اگلا ? get_پچھلا(اگلا) : تہی;
	    اگر (r_انتہا)
		سیٹ_اگلا(r_انتہا, تہی);
	    اگر (اگلا)
		سیٹ_پچھلا(اگلا, تہی);

	    //ملاؤ
	    خالی    *merged = تہی;
	    خالی    *merged_دم = تہی;

	    جب (بایاں || دایاں)
	    {
		خالی	*chosen = تہی;
		اگر (!بایاں)
		{
		    chosen = دایاں;
		    دایاں = get_اگلا(دایاں);
		}
		ورنہ اگر (!دایاں)
		{
		    chosen = بایاں;
		    بایاں = get_اگلا(بایاں);
		}
		ورنہ اگر (compare(بایاں, دایاں) <= 0)
		{
		    chosen = بایاں;
		    بایاں = get_اگلا(بایاں);
		}
		ورنہ
		{
		    chosen = دایاں;
		    دایاں = get_اگلا(دایاں);
		}

		اگر (merged_دم)
		{
		    سیٹ_اگلا(merged_دم, chosen);
		    سیٹ_پچھلا(chosen, merged_دم);
		    merged_دم = chosen;
		}
		ورنہ
		{
		    merged = merged_دم = chosen;
		    سیٹ_پچھلا(chosen, تہی);
		}
	    }

	    // Connect to full list
	    اگر (!new_سر)
		new_سر = merged;
	    ورنہ
	    {
		سیٹ_اگلا(دم, merged);
		سیٹ_پچھلا(merged, دم);
	    }

	    // Move tail to end
	    جب (get_اگلا(merged_دم))
		merged_دم = get_اگلا(merged_دم);
	    دم = merged_دم;

	    curr = اگلا;
	}

	سر = new_سر;
    }

    واپس سر;
}

ساکن ثابت char_u base64_جدول[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

ساکن char_u base64_dec_جدول[256];

#تعریف_کرو BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    ساکن خالی
init_base64_dec_جدول(خالی)
{
    ساکن صحیح base64_dec_tbl_initialized = FALSE;

    اگر (base64_dec_tbl_initialized)
	واپس;

    vim_memset(base64_dec_جدول, 0xFF, حجم(base64_dec_جدول));
    برائے (size_t i = 0; i < حجم(base64_جدول) - 1; i++)
	base64_dec_جدول[(char_u)base64_جدول[i]] = (char_u)i;
    base64_dec_جدول['='] = 0;

    base64_dec_tbl_initialized = TRUE;
}

/*
 * Base64 encode "src[len]" into caller-supplied "dst", which must have room for
 * at least BASE64_ENCODED_LEN(len) bytes (NUL terminated). Returns the number
 * of bytes written, excluding the NUL.
 */
    طویل
base64_encode_buf(char_u *dst, ثابت char_u *src, size_t len)
{
    size_t i, j;

    برائے (i = 0, j = 0; i < len;)
    {
	size_t بقایا = len - i;

	int_u octet_a = src[i++];
	int_u octet_b = بقایا > 1 ? src[i++] : 0;
	int_u octet_c = بقایا > 2 ? src[i++] : 0;

	int_u triple = (octet_a << 16) | (octet_b << 8) | octet_c;

	dst[j++] = base64_جدول[(triple >> 18) & 0x3F];
	dst[j++] = base64_جدول[(triple >> 12) & 0x3F];
	dst[j++] = بقایا > 1
	    ? base64_جدول[(triple >> 6) & 0x3F]
	    : '=';
	dst[j++] = بقایا > 2
	    ? base64_جدول[triple & 0x3F]
	    : '=';
    }

    dst[j] = NUL;
    واپس (طویل)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(ثابت char_u *ڈیٹا, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    اگر (encoded == تہی)
	واپس تہی;
    base64_encode_buf(encoded, ڈیٹا, len);
    واپس encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    صحیح
base64_decode(ثابت char_u *ڈیٹا, size_t len, garray_T *برآمد)
{
    اگر (len == 0)
	واپس OK;

    اگر (len % 4 != 0)
    {
	semsg(_(e_invalid_دلیل_str), ڈیٹا);
	واپس FAIL;
    }

    init_base64_dec_جدول();

    size_t decoded_len = (len / 4) * 3;
    اگر (ڈیٹا[len - 1] == '=')
	decoded_len--;
    اگر (ڈیٹا[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    برائے (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_جدول[(char_u)ڈیٹا[i++]];
	int_u sextet_b = base64_dec_جدول[(char_u)ڈیٹا[i++]];
	int_u sextet_c = base64_dec_جدول[(char_u)ڈیٹا[i++]];
	int_u sextet_d = base64_dec_جدول[(char_u)ڈیٹا[i++]];

	اگر (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_دلیل_str), ڈیٹا);
	    ga_clear(برآمد);
	    واپس FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	اگر (j < decoded_len)
	{
	    ga_append(برآمد, (triple >> 16) & 0xFF);
	    j++;
	}
	اگر (j < decoded_len)
	{
	    ga_append(برآمد, (triple >> 8) & 0xFF);
	    j++;
	}
	اگر (j < decoded_len)
	{
	    ga_append(برآمد, triple & 0xFF);
	    j++;
	}

	اگر (j == decoded_len)
	{
	    اگر ((ڈیٹا[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((ڈیٹا[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_دلیل_str), ڈیٹا);
		ga_clear(برآمد);
		واپس FAIL;
	    }
	}
    }
    واپس OK;
}
