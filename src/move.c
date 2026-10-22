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

#include "vim.h"

static void redraw_for_cursorline(win_T *wp);
static цео scrolljump_value(void);
static цео check_top_offset(void);
static void curs_rows(win_T *wp);

typedef struct
{
    linenr_T	    lnum;	// line number
#ifdef FEAT_DIFF
    цео		    fill;	// filler lines
#endif
    цео		    height;	// height of added line
} lineoff_T;

static void topline_back(lineoff_T *lp);
static void botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    static цео
adjust_plines_for_skipcol(win_T *wp)
{
    ако (wp->w_skipcol == 0)
	врати 0;

    цео width = wp->w_width - win_col_off(wp);
    цео w2 = width + win_col_off2(wp);
    ако (wp->w_skipcol >= width && w2 > 0)
	врати (wp->w_skipcol - width) / w2 + 1;

    врати 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    цео
plines_correct_topline(win_T *wp, linenr_T lnum, цео limit_winheight)
{
    цео n;
#ifdef FEAT_DIFF
    ако (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    инако
#endif
	n = plines_win(wp, lnum, FALSE);
    ако (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    ако (limit_winheight && n > wp->w_height)
	n = wp->w_height;
    врати n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    static void
comp_botline(win_T *wp)
{
    цео		n;
    linenr_T	lnum;
    цео		done;
    цео		i = 0;
    цео		use_cache;
#ifdef FEAT_FOLDING
    linenr_T    last;
    цео		folded;
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
		    && wp->w_lines_valid > 0
		    && wp->w_lines[0].wl_lnum <= wp->w_topline;

    ако (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	done = wp->w_cline_row;
    }
    инако
    {
	lnum = wp->w_topline;
	done = 0;
    }

    // Find the w_lines[] entry for the starting line.
    ако (use_cache)
	док (i < wp->w_lines_valid && wp->w_lines[i].wl_lnum < lnum)
	    ++i;

    за ( ; lnum <= wp->w_buffer->b_ml.ml_line_count; ++i)
    {
	цео	valid = FALSE;

#ifdef FEAT_FOLDING
	last = lnum;
	folded = FALSE;
#endif
	// Try to use the size from the previous redraw.
	ако (use_cache && i < wp->w_lines_valid)
	{
	    ако (wp->w_lines[i].wl_lnum < lnum || !wp->w_lines[i].wl_valid)
		continue;		// skip changed or deleted lines
	    ако (wp->w_lines[i].wl_lnum == lnum)
		valid = TRUE;
	    инако // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	ако (valid)
	{
	    n = wp->w_lines[i].wl_size;
#ifdef FEAT_FOLDING
	    folded = wp->w_lines[i].wl_folded;
	    last = wp->w_lines[i].wl_lastlnum;
#endif
	}
	инако
	{
#ifdef FEAT_FOLDING
	    ако (hasFoldingWin(wp, lnum, NULL, &last, TRUE, NULL))
	    {
		n = 1;
		folded = TRUE;
	    }
	    инако
#endif
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	ако (
#ifdef FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && last >= wp->w_cursor.lnum
#инако
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
	ако (done + n > wp->w_height)
	    break;
	done += n;
#ifdef FEAT_FOLDING
	lnum = last + 1;
#инако
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
    static void
redraw_for_cursorline(win_T *wp)
{
    ако ((wp->w_p_rnu
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
    static void
redraw_for_cursorcolumn(win_T *wp)
{
    ако ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	ако (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	инако ако (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#endif

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    void
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
    цео
sms_marker_overlap(win_T *wp, цео extra2)
{
    ако (extra2 == -1)
	extra2 = win_col_off(wp) - win_col_off2(wp);
#ако defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    ако (*get_showbreak_value(wp) != NUL)
	врати 0;
#endif
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    ако (wp->w_p_list && wp->w_lcs_chars.prec)
	врати 1;

    врати extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    static цео
skipcol_from_plines(win_T *wp, цео plines_off)
{
    цео width1 = wp->w_width - win_col_off(wp);

    цео skipcol = 0;
    ако (plines_off > 0)
	skipcol += width1;
    ако (plines_off > 1)
	skipcol += (width1 + win_col_off2(wp)) * (plines_off - 1);
    врати skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    static void
reset_skipcol(void)
{
    ако (curwin->w_skipcol == 0)
	врати;

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
    void
update_topline_redraw(void)
{
    update_topline();
    ако (must_redraw)
	update_screen(0);
}

/*
 * Return true when 'scrolloffpad' may augment 'scrolloff'.
 * This only applies to automatic cursor visibility correction.
 * For now 'scrolloffpad' is treated as boolean: 0 disables, > 0 enables.
 */
    static bool
use_scrolloffpad(void)
{
    врати get_scrolloff_value() > 0 && get_scrolloffpad_value() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    static bool
scrolloffpad_eof_pressure(linenr_T lnum, long so)
{
    ако (!use_scrolloffpad() || so <= 0)
	врати false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    врати lnum > curbuf->b_ml.ml_line_count - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    void
update_topline(void)
{
    long	line_count;
    цео		halfheight;
    цео		n;
#ifdef FEAT_FOLDING
    linenr_T	lnum;
#endif
    цео		check_topline = FALSE;
    цео		check_botline = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    цео		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    ако (skip_update_topline)
	врати;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    ако (!screen_valid(TRUE) || curwin->w_height == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	врати;
    }

    check_cursor_moved(curwin);
    ако (curwin->w_valid & VALID_TOPLINE)
	врати;

    // When dragging with the mouse, don't scroll that quickly
    ако (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    цео old_topfill = curwin->w_topfill;
#endif

    /*
     * If the buffer is empty, always set topline to 1.
     */
    ако (BUFEMPTY())		// special case - file is empty
    {
	ако (curwin->w_topline != 1)
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
    инако
    {
	ако (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    ако (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    инако ако (check_top_offset())
		check_topline = TRUE;
	    инако ако (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		цео overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, NULL, NULL, 0);
		overlap = sms_marker_overlap(curwin, -1);
		ако (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#ifdef FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	ако (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#endif

	ако (check_topline)
	{
	    halfheight = curwin->w_height / 2 - 1;
	    ако (halfheight < 2)
		halfheight = 2;

#ifdef FEAT_FOLDING
	    ако (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		за (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    ако (lnum >= curbuf->b_ml.ml_line_count || n >= halfheight)
			break;
		    (void)hasFolding(lnum, NULL, &lnum);
		}
	    }
	    инако
#endif
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    цео min_scroll = scrolljump_value();
	    ако (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    инако ако (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    инако
	    {
		scroll_cursor_top(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	инако
	{
#ifdef FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
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
    ако (check_botline)
    {
	ако (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	ако (curwin->w_botline <= curbuf->b_ml.ml_line_count || use_scrolloffpad())
	{
	    ако (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		ако (((long)curwin->w_cursor.lnum
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
		    (void)hasFolding(loff.lnum, NULL, &loff.lnum);
#endif
#ifdef FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_rows;
#endif
		    loff.height = 0;
		    док (loff.lnum < curwin->w_botline
#ifdef FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#endif
			    )
		    {
			n += loff.height;
			ако (n >= *so_ptr)
			    break;
			botline_forw(&loff);
		    }
		    ако (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		инако
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    ако (check_botline)
	    {
#ifdef FEAT_FOLDING
		ако (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    line_count = 0;
		    за (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++line_count;
			// stop at end of file or when we know we are far off
			ако (lnum <= 0 || line_count > curwin->w_height + 1)
			    break;
			(void)hasFolding(lnum, &lnum, NULL);
		    }
		}
		инако
#endif
		    line_count = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		ако (line_count <= curwin->w_height + 1)
		{
		    ако (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    инако
			scroll_cursor_bot(scrolljump_value(), FALSE);
		}
		инако
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    ако (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#endif
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	ако (!curwin->w_p_sms)
	    reset_skipcol();
	инако ако (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	ако (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    static цео
scrolljump_value(void)
{
    ако (p_sj >= 0)
	врати (цео)p_sj;
    врати (curwin->w_height * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    static цео
check_top_offset(void)
{
    lineoff_T	loff;
    цео		n;
    long	so = get_scrolloff_value();

    ако (curwin->w_cursor.lnum < curwin->w_topline + so
#ifdef FEAT_FOLDING
		    || hasAnyFolding(curwin)
#endif
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#инако
	n = 0;
#endif
	// Count the visible screen lines above the cursor line.
	док (n < so)
	{
	    topline_back(&loff);
	    // Stop when included a line above the window.
	    ако (loff.lnum < curwin->w_topline
#ifdef FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#endif
		    )
		break;
	    n += loff.height;
	}
	ако (n < so)
	    врати TRUE;
    }
    врати FALSE;
}

/*
 * Update w_curswant.
 */
    void
update_curswant_force(void)
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
    void
update_curswant(void)
{
    ако (curwin->w_set_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    void
check_cursor_moved(win_T *wp)
{
    ако (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    инако ако (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    инако ако (wp->w_cursor.col != wp->w_valid_cursor.col
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
    void
changed_window_setting(void)
{
    changed_window_setting_win(curwin);
}

    void
changed_window_setting_win(win_T *wp)
{
    wp->w_lines_valid = 0;
    changed_line_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#ако defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    void
changed_window_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ако (wp->w_buffer == buf)
	    changed_window_setting_win(wp);
}
#endif

/*
 * Call changed_window_setting_win() for every window.
 */
    void
changed_window_setting_all(void)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	changed_window_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    void
set_topline(win_T *wp, linenr_T lnum)
{
#ifdef FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#endif

#ifdef FEAT_FOLDING
    // go to first of folded lines
    (void)hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
#endif
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    ако (wp->w_botline > wp->w_buffer->b_ml.ml_line_count + 1)
	wp->w_botline = wp->w_buffer->b_ml.ml_line_count + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_set = true;
#ifdef FEAT_DIFF
    ако (lnum != prev_topline)
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
    void
changed_cline_bef_curs(void)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    void
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
    void
changed_line_abv_curs(void)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    void
changed_line_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#ако defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    void
changed_line_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ако (wp->w_buffer == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#endif

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    void
validate_botline(void)
{
    validate_botline_win(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    void
validate_botline_win(win_T *wp)
{
    ако (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    void
invalidate_botline(void)
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    void
invalidate_botline_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    void
approximate_botline_win(
    win_T	*wp)
{
    wp->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    цео
cursor_valid(void)
{
    check_cursor_moved(curwin);
    врати ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    void
validate_cursor(void)
{
    check_cursor_lnum();
    check_cursor_moved(curwin);
    ако ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#ако defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    void
validate_cline_row(void)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    ако (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);
}
#endif

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    static void
curs_rows(win_T *wp)
{
    linenr_T	lnum;
    цео		i;
    цео		all_invalid;
    цео		valid;
#ifdef FEAT_FOLDING
    long	fold_count;
#endif

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_lines_valid == 0
			|| wp->w_lines[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_row = 0;
    за (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	ако (!all_invalid && i < wp->w_lines_valid)
	{
	    ако (wp->w_lines[i].wl_lnum < lnum || !wp->w_lines[i].wl_valid)
		continue;		// skip changed or deleted lines
	    ако (wp->w_lines[i].wl_lnum == lnum)
	    {
#ifdef FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		ако (!wp->w_buffer->b_mod_set
			|| wp->w_lines[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_buffer->b_mod_top
					     > wp->w_lines[i].wl_lastlnum + 1)
#endif
		valid = TRUE;
	    }
	    инако ако (wp->w_lines[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	ако (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#ifdef FEAT_DIFF
					    && !wp->w_p_diff
#endif
	       )))
	{
#ifdef FEAT_FOLDING
	    lnum = wp->w_lines[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    ако (lnum > wp->w_cursor.lnum)
		break;
#инако
	    ++lnum;
#endif
	    wp->w_cline_row += wp->w_lines[i].wl_size;
	}
	инако
	{
#ifdef FEAT_FOLDING
	    fold_count = foldedCount(wp, lnum, NULL);
	    ако (fold_count)
	    {
		lnum += fold_count;
		ако (lnum > wp->w_cursor.lnum)
		    break;
		++wp->w_cline_row;
	    }
	    инако
#endif
	    {
		wp->w_cline_row += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    ако (!(wp->w_valid & VALID_CHEIGHT))
    {
	ако (all_invalid
		|| i == wp->w_lines_valid
		|| (i < wp->w_lines_valid
		    && (!wp->w_lines[i].wl_valid
			|| wp->w_lines[i].wl_lnum != wp->w_cursor.lnum)))
	{
#ifdef FEAT_DIFF
	    ако (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_height = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    инако
#endif
		wp->w_cline_height = plines_win(wp, wp->w_cursor.lnum, TRUE);
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      NULL, NULL, TRUE, NULL);
#endif
	}
	инако ако (i > wp->w_lines_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_height = 0;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      NULL, NULL, TRUE, NULL);
#endif
	}
	инако
	{
	    wp->w_cline_height = wp->w_lines[i].wl_size;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = wp->w_lines[i].wl_folded;
#endif
	}
    }

    redraw_for_cursorline(curwin);
    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;
}

/*
 * Validate curwin->w_virtcol only.
 */
    void
validate_virtcol(void)
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    void
validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);

    ако (wp->w_valid & VALID_VIRTCOL)
	врати;

#ifdef FEAT_PROP_POPUP
    wp->w_virtcol_first_char = 0;
#endif
    getvvcol(wp, &wp->w_cursor, NULL, &(wp->w_virtcol), NULL,
							 GETVCOL_FOR_VIRTCOL);
#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#endif
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    void
validate_cheight(void)
{
    check_cursor_moved(curwin);

    ако (curwin->w_valid & VALID_CHEIGHT)
	врати;

#ifdef FEAT_DIFF
    ако (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_height = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    инако
#endif
	curwin->w_cline_height = plines(curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, NULL, NULL);
#endif
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    void
validate_cursor_col(void)
{
    colnr_T off;
    colnr_T col;
    цео     width;

    validate_virtcol();

    ако (curwin->w_valid & VALID_WCOL)
	врати;

    col = curwin->w_virtcol;
    off = curwin_col_off();
    col += off;
    width = curwin->w_width - off + curwin_col_off2();

    // long line wrapping, adjust curwin->w_wrow
    ако (curwin->w_p_wrap
	    && col >= (colnr_T)curwin->w_width
	    && width > 0)
	// use same formula as what is used in curs_columns()
	col -= ((col - curwin->w_width) / width + 1) * width;
    ако (col > (цео)curwin->w_leftcol)
	col -= curwin->w_leftcol;
    инако
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
    цео
win_col_off(win_T *wp)
{
    врати (((wp->w_p_nu || wp->w_p_rnu) ? number_width(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#ifdef FEAT_FOLDING
	    + wp->w_p_fdc
#endif
#ifdef FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#endif
	   );
}

    цео
curwin_col_off(void)
{
    врати win_col_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    цео
win_col_off2(win_T *wp)
{
    ако ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != NULL)
	врати number_width(wp) + 1;
    врати 0;
}

    цео
curwin_col_off2(void)
{
    врати win_col_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    void
curs_columns(
    цео		may_scroll)	// when TRUE, may scroll horizontally
{
    long	diff;
    цео		extra;		// offset for first screen line
    long	off_left, off_right;
    цео		n;
    цео		p_lines;
    цео		width1;		// text width for first screen line
    цео		width2 = 0;	// text width for second and later screen line
    цео		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    long	so = get_scrolloff_value();
    long	siso = get_sidescrolloff_value();
    цео		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    ако (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);

#ifdef FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_first_char = 0;
#endif

    /*
     * Compute the number of virtual columns.
     */
#ifdef FEAT_FOLDING
    ако (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    инако
#endif
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    ако (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_col_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_row;

    width1 = curwin->w_width - extra;
    ако (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_width - 1;
	ако (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_height - 1;
	инако
	    curwin->w_wrow = curwin->w_height - 1 - curwin->w_empty_rows;
    }
    инако ако (curwin->w_p_wrap && curwin->w_width != 0)
    {
	width2 = width1 + curwin_col_off2();

	// skip columns that are not visible
	ако (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    ако (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    инако
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	ако (curwin->w_wcol >= curwin->w_width)
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
    инако ако (may_scroll
#ifdef FEAT_FOLDING
	    && !curwin->w_cline_folded
#endif
	    )
    {
#ifdef FEAT_PROP_POPUP
	ако (curwin->w_virtcol_first_char > 0)
	{
	    цео cols = (curwin->w_width - extra);
	    цео rows = cols > 0 ? curwin->w_virtcol_first_char / cols : 1;

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
	off_left = startcol - curwin->w_leftcol - siso;
	off_right = endcol - curwin->w_leftcol - (curwin->w_width - siso) + 1;
	ако (off_left < 0 || off_right > 0)
	{
	    ако (off_left < 0)
		diff = -off_left;
	    инако
		diff = off_right;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    ако (p_ss == 0 || diff >= width1 / 2 || off_right >= off_left)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    инако
	    {
		ако (diff < p_ss)
		    diff = p_ss;
		ако (off_left < 0)
		    new_leftcol = curwin->w_leftcol - (цео)diff;
		инако
		    new_leftcol = curwin->w_leftcol + (цео)diff;
	    }
	    ако (new_leftcol < 0)
		new_leftcol = 0;
	    ако (new_leftcol != (цео)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    инако ако (curwin->w_wcol > (цео)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    инако
	curwin->w_wcol = 0;

#ifdef FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    ако (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    инако
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#endif

    prev_skipcol = curwin->w_skipcol;

    p_lines = 0;

    ако ((curwin->w_wrow >= curwin->w_height
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_height)
		    && (p_lines =
#ifdef FEAT_DIFF
			plines_win_nofill
#инако
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
	ако (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	ако (p_lines == 0)
	    p_lines = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_lines;
	ако (p_lines > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	инако
	    n = p_lines;
	ако ((colnr_T)n >= curwin->w_height + curwin->w_skipcol / width2 - so)
	    extra += 2;

	ако (extra == 3 || curwin->w_height <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    ако (n > curwin->w_height / 2)
		n -= curwin->w_height / 2;
	    инако
		n = 0;
	    // don't skip more than necessary
	    ако (n > p_lines - curwin->w_height + 1)
		n = p_lines - curwin->w_height + 1;
	    ако (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    инако
		curwin->w_skipcol = 0;
	}
	инако ако (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    ако (extra > 0)
	    {
		ако ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	инако ако (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_height + 1) * width2;
	    док (endcol > curwin->w_virtcol)
		endcol -= width2;
	    ако (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	ако (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - prev_skipcol) / width2;
	инако
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	ако (curwin->w_wrow >= curwin->w_height)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_height + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((цео)prev_skipcol - (цео)curwin->w_skipcol) / width2;
	ако (extra > 0)
	    win_ins_lines(curwin, 0, extra, FALSE, FALSE);
	инако ако (extra < 0)
	    win_del_lines(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    инако ако (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    ако (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#endif
#ако defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ако (popup_is_popup(curwin) && curbuf->b_term != NULL)
    {
	curwin->w_wrow += popup_top_extra(curwin);
	curwin->w_wcol += popup_left_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    инако
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#endif

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#ако defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    void
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	цео	*rowp,	// screen row
	цео	*scolp,	// start screen column
	цео	*ccolp,	// cursor screen column
	цео	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    цео		row = 0;
    colnr_T	coloff = 0;

    ако (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    col;
	цео	    width;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	цео	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
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
	ако (is_folded)
	{
	    row += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	инако
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    col = scol;
	    col += off;
	    width = wp->w_width - off + win_col_off2(wp);

	    // long line wrapping, adjust row
	    ако (wp->w_p_wrap
		    && col >= (colnr_T)wp->w_width
		    && width > 0)
	    {
		// use same formula as what is used in curs_columns()
		цео rowoff = ((col - wp->w_width) / width + 1);
		col -= rowoff * width;
		row += rowoff;
	    }
	    col -= wp->w_leftcol;
	    ако (col >= wp->w_width)
		col = -1;
	    ако (col >= 0 && row >= 0 && row < wp->w_height)
	    {
		coloff = col - scol + wp->w_wincol + 1;
		row += W_WINROW(wp) + 1;
	    }
	    инако
		// character is out of the window
		row = scol = ccol = ecol = 0;
	}
    }
    *rowp = row;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    ако (wp->w_p_rl && row > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	цео endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_width - endoff;
    }
# endif
}
#endif

#ако defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    void
f_screenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    цео		row = 0;
    цео		scol = 0, ccol = 0, ecol = 0;

    ако (rettv_dict_alloc(rettv) == FAIL)
	врати;
    dict = rettv->vval.v_dict;

    ако (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_number_arg(argvars, 2) == FAIL))
	врати;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ако (wp == NULL)
	врати;

    pos.lnum = tv_get_number(&argvars[1]);
    ако (pos.lnum > wp->w_buffer->b_ml.ml_line_count)
    {
	semsg(_(e_invalid_line_number_nr), pos.lnum);
	врати;
    }
    pos.col = tv_get_number(&argvars[2]) - 1;
    ако (pos.col < 0)
	pos.col = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &row, &scol, &ccol, &ecol);

    dict_add_number(dict, "row", row);
    dict_add_number(dict, "col", scol);
    dict_add_number(dict, "curscol", ccol);
    dict_add_number(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    static цео
virtcol2col(win_T *wp, linenr_T lnum, цео vcol)
{
    цео		offset = vcol2col(wp, lnum, vcol - 1, NULL);
    char_u	*line = ml_get_buf(wp->w_buffer, lnum, FALSE);
    char_u	*p = line + offset;

    ако (*p == NUL)
    {
	ако (p == line)  // empty line
	    врати 0;
	// Move to the first byte of the last char.
	MB_PTR_BACK(line, p);
    }
    врати (цео)(p - line + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    void
f_virtcol2col(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    цео		screencol;
    цео		error = FALSE;

    rettv->vval.v_number = -1;

    ако (check_for_number_arg(argvars, 0) == FAIL
	    || check_for_number_arg(argvars, 1) == FAIL
	    || check_for_number_arg(argvars, 2) == FAIL)
	врати;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ако (wp == NULL)
	врати;

    lnum = tv_get_number_chk(&argvars[1], &error);
    ако (error || lnum < 0 || lnum > wp->w_buffer->b_ml.ml_line_count)
	врати;

    screencol = tv_get_number_chk(&argvars[2], &error);
    ако (error || screencol < 0)
	врати;

    rettv->vval.v_number = virtcol2col(wp, lnum, screencol);
}
#endif

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
static void cursor_correct_sms(void)
{
    ако (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	врати;

    long    so = get_scrolloff_value();
    цео	    width1 = curwin->w_width - curwin_col_off();
    цео	    width2 = width1 + curwin_col_off2();
    цео	    so_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    цео	    space_cols = (curwin->w_height - 1) * width2;
    цео	    overlap, top, bot;
    цео	    size = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    ако (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_cols = 0;               // Ignore 'scrolloff' at top of buffer.
    инако ако (so_cols > space_cols / 2)
	so_cols = space_cols / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    док (so_cols > size && so_cols - width2 >= width1 && width1 > 0)
	so_cols -= width2;
    ако (so_cols >= width1 && so_cols > size)
	so_cols -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_width - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    top = curwin->w_skipcol + (so_cols != 0 ? so_cols : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_height - 1) * width2
								    - so_cols;
    validate_virtcol();
    colnr_T col = curwin->w_virtcol;

    ако (col < top)
    {
	ако (col < width1)
	    col += width1;
	док (width2 > 0 && col < top)
	    col += width2;
    }
    инако
	док (width2 > 0 && col >= bot)
	    col -= width2;

    ако (col != curwin->w_virtcol)
    {
	цео rc;

	curwin->w_curswant = col;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	ако (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	{
	    validate_virtcol();
	    ако (curwin->w_virtcol < curwin->w_skipcol + overlap)
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
    void
scroll_redraw(цео up, long count)
{
    linenr_T	prev_topline = curwin->w_topline;
    цео		prev_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    цео		prev_topfill = curwin->w_topfill;
#endif
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    ако (up)
	scrollup(count, TRUE);
    инако
	scrolldown(count, TRUE);
    ако (get_scrolloff_value() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	док (curwin->w_topline == prev_topline
		&& curwin->w_skipcol == prev_skipcol
#ifdef FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#endif
		)
	{
	    ако (up)
	    {
		ако (curwin->w_cursor.lnum > prev_lnum
			|| cursor_down(1L, FALSE) == FAIL)
		    break;
	    }
	    инако
	    {
		ако (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_up(1L, FALSE) == FAIL)
		    break;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    ако (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    void
scrolldown(
    long	line_count,
    цео		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    long	done = 0;	// total # of physical lines done
    цео		wrow;
    цео		moved = FALSE;
    цео		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    цео		width1 = 0;
    цео		width2 = 0;

    ако (do_sms)
    {
	width1 = curwin->w_width - curwin_col_off();
	width2 = width1 + curwin_col_off2();
    }

#ifdef FEAT_FOLDING
    linenr_T	first;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
    validate_cursor();		// w_wrow needs to be valid
    за (цео todo = line_count; todo > 0; --todo)
    {
#ifdef FEAT_DIFF
	ако (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_height - 1)
	{
	    ++curwin->w_topfill;
	    ++done;
	}
	инако
#endif
	{
	    // break when at the very top
	    ако (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		break;
	    ако (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		ако (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		инако
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++done;
	    }
	    инако
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#ifdef FEAT_DIFF
		curwin->w_topfill = 0;
#endif
#ifdef FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		ако (hasFolding(curwin->w_topline, &first, NULL))
		{
		    ++done;
		    ако (!byfold)
			todo -= curwin->w_topline - first - 1;
		    curwin->w_botline -= curwin->w_topline - first;
		    curwin->w_topline = first;
		}
		инако
#endif
		ако (do_sms)
		{
		    цео size = linetabsize_eol(curwin, curwin->w_topline);
		    ако (size > width1)
		    {
			curwin->w_skipcol = width1;
			size -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    док (size > width2)
		    {
			curwin->w_skipcol += width2;
			size -= width2;
		    }
		    ++done;
		}
		инако
		    done += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += done;		// keep w_wrow updated
    curwin->w_cline_row += done;	// keep w_cline_row updated

#ifdef FEAT_DIFF
    ако (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_row = 0;
    check_topfill(curwin, TRUE);
#endif

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    ако (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    док (wrow >= curwin->w_height && curwin->w_cursor.lnum > 1)
    {
#ifdef FEAT_FOLDING
	ако (hasFolding(curwin->w_cursor.lnum, &first, NULL))
	{
	    --wrow;
	    ако (first == 1)
		curwin->w_cursor.lnum = 1;
	    инако
		curwin->w_cursor.lnum = first - 1;
	}
	инако
#endif
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    ако (moved)
    {
#ifdef FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#endif
	coladvance(curwin->w_curswant);
    }
    ако (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    void
scrollup(
    long	line_count,
    цео		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    цео		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    ако (do_sms
#ifdef FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#endif
#ifdef FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#endif
       )
    {
	цео	    width1 = curwin->w_width - curwin_col_off();
	цео	    width2 = width1 + curwin_col_off2();
	цео	    size = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	ако (do_sms)
	    size = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	за (цео todo = line_count; todo > 0; --todo)
	{
#ifdef FEAT_DIFF
	    ако (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    инако
#endif
	    {
		linenr_T lnum = curwin->w_topline;

#ifdef FEAT_FOLDING
		ако (byfold)
		    // for a closed fold: go to the last line in the fold
		    (void)hasFolding(lnum, NULL, &lnum);
#endif
		ако (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    цео add = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += add;
		    ако (curwin->w_skipcol >= size)
		    {
			ако (lnum == curbuf->b_ml.ml_line_count)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= add;
			    break;
			}
			++lnum;
		    }
		}
		инако
		{
		    ако (lnum >= curbuf->b_ml.ml_line_count)
			break;
		    ++lnum;
		}

		ако (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#ifdef FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#endif
		    curwin->w_skipcol = 0;
		    ако (todo > 1 && do_sms)
			size = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	ако (prev_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    инако
    {
	curwin->w_topline += line_count;
	curwin->w_botline += line_count;	// approximate w_botline
    }

    ако (curwin->w_topline > curbuf->b_ml.ml_line_count)
	curwin->w_topline = curbuf->b_ml.ml_line_count;
    ако (curwin->w_botline > curbuf->b_ml.ml_line_count + 1)
	curwin->w_botline = curbuf->b_ml.ml_line_count + 1;

#ifdef FEAT_DIFF
    check_topfill(curwin, FALSE);
#endif

#ifdef FEAT_FOLDING
    ако (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    ако (curwin->w_cursor.lnum < curwin->w_topline)
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
    void
adjust_skipcol(void)
{
    ако (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	врати;

    цео	    width1 = curwin->w_width - curwin_col_off();
    ако (width1 <= 0)
	врати;  // no text will be displayed

    цео	    width2 = width1 + curwin_col_off2();
    long    so = get_scrolloff_value();
    цео	    scrolloff_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    цео	    scrolled = FALSE;
    цео	    row = 0;
    цео	    overlap, col;

    validate_cheight();
    ако (curwin->w_cline_height == curwin->w_height
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_height)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	врати;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_width - width2);
    док (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_cols)
    {
	// scroll a screen line down
	ако (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	инако
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    ако (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	врати;  // don't scroll in the other direction now
    }

    col = curwin->w_virtcol + scrolloff_cols;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    ако (scrolloff_cols > 0)
    {
	цео size = linetabsize_eol(curwin, curwin->w_topline);
	size = width1 + width2 * ((size - width1 + width2 - 1) / width2);
	док (col > size)
	    col -= width2;
    }
    col -= curwin->w_skipcol;

    ако (col >= width1)
    {
	col -= width1;
	++row;
    }
    ако (col > width2)
    {
	row += col / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	col = col % width2;
    }
    ако (row >= curwin->w_height)
    {
	ако (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --row;
	}
	ако (row >= curwin->w_height)
	    curwin->w_skipcol += (row - curwin->w_height) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#ifdef FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    void
check_topfill(
    win_T	*wp,
    цео		down)	// when TRUE scroll down when not enough space
{
    цео		n;

    ако (wp->w_topfill <= 0)
	врати;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    ако (wp->w_topfill + n > wp->w_height)
    {
	ако (down && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	инако
	{
	    wp->w_topfill = wp->w_height - n;
	    ако (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#endif

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    void
scrolldown_clamp(void)
{
    цео		end_row;
#ifdef FEAT_DIFF
    цео		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#endif

    ако (curwin->w_topline <= 1
#ifdef FEAT_DIFF
	    && !can_fill
#endif
	    )
	врати;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    end_row = curwin->w_wrow;
#ifdef FEAT_DIFF
    ако (can_fill)
	++end_row;
    инако
	end_row += plines_nofill(curwin->w_topline - 1);
#инако
    end_row += plines(curwin->w_topline - 1);
#endif
    ако (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_cheight();
	validate_virtcol();
	end_row += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    ако (end_row < curwin->w_height - get_scrolloff_value())
    {
#ifdef FEAT_DIFF
	ако (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	инако
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#инако
	--curwin->w_topline;
#endif
#ifdef FEAT_FOLDING
	(void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
	--curwin->w_botline;	    // approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    void
scrollup_clamp(void)
{
    цео	    start_row;

    ако (curwin->w_topline == curbuf->b_ml.ml_line_count
#ifdef FEAT_DIFF
	    && curwin->w_topfill == 0
#endif
	    )
	врати;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#ifdef FEAT_DIFF
    start_row = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#инако
    start_row = curwin->w_wrow - plines(curwin->w_topline);
#endif
    ако (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	start_row -= curwin->w_virtcol / curwin->w_width;
    }
    ако (start_row >= get_scrolloff_value())
    {
#ifdef FEAT_DIFF
	ако (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	инако
#endif
	{
#ifdef FEAT_FOLDING
	    (void)hasFolding(curwin->w_topline, NULL, &curwin->w_topline);
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
    static void
topline_back_winheight(
    lineoff_T	*lp,
    цео		winheight)	// when TRUE limit to window height
{
#ifdef FEAT_DIFF
    ако (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    инако
#endif
    {
	--lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	ако (lp->lnum < 1)
	    lp->height = MAXCOL;
	инако
#ifdef FEAT_FOLDING
	    ако (hasFolding(lp->lnum, &lp->lnum, NULL))
	    // Add a closed fold
	    lp->height = 1;
	инако
#endif
	    lp->height = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    static void
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
    static void
botline_forw(lineoff_T *lp)
{
#ifdef FEAT_DIFF
    ако (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    инако
#endif
    {
	++lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	ако (lp->lnum > curbuf->b_ml.ml_line_count)
	    lp->height = MAXCOL;
	инако
#ifdef FEAT_FOLDING
	    ако (hasFolding(lp->lnum, NULL, &lp->lnum))
	    {
		// Add a closed fold.
		lp->height = 1;
	    }
	    инако
#endif
	    lp->height = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    void
scroll_cursor_top(цео min_scroll, цео always)
{
    цео		scrolled = 0;
    цео		extra = 0;
    цео		used;
    цео		i;
    linenr_T	top;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    цео		old_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#endif
    linenr_T	new_topline;
    цео		off = get_scrolloff_value();

    ако (mouse_dragging > 0)
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
    ако (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = used;

#ifdef FEAT_FOLDING
    ако (hasFolding(curwin->w_cursor.lnum, &top, &bot))
    {
	--top;
	++bot;
    }
    инако
#endif
    {
	top = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = top + 1;

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
    док (top > 0)
    {
#ifdef FEAT_FOLDING
	ако (hasFolding(top, &top, NULL))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	инако
#endif
	    i = PLINES_NOFILL(top);
	ако (top < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	ако ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    break;

	used += i;
	ако (extra + i <= off && bot < curbuf->b_ml.ml_line_count)
	{
#ifdef FEAT_FOLDING
	    ако (hasFolding(bot, NULL, &bot))
		// count one logical line for a sequence of folded lines
		++used;
	    инако
#endif
		used += plines(bot);
	}
	ако (used > curwin->w_height)
	    break;

	extra += i;
	new_topline = top;
	--top;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    ако (used > curwin->w_height)
	scroll_cursor_halfway(FALSE, FALSE);
    инако
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	ако (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	ако (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	ако (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    ако (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#endif
	ако (curwin->w_topline != old_topline)
	    reset_skipcol();
	инако ако (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    ако (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		цео	width1 = curwin->w_width - curwin_col_off();
		цео	width2 = width1 + curwin_col_off2();
		цео	plines_off = 0;
		цео	skipcol;

		ако (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		ако (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	ако (curwin->w_topline != old_topline
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
    void
set_empty_rows(win_T *wp, цео used)
{
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    ако (used == 0)
	wp->w_empty_rows = 0;	// single line that doesn't fit
    инако
    {
	wp->w_empty_rows = wp->w_height - used;
#ifdef FEAT_DIFF
	ако (wp->w_botline <= wp->w_buffer->b_ml.ml_line_count)
	{
	    wp->w_filler_rows = diff_check_fill(wp, wp->w_botline);
	    ако (wp->w_empty_rows > wp->w_filler_rows)
		wp->w_empty_rows -= wp->w_filler_rows;
	    инако
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
    void
scroll_cursor_bot(цео min_scroll, цео set_topbot)
{
    цео		used;
    цео		scrolled = 0;
    цео		extra = 0;
    цео		i;
    linenr_T	line_count;
    linenr_T	old_topline = curwin->w_topline;
    цео		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#ifdef FEAT_DIFF
    цео		old_topfill = curwin->w_topfill;
    цео		fill_below_window;
#endif
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    цео		old_empty_rows = curwin->w_empty_rows;
    linenr_T	cln;		    // Cursor Line Number
    long	so = get_scrolloff_value();
    цео		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    ако (set_topbot)
    {
	used = 0;
	linenr_T cln_last = cln;
#ifdef FEAT_FOLDING
	(void)hasFolding(cln, NULL, &cln_last);
#endif
	curwin->w_botline = cln_last + 1;
	loff.lnum = cln_last + 1;
#ifdef FEAT_DIFF
	loff.fill = 0;
#endif
	док (TRUE)
	{
	    topline_back_winheight(&loff, FALSE);
	    ако (loff.height == MAXCOL)
		break;
	    ако (used + loff.height > curwin->w_height)
	    {
		ако (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    ако (used < curwin->w_height)
		    {
			цео plines_offset = used + loff.height
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
		break;
	    }
#ifdef FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#endif
	    curwin->w_topline = loff.lnum;
	    used += loff.height;
	}

	set_empty_rows(curwin, used);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	ако (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#endif
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    ако (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    инако
		reset_skipcol();
	}
    }
    инако
	validate_botline();

    // The lines of the cursor line itself are always used.
#ifdef FEAT_DIFF
    used = plines_nofill(cln);
#инако
    validate_cheight();
    used = curwin->w_cline_height;
#endif

    ако (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	цео upto_cursor = plines_win_col(curwin, cln, curwin->w_cursor.col);

	ако (upto_cursor < used)
	    used = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    ако (cln >= curwin->w_botline)
    {
	scrolled = used;
	ако (cln == curwin->w_botline)
	    scrolled -= curwin->w_empty_rows;
	ако (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    цео top_plines =
#ifdef FEAT_DIFF
			    plines_win_nofill
#инако
			    plines_win
#endif
					(curwin, curwin->w_topline, FALSE);
	    цео width1 = curwin->w_width - curwin_col_off();

	    ако (width1 > 0)
	    {
		цео width2 = width1 + curwin_col_off2();
		цео skip_lines = 0;

		// A similar formula is used in curs_columns().
		ако (curwin->w_skipcol > width1)
		    skip_lines += (curwin->w_skipcol - width1) / width2 + 1;
		инако ако (curwin->w_skipcol > 0)
		    skip_lines = 1;

		top_plines -= skip_lines;
		ако (top_plines > curwin->w_height)
		{
		    scrolled += (top_plines - curwin->w_height);
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
    ако (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
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

    док (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	ако ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_line_count)
		&& loff.lnum <= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_below_window)
#endif
		)
	    break;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_back(&loff);
	ако (loff.height == MAXCOL)
	    used = MAXCOL;
	инако
	    used += loff.height;
	ако (used > curwin->w_height)
	    break;
	ако (loff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_below_window)
#endif
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.height;
	    ако (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_empty_rows;
	}

	ако (boff.lnum < curbuf->b_ml.ml_line_count)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    used += boff.height;
	    ако (used > curwin->w_height)
		break;
	    ако (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.height;
		ако (boff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_rows)
#endif
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.height;
		    ако (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_empty_rows;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    ако (scrolled <= 0)
	line_count = 0;
    // more than a screenfull, don't scroll but redraw
    инако ако (used > curwin->w_height)
	line_count = used;
    // scroll minimal number of lines
    инако
    {
	line_count = 0;
#ifdef FEAT_DIFF
	boff.fill = curwin->w_topfill;
#endif
	boff.lnum = curwin->w_topline - 1;
	за (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.height;
	    ++line_count;
	}
	ако (i < scrolled)	// below curwin->w_botline, don't scroll
	    line_count = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    ако (line_count >= curwin->w_height && line_count > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    инако ако (line_count > 0)
    {
	ако (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	инако
	    scrollup(line_count, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    ако (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && set_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_empty_rows = old_empty_rows;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    ако (set_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    void
scroll_cursor_halfway(цео atend, цео prefer_above)
{
    цео		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#ifdef FEAT_DIFF
    цео		topfill = 0;
#endif
    цео		below = 0;
    цео		used;
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
    (void)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#endif
#ifdef FEAT_DIFF
    used = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#инако
    used = plines(loff.lnum);
#endif
    topline = loff.lnum;

    цео want_height;
    цео do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    ако (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	ако (atend)
	{
	    want_height = (curwin->w_height - used) / 2;
	    used = 0;
	}
	инако
	    want_height = curwin->w_height;
    }

    док (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	ако (do_sms)
	{
	    topline_back_winheight(&loff, FALSE);
	    ако (loff.height == MAXCOL)
		break;
	    used += loff.height;
	    ако (!atend && boff.lnum < curbuf->b_ml.ml_line_count)
	    {
		botline_forw(&boff);
		used += boff.height;
	    }
	    ако (used > want_height)
	    {
		ако (used - loff.height < want_height)
		{
		    topline = loff.lnum;
#ifdef FEAT_DIFF
		    topfill = loff.fill;
#endif
		    skipcol = skipcol_from_plines(curwin, used - want_height);
		}
		break;
	    }
	    topline = loff.lnum;
#ifdef FEAT_DIFF
	    topfill = loff.fill;
#endif
	    continue;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	цео done = FALSE;
	за (цео round = 1; round <= 2; ++round)
	{
	    ако (prefer_above ? (round == 2 && below < above)
			     : (round == 1 && below <= above))
	    {
		// add a line below the cursor
		ако (boff.lnum < curbuf->b_ml.ml_line_count)
		{
		    botline_forw(&boff);
		    used += boff.height;
		    ако (used > curwin->w_height)
		    {
			done = TRUE;
			break;
		    }
		    below += boff.height;
		}
		инако
		{
		    ++below;	    // count a "~" line
		    ако (atend)
			++used;
		}
	    }

	    ако (prefer_above ? (round == 1 && below >= above)
			     : (round == 1 && below > above))
	    {
		// add a line above the cursor
		topline_back(&loff);
		ако (loff.height == MAXCOL)
		    used = MAXCOL;
		инако
		    used += loff.height;
		ако (used > curwin->w_height)
		{
		    done = TRUE;
		    break;
		}
		above += loff.height;
		topline = loff.lnum;
#ifdef FEAT_DIFF
		topfill = loff.fill;
#endif
	    }
	}
	ако (done)
	    break;
    }

#ifdef FEAT_FOLDING
    ако (!hasFolding(topline, &curwin->w_topline, NULL))
#endif
    {
	ако (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    ако (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    инако ако (do_sms)
		reset_skipcol();
	}
    }
#ifdef FEAT_DIFF
    curwin->w_topfill = topfill;
    ако (old_topline > curwin->w_topline + curwin->w_height)
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
    void
cursor_correct(void)
{
    цео		above = 0;	    // screen lines above topline
    linenr_T	topline;
    цео		below = 0;	    // screen lines below botline
    linenr_T	botline;
    цео		above_wanted, below_wanted;
    linenr_T	cln;		    // Cursor Line Number
    цео		max_off;
    long	so = get_scrolloff_value();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    below_wanted = so;
    ако (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	below_wanted = mouse_dragging - 1;
    }
    ако (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_height / 2;
	ако (below_wanted > max_off)
	    below_wanted = max_off;
    }
    validate_botline();
    ако (curwin->w_botline == curbuf->b_ml.ml_line_count + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	below_wanted = 0;
	max_off = (curwin->w_height - 1) / 2;
	ако (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    ако (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - below_wanted
#ifdef FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#endif
	    )
	врати;

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
    док ((above < above_wanted || below < below_wanted) && topline < botline)
    {
	ако (below < below_wanted && (below <= above || above >= above_wanted))
	{
#ifdef FEAT_FOLDING
	    ако (hasFolding(botline, &botline, NULL))
		++below;
	    инако
#endif
		below += plines(botline);
	    --botline;
	}
	ако (above < above_wanted && (above < below || below >= below_wanted))
	{
#ifdef FEAT_FOLDING
	    ако (hasFolding(topline, NULL, &topline))
		++above;
	    инако
#endif
		above += PLINES_NOFILL(topline);
#ifdef FEAT_DIFF
	    // Count filler lines below this line as context.
	    ако (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#endif
	    ++topline;
	}
    }
    ако (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    инако ако (topline > botline)
	curwin->w_cursor.lnum = botline;
    инако
    {
	ако (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	ако (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_line_count)
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
static цео get_scroll_overlap(цео dir)
{
    lineoff_T loff;
    цео	    min_height = curwin->w_height - 2;

    validate_botline();
    ако ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_line_count))
	врати min_height + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#ifdef FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_rows : curwin->w_topfill);
    loff.height = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#инако
    loff.height = plines(loff.lnum);
#endif

    цео h1 = loff.height;
    ако (h1 > min_height)
	врати min_height + 2;  // no overlap
    ако (dir == FORWARD)
	topline_back(&loff);
    инако
	botline_forw(&loff);

    цео h2 = loff.height;
    ако (h2 == MAXCOL || h2 + h1 > min_height)
	врати min_height + 2;  // no overlap
    ако (dir == FORWARD)
	topline_back(&loff);
    инако
	botline_forw(&loff);

    цео h3 = loff.height;
    ако (h3 == MAXCOL || h3 + h2 > min_height)
	врати min_height + 2;  // no overlap
    ако (dir == FORWARD)
	topline_back(&loff);
    инако
	botline_forw(&loff);

    цео h4 = loff.height;
    ако (h4 == MAXCOL || h4 + h3 + h2 > min_height || h3 + h2 + h1 > min_height)
	врати min_height + 1;  // 1 line overlap
    инако
	врати min_height;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
static цео scroll_with_sms(цео dir, long count, long *curscount)
{
    цео		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    цео		prev_topfill = curwin->w_topfill;
#endif

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, count);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    ако (!prev_sms && curwin->w_skipcol > 0)
    {
	цео fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	ако (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	цео width1 = curwin->w_width - curwin_col_off();
	цео width2 = width1 + curwin_col_off2();
	count = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	ако (fixdir == FORWARD)
	    count = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, count);
	*curscount += count * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    врати curwin->w_topline != prev_topline
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
    цео
pagescroll(цео dir, long count, цео half)
{
    цео		did_move = FALSE;
    цео		buflen = curbuf->b_ml.ml_line_count;
    colnr_T	prev_col = curwin->w_cursor.col;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    ако (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	ако (count)
	    curwin->w_p_scr = MIN(curwin->w_height, count);
	count = MIN(curwin->w_height, curwin->w_p_scr);

	long curscount = count;
	// Adjust count so as to not reveal end of buffer lines.
	ако (dir == FORWARD
		    && (curwin->w_topline + curwin->w_height + count > buflen
#ifdef FEAT_FOLDING
							|| hasAnyFolding(curwin)
#endif
	   ))
	{
	    цео n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    ако (n - count < curwin->w_height && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_height + count);
	    ако (n < curwin->w_height + count)
		count = n - curwin->w_height;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	ако (count > 0)
	{
	    did_move = scroll_with_sms(dir, count, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.col = prev_col;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	ако (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	инако ако (dir == FORWARD)
	    cursor_down_inner(curwin, curscount);
	инако
	    cursor_up_inner(curwin, curscount);
    }
    инако
    {
	// Scroll [count] times 'window' or current window height lines.
	count *= ((ONE_WINDOW && p_window > 0 && p_window < Rows - 1) ?
				MAX(1, p_window - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, count, &count);

	ако (did_move)
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

    ако (get_scrolloff_value() > 0)
	cursor_correct();
#ifdef FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#endif
    did_move = did_move
	|| prev_col != curwin->w_cursor.col
	|| prev_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    ако (!did_move)
	beep_flush();
    инако ако (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    инако ако (p_sol)
	nv_g_home_m_cmd(&ca);

    врати did_move ? OK : FAIL;
}

    void
do_check_cursorbind(void)
{
    static win_T	*prev_curwin = NULL;
    static buf_T	*prev_curbuf = NULL;
    static varnumber_T	prev_changedtick = 0;
    static pos_T	prev_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    ако (curwin == prev_curwin && curbuf == prev_curbuf
	    && CHANGEDTICK(curbuf) == prev_changedtick
	    && EQUAL_POS(curwin->w_cursor, prev_cursor))
	врати;
    prev_curwin = curwin;
    prev_curbuf = curbuf;
    prev_changedtick = CHANGEDTICK(curbuf);
    prev_cursor = curwin->w_cursor;

    linenr_T	line = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    цео		set_curswant = curwin->w_set_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    цео		old_VIsual_select = VIsual_select;
    цео		old_VIsual_active = VIsual_active;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_active = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_buffer;
	// skip original window and windows with 'nocursorbind'
	ако (curwin != old_curwin && curwin->w_p_crb)
	{
#ifdef FEAT_DIFF
	    ако (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_line(old_curbuf, line);
	    инако
#endif
		curwin->w_cursor.lnum = line;
	    curwin->w_cursor.col = col;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_set_curswant = set_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    цео restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    ако (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    ако (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    ако (!curwin->w_p_scb)
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
