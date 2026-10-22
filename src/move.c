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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ scrolljump_ಮೌಲ್ಯ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_ತುದಿ_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ curs_ಅಡ್ಡಸಾಲುಗಳು(win_T *wp);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    linenr_T	    lnum;	// line number
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		    fill;	// filler lines
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		    ಎತ್ತರ;	// height of added line
} lineoff_T;

ಸ್ಥಿರ ಶೂನ್ಯ topline_ಹಿಂಭಾಗ(lineoff_T *lp);
ಸ್ಥಿರ ಶೂನ್ಯ botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
adjust_plines_for_skipcol(win_T *wp)
{
    ಆದರೆ (wp->w_skipcol == 0)
	ಹಿಂತಿರುಗಿ 0;

    ಪೂರ್ಣಾಂಕ ಅಗಲ = wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp);
    ಪೂರ್ಣಾಂಕ w2 = ಅಗಲ + win_ಲಂಬಸಾಲು_off2(wp);
    ಆದರೆ (wp->w_skipcol >= ಅಗಲ && w2 > 0)
	ಹಿಂತಿರುಗಿ (wp->w_skipcol - ಅಗಲ) / w2 + 1;

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    ಪೂರ್ಣಾಂಕ
plines_correct_topline(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಂಕ ಎಲ್ಲೆ_winheight)
{
    ಪೂರ್ಣಾಂಕ n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	n = plines_win(wp, lnum, FALSE);
    ಆದರೆ (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    ಆದರೆ (ಎಲ್ಲೆ_winheight && n > wp->w_ಎತ್ತರ)
	n = wp->w_ಎತ್ತರ;
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
comp_botline(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ		n;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ;
    ಪೂರ್ಣಾಂಕ		i = 0;
    ಪೂರ್ಣಾಂಕ		use_cache;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    linenr_T    ಕೊನೆಯ;
    ಪೂರ್ಣಾಂಕ		folded;
#ಆದರೆ_ಅಂತ್ಯ

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
		    && !wp->w_ಬಫರ್->b_mod_ಗಣ
		    && dollar_vcol == -1
		    && wp->w_skipcol == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    && !wp->w_p_diff
#ಆದರೆ_ಅಂತ್ಯ
		    && wp->w_ಸಾಲುಗಳು_valid > 0
		    && wp->w_ಸಾಲುಗಳು[0].wl_lnum <= wp->w_topline;

    ಆದರೆ (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	ಮುಗಿದ = wp->w_cline_ಅಡ್ಡಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	lnum = wp->w_topline;
	ಮುಗಿದ = 0;
    }

    // Find the w_lines[] entry for the starting line.
    ಆದರೆ (use_cache)
	ಆಗ (i < wp->w_ಸಾಲುಗಳು_valid && wp->w_ಸಾಲುಗಳು[i].wl_lnum < lnum)
	    ++i;

    ಗಾಗಿ ( ; lnum <= wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++i)
    {
	ಪೂರ್ಣಾಂಕ	valid = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಕೊನೆಯ = lnum;
	folded = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
	// Try to use the size from the previous redraw.
	ಆದರೆ (use_cache && i < wp->w_ಸಾಲುಗಳು_valid)
	{
	    ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lnum < lnum || !wp->w_ಸಾಲುಗಳು[i].wl_valid)
		ಮುಂದುವರಿ;		// skip changed or deleted lines
	    ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lnum == lnum)
		valid = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	ಆದರೆ (valid)
	{
	    n = wp->w_ಸಾಲುಗಳು[i].wl_ಗಾತ್ರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    folded = wp->w_ಸಾಲುಗಳು[i].wl_folded;
	    ಕೊನೆಯ = wp->w_ಸಾಲುಗಳು[i].wl_lastlnum;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (hasFoldingWin(wp, lnum, ಖಾಲಿ, &ಕೊನೆಯ, TRUE, ಖಾಲಿ))
	    {
		n = 1;
		folded = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && ಕೊನೆಯ >= wp->w_cursor.lnum
#ಇಲ್ಲದಿದ್ದರೆ
		lnum == wp->w_cursor.lnum
#ಆದರೆ_ಅಂತ್ಯ
	   )
	{
	    wp->w_cline_ಅಡ್ಡಸಾಲು = ಮುಗಿದ;
	    wp->w_cline_ಎತ್ತರ = n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    wp->w_cline_folded = folded;
#ಆದರೆ_ಅಂತ್ಯ
	    redraw_for_cursorline(wp);
	    wp->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	ಆದರೆ (ಮುಗಿದ + n > wp->w_ಎತ್ತರ)
	    ನಿಲ್ಲಿಸು;
	ಮುಗಿದ += n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	lnum = ಕೊನೆಯ + 1;
#ಇಲ್ಲದಿದ್ದರೆ
	++lnum;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // wp->w_botline is the line that is just below the window
    wp->w_botline = lnum;
    wp->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    ಗಣ_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು(wp, ಮುಗಿದ);
}

/*
 * Redraw when w_cline_row changes and 'relativenumber' or 'cursorline' is
 * set.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_for_cursorline(win_T *wp)
{
    ಆದರೆ ((wp->w_p_rnu
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		|| wp->w_p_cul
#ಆದರೆ_ಅಂತ್ಯ
		)
	    && (wp->w_valid & VALID_CROW) == 0
	    && !pum_visible())
    {
	// win_line() will redraw the number column and cursorline only.
	redraw_win_later(wp, UPD_VALID);
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
/*
 * Redraw when w_virtcol changes and 'cursorcolumn' is set or 'cursorlineopt'
 * contains "screenline".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_for_cursorcolumn(win_T *wp)
{
    ಆದರೆ ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	ಆದರೆ (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    ಶೂನ್ಯ
ಗಣ_valid_virtcol(win_T *wp, colnr_T vcol)
{
    wp->w_virtcol = vcol;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#ಆದರೆ_ಅಂತ್ಯ
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
    ಪೂರ್ಣಾಂಕ
sms_marker_overlap(win_T *wp, ಪೂರ್ಣಾಂಕ extra2)
{
    ಆದರೆ (extra2 == -1)
	extra2 = win_ಲಂಬಸಾಲು_off(wp) - win_ಲಂಬಸಾಲು_off2(wp);
#ಆದರೆ defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    ಆದರೆ (*get_showbreak_ಮೌಲ್ಯ(wp) != NUL)
	ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    ಆದರೆ (wp->w_p_list && wp->w_lcs_chars.prec)
	ಹಿಂತಿರುಗಿ 1;

    ಹಿಂತಿರುಗಿ extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
skipcol_from_plines(win_T *wp, ಪೂರ್ಣಾಂಕ plines_off)
{
    ಪೂರ್ಣಾಂಕ width1 = wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp);

    ಪೂರ್ಣಾಂಕ skipcol = 0;
    ಆದರೆ (plines_off > 0)
	skipcol += width1;
    ಆದರೆ (plines_off > 1)
	skipcol += (width1 + win_ಲಂಬಸಾಲು_off2(wp)) * (plines_off - 1);
    ಹಿಂತಿರುಗಿ skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reset_skipcol(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_skipcol == 0)
	ಹಿಂತಿರುಗಿ;

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
    ಆದರೆ (must_redraw)
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
    ಹಿಂತಿರುಗಿ get_scrolloff_ಮೌಲ್ಯ() > 0 && get_scrolloffpad_ಮೌಲ್ಯ() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    ಸ್ಥಿರ bool
scrolloffpad_eof_pressure(linenr_T lnum, ದೀರ್ಘ so)
{
    ಆದರೆ (!use_scrolloffpad() || so <= 0)
	ಹಿಂತಿರುಗಿ false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    ಹಿಂತಿರುಗಿ lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    ಶೂನ್ಯ
update_topline(ಶೂನ್ಯ)
{
    ದೀರ್ಘ	ಸಾಲು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		halfheight;
    ಪೂರ್ಣಾಂಕ		n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    linenr_T	lnum;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		check_topline = FALSE;
    ಪೂರ್ಣಾಂಕ		check_botline = FALSE;
    ದೀರ್ಘ	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    ಪೂರ್ಣಾಂಕ		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    ಆದರೆ (skip_update_topline)
	ಹಿಂತಿರುಗಿ;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    ಆದರೆ (!screen_valid(TRUE) || curwin->w_ಎತ್ತರ == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	ಹಿಂತಿರುಗಿ;
    }

    check_cursor_moved(curwin);
    ಆದರೆ (curwin->w_valid & VALID_TOPLINE)
	ಹಿಂತಿರುಗಿ;

    // When dragging with the mouse, don't scroll that quickly
    ಆದರೆ (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ old_topfill = curwin->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * If the buffer is empty, always set topline to 1.
     */
    ಆದರೆ (BUFEMPTY())		// special case - file is empty
    {
	ಆದರೆ (curwin->w_topline != 1)
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
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    ಆದರೆ (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_ತುದಿ_ಸ್ಥಾನಾಂತರ())
		check_topline = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		ಪೂರ್ಣಾಂಕ overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, ಖಾಲಿ, ಖಾಲಿ, 0);
		overlap = sms_marker_overlap(curwin, -1);
		ಆದರೆ (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	ಆದರೆ (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (check_topline)
	{
	    halfheight = curwin->w_ಎತ್ತರ / 2 - 1;
	    ಆದರೆ (halfheight < 2)
		halfheight = 2;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		ಗಾಗಿ (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    ಆದರೆ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || n >= halfheight)
			ನಿಲ್ಲಿಸು;
		    (ಶೂನ್ಯ)hasFolding(lnum, ಖಾಲಿ, &lnum);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    ಪೂರ್ಣಾಂಕ min_scroll = scrolljump_ಮೌಲ್ಯ();
	    ಆದರೆ (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		scroll_cursor_ತುದಿ(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
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
    ಆದರೆ (check_botline)
    {
	ಆದರೆ (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	ಆದರೆ (curwin->w_botline <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || use_scrolloffpad())
	{
	    ಆದರೆ (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		ಆದರೆ (((ದೀರ್ಘ)curwin->w_cursor.lnum
					   >= (ದೀರ್ಘ)curwin->w_botline - *so_ptr
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			|| hasAnyFolding(curwin)
#ಆದರೆ_ಅಂತ್ಯ
			))
		{
		    lineoff_T	loff;

		    // Cursor is (a few lines) above botline, check if there
		    // are 'scrolloff' window lines below the cursor.
		    // If not, need to scroll.
		    n = eof_pressure ? 0 : curwin->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
		    loff.lnum = curwin->w_cursor.lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		    // In a fold go to its last line.
		    (ಶೂನ್ಯ)hasFolding(loff.lnum, ಖಾಲಿ, &loff.lnum);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_ಅಡ್ಡಸಾಲುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
		    loff.ಎತ್ತರ = 0;
		    ಆಗ (loff.lnum < curwin->w_botline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#ಆದರೆ_ಅಂತ್ಯ
			    )
		    {
			n += loff.ಎತ್ತರ;
			ಆದರೆ (n >= *so_ptr)
			    ನಿಲ್ಲಿಸು;
			botline_forw(&loff);
		    }
		    ಆದರೆ (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    ಆದರೆ (check_botline)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		ಆದರೆ (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    ಸಾಲು_ಎಣಿಕೆ = 0;
		    ಗಾಗಿ (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++ಸಾಲು_ಎಣಿಕೆ;
			// stop at end of file or when we know we are far off
			ಆದರೆ (lnum <= 0 || ಸಾಲು_ಎಣಿಕೆ > curwin->w_ಎತ್ತರ + 1)
			    ನಿಲ್ಲಿಸು;
			(ಶೂನ್ಯ)hasFolding(lnum, &lnum, ಖಾಲಿ);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    ಸಾಲು_ಎಣಿಕೆ = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ <= curwin->w_ಎತ್ತರ + 1)
		{
		    ಆದರೆ (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    ಇಲ್ಲದಿದ್ದರೆ
			scroll_cursor_bot(scrolljump_ಮೌಲ್ಯ(), FALSE);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    ಆದರೆ (curwin->w_topline != old_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	ಆದರೆ (!curwin->w_p_sms)
	    reset_skipcol();
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	ಆದರೆ (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
scrolljump_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಆದರೆ (p_sj >= 0)
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)p_sj;
    ಹಿಂತಿರುಗಿ (curwin->w_ಎತ್ತರ * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_ತುದಿ_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ)
{
    lineoff_T	loff;
    ಪೂರ್ಣಾಂಕ		n;
    ದೀರ್ಘ	so = get_scrolloff_ಮೌಲ್ಯ();

    ಆದರೆ (curwin->w_cursor.lnum < curwin->w_topline + so
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		    || hasAnyFolding(curwin)
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#ಇಲ್ಲದಿದ್ದರೆ
	n = 0;
#ಆದರೆ_ಅಂತ್ಯ
	// Count the visible screen lines above the cursor line.
	ಆಗ (n < so)
	{
	    topline_ಹಿಂಭಾಗ(&loff);
	    // Stop when included a line above the window.
	    ಆದರೆ (loff.lnum < curwin->w_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#ಆದರೆ_ಅಂತ್ಯ
		    )
		ನಿಲ್ಲಿಸು;
	    n += loff.ಎತ್ತರ;
	}
	ಆದರೆ (n < so)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Update w_curswant.
 */
    ಶೂನ್ಯ
update_curswant_force(ಶೂನ್ಯ)
{
    validate_virtcol();
    curwin->w_curswant = curwin->w_virtcol
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	- curwin->w_virtcol_ಮೊದಲ_char
#ಆದರೆ_ಅಂತ್ಯ
	;
    curwin->w_ಗಣ_curswant = false;
}

/*
 * Update w_curswant if w_set_curswant is set.
 */
    ಶೂನ್ಯ
update_curswant(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_ಗಣ_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    ಶೂನ್ಯ
check_cursor_moved(win_T *wp)
{
    ಆದರೆ (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_cursor.ಲಂಬಸಾಲು != wp->w_valid_cursor.ಲಂಬಸಾಲು
	     || wp->w_leftcol != wp->w_valid_leftcol
	     || wp->w_cursor.coladd != wp->w_valid_cursor.coladd)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
	wp->w_valid_cursor.ಲಂಬಸಾಲು = wp->w_cursor.ಲಂಬಸಾಲು;
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
ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting(ಶೂನ್ಯ)
{
    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_win(curwin);
}

    ಶೂನ್ಯ
ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_win(win_T *wp)
{
    wp->w_ಸಾಲುಗಳು_valid = 0;
    ಬದಲಾಯಿತು_ಸಾಲು_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    ಶೂನ್ಯ
ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಆದರೆ (wp->w_ಬಫರ್ == buf)
	    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_win(wp);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Call changed_window_setting_win() for every window.
 */
    ಶೂನ್ಯ
ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_all(ಶೂನ್ಯ)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    ಶೂನ್ಯ
ಗಣ_topline(win_T *wp, linenr_T lnum)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // go to first of folded lines
    (ಶೂನ್ಯ)hasFoldingWin(wp, lnum, &lnum, ಖಾಲಿ, TRUE, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    ಆದರೆ (wp->w_botline > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	wp->w_botline = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_ಗಣ = true;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (lnum != prev_topline)
	// Keep the filler lines when the topline didn't change.
	wp->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
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
ಬದಲಾಯಿತು_cline_bef_curs(ಶೂನ್ಯ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    ಶೂನ್ಯ
ಬದಲಾಯಿತು_cline_bef_curs_win(win_T *wp)
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
ಬದಲಾಯಿತು_ಸಾಲು_abv_curs(ಶೂನ್ಯ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    ಶೂನ್ಯ
ಬದಲಾಯಿತು_ಸಾಲು_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    ಶೂನ್ಯ
ಬದಲಾಯಿತು_ಸಾಲು_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_ಬಫರ್ == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಆದರೆ (!(wp->w_valid & VALID_BOTLINE))
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
    ಪೂರ್ಣಾಂಕ
cursor_valid(ಶೂನ್ಯ)
{
    check_cursor_moved(curwin);
    ಹಿಂತಿರುಗಿ ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
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
    ಆದರೆ ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#ಆದರೆ defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    ಶೂನ್ಯ
validate_cline_ಅಡ್ಡಸಾಲು(ಶೂನ್ಯ)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    ಆದರೆ (!(curwin->w_valid & VALID_CROW))
	curs_ಅಡ್ಡಸಾಲುಗಳು(curwin);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
curs_ಅಡ್ಡಸಾಲುಗಳು(win_T *wp)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		all_invalid;
    ಪೂರ್ಣಾಂಕ		valid;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ದೀರ್ಘ	fold_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_ಸಾಲುಗಳು_valid == 0
			|| wp->w_ಸಾಲುಗಳು[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_ಅಡ್ಡಸಾಲು = 0;
    ಗಾಗಿ (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	ಆದರೆ (!all_invalid && i < wp->w_ಸಾಲುಗಳು_valid)
	{
	    ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lnum < lnum || !wp->w_ಸಾಲುಗಳು[i].wl_valid)
		ಮುಂದುವರಿ;		// skip changed or deleted lines
	    ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lnum == lnum)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		ಆದರೆ (!wp->w_ಬಫರ್->b_mod_ಗಣ
			|| wp->w_ಸಾಲುಗಳು[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_ಬಫರ್->b_mod_ತುದಿ
					     > wp->w_ಸಾಲುಗಳು[i].wl_lastlnum + 1)
#ಆದರೆ_ಅಂತ್ಯ
		valid = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	ಆದರೆ (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
					    && !wp->w_p_diff
#ಆದರೆ_ಅಂತ್ಯ
	       )))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    lnum = wp->w_ಸಾಲುಗಳು[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    ಆದರೆ (lnum > wp->w_cursor.lnum)
		ನಿಲ್ಲಿಸು;
#ಇಲ್ಲದಿದ್ದರೆ
	    ++lnum;
#ಆದರೆ_ಅಂತ್ಯ
	    wp->w_cline_ಅಡ್ಡಸಾಲು += wp->w_ಸಾಲುಗಳು[i].wl_ಗಾತ್ರ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    fold_ಎಣಿಕೆ = folded_ಎಣಿಕೆ(wp, lnum, ಖಾಲಿ);
	    ಆದರೆ (fold_ಎಣಿಕೆ)
	    {
		lnum += fold_ಎಣಿಕೆ;
		ಆದರೆ (lnum > wp->w_cursor.lnum)
		    ನಿಲ್ಲಿಸು;
		++wp->w_cline_ಅಡ್ಡಸಾಲು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		wp->w_cline_ಅಡ್ಡಸಾಲು += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    ಆದರೆ (!(wp->w_valid & VALID_CHEIGHT))
    {
	ಆದರೆ (all_invalid
		|| i == wp->w_ಸಾಲುಗಳು_valid
		|| (i < wp->w_ಸಾಲುಗಳು_valid
		    && (!wp->w_ಸಾಲುಗಳು[i].wl_valid
			|| wp->w_ಸಾಲುಗಳು[i].wl_lnum != wp->w_cursor.lnum)))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    ಆದರೆ (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_ಎತ್ತರ = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		wp->w_cline_ಎತ್ತರ = plines_win(wp, wp->w_cursor.lnum, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ಖಾಲಿ, ಖಾಲಿ, TRUE, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i > wp->w_ಸಾಲುಗಳು_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_ಎತ್ತರ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ಖಾಲಿ, ಖಾಲಿ, TRUE, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp->w_cline_ಎತ್ತರ = wp->w_ಸಾಲುಗಳು[i].wl_ಗಾತ್ರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    wp->w_cline_folded = wp->w_ಸಾಲುಗಳು[i].wl_folded;
#ಆದರೆ_ಅಂತ್ಯ
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

    ಆದರೆ (wp->w_valid & VALID_VIRTCOL)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    wp->w_virtcol_ಮೊದಲ_char = 0;
#ಆದರೆ_ಅಂತ್ಯ
    getvvcol(wp, &wp->w_cursor, ಖಾಲಿ, &(wp->w_virtcol), ಖಾಲಿ,
							 GETVCOL_FOR_VIRTCOL);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#ಆದರೆ_ಅಂತ್ಯ
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    ಶೂನ್ಯ
validate_cheight(ಶೂನ್ಯ)
{
    check_cursor_moved(curwin);

    ಆದರೆ (curwin->w_valid & VALID_CHEIGHT)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_ಎತ್ತರ = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	curwin->w_cline_ಎತ್ತರ = plines(curwin->w_cursor.lnum);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, ಖಾಲಿ, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    ಶೂನ್ಯ
validate_cursor_ಲಂಬಸಾಲು(ಶೂನ್ಯ)
{
    colnr_T off;
    colnr_T ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ     ಅಗಲ;

    validate_virtcol();

    ಆದರೆ (curwin->w_valid & VALID_WCOL)
	ಹಿಂತಿರುಗಿ;

    ಲಂಬಸಾಲು = curwin->w_virtcol;
    off = curwin_ಲಂಬಸಾಲು_off();
    ಲಂಬಸಾಲು += off;
    ಅಗಲ = curwin->w_ಅಗಲ - off + curwin_ಲಂಬಸಾಲು_off2();

    // long line wrapping, adjust curwin->w_wrow
    ಆದರೆ (curwin->w_p_wrap
	    && ಲಂಬಸಾಲು >= (colnr_T)curwin->w_ಅಗಲ
	    && ಅಗಲ > 0)
	// use same formula as what is used in curs_columns()
	ಲಂಬಸಾಲು -= ((ಲಂಬಸಾಲು - curwin->w_ಅಗಲ) / ಅಗಲ + 1) * ಅಗಲ;
    ಆದರೆ (ಲಂಬಸಾಲು > (ಪೂರ್ಣಾಂಕ)curwin->w_leftcol)
	ಲಂಬಸಾಲು -= curwin->w_leftcol;
    ಇಲ್ಲದಿದ್ದರೆ
	ಲಂಬಸಾಲು = 0;
    curwin->w_wcol = ಲಂಬಸಾಲು;

    curwin->w_valid |= VALID_WCOL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    curwin->w_flags &= ~WFLAG_WCOL_OFF_ADDED;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Compute offset of a window, occupied by absolute or relative line number,
 * fold column and sign column (these don't move when scrolling horizontally).
 */
    ಪೂರ್ಣಾಂಕ
win_ಲಂಬಸಾಲು_off(win_T *wp)
{
    ಹಿಂತಿರುಗಿ (((wp->w_p_nu || wp->w_p_rnu) ? ಸಂಖ್ಯೆ_ಅಗಲ(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    + wp->w_p_fdc
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#ಆದರೆ_ಅಂತ್ಯ
	   );
}

    ಪೂರ್ಣಾಂಕ
curwin_ಲಂಬಸಾಲು_off(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ win_ಲಂಬಸಾಲು_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    ಪೂರ್ಣಾಂಕ
win_ಲಂಬಸಾಲು_off2(win_T *wp)
{
    ಆದರೆ ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಸಂಖ್ಯೆ_ಅಗಲ(wp) + 1;
    ಹಿಂತಿರುಗಿ 0;
}

    ಪೂರ್ಣಾಂಕ
curwin_ಲಂಬಸಾಲು_off2(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ win_ಲಂಬಸಾಲು_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    ಶೂನ್ಯ
curs_columns(
    ಪೂರ್ಣಾಂಕ		may_scroll)	// when TRUE, may scroll horizontally
{
    ದೀರ್ಘ	diff;
    ಪೂರ್ಣಾಂಕ		extra;		// offset for first screen line
    ದೀರ್ಘ	off_ಎಡ, off_ಬಲ;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		p_ಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		width1;		// text width for first screen line
    ಪೂರ್ಣಾಂಕ		width2 = 0;	// text width for second and later screen line
    ಪೂರ್ಣಾಂಕ		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    ದೀರ್ಘ	so = get_scrolloff_ಮೌಲ್ಯ();
    ದೀರ್ಘ	siso = get_sidescrolloff_ಮೌಲ್ಯ();
    ಪೂರ್ಣಾಂಕ		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    ಆದರೆ (!(curwin->w_valid & VALID_CROW))
	curs_ಅಡ್ಡಸಾಲುಗಳು(curwin);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_ಮೊದಲ_char = 0;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Compute the number of virtual columns.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    ಆದರೆ (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_ಲಂಬಸಾಲು_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_ಅಡ್ಡಸಾಲು;

    width1 = curwin->w_ಅಗಲ - extra;
    ಆದರೆ (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_ಅಗಲ - 1;
	ಆದರೆ (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_ಎತ್ತರ - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_wrow = curwin->w_ಎತ್ತರ - 1 - curwin->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_p_wrap && curwin->w_ಅಗಲ != 0)
    {
	width2 = width1 + curwin_ಲಂಬಸಾಲು_off2();

	// skip columns that are not visible
	ಆದರೆ (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    ಆದರೆ (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    ಇಲ್ಲದಿದ್ದರೆ
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	ಆದರೆ (curwin->w_wcol >= curwin->w_ಅಗಲ)
	{
	    // this same formula is used in validate_cursor_col()
	    n = (curwin->w_wcol - curwin->w_ಅಗಲ) / width2 + 1;
	    curwin->w_wcol -= n * width2;
	    curwin->w_wrow += n;
	}
    }

    // No line wrapping: compute curwin->w_leftcol if scrolling is on and line
    // is not folded.
    // If scrolling is off, curwin->w_leftcol is assumed to be 0
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (may_scroll
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    && !curwin->w_cline_folded
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (curwin->w_virtcol_ಮೊದಲ_char > 0)
	{
	    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = (curwin->w_ಅಗಲ - extra);
	    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2 = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > 0 ? curwin->w_virtcol_ಮೊದಲ_char / ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ : 1;

	    // each "above" text prop shifts the text one row down
	    curwin->w_wrow += ಅಡ್ಡಸಾಲುಗಳು_2;
	    curwin->w_wcol -= ಅಡ್ಡಸಾಲುಗಳು_2 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	    endcol -= ಅಡ್ಡಸಾಲುಗಳು_2 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	    curwin->w_cline_ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು_2 + 1;
	}
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * If Cursor is left of the screen, scroll rightwards.
	 * If Cursor is right of the screen, scroll leftwards.
	 * If we get closer to the edge than 'sidescrolloff', scroll a little
	 * extra.
	 */
	off_ಎಡ = startcol - curwin->w_leftcol - siso;
	off_ಬಲ = endcol - curwin->w_leftcol - (curwin->w_ಅಗಲ - siso) + 1;
	ಆದರೆ (off_ಎಡ < 0 || off_ಬಲ > 0)
	{
	    ಆದರೆ (off_ಎಡ < 0)
		diff = -off_ಎಡ;
	    ಇಲ್ಲದಿದ್ದರೆ
		diff = off_ಬಲ;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    ಆದರೆ (p_ss == 0 || diff >= width1 / 2 || off_ಬಲ >= off_ಎಡ)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (diff < p_ss)
		    diff = p_ss;
		ಆದರೆ (off_ಎಡ < 0)
		    new_leftcol = curwin->w_leftcol - (ಪೂರ್ಣಾಂಕ)diff;
		ಇಲ್ಲದಿದ್ದರೆ
		    new_leftcol = curwin->w_leftcol + (ಪೂರ್ಣಾಂಕ)diff;
	    }
	    ಆದರೆ (new_leftcol < 0)
		new_leftcol = 0;
	    ಆದರೆ (new_leftcol != (ಪೂರ್ಣಾಂಕ)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_wcol > (ಪೂರ್ಣಾಂಕ)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    ಇಲ್ಲದಿದ್ದರೆ
	curwin->w_wcol = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    ಆದರೆ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    ಇಲ್ಲದಿದ್ದರೆ
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#ಆದರೆ_ಅಂತ್ಯ

    prev_skipcol = curwin->w_skipcol;

    p_ಸಾಲುಗಳು = 0;

    ಆದರೆ ((curwin->w_wrow >= curwin->w_ಎತ್ತರ
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_ಎತ್ತರ)
		    && (p_ಸಾಲುಗಳು =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			plines_win_nofill
#ಇಲ್ಲದಿದ್ದರೆ
			plines_win
#ಆದರೆ_ಅಂತ್ಯ
			(curwin, curwin->w_cursor.lnum, FALSE))
						    - 1 >= curwin->w_ಎತ್ತರ))
	    && curwin->w_ಎತ್ತರ != 0
	    && curwin->w_cursor.lnum == curwin->w_topline
	    && width2 > 0
	    && curwin->w_ಅಗಲ != 0)
    {
	// Cursor past end of screen.  Happens with a single line that does
	// not fit on screen.  Find a skipcol to show the text around the
	// cursor.  Avoid scrolling all the time. compute value of "extra":
	// 1: Less than 'scrolloff' lines above
	// 2: Less than 'scrolloff' lines below
	// 3: both of them
	extra = 0;
	ಆದರೆ (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	ಆದರೆ (p_ಸಾಲುಗಳು == 0)
	    p_ಸಾಲುಗಳು = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_ಸಾಲುಗಳು;
	ಆದರೆ (p_ಸಾಲುಗಳು > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	ಇಲ್ಲದಿದ್ದರೆ
	    n = p_ಸಾಲುಗಳು;
	ಆದರೆ ((colnr_T)n >= curwin->w_ಎತ್ತರ + curwin->w_skipcol / width2 - so)
	    extra += 2;

	ಆದರೆ (extra == 3 || curwin->w_ಎತ್ತರ <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    ಆದರೆ (n > curwin->w_ಎತ್ತರ / 2)
		n -= curwin->w_ಎತ್ತರ / 2;
	    ಇಲ್ಲದಿದ್ದರೆ
		n = 0;
	    // don't skip more than necessary
	    ಆದರೆ (n > p_ಸಾಲುಗಳು - curwin->w_ಎತ್ತರ + 1)
		n = p_ಸಾಲುಗಳು - curwin->w_ಎತ್ತರ + 1;
	    ಆದರೆ (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    ಇಲ್ಲದಿದ್ದರೆ
		curwin->w_skipcol = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    ಆದರೆ (extra > 0)
	    {
		ಆದರೆ ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_ಎತ್ತರ + 1) * width2;
	    ಆಗ (endcol > curwin->w_virtcol)
		endcol -= width2;
	    ಆದರೆ (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	ಆದರೆ (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - prev_skipcol) / width2;
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	ಆದರೆ (curwin->w_wrow >= curwin->w_ಎತ್ತರ)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_ಎತ್ತರ + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((ಪೂರ್ಣಾಂಕ)prev_skipcol - (ಪೂರ್ಣಾಂಕ)curwin->w_skipcol) / width2;
	ಆದರೆ (extra > 0)
	    win_ins_ಸಾಲುಗಳು(curwin, 0, extra, FALSE, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra < 0)
	    win_del_ಸಾಲುಗಳು(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    ಆದರೆ (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ಆದರೆ (popup_is_popup(curwin) && curbuf->b_ಪದ != ಖಾಲಿ)
    {
	curwin->w_wrow += popup_ತುದಿ_extra(curwin);
	curwin->w_wcol += popup_ಎಡ_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#ಆದರೆ_ಅಂತ್ಯ

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    ಶೂನ್ಯ
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	ಪೂರ್ಣಾಂಕ	*rowp,	// screen row
	ಪೂರ್ಣಾಂಕ	*scolp,	// start screen column
	ಪೂರ್ಣಾಂಕ	*ccolp,	// cursor screen column
	ಪೂರ್ಣಾಂಕ	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = 0;
    colnr_T	coloff = 0;

    ಆದರೆ (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ	    ಅಗಲ;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	ಪೂರ್ಣಾಂಕ	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, ಖಾಲಿ, TRUE, ಖಾಲಿ);
# endif
	ಅಡ್ಡಸಾಲು = plines_m_win(wp, wp->w_topline, lnum - 1, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
	// "row" should be the screen line where line "lnum" begins, which can
	// be negative if "lnum" is "w_topline" and "w_skipcol" is non-zero.
	ಅಡ್ಡಸಾಲು -= adjust_plines_for_skipcol(wp);

# ifdef FEAT_DIFF
	// Add filler lines above this buffer line.
	ಅಡ್ಡಸಾಲು += lnum == wp->w_topline ? wp->w_topfill
				     : diff_check_fill(wp, lnum);
# endif

	colnr_T	off = win_ಲಂಬಸಾಲು_off(wp);
# ifdef FEAT_FOLDING
	ಆದರೆ (is_folded)
	{
	    ಅಡ್ಡಸಾಲು += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    ಲಂಬಸಾಲು = scol;
	    ಲಂಬಸಾಲು += off;
	    ಅಗಲ = wp->w_ಅಗಲ - off + win_ಲಂಬಸಾಲು_off2(wp);

	    // long line wrapping, adjust row
	    ಆದರೆ (wp->w_p_wrap
		    && ಲಂಬಸಾಲು >= (colnr_T)wp->w_ಅಗಲ
		    && ಅಗಲ > 0)
	    {
		// use same formula as what is used in curs_columns()
		ಪೂರ್ಣಾಂಕ rowoff = ((ಲಂಬಸಾಲು - wp->w_ಅಗಲ) / ಅಗಲ + 1);
		ಲಂಬಸಾಲು -= rowoff * ಅಗಲ;
		ಅಡ್ಡಸಾಲು += rowoff;
	    }
	    ಲಂಬಸಾಲು -= wp->w_leftcol;
	    ಆದರೆ (ಲಂಬಸಾಲು >= wp->w_ಅಗಲ)
		ಲಂಬಸಾಲು = -1;
	    ಆದರೆ (ಲಂಬಸಾಲು >= 0 && ಅಡ್ಡಸಾಲು >= 0 && ಅಡ್ಡಸಾಲು < wp->w_ಎತ್ತರ)
	    {
		coloff = ಲಂಬಸಾಲು - scol + wp->w_wincol + 1;
		ಅಡ್ಡಸಾಲು += W_WINROW(wp) + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// character is out of the window
		ಅಡ್ಡಸಾಲು = scol = ccol = ecol = 0;
	}
    }
    *rowp = ಅಡ್ಡಸಾಲು;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    ಆದರೆ (wp->w_p_rl && ಅಡ್ಡಸಾಲು > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	ಪೂರ್ಣಾಂಕ endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_ಅಗಲ - endoff;
    }
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    ಶೂನ್ಯ
f_screenpos(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		scol = 0, ccol = 0, ecol = 0;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    dict = rettv->vval.v_dict;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    pos.lnum = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    ಆದರೆ (pos.lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), pos.lnum);
	ಹಿಂತಿರುಗಿ;
    }
    pos.ಲಂಬಸಾಲು = tv_get_ಸಂಖ್ಯೆ(&argvars[2]) - 1;
    ಆದರೆ (pos.ಲಂಬಸಾಲು < 0)
	pos.ಲಂಬಸಾಲು = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &ಅಡ್ಡಸಾಲು, &scol, &ccol, &ecol);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "row", ಅಡ್ಡಸಾಲು);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "col", scol);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "curscol", ccol);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
virtcol2col(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಂಕ vcol)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ = vcol2col(wp, lnum, vcol - 1, ಖಾಲಿ);
    char_u	*ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
    char_u	*p = ಸಾಲು + ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (*p == NUL)
    {
	ಆದರೆ (p == ಸಾಲು)  // empty line
	    ಹಿಂತಿರುಗಿ 0;
	// Move to the first byte of the last char.
	MB_PTR_ಹಿಂಭಾಗ(ಸಾಲು, p);
    }
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    ಶೂನ್ಯ
f_virtcol2col(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		screencol;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
	    || check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
	    || check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL)
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    lnum = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಆದರೆ (error || lnum < 0 || lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;

    screencol = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    ಆದರೆ (error || screencol < 0)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = virtcol2col(wp, lnum, screencol);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
ಸ್ಥಿರ ಶೂನ್ಯ cursor_correct_sms(ಶೂನ್ಯ)
{
    ಆದರೆ (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	ಹಿಂತಿರುಗಿ;

    ದೀರ್ಘ    so = get_scrolloff_ಮೌಲ್ಯ();
    ಪೂರ್ಣಾಂಕ	    width1 = curwin->w_ಅಗಲ - curwin_ಲಂಬಸಾಲು_off();
    ಪೂರ್ಣಾಂಕ	    width2 = width1 + curwin_ಲಂಬಸಾಲು_off2();
    ಪೂರ್ಣಾಂಕ	    so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = so == 0 ? 0 : width1 + (so - 1) * width2;
    ಪೂರ್ಣಾಂಕ	    space_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = (curwin->w_ಎತ್ತರ - 1) * width2;
    ಪೂರ್ಣಾಂಕ	    overlap, ತುದಿ, bot;
    ಪೂರ್ಣಾಂಕ	    ಗಾತ್ರ_2 = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    ಆದರೆ (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;               // Ignore 'scrolloff' at top of buffer.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > space_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ / 2)
	so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = space_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    ಆಗ (so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > ಗಾತ್ರ_2 && so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - width2 >= width1 && width1 > 0)
	so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ -= width2;
    ಆದರೆ (so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ >= width1 && so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > ಗಾತ್ರ_2)
	so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_ಅಗಲ - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    ತುದಿ = curwin->w_skipcol + (so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != 0 ? so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_ಎತ್ತರ - 1) * width2
								    - so_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    validate_virtcol();
    colnr_T ಲಂಬಸಾಲು = curwin->w_virtcol;

    ಆದರೆ (ಲಂಬಸಾಲು < ತುದಿ)
    {
	ಆದರೆ (ಲಂಬಸಾಲು < width1)
	    ಲಂಬಸಾಲು += width1;
	ಆಗ (width2 > 0 && ಲಂಬಸಾಲು < ತುದಿ)
	    ಲಂಬಸಾಲು += width2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಆಗ (width2 > 0 && ಲಂಬಸಾಲು >= bot)
	    ಲಂಬಸಾಲು -= width2;

    ಆದರೆ (ಲಂಬಸಾಲು != curwin->w_virtcol)
    {
	ಪೂರ್ಣಾಂಕ rc;

	curwin->w_curswant = ಲಂಬಸಾಲು;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	ಆದರೆ (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    validate_virtcol();
	    ಆದರೆ (curwin->w_virtcol < curwin->w_skipcol + overlap)
	    {
		// Cursor still not visible: move it to the next line instead.
		curwin->w_cursor.lnum++;
		curwin->w_cursor.ಲಂಬಸಾಲು = 0;
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
scroll_redraw(ಪೂರ್ಣಾಂಕ ಮೇಲೆ, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    linenr_T	prev_topline = curwin->w_topline;
    ಪೂರ್ಣಾಂಕ		prev_skipcol = curwin->w_skipcol;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		prev_topfill = curwin->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    ಆದರೆ (ಮೇಲೆ)
	scrollup(ಎಣಿಕೆ_2, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	scrolldown(ಎಣಿಕೆ_2, TRUE);
    ಆದರೆ (get_scrolloff_ಮೌಲ್ಯ() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	ಆಗ (curwin->w_topline == prev_topline
		&& curwin->w_skipcol == prev_skipcol
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    ಆದರೆ (ಮೇಲೆ)
	    {
		ಆದರೆ (curwin->w_cursor.lnum > prev_lnum
			|| cursor_ಕೆಳಕ್ಕೆ(1L, FALSE) == FAIL)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_ಮೇಲೆ(1L, FALSE) == FAIL)
		    ನಿಲ್ಲಿಸು;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    ಆದರೆ (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    ಶೂನ್ಯ
scrolldown(
    ದೀರ್ಘ	ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		byfold ಬಳಸದ)	// TRUE: count a closed fold as one line
{
    ದೀರ್ಘ	ಮುಗಿದ = 0;	// total # of physical lines done
    ಪೂರ್ಣಾಂಕ		wrow;
    ಪೂರ್ಣಾಂಕ		moved = FALSE;
    ಪೂರ್ಣಾಂಕ		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ಪೂರ್ಣಾಂಕ		width1 = 0;
    ಪೂರ್ಣಾಂಕ		width2 = 0;

    ಆದರೆ (do_sms)
    {
	width1 = curwin->w_ಅಗಲ - curwin_ಲಂಬಸಾಲು_off();
	width2 = width1 + curwin_ಲಂಬಸಾಲು_off2();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    linenr_T	ಮೊದಲ;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    validate_cursor();		// w_wrow needs to be valid
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ todo = ಸಾಲು_ಎಣಿಕೆ; todo > 0; --todo)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	ಆದರೆ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_ಎತ್ತರ - 1)
	{
	    ++curwin->w_topfill;
	    ++ಮುಗಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    // break when at the very top
	    ಆದರೆ (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		ಆದರೆ (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		ಇಲ್ಲದಿದ್ದರೆ
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++ಮುಗಿದ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		curwin->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		ಆದರೆ (hasFolding(curwin->w_topline, &ಮೊದಲ, ಖಾಲಿ))
		{
		    ++ಮುಗಿದ;
		    ಆದರೆ (!byfold)
			todo -= curwin->w_topline - ಮೊದಲ - 1;
		    curwin->w_botline -= curwin->w_topline - ಮೊದಲ;
		    curwin->w_topline = ಮೊದಲ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (do_sms)
		{
		    ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = linetabsize_eol(curwin, curwin->w_topline);
		    ಆದರೆ (ಗಾತ್ರ_2 > width1)
		    {
			curwin->w_skipcol = width1;
			ಗಾತ್ರ_2 -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    ಆಗ (ಗಾತ್ರ_2 > width2)
		    {
			curwin->w_skipcol += width2;
			ಗಾತ್ರ_2 -= width2;
		    }
		    ++ಮುಗಿದ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಮುಗಿದ += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += ಮುಗಿದ;		// keep w_wrow updated
    curwin->w_cline_ಅಡ್ಡಸಾಲು += ಮುಗಿದ;	// keep w_cline_row updated

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_ಅಡ್ಡಸಾಲು = 0;
    check_topfill(curwin, TRUE);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    ಆದರೆ (curwin->w_p_wrap && curwin->w_ಅಗಲ != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_ಎತ್ತರ - 1 -
	    curwin->w_virtcol / curwin->w_ಅಗಲ;
    }
    ಆಗ (wrow >= curwin->w_ಎತ್ತರ && curwin->w_cursor.lnum > 1)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಆದರೆ (hasFolding(curwin->w_cursor.lnum, &ಮೊದಲ, ಖಾಲಿ))
	{
	    --wrow;
	    ಆದರೆ (ಮೊದಲ == 1)
		curwin->w_cursor.lnum = 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		curwin->w_cursor.lnum = ಮೊದಲ - 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    ಆದರೆ (moved)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#ಆದರೆ_ಅಂತ್ಯ
	coladvance(curwin->w_curswant);
    }
    ಆದರೆ (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    ಶೂನ್ಯ
scrollup(
    ದೀರ್ಘ	ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		byfold ಬಳಸದ)	// TRUE: count a closed fold as one line
{
    ಪೂರ್ಣಾಂಕ		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    ಆದರೆ (do_sms
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	ಪೂರ್ಣಾಂಕ	    width1 = curwin->w_ಅಗಲ - curwin_ಲಂಬಸಾಲು_off();
	ಪೂರ್ಣಾಂಕ	    width2 = width1 + curwin_ಲಂಬಸಾಲು_off2();
	ಪೂರ್ಣಾಂಕ	    ಗಾತ್ರ_2 = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	ಆದರೆ (do_sms)
	    ಗಾತ್ರ_2 = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ todo = ಸಾಲು_ಎಣಿಕೆ; todo > 0; --todo)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    ಆದರೆ (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		linenr_T lnum = curwin->w_topline;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		ಆದರೆ (byfold)
		    // for a closed fold: go to the last line in the fold
		    (ಶೂನ್ಯ)hasFolding(lnum, ಖಾಲಿ, &lnum);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += ಕೂಡಿಸು;
		    ಆದರೆ (curwin->w_skipcol >= ಗಾತ್ರ_2)
		    {
			ಆದರೆ (lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= ಕೂಡಿಸು;
			    ನಿಲ್ಲಿಸು;
			}
			++lnum;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			ನಿಲ್ಲಿಸು;
		    ++lnum;
		}

		ಆದರೆ (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#ಆದರೆ_ಅಂತ್ಯ
		    curwin->w_skipcol = 0;
		    ಆದರೆ (todo > 1 && do_sms)
			ಗಾತ್ರ_2 = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	ಆದರೆ (prev_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	curwin->w_topline += ಸಾಲು_ಎಣಿಕೆ;
	curwin->w_botline += ಸಾಲು_ಎಣಿಕೆ;	// approximate w_botline
    }

    ಆದರೆ (curwin->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	curwin->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಆದರೆ (curwin->w_botline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	curwin->w_botline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    check_topfill(curwin, FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    ಆದರೆ (curwin->w_cursor.lnum < curwin->w_topline)
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
    ಆದರೆ (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ	    width1 = curwin->w_ಅಗಲ - curwin_ಲಂಬಸಾಲು_off();
    ಆದರೆ (width1 <= 0)
	ಹಿಂತಿರುಗಿ;  // no text will be displayed

    ಪೂರ್ಣಾಂಕ	    width2 = width1 + curwin_ಲಂಬಸಾಲು_off2();
    ದೀರ್ಘ    so = get_scrolloff_ಮೌಲ್ಯ();
    ಪೂರ್ಣಾಂಕ	    scrolloff_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = so == 0 ? 0 : width1 + (so - 1) * width2;
    ಪೂರ್ಣಾಂಕ	    scrolled = FALSE;
    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ	    overlap, ಲಂಬಸಾಲು;

    validate_cheight();
    ಆದರೆ (curwin->w_cline_ಎತ್ತರ == curwin->w_ಎತ್ತರ
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_ಎತ್ತರ)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	ಹಿಂತಿರುಗಿ;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_ಅಗಲ - width2);
    ಆಗ (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    {
	// scroll a screen line down
	ಆದರೆ (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    ಆದರೆ (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	ಹಿಂತಿರುಗಿ;  // don't scroll in the other direction now
    }

    ಲಂಬಸಾಲು = curwin->w_virtcol + scrolloff_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    ಆದರೆ (scrolloff_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > 0)
    {
	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = linetabsize_eol(curwin, curwin->w_topline);
	ಗಾತ್ರ_2 = width1 + width2 * ((ಗಾತ್ರ_2 - width1 + width2 - 1) / width2);
	ಆಗ (ಲಂಬಸಾಲು > ಗಾತ್ರ_2)
	    ಲಂಬಸಾಲು -= width2;
    }
    ಲಂಬಸಾಲು -= curwin->w_skipcol;

    ಆದರೆ (ಲಂಬಸಾಲು >= width1)
    {
	ಲಂಬಸಾಲು -= width1;
	++ಅಡ್ಡಸಾಲು;
    }
    ಆದರೆ (ಲಂಬಸಾಲು > width2)
    {
	ಅಡ್ಡಸಾಲು += ಲಂಬಸಾಲು / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	ಲಂಬಸಾಲು = ಲಂಬಸಾಲು % width2;
    }
    ಆದರೆ (ಅಡ್ಡಸಾಲು >= curwin->w_ಎತ್ತರ)
    {
	ಆದರೆ (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --ಅಡ್ಡಸಾಲು;
	}
	ಆದರೆ (ಅಡ್ಡಸಾಲು >= curwin->w_ಎತ್ತರ)
	    curwin->w_skipcol += (ಅಡ್ಡಸಾಲು - curwin->w_ಎತ್ತರ) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    ಶೂನ್ಯ
check_topfill(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		ಕೆಳಕ್ಕೆ)	// when TRUE scroll down when not enough space
{
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (wp->w_topfill <= 0)
	ಹಿಂತಿರುಗಿ;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    ಆದರೆ (wp->w_topfill + n > wp->w_ಎತ್ತರ)
    {
	ಆದರೆ (ಕೆಳಕ್ಕೆ && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp->w_topfill = wp->w_ಎತ್ತರ - n;
	    ಆದರೆ (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    ಶೂನ್ಯ
scrolldown_clamp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_ಅಡ್ಡಸಾಲು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (curwin->w_topline <= 1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    && !can_fill
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಹಿಂತಿರುಗಿ;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    ಕೊನೆ_ಅಡ್ಡಸಾಲು = curwin->w_wrow;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (can_fill)
	++ಕೊನೆ_ಅಡ್ಡಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆ_ಅಡ್ಡಸಾಲು += plines_nofill(curwin->w_topline - 1);
#ಇಲ್ಲದಿದ್ದರೆ
    ಕೊನೆ_ಅಡ್ಡಸಾಲು += plines(curwin->w_topline - 1);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (curwin->w_p_wrap && curwin->w_ಅಗಲ != 0)
    {
	validate_cheight();
	validate_virtcol();
	ಕೊನೆ_ಅಡ್ಡಸಾಲು += curwin->w_cline_ಎತ್ತರ - 1 -
	    curwin->w_virtcol / curwin->w_ಅಗಲ;
    }
    ಆದರೆ (ಕೊನೆ_ಅಡ್ಡಸಾಲು < curwin->w_ಎತ್ತರ - get_scrolloff_ಮೌಲ್ಯ())
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	ಆದರೆ (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	--curwin->w_topline;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	(ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
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
    ಪೂರ್ಣಾಂಕ	    ಆರಂಭ_ಅಡ್ಡಸಾಲು;

    ಆದರೆ (curwin->w_topline == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    && curwin->w_topfill == 0
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಹಿಂತಿರುಗಿ;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆರಂಭ_ಅಡ್ಡಸಾಲು = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#ಇಲ್ಲದಿದ್ದರೆ
    ಆರಂಭ_ಅಡ್ಡಸಾಲು = curwin->w_wrow - plines(curwin->w_topline);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (curwin->w_p_wrap && curwin->w_ಅಗಲ != 0)
    {
	validate_virtcol();
	ಆರಂಭ_ಅಡ್ಡಸಾಲು -= curwin->w_virtcol / curwin->w_ಅಗಲ;
    }
    ಆದರೆ (ಆರಂಭ_ಅಡ್ಡಸಾಲು >= get_scrolloff_ಮೌಲ್ಯ())
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	ಆದರೆ (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    (ಶೂನ್ಯ)hasFolding(curwin->w_topline, ಖಾಲಿ, &curwin->w_topline);
#ಆದರೆ_ಅಂತ್ಯ
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
topline_ಹಿಂಭಾಗ_winheight(
    lineoff_T	*lp,
    ಪೂರ್ಣಾಂಕ		winheight)	// when TRUE limit to window height
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->ಎತ್ತರ = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	--lp->lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	lp->fill = 0;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (lp->lnum < 1)
	    lp->ಎತ್ತರ = MAXCOL;
	ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (hasFolding(lp->lnum, &lp->lnum, ಖಾಲಿ))
	    // Add a closed fold
	    lp->ಎತ್ತರ = 1;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    lp->ಎತ್ತರ = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
topline_ಹಿಂಭಾಗ(lineoff_T *lp)
{
    topline_ಹಿಂಭಾಗ_winheight(lp, TRUE);
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->ಎತ್ತರ = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	++lp->lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	lp->fill = 0;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (lp->lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    lp->ಎತ್ತರ = MAXCOL;
	ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (hasFolding(lp->lnum, ಖಾಲಿ, &lp->lnum))
	    {
		// Add a closed fold.
		lp->ಎತ್ತರ = 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    lp->ಎತ್ತರ = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    ಶೂನ್ಯ
scroll_cursor_ತುದಿ(ಪೂರ್ಣಾಂಕ min_scroll, ಪೂರ್ಣಾಂಕ always)
{
    ಪೂರ್ಣಾಂಕ		scrolled = 0;
    ಪೂರ್ಣಾಂಕ		extra = 0;
    ಪೂರ್ಣಾಂಕ		ಬಳಸಿದ;
    ಪೂರ್ಣಾಂಕ		i;
    linenr_T	ತುದಿ;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    ಪೂರ್ಣಾಂಕ		old_skipcol = curwin->w_skipcol;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ
    linenr_T	new_topline;
    ಪೂರ್ಣಾಂಕ		off = get_scrolloff_ಮೌಲ್ಯ();

    ಆದರೆ (mouse_dragging > 0)
	off = mouse_dragging - 1;

    /*
     * Decrease topline until:
     * - it has become 1
     * - (part of) the cursor line is moved off the screen or
     * - moved at least 'scrolljump' lines and
     * - at least 'scrolloff' lines above and below the cursor
     */
    validate_cheight();
    ಬಳಸಿದ = curwin->w_cline_ಎತ್ತರ; // includes filler lines above
    ಆದರೆ (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = ಬಳಸಿದ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (hasFolding(curwin->w_cursor.lnum, &ತುದಿ, &bot))
    {
	--ತುದಿ;
	++bot;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ತುದಿ = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = ತುದಿ + 1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    // "used" already contains the number of filler lines above, don't add it
    // again.
    // Hide filler lines above cursor line by adding them to "extra".
    extra += diff_check_fill(curwin, curwin->w_cursor.lnum);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Check if the lines from "top" to "bot" fit in the window.  If they do,
     * set new_topline and advance "top" and "bot" to include more lines.
     */
    ಆಗ (ತುದಿ > 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಆದರೆ (hasFolding(ತುದಿ, &ತುದಿ, ಖಾಲಿ))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    i = PLINES_NOFILL(ತುದಿ);
	ಆದರೆ (ತುದಿ < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	ಆದರೆ ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    ನಿಲ್ಲಿಸು;

	ಬಳಸಿದ += i;
	ಆದರೆ (extra + i <= off && bot < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (hasFolding(bot, ಖಾಲಿ, &bot))
		// count one logical line for a sequence of folded lines
		++ಬಳಸಿದ;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಬಳಸಿದ += plines(bot);
	}
	ಆದರೆ (ಬಳಸಿದ > curwin->w_ಎತ್ತರ)
	    ನಿಲ್ಲಿಸು;

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
    ಆದರೆ (ಬಳಸಿದ > curwin->w_ಎತ್ತರ)
	scroll_cursor_halfway(FALSE, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	ಆದರೆ (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	ಆದರೆ (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	ಆದರೆ (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    ಆದರೆ (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (curwin->w_topline != old_topline)
	    reset_skipcol();
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    ಆದರೆ (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		ಪೂರ್ಣಾಂಕ	width1 = curwin->w_ಅಗಲ - curwin_ಲಂಬಸಾಲು_off();
		ಪೂರ್ಣಾಂಕ	width2 = width1 + curwin_ಲಂಬಸಾಲು_off2();
		ಪೂರ್ಣಾಂಕ	plines_off = 0;
		ಪೂರ್ಣಾಂಕ	skipcol;

		ಆದರೆ (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		ಆದರೆ (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	ಆದರೆ (curwin->w_topline != old_topline
		|| curwin->w_skipcol != old_skipcol
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#ಆದರೆ_ಅಂತ್ಯ
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
ಗಣ_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು(win_T *wp, ಪೂರ್ಣಾಂಕ ಬಳಸಿದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    wp->w_filler_ಅಡ್ಡಸಾಲುಗಳು = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಬಳಸಿದ == 0)
	wp->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು = 0;	// single line that doesn't fit
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wp->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು = wp->w_ಎತ್ತರ - ಬಳಸಿದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	ಆದರೆ (wp->w_botline <= wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    wp->w_filler_ಅಡ್ಡಸಾಲುಗಳು = diff_check_fill(wp, wp->w_botline);
	    ಆದರೆ (wp->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು > wp->w_filler_ಅಡ್ಡಸಾಲುಗಳು)
		wp->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು -= wp->w_filler_ಅಡ್ಡಸಾಲುಗಳು;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		wp->w_filler_ಅಡ್ಡಸಾಲುಗಳು = wp->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
		wp->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು = 0;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
}

/*
 * Recompute topline to put the cursor at the bottom of the window.
 * When scrolling scroll at least "min_scroll" lines.
 * If "set_topbot" is TRUE, set topline and botline first (for "zb").
 * This is messy stuff!!!
 */
    ಶೂನ್ಯ
scroll_cursor_bot(ಪೂರ್ಣಾಂಕ min_scroll, ಪೂರ್ಣಾಂಕ ಗಣ_topbot)
{
    ಪೂರ್ಣಾಂಕ		ಬಳಸಿದ;
    ಪೂರ್ಣಾಂಕ		scrolled = 0;
    ಪೂರ್ಣಾಂಕ		extra = 0;
    ಪೂರ್ಣಾಂಕ		i;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;
    linenr_T	old_topline = curwin->w_topline;
    ಪೂರ್ಣಾಂಕ		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		old_topfill = curwin->w_topfill;
    ಪೂರ್ಣಾಂಕ		fill_ಕೆಳಗೆ_ಕಿಟಕಿ;
#ಆದರೆ_ಅಂತ್ಯ
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    ಪೂರ್ಣಾಂಕ		old_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು = curwin->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
    linenr_T	cln;		    // Cursor Line Number
    ದೀರ್ಘ	so = get_scrolloff_ಮೌಲ್ಯ();
    ಪೂರ್ಣಾಂಕ		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    ಆದರೆ (ಗಣ_topbot)
    {
	ಬಳಸಿದ = 0;
	linenr_T cln_ಕೊನೆಯ = cln;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	(ಶೂನ್ಯ)hasFolding(cln, ಖಾಲಿ, &cln_ಕೊನೆಯ);
#ಆದರೆ_ಅಂತ್ಯ
	curwin->w_botline = cln_ಕೊನೆಯ + 1;
	loff.lnum = cln_ಕೊನೆಯ + 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	loff.fill = 0;
#ಆದರೆ_ಅಂತ್ಯ
	ಆಗ (TRUE)
	{
	    topline_ಹಿಂಭಾಗ_winheight(&loff, FALSE);
	    ಆದರೆ (loff.ಎತ್ತರ == MAXCOL)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (ಬಳಸಿದ + loff.ಎತ್ತರ > curwin->w_ಎತ್ತರ)
	    {
		ಆದರೆ (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    ಆದರೆ (ಬಳಸಿದ < curwin->w_ಎತ್ತರ)
		    {
			ಪೂರ್ಣಾಂಕ plines_ಸ್ಥಾನಾಂತರ = ಬಳಸಿದ + loff.ಎತ್ತರ
							    - curwin->w_ಎತ್ತರ;
			ಬಳಸಿದ = curwin->w_ಎತ್ತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			curwin->w_topfill = loff.fill;
#ಆದರೆ_ಅಂತ್ಯ
			curwin->w_topline = loff.lnum;
			curwin->w_skipcol = skipcol_from_plines(
							curwin, plines_ಸ್ಥಾನಾಂತರ);
		    }
		}
		ನಿಲ್ಲಿಸು;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#ಆದರೆ_ಅಂತ್ಯ
	    curwin->w_topline = loff.lnum;
	    ಬಳಸಿದ += loff.ಎತ್ತರ;
	}

	ಗಣ_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು(curwin, ಬಳಸಿದ);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	ಆದರೆ (curwin->w_topline != old_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#ಆದರೆ_ಅಂತ್ಯ
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    ಆದರೆ (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    ಇಲ್ಲದಿದ್ದರೆ
		reset_skipcol();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	validate_botline();

    // The lines of the cursor line itself are always used.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಬಳಸಿದ = plines_nofill(cln);
#ಇಲ್ಲದಿದ್ದರೆ
    validate_cheight();
    ಬಳಸಿದ = curwin->w_cline_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	ಪೂರ್ಣಾಂಕ upto_cursor = plines_win_ಲಂಬಸಾಲು(curwin, cln, curwin->w_cursor.ಲಂಬಸಾಲು);

	ಆದರೆ (upto_cursor < ಬಳಸಿದ)
	    ಬಳಸಿದ = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    ಆದರೆ (cln >= curwin->w_botline)
    {
	scrolled = ಬಳಸಿದ;
	ಆದರೆ (cln == curwin->w_botline)
	    scrolled -= curwin->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
	ಆದರೆ (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    ಪೂರ್ಣಾಂಕ ತುದಿ_plines =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			    plines_win_nofill
#ಇಲ್ಲದಿದ್ದರೆ
			    plines_win
#ಆದರೆ_ಅಂತ್ಯ
					(curwin, curwin->w_topline, FALSE);
	    ಪೂರ್ಣಾಂಕ width1 = curwin->w_ಅಗಲ - curwin_ಲಂಬಸಾಲು_off();

	    ಆದರೆ (width1 > 0)
	    {
		ಪೂರ್ಣಾಂಕ width2 = width1 + curwin_ಲಂಬಸಾಲು_off2();
		ಪೂರ್ಣಾಂಕ skip_ಸಾಲುಗಳು = 0;

		// A similar formula is used in curs_columns().
		ಆದರೆ (curwin->w_skipcol > width1)
		    skip_ಸಾಲುಗಳು += (curwin->w_skipcol - width1) / width2 + 1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_skipcol > 0)
		    skip_ಸಾಲುಗಳು = 1;

		ತುದಿ_plines -= skip_ಸಾಲುಗಳು;
		ಆದರೆ (ತುದಿ_plines > curwin->w_ಎತ್ತರ)
		{
		    scrolled += (ತುದಿ_plines - curwin->w_ಎತ್ತರ);
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
#ಆದರೆ_ಅಂತ್ಯ
    {
	loff.lnum = cln;
	boff.lnum = cln;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    loff.fill = 0;
    boff.fill = 0;
    fill_ಕೆಳಗೆ_ಕಿಟಕಿ = diff_check_fill(curwin, curwin->w_botline)
						      - curwin->w_filler_ಅಡ್ಡಸಾಲುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	ಆದರೆ ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		&& loff.lnum <= curwin->w_botline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_ಕೆಳಗೆ_ಕಿಟಕಿ)
#ಆದರೆ_ಅಂತ್ಯ
		)
	    ನಿಲ್ಲಿಸು;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_ಹಿಂಭಾಗ(&loff);
	ಆದರೆ (loff.ಎತ್ತರ == MAXCOL)
	    ಬಳಸಿದ = MAXCOL;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಬಳಸಿದ += loff.ಎತ್ತರ;
	ಆದರೆ (ಬಳಸಿದ > curwin->w_ಎತ್ತರ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (loff.lnum >= curwin->w_botline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_ಕೆಳಗೆ_ಕಿಟಕಿ)
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.ಎತ್ತರ;
	    ಆದರೆ (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
	}

	ಆದರೆ (boff.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    ಬಳಸಿದ += boff.ಎತ್ತರ;
	    ಆದರೆ (ಬಳಸಿದ > curwin->w_ಎತ್ತರ)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.ಎತ್ತರ;
		ಆದರೆ (boff.lnum >= curwin->w_botline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_ಅಡ್ಡಸಾಲುಗಳು)
#ಆದರೆ_ಅಂತ್ಯ
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.ಎತ್ತರ;
		    ಆದರೆ (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    ಆದರೆ (scrolled <= 0)
	ಸಾಲು_ಎಣಿಕೆ = 0;
    // more than a screenfull, don't scroll but redraw
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಳಸಿದ > curwin->w_ಎತ್ತರ)
	ಸಾಲು_ಎಣಿಕೆ = ಬಳಸಿದ;
    // scroll minimal number of lines
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಾಲು_ಎಣಿಕೆ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	boff.fill = curwin->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ
	boff.lnum = curwin->w_topline - 1;
	ಗಾಗಿ (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.ಎತ್ತರ;
	    ++ಸಾಲು_ಎಣಿಕೆ;
	}
	ಆದರೆ (i < scrolled)	// below curwin->w_botline, don't scroll
	    ಸಾಲು_ಎಣಿಕೆ = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ >= curwin->w_ಎತ್ತರ && ಸಾಲು_ಎಣಿಕೆ > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ > 0)
    {
	ಆದರೆ (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	ಇಲ್ಲದಿದ್ದರೆ
	    scrollup(ಸಾಲು_ಎಣಿಕೆ, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    ಆದರೆ (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && ಗಣ_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು = old_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    ಆದರೆ (ಗಣ_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    ಶೂನ್ಯ
scroll_cursor_halfway(ಪೂರ್ಣಾಂಕ atend, ಪೂರ್ಣಾಂಕ prefer_above)
{
    ಪೂರ್ಣಾಂಕ		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ಕೆಳಗೆ = 0;
    ಪೂರ್ಣಾಂಕ		ಬಳಸಿದ;
    lineoff_T	loff;
    lineoff_T	boff;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    linenr_T	old_topline = curwin->w_topline;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // if the width changed this needs to be updated first
    may_update_popup_position();
#ಆದರೆ_ಅಂತ್ಯ
    loff.lnum = boff.lnum = curwin->w_cursor.lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    (ಶೂನ್ಯ)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಬಳಸಿದ = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#ಇಲ್ಲದಿದ್ದರೆ
    ಬಳಸಿದ = plines(loff.lnum);
#ಆದರೆ_ಅಂತ್ಯ
    topline = loff.lnum;

    ಪೂರ್ಣಾಂಕ want_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ಆದರೆ (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	ಆದರೆ (atend)
	{
	    want_ಎತ್ತರ = (curwin->w_ಎತ್ತರ - ಬಳಸಿದ) / 2;
	    ಬಳಸಿದ = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    want_ಎತ್ತರ = curwin->w_ಎತ್ತರ;
    }

    ಆಗ (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	ಆದರೆ (do_sms)
	{
	    topline_ಹಿಂಭಾಗ_winheight(&loff, FALSE);
	    ಆದರೆ (loff.ಎತ್ತರ == MAXCOL)
		ನಿಲ್ಲಿಸು;
	    ಬಳಸಿದ += loff.ಎತ್ತರ;
	    ಆದರೆ (!atend && boff.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		botline_forw(&boff);
		ಬಳಸಿದ += boff.ಎತ್ತರ;
	    }
	    ಆದರೆ (ಬಳಸಿದ > want_ಎತ್ತರ)
	    {
		ಆದರೆ (ಬಳಸಿದ - loff.ಎತ್ತರ < want_ಎತ್ತರ)
		{
		    topline = loff.lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    topfill = loff.fill;
#ಆದರೆ_ಅಂತ್ಯ
		    skipcol = skipcol_from_plines(curwin, ಬಳಸಿದ - want_ಎತ್ತರ);
		}
		ನಿಲ್ಲಿಸು;
	    }
	    topline = loff.lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    topfill = loff.fill;
#ಆದರೆ_ಅಂತ್ಯ
	    ಮುಂದುವರಿ;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	ಪೂರ್ಣಾಂಕ ಮುಗಿದ = FALSE;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
	{
	    ಆದರೆ (prefer_above ? (ಸುತ್ತು == 2 && ಕೆಳಗೆ < above)
			     : (ಸುತ್ತು == 1 && ಕೆಳಗೆ <= above))
	    {
		// add a line below the cursor
		ಆದರೆ (boff.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		{
		    botline_forw(&boff);
		    ಬಳಸಿದ += boff.ಎತ್ತರ;
		    ಆದರೆ (ಬಳಸಿದ > curwin->w_ಎತ್ತರ)
		    {
			ಮುಗಿದ = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		    ಕೆಳಗೆ += boff.ಎತ್ತರ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ++ಕೆಳಗೆ;	    // count a "~" line
		    ಆದರೆ (atend)
			++ಬಳಸಿದ;
		}
	    }

	    ಆದರೆ (prefer_above ? (ಸುತ್ತು == 1 && ಕೆಳಗೆ >= above)
			     : (ಸುತ್ತು == 1 && ಕೆಳಗೆ > above))
	    {
		// add a line above the cursor
		topline_ಹಿಂಭಾಗ(&loff);
		ಆದರೆ (loff.ಎತ್ತರ == MAXCOL)
		    ಬಳಸಿದ = MAXCOL;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಬಳಸಿದ += loff.ಎತ್ತರ;
		ಆದರೆ (ಬಳಸಿದ > curwin->w_ಎತ್ತರ)
		{
		    ಮುಗಿದ = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
		above += loff.ಎತ್ತರ;
		topline = loff.lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		topfill = loff.fill;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
	ಆದರೆ (ಮುಗಿದ)
	    ನಿಲ್ಲಿಸು;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (!hasFolding(topline, &curwin->w_topline, ಖಾಲಿ))
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಆದರೆ (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    ಆದರೆ (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_sms)
		reset_skipcol();
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    curwin->w_topfill = topfill;
    ಆದರೆ (old_topline > curwin->w_topline + curwin->w_ಎತ್ತರ)
	curwin->w_botfill = false;
    check_topfill(curwin, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
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
    ಪೂರ್ಣಾಂಕ		above = 0;	    // screen lines above topline
    linenr_T	topline;
    ಪೂರ್ಣಾಂಕ		ಕೆಳಗೆ = 0;	    // screen lines below botline
    linenr_T	botline;
    ಪೂರ್ಣಾಂಕ		above_wanted, ಕೆಳಗೆ_wanted;
    linenr_T	cln;		    // Cursor Line Number
    ಪೂರ್ಣಾಂಕ		max_off;
    ದೀರ್ಘ	so = get_scrolloff_ಮೌಲ್ಯ();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    ಕೆಳಗೆ_wanted = so;
    ಆದರೆ (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	ಕೆಳಗೆ_wanted = mouse_dragging - 1;
    }
    ಆದರೆ (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_ಎತ್ತರ / 2;
	ಆದರೆ (ಕೆಳಗೆ_wanted > max_off)
	    ಕೆಳಗೆ_wanted = max_off;
    }
    validate_botline();
    ಆದರೆ (curwin->w_botline == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	ಕೆಳಗೆ_wanted = 0;
	max_off = (curwin->w_ಎತ್ತರ - 1) / 2;
	ಆದರೆ (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    ಆದರೆ (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - ಕೆಳಗೆ_wanted
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಹಿಂತಿರುಗಿ;

    /*
     * Narrow down the area where the cursor can be put by taking lines from
     * the top and the bottom until:
     * - the desired context lines are found
     * - the lines from the top is past the lines from the bottom
     */
    topline = curwin->w_topline;
    botline = curwin->w_botline - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    // count filler lines as context
    above = curwin->w_topfill;
    ಕೆಳಗೆ = curwin->w_filler_ಅಡ್ಡಸಾಲುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
    ಆಗ ((above < above_wanted || ಕೆಳಗೆ < ಕೆಳಗೆ_wanted) && topline < botline)
    {
	ಆದರೆ (ಕೆಳಗೆ < ಕೆಳಗೆ_wanted && (ಕೆಳಗೆ <= above || above >= above_wanted))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (hasFolding(botline, &botline, ಖಾಲಿ))
		++ಕೆಳಗೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಕೆಳಗೆ += plines(botline);
	    --botline;
	}
	ಆದರೆ (above < above_wanted && (above < ಕೆಳಗೆ || ಕೆಳಗೆ >= ಕೆಳಗೆ_wanted))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (hasFolding(topline, ಖಾಲಿ, &topline))
		++above;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		above += PLINES_NOFILL(topline);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    // Count filler lines below this line as context.
	    ಆದರೆ (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#ಆದರೆ_ಅಂತ್ಯ
	    ++topline;
	}
    }
    ಆದರೆ (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (topline > botline)
	curwin->w_cursor.lnum = botline;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	ಆದರೆ (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_scroll_overlap(ಪೂರ್ಣಾಂಕ dir)
{
    lineoff_T loff;
    ಪೂರ್ಣಾಂಕ	    min_ಎತ್ತರ = curwin->w_ಎತ್ತರ - 2;

    validate_botline();
    ಆದರೆ ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
	ಹಿಂತಿರುಗಿ min_ಎತ್ತರ + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_ಅಡ್ಡಸಾಲುಗಳು : curwin->w_topfill);
    loff.ಎತ್ತರ = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#ಇಲ್ಲದಿದ್ದರೆ
    loff.ಎತ್ತರ = plines(loff.lnum);
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ h1 = loff.ಎತ್ತರ;
    ಆದರೆ (h1 > min_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ min_ಎತ್ತರ + 2;  // no overlap
    ಆದರೆ (dir == FORWARD)
	topline_ಹಿಂಭಾಗ(&loff);
    ಇಲ್ಲದಿದ್ದರೆ
	botline_forw(&loff);

    ಪೂರ್ಣಾಂಕ h2 = loff.ಎತ್ತರ;
    ಆದರೆ (h2 == MAXCOL || h2 + h1 > min_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ min_ಎತ್ತರ + 2;  // no overlap
    ಆದರೆ (dir == FORWARD)
	topline_ಹಿಂಭಾಗ(&loff);
    ಇಲ್ಲದಿದ್ದರೆ
	botline_forw(&loff);

    ಪೂರ್ಣಾಂಕ h3 = loff.ಎತ್ತರ;
    ಆದರೆ (h3 == MAXCOL || h3 + h2 > min_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ min_ಎತ್ತರ + 2;  // no overlap
    ಆದರೆ (dir == FORWARD)
	topline_ಹಿಂಭಾಗ(&loff);
    ಇಲ್ಲದಿದ್ದರೆ
	botline_forw(&loff);

    ಪೂರ್ಣಾಂಕ h4 = loff.ಎತ್ತರ;
    ಆದರೆ (h4 == MAXCOL || h4 + h3 + h2 > min_ಎತ್ತರ || h3 + h2 + h1 > min_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ min_ಎತ್ತರ + 1;  // 1 line overlap
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ min_ಎತ್ತರ;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ scroll_with_sms(ಪೂರ್ಣಾಂಕ dir, ದೀರ್ಘ ಎಣಿಕೆ_2, ದೀರ್ಘ *curscount)
{
    ಪೂರ್ಣಾಂಕ		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		prev_topfill = curwin->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, ಎಣಿಕೆ_2);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    ಆದರೆ (!prev_sms && curwin->w_skipcol > 0)
    {
	ಪೂರ್ಣಾಂಕ fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	ಆದರೆ (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	ಪೂರ್ಣಾಂಕ width1 = curwin->w_ಅಗಲ - curwin_ಲಂಬಸಾಲು_off();
	ಪೂರ್ಣಾಂಕ width2 = width1 + curwin_ಲಂಬಸಾಲು_off2();
	ಎಣಿಕೆ_2 = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	ಆದರೆ (fixdir == FORWARD)
	    ಎಣಿಕೆ_2 = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, ಎಣಿಕೆ_2);
	*curscount += ಎಣಿಕೆ_2 * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    ಹಿಂತಿರುಗಿ curwin->w_topline != prev_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	|| curwin->w_topfill != prev_topfill
#ಆದರೆ_ಅಂತ್ಯ
	|| curwin->w_skipcol != prev_skipcol;
}

/*
 * Move screen "count" (half) pages up ("dir" is BACKWARD) or down ("dir" is
 * FORWARD) and update the screen. Handle moving the cursor and not scrolling
 * to reveal end of buffer lines for half-page scrolling with CTRL-D and CTRL-U.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
pagescroll(ಪೂರ್ಣಾಂಕ dir, ದೀರ್ಘ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ half)
{
    ಪೂರ್ಣಾಂಕ		did_move = FALSE;
    ಪೂರ್ಣಾಂಕ		buflen = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    colnr_T	prev_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    ಆದರೆ (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	ಆದರೆ (ಎಣಿಕೆ_2)
	    curwin->w_p_scr = MIN(curwin->w_ಎತ್ತರ, ಎಣಿಕೆ_2);
	ಎಣಿಕೆ_2 = MIN(curwin->w_ಎತ್ತರ, curwin->w_p_scr);

	ದೀರ್ಘ curscount = ಎಣಿಕೆ_2;
	// Adjust count so as to not reveal end of buffer lines.
	ಆದರೆ (dir == FORWARD
		    && (curwin->w_topline + curwin->w_ಎತ್ತರ + ಎಣಿಕೆ_2 > buflen
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
							|| hasAnyFolding(curwin)
#ಆದರೆ_ಅಂತ್ಯ
	   ))
	{
	    ಪೂರ್ಣಾಂಕ n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    ಆದರೆ (n - ಎಣಿಕೆ_2 < curwin->w_ಎತ್ತರ && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_ಎತ್ತರ + ಎಣಿಕೆ_2);
	    ಆದರೆ (n < curwin->w_ಎತ್ತರ + ಎಣಿಕೆ_2)
		ಎಣಿಕೆ_2 = n - curwin->w_ಎತ್ತರ;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	ಆದರೆ (ಎಣಿಕೆ_2 > 0)
	{
	    did_move = scroll_with_sms(dir, ಎಣಿಕೆ_2, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.ಲಂಬಸಾಲು = prev_ಲಂಬಸಾಲು;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	ಆದರೆ (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == FORWARD)
	    cursor_ಕೆಳಕ್ಕೆ_inner(curwin, curscount);
	ಇಲ್ಲದಿದ್ದರೆ
	    cursor_ಮೇಲೆ_inner(curwin, curscount);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Scroll [count] times 'window' or current window height lines.
	ಎಣಿಕೆ_2 *= ((ONE_ಕಿಟಕಿ && p_ಕಿಟಕಿ > 0 && p_ಕಿಟಕಿ < ಅಡ್ಡಸಾಲುಗಳು - 1) ?
				MAX(1, p_ಕಿಟಕಿ - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, ಎಣಿಕೆ_2, &ಎಣಿಕೆ_2);

	ಆದರೆ (did_move)
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

    ಆದರೆ (get_scrolloff_ಮೌಲ್ಯ() > 0)
	cursor_correct();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#ಆದರೆ_ಅಂತ್ಯ
    did_move = did_move
	|| prev_ಲಂಬಸಾಲು != curwin->w_cursor.ಲಂಬಸಾಲು
	|| prev_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    ಆದರೆ (!did_move)
	beep_flush();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_sol)
	nv_g_home_m_cmd(&ca);

    ಹಿಂತಿರುಗಿ did_move ? OK : FAIL;
}

    ಶೂನ್ಯ
do_check_cursorbind(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ win_T	*prev_curwin = ಖಾಲಿ;
    ಸ್ಥಿರ buf_T	*prev_curbuf = ಖಾಲಿ;
    ಸ್ಥಿರ varnumber_T	prev_changedtick = 0;
    ಸ್ಥಿರ pos_T	prev_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    ಆದರೆ (curwin == prev_curwin && curbuf == prev_curbuf
	    && CHANGEDTICK(curbuf) == prev_changedtick
	    && EQUAL_POS(curwin->w_cursor, prev_cursor))
	ಹಿಂತಿರುಗಿ;
    prev_curwin = curwin;
    prev_curbuf = curbuf;
    prev_changedtick = CHANGEDTICK(curbuf);
    prev_cursor = curwin->w_cursor;

    linenr_T	ಸಾಲು = curwin->w_cursor.lnum;
    colnr_T	ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    ಪೂರ್ಣಾಂಕ		ಗಣ_curswant = curwin->w_ಗಣ_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    ಪೂರ್ಣಾಂಕ		old_VIsual_select = VIsual_select;
    ಪೂರ್ಣಾಂಕ		old_VIsual_ಸಕ್ರಿಯ = VIsual_ಸಕ್ರಿಯ;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_ಸಕ್ರಿಯ = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_ಬಫರ್;
	// skip original window and windows with 'nocursorbind'
	ಆದರೆ (curwin != old_curwin && curwin->w_p_crb)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    ಆದರೆ (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_ಸಾಲು(old_curbuf, ಸಾಲು);
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		curwin->w_cursor.lnum = ಸಾಲು;
	    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_ಗಣ_curswant = ಗಣ_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    ಪೂರ್ಣಾಂಕ restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    ಆದರೆ (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    ಆದರೆ (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    ಆದರೆ (!curwin->w_p_scb)
		update_topline();
	    curwin->w_redr_ಸ್ಥಿತಿ = true;
	}
    }

    /*
     * reset current-window
     */
    VIsual_select = old_VIsual_select;
    VIsual_ಸಕ್ರಿಯ = old_VIsual_ಸಕ್ರಿಯ;
    curwin = old_curwin;
    curbuf = old_curbuf;
}
