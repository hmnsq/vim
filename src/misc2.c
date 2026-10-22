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
#અન્તર્ભાવ "vim.h"

સ્થિર char_u	*username = ખાલી; // cached result of mch_get_user_name()

સ્થિર પૂર્ણાંક coladvance2(pos_T *pos, પૂર્ણાંક addspaces, પૂર્ણાંક finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    પૂર્ણાંક
virtual_active(શૂન્ય)
{
    unsigned પૂર્ણાંક cur_ve_flags = get_ve_flags();

    જો (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_ઉમેરો) && (State & MODE_ઉમેરો)))
	પાછા TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    જો (virtual_op != MAYBE)
	પાછા virtual_op;
    પાછા (cur_ve_flags & VE_BLOCK) && VIsual_active && VIsual_mode == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    પૂર્ણાંક
getviscol(શૂન્ય)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, ખાલી, ખાલી, GETVCOL_NO_ABOVE);
    પાછા (પૂર્ણાંક)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    પૂર્ણાંક
coladvance_force(colnr_T wcol)
{
    પૂર્ણાંક rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    જો (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    નહીંતર
	// Virtcol is valid
	set_valid_virtcol(curwin, wcol);
    પાછા rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    પૂર્ણાંક
getviscol2(colnr_T col, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = col;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, ખાલી, ખાલી, GETVCOL_NO_ABOVE);
    પાછા (પૂર્ણાંક)x;
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
    પૂર્ણાંક
coladvance(colnr_T wantcol)
{
    પૂર્ણાંક rc = getvpos(&curwin->w_cursor, wantcol);

    જો (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    નહીંતર જો (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	set_valid_virtcol(curwin, wantcol);
    પાછા rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    પૂર્ણાંક
getvpos(pos_T *pos, colnr_T wantcol)
{
    પાછા coladvance2(pos, FALSE, virtual_active(), wantcol);
}

    સ્થિર પૂર્ણાંક
coladvance2(
    pos_T	*pos,
    પૂર્ણાંક		addspaces,	// change the text to achieve our goal?
    પૂર્ણાંક		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    પૂર્ણાંક		idx;
    char_u	*લીટી;
    પૂર્ણાંક		linelen;
    colnr_T	col = 0;
    પૂર્ણાંક		csize = 0;
    પૂર્ણાંક		one_more;
#ifdef FEAT_LINEBREAK
    પૂર્ણાંક		head = 0;
#endif

    one_more = (State & MODE_ઉમેરો)
		    || restart_edit != NUL
		    || (VIsual_active && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    લીટી = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    જો (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_more;
	    col = wcol;

	    જો ((addspaces || finetune) && !VIsual_active)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_more;
		જો (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    નહીંતર
    {
	પૂર્ણાંક		width = curwin->w_width - win_col_off(curwin);
	chartabsize_T	cts;

	જો (finetune
		&& curwin->w_p_wrap
		&& curwin->w_width != 0
		&& wcol >= (colnr_T)width
		&& width > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    જો (csize > 0)
		csize--;

	    જો (wcol / width > (colnr_T)csize / width
		    && ((State & MODE_ઉમેરો) == 0 || (પૂર્ણાંક)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / width + 1) * width - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, લીટી, લીટી);
#ifdef FEAT_PROP_POPUP
	cts.cts_no_above = true;
#endif
	જ્યારે (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ifdef FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &head, ખાલી);
	    MB_PTR_ADV(cts.cts_ptr);
#નહીંતર
	    csize = lbr_chartabsize_adv(&cts);
#endif
	    cts.cts_vcol += csize;
	}
	col = cts.cts_vcol;
	idx = (પૂર્ણાંક)(cts.cts_ptr - લીટી);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	જો (col > wcol || (!virtual_active() && one_more == 0))
	{
	    idx -= 1;
#ifdef FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= head;
#endif
	    col -= csize;
	}

	જો (virtual_active()
		&& addspaces
		&& wcol >= 0
		&& ((col != wcol && col != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    જો (લીટી[idx] == NUL)
	    {
		// Append spaces
		પૂર્ણાંક	correct = wcol - col;
		char_u	*newline = alloc(idx + correct + 1);
		પૂર્ણાંક	t;

		જો (newline == ખાલી)
		    પાછા FAIL;

		માટે (t = 0; t < idx; ++t)
		    newline[t] = લીટી[t];

		માટે (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, (colnr_T)idx);
		idx += correct;
		col = wcol;
	    }
	    નહીંતર
	    {
		// Break a tab
		પૂર્ણાંક	correct = wcol - col - csize + 1; // negative!!
		char_u	*newline;
		પૂર્ણાંક	t, s = 0;
		પૂર્ણાંક	v;

		જો (-correct > csize)
		    પાછા FAIL;

		newline = alloc(linelen + csize);
		જો (newline == ખાલી)
		    પાછા FAIL;

		માટે (t = 0; t < linelen; t++)
		{
		    જો (t != idx)
			newline[s++] = લીટી[t];
		    નહીંતર
			માટે (v = 0; v < csize; v++)
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

    જો (idx < 0)
	pos->col = 0;
    નહીંતર
	pos->col = idx;

    pos->coladd = 0;

    જો (finetune)
    {
	જો (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    જો (!one_more)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, ખાલી, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	નહીંતર
	{
	    પૂર્ણાંક b = (પૂર્ણાંક)wcol - (પૂર્ણાંક)col;

	    // The difference between wcol and col is used to set coladd.
	    જો (b > 0 && b < (MAXCOL - 2 * curwin->w_width))
		pos->coladd = b;

	    col += b;
	}
    }

    // prevent from moving onto a trail byte
    જો (has_mbyte)
	mb_adjustpos(curbuf, pos);

    જો (wcol < 0 || col < wcol)
	પાછા FAIL;
    પાછા OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    પૂર્ણાંક
inc_cursor(શૂન્ય)
{
    પાછા inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    પૂર્ણાંક
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    જો (lp->col != MAXCOL)
    {
	p = ml_get_pos(lp);
	જો (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    જો (has_mbyte)
	    {
		પૂર્ણાંક l = (*mb_ptr2len)(p);

		lp->col += l;
		પાછા ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->col++;
	    lp->coladd = 0;
	    પાછા ((p[1] != NUL) ? 0 : 2);
	}
    }
    જો (lp->lnum != curbuf->b_ml.ml_લીટી_ગણતરી)     // there is a next line
    {
	lp->col = 0;
	lp->lnum++;
	lp->coladd = 0;
	પાછા 1;
    }
    પાછા -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    પૂર્ણાંક
incl(pos_T *lp)
{
    પૂર્ણાંક	    r;

    જો ((r = inc(lp)) >= 1 && lp->col)
	r = inc(lp);
    પાછા r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    પૂર્ણાંક
dec_cursor(શૂન્ય)
{
    પાછા dec(&curwin->w_cursor);
}

    પૂર્ણાંક
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    જો (lp->col == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	જો (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	પાછા 0;
    }

    જો (lp->col > 0)
    {
	// still within line
	lp->col--;
	જો (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	}
	પાછા 0;
    }

    જો (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	જો (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	પાછા 1;
    }

    // at start of file
    પાછા -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    પૂર્ણાંક
decl(pos_T *lp)
{
    પૂર્ણાંક	    r;

    જો ((r = dec(lp)) == 1 && lp->col)
	r = dec(lp);
    પાછા r;
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
    જો (hasAnyFolding(wp))
    {
	જો (lnum > cursor)
	{
	    જ્યારે (lnum > cursor)
	    {
		(શૂન્ય)hasFoldingWin(wp, lnum, &lnum, ખાલી, TRUE, ખાલી);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		જો (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	નહીંતર જો (lnum < cursor)
	{
	    જ્યારે (lnum < cursor)
	    {
		(શૂન્ય)hasFoldingWin(wp, lnum, ખાલી, &lnum, TRUE, ખાલી);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		જો (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    નહીંતર
#endif
	retval = lnum - cursor;

    પાછા retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    શૂન્ય
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    જો (pos->lnum > buf->b_ml.ml_લીટી_ગણતરી)
	pos->lnum = buf->b_ml.ml_લીટી_ગણતરી;

    જો (pos->col > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	જો (pos->col > len)
	    pos->col = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    શૂન્ય
check_cursor_lnum(શૂન્ય)
{
    જો (curwin->w_cursor.lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
    {
#ifdef FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	જો (!hasFolding(curbuf->b_ml.ml_લીટી_ગણતરી,
						&curwin->w_cursor.lnum, ખાલી))
#endif
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
    }
    જો (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    શૂન્ય
check_cursor_col(શૂન્ય)
{
    check_cursor_col_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    શૂન્ય
check_cursor_col_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.col;
    colnr_T      oldcoladd = win->w_cursor.col + win->w_cursor.coladd;
    unsigned પૂર્ણાંક cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_buffer, win->w_cursor.lnum);
    જો (len == 0)
	win->w_cursor.col = 0;
    નહીંતર જો (win->w_cursor.col >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	જો ((State & MODE_ઉમેરો) || restart_edit
		|| (VIsual_active && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_active())
	    win->w_cursor.col = len;
	નહીંતર
	{
	    win->w_cursor.col = len - 1;
	    // Move the cursor to the head byte.
	    જો (has_mbyte)
		mb_adjustpos(win->w_buffer, &win->w_cursor);
	}
    }
    નહીંતર જો (win->w_cursor.col < 0)
	win->w_cursor.col = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    જો (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    નહીંતર જો (cur_ve_flags == VE_ALL)
    {
	જો (oldcoladd > win->w_cursor.col)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.col;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    જો (win->w_cursor.col + 1 < len)
	    {
		પૂર્ણાંક cs, ce;

		getvcol(win, &win->w_cursor, &cs, ખાલી, &ce, 0);
		જો (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	નહીંતર
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    શૂન્ય
check_cursor(શૂન્ય)
{
    check_cursor_lnum();
    check_cursor_col();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    શૂન્ય
check_visual_pos(શૂન્ય)
{
    જો (VIsual.lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	VIsual.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	VIsual.col = 0;
	VIsual.coladd = 0;
    }
    નહીંતર
    {
	પૂર્ણાંક len = ml_get_len(VIsual.lnum);

	જો (VIsual.col > len)
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
    શૂન્ય
adjust_cursor_col(શૂન્ય)
{
    જો (curwin->w_cursor.col > 0
	    && (!VIsual_active || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    પૂર્ણાંક
set_leftcol(colnr_T leftcol)
{
    પૂર્ણાંક		retval = FALSE;

    // Return quickly when there is no change.
    જો (curwin->w_leftcol == leftcol)
	પાછા FALSE;
    curwin->w_leftcol = leftcol;

    changed_cline_bef_curs();
    long lastcol = curwin->w_leftcol + curwin->w_width - curwin_col_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    long siso = get_sidescrolloff_કિંમત();
    જો (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    નહીંતર જો (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(શૂન્ય)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, ખાલી, &e, 0);
    જો (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    નહીંતર જો (s < curwin->w_leftcol)
    {
	retval = TRUE;
	જો (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    changed_cline_bef_curs();
	}
    }

    જો (retval)
	curwin->w_set_curswant = true;
    redraw_later(UPD_NOT_VALID);
    પાછા retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    પૂર્ણાંક
copy_option_part(
    char_u	**option,
    char_u	*buf,
    પૂર્ણાંક		maxlen,
    અક્ષર	*sep_chars)
{
    પૂર્ણાંક	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    જો (*p == '.')
	buf[len++] = *p++;
    જ્યારે (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == ખાલી)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	જો (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != ખાલી)
	    ++p;
	જો (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    જો (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_to_option_part(p);	// p points to next file name

    *option = p;
    પાછા len;
}

#જો !defined(HAVE_MEMSET) && !defined(PROTO)
    શૂન્ય *
vim_memset(શૂન્ય *ptr, પૂર્ણાંક c, size_t size)
{
    અક્ષર *p = ptr;

    જ્યારે (size-- > 0)
	*p++ = c;
    પાછા ptr;
}
#endif

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    પૂર્ણાંક
vim_isspace(પૂર્ણાંક x)
{
    પાછા ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

સ્થિર સંરચના modmasktable
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
#જો defined(MACOS_X) || defined(FEAT_GUI_GTK)
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
#define MOD_ચાવીઓ_ENTRY_SIZE 5

સ્થિર char_u modifier_ચાવીઓ_table[] =
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
    MOD_MASK_CTRL,  KS_EXTRA, (પૂર્ણાંક)KE_C_END,	'@', '7',	// end
    MOD_MASK_SHIFT, '*', '9',			'@', '9',	// exit
    MOD_MASK_SHIFT, '*', '0',			'@', '0',	// find
    MOD_MASK_SHIFT, '#', '1',			'%', '1',	// help
    MOD_MASK_SHIFT, '#', '2',			'k', 'h',	// home
    MOD_MASK_CTRL,  KS_EXTRA, (પૂર્ણાંક)KE_C_HOME,	'k', 'h',	// home
    MOD_MASK_SHIFT, '#', '3',			'k', 'I',	// insert
    MOD_MASK_SHIFT, '#', '4',			'k', 'l',	// left arrow
    MOD_MASK_CTRL,  KS_EXTRA, (પૂર્ણાંક)KE_C_ડાબું,	'k', 'l',	// left arrow
    MOD_MASK_SHIFT, '%', 'a',			'%', '3',	// message
    MOD_MASK_SHIFT, '%', 'b',			'%', '4',	// move
    MOD_MASK_SHIFT, '%', 'c',			'%', '5',	// next
    MOD_MASK_SHIFT, '%', 'd',			'%', '7',	// options
    MOD_MASK_SHIFT, '%', 'e',			'%', '8',	// previous
    MOD_MASK_SHIFT, '%', 'f',			'%', '9',	// print
    MOD_MASK_SHIFT, '%', 'g',			'%', '0',	// redo
    MOD_MASK_SHIFT, '%', 'h',			'&', '3',	// replace
    MOD_MASK_SHIFT, '%', 'i',			'k', 'r',	// right arr.
    MOD_MASK_CTRL,  KS_EXTRA, (પૂર્ણાંક)KE_C_જમણું,	'k', 'r',	// right arr.
    MOD_MASK_SHIFT, '%', 'j',			'&', '5',	// resume
    MOD_MASK_SHIFT, '!', '1',			'&', '6',	// save
    MOD_MASK_SHIFT, '!', '2',			'&', '7',	// suspend
    MOD_MASK_SHIFT, '!', '3',			'&', '8',	// undo
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_UP,	'k', 'u',	// up arrow
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_DOWN,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_XF1,	KS_EXTRA, (પૂર્ણાંક)KE_XF1,
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_XF2,	KS_EXTRA, (પૂર્ણાંક)KE_XF2,
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_XF3,	KS_EXTRA, (પૂર્ણાંક)KE_XF3,
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_XF4,	KS_EXTRA, (પૂર્ણાંક)KE_XF4,

    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F1,	'k', '1',	// F1
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F2,	'k', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F3,	'k', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F4,	'k', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F5,	'k', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F6,	'k', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F7,	'k', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F8,	'k', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F9,	'k', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F10,	'k', ';',	// F10

    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F11,	'F', '1',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F12,	'F', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F13,	'F', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F14,	'F', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F15,	'F', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F16,	'F', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F17,	'F', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F18,	'F', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F19,	'F', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F20,	'F', 'A',

    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F21,	'F', 'B',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F22,	'F', 'C',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F23,	'F', 'D',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F24,	'F', 'E',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F25,	'F', 'F',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F26,	'F', 'G',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F27,	'F', 'H',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F28,	'F', 'I',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F29,	'F', 'J',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F30,	'F', 'K',

    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F31,	'F', 'L',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F32,	'F', 'M',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F33,	'F', 'N',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F34,	'F', 'O',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F35,	'F', 'P',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F36,	'F', 'Q',
    MOD_MASK_SHIFT, KS_EXTRA, (પૂર્ણાંક)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_MASK_SHIFT, 'k', 'B',			KS_EXTRA, (પૂર્ણાંક)KE_TAB,

    NUL
};

સ્થિર સંરચના ચાવી_name_entry
{
    પૂર્ણાંક		enabled;	    // is this entry available (TRUE/FALSE)?
    પૂર્ણાંક		ચાવી;		    // special key code or ascii value
    string_T	name;		    // name of key
    પૂર્ણાંક		is_alt;		    // is an alternative name
} ચાવી_names_table[] =
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
#નહીંતર
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
#નહીંતર
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
    {TRUE, K_ડાબું, STR_LITERAL_INIT("Left"), FALSE},
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
#નહીંતર
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
#નહીંતર
    FALSE,
#endif
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_જમણું, STR_LITERAL_INIT("Right"), FALSE},
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
#નહીંતર
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
#નહીંતર
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
    સ્થિર પૂર્ણાંક
name_to_mod_mask(પૂર્ણાંક c)
{
    પૂર્ણાંક	    i;

    c = TOUPPER_ASC(c);
    માટે (i = 0; mod_mask_table[i].mod_mask != 0; i++)
	જો (c == mod_mask_table[i].name)
	    પાછા mod_mask_table[i].mod_flag;
    પાછા 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    પૂર્ણાંક
simplify_ચાવી(પૂર્ણાંક ચાવી, પૂર્ણાંક *modifiers)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    key0;
    પૂર્ણાંક	    key1;

    જો (!(*modifiers & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	પાછા ચાવી;

    // TAB is a special case
    જો (ચાવી == TAB && (*modifiers & MOD_MASK_SHIFT))
    {
	*modifiers &= ~MOD_MASK_SHIFT;
	પાછા K_S_TAB;
    }
    key0 = KEY2TERMCAP0(ચાવી);
    key1 = KEY2TERMCAP1(ચાવી);
    માટે (i = 0; modifier_ચાવીઓ_table[i] != NUL; i += MOD_ચાવીઓ_ENTRY_SIZE)
    {
	જો (key0 == modifier_ચાવીઓ_table[i + 3]
		&& key1 == modifier_ચાવીઓ_table[i + 4]
		&& (*modifiers & modifier_ચાવીઓ_table[i]))
	{
	    *modifiers &= ~modifier_ચાવીઓ_table[i];
	    પાછા TERMCAP2_ચાવી(modifier_ચાવીઓ_table[i + 1],
		    modifier_ચાવીઓ_table[i + 2]);
	}
    }
    પાછા ચાવી;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    પૂર્ણાંક
handle_x_ચાવીઓ(પૂર્ણાંક ચાવી)
{
    પસંદગી (ચાવી)
    {
	સ્થિતિ K_XUP:	પાછા K_UP;
	સ્થિતિ K_XDOWN:	પાછા K_DOWN;
	સ્થિતિ K_XLEFT:	પાછા K_ડાબું;
	સ્થિતિ K_XRIGHT:	પાછા K_જમણું;
	સ્થિતિ K_XHOME:	પાછા K_HOME;
	સ્થિતિ K_ZHOME:	પાછા K_HOME;
	સ્થિતિ K_XEND:	પાછા K_END;
	સ્થિતિ K_ZEND:	પાછા K_END;
	સ્થિતિ K_XF1:	પાછા K_F1;
	સ્થિતિ K_XF2:	પાછા K_F2;
	સ્થિતિ K_XF3:	પાછા K_F3;
	સ્થિતિ K_XF4:	પાછા K_F4;
	સ્થિતિ K_S_XF1:	પાછા K_S_F1;
	સ્થિતિ K_S_XF2:	પાછા K_S_F2;
	સ્થિતિ K_S_XF3:	પાછા K_S_F3;
	સ્થિતિ K_S_XF4:	પાછા K_S_F4;
    }
    પાછા ચાવી;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_ચાવી_name(પૂર્ણાંક c, પૂર્ણાંક modifiers)
{
    સ્થિર char_u string[MAX_ચાવી_NAME_LEN + 1];
    પૂર્ણાંક	    i, idx, len;
    પૂર્ણાંક	    table_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    જો (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_ચાવી)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    જો (IS_SPECIAL(c))
    {
	માટે (i = 0; modifier_ચાવીઓ_table[i] != 0; i += MOD_ચાવીઓ_ENTRY_SIZE)
	    જો (       KEY2TERMCAP0(c) == (પૂર્ણાંક)modifier_ચાવીઓ_table[i + 1]
		    && (પૂર્ણાંક)KEY2TERMCAP1(c) == (પૂર્ણાંક)modifier_ચાવીઓ_table[i + 2])
	    {
		modifiers |= modifier_ચાવીઓ_table[i];
		c = TERMCAP2_ચાવી(modifier_ચાવીઓ_table[i + 3],
						   modifier_ચાવીઓ_table[i + 4]);
		તોડો;
	    }
    }

    // try to find the key in the special key table
    table_idx = find_special_ચાવી_in_table(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    જો (c > 0 && (*mb_char2len)(c) == 1)
    {
	જો (table_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_MASK_ALT;
	    // try again, to find the un-alted key in the special key table
	    table_idx = find_special_ચાવી_in_table(c);
	}
	જો (table_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_MASK_CTRL;
	}
    }

    // translate the modifier into a string
    માટે (i = 0; mod_mask_table[i].name != 'A'; i++)
	જો ((modifiers & mod_mask_table[i].mod_mask)
						== mod_mask_table[i].mod_flag)
	{
	    string[idx++] = mod_mask_table[i].name;
	    string[idx++] = (char_u)'-';
	}

    જો (table_idx < 0)		// unknown special key, may output t_xx
    {
	જો (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	નહીંતર
	{
	    len = (*mb_char2len)(c);
	    જો (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    નહીંતર જો (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    નહીંતર
	    {
		char_u	*s = transchar(c);
		જ્યારે (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    નહીંતર		// use name of special key
    {
	string_T    *s;

	s = &ચાવી_names_table[table_idx].name;

	જો (s->length + idx + 2 <= MAX_ચાવી_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (પૂર્ણાંક)s->length;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    પાછા string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    પૂર્ણાંક
trans_special(
    char_u	**srcp,
    char_u	*dst,
    પૂર્ણાંક		flags,		// FSK_ values
    પૂર્ણાંક		escape_ks,	// escape K_SPECIAL bytes in the character
    પૂર્ણાંક		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    પૂર્ણાંક		modifiers = 0;
    પૂર્ણાંક		ચાવી;

    ચાવી = find_special_ચાવી(srcp, &modifiers, flags, did_simplify);
    જો (ચાવી == 0)
	પાછા 0;

    પાછા special_to_buf(ચાવી, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    પૂર્ણાંક
special_to_buf(પૂર્ણાંક ચાવી, પૂર્ણાંક modifiers, પૂર્ણાંક escape_ks, char_u *dst)
{
    પૂર્ણાંક		dlen = 0;

    // Put the appropriate modifier in a string
    જો (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    જો (IS_SPECIAL(ચાવી))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(ચાવી);
	dst[dlen++] = KEY2TERMCAP1(ચાવી);
    }
    નહીંતર જો (escape_ks)
	dlen = (પૂર્ણાંક)(add_char2buf(ચાવી, dst + dlen) - dst);
    નહીંતર જો (has_mbyte)
	dlen += (*mb_char2bytes)(ચાવી, dst + dlen);
    નહીંતર
	dst[dlen++] = ચાવી;

    પાછા dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    પૂર્ણાંક
find_special_ચાવી(
    char_u	**srcp,
    પૂર્ણાંક		*modp,
    પૂર્ણાંક		flags,		// FSK_ values
    પૂર્ણાંક		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*last_dash;
    char_u	*end_of_name;
    char_u	*src;
    char_u	*bp;
    પૂર્ણાંક		in_string = flags & FSK_IN_STRING;
    પૂર્ણાંક		modifiers;
    પૂર્ણાંક		bit;
    પૂર્ણાંક		ચાવી;
    uvarnumber_T	n;
    પૂર્ણાંક		l;

    src = *srcp;
    જો (src[0] != '<')
	પાછા 0;
    જો (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    last_dash = src;
    માટે (bp = src + 1; *bp == '-' || vim_isNormalIDc(*bp); bp++)
    {
	જો (*bp == '-')
	{
	    last_dash = bp;
	    જો (bp[1] != NUL)
	    {
		જો (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		નહીંતર
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		જો (!(in_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		નહીંતર જો (in_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	જો (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	નહીંતર જો (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, ખાલી, &l, STR2NR_ALL, ખાલી, ખાલી, 0, TRUE, ખાલી);
	    જો (l == 0)
	    {
		emsg(_(e_invalid_argument));
		પાછા 0;
	    }
	    bp += l + 5;
	    તોડો;
	}
    }

    જો (*bp == '>')	// found matching '>'
    {
	end_of_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	માટે (bp = src + 1; bp < last_dash; bp++)
	{
	    જો (*bp != '-')
	    {
		bit = name_to_mod_mask(*bp);
		જો (bit == 0x0)
		    તોડો;	// Illegal modifier name
		modifiers |= bit;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	જો (bp >= last_dash)
	{
	    જો (STRNICMP(last_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(last_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(last_dash + 6, ખાલી, &l, STR2NR_ALL, ખાલી,
							    &n, 0, TRUE, ખાલી);
		જો (l == 0)
		{
		    emsg(_(e_invalid_argument));
		    પાછા 0;
		}
		ચાવી = (પૂર્ણાંક)n;
	    }
	    નહીંતર
	    {
		પૂર્ણાંક off = 1;

		// Modifier with single letter, or special key name.
		જો (in_string && last_dash[1] == '\\' && last_dash[2] == '"')
		    off = 2;
		જો (has_mbyte)
		    l = mb_ptr2len(last_dash + off);
		નહીંતર
		    l = 1;
		જો (modifiers != 0 && last_dash[l + off] == '>')
		    ચાવી = PTR2CHAR(last_dash + off);
		નહીંતર
		{
		    ચાવી = get_special_ચાવી_code(last_dash + off);
		    જો (!(flags & FSK_KEEP_X_ચાવી))
			ચાવી = handle_x_ચાવીઓ(ચાવી);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    જો (ચાવી != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		ચાવી = simplify_ચાવી(ચાવી, &modifiers);

		જો ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    જો (ચાવી == K_BS)
			ચાવી = BS;
		    નહીંતર જો (ચાવી == K_DEL || ચાવી == K_KDEL)
			ચાવી = DEL;
		}
		નહીંતર જો (ચાવી == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    જો ((flags & FSK_SIMPLIFY) != 0)
		    {
			જો (did_simplify != ખાલી)
			    *did_simplify = TRUE;
		    }
		    નહીંતર
			ચાવી = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		જો (!IS_SPECIAL(ચાવી))
		    ચાવી = extract_modifiers(ચાવી, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = end_of_name;
		પાછા ચાવી;
	    }
	}
    }
    પાછા 0;
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
    પૂર્ણાંક
may_adjust_ચાવી_for_ctrl(પૂર્ણાંક modifiers, પૂર્ણાંક ચાવી)
{
    જો ((modifiers & MOD_MASK_CTRL) == 0)
	પાછા ચાવી;

    જો (ASCII_ISALPHA(ચાવી))
    {
#ifdef FEAT_TERMINAL
	check_no_reduce_ચાવીઓ();  // may update the no_reduce_keys flag
#endif
	પાછા no_reduce_ચાવીઓ == 0 ? TOUPPER_ASC(ચાવી) : ચાવી;
    }
    જો (ચાવી == '2')
	પાછા '@';
    જો (ચાવી == '6')
	પાછા '^';
    જો (ચાવી == '-')
	પાછા '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    જો (ચાવી == '$' && (modifiers & MOD_MASK_SHIFT) == 0)
	પાછા ']';

    પાછા ચાવી;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    પૂર્ણાંક
may_remove_shift_modifier(પૂર્ણાંક modifiers, પૂર્ણાંક ચાવી)
{
    જો ((modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
#ifdef FEAT_GUI_GTK
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_CMD)
#endif
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    && ((ચાવી >= '!' && ચાવી <= '/')
		|| (ચાવી >= ':' && ચાવી <= 'Z')
		|| vim_isdigit(ચાવી)
		|| (ચાવી >= '[' && ચાવી <= '`')
		|| (ચાવી >= '{' && ચાવી <= '~')))
	પાછા modifiers & ~MOD_MASK_SHIFT;

    જો (modifiers == (MOD_MASK_SHIFT | MOD_MASK_CTRL)
		&& (ચાવી == '{' || ચાવી == '}' || ચાવી == '|'))
	પાછા modifiers & ~MOD_MASK_SHIFT;

    પાછા modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    પૂર્ણાંક
extract_modifiers(પૂર્ણાંક ચાવી, પૂર્ણાંક *modp, પૂર્ણાંક simplify, પૂર્ણાંક *did_simplify)
{
    પૂર્ણાંક	modifiers = *modp;

#ifdef MACOS_X
    // Command-key really special, no fancynest
    જો (!(modifiers & MOD_MASK_CMD))
#endif
    જો ((modifiers & MOD_MASK_SHIFT) && ASCII_ISALPHA(ચાવી))
    {
	ચાવી = TOUPPER_ASC(ચાવી);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	જો (simplify || modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    modifiers &= ~MOD_MASK_SHIFT;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    જો ((modifiers & MOD_MASK_CTRL) && ASCII_ISALPHA(ચાવી))
	ચાવી = TOUPPER_ASC(ચાવી);

    જો (simplify && (modifiers & MOD_MASK_CTRL)
	    && ((ચાવી >= '?' && ચાવી <= '_') || ASCII_ISALPHA(ચાવી)))
    {
	ચાવી = Ctrl_chr(ચાવી);
	modifiers &= ~MOD_MASK_CTRL;
	// <C-@> is <Nul>
	જો (ચાવી == NUL)
	    ચાવી = K_ZERO;
	જો (did_simplify != ખાલી)
	    *did_simplify = TRUE;
    }

#ifdef MACOS_X
    // Command-key really special, no fancynest
    જો (!(modifiers & MOD_MASK_CMD))
#endif
    જો (simplify && (modifiers & MOD_MASK_ALT) && ચાવી < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	ચાવી |= 0x80;
	modifiers &= ~MOD_MASK_ALT;	// remove the META modifier
	જો (did_simplify != ખાલી)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    પાછા ચાવી;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    પૂર્ણાંક
find_special_ચાવી_in_table(પૂર્ણાંક c)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(ચાવી_names_table); i++)
	જો (c == ચાવી_names_table[i].ચાવી && !ચાવી_names_table[i].is_alt)
	    પાછા ચાવી_names_table[i].enabled ? i : -1;

    પાછા -1;
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
    સ્થિર પૂર્ણાંક
cmp_ચાવી_name_entry(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    char_u  *p1 = ((સંરચના ચાવી_name_entry *)a)->name.string;
    char_u  *p2 = ((સંરચના ચાવી_name_entry *)b)->name.string;
    પૂર્ણાંક	    result = 0;

    જો (p1 == p2)
	પાછા 0;

    જ્યારે (vim_isNormalIDc(*p1) && *p2 != NUL)
    {
	જો ((result = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    તોડો;
	++p1;
	++p2;
    }

    જો (result == 0)
    {
	જો (*p2 == NUL)
	{
	    જો (vim_isNormalIDc(*p1))
		result = 1;
	}
	નહીંતર
	{
	    result = -1;
	}
    }

    પાછા result;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    પૂર્ણાંક
get_special_ચાવી_code(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    જો (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	જો (add_termcap_entry(string, FALSE) == OK)
	    પાછા TERMCAP2_ચાવી(name[2], name[3]);
    }
    નહીંતર
    {
	સંરચના ચાવી_name_entry	target;
	સંરચના ચાવી_name_entry	*entry;

	target.enabled = TRUE;
	target.ચાવી = 0;
	target.name.string = name;
	target.name.length = 0;

	entry = (સંરચના ચાવી_name_entry *)bsearch(
	    &target,
	    &ચાવી_names_table,
	    ARRAY_LENGTH(ચાવી_names_table),
	    કદ(ચાવી_names_table[0]),
	    cmp_ચાવી_name_entry);
	જો (entry != ખાલી && entry->enabled)
	{
	    પૂર્ણાંક ચાવી = entry->ચાવી;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    પાછા ચાવી == K_TAB ? TAB : ચાવી;
	}
    }

    પાછા 0;
}

    char_u *
get_ચાવી_name(પૂર્ણાંક i)
{
    જો (i < 0 || i >= (પૂર્ણાંક)ARRAY_LENGTH(ચાવી_names_table))
	પાછા ખાલી;

    પાછા ચાવી_names_table[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    પૂર્ણાંક
get_fileformat(buf_T *buf)
{
    પૂર્ણાંક		c = *buf->b_p_ff;

    જો (buf->b_p_bin || c == 'u')
	પાછા EOL_UNIX;
    જો (c == 'm')
	પાછા EOL_MAC;
    પાછા EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    પૂર્ણાંક
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    પૂર્ણાંક		c;

    જો (eap != ખાલી && eap->force_ff != 0)
	c = eap->force_ff;
    નહીંતર
    {
	જો ((eap != ખાલી && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    પાછા EOL_UNIX;
	c = *buf->b_p_ff;
    }
    જો (c == 'u')
	પાછા EOL_UNIX;
    જો (c == 'm')
	પાછા EOL_MAC;
    પાછા EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    શૂન્ય
set_fileformat(
    પૂર્ણાંક		t,
    પૂર્ણાંક		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    અક્ષર	*p = ખાલી;

    પસંદગી (t)
    {
    સ્થિતિ EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	તોડો;
    સ્થિતિ EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	તોડો;
    સ્થિતિ EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	તોડો;
    }
    જો (p != ખાલી)
	set_string_option_direct((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_status(curbuf);
    redraw_tabline = TRUE;
#જો defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    પૂર્ણાંક
default_fileformat(શૂન્ય)
{
    પસંદગી (*p_ffs)
    {
	સ્થિતિ 'm':   પાછા EOL_MAC;
	સ્થિતિ 'd':   પાછા EOL_DOS;
    }
    પાછા EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    પૂર્ણાંક
call_shell(char_u *cmd, પૂર્ણાંક opt)
{
    પૂર્ણાંક		retval;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif

    જો (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == ખાલી ? p_sh : cmd);
	સંદેશ_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ifdef FEAT_PROFILE
    જો (do_profiling == PROF_YES)
	prof_child_enter(&wait_time);
#endif

    જો (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_empty));
	retval = -1;
    }
    નહીંતર
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

	જો (cmd == ખાલી || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	નહીંતર
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    જો (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		જો (ecmd == ખાલી)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    જો (ncmd != ખાલી)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((અક્ષર *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    નહીંતર
		retval = -1;
	    જો (ecmd != cmd)
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
    જો (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif

    પાછા retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    પૂર્ણાંક
get_real_state(શૂન્ય)
{
    જો (State & MODE_NORMAL)
    {
	જો (VIsual_active)
	{
	    જો (VIsual_select)
		પાછા MODE_SELECT;
	    પાછા MODE_VISUAL;
	}
	નહીંતર જો (finish_op)
	    પાછા MODE_OP_PENDING;
    }
    પાછા State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    પૂર્ણાંક
after_pathsep(char_u *b, char_u *p)
{
    પાછા p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_head_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    પૂર્ણાંક
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    જો (f1 == ખાલી || f2 == ખાલી)
	પાછા FALSE;

    (શૂન્ય)vim_FullName(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    પાછા (t1 - ffname == t2 - f2
	     && pathcmp((અક્ષર *)ffname, (અક્ષર *)f2, (પૂર્ણાંક)(t1 - ffname)) == 0);
}

#જો defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    પૂર્ણાંક
vim_chdirfile(char_u *fname, અક્ષર *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    જો (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    જો (pathcmp((અક્ષર *)old_dir, (અક્ષર *)new_dir, -1) == 0)
	// nothing to do
	પાછા OK;

    જો (trigger_autocmd != ખાલી)
	trigger_DirChangedPre((char_u *)trigger_autocmd, new_dir);

    જો (mch_chdir((અક્ષર *)new_dir) != 0)
	પાછા FAIL;

    જો (trigger_autocmd != ખાલી)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    પાછા OK;
}
#endif

#જો defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    સ્થિર પૂર્ણાંક
illegal_slash(અચલ અક્ષર *name)
{
    જો (name[0] == NUL)
	પાછા FALSE;	    // no file name is not illegal
    જો (name[strlen(name) - 1] != '/')
	પાછા FALSE;	    // no trailing slash
    જો (mch_isdir((char_u *)name))
	પાછા FALSE;	    // trailing slash for a directory
    પાછા TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    પૂર્ણાંક
vim_stat(અચલ અક્ષર *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    પાછા illegal_slash(name) ? -1 : stat(name, stp);
}
#endif

#જો defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_table[SHAPE_IDX_ગણતરી] =
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
સ્થિર string_T mshape_names[] =
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
    {ખાલી, 0}
};

#  define MSHAPE_NAMES_ગણતરી  (ARRAY_LENGTH(mshape_names) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    અક્ષર *
parse_shape_opt(પૂર્ણાંક what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    પૂર્ણાંક		idx = 0;		// init for GCC
    પૂર્ણાંક		all_idx;
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;
    long	n;
    પૂર્ણાંક		found_ve = FALSE;	// found "ve" flag
    પૂર્ણાંક		round;

    /*
     * First round: check for errors; second round: do it for real.
     */
    માટે (round = 1; round <= 2; ++round)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	જો (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	નહીંતર
# endif
	    modep = p_guicursor;
	જ્યારે (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    જો (colonp == ખાલી || (commap != ખાલી && commap < colonp))
		પાછા e_missing_colon_2;
	    જો (colonp == modep)
		પાછા e_illegal_mode;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    જ્યારે (modep < colonp || all_idx >= 0)
	    {
		જો (all_idx < 0)
		{
		    // Find the mode.
		    જો (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    નહીંતર
			len = 2;
		    જો (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_ગણતરી - 1;
		    નહીંતર
		    {
			માટે (idx = 0; idx < SHAPE_IDX_ગણતરી; ++idx)
			    જો (STRNICMP(modep, shape_table[idx].name, len)
									 == 0)
				તોડો;
			જો (idx == SHAPE_IDX_ગણતરી
				   || (shape_table[idx].used_for & what) == 0)
			    પાછા e_illegal_mode;
			જો (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    found_ve = TRUE;
		    }
		    modep += len + 1;
		}

		જો (all_idx >= 0)
		    idx = all_idx--;
		નહીંતર જો (round == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    જો (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_table[idx].mshape = 0;
		    }
		    નહીંતર
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
		માટે (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    જો (what == SHAPE_MOUSE)
		    {
			માટે (i = 0; ; ++i)
			{
			    જો (mshape_names[i].string == ખાલી)
			    {
				જો (!VIM_ISDIGIT(*p))
				    પાછા e_illegal_mouseshape;
				જો (round == 2)
				    shape_table[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				નહીંતર
				    (શૂન્ય)getdigits(&p);
				તોડો;
			    }
			    જો (STRNICMP(p, mshape_names[i].string, mshape_names[i].length) == 0)
			    {
				જો (round == 2)
				    shape_table[idx].mshape = i;
				p += mshape_names[i].length;
				તોડો;
			    }
			}
		    }
		    નહીંતર // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			જો (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			નહીંતર જો (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			નહીંતર જો (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			નહીંતર જો (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			નહીંતર જો (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			જો (len != 0)
			{
			    p += len;
			    જો (!VIM_ISDIGIT(*p))
				પાછા e_digit_expected;
			    n = getdigits(&p);
			    જો (len == 3)   // "ver" or "hor"
			    {
				જો (n == 0)
				    પાછા e_illegal_percentage;
				જો (round == 2)
				{
				    જો (TOLOWER_ASC(i) == 'v')
					shape_table[idx].shape = SHAPE_VER;
				    નહીંતર
					shape_table[idx].shape = SHAPE_HOR;
				    shape_table[idx].percentage = n;
				}
			    }
			    નહીંતર જો (round == 2)
			    {
				જો (len == 9)
				    shape_table[idx].blinkwait = n;
				નહીંતર જો (len == 7)
				    shape_table[idx].blinkon = n;
				નહીંતર
				    shape_table[idx].blinkoff = n;
			    }
			}
			નહીંતર જો (STRNICMP(p, "block", 5) == 0)
			{
			    જો (round == 2)
				shape_table[idx].shape = SHAPE_BLOCK;
			    p += 5;
			}
			નહીંતર	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    જો (commap == ખાલી)		    // last part
			    {
				જો (endp == ખાલી)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    નહીંતર જો (endp > commap || endp == ખાલી)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    જો (slashp != ખાલી && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_group(p, (પૂર્ણાંક)(slashp - p));
				p = slashp + 1;
			    }
			    જો (round == 2)
			    {
				shape_table[idx].id = syn_check_group(p,
							     (પૂર્ણાંક)(endp - p));
				shape_table[idx].id_lm = shape_table[idx].id;
				જો (slashp != ખાલી && slashp < endp)
				    shape_table[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    જો (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    જો (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    જો (!found_ve)
    {
# ifdef FEAT_MOUSESHAPE
	જો (what == SHAPE_MOUSE)
	{
	    shape_table[SHAPE_IDX_VE].mshape = shape_table[SHAPE_IDX_V].mshape;
	}
	નહીંતર
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

    પાછા ખાલી;
}

# જો defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    પૂર્ણાંક
get_shape_idx(પૂર્ણાંક mouse)
{
#  ifdef FEAT_MOUSESHAPE
    જો (mouse && (State == MODE_HITRETURN || State == MODE_ASKMORE))
    {
#   ifdef FEAT_GUI
	પૂર્ણાંક x, y;
	gui_mch_getmouse(&x, &y);
	જો (Y_2_ROW(y) == Rows - 1)
	    પાછા SHAPE_IDX_MOREL;
#   endif
	પાછા SHAPE_IDX_MORE;
    }
    જો (mouse && drag_status_લીટી)
	પાછા SHAPE_IDX_SDRAG;
    જો (mouse && drag_sep_લીટી)
	પાછા SHAPE_IDX_VDRAG;
#  endif
    જો (!mouse && State == MODE_SHOWMATCH)
	પાછા SHAPE_IDX_SM;
    જો (State & VREPLACE_FLAG)
	પાછા SHAPE_IDX_R;
    જો (State & REPLACE_FLAG)
	પાછા SHAPE_IDX_R;
    જો (State & MODE_ઉમેરો)
	પાછા SHAPE_IDX_I;
    જો (State & MODE_CMDLINE)
    {
	જો (cmdline_at_end())
	    પાછા SHAPE_IDX_C;
	જો (cmdline_overstrike())
	    પાછા SHAPE_IDX_CR;
	પાછા SHAPE_IDX_CI;
    }
    જો (finish_op)
	પાછા SHAPE_IDX_O;
    જો (VIsual_active)
    {
	જો (*p_sel == 'e')
	    પાછા SHAPE_IDX_VE;
	નહીંતર
	    પાછા SHAPE_IDX_V;
    }
    પાછા SHAPE_IDX_N;
}
# endif

# જો defined(FEAT_MOUSESHAPE)
સ્થિર પૂર્ણાંક current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    શૂન્ય
update_mouseshape(પૂર્ણાંક shape_idx)
{
    પૂર્ણાંક new_mouse_shape;

    // Only works in GUI mode.
    જો (!gui.in_use || gui.starting)
	પાછા;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    જો (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	પાછા;
    }

    // When ignoring the mouse don't change shape on the statusline.
    જો (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_STATUS
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    જો (shape_idx == -2
	    && current_mouse_shape != shape_table[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_STATUS].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_VSEP].mshape)
	પાછા;
    જો (shape_idx < 0)
	new_mouse_shape = shape_table[get_shape_idx(TRUE)].mshape;
    નહીંતર
	new_mouse_shape = shape_table[shape_idx].mshape;
    જો (new_mouse_shape != current_mouse_shape)
    {
	mch_set_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#endif // CURSOR_SHAPE

#જો defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    શૂન્ય
f_getmouseshape(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;
# જો defined(FEAT_MOUSESHAPE)
    જો (current_mouse_shape >= 0
			      && current_mouse_shape < (પૂર્ણાંક)MSHAPE_NAMES_ગણતરી)
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
    પૂર્ણાંક
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    પૂર્ણાંક		r;
    char_u	*file_to_find = ખાલી;
    અક્ષર	*શોધ_ctx = ખાલી;

    dir_name = find_directory_in_path(new_dir, (પૂર્ણાંક)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_to_find, &શોધ_ctx);
    vim_free(file_to_find);
    vim_findfile_cleanup(શોધ_ctx);
    જો (dir_name == ખાલી)
	પાછા -1;
    r = mch_chdir((અક્ષર *)dir_name);
    vim_free(dir_name);
    પાછા r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
get_user_name(char_u *buf, પૂર્ણાંક len)
{
    જો (username == ખાલી)
    {
	જો (mch_get_user_name(buf, len) == FAIL)
	    પાછા FAIL;
	username = vim_strsave(buf);
    }
    નહીંતર
	vim_strncpy(buf, username, len - 1);
    પાછા OK;
}

#જો defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    શૂન્ય
free_username(શૂન્ય)
{
    vim_free(username);
}
#endif

#જો !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    શૂન્ય
qsort(
    શૂન્ય	*base,
    size_t	elm_ગણતરી,
    size_t	elm_size,
    પૂર્ણાંક (*cmp)(અચલ શૂન્ય *, અચલ શૂન્ય *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		gap;

    buf = alloc(elm_size);
    જો (buf == ખાલી)
	પાછા;

    માટે (gap = elm_ગણતરી / 2; gap > 0; gap /= 2)
	માટે (i = gap; i < elm_ગણતરી; ++i)
	    માટે (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)base + j * elm_size;
		p2 = (char_u *)base + (j + gap) * elm_size;
		જો ((*cmp)((શૂન્ય *)p1, (શૂન્ય *)p2) <= 0)
		    તોડો;
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

#જો !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

સ્થિર પૂર્ણાંક  envsize = -1;	// current size of environment
બાહ્ય અક્ષર **environ;		// the global which is your env.

સ્થિર પૂર્ણાંક  findenv(અક્ષર *name); // look for a name in the env.
સ્થિર પૂર્ણાંક  newenv(શૂન્ય);	// copy env. from stack to heap
સ્થિર પૂર્ણાંક  moreenv(શૂન્ય);	// incr. size of env.

    પૂર્ણાંક
putenv(અચલ અક્ષર *string)
{
    પૂર્ણાંક	    i;
    અક્ષર    *p;

    જો (envsize < 0)
    {				// first time putenv called
	જો (newenv() < 0)	// copy env. to heap
	    પાછા -1;
    }

    i = findenv((અક્ષર *)string); // look for name in environment

    જો (i < 0)
    {				// name must be added
	માટે (i = 0; environ[i]; i++);
	જો (i >= (envsize - 1))
	{			// need new slot
	    જો (moreenv() < 0)
		પાછા -1;
	}
	p = alloc(strlen(string) + 1);
	જો (p == ખાલી)		// not enough core
	    પાછા -1;
	environ[i + 1] = 0;	// new end of env.
    }
    નહીંતર
    {				// name already in env.
	p = vim_realloc(environ[i], strlen(string) + 1);
	જો (p == ખાલી)
	    પાછા -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    પાછા 0;
}

    સ્થિર પૂર્ણાંક
findenv(અક્ષર *name)
{
    અક્ષર    *namechar, *envchar;
    પૂર્ણાંક	    i, found;

    found = 0;
    માટે (i = 0; environ[i] && !found; i++)
    {
	envchar = environ[i];
	namechar = name;
	જ્યારે (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	found = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    પાછા found ? i - 1 : -1;
}

    સ્થિર પૂર્ણાંક
newenv(શૂન્ય)
{
    અક્ષર    **env, *elem;
    પૂર્ણાંક	    i, esize;

    માટે (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(અક્ષર *, esize);
    જો (env == ખાલી)
	પાછા -1;

    માટે (i = 0; environ[i]; i++)
    {
	elem = alloc(strlen(environ[i]) + 1);
	જો (elem == ખાલી)
	    પાછા -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
moreenv(શૂન્ય)
{
    પૂર્ણાંક	    esize;
    અક્ષર    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((અક્ષર *)environ, esize * કદ (*env));
    જો (env == 0)
	પાછા -1;
    environ = env;
    envsize = esize;
    પાછા 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(અચલ char_u *string)
{
    પૂર્ણાંક i;
    char_u *p;

    જો (envsize < 0)
	પાછા ખાલી;

    i = findenv((અક્ષર *)string);

    જો (i < 0)
	પાછા ખાલી;

    p = vim_strchr((char_u *)environ[i], '=');
    પાછા (p + 1);
}
# endif

#endif // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#જો defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    પૂર્ણાંક
filewritable(char_u *fname)
{
    પૂર્ણાંક		retval = 0;
# જો defined(UNIX) || defined(VMS)
    પૂર્ણાંક		perm = 0;
# endif

# જો defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    જો (
# ifdef MSWIN
	    mch_writable(fname) &&
# નહીંતર
#  જો defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((અક્ષર *)fname, W_OK) == 0
       )
    {
	++retval;
	જો (mch_isdir(fname))
	    ++retval;
    }
    પાછા retval;
}
#endif

#જો defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    પૂર્ણાંક
get2c(FILE *fd)
{
    પૂર્ણાંક		c, n;

    n = getc(fd);
    જો (n == EOF) પાછા -1;
    c = getc(fd);
    જો (c == EOF) પાછા -1;
    પાછા (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    પૂર્ણાંક
get3c(FILE *fd)
{
    પૂર્ણાંક		c, n;

    n = getc(fd);
    જો (n == EOF) પાછા -1;
    c = getc(fd);
    જો (c == EOF) પાછા -1;
    n = (n << 8) + c;
    c = getc(fd);
    જો (c == EOF) પાછા -1;
    પાછા (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    પૂર્ણાંક
get4c(FILE *fd)
{
    પૂર્ણાંક		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    unsigned	n;

    c = getc(fd);
    જો (c == EOF) પાછા -1;
    n = (unsigned)c;
    c = getc(fd);
    જો (c == EOF) પાછા -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    જો (c == EOF) પાછા -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    જો (c == EOF) પાછા -1;
    n = (n << 8) + (unsigned)c;
    પાછા (પૂર્ણાંક)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, પૂર્ણાંક cnt)
{
    char_u	*str;
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;

    // allocate memory
    str = alloc(cnt + 1);
    જો (str == ખાલી)
	પાછા ખાલી;

    // Read the string.  Quit when running into the EOF.
    માટે (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	જો (c == EOF)
	{
	    vim_free(str);
	    પાછા ખાલી;
	}
	str[i] = c;
    }
    str[i] = NUL;
    પાછા str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    પૂર્ણાંક
put_bytes(FILE *fd, long_u nr, પૂર્ણાંક len)
{
    પૂર્ણાંક	    i;

    માટે (i = len - 1; i >= 0; --i)
	જો (putc((પૂર્ણાંક)(nr >> (i * 8)), fd) == EOF)
	    પાછા FAIL;
    પાછા OK;
}

#endif

#ifndef PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    long
elapsed(સંરચના timeval *start_tv)
{
    સંરચના timeval  now_tv;

    gettimeofday(&now_tv, ખાલી);
    પાછા (now_tv.tv_sec - start_tv->tv_sec) * 1000L
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
    DWORD	now = Get_Tick_ગણતરી();

    પાછા (long)now - (long)start_tick;
}
# endif
#endif

#જો defined(FEAT_JOB_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    પૂર્ણાંક
mch_parse_cmd(char_u *cmd, પૂર્ણાંક use_shcf, અક્ષર ***argv, પૂર્ણાંક *argc)
{
    પૂર્ણાંક		i;
    char_u	*p, *d;
    પૂર્ણાંક		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    માટે (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	જ્યારે (*p != NUL)
	{
	    જો (i == 2)
		(*argv)[*argc] = (અક્ષર *)p;
	    ++*argc;
	    d = p;
	    જ્યારે (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		જો (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		નહીંતર
		{
		    જો (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    જો (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    જો (*p == NUL)
	    {
		જો (i == 2)
		    *d++ = NUL;
		તોડો;
	    }
	    જો (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	જો (*argv == ખાલી)
	{
	    જો (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		માટે (;;)
		{
		    p = skiptowhite(p);
		    જો (*p == NUL)
			તોડો;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(અક્ષર *, *argc + 4);
	    જો (*argv == ખાલી)	    // out of memory
		પાછા FAIL;
	}
    }
    પાછા OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    પૂર્ણાંક
build_argv_from_string(char_u *cmd, અક્ષર ***argv, પૂર્ણાંક *argc)
{
    char_u	*cmd_copy;
    પૂર્ણાંક		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    જો (cmd_copy == ખાલી
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	પાછા FAIL;
    }
    માટે (i = 0; i < *argc; i++)
	(*argv)[i] = (અક્ષર *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = ખાલી;
    vim_free(cmd_copy);
    પાછા OK;
}

#endif

#જો defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    પૂર્ણાંક
build_argv_from_list(list_T *l, અક્ષર ***argv, પૂર્ણાંક *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(અક્ષર *, l->lv_len + 1);
    જો (*argv == ખાલી)
	પાછા FAIL;
    *argc = 0;
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	જો (s == ખાલી)
	{
	    પૂર્ણાંક i;

	    માટે (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = ખાલી;
	    પાછા FAIL;
	}
	(*argv)[*argc] = (અક્ષર *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = ખાલી;
    પાછા OK;
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
    પૂર્ણાંક
get_special_pty_type(શૂન્ય)
{
#ifdef MSWIN
    પાછા get_conpty_type();
#નહીંતર
    પાછા 0;
#endif
}

// compare two keyvalue_T structs by case sensitive value
    પૂર્ણાંક
cmp_keyvalue_કિંમત(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    પાછા STRCMP(kv1->કિંમત.string, kv2->કિંમત.string);
}

// compare two keyvalue_T structs by value with length
    પૂર્ણાંક
cmp_keyvalue_કિંમત_n(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    પાછા STRNCMP(kv1->કિંમત.string, kv2->કિંમત.string, MAX(kv1->કિંમત.length,
		kv2->કિંમત.length));
}

// compare two keyvalue_T structs by case insensitive value
    પૂર્ણાંક
cmp_keyvalue_કિંમત_i(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    પાછા STRICMP(kv1->કિંમત.string, kv2->કિંમત.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    પૂર્ણાંક
cmp_keyvalue_કિંમત_ni(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    પાછા vim_strnicmp_asc((અક્ષર *)kv1->કિંમત.string,
	    (અક્ષર *)kv2->કિંમત.string, MAX(kv1->કિંમત.length,
		    kv2->કિંમત.length));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    શૂન્ય *
mergesort_list(
    શૂન્ય	*head,
    શૂન્ય	*(*get_next)(શૂન્ય *),
    શૂન્ય	(*set_next)(શૂન્ય *, શૂન્ય *),
    શૂન્ય	*(*get_prev)(શૂન્ય *),
    શૂન્ય	(*set_prev)(શૂન્ય *, શૂન્ય *),
    પૂર્ણાંક		(*compare)(અચલ શૂન્ય *, અચલ શૂન્ય *))
{
    જો (!head || !get_next(head))
	પાછા head;

    // Count length
    પૂર્ણાંક	    n = 0;
    શૂન્ય*   curr = head;
    જ્યારે (curr)
    {
	n++;
	curr = get_next(curr);
    }

    પૂર્ણાંક	size;
    માટે (size = 1; size < n; size *= 2)
    {
	શૂન્ય*	new_head = ખાલી;
	શૂન્ય*	tail = ખાલી;
	curr = head;

	જ્યારે (curr)
	{
	    // Split two runs
	    શૂન્ય    *ડાબું = curr;
	    શૂન્ય    *જમણું = ડાબું;
	    પૂર્ણાંક	    i;
	    માટે (i = 0; i < size && જમણું; ++i)
		જમણું = get_next(જમણું);

	    શૂન્ય    *next = જમણું;
	    માટે (i = 0; i < size && next; ++i)
		next = get_next(next);

	    // Break links
	    શૂન્ય    *l_end = જમણું ? get_prev(જમણું) : ખાલી;
	    જો (l_end)
		set_next(l_end, ખાલી);
	    જો (જમણું)
		set_prev(જમણું, ખાલી);

	    શૂન્ય    *r_end = next ? get_prev(next) : ખાલી;
	    જો (r_end)
		set_next(r_end, ખાલી);
	    જો (next)
		set_prev(next, ખાલી);

	    // Merge
	    શૂન્ય    *merged = ખાલી;
	    શૂન્ય    *merged_tail = ખાલી;

	    જ્યારે (ડાબું || જમણું)
	    {
		શૂન્ય	*chosen = ખાલી;
		જો (!ડાબું)
		{
		    chosen = જમણું;
		    જમણું = get_next(જમણું);
		}
		નહીંતર જો (!જમણું)
		{
		    chosen = ડાબું;
		    ડાબું = get_next(ડાબું);
		}
		નહીંતર જો (compare(ડાબું, જમણું) <= 0)
		{
		    chosen = ડાબું;
		    ડાબું = get_next(ડાબું);
		}
		નહીંતર
		{
		    chosen = જમણું;
		    જમણું = get_next(જમણું);
		}

		જો (merged_tail)
		{
		    set_next(merged_tail, chosen);
		    set_prev(chosen, merged_tail);
		    merged_tail = chosen;
		}
		નહીંતર
		{
		    merged = merged_tail = chosen;
		    set_prev(chosen, ખાલી);
		}
	    }

	    // Connect to full list
	    જો (!new_head)
		new_head = merged;
	    નહીંતર
	    {
		set_next(tail, merged);
		set_prev(merged, tail);
	    }

	    // Move tail to end
	    જ્યારે (get_next(merged_tail))
		merged_tail = get_next(merged_tail);
	    tail = merged_tail;

	    curr = next;
	}

	head = new_head;
    }

    પાછા head;
}

સ્થિર અચલ char_u base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

સ્થિર char_u base64_dec_table[256];

#define BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    સ્થિર શૂન્ય
init_base64_dec_table(શૂન્ય)
{
    સ્થિર પૂર્ણાંક base64_dec_tbl_initialized = FALSE;

    જો (base64_dec_tbl_initialized)
	પાછા;

    vim_memset(base64_dec_table, 0xFF, કદ(base64_dec_table));
    માટે (size_t i = 0; i < કદ(base64_table) - 1; i++)
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
base64_encode_buf(char_u *dst, અચલ char_u *src, size_t len)
{
    size_t i, j;

    માટે (i = 0, j = 0; i < len;)
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
    પાછા (long)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(અચલ char_u *data, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    જો (encoded == ખાલી)
	પાછા ખાલી;
    base64_encode_buf(encoded, data, len);
    પાછા encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    પૂર્ણાંક
base64_decode(અચલ char_u *data, size_t len, garray_T *out)
{
    જો (len == 0)
	પાછા OK;

    જો (len % 4 != 0)
    {
	semsg(_(e_invalid_argument_str), data);
	પાછા FAIL;
    }

    init_base64_dec_table();

    size_t decoded_len = (len / 4) * 3;
    જો (data[len - 1] == '=')
	decoded_len--;
    જો (data[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    માટે (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_table[(char_u)data[i++]];
	int_u sextet_b = base64_dec_table[(char_u)data[i++]];
	int_u sextet_c = base64_dec_table[(char_u)data[i++]];
	int_u sextet_d = base64_dec_table[(char_u)data[i++]];

	જો (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_argument_str), data);
	    ga_clear(out);
	    પાછા FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	જો (j < decoded_len)
	{
	    ga_append(out, (triple >> 16) & 0xFF);
	    j++;
	}
	જો (j < decoded_len)
	{
	    ga_append(out, (triple >> 8) & 0xFF);
	    j++;
	}
	જો (j < decoded_len)
	{
	    ga_append(out, triple & 0xFF);
	    j++;
	}

	જો (j == decoded_len)
	{
	    જો ((data[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((data[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_argument_str), data);
		ga_clear(out);
		પાછા FAIL;
	    }
	}
    }
    પાછા OK;
}
