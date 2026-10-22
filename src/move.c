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

#ضمن "vim.h"

ساكن فراغ redraw_for_cursorline(win_T *wp);
ساكن صحيح scrolljump_قدر(فراغ);
ساكن صحيح check_چوٹی_وتھ(فراغ);
ساكن فراغ curs_صفاں(win_T *wp);

تعريفنوع بنية
{
    linenr_T	    lnum;	// line number
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		    fill;	// filler lines
#جے_انت
    صحيح		    اچائی;	// height of added line
} lineoff_T;

ساكن فراغ topline_پٹھ(lineoff_T *lp);
ساكن فراغ botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    ساكن صحيح
adjust_plines_for_skipcol(win_T *wp)
{
    إذا (wp->w_skipcol == 0)
	أرجع 0;

    صحيح چوڑائی = wp->w_چوڑائی - win_خانہ_off(wp);
    صحيح w2 = چوڑائی + win_خانہ_off2(wp);
    إذا (wp->w_skipcol >= چوڑائی && w2 > 0)
	أرجع (wp->w_skipcol - چوڑائی) / w2 + 1;

    أرجع 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    صحيح
plines_correct_topline(win_T *wp, linenr_T lnum, صحيح حد_بندی_winheight)
{
    صحيح n;
#جے_تعریف_شدہ FEAT_DIFF
    إذا (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    وإلا
#جے_انت
	n = plines_win(wp, lnum, FALSE);
    إذا (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    إذا (حد_بندی_winheight && n > wp->w_اچائی)
	n = wp->w_اچائی;
    أرجع n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    ساكن فراغ
comp_botline(win_T *wp)
{
    صحيح		n;
    linenr_T	lnum;
    صحيح		پورا;
    صحيح		i = 0;
    صحيح		use_cache;
#جے_تعریف_شدہ FEAT_FOLDING
    linenr_T    آخری;
    صحيح		folded;
#جے_انت

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
#جے_تعریف_شدہ FEAT_DIFF
		    && !wp->w_p_diff
#جے_انت
		    && wp->w_سطریں_valid > 0
		    && wp->w_سطریں[0].wl_lnum <= wp->w_topline;

    إذا (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	پورا = wp->w_cline_صف;
    }
    وإلا
    {
	lnum = wp->w_topline;
	پورا = 0;
    }

    // Find the w_lines[] entry for the starting line.
    إذا (use_cache)
	طالما (i < wp->w_سطریں_valid && wp->w_سطریں[i].wl_lnum < lnum)
	    ++i;

    لكل ( ; lnum <= wp->w_بفر->b_ml.ml_سطر_گنتی; ++i)
    {
	صحيح	valid = FALSE;

#جے_تعریف_شدہ FEAT_FOLDING
	آخری = lnum;
	folded = FALSE;
#جے_انت
	// Try to use the size from the previous redraw.
	إذا (use_cache && i < wp->w_سطریں_valid)
	{
	    إذا (wp->w_سطریں[i].wl_lnum < lnum || !wp->w_سطریں[i].wl_valid)
		استمر;		// skip changed or deleted lines
	    إذا (wp->w_سطریں[i].wl_lnum == lnum)
		valid = TRUE;
	    وإلا // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	إذا (valid)
	{
	    n = wp->w_سطریں[i].wl_ناپ;
#جے_تعریف_شدہ FEAT_FOLDING
	    folded = wp->w_سطریں[i].wl_folded;
	    آخری = wp->w_سطریں[i].wl_lastlnum;
#جے_انت
	}
	وإلا
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    إذا (hasFoldingWin(wp, lnum, عدم, &آخری, TRUE, عدم))
	    {
		n = 1;
		folded = TRUE;
	    }
	    وإلا
#جے_انت
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	إذا (
#جے_تعریف_شدہ FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && آخری >= wp->w_cursor.lnum
#وإلا
		lnum == wp->w_cursor.lnum
#جے_انت
	   )
	{
	    wp->w_cline_صف = پورا;
	    wp->w_cline_اچائی = n;
#جے_تعریف_شدہ FEAT_FOLDING
	    wp->w_cline_folded = folded;
#جے_انت
	    redraw_for_cursorline(wp);
	    wp->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	إذا (پورا + n > wp->w_اچائی)
	    اكسر;
	پورا += n;
#جے_تعریف_شدہ FEAT_FOLDING
	lnum = آخری + 1;
#وإلا
	++lnum;
#جے_انت
    }

    // wp->w_botline is the line that is just below the window
    wp->w_botline = lnum;
    wp->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    سیٹ_خالی_تھاواں_صفاں(wp, پورا);
}

/*
 * Redraw when w_cline_row changes and 'relativenumber' or 'cursorline' is
 * set.
 */
    ساكن فراغ
redraw_for_cursorline(win_T *wp)
{
    إذا ((wp->w_p_rnu
#جے_تعریف_شدہ FEAT_SYN_HL
		|| wp->w_p_cul
#جے_انت
		)
	    && (wp->w_valid & VALID_CROW) == 0
	    && !pum_visible())
    {
	// win_line() will redraw the number column and cursorline only.
	redraw_win_later(wp, UPD_VALID);
    }
}

#جے_تعریف_شدہ FEAT_SYN_HL
/*
 * Redraw when w_virtcol changes and 'cursorcolumn' is set or 'cursorlineopt'
 * contains "screenline".
 */
    ساكن فراغ
redraw_for_cursorcolumn(win_T *wp)
{
    إذا ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	إذا (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	وإلا إذا (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#جے_انت

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    فراغ
سیٹ_valid_virtcol(win_T *wp, colnr_T vcol)
{
    wp->w_virtcol = vcol;
#جے_تعریف_شدہ FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#جے_انت
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
    صحيح
sms_marker_overlap(win_T *wp, صحيح extra2)
{
    إذا (extra2 == -1)
	extra2 = win_خانہ_off(wp) - win_خانہ_off2(wp);
#إذا defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    إذا (*get_showbreak_قدر(wp) != NUL)
	أرجع 0;
#جے_انت
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    إذا (wp->w_p_list && wp->w_lcs_chars.prec)
	أرجع 1;

    أرجع extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    ساكن صحيح
skipcol_from_plines(win_T *wp, صحيح plines_off)
{
    صحيح width1 = wp->w_چوڑائی - win_خانہ_off(wp);

    صحيح skipcol = 0;
    إذا (plines_off > 0)
	skipcol += width1;
    إذا (plines_off > 1)
	skipcol += (width1 + win_خانہ_off2(wp)) * (plines_off - 1);
    أرجع skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    ساكن فراغ
reset_skipcol(فراغ)
{
    إذا (curwin->w_skipcol == 0)
	أرجع;

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
    فراغ
update_topline_redraw(فراغ)
{
    update_topline();
    إذا (must_redraw)
	update_screen(0);
}

/*
 * Return true when 'scrolloffpad' may augment 'scrolloff'.
 * This only applies to automatic cursor visibility correction.
 * For now 'scrolloffpad' is treated as boolean: 0 disables, > 0 enables.
 */
    ساكن bool
use_scrolloffpad(فراغ)
{
    أرجع get_scrolloff_قدر() > 0 && get_scrolloffpad_قدر() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    ساكن bool
scrolloffpad_eof_pressure(linenr_T lnum, لما so)
{
    إذا (!use_scrolloffpad() || so <= 0)
	أرجع false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    أرجع lnum > curbuf->b_ml.ml_سطر_گنتی - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    فراغ
update_topline(فراغ)
{
    لما	سطر_گنتی;
    صحيح		halfheight;
    صحيح		n;
#جے_تعریف_شدہ FEAT_FOLDING
    linenr_T	lnum;
#جے_انت
    صحيح		check_topline = FALSE;
    صحيح		check_botline = FALSE;
    لما	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    صحيح		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    إذا (skip_update_topline)
	أرجع;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    إذا (!screen_valid(TRUE) || curwin->w_اچائی == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	أرجع;
    }

    check_cursor_moved(curwin);
    إذا (curwin->w_valid & VALID_TOPLINE)
	أرجع;

    // When dragging with the mouse, don't scroll that quickly
    إذا (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح old_topfill = curwin->w_topfill;
#جے_انت

    /*
     * If the buffer is empty, always set topline to 1.
     */
    إذا (BUFEMPTY())		// special case - file is empty
    {
	إذا (curwin->w_topline != 1)
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
    وإلا
    {
	إذا (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    إذا (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    وإلا إذا (check_چوٹی_وتھ())
		check_topline = TRUE;
	    وإلا إذا (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		صحيح overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, عدم, عدم, 0);
		overlap = sms_marker_overlap(curwin, -1);
		إذا (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#جے_تعریف_شدہ FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	إذا (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#جے_انت

	إذا (check_topline)
	{
	    halfheight = curwin->w_اچائی / 2 - 1;
	    إذا (halfheight < 2)
		halfheight = 2;

#جے_تعریف_شدہ FEAT_FOLDING
	    إذا (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		لكل (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    إذا (lnum >= curbuf->b_ml.ml_سطر_گنتی || n >= halfheight)
			اكسر;
		    (فراغ)hasFolding(lnum, عدم, &lnum);
		}
	    }
	    وإلا
#جے_انت
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    صحيح min_scroll = scrolljump_قدر();
	    إذا (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    وإلا إذا (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    وإلا
	    {
		scroll_cursor_چوٹی(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	وإلا
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#جے_انت
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
    إذا (check_botline)
    {
	إذا (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	إذا (curwin->w_botline <= curbuf->b_ml.ml_سطر_گنتی || use_scrolloffpad())
	{
	    إذا (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		إذا (((لما)curwin->w_cursor.lnum
					   >= (لما)curwin->w_botline - *so_ptr
#جے_تعریف_شدہ FEAT_FOLDING
			|| hasAnyFolding(curwin)
#جے_انت
			))
		{
		    lineoff_T	loff;

		    // Cursor is (a few lines) above botline, check if there
		    // are 'scrolloff' window lines below the cursor.
		    // If not, need to scroll.
		    n = eof_pressure ? 0 : curwin->w_خالی_تھاواں_صفاں;
		    loff.lnum = curwin->w_cursor.lnum;
#جے_تعریف_شدہ FEAT_FOLDING
		    // In a fold go to its last line.
		    (فراغ)hasFolding(loff.lnum, عدم, &loff.lnum);
#جے_انت
#جے_تعریف_شدہ FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_صفاں;
#جے_انت
		    loff.اچائی = 0;
		    طالما (loff.lnum < curwin->w_botline
#جے_تعریف_شدہ FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#جے_انت
			    )
		    {
			n += loff.اچائی;
			إذا (n >= *so_ptr)
			    اكسر;
			botline_forw(&loff);
		    }
		    إذا (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		وإلا
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    إذا (check_botline)
	    {
#جے_تعریف_شدہ FEAT_FOLDING
		إذا (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    سطر_گنتی = 0;
		    لكل (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++سطر_گنتی;
			// stop at end of file or when we know we are far off
			إذا (lnum <= 0 || سطر_گنتی > curwin->w_اچائی + 1)
			    اكسر;
			(فراغ)hasFolding(lnum, &lnum, عدم);
		    }
		}
		وإلا
#جے_انت
		    سطر_گنتی = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		إذا (سطر_گنتی <= curwin->w_اچائی + 1)
		{
		    إذا (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    وإلا
			scroll_cursor_bot(scrolljump_قدر(), FALSE);
		}
		وإلا
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    إذا (curwin->w_topline != old_topline
#جے_تعریف_شدہ FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#جے_انت
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	إذا (!curwin->w_p_sms)
	    reset_skipcol();
	وإلا إذا (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	إذا (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    ساكن صحيح
scrolljump_قدر(فراغ)
{
    إذا (p_sj >= 0)
	أرجع (صحيح)p_sj;
    أرجع (curwin->w_اچائی * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    ساكن صحيح
check_چوٹی_وتھ(فراغ)
{
    lineoff_T	loff;
    صحيح		n;
    لما	so = get_scrolloff_قدر();

    إذا (curwin->w_cursor.lnum < curwin->w_topline + so
#جے_تعریف_شدہ FEAT_FOLDING
		    || hasAnyFolding(curwin)
#جے_انت
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#جے_تعریف_شدہ FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#وإلا
	n = 0;
#جے_انت
	// Count the visible screen lines above the cursor line.
	طالما (n < so)
	{
	    topline_پٹھ(&loff);
	    // Stop when included a line above the window.
	    إذا (loff.lnum < curwin->w_topline
#جے_تعریف_شدہ FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#جے_انت
		    )
		اكسر;
	    n += loff.اچائی;
	}
	إذا (n < so)
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Update w_curswant.
 */
    فراغ
update_curswant_force(فراغ)
{
    validate_virtcol();
    curwin->w_curswant = curwin->w_virtcol
#جے_تعریف_شدہ FEAT_PROP_POPUP
	- curwin->w_virtcol_پہلا_char
#جے_انت
	;
    curwin->w_سیٹ_curswant = false;
}

/*
 * Update w_curswant if w_set_curswant is set.
 */
    فراغ
update_curswant(فراغ)
{
    إذا (curwin->w_سیٹ_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    فراغ
check_cursor_moved(win_T *wp)
{
    إذا (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    وإلا إذا (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    وإلا إذا (wp->w_cursor.خانہ != wp->w_valid_cursor.خانہ
	     || wp->w_leftcol != wp->w_valid_leftcol
	     || wp->w_cursor.coladd != wp->w_valid_cursor.coladd)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
	wp->w_valid_cursor.خانہ = wp->w_cursor.خانہ;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_cursor.coladd = wp->w_cursor.coladd;
    }
}

/*
 * Call this function when some window settings have changed, which require
 * the cursor position, botline and topline to be recomputed and the window to
 * be redrawn.  E.g, when changing the 'wrap' option or folding.
 */
    فراغ
بدلیا_کھڑکی_setting(فراغ)
{
    بدلیا_کھڑکی_setting_win(curwin);
}

    فراغ
بدلیا_کھڑکی_setting_win(win_T *wp)
{
    wp->w_سطریں_valid = 0;
    بدلیا_سطر_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    فراغ
بدلیا_کھڑکی_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	إذا (wp->w_بفر == buf)
	    بدلیا_کھڑکی_setting_win(wp);
}
#جے_انت

/*
 * Call changed_window_setting_win() for every window.
 */
    فراغ
بدلیا_کھڑکی_setting_all(فراغ)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	بدلیا_کھڑکی_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    فراغ
سیٹ_topline(win_T *wp, linenr_T lnum)
{
#جے_تعریف_شدہ FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#جے_انت

#جے_تعریف_شدہ FEAT_FOLDING
    // go to first of folded lines
    (فراغ)hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
#جے_انت
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    إذا (wp->w_botline > wp->w_بفر->b_ml.ml_سطر_گنتی + 1)
	wp->w_botline = wp->w_بفر->b_ml.ml_سطر_گنتی + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_سیٹ = true;
#جے_تعریف_شدہ FEAT_DIFF
    إذا (lnum != prev_topline)
	// Keep the filler lines when the topline didn't change.
	wp->w_topfill = 0;
#جے_انت
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
    فراغ
بدلیا_cline_bef_curs(فراغ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    فراغ
بدلیا_cline_bef_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

/*
 * Call this function when the length of a line (in screen characters) above
 * the cursor have changed.
 * Need to take care of w_botline separately!
 */
    فراغ
بدلیا_سطر_abv_curs(فراغ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    فراغ
بدلیا_سطر_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    فراغ
بدلیا_سطر_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_بفر == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#جے_انت

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    فراغ
validate_botline(فراغ)
{
    validate_botline_win(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    فراغ
validate_botline_win(win_T *wp)
{
    إذا (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    فراغ
invalidate_botline(فراغ)
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    فراغ
invalidate_botline_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    فراغ
approximate_botline_win(
    win_T	*wp)
{
    wp->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    صحيح
cursor_valid(فراغ)
{
    check_cursor_moved(curwin);
    أرجع ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    فراغ
validate_cursor(فراغ)
{
    check_cursor_lnum();
    check_cursor_moved(curwin);
    إذا ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#إذا defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    فراغ
validate_cline_صف(فراغ)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    إذا (!(curwin->w_valid & VALID_CROW))
	curs_صفاں(curwin);
}
#جے_انت

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    ساكن فراغ
curs_صفاں(win_T *wp)
{
    linenr_T	lnum;
    صحيح		i;
    صحيح		all_invalid;
    صحيح		valid;
#جے_تعریف_شدہ FEAT_FOLDING
    لما	fold_گنتی;
#جے_انت

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_سطریں_valid == 0
			|| wp->w_سطریں[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_صف = 0;
    لكل (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	إذا (!all_invalid && i < wp->w_سطریں_valid)
	{
	    إذا (wp->w_سطریں[i].wl_lnum < lnum || !wp->w_سطریں[i].wl_valid)
		استمر;		// skip changed or deleted lines
	    إذا (wp->w_سطریں[i].wl_lnum == lnum)
	    {
#جے_تعریف_شدہ FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		إذا (!wp->w_بفر->b_mod_سیٹ
			|| wp->w_سطریں[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_بفر->b_mod_چوٹی
					     > wp->w_سطریں[i].wl_lastlnum + 1)
#جے_انت
		valid = TRUE;
	    }
	    وإلا إذا (wp->w_سطریں[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	إذا (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#جے_تعریف_شدہ FEAT_DIFF
					    && !wp->w_p_diff
#جے_انت
	       )))
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    lnum = wp->w_سطریں[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    إذا (lnum > wp->w_cursor.lnum)
		اكسر;
#وإلا
	    ++lnum;
#جے_انت
	    wp->w_cline_صف += wp->w_سطریں[i].wl_ناپ;
	}
	وإلا
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    fold_گنتی = folded_گنتی(wp, lnum, عدم);
	    إذا (fold_گنتی)
	    {
		lnum += fold_گنتی;
		إذا (lnum > wp->w_cursor.lnum)
		    اكسر;
		++wp->w_cline_صف;
	    }
	    وإلا
#جے_انت
	    {
		wp->w_cline_صف += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    إذا (!(wp->w_valid & VALID_CHEIGHT))
    {
	إذا (all_invalid
		|| i == wp->w_سطریں_valid
		|| (i < wp->w_سطریں_valid
		    && (!wp->w_سطریں[i].wl_valid
			|| wp->w_سطریں[i].wl_lnum != wp->w_cursor.lnum)))
	{
#جے_تعریف_شدہ FEAT_DIFF
	    إذا (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_اچائی = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    وإلا
#جے_انت
		wp->w_cline_اچائی = plines_win(wp, wp->w_cursor.lnum, TRUE);
#جے_تعریف_شدہ FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      عدم, عدم, TRUE, عدم);
#جے_انت
	}
	وإلا إذا (i > wp->w_سطریں_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_اچائی = 0;
#جے_تعریف_شدہ FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      عدم, عدم, TRUE, عدم);
#جے_انت
	}
	وإلا
	{
	    wp->w_cline_اچائی = wp->w_سطریں[i].wl_ناپ;
#جے_تعریف_شدہ FEAT_FOLDING
	    wp->w_cline_folded = wp->w_سطریں[i].wl_folded;
#جے_انت
	}
    }

    redraw_for_cursorline(curwin);
    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;
}

/*
 * Validate curwin->w_virtcol only.
 */
    فراغ
validate_virtcol(فراغ)
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    فراغ
validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);

    إذا (wp->w_valid & VALID_VIRTCOL)
	أرجع;

#جے_تعریف_شدہ FEAT_PROP_POPUP
    wp->w_virtcol_پہلا_char = 0;
#جے_انت
    getvvcol(wp, &wp->w_cursor, عدم, &(wp->w_virtcol), عدم,
							 GETVCOL_FOR_VIRTCOL);
#جے_تعریف_شدہ FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#جے_انت
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    فراغ
validate_cheight(فراغ)
{
    check_cursor_moved(curwin);

    إذا (curwin->w_valid & VALID_CHEIGHT)
	أرجع;

#جے_تعریف_شدہ FEAT_DIFF
    إذا (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_اچائی = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    وإلا
#جے_انت
	curwin->w_cline_اچائی = plines(curwin->w_cursor.lnum);
#جے_تعریف_شدہ FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, عدم, عدم);
#جے_انت
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    فراغ
validate_cursor_خانہ(فراغ)
{
    colnr_T off;
    colnr_T خانہ;
    صحيح     چوڑائی;

    validate_virtcol();

    إذا (curwin->w_valid & VALID_WCOL)
	أرجع;

    خانہ = curwin->w_virtcol;
    off = curwin_خانہ_off();
    خانہ += off;
    چوڑائی = curwin->w_چوڑائی - off + curwin_خانہ_off2();

    // long line wrapping, adjust curwin->w_wrow
    إذا (curwin->w_p_wrap
	    && خانہ >= (colnr_T)curwin->w_چوڑائی
	    && چوڑائی > 0)
	// use same formula as what is used in curs_columns()
	خانہ -= ((خانہ - curwin->w_چوڑائی) / چوڑائی + 1) * چوڑائی;
    إذا (خانہ > (صحيح)curwin->w_leftcol)
	خانہ -= curwin->w_leftcol;
    وإلا
	خانہ = 0;
    curwin->w_wcol = خانہ;

    curwin->w_valid |= VALID_WCOL;
#جے_تعریف_شدہ FEAT_PROP_POPUP
    curwin->w_flags &= ~WFLAG_WCOL_OFF_ADDED;
#جے_انت
}

/*
 * Compute offset of a window, occupied by absolute or relative line number,
 * fold column and sign column (these don't move when scrolling horizontally).
 */
    صحيح
win_خانہ_off(win_T *wp)
{
    أرجع (((wp->w_p_nu || wp->w_p_rnu) ? نمبر_چوڑائی(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#جے_تعریف_شدہ FEAT_FOLDING
	    + wp->w_p_fdc
#جے_انت
#جے_تعریف_شدہ FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#جے_انت
	   );
}

    صحيح
curwin_خانہ_off(فراغ)
{
    أرجع win_خانہ_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    صحيح
win_خانہ_off2(win_T *wp)
{
    إذا ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != عدم)
	أرجع نمبر_چوڑائی(wp) + 1;
    أرجع 0;
}

    صحيح
curwin_خانہ_off2(فراغ)
{
    أرجع win_خانہ_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    فراغ
curs_columns(
    صحيح		may_scroll)	// when TRUE, may scroll horizontally
{
    لما	diff;
    صحيح		extra;		// offset for first screen line
    لما	off_بایاں, off_دایاں;
    صحيح		n;
    صحيح		p_سطریں;
    صحيح		width1;		// text width for first screen line
    صحيح		width2 = 0;	// text width for second and later screen line
    صحيح		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    لما	so = get_scrolloff_قدر();
    لما	siso = get_sidescrolloff_قدر();
    صحيح		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    إذا (!(curwin->w_valid & VALID_CROW))
	curs_صفاں(curwin);

#جے_تعریف_شدہ FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_پہلا_char = 0;
#جے_انت

    /*
     * Compute the number of virtual columns.
     */
#جے_تعریف_شدہ FEAT_FOLDING
    إذا (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    وإلا
#جے_انت
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    إذا (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_خانہ_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_صف;

    width1 = curwin->w_چوڑائی - extra;
    إذا (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_چوڑائی - 1;
	إذا (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_اچائی - 1;
	وإلا
	    curwin->w_wrow = curwin->w_اچائی - 1 - curwin->w_خالی_تھاواں_صفاں;
    }
    وإلا إذا (curwin->w_p_wrap && curwin->w_چوڑائی != 0)
    {
	width2 = width1 + curwin_خانہ_off2();

	// skip columns that are not visible
	إذا (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    إذا (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    وإلا
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	إذا (curwin->w_wcol >= curwin->w_چوڑائی)
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
    وإلا إذا (may_scroll
#جے_تعریف_شدہ FEAT_FOLDING
	    && !curwin->w_cline_folded
#جے_انت
	    )
    {
#جے_تعریف_شدہ FEAT_PROP_POPUP
	إذا (curwin->w_virtcol_پہلا_char > 0)
	{
	    صحيح خانے_گنتی = (curwin->w_چوڑائی - extra);
	    صحيح صفاں_2 = خانے_گنتی > 0 ? curwin->w_virtcol_پہلا_char / خانے_گنتی : 1;

	    // each "above" text prop shifts the text one row down
	    curwin->w_wrow += صفاں_2;
	    curwin->w_wcol -= صفاں_2 * خانے_گنتی;
	    endcol -= صفاں_2 * خانے_گنتی;
	    curwin->w_cline_اچائی = صفاں_2 + 1;
	}
#جے_انت
	/*
	 * If Cursor is left of the screen, scroll rightwards.
	 * If Cursor is right of the screen, scroll leftwards.
	 * If we get closer to the edge than 'sidescrolloff', scroll a little
	 * extra.
	 */
	off_بایاں = startcol - curwin->w_leftcol - siso;
	off_دایاں = endcol - curwin->w_leftcol - (curwin->w_چوڑائی - siso) + 1;
	إذا (off_بایاں < 0 || off_دایاں > 0)
	{
	    إذا (off_بایاں < 0)
		diff = -off_بایاں;
	    وإلا
		diff = off_دایاں;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    إذا (p_ss == 0 || diff >= width1 / 2 || off_دایاں >= off_بایاں)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    وإلا
	    {
		إذا (diff < p_ss)
		    diff = p_ss;
		إذا (off_بایاں < 0)
		    new_leftcol = curwin->w_leftcol - (صحيح)diff;
		وإلا
		    new_leftcol = curwin->w_leftcol + (صحيح)diff;
	    }
	    إذا (new_leftcol < 0)
		new_leftcol = 0;
	    إذا (new_leftcol != (صحيح)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    وإلا إذا (curwin->w_wcol > (صحيح)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    وإلا
	curwin->w_wcol = 0;

#جے_تعریف_شدہ FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    إذا (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    وإلا
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#جے_انت

    prev_skipcol = curwin->w_skipcol;

    p_سطریں = 0;

    إذا ((curwin->w_wrow >= curwin->w_اچائی
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_اچائی)
		    && (p_سطریں =
#جے_تعریف_شدہ FEAT_DIFF
			plines_win_nofill
#وإلا
			plines_win
#جے_انت
			(curwin, curwin->w_cursor.lnum, FALSE))
						    - 1 >= curwin->w_اچائی))
	    && curwin->w_اچائی != 0
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
	إذا (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	إذا (p_سطریں == 0)
	    p_سطریں = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_سطریں;
	إذا (p_سطریں > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	وإلا
	    n = p_سطریں;
	إذا ((colnr_T)n >= curwin->w_اچائی + curwin->w_skipcol / width2 - so)
	    extra += 2;

	إذا (extra == 3 || curwin->w_اچائی <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    إذا (n > curwin->w_اچائی / 2)
		n -= curwin->w_اچائی / 2;
	    وإلا
		n = 0;
	    // don't skip more than necessary
	    إذا (n > p_سطریں - curwin->w_اچائی + 1)
		n = p_سطریں - curwin->w_اچائی + 1;
	    إذا (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    وإلا
		curwin->w_skipcol = 0;
	}
	وإلا إذا (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    إذا (extra > 0)
	    {
		إذا ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	وإلا إذا (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_اچائی + 1) * width2;
	    طالما (endcol > curwin->w_virtcol)
		endcol -= width2;
	    إذا (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	إذا (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - prev_skipcol) / width2;
	وإلا
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	إذا (curwin->w_wrow >= curwin->w_اچائی)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_اچائی + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((صحيح)prev_skipcol - (صحيح)curwin->w_skipcol) / width2;
	إذا (extra > 0)
	    win_ins_سطریں(curwin, 0, extra, FALSE, FALSE);
	وإلا إذا (extra < 0)
	    win_del_سطریں(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    وإلا إذا (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    إذا (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#جے_تعریف_شدہ FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#جے_انت
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    إذا (popup_is_popup(curwin) && curbuf->b_رکن != عدم)
    {
	curwin->w_wrow += popup_چوٹی_extra(curwin);
	curwin->w_wcol += popup_بایاں_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    وإلا
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#جے_انت

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#إذا defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    فراغ
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	صحيح	*rowp,	// screen row
	صحيح	*scolp,	// start screen column
	صحيح	*ccolp,	// cursor screen column
	صحيح	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    صحيح		صف = 0;
    colnr_T	coloff = 0;

    إذا (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    خانہ;
	صحيح	    چوڑائی;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	صحيح	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
# endif
	صف = plines_m_win(wp, wp->w_topline, lnum - 1, ودھ_توں_ودھ_عدد);
	// "row" should be the screen line where line "lnum" begins, which can
	// be negative if "lnum" is "w_topline" and "w_skipcol" is non-zero.
	صف -= adjust_plines_for_skipcol(wp);

# ifdef FEAT_DIFF
	// Add filler lines above this buffer line.
	صف += lnum == wp->w_topline ? wp->w_topfill
				     : diff_check_fill(wp, lnum);
# endif

	colnr_T	off = win_خانہ_off(wp);
# ifdef FEAT_FOLDING
	إذا (is_folded)
	{
	    صف += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	وإلا
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    خانہ = scol;
	    خانہ += off;
	    چوڑائی = wp->w_چوڑائی - off + win_خانہ_off2(wp);

	    // long line wrapping, adjust row
	    إذا (wp->w_p_wrap
		    && خانہ >= (colnr_T)wp->w_چوڑائی
		    && چوڑائی > 0)
	    {
		// use same formula as what is used in curs_columns()
		صحيح rowoff = ((خانہ - wp->w_چوڑائی) / چوڑائی + 1);
		خانہ -= rowoff * چوڑائی;
		صف += rowoff;
	    }
	    خانہ -= wp->w_leftcol;
	    إذا (خانہ >= wp->w_چوڑائی)
		خانہ = -1;
	    إذا (خانہ >= 0 && صف >= 0 && صف < wp->w_اچائی)
	    {
		coloff = خانہ - scol + wp->w_wincol + 1;
		صف += W_WINROW(wp) + 1;
	    }
	    وإلا
		// character is out of the window
		صف = scol = ccol = ecol = 0;
	}
    }
    *rowp = صف;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    إذا (wp->w_p_rl && صف > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	صحيح endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_چوڑائی - endoff;
    }
# endif
}
#جے_انت

#إذا defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    فراغ
f_screenpos(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    صحيح		صف = 0;
    صحيح		scol = 0, ccol = 0, ecol = 0;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    dict = rettv->vval.v_dict;

    إذا (وچ_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_نمبر_arg(argvars, 2) == FAIL))
	أرجع;

    wp = لبھو_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	أرجع;

    pos.lnum = tv_get_نمبر(&argvars[1]);
    إذا (pos.lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
    {
	semsg(_(e_invalid_سطر_نمبر_nr), pos.lnum);
	أرجع;
    }
    pos.خانہ = tv_get_نمبر(&argvars[2]) - 1;
    إذا (pos.خانہ < 0)
	pos.خانہ = 0;
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
    ساكن صحيح
virtcol2col(win_T *wp, linenr_T lnum, صحيح vcol)
{
    صحيح		وتھ = vcol2col(wp, lnum, vcol - 1, عدم);
    char_u	*سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);
    char_u	*p = سطر + وتھ;

    إذا (*p == NUL)
    {
	إذا (p == سطر)  // empty line
	    أرجع 0;
	// Move to the first byte of the last char.
	MB_PTR_پٹھ(سطر, p);
    }
    أرجع (صحيح)(p - سطر + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    فراغ
f_virtcol2col(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    صحيح		screencol;
    صحيح		error = FALSE;

    rettv->vval.v_نمبر = -1;

    إذا (check_for_نمبر_arg(argvars, 0) == FAIL
	    || check_for_نمبر_arg(argvars, 1) == FAIL
	    || check_for_نمبر_arg(argvars, 2) == FAIL)
	أرجع;

    wp = لبھو_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	أرجع;

    lnum = tv_get_نمبر_chk(&argvars[1], &error);
    إذا (error || lnum < 0 || lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
	أرجع;

    screencol = tv_get_نمبر_chk(&argvars[2], &error);
    إذا (error || screencol < 0)
	أرجع;

    rettv->vval.v_نمبر = virtcol2col(wp, lnum, screencol);
}
#جے_انت

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
ساكن فراغ cursor_correct_sms(فراغ)
{
    إذا (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	أرجع;

    لما    so = get_scrolloff_قدر();
    صحيح	    width1 = curwin->w_چوڑائی - curwin_خانہ_off();
    صحيح	    width2 = width1 + curwin_خانہ_off2();
    صحيح	    so_خانے_گنتی = so == 0 ? 0 : width1 + (so - 1) * width2;
    صحيح	    space_خانے_گنتی = (curwin->w_اچائی - 1) * width2;
    صحيح	    overlap, چوٹی, bot;
    صحيح	    ناپ = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    إذا (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_خانے_گنتی = 0;               // Ignore 'scrolloff' at top of buffer.
    وإلا إذا (so_خانے_گنتی > space_خانے_گنتی / 2)
	so_خانے_گنتی = space_خانے_گنتی / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    طالما (so_خانے_گنتی > ناپ && so_خانے_گنتی - width2 >= width1 && width1 > 0)
	so_خانے_گنتی -= width2;
    إذا (so_خانے_گنتی >= width1 && so_خانے_گنتی > ناپ)
	so_خانے_گنتی -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_چوڑائی - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    چوٹی = curwin->w_skipcol + (so_خانے_گنتی != 0 ? so_خانے_گنتی : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_اچائی - 1) * width2
								    - so_خانے_گنتی;
    validate_virtcol();
    colnr_T خانہ = curwin->w_virtcol;

    إذا (خانہ < چوٹی)
    {
	إذا (خانہ < width1)
	    خانہ += width1;
	طالما (width2 > 0 && خانہ < چوٹی)
	    خانہ += width2;
    }
    وإلا
	طالما (width2 > 0 && خانہ >= bot)
	    خانہ -= width2;

    إذا (خانہ != curwin->w_virtcol)
    {
	صحيح rc;

	curwin->w_curswant = خانہ;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	إذا (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی)
	{
	    validate_virtcol();
	    إذا (curwin->w_virtcol < curwin->w_skipcol + overlap)
	    {
		// Cursor still not visible: move it to the next line instead.
		curwin->w_cursor.lnum++;
		curwin->w_cursor.خانہ = 0;
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
    فراغ
scroll_redraw(صحيح اتے, لما گنتی)
{
    linenr_T	prev_topline = curwin->w_topline;
    صحيح		prev_skipcol = curwin->w_skipcol;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		prev_topfill = curwin->w_topfill;
#جے_انت
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    إذا (اتے)
	scrollup(گنتی, TRUE);
    وإلا
	scrolldown(گنتی, TRUE);
    إذا (get_scrolloff_قدر() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	طالما (curwin->w_topline == prev_topline
		&& curwin->w_skipcol == prev_skipcol
#جے_تعریف_شدہ FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#جے_انت
		)
	{
	    إذا (اتے)
	    {
		إذا (curwin->w_cursor.lnum > prev_lnum
			|| cursor_تھلے(1L, FALSE) == FAIL)
		    اكسر;
	    }
	    وإلا
	    {
		إذا (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_اتے(1L, FALSE) == FAIL)
		    اكسر;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    إذا (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    فراغ
scrolldown(
    لما	سطر_گنتی,
    صحيح		byfold بن_ورتیا)	// TRUE: count a closed fold as one line
{
    لما	پورا = 0;	// total # of physical lines done
    صحيح		wrow;
    صحيح		moved = FALSE;
    صحيح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    صحيح		width1 = 0;
    صحيح		width2 = 0;

    إذا (do_sms)
    {
	width1 = curwin->w_چوڑائی - curwin_خانہ_off();
	width2 = width1 + curwin_خانہ_off2();
    }

#جے_تعریف_شدہ FEAT_FOLDING
    linenr_T	پہلا;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#جے_انت
    validate_cursor();		// w_wrow needs to be valid
    لكل (صحيح todo = سطر_گنتی; todo > 0; --todo)
    {
#جے_تعریف_شدہ FEAT_DIFF
	إذا (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_اچائی - 1)
	{
	    ++curwin->w_topfill;
	    ++پورا;
	}
	وإلا
#جے_انت
	{
	    // break when at the very top
	    إذا (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		اكسر;
	    إذا (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		إذا (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		وإلا
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++پورا;
	    }
	    وإلا
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#جے_تعریف_شدہ FEAT_DIFF
		curwin->w_topfill = 0;
#جے_انت
#جے_تعریف_شدہ FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		إذا (hasFolding(curwin->w_topline, &پہلا, عدم))
		{
		    ++پورا;
		    إذا (!byfold)
			todo -= curwin->w_topline - پہلا - 1;
		    curwin->w_botline -= curwin->w_topline - پہلا;
		    curwin->w_topline = پہلا;
		}
		وإلا
#جے_انت
		إذا (do_sms)
		{
		    صحيح ناپ = linetabsize_eol(curwin, curwin->w_topline);
		    إذا (ناپ > width1)
		    {
			curwin->w_skipcol = width1;
			ناپ -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    طالما (ناپ > width2)
		    {
			curwin->w_skipcol += width2;
			ناپ -= width2;
		    }
		    ++پورا;
		}
		وإلا
		    پورا += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += پورا;		// keep w_wrow updated
    curwin->w_cline_صف += پورا;	// keep w_cline_row updated

#جے_تعریف_شدہ FEAT_DIFF
    إذا (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_صف = 0;
    check_topfill(curwin, TRUE);
#جے_انت

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    إذا (curwin->w_p_wrap && curwin->w_چوڑائی != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_اچائی - 1 -
	    curwin->w_virtcol / curwin->w_چوڑائی;
    }
    طالما (wrow >= curwin->w_اچائی && curwin->w_cursor.lnum > 1)
    {
#جے_تعریف_شدہ FEAT_FOLDING
	إذا (hasFolding(curwin->w_cursor.lnum, &پہلا, عدم))
	{
	    --wrow;
	    إذا (پہلا == 1)
		curwin->w_cursor.lnum = 1;
	    وإلا
		curwin->w_cursor.lnum = پہلا - 1;
	}
	وإلا
#جے_انت
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    إذا (moved)
    {
#جے_تعریف_شدہ FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#جے_انت
	coladvance(curwin->w_curswant);
    }
    إذا (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    فراغ
scrollup(
    لما	سطر_گنتی,
    صحيح		byfold بن_ورتیا)	// TRUE: count a closed fold as one line
{
    صحيح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    إذا (do_sms
#جے_تعریف_شدہ FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#جے_انت
#جے_تعریف_شدہ FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#جے_انت
       )
    {
	صحيح	    width1 = curwin->w_چوڑائی - curwin_خانہ_off();
	صحيح	    width2 = width1 + curwin_خانہ_off2();
	صحيح	    ناپ = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	إذا (do_sms)
	    ناپ = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	لكل (صحيح todo = سطر_گنتی; todo > 0; --todo)
	{
#جے_تعریف_شدہ FEAT_DIFF
	    إذا (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    وإلا
#جے_انت
	    {
		linenr_T lnum = curwin->w_topline;

#جے_تعریف_شدہ FEAT_FOLDING
		إذا (byfold)
		    // for a closed fold: go to the last line in the fold
		    (فراغ)hasFolding(lnum, عدم, &lnum);
#جے_انت
		إذا (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    صحيح جوڑو = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += جوڑو;
		    إذا (curwin->w_skipcol >= ناپ)
		    {
			إذا (lnum == curbuf->b_ml.ml_سطر_گنتی)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= جوڑو;
			    اكسر;
			}
			++lnum;
		    }
		}
		وإلا
		{
		    إذا (lnum >= curbuf->b_ml.ml_سطر_گنتی)
			اكسر;
		    ++lnum;
		}

		إذا (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#جے_تعریف_شدہ FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#جے_انت
		    curwin->w_skipcol = 0;
		    إذا (todo > 1 && do_sms)
			ناپ = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	إذا (prev_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    وإلا
    {
	curwin->w_topline += سطر_گنتی;
	curwin->w_botline += سطر_گنتی;	// approximate w_botline
    }

    إذا (curwin->w_topline > curbuf->b_ml.ml_سطر_گنتی)
	curwin->w_topline = curbuf->b_ml.ml_سطر_گنتی;
    إذا (curwin->w_botline > curbuf->b_ml.ml_سطر_گنتی + 1)
	curwin->w_botline = curbuf->b_ml.ml_سطر_گنتی + 1;

#جے_تعریف_شدہ FEAT_DIFF
    check_topfill(curwin, FALSE);
#جے_انت

#جے_تعریف_شدہ FEAT_FOLDING
    إذا (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#جے_انت

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    إذا (curwin->w_cursor.lnum < curwin->w_topline)
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
    فراغ
adjust_skipcol(فراغ)
{
    إذا (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	أرجع;

    صحيح	    width1 = curwin->w_چوڑائی - curwin_خانہ_off();
    إذا (width1 <= 0)
	أرجع;  // no text will be displayed

    صحيح	    width2 = width1 + curwin_خانہ_off2();
    لما    so = get_scrolloff_قدر();
    صحيح	    scrolloff_خانے_گنتی = so == 0 ? 0 : width1 + (so - 1) * width2;
    صحيح	    scrolled = FALSE;
    صحيح	    صف = 0;
    صحيح	    overlap, خانہ;

    validate_cheight();
    إذا (curwin->w_cline_اچائی == curwin->w_اچائی
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_اچائی)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	أرجع;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_چوڑائی - width2);
    طالما (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_خانے_گنتی)
    {
	// scroll a screen line down
	إذا (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	وإلا
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    إذا (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	أرجع;  // don't scroll in the other direction now
    }

    خانہ = curwin->w_virtcol + scrolloff_خانے_گنتی;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    إذا (scrolloff_خانے_گنتی > 0)
    {
	صحيح ناپ = linetabsize_eol(curwin, curwin->w_topline);
	ناپ = width1 + width2 * ((ناپ - width1 + width2 - 1) / width2);
	طالما (خانہ > ناپ)
	    خانہ -= width2;
    }
    خانہ -= curwin->w_skipcol;

    إذا (خانہ >= width1)
    {
	خانہ -= width1;
	++صف;
    }
    إذا (خانہ > width2)
    {
	صف += خانہ / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	خانہ = خانہ % width2;
    }
    إذا (صف >= curwin->w_اچائی)
    {
	إذا (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --صف;
	}
	إذا (صف >= curwin->w_اچائی)
	    curwin->w_skipcol += (صف - curwin->w_اچائی) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#جے_تعریف_شدہ FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    فراغ
check_topfill(
    win_T	*wp,
    صحيح		تھلے)	// when TRUE scroll down when not enough space
{
    صحيح		n;

    إذا (wp->w_topfill <= 0)
	أرجع;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    إذا (wp->w_topfill + n > wp->w_اچائی)
    {
	إذا (تھلے && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	وإلا
	{
	    wp->w_topfill = wp->w_اچائی - n;
	    إذا (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#جے_انت

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    فراغ
scrolldown_clamp(فراغ)
{
    صحيح		سرا_صف;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#جے_انت

    إذا (curwin->w_topline <= 1
#جے_تعریف_شدہ FEAT_DIFF
	    && !can_fill
#جے_انت
	    )
	أرجع;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    سرا_صف = curwin->w_wrow;
#جے_تعریف_شدہ FEAT_DIFF
    إذا (can_fill)
	++سرا_صف;
    وإلا
	سرا_صف += plines_nofill(curwin->w_topline - 1);
#وإلا
    سرا_صف += plines(curwin->w_topline - 1);
#جے_انت
    إذا (curwin->w_p_wrap && curwin->w_چوڑائی != 0)
    {
	validate_cheight();
	validate_virtcol();
	سرا_صف += curwin->w_cline_اچائی - 1 -
	    curwin->w_virtcol / curwin->w_چوڑائی;
    }
    إذا (سرا_صف < curwin->w_اچائی - get_scrolloff_قدر())
    {
#جے_تعریف_شدہ FEAT_DIFF
	إذا (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	وإلا
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#وإلا
	--curwin->w_topline;
#جے_انت
#جے_تعریف_شدہ FEAT_FOLDING
	(فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#جے_انت
	--curwin->w_botline;	    // approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    فراغ
scrollup_clamp(فراغ)
{
    صحيح	    شروع_صف;

    إذا (curwin->w_topline == curbuf->b_ml.ml_سطر_گنتی
#جے_تعریف_شدہ FEAT_DIFF
	    && curwin->w_topfill == 0
#جے_انت
	    )
	أرجع;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#جے_تعریف_شدہ FEAT_DIFF
    شروع_صف = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#وإلا
    شروع_صف = curwin->w_wrow - plines(curwin->w_topline);
#جے_انت
    إذا (curwin->w_p_wrap && curwin->w_چوڑائی != 0)
    {
	validate_virtcol();
	شروع_صف -= curwin->w_virtcol / curwin->w_چوڑائی;
    }
    إذا (شروع_صف >= get_scrolloff_قدر())
    {
#جے_تعریف_شدہ FEAT_DIFF
	إذا (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	وإلا
#جے_انت
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    (فراغ)hasFolding(curwin->w_topline, عدم, &curwin->w_topline);
#جے_انت
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
    ساكن فراغ
topline_پٹھ_winheight(
    lineoff_T	*lp,
    صحيح		winheight)	// when TRUE limit to window height
{
#جے_تعریف_شدہ FEAT_DIFF
    إذا (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->اچائی = 1;
    }
    وإلا
#جے_انت
    {
	--lp->lnum;
#جے_تعریف_شدہ FEAT_DIFF
	lp->fill = 0;
#جے_انت
	إذا (lp->lnum < 1)
	    lp->اچائی = MAXCOL;
	وإلا
#جے_تعریف_شدہ FEAT_FOLDING
	    إذا (hasFolding(lp->lnum, &lp->lnum, عدم))
	    // Add a closed fold
	    lp->اچائی = 1;
	وإلا
#جے_انت
	    lp->اچائی = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    ساكن فراغ
topline_پٹھ(lineoff_T *lp)
{
    topline_پٹھ_winheight(lp, TRUE);
}


/*
 * Add one line below "lp->lnum".  This can be a filler line, a closed fold or
 * a (wrapped) text line.  Uses and sets "lp->fill".
 * Returns the height of the added line in "lp->height".
 * Lines below the last one are incredibly high.
 */
    ساكن فراغ
botline_forw(lineoff_T *lp)
{
#جے_تعریف_شدہ FEAT_DIFF
    إذا (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->اچائی = 1;
    }
    وإلا
#جے_انت
    {
	++lp->lnum;
#جے_تعریف_شدہ FEAT_DIFF
	lp->fill = 0;
#جے_انت
	إذا (lp->lnum > curbuf->b_ml.ml_سطر_گنتی)
	    lp->اچائی = MAXCOL;
	وإلا
#جے_تعریف_شدہ FEAT_FOLDING
	    إذا (hasFolding(lp->lnum, عدم, &lp->lnum))
	    {
		// Add a closed fold.
		lp->اچائی = 1;
	    }
	    وإلا
#جے_انت
	    lp->اچائی = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    فراغ
scroll_cursor_چوٹی(صحيح min_scroll, صحيح always)
{
    صحيح		scrolled = 0;
    صحيح		extra = 0;
    صحيح		ورتیا;
    صحيح		i;
    linenr_T	چوٹی;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    صحيح		old_skipcol = curwin->w_skipcol;
#جے_تعریف_شدہ FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#جے_انت
    linenr_T	new_topline;
    صحيح		off = get_scrolloff_قدر();

    إذا (mouse_dragging > 0)
	off = mouse_dragging - 1;

    /*
     * Decrease topline until:
     * - it has become 1
     * - (part of) the cursor line is moved off the screen or
     * - moved at least 'scrolljump' lines and
     * - at least 'scrolloff' lines above and below the cursor
     */
    validate_cheight();
    ورتیا = curwin->w_cline_اچائی; // includes filler lines above
    إذا (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = ورتیا;

#جے_تعریف_شدہ FEAT_FOLDING
    إذا (hasFolding(curwin->w_cursor.lnum, &چوٹی, &bot))
    {
	--چوٹی;
	++bot;
    }
    وإلا
#جے_انت
    {
	چوٹی = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = چوٹی + 1;

#جے_تعریف_شدہ FEAT_DIFF
    // "used" already contains the number of filler lines above, don't add it
    // again.
    // Hide filler lines above cursor line by adding them to "extra".
    extra += diff_check_fill(curwin, curwin->w_cursor.lnum);
#جے_انت

    /*
     * Check if the lines from "top" to "bot" fit in the window.  If they do,
     * set new_topline and advance "top" and "bot" to include more lines.
     */
    طالما (چوٹی > 0)
    {
#جے_تعریف_شدہ FEAT_FOLDING
	إذا (hasFolding(چوٹی, &چوٹی, عدم))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	وإلا
#جے_انت
	    i = PLINES_NOFILL(چوٹی);
	إذا (چوٹی < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	إذا ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    اكسر;

	ورتیا += i;
	إذا (extra + i <= off && bot < curbuf->b_ml.ml_سطر_گنتی)
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    إذا (hasFolding(bot, عدم, &bot))
		// count one logical line for a sequence of folded lines
		++ورتیا;
	    وإلا
#جے_انت
		ورتیا += plines(bot);
	}
	إذا (ورتیا > curwin->w_اچائی)
	    اكسر;

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
    إذا (ورتیا > curwin->w_اچائی)
	scroll_cursor_halfway(FALSE, FALSE);
    وإلا
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	إذا (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	إذا (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#جے_تعریف_شدہ FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	إذا (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    إذا (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#جے_انت
	إذا (curwin->w_topline != old_topline)
	    reset_skipcol();
	وإلا إذا (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    إذا (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		صحيح	width1 = curwin->w_چوڑائی - curwin_خانہ_off();
		صحيح	width2 = width1 + curwin_خانہ_off2();
		صحيح	plines_off = 0;
		صحيح	skipcol;

		إذا (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		إذا (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	إذا (curwin->w_topline != old_topline
		|| curwin->w_skipcol != old_skipcol
#جے_تعریف_شدہ FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#جے_انت
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
    فراغ
سیٹ_خالی_تھاواں_صفاں(win_T *wp, صحيح ورتیا)
{
#جے_تعریف_شدہ FEAT_DIFF
    wp->w_filler_صفاں = 0;
#جے_انت
    إذا (ورتیا == 0)
	wp->w_خالی_تھاواں_صفاں = 0;	// single line that doesn't fit
    وإلا
    {
	wp->w_خالی_تھاواں_صفاں = wp->w_اچائی - ورتیا;
#جے_تعریف_شدہ FEAT_DIFF
	إذا (wp->w_botline <= wp->w_بفر->b_ml.ml_سطر_گنتی)
	{
	    wp->w_filler_صفاں = diff_check_fill(wp, wp->w_botline);
	    إذا (wp->w_خالی_تھاواں_صفاں > wp->w_filler_صفاں)
		wp->w_خالی_تھاواں_صفاں -= wp->w_filler_صفاں;
	    وإلا
	    {
		wp->w_filler_صفاں = wp->w_خالی_تھاواں_صفاں;
		wp->w_خالی_تھاواں_صفاں = 0;
	    }
	}
#جے_انت
    }
}

/*
 * Recompute topline to put the cursor at the bottom of the window.
 * When scrolling scroll at least "min_scroll" lines.
 * If "set_topbot" is TRUE, set topline and botline first (for "zb").
 * This is messy stuff!!!
 */
    فراغ
scroll_cursor_bot(صحيح min_scroll, صحيح سیٹ_topbot)
{
    صحيح		ورتیا;
    صحيح		scrolled = 0;
    صحيح		extra = 0;
    صحيح		i;
    linenr_T	سطر_گنتی;
    linenr_T	old_topline = curwin->w_topline;
    صحيح		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		old_topfill = curwin->w_topfill;
    صحيح		fill_ہیٹھاں_کھڑکی;
#جے_انت
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    صحيح		old_خالی_تھاواں_صفاں = curwin->w_خالی_تھاواں_صفاں;
    linenr_T	cln;		    // Cursor Line Number
    لما	so = get_scrolloff_قدر();
    صحيح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    إذا (سیٹ_topbot)
    {
	ورتیا = 0;
	linenr_T cln_آخری = cln;
#جے_تعریف_شدہ FEAT_FOLDING
	(فراغ)hasFolding(cln, عدم, &cln_آخری);
#جے_انت
	curwin->w_botline = cln_آخری + 1;
	loff.lnum = cln_آخری + 1;
#جے_تعریف_شدہ FEAT_DIFF
	loff.fill = 0;
#جے_انت
	طالما (TRUE)
	{
	    topline_پٹھ_winheight(&loff, FALSE);
	    إذا (loff.اچائی == MAXCOL)
		اكسر;
	    إذا (ورتیا + loff.اچائی > curwin->w_اچائی)
	    {
		إذا (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    إذا (ورتیا < curwin->w_اچائی)
		    {
			صحيح plines_وتھ = ورتیا + loff.اچائی
							    - curwin->w_اچائی;
			ورتیا = curwin->w_اچائی;
#جے_تعریف_شدہ FEAT_DIFF
			curwin->w_topfill = loff.fill;
#جے_انت
			curwin->w_topline = loff.lnum;
			curwin->w_skipcol = skipcol_from_plines(
							curwin, plines_وتھ);
		    }
		}
		اكسر;
	    }
#جے_تعریف_شدہ FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#جے_انت
	    curwin->w_topline = loff.lnum;
	    ورتیا += loff.اچائی;
	}

	سیٹ_خالی_تھاواں_صفاں(curwin, ورتیا);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	إذا (curwin->w_topline != old_topline
#جے_تعریف_شدہ FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#جے_انت
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    إذا (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    وإلا
		reset_skipcol();
	}
    }
    وإلا
	validate_botline();

    // The lines of the cursor line itself are always used.
#جے_تعریف_شدہ FEAT_DIFF
    ورتیا = plines_nofill(cln);
#وإلا
    validate_cheight();
    ورتیا = curwin->w_cline_اچائی;
#جے_انت

    إذا (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	صحيح upto_cursor = plines_win_خانہ(curwin, cln, curwin->w_cursor.خانہ);

	إذا (upto_cursor < ورتیا)
	    ورتیا = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    إذا (cln >= curwin->w_botline)
    {
	scrolled = ورتیا;
	إذا (cln == curwin->w_botline)
	    scrolled -= curwin->w_خالی_تھاواں_صفاں;
	إذا (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    صحيح چوٹی_plines =
#جے_تعریف_شدہ FEAT_DIFF
			    plines_win_nofill
#وإلا
			    plines_win
#جے_انت
					(curwin, curwin->w_topline, FALSE);
	    صحيح width1 = curwin->w_چوڑائی - curwin_خانہ_off();

	    إذا (width1 > 0)
	    {
		صحيح width2 = width1 + curwin_خانہ_off2();
		صحيح skip_سطریں = 0;

		// A similar formula is used in curs_columns().
		إذا (curwin->w_skipcol > width1)
		    skip_سطریں += (curwin->w_skipcol - width1) / width2 + 1;
		وإلا إذا (curwin->w_skipcol > 0)
		    skip_سطریں = 1;

		چوٹی_plines -= skip_سطریں;
		إذا (چوٹی_plines > curwin->w_اچائی)
		{
		    scrolled += (چوٹی_plines - curwin->w_اچائی);
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
#جے_تعریف_شدہ FEAT_FOLDING
    إذا (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
#جے_انت
    {
	loff.lnum = cln;
	boff.lnum = cln;
    }
#جے_تعریف_شدہ FEAT_DIFF
    loff.fill = 0;
    boff.fill = 0;
    fill_ہیٹھاں_کھڑکی = diff_check_fill(curwin, curwin->w_botline)
						      - curwin->w_filler_صفاں;
#جے_انت

    طالما (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	إذا ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_سطر_گنتی)
		&& loff.lnum <= curwin->w_botline
#جے_تعریف_شدہ FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_ہیٹھاں_کھڑکی)
#جے_انت
		)
	    اكسر;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_پٹھ(&loff);
	إذا (loff.اچائی == MAXCOL)
	    ورتیا = MAXCOL;
	وإلا
	    ورتیا += loff.اچائی;
	إذا (ورتیا > curwin->w_اچائی)
	    اكسر;
	إذا (loff.lnum >= curwin->w_botline
#جے_تعریف_شدہ FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_ہیٹھاں_کھڑکی)
#جے_انت
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.اچائی;
	    إذا (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_خالی_تھاواں_صفاں;
	}

	إذا (boff.lnum < curbuf->b_ml.ml_سطر_گنتی)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    ورتیا += boff.اچائی;
	    إذا (ورتیا > curwin->w_اچائی)
		اكسر;
	    إذا (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.اچائی;
		إذا (boff.lnum >= curwin->w_botline
#جے_تعریف_شدہ FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_صفاں)
#جے_انت
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.اچائی;
		    إذا (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_خالی_تھاواں_صفاں;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    إذا (scrolled <= 0)
	سطر_گنتی = 0;
    // more than a screenfull, don't scroll but redraw
    وإلا إذا (ورتیا > curwin->w_اچائی)
	سطر_گنتی = ورتیا;
    // scroll minimal number of lines
    وإلا
    {
	سطر_گنتی = 0;
#جے_تعریف_شدہ FEAT_DIFF
	boff.fill = curwin->w_topfill;
#جے_انت
	boff.lnum = curwin->w_topline - 1;
	لكل (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.اچائی;
	    ++سطر_گنتی;
	}
	إذا (i < scrolled)	// below curwin->w_botline, don't scroll
	    سطر_گنتی = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    إذا (سطر_گنتی >= curwin->w_اچائی && سطر_گنتی > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    وإلا إذا (سطر_گنتی > 0)
    {
	إذا (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	وإلا
	    scrollup(سطر_گنتی, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    إذا (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && سیٹ_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_خالی_تھاواں_صفاں = old_خالی_تھاواں_صفاں;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    إذا (سیٹ_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    فراغ
scroll_cursor_halfway(صحيح atend, صحيح prefer_above)
{
    صحيح		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		topfill = 0;
#جے_انت
    صحيح		ہیٹھاں = 0;
    صحيح		ورتیا;
    lineoff_T	loff;
    lineoff_T	boff;
#جے_تعریف_شدہ FEAT_DIFF
    linenr_T	old_topline = curwin->w_topline;
#جے_انت

#جے_تعریف_شدہ FEAT_PROP_POPUP
    // if the width changed this needs to be updated first
    may_update_popup_position();
#جے_انت
    loff.lnum = boff.lnum = curwin->w_cursor.lnum;
#جے_تعریف_شدہ FEAT_FOLDING
    (فراغ)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#جے_انت
#جے_تعریف_شدہ FEAT_DIFF
    ورتیا = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#وإلا
    ورتیا = plines(loff.lnum);
#جے_انت
    topline = loff.lnum;

    صحيح want_اچائی;
    صحيح do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    إذا (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	إذا (atend)
	{
	    want_اچائی = (curwin->w_اچائی - ورتیا) / 2;
	    ورتیا = 0;
	}
	وإلا
	    want_اچائی = curwin->w_اچائی;
    }

    طالما (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	إذا (do_sms)
	{
	    topline_پٹھ_winheight(&loff, FALSE);
	    إذا (loff.اچائی == MAXCOL)
		اكسر;
	    ورتیا += loff.اچائی;
	    إذا (!atend && boff.lnum < curbuf->b_ml.ml_سطر_گنتی)
	    {
		botline_forw(&boff);
		ورتیا += boff.اچائی;
	    }
	    إذا (ورتیا > want_اچائی)
	    {
		إذا (ورتیا - loff.اچائی < want_اچائی)
		{
		    topline = loff.lnum;
#جے_تعریف_شدہ FEAT_DIFF
		    topfill = loff.fill;
#جے_انت
		    skipcol = skipcol_from_plines(curwin, ورتیا - want_اچائی);
		}
		اكسر;
	    }
	    topline = loff.lnum;
#جے_تعریف_شدہ FEAT_DIFF
	    topfill = loff.fill;
#جے_انت
	    استمر;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	صحيح پورا = FALSE;
	لكل (صحيح گیڑا = 1; گیڑا <= 2; ++گیڑا)
	{
	    إذا (prefer_above ? (گیڑا == 2 && ہیٹھاں < above)
			     : (گیڑا == 1 && ہیٹھاں <= above))
	    {
		// add a line below the cursor
		إذا (boff.lnum < curbuf->b_ml.ml_سطر_گنتی)
		{
		    botline_forw(&boff);
		    ورتیا += boff.اچائی;
		    إذا (ورتیا > curwin->w_اچائی)
		    {
			پورا = TRUE;
			اكسر;
		    }
		    ہیٹھاں += boff.اچائی;
		}
		وإلا
		{
		    ++ہیٹھاں;	    // count a "~" line
		    إذا (atend)
			++ورتیا;
		}
	    }

	    إذا (prefer_above ? (گیڑا == 1 && ہیٹھاں >= above)
			     : (گیڑا == 1 && ہیٹھاں > above))
	    {
		// add a line above the cursor
		topline_پٹھ(&loff);
		إذا (loff.اچائی == MAXCOL)
		    ورتیا = MAXCOL;
		وإلا
		    ورتیا += loff.اچائی;
		إذا (ورتیا > curwin->w_اچائی)
		{
		    پورا = TRUE;
		    اكسر;
		}
		above += loff.اچائی;
		topline = loff.lnum;
#جے_تعریف_شدہ FEAT_DIFF
		topfill = loff.fill;
#جے_انت
	    }
	}
	إذا (پورا)
	    اكسر;
    }

#جے_تعریف_شدہ FEAT_FOLDING
    إذا (!hasFolding(topline, &curwin->w_topline, عدم))
#جے_انت
    {
	إذا (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    إذا (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    وإلا إذا (do_sms)
		reset_skipcol();
	}
    }
#جے_تعریف_شدہ FEAT_DIFF
    curwin->w_topfill = topfill;
    إذا (old_topline > curwin->w_topline + curwin->w_اچائی)
	curwin->w_botfill = false;
    check_topfill(curwin, FALSE);
#جے_انت
    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
    curwin->w_valid |= VALID_TOPLINE;
}

/*
 * Correct the cursor position so that it is in a part of the screen at least
 * 'scrolloff' lines from the top and bottom, if possible.
 * If not possible, put it at the same position as scroll_cursor_halfway().
 * When called topline must be valid!
 */
    فراغ
cursor_correct(فراغ)
{
    صحيح		above = 0;	    // screen lines above topline
    linenr_T	topline;
    صحيح		ہیٹھاں = 0;	    // screen lines below botline
    linenr_T	botline;
    صحيح		above_wanted, ہیٹھاں_wanted;
    linenr_T	cln;		    // Cursor Line Number
    صحيح		max_off;
    لما	so = get_scrolloff_قدر();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    ہیٹھاں_wanted = so;
    إذا (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	ہیٹھاں_wanted = mouse_dragging - 1;
    }
    إذا (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_اچائی / 2;
	إذا (ہیٹھاں_wanted > max_off)
	    ہیٹھاں_wanted = max_off;
    }
    validate_botline();
    إذا (curwin->w_botline == curbuf->b_ml.ml_سطر_گنتی + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	ہیٹھاں_wanted = 0;
	max_off = (curwin->w_اچائی - 1) / 2;
	إذا (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    إذا (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - ہیٹھاں_wanted
#جے_تعریف_شدہ FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#جے_انت
	    )
	أرجع;

    /*
     * Narrow down the area where the cursor can be put by taking lines from
     * the top and the bottom until:
     * - the desired context lines are found
     * - the lines from the top is past the lines from the bottom
     */
    topline = curwin->w_topline;
    botline = curwin->w_botline - 1;
#جے_تعریف_شدہ FEAT_DIFF
    // count filler lines as context
    above = curwin->w_topfill;
    ہیٹھاں = curwin->w_filler_صفاں;
#جے_انت
    طالما ((above < above_wanted || ہیٹھاں < ہیٹھاں_wanted) && topline < botline)
    {
	إذا (ہیٹھاں < ہیٹھاں_wanted && (ہیٹھاں <= above || above >= above_wanted))
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    إذا (hasFolding(botline, &botline, عدم))
		++ہیٹھاں;
	    وإلا
#جے_انت
		ہیٹھاں += plines(botline);
	    --botline;
	}
	إذا (above < above_wanted && (above < ہیٹھاں || ہیٹھاں >= ہیٹھاں_wanted))
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    إذا (hasFolding(topline, عدم, &topline))
		++above;
	    وإلا
#جے_انت
		above += PLINES_NOFILL(topline);
#جے_تعریف_شدہ FEAT_DIFF
	    // Count filler lines below this line as context.
	    إذا (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#جے_انت
	    ++topline;
	}
    }
    إذا (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    وإلا إذا (topline > botline)
	curwin->w_cursor.lnum = botline;
    وإلا
    {
	إذا (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	إذا (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_سطر_گنتی)
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
ساكن صحيح get_scroll_overlap(صحيح dir)
{
    lineoff_T loff;
    صحيح	    min_اچائی = curwin->w_اچائی - 2;

    validate_botline();
    إذا ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_سطر_گنتی))
	أرجع min_اچائی + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#جے_تعریف_شدہ FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_صفاں : curwin->w_topfill);
    loff.اچائی = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#وإلا
    loff.اچائی = plines(loff.lnum);
#جے_انت

    صحيح h1 = loff.اچائی;
    إذا (h1 > min_اچائی)
	أرجع min_اچائی + 2;  // no overlap
    إذا (dir == FORWARD)
	topline_پٹھ(&loff);
    وإلا
	botline_forw(&loff);

    صحيح h2 = loff.اچائی;
    إذا (h2 == MAXCOL || h2 + h1 > min_اچائی)
	أرجع min_اچائی + 2;  // no overlap
    إذا (dir == FORWARD)
	topline_پٹھ(&loff);
    وإلا
	botline_forw(&loff);

    صحيح h3 = loff.اچائی;
    إذا (h3 == MAXCOL || h3 + h2 > min_اچائی)
	أرجع min_اچائی + 2;  // no overlap
    إذا (dir == FORWARD)
	topline_پٹھ(&loff);
    وإلا
	botline_forw(&loff);

    صحيح h4 = loff.اچائی;
    إذا (h4 == MAXCOL || h4 + h3 + h2 > min_اچائی || h3 + h2 + h1 > min_اچائی)
	أرجع min_اچائی + 1;  // 1 line overlap
    وإلا
	أرجع min_اچائی;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
ساكن صحيح scroll_with_sms(صحيح dir, لما گنتی, لما *curscount)
{
    صحيح		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		prev_topfill = curwin->w_topfill;
#جے_انت

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, گنتی);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    إذا (!prev_sms && curwin->w_skipcol > 0)
    {
	صحيح fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	إذا (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	صحيح width1 = curwin->w_چوڑائی - curwin_خانہ_off();
	صحيح width2 = width1 + curwin_خانہ_off2();
	گنتی = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	إذا (fixdir == FORWARD)
	    گنتی = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, گنتی);
	*curscount += گنتی * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    أرجع curwin->w_topline != prev_topline
#جے_تعریف_شدہ FEAT_DIFF
	|| curwin->w_topfill != prev_topfill
#جے_انت
	|| curwin->w_skipcol != prev_skipcol;
}

/*
 * Move screen "count" (half) pages up ("dir" is BACKWARD) or down ("dir" is
 * FORWARD) and update the screen. Handle moving the cursor and not scrolling
 * to reveal end of buffer lines for half-page scrolling with CTRL-D and CTRL-U.
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
pagescroll(صحيح dir, لما گنتی, صحيح half)
{
    صحيح		did_move = FALSE;
    صحيح		buflen = curbuf->b_ml.ml_سطر_گنتی;
    colnr_T	prev_خانہ = curwin->w_cursor.خانہ;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    إذا (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	إذا (گنتی)
	    curwin->w_p_scr = MIN(curwin->w_اچائی, گنتی);
	گنتی = MIN(curwin->w_اچائی, curwin->w_p_scr);

	لما curscount = گنتی;
	// Adjust count so as to not reveal end of buffer lines.
	إذا (dir == FORWARD
		    && (curwin->w_topline + curwin->w_اچائی + گنتی > buflen
#جے_تعریف_شدہ FEAT_FOLDING
							|| hasAnyFolding(curwin)
#جے_انت
	   ))
	{
	    صحيح n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    إذا (n - گنتی < curwin->w_اچائی && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_اچائی + گنتی);
	    إذا (n < curwin->w_اچائی + گنتی)
		گنتی = n - curwin->w_اچائی;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	إذا (گنتی > 0)
	{
	    did_move = scroll_with_sms(dir, گنتی, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.خانہ = prev_خانہ;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	إذا (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	وإلا إذا (dir == FORWARD)
	    cursor_تھلے_inner(curwin, curscount);
	وإلا
	    cursor_اتے_inner(curwin, curscount);
    }
    وإلا
    {
	// Scroll [count] times 'window' or current window height lines.
	گنتی *= ((ONE_کھڑکی && p_کھڑکی > 0 && p_کھڑکی < صفاں - 1) ?
				MAX(1, p_کھڑکی - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, گنتی, &گنتی);

	إذا (did_move)
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

    إذا (get_scrolloff_قدر() > 0)
	cursor_correct();
#جے_تعریف_شدہ FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#جے_انت
    did_move = did_move
	|| prev_خانہ != curwin->w_cursor.خانہ
	|| prev_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    إذا (!did_move)
	beep_flush();
    وإلا إذا (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    وإلا إذا (p_sol)
	nv_g_home_m_cmd(&ca);

    أرجع did_move ? OK : FAIL;
}

    فراغ
do_check_cursorbind(فراغ)
{
    ساكن win_T	*prev_curwin = عدم;
    ساكن buf_T	*prev_curbuf = عدم;
    ساكن varnumber_T	prev_changedtick = 0;
    ساكن pos_T	prev_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    إذا (curwin == prev_curwin && curbuf == prev_curbuf
	    && CHANGEDTICK(curbuf) == prev_changedtick
	    && EQUAL_POS(curwin->w_cursor, prev_cursor))
	أرجع;
    prev_curwin = curwin;
    prev_curbuf = curbuf;
    prev_changedtick = CHANGEDTICK(curbuf);
    prev_cursor = curwin->w_cursor;

    linenr_T	سطر = curwin->w_cursor.lnum;
    colnr_T	خانہ = curwin->w_cursor.خانہ;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    صحيح		سیٹ_curswant = curwin->w_سیٹ_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    صحيح		old_VIsual_select = VIsual_select;
    صحيح		old_VIsual_سرگرم = VIsual_سرگرم;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_سرگرم = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_بفر;
	// skip original window and windows with 'nocursorbind'
	إذا (curwin != old_curwin && curwin->w_p_crb)
	{
#جے_تعریف_شدہ FEAT_DIFF
	    إذا (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_سطر(old_curbuf, سطر);
	    وإلا
#جے_انت
		curwin->w_cursor.lnum = سطر;
	    curwin->w_cursor.خانہ = خانہ;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_سیٹ_curswant = سیٹ_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    صحيح restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    إذا (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    إذا (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    إذا (!curwin->w_p_scb)
		update_topline();
	    curwin->w_redr_حالت = true;
	}
    }

    /*
     * reset current-window
     */
    VIsual_select = old_VIsual_select;
    VIsual_سرگرم = old_VIsual_سرگرم;
    curwin = old_curwin;
    curbuf = old_curbuf;
}
