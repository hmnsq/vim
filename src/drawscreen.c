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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ ਖਾਲੀ win_update(win_T *wp);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
ਸਥਿਰ ਖਾਲੀ redraw_custom_statusline(win_T *wp);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ borrow_stl_vsep_hl(ਖਾਲੀ);
#ਜੇ defined(FEAT_ਖੋਜ_EXTRA) || defined(FEAT_CLIPBOARD)
ਸਥਿਰ ਪੂਰਨ  did_update_one_ਖਿੜਕੀ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਖਾਲੀ redraw_listener_cleanup(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ invoke_redraw_listener_ਸ਼ੁਰੂ_or_ਸਿਰਾ(bool ਸ਼ੁਰੂ);
#ਜੇ_ਅੰਤ

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    ਪੂਰਨ
update_screen(ਪੂਰਨ type_arg)
{
    ਪੂਰਨ		type = type_arg;
    win_T	*wp;
    ਸਥਿਰ ਪੂਰਨ	did_intro = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਪੂਰਨ		did_one = FALSE;
    ਪੂਰਨ		did_undraw = FALSE;
    ਪੂਰਨ		gui_cursor_ਖਾਨਾ = 0;
    ਪੂਰਨ		gui_cursor_ਪੰਕਤੀ = 0;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		no_update = FALSE;
    ਪੂਰਨ		save_pum_will_redraw = pum_will_redraw;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਪੂਰਨ		did_redraw_ਖਿੜਕੀ = FALSE;
#ਜੇ_ਅੰਤ
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    ਜੇ (!screen_valid(TRUE))
	ਵਾਪਸ FAIL;

    ਜੇ (type == UPD_VALID_NO_UPDATE)
    {
	no_update = TRUE;
	type = 0;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    {
	buf_T *buf;

	// Before updating the screen, notify any listeners of changed text.
	FOR_ALL_BUFFERS(buf)
	    invoke_listeners(buf);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    // May have postponed updating diffs.
    ਜੇ (need_diff_redraw)
	diff_redraw(TRUE);
#ਜੇ_ਅੰਤ

    ਜੇ (must_redraw)
    {
	ਜੇ (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    ਜੇ (curwin->w_ਸਤਰਾਂ_valid == 0 && type < UPD_NOT_VALID
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    && !ਪਦ_do_update_ਖਿੜਕੀ(curwin)
#ਜੇ_ਅੰਤ
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    ਜੇ (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	ਜੇ (type > UPD_INVERTED_ALL)
	    curwin->w_ਸਤਰਾਂ_valid = 0;	// don't use w_lines[].wl_size now
	ਵਾਪਸ FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    ਪੂਰਨ hid_cursor = !sync_output_ਸਰਗਰਮ();
    ਜੇ (hid_cursor)
	cursor_off();
    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_ENABLE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // Update popup_mask if needed.  This may set w_redraw_top and w_redraw_bot
    // in some windows.
    may_update_popup_ਨਕਾਬ(type);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ++display_tick;	    // let syntax code know we're in a next round of
			    // display updating
#ਜੇ_ਅੰਤ
    ਜੇ (no_update)
	++no_win_do_ਸਤਰਾਂ_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    ਜੇ (ਸੁਨੇਹਾ_scrolled)
    {
	clear_cmdline = TRUE;
	ਜੇ (type != UPD_CLEAR)
	{
	    ਜੇ (ਸੁਨੇਹਾ_scrolled > ਪੰਕਤੀਆਂ - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    ਨਹੀਂਤਾਂ
	    {
		check_for_delay(FALSE);
		ਜੇ (screen_ins_ਸਤਰਾਂ(0, 0, ਸੁਨੇਹਾ_scrolled, (ਪੂਰਨ)ਪੰਕਤੀਆਂ, 0, ਨਿਰਾ)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    ਜੇ (wp->w_winrow < ਸੁਨੇਹਾ_scrolled)
		    {
			ਜੇ (W_WINROW(wp) + wp->w_ਉਚਾਈ > ਸੁਨੇਹਾ_scrolled
				&& wp->w_redr_type < UPD_REDRAW_ਸਿਖਰ
				&& wp->w_ਸਤਰਾਂ_valid > 0
				&& wp->w_topline == wp->w_ਸਤਰਾਂ[0].wl_lnum)
			{
			    wp->w_upd_ਪੰਕਤੀਆਂ = ਸੁਨੇਹਾ_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_ਸਿਖਰ;
			}
			ਨਹੀਂਤਾਂ
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    ਜੇ (W_WINROW(wp) + wp->w_ਉਚਾਈ
					 + wp->w_ਸਥਿਤੀ_ਉਚਾਈ <= ਸੁਨੇਹਾ_scrolled)
				wp->w_redr_ਸਥਿਤੀ = true;
			}
		    }
		}
		ਜੇ (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#ਜੇ defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#ਜੇ_ਅੰਤ
	}
	ਸੁਨੇਹਾ_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    ਜੇ (need_highlight_ਬਦਲਿਆ)
	highlight_ਬਦਲਿਆ();

    ਜੇ (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    ਜੇ (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    ਜੇ (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? ਸੰਖਿਆ_ਚੌੜਾਈ(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    invoke_redraw_listener_ਸ਼ੁਰੂ_or_ਸਿਰਾ(true);
#ਜੇ_ਅੰਤ

    // Only start redrawing if there is really something to do.
    ਜੇ (type == UPD_INVERTED)
	update_curswant();
    ਜੇ (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_ਸਤਰਾਂ[0].wl_valid
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#ਜੇ_ਅੰਤ
		    && curwin->w_topline == curwin->w_ਸਤਰਾਂ[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_ਸਰਗਰਮ
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_ਬਹੁਲਕ == VIsual_ਬਹੁਲਕ
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    ਜੇ (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#ਜੇ defined(FEAT_TABPANEL)
    ਜੇ (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (wp->w_ਬਫਰ->b_mod_ਸੈੱਟ)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    ਲਈ (wwp = firstwin; wwp != wp; wwp = wwp->w_ਅਗਲਾ)
		ਜੇ (wwp->w_ਬਫਰ == wp->w_ਬਫਰ)
		    ਰੋਕੋ;
	    ਜੇ (wwp == wp && syntax_present(wp))
		syn_ਢੇਰ_apply_changes(wp->w_ਬਫਰ);
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (pum_redraw_ਵਿੱਚ_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#ਜੇ defined(FEAT_ਖੋਜ_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_ਖਿੜਕੀ = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    screen_ਖੋਜ_hl.rm.regprog = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    FOR_ALL_WINDOWS(wp)
    {
	override_success = ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ਜੇ (wp->w_redr_type != 0)
	{
	    cursor_off();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    did_redraw_ਖਿੜਕੀ = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    ਜੇ (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		ਜੇ (gui.ਵਿੱਚ_use && (wp == curwin || cursor_is_sleeping()))
		{
		    gui_cursor_ਖਾਨਾ = gui.cursor_ਖਾਨਾ;
		    gui_cursor_ਪੰਕਤੀ = gui.cursor_ਪੰਕਤੀ;
		    gui_undraw_cursor();
		    did_undraw = TRUE;
		}
	    }
#ਜੇ_ਅੰਤ
	    win_update(wp);
	}

	// redraw status line after the window to minimize cursor movement
	ਜੇ (wp->w_redr_ਸਥਿਤੀ)
	{
	    cursor_off();
	    win_redr_ਸਥਿਤੀ(wp, TRUE); // any popup menu will be redrawn below
	}

	ਜੇ (override_success)
	    ਕੱਢੋ_highlight_overrides();

    }
#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
    ਸਿਰਾ_ਖੋਜ_hl();
#ਜੇ_ਅੰਤ

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    ਜੇ (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    ਜੇ (wp->w_vsep_ਚੌੜਾਈ > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_ਬਫਰ->b_mod_ਸੈੱਟ = false;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    ਜੇ (did_redraw_ਖਿੜਕੀ || popup_need_redraw())
	update_popups(win_update);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    FOR_ALL_WINDOWS(wp)
	// If this window contains a terminal, after redrawing all windows, the
	// dirty row range can be reset.
	ਪਦ_did_update_ਖਿੜਕੀ(wp);
#ਜੇ_ਅੰਤ

    after_updating_screen(TRUE);

    // Clear or redraw the command line.  Done last, because scrolling may
    // mess up the command line.
    ਜੇ (clear_cmdline || redraw_cmdline || redraw_ਬਹੁਲਕ)
	showmode();

    ਜੇ (no_update)
	--no_win_do_ਸਤਰਾਂ_ins;

    // May put up an introductory message when not editing a file
    ਜੇ (!did_intro)
	maybe_intro_ਸੁਨੇਹਾ();
    did_intro = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	ਜੇ (did_undraw && !gui_mch_is_blink_off())
	{
	    mch_disable_flush();
	    ਬਾਹਰਲਾ_flush();	// required before updating the cursor
	    mch_enable_flush();

	    // Put the GUI position where the cursor was, gui_update_cursor()
	    // uses that.
	    gui.ਖਾਨਾ = gui_cursor_ਖਾਨਾ;
	    gui.ਪੰਕਤੀ = gui_cursor_ਪੰਕਤੀ;
	    gui.ਖਾਨਾ = mb_fix_ਖਾਨਾ(gui.ਖਾਨਾ, gui.ਪੰਕਤੀ);
	    gui_update_cursor(FALSE, FALSE);
	    gui_may_flush();
	    screen_cur_ਖਾਨਾ = gui.ਖਾਨਾ;
	    screen_cur_ਪੰਕਤੀ = gui.ਪੰਕਤੀ;
	}
	ਨਹੀਂਤਾਂ
	    ਬਾਹਰਲਾ_flush();
	gui_update_scrollbars(FALSE);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
    // Make sure to do this last!
    draw_image_placements();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    invoke_redraw_listener_ਸ਼ੁਰੂ_or_ਸਿਰਾ(false);
    redraw_listener_cleanup();
#ਜੇ_ਅੰਤ

    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_DISABLE);
    ਜੇ (hid_cursor)
	cursor_on();

    ਵਾਪਸ OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    ਪੂਰਨ
statusline_ਪੰਕਤੀ(win_T *wp)
{
#ਜੇ defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    ਜੇ (wp->w_ਚੌਖਟਾ->fr_ਉਚਾਈ == wp->w_ਸਥਿਤੀ_ਉਚਾਈ && !popup_is_popup(wp))
	ਵਾਪਸ wp->w_winrow;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ W_WINROW(wp) + wp->w_ਉਚਾਈ;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    ਖਾਲੀ
win_redr_ਸਥਿਤੀ(win_T *wp, ਪੂਰਨ ignore_pum ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		fillchar;
    ਪੂਰਨ		attr;
    ਪੂਰਨ		i;
    ਸਥਿਰ ਪੂਰਨ  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    ਜੇ (busy)
	ਵਾਪਸ;
    busy = TRUE;

    ਪੰਕਤੀ = statusline_ਪੰਕਤੀ(wp);

    wp->w_redr_ਸਥਿਤੀ = false;
    ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_ਸਥਿਤੀ = true;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    ਨਹੀਂਤਾਂ ਜੇ (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
    {
	char_u	*p;
	ਪੂਰਨ	plen;
	ਪੂਰਨ	NameBufflen;
	ਪੂਰਨ	this_ru_ਖਾਨਾ;
	ਪੂਰਨ	n;			// scratch value

	fillchar = fillchar_ਸਥਿਤੀ(&attr, wp);

	get_trans_bufname(wp->w_ਬਫਰ);
	p = NameBuff;
	plen = (ਪੂਰਨ)STRLEN(p);

	ਜੇ ((bt_help(wp->w_ਬਫਰ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		    || wp->w_p_pvw
#ਜੇ_ਅੰਤ
		    || buf_Is_ਬਦਲਿਆ(wp->w_ਬਫਰ)
		    || wp->w_ਬਫਰ->b_p_ro)
		&& plen < MAXPATHL - 1)
	{
	    *(p + plen++) = ' ';	// replace NUL with space
	    *(p + plen) = NUL;		// NUL terminate the string
	}
	ਜੇ (bt_help(wp->w_ਬਫਰ))
	    plen += vim_snprintf((ਅੱਖਰ *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	ਜੇ (wp->w_p_pvw)
	    plen += vim_snprintf((ਅੱਖਰ *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#ਜੇ_ਅੰਤ
	ਜੇ (buf_Is_ਬਦਲਿਆ(wp->w_ਬਫਰ) && !bt_terminal(wp->w_ਬਫਰ))
	    plen += vim_snprintf((ਅੱਖਰ *)p + plen, MAXPATHL - plen, "%s", "[+]");
	ਜੇ (wp->w_ਬਫਰ->b_p_ro)
	    plen += vim_snprintf((ਅੱਖਰ *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_ਖਾਨਾ = ru_ਖਾਨਾ - (cmdline_ਚੌੜਾਈ - wp->w_ਚੌੜਾਈ);
	n = (wp->w_ਚੌੜਾਈ + 1) / 2;
	ਜੇ (this_ru_ਖਾਨਾ < n)
	    this_ru_ਖਾਨਾ = n;
	ਜੇ (this_ru_ਖਾਨਾ <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    ਲਈ (i = 0; p[i] != NUL && plen >= this_ru_ਖਾਨਾ - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    ਜੇ (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (plen > this_ru_ਖਾਨਾ - 1)
	{
	    p += plen - (this_ru_ਖਾਨਾ - 1);
	    *p = '<';
	    plen = this_ru_ਖਾਨਾ - 1;
	}

	screen_puts(p, ਪੰਕਤੀ, wp->w_wincol, attr);
	screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1, plen + wp->w_wincol,
		    this_ru_ਖਾਨਾ + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	ਲਈ (i = 1; i < wp->w_ਸਥਿਤੀ_ਉਚਾਈ; i++)
	    screen_fill(ਪੰਕਤੀ + i, ਪੰਕਤੀ + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	ਜੇ ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_ਖਾਨਾ - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, ਪੰਕਤੀ, (ਪੂਰਨ)(this_ru_ਖਾਨਾ - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	ਜੇ (p_sc && *p_sloc == 's')
	{
	    n = this_ru_ਖਾਨਾ - plen - 2;		    // perform the calculation here so we only do it once
	    ਪੂਰਨ	ਚੌੜਾਈ = MIN(10, n);

	    ਜੇ (ਚੌੜਾਈ > 0)
		screen_puts_len(showcmd_buf, ਚੌੜਾਈ, ਪੰਕਤੀ,
				wp->w_wincol + this_ru_ਖਾਨਾ - ਚੌੜਾਈ - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    ਜੇ (wp->w_vsep_ਚੌੜਾਈ != 0 && wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0 && redrawing())
    {
	ਲਈ (i = 0; i < wp->w_ਸਥਿਤੀ_ਉਚਾਈ; i++)
	{
	    ਪੂਰਨ r = ਪੰਕਤੀ + i;
	    ਜੇ (stl_connected(wp))
		fillchar = fillchar_ਸਥਿਤੀ(&attr, wp);
	    ਨਹੀਂਤਾਂ
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
    ਸਥਿਰ ਖਾਲੀ
borrow_stl_vsep_hl(ਖਾਲੀ)
{
    win_T   *ਖੱਬਾ = ਨਿਰਾ;
    win_T   *ਸੱਜਾ = ਨਿਰਾ;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    ਜੇ (!redrawing() || Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    FOR_ALL_WINDOWS(ਖੱਬਾ)
    {
	ਜੇ (ਖੱਬਾ->w_ਸਥਿਤੀ_ਉਚਾਈ == 0 || ਖੱਬਾ->w_vsep_ਚੌੜਾਈ == 0)
	    ਜਾਰੀ;
	// A window of zero width has no status line cell to borrow from.
	ਜੇ (ਖੱਬਾ->w_ਚੌੜਾਈ == 0)
	    ਜਾਰੀ;
	ਜੇ (!stl_connected(ਖੱਬਾ))
	    ਜਾਰੀ;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = ਨਿਰਾ;
	ਪੂਰਨ ਸ਼ੁਰੂ = 0;
	ਪੂਰਨ ਸਿਰਾ = 0;

	FOR_ALL_WINDOWS(ਸੱਜਾ)
	{
	    ਜੇ (ਸੱਜਾ == ਖੱਬਾ || ਸੱਜਾ->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)
		ਜਾਰੀ;
	    ਜੇ (ਸੱਜਾ->w_wincol != W_ENDCOL(ਖੱਬਾ) + 1)
		ਜਾਰੀ;
	    ਪੂਰਨ l_stl_ਪੰਕਤੀ = W_WINROW(ਖੱਬਾ) + ਖੱਬਾ->w_ਉਚਾਈ;
	    ਪੂਰਨ r_stl_ਪੰਕਤੀ = W_WINROW(ਸੱਜਾ) + ਸੱਜਾ->w_ਉਚਾਈ;

	    ਸ਼ੁਰੂ = l_stl_ਪੰਕਤੀ > r_stl_ਪੰਕਤੀ ? l_stl_ਪੰਕਤੀ : r_stl_ਪੰਕਤੀ;
	    ਸਿਰਾ = l_stl_ਪੰਕਤੀ + ਖੱਬਾ->w_ਸਥਿਤੀ_ਉਚਾਈ
				< r_stl_ਪੰਕਤੀ + ਸੱਜਾ->w_ਸਥਿਤੀ_ਉਚਾਈ
		? l_stl_ਪੰਕਤੀ + ਖੱਬਾ->w_ਸਥਿਤੀ_ਉਚਾਈ
		: r_stl_ਪੰਕਤੀ + ਸੱਜਾ->w_ਸਥਿਤੀ_ਉਚਾਈ;
	    ਜੇ (ਸ਼ੁਰੂ < ਸਿਰਾ)
	    {
		neighbour = ਸੱਜਾ;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (neighbour == ਨਿਰਾ)
	    ਜਾਰੀ;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	ਪੂਰਨ	hl;
	ਜੇ (ਖੱਬਾ != curwin && neighbour != curwin
		&& fillchar_ਸਥਿਤੀ(&hl, ਖੱਬਾ) != ' ')
	    ਜਾਰੀ;

	// Source: prefer curwin's side; otherwise left window's right edge.
	ਪੂਰਨ dst_ਖਾਨਾ = W_ENDCOL(ਖੱਬਾ);
	ਪੂਰਨ src_ਖਾਨਾ = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(ਖੱਬਾ) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	ਜੇ (dst_ਖਾਨਾ >= screen_Columns || src_ਖਾਨਾ >= screen_Columns)
	    ਜਾਰੀ;
	ਜੇ (ਸਿਰਾ > screen_ਪੰਕਤੀਆਂ)
	    ਸਿਰਾ = screen_ਪੰਕਤੀਆਂ;

	ਲਈ (ਪੂਰਨ r = ਸ਼ੁਰੂ; r < ਸਿਰਾ; r++)
	{
	    ਬਿਨਾਨਿਸ਼ਾਨੀ	dst_off = ਸਤਰ_ਵਿੱਥ[r] + dst_ਖਾਨਾ;
	    sattr_T	attr = ScreenAttrs[ਸਤਰ_ਵਿੱਥ[r] + src_ਖਾਨਾ];

	    ਜੇ (ScreenAttrs[dst_off] != attr)
	    {
		ScreenAttrs[dst_off] = attr;
		screen_char(dst_off, r, dst_ਖਾਨਾ);
	    }
	}
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
/*
 * Redraw the status line according to 'statusline' and take care of any
 * errors encountered.
 */
    ਸਥਿਰ ਖਾਲੀ
redraw_custom_statusline(win_T *wp)
{
    ਸਥਿਰ ਪੂਰਨ	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    ਜੇ (entered)
	ਵਾਪਸ;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    ਖਾਲੀ
showruler(ਪੂਰਨ always)
{
    ਜੇ (!always && !redrawing())
	ਵਾਪਸ;
    ਜੇ (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_ਸਥਿਤੀ = true;
	ਵਾਪਸ;
    }
#ਜੇ defined(FEAT_STL_OPT)
    ਜੇ ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_ਸਥਿਤੀ_ਉਚਾਈ)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	win_redr_ruler(curwin, always, FALSE);

    ਜੇ (need_maketitle
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_ਵਿੱਚ_ICON))
	    || (p_title && (stl_syntax & STL_ਵਿੱਚ_TITLE))
#ਜੇ_ਅੰਤ
       )
	maketitle();

    // Redraw the tab pages line if needed.
    ਜੇ (redraw_tabline)
	draw_tabline();

#ਜੇ defined(FEAT_TABPANEL)
    ਜੇ (redraw_tabpanel)
	draw_tabpanel();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
win_redr_ruler(win_T *wp, ਪੂਰਨ always, ਪੂਰਨ ignore_pum)
{
    ਪੂਰਨ	ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ = FALSE;

    // If 'ruler' off don't do anything
    ਜੇ (!p_ru)
	ਵਾਪਸ;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    ਜੇ (wp->w_cursor.lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    ਜੇ (wp == lastwin && lastwin->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)
	ਜੇ (edit_submode != ਨਿਰਾ)
	    ਵਾਪਸ;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    ਜੇ (!ignore_pum && pum_visible())
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    ਜੇ (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) == 0
		&& *ml_get_buf(wp->w_ਬਫਰ, wp->w_cursor.lnum, FALSE) == NUL)
	ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    ਜੇ (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.ਖਾਨਾ != wp->w_ru_cursor.ਖਾਨਾ
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ != wp->w_ru_ਸਤਰ_ਗਿਣਤੀ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#ਜੇ_ਅੰਤ
	    || ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ != wp->w_ru_ਖਾਲੀ_ਥਾਵਾਂ)
    {
	ਪੂਰਨ	ਪੰਕਤੀ;
	ਪੂਰਨ	fillchar;
	ਪੂਰਨ	attr;
	ਪੂਰਨ	off;
	ਪੂਰਨ	ਚੌੜਾਈ;
	colnr_T	virtcol;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RULER_BUF_LEN 70
	char_u	ਬਫਰ[RULER_BUF_LEN];
	ਪੂਰਨ	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	ਪੂਰਨ	rel_poslen;
	ਪੂਰਨ	this_ru_ਖਾਨਾ;
	ਪੂਰਨ	n1;			    // scratch value
	ਪੂਰਨ	n2;			    // scratch value
	bool	override_success =
	    ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ)
	{
	    ਪੰਕਤੀ = statusline_ਪੰਕਤੀ(wp);
	    fillchar = fillchar_ਸਥਿਤੀ(&attr, wp);
	    off = wp->w_wincol;
	    ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_ਖਾਨਾ_off;
	    ਚੌੜਾਈ = cmdline_ਚੌੜਾਈ;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	ਜੇ (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, ਨਿਰਾ, &virtcol, ਨਿਰਾ, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((ਅੱਖਰ *)ਬਫਰ, RULER_BUF_LEN, _("%ld,"),
		(wp->w_ਬਫਰ->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
		    ? 0L
		    : (ਲੰਮਾ)(wp->w_cursor.lnum));
	bufferlen += ਖਾਨਾ_print(ਬਫਰ + bufferlen, RULER_BUF_LEN - bufferlen,
			ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ ? 0 : (ਪੂਰਨ)wp->w_cursor.ਖਾਨਾ + 1,
			(ਪੂਰਨ)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)	// can't use last char of screen
	    ++n1;

	this_ru_ਖਾਨਾ = ru_ਖਾਨਾ - (cmdline_ਚੌੜਾਈ - ਚੌੜਾਈ);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (ਚੌੜਾਈ + 1) / 2;
	ਜੇ (this_ru_ਖਾਨਾ < n2)
	    this_ru_ਖਾਨਾ = n2;
	ਜੇ (this_ru_ਖਾਨਾ + n1 < ਚੌੜਾਈ)
	{
	    // need at least space for rel_pos + NUL
	    ਜਦੋਂ (this_ru_ਖਾਨਾ + n1 < ਚੌੜਾਈ
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		ਜੇ (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, ਬਫਰ + bufferlen);
		ਨਹੀਂਤਾਂ
		    ਬਫਰ[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((ਅੱਖਰ *)ਬਫਰ + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	ਜੇ (has_mbyte)
	{
	    ਲਈ (n1 = 0, n2 = 0; ਬਫਰ[n1] != NUL; n1 += (*mb_ptr2len)(ਬਫਰ + n1))
	    {
		n2 += (*mb_ptr2cells)(ਬਫਰ + n1);
		ਜੇ (this_ru_ਖਾਨਾ + n2 > ਚੌੜਾਈ)
		{
		    bufferlen = n1;
		    ਬਫਰ[bufferlen] = NUL;
		    ਰੋਕੋ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (this_ru_ਖਾਨਾ + bufferlen > ਚੌੜਾਈ)
	{
	    bufferlen = ਚੌੜਾਈ - this_ru_ਖਾਨਾ;
	    ਬਫਰ[bufferlen] = NUL;
	}

	screen_puts(ਬਫਰ, ਪੰਕਤੀ, this_ru_ਖਾਨਾ + off, attr);
	n1 = redraw_cmdline;
	screen_fill(ਪੰਕਤੀ, ਪੰਕਤੀ + 1,
		this_ru_ਖਾਨਾ + off + bufferlen,
		(off + ਚੌੜਾਈ),
		fillchar, fillchar, attr);
	// don't redraw the cmdline because of showing the ruler
	redraw_cmdline = n1;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_ਖਾਲੀ_ਥਾਵਾਂ = ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_ਸਤਰ_ਗਿਣਤੀ = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#ਜੇ_ਅੰਤ

	ਜੇ (override_success)
	    ਕੱਢੋ_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    ਖਾਲੀ
after_updating_screen(ਪੂਰਨ may_resize_shell ਅਣਵਰਤਿਆ)
{
    updating_screen = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (may_resize_shell)
	gui_may_resize_shell();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪਦ_check_channel_closed_recently();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_DROP_FILE
    // If handle_drop() was called while updating_screen was TRUE need to
    // handle the drop now.
    handle_any_postponed_drop();
#ਜੇ_ਅੰਤ
}

/*
 * Update all windows that are editing the current buffer.
 */
    ਖਾਲੀ
update_curbuf(ਪੂਰਨ type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#ਜੇ defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਾਠ_ਮੰਜ਼ਿਲ_screenline(win_T *wp, char_u *ਪਾਠ, ਪੂਰਨ ਖਾਨਾ)
{
    ਪੂਰਨ		off = (ਪੂਰਨ)(current_Screen_ਸਤਰ - Screen_ਸਤਰਾਂ);

    ਜੇ (has_mbyte)
    {
	ਪੂਰਨ	cells;
	ਪੂਰਨ	u8c, u8cc[MAX_MCO];
	ਪੂਰਨ	i;
	ਪੂਰਨ	idx;
	ਪੂਰਨ	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	ਪੂਰਨ	prev_c = 0;		// previous Arabic character
	ਪੂਰਨ	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	ਜੇ (wp->w_p_rl)
	    idx = off;
	ਨਹੀਂਤਾਂ
# endif
	    idx = off + ਖਾਨਾ;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	ਲਈ (p = ਪਾਠ; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    ਜੇ (ਖਾਨਾ + cells > wp->w_ਚੌੜਾਈ)
		ਰੋਕੋ;
# ifdef FEAT_RIGHTLEFT
	    ਜੇ (wp->w_p_rl)
		idx = off + wp->w_ਚੌੜਾਈ - ਖਾਨਾ - cells;
# endif
	    Screen_ਸਤਰਾਂ[idx] = *p;
	    ਜੇ (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		ਜੇ (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_ਸਤਰਾਂ_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    prev_c = u8c;
# endif
		}
		ਨਹੀਂਤਾਂ
		{
# ifdef FEAT_ARABIC
		    ਜੇ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			ਪੂਰਨ	pc, pc1, nc;
			ਪੂਰਨ	pcc[MAX_MCO];
			ਪੂਰਨ	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ਜੇ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			ਨਹੀਂਤਾਂ
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			Screen_ਸਤਰਾਂ[idx] = firstbyte;
		    }
		    ਨਹੀਂਤਾਂ
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_ਸਤਰਾਂ_UC[idx] = u8c;
		    ਲਈ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ਸਤਰਾਂ_C[i][idx] = u8cc[i];
			ਜੇ (u8cc[i] == 0)
			    ਰੋਕੋ;
		    }
		}
		ਜੇ (cells > 1)
		    Screen_ਸਤਰਾਂ[idx + 1] = 0;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    ਨਹੀਂਤਾਂ ਜੇ (cells > 1)
		// double-width character
		Screen_ਸਤਰਾਂ[idx + 1] = p[1];
	    ਖਾਨਾ += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ len = (ਪੂਰਨ)STRLEN(ਪਾਠ);
	ਪੂਰਨ n = wp->w_ਚੌੜਾਈ - ਖਾਨਾ;

	ਜੇ (len > n)
	    len = n;
	ਜੇ (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    ਜੇ (wp->w_p_rl)
		mch_memmove(current_Screen_ਸਤਰ, ਪਾਠ, len);
	    ਨਹੀਂਤਾਂ
# endif
		mch_memmove(current_Screen_ਸਤਰ + ਖਾਨਾ, ਪਾਠ, len);
	    ਖਾਨਾ += len;
	}
    }
    ਵਾਪਸ ਖਾਨਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
/*
 * Draw the window toolbar.
 */
    ਸਥਿਰ ਖਾਲੀ
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    ਪੂਰਨ		ਵਸਤੂ_idx = 0;
    ਪੂਰਨ		ਵਸਤੂ_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		ਖਾਨਾ = 0;
    ਪੂਰਨ		ਅਗਲਾ_ਖਾਨਾ;
    ਪੂਰਨ		off = (ਪੂਰਨ)(current_Screen_ਸਤਰ - Screen_ਸਤਰਾਂ);
    bool	override_success =
	ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ਪੂਰਨ		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    ਪੂਰਨ		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_ਚੀਜ਼ਾਂ);
    FOR_ALL_ਬੱਚਾ_MENUS(wp->w_winbar, menu)
	++ਵਸਤੂ_ਗਿਣਤੀ;
    wp->w_winbar_ਚੀਜ਼ਾਂ = ALLOC_CLEAR_MULT(winbar_ਵਸਤੂ_T, ਵਸਤੂ_ਗਿਣਤੀ + 1);

    // TODO: use fewer spaces if there is not enough room
    ਲਈ (menu = wp->w_winbar->children;
			  menu != ਨਿਰਾ && ਖਾਨਾ < wp->w_ਚੌੜਾਈ; menu = menu->ਅਗਲਾ)
    {
	space_ਮੰਜ਼ਿਲ_screenline(off + ਖਾਨਾ, fill_attr);
	ਜੇ (++ਖਾਨਾ >= wp->w_ਚੌੜਾਈ)
	    ਰੋਕੋ;
	ਜੇ (ਖਾਨਾ > 1)
	{
	    space_ਮੰਜ਼ਿਲ_screenline(off + ਖਾਨਾ, fill_attr);
	    ਜੇ (++ਖਾਨਾ >= wp->w_ਚੌੜਾਈ)
		ਰੋਕੋ;
	}

	wp->w_winbar_ਚੀਜ਼ਾਂ[ਵਸਤੂ_idx].wb_startcol = ਖਾਨਾ;
	space_ਮੰਜ਼ਿਲ_screenline(off + ਖਾਨਾ, button_attr);
	ਜੇ (++ਖਾਨਾ >= wp->w_ਚੌੜਾਈ)
	    ਰੋਕੋ;

	ਅਗਲਾ_ਖਾਨਾ = ਪਾਠ_ਮੰਜ਼ਿਲ_screenline(wp, menu->name, ਖਾਨਾ);
	ਜਦੋਂ (ਖਾਨਾ < ਅਗਲਾ_ਖਾਨਾ)
	{
	    ScreenAttrs[off + ਖਾਨਾ] = button_attr;
	    ++ਖਾਨਾ;
	}
	wp->w_winbar_ਚੀਜ਼ਾਂ[ਵਸਤੂ_idx].wb_endcol = ਖਾਨਾ;
	wp->w_winbar_ਚੀਜ਼ਾਂ[ਵਸਤੂ_idx].wb_menu = menu;
	++ਵਸਤੂ_idx;

	ਜੇ (ਖਾਨਾ >= wp->w_ਚੌੜਾਈ)
	    ਰੋਕੋ;
	space_ਮੰਜ਼ਿਲ_screenline(off + ਖਾਨਾ, button_attr);
	++ਖਾਨਾ;
    }
    ਜਦੋਂ (ਖਾਨਾ < wp->w_ਚੌੜਾਈ)
    {
	space_ਮੰਜ਼ਿਲ_screenline(off + ਖਾਨਾ, fill_attr);
	++ਖਾਨਾ;
    }
    wp->w_winbar_ਚੀਜ਼ਾਂ[ਵਸਤੂ_idx].wb_menu = ਨਿਰਾ; // end marker

    screen_ਸਤਰ(wp, wp->w_winrow, wp->w_wincol, wp->w_ਚੌੜਾਈ,
							  wp->w_ਚੌੜਾਈ, -1, 0);

    ਜੇ (override_success)
	ਕੱਢੋ_highlight_overrides();
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    ਸਥਿਰ ਖਾਲੀ
copy_ਪਾਠ_attr(
    ਪੂਰਨ		off,
    char_u	*buf,
    ਪੂਰਨ		len,
    ਪੂਰਨ		attr)
{
    ਪੂਰਨ		i;

    mch_memmove(Screen_ਸਤਰਾਂ + off, buf, (size_t)len);
    ਜੇ (enc_utf8)
	vim_memset(Screen_ਸਤਰਾਂ_UC + off, 0, ਆਕਾਰ(u8char_T) * (size_t)len);
    ਲਈ (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    ਸਥਿਰ ਖਾਲੀ
fold_ਸਤਰ(
    win_T	*wp,
    ਲੰਮਾ	fold_ਗਿਣਤੀ,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    ਪੂਰਨ		ਪੰਕਤੀ)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*ਸਿਖਰ, *bot;
    linenr_T	lnume = lnum + fold_ਗਿਣਤੀ - 1;
    ਪੂਰਨ		len;
    char_u	*ਪਾਠ;
    ਪੂਰਨ		fdc;
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		txtcol;
    ਪੂਰਨ		off = (ਪੂਰਨ)(current_Screen_ਸਤਰ - Screen_ਸਤਰਾਂ);
    ਪੂਰਨ		ri;

    // Build the fold line:
    // 1. Add the cmdwin_type for the command-line window
    // 2. Add the 'foldcolumn'
    // 3. Add the 'number' or 'relativenumber' column
    // 4. Compose the text
    // 5. Add the text
    // 6. set highlighting for the Visual area an other text
    ਖਾਨਾ = 0;

    // 1. Add the cmdwin_type for the command-line window
    // Ignores 'rightleft', this window is never right-left.
    ਜੇ (wp == cmdwin_win)
    {
	Screen_ਸਤਰਾਂ[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	ਜੇ (enc_utf8)
	    Screen_ਸਤਰਾਂ_UC[off] = 0;
	++ਖਾਨਾ;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    ਕਰੋ { \
	ਜੇ (wp->w_p_rl) \
	    ਲਈ (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_ਚੌੜਾਈ - (p) - (l)) + ri] = v; \
	 ਨਹੀਂਤਾਂ \
	    ਲਈ (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } ਜਦੋਂ (0)
# ਨਹੀਂਤਾਂ
#  define RL_MEMSET(p, v, l) \
    ਕਰੋ { \
	ਲਈ (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } ਜਦੋਂ (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, ਖਾਨਾ);
    ਜੇ (fdc > 0)
    {
	char_u	*p;
	ਪੂਰਨ	i;
	ਪੂਰਨ	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	ਲਈ (i = 0; i < fdc; i++)
	{
	    ਪੂਰਨ		ch;

	    ਜੇ (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    ਨਹੀਂਤਾਂ
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    ਜੇ (wp->w_p_rl)
		idx = off + wp->w_ਚੌੜਾਈ - i - 1 - ਖਾਨਾ;
	    ਨਹੀਂਤਾਂ
# endif
		idx = off + ਖਾਨਾ + i;
	    ਜੇ (enc_utf8)
	    {
		ਜੇ (ch >= 0x80)
		{
		    Screen_ਸਤਰਾਂ_UC[idx] = ch;
		    Screen_ਸਤਰਾਂ_C[0][idx] = 0;
		    Screen_ਸਤਰਾਂ[idx] = 0x80;
		}
		ਨਹੀਂਤਾਂ
		{
		    Screen_ਸਤਰਾਂ[idx] = ch;
		    Screen_ਸਤਰਾਂ_UC[idx] = 0;
		}
	    }
	    ਨਹੀਂਤਾਂ
		Screen_ਸਤਰਾਂ[idx] = ch;
	}

	RL_MEMSET(ਖਾਨਾ, HL_ATTR(HLF_FC), fdc);
	ਖਾਨਾ += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    //ਪਾਠ
    RL_MEMSET(ਖਾਨਾ, HL_ATTR(HLF_FL), wp->w_ਚੌੜਾਈ - ਖਾਨਾ);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    ਜੇ (signcolumn_on(wp))
    {
	len = wp->w_ਚੌੜਾਈ - ਖਾਨਾ;
	ਜੇ (len > 0)
	{
	    ਜੇ (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    ਜੇ (wp->w_p_rl)
		// the line number isn't reversed
		copy_ਪਾਠ_attr(off + wp->w_ਚੌੜਾਈ - len - ਖਾਨਾ,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    ਨਹੀਂਤਾਂ
#  endif
		copy_ਪਾਠ_attr(off + ਖਾਨਾ, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    ਖਾਨਾ += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    ਜੇ (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_ਚੌੜਾਈ - ਖਾਨਾ;
	ਜੇ (len > 0)
	{
	    ਪੂਰਨ	    w = ਸੰਖਿਆ_ਚੌੜਾਈ(wp);
	    ਲੰਮਾ    ਅੰਕ;
	    ਅੱਖਰ    *fmt = "%*ld ";

	    ਜੇ (len > w + 1)
		len = w + 1;

	    ਜੇ (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		ਅੰਕ = (ਲੰਮਾ)lnum;
	    ਨਹੀਂਤਾਂ
	    {
		// 'relativenumber', don't use negative numbers
		ਅੰਕ = labs((ਲੰਮਾ)get_cursor_rel_lnum(wp, lnum));
		ਜੇ (ਅੰਕ == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    ਅੰਕ = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((ਅੱਖਰ *)buf, ਆਕਾਰ(buf), fmt, w, ਅੰਕ);
# ifdef FEAT_RIGHTLEFT
	    ਜੇ (wp->w_p_rl)
		// the line number isn't reversed
		copy_ਪਾਠ_attr(off + wp->w_ਚੌੜਾਈ - len - ਖਾਨਾ, buf, len,
							     HL_ATTR(HLF_FL));
	    ਨਹੀਂਤਾਂ
# endif
		copy_ਪਾਠ_attr(off + ਖਾਨਾ, buf, len, HL_ATTR(HLF_FL));
	    ਖਾਨਾ += len;
	}
    }

    // 4. Compose the folded-line string with 'foldtext', if set.
    ਪਾਠ = get_foldtext(wp, lnum, lnume, foldinfo, buf);

    txtcol = ਖਾਨਾ;	// remember where text starts

    // 5. move the text to current_ScreenLine.  Fill up with "fold" from
    //    'fillchars'.
    //    Right-left text is put in columns 0 - number-col, normal text is put
    //    in columns number-col - window-width.
    ਖਾਨਾ = ਪਾਠ_ਮੰਜ਼ਿਲ_screenline(wp, ਪਾਠ, ਖਾਨਾ);

    // Fill the rest of the line with the fold filler
    ਜਦੋਂ (ਖਾਨਾ < wp->w_ਚੌੜਾਈ)
    {
	ਪੂਰਨ c = wp->w_fill_chars.fold;
	ਪੂਰਨ idx = off + ਖਾਨਾ;

# ifdef FEAT_RIGHTLEFT
	ਜੇ (wp->w_p_rl)
	    idx = off + wp->w_ਚੌੜਾਈ - 1 - ਖਾਨਾ;
# endif

	ਜੇ (enc_utf8)
	{
	    ਜੇ (c >= 0x80)
	    {
		Screen_ਸਤਰਾਂ_UC[idx] = c;
		Screen_ਸਤਰਾਂ_C[0][idx] = 0;
		Screen_ਸਤਰਾਂ[idx] = 0x80; // avoid storing zero
	    }
	    ਨਹੀਂਤਾਂ
	    {
		Screen_ਸਤਰਾਂ_UC[idx] = 0;
		Screen_ਸਤਰਾਂ[idx] = c;
	    }
	}
	ਨਹੀਂਤਾਂ
	    Screen_ਸਤਰਾਂ[idx] = c;
	++ਖਾਨਾ;
    }

    ਜੇ (ਪਾਠ != buf)
	vim_free(ਪਾਠ);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    ਜੇ (VIsual_ਸਰਗਰਮ && wp->w_ਬਫਰ == curwin->w_ਬਫਰ)
    {
	ਜੇ (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    ਸਿਖਰ = &curwin->w_cursor;
	    bot = &VIsual;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Visual is before curwin->w_cursor
	    ਸਿਖਰ = &VIsual;
	    bot = &curwin->w_cursor;
	}
	ਜੇ (lnum >= ਸਿਖਰ->lnum
		&& lnume <= bot->lnum
		&& (VIsual_ਬਹੁਲਕ != 'v'
		    || ((lnum > ਸਿਖਰ->lnum
			    || (lnum == ਸਿਖਰ->lnum
				&& ਸਿਖਰ->ਖਾਨਾ == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->ਖਾਨਾ - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_ਬਫਰ, lnume))))))
	{
	    ਜੇ (VIsual_ਬਹੁਲਕ == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		ਜੇ (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_ਚੌੜਾਈ)
		{
		    ਜੇ (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_ਚੌੜਾਈ)
			len = wp->w_old_cursor_lcol;
		    ਨਹੀਂਤਾਂ
			len = wp->w_ਚੌੜਾਈ - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (ਪੂਰਨ)wp->w_old_cursor_fcol);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_ਚੌੜਾਈ - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    ਜੇ (wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ)
    {
	ਪੂਰਨ i = 0;
	ਪੂਰਨ j = wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ[i];
	ਪੂਰਨ old_txtcol = txtcol;

	ਜਦੋਂ (j > -1)
	{
	    txtcol += j;
	    ਜੇ (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    ਨਹੀਂਤਾਂ
		txtcol -= wp->w_leftcol;
	    ਜੇ (txtcol >= 0 && txtcol < wp->w_ਚੌੜਾਈ)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    ਜੇ (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	ਜੇ (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	ਨਹੀਂਤਾਂ
	    txtcol -= wp->w_leftcol;
	ਜੇ (txtcol >= 0 && txtcol < wp->w_ਚੌੜਾਈ)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_ਸਤਰ(wp, ਪੰਕਤੀ + W_WINROW(wp), wp->w_wincol, wp->w_ਚੌੜਾਈ, wp->w_ਚੌੜਾਈ,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    ਜੇ (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_ਪੰਕਤੀ = ਪੰਕਤੀ;
	curwin->w_cline_ਉਚਾਈ = 1;
	curwin->w_cline_folded = true;
	curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
    }

# ifdef FEAT_CONCEAL
    // When the line was not folded w_wrow may have been set, recompute it.
    ਜੇ (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_ਸਤਰ(wp))
	curs_columns(TRUE);
# endif
}
#ਜੇ_ਅੰਤ

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
    ਸਥਿਰ ਖਾਲੀ
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_ਬਫਰ;
    ਪੂਰਨ		type;
    ਪੂਰਨ		ਸਿਖਰ_ਸਿਰਾ = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    ਪੂਰਨ		mid_ਸ਼ੁਰੂ = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    ਪੂਰਨ		mid_ਸਿਰਾ = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    ਪੂਰਨ		bot_ਸ਼ੁਰੂ = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    ਪੂਰਨ		scrolled_ਥੱਲੇ = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    ਪੂਰਨ		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //ਸਤਰਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਪੂਰਨ		ਸਿਖਰ_ਮੰਜ਼ਿਲ_mod = FALSE;    // redraw above mod_top
#ਜੇ_ਅੰਤ

    ਪੂਰਨ		ਪੰਕਤੀ;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    ਪੂਰਨ		idx;		// current index in w_lines[]
    ਪੂਰਨ		srow;		// starting row of the current line

    ਪੂਰਨ		eof = FALSE;	// if TRUE, we hit the end of the file
    ਪੂਰਨ		didline = FALSE; // if TRUE, we finished the last line
    ਪੂਰਨ		i;
    ਲੰਮਾ	j;
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		old_wrow = wp->w_wrow;
    ਪੂਰਨ		old_wcol = wp->w_wcol;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਲੰਮਾ	fold_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_ਸਤਰ 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    ਪੂਰਨ		did_update = DID_NONE;
    linenr_T	syntax_ਆਖਰੀ_parsed = 0;		// last parsed text line
#ਜੇ_ਅੰਤ
    linenr_T	mod_ਸਿਖਰ = 0;
    linenr_T	mod_bot = 0;
#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_ਖੋਜ_EXTRA)
    ਪੂਰਨ		save_got_int;
#ਜੇ_ਅੰਤ
    bool	override_success;

#ਜੇ defined(FEAT_ਖੋਜ_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    ਜੇ (!did_update_one_ਖਿੜਕੀ)
    {
	did_update_one_ਖਿੜਕੀ = TRUE;
# ifdef FEAT_ਖੋਜ_EXTRA
	ਸ਼ੁਰੂ_ਖੋਜ_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	ਜੇ (clip_ਤਾਰਾ.ਉਪਲਬਧ && clip_isautosel_ਤਾਰਾ())
	    clip_update_selection(&clip_ਤਾਰਾ);
	ਜੇ (clip_plus.ਉਪਲਬਧ && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    ਜੇ (wp->w_p_cuc)
	validate_virtcol_win(wp);
#ਜੇ_ਅੰਤ

    type = wp->w_redr_type;

    ਜੇ (type == UPD_NOT_VALID)
    {
	wp->w_redr_ਸਥਿਤੀ = true;
	wp->w_ਸਤਰਾਂ_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    ਜੇ (wp->w_ਉਚਾਈ + WINBAR_ਉਚਾਈ(wp) == 0
	    || (wp->w_ਚੌਖਟਾ->fr_ਉਚਾਈ == wp->w_ਸਥਿਤੀ_ਉਚਾਈ
#ਜੇ defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#ਜੇ_ਅੰਤ
	       ))
    {
	wp->w_redr_type = 0;
	ਵਾਪਸ;
    }

    // Window is zero-width: Only need to draw the separator.
    ਜੇ (wp->w_ਚੌੜਾਈ == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	ਵਾਪਸ;
    }

    override_success = ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    ਜੇ (ਪਦ_do_update_ਖਿੜਕੀ(wp))
    {
	ਪਦ_update_ਖਿੜਕੀ(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	ਜੇ (winbar_ਉਚਾਈ(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	ਜੇ (override_success)
	    ਕੱਢੋ_highlight_overrides();
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    init_ਖੋਜ_hl(wp, &screen_ਖੋਜ_hl);
#ਜੇ_ਅੰਤ

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    ਜੇ (wp->w_skipcol > 0 && wp->w_ਚੌੜਾਈ > win_ਖਾਨਾ_off(wp))
    {
	ਪੂਰਨ w = 0;
	ਪੂਰਨ width1 = wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp);
	ਪੂਰਨ width2 = width1 + win_ਖਾਨਾ_off2(wp);
	ਪੂਰਨ ਜੋੜੋ = width1;

	ਜਦੋਂ (w < wp->w_skipcol)
	{
	    ਜੇ (w > 0)
		ਜੋੜੋ = width2;
	    w += ਜੋੜੋ;
	}
	ਜੇ (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - ਜੋੜੋ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? ਸੰਖਿਆ_ਚੌੜਾਈ(wp) : 0;
    ਜੇ (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_ਸਿਖਰ = wp->w_redraw_ਸਿਖਰ;
	ਜੇ (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	ਨਹੀਂਤਾਂ
	    mod_bot = 0;
	ਜੇ (buf->b_mod_ਸੈੱਟ)
	{
	    ਜੇ (mod_ਸਿਖਰ == 0 || mod_ਸਿਖਰ > buf->b_mod_ਸਿਖਰ)
	    {
		mod_ਸਿਖਰ = buf->b_mod_ਸਿਖਰ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		ਜੇ (syntax_present(wp))
		{
		    mod_ਸਿਖਰ -= buf->b_s.b_syn_sync_linebreaks;
		    ਜੇ (mod_ਸਿਖਰ < 1)
			mod_ਸਿਖਰ = 1;
		}
#ਜੇ_ਅੰਤ
	    }
	    ਜੇ (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    ਜੇ (screen_ਖੋਜ_hl.rm.regprog != ਨਿਰਾ
		    && re_multiline(screen_ਖੋਜ_hl.rm.regprog))
		ਸਿਖਰ_ਮੰਜ਼ਿਲ_mod = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		matchitem_T *cur = wp->w_ਮੇਲ_ਸਿਰ;

		ਜਦੋਂ (cur != ਨਿਰਾ)
		{
		    ਜੇ (cur->mit_ਮੇਲ.regprog != ਨਿਰਾ
				       && re_multiline(cur->mit_ਮੇਲ.regprog))
		    {
			ਸਿਖਰ_ਮੰਜ਼ਿਲ_mod = TRUE;
			ਰੋਕੋ;
		    }
		    cur = cur->mit_ਅਗਲਾ;
		}
	    }
#ਜੇ_ਅੰਤ
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	ਜੇ (ਖੋਜ_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    ਜੇ (mod_ਸਿਖਰ == 0 || mod_ਸਿਖਰ > ਖੋਜ_hl_has_cursor_lnum)
		mod_ਸਿਖਰ = ਖੋਜ_hl_has_cursor_lnum;
	    ਜੇ (mod_bot == 0 || mod_bot < ਖੋਜ_hl_has_cursor_lnum + 1)
		mod_bot = ਖੋਜ_hl_has_cursor_lnum + 1;
	}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਜੇ (mod_ਸਿਖਰ != 0 && hasAnyFolding(wp))
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
	    ਲਈ (i = 0; i < wp->w_ਸਤਰਾਂ_valid; ++i)
		ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_valid)
		{
		    ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lastlnum < mod_ਸਿਖਰ)
			lnumt = wp->w_ਸਤਰਾਂ[i].wl_lastlnum + 1;
		    ਜੇ (lnumb == MAXLNUM && wp->w_ਸਤਰਾਂ[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_ਸਤਰਾਂ[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			ਜੇ (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (ਖਾਲੀ)hasFoldingWin(wp, mod_ਸਿਖਰ, &mod_ਸਿਖਰ, ਨਿਰਾ, TRUE, ਨਿਰਾ);
	    ਜੇ (mod_ਸਿਖਰ > lnumt)
		mod_ਸਿਖਰ = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (ਖਾਲੀ)hasFoldingWin(wp, mod_bot, ਨਿਰਾ, &mod_bot, TRUE, ਨਿਰਾ);
	    ++mod_bot;
	    ਜੇ (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#ਜੇ_ਅੰਤ

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	ਜੇ (mod_ਸਿਖਰ != 0 && mod_ਸਿਖਰ < wp->w_topline)
	{
	    ਜੇ (mod_bot > wp->w_topline)
		mod_ਸਿਖਰ = wp->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਨਹੀਂਤਾਂ ਜੇ (syntax_present(wp))
		ਸਿਖਰ_ਸਿਰਾ = 1;
#ਜੇ_ਅੰਤ
	}
    }
    wp->w_redraw_ਸਿਖਰ = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਖੋਜ_hl_has_cursor_lnum = 0;
#ਜੇ_ਅੰਤ


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    ਜੇ (type == UPD_REDRAW_ਸਿਖਰ)
    {
	j = 0;
	ਲਈ (i = 0; i < wp->w_ਸਤਰਾਂ_valid; ++i)
	{
	    j += wp->w_ਸਤਰਾਂ[i].wl_ਮਾਪ;
	    ਜੇ (j >= wp->w_upd_ਪੰਕਤੀਆਂ)
	    {
		ਸਿਖਰ_ਸਿਰਾ = j;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (ਸਿਖਰ_ਸਿਰਾ == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	ਨਹੀਂਤਾਂ
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    ਜੇ (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    ਜੇ ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#ਜੇ_ਅੰਤ
	    )
    {
	ਜੇ (mod_ਸਿਖਰ != 0
		&& wp->w_topline == mod_ਸਿਖਰ
		&& (!wp->w_ਸਤਰਾਂ[0].wl_valid
		    || wp->w_topline == wp->w_ਸਤਰਾਂ[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	ਨਹੀਂਤਾਂ ਜੇ (wp->w_ਸਤਰਾਂ[0].wl_valid
		&& (wp->w_topline < wp->w_ਸਤਰਾਂ[0].wl_lnum
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    || (wp->w_topline == wp->w_ਸਤਰਾਂ[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#ਜੇ_ਅੰਤ
		   ))
	{
	    // New topline is above old topline: May scroll down.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		ਲਈ (ln = wp->w_topline; ln < wp->w_ਸਤਰਾਂ[0].wl_lnum; ++ln)
		{
		    ++j;
		    ਜੇ (j >= wp->w_ਉਚਾਈ - 2)
			ਰੋਕੋ;
		    (ਖਾਲੀ)hasFoldingWin(wp, ln, ਨਿਰਾ, &ln, TRUE, ਨਿਰਾ);
		}
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		j = wp->w_ਸਤਰਾਂ[0].wl_lnum - wp->w_topline;
	    ਜੇ (j < wp->w_ਉਚਾਈ - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_ਸਤਰਾਂ[0].wl_lnum - 1,
								wp->w_ਉਚਾਈ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		ਜੇ (wp->w_ਸਤਰਾਂ[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_ਸਤਰਾਂ[0].wl_lnum)
							  - wp->w_old_topfill;
#ਜੇ_ਅੰਤ
		ਜੇ (i < wp->w_ਉਚਾਈ - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    ਜੇ (i > 0)
			check_for_delay(FALSE);
		    ਜੇ (win_ins_ਸਤਰਾਂ(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			ਜੇ (wp->w_ਸਤਰਾਂ_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    ਸਿਖਰ_ਸਿਰਾ = i;
			    scrolled_ਥੱਲੇ = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    ਜੇ ((wp->w_ਸਤਰਾਂ_valid += j) > wp->w_ਉਚਾਈ)
				wp->w_ਸਤਰਾਂ_valid = wp->w_ਉਚਾਈ;
			    ਲਈ (idx = wp->w_ਸਤਰਾਂ_valid; idx - j >= 0; idx--)
				wp->w_ਸਤਰਾਂ[idx] = wp->w_ਸਤਰਾਂ[idx - j];
			    ਜਦੋਂ (idx >= 0)
				wp->w_ਸਤਰਾਂ[idx--].wl_valid = FALSE;
			}
		    }
		    ਨਹੀਂਤਾਂ
			mid_ਸ਼ੁਰੂ = 0;		// redraw all lines
		}
		ਨਹੀਂਤਾਂ
		    mid_ਸ਼ੁਰੂ = 0;		// redraw all lines
	    }
	    ਨਹੀਂਤਾਂ
		mid_ਸ਼ੁਰੂ = 0;		// redraw all lines
	}
	ਨਹੀਂਤਾਂ
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    ਪੰਕਤੀ = 0;
	    ਲਈ (i = 0; i < wp->w_ਸਤਰਾਂ_valid && i < ਪੰਕਤੀਆਂ; i++)
	    {
		ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_valid
			&& wp->w_ਸਤਰਾਂ[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    ਰੋਕੋ;
		}
		ਪੰਕਤੀ += wp->w_ਸਤਰਾਂ[i].wl_ਮਾਪ;
	    }
	    ਜੇ (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//ਸਤਰਾਂ
		mid_ਸ਼ੁਰੂ = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		ਜੇ (wp->w_ਸਤਰਾਂ[0].wl_lnum == wp->w_topline)
		    ਪੰਕਤੀ += wp->w_old_topfill;
		ਨਹੀਂਤਾਂ
		    ਪੰਕਤੀ += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		ਪੰਕਤੀ -= wp->w_topfill;
#ਜੇ_ਅੰਤ
		ਜੇ (ਪੰਕਤੀ > ਪੰਕਤੀਆਂ)  // just in case
		    ਪੰਕਤੀ = ਪੰਕਤੀਆਂ;
		ਜੇ (ਪੰਕਤੀ > 0)
		{
		    check_for_delay(FALSE);
		    ਜੇ (win_del_ਸਤਰਾਂ(wp, 0, ਪੰਕਤੀ, FALSE, wp == firstwin, 0)
									 == OK)
			bot_ਸ਼ੁਰੂ = wp->w_ਉਚਾਈ - ਪੰਕਤੀ;
		    ਨਹੀਂਤਾਂ
			mid_ਸ਼ੁਰੂ = 0;		// redraw all lines
		}
		ਜੇ ((ਪੰਕਤੀ == 0 || bot_ਸ਼ੁਰੂ < 999) && wp->w_ਸਤਰਾਂ_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_ਸ਼ੁਰੂ = 0;
		    idx = 0;
		    ਲਈ (;;)
		    {
			wp->w_ਸਤਰਾਂ[idx] = wp->w_ਸਤਰਾਂ[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			ਜੇ (ਪੰਕਤੀ > 0 && bot_ਸ਼ੁਰੂ + ਪੰਕਤੀ
				 + (ਪੂਰਨ)wp->w_ਸਤਰਾਂ[j].wl_ਮਾਪ > wp->w_ਉਚਾਈ)
			{
			    wp->w_ਸਤਰਾਂ_valid = idx + 1;
			    ਰੋਕੋ;
			}
			bot_ਸ਼ੁਰੂ += wp->w_ਸਤਰਾਂ[idx++].wl_ਮਾਪ;

			// stop at the last valid entry in w_lines[].wl_size
			ਜੇ (++j >= wp->w_ਸਤਰਾਂ_valid)
			{
			    wp->w_ਸਤਰਾਂ_valid = idx;
			    ਰੋਕੋ;
			}
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    ਜੇ (wp->w_p_diff && bot_ਸ਼ੁਰੂ > 0)
			wp->w_ਸਤਰਾਂ[0].wl_ਮਾਪ = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#ਜੇ_ਅੰਤ
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	ਜੇ (mid_ਸ਼ੁਰੂ == 0)
	    mid_ਸਿਰਾ = wp->w_ਉਚਾਈ;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	ਜੇ (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_ਸ਼ੁਰੂ = 0;
	mid_ਸਿਰਾ = wp->w_ਉਚਾਈ;
    }

    ਜੇ (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_ਸ਼ੁਰੂ = 0;
	mid_ਸਿਰਾ = wp->w_ਉਚਾਈ;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    ਜੇ ((VIsual_ਸਰਗਰਮ && buf == curwin->w_ਬਫਰ)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, ਮੰਜ਼ਿਲ;

	ਜੇ (VIsual_ਸਰਗਰਮ)
	{
	    ਜੇ (VIsual_ਬਹੁਲਕ != wp->w_old_visual_ਬਹੁਲਕ
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		ਜੇ (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    ਮੰਜ਼ਿਲ = VIsual.lnum;
		}
		ਨਹੀਂਤਾਂ
		{
		    from = VIsual.lnum;
		    ਮੰਜ਼ਿਲ = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		ਜੇ (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		ਜੇ (wp->w_old_cursor_lnum > ਮੰਜ਼ਿਲ)
		    ਮੰਜ਼ਿਲ = wp->w_old_cursor_lnum;
		ਜੇ (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		ਜੇ (wp->w_old_visual_lnum > ਮੰਜ਼ਿਲ)
		    ਮੰਜ਼ਿਲ = wp->w_old_visual_lnum;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		ਜੇ (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    ਮੰਜ਼ਿਲ = wp->w_old_cursor_lnum;
		}
		ਨਹੀਂਤਾਂ
		{
		    from = wp->w_old_cursor_lnum;
		    ਮੰਜ਼ਿਲ = curwin->w_cursor.lnum;
		    ਜੇ (from == 0)	// Visual mode just started
			from = ਮੰਜ਼ਿਲ;
		}

		ਜੇ (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.ਖਾਨਾ != wp->w_old_visual_ਖਾਨਾ)
		{
		    ਜੇ (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    ਜੇ (wp->w_old_visual_lnum > ਮੰਜ਼ਿਲ)
			ਮੰਜ਼ਿਲ = wp->w_old_visual_lnum;
		    ਜੇ (VIsual.lnum < from)
			from = VIsual.lnum;
		    ਜੇ (VIsual.lnum > ਮੰਜ਼ਿਲ)
			ਮੰਜ਼ਿਲ = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    ਜੇ (VIsual_ਬਹੁਲਕ == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_ਸਿਰਾ_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		ਜੇ (curwin->w_curswant == MAXCOL)
		{
		    ਜੇ (get_ve_flags() & VE_ਖੰਡ)
		    {
			pos_T	    pos;
			ਪੂਰਨ	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			ਲਈ (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.ਖਾਨਾ = (ਪੂਰਨ)ml_get_buf_len(wp->w_ਬਫਰ, pos.lnum);
			    getvvcol(wp, &pos, ਨਿਰਾ, ਨਿਰਾ, &t, 0);
			    ਜੇ (toc < t)
				toc = t;
			}
			++toc;
		    }
		    ਨਹੀਂਤਾਂ
			toc = MAXCOL;
		}

		ਜੇ (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    ਜੇ (from > VIsual.lnum)
			from = VIsual.lnum;
		    ਜੇ (ਮੰਜ਼ਿਲ < VIsual.lnum)
			ਮੰਜ਼ਿਲ = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // Use the line numbers of the old Visual area.
	    ਜੇ (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		ਮੰਜ਼ਿਲ = wp->w_old_visual_lnum;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		from = wp->w_old_visual_lnum;
		ਮੰਜ਼ਿਲ = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	ਜੇ (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	ਜੇ (wp->w_valid & VALID_BOTLINE)
	{
	    from = MIN(from, wp->w_botline);
	    ਮੰਜ਼ਿਲ = MIN(ਮੰਜ਼ਿਲ, wp->w_botline);
	}

	// Find the minimal part to be updated.
	// Watch out for scrolling that made entries in w_lines[] invalid.
	// E.g., CTRL-U makes the first half of w_lines[] invalid and sets
	// top_end; need to redraw from top_end to the "to" line.
	// A middle mouse click with a Visual selection may change the text
	// above the Visual area and reset wl_valid, do count these for
	// mid_end (in srow).
	ਜੇ (mid_ਸ਼ੁਰੂ > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    ਜੇ (scrolled_ਥੱਲੇ)
		mid_ਸ਼ੁਰੂ = ਸਿਖਰ_ਸਿਰਾ;
	    ਨਹੀਂਤਾਂ
		mid_ਸ਼ੁਰੂ = 0;
	    ਜਦੋਂ (lnum < from && idx < wp->w_ਸਤਰਾਂ_valid)	// find start
	    {
		ਜੇ (wp->w_ਸਤਰਾਂ[idx].wl_valid)
		    mid_ਸ਼ੁਰੂ += wp->w_ਸਤਰਾਂ[idx].wl_ਮਾਪ;
		ਨਹੀਂਤਾਂ ਜੇ (!scrolled_ਥੱਲੇ)
		    srow += wp->w_ਸਤਰਾਂ[idx].wl_ਮਾਪ;
		++idx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		ਜੇ (idx < wp->w_ਸਤਰਾਂ_valid && wp->w_ਸਤਰਾਂ[idx].wl_valid)
		    lnum = wp->w_ਸਤਰਾਂ[idx].wl_lnum;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    ++lnum;
	    }
	    srow += mid_ਸ਼ੁਰੂ;
	    mid_ਸਿਰਾ = wp->w_ਉਚਾਈ;
	    ਲਈ ( ; idx < wp->w_ਸਤਰਾਂ_valid; ++idx)		// find end
	    {
		ਜੇ (wp->w_ਸਤਰਾਂ[idx].wl_valid
			&& wp->w_ਸਤਰਾਂ[idx].wl_lnum >= ਮੰਜ਼ਿਲ + 1)
		{
		    // Only update until first row of this line
		    mid_ਸਿਰਾ = srow;
		    ਰੋਕੋ;
		}
		srow += wp->w_ਸਤਰਾਂ[idx].wl_ਮਾਪ;
	    }
	}
    }

    ਜੇ (VIsual_ਸਰਗਰਮ && buf == curwin->w_ਬਫਰ)
    {
	wp->w_old_visual_ਬਹੁਲਕ = VIsual_ਬਹੁਲਕ;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_ਖਾਨਾ = VIsual.ਖਾਨਾ;
	wp->w_old_curswant = curwin->w_curswant;
    }
    ਨਹੀਂਤਾਂ
    {
	wp->w_old_visual_ਬਹੁਲਕ = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_ਖਾਨਾ = 0;
    }

#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_ਖੋਜ_EXTRA)
    // reset got_int, otherwise regexp won't work
    save_got_int = got_int;
    got_int = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SYN_ਸਮਾਂ_ਹੱਦਬੰਦੀ
    // Set the time limit to 'redrawtime'.
    redrawtime_ਹੱਦਬੰਦੀ_ਸੈੱਟ = TRUE;
    init_regexp_timeout(p_rdt);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    win_foldinfo.fi_level = 0;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    // Draw the window toolbar, if there is one.
    // TODO: only when needed.
    ਜੇ (winbar_ਉਚਾਈ(wp) > 0)
	redraw_win_toolbar(wp);
#ਜੇ_ਅੰਤ

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_ਖੇਤਰ(spv);
    ਜੇ (spell_check_ਖਿੜਕੀ(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_ਸਿਖਰ == 0;
    }
#ਜੇ_ਅੰਤ

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    ਪੰਕਤੀ = 0;
    srow = 0;
    ਲਈ (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	ਜੇ (ਪੰਕਤੀ == wp->w_ਉਚਾਈ)
	{
	    didline = TRUE;
	    ਰੋਕੋ;
	}

	// stop updating when hit the end of the file
	ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    eof = TRUE;
	    ਰੋਕੋ;
	}

	// Remember the starting row of the line that is going to be dealt
	// with.  It is used further down when the line doesn't fit.
	srow = ਪੰਕਤੀ;

	// Update a line when it is in an area that needs updating, when it
	// has changes or w_lines[idx] is invalid.
	// "bot_start" may be halfway a wrapped line after using
	// win_del_lines(), check if the current line includes it.
	// When syntax folding is being used, the saved syntax states will
	// already have been updated, we can't see where the syntax state is
	// the same again, just update until the end of the window.
	ਜੇ (ਪੰਕਤੀ < ਸਿਖਰ_ਸਿਰਾ
		|| (ਪੰਕਤੀ >= mid_ਸ਼ੁਰੂ && ਪੰਕਤੀ < mid_ਸਿਰਾ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		|| ਸਿਖਰ_ਮੰਜ਼ਿਲ_mod
#ਜੇ_ਅੰਤ
		|| idx >= wp->w_ਸਤਰਾਂ_valid
		|| (ਪੰਕਤੀ + wp->w_ਸਤਰਾਂ[idx].wl_ਮਾਪ > bot_ਸ਼ੁਰੂ)
		|| (mod_ਸਿਖਰ != 0
		    && (lnum == mod_ਸਿਖਰ
			|| (lnum >= mod_ਸਿਖਰ
			    && (lnum < mod_bot
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_ਸਤਰ
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_ਬਦਲਿਆ(lnum)))
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_ਮੇਲ_ਸਿਰ != ਨਿਰਾ
						    && buf->b_mod_ਸੈੱਟ
						    && buf->b_mod_xlines != 0)
#ਜੇ_ਅੰਤ
				))))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		|| (wp->w_p_cul && lnum == wp->w_cursor.lnum)
		|| lnum == wp->w_ਆਖਰੀ_cursorline
#ਜੇ_ਅੰਤ
				)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	    ਜੇ (lnum == mod_ਸਿਖਰ)
		ਸਿਖਰ_ਮੰਜ਼ਿਲ_mod = FALSE;
#ਜੇ_ਅੰਤ

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    ਜੇ (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_ਸਿਖਰ && lnum < MAX(mod_bot, mod_ਸਿਖਰ + 1)
		    && (!scrolled_ਥੱਲੇ || ਪੰਕਤੀ >= ਸਿਖਰ_ਸਿਰਾ))
	    {
		scrolled_for_mod = TRUE;

		ਪੂਰਨ		old_cline_ਉਚਾਈ = 0;
		ਪੂਰਨ		old_ਪੰਕਤੀਆਂ = 0;
		ਪੂਰਨ		new_ਪੰਕਤੀਆਂ = 0;
		ਪੂਰਨ		xtra_ਪੰਕਤੀਆਂ;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		ਲਈ (i = idx; i < wp->w_ਸਤਰਾਂ_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_valid
			    && wp->w_ਸਤਰਾਂ[i].wl_lnum == mod_bot)
			ਰੋਕੋ;
		    ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_ਉਚਾਈ = wp->w_ਸਤਰਾਂ[i].wl_ਮਾਪ;
		    old_ਪੰਕਤੀਆਂ += wp->w_ਸਤਰਾਂ[i].wl_ਮਾਪ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		    ਜੇ (wp->w_ਸਤਰਾਂ[i].wl_valid
			    && wp->w_ਸਤਰਾਂ[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			ਜਦੋਂ (i < wp->w_ਸਤਰਾਂ_valid
						  && !wp->w_ਸਤਰਾਂ[i].wl_valid)
			    old_ਪੰਕਤੀਆਂ += wp->w_ਸਤਰਾਂ[i++].wl_ਮਾਪ;
			ਰੋਕੋ;
		    }
#ਜੇ_ਅੰਤ
		}

		ਜੇ (i >= wp->w_ਸਤਰਾਂ_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_ਸ਼ੁਰੂ = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    ਲਈ (l = lnum; l < mod_bot; ++l)
		    {
			ਜੇ (dollar_vcol >= 0 && wp == curwin &&
				old_cline_ਉਚਾਈ > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_ਪੰਕਤੀਆਂ += old_cline_ਉਚਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			ਨਹੀਂਤਾਂ ਜੇ (hasFoldingWin(wp, l, ਨਿਰਾ, &l, TRUE, ਨਿਰਾ))
			    ++new_ਪੰਕਤੀਆਂ;
#ਜੇ_ਅੰਤ
			ਨਹੀਂਤਾਂ
			    new_ਪੰਕਤੀਆਂ += plines_correct_topline(wp, l, TRUE);
			++j;
			ਜੇ (new_ਪੰਕਤੀਆਂ > wp->w_ਉਚਾਈ - ਪੰਕਤੀ - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_ਪੰਕਤੀਆਂ = 9999;
			    ਰੋਕੋ;
			}
		    }
		    xtra_ਪੰਕਤੀਆਂ = new_ਪੰਕਤੀਆਂ - old_ਪੰਕਤੀਆਂ;
		    ਜੇ (xtra_ਪੰਕਤੀਆਂ < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			ਜੇ (ਪੰਕਤੀ - xtra_ਪੰਕਤੀਆਂ >= wp->w_ਉਚਾਈ - 2)
			    mod_bot = MAXLNUM;
			ਨਹੀਂਤਾਂ
			{
			    check_for_delay(FALSE);
			    ਜੇ (win_del_ਸਤਰਾਂ(wp, ਪੰਕਤੀ,
					  -xtra_ਪੰਕਤੀਆਂ, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    ਨਹੀਂਤਾਂ
				bot_ਸ਼ੁਰੂ = wp->w_ਉਚਾਈ + xtra_ਪੰਕਤੀਆਂ;
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (xtra_ਪੰਕਤੀਆਂ > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			ਜੇ (ਪੰਕਤੀ + xtra_ਪੰਕਤੀਆਂ >= wp->w_ਉਚਾਈ - 2)
			    mod_bot = MAXLNUM;
			ਨਹੀਂਤਾਂ
			{
			    check_for_delay(FALSE);
			    ਜੇ (win_ins_ਸਤਰਾਂ(wp, ਪੰਕਤੀ + old_ਪੰਕਤੀਆਂ,
					     xtra_ਪੰਕਤੀਆਂ, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    ਨਹੀਂਤਾਂ ਜੇ (ਸਿਖਰ_ਸਿਰਾ > ਪੰਕਤੀ + old_ਪੰਕਤੀਆਂ)
				// Scrolled the part at the top that requires
				// updating down.
				ਸਿਖਰ_ਸਿਰਾ += xtra_ਪੰਕਤੀਆਂ;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    ਜੇ (mod_bot != MAXLNUM && i != j)
		    {
			ਜੇ (j < i)
			{
			    ਪੂਰਨ x = ਪੰਕਤੀ + new_ਪੰਕਤੀਆਂ;

			    // move entries in w_lines[] upwards
			    ਲਈ (;;)
			    {
				// stop at last valid entry in w_lines[]
				ਜੇ (i >= wp->w_ਸਤਰਾਂ_valid)
				{
				    wp->w_ਸਤਰਾਂ_valid = j;
				    ਰੋਕੋ;
				}
				wp->w_ਸਤਰਾਂ[j] = wp->w_ਸਤਰਾਂ[i];
				// stop at a line that won't fit
				ਜੇ (x + (ਪੂਰਨ)wp->w_ਸਤਰਾਂ[j].wl_ਮਾਪ
							   > wp->w_ਉਚਾਈ)
				{
				    wp->w_ਸਤਰਾਂ_valid = j + 1;
				    ਰੋਕੋ;
				}
				x += wp->w_ਸਤਰਾਂ[j++].wl_ਮਾਪ;
				++i;
			    }
			    ਜੇ (bot_ਸ਼ੁਰੂ > x)
				bot_ਸ਼ੁਰੂ = x;
			}
			ਨਹੀਂਤਾਂ // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_ਸਤਰਾਂ_valid += j;
			    ਜੇ (wp->w_ਸਤਰਾਂ_valid > wp->w_ਉਚਾਈ)
				wp->w_ਸਤਰਾਂ_valid = wp->w_ਉਚਾਈ;
			    ਲਈ (i = wp->w_ਸਤਰਾਂ_valid; i - j >= idx; --i)
				wp->w_ਸਤਰਾਂ[i] = wp->w_ਸਤਰਾਂ[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    ਜਦੋਂ (i >= idx)
			    {
				wp->w_ਸਤਰਾਂ[i].wl_ਮਾਪ = 0;
				wp->w_ਸਤਰਾਂ[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_ਗਿਣਤੀ = folded_ਗਿਣਤੀ(wp, lnum, &win_foldinfo);
	    ਜੇ (fold_ਗਿਣਤੀ != 0)
	    {
		fold_ਸਤਰ(wp, fold_ਗਿਣਤੀ, &win_foldinfo, lnum, ਪੰਕਤੀ);
		++ਪੰਕਤੀ;
		--fold_ਗਿਣਤੀ;
		wp->w_ਸਤਰਾਂ[idx].wl_folded = TRUE;
		wp->w_ਸਤਰਾਂ[idx].wl_lastlnum = lnum + fold_ਗਿਣਤੀ;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
# ifdef FEAT_SPELL
		spv.spv_capcol_lnum = 0;
# endif
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜੇ (idx < wp->w_ਸਤਰਾਂ_valid
		    && wp->w_ਸਤਰਾਂ[idx].wl_valid
		    && wp->w_ਸਤਰਾਂ[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_ਸਤਰਾਂ[idx].wl_ਮਾਪ > wp->w_ਉਚਾਈ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#ਜੇ_ਅੰਤ
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		ਪੰਕਤੀ = wp->w_ਉਚਾਈ + 1;
	    }
	    ਨਹੀਂਤਾਂ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
		prepare_ਖੋਜ_hl(wp, &screen_ਖੋਜ_hl, lnum);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		ਜੇ (syntax_ਆਖਰੀ_parsed != 0 && syntax_ਆਖਰੀ_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_ਸਿਰਾ_parsing(wp, syntax_ਆਖਰੀ_parsed + 1);
#ਜੇ_ਅੰਤ

		// Display one line.
		ਪੰਕਤੀ = win_ਸਤਰ(wp, lnum, srow, wp->w_ਉਚਾਈ, 0, &spv);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		wp->w_ਸਤਰਾਂ[idx].wl_folded = FALSE;
		wp->w_ਸਤਰਾਂ[idx].wl_lastlnum = lnum;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		did_update = DID_ਸਤਰ;
		syntax_ਆਖਰੀ_parsed = lnum;
#ਜੇ_ਅੰਤ
	    }

	    wp->w_ਸਤਰਾਂ[idx].wl_lnum = lnum;
	    wp->w_ਸਤਰਾਂ[idx].wl_valid = TRUE;

	    ਪੂਰਨ is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    ਜੇ (ਪੰਕਤੀ > wp->w_ਉਚਾਈ || ਪੰਕਤੀ + wp->w_winrow >= ਪੰਕਤੀਆਂ)
	    {
		// we may need the size of that too long line later on
		ਜੇ (dollar_vcol == -1 || !is_curline)
		    wp->w_ਸਤਰਾਂ[idx].wl_ਮਾਪ = plines_win(wp, lnum, TRUE);
		++idx;
		ਰੋਕੋ;
	    }
	    ਜੇ (dollar_vcol == -1 || !is_curline)
		wp->w_ਸਤਰਾਂ[idx].wl_ਮਾਪ = ਪੰਕਤੀ - srow;
	    ++idx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    lnum += fold_ਗਿਣਤੀ + 1;
#ਨਹੀਂਤਾਂ
	    ++lnum;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    ਜੇ ((wp->w_p_nu && mod_ਸਿਖਰ != 0 && lnum >= mod_bot
			&& buf->b_mod_ਸੈੱਟ && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_ਆਖਰੀ_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		fold_ਗਿਣਤੀ = folded_ਗਿਣਤੀ(wp, lnum, &win_foldinfo);
		ਜੇ (fold_ਗਿਣਤੀ != 0)
		    fold_ਸਤਰ(wp, fold_ਗਿਣਤੀ, &win_foldinfo, lnum, ਪੰਕਤੀ);
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    (ਖਾਲੀ)win_ਸਤਰ(wp, lnum, srow, wp->w_ਉਚਾਈ,
					       wp->w_ਸਤਰਾਂ[idx].wl_ਮਾਪ, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    ਪੰਕਤੀ += wp->w_ਸਤਰਾਂ[idx++].wl_ਮਾਪ;
	    ਜੇ (ਪੰਕਤੀ > wp->w_ਉਚਾਈ)	// past end of screen
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    lnum = wp->w_ਸਤਰਾਂ[idx - 1].wl_lastlnum + 1;
#ਨਹੀਂਤਾਂ
	    ++lnum;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    did_update = DID_NONE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#ਜੇ_ਅੰਤ
	}

	ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    eof = TRUE;
	    ਰੋਕੋ;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	ਜੇ (idx >= ਪੰਕਤੀਆਂ)
	    ਰੋਕੋ;
    }

    // End of loop over all window lines.

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Now that the window has been redrawn with the old and new cursor line,
    // update w_last_cursorline.
    wp->w_ਆਖਰੀ_cursorline = wp->w_p_cul ? wp->w_cursor.lnum : 0;
#ਜੇ_ਅੰਤ
    wp->w_ਆਖਰੀ_cursor_lnum_rnu = wp->w_p_rnu ? wp->w_cursor.lnum : 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VTP
    // Rewrite the character at the end of the screen line.
    // See the version that was fixed.
    ਜੇ (use_vtp() && get_conpty_fix_type() < 1)
    {
	ਪੂਰਨ k;

	ਲਈ (k = 0; k < ਪੰਕਤੀਆਂ; ++k)
	    ਜੇ (enc_utf8)
		ਜੇ ((*mb_off2cells)(ਸਤਰ_ਵਿੱਥ[k] + topframe->fr_ਚੌੜਾਈ - 2,
					   ਸਤਰ_ਵਿੱਥ[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_ਚੌੜਾਈ - 2, 1, 2,
			    FALSE, FALSE);
		ਨਹੀਂਤਾਂ
		    screen_draw_rectangle(k, topframe->fr_ਚੌੜਾਈ - 1, 1, 1,
			    FALSE, FALSE);
	    ਨਹੀਂਤਾਂ
		screen_char(ਸਤਰ_ਵਿੱਥ[k] + topframe->fr_ਚੌੜਾਈ - 1, k,
			cmdline_ਚੌੜਾਈ - 1);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (idx > wp->w_ਸਤਰਾਂ_valid)
	wp->w_ਸਤਰਾਂ_valid = idx;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    ਜੇ (syntax_ਆਖਰੀ_parsed != 0 && syntax_present(wp))
	syntax_ਸਿਰਾ_parsing(wp, syntax_ਆਖਰੀ_parsed + 1);
#ਜੇ_ਅੰਤ

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    wp->w_filler_ਪੰਕਤੀਆਂ = 0;
#ਜੇ_ਅੰਤ
    ਜੇ (!eof && !didline)
    {
	ਜੇ (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	ਨਹੀਂਤਾਂ ਜੇ (diff_check_fill(wp, lnum) >= wp->w_ਉਚਾਈ - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_ਪੰਕਤੀਆਂ = wp->w_ਉਚਾਈ - srow;
	}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਨਹੀਂਤਾਂ ਜੇ (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    ਪੂਰਨ		scr_ਪੰਕਤੀ = W_WINROW(wp) + wp->w_ਉਚਾਈ - 1;
	    ਪੂਰਨ		ਚਿੰਨ੍ਹ  = wp->w_fill_chars.lastline;
	    ਪੂਰਨ		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(ਚਿੰਨ੍ਹ, &fillbuf[0]);
	    mb_char2bytes(ਚਿੰਨ੍ਹ, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_ਚੌੜਾਈ > 2 ? 2 : wp->w_ਚੌੜਾਈ) * charlen,
			    scr_ਪੰਕਤੀ, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_ਪੰਕਤੀ, scr_ਪੰਕਤੀ + 1,
		    (ਪੂਰਨ)wp->w_wincol + 2, (ਪੂਰਨ)W_ENDCOL(wp),
		    ਚਿੰਨ੍ਹ, ' ', HL_ATTR(HLF_AT));
	    ਸੈੱਟ_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ(wp, srow);
	    wp->w_botline = lnum;
	}
	ਨਹੀਂਤਾਂ ਜੇ (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    ਪੂਰਨ ਸ਼ੁਰੂ_ਖਾਨਾ = (ਪੂਰਨ)W_ENDCOL(wp) - 3;
	    ਪੂਰਨ ਚਿੰਨ੍ਹ    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_ਉਚਾਈ - 1,
		    W_WINROW(wp) + wp->w_ਉਚਾਈ,
		    (ਸ਼ੁਰੂ_ਖਾਨਾ < wp->w_wincol ? wp->w_wincol : ਸ਼ੁਰੂ_ਖਾਨਾ),
		    (ਪੂਰਨ)W_ENDCOL(wp),
		    ਚਿੰਨ੍ਹ, ਚਿੰਨ੍ਹ, HL_ATTR(HLF_AT));
	    ਸੈੱਟ_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ(wp, srow);
	    wp->w_botline = lnum;
	}
	ਨਹੀਂਤਾਂ
	{
	    win_draw_ਸਿਰਾ(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_ਉਚਾਈ, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	draw_vsep_win(wp, ਪੰਕਤੀ);
	ਜੇ (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    ਜੇ (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		ਜੇ (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		ਨਹੀਂਤਾਂ
		    i = wp->w_fill_chars.diff;
		ਜੇ (ਪੰਕਤੀ + j > wp->w_ਉਚਾਈ)
		    j = wp->w_ਉਚਾਈ - ਪੰਕਤੀ;
		win_draw_ਸਿਰਾ(wp, i, i, TRUE, ਪੰਕਤੀ, ਪੰਕਤੀ + (ਪੂਰਨ)j, HLF_DED);
		ਪੰਕਤੀ += j;
	    }
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	ਜੇ (WIN_IS_POPUP(wp))
	    win_draw_ਸਿਰਾ(wp, ' ', ' ', FALSE, ਪੰਕਤੀ, wp->w_ਉਚਾਈ, HLF_AT);
	ਨਹੀਂਤਾਂ
	    win_draw_ਸਿਰਾ(wp, wp->w_fill_chars.eob, ' ', FALSE,
						   ਪੰਕਤੀ, wp->w_ਉਚਾਈ, HLF_EOB);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SYN_ਸਮਾਂ_ਹੱਦਬੰਦੀ
    disable_regexp_timeout();
    redrawtime_ਹੱਦਬੰਦੀ_ਸੈੱਟ = FALSE;
#ਜੇ_ਅੰਤ

    // Reset the type of redrawing required, the window has been updated.
    wp->w_redr_type = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    wp->w_old_topfill = wp->w_topfill;
    wp->w_old_botfill = wp->w_botfill;
#ਜੇ_ਅੰਤ

    ਜੇ (dollar_vcol == -1 || wp != curwin)
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
	ਜੇ (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#ਜੇ defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    ਪੂਰਨ		new_wcol = wp->w_wcol;
#ਜੇ_ਅੰਤ
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#ਜੇ defined(FEAT_CONCEAL)
	    ਜੇ (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
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
		ਜੇ (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		ਨਹੀਂਤਾਂ
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#ਜੇ_ਅੰਤ
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    ਜੇ (wp->w_redr_type != 0)
	    {
		// Don't update for changes in buffer again.
		bool b = curbuf->b_mod_ਸੈੱਟ;
		curbuf->b_mod_ਸੈੱਟ = false;
		j = curbuf->b_mod_xlines;
		curbuf->b_mod_xlines = 0;
		curs_columns(TRUE);
		win_update(curwin);
		curbuf->b_mod_ਸੈੱਟ = b;
		curbuf->b_mod_xlines = j;
	    }
	    // Other windows might have w_redr_type raised in update_topline().
	    must_redraw = 0;
	    FOR_ALL_WINDOWS(wwp)
		ਜੇ (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_ਖੋਜ_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    ਜੇ (!got_int)
	got_int = save_got_int;
#ਜੇ_ਅੰਤ

    ਜੇ (override_success)
	ਕੱਢੋ_highlight_overrides();
}

#ਜੇ defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    ਸਥਿਰ ਖਾਲੀ
update_prepare(ਖਾਲੀ)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_ਖੋਜ_EXTRA
    ਸ਼ੁਰੂ_ਖੋਜ_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_ਨਕਾਬ(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    ਸਥਿਰ ਖਾਲੀ
update_ਅੰਤ(ਖਾਲੀ)
{
    ਜੇ (redraw_cmdline || redraw_ਬਹੁਲਕ)
	showmode();

# ifdef FEAT_ਖੋਜ_EXTRA
    ਸਿਰਾ_ਖੋਜ_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	ਬਾਹਰਲਾ_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_NETBEANS_INTG)
    ਖਾਲੀ
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    ਪੂਰਨ		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_ਸਤਰ_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    ਜੇ (!doit || updating_screen
	    || State == ਬਹੁਲਕ_ASKMORE || State == ਬਹੁਲਕ_HITRETURN
	    || ਸੁਨੇਹਾ_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	ਵਾਪਸ;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (wp->w_redr_type != 0)
	    win_update(wp);
	ਜੇ (wp->w_redr_ਸਥਿਤੀ)
	    win_redr_ਸਥਿਤੀ(wp, FALSE);
    }

# ਜੇ defined(FEAT_TABPANEL)
    ਜੇ (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_ਅੰਤ();
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    ਖਾਲੀ
update_ਖਿੜਕੀ(win_T *wp)
{
    // return if already busy updating
    ਜੇ (updating_screen)
	ਵਾਪਸ;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    ਜੇ (clip_ਤਾਰਾ.ਉਪਲਬਧ && clip_isautosel_ਤਾਰਾ())
	clip_update_selection(&clip_ਤਾਰਾ);
    ਜੇ (clip_plus.ਉਪਲਬਧ && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    ਜੇ (redraw_tabline)
	draw_tabline();

# ਜੇ defined(FEAT_TABPANEL)
    ਜੇ (redraw_tabpanel)
	draw_tabpanel();
# endif

    ਜੇ (wp->w_redr_ਸਥਿਤੀ || p_ru
# ifdef FEAT_STL_OPT
	    || *p_stl != NUL || *wp->w_p_stl != NUL
# endif
	    )
	win_redr_ਸਥਿਤੀ(wp, FALSE);

# ifdef FEAT_PROP_POPUP
    // Display popup windows on top of everything.
    update_popups(win_update);
# endif

    update_ਅੰਤ();
}
#ਜੇ_ਅੰਤ

/*
 * Redraw as soon as possible.  When the command line is not scrolled redraw
 * right away and restore what was on the command line.
 * Return a code indicating what happened.
 */
    ਪੂਰਨ
redraw_asap(ਪੂਰਨ type)
{
    ਪੂਰਨ		ਪੰਕਤੀਆਂ_2;
    ਪੂਰਨ		ਖਾਨੇ_ਗਿਣਤੀ = screen_Columns;
    ਪੂਰਨ		r;
    ਪੂਰਨ		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    ਪੂਰਨ		i;
    u8char_T	*screenlineUC = ਨਿਰਾ;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = ਨਿਰਾ;	// copy from ScreenLines2[]

    redraw_later(type);
    ਜੇ (ਸੁਨੇਹਾ_scrolled
	    || (State != ਬਹੁਲਕ_ਸਾਧਾਰਨ && State != ਬਹੁਲਕ_ਸਾਧਾਰਨ_BUSY)
	    || exiting)
	ਵਾਪਸ ret;

    // Allocate space to save the text displayed in the command line area.
    ਪੰਕਤੀਆਂ_2 = screen_ਪੰਕਤੀਆਂ - cmdline_ਪੰਕਤੀ;
    screenline = LALLOC_MULT(schar_T, ਪੰਕਤੀਆਂ_2 * ਖਾਨੇ_ਗਿਣਤੀ);
    screenattr = LALLOC_MULT(sattr_T, ਪੰਕਤੀਆਂ_2 * ਖਾਨੇ_ਗਿਣਤੀ);
    ਜੇ (screenline == ਨਿਰਾ || screenattr == ਨਿਰਾ)
	ret = 2;
    ਜੇ (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, ਪੰਕਤੀਆਂ_2 * ਖਾਨੇ_ਗਿਣਤੀ);
	ਜੇ (screenlineUC == ਨਿਰਾ)
	    ret = 2;
	ਲਈ (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, ਪੰਕਤੀਆਂ_2 * ਖਾਨੇ_ਗਿਣਤੀ);
	    ਜੇ (screenlineC[i] == ਨਿਰਾ)
		ret = 2;
	}
    }
    ਜੇ (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, ਪੰਕਤੀਆਂ_2 * ਖਾਨੇ_ਗਿਣਤੀ);
	ਜੇ (screenline2 == ਨਿਰਾ)
	    ret = 2;
    }

    ਜੇ (ret != 2)
    {
	// Save the text displayed in the command line area.
	ਲਈ (r = 0; r < ਪੰਕਤੀਆਂ_2; ++r)
	{
	    mch_memmove(screenline + r * ਖਾਨੇ_ਗਿਣਤੀ,
			Screen_ਸਤਰਾਂ + ਸਤਰ_ਵਿੱਥ[cmdline_ਪੰਕਤੀ + r],
			(size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(schar_T));
	    mch_memmove(screenattr + r * ਖਾਨੇ_ਗਿਣਤੀ,
			ScreenAttrs + ਸਤਰ_ਵਿੱਥ[cmdline_ਪੰਕਤੀ + r],
			(size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(sattr_T));
	    ਜੇ (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * ਖਾਨੇ_ਗਿਣਤੀ,
			    Screen_ਸਤਰਾਂ_UC + ਸਤਰ_ਵਿੱਥ[cmdline_ਪੰਕਤੀ + r],
			    (size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(u8char_T));
		ਲਈ (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * ਖਾਨੇ_ਗਿਣਤੀ,
				Screen_ਸਤਰਾਂ_C[i] + ਸਤਰ_ਵਿੱਥ[cmdline_ਪੰਕਤੀ + r],
				(size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(u8char_T));
	    }
	    ਜੇ (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * ਖਾਨੇ_ਗਿਣਤੀ,
			    ScreenLines2 + ਸਤਰ_ਵਿੱਥ[cmdline_ਪੰਕਤੀ + r],
			    (size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(schar_T));
	}

	update_screen(0);
	ret = 3;

	ਜੇ (must_redraw == 0)
	{
	    ਪੂਰਨ	off = (ਪੂਰਨ)(current_Screen_ਸਤਰ - Screen_ਸਤਰਾਂ);

	    // Restore the text displayed in the command line area.
	    ਲਈ (r = 0; r < ਪੰਕਤੀਆਂ_2; ++r)
	    {
		mch_memmove(current_Screen_ਸਤਰ,
			    screenline + r * ਖਾਨੇ_ਗਿਣਤੀ,
			    (size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * ਖਾਨੇ_ਗਿਣਤੀ,
			    (size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(sattr_T));
		ਜੇ (enc_utf8)
		{
		    mch_memmove(Screen_ਸਤਰਾਂ_UC + off,
				screenlineUC + r * ਖਾਨੇ_ਗਿਣਤੀ,
				(size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(u8char_T));
		    ਲਈ (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_ਸਤਰਾਂ_C[i] + off,
				    screenlineC[i] + r * ਖਾਨੇ_ਗਿਣਤੀ,
				    (size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(u8char_T));
		}
		ਜੇ (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * ਖਾਨੇ_ਗਿਣਤੀ,
				(size_t)ਖਾਨੇ_ਗਿਣਤੀ * ਆਕਾਰ(schar_T));
		screen_ਸਤਰ(curwin, cmdline_ਪੰਕਤੀ + r, 0, ਖਾਨੇ_ਗਿਣਤੀ, ਖਾਨੇ_ਗਿਣਤੀ, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    ਜੇ (enc_utf8)
    {
	vim_free(screenlineUC);
	ਲਈ (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    ਜੇ (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_ਸੁਨੇਹਾ();

    setcursor();

    ਵਾਪਸ ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    ਖਾਲੀ
redraw_after_callback(ਪੂਰਨ call_update_screen, ਪੂਰਨ do_ਸੁਨੇਹਾ_2)
{
    ++redrawing_for_callback;

    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_ENABLE);

    ਜੇ (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_ASKMORE
	    || State == ਬਹੁਲਕ_SETWSIZE || State == ਬਹੁਲਕ_EXTERNCMD
	    || State == ਬਹੁਲਕ_CONFIRM || exmode_ਸਰਗਰਮ)
    {
	ਜੇ (do_ਸੁਨੇਹਾ_2)
	    repeat_ਸੁਨੇਹਾ();
    }
    ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
    {
	ਜੇ (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	ਜੇ (cmdline_ਪੰਕਤੀ > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    ਜੇ (ਸੁਨੇਹਾ_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (State & (ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	ਜੇ (ਸੁਨੇਹਾ_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    ਸੁਨੇਹਾ_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_DISABLE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	ਬਾਹਰਲਾ_flush_cursor(FALSE, FALSE);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਬਾਹਰਲਾ_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    ਖਾਲੀ
redraw_later(ਪੂਰਨ type)
{
    redraw_win_later(curwin, type);
}

    ਖਾਲੀ
redraw_win_later(
    win_T	*wp,
    ਪੂਰਨ		type)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    ਜੇ (inside_redraw_on_ਸ਼ੁਰੂ_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	ਜੇ (type >= UPD_NOT_VALID)
	    wp->w_ਸਤਰਾਂ_valid = 0;
	ਜੇ (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    ਖਾਲੀ
redraw_later_clear(ਖਾਲੀ)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    ਖਾਲੀ
redraw_all_later(ਪੂਰਨ type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    ਸੈੱਟ_must_redraw(type);
}

#ਜੇ 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    ਖਾਲੀ
redraw_all_windows_later(ਪੂਰਨ type)
{
    redraw_all_later(type);
# ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Set "must_redraw" to "type" unless it already has a higher value
 * or it is currently not allowed.
 */
    ਖਾਲੀ
ਸੈੱਟ_must_redraw(ਪੂਰਨ type)
{
    ਜੇ (!redraw_not_allowed &&
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    !inside_redraw_on_ਸ਼ੁਰੂ_cb &&
#ਜੇ_ਅੰਤ
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    ਖਾਲੀ
redraw_curbuf_later(ਪੂਰਨ type)
{
    redraw_buf_later(curbuf, type);
}

    ਖਾਲੀ
redraw_buf_later(buf_T *buf, ਪੂਰਨ type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (wp->w_ਬਫਰ == buf)
	    redraw_win_later(wp, type);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	ਜੇ (wp->w_ਬਫਰ == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(curtab, wp)
	ਜੇ (wp->w_ਬਫਰ == buf)
	    redraw_win_later(wp, type);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    ਜੇ (curwin->w_ਬਫਰ == buf)
	redraw_win_later(curwin, type);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_SIGNS)
    ਖਾਲੀ
redraw_buf_ਸਤਰ_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_ਬਫਰ == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
    ਖਾਲੀ
redraw_buf_and_ਸਥਿਤੀ_later(buf_T *buf, ਪੂਰਨ type)
{
    win_T	*wp;

    ਜੇ (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	ਵਾਪਸ;
    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (wp->w_ਬਫਰ == buf)
	{
	    redraw_win_later(wp, type);
	    wp->w_redr_ਸਥਿਤੀ = true;
	}
    }
}
#ਜੇ_ਅੰਤ

/*
 * mark the ruler for redraw when the last window has no status line and the
 * ruler takes its place in the last screen line; showmode() draws it
 */
    ਸਥਿਰ ਖਾਲੀ
ruler_redraw_lastwin(ਖਾਲੀ)
{
    ਜੇ (p_ru && lastwin->w_ਸਥਿਤੀ_ਉਚਾਈ == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    ਖਾਲੀ
ਸਥਿਤੀ_redraw_all(ਖਾਲੀ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ)
	{
	    wp->w_redr_ਸਥਿਤੀ = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    ਖਾਲੀ
ਸਥਿਤੀ_redraw_curbuf(ਖਾਲੀ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_ਸਥਿਤੀ_ਉਚਾਈ != 0 && wp->w_ਬਫਰ == curbuf)
	{
	    wp->w_redr_ਸਥਿਤੀ = true;
	    redraw_later(UPD_VALID);
	}
    ਜੇ (lastwin->w_ਬਫਰ == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    ਖਾਲੀ
redraw_statuslines(ਖਾਲੀ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_redr_ਸਥਿਤੀ)
	{
	    bool ret = ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_ਸਥਿਤੀ(wp, FALSE);
	    ਜੇ (ret)
		ਕੱਢੋ_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    ਜੇ (redraw_tabline)
	draw_tabline();

#ਜੇ defined(FEAT_TABPANEL)
    ਜੇ (redraw_tabpanel)
	draw_tabpanel();
#ਜੇ_ਅੰਤ
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    ਖਾਲੀ
win_redraw_ਆਖਰੀ_ਸਥਿਤੀ(ਚੌਖਟਾ_T *frp)
{
    ਜੇ (frp->fr_layout == FR_ਪੱਤੀ)
	frp->fr_win->w_redr_ਸਥਿਤੀ = true;
    ਨਹੀਂਤਾਂ ਜੇ (frp->fr_layout == FR_ਪੰਕਤੀ)
    {
	FOR_ALL_ਚੌਖਟੇ(frp, frp->fr_ਬੱਚਾ)
	    win_redraw_ਆਖਰੀ_ਸਥਿਤੀ(frp);
    }
    ਨਹੀਂਤਾਂ // frp->fr_layout == FR_COL
    {
	frp = frp->fr_ਬੱਚਾ;
	ਜਦੋਂ (frp->fr_ਅਗਲਾ != ਨਿਰਾ)
	    frp = frp->fr_ਅਗਲਾ;
	win_redraw_ਆਖਰੀ_ਸਥਿਤੀ(frp);
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
    ਖਾਲੀ
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    ਖਾਲੀ
redraw_win_range_later(
    win_T	*wp,
    linenr_T	ਪਹਿਲਾ,
    linenr_T	ਆਖਰੀ)
{
    ਜੇ (ਆਖਰੀ >= wp->w_topline && ਪਹਿਲਾ < wp->w_botline)
    {
	ਜੇ (wp->w_redraw_ਸਿਖਰ == 0 || wp->w_redraw_ਸਿਖਰ > ਪਹਿਲਾ)
	    wp->w_redraw_ਸਿਖਰ = ਪਹਿਲਾ;
	ਜੇ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < ਆਖਰੀ)
	    wp->w_redraw_bot = ਆਖਰੀ;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    ਖਾਲੀ
redraw_win_range_now(
    win_T	*wp,
    linenr_T	ਪਹਿਲਾ,
    linenr_T	ਆਖਰੀ)
{
    ਜੇ (ਆਖਰੀ >= wp->w_topline && ਪਹਿਲਾ < wp->w_botline)
    {
	ਜੇ (wp->w_redraw_ਸਿਖਰ == 0 || wp->w_redraw_ਸਿਖਰ > ਪਹਿਲਾ)
	    wp->w_redraw_ਸਿਖਰ = ਪਹਿਲਾ;
	ਜੇ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < ਆਖਰੀ)
	    wp->w_redraw_bot = ਆਖਰੀ;
	ਜੇ (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ bool redraw_cb_ਵਿੱਚ_ਤਰੱਕੀ = false;

    ਖਾਲੀ
f_redraw_listener_ਜੋੜੋ(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    ਸਥਿਰ ਪੂਰਨ		id;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (redraw_cb_ਵਿੱਚ_ਤਰੱਕੀ)
    {
	emsg(_(e_cannot_ਜੋੜੋ_redraw_listener_ਵਿੱਚ_listener_callback));
	ਵਾਪਸ;
    }

    ਜੇ (check_for_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    ਜੇ (rln == ਨਿਰਾ)
	ਵਾਪਸ;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    ਜੇ (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	ਜੇ (cb.cb_name == ਨਿਰਾ)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    ਵਾਪਸ;
	}
	copy_callback(&rln->rl_callbacks.on_ਸ਼ੁਰੂ, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    ਜੇ (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	ਜੇ (cb.cb_name == ਨਿਰਾ)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_ਸ਼ੁਰੂ);
	    vim_free(rln);
	    ਵਾਪਸ;
	}
	copy_callback(&rln->rl_callbacks.on_ਸਿਰਾ, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    ਜੇ (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	ਵਾਪਸ;
    }

    rln->rl_ਅਗਲਾ = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_ਸੰਖਿਆ;
    rettv->vval.v_ਸੰਖਿਆ = id;

    ਵਾਪਸ;
}

    ਸਥਿਰ ਖਾਲੀ
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_ਸ਼ੁਰੂ);
    free_callback(&rln->rl_callbacks.on_ਸਿਰਾ);

    vim_free(rln);
}


    ਸਥਿਰ ਖਾਲੀ
redraw_listener_cleanup(ਖਾਲੀ)
{
    ਲਈ (redraw_listener_T *rln = redraw_listeners; rln != ਨਿਰਾ;)
    {
	redraw_listener_T *ਅਗਲਾ = rln->rl_ਅਗਲਾ;
	ਜੇ (rln->rl_id == 0)
	{
	    ਜੇ (redraw_listeners == rln)
		redraw_listeners = rln->rl_ਅਗਲਾ;
	    redraw_listener_free(rln);
	}
	rln = ਅਗਲਾ;
    }
}

/*
 * Return the redraw listener struct with the specified id. Returns NULL if not
 * found.
 */
    ਸਥਿਰ redraw_listener_T *
get_redraw_listener(ਪੂਰਨ id)
{
    ਲਈ (redraw_listener_T *rln = redraw_listeners; rln != ਨਿਰਾ; rln = rln->rl_ਅਗਲਾ)
	ਜੇ (rln->rl_id == id)
	    ਵਾਪਸ rln;
    ਵਾਪਸ ਨਿਰਾ;
}

    ਖਾਲੀ
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ			id;
    redraw_listener_T	*rln;

    ਜੇ (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = argvars[0].vval.v_ਸੰਖਿਆ;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_ਸੰਖਿਆ;
    ਜੇ (rln == ਨਿਰਾ)
    {
	rettv->vval.v_ਸੰਖਿਆ = 0;
	ਵਾਪਸ;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_ਸੰਖਿਆ = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    ਸਥਿਰ ਖਾਲੀ
invoke_redraw_listener_ਸ਼ੁਰੂ_or_ਸਿਰਾ(bool ਸ਼ੁਰੂ)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    ਜੇ (ਸ਼ੁਰੂ)
	inside_redraw_on_ਸ਼ੁਰੂ_cb = true;

    redraw_cb_ਵਿੱਚ_ਤਰੱਕੀ = true;
    ਲਈ (redraw_listener_T *rln = redraw_listeners; rln != ਨਿਰਾ; rln = rln->rl_ਅਗਲਾ)
    {
	ਜੇ (rln->rl_id == 0)
	    // Listener has been removed, skip
	    ਜਾਰੀ;
	ਜੇ (ਸ਼ੁਰੂ && rln->rl_callbacks.on_ਸ਼ੁਰੂ.cb_name != ਨਿਰਾ)
	{
	    call_callback(&rln->rl_callbacks.on_ਸ਼ੁਰੂ, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	ਨਹੀਂਤਾਂ ਜੇ (rln->rl_callbacks.on_ਸਿਰਾ.cb_name != ਨਿਰਾ)
	{
	    call_callback(&rln->rl_callbacks.on_ਸਿਰਾ, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_ਵਿੱਚ_ਤਰੱਕੀ = false;

    ਜੇ (ਸ਼ੁਰੂ)
	inside_redraw_on_ਸ਼ੁਰੂ_cb = false;
}
#ਜੇ_ਅੰਤ // FEAT_EVAL
