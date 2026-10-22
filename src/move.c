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

#अन्तर्भाव "vim.h"

स्थिर शून्य redraw_for_cursorline(win_T *wp);
स्थिर पूर्णाङ्क scrolljump_मानम्(शून्य);
स्थिर पूर्णाङ्क check_शिखरम्_offset(शून्य);
स्थिर शून्य curs_rows(win_T *wp);

प्रकारनिर्देश संरचना
{
    linenr_T	    lnum;	// line number
#ifdef FEAT_DIFF
    पूर्णाङ्क		    fill;	// filler lines
#endif
    पूर्णाङ्क		    height;	// height of added line
} lineoff_T;

स्थिर शून्य topline_back(lineoff_T *lp);
स्थिर शून्य botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    स्थिर पूर्णाङ्क
adjust_plines_for_skipcol(win_T *wp)
{
    जइ (wp->w_skipcol == 0)
	प्रत्यागम 0;

    पूर्णाङ्क width = wp->w_width - win_col_off(wp);
    पूर्णाङ्क w2 = width + win_col_off2(wp);
    जइ (wp->w_skipcol >= width && w2 > 0)
	प्रत्यागम (wp->w_skipcol - width) / w2 + 1;

    प्रत्यागम 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    पूर्णाङ्क
plines_correct_topline(win_T *wp, linenr_T lnum, पूर्णाङ्क limit_winheight)
{
    पूर्णाङ्क n;
#ifdef FEAT_DIFF
    जइ (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    अण्णहा
#endif
	n = plines_win(wp, lnum, FALSE);
    जइ (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    जइ (limit_winheight && n > wp->w_height)
	n = wp->w_height;
    प्रत्यागम n;
}

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    स्थिर शून्य
comp_botline(win_T *wp)
{
    पूर्णाङ्क		n;
    linenr_T	lnum;
    पूर्णाङ्क		done;
    पूर्णाङ्क		i = 0;
    पूर्णाङ्क		use_cache;
#ifdef FEAT_FOLDING
    linenr_T    last;
    पूर्णाङ्क		folded;
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
		    && wp->w_पङ्क्तयः_valid > 0
		    && wp->w_पङ्क्तयः[0].wl_lnum <= wp->w_topline;

    जइ (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	done = wp->w_cline_row;
    }
    अण्णहा
    {
	lnum = wp->w_topline;
	done = 0;
    }

    // Find the w_lines[] entry for the starting line.
    जइ (use_cache)
	जाव (i < wp->w_पङ्क्तयः_valid && wp->w_पङ्क्तयः[i].wl_lnum < lnum)
	    ++i;

    प्रति ( ; lnum <= wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्; ++i)
    {
	पूर्णाङ्क	valid = FALSE;

#ifdef FEAT_FOLDING
	last = lnum;
	folded = FALSE;
#endif
	// Try to use the size from the previous redraw.
	जइ (use_cache && i < wp->w_पङ्क्तयः_valid)
	{
	    जइ (wp->w_पङ्क्तयः[i].wl_lnum < lnum || !wp->w_पङ्क्तयः[i].wl_valid)
		अनुवर्त;		// skip changed or deleted lines
	    जइ (wp->w_पङ्क्तयः[i].wl_lnum == lnum)
		valid = TRUE;
	    अण्णहा // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	जइ (valid)
	{
	    n = wp->w_पङ्क्तयः[i].wl_size;
#ifdef FEAT_FOLDING
	    folded = wp->w_पङ्क्तयः[i].wl_folded;
	    last = wp->w_पङ्क्तयः[i].wl_lastlnum;
#endif
	}
	अण्णहा
	{
#ifdef FEAT_FOLDING
	    जइ (hasFoldingWin(wp, lnum, रिक्त, &last, TRUE, रिक्त))
	    {
		n = 1;
		folded = TRUE;
	    }
	    अण्णहा
#endif
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	जइ (
#ifdef FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && last >= wp->w_cursor.lnum
#अण्णहा
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
	जइ (done + n > wp->w_height)
	    भङ्ग;
	done += n;
#ifdef FEAT_FOLDING
	lnum = last + 1;
#अण्णहा
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
    स्थिर शून्य
redraw_for_cursorline(win_T *wp)
{
    जइ ((wp->w_p_rnu
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
    स्थिर शून्य
redraw_for_cursorcolumn(win_T *wp)
{
    जइ ((wp->w_valid & VALID_VIRTCOL) == 0 && !pum_visible())
    {
	// When 'cursorcolumn' is set need to redraw with UPD_SOME_VALID.
	जइ (wp->w_p_cuc)
	    redraw_win_later(wp, UPD_SOME_VALID);
	// When 'cursorlineopt' contains "screenline" need to redraw with
	// UPD_VALID.
	अण्णहा जइ (wp->w_p_cul && (wp->w_p_culopt_flags & CULOPT_SCRLINE))
	    redraw_win_later(wp, UPD_VALID);
    }
}
#endif

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    शून्य
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
    पूर्णाङ्क
sms_marker_overlap(win_T *wp, पूर्णाङ्क extra2)
{
    जइ (extra2 == -1)
	extra2 = win_col_off(wp) - win_col_off2(wp);
#जइ defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    जइ (*get_showbreak_मानम्(wp) != NUL)
	प्रत्यागम 0;
#endif
    // Overlap when 'list' and 'listchars' "precedes" are set is 1.
    जइ (wp->w_p_list && wp->w_lcs_chars.prec)
	प्रत्यागम 1;

    प्रत्यागम extra2 > 3 ? 0 : 3 - extra2;
}

/*
 * Calculates the skipcol offset for window "wp" given how many
 * physical lines we want to scroll down.
 */
    स्थिर पूर्णाङ्क
skipcol_from_plines(win_T *wp, पूर्णाङ्क plines_off)
{
    पूर्णाङ्क width1 = wp->w_width - win_col_off(wp);

    पूर्णाङ्क skipcol = 0;
    जइ (plines_off > 0)
	skipcol += width1;
    जइ (plines_off > 1)
	skipcol += (width1 + win_col_off2(wp)) * (plines_off - 1);
    प्रत्यागम skipcol;
}

/*
 * Set curwin->w_skipcol to zero and redraw later if needed.
 */
    स्थिर शून्य
reset_skipcol(शून्य)
{
    जइ (curwin->w_skipcol == 0)
	प्रत्यागम;

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
    शून्य
update_topline_redraw(शून्य)
{
    update_topline();
    जइ (must_redraw)
	update_screen(0);
}

/*
 * Return true when 'scrolloffpad' may augment 'scrolloff'.
 * This only applies to automatic cursor visibility correction.
 * For now 'scrolloffpad' is treated as boolean: 0 disables, > 0 enables.
 */
    स्थिर bool
use_scrolloffpad(शून्य)
{
    प्रत्यागम get_scrolloff_मानम्() > 0 && get_scrolloffpad_मानम्() > 0;
}

/*
 * Return TRUE when there are not enough real buffer lines below "lnum" to
 * satisfy the requested "so" context.
 */
    स्थिर bool
scrolloffpad_eof_pressure(linenr_T lnum, long so)
{
    जइ (!use_scrolloffpad() || so <= 0)
	प्रत्यागम false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    प्रत्यागम lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम् - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    शून्य
update_topline(शून्य)
{
    long	पङ्क्तिः_गणनम्;
    पूर्णाङ्क		halfheight;
    पूर्णाङ्क		n;
#ifdef FEAT_FOLDING
    linenr_T	lnum;
#endif
    पूर्णाङ्क		check_topline = FALSE;
    पूर्णाङ्क		check_botline = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    पूर्णाङ्क		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    जइ (skip_update_topline)
	प्रत्यागम;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    जइ (!screen_valid(TRUE) || curwin->w_height == 0)
    {
	check_cursor_lnum();
	curwin->w_topline = curwin->w_cursor.lnum;
	curwin->w_botline = curwin->w_topline;
	curwin->w_scbind_pos = 1;
	प्रत्यागम;
    }

    check_cursor_moved(curwin);
    जइ (curwin->w_valid & VALID_TOPLINE)
	प्रत्यागम;

    // When dragging with the mouse, don't scroll that quickly
    जइ (mouse_dragging > 0)
	*so_ptr = mouse_dragging - 1;
    eof_pressure = scrolloffpad_eof_pressure(curwin->w_cursor.lnum, *so_ptr);

    linenr_T old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    पूर्णाङ्क old_topfill = curwin->w_topfill;
#endif

    /*
     * If the buffer is empty, always set topline to 1.
     */
    जइ (BUFEMPTY())		// special case - file is empty
    {
	जइ (curwin->w_topline != 1)
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
    अण्णहा
    {
	जइ (curwin->w_topline > 1 || curwin->w_skipcol > 0)
	{
	    // If the cursor is above topline, scrolling is always needed.
	    // If the cursor is far below topline and there is no folding,
	    // scrolling down is never needed.
	    जइ (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    अण्णहा जइ (check_शिखरम्_offset())
		check_topline = TRUE;
	    अण्णहा जइ (curwin->w_skipcol > 0
				 && curwin->w_cursor.lnum == curwin->w_topline)
	    {
		colnr_T vcol;
		पूर्णाङ्क overlap;

		// Check that the cursor position is visible.  Add columns for
		// the marker displayed in the top-left if needed.
		getvvcol(curwin, &curwin->w_cursor, &vcol, रिक्त, रिक्त, 0);
		overlap = sms_marker_overlap(curwin, -1);
		जइ (curwin->w_skipcol + overlap > vcol)
		    check_topline = TRUE;
	    }
	}
#ifdef FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	जइ (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#endif

	जइ (check_topline)
	{
	    halfheight = curwin->w_height / 2 - 1;
	    जइ (halfheight < 2)
		halfheight = 2;

#ifdef FEAT_FOLDING
	    जइ (hasAnyFolding(curwin))
	    {
		// Count the number of logical lines between the cursor and
		// topline + scrolloff (approximation of how much will be
		// scrolled).
		n = 0;
		प्रति (lnum = curwin->w_cursor.lnum;
				    lnum < curwin->w_topline + *so_ptr; ++lnum)
		{
		    ++n;
		    // stop at end of file or when we know we are far off
		    जइ (lnum >= curbuf->b_ml.ml_पङ्क्तिः_गणनम् || n >= halfheight)
			भङ्ग;
		    (शून्य)hasFolding(lnum, रिक्त, &lnum);
		}
	    }
	    अण्णहा
#endif
		n = curwin->w_topline + *so_ptr - curwin->w_cursor.lnum;

	    // If we weren't very close to begin with, we scroll to put the
	    // cursor in the middle of the window.  Otherwise put the cursor
	    // near the top of the window.
	    पूर्णाङ्क min_scroll = scrolljump_मानम्();
	    जइ (eof_pressure)
		scroll_cursor_halfway(TRUE, TRUE);
	    अण्णहा जइ (n >= halfheight && min_scroll < halfheight)
		scroll_cursor_halfway(FALSE, FALSE);
	    अण्णहा
	    {
		scroll_cursor_शिखरम्(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	अण्णहा
	{
#ifdef FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (शून्य)hasFolding(curwin->w_topline, &curwin->w_topline, रिक्त);
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
    जइ (check_botline)
    {
	जइ (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	जइ (curwin->w_botline <= curbuf->b_ml.ml_पङ्क्तिः_गणनम् || use_scrolloffpad())
	{
	    जइ (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		जइ (((long)curwin->w_cursor.lnum
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
		    (शून्य)hasFolding(loff.lnum, रिक्त, &loff.lnum);
#endif
#ifdef FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_rows;
#endif
		    loff.height = 0;
		    जाव (loff.lnum < curwin->w_botline
#ifdef FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#endif
			    )
		    {
			n += loff.height;
			जइ (n >= *so_ptr)
			    भङ्ग;
			botline_forw(&loff);
		    }
		    जइ (n >= *so_ptr && !eof_pressure)
			// sufficient context, no need to scroll
			check_botline = FALSE;
		}
		अण्णहा
		    // sufficient context, no need to scroll
		    check_botline = FALSE;
	    }
	    जइ (check_botline)
	    {
#ifdef FEAT_FOLDING
		जइ (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    पङ्क्तिः_गणनम् = 0;
		    प्रति (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++पङ्क्तिः_गणनम्;
			// stop at end of file or when we know we are far off
			जइ (lnum <= 0 || पङ्क्तिः_गणनम् > curwin->w_height + 1)
			    भङ्ग;
			(शून्य)hasFolding(lnum, &lnum, रिक्त);
		    }
		}
		अण्णहा
#endif
		    पङ्क्तिः_गणनम् = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		जइ (पङ्क्तिः_गणनम् <= curwin->w_height + 1)
		{
		    जइ (eof_pressure)
			scroll_cursor_halfway(TRUE, TRUE);
		    अण्णहा
			scroll_cursor_bot(scrolljump_मानम्(), FALSE);
		}
		अण्णहा
		    scroll_cursor_halfway(eof_pressure, eof_pressure);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    जइ (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#endif
	    )
    {
	dollar_vcol = -1;
	redraw_later(UPD_VALID);

	// When 'smoothscroll' is not set, should reset w_skipcol.
	जइ (!curwin->w_p_sms)
	    reset_skipcol();
	अण्णहा जइ (curwin->w_skipcol != 0)
	    redraw_later(UPD_SOME_VALID);

	// May need to set w_skipcol when cursor in w_topline.
	जइ (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

    *so_ptr = save_so;
}

/*
 * Return the scrolljump value to use for the current window.
 * When 'scrolljump' is positive use it as-is.
 * When 'scrolljump' is negative use it as a percentage of the window height.
 */
    स्थिर पूर्णाङ्क
scrolljump_मानम्(शून्य)
{
    जइ (p_sj >= 0)
	प्रत्यागम (पूर्णाङ्क)p_sj;
    प्रत्यागम (curwin->w_height * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    स्थिर पूर्णाङ्क
check_शिखरम्_offset(शून्य)
{
    lineoff_T	loff;
    पूर्णाङ्क		n;
    long	so = get_scrolloff_मानम्();

    जइ (curwin->w_cursor.lnum < curwin->w_topline + so
#ifdef FEAT_FOLDING
		    || hasAnyFolding(curwin)
#endif
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#अण्णहा
	n = 0;
#endif
	// Count the visible screen lines above the cursor line.
	जाव (n < so)
	{
	    topline_back(&loff);
	    // Stop when included a line above the window.
	    जइ (loff.lnum < curwin->w_topline
#ifdef FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#endif
		    )
		भङ्ग;
	    n += loff.height;
	}
	जइ (n < so)
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Update w_curswant.
 */
    शून्य
update_curswant_force(शून्य)
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
    शून्य
update_curswant(शून्य)
{
    जइ (curwin->w_set_curswant)
	update_curswant_force();
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    शून्य
check_cursor_moved(win_T *wp)
{
    जइ (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    अण्णहा जइ (wp->w_skipcol != wp->w_valid_skipcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				      |VALID_CHEIGHT|VALID_CROW
				      |VALID_BOTLINE|VALID_BOTLINE_AP);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
	wp->w_valid_skipcol = wp->w_skipcol;
    }
    अण्णहा जइ (wp->w_cursor.col != wp->w_valid_cursor.col
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
    शून्य
changed_window_setting(शून्य)
{
    changed_window_setting_win(curwin);
}

    शून्य
changed_window_setting_win(win_T *wp)
{
    wp->w_पङ्क्तयः_valid = 0;
    changed_पङ्क्तिः_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#जइ defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    शून्य
changed_window_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	जइ (wp->w_buffer == buf)
	    changed_window_setting_win(wp);
}
#endif

/*
 * Call changed_window_setting_win() for every window.
 */
    शून्य
changed_window_setting_all(शून्य)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	changed_window_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    शून्य
set_topline(win_T *wp, linenr_T lnum)
{
#ifdef FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#endif

#ifdef FEAT_FOLDING
    // go to first of folded lines
    (शून्य)hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);
#endif
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    जइ (wp->w_botline > wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम् + 1)
	wp->w_botline = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम् + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_set = true;
#ifdef FEAT_DIFF
    जइ (lnum != prev_topline)
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
    शून्य
changed_cline_bef_curs(शून्य)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    शून्य
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
    शून्य
changed_पङ्क्तिः_abv_curs(शून्य)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    शून्य
changed_पङ्क्तिः_abv_curs_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#जइ defined(FEAT_PROP_POPUP)
/*
 * Display of line has changed for "buf", invalidate cursor position and
 * w_botline.
 */
    शून्य
changed_पङ्क्तिः_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	जइ (wp->w_buffer == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#endif

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    शून्य
validate_botline(शून्य)
{
    validate_botline_win(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    शून्य
validate_botline_win(win_T *wp)
{
    जइ (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    शून्य
invalidate_botline(शून्य)
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    शून्य
invalidate_botline_win(win_T *wp)
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    शून्य
approximate_botline_win(
    win_T	*wp)
{
    wp->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    पूर्णाङ्क
cursor_valid(शून्य)
{
    check_cursor_moved(curwin);
    प्रत्यागम ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    शून्य
validate_cursor(शून्य)
{
    check_cursor_lnum();
    check_cursor_moved(curwin);
    जइ ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#जइ defined(FEAT_GUI)
/*
 * validate w_cline_row.
 */
    शून्य
validate_cline_row(शून्य)
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    जइ (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);
}
#endif

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    स्थिर शून्य
curs_rows(win_T *wp)
{
    linenr_T	lnum;
    पूर्णाङ्क		i;
    पूर्णाङ्क		all_invalid;
    पूर्णाङ्क		valid;
#ifdef FEAT_FOLDING
    long	fold_गणनम्;
#endif

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_पङ्क्तयः_valid == 0
			|| wp->w_पङ्क्तयः[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_row = 0;
    प्रति (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	जइ (!all_invalid && i < wp->w_पङ्क्तयः_valid)
	{
	    जइ (wp->w_पङ्क्तयः[i].wl_lnum < lnum || !wp->w_पङ्क्तयः[i].wl_valid)
		अनुवर्त;		// skip changed or deleted lines
	    जइ (wp->w_पङ्क्तयः[i].wl_lnum == lnum)
	    {
#ifdef FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		जइ (!wp->w_buffer->b_mod_set
			|| wp->w_पङ्क्तयः[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_buffer->b_mod_शिखरम्
					     > wp->w_पङ्क्तयः[i].wl_lastlnum + 1)
#endif
		valid = TRUE;
	    }
	    अण्णहा जइ (wp->w_पङ्क्तयः[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	जइ (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#ifdef FEAT_DIFF
					    && !wp->w_p_diff
#endif
	       )))
	{
#ifdef FEAT_FOLDING
	    lnum = wp->w_पङ्क्तयः[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    जइ (lnum > wp->w_cursor.lnum)
		भङ्ग;
#अण्णहा
	    ++lnum;
#endif
	    wp->w_cline_row += wp->w_पङ्क्तयः[i].wl_size;
	}
	अण्णहा
	{
#ifdef FEAT_FOLDING
	    fold_गणनम् = folded_गणनम्(wp, lnum, रिक्त);
	    जइ (fold_गणनम्)
	    {
		lnum += fold_गणनम्;
		जइ (lnum > wp->w_cursor.lnum)
		    भङ्ग;
		++wp->w_cline_row;
	    }
	    अण्णहा
#endif
	    {
		wp->w_cline_row += plines_correct_topline(wp, lnum, TRUE);
		++lnum;
	    }
	}
    }

    check_cursor_moved(wp);
    जइ (!(wp->w_valid & VALID_CHEIGHT))
    {
	जइ (all_invalid
		|| i == wp->w_पङ्क्तयः_valid
		|| (i < wp->w_पङ्क्तयः_valid
		    && (!wp->w_पङ्क्तयः[i].wl_valid
			|| wp->w_पङ्क्तयः[i].wl_lnum != wp->w_cursor.lnum)))
	{
#ifdef FEAT_DIFF
	    जइ (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_height = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    अण्णहा
#endif
		wp->w_cline_height = plines_win(wp, wp->w_cursor.lnum, TRUE);
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      रिक्त, रिक्त, TRUE, रिक्त);
#endif
	}
	अण्णहा जइ (i > wp->w_पङ्क्तयः_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_height = 0;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      रिक्त, रिक्त, TRUE, रिक्त);
#endif
	}
	अण्णहा
	{
	    wp->w_cline_height = wp->w_पङ्क्तयः[i].wl_size;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = wp->w_पङ्क्तयः[i].wl_folded;
#endif
	}
    }

    redraw_for_cursorline(curwin);
    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;
}

/*
 * Validate curwin->w_virtcol only.
 */
    शून्य
validate_virtcol(शून्य)
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    शून्य
validate_virtcol_win(win_T *wp)
{
    check_cursor_moved(wp);

    जइ (wp->w_valid & VALID_VIRTCOL)
	प्रत्यागम;

#ifdef FEAT_PROP_POPUP
    wp->w_virtcol_first_char = 0;
#endif
    getvvcol(wp, &wp->w_cursor, रिक्त, &(wp->w_virtcol), रिक्त,
							 GETVCOL_FOR_VIRTCOL);
#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#endif
    wp->w_valid |= VALID_VIRTCOL;
}

/*
 * Validate curwin->w_cline_height only.
 */
    शून्य
validate_cheight(शून्य)
{
    check_cursor_moved(curwin);

    जइ (curwin->w_valid & VALID_CHEIGHT)
	प्रत्यागम;

#ifdef FEAT_DIFF
    जइ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_height = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    अण्णहा
#endif
	curwin->w_cline_height = plines(curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, रिक्त, रिक्त);
#endif
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    शून्य
validate_cursor_col(शून्य)
{
    colnr_T off;
    colnr_T col;
    पूर्णाङ्क     width;

    validate_virtcol();

    जइ (curwin->w_valid & VALID_WCOL)
	प्रत्यागम;

    col = curwin->w_virtcol;
    off = curwin_col_off();
    col += off;
    width = curwin->w_width - off + curwin_col_off2();

    // long line wrapping, adjust curwin->w_wrow
    जइ (curwin->w_p_wrap
	    && col >= (colnr_T)curwin->w_width
	    && width > 0)
	// use same formula as what is used in curs_columns()
	col -= ((col - curwin->w_width) / width + 1) * width;
    जइ (col > (पूर्णाङ्क)curwin->w_leftcol)
	col -= curwin->w_leftcol;
    अण्णहा
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
    पूर्णाङ्क
win_col_off(win_T *wp)
{
    प्रत्यागम (((wp->w_p_nu || wp->w_p_rnu) ? सङ्ख्या_width(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#ifdef FEAT_FOLDING
	    + wp->w_p_fdc
#endif
#ifdef FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#endif
	   );
}

    पूर्णाङ्क
curwin_col_off(शून्य)
{
    प्रत्यागम win_col_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    पूर्णाङ्क
win_col_off2(win_T *wp)
{
    जइ ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != रिक्त)
	प्रत्यागम सङ्ख्या_width(wp) + 1;
    प्रत्यागम 0;
}

    पूर्णाङ्क
curwin_col_off2(शून्य)
{
    प्रत्यागम win_col_off2(curwin);
}

/*
 * Compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    शून्य
curs_columns(
    पूर्णाङ्क		may_scroll)	// when TRUE, may scroll horizontally
{
    long	diff;
    पूर्णाङ्क		extra;		// offset for first screen line
    long	off_वामः, off_दक्षिणः;
    पूर्णाङ्क		n;
    पूर्णाङ्क		p_पङ्क्तयः;
    पूर्णाङ्क		width1;		// text width for first screen line
    पूर्णाङ्क		width2 = 0;	// text width for second and later screen line
    पूर्णाङ्क		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    long	so = get_scrolloff_मानम्();
    long	siso = get_sidescrolloff_मानम्();
    पूर्णाङ्क		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    जइ (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin);

#ifdef FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_first_char = 0;
#endif

    /*
     * Compute the number of virtual columns.
     */
#ifdef FEAT_FOLDING
    जइ (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    अण्णहा
#endif
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    जइ (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_col_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_row;

    width1 = curwin->w_width - extra;
    जइ (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_width - 1;
	जइ (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_height - 1;
	अण्णहा
	    curwin->w_wrow = curwin->w_height - 1 - curwin->w_empty_rows;
    }
    अण्णहा जइ (curwin->w_p_wrap && curwin->w_width != 0)
    {
	width2 = width1 + curwin_col_off2();

	// skip columns that are not visible
	जइ (curwin->w_cursor.lnum == curwin->w_topline
		&& curwin->w_skipcol > 0
		&& curwin->w_wcol >= curwin->w_skipcol)
	{
	    // Deduct by multiples of width2.  This allows the long line
	    // wrapping formula below to correctly calculate the w_wcol value
	    // when wrapping.
	    जइ (curwin->w_skipcol <= width1)
		curwin->w_wcol -= width2;
	    अण्णहा
		curwin->w_wcol -= width2
			       * (((curwin->w_skipcol - width1) / width2) + 1);

	    did_sub_skipcol = TRUE;
	}

	// long line wrapping, adjust curwin->w_wrow
	जइ (curwin->w_wcol >= curwin->w_width)
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
    अण्णहा जइ (may_scroll
#ifdef FEAT_FOLDING
	    && !curwin->w_cline_folded
#endif
	    )
    {
#ifdef FEAT_PROP_POPUP
	जइ (curwin->w_virtcol_first_char > 0)
	{
	    पूर्णाङ्क cols = (curwin->w_width - extra);
	    पूर्णाङ्क rows = cols > 0 ? curwin->w_virtcol_first_char / cols : 1;

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
	off_वामः = startcol - curwin->w_leftcol - siso;
	off_दक्षिणः = endcol - curwin->w_leftcol - (curwin->w_width - siso) + 1;
	जइ (off_वामः < 0 || off_दक्षिणः > 0)
	{
	    जइ (off_वामः < 0)
		diff = -off_वामः;
	    अण्णहा
		diff = off_दक्षिणः;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    जइ (p_ss == 0 || diff >= width1 / 2 || off_दक्षिणः >= off_वामः)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    अण्णहा
	    {
		जइ (diff < p_ss)
		    diff = p_ss;
		जइ (off_वामः < 0)
		    new_leftcol = curwin->w_leftcol - (पूर्णाङ्क)diff;
		अण्णहा
		    new_leftcol = curwin->w_leftcol + (पूर्णाङ्क)diff;
	    }
	    जइ (new_leftcol < 0)
		new_leftcol = 0;
	    जइ (new_leftcol != (पूर्णाङ्क)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		// screen has to be redrawn with new curwin->w_leftcol
		redraw_later(UPD_NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    अण्णहा जइ (curwin->w_wcol > (पूर्णाङ्क)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    अण्णहा
	curwin->w_wcol = 0;

#ifdef FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    जइ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    अण्णहा
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#endif

    prev_skipcol = curwin->w_skipcol;

    p_पङ्क्तयः = 0;

    जइ ((curwin->w_wrow >= curwin->w_height
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_height)
		    && (p_पङ्क्तयः =
#ifdef FEAT_DIFF
			plines_win_nofill
#अण्णहा
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
	जइ (curwin->w_skipcol + so * width2 > curwin->w_virtcol)
	    extra = 1;
	// Compute last display line of the buffer line that we want at the
	// bottom of the window.
	जइ (p_पङ्क्तयः == 0)
	    p_पङ्क्तयः = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_पङ्क्तयः;
	जइ (p_पङ्क्तयः > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	अण्णहा
	    n = p_पङ्क्तयः;
	जइ ((colnr_T)n >= curwin->w_height + curwin->w_skipcol / width2 - so)
	    extra += 2;

	जइ (extra == 3 || curwin->w_height <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    जइ (n > curwin->w_height / 2)
		n -= curwin->w_height / 2;
	    अण्णहा
		n = 0;
	    // don't skip more than necessary
	    जइ (n > p_पङ्क्तयः - curwin->w_height + 1)
		n = p_पङ्क्तयः - curwin->w_height + 1;
	    जइ (n > 0)
		curwin->w_skipcol = width1 + (n - 1) * width2;
	    अण्णहा
		curwin->w_skipcol = 0;
	}
	अण्णहा जइ (extra == 1)
	{
	    // less than 'scrolloff' lines above, decrease skipcol
	    extra = (curwin->w_skipcol + so * width2 - curwin->w_virtcol
				     + width2 - 1) / width2;
	    जइ (extra > 0)
	    {
		जइ ((colnr_T)(extra * width2) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width2;
		curwin->w_skipcol -= extra * width2;
	    }
	}
	अण्णहा जइ (extra == 2)
	{
	    // less than 'scrolloff' lines below, increase skipcol
	    endcol = (n - curwin->w_height + 1) * width2;
	    जाव (endcol > curwin->w_virtcol)
		endcol -= width2;
	    जइ (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	// adjust w_wrow for the changed w_skipcol
	जइ (did_sub_skipcol)
	    curwin->w_wrow -= (curwin->w_skipcol - prev_skipcol) / width2;
	अण्णहा
	    curwin->w_wrow -= curwin->w_skipcol / width2;

	जइ (curwin->w_wrow >= curwin->w_height)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_height + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((पूर्णाङ्क)prev_skipcol - (पूर्णाङ्क)curwin->w_skipcol) / width2;
	जइ (extra > 0)
	    win_ins_पङ्क्तयः(curwin, 0, extra, FALSE, FALSE);
	अण्णहा जइ (extra < 0)
	    win_del_पङ्क्तयः(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    अण्णहा जइ (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    जइ (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#ifdef FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#endif
#जइ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    जइ (popup_is_popup(curwin) && curbuf->b_term != रिक्त)
    {
	curwin->w_wrow += popup_शिखरम्_extra(curwin);
	curwin->w_wcol += popup_वामः_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    अण्णहा
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#endif

    // now w_leftcol and w_skipcol are valid, avoid check_cursor_moved()
    // thinking otherwise
    curwin->w_valid_leftcol = curwin->w_leftcol;
    curwin->w_valid_skipcol = curwin->w_skipcol;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

#जइ defined(FEAT_EVAL) || defined(FEAT_PROP_POPUP)
/*
 * Compute the screen position of text character at "pos" in window "wp"
 * The resulting values are one-based, zero when character is not visible.
 */
    शून्य
textpos2screenpos(
	win_T	*wp,
	pos_T	*pos,
	पूर्णाङ्क	*rowp,	// screen row
	पूर्णाङ्क	*scolp,	// start screen column
	पूर्णाङ्क	*ccolp,	// cursor screen column
	पूर्णाङ्क	*ecolp)	// end screen column
{
    colnr_T	scol = 0, ccol = 0, ecol = 0;
    पूर्णाङ्क		row = 0;
    colnr_T	coloff = 0;

    जइ (pos->lnum >= wp->w_topline && pos->lnum <= wp->w_botline)
    {
	colnr_T	    col;
	पूर्णाङ्क	    width;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	पूर्णाङ्क	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);
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
	जइ (is_folded)
	{
	    row += W_WINROW(wp) + 1;
	    coloff = wp->w_wincol + 1 + off;
	}
	अण्णहा
# endif
	{
	    getvcol(wp, pos, &scol, &ccol, &ecol, 0);

	    // similar to what is done in validate_cursor_col()
	    col = scol;
	    col += off;
	    width = wp->w_width - off + win_col_off2(wp);

	    // long line wrapping, adjust row
	    जइ (wp->w_p_wrap
		    && col >= (colnr_T)wp->w_width
		    && width > 0)
	    {
		// use same formula as what is used in curs_columns()
		पूर्णाङ्क rowoff = ((col - wp->w_width) / width + 1);
		col -= rowoff * width;
		row += rowoff;
	    }
	    col -= wp->w_leftcol;
	    जइ (col >= wp->w_width)
		col = -1;
	    जइ (col >= 0 && row >= 0 && row < wp->w_height)
	    {
		coloff = col - scol + wp->w_wincol + 1;
		row += W_WINROW(wp) + 1;
	    }
	    अण्णहा
		// character is out of the window
		row = scol = ccol = ecol = 0;
	}
    }
    *rowp = row;
    *scolp = scol + coloff;
    *ccolp = ccol + coloff;
    *ecolp = ecol + coloff;
# ifdef FEAT_RIGHTLEFT
    जइ (wp->w_p_rl && row > 0)
    {
	// With 'rightleft' the cursor is on the leftmost cell of the
	// character, which comes last in reading order.
	पूर्णाङ्क endoff = *ecolp - wp->w_wincol - 1;

	*ccolp = wp->w_wincol + wp->w_width - endoff;
    }
# endif
}
#endif

#जइ defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    शून्य
f_screenpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    पूर्णाङ्क		row = 0;
    पूर्णाङ्क		scol = 0, ccol = 0, ecol = 0;

    जइ (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;
    dict = rettv->vval.v_dict;

    जइ (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    wp = find_win_by_nr_or_id(&argvars[0]);
    जइ (wp == रिक्त)
	प्रत्यागम;

    pos.lnum = tv_get_सङ्ख्या(&argvars[1]);
    जइ (pos.lnum > wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	semsg(_(e_invalid_पङ्क्तिः_सङ्ख्या_nr), pos.lnum);
	प्रत्यागम;
    }
    pos.col = tv_get_सङ्ख्या(&argvars[2]) - 1;
    जइ (pos.col < 0)
	pos.col = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &row, &scol, &ccol, &ecol);

    dict_add_सङ्ख्या(dict, "row", row);
    dict_add_सङ्ख्या(dict, "col", scol);
    dict_add_सङ्ख्या(dict, "curscol", ccol);
    dict_add_सङ्ख्या(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    स्थिर पूर्णाङ्क
virtcol2col(win_T *wp, linenr_T lnum, पूर्णाङ्क vcol)
{
    पूर्णाङ्क		offset = vcol2col(wp, lnum, vcol - 1, रिक्त);
    char_u	*पङ्क्तिः = ml_get_buf(wp->w_buffer, lnum, FALSE);
    char_u	*p = पङ्क्तिः + offset;

    जइ (*p == NUL)
    {
	जइ (p == पङ्क्तिः)  // empty line
	    प्रत्यागम 0;
	// Move to the first byte of the last char.
	MB_PTR_BACK(पङ्क्तिः, p);
    }
    प्रत्यागम (पूर्णाङ्क)(p - पङ्क्तिः + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    शून्य
f_virtcol2col(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    पूर्णाङ्क		screencol;
    पूर्णाङ्क		error = FALSE;

    rettv->vval.v_सङ्ख्या = -1;

    जइ (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
	    || check_for_सङ्ख्या_arg(argvars, 1) == FAIL
	    || check_for_सङ्ख्या_arg(argvars, 2) == FAIL)
	प्रत्यागम;

    wp = find_win_by_nr_or_id(&argvars[0]);
    जइ (wp == रिक्त)
	प्रत्यागम;

    lnum = tv_get_सङ्ख्या_chk(&argvars[1], &error);
    जइ (error || lnum < 0 || lnum > wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	प्रत्यागम;

    screencol = tv_get_सङ्ख्या_chk(&argvars[2], &error);
    जइ (error || screencol < 0)
	प्रत्यागम;

    rettv->vval.v_सङ्ख्या = virtcol2col(wp, lnum, screencol);
}
#endif

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
स्थिर शून्य cursor_correct_sms(शून्य)
{
    जइ (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	प्रत्यागम;

    long    so = get_scrolloff_मानम्();
    पूर्णाङ्क	    width1 = curwin->w_width - curwin_col_off();
    पूर्णाङ्क	    width2 = width1 + curwin_col_off2();
    पूर्णाङ्क	    so_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    पूर्णाङ्क	    space_cols = (curwin->w_height - 1) * width2;
    पूर्णाङ्क	    overlap, शिखरम्, bot;
    पूर्णाङ्क	    size = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    जइ (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_cols = 0;               // Ignore 'scrolloff' at top of buffer.
    अण्णहा जइ (so_cols > space_cols / 2)
	so_cols = space_cols / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    जाव (so_cols > size && so_cols - width2 >= width1 && width1 > 0)
	so_cols -= width2;
    जइ (so_cols >= width1 && so_cols > size)
	so_cols -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_width - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    शिखरम् = curwin->w_skipcol + (so_cols != 0 ? so_cols : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_height - 1) * width2
								    - so_cols;
    validate_virtcol();
    colnr_T col = curwin->w_virtcol;

    जइ (col < शिखरम्)
    {
	जइ (col < width1)
	    col += width1;
	जाव (width2 > 0 && col < शिखरम्)
	    col += width2;
    }
    अण्णहा
	जाव (width2 > 0 && col >= bot)
	    col -= width2;

    जइ (col != curwin->w_virtcol)
    {
	पूर्णाङ्क rc;

	curwin->w_curswant = col;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	जइ (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    validate_virtcol();
	    जइ (curwin->w_virtcol < curwin->w_skipcol + overlap)
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
    शून्य
scroll_redraw(पूर्णाङ्क up, long गणनम्)
{
    linenr_T	prev_topline = curwin->w_topline;
    पूर्णाङ्क		prev_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    पूर्णाङ्क		prev_topfill = curwin->w_topfill;
#endif
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    जइ (up)
	scrollup(गणनम्, TRUE);
    अण्णहा
	scrolldown(गणनम्, TRUE);
    जइ (get_scrolloff_मानम्() > 0)
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	जाव (curwin->w_topline == prev_topline
		&& curwin->w_skipcol == prev_skipcol
#ifdef FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#endif
		)
	{
	    जइ (up)
	    {
		जइ (curwin->w_cursor.lnum > prev_lnum
			|| cursor_down(1L, FALSE) == FAIL)
		    भङ्ग;
	    }
	    अण्णहा
	    {
		जइ (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_up(1L, FALSE) == FAIL)
		    भङ्ग;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }

    cursor_correct_sms();
    जइ (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(UPD_VALID);
}

/*
 * Scroll the current window down by "line_count" logical lines.  "CTRL-Y"
 */
    शून्य
scrolldown(
    long	पङ्क्तिः_गणनम्,
    पूर्णाङ्क		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    long	done = 0;	// total # of physical lines done
    पूर्णाङ्क		wrow;
    पूर्णाङ्क		moved = FALSE;
    पूर्णाङ्क		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    पूर्णाङ्क		width1 = 0;
    पूर्णाङ्क		width2 = 0;

    जइ (do_sms)
    {
	width1 = curwin->w_width - curwin_col_off();
	width2 = width1 + curwin_col_off2();
    }

#ifdef FEAT_FOLDING
    linenr_T	first;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (शून्य)hasFolding(curwin->w_topline, &curwin->w_topline, रिक्त);
#endif
    validate_cursor();		// w_wrow needs to be valid
    प्रति (पूर्णाङ्क todo = पङ्क्तिः_गणनम्; todo > 0; --todo)
    {
#ifdef FEAT_DIFF
	जइ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_height - 1)
	{
	    ++curwin->w_topfill;
	    ++done;
	}
	अण्णहा
#endif
	{
	    // break when at the very top
	    जइ (curwin->w_topline == 1
				   && (!do_sms || curwin->w_skipcol < width1))
		भङ्ग;
	    जइ (do_sms && curwin->w_skipcol >= width1)
	    {
		// scroll a screen line down
		जइ (curwin->w_skipcol >= width1 + width2)
		    curwin->w_skipcol -= width2;
		अण्णहा
		    curwin->w_skipcol -= width1;
		redraw_later(UPD_NOT_VALID);
		++done;
	    }
	    अण्णहा
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#ifdef FEAT_DIFF
		curwin->w_topfill = 0;
#endif
#ifdef FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		जइ (hasFolding(curwin->w_topline, &first, रिक्त))
		{
		    ++done;
		    जइ (!byfold)
			todo -= curwin->w_topline - first - 1;
		    curwin->w_botline -= curwin->w_topline - first;
		    curwin->w_topline = first;
		}
		अण्णहा
#endif
		जइ (do_sms)
		{
		    पूर्णाङ्क size = linetabsize_eol(curwin, curwin->w_topline);
		    जइ (size > width1)
		    {
			curwin->w_skipcol = width1;
			size -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    जाव (size > width2)
		    {
			curwin->w_skipcol += width2;
			size -= width2;
		    }
		    ++done;
		}
		अण्णहा
		    done += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += done;		// keep w_wrow updated
    curwin->w_cline_row += done;	// keep w_cline_row updated

#ifdef FEAT_DIFF
    जइ (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_row = 0;
    check_topfill(curwin, TRUE);
#endif

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    जइ (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    जाव (wrow >= curwin->w_height && curwin->w_cursor.lnum > 1)
    {
#ifdef FEAT_FOLDING
	जइ (hasFolding(curwin->w_cursor.lnum, &first, रिक्त))
	{
	    --wrow;
	    जइ (first == 1)
		curwin->w_cursor.lnum = 1;
	    अण्णहा
		curwin->w_cursor.lnum = first - 1;
	}
	अण्णहा
#endif
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    जइ (moved)
    {
#ifdef FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#endif
	coladvance(curwin->w_curswant);
    }
    जइ (curwin->w_cursor.lnum < curwin->w_topline)
	curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * Scroll the current window up by "line_count" logical lines.  "CTRL-E"
 */
    शून्य
scrollup(
    long	पङ्क्तिः_गणनम्,
    पूर्णाङ्क		byfold UNUSED)	// TRUE: count a closed fold as one line
{
    पूर्णाङ्क		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    जइ (do_sms
#ifdef FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#endif
#ifdef FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#endif
       )
    {
	पूर्णाङ्क	    width1 = curwin->w_width - curwin_col_off();
	पूर्णाङ्क	    width2 = width1 + curwin_col_off2();
	पूर्णाङ्क	    size = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	जइ (do_sms)
	    size = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	प्रति (पूर्णाङ्क todo = पङ्क्तिः_गणनम्; todo > 0; --todo)
	{
#ifdef FEAT_DIFF
	    जइ (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    अण्णहा
#endif
	    {
		linenr_T lnum = curwin->w_topline;

#ifdef FEAT_FOLDING
		जइ (byfold)
		    // for a closed fold: go to the last line in the fold
		    (शून्य)hasFolding(lnum, रिक्त, &lnum);
#endif
		जइ (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    पूर्णाङ्क add = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += add;
		    जइ (curwin->w_skipcol >= size)
		    {
			जइ (lnum == curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= add;
			    भङ्ग;
			}
			++lnum;
		    }
		}
		अण्णहा
		{
		    जइ (lnum >= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
			भङ्ग;
		    ++lnum;
		}

		जइ (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#ifdef FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#endif
		    curwin->w_skipcol = 0;
		    जइ (todo > 1 && do_sms)
			size = linetabsize_eol(curwin, curwin->w_topline);
		}
	    }
	}

	जइ (prev_skipcol > 0 || curwin->w_skipcol > 0)
	    // need to redraw more, because wl_size of the (new) topline may
	    // now be invalid
	    redraw_later(UPD_NOT_VALID);
    }
    अण्णहा
    {
	curwin->w_topline += पङ्क्तिः_गणनम्;
	curwin->w_botline += पङ्क्तिः_गणनम्;	// approximate w_botline
    }

    जइ (curwin->w_topline > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	curwin->w_topline = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
    जइ (curwin->w_botline > curbuf->b_ml.ml_पङ्क्तिः_गणनम् + 1)
	curwin->w_botline = curbuf->b_ml.ml_पङ्क्तिः_गणनम् + 1;

#ifdef FEAT_DIFF
    check_topfill(curwin, FALSE);
#endif

#ifdef FEAT_FOLDING
    जइ (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(शून्य)hasFolding(curwin->w_topline, &curwin->w_topline, रिक्त);
#endif

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    जइ (curwin->w_cursor.lnum < curwin->w_topline)
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
    शून्य
adjust_skipcol(शून्य)
{
    जइ (!curwin->w_p_wrap
	    || !curwin->w_p_sms
	    || curwin->w_cursor.lnum != curwin->w_topline)
	प्रत्यागम;

    पूर्णाङ्क	    width1 = curwin->w_width - curwin_col_off();
    जइ (width1 <= 0)
	प्रत्यागम;  // no text will be displayed

    पूर्णाङ्क	    width2 = width1 + curwin_col_off2();
    long    so = get_scrolloff_मानम्();
    पूर्णाङ्क	    scrolloff_cols = so == 0 ? 0 : width1 + (so - 1) * width2;
    पूर्णाङ्क	    scrolled = FALSE;
    पूर्णाङ्क	    row = 0;
    पूर्णाङ्क	    overlap, col;

    validate_cheight();
    जइ (curwin->w_cline_height == curwin->w_height
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_height)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	प्रत्यागम;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_width - width2);
    जाव (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_cols)
    {
	// scroll a screen line down
	जइ (curwin->w_skipcol >= width1 + width2)
	    curwin->w_skipcol -= width2;
	अण्णहा
	    curwin->w_skipcol -= width1;
	scrolled = TRUE;
    }
    जइ (scrolled)
    {
	validate_virtcol();
	redraw_later(UPD_NOT_VALID);
	प्रत्यागम;  // don't scroll in the other direction now
    }

    col = curwin->w_virtcol + scrolloff_cols;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    जइ (scrolloff_cols > 0)
    {
	पूर्णाङ्क size = linetabsize_eol(curwin, curwin->w_topline);
	size = width1 + width2 * ((size - width1 + width2 - 1) / width2);
	जाव (col > size)
	    col -= width2;
    }
    col -= curwin->w_skipcol;

    जइ (col >= width1)
    {
	col -= width1;
	++row;
    }
    जइ (col > width2)
    {
	row += col / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	col = col % width2;
    }
    जइ (row >= curwin->w_height)
    {
	जइ (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --row;
	}
	जइ (row >= curwin->w_height)
	    curwin->w_skipcol += (row - curwin->w_height) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#ifdef FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    शून्य
check_topfill(
    win_T	*wp,
    पूर्णाङ्क		down)	// when TRUE scroll down when not enough space
{
    पूर्णाङ्क		n;

    जइ (wp->w_topfill <= 0)
	प्रत्यागम;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    जइ (wp->w_topfill + n > wp->w_height)
    {
	जइ (down && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	अण्णहा
	{
	    wp->w_topfill = wp->w_height - n;
	    जइ (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#endif

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    शून्य
scrolldown_clamp(शून्य)
{
    पूर्णाङ्क		end_row;
#ifdef FEAT_DIFF
    पूर्णाङ्क		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#endif

    जइ (curwin->w_topline <= 1
#ifdef FEAT_DIFF
	    && !can_fill
#endif
	    )
	प्रत्यागम;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    end_row = curwin->w_wrow;
#ifdef FEAT_DIFF
    जइ (can_fill)
	++end_row;
    अण्णहा
	end_row += plines_nofill(curwin->w_topline - 1);
#अण्णहा
    end_row += plines(curwin->w_topline - 1);
#endif
    जइ (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_cheight();
	validate_virtcol();
	end_row += curwin->w_cline_height - 1 -
	    curwin->w_virtcol / curwin->w_width;
    }
    जइ (end_row < curwin->w_height - get_scrolloff_मानम्())
    {
#ifdef FEAT_DIFF
	जइ (can_fill)
	{
	    ++curwin->w_topfill;
	    check_topfill(curwin, TRUE);
	}
	अण्णहा
	{
	    --curwin->w_topline;
	    curwin->w_topfill = 0;
	}
#अण्णहा
	--curwin->w_topline;
#endif
#ifdef FEAT_FOLDING
	(शून्य)hasFolding(curwin->w_topline, &curwin->w_topline, रिक्त);
#endif
	--curwin->w_botline;	    // approximate w_botline
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    शून्य
scrollup_clamp(शून्य)
{
    पूर्णाङ्क	    start_row;

    जइ (curwin->w_topline == curbuf->b_ml.ml_पङ्क्तिः_गणनम्
#ifdef FEAT_DIFF
	    && curwin->w_topfill == 0
#endif
	    )
	प्रत्यागम;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#ifdef FEAT_DIFF
    start_row = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#अण्णहा
    start_row = curwin->w_wrow - plines(curwin->w_topline);
#endif
    जइ (curwin->w_p_wrap && curwin->w_width != 0)
    {
	validate_virtcol();
	start_row -= curwin->w_virtcol / curwin->w_width;
    }
    जइ (start_row >= get_scrolloff_मानम्())
    {
#ifdef FEAT_DIFF
	जइ (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	अण्णहा
#endif
	{
#ifdef FEAT_FOLDING
	    (शून्य)hasFolding(curwin->w_topline, रिक्त, &curwin->w_topline);
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
    स्थिर शून्य
topline_back_winheight(
    lineoff_T	*lp,
    पूर्णाङ्क		winheight)	// when TRUE limit to window height
{
#ifdef FEAT_DIFF
    जइ (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    अण्णहा
#endif
    {
	--lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	जइ (lp->lnum < 1)
	    lp->height = MAXCOL;
	अण्णहा
#ifdef FEAT_FOLDING
	    जइ (hasFolding(lp->lnum, &lp->lnum, रिक्त))
	    // Add a closed fold
	    lp->height = 1;
	अण्णहा
#endif
	    lp->height = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    स्थिर शून्य
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
    स्थिर शून्य
botline_forw(lineoff_T *lp)
{
#ifdef FEAT_DIFF
    जइ (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->height = 1;
    }
    अण्णहा
#endif
    {
	++lp->lnum;
#ifdef FEAT_DIFF
	lp->fill = 0;
#endif
	जइ (lp->lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    lp->height = MAXCOL;
	अण्णहा
#ifdef FEAT_FOLDING
	    जइ (hasFolding(lp->lnum, रिक्त, &lp->lnum))
	    {
		// Add a closed fold.
		lp->height = 1;
	    }
	    अण्णहा
#endif
	    lp->height = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    शून्य
scroll_cursor_शिखरम्(पूर्णाङ्क min_scroll, पूर्णाङ्क always)
{
    पूर्णाङ्क		scrolled = 0;
    पूर्णाङ्क		extra = 0;
    पूर्णाङ्क		used;
    पूर्णाङ्क		i;
    linenr_T	शिखरम्;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    पूर्णाङ्क		old_skipcol = curwin->w_skipcol;
#ifdef FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#endif
    linenr_T	new_topline;
    पूर्णाङ्क		off = get_scrolloff_मानम्();

    जइ (mouse_dragging > 0)
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
    जइ (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = used;

#ifdef FEAT_FOLDING
    जइ (hasFolding(curwin->w_cursor.lnum, &शिखरम्, &bot))
    {
	--शिखरम्;
	++bot;
    }
    अण्णहा
#endif
    {
	शिखरम् = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = शिखरम् + 1;

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
    जाव (शिखरम् > 0)
    {
#ifdef FEAT_FOLDING
	जइ (hasFolding(शिखरम्, &शिखरम्, रिक्त))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	अण्णहा
#endif
	    i = PLINES_NOFILL(शिखरम्);
	जइ (शिखरम् < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	जइ ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    भङ्ग;

	used += i;
	जइ (extra + i <= off && bot < curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
#ifdef FEAT_FOLDING
	    जइ (hasFolding(bot, रिक्त, &bot))
		// count one logical line for a sequence of folded lines
		++used;
	    अण्णहा
#endif
		used += plines(bot);
	}
	जइ (used > curwin->w_height)
	    भङ्ग;

	extra += i;
	new_topline = शिखरम्;
	--शिखरम्;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    जइ (used > curwin->w_height)
	scroll_cursor_halfway(FALSE, FALSE);
    अण्णहा
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines.
	 */
	जइ (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	जइ (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
#ifdef FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	जइ (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    जइ (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#endif
	जइ (curwin->w_topline != old_topline)
	    reset_skipcol();
	अण्णहा जइ (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    जइ (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		पूर्णाङ्क	width1 = curwin->w_width - curwin_col_off();
		पूर्णाङ्क	width2 = width1 + curwin_col_off2();
		पूर्णाङ्क	plines_off = 0;
		पूर्णाङ्क	skipcol;

		जइ (width2 > 0 && curwin->w_virtcol >= (colnr_T)width1)
		    plines_off = (curwin->w_virtcol - width1) / width2 + 1;
		skipcol = skipcol_from_plines(curwin, plines_off);
		जइ (skipcol != curwin->w_skipcol)
		{
		    curwin->w_skipcol = skipcol;
		    redraw_later(UPD_SOME_VALID);
		}
	    }
	}
	जइ (curwin->w_topline != old_topline
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
    शून्य
set_empty_rows(win_T *wp, पूर्णाङ्क used)
{
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    जइ (used == 0)
	wp->w_empty_rows = 0;	// single line that doesn't fit
    अण्णहा
    {
	wp->w_empty_rows = wp->w_height - used;
#ifdef FEAT_DIFF
	जइ (wp->w_botline <= wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    wp->w_filler_rows = diff_check_fill(wp, wp->w_botline);
	    जइ (wp->w_empty_rows > wp->w_filler_rows)
		wp->w_empty_rows -= wp->w_filler_rows;
	    अण्णहा
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
    शून्य
scroll_cursor_bot(पूर्णाङ्क min_scroll, पूर्णाङ्क set_topbot)
{
    पूर्णाङ्क		used;
    पूर्णाङ्क		scrolled = 0;
    पूर्णाङ्क		extra = 0;
    पूर्णाङ्क		i;
    linenr_T	पङ्क्तिः_गणनम्;
    linenr_T	old_topline = curwin->w_topline;
    पूर्णाङ्क		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#ifdef FEAT_DIFF
    पूर्णाङ्क		old_topfill = curwin->w_topfill;
    पूर्णाङ्क		fill_below_window;
#endif
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    पूर्णाङ्क		old_empty_rows = curwin->w_empty_rows;
    linenr_T	cln;		    // Cursor Line Number
    long	so = get_scrolloff_मानम्();
    पूर्णाङ्क		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    जइ (set_topbot)
    {
	used = 0;
	linenr_T cln_last = cln;
#ifdef FEAT_FOLDING
	(शून्य)hasFolding(cln, रिक्त, &cln_last);
#endif
	curwin->w_botline = cln_last + 1;
	loff.lnum = cln_last + 1;
#ifdef FEAT_DIFF
	loff.fill = 0;
#endif
	जाव (TRUE)
	{
	    topline_back_winheight(&loff, FALSE);
	    जइ (loff.height == MAXCOL)
		भङ्ग;
	    जइ (used + loff.height > curwin->w_height)
	    {
		जइ (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    जइ (used < curwin->w_height)
		    {
			पूर्णाङ्क plines_offset = used + loff.height
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
		भङ्ग;
	    }
#ifdef FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#endif
	    curwin->w_topline = loff.lnum;
	    used += loff.height;
	}

	set_empty_rows(curwin, used);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	जइ (curwin->w_topline != old_topline
#ifdef FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#endif
		|| curwin->w_skipcol != old_skipcol
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    जइ (curwin->w_skipcol != old_skipcol)
		redraw_later(UPD_NOT_VALID);
	    अण्णहा
		reset_skipcol();
	}
    }
    अण्णहा
	validate_botline();

    // The lines of the cursor line itself are always used.
#ifdef FEAT_DIFF
    used = plines_nofill(cln);
#अण्णहा
    validate_cheight();
    used = curwin->w_cline_height;
#endif

    जइ (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	पूर्णाङ्क upto_cursor = plines_win_col(curwin, cln, curwin->w_cursor.col);

	जइ (upto_cursor < used)
	    used = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    जइ (cln >= curwin->w_botline)
    {
	scrolled = used;
	जइ (cln == curwin->w_botline)
	    scrolled -= curwin->w_empty_rows;
	जइ (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    पूर्णाङ्क शिखरम्_plines =
#ifdef FEAT_DIFF
			    plines_win_nofill
#अण्णहा
			    plines_win
#endif
					(curwin, curwin->w_topline, FALSE);
	    पूर्णाङ्क width1 = curwin->w_width - curwin_col_off();

	    जइ (width1 > 0)
	    {
		पूर्णाङ्क width2 = width1 + curwin_col_off2();
		पूर्णाङ्क skip_पङ्क्तयः = 0;

		// A similar formula is used in curs_columns().
		जइ (curwin->w_skipcol > width1)
		    skip_पङ्क्तयः += (curwin->w_skipcol - width1) / width2 + 1;
		अण्णहा जइ (curwin->w_skipcol > 0)
		    skip_पङ्क्तयः = 1;

		शिखरम्_plines -= skip_पङ्क्तयः;
		जइ (शिखरम्_plines > curwin->w_height)
		{
		    scrolled += (शिखरम्_plines - curwin->w_height);
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
    जइ (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
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

    जाव (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	जइ ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		&& loff.lnum <= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_below_window)
#endif
		)
	    भङ्ग;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_back(&loff);
	जइ (loff.height == MAXCOL)
	    used = MAXCOL;
	अण्णहा
	    used += loff.height;
	जइ (used > curwin->w_height)
	    भङ्ग;
	जइ (loff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_below_window)
#endif
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.height;
	    जइ (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_empty_rows;
	}

	जइ (boff.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    used += boff.height;
	    जइ (used > curwin->w_height)
		भङ्ग;
	    जइ (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.height;
		जइ (boff.lnum >= curwin->w_botline
#ifdef FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_rows)
#endif
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.height;
		    जइ (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_empty_rows;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    जइ (scrolled <= 0)
	पङ्क्तिः_गणनम् = 0;
    // more than a screenfull, don't scroll but redraw
    अण्णहा जइ (used > curwin->w_height)
	पङ्क्तिः_गणनम् = used;
    // scroll minimal number of lines
    अण्णहा
    {
	पङ्क्तिः_गणनम् = 0;
#ifdef FEAT_DIFF
	boff.fill = curwin->w_topfill;
#endif
	boff.lnum = curwin->w_topline - 1;
	प्रति (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.height;
	    ++पङ्क्तिः_गणनम्;
	}
	जइ (i < scrolled)	// below curwin->w_botline, don't scroll
	    पङ्क्तिः_गणनम् = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    जइ (पङ्क्तिः_गणनम् >= curwin->w_height && पङ्क्तिः_गणनम् > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    अण्णहा जइ (पङ्क्तिः_गणनम् > 0)
    {
	जइ (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	अण्णहा
	    scrollup(पङ्क्तिः_गणनम्, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    जइ (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && set_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_empty_rows = old_empty_rows;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    जइ (set_topbot)
	cursor_correct_sms();
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    शून्य
scroll_cursor_halfway(पूर्णाङ्क atend, पूर्णाङ्क prefer_above)
{
    पूर्णाङ्क		above = 0;
    linenr_T	topline;
    colnr_T	skipcol = 0;
#ifdef FEAT_DIFF
    पूर्णाङ्क		topfill = 0;
#endif
    पूर्णाङ्क		below = 0;
    पूर्णाङ्क		used;
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
    (शून्य)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#endif
#ifdef FEAT_DIFF
    used = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#अण्णहा
    used = plines(loff.lnum);
#endif
    topline = loff.lnum;

    पूर्णाङ्क want_height;
    पूर्णाङ्क do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    जइ (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	जइ (atend)
	{
	    want_height = (curwin->w_height - used) / 2;
	    used = 0;
	}
	अण्णहा
	    want_height = curwin->w_height;
    }

    जाव (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	जइ (do_sms)
	{
	    topline_back_winheight(&loff, FALSE);
	    जइ (loff.height == MAXCOL)
		भङ्ग;
	    used += loff.height;
	    जइ (!atend && boff.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    {
		botline_forw(&boff);
		used += boff.height;
	    }
	    जइ (used > want_height)
	    {
		जइ (used - loff.height < want_height)
		{
		    topline = loff.lnum;
#ifdef FEAT_DIFF
		    topfill = loff.fill;
#endif
		    skipcol = skipcol_from_plines(curwin, used - want_height);
		}
		भङ्ग;
	    }
	    topline = loff.lnum;
#ifdef FEAT_DIFF
	    topfill = loff.fill;
#endif
	    अनुवर्त;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	पूर्णाङ्क done = FALSE;
	प्रति (पूर्णाङ्क round = 1; round <= 2; ++round)
	{
	    जइ (prefer_above ? (round == 2 && below < above)
			     : (round == 1 && below <= above))
	    {
		// add a line below the cursor
		जइ (boff.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		{
		    botline_forw(&boff);
		    used += boff.height;
		    जइ (used > curwin->w_height)
		    {
			done = TRUE;
			भङ्ग;
		    }
		    below += boff.height;
		}
		अण्णहा
		{
		    ++below;	    // count a "~" line
		    जइ (atend)
			++used;
		}
	    }

	    जइ (prefer_above ? (round == 1 && below >= above)
			     : (round == 1 && below > above))
	    {
		// add a line above the cursor
		topline_back(&loff);
		जइ (loff.height == MAXCOL)
		    used = MAXCOL;
		अण्णहा
		    used += loff.height;
		जइ (used > curwin->w_height)
		{
		    done = TRUE;
		    भङ्ग;
		}
		above += loff.height;
		topline = loff.lnum;
#ifdef FEAT_DIFF
		topfill = loff.fill;
#endif
	    }
	}
	जइ (done)
	    भङ्ग;
    }

#ifdef FEAT_FOLDING
    जइ (!hasFolding(topline, &curwin->w_topline, रिक्त))
#endif
    {
	जइ (curwin->w_topline != topline
		|| skipcol != 0
		|| curwin->w_skipcol != 0)
	{
	    curwin->w_topline = topline;
	    जइ (skipcol != 0)
	    {
		curwin->w_skipcol = skipcol;
		redraw_later(UPD_NOT_VALID);
	    }
	    अण्णहा जइ (do_sms)
		reset_skipcol();
	}
    }
#ifdef FEAT_DIFF
    curwin->w_topfill = topfill;
    जइ (old_topline > curwin->w_topline + curwin->w_height)
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
    शून्य
cursor_correct(शून्य)
{
    पूर्णाङ्क		above = 0;	    // screen lines above topline
    linenr_T	topline;
    पूर्णाङ्क		below = 0;	    // screen lines below botline
    linenr_T	botline;
    पूर्णाङ्क		above_wanted, below_wanted;
    linenr_T	cln;		    // Cursor Line Number
    पूर्णाङ्क		max_off;
    long	so = get_scrolloff_मानम्();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    below_wanted = so;
    जइ (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	below_wanted = mouse_dragging - 1;
    }
    जइ (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_height / 2;
	जइ (below_wanted > max_off)
	    below_wanted = max_off;
    }
    validate_botline();
    जइ (curwin->w_botline == curbuf->b_ml.ml_पङ्क्तिः_गणनम् + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	below_wanted = 0;
	max_off = (curwin->w_height - 1) / 2;
	जइ (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    जइ (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - below_wanted
#ifdef FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#endif
	    )
	प्रत्यागम;

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
    जाव ((above < above_wanted || below < below_wanted) && topline < botline)
    {
	जइ (below < below_wanted && (below <= above || above >= above_wanted))
	{
#ifdef FEAT_FOLDING
	    जइ (hasFolding(botline, &botline, रिक्त))
		++below;
	    अण्णहा
#endif
		below += plines(botline);
	    --botline;
	}
	जइ (above < above_wanted && (above < below || below >= below_wanted))
	{
#ifdef FEAT_FOLDING
	    जइ (hasFolding(topline, रिक्त, &topline))
		++above;
	    अण्णहा
#endif
		above += PLINES_NOFILL(topline);
#ifdef FEAT_DIFF
	    // Count filler lines below this line as context.
	    जइ (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#endif
	    ++topline;
	}
    }
    जइ (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    अण्णहा जइ (topline > botline)
	curwin->w_cursor.lnum = botline;
    अण्णहा
    {
	जइ (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	जइ (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
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
स्थिर पूर्णाङ्क get_scroll_overlap(पूर्णाङ्क dir)
{
    lineoff_T loff;
    पूर्णाङ्क	    min_height = curwin->w_height - 2;

    validate_botline();
    जइ ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_पङ्क्तिः_गणनम्))
	प्रत्यागम min_height + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#ifdef FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_rows : curwin->w_topfill);
    loff.height = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#अण्णहा
    loff.height = plines(loff.lnum);
#endif

    पूर्णाङ्क h1 = loff.height;
    जइ (h1 > min_height)
	प्रत्यागम min_height + 2;  // no overlap
    जइ (dir == FORWARD)
	topline_back(&loff);
    अण्णहा
	botline_forw(&loff);

    पूर्णाङ्क h2 = loff.height;
    जइ (h2 == MAXCOL || h2 + h1 > min_height)
	प्रत्यागम min_height + 2;  // no overlap
    जइ (dir == FORWARD)
	topline_back(&loff);
    अण्णहा
	botline_forw(&loff);

    पूर्णाङ्क h3 = loff.height;
    जइ (h3 == MAXCOL || h3 + h2 > min_height)
	प्रत्यागम min_height + 2;  // no overlap
    जइ (dir == FORWARD)
	topline_back(&loff);
    अण्णहा
	botline_forw(&loff);

    पूर्णाङ्क h4 = loff.height;
    जइ (h4 == MAXCOL || h4 + h3 + h2 > min_height || h3 + h2 + h1 > min_height)
	प्रत्यागम min_height + 1;  // 1 line overlap
    अण्णहा
	प्रत्यागम min_height;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
स्थिर पूर्णाङ्क scroll_with_sms(पूर्णाङ्क dir, long गणनम्, long *curscount)
{
    पूर्णाङ्क		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    पूर्णाङ्क		prev_topfill = curwin->w_topfill;
#endif

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, गणनम्);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    जइ (!prev_sms && curwin->w_skipcol > 0)
    {
	पूर्णाङ्क fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	जइ (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	पूर्णाङ्क width1 = curwin->w_width - curwin_col_off();
	पूर्णाङ्क width2 = width1 + curwin_col_off2();
	गणनम् = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	जइ (fixdir == FORWARD)
	    गणनम् = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, गणनम्);
	*curscount += गणनम् * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    प्रत्यागम curwin->w_topline != prev_topline
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
    पूर्णाङ्क
pagescroll(पूर्णाङ्क dir, long गणनम्, पूर्णाङ्क half)
{
    पूर्णाङ्क		did_move = FALSE;
    पूर्णाङ्क		buflen = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
    colnr_T	prev_col = curwin->w_cursor.col;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    जइ (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	जइ (गणनम्)
	    curwin->w_p_scr = MIN(curwin->w_height, गणनम्);
	गणनम् = MIN(curwin->w_height, curwin->w_p_scr);

	long curscount = गणनम्;
	// Adjust count so as to not reveal end of buffer lines.
	जइ (dir == FORWARD
		    && (curwin->w_topline + curwin->w_height + गणनम् > buflen
#ifdef FEAT_FOLDING
							|| hasAnyFolding(curwin)
#endif
	   ))
	{
	    पूर्णाङ्क n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    जइ (n - गणनम् < curwin->w_height && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_height + गणनम्);
	    जइ (n < curwin->w_height + गणनम्)
		गणनम् = n - curwin->w_height;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	जइ (गणनम् > 0)
	{
	    did_move = scroll_with_sms(dir, गणनम्, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.col = prev_col;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	जइ (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	अण्णहा जइ (dir == FORWARD)
	    cursor_down_inner(curwin, curscount);
	अण्णहा
	    cursor_up_inner(curwin, curscount);
    }
    अण्णहा
    {
	// Scroll [count] times 'window' or current window height lines.
	गणनम् *= ((ONE_WINDOW && p_window > 0 && p_window < Rows - 1) ?
				MAX(1, p_window - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, गणनम्, &गणनम्);

	जइ (did_move)
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

    जइ (get_scrolloff_मानम्() > 0)
	cursor_correct();
#ifdef FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#endif
    did_move = did_move
	|| prev_col != curwin->w_cursor.col
	|| prev_lnum != curwin->w_cursor.lnum;

    // Error if both the viewport and cursor did not change.
    जइ (!did_move)
	beep_flush();
    अण्णहा जइ (!curwin->w_p_sms)
	beginline(BL_SOL | BL_FIX);
    अण्णहा जइ (p_sol)
	nv_g_home_m_cmd(&ca);

    प्रत्यागम did_move ? OK : FAIL;
}

    शून्य
do_check_cursorbind(शून्य)
{
    स्थिर win_T	*prev_curwin = रिक्त;
    स्थिर buf_T	*prev_curbuf = रिक्त;
    स्थिर varnumber_T	prev_changedtick = 0;
    स्थिर pos_T	prev_cursor = {0, 0, 0};

    // Nothing to do when the cursor didn't move and the text didn't change.
    // After a change the corresponding line in a diff may be different.
    जइ (curwin == prev_curwin && curbuf == prev_curbuf
	    && CHANGEDTICK(curbuf) == prev_changedtick
	    && EQUAL_POS(curwin->w_cursor, prev_cursor))
	प्रत्यागम;
    prev_curwin = curwin;
    prev_curbuf = curbuf;
    prev_changedtick = CHANGEDTICK(curbuf);
    prev_cursor = curwin->w_cursor;

    linenr_T	पङ्क्तिः = curwin->w_cursor.lnum;
    colnr_T	col = curwin->w_cursor.col;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    पूर्णाङ्क		set_curswant = curwin->w_set_curswant;
    win_T	*old_curwin = curwin;
    buf_T	*old_curbuf = curbuf;
    पूर्णाङ्क		old_VIsual_select = VIsual_select;
    पूर्णाङ्क		old_VIsual_active = VIsual_active;

    /*
     * loop through the cursorbound windows
     */
    VIsual_select = VIsual_active = 0;
    FOR_ALL_WINDOWS(curwin)
    {
	curbuf = curwin->w_buffer;
	// skip original window and windows with 'nocursorbind'
	जइ (curwin != old_curwin && curwin->w_p_crb)
	{
#ifdef FEAT_DIFF
	    जइ (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_पङ्क्तिः(old_curbuf, पङ्क्तिः);
	    अण्णहा
#endif
		curwin->w_cursor.lnum = पङ्क्तिः;
	    curwin->w_cursor.col = col;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_set_curswant = set_curswant;

	    // Make sure the cursor is in a valid position.  Temporarily set
	    // "restart_edit" to allow the cursor to be beyond the EOL.
	    पूर्णाङ्क restart_edit_save = restart_edit;
	    restart_edit = 'a';
	    check_cursor();

	    // Avoid a scroll here for the cursor position, 'scrollbind' is
	    // more important.
	    जइ (!curwin->w_p_scb)
		validate_cursor();

	    restart_edit = restart_edit_save;
	    // Correct cursor for multi-byte character.
	    जइ (has_mbyte)
		mb_adjust_cursor();
	    redraw_later(UPD_VALID);

	    // Only scroll when 'scrollbind' hasn't done this.
	    जइ (!curwin->w_p_scb)
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
