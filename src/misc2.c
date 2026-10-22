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
#அந்தர்பாவ "vim.h"

நிலையான char_u	*username = வெற்று; // cached result of mch_get_user_name()

நிலையான முழு coladvance2(pos_T *pos, முழு addspaces, முழு finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    முழு
virtual_active(வெறுமை)
{
    unsigned முழு cur_ve_flags = get_ve_flags();

    எனில் (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_செருகு) && (State & MODE_செருகு)))
	திரும்பு TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    எனில் (virtual_op != MAYBE)
	திரும்பு virtual_op;
    திரும்பு (cur_ve_flags & VE_BLOCK) && VIsual_active && VIsual_mode == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    முழு
getviscol(வெறுமை)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, வெற்று, வெற்று, GETVCOL_NO_ABOVE);
    திரும்பு (முழு)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    முழு
coladvance_force(colnr_T wcol)
{
    முழு rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    எனில் (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    இல்லையெனில்
	// Virtcol is valid
	set_valid_virtcol(curwin, wcol);
    திரும்பு rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    முழு
getviscol2(colnr_T col, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = col;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, வெற்று, வெற்று, GETVCOL_NO_ABOVE);
    திரும்பு (முழு)x;
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
    முழு
coladvance(colnr_T wantcol)
{
    முழு rc = getvpos(&curwin->w_cursor, wantcol);

    எனில் (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    இல்லையெனில் எனில் (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	set_valid_virtcol(curwin, wantcol);
    திரும்பு rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    முழு
getvpos(pos_T *pos, colnr_T wantcol)
{
    திரும்பு coladvance2(pos, FALSE, virtual_active(), wantcol);
}

    நிலையான முழு
coladvance2(
    pos_T	*pos,
    முழு		addspaces,	// change the text to achieve our goal?
    முழு		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    முழு		idx;
    char_u	*வரி;
    முழு		linelen;
    colnr_T	col = 0;
    முழு		csize = 0;
    முழு		one_more;
#ifdef FEAT_LINEBREAK
    முழு		head = 0;
#endif

    one_more = (State & MODE_செருகு)
		    || restart_edit != NUL
		    || (VIsual_active && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    வரி = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    எனில் (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_more;
	    col = wcol;

	    எனில் ((addspaces || finetune) && !VIsual_active)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_more;
		எனில் (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    இல்லையெனில்
    {
	முழு		width = curwin->w_width - win_col_off(curwin);
	chartabsize_T	cts;

	எனில் (finetune
		&& curwin->w_p_wrap
		&& curwin->w_width != 0
		&& wcol >= (colnr_T)width
		&& width > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    எனில் (csize > 0)
		csize--;

	    எனில் (wcol / width > (colnr_T)csize / width
		    && ((State & MODE_செருகு) == 0 || (முழு)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / width + 1) * width - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, வரி, வரி);
#ifdef FEAT_PROP_POPUP
	cts.cts_no_above = true;
#endif
	போது (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ifdef FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &head, வெற்று);
	    MB_PTR_ADV(cts.cts_ptr);
#இல்லையெனில்
	    csize = lbr_chartabsize_adv(&cts);
#endif
	    cts.cts_vcol += csize;
	}
	col = cts.cts_vcol;
	idx = (முழு)(cts.cts_ptr - வரி);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	எனில் (col > wcol || (!virtual_active() && one_more == 0))
	{
	    idx -= 1;
#ifdef FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= head;
#endif
	    col -= csize;
	}

	எனில் (virtual_active()
		&& addspaces
		&& wcol >= 0
		&& ((col != wcol && col != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    எனில் (வரி[idx] == NUL)
	    {
		// Append spaces
		முழு	correct = wcol - col;
		char_u	*newline = alloc(idx + correct + 1);
		முழு	t;

		எனில் (newline == வெற்று)
		    திரும்பு FAIL;

		க்கு (t = 0; t < idx; ++t)
		    newline[t] = வரி[t];

		க்கு (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, (colnr_T)idx);
		idx += correct;
		col = wcol;
	    }
	    இல்லையெனில்
	    {
		// Break a tab
		முழு	correct = wcol - col - csize + 1; // negative!!
		char_u	*newline;
		முழு	t, s = 0;
		முழு	v;

		எனில் (-correct > csize)
		    திரும்பு FAIL;

		newline = alloc(linelen + csize);
		எனில் (newline == வெற்று)
		    திரும்பு FAIL;

		க்கு (t = 0; t < linelen; t++)
		{
		    எனில் (t != idx)
			newline[s++] = வரி[t];
		    இல்லையெனில்
			க்கு (v = 0; v < csize; v++)
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

    எனில் (idx < 0)
	pos->col = 0;
    இல்லையெனில்
	pos->col = idx;

    pos->coladd = 0;

    எனில் (finetune)
    {
	எனில் (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    எனில் (!one_more)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, வெற்று, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	இல்லையெனில்
	{
	    முழு b = (முழு)wcol - (முழு)col;

	    // The difference between wcol and col is used to set coladd.
	    எனில் (b > 0 && b < (MAXCOL - 2 * curwin->w_width))
		pos->coladd = b;

	    col += b;
	}
    }

    // prevent from moving onto a trail byte
    எனில் (has_mbyte)
	mb_adjustpos(curbuf, pos);

    எனில் (wcol < 0 || col < wcol)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    முழு
inc_cursor(வெறுமை)
{
    திரும்பு inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    முழு
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    எனில் (lp->col != MAXCOL)
    {
	p = ml_get_pos(lp);
	எனில் (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    எனில் (has_mbyte)
	    {
		முழு l = (*mb_ptr2len)(p);

		lp->col += l;
		திரும்பு ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->col++;
	    lp->coladd = 0;
	    திரும்பு ((p[1] != NUL) ? 0 : 2);
	}
    }
    எனில் (lp->lnum != curbuf->b_ml.ml_வரி_மொத்தம்)     // there is a next line
    {
	lp->col = 0;
	lp->lnum++;
	lp->coladd = 0;
	திரும்பு 1;
    }
    திரும்பு -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    முழு
incl(pos_T *lp)
{
    முழு	    r;

    எனில் ((r = inc(lp)) >= 1 && lp->col)
	r = inc(lp);
    திரும்பு r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    முழு
dec_cursor(வெறுமை)
{
    திரும்பு dec(&curwin->w_cursor);
}

    முழு
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    எனில் (lp->col == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	எனில் (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	திரும்பு 0;
    }

    எனில் (lp->col > 0)
    {
	// still within line
	lp->col--;
	எனில் (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	}
	திரும்பு 0;
    }

    எனில் (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	எனில் (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	திரும்பு 1;
    }

    // at start of file
    திரும்பு -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    முழு
decl(pos_T *lp)
{
    முழு	    r;

    எனில் ((r = dec(lp)) == 1 && lp->col)
	r = dec(lp);
    திரும்பு r;
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
    எனில் (hasAnyFolding(wp))
    {
	எனில் (lnum > cursor)
	{
	    போது (lnum > cursor)
	    {
		(வெறுமை)hasFoldingWin(wp, lnum, &lnum, வெற்று, TRUE, வெற்று);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		எனில் (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	இல்லையெனில் எனில் (lnum < cursor)
	{
	    போது (lnum < cursor)
	    {
		(வெறுமை)hasFoldingWin(wp, lnum, வெற்று, &lnum, TRUE, வெற்று);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		எனில் (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    இல்லையெனில்
#endif
	retval = lnum - cursor;

    திரும்பு retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    வெறுமை
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    எனில் (pos->lnum > buf->b_ml.ml_வரி_மொத்தம்)
	pos->lnum = buf->b_ml.ml_வரி_மொத்தம்;

    எனில் (pos->col > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	எனில் (pos->col > len)
	    pos->col = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    வெறுமை
check_cursor_lnum(வெறுமை)
{
    எனில் (curwin->w_cursor.lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
    {
#ifdef FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	எனில் (!hasFolding(curbuf->b_ml.ml_வரி_மொத்தம்,
						&curwin->w_cursor.lnum, வெற்று))
#endif
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
    }
    எனில் (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    வெறுமை
check_cursor_col(வெறுமை)
{
    check_cursor_col_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    வெறுமை
check_cursor_col_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.col;
    colnr_T      oldcoladd = win->w_cursor.col + win->w_cursor.coladd;
    unsigned முழு cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_buffer, win->w_cursor.lnum);
    எனில் (len == 0)
	win->w_cursor.col = 0;
    இல்லையெனில் எனில் (win->w_cursor.col >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	எனில் ((State & MODE_செருகு) || restart_edit
		|| (VIsual_active && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_active())
	    win->w_cursor.col = len;
	இல்லையெனில்
	{
	    win->w_cursor.col = len - 1;
	    // Move the cursor to the head byte.
	    எனில் (has_mbyte)
		mb_adjustpos(win->w_buffer, &win->w_cursor);
	}
    }
    இல்லையெனில் எனில் (win->w_cursor.col < 0)
	win->w_cursor.col = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    எனில் (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    இல்லையெனில் எனில் (cur_ve_flags == VE_ALL)
    {
	எனில் (oldcoladd > win->w_cursor.col)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.col;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    எனில் (win->w_cursor.col + 1 < len)
	    {
		முழு cs, ce;

		getvcol(win, &win->w_cursor, &cs, வெற்று, &ce, 0);
		எனில் (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	இல்லையெனில்
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    வெறுமை
check_cursor(வெறுமை)
{
    check_cursor_lnum();
    check_cursor_col();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    வெறுமை
check_visual_pos(வெறுமை)
{
    எனில் (VIsual.lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	VIsual.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	VIsual.col = 0;
	VIsual.coladd = 0;
    }
    இல்லையெனில்
    {
	முழு len = ml_get_len(VIsual.lnum);

	எனில் (VIsual.col > len)
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
    வெறுமை
adjust_cursor_col(வெறுமை)
{
    எனில் (curwin->w_cursor.col > 0
	    && (!VIsual_active || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    முழு
set_leftcol(colnr_T leftcol)
{
    முழு		retval = FALSE;

    // Return quickly when there is no change.
    எனில் (curwin->w_leftcol == leftcol)
	திரும்பு FALSE;
    curwin->w_leftcol = leftcol;

    changed_cline_bef_curs();
    long lastcol = curwin->w_leftcol + curwin->w_width - curwin_col_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    long siso = get_sidescrolloff_மதிப்பு();
    எனில் (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    இல்லையெனில் எனில் (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(வெறுமை)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, வெற்று, &e, 0);
    எனில் (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    இல்லையெனில் எனில் (s < curwin->w_leftcol)
    {
	retval = TRUE;
	எனில் (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    changed_cline_bef_curs();
	}
    }

    எனில் (retval)
	curwin->w_set_curswant = true;
    redraw_later(UPD_NOT_VALID);
    திரும்பு retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    முழு
copy_option_part(
    char_u	**option,
    char_u	*buf,
    முழு		maxlen,
    எழுத்து	*sep_chars)
{
    முழு	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    எனில் (*p == '.')
	buf[len++] = *p++;
    போது (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == வெற்று)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	எனில் (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != வெற்று)
	    ++p;
	எனில் (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    எனில் (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_to_option_part(p);	// p points to next file name

    *option = p;
    திரும்பு len;
}

#எனில் !defined(HAVE_MEMSET) && !defined(PROTO)
    வெறுமை *
vim_memset(வெறுமை *ptr, முழு c, size_t size)
{
    எழுத்து *p = ptr;

    போது (size-- > 0)
	*p++ = c;
    திரும்பு ptr;
}
#endif

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    முழு
vim_isspace(முழு x)
{
    திரும்பு ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

நிலையான கட்டமைப்பு modmasktable
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
#எனில் defined(MACOS_X) || defined(FEAT_GUI_GTK)
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
#define MOD_சாவிகள்_ENTRY_SIZE 5

நிலையான char_u modifier_சாவிகள்_table[] =
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
    MOD_MASK_CTRL,  KS_EXTRA, (முழு)KE_C_END,	'@', '7',	// end
    MOD_MASK_SHIFT, '*', '9',			'@', '9',	// exit
    MOD_MASK_SHIFT, '*', '0',			'@', '0',	// find
    MOD_MASK_SHIFT, '#', '1',			'%', '1',	// help
    MOD_MASK_SHIFT, '#', '2',			'k', 'h',	// home
    MOD_MASK_CTRL,  KS_EXTRA, (முழு)KE_C_HOME,	'k', 'h',	// home
    MOD_MASK_SHIFT, '#', '3',			'k', 'I',	// insert
    MOD_MASK_SHIFT, '#', '4',			'k', 'l',	// left arrow
    MOD_MASK_CTRL,  KS_EXTRA, (முழு)KE_C_இடது,	'k', 'l',	// left arrow
    MOD_MASK_SHIFT, '%', 'a',			'%', '3',	// message
    MOD_MASK_SHIFT, '%', 'b',			'%', '4',	// move
    MOD_MASK_SHIFT, '%', 'c',			'%', '5',	// next
    MOD_MASK_SHIFT, '%', 'd',			'%', '7',	// options
    MOD_MASK_SHIFT, '%', 'e',			'%', '8',	// previous
    MOD_MASK_SHIFT, '%', 'f',			'%', '9',	// print
    MOD_MASK_SHIFT, '%', 'g',			'%', '0',	// redo
    MOD_MASK_SHIFT, '%', 'h',			'&', '3',	// replace
    MOD_MASK_SHIFT, '%', 'i',			'k', 'r',	// right arr.
    MOD_MASK_CTRL,  KS_EXTRA, (முழு)KE_C_வலது,	'k', 'r',	// right arr.
    MOD_MASK_SHIFT, '%', 'j',			'&', '5',	// resume
    MOD_MASK_SHIFT, '!', '1',			'&', '6',	// save
    MOD_MASK_SHIFT, '!', '2',			'&', '7',	// suspend
    MOD_MASK_SHIFT, '!', '3',			'&', '8',	// undo
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_UP,	'k', 'u',	// up arrow
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_DOWN,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_XF1,	KS_EXTRA, (முழு)KE_XF1,
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_XF2,	KS_EXTRA, (முழு)KE_XF2,
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_XF3,	KS_EXTRA, (முழு)KE_XF3,
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_XF4,	KS_EXTRA, (முழு)KE_XF4,

    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F1,	'k', '1',	// F1
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F2,	'k', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F3,	'k', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F4,	'k', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F5,	'k', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F6,	'k', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F7,	'k', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F8,	'k', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F9,	'k', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F10,	'k', ';',	// F10

    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F11,	'F', '1',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F12,	'F', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F13,	'F', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F14,	'F', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F15,	'F', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F16,	'F', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F17,	'F', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F18,	'F', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F19,	'F', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F20,	'F', 'A',

    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F21,	'F', 'B',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F22,	'F', 'C',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F23,	'F', 'D',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F24,	'F', 'E',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F25,	'F', 'F',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F26,	'F', 'G',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F27,	'F', 'H',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F28,	'F', 'I',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F29,	'F', 'J',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F30,	'F', 'K',

    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F31,	'F', 'L',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F32,	'F', 'M',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F33,	'F', 'N',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F34,	'F', 'O',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F35,	'F', 'P',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F36,	'F', 'Q',
    MOD_MASK_SHIFT, KS_EXTRA, (முழு)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_MASK_SHIFT, 'k', 'B',			KS_EXTRA, (முழு)KE_TAB,

    NUL
};

நிலையான கட்டமைப்பு சாவி_name_entry
{
    முழு		enabled;	    // is this entry available (TRUE/FALSE)?
    முழு		சாவி;		    // special key code or ascii value
    string_T	name;		    // name of key
    முழு		is_alt;		    // is an alternative name
} சாவி_names_table[] =
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
#இல்லையெனில்
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
#இல்லையெனில்
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
    {TRUE, K_இடது, STR_LITERAL_INIT("Left"), FALSE},
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
#இல்லையெனில்
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
#இல்லையெனில்
    FALSE,
#endif
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_வலது, STR_LITERAL_INIT("Right"), FALSE},
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
#இல்லையெனில்
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
#இல்லையெனில்
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
    நிலையான முழு
name_to_mod_mask(முழு c)
{
    முழு	    i;

    c = TOUPPER_ASC(c);
    க்கு (i = 0; mod_mask_table[i].mod_mask != 0; i++)
	எனில் (c == mod_mask_table[i].name)
	    திரும்பு mod_mask_table[i].mod_flag;
    திரும்பு 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    முழு
simplify_சாவி(முழு சாவி, முழு *modifiers)
{
    முழு	    i;
    முழு	    key0;
    முழு	    key1;

    எனில் (!(*modifiers & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	திரும்பு சாவி;

    // TAB is a special case
    எனில் (சாவி == TAB && (*modifiers & MOD_MASK_SHIFT))
    {
	*modifiers &= ~MOD_MASK_SHIFT;
	திரும்பு K_S_TAB;
    }
    key0 = KEY2TERMCAP0(சாவி);
    key1 = KEY2TERMCAP1(சாவி);
    க்கு (i = 0; modifier_சாவிகள்_table[i] != NUL; i += MOD_சாவிகள்_ENTRY_SIZE)
    {
	எனில் (key0 == modifier_சாவிகள்_table[i + 3]
		&& key1 == modifier_சாவிகள்_table[i + 4]
		&& (*modifiers & modifier_சாவிகள்_table[i]))
	{
	    *modifiers &= ~modifier_சாவிகள்_table[i];
	    திரும்பு TERMCAP2_சாவி(modifier_சாவிகள்_table[i + 1],
		    modifier_சாவிகள்_table[i + 2]);
	}
    }
    திரும்பு சாவி;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    முழு
handle_x_சாவிகள்(முழு சாவி)
{
    தேர்வு (சாவி)
    {
	நிலை K_XUP:	திரும்பு K_UP;
	நிலை K_XDOWN:	திரும்பு K_DOWN;
	நிலை K_XLEFT:	திரும்பு K_இடது;
	நிலை K_XRIGHT:	திரும்பு K_வலது;
	நிலை K_XHOME:	திரும்பு K_HOME;
	நிலை K_ZHOME:	திரும்பு K_HOME;
	நிலை K_XEND:	திரும்பு K_END;
	நிலை K_ZEND:	திரும்பு K_END;
	நிலை K_XF1:	திரும்பு K_F1;
	நிலை K_XF2:	திரும்பு K_F2;
	நிலை K_XF3:	திரும்பு K_F3;
	நிலை K_XF4:	திரும்பு K_F4;
	நிலை K_S_XF1:	திரும்பு K_S_F1;
	நிலை K_S_XF2:	திரும்பு K_S_F2;
	நிலை K_S_XF3:	திரும்பு K_S_F3;
	நிலை K_S_XF4:	திரும்பு K_S_F4;
    }
    திரும்பு சாவி;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_சாவி_name(முழு c, முழு modifiers)
{
    நிலையான char_u string[MAX_சாவி_NAME_LEN + 1];
    முழு	    i, idx, len;
    முழு	    table_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    எனில் (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_சாவி)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    எனில் (IS_SPECIAL(c))
    {
	க்கு (i = 0; modifier_சாவிகள்_table[i] != 0; i += MOD_சாவிகள்_ENTRY_SIZE)
	    எனில் (       KEY2TERMCAP0(c) == (முழு)modifier_சாவிகள்_table[i + 1]
		    && (முழு)KEY2TERMCAP1(c) == (முழு)modifier_சாவிகள்_table[i + 2])
	    {
		modifiers |= modifier_சாவிகள்_table[i];
		c = TERMCAP2_சாவி(modifier_சாவிகள்_table[i + 3],
						   modifier_சாவிகள்_table[i + 4]);
		நிறுத்து;
	    }
    }

    // try to find the key in the special key table
    table_idx = find_special_சாவி_in_table(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    எனில் (c > 0 && (*mb_char2len)(c) == 1)
    {
	எனில் (table_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_MASK_ALT;
	    // try again, to find the un-alted key in the special key table
	    table_idx = find_special_சாவி_in_table(c);
	}
	எனில் (table_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_MASK_CTRL;
	}
    }

    // translate the modifier into a string
    க்கு (i = 0; mod_mask_table[i].name != 'A'; i++)
	எனில் ((modifiers & mod_mask_table[i].mod_mask)
						== mod_mask_table[i].mod_flag)
	{
	    string[idx++] = mod_mask_table[i].name;
	    string[idx++] = (char_u)'-';
	}

    எனில் (table_idx < 0)		// unknown special key, may output t_xx
    {
	எனில் (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	இல்லையெனில்
	{
	    len = (*mb_char2len)(c);
	    எனில் (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    இல்லையெனில் எனில் (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    இல்லையெனில்
	    {
		char_u	*s = transchar(c);
		போது (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    இல்லையெனில்		// use name of special key
    {
	string_T    *s;

	s = &சாவி_names_table[table_idx].name;

	எனில் (s->length + idx + 2 <= MAX_சாவி_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (முழு)s->length;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    திரும்பு string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    முழு
trans_special(
    char_u	**srcp,
    char_u	*dst,
    முழு		flags,		// FSK_ values
    முழு		escape_ks,	// escape K_SPECIAL bytes in the character
    முழு		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    முழு		modifiers = 0;
    முழு		சாவி;

    சாவி = find_special_சாவி(srcp, &modifiers, flags, did_simplify);
    எனில் (சாவி == 0)
	திரும்பு 0;

    திரும்பு special_to_buf(சாவி, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    முழு
special_to_buf(முழு சாவி, முழு modifiers, முழு escape_ks, char_u *dst)
{
    முழு		dlen = 0;

    // Put the appropriate modifier in a string
    எனில் (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    எனில் (IS_SPECIAL(சாவி))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(சாவி);
	dst[dlen++] = KEY2TERMCAP1(சாவி);
    }
    இல்லையெனில் எனில் (escape_ks)
	dlen = (முழு)(add_char2buf(சாவி, dst + dlen) - dst);
    இல்லையெனில் எனில் (has_mbyte)
	dlen += (*mb_char2bytes)(சாவி, dst + dlen);
    இல்லையெனில்
	dst[dlen++] = சாவி;

    திரும்பு dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    முழு
find_special_சாவி(
    char_u	**srcp,
    முழு		*modp,
    முழு		flags,		// FSK_ values
    முழு		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*last_dash;
    char_u	*end_of_name;
    char_u	*src;
    char_u	*bp;
    முழு		in_string = flags & FSK_IN_STRING;
    முழு		modifiers;
    முழு		bit;
    முழு		சாவி;
    uvarnumber_T	n;
    முழு		l;

    src = *srcp;
    எனில் (src[0] != '<')
	திரும்பு 0;
    எனில் (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    last_dash = src;
    க்கு (bp = src + 1; *bp == '-' || vim_isNormalIDc(*bp); bp++)
    {
	எனில் (*bp == '-')
	{
	    last_dash = bp;
	    எனில் (bp[1] != NUL)
	    {
		எனில் (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		இல்லையெனில்
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		எனில் (!(in_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		இல்லையெனில் எனில் (in_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	எனில் (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	இல்லையெனில் எனில் (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, வெற்று, &l, STR2NR_ALL, வெற்று, வெற்று, 0, TRUE, வெற்று);
	    எனில் (l == 0)
	    {
		emsg(_(e_invalid_argument));
		திரும்பு 0;
	    }
	    bp += l + 5;
	    நிறுத்து;
	}
    }

    எனில் (*bp == '>')	// found matching '>'
    {
	end_of_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	க்கு (bp = src + 1; bp < last_dash; bp++)
	{
	    எனில் (*bp != '-')
	    {
		bit = name_to_mod_mask(*bp);
		எனில் (bit == 0x0)
		    நிறுத்து;	// Illegal modifier name
		modifiers |= bit;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	எனில் (bp >= last_dash)
	{
	    எனில் (STRNICMP(last_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(last_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(last_dash + 6, வெற்று, &l, STR2NR_ALL, வெற்று,
							    &n, 0, TRUE, வெற்று);
		எனில் (l == 0)
		{
		    emsg(_(e_invalid_argument));
		    திரும்பு 0;
		}
		சாவி = (முழு)n;
	    }
	    இல்லையெனில்
	    {
		முழு off = 1;

		// Modifier with single letter, or special key name.
		எனில் (in_string && last_dash[1] == '\\' && last_dash[2] == '"')
		    off = 2;
		எனில் (has_mbyte)
		    l = mb_ptr2len(last_dash + off);
		இல்லையெனில்
		    l = 1;
		எனில் (modifiers != 0 && last_dash[l + off] == '>')
		    சாவி = PTR2CHAR(last_dash + off);
		இல்லையெனில்
		{
		    சாவி = get_special_சாவி_code(last_dash + off);
		    எனில் (!(flags & FSK_KEEP_X_சாவி))
			சாவி = handle_x_சாவிகள்(சாவி);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    எனில் (சாவி != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		சாவி = simplify_சாவி(சாவி, &modifiers);

		எனில் ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    எனில் (சாவி == K_BS)
			சாவி = BS;
		    இல்லையெனில் எனில் (சாவி == K_DEL || சாவி == K_KDEL)
			சாவி = DEL;
		}
		இல்லையெனில் எனில் (சாவி == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    எனில் ((flags & FSK_SIMPLIFY) != 0)
		    {
			எனில் (did_simplify != வெற்று)
			    *did_simplify = TRUE;
		    }
		    இல்லையெனில்
			சாவி = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		எனில் (!IS_SPECIAL(சாவி))
		    சாவி = extract_modifiers(சாவி, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = end_of_name;
		திரும்பு சாவி;
	    }
	}
    }
    திரும்பு 0;
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
    முழு
may_adjust_சாவி_for_ctrl(முழு modifiers, முழு சாவி)
{
    எனில் ((modifiers & MOD_MASK_CTRL) == 0)
	திரும்பு சாவி;

    எனில் (ASCII_ISALPHA(சாவி))
    {
#ifdef FEAT_TERMINAL
	check_no_reduce_சாவிகள்();  // may update the no_reduce_keys flag
#endif
	திரும்பு no_reduce_சாவிகள் == 0 ? TOUPPER_ASC(சாவி) : சாவி;
    }
    எனில் (சாவி == '2')
	திரும்பு '@';
    எனில் (சாவி == '6')
	திரும்பு '^';
    எனில் (சாவி == '-')
	திரும்பு '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    எனில் (சாவி == '$' && (modifiers & MOD_MASK_SHIFT) == 0)
	திரும்பு ']';

    திரும்பு சாவி;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    முழு
may_remove_shift_modifier(முழு modifiers, முழு சாவி)
{
    எனில் ((modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
#ifdef FEAT_GUI_GTK
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_CMD)
#endif
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    && ((சாவி >= '!' && சாவி <= '/')
		|| (சாவி >= ':' && சாவி <= 'Z')
		|| vim_isdigit(சாவி)
		|| (சாவி >= '[' && சாவி <= '`')
		|| (சாவி >= '{' && சாவி <= '~')))
	திரும்பு modifiers & ~MOD_MASK_SHIFT;

    எனில் (modifiers == (MOD_MASK_SHIFT | MOD_MASK_CTRL)
		&& (சாவி == '{' || சாவி == '}' || சாவி == '|'))
	திரும்பு modifiers & ~MOD_MASK_SHIFT;

    திரும்பு modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    முழு
extract_modifiers(முழு சாவி, முழு *modp, முழு simplify, முழு *did_simplify)
{
    முழு	modifiers = *modp;

#ifdef MACOS_X
    // Command-key really special, no fancynest
    எனில் (!(modifiers & MOD_MASK_CMD))
#endif
    எனில் ((modifiers & MOD_MASK_SHIFT) && ASCII_ISALPHA(சாவி))
    {
	சாவி = TOUPPER_ASC(சாவி);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	எனில் (simplify || modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    modifiers &= ~MOD_MASK_SHIFT;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    எனில் ((modifiers & MOD_MASK_CTRL) && ASCII_ISALPHA(சாவி))
	சாவி = TOUPPER_ASC(சாவி);

    எனில் (simplify && (modifiers & MOD_MASK_CTRL)
	    && ((சாவி >= '?' && சாவி <= '_') || ASCII_ISALPHA(சாவி)))
    {
	சாவி = Ctrl_chr(சாவி);
	modifiers &= ~MOD_MASK_CTRL;
	// <C-@> is <Nul>
	எனில் (சாவி == NUL)
	    சாவி = K_ZERO;
	எனில் (did_simplify != வெற்று)
	    *did_simplify = TRUE;
    }

#ifdef MACOS_X
    // Command-key really special, no fancynest
    எனில் (!(modifiers & MOD_MASK_CMD))
#endif
    எனில் (simplify && (modifiers & MOD_MASK_ALT) && சாவி < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	சாவி |= 0x80;
	modifiers &= ~MOD_MASK_ALT;	// remove the META modifier
	எனில் (did_simplify != வெற்று)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    திரும்பு சாவி;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    முழு
find_special_சாவி_in_table(முழு c)
{
    முழு	    i;

    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(சாவி_names_table); i++)
	எனில் (c == சாவி_names_table[i].சாவி && !சாவி_names_table[i].is_alt)
	    திரும்பு சாவி_names_table[i].enabled ? i : -1;

    திரும்பு -1;
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
    நிலையான முழு
cmp_சாவி_name_entry(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    char_u  *p1 = ((கட்டமைப்பு சாவி_name_entry *)a)->name.string;
    char_u  *p2 = ((கட்டமைப்பு சாவி_name_entry *)b)->name.string;
    முழு	    result = 0;

    எனில் (p1 == p2)
	திரும்பு 0;

    போது (vim_isNormalIDc(*p1) && *p2 != NUL)
    {
	எனில் ((result = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    நிறுத்து;
	++p1;
	++p2;
    }

    எனில் (result == 0)
    {
	எனில் (*p2 == NUL)
	{
	    எனில் (vim_isNormalIDc(*p1))
		result = 1;
	}
	இல்லையெனில்
	{
	    result = -1;
	}
    }

    திரும்பு result;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    முழு
get_special_சாவி_code(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    எனில் (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	எனில் (add_termcap_entry(string, FALSE) == OK)
	    திரும்பு TERMCAP2_சாவி(name[2], name[3]);
    }
    இல்லையெனில்
    {
	கட்டமைப்பு சாவி_name_entry	target;
	கட்டமைப்பு சாவி_name_entry	*entry;

	target.enabled = TRUE;
	target.சாவி = 0;
	target.name.string = name;
	target.name.length = 0;

	entry = (கட்டமைப்பு சாவி_name_entry *)bsearch(
	    &target,
	    &சாவி_names_table,
	    ARRAY_LENGTH(சாவி_names_table),
	    அளவு(சாவி_names_table[0]),
	    cmp_சாவி_name_entry);
	எனில் (entry != வெற்று && entry->enabled)
	{
	    முழு சாவி = entry->சாவி;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    திரும்பு சாவி == K_TAB ? TAB : சாவி;
	}
    }

    திரும்பு 0;
}

    char_u *
get_சாவி_name(முழு i)
{
    எனில் (i < 0 || i >= (முழு)ARRAY_LENGTH(சாவி_names_table))
	திரும்பு வெற்று;

    திரும்பு சாவி_names_table[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    முழு
get_fileformat(buf_T *buf)
{
    முழு		c = *buf->b_p_ff;

    எனில் (buf->b_p_bin || c == 'u')
	திரும்பு EOL_UNIX;
    எனில் (c == 'm')
	திரும்பு EOL_MAC;
    திரும்பு EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    முழு
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    முழு		c;

    எனில் (eap != வெற்று && eap->force_ff != 0)
	c = eap->force_ff;
    இல்லையெனில்
    {
	எனில் ((eap != வெற்று && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    திரும்பு EOL_UNIX;
	c = *buf->b_p_ff;
    }
    எனில் (c == 'u')
	திரும்பு EOL_UNIX;
    எனில் (c == 'm')
	திரும்பு EOL_MAC;
    திரும்பு EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    வெறுமை
set_fileformat(
    முழு		t,
    முழு		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    எழுத்து	*p = வெற்று;

    தேர்வு (t)
    {
    நிலை EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	நிறுத்து;
    நிலை EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	நிறுத்து;
    நிலை EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	நிறுத்து;
    }
    எனில் (p != வெற்று)
	set_string_option_direct((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_status(curbuf);
    redraw_tabline = TRUE;
#எனில் defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    முழு
default_fileformat(வெறுமை)
{
    தேர்வு (*p_ffs)
    {
	நிலை 'm':   திரும்பு EOL_MAC;
	நிலை 'd':   திரும்பு EOL_DOS;
    }
    திரும்பு EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    முழு
call_shell(char_u *cmd, முழு opt)
{
    முழு		retval;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif

    எனில் (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == வெற்று ? p_sh : cmd);
	செய்தி_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
	prof_child_enter(&wait_time);
#endif

    எனில் (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_empty));
	retval = -1;
    }
    இல்லையெனில்
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

	எனில் (cmd == வெற்று || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	இல்லையெனில்
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    எனில் (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		எனில் (ecmd == வெற்று)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    எனில் (ncmd != வெற்று)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((எழுத்து *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    இல்லையெனில்
		retval = -1;
	    எனில் (ecmd != cmd)
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
    எனில் (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif

    திரும்பு retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    முழு
get_real_state(வெறுமை)
{
    எனில் (State & MODE_NORMAL)
    {
	எனில் (VIsual_active)
	{
	    எனில் (VIsual_select)
		திரும்பு MODE_SELECT;
	    திரும்பு MODE_VISUAL;
	}
	இல்லையெனில் எனில் (finish_op)
	    திரும்பு MODE_OP_PENDING;
    }
    திரும்பு State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    முழு
after_pathsep(char_u *b, char_u *p)
{
    திரும்பு p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_head_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    முழு
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    எனில் (f1 == வெற்று || f2 == வெற்று)
	திரும்பு FALSE;

    (வெறுமை)vim_FullName(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    திரும்பு (t1 - ffname == t2 - f2
	     && pathcmp((எழுத்து *)ffname, (எழுத்து *)f2, (முழு)(t1 - ffname)) == 0);
}

#எனில் defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    முழு
vim_chdirfile(char_u *fname, எழுத்து *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    எனில் (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    எனில் (pathcmp((எழுத்து *)old_dir, (எழுத்து *)new_dir, -1) == 0)
	// nothing to do
	திரும்பு OK;

    எனில் (trigger_autocmd != வெற்று)
	trigger_DirChangedPre((char_u *)trigger_autocmd, new_dir);

    எனில் (mch_chdir((எழுத்து *)new_dir) != 0)
	திரும்பு FAIL;

    எனில் (trigger_autocmd != வெற்று)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    திரும்பு OK;
}
#endif

#எனில் defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    நிலையான முழு
illegal_slash(மாறிலி எழுத்து *name)
{
    எனில் (name[0] == NUL)
	திரும்பு FALSE;	    // no file name is not illegal
    எனில் (name[strlen(name) - 1] != '/')
	திரும்பு FALSE;	    // no trailing slash
    எனில் (mch_isdir((char_u *)name))
	திரும்பு FALSE;	    // trailing slash for a directory
    திரும்பு TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    முழு
vim_stat(மாறிலி எழுத்து *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    திரும்பு illegal_slash(name) ? -1 : stat(name, stp);
}
#endif

#எனில் defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_table[SHAPE_IDX_மொத்தம்] =
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
நிலையான string_T mshape_names[] =
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
    {வெற்று, 0}
};

#  define MSHAPE_NAMES_மொத்தம்  (ARRAY_LENGTH(mshape_names) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    எழுத்து *
parse_shape_opt(முழு what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    முழு		idx = 0;		// init for GCC
    முழு		all_idx;
    முழு		len;
    முழு		i;
    long	n;
    முழு		found_ve = FALSE;	// found "ve" flag
    முழு		round;

    /*
     * First round: check for errors; second round: do it for real.
     */
    க்கு (round = 1; round <= 2; ++round)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	எனில் (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	இல்லையெனில்
# endif
	    modep = p_guicursor;
	போது (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    எனில் (colonp == வெற்று || (commap != வெற்று && commap < colonp))
		திரும்பு e_missing_colon_2;
	    எனில் (colonp == modep)
		திரும்பு e_illegal_mode;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    போது (modep < colonp || all_idx >= 0)
	    {
		எனில் (all_idx < 0)
		{
		    // Find the mode.
		    எனில் (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    இல்லையெனில்
			len = 2;
		    எனில் (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_மொத்தம் - 1;
		    இல்லையெனில்
		    {
			க்கு (idx = 0; idx < SHAPE_IDX_மொத்தம்; ++idx)
			    எனில் (STRNICMP(modep, shape_table[idx].name, len)
									 == 0)
				நிறுத்து;
			எனில் (idx == SHAPE_IDX_மொத்தம்
				   || (shape_table[idx].used_for & what) == 0)
			    திரும்பு e_illegal_mode;
			எனில் (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    found_ve = TRUE;
		    }
		    modep += len + 1;
		}

		எனில் (all_idx >= 0)
		    idx = all_idx--;
		இல்லையெனில் எனில் (round == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    எனில் (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_table[idx].mshape = 0;
		    }
		    இல்லையெனில்
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
		க்கு (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    எனில் (what == SHAPE_MOUSE)
		    {
			க்கு (i = 0; ; ++i)
			{
			    எனில் (mshape_names[i].string == வெற்று)
			    {
				எனில் (!VIM_ISDIGIT(*p))
				    திரும்பு e_illegal_mouseshape;
				எனில் (round == 2)
				    shape_table[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				இல்லையெனில்
				    (வெறுமை)getdigits(&p);
				நிறுத்து;
			    }
			    எனில் (STRNICMP(p, mshape_names[i].string, mshape_names[i].length) == 0)
			    {
				எனில் (round == 2)
				    shape_table[idx].mshape = i;
				p += mshape_names[i].length;
				நிறுத்து;
			    }
			}
		    }
		    இல்லையெனில் // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			எனில் (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			இல்லையெனில் எனில் (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			இல்லையெனில் எனில் (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			இல்லையெனில் எனில் (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			இல்லையெனில் எனில் (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			எனில் (len != 0)
			{
			    p += len;
			    எனில் (!VIM_ISDIGIT(*p))
				திரும்பு e_digit_expected;
			    n = getdigits(&p);
			    எனில் (len == 3)   // "ver" or "hor"
			    {
				எனில் (n == 0)
				    திரும்பு e_illegal_percentage;
				எனில் (round == 2)
				{
				    எனில் (TOLOWER_ASC(i) == 'v')
					shape_table[idx].shape = SHAPE_VER;
				    இல்லையெனில்
					shape_table[idx].shape = SHAPE_HOR;
				    shape_table[idx].percentage = n;
				}
			    }
			    இல்லையெனில் எனில் (round == 2)
			    {
				எனில் (len == 9)
				    shape_table[idx].blinkwait = n;
				இல்லையெனில் எனில் (len == 7)
				    shape_table[idx].blinkon = n;
				இல்லையெனில்
				    shape_table[idx].blinkoff = n;
			    }
			}
			இல்லையெனில் எனில் (STRNICMP(p, "block", 5) == 0)
			{
			    எனில் (round == 2)
				shape_table[idx].shape = SHAPE_BLOCK;
			    p += 5;
			}
			இல்லையெனில்	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    எனில் (commap == வெற்று)		    // last part
			    {
				எனில் (endp == வெற்று)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    இல்லையெனில் எனில் (endp > commap || endp == வெற்று)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    எனில் (slashp != வெற்று && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_group(p, (முழு)(slashp - p));
				p = slashp + 1;
			    }
			    எனில் (round == 2)
			    {
				shape_table[idx].id = syn_check_group(p,
							     (முழு)(endp - p));
				shape_table[idx].id_lm = shape_table[idx].id;
				எனில் (slashp != வெற்று && slashp < endp)
				    shape_table[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    எனில் (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    எனில் (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    எனில் (!found_ve)
    {
# ifdef FEAT_MOUSESHAPE
	எனில் (what == SHAPE_MOUSE)
	{
	    shape_table[SHAPE_IDX_VE].mshape = shape_table[SHAPE_IDX_V].mshape;
	}
	இல்லையெனில்
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

    திரும்பு வெற்று;
}

# எனில் defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    முழு
get_shape_idx(முழு mouse)
{
#  ifdef FEAT_MOUSESHAPE
    எனில் (mouse && (State == MODE_HITRETURN || State == MODE_ASKMORE))
    {
#   ifdef FEAT_GUI
	முழு x, y;
	gui_mch_getmouse(&x, &y);
	எனில் (Y_2_ROW(y) == Rows - 1)
	    திரும்பு SHAPE_IDX_MOREL;
#   endif
	திரும்பு SHAPE_IDX_MORE;
    }
    எனில் (mouse && drag_status_வரி)
	திரும்பு SHAPE_IDX_SDRAG;
    எனில் (mouse && drag_sep_வரி)
	திரும்பு SHAPE_IDX_VDRAG;
#  endif
    எனில் (!mouse && State == MODE_SHOWMATCH)
	திரும்பு SHAPE_IDX_SM;
    எனில் (State & VREPLACE_FLAG)
	திரும்பு SHAPE_IDX_R;
    எனில் (State & REPLACE_FLAG)
	திரும்பு SHAPE_IDX_R;
    எனில் (State & MODE_செருகு)
	திரும்பு SHAPE_IDX_I;
    எனில் (State & MODE_CMDLINE)
    {
	எனில் (cmdline_at_end())
	    திரும்பு SHAPE_IDX_C;
	எனில் (cmdline_overstrike())
	    திரும்பு SHAPE_IDX_CR;
	திரும்பு SHAPE_IDX_CI;
    }
    எனில் (finish_op)
	திரும்பு SHAPE_IDX_O;
    எனில் (VIsual_active)
    {
	எனில் (*p_sel == 'e')
	    திரும்பு SHAPE_IDX_VE;
	இல்லையெனில்
	    திரும்பு SHAPE_IDX_V;
    }
    திரும்பு SHAPE_IDX_N;
}
# endif

# எனில் defined(FEAT_MOUSESHAPE)
நிலையான முழு current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    வெறுமை
update_mouseshape(முழு shape_idx)
{
    முழு new_mouse_shape;

    // Only works in GUI mode.
    எனில் (!gui.in_use || gui.starting)
	திரும்பு;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    எனில் (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	திரும்பு;
    }

    // When ignoring the mouse don't change shape on the statusline.
    எனில் (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_STATUS
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    எனில் (shape_idx == -2
	    && current_mouse_shape != shape_table[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_STATUS].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_VSEP].mshape)
	திரும்பு;
    எனில் (shape_idx < 0)
	new_mouse_shape = shape_table[get_shape_idx(TRUE)].mshape;
    இல்லையெனில்
	new_mouse_shape = shape_table[shape_idx].mshape;
    எனில் (new_mouse_shape != current_mouse_shape)
    {
	mch_set_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#endif // CURSOR_SHAPE

#எனில் defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    வெறுமை
f_getmouseshape(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;
# எனில் defined(FEAT_MOUSESHAPE)
    எனில் (current_mouse_shape >= 0
			      && current_mouse_shape < (முழு)MSHAPE_NAMES_மொத்தம்)
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
    முழு
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    முழு		r;
    char_u	*file_to_find = வெற்று;
    எழுத்து	*தேடல்_ctx = வெற்று;

    dir_name = find_directory_in_path(new_dir, (முழு)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_to_find, &தேடல்_ctx);
    vim_free(file_to_find);
    vim_findfile_cleanup(தேடல்_ctx);
    எனில் (dir_name == வெற்று)
	திரும்பு -1;
    r = mch_chdir((எழுத்து *)dir_name);
    vim_free(dir_name);
    திரும்பு r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    முழு
get_user_name(char_u *buf, முழு len)
{
    எனில் (username == வெற்று)
    {
	எனில் (mch_get_user_name(buf, len) == FAIL)
	    திரும்பு FAIL;
	username = vim_strsave(buf);
    }
    இல்லையெனில்
	vim_strncpy(buf, username, len - 1);
    திரும்பு OK;
}

#எனில் defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    வெறுமை
free_username(வெறுமை)
{
    vim_free(username);
}
#endif

#எனில் !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    வெறுமை
qsort(
    வெறுமை	*base,
    size_t	elm_மொத்தம்,
    size_t	elm_size,
    முழு (*cmp)(மாறிலி வெறுமை *, மாறிலி வெறுமை *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    முழு		i, j;
    முழு		gap;

    buf = alloc(elm_size);
    எனில் (buf == வெற்று)
	திரும்பு;

    க்கு (gap = elm_மொத்தம் / 2; gap > 0; gap /= 2)
	க்கு (i = gap; i < elm_மொத்தம்; ++i)
	    க்கு (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)base + j * elm_size;
		p2 = (char_u *)base + (j + gap) * elm_size;
		எனில் ((*cmp)((வெறுமை *)p1, (வெறுமை *)p2) <= 0)
		    நிறுத்து;
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

#எனில் !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

நிலையான முழு  envsize = -1;	// current size of environment
வெளி எழுத்து **environ;		// the global which is your env.

நிலையான முழு  findenv(எழுத்து *name); // look for a name in the env.
நிலையான முழு  newenv(வெறுமை);	// copy env. from stack to heap
நிலையான முழு  moreenv(வெறுமை);	// incr. size of env.

    முழு
putenv(மாறிலி எழுத்து *string)
{
    முழு	    i;
    எழுத்து    *p;

    எனில் (envsize < 0)
    {				// first time putenv called
	எனில் (newenv() < 0)	// copy env. to heap
	    திரும்பு -1;
    }

    i = findenv((எழுத்து *)string); // look for name in environment

    எனில் (i < 0)
    {				// name must be added
	க்கு (i = 0; environ[i]; i++);
	எனில் (i >= (envsize - 1))
	{			// need new slot
	    எனில் (moreenv() < 0)
		திரும்பு -1;
	}
	p = alloc(strlen(string) + 1);
	எனில் (p == வெற்று)		// not enough core
	    திரும்பு -1;
	environ[i + 1] = 0;	// new end of env.
    }
    இல்லையெனில்
    {				// name already in env.
	p = vim_realloc(environ[i], strlen(string) + 1);
	எனில் (p == வெற்று)
	    திரும்பு -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    திரும்பு 0;
}

    நிலையான முழு
findenv(எழுத்து *name)
{
    எழுத்து    *namechar, *envchar;
    முழு	    i, found;

    found = 0;
    க்கு (i = 0; environ[i] && !found; i++)
    {
	envchar = environ[i];
	namechar = name;
	போது (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	found = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    திரும்பு found ? i - 1 : -1;
}

    நிலையான முழு
newenv(வெறுமை)
{
    எழுத்து    **env, *elem;
    முழு	    i, esize;

    க்கு (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(எழுத்து *, esize);
    எனில் (env == வெற்று)
	திரும்பு -1;

    க்கு (i = 0; environ[i]; i++)
    {
	elem = alloc(strlen(environ[i]) + 1);
	எனில் (elem == வெற்று)
	    திரும்பு -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    திரும்பு 0;
}

    நிலையான முழு
moreenv(வெறுமை)
{
    முழு	    esize;
    எழுத்து    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((எழுத்து *)environ, esize * அளவு (*env));
    எனில் (env == 0)
	திரும்பு -1;
    environ = env;
    envsize = esize;
    திரும்பு 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(மாறிலி char_u *string)
{
    முழு i;
    char_u *p;

    எனில் (envsize < 0)
	திரும்பு வெற்று;

    i = findenv((எழுத்து *)string);

    எனில் (i < 0)
	திரும்பு வெற்று;

    p = vim_strchr((char_u *)environ[i], '=');
    திரும்பு (p + 1);
}
# endif

#endif // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#எனில் defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    முழு
filewritable(char_u *fname)
{
    முழு		retval = 0;
# எனில் defined(UNIX) || defined(VMS)
    முழு		perm = 0;
# endif

# எனில் defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    எனில் (
# ifdef MSWIN
	    mch_writable(fname) &&
# இல்லையெனில்
#  எனில் defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((எழுத்து *)fname, W_OK) == 0
       )
    {
	++retval;
	எனில் (mch_isdir(fname))
	    ++retval;
    }
    திரும்பு retval;
}
#endif

#எனில் defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    முழு
get2c(FILE *fd)
{
    முழு		c, n;

    n = getc(fd);
    எனில் (n == EOF) திரும்பு -1;
    c = getc(fd);
    எனில் (c == EOF) திரும்பு -1;
    திரும்பு (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    முழு
get3c(FILE *fd)
{
    முழு		c, n;

    n = getc(fd);
    எனில் (n == EOF) திரும்பு -1;
    c = getc(fd);
    எனில் (c == EOF) திரும்பு -1;
    n = (n << 8) + c;
    c = getc(fd);
    எனில் (c == EOF) திரும்பு -1;
    திரும்பு (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    முழு
get4c(FILE *fd)
{
    முழு		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    unsigned	n;

    c = getc(fd);
    எனில் (c == EOF) திரும்பு -1;
    n = (unsigned)c;
    c = getc(fd);
    எனில் (c == EOF) திரும்பு -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    எனில் (c == EOF) திரும்பு -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    எனில் (c == EOF) திரும்பு -1;
    n = (n << 8) + (unsigned)c;
    திரும்பு (முழு)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, முழு cnt)
{
    char_u	*str;
    முழு		i;
    முழு		c;

    // allocate memory
    str = alloc(cnt + 1);
    எனில் (str == வெற்று)
	திரும்பு வெற்று;

    // Read the string.  Quit when running into the EOF.
    க்கு (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	எனில் (c == EOF)
	{
	    vim_free(str);
	    திரும்பு வெற்று;
	}
	str[i] = c;
    }
    str[i] = NUL;
    திரும்பு str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    முழு
put_bytes(FILE *fd, long_u nr, முழு len)
{
    முழு	    i;

    க்கு (i = len - 1; i >= 0; --i)
	எனில் (putc((முழு)(nr >> (i * 8)), fd) == EOF)
	    திரும்பு FAIL;
    திரும்பு OK;
}

#endif

#ifndef PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    long
elapsed(கட்டமைப்பு timeval *start_tv)
{
    கட்டமைப்பு timeval  now_tv;

    gettimeofday(&now_tv, வெற்று);
    திரும்பு (now_tv.tv_sec - start_tv->tv_sec) * 1000L
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
    DWORD	now = Get_Tick_மொத்தம்();

    திரும்பு (long)now - (long)start_tick;
}
# endif
#endif

#எனில் defined(FEAT_JOB_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    முழு
mch_parse_cmd(char_u *cmd, முழு use_shcf, எழுத்து ***argv, முழு *argc)
{
    முழு		i;
    char_u	*p, *d;
    முழு		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    க்கு (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	போது (*p != NUL)
	{
	    எனில் (i == 2)
		(*argv)[*argc] = (எழுத்து *)p;
	    ++*argc;
	    d = p;
	    போது (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		எனில் (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		இல்லையெனில்
		{
		    எனில் (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    எனில் (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    எனில் (*p == NUL)
	    {
		எனில் (i == 2)
		    *d++ = NUL;
		நிறுத்து;
	    }
	    எனில் (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	எனில் (*argv == வெற்று)
	{
	    எனில் (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		க்கு (;;)
		{
		    p = skiptowhite(p);
		    எனில் (*p == NUL)
			நிறுத்து;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(எழுத்து *, *argc + 4);
	    எனில் (*argv == வெற்று)	    // out of memory
		திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    முழு
build_argv_from_string(char_u *cmd, எழுத்து ***argv, முழு *argc)
{
    char_u	*cmd_copy;
    முழு		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    எனில் (cmd_copy == வெற்று
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	திரும்பு FAIL;
    }
    க்கு (i = 0; i < *argc; i++)
	(*argv)[i] = (எழுத்து *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = வெற்று;
    vim_free(cmd_copy);
    திரும்பு OK;
}

#endif

#எனில் defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    முழு
build_argv_from_list(list_T *l, எழுத்து ***argv, முழு *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(எழுத்து *, l->lv_len + 1);
    எனில் (*argv == வெற்று)
	திரும்பு FAIL;
    *argc = 0;
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	எனில் (s == வெற்று)
	{
	    முழு i;

	    க்கு (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = வெற்று;
	    திரும்பு FAIL;
	}
	(*argv)[*argc] = (எழுத்து *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = வெற்று;
    திரும்பு OK;
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
    முழு
get_special_pty_type(வெறுமை)
{
#ifdef MSWIN
    திரும்பு get_conpty_type();
#இல்லையெனில்
    திரும்பு 0;
#endif
}

// compare two keyvalue_T structs by case sensitive value
    முழு
cmp_keyvalue_மதிப்பு(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    திரும்பு STRCMP(kv1->மதிப்பு.string, kv2->மதிப்பு.string);
}

// compare two keyvalue_T structs by value with length
    முழு
cmp_keyvalue_மதிப்பு_n(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    திரும்பு STRNCMP(kv1->மதிப்பு.string, kv2->மதிப்பு.string, MAX(kv1->மதிப்பு.length,
		kv2->மதிப்பு.length));
}

// compare two keyvalue_T structs by case insensitive value
    முழு
cmp_keyvalue_மதிப்பு_i(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    திரும்பு STRICMP(kv1->மதிப்பு.string, kv2->மதிப்பு.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    முழு
cmp_keyvalue_மதிப்பு_ni(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    திரும்பு vim_strnicmp_asc((எழுத்து *)kv1->மதிப்பு.string,
	    (எழுத்து *)kv2->மதிப்பு.string, MAX(kv1->மதிப்பு.length,
		    kv2->மதிப்பு.length));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    வெறுமை *
mergesort_list(
    வெறுமை	*head,
    வெறுமை	*(*get_next)(வெறுமை *),
    வெறுமை	(*set_next)(வெறுமை *, வெறுமை *),
    வெறுமை	*(*get_prev)(வெறுமை *),
    வெறுமை	(*set_prev)(வெறுமை *, வெறுமை *),
    முழு		(*compare)(மாறிலி வெறுமை *, மாறிலி வெறுமை *))
{
    எனில் (!head || !get_next(head))
	திரும்பு head;

    // Count length
    முழு	    n = 0;
    வெறுமை*   curr = head;
    போது (curr)
    {
	n++;
	curr = get_next(curr);
    }

    முழு	size;
    க்கு (size = 1; size < n; size *= 2)
    {
	வெறுமை*	new_head = வெற்று;
	வெறுமை*	tail = வெற்று;
	curr = head;

	போது (curr)
	{
	    // Split two runs
	    வெறுமை    *இடது = curr;
	    வெறுமை    *வலது = இடது;
	    முழு	    i;
	    க்கு (i = 0; i < size && வலது; ++i)
		வலது = get_next(வலது);

	    வெறுமை    *next = வலது;
	    க்கு (i = 0; i < size && next; ++i)
		next = get_next(next);

	    // Break links
	    வெறுமை    *l_end = வலது ? get_prev(வலது) : வெற்று;
	    எனில் (l_end)
		set_next(l_end, வெற்று);
	    எனில் (வலது)
		set_prev(வலது, வெற்று);

	    வெறுமை    *r_end = next ? get_prev(next) : வெற்று;
	    எனில் (r_end)
		set_next(r_end, வெற்று);
	    எனில் (next)
		set_prev(next, வெற்று);

	    // Merge
	    வெறுமை    *merged = வெற்று;
	    வெறுமை    *merged_tail = வெற்று;

	    போது (இடது || வலது)
	    {
		வெறுமை	*chosen = வெற்று;
		எனில் (!இடது)
		{
		    chosen = வலது;
		    வலது = get_next(வலது);
		}
		இல்லையெனில் எனில் (!வலது)
		{
		    chosen = இடது;
		    இடது = get_next(இடது);
		}
		இல்லையெனில் எனில் (compare(இடது, வலது) <= 0)
		{
		    chosen = இடது;
		    இடது = get_next(இடது);
		}
		இல்லையெனில்
		{
		    chosen = வலது;
		    வலது = get_next(வலது);
		}

		எனில் (merged_tail)
		{
		    set_next(merged_tail, chosen);
		    set_prev(chosen, merged_tail);
		    merged_tail = chosen;
		}
		இல்லையெனில்
		{
		    merged = merged_tail = chosen;
		    set_prev(chosen, வெற்று);
		}
	    }

	    // Connect to full list
	    எனில் (!new_head)
		new_head = merged;
	    இல்லையெனில்
	    {
		set_next(tail, merged);
		set_prev(merged, tail);
	    }

	    // Move tail to end
	    போது (get_next(merged_tail))
		merged_tail = get_next(merged_tail);
	    tail = merged_tail;

	    curr = next;
	}

	head = new_head;
    }

    திரும்பு head;
}

நிலையான மாறிலி char_u base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

நிலையான char_u base64_dec_table[256];

#define BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    நிலையான வெறுமை
init_base64_dec_table(வெறுமை)
{
    நிலையான முழு base64_dec_tbl_initialized = FALSE;

    எனில் (base64_dec_tbl_initialized)
	திரும்பு;

    vim_memset(base64_dec_table, 0xFF, அளவு(base64_dec_table));
    க்கு (size_t i = 0; i < அளவு(base64_table) - 1; i++)
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
base64_encode_buf(char_u *dst, மாறிலி char_u *src, size_t len)
{
    size_t i, j;

    க்கு (i = 0, j = 0; i < len;)
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
    திரும்பு (long)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(மாறிலி char_u *data, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    எனில் (encoded == வெற்று)
	திரும்பு வெற்று;
    base64_encode_buf(encoded, data, len);
    திரும்பு encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    முழு
base64_decode(மாறிலி char_u *data, size_t len, garray_T *out)
{
    எனில் (len == 0)
	திரும்பு OK;

    எனில் (len % 4 != 0)
    {
	semsg(_(e_invalid_argument_str), data);
	திரும்பு FAIL;
    }

    init_base64_dec_table();

    size_t decoded_len = (len / 4) * 3;
    எனில் (data[len - 1] == '=')
	decoded_len--;
    எனில் (data[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    க்கு (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_table[(char_u)data[i++]];
	int_u sextet_b = base64_dec_table[(char_u)data[i++]];
	int_u sextet_c = base64_dec_table[(char_u)data[i++]];
	int_u sextet_d = base64_dec_table[(char_u)data[i++]];

	எனில் (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_argument_str), data);
	    ga_clear(out);
	    திரும்பு FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	எனில் (j < decoded_len)
	{
	    ga_append(out, (triple >> 16) & 0xFF);
	    j++;
	}
	எனில் (j < decoded_len)
	{
	    ga_append(out, (triple >> 8) & 0xFF);
	    j++;
	}
	எனில் (j < decoded_len)
	{
	    ga_append(out, triple & 0xFF);
	    j++;
	}

	எனில் (j == decoded_len)
	{
	    எனில் ((data[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((data[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_argument_str), data);
		ga_clear(out);
		திரும்பு FAIL;
	    }
	}
    }
    திரும்பு OK;
}
