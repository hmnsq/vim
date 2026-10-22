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

#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର ଶୂନ୍ୟ redraw_for_cursorline(win_T *wp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scrolljump_ମୂଲ୍ୟ(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_ଶୀର୍ଷ_offset(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ curs_rows(win_T *wp);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    linenr_T	    lnum;	// line number
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    fill;	// filler lines
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    height;	// height of added line
} lineoff_T;

ସ୍ଥିର ଶୂନ୍ୟ topline_back(lineoff_T *lp);
ସ୍ଥିର ଶୂନ୍ୟ botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
adjust_plines_for_skipcol(win_T *wp)
{
    ଯଦି (wp->w_skipcol == 0)
	ଫେରନ୍ତୁ 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = wp->w_width - win_col_off(wp);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା w2 = width + win_col_off2(wp);
    ଯଦି (wp->w_skipcol >= width && w2 > 0)
	ଫେରନ୍ତୁ (wp->w_skipcol - width) / w2 + 1;

    ଫେରନ୍ତୁ 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plines_correct_topline(win_T *wp, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା limit_winheight)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
#ifdef FEAT_DIFF
    ଯଦି (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    ଅନ୍ୟଥା
#endif
	n = plines_win(wp, lnum, FALSE);
    ଯଦି (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    ଯଦି (limit_winheight && n > wp->w_height)
	n = wp->w_height;
    ଫେରନ୍ତୁ n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
comp_botline(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_cache;
#ifdef FEAT_FOLDING
    linenr_T    last;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		folded;
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
		    && wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid > 0
		    && wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[0].wl_lnum <= wp->w_topline;

    ଯଦି (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	done = wp->w_cline_row;
    }
    ଅନ୍ୟଥା
    {
	lnum = wp->w_topline;
	done = 0;
    }

    // Find the w_lines[] entry for the starting line.
    ଯଦି (use_cache)
	ଯେତେବେଳେ (i < wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid && wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lnum < lnum)
	    ++i;

    ପାଇଁ ( ; lnum <= wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା; ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	valid = FALSE;

#ifdef FEAT_FOLDING
	last = lnum;
	folded = FALSE;
#endif
	// Try to use the size from the previous redraw.
	ଯଦି (use_cache && i < wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid)
	{
	    ଯଦି (wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lnum < lnum || !wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_valid)
		ଜାରି;		// skip changed or deleted lines
	    ଯଦି (wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lnum == lnum)
		valid = TRUE;
	    ଅନ୍ୟଥା // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	ଯଦି (valid)
	{
	    n = wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_size;
#ifdef FEAT_FOLDING
	    folded = wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_folded;
	    last = wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lastlnum;
#endif
	}
	ଅନ୍ୟଥା
	{
#ifdef FEAT_FOLDING
	    ଯଦି (hasFoldingWin(wp, lnum, ଖାଲି, &last, TRUE, ଖାଲି))
	    {
		n = 1;
		folded = TRUE;
	    }
	    ଅନ୍ୟଥା
#endif
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	ଯଦି (
#ifdef FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && last >= wp->w_cursor.lnum
#ଅନ୍ୟଥା
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
	ଯଦି (done + n > wp->w_height)
	    ଭାଙ୍ଗ;
	done += n;
#ifdef FEAT_FOLDING
	lnum = last + 1;
#ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
redraw_for_cursorline(win_T *wp)
{
    ଯଦି ((wp->w_p_rnu
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
    ସ୍ଥିର ଶୂନ୍ୟ
redraw_for_cursorcolumn(win_T *wp)
{
    ଯଦି ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	ଯଦି (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	ଅନ୍ୟଥା ଯଦି (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#endif

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    ଶୂନ୍ୟ
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sms_marker_overlap(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra2)
{
    ଯଦି (extra2 == -1)
	extra2 = win_col_off(wp) - win_col_off2(wp);
#ଯଦି defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    ଯଦି (*get_showbreak_ମୂଲ୍ୟ(wp) != NUL)
	ଫେରନ୍ତୁ 0;
#endif
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    ଯଦି (wp->w_p_list && wp->w_lcs_chars.prec)
	ଫେରନ୍ତୁ 1;

    ଫେରନ୍ତୁ extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
skipcol_from_plines(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା plines_off)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା width1 = wp->w_width - win_col_off(wp);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା skipcol = 0;
    ଯଦି (plines_off > 0)
	skipcol += width1;
    ଯଦି (plines_off > 1)
	skipcol += (width1 + win_col_off2(wp)) * (plines_off - 1);
    ଫେରନ୍ତୁ skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
reset_skipcol(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_skipcol == 0)
	ଫେରନ୍ତୁ;

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
    ଶୂନ୍ୟ
update_topline_redraw(ଶୂନ୍ୟ)
{
    update_topline();
    ଯଦି (must_redraw)
	update_screen(0);
}

/*
 * Return true when 'scrolloffpad' may augment 'scrolloff'.
 * This only applies to automatic cursor visibility correction.
 * For now 'scrolloffpad' is treated as boolean: 0 disables, > 0 enables.
 */
    ସ୍ଥିର bool
use_scrolloffpad(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ get_scrolloff_ମୂଲ୍ୟ() > 0 && get_scrolloffpad_ମୂଲ୍ୟ() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    ସ୍ଥିର bool
scrolloffpad_eof_pressure(linenr_T lnum, long so)
{
    ଯଦି (!use_scrolloffpad() || so <= 0)
	ଫେରନ୍ତୁ false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    ଫେରନ୍ତୁ lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    ଶୂନ୍ୟ
update_topline(ଶୂନ୍ୟ)
{
    long	ଧାଡ଼ି_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		halfheight;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
#ifdef FEAT_FOLDING
    linenr_T	lnum;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_topline = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_botline = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    ଯଦି (skip_update_topline)
	ଫେରନ୍ତୁ;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    ଯଦି (!screen_valid(TRUE) || curwin->w_height == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	ଫେରନ୍ତୁ;
    }

    check_cursor_moved(curwin);
    ଯଦି (curwin->w_valid & VALID_TOPLINE)
	ଫେରନ୍ତୁ;

    // When dragging with the mouse, don't scroll that quickly
    ଯଦି (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_topfill = curwin->w_topfill;
#endif

    /*
     * If the buffer is empty, always set topline to 1.
     */
    ଯଦି (BUFEMPTY())		// special case - file is empty
    {
	ଯଦି (curwin->w_topline != 1)
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
    ଅନ୍ୟଥା
    {
	ଯଦି (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    ଯଦି (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    ଅନ୍ୟଥା ଯଦି (check_ଶୀର୍ଷ_offset())
		check_topline = TRUE;
	    ଅନ୍ୟଥା ଯଦି (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, ଖାଲି, ଖାଲି, 0);
		overlap = sms_marker_overlap(curwin, -1);
		ଯଦି (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#ifdef FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	ଯଦି (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#endif

	ଯଦି (check_topline)
	{
	    halfheight = curwin->w_height / 2 - 1;
	    ଯଦି (halfheight < 2)
		halfheight = 2;

#ifdef FEAT_FOLDING
	    ଯଦି (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		ପାଇଁ (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    ଯଦି (lnum >= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା || n >= halfheight)
			ଭାଙ୍ଗ;
		    (ଶୂନ୍ୟ)hasFolding(lnum, ଖାଲି, &lnum);
		}
	    }
	    ଅନ୍ୟଥା
#endif
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_scroll = scrolljump_ମୂଲ୍ୟ();
	    ଯଦି (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    ଅନ୍ୟଥା ଯଦି (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    ଅନ୍ୟଥା
	    {
		scroll_cursor_ଶୀର୍ଷ(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	ଅନ୍ୟଥା
	{
#ifdef FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (ଶୂନ୍ୟ)hasFolding(curwin->w_topline, &curwin->w_topline, ଖାଲି);
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
    ଯଦି (check_botline)
    {
	ଯଦି (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	ଯଦି (curwin->w_botline <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା || use_scrolloffpad())
	{
	    ଯଦି (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		ଯଦି (((long)curwin->w_cursor.lnum
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
		    (ଶୂନ୍ୟ)hasFolding(loff.lnum, ଖାଲି, &loff.lnum);
#endif
#ifdef FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_rows;
#endif
		    loff.height = 0;
		    ଯେତେବେଳେ (loff.lnum < curwin->w_botline
#ifdef FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#endif
			    )
		    {
			n += loff.height;
			ଯଦି (n >= *so_ptr)
			    ଭାଙ୍ଗ;
			botline_forw(&loff);
		    }
		    ଯଦି (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		ଅନ୍ୟଥା
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    ଯଦି (check_botline)
	    {
#ifdef FEAT_FOLDING
		ଯଦି (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    ଧାଡ଼ି_ଗଣନା = 0;
		    ପାଇଁ (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++ଧାଡ଼ି_ଗଣନା;
			// stop at end of file or when we know we are far off
			ଯଦି (lnum <= 0 || ଧାଡ଼ି_ଗଣନା > curwin->w_height + 1)
			    ଭାଙ୍ଗ;
			(ଶୂନ୍ୟ)hasFolding(lnum, &lnum, ଖାଲି);
		    }
		}
		ଅନ୍ୟଥା
#endif
		    ଧାଡ଼ି_ଗଣନା = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		ଯଦି (ଧାଡ଼ି_ଗଣନା <= curwin->w_height + 1)
		{
		    ଯଦି (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    ଅନ୍ୟଥା
			scroll_cursor_bot(scrolljump_ମୂଲ୍ୟ(), FALSE);
		}
		ଅନ୍ୟଥା
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    ଯଦି (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#endif
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	ଯଦି (!curwin->w_p_sms)
	    reset_skipcol();
	ଅନ୍ୟଥା ଯଦି (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	ଯଦି (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
scrolljump_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
    ଯଦି (p_sj >= 0)
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_sj;
    ଫେରନ୍ତୁ (curwin->w_height * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_ଶୀର୍ଷ_offset(ଶୂନ୍ୟ)
{
    lineoff_T	loff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    long	so = get_scrolloff_ମୂଲ୍ୟ();

    ଯଦି (curwin->w_cursor.lnum < curwin->w_topline + so
#ifdef FEAT_FOLDING
		    || hasAnyFolding(curwin)
#endif
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#ଅନ୍ୟଥା
	n = 0;
#endif
	// Count the visible screen lines above the cursor line.
	ଯେତେବେଳେ (n < so)
	{
	    topline_back(&loff);
	    // Stop when included a line above the window.
	    ଯଦି (loff.lnum < curwin->w_topline
#ifdef FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#endif
		    )
		ଭାଙ୍ଗ;
	    n += loff.height;
	}
	ଯଦି (n < so)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Update w_curswant.
 */
    ଶୂନ୍ୟ
update_curswant_force(ଶୂନ୍ୟ)
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
    ଶୂନ୍ୟ
update_curswant(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_set_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    ଶୂନ୍ୟ
check_cursor_moved(win_T *wp)
{
    ଯଦି (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ଅନ୍ୟଥା ଯଦି (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    ଅନ୍ୟଥା ଯଦି (wp->w_cursor.col != wp->w_valid_cursor.col
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
    ଶୂନ୍ୟ
changed_window_setting(ଶୂନ୍ୟ)
{
    changed_window_setting_win(curwin);
}

    ଶୂନ୍ୟ
changed_window_setting_win(win_T *wp)
{
    wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid = 0;
    changed_ଧାଡ଼ି_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    ଶୂନ୍ୟ
changed_window_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ଯଦି (wp->w_buffer == buf)
	    changed_window_setting_win(wp);
}
#endif

/*
 * Call changed_window_setting_win() for every window.
 */
    ଶୂନ୍ୟ
changed_window_setting_all(ଶୂନ୍ୟ)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	changed_window_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    ଶୂନ୍ୟ
set_topline(win_T *wp, linenr_T lnum)
{
#ifdef FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#endif

#ifdef FEAT_FOLDING
    // go to first of folded lines
    (ଶୂନ୍ୟ)hasFoldingWin(wp, lnum, &lnum, ଖାଲି, TRUE, ଖାଲି);
#endif
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    ଯଦି (wp->w_botline > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
	wp->w_botline = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_set = true;
#ifdef FEAT_DIFF
    ଯଦି (lnum != prev_topline)
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
    ଶୂନ୍ୟ
changed_cline_bef_curs(ଶୂନ୍ୟ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
changed_ଧାଡ଼ି_abv_curs(ଶୂନ୍ୟ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    ଶୂନ୍ୟ
changed_ଧାଡ଼ି_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    ଶୂନ୍ୟ
changed_ଧାଡ଼ି_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp->w_buffer == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#endif

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    ଶୂନ୍ୟ
validate_botline(ଶୂନ୍ୟ)
{
    validate_botline_win(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    ଶୂନ୍ୟ
validate_botline_win(win_T *wp)
{
    ଯଦି (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    ଶୂନ୍ୟ
invalidate_botline(ଶୂନ୍ୟ)
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    ଶୂନ୍ୟ
invalidate_botline_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    ଶୂନ୍ୟ
approximate_botline_win(
    win_T	*wp)
{
    wp->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cursor_valid(ଶୂନ୍ୟ)
{
    check_cursor_moved(curwin);
    ଫେରନ୍ତୁ ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    ଶୂନ୍ୟ
validate_cursor(ଶୂନ୍ୟ)
{
    check_cursor_lnum();
    check_cursor_moved(curwin);
    ଯଦି ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#ଯଦି defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    ଶୂନ୍ୟ
validate_cline_row(ଶୂନ୍ୟ)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    ଯଦି (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);
}
#endif

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
curs_rows(win_T *wp)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		all_invalid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		valid;
#ifdef FEAT_FOLDING
    long	fold_ଗଣନା;
#endif

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid == 0
			|| wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_row = 0;
    ପାଇଁ (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	ଯଦି (!all_invalid && i < wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid)
	{
	    ଯଦି (wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lnum < lnum || !wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_valid)
		ଜାରି;		// skip changed or deleted lines
	    ଯଦି (wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lnum == lnum)
	    {
#ifdef FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		ଯଦି (!wp->w_buffer->b_mod_set
			|| wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_buffer->b_mod_ଶୀର୍ଷ
					     > wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lastlnum + 1)
#endif
		valid = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	ଯଦି (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#ifdef FEAT_DIFF
					    && !wp->w_p_diff
#endif
	       )))
	{
#ifdef FEAT_FOLDING
	    lnum = wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    ଯଦି (lnum > wp->w_cursor.lnum)
		ଭାଙ୍ଗ;
#ଅନ୍ୟଥା
	    ++lnum;
#endif
	    wp->w_cline_row += wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_size;
	}
	ଅନ୍ୟଥା
	{
#ifdef FEAT_FOLDING
	    fold_ଗଣନା = folded_ଗଣନା(wp, lnum, ଖାଲି);
	    ଯଦି (fold_ଗଣନା)
	    {
		lnum += fold_ଗଣନା;
		ଯଦି (lnum > wp->w_cursor.lnum)
		    ଭାଙ୍ଗ;
		++wp->w_cline_row;
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		wp->w_cline_row += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    ଯଦି (!(wp->w_valid & VALID_CHEIGHT))
    {
	ଯଦି (all_invalid
		|| i == wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid
		|| (i < wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid
		    && (!wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_valid
			|| wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_lnum != wp->w_cursor.lnum)))
	{
#ifdef FEAT_DIFF
	    ଯଦି (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_height = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    ଅନ୍ୟଥା
#endif
		wp->w_cline_height = plines_win(wp, wp->w_cursor.lnum, TRUE);
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ଖାଲି, ଖାଲି, TRUE, ଖାଲି);
#endif
	}
	ଅନ୍ୟଥା ଯଦି (i > wp->w_ଧାଡ଼ିଗୁଡ଼ିକ_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_height = 0;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      ଖାଲି, ଖାଲି, TRUE, ଖାଲି);
#endif
	}
	ଅନ୍ୟଥା
	{
	    wp->w_cline_height = wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_size;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = wp->w_ଧାଡ଼ିଗୁଡ଼ିକ[i].wl_folded;
#endif
	}
    }

    redraw_for_cursorline(curwin);
    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;
}

/*
 * Validate curwin->w_virtcol only.
 */
    ଶୂନ୍ୟ
validate_virtcol(ଶୂନ୍ୟ)
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    ଶୂନ୍ୟ
validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);

    ଯଦି (wp->w_valid & VALID_VIRTCOL)
	ଫେରନ୍ତୁ;

#ifdef FEAT_PROP_POPUP
    wp->w_virtcol_first_char = 0;
#endif
    getvvcol(wp, &wp->w_cursor, ଖାଲି, &(wp->w_virtcol), ଖାଲି,
							 GETVCOL_FOR_VIRTCOL);
#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#endif
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    ଶୂନ୍ୟ
validate_cheight(ଶୂନ୍ୟ)
{
    check_cursor_moved(curwin);

    ଯଦି (curwin->w_valid & VALID_CHEIGHT)
	ଫେରନ୍ତୁ;

#ifdef FEAT_DIFF
    ଯଦି (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_height = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    ଅନ୍ୟଥା
#endif
	curwin->w_cline_height = plines(curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, ଖାଲି, ଖାଲି);
#endif
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    ଶୂନ୍ୟ
validate_cursor_col(ଶୂନ୍ୟ)
{
    colnr_T off;
    colnr_T col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     width;

    validate_virtcol();

    ଯଦି (curwin->w_valid & VALID_WCOL)
	ଫେରନ୍ତୁ;

    col = curwin->w_virtcol;
    off = curwin_col_off();
    col += off;
    width = curwin->w_width - off + curwin_col_off2();

    // long line wrapping, adjust curwin->w_wrow
    ଯଦି (curwin->w_p_wrap
	    && col >= (colnr_T)curwin->w_width
	    && width > 0)
	// use same formula as what is used in curs_columns()
	col -= ((col - curwin->w_width) / width + 1) * width;
    ଯଦି (col > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_leftcol)
	col -= curwin->w_leftcol;
    ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_col_off(win_T *wp)
{
    ଫେରନ୍ତୁ (((wp->w_p_nu || wp->w_p_rnu) ? ସଂଖ୍ୟା_width(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#ifdef FEAT_FOLDING
	    + wp->w_p_fdc
#endif
#ifdef FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#endif
	   );
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
curwin_col_off(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ win_col_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_col_off2(win_T *wp)
{
    ଯଦି ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != ଖାଲି)
	ଫେରନ୍ତୁ ସଂଖ୍ୟା_width(wp) + 1;
    ଫେରନ୍ତୁ 0;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
curwin_col_off2(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ win_col_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    ଶୂନ୍ୟ
curs_columns(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		may_scroll)	// when TRUE, may scroll horizontally
{
    long	diff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra;		// offset for first screen line
    long	off_ବାମ, off_ଡାହାଣ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		p_ଧାଡ଼ିଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width1;		// text width for first screen line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width2 = 0;	// text width for second and later screen line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    long	so = get_scrolloff_ମୂଲ୍ୟ();
    long	siso = get_sidescrolloff_ମୂଲ୍ୟ();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    ଯଦି (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);

#ifdef FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_first_char = 0;
#endif

    /*
     * Compute the number of virtual columns.
     */
#ifdef FEAT_FOLDING
    ଯଦି (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    ଅନ୍ୟଥା
#endif
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    ଯଦି (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_col_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_row;

    width1 = curwin->w_width - extra;
    ଯଦି (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_width - 1;
	ଯଦି (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_height - 1;
	ଅନ୍ୟଥା
	    curwin->w_wrow = curwin->w_height - 1 - curwin->w_empty_rows;
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_p_wrap && curwin->w_width != 0)
    {
	width2 = width1 + curwin_col_off2();

	// skip columns that are not visible
	ଯଦି (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    ଯଦି (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    ଅନ୍ୟଥା
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	ଯଦି (curwin->w_wcol >= curwin->w_width)
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
    ଅନ୍ୟଥା ଯଦି (may_scroll
#ifdef FEAT_FOLDING
	    && !curwin->w_cline_folded
#endif
	    )
    {
#ifdef FEAT_PROP_POPUP
	ଯଦି (curwin->w_virtcol_first_char > 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols = (curwin->w_width - extra);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows = cols > 0 ? curwin->w_virtcol_first_char / cols : 1;

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
	off_ବାମ = startcol - curwin->w_leftcol - siso;
	off_ଡାହାଣ = endcol - curwin->w_leftcol - (curwin->w_width - siso) + 1;
	ଯଦି (off_ବାମ < 0 || off_ଡାହାଣ > 0)
	{
	    ଯଦି (off_ବାମ < 0)
		diff = -off_ବାମ;
	    ଅନ୍ୟଥା
		diff = off_ଡାହାଣ;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    ଯଦି (p_ss == 0 || diff >= width1 / 2 || off_ଡାହାଣ >= off_ବାମ)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    ଅନ୍ୟଥା
	    {
		ଯଦି (diff < p_ss)
		    diff = p_ss;
		ଯଦି (off_ବାମ < 0)
		    new_leftcol = curwin->w_leftcol - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)diff;
		ଅନ୍ୟଥା
		    new_leftcol = curwin->w_leftcol + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)diff;
	    }
	    ଯଦି (new_leftcol < 0)
		new_leftcol = 0;
	    ଯଦି (new_leftcol != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    ଅନ୍ୟଥା ଯଦି (curwin->w_wcol > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    ଅନ୍ୟଥା
	curwin->w_wcol = 0;

#ifdef FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    ଯଦି (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    ଅନ୍ୟଥା
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#endif

    prev_skipcol = curwin->w_skipcol;

    p_ଧାଡ଼ିଗୁଡ଼ିକ = 0;

    ଯଦି ((curwin->w_wrow >= curwin->w_height
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_height)
		    && (p_ଧାଡ଼ିଗୁଡ଼ିକ =
#ifdef FEAT_DIFF
			plines_win_nofill
#ଅନ୍ୟଥା
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
	ଯଦି (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	ଯଦି (p_ଧାଡ଼ିଗୁଡ଼ିକ == 0)
	    p_ଧାଡ଼ିଗୁଡ଼ିକ = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_ଧାଡ଼ିଗୁଡ଼ିକ;
	ଯଦି (p_ଧାଡ଼ିଗୁଡ଼ିକ > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	ଅନ୍ୟଥା
	    n = p_ଧାଡ଼ିଗୁଡ଼ିକ;
	ଯଦି ((colnr_T)n >= curwin->w_height + curwin->w_skipcol / width2 - so)
	    extra += 2;

	ଯଦି (extra == 3 || curwin->w_height <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    ଯଦି (n > curwin->w_height / 2)
		n -= curwin->w_height / 2;
	    ଅନ୍ୟଥା
		n = 0;
	    // don't skip more than necessary
	    ଯଦି (n > p_ଧାଡ଼ିଗୁଡ଼ିକ - curwin->w_height + 1)
		n = p_ଧାଡ଼ିଗୁଡ଼ିକ - curwin->w_height + 1;
	    ଯଦି (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    ଅନ୍ୟଥା
		curwin->w_skipcol = 0;
	}
	ଅନ୍ୟଥା ଯଦି (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    ଯଦି (extra > 0)
	    {
		ଯଦି ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_height + 1) * width2;
	    ଯେତେବେଳେ (endcol > curwin->w_virtcol)
		endcol -= width2;
	    ଯଦି (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	ଯଦି (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - prev_skipcol) / width2;
	ଅନ୍ୟଥା
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	ଯଦି (curwin->w_wrow >= curwin->w_height)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_height + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prev_skipcol - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_skipcol) / width2;
	ଯଦି (extra > 0)
	    win_ins_ଧାଡ଼ିଗୁଡ଼ିକ(curwin, 0, extra, FALSE, FALSE);
	ଅନ୍ୟଥା ଯଦି (extra < 0)
	    win_del_ଧାଡ଼ିଗୁଡ଼ିକ(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    ଅନ୍ୟଥା ଯଦି (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    ଯଦି (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#endif
#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ଯଦି (popup_is_popup(curwin) && curbuf->b_term != ଖାଲି)
    {
	curwin->w_wrow += popup_ଶୀର୍ଷ_extra(curwin);
	curwin->w_wcol += popup_ବାମ_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    ଅନ୍ୟଥା
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#endif

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    ଶୂନ୍ୟ
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*rowp,	// screen row
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*scolp,	// start screen column
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*ccolp,	// cursor screen column
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = 0;
    colnr_T	coloff = 0;

    ଯଦି (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, ଖାଲି, TRUE, ଖାଲି);
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
	ଯଦି (is_folded)
	{
	    row += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	ଅନ୍ୟଥା
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    col = scol;
	    col += off;
	    width = wp->w_width - off + win_col_off2(wp);

	    // long line wrapping, adjust row
	    ଯଦି (wp->w_p_wrap
		    && col >= (colnr_T)wp->w_width
		    && width > 0)
	    {
		// use same formula as what is used in curs_columns()
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା rowoff = ((col - wp->w_width) / width + 1);
		col -= rowoff * width;
		row += rowoff;
	    }
	    col -= wp->w_leftcol;
	    ଯଦି (col >= wp->w_width)
		col = -1;
	    ଯଦି (col >= 0 && row >= 0 && row < wp->w_height)
	    {
		coloff = col - scol + wp->w_wincol + 1;
		row += W_WINROW(wp) + 1;
	    }
	    ଅନ୍ୟଥା
		// character is out of the window
		row = scol = ccol = ecol = 0;
	}
    }
    *rowp = row;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    ଯଦି (wp->w_p_rl && row > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_width - endoff;
    }
# endif
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    ଶୂନ୍ୟ
f_screenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scol = 0, ccol = 0, ecol = 0;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    dict = rettv->vval.v_dict;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    pos.lnum = tv_get_ସଂଖ୍ୟା(&argvars[1]);
    ଯଦି (pos.lnum > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	semsg(_(e_invalid_ଧାଡ଼ି_ସଂଖ୍ୟା_nr), pos.lnum);
	ଫେରନ୍ତୁ;
    }
    pos.col = tv_get_ସଂଖ୍ୟା(&argvars[2]) - 1;
    ଯଦି (pos.col < 0)
	pos.col = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &row, &scol, &ccol, &ecol);

    dict_add_ସଂଖ୍ୟା(dict, "row", row);
    dict_add_ସଂଖ୍ୟା(dict, "col", scol);
    dict_add_ସଂଖ୍ୟା(dict, "curscol", ccol);
    dict_add_ସଂଖ୍ୟା(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
virtcol2col(win_T *wp, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vcol)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		offset = vcol2col(wp, lnum, vcol - 1, ଖାଲି);
    char_u	*ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);
    char_u	*p = ଧାଡ଼ି + offset;

    ଯଦି (*p == NUL)
    {
	ଯଦି (p == ଧାଡ଼ି)  // empty line
	    ଫେରନ୍ତୁ 0;
	// Move to the first byte of the last char.
	MB_PTR_BACK(ଧାଡ଼ି, p);
    }
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - ଧାଡ଼ି + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    ଶୂନ୍ୟ
f_virtcol2col(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		screencol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
	    || check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
	    || check_for_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ;

    lnum = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
    ଯଦି (error || lnum < 0 || lnum > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ;

    screencol = tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
    ଯଦି (error || screencol < 0)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = virtcol2col(wp, lnum, screencol);
}
#endif

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
ସ୍ଥିର ଶୂନ୍ୟ cursor_correct_sms(ଶୂନ୍ୟ)
{
    ଯଦି (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	ଫେରନ୍ତୁ;

    long    so = get_scrolloff_ମୂଲ୍ୟ();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width1 = curwin->w_width - curwin_col_off();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width2 = width1 + curwin_col_off2();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    so_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    space_cols = (curwin->w_height - 1) * width2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    overlap, ଶୀର୍ଷ, bot;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    size = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    ଯଦି (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_cols = 0;               // Ignore 'scrolloff' at top of buffer.
    ଅନ୍ୟଥା ଯଦି (so_cols > space_cols / 2)
	so_cols = space_cols / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    ଯେତେବେଳେ (so_cols > size && so_cols - width2 >= width1 && width1 > 0)
	so_cols -= width2;
    ଯଦି (so_cols >= width1 && so_cols > size)
	so_cols -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_width - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    ଶୀର୍ଷ = curwin->w_skipcol + (so_cols != 0 ? so_cols : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_height - 1) * width2
								    - so_cols;
    validate_virtcol();
    colnr_T col = curwin->w_virtcol;

    ଯଦି (col < ଶୀର୍ଷ)
    {
	ଯଦି (col < width1)
	    col += width1;
	ଯେତେବେଳେ (width2 > 0 && col < ଶୀର୍ଷ)
	    col += width2;
    }
    ଅନ୍ୟଥା
	ଯେତେବେଳେ (width2 > 0 && col >= bot)
	    col -= width2;

    ଯଦି (col != curwin->w_virtcol)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	curwin->w_curswant = col;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	ଯଦି (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    validate_virtcol();
	    ଯଦି (curwin->w_virtcol < curwin->w_skipcol + overlap)
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
    ଶୂନ୍ୟ
scroll_redraw(ପୂର୍ଣ୍ଣସଂଖ୍ୟା up, long ଗଣନା_2)
{
    linenr_T	prev_topline = curwin->w_topline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_topfill = curwin->w_topfill;
#endif
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    ଯଦି (up)
	scrollup(ଗଣନା_2, TRUE);
    ଅନ୍ୟଥା
	scrolldown(ଗଣନା_2, TRUE);
    ଯଦି (get_scrolloff_ମୂଲ୍ୟ() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	ଯେତେବେଳେ (curwin->w_topline == prev_topline
		&& curwin->w_skipcol == prev_skipcol
#ifdef FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#endif
		)
	{
	    ଯଦି (up)
	    {
		ଯଦି (curwin->w_cursor.lnum > prev_lnum
			|| cursor_down(1L, FALSE) == FAIL)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_up(1L, FALSE) == FAIL)
		    ଭାଙ୍ଗ;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    ଯଦି (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    ଶୂନ୍ୟ
scrolldown(
    long	ଧାଡ଼ି_ଗଣନା,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    long	done = 0;	// total # of physical lines done
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		moved = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width1 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width2 = 0;

    ଯଦି (do_sms)
    {
	width1 = curwin->w_width - curwin_col_off();
	width2 = width1 + curwin_col_off2();
    }

#ifdef FEAT_FOLDING
    linenr_T	first;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (ଶୂନ୍ୟ)hasFolding(curwin->w_topline, &curwin->w_topline, ଖାଲି);
#endif
    validate_cursor();		// w_wrow needs to be valid
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା todo = ଧାଡ଼ି_ଗଣନା; todo > 0; --todo)
    {
#ifdef FEAT_DIFF
	ଯଦି (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_height - 1)
	{
	    ++curwin->w_topfill;
	    ++done;
	}
	ଅନ୍ୟଥା
#endif
	{
	    // break when at the very top
	    ଯଦି (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		ଭାଙ୍ଗ;
	    ଯଦି (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		ଯଦି (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		ଅନ୍ୟଥା
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++done;
	    }
	    ଅନ୍ୟଥା
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#ifdef FEAT_DIFF
		curwin->w_topfill = 0;
#endif
#ifdef FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		ଯଦି (hasFolding(curwin->w_topline, &first, ଖାଲି))
		{
		    ++done;
		    ଯଦି (!byfold)
			todo -= curwin->w_topline - first - 1;
		    curwin->w_botline -= curwin->w_topline - first;
		    curwin->w_topline = first;
		}
		ଅନ୍ୟଥା
#endif
		ଯଦି (do_sms)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = linetabsize_eol(curwin, curwin->w_topline);
		    ଯଦି (size > width1)
		    {
			curwin->w_skipcol = width1;
			size -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    ଯେତେବେଳେ (size > width2)
		    {
			curwin->w_skipcol += width2;
			size -= width2;
		    }
		    ++done;
		}
		ଅନ୍ୟଥା
		    done += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += done;		// keep w_wrow updated
    curwin->w_cline_row += done;	// keep w_cline_row updated

#ifdef FEAT_DIFF
    ଯଦି (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_row = 0;
    check_topfill(curwin, TRUE);
#endif

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    ଯଦି (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    ଯେତେବେଳେ (wrow >= curwin->w_height && curwin->w_cursor.lnum > 1)
    {
#ifdef FEAT_FOLDING
	ଯଦି (hasFolding(curwin->w_cursor.lnum, &first, ଖାଲି))
	{
	    --wrow;
	    ଯଦି (first == 1)
		curwin->w_cursor.lnum = 1;
	    ଅନ୍ୟଥା
		curwin->w_cursor.lnum = first - 1;
	}
	ଅନ୍ୟଥା
#endif
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    ଯଦି (moved)
    {
#ifdef FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#endif
	coladvance(curwin->w_curswant);
    }
    ଯଦି (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    ଶୂନ୍ୟ
scrollup(
    long	ଧାଡ଼ି_ଗଣନା,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    ଯଦି (do_sms
#ifdef FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#endif
#ifdef FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#endif
       )
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width1 = curwin->w_width - curwin_col_off();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width2 = width1 + curwin_col_off2();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    size = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	ଯଦି (do_sms)
	    size = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା todo = ଧାଡ଼ି_ଗଣନା; todo > 0; --todo)
	{
#ifdef FEAT_DIFF
	    ଯଦି (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    ଅନ୍ୟଥା
#endif
	    {
		linenr_T lnum = curwin->w_topline;

#ifdef FEAT_FOLDING
		ଯଦି (byfold)
		    // for a closed fold: go to the last line in the fold
		    (ଶୂନ୍ୟ)hasFolding(lnum, ଖାଲି, &lnum);
#endif
		ଯଦି (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା add = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += add;
		    ଯଦି (curwin->w_skipcol >= size)
		    {
			ଯଦି (lnum == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= add;
			    ଭାଙ୍ଗ;
			}
			++lnum;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (lnum >= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
			ଭାଙ୍ଗ;
		    ++lnum;
		}

		ଯଦି (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#ifdef FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#endif
		    curwin->w_skipcol = 0;
		    ଯଦି (todo > 1 && do_sms)
			size = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	ଯଦି (prev_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    ଅନ୍ୟଥା
    {
	curwin->w_topline += ଧାଡ଼ି_ଗଣନା;
	curwin->w_botline += ଧାଡ଼ି_ଗଣନା;	// approximate w_botline
    }

    ଯଦି (curwin->w_topline > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	curwin->w_topline = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଯଦି (curwin->w_botline > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
	curwin->w_botline = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1;

#ifdef FEAT_DIFF
    check_topfill(curwin, FALSE);
#endif

#ifdef FEAT_FOLDING
    ଯଦି (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(ଶୂନ୍ୟ)hasFolding(curwin->w_topline, &curwin->w_topline, ଖାଲି);
#endif

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    ଯଦି (curwin->w_cursor.lnum < curwin->w_topline)
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
    ଶୂନ୍ୟ
adjust_skipcol(ଶୂନ୍ୟ)
{
    ଯଦି (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width1 = curwin->w_width - curwin_col_off();
    ଯଦି (width1 <= 0)
	ଫେରନ୍ତୁ;  // no text will be displayed

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width2 = width1 + curwin_col_off2();
    long    so = get_scrolloff_ମୂଲ୍ୟ();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    scrolloff_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    scrolled = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    row = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    overlap, col;

    validate_cheight();
    ଯଦି (curwin->w_cline_height == curwin->w_height
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_height)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	ଫେରନ୍ତୁ;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_width - width2);
    ଯେତେବେଳେ (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_cols)
    {
	// scroll a screen line down
	ଯଦି (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	ଅନ୍ୟଥା
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    ଯଦି (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	ଫେରନ୍ତୁ;  // don't scroll in the other direction now
    }

    col = curwin->w_virtcol + scrolloff_cols;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    ଯଦି (scrolloff_cols > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = linetabsize_eol(curwin, curwin->w_topline);
	size = width1 + width2 * ((size - width1 + width2 - 1) / width2);
	ଯେତେବେଳେ (col > size)
	    col -= width2;
    }
    col -= curwin->w_skipcol;

    ଯଦି (col >= width1)
    {
	col -= width1;
	++row;
    }
    ଯଦି (col > width2)
    {
	row += col / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	col = col % width2;
    }
    ଯଦି (row >= curwin->w_height)
    {
	ଯଦି (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --row;
	}
	ଯଦି (row >= curwin->w_height)
	    curwin->w_skipcol += (row - curwin->w_height) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#ifdef FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    ଶୂନ୍ୟ
check_topfill(
    win_T	*wp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		down)	// when TRUE scroll down when not enough space
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (wp->w_topfill <= 0)
	ଫେରନ୍ତୁ;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    ଯଦି (wp->w_topfill + n > wp->w_height)
    {
	ଯଦି (down && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	ଅନ୍ୟଥା
	{
	    wp->w_topfill = wp->w_height - n;
	    ଯଦି (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#endif

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    ଶୂନ୍ୟ
scrolldown_clamp(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_row;
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#endif

    ଯଦି (curwin->w_topline <= 1
#ifdef FEAT_DIFF
	    && !can_fill
#endif
	    )
	ଫେରନ୍ତୁ;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    end_row = curwin->w_wrow;
#ifdef FEAT_DIFF
    ଯଦି (can_fill)
	++end_row;
    ଅନ୍ୟଥା
	end_row += plines_nofill(curwin->w_topline - 1);
#ଅନ୍ୟଥା
    end_row += plines(curwin->w_topline - 1);
#endif
    ଯଦି (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_cheight();
	validate_virtcol();
	end_row += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    ଯଦି (end_row < curwin->w_height - get_scrolloff_ମୂଲ୍ୟ())
    {
#ifdef FEAT_DIFF
	ଯଦି (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	ଅନ୍ୟଥା
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#ଅନ୍ୟଥା
	--curwin->w_topline;
#endif
#ifdef FEAT_FOLDING
	(ଶୂନ୍ୟ)hasFolding(curwin->w_topline, &curwin->w_topline, ଖାଲି);
#endif
	--curwin->w_botline;	    // approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    ଶୂନ୍ୟ
scrollup_clamp(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    start_row;

    ଯଦି (curwin->w_topline == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
#ifdef FEAT_DIFF
	    && curwin->w_topfill == 0
#endif
	    )
	ଫେରନ୍ତୁ;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#ifdef FEAT_DIFF
    start_row = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#ଅନ୍ୟଥା
    start_row = curwin->w_wrow - plines(curwin->w_topline);
#endif
    ଯଦି (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	start_row -= curwin->w_virtcol / curwin->w_width;
    }
    ଯଦି (start_row >= get_scrolloff_ମୂଲ୍ୟ())
    {
#ifdef FEAT_DIFF
	ଯଦି (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	ଅନ୍ୟଥା
#endif
	{
#ifdef FEAT_FOLDING
	    (ଶୂନ୍ୟ)hasFolding(curwin->w_topline, ଖାଲି, &curwin->w_topline);
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
    ସ୍ଥିର ଶୂନ୍ୟ
topline_back_winheight(
    lineoff_T	*lp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winheight)	// when TRUE limit to window height
{
#ifdef FEAT_DIFF
    ଯଦି (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    ଅନ୍ୟଥା
#endif
    {
	--lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	ଯଦି (lp->lnum < 1)
	    lp->height = MAXCOL;
	ଅନ୍ୟଥା
#ifdef FEAT_FOLDING
	    ଯଦି (hasFolding(lp->lnum, &lp->lnum, ଖାଲି))
	    // Add a closed fold
	    lp->height = 1;
	ଅନ୍ୟଥା
#endif
	    lp->height = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
botline_forw(lineoff_T *lp)
{
#ifdef FEAT_DIFF
    ଯଦି (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    ଅନ୍ୟଥା
#endif
    {
	++lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	ଯଦି (lp->lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    lp->height = MAXCOL;
	ଅନ୍ୟଥା
#ifdef FEAT_FOLDING
	    ଯଦି (hasFolding(lp->lnum, ଖାଲି, &lp->lnum))
	    {
		// Add a closed fold.
		lp->height = 1;
	    }
	    ଅନ୍ୟଥା
#endif
	    lp->height = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    ଶୂନ୍ୟ
scroll_cursor_ଶୀର୍ଷ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_scroll, ପୂର୍ଣ୍ଣସଂଖ୍ୟା always)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scrolled = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		used;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    linenr_T	ଶୀର୍ଷ;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#endif
    linenr_T	new_topline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off = get_scrolloff_ମୂଲ୍ୟ();

    ଯଦି (mouse_dragging > 0)
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
    ଯଦି (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = used;

#ifdef FEAT_FOLDING
    ଯଦି (hasFolding(curwin->w_cursor.lnum, &ଶୀର୍ଷ, &bot))
    {
	--ଶୀର୍ଷ;
	++bot;
    }
    ଅନ୍ୟଥା
#endif
    {
	ଶୀର୍ଷ = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = ଶୀର୍ଷ + 1;

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
    ଯେତେବେଳେ (ଶୀର୍ଷ > 0)
    {
#ifdef FEAT_FOLDING
	ଯଦି (hasFolding(ଶୀର୍ଷ, &ଶୀର୍ଷ, ଖାଲି))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	ଅନ୍ୟଥା
#endif
	    i = PLINES_NOFILL(ଶୀର୍ଷ);
	ଯଦି (ଶୀର୍ଷ < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	ଯଦି ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    ଭାଙ୍ଗ;

	used += i;
	ଯଦି (extra + i <= off && bot < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
#ifdef FEAT_FOLDING
	    ଯଦି (hasFolding(bot, ଖାଲି, &bot))
		// count one logical line for a sequence of folded lines
		++used;
	    ଅନ୍ୟଥା
#endif
		used += plines(bot);
	}
	ଯଦି (used > curwin->w_height)
	    ଭାଙ୍ଗ;

	extra += i;
	new_topline = ଶୀର୍ଷ;
	--ଶୀର୍ଷ;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    ଯଦି (used > curwin->w_height)
	scroll_cursor_halfway(FALSE, FALSE);
    ଅନ୍ୟଥା
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	ଯଦି (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	ଯଦି (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	ଯଦି (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    ଯଦି (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#endif
	ଯଦି (curwin->w_topline != old_topline)
	    reset_skipcol();
	ଅନ୍ୟଥା ଯଦି (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    ଯଦି (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	width1 = curwin->w_width - curwin_col_off();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	width2 = width1 + curwin_col_off2();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	plines_off = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	skipcol;

		ଯଦି (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		ଯଦି (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	ଯଦି (curwin->w_topline != old_topline
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
    ଶୂନ୍ୟ
set_empty_rows(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା used)
{
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    ଯଦି (used == 0)
	wp->w_empty_rows = 0;	// single line that doesn't fit
    ଅନ୍ୟଥା
    {
	wp->w_empty_rows = wp->w_height - used;
#ifdef FEAT_DIFF
	ଯଦି (wp->w_botline <= wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    wp->w_filler_rows = diff_check_fill(wp, wp->w_botline);
	    ଯଦି (wp->w_empty_rows > wp->w_filler_rows)
		wp->w_empty_rows -= wp->w_filler_rows;
	    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
scroll_cursor_bot(ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_scroll, ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_topbot)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		used;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scrolled = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    linenr_T	ଧାଡ଼ି_ଗଣନା;
    linenr_T	old_topline = curwin->w_topline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_topfill = curwin->w_topfill;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fill_below_window;
#endif
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_empty_rows = curwin->w_empty_rows;
    linenr_T	cln;		    // Cursor Line Number
    long	so = get_scrolloff_ମୂଲ୍ୟ();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    ଯଦି (set_topbot)
    {
	used = 0;
	linenr_T cln_last = cln;
#ifdef FEAT_FOLDING
	(ଶୂନ୍ୟ)hasFolding(cln, ଖାଲି, &cln_last);
#endif
	curwin->w_botline = cln_last + 1;
	loff.lnum = cln_last + 1;
#ifdef FEAT_DIFF
	loff.fill = 0;
#endif
	ଯେତେବେଳେ (TRUE)
	{
	    topline_back_winheight(&loff, FALSE);
	    ଯଦି (loff.height == MAXCOL)
		ଭାଙ୍ଗ;
	    ଯଦି (used + loff.height > curwin->w_height)
	    {
		ଯଦି (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    ଯଦି (used < curwin->w_height)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା plines_offset = used + loff.height
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
		ଭାଙ୍ଗ;
	    }
#ifdef FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#endif
	    curwin->w_topline = loff.lnum;
	    used += loff.height;
	}

	set_empty_rows(curwin, used);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	ଯଦି (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#endif
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    ଯଦି (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    ଅନ୍ୟଥା
		reset_skipcol();
	}
    }
    ଅନ୍ୟଥା
	validate_botline();

    // The lines of the cursor line itself are always used.
#ifdef FEAT_DIFF
    used = plines_nofill(cln);
#ଅନ୍ୟଥା
    validate_cheight();
    used = curwin->w_cline_height;
#endif

    ଯଦି (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା upto_cursor = plines_win_col(curwin, cln, curwin->w_cursor.col);

	ଯଦି (upto_cursor < used)
	    used = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    ଯଦି (cln >= curwin->w_botline)
    {
	scrolled = used;
	ଯଦି (cln == curwin->w_botline)
	    scrolled -= curwin->w_empty_rows;
	ଯଦି (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ_plines =
#ifdef FEAT_DIFF
			    plines_win_nofill
#ଅନ୍ୟଥା
			    plines_win
#endif
					(curwin, curwin->w_topline, FALSE);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା width1 = curwin->w_width - curwin_col_off();

	    ଯଦି (width1 > 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width2 = width1 + curwin_col_off2();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip_ଧାଡ଼ିଗୁଡ଼ିକ = 0;

		// A similar formula is used in curs_columns().
		ଯଦି (curwin->w_skipcol > width1)
		    skip_ଧାଡ଼ିଗୁଡ଼ିକ += (curwin->w_skipcol - width1) / width2 + 1;
		ଅନ୍ୟଥା ଯଦି (curwin->w_skipcol > 0)
		    skip_ଧାଡ଼ିଗୁଡ଼ିକ = 1;

		ଶୀର୍ଷ_plines -= skip_ଧାଡ଼ିଗୁଡ଼ିକ;
		ଯଦି (ଶୀର୍ଷ_plines > curwin->w_height)
		{
		    scrolled += (ଶୀର୍ଷ_plines - curwin->w_height);
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
    ଯଦି (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
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

    ଯେତେବେଳେ (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	ଯଦି ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		&& loff.lnum <= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_below_window)
#endif
		)
	    ଭାଙ୍ଗ;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_back(&loff);
	ଯଦି (loff.height == MAXCOL)
	    used = MAXCOL;
	ଅନ୍ୟଥା
	    used += loff.height;
	ଯଦି (used > curwin->w_height)
	    ଭାଙ୍ଗ;
	ଯଦି (loff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_below_window)
#endif
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.height;
	    ଯଦି (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_empty_rows;
	}

	ଯଦି (boff.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    used += boff.height;
	    ଯଦି (used > curwin->w_height)
		ଭାଙ୍ଗ;
	    ଯଦି (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.height;
		ଯଦି (boff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_rows)
#endif
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.height;
		    ଯଦି (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_empty_rows;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    ଯଦି (scrolled <= 0)
	ଧାଡ଼ି_ଗଣନା = 0;
    // more than a screenfull, don't scroll but redraw
    ଅନ୍ୟଥା ଯଦି (used > curwin->w_height)
	ଧାଡ଼ି_ଗଣନା = used;
    // scroll minimal number of lines
    ଅନ୍ୟଥା
    {
	ଧାଡ଼ି_ଗଣନା = 0;
#ifdef FEAT_DIFF
	boff.fill = curwin->w_topfill;
#endif
	boff.lnum = curwin->w_topline - 1;
	ପାଇଁ (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.height;
	    ++ଧାଡ଼ି_ଗଣନା;
	}
	ଯଦି (i < scrolled)	// below curwin->w_botline, don't scroll
	    ଧାଡ଼ି_ଗଣନା = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    ଯଦି (ଧାଡ଼ି_ଗଣନା >= curwin->w_height && ଧାଡ଼ି_ଗଣନା > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି_ଗଣନା > 0)
    {
	ଯଦି (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	ଅନ୍ୟଥା
	    scrollup(ଧାଡ଼ି_ଗଣନା, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    ଯଦି (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && set_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_empty_rows = old_empty_rows;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    ଯଦି (set_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    ଶୂନ୍ୟ
scroll_cursor_halfway(ପୂର୍ଣ୍ଣସଂଖ୍ୟା atend, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prefer_above)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		topfill = 0;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		below = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		used;
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
    (ଶୂନ୍ୟ)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#endif
#ifdef FEAT_DIFF
    used = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#ଅନ୍ୟଥା
    used = plines(loff.lnum);
#endif
    topline = loff.lnum;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ଯଦି (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	ଯଦି (atend)
	{
	    want_height = (curwin->w_height - used) / 2;
	    used = 0;
	}
	ଅନ୍ୟଥା
	    want_height = curwin->w_height;
    }

    ଯେତେବେଳେ (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	ଯଦି (do_sms)
	{
	    topline_back_winheight(&loff, FALSE);
	    ଯଦି (loff.height == MAXCOL)
		ଭାଙ୍ଗ;
	    used += loff.height;
	    ଯଦି (!atend && boff.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    {
		botline_forw(&boff);
		used += boff.height;
	    }
	    ଯଦି (used > want_height)
	    {
		ଯଦି (used - loff.height < want_height)
		{
		    topline = loff.lnum;
#ifdef FEAT_DIFF
		    topfill = loff.fill;
#endif
		    skipcol = skipcol_from_plines(curwin, used - want_height);
		}
		ଭାଙ୍ଗ;
	    }
	    topline = loff.lnum;
#ifdef FEAT_DIFF
	    topfill = loff.fill;
#endif
	    ଜାରି;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା done = FALSE;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା round = 1; round <= 2; ++round)
	{
	    ଯଦି (prefer_above ? (round == 2 && below < above)
			     : (round == 1 && below <= above))
	    {
		// add a line below the cursor
		ଯଦି (boff.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		{
		    botline_forw(&boff);
		    used += boff.height;
		    ଯଦି (used > curwin->w_height)
		    {
			done = TRUE;
			ଭାଙ୍ଗ;
		    }
		    below += boff.height;
		}
		ଅନ୍ୟଥା
		{
		    ++below;	    // count a "~" line
		    ଯଦି (atend)
			++used;
		}
	    }

	    ଯଦି (prefer_above ? (round == 1 && below >= above)
			     : (round == 1 && below > above))
	    {
		// add a line above the cursor
		topline_back(&loff);
		ଯଦି (loff.height == MAXCOL)
		    used = MAXCOL;
		ଅନ୍ୟଥା
		    used += loff.height;
		ଯଦି (used > curwin->w_height)
		{
		    done = TRUE;
		    ଭାଙ୍ଗ;
		}
		above += loff.height;
		topline = loff.lnum;
#ifdef FEAT_DIFF
		topfill = loff.fill;
#endif
	    }
	}
	ଯଦି (done)
	    ଭାଙ୍ଗ;
    }

#ifdef FEAT_FOLDING
    ଯଦି (!hasFolding(topline, &curwin->w_topline, ଖାଲି))
#endif
    {
	ଯଦି (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    ଯଦି (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    ଅନ୍ୟଥା ଯଦି (do_sms)
		reset_skipcol();
	}
    }
#ifdef FEAT_DIFF
    curwin->w_topfill = topfill;
    ଯଦି (old_topline > curwin->w_topline + curwin->w_height)
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
    ଶୂନ୍ୟ
cursor_correct(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		above = 0;	    // screen lines above topline
    linenr_T	topline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		below = 0;	    // screen lines below botline
    linenr_T	botline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		above_wanted, below_wanted;
    linenr_T	cln;		    // Cursor Line Number
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_off;
    long	so = get_scrolloff_ମୂଲ୍ୟ();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    below_wanted = so;
    ଯଦି (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	below_wanted = mouse_dragging - 1;
    }
    ଯଦି (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_height / 2;
	ଯଦି (below_wanted > max_off)
	    below_wanted = max_off;
    }
    validate_botline();
    ଯଦି (curwin->w_botline == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	below_wanted = 0;
	max_off = (curwin->w_height - 1) / 2;
	ଯଦି (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    ଯଦି (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - below_wanted
#ifdef FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#endif
	    )
	ଫେରନ୍ତୁ;

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
    ଯେତେବେଳେ ((above < above_wanted || below < below_wanted) && topline < botline)
    {
	ଯଦି (below < below_wanted && (below <= above || above >= above_wanted))
	{
#ifdef FEAT_FOLDING
	    ଯଦି (hasFolding(botline, &botline, ଖାଲି))
		++below;
	    ଅନ୍ୟଥା
#endif
		below += plines(botline);
	    --botline;
	}
	ଯଦି (above < above_wanted && (above < below || below >= below_wanted))
	{
#ifdef FEAT_FOLDING
	    ଯଦି (hasFolding(topline, ଖାଲି, &topline))
		++above;
	    ଅନ୍ୟଥା
#endif
		above += PLINES_NOFILL(topline);
#ifdef FEAT_DIFF
	    // Count filler lines below this line as context.
	    ଯଦି (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#endif
	    ++topline;
	}
    }
    ଯଦି (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    ଅନ୍ୟଥା ଯଦି (topline > botline)
	curwin->w_cursor.lnum = botline;
    ଅନ୍ୟଥା
    {
	ଯଦି (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	ଯଦି (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_scroll_overlap(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir)
{
    lineoff_T loff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    min_height = curwin->w_height - 2;

    validate_botline();
    ଯଦି ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା))
	ଫେରନ୍ତୁ min_height + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#ifdef FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_rows : curwin->w_topfill);
    loff.height = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#ଅନ୍ୟଥା
    loff.height = plines(loff.lnum);
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h1 = loff.height;
    ଯଦି (h1 > min_height)
	ଫେରନ୍ତୁ min_height + 2;  // no overlap
    ଯଦି (dir == FORWARD)
	topline_back(&loff);
    ଅନ୍ୟଥା
	botline_forw(&loff);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h2 = loff.height;
    ଯଦି (h2 == MAXCOL || h2 + h1 > min_height)
	ଫେରନ୍ତୁ min_height + 2;  // no overlap
    ଯଦି (dir == FORWARD)
	topline_back(&loff);
    ଅନ୍ୟଥା
	botline_forw(&loff);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h3 = loff.height;
    ଯଦି (h3 == MAXCOL || h3 + h2 > min_height)
	ଫେରନ୍ତୁ min_height + 2;  // no overlap
    ଯଦି (dir == FORWARD)
	topline_back(&loff);
    ଅନ୍ୟଥା
	botline_forw(&loff);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h4 = loff.height;
    ଯଦି (h4 == MAXCOL || h4 + h3 + h2 > min_height || h3 + h2 + h1 > min_height)
	ଫେରନ୍ତୁ min_height + 1;  // 1 line overlap
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ min_height;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scroll_with_sms(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, long ଗଣନା_2, long *curscount)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_topfill = curwin->w_topfill;
#endif

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, ଗଣନା_2);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    ଯଦି (!prev_sms && curwin->w_skipcol > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	ଯଦି (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width1 = curwin->w_width - curwin_col_off();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width2 = width1 + curwin_col_off2();
	ଗଣନା_2 = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	ଯଦି (fixdir == FORWARD)
	    ଗଣନା_2 = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, ଗଣନା_2);
	*curscount += ଗଣନା_2 * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    ଫେରନ୍ତୁ curwin->w_topline != prev_topline
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pagescroll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, long ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା half)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_move = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    colnr_T	prev_col = curwin->w_cursor.col;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    ଯଦି (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	ଯଦି (ଗଣନା_2)
	    curwin->w_p_scr = MIN(curwin->w_height, ଗଣନା_2);
	ଗଣନା_2 = MIN(curwin->w_height, curwin->w_p_scr);

	long curscount = ଗଣନା_2;
	// Adjust count so as to not reveal end of buffer lines.
	ଯଦି (dir == FORWARD
		    && (curwin->w_topline + curwin->w_height + ଗଣନା_2 > buflen
#ifdef FEAT_FOLDING
							|| hasAnyFolding(curwin)
#endif
	   ))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    ଯଦି (n - ଗଣନା_2 < curwin->w_height && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_height + ଗଣନା_2);
	    ଯଦି (n < curwin->w_height + ଗଣନା_2)
		ଗଣନା_2 = n - curwin->w_height;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	ଯଦି (ଗଣନା_2 > 0)
	{
	    did_move = scroll_with_sms(dir, ଗଣନା_2, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.col = prev_col;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	ଯଦି (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	ଅନ୍ୟଥା ଯଦି (dir == FORWARD)
	    cursor_down_inner(curwin, curscount);
	ଅନ୍ୟଥା
	    cursor_up_inner(curwin, curscount);
    }
    ଅନ୍ୟଥା
    {
	// Scroll [count] times 'window' or current window height lines.
	ଗଣନା_2 *= ((ONE_WINDOW && p_window > 0 && p_window < Rows - 1) ?
				MAX(1, p_window - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, ଗଣନା_2, &ଗଣନା_2);

	ଯଦି (did_move)
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

    ଯଦି (get_scrolloff_ମୂଲ୍ୟ() > 0)
	cursor_correct();
#ifdef FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#endif
    did_move = did_move
	|| prev_col != curwin->w_cursor.col
	|| prev_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    ଯଦି (!did_move)
	beep_flush();
    ଅନ୍ୟଥା ଯଦି (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    ଅନ୍ୟଥା ଯଦି (p_sol)
	nv_g_home_m_cmd(&ca);

    ଫେରନ୍ତୁ did_move ? OK : FAIL;
}

    ଶୂନ୍ୟ
do_check_cursorbind(ଶୂନ୍ୟ)
{
    ସ୍ଥିର win_T	*prev_curwin = ଖାଲି;
    ସ୍ଥିର buf_T	*prev_curbuf = ଖାଲି;
    ସ୍ଥିର varnumber_T	prev_changedtick = 0;
    ସ୍ଥିର pos_T	prev_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    ଯଦି (curwin == prev_curwin && curbuf == prev_curbuf
	    && CHANGEDTICK(curbuf) == prev_changedtick
	    && EQUAL_POS(curwin->w_cursor, prev_cursor))
	ଫେରନ୍ତୁ;
    prev_curwin = curwin;
    prev_curbuf = curbuf;
    prev_changedtick = CHANGEDTICK(curbuf);
    prev_cursor = curwin->w_cursor;

    linenr_T	ଧାଡ଼ି = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_curswant = curwin->w_set_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_VIsual_select = VIsual_select;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_VIsual_active = VIsual_active;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_active = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_buffer;
	// skip original window and windows with 'nocursorbind'
	ଯଦି (curwin != old_curwin && curwin->w_p_crb)
	{
#ifdef FEAT_DIFF
	    ଯଦି (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_ଧାଡ଼ି(old_curbuf, ଧାଡ଼ି);
	    ଅନ୍ୟଥା
#endif
		curwin->w_cursor.lnum = ଧାଡ଼ି;
	    curwin->w_cursor.col = col;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_set_curswant = set_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    ଯଦି (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    ଯଦି (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    ଯଦି (!curwin->w_p_scb)
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
