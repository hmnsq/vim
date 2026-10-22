/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * drawscreen.c: Code for updating all the windows on the screen.
 * This is the top level, drawline.c is the middle and screen.c the lower
 * level.
 *
 * update_screen() is the function that updates all windows and status lines.
 * It is called form the main loop when must_redraw is non-zero.  It may be
 * called from other places when an immediate screen update is needed.
 *
 * The part of the buffer that is displayed in a window is set with:
 * - w_topline (first buffer line in window)
 * - w_topfill (filler lines above the first line)
 * - w_leftcol (leftmost window cell in window),
 * - w_skipcol (skipped window cells of first line)
 *
 * Commands that only move the cursor around in a window, do not need to take
 * action to update the display.  The main loop will check if w_topline is
 * valid and update it (scroll the window) when needed.
 *
 * Commands that scroll a window change w_topline and must call
 * check_cursor() to move the cursor into the visible part of the window, and
 * call redraw_later(UPD_VALID) to have the window displayed by update_screen()
 * later.
 *
 * Commands that change text in the buffer must call changed_bytes() or
 * changed_lines() to mark the area that changed and will require updating
 * later.  The main loop will call update_screen(), which will update each
 * window that shows the changed buffer.  This assumes text above the change
 * can remain displayed as it is.  Text after the change may need updating for
 * scrolling, folding and syntax highlighting.
 *
 * Commands that change how a window is displayed (e.g., setting 'list') or
 * invalidate the contents of a window in another way (e.g., change fold
 * settings), must call redraw_later(UPD_NOT_VALID) to have the whole window
 * redisplayed by update_screen() later.
 *
 * Commands that change how a buffer is displayed (e.g., setting 'tabstop')
 * must call redraw_curbuf_later(UPD_NOT_VALID) to have all the windows for the
 * buffer redisplayed by update_screen() later.
 *
 * Commands that change highlighting and possibly cause a scroll too must call
 * redraw_later(UPD_SOME_VALID) to update the whole window but still use
 * scrolling to avoid redrawing everything.  But the length of displayed lines
 * must not change, use UPD_NOT_VALID then.
 *
 * Commands that move the window position must call redraw_later(UPD_NOT_VALID).
 * TODO: should minimize redrawing by scrolling when possible.
 *
 * Commands that change everything (e.g., resizing the screen) must call
 * redraw_all_later(UPD_NOT_VALID) or redraw_all_later(UPD_CLEAR).
 *
 * Things that are handled indirectly:
 * - When messages scroll the screen up, msg_scrolled will be set and
 *   update_screen() called to redraw.
 */

#অন্তর্ভাব "vim.h"

স্থির শূন্য win_update(win_T *wp);
#ifdef FEAT_STL_OPT
স্থির শূন্য redraw_custom_statusline(win_T *wp);
#endif
স্থির শূন্য borrow_stl_vsep_hl(শূন্য);
#যদি defined(FEAT_অনুসন্ধান_EXTRA) || defined(FEAT_CLIPBOARD)
স্থির পূর্ণসংখ্যা  did_update_one_window;
#endif

#ifdef FEAT_EVAL
স্থির শূন্য redraw_listener_cleanup(শূন্য);
স্থির শূন্য invoke_redraw_listener_start_or_end(bool start);
#endif

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    পূর্ণসংখ্যা
update_screen(পূর্ণসংখ্যা type_arg)
{
    পূর্ণসংখ্যা		type = type_arg;
    win_T	*wp;
    স্থির পূর্ণসংখ্যা	did_intro = FALSE;
#ifdef FEAT_GUI
    পূর্ণসংখ্যা		did_one = FALSE;
    পূর্ণসংখ্যা		did_undraw = FALSE;
    পূর্ণসংখ্যা		gui_cursor_col = 0;
    পূর্ণসংখ্যা		gui_cursor_row = 0;
#endif
    পূর্ণসংখ্যা		no_update = FALSE;
    পূর্ণসংখ্যা		save_pum_will_redraw = pum_will_redraw;
#ifdef FEAT_PROP_POPUP
    পূর্ণসংখ্যা		did_redraw_window = FALSE;
#endif
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    যদি (!screen_valid(TRUE))
	ফেরত FAIL;

    যদি (type == UPD_VALID_NO_UPDATE)
    {
	no_update = TRUE;
	type = 0;
    }

#ifdef FEAT_EVAL
    {
	buf_T *buf;

	// Before updating the screen, notify any listeners of changed text.
	FOR_ALL_BUFFERS(buf)
	    invoke_listeners(buf);
    }
#endif

#ifdef FEAT_DIFF
    // May have postponed updating diffs.
    যদি (need_diff_redraw)
	diff_redraw(TRUE);
#endif

    যদি (must_redraw)
    {
	যদি (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    যদি (curwin->w_সারিগুলি_valid == 0 && type < UPD_NOT_VALID
#ifdef FEAT_TERMINAL
	    && !term_do_update_window(curwin)
#endif
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    যদি (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	যদি (type > UPD_INVERTED_ALL)
	    curwin->w_সারিগুলি_valid = 0;	// don't use w_lines[].wl_size now
	ফেরত FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    পূর্ণসংখ্যা hid_cursor = !sync_output_active();
    যদি (hid_cursor)
	cursor_off();
    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

#ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.  This may set w_redraw_top and w_redraw_bot
    // in some windows.
    may_update_popup_mask(type);
#endif

#ifdef FEAT_SYN_HL
    ++display_tick;	    // let syntax code know we're in a next round of
			    // display updating
#endif
    যদি (no_update)
	++no_win_do_সারিগুলি_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    যদি (বার্তা_scrolled)
    {
	clear_cmdline = TRUE;
	যদি (type != UPD_CLEAR)
	{
	    যদি (বার্তা_scrolled > Rows - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    অন্যথা
	    {
		check_for_delay(FALSE);
		যদি (screen_ins_সারিগুলি(0, 0, বার্তা_scrolled, (পূর্ণসংখ্যা)Rows, 0, ফাঁকা)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    যদি (wp->w_winrow < বার্তা_scrolled)
		    {
			যদি (W_WINROW(wp) + wp->w_height > বার্তা_scrolled
				&& wp->w_redr_type < UPD_REDRAW_শীর্ষ
				&& wp->w_সারিগুলি_valid > 0
				&& wp->w_topline == wp->w_সারিগুলি[0].wl_lnum)
			{
			    wp->w_upd_rows = বার্তা_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_শীর্ষ;
			}
			অন্যথা
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    যদি (W_WINROW(wp) + wp->w_height
					 + wp->w_status_height <= বার্তা_scrolled)
				wp->w_redr_status = true;
			}
		    }
		}
		যদি (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#যদি defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#endif
	}
	বার্তা_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    যদি (need_highlight_changed)
	highlight_changed();

    যদি (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    যদি (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    যদি (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? সংখ্যা_width(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#endif

#ifdef FEAT_EVAL
    invoke_redraw_listener_start_or_end(true);
#endif

    // Only start redrawing if there is really something to do.
    যদি (type == UPD_INVERTED)
	update_curswant();
    যদি (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_সারিগুলি[0].wl_valid
#ifdef FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#endif
		    && curwin->w_topline == curwin->w_সারিগুলি[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_active
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_mode == VIsual_mode
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    যদি (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#যদি defined(FEAT_TABPANEL)
    যদি (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#endif

#ifdef FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	যদি (wp->w_বাফার->b_mod_set)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    জন্য (wwp = firstwin; wwp != wp; wwp = wwp->w_পরবর্তী)
		যদি (wwp->w_বাফার == wp->w_বাফার)
		    বিরতি;
	    যদি (wwp == wp && syntax_present(wp))
		syn_স্তূপ_apply_changes(wp->w_বাফার);
	}
    }
#endif

    যদি (pum_redraw_in_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#যদি defined(FEAT_অনুসন্ধান_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_window = FALSE;
#endif
#ifdef FEAT_অনুসন্ধান_EXTRA
    screen_অনুসন্ধান_hl.rm.regprog = ফাঁকা;
#endif
    FOR_ALL_WINDOWS(wp)
    {
	override_success = ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);

	যদি (wp->w_redr_type != 0)
	{
	    cursor_off();
#ifdef FEAT_PROP_POPUP
	    did_redraw_window = TRUE;
#endif
#ifdef FEAT_GUI
	    যদি (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		যদি (gui.in_use && (wp == curwin || cursor_is_sleeping()))
		{
		    gui_cursor_col = gui.cursor_col;
		    gui_cursor_row = gui.cursor_row;
		    gui_undraw_cursor();
		    did_undraw = TRUE;
		}
	    }
#endif
	    win_update(wp);
	}

	// redraw status line after the window to minimize cursor movement
	যদি (wp->w_redr_status)
	{
	    cursor_off();
	    win_redr_status(wp, TRUE); // any popup menu will be redrawn below
	}

	যদি (override_success)
	    বের_করো_highlight_overrides();

    }
#যদি defined(FEAT_অনুসন্ধান_EXTRA)
    end_অনুসন্ধান_hl();
#endif

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    যদি (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    যদি (wp->w_vsep_width > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_বাফার->b_mod_set = false;

#ifdef FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    যদি (did_redraw_window || popup_need_redraw())
	update_popups(win_update);
#endif

#ifdef FEAT_TERMINAL
    FOR_ALL_WINDOWS(wp)
	// If this window contains a terminal, after redrawing all windows, the
	// dirty row range can be reset.
	term_did_update_window(wp);
#endif

    after_updating_screen(TRUE);

    // Clear or redraw the command line.  Done last, because scrolling may
    // mess up the command line.
    যদি (clear_cmdline || redraw_cmdline || redraw_mode)
	showmode();

    যদি (no_update)
	--no_win_do_সারিগুলি_ins;

    // May put up an introductory message when not editing a file
    যদি (!did_intro)
	maybe_intro_message();
    did_intro = TRUE;

#ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    যদি (gui.in_use)
    {
	যদি (did_undraw && !gui_mch_is_blink_off())
	{
	    mch_disable_flush();
	    out_flush();	// required before updating the cursor
	    mch_enable_flush();

	    // Put the GUI position where the cursor was, gui_update_cursor()
	    // uses that.
	    gui.col = gui_cursor_col;
	    gui.row = gui_cursor_row;
	    gui.col = mb_fix_col(gui.col, gui.row);
	    gui_update_cursor(FALSE, FALSE);
	    gui_may_flush();
	    screen_cur_col = gui.col;
	    screen_cur_row = gui.row;
	}
	অন্যথা
	    out_flush();
	gui_update_scrollbars(FALSE);
    }
#endif

#ifdef FEAT_IMAGE
    // Make sure to do this last!
    draw_image_placements();
#endif

#ifdef FEAT_EVAL
    invoke_redraw_listener_start_or_end(false);
    redraw_listener_cleanup();
#endif

    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
    যদি (hid_cursor)
	cursor_on();

    ফেরত OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    পূর্ণসংখ্যা
statusline_row(win_T *wp)
{
#যদি defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    যদি (wp->w_frame->fr_height == wp->w_status_height && !popup_is_popup(wp))
	ফেরত wp->w_winrow;
#endif
    ফেরত W_WINROW(wp) + wp->w_height;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    শূন্য
win_redr_status(win_T *wp, পূর্ণসংখ্যা ignore_pum UNUSED)
{
    পূর্ণসংখ্যা		row;
    পূর্ণসংখ্যা		fillchar;
    পূর্ণসংখ্যা		attr;
    পূর্ণসংখ্যা		i;
    স্থির পূর্ণসংখ্যা  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    যদি (busy)
	ফেরত;
    busy = TRUE;

    row = statusline_row(wp);

    wp->w_redr_status = false;
    যদি (wp->w_status_height == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    অন্যথা যদি (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_status = true;
    }
#ifdef FEAT_STL_OPT
    অন্যথা যদি (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#endif
    অন্যথা
    {
	char_u	*p;
	পূর্ণসংখ্যা	plen;
	পূর্ণসংখ্যা	NameBufflen;
	পূর্ণসংখ্যা	this_ru_col;
	পূর্ণসংখ্যা	n;			// scratch value

	fillchar = fillchar_status(&attr, wp);

	get_trans_bufname(wp->w_বাফার);
	p = NameBuff;
	plen = (পূর্ণসংখ্যা)STRLEN(p);

	যদি ((bt_help(wp->w_বাফার)
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
		    || bufIsChanged(wp->w_বাফার)
		    || wp->w_বাফার->b_p_ro)
		&& plen < MAXPATHL - 1)
	{
	    *(p + plen++) = ' ';	// replace NUL with space
	    *(p + plen) = NUL;		// NUL terminate the string
	}
	যদি (bt_help(wp->w_বাফার))
	    plen += vim_snprintf((অক্ষর *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#ifdef FEAT_QUICKFIX
	যদি (wp->w_p_pvw)
	    plen += vim_snprintf((অক্ষর *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#endif
	যদি (bufIsChanged(wp->w_বাফার) && !bt_terminal(wp->w_বাফার))
	    plen += vim_snprintf((অক্ষর *)p + plen, MAXPATHL - plen, "%s", "[+]");
	যদি (wp->w_বাফার->b_p_ro)
	    plen += vim_snprintf((অক্ষর *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_col = ru_col - (cmdline_width - wp->w_width);
	n = (wp->w_width + 1) / 2;
	যদি (this_ru_col < n)
	    this_ru_col = n;
	যদি (this_ru_col <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	অন্যথা যদি (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    জন্য (i = 0; p[i] != NUL && plen >= this_ru_col - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    যদি (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	অন্যথা যদি (plen > this_ru_col - 1)
	{
	    p += plen - (this_ru_col - 1);
	    *p = '<';
	    plen = this_ru_col - 1;
	}

	screen_puts(p, row, wp->w_wincol, attr);
	screen_fill(row, row + 1, plen + wp->w_wincol,
		    this_ru_col + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	জন্য (i = 1; i < wp->w_status_height; i++)
	    screen_fill(row + i, row + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	যদি ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_col - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, row, (পূর্ণসংখ্যা)(this_ru_col - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	যদি (p_sc && *p_sloc == 's')
	{
	    n = this_ru_col - plen - 2;		    // perform the calculation here so we only do it once
	    পূর্ণসংখ্যা	width = MIN(10, n);

	    যদি (width > 0)
		screen_puts_len(showcmd_buf, width, row,
				wp->w_wincol + this_ru_col - width - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    যদি (wp->w_vsep_width != 0 && wp->w_status_height != 0 && redrawing())
    {
	জন্য (i = 0; i < wp->w_status_height; i++)
	{
	    পূর্ণসংখ্যা r = row + i;
	    যদি (stl_connected(wp))
		fillchar = fillchar_status(&attr, wp);
	    অন্যথা
		fillchar = fillchar_vsep(&attr, wp, r);
	    screen_putchar(fillchar, r, W_ENDCOL(wp), attr);
	}
    }
    busy = FALSE;
}

/*
 * Borrow status line edge highlight to adjacent vsep cells.
 *  - When the pair involves curwin: borrow curwin's edge attr so custom
 *    statusline highlights flow into the vsep cell.
 *  - When both windows are non-current: borrow the left window's right-edge
 *    attr only if the status fillchar is a space, so StatusLineNC blends
 *    over the join without changing visible characters.
 *  - Cells where the vsep char is drawn (stl_connected == FALSE) are left
 *    untouched so the VertSplit highlight is preserved.
 * Called for every cursor movement, thus only cells whose attribute changed
 * are written to the screen.
 */
    স্থির শূন্য
borrow_stl_vsep_hl(শূন্য)
{
    win_T   *বাম = ফাঁকা;
    win_T   *ডান = ফাঁকা;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    যদি (!redrawing() || Screen_সারিগুলি == ফাঁকা)
	ফেরত;

    FOR_ALL_WINDOWS(বাম)
    {
	যদি (বাম->w_status_height == 0 || বাম->w_vsep_width == 0)
	    চালু;
	// A window of zero width has no status line cell to borrow from.
	যদি (বাম->w_width == 0)
	    চালু;
	যদি (!stl_connected(বাম))
	    চালু;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = ফাঁকা;
	পূর্ণসংখ্যা start = 0;
	পূর্ণসংখ্যা end = 0;

	FOR_ALL_WINDOWS(ডান)
	{
	    যদি (ডান == বাম || ডান->w_status_height == 0)
		চালু;
	    যদি (ডান->w_wincol != W_ENDCOL(বাম) + 1)
		চালু;
	    পূর্ণসংখ্যা l_stl_row = W_WINROW(বাম) + বাম->w_height;
	    পূর্ণসংখ্যা r_stl_row = W_WINROW(ডান) + ডান->w_height;

	    start = l_stl_row > r_stl_row ? l_stl_row : r_stl_row;
	    end = l_stl_row + বাম->w_status_height
				< r_stl_row + ডান->w_status_height
		? l_stl_row + বাম->w_status_height
		: r_stl_row + ডান->w_status_height;
	    যদি (start < end)
	    {
		neighbour = ডান;
		বিরতি;
	    }
	}
	যদি (neighbour == ফাঁকা)
	    চালু;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	পূর্ণসংখ্যা	hl;
	যদি (বাম != curwin && neighbour != curwin
		&& fillchar_status(&hl, বাম) != ' ')
	    চালু;

	// Source: prefer curwin's side; otherwise left window's right edge.
	পূর্ণসংখ্যা dst_col = W_ENDCOL(বাম);
	পূর্ণসংখ্যা src_col = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(বাম) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	যদি (dst_col >= screen_Columns || src_col >= screen_Columns)
	    চালু;
	যদি (end > screen_Rows)
	    end = screen_Rows;

	জন্য (পূর্ণসংখ্যা r = start; r < end; r++)
	{
	    unsigned	dst_off = সারি_Offset[r] + dst_col;
	    sattr_T	attr = ScreenAttrs[সারি_Offset[r] + src_col];

	    যদি (ScreenAttrs[dst_off] != attr)
	    {
		ScreenAttrs[dst_off] = attr;
		screen_char(dst_off, r, dst_col);
	    }
	}
    }
}

#ifdef FEAT_STL_OPT
/*
 * Redraw the status line according to 'statusline' and take care of any
 * errors encountered.
 */
    স্থির শূন্য
redraw_custom_statusline(win_T *wp)
{
    স্থির পূর্ণসংখ্যা	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    যদি (entered)
	ফেরত;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#endif

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    শূন্য
showruler(পূর্ণসংখ্যা always)
{
    যদি (!always && !redrawing())
	ফেরত;
    যদি (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_status = true;
	ফেরত;
    }
#যদি defined(FEAT_STL_OPT)
    যদি ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_status_height)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    অন্যথা
#endif
	win_redr_ruler(curwin, always, FALSE);

    যদি (need_maketitle
#ifdef FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_IN_ICON))
	    || (p_title && (stl_syntax & STL_IN_TITLE))
#endif
       )
	maketitle();

    // Redraw the tab pages line if needed.
    যদি (redraw_tabline)
	draw_tabline();

#যদি defined(FEAT_TABPANEL)
    যদি (redraw_tabpanel)
	draw_tabpanel();
#endif
}

    শূন্য
win_redr_ruler(win_T *wp, পূর্ণসংখ্যা always, পূর্ণসংখ্যা ignore_pum)
{
    পূর্ণসংখ্যা	empty_সারি = FALSE;

    // If 'ruler' off don't do anything
    যদি (!p_ru)
	ফেরত;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    যদি (wp->w_cursor.lnum > wp->w_বাফার->b_ml.ml_সারি_গুনতি)
	ফেরত;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    যদি (wp == lastwin && lastwin->w_status_height == 0)
	যদি (edit_submode != ফাঁকা)
	    ফেরত;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    যদি (!ignore_pum && pum_visible())
	ফেরত;

#ifdef FEAT_STL_OPT
    যদি (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	ফেরত;
    }
#endif

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    যদি ((State & MODE_ঢোকাও) == 0
		&& *ml_get_buf(wp->w_বাফার, wp->w_cursor.lnum, FALSE) == NUL)
	empty_সারি = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    যদি (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.col != wp->w_ru_cursor.col
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_বাফার->b_ml.ml_সারি_গুনতি != wp->w_ru_সারি_গুনতি
#ifdef FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#endif
	    || empty_সারি != wp->w_ru_empty)
    {
	পূর্ণসংখ্যা	row;
	পূর্ণসংখ্যা	fillchar;
	পূর্ণসংখ্যা	attr;
	পূর্ণসংখ্যা	off;
	পূর্ণসংখ্যা	width;
	colnr_T	virtcol;
#define RULER_BUF_LEN 70
	char_u	বাফার[RULER_BUF_LEN];
	পূর্ণসংখ্যা	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	পূর্ণসংখ্যা	rel_poslen;
	পূর্ণসংখ্যা	this_ru_col;
	পূর্ণসংখ্যা	n1;			    // scratch value
	পূর্ণসংখ্যা	n2;			    // scratch value
	bool	override_success =
	    ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	যদি (wp->w_status_height)
	{
	    row = statusline_row(wp);
	    fillchar = fillchar_status(&attr, wp);
	    off = wp->w_wincol;
	    width = wp->w_width;
	}
	অন্যথা
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_col_off;
	    width = cmdline_width;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	যদি (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, ফাঁকা, &virtcol, ফাঁকা, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((অক্ষর *)বাফার, RULER_BUF_LEN, _("%ld,"),
		(wp->w_বাফার->b_ml.ml_flags & ML_EMPTY)
		    ? 0L
		    : (long)(wp->w_cursor.lnum));
	bufferlen += col_print(বাফার + bufferlen, RULER_BUF_LEN - bufferlen,
			empty_সারি ? 0 : (পূর্ণসংখ্যা)wp->w_cursor.col + 1,
			(পূর্ণসংখ্যা)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	যদি (wp->w_status_height == 0)	// can't use last char of screen
	    ++n1;

	this_ru_col = ru_col - (cmdline_width - width);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (width + 1) / 2;
	যদি (this_ru_col < n2)
	    this_ru_col = n2;
	যদি (this_ru_col + n1 < width)
	{
	    // need at least space for rel_pos + NUL
	    যখন (this_ru_col + n1 < width
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		যদি (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, বাফার + bufferlen);
		অন্যথা
		    বাফার[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((অক্ষর *)বাফার + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	যদি (has_mbyte)
	{
	    জন্য (n1 = 0, n2 = 0; বাফার[n1] != NUL; n1 += (*mb_ptr2len)(বাফার + n1))
	    {
		n2 += (*mb_ptr2cells)(বাফার + n1);
		যদি (this_ru_col + n2 > width)
		{
		    bufferlen = n1;
		    বাফার[bufferlen] = NUL;
		    বিরতি;
		}
	    }
	}
	অন্যথা যদি (this_ru_col + bufferlen > width)
	{
	    bufferlen = width - this_ru_col;
	    বাফার[bufferlen] = NUL;
	}

	screen_puts(বাফার, row, this_ru_col + off, attr);
	n1 = redraw_cmdline;
	screen_fill(row, row + 1,
		this_ru_col + off + bufferlen,
		(off + width),
		fillchar, fillchar, attr);
	// don't redraw the cmdline because of showing the ruler
	redraw_cmdline = n1;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_empty = empty_সারি;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_সারি_গুনতি = wp->w_বাফার->b_ml.ml_সারি_গুনতি;
#ifdef FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#endif

	যদি (override_success)
	    বের_করো_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    শূন্য
after_updating_screen(পূর্ণসংখ্যা may_resize_shell UNUSED)
{
    updating_screen = FALSE;
#ifdef FEAT_GUI
    যদি (may_resize_shell)
	gui_may_resize_shell();
#endif
#ifdef FEAT_TERMINAL
    term_check_channel_closed_recently();
#endif

#ifdef HAVE_DROP_FILE
    // If handle_drop() was called while updating_screen was TRUE need to
    // handle the drop now.
    handle_any_postponed_drop();
#endif
}

/*
 * Update all windows that are editing the current buffer.
 */
    শূন্য
update_curbuf(পূর্ণসংখ্যা type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#যদি defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    স্থির পূর্ণসংখ্যা
text_to_screenline(win_T *wp, char_u *text, পূর্ণসংখ্যা col)
{
    পূর্ণসংখ্যা		off = (পূর্ণসংখ্যা)(current_Screen_সারি - Screen_সারিগুলি);

    যদি (has_mbyte)
    {
	পূর্ণসংখ্যা	cells;
	পূর্ণসংখ্যা	u8c, u8cc[MAX_MCO];
	পূর্ণসংখ্যা	i;
	পূর্ণসংখ্যা	idx;
	পূর্ণসংখ্যা	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	পূর্ণসংখ্যা	পূর্ব_c = 0;		// previous Arabic character
	পূর্ণসংখ্যা	পূর্ব_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	যদি (wp->w_p_rl)
	    idx = off;
	অন্যথা
# endif
	    idx = off + col;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	জন্য (p = text; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    যদি (col + cells > wp->w_width)
		বিরতি;
# ifdef FEAT_RIGHTLEFT
	    যদি (wp->w_p_rl)
		idx = off + wp->w_width - col - cells;
# endif
	    Screen_সারিগুলি[idx] = *p;
	    যদি (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		যদি (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_সারিগুলি_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    পূর্ব_c = u8c;
# endif
		}
		অন্যথা
		{
# ifdef FEAT_ARABIC
		    যদি (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			পূর্ণসংখ্যা	pc, pc1, nc;
			পূর্ণসংখ্যা	pcc[MAX_MCO];
			পূর্ণসংখ্যা	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			যদি (wp->w_p_rl)
			{
			    pc = পূর্ব_c;
			    pc1 = পূর্ব_c1;
			    nc = utf_ptr2char(p + c_len);
			    পূর্ব_c1 = u8cc[0];
			}
			অন্যথা
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = পূর্ব_c;
			    pc1 = pcc[0];
			}
			পূর্ব_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			Screen_সারিগুলি[idx] = firstbyte;
		    }
		    অন্যথা
			পূর্ব_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_সারিগুলি_UC[idx] = u8c;
		    জন্য (i = 0; i < Screen_mco; ++i)
		    {
			Screen_সারিগুলি_C[i][idx] = u8cc[i];
			যদি (u8cc[i] == 0)
			    বিরতি;
		    }
		}
		যদি (cells > 1)
		    Screen_সারিগুলি[idx + 1] = 0;
	    }
	    অন্যথা যদি (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    অন্যথা যদি (cells > 1)
		// double-width character
		Screen_সারিগুলি[idx + 1] = p[1];
	    col += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    অন্যথা
    {
	পূর্ণসংখ্যা len = (পূর্ণসংখ্যা)STRLEN(text);
	পূর্ণসংখ্যা n = wp->w_width - col;

	যদি (len > n)
	    len = n;
	যদি (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    যদি (wp->w_p_rl)
		mch_memmove(current_Screen_সারি, text, len);
	    অন্যথা
# endif
		mch_memmove(current_Screen_সারি + col, text, len);
	    col += len;
	}
    }
    ফেরত col;
}
#endif

#ifdef FEAT_MENU
/*
 * Draw the window toolbar.
 */
    স্থির শূন্য
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    পূর্ণসংখ্যা		item_idx = 0;
    পূর্ণসংখ্যা		item_গুনতি = 0;
    পূর্ণসংখ্যা		col = 0;
    পূর্ণসংখ্যা		পরবর্তী_col;
    পূর্ণসংখ্যা		off = (পূর্ণসংখ্যা)(current_Screen_সারি - Screen_সারিগুলি);
    bool	override_success =
	ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);
    পূর্ণসংখ্যা		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    পূর্ণসংখ্যা		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_বস্তুসমূহ);
    FOR_ALL_CHILD_MENUS(wp->w_winbar, menu)
	++item_গুনতি;
    wp->w_winbar_বস্তুসমূহ = ALLOC_CLEAR_MULT(winbar_item_T, item_গুনতি + 1);

    // TODO: use fewer spaces if there is not enough room
    জন্য (menu = wp->w_winbar->children;
			  menu != ফাঁকা && col < wp->w_width; menu = menu->পরবর্তী)
    {
	space_to_screenline(off + col, fill_attr);
	যদি (++col >= wp->w_width)
	    বিরতি;
	যদি (col > 1)
	{
	    space_to_screenline(off + col, fill_attr);
	    যদি (++col >= wp->w_width)
		বিরতি;
	}

	wp->w_winbar_বস্তুসমূহ[item_idx].wb_startcol = col;
	space_to_screenline(off + col, button_attr);
	যদি (++col >= wp->w_width)
	    বিরতি;

	পরবর্তী_col = text_to_screenline(wp, menu->name, col);
	যখন (col < পরবর্তী_col)
	{
	    ScreenAttrs[off + col] = button_attr;
	    ++col;
	}
	wp->w_winbar_বস্তুসমূহ[item_idx].wb_endcol = col;
	wp->w_winbar_বস্তুসমূহ[item_idx].wb_menu = menu;
	++item_idx;

	যদি (col >= wp->w_width)
	    বিরতি;
	space_to_screenline(off + col, button_attr);
	++col;
    }
    যখন (col < wp->w_width)
    {
	space_to_screenline(off + col, fill_attr);
	++col;
    }
    wp->w_winbar_বস্তুসমূহ[item_idx].wb_menu = ফাঁকা; // end marker

    screen_সারি(wp, wp->w_winrow, wp->w_wincol, wp->w_width,
							  wp->w_width, -1, 0);

    যদি (override_success)
	বের_করো_highlight_overrides();
}
#endif

#যদি defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    স্থির শূন্য
copy_text_attr(
    পূর্ণসংখ্যা		off,
    char_u	*buf,
    পূর্ণসংখ্যা		len,
    পূর্ণসংখ্যা		attr)
{
    পূর্ণসংখ্যা		i;

    mch_memmove(Screen_সারিগুলি + off, buf, (size_t)len);
    যদি (enc_utf8)
	vim_memset(Screen_সারিগুলি_UC + off, 0, আকার(u8char_T) * (size_t)len);
    জন্য (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    স্থির শূন্য
fold_সারি(
    win_T	*wp,
    long	fold_গুনতি,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    পূর্ণসংখ্যা		row)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*শীর্ষ_2, *bot;
    linenr_T	lnume = lnum + fold_গুনতি - 1;
    পূর্ণসংখ্যা		len;
    char_u	*text;
    পূর্ণসংখ্যা		fdc;
    পূর্ণসংখ্যা		col;
    পূর্ণসংখ্যা		txtcol;
    পূর্ণসংখ্যা		off = (পূর্ণসংখ্যা)(current_Screen_সারি - Screen_সারিগুলি);
    পূর্ণসংখ্যা		ri;

    // Build the fold line:
    // 1. Add the cmdwin_type for the command-line window
    // 2. Add the 'foldcolumn'
    // 3. Add the 'number' or 'relativenumber' column
    // 4. Compose the text
    // 5. Add the text
    // 6. set highlighting for the Visual area an other text
    col = 0;

    // 1. Add the cmdwin_type for the command-line window
    // Ignores 'rightleft', this window is never right-left.
    যদি (wp == cmdwin_win)
    {
	Screen_সারিগুলি[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	যদি (enc_utf8)
	    Screen_সারিগুলি_UC[off] = 0;
	++col;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    করো { \
	যদি (wp->w_p_rl) \
	    জন্য (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_width - (p) - (l)) + ri] = v; \
	 অন্যথা \
	    জন্য (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } যখন (0)
# অন্যথা
#  define RL_MEMSET(p, v, l) \
    করো { \
	জন্য (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } যখন (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, col);
    যদি (fdc > 0)
    {
	char_u	*p;
	পূর্ণসংখ্যা	i;
	পূর্ণসংখ্যা	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	জন্য (i = 0; i < fdc; i++)
	{
	    পূর্ণসংখ্যা		ch;

	    যদি (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    অন্যথা
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    যদি (wp->w_p_rl)
		idx = off + wp->w_width - i - 1 - col;
	    অন্যথা
# endif
		idx = off + col + i;
	    যদি (enc_utf8)
	    {
		যদি (ch >= 0x80)
		{
		    Screen_সারিগুলি_UC[idx] = ch;
		    Screen_সারিগুলি_C[0][idx] = 0;
		    Screen_সারিগুলি[idx] = 0x80;
		}
		অন্যথা
		{
		    Screen_সারিগুলি[idx] = ch;
		    Screen_সারিগুলি_UC[idx] = 0;
		}
	    }
	    অন্যথা
		Screen_সারিগুলি[idx] = ch;
	}

	RL_MEMSET(col, HL_ATTR(HLF_FC), fdc);
	col += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    // text
    RL_MEMSET(col, HL_ATTR(HLF_FL), wp->w_width - col);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    যদি (signcolumn_on(wp))
    {
	len = wp->w_width - col;
	যদি (len > 0)
	{
	    যদি (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    যদি (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    অন্যথা
#  endif
		copy_text_attr(off + col, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    col += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    যদি (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_width - col;
	যদি (len > 0)
	{
	    পূর্ণসংখ্যা	    w = সংখ্যা_width(wp);
	    long    অঙ্ক;
	    অক্ষর    *fmt = "%*ld ";

	    যদি (len > w + 1)
		len = w + 1;

	    যদি (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		অঙ্ক = (long)lnum;
	    অন্যথা
	    {
		// 'relativenumber', don't use negative numbers
		অঙ্ক = labs((long)get_cursor_rel_lnum(wp, lnum));
		যদি (অঙ্ক == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    অঙ্ক = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((অক্ষর *)buf, আকার(buf), fmt, w, অঙ্ক);
# ifdef FEAT_RIGHTLEFT
	    যদি (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col, buf, len,
							     HL_ATTR(HLF_FL));
	    অন্যথা
# endif
		copy_text_attr(off + col, buf, len, HL_ATTR(HLF_FL));
	    col += len;
	}
    }

    // 4. Compose the folded-line string with 'foldtext', if set.
    text = get_foldtext(wp, lnum, lnume, foldinfo, buf);

    txtcol = col;	// remember where text starts

    // 5. move the text to current_ScreenLine.  Fill up with "fold" from
    //    'fillchars'.
    //    Right-left text is put in columns 0 - number-col, normal text is put
    //    in columns number-col - window-width.
    col = text_to_screenline(wp, text, col);

    // Fill the rest of the line with the fold filler
    যখন (col < wp->w_width)
    {
	পূর্ণসংখ্যা c = wp->w_fill_chars.fold;
	পূর্ণসংখ্যা idx = off + col;

# ifdef FEAT_RIGHTLEFT
	যদি (wp->w_p_rl)
	    idx = off + wp->w_width - 1 - col;
# endif

	যদি (enc_utf8)
	{
	    যদি (c >= 0x80)
	    {
		Screen_সারিগুলি_UC[idx] = c;
		Screen_সারিগুলি_C[0][idx] = 0;
		Screen_সারিগুলি[idx] = 0x80; // avoid storing zero
	    }
	    অন্যথা
	    {
		Screen_সারিগুলি_UC[idx] = 0;
		Screen_সারিগুলি[idx] = c;
	    }
	}
	অন্যথা
	    Screen_সারিগুলি[idx] = c;
	++col;
    }

    যদি (text != buf)
	vim_free(text);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    যদি (VIsual_active && wp->w_বাফার == curwin->w_বাফার)
    {
	যদি (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    শীর্ষ_2 = &curwin->w_cursor;
	    bot = &VIsual;
	}
	অন্যথা
	{
	    // Visual is before curwin->w_cursor
	    শীর্ষ_2 = &VIsual;
	    bot = &curwin->w_cursor;
	}
	যদি (lnum >= শীর্ষ_2->lnum
		&& lnume <= bot->lnum
		&& (VIsual_mode != 'v'
		    || ((lnum > শীর্ষ_2->lnum
			    || (lnum == শীর্ষ_2->lnum
				&& শীর্ষ_2->col == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->col - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_বাফার, lnume))))))
	{
	    যদি (VIsual_mode == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		যদি (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_width)
		{
		    যদি (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_width)
			len = wp->w_old_cursor_lcol;
		    অন্যথা
			len = wp->w_width - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (পূর্ণসংখ্যা)wp->w_old_cursor_fcol);
		}
	    }
	    অন্যথা
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_width - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    যদি (wp->w_p_cc_cols)
    {
	পূর্ণসংখ্যা i = 0;
	পূর্ণসংখ্যা j = wp->w_p_cc_cols[i];
	পূর্ণসংখ্যা old_txtcol = txtcol;

	যখন (j > -1)
	{
	    txtcol += j;
	    যদি (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    অন্যথা
		txtcol -= wp->w_leftcol;
	    যদি (txtcol >= 0 && txtcol < wp->w_width)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_cols[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    যদি (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	যদি (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	অন্যথা
	    txtcol -= wp->w_leftcol;
	যদি (txtcol >= 0 && txtcol < wp->w_width)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_সারি(wp, row + W_WINROW(wp), wp->w_wincol, wp->w_width, wp->w_width,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    যদি (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_row = row;
	curwin->w_cline_height = 1;
	curwin->w_cline_folded = true;
	curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
    }

# ifdef FEAT_CONCEAL
    // When the line was not folded w_wrow may have been set, recompute it.
    যদি (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_সারি(wp))
	curs_columns(TRUE);
# endif
}
#endif

/*
 * Update a single window.
 *
 * This may cause the windows below it also to be redrawn (when clearing the
 * screen or scrolling lines).
 *
 * How the window is redrawn depends on wp->w_redr_type.  Each type also
 * implies the one below it.
 * UPD_NOT_VALID	redraw the whole window
 * UPD_SOME_VALID	redraw the whole window but do scroll when possible
 * UPD_REDRAW_TOP	redraw the top w_upd_rows window lines, otherwise like
 *			UPD_VALID
 * UPD_INVERTED		redraw the changed part of the Visual area
 * UPD_INVERTED_ALL	redraw the whole Visual area
 * UPD_VALID	1. scroll up/down to adjust for a changed w_topline
 *		2. update lines at the top when scrolled down
 *		3. redraw changed text:
 *		   - if wp->w_buffer->b_mod_set set, update lines between
 *		     b_mod_top and b_mod_bot.
 *		   - if wp->w_redraw_top non-zero, redraw lines between
 *		     wp->w_redraw_top and wp->w_redraw_bot.
 *		   - continue redrawing when syntax status is invalid.
 *		4. if scrolled up, update lines at the bottom.
 * This results in three areas that may need updating:
 * top: from first row to top_end (when scrolled down)
 * mid: from mid_start to mid_end (update inversion or changed text)
 * bot: from bot_start to last row (when scrolled up)
 */
    স্থির শূন্য
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_বাফার;
    পূর্ণসংখ্যা		type;
    পূর্ণসংখ্যা		শীর্ষ_end = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    পূর্ণসংখ্যা		mid_start = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    পূর্ণসংখ্যা		mid_end = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    পূর্ণসংখ্যা		bot_start = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    পূর্ণসংখ্যা		scrolled_down = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    পূর্ণসংখ্যা		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //সারিগুলি
#ifdef FEAT_অনুসন্ধান_EXTRA
    পূর্ণসংখ্যা		শীর্ষ_to_mod = FALSE;    // redraw above mod_top
#endif

    পূর্ণসংখ্যা		row;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    পূর্ণসংখ্যা		idx;		// current index in w_lines[]
    পূর্ণসংখ্যা		srow;		// starting row of the current line

    পূর্ণসংখ্যা		eof = FALSE;	// if TRUE, we hit the end of the file
    পূর্ণসংখ্যা		didline = FALSE; // if TRUE, we finished the last line
    পূর্ণসংখ্যা		i;
    long	j;
    স্থির পূর্ণসংখ্যা	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#ifdef FEAT_CONCEAL
    পূর্ণসংখ্যা		old_wrow = wp->w_wrow;
    পূর্ণসংখ্যা		old_wcol = wp->w_wcol;
#endif
#ifdef FEAT_FOLDING
    long	fold_গুনতি;
#endif
#ifdef FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_সারি 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    পূর্ণসংখ্যা		did_update = DID_NONE;
    linenr_T	syntax_last_parsed = 0;		// last parsed text line
#endif
    linenr_T	mod_শীর্ষ = 0;
    linenr_T	mod_bot = 0;
#যদি defined(FEAT_SYN_HL) || defined(FEAT_অনুসন্ধান_EXTRA)
    পূর্ণসংখ্যা		save_got_int;
#endif
    bool	override_success;

#যদি defined(FEAT_অনুসন্ধান_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    যদি (!did_update_one_window)
    {
	did_update_one_window = TRUE;
# ifdef FEAT_অনুসন্ধান_EXTRA
	start_অনুসন্ধান_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	যদি (clip_star.available && clip_isautosel_star())
	    clip_update_selection(&clip_star);
	যদি (clip_plus.available && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#endif

#ifdef FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    যদি (wp->w_p_cuc)
	validate_virtcol_win(wp);
#endif

    type = wp->w_redr_type;

    যদি (type == UPD_NOT_VALID)
    {
	wp->w_redr_status = true;
	wp->w_সারিগুলি_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    যদি (wp->w_height + WINBAR_HEIGHT(wp) == 0
	    || (wp->w_frame->fr_height == wp->w_status_height
#যদি defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#endif
	       ))
    {
	wp->w_redr_type = 0;
	ফেরত;
    }

    // Window is zero-width: Only need to draw the separator.
    যদি (wp->w_width == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	ফেরত;
    }

    override_success = ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);


#ifdef FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    যদি (term_do_update_window(wp))
    {
	term_update_window(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	যদি (winbar_height(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	যদি (override_success)
	    বের_করো_highlight_overrides();
	ফেরত;
    }
#endif

#ifdef FEAT_অনুসন্ধান_EXTRA
    init_অনুসন্ধান_hl(wp, &screen_অনুসন্ধান_hl);
#endif

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    যদি (wp->w_skipcol > 0 && wp->w_width > win_col_off(wp))
    {
	পূর্ণসংখ্যা w = 0;
	পূর্ণসংখ্যা width1 = wp->w_width - win_col_off(wp);
	পূর্ণসংখ্যা width2 = width1 + win_col_off2(wp);
	পূর্ণসংখ্যা add = width1;

	যখন (w < wp->w_skipcol)
	{
	    যদি (w > 0)
		add = width2;
	    w += add;
	}
	যদি (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - add;
    }

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? সংখ্যা_width(wp) : 0;
    যদি (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    অন্যথা
#endif

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_শীর্ষ = wp->w_redraw_শীর্ষ;
	যদি (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	অন্যথা
	    mod_bot = 0;
	যদি (buf->b_mod_set)
	{
	    যদি (mod_শীর্ষ == 0 || mod_শীর্ষ > buf->b_mod_শীর্ষ)
	    {
		mod_শীর্ষ = buf->b_mod_শীর্ষ;
#ifdef FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		যদি (syntax_present(wp))
		{
		    mod_শীর্ষ -= buf->b_s.b_syn_sync_linebreaks;
		    যদি (mod_শীর্ষ < 1)
			mod_শীর্ষ = 1;
		}
#endif
	    }
	    যদি (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ifdef FEAT_অনুসন্ধান_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    যদি (screen_অনুসন্ধান_hl.rm.regprog != ফাঁকা
		    && re_multiline(screen_অনুসন্ধান_hl.rm.regprog))
		শীর্ষ_to_mod = TRUE;
	    অন্যথা
	    {
		matchitem_T *cur = wp->w_match_শীর্ষ;

		যখন (cur != ফাঁকা)
		{
		    যদি (cur->mit_match.regprog != ফাঁকা
				       && re_multiline(cur->mit_match.regprog))
		    {
			শীর্ষ_to_mod = TRUE;
			বিরতি;
		    }
		    cur = cur->mit_পরবর্তী;
		}
	    }
#endif
	}

#ifdef FEAT_অনুসন্ধান_EXTRA
	যদি (অনুসন্ধান_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    যদি (mod_শীর্ষ == 0 || mod_শীর্ষ > অনুসন্ধান_hl_has_cursor_lnum)
		mod_শীর্ষ = অনুসন্ধান_hl_has_cursor_lnum;
	    যদি (mod_bot == 0 || mod_bot < অনুসন্ধান_hl_has_cursor_lnum + 1)
		mod_bot = অনুসন্ধান_hl_has_cursor_lnum + 1;
	}
#endif

#ifdef FEAT_FOLDING
	যদি (mod_শীর্ষ != 0 && hasAnyFolding(wp))
	{
	    linenr_T	lnumt, lnumb;

	    // A change in a line can cause lines above it to become folded or
	    // unfolded.  Find the top most buffer line that may be affected.
	    // If the line was previously folded and displayed, get the first
	    // line of that fold.  If the line is folded now, get the first
	    // folded line.  Use the minimum of these two.

	    // Find last valid w_lines[] entry above mod_top.  Set lnumt to
	    // the line below it.  If there is no valid entry, use w_topline.
	    // Find the first valid w_lines[] entry below mod_bot.  Set lnumb
	    // to this line.  If there is no valid entry, use MAXLNUM.
	    lnumt = wp->w_topline;
	    lnumb = MAXLNUM;
	    জন্য (i = 0; i < wp->w_সারিগুলি_valid; ++i)
		যদি (wp->w_সারিগুলি[i].wl_valid)
		{
		    যদি (wp->w_সারিগুলি[i].wl_lastlnum < mod_শীর্ষ)
			lnumt = wp->w_সারিগুলি[i].wl_lastlnum + 1;
		    যদি (lnumb == MAXLNUM && wp->w_সারিগুলি[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_সারিগুলি[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			যদি (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (শূন্য)hasFoldingWin(wp, mod_শীর্ষ, &mod_শীর্ষ, ফাঁকা, TRUE, ফাঁকা);
	    যদি (mod_শীর্ষ > lnumt)
		mod_শীর্ষ = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (শূন্য)hasFoldingWin(wp, mod_bot, ফাঁকা, &mod_bot, TRUE, ফাঁকা);
	    ++mod_bot;
	    যদি (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#endif

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	যদি (mod_শীর্ষ != 0 && mod_শীর্ষ < wp->w_topline)
	{
	    যদি (mod_bot > wp->w_topline)
		mod_শীর্ষ = wp->w_topline;
#ifdef FEAT_SYN_HL
	    অন্যথা যদি (syntax_present(wp))
		শীর্ষ_end = 1;
#endif
	}
    }
    wp->w_redraw_শীর্ষ = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#ifdef FEAT_অনুসন্ধান_EXTRA
    অনুসন্ধান_hl_has_cursor_lnum = 0;
#endif


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    যদি (type == UPD_REDRAW_শীর্ষ)
    {
	j = 0;
	জন্য (i = 0; i < wp->w_সারিগুলি_valid; ++i)
	{
	    j += wp->w_সারিগুলি[i].wl_আকার;
	    যদি (j >= wp->w_upd_rows)
	    {
		শীর্ষ_end = j;
		বিরতি;
	    }
	}
	যদি (শীর্ষ_end == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	অন্যথা
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    যদি (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    যদি ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#ifdef FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#endif
	    )
    {
	যদি (mod_শীর্ষ != 0
		&& wp->w_topline == mod_শীর্ষ
		&& (!wp->w_সারিগুলি[0].wl_valid
		    || wp->w_topline == wp->w_সারিগুলি[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	অন্যথা যদি (wp->w_সারিগুলি[0].wl_valid
		&& (wp->w_topline < wp->w_সারিগুলি[0].wl_lnum
#ifdef FEAT_DIFF
		    || (wp->w_topline == wp->w_সারিগুলি[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#endif
		   ))
	{
	    // New topline is above old topline: May scroll down.
#ifdef FEAT_FOLDING
	    যদি (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		জন্য (ln = wp->w_topline; ln < wp->w_সারিগুলি[0].wl_lnum; ++ln)
		{
		    ++j;
		    যদি (j >= wp->w_height - 2)
			বিরতি;
		    (শূন্য)hasFoldingWin(wp, ln, ফাঁকা, &ln, TRUE, ফাঁকা);
		}
	    }
	    অন্যথা
#endif
		j = wp->w_সারিগুলি[0].wl_lnum - wp->w_topline;
	    যদি (j < wp->w_height - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_সারিগুলি[0].wl_lnum - 1,
								wp->w_height);
#ifdef FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		যদি (wp->w_সারিগুলি[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_সারিগুলি[0].wl_lnum)
							  - wp->w_old_topfill;
#endif
		যদি (i < wp->w_height - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    যদি (i > 0)
			check_for_delay(FALSE);
		    যদি (win_ins_সারিগুলি(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			যদি (wp->w_সারিগুলি_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    শীর্ষ_end = i;
			    scrolled_down = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    যদি ((wp->w_সারিগুলি_valid += j) > wp->w_height)
				wp->w_সারিগুলি_valid = wp->w_height;
			    জন্য (idx = wp->w_সারিগুলি_valid; idx - j >= 0; idx--)
				wp->w_সারিগুলি[idx] = wp->w_সারিগুলি[idx - j];
			    যখন (idx >= 0)
				wp->w_সারিগুলি[idx--].wl_valid = FALSE;
			}
		    }
		    অন্যথা
			mid_start = 0;		// redraw all lines
		}
		অন্যথা
		    mid_start = 0;		// redraw all lines
	    }
	    অন্যথা
		mid_start = 0;		// redraw all lines
	}
	অন্যথা
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    row = 0;
	    জন্য (i = 0; i < wp->w_সারিগুলি_valid && i < Rows; i++)
	    {
		যদি (wp->w_সারিগুলি[i].wl_valid
			&& wp->w_সারিগুলি[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    বিরতি;
		}
		row += wp->w_সারিগুলি[i].wl_আকার;
	    }
	    যদি (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//সারিগুলি
		mid_start = 0;
	    }
	    অন্যথা
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#ifdef FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		যদি (wp->w_সারিগুলি[0].wl_lnum == wp->w_topline)
		    row += wp->w_old_topfill;
		অন্যথা
		    row += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		row -= wp->w_topfill;
#endif
		যদি (row > Rows)  // just in case
		    row = Rows;
		যদি (row > 0)
		{
		    check_for_delay(FALSE);
		    যদি (win_del_সারিগুলি(wp, 0, row, FALSE, wp == firstwin, 0)
									 == OK)
			bot_start = wp->w_height - row;
		    অন্যথা
			mid_start = 0;		// redraw all lines
		}
		যদি ((row == 0 || bot_start < 999) && wp->w_সারিগুলি_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_start = 0;
		    idx = 0;
		    জন্য (;;)
		    {
			wp->w_সারিগুলি[idx] = wp->w_সারিগুলি[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			যদি (row > 0 && bot_start + row
				 + (পূর্ণসংখ্যা)wp->w_সারিগুলি[j].wl_আকার > wp->w_height)
			{
			    wp->w_সারিগুলি_valid = idx + 1;
			    বিরতি;
			}
			bot_start += wp->w_সারিগুলি[idx++].wl_আকার;

			// stop at the last valid entry in w_lines[].wl_size
			যদি (++j >= wp->w_সারিগুলি_valid)
			{
			    wp->w_সারিগুলি_valid = idx;
			    বিরতি;
			}
		    }
#ifdef FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    যদি (wp->w_p_diff && bot_start > 0)
			wp->w_সারিগুলি[0].wl_আকার = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#endif
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	যদি (mid_start == 0)
	    mid_end = wp->w_height;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	যদি (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    অন্যথা
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
    }

    যদি (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    যদি ((VIsual_active && buf == curwin->w_বাফার)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, to;

	যদি (VIsual_active)
	{
	    যদি (VIsual_mode != wp->w_old_visual_mode
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		যদি (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = VIsual.lnum;
		}
		অন্যথা
		{
		    from = VIsual.lnum;
		    to = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		যদি (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		যদি (wp->w_old_cursor_lnum > to)
		    to = wp->w_old_cursor_lnum;
		যদি (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		যদি (wp->w_old_visual_lnum > to)
		    to = wp->w_old_visual_lnum;
	    }
	    অন্যথা
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		যদি (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = wp->w_old_cursor_lnum;
		}
		অন্যথা
		{
		    from = wp->w_old_cursor_lnum;
		    to = curwin->w_cursor.lnum;
		    যদি (from == 0)	// Visual mode just started
			from = to;
		}

		যদি (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.col != wp->w_old_visual_col)
		{
		    যদি (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    যদি (wp->w_old_visual_lnum > to)
			to = wp->w_old_visual_lnum;
		    যদি (VIsual.lnum < from)
			from = VIsual.lnum;
		    যদি (VIsual.lnum > to)
			to = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    যদি (VIsual_mode == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_END_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		যদি (curwin->w_curswant == MAXCOL)
		{
		    যদি (get_ve_flags() & VE_BLOCK)
		    {
			pos_T	    pos;
			পূর্ণসংখ্যা	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			জন্য (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.col = (পূর্ণসংখ্যা)ml_get_buf_len(wp->w_বাফার, pos.lnum);
			    getvvcol(wp, &pos, ফাঁকা, ফাঁকা, &t, 0);
			    যদি (toc < t)
				toc = t;
			}
			++toc;
		    }
		    অন্যথা
			toc = MAXCOL;
		}

		যদি (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    যদি (from > VIsual.lnum)
			from = VIsual.lnum;
		    যদি (to < VIsual.lnum)
			to = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	অন্যথা
	{
	    // Use the line numbers of the old Visual area.
	    যদি (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		to = wp->w_old_visual_lnum;
	    }
	    অন্যথা
	    {
		from = wp->w_old_visual_lnum;
		to = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	যদি (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	যদি (wp->w_valid & VALID_BOTLINE)
	{
	    from = MIN(from, wp->w_botline);
	    to = MIN(to, wp->w_botline);
	}

	// Find the minimal part to be updated.
	// Watch out for scrolling that made entries in w_lines[] invalid.
	// E.g., CTRL-U makes the first half of w_lines[] invalid and sets
	// top_end; need to redraw from top_end to the "to" line.
	// A middle mouse click with a Visual selection may change the text
	// above the Visual area and reset wl_valid, do count these for
	// mid_end (in srow).
	যদি (mid_start > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    যদি (scrolled_down)
		mid_start = শীর্ষ_end;
	    অন্যথা
		mid_start = 0;
	    যখন (lnum < from && idx < wp->w_সারিগুলি_valid)	// find start
	    {
		যদি (wp->w_সারিগুলি[idx].wl_valid)
		    mid_start += wp->w_সারিগুলি[idx].wl_আকার;
		অন্যথা যদি (!scrolled_down)
		    srow += wp->w_সারিগুলি[idx].wl_আকার;
		++idx;
#ifdef FEAT_FOLDING
		যদি (idx < wp->w_সারিগুলি_valid && wp->w_সারিগুলি[idx].wl_valid)
		    lnum = wp->w_সারিগুলি[idx].wl_lnum;
		অন্যথা
#endif
		    ++lnum;
	    }
	    srow += mid_start;
	    mid_end = wp->w_height;
	    জন্য ( ; idx < wp->w_সারিগুলি_valid; ++idx)		// find end
	    {
		যদি (wp->w_সারিগুলি[idx].wl_valid
			&& wp->w_সারিগুলি[idx].wl_lnum >= to + 1)
		{
		    // Only update until first row of this line
		    mid_end = srow;
		    বিরতি;
		}
		srow += wp->w_সারিগুলি[idx].wl_আকার;
	    }
	}
    }

    যদি (VIsual_active && buf == curwin->w_বাফার)
    {
	wp->w_old_visual_mode = VIsual_mode;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_col = VIsual.col;
	wp->w_old_curswant = curwin->w_curswant;
    }
    অন্যথা
    {
	wp->w_old_visual_mode = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_col = 0;
    }

#যদি defined(FEAT_SYN_HL) || defined(FEAT_অনুসন্ধান_EXTRA)
    // reset got_int, otherwise regexp won't work
    save_got_int = got_int;
    got_int = 0;
#endif
#ifdef SYN_TIME_LIMIT
    // Set the time limit to 'redrawtime'.
    redrawtime_limit_set = TRUE;
    init_regexp_timeout(p_rdt);
#endif
#ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
#endif

#ifdef FEAT_MENU
    // Draw the window toolbar, if there is one.
    // TODO: only when needed.
    যদি (winbar_height(wp) > 0)
	redraw_win_toolbar(wp);
#endif

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#ifdef FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_FIELD(spv);
    যদি (spell_check_window(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_শীর্ষ == 0;
    }
#endif

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    row = 0;
    srow = 0;
    জন্য (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	যদি (row == wp->w_height)
	{
	    didline = TRUE;
	    বিরতি;
	}

	// stop updating when hit the end of the file
	যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
	{
	    eof = TRUE;
	    বিরতি;
	}

	// Remember the starting row of the line that is going to be dealt
	// with.  It is used further down when the line doesn't fit.
	srow = row;

	// Update a line when it is in an area that needs updating, when it
	// has changes or w_lines[idx] is invalid.
	// "bot_start" may be halfway a wrapped line after using
	// win_del_lines(), check if the current line includes it.
	// When syntax folding is being used, the saved syntax states will
	// already have been updated, we can't see where the syntax state is
	// the same again, just update until the end of the window.
	যদি (row < শীর্ষ_end
		|| (row >= mid_start && row < mid_end)
#ifdef FEAT_অনুসন্ধান_EXTRA
		|| শীর্ষ_to_mod
#endif
		|| idx >= wp->w_সারিগুলি_valid
		|| (row + wp->w_সারিগুলি[idx].wl_আকার > bot_start)
		|| (mod_শীর্ষ != 0
		    && (lnum == mod_শীর্ষ
			|| (lnum >= mod_শীর্ষ
			    && (lnum < mod_bot
#ifdef FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_সারি
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_changed(lnum)))
#endif
#ifdef FEAT_অনুসন্ধান_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_match_শীর্ষ != ফাঁকা
						    && buf->b_mod_set
						    && buf->b_mod_xlines != 0)
#endif
				))))
#ifdef FEAT_SYN_HL
		|| (wp->w_p_cul && lnum == wp->w_cursor.lnum)
		|| lnum == wp->w_last_cursorline
#endif
				)
	{
#ifdef FEAT_অনুসন্ধান_EXTRA
	    যদি (lnum == mod_শীর্ষ)
		শীর্ষ_to_mod = FALSE;
#endif

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    যদি (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_শীর্ষ && lnum < MAX(mod_bot, mod_শীর্ষ + 1)
		    && (!scrolled_down || row >= শীর্ষ_end))
	    {
		scrolled_for_mod = TRUE;

		পূর্ণসংখ্যা		old_cline_height = 0;
		পূর্ণসংখ্যা		old_rows = 0;
		পূর্ণসংখ্যা		new_rows = 0;
		পূর্ণসংখ্যা		xtra_rows;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		জন্য (i = idx; i < wp->w_সারিগুলি_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    যদি (wp->w_সারিগুলি[i].wl_valid
			    && wp->w_সারিগুলি[i].wl_lnum == mod_bot)
			বিরতি;
		    যদি (wp->w_সারিগুলি[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_height = wp->w_সারিগুলি[i].wl_আকার;
		    old_rows += wp->w_সারিগুলি[i].wl_আকার;
#ifdef FEAT_FOLDING
		    যদি (wp->w_সারিগুলি[i].wl_valid
			    && wp->w_সারিগুলি[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			যখন (i < wp->w_সারিগুলি_valid
						  && !wp->w_সারিগুলি[i].wl_valid)
			    old_rows += wp->w_সারিগুলি[i++].wl_আকার;
			বিরতি;
		    }
#endif
		}

		যদি (i >= wp->w_সারিগুলি_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_start = 0;
		}
		অন্যথা
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    জন্য (l = lnum; l < mod_bot; ++l)
		    {
			যদি (dollar_vcol >= 0 && wp == curwin &&
				old_cline_height > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_rows += old_cline_height;
#ifdef FEAT_FOLDING
			অন্যথা যদি (hasFoldingWin(wp, l, ফাঁকা, &l, TRUE, ফাঁকা))
			    ++new_rows;
#endif
			অন্যথা
			    new_rows += plines_correct_topline(wp, l, TRUE);
			++j;
			যদি (new_rows > wp->w_height - row - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_rows = 9999;
			    বিরতি;
			}
		    }
		    xtra_rows = new_rows - old_rows;
		    যদি (xtra_rows < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			যদি (row - xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			অন্যথা
			{
			    check_for_delay(FALSE);
			    যদি (win_del_সারিগুলি(wp, row,
					  -xtra_rows, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    অন্যথা
				bot_start = wp->w_height + xtra_rows;
			}
		    }
		    অন্যথা যদি (xtra_rows > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			যদি (row + xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			অন্যথা
			{
			    check_for_delay(FALSE);
			    যদি (win_ins_সারিগুলি(wp, row + old_rows,
					     xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    অন্যথা যদি (শীর্ষ_end > row + old_rows)
				// Scrolled the part at the top that requires
				// updating down.
				শীর্ষ_end += xtra_rows;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    যদি (mod_bot != MAXLNUM && i != j)
		    {
			যদি (j < i)
			{
			    পূর্ণসংখ্যা x = row + new_rows;

			    // move entries in w_lines[] upwards
			    জন্য (;;)
			    {
				// stop at last valid entry in w_lines[]
				যদি (i >= wp->w_সারিগুলি_valid)
				{
				    wp->w_সারিগুলি_valid = j;
				    বিরতি;
				}
				wp->w_সারিগুলি[j] = wp->w_সারিগুলি[i];
				// stop at a line that won't fit
				যদি (x + (পূর্ণসংখ্যা)wp->w_সারিগুলি[j].wl_আকার
							   > wp->w_height)
				{
				    wp->w_সারিগুলি_valid = j + 1;
				    বিরতি;
				}
				x += wp->w_সারিগুলি[j++].wl_আকার;
				++i;
			    }
			    যদি (bot_start > x)
				bot_start = x;
			}
			অন্যথা // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_সারিগুলি_valid += j;
			    যদি (wp->w_সারিগুলি_valid > wp->w_height)
				wp->w_সারিগুলি_valid = wp->w_height;
			    জন্য (i = wp->w_সারিগুলি_valid; i - j >= idx; --i)
				wp->w_সারিগুলি[i] = wp->w_সারিগুলি[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    যখন (i >= idx)
			    {
				wp->w_সারিগুলি[i].wl_আকার = 0;
				wp->w_সারিগুলি[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ifdef FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_গুনতি = folded_গুনতি(wp, lnum, &win_foldinfo);
	    যদি (fold_গুনতি != 0)
	    {
		fold_সারি(wp, fold_গুনতি, &win_foldinfo, lnum, row);
		++row;
		--fold_গুনতি;
		wp->w_সারিগুলি[idx].wl_folded = TRUE;
		wp->w_সারিগুলি[idx].wl_lastlnum = lnum + fold_গুনতি;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
# ifdef FEAT_SPELL
		spv.spv_capcol_lnum = 0;
# endif
	    }
	    অন্যথা
#endif
	    যদি (idx < wp->w_সারিগুলি_valid
		    && wp->w_সারিগুলি[idx].wl_valid
		    && wp->w_সারিগুলি[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_সারিগুলি[idx].wl_আকার > wp->w_height
#ifdef FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#endif
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		row = wp->w_height + 1;
	    }
	    অন্যথা
	    {
#ifdef FEAT_অনুসন্ধান_EXTRA
		prepare_অনুসন্ধান_hl(wp, &screen_অনুসন্ধান_hl, lnum);
#endif
#ifdef FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		যদি (syntax_last_parsed != 0 && syntax_last_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

		// Display one line.
		row = win_সারি(wp, lnum, srow, wp->w_height, 0, &spv);

#ifdef FEAT_FOLDING
		wp->w_সারিগুলি[idx].wl_folded = FALSE;
		wp->w_সারিগুলি[idx].wl_lastlnum = lnum;
#endif
#ifdef FEAT_SYN_HL
		did_update = DID_সারি;
		syntax_last_parsed = lnum;
#endif
	    }

	    wp->w_সারিগুলি[idx].wl_lnum = lnum;
	    wp->w_সারিগুলি[idx].wl_valid = TRUE;

	    পূর্ণসংখ্যা is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    যদি (row > wp->w_height || row + wp->w_winrow >= Rows)
	    {
		// we may need the size of that too long line later on
		যদি (dollar_vcol == -1 || !is_curline)
		    wp->w_সারিগুলি[idx].wl_আকার = plines_win(wp, lnum, TRUE);
		++idx;
		বিরতি;
	    }
	    যদি (dollar_vcol == -1 || !is_curline)
		wp->w_সারিগুলি[idx].wl_আকার = row - srow;
	    ++idx;
#ifdef FEAT_FOLDING
	    lnum += fold_গুনতি + 1;
#অন্যথা
	    ++lnum;
#endif
	}
	অন্যথা
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    যদি ((wp->w_p_nu && mod_শীর্ষ != 0 && lnum >= mod_bot
			&& buf->b_mod_set && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_last_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#ifdef FEAT_FOLDING
		fold_গুনতি = folded_গুনতি(wp, lnum, &win_foldinfo);
		যদি (fold_গুনতি != 0)
		    fold_সারি(wp, fold_গুনতি, &win_foldinfo, lnum, row);
		অন্যথা
#endif
		    (শূন্য)win_সারি(wp, lnum, srow, wp->w_height,
					       wp->w_সারিগুলি[idx].wl_আকার, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    row += wp->w_সারিগুলি[idx++].wl_আকার;
	    যদি (row > wp->w_height)	// past end of screen
		বিরতি;
#ifdef FEAT_FOLDING
	    lnum = wp->w_সারিগুলি[idx - 1].wl_lastlnum + 1;
#অন্যথা
	    ++lnum;
#endif
#ifdef FEAT_SYN_HL
	    did_update = DID_NONE;
#endif
#ifdef FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#endif
	}

	যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
	{
	    eof = TRUE;
	    বিরতি;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	যদি (idx >= Rows)
	    বিরতি;
    }

    // End of loop over all window lines.

#ifdef FEAT_SYN_HL
    // Now that the window has been redrawn with the old and new cursor line,
    // update w_last_cursorline.
    wp->w_last_cursorline = wp->w_p_cul ? wp->w_cursor.lnum : 0;
#endif
    wp->w_last_cursor_lnum_rnu = wp->w_p_rnu ? wp->w_cursor.lnum : 0;

#ifdef FEAT_VTP
    // Rewrite the character at the end of the screen line.
    // See the version that was fixed.
    যদি (use_vtp() && get_conpty_fix_type() < 1)
    {
	পূর্ণসংখ্যা k;

	জন্য (k = 0; k < Rows; ++k)
	    যদি (enc_utf8)
		যদি ((*mb_off2cells)(সারি_Offset[k] + topframe->fr_width - 2,
					   সারি_Offset[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_width - 2, 1, 2,
			    FALSE, FALSE);
		অন্যথা
		    screen_draw_rectangle(k, topframe->fr_width - 1, 1, 1,
			    FALSE, FALSE);
	    অন্যথা
		screen_char(সারি_Offset[k] + topframe->fr_width - 1, k,
			cmdline_width - 1);
    }
#endif

    যদি (idx > wp->w_সারিগুলি_valid)
	wp->w_সারিগুলি_valid = idx;

#ifdef FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    যদি (syntax_last_parsed != 0 && syntax_present(wp))
	syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_empty_rows = 0;
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    যদি (!eof && !didline)
    {
	যদি (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#ifdef FEAT_DIFF
	অন্যথা যদি (diff_check_fill(wp, lnum) >= wp->w_height - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_rows = wp->w_height - srow;
	}
#endif
#ifdef FEAT_PROP_POPUP
	অন্যথা যদি (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#endif
	অন্যথা যদি (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    পূর্ণসংখ্যা		scr_row = W_WINROW(wp) + wp->w_height - 1;
	    পূর্ণসংখ্যা		প্রতীক  = wp->w_fill_chars.lastline;
	    পূর্ণসংখ্যা		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(প্রতীক, &fillbuf[0]);
	    mb_char2bytes(প্রতীক, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_width > 2 ? 2 : wp->w_width) * charlen,
			    scr_row, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_row, scr_row + 1,
		    (পূর্ণসংখ্যা)wp->w_wincol + 2, (পূর্ণসংখ্যা)W_ENDCOL(wp),
		    প্রতীক, ' ', HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	অন্যথা যদি (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    পূর্ণসংখ্যা start_col = (পূর্ণসংখ্যা)W_ENDCOL(wp) - 3;
	    পূর্ণসংখ্যা প্রতীক    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_height - 1,
		    W_WINROW(wp) + wp->w_height,
		    (start_col < wp->w_wincol ? wp->w_wincol : start_col),
		    (পূর্ণসংখ্যা)W_ENDCOL(wp),
		    প্রতীক, প্রতীক, HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	অন্যথা
	{
	    win_draw_end(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_height, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    অন্যথা
    {
	draw_vsep_win(wp, row);
	যদি (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_সারি_গুনতি + 1;
#ifdef FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    যদি (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		যদি (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		অন্যথা
		    i = wp->w_fill_chars.diff;
		যদি (row + j > wp->w_height)
		    j = wp->w_height - row;
		win_draw_end(wp, i, i, TRUE, row, row + (পূর্ণসংখ্যা)j, HLF_DED);
		row += j;
	    }
#endif
	}
	অন্যথা যদি (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	যদি (WIN_IS_POPUP(wp))
	    win_draw_end(wp, ' ', ' ', FALSE, row, wp->w_height, HLF_AT);
	অন্যথা
	    win_draw_end(wp, wp->w_fill_chars.eob, ' ', FALSE,
						   row, wp->w_height, HLF_EOB);
    }

#ifdef SYN_TIME_LIMIT
    disable_regexp_timeout();
    redrawtime_limit_set = FALSE;
#endif

    // Reset the type of redrawing required, the window has been updated.
    wp->w_redr_type = 0;
#ifdef FEAT_DIFF
    wp->w_old_topfill = wp->w_topfill;
    wp->w_old_botfill = wp->w_botfill;
#endif

    যদি (dollar_vcol == -1 || wp != curwin)
    {
	// There is a trick with w_botline.  If we invalidate it on each
	// change that might modify it, this will cause a lot of expensive
	// calls to plines() in update_topline() each time.  Therefore the
	// value of w_botline is often approximated, and this value is used to
	// compute the value of w_topline.  If the value of w_botline was
	// wrong, check that the value of w_topline is correct (cursor is on
	// the visible part of the text).  If it's not, we need to redraw
	// again.  Mostly this just means scrolling up a few lines, so it
	// doesn't look too bad.  Only do this for the current window (where
	// changes are relevant).
	wp->w_valid |= VALID_BOTLINE;
	যদি (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#যদি defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    পূর্ণসংখ্যা		new_wcol = wp->w_wcol;
#endif
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#যদি defined(FEAT_CONCEAL)
	    যদি (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
						    != (VALID_WCOL|VALID_WROW))
	    {
		// A win_line() call applied a fix to screen cursor column to
		// accommodate concealment of cursor line, but in this call to
		// update_topline() the cursor's row or column got invalidated.
		// If they are left invalid, setcursor() will recompute them
		// but there won't be any further win_line() call to re-fix the
		// column and the cursor will end up misplaced.  So we call
		// cursor validation now and reapply the fix again (or call
		// win_line() to do it for us).
		validate_cursor();
		যদি (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		অন্যথা
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#endif
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    যদি (wp->w_redr_type != 0)
	    {
		// Don't update for changes in buffer again.
		bool b = curbuf->b_mod_set;
		curbuf->b_mod_set = false;
		j = curbuf->b_mod_xlines;
		curbuf->b_mod_xlines = 0;
		curs_columns(TRUE);
		win_update(curwin);
		curbuf->b_mod_set = b;
		curbuf->b_mod_xlines = j;
	    }
	    // Other windows might have w_redr_type raised in update_topline().
	    must_redraw = 0;
	    FOR_ALL_WINDOWS(wwp)
		যদি (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#যদি defined(FEAT_SYN_HL) || defined(FEAT_অনুসন্ধান_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    যদি (!got_int)
	got_int = save_got_int;
#endif

    যদি (override_success)
	বের_করো_highlight_overrides();
}

#যদি defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    স্থির শূন্য
update_prepare(শূন্য)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    যদি (gui.in_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_অনুসন্ধান_EXTRA
    start_অনুসন্ধান_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_mask(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    স্থির শূন্য
update_finish(শূন্য)
{
    যদি (redraw_cmdline || redraw_mode)
	showmode();

# ifdef FEAT_অনুসন্ধান_EXTRA
    end_অনুসন্ধান_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    যদি (gui.in_use)
    {
	out_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#endif

#যদি defined(FEAT_NETBEANS_INTG)
    শূন্য
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    পূর্ণসংখ্যা		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_সারি_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    যদি (!doit || updating_screen
	    || State == MODE_ASKMORE || State == MODE_HITRETURN
	    || বার্তা_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	ফেরত;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	যদি (wp->w_redr_type != 0)
	    win_update(wp);
	যদি (wp->w_redr_status)
	    win_redr_status(wp, FALSE);
    }

# যদি defined(FEAT_TABPANEL)
    যদি (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_finish();
}
#endif

#যদি defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    শূন্য
updateWindow(win_T *wp)
{
    // return if already busy updating
    যদি (updating_screen)
	ফেরত;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    যদি (clip_star.available && clip_isautosel_star())
	clip_update_selection(&clip_star);
    যদি (clip_plus.available && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    যদি (redraw_tabline)
	draw_tabline();

# যদি defined(FEAT_TABPANEL)
    যদি (redraw_tabpanel)
	draw_tabpanel();
# endif

    যদি (wp->w_redr_status || p_ru
# ifdef FEAT_STL_OPT
	    || *p_stl != NUL || *wp->w_p_stl != NUL
# endif
	    )
	win_redr_status(wp, FALSE);

# ifdef FEAT_PROP_POPUP
    // Display popup windows on top of everything.
    update_popups(win_update);
# endif

    update_finish();
}
#endif

/*
 * Redraw as soon as possible.  When the command line is not scrolled redraw
 * right away and restore what was on the command line.
 * Return a code indicating what happened.
 */
    পূর্ণসংখ্যা
redraw_asap(পূর্ণসংখ্যা type)
{
    পূর্ণসংখ্যা		rows;
    পূর্ণসংখ্যা		cols = screen_Columns;
    পূর্ণসংখ্যা		r;
    পূর্ণসংখ্যা		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    পূর্ণসংখ্যা		i;
    u8char_T	*screenlineUC = ফাঁকা;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = ফাঁকা;	// copy from ScreenLines2[]

    redraw_later(type);
    যদি (বার্তা_scrolled
	    || (State != MODE_NORMAL && State != MODE_NORMAL_BUSY)
	    || exiting)
	ফেরত ret;

    // Allocate space to save the text displayed in the command line area.
    rows = screen_Rows - cmdline_row;
    screenline = LALLOC_MULT(schar_T, rows * cols);
    screenattr = LALLOC_MULT(sattr_T, rows * cols);
    যদি (screenline == ফাঁকা || screenattr == ফাঁকা)
	ret = 2;
    যদি (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, rows * cols);
	যদি (screenlineUC == ফাঁকা)
	    ret = 2;
	জন্য (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, rows * cols);
	    যদি (screenlineC[i] == ফাঁকা)
		ret = 2;
	}
    }
    যদি (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, rows * cols);
	যদি (screenline2 == ফাঁকা)
	    ret = 2;
    }

    যদি (ret != 2)
    {
	// Save the text displayed in the command line area.
	জন্য (r = 0; r < rows; ++r)
	{
	    mch_memmove(screenline + r * cols,
			Screen_সারিগুলি + সারি_Offset[cmdline_row + r],
			(size_t)cols * আকার(schar_T));
	    mch_memmove(screenattr + r * cols,
			ScreenAttrs + সারি_Offset[cmdline_row + r],
			(size_t)cols * আকার(sattr_T));
	    যদি (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * cols,
			    Screen_সারিগুলি_UC + সারি_Offset[cmdline_row + r],
			    (size_t)cols * আকার(u8char_T));
		জন্য (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * cols,
				Screen_সারিগুলি_C[i] + সারি_Offset[cmdline_row + r],
				(size_t)cols * আকার(u8char_T));
	    }
	    যদি (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * cols,
			    ScreenLines2 + সারি_Offset[cmdline_row + r],
			    (size_t)cols * আকার(schar_T));
	}

	update_screen(0);
	ret = 3;

	যদি (must_redraw == 0)
	{
	    পূর্ণসংখ্যা	off = (পূর্ণসংখ্যা)(current_Screen_সারি - Screen_সারিগুলি);

	    // Restore the text displayed in the command line area.
	    জন্য (r = 0; r < rows; ++r)
	    {
		mch_memmove(current_Screen_সারি,
			    screenline + r * cols,
			    (size_t)cols * আকার(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * cols,
			    (size_t)cols * আকার(sattr_T));
		যদি (enc_utf8)
		{
		    mch_memmove(Screen_সারিগুলি_UC + off,
				screenlineUC + r * cols,
				(size_t)cols * আকার(u8char_T));
		    জন্য (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_সারিগুলি_C[i] + off,
				    screenlineC[i] + r * cols,
				    (size_t)cols * আকার(u8char_T));
		}
		যদি (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * cols,
				(size_t)cols * আকার(schar_T));
		screen_সারি(curwin, cmdline_row + r, 0, cols, cols, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    যদি (enc_utf8)
    {
	vim_free(screenlineUC);
	জন্য (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    যদি (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_message();

    setcursor();

    ফেরত ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    শূন্য
redraw_after_callback(পূর্ণসংখ্যা call_update_screen, পূর্ণসংখ্যা do_message)
{
    ++redrawing_for_callback;

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

    যদি (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_SETWSIZE || State == MODE_EXTERNCMD
	    || State == MODE_CONFIRM || exmode_active)
    {
	যদি (do_message)
	    repeat_message();
    }
    অন্যথা যদি (State & MODE_CMDLINE)
    {
	যদি (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	যদি (cmdline_row > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    যদি (বার্তা_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    অন্যথা যদি (State & (MODE_NORMAL | MODE_ঢোকাও | MODE_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	যদি (বার্তা_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    বার্তা_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
#ifdef FEAT_GUI
    যদি (gui.in_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	out_flush_cursor(FALSE, FALSE);
    অন্যথা
#endif
	out_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    শূন্য
redraw_later(পূর্ণসংখ্যা type)
{
    redraw_win_later(curwin, type);
}

    শূন্য
redraw_win_later(
    win_T	*wp,
    পূর্ণসংখ্যা		type)
{
#ifdef FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    যদি (inside_redraw_on_start_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    অন্যথা
#endif
    যদি (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	যদি (type >= UPD_NOT_VALID)
	    wp->w_সারিগুলি_valid = 0;
	যদি (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    শূন্য
redraw_later_clear(শূন্য)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    শূন্য
redraw_all_later(পূর্ণসংখ্যা type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    set_must_redraw(type);
}

#যদি 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    শূন্য
redraw_all_windows_later(পূর্ণসংখ্যা type)
{
    redraw_all_later(type);
# ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
# endif
}
#endif

/*
 * Set "must_redraw" to "type" unless it already has a higher value
 * or it is currently not allowed.
 */
    শূন্য
set_must_redraw(পূর্ণসংখ্যা type)
{
    যদি (!redraw_not_allowed &&
#ifdef FEAT_EVAL
	    !inside_redraw_on_start_cb &&
#endif
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    শূন্য
redraw_curbuf_later(পূর্ণসংখ্যা type)
{
    redraw_buf_later(curbuf, type);
}

    শূন্য
redraw_buf_later(buf_T *buf, পূর্ণসংখ্যা type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	যদি (wp->w_বাফার == buf)
	    redraw_win_later(wp, type);
    }
#ifdef FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	যদি (wp->w_বাফার == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	যদি (wp->w_বাফার == buf)
	    redraw_win_later(wp, type);
#endif
#যদি defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    যদি (curwin->w_বাফার == buf)
	redraw_win_later(curwin, type);
#endif
}

#যদি defined(FEAT_SIGNS)
    শূন্য
redraw_buf_সারি_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_বাফার == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#endif

#যদি defined(FEAT_JOB_CHANNEL)
    শূন্য
redraw_buf_and_status_later(buf_T *buf, পূর্ণসংখ্যা type)
{
    win_T	*wp;

    যদি (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	ফেরত;
    FOR_ALL_WINDOWS(wp)
    {
	যদি (wp->w_বাফার == buf)
	{
	    redraw_win_later(wp, type);
	    wp->w_redr_status = true;
	}
    }
}
#endif

/*
 * mark the ruler for redraw when the last window has no status line and the
 * ruler takes its place in the last screen line; showmode() draws it
 */
    স্থির শূন্য
ruler_redraw_lastwin(শূন্য)
{
    যদি (p_ru && lastwin->w_status_height == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    শূন্য
status_redraw_all(শূন্য)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_status_height)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    শূন্য
status_redraw_curbuf(শূন্য)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_status_height != 0 && wp->w_বাফার == curbuf)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    যদি (lastwin->w_বাফার == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    শূন্য
redraw_statuslines(শূন্য)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_redr_status)
	{
	    bool ret = ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_status(wp, FALSE);
	    যদি (ret)
		বের_করো_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    যদি (redraw_tabline)
	draw_tabline();

#যদি defined(FEAT_TABPANEL)
    যদি (redraw_tabpanel)
	draw_tabpanel();
#endif
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    শূন্য
win_redraw_last_status(frame_T *frp)
{
    যদি (frp->fr_layout == FR_LEAF)
	frp->fr_win->w_redr_status = true;
    অন্যথা যদি (frp->fr_layout == FR_ROW)
    {
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    win_redraw_last_status(frp);
    }
    অন্যথা // frp->fr_layout == FR_COL
    {
	frp = frp->fr_child;
	যখন (frp->fr_পরবর্তী != ফাঁকা)
	    frp = frp->fr_পরবর্তী;
	win_redraw_last_status(frp);
    }
}

/*
 * Changed something in the current window, at buffer line "lnum", that
 * requires that line and possibly other lines to be redrawn.
 * Used when entering/leaving Insert mode with the cursor on a folded line.
 * Used to remove the "$" from a change command.
 * Note that when also inserting/deleting lines w_redraw_top and w_redraw_bot
 * may become invalid and the whole window will have to be redrawn.
 */
    শূন্য
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    শূন্য
redraw_win_range_later(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    যদি (last >= wp->w_topline && first < wp->w_botline)
    {
	যদি (wp->w_redraw_শীর্ষ == 0 || wp->w_redraw_শীর্ষ > first)
	    wp->w_redraw_শীর্ষ = first;
	যদি (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    শূন্য
redraw_win_range_now(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    যদি (last >= wp->w_topline && first < wp->w_botline)
    {
	যদি (wp->w_redraw_শীর্ষ == 0 || wp->w_redraw_শীর্ষ > first)
	    wp->w_redraw_শীর্ষ = first;
	যদি (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	যদি (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#ifdef FEAT_EVAL
স্থির bool redraw_cb_in_progress = false;

    শূন্য
f_redraw_listener_add(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    স্থির পূর্ণসংখ্যা		id;

    যদি (check_secure())
	ফেরত;

    যদি (redraw_cb_in_progress)
    {
	emsg(_(e_cannot_add_redraw_listener_in_listener_callback));
	ফেরত;
    }

    যদি (check_for_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    যদি (rln == ফাঁকা)
	ফেরত;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    যদি (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	যদি (cb.cb_name == ফাঁকা)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    ফেরত;
	}
	copy_callback(&rln->rl_callbacks.on_start, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    যদি (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	যদি (cb.cb_name == ফাঁকা)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_start);
	    vim_free(rln);
	    ফেরত;
	}
	copy_callback(&rln->rl_callbacks.on_end, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    যদি (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	ফেরত;
    }

    rln->rl_পরবর্তী = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_সংখ্যা;
    rettv->vval.v_সংখ্যা = id;

    ফেরত;
}

    স্থির শূন্য
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_start);
    free_callback(&rln->rl_callbacks.on_end);

    vim_free(rln);
}


    স্থির শূন্য
redraw_listener_cleanup(শূন্য)
{
    জন্য (redraw_listener_T *rln = redraw_listeners; rln != ফাঁকা;)
    {
	redraw_listener_T *পরবর্তী = rln->rl_পরবর্তী;
	যদি (rln->rl_id == 0)
	{
	    যদি (redraw_listeners == rln)
		redraw_listeners = rln->rl_পরবর্তী;
	    redraw_listener_free(rln);
	}
	rln = পরবর্তী;
    }
}

/*
 * Return the redraw listener struct with the specified id. Returns NULL if not
 * found.
 */
    স্থির redraw_listener_T *
get_redraw_listener(পূর্ণসংখ্যা id)
{
    জন্য (redraw_listener_T *rln = redraw_listeners; rln != ফাঁকা; rln = rln->rl_পরবর্তী)
	যদি (rln->rl_id == id)
	    ফেরত rln;
    ফেরত ফাঁকা;
}

    শূন্য
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv UNUSED)
{
    পূর্ণসংখ্যা			id;
    redraw_listener_T	*rln;

    যদি (check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    id = argvars[0].vval.v_সংখ্যা;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_সংখ্যা;
    যদি (rln == ফাঁকা)
    {
	rettv->vval.v_সংখ্যা = 0;
	ফেরত;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_সংখ্যা = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    স্থির শূন্য
invoke_redraw_listener_start_or_end(bool start)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    যদি (start)
	inside_redraw_on_start_cb = true;

    redraw_cb_in_progress = true;
    জন্য (redraw_listener_T *rln = redraw_listeners; rln != ফাঁকা; rln = rln->rl_পরবর্তী)
    {
	যদি (rln->rl_id == 0)
	    // Listener has been removed, skip
	    চালু;
	যদি (start && rln->rl_callbacks.on_start.cb_name != ফাঁকা)
	{
	    call_callback(&rln->rl_callbacks.on_start, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	অন্যথা যদি (rln->rl_callbacks.on_end.cb_name != ফাঁকা)
	{
	    call_callback(&rln->rl_callbacks.on_end, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_in_progress = false;

    যদি (start)
	inside_redraw_on_start_cb = false;
}
#endif // FEAT_EVAL
