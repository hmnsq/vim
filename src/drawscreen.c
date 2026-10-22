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

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಶೂನ್ಯ win_update(win_T *wp);
#ifdef FEAT_STL_OPT
ಸ್ಥಿರ ಶೂನ್ಯ redraw_custom_statusline(win_T *wp);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ borrow_stl_vsep_hl(ಶೂನ್ಯ);
#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_CLIPBOARD)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  did_update_one_window;
#endif

#ifdef FEAT_EVAL
ಸ್ಥಿರ ಶೂನ್ಯ redraw_listener_cleanup(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ invoke_redraw_listener_start_or_end(bool start);
#endif

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    ಪೂರ್ಣಾಙ್ಕ
update_screen(ಪೂರ್ಣಾಙ್ಕ type_arg)
{
    ಪೂರ್ಣಾಙ್ಕ		type = type_arg;
    win_T	*wp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_intro = FALSE;
#ifdef FEAT_GUI
    ಪೂರ್ಣಾಙ್ಕ		did_one = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		did_undraw = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		gui_cursor_col = 0;
    ಪೂರ್ಣಾಙ್ಕ		gui_cursor_row = 0;
#endif
    ಪೂರ್ಣಾಙ್ಕ		no_update = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		save_pum_will_redraw = pum_will_redraw;
#ifdef FEAT_PROP_POPUP
    ಪೂರ್ಣಾಙ್ಕ		did_redraw_window = FALSE;
#endif
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    ಯದಿ (!screen_valid(TRUE))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (type == UPD_VALID_NO_UPDATE)
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
    ಯದಿ (need_diff_redraw)
	diff_redraw(TRUE);
#endif

    ಯದಿ (must_redraw)
    {
	ಯದಿ (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    ಯದಿ (curwin->w_ಸಾಲುಗಳು_valid == 0 && type < UPD_NOT_VALID
#ifdef FEAT_TERMINAL
	    && !term_do_update_window(curwin)
#endif
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    ಯದಿ (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	ಯದಿ (type > UPD_INVERTED_ALL)
	    curwin->w_ಸಾಲುಗಳು_valid = 0;	// don't use w_lines[].wl_size now
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    ಪೂರ್ಣಾಙ್ಕ hid_cursor = !sync_output_active();
    ಯದಿ (hid_cursor)
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
    ಯದಿ (no_update)
	++no_win_do_ಸಾಲುಗಳು_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    ಯದಿ (ಸಂದೇಶ_scrolled)
    {
	clear_cmdline = TRUE;
	ಯದಿ (type != UPD_CLEAR)
	{
	    ಯದಿ (ಸಂದೇಶ_scrolled > Rows - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    ಅನ್ಯಥಾ
	    {
		check_for_delay(FALSE);
		ಯದಿ (screen_ins_ಸಾಲುಗಳು(0, 0, ಸಂದೇಶ_scrolled, (ಪೂರ್ಣಾಙ್ಕ)Rows, 0, ರಿಕ್ತ)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    ಯದಿ (wp->w_winrow < ಸಂದೇಶ_scrolled)
		    {
			ಯದಿ (W_WINROW(wp) + wp->w_height > ಸಂದೇಶ_scrolled
				&& wp->w_redr_type < UPD_REDRAW_ತುದಿ
				&& wp->w_ಸಾಲುಗಳು_valid > 0
				&& wp->w_topline == wp->w_ಸಾಲುಗಳು[0].wl_lnum)
			{
			    wp->w_upd_rows = ಸಂದೇಶ_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_ತುದಿ;
			}
			ಅನ್ಯಥಾ
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    ಯದಿ (W_WINROW(wp) + wp->w_height
					 + wp->w_status_height <= ಸಂದೇಶ_scrolled)
				wp->w_redr_status = true;
			}
		    }
		}
		ಯದಿ (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#ಯದಿ defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#endif
	}
	ಸಂದೇಶ_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    ಯದಿ (need_highlight_changed)
	highlight_changed();

    ಯದಿ (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    ಯದಿ (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    ಯದಿ (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? ಸಂಖ್ಯೆ_width(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#endif

#ifdef FEAT_EVAL
    invoke_redraw_listener_start_or_end(true);
#endif

    // Only start redrawing if there is really something to do.
    ಯದಿ (type == UPD_INVERTED)
	update_curswant();
    ಯದಿ (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_ಸಾಲುಗಳು[0].wl_valid
#ifdef FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#endif
		    && curwin->w_topline == curwin->w_ಸಾಲುಗಳು[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_active
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_mode == VIsual_mode
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    ಯದಿ (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#ಯದಿ defined(FEAT_TABPANEL)
    ಯದಿ (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#endif

#ifdef FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ (wp->w_buffer->b_mod_set)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    ಪ್ರತಿ (wwp = firstwin; wwp != wp; wwp = wwp->w_next)
		ಯದಿ (wwp->w_buffer == wp->w_buffer)
		    ಭಙ್ಗ;
	    ಯದಿ (wwp == wp && syntax_present(wp))
		syn_ರಾಶಿ_apply_changes(wp->w_buffer);
	}
    }
#endif

    ಯದಿ (pum_redraw_in_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_window = FALSE;
#endif
#ifdef FEAT_ಹುಡುಕು_EXTRA
    screen_ಹುಡುಕು_hl.rm.regprog = ರಿಕ್ತ;
#endif
    FOR_ALL_WINDOWS(wp)
    {
	override_success = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ಯದಿ (wp->w_redr_type != 0)
	{
	    cursor_off();
#ifdef FEAT_PROP_POPUP
	    did_redraw_window = TRUE;
#endif
#ifdef FEAT_GUI
	    ಯದಿ (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		ಯದಿ (gui.in_use && (wp == curwin || cursor_is_sleeping()))
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
	ಯದಿ (wp->w_redr_status)
	{
	    cursor_off();
	    win_redr_status(wp, TRUE); // any popup menu will be redrawn below
	}

	ಯದಿ (override_success)
	    ತೆಗೆ_highlight_overrides();

    }
#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA)
    end_ಹುಡುಕು_hl();
#endif

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    ಯದಿ (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    ಯದಿ (wp->w_vsep_width > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_buffer->b_mod_set = false;

#ifdef FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    ಯದಿ (did_redraw_window || popup_need_redraw())
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
    ಯದಿ (clear_cmdline || redraw_cmdline || redraw_mode)
	showmode();

    ಯದಿ (no_update)
	--no_win_do_ಸಾಲುಗಳು_ins;

    // May put up an introductory message when not editing a file
    ಯದಿ (!did_intro)
	maybe_intro_message();
    did_intro = TRUE;

#ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    ಯದಿ (gui.in_use)
    {
	ಯದಿ (did_undraw && !gui_mch_is_blink_off())
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
	ಅನ್ಯಥಾ
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
    ಯದಿ (hid_cursor)
	cursor_on();

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    ಪೂರ್ಣಾಙ್ಕ
statusline_row(win_T *wp)
{
#ಯದಿ defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    ಯದಿ (wp->w_frame->fr_height == wp->w_status_height && !popup_is_popup(wp))
	ಪ್ರತ್ಯಾಗಮ wp->w_winrow;
#endif
    ಪ್ರತ್ಯಾಗಮ W_WINROW(wp) + wp->w_height;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    ಶೂನ್ಯ
win_redr_status(win_T *wp, ಪೂರ್ಣಾಙ್ಕ ignore_pum UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		fillchar;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    ಯದಿ (busy)
	ಪ್ರತ್ಯಾಗಮ;
    busy = TRUE;

    row = statusline_row(wp);

    wp->w_redr_status = false;
    ಯದಿ (wp->w_status_height == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_status = true;
    }
#ifdef FEAT_STL_OPT
    ಅನ್ಯಥಾ ಯದಿ (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#endif
    ಅನ್ಯಥಾ
    {
	char_u	*p;
	ಪೂರ್ಣಾಙ್ಕ	plen;
	ಪೂರ್ಣಾಙ್ಕ	NameBufflen;
	ಪೂರ್ಣಾಙ್ಕ	this_ru_col;
	ಪೂರ್ಣಾಙ್ಕ	n;			// scratch value

	fillchar = fillchar_status(&attr, wp);

	get_trans_bufname(wp->w_buffer);
	p = NameBuff;
	plen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);

	ಯದಿ ((bt_help(wp->w_buffer)
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
		    || bufIsChanged(wp->w_buffer)
		    || wp->w_buffer->b_p_ro)
		&& plen < MAXPATHL - 1)
	{
	    *(p + plen++) = ' ';	// replace NUL with space
	    *(p + plen) = NUL;		// NUL terminate the string
	}
	ಯದಿ (bt_help(wp->w_buffer))
	    plen += vim_snprintf((ವರ್ಣ *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#ifdef FEAT_QUICKFIX
	ಯದಿ (wp->w_p_pvw)
	    plen += vim_snprintf((ವರ್ಣ *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#endif
	ಯದಿ (bufIsChanged(wp->w_buffer) && !bt_terminal(wp->w_buffer))
	    plen += vim_snprintf((ವರ್ಣ *)p + plen, MAXPATHL - plen, "%s", "[+]");
	ಯದಿ (wp->w_buffer->b_p_ro)
	    plen += vim_snprintf((ವರ್ಣ *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_col = ru_col - (cmdline_width - wp->w_width);
	n = (wp->w_width + 1) / 2;
	ಯದಿ (this_ru_col < n)
	    this_ru_col = n;
	ಯದಿ (this_ru_col <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    ಪ್ರತಿ (i = 0; p[i] != NUL && plen >= this_ru_col - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    ಯದಿ (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (plen > this_ru_col - 1)
	{
	    p += plen - (this_ru_col - 1);
	    *p = '<';
	    plen = this_ru_col - 1;
	}

	screen_puts(p, row, wp->w_wincol, attr);
	screen_fill(row, row + 1, plen + wp->w_wincol,
		    this_ru_col + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	ಪ್ರತಿ (i = 1; i < wp->w_status_height; i++)
	    screen_fill(row + i, row + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	ಯದಿ ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_col - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, row, (ಪೂರ್ಣಾಙ್ಕ)(this_ru_col - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	ಯದಿ (p_sc && *p_sloc == 's')
	{
	    n = this_ru_col - plen - 2;		    // perform the calculation here so we only do it once
	    ಪೂರ್ಣಾಙ್ಕ	width = MIN(10, n);

	    ಯದಿ (width > 0)
		screen_puts_len(showcmd_buf, width, row,
				wp->w_wincol + this_ru_col - width - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    ಯದಿ (wp->w_vsep_width != 0 && wp->w_status_height != 0 && redrawing())
    {
	ಪ್ರತಿ (i = 0; i < wp->w_status_height; i++)
	{
	    ಪೂರ್ಣಾಙ್ಕ r = row + i;
	    ಯದಿ (stl_connected(wp))
		fillchar = fillchar_status(&attr, wp);
	    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
borrow_stl_vsep_hl(ಶೂನ್ಯ)
{
    win_T   *ಎಡ = ರಿಕ್ತ;
    win_T   *ಬಲ = ರಿಕ್ತ;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    ಯದಿ (!redrawing() || Screen_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    FOR_ALL_WINDOWS(ಎಡ)
    {
	ಯದಿ (ಎಡ->w_status_height == 0 || ಎಡ->w_vsep_width == 0)
	    ಅನುವರ್ತ;
	// A window of zero width has no status line cell to borrow from.
	ಯದಿ (ಎಡ->w_width == 0)
	    ಅನುವರ್ತ;
	ಯದಿ (!stl_connected(ಎಡ))
	    ಅನುವರ್ತ;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ start = 0;
	ಪೂರ್ಣಾಙ್ಕ end = 0;

	FOR_ALL_WINDOWS(ಬಲ)
	{
	    ಯದಿ (ಬಲ == ಎಡ || ಬಲ->w_status_height == 0)
		ಅನುವರ್ತ;
	    ಯದಿ (ಬಲ->w_wincol != W_ENDCOL(ಎಡ) + 1)
		ಅನುವರ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ l_stl_row = W_WINROW(ಎಡ) + ಎಡ->w_height;
	    ಪೂರ್ಣಾಙ್ಕ r_stl_row = W_WINROW(ಬಲ) + ಬಲ->w_height;

	    start = l_stl_row > r_stl_row ? l_stl_row : r_stl_row;
	    end = l_stl_row + ಎಡ->w_status_height
				< r_stl_row + ಬಲ->w_status_height
		? l_stl_row + ಎಡ->w_status_height
		: r_stl_row + ಬಲ->w_status_height;
	    ಯದಿ (start < end)
	    {
		neighbour = ಬಲ;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (neighbour == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	ಪೂರ್ಣಾಙ್ಕ	hl;
	ಯದಿ (ಎಡ != curwin && neighbour != curwin
		&& fillchar_status(&hl, ಎಡ) != ' ')
	    ಅನುವರ್ತ;

	// Source: prefer curwin's side; otherwise left window's right edge.
	ಪೂರ್ಣಾಙ್ಕ dst_col = W_ENDCOL(ಎಡ);
	ಪೂರ್ಣಾಙ್ಕ src_col = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(ಎಡ) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	ಯದಿ (dst_col >= screen_Columns || src_col >= screen_Columns)
	    ಅನುವರ್ತ;
	ಯದಿ (end > screen_Rows)
	    end = screen_Rows;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ r = start; r < end; r++)
	{
	    unsigned	dst_off = ಸಾಲು_Offset[r] + dst_col;
	    sattr_T	attr = ScreenAttrs[ಸಾಲು_Offset[r] + src_col];

	    ಯದಿ (ScreenAttrs[dst_off] != attr)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_custom_statusline(win_T *wp)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    ಯದಿ (entered)
	ಪ್ರತ್ಯಾಗಮ;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#endif

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    ಶೂನ್ಯ
showruler(ಪೂರ್ಣಾಙ್ಕ always)
{
    ಯದಿ (!always && !redrawing())
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_status = true;
	ಪ್ರತ್ಯಾಗಮ;
    }
#ಯದಿ defined(FEAT_STL_OPT)
    ಯದಿ ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_status_height)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    ಅನ್ಯಥಾ
#endif
	win_redr_ruler(curwin, always, FALSE);

    ಯದಿ (need_maketitle
#ifdef FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_IN_ICON))
	    || (p_title && (stl_syntax & STL_IN_TITLE))
#endif
       )
	maketitle();

    // Redraw the tab pages line if needed.
    ಯದಿ (redraw_tabline)
	draw_tabline();

#ಯದಿ defined(FEAT_TABPANEL)
    ಯದಿ (redraw_tabpanel)
	draw_tabpanel();
#endif
}

    ಶೂನ್ಯ
win_redr_ruler(win_T *wp, ಪೂರ್ಣಾಙ್ಕ always, ಪೂರ್ಣಾಙ್ಕ ignore_pum)
{
    ಪೂರ್ಣಾಙ್ಕ	empty_ಸಾಲು = FALSE;

    // If 'ruler' off don't do anything
    ಯದಿ (!p_ru)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    ಯದಿ (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    ಯದಿ (wp == lastwin && lastwin->w_status_height == 0)
	ಯದಿ (edit_submode != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    ಯದಿ (!ignore_pum && pum_visible())
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_STL_OPT
    ಯದಿ (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    ಯದಿ ((State & MODE_ಸೇರಿಸು) == 0
		&& *ml_get_buf(wp->w_buffer, wp->w_cursor.lnum, FALSE) == NUL)
	empty_ಸಾಲು = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    ಯದಿ (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.col != wp->w_ru_cursor.col
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ != wp->w_ru_ಸಾಲು_ಎಣಿಕೆ
#ifdef FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#endif
	    || empty_ಸಾಲು != wp->w_ru_empty)
    {
	ಪೂರ್ಣಾಙ್ಕ	row;
	ಪೂರ್ಣಾಙ್ಕ	fillchar;
	ಪೂರ್ಣಾಙ್ಕ	attr;
	ಪೂರ್ಣಾಙ್ಕ	off;
	ಪೂರ್ಣಾಙ್ಕ	width;
	colnr_T	virtcol;
#define RULER_BUF_LEN 70
	char_u	buffer[RULER_BUF_LEN];
	ಪೂರ್ಣಾಙ್ಕ	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	ಪೂರ್ಣಾಙ್ಕ	rel_poslen;
	ಪೂರ್ಣಾಙ್ಕ	this_ru_col;
	ಪೂರ್ಣಾಙ್ಕ	n1;			    // scratch value
	ಪೂರ್ಣಾಙ್ಕ	n2;			    // scratch value
	bool	override_success =
	    ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	ಯದಿ (wp->w_status_height)
	{
	    row = statusline_row(wp);
	    fillchar = fillchar_status(&attr, wp);
	    off = wp->w_wincol;
	    width = wp->w_width;
	}
	ಅನ್ಯಥಾ
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_col_off;
	    width = cmdline_width;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	ಯದಿ (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, ರಿಕ್ತ, &virtcol, ರಿಕ್ತ, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((ವರ್ಣ *)buffer, RULER_BUF_LEN, _("%ld,"),
		(wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		    ? 0L
		    : (long)(wp->w_cursor.lnum));
	bufferlen += col_print(buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			empty_ಸಾಲು ? 0 : (ಪೂರ್ಣಾಙ್ಕ)wp->w_cursor.col + 1,
			(ಪೂರ್ಣಾಙ್ಕ)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	ಯದಿ (wp->w_status_height == 0)	// can't use last char of screen
	    ++n1;

	this_ru_col = ru_col - (cmdline_width - width);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (width + 1) / 2;
	ಯದಿ (this_ru_col < n2)
	    this_ru_col = n2;
	ಯದಿ (this_ru_col + n1 < width)
	{
	    // need at least space for rel_pos + NUL
	    ಯಾವತ್ (this_ru_col + n1 < width
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		ಯದಿ (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, buffer + bufferlen);
		ಅನ್ಯಥಾ
		    buffer[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((ವರ್ಣ *)buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	ಯದಿ (has_mbyte)
	{
	    ಪ್ರತಿ (n1 = 0, n2 = 0; buffer[n1] != NUL; n1 += (*mb_ptr2len)(buffer + n1))
	    {
		n2 += (*mb_ptr2cells)(buffer + n1);
		ಯದಿ (this_ru_col + n2 > width)
		{
		    bufferlen = n1;
		    buffer[bufferlen] = NUL;
		    ಭಙ್ಗ;
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (this_ru_col + bufferlen > width)
	{
	    bufferlen = width - this_ru_col;
	    buffer[bufferlen] = NUL;
	}

	screen_puts(buffer, row, this_ru_col + off, attr);
	n1 = redraw_cmdline;
	screen_fill(row, row + 1,
		this_ru_col + off + bufferlen,
		(off + width),
		fillchar, fillchar, attr);
	// don't redraw the cmdline because of showing the ruler
	redraw_cmdline = n1;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_empty = empty_ಸಾಲು;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_ಸಾಲು_ಎಣಿಕೆ = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
#ifdef FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#endif

	ಯದಿ (override_success)
	    ತೆಗೆ_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    ಶೂನ್ಯ
after_updating_screen(ಪೂರ್ಣಾಙ್ಕ may_resize_shell UNUSED)
{
    updating_screen = FALSE;
#ifdef FEAT_GUI
    ಯದಿ (may_resize_shell)
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
    ಶೂನ್ಯ
update_curbuf(ಪೂರ್ಣಾಙ್ಕ type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#ಯದಿ defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
text_to_screenline(win_T *wp, char_u *text, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪೂರ್ಣಾಙ್ಕ		off = (ಪೂರ್ಣಾಙ್ಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);

    ಯದಿ (has_mbyte)
    {
	ಪೂರ್ಣಾಙ್ಕ	cells;
	ಪೂರ್ಣಾಙ್ಕ	u8c, u8cc[MAX_MCO];
	ಪೂರ್ಣಾಙ್ಕ	i;
	ಪೂರ್ಣಾಙ್ಕ	idx;
	ಪೂರ್ಣಾಙ್ಕ	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	ಪೂರ್ಣಾಙ್ಕ	prev_c = 0;		// previous Arabic character
	ಪೂರ್ಣಾಙ್ಕ	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	ಯದಿ (wp->w_p_rl)
	    idx = off;
	ಅನ್ಯಥಾ
# endif
	    idx = off + col;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	ಪ್ರತಿ (p = text; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    ಯದಿ (col + cells > wp->w_width)
		ಭಙ್ಗ;
# ifdef FEAT_RIGHTLEFT
	    ಯದಿ (wp->w_p_rl)
		idx = off + wp->w_width - col - cells;
# endif
	    Screen_ಸಾಲುಗಳು[idx] = *p;
	    ಯದಿ (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		ಯದಿ (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_ಸಾಲುಗಳು_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    prev_c = u8c;
# endif
		}
		ಅನ್ಯಥಾ
		{
# ifdef FEAT_ARABIC
		    ಯದಿ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			ಪೂರ್ಣಾಙ್ಕ	pc, pc1, nc;
			ಪೂರ್ಣಾಙ್ಕ	pcc[MAX_MCO];
			ಪೂರ್ಣಾಙ್ಕ	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ಯದಿ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			ಅನ್ಯಥಾ
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			Screen_ಸಾಲುಗಳು[idx] = firstbyte;
		    }
		    ಅನ್ಯಥಾ
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_ಸಾಲುಗಳು_UC[idx] = u8c;
		    ಪ್ರತಿ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ಸಾಲುಗಳು_C[i][idx] = u8cc[i];
			ಯದಿ (u8cc[i] == 0)
			    ಭಙ್ಗ;
		    }
		}
		ಯದಿ (cells > 1)
		    Screen_ಸಾಲುಗಳು[idx + 1] = 0;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    ಅನ್ಯಥಾ ಯದಿ (cells > 1)
		// double-width character
		Screen_ಸಾಲುಗಳು[idx + 1] = p[1];
	    col += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(text);
	ಪೂರ್ಣಾಙ್ಕ n = wp->w_width - col;

	ಯದಿ (len > n)
	    len = n;
	ಯದಿ (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    ಯದಿ (wp->w_p_rl)
		mch_memmove(current_Screen_ಸಾಲು, text, len);
	    ಅನ್ಯಥಾ
# endif
		mch_memmove(current_Screen_ಸಾಲು + col, text, len);
	    col += len;
	}
    }
    ಪ್ರತ್ಯಾಗಮ col;
}
#endif

#ifdef FEAT_MENU
/*
 * Draw the window toolbar.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    ಪೂರ್ಣಾಙ್ಕ		item_idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		item_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		col = 0;
    ಪೂರ್ಣಾಙ್ಕ		next_col;
    ಪೂರ್ಣಾಙ್ಕ		off = (ಪೂರ್ಣಾಙ್ಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
    bool	override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ಪೂರ್ಣಾಙ್ಕ		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    ಪೂರ್ಣಾಙ್ಕ		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_ವಸ್ತುಗಳು);
    FOR_ALL_CHILD_MENUS(wp->w_winbar, menu)
	++item_ಎಣಿಕೆ;
    wp->w_winbar_ವಸ್ತುಗಳು = ALLOC_CLEAR_MULT(winbar_item_T, item_ಎಣಿಕೆ + 1);

    // TODO: use fewer spaces if there is not enough room
    ಪ್ರತಿ (menu = wp->w_winbar->children;
			  menu != ರಿಕ್ತ && col < wp->w_width; menu = menu->next)
    {
	space_to_screenline(off + col, fill_attr);
	ಯದಿ (++col >= wp->w_width)
	    ಭಙ್ಗ;
	ಯದಿ (col > 1)
	{
	    space_to_screenline(off + col, fill_attr);
	    ಯದಿ (++col >= wp->w_width)
		ಭಙ್ಗ;
	}

	wp->w_winbar_ವಸ್ತುಗಳು[item_idx].wb_startcol = col;
	space_to_screenline(off + col, button_attr);
	ಯದಿ (++col >= wp->w_width)
	    ಭಙ್ಗ;

	next_col = text_to_screenline(wp, menu->name, col);
	ಯಾವತ್ (col < next_col)
	{
	    ScreenAttrs[off + col] = button_attr;
	    ++col;
	}
	wp->w_winbar_ವಸ್ತುಗಳು[item_idx].wb_endcol = col;
	wp->w_winbar_ವಸ್ತುಗಳು[item_idx].wb_menu = menu;
	++item_idx;

	ಯದಿ (col >= wp->w_width)
	    ಭಙ್ಗ;
	space_to_screenline(off + col, button_attr);
	++col;
    }
    ಯಾವತ್ (col < wp->w_width)
    {
	space_to_screenline(off + col, fill_attr);
	++col;
    }
    wp->w_winbar_ವಸ್ತುಗಳು[item_idx].wb_menu = ರಿಕ್ತ; // end marker

    screen_ಸಾಲು(wp, wp->w_winrow, wp->w_wincol, wp->w_width,
							  wp->w_width, -1, 0);

    ಯದಿ (override_success)
	ತೆಗೆ_highlight_overrides();
}
#endif

#ಯದಿ defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_text_attr(
    ಪೂರ್ಣಾಙ್ಕ		off,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		attr)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    mch_memmove(Screen_ಸಾಲುಗಳು + off, buf, (size_t)len);
    ಯದಿ (enc_utf8)
	vim_memset(Screen_ಸಾಲುಗಳು_UC + off, 0, ಪರಿಮಾಣ(u8char_T) * (size_t)len);
    ಪ್ರತಿ (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fold_ಸಾಲು(
    win_T	*wp,
    long	fold_ಎಣಿಕೆ,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    ಪೂರ್ಣಾಙ್ಕ		row)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*ತುದಿ, *bot;
    linenr_T	lnume = lnum + fold_ಎಣಿಕೆ - 1;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*text;
    ಪೂರ್ಣಾಙ್ಕ		fdc;
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		txtcol;
    ಪೂರ್ಣಾಙ್ಕ		off = (ಪೂರ್ಣಾಙ್ಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
    ಪೂರ್ಣಾಙ್ಕ		ri;

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
    ಯದಿ (wp == cmdwin_win)
    {
	Screen_ಸಾಲುಗಳು[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	ಯದಿ (enc_utf8)
	    Screen_ಸಾಲುಗಳು_UC[off] = 0;
	++col;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    ಕುರು { \
	ಯದಿ (wp->w_p_rl) \
	    ಪ್ರತಿ (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_width - (p) - (l)) + ri] = v; \
	 ಅನ್ಯಥಾ \
	    ಪ್ರತಿ (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } ಯಾವತ್ (0)
# ಅನ್ಯಥಾ
#  define RL_MEMSET(p, v, l) \
    ಕುರು { \
	ಪ್ರತಿ (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } ಯಾವತ್ (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, col);
    ಯದಿ (fdc > 0)
    {
	char_u	*p;
	ಪೂರ್ಣಾಙ್ಕ	i;
	ಪೂರ್ಣಾಙ್ಕ	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	ಪ್ರತಿ (i = 0; i < fdc; i++)
	{
	    ಪೂರ್ಣಾಙ್ಕ		ch;

	    ಯದಿ (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    ಅನ್ಯಥಾ
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    ಯದಿ (wp->w_p_rl)
		idx = off + wp->w_width - i - 1 - col;
	    ಅನ್ಯಥಾ
# endif
		idx = off + col + i;
	    ಯದಿ (enc_utf8)
	    {
		ಯದಿ (ch >= 0x80)
		{
		    Screen_ಸಾಲುಗಳು_UC[idx] = ch;
		    Screen_ಸಾಲುಗಳು_C[0][idx] = 0;
		    Screen_ಸಾಲುಗಳು[idx] = 0x80;
		}
		ಅನ್ಯಥಾ
		{
		    Screen_ಸಾಲುಗಳು[idx] = ch;
		    Screen_ಸಾಲುಗಳು_UC[idx] = 0;
		}
	    }
	    ಅನ್ಯಥಾ
		Screen_ಸಾಲುಗಳು[idx] = ch;
	}

	RL_MEMSET(col, HL_ATTR(HLF_FC), fdc);
	col += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    // text
    RL_MEMSET(col, HL_ATTR(HLF_FL), wp->w_width - col);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    ಯದಿ (signcolumn_on(wp))
    {
	len = wp->w_width - col;
	ಯದಿ (len > 0)
	{
	    ಯದಿ (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    ಯದಿ (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    ಅನ್ಯಥಾ
#  endif
		copy_text_attr(off + col, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    col += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    ಯದಿ (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_width - col;
	ಯದಿ (len > 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    w = ಸಂಖ್ಯೆ_width(wp);
	    long    ಅಂಕೆ;
	    ವರ್ಣ    *fmt = "%*ld ";

	    ಯದಿ (len > w + 1)
		len = w + 1;

	    ಯದಿ (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		ಅಂಕೆ = (long)lnum;
	    ಅನ್ಯಥಾ
	    {
		// 'relativenumber', don't use negative numbers
		ಅಂಕೆ = labs((long)get_cursor_rel_lnum(wp, lnum));
		ಯದಿ (ಅಂಕೆ == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    ಅಂಕೆ = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf), fmt, w, ಅಂಕೆ);
# ifdef FEAT_RIGHTLEFT
	    ಯದಿ (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col, buf, len,
							     HL_ATTR(HLF_FL));
	    ಅನ್ಯಥಾ
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
    ಯಾವತ್ (col < wp->w_width)
    {
	ಪೂರ್ಣಾಙ್ಕ c = wp->w_fill_chars.fold;
	ಪೂರ್ಣಾಙ್ಕ idx = off + col;

# ifdef FEAT_RIGHTLEFT
	ಯದಿ (wp->w_p_rl)
	    idx = off + wp->w_width - 1 - col;
# endif

	ಯದಿ (enc_utf8)
	{
	    ಯದಿ (c >= 0x80)
	    {
		Screen_ಸಾಲುಗಳು_UC[idx] = c;
		Screen_ಸಾಲುಗಳು_C[0][idx] = 0;
		Screen_ಸಾಲುಗಳು[idx] = 0x80; // avoid storing zero
	    }
	    ಅನ್ಯಥಾ
	    {
		Screen_ಸಾಲುಗಳು_UC[idx] = 0;
		Screen_ಸಾಲುಗಳು[idx] = c;
	    }
	}
	ಅನ್ಯಥಾ
	    Screen_ಸಾಲುಗಳು[idx] = c;
	++col;
    }

    ಯದಿ (text != buf)
	vim_free(text);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    ಯದಿ (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
	ಯದಿ (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    ತುದಿ = &curwin->w_cursor;
	    bot = &VIsual;
	}
	ಅನ್ಯಥಾ
	{
	    // Visual is before curwin->w_cursor
	    ತುದಿ = &VIsual;
	    bot = &curwin->w_cursor;
	}
	ಯದಿ (lnum >= ತುದಿ->lnum
		&& lnume <= bot->lnum
		&& (VIsual_mode != 'v'
		    || ((lnum > ತುದಿ->lnum
			    || (lnum == ತುದಿ->lnum
				&& ತುದಿ->col == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->col - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_buffer, lnume))))))
	{
	    ಯದಿ (VIsual_mode == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		ಯದಿ (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_width)
		{
		    ಯದಿ (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_width)
			len = wp->w_old_cursor_lcol;
		    ಅನ್ಯಥಾ
			len = wp->w_width - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (ಪೂರ್ಣಾಙ್ಕ)wp->w_old_cursor_fcol);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_width - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    ಯದಿ (wp->w_p_cc_cols)
    {
	ಪೂರ್ಣಾಙ್ಕ i = 0;
	ಪೂರ್ಣಾಙ್ಕ j = wp->w_p_cc_cols[i];
	ಪೂರ್ಣಾಙ್ಕ old_txtcol = txtcol;

	ಯಾವತ್ (j > -1)
	{
	    txtcol += j;
	    ಯದಿ (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    ಅನ್ಯಥಾ
		txtcol -= wp->w_leftcol;
	    ಯದಿ (txtcol >= 0 && txtcol < wp->w_width)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_cols[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    ಯದಿ (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	ಯದಿ (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	ಅನ್ಯಥಾ
	    txtcol -= wp->w_leftcol;
	ಯದಿ (txtcol >= 0 && txtcol < wp->w_width)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_ಸಾಲು(wp, row + W_WINROW(wp), wp->w_wincol, wp->w_width, wp->w_width,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    ಯದಿ (wp == curwin
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
    ಯದಿ (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_ಸಾಲು(wp))
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
    ಸ್ಥಿರ ಶೂನ್ಯ
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_buffer;
    ಪೂರ್ಣಾಙ್ಕ		type;
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ_end = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    ಪೂರ್ಣಾಙ್ಕ		mid_start = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    ಪೂರ್ಣಾಙ್ಕ		mid_end = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    ಪೂರ್ಣಾಙ್ಕ		bot_start = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    ಪೂರ್ಣಾಙ್ಕ		scrolled_down = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    ಪೂರ್ಣಾಙ್ಕ		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //ಸಾಲುಗಳು
#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ_to_mod = FALSE;    // redraw above mod_top
#endif

    ಪೂರ್ಣಾಙ್ಕ		row;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    ಪೂರ್ಣಾಙ್ಕ		idx;		// current index in w_lines[]
    ಪೂರ್ಣಾಙ್ಕ		srow;		// starting row of the current line

    ಪೂರ್ಣಾಙ್ಕ		eof = FALSE;	// if TRUE, we hit the end of the file
    ಪೂರ್ಣಾಙ್ಕ		didline = FALSE; // if TRUE, we finished the last line
    ಪೂರ್ಣಾಙ್ಕ		i;
    long	j;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#ifdef FEAT_CONCEAL
    ಪೂರ್ಣಾಙ್ಕ		old_wrow = wp->w_wrow;
    ಪೂರ್ಣಾಙ್ಕ		old_wcol = wp->w_wcol;
#endif
#ifdef FEAT_FOLDING
    long	fold_ಎಣಿಕೆ;
#endif
#ifdef FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_ಸಾಲು 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    ಪೂರ್ಣಾಙ್ಕ		did_update = DID_NONE;
    linenr_T	syntax_last_parsed = 0;		// last parsed text line
#endif
    linenr_T	mod_ತುದಿ = 0;
    linenr_T	mod_bot = 0;
#ಯದಿ defined(FEAT_SYN_HL) || defined(FEAT_ಹುಡುಕು_EXTRA)
    ಪೂರ್ಣಾಙ್ಕ		save_got_int;
#endif
    bool	override_success;

#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    ಯದಿ (!did_update_one_window)
    {
	did_update_one_window = TRUE;
# ifdef FEAT_ಹುಡುಕು_EXTRA
	start_ಹುಡುಕು_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	ಯದಿ (clip_star.available && clip_isautosel_star())
	    clip_update_selection(&clip_star);
	ಯದಿ (clip_plus.available && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#endif

#ifdef FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    ಯದಿ (wp->w_p_cuc)
	validate_virtcol_win(wp);
#endif

    type = wp->w_redr_type;

    ಯದಿ (type == UPD_NOT_VALID)
    {
	wp->w_redr_status = true;
	wp->w_ಸಾಲುಗಳು_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    ಯದಿ (wp->w_height + WINBAR_HEIGHT(wp) == 0
	    || (wp->w_frame->fr_height == wp->w_status_height
#ಯದಿ defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#endif
	       ))
    {
	wp->w_redr_type = 0;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Window is zero-width: Only need to draw the separator.
    ಯದಿ (wp->w_width == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	ಪ್ರತ್ಯಾಗಮ;
    }

    override_success = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);


#ifdef FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    ಯದಿ (term_do_update_window(wp))
    {
	term_update_window(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	ಯದಿ (winbar_height(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	ಯದಿ (override_success)
	    ತೆಗೆ_highlight_overrides();
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

#ifdef FEAT_ಹುಡುಕು_EXTRA
    init_ಹುಡುಕು_hl(wp, &screen_ಹುಡುಕು_hl);
#endif

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    ಯದಿ (wp->w_skipcol > 0 && wp->w_width > win_col_off(wp))
    {
	ಪೂರ್ಣಾಙ್ಕ w = 0;
	ಪೂರ್ಣಾಙ್ಕ width1 = wp->w_width - win_col_off(wp);
	ಪೂರ್ಣಾಙ್ಕ width2 = width1 + win_col_off2(wp);
	ಪೂರ್ಣಾಙ್ಕ add = width1;

	ಯಾವತ್ (w < wp->w_skipcol)
	{
	    ಯದಿ (w > 0)
		add = width2;
	    w += add;
	}
	ಯದಿ (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - add;
    }

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? ಸಂಖ್ಯೆ_width(wp) : 0;
    ಯದಿ (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    ಅನ್ಯಥಾ
#endif

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_ತುದಿ = wp->w_redraw_ತುದಿ;
	ಯದಿ (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	ಅನ್ಯಥಾ
	    mod_bot = 0;
	ಯದಿ (buf->b_mod_set)
	{
	    ಯದಿ (mod_ತುದಿ == 0 || mod_ತುದಿ > buf->b_mod_ತುದಿ)
	    {
		mod_ತುದಿ = buf->b_mod_ತುದಿ;
#ifdef FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		ಯದಿ (syntax_present(wp))
		{
		    mod_ತುದಿ -= buf->b_s.b_syn_sync_linebreaks;
		    ಯದಿ (mod_ತುದಿ < 1)
			mod_ತುದಿ = 1;
		}
#endif
	    }
	    ಯದಿ (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ifdef FEAT_ಹುಡುಕು_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    ಯದಿ (screen_ಹುಡುಕು_hl.rm.regprog != ರಿಕ್ತ
		    && re_multiline(screen_ಹುಡುಕು_hl.rm.regprog))
		ತುದಿ_to_mod = TRUE;
	    ಅನ್ಯಥಾ
	    {
		matchitem_T *cur = wp->w_match_head;

		ಯಾವತ್ (cur != ರಿಕ್ತ)
		{
		    ಯದಿ (cur->mit_match.regprog != ರಿಕ್ತ
				       && re_multiline(cur->mit_match.regprog))
		    {
			ತುದಿ_to_mod = TRUE;
			ಭಙ್ಗ;
		    }
		    cur = cur->mit_next;
		}
	    }
#endif
	}

#ifdef FEAT_ಹುಡುಕು_EXTRA
	ಯದಿ (ಹುಡುಕು_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    ಯದಿ (mod_ತುದಿ == 0 || mod_ತುದಿ > ಹುಡುಕು_hl_has_cursor_lnum)
		mod_ತುದಿ = ಹುಡುಕು_hl_has_cursor_lnum;
	    ಯದಿ (mod_bot == 0 || mod_bot < ಹುಡುಕು_hl_has_cursor_lnum + 1)
		mod_bot = ಹುಡುಕು_hl_has_cursor_lnum + 1;
	}
#endif

#ifdef FEAT_FOLDING
	ಯದಿ (mod_ತುದಿ != 0 && hasAnyFolding(wp))
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
	    ಪ್ರತಿ (i = 0; i < wp->w_ಸಾಲುಗಳು_valid; ++i)
		ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_valid)
		{
		    ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_lastlnum < mod_ತುದಿ)
			lnumt = wp->w_ಸಾಲುಗಳು[i].wl_lastlnum + 1;
		    ಯದಿ (lnumb == MAXLNUM && wp->w_ಸಾಲುಗಳು[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_ಸಾಲುಗಳು[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			ಯದಿ (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (ಶೂನ್ಯ)hasFoldingWin(wp, mod_ತುದಿ, &mod_ತುದಿ, ರಿಕ್ತ, TRUE, ರಿಕ್ತ);
	    ಯದಿ (mod_ತುದಿ > lnumt)
		mod_ತುದಿ = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (ಶೂನ್ಯ)hasFoldingWin(wp, mod_bot, ರಿಕ್ತ, &mod_bot, TRUE, ರಿಕ್ತ);
	    ++mod_bot;
	    ಯದಿ (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#endif

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	ಯದಿ (mod_ತುದಿ != 0 && mod_ತುದಿ < wp->w_topline)
	{
	    ಯದಿ (mod_bot > wp->w_topline)
		mod_ತುದಿ = wp->w_topline;
#ifdef FEAT_SYN_HL
	    ಅನ್ಯಥಾ ಯದಿ (syntax_present(wp))
		ತುದಿ_end = 1;
#endif
	}
    }
    wp->w_redraw_ತುದಿ = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಹುಡುಕು_hl_has_cursor_lnum = 0;
#endif


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    ಯದಿ (type == UPD_REDRAW_ತುದಿ)
    {
	j = 0;
	ಪ್ರತಿ (i = 0; i < wp->w_ಸಾಲುಗಳು_valid; ++i)
	{
	    j += wp->w_ಸಾಲುಗಳು[i].wl_size;
	    ಯದಿ (j >= wp->w_upd_rows)
	    {
		ತುದಿ_end = j;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (ತುದಿ_end == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	ಅನ್ಯಥಾ
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    ಯದಿ (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    ಯದಿ ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#ifdef FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#endif
	    )
    {
	ಯದಿ (mod_ತುದಿ != 0
		&& wp->w_topline == mod_ತುದಿ
		&& (!wp->w_ಸಾಲುಗಳು[0].wl_valid
		    || wp->w_topline == wp->w_ಸಾಲುಗಳು[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	ಅನ್ಯಥಾ ಯದಿ (wp->w_ಸಾಲುಗಳು[0].wl_valid
		&& (wp->w_topline < wp->w_ಸಾಲುಗಳು[0].wl_lnum
#ifdef FEAT_DIFF
		    || (wp->w_topline == wp->w_ಸಾಲುಗಳು[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#endif
		   ))
	{
	    // New topline is above old topline: May scroll down.
#ifdef FEAT_FOLDING
	    ಯದಿ (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		ಪ್ರತಿ (ln = wp->w_topline; ln < wp->w_ಸಾಲುಗಳು[0].wl_lnum; ++ln)
		{
		    ++j;
		    ಯದಿ (j >= wp->w_height - 2)
			ಭಙ್ಗ;
		    (ಶೂನ್ಯ)hasFoldingWin(wp, ln, ರಿಕ್ತ, &ln, TRUE, ರಿಕ್ತ);
		}
	    }
	    ಅನ್ಯಥಾ
#endif
		j = wp->w_ಸಾಲುಗಳು[0].wl_lnum - wp->w_topline;
	    ಯದಿ (j < wp->w_height - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_ಸಾಲುಗಳು[0].wl_lnum - 1,
								wp->w_height);
#ifdef FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		ಯದಿ (wp->w_ಸಾಲುಗಳು[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_ಸಾಲುಗಳು[0].wl_lnum)
							  - wp->w_old_topfill;
#endif
		ಯದಿ (i < wp->w_height - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    ಯದಿ (i > 0)
			check_for_delay(FALSE);
		    ಯದಿ (win_ins_ಸಾಲುಗಳು(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			ಯದಿ (wp->w_ಸಾಲುಗಳು_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    ತುದಿ_end = i;
			    scrolled_down = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    ಯದಿ ((wp->w_ಸಾಲುಗಳು_valid += j) > wp->w_height)
				wp->w_ಸಾಲುಗಳು_valid = wp->w_height;
			    ಪ್ರತಿ (idx = wp->w_ಸಾಲುಗಳು_valid; idx - j >= 0; idx--)
				wp->w_ಸಾಲುಗಳು[idx] = wp->w_ಸಾಲುಗಳು[idx - j];
			    ಯಾವತ್ (idx >= 0)
				wp->w_ಸಾಲುಗಳು[idx--].wl_valid = FALSE;
			}
		    }
		    ಅನ್ಯಥಾ
			mid_start = 0;		// redraw all lines
		}
		ಅನ್ಯಥಾ
		    mid_start = 0;		// redraw all lines
	    }
	    ಅನ್ಯಥಾ
		mid_start = 0;		// redraw all lines
	}
	ಅನ್ಯಥಾ
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    row = 0;
	    ಪ್ರತಿ (i = 0; i < wp->w_ಸಾಲುಗಳು_valid && i < Rows; i++)
	    {
		ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_valid
			&& wp->w_ಸಾಲುಗಳು[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    ಭಙ್ಗ;
		}
		row += wp->w_ಸಾಲುಗಳು[i].wl_size;
	    }
	    ಯದಿ (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//ಸಾಲುಗಳು
		mid_start = 0;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#ifdef FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		ಯದಿ (wp->w_ಸಾಲುಗಳು[0].wl_lnum == wp->w_topline)
		    row += wp->w_old_topfill;
		ಅನ್ಯಥಾ
		    row += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		row -= wp->w_topfill;
#endif
		ಯದಿ (row > Rows)  // just in case
		    row = Rows;
		ಯದಿ (row > 0)
		{
		    check_for_delay(FALSE);
		    ಯದಿ (win_del_ಸಾಲುಗಳು(wp, 0, row, FALSE, wp == firstwin, 0)
									 == OK)
			bot_start = wp->w_height - row;
		    ಅನ್ಯಥಾ
			mid_start = 0;		// redraw all lines
		}
		ಯದಿ ((row == 0 || bot_start < 999) && wp->w_ಸಾಲುಗಳು_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_start = 0;
		    idx = 0;
		    ಪ್ರತಿ (;;)
		    {
			wp->w_ಸಾಲುಗಳು[idx] = wp->w_ಸಾಲುಗಳು[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			ಯದಿ (row > 0 && bot_start + row
				 + (ಪೂರ್ಣಾಙ್ಕ)wp->w_ಸಾಲುಗಳು[j].wl_size > wp->w_height)
			{
			    wp->w_ಸಾಲುಗಳು_valid = idx + 1;
			    ಭಙ್ಗ;
			}
			bot_start += wp->w_ಸಾಲುಗಳು[idx++].wl_size;

			// stop at the last valid entry in w_lines[].wl_size
			ಯದಿ (++j >= wp->w_ಸಾಲುಗಳು_valid)
			{
			    wp->w_ಸಾಲುಗಳು_valid = idx;
			    ಭಙ್ಗ;
			}
		    }
#ifdef FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    ಯದಿ (wp->w_p_diff && bot_start > 0)
			wp->w_ಸಾಲುಗಳು[0].wl_size = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#endif
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	ಯದಿ (mid_start == 0)
	    mid_end = wp->w_height;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	ಯದಿ (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    ಅನ್ಯಥಾ
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
    }

    ಯದಿ (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    ಯದಿ ((VIsual_active && buf == curwin->w_buffer)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, to;

	ಯದಿ (VIsual_active)
	{
	    ಯದಿ (VIsual_mode != wp->w_old_visual_mode
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		ಯದಿ (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = VIsual.lnum;
		}
		ಅನ್ಯಥಾ
		{
		    from = VIsual.lnum;
		    to = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		ಯದಿ (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		ಯದಿ (wp->w_old_cursor_lnum > to)
		    to = wp->w_old_cursor_lnum;
		ಯದಿ (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		ಯದಿ (wp->w_old_visual_lnum > to)
		    to = wp->w_old_visual_lnum;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		ಯದಿ (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = wp->w_old_cursor_lnum;
		}
		ಅನ್ಯಥಾ
		{
		    from = wp->w_old_cursor_lnum;
		    to = curwin->w_cursor.lnum;
		    ಯದಿ (from == 0)	// Visual mode just started
			from = to;
		}

		ಯದಿ (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.col != wp->w_old_visual_col)
		{
		    ಯದಿ (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    ಯದಿ (wp->w_old_visual_lnum > to)
			to = wp->w_old_visual_lnum;
		    ಯದಿ (VIsual.lnum < from)
			from = VIsual.lnum;
		    ಯದಿ (VIsual.lnum > to)
			to = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    ಯದಿ (VIsual_mode == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_END_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		ಯದಿ (curwin->w_curswant == MAXCOL)
		{
		    ಯದಿ (get_ve_flags() & VE_BLOCK)
		    {
			pos_T	    pos;
			ಪೂರ್ಣಾಙ್ಕ	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			ಪ್ರತಿ (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.col = (ಪೂರ್ಣಾಙ್ಕ)ml_get_buf_len(wp->w_buffer, pos.lnum);
			    getvvcol(wp, &pos, ರಿಕ್ತ, ರಿಕ್ತ, &t, 0);
			    ಯದಿ (toc < t)
				toc = t;
			}
			++toc;
		    }
		    ಅನ್ಯಥಾ
			toc = MAXCOL;
		}

		ಯದಿ (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    ಯದಿ (from > VIsual.lnum)
			from = VIsual.lnum;
		    ಯದಿ (to < VIsual.lnum)
			to = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // Use the line numbers of the old Visual area.
	    ಯದಿ (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		to = wp->w_old_visual_lnum;
	    }
	    ಅನ್ಯಥಾ
	    {
		from = wp->w_old_visual_lnum;
		to = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	ಯದಿ (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	ಯದಿ (wp->w_valid & VALID_BOTLINE)
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
	ಯದಿ (mid_start > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    ಯದಿ (scrolled_down)
		mid_start = ತುದಿ_end;
	    ಅನ್ಯಥಾ
		mid_start = 0;
	    ಯಾವತ್ (lnum < from && idx < wp->w_ಸಾಲುಗಳು_valid)	// find start
	    {
		ಯದಿ (wp->w_ಸಾಲುಗಳು[idx].wl_valid)
		    mid_start += wp->w_ಸಾಲುಗಳು[idx].wl_size;
		ಅನ್ಯಥಾ ಯದಿ (!scrolled_down)
		    srow += wp->w_ಸಾಲುಗಳು[idx].wl_size;
		++idx;
#ifdef FEAT_FOLDING
		ಯದಿ (idx < wp->w_ಸಾಲುಗಳು_valid && wp->w_ಸಾಲುಗಳು[idx].wl_valid)
		    lnum = wp->w_ಸಾಲುಗಳು[idx].wl_lnum;
		ಅನ್ಯಥಾ
#endif
		    ++lnum;
	    }
	    srow += mid_start;
	    mid_end = wp->w_height;
	    ಪ್ರತಿ ( ; idx < wp->w_ಸಾಲುಗಳು_valid; ++idx)		// find end
	    {
		ಯದಿ (wp->w_ಸಾಲುಗಳು[idx].wl_valid
			&& wp->w_ಸಾಲುಗಳು[idx].wl_lnum >= to + 1)
		{
		    // Only update until first row of this line
		    mid_end = srow;
		    ಭಙ್ಗ;
		}
		srow += wp->w_ಸಾಲುಗಳು[idx].wl_size;
	    }
	}
    }

    ಯದಿ (VIsual_active && buf == curwin->w_buffer)
    {
	wp->w_old_visual_mode = VIsual_mode;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_col = VIsual.col;
	wp->w_old_curswant = curwin->w_curswant;
    }
    ಅನ್ಯಥಾ
    {
	wp->w_old_visual_mode = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_col = 0;
    }

#ಯದಿ defined(FEAT_SYN_HL) || defined(FEAT_ಹುಡುಕು_EXTRA)
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
    ಯದಿ (winbar_height(wp) > 0)
	redraw_win_toolbar(wp);
#endif

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#ifdef FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_FIELD(spv);
    ಯದಿ (spell_check_window(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_ತುದಿ == 0;
    }
#endif

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    row = 0;
    srow = 0;
    ಪ್ರತಿ (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	ಯದಿ (row == wp->w_height)
	{
	    didline = TRUE;
	    ಭಙ್ಗ;
	}

	// stop updating when hit the end of the file
	ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    eof = TRUE;
	    ಭಙ್ಗ;
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
	ಯದಿ (row < ತುದಿ_end
		|| (row >= mid_start && row < mid_end)
#ifdef FEAT_ಹುಡುಕು_EXTRA
		|| ತುದಿ_to_mod
#endif
		|| idx >= wp->w_ಸಾಲುಗಳು_valid
		|| (row + wp->w_ಸಾಲುಗಳು[idx].wl_size > bot_start)
		|| (mod_ತುದಿ != 0
		    && (lnum == mod_ತುದಿ
			|| (lnum >= mod_ತುದಿ
			    && (lnum < mod_bot
#ifdef FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_ಸಾಲು
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_changed(lnum)))
#endif
#ifdef FEAT_ಹುಡುಕು_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_match_head != ರಿಕ್ತ
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
#ifdef FEAT_ಹುಡುಕು_EXTRA
	    ಯದಿ (lnum == mod_ತುದಿ)
		ತುದಿ_to_mod = FALSE;
#endif

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    ಯದಿ (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_ತುದಿ && lnum < MAX(mod_bot, mod_ತುದಿ + 1)
		    && (!scrolled_down || row >= ತುದಿ_end))
	    {
		scrolled_for_mod = TRUE;

		ಪೂರ್ಣಾಙ್ಕ		old_cline_height = 0;
		ಪೂರ್ಣಾಙ್ಕ		old_rows = 0;
		ಪೂರ್ಣಾಙ್ಕ		new_rows = 0;
		ಪೂರ್ಣಾಙ್ಕ		xtra_rows;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		ಪ್ರತಿ (i = idx; i < wp->w_ಸಾಲುಗಳು_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_valid
			    && wp->w_ಸಾಲುಗಳು[i].wl_lnum == mod_bot)
			ಭಙ್ಗ;
		    ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_height = wp->w_ಸಾಲುಗಳು[i].wl_size;
		    old_rows += wp->w_ಸಾಲುಗಳು[i].wl_size;
#ifdef FEAT_FOLDING
		    ಯದಿ (wp->w_ಸಾಲುಗಳು[i].wl_valid
			    && wp->w_ಸಾಲುಗಳು[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			ಯಾವತ್ (i < wp->w_ಸಾಲುಗಳು_valid
						  && !wp->w_ಸಾಲುಗಳು[i].wl_valid)
			    old_rows += wp->w_ಸಾಲುಗಳು[i++].wl_size;
			ಭಙ್ಗ;
		    }
#endif
		}

		ಯದಿ (i >= wp->w_ಸಾಲುಗಳು_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_start = 0;
		}
		ಅನ್ಯಥಾ
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    ಪ್ರತಿ (l = lnum; l < mod_bot; ++l)
		    {
			ಯದಿ (dollar_vcol >= 0 && wp == curwin &&
				old_cline_height > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_rows += old_cline_height;
#ifdef FEAT_FOLDING
			ಅನ್ಯಥಾ ಯದಿ (hasFoldingWin(wp, l, ರಿಕ್ತ, &l, TRUE, ರಿಕ್ತ))
			    ++new_rows;
#endif
			ಅನ್ಯಥಾ
			    new_rows += plines_correct_topline(wp, l, TRUE);
			++j;
			ಯದಿ (new_rows > wp->w_height - row - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_rows = 9999;
			    ಭಙ್ಗ;
			}
		    }
		    xtra_rows = new_rows - old_rows;
		    ಯದಿ (xtra_rows < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			ಯದಿ (row - xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			ಅನ್ಯಥಾ
			{
			    check_for_delay(FALSE);
			    ಯದಿ (win_del_ಸಾಲುಗಳು(wp, row,
					  -xtra_rows, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    ಅನ್ಯಥಾ
				bot_start = wp->w_height + xtra_rows;
			}
		    }
		    ಅನ್ಯಥಾ ಯದಿ (xtra_rows > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			ಯದಿ (row + xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			ಅನ್ಯಥಾ
			{
			    check_for_delay(FALSE);
			    ಯದಿ (win_ins_ಸಾಲುಗಳು(wp, row + old_rows,
					     xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    ಅನ್ಯಥಾ ಯದಿ (ತುದಿ_end > row + old_rows)
				// Scrolled the part at the top that requires
				// updating down.
				ತುದಿ_end += xtra_rows;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    ಯದಿ (mod_bot != MAXLNUM && i != j)
		    {
			ಯದಿ (j < i)
			{
			    ಪೂರ್ಣಾಙ್ಕ x = row + new_rows;

			    // move entries in w_lines[] upwards
			    ಪ್ರತಿ (;;)
			    {
				// stop at last valid entry in w_lines[]
				ಯದಿ (i >= wp->w_ಸಾಲುಗಳು_valid)
				{
				    wp->w_ಸಾಲುಗಳು_valid = j;
				    ಭಙ್ಗ;
				}
				wp->w_ಸಾಲುಗಳು[j] = wp->w_ಸಾಲುಗಳು[i];
				// stop at a line that won't fit
				ಯದಿ (x + (ಪೂರ್ಣಾಙ್ಕ)wp->w_ಸಾಲುಗಳು[j].wl_size
							   > wp->w_height)
				{
				    wp->w_ಸಾಲುಗಳು_valid = j + 1;
				    ಭಙ್ಗ;
				}
				x += wp->w_ಸಾಲುಗಳು[j++].wl_size;
				++i;
			    }
			    ಯದಿ (bot_start > x)
				bot_start = x;
			}
			ಅನ್ಯಥಾ // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_ಸಾಲುಗಳು_valid += j;
			    ಯದಿ (wp->w_ಸಾಲುಗಳು_valid > wp->w_height)
				wp->w_ಸಾಲುಗಳು_valid = wp->w_height;
			    ಪ್ರತಿ (i = wp->w_ಸಾಲುಗಳು_valid; i - j >= idx; --i)
				wp->w_ಸಾಲುಗಳು[i] = wp->w_ಸಾಲುಗಳು[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    ಯಾವತ್ (i >= idx)
			    {
				wp->w_ಸಾಲುಗಳು[i].wl_size = 0;
				wp->w_ಸಾಲುಗಳು[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ifdef FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_ಎಣಿಕೆ = folded_ಎಣಿಕೆ(wp, lnum, &win_foldinfo);
	    ಯದಿ (fold_ಎಣಿಕೆ != 0)
	    {
		fold_ಸಾಲು(wp, fold_ಎಣಿಕೆ, &win_foldinfo, lnum, row);
		++row;
		--fold_ಎಣಿಕೆ;
		wp->w_ಸಾಲುಗಳು[idx].wl_folded = TRUE;
		wp->w_ಸಾಲುಗಳು[idx].wl_lastlnum = lnum + fold_ಎಣಿಕೆ;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
# ifdef FEAT_SPELL
		spv.spv_capcol_lnum = 0;
# endif
	    }
	    ಅನ್ಯಥಾ
#endif
	    ಯದಿ (idx < wp->w_ಸಾಲುಗಳು_valid
		    && wp->w_ಸಾಲುಗಳು[idx].wl_valid
		    && wp->w_ಸಾಲುಗಳು[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_ಸಾಲುಗಳು[idx].wl_size > wp->w_height
#ifdef FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#endif
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		row = wp->w_height + 1;
	    }
	    ಅನ್ಯಥಾ
	    {
#ifdef FEAT_ಹುಡುಕು_EXTRA
		prepare_ಹುಡುಕು_hl(wp, &screen_ಹುಡುಕು_hl, lnum);
#endif
#ifdef FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		ಯದಿ (syntax_last_parsed != 0 && syntax_last_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

		// Display one line.
		row = win_ಸಾಲು(wp, lnum, srow, wp->w_height, 0, &spv);

#ifdef FEAT_FOLDING
		wp->w_ಸಾಲುಗಳು[idx].wl_folded = FALSE;
		wp->w_ಸಾಲುಗಳು[idx].wl_lastlnum = lnum;
#endif
#ifdef FEAT_SYN_HL
		did_update = DID_ಸಾಲು;
		syntax_last_parsed = lnum;
#endif
	    }

	    wp->w_ಸಾಲುಗಳು[idx].wl_lnum = lnum;
	    wp->w_ಸಾಲುಗಳು[idx].wl_valid = TRUE;

	    ಪೂರ್ಣಾಙ್ಕ is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    ಯದಿ (row > wp->w_height || row + wp->w_winrow >= Rows)
	    {
		// we may need the size of that too long line later on
		ಯದಿ (dollar_vcol == -1 || !is_curline)
		    wp->w_ಸಾಲುಗಳು[idx].wl_size = plines_win(wp, lnum, TRUE);
		++idx;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (dollar_vcol == -1 || !is_curline)
		wp->w_ಸಾಲುಗಳು[idx].wl_size = row - srow;
	    ++idx;
#ifdef FEAT_FOLDING
	    lnum += fold_ಎಣಿಕೆ + 1;
#ಅನ್ಯಥಾ
	    ++lnum;
#endif
	}
	ಅನ್ಯಥಾ
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    ಯದಿ ((wp->w_p_nu && mod_ತುದಿ != 0 && lnum >= mod_bot
			&& buf->b_mod_set && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_last_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#ifdef FEAT_FOLDING
		fold_ಎಣಿಕೆ = folded_ಎಣಿಕೆ(wp, lnum, &win_foldinfo);
		ಯದಿ (fold_ಎಣಿಕೆ != 0)
		    fold_ಸಾಲು(wp, fold_ಎಣಿಕೆ, &win_foldinfo, lnum, row);
		ಅನ್ಯಥಾ
#endif
		    (ಶೂನ್ಯ)win_ಸಾಲು(wp, lnum, srow, wp->w_height,
					       wp->w_ಸಾಲುಗಳು[idx].wl_size, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    row += wp->w_ಸಾಲುಗಳು[idx++].wl_size;
	    ಯದಿ (row > wp->w_height)	// past end of screen
		ಭಙ್ಗ;
#ifdef FEAT_FOLDING
	    lnum = wp->w_ಸಾಲುಗಳು[idx - 1].wl_lastlnum + 1;
#ಅನ್ಯಥಾ
	    ++lnum;
#endif
#ifdef FEAT_SYN_HL
	    did_update = DID_NONE;
#endif
#ifdef FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#endif
	}

	ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    eof = TRUE;
	    ಭಙ್ಗ;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	ಯದಿ (idx >= Rows)
	    ಭಙ್ಗ;
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
    ಯದಿ (use_vtp() && get_conpty_fix_type() < 1)
    {
	ಪೂರ್ಣಾಙ್ಕ k;

	ಪ್ರತಿ (k = 0; k < Rows; ++k)
	    ಯದಿ (enc_utf8)
		ಯದಿ ((*mb_off2cells)(ಸಾಲು_Offset[k] + topframe->fr_width - 2,
					   ಸಾಲು_Offset[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_width - 2, 1, 2,
			    FALSE, FALSE);
		ಅನ್ಯಥಾ
		    screen_draw_rectangle(k, topframe->fr_width - 1, 1, 1,
			    FALSE, FALSE);
	    ಅನ್ಯಥಾ
		screen_char(ಸಾಲು_Offset[k] + topframe->fr_width - 1, k,
			cmdline_width - 1);
    }
#endif

    ಯದಿ (idx > wp->w_ಸಾಲುಗಳು_valid)
	wp->w_ಸಾಲುಗಳು_valid = idx;

#ifdef FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    ಯದಿ (syntax_last_parsed != 0 && syntax_present(wp))
	syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_empty_rows = 0;
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    ಯದಿ (!eof && !didline)
    {
	ಯದಿ (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#ifdef FEAT_DIFF
	ಅನ್ಯಥಾ ಯದಿ (diff_check_fill(wp, lnum) >= wp->w_height - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_rows = wp->w_height - srow;
	}
#endif
#ifdef FEAT_PROP_POPUP
	ಅನ್ಯಥಾ ಯದಿ (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#endif
	ಅನ್ಯಥಾ ಯದಿ (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    ಪೂರ್ಣಾಙ್ಕ		scr_row = W_WINROW(wp) + wp->w_height - 1;
	    ಪೂರ್ಣಾಙ್ಕ		ಚಿಹ್ನೆ  = wp->w_fill_chars.lastline;
	    ಪೂರ್ಣಾಙ್ಕ		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(ಚಿಹ್ನೆ, &fillbuf[0]);
	    mb_char2bytes(ಚಿಹ್ನೆ, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_width > 2 ? 2 : wp->w_width) * charlen,
			    scr_row, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_row, scr_row + 1,
		    (ಪೂರ್ಣಾಙ್ಕ)wp->w_wincol + 2, (ಪೂರ್ಣಾಙ್ಕ)W_ENDCOL(wp),
		    ಚಿಹ್ನೆ, ' ', HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	ಅನ್ಯಥಾ ಯದಿ (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    ಪೂರ್ಣಾಙ್ಕ start_col = (ಪೂರ್ಣಾಙ್ಕ)W_ENDCOL(wp) - 3;
	    ಪೂರ್ಣಾಙ್ಕ ಚಿಹ್ನೆ    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_height - 1,
		    W_WINROW(wp) + wp->w_height,
		    (start_col < wp->w_wincol ? wp->w_wincol : start_col),
		    (ಪೂರ್ಣಾಙ್ಕ)W_ENDCOL(wp),
		    ಚಿಹ್ನೆ, ಚಿಹ್ನೆ, HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	ಅನ್ಯಥಾ
	{
	    win_draw_end(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_height, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    ಅನ್ಯಥಾ
    {
	draw_vsep_win(wp, row);
	ಯದಿ (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
#ifdef FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    ಯದಿ (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		ಯದಿ (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		ಅನ್ಯಥಾ
		    i = wp->w_fill_chars.diff;
		ಯದಿ (row + j > wp->w_height)
		    j = wp->w_height - row;
		win_draw_end(wp, i, i, TRUE, row, row + (ಪೂರ್ಣಾಙ್ಕ)j, HLF_DED);
		row += j;
	    }
#endif
	}
	ಅನ್ಯಥಾ ಯದಿ (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	ಯದಿ (WIN_IS_POPUP(wp))
	    win_draw_end(wp, ' ', ' ', FALSE, row, wp->w_height, HLF_AT);
	ಅನ್ಯಥಾ
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

    ಯದಿ (dollar_vcol == -1 || wp != curwin)
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
	ಯದಿ (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#ಯದಿ defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    ಪೂರ್ಣಾಙ್ಕ		new_wcol = wp->w_wcol;
#endif
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#ಯದಿ defined(FEAT_CONCEAL)
	    ಯದಿ (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
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
		ಯದಿ (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		ಅನ್ಯಥಾ
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#endif
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    ಯದಿ (wp->w_redr_type != 0)
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
		ಯದಿ (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#ಯದಿ defined(FEAT_SYN_HL) || defined(FEAT_ಹುಡುಕು_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    ಯದಿ (!got_int)
	got_int = save_got_int;
#endif

    ಯದಿ (override_success)
	ತೆಗೆ_highlight_overrides();
}

#ಯದಿ defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_prepare(ಶೂನ್ಯ)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    ಯದಿ (gui.in_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_ಹುಡುಕು_EXTRA
    start_ಹುಡುಕು_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_mask(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_finish(ಶೂನ್ಯ)
{
    ಯದಿ (redraw_cmdline || redraw_mode)
	showmode();

# ifdef FEAT_ಹುಡುಕು_EXTRA
    end_ಹುಡುಕು_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    ಯದಿ (gui.in_use)
    {
	out_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#endif

#ಯದಿ defined(FEAT_NETBEANS_INTG)
    ಶೂನ್ಯ
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_ಸಾಲು_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    ಯದಿ (!doit || updating_screen
	    || State == MODE_ASKMORE || State == MODE_HITRETURN
	    || ಸಂದೇಶ_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	ಪ್ರತ್ಯಾಗಮ;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ (wp->w_redr_type != 0)
	    win_update(wp);
	ಯದಿ (wp->w_redr_status)
	    win_redr_status(wp, FALSE);
    }

# ಯದಿ defined(FEAT_TABPANEL)
    ಯದಿ (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_finish();
}
#endif

#ಯದಿ defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    ಶೂನ್ಯ
updateWindow(win_T *wp)
{
    // return if already busy updating
    ಯದಿ (updating_screen)
	ಪ್ರತ್ಯಾಗಮ;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    ಯದಿ (clip_star.available && clip_isautosel_star())
	clip_update_selection(&clip_star);
    ಯದಿ (clip_plus.available && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    ಯದಿ (redraw_tabline)
	draw_tabline();

# ಯದಿ defined(FEAT_TABPANEL)
    ಯದಿ (redraw_tabpanel)
	draw_tabpanel();
# endif

    ಯದಿ (wp->w_redr_status || p_ru
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
    ಪೂರ್ಣಾಙ್ಕ
redraw_asap(ಪೂರ್ಣಾಙ್ಕ type)
{
    ಪೂರ್ಣಾಙ್ಕ		rows;
    ಪೂರ್ಣಾಙ್ಕ		cols = screen_Columns;
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಪೂರ್ಣಾಙ್ಕ		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    ಪೂರ್ಣಾಙ್ಕ		i;
    u8char_T	*screenlineUC = ರಿಕ್ತ;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = ರಿಕ್ತ;	// copy from ScreenLines2[]

    redraw_later(type);
    ಯದಿ (ಸಂದೇಶ_scrolled
	    || (State != MODE_NORMAL && State != MODE_NORMAL_BUSY)
	    || exiting)
	ಪ್ರತ್ಯಾಗಮ ret;

    // Allocate space to save the text displayed in the command line area.
    rows = screen_Rows - cmdline_row;
    screenline = LALLOC_MULT(schar_T, rows * cols);
    screenattr = LALLOC_MULT(sattr_T, rows * cols);
    ಯದಿ (screenline == ರಿಕ್ತ || screenattr == ರಿಕ್ತ)
	ret = 2;
    ಯದಿ (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, rows * cols);
	ಯದಿ (screenlineUC == ರಿಕ್ತ)
	    ret = 2;
	ಪ್ರತಿ (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, rows * cols);
	    ಯದಿ (screenlineC[i] == ರಿಕ್ತ)
		ret = 2;
	}
    }
    ಯದಿ (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, rows * cols);
	ಯದಿ (screenline2 == ರಿಕ್ತ)
	    ret = 2;
    }

    ಯದಿ (ret != 2)
    {
	// Save the text displayed in the command line area.
	ಪ್ರತಿ (r = 0; r < rows; ++r)
	{
	    mch_memmove(screenline + r * cols,
			Screen_ಸಾಲುಗಳು + ಸಾಲು_Offset[cmdline_row + r],
			(size_t)cols * ಪರಿಮಾಣ(schar_T));
	    mch_memmove(screenattr + r * cols,
			ScreenAttrs + ಸಾಲು_Offset[cmdline_row + r],
			(size_t)cols * ಪರಿಮಾಣ(sattr_T));
	    ಯದಿ (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * cols,
			    Screen_ಸಾಲುಗಳು_UC + ಸಾಲು_Offset[cmdline_row + r],
			    (size_t)cols * ಪರಿಮಾಣ(u8char_T));
		ಪ್ರತಿ (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * cols,
				Screen_ಸಾಲುಗಳು_C[i] + ಸಾಲು_Offset[cmdline_row + r],
				(size_t)cols * ಪರಿಮಾಣ(u8char_T));
	    }
	    ಯದಿ (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * cols,
			    ScreenLines2 + ಸಾಲು_Offset[cmdline_row + r],
			    (size_t)cols * ಪರಿಮಾಣ(schar_T));
	}

	update_screen(0);
	ret = 3;

	ಯದಿ (must_redraw == 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ	off = (ಪೂರ್ಣಾಙ್ಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);

	    // Restore the text displayed in the command line area.
	    ಪ್ರತಿ (r = 0; r < rows; ++r)
	    {
		mch_memmove(current_Screen_ಸಾಲು,
			    screenline + r * cols,
			    (size_t)cols * ಪರಿಮಾಣ(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * cols,
			    (size_t)cols * ಪರಿಮಾಣ(sattr_T));
		ಯದಿ (enc_utf8)
		{
		    mch_memmove(Screen_ಸಾಲುಗಳು_UC + off,
				screenlineUC + r * cols,
				(size_t)cols * ಪರಿಮಾಣ(u8char_T));
		    ಪ್ರತಿ (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_ಸಾಲುಗಳು_C[i] + off,
				    screenlineC[i] + r * cols,
				    (size_t)cols * ಪರಿಮಾಣ(u8char_T));
		}
		ಯದಿ (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * cols,
				(size_t)cols * ಪರಿಮಾಣ(schar_T));
		screen_ಸಾಲು(curwin, cmdline_row + r, 0, cols, cols, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    ಯದಿ (enc_utf8)
    {
	vim_free(screenlineUC);
	ಪ್ರತಿ (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    ಯದಿ (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_message();

    setcursor();

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    ಶೂನ್ಯ
redraw_after_callback(ಪೂರ್ಣಾಙ್ಕ call_update_screen, ಪೂರ್ಣಾಙ್ಕ do_message)
{
    ++redrawing_for_callback;

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

    ಯದಿ (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_SETWSIZE || State == MODE_EXTERNCMD
	    || State == MODE_CONFIRM || exmode_active)
    {
	ಯದಿ (do_message)
	    repeat_message();
    }
    ಅನ್ಯಥಾ ಯದಿ (State & MODE_CMDLINE)
    {
	ಯದಿ (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	ಯದಿ (cmdline_row > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    ಯದಿ (ಸಂದೇಶ_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (State & (MODE_NORMAL | MODE_ಸೇರಿಸು | MODE_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	ಯದಿ (ಸಂದೇಶ_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    ಸಂದೇಶ_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	out_flush_cursor(FALSE, FALSE);
    ಅನ್ಯಥಾ
#endif
	out_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    ಶೂನ್ಯ
redraw_later(ಪೂರ್ಣಾಙ್ಕ type)
{
    redraw_win_later(curwin, type);
}

    ಶೂನ್ಯ
redraw_win_later(
    win_T	*wp,
    ಪೂರ್ಣಾಙ್ಕ		type)
{
#ifdef FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    ಯದಿ (inside_redraw_on_start_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	ಯದಿ (type >= UPD_NOT_VALID)
	    wp->w_ಸಾಲುಗಳು_valid = 0;
	ಯದಿ (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    ಶೂನ್ಯ
redraw_later_clear(ಶೂನ್ಯ)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    ಶೂನ್ಯ
redraw_all_later(ಪೂರ್ಣಾಙ್ಕ type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    set_must_redraw(type);
}

#ಯದಿ 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    ಶೂನ್ಯ
redraw_all_windows_later(ಪೂರ್ಣಾಙ್ಕ type)
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
    ಶೂನ್ಯ
set_must_redraw(ಪೂರ್ಣಾಙ್ಕ type)
{
    ಯದಿ (!redraw_not_allowed &&
#ifdef FEAT_EVAL
	    !inside_redraw_on_start_cb &&
#endif
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    ಶೂನ್ಯ
redraw_curbuf_later(ಪೂರ್ಣಾಙ್ಕ type)
{
    redraw_buf_later(curbuf, type);
}

    ಶೂನ್ಯ
redraw_buf_later(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    }
#ifdef FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	ಯದಿ (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	ಯದಿ (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
#endif
#ಯದಿ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    ಯದಿ (curwin->w_buffer == buf)
	redraw_win_later(curwin, type);
#endif
}

#ಯದಿ defined(FEAT_SIGNS)
    ಶೂನ್ಯ
redraw_buf_ಸಾಲು_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_buffer == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#endif

#ಯದಿ defined(FEAT_JOB_CHANNEL)
    ಶೂನ್ಯ
redraw_buf_and_status_later(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ type)
{
    win_T	*wp;

    ಯದಿ (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	ಪ್ರತ್ಯಾಗಮ;
    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ (wp->w_buffer == buf)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ruler_redraw_lastwin(ಶೂನ್ಯ)
{
    ಯದಿ (p_ru && lastwin->w_status_height == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    ಶೂನ್ಯ
status_redraw_all(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_status_height)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    ಶೂನ್ಯ
status_redraw_curbuf(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_status_height != 0 && wp->w_buffer == curbuf)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    ಯದಿ (lastwin->w_buffer == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    ಶೂನ್ಯ
redraw_statuslines(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_redr_status)
	{
	    bool ret = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_status(wp, FALSE);
	    ಯದಿ (ret)
		ತೆಗೆ_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    ಯದಿ (redraw_tabline)
	draw_tabline();

#ಯದಿ defined(FEAT_TABPANEL)
    ಯದಿ (redraw_tabpanel)
	draw_tabpanel();
#endif
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    ಶೂನ್ಯ
win_redraw_last_status(frame_T *frp)
{
    ಯದಿ (frp->fr_layout == FR_LEAF)
	frp->fr_win->w_redr_status = true;
    ಅನ್ಯಥಾ ಯದಿ (frp->fr_layout == FR_ROW)
    {
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    win_redraw_last_status(frp);
    }
    ಅನ್ಯಥಾ // frp->fr_layout == FR_COL
    {
	frp = frp->fr_child;
	ಯಾವತ್ (frp->fr_next != ರಿಕ್ತ)
	    frp = frp->fr_next;
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
    ಶೂನ್ಯ
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    ಶೂನ್ಯ
redraw_win_range_later(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    ಯದಿ (last >= wp->w_topline && first < wp->w_botline)
    {
	ಯದಿ (wp->w_redraw_ತುದಿ == 0 || wp->w_redraw_ತುದಿ > first)
	    wp->w_redraw_ತುದಿ = first;
	ಯದಿ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    ಶೂನ್ಯ
redraw_win_range_now(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    ಯದಿ (last >= wp->w_topline && first < wp->w_botline)
    {
	ಯದಿ (wp->w_redraw_ತುದಿ == 0 || wp->w_redraw_ತುದಿ > first)
	    wp->w_redraw_ತುದಿ = first;
	ಯದಿ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	ಯದಿ (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#ifdef FEAT_EVAL
ಸ್ಥಿರ bool redraw_cb_in_progress = false;

    ಶೂನ್ಯ
f_redraw_listener_add(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		id;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (redraw_cb_in_progress)
    {
	emsg(_(e_cannot_add_redraw_listener_in_listener_callback));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (check_for_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    ಯದಿ (rln == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    ಯದಿ (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	ಯದಿ (cb.cb_name == ರಿಕ್ತ)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	copy_callback(&rln->rl_callbacks.on_start, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    ಯದಿ (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	ಯದಿ (cb.cb_name == ರಿಕ್ತ)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_start);
	    vim_free(rln);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	copy_callback(&rln->rl_callbacks.on_end, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    ಯದಿ (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	ಪ್ರತ್ಯಾಗಮ;
    }

    rln->rl_next = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = id;

    ಪ್ರತ್ಯಾಗಮ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_start);
    free_callback(&rln->rl_callbacks.on_end);

    vim_free(rln);
}


    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_listener_cleanup(ಶೂನ್ಯ)
{
    ಪ್ರತಿ (redraw_listener_T *rln = redraw_listeners; rln != ರಿಕ್ತ;)
    {
	redraw_listener_T *next = rln->rl_next;
	ಯದಿ (rln->rl_id == 0)
	{
	    ಯದಿ (redraw_listeners == rln)
		redraw_listeners = rln->rl_next;
	    redraw_listener_free(rln);
	}
	rln = next;
    }
}

/*
 * Return the redraw listener struct with the specified id. Returns NULL if not
 * found.
 */
    ಸ್ಥಿರ redraw_listener_T *
get_redraw_listener(ಪೂರ್ಣಾಙ್ಕ id)
{
    ಪ್ರತಿ (redraw_listener_T *rln = redraw_listeners; rln != ರಿಕ್ತ; rln = rln->rl_next)
	ಯದಿ (rln->rl_id == id)
	    ಪ್ರತ್ಯಾಗಮ rln;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಶೂನ್ಯ
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ			id;
    redraw_listener_T	*rln;

    ಯದಿ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = argvars[0].vval.v_ಸಂಖ್ಯೆ;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    ಯದಿ (rln == ರಿಕ್ತ)
    {
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_ಸಂಖ್ಯೆ = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_redraw_listener_start_or_end(bool start)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    ಯದಿ (start)
	inside_redraw_on_start_cb = true;

    redraw_cb_in_progress = true;
    ಪ್ರತಿ (redraw_listener_T *rln = redraw_listeners; rln != ರಿಕ್ತ; rln = rln->rl_next)
    {
	ಯದಿ (rln->rl_id == 0)
	    // Listener has been removed, skip
	    ಅನುವರ್ತ;
	ಯದಿ (start && rln->rl_callbacks.on_start.cb_name != ರಿಕ್ತ)
	{
	    call_callback(&rln->rl_callbacks.on_start, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	ಅನ್ಯಥಾ ಯದಿ (rln->rl_callbacks.on_end.cb_name != ರಿಕ್ತ)
	{
	    call_callback(&rln->rl_callbacks.on_end, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_in_progress = false;

    ಯದಿ (start)
	inside_redraw_on_start_cb = false;
}
#endif // FEAT_EVAL
