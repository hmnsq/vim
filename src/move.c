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
ساكن صحيح scrolljump_ارزښت(فراغ);
ساكن صحيح check_سر_دننه_واټن(فراغ);
ساكن فراغ curs_لیکې(win_T *wp);

تعريفنوع بنية
{
    linenr_T	    lnum;	// line number
#که_تعریف_شوی FEAT_DIFF
    صحيح		    fill;	// filler lines
#د_که_پای
    صحيح		    لوړوالی;	// height of added line
} lineoff_T;

ساكن فراغ topline_شا(lineoff_T *lp);
ساكن فراغ botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    ساكن صحيح
adjust_plines_for_skipcol(win_T *wp)
{
    إذا (wp->w_skipcol == 0)
	أرجع 0;

    صحيح پلنوالی = wp->w_پلنوالی - win_ستون_off(wp);
    صحيح w2 = پلنوالی + win_ستون_off2(wp);
    إذا (wp->w_skipcol >= پلنوالی && w2 > 0)
	أرجع (wp->w_skipcol - پلنوالی) / w2 + 1;

    أرجع 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    صحيح
plines_correct_topline(win_T *wp, linenr_T lnum, صحيح حد_winheight)
{
    صحيح n;
#که_تعریف_شوی FEAT_DIFF
    إذا (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    وإلا
#د_که_پای
	n = plines_win(wp, lnum, FALSE);
    إذا (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    إذا (حد_winheight && n > wp->w_لوړوالی)
	n = wp->w_لوړوالی;
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
    صحيح		بشپړ;
    صحيح		i = 0;
    صحيح		use_cache;
#که_تعریف_شوی FEAT_FOLDING
    linenr_T    وروستی;
    صحيح		folded;
#د_که_پای

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
		    && !wp->w_بفر->b_mod_سېټ
		    && dollar_vcol == -1
		    && wp->w_skipcol == 0
#که_تعریف_شوی FEAT_DIFF
		    && !wp->w_p_diff
#د_که_پای
		    && wp->w_کرښې_valid > 0
		    && wp->w_کرښې[0].wl_lnum <= wp->w_topline;

    إذا (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	بشپړ = wp->w_cline_لیکه;
    }
    وإلا
    {
	lnum = wp->w_topline;
	بشپړ = 0;
    }

    // Find the w_lines[] entry for the starting line.
    إذا (use_cache)
	طالما (i < wp->w_کرښې_valid && wp->w_کرښې[i].wl_lnum < lnum)
	    ++i;

    لكل ( ; lnum <= wp->w_بفر->b_ml.ml_کرښه_شمېر; ++i)
    {
	صحيح	valid = FALSE;

#که_تعریف_شوی FEAT_FOLDING
	وروستی = lnum;
	folded = FALSE;
#د_که_پای
	// Try to use the size from the previous redraw.
	إذا (use_cache && i < wp->w_کرښې_valid)
	{
	    إذا (wp->w_کرښې[i].wl_lnum < lnum || !wp->w_کرښې[i].wl_valid)
		استمر;		// skip changed or deleted lines
	    إذا (wp->w_کرښې[i].wl_lnum == lnum)
		valid = TRUE;
	    وإلا // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	إذا (valid)
	{
	    n = wp->w_کرښې[i].wl_اندازه;
#که_تعریف_شوی FEAT_FOLDING
	    folded = wp->w_کرښې[i].wl_folded;
	    وروستی = wp->w_کرښې[i].wl_lastlnum;
#د_که_پای
	}
	وإلا
	{
#که_تعریف_شوی FEAT_FOLDING
	    إذا (hasFoldingWin(wp, lnum, عدم, &وروستی, TRUE, عدم))
	    {
		n = 1;
		folded = TRUE;
	    }
	    وإلا
#د_که_پای
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	إذا (
#که_تعریف_شوی FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && وروستی >= wp->w_cursor.lnum
#وإلا
		lnum == wp->w_cursor.lnum
#د_که_پای
	   )
	{
	    wp->w_cline_لیکه = بشپړ;
	    wp->w_cline_لوړوالی = n;
#که_تعریف_شوی FEAT_FOLDING
	    wp->w_cline_folded = folded;
#د_که_پای
	    redraw_for_cursorline(wp);
	    wp->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	إذا (بشپړ + n > wp->w_لوړوالی)
	    اكسر;
	بشپړ += n;
#که_تعریف_شوی FEAT_FOLDING
	lnum = وروستی + 1;
#وإلا
	++lnum;
#د_که_پای
    }

    // wp->w_botline is the line that is just below the window
    wp->w_botline = lnum;
    wp->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    سېټ_تشه_لیکې(wp, بشپړ);
}

/*
 * Redraw when w_cline_row changes and 'relativenumber' or 'cursorline' is
 * set.
 */
    ساكن فراغ
redraw_for_cursorline(win_T *wp)
{
    إذا ((wp->w_p_rnu
#که_تعریف_شوی FEAT_SYN_HL
		|| wp->w_p_cul
#د_که_پای
		)
	    && (wp->w_valid & VALID_CROW) == 0
	    && !pum_visible())
    {
	// win_line() will redraw the number column and cursorline only.
	redraw_win_later(wp, UPD_VALID);
    }
}

#که_تعریف_شوی FEAT_SYN_HL
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
#د_که_پای

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    فراغ
سېټ_valid_virtcol(win_T *wp, colnr_T vcol)
{
    wp->w_virtcol = vcol;
#که_تعریف_شوی FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#د_که_پای
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
	extra2 = win_ستون_off(wp) - win_ستون_off2(wp);
#إذا defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    إذا (*get_showbreak_ارزښت(wp) != NUL)
	أرجع 0;
#د_که_پای
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
    صحيح width1 = wp->w_پلنوالی - win_ستون_off(wp);

    صحيح skipcol = 0;
    إذا (plines_off > 0)
	skipcol += width1;
    إذا (plines_off > 1)
	skipcol += (width1 + win_ستون_off2(wp)) * (plines_off - 1);
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
    أرجع get_scrolloff_ارزښت() > 0 && get_scrolloffpad_ارزښت() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    ساكن bool
scrolloffpad_eof_pressure(linenr_T lnum, اوږد so)
{
    إذا (!use_scrolloffpad() || so <= 0)
	أرجع false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    أرجع lnum > curbuf->b_ml.ml_کرښه_شمېر - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    فراغ
update_topline(فراغ)
{
    اوږد	کرښه_شمېر;
    صحيح		halfheight;
    صحيح		n;
#که_تعریف_شوی FEAT_FOLDING
    linenr_T	lnum;
#د_که_پای
    صحيح		check_topline = FALSE;
    صحيح		check_botline = FALSE;
    اوږد	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    صحيح		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    إذا (skip_update_topline)
	أرجع;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    إذا (!screen_valid(TRUE) || curwin->w_لوړوالی == 0)
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
#که_تعریف_شوی FEAT_DIFF
    صحيح old_topfill = curwin->w_topfill;
#د_که_پای

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
	    وإلا إذا (check_سر_دننه_واټن())
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
#که_تعریف_شوی FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	إذا (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#د_که_پای

	إذا (check_topline)
	{
	    halfheight = curwin->w_لوړوالی / 2 - 1;
	    إذا (halfheight < 2)
		halfheight = 2;

#که_تعریف_شوی FEAT_FOLDING
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
		    إذا (lnum >= curbuf->b_ml.ml_کرښه_شمېر || n >= halfheight)
			اكسر;
		    (فراغ)hasFolding(lnum, عدم, &lnum);
		}
	    }
	    وإلا
#د_که_پای
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    صحيح min_scroll = scrolljump_ارزښت();
	    إذا (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    وإلا إذا (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    وإلا
	    {
		scroll_cursor_سر(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	وإلا
	{
#که_تعریف_شوی FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#د_که_پای
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

	إذا (curwin->w_botline <= curbuf->b_ml.ml_کرښه_شمېر || use_scrolloffpad())
	{
	    إذا (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		إذا (((اوږد)curwin->w_cursor.lnum
					   >= (اوږد)curwin->w_botline - *so_ptr
#که_تعریف_شوی FEAT_FOLDING
			|| hasAnyFolding(curwin)
#د_که_پای
			))
		{
		    lineoff_T	loff;

		    // Cursor is (a few lines) above botline, check if there
		    // are 'scrolloff' window lines below the cursor.
		    // If not, need to scroll.
		    n = eof_pressure ? 0 : curwin->w_تشه_لیکې;
		    loff.lnum = curwin->w_cursor.lnum;
#که_تعریف_شوی FEAT_FOLDING
		    // In a fold go to its last line.
		    (فراغ)hasFolding(loff.lnum, عدم, &loff.lnum);
#د_که_پای
#که_تعریف_شوی FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_لیکې;
#د_که_پای
		    loff.لوړوالی = 0;
		    طالما (loff.lnum < curwin->w_botline
#که_تعریف_شوی FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#د_که_پای
			    )
		    {
			n += loff.لوړوالی;
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
#که_تعریف_شوی FEAT_FOLDING
		إذا (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    کرښه_شمېر = 0;
		    لكل (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++کرښه_شمېر;
			// stop at end of file or when we know we are far off
			إذا (lnum <= 0 || کرښه_شمېر > curwin->w_لوړوالی + 1)
			    اكسر;
			(فراغ)hasFolding(lnum, &lnum, عدم);
		    }
		}
		وإلا
#د_که_پای
		    کرښه_شمېر = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		إذا (کرښه_شمېر <= curwin->w_لوړوالی + 1)
		{
		    إذا (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    وإلا
			scroll_cursor_bot(scrolljump_ارزښت(), FALSE);
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
#که_تعریف_شوی FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#د_که_پای
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
scrolljump_ارزښت(فراغ)
{
    إذا (p_sj >= 0)
	أرجع (صحيح)p_sj;
    أرجع (curwin->w_لوړوالی * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    ساكن صحيح
check_سر_دننه_واټن(فراغ)
{
    lineoff_T	loff;
    صحيح		n;
    اوږد	so = get_scrolloff_ارزښت();

    إذا (curwin->w_cursor.lnum < curwin->w_topline + so
#که_تعریف_شوی FEAT_FOLDING
		    || hasAnyFolding(curwin)
#د_که_پای
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#که_تعریف_شوی FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#وإلا
	n = 0;
#د_که_پای
	// Count the visible screen lines above the cursor line.
	طالما (n < so)
	{
	    topline_شا(&loff);
	    // Stop when included a line above the window.
	    إذا (loff.lnum < curwin->w_topline
#که_تعریف_شوی FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#د_که_پای
		    )
		اكسر;
	    n += loff.لوړوالی;
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
#که_تعریف_شوی FEAT_PROP_POPUP
	- curwin->w_virtcol_لومړی_char
#د_که_پای
	;
    curwin->w_سېټ_curswant = false;
}

/*
 * Update w_curswant if w_set_curswant is set.
 */
    فراغ
update_curswant(فراغ)
{
    إذا (curwin->w_سېټ_curswant)
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
    وإلا إذا (wp->w_cursor.ستون != wp->w_valid_cursor.ستون
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
    فراغ
بدل_شوی_کړکۍ_setting(فراغ)
{
    بدل_شوی_کړکۍ_setting_win(curwin);
}

    فراغ
بدل_شوی_کړکۍ_setting_win(win_T *wp)
{
    wp->w_کرښې_valid = 0;
    بدل_شوی_کرښه_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    فراغ
بدل_شوی_کړکۍ_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	إذا (wp->w_بفر == buf)
	    بدل_شوی_کړکۍ_setting_win(wp);
}
#د_که_پای

/*
 * Call changed_window_setting_win() for every window.
 */
    فراغ
بدل_شوی_کړکۍ_setting_all(فراغ)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	بدل_شوی_کړکۍ_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    فراغ
سېټ_topline(win_T *wp, linenr_T lnum)
{
#که_تعریف_شوی FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#د_که_پای

#که_تعریف_شوی FEAT_FOLDING
    // go to first of folded lines
    (فراغ)hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
#د_که_پای
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    إذا (wp->w_botline > wp->w_بفر->b_ml.ml_کرښه_شمېر + 1)
	wp->w_botline = wp->w_بفر->b_ml.ml_کرښه_شمېر + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_سېټ = true;
#که_تعریف_شوی FEAT_DIFF
    إذا (lnum != prev_topline)
	// Keep the filler lines when the topline didn't change.
	wp->w_topfill = 0;
#د_که_پای
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
بدل_شوی_cline_bef_curs(فراغ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    فراغ
بدل_شوی_cline_bef_curs_win(win_T *wp)
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
بدل_شوی_کرښه_abv_curs(فراغ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    فراغ
بدل_شوی_کرښه_abv_curs_win(win_T *wp)
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
بدل_شوی_کرښه_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_بفر == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#د_که_پای

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
validate_cline_لیکه(فراغ)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    إذا (!(curwin->w_valid & VALID_CROW))
	curs_لیکې(curwin);
}
#د_که_پای

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    ساكن فراغ
curs_لیکې(win_T *wp)
{
    linenr_T	lnum;
    صحيح		i;
    صحيح		all_invalid;
    صحيح		valid;
#که_تعریف_شوی FEAT_FOLDING
    اوږد	fold_شمېر;
#د_که_پای

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_کرښې_valid == 0
			|| wp->w_کرښې[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_لیکه = 0;
    لكل (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	إذا (!all_invalid && i < wp->w_کرښې_valid)
	{
	    إذا (wp->w_کرښې[i].wl_lnum < lnum || !wp->w_کرښې[i].wl_valid)
		استمر;		// skip changed or deleted lines
	    إذا (wp->w_کرښې[i].wl_lnum == lnum)
	    {
#که_تعریف_شوی FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		إذا (!wp->w_بفر->b_mod_سېټ
			|| wp->w_کرښې[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_بفر->b_mod_سر
					     > wp->w_کرښې[i].wl_lastlnum + 1)
#د_که_پای
		valid = TRUE;
	    }
	    وإلا إذا (wp->w_کرښې[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	إذا (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#که_تعریف_شوی FEAT_DIFF
					    && !wp->w_p_diff
#د_که_پای
	       )))
	{
#که_تعریف_شوی FEAT_FOLDING
	    lnum = wp->w_کرښې[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    إذا (lnum > wp->w_cursor.lnum)
		اكسر;
#وإلا
	    ++lnum;
#د_که_پای
	    wp->w_cline_لیکه += wp->w_کرښې[i].wl_اندازه;
	}
	وإلا
	{
#که_تعریف_شوی FEAT_FOLDING
	    fold_شمېر = folded_شمېر(wp, lnum, عدم);
	    إذا (fold_شمېر)
	    {
		lnum += fold_شمېر;
		إذا (lnum > wp->w_cursor.lnum)
		    اكسر;
		++wp->w_cline_لیکه;
	    }
	    وإلا
#د_که_پای
	    {
		wp->w_cline_لیکه += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    إذا (!(wp->w_valid & VALID_CHEIGHT))
    {
	إذا (all_invalid
		|| i == wp->w_کرښې_valid
		|| (i < wp->w_کرښې_valid
		    && (!wp->w_کرښې[i].wl_valid
			|| wp->w_کرښې[i].wl_lnum != wp->w_cursor.lnum)))
	{
#که_تعریف_شوی FEAT_DIFF
	    إذا (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_لوړوالی = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    وإلا
#د_که_پای
		wp->w_cline_لوړوالی = plines_win(wp, wp->w_cursor.lnum, TRUE);
#که_تعریف_شوی FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      عدم, عدم, TRUE, عدم);
#د_که_پای
	}
	وإلا إذا (i > wp->w_کرښې_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_لوړوالی = 0;
#که_تعریف_شوی FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      عدم, عدم, TRUE, عدم);
#د_که_پای
	}
	وإلا
	{
	    wp->w_cline_لوړوالی = wp->w_کرښې[i].wl_اندازه;
#که_تعریف_شوی FEAT_FOLDING
	    wp->w_cline_folded = wp->w_کرښې[i].wl_folded;
#د_که_پای
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

#که_تعریف_شوی FEAT_PROP_POPUP
    wp->w_virtcol_لومړی_char = 0;
#د_که_پای
    getvvcol(wp, &wp->w_cursor, عدم, &(wp->w_virtcol), عدم,
							 GETVCOL_FOR_VIRTCOL);
#که_تعریف_شوی FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#د_که_پای
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

#که_تعریف_شوی FEAT_DIFF
    إذا (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_لوړوالی = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    وإلا
#د_که_پای
	curwin->w_cline_لوړوالی = plines(curwin->w_cursor.lnum);
#که_تعریف_شوی FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, عدم, عدم);
#د_که_پای
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    فراغ
validate_cursor_ستون(فراغ)
{
    colnr_T off;
    colnr_T ستون;
    صحيح     پلنوالی;

    validate_virtcol();

    إذا (curwin->w_valid & VALID_WCOL)
	أرجع;

    ستون = curwin->w_virtcol;
    off = curwin_ستون_off();
    ستون += off;
    پلنوالی = curwin->w_پلنوالی - off + curwin_ستون_off2();

    // long line wrapping, adjust curwin->w_wrow
    إذا (curwin->w_p_wrap
	    && ستون >= (colnr_T)curwin->w_پلنوالی
	    && پلنوالی > 0)
	// use same formula as what is used in curs_columns()
	ستون -= ((ستون - curwin->w_پلنوالی) / پلنوالی + 1) * پلنوالی;
    إذا (ستون > (صحيح)curwin->w_leftcol)
	ستون -= curwin->w_leftcol;
    وإلا
	ستون = 0;
    curwin->w_wcol = ستون;

    curwin->w_valid |= VALID_WCOL;
#که_تعریف_شوی FEAT_PROP_POPUP
    curwin->w_flags &= ~WFLAG_WCOL_OFF_ADDED;
#د_که_پای
}

/*
 * Compute offset of a window, occupied by absolute or relative line number,
 * fold column and sign column (these don't move when scrolling horizontally).
 */
    صحيح
win_ستون_off(win_T *wp)
{
    أرجع (((wp->w_p_nu || wp->w_p_rnu) ? شمېره_پلنوالی(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#که_تعریف_شوی FEAT_FOLDING
	    + wp->w_p_fdc
#د_که_پای
#که_تعریف_شوی FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#د_که_پای
	   );
}

    صحيح
curwin_ستون_off(فراغ)
{
    أرجع win_ستون_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    صحيح
win_ستون_off2(win_T *wp)
{
    إذا ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != عدم)
	أرجع شمېره_پلنوالی(wp) + 1;
    أرجع 0;
}

    صحيح
curwin_ستون_off2(فراغ)
{
    أرجع win_ستون_off2(curwin);
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
    اوږد	diff;
    صحيح		extra;		// offset for first screen line
    اوږد	off_کیڼ, off_ښی;
    صحيح		n;
    صحيح		p_کرښې;
    صحيح		width1;		// text width for first screen line
    صحيح		width2 = 0;	// text width for second and later screen line
    صحيح		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    اوږد	so = get_scrolloff_ارزښت();
    اوږد	siso = get_sidescrolloff_ارزښت();
    صحيح		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    إذا (!(curwin->w_valid & VALID_CROW))
	curs_لیکې(curwin);

#که_تعریف_شوی FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_لومړی_char = 0;
#د_که_پای

    /*
     * Compute the number of virtual columns.
     */
#که_تعریف_شوی FEAT_FOLDING
    إذا (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    وإلا
#د_که_پای
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    إذا (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_ستون_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_لیکه;

    width1 = curwin->w_پلنوالی - extra;
    إذا (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_پلنوالی - 1;
	إذا (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_لوړوالی - 1;
	وإلا
	    curwin->w_wrow = curwin->w_لوړوالی - 1 - curwin->w_تشه_لیکې;
    }
    وإلا إذا (curwin->w_p_wrap && curwin->w_پلنوالی != 0)
    {
	width2 = width1 + curwin_ستون_off2();

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
	إذا (curwin->w_wcol >= curwin->w_پلنوالی)
	{
	    // this same formula is used in validate_cursor_col()
	    n = (curwin->w_wcol - curwin->w_پلنوالی) / width2 + 1;
	    curwin->w_wcol -= n * width2;
	    curwin->w_wrow += n;
	}
    }

    // No line wrapping: compute curwin->w_leftcol if scrolling is on and line
    // is not folded.
    // If scrolling is off, curwin->w_leftcol is assumed to be 0
    وإلا إذا (may_scroll
#که_تعریف_شوی FEAT_FOLDING
	    && !curwin->w_cline_folded
#د_که_پای
	    )
    {
#که_تعریف_شوی FEAT_PROP_POPUP
	إذا (curwin->w_virtcol_لومړی_char > 0)
	{
	    صحيح ستون_شمېر = (curwin->w_پلنوالی - extra);
	    صحيح لیکې_2 = ستون_شمېر > 0 ? curwin->w_virtcol_لومړی_char / ستون_شمېر : 1;

	    // each "above" text prop shifts the text one row down
	    curwin->w_wrow += لیکې_2;
	    curwin->w_wcol -= لیکې_2 * ستون_شمېر;
	    endcol -= لیکې_2 * ستون_شمېر;
	    curwin->w_cline_لوړوالی = لیکې_2 + 1;
	}
#د_که_پای
	/*
	 * If Cursor is left of the screen, scroll rightwards.
	 * If Cursor is right of the screen, scroll leftwards.
	 * If we get closer to the edge than 'sidescrolloff', scroll a little
	 * extra.
	 */
	off_کیڼ = startcol - curwin->w_leftcol - siso;
	off_ښی = endcol - curwin->w_leftcol - (curwin->w_پلنوالی - siso) + 1;
	إذا (off_کیڼ < 0 || off_ښی > 0)
	{
	    إذا (off_کیڼ < 0)
		diff = -off_کیڼ;
	    وإلا
		diff = off_ښی;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    إذا (p_ss == 0 || diff >= width1 / 2 || off_ښی >= off_کیڼ)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    وإلا
	    {
		إذا (diff < p_ss)
		    diff = p_ss;
		إذا (off_کیڼ < 0)
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

#که_تعریف_شوی FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    إذا (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    وإلا
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#د_که_پای

    prev_skipcol = curwin->w_skipcol;

    p_کرښې = 0;

    إذا ((curwin->w_wrow >= curwin->w_لوړوالی
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_لوړوالی)
		    && (p_کرښې =
#که_تعریف_شوی FEAT_DIFF
			plines_win_nofill
#وإلا
			plines_win
#د_که_پای
			(curwin, curwin->w_cursor.lnum, FALSE))
						    - 1 >= curwin->w_لوړوالی))
	    && curwin->w_لوړوالی != 0
	    && curwin->w_cursor.lnum == curwin->w_topline
	    && width2 > 0
	    && curwin->w_پلنوالی != 0)
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
	إذا (p_کرښې == 0)
	    p_کرښې = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_کرښې;
	إذا (p_کرښې > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	وإلا
	    n = p_کرښې;
	إذا ((colnr_T)n >= curwin->w_لوړوالی + curwin->w_skipcol / width2 - so)
	    extra += 2;

	إذا (extra == 3 || curwin->w_لوړوالی <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    إذا (n > curwin->w_لوړوالی / 2)
		n -= curwin->w_لوړوالی / 2;
	    وإلا
		n = 0;
	    // don't skip more than necessary
	    إذا (n > p_کرښې - curwin->w_لوړوالی + 1)
		n = p_کرښې - curwin->w_لوړوالی + 1;
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
	    endcol = (n - curwin->w_لوړوالی + 1) * width2;
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

	إذا (curwin->w_wrow >= curwin->w_لوړوالی)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_لوړوالی + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((صحيح)prev_skipcol - (صحيح)curwin->w_skipcol) / width2;
	إذا (extra > 0)
	    win_ins_کرښې(curwin, 0, extra, FALSE, FALSE);
	وإلا إذا (extra < 0)
	    win_del_کرښې(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    وإلا إذا (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    إذا (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#که_تعریف_شوی FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#د_که_پای
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    إذا (popup_is_popup(curwin) && curbuf->b_غړی != عدم)
    {
	curwin->w_wrow += popup_سر_extra(curwin);
	curwin->w_wcol += popup_کیڼ_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    وإلا
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#د_که_پای

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
    صحيح		لیکه = 0;
    colnr_T	coloff = 0;

    إذا (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    ستون;
	صحيح	    پلنوالی;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	صحيح	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
# endif
	لیکه = plines_m_win(wp, wp->w_topline, lnum - 1, لوی_عدد);
	// "row" should be the screen line where line "lnum" begins, which can
	// be negative if "lnum" is "w_topline" and "w_skipcol" is non-zero.
	لیکه -= adjust_plines_for_skipcol(wp);

# ifdef FEAT_DIFF
	// Add filler lines above this buffer line.
	لیکه += lnum == wp->w_topline ? wp->w_topfill
				     : diff_check_fill(wp, lnum);
# endif

	colnr_T	off = win_ستون_off(wp);
# ifdef FEAT_FOLDING
	إذا (is_folded)
	{
	    لیکه += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	وإلا
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    ستون = scol;
	    ستون += off;
	    پلنوالی = wp->w_پلنوالی - off + win_ستون_off2(wp);

	    // long line wrapping, adjust row
	    إذا (wp->w_p_wrap
		    && ستون >= (colnr_T)wp->w_پلنوالی
		    && پلنوالی > 0)
	    {
		// use same formula as what is used in curs_columns()
		صحيح rowoff = ((ستون - wp->w_پلنوالی) / پلنوالی + 1);
		ستون -= rowoff * پلنوالی;
		لیکه += rowoff;
	    }
	    ستون -= wp->w_leftcol;
	    إذا (ستون >= wp->w_پلنوالی)
		ستون = -1;
	    إذا (ستون >= 0 && لیکه >= 0 && لیکه < wp->w_لوړوالی)
	    {
		coloff = ستون - scol + wp->w_wincol + 1;
		لیکه += W_WINROW(wp) + 1;
	    }
	    وإلا
		// character is out of the window
		لیکه = scol = ccol = ecol = 0;
	}
    }
    *rowp = لیکه;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    إذا (wp->w_p_rl && لیکه > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	صحيح endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_پلنوالی - endoff;
    }
# endif
}
#د_که_پای

#إذا defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    فراغ
f_screenpos(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    صحيح		لیکه = 0;
    صحيح		scol = 0, ccol = 0, ecol = 0;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    dict = rettv->vval.v_dict;

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL
		|| check_for_شمېره_arg(argvars, 2) == FAIL))
	أرجع;

    wp = پیدا_کړه_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	أرجع;

    pos.lnum = tv_get_شمېره(&argvars[1]);
    إذا (pos.lnum > wp->w_بفر->b_ml.ml_کرښه_شمېر)
    {
	semsg(_(e_invalid_کرښه_شمېره_nr), pos.lnum);
	أرجع;
    }
    pos.ستون = tv_get_شمېره(&argvars[2]) - 1;
    إذا (pos.ستون < 0)
	pos.ستون = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &لیکه, &scol, &ccol, &ecol);

    dict_جمع_کړه_شمېره(dict, "row", لیکه);
    dict_جمع_کړه_شمېره(dict, "col", scol);
    dict_جمع_کړه_شمېره(dict, "curscol", ccol);
    dict_جمع_کړه_شمېره(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    ساكن صحيح
virtcol2col(win_T *wp, linenr_T lnum, صحيح vcol)
{
    صحيح		دننه_واټن = vcol2col(wp, lnum, vcol - 1, عدم);
    char_u	*کرښه = ml_get_buf(wp->w_بفر, lnum, FALSE);
    char_u	*p = کرښه + دننه_واټن;

    إذا (*p == NUL)
    {
	إذا (p == کرښه)  // empty line
	    أرجع 0;
	// Move to the first byte of the last char.
	MB_PTR_شا(کرښه, p);
    }
    أرجع (صحيح)(p - کرښه + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    فراغ
f_virtcol2col(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    صحيح		screencol;
    صحيح		error = FALSE;

    rettv->vval.v_شمېره = -1;

    إذا (check_for_شمېره_arg(argvars, 0) == FAIL
	    || check_for_شمېره_arg(argvars, 1) == FAIL
	    || check_for_شمېره_arg(argvars, 2) == FAIL)
	أرجع;

    wp = پیدا_کړه_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	أرجع;

    lnum = tv_get_شمېره_chk(&argvars[1], &error);
    إذا (error || lnum < 0 || lnum > wp->w_بفر->b_ml.ml_کرښه_شمېر)
	أرجع;

    screencol = tv_get_شمېره_chk(&argvars[2], &error);
    إذا (error || screencol < 0)
	أرجع;

    rettv->vval.v_شمېره = virtcol2col(wp, lnum, screencol);
}
#د_که_پای

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
ساكن فراغ cursor_correct_sms(فراغ)
{
    إذا (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	أرجع;

    اوږد    so = get_scrolloff_ارزښت();
    صحيح	    width1 = curwin->w_پلنوالی - curwin_ستون_off();
    صحيح	    width2 = width1 + curwin_ستون_off2();
    صحيح	    so_ستون_شمېر = so == 0 ? 0 : width1 + (so - 1) * width2;
    صحيح	    space_ستون_شمېر = (curwin->w_لوړوالی - 1) * width2;
    صحيح	    overlap, سر_2, bot;
    صحيح	    اندازه = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    إذا (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_ستون_شمېر = 0;               // Ignore 'scrolloff' at top of buffer.
    وإلا إذا (so_ستون_شمېر > space_ستون_شمېر / 2)
	so_ستون_شمېر = space_ستون_شمېر / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    طالما (so_ستون_شمېر > اندازه && so_ستون_شمېر - width2 >= width1 && width1 > 0)
	so_ستون_شمېر -= width2;
    إذا (so_ستون_شمېر >= width1 && so_ستون_شمېر > اندازه)
	so_ستون_شمېر -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_پلنوالی - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    سر_2 = curwin->w_skipcol + (so_ستون_شمېر != 0 ? so_ستون_شمېر : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_لوړوالی - 1) * width2
								    - so_ستون_شمېر;
    validate_virtcol();
    colnr_T ستون = curwin->w_virtcol;

    إذا (ستون < سر_2)
    {
	إذا (ستون < width1)
	    ستون += width1;
	طالما (width2 > 0 && ستون < سر_2)
	    ستون += width2;
    }
    وإلا
	طالما (width2 > 0 && ستون >= bot)
	    ستون -= width2;

    إذا (ستون != curwin->w_virtcol)
    {
	صحيح rc;

	curwin->w_curswant = ستون;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	إذا (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_کرښه_شمېر)
	{
	    validate_virtcol();
	    إذا (curwin->w_virtcol < curwin->w_skipcol + overlap)
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
    فراغ
scroll_redraw(صحيح پورته, اوږد شمېر)
{
    linenr_T	prev_topline = curwin->w_topline;
    صحيح		prev_skipcol = curwin->w_skipcol;
#که_تعریف_شوی FEAT_DIFF
    صحيح		prev_topfill = curwin->w_topfill;
#د_که_پای
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    إذا (پورته)
	scrollup(شمېر, TRUE);
    وإلا
	scrolldown(شمېر, TRUE);
    إذا (get_scrolloff_ارزښت() > 0)
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
#که_تعریف_شوی FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#د_که_پای
		)
	{
	    إذا (پورته)
	    {
		إذا (curwin->w_cursor.lnum > prev_lnum
			|| cursor_ښکته(1L, FALSE) == FAIL)
		    اكسر;
	    }
	    وإلا
	    {
		إذا (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_پورته(1L, FALSE) == FAIL)
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
    اوږد	کرښه_شمېر,
    صحيح		byfold ناکارول_شوی)	// TRUE: count a closed fold as one line
{
    اوږد	بشپړ = 0;	// total # of physical lines done
    صحيح		wrow;
    صحيح		moved = FALSE;
    صحيح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    صحيح		width1 = 0;
    صحيح		width2 = 0;

    إذا (do_sms)
    {
	width1 = curwin->w_پلنوالی - curwin_ستون_off();
	width2 = width1 + curwin_ستون_off2();
    }

#که_تعریف_شوی FEAT_FOLDING
    linenr_T	لومړی;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#د_که_پای
    validate_cursor();		// w_wrow needs to be valid
    لكل (صحيح todo = کرښه_شمېر; todo > 0; --todo)
    {
#که_تعریف_شوی FEAT_DIFF
	إذا (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_لوړوالی - 1)
	{
	    ++curwin->w_topfill;
	    ++بشپړ;
	}
	وإلا
#د_که_پای
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
		++بشپړ;
	    }
	    وإلا
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#که_تعریف_شوی FEAT_DIFF
		curwin->w_topfill = 0;
#د_که_پای
#که_تعریف_شوی FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		إذا (hasFolding(curwin->w_topline, &لومړی, عدم))
		{
		    ++بشپړ;
		    إذا (!byfold)
			todo -= curwin->w_topline - لومړی - 1;
		    curwin->w_botline -= curwin->w_topline - لومړی;
		    curwin->w_topline = لومړی;
		}
		وإلا
#د_که_پای
		إذا (do_sms)
		{
		    صحيح اندازه = linetabsize_eol(curwin, curwin->w_topline);
		    إذا (اندازه > width1)
		    {
			curwin->w_skipcol = width1;
			اندازه -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    طالما (اندازه > width2)
		    {
			curwin->w_skipcol += width2;
			اندازه -= width2;
		    }
		    ++بشپړ;
		}
		وإلا
		    بشپړ += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += بشپړ;		// keep w_wrow updated
    curwin->w_cline_لیکه += بشپړ;	// keep w_cline_row updated

#که_تعریف_شوی FEAT_DIFF
    إذا (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_لیکه = 0;
    check_topfill(curwin, TRUE);
#د_که_پای

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    إذا (curwin->w_p_wrap && curwin->w_پلنوالی != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_لوړوالی - 1 -
	    curwin->w_virtcol / curwin->w_پلنوالی;
    }
    طالما (wrow >= curwin->w_لوړوالی && curwin->w_cursor.lnum > 1)
    {
#که_تعریف_شوی FEAT_FOLDING
	إذا (hasFolding(curwin->w_cursor.lnum, &لومړی, عدم))
	{
	    --wrow;
	    إذا (لومړی == 1)
		curwin->w_cursor.lnum = 1;
	    وإلا
		curwin->w_cursor.lnum = لومړی - 1;
	}
	وإلا
#د_که_پای
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    إذا (moved)
    {
#که_تعریف_شوی FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#د_که_پای
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
    اوږد	کرښه_شمېر,
    صحيح		byfold ناکارول_شوی)	// TRUE: count a closed fold as one line
{
    صحيح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    إذا (do_sms
#که_تعریف_شوی FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#د_که_پای
#که_تعریف_شوی FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#د_که_پای
       )
    {
	صحيح	    width1 = curwin->w_پلنوالی - curwin_ستون_off();
	صحيح	    width2 = width1 + curwin_ستون_off2();
	صحيح	    اندازه = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	إذا (do_sms)
	    اندازه = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	لكل (صحيح todo = کرښه_شمېر; todo > 0; --todo)
	{
#که_تعریف_شوی FEAT_DIFF
	    إذا (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    وإلا
#د_که_پای
	    {
		linenr_T lnum = curwin->w_topline;

#که_تعریف_شوی FEAT_FOLDING
		إذا (byfold)
		    // for a closed fold: go to the last line in the fold
		    (فراغ)hasFolding(lnum, عدم, &lnum);
#د_که_پای
		إذا (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    صحيح جمع_کړه = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += جمع_کړه;
		    إذا (curwin->w_skipcol >= اندازه)
		    {
			إذا (lnum == curbuf->b_ml.ml_کرښه_شمېر)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= جمع_کړه;
			    اكسر;
			}
			++lnum;
		    }
		}
		وإلا
		{
		    إذا (lnum >= curbuf->b_ml.ml_کرښه_شمېر)
			اكسر;
		    ++lnum;
		}

		إذا (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#که_تعریف_شوی FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#د_که_پای
		    curwin->w_skipcol = 0;
		    إذا (todo > 1 && do_sms)
			اندازه = linetabsize_eol(curwin, curwin->w_topline);
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
	curwin->w_topline += کرښه_شمېر;
	curwin->w_botline += کرښه_شمېر;	// approximate w_botline
    }

    إذا (curwin->w_topline > curbuf->b_ml.ml_کرښه_شمېر)
	curwin->w_topline = curbuf->b_ml.ml_کرښه_شمېر;
    إذا (curwin->w_botline > curbuf->b_ml.ml_کرښه_شمېر + 1)
	curwin->w_botline = curbuf->b_ml.ml_کرښه_شمېر + 1;

#که_تعریف_شوی FEAT_DIFF
    check_topfill(curwin, FALSE);
#د_که_پای

#که_تعریف_شوی FEAT_FOLDING
    إذا (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#د_که_پای

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

    صحيح	    width1 = curwin->w_پلنوالی - curwin_ستون_off();
    إذا (width1 <= 0)
	أرجع;  // no text will be displayed

    صحيح	    width2 = width1 + curwin_ستون_off2();
    اوږد    so = get_scrolloff_ارزښت();
    صحيح	    scrolloff_ستون_شمېر = so == 0 ? 0 : width1 + (so - 1) * width2;
    صحيح	    scrolled = FALSE;
    صحيح	    لیکه = 0;
    صحيح	    overlap, ستون;

    validate_cheight();
    إذا (curwin->w_cline_لوړوالی == curwin->w_لوړوالی
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_لوړوالی)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	أرجع;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_پلنوالی - width2);
    طالما (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_ستون_شمېر)
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

    ستون = curwin->w_virtcol + scrolloff_ستون_شمېر;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    إذا (scrolloff_ستون_شمېر > 0)
    {
	صحيح اندازه = linetabsize_eol(curwin, curwin->w_topline);
	اندازه = width1 + width2 * ((اندازه - width1 + width2 - 1) / width2);
	طالما (ستون > اندازه)
	    ستون -= width2;
    }
    ستون -= curwin->w_skipcol;

    إذا (ستون >= width1)
    {
	ستون -= width1;
	++لیکه;
    }
    إذا (ستون > width2)
    {
	لیکه += ستون / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	ستون = ستون % width2;
    }
    إذا (لیکه >= curwin->w_لوړوالی)
    {
	إذا (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --لیکه;
	}
	إذا (لیکه >= curwin->w_لوړوالی)
	    curwin->w_skipcol += (لیکه - curwin->w_لوړوالی) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#که_تعریف_شوی FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    فراغ
check_topfill(
    win_T	*wp,
    صحيح		ښکته)	// when TRUE scroll down when not enough space
{
    صحيح		n;

    إذا (wp->w_topfill <= 0)
	أرجع;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    إذا (wp->w_topfill + n > wp->w_لوړوالی)
    {
	إذا (ښکته && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	وإلا
	{
	    wp->w_topfill = wp->w_لوړوالی - n;
	    إذا (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#د_که_پای

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    فراغ
scrolldown_clamp(فراغ)
{
    صحيح		پای_ټکی_لیکه;
#که_تعریف_شوی FEAT_DIFF
    صحيح		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#د_که_پای

    إذا (curwin->w_topline <= 1
#که_تعریف_شوی FEAT_DIFF
	    && !can_fill
#د_که_پای
	    )
	أرجع;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    پای_ټکی_لیکه = curwin->w_wrow;
#که_تعریف_شوی FEAT_DIFF
    إذا (can_fill)
	++پای_ټکی_لیکه;
    وإلا
	پای_ټکی_لیکه += plines_nofill(curwin->w_topline - 1);
#وإلا
    پای_ټکی_لیکه += plines(curwin->w_topline - 1);
#د_که_پای
    إذا (curwin->w_p_wrap && curwin->w_پلنوالی != 0)
    {
	validate_cheight();
	validate_virtcol();
	پای_ټکی_لیکه += curwin->w_cline_لوړوالی - 1 -
	    curwin->w_virtcol / curwin->w_پلنوالی;
    }
    إذا (پای_ټکی_لیکه < curwin->w_لوړوالی - get_scrolloff_ارزښت())
    {
#که_تعریف_شوی FEAT_DIFF
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
#د_که_پای
#که_تعریف_شوی FEAT_FOLDING
	(فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#د_که_پای
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
    صحيح	    پیل_لیکه;

    إذا (curwin->w_topline == curbuf->b_ml.ml_کرښه_شمېر
#که_تعریف_شوی FEAT_DIFF
	    && curwin->w_topfill == 0
#د_که_پای
	    )
	أرجع;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#که_تعریف_شوی FEAT_DIFF
    پیل_لیکه = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#وإلا
    پیل_لیکه = curwin->w_wrow - plines(curwin->w_topline);
#د_که_پای
    إذا (curwin->w_p_wrap && curwin->w_پلنوالی != 0)
    {
	validate_virtcol();
	پیل_لیکه -= curwin->w_virtcol / curwin->w_پلنوالی;
    }
    إذا (پیل_لیکه >= get_scrolloff_ارزښت())
    {
#که_تعریف_شوی FEAT_DIFF
	إذا (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	وإلا
#د_که_پای
	{
#که_تعریف_شوی FEAT_FOLDING
	    (فراغ)hasFolding(curwin->w_topline, عدم, &curwin->w_topline);
#د_که_پای
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
topline_شا_winheight(
    lineoff_T	*lp,
    صحيح		winheight)	// when TRUE limit to window height
{
#که_تعریف_شوی FEAT_DIFF
    إذا (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->لوړوالی = 1;
    }
    وإلا
#د_که_پای
    {
	--lp->lnum;
#که_تعریف_شوی FEAT_DIFF
	lp->fill = 0;
#د_که_پای
	إذا (lp->lnum < 1)
	    lp->لوړوالی = MAXCOL;
	وإلا
#که_تعریف_شوی FEAT_FOLDING
	    إذا (hasFolding(lp->lnum, &lp->lnum, عدم))
	    // Add a closed fold
	    lp->لوړوالی = 1;
	وإلا
#د_که_پای
	    lp->لوړوالی = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    ساكن فراغ
topline_شا(lineoff_T *lp)
{
    topline_شا_winheight(lp, TRUE);
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
#که_تعریف_شوی FEAT_DIFF
    إذا (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->لوړوالی = 1;
    }
    وإلا
#د_که_پای
    {
	++lp->lnum;
#که_تعریف_شوی FEAT_DIFF
	lp->fill = 0;
#د_که_پای
	إذا (lp->lnum > curbuf->b_ml.ml_کرښه_شمېر)
	    lp->لوړوالی = MAXCOL;
	وإلا
#که_تعریف_شوی FEAT_FOLDING
	    إذا (hasFolding(lp->lnum, عدم, &lp->lnum))
	    {
		// Add a closed fold.
		lp->لوړوالی = 1;
	    }
	    وإلا
#د_که_پای
	    lp->لوړوالی = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    فراغ
scroll_cursor_سر(صحيح min_scroll, صحيح always)
{
    صحيح		scrolled = 0;
    صحيح		extra = 0;
    صحيح		کارول_شوی;
    صحيح		i;
    linenr_T	سر_2;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    صحيح		old_skipcol = curwin->w_skipcol;
#که_تعریف_شوی FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#د_که_پای
    linenr_T	new_topline;
    صحيح		off = get_scrolloff_ارزښت();

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
    کارول_شوی = curwin->w_cline_لوړوالی; // includes filler lines above
    إذا (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = کارول_شوی;

#که_تعریف_شوی FEAT_FOLDING
    إذا (hasFolding(curwin->w_cursor.lnum, &سر_2, &bot))
    {
	--سر_2;
	++bot;
    }
    وإلا
#د_که_پای
    {
	سر_2 = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = سر_2 + 1;

#که_تعریف_شوی FEAT_DIFF
    // "used" already contains the number of filler lines above, don't add it
    // again.
    // Hide filler lines above cursor line by adding them to "extra".
    extra += diff_check_fill(curwin, curwin->w_cursor.lnum);
#د_که_پای

    /*
     * Check if the lines from "top" to "bot" fit in the window.  If they do,
     * set new_topline and advance "top" and "bot" to include more lines.
     */
    طالما (سر_2 > 0)
    {
#که_تعریف_شوی FEAT_FOLDING
	إذا (hasFolding(سر_2, &سر_2, عدم))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	وإلا
#د_که_پای
	    i = PLINES_NOFILL(سر_2);
	إذا (سر_2 < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	إذا ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    اكسر;

	کارول_شوی += i;
	إذا (extra + i <= off && bot < curbuf->b_ml.ml_کرښه_شمېر)
	{
#که_تعریف_شوی FEAT_FOLDING
	    إذا (hasFolding(bot, عدم, &bot))
		// count one logical line for a sequence of folded lines
		++کارول_شوی;
	    وإلا
#د_که_پای
		کارول_شوی += plines(bot);
	}
	إذا (کارول_شوی > curwin->w_لوړوالی)
	    اكسر;

	extra += i;
	new_topline = سر_2;
	--سر_2;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    إذا (کارول_شوی > curwin->w_لوړوالی)
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
#که_تعریف_شوی FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	إذا (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    إذا (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#د_که_پای
	إذا (curwin->w_topline != old_topline)
	    reset_skipcol();
	وإلا إذا (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    إذا (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		صحيح	width1 = curwin->w_پلنوالی - curwin_ستون_off();
		صحيح	width2 = width1 + curwin_ستون_off2();
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
#که_تعریف_شوی FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#د_که_پای
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
سېټ_تشه_لیکې(win_T *wp, صحيح کارول_شوی)
{
#که_تعریف_شوی FEAT_DIFF
    wp->w_filler_لیکې = 0;
#د_که_پای
    إذا (کارول_شوی == 0)
	wp->w_تشه_لیکې = 0;	// single line that doesn't fit
    وإلا
    {
	wp->w_تشه_لیکې = wp->w_لوړوالی - کارول_شوی;
#که_تعریف_شوی FEAT_DIFF
	إذا (wp->w_botline <= wp->w_بفر->b_ml.ml_کرښه_شمېر)
	{
	    wp->w_filler_لیکې = diff_check_fill(wp, wp->w_botline);
	    إذا (wp->w_تشه_لیکې > wp->w_filler_لیکې)
		wp->w_تشه_لیکې -= wp->w_filler_لیکې;
	    وإلا
	    {
		wp->w_filler_لیکې = wp->w_تشه_لیکې;
		wp->w_تشه_لیکې = 0;
	    }
	}
#د_که_پای
    }
}

/*
 * Recompute topline to put the cursor at the bottom of the window.
 * When scrolling scroll at least "min_scroll" lines.
 * If "set_topbot" is TRUE, set topline and botline first (for "zb").
 * This is messy stuff!!!
 */
    فراغ
scroll_cursor_bot(صحيح min_scroll, صحيح سېټ_topbot)
{
    صحيح		کارول_شوی;
    صحيح		scrolled = 0;
    صحيح		extra = 0;
    صحيح		i;
    linenr_T	کرښه_شمېر;
    linenr_T	old_topline = curwin->w_topline;
    صحيح		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#که_تعریف_شوی FEAT_DIFF
    صحيح		old_topfill = curwin->w_topfill;
    صحيح		fill_لاندې_کړکۍ;
#د_که_پای
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    صحيح		old_تشه_لیکې = curwin->w_تشه_لیکې;
    linenr_T	cln;		    // Cursor Line Number
    اوږد	so = get_scrolloff_ارزښت();
    صحيح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    إذا (سېټ_topbot)
    {
	کارول_شوی = 0;
	linenr_T cln_وروستی = cln;
#که_تعریف_شوی FEAT_FOLDING
	(فراغ)hasFolding(cln, عدم, &cln_وروستی);
#د_که_پای
	curwin->w_botline = cln_وروستی + 1;
	loff.lnum = cln_وروستی + 1;
#که_تعریف_شوی FEAT_DIFF
	loff.fill = 0;
#د_که_پای
	طالما (TRUE)
	{
	    topline_شا_winheight(&loff, FALSE);
	    إذا (loff.لوړوالی == MAXCOL)
		اكسر;
	    إذا (کارول_شوی + loff.لوړوالی > curwin->w_لوړوالی)
	    {
		إذا (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    إذا (کارول_شوی < curwin->w_لوړوالی)
		    {
			صحيح plines_دننه_واټن = کارول_شوی + loff.لوړوالی
							    - curwin->w_لوړوالی;
			کارول_شوی = curwin->w_لوړوالی;
#که_تعریف_شوی FEAT_DIFF
			curwin->w_topfill = loff.fill;
#د_که_پای
			curwin->w_topline = loff.lnum;
			curwin->w_skipcol = skipcol_from_plines(
							curwin, plines_دننه_واټن);
		    }
		}
		اكسر;
	    }
#که_تعریف_شوی FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#د_که_پای
	    curwin->w_topline = loff.lnum;
	    کارول_شوی += loff.لوړوالی;
	}

	سېټ_تشه_لیکې(curwin, کارول_شوی);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	إذا (curwin->w_topline != old_topline
#که_تعریف_شوی FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#د_که_پای
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
#که_تعریف_شوی FEAT_DIFF
    کارول_شوی = plines_nofill(cln);
#وإلا
    validate_cheight();
    کارول_شوی = curwin->w_cline_لوړوالی;
#د_که_پای

    إذا (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	صحيح upto_cursor = plines_win_ستون(curwin, cln, curwin->w_cursor.ستون);

	إذا (upto_cursor < کارول_شوی)
	    کارول_شوی = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    إذا (cln >= curwin->w_botline)
    {
	scrolled = کارول_شوی;
	إذا (cln == curwin->w_botline)
	    scrolled -= curwin->w_تشه_لیکې;
	إذا (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    صحيح سر_plines =
#که_تعریف_شوی FEAT_DIFF
			    plines_win_nofill
#وإلا
			    plines_win
#د_که_پای
					(curwin, curwin->w_topline, FALSE);
	    صحيح width1 = curwin->w_پلنوالی - curwin_ستون_off();

	    إذا (width1 > 0)
	    {
		صحيح width2 = width1 + curwin_ستون_off2();
		صحيح skip_کرښې = 0;

		// A similar formula is used in curs_columns().
		إذا (curwin->w_skipcol > width1)
		    skip_کرښې += (curwin->w_skipcol - width1) / width2 + 1;
		وإلا إذا (curwin->w_skipcol > 0)
		    skip_کرښې = 1;

		سر_plines -= skip_کرښې;
		إذا (سر_plines > curwin->w_لوړوالی)
		{
		    scrolled += (سر_plines - curwin->w_لوړوالی);
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
#که_تعریف_شوی FEAT_FOLDING
    إذا (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
#د_که_پای
    {
	loff.lnum = cln;
	boff.lnum = cln;
    }
#که_تعریف_شوی FEAT_DIFF
    loff.fill = 0;
    boff.fill = 0;
    fill_لاندې_کړکۍ = diff_check_fill(curwin, curwin->w_botline)
						      - curwin->w_filler_لیکې;
#د_که_پای

    طالما (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	إذا ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_کرښه_شمېر)
		&& loff.lnum <= curwin->w_botline
#که_تعریف_شوی FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_لاندې_کړکۍ)
#د_که_پای
		)
	    اكسر;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_شا(&loff);
	إذا (loff.لوړوالی == MAXCOL)
	    کارول_شوی = MAXCOL;
	وإلا
	    کارول_شوی += loff.لوړوالی;
	إذا (کارول_شوی > curwin->w_لوړوالی)
	    اكسر;
	إذا (loff.lnum >= curwin->w_botline
#که_تعریف_شوی FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_لاندې_کړکۍ)
#د_که_پای
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.لوړوالی;
	    إذا (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_تشه_لیکې;
	}

	إذا (boff.lnum < curbuf->b_ml.ml_کرښه_شمېر)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    کارول_شوی += boff.لوړوالی;
	    إذا (کارول_شوی > curwin->w_لوړوالی)
		اكسر;
	    إذا (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.لوړوالی;
		إذا (boff.lnum >= curwin->w_botline
#که_تعریف_شوی FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_لیکې)
#د_که_پای
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.لوړوالی;
		    إذا (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_تشه_لیکې;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    إذا (scrolled <= 0)
	کرښه_شمېر = 0;
    // more than a screenfull, don't scroll but redraw
    وإلا إذا (کارول_شوی > curwin->w_لوړوالی)
	کرښه_شمېر = کارول_شوی;
    // scroll minimal number of lines
    وإلا
    {
	کرښه_شمېر = 0;
#که_تعریف_شوی FEAT_DIFF
	boff.fill = curwin->w_topfill;
#د_که_پای
	boff.lnum = curwin->w_topline - 1;
	لكل (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.لوړوالی;
	    ++کرښه_شمېر;
	}
	إذا (i < scrolled)	// below curwin->w_botline, don't scroll
	    کرښه_شمېر = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    إذا (کرښه_شمېر >= curwin->w_لوړوالی && کرښه_شمېر > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    وإلا إذا (کرښه_شمېر > 0)
    {
	إذا (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	وإلا
	    scrollup(کرښه_شمېر, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    إذا (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && سېټ_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_تشه_لیکې = old_تشه_لیکې;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    إذا (سېټ_topbot)
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
#که_تعریف_شوی FEAT_DIFF
    صحيح		topfill = 0;
#د_که_پای
    صحيح		لاندې = 0;
    صحيح		کارول_شوی;
    lineoff_T	loff;
    lineoff_T	boff;
#که_تعریف_شوی FEAT_DIFF
    linenr_T	old_topline = curwin->w_topline;
#د_که_پای

#که_تعریف_شوی FEAT_PROP_POPUP
    // if the width changed this needs to be updated first
    may_update_popup_position();
#د_که_پای
    loff.lnum = boff.lnum = curwin->w_cursor.lnum;
#که_تعریف_شوی FEAT_FOLDING
    (فراغ)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#د_که_پای
#که_تعریف_شوی FEAT_DIFF
    کارول_شوی = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#وإلا
    کارول_شوی = plines(loff.lnum);
#د_که_پای
    topline = loff.lnum;

    صحيح want_لوړوالی;
    صحيح do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    إذا (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	إذا (atend)
	{
	    want_لوړوالی = (curwin->w_لوړوالی - کارول_شوی) / 2;
	    کارول_شوی = 0;
	}
	وإلا
	    want_لوړوالی = curwin->w_لوړوالی;
    }

    طالما (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	إذا (do_sms)
	{
	    topline_شا_winheight(&loff, FALSE);
	    إذا (loff.لوړوالی == MAXCOL)
		اكسر;
	    کارول_شوی += loff.لوړوالی;
	    إذا (!atend && boff.lnum < curbuf->b_ml.ml_کرښه_شمېر)
	    {
		botline_forw(&boff);
		کارول_شوی += boff.لوړوالی;
	    }
	    إذا (کارول_شوی > want_لوړوالی)
	    {
		إذا (کارول_شوی - loff.لوړوالی < want_لوړوالی)
		{
		    topline = loff.lnum;
#که_تعریف_شوی FEAT_DIFF
		    topfill = loff.fill;
#د_که_پای
		    skipcol = skipcol_from_plines(curwin, کارول_شوی - want_لوړوالی);
		}
		اكسر;
	    }
	    topline = loff.lnum;
#که_تعریف_شوی FEAT_DIFF
	    topfill = loff.fill;
#د_که_پای
	    استمر;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	صحيح بشپړ = FALSE;
	لكل (صحيح پړاو = 1; پړاو <= 2; ++پړاو)
	{
	    إذا (prefer_above ? (پړاو == 2 && لاندې < above)
			     : (پړاو == 1 && لاندې <= above))
	    {
		// add a line below the cursor
		إذا (boff.lnum < curbuf->b_ml.ml_کرښه_شمېر)
		{
		    botline_forw(&boff);
		    کارول_شوی += boff.لوړوالی;
		    إذا (کارول_شوی > curwin->w_لوړوالی)
		    {
			بشپړ = TRUE;
			اكسر;
		    }
		    لاندې += boff.لوړوالی;
		}
		وإلا
		{
		    ++لاندې;	    // count a "~" line
		    إذا (atend)
			++کارول_شوی;
		}
	    }

	    إذا (prefer_above ? (پړاو == 1 && لاندې >= above)
			     : (پړاو == 1 && لاندې > above))
	    {
		// add a line above the cursor
		topline_شا(&loff);
		إذا (loff.لوړوالی == MAXCOL)
		    کارول_شوی = MAXCOL;
		وإلا
		    کارول_شوی += loff.لوړوالی;
		إذا (کارول_شوی > curwin->w_لوړوالی)
		{
		    بشپړ = TRUE;
		    اكسر;
		}
		above += loff.لوړوالی;
		topline = loff.lnum;
#که_تعریف_شوی FEAT_DIFF
		topfill = loff.fill;
#د_که_پای
	    }
	}
	إذا (بشپړ)
	    اكسر;
    }

#که_تعریف_شوی FEAT_FOLDING
    إذا (!hasFolding(topline, &curwin->w_topline, عدم))
#د_که_پای
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
#که_تعریف_شوی FEAT_DIFF
    curwin->w_topfill = topfill;
    إذا (old_topline > curwin->w_topline + curwin->w_لوړوالی)
	curwin->w_botfill = false;
    check_topfill(curwin, FALSE);
#د_که_پای
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
    صحيح		لاندې = 0;	    // screen lines below botline
    linenr_T	botline;
    صحيح		above_wanted, لاندې_wanted;
    linenr_T	cln;		    // Cursor Line Number
    صحيح		max_off;
    اوږد	so = get_scrolloff_ارزښت();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    لاندې_wanted = so;
    إذا (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	لاندې_wanted = mouse_dragging - 1;
    }
    إذا (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_لوړوالی / 2;
	إذا (لاندې_wanted > max_off)
	    لاندې_wanted = max_off;
    }
    validate_botline();
    إذا (curwin->w_botline == curbuf->b_ml.ml_کرښه_شمېر + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	لاندې_wanted = 0;
	max_off = (curwin->w_لوړوالی - 1) / 2;
	إذا (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    إذا (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - لاندې_wanted
#که_تعریف_شوی FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#د_که_پای
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
#که_تعریف_شوی FEAT_DIFF
    // count filler lines as context
    above = curwin->w_topfill;
    لاندې = curwin->w_filler_لیکې;
#د_که_پای
    طالما ((above < above_wanted || لاندې < لاندې_wanted) && topline < botline)
    {
	إذا (لاندې < لاندې_wanted && (لاندې <= above || above >= above_wanted))
	{
#که_تعریف_شوی FEAT_FOLDING
	    إذا (hasFolding(botline, &botline, عدم))
		++لاندې;
	    وإلا
#د_که_پای
		لاندې += plines(botline);
	    --botline;
	}
	إذا (above < above_wanted && (above < لاندې || لاندې >= لاندې_wanted))
	{
#که_تعریف_شوی FEAT_FOLDING
	    إذا (hasFolding(topline, عدم, &topline))
		++above;
	    وإلا
#د_که_پای
		above += PLINES_NOFILL(topline);
#که_تعریف_شوی FEAT_DIFF
	    // Count filler lines below this line as context.
	    إذا (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#د_که_پای
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
	إذا (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_کرښه_شمېر)
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
    صحيح	    min_لوړوالی = curwin->w_لوړوالی - 2;

    validate_botline();
    إذا ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_کرښه_شمېر))
	أرجع min_لوړوالی + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#که_تعریف_شوی FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_لیکې : curwin->w_topfill);
    loff.لوړوالی = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#وإلا
    loff.لوړوالی = plines(loff.lnum);
#د_که_پای

    صحيح h1 = loff.لوړوالی;
    إذا (h1 > min_لوړوالی)
	أرجع min_لوړوالی + 2;  // no overlap
    إذا (dir == FORWARD)
	topline_شا(&loff);
    وإلا
	botline_forw(&loff);

    صحيح h2 = loff.لوړوالی;
    إذا (h2 == MAXCOL || h2 + h1 > min_لوړوالی)
	أرجع min_لوړوالی + 2;  // no overlap
    إذا (dir == FORWARD)
	topline_شا(&loff);
    وإلا
	botline_forw(&loff);

    صحيح h3 = loff.لوړوالی;
    إذا (h3 == MAXCOL || h3 + h2 > min_لوړوالی)
	أرجع min_لوړوالی + 2;  // no overlap
    إذا (dir == FORWARD)
	topline_شا(&loff);
    وإلا
	botline_forw(&loff);

    صحيح h4 = loff.لوړوالی;
    إذا (h4 == MAXCOL || h4 + h3 + h2 > min_لوړوالی || h3 + h2 + h1 > min_لوړوالی)
	أرجع min_لوړوالی + 1;  // 1 line overlap
    وإلا
	أرجع min_لوړوالی;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
ساكن صحيح scroll_with_sms(صحيح dir, اوږد شمېر, اوږد *curscount)
{
    صحيح		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#که_تعریف_شوی FEAT_DIFF
    صحيح		prev_topfill = curwin->w_topfill;
#د_که_پای

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, شمېر);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    إذا (!prev_sms && curwin->w_skipcol > 0)
    {
	صحيح fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	إذا (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	صحيح width1 = curwin->w_پلنوالی - curwin_ستون_off();
	صحيح width2 = width1 + curwin_ستون_off2();
	شمېر = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	إذا (fixdir == FORWARD)
	    شمېر = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, شمېر);
	*curscount += شمېر * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    أرجع curwin->w_topline != prev_topline
#که_تعریف_شوی FEAT_DIFF
	|| curwin->w_topfill != prev_topfill
#د_که_پای
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
pagescroll(صحيح dir, اوږد شمېر, صحيح half)
{
    صحيح		did_move = FALSE;
    صحيح		buflen = curbuf->b_ml.ml_کرښه_شمېر;
    colnr_T	prev_ستون = curwin->w_cursor.ستون;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    إذا (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	إذا (شمېر)
	    curwin->w_p_scr = MIN(curwin->w_لوړوالی, شمېر);
	شمېر = MIN(curwin->w_لوړوالی, curwin->w_p_scr);

	اوږد curscount = شمېر;
	// Adjust count so as to not reveal end of buffer lines.
	إذا (dir == FORWARD
		    && (curwin->w_topline + curwin->w_لوړوالی + شمېر > buflen
#که_تعریف_شوی FEAT_FOLDING
							|| hasAnyFolding(curwin)
#د_که_پای
	   ))
	{
	    صحيح n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    إذا (n - شمېر < curwin->w_لوړوالی && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_لوړوالی + شمېر);
	    إذا (n < curwin->w_لوړوالی + شمېر)
		شمېر = n - curwin->w_لوړوالی;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	إذا (شمېر > 0)
	{
	    did_move = scroll_with_sms(dir, شمېر, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.ستون = prev_ستون;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	إذا (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	وإلا إذا (dir == FORWARD)
	    cursor_ښکته_inner(curwin, curscount);
	وإلا
	    cursor_پورته_inner(curwin, curscount);
    }
    وإلا
    {
	// Scroll [count] times 'window' or current window height lines.
	شمېر *= ((ONE_کړکۍ && p_کړکۍ > 0 && p_کړکۍ < لیکې - 1) ?
				MAX(1, p_کړکۍ - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, شمېر, &شمېر);

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

    إذا (get_scrolloff_ارزښت() > 0)
	cursor_correct();
#که_تعریف_شوی FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#د_که_پای
    did_move = did_move
	|| prev_ستون != curwin->w_cursor.ستون
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

    linenr_T	کرښه = curwin->w_cursor.lnum;
    colnr_T	ستون = curwin->w_cursor.ستون;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    صحيح		سېټ_curswant = curwin->w_سېټ_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    صحيح		old_VIsual_select = VIsual_select;
    صحيح		old_VIsual_فعال = VIsual_فعال;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_فعال = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_بفر;
	// skip original window and windows with 'nocursorbind'
	إذا (curwin != old_curwin && curwin->w_p_crb)
	{
#که_تعریف_شوی FEAT_DIFF
	    إذا (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_کرښه(old_curbuf, کرښه);
	    وإلا
#د_که_پای
		curwin->w_cursor.lnum = کرښه;
	    curwin->w_cursor.ستون = ستون;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_سېټ_curswant = سېټ_curswant;

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
    VIsual_فعال = old_VIsual_فعال;
    curwin = old_curwin;
    curbuf = old_curbuf;
}
