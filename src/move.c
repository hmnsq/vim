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
ساكن صحيح check_چوٽي_وٿي(فراغ);
ساكن فراغ curs_صفون(win_T *wp);

تعريفنوع بنية
{
    linenr_T	    lnum;	// line number
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		    fill;	// filler lines
#جيڪڏهن_پورو
    صحيح		    اوچائي;	// height of added line
} lineoff_T;

ساكن فراغ topline_پٺ(lineoff_T *lp);
ساكن فراغ botline_forw(lineoff_T *lp);

/*
 * Get the number of screen lines skipped with "wp->w_skipcol".
 */
    ساكن صحيح
adjust_plines_for_skipcol(win_T *wp)
{
    إذا (wp->w_skipcol == 0)
	أرجع 0;

    صحيح ويڪر = wp->w_ويڪر - win_کانو_off(wp);
    صحيح w2 = ويڪر + win_کانو_off2(wp);
    إذا (wp->w_skipcol >= ويڪر && w2 > 0)
	أرجع (wp->w_skipcol - ويڪر) / w2 + 1;

    أرجع 0;
}

/*
 * Return how many lines "lnum" will take on the screen, taking into account
 * whether it is the first line, whether w_skipcol is non-zero and limiting to
 * the window height.
 */
    صحيح
plines_correct_topline(win_T *wp, linenr_T lnum, صحيح حد_بندي_winheight)
{
    صحيح n;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    إذا (lnum == wp->w_topline)
	n = plines_win_nofill(wp, lnum, FALSE) + wp->w_topfill;
    وإلا
#جيڪڏهن_پورو
	n = plines_win(wp, lnum, FALSE);
    إذا (lnum == wp->w_topline)
	n -= adjust_plines_for_skipcol(wp);
    إذا (حد_بندي_winheight && n > wp->w_اوچائي)
	n = wp->w_اوچائي;
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
    صحيح		پورو;
    صحيح		i = 0;
    صحيح		use_cache;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    linenr_T    آخري;
    صحيح		folded;
#جيڪڏهن_پورو

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
		    && !wp->w_بفر->b_mod_سيٽ
		    && dollar_vcol == -1
		    && wp->w_skipcol == 0
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		    && !wp->w_p_diff
#جيڪڏهن_پورو
		    && wp->w_سٽون_valid > 0
		    && wp->w_سٽون[0].wl_lnum <= wp->w_topline;

    إذا (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	پورو = wp->w_cline_صف;
    }
    وإلا
    {
	lnum = wp->w_topline;
	پورو = 0;
    }

    // Find the w_lines[] entry for the starting line.
    إذا (use_cache)
	طالما (i < wp->w_سٽون_valid && wp->w_سٽون[i].wl_lnum < lnum)
	    ++i;

    لكل ( ; lnum <= wp->w_بفر->b_ml.ml_سٽ_ڳڻپ; ++i)
    {
	صحيح	valid = FALSE;

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	آخري = lnum;
	folded = FALSE;
#جيڪڏهن_پورو
	// Try to use the size from the previous redraw.
	إذا (use_cache && i < wp->w_سٽون_valid)
	{
	    إذا (wp->w_سٽون[i].wl_lnum < lnum || !wp->w_سٽون[i].wl_valid)
		استمر;		// skip changed or deleted lines
	    إذا (wp->w_سٽون[i].wl_lnum == lnum)
		valid = TRUE;
	    وإلا // wl_lnum > lnum
		--i;			// hold at inserted lines
	}

	// The cache is not used with 'smoothscroll' or in diff mode, so here
	// wl_size holds the same height as plines_correct_topline().
	إذا (valid)
	{
	    n = wp->w_سٽون[i].wl_سائيز;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    folded = wp->w_سٽون[i].wl_folded;
	    آخري = wp->w_سٽون[i].wl_lastlnum;
#جيڪڏهن_پورو
	}
	وإلا
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    إذا (hasFoldingWin(wp, lnum, عدم, &آخري, TRUE, عدم))
	    {
		n = 1;
		folded = TRUE;
	    }
	    وإلا
#جيڪڏهن_پورو
		n = plines_correct_topline(wp, lnum, TRUE);
	}
	إذا (
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		lnum <= wp->w_cursor.lnum && آخري >= wp->w_cursor.lnum
#وإلا
		lnum == wp->w_cursor.lnum
#جيڪڏهن_پورو
	   )
	{
	    wp->w_cline_صف = پورو;
	    wp->w_cline_اوچائي = n;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    wp->w_cline_folded = folded;
#جيڪڏهن_پورو
	    redraw_for_cursorline(wp);
	    wp->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	إذا (پورو + n > wp->w_اوچائي)
	    اكسر;
	پورو += n;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	lnum = آخري + 1;
#وإلا
	++lnum;
#جيڪڏهن_پورو
    }

    // wp->w_botline is the line that is just below the window
    wp->w_botline = lnum;
    wp->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    سيٽ_خالي_جايون_صفون(wp, پورو);
}

/*
 * Redraw when w_cline_row changes and 'relativenumber' or 'cursorline' is
 * set.
 */
    ساكن فراغ
redraw_for_cursorline(win_T *wp)
{
    إذا ((wp->w_p_rnu
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
		|| wp->w_p_cul
#جيڪڏهن_پورو
		)
	    && (wp->w_valid & VALID_CROW) == 0
	    && !pum_visible())
    {
	// win_line() will redraw the number column and cursorline only.
	redraw_win_later(wp, UPD_VALID);
    }
}

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
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
#جيڪڏهن_پورو

/*
 * Set wp->w_virtcol to a value ("vcol") that is already valid.
 * Handles redrawing if wp->w_virtcol was previously invalid.
 */
    فراغ
سيٽ_valid_virtcol(win_T *wp, colnr_T vcol)
{
    wp->w_virtcol = vcol;
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#جيڪڏهن_پورو
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
	extra2 = win_کانو_off(wp) - win_کانو_off2(wp);
#إذا defined(FEAT_LINEBREAK)
    // There is no marker overlap when in showbreak mode, thus no need to
    // account for it.  See wlv_screen_line().
    إذا (*get_showbreak_قدر(wp) != NUL)
	أرجع 0;
#جيڪڏهن_پورو
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
    صحيح width1 = wp->w_ويڪر - win_کانو_off(wp);

    صحيح skipcol = 0;
    إذا (plines_off > 0)
	skipcol += width1;
    إذا (plines_off > 1)
	skipcol += (width1 + win_کانو_off2(wp)) * (plines_off - 1);
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
scrolloffpad_eof_pressure(linenr_T lnum, ڊگهو so)
{
    إذا (!use_scrolloffpad() || so <= 0)
	أرجع false;

    // Use subtraction to avoid signed overflow in "lnum + so".
    أرجع lnum > curbuf->b_ml.ml_سٽ_ڳڻپ - so;
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    فراغ
update_topline(فراغ)
{
    ڊگهو	سٽ_ڳڻپ;
    صحيح		halfheight;
    صحيح		n;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    linenr_T	lnum;
#جيڪڏهن_پورو
    صحيح		check_topline = FALSE;
    صحيح		check_botline = FALSE;
    ڊگهو	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;
    صحيح		save_so = *so_ptr;
    bool	eof_pressure;

    // Cursor is updated instead when this is TRUE for 'splitkeep'.
    إذا (skip_update_topline)
	أرجع;

    // If there is no valid screen and when the window height is zero just use
    // the cursor line.
    إذا (!screen_valid(TRUE) || curwin->w_اوچائي == 0)
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح old_topfill = curwin->w_topfill;
#جيڪڏهن_پورو

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
	    وإلا إذا (check_چوٽي_وٿي())
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    // Check if there are more filler lines than allowed.
	إذا (!check_topline && curwin->w_topfill > diff_check_fill(curwin,
							   curwin->w_topline))
	    check_topline = TRUE;
#جيڪڏهن_پورو

	إذا (check_topline)
	{
	    halfheight = curwin->w_اوچائي / 2 - 1;
	    إذا (halfheight < 2)
		halfheight = 2;

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
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
		    إذا (lnum >= curbuf->b_ml.ml_سٽ_ڳڻپ || n >= halfheight)
			اكسر;
		    (فراغ)hasFolding(lnum, عدم, &lnum);
		}
	    }
	    وإلا
#جيڪڏهن_پورو
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
		scroll_cursor_چوٽي(min_scroll, FALSE);
		check_botline = TRUE;
	    }
	}

	وإلا
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    // Make sure topline is the first line of a fold.
	    (فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#جيڪڏهن_پورو
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

	إذا (curwin->w_botline <= curbuf->b_ml.ml_سٽ_ڳڻپ || use_scrolloffpad())
	{
	    إذا (curwin->w_cursor.lnum < curwin->w_botline)
	    {
		إذا (((ڊگهو)curwin->w_cursor.lnum
					   >= (ڊگهو)curwin->w_botline - *so_ptr
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
			|| hasAnyFolding(curwin)
#جيڪڏهن_پورو
			))
		{
		    lineoff_T	loff;

		    // Cursor is (a few lines) above botline, check if there
		    // are 'scrolloff' window lines below the cursor.
		    // If not, need to scroll.
		    n = eof_pressure ? 0 : curwin->w_خالي_جايون_صفون;
		    loff.lnum = curwin->w_cursor.lnum;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		    // In a fold go to its last line.
		    (فراغ)hasFolding(loff.lnum, عدم, &loff.lnum);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		    loff.fill = 0;
		    n += curwin->w_filler_صفون;
#جيڪڏهن_پورو
		    loff.اوچائي = 0;
		    طالما (loff.lnum < curwin->w_botline
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
			    && (loff.lnum + 1 < curwin->w_botline
							     || loff.fill == 0)
#جيڪڏهن_پورو
			    )
		    {
			n += loff.اوچائي;
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
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		إذا (hasAnyFolding(curwin))
		{
		    // Count the number of logical lines between the cursor and
		    // botline - scrolloff (approximation of how much will be
		    // scrolled).
		    سٽ_ڳڻپ = 0;
		    لكل (lnum = curwin->w_cursor.lnum;
				   lnum >= curwin->w_botline - *so_ptr; --lnum)
		    {
			++سٽ_ڳڻپ;
			// stop at end of file or when we know we are far off
			إذا (lnum <= 0 || سٽ_ڳڻپ > curwin->w_اوچائي + 1)
			    اكسر;
			(فراغ)hasFolding(lnum, &lnum, عدم);
		    }
		}
		وإلا
#جيڪڏهن_پورو
		    سٽ_ڳڻپ = curwin->w_cursor.lnum - curwin->w_botline
								 + 1 + *so_ptr;
		إذا (سٽ_ڳڻپ <= curwin->w_اوچائي + 1)
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    || curwin->w_topfill != old_topfill
#جيڪڏهن_پورو
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
    أرجع (curwin->w_اوچائي * -p_sj) / 100;
}

/*
 * Return TRUE when there are not 'scrolloff' lines above the cursor for the
 * current window.
 */
    ساكن صحيح
check_چوٽي_وٿي(فراغ)
{
    lineoff_T	loff;
    صحيح		n;
    ڊگهو	so = get_scrolloff_قدر();

    إذا (curwin->w_cursor.lnum < curwin->w_topline + so
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		    || hasAnyFolding(curwin)
#جيڪڏهن_پورو
	    )
    {
	loff.lnum = curwin->w_cursor.lnum;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	loff.fill = 0;
	n = curwin->w_topfill;	    // always have this context
#وإلا
	n = 0;
#جيڪڏهن_پورو
	// Count the visible screen lines above the cursor line.
	طالما (n < so)
	{
	    topline_پٺ(&loff);
	    // Stop when included a line above the window.
	    إذا (loff.lnum < curwin->w_topline
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		    || (loff.lnum == curwin->w_topline && loff.fill > 0)
#جيڪڏهن_پورو
		    )
		اكسر;
	    n += loff.اوچائي;
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
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	- curwin->w_virtcol_پهريون_char
#جيڪڏهن_پورو
	;
    curwin->w_سيٽ_curswant = false;
}

/*
 * Update w_curswant if w_set_curswant is set.
 */
    فراغ
update_curswant(فراغ)
{
    إذا (curwin->w_سيٽ_curswant)
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
    وإلا إذا (wp->w_cursor.کانو != wp->w_valid_cursor.کانو
	     || wp->w_leftcol != wp->w_valid_leftcol
	     || wp->w_cursor.coladd != wp->w_valid_cursor.coladd)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
	wp->w_valid_cursor.کانو = wp->w_cursor.کانو;
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
بدليو_دري_setting(فراغ)
{
    بدليو_دري_setting_win(curwin);
}

    فراغ
بدليو_دري_setting_win(win_T *wp)
{
    wp->w_سٽون_valid = 0;
    بدليو_سٽ_abv_curs_win(wp);
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP|VALID_TOPLINE);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Call changed_window_setting_win() for every window containing "buf".
 */
    فراغ
بدليو_دري_setting_buf(buf_T *buf)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	إذا (wp->w_بفر == buf)
	    بدليو_دري_setting_win(wp);
}
#جيڪڏهن_پورو

/*
 * Call changed_window_setting_win() for every window.
 */
    فراغ
بدليو_دري_setting_all(فراغ)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	بدليو_دري_setting_win(wp);
}

/*
 * Set wp->w_topline to a certain number.
 */
    فراغ
سيٽ_topline(win_T *wp, linenr_T lnum)
{
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    linenr_T prev_topline = wp->w_topline;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    // go to first of folded lines
    (فراغ)hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
#جيڪڏهن_پورو
    // Approximate the value of w_botline
    wp->w_botline += lnum - wp->w_topline;
    إذا (wp->w_botline > wp->w_بفر->b_ml.ml_سٽ_ڳڻپ + 1)
	wp->w_botline = wp->w_بفر->b_ml.ml_سٽ_ڳڻپ + 1;
    wp->w_topline = lnum;
    wp->w_topline_was_سيٽ = true;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    إذا (lnum != prev_topline)
	// Keep the filler lines when the topline didn't change.
	wp->w_topfill = 0;
#جيڪڏهن_پورو
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
بدليو_cline_bef_curs(فراغ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    فراغ
بدليو_cline_bef_curs_win(win_T *wp)
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
بدليو_سٽ_abv_curs(فراغ)
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    فراغ
بدليو_سٽ_abv_curs_win(win_T *wp)
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
بدليو_سٽ_display_buf(buf_T *buf)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_بفر == buf)
	    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				|VALID_CROW|VALID_CHEIGHT
				|VALID_TOPLINE|VALID_BOTLINE|VALID_BOTLINE_AP);
}
#جيڪڏهن_پورو

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
	curs_صفون(curwin);
}
#جيڪڏهن_پورو

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topline.
 */
    ساكن فراغ
curs_صفون(win_T *wp)
{
    linenr_T	lnum;
    صحيح		i;
    صحيح		all_invalid;
    صحيح		valid;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    ڊگهو	fold_ڳڻپ;
#جيڪڏهن_پورو

    // Check if wp->w_lines[].wl_size is invalid
    all_invalid = (!redrawing()
			|| wp->w_سٽون_valid == 0
			|| wp->w_سٽون[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_صف = 0;
    لكل (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	إذا (!all_invalid && i < wp->w_سٽون_valid)
	{
	    إذا (wp->w_سٽون[i].wl_lnum < lnum || !wp->w_سٽون[i].wl_valid)
		استمر;		// skip changed or deleted lines
	    إذا (wp->w_سٽون[i].wl_lnum == lnum)
	    {
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		// Check for newly inserted lines below this row, in which
		// case we need to check for folded lines.
		إذا (!wp->w_بفر->b_mod_سيٽ
			|| wp->w_سٽون[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_بفر->b_mod_چوٽي
					     > wp->w_سٽون[i].wl_lastlnum + 1)
#جيڪڏهن_پورو
		valid = TRUE;
	    }
	    وإلا إذا (wp->w_سٽون[i].wl_lnum > lnum)
		--i;			// hold at inserted lines
	}
	إذا (valid
	       && (lnum != wp->w_topline || (wp->w_skipcol == 0
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
					    && !wp->w_p_diff
#جيڪڏهن_پورو
	       )))
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    lnum = wp->w_سٽون[i].wl_lastlnum + 1;
	    // Cursor inside folded lines, don't count this row
	    إذا (lnum > wp->w_cursor.lnum)
		اكسر;
#وإلا
	    ++lnum;
#جيڪڏهن_پورو
	    wp->w_cline_صف += wp->w_سٽون[i].wl_سائيز;
	}
	وإلا
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    fold_ڳڻپ = folded_ڳڻپ(wp, lnum, عدم);
	    إذا (fold_ڳڻپ)
	    {
		lnum += fold_ڳڻپ;
		إذا (lnum > wp->w_cursor.lnum)
		    اكسر;
		++wp->w_cline_صف;
	    }
	    وإلا
#جيڪڏهن_پورو
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
		|| i == wp->w_سٽون_valid
		|| (i < wp->w_سٽون_valid
		    && (!wp->w_سٽون[i].wl_valid
			|| wp->w_سٽون[i].wl_lnum != wp->w_cursor.lnum)))
	{
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    إذا (wp->w_cursor.lnum == wp->w_topline)
		wp->w_cline_اوچائي = plines_win_nofill(wp, wp->w_cursor.lnum,
							TRUE) + wp->w_topfill;
	    وإلا
#جيڪڏهن_پورو
		wp->w_cline_اوچائي = plines_win(wp, wp->w_cursor.lnum, TRUE);
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      عدم, عدم, TRUE, عدم);
#جيڪڏهن_پورو
	}
	وإلا إذا (i > wp->w_سٽون_valid)
	{
	    // a line that is too long to fit on the last screen line
	    wp->w_cline_اوچائي = 0;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      عدم, عدم, TRUE, عدم);
#جيڪڏهن_پورو
	}
	وإلا
	{
	    wp->w_cline_اوچائي = wp->w_سٽون[i].wl_سائيز;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    wp->w_cline_folded = wp->w_سٽون[i].wl_folded;
#جيڪڏهن_پورو
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

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    wp->w_virtcol_پهريون_char = 0;
#جيڪڏهن_پورو
    getvvcol(wp, &wp->w_cursor, عدم, &(wp->w_virtcol), عدم,
							 GETVCOL_FOR_VIRTCOL);
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    redraw_for_cursorcolumn(wp);
#جيڪڏهن_پورو
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

#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    إذا (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_اوچائي = plines_nofill(curwin->w_cursor.lnum)
	    + curwin->w_topfill;
    وإلا
#جيڪڏهن_پورو
	curwin->w_cline_اوچائي = plines(curwin->w_cursor.lnum);
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, عدم, عدم);
#جيڪڏهن_پورو
    curwin->w_valid |= VALID_CHEIGHT;
}

/*
 * Validate w_wcol and w_virtcol only.
 */
    فراغ
validate_cursor_کانو(فراغ)
{
    colnr_T off;
    colnr_T کانو;
    صحيح     ويڪر;

    validate_virtcol();

    إذا (curwin->w_valid & VALID_WCOL)
	أرجع;

    کانو = curwin->w_virtcol;
    off = curwin_کانو_off();
    کانو += off;
    ويڪر = curwin->w_ويڪر - off + curwin_کانو_off2();

    // long line wrapping, adjust curwin->w_wrow
    إذا (curwin->w_p_wrap
	    && کانو >= (colnr_T)curwin->w_ويڪر
	    && ويڪر > 0)
	// use same formula as what is used in curs_columns()
	کانو -= ((کانو - curwin->w_ويڪر) / ويڪر + 1) * ويڪر;
    إذا (کانو > (صحيح)curwin->w_leftcol)
	کانو -= curwin->w_leftcol;
    وإلا
	کانو = 0;
    curwin->w_wcol = کانو;

    curwin->w_valid |= VALID_WCOL;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    curwin->w_flags &= ~WFLAG_WCOL_OFF_ADDED;
#جيڪڏهن_پورو
}

/*
 * Compute offset of a window, occupied by absolute or relative line number,
 * fold column and sign column (these don't move when scrolling horizontally).
 */
    صحيح
win_کانو_off(win_T *wp)
{
    أرجع (((wp->w_p_nu || wp->w_p_rnu) ? عدد_ويڪر(wp) + 1 : 0)
	    + (wp != cmdwin_win ? 0 : 1)
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    + wp->w_p_fdc
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
	    + (signcolumn_on(wp) ? 2 : 0)
#جيڪڏهن_پورو
	   );
}

    صحيح
curwin_کانو_off(فراغ)
{
    أرجع win_کانو_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's positive if 'number' or 'relativenumber' is on and 'n'
 * is in 'cpoptions'.
 */
    صحيح
win_کانو_off2(win_T *wp)
{
    إذا ((wp->w_p_nu || wp->w_p_rnu) && vim_strchr(p_cpo, CPO_NUMCOL) != عدم)
	أرجع عدد_ويڪر(wp) + 1;
    أرجع 0;
}

    صحيح
curwin_کانو_off2(فراغ)
{
    أرجع win_کانو_off2(curwin);
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
    ڊگهو	diff;
    صحيح		extra;		// offset for first screen line
    ڊگهو	off_کاٻو, off_ساڄو;
    صحيح		n;
    صحيح		p_سٽون;
    صحيح		width1;		// text width for first screen line
    صحيح		width2 = 0;	// text width for second and later screen line
    صحيح		new_leftcol;
    colnr_T	startcol;
    colnr_T	endcol;
    colnr_T	prev_skipcol;
    ڊگهو	so = get_scrolloff_قدر();
    ڊگهو	siso = get_sidescrolloff_قدر();
    صحيح		did_sub_skipcol = FALSE;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    إذا (!(curwin->w_valid & VALID_CROW))
	curs_صفون(curwin);

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // will be set by getvvcol() but not reset
    curwin->w_virtcol_پهريون_char = 0;
#جيڪڏهن_پورو

    /*
     * Compute the number of virtual columns.
     */
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    إذا (curwin->w_cline_folded)
	// In a folded line the cursor is always in the first column
	startcol = curwin->w_virtcol = endcol = curwin->w_leftcol;
    وإلا
#جيڪڏهن_پورو
	getvvcol(curwin, &curwin->w_cursor, &startcol,
			 &(curwin->w_virtcol), &endcol, GETVCOL_FOR_VIRTCOL);

    // remove '$' from change command when cursor moves onto it
    إذا (startcol > dollar_vcol)
	dollar_vcol = -1;

    extra = curwin_کانو_off();
    curwin->w_wcol = curwin->w_virtcol + extra;
    endcol += extra;

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_صف;

    width1 = curwin->w_ويڪر - extra;
    إذا (width1 <= 0)
    {
	// No room for text, put cursor in last char of window.
	// If not wrapping, the last non-empty line.
	curwin->w_wcol = curwin->w_ويڪر - 1;
	إذا (curwin->w_p_wrap)
	    curwin->w_wrow = curwin->w_اوچائي - 1;
	وإلا
	    curwin->w_wrow = curwin->w_اوچائي - 1 - curwin->w_خالي_جايون_صفون;
    }
    وإلا إذا (curwin->w_p_wrap && curwin->w_ويڪر != 0)
    {
	width2 = width1 + curwin_کانو_off2();

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
	إذا (curwin->w_wcol >= curwin->w_ويڪر)
	{
	    // this same formula is used in validate_cursor_col()
	    n = (curwin->w_wcol - curwin->w_ويڪر) / width2 + 1;
	    curwin->w_wcol -= n * width2;
	    curwin->w_wrow += n;
	}
    }

    // No line wrapping: compute curwin->w_leftcol if scrolling is on and line
    // is not folded.
    // If scrolling is off, curwin->w_leftcol is assumed to be 0
    وإلا إذا (may_scroll
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    && !curwin->w_cline_folded
#جيڪڏهن_پورو
	    )
    {
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	إذا (curwin->w_virtcol_پهريون_char > 0)
	{
	    صحيح کانن_ڳڻپ = (curwin->w_ويڪر - extra);
	    صحيح صفون_2 = کانن_ڳڻپ > 0 ? curwin->w_virtcol_پهريون_char / کانن_ڳڻپ : 1;

	    // each "above" text prop shifts the text one row down
	    curwin->w_wrow += صفون_2;
	    curwin->w_wcol -= صفون_2 * کانن_ڳڻپ;
	    endcol -= صفون_2 * کانن_ڳڻپ;
	    curwin->w_cline_اوچائي = صفون_2 + 1;
	}
#جيڪڏهن_پورو
	/*
	 * If Cursor is left of the screen, scroll rightwards.
	 * If Cursor is right of the screen, scroll leftwards.
	 * If we get closer to the edge than 'sidescrolloff', scroll a little
	 * extra.
	 */
	off_کاٻو = startcol - curwin->w_leftcol - siso;
	off_ساڄو = endcol - curwin->w_leftcol - (curwin->w_ويڪر - siso) + 1;
	إذا (off_کاٻو < 0 || off_ساڄو > 0)
	{
	    إذا (off_کاٻو < 0)
		diff = -off_کاٻو;
	    وإلا
		diff = off_ساڄو;

	    // When far off or not enough room on either side, put cursor in
	    // middle of window.
	    إذا (p_ss == 0 || diff >= width1 / 2 || off_ساڄو >= off_کاٻو)
		new_leftcol = curwin->w_wcol - extra - width1 / 2;
	    وإلا
	    {
		إذا (diff < p_ss)
		    diff = p_ss;
		إذا (off_کاٻو < 0)
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

#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    // Skip over filler lines.  At the top use w_topfill, there
    // may be some filler lines above the window.
    إذا (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_wrow += curwin->w_topfill;
    وإلا
	curwin->w_wrow += diff_check_fill(curwin, curwin->w_cursor.lnum);
#جيڪڏهن_پورو

    prev_skipcol = curwin->w_skipcol;

    p_سٽون = 0;

    إذا ((curwin->w_wrow >= curwin->w_اوچائي
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + so >= curwin->w_اوچائي)
		    && (p_سٽون =
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
			plines_win_nofill
#وإلا
			plines_win
#جيڪڏهن_پورو
			(curwin, curwin->w_cursor.lnum, FALSE))
						    - 1 >= curwin->w_اوچائي))
	    && curwin->w_اوچائي != 0
	    && curwin->w_cursor.lnum == curwin->w_topline
	    && width2 > 0
	    && curwin->w_ويڪر != 0)
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
	إذا (p_سٽون == 0)
	    p_سٽون = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--p_سٽون;
	إذا (p_سٽون > curwin->w_wrow + so)
	    n = curwin->w_wrow + so;
	وإلا
	    n = p_سٽون;
	إذا ((colnr_T)n >= curwin->w_اوچائي + curwin->w_skipcol / width2 - so)
	    extra += 2;

	إذا (extra == 3 || curwin->w_اوچائي <= so * 2)
	{
	    // not enough room for 'scrolloff', put cursor in the middle
	    n = curwin->w_virtcol / width2;
	    إذا (n > curwin->w_اوچائي / 2)
		n -= curwin->w_اوچائي / 2;
	    وإلا
		n = 0;
	    // don't skip more than necessary
	    إذا (n > p_سٽون - curwin->w_اوچائي + 1)
		n = p_سٽون - curwin->w_اوچائي + 1;
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
	    endcol = (n - curwin->w_اوچائي + 1) * width2;
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

	إذا (curwin->w_wrow >= curwin->w_اوچائي)
	{
	    // small window, make sure cursor is in it
	    extra = curwin->w_wrow - curwin->w_اوچائي + 1;
	    curwin->w_skipcol += extra * width2;
	    curwin->w_wrow -= extra;
	}

	extra = ((صحيح)prev_skipcol - (صحيح)curwin->w_skipcol) / width2;
	إذا (extra > 0)
	    win_ins_سٽون(curwin, 0, extra, FALSE, FALSE);
	وإلا إذا (extra < 0)
	    win_del_سٽون(curwin, 0, -extra, FALSE, FALSE, 0);
    }
    وإلا إذا (!curwin->w_p_sms)
	curwin->w_skipcol = 0;
    إذا (prev_skipcol != curwin->w_skipcol)
	redraw_later(UPD_SOME_VALID);

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    redraw_for_cursorcolumn(curwin);
#جيڪڏهن_پورو
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    إذا (popup_is_popup(curwin) && curbuf->b_رڪن != عدم)
    {
	curwin->w_wrow += popup_چوٽي_extra(curwin);
	curwin->w_wcol += popup_کاٻو_extra(curwin);
	curwin->w_flags |= WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED;
    }
    وإلا
	curwin->w_flags &= ~(WFLAG_WCOL_OFF_ADDED + WFLAG_WROW_OFF_ADDED);
#جيڪڏهن_پورو

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
	colnr_T	    کانو;
	صحيح	    ويڪر;
	linenr_T    lnum = pos->lnum;
# ifdef FEAT_FOLDING
	صحيح	    is_folded;

	is_folded = hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
# endif
	صف = plines_m_win(wp, wp->w_topline, lnum - 1, وڌ_کان_وڌ_عدد);
	// "row" should be the screen line where line "lnum" begins, which can
	// be negative if "lnum" is "w_topline" and "w_skipcol" is non-zero.
	صف -= adjust_plines_for_skipcol(wp);

# ifdef FEAT_DIFF
	// Add filler lines above this buffer line.
	صف += lnum == wp->w_topline ? wp->w_topfill
				     : diff_check_fill(wp, lnum);
# endif

	colnr_T	off = win_کانو_off(wp);
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
	    کانو = scol;
	    کانو += off;
	    ويڪر = wp->w_ويڪر - off + win_کانو_off2(wp);

	    // long line wrapping, adjust row
	    إذا (wp->w_p_wrap
		    && کانو >= (colnr_T)wp->w_ويڪر
		    && ويڪر > 0)
	    {
		// use same formula as what is used in curs_columns()
		صحيح rowoff = ((کانو - wp->w_ويڪر) / ويڪر + 1);
		کانو -= rowoff * ويڪر;
		صف += rowoff;
	    }
	    کانو -= wp->w_leftcol;
	    إذا (کانو >= wp->w_ويڪر)
		کانو = -1;
	    إذا (کانو >= 0 && صف >= 0 && صف < wp->w_اوچائي)
	    {
		coloff = کانو - scol + wp->w_wincol + 1;
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

	*ccolp = wp->w_wincol + wp->w_ويڪر - endoff;
    }
# endif
}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
/*
 * "screenpos({winid}, {lnum}, {col})" function
 */
    فراغ
f_screenpos(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    dict_T	*dict;
    win_T	*wp;
    pos_T	pos;
    صحيح		صف = 0;
    صحيح		scol = 0, ccol = 0, ecol = 0;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    dict = rettv->vval.v_dict;

    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_عدد_arg(argvars, 2) == FAIL))
	أرجع;

    wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	أرجع;

    pos.lnum = tv_get_عدد(&argvars[1]);
    إذا (pos.lnum > wp->w_بفر->b_ml.ml_سٽ_ڳڻپ)
    {
	semsg(_(e_invalid_سٽ_عدد_nr), pos.lnum);
	أرجع;
    }
    pos.کانو = tv_get_عدد(&argvars[2]) - 1;
    إذا (pos.کانو < 0)
	pos.کانو = 0;
    pos.coladd = 0;
    textpos2screenpos(wp, &pos, &صف, &scol, &ccol, &ecol);

    dict_جوڙيو_عدد(dict, "row", صف);
    dict_جوڙيو_عدد(dict, "col", scol);
    dict_جوڙيو_عدد(dict, "curscol", ccol);
    dict_جوڙيو_عدد(dict, "endcol", ecol);
}

/*
 * Convert a virtual (screen) column to a character column.  The first column
 * is one.  For a multibyte character, the column number of the first byte is
 * returned.
 */
    ساكن صحيح
virtcol2col(win_T *wp, linenr_T lnum, صحيح vcol)
{
    صحيح		وٿي = vcol2col(wp, lnum, vcol - 1, عدم);
    char_u	*سٽ = ml_get_buf(wp->w_بفر, lnum, FALSE);
    char_u	*p = سٽ + وٿي;

    إذا (*p == NUL)
    {
	إذا (p == سٽ)  // empty line
	    أرجع 0;
	// Move to the first byte of the last char.
	MB_PTR_پٺ(سٽ, p);
    }
    أرجع (صحيح)(p - سٽ + 1);
}

/*
 * "virtcol2col({winid}, {lnum}, {col})" function
 */
    فراغ
f_virtcol2col(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    win_T	*wp;
    linenr_T	lnum;
    صحيح		screencol;
    صحيح		error = FALSE;

    rettv->vval.v_عدد = -1;

    إذا (check_for_عدد_arg(argvars, 0) == FAIL
	    || check_for_عدد_arg(argvars, 1) == FAIL
	    || check_for_عدد_arg(argvars, 2) == FAIL)
	أرجع;

    wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	أرجع;

    lnum = tv_get_عدد_chk(&argvars[1], &error);
    إذا (error || lnum < 0 || lnum > wp->w_بفر->b_ml.ml_سٽ_ڳڻپ)
	أرجع;

    screencol = tv_get_عدد_chk(&argvars[2], &error);
    إذا (error || screencol < 0)
	أرجع;

    rettv->vval.v_عدد = virtcol2col(wp, lnum, screencol);
}
#جيڪڏهن_پورو

/*
 * Make sure the cursor is in the visible part of the topline after scrolling
 * the screen with 'smoothscroll'.
 */
ساكن فراغ cursor_correct_sms(فراغ)
{
    إذا (!curwin->w_p_sms || !curwin->w_p_wrap
	|| curwin->w_cursor.lnum != curwin->w_topline)
	أرجع;

    ڊگهو    so = get_scrolloff_قدر();
    صحيح	    width1 = curwin->w_ويڪر - curwin_کانو_off();
    صحيح	    width2 = width1 + curwin_کانو_off2();
    صحيح	    so_کانن_ڳڻپ = so == 0 ? 0 : width1 + (so - 1) * width2;
    صحيح	    space_کانن_ڳڻپ = (curwin->w_اوچائي - 1) * width2;
    صحيح	    overlap, چوٽي, bot;
    صحيح	    سائيز = so == 0 ? 0 : linetabsize_eol(curwin, curwin->w_topline);

    إذا (curwin->w_topline == 1 && curwin->w_skipcol == 0)
	so_کانن_ڳڻپ = 0;               // Ignore 'scrolloff' at top of buffer.
    وإلا إذا (so_کانن_ڳڻپ > space_کانن_ڳڻپ / 2)
	so_کانن_ڳڻپ = space_کانن_ڳڻپ / 2;  // Not enough room: put cursor in the middle.

    // Not enough screen lines in topline: ignore 'scrolloff'.
    طالما (so_کانن_ڳڻپ > سائيز && so_کانن_ڳڻپ - width2 >= width1 && width1 > 0)
	so_کانن_ڳڻپ -= width2;
    إذا (so_کانن_ڳڻپ >= width1 && so_کانن_ڳڻپ > سائيز)
	so_کانن_ڳڻپ -= width1;

    overlap = curwin->w_skipcol == 0 ? 0
			: sms_marker_overlap(curwin, curwin->w_ويڪر - width2);
    // If we have non-zero scrolloff, ignore marker overlap.
    چوٽي = curwin->w_skipcol + (so_کانن_ڳڻپ != 0 ? so_کانن_ڳڻپ : overlap);
    bot = curwin->w_skipcol + width1 + (curwin->w_اوچائي - 1) * width2
								    - so_کانن_ڳڻپ;
    validate_virtcol();
    colnr_T کانو = curwin->w_virtcol;

    إذا (کانو < چوٽي)
    {
	إذا (کانو < width1)
	    کانو += width1;
	طالما (width2 > 0 && کانو < چوٽي)
	    کانو += width2;
    }
    وإلا
	طالما (width2 > 0 && کانو >= bot)
	    کانو -= width2;

    إذا (کانو != curwin->w_virtcol)
    {
	صحيح rc;

	curwin->w_curswant = کانو;
	rc = coladvance(curwin->w_curswant);
	// validate_virtcol() marked various things as valid, but after
	// moving the cursor they need to be recomputed
	curwin->w_valid &=
	    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	إذا (rc == FAIL && curwin->w_skipcol > 0
		&& curwin->w_cursor.lnum < curbuf->b_ml.ml_سٽ_ڳڻپ)
	{
	    validate_virtcol();
	    إذا (curwin->w_virtcol < curwin->w_skipcol + overlap)
	    {
		// Cursor still not visible: move it to the next line instead.
		curwin->w_cursor.lnum++;
		curwin->w_cursor.کانو = 0;
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
scroll_redraw(صحيح مٿي, ڊگهو ڳڻپ)
{
    linenr_T	prev_topline = curwin->w_topline;
    صحيح		prev_skipcol = curwin->w_skipcol;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		prev_topfill = curwin->w_topfill;
#جيڪڏهن_پورو
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    إذا (مٿي)
	scrollup(ڳڻپ, TRUE);
    وإلا
	scrolldown(ڳڻپ, TRUE);
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#جيڪڏهن_پورو
		)
	{
	    إذا (مٿي)
	    {
		إذا (curwin->w_cursor.lnum > prev_lnum
			|| cursor_هيٺ_ڏانهن(1L, FALSE) == FAIL)
		    اكسر;
	    }
	    وإلا
	    {
		إذا (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_مٿي(1L, FALSE) == FAIL)
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
    ڊگهو	سٽ_ڳڻپ,
    صحيح		byfold اڻ_استعمال_ٿيل)	// TRUE: count a closed fold as one line
{
    ڊگهو	پورو = 0;	// total # of physical lines done
    صحيح		wrow;
    صحيح		moved = FALSE;
    صحيح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    صحيح		width1 = 0;
    صحيح		width2 = 0;

    إذا (do_sms)
    {
	width1 = curwin->w_ويڪر - curwin_کانو_off();
	width2 = width1 + curwin_کانو_off2();
    }

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    linenr_T	پهريون;

    // Make sure w_topline is at the first of a sequence of folded lines.
    (فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#جيڪڏهن_پورو
    validate_cursor();		// w_wrow needs to be valid
    لكل (صحيح todo = سٽ_ڳڻپ; todo > 0; --todo)
    {
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	إذا (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline)
		&& curwin->w_topfill < curwin->w_اوچائي - 1)
	{
	    ++curwin->w_topfill;
	    ++پورو;
	}
	وإلا
#جيڪڏهن_پورو
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
		++پورو;
	    }
	    وإلا
	    {
		// scroll a text line down
		--curwin->w_topline;
		curwin->w_skipcol = 0;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		curwin->w_topfill = 0;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		// A sequence of folded lines only counts for one logical line
		إذا (hasFolding(curwin->w_topline, &پهريون, عدم))
		{
		    ++پورو;
		    إذا (!byfold)
			todo -= curwin->w_topline - پهريون - 1;
		    curwin->w_botline -= curwin->w_topline - پهريون;
		    curwin->w_topline = پهريون;
		}
		وإلا
#جيڪڏهن_پورو
		إذا (do_sms)
		{
		    صحيح سائيز = linetabsize_eol(curwin, curwin->w_topline);
		    إذا (سائيز > width1)
		    {
			curwin->w_skipcol = width1;
			سائيز -= width1;
			redraw_later(UPD_NOT_VALID);
		    }
		    طالما (سائيز > width2)
		    {
			curwin->w_skipcol += width2;
			سائيز -= width2;
		    }
		    ++پورو;
		}
		وإلا
		    پورو += PLINES_NOFILL(curwin->w_topline);
	    }
	}
	--curwin->w_botline;		// approximate w_botline
	invalidate_botline();
    }
    curwin->w_wrow += پورو;		// keep w_wrow updated
    curwin->w_cline_صف += پورو;	// keep w_cline_row updated

#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    إذا (curwin->w_cursor.lnum == curwin->w_topline)
	curwin->w_cline_صف = 0;
    check_topfill(curwin, TRUE);
#جيڪڏهن_پورو

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    إذا (curwin->w_p_wrap && curwin->w_ويڪر != 0)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_اوچائي - 1 -
	    curwin->w_virtcol / curwin->w_ويڪر;
    }
    طالما (wrow >= curwin->w_اوچائي && curwin->w_cursor.lnum > 1)
    {
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	إذا (hasFolding(curwin->w_cursor.lnum, &پهريون, عدم))
	{
	    --wrow;
	    إذا (پهريون == 1)
		curwin->w_cursor.lnum = 1;
	    وإلا
		curwin->w_cursor.lnum = پهريون - 1;
	}
	وإلا
#جيڪڏهن_پورو
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    إذا (moved)
    {
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	// Move cursor to first line of closed fold.
	foldAdjustCursor();
#جيڪڏهن_پورو
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
    ڊگهو	سٽ_ڳڻپ,
    صحيح		byfold اڻ_استعمال_ٿيل)	// TRUE: count a closed fold as one line
{
    صحيح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    إذا (do_sms
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    || (byfold && hasAnyFolding(curwin))
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    || (curwin->w_p_diff && !curwin->w_p_wrap)
#جيڪڏهن_پورو
       )
    {
	صحيح	    width1 = curwin->w_ويڪر - curwin_کانو_off();
	صحيح	    width2 = width1 + curwin_کانو_off2();
	صحيح	    سائيز = 0;
	colnr_T	    prev_skipcol = curwin->w_skipcol;

	إذا (do_sms)
	    سائيز = linetabsize_eol(curwin, curwin->w_topline);

	// diff mode: first consume "topfill"
	// 'smoothscroll': increase "w_skipcol" until it goes over the end of
	// the line, then advance to the next line.
	// folding: count each sequence of folded lines as one logical line.
	لكل (صحيح todo = سٽ_ڳڻپ; todo > 0; --todo)
	{
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    إذا (curwin->w_topfill > 0)
		--curwin->w_topfill;
	    وإلا
#جيڪڏهن_پورو
	    {
		linenr_T lnum = curwin->w_topline;

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		إذا (byfold)
		    // for a closed fold: go to the last line in the fold
		    (فراغ)hasFolding(lnum, عدم, &lnum);
#جيڪڏهن_پورو
		إذا (lnum == curwin->w_topline && do_sms)
		{
		    // 'smoothscroll': increase "w_skipcol" until it goes over
		    // the end of the line, then advance to the next line.
		    صحيح جوڙيو = curwin->w_skipcol > 0 ? width2 : width1;
		    curwin->w_skipcol += جوڙيو;
		    إذا (curwin->w_skipcol >= سائيز)
		    {
			إذا (lnum == curbuf->b_ml.ml_سٽ_ڳڻپ)
			{
			    // at the last screen line, can't scroll further
			    curwin->w_skipcol -= جوڙيو;
			    اكسر;
			}
			++lnum;
		    }
		}
		وإلا
		{
		    إذا (lnum >= curbuf->b_ml.ml_سٽ_ڳڻپ)
			اكسر;
		    ++lnum;
		}

		إذا (lnum > curwin->w_topline)
		{
		    // approximate w_botline
		    curwin->w_botline += lnum - curwin->w_topline;
		    curwin->w_topline = lnum;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		    curwin->w_topfill = diff_check_fill(curwin, lnum);
#جيڪڏهن_پورو
		    curwin->w_skipcol = 0;
		    إذا (todo > 1 && do_sms)
			سائيز = linetabsize_eol(curwin, curwin->w_topline);
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
	curwin->w_topline += سٽ_ڳڻپ;
	curwin->w_botline += سٽ_ڳڻپ;	// approximate w_botline
    }

    إذا (curwin->w_topline > curbuf->b_ml.ml_سٽ_ڳڻپ)
	curwin->w_topline = curbuf->b_ml.ml_سٽ_ڳڻپ;
    إذا (curwin->w_botline > curbuf->b_ml.ml_سٽ_ڳڻپ + 1)
	curwin->w_botline = curbuf->b_ml.ml_سٽ_ڳڻپ + 1;

#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    check_topfill(curwin, FALSE);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    إذا (hasAnyFolding(curwin))
	// Make sure w_topline is at the first of a sequence of folded lines.
	(فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#جيڪڏهن_پورو

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

    صحيح	    width1 = curwin->w_ويڪر - curwin_کانو_off();
    إذا (width1 <= 0)
	أرجع;  // no text will be displayed

    صحيح	    width2 = width1 + curwin_کانو_off2();
    ڊگهو    so = get_scrolloff_قدر();
    صحيح	    scrolloff_کانن_ڳڻپ = so == 0 ? 0 : width1 + (so - 1) * width2;
    صحيح	    scrolled = FALSE;
    صحيح	    صف = 0;
    صحيح	    overlap, کانو;

    validate_cheight();
    إذا (curwin->w_cline_اوچائي == curwin->w_اوچائي
	    // w_cline_height may be capped at w_height, check there aren't
	    // actually more lines.
	    && plines_win(curwin, curwin->w_cursor.lnum, FALSE)
							   <= curwin->w_اوچائي)
    {
	// the line just fits in the window, don't scroll
	reset_skipcol();
	أرجع;
    }

    validate_virtcol();
    overlap = sms_marker_overlap(curwin, curwin->w_ويڪر - width2);
    طالما (curwin->w_skipcol > 0
	    && curwin->w_virtcol < curwin->w_skipcol + overlap + scrolloff_کانن_ڳڻپ)
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

    کانو = curwin->w_virtcol + scrolloff_کانن_ڳڻپ;

    // Avoid adjusting for 'scrolloff' beyond the text line height.
    إذا (scrolloff_کانن_ڳڻپ > 0)
    {
	صحيح سائيز = linetabsize_eol(curwin, curwin->w_topline);
	سائيز = width1 + width2 * ((سائيز - width1 + width2 - 1) / width2);
	طالما (کانو > سائيز)
	    کانو -= width2;
    }
    کانو -= curwin->w_skipcol;

    إذا (کانو >= width1)
    {
	کانو -= width1;
	++صف;
    }
    إذا (کانو > width2)
    {
	صف += کانو / width2;
	// col may no longer be used, but make
	// sure it is correct anyhow, just in case
	کانو = کانو % width2;
    }
    إذا (صف >= curwin->w_اوچائي)
    {
	إذا (curwin->w_skipcol == 0)
	{
	    curwin->w_skipcol += width1;
	    --صف;
	}
	إذا (صف >= curwin->w_اوچائي)
	    curwin->w_skipcol += (صف - curwin->w_اوچائي) * width2;
	redraw_later(UPD_NOT_VALID);
    }
}

#جيڪڏهن_وصف_ٿيل FEAT_DIFF
/*
 * Don't end up with too many filler lines in the window.
 */
    فراغ
check_topfill(
    win_T	*wp,
    صحيح		هيٺ_ڏانهن)	// when TRUE scroll down when not enough space
{
    صحيح		n;

    إذا (wp->w_topfill <= 0)
	أرجع;

    n = plines_win_nofill(wp, wp->w_topline, TRUE);
    إذا (wp->w_topfill + n > wp->w_اوچائي)
    {
	إذا (هيٺ_ڏانهن && wp->w_topline > 1)
	{
	    --wp->w_topline;
	    wp->w_topfill = 0;
	}
	وإلا
	{
	    wp->w_topfill = wp->w_اوچائي - n;
	    إذا (wp->w_topfill < 0)
		wp->w_topfill = 0;
	}
    }
}
#جيڪڏهن_پورو

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    فراغ
scrolldown_clamp(فراغ)
{
    صحيح		پڇاڙي_صف;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		can_fill = (curwin->w_topfill
				< diff_check_fill(curwin, curwin->w_topline));
#جيڪڏهن_پورو

    إذا (curwin->w_topline <= 1
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    && !can_fill
#جيڪڏهن_پورو
	    )
	أرجع;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    پڇاڙي_صف = curwin->w_wrow;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    إذا (can_fill)
	++پڇاڙي_صف;
    وإلا
	پڇاڙي_صف += plines_nofill(curwin->w_topline - 1);
#وإلا
    پڇاڙي_صف += plines(curwin->w_topline - 1);
#جيڪڏهن_پورو
    إذا (curwin->w_p_wrap && curwin->w_ويڪر != 0)
    {
	validate_cheight();
	validate_virtcol();
	پڇاڙي_صف += curwin->w_cline_اوچائي - 1 -
	    curwin->w_virtcol / curwin->w_ويڪر;
    }
    إذا (پڇاڙي_صف < curwin->w_اوچائي - get_scrolloff_قدر())
    {
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
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
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	(فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#جيڪڏهن_پورو
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
    صحيح	    شروعات_صف;

    إذا (curwin->w_topline == curbuf->b_ml.ml_سٽ_ڳڻپ
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    && curwin->w_topfill == 0
#جيڪڏهن_پورو
	    )
	أرجع;

    validate_cursor();	    // w_wrow needs to be valid

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    شروعات_صف = curwin->w_wrow - plines_nofill(curwin->w_topline)
							  - curwin->w_topfill;
#وإلا
    شروعات_صف = curwin->w_wrow - plines(curwin->w_topline);
#جيڪڏهن_پورو
    إذا (curwin->w_p_wrap && curwin->w_ويڪر != 0)
    {
	validate_virtcol();
	شروعات_صف -= curwin->w_virtcol / curwin->w_ويڪر;
    }
    إذا (شروعات_صف >= get_scrolloff_قدر())
    {
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	إذا (curwin->w_topfill > 0)
	    --curwin->w_topfill;
	وإلا
#جيڪڏهن_پورو
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    (فراغ)hasFolding(curwin->w_topline, عدم, &curwin->w_topline);
#جيڪڏهن_پورو
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
topline_پٺ_winheight(
    lineoff_T	*lp,
    صحيح		winheight)	// when TRUE limit to window height
{
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    إذا (lp->fill < diff_check_fill(curwin, lp->lnum))
    {
	// Add a filler line.
	++lp->fill;
	lp->اوچائي = 1;
    }
    وإلا
#جيڪڏهن_پورو
    {
	--lp->lnum;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	lp->fill = 0;
#جيڪڏهن_پورو
	إذا (lp->lnum < 1)
	    lp->اوچائي = MAXCOL;
	وإلا
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    إذا (hasFolding(lp->lnum, &lp->lnum, عدم))
	    // Add a closed fold
	    lp->اوچائي = 1;
	وإلا
#جيڪڏهن_پورو
	    lp->اوچائي = PLINES_WIN_NOFILL(curwin, lp->lnum, winheight);
    }
}

    ساكن فراغ
topline_پٺ(lineoff_T *lp)
{
    topline_پٺ_winheight(lp, TRUE);
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    إذا (lp->fill < diff_check_fill(curwin, lp->lnum + 1))
    {
	// Add a filler line.
	++lp->fill;
	lp->اوچائي = 1;
    }
    وإلا
#جيڪڏهن_پورو
    {
	++lp->lnum;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	lp->fill = 0;
#جيڪڏهن_پورو
	إذا (lp->lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
	    lp->اوچائي = MAXCOL;
	وإلا
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    إذا (hasFolding(lp->lnum, عدم, &lp->lnum))
	    {
		// Add a closed fold.
		lp->اوچائي = 1;
	    }
	    وإلا
#جيڪڏهن_پورو
	    lp->اوچائي = PLINES_NOFILL(lp->lnum);
    }
}

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    فراغ
scroll_cursor_چوٽي(صحيح min_scroll, صحيح always)
{
    صحيح		scrolled = 0;
    صحيح		extra = 0;
    صحيح		استعمال_ٿيل;
    صحيح		i;
    linenr_T	چوٽي;		// just above displayed lines
    linenr_T	bot;		// just below displayed lines
    linenr_T	old_topline = curwin->w_topline;
    صحيح		old_skipcol = curwin->w_skipcol;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    linenr_T	old_topfill = curwin->w_topfill;
#جيڪڏهن_پورو
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
    استعمال_ٿيل = curwin->w_cline_اوچائي; // includes filler lines above
    إذا (curwin->w_cursor.lnum < curwin->w_topline)
	scrolled = استعمال_ٿيل;

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    إذا (hasFolding(curwin->w_cursor.lnum, &چوٽي, &bot))
    {
	--چوٽي;
	++bot;
    }
    وإلا
#جيڪڏهن_پورو
    {
	چوٽي = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = چوٽي + 1;

#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    // "used" already contains the number of filler lines above, don't add it
    // again.
    // Hide filler lines above cursor line by adding them to "extra".
    extra += diff_check_fill(curwin, curwin->w_cursor.lnum);
#جيڪڏهن_پورو

    /*
     * Check if the lines from "top" to "bot" fit in the window.  If they do,
     * set new_topline and advance "top" and "bot" to include more lines.
     */
    طالما (چوٽي > 0)
    {
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	إذا (hasFolding(چوٽي, &چوٽي, عدم))
	    // count one logical line for a sequence of folded lines
	    i = 1;
	وإلا
#جيڪڏهن_پورو
	    i = PLINES_NOFILL(چوٽي);
	إذا (چوٽي < curwin->w_topline)
	    scrolled += i;

	// If scrolling is needed, scroll at least 'sj' lines.
	إذا ((new_topline >= curwin->w_topline || scrolled > min_scroll)
		&& extra >= off)
	    اكسر;

	استعمال_ٿيل += i;
	إذا (extra + i <= off && bot < curbuf->b_ml.ml_سٽ_ڳڻپ)
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    إذا (hasFolding(bot, عدم, &bot))
		// count one logical line for a sequence of folded lines
		++استعمال_ٿيل;
	    وإلا
#جيڪڏهن_پورو
		استعمال_ٿيل += plines(bot);
	}
	إذا (استعمال_ٿيل > curwin->w_اوچائي)
	    اكسر;

	extra += i;
	new_topline = چوٽي;
	--چوٽي;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    إذا (استعمال_ٿيل > curwin->w_اوچائي)
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	curwin->w_topfill = diff_check_fill(curwin, curwin->w_topline);
	إذا (curwin->w_topfill > 0 && extra > off)
	{
	    curwin->w_topfill -= extra - off;
	    إذا (curwin->w_topfill < 0)
		curwin->w_topfill = 0;
	}
	check_topfill(curwin, FALSE);
#جيڪڏهن_پورو
	إذا (curwin->w_topline != old_topline)
	    reset_skipcol();
	وإلا إذا (curwin->w_topline == curwin->w_cursor.lnum)
	{
	    validate_virtcol();
	    إذا (curwin->w_skipcol >= curwin->w_virtcol)
	    {
		// Skip up to the screen line the cursor is in, so that the
		// position in the line is kept.
		صحيح	width1 = curwin->w_ويڪر - curwin_کانو_off();
		صحيح	width2 = width1 + curwin_کانو_off2();
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#جيڪڏهن_پورو
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
سيٽ_خالي_جايون_صفون(win_T *wp, صحيح استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    wp->w_filler_صفون = 0;
#جيڪڏهن_پورو
    إذا (استعمال_ٿيل == 0)
	wp->w_خالي_جايون_صفون = 0;	// single line that doesn't fit
    وإلا
    {
	wp->w_خالي_جايون_صفون = wp->w_اوچائي - استعمال_ٿيل;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	إذا (wp->w_botline <= wp->w_بفر->b_ml.ml_سٽ_ڳڻپ)
	{
	    wp->w_filler_صفون = diff_check_fill(wp, wp->w_botline);
	    إذا (wp->w_خالي_جايون_صفون > wp->w_filler_صفون)
		wp->w_خالي_جايون_صفون -= wp->w_filler_صفون;
	    وإلا
	    {
		wp->w_filler_صفون = wp->w_خالي_جايون_صفون;
		wp->w_خالي_جايون_صفون = 0;
	    }
	}
#جيڪڏهن_پورو
    }
}

/*
 * Recompute topline to put the cursor at the bottom of the window.
 * When scrolling scroll at least "min_scroll" lines.
 * If "set_topbot" is TRUE, set topline and botline first (for "zb").
 * This is messy stuff!!!
 */
    فراغ
scroll_cursor_bot(صحيح min_scroll, صحيح سيٽ_topbot)
{
    صحيح		استعمال_ٿيل;
    صحيح		scrolled = 0;
    صحيح		extra = 0;
    صحيح		i;
    linenr_T	سٽ_ڳڻپ;
    linenr_T	old_topline = curwin->w_topline;
    صحيح		old_skipcol = curwin->w_skipcol;
    lineoff_T	loff;
    lineoff_T	boff;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		old_topfill = curwin->w_topfill;
    صحيح		fill_هيٺ_دري;
#جيڪڏهن_پورو
    linenr_T	old_botline = curwin->w_botline;
    linenr_T	old_valid = curwin->w_valid;
    صحيح		old_خالي_جايون_صفون = curwin->w_خالي_جايون_صفون;
    linenr_T	cln;		    // Cursor Line Number
    ڊگهو	so = get_scrolloff_قدر();
    صحيح		do_sms = curwin->w_p_wrap && curwin->w_p_sms;

    cln = curwin->w_cursor.lnum;
    إذا (سيٽ_topbot)
    {
	استعمال_ٿيل = 0;
	linenr_T cln_آخري = cln;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	(فراغ)hasFolding(cln, عدم, &cln_آخري);
#جيڪڏهن_پورو
	curwin->w_botline = cln_آخري + 1;
	loff.lnum = cln_آخري + 1;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	loff.fill = 0;
#جيڪڏهن_پورو
	طالما (TRUE)
	{
	    topline_پٺ_winheight(&loff, FALSE);
	    إذا (loff.اوچائي == MAXCOL)
		اكسر;
	    إذا (استعمال_ٿيل + loff.اوچائي > curwin->w_اوچائي)
	    {
		إذا (do_sms)
		{
		    // 'smoothscroll' and 'wrap' are set.  The above line is
		    // too long to show in its entirety, so we show just a part
		    // of it.
		    إذا (استعمال_ٿيل < curwin->w_اوچائي)
		    {
			صحيح plines_وٿي = استعمال_ٿيل + loff.اوچائي
							    - curwin->w_اوچائي;
			استعمال_ٿيل = curwin->w_اوچائي;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
			curwin->w_topfill = loff.fill;
#جيڪڏهن_پورو
			curwin->w_topline = loff.lnum;
			curwin->w_skipcol = skipcol_from_plines(
							curwin, plines_وٿي);
		    }
		}
		اكسر;
	    }
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    curwin->w_topfill = loff.fill;
#جيڪڏهن_پورو
	    curwin->w_topline = loff.lnum;
	    استعمال_ٿيل += loff.اوچائي;
	}

	سيٽ_خالي_جايون_صفون(curwin, استعمال_ٿيل);
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	إذا (curwin->w_topline != old_topline
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		|| curwin->w_topfill != old_topfill
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    استعمال_ٿيل = plines_nofill(cln);
#وإلا
    validate_cheight();
    استعمال_ٿيل = curwin->w_cline_اوچائي;
#جيڪڏهن_پورو

    إذا (do_sms && (dy_flags & DY_LASTLINE))
    {
	// The rest of the cursor line may be cut off at the bottom.
	صحيح upto_cursor = plines_win_کانو(curwin, cln, curwin->w_cursor.کانو);

	إذا (upto_cursor < استعمال_ٿيل)
	    استعمال_ٿيل = upto_cursor;
    }

    // If the cursor is on or below botline, we will at least scroll by the
    // height of the cursor line, which is "used".  Correct for empty lines,
    // which are really part of botline.
    إذا (cln >= curwin->w_botline)
    {
	scrolled = استعمال_ٿيل;
	إذا (cln == curwin->w_botline)
	    scrolled -= curwin->w_خالي_جايون_صفون;
	إذا (do_sms)
	{
	    // 'smoothscroll' and 'wrap' are set.
	    // Calculate how many screen lines the current top line of window
	    // occupies. If it is occupying more than the entire window, we
	    // need to scroll the additional clipped lines to scroll past the
	    // top line before we can move on to the other lines.
	    صحيح چوٽي_plines =
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
			    plines_win_nofill
#وإلا
			    plines_win
#جيڪڏهن_پورو
					(curwin, curwin->w_topline, FALSE);
	    صحيح width1 = curwin->w_ويڪر - curwin_کانو_off();

	    إذا (width1 > 0)
	    {
		صحيح width2 = width1 + curwin_کانو_off2();
		صحيح skip_سٽون = 0;

		// A similar formula is used in curs_columns().
		إذا (curwin->w_skipcol > width1)
		    skip_سٽون += (curwin->w_skipcol - width1) / width2 + 1;
		وإلا إذا (curwin->w_skipcol > 0)
		    skip_سٽون = 1;

		چوٽي_plines -= skip_سٽون;
		إذا (چوٽي_plines > curwin->w_اوچائي)
		{
		    scrolled += (چوٽي_plines - curwin->w_اوچائي);
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
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    إذا (!hasFolding(curwin->w_cursor.lnum, &loff.lnum, &boff.lnum))
#جيڪڏهن_پورو
    {
	loff.lnum = cln;
	boff.lnum = cln;
    }
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    loff.fill = 0;
    boff.fill = 0;
    fill_هيٺ_دري = diff_check_fill(curwin, curwin->w_botline)
						      - curwin->w_filler_صفون;
#جيڪڏهن_پورو

    طالما (loff.lnum > 1)
    {
	// Stop when scrolled nothing or at least "min_scroll", found "extra"
	// context for 'scrolloff' and counted all lines below the window.
	إذا ((((scrolled <= 0 || scrolled >= min_scroll)
		    && extra >= (mouse_dragging > 0 ? mouse_dragging - 1 : so))
		    || boff.lnum + 1 > curbuf->b_ml.ml_سٽ_ڳڻپ)
		&& loff.lnum <= curwin->w_botline
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		&& (loff.lnum < curwin->w_botline
		    || loff.fill >= fill_هيٺ_دري)
#جيڪڏهن_پورو
		)
	    اكسر;

	linenr_T loff_lnum_before = loff.lnum;
	// Add one line above
	topline_پٺ(&loff);
	إذا (loff.اوچائي == MAXCOL)
	    استعمال_ٿيل = MAXCOL;
	وإلا
	    استعمال_ٿيل += loff.اوچائي;
	إذا (استعمال_ٿيل > curwin->w_اوچائي)
	    اكسر;
	إذا (loff.lnum >= curwin->w_botline
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		&& (loff.lnum > curwin->w_botline
		    || loff.fill <= fill_هيٺ_دري)
#جيڪڏهن_پورو
		)
	{
	    // Count screen lines that are below the window.
	    scrolled += loff.اوچائي;
	    إذا (loff.lnum == curwin->w_botline
		    && loff_lnum_before > curwin->w_botline)
		scrolled -= curwin->w_خالي_جايون_صفون;
	}

	إذا (boff.lnum < curbuf->b_ml.ml_سٽ_ڳڻپ)
	{
	    linenr_T boff_lnum_before = boff.lnum;
	    // Add one line below
	    botline_forw(&boff);
	    استعمال_ٿيل += boff.اوچائي;
	    إذا (استعمال_ٿيل > curwin->w_اوچائي)
		اكسر;
	    إذا (extra < ( mouse_dragging > 0 ? mouse_dragging - 1 : so)
		    || scrolled < min_scroll)
	    {
		extra += boff.اوچائي;
		إذا (boff.lnum >= curwin->w_botline
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
			|| (boff.lnum + 1 == curwin->w_botline
			    && boff.fill > curwin->w_filler_صفون)
#جيڪڏهن_پورو
		   )
		{
		    // Count screen lines that are below the window.
		    scrolled += boff.اوچائي;
		    إذا (boff.lnum >= curwin->w_botline
			    && boff_lnum_before < curwin->w_botline)
			scrolled -= curwin->w_خالي_جايون_صفون;
		}
	    }
	}
    }

    // curwin->w_empty_rows is larger, no need to scroll
    إذا (scrolled <= 0)
	سٽ_ڳڻپ = 0;
    // more than a screenfull, don't scroll but redraw
    وإلا إذا (استعمال_ٿيل > curwin->w_اوچائي)
	سٽ_ڳڻپ = استعمال_ٿيل;
    // scroll minimal number of lines
    وإلا
    {
	سٽ_ڳڻپ = 0;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	boff.fill = curwin->w_topfill;
#جيڪڏهن_پورو
	boff.lnum = curwin->w_topline - 1;
	لكل (i = 0; i < scrolled && boff.lnum < curwin->w_botline; )
	{
	    botline_forw(&boff);
	    i += boff.اوچائي;
	    ++سٽ_ڳڻپ;
	}
	إذا (i < scrolled)	// below curwin->w_botline, don't scroll
	    سٽ_ڳڻپ = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    bool eof_pressure = scrolloffpad_eof_pressure(cln, so);
    إذا (سٽ_ڳڻپ >= curwin->w_اوچائي && سٽ_ڳڻپ > min_scroll)
	scroll_cursor_halfway(eof_pressure, TRUE);
    وإلا إذا (سٽ_ڳڻپ > 0)
    {
	إذا (do_sms)
	    scrollup(scrolled, TRUE);  // TODO
	وإلا
	    scrollup(سٽ_ڳڻپ, TRUE);
    }

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    إذا (curwin->w_topline == old_topline
	    && curwin->w_skipcol == old_skipcol
	    && سيٽ_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_خالي_جايون_صفون = old_خالي_جايون_صفون;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;

    // Make sure cursor is still visible after adjusting skipcol for "zb".
    إذا (سيٽ_topbot)
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		topfill = 0;
#جيڪڏهن_پورو
    صحيح		هيٺ = 0;
    صحيح		استعمال_ٿيل;
    lineoff_T	loff;
    lineoff_T	boff;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    linenr_T	old_topline = curwin->w_topline;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    // if the width changed this needs to be updated first
    may_update_popup_position();
#جيڪڏهن_پورو
    loff.lnum = boff.lnum = curwin->w_cursor.lnum;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    (فراغ)hasFolding(loff.lnum, &loff.lnum, &boff.lnum);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    استعمال_ٿيل = plines_nofill(loff.lnum);
    loff.fill = 0;
    boff.fill = 0;
#وإلا
    استعمال_ٿيل = plines(loff.lnum);
#جيڪڏهن_پورو
    topline = loff.lnum;

    صحيح want_اوچائي;
    صحيح do_sms = curwin->w_p_wrap && curwin->w_p_sms;
    إذا (do_sms)
    {
	// 'smoothscroll' and 'wrap' are set
	إذا (atend)
	{
	    want_اوچائي = (curwin->w_اوچائي - استعمال_ٿيل) / 2;
	    استعمال_ٿيل = 0;
	}
	وإلا
	    want_اوچائي = curwin->w_اوچائي;
    }

    طالما (topline > 1)
    {
	// If using smoothscroll, we can precisely scroll to the
	// exact point where the cursor is halfway down the screen.
	إذا (do_sms)
	{
	    topline_پٺ_winheight(&loff, FALSE);
	    إذا (loff.اوچائي == MAXCOL)
		اكسر;
	    استعمال_ٿيل += loff.اوچائي;
	    إذا (!atend && boff.lnum < curbuf->b_ml.ml_سٽ_ڳڻپ)
	    {
		botline_forw(&boff);
		استعمال_ٿيل += boff.اوچائي;
	    }
	    إذا (استعمال_ٿيل > want_اوچائي)
	    {
		إذا (استعمال_ٿيل - loff.اوچائي < want_اوچائي)
		{
		    topline = loff.lnum;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		    topfill = loff.fill;
#جيڪڏهن_پورو
		    skipcol = skipcol_from_plines(curwin, استعمال_ٿيل - want_اوچائي);
		}
		اكسر;
	    }
	    topline = loff.lnum;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    topfill = loff.fill;
#جيڪڏهن_پورو
	    استمر;
	}

	// If not using smoothscroll, we have to iteratively find how many
	// lines to scroll down to roughly fit the cursor.
	// This may not be right in the middle if the lines'
	// physical height > 1 (e.g. 'wrap' is on).

	// Depending on "prefer_above" we add a line above or below first.
	// Loop twice to avoid duplicating code.
	صحيح پورو = FALSE;
	لكل (صحيح ڦيرو = 1; ڦيرو <= 2; ++ڦيرو)
	{
	    إذا (prefer_above ? (ڦيرو == 2 && هيٺ < above)
			     : (ڦيرو == 1 && هيٺ <= above))
	    {
		// add a line below the cursor
		إذا (boff.lnum < curbuf->b_ml.ml_سٽ_ڳڻپ)
		{
		    botline_forw(&boff);
		    استعمال_ٿيل += boff.اوچائي;
		    إذا (استعمال_ٿيل > curwin->w_اوچائي)
		    {
			پورو = TRUE;
			اكسر;
		    }
		    هيٺ += boff.اوچائي;
		}
		وإلا
		{
		    ++هيٺ;	    // count a "~" line
		    إذا (atend)
			++استعمال_ٿيل;
		}
	    }

	    إذا (prefer_above ? (ڦيرو == 1 && هيٺ >= above)
			     : (ڦيرو == 1 && هيٺ > above))
	    {
		// add a line above the cursor
		topline_پٺ(&loff);
		إذا (loff.اوچائي == MAXCOL)
		    استعمال_ٿيل = MAXCOL;
		وإلا
		    استعمال_ٿيل += loff.اوچائي;
		إذا (استعمال_ٿيل > curwin->w_اوچائي)
		{
		    پورو = TRUE;
		    اكسر;
		}
		above += loff.اوچائي;
		topline = loff.lnum;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
		topfill = loff.fill;
#جيڪڏهن_پورو
	    }
	}
	إذا (پورو)
	    اكسر;
    }

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    إذا (!hasFolding(topline, &curwin->w_topline, عدم))
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    curwin->w_topfill = topfill;
    إذا (old_topline > curwin->w_topline + curwin->w_اوچائي)
	curwin->w_botfill = false;
    check_topfill(curwin, FALSE);
#جيڪڏهن_پورو
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
    صحيح		هيٺ = 0;	    // screen lines below botline
    linenr_T	botline;
    صحيح		above_wanted, هيٺ_wanted;
    linenr_T	cln;		    // Cursor Line Number
    صحيح		max_off;
    ڊگهو	so = get_scrolloff_قدر();

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = so;
    هيٺ_wanted = so;
    إذا (mouse_dragging > 0)
    {
	above_wanted = mouse_dragging - 1;
	هيٺ_wanted = mouse_dragging - 1;
    }
    إذا (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_اوچائي / 2;
	إذا (هيٺ_wanted > max_off)
	    هيٺ_wanted = max_off;
    }
    validate_botline();
    إذا (curwin->w_botline == curbuf->b_ml.ml_سٽ_ڳڻپ + 1
	    && mouse_dragging == 0)
    {
	// Missing context below EOF must not move the cursor up.  Automatic
	// scrolling handles the centering for 'scrolloffpad'.
	هيٺ_wanted = 0;
	max_off = (curwin->w_اوچائي - 1) / 2;
	إذا (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    إذا (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - هيٺ_wanted
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    // count filler lines as context
    above = curwin->w_topfill;
    هيٺ = curwin->w_filler_صفون;
#جيڪڏهن_پورو
    طالما ((above < above_wanted || هيٺ < هيٺ_wanted) && topline < botline)
    {
	إذا (هيٺ < هيٺ_wanted && (هيٺ <= above || above >= above_wanted))
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    إذا (hasFolding(botline, &botline, عدم))
		++هيٺ;
	    وإلا
#جيڪڏهن_پورو
		هيٺ += plines(botline);
	    --botline;
	}
	إذا (above < above_wanted && (above < هيٺ || هيٺ >= هيٺ_wanted))
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    إذا (hasFolding(topline, عدم, &topline))
		++above;
	    وإلا
#جيڪڏهن_پورو
		above += PLINES_NOFILL(topline);
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    // Count filler lines below this line as context.
	    إذا (topline < botline)
		above += diff_check_fill(curwin, topline + 1);
#جيڪڏهن_پورو
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
	إذا (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_سٽ_ڳڻپ)
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
    صحيح	    min_اوچائي = curwin->w_اوچائي - 2;

    validate_botline();
    إذا ((dir == BACKWARD && curwin->w_topline == 1)
	|| (dir == FORWARD && curwin->w_botline > curbuf->b_ml.ml_سٽ_ڳڻپ))
	أرجع min_اوچائي + 2;  // no overlap, still handle 'smoothscroll'

    loff.lnum = dir == FORWARD ? curwin->w_botline : curwin->w_topline - 1;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    loff.fill = diff_check_fill(curwin, loff.lnum + (dir == BACKWARD))
		- (dir == FORWARD ? curwin->w_filler_صفون : curwin->w_topfill);
    loff.اوچائي = loff.fill > 0 ? 1 : plines_nofill(loff.lnum);
#وإلا
    loff.اوچائي = plines(loff.lnum);
#جيڪڏهن_پورو

    صحيح h1 = loff.اوچائي;
    إذا (h1 > min_اوچائي)
	أرجع min_اوچائي + 2;  // no overlap
    إذا (dir == FORWARD)
	topline_پٺ(&loff);
    وإلا
	botline_forw(&loff);

    صحيح h2 = loff.اوچائي;
    إذا (h2 == MAXCOL || h2 + h1 > min_اوچائي)
	أرجع min_اوچائي + 2;  // no overlap
    إذا (dir == FORWARD)
	topline_پٺ(&loff);
    وإلا
	botline_forw(&loff);

    صحيح h3 = loff.اوچائي;
    إذا (h3 == MAXCOL || h3 + h2 > min_اوچائي)
	أرجع min_اوچائي + 2;  // no overlap
    إذا (dir == FORWARD)
	topline_پٺ(&loff);
    وإلا
	botline_forw(&loff);

    صحيح h4 = loff.اوچائي;
    إذا (h4 == MAXCOL || h4 + h3 + h2 > min_اوچائي || h3 + h2 + h1 > min_اوچائي)
	أرجع min_اوچائي + 1;  // 1 line overlap
    وإلا
	أرجع min_اوچائي;      // 2 lines overlap
}

/*
 * Scroll "count" lines with 'smoothscroll' in direction "dir". Return TRUE
 * when scrolling happened. Adjust "curscount" for scrolling different amount of
 * lines when 'smoothscroll' is disabled.
 */
ساكن صحيح scroll_with_sms(صحيح dir, ڊگهو ڳڻپ, ڊگهو *curscount)
{
    صحيح		prev_sms = curwin->w_p_sms;
    colnr_T	prev_skipcol = curwin->w_skipcol;
    linenr_T	prev_topline = curwin->w_topline;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		prev_topfill = curwin->w_topfill;
#جيڪڏهن_پورو

    curwin->w_p_sms = TRUE;
    scroll_redraw(dir == FORWARD, ڳڻپ);

    // Not actually smoothscrolling but ended up with partially visible line.
    // Continue scrolling until skipcol is zero.
    إذا (!prev_sms && curwin->w_skipcol > 0)
    {
	صحيح fixdir = dir;
	// Reverse the scroll direction when topline already changed. One line
	// extra for scrolling backward so that consuming skipcol is symmetric.
	إذا (labs(curwin->w_topline - prev_topline) > (dir == BACKWARD))
	    fixdir = dir * -1;

	صحيح width1 = curwin->w_ويڪر - curwin_کانو_off();
	صحيح width2 = width1 + curwin_کانو_off2();
	ڳڻپ = 1 + (curwin->w_skipcol - width1 - 1) / width2;
	إذا (fixdir == FORWARD)
	    ڳڻپ = 1 + (linetabsize_eol(curwin, curwin->w_topline)
			    - curwin->w_skipcol - width1 + width2 - 1) / width2;
	scroll_redraw(fixdir == FORWARD, ڳڻپ);
	*curscount += ڳڻپ * (fixdir == dir ? 1 : -1);
    }
    curwin->w_p_sms = prev_sms;

    أرجع curwin->w_topline != prev_topline
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	|| curwin->w_topfill != prev_topfill
#جيڪڏهن_پورو
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
pagescroll(صحيح dir, ڊگهو ڳڻپ, صحيح half)
{
    صحيح		did_move = FALSE;
    صحيح		buflen = curbuf->b_ml.ml_سٽ_ڳڻپ;
    colnr_T	prev_کانو = curwin->w_cursor.کانو;
    colnr_T	prev_curswant = curwin->w_curswant;
    linenr_T	prev_lnum = curwin->w_cursor.lnum;
    oparg_T	oa = { 0 };
    cmdarg_T	ca = { 0 };
    ca.oap = &oa;

    إذا (half)
    {
	// Scroll [count], 'scroll' or current window height lines.
	إذا (ڳڻپ)
	    curwin->w_p_scr = MIN(curwin->w_اوچائي, ڳڻپ);
	ڳڻپ = MIN(curwin->w_اوچائي, curwin->w_p_scr);

	ڊگهو curscount = ڳڻپ;
	// Adjust count so as to not reveal end of buffer lines.
	إذا (dir == FORWARD
		    && (curwin->w_topline + curwin->w_اوچائي + ڳڻپ > buflen
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
							|| hasAnyFolding(curwin)
#جيڪڏهن_پورو
	   ))
	{
	    صحيح n = plines_correct_topline(curwin, curwin->w_topline, FALSE);
	    إذا (n - ڳڻپ < curwin->w_اوچائي && curwin->w_topline < buflen)
		n += plines_m_win(curwin, curwin->w_topline + 1, buflen,
						    curwin->w_اوچائي + ڳڻپ);
	    إذا (n < curwin->w_اوچائي + ڳڻپ)
		ڳڻپ = n - curwin->w_اوچائي;
	}

	// (Try to) scroll the window unless already at the end of the buffer.
	إذا (ڳڻپ > 0)
	{
	    did_move = scroll_with_sms(dir, ڳڻپ, &curscount);
	    curwin->w_cursor.lnum = prev_lnum;
	    curwin->w_cursor.کانو = prev_کانو;
	    curwin->w_curswant = prev_curswant;
	}

	// Move the cursor the same amount of screen lines.
	إذا (curwin->w_p_wrap)
	    nv_screengo(&oa, dir, curscount);
	وإلا إذا (dir == FORWARD)
	    cursor_هيٺ_ڏانهن_inner(curwin, curscount);
	وإلا
	    cursor_مٿي_inner(curwin, curscount);
    }
    وإلا
    {
	// Scroll [count] times 'window' or current window height lines.
	ڳڻپ *= ((ONE_دري && p_دري > 0 && p_دري < صفون - 1) ?
				MAX(1, p_دري - 2) : get_scroll_overlap(dir));
	did_move = scroll_with_sms(dir, ڳڻپ, &ڳڻپ);

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
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    // Move cursor to first line of closed fold.
    foldAdjustCursor();
#جيڪڏهن_پورو
    did_move = did_move
	|| prev_کانو != curwin->w_cursor.کانو
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

    linenr_T	سٽ = curwin->w_cursor.lnum;
    colnr_T	کانو = curwin->w_cursor.کانو;
    colnr_T	coladd = curwin->w_cursor.coladd;
    colnr_T	curswant = curwin->w_curswant;
    صحيح		سيٽ_curswant = curwin->w_سيٽ_curswant;
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    إذا (curwin->w_p_diff)
		curwin->w_cursor.lnum =
				 diff_get_corresponding_سٽ(old_curbuf, سٽ);
	    وإلا
#جيڪڏهن_پورو
		curwin->w_cursor.lnum = سٽ;
	    curwin->w_cursor.کانو = کانو;
	    curwin->w_cursor.coladd = coladd;
	    curwin->w_curswant = curswant;
	    curwin->w_سيٽ_curswant = سيٽ_curswant;

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
