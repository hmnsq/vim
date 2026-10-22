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

#అన్తర్భావ "vim.h"

స్థిర శూన్య redraw_for_cursorline(win_T *wp);
స్థిర పూర్ణాంకం scrolljump_విలువ(శూన్య);
స్థిర పూర్ణాంకం check_శిఖరం_స్థానభ్రంశం(శూన్య);
స్థిర శూన్య curs_అడ్డువరుసలు(win_T *wp);

రకనిర్వచనం నిర్మాణం
{
    linenr_T	    lnum;	// line number
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		    fill;	// filler lines
#అయితే_ముగింపు
    పూర్ణాంకం		    ఎత్తు;	// height of added line
} lineoff_T;

స్థిర శూన్య topline_వెనుక(lineoff_T *lp);
స్థిర శూన్య botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    స్థిర పూర్ణాంకం
adjust_plines_for_skipcol(win_T *wp)
{
    ఒకవేళ (wp->w_skipcol == 0)
	తిరిగి 0;

    పూర్ణాంకం వెడల్పు = wp->w_వెడల్పు - win_నిలువు_off(wp);
    పూర్ణాంకం w2 = వెడల్పు + win_నిలువు_off2(wp);
    ఒకవేళ (wp->w_skipcol >= వెడల్పు && w2 > 0)
	తిరిగి (wp->w_skipcol - వెడల్పు) / w2 + 1;

    తిరిగి 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    పూర్ణాంకం
plines_correct_topline(win_T *wp, linenr_T lnum, పూర్ణాంకం హద్దు_winheight)
{
    పూర్ణాంకం n;
#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    లేకపోతే
#అయితే_ముగింపు
	n = plines_win(wp, lnum, FALSE);
    ఒకవేళ (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    ఒకవేళ (హద్దు_winheight && n > wp->w_ఎత్తు)
	n = wp->w_ఎత్తు;
    తిరిగి n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    స్థిర శూన్య
comp_botline(win_T *wp)
{
    పూర్ణాంకం		n;
    linenr_T	lnum;
    పూర్ణాంకం		పూర్తి;
    పూర్ణాంకం		i = 0;
    పూర్ణాంకం		use_cache;
#నిర్వచితమైతే FEAT_FOLDING
    linenr_T    చివరి;
    పూర్ణాంకం		folded;
#అయితే_ముగింపు

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
		    && !wp->w_బఫరు->b_mod_సమితి
		    && dollar_vcol == -1
		    && wp->w_skipcol == 0
#నిర్వచితమైతే FEAT_DIFF
		    && !wp->w_p_diff
#అయితే_ముగింపు
		    && wp->w_పంక్తులు_valid > 0
		    && wp->w_పంక్తులు[0].wl_lnum <= wp->w_topline;

    ఒకవేళ (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	పూర్తి = wp->w_cline_అడ్డువరుస;
    }
    లేకపోతే
    {
	lnum = wp->w_topline;
	పూర్తి = 0;
    }

    // Find the w_lines[] entry for the starting line.
    ఒకవేళ (use_cache)
	అయితే (i < wp->w_పంక్తులు_valid && wp->w_పంక్తులు[i].wl_lnum < lnum)
	    ++i;

    కొరకు ( ; lnum <= wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క; ++i)
    {
	పూర్ణాంకం	valid = FALSE;

#నిర్వచితమైతే FEAT_FOLDING
	చివరి = lnum;
	folded = FALSE;
#అయితే_ముగింపు
	// Try to use the size from the previous redraw.
	ఒకవేళ (use_cache && i < wp->w_పంక్తులు_valid)
	{
	    ఒకవేళ (wp->w_పంక్తులు[i].wl_lnum < lnum || !wp->w_పంక్తులు[i].wl_valid)
		కొనసాగు;		// skip changed or deleted lines
	    ఒకవేళ (wp->w_పంక్తులు[i].wl_lnum == lnum)
		valid = TRUE;
	    లేకపోతే // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	ఒకవేళ (valid)
	{
	    n = wp->w_పంక్తులు[i].wl_కొలత;
#నిర్వచితమైతే FEAT_FOLDING
	    folded = wp->w_పంక్తులు[i].wl_folded;
	    చివరి = wp->w_పంక్తులు[i].wl_lastlnum;
#అయితే_ముగింపు
	}
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (hasFoldingWin(wp, lnum, ఖాళీ, &చివరి, TRUE, ఖాళీ))
	    {
		n = 1;
		folded = TRUE;
	    }
	    లేకపోతే
#అయితే_ముగింపు
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	ఒకవేళ (
#నిర్వచితమైతే FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && చివరి >= wp->w_cursor.lnum
#లేకపోతే
		lnum == wp->w_cursor.lnum
#అయితే_ముగింపు
	   )
	{
	    wp->w_cline_అడ్డువరుస = పూర్తి;
	    wp->w_cline_ఎత్తు = n;
#నిర్వచితమైతే FEAT_FOLDING
	    wp->w_cline_folded = folded;
#అయితే_ముగింపు
	    redraw_for_cursorline(wp);
	    wp->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	ఒకవేళ (పూర్తి + n > wp->w_ఎత్తు)
	    ఆపు;
	పూర్తి += n;
#నిర్వచితమైతే FEAT_FOLDING
	lnum = చివరి + 1;
#లేకపోతే
	++lnum;
#అయితే_ముగింపు
    }

    // wp->w_botline is the line that is just below the window
    wp->w_botline = lnum;
    wp->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    సమితి_ఖాళీలు_అడ్డువరుసలు(wp, పూర్తి);
}

/*
 * Redraw when w_cline_row changes and 'relativenumber' or 'cursorline' is
 * set.
 */
    స్థిర శూన్య
redraw_for_cursorline(win_T *wp)
{
    ఒకవేళ ((wp->w_p_rnu
#నిర్వచితమైతే FEAT_SYN_HL
		|| wp->w_p_cul
#అయితే_ముగింపు
		)
	    && (wp->w_valid & VALID_CROW) == 0
	    && !pum_visible())
    {
	// win_line() will redraw the number column and cursorline only.
	redraw_win_later(wp, UPD_VALID);
    }
}

#నిర్వచితమైతే FEAT_SYN_HL
/*
 * Redraw when w_virtcol changes and 'cursorcolumn' is set or 'cursorlineopt'
 * contains "screenline".
 */
    స్థిర శూన్య
redraw_for_cursorcolumn(win_T *wp)
{
    ఒకవేళ ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	ఒకవేళ (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	లేకపోతే ఒకవేళ (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#అయితే_ముగింపు

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    శూన్య
సమితి_valid_virtcol(win_T *wp, colnr_T vcol)
{
    wp->w_virtcol = vcol;
#నిర్వచితమైతే FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#అయితే_ముగింపు
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
    పూర్ణాంకం
sms_marker_overlap(win_T *wp, పూర్ణాంకం extra2)
{
    ఒకవేళ (extra2 == -1)
	extra2 = win_నిలువు_off(wp) - win_నిలువు_off2(wp);
#ఒకవేళ defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    ఒకవేళ (*get_showbreak_విలువ(wp) != NUL)
	తిరిగి 0;
#అయితే_ముగింపు
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    ఒకవేళ (wp->w_p_list && wp->w_lcs_chars.prec)
	తిరిగి 1;

    తిరిగి extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    స్థిర పూర్ణాంకం
skipcol_from_plines(win_T *wp, పూర్ణాంకం plines_off)
{
    పూర్ణాంకం width1 = wp->w_వెడల్పు - win_నిలువు_off(wp);

    పూర్ణాంకం skipcol = 0;
    ఒకవేళ (plines_off > 0)
	skipcol += width1;
    ఒకవేళ (plines_off > 1)
	skipcol += (width1 + win_నిలువు_off2(wp)) * (plines_off - 1);
    తిరిగి skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    స్థిర శూన్య
reset_skipcol(శూన్య)
{
    ఒకవేళ (curwin->w_skipcol == 0)
	తిరిగి;

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
    శూన్య
update_topline_redraw(శూన్య)
{
    update_topline();
    ఒకవేళ (must_redraw)
	update_screen(0);
}

/*
 * Return true when 'scrolloffpad' may augment 'scrolloff'.
 * This only applies to automatic cursor visibility correction.
 * For now 'scrolloffpad' is treated as boolean: 0 disables, > 0 enables.
 */
    స్థిర bool
use_scrolloffpad(శూన్య)
{
    తిరిగి get_scrolloff_విలువ() > 0 && get_scrolloffpad_విలువ() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    స్థిర bool
scrolloffpad_eof_pressure(linenr_T lnum, దీర్ఘ so)
{
    ఒకవేళ (!use_scrolloffpad() || so <= 0)
	తిరిగి false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    తిరిగి lnum > curbuf->b_ml.ml_పంక్తి_లెక్క - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    శూన్య
update_topline(శూన్య)
{
    దీర్ఘ	పంక్తి_లెక్క;
    పూర్ణాంకం		halfheight;
    పూర్ణాంకం		n;
#నిర్వచితమైతే FEAT_FOLDING
    linenr_T	lnum;
#అయితే_ముగింపు
    పూర్ణాంకం		check_topline = FALSE;
    పూర్ణాంకం		check_botline = FALSE;
    దీర్ఘ	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    పూర్ణాంకం		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    ఒకవేళ (skip_update_topline)
	తిరిగి;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    ఒకవేళ (!screen_valid(TRUE) || curwin->w_ఎత్తు == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	తిరిగి;
    }

    check_cursor_moved(curwin);
    ఒకవేళ (curwin->w_valid & VALID_TOPLINE)
	తిరిగి;

    // When dragging with the mouse, don't scroll that quickly
    ఒకవేళ (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం old_topfill = curwin->w_topfill;
#అయితే_ముగింపు

    /*
     * If the buffer is empty, always set topline to 1.
     */
    ఒకవేళ (BUFEMPTY())		// special case - file is empty
    {
	ఒకవేళ (curwin->w_topline != 1)
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
    లేకపోతే
    {
	ఒకవేళ (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    ఒకవేళ (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    లేకపోతే ఒకవేళ (check_శిఖరం_స్థానభ్రంశం())
		check_topline = TRUE;
	    లేకపోతే ఒకవేళ (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		పూర్ణాంకం overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, ఖాళీ, ఖాళీ, 0);
		overlap = sms_marker_overlap(curwin, -1);
		ఒకవేళ (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#నిర్వచితమైతే FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	ఒకవేళ (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#అయితే_ముగింపు

	ఒకవేళ (check_topline)
	{
	    halfheight = curwin->w_ఎత్తు / 2 - 1;
	    ఒకవేళ (halfheight < 2)
		halfheight = 2;

#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		కొరకు (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    ఒకవేళ (lnum >= curbuf->b_ml.ml_పంక్తి_లెక్క || n >= halfheight)
			ఆపు;
		    (శూన్య)hasFolding(lnum, ఖాళీ, &lnum);
		}
	    }
	    లేకపోతే
#అయితే_ముగింపు
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    పూర్ణాంకం min_scroll = scrolljump_విలువ();
	    ఒకవేళ (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    లేకపోతే ఒకవేళ (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    లేకపోతే
	    {
		scroll_cursor_శిఖరం(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	లేకపోతే
	{
#నిర్వచితమైతే FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (శూన్య)hasFolding(curwin->w_topline, &curwin->w_topline, ఖాళీ);
#అయితే_ముగింపు
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
    ఒకవేళ (check_botline)
    {
	ఒకవేళ (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	ఒకవేళ (curwin->w_botline <= curbuf->b_ml.ml_పంక్తి_లెక్క || use_scrolloffpad())
	{
	    ఒకవేళ (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		ఒకవేళ (((దీర్ఘ)curwin->w_cursor.lnum
					   >= (దీర్ఘ)curwin->w_botline - *so_ptr
#నిర్వచితమైతే FEAT_FOLDING
			|| hasAnyFolding(curwin)
#అయితే_ముగింపు
			))
		{
		    lineoff_T	loff;

		    // Cursor is (a few lines) above botline, check if there
		    // are 'scrolloff' window lines below the cursor.
		    // If not, need to scroll.
		    n = eof_pressure ? 0 : curwin->w_ఖాళీలు_అడ్డువరుసలు;
		    loff.lnum = curwin->w_cursor.lnum;
#నిర్వచితమైతే FEAT_FOLDING
		    // In a fold go to its last line.
		    (శూన్య)hasFolding(loff.lnum, ఖాళీ, &loff.lnum);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_అడ్డువరుసలు;
#అయితే_ముగింపు
		    loff.ఎత్తు = 0;
		    అయితే (loff.lnum < curwin->w_botline
#నిర్వచితమైతే FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#అయితే_ముగింపు
			    )
		    {
			n += loff.ఎత్తు;
			ఒకవేళ (n >= *so_ptr)
			    ఆపు;
			botline_forw(&loff);
		    }
		    ఒకవేళ (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		లేకపోతే
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    ఒకవేళ (check_botline)
	    {
#నిర్వచితమైతే FEAT_FOLDING
		ఒకవేళ (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    పంక్తి_లెక్క = 0;
		    కొరకు (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++పంక్తి_లెక్క;
			// stop at end of file or when we know we are far off
			ఒకవేళ (lnum <= 0 || పంక్తి_లెక్క > curwin->w_ఎత్తు + 1)
			    ఆపు;
			(శూన్య)hasFolding(lnum, &lnum, ఖాళీ);
		    }
		}
		లేకపోతే
#అయితే_ముగింపు
		    పంక్తి_లెక్క = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		ఒకవేళ (పంక్తి_లెక్క <= curwin->w_ఎత్తు + 1)
		{
		    ఒకవేళ (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    లేకపోతే
			scroll_cursor_bot(scrolljump_విలువ(), FALSE);
		}
		లేకపోతే
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    ఒకవేళ (curwin->w_topline != old_topline
#నిర్వచితమైతే FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#అయితే_ముగింపు
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	ఒకవేళ (!curwin->w_p_sms)
	    reset_skipcol();
	లేకపోతే ఒకవేళ (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	ఒకవేళ (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    స్థిర పూర్ణాంకం
scrolljump_విలువ(శూన్య)
{
    ఒకవేళ (p_sj >= 0)
	తిరిగి (పూర్ణాంకం)p_sj;
    తిరిగి (curwin->w_ఎత్తు * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    స్థిర పూర్ణాంకం
check_శిఖరం_స్థానభ్రంశం(శూన్య)
{
    lineoff_T	loff;
    పూర్ణాంకం		n;
    దీర్ఘ	so = get_scrolloff_విలువ();

    ఒకవేళ (curwin->w_cursor.lnum < curwin->w_topline + so
#నిర్వచితమైతే FEAT_FOLDING
		    || hasAnyFolding(curwin)
#అయితే_ముగింపు
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#నిర్వచితమైతే FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#లేకపోతే
	n = 0;
#అయితే_ముగింపు
	// Count the visible screen lines above the cursor line.
	అయితే (n < so)
	{
	    topline_వెనుక(&loff);
	    // Stop when included a line above the window.
	    ఒకవేళ (loff.lnum < curwin->w_topline
#నిర్వచితమైతే FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#అయితే_ముగింపు
		    )
		ఆపు;
	    n += loff.ఎత్తు;
	}
	ఒకవేళ (n < so)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Update w_curswant.
 */
    శూన్య
update_curswant_force(శూన్య)
{
    validate_virtcol();
    curwin->w_curswant = curwin->w_virtcol
#నిర్వచితమైతే FEAT_PROP_POPUP
	- curwin->w_virtcol_మొదటి_char
#అయితే_ముగింపు
	;
    curwin->w_సమితి_curswant = false;
}

/*
 * Update w_curswant if w_set_curswant is set.
 */
    శూన్య
update_curswant(శూన్య)
{
    ఒకవేళ (curwin->w_సమితి_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    శూన్య
check_cursor_moved(win_T *wp)
{
    ఒకవేళ (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    లేకపోతే ఒకవేళ (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    లేకపోతే ఒకవేళ (wp->w_cursor.నిలువు != wp->w_valid_cursor.నిలువు
	     || wp->w_leftcol != wp->w_valid_leftcol
	     || wp->w_cursor.coladd != wp->w_valid_cursor.coladd)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
	wp->w_valid_cursor.నిలువు = wp->w_cursor.నిలువు;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_cursor.coladd = wp->w_cursor.coladd;
    }
}

/*
 * Call this function when some window settings have changed, which require
 * the cursor position, botline and topline to be recomputed and the window to
 * be redrawn.  E.g, when changing the 'wrap' option or folding.
 */
    శూన్య
మారింది_కిటికీ_setting(శూన్య)
{
    మారింది_కిటికీ_setting_win(curwin);
}

    శూన్య
మారింది_కిటికీ_setting_win(win_T *wp)
{
    wp->w_పంక్తులు_valid = 0;
    మారింది_పంక్తి_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    శూన్య
మారింది_కిటికీ_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ఒకవేళ (wp->w_బఫరు == buf)
	    మారింది_కిటికీ_setting_win(wp);
}
#అయితే_ముగింపు

/*
 * Call changed_window_setting_win() for every window.
 */
    శూన్య
మారింది_కిటికీ_setting_all(శూన్య)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	మారింది_కిటికీ_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    శూన్య
సమితి_topline(win_T *wp, linenr_T lnum)
{
#నిర్వచితమైతే FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_FOLDING
    // go to first of folded lines
    (శూన్య)hasFoldingWin(wp, lnum, &lnum, ఖాళీ, TRUE, ఖాళీ);
#అయితే_ముగింపు
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    ఒకవేళ (wp->w_botline > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క + 1)
	wp->w_botline = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_సమితి = true;
#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (lnum != prev_topline)
	// Keep the filler lines when the topline didn't change.
	wp->w_topfill = 0;
#అయితే_ముగింపు
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
    శూన్య
మారింది_cline_bef_curs(శూన్య)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    శూన్య
మారింది_cline_bef_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

/*
 * Call this function when the length of a line (in screen characters) above
 * the cursor have changed.
 * Need to take care of w_botline separately!
 */
    శూన్య
మారింది_పంక్తి_abv_curs(శూన్య)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    శూన్య
మారింది_పంక్తి_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    శూన్య
మారింది_పంక్తి_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_బఫరు == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#అయితే_ముగింపు

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    శూన్య
validate_botline(శూన్య)
{
    validate_botline_win(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    శూన్య
validate_botline_win(win_T *wp)
{
    ఒకవేళ (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    శూన్య
invalidate_botline(శూన్య)
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    శూన్య
invalidate_botline_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    శూన్య
approximate_botline_win(
    win_T	*wp)
{
    wp->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    పూర్ణాంకం
cursor_valid(శూన్య)
{
    check_cursor_moved(curwin);
    తిరిగి ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    శూన్య
validate_cursor(శూన్య)
{
    check_cursor_lnum();
    check_cursor_moved(curwin);
    ఒకవేళ ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#ఒకవేళ defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    శూన్య
validate_cline_అడ్డువరుస(శూన్య)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    ఒకవేళ (!(curwin->w_valid & VALID_CROW))
	curs_అడ్డువరుసలు(curwin);
}
#అయితే_ముగింపు

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    స్థిర శూన్య
curs_అడ్డువరుసలు(win_T *wp)
{
    linenr_T	lnum;
    పూర్ణాంకం		i;
    పూర్ణాంకం		all_invalid;
    పూర్ణాంకం		valid;
#నిర్వచితమైతే FEAT_FOLDING
    దీర్ఘ	fold_లెక్క;
#అయితే_ముగింపు

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_పంక్తులు_valid == 0
			|| wp->w_పంక్తులు[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_అడ్డువరుస = 0;
    కొరకు (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	ఒకవేళ (!all_invalid && i < wp->w_పంక్తులు_valid)
	{
	    ఒకవేళ (wp->w_పంక్తులు[i].wl_lnum < lnum || !wp->w_పంక్తులు[i].wl_valid)
		కొనసాగు;		// skip changed or deleted lines
	    ఒకవేళ (wp->w_పంక్తులు[i].wl_lnum == lnum)
	    {
#నిర్వచితమైతే FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		ఒకవేళ (!wp->w_బఫరు->b_mod_సమితి
			|| wp->w_పంక్తులు[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_బఫరు->b_mod_శిఖరం
					     > wp->w_పంక్తులు[i].wl_lastlnum + 1)
#అయితే_ముగింపు
		valid = TRUE;
	    }
	    లేకపోతే ఒకవేళ (wp->w_పంక్తులు[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	ఒకవేళ (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#నిర్వచితమైతే FEAT_DIFF
					    && !wp->w_p_diff
#అయితే_ముగింపు
	       )))
	{
#నిర్వచితమైతే FEAT_FOLDING
	    lnum = wp->w_పంక్తులు[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    ఒకవేళ (lnum > wp->w_cursor.lnum)
		ఆపు;
#లేకపోతే
	    ++lnum;
#అయితే_ముగింపు
	    wp->w_cline_అడ్డువరుస += wp->w_పంక్తులు[i].wl_కొలత;
	}
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_FOLDING
	    fold_లెక్క = folded_లెక్క(wp, lnum, ఖాళీ);
	    ఒకవేళ (fold_లెక్క)
	    {
		lnum += fold_లెక్క;
		ఒకవేళ (lnum > wp->w_cursor.lnum)
		    ఆపు;
		++wp->w_cline_అడ్డువరుస;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		wp->w_cline_అడ్డువరుస += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    ఒకవేళ (!(wp->w_valid & VALID_CHEIGHT))
    {
	ఒకవేళ (all_invalid
		|| i == wp->w_పంక్తులు_valid
		|| (i < wp->w_పంక్తులు_valid
		    && (!wp->w_పంక్తులు[i].wl_valid
			|| wp->w_పంక్తులు[i].wl_lnum != wp->w_cursor.lnum)))
	{
#నిర్వచితమైతే FEAT_DIFF
	    ఒకవేళ (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_ఎత్తు = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    లేకపోతే
#అయితే_ముగింపు
		wp->w_cline_ఎత్తు = plines_win(wp, wp->w_cursor.lnum, TRUE);
#నిర్వచితమైతే FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ఖాళీ, ఖాళీ, TRUE, ఖాళీ);
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (i > wp->w_పంక్తులు_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_ఎత్తు = 0;
#నిర్వచితమైతే FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ఖాళీ, ఖాళీ, TRUE, ఖాళీ);
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    wp->w_cline_ఎత్తు = wp->w_పంక్తులు[i].wl_కొలత;
#నిర్వచితమైతే FEAT_FOLDING
	    wp->w_cline_folded = wp->w_పంక్తులు[i].wl_folded;
#అయితే_ముగింపు
	}
    }

    redraw_for_cursorline(curwin);
    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;
}

/*
 * Validate curwin->w_virtcol only.
 */
    శూన్య
validate_virtcol(శూన్య)
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    శూన్య
validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);

    ఒకవేళ (wp->w_valid & VALID_VIRTCOL)
	తిరిగి;

#నిర్వచితమైతే FEAT_PROP_POPUP
    wp->w_virtcol_మొదటి_char = 0;
#అయితే_ముగింపు
    getvvcol(wp, &wp->w_cursor, ఖాళీ, &(wp->w_virtcol), ఖాళీ,
							 GETVCOL_FOR_VIRTCOL);
#నిర్వచితమైతే FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#అయితే_ముగింపు
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    శూన్య
validate_cheight(శూన్య)
{
    check_cursor_moved(curwin);

    ఒకవేళ (curwin->w_valid & VALID_CHEIGHT)
	తిరిగి;

#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_ఎత్తు = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    లేకపోతే
#అయితే_ముగింపు
	curwin->w_cline_ఎత్తు = plines(curwin->w_cursor.lnum);
#నిర్వచితమైతే FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, ఖాళీ, ఖాళీ);
#అయితే_ముగింపు
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    శూన్య
validate_cursor_నిలువు(శూన్య)
{
    colnr_T off;
    colnr_T నిలువు;
    పూర్ణాంకం     వెడల్పు;

    validate_virtcol();

    ఒకవేళ (curwin->w_valid & VALID_WCOL)
	తిరిగి;

    నిలువు = curwin->w_virtcol;
    off = curwin_నిలువు_off();
    నిలువు += off;
    వెడల్పు = curwin->w_వెడల్పు - off + curwin_నిలువు_off2();

    // long line wrapping, adjust curwin->w_wrow
    ఒకవేళ (curwin->w_p_wrap
	    && నిలువు >= (colnr_T)curwin->w_వెడల్పు
	    && వెడల్పు > 0)
	// use same formula as what is used in curs_columns()
	నిలువు -= ((నిలువు - curwin->w_వెడల్పు) / వెడల్పు + 1) * వెడల్పు;
    ఒకవేళ (నిలువు > (పూర్ణాంకం)curwin->w_leftcol)
	నిలువు -= curwin->w_leftcol;
    లేకపోతే
	నిలువు = 0;
    curwin->w_wcol = నిలువు;

    curwin->w_valid |= VALID_WCOL;
#నిర్వచితమైతే FEAT_PROP_POPUP
    curwin->w_flags &= ~WFLAG_WCOL_OFF_ADDED;
#అయితే_ముగింపు
}

/*
 * Compute offset of a window, occupied by absolute or relative line number,
 * fold column and sign column (these don't move when scrolling horizontally).
 */
    పూర్ణాంకం
win_నిలువు_off(win_T *wp)
{
    తిరిగి (((wp->w_p_nu || wp->w_p_rnu) ? సంఖ్య_వెడల్పు(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#నిర్వచితమైతే FEAT_FOLDING
	    + wp->w_p_fdc
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#అయితే_ముగింపు
	   );
}

    పూర్ణాంకం
curwin_నిలువు_off(శూన్య)
{
    తిరిగి win_నిలువు_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    పూర్ణాంకం
win_నిలువు_off2(win_T *wp)
{
    ఒకవేళ ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != ఖాళీ)
	తిరిగి సంఖ్య_వెడల్పు(wp) + 1;
    తిరిగి 0;
}

    పూర్ణాంకం
curwin_నిలువు_off2(శూన్య)
{
    తిరిగి win_నిలువు_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    శూన్య
curs_columns(
    పూర్ణాంకం		may_scroll)	// when TRUE, may scroll horizontally
{
    దీర్ఘ	diff;
    పూర్ణాంకం		extra;		// offset for first screen line
    దీర్ఘ	off_ఎడమ, off_కుడి;
    పూర్ణాంకం		n;
    పూర్ణాంకం		p_పంక్తులు;
    పూర్ణాంకం		width1;		// text width for first screen line
    పూర్ణాంకం		width2 = 0;	// text width for second and later screen line
    పూర్ణాంకం		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    దీర్ఘ	so = get_scrolloff_విలువ();
    దీర్ఘ	siso = get_sidescrolloff_విలువ();
    పూర్ణాంకం		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    ఒకవేళ (!(curwin->w_valid & VALID_CROW))
	curs_అడ్డువరుసలు(curwin);

#నిర్వచితమైతే FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_మొదటి_char = 0;
#అయితే_ముగింపు

    /*
     * Compute the number of virtual columns.
     */
#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    లేకపోతే
#అయితే_ముగింపు
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    ఒకవేళ (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_నిలువు_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_అడ్డువరుస;

    width1 = curwin->w_వెడల్పు - extra;
    ఒకవేళ (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_వెడల్పు - 1;
	ఒకవేళ (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_ఎత్తు - 1;
	లేకపోతే
	    curwin->w_wrow = curwin->w_ఎత్తు - 1 - curwin->w_ఖాళీలు_అడ్డువరుసలు;
    }
    లేకపోతే ఒకవేళ (curwin->w_p_wrap && curwin->w_వెడల్పు != 0)
    {
	width2 = width1 + curwin_నిలువు_off2();

	// skip columns that are not visible
	ఒకవేళ (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    ఒకవేళ (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    లేకపోతే
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	ఒకవేళ (curwin->w_wcol >= curwin->w_వెడల్పు)
	{
	    // this same formula is used in validate_cursor_col()
	    n = (curwin->w_wcol - curwin->w_వెడల్పు) / width2 + 1;
	    curwin->w_wcol -= n * width2;
	    curwin->w_wrow += n;
	}
    }

    // No line wrapping: compute curwin->w_leftcol if scrolling is on and line
    // is not folded.
    // If scrolling is off, curwin->w_leftcol is assumed to be 0
    లేకపోతే ఒకవేళ (may_scroll
#నిర్వచితమైతే FEAT_FOLDING
	    && !curwin->w_cline_folded
#అయితే_ముగింపు
	    )
    {
#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (curwin->w_virtcol_మొదటి_char > 0)
	{
	    పూర్ణాంకం నిలువుల_సంఖ్య = (curwin->w_వెడల్పు - extra);
	    పూర్ణాంకం అడ్డువరుసలు_2 = నిలువుల_సంఖ్య > 0 ? curwin->w_virtcol_మొదటి_char / నిలువుల_సంఖ్య : 1;

	    // each "above" text prop shifts the text one row down
	    curwin->w_wrow += అడ్డువరుసలు_2;
	    curwin->w_wcol -= అడ్డువరుసలు_2 * నిలువుల_సంఖ్య;
	    endcol -= అడ్డువరుసలు_2 * నిలువుల_సంఖ్య;
	    curwin->w_cline_ఎత్తు = అడ్డువరుసలు_2 + 1;
	}
#అయితే_ముగింపు
	/*
	 * If Cursor is left of the screen, scroll rightwards.
	 * If Cursor is right of the screen, scroll leftwards.
	 * If we get closer to the edge than 'sidescrolloff', scroll a little
	 * extra.
	 */
	off_ఎడమ = startcol - curwin->w_leftcol - siso;
	off_కుడి = endcol - curwin->w_leftcol - (curwin->w_వెడల్పు - siso) + 1;
	ఒకవేళ (off_ఎడమ < 0 || off_కుడి > 0)
	{
	    ఒకవేళ (off_ఎడమ < 0)
		diff = -off_ఎడమ;
	    లేకపోతే
		diff = off_కుడి;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    ఒకవేళ (p_ss == 0 || diff >= width1 / 2 || off_కుడి >= off_ఎడమ)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    లేకపోతే
	    {
		ఒకవేళ (diff < p_ss)
		    diff = p_ss;
		ఒకవేళ (off_ఎడమ < 0)
		    new_leftcol = curwin->w_leftcol - (పూర్ణాంకం)diff;
		లేకపోతే
		    new_leftcol = curwin->w_leftcol + (పూర్ణాంకం)diff;
	    }
	    ఒకవేళ (new_leftcol < 0)
		new_leftcol = 0;
	    ఒకవేళ (new_leftcol != (పూర్ణాంకం)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    లేకపోతే ఒకవేళ (curwin->w_wcol > (పూర్ణాంకం)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    లేకపోతే
	curwin->w_wcol = 0;

#నిర్వచితమైతే FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    ఒకవేళ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    లేకపోతే
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#అయితే_ముగింపు

    prev_skipcol = curwin->w_skipcol;

    p_పంక్తులు = 0;

    ఒకవేళ ((curwin->w_wrow >= curwin->w_ఎత్తు
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_ఎత్తు)
		    && (p_పంక్తులు =
#నిర్వచితమైతే FEAT_DIFF
			plines_win_nofill
#లేకపోతే
			plines_win
#అయితే_ముగింపు
			(curwin, curwin->w_cursor.lnum, FALSE))
						    - 1 >= curwin->w_ఎత్తు))
	    && curwin->w_ఎత్తు != 0
	    && curwin->w_cursor.lnum == curwin->w_topline
	    && width2 > 0
	    && curwin->w_వెడల్పు != 0)
    {
	// Cursor past end of screen.  Happens with a single line that does
	// not fit on screen.  Find a skipcol to show the text around the
	// cursor.  Avoid scrolling all the time. compute value of "extra":
	// 1: Less than 'scrolloff' lines above
	// 2: Less than 'scrolloff' lines below
	// 3: both of them
	extra = 0;
	ఒకవేళ (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	ఒకవేళ (p_పంక్తులు == 0)
	    p_పంక్తులు = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_పంక్తులు;
	ఒకవేళ (p_పంక్తులు > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	లేకపోతే
	    n = p_పంక్తులు;
	ఒకవేళ ((colnr_T)n >= curwin->w_ఎత్తు + curwin->w_skipcol / width2 - so)
	    extra += 2;

	ఒకవేళ (extra == 3 || curwin->w_ఎత్తు <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    ఒకవేళ (n > curwin->w_ఎత్తు / 2)
		n -= curwin->w_ఎత్తు / 2;
	    లేకపోతే
		n = 0;
	    // don't skip more than necessary
	    ఒకవేళ (n > p_పంక్తులు - curwin->w_ఎత్తు + 1)
		n = p_పంక్తులు - curwin->w_ఎత్తు + 1;
	    ఒకవేళ (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    లేకపోతే
		curwin->w_skipcol = 0;
	}
	లేకపోతే ఒకవేళ (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    ఒకవేళ (extra > 0)
	    {
		ఒకవేళ ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	లేకపోతే ఒకవేళ (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_ఎత్తు + 1) * width2;
	    అయితే (endcol > curwin->w_virtcol)
		endcol -= width2;
	    ఒకవేళ (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	ఒకవేళ (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - prev_skipcol) / width2;
	లేకపోతే
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	ఒకవేళ (curwin->w_wrow >= curwin->w_ఎత్తు)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_ఎత్తు + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((పూర్ణాంకం)prev_skipcol - (పూర్ణాంకం)curwin->w_skipcol) / width2;
	ఒకవేళ (extra > 0)
	    win_ins_పంక్తులు(curwin, 0, extra, FALSE, FALSE);
	లేకపోతే ఒకవేళ (extra < 0)
	    win_del_పంక్తులు(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    లేకపోతే ఒకవేళ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    ఒకవేళ (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#నిర్వచితమైతే FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ఒకవేళ (popup_is_popup(curwin) && curbuf->b_పదం != ఖాళీ)
    {
	curwin->w_wrow += popup_శిఖరం_extra(curwin);
	curwin->w_wcol += popup_ఎడమ_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    లేకపోతే
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#అయితే_ముగింపు

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    శూన్య
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	పూర్ణాంకం	*rowp,	// screen row
	పూర్ణాంకం	*scolp,	// start screen column
	పూర్ణాంకం	*ccolp,	// cursor screen column
	పూర్ణాంకం	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    పూర్ణాంకం		అడ్డువరుస = 0;
    colnr_T	coloff = 0;

    ఒకవేళ (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    నిలువు;
	పూర్ణాంకం	    వెడల్పు;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	పూర్ణాంకం	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, ఖాళీ, TRUE, ఖాళీ);
# endif
	అడ్డువరుస = plines_m_win(wp, wp->w_topline, lnum - 1, గరిష్ఠ_పూర్ణాంకం);
	// "row" should be the screen line where line "lnum" begins, which can
	// be negative if "lnum" is "w_topline" and "w_skipcol" is non-zero.
	అడ్డువరుస -= adjust_plines_for_skipcol(wp);

# ifdef FEAT_DIFF
	// Add filler lines above this buffer line.
	అడ్డువరుస += lnum == wp->w_topline ? wp->w_topfill
				     : diff_check_fill(wp, lnum);
# endif

	colnr_T	off = win_నిలువు_off(wp);
# ifdef FEAT_FOLDING
	ఒకవేళ (is_folded)
	{
	    అడ్డువరుస += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	లేకపోతే
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    నిలువు = scol;
	    నిలువు += off;
	    వెడల్పు = wp->w_వెడల్పు - off + win_నిలువు_off2(wp);

	    // long line wrapping, adjust row
	    ఒకవేళ (wp->w_p_wrap
		    && నిలువు >= (colnr_T)wp->w_వెడల్పు
		    && వెడల్పు > 0)
	    {
		// use same formula as what is used in curs_columns()
		పూర్ణాంకం rowoff = ((నిలువు - wp->w_వెడల్పు) / వెడల్పు + 1);
		నిలువు -= rowoff * వెడల్పు;
		అడ్డువరుస += rowoff;
	    }
	    నిలువు -= wp->w_leftcol;
	    ఒకవేళ (నిలువు >= wp->w_వెడల్పు)
		నిలువు = -1;
	    ఒకవేళ (నిలువు >= 0 && అడ్డువరుస >= 0 && అడ్డువరుస < wp->w_ఎత్తు)
	    {
		coloff = నిలువు - scol + wp->w_wincol + 1;
		అడ్డువరుస += W_WINROW(wp) + 1;
	    }
	    లేకపోతే
		// character is out of the window
		అడ్డువరుస = scol = ccol = ecol = 0;
	}
    }
    *rowp = అడ్డువరుస;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    ఒకవేళ (wp->w_p_rl && అడ్డువరుస > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	పూర్ణాంకం endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_వెడల్పు - endoff;
    }
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    శూన్య
f_screenpos(typval_T *argvars వాడని, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    పూర్ణాంకం		అడ్డువరుస = 0;
    పూర్ణాంకం		scol = 0, ccol = 0, ecol = 0;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
    dict = rettv->vval.v_dict;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 2) == FAIL))
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    pos.lnum = tv_get_సంఖ్య(&argvars[1]);
    ఒకవేళ (pos.lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
    {
	semsg(_(e_invalid_పంక్తి_సంఖ్య_nr), pos.lnum);
	తిరిగి;
    }
    pos.నిలువు = tv_get_సంఖ్య(&argvars[2]) - 1;
    ఒకవేళ (pos.నిలువు < 0)
	pos.నిలువు = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &అడ్డువరుస, &scol, &ccol, &ecol);

    dict_కూడు_సంఖ్య(dict, "row", అడ్డువరుస);
    dict_కూడు_సంఖ్య(dict, "col", scol);
    dict_కూడు_సంఖ్య(dict, "curscol", ccol);
    dict_కూడు_సంఖ్య(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    స్థిర పూర్ణాంకం
virtcol2col(win_T *wp, linenr_T lnum, పూర్ణాంకం vcol)
{
    పూర్ణాంకం		స్థానభ్రంశం = vcol2col(wp, lnum, vcol - 1, ఖాళీ);
    char_u	*పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
    char_u	*p = పంక్తి + స్థానభ్రంశం;

    ఒకవేళ (*p == NUL)
    {
	ఒకవేళ (p == పంక్తి)  // empty line
	    తిరిగి 0;
	// Move to the first byte of the last char.
	MB_PTR_వెనుక(పంక్తి, p);
    }
    తిరిగి (పూర్ణాంకం)(p - పంక్తి + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    శూన్య
f_virtcol2col(typval_T *argvars వాడని, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    పూర్ణాంకం		screencol;
    పూర్ణాంకం		error = FALSE;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (check_for_సంఖ్య_arg(argvars, 0) == FAIL
	    || check_for_సంఖ్య_arg(argvars, 1) == FAIL
	    || check_for_సంఖ్య_arg(argvars, 2) == FAIL)
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    lnum = tv_get_సంఖ్య_chk(&argvars[1], &error);
    ఒకవేళ (error || lnum < 0 || lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి;

    screencol = tv_get_సంఖ్య_chk(&argvars[2], &error);
    ఒకవేళ (error || screencol < 0)
	తిరిగి;

    rettv->vval.v_సంఖ్య = virtcol2col(wp, lnum, screencol);
}
#అయితే_ముగింపు

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
స్థిర శూన్య cursor_correct_sms(శూన్య)
{
    ఒకవేళ (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	తిరిగి;

    దీర్ఘ    so = get_scrolloff_విలువ();
    పూర్ణాంకం	    width1 = curwin->w_వెడల్పు - curwin_నిలువు_off();
    పూర్ణాంకం	    width2 = width1 + curwin_నిలువు_off2();
    పూర్ణాంకం	    so_నిలువుల_సంఖ్య = so == 0 ? 0 : width1 + (so - 1) * width2;
    పూర్ణాంకం	    space_నిలువుల_సంఖ్య = (curwin->w_ఎత్తు - 1) * width2;
    పూర్ణాంకం	    overlap, శిఖరం, bot;
    పూర్ణాంకం	    కొలత = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    ఒకవేళ (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_నిలువుల_సంఖ్య = 0;               // Ignore 'scrolloff' at top of buffer.
    లేకపోతే ఒకవేళ (so_నిలువుల_సంఖ్య > space_నిలువుల_సంఖ్య / 2)
	so_నిలువుల_సంఖ్య = space_నిలువుల_సంఖ్య / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    అయితే (so_నిలువుల_సంఖ్య > కొలత && so_నిలువుల_సంఖ్య - width2 >= width1 && width1 > 0)
	so_నిలువుల_సంఖ్య -= width2;
    ఒకవేళ (so_నిలువుల_సంఖ్య >= width1 && so_నిలువుల_సంఖ్య > కొలత)
	so_నిలువుల_సంఖ్య -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_వెడల్పు - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    శిఖరం = curwin->w_skipcol + (so_నిలువుల_సంఖ్య != 0 ? so_నిలువుల_సంఖ్య : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_ఎత్తు - 1) * width2
								    - so_నిలువుల_సంఖ్య;
    validate_virtcol();
    colnr_T నిలువు = curwin->w_virtcol;

    ఒకవేళ (నిలువు < శిఖరం)
    {
	ఒకవేళ (నిలువు < width1)
	    నిలువు += width1;
	అయితే (width2 > 0 && నిలువు < శిఖరం)
	    నిలువు += width2;
    }
    లేకపోతే
	అయితే (width2 > 0 && నిలువు >= bot)
	    నిలువు -= width2;

    ఒకవేళ (నిలువు != curwin->w_virtcol)
    {
	పూర్ణాంకం rc;

	curwin->w_curswant = నిలువు;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	ఒకవేళ (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    validate_virtcol();
	    ఒకవేళ (curwin->w_virtcol < curwin->w_skipcol + overlap)
	    {
		// Cursor still not visible: move it to the next line instead.
		curwin->w_cursor.lnum++;
		curwin->w_cursor.నిలువు = 0;
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
    శూన్య
scroll_redraw(పూర్ణాంకం పైకి, దీర్ఘ లెక్క)
{
    linenr_T	prev_topline = curwin->w_topline;
    పూర్ణాంకం		prev_skipcol = curwin->w_skipcol;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		prev_topfill = curwin->w_topfill;
#అయితే_ముగింపు
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    ఒకవేళ (పైకి)
	scrollup(లెక్క, TRUE);
    లేకపోతే
	scrolldown(లెక్క, TRUE);
    ఒకవేళ (get_scrolloff_విలువ() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	అయితే (curwin->w_topline == prev_topline
		&& curwin->w_skipcol == prev_skipcol
#నిర్వచితమైతే FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#అయితే_ముగింపు
		)
	{
	    ఒకవేళ (పైకి)
	    {
		ఒకవేళ (curwin->w_cursor.lnum > prev_lnum
			|| cursor_కిందికి(1L, FALSE) == FAIL)
		    ఆపు;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_పైకి(1L, FALSE) == FAIL)
		    ఆపు;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    ఒకవేళ (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    శూన్య
scrolldown(
    దీర్ఘ	పంక్తి_లెక్క,
    పూర్ణాంకం		byfold వాడని)	// TRUE: count a closed fold as one line
{
    దీర్ఘ	పూర్తి = 0;	// total # of physical lines done
    పూర్ణాంకం		wrow;
    పూర్ణాంకం		moved = FALSE;
    పూర్ణాంకం		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    పూర్ణాంకం		width1 = 0;
    పూర్ణాంకం		width2 = 0;

    ఒకవేళ (do_sms)
    {
	width1 = curwin->w_వెడల్పు - curwin_నిలువు_off();
	width2 = width1 + curwin_నిలువు_off2();
    }

#నిర్వచితమైతే FEAT_FOLDING
    linenr_T	మొదటి;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (శూన్య)hasFolding(curwin->w_topline, &curwin->w_topline, ఖాళీ);
#అయితే_ముగింపు
    validate_cursor();		// w_wrow needs to be valid
    కొరకు (పూర్ణాంకం todo = పంక్తి_లెక్క; todo > 0; --todo)
    {
#నిర్వచితమైతే FEAT_DIFF
	ఒకవేళ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_ఎత్తు - 1)
	{
	    ++curwin->w_topfill;
	    ++పూర్తి;
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    // break when at the very top
	    ఒకవేళ (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		ఆపు;
	    ఒకవేళ (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		ఒకవేళ (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		లేకపోతే
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++పూర్తి;
	    }
	    లేకపోతే
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#నిర్వచితమైతే FEAT_DIFF
		curwin->w_topfill = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		ఒకవేళ (hasFolding(curwin->w_topline, &మొదటి, ఖాళీ))
		{
		    ++పూర్తి;
		    ఒకవేళ (!byfold)
			todo -= curwin->w_topline - మొదటి - 1;
		    curwin->w_botline -= curwin->w_topline - మొదటి;
		    curwin->w_topline = మొదటి;
		}
		లేకపోతే
#అయితే_ముగింపు
		ఒకవేళ (do_sms)
		{
		    పూర్ణాంకం కొలత = linetabsize_eol(curwin, curwin->w_topline);
		    ఒకవేళ (కొలత > width1)
		    {
			curwin->w_skipcol = width1;
			కొలత -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    అయితే (కొలత > width2)
		    {
			curwin->w_skipcol += width2;
			కొలత -= width2;
		    }
		    ++పూర్తి;
		}
		లేకపోతే
		    పూర్తి += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += పూర్తి;		// keep w_wrow updated
    curwin->w_cline_అడ్డువరుస += పూర్తి;	// keep w_cline_row updated

#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_అడ్డువరుస = 0;
    check_topfill(curwin, TRUE);
#అయితే_ముగింపు

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    ఒకవేళ (curwin->w_p_wrap && curwin->w_వెడల్పు != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_ఎత్తు - 1 -
	    curwin->w_virtcol / curwin->w_వెడల్పు;
    }
    అయితే (wrow >= curwin->w_ఎత్తు && curwin->w_cursor.lnum > 1)
    {
#నిర్వచితమైతే FEAT_FOLDING
	ఒకవేళ (hasFolding(curwin->w_cursor.lnum, &మొదటి, ఖాళీ))
	{
	    --wrow;
	    ఒకవేళ (మొదటి == 1)
		curwin->w_cursor.lnum = 1;
	    లేకపోతే
		curwin->w_cursor.lnum = మొదటి - 1;
	}
	లేకపోతే
#అయితే_ముగింపు
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    ఒకవేళ (moved)
    {
#నిర్వచితమైతే FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#అయితే_ముగింపు
	coladvance(curwin->w_curswant);
    }
    ఒకవేళ (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    శూన్య
scrollup(
    దీర్ఘ	పంక్తి_లెక్క,
    పూర్ణాంకం		byfold వాడని)	// TRUE: count a closed fold as one line
{
    పూర్ణాంకం		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    ఒకవేళ (do_sms
#నిర్వచితమైతే FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#అయితే_ముగింపు
       )
    {
	పూర్ణాంకం	    width1 = curwin->w_వెడల్పు - curwin_నిలువు_off();
	పూర్ణాంకం	    width2 = width1 + curwin_నిలువు_off2();
	పూర్ణాంకం	    కొలత = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	ఒకవేళ (do_sms)
	    కొలత = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	కొరకు (పూర్ణాంకం todo = పంక్తి_లెక్క; todo > 0; --todo)
	{
#నిర్వచితమైతే FEAT_DIFF
	    ఒకవేళ (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    లేకపోతే
#అయితే_ముగింపు
	    {
		linenr_T lnum = curwin->w_topline;

#నిర్వచితమైతే FEAT_FOLDING
		ఒకవేళ (byfold)
		    // for a closed fold: go to the last line in the fold
		    (శూన్య)hasFolding(lnum, ఖాళీ, &lnum);
#అయితే_ముగింపు
		ఒకవేళ (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    పూర్ణాంకం కూడు = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += కూడు;
		    ఒకవేళ (curwin->w_skipcol >= కొలత)
		    {
			ఒకవేళ (lnum == curbuf->b_ml.ml_పంక్తి_లెక్క)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= కూడు;
			    ఆపు;
			}
			++lnum;
		    }
		}
		లేకపోతే
		{
		    ఒకవేళ (lnum >= curbuf->b_ml.ml_పంక్తి_లెక్క)
			ఆపు;
		    ++lnum;
		}

		ఒకవేళ (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#నిర్వచితమైతే FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#అయితే_ముగింపు
		    curwin->w_skipcol = 0;
		    ఒకవేళ (todo > 1 && do_sms)
			కొలత = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	ఒకవేళ (prev_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    లేకపోతే
    {
	curwin->w_topline += పంక్తి_లెక్క;
	curwin->w_botline += పంక్తి_లెక్క;	// approximate w_botline
    }

    ఒకవేళ (curwin->w_topline > curbuf->b_ml.ml_పంక్తి_లెక్క)
	curwin->w_topline = curbuf->b_ml.ml_పంక్తి_లెక్క;
    ఒకవేళ (curwin->w_botline > curbuf->b_ml.ml_పంక్తి_లెక్క + 1)
	curwin->w_botline = curbuf->b_ml.ml_పంక్తి_లెక్క + 1;

#నిర్వచితమైతే FEAT_DIFF
    check_topfill(curwin, FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(శూన్య)hasFolding(curwin->w_topline, &curwin->w_topline, ఖాళీ);
#అయితే_ముగింపు

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    ఒకవేళ (curwin->w_cursor.lnum < curwin->w_topline)
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
    శూన్య
adjust_skipcol(శూన్య)
{
    ఒకవేళ (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	తిరిగి;

    పూర్ణాంకం	    width1 = curwin->w_వెడల్పు - curwin_నిలువు_off();
    ఒకవేళ (width1 <= 0)
	తిరిగి;  // no text will be displayed

    పూర్ణాంకం	    width2 = width1 + curwin_నిలువు_off2();
    దీర్ఘ    so = get_scrolloff_విలువ();
    పూర్ణాంకం	    scrolloff_నిలువుల_సంఖ్య = so == 0 ? 0 : width1 + (so - 1) * width2;
    పూర్ణాంకం	    scrolled = FALSE;
    పూర్ణాంకం	    అడ్డువరుస = 0;
    పూర్ణాంకం	    overlap, నిలువు;

    validate_cheight();
    ఒకవేళ (curwin->w_cline_ఎత్తు == curwin->w_ఎత్తు
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_ఎత్తు)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	తిరిగి;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_వెడల్పు - width2);
    అయితే (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_నిలువుల_సంఖ్య)
    {
	// scroll a screen line down
	ఒకవేళ (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	లేకపోతే
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    ఒకవేళ (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	తిరిగి;  // don't scroll in the other direction now
    }

    నిలువు = curwin->w_virtcol + scrolloff_నిలువుల_సంఖ్య;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    ఒకవేళ (scrolloff_నిలువుల_సంఖ్య > 0)
    {
	పూర్ణాంకం కొలత = linetabsize_eol(curwin, curwin->w_topline);
	కొలత = width1 + width2 * ((కొలత - width1 + width2 - 1) / width2);
	అయితే (నిలువు > కొలత)
	    నిలువు -= width2;
    }
    నిలువు -= curwin->w_skipcol;

    ఒకవేళ (నిలువు >= width1)
    {
	నిలువు -= width1;
	++అడ్డువరుస;
    }
    ఒకవేళ (నిలువు > width2)
    {
	అడ్డువరుస += నిలువు / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	నిలువు = నిలువు % width2;
    }
    ఒకవేళ (అడ్డువరుస >= curwin->w_ఎత్తు)
    {
	ఒకవేళ (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --అడ్డువరుస;
	}
	ఒకవేళ (అడ్డువరుస >= curwin->w_ఎత్తు)
	    curwin->w_skipcol += (అడ్డువరుస - curwin->w_ఎత్తు) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#నిర్వచితమైతే FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    శూన్య
check_topfill(
    win_T	*wp,
    పూర్ణాంకం		కిందికి)	// when TRUE scroll down when not enough space
{
    పూర్ణాంకం		n;

    ఒకవేళ (wp->w_topfill <= 0)
	తిరిగి;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    ఒకవేళ (wp->w_topfill + n > wp->w_ఎత్తు)
    {
	ఒకవేళ (కిందికి && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	లేకపోతే
	{
	    wp->w_topfill = wp->w_ఎత్తు - n;
	    ఒకవేళ (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#అయితే_ముగింపు

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    శూన్య
scrolldown_clamp(శూన్య)
{
    పూర్ణాంకం		చివర_అడ్డువరుస;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#అయితే_ముగింపు

    ఒకవేళ (curwin->w_topline <= 1
#నిర్వచితమైతే FEAT_DIFF
	    && !can_fill
#అయితే_ముగింపు
	    )
	తిరిగి;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    చివర_అడ్డువరుస = curwin->w_wrow;
#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (can_fill)
	++చివర_అడ్డువరుస;
    లేకపోతే
	చివర_అడ్డువరుస += plines_nofill(curwin->w_topline - 1);
#లేకపోతే
    చివర_అడ్డువరుస += plines(curwin->w_topline - 1);
#అయితే_ముగింపు
    ఒకవేళ (curwin->w_p_wrap && curwin->w_వెడల్పు != 0)
    {
	validate_cheight();
	validate_virtcol();
	చివర_అడ్డువరుస += curwin->w_cline_ఎత్తు - 1 -
	    curwin->w_virtcol / curwin->w_వెడల్పు;
    }
    ఒకవేళ (చివర_అడ్డువరుస < curwin->w_ఎత్తు - get_scrolloff_విలువ())
    {
#నిర్వచితమైతే FEAT_DIFF
	ఒకవేళ (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	లేకపోతే
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#లేకపోతే
	--curwin->w_topline;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
	(శూన్య)hasFolding(curwin->w_topline, &curwin->w_topline, ఖాళీ);
#అయితే_ముగింపు
	--curwin->w_botline;	    // approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    శూన్య
scrollup_clamp(శూన్య)
{
    పూర్ణాంకం	    ప్రారంభం_అడ్డువరుస;

    ఒకవేళ (curwin->w_topline == curbuf->b_ml.ml_పంక్తి_లెక్క
#నిర్వచితమైతే FEAT_DIFF
	    && curwin->w_topfill == 0
#అయితే_ముగింపు
	    )
	తిరిగి;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#నిర్వచితమైతే FEAT_DIFF
    ప్రారంభం_అడ్డువరుస = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#లేకపోతే
    ప్రారంభం_అడ్డువరుస = curwin->w_wrow - plines(curwin->w_topline);
#అయితే_ముగింపు
    ఒకవేళ (curwin->w_p_wrap && curwin->w_వెడల్పు != 0)
    {
	validate_virtcol();
	ప్రారంభం_అడ్డువరుస -= curwin->w_virtcol / curwin->w_వెడల్పు;
    }
    ఒకవేళ (ప్రారంభం_అడ్డువరుస >= get_scrolloff_విలువ())
    {
#నిర్వచితమైతే FEAT_DIFF
	ఒకవేళ (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	లేకపోతే
#అయితే_ముగింపు
	{
#నిర్వచితమైతే FEAT_FOLDING
	    (శూన్య)hasFolding(curwin->w_topline, ఖాళీ, &curwin->w_topline);
#అయితే_ముగింపు
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
    స్థిర శూన్య
topline_వెనుక_winheight(
    lineoff_T	*lp,
    పూర్ణాంకం		winheight)	// when TRUE limit to window height
{
#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->ఎత్తు = 1;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	--lp->lnum;
#నిర్వచితమైతే FEAT_DIFF
	lp->fill = 0;
#అయితే_ముగింపు
	ఒకవేళ (lp->lnum < 1)
	    lp->ఎత్తు = MAXCOL;
	లేకపోతే
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (hasFolding(lp->lnum, &lp->lnum, ఖాళీ))
	    // Add a closed fold
	    lp->ఎత్తు = 1;
	లేకపోతే
#అయితే_ముగింపు
	    lp->ఎత్తు = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    స్థిర శూన్య
topline_వెనుక(lineoff_T *lp)
{
    topline_వెనుక_winheight(lp, TRUE);
}


/*
 * Add one line below "lp->lnum".  This can be a filler line, a closed fold or
 * a (wrapped) text line.  Uses and sets "lp->fill".
 * Returns the height of the added line in "lp->height".
 * Lines below the last one are incredibly high.
 */
    స్థిర శూన్య
botline_forw(lineoff_T *lp)
{
#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->ఎత్తు = 1;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	++lp->lnum;
#నిర్వచితమైతే FEAT_DIFF
	lp->fill = 0;
#అయితే_ముగింపు
	ఒకవేళ (lp->lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    lp->ఎత్తు = MAXCOL;
	లేకపోతే
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (hasFolding(lp->lnum, ఖాళీ, &lp->lnum))
	    {
		// Add a closed fold.
		lp->ఎత్తు = 1;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    lp->ఎత్తు = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    శూన్య
scroll_cursor_శిఖరం(పూర్ణాంకం min_scroll, పూర్ణాంకం always)
{
    పూర్ణాంకం		scrolled = 0;
    పూర్ణాంకం		extra = 0;
    పూర్ణాంకం		వాడిన;
    పూర్ణాంకం		i;
    linenr_T	శిఖరం;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    పూర్ణాంకం		old_skipcol = curwin->w_skipcol;
#నిర్వచితమైతే FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#అయితే_ముగింపు
    linenr_T	new_topline;
    పూర్ణాంకం		off = get_scrolloff_విలువ();

    ఒకవేళ (mouse_dragging > 0)
	off = mouse_dragging - 1;

    /*
     * Decrease topline until:
     * - it has become 1
     * - (part of) the cursor line is moved off the screen or
     * - moved at least 'scrolljump' lines and
     * - at least 'scrolloff' lines above and below the cursor
     */
    validate_cheight();
    వాడిన = curwin->w_cline_ఎత్తు; // includes filler lines above
    ఒకవేళ (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = వాడిన;

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (hasFolding(curwin->w_cursor.lnum, &శిఖరం, &bot))
    {
	--శిఖరం;
	++bot;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	శిఖరం = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = శిఖరం + 1;

#నిర్వచితమైతే FEAT_DIFF
    // "used" already contains the number of filler lines above, don't add it
    // again.
    // Hide filler lines above cursor line by adding them to "extra".
    extra += diff_check_fill(curwin, curwin->w_cursor.lnum);
#అయితే_ముగింపు

    /*
     * Check if the lines from "top" to "bot" fit in the window.  If they do,
     * set new_topline and advance "top" and "bot" to include more lines.
     */
    అయితే (శిఖరం > 0)
    {
#నిర్వచితమైతే FEAT_FOLDING
	ఒకవేళ (hasFolding(శిఖరం, &శిఖరం, ఖాళీ))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	లేకపోతే
#అయితే_ముగింపు
	    i = PLINES_NOFILL(శిఖరం);
	ఒకవేళ (శిఖరం < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	ఒకవేళ ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    ఆపు;

	వాడిన += i;
	ఒకవేళ (extra + i <= off && bot < curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (hasFolding(bot, ఖాళీ, &bot))
		// count one logical line for a sequence of folded lines
		++వాడిన;
	    లేకపోతే
#అయితే_ముగింపు
		వాడిన += plines(bot);
	}
	ఒకవేళ (వాడిన > curwin->w_ఎత్తు)
	    ఆపు;

	extra += i;
	new_topline = శిఖరం;
	--శిఖరం;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    ఒకవేళ (వాడిన > curwin->w_ఎత్తు)
	scroll_cursor_halfway(FALSE, FALSE);
    లేకపోతే
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	ఒకవేళ (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	ఒకవేళ (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#నిర్వచితమైతే FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	ఒకవేళ (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    ఒకవేళ (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#అయితే_ముగింపు
	ఒకవేళ (curwin->w_topline != old_topline)
	    reset_skipcol();
	లేకపోతే ఒకవేళ (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    ఒకవేళ (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		పూర్ణాంకం	width1 = curwin->w_వెడల్పు - curwin_నిలువు_off();
		పూర్ణాంకం	width2 = width1 + curwin_నిలువు_off2();
		పూర్ణాంకం	plines_off = 0;
		పూర్ణాంకం	skipcol;

		ఒకవేళ (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		ఒకవేళ (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	ఒకవేళ (curwin->w_topline != old_topline
		|| curwin->w_skipcol != old_skipcol
#నిర్వచితమైతే FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#అయితే_ముగింపు
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
    శూన్య
సమితి_ఖాళీలు_అడ్డువరుసలు(win_T *wp, పూర్ణాంకం వాడిన)
{
#నిర్వచితమైతే FEAT_DIFF
    wp->w_filler_అడ్డువరుసలు = 0;
#అయితే_ముగింపు
    ఒకవేళ (వాడిన == 0)
	wp->w_ఖాళీలు_అడ్డువరుసలు = 0;	// single line that doesn't fit
    లేకపోతే
    {
	wp->w_ఖాళీలు_అడ్డువరుసలు = wp->w_ఎత్తు - వాడిన;
#నిర్వచితమైతే FEAT_DIFF
	ఒకవేళ (wp->w_botline <= wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	{
	    wp->w_filler_అడ్డువరుసలు = diff_check_fill(wp, wp->w_botline);
	    ఒకవేళ (wp->w_ఖాళీలు_అడ్డువరుసలు > wp->w_filler_అడ్డువరుసలు)
		wp->w_ఖాళీలు_అడ్డువరుసలు -= wp->w_filler_అడ్డువరుసలు;
	    లేకపోతే
	    {
		wp->w_filler_అడ్డువరుసలు = wp->w_ఖాళీలు_అడ్డువరుసలు;
		wp->w_ఖాళీలు_అడ్డువరుసలు = 0;
	    }
	}
#అయితే_ముగింపు
    }
}

/*
 * Recompute topline to put the cursor at the bottom of the window.
 * When scrolling scroll at least "min_scroll" lines.
 * If "set_topbot" is TRUE, set topline and botline first (for "zb").
 * This is messy stuff!!!
 */
    శూన్య
scroll_cursor_bot(పూర్ణాంకం min_scroll, పూర్ణాంకం సమితి_topbot)
{
    పూర్ణాంకం		వాడిన;
    పూర్ణాంకం		scrolled = 0;
    పూర్ణాంకం		extra = 0;
    పూర్ణాంకం		i;
    linenr_T	పంక్తి_లెక్క;
    linenr_T	old_topline = curwin->w_topline;
    పూర్ణాంకం		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		old_topfill = curwin->w_topfill;
    పూర్ణాంకం		fill_క్రింద_కిటికీ;
#అయితే_ముగింపు
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    పూర్ణాంకం		old_ఖాళీలు_అడ్డువరుసలు = curwin->w_ఖాళీలు_అడ్డువరుసలు;
    linenr_T	cln;		    // Cursor Line Number
    దీర్ఘ	so = get_scrolloff_విలువ();
    పూర్ణాంకం		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    ఒకవేళ (సమితి_topbot)
    {
	వాడిన = 0;
	linenr_T cln_చివరి = cln;
#నిర్వచితమైతే FEAT_FOLDING
	(శూన్య)hasFolding(cln, ఖాళీ, &cln_చివరి);
#అయితే_ముగింపు
	curwin->w_botline = cln_చివరి + 1;
	loff.lnum = cln_చివరి + 1;
#నిర్వచితమైతే FEAT_DIFF
	loff.fill = 0;
#అయితే_ముగింపు
	అయితే (TRUE)
	{
	    topline_వెనుక_winheight(&loff, FALSE);
	    ఒకవేళ (loff.ఎత్తు == MAXCOL)
		ఆపు;
	    ఒకవేళ (వాడిన + loff.ఎత్తు > curwin->w_ఎత్తు)
	    {
		ఒకవేళ (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    ఒకవేళ (వాడిన < curwin->w_ఎత్తు)
		    {
			పూర్ణాంకం plines_స్థానభ్రంశం = వాడిన + loff.ఎత్తు
							    - curwin->w_ఎత్తు;
			వాడిన = curwin->w_ఎత్తు;
#నిర్వచితమైతే FEAT_DIFF
			curwin->w_topfill = loff.fill;
#అయితే_ముగింపు
			curwin->w_topline = loff.lnum;
			curwin->w_skipcol = skipcol_from_plines(
							curwin, plines_స్థానభ్రంశం);
		    }
		}
		ఆపు;
	    }
#నిర్వచితమైతే FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#అయితే_ముగింపు
	    curwin->w_topline = loff.lnum;
	    వాడిన += loff.ఎత్తు;
	}

	సమితి_ఖాళీలు_అడ్డువరుసలు(curwin, వాడిన);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	ఒకవేళ (curwin->w_topline != old_topline
#నిర్వచితమైతే FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#అయితే_ముగింపు
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    ఒకవేళ (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    లేకపోతే
		reset_skipcol();
	}
    }
    లేకపోతే
	validate_botline();

    // The lines of the cursor line itself are always used.
#నిర్వచితమైతే FEAT_DIFF
    వాడిన = plines_nofill(cln);
#లేకపోతే
    validate_cheight();
    వాడిన = curwin->w_cline_ఎత్తు;
#అయితే_ముగింపు

    ఒకవేళ (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	పూర్ణాంకం upto_cursor = plines_win_నిలువు(curwin, cln, curwin->w_cursor.నిలువు);

	ఒకవేళ (upto_cursor < వాడిన)
	    వాడిన = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    ఒకవేళ (cln >= curwin->w_botline)
    {
	scrolled = వాడిన;
	ఒకవేళ (cln == curwin->w_botline)
	    scrolled -= curwin->w_ఖాళీలు_అడ్డువరుసలు;
	ఒకవేళ (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    పూర్ణాంకం శిఖరం_plines =
#నిర్వచితమైతే FEAT_DIFF
			    plines_win_nofill
#లేకపోతే
			    plines_win
#అయితే_ముగింపు
					(curwin, curwin->w_topline, FALSE);
	    పూర్ణాంకం width1 = curwin->w_వెడల్పు - curwin_నిలువు_off();

	    ఒకవేళ (width1 > 0)
	    {
		పూర్ణాంకం width2 = width1 + curwin_నిలువు_off2();
		పూర్ణాంకం skip_పంక్తులు = 0;

		// A similar formula is used in curs_columns().
		ఒకవేళ (curwin->w_skipcol > width1)
		    skip_పంక్తులు += (curwin->w_skipcol - width1) / width2 + 1;
		లేకపోతే ఒకవేళ (curwin->w_skipcol > 0)
		    skip_పంక్తులు = 1;

		శిఖరం_plines -= skip_పంక్తులు;
		ఒకవేళ (శిఖరం_plines > curwin->w_ఎత్తు)
		{
		    scrolled += (శిఖరం_plines - curwin->w_ఎత్తు);
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
#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
#అయితే_ముగింపు
    {
	loff.lnum = cln;
	boff.lnum = cln;
    }
#నిర్వచితమైతే FEAT_DIFF
    loff.fill = 0;
    boff.fill = 0;
    fill_క్రింద_కిటికీ = diff_check_fill(curwin, curwin->w_botline)
						      - curwin->w_filler_అడ్డువరుసలు;
#అయితే_ముగింపు

    అయితే (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	ఒకవేళ ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_పంక్తి_లెక్క)
		&& loff.lnum <= curwin->w_botline
#నిర్వచితమైతే FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_క్రింద_కిటికీ)
#అయితే_ముగింపు
		)
	    ఆపు;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_వెనుక(&loff);
	ఒకవేళ (loff.ఎత్తు == MAXCOL)
	    వాడిన = MAXCOL;
	లేకపోతే
	    వాడిన += loff.ఎత్తు;
	ఒకవేళ (వాడిన > curwin->w_ఎత్తు)
	    ఆపు;
	ఒకవేళ (loff.lnum >= curwin->w_botline
#నిర్వచితమైతే FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_క్రింద_కిటికీ)
#అయితే_ముగింపు
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.ఎత్తు;
	    ఒకవేళ (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_ఖాళీలు_అడ్డువరుసలు;
	}

	ఒకవేళ (boff.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    వాడిన += boff.ఎత్తు;
	    ఒకవేళ (వాడిన > curwin->w_ఎత్తు)
		ఆపు;
	    ఒకవేళ (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.ఎత్తు;
		ఒకవేళ (boff.lnum >= curwin->w_botline
#నిర్వచితమైతే FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_అడ్డువరుసలు)
#అయితే_ముగింపు
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.ఎత్తు;
		    ఒకవేళ (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_ఖాళీలు_అడ్డువరుసలు;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    ఒకవేళ (scrolled <= 0)
	పంక్తి_లెక్క = 0;
    // more than a screenfull, don't scroll but redraw
    లేకపోతే ఒకవేళ (వాడిన > curwin->w_ఎత్తు)
	పంక్తి_లెక్క = వాడిన;
    // scroll minimal number of lines
    లేకపోతే
    {
	పంక్తి_లెక్క = 0;
#నిర్వచితమైతే FEAT_DIFF
	boff.fill = curwin->w_topfill;
#అయితే_ముగింపు
	boff.lnum = curwin->w_topline - 1;
	కొరకు (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.ఎత్తు;
	    ++పంక్తి_లెక్క;
	}
	ఒకవేళ (i < scrolled)	// below curwin->w_botline, don't scroll
	    పంక్తి_లెక్క = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    ఒకవేళ (పంక్తి_లెక్క >= curwin->w_ఎత్తు && పంక్తి_లెక్క > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    లేకపోతే ఒకవేళ (పంక్తి_లెక్క > 0)
    {
	ఒకవేళ (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	లేకపోతే
	    scrollup(పంక్తి_లెక్క, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    ఒకవేళ (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && సమితి_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_ఖాళీలు_అడ్డువరుసలు = old_ఖాళీలు_అడ్డువరుసలు;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    ఒకవేళ (సమితి_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    శూన్య
scroll_cursor_halfway(పూర్ణాంకం atend, పూర్ణాంకం prefer_above)
{
    పూర్ణాంకం		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		topfill = 0;
#అయితే_ముగింపు
    పూర్ణాంకం		క్రింద = 0;
    పూర్ణాంకం		వాడిన;
    lineoff_T	loff;
    lineoff_T	boff;
#నిర్వచితమైతే FEAT_DIFF
    linenr_T	old_topline = curwin->w_topline;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
    // if the width changed this needs to be updated first
    may_update_popup_position();
#అయితే_ముగింపు
    loff.lnum = boff.lnum = curwin->w_cursor.lnum;
#నిర్వచితమైతే FEAT_FOLDING
    (శూన్య)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
    వాడిన = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#లేకపోతే
    వాడిన = plines(loff.lnum);
#అయితే_ముగింపు
    topline = loff.lnum;

    పూర్ణాంకం want_ఎత్తు;
    పూర్ణాంకం do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ఒకవేళ (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	ఒకవేళ (atend)
	{
	    want_ఎత్తు = (curwin->w_ఎత్తు - వాడిన) / 2;
	    వాడిన = 0;
	}
	లేకపోతే
	    want_ఎత్తు = curwin->w_ఎత్తు;
    }

    అయితే (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	ఒకవేళ (do_sms)
	{
	    topline_వెనుక_winheight(&loff, FALSE);
	    ఒకవేళ (loff.ఎత్తు == MAXCOL)
		ఆపు;
	    వాడిన += loff.ఎత్తు;
	    ఒకవేళ (!atend && boff.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
	    {
		botline_forw(&boff);
		వాడిన += boff.ఎత్తు;
	    }
	    ఒకవేళ (వాడిన > want_ఎత్తు)
	    {
		ఒకవేళ (వాడిన - loff.ఎత్తు < want_ఎత్తు)
		{
		    topline = loff.lnum;
#నిర్వచితమైతే FEAT_DIFF
		    topfill = loff.fill;
#అయితే_ముగింపు
		    skipcol = skipcol_from_plines(curwin, వాడిన - want_ఎత్తు);
		}
		ఆపు;
	    }
	    topline = loff.lnum;
#నిర్వచితమైతే FEAT_DIFF
	    topfill = loff.fill;
#అయితే_ముగింపు
	    కొనసాగు;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	పూర్ణాంకం పూర్తి = FALSE;
	కొరకు (పూర్ణాంకం విడత = 1; విడత <= 2; ++విడత)
	{
	    ఒకవేళ (prefer_above ? (విడత == 2 && క్రింద < above)
			     : (విడత == 1 && క్రింద <= above))
	    {
		// add a line below the cursor
		ఒకవేళ (boff.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
		{
		    botline_forw(&boff);
		    వాడిన += boff.ఎత్తు;
		    ఒకవేళ (వాడిన > curwin->w_ఎత్తు)
		    {
			పూర్తి = TRUE;
			ఆపు;
		    }
		    క్రింద += boff.ఎత్తు;
		}
		లేకపోతే
		{
		    ++క్రింద;	    // count a "~" line
		    ఒకవేళ (atend)
			++వాడిన;
		}
	    }

	    ఒకవేళ (prefer_above ? (విడత == 1 && క్రింద >= above)
			     : (విడత == 1 && క్రింద > above))
	    {
		// add a line above the cursor
		topline_వెనుక(&loff);
		ఒకవేళ (loff.ఎత్తు == MAXCOL)
		    వాడిన = MAXCOL;
		లేకపోతే
		    వాడిన += loff.ఎత్తు;
		ఒకవేళ (వాడిన > curwin->w_ఎత్తు)
		{
		    పూర్తి = TRUE;
		    ఆపు;
		}
		above += loff.ఎత్తు;
		topline = loff.lnum;
#నిర్వచితమైతే FEAT_DIFF
		topfill = loff.fill;
#అయితే_ముగింపు
	    }
	}
	ఒకవేళ (పూర్తి)
	    ఆపు;
    }

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (!hasFolding(topline, &curwin->w_topline, ఖాళీ))
#అయితే_ముగింపు
    {
	ఒకవేళ (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    ఒకవేళ (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    లేకపోతే ఒకవేళ (do_sms)
		reset_skipcol();
	}
    }
#నిర్వచితమైతే FEAT_DIFF
    curwin->w_topfill = topfill;
    ఒకవేళ (old_topline > curwin->w_topline + curwin->w_ఎత్తు)
	curwin->w_botfill = false;
    check_topfill(curwin, FALSE);
#అయితే_ముగింపు
    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
    curwin->w_valid |= VALID_TOPLINE;
}

/*
 * Correct the cursor position so that it is in a part of the screen at least
 * 'scrolloff' lines from the top and bottom, if possible.
 * If not possible, put it at the same position as scroll_cursor_halfway().
 * When called topline must be valid!
 */
    శూన్య
cursor_correct(శూన్య)
{
    పూర్ణాంకం		above = 0;	    // screen lines above topline
    linenr_T	topline;
    పూర్ణాంకం		క్రింద = 0;	    // screen lines below botline
    linenr_T	botline;
    పూర్ణాంకం		above_wanted, క్రింద_wanted;
    linenr_T	cln;		    // Cursor Line Number
    పూర్ణాంకం		max_off;
    దీర్ఘ	so = get_scrolloff_విలువ();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    క్రింద_wanted = so;
    ఒకవేళ (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	క్రింద_wanted = mouse_dragging - 1;
    }
    ఒకవేళ (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_ఎత్తు / 2;
	ఒకవేళ (క్రింద_wanted > max_off)
	    క్రింద_wanted = max_off;
    }
    validate_botline();
    ఒకవేళ (curwin->w_botline == curbuf->b_ml.ml_పంక్తి_లెక్క + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	క్రింద_wanted = 0;
	max_off = (curwin->w_ఎత్తు - 1) / 2;
	ఒకవేళ (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    ఒకవేళ (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - క్రింద_wanted
#నిర్వచితమైతే FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#అయితే_ముగింపు
	    )
	తిరిగి;

    /*
     * Narrow down the area where the cursor can be put by taking lines from
     * the top and the bottom until:
     * - the desired context lines are found
     * - the lines from the top is past the lines from the bottom
     */
    topline = curwin->w_topline;
    botline = curwin->w_botline - 1;
#నిర్వచితమైతే FEAT_DIFF
    // count filler lines as context
    above = curwin->w_topfill;
    క్రింద = curwin->w_filler_అడ్డువరుసలు;
#అయితే_ముగింపు
    అయితే ((above < above_wanted || క్రింద < క్రింద_wanted) && topline < botline)
    {
	ఒకవేళ (క్రింద < క్రింద_wanted && (క్రింద <= above || above >= above_wanted))
	{
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (hasFolding(botline, &botline, ఖాళీ))
		++క్రింద;
	    లేకపోతే
#అయితే_ముగింపు
		క్రింద += plines(botline);
	    --botline;
	}
	ఒకవేళ (above < above_wanted && (above < క్రింద || క్రింద >= క్రింద_wanted))
	{
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (hasFolding(topline, ఖాళీ, &topline))
		++above;
	    లేకపోతే
#అయితే_ముగింపు
		above += PLINES_NOFILL(topline);
#నిర్వచితమైతే FEAT_DIFF
	    // Count filler lines below this line as context.
	    ఒకవేళ (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#అయితే_ముగింపు
	    ++topline;
	}
    }
    ఒకవేళ (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    లేకపోతే ఒకవేళ (topline > botline)
	curwin->w_cursor.lnum = botline;
    లేకపోతే
    {
	ఒకవేళ (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	ఒకవేళ (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_పంక్తి_లెక్క)
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
స్థిర పూర్ణాంకం get_scroll_overlap(పూర్ణాంకం dir)
{
    lineoff_T loff;
    పూర్ణాంకం	    min_ఎత్తు = curwin->w_ఎత్తు - 2;

    validate_botline();
    ఒకవేళ ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_పంక్తి_లెక్క))
	తిరిగి min_ఎత్తు + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#నిర్వచితమైతే FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_అడ్డువరుసలు : curwin->w_topfill);
    loff.ఎత్తు = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#లేకపోతే
    loff.ఎత్తు = plines(loff.lnum);
#అయితే_ముగింపు

    పూర్ణాంకం h1 = loff.ఎత్తు;
    ఒకవేళ (h1 > min_ఎత్తు)
	తిరిగి min_ఎత్తు + 2;  // no overlap
    ఒకవేళ (dir == FORWARD)
	topline_వెనుక(&loff);
    లేకపోతే
	botline_forw(&loff);

    పూర్ణాంకం h2 = loff.ఎత్తు;
    ఒకవేళ (h2 == MAXCOL || h2 + h1 > min_ఎత్తు)
	తిరిగి min_ఎత్తు + 2;  // no overlap
    ఒకవేళ (dir == FORWARD)
	topline_వెనుక(&loff);
    లేకపోతే
	botline_forw(&loff);

    పూర్ణాంకం h3 = loff.ఎత్తు;
    ఒకవేళ (h3 == MAXCOL || h3 + h2 > min_ఎత్తు)
	తిరిగి min_ఎత్తు + 2;  // no overlap
    ఒకవేళ (dir == FORWARD)
	topline_వెనుక(&loff);
    లేకపోతే
	botline_forw(&loff);

    పూర్ణాంకం h4 = loff.ఎత్తు;
    ఒకవేళ (h4 == MAXCOL || h4 + h3 + h2 > min_ఎత్తు || h3 + h2 + h1 > min_ఎత్తు)
	తిరిగి min_ఎత్తు + 1;  // 1 line overlap
    లేకపోతే
	తిరిగి min_ఎత్తు;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
స్థిర పూర్ణాంకం scroll_with_sms(పూర్ణాంకం dir, దీర్ఘ లెక్క, దీర్ఘ *curscount)
{
    పూర్ణాంకం		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		prev_topfill = curwin->w_topfill;
#అయితే_ముగింపు

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, లెక్క);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    ఒకవేళ (!prev_sms && curwin->w_skipcol > 0)
    {
	పూర్ణాంకం fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	ఒకవేళ (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	పూర్ణాంకం width1 = curwin->w_వెడల్పు - curwin_నిలువు_off();
	పూర్ణాంకం width2 = width1 + curwin_నిలువు_off2();
	లెక్క = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	ఒకవేళ (fixdir == FORWARD)
	    లెక్క = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, లెక్క);
	*curscount += లెక్క * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    తిరిగి curwin->w_topline != prev_topline
#నిర్వచితమైతే FEAT_DIFF
	|| curwin->w_topfill != prev_topfill
#అయితే_ముగింపు
	|| curwin->w_skipcol != prev_skipcol;
}

/*
 * Move screen "count" (half) pages up ("dir" is BACKWARD) or down ("dir" is
 * FORWARD) and update the screen. Handle moving the cursor and not scrolling
 * to reveal end of buffer lines for half-page scrolling with CTRL-D and CTRL-U.
 *
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
pagescroll(పూర్ణాంకం dir, దీర్ఘ లెక్క, పూర్ణాంకం half)
{
    పూర్ణాంకం		did_move = FALSE;
    పూర్ణాంకం		buflen = curbuf->b_ml.ml_పంక్తి_లెక్క;
    colnr_T	prev_నిలువు = curwin->w_cursor.నిలువు;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    ఒకవేళ (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	ఒకవేళ (లెక్క)
	    curwin->w_p_scr = MIN(curwin->w_ఎత్తు, లెక్క);
	లెక్క = MIN(curwin->w_ఎత్తు, curwin->w_p_scr);

	దీర్ఘ curscount = లెక్క;
	// Adjust count so as to not reveal end of buffer lines.
	ఒకవేళ (dir == FORWARD
		    && (curwin->w_topline + curwin->w_ఎత్తు + లెక్క > buflen
#నిర్వచితమైతే FEAT_FOLDING
							|| hasAnyFolding(curwin)
#అయితే_ముగింపు
	   ))
	{
	    పూర్ణాంకం n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    ఒకవేళ (n - లెక్క < curwin->w_ఎత్తు && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_ఎత్తు + లెక్క);
	    ఒకవేళ (n < curwin->w_ఎత్తు + లెక్క)
		లెక్క = n - curwin->w_ఎత్తు;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	ఒకవేళ (లెక్క > 0)
	{
	    did_move = scroll_with_sms(dir, లెక్క, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.నిలువు = prev_నిలువు;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	ఒకవేళ (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	లేకపోతే ఒకవేళ (dir == FORWARD)
	    cursor_కిందికి_inner(curwin, curscount);
	లేకపోతే
	    cursor_పైకి_inner(curwin, curscount);
    }
    లేకపోతే
    {
	// Scroll [count] times 'window' or current window height lines.
	లెక్క *= ((ONE_కిటికీ && p_కిటికీ > 0 && p_కిటికీ < అడ్డువరుసలు - 1) ?
				MAX(1, p_కిటికీ - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, లెక్క, &లెక్క);

	ఒకవేళ (did_move)
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

    ఒకవేళ (get_scrolloff_విలువ() > 0)
	cursor_correct();
#నిర్వచితమైతే FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#అయితే_ముగింపు
    did_move = did_move
	|| prev_నిలువు != curwin->w_cursor.నిలువు
	|| prev_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    ఒకవేళ (!did_move)
	beep_flush();
    లేకపోతే ఒకవేళ (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    లేకపోతే ఒకవేళ (p_sol)
	nv_g_home_m_cmd(&ca);

    తిరిగి did_move ? OK : FAIL;
}

    శూన్య
do_check_cursorbind(శూన్య)
{
    స్థిర win_T	*prev_curwin = ఖాళీ;
    స్థిర buf_T	*prev_curbuf = ఖాళీ;
    స్థిర varnumber_T	prev_changedtick = 0;
    స్థిర pos_T	prev_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    ఒకవేళ (curwin == prev_curwin && curbuf == prev_curbuf
	    && CHANGEDTICK(curbuf) == prev_changedtick
	    && EQUAL_POS(curwin->w_cursor, prev_cursor))
	తిరిగి;
    prev_curwin = curwin;
    prev_curbuf = curbuf;
    prev_changedtick = CHANGEDTICK(curbuf);
    prev_cursor = curwin->w_cursor;

    linenr_T	పంక్తి = curwin->w_cursor.lnum;
    colnr_T	నిలువు = curwin->w_cursor.నిలువు;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    పూర్ణాంకం		సమితి_curswant = curwin->w_సమితి_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    పూర్ణాంకం		old_VIsual_select = VIsual_select;
    పూర్ణాంకం		old_VIsual_చురుకైన = VIsual_చురుకైన;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_చురుకైన = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_బఫరు;
	// skip original window and windows with 'nocursorbind'
	ఒకవేళ (curwin != old_curwin && curwin->w_p_crb)
	{
#నిర్వచితమైతే FEAT_DIFF
	    ఒకవేళ (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_పంక్తి(old_curbuf, పంక్తి);
	    లేకపోతే
#అయితే_ముగింపు
		curwin->w_cursor.lnum = పంక్తి;
	    curwin->w_cursor.నిలువు = నిలువు;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_సమితి_curswant = సమితి_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    పూర్ణాంకం restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    ఒకవేళ (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    ఒకవేళ (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    ఒకవేళ (!curwin->w_p_scb)
		update_topline();
	    curwin->w_redr_స్థితి = true;
	}
    }

    /*
     * reset current-window
     */
    VIsual_select = old_VIsual_select;
    VIsual_చురుకైన = old_VIsual_చురుకైన;
    curwin = old_curwin;
    curbuf = old_curbuf;
}
