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

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಶೂನ್ಯ redraw_for_cursorline(win_T *wp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ scrolljump_ಮೌಲ್ಯ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_ತುದಿ_offset(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ curs_rows(win_T *wp);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    linenr_T	    lnum;	// line number
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		    fill;	// filler lines
#endif
    ಪೂರ್ಣಾಙ್ಕ		    height;	// height of added line
} lineoff_T;

ಸ್ಥಿರ ಶೂನ್ಯ topline_back(lineoff_T *lp);
ಸ್ಥಿರ ಶೂನ್ಯ botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
adjust_plines_for_skipcol(win_T *wp)
{
    ಯದಿ (wp->w_skipcol == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪೂರ್ಣಾಙ್ಕ width = wp->w_width - win_col_off(wp);
    ಪೂರ್ಣಾಙ್ಕ w2 = width + win_col_off2(wp);
    ಯದಿ (wp->w_skipcol >= width && w2 > 0)
	ಪ್ರತ್ಯಾಗಮ (wp->w_skipcol - width) / w2 + 1;

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    ಪೂರ್ಣಾಙ್ಕ
plines_correct_topline(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ limit_winheight)
{
    ಪೂರ್ಣಾಙ್ಕ n;
#ifdef FEAT_DIFF
    ಯದಿ (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    ಅನ್ಯಥಾ
#endif
	n = plines_win(wp, lnum, FALSE);
    ಯದಿ (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    ಯದಿ (limit_winheight && n > wp->w_height)
	n = wp->w_height;
    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
comp_botline(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		done;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;
    ಪೂರ್ಣಾಙ್ಕ		use_cache;
#ifdef FEAT_FOLDING
    linenr_T    last;
    ಪೂರ್ಣಾಙ್ಕ		folded;
#endif

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
		    && !wp->w_buffer->b_mod_set
		    && dollar_vcol == -1
		    && wp->w_skipcol == 0
#ifdef FEAT_DIFF
		    && !wp->w_p_diff
#endif
		    && wp->w_ಸಾಲುಗಳು_valid > 0
		    && wp->w_ಸಾಲುಗಳು[0].wl_lnum <= wp->w_topline;

    ಯದಿ (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	done = wp->w_cline_row;
    }
    ಅನ್ಯಥಾ
    {
	lnum = wp->w_topline;
	done = 0;
    }

    // Find the w_lines[] entry for the starting line.
    ಯದಿ (use_cache)
	ಯಾವತ್ (i < wp->w_ಸಾಲುಗಳು_valid && wp->w_ಸಾಲುಗಳು[i].wl_lnum < lnum)
	    ++i;

    ಪ್ರತಿ ( ; lnum <= wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ	valid = FALSE;

#ifdef FEAT_FOLDING
	last = lnum;
	folded = FALSE;
#endif
	// Try to use the size from the previous redraw.
	ಯದಿ (use_cache && i < wp->w_ಸಾಲುಗಳು_valid)
	{
	    ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_lnum < lnum || !wp->w_ಸಾಲುಗಳು[i].wl_valid)
		ಅನುವರ್ತ;		// skip changed or deleted lines
	    ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_lnum == lnum)
		valid = TRUE;
	    ಅನ್ಯಥಾ // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	ಯದಿ (valid)
	{
	    n = wp->w_ಸಾಲುಗಳು[i].wl_size;
#ifdef FEAT_FOLDING
	    folded = wp->w_ಸಾಲುಗಳು[i].wl_folded;
	    last = wp->w_ಸಾಲುಗಳು[i].wl_lastlnum;
#endif
	}
	ಅನ್ಯಥಾ
	{
#ifdef FEAT_FOLDING
	    ಯದಿ (hasFoldingWin(wp, lnum, ರಿಕ್ತ, &last, TRUE, ರಿಕ್ತ))
	    {
		n = 1;
		folded = TRUE;
	    }
	    ಅನ್ಯಥಾ
#endif
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	ಯದಿ (
#ifdef FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && last >= wp->w_cursor.lnum
#ಅನ್ಯಥಾ
		lnum == wp->w_cursor.lnum
#endif
	   )
	{
	    wp->w_cline_row = done;
	    wp->w_cline_height = n;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = folded;
#endif
	    redraw_for_cursorline(wp);
	    wp->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	ಯದಿ (done + n > wp->w_height)
	    ಭಙ್ಗ;
	done += n;
#ifdef FEAT_FOLDING
	lnum = last + 1;
#ಅನ್ಯಥಾ
	++lnum;
#endif
    }

    // wp->w_botline is the line that is just below the window
    wp->w_botline = lnum;
    wp->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    set_empty_rows(wp, done);
}

/*
 * Redraw when w_cline_row changes and 'relativenumber' or 'cursorline' is
 * set.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_for_cursorline(win_T *wp)
{
    ಯದಿ ((wp->w_p_rnu
#ifdef FEAT_SYN_HL
		|| wp->w_p_cul
#endif
		)
	    && (wp->w_valid & VALID_CROW) == 0
	    && !pum_visible())
    {
	// win_line() will redraw the number column and cursorline only.
	redraw_win_later(wp, UPD_VALID);
    }
}

#ifdef FEAT_SYN_HL
/*
 * Redraw when w_virtcol changes and 'cursorcolumn' is set or 'cursorlineopt'
 * contains "screenline".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_for_cursorcolumn(win_T *wp)
{
    ಯದಿ ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	ಯದಿ (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	ಅನ್ಯಥಾ ಯದಿ (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#endif

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    ಶೂನ್ಯ
set_valid_virtcol(win_T *wp, colnr_T vcol)
{
    wp->w_virtcol = vcol;
#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#endif
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
    ಪೂರ್ಣಾಙ್ಕ
sms_marker_overlap(win_T *wp, ಪೂರ್ಣಾಙ್ಕ extra2)
{
    ಯದಿ (extra2 == -1)
	extra2 = win_col_off(wp) - win_col_off2(wp);
#ಯದಿ defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    ಯದಿ (*get_showbreak_ಮೌಲ್ಯ(wp) != NUL)
	ಪ್ರತ್ಯಾಗಮ 0;
#endif
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    ಯದಿ (wp->w_p_list && wp->w_lcs_chars.prec)
	ಪ್ರತ್ಯಾಗಮ 1;

    ಪ್ರತ್ಯಾಗಮ extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
skipcol_from_plines(win_T *wp, ಪೂರ್ಣಾಙ್ಕ plines_off)
{
    ಪೂರ್ಣಾಙ್ಕ width1 = wp->w_width - win_col_off(wp);

    ಪೂರ್ಣಾಙ್ಕ skipcol = 0;
    ಯದಿ (plines_off > 0)
	skipcol += width1;
    ಯದಿ (plines_off > 1)
	skipcol += (width1 + win_col_off2(wp)) * (plines_off - 1);
    ಪ್ರತ್ಯಾಗಮ skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reset_skipcol(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_skipcol == 0)
	ಪ್ರತ್ಯಾಗಮ;

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
    ಶೂನ್ಯ
update_topline_redraw(ಶೂನ್ಯ)
{
    update_topline();
    ಯದಿ (must_redraw)
	update_screen(0);
}

/*
 * Return true when 'scrolloffpad' may augment 'scrolloff'.
 * This only applies to automatic cursor visibility correction.
 * For now 'scrolloffpad' is treated as boolean: 0 disables, > 0 enables.
 */
    ಸ್ಥಿರ bool
use_scrolloffpad(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ get_scrolloff_ಮೌಲ್ಯ() > 0 && get_scrolloffpad_ಮೌಲ್ಯ() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    ಸ್ಥಿರ bool
scrolloffpad_eof_pressure(linenr_T lnum, long so)
{
    ಯದಿ (!use_scrolloffpad() || so <= 0)
	ಪ್ರತ್ಯಾಗಮ false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    ಪ್ರತ್ಯಾಗಮ lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    ಶೂನ್ಯ
update_topline(ಶೂನ್ಯ)
{
    long	ಸಾಲು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		halfheight;
    ಪೂರ್ಣಾಙ್ಕ		n;
#ifdef FEAT_FOLDING
    linenr_T	lnum;
#endif
    ಪೂರ್ಣಾಙ್ಕ		check_topline = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		check_botline = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    ಪೂರ್ಣಾಙ್ಕ		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    ಯದಿ (skip_update_topline)
	ಪ್ರತ್ಯಾಗಮ;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    ಯದಿ (!screen_valid(TRUE) || curwin->w_height == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	ಪ್ರತ್ಯಾಗಮ;
    }

    check_cursor_moved(curwin);
    ಯದಿ (curwin->w_valid & VALID_TOPLINE)
	ಪ್ರತ್ಯಾಗಮ;

    // When dragging with the mouse, don't scroll that quickly
    ಯದಿ (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ old_topfill = curwin->w_topfill;
#endif

    /*
     * If the buffer is empty, always set topline to 1.
     */
    ಯದಿ (BUFEMPTY())		// special case - file is empty
    {
	ಯದಿ (curwin->w_topline != 1)
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
    ಅನ್ಯಥಾ
    {
	ಯದಿ (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    ಯದಿ (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    ಅನ್ಯಥಾ ಯದಿ (check_ತುದಿ_offset())
		check_topline = TRUE;
	    ಅನ್ಯಥಾ ಯದಿ (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		ಪೂರ್ಣಾಙ್ಕ overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, ರಿಕ್ತ, ರಿಕ್ತ, 0);
		overlap = sms_marker_overlap(curwin, -1);
		ಯದಿ (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#ifdef FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	ಯದಿ (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#endif

	ಯದಿ (check_topline)
	{
	    halfheight = curwin->w_height / 2 - 1;
	    ಯದಿ (halfheight < 2)
		halfheight = 2;

#ifdef FEAT_FOLDING
	    ಯದಿ (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		ಪ್ರತಿ (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    ಯದಿ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || n >= halfheight)
			ಭಙ್ಗ;
		    (ಶೂನ್ಯ)hasFolding(lnum, ರಿಕ್ತ, &lnum);
		}
	    }
	    ಅನ್ಯಥಾ
#endif
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    ಪೂರ್ಣಾಙ್ಕ min_scroll = scrolljump_ಮೌಲ್ಯ();
	    ಯದಿ (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    ಅನ್ಯಥಾ ಯದಿ (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    ಅನ್ಯಥಾ
	    {
		scroll_cursor_ತುದಿ(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	ಅನ್ಯಥಾ
	{
#ifdef FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ರಿಕ್ತ);
#endif
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
    ಯದಿ (check_botline)
    {
	ಯದಿ (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	ಯದಿ (curwin->w_botline <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || use_scrolloffpad())
	{
	    ಯದಿ (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		ಯದಿ (((long)curwin->w_cursor.lnum
					   >= (long)curwin->w_botline - *so_ptr
#ifdef FEAT_FOLDING
			|| hasAnyFolding(curwin)
#endif
			))
		{
		    lineoff_T	loff;

		    // Cursor is (a few lines) above botline, check if there
		    // are 'scrolloff' window lines below the cursor.
		    // If not, need to scroll.
		    n = eof_pressure ? 0 : curwin->w_empty_rows;
		    loff.lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
		    // In a fold go to its last line.
		    (ಶೂನ್ಯ)hasFolding(loff.lnum, ರಿಕ್ತ, &loff.lnum);
#endif
#ifdef FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_rows;
#endif
		    loff.height = 0;
		    ಯಾವತ್ (loff.lnum < curwin->w_botline
#ifdef FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#endif
			    )
		    {
			n += loff.height;
			ಯದಿ (n >= *so_ptr)
			    ಭಙ್ಗ;
			botline_forw(&loff);
		    }
		    ಯದಿ (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		ಅನ್ಯಥಾ
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    ಯದಿ (check_botline)
	    {
#ifdef FEAT_FOLDING
		ಯದಿ (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    ಸಾಲು_ಎಣಿಕೆ = 0;
		    ಪ್ರತಿ (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++ಸಾಲು_ಎಣಿಕೆ;
			// stop at end of file or when we know we are far off
			ಯದಿ (lnum <= 0 || ಸಾಲು_ಎಣಿಕೆ > curwin->w_height + 1)
			    ಭಙ್ಗ;
			(ಶೂನ್ಯ)hasFolding(lnum, &lnum, ರಿಕ್ತ);
		    }
		}
		ಅನ್ಯಥಾ
#endif
		    ಸಾಲು_ಎಣಿಕೆ = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		ಯದಿ (ಸಾಲು_ಎಣಿಕೆ <= curwin->w_height + 1)
		{
		    ಯದಿ (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    ಅನ್ಯಥಾ
			scroll_cursor_bot(scrolljump_ಮೌಲ್ಯ(), FALSE);
		}
		ಅನ್ಯಥಾ
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    ಯದಿ (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#endif
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	ಯದಿ (!curwin->w_p_sms)
	    reset_skipcol();
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	ಯದಿ (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
scrolljump_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಯದಿ (p_sj >= 0)
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)p_sj;
    ಪ್ರತ್ಯಾಗಮ (curwin->w_height * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_ತುದಿ_offset(ಶೂನ್ಯ)
{
    lineoff_T	loff;
    ಪೂರ್ಣಾಙ್ಕ		n;
    long	so = get_scrolloff_ಮೌಲ್ಯ();

    ಯದಿ (curwin->w_cursor.lnum < curwin->w_topline + so
#ifdef FEAT_FOLDING
		    || hasAnyFolding(curwin)
#endif
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#ಅನ್ಯಥಾ
	n = 0;
#endif
	// Count the visible screen lines above the cursor line.
	ಯಾವತ್ (n < so)
	{
	    topline_back(&loff);
	    // Stop when included a line above the window.
	    ಯದಿ (loff.lnum < curwin->w_topline
#ifdef FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#endif
		    )
		ಭಙ್ಗ;
	    n += loff.height;
	}
	ಯದಿ (n < so)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Update w_curswant.
 */
    ಶೂನ್ಯ
update_curswant_force(ಶೂನ್ಯ)
{
    validate_virtcol();
    curwin->w_curswant = curwin->w_virtcol
#ifdef FEAT_PROP_POPUP
	- curwin->w_virtcol_first_char
#endif
	;
    curwin->w_set_curswant = false;
}

/*
 * Update w_curswant if w_set_curswant is set.
 */
    ಶೂನ್ಯ
update_curswant(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_set_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    ಶೂನ್ಯ
check_cursor_moved(win_T *wp)
{
    ಯದಿ (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ಅನ್ಯಥಾ ಯದಿ (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ಅನ್ಯಥಾ ಯದಿ (wp->w_cursor.col != wp->w_valid_cursor.col
	     || wp->w_leftcol != wp->w_valid_leftcol
	     || wp->w_cursor.coladd != wp->w_valid_cursor.coladd)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
	wp->w_valid_cursor.col = wp->w_cursor.col;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_cursor.coladd = wp->w_cursor.coladd;
    }
}

/*
 * Call this function when some window settings have changed, which require
 * the cursor position, botline and topline to be recomputed and the window to
 * be redrawn.  E.g, when changing the 'wrap' option or folding.
 */
    ಶೂನ್ಯ
changed_window_setting(ಶೂನ್ಯ)
{
    changed_window_setting_win(curwin);
}

    ಶೂನ್ಯ
changed_window_setting_win(win_T *wp)
{
    wp->w_ಸಾಲುಗಳು_valid = 0;
    changed_ಸಾಲು_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    ಶೂನ್ಯ
changed_window_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಯದಿ (wp->w_buffer == buf)
	    changed_window_setting_win(wp);
}
#endif

/*
 * Call changed_window_setting_win() for every window.
 */
    ಶೂನ್ಯ
changed_window_setting_all(ಶೂನ್ಯ)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	changed_window_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    ಶೂನ್ಯ
set_topline(win_T *wp, linenr_T lnum)
{
#ifdef FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#endif

#ifdef FEAT_FOLDING
    // go to first of folded lines
    (ಶೂನ್ಯ)hasFoldingWin(wp, lnum, &lnum, ರಿಕ್ತ, TRUE, ರಿಕ್ತ);
#endif
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    ಯದಿ (wp->w_botline > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	wp->w_botline = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_set = true;
#ifdef FEAT_DIFF
    ಯದಿ (lnum != prev_topline)
	// Keep the filler lines when the topline didn't change.
	wp->w_topfill = 0;
#endif
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
    ಶೂನ್ಯ
changed_cline_bef_curs(ಶೂನ್ಯ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    ಶೂನ್ಯ
changed_cline_bef_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

/*
 * Call this function when the length of a line (in screen characters) above
 * the cursor have changed.
 * Need to take care of w_botline separately!
 */
    ಶೂನ್ಯ
changed_ಸಾಲು_abv_curs(ಶೂನ್ಯ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    ಶೂನ್ಯ
changed_ಸಾಲು_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    ಶೂನ್ಯ
changed_ಸಾಲು_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_buffer == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#endif

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    ಶೂನ್ಯ
validate_botline(ಶೂನ್ಯ)
{
    validate_botline_win(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    ಶೂನ್ಯ
validate_botline_win(win_T *wp)
{
    ಯದಿ (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    ಶೂನ್ಯ
invalidate_botline(ಶೂನ್ಯ)
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    ಶೂನ್ಯ
invalidate_botline_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    ಶೂನ್ಯ
approximate_botline_win(
    win_T	*wp)
{
    wp->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    ಪೂರ್ಣಾಙ್ಕ
cursor_valid(ಶೂನ್ಯ)
{
    check_cursor_moved(curwin);
    ಪ್ರತ್ಯಾಗಮ ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    ಶೂನ್ಯ
validate_cursor(ಶೂನ್ಯ)
{
    check_cursor_lnum();
    check_cursor_moved(curwin);
    ಯದಿ ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#ಯದಿ defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    ಶೂನ್ಯ
validate_cline_row(ಶೂನ್ಯ)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    ಯದಿ (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);
}
#endif

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
curs_rows(win_T *wp)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		all_invalid;
    ಪೂರ್ಣಾಙ್ಕ		valid;
#ifdef FEAT_FOLDING
    long	fold_ಎಣಿಕೆ;
#endif

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_ಸಾಲುಗಳು_valid == 0
			|| wp->w_ಸಾಲುಗಳು[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_row = 0;
    ಪ್ರತಿ (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	ಯದಿ (!all_invalid && i < wp->w_ಸಾಲುಗಳು_valid)
	{
	    ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_lnum < lnum || !wp->w_ಸಾಲುಗಳು[i].wl_valid)
		ಅನುವರ್ತ;		// skip changed or deleted lines
	    ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_lnum == lnum)
	    {
#ifdef FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		ಯದಿ (!wp->w_buffer->b_mod_set
			|| wp->w_ಸಾಲುಗಳು[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_buffer->b_mod_ತುದಿ
					     > wp->w_ಸಾಲುಗಳು[i].wl_lastlnum + 1)
#endif
		valid = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	ಯದಿ (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#ifdef FEAT_DIFF
					    && !wp->w_p_diff
#endif
	       )))
	{
#ifdef FEAT_FOLDING
	    lnum = wp->w_ಸಾಲುಗಳು[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    ಯದಿ (lnum > wp->w_cursor.lnum)
		ಭಙ್ಗ;
#ಅನ್ಯಥಾ
	    ++lnum;
#endif
	    wp->w_cline_row += wp->w_ಸಾಲುಗಳು[i].wl_size;
	}
	ಅನ್ಯಥಾ
	{
#ifdef FEAT_FOLDING
	    fold_ಎಣಿಕೆ = folded_ಎಣಿಕೆ(wp, lnum, ರಿಕ್ತ);
	    ಯದಿ (fold_ಎಣಿಕೆ)
	    {
		lnum += fold_ಎಣಿಕೆ;
		ಯದಿ (lnum > wp->w_cursor.lnum)
		    ಭಙ್ಗ;
		++wp->w_cline_row;
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		wp->w_cline_row += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    ಯದಿ (!(wp->w_valid & VALID_CHEIGHT))
    {
	ಯದಿ (all_invalid
		|| i == wp->w_ಸಾಲುಗಳು_valid
		|| (i < wp->w_ಸಾಲುಗಳು_valid
		    && (!wp->w_ಸಾಲುಗಳು[i].wl_valid
			|| wp->w_ಸಾಲುಗಳು[i].wl_lnum != wp->w_cursor.lnum)))
	{
#ifdef FEAT_DIFF
	    ಯದಿ (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_height = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    ಅನ್ಯಥಾ
#endif
		wp->w_cline_height = plines_win(wp, wp->w_cursor.lnum, TRUE);
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ರಿಕ್ತ, ರಿಕ್ತ, TRUE, ರಿಕ್ತ);
#endif
	}
	ಅನ್ಯಥಾ ಯದಿ (i > wp->w_ಸಾಲುಗಳು_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_height = 0;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ರಿಕ್ತ, ರಿಕ್ತ, TRUE, ರಿಕ್ತ);
#endif
	}
	ಅನ್ಯಥಾ
	{
	    wp->w_cline_height = wp->w_ಸಾಲುಗಳು[i].wl_size;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = wp->w_ಸಾಲುಗಳು[i].wl_folded;
#endif
	}
    }

    redraw_for_cursorline(curwin);
    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;
}

/*
 * Validate curwin->w_virtcol only.
 */
    ಶೂನ್ಯ
validate_virtcol(ಶೂನ್ಯ)
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    ಶೂನ್ಯ
validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);

    ಯದಿ (wp->w_valid & VALID_VIRTCOL)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_PROP_POPUP
    wp->w_virtcol_first_char = 0;
#endif
    getvvcol(wp, &wp->w_cursor, ರಿಕ್ತ, &(wp->w_virtcol), ರಿಕ್ತ,
							 GETVCOL_FOR_VIRTCOL);
#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#endif
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    ಶೂನ್ಯ
validate_cheight(ಶೂನ್ಯ)
{
    check_cursor_moved(curwin);

    ಯದಿ (curwin->w_valid & VALID_CHEIGHT)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_DIFF
    ಯದಿ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_height = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    ಅನ್ಯಥಾ
#endif
	curwin->w_cline_height = plines(curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, ರಿಕ್ತ, ರಿಕ್ತ);
#endif
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    ಶೂನ್ಯ
validate_cursor_col(ಶೂನ್ಯ)
{
    colnr_T off;
    colnr_T col;
    ಪೂರ್ಣಾಙ್ಕ     width;

    validate_virtcol();

    ಯದಿ (curwin->w_valid & VALID_WCOL)
	ಪ್ರತ್ಯಾಗಮ;

    col = curwin->w_virtcol;
    off = curwin_col_off();
    col += off;
    width = curwin->w_width - off + curwin_col_off2();

    // long line wrapping, adjust curwin->w_wrow
    ಯದಿ (curwin->w_p_wrap
	    && col >= (colnr_T)curwin->w_width
	    && width > 0)
	// use same formula as what is used in curs_columns()
	col -= ((col - curwin->w_width) / width + 1) * width;
    ಯದಿ (col > (ಪೂರ್ಣಾಙ್ಕ)curwin->w_leftcol)
	col -= curwin->w_leftcol;
    ಅನ್ಯಥಾ
	col = 0;
    curwin->w_wcol = col;

    curwin->w_valid |= VALID_WCOL;
#ifdef FEAT_PROP_POPUP
    curwin->w_flags &= ~WFLAG_WCOL_OFF_ADDED;
#endif
}

/*
 * Compute offset of a window, occupied by absolute or relative line number,
 * fold column and sign column (these don't move when scrolling horizontally).
 */
    ಪೂರ್ಣಾಙ್ಕ
win_col_off(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ (((wp->w_p_nu || wp->w_p_rnu) ? ಸಂಖ್ಯೆ_width(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#ifdef FEAT_FOLDING
	    + wp->w_p_fdc
#endif
#ifdef FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#endif
	   );
}

    ಪೂರ್ಣಾಙ್ಕ
curwin_col_off(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ win_col_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    ಪೂರ್ಣಾಙ್ಕ
win_col_off2(win_T *wp)
{
    ಯದಿ ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ಸಂಖ್ಯೆ_width(wp) + 1;
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಪೂರ್ಣಾಙ್ಕ
curwin_col_off2(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ win_col_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    ಶೂನ್ಯ
curs_columns(
    ಪೂರ್ಣಾಙ್ಕ		may_scroll)	// when TRUE, may scroll horizontally
{
    long	diff;
    ಪೂರ್ಣಾಙ್ಕ		extra;		// offset for first screen line
    long	off_ಎಡ, off_ಬಲ;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		p_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಙ್ಕ		width1;		// text width for first screen line
    ಪೂರ್ಣಾಙ್ಕ		width2 = 0;	// text width for second and later screen line
    ಪೂರ್ಣಾಙ್ಕ		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    long	so = get_scrolloff_ಮೌಲ್ಯ();
    long	siso = get_sidescrolloff_ಮೌಲ್ಯ();
    ಪೂರ್ಣಾಙ್ಕ		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    ಯದಿ (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);

#ifdef FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_first_char = 0;
#endif

    /*
     * Compute the number of virtual columns.
     */
#ifdef FEAT_FOLDING
    ಯದಿ (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    ಅನ್ಯಥಾ
#endif
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    ಯದಿ (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_col_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_row;

    width1 = curwin->w_width - extra;
    ಯದಿ (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_width - 1;
	ಯದಿ (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_height - 1;
	ಅನ್ಯಥಾ
	    curwin->w_wrow = curwin->w_height - 1 - curwin->w_empty_rows;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_p_wrap && curwin->w_width != 0)
    {
	width2 = width1 + curwin_col_off2();

	// skip columns that are not visible
	ಯದಿ (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    ಯದಿ (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    ಅನ್ಯಥಾ
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	ಯದಿ (curwin->w_wcol >= curwin->w_width)
	{
	    // this same formula is used in validate_cursor_col()
	    n = (curwin->w_wcol - curwin->w_width) / width2 + 1;
	    curwin->w_wcol -= n * width2;
	    curwin->w_wrow += n;
	}
    }

    // No line wrapping: compute curwin->w_leftcol if scrolling is on and line
    // is not folded.
    // If scrolling is off, curwin->w_leftcol is assumed to be 0
    ಅನ್ಯಥಾ ಯದಿ (may_scroll
#ifdef FEAT_FOLDING
	    && !curwin->w_cline_folded
#endif
	    )
    {
#ifdef FEAT_PROP_POPUP
	ಯದಿ (curwin->w_virtcol_first_char > 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ cols = (curwin->w_width - extra);
	    ಪೂರ್ಣಾಙ್ಕ rows = cols > 0 ? curwin->w_virtcol_first_char / cols : 1;

	    // each "above" text prop shifts the text one row down
	    curwin->w_wrow += rows;
	    curwin->w_wcol -= rows * cols;
	    endcol -= rows * cols;
	    curwin->w_cline_height = rows + 1;
	}
#endif
	/*
	 * If Cursor is left of the screen, scroll rightwards.
	 * If Cursor is right of the screen, scroll leftwards.
	 * If we get closer to the edge than 'sidescrolloff', scroll a little
	 * extra.
	 */
	off_ಎಡ = startcol - curwin->w_leftcol - siso;
	off_ಬಲ = endcol - curwin->w_leftcol - (curwin->w_width - siso) + 1;
	ಯದಿ (off_ಎಡ < 0 || off_ಬಲ > 0)
	{
	    ಯದಿ (off_ಎಡ < 0)
		diff = -off_ಎಡ;
	    ಅನ್ಯಥಾ
		diff = off_ಬಲ;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    ಯದಿ (p_ss == 0 || diff >= width1 / 2 || off_ಬಲ >= off_ಎಡ)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (diff < p_ss)
		    diff = p_ss;
		ಯದಿ (off_ಎಡ < 0)
		    new_leftcol = curwin->w_leftcol - (ಪೂರ್ಣಾಙ್ಕ)diff;
		ಅನ್ಯಥಾ
		    new_leftcol = curwin->w_leftcol + (ಪೂರ್ಣಾಙ್ಕ)diff;
	    }
	    ಯದಿ (new_leftcol < 0)
		new_leftcol = 0;
	    ಯದಿ (new_leftcol != (ಪೂರ್ಣಾಙ್ಕ)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_wcol > (ಪೂರ್ಣಾಙ್ಕ)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    ಅನ್ಯಥಾ
	curwin->w_wcol = 0;

#ifdef FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    ಯದಿ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    ಅನ್ಯಥಾ
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#endif

    prev_skipcol = curwin->w_skipcol;

    p_ಸಾಲುಗಳು = 0;

    ಯದಿ ((curwin->w_wrow >= curwin->w_height
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_height)
		    && (p_ಸಾಲುಗಳು =
#ifdef FEAT_DIFF
			plines_win_nofill
#ಅನ್ಯಥಾ
			plines_win
#endif
			(curwin, curwin->w_cursor.lnum, FALSE))
						    - 1 >= curwin->w_height))
	    && curwin->w_height != 0
	    && curwin->w_cursor.lnum == curwin->w_topline
	    && width2 > 0
	    && curwin->w_width != 0)
    {
	// Cursor past end of screen.  Happens with a single line that does
	// not fit on screen.  Find a skipcol to show the text around the
	// cursor.  Avoid scrolling all the time. compute value of "extra":
	// 1: Less than 'scrolloff' lines above
	// 2: Less than 'scrolloff' lines below
	// 3: both of them
	extra = 0;
	ಯದಿ (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	ಯದಿ (p_ಸಾಲುಗಳು == 0)
	    p_ಸಾಲುಗಳು = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_ಸಾಲುಗಳು;
	ಯದಿ (p_ಸಾಲುಗಳು > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	ಅನ್ಯಥಾ
	    n = p_ಸಾಲುಗಳು;
	ಯದಿ ((colnr_T)n >= curwin->w_height + curwin->w_skipcol / width2 - so)
	    extra += 2;

	ಯದಿ (extra == 3 || curwin->w_height <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    ಯದಿ (n > curwin->w_height / 2)
		n -= curwin->w_height / 2;
	    ಅನ್ಯಥಾ
		n = 0;
	    // don't skip more than necessary
	    ಯದಿ (n > p_ಸಾಲುಗಳು - curwin->w_height + 1)
		n = p_ಸಾಲುಗಳು - curwin->w_height + 1;
	    ಯದಿ (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    ಅನ್ಯಥಾ
		curwin->w_skipcol = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    ಯದಿ (extra > 0)
	    {
		ಯದಿ ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_height + 1) * width2;
	    ಯಾವತ್ (endcol > curwin->w_virtcol)
		endcol -= width2;
	    ಯದಿ (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	ಯದಿ (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - prev_skipcol) / width2;
	ಅನ್ಯಥಾ
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	ಯದಿ (curwin->w_wrow >= curwin->w_height)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_height + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((ಪೂರ್ಣಾಙ್ಕ)prev_skipcol - (ಪೂರ್ಣಾಙ್ಕ)curwin->w_skipcol) / width2;
	ಯದಿ (extra > 0)
	    win_ins_ಸಾಲುಗಳು(curwin, 0, extra, FALSE, FALSE);
	ಅನ್ಯಥಾ ಯದಿ (extra < 0)
	    win_del_ಸಾಲುಗಳು(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    ಅನ್ಯಥಾ ಯದಿ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    ಯದಿ (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#endif
#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ಯದಿ (popup_is_popup(curwin) && curbuf->b_term != ರಿಕ್ತ)
    {
	curwin->w_wrow += popup_ತುದಿ_extra(curwin);
	curwin->w_wcol += popup_ಎಡ_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    ಅನ್ಯಥಾ
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#endif

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    ಶೂನ್ಯ
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	ಪೂರ್ಣಾಙ್ಕ	*rowp,	// screen row
	ಪೂರ್ಣಾಙ್ಕ	*scolp,	// start screen column
	ಪೂರ್ಣಾಙ್ಕ	*ccolp,	// cursor screen column
	ಪೂರ್ಣಾಙ್ಕ	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    ಪೂರ್ಣಾಙ್ಕ		row = 0;
    colnr_T	coloff = 0;

    ಯದಿ (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    col;
	ಪೂರ್ಣಾಙ್ಕ	    width;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	ಪೂರ್ಣಾಙ್ಕ	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, ರಿಕ್ತ, TRUE, ರಿಕ್ತ);
# endif
	row = plines_m_win(wp, wp->w_topline, lnum - 1, INT_MAX);
	// "row" should be the screen line where line "lnum" begins, which can
	// be negative if "lnum" is "w_topline" and "w_skipcol" is non-zero.
	row -= adjust_plines_for_skipcol(wp);

# ifdef FEAT_DIFF
	// Add filler lines above this buffer line.
	row += lnum == wp->w_topline ? wp->w_topfill
				     : diff_check_fill(wp, lnum);
# endif

	colnr_T	off = win_col_off(wp);
# ifdef FEAT_FOLDING
	ಯದಿ (is_folded)
	{
	    row += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	ಅನ್ಯಥಾ
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    col = scol;
	    col += off;
	    width = wp->w_width - off + win_col_off2(wp);

	    // long line wrapping, adjust row
	    ಯದಿ (wp->w_p_wrap
		    && col >= (colnr_T)wp->w_width
		    && width > 0)
	    {
		// use same formula as what is used in curs_columns()
		ಪೂರ್ಣಾಙ್ಕ rowoff = ((col - wp->w_width) / width + 1);
		col -= rowoff * width;
		row += rowoff;
	    }
	    col -= wp->w_leftcol;
	    ಯದಿ (col >= wp->w_width)
		col = -1;
	    ಯದಿ (col >= 0 && row >= 0 && row < wp->w_height)
	    {
		coloff = col - scol + wp->w_wincol + 1;
		row += W_WINROW(wp) + 1;
	    }
	    ಅನ್ಯಥಾ
		// character is out of the window
		row = scol = ccol = ecol = 0;
	}
    }
    *rowp = row;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    ಯದಿ (wp->w_p_rl && row > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	ಪೂರ್ಣಾಙ್ಕ endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_width - endoff;
    }
# endif
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    ಶೂನ್ಯ
f_screenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    ಪೂರ್ಣಾಙ್ಕ		row = 0;
    ಪೂರ್ಣಾಙ್ಕ		scol = 0, ccol = 0, ecol = 0;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    dict = rettv->vval.v_dict;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    pos.lnum = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    ಯದಿ (pos.lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), pos.lnum);
	ಪ್ರತ್ಯಾಗಮ;
    }
    pos.col = tv_get_ಸಂಖ್ಯೆ(&argvars[2]) - 1;
    ಯದಿ (pos.col < 0)
	pos.col = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &row, &scol, &ccol, &ecol);

    dict_add_ಸಂಖ್ಯೆ(dict, "row", row);
    dict_add_ಸಂಖ್ಯೆ(dict, "col", scol);
    dict_add_ಸಂಖ್ಯೆ(dict, "curscol", ccol);
    dict_add_ಸಂಖ್ಯೆ(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
virtcol2col(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ vcol)
{
    ಪೂರ್ಣಾಙ್ಕ		offset = vcol2col(wp, lnum, vcol - 1, ರಿಕ್ತ);
    char_u	*ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);
    char_u	*p = ಸಾಲು + offset;

    ಯದಿ (*p == NUL)
    {
	ಯದಿ (p == ಸಾಲು)  // empty line
	    ಪ್ರತ್ಯಾಗಮ 0;
	// Move to the first byte of the last char.
	MB_PTR_BACK(ಸಾಲು, p);
    }
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    ಶೂನ್ಯ
f_virtcol2col(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		screencol;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
	    || check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
	    || check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    lnum = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಯದಿ (error || lnum < 0 || lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;

    screencol = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    ಯದಿ (error || screencol < 0)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = virtcol2col(wp, lnum, screencol);
}
#endif

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
ಸ್ಥಿರ ಶೂನ್ಯ cursor_correct_sms(ಶೂನ್ಯ)
{
    ಯದಿ (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	ಪ್ರತ್ಯಾಗಮ;

    long    so = get_scrolloff_ಮೌಲ್ಯ();
    ಪೂರ್ಣಾಙ್ಕ	    width1 = curwin->w_width - curwin_col_off();
    ಪೂರ್ಣಾಙ್ಕ	    width2 = width1 + curwin_col_off2();
    ಪೂರ್ಣಾಙ್ಕ	    so_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    ಪೂರ್ಣಾಙ್ಕ	    space_cols = (curwin->w_height - 1) * width2;
    ಪೂರ್ಣಾಙ್ಕ	    overlap, ತುದಿ, bot;
    ಪೂರ್ಣಾಙ್ಕ	    size = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    ಯದಿ (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_cols = 0;               // Ignore 'scrolloff' at top of buffer.
    ಅನ್ಯಥಾ ಯದಿ (so_cols > space_cols / 2)
	so_cols = space_cols / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    ಯಾವತ್ (so_cols > size && so_cols - width2 >= width1 && width1 > 0)
	so_cols -= width2;
    ಯದಿ (so_cols >= width1 && so_cols > size)
	so_cols -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_width - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    ತುದಿ = curwin->w_skipcol + (so_cols != 0 ? so_cols : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_height - 1) * width2
								    - so_cols;
    validate_virtcol();
    colnr_T col = curwin->w_virtcol;

    ಯದಿ (col < ತುದಿ)
    {
	ಯದಿ (col < width1)
	    col += width1;
	ಯಾವತ್ (width2 > 0 && col < ತುದಿ)
	    col += width2;
    }
    ಅನ್ಯಥಾ
	ಯಾವತ್ (width2 > 0 && col >= bot)
	    col -= width2;

    ಯದಿ (col != curwin->w_virtcol)
    {
	ಪೂರ್ಣಾಙ್ಕ rc;

	curwin->w_curswant = col;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	ಯದಿ (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    validate_virtcol();
	    ಯದಿ (curwin->w_virtcol < curwin->w_skipcol + overlap)
	    {
		// Cursor still not visible: move it to the next line instead.
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
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
    ಶೂನ್ಯ
scroll_redraw(ಪೂರ್ಣಾಙ್ಕ up, long ಎಣಿಕೆ)
{
    linenr_T	prev_topline = curwin->w_topline;
    ಪೂರ್ಣಾಙ್ಕ		prev_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		prev_topfill = curwin->w_topfill;
#endif
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    ಯದಿ (up)
	scrollup(ಎಣಿಕೆ, TRUE);
    ಅನ್ಯಥಾ
	scrolldown(ಎಣಿಕೆ, TRUE);
    ಯದಿ (get_scrolloff_ಮೌಲ್ಯ() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	ಯಾವತ್ (curwin->w_topline == prev_topline
		&& curwin->w_skipcol == prev_skipcol
#ifdef FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#endif
		)
	{
	    ಯದಿ (up)
	    {
		ಯದಿ (curwin->w_cursor.lnum > prev_lnum
			|| cursor_down(1L, FALSE) == FAIL)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_up(1L, FALSE) == FAIL)
		    ಭಙ್ಗ;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    ಯದಿ (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    ಶೂನ್ಯ
scrolldown(
    long	ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    long	done = 0;	// total # of physical lines done
    ಪೂರ್ಣಾಙ್ಕ		wrow;
    ಪೂರ್ಣಾಙ್ಕ		moved = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ಪೂರ್ಣಾಙ್ಕ		width1 = 0;
    ಪೂರ್ಣಾಙ್ಕ		width2 = 0;

    ಯದಿ (do_sms)
    {
	width1 = curwin->w_width - curwin_col_off();
	width2 = width1 + curwin_col_off2();
    }

#ifdef FEAT_FOLDING
    linenr_T	first;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ರಿಕ್ತ);
#endif
    validate_cursor();		// w_wrow needs to be valid
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ todo = ಸಾಲು_ಎಣಿಕೆ; todo > 0; --todo)
    {
#ifdef FEAT_DIFF
	ಯದಿ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_height - 1)
	{
	    ++curwin->w_topfill;
	    ++done;
	}
	ಅನ್ಯಥಾ
#endif
	{
	    // break when at the very top
	    ಯದಿ (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		ಭಙ್ಗ;
	    ಯದಿ (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		ಯದಿ (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		ಅನ್ಯಥಾ
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++done;
	    }
	    ಅನ್ಯಥಾ
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#ifdef FEAT_DIFF
		curwin->w_topfill = 0;
#endif
#ifdef FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		ಯದಿ (hasFolding(curwin->w_topline, &first, ರಿಕ್ತ))
		{
		    ++done;
		    ಯದಿ (!byfold)
			todo -= curwin->w_topline - first - 1;
		    curwin->w_botline -= curwin->w_topline - first;
		    curwin->w_topline = first;
		}
		ಅನ್ಯಥಾ
#endif
		ಯದಿ (do_sms)
		{
		    ಪೂರ್ಣಾಙ್ಕ size = linetabsize_eol(curwin, curwin->w_topline);
		    ಯದಿ (size > width1)
		    {
			curwin->w_skipcol = width1;
			size -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    ಯಾವತ್ (size > width2)
		    {
			curwin->w_skipcol += width2;
			size -= width2;
		    }
		    ++done;
		}
		ಅನ್ಯಥಾ
		    done += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += done;		// keep w_wrow updated
    curwin->w_cline_row += done;	// keep w_cline_row updated

#ifdef FEAT_DIFF
    ಯದಿ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_row = 0;
    check_topfill(curwin, TRUE);
#endif

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    ಯದಿ (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    ಯಾವತ್ (wrow >= curwin->w_height && curwin->w_cursor.lnum > 1)
    {
#ifdef FEAT_FOLDING
	ಯದಿ (hasFolding(curwin->w_cursor.lnum, &first, ರಿಕ್ತ))
	{
	    --wrow;
	    ಯದಿ (first == 1)
		curwin->w_cursor.lnum = 1;
	    ಅನ್ಯಥಾ
		curwin->w_cursor.lnum = first - 1;
	}
	ಅನ್ಯಥಾ
#endif
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    ಯದಿ (moved)
    {
#ifdef FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#endif
	coladvance(curwin->w_curswant);
    }
    ಯದಿ (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    ಶೂನ್ಯ
scrollup(
    long	ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    ಪೂರ್ಣಾಙ್ಕ		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    ಯದಿ (do_sms
#ifdef FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#endif
#ifdef FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#endif
       )
    {
	ಪೂರ್ಣಾಙ್ಕ	    width1 = curwin->w_width - curwin_col_off();
	ಪೂರ್ಣಾಙ್ಕ	    width2 = width1 + curwin_col_off2();
	ಪೂರ್ಣಾಙ್ಕ	    size = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	ಯದಿ (do_sms)
	    size = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ todo = ಸಾಲು_ಎಣಿಕೆ; todo > 0; --todo)
	{
#ifdef FEAT_DIFF
	    ಯದಿ (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    ಅನ್ಯಥಾ
#endif
	    {
		linenr_T lnum = curwin->w_topline;

#ifdef FEAT_FOLDING
		ಯದಿ (byfold)
		    // for a closed fold: go to the last line in the fold
		    (ಶೂನ್ಯ)hasFolding(lnum, ರಿಕ್ತ, &lnum);
#endif
		ಯದಿ (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    ಪೂರ್ಣಾಙ್ಕ add = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += add;
		    ಯದಿ (curwin->w_skipcol >= size)
		    {
			ಯದಿ (lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= add;
			    ಭಙ್ಗ;
			}
			++lnum;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			ಭಙ್ಗ;
		    ++lnum;
		}

		ಯದಿ (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#ifdef FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#endif
		    curwin->w_skipcol = 0;
		    ಯದಿ (todo > 1 && do_sms)
			size = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	ಯದಿ (prev_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    ಅನ್ಯಥಾ
    {
	curwin->w_topline += ಸಾಲು_ಎಣಿಕೆ;
	curwin->w_botline += ಸಾಲು_ಎಣಿಕೆ;	// approximate w_botline
    }

    ಯದಿ (curwin->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	curwin->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಯದಿ (curwin->w_botline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	curwin->w_botline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;

#ifdef FEAT_DIFF
    check_topfill(curwin, FALSE);
#endif

#ifdef FEAT_FOLDING
    ಯದಿ (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ರಿಕ್ತ);
#endif

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    ಯದಿ (curwin->w_cursor.lnum < curwin->w_topline)
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
    ಶೂನ್ಯ
adjust_skipcol(ಶೂನ್ಯ)
{
    ಯದಿ (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ	    width1 = curwin->w_width - curwin_col_off();
    ಯದಿ (width1 <= 0)
	ಪ್ರತ್ಯಾಗಮ;  // no text will be displayed

    ಪೂರ್ಣಾಙ್ಕ	    width2 = width1 + curwin_col_off2();
    long    so = get_scrolloff_ಮೌಲ್ಯ();
    ಪೂರ್ಣಾಙ್ಕ	    scrolloff_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    ಪೂರ್ಣಾಙ್ಕ	    scrolled = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    row = 0;
    ಪೂರ್ಣಾಙ್ಕ	    overlap, col;

    validate_cheight();
    ಯದಿ (curwin->w_cline_height == curwin->w_height
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_height)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	ಪ್ರತ್ಯಾಗಮ;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_width - width2);
    ಯಾವತ್ (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_cols)
    {
	// scroll a screen line down
	ಯದಿ (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	ಅನ್ಯಥಾ
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    ಯದಿ (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	ಪ್ರತ್ಯಾಗಮ;  // don't scroll in the other direction now
    }

    col = curwin->w_virtcol + scrolloff_cols;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    ಯದಿ (scrolloff_cols > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ size = linetabsize_eol(curwin, curwin->w_topline);
	size = width1 + width2 * ((size - width1 + width2 - 1) / width2);
	ಯಾವತ್ (col > size)
	    col -= width2;
    }
    col -= curwin->w_skipcol;

    ಯದಿ (col >= width1)
    {
	col -= width1;
	++row;
    }
    ಯದಿ (col > width2)
    {
	row += col / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	col = col % width2;
    }
    ಯದಿ (row >= curwin->w_height)
    {
	ಯದಿ (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --row;
	}
	ಯದಿ (row >= curwin->w_height)
	    curwin->w_skipcol += (row - curwin->w_height) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#ifdef FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    ಶೂನ್ಯ
check_topfill(
    win_T	*wp,
    ಪೂರ್ಣಾಙ್ಕ		down)	// when TRUE scroll down when not enough space
{
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (wp->w_topfill <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    ಯದಿ (wp->w_topfill + n > wp->w_height)
    {
	ಯದಿ (down && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	ಅನ್ಯಥಾ
	{
	    wp->w_topfill = wp->w_height - n;
	    ಯದಿ (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#endif

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    ಶೂನ್ಯ
scrolldown_clamp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		end_row;
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#endif

    ಯದಿ (curwin->w_topline <= 1
#ifdef FEAT_DIFF
	    && !can_fill
#endif
	    )
	ಪ್ರತ್ಯಾಗಮ;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    end_row = curwin->w_wrow;
#ifdef FEAT_DIFF
    ಯದಿ (can_fill)
	++end_row;
    ಅನ್ಯಥಾ
	end_row += plines_nofill(curwin->w_topline - 1);
#ಅನ್ಯಥಾ
    end_row += plines(curwin->w_topline - 1);
#endif
    ಯದಿ (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_cheight();
	validate_virtcol();
	end_row += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    ಯದಿ (end_row < curwin->w_height - get_scrolloff_ಮೌಲ್ಯ())
    {
#ifdef FEAT_DIFF
	ಯದಿ (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	ಅನ್ಯಥಾ
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#ಅನ್ಯಥಾ
	--curwin->w_topline;
#endif
#ifdef FEAT_FOLDING
	(ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ರಿಕ್ತ);
#endif
	--curwin->w_botline;	    // approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    ಶೂನ್ಯ
scrollup_clamp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    start_row;

    ಯದಿ (curwin->w_topline == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
#ifdef FEAT_DIFF
	    && curwin->w_topfill == 0
#endif
	    )
	ಪ್ರತ್ಯಾಗಮ;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#ifdef FEAT_DIFF
    start_row = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#ಅನ್ಯಥಾ
    start_row = curwin->w_wrow - plines(curwin->w_topline);
#endif
    ಯದಿ (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	start_row -= curwin->w_virtcol / curwin->w_width;
    }
    ಯದಿ (start_row >= get_scrolloff_ಮೌಲ್ಯ())
    {
#ifdef FEAT_DIFF
	ಯದಿ (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	ಅನ್ಯಥಾ
#endif
	{
#ifdef FEAT_FOLDING
	    (ಶೂನ್ಯ)hasFolding(curwin->w_topline, ರಿಕ್ತ, &curwin->w_topline);
#endif
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
    ಸ್ಥಿರ ಶೂನ್ಯ
topline_back_winheight(
    lineoff_T	*lp,
    ಪೂರ್ಣಾಙ್ಕ		winheight)	// when TRUE limit to window height
{
#ifdef FEAT_DIFF
    ಯದಿ (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    ಅನ್ಯಥಾ
#endif
    {
	--lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	ಯದಿ (lp->lnum < 1)
	    lp->height = MAXCOL;
	ಅನ್ಯಥಾ
#ifdef FEAT_FOLDING
	    ಯದಿ (hasFolding(lp->lnum, &lp->lnum, ರಿಕ್ತ))
	    // Add a closed fold
	    lp->height = 1;
	ಅನ್ಯಥಾ
#endif
	    lp->height = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
topline_back(lineoff_T *lp)
{
    topline_back_winheight(lp, TRUE);
}


/*
 * Add one line below "lp->lnum".  This can be a filler line, a closed fold or
 * a (wrapped) text line.  Uses and sets "lp->fill".
 * Returns the height of the added line in "lp->height".
 * Lines below the last one are incredibly high.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
botline_forw(lineoff_T *lp)
{
#ifdef FEAT_DIFF
    ಯದಿ (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    ಅನ್ಯಥಾ
#endif
    {
	++lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	ಯದಿ (lp->lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    lp->height = MAXCOL;
	ಅನ್ಯಥಾ
#ifdef FEAT_FOLDING
	    ಯದಿ (hasFolding(lp->lnum, ರಿಕ್ತ, &lp->lnum))
	    {
		// Add a closed fold.
		lp->height = 1;
	    }
	    ಅನ್ಯಥಾ
#endif
	    lp->height = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    ಶೂನ್ಯ
scroll_cursor_ತುದಿ(ಪೂರ್ಣಾಙ್ಕ min_scroll, ಪೂರ್ಣಾಙ್ಕ always)
{
    ಪೂರ್ಣಾಙ್ಕ		scrolled = 0;
    ಪೂರ್ಣಾಙ್ಕ		extra = 0;
    ಪೂರ್ಣಾಙ್ಕ		used;
    ಪೂರ್ಣಾಙ್ಕ		i;
    linenr_T	ತುದಿ;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    ಪೂರ್ಣಾಙ್ಕ		old_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#endif
    linenr_T	new_topline;
    ಪೂರ್ಣಾಙ್ಕ		off = get_scrolloff_ಮೌಲ್ಯ();

    ಯದಿ (mouse_dragging > 0)
	off = mouse_dragging - 1;

    /*
     * Decrease topline until:
     * - it has become 1
     * - (part of) the cursor line is moved off the screen or
     * - moved at least 'scrolljump' lines and
     * - at least 'scrolloff' lines above and below the cursor
     */
    validate_cheight();
    used = curwin->w_cline_height; // includes filler lines above
    ಯದಿ (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = used;

#ifdef FEAT_FOLDING
    ಯದಿ (hasFolding(curwin->w_cursor.lnum, &ತುದಿ, &bot))
    {
	--ತುದಿ;
	++bot;
    }
    ಅನ್ಯಥಾ
#endif
    {
	ತುದಿ = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = ತುದಿ + 1;

#ifdef FEAT_DIFF
    // "used" already contains the number of filler lines above, don't add it
    // again.
    // Hide filler lines above cursor line by adding them to "extra".
    extra += diff_check_fill(curwin, curwin->w_cursor.lnum);
#endif

    /*
     * Check if the lines from "top" to "bot" fit in the window.  If they do,
     * set new_topline and advance "top" and "bot" to include more lines.
     */
    ಯಾವತ್ (ತುದಿ > 0)
    {
#ifdef FEAT_FOLDING
	ಯದಿ (hasFolding(ತುದಿ, &ತುದಿ, ರಿಕ್ತ))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	ಅನ್ಯಥಾ
#endif
	    i = PLINES_NOFILL(ತುದಿ);
	ಯದಿ (ತುದಿ < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	ಯದಿ ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    ಭಙ್ಗ;

	used += i;
	ಯದಿ (extra + i <= off && bot < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
#ifdef FEAT_FOLDING
	    ಯದಿ (hasFolding(bot, ರಿಕ್ತ, &bot))
		// count one logical line for a sequence of folded lines
		++used;
	    ಅನ್ಯಥಾ
#endif
		used += plines(bot);
	}
	ಯದಿ (used > curwin->w_height)
	    ಭಙ್ಗ;

	extra += i;
	new_topline = ತುದಿ;
	--ತುದಿ;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    ಯದಿ (used > curwin->w_height)
	scroll_cursor_halfway(FALSE, FALSE);
    ಅನ್ಯಥಾ
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	ಯದಿ (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	ಯದಿ (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	ಯದಿ (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    ಯದಿ (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#endif
	ಯದಿ (curwin->w_topline != old_topline)
	    reset_skipcol();
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    ಯದಿ (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		ಪೂರ್ಣಾಙ್ಕ	width1 = curwin->w_width - curwin_col_off();
		ಪೂರ್ಣಾಙ್ಕ	width2 = width1 + curwin_col_off2();
		ಪೂರ್ಣಾಙ್ಕ	plines_off = 0;
		ಪೂರ್ಣಾಙ್ಕ	skipcol;

		ಯದಿ (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		ಯದಿ (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	ಯದಿ (curwin->w_topline != old_topline
		|| curwin->w_skipcol != old_skipcol
#ifdef FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#endif
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
    ಶೂನ್ಯ
set_empty_rows(win_T *wp, ಪೂರ್ಣಾಙ್ಕ used)
{
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    ಯದಿ (used == 0)
	wp->w_empty_rows = 0;	// single line that doesn't fit
    ಅನ್ಯಥಾ
    {
	wp->w_empty_rows = wp->w_height - used;
#ifdef FEAT_DIFF
	ಯದಿ (wp->w_botline <= wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    wp->w_filler_rows = diff_check_fill(wp, wp->w_botline);
	    ಯದಿ (wp->w_empty_rows > wp->w_filler_rows)
		wp->w_empty_rows -= wp->w_filler_rows;
	    ಅನ್ಯಥಾ
	    {
		wp->w_filler_rows = wp->w_empty_rows;
		wp->w_empty_rows = 0;
	    }
	}
#endif
    }
}

/*
 * Recompute topline to put the cursor at the bottom of the window.
 * When scrolling scroll at least "min_scroll" lines.
 * If "set_topbot" is TRUE, set topline and botline first (for "zb").
 * This is messy stuff!!!
 */
    ಶೂನ್ಯ
scroll_cursor_bot(ಪೂರ್ಣಾಙ್ಕ min_scroll, ಪೂರ್ಣಾಙ್ಕ set_topbot)
{
    ಪೂರ್ಣಾಙ್ಕ		used;
    ಪೂರ್ಣಾಙ್ಕ		scrolled = 0;
    ಪೂರ್ಣಾಙ್ಕ		extra = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;
    linenr_T	old_topline = curwin->w_topline;
    ಪೂರ್ಣಾಙ್ಕ		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		old_topfill = curwin->w_topfill;
    ಪೂರ್ಣಾಙ್ಕ		fill_below_window;
#endif
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    ಪೂರ್ಣಾಙ್ಕ		old_empty_rows = curwin->w_empty_rows;
    linenr_T	cln;		    // Cursor Line Number
    long	so = get_scrolloff_ಮೌಲ್ಯ();
    ಪೂರ್ಣಾಙ್ಕ		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    ಯದಿ (set_topbot)
    {
	used = 0;
	linenr_T cln_last = cln;
#ifdef FEAT_FOLDING
	(ಶೂನ್ಯ)hasFolding(cln, ರಿಕ್ತ, &cln_last);
#endif
	curwin->w_botline = cln_last + 1;
	loff.lnum = cln_last + 1;
#ifdef FEAT_DIFF
	loff.fill = 0;
#endif
	ಯಾವತ್ (TRUE)
	{
	    topline_back_winheight(&loff, FALSE);
	    ಯದಿ (loff.height == MAXCOL)
		ಭಙ್ಗ;
	    ಯದಿ (used + loff.height > curwin->w_height)
	    {
		ಯದಿ (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    ಯದಿ (used < curwin->w_height)
		    {
			ಪೂರ್ಣಾಙ್ಕ plines_offset = used + loff.height
							    - curwin->w_height;
			used = curwin->w_height;
#ifdef FEAT_DIFF
			curwin->w_topfill = loff.fill;
#endif
			curwin->w_topline = loff.lnum;
			curwin->w_skipcol = skipcol_from_plines(
							curwin, plines_offset);
		    }
		}
		ಭಙ್ಗ;
	    }
#ifdef FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#endif
	    curwin->w_topline = loff.lnum;
	    used += loff.height;
	}

	set_empty_rows(curwin, used);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	ಯದಿ (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#endif
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    ಯದಿ (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    ಅನ್ಯಥಾ
		reset_skipcol();
	}
    }
    ಅನ್ಯಥಾ
	validate_botline();

    // The lines of the cursor line itself are always used.
#ifdef FEAT_DIFF
    used = plines_nofill(cln);
#ಅನ್ಯಥಾ
    validate_cheight();
    used = curwin->w_cline_height;
#endif

    ಯದಿ (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	ಪೂರ್ಣಾಙ್ಕ upto_cursor = plines_win_col(curwin, cln, curwin->w_cursor.col);

	ಯದಿ (upto_cursor < used)
	    used = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    ಯದಿ (cln >= curwin->w_botline)
    {
	scrolled = used;
	ಯದಿ (cln == curwin->w_botline)
	    scrolled -= curwin->w_empty_rows;
	ಯದಿ (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    ಪೂರ್ಣಾಙ್ಕ ತುದಿ_plines =
#ifdef FEAT_DIFF
			    plines_win_nofill
#ಅನ್ಯಥಾ
			    plines_win
#endif
					(curwin, curwin->w_topline, FALSE);
	    ಪೂರ್ಣಾಙ್ಕ width1 = curwin->w_width - curwin_col_off();

	    ಯದಿ (width1 > 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ width2 = width1 + curwin_col_off2();
		ಪೂರ್ಣಾಙ್ಕ skip_ಸಾಲುಗಳು = 0;

		// A similar formula is used in curs_columns().
		ಯದಿ (curwin->w_skipcol > width1)
		    skip_ಸಾಲುಗಳು += (curwin->w_skipcol - width1) / width2 + 1;
		ಅನ್ಯಥಾ ಯದಿ (curwin->w_skipcol > 0)
		    skip_ಸಾಲುಗಳು = 1;

		ತುದಿ_plines -= skip_ಸಾಲುಗಳು;
		ಯದಿ (ತುದಿ_plines > curwin->w_height)
		{
		    scrolled += (ತುದಿ_plines - curwin->w_height);
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
#ifdef FEAT_FOLDING
    ಯದಿ (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
#endif
    {
	loff.lnum = cln;
	boff.lnum = cln;
    }
#ifdef FEAT_DIFF
    loff.fill = 0;
    boff.fill = 0;
    fill_below_window = diff_check_fill(curwin, curwin->w_botline)
						      - curwin->w_filler_rows;
#endif

    ಯಾವತ್ (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	ಯದಿ ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		&& loff.lnum <= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_below_window)
#endif
		)
	    ಭಙ್ಗ;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_back(&loff);
	ಯದಿ (loff.height == MAXCOL)
	    used = MAXCOL;
	ಅನ್ಯಥಾ
	    used += loff.height;
	ಯದಿ (used > curwin->w_height)
	    ಭಙ್ಗ;
	ಯದಿ (loff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_below_window)
#endif
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.height;
	    ಯದಿ (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_empty_rows;
	}

	ಯದಿ (boff.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    used += boff.height;
	    ಯದಿ (used > curwin->w_height)
		ಭಙ್ಗ;
	    ಯದಿ (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.height;
		ಯದಿ (boff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_rows)
#endif
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.height;
		    ಯದಿ (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_empty_rows;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    ಯದಿ (scrolled <= 0)
	ಸಾಲು_ಎಣಿಕೆ = 0;
    // more than a screenfull, don't scroll but redraw
    ಅನ್ಯಥಾ ಯದಿ (used > curwin->w_height)
	ಸಾಲು_ಎಣಿಕೆ = used;
    // scroll minimal number of lines
    ಅನ್ಯಥಾ
    {
	ಸಾಲು_ಎಣಿಕೆ = 0;
#ifdef FEAT_DIFF
	boff.fill = curwin->w_topfill;
#endif
	boff.lnum = curwin->w_topline - 1;
	ಪ್ರತಿ (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.height;
	    ++ಸಾಲು_ಎಣಿಕೆ;
	}
	ಯದಿ (i < scrolled)	// below curwin->w_botline, don't scroll
	    ಸಾಲು_ಎಣಿಕೆ = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    ಯದಿ (ಸಾಲು_ಎಣಿಕೆ >= curwin->w_height && ಸಾಲು_ಎಣಿಕೆ > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    ಅನ್ಯಥಾ ಯದಿ (ಸಾಲು_ಎಣಿಕೆ > 0)
    {
	ಯದಿ (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	ಅನ್ಯಥಾ
	    scrollup(ಸಾಲು_ಎಣಿಕೆ, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    ಯದಿ (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && set_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_empty_rows = old_empty_rows;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    ಯದಿ (set_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    ಶೂನ್ಯ
scroll_cursor_halfway(ಪೂರ್ಣಾಙ್ಕ atend, ಪೂರ್ಣಾಙ್ಕ prefer_above)
{
    ಪೂರ್ಣಾಙ್ಕ		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		topfill = 0;
#endif
    ಪೂರ್ಣಾಙ್ಕ		below = 0;
    ಪೂರ್ಣಾಙ್ಕ		used;
    lineoff_T	loff;
    lineoff_T	boff;
#ifdef FEAT_DIFF
    linenr_T	old_topline = curwin->w_topline;
#endif

#ifdef FEAT_PROP_POPUP
    // if the width changed this needs to be updated first
    may_update_popup_position();
#endif
    loff.lnum = boff.lnum = curwin->w_cursor.lnum;
#ifdef FEAT_FOLDING
    (ಶೂನ್ಯ)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#endif
#ifdef FEAT_DIFF
    used = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#ಅನ್ಯಥಾ
    used = plines(loff.lnum);
#endif
    topline = loff.lnum;

    ಪೂರ್ಣಾಙ್ಕ want_height;
    ಪೂರ್ಣಾಙ್ಕ do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ಯದಿ (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	ಯದಿ (atend)
	{
	    want_height = (curwin->w_height - used) / 2;
	    used = 0;
	}
	ಅನ್ಯಥಾ
	    want_height = curwin->w_height;
    }

    ಯಾವತ್ (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	ಯದಿ (do_sms)
	{
	    topline_back_winheight(&loff, FALSE);
	    ಯದಿ (loff.height == MAXCOL)
		ಭಙ್ಗ;
	    used += loff.height;
	    ಯದಿ (!atend && boff.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		botline_forw(&boff);
		used += boff.height;
	    }
	    ಯದಿ (used > want_height)
	    {
		ಯದಿ (used - loff.height < want_height)
		{
		    topline = loff.lnum;
#ifdef FEAT_DIFF
		    topfill = loff.fill;
#endif
		    skipcol = skipcol_from_plines(curwin, used - want_height);
		}
		ಭಙ್ಗ;
	    }
	    topline = loff.lnum;
#ifdef FEAT_DIFF
	    topfill = loff.fill;
#endif
	    ಅನುವರ್ತ;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	ಪೂರ್ಣಾಙ್ಕ done = FALSE;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ round = 1; round <= 2; ++round)
	{
	    ಯದಿ (prefer_above ? (round == 2 && below < above)
			     : (round == 1 && below <= above))
	    {
		// add a line below the cursor
		ಯದಿ (boff.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		{
		    botline_forw(&boff);
		    used += boff.height;
		    ಯದಿ (used > curwin->w_height)
		    {
			done = TRUE;
			ಭಙ್ಗ;
		    }
		    below += boff.height;
		}
		ಅನ್ಯಥಾ
		{
		    ++below;	    // count a "~" line
		    ಯದಿ (atend)
			++used;
		}
	    }

	    ಯದಿ (prefer_above ? (round == 1 && below >= above)
			     : (round == 1 && below > above))
	    {
		// add a line above the cursor
		topline_back(&loff);
		ಯದಿ (loff.height == MAXCOL)
		    used = MAXCOL;
		ಅನ್ಯಥಾ
		    used += loff.height;
		ಯದಿ (used > curwin->w_height)
		{
		    done = TRUE;
		    ಭಙ್ಗ;
		}
		above += loff.height;
		topline = loff.lnum;
#ifdef FEAT_DIFF
		topfill = loff.fill;
#endif
	    }
	}
	ಯದಿ (done)
	    ಭಙ್ಗ;
    }

#ifdef FEAT_FOLDING
    ಯದಿ (!hasFolding(topline, &curwin->w_topline, ರಿಕ್ತ))
#endif
    {
	ಯದಿ (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    ಯದಿ (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (do_sms)
		reset_skipcol();
	}
    }
#ifdef FEAT_DIFF
    curwin->w_topfill = topfill;
    ಯದಿ (old_topline > curwin->w_topline + curwin->w_height)
	curwin->w_botfill = false;
    check_topfill(curwin, FALSE);
#endif
    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
    curwin->w_valid |= VALID_TOPLINE;
}

/*
 * Correct the cursor position so that it is in a part of the screen at least
 * 'scrolloff' lines from the top and bottom, if possible.
 * If not possible, put it at the same position as scroll_cursor_halfway().
 * When called topline must be valid!
 */
    ಶೂನ್ಯ
cursor_correct(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		above = 0;	    // screen lines above topline
    linenr_T	topline;
    ಪೂರ್ಣಾಙ್ಕ		below = 0;	    // screen lines below botline
    linenr_T	botline;
    ಪೂರ್ಣಾಙ್ಕ		above_wanted, below_wanted;
    linenr_T	cln;		    // Cursor Line Number
    ಪೂರ್ಣಾಙ್ಕ		max_off;
    long	so = get_scrolloff_ಮೌಲ್ಯ();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    below_wanted = so;
    ಯದಿ (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	below_wanted = mouse_dragging - 1;
    }
    ಯದಿ (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_height / 2;
	ಯದಿ (below_wanted > max_off)
	    below_wanted = max_off;
    }
    validate_botline();
    ಯದಿ (curwin->w_botline == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	below_wanted = 0;
	max_off = (curwin->w_height - 1) / 2;
	ಯದಿ (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    ಯದಿ (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - below_wanted
#ifdef FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#endif
	    )
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * Narrow down the area where the cursor can be put by taking lines from
     * the top and the bottom until:
     * - the desired context lines are found
     * - the lines from the top is past the lines from the bottom
     */
    topline = curwin->w_topline;
    botline = curwin->w_botline - 1;
#ifdef FEAT_DIFF
    // count filler lines as context
    above = curwin->w_topfill;
    below = curwin->w_filler_rows;
#endif
    ಯಾವತ್ ((above < above_wanted || below < below_wanted) && topline < botline)
    {
	ಯದಿ (below < below_wanted && (below <= above || above >= above_wanted))
	{
#ifdef FEAT_FOLDING
	    ಯದಿ (hasFolding(botline, &botline, ರಿಕ್ತ))
		++below;
	    ಅನ್ಯಥಾ
#endif
		below += plines(botline);
	    --botline;
	}
	ಯದಿ (above < above_wanted && (above < below || below >= below_wanted))
	{
#ifdef FEAT_FOLDING
	    ಯದಿ (hasFolding(topline, ರಿಕ್ತ, &topline))
		++above;
	    ಅನ್ಯಥಾ
#endif
		above += PLINES_NOFILL(topline);
#ifdef FEAT_DIFF
	    // Count filler lines below this line as context.
	    ಯದಿ (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#endif
	    ++topline;
	}
    }
    ಯದಿ (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    ಅನ್ಯಥಾ ಯದಿ (topline > botline)
	curwin->w_cursor.lnum = botline;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	ಯದಿ (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_scroll_overlap(ಪೂರ್ಣಾಙ್ಕ dir)
{
    lineoff_T loff;
    ಪೂರ್ಣಾಙ್ಕ	    min_height = curwin->w_height - 2;

    validate_botline();
    ಯದಿ ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
	ಪ್ರತ್ಯಾಗಮ min_height + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#ifdef FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_rows : curwin->w_topfill);
    loff.height = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#ಅನ್ಯಥಾ
    loff.height = plines(loff.lnum);
#endif

    ಪೂರ್ಣಾಙ್ಕ h1 = loff.height;
    ಯದಿ (h1 > min_height)
	ಪ್ರತ್ಯಾಗಮ min_height + 2;  // no overlap
    ಯದಿ (dir == FORWARD)
	topline_back(&loff);
    ಅನ್ಯಥಾ
	botline_forw(&loff);

    ಪೂರ್ಣಾಙ್ಕ h2 = loff.height;
    ಯದಿ (h2 == MAXCOL || h2 + h1 > min_height)
	ಪ್ರತ್ಯಾಗಮ min_height + 2;  // no overlap
    ಯದಿ (dir == FORWARD)
	topline_back(&loff);
    ಅನ್ಯಥಾ
	botline_forw(&loff);

    ಪೂರ್ಣಾಙ್ಕ h3 = loff.height;
    ಯದಿ (h3 == MAXCOL || h3 + h2 > min_height)
	ಪ್ರತ್ಯಾಗಮ min_height + 2;  // no overlap
    ಯದಿ (dir == FORWARD)
	topline_back(&loff);
    ಅನ್ಯಥಾ
	botline_forw(&loff);

    ಪೂರ್ಣಾಙ್ಕ h4 = loff.height;
    ಯದಿ (h4 == MAXCOL || h4 + h3 + h2 > min_height || h3 + h2 + h1 > min_height)
	ಪ್ರತ್ಯಾಗಮ min_height + 1;  // 1 line overlap
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ min_height;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ scroll_with_sms(ಪೂರ್ಣಾಙ್ಕ dir, long ಎಣಿಕೆ, long *curscount)
{
    ಪೂರ್ಣಾಙ್ಕ		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		prev_topfill = curwin->w_topfill;
#endif

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, ಎಣಿಕೆ);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    ಯದಿ (!prev_sms && curwin->w_skipcol > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	ಯದಿ (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	ಪೂರ್ಣಾಙ್ಕ width1 = curwin->w_width - curwin_col_off();
	ಪೂರ್ಣಾಙ್ಕ width2 = width1 + curwin_col_off2();
	ಎಣಿಕೆ = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	ಯದಿ (fixdir == FORWARD)
	    ಎಣಿಕೆ = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, ಎಣಿಕೆ);
	*curscount += ಎಣಿಕೆ * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    ಪ್ರತ್ಯಾಗಮ curwin->w_topline != prev_topline
#ifdef FEAT_DIFF
	|| curwin->w_topfill != prev_topfill
#endif
	|| curwin->w_skipcol != prev_skipcol;
}

/*
 * Move screen "count" (half) pages up ("dir" is BACKWARD) or down ("dir" is
 * FORWARD) and update the screen. Handle moving the cursor and not scrolling
 * to reveal end of buffer lines for half-page scrolling with CTRL-D and CTRL-U.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
pagescroll(ಪೂರ್ಣಾಙ್ಕ dir, long ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ half)
{
    ಪೂರ್ಣಾಙ್ಕ		did_move = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		buflen = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    colnr_T	prev_col = curwin->w_cursor.col;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    ಯದಿ (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	ಯದಿ (ಎಣಿಕೆ)
	    curwin->w_p_scr = MIN(curwin->w_height, ಎಣಿಕೆ);
	ಎಣಿಕೆ = MIN(curwin->w_height, curwin->w_p_scr);

	long curscount = ಎಣಿಕೆ;
	// Adjust count so as to not reveal end of buffer lines.
	ಯದಿ (dir == FORWARD
		    && (curwin->w_topline + curwin->w_height + ಎಣಿಕೆ > buflen
#ifdef FEAT_FOLDING
							|| hasAnyFolding(curwin)
#endif
	   ))
	{
	    ಪೂರ್ಣಾಙ್ಕ n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    ಯದಿ (n - ಎಣಿಕೆ < curwin->w_height && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_height + ಎಣಿಕೆ);
	    ಯದಿ (n < curwin->w_height + ಎಣಿಕೆ)
		ಎಣಿಕೆ = n - curwin->w_height;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	ಯದಿ (ಎಣಿಕೆ > 0)
	{
	    did_move = scroll_with_sms(dir, ಎಣಿಕೆ, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.col = prev_col;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	ಯದಿ (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	ಅನ್ಯಥಾ ಯದಿ (dir == FORWARD)
	    cursor_down_inner(curwin, curscount);
	ಅನ್ಯಥಾ
	    cursor_up_inner(curwin, curscount);
    }
    ಅನ್ಯಥಾ
    {
	// Scroll [count] times 'window' or current window height lines.
	ಎಣಿಕೆ *= ((ONE_WINDOW && p_window > 0 && p_window < Rows - 1) ?
				MAX(1, p_window - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, ಎಣಿಕೆ, &ಎಣಿಕೆ);

	ಯದಿ (did_move)
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

    ಯದಿ (get_scrolloff_ಮೌಲ್ಯ() > 0)
	cursor_correct();
#ifdef FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#endif
    did_move = did_move
	|| prev_col != curwin->w_cursor.col
	|| prev_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    ಯದಿ (!did_move)
	beep_flush();
    ಅನ್ಯಥಾ ಯದಿ (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    ಅನ್ಯಥಾ ಯದಿ (p_sol)
	nv_g_home_m_cmd(&ca);

    ಪ್ರತ್ಯಾಗಮ did_move ? OK : FAIL;
}

    ಶೂನ್ಯ
do_check_cursorbind(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ win_T	*prev_curwin = ರಿಕ್ತ;
    ಸ್ಥಿರ buf_T	*prev_curbuf = ರಿಕ್ತ;
    ಸ್ಥಿರ varnumber_T	prev_changedtick = 0;
    ಸ್ಥಿರ pos_T	prev_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    ಯದಿ (curwin == prev_curwin && curbuf == prev_curbuf
	    && CHANGEDTICK(curbuf) == prev_changedtick
	    && EQUAL_POS(curwin->w_cursor, prev_cursor))
	ಪ್ರತ್ಯಾಗಮ;
    prev_curwin = curwin;
    prev_curbuf = curbuf;
    prev_changedtick = CHANGEDTICK(curbuf);
    prev_cursor = curwin->w_cursor;

    linenr_T	ಸಾಲು = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    ಪೂರ್ಣಾಙ್ಕ		set_curswant = curwin->w_set_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    ಪೂರ್ಣಾಙ್ಕ		old_VIsual_select = VIsual_select;
    ಪೂರ್ಣಾಙ್ಕ		old_VIsual_active = VIsual_active;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_active = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_buffer;
	// skip original window and windows with 'nocursorbind'
	ಯದಿ (curwin != old_curwin && curwin->w_p_crb)
	{
#ifdef FEAT_DIFF
	    ಯದಿ (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_ಸಾಲು(old_curbuf, ಸಾಲು);
	    ಅನ್ಯಥಾ
#endif
		curwin->w_cursor.lnum = ಸಾಲು;
	    curwin->w_cursor.col = col;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_set_curswant = set_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    ಪೂರ್ಣಾಙ್ಕ restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    ಯದಿ (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    ಯದಿ (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    ಯದಿ (!curwin->w_p_scb)
		update_topline();
	    curwin->w_redr_status = true;
	}
    }

    /*
     * reset current-window
     */
    VIsual_select = old_VIsual_select;
    VIsual_active = old_VIsual_active;
    curwin = old_curwin;
    curbuf = old_curbuf;
}
