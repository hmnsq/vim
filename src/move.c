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

#অন্তর্ভাব "vim.h"

স্থির শূন্য redraw_for_cursorline(win_T *wp);
স্থির পূর্ণাঙ্ক scrolljump_মান(শূন্য);
স্থির পূর্ণাঙ্ক check_শীর্ষ_offset(শূন্য);
স্থির শূন্য curs_rows(win_T *wp);

প্রকারনির্দেশ সংরচনা
{
    linenr_T	    lnum;	// line number
#ifdef FEAT_DIFF
    পূর্ণাঙ্ক		    fill;	// filler lines
#endif
    পূর্ণাঙ্ক		    height;	// height of added line
} lineoff_T;

স্থির শূন্য topline_back(lineoff_T *lp);
স্থির শূন্য botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    স্থির পূর্ণাঙ্ক
adjust_plines_for_skipcol(win_T *wp)
{
    যদি (wp->w_skipcol == 0)
	প্রত্যাগম 0;

    পূর্ণাঙ্ক width = wp->w_width - win_col_off(wp);
    পূর্ণাঙ্ক w2 = width + win_col_off2(wp);
    যদি (wp->w_skipcol >= width && w2 > 0)
	প্রত্যাগম (wp->w_skipcol - width) / w2 + 1;

    প্রত্যাগম 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    পূর্ণাঙ্ক
plines_correct_topline(win_T *wp, linenr_T lnum, পূর্ণাঙ্ক limit_winheight)
{
    পূর্ণাঙ্ক n;
#ifdef FEAT_DIFF
    যদি (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    অন্যথা
#endif
	n = plines_win(wp, lnum, FALSE);
    যদি (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    যদি (limit_winheight && n > wp->w_height)
	n = wp->w_height;
    প্রত্যাগম n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    স্থির শূন্য
comp_botline(win_T *wp)
{
    পূর্ণাঙ্ক		n;
    linenr_T	lnum;
    পূর্ণাঙ্ক		done;
    পূর্ণাঙ্ক		i = 0;
    পূর্ণাঙ্ক		use_cache;
#ifdef FEAT_FOLDING
    linenr_T    last;
    পূর্ণাঙ্ক		folded;
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
		    && wp->w_সারিগুলি_valid > 0
		    && wp->w_সারিগুলি[0].wl_lnum <= wp->w_topline;

    যদি (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	done = wp->w_cline_row;
    }
    অন্যথা
    {
	lnum = wp->w_topline;
	done = 0;
    }

    // Find the w_lines[] entry for the starting line.
    যদি (use_cache)
	যাবত্ (i < wp->w_সারিগুলি_valid && wp->w_সারিগুলি[i].wl_lnum < lnum)
	    ++i;

    প্রতি ( ; lnum <= wp->w_buffer->b_ml.ml_সারি_গুনতি; ++i)
    {
	পূর্ণাঙ্ক	valid = FALSE;

#ifdef FEAT_FOLDING
	last = lnum;
	folded = FALSE;
#endif
	// Try to use the size from the previous redraw.
	যদি (use_cache && i < wp->w_সারিগুলি_valid)
	{
	    যদি (wp->w_সারিগুলি[i].wl_lnum < lnum || !wp->w_সারিগুলি[i].wl_valid)
		অনুবর্ত;		// skip changed or deleted lines
	    যদি (wp->w_সারিগুলি[i].wl_lnum == lnum)
		valid = TRUE;
	    অন্যথা // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	যদি (valid)
	{
	    n = wp->w_সারিগুলি[i].wl_size;
#ifdef FEAT_FOLDING
	    folded = wp->w_সারিগুলি[i].wl_folded;
	    last = wp->w_সারিগুলি[i].wl_lastlnum;
#endif
	}
	অন্যথা
	{
#ifdef FEAT_FOLDING
	    যদি (hasFoldingWin(wp, lnum, রিক্ত, &last, TRUE, রিক্ত))
	    {
		n = 1;
		folded = TRUE;
	    }
	    অন্যথা
#endif
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	যদি (
#ifdef FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && last >= wp->w_cursor.lnum
#অন্যথা
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
	যদি (done + n > wp->w_height)
	    ভঙ্গ;
	done += n;
#ifdef FEAT_FOLDING
	lnum = last + 1;
#অন্যথা
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
    স্থির শূন্য
redraw_for_cursorline(win_T *wp)
{
    যদি ((wp->w_p_rnu
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
    স্থির শূন্য
redraw_for_cursorcolumn(win_T *wp)
{
    যদি ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	যদি (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	অন্যথা যদি (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#endif

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    শূন্য
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
    পূর্ণাঙ্ক
sms_marker_overlap(win_T *wp, পূর্ণাঙ্ক extra2)
{
    যদি (extra2 == -1)
	extra2 = win_col_off(wp) - win_col_off2(wp);
#যদি defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    যদি (*get_showbreak_মান(wp) != NUL)
	প্রত্যাগম 0;
#endif
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    যদি (wp->w_p_list && wp->w_lcs_chars.prec)
	প্রত্যাগম 1;

    প্রত্যাগম extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    স্থির পূর্ণাঙ্ক
skipcol_from_plines(win_T *wp, পূর্ণাঙ্ক plines_off)
{
    পূর্ণাঙ্ক width1 = wp->w_width - win_col_off(wp);

    পূর্ণাঙ্ক skipcol = 0;
    যদি (plines_off > 0)
	skipcol += width1;
    যদি (plines_off > 1)
	skipcol += (width1 + win_col_off2(wp)) * (plines_off - 1);
    প্রত্যাগম skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    স্থির শূন্য
reset_skipcol(শূন্য)
{
    যদি (curwin->w_skipcol == 0)
	প্রত্যাগম;

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
    শূন্য
update_topline_redraw(শূন্য)
{
    update_topline();
    যদি (must_redraw)
	update_screen(0);
}

/*
 * Return true when 'scrolloffpad' may augment 'scrolloff'.
 * This only applies to automatic cursor visibility correction.
 * For now 'scrolloffpad' is treated as boolean: 0 disables, > 0 enables.
 */
    স্থির bool
use_scrolloffpad(শূন্য)
{
    প্রত্যাগম get_scrolloff_মান() > 0 && get_scrolloffpad_মান() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    স্থির bool
scrolloffpad_eof_pressure(linenr_T lnum, long so)
{
    যদি (!use_scrolloffpad() || so <= 0)
	প্রত্যাগম false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    প্রত্যাগম lnum > curbuf->b_ml.ml_সারি_গুনতি - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    শূন্য
update_topline(শূন্য)
{
    long	সারি_গুনতি;
    পূর্ণাঙ্ক		halfheight;
    পূর্ণাঙ্ক		n;
#ifdef FEAT_FOLDING
    linenr_T	lnum;
#endif
    পূর্ণাঙ্ক		check_topline = FALSE;
    পূর্ণাঙ্ক		check_botline = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    পূর্ণাঙ্ক		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    যদি (skip_update_topline)
	প্রত্যাগম;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    যদি (!screen_valid(TRUE) || curwin->w_height == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	প্রত্যাগম;
    }

    check_cursor_moved(curwin);
    যদি (curwin->w_valid & VALID_TOPLINE)
	প্রত্যাগম;

    // When dragging with the mouse, don't scroll that quickly
    যদি (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    পূর্ণাঙ্ক old_topfill = curwin->w_topfill;
#endif

    /*
     * If the buffer is empty, always set topline to 1.
     */
    যদি (BUFEMPTY())		// special case - file is empty
    {
	যদি (curwin->w_topline != 1)
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
    অন্যথা
    {
	যদি (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    যদি (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    অন্যথা যদি (check_শীর্ষ_offset())
		check_topline = TRUE;
	    অন্যথা যদি (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		পূর্ণাঙ্ক overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, রিক্ত, রিক্ত, 0);
		overlap = sms_marker_overlap(curwin, -1);
		যদি (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#ifdef FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	যদি (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#endif

	যদি (check_topline)
	{
	    halfheight = curwin->w_height / 2 - 1;
	    যদি (halfheight < 2)
		halfheight = 2;

#ifdef FEAT_FOLDING
	    যদি (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		প্রতি (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    যদি (lnum >= curbuf->b_ml.ml_সারি_গুনতি || n >= halfheight)
			ভঙ্গ;
		    (শূন্য)hasFolding(lnum, রিক্ত, &lnum);
		}
	    }
	    অন্যথা
#endif
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    পূর্ণাঙ্ক min_scroll = scrolljump_মান();
	    যদি (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    অন্যথা যদি (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    অন্যথা
	    {
		scroll_cursor_শীর্ষ(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	অন্যথা
	{
#ifdef FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (শূন্য)hasFolding(curwin->w_topline, &curwin->w_topline, রিক্ত);
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
    যদি (check_botline)
    {
	যদি (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	যদি (curwin->w_botline <= curbuf->b_ml.ml_সারি_গুনতি || use_scrolloffpad())
	{
	    যদি (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		যদি (((long)curwin->w_cursor.lnum
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
		    (শূন্য)hasFolding(loff.lnum, রিক্ত, &loff.lnum);
#endif
#ifdef FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_rows;
#endif
		    loff.height = 0;
		    যাবত্ (loff.lnum < curwin->w_botline
#ifdef FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#endif
			    )
		    {
			n += loff.height;
			যদি (n >= *so_ptr)
			    ভঙ্গ;
			botline_forw(&loff);
		    }
		    যদি (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		অন্যথা
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    যদি (check_botline)
	    {
#ifdef FEAT_FOLDING
		যদি (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    সারি_গুনতি = 0;
		    প্রতি (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++সারি_গুনতি;
			// stop at end of file or when we know we are far off
			যদি (lnum <= 0 || সারি_গুনতি > curwin->w_height + 1)
			    ভঙ্গ;
			(শূন্য)hasFolding(lnum, &lnum, রিক্ত);
		    }
		}
		অন্যথা
#endif
		    সারি_গুনতি = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		যদি (সারি_গুনতি <= curwin->w_height + 1)
		{
		    যদি (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    অন্যথা
			scroll_cursor_bot(scrolljump_মান(), FALSE);
		}
		অন্যথা
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    যদি (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#endif
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	যদি (!curwin->w_p_sms)
	    reset_skipcol();
	অন্যথা যদি (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	যদি (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    স্থির পূর্ণাঙ্ক
scrolljump_মান(শূন্য)
{
    যদি (p_sj >= 0)
	প্রত্যাগম (পূর্ণাঙ্ক)p_sj;
    প্রত্যাগম (curwin->w_height * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    স্থির পূর্ণাঙ্ক
check_শীর্ষ_offset(শূন্য)
{
    lineoff_T	loff;
    পূর্ণাঙ্ক		n;
    long	so = get_scrolloff_মান();

    যদি (curwin->w_cursor.lnum < curwin->w_topline + so
#ifdef FEAT_FOLDING
		    || hasAnyFolding(curwin)
#endif
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#অন্যথা
	n = 0;
#endif
	// Count the visible screen lines above the cursor line.
	যাবত্ (n < so)
	{
	    topline_back(&loff);
	    // Stop when included a line above the window.
	    যদি (loff.lnum < curwin->w_topline
#ifdef FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#endif
		    )
		ভঙ্গ;
	    n += loff.height;
	}
	যদি (n < so)
	    প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Update w_curswant.
 */
    শূন্য
update_curswant_force(শূন্য)
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
    শূন্য
update_curswant(শূন্য)
{
    যদি (curwin->w_set_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    শূন্য
check_cursor_moved(win_T *wp)
{
    যদি (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    অন্যথা যদি (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    অন্যথা যদি (wp->w_cursor.col != wp->w_valid_cursor.col
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
    শূন্য
changed_window_setting(শূন্য)
{
    changed_window_setting_win(curwin);
}

    শূন্য
changed_window_setting_win(win_T *wp)
{
    wp->w_সারিগুলি_valid = 0;
    changed_সারি_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#যদি defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    শূন্য
changed_window_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি (wp->w_buffer == buf)
	    changed_window_setting_win(wp);
}
#endif

/*
 * Call changed_window_setting_win() for every window.
 */
    শূন্য
changed_window_setting_all(শূন্য)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	changed_window_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    শূন্য
set_topline(win_T *wp, linenr_T lnum)
{
#ifdef FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#endif

#ifdef FEAT_FOLDING
    // go to first of folded lines
    (শূন্য)hasFoldingWin(wp, lnum, &lnum, রিক্ত, TRUE, রিক্ত);
#endif
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    যদি (wp->w_botline > wp->w_buffer->b_ml.ml_সারি_গুনতি + 1)
	wp->w_botline = wp->w_buffer->b_ml.ml_সারি_গুনতি + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_set = true;
#ifdef FEAT_DIFF
    যদি (lnum != prev_topline)
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
    শূন্য
changed_cline_bef_curs(শূন্য)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    শূন্য
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
    শূন্য
changed_সারি_abv_curs(শূন্য)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    শূন্য
changed_সারি_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#যদি defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    শূন্য
changed_সারি_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_buffer == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#endif

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    শূন্য
validate_botline(শূন্য)
{
    validate_botline_win(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    শূন্য
validate_botline_win(win_T *wp)
{
    যদি (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    শূন্য
invalidate_botline(শূন্য)
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    শূন্য
invalidate_botline_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    শূন্য
approximate_botline_win(
    win_T	*wp)
{
    wp->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    পূর্ণাঙ্ক
cursor_valid(শূন্য)
{
    check_cursor_moved(curwin);
    প্রত্যাগম ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    শূন্য
validate_cursor(শূন্য)
{
    check_cursor_lnum();
    check_cursor_moved(curwin);
    যদি ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#যদি defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    শূন্য
validate_cline_row(শূন্য)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    যদি (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);
}
#endif

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    স্থির শূন্য
curs_rows(win_T *wp)
{
    linenr_T	lnum;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		all_invalid;
    পূর্ণাঙ্ক		valid;
#ifdef FEAT_FOLDING
    long	fold_গুনতি;
#endif

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_সারিগুলি_valid == 0
			|| wp->w_সারিগুলি[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_row = 0;
    প্রতি (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	যদি (!all_invalid && i < wp->w_সারিগুলি_valid)
	{
	    যদি (wp->w_সারিগুলি[i].wl_lnum < lnum || !wp->w_সারিগুলি[i].wl_valid)
		অনুবর্ত;		// skip changed or deleted lines
	    যদি (wp->w_সারিগুলি[i].wl_lnum == lnum)
	    {
#ifdef FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		যদি (!wp->w_buffer->b_mod_set
			|| wp->w_সারিগুলি[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_buffer->b_mod_শীর্ষ
					     > wp->w_সারিগুলি[i].wl_lastlnum + 1)
#endif
		valid = TRUE;
	    }
	    অন্যথা যদি (wp->w_সারিগুলি[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	যদি (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#ifdef FEAT_DIFF
					    && !wp->w_p_diff
#endif
	       )))
	{
#ifdef FEAT_FOLDING
	    lnum = wp->w_সারিগুলি[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    যদি (lnum > wp->w_cursor.lnum)
		ভঙ্গ;
#অন্যথা
	    ++lnum;
#endif
	    wp->w_cline_row += wp->w_সারিগুলি[i].wl_size;
	}
	অন্যথা
	{
#ifdef FEAT_FOLDING
	    fold_গুনতি = folded_গুনতি(wp, lnum, রিক্ত);
	    যদি (fold_গুনতি)
	    {
		lnum += fold_গুনতি;
		যদি (lnum > wp->w_cursor.lnum)
		    ভঙ্গ;
		++wp->w_cline_row;
	    }
	    অন্যথা
#endif
	    {
		wp->w_cline_row += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    যদি (!(wp->w_valid & VALID_CHEIGHT))
    {
	যদি (all_invalid
		|| i == wp->w_সারিগুলি_valid
		|| (i < wp->w_সারিগুলি_valid
		    && (!wp->w_সারিগুলি[i].wl_valid
			|| wp->w_সারিগুলি[i].wl_lnum != wp->w_cursor.lnum)))
	{
#ifdef FEAT_DIFF
	    যদি (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_height = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    অন্যথা
#endif
		wp->w_cline_height = plines_win(wp, wp->w_cursor.lnum, TRUE);
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      রিক্ত, রিক্ত, TRUE, রিক্ত);
#endif
	}
	অন্যথা যদি (i > wp->w_সারিগুলি_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_height = 0;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      রিক্ত, রিক্ত, TRUE, রিক্ত);
#endif
	}
	অন্যথা
	{
	    wp->w_cline_height = wp->w_সারিগুলি[i].wl_size;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = wp->w_সারিগুলি[i].wl_folded;
#endif
	}
    }

    redraw_for_cursorline(curwin);
    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;
}

/*
 * Validate curwin->w_virtcol only.
 */
    শূন্য
validate_virtcol(শূন্য)
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    শূন্য
validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);

    যদি (wp->w_valid & VALID_VIRTCOL)
	প্রত্যাগম;

#ifdef FEAT_PROP_POPUP
    wp->w_virtcol_first_char = 0;
#endif
    getvvcol(wp, &wp->w_cursor, রিক্ত, &(wp->w_virtcol), রিক্ত,
							 GETVCOL_FOR_VIRTCOL);
#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#endif
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    শূন্য
validate_cheight(শূন্য)
{
    check_cursor_moved(curwin);

    যদি (curwin->w_valid & VALID_CHEIGHT)
	প্রত্যাগম;

#ifdef FEAT_DIFF
    যদি (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_height = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    অন্যথা
#endif
	curwin->w_cline_height = plines(curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, রিক্ত, রিক্ত);
#endif
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    শূন্য
validate_cursor_col(শূন্য)
{
    colnr_T off;
    colnr_T col;
    পূর্ণাঙ্ক     width;

    validate_virtcol();

    যদি (curwin->w_valid & VALID_WCOL)
	প্রত্যাগম;

    col = curwin->w_virtcol;
    off = curwin_col_off();
    col += off;
    width = curwin->w_width - off + curwin_col_off2();

    // long line wrapping, adjust curwin->w_wrow
    যদি (curwin->w_p_wrap
	    && col >= (colnr_T)curwin->w_width
	    && width > 0)
	// use same formula as what is used in curs_columns()
	col -= ((col - curwin->w_width) / width + 1) * width;
    যদি (col > (পূর্ণাঙ্ক)curwin->w_leftcol)
	col -= curwin->w_leftcol;
    অন্যথা
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
    পূর্ণাঙ্ক
win_col_off(win_T *wp)
{
    প্রত্যাগম (((wp->w_p_nu || wp->w_p_rnu) ? সংখ্যা_width(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#ifdef FEAT_FOLDING
	    + wp->w_p_fdc
#endif
#ifdef FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#endif
	   );
}

    পূর্ণাঙ্ক
curwin_col_off(শূন্য)
{
    প্রত্যাগম win_col_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    পূর্ণাঙ্ক
win_col_off2(win_T *wp)
{
    যদি ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != রিক্ত)
	প্রত্যাগম সংখ্যা_width(wp) + 1;
    প্রত্যাগম 0;
}

    পূর্ণাঙ্ক
curwin_col_off2(শূন্য)
{
    প্রত্যাগম win_col_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    শূন্য
curs_columns(
    পূর্ণাঙ্ক		may_scroll)	// when TRUE, may scroll horizontally
{
    long	diff;
    পূর্ণাঙ্ক		extra;		// offset for first screen line
    long	off_বাম, off_ডান;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		p_সারিগুলি;
    পূর্ণাঙ্ক		width1;		// text width for first screen line
    পূর্ণাঙ্ক		width2 = 0;	// text width for second and later screen line
    পূর্ণাঙ্ক		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    long	so = get_scrolloff_মান();
    long	siso = get_sidescrolloff_মান();
    পূর্ণাঙ্ক		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    যদি (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);

#ifdef FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_first_char = 0;
#endif

    /*
     * Compute the number of virtual columns.
     */
#ifdef FEAT_FOLDING
    যদি (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    অন্যথা
#endif
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    যদি (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_col_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_row;

    width1 = curwin->w_width - extra;
    যদি (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_width - 1;
	যদি (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_height - 1;
	অন্যথা
	    curwin->w_wrow = curwin->w_height - 1 - curwin->w_empty_rows;
    }
    অন্যথা যদি (curwin->w_p_wrap && curwin->w_width != 0)
    {
	width2 = width1 + curwin_col_off2();

	// skip columns that are not visible
	যদি (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    যদি (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    অন্যথা
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	যদি (curwin->w_wcol >= curwin->w_width)
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
    অন্যথা যদি (may_scroll
#ifdef FEAT_FOLDING
	    && !curwin->w_cline_folded
#endif
	    )
    {
#ifdef FEAT_PROP_POPUP
	যদি (curwin->w_virtcol_first_char > 0)
	{
	    পূর্ণাঙ্ক cols = (curwin->w_width - extra);
	    পূর্ণাঙ্ক rows = cols > 0 ? curwin->w_virtcol_first_char / cols : 1;

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
	off_বাম = startcol - curwin->w_leftcol - siso;
	off_ডান = endcol - curwin->w_leftcol - (curwin->w_width - siso) + 1;
	যদি (off_বাম < 0 || off_ডান > 0)
	{
	    যদি (off_বাম < 0)
		diff = -off_বাম;
	    অন্যথা
		diff = off_ডান;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    যদি (p_ss == 0 || diff >= width1 / 2 || off_ডান >= off_বাম)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    অন্যথা
	    {
		যদি (diff < p_ss)
		    diff = p_ss;
		যদি (off_বাম < 0)
		    new_leftcol = curwin->w_leftcol - (পূর্ণাঙ্ক)diff;
		অন্যথা
		    new_leftcol = curwin->w_leftcol + (পূর্ণাঙ্ক)diff;
	    }
	    যদি (new_leftcol < 0)
		new_leftcol = 0;
	    যদি (new_leftcol != (পূর্ণাঙ্ক)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    অন্যথা যদি (curwin->w_wcol > (পূর্ণাঙ্ক)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    অন্যথা
	curwin->w_wcol = 0;

#ifdef FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    যদি (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    অন্যথা
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#endif

    prev_skipcol = curwin->w_skipcol;

    p_সারিগুলি = 0;

    যদি ((curwin->w_wrow >= curwin->w_height
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_height)
		    && (p_সারিগুলি =
#ifdef FEAT_DIFF
			plines_win_nofill
#অন্যথা
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
	যদি (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	যদি (p_সারিগুলি == 0)
	    p_সারিগুলি = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_সারিগুলি;
	যদি (p_সারিগুলি > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	অন্যথা
	    n = p_সারিগুলি;
	যদি ((colnr_T)n >= curwin->w_height + curwin->w_skipcol / width2 - so)
	    extra += 2;

	যদি (extra == 3 || curwin->w_height <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    যদি (n > curwin->w_height / 2)
		n -= curwin->w_height / 2;
	    অন্যথা
		n = 0;
	    // don't skip more than necessary
	    যদি (n > p_সারিগুলি - curwin->w_height + 1)
		n = p_সারিগুলি - curwin->w_height + 1;
	    যদি (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    অন্যথা
		curwin->w_skipcol = 0;
	}
	অন্যথা যদি (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    যদি (extra > 0)
	    {
		যদি ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	অন্যথা যদি (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_height + 1) * width2;
	    যাবত্ (endcol > curwin->w_virtcol)
		endcol -= width2;
	    যদি (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	যদি (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - prev_skipcol) / width2;
	অন্যথা
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	যদি (curwin->w_wrow >= curwin->w_height)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_height + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((পূর্ণাঙ্ক)prev_skipcol - (পূর্ণাঙ্ক)curwin->w_skipcol) / width2;
	যদি (extra > 0)
	    win_ins_সারিগুলি(curwin, 0, extra, FALSE, FALSE);
	অন্যথা যদি (extra < 0)
	    win_del_সারিগুলি(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    অন্যথা যদি (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    যদি (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#endif
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    যদি (popup_is_popup(curwin) && curbuf->b_term != রিক্ত)
    {
	curwin->w_wrow += popup_শীর্ষ_extra(curwin);
	curwin->w_wcol += popup_বাম_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    অন্যথা
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#endif

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#যদি defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    শূন্য
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	পূর্ণাঙ্ক	*rowp,	// screen row
	পূর্ণাঙ্ক	*scolp,	// start screen column
	পূর্ণাঙ্ক	*ccolp,	// cursor screen column
	পূর্ণাঙ্ক	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    পূর্ণাঙ্ক		row = 0;
    colnr_T	coloff = 0;

    যদি (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    col;
	পূর্ণাঙ্ক	    width;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	পূর্ণাঙ্ক	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, রিক্ত, TRUE, রিক্ত);
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
	যদি (is_folded)
	{
	    row += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	অন্যথা
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    col = scol;
	    col += off;
	    width = wp->w_width - off + win_col_off2(wp);

	    // long line wrapping, adjust row
	    যদি (wp->w_p_wrap
		    && col >= (colnr_T)wp->w_width
		    && width > 0)
	    {
		// use same formula as what is used in curs_columns()
		পূর্ণাঙ্ক rowoff = ((col - wp->w_width) / width + 1);
		col -= rowoff * width;
		row += rowoff;
	    }
	    col -= wp->w_leftcol;
	    যদি (col >= wp->w_width)
		col = -1;
	    যদি (col >= 0 && row >= 0 && row < wp->w_height)
	    {
		coloff = col - scol + wp->w_wincol + 1;
		row += W_WINROW(wp) + 1;
	    }
	    অন্যথা
		// character is out of the window
		row = scol = ccol = ecol = 0;
	}
    }
    *rowp = row;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    যদি (wp->w_p_rl && row > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	পূর্ণাঙ্ক endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_width - endoff;
    }
# endif
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    শূন্য
f_screenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    পূর্ণাঙ্ক		row = 0;
    পূর্ণাঙ্ক		scol = 0, ccol = 0, ecol = 0;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	প্রত্যাগম;
    dict = rettv->vval.v_dict;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 2) == FAIL))
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == রিক্ত)
	প্রত্যাগম;

    pos.lnum = tv_get_সংখ্যা(&argvars[1]);
    যদি (pos.lnum > wp->w_buffer->b_ml.ml_সারি_গুনতি)
    {
	semsg(_(e_invalid_সারি_সংখ্যা_nr), pos.lnum);
	প্রত্যাগম;
    }
    pos.col = tv_get_সংখ্যা(&argvars[2]) - 1;
    যদি (pos.col < 0)
	pos.col = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &row, &scol, &ccol, &ecol);

    dict_add_সংখ্যা(dict, "row", row);
    dict_add_সংখ্যা(dict, "col", scol);
    dict_add_সংখ্যা(dict, "curscol", ccol);
    dict_add_সংখ্যা(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    স্থির পূর্ণাঙ্ক
virtcol2col(win_T *wp, linenr_T lnum, পূর্ণাঙ্ক vcol)
{
    পূর্ণাঙ্ক		offset = vcol2col(wp, lnum, vcol - 1, রিক্ত);
    char_u	*সারি = ml_get_buf(wp->w_buffer, lnum, FALSE);
    char_u	*p = সারি + offset;

    যদি (*p == NUL)
    {
	যদি (p == সারি)  // empty line
	    প্রত্যাগম 0;
	// Move to the first byte of the last char.
	MB_PTR_BACK(সারি, p);
    }
    প্রত্যাগম (পূর্ণাঙ্ক)(p - সারি + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    শূন্য
f_virtcol2col(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    পূর্ণাঙ্ক		screencol;
    পূর্ণাঙ্ক		error = FALSE;

    rettv->vval.v_সংখ্যা = -1;

    যদি (check_for_সংখ্যা_arg(argvars, 0) == FAIL
	    || check_for_সংখ্যা_arg(argvars, 1) == FAIL
	    || check_for_সংখ্যা_arg(argvars, 2) == FAIL)
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == রিক্ত)
	প্রত্যাগম;

    lnum = tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (error || lnum < 0 || lnum > wp->w_buffer->b_ml.ml_সারি_গুনতি)
	প্রত্যাগম;

    screencol = tv_get_সংখ্যা_chk(&argvars[2], &error);
    যদি (error || screencol < 0)
	প্রত্যাগম;

    rettv->vval.v_সংখ্যা = virtcol2col(wp, lnum, screencol);
}
#endif

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
স্থির শূন্য cursor_correct_sms(শূন্য)
{
    যদি (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	প্রত্যাগম;

    long    so = get_scrolloff_মান();
    পূর্ণাঙ্ক	    width1 = curwin->w_width - curwin_col_off();
    পূর্ণাঙ্ক	    width2 = width1 + curwin_col_off2();
    পূর্ণাঙ্ক	    so_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    পূর্ণাঙ্ক	    space_cols = (curwin->w_height - 1) * width2;
    পূর্ণাঙ্ক	    overlap, শীর্ষ, bot;
    পূর্ণাঙ্ক	    size = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    যদি (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_cols = 0;               // Ignore 'scrolloff' at top of buffer.
    অন্যথা যদি (so_cols > space_cols / 2)
	so_cols = space_cols / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    যাবত্ (so_cols > size && so_cols - width2 >= width1 && width1 > 0)
	so_cols -= width2;
    যদি (so_cols >= width1 && so_cols > size)
	so_cols -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_width - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    শীর্ষ = curwin->w_skipcol + (so_cols != 0 ? so_cols : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_height - 1) * width2
								    - so_cols;
    validate_virtcol();
    colnr_T col = curwin->w_virtcol;

    যদি (col < শীর্ষ)
    {
	যদি (col < width1)
	    col += width1;
	যাবত্ (width2 > 0 && col < শীর্ষ)
	    col += width2;
    }
    অন্যথা
	যাবত্ (width2 > 0 && col >= bot)
	    col -= width2;

    যদি (col != curwin->w_virtcol)
    {
	পূর্ণাঙ্ক rc;

	curwin->w_curswant = col;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	যদি (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_সারি_গুনতি)
	{
	    validate_virtcol();
	    যদি (curwin->w_virtcol < curwin->w_skipcol + overlap)
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
    শূন্য
scroll_redraw(পূর্ণাঙ্ক up, long গুনতি)
{
    linenr_T	prev_topline = curwin->w_topline;
    পূর্ণাঙ্ক		prev_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    পূর্ণাঙ্ক		prev_topfill = curwin->w_topfill;
#endif
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    যদি (up)
	scrollup(গুনতি, TRUE);
    অন্যথা
	scrolldown(গুনতি, TRUE);
    যদি (get_scrolloff_মান() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	যাবত্ (curwin->w_topline == prev_topline
		&& curwin->w_skipcol == prev_skipcol
#ifdef FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#endif
		)
	{
	    যদি (up)
	    {
		যদি (curwin->w_cursor.lnum > prev_lnum
			|| cursor_down(1L, FALSE) == FAIL)
		    ভঙ্গ;
	    }
	    অন্যথা
	    {
		যদি (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_up(1L, FALSE) == FAIL)
		    ভঙ্গ;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    যদি (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    শূন্য
scrolldown(
    long	সারি_গুনতি,
    পূর্ণাঙ্ক		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    long	done = 0;	// total # of physical lines done
    পূর্ণাঙ্ক		wrow;
    পূর্ণাঙ্ক		moved = FALSE;
    পূর্ণাঙ্ক		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    পূর্ণাঙ্ক		width1 = 0;
    পূর্ণাঙ্ক		width2 = 0;

    যদি (do_sms)
    {
	width1 = curwin->w_width - curwin_col_off();
	width2 = width1 + curwin_col_off2();
    }

#ifdef FEAT_FOLDING
    linenr_T	first;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (শূন্য)hasFolding(curwin->w_topline, &curwin->w_topline, রিক্ত);
#endif
    validate_cursor();		// w_wrow needs to be valid
    প্রতি (পূর্ণাঙ্ক todo = সারি_গুনতি; todo > 0; --todo)
    {
#ifdef FEAT_DIFF
	যদি (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_height - 1)
	{
	    ++curwin->w_topfill;
	    ++done;
	}
	অন্যথা
#endif
	{
	    // break when at the very top
	    যদি (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		ভঙ্গ;
	    যদি (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		যদি (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		অন্যথা
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++done;
	    }
	    অন্যথা
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#ifdef FEAT_DIFF
		curwin->w_topfill = 0;
#endif
#ifdef FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		যদি (hasFolding(curwin->w_topline, &first, রিক্ত))
		{
		    ++done;
		    যদি (!byfold)
			todo -= curwin->w_topline - first - 1;
		    curwin->w_botline -= curwin->w_topline - first;
		    curwin->w_topline = first;
		}
		অন্যথা
#endif
		যদি (do_sms)
		{
		    পূর্ণাঙ্ক size = linetabsize_eol(curwin, curwin->w_topline);
		    যদি (size > width1)
		    {
			curwin->w_skipcol = width1;
			size -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    যাবত্ (size > width2)
		    {
			curwin->w_skipcol += width2;
			size -= width2;
		    }
		    ++done;
		}
		অন্যথা
		    done += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += done;		// keep w_wrow updated
    curwin->w_cline_row += done;	// keep w_cline_row updated

#ifdef FEAT_DIFF
    যদি (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_row = 0;
    check_topfill(curwin, TRUE);
#endif

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    যদি (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    যাবত্ (wrow >= curwin->w_height && curwin->w_cursor.lnum > 1)
    {
#ifdef FEAT_FOLDING
	যদি (hasFolding(curwin->w_cursor.lnum, &first, রিক্ত))
	{
	    --wrow;
	    যদি (first == 1)
		curwin->w_cursor.lnum = 1;
	    অন্যথা
		curwin->w_cursor.lnum = first - 1;
	}
	অন্যথা
#endif
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    যদি (moved)
    {
#ifdef FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#endif
	coladvance(curwin->w_curswant);
    }
    যদি (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    শূন্য
scrollup(
    long	সারি_গুনতি,
    পূর্ণাঙ্ক		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    পূর্ণাঙ্ক		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    যদি (do_sms
#ifdef FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#endif
#ifdef FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#endif
       )
    {
	পূর্ণাঙ্ক	    width1 = curwin->w_width - curwin_col_off();
	পূর্ণাঙ্ক	    width2 = width1 + curwin_col_off2();
	পূর্ণাঙ্ক	    size = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	যদি (do_sms)
	    size = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	প্রতি (পূর্ণাঙ্ক todo = সারি_গুনতি; todo > 0; --todo)
	{
#ifdef FEAT_DIFF
	    যদি (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    অন্যথা
#endif
	    {
		linenr_T lnum = curwin->w_topline;

#ifdef FEAT_FOLDING
		যদি (byfold)
		    // for a closed fold: go to the last line in the fold
		    (শূন্য)hasFolding(lnum, রিক্ত, &lnum);
#endif
		যদি (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    পূর্ণাঙ্ক add = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += add;
		    যদি (curwin->w_skipcol >= size)
		    {
			যদি (lnum == curbuf->b_ml.ml_সারি_গুনতি)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= add;
			    ভঙ্গ;
			}
			++lnum;
		    }
		}
		অন্যথা
		{
		    যদি (lnum >= curbuf->b_ml.ml_সারি_গুনতি)
			ভঙ্গ;
		    ++lnum;
		}

		যদি (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#ifdef FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#endif
		    curwin->w_skipcol = 0;
		    যদি (todo > 1 && do_sms)
			size = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	যদি (prev_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    অন্যথা
    {
	curwin->w_topline += সারি_গুনতি;
	curwin->w_botline += সারি_গুনতি;	// approximate w_botline
    }

    যদি (curwin->w_topline > curbuf->b_ml.ml_সারি_গুনতি)
	curwin->w_topline = curbuf->b_ml.ml_সারি_গুনতি;
    যদি (curwin->w_botline > curbuf->b_ml.ml_সারি_গুনতি + 1)
	curwin->w_botline = curbuf->b_ml.ml_সারি_গুনতি + 1;

#ifdef FEAT_DIFF
    check_topfill(curwin, FALSE);
#endif

#ifdef FEAT_FOLDING
    যদি (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(শূন্য)hasFolding(curwin->w_topline, &curwin->w_topline, রিক্ত);
#endif

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    যদি (curwin->w_cursor.lnum < curwin->w_topline)
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
    শূন্য
adjust_skipcol(শূন্য)
{
    যদি (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	প্রত্যাগম;

    পূর্ণাঙ্ক	    width1 = curwin->w_width - curwin_col_off();
    যদি (width1 <= 0)
	প্রত্যাগম;  // no text will be displayed

    পূর্ণাঙ্ক	    width2 = width1 + curwin_col_off2();
    long    so = get_scrolloff_মান();
    পূর্ণাঙ্ক	    scrolloff_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    পূর্ণাঙ্ক	    scrolled = FALSE;
    পূর্ণাঙ্ক	    row = 0;
    পূর্ণাঙ্ক	    overlap, col;

    validate_cheight();
    যদি (curwin->w_cline_height == curwin->w_height
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_height)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	প্রত্যাগম;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_width - width2);
    যাবত্ (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_cols)
    {
	// scroll a screen line down
	যদি (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	অন্যথা
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    যদি (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	প্রত্যাগম;  // don't scroll in the other direction now
    }

    col = curwin->w_virtcol + scrolloff_cols;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    যদি (scrolloff_cols > 0)
    {
	পূর্ণাঙ্ক size = linetabsize_eol(curwin, curwin->w_topline);
	size = width1 + width2 * ((size - width1 + width2 - 1) / width2);
	যাবত্ (col > size)
	    col -= width2;
    }
    col -= curwin->w_skipcol;

    যদি (col >= width1)
    {
	col -= width1;
	++row;
    }
    যদি (col > width2)
    {
	row += col / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	col = col % width2;
    }
    যদি (row >= curwin->w_height)
    {
	যদি (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --row;
	}
	যদি (row >= curwin->w_height)
	    curwin->w_skipcol += (row - curwin->w_height) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#ifdef FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    শূন্য
check_topfill(
    win_T	*wp,
    পূর্ণাঙ্ক		down)	// when TRUE scroll down when not enough space
{
    পূর্ণাঙ্ক		n;

    যদি (wp->w_topfill <= 0)
	প্রত্যাগম;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    যদি (wp->w_topfill + n > wp->w_height)
    {
	যদি (down && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	অন্যথা
	{
	    wp->w_topfill = wp->w_height - n;
	    যদি (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#endif

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    শূন্য
scrolldown_clamp(শূন্য)
{
    পূর্ণাঙ্ক		end_row;
#ifdef FEAT_DIFF
    পূর্ণাঙ্ক		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#endif

    যদি (curwin->w_topline <= 1
#ifdef FEAT_DIFF
	    && !can_fill
#endif
	    )
	প্রত্যাগম;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    end_row = curwin->w_wrow;
#ifdef FEAT_DIFF
    যদি (can_fill)
	++end_row;
    অন্যথা
	end_row += plines_nofill(curwin->w_topline - 1);
#অন্যথা
    end_row += plines(curwin->w_topline - 1);
#endif
    যদি (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_cheight();
	validate_virtcol();
	end_row += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    যদি (end_row < curwin->w_height - get_scrolloff_মান())
    {
#ifdef FEAT_DIFF
	যদি (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	অন্যথা
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#অন্যথা
	--curwin->w_topline;
#endif
#ifdef FEAT_FOLDING
	(শূন্য)hasFolding(curwin->w_topline, &curwin->w_topline, রিক্ত);
#endif
	--curwin->w_botline;	    // approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    শূন্য
scrollup_clamp(শূন্য)
{
    পূর্ণাঙ্ক	    start_row;

    যদি (curwin->w_topline == curbuf->b_ml.ml_সারি_গুনতি
#ifdef FEAT_DIFF
	    && curwin->w_topfill == 0
#endif
	    )
	প্রত্যাগম;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#ifdef FEAT_DIFF
    start_row = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#অন্যথা
    start_row = curwin->w_wrow - plines(curwin->w_topline);
#endif
    যদি (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	start_row -= curwin->w_virtcol / curwin->w_width;
    }
    যদি (start_row >= get_scrolloff_মান())
    {
#ifdef FEAT_DIFF
	যদি (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	অন্যথা
#endif
	{
#ifdef FEAT_FOLDING
	    (শূন্য)hasFolding(curwin->w_topline, রিক্ত, &curwin->w_topline);
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
    স্থির শূন্য
topline_back_winheight(
    lineoff_T	*lp,
    পূর্ণাঙ্ক		winheight)	// when TRUE limit to window height
{
#ifdef FEAT_DIFF
    যদি (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    অন্যথা
#endif
    {
	--lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	যদি (lp->lnum < 1)
	    lp->height = MAXCOL;
	অন্যথা
#ifdef FEAT_FOLDING
	    যদি (hasFolding(lp->lnum, &lp->lnum, রিক্ত))
	    // Add a closed fold
	    lp->height = 1;
	অন্যথা
#endif
	    lp->height = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    স্থির শূন্য
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
    স্থির শূন্য
botline_forw(lineoff_T *lp)
{
#ifdef FEAT_DIFF
    যদি (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    অন্যথা
#endif
    {
	++lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	যদি (lp->lnum > curbuf->b_ml.ml_সারি_গুনতি)
	    lp->height = MAXCOL;
	অন্যথা
#ifdef FEAT_FOLDING
	    যদি (hasFolding(lp->lnum, রিক্ত, &lp->lnum))
	    {
		// Add a closed fold.
		lp->height = 1;
	    }
	    অন্যথা
#endif
	    lp->height = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    শূন্য
scroll_cursor_শীর্ষ(পূর্ণাঙ্ক min_scroll, পূর্ণাঙ্ক always)
{
    পূর্ণাঙ্ক		scrolled = 0;
    পূর্ণাঙ্ক		extra = 0;
    পূর্ণাঙ্ক		used;
    পূর্ণাঙ্ক		i;
    linenr_T	শীর্ষ;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    পূর্ণাঙ্ক		old_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#endif
    linenr_T	new_topline;
    পূর্ণাঙ্ক		off = get_scrolloff_মান();

    যদি (mouse_dragging > 0)
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
    যদি (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = used;

#ifdef FEAT_FOLDING
    যদি (hasFolding(curwin->w_cursor.lnum, &শীর্ষ, &bot))
    {
	--শীর্ষ;
	++bot;
    }
    অন্যথা
#endif
    {
	শীর্ষ = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = শীর্ষ + 1;

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
    যাবত্ (শীর্ষ > 0)
    {
#ifdef FEAT_FOLDING
	যদি (hasFolding(শীর্ষ, &শীর্ষ, রিক্ত))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	অন্যথা
#endif
	    i = PLINES_NOFILL(শীর্ষ);
	যদি (শীর্ষ < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	যদি ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    ভঙ্গ;

	used += i;
	যদি (extra + i <= off && bot < curbuf->b_ml.ml_সারি_গুনতি)
	{
#ifdef FEAT_FOLDING
	    যদি (hasFolding(bot, রিক্ত, &bot))
		// count one logical line for a sequence of folded lines
		++used;
	    অন্যথা
#endif
		used += plines(bot);
	}
	যদি (used > curwin->w_height)
	    ভঙ্গ;

	extra += i;
	new_topline = শীর্ষ;
	--শীর্ষ;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    যদি (used > curwin->w_height)
	scroll_cursor_halfway(FALSE, FALSE);
    অন্যথা
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	যদি (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	যদি (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	যদি (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    যদি (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#endif
	যদি (curwin->w_topline != old_topline)
	    reset_skipcol();
	অন্যথা যদি (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    যদি (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		পূর্ণাঙ্ক	width1 = curwin->w_width - curwin_col_off();
		পূর্ণাঙ্ক	width2 = width1 + curwin_col_off2();
		পূর্ণাঙ্ক	plines_off = 0;
		পূর্ণাঙ্ক	skipcol;

		যদি (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		যদি (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	যদি (curwin->w_topline != old_topline
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
    শূন্য
set_empty_rows(win_T *wp, পূর্ণাঙ্ক used)
{
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    যদি (used == 0)
	wp->w_empty_rows = 0;	// single line that doesn't fit
    অন্যথা
    {
	wp->w_empty_rows = wp->w_height - used;
#ifdef FEAT_DIFF
	যদি (wp->w_botline <= wp->w_buffer->b_ml.ml_সারি_গুনতি)
	{
	    wp->w_filler_rows = diff_check_fill(wp, wp->w_botline);
	    যদি (wp->w_empty_rows > wp->w_filler_rows)
		wp->w_empty_rows -= wp->w_filler_rows;
	    অন্যথা
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
    শূন্য
scroll_cursor_bot(পূর্ণাঙ্ক min_scroll, পূর্ণাঙ্ক set_topbot)
{
    পূর্ণাঙ্ক		used;
    পূর্ণাঙ্ক		scrolled = 0;
    পূর্ণাঙ্ক		extra = 0;
    পূর্ণাঙ্ক		i;
    linenr_T	সারি_গুনতি;
    linenr_T	old_topline = curwin->w_topline;
    পূর্ণাঙ্ক		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#ifdef FEAT_DIFF
    পূর্ণাঙ্ক		old_topfill = curwin->w_topfill;
    পূর্ণাঙ্ক		fill_below_window;
#endif
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    পূর্ণাঙ্ক		old_empty_rows = curwin->w_empty_rows;
    linenr_T	cln;		    // Cursor Line Number
    long	so = get_scrolloff_মান();
    পূর্ণাঙ্ক		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    যদি (set_topbot)
    {
	used = 0;
	linenr_T cln_last = cln;
#ifdef FEAT_FOLDING
	(শূন্য)hasFolding(cln, রিক্ত, &cln_last);
#endif
	curwin->w_botline = cln_last + 1;
	loff.lnum = cln_last + 1;
#ifdef FEAT_DIFF
	loff.fill = 0;
#endif
	যাবত্ (TRUE)
	{
	    topline_back_winheight(&loff, FALSE);
	    যদি (loff.height == MAXCOL)
		ভঙ্গ;
	    যদি (used + loff.height > curwin->w_height)
	    {
		যদি (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    যদি (used < curwin->w_height)
		    {
			পূর্ণাঙ্ক plines_offset = used + loff.height
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
		ভঙ্গ;
	    }
#ifdef FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#endif
	    curwin->w_topline = loff.lnum;
	    used += loff.height;
	}

	set_empty_rows(curwin, used);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	যদি (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#endif
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    যদি (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    অন্যথা
		reset_skipcol();
	}
    }
    অন্যথা
	validate_botline();

    // The lines of the cursor line itself are always used.
#ifdef FEAT_DIFF
    used = plines_nofill(cln);
#অন্যথা
    validate_cheight();
    used = curwin->w_cline_height;
#endif

    যদি (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	পূর্ণাঙ্ক upto_cursor = plines_win_col(curwin, cln, curwin->w_cursor.col);

	যদি (upto_cursor < used)
	    used = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    যদি (cln >= curwin->w_botline)
    {
	scrolled = used;
	যদি (cln == curwin->w_botline)
	    scrolled -= curwin->w_empty_rows;
	যদি (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    পূর্ণাঙ্ক শীর্ষ_plines =
#ifdef FEAT_DIFF
			    plines_win_nofill
#অন্যথা
			    plines_win
#endif
					(curwin, curwin->w_topline, FALSE);
	    পূর্ণাঙ্ক width1 = curwin->w_width - curwin_col_off();

	    যদি (width1 > 0)
	    {
		পূর্ণাঙ্ক width2 = width1 + curwin_col_off2();
		পূর্ণাঙ্ক skip_সারিগুলি = 0;

		// A similar formula is used in curs_columns().
		যদি (curwin->w_skipcol > width1)
		    skip_সারিগুলি += (curwin->w_skipcol - width1) / width2 + 1;
		অন্যথা যদি (curwin->w_skipcol > 0)
		    skip_সারিগুলি = 1;

		শীর্ষ_plines -= skip_সারিগুলি;
		যদি (শীর্ষ_plines > curwin->w_height)
		{
		    scrolled += (শীর্ষ_plines - curwin->w_height);
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
    যদি (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
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

    যাবত্ (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	যদি ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_সারি_গুনতি)
		&& loff.lnum <= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_below_window)
#endif
		)
	    ভঙ্গ;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_back(&loff);
	যদি (loff.height == MAXCOL)
	    used = MAXCOL;
	অন্যথা
	    used += loff.height;
	যদি (used > curwin->w_height)
	    ভঙ্গ;
	যদি (loff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_below_window)
#endif
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.height;
	    যদি (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_empty_rows;
	}

	যদি (boff.lnum < curbuf->b_ml.ml_সারি_গুনতি)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    used += boff.height;
	    যদি (used > curwin->w_height)
		ভঙ্গ;
	    যদি (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.height;
		যদি (boff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_rows)
#endif
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.height;
		    যদি (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_empty_rows;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    যদি (scrolled <= 0)
	সারি_গুনতি = 0;
    // more than a screenfull, don't scroll but redraw
    অন্যথা যদি (used > curwin->w_height)
	সারি_গুনতি = used;
    // scroll minimal number of lines
    অন্যথা
    {
	সারি_গুনতি = 0;
#ifdef FEAT_DIFF
	boff.fill = curwin->w_topfill;
#endif
	boff.lnum = curwin->w_topline - 1;
	প্রতি (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.height;
	    ++সারি_গুনতি;
	}
	যদি (i < scrolled)	// below curwin->w_botline, don't scroll
	    সারি_গুনতি = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    যদি (সারি_গুনতি >= curwin->w_height && সারি_গুনতি > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    অন্যথা যদি (সারি_গুনতি > 0)
    {
	যদি (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	অন্যথা
	    scrollup(সারি_গুনতি, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    যদি (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && set_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_empty_rows = old_empty_rows;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    যদি (set_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    শূন্য
scroll_cursor_halfway(পূর্ণাঙ্ক atend, পূর্ণাঙ্ক prefer_above)
{
    পূর্ণাঙ্ক		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#ifdef FEAT_DIFF
    পূর্ণাঙ্ক		topfill = 0;
#endif
    পূর্ণাঙ্ক		below = 0;
    পূর্ণাঙ্ক		used;
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
    (শূন্য)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#endif
#ifdef FEAT_DIFF
    used = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#অন্যথা
    used = plines(loff.lnum);
#endif
    topline = loff.lnum;

    পূর্ণাঙ্ক want_height;
    পূর্ণাঙ্ক do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    যদি (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	যদি (atend)
	{
	    want_height = (curwin->w_height - used) / 2;
	    used = 0;
	}
	অন্যথা
	    want_height = curwin->w_height;
    }

    যাবত্ (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	যদি (do_sms)
	{
	    topline_back_winheight(&loff, FALSE);
	    যদি (loff.height == MAXCOL)
		ভঙ্গ;
	    used += loff.height;
	    যদি (!atend && boff.lnum < curbuf->b_ml.ml_সারি_গুনতি)
	    {
		botline_forw(&boff);
		used += boff.height;
	    }
	    যদি (used > want_height)
	    {
		যদি (used - loff.height < want_height)
		{
		    topline = loff.lnum;
#ifdef FEAT_DIFF
		    topfill = loff.fill;
#endif
		    skipcol = skipcol_from_plines(curwin, used - want_height);
		}
		ভঙ্গ;
	    }
	    topline = loff.lnum;
#ifdef FEAT_DIFF
	    topfill = loff.fill;
#endif
	    অনুবর্ত;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	পূর্ণাঙ্ক done = FALSE;
	প্রতি (পূর্ণাঙ্ক round = 1; round <= 2; ++round)
	{
	    যদি (prefer_above ? (round == 2 && below < above)
			     : (round == 1 && below <= above))
	    {
		// add a line below the cursor
		যদি (boff.lnum < curbuf->b_ml.ml_সারি_গুনতি)
		{
		    botline_forw(&boff);
		    used += boff.height;
		    যদি (used > curwin->w_height)
		    {
			done = TRUE;
			ভঙ্গ;
		    }
		    below += boff.height;
		}
		অন্যথা
		{
		    ++below;	    // count a "~" line
		    যদি (atend)
			++used;
		}
	    }

	    যদি (prefer_above ? (round == 1 && below >= above)
			     : (round == 1 && below > above))
	    {
		// add a line above the cursor
		topline_back(&loff);
		যদি (loff.height == MAXCOL)
		    used = MAXCOL;
		অন্যথা
		    used += loff.height;
		যদি (used > curwin->w_height)
		{
		    done = TRUE;
		    ভঙ্গ;
		}
		above += loff.height;
		topline = loff.lnum;
#ifdef FEAT_DIFF
		topfill = loff.fill;
#endif
	    }
	}
	যদি (done)
	    ভঙ্গ;
    }

#ifdef FEAT_FOLDING
    যদি (!hasFolding(topline, &curwin->w_topline, রিক্ত))
#endif
    {
	যদি (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    যদি (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    অন্যথা যদি (do_sms)
		reset_skipcol();
	}
    }
#ifdef FEAT_DIFF
    curwin->w_topfill = topfill;
    যদি (old_topline > curwin->w_topline + curwin->w_height)
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
    শূন্য
cursor_correct(শূন্য)
{
    পূর্ণাঙ্ক		above = 0;	    // screen lines above topline
    linenr_T	topline;
    পূর্ণাঙ্ক		below = 0;	    // screen lines below botline
    linenr_T	botline;
    পূর্ণাঙ্ক		above_wanted, below_wanted;
    linenr_T	cln;		    // Cursor Line Number
    পূর্ণাঙ্ক		max_off;
    long	so = get_scrolloff_মান();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    below_wanted = so;
    যদি (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	below_wanted = mouse_dragging - 1;
    }
    যদি (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_height / 2;
	যদি (below_wanted > max_off)
	    below_wanted = max_off;
    }
    validate_botline();
    যদি (curwin->w_botline == curbuf->b_ml.ml_সারি_গুনতি + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	below_wanted = 0;
	max_off = (curwin->w_height - 1) / 2;
	যদি (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    যদি (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - below_wanted
#ifdef FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#endif
	    )
	প্রত্যাগম;

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
    যাবত্ ((above < above_wanted || below < below_wanted) && topline < botline)
    {
	যদি (below < below_wanted && (below <= above || above >= above_wanted))
	{
#ifdef FEAT_FOLDING
	    যদি (hasFolding(botline, &botline, রিক্ত))
		++below;
	    অন্যথা
#endif
		below += plines(botline);
	    --botline;
	}
	যদি (above < above_wanted && (above < below || below >= below_wanted))
	{
#ifdef FEAT_FOLDING
	    যদি (hasFolding(topline, রিক্ত, &topline))
		++above;
	    অন্যথা
#endif
		above += PLINES_NOFILL(topline);
#ifdef FEAT_DIFF
	    // Count filler lines below this line as context.
	    যদি (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#endif
	    ++topline;
	}
    }
    যদি (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    অন্যথা যদি (topline > botline)
	curwin->w_cursor.lnum = botline;
    অন্যথা
    {
	যদি (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	যদি (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_সারি_গুনতি)
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
স্থির পূর্ণাঙ্ক get_scroll_overlap(পূর্ণাঙ্ক dir)
{
    lineoff_T loff;
    পূর্ণাঙ্ক	    min_height = curwin->w_height - 2;

    validate_botline();
    যদি ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_সারি_গুনতি))
	প্রত্যাগম min_height + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#ifdef FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_rows : curwin->w_topfill);
    loff.height = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#অন্যথা
    loff.height = plines(loff.lnum);
#endif

    পূর্ণাঙ্ক h1 = loff.height;
    যদি (h1 > min_height)
	প্রত্যাগম min_height + 2;  // no overlap
    যদি (dir == FORWARD)
	topline_back(&loff);
    অন্যথা
	botline_forw(&loff);

    পূর্ণাঙ্ক h2 = loff.height;
    যদি (h2 == MAXCOL || h2 + h1 > min_height)
	প্রত্যাগম min_height + 2;  // no overlap
    যদি (dir == FORWARD)
	topline_back(&loff);
    অন্যথা
	botline_forw(&loff);

    পূর্ণাঙ্ক h3 = loff.height;
    যদি (h3 == MAXCOL || h3 + h2 > min_height)
	প্রত্যাগম min_height + 2;  // no overlap
    যদি (dir == FORWARD)
	topline_back(&loff);
    অন্যথা
	botline_forw(&loff);

    পূর্ণাঙ্ক h4 = loff.height;
    যদি (h4 == MAXCOL || h4 + h3 + h2 > min_height || h3 + h2 + h1 > min_height)
	প্রত্যাগম min_height + 1;  // 1 line overlap
    অন্যথা
	প্রত্যাগম min_height;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
স্থির পূর্ণাঙ্ক scroll_with_sms(পূর্ণাঙ্ক dir, long গুনতি, long *curscount)
{
    পূর্ণাঙ্ক		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    পূর্ণাঙ্ক		prev_topfill = curwin->w_topfill;
#endif

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, গুনতি);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    যদি (!prev_sms && curwin->w_skipcol > 0)
    {
	পূর্ণাঙ্ক fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	যদি (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	পূর্ণাঙ্ক width1 = curwin->w_width - curwin_col_off();
	পূর্ণাঙ্ক width2 = width1 + curwin_col_off2();
	গুনতি = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	যদি (fixdir == FORWARD)
	    গুনতি = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, গুনতি);
	*curscount += গুনতি * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    প্রত্যাগম curwin->w_topline != prev_topline
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
    পূর্ণাঙ্ক
pagescroll(পূর্ণাঙ্ক dir, long গুনতি, পূর্ণাঙ্ক half)
{
    পূর্ণাঙ্ক		did_move = FALSE;
    পূর্ণাঙ্ক		buflen = curbuf->b_ml.ml_সারি_গুনতি;
    colnr_T	prev_col = curwin->w_cursor.col;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    যদি (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	যদি (গুনতি)
	    curwin->w_p_scr = MIN(curwin->w_height, গুনতি);
	গুনতি = MIN(curwin->w_height, curwin->w_p_scr);

	long curscount = গুনতি;
	// Adjust count so as to not reveal end of buffer lines.
	যদি (dir == FORWARD
		    && (curwin->w_topline + curwin->w_height + গুনতি > buflen
#ifdef FEAT_FOLDING
							|| hasAnyFolding(curwin)
#endif
	   ))
	{
	    পূর্ণাঙ্ক n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    যদি (n - গুনতি < curwin->w_height && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_height + গুনতি);
	    যদি (n < curwin->w_height + গুনতি)
		গুনতি = n - curwin->w_height;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	যদি (গুনতি > 0)
	{
	    did_move = scroll_with_sms(dir, গুনতি, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.col = prev_col;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	যদি (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	অন্যথা যদি (dir == FORWARD)
	    cursor_down_inner(curwin, curscount);
	অন্যথা
	    cursor_up_inner(curwin, curscount);
    }
    অন্যথা
    {
	// Scroll [count] times 'window' or current window height lines.
	গুনতি *= ((ONE_WINDOW && p_window > 0 && p_window < Rows - 1) ?
				MAX(1, p_window - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, গুনতি, &গুনতি);

	যদি (did_move)
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

    যদি (get_scrolloff_মান() > 0)
	cursor_correct();
#ifdef FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#endif
    did_move = did_move
	|| prev_col != curwin->w_cursor.col
	|| prev_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    যদি (!did_move)
	beep_flush();
    অন্যথা যদি (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    অন্যথা যদি (p_sol)
	nv_g_home_m_cmd(&ca);

    প্রত্যাগম did_move ? OK : FAIL;
}

    শূন্য
do_check_cursorbind(শূন্য)
{
    স্থির win_T	*prev_curwin = রিক্ত;
    স্থির buf_T	*prev_curbuf = রিক্ত;
    স্থির varnumber_T	prev_changedtick = 0;
    স্থির pos_T	prev_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    যদি (curwin == prev_curwin && curbuf == prev_curbuf
	    && CHANGEDTICK(curbuf) == prev_changedtick
	    && EQUAL_POS(curwin->w_cursor, prev_cursor))
	প্রত্যাগম;
    prev_curwin = curwin;
    prev_curbuf = curbuf;
    prev_changedtick = CHANGEDTICK(curbuf);
    prev_cursor = curwin->w_cursor;

    linenr_T	সারি = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    পূর্ণাঙ্ক		set_curswant = curwin->w_set_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    পূর্ণাঙ্ক		old_VIsual_select = VIsual_select;
    পূর্ণাঙ্ক		old_VIsual_active = VIsual_active;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_active = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_buffer;
	// skip original window and windows with 'nocursorbind'
	যদি (curwin != old_curwin && curwin->w_p_crb)
	{
#ifdef FEAT_DIFF
	    যদি (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_সারি(old_curbuf, সারি);
	    অন্যথা
#endif
		curwin->w_cursor.lnum = সারি;
	    curwin->w_cursor.col = col;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_set_curswant = set_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    পূর্ণাঙ্ক restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    যদি (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    যদি (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    যদি (!curwin->w_p_scb)
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
