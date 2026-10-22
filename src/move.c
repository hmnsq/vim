/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * move.c: Functions for moving the cursor and scrolling text.
 *
 * There are two ways to move the cursor:
 * 1. Move the cursor directly, the text is scrolled to keep the cursor in the
 *    window.
 * 2. Scroll the text, the cursor is moved into the text visible in the
 *    window.
 * The 'scrolloff' option makes this a bit complicated.
 */

#شامل "vim.h"

ساکن خالی redraw_for_cursorline(win_T *wp);
ساکن صحیح scrolljump_قدر(خالی);
ساکن صحیح check_چوٹی_انحراف(خالی);
ساکن خالی curs_صفیں(win_T *wp);

قسمتعریف ساخت
{
    linenr_T	    lnum;	// line number
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		    fill;	// filler lines
#اگر_ختم
    صحیح		    اونچائی;	// height of added line
} lineoff_T;

ساکن خالی topline_عقب(lineoff_T *lp);
ساکن خالی botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    ساکن صحیح
adjust_plines_for_skipcol(win_T *wp)
{
    اگر (wp->w_skipcol == 0)
	واپس 0;

    صحیح چوڑائی = wp->w_چوڑائی - win_ستون_off(wp);
    صحیح w2 = چوڑائی + win_ستون_off2(wp);
    اگر (wp->w_skipcol >= چوڑائی && w2 > 0)
	واپس (wp->w_skipcol - چوڑائی) / w2 + 1;

    واپس 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    صحیح
plines_correct_topline(win_T *wp, linenr_T lnum, صحیح حد_بندی_winheight)
{
    صحیح n;
#اگر_تعریف_شدہ FEAT_DIFF
    اگر (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    ورنہ
#اگر_ختم
	n = plines_win(wp, lnum, FALSE);
    اگر (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    اگر (حد_بندی_winheight && n > wp->w_اونچائی)
	n = wp->w_اونچائی;
    واپس n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    ساکن خالی
comp_botline(win_T *wp)
{
    صحیح		n;
    linenr_T	lnum;
    صحیح		مکمل;
    صحیح		i = 0;
    صحیح		use_cache;
#اگر_تعریف_شدہ FEAT_FOLDING
    linenr_T    آخری;
    صحیح		folded;
#اگر_ختم

    /*
     * If w_cline_row is valid, start there.
     * Otherwise have to start at w_topline.
     */
    check_cursor_moved(wp);

    // The wl_size values computed for the previous redraw give the height of
    // each displayed line.  When the display is up-to-date they are equal to
    // what plines_correct_topline() would compute, so reuse them to avoid
    // walking every line to measure its width on each scroll (like curs_rows()
    // does).  Only trust them when actually redrawing, the buffer was not
    // changed, no "$" is displayed for a change and w_lines[] starts at or
    // above w_topline.  With 'smoothscroll' or in diff mode wl_size includes
    // the skipped rows of the top line and the filler lines, so it does not
    // match plines_correct_topline(); do not use the cache then.
    use_cache = redrawing()
		    && !wp->w_بفر->b_mod_سیٹ
		    && dollar_vcol == -1
		    && wp->w_skipcol == 0
#اگر_تعریف_شدہ FEAT_DIFF
		    && !wp->w_p_diff
#اگر_ختم
		    && wp->w_سطریں_valid > 0
		    && wp->w_سطریں[0].wl_lnum <= wp->w_topline;

    اگر (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	مکمل = wp->w_cline_صف;
    }
    ورنہ
    {
	lnum = wp->w_topline;
	مکمل = 0;
    }

    // Find the w_lines[] entry for the starting line.
    اگر (use_cache)
	جب (i < wp->w_سطریں_valid && wp->w_سطریں[i].wl_lnum < lnum)
	    ++i;

    برائے ( ; lnum <= wp->w_بفر->b_ml.ml_سطر_گنتی; ++i)
    {
	صحیح	valid = FALSE;

#اگر_تعریف_شدہ FEAT_FOLDING
	آخری = lnum;
	folded = FALSE;
#اگر_ختم
	// Try to use the size from the previous redraw.
	اگر (use_cache && i < wp->w_سطریں_valid)
	{
	    اگر (wp->w_سطریں[i].wl_lnum < lnum || !wp->w_سطریں[i].wl_valid)
		جاری;		// skip changed or deleted lines
	    اگر (wp->w_سطریں[i].wl_lnum == lnum)
		valid = TRUE;
	    ورنہ // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	اگر (valid)
	{
	    n = wp->w_سطریں[i].wl_سائز;
#اگر_تعریف_شدہ FEAT_FOLDING
	    folded = wp->w_سطریں[i].wl_folded;
	    آخری = wp->w_سطریں[i].wl_lastlnum;
#اگر_ختم
	}
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (hasFoldingWin(wp, lnum, تہی, &آخری, TRUE, تہی))
	    {
		n = 1;
		folded = TRUE;
	    }
	    ورنہ
#اگر_ختم
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	اگر (
#اگر_تعریف_شدہ FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && آخری >= wp->w_cursor.lnum
#ورنہ
		lnum == wp->w_cursor.lnum
#اگر_ختم
	   )
	{
	    wp->w_cline_صف = مکمل;
	    wp->w_cline_اونچائی = n;
#اگر_تعریف_شدہ FEAT_FOLDING
	    wp->w_cline_folded = folded;
#اگر_ختم
	    redraw_for_cursorline(wp);
	    wp->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	اگر (مکمل + n > wp->w_اونچائی)
	    توڑو;
	مکمل += n;
#اگر_تعریف_شدہ FEAT_FOLDING
	lnum = آخری + 1;
#ورنہ
	++lnum;
#اگر_ختم
    }

    // wp->w_botline is the line that is just below the window
    wp->w_botline = lnum;
    wp->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    سیٹ_خالی_خانے_صفیں(wp, مکمل);
}

/*
 * Redraw when w_cline_row changes and 'relativenumber' or 'cursorline' is
 * set.
 */
    ساکن خالی
redraw_for_cursorline(win_T *wp)
{
    اگر ((wp->w_p_rnu
#اگر_تعریف_شدہ FEAT_SYN_HL
		|| wp->w_p_cul
#اگر_ختم
		)
	    && (wp->w_valid & VALID_CROW) == 0
	    && !pum_visible())
    {
	// win_line() will redraw the number column and cursorline only.
	redraw_win_later(wp, UPD_VALID);
    }
}

#اگر_تعریف_شدہ FEAT_SYN_HL
/*
 * Redraw when w_virtcol changes and 'cursorcolumn' is set or 'cursorlineopt'
 * contains "screenline".
 */
    ساکن خالی
redraw_for_cursorcolumn(win_T *wp)
{
    اگر ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	اگر (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	ورنہ اگر (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#اگر_ختم

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    خالی
سیٹ_valid_virtcol(win_T *wp, colnr_T vcol)
{
    wp->w_virtcol = vcol;
#اگر_تعریف_شدہ FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#اگر_ختم
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Calculates how much the 'listchars' "precedes" or 'smoothscroll' "<<<"
 * marker overlaps with buffer text for window "wp".
 * Parameter "extra2" should be the padding on the 2nd line, not the first
 * line. When "extra2" is -1 calculate the padding.
 * Returns the number of columns of overlap with buffer text, excluding the
 * extra padding on the ledge.
 */
    صحیح
sms_marker_overlap(win_T *wp, صحیح extra2)
{
    اگر (extra2 == -1)
	extra2 = win_ستون_off(wp) - win_ستون_off2(wp);
#اگر defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    اگر (*get_showbreak_قدر(wp) != NUL)
	واپس 0;
#اگر_ختم
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    اگر (wp->w_p_list && wp->w_lcs_chars.prec)
	واپس 1;

    واپس extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    ساکن صحیح
skipcol_from_plines(win_T *wp, صحیح plines_off)
{
    صحیح width1 = wp->w_چوڑائی - win_ستون_off(wp);

    صحیح skipcol = 0;
    اگر (plines_off > 0)
	skipcol += width1;
    اگر (plines_off > 1)
	skipcol += (width1 + win_ستون_off2(wp)) * (plines_off - 1);
    واپس skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    ساکن خالی
reset_skipcol(خالی)
{
    اگر (curwin->w_skipcol == 0)
	واپس;

    curwin->w_skipcol = 0;

    // Should use the least expensive way that displays all that changed.
    // UPD_NOT_VALID is too expensive, UPD_REDRAW_TOP does not redraw
    // enough when the top line gets another screen line.
    redraw_later(UPD_SOME_VALID);
}

/*
 * Update curwin->w_topline and redraw if necessary.
 * Used to update the screen before printing a message.
 */
    خالی
update_topline_redraw(خالی)
{
    update_topline();
    اگر (must_redraw)
	update_screen(0);
}

/*
 * Return true when 'scrolloffpad' may augment 'scrolloff'.
 * This only applies to automatic cursor visibility correction.
 * For now 'scrolloffpad' is treated as boolean: 0 disables, > 0 enables.
 */
    ساکن bool
use_scrolloffpad(خالی)
{
    واپس get_scrolloff_قدر() > 0 && get_scrolloffpad_قدر() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    ساکن bool
scrolloffpad_eof_pressure(linenr_T lnum, طویل so)
{
    اگر (!use_scrolloffpad() || so <= 0)
	واپس false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    واپس lnum > curbuf->b_ml.ml_سطر_گنتی - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    خالی
update_topline(خالی)
{
    طویل	سطر_گنتی;
    صحیح		halfheight;
    صحیح		n;
#اگر_تعریف_شدہ FEAT_FOLDING
    linenr_T	lnum;
#اگر_ختم
    صحیح		check_topline = FALSE;
    صحیح		check_botline = FALSE;
    طویل	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    صحیح		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    اگر (skip_update_topline)
	واپس;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    اگر (!screen_valid(TRUE) || curwin->w_اونچائی == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	واپس;
    }

    check_cursor_moved(curwin);
    اگر (curwin->w_valid & VALID_TOPLINE)
	واپس;

    // When dragging with the mouse, don't scroll that quickly
    اگر (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح old_topfill = curwin->w_topfill;
#اگر_ختم

    /*
     * If the buffer is empty, always set topline to 1.
     */
    اگر (BUFEMPTY())		// special case - file is empty
    {
	اگر (curwin->w_topline != 1)
	    redraw_later(UPD_NOT_VALID);
	curwin->w_topline = 1;
	curwin->w_botline = 2;
	curwin->w_skipcol = 0;
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	curwin->w_scbind_pos = 1;
    }

    /*
     * If the cursor is above or near the top of the window, scroll the window
     * to show the line the cursor is in, with 'scrolloff' context.
     */
    ورنہ
    {
	اگر (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    اگر (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    ورنہ اگر (check_چوٹی_انحراف())
		check_topline = TRUE;
	    ورنہ اگر (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		صحیح overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, تہی, تہی, 0);
		overlap = sms_marker_overlap(curwin, -1);
		اگر (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#اگر_تعریف_شدہ FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	اگر (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#اگر_ختم

	اگر (check_topline)
	{
	    halfheight = curwin->w_اونچائی / 2 - 1;
	    اگر (halfheight < 2)
		halfheight = 2;

#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		برائے (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    اگر (lnum >= curbuf->b_ml.ml_سطر_گنتی || n >= halfheight)
			توڑو;
		    (خالی)hasFolding(lnum, تہی, &lnum);
		}
	    }
	    ورنہ
#اگر_ختم
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    صحیح min_scroll = scrolljump_قدر();
	    اگر (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    ورنہ اگر (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    ورنہ
	    {
		scroll_cursor_چوٹی(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	ورنہ
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (خالی)hasFolding(curwin->w_topline, &curwin->w_topline, تہی);
#اگر_ختم
	    check_botline = TRUE;
	}
    }

    /*
     * If the cursor is below the bottom of the window, scroll the window
     * to put the cursor on the window.
     * When w_botline is invalid, recompute it first, to avoid a redraw later.
     * If w_botline was approximated, we might need a redraw later in a few
     * cases, but we don't want to spend (a lot of) time recomputing w_botline
     * for every small change.
     */
    اگر (check_botline)
    {
	اگر (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	اگر (curwin->w_botline <= curbuf->b_ml.ml_سطر_گنتی || use_scrolloffpad())
	{
	    اگر (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		اگر (((طویل)curwin->w_cursor.lnum
					   >= (طویل)curwin->w_botline - *so_ptr
#اگر_تعریف_شدہ FEAT_FOLDING
			|| hasAnyFolding(curwin)
#اگر_ختم
			))
		{
		    lineoff_T	loff;

		    // Cursor is (a few lines) above botline, check if there
		    // are 'scrolloff' window lines below the cursor.
		    // If not, need to scroll.
		    n = eof_pressure ? 0 : curwin->w_خالی_خانے_صفیں;
		    loff.lnum = curwin->w_cursor.lnum;
#اگر_تعریف_شدہ FEAT_FOLDING
		    // In a fold go to its last line.
		    (خالی)hasFolding(loff.lnum, تہی, &loff.lnum);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_صفیں;
#اگر_ختم
		    loff.اونچائی = 0;
		    جب (loff.lnum < curwin->w_botline
#اگر_تعریف_شدہ FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#اگر_ختم
			    )
		    {
			n += loff.اونچائی;
			اگر (n >= *so_ptr)
			    توڑو;
			botline_forw(&loff);
		    }
		    اگر (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		ورنہ
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    اگر (check_botline)
	    {
#اگر_تعریف_شدہ FEAT_FOLDING
		اگر (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    سطر_گنتی = 0;
		    برائے (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++سطر_گنتی;
			// stop at end of file or when we know we are far off
			اگر (lnum <= 0 || سطر_گنتی > curwin->w_اونچائی + 1)
			    توڑو;
			(خالی)hasFolding(lnum, &lnum, تہی);
		    }
		}
		ورنہ
#اگر_ختم
		    سطر_گنتی = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		اگر (سطر_گنتی <= curwin->w_اونچائی + 1)
		{
		    اگر (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    ورنہ
			scroll_cursor_bot(scrolljump_قدر(), FALSE);
		}
		ورنہ
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    اگر (curwin->w_topline != old_topline
#اگر_تعریف_شدہ FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#اگر_ختم
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	اگر (!curwin->w_p_sms)
	    reset_skipcol();
	ورنہ اگر (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	اگر (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    ساکن صحیح
scrolljump_قدر(خالی)
{
    اگر (p_sj >= 0)
	واپس (صحیح)p_sj;
    واپس (curwin->w_اونچائی * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    ساکن صحیح
check_چوٹی_انحراف(خالی)
{
    lineoff_T	loff;
    صحیح		n;
    طویل	so = get_scrolloff_قدر();

    اگر (curwin->w_cursor.lnum < curwin->w_topline + so
#اگر_تعریف_شدہ FEAT_FOLDING
		    || hasAnyFolding(curwin)
#اگر_ختم
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#اگر_تعریف_شدہ FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#ورنہ
	n = 0;
#اگر_ختم
	// Count the visible screen lines above the cursor line.
	جب (n < so)
	{
	    topline_عقب(&loff);
	    // Stop when included a line above the window.
	    اگر (loff.lnum < curwin->w_topline
#اگر_تعریف_شدہ FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#اگر_ختم
		    )
		توڑو;
	    n += loff.اونچائی;
	}
	اگر (n < so)
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Update w_curswant.
 */
    خالی
update_curswant_force(خالی)
{
    validate_virtcol();
    curwin->w_curswant = curwin->w_virtcol
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	- curwin->w_virtcol_پہلا_char
#اگر_ختم
	;
    curwin->w_سیٹ_curswant = false;
}

/*
 * Update w_curswant if w_set_curswant is set.
 */
    خالی
update_curswant(خالی)
{
    اگر (curwin->w_سیٹ_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    خالی
check_cursor_moved(win_T *wp)
{
    اگر (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ورنہ اگر (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ورنہ اگر (wp->w_cursor.ستون != wp->w_valid_cursor.ستون
	     || wp->w_leftcol != wp->w_valid_leftcol
	     || wp->w_cursor.coladd != wp->w_valid_cursor.coladd)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
	wp->w_valid_cursor.ستون = wp->w_cursor.ستون;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_cursor.coladd = wp->w_cursor.coladd;
    }
}

/*
 * Call this function when some window settings have changed, which require
 * the cursor position, botline and topline to be recomputed and the window to
 * be redrawn.  E.g, when changing the 'wrap' option or folding.
 */
    خالی
بدلا_کھڑکی_setting(خالی)
{
    بدلا_کھڑکی_setting_win(curwin);
}

    خالی
بدلا_کھڑکی_setting_win(win_T *wp)
{
    wp->w_سطریں_valid = 0;
    بدلا_سطر_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    خالی
بدلا_کھڑکی_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_بفر == buf)
	    بدلا_کھڑکی_setting_win(wp);
}
#اگر_ختم

/*
 * Call changed_window_setting_win() for every window.
 */
    خالی
بدلا_کھڑکی_setting_all(خالی)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	بدلا_کھڑکی_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    خالی
سیٹ_topline(win_T *wp, linenr_T lnum)
{
#اگر_تعریف_شدہ FEAT_DIFF
    linenr_T پچھلا_topline = wp->w_topline;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_FOLDING
    // go to first of folded lines
    (خالی)hasFoldingWin(wp, lnum, &lnum, تہی, TRUE, تہی);
#اگر_ختم
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    اگر (wp->w_botline > wp->w_بفر->b_ml.ml_سطر_گنتی + 1)
	wp->w_botline = wp->w_بفر->b_ml.ml_سطر_گنتی + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_سیٹ = true;
#اگر_تعریف_شدہ FEAT_DIFF
    اگر (lnum != پچھلا_topline)
	// Keep the filler lines when the topline didn't change.
	wp->w_topfill = 0;
#اگر_ختم
    wp->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_TOPLINE);
    // Don't set VALID_TOPLINE here, 'scrolloff' needs to be checked.
    redraw_later(UPD_VALID);
}

/*
 * Call this function when the length of the cursor line (in screen
 * characters) has changed, and the change is before the cursor.
 * If the line length changed the number of screen lines might change,
 * requiring updating w_topline.  That may also invalidate w_crow.
 * Need to take care of w_botline separately!
 */
    خالی
بدلا_cline_bef_curs(خالی)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    خالی
بدلا_cline_bef_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

/*
 * Call this function when the length of a line (in screen characters) above
 * the cursor have changed.
 * Need to take care of w_botline separately!
 */
    خالی
بدلا_سطر_abv_curs(خالی)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    خالی
بدلا_سطر_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    خالی
بدلا_سطر_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_بفر == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#اگر_ختم

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    خالی
validate_botline(خالی)
{
    validate_botline_win(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    خالی
validate_botline_win(win_T *wp)
{
    اگر (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    خالی
invalidate_botline(خالی)
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    خالی
invalidate_botline_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    خالی
approximate_botline_win(
    win_T	*wp)
{
    wp->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    صحیح
cursor_valid(خالی)
{
    check_cursor_moved(curwin);
    واپس ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    خالی
validate_cursor(خالی)
{
    check_cursor_lnum();
    check_cursor_moved(curwin);
    اگر ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#اگر defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    خالی
validate_cline_صف(خالی)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    اگر (!(curwin->w_valid & VALID_CROW))
	curs_صفیں(curwin);
}
#اگر_ختم

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    ساکن خالی
curs_صفیں(win_T *wp)
{
    linenr_T	lnum;
    صحیح		i;
    صحیح		all_invalid;
    صحیح		valid;
#اگر_تعریف_شدہ FEAT_FOLDING
    طویل	fold_گنتی;
#اگر_ختم

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_سطریں_valid == 0
			|| wp->w_سطریں[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_صف = 0;
    برائے (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	اگر (!all_invalid && i < wp->w_سطریں_valid)
	{
	    اگر (wp->w_سطریں[i].wl_lnum < lnum || !wp->w_سطریں[i].wl_valid)
		جاری;		// skip changed or deleted lines
	    اگر (wp->w_سطریں[i].wl_lnum == lnum)
	    {
#اگر_تعریف_شدہ FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		اگر (!wp->w_بفر->b_mod_سیٹ
			|| wp->w_سطریں[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_بفر->b_mod_چوٹی
					     > wp->w_سطریں[i].wl_lastlnum + 1)
#اگر_ختم
		valid = TRUE;
	    }
	    ورنہ اگر (wp->w_سطریں[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	اگر (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#اگر_تعریف_شدہ FEAT_DIFF
					    && !wp->w_p_diff
#اگر_ختم
	       )))
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    lnum = wp->w_سطریں[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    اگر (lnum > wp->w_cursor.lnum)
		توڑو;
#ورنہ
	    ++lnum;
#اگر_ختم
	    wp->w_cline_صف += wp->w_سطریں[i].wl_سائز;
	}
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    fold_گنتی = folded_گنتی(wp, lnum, تہی);
	    اگر (fold_گنتی)
	    {
		lnum += fold_گنتی;
		اگر (lnum > wp->w_cursor.lnum)
		    توڑو;
		++wp->w_cline_صف;
	    }
	    ورنہ
#اگر_ختم
	    {
		wp->w_cline_صف += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    اگر (!(wp->w_valid & VALID_CHEIGHT))
    {
	اگر (all_invalid
		|| i == wp->w_سطریں_valid
		|| (i < wp->w_سطریں_valid
		    && (!wp->w_سطریں[i].wl_valid
			|| wp->w_سطریں[i].wl_lnum != wp->w_cursor.lnum)))
	{
#اگر_تعریف_شدہ FEAT_DIFF
	    اگر (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_اونچائی = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    ورنہ
#اگر_ختم
		wp->w_cline_اونچائی = plines_win(wp, wp->w_cursor.lnum, TRUE);
#اگر_تعریف_شدہ FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      تہی, تہی, TRUE, تہی);
#اگر_ختم
	}
	ورنہ اگر (i > wp->w_سطریں_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_اونچائی = 0;
#اگر_تعریف_شدہ FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      تہی, تہی, TRUE, تہی);
#اگر_ختم
	}
	ورنہ
	{
	    wp->w_cline_اونچائی = wp->w_سطریں[i].wl_سائز;
#اگر_تعریف_شدہ FEAT_FOLDING
	    wp->w_cline_folded = wp->w_سطریں[i].wl_folded;
#اگر_ختم
	}
    }

    redraw_for_cursorline(curwin);
    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;
}

/*
 * Validate curwin->w_virtcol only.
 */
    خالی
validate_virtcol(خالی)
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    خالی
validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);

    اگر (wp->w_valid & VALID_VIRTCOL)
	واپس;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    wp->w_virtcol_پہلا_char = 0;
#اگر_ختم
    getvvcol(wp, &wp->w_cursor, تہی, &(wp->w_virtcol), تہی,
							 GETVCOL_FOR_VIRTCOL);
#اگر_تعریف_شدہ FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#اگر_ختم
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    خالی
validate_cheight(خالی)
{
    check_cursor_moved(curwin);

    اگر (curwin->w_valid & VALID_CHEIGHT)
	واپس;

#اگر_تعریف_شدہ FEAT_DIFF
    اگر (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_اونچائی = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    ورنہ
#اگر_ختم
	curwin->w_cline_اونچائی = plines(curwin->w_cursor.lnum);
#اگر_تعریف_شدہ FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, تہی, تہی);
#اگر_ختم
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    خالی
validate_cursor_ستون(خالی)
{
    colnr_T off;
    colnr_T ستون;
    صحیح     چوڑائی;

    validate_virtcol();

    اگر (curwin->w_valid & VALID_WCOL)
	واپس;

    ستون = curwin->w_virtcol;
    off = curwin_ستون_off();
    ستون += off;
    چوڑائی = curwin->w_چوڑائی - off + curwin_ستون_off2();

    // long line wrapping, adjust curwin->w_wrow
    اگر (curwin->w_p_wrap
	    && ستون >= (colnr_T)curwin->w_چوڑائی
	    && چوڑائی > 0)
	// use same formula as what is used in curs_columns()
	ستون -= ((ستون - curwin->w_چوڑائی) / چوڑائی + 1) * چوڑائی;
    اگر (ستون > (صحیح)curwin->w_leftcol)
	ستون -= curwin->w_leftcol;
    ورنہ
	ستون = 0;
    curwin->w_wcol = ستون;

    curwin->w_valid |= VALID_WCOL;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    curwin->w_flags &= ~WFLAG_WCOL_OFF_ADDED;
#اگر_ختم
}

/*
 * Compute offset of a window, occupied by absolute or relative line number,
 * fold column and sign column (these don't move when scrolling horizontally).
 */
    صحیح
win_ستون_off(win_T *wp)
{
    واپس (((wp->w_p_nu || wp->w_p_rnu) ? نمبر_چوڑائی(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#اگر_تعریف_شدہ FEAT_FOLDING
	    + wp->w_p_fdc
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#اگر_ختم
	   );
}

    صحیح
curwin_ستون_off(خالی)
{
    واپس win_ستون_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    صحیح
win_ستون_off2(win_T *wp)
{
    اگر ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != تہی)
	واپس نمبر_چوڑائی(wp) + 1;
    واپس 0;
}

    صحیح
curwin_ستون_off2(خالی)
{
    واپس win_ستون_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    خالی
curs_columns(
    صحیح		may_scroll)	// when TRUE, may scroll horizontally
{
    طویل	diff;
    صحیح		extra;		// offset for first screen line
    طویل	off_بایاں, off_دایاں;
    صحیح		n;
    صحیح		p_سطریں;
    صحیح		width1;		// text width for first screen line
    صحیح		width2 = 0;	// text width for second and later screen line
    صحیح		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	پچھلا_skipcol;
    طویل	so = get_scrolloff_قدر();
    طویل	siso = get_sidescrolloff_قدر();
    صحیح		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    اگر (!(curwin->w_valid & VALID_CROW))
	curs_صفیں(curwin);

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_پہلا_char = 0;
#اگر_ختم

    /*
     * Compute the number of virtual columns.
     */
#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    ورنہ
#اگر_ختم
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    اگر (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_ستون_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_صف;

    width1 = curwin->w_چوڑائی - extra;
    اگر (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_چوڑائی - 1;
	اگر (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_اونچائی - 1;
	ورنہ
	    curwin->w_wrow = curwin->w_اونچائی - 1 - curwin->w_خالی_خانے_صفیں;
    }
    ورنہ اگر (curwin->w_p_wrap && curwin->w_چوڑائی != 0)
    {
	width2 = width1 + curwin_ستون_off2();

	// skip columns that are not visible
	اگر (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    اگر (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    ورنہ
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	اگر (curwin->w_wcol >= curwin->w_چوڑائی)
	{
	    // this same formula is used in validate_cursor_col()
	    n = (curwin->w_wcol - curwin->w_چوڑائی) / width2 + 1;
	    curwin->w_wcol -= n * width2;
	    curwin->w_wrow += n;
	}
    }

    // No line wrapping: compute curwin->w_leftcol if scrolling is on and line
    // is not folded.
    // If scrolling is off, curwin->w_leftcol is assumed to be 0
    ورنہ اگر (may_scroll
#اگر_تعریف_شدہ FEAT_FOLDING
	    && !curwin->w_cline_folded
#اگر_ختم
	    )
    {
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (curwin->w_virtcol_پہلا_char > 0)
	{
	    صحیح ستون_تعداد = (curwin->w_چوڑائی - extra);
	    صحیح صفیں_2 = ستون_تعداد > 0 ? curwin->w_virtcol_پہلا_char / ستون_تعداد : 1;

	    // each "above" text prop shifts the text one row down
	    curwin->w_wrow += صفیں_2;
	    curwin->w_wcol -= صفیں_2 * ستون_تعداد;
	    endcol -= صفیں_2 * ستون_تعداد;
	    curwin->w_cline_اونچائی = صفیں_2 + 1;
	}
#اگر_ختم
	/*
	 * If Cursor is left of the screen, scroll rightwards.
	 * If Cursor is right of the screen, scroll leftwards.
	 * If we get closer to the edge than 'sidescrolloff', scroll a little
	 * extra.
	 */
	off_بایاں = startcol - curwin->w_leftcol - siso;
	off_دایاں = endcol - curwin->w_leftcol - (curwin->w_چوڑائی - siso) + 1;
	اگر (off_بایاں < 0 || off_دایاں > 0)
	{
	    اگر (off_بایاں < 0)
		diff = -off_بایاں;
	    ورنہ
		diff = off_دایاں;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    اگر (p_ss == 0 || diff >= width1 / 2 || off_دایاں >= off_بایاں)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    ورنہ
	    {
		اگر (diff < p_ss)
		    diff = p_ss;
		اگر (off_بایاں < 0)
		    new_leftcol = curwin->w_leftcol - (صحیح)diff;
		ورنہ
		    new_leftcol = curwin->w_leftcol + (صحیح)diff;
	    }
	    اگر (new_leftcol < 0)
		new_leftcol = 0;
	    اگر (new_leftcol != (صحیح)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    ورنہ اگر (curwin->w_wcol > (صحیح)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    ورنہ
	curwin->w_wcol = 0;

#اگر_تعریف_شدہ FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    اگر (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    ورنہ
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#اگر_ختم

    پچھلا_skipcol = curwin->w_skipcol;

    p_سطریں = 0;

    اگر ((curwin->w_wrow >= curwin->w_اونچائی
		|| ((پچھلا_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_اونچائی)
		    && (p_سطریں =
#اگر_تعریف_شدہ FEAT_DIFF
			plines_win_nofill
#ورنہ
			plines_win
#اگر_ختم
			(curwin, curwin->w_cursor.lnum, FALSE))
						    - 1 >= curwin->w_اونچائی))
	    && curwin->w_اونچائی != 0
	    && curwin->w_cursor.lnum == curwin->w_topline
	    && width2 > 0
	    && curwin->w_چوڑائی != 0)
    {
	// Cursor past end of screen.  Happens with a single line that does
	// not fit on screen.  Find a skipcol to show the text around the
	// cursor.  Avoid scrolling all the time. compute value of "extra":
	// 1: Less than 'scrolloff' lines above
	// 2: Less than 'scrolloff' lines below
	// 3: both of them
	extra = 0;
	اگر (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	اگر (p_سطریں == 0)
	    p_سطریں = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_سطریں;
	اگر (p_سطریں > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	ورنہ
	    n = p_سطریں;
	اگر ((colnr_T)n >= curwin->w_اونچائی + curwin->w_skipcol / width2 - so)
	    extra += 2;

	اگر (extra == 3 || curwin->w_اونچائی <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    اگر (n > curwin->w_اونچائی / 2)
		n -= curwin->w_اونچائی / 2;
	    ورنہ
		n = 0;
	    // don't skip more than necessary
	    اگر (n > p_سطریں - curwin->w_اونچائی + 1)
		n = p_سطریں - curwin->w_اونچائی + 1;
	    اگر (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    ورنہ
		curwin->w_skipcol = 0;
	}
	ورنہ اگر (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    اگر (extra > 0)
	    {
		اگر ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	ورنہ اگر (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_اونچائی + 1) * width2;
	    جب (endcol > curwin->w_virtcol)
		endcol -= width2;
	    اگر (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	اگر (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - پچھلا_skipcol) / width2;
	ورنہ
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	اگر (curwin->w_wrow >= curwin->w_اونچائی)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_اونچائی + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((صحیح)پچھلا_skipcol - (صحیح)curwin->w_skipcol) / width2;
	اگر (extra > 0)
	    win_ins_سطریں(curwin, 0, extra, FALSE, FALSE);
	ورنہ اگر (extra < 0)
	    win_del_سطریں(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    ورنہ اگر (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    اگر (پچھلا_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#اگر_تعریف_شدہ FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#اگر_ختم
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    اگر (popup_is_popup(curwin) && curbuf->b_رکن != تہی)
    {
	curwin->w_wrow += popup_چوٹی_extra(curwin);
	curwin->w_wcol += popup_بایاں_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    ورنہ
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#اگر_ختم

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#اگر defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    خالی
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	صحیح	*rowp,	// screen row
	صحیح	*scolp,	// start screen column
	صحیح	*ccolp,	// cursor screen column
	صحیح	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    صحیح		صف = 0;
    colnr_T	coloff = 0;

    اگر (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    ستون;
	صحیح	    چوڑائی;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	صحیح	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, تہی, TRUE, تہی);
# endif
	صف = plines_m_win(wp, wp->w_topline, lnum - 1, زیادہ_سے_زیادہ_عدد);
	// "row" should be the screen line where line "lnum" begins, which can
	// be negative if "lnum" is "w_topline" and "w_skipcol" is non-zero.
	صف -= adjust_plines_for_skipcol(wp);

# ifdef FEAT_DIFF
	// Add filler lines above this buffer line.
	صف += lnum == wp->w_topline ? wp->w_topfill
				     : diff_check_fill(wp, lnum);
# endif

	colnr_T	off = win_ستون_off(wp);
# ifdef FEAT_FOLDING
	اگر (is_folded)
	{
	    صف += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	ورنہ
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    ستون = scol;
	    ستون += off;
	    چوڑائی = wp->w_چوڑائی - off + win_ستون_off2(wp);

	    // long line wrapping, adjust row
	    اگر (wp->w_p_wrap
		    && ستون >= (colnr_T)wp->w_چوڑائی
		    && چوڑائی > 0)
	    {
		// use same formula as what is used in curs_columns()
		صحیح rowoff = ((ستون - wp->w_چوڑائی) / چوڑائی + 1);
		ستون -= rowoff * چوڑائی;
		صف += rowoff;
	    }
	    ستون -= wp->w_leftcol;
	    اگر (ستون >= wp->w_چوڑائی)
		ستون = -1;
	    اگر (ستون >= 0 && صف >= 0 && صف < wp->w_اونچائی)
	    {
		coloff = ستون - scol + wp->w_wincol + 1;
		صف += W_WINROW(wp) + 1;
	    }
	    ورنہ
		// character is out of the window
		صف = scol = ccol = ecol = 0;
	}
    }
    *rowp = صف;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    اگر (wp->w_p_rl && صف > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	صحیح endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_چوڑائی - endoff;
    }
# endif
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    خالی
f_screenpos(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    صحیح		صف = 0;
    صحیح		scol = 0, ccol = 0, ecol = 0;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
    dict = rettv->vval.v_dict;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_نمبر_arg(argvars, 2) == FAIL))
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    اگر (wp == تہی)
	واپس;

    pos.lnum = tv_get_نمبر(&argvars[1]);
    اگر (pos.lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
    {
	semsg(_(e_invalid_سطر_نمبر_nr), pos.lnum);
	واپس;
    }
    pos.ستون = tv_get_نمبر(&argvars[2]) - 1;
    اگر (pos.ستون < 0)
	pos.ستون = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &صف, &scol, &ccol, &ecol);

    dict_جوڑو_نمبر(dict, "row", صف);
    dict_جوڑو_نمبر(dict, "col", scol);
    dict_جوڑو_نمبر(dict, "curscol", ccol);
    dict_جوڑو_نمبر(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    ساکن صحیح
virtcol2col(win_T *wp, linenr_T lnum, صحیح vcol)
{
    صحیح		انحراف = vcol2col(wp, lnum, vcol - 1, تہی);
    char_u	*سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);
    char_u	*p = سطر + انحراف;

    اگر (*p == NUL)
    {
	اگر (p == سطر)  // empty line
	    واپس 0;
	// Move to the first byte of the last char.
	MB_PTR_عقب(سطر, p);
    }
    واپس (صحیح)(p - سطر + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    خالی
f_virtcol2col(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    صحیح		screencol;
    صحیح		error = FALSE;

    rettv->vval.v_نمبر = -1;

    اگر (check_for_نمبر_arg(argvars, 0) == FAIL
	    || check_for_نمبر_arg(argvars, 1) == FAIL
	    || check_for_نمبر_arg(argvars, 2) == FAIL)
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    اگر (wp == تہی)
	واپس;

    lnum = tv_get_نمبر_chk(&argvars[1], &error);
    اگر (error || lnum < 0 || lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
	واپس;

    screencol = tv_get_نمبر_chk(&argvars[2], &error);
    اگر (error || screencol < 0)
	واپس;

    rettv->vval.v_نمبر = virtcol2col(wp, lnum, screencol);
}
#اگر_ختم

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
ساکن خالی cursor_correct_sms(خالی)
{
    اگر (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	واپس;

    طویل    so = get_scrolloff_قدر();
    صحیح	    width1 = curwin->w_چوڑائی - curwin_ستون_off();
    صحیح	    width2 = width1 + curwin_ستون_off2();
    صحیح	    so_ستون_تعداد = so == 0 ? 0 : width1 + (so - 1) * width2;
    صحیح	    space_ستون_تعداد = (curwin->w_اونچائی - 1) * width2;
    صحیح	    overlap, چوٹی, bot;
    صحیح	    سائز = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    اگر (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_ستون_تعداد = 0;               // Ignore 'scrolloff' at top of buffer.
    ورنہ اگر (so_ستون_تعداد > space_ستون_تعداد / 2)
	so_ستون_تعداد = space_ستون_تعداد / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    جب (so_ستون_تعداد > سائز && so_ستون_تعداد - width2 >= width1 && width1 > 0)
	so_ستون_تعداد -= width2;
    اگر (so_ستون_تعداد >= width1 && so_ستون_تعداد > سائز)
	so_ستون_تعداد -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_چوڑائی - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    چوٹی = curwin->w_skipcol + (so_ستون_تعداد != 0 ? so_ستون_تعداد : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_اونچائی - 1) * width2
								    - so_ستون_تعداد;
    validate_virtcol();
    colnr_T ستون = curwin->w_virtcol;

    اگر (ستون < چوٹی)
    {
	اگر (ستون < width1)
	    ستون += width1;
	جب (width2 > 0 && ستون < چوٹی)
	    ستون += width2;
    }
    ورنہ
	جب (width2 > 0 && ستون >= bot)
	    ستون -= width2;

    اگر (ستون != curwin->w_virtcol)
    {
	صحیح rc;

	curwin->w_curswant = ستون;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	اگر (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی)
	{
	    validate_virtcol();
	    اگر (curwin->w_virtcol < curwin->w_skipcol + overlap)
	    {
		// Cursor still not visible: move it to the next line instead.
		curwin->w_cursor.lnum++;
		curwin->w_cursor.ستون = 0;
		curwin->w_cursor.coladd = 0;
		curwin->w_curswant = 0;
		curwin->w_valid &= ~VALID_VIRTCOL;
	    }
	}
    }
}

/*
 * Scroll "count" lines up or down, and redraw.
 */
    خالی
scroll_redraw(صحیح اوپر, طویل گنتی)
{
    linenr_T	پچھلا_topline = curwin->w_topline;
    صحیح		پچھلا_skipcol = curwin->w_skipcol;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		پچھلا_topfill = curwin->w_topfill;
#اگر_ختم
    linenr_T	پچھلا_lnum = curwin->w_cursor.lnum;

    اگر (اوپر)
	scrollup(گنتی, TRUE);
    ورنہ
	scrolldown(گنتی, TRUE);
    اگر (get_scrolloff_قدر() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	جب (curwin->w_topline == پچھلا_topline
		&& curwin->w_skipcol == پچھلا_skipcol
#اگر_تعریف_شدہ FEAT_DIFF
		&& curwin->w_topfill == پچھلا_topfill
#اگر_ختم
		)
	{
	    اگر (اوپر)
	    {
		اگر (curwin->w_cursor.lnum > پچھلا_lnum
			|| cursor_نیچے_کو(1L, FALSE) == FAIL)
		    توڑو;
	    }
	    ورنہ
	    {
		اگر (curwin->w_cursor.lnum < پچھلا_lnum
			|| پچھلا_topline == 1L
			|| cursor_اوپر(1L, FALSE) == FAIL)
		    توڑو;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    اگر (curwin->w_cursor.lnum != پچھلا_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    خالی
scrolldown(
    طویل	سطر_گنتی,
    صحیح		byfold غیر_مستعمل)	// TRUE: count a closed fold as one line
{
    طویل	مکمل = 0;	// total # of physical lines done
    صحیح		wrow;
    صحیح		moved = FALSE;
    صحیح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    صحیح		width1 = 0;
    صحیح		width2 = 0;

    اگر (do_sms)
    {
	width1 = curwin->w_چوڑائی - curwin_ستون_off();
	width2 = width1 + curwin_ستون_off2();
    }

#اگر_تعریف_شدہ FEAT_FOLDING
    linenr_T	پہلا;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (خالی)hasFolding(curwin->w_topline, &curwin->w_topline, تہی);
#اگر_ختم
    validate_cursor();		// w_wrow needs to be valid
    برائے (صحیح todo = سطر_گنتی; todo > 0; --todo)
    {
#اگر_تعریف_شدہ FEAT_DIFF
	اگر (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_اونچائی - 1)
	{
	    ++curwin->w_topfill;
	    ++مکمل;
	}
	ورنہ
#اگر_ختم
	{
	    // break when at the very top
	    اگر (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		توڑو;
	    اگر (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		اگر (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		ورنہ
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++مکمل;
	    }
	    ورنہ
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#اگر_تعریف_شدہ FEAT_DIFF
		curwin->w_topfill = 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		اگر (hasFolding(curwin->w_topline, &پہلا, تہی))
		{
		    ++مکمل;
		    اگر (!byfold)
			todo -= curwin->w_topline - پہلا - 1;
		    curwin->w_botline -= curwin->w_topline - پہلا;
		    curwin->w_topline = پہلا;
		}
		ورنہ
#اگر_ختم
		اگر (do_sms)
		{
		    صحیح سائز = linetabsize_eol(curwin, curwin->w_topline);
		    اگر (سائز > width1)
		    {
			curwin->w_skipcol = width1;
			سائز -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    جب (سائز > width2)
		    {
			curwin->w_skipcol += width2;
			سائز -= width2;
		    }
		    ++مکمل;
		}
		ورنہ
		    مکمل += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += مکمل;		// keep w_wrow updated
    curwin->w_cline_صف += مکمل;	// keep w_cline_row updated

#اگر_تعریف_شدہ FEAT_DIFF
    اگر (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_صف = 0;
    check_topfill(curwin, TRUE);
#اگر_ختم

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    اگر (curwin->w_p_wrap && curwin->w_چوڑائی != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_اونچائی - 1 -
	    curwin->w_virtcol / curwin->w_چوڑائی;
    }
    جب (wrow >= curwin->w_اونچائی && curwin->w_cursor.lnum > 1)
    {
#اگر_تعریف_شدہ FEAT_FOLDING
	اگر (hasFolding(curwin->w_cursor.lnum, &پہلا, تہی))
	{
	    --wrow;
	    اگر (پہلا == 1)
		curwin->w_cursor.lnum = 1;
	    ورنہ
		curwin->w_cursor.lnum = پہلا - 1;
	}
	ورنہ
#اگر_ختم
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    اگر (moved)
    {
#اگر_تعریف_شدہ FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#اگر_ختم
	coladvance(curwin->w_curswant);
    }
    اگر (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    خالی
scrollup(
    طویل	سطر_گنتی,
    صحیح		byfold غیر_مستعمل)	// TRUE: count a closed fold as one line
{
    صحیح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    اگر (do_sms
#اگر_تعریف_شدہ FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#اگر_ختم
       )
    {
	صحیح	    width1 = curwin->w_چوڑائی - curwin_ستون_off();
	صحیح	    width2 = width1 + curwin_ستون_off2();
	صحیح	    سائز = 0;
	colnr_T	    پچھلا_skipcol = curwin->w_skipcol;

	اگر (do_sms)
	    سائز = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	برائے (صحیح todo = سطر_گنتی; todo > 0; --todo)
	{
#اگر_تعریف_شدہ FEAT_DIFF
	    اگر (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    ورنہ
#اگر_ختم
	    {
		linenr_T lnum = curwin->w_topline;

#اگر_تعریف_شدہ FEAT_FOLDING
		اگر (byfold)
		    // for a closed fold: go to the last line in the fold
		    (خالی)hasFolding(lnum, تہی, &lnum);
#اگر_ختم
		اگر (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    صحیح جوڑو = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += جوڑو;
		    اگر (curwin->w_skipcol >= سائز)
		    {
			اگر (lnum == curbuf->b_ml.ml_سطر_گنتی)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= جوڑو;
			    توڑو;
			}
			++lnum;
		    }
		}
		ورنہ
		{
		    اگر (lnum >= curbuf->b_ml.ml_سطر_گنتی)
			توڑو;
		    ++lnum;
		}

		اگر (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#اگر_تعریف_شدہ FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#اگر_ختم
		    curwin->w_skipcol = 0;
		    اگر (todo > 1 && do_sms)
			سائز = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	اگر (پچھلا_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    ورنہ
    {
	curwin->w_topline += سطر_گنتی;
	curwin->w_botline += سطر_گنتی;	// approximate w_botline
    }

    اگر (curwin->w_topline > curbuf->b_ml.ml_سطر_گنتی)
	curwin->w_topline = curbuf->b_ml.ml_سطر_گنتی;
    اگر (curwin->w_botline > curbuf->b_ml.ml_سطر_گنتی + 1)
	curwin->w_botline = curbuf->b_ml.ml_سطر_گنتی + 1;

#اگر_تعریف_شدہ FEAT_DIFF
    check_topfill(curwin, FALSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(خالی)hasFolding(curwin->w_topline, &curwin->w_topline, تہی);
#اگر_ختم

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    اگر (curwin->w_cursor.lnum < curwin->w_topline)
    {
	curwin->w_cursor.lnum = curwin->w_topline;
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	coladvance(curwin->w_curswant);
    }
}

/*
 * Called after changing the cursor column: make sure that curwin->w_skipcol is
 * valid for 'smoothscroll'.
 */
    خالی
adjust_skipcol(خالی)
{
    اگر (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	واپس;

    صحیح	    width1 = curwin->w_چوڑائی - curwin_ستون_off();
    اگر (width1 <= 0)
	واپس;  // no text will be displayed

    صحیح	    width2 = width1 + curwin_ستون_off2();
    طویل    so = get_scrolloff_قدر();
    صحیح	    scrolloff_ستون_تعداد = so == 0 ? 0 : width1 + (so - 1) * width2;
    صحیح	    scrolled = FALSE;
    صحیح	    صف = 0;
    صحیح	    overlap, ستون;

    validate_cheight();
    اگر (curwin->w_cline_اونچائی == curwin->w_اونچائی
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_اونچائی)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	واپس;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_چوڑائی - width2);
    جب (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_ستون_تعداد)
    {
	// scroll a screen line down
	اگر (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	ورنہ
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    اگر (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	واپس;  // don't scroll in the other direction now
    }

    ستون = curwin->w_virtcol + scrolloff_ستون_تعداد;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    اگر (scrolloff_ستون_تعداد > 0)
    {
	صحیح سائز = linetabsize_eol(curwin, curwin->w_topline);
	سائز = width1 + width2 * ((سائز - width1 + width2 - 1) / width2);
	جب (ستون > سائز)
	    ستون -= width2;
    }
    ستون -= curwin->w_skipcol;

    اگر (ستون >= width1)
    {
	ستون -= width1;
	++صف;
    }
    اگر (ستون > width2)
    {
	صف += ستون / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	ستون = ستون % width2;
    }
    اگر (صف >= curwin->w_اونچائی)
    {
	اگر (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --صف;
	}
	اگر (صف >= curwin->w_اونچائی)
	    curwin->w_skipcol += (صف - curwin->w_اونچائی) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#اگر_تعریف_شدہ FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    خالی
check_topfill(
    win_T	*wp,
    صحیح		نیچے_کو)	// when TRUE scroll down when not enough space
{
    صحیح		n;

    اگر (wp->w_topfill <= 0)
	واپس;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    اگر (wp->w_topfill + n > wp->w_اونچائی)
    {
	اگر (نیچے_کو && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	ورنہ
	{
	    wp->w_topfill = wp->w_اونچائی - n;
	    اگر (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#اگر_ختم

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    خالی
scrolldown_clamp(خالی)
{
    صحیح		انتہا_صف;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#اگر_ختم

    اگر (curwin->w_topline <= 1
#اگر_تعریف_شدہ FEAT_DIFF
	    && !can_fill
#اگر_ختم
	    )
	واپس;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    انتہا_صف = curwin->w_wrow;
#اگر_تعریف_شدہ FEAT_DIFF
    اگر (can_fill)
	++انتہا_صف;
    ورنہ
	انتہا_صف += plines_nofill(curwin->w_topline - 1);
#ورنہ
    انتہا_صف += plines(curwin->w_topline - 1);
#اگر_ختم
    اگر (curwin->w_p_wrap && curwin->w_چوڑائی != 0)
    {
	validate_cheight();
	validate_virtcol();
	انتہا_صف += curwin->w_cline_اونچائی - 1 -
	    curwin->w_virtcol / curwin->w_چوڑائی;
    }
    اگر (انتہا_صف < curwin->w_اونچائی - get_scrolloff_قدر())
    {
#اگر_تعریف_شدہ FEAT_DIFF
	اگر (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	ورنہ
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#ورنہ
	--curwin->w_topline;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
	(خالی)hasFolding(curwin->w_topline, &curwin->w_topline, تہی);
#اگر_ختم
	--curwin->w_botline;	    // approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    خالی
scrollup_clamp(خالی)
{
    صحیح	    آغاز_صف;

    اگر (curwin->w_topline == curbuf->b_ml.ml_سطر_گنتی
#اگر_تعریف_شدہ FEAT_DIFF
	    && curwin->w_topfill == 0
#اگر_ختم
	    )
	واپس;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#اگر_تعریف_شدہ FEAT_DIFF
    آغاز_صف = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#ورنہ
    آغاز_صف = curwin->w_wrow - plines(curwin->w_topline);
#اگر_ختم
    اگر (curwin->w_p_wrap && curwin->w_چوڑائی != 0)
    {
	validate_virtcol();
	آغاز_صف -= curwin->w_virtcol / curwin->w_چوڑائی;
    }
    اگر (آغاز_صف >= get_scrolloff_قدر())
    {
#اگر_تعریف_شدہ FEAT_DIFF
	اگر (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	ورنہ
#اگر_ختم
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    (خالی)hasFolding(curwin->w_topline, تہی, &curwin->w_topline);
#اگر_ختم
	    ++curwin->w_topline;
	}
	++curwin->w_botline;		// approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Add one line above "lp->lnum".  This can be a filler line, a closed fold or
 * a (wrapped) text line.  Uses and sets "lp->fill".
 * Returns the height of the added line in "lp->height".
 * Lines above the first one are incredibly high: MAXCOL.
 */
    ساکن خالی
topline_عقب_winheight(
    lineoff_T	*lp,
    صحیح		winheight)	// when TRUE limit to window height
{
#اگر_تعریف_شدہ FEAT_DIFF
    اگر (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->اونچائی = 1;
    }
    ورنہ
#اگر_ختم
    {
	--lp->lnum;
#اگر_تعریف_شدہ FEAT_DIFF
	lp->fill = 0;
#اگر_ختم
	اگر (lp->lnum < 1)
	    lp->اونچائی = MAXCOL;
	ورنہ
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (hasFolding(lp->lnum, &lp->lnum, تہی))
	    // Add a closed fold
	    lp->اونچائی = 1;
	ورنہ
#اگر_ختم
	    lp->اونچائی = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    ساکن خالی
topline_عقب(lineoff_T *lp)
{
    topline_عقب_winheight(lp, TRUE);
}


/*
 * Add one line below "lp->lnum".  This can be a filler line, a closed fold or
 * a (wrapped) text line.  Uses and sets "lp->fill".
 * Returns the height of the added line in "lp->height".
 * Lines below the last one are incredibly high.
 */
    ساکن خالی
botline_forw(lineoff_T *lp)
{
#اگر_تعریف_شدہ FEAT_DIFF
    اگر (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->اونچائی = 1;
    }
    ورنہ
#اگر_ختم
    {
	++lp->lnum;
#اگر_تعریف_شدہ FEAT_DIFF
	lp->fill = 0;
#اگر_ختم
	اگر (lp->lnum > curbuf->b_ml.ml_سطر_گنتی)
	    lp->اونچائی = MAXCOL;
	ورنہ
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (hasFolding(lp->lnum, تہی, &lp->lnum))
	    {
		// Add a closed fold.
		lp->اونچائی = 1;
	    }
	    ورنہ
#اگر_ختم
	    lp->اونچائی = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    خالی
scroll_cursor_چوٹی(صحیح min_scroll, صحیح always)
{
    صحیح		scrolled = 0;
    صحیح		extra = 0;
    صحیح		مستعمل;
    صحیح		i;
    linenr_T	چوٹی;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    صحیح		old_skipcol = curwin->w_skipcol;
#اگر_تعریف_شدہ FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#اگر_ختم
    linenr_T	new_topline;
    صحیح		off = get_scrolloff_قدر();

    اگر (mouse_dragging > 0)
	off = mouse_dragging - 1;

    /*
     * Decrease topline until:
     * - it has become 1
     * - (part of) the cursor line is moved off the screen or
     * - moved at least 'scrolljump' lines and
     * - at least 'scrolloff' lines above and below the cursor
     */
    validate_cheight();
    مستعمل = curwin->w_cline_اونچائی; // includes filler lines above
    اگر (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = مستعمل;

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (hasFolding(curwin->w_cursor.lnum, &چوٹی, &bot))
    {
	--چوٹی;
	++bot;
    }
    ورنہ
#اگر_ختم
    {
	چوٹی = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = چوٹی + 1;

#اگر_تعریف_شدہ FEAT_DIFF
    // "used" already contains the number of filler lines above, don't add it
    // again.
    // Hide filler lines above cursor line by adding them to "extra".
    extra += diff_check_fill(curwin, curwin->w_cursor.lnum);
#اگر_ختم

    /*
     * Check if the lines from "top" to "bot" fit in the window.  If they do,
     * set new_topline and advance "top" and "bot" to include more lines.
     */
    جب (چوٹی > 0)
    {
#اگر_تعریف_شدہ FEAT_FOLDING
	اگر (hasFolding(چوٹی, &چوٹی, تہی))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	ورنہ
#اگر_ختم
	    i = PLINES_NOFILL(چوٹی);
	اگر (چوٹی < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	اگر ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    توڑو;

	مستعمل += i;
	اگر (extra + i <= off && bot < curbuf->b_ml.ml_سطر_گنتی)
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (hasFolding(bot, تہی, &bot))
		// count one logical line for a sequence of folded lines
		++مستعمل;
	    ورنہ
#اگر_ختم
		مستعمل += plines(bot);
	}
	اگر (مستعمل > curwin->w_اونچائی)
	    توڑو;

	extra += i;
	new_topline = چوٹی;
	--چوٹی;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    اگر (مستعمل > curwin->w_اونچائی)
	scroll_cursor_halfway(FALSE, FALSE);
    ورنہ
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	اگر (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	اگر (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#اگر_تعریف_شدہ FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	اگر (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    اگر (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#اگر_ختم
	اگر (curwin->w_topline != old_topline)
	    reset_skipcol();
	ورنہ اگر (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    اگر (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		صحیح	width1 = curwin->w_چوڑائی - curwin_ستون_off();
		صحیح	width2 = width1 + curwin_ستون_off2();
		صحیح	plines_off = 0;
		صحیح	skipcol;

		اگر (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		اگر (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	اگر (curwin->w_topline != old_topline
		|| curwin->w_skipcol != old_skipcol
#اگر_تعریف_شدہ FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#اگر_ختم
		)
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	curwin->w_valid |= VALID_TOPLINE;
    }
}

/*
 * Set w_empty_rows and w_filler_rows for window "wp", having used up "used"
 * screen lines for text lines.
 */
    خالی
سیٹ_خالی_خانے_صفیں(win_T *wp, صحیح مستعمل)
{
#اگر_تعریف_شدہ FEAT_DIFF
    wp->w_filler_صفیں = 0;
#اگر_ختم
    اگر (مستعمل == 0)
	wp->w_خالی_خانے_صفیں = 0;	// single line that doesn't fit
    ورنہ
    {
	wp->w_خالی_خانے_صفیں = wp->w_اونچائی - مستعمل;
#اگر_تعریف_شدہ FEAT_DIFF
	اگر (wp->w_botline <= wp->w_بفر->b_ml.ml_سطر_گنتی)
	{
	    wp->w_filler_صفیں = diff_check_fill(wp, wp->w_botline);
	    اگر (wp->w_خالی_خانے_صفیں > wp->w_filler_صفیں)
		wp->w_خالی_خانے_صفیں -= wp->w_filler_صفیں;
	    ورنہ
	    {
		wp->w_filler_صفیں = wp->w_خالی_خانے_صفیں;
		wp->w_خالی_خانے_صفیں = 0;
	    }
	}
#اگر_ختم
    }
}

/*
 * Recompute topline to put the cursor at the bottom of the window.
 * When scrolling scroll at least "min_scroll" lines.
 * If "set_topbot" is TRUE, set topline and botline first (for "zb").
 * This is messy stuff!!!
 */
    خالی
scroll_cursor_bot(صحیح min_scroll, صحیح سیٹ_topbot)
{
    صحیح		مستعمل;
    صحیح		scrolled = 0;
    صحیح		extra = 0;
    صحیح		i;
    linenr_T	سطر_گنتی;
    linenr_T	old_topline = curwin->w_topline;
    صحیح		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		old_topfill = curwin->w_topfill;
    صحیح		fill_نیچے_کھڑکی;
#اگر_ختم
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    صحیح		old_خالی_خانے_صفیں = curwin->w_خالی_خانے_صفیں;
    linenr_T	cln;		    // Cursor Line Number
    طویل	so = get_scrolloff_قدر();
    صحیح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    اگر (سیٹ_topbot)
    {
	مستعمل = 0;
	linenr_T cln_آخری = cln;
#اگر_تعریف_شدہ FEAT_FOLDING
	(خالی)hasFolding(cln, تہی, &cln_آخری);
#اگر_ختم
	curwin->w_botline = cln_آخری + 1;
	loff.lnum = cln_آخری + 1;
#اگر_تعریف_شدہ FEAT_DIFF
	loff.fill = 0;
#اگر_ختم
	جب (TRUE)
	{
	    topline_عقب_winheight(&loff, FALSE);
	    اگر (loff.اونچائی == MAXCOL)
		توڑو;
	    اگر (مستعمل + loff.اونچائی > curwin->w_اونچائی)
	    {
		اگر (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    اگر (مستعمل < curwin->w_اونچائی)
		    {
			صحیح plines_انحراف = مستعمل + loff.اونچائی
							    - curwin->w_اونچائی;
			مستعمل = curwin->w_اونچائی;
#اگر_تعریف_شدہ FEAT_DIFF
			curwin->w_topfill = loff.fill;
#اگر_ختم
			curwin->w_topline = loff.lnum;
			curwin->w_skipcol = skipcol_from_plines(
							curwin, plines_انحراف);
		    }
		}
		توڑو;
	    }
#اگر_تعریف_شدہ FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#اگر_ختم
	    curwin->w_topline = loff.lnum;
	    مستعمل += loff.اونچائی;
	}

	سیٹ_خالی_خانے_صفیں(curwin, مستعمل);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	اگر (curwin->w_topline != old_topline
#اگر_تعریف_شدہ FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#اگر_ختم
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    اگر (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    ورنہ
		reset_skipcol();
	}
    }
    ورنہ
	validate_botline();

    // The lines of the cursor line itself are always used.
#اگر_تعریف_شدہ FEAT_DIFF
    مستعمل = plines_nofill(cln);
#ورنہ
    validate_cheight();
    مستعمل = curwin->w_cline_اونچائی;
#اگر_ختم

    اگر (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	صحیح upto_cursor = plines_win_ستون(curwin, cln, curwin->w_cursor.ستون);

	اگر (upto_cursor < مستعمل)
	    مستعمل = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    اگر (cln >= curwin->w_botline)
    {
	scrolled = مستعمل;
	اگر (cln == curwin->w_botline)
	    scrolled -= curwin->w_خالی_خانے_صفیں;
	اگر (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    صحیح چوٹی_plines =
#اگر_تعریف_شدہ FEAT_DIFF
			    plines_win_nofill
#ورنہ
			    plines_win
#اگر_ختم
					(curwin, curwin->w_topline, FALSE);
	    صحیح width1 = curwin->w_چوڑائی - curwin_ستون_off();

	    اگر (width1 > 0)
	    {
		صحیح width2 = width1 + curwin_ستون_off2();
		صحیح skip_سطریں = 0;

		// A similar formula is used in curs_columns().
		اگر (curwin->w_skipcol > width1)
		    skip_سطریں += (curwin->w_skipcol - width1) / width2 + 1;
		ورنہ اگر (curwin->w_skipcol > 0)
		    skip_سطریں = 1;

		چوٹی_plines -= skip_سطریں;
		اگر (چوٹی_plines > curwin->w_اونچائی)
		{
		    scrolled += (چوٹی_plines - curwin->w_اونچائی);
		}
	    }
	}
    }

    /*
     * Stop counting lines to scroll when
     * - hitting start of the file
     * - scrolled nothing or at least 'sj' lines
     * - at least 'scrolloff' lines below the cursor
     * - lines between botline and cursor have been counted
     */
#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
#اگر_ختم
    {
	loff.lnum = cln;
	boff.lnum = cln;
    }
#اگر_تعریف_شدہ FEAT_DIFF
    loff.fill = 0;
    boff.fill = 0;
    fill_نیچے_کھڑکی = diff_check_fill(curwin, curwin->w_botline)
						      - curwin->w_filler_صفیں;
#اگر_ختم

    جب (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	اگر ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_سطر_گنتی)
		&& loff.lnum <= curwin->w_botline
#اگر_تعریف_شدہ FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_نیچے_کھڑکی)
#اگر_ختم
		)
	    توڑو;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_عقب(&loff);
	اگر (loff.اونچائی == MAXCOL)
	    مستعمل = MAXCOL;
	ورنہ
	    مستعمل += loff.اونچائی;
	اگر (مستعمل > curwin->w_اونچائی)
	    توڑو;
	اگر (loff.lnum >= curwin->w_botline
#اگر_تعریف_شدہ FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_نیچے_کھڑکی)
#اگر_ختم
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.اونچائی;
	    اگر (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_خالی_خانے_صفیں;
	}

	اگر (boff.lnum < curbuf->b_ml.ml_سطر_گنتی)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    مستعمل += boff.اونچائی;
	    اگر (مستعمل > curwin->w_اونچائی)
		توڑو;
	    اگر (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.اونچائی;
		اگر (boff.lnum >= curwin->w_botline
#اگر_تعریف_شدہ FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_صفیں)
#اگر_ختم
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.اونچائی;
		    اگر (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_خالی_خانے_صفیں;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    اگر (scrolled <= 0)
	سطر_گنتی = 0;
    // more than a screenfull, don't scroll but redraw
    ورنہ اگر (مستعمل > curwin->w_اونچائی)
	سطر_گنتی = مستعمل;
    // scroll minimal number of lines
    ورنہ
    {
	سطر_گنتی = 0;
#اگر_تعریف_شدہ FEAT_DIFF
	boff.fill = curwin->w_topfill;
#اگر_ختم
	boff.lnum = curwin->w_topline - 1;
	برائے (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.اونچائی;
	    ++سطر_گنتی;
	}
	اگر (i < scrolled)	// below curwin->w_botline, don't scroll
	    سطر_گنتی = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    اگر (سطر_گنتی >= curwin->w_اونچائی && سطر_گنتی > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    ورنہ اگر (سطر_گنتی > 0)
    {
	اگر (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	ورنہ
	    scrollup(سطر_گنتی, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    اگر (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && سیٹ_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_خالی_خانے_صفیں = old_خالی_خانے_صفیں;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    اگر (سیٹ_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    خالی
scroll_cursor_halfway(صحیح atend, صحیح prefer_above)
{
    صحیح		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		topfill = 0;
#اگر_ختم
    صحیح		نیچے = 0;
    صحیح		مستعمل;
    lineoff_T	loff;
    lineoff_T	boff;
#اگر_تعریف_شدہ FEAT_DIFF
    linenr_T	old_topline = curwin->w_topline;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // if the width changed this needs to be updated first
    may_update_popup_position();
#اگر_ختم
    loff.lnum = boff.lnum = curwin->w_cursor.lnum;
#اگر_تعریف_شدہ FEAT_FOLDING
    (خالی)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
    مستعمل = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#ورنہ
    مستعمل = plines(loff.lnum);
#اگر_ختم
    topline = loff.lnum;

    صحیح want_اونچائی;
    صحیح do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    اگر (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	اگر (atend)
	{
	    want_اونچائی = (curwin->w_اونچائی - مستعمل) / 2;
	    مستعمل = 0;
	}
	ورنہ
	    want_اونچائی = curwin->w_اونچائی;
    }

    جب (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	اگر (do_sms)
	{
	    topline_عقب_winheight(&loff, FALSE);
	    اگر (loff.اونچائی == MAXCOL)
		توڑو;
	    مستعمل += loff.اونچائی;
	    اگر (!atend && boff.lnum < curbuf->b_ml.ml_سطر_گنتی)
	    {
		botline_forw(&boff);
		مستعمل += boff.اونچائی;
	    }
	    اگر (مستعمل > want_اونچائی)
	    {
		اگر (مستعمل - loff.اونچائی < want_اونچائی)
		{
		    topline = loff.lnum;
#اگر_تعریف_شدہ FEAT_DIFF
		    topfill = loff.fill;
#اگر_ختم
		    skipcol = skipcol_from_plines(curwin, مستعمل - want_اونچائی);
		}
		توڑو;
	    }
	    topline = loff.lnum;
#اگر_تعریف_شدہ FEAT_DIFF
	    topfill = loff.fill;
#اگر_ختم
	    جاری;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	صحیح مکمل = FALSE;
	برائے (صحیح باری = 1; باری <= 2; ++باری)
	{
	    اگر (prefer_above ? (باری == 2 && نیچے < above)
			     : (باری == 1 && نیچے <= above))
	    {
		// add a line below the cursor
		اگر (boff.lnum < curbuf->b_ml.ml_سطر_گنتی)
		{
		    botline_forw(&boff);
		    مستعمل += boff.اونچائی;
		    اگر (مستعمل > curwin->w_اونچائی)
		    {
			مکمل = TRUE;
			توڑو;
		    }
		    نیچے += boff.اونچائی;
		}
		ورنہ
		{
		    ++نیچے;	    // count a "~" line
		    اگر (atend)
			++مستعمل;
		}
	    }

	    اگر (prefer_above ? (باری == 1 && نیچے >= above)
			     : (باری == 1 && نیچے > above))
	    {
		// add a line above the cursor
		topline_عقب(&loff);
		اگر (loff.اونچائی == MAXCOL)
		    مستعمل = MAXCOL;
		ورنہ
		    مستعمل += loff.اونچائی;
		اگر (مستعمل > curwin->w_اونچائی)
		{
		    مکمل = TRUE;
		    توڑو;
		}
		above += loff.اونچائی;
		topline = loff.lnum;
#اگر_تعریف_شدہ FEAT_DIFF
		topfill = loff.fill;
#اگر_ختم
	    }
	}
	اگر (مکمل)
	    توڑو;
    }

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (!hasFolding(topline, &curwin->w_topline, تہی))
#اگر_ختم
    {
	اگر (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    اگر (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    ورنہ اگر (do_sms)
		reset_skipcol();
	}
    }
#اگر_تعریف_شدہ FEAT_DIFF
    curwin->w_topfill = topfill;
    اگر (old_topline > curwin->w_topline + curwin->w_اونچائی)
	curwin->w_botfill = false;
    check_topfill(curwin, FALSE);
#اگر_ختم
    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
    curwin->w_valid |= VALID_TOPLINE;
}

/*
 * Correct the cursor position so that it is in a part of the screen at least
 * 'scrolloff' lines from the top and bottom, if possible.
 * If not possible, put it at the same position as scroll_cursor_halfway().
 * When called topline must be valid!
 */
    خالی
cursor_correct(خالی)
{
    صحیح		above = 0;	    // screen lines above topline
    linenr_T	topline;
    صحیح		نیچے = 0;	    // screen lines below botline
    linenr_T	botline;
    صحیح		above_wanted, نیچے_wanted;
    linenr_T	cln;		    // Cursor Line Number
    صحیح		max_off;
    طویل	so = get_scrolloff_قدر();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    نیچے_wanted = so;
    اگر (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	نیچے_wanted = mouse_dragging - 1;
    }
    اگر (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_اونچائی / 2;
	اگر (نیچے_wanted > max_off)
	    نیچے_wanted = max_off;
    }
    validate_botline();
    اگر (curwin->w_botline == curbuf->b_ml.ml_سطر_گنتی + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	نیچے_wanted = 0;
	max_off = (curwin->w_اونچائی - 1) / 2;
	اگر (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    اگر (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - نیچے_wanted
#اگر_تعریف_شدہ FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#اگر_ختم
	    )
	واپس;

    /*
     * Narrow down the area where the cursor can be put by taking lines from
     * the top and the bottom until:
     * - the desired context lines are found
     * - the lines from the top is past the lines from the bottom
     */
    topline = curwin->w_topline;
    botline = curwin->w_botline - 1;
#اگر_تعریف_شدہ FEAT_DIFF
    // count filler lines as context
    above = curwin->w_topfill;
    نیچے = curwin->w_filler_صفیں;
#اگر_ختم
    جب ((above < above_wanted || نیچے < نیچے_wanted) && topline < botline)
    {
	اگر (نیچے < نیچے_wanted && (نیچے <= above || above >= above_wanted))
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (hasFolding(botline, &botline, تہی))
		++نیچے;
	    ورنہ
#اگر_ختم
		نیچے += plines(botline);
	    --botline;
	}
	اگر (above < above_wanted && (above < نیچے || نیچے >= نیچے_wanted))
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (hasFolding(topline, تہی, &topline))
		++above;
	    ورنہ
#اگر_ختم
		above += PLINES_NOFILL(topline);
#اگر_تعریف_شدہ FEAT_DIFF
	    // Count filler lines below this line as context.
	    اگر (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#اگر_ختم
	    ++topline;
	}
    }
    اگر (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    ورنہ اگر (topline > botline)
	curwin->w_cursor.lnum = botline;
    ورنہ
    {
	اگر (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	اگر (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_سطر_گنتی)
	{
	    curwin->w_cursor.lnum = botline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
    }
    check_cursor_moved(curwin);
    curwin->w_valid |= VALID_TOPLINE;
}

/*
 * Decide how much overlap to use for page-up or page-down scrolling.
 * This is symmetric, so that doing both keeps the same lines displayed.
 * Three lines are examined:
 *
 *  before CTRL-F          after CTRL-F / before CTRL-B
 *     etc.                    l1
 *  l1 last but one line       ------------
 *  l2 last text line          l2 top text line
 *  -------------              l3 second text line
 *  l3                            etc.
 */
ساکن صحیح get_scroll_overlap(صحیح dir)
{
    lineoff_T loff;
    صحیح	    min_اونچائی = curwin->w_اونچائی - 2;

    validate_botline();
    اگر ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_سطر_گنتی))
	واپس min_اونچائی + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#اگر_تعریف_شدہ FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_صفیں : curwin->w_topfill);
    loff.اونچائی = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#ورنہ
    loff.اونچائی = plines(loff.lnum);
#اگر_ختم

    صحیح h1 = loff.اونچائی;
    اگر (h1 > min_اونچائی)
	واپس min_اونچائی + 2;  // no overlap
    اگر (dir == FORWARD)
	topline_عقب(&loff);
    ورنہ
	botline_forw(&loff);

    صحیح h2 = loff.اونچائی;
    اگر (h2 == MAXCOL || h2 + h1 > min_اونچائی)
	واپس min_اونچائی + 2;  // no overlap
    اگر (dir == FORWARD)
	topline_عقب(&loff);
    ورنہ
	botline_forw(&loff);

    صحیح h3 = loff.اونچائی;
    اگر (h3 == MAXCOL || h3 + h2 > min_اونچائی)
	واپس min_اونچائی + 2;  // no overlap
    اگر (dir == FORWARD)
	topline_عقب(&loff);
    ورنہ
	botline_forw(&loff);

    صحیح h4 = loff.اونچائی;
    اگر (h4 == MAXCOL || h4 + h3 + h2 > min_اونچائی || h3 + h2 + h1 > min_اونچائی)
	واپس min_اونچائی + 1;  // 1 line overlap
    ورنہ
	واپس min_اونچائی;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
ساکن صحیح scroll_with_sms(صحیح dir, طویل گنتی, طویل *curscount)
{
    صحیح		پچھلا_sms = curwin->w_p_sms;
    colnr_T	پچھلا_skipcol = curwin->w_skipcol;
    linenr_T	پچھلا_topline = curwin->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		پچھلا_topfill = curwin->w_topfill;
#اگر_ختم

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, گنتی);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    اگر (!پچھلا_sms && curwin->w_skipcol > 0)
    {
	صحیح fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	اگر (labs(curwin->w_topline - پچھلا_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	صحیح width1 = curwin->w_چوڑائی - curwin_ستون_off();
	صحیح width2 = width1 + curwin_ستون_off2();
	گنتی = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	اگر (fixdir == FORWARD)
	    گنتی = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, گنتی);
	*curscount += گنتی * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = پچھلا_sms;

    واپس curwin->w_topline != پچھلا_topline
#اگر_تعریف_شدہ FEAT_DIFF
	|| curwin->w_topfill != پچھلا_topfill
#اگر_ختم
	|| curwin->w_skipcol != پچھلا_skipcol;
}

/*
 * Move screen "count" (half) pages up ("dir" is BACKWARD) or down ("dir" is
 * FORWARD) and update the screen. Handle moving the cursor and not scrolling
 * to reveal end of buffer lines for half-page scrolling with CTRL-D and CTRL-U.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
pagescroll(صحیح dir, طویل گنتی, صحیح half)
{
    صحیح		did_move = FALSE;
    صحیح		buflen = curbuf->b_ml.ml_سطر_گنتی;
    colnr_T	پچھلا_ستون = curwin->w_cursor.ستون;
    colnr_T	پچھلا_curswant = curwin->w_curswant;
    linenr_T	پچھلا_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    اگر (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	اگر (گنتی)
	    curwin->w_p_scr = MIN(curwin->w_اونچائی, گنتی);
	گنتی = MIN(curwin->w_اونچائی, curwin->w_p_scr);

	طویل curscount = گنتی;
	// Adjust count so as to not reveal end of buffer lines.
	اگر (dir == FORWARD
		    && (curwin->w_topline + curwin->w_اونچائی + گنتی > buflen
#اگر_تعریف_شدہ FEAT_FOLDING
							|| hasAnyFolding(curwin)
#اگر_ختم
	   ))
	{
	    صحیح n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    اگر (n - گنتی < curwin->w_اونچائی && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_اونچائی + گنتی);
	    اگر (n < curwin->w_اونچائی + گنتی)
		گنتی = n - curwin->w_اونچائی;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	اگر (گنتی > 0)
	{
	    did_move = scroll_with_sms(dir, گنتی, &curscount);
	    curwin->w_cursor.lnum = پچھلا_lnum;
	    curwin->w_cursor.ستون = پچھلا_ستون;
	    curwin->w_curswant = پچھلا_curswant;
	}

	// Move the cursor the same amount of screen lines.
	اگر (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	ورنہ اگر (dir == FORWARD)
	    cursor_نیچے_کو_inner(curwin, curscount);
	ورنہ
	    cursor_اوپر_inner(curwin, curscount);
    }
    ورنہ
    {
	// Scroll [count] times 'window' or current window height lines.
	گنتی *= ((ONE_کھڑکی && p_کھڑکی > 0 && p_کھڑکی < صفیں - 1) ?
				MAX(1, p_کھڑکی - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, گنتی, &گنتی);

	اگر (did_move)
	{
	    // Place cursor at top or bottom of window.
	    validate_botline();
	    linenr_T lnum = (dir == FORWARD ? curwin->w_topline
						    : curwin->w_botline - 1);
	    // In silent Ex mode the value of w_botline - 1 may be 0,
	    // but cursor lnum needs to be at least 1.
	    curwin->w_cursor.lnum = MAX(lnum, 1);
	}
    }

    اگر (get_scrolloff_قدر() > 0)
	cursor_correct();
#اگر_تعریف_شدہ FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#اگر_ختم
    did_move = did_move
	|| پچھلا_ستون != curwin->w_cursor.ستون
	|| پچھلا_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    اگر (!did_move)
	beep_flush();
    ورنہ اگر (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    ورنہ اگر (p_sol)
	nv_g_home_m_cmd(&ca);

    واپس did_move ? OK : FAIL;
}

    خالی
do_check_cursorbind(خالی)
{
    ساکن win_T	*پچھلا_curwin = تہی;
    ساکن buf_T	*پچھلا_curbuf = تہی;
    ساکن varnumber_T	پچھلا_changedtick = 0;
    ساکن pos_T	پچھلا_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    اگر (curwin == پچھلا_curwin && curbuf == پچھلا_curbuf
	    && CHANGEDTICK(curbuf) == پچھلا_changedtick
	    && EQUAL_POS(curwin->w_cursor, پچھلا_cursor))
	واپس;
    پچھلا_curwin = curwin;
    پچھلا_curbuf = curbuf;
    پچھلا_changedtick = CHANGEDTICK(curbuf);
    پچھلا_cursor = curwin->w_cursor;

    linenr_T	سطر = curwin->w_cursor.lnum;
    colnr_T	ستون = curwin->w_cursor.ستون;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    صحیح		سیٹ_curswant = curwin->w_سیٹ_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    صحیح		old_VIsual_select = VIsual_select;
    صحیح		old_VIsual_فعال = VIsual_فعال;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_فعال = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_بفر;
	// skip original window and windows with 'nocursorbind'
	اگر (curwin != old_curwin && curwin->w_p_crb)
	{
#اگر_تعریف_شدہ FEAT_DIFF
	    اگر (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_سطر(old_curbuf, سطر);
	    ورنہ
#اگر_ختم
		curwin->w_cursor.lnum = سطر;
	    curwin->w_cursor.ستون = ستون;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_سیٹ_curswant = سیٹ_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    صحیح restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    اگر (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    اگر (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    اگر (!curwin->w_p_scb)
		update_topline();
	    curwin->w_redr_حالت = true;
	}
    }

    /*
     * reset current-window
     */
    VIsual_select = old_VIsual_select;
    VIsual_فعال = old_VIsual_فعال;
    curwin = old_curwin;
    curbuf = old_curbuf;
}
