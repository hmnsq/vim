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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ ਖਾਲੀ redraw_for_cursorline(win_T *wp);
ਸਥਿਰ ਪੂਰਨ scrolljump_ਮੁੱਲ(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ check_ਸਿਖਰ_ਵਿੱਥ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ curs_ਪੰਕਤੀਆਂ(win_T *wp);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    linenr_T	    lnum;	// line number
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		    fill;	// filler lines
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		    ਉਚਾਈ;	// height of added line
} lineoff_T;

ਸਥਿਰ ਖਾਲੀ topline_ਪਿੱਠ(lineoff_T *lp);
ਸਥਿਰ ਖਾਲੀ botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    ਸਥਿਰ ਪੂਰਨ
adjust_plines_for_skipcol(win_T *wp)
{
    ਜੇ (wp->w_skipcol == 0)
	ਵਾਪਸ 0;

    ਪੂਰਨ ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp);
    ਪੂਰਨ w2 = ਚੌੜਾਈ + win_ਖਾਨਾ_off2(wp);
    ਜੇ (wp->w_skipcol >= ਚੌੜਾਈ && w2 > 0)
	ਵਾਪਸ (wp->w_skipcol - ਚੌੜਾਈ) / w2 + 1;

    ਵਾਪਸ 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    ਪੂਰਨ
plines_correct_topline(win_T *wp, linenr_T lnum, ਪੂਰਨ ਹੱਦਬੰਦੀ_winheight)
{
    ਪੂਰਨ n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	n = plines_win(wp, lnum, FALSE);
    ਜੇ (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    ਜੇ (ਹੱਦਬੰਦੀ_winheight && n > wp->w_ਉਚਾਈ)
	n = wp->w_ਉਚਾਈ;
    ਵਾਪਸ n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    ਸਥਿਰ ਖਾਲੀ
comp_botline(win_T *wp)
{
    ਪੂਰਨ		n;
    linenr_T	lnum;
    ਪੂਰਨ		ਪੂਰਾ;
    ਪੂਰਨ		i = 0;
    ਪੂਰਨ		use_cache;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    linenr_T    ਆਖਰੀ;
    ਪੂਰਨ		folded;
#ਜੇ_ਅੰਤ

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
		    && !wp->w_ਬਫਰ->b_mod_ਸੈੱਟ
		    && dollar_vcol == -1
		    && wp->w_skipcol == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    && !wp->w_p_diff
#ਜੇ_ਅੰਤ
		    && wp->w_ਸਤਰਾਂ_valid > 0
		    && wp->w_ਸਤਰਾਂ[0].wl_lnum <= wp->w_topline;

    ਜੇ (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	ਪੂਰਾ = wp->w_cline_ਪੰਕਤੀ;
    }
    ਨਹੀਂਤਾਂ
    {
	lnum = wp->w_topline;
	ਪੂਰਾ = 0;
    }

    // Find the w_lines[] entry for the starting line.
    ਜੇ (use_cache)
	ਜਦੋਂ (i < wp->w_ਸਤਰਾਂ_valid && wp->w_ਸਤਰਾਂ[i].wl_lnum < lnum)
	    ++i;

    ਲਈ ( ; lnum <= wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++i)
    {
	ਪੂਰਨ	valid = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਆਖਰੀ = lnum;
	folded = FALSE;
#ਜੇ_ਅੰਤ
	// Try to use the size from the previous redraw.
	ਜੇ (use_cache && i < wp->w_ਸਤਰਾਂ_valid)
	{
	    ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lnum < lnum || !wp->w_ਸਤਰਾਂ[i].wl_valid)
		ਜਾਰੀ;		// skip changed or deleted lines
	    ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lnum == lnum)
		valid = TRUE;
	    ਨਹੀਂਤਾਂ // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	ਜੇ (valid)
	{
	    n = wp->w_ਸਤਰਾਂ[i].wl_ਮਾਪ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    folded = wp->w_ਸਤਰਾਂ[i].wl_folded;
	    ਆਖਰੀ = wp->w_ਸਤਰਾਂ[i].wl_lastlnum;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (hasFoldingWin(wp, lnum, ਨਿਰਾ, &ਆਖਰੀ, TRUE, ਨਿਰਾ))
	    {
		n = 1;
		folded = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && ਆਖਰੀ >= wp->w_cursor.lnum
#ਨਹੀਂਤਾਂ
		lnum == wp->w_cursor.lnum
#ਜੇ_ਅੰਤ
	   )
	{
	    wp->w_cline_ਪੰਕਤੀ = ਪੂਰਾ;
	    wp->w_cline_ਉਚਾਈ = n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    wp->w_cline_folded = folded;
#ਜੇ_ਅੰਤ
	    redraw_for_cursorline(wp);
	    wp->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	ਜੇ (ਪੂਰਾ + n > wp->w_ਉਚਾਈ)
	    ਰੋਕੋ;
	ਪੂਰਾ += n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	lnum = ਆਖਰੀ + 1;
#ਨਹੀਂਤਾਂ
	++lnum;
#ਜੇ_ਅੰਤ
    }

    // wp->w_botline is the line that is just below the window
    wp->w_botline = lnum;
    wp->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    ਸੈੱਟ_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ(wp, ਪੂਰਾ);
}

/*
 * Redraw when w_cline_row changes and 'relativenumber' or 'cursorline' is
 * set.
 */
    ਸਥਿਰ ਖਾਲੀ
redraw_for_cursorline(win_T *wp)
{
    ਜੇ ((wp->w_p_rnu
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		|| wp->w_p_cul
#ਜੇ_ਅੰਤ
		)
	    && (wp->w_valid & VALID_CROW) == 0
	    && !pum_visible())
    {
	// win_line() will redraw the number column and cursorline only.
	redraw_win_later(wp, UPD_VALID);
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
/*
 * Redraw when w_virtcol changes and 'cursorcolumn' is set or 'cursorlineopt'
 * contains "screenline".
 */
    ਸਥਿਰ ਖਾਲੀ
redraw_for_cursorcolumn(win_T *wp)
{
    ਜੇ ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	ਜੇ (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    ਖਾਲੀ
ਸੈੱਟ_valid_virtcol(win_T *wp, colnr_T vcol)
{
    wp->w_virtcol = vcol;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#ਜੇ_ਅੰਤ
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
    ਪੂਰਨ
sms_marker_overlap(win_T *wp, ਪੂਰਨ extra2)
{
    ਜੇ (extra2 == -1)
	extra2 = win_ਖਾਨਾ_off(wp) - win_ਖਾਨਾ_off2(wp);
#ਜੇ defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    ਜੇ (*get_showbreak_ਮੁੱਲ(wp) != NUL)
	ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    ਜੇ (wp->w_p_list && wp->w_lcs_chars.prec)
	ਵਾਪਸ 1;

    ਵਾਪਸ extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    ਸਥਿਰ ਪੂਰਨ
skipcol_from_plines(win_T *wp, ਪੂਰਨ plines_off)
{
    ਪੂਰਨ width1 = wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp);

    ਪੂਰਨ skipcol = 0;
    ਜੇ (plines_off > 0)
	skipcol += width1;
    ਜੇ (plines_off > 1)
	skipcol += (width1 + win_ਖਾਨਾ_off2(wp)) * (plines_off - 1);
    ਵਾਪਸ skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    ਸਥਿਰ ਖਾਲੀ
reset_skipcol(ਖਾਲੀ)
{
    ਜੇ (curwin->w_skipcol == 0)
	ਵਾਪਸ;

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
    ਖਾਲੀ
update_topline_redraw(ਖਾਲੀ)
{
    update_topline();
    ਜੇ (must_redraw)
	update_screen(0);
}

/*
 * Return true when 'scrolloffpad' may augment 'scrolloff'.
 * This only applies to automatic cursor visibility correction.
 * For now 'scrolloffpad' is treated as boolean: 0 disables, > 0 enables.
 */
    ਸਥਿਰ bool
use_scrolloffpad(ਖਾਲੀ)
{
    ਵਾਪਸ get_scrolloff_ਮੁੱਲ() > 0 && get_scrolloffpad_ਮੁੱਲ() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    ਸਥਿਰ bool
scrolloffpad_eof_pressure(linenr_T lnum, ਲੰਮਾ so)
{
    ਜੇ (!use_scrolloffpad() || so <= 0)
	ਵਾਪਸ false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    ਵਾਪਸ lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    ਖਾਲੀ
update_topline(ਖਾਲੀ)
{
    ਲੰਮਾ	ਸਤਰ_ਗਿਣਤੀ;
    ਪੂਰਨ		halfheight;
    ਪੂਰਨ		n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    linenr_T	lnum;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		check_topline = FALSE;
    ਪੂਰਨ		check_botline = FALSE;
    ਲੰਮਾ	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    ਪੂਰਨ		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    ਜੇ (skip_update_topline)
	ਵਾਪਸ;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    ਜੇ (!screen_valid(TRUE) || curwin->w_ਉਚਾਈ == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	ਵਾਪਸ;
    }

    check_cursor_moved(curwin);
    ਜੇ (curwin->w_valid & VALID_TOPLINE)
	ਵਾਪਸ;

    // When dragging with the mouse, don't scroll that quickly
    ਜੇ (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ old_topfill = curwin->w_topfill;
#ਜੇ_ਅੰਤ

    /*
     * If the buffer is empty, always set topline to 1.
     */
    ਜੇ (BUFEMPTY())		// special case - file is empty
    {
	ਜੇ (curwin->w_topline != 1)
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
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    ਜੇ (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    ਨਹੀਂਤਾਂ ਜੇ (check_ਸਿਖਰ_ਵਿੱਥ())
		check_topline = TRUE;
	    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		ਪੂਰਨ overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, ਨਿਰਾ, ਨਿਰਾ, 0);
		overlap = sms_marker_overlap(curwin, -1);
		ਜੇ (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	ਜੇ (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#ਜੇ_ਅੰਤ

	ਜੇ (check_topline)
	{
	    halfheight = curwin->w_ਉਚਾਈ / 2 - 1;
	    ਜੇ (halfheight < 2)
		halfheight = 2;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		ਲਈ (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    ਜੇ (lnum >= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ || n >= halfheight)
			ਰੋਕੋ;
		    (ਖਾਲੀ)hasFolding(lnum, ਨਿਰਾ, &lnum);
		}
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    ਪੂਰਨ min_scroll = scrolljump_ਮੁੱਲ();
	    ਜੇ (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    ਨਹੀਂਤਾਂ ਜੇ (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    ਨਹੀਂਤਾਂ
	    {
		scroll_cursor_ਸਿਖਰ(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (ਖਾਲੀ)hasFolding(curwin->w_topline, &curwin->w_topline, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
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
    ਜੇ (check_botline)
    {
	ਜੇ (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	ਜੇ (curwin->w_botline <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ || use_scrolloffpad())
	{
	    ਜੇ (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		ਜੇ (((ਲੰਮਾ)curwin->w_cursor.lnum
					   >= (ਲੰਮਾ)curwin->w_botline - *so_ptr
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			|| hasAnyFolding(curwin)
#ਜੇ_ਅੰਤ
			))
		{
		    lineoff_T	loff;

		    // Cursor is (a few lines) above botline, check if there
		    // are 'scrolloff' window lines below the cursor.
		    // If not, need to scroll.
		    n = eof_pressure ? 0 : curwin->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
		    loff.lnum = curwin->w_cursor.lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		    // In a fold go to its last line.
		    (ਖਾਲੀ)hasFolding(loff.lnum, ਨਿਰਾ, &loff.lnum);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_ਪੰਕਤੀਆਂ;
#ਜੇ_ਅੰਤ
		    loff.ਉਚਾਈ = 0;
		    ਜਦੋਂ (loff.lnum < curwin->w_botline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#ਜੇ_ਅੰਤ
			    )
		    {
			n += loff.ਉਚਾਈ;
			ਜੇ (n >= *so_ptr)
			    ਰੋਕੋ;
			botline_forw(&loff);
		    }
		    ਜੇ (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		ਨਹੀਂਤਾਂ
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    ਜੇ (check_botline)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		ਜੇ (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    ਸਤਰ_ਗਿਣਤੀ = 0;
		    ਲਈ (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++ਸਤਰ_ਗਿਣਤੀ;
			// stop at end of file or when we know we are far off
			ਜੇ (lnum <= 0 || ਸਤਰ_ਗਿਣਤੀ > curwin->w_ਉਚਾਈ + 1)
			    ਰੋਕੋ;
			(ਖਾਲੀ)hasFolding(lnum, &lnum, ਨਿਰਾ);
		    }
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    ਸਤਰ_ਗਿਣਤੀ = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		ਜੇ (ਸਤਰ_ਗਿਣਤੀ <= curwin->w_ਉਚਾਈ + 1)
		{
		    ਜੇ (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    ਨਹੀਂਤਾਂ
			scroll_cursor_bot(scrolljump_ਮੁੱਲ(), FALSE);
		}
		ਨਹੀਂਤਾਂ
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    ਜੇ (curwin->w_topline != old_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#ਜੇ_ਅੰਤ
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	ਜੇ (!curwin->w_p_sms)
	    reset_skipcol();
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	ਜੇ (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    ਸਥਿਰ ਪੂਰਨ
scrolljump_ਮੁੱਲ(ਖਾਲੀ)
{
    ਜੇ (p_sj >= 0)
	ਵਾਪਸ (ਪੂਰਨ)p_sj;
    ਵਾਪਸ (curwin->w_ਉਚਾਈ * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    ਸਥਿਰ ਪੂਰਨ
check_ਸਿਖਰ_ਵਿੱਥ(ਖਾਲੀ)
{
    lineoff_T	loff;
    ਪੂਰਨ		n;
    ਲੰਮਾ	so = get_scrolloff_ਮੁੱਲ();

    ਜੇ (curwin->w_cursor.lnum < curwin->w_topline + so
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		    || hasAnyFolding(curwin)
#ਜੇ_ਅੰਤ
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#ਨਹੀਂਤਾਂ
	n = 0;
#ਜੇ_ਅੰਤ
	// Count the visible screen lines above the cursor line.
	ਜਦੋਂ (n < so)
	{
	    topline_ਪਿੱਠ(&loff);
	    // Stop when included a line above the window.
	    ਜੇ (loff.lnum < curwin->w_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#ਜੇ_ਅੰਤ
		    )
		ਰੋਕੋ;
	    n += loff.ਉਚਾਈ;
	}
	ਜੇ (n < so)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Update w_curswant.
 */
    ਖਾਲੀ
update_curswant_force(ਖਾਲੀ)
{
    validate_virtcol();
    curwin->w_curswant = curwin->w_virtcol
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	- curwin->w_virtcol_ਪਹਿਲਾ_char
#ਜੇ_ਅੰਤ
	;
    curwin->w_ਸੈੱਟ_curswant = false;
}

/*
 * Update w_curswant if w_set_curswant is set.
 */
    ਖਾਲੀ
update_curswant(ਖਾਲੀ)
{
    ਜੇ (curwin->w_ਸੈੱਟ_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    ਖਾਲੀ
check_cursor_moved(win_T *wp)
{
    ਜੇ (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_cursor.ਖਾਨਾ != wp->w_valid_cursor.ਖਾਨਾ
	     || wp->w_leftcol != wp->w_valid_leftcol
	     || wp->w_cursor.coladd != wp->w_valid_cursor.coladd)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
	wp->w_valid_cursor.ਖਾਨਾ = wp->w_cursor.ਖਾਨਾ;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_cursor.coladd = wp->w_cursor.coladd;
    }
}

/*
 * Call this function when some window settings have changed, which require
 * the cursor position, botline and topline to be recomputed and the window to
 * be redrawn.  E.g, when changing the 'wrap' option or folding.
 */
    ਖਾਲੀ
ਬਦਲਿਆ_ਖਿੜਕੀ_setting(ਖਾਲੀ)
{
    ਬਦਲਿਆ_ਖਿੜਕੀ_setting_win(curwin);
}

    ਖਾਲੀ
ਬਦਲਿਆ_ਖਿੜਕੀ_setting_win(win_T *wp)
{
    wp->w_ਸਤਰਾਂ_valid = 0;
    ਬਦਲਿਆ_ਸਤਰ_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    ਖਾਲੀ
ਬਦਲਿਆ_ਖਿੜਕੀ_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ਜੇ (wp->w_ਬਫਰ == buf)
	    ਬਦਲਿਆ_ਖਿੜਕੀ_setting_win(wp);
}
#ਜੇ_ਅੰਤ

/*
 * Call changed_window_setting_win() for every window.
 */
    ਖਾਲੀ
ਬਦਲਿਆ_ਖਿੜਕੀ_setting_all(ਖਾਲੀ)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ਬਦਲਿਆ_ਖਿੜਕੀ_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    ਖਾਲੀ
ਸੈੱਟ_topline(win_T *wp, linenr_T lnum)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // go to first of folded lines
    (ਖਾਲੀ)hasFoldingWin(wp, lnum, &lnum, ਨਿਰਾ, TRUE, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    ਜੇ (wp->w_botline > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	wp->w_botline = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_ਸੈੱਟ = true;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (lnum != prev_topline)
	// Keep the filler lines when the topline didn't change.
	wp->w_topfill = 0;
#ਜੇ_ਅੰਤ
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
    ਖਾਲੀ
ਬਦਲਿਆ_cline_bef_curs(ਖਾਲੀ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    ਖਾਲੀ
ਬਦਲਿਆ_cline_bef_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

/*
 * Call this function when the length of a line (in screen characters) above
 * the cursor have changed.
 * Need to take care of w_botline separately!
 */
    ਖਾਲੀ
ਬਦਲਿਆ_ਸਤਰ_abv_curs(ਖਾਲੀ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    ਖਾਲੀ
ਬਦਲਿਆ_ਸਤਰ_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    ਖਾਲੀ
ਬਦਲਿਆ_ਸਤਰ_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_ਬਫਰ == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#ਜੇ_ਅੰਤ

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    ਖਾਲੀ
validate_botline(ਖਾਲੀ)
{
    validate_botline_win(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    ਖਾਲੀ
validate_botline_win(win_T *wp)
{
    ਜੇ (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    ਖਾਲੀ
invalidate_botline(ਖਾਲੀ)
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    ਖਾਲੀ
invalidate_botline_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    ਖਾਲੀ
approximate_botline_win(
    win_T	*wp)
{
    wp->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    ਪੂਰਨ
cursor_valid(ਖਾਲੀ)
{
    check_cursor_moved(curwin);
    ਵਾਪਸ ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    ਖਾਲੀ
validate_cursor(ਖਾਲੀ)
{
    check_cursor_lnum();
    check_cursor_moved(curwin);
    ਜੇ ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#ਜੇ defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    ਖਾਲੀ
validate_cline_ਪੰਕਤੀ(ਖਾਲੀ)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    ਜੇ (!(curwin->w_valid & VALID_CROW))
	curs_ਪੰਕਤੀਆਂ(curwin);
}
#ਜੇ_ਅੰਤ

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    ਸਥਿਰ ਖਾਲੀ
curs_ਪੰਕਤੀਆਂ(win_T *wp)
{
    linenr_T	lnum;
    ਪੂਰਨ		i;
    ਪੂਰਨ		all_invalid;
    ਪੂਰਨ		valid;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਲੰਮਾ	fold_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_ਸਤਰਾਂ_valid == 0
			|| wp->w_ਸਤਰਾਂ[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_ਪੰਕਤੀ = 0;
    ਲਈ (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	ਜੇ (!all_invalid && i < wp->w_ਸਤਰਾਂ_valid)
	{
	    ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lnum < lnum || !wp->w_ਸਤਰਾਂ[i].wl_valid)
		ਜਾਰੀ;		// skip changed or deleted lines
	    ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lnum == lnum)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		ਜੇ (!wp->w_ਬਫਰ->b_mod_ਸੈੱਟ
			|| wp->w_ਸਤਰਾਂ[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_ਬਫਰ->b_mod_ਸਿਖਰ
					     > wp->w_ਸਤਰਾਂ[i].wl_lastlnum + 1)
#ਜੇ_ਅੰਤ
		valid = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	ਜੇ (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
					    && !wp->w_p_diff
#ਜੇ_ਅੰਤ
	       )))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    lnum = wp->w_ਸਤਰਾਂ[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    ਜੇ (lnum > wp->w_cursor.lnum)
		ਰੋਕੋ;
#ਨਹੀਂਤਾਂ
	    ++lnum;
#ਜੇ_ਅੰਤ
	    wp->w_cline_ਪੰਕਤੀ += wp->w_ਸਤਰਾਂ[i].wl_ਮਾਪ;
	}
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    fold_ਗਿਣਤੀ = folded_ਗਿਣਤੀ(wp, lnum, ਨਿਰਾ);
	    ਜੇ (fold_ਗਿਣਤੀ)
	    {
		lnum += fold_ਗਿਣਤੀ;
		ਜੇ (lnum > wp->w_cursor.lnum)
		    ਰੋਕੋ;
		++wp->w_cline_ਪੰਕਤੀ;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		wp->w_cline_ਪੰਕਤੀ += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    ਜੇ (!(wp->w_valid & VALID_CHEIGHT))
    {
	ਜੇ (all_invalid
		|| i == wp->w_ਸਤਰਾਂ_valid
		|| (i < wp->w_ਸਤਰਾਂ_valid
		    && (!wp->w_ਸਤਰਾਂ[i].wl_valid
			|| wp->w_ਸਤਰਾਂ[i].wl_lnum != wp->w_cursor.lnum)))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    ਜੇ (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_ਉਚਾਈ = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		wp->w_cline_ਉਚਾਈ = plines_win(wp, wp->w_cursor.lnum, TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ਨਿਰਾ, ਨਿਰਾ, TRUE, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (i > wp->w_ਸਤਰਾਂ_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_ਉਚਾਈ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ਨਿਰਾ, ਨਿਰਾ, TRUE, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    wp->w_cline_ਉਚਾਈ = wp->w_ਸਤਰਾਂ[i].wl_ਮਾਪ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    wp->w_cline_folded = wp->w_ਸਤਰਾਂ[i].wl_folded;
#ਜੇ_ਅੰਤ
	}
    }

    redraw_for_cursorline(curwin);
    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;
}

/*
 * Validate curwin->w_virtcol only.
 */
    ਖਾਲੀ
validate_virtcol(ਖਾਲੀ)
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    ਖਾਲੀ
validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);

    ਜੇ (wp->w_valid & VALID_VIRTCOL)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    wp->w_virtcol_ਪਹਿਲਾ_char = 0;
#ਜੇ_ਅੰਤ
    getvvcol(wp, &wp->w_cursor, ਨਿਰਾ, &(wp->w_virtcol), ਨਿਰਾ,
							 GETVCOL_FOR_VIRTCOL);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#ਜੇ_ਅੰਤ
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    ਖਾਲੀ
validate_cheight(ਖਾਲੀ)
{
    check_cursor_moved(curwin);

    ਜੇ (curwin->w_valid & VALID_CHEIGHT)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_ਉਚਾਈ = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	curwin->w_cline_ਉਚਾਈ = plines(curwin->w_cursor.lnum);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, ਨਿਰਾ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    ਖਾਲੀ
validate_cursor_ਖਾਨਾ(ਖਾਲੀ)
{
    colnr_T off;
    colnr_T ਖਾਨਾ;
    ਪੂਰਨ     ਚੌੜਾਈ;

    validate_virtcol();

    ਜੇ (curwin->w_valid & VALID_WCOL)
	ਵਾਪਸ;

    ਖਾਨਾ = curwin->w_virtcol;
    off = curwin_ਖਾਨਾ_off();
    ਖਾਨਾ += off;
    ਚੌੜਾਈ = curwin->w_ਚੌੜਾਈ - off + curwin_ਖਾਨਾ_off2();

    // long line wrapping, adjust curwin->w_wrow
    ਜੇ (curwin->w_p_wrap
	    && ਖਾਨਾ >= (colnr_T)curwin->w_ਚੌੜਾਈ
	    && ਚੌੜਾਈ > 0)
	// use same formula as what is used in curs_columns()
	ਖਾਨਾ -= ((ਖਾਨਾ - curwin->w_ਚੌੜਾਈ) / ਚੌੜਾਈ + 1) * ਚੌੜਾਈ;
    ਜੇ (ਖਾਨਾ > (ਪੂਰਨ)curwin->w_leftcol)
	ਖਾਨਾ -= curwin->w_leftcol;
    ਨਹੀਂਤਾਂ
	ਖਾਨਾ = 0;
    curwin->w_wcol = ਖਾਨਾ;

    curwin->w_valid |= VALID_WCOL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    curwin->w_flags &= ~WFLAG_WCOL_OFF_ADDED;
#ਜੇ_ਅੰਤ
}

/*
 * Compute offset of a window, occupied by absolute or relative line number,
 * fold column and sign column (these don't move when scrolling horizontally).
 */
    ਪੂਰਨ
win_ਖਾਨਾ_off(win_T *wp)
{
    ਵਾਪਸ (((wp->w_p_nu || wp->w_p_rnu) ? ਸੰਖਿਆ_ਚੌੜਾਈ(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    + wp->w_p_fdc
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#ਜੇ_ਅੰਤ
	   );
}

    ਪੂਰਨ
curwin_ਖਾਨਾ_off(ਖਾਲੀ)
{
    ਵਾਪਸ win_ਖਾਨਾ_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    ਪੂਰਨ
win_ਖਾਨਾ_off2(win_T *wp)
{
    ਜੇ ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != ਨਿਰਾ)
	ਵਾਪਸ ਸੰਖਿਆ_ਚੌੜਾਈ(wp) + 1;
    ਵਾਪਸ 0;
}

    ਪੂਰਨ
curwin_ਖਾਨਾ_off2(ਖਾਲੀ)
{
    ਵਾਪਸ win_ਖਾਨਾ_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    ਖਾਲੀ
curs_columns(
    ਪੂਰਨ		may_scroll)	// when TRUE, may scroll horizontally
{
    ਲੰਮਾ	diff;
    ਪੂਰਨ		extra;		// offset for first screen line
    ਲੰਮਾ	off_ਖੱਬਾ, off_ਸੱਜਾ;
    ਪੂਰਨ		n;
    ਪੂਰਨ		p_ਸਤਰਾਂ;
    ਪੂਰਨ		width1;		// text width for first screen line
    ਪੂਰਨ		width2 = 0;	// text width for second and later screen line
    ਪੂਰਨ		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    ਲੰਮਾ	so = get_scrolloff_ਮੁੱਲ();
    ਲੰਮਾ	siso = get_sidescrolloff_ਮੁੱਲ();
    ਪੂਰਨ		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    ਜੇ (!(curwin->w_valid & VALID_CROW))
	curs_ਪੰਕਤੀਆਂ(curwin);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_ਪਹਿਲਾ_char = 0;
#ਜੇ_ਅੰਤ

    /*
     * Compute the number of virtual columns.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    ਜੇ (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_ਖਾਨਾ_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_ਪੰਕਤੀ;

    width1 = curwin->w_ਚੌੜਾਈ - extra;
    ਜੇ (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_ਚੌੜਾਈ - 1;
	ਜੇ (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_ਉਚਾਈ - 1;
	ਨਹੀਂਤਾਂ
	    curwin->w_wrow = curwin->w_ਉਚਾਈ - 1 - curwin->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_p_wrap && curwin->w_ਚੌੜਾਈ != 0)
    {
	width2 = width1 + curwin_ਖਾਨਾ_off2();

	// skip columns that are not visible
	ਜੇ (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    ਜੇ (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    ਨਹੀਂਤਾਂ
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	ਜੇ (curwin->w_wcol >= curwin->w_ਚੌੜਾਈ)
	{
	    // this same formula is used in validate_cursor_col()
	    n = (curwin->w_wcol - curwin->w_ਚੌੜਾਈ) / width2 + 1;
	    curwin->w_wcol -= n * width2;
	    curwin->w_wrow += n;
	}
    }

    // No line wrapping: compute curwin->w_leftcol if scrolling is on and line
    // is not folded.
    // If scrolling is off, curwin->w_leftcol is assumed to be 0
    ਨਹੀਂਤਾਂ ਜੇ (may_scroll
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    && !curwin->w_cline_folded
#ਜੇ_ਅੰਤ
	    )
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (curwin->w_virtcol_ਪਹਿਲਾ_char > 0)
	{
	    ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ = (curwin->w_ਚੌੜਾਈ - extra);
	    ਪੂਰਨ ਪੰਕਤੀਆਂ_2 = ਖਾਨੇ_ਗਿਣਤੀ > 0 ? curwin->w_virtcol_ਪਹਿਲਾ_char / ਖਾਨੇ_ਗਿਣਤੀ : 1;

	    // each "above" text prop shifts the text one row down
	    curwin->w_wrow += ਪੰਕਤੀਆਂ_2;
	    curwin->w_wcol -= ਪੰਕਤੀਆਂ_2 * ਖਾਨੇ_ਗਿਣਤੀ;
	    endcol -= ਪੰਕਤੀਆਂ_2 * ਖਾਨੇ_ਗਿਣਤੀ;
	    curwin->w_cline_ਉਚਾਈ = ਪੰਕਤੀਆਂ_2 + 1;
	}
#ਜੇ_ਅੰਤ
	/*
	 * If Cursor is left of the screen, scroll rightwards.
	 * If Cursor is right of the screen, scroll leftwards.
	 * If we get closer to the edge than 'sidescrolloff', scroll a little
	 * extra.
	 */
	off_ਖੱਬਾ = startcol - curwin->w_leftcol - siso;
	off_ਸੱਜਾ = endcol - curwin->w_leftcol - (curwin->w_ਚੌੜਾਈ - siso) + 1;
	ਜੇ (off_ਖੱਬਾ < 0 || off_ਸੱਜਾ > 0)
	{
	    ਜੇ (off_ਖੱਬਾ < 0)
		diff = -off_ਖੱਬਾ;
	    ਨਹੀਂਤਾਂ
		diff = off_ਸੱਜਾ;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    ਜੇ (p_ss == 0 || diff >= width1 / 2 || off_ਸੱਜਾ >= off_ਖੱਬਾ)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (diff < p_ss)
		    diff = p_ss;
		ਜੇ (off_ਖੱਬਾ < 0)
		    new_leftcol = curwin->w_leftcol - (ਪੂਰਨ)diff;
		ਨਹੀਂਤਾਂ
		    new_leftcol = curwin->w_leftcol + (ਪੂਰਨ)diff;
	    }
	    ਜੇ (new_leftcol < 0)
		new_leftcol = 0;
	    ਜੇ (new_leftcol != (ਪੂਰਨ)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_wcol > (ਪੂਰਨ)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    ਨਹੀਂਤਾਂ
	curwin->w_wcol = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    ਜੇ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    ਨਹੀਂਤਾਂ
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#ਜੇ_ਅੰਤ

    prev_skipcol = curwin->w_skipcol;

    p_ਸਤਰਾਂ = 0;

    ਜੇ ((curwin->w_wrow >= curwin->w_ਉਚਾਈ
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_ਉਚਾਈ)
		    && (p_ਸਤਰਾਂ =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			plines_win_nofill
#ਨਹੀਂਤਾਂ
			plines_win
#ਜੇ_ਅੰਤ
			(curwin, curwin->w_cursor.lnum, FALSE))
						    - 1 >= curwin->w_ਉਚਾਈ))
	    && curwin->w_ਉਚਾਈ != 0
	    && curwin->w_cursor.lnum == curwin->w_topline
	    && width2 > 0
	    && curwin->w_ਚੌੜਾਈ != 0)
    {
	// Cursor past end of screen.  Happens with a single line that does
	// not fit on screen.  Find a skipcol to show the text around the
	// cursor.  Avoid scrolling all the time. compute value of "extra":
	// 1: Less than 'scrolloff' lines above
	// 2: Less than 'scrolloff' lines below
	// 3: both of them
	extra = 0;
	ਜੇ (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	ਜੇ (p_ਸਤਰਾਂ == 0)
	    p_ਸਤਰਾਂ = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_ਸਤਰਾਂ;
	ਜੇ (p_ਸਤਰਾਂ > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	ਨਹੀਂਤਾਂ
	    n = p_ਸਤਰਾਂ;
	ਜੇ ((colnr_T)n >= curwin->w_ਉਚਾਈ + curwin->w_skipcol / width2 - so)
	    extra += 2;

	ਜੇ (extra == 3 || curwin->w_ਉਚਾਈ <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    ਜੇ (n > curwin->w_ਉਚਾਈ / 2)
		n -= curwin->w_ਉਚਾਈ / 2;
	    ਨਹੀਂਤਾਂ
		n = 0;
	    // don't skip more than necessary
	    ਜੇ (n > p_ਸਤਰਾਂ - curwin->w_ਉਚਾਈ + 1)
		n = p_ਸਤਰਾਂ - curwin->w_ਉਚਾਈ + 1;
	    ਜੇ (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    ਨਹੀਂਤਾਂ
		curwin->w_skipcol = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    ਜੇ (extra > 0)
	    {
		ਜੇ ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_ਉਚਾਈ + 1) * width2;
	    ਜਦੋਂ (endcol > curwin->w_virtcol)
		endcol -= width2;
	    ਜੇ (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	ਜੇ (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - prev_skipcol) / width2;
	ਨਹੀਂਤਾਂ
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	ਜੇ (curwin->w_wrow >= curwin->w_ਉਚਾਈ)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_ਉਚਾਈ + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((ਪੂਰਨ)prev_skipcol - (ਪੂਰਨ)curwin->w_skipcol) / width2;
	ਜੇ (extra > 0)
	    win_ins_ਸਤਰਾਂ(curwin, 0, extra, FALSE, FALSE);
	ਨਹੀਂਤਾਂ ਜੇ (extra < 0)
	    win_del_ਸਤਰਾਂ(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    ਨਹੀਂਤਾਂ ਜੇ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    ਜੇ (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ਜੇ (popup_is_popup(curwin) && curbuf->b_ਪਦ != ਨਿਰਾ)
    {
	curwin->w_wrow += popup_ਸਿਖਰ_extra(curwin);
	curwin->w_wcol += popup_ਖੱਬਾ_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    ਨਹੀਂਤਾਂ
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#ਜੇ_ਅੰਤ

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    ਖਾਲੀ
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	ਪੂਰਨ	*rowp,	// screen row
	ਪੂਰਨ	*scolp,	// start screen column
	ਪੂਰਨ	*ccolp,	// cursor screen column
	ਪੂਰਨ	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    ਪੂਰਨ		ਪੰਕਤੀ = 0;
    colnr_T	coloff = 0;

    ਜੇ (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    ਖਾਨਾ;
	ਪੂਰਨ	    ਚੌੜਾਈ;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	ਪੂਰਨ	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, ਨਿਰਾ, TRUE, ਨਿਰਾ);
# endif
	ਪੰਕਤੀ = plines_m_win(wp, wp->w_topline, lnum - 1, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
	// "row" should be the screen line where line "lnum" begins, which can
	// be negative if "lnum" is "w_topline" and "w_skipcol" is non-zero.
	ਪੰਕਤੀ -= adjust_plines_for_skipcol(wp);

# ifdef FEAT_DIFF
	// Add filler lines above this buffer line.
	ਪੰਕਤੀ += lnum == wp->w_topline ? wp->w_topfill
				     : diff_check_fill(wp, lnum);
# endif

	colnr_T	off = win_ਖਾਨਾ_off(wp);
# ifdef FEAT_FOLDING
	ਜੇ (is_folded)
	{
	    ਪੰਕਤੀ += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	ਨਹੀਂਤਾਂ
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    ਖਾਨਾ = scol;
	    ਖਾਨਾ += off;
	    ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ - off + win_ਖਾਨਾ_off2(wp);

	    // long line wrapping, adjust row
	    ਜੇ (wp->w_p_wrap
		    && ਖਾਨਾ >= (colnr_T)wp->w_ਚੌੜਾਈ
		    && ਚੌੜਾਈ > 0)
	    {
		// use same formula as what is used in curs_columns()
		ਪੂਰਨ rowoff = ((ਖਾਨਾ - wp->w_ਚੌੜਾਈ) / ਚੌੜਾਈ + 1);
		ਖਾਨਾ -= rowoff * ਚੌੜਾਈ;
		ਪੰਕਤੀ += rowoff;
	    }
	    ਖਾਨਾ -= wp->w_leftcol;
	    ਜੇ (ਖਾਨਾ >= wp->w_ਚੌੜਾਈ)
		ਖਾਨਾ = -1;
	    ਜੇ (ਖਾਨਾ >= 0 && ਪੰਕਤੀ >= 0 && ਪੰਕਤੀ < wp->w_ਉਚਾਈ)
	    {
		coloff = ਖਾਨਾ - scol + wp->w_wincol + 1;
		ਪੰਕਤੀ += W_WINROW(wp) + 1;
	    }
	    ਨਹੀਂਤਾਂ
		// character is out of the window
		ਪੰਕਤੀ = scol = ccol = ecol = 0;
	}
    }
    *rowp = ਪੰਕਤੀ;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    ਜੇ (wp->w_p_rl && ਪੰਕਤੀ > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	ਪੂਰਨ endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_ਚੌੜਾਈ - endoff;
    }
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    ਖਾਲੀ
f_screenpos(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    ਪੂਰਨ		ਪੰਕਤੀ = 0;
    ਪੂਰਨ		scol = 0, ccol = 0, ecol = 0;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    dict = rettv->vval.v_dict;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    pos.lnum = tv_get_ਸੰਖਿਆ(&argvars[1]);
    ਜੇ (pos.lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	semsg(_(e_invalid_ਸਤਰ_ਸੰਖਿਆ_nr), pos.lnum);
	ਵਾਪਸ;
    }
    pos.ਖਾਨਾ = tv_get_ਸੰਖਿਆ(&argvars[2]) - 1;
    ਜੇ (pos.ਖਾਨਾ < 0)
	pos.ਖਾਨਾ = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &ਪੰਕਤੀ, &scol, &ccol, &ecol);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "row", ਪੰਕਤੀ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "col", scol);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "curscol", ccol);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    ਸਥਿਰ ਪੂਰਨ
virtcol2col(win_T *wp, linenr_T lnum, ਪੂਰਨ vcol)
{
    ਪੂਰਨ		ਵਿੱਥ = vcol2col(wp, lnum, vcol - 1, ਨਿਰਾ);
    char_u	*ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
    char_u	*p = ਸਤਰ + ਵਿੱਥ;

    ਜੇ (*p == NUL)
    {
	ਜੇ (p == ਸਤਰ)  // empty line
	    ਵਾਪਸ 0;
	// Move to the first byte of the last char.
	MB_PTR_ਪਿੱਠ(ਸਤਰ, p);
    }
    ਵਾਪਸ (ਪੂਰਨ)(p - ਸਤਰ + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    ਖਾਲੀ
f_virtcol2col(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    ਪੂਰਨ		screencol;
    ਪੂਰਨ		error = FALSE;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
	    || check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
	    || check_for_ਸੰਖਿਆ_arg(argvars, 2) == FAIL)
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ;

    lnum = tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
    ਜੇ (error || lnum < 0 || lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ;

    screencol = tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
    ਜੇ (error || screencol < 0)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = virtcol2col(wp, lnum, screencol);
}
#ਜੇ_ਅੰਤ

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
ਸਥਿਰ ਖਾਲੀ cursor_correct_sms(ਖਾਲੀ)
{
    ਜੇ (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	ਵਾਪਸ;

    ਲੰਮਾ    so = get_scrolloff_ਮੁੱਲ();
    ਪੂਰਨ	    width1 = curwin->w_ਚੌੜਾਈ - curwin_ਖਾਨਾ_off();
    ਪੂਰਨ	    width2 = width1 + curwin_ਖਾਨਾ_off2();
    ਪੂਰਨ	    so_ਖਾਨੇ_ਗਿਣਤੀ = so == 0 ? 0 : width1 + (so - 1) * width2;
    ਪੂਰਨ	    space_ਖਾਨੇ_ਗਿਣਤੀ = (curwin->w_ਉਚਾਈ - 1) * width2;
    ਪੂਰਨ	    overlap, ਸਿਖਰ, bot;
    ਪੂਰਨ	    ਮਾਪ = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    ਜੇ (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_ਖਾਨੇ_ਗਿਣਤੀ = 0;               // Ignore 'scrolloff' at top of buffer.
    ਨਹੀਂਤਾਂ ਜੇ (so_ਖਾਨੇ_ਗਿਣਤੀ > space_ਖਾਨੇ_ਗਿਣਤੀ / 2)
	so_ਖਾਨੇ_ਗਿਣਤੀ = space_ਖਾਨੇ_ਗਿਣਤੀ / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    ਜਦੋਂ (so_ਖਾਨੇ_ਗਿਣਤੀ > ਮਾਪ && so_ਖਾਨੇ_ਗਿਣਤੀ - width2 >= width1 && width1 > 0)
	so_ਖਾਨੇ_ਗਿਣਤੀ -= width2;
    ਜੇ (so_ਖਾਨੇ_ਗਿਣਤੀ >= width1 && so_ਖਾਨੇ_ਗਿਣਤੀ > ਮਾਪ)
	so_ਖਾਨੇ_ਗਿਣਤੀ -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_ਚੌੜਾਈ - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    ਸਿਖਰ = curwin->w_skipcol + (so_ਖਾਨੇ_ਗਿਣਤੀ != 0 ? so_ਖਾਨੇ_ਗਿਣਤੀ : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_ਉਚਾਈ - 1) * width2
								    - so_ਖਾਨੇ_ਗਿਣਤੀ;
    validate_virtcol();
    colnr_T ਖਾਨਾ = curwin->w_virtcol;

    ਜੇ (ਖਾਨਾ < ਸਿਖਰ)
    {
	ਜੇ (ਖਾਨਾ < width1)
	    ਖਾਨਾ += width1;
	ਜਦੋਂ (width2 > 0 && ਖਾਨਾ < ਸਿਖਰ)
	    ਖਾਨਾ += width2;
    }
    ਨਹੀਂਤਾਂ
	ਜਦੋਂ (width2 > 0 && ਖਾਨਾ >= bot)
	    ਖਾਨਾ -= width2;

    ਜੇ (ਖਾਨਾ != curwin->w_virtcol)
    {
	ਪੂਰਨ rc;

	curwin->w_curswant = ਖਾਨਾ;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	ਜੇ (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    validate_virtcol();
	    ਜੇ (curwin->w_virtcol < curwin->w_skipcol + overlap)
	    {
		// Cursor still not visible: move it to the next line instead.
		curwin->w_cursor.lnum++;
		curwin->w_cursor.ਖਾਨਾ = 0;
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
    ਖਾਲੀ
scroll_redraw(ਪੂਰਨ ਉੱਪਰ, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    linenr_T	prev_topline = curwin->w_topline;
    ਪੂਰਨ		prev_skipcol = curwin->w_skipcol;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		prev_topfill = curwin->w_topfill;
#ਜੇ_ਅੰਤ
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    ਜੇ (ਉੱਪਰ)
	scrollup(ਗਿਣਤੀ_2, TRUE);
    ਨਹੀਂਤਾਂ
	scrolldown(ਗਿਣਤੀ_2, TRUE);
    ਜੇ (get_scrolloff_ਮੁੱਲ() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	ਜਦੋਂ (curwin->w_topline == prev_topline
		&& curwin->w_skipcol == prev_skipcol
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#ਜੇ_ਅੰਤ
		)
	{
	    ਜੇ (ਉੱਪਰ)
	    {
		ਜੇ (curwin->w_cursor.lnum > prev_lnum
			|| cursor_ਥੱਲੇ(1L, FALSE) == FAIL)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_ਉੱਪਰ(1L, FALSE) == FAIL)
		    ਰੋਕੋ;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    ਜੇ (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    ਖਾਲੀ
scrolldown(
    ਲੰਮਾ	ਸਤਰ_ਗਿਣਤੀ,
    ਪੂਰਨ		byfold ਅਣਵਰਤਿਆ)	// TRUE: count a closed fold as one line
{
    ਲੰਮਾ	ਪੂਰਾ = 0;	// total # of physical lines done
    ਪੂਰਨ		wrow;
    ਪੂਰਨ		moved = FALSE;
    ਪੂਰਨ		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ਪੂਰਨ		width1 = 0;
    ਪੂਰਨ		width2 = 0;

    ਜੇ (do_sms)
    {
	width1 = curwin->w_ਚੌੜਾਈ - curwin_ਖਾਨਾ_off();
	width2 = width1 + curwin_ਖਾਨਾ_off2();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    linenr_T	ਪਹਿਲਾ;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (ਖਾਲੀ)hasFolding(curwin->w_topline, &curwin->w_topline, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    validate_cursor();		// w_wrow needs to be valid
    ਲਈ (ਪੂਰਨ todo = ਸਤਰ_ਗਿਣਤੀ; todo > 0; --todo)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	ਜੇ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_ਉਚਾਈ - 1)
	{
	    ++curwin->w_topfill;
	    ++ਪੂਰਾ;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    // break when at the very top
	    ਜੇ (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		ਰੋਕੋ;
	    ਜੇ (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		ਜੇ (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		ਨਹੀਂਤਾਂ
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++ਪੂਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		curwin->w_topfill = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		ਜੇ (hasFolding(curwin->w_topline, &ਪਹਿਲਾ, ਨਿਰਾ))
		{
		    ++ਪੂਰਾ;
		    ਜੇ (!byfold)
			todo -= curwin->w_topline - ਪਹਿਲਾ - 1;
		    curwin->w_botline -= curwin->w_topline - ਪਹਿਲਾ;
		    curwin->w_topline = ਪਹਿਲਾ;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਜੇ (do_sms)
		{
		    ਪੂਰਨ ਮਾਪ = linetabsize_eol(curwin, curwin->w_topline);
		    ਜੇ (ਮਾਪ > width1)
		    {
			curwin->w_skipcol = width1;
			ਮਾਪ -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    ਜਦੋਂ (ਮਾਪ > width2)
		    {
			curwin->w_skipcol += width2;
			ਮਾਪ -= width2;
		    }
		    ++ਪੂਰਾ;
		}
		ਨਹੀਂਤਾਂ
		    ਪੂਰਾ += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += ਪੂਰਾ;		// keep w_wrow updated
    curwin->w_cline_ਪੰਕਤੀ += ਪੂਰਾ;	// keep w_cline_row updated

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_ਪੰਕਤੀ = 0;
    check_topfill(curwin, TRUE);
#ਜੇ_ਅੰਤ

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    ਜੇ (curwin->w_p_wrap && curwin->w_ਚੌੜਾਈ != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_ਉਚਾਈ - 1 -
	    curwin->w_virtcol / curwin->w_ਚੌੜਾਈ;
    }
    ਜਦੋਂ (wrow >= curwin->w_ਉਚਾਈ && curwin->w_cursor.lnum > 1)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਜੇ (hasFolding(curwin->w_cursor.lnum, &ਪਹਿਲਾ, ਨਿਰਾ))
	{
	    --wrow;
	    ਜੇ (ਪਹਿਲਾ == 1)
		curwin->w_cursor.lnum = 1;
	    ਨਹੀਂਤਾਂ
		curwin->w_cursor.lnum = ਪਹਿਲਾ - 1;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    ਜੇ (moved)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#ਜੇ_ਅੰਤ
	coladvance(curwin->w_curswant);
    }
    ਜੇ (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    ਖਾਲੀ
scrollup(
    ਲੰਮਾ	ਸਤਰ_ਗਿਣਤੀ,
    ਪੂਰਨ		byfold ਅਣਵਰਤਿਆ)	// TRUE: count a closed fold as one line
{
    ਪੂਰਨ		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    ਜੇ (do_sms
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#ਜੇ_ਅੰਤ
       )
    {
	ਪੂਰਨ	    width1 = curwin->w_ਚੌੜਾਈ - curwin_ਖਾਨਾ_off();
	ਪੂਰਨ	    width2 = width1 + curwin_ਖਾਨਾ_off2();
	ਪੂਰਨ	    ਮਾਪ = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	ਜੇ (do_sms)
	    ਮਾਪ = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	ਲਈ (ਪੂਰਨ todo = ਸਤਰ_ਗਿਣਤੀ; todo > 0; --todo)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    ਜੇ (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		linenr_T lnum = curwin->w_topline;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		ਜੇ (byfold)
		    // for a closed fold: go to the last line in the fold
		    (ਖਾਲੀ)hasFolding(lnum, ਨਿਰਾ, &lnum);
#ਜੇ_ਅੰਤ
		ਜੇ (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    ਪੂਰਨ ਜੋੜੋ = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += ਜੋੜੋ;
		    ਜੇ (curwin->w_skipcol >= ਮਾਪ)
		    {
			ਜੇ (lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= ਜੋੜੋ;
			    ਰੋਕੋ;
			}
			++lnum;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (lnum >= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
			ਰੋਕੋ;
		    ++lnum;
		}

		ਜੇ (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#ਜੇ_ਅੰਤ
		    curwin->w_skipcol = 0;
		    ਜੇ (todo > 1 && do_sms)
			ਮਾਪ = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	ਜੇ (prev_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    ਨਹੀਂਤਾਂ
    {
	curwin->w_topline += ਸਤਰ_ਗਿਣਤੀ;
	curwin->w_botline += ਸਤਰ_ਗਿਣਤੀ;	// approximate w_botline
    }

    ਜੇ (curwin->w_topline > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	curwin->w_topline = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਜੇ (curwin->w_botline > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	curwin->w_botline = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    check_topfill(curwin, FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(ਖਾਲੀ)hasFolding(curwin->w_topline, &curwin->w_topline, ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    ਜੇ (curwin->w_cursor.lnum < curwin->w_topline)
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
    ਖਾਲੀ
adjust_skipcol(ਖਾਲੀ)
{
    ਜੇ (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	ਵਾਪਸ;

    ਪੂਰਨ	    width1 = curwin->w_ਚੌੜਾਈ - curwin_ਖਾਨਾ_off();
    ਜੇ (width1 <= 0)
	ਵਾਪਸ;  // no text will be displayed

    ਪੂਰਨ	    width2 = width1 + curwin_ਖਾਨਾ_off2();
    ਲੰਮਾ    so = get_scrolloff_ਮੁੱਲ();
    ਪੂਰਨ	    scrolloff_ਖਾਨੇ_ਗਿਣਤੀ = so == 0 ? 0 : width1 + (so - 1) * width2;
    ਪੂਰਨ	    scrolled = FALSE;
    ਪੂਰਨ	    ਪੰਕਤੀ = 0;
    ਪੂਰਨ	    overlap, ਖਾਨਾ;

    validate_cheight();
    ਜੇ (curwin->w_cline_ਉਚਾਈ == curwin->w_ਉਚਾਈ
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_ਉਚਾਈ)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	ਵਾਪਸ;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_ਚੌੜਾਈ - width2);
    ਜਦੋਂ (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_ਖਾਨੇ_ਗਿਣਤੀ)
    {
	// scroll a screen line down
	ਜੇ (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	ਨਹੀਂਤਾਂ
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    ਜੇ (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	ਵਾਪਸ;  // don't scroll in the other direction now
    }

    ਖਾਨਾ = curwin->w_virtcol + scrolloff_ਖਾਨੇ_ਗਿਣਤੀ;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    ਜੇ (scrolloff_ਖਾਨੇ_ਗਿਣਤੀ > 0)
    {
	ਪੂਰਨ ਮਾਪ = linetabsize_eol(curwin, curwin->w_topline);
	ਮਾਪ = width1 + width2 * ((ਮਾਪ - width1 + width2 - 1) / width2);
	ਜਦੋਂ (ਖਾਨਾ > ਮਾਪ)
	    ਖਾਨਾ -= width2;
    }
    ਖਾਨਾ -= curwin->w_skipcol;

    ਜੇ (ਖਾਨਾ >= width1)
    {
	ਖਾਨਾ -= width1;
	++ਪੰਕਤੀ;
    }
    ਜੇ (ਖਾਨਾ > width2)
    {
	ਪੰਕਤੀ += ਖਾਨਾ / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	ਖਾਨਾ = ਖਾਨਾ % width2;
    }
    ਜੇ (ਪੰਕਤੀ >= curwin->w_ਉਚਾਈ)
    {
	ਜੇ (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --ਪੰਕਤੀ;
	}
	ਜੇ (ਪੰਕਤੀ >= curwin->w_ਉਚਾਈ)
	    curwin->w_skipcol += (ਪੰਕਤੀ - curwin->w_ਉਚਾਈ) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    ਖਾਲੀ
check_topfill(
    win_T	*wp,
    ਪੂਰਨ		ਥੱਲੇ)	// when TRUE scroll down when not enough space
{
    ਪੂਰਨ		n;

    ਜੇ (wp->w_topfill <= 0)
	ਵਾਪਸ;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    ਜੇ (wp->w_topfill + n > wp->w_ਉਚਾਈ)
    {
	ਜੇ (ਥੱਲੇ && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    wp->w_topfill = wp->w_ਉਚਾਈ - n;
	    ਜੇ (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#ਜੇ_ਅੰਤ

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    ਖਾਲੀ
scrolldown_clamp(ਖਾਲੀ)
{
    ਪੂਰਨ		ਸਿਰਾ_ਪੰਕਤੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#ਜੇ_ਅੰਤ

    ਜੇ (curwin->w_topline <= 1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    && !can_fill
#ਜੇ_ਅੰਤ
	    )
	ਵਾਪਸ;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    ਸਿਰਾ_ਪੰਕਤੀ = curwin->w_wrow;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (can_fill)
	++ਸਿਰਾ_ਪੰਕਤੀ;
    ਨਹੀਂਤਾਂ
	ਸਿਰਾ_ਪੰਕਤੀ += plines_nofill(curwin->w_topline - 1);
#ਨਹੀਂਤਾਂ
    ਸਿਰਾ_ਪੰਕਤੀ += plines(curwin->w_topline - 1);
#ਜੇ_ਅੰਤ
    ਜੇ (curwin->w_p_wrap && curwin->w_ਚੌੜਾਈ != 0)
    {
	validate_cheight();
	validate_virtcol();
	ਸਿਰਾ_ਪੰਕਤੀ += curwin->w_cline_ਉਚਾਈ - 1 -
	    curwin->w_virtcol / curwin->w_ਚੌੜਾਈ;
    }
    ਜੇ (ਸਿਰਾ_ਪੰਕਤੀ < curwin->w_ਉਚਾਈ - get_scrolloff_ਮੁੱਲ())
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	ਜੇ (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	ਨਹੀਂਤਾਂ
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#ਨਹੀਂਤਾਂ
	--curwin->w_topline;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	(ਖਾਲੀ)hasFolding(curwin->w_topline, &curwin->w_topline, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
	--curwin->w_botline;	    // approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    ਖਾਲੀ
scrollup_clamp(ਖਾਲੀ)
{
    ਪੂਰਨ	    ਸ਼ੁਰੂ_ਪੰਕਤੀ;

    ਜੇ (curwin->w_topline == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    && curwin->w_topfill == 0
#ਜੇ_ਅੰਤ
	    )
	ਵਾਪਸ;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਸ਼ੁਰੂ_ਪੰਕਤੀ = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#ਨਹੀਂਤਾਂ
    ਸ਼ੁਰੂ_ਪੰਕਤੀ = curwin->w_wrow - plines(curwin->w_topline);
#ਜੇ_ਅੰਤ
    ਜੇ (curwin->w_p_wrap && curwin->w_ਚੌੜਾਈ != 0)
    {
	validate_virtcol();
	ਸ਼ੁਰੂ_ਪੰਕਤੀ -= curwin->w_virtcol / curwin->w_ਚੌੜਾਈ;
    }
    ਜੇ (ਸ਼ੁਰੂ_ਪੰਕਤੀ >= get_scrolloff_ਮੁੱਲ())
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	ਜੇ (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    (ਖਾਲੀ)hasFolding(curwin->w_topline, ਨਿਰਾ, &curwin->w_topline);
#ਜੇ_ਅੰਤ
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
    ਸਥਿਰ ਖਾਲੀ
topline_ਪਿੱਠ_winheight(
    lineoff_T	*lp,
    ਪੂਰਨ		winheight)	// when TRUE limit to window height
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->ਉਚਾਈ = 1;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	--lp->lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	lp->fill = 0;
#ਜੇ_ਅੰਤ
	ਜੇ (lp->lnum < 1)
	    lp->ਉਚਾਈ = MAXCOL;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (hasFolding(lp->lnum, &lp->lnum, ਨਿਰਾ))
	    // Add a closed fold
	    lp->ਉਚਾਈ = 1;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    lp->ਉਚਾਈ = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    ਸਥਿਰ ਖਾਲੀ
topline_ਪਿੱਠ(lineoff_T *lp)
{
    topline_ਪਿੱਠ_winheight(lp, TRUE);
}


/*
 * Add one line below "lp->lnum".  This can be a filler line, a closed fold or
 * a (wrapped) text line.  Uses and sets "lp->fill".
 * Returns the height of the added line in "lp->height".
 * Lines below the last one are incredibly high.
 */
    ਸਥਿਰ ਖਾਲੀ
botline_forw(lineoff_T *lp)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->ਉਚਾਈ = 1;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	++lp->lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	lp->fill = 0;
#ਜੇ_ਅੰਤ
	ਜੇ (lp->lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    lp->ਉਚਾਈ = MAXCOL;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (hasFolding(lp->lnum, ਨਿਰਾ, &lp->lnum))
	    {
		// Add a closed fold.
		lp->ਉਚਾਈ = 1;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    lp->ਉਚਾਈ = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    ਖਾਲੀ
scroll_cursor_ਸਿਖਰ(ਪੂਰਨ min_scroll, ਪੂਰਨ always)
{
    ਪੂਰਨ		scrolled = 0;
    ਪੂਰਨ		extra = 0;
    ਪੂਰਨ		ਵਰਤਿਆ;
    ਪੂਰਨ		i;
    linenr_T	ਸਿਖਰ;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    ਪੂਰਨ		old_skipcol = curwin->w_skipcol;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#ਜੇ_ਅੰਤ
    linenr_T	new_topline;
    ਪੂਰਨ		off = get_scrolloff_ਮੁੱਲ();

    ਜੇ (mouse_dragging > 0)
	off = mouse_dragging - 1;

    /*
     * Decrease topline until:
     * - it has become 1
     * - (part of) the cursor line is moved off the screen or
     * - moved at least 'scrolljump' lines and
     * - at least 'scrolloff' lines above and below the cursor
     */
    validate_cheight();
    ਵਰਤਿਆ = curwin->w_cline_ਉਚਾਈ; // includes filler lines above
    ਜੇ (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = ਵਰਤਿਆ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (hasFolding(curwin->w_cursor.lnum, &ਸਿਖਰ, &bot))
    {
	--ਸਿਖਰ;
	++bot;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਸਿਖਰ = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = ਸਿਖਰ + 1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    // "used" already contains the number of filler lines above, don't add it
    // again.
    // Hide filler lines above cursor line by adding them to "extra".
    extra += diff_check_fill(curwin, curwin->w_cursor.lnum);
#ਜੇ_ਅੰਤ

    /*
     * Check if the lines from "top" to "bot" fit in the window.  If they do,
     * set new_topline and advance "top" and "bot" to include more lines.
     */
    ਜਦੋਂ (ਸਿਖਰ > 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਜੇ (hasFolding(ਸਿਖਰ, &ਸਿਖਰ, ਨਿਰਾ))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    i = PLINES_NOFILL(ਸਿਖਰ);
	ਜੇ (ਸਿਖਰ < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	ਜੇ ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    ਰੋਕੋ;

	ਵਰਤਿਆ += i;
	ਜੇ (extra + i <= off && bot < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (hasFolding(bot, ਨਿਰਾ, &bot))
		// count one logical line for a sequence of folded lines
		++ਵਰਤਿਆ;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਵਰਤਿਆ += plines(bot);
	}
	ਜੇ (ਵਰਤਿਆ > curwin->w_ਉਚਾਈ)
	    ਰੋਕੋ;

	extra += i;
	new_topline = ਸਿਖਰ;
	--ਸਿਖਰ;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    ਜੇ (ਵਰਤਿਆ > curwin->w_ਉਚਾਈ)
	scroll_cursor_halfway(FALSE, FALSE);
    ਨਹੀਂਤਾਂ
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	ਜੇ (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	ਜੇ (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	ਜੇ (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    ਜੇ (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#ਜੇ_ਅੰਤ
	ਜੇ (curwin->w_topline != old_topline)
	    reset_skipcol();
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    ਜੇ (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		ਪੂਰਨ	width1 = curwin->w_ਚੌੜਾਈ - curwin_ਖਾਨਾ_off();
		ਪੂਰਨ	width2 = width1 + curwin_ਖਾਨਾ_off2();
		ਪੂਰਨ	plines_off = 0;
		ਪੂਰਨ	skipcol;

		ਜੇ (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		ਜੇ (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	ਜੇ (curwin->w_topline != old_topline
		|| curwin->w_skipcol != old_skipcol
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#ਜੇ_ਅੰਤ
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
    ਖਾਲੀ
ਸੈੱਟ_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ(win_T *wp, ਪੂਰਨ ਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    wp->w_filler_ਪੰਕਤੀਆਂ = 0;
#ਜੇ_ਅੰਤ
    ਜੇ (ਵਰਤਿਆ == 0)
	wp->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ = 0;	// single line that doesn't fit
    ਨਹੀਂਤਾਂ
    {
	wp->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ = wp->w_ਉਚਾਈ - ਵਰਤਿਆ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	ਜੇ (wp->w_botline <= wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    wp->w_filler_ਪੰਕਤੀਆਂ = diff_check_fill(wp, wp->w_botline);
	    ਜੇ (wp->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ > wp->w_filler_ਪੰਕਤੀਆਂ)
		wp->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ -= wp->w_filler_ਪੰਕਤੀਆਂ;
	    ਨਹੀਂਤਾਂ
	    {
		wp->w_filler_ਪੰਕਤੀਆਂ = wp->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
		wp->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ = 0;
	    }
	}
#ਜੇ_ਅੰਤ
    }
}

/*
 * Recompute topline to put the cursor at the bottom of the window.
 * When scrolling scroll at least "min_scroll" lines.
 * If "set_topbot" is TRUE, set topline and botline first (for "zb").
 * This is messy stuff!!!
 */
    ਖਾਲੀ
scroll_cursor_bot(ਪੂਰਨ min_scroll, ਪੂਰਨ ਸੈੱਟ_topbot)
{
    ਪੂਰਨ		ਵਰਤਿਆ;
    ਪੂਰਨ		scrolled = 0;
    ਪੂਰਨ		extra = 0;
    ਪੂਰਨ		i;
    linenr_T	ਸਤਰ_ਗਿਣਤੀ;
    linenr_T	old_topline = curwin->w_topline;
    ਪੂਰਨ		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		old_topfill = curwin->w_topfill;
    ਪੂਰਨ		fill_ਹੇਠਾਂ_ਖਿੜਕੀ;
#ਜੇ_ਅੰਤ
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    ਪੂਰਨ		old_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ = curwin->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
    linenr_T	cln;		    // Cursor Line Number
    ਲੰਮਾ	so = get_scrolloff_ਮੁੱਲ();
    ਪੂਰਨ		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    ਜੇ (ਸੈੱਟ_topbot)
    {
	ਵਰਤਿਆ = 0;
	linenr_T cln_ਆਖਰੀ = cln;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	(ਖਾਲੀ)hasFolding(cln, ਨਿਰਾ, &cln_ਆਖਰੀ);
#ਜੇ_ਅੰਤ
	curwin->w_botline = cln_ਆਖਰੀ + 1;
	loff.lnum = cln_ਆਖਰੀ + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	loff.fill = 0;
#ਜੇ_ਅੰਤ
	ਜਦੋਂ (TRUE)
	{
	    topline_ਪਿੱਠ_winheight(&loff, FALSE);
	    ਜੇ (loff.ਉਚਾਈ == MAXCOL)
		ਰੋਕੋ;
	    ਜੇ (ਵਰਤਿਆ + loff.ਉਚਾਈ > curwin->w_ਉਚਾਈ)
	    {
		ਜੇ (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    ਜੇ (ਵਰਤਿਆ < curwin->w_ਉਚਾਈ)
		    {
			ਪੂਰਨ plines_ਵਿੱਥ = ਵਰਤਿਆ + loff.ਉਚਾਈ
							    - curwin->w_ਉਚਾਈ;
			ਵਰਤਿਆ = curwin->w_ਉਚਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			curwin->w_topfill = loff.fill;
#ਜੇ_ਅੰਤ
			curwin->w_topline = loff.lnum;
			curwin->w_skipcol = skipcol_from_plines(
							curwin, plines_ਵਿੱਥ);
		    }
		}
		ਰੋਕੋ;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#ਜੇ_ਅੰਤ
	    curwin->w_topline = loff.lnum;
	    ਵਰਤਿਆ += loff.ਉਚਾਈ;
	}

	ਸੈੱਟ_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ(curwin, ਵਰਤਿਆ);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	ਜੇ (curwin->w_topline != old_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#ਜੇ_ਅੰਤ
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    ਜੇ (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    ਨਹੀਂਤਾਂ
		reset_skipcol();
	}
    }
    ਨਹੀਂਤਾਂ
	validate_botline();

    // The lines of the cursor line itself are always used.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਵਰਤਿਆ = plines_nofill(cln);
#ਨਹੀਂਤਾਂ
    validate_cheight();
    ਵਰਤਿਆ = curwin->w_cline_ਉਚਾਈ;
#ਜੇ_ਅੰਤ

    ਜੇ (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	ਪੂਰਨ upto_cursor = plines_win_ਖਾਨਾ(curwin, cln, curwin->w_cursor.ਖਾਨਾ);

	ਜੇ (upto_cursor < ਵਰਤਿਆ)
	    ਵਰਤਿਆ = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    ਜੇ (cln >= curwin->w_botline)
    {
	scrolled = ਵਰਤਿਆ;
	ਜੇ (cln == curwin->w_botline)
	    scrolled -= curwin->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
	ਜੇ (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    ਪੂਰਨ ਸਿਖਰ_plines =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			    plines_win_nofill
#ਨਹੀਂਤਾਂ
			    plines_win
#ਜੇ_ਅੰਤ
					(curwin, curwin->w_topline, FALSE);
	    ਪੂਰਨ width1 = curwin->w_ਚੌੜਾਈ - curwin_ਖਾਨਾ_off();

	    ਜੇ (width1 > 0)
	    {
		ਪੂਰਨ width2 = width1 + curwin_ਖਾਨਾ_off2();
		ਪੂਰਨ skip_ਸਤਰਾਂ = 0;

		// A similar formula is used in curs_columns().
		ਜੇ (curwin->w_skipcol > width1)
		    skip_ਸਤਰਾਂ += (curwin->w_skipcol - width1) / width2 + 1;
		ਨਹੀਂਤਾਂ ਜੇ (curwin->w_skipcol > 0)
		    skip_ਸਤਰਾਂ = 1;

		ਸਿਖਰ_plines -= skip_ਸਤਰਾਂ;
		ਜੇ (ਸਿਖਰ_plines > curwin->w_ਉਚਾਈ)
		{
		    scrolled += (ਸਿਖਰ_plines - curwin->w_ਉਚਾਈ);
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
#ਜੇ_ਅੰਤ
    {
	loff.lnum = cln;
	boff.lnum = cln;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    loff.fill = 0;
    boff.fill = 0;
    fill_ਹੇਠਾਂ_ਖਿੜਕੀ = diff_check_fill(curwin, curwin->w_botline)
						      - curwin->w_filler_ਪੰਕਤੀਆਂ;
#ਜੇ_ਅੰਤ

    ਜਦੋਂ (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	ਜੇ ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		&& loff.lnum <= curwin->w_botline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_ਹੇਠਾਂ_ਖਿੜਕੀ)
#ਜੇ_ਅੰਤ
		)
	    ਰੋਕੋ;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_ਪਿੱਠ(&loff);
	ਜੇ (loff.ਉਚਾਈ == MAXCOL)
	    ਵਰਤਿਆ = MAXCOL;
	ਨਹੀਂਤਾਂ
	    ਵਰਤਿਆ += loff.ਉਚਾਈ;
	ਜੇ (ਵਰਤਿਆ > curwin->w_ਉਚਾਈ)
	    ਰੋਕੋ;
	ਜੇ (loff.lnum >= curwin->w_botline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_ਹੇਠਾਂ_ਖਿੜਕੀ)
#ਜੇ_ਅੰਤ
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.ਉਚਾਈ;
	    ਜੇ (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
	}

	ਜੇ (boff.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    ਵਰਤਿਆ += boff.ਉਚਾਈ;
	    ਜੇ (ਵਰਤਿਆ > curwin->w_ਉਚਾਈ)
		ਰੋਕੋ;
	    ਜੇ (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.ਉਚਾਈ;
		ਜੇ (boff.lnum >= curwin->w_botline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_ਪੰਕਤੀਆਂ)
#ਜੇ_ਅੰਤ
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.ਉਚਾਈ;
		    ਜੇ (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    ਜੇ (scrolled <= 0)
	ਸਤਰ_ਗਿਣਤੀ = 0;
    // more than a screenfull, don't scroll but redraw
    ਨਹੀਂਤਾਂ ਜੇ (ਵਰਤਿਆ > curwin->w_ਉਚਾਈ)
	ਸਤਰ_ਗਿਣਤੀ = ਵਰਤਿਆ;
    // scroll minimal number of lines
    ਨਹੀਂਤਾਂ
    {
	ਸਤਰ_ਗਿਣਤੀ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	boff.fill = curwin->w_topfill;
#ਜੇ_ਅੰਤ
	boff.lnum = curwin->w_topline - 1;
	ਲਈ (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.ਉਚਾਈ;
	    ++ਸਤਰ_ਗਿਣਤੀ;
	}
	ਜੇ (i < scrolled)	// below curwin->w_botline, don't scroll
	    ਸਤਰ_ਗਿਣਤੀ = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    ਜੇ (ਸਤਰ_ਗਿਣਤੀ >= curwin->w_ਉਚਾਈ && ਸਤਰ_ਗਿਣਤੀ > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ_ਗਿਣਤੀ > 0)
    {
	ਜੇ (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	ਨਹੀਂਤਾਂ
	    scrollup(ਸਤਰ_ਗਿਣਤੀ, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    ਜੇ (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && ਸੈੱਟ_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ = old_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    ਜੇ (ਸੈੱਟ_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    ਖਾਲੀ
scroll_cursor_halfway(ਪੂਰਨ atend, ਪੂਰਨ prefer_above)
{
    ਪੂਰਨ		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		topfill = 0;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਹੇਠਾਂ = 0;
    ਪੂਰਨ		ਵਰਤਿਆ;
    lineoff_T	loff;
    lineoff_T	boff;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    linenr_T	old_topline = curwin->w_topline;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // if the width changed this needs to be updated first
    may_update_popup_position();
#ਜੇ_ਅੰਤ
    loff.lnum = boff.lnum = curwin->w_cursor.lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    (ਖਾਲੀ)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਵਰਤਿਆ = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#ਨਹੀਂਤਾਂ
    ਵਰਤਿਆ = plines(loff.lnum);
#ਜੇ_ਅੰਤ
    topline = loff.lnum;

    ਪੂਰਨ want_ਉਚਾਈ;
    ਪੂਰਨ do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ਜੇ (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	ਜੇ (atend)
	{
	    want_ਉਚਾਈ = (curwin->w_ਉਚਾਈ - ਵਰਤਿਆ) / 2;
	    ਵਰਤਿਆ = 0;
	}
	ਨਹੀਂਤਾਂ
	    want_ਉਚਾਈ = curwin->w_ਉਚਾਈ;
    }

    ਜਦੋਂ (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	ਜੇ (do_sms)
	{
	    topline_ਪਿੱਠ_winheight(&loff, FALSE);
	    ਜੇ (loff.ਉਚਾਈ == MAXCOL)
		ਰੋਕੋ;
	    ਵਰਤਿਆ += loff.ਉਚਾਈ;
	    ਜੇ (!atend && boff.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		botline_forw(&boff);
		ਵਰਤਿਆ += boff.ਉਚਾਈ;
	    }
	    ਜੇ (ਵਰਤਿਆ > want_ਉਚਾਈ)
	    {
		ਜੇ (ਵਰਤਿਆ - loff.ਉਚਾਈ < want_ਉਚਾਈ)
		{
		    topline = loff.lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    topfill = loff.fill;
#ਜੇ_ਅੰਤ
		    skipcol = skipcol_from_plines(curwin, ਵਰਤਿਆ - want_ਉਚਾਈ);
		}
		ਰੋਕੋ;
	    }
	    topline = loff.lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    topfill = loff.fill;
#ਜੇ_ਅੰਤ
	    ਜਾਰੀ;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	ਪੂਰਨ ਪੂਰਾ = FALSE;
	ਲਈ (ਪੂਰਨ ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
	{
	    ਜੇ (prefer_above ? (ਗੇੜਾ == 2 && ਹੇਠਾਂ < above)
			     : (ਗੇੜਾ == 1 && ਹੇਠਾਂ <= above))
	    {
		// add a line below the cursor
		ਜੇ (boff.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		{
		    botline_forw(&boff);
		    ਵਰਤਿਆ += boff.ਉਚਾਈ;
		    ਜੇ (ਵਰਤਿਆ > curwin->w_ਉਚਾਈ)
		    {
			ਪੂਰਾ = TRUE;
			ਰੋਕੋ;
		    }
		    ਹੇਠਾਂ += boff.ਉਚਾਈ;
		}
		ਨਹੀਂਤਾਂ
		{
		    ++ਹੇਠਾਂ;	    // count a "~" line
		    ਜੇ (atend)
			++ਵਰਤਿਆ;
		}
	    }

	    ਜੇ (prefer_above ? (ਗੇੜਾ == 1 && ਹੇਠਾਂ >= above)
			     : (ਗੇੜਾ == 1 && ਹੇਠਾਂ > above))
	    {
		// add a line above the cursor
		topline_ਪਿੱਠ(&loff);
		ਜੇ (loff.ਉਚਾਈ == MAXCOL)
		    ਵਰਤਿਆ = MAXCOL;
		ਨਹੀਂਤਾਂ
		    ਵਰਤਿਆ += loff.ਉਚਾਈ;
		ਜੇ (ਵਰਤਿਆ > curwin->w_ਉਚਾਈ)
		{
		    ਪੂਰਾ = TRUE;
		    ਰੋਕੋ;
		}
		above += loff.ਉਚਾਈ;
		topline = loff.lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		topfill = loff.fill;
#ਜੇ_ਅੰਤ
	    }
	}
	ਜੇ (ਪੂਰਾ)
	    ਰੋਕੋ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (!hasFolding(topline, &curwin->w_topline, ਨਿਰਾ))
#ਜੇ_ਅੰਤ
    {
	ਜੇ (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    ਜੇ (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (do_sms)
		reset_skipcol();
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    curwin->w_topfill = topfill;
    ਜੇ (old_topline > curwin->w_topline + curwin->w_ਉਚਾਈ)
	curwin->w_botfill = false;
    check_topfill(curwin, FALSE);
#ਜੇ_ਅੰਤ
    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
    curwin->w_valid |= VALID_TOPLINE;
}

/*
 * Correct the cursor position so that it is in a part of the screen at least
 * 'scrolloff' lines from the top and bottom, if possible.
 * If not possible, put it at the same position as scroll_cursor_halfway().
 * When called topline must be valid!
 */
    ਖਾਲੀ
cursor_correct(ਖਾਲੀ)
{
    ਪੂਰਨ		above = 0;	    // screen lines above topline
    linenr_T	topline;
    ਪੂਰਨ		ਹੇਠਾਂ = 0;	    // screen lines below botline
    linenr_T	botline;
    ਪੂਰਨ		above_wanted, ਹੇਠਾਂ_wanted;
    linenr_T	cln;		    // Cursor Line Number
    ਪੂਰਨ		max_off;
    ਲੰਮਾ	so = get_scrolloff_ਮੁੱਲ();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    ਹੇਠਾਂ_wanted = so;
    ਜੇ (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	ਹੇਠਾਂ_wanted = mouse_dragging - 1;
    }
    ਜੇ (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_ਉਚਾਈ / 2;
	ਜੇ (ਹੇਠਾਂ_wanted > max_off)
	    ਹੇਠਾਂ_wanted = max_off;
    }
    validate_botline();
    ਜੇ (curwin->w_botline == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	ਹੇਠਾਂ_wanted = 0;
	max_off = (curwin->w_ਉਚਾਈ - 1) / 2;
	ਜੇ (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    ਜੇ (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - ਹੇਠਾਂ_wanted
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#ਜੇ_ਅੰਤ
	    )
	ਵਾਪਸ;

    /*
     * Narrow down the area where the cursor can be put by taking lines from
     * the top and the bottom until:
     * - the desired context lines are found
     * - the lines from the top is past the lines from the bottom
     */
    topline = curwin->w_topline;
    botline = curwin->w_botline - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    // count filler lines as context
    above = curwin->w_topfill;
    ਹੇਠਾਂ = curwin->w_filler_ਪੰਕਤੀਆਂ;
#ਜੇ_ਅੰਤ
    ਜਦੋਂ ((above < above_wanted || ਹੇਠਾਂ < ਹੇਠਾਂ_wanted) && topline < botline)
    {
	ਜੇ (ਹੇਠਾਂ < ਹੇਠਾਂ_wanted && (ਹੇਠਾਂ <= above || above >= above_wanted))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (hasFolding(botline, &botline, ਨਿਰਾ))
		++ਹੇਠਾਂ;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਹੇਠਾਂ += plines(botline);
	    --botline;
	}
	ਜੇ (above < above_wanted && (above < ਹੇਠਾਂ || ਹੇਠਾਂ >= ਹੇਠਾਂ_wanted))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (hasFolding(topline, ਨਿਰਾ, &topline))
		++above;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		above += PLINES_NOFILL(topline);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    // Count filler lines below this line as context.
	    ਜੇ (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#ਜੇ_ਅੰਤ
	    ++topline;
	}
    }
    ਜੇ (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    ਨਹੀਂਤਾਂ ਜੇ (topline > botline)
	curwin->w_cursor.lnum = botline;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	ਜੇ (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
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
ਸਥਿਰ ਪੂਰਨ get_scroll_overlap(ਪੂਰਨ dir)
{
    lineoff_T loff;
    ਪੂਰਨ	    min_ਉਚਾਈ = curwin->w_ਉਚਾਈ - 2;

    validate_botline();
    ਜੇ ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ))
	ਵਾਪਸ min_ਉਚਾਈ + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_ਪੰਕਤੀਆਂ : curwin->w_topfill);
    loff.ਉਚਾਈ = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#ਨਹੀਂਤਾਂ
    loff.ਉਚਾਈ = plines(loff.lnum);
#ਜੇ_ਅੰਤ

    ਪੂਰਨ h1 = loff.ਉਚਾਈ;
    ਜੇ (h1 > min_ਉਚਾਈ)
	ਵਾਪਸ min_ਉਚਾਈ + 2;  // no overlap
    ਜੇ (dir == FORWARD)
	topline_ਪਿੱਠ(&loff);
    ਨਹੀਂਤਾਂ
	botline_forw(&loff);

    ਪੂਰਨ h2 = loff.ਉਚਾਈ;
    ਜੇ (h2 == MAXCOL || h2 + h1 > min_ਉਚਾਈ)
	ਵਾਪਸ min_ਉਚਾਈ + 2;  // no overlap
    ਜੇ (dir == FORWARD)
	topline_ਪਿੱਠ(&loff);
    ਨਹੀਂਤਾਂ
	botline_forw(&loff);

    ਪੂਰਨ h3 = loff.ਉਚਾਈ;
    ਜੇ (h3 == MAXCOL || h3 + h2 > min_ਉਚਾਈ)
	ਵਾਪਸ min_ਉਚਾਈ + 2;  // no overlap
    ਜੇ (dir == FORWARD)
	topline_ਪਿੱਠ(&loff);
    ਨਹੀਂਤਾਂ
	botline_forw(&loff);

    ਪੂਰਨ h4 = loff.ਉਚਾਈ;
    ਜੇ (h4 == MAXCOL || h4 + h3 + h2 > min_ਉਚਾਈ || h3 + h2 + h1 > min_ਉਚਾਈ)
	ਵਾਪਸ min_ਉਚਾਈ + 1;  // 1 line overlap
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ min_ਉਚਾਈ;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
ਸਥਿਰ ਪੂਰਨ scroll_with_sms(ਪੂਰਨ dir, ਲੰਮਾ ਗਿਣਤੀ_2, ਲੰਮਾ *curscount)
{
    ਪੂਰਨ		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		prev_topfill = curwin->w_topfill;
#ਜੇ_ਅੰਤ

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, ਗਿਣਤੀ_2);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    ਜੇ (!prev_sms && curwin->w_skipcol > 0)
    {
	ਪੂਰਨ fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	ਜੇ (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	ਪੂਰਨ width1 = curwin->w_ਚੌੜਾਈ - curwin_ਖਾਨਾ_off();
	ਪੂਰਨ width2 = width1 + curwin_ਖਾਨਾ_off2();
	ਗਿਣਤੀ_2 = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	ਜੇ (fixdir == FORWARD)
	    ਗਿਣਤੀ_2 = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, ਗਿਣਤੀ_2);
	*curscount += ਗਿਣਤੀ_2 * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    ਵਾਪਸ curwin->w_topline != prev_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	|| curwin->w_topfill != prev_topfill
#ਜੇ_ਅੰਤ
	|| curwin->w_skipcol != prev_skipcol;
}

/*
 * Move screen "count" (half) pages up ("dir" is BACKWARD) or down ("dir" is
 * FORWARD) and update the screen. Handle moving the cursor and not scrolling
 * to reveal end of buffer lines for half-page scrolling with CTRL-D and CTRL-U.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
pagescroll(ਪੂਰਨ dir, ਲੰਮਾ ਗਿਣਤੀ_2, ਪੂਰਨ half)
{
    ਪੂਰਨ		did_move = FALSE;
    ਪੂਰਨ		buflen = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    colnr_T	prev_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    ਜੇ (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	ਜੇ (ਗਿਣਤੀ_2)
	    curwin->w_p_scr = MIN(curwin->w_ਉਚਾਈ, ਗਿਣਤੀ_2);
	ਗਿਣਤੀ_2 = MIN(curwin->w_ਉਚਾਈ, curwin->w_p_scr);

	ਲੰਮਾ curscount = ਗਿਣਤੀ_2;
	// Adjust count so as to not reveal end of buffer lines.
	ਜੇ (dir == FORWARD
		    && (curwin->w_topline + curwin->w_ਉਚਾਈ + ਗਿਣਤੀ_2 > buflen
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
							|| hasAnyFolding(curwin)
#ਜੇ_ਅੰਤ
	   ))
	{
	    ਪੂਰਨ n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    ਜੇ (n - ਗਿਣਤੀ_2 < curwin->w_ਉਚਾਈ && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_ਉਚਾਈ + ਗਿਣਤੀ_2);
	    ਜੇ (n < curwin->w_ਉਚਾਈ + ਗਿਣਤੀ_2)
		ਗਿਣਤੀ_2 = n - curwin->w_ਉਚਾਈ;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	ਜੇ (ਗਿਣਤੀ_2 > 0)
	{
	    did_move = scroll_with_sms(dir, ਗਿਣਤੀ_2, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.ਖਾਨਾ = prev_ਖਾਨਾ;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	ਜੇ (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	ਨਹੀਂਤਾਂ ਜੇ (dir == FORWARD)
	    cursor_ਥੱਲੇ_inner(curwin, curscount);
	ਨਹੀਂਤਾਂ
	    cursor_ਉੱਪਰ_inner(curwin, curscount);
    }
    ਨਹੀਂਤਾਂ
    {
	// Scroll [count] times 'window' or current window height lines.
	ਗਿਣਤੀ_2 *= ((ONE_ਖਿੜਕੀ && p_ਖਿੜਕੀ > 0 && p_ਖਿੜਕੀ < ਪੰਕਤੀਆਂ - 1) ?
				MAX(1, p_ਖਿੜਕੀ - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, ਗਿਣਤੀ_2, &ਗਿਣਤੀ_2);

	ਜੇ (did_move)
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

    ਜੇ (get_scrolloff_ਮੁੱਲ() > 0)
	cursor_correct();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#ਜੇ_ਅੰਤ
    did_move = did_move
	|| prev_ਖਾਨਾ != curwin->w_cursor.ਖਾਨਾ
	|| prev_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    ਜੇ (!did_move)
	beep_flush();
    ਨਹੀਂਤਾਂ ਜੇ (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    ਨਹੀਂਤਾਂ ਜੇ (p_sol)
	nv_g_home_m_cmd(&ca);

    ਵਾਪਸ did_move ? OK : FAIL;
}

    ਖਾਲੀ
do_check_cursorbind(ਖਾਲੀ)
{
    ਸਥਿਰ win_T	*prev_curwin = ਨਿਰਾ;
    ਸਥਿਰ buf_T	*prev_curbuf = ਨਿਰਾ;
    ਸਥਿਰ varnumber_T	prev_changedtick = 0;
    ਸਥਿਰ pos_T	prev_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    ਜੇ (curwin == prev_curwin && curbuf == prev_curbuf
	    && CHANGEDTICK(curbuf) == prev_changedtick
	    && EQUAL_POS(curwin->w_cursor, prev_cursor))
	ਵਾਪਸ;
    prev_curwin = curwin;
    prev_curbuf = curbuf;
    prev_changedtick = CHANGEDTICK(curbuf);
    prev_cursor = curwin->w_cursor;

    linenr_T	ਸਤਰ = curwin->w_cursor.lnum;
    colnr_T	ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    ਪੂਰਨ		ਸੈੱਟ_curswant = curwin->w_ਸੈੱਟ_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    ਪੂਰਨ		old_VIsual_select = VIsual_select;
    ਪੂਰਨ		old_VIsual_ਸਰਗਰਮ = VIsual_ਸਰਗਰਮ;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_ਸਰਗਰਮ = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_ਬਫਰ;
	// skip original window and windows with 'nocursorbind'
	ਜੇ (curwin != old_curwin && curwin->w_p_crb)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    ਜੇ (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_ਸਤਰ(old_curbuf, ਸਤਰ);
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		curwin->w_cursor.lnum = ਸਤਰ;
	    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_ਸੈੱਟ_curswant = ਸੈੱਟ_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    ਪੂਰਨ restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    ਜੇ (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    ਜੇ (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    ਜੇ (!curwin->w_p_scb)
		update_topline();
	    curwin->w_redr_ਸਥਿਤੀ = true;
	}
    }

    /*
     * reset current-window
     */
    VIsual_select = old_VIsual_select;
    VIsual_ਸਰਗਰਮ = old_VIsual_ਸਰਗਰਮ;
    curwin = old_curwin;
    curbuf = old_curbuf;
}
