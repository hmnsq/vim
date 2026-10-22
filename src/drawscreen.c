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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
ಸ್ಥಿರ ಶೂನ್ಯ redraw_custom_statusline(win_T *wp);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ borrow_stl_vsep_hl(ಶೂನ್ಯ);
#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_CLIPBOARD)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  did_update_one_ಕಿಟಕಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಶೂನ್ಯ redraw_listener_cleanup(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ invoke_redraw_listener_ಆರಂಭ_or_ಕೊನೆ(bool ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    ಪೂರ್ಣಾಂಕ
update_screen(ಪೂರ್ಣಾಂಕ type_arg)
{
    ಪೂರ್ಣಾಂಕ		type = type_arg;
    win_T	*wp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_intro = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಪೂರ್ಣಾಂಕ		did_one = FALSE;
    ಪೂರ್ಣಾಂಕ		did_undraw = FALSE;
    ಪೂರ್ಣಾಂಕ		gui_cursor_ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		gui_cursor_ಅಡ್ಡಸಾಲು = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		no_update = FALSE;
    ಪೂರ್ಣಾಂಕ		save_pum_will_redraw = pum_will_redraw;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಪೂರ್ಣಾಂಕ		did_redraw_ಕಿಟಕಿ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    ಆದರೆ (!screen_valid(TRUE))
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (type == UPD_VALID_NO_UPDATE)
    {
	no_update = TRUE;
	type = 0;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    {
	buf_T *buf;

	// Before updating the screen, notify any listeners of changed text.
	FOR_ALL_BUFFERS(buf)
	    invoke_listeners(buf);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    // May have postponed updating diffs.
    ಆದರೆ (need_diff_redraw)
	diff_redraw(TRUE);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (must_redraw)
    {
	ಆದರೆ (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    ಆದರೆ (curwin->w_ಸಾಲುಗಳು_valid == 0 && type < UPD_NOT_VALID
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    && !ಪದ_do_update_ಕಿಟಕಿ(curwin)
#ಆದರೆ_ಅಂತ್ಯ
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    ಆದರೆ (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	ಆದರೆ (type > UPD_INVERTED_ALL)
	    curwin->w_ಸಾಲುಗಳು_valid = 0;	// don't use w_lines[].wl_size now
	ಹಿಂತಿರುಗಿ FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    ಪೂರ್ಣಾಂಕ hid_cursor = !sync_output_ಸಕ್ರಿಯ();
    ಆದರೆ (hid_cursor)
	cursor_off();
    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_ENABLE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // Update popup_mask if needed.  This may set w_redraw_top and w_redraw_bot
    // in some windows.
    may_update_popup_ಮುಖವಾಡ(type);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ++display_tick;	    // let syntax code know we're in a next round of
			    // display updating
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (no_update)
	++no_win_do_ಸಾಲುಗಳು_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    ಆದರೆ (ಸಂದೇಶ_scrolled)
    {
	clear_cmdline = TRUE;
	ಆದರೆ (type != UPD_CLEAR)
	{
	    ಆದರೆ (ಸಂದೇಶ_scrolled > ಅಡ್ಡಸಾಲುಗಳು - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		check_for_delay(FALSE);
		ಆದರೆ (screen_ins_ಸಾಲುಗಳು(0, 0, ಸಂದೇಶ_scrolled, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, 0, ಖಾಲಿ)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    ಆದರೆ (wp->w_winrow < ಸಂದೇಶ_scrolled)
		    {
			ಆದರೆ (W_WINROW(wp) + wp->w_ಎತ್ತರ > ಸಂದೇಶ_scrolled
				&& wp->w_redr_type < UPD_REDRAW_ತುದಿ
				&& wp->w_ಸಾಲುಗಳು_valid > 0
				&& wp->w_topline == wp->w_ಸಾಲುಗಳು[0].wl_lnum)
			{
			    wp->w_upd_ಅಡ್ಡಸಾಲುಗಳು = ಸಂದೇಶ_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_ತುದಿ;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    ಆದರೆ (W_WINROW(wp) + wp->w_ಎತ್ತರ
					 + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ <= ಸಂದೇಶ_scrolled)
				wp->w_redr_ಸ್ಥಿತಿ = true;
			}
		    }
		}
		ಆದರೆ (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#ಆದರೆ defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಸಂದೇಶ_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    ಆದರೆ (need_highlight_ಬದಲಾಯಿತು)
	highlight_ಬದಲಾಯಿತು();

    ಆದರೆ (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    ಆದರೆ (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    ಆದರೆ (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? ಸಂಖ್ಯೆ_ಅಗಲ(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    invoke_redraw_listener_ಆರಂಭ_or_ಕೊನೆ(true);
#ಆದರೆ_ಅಂತ್ಯ

    // Only start redrawing if there is really something to do.
    ಆದರೆ (type == UPD_INVERTED)
	update_curswant();
    ಆದರೆ (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_ಸಾಲುಗಳು[0].wl_valid
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#ಆದರೆ_ಅಂತ್ಯ
		    && curwin->w_topline == curwin->w_ಸಾಲುಗಳು[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_ಸಕ್ರಿಯ
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_ಬಹುಲಕ == VIsual_ಬಹುಲಕ
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    ಆದರೆ (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#ಆದರೆ defined(FEAT_TABPANEL)
    ಆದರೆ (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (wp->w_ಬಫರ್->b_mod_ಗಣ)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    ಗಾಗಿ (wwp = firstwin; wwp != wp; wwp = wwp->w_ಮುಂದಿನ)
		ಆದರೆ (wwp->w_ಬಫರ್ == wp->w_ಬಫರ್)
		    ನಿಲ್ಲಿಸು;
	    ಆದರೆ (wwp == wp && syntax_present(wp))
		syn_ರಾಶಿ_apply_changes(wp->w_ಬಫರ್);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (pum_redraw_ಒಳ_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_ಕಿಟಕಿ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    screen_ಹುಡುಕು_hl.rm.regprog = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    FOR_ALL_WINDOWS(wp)
    {
	override_success = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ಆದರೆ (wp->w_redr_type != 0)
	{
	    cursor_off();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    did_redraw_ಕಿಟಕಿ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    ಆದರೆ (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		ಆದರೆ (gui.ಒಳ_use && (wp == curwin || cursor_is_sleeping()))
		{
		    gui_cursor_ಲಂಬಸಾಲು = gui.cursor_ಲಂಬಸಾಲು;
		    gui_cursor_ಅಡ್ಡಸಾಲು = gui.cursor_ಅಡ್ಡಸಾಲು;
		    gui_undraw_cursor();
		    did_undraw = TRUE;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    win_update(wp);
	}

	// redraw status line after the window to minimize cursor movement
	ಆದರೆ (wp->w_redr_ಸ್ಥಿತಿ)
	{
	    cursor_off();
	    win_redr_ಸ್ಥಿತಿ(wp, TRUE); // any popup menu will be redrawn below
	}

	ಆದರೆ (override_success)
	    ತೆಗೆ_highlight_overrides();

    }
#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
    ಕೊನೆ_ಹುಡುಕು_hl();
#ಆದರೆ_ಅಂತ್ಯ

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    ಆದರೆ (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (wp->w_vsep_ಅಗಲ > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_ಬಫರ್->b_mod_ಗಣ = false;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    ಆದರೆ (did_redraw_ಕಿಟಕಿ || popup_need_redraw())
	update_popups(win_update);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    FOR_ALL_WINDOWS(wp)
	// If this window contains a terminal, after redrawing all windows, the
	// dirty row range can be reset.
	ಪದ_did_update_ಕಿಟಕಿ(wp);
#ಆದರೆ_ಅಂತ್ಯ

    after_updating_screen(TRUE);

    // Clear or redraw the command line.  Done last, because scrolling may
    // mess up the command line.
    ಆದರೆ (clear_cmdline || redraw_cmdline || redraw_ಬಹುಲಕ)
	showmode();

    ಆದರೆ (no_update)
	--no_win_do_ಸಾಲುಗಳು_ins;

    // May put up an introductory message when not editing a file
    ಆದರೆ (!did_intro)
	maybe_intro_ಸಂದೇಶ();
    did_intro = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    ಆದರೆ (gui.ಒಳ_use)
    {
	ಆದರೆ (did_undraw && !gui_mch_is_blink_off())
	{
	    mch_disable_flush();
	    ಹೊರಹೋಗು_flush();	// required before updating the cursor
	    mch_enable_flush();

	    // Put the GUI position where the cursor was, gui_update_cursor()
	    // uses that.
	    gui.ಲಂಬಸಾಲು = gui_cursor_ಲಂಬಸಾಲು;
	    gui.ಅಡ್ಡಸಾಲು = gui_cursor_ಅಡ್ಡಸಾಲು;
	    gui.ಲಂಬಸಾಲು = mb_fix_ಲಂಬಸಾಲು(gui.ಲಂಬಸಾಲು, gui.ಅಡ್ಡಸಾಲು);
	    gui_update_cursor(FALSE, FALSE);
	    gui_may_flush();
	    screen_cur_ಲಂಬಸಾಲು = gui.ಲಂಬಸಾಲು;
	    screen_cur_ಅಡ್ಡಸಾಲು = gui.ಅಡ್ಡಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೊರಹೋಗು_flush();
	gui_update_scrollbars(FALSE);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
    // Make sure to do this last!
    draw_image_placements();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    invoke_redraw_listener_ಆರಂಭ_or_ಕೊನೆ(false);
    redraw_listener_cleanup();
#ಆದರೆ_ಅಂತ್ಯ

    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_DISABLE);
    ಆದರೆ (hid_cursor)
	cursor_on();

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    ಪೂರ್ಣಾಂಕ
statusline_ಅಡ್ಡಸಾಲು(win_T *wp)
{
#ಆದರೆ defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    ಆದರೆ (wp->w_ಚೌಕಟ್ಟು->fr_ಎತ್ತರ == wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ && !popup_is_popup(wp))
	ಹಿಂತಿರುಗಿ wp->w_winrow;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ W_WINROW(wp) + wp->w_ಎತ್ತರ;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    ಶೂನ್ಯ
win_redr_ಸ್ಥಿತಿ(win_T *wp, ಪೂರ್ಣಾಂಕ ignore_pum ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		fillchar;
    ಪೂರ್ಣಾಂಕ		attr;
    ಪೂರ್ಣಾಂಕ		i;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    ಆದರೆ (busy)
	ಹಿಂತಿರುಗಿ;
    busy = TRUE;

    ಅಡ್ಡಸಾಲು = statusline_ಅಡ್ಡಸಾಲು(wp);

    wp->w_redr_ಸ್ಥಿತಿ = false;
    ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_ಸ್ಥಿತಿ = true;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	*p;
	ಪೂರ್ಣಾಂಕ	plen;
	ಪೂರ್ಣಾಂಕ	NameBufflen;
	ಪೂರ್ಣಾಂಕ	this_ru_ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ	n;			// scratch value

	fillchar = fillchar_ಸ್ಥಿತಿ(&attr, wp);

	get_trans_bufname(wp->w_ಬಫರ್);
	p = NameBuff;
	plen = (ಪೂರ್ಣಾಂಕ)STRLEN(p);

	ಆದರೆ ((bt_help(wp->w_ಬಫರ್)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		    || wp->w_p_pvw
#ಆದರೆ_ಅಂತ್ಯ
		    || buf_Is_ಬದಲಾಯಿತು(wp->w_ಬಫರ್)
		    || wp->w_ಬಫರ್->b_p_ro)
		&& plen < MAXPATHL - 1)
	{
	    *(p + plen++) = ' ';	// replace NUL with space
	    *(p + plen) = NUL;		// NUL terminate the string
	}
	ಆದರೆ (bt_help(wp->w_ಬಫರ್))
	    plen += vim_snprintf((ಅಕ್ಷರ *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	ಆದರೆ (wp->w_p_pvw)
	    plen += vim_snprintf((ಅಕ್ಷರ *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(wp->w_ಬಫರ್) && !bt_terminal(wp->w_ಬಫರ್))
	    plen += vim_snprintf((ಅಕ್ಷರ *)p + plen, MAXPATHL - plen, "%s", "[+]");
	ಆದರೆ (wp->w_ಬಫರ್->b_p_ro)
	    plen += vim_snprintf((ಅಕ್ಷರ *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_ಲಂಬಸಾಲು = ru_ಲಂಬಸಾಲು - (cmdline_ಅಗಲ - wp->w_ಅಗಲ);
	n = (wp->w_ಅಗಲ + 1) / 2;
	ಆದರೆ (this_ru_ಲಂಬಸಾಲು < n)
	    this_ru_ಲಂಬಸಾಲು = n;
	ಆದರೆ (this_ru_ಲಂಬಸಾಲು <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    ಗಾಗಿ (i = 0; p[i] != NUL && plen >= this_ru_ಲಂಬಸಾಲು - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    ಆದರೆ (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (plen > this_ru_ಲಂಬಸಾಲು - 1)
	{
	    p += plen - (this_ru_ಲಂಬಸಾಲು - 1);
	    *p = '<';
	    plen = this_ru_ಲಂಬಸಾಲು - 1;
	}

	screen_puts(p, ಅಡ್ಡಸಾಲು, wp->w_wincol, attr);
	screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1, plen + wp->w_wincol,
		    this_ru_ಲಂಬಸಾಲು + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	ಗಾಗಿ (i = 1; i < wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ; i++)
	    screen_fill(ಅಡ್ಡಸಾಲು + i, ಅಡ್ಡಸಾಲು + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	ಆದರೆ ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_ಲಂಬಸಾಲು - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, ಅಡ್ಡಸಾಲು, (ಪೂರ್ಣಾಂಕ)(this_ru_ಲಂಬಸಾಲು - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	ಆದರೆ (p_sc && *p_sloc == 's')
	{
	    n = this_ru_ಲಂಬಸಾಲು - plen - 2;		    // perform the calculation here so we only do it once
	    ಪೂರ್ಣಾಂಕ	ಅಗಲ = MIN(10, n);

	    ಆದರೆ (ಅಗಲ > 0)
		screen_puts_len(showcmd_buf, ಅಗಲ, ಅಡ್ಡಸಾಲು,
				wp->w_wincol + this_ru_ಲಂಬಸಾಲು - ಅಗಲ - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    ಆದರೆ (wp->w_vsep_ಅಗಲ != 0 && wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0 && redrawing())
    {
	ಗಾಗಿ (i = 0; i < wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ; i++)
	{
	    ಪೂರ್ಣಾಂಕ r = ಅಡ್ಡಸಾಲು + i;
	    ಆದರೆ (stl_connected(wp))
		fillchar = fillchar_ಸ್ಥಿತಿ(&attr, wp);
	    ಇಲ್ಲದಿದ್ದರೆ
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
    win_T   *ಎಡ_2 = ಖಾಲಿ;
    win_T   *ಬಲ_2 = ಖಾಲಿ;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    ಆದರೆ (!redrawing() || Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_WINDOWS(ಎಡ_2)
    {
	ಆದರೆ (ಎಡ_2->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0 || ಎಡ_2->w_vsep_ಅಗಲ == 0)
	    ಮುಂದುವರಿ;
	// A window of zero width has no status line cell to borrow from.
	ಆದರೆ (ಎಡ_2->w_ಅಗಲ == 0)
	    ಮುಂದುವರಿ;
	ಆದರೆ (!stl_connected(ಎಡ_2))
	    ಮುಂದುವರಿ;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ ಆರಂಭ = 0;
	ಪೂರ್ಣಾಂಕ ಕೊನೆ = 0;

	FOR_ALL_WINDOWS(ಬಲ_2)
	{
	    ಆದರೆ (ಬಲ_2 == ಎಡ_2 || ಬಲ_2->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)
		ಮುಂದುವರಿ;
	    ಆದರೆ (ಬಲ_2->w_wincol != W_ENDCOL(ಎಡ_2) + 1)
		ಮುಂದುವರಿ;
	    ಪೂರ್ಣಾಂಕ l_stl_ಅಡ್ಡಸಾಲು = W_WINROW(ಎಡ_2) + ಎಡ_2->w_ಎತ್ತರ;
	    ಪೂರ್ಣಾಂಕ r_stl_ಅಡ್ಡಸಾಲು = W_WINROW(ಬಲ_2) + ಬಲ_2->w_ಎತ್ತರ;

	    ಆರಂಭ = l_stl_ಅಡ್ಡಸಾಲು > r_stl_ಅಡ್ಡಸಾಲು ? l_stl_ಅಡ್ಡಸಾಲು : r_stl_ಅಡ್ಡಸಾಲು;
	    ಕೊನೆ = l_stl_ಅಡ್ಡಸಾಲು + ಎಡ_2->w_ಸ್ಥಿತಿ_ಎತ್ತರ
				< r_stl_ಅಡ್ಡಸಾಲು + ಬಲ_2->w_ಸ್ಥಿತಿ_ಎತ್ತರ
		? l_stl_ಅಡ್ಡಸಾಲು + ಎಡ_2->w_ಸ್ಥಿತಿ_ಎತ್ತರ
		: r_stl_ಅಡ್ಡಸಾಲು + ಬಲ_2->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	    ಆದರೆ (ಆರಂಭ < ಕೊನೆ)
	    {
		neighbour = ಬಲ_2;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (neighbour == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	ಪೂರ್ಣಾಂಕ	hl;
	ಆದರೆ (ಎಡ_2 != curwin && neighbour != curwin
		&& fillchar_ಸ್ಥಿತಿ(&hl, ಎಡ_2) != ' ')
	    ಮುಂದುವರಿ;

	// Source: prefer curwin's side; otherwise left window's right edge.
	ಪೂರ್ಣಾಂಕ dst_ಲಂಬಸಾಲು = W_ENDCOL(ಎಡ_2);
	ಪೂರ್ಣಾಂಕ src_ಲಂಬಸಾಲು = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(ಎಡ_2) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	ಆದರೆ (dst_ಲಂಬಸಾಲು >= screen_Columns || src_ಲಂಬಸಾಲು >= screen_Columns)
	    ಮುಂದುವರಿ;
	ಆದರೆ (ಕೊನೆ > screen_ಅಡ್ಡಸಾಲುಗಳು)
	    ಕೊನೆ = screen_ಅಡ್ಡಸಾಲುಗಳು;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ r = ಆರಂಭ; r < ಕೊನೆ; r++)
	{
	    ಚಿಹ್ನೆರಹಿತ	dst_off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[r] + dst_ಲಂಬಸಾಲು;
	    sattr_T	attr = ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[r] + src_ಲಂಬಸಾಲು];

	    ಆದರೆ (ScreenAttrs[dst_off] != attr)
	    {
		ScreenAttrs[dst_off] = attr;
		screen_char(dst_off, r, dst_ಲಂಬಸಾಲು);
	    }
	}
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
/*
 * Redraw the status line according to 'statusline' and take care of any
 * errors encountered.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_custom_statusline(win_T *wp)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    ಆದರೆ (entered)
	ಹಿಂತಿರುಗಿ;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    ಶೂನ್ಯ
showruler(ಪೂರ್ಣಾಂಕ always)
{
    ಆದರೆ (!always && !redrawing())
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_ಸ್ಥಿತಿ = true;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ defined(FEAT_STL_OPT)
    ಆದರೆ ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	win_redr_ruler(curwin, always, FALSE);

    ಆದರೆ (need_maketitle
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_ಒಳ_ICON))
	    || (p_title && (stl_syntax & STL_ಒಳ_TITLE))
#ಆದರೆ_ಅಂತ್ಯ
       )
	maketitle();

    // Redraw the tab pages line if needed.
    ಆದರೆ (redraw_tabline)
	draw_tabline();

#ಆದರೆ defined(FEAT_TABPANEL)
    ಆದರೆ (redraw_tabpanel)
	draw_tabpanel();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
win_redr_ruler(win_T *wp, ಪೂರ್ಣಾಂಕ always, ಪೂರ್ಣಾಂಕ ignore_pum)
{
    ಪೂರ್ಣಾಂಕ	ಖಾಲಿಗಳು_ಸಾಲು = FALSE;

    // If 'ruler' off don't do anything
    ಆದರೆ (!p_ru)
	ಹಿಂತಿರುಗಿ;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    ಆದರೆ (wp->w_cursor.lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    ಆದರೆ (wp == lastwin && lastwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)
	ಆದರೆ (edit_submode != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    ಆದರೆ (!ignore_pum && pum_visible())
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    ಆದರೆ (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0
		&& *ml_get_buf(wp->w_ಬಫರ್, wp->w_cursor.lnum, FALSE) == NUL)
	ಖಾಲಿಗಳು_ಸಾಲು = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    ಆದರೆ (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.ಲಂಬಸಾಲು != wp->w_ru_cursor.ಲಂಬಸಾಲು
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ != wp->w_ru_ಸಾಲು_ಎಣಿಕೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#ಆದರೆ_ಅಂತ್ಯ
	    || ಖಾಲಿಗಳು_ಸಾಲು != wp->w_ru_ಖಾಲಿಗಳು)
    {
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು;
	ಪೂರ್ಣಾಂಕ	fillchar;
	ಪೂರ್ಣಾಂಕ	attr;
	ಪೂರ್ಣಾಂಕ	off;
	ಪೂರ್ಣಾಂಕ	ಅಗಲ;
	colnr_T	virtcol;
#ವ್ಯಾಖ್ಯಾನಿಸು RULER_BUF_LEN 70
	char_u	ಬಫರ್[RULER_BUF_LEN];
	ಪೂರ್ಣಾಂಕ	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	ಪೂರ್ಣಾಂಕ	rel_poslen;
	ಪೂರ್ಣಾಂಕ	this_ru_ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ	n1;			    // scratch value
	ಪೂರ್ಣಾಂಕ	n2;			    // scratch value
	bool	override_success =
	    ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ)
	{
	    ಅಡ್ಡಸಾಲು = statusline_ಅಡ್ಡಸಾಲು(wp);
	    fillchar = fillchar_ಸ್ಥಿತಿ(&attr, wp);
	    off = wp->w_wincol;
	    ಅಗಲ = wp->w_ಅಗಲ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_ಲಂಬಸಾಲು_off;
	    ಅಗಲ = cmdline_ಅಗಲ;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	ಆದರೆ (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, ಖಾಲಿ, &virtcol, ಖಾಲಿ, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((ಅಕ್ಷರ *)ಬಫರ್, RULER_BUF_LEN, _("%ld,"),
		(wp->w_ಬಫರ್->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
		    ? 0L
		    : (ದೀರ್ಘ)(wp->w_cursor.lnum));
	bufferlen += ಲಂಬಸಾಲು_print(ಬಫರ್ + bufferlen, RULER_BUF_LEN - bufferlen,
			ಖಾಲಿಗಳು_ಸಾಲು ? 0 : (ಪೂರ್ಣಾಂಕ)wp->w_cursor.ಲಂಬಸಾಲು + 1,
			(ಪೂರ್ಣಾಂಕ)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)	// can't use last char of screen
	    ++n1;

	this_ru_ಲಂಬಸಾಲು = ru_ಲಂಬಸಾಲು - (cmdline_ಅಗಲ - ಅಗಲ);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (ಅಗಲ + 1) / 2;
	ಆದರೆ (this_ru_ಲಂಬಸಾಲು < n2)
	    this_ru_ಲಂಬಸಾಲು = n2;
	ಆದರೆ (this_ru_ಲಂಬಸಾಲು + n1 < ಅಗಲ)
	{
	    // need at least space for rel_pos + NUL
	    ಆಗ (this_ru_ಲಂಬಸಾಲು + n1 < ಅಗಲ
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		ಆದರೆ (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, ಬಫರ್ + bufferlen);
		ಇಲ್ಲದಿದ್ದರೆ
		    ಬಫರ್[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((ಅಕ್ಷರ *)ಬಫರ್ + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	ಆದರೆ (has_mbyte)
	{
	    ಗಾಗಿ (n1 = 0, n2 = 0; ಬಫರ್[n1] != NUL; n1 += (*mb_ptr2len)(ಬಫರ್ + n1))
	    {
		n2 += (*mb_ptr2cells)(ಬಫರ್ + n1);
		ಆದರೆ (this_ru_ಲಂಬಸಾಲು + n2 > ಅಗಲ)
		{
		    bufferlen = n1;
		    ಬಫರ್[bufferlen] = NUL;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (this_ru_ಲಂಬಸಾಲು + bufferlen > ಅಗಲ)
	{
	    bufferlen = ಅಗಲ - this_ru_ಲಂಬಸಾಲು;
	    ಬಫರ್[bufferlen] = NUL;
	}

	screen_puts(ಬಫರ್, ಅಡ್ಡಸಾಲು, this_ru_ಲಂಬಸಾಲು + off, attr);
	n1 = redraw_cmdline;
	screen_fill(ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + 1,
		this_ru_ಲಂಬಸಾಲು + off + bufferlen,
		(off + ಅಗಲ),
		fillchar, fillchar, attr);
	// don't redraw the cmdline because of showing the ruler
	redraw_cmdline = n1;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_ಖಾಲಿಗಳು = ಖಾಲಿಗಳು_ಸಾಲು;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_ಸಾಲು_ಎಣಿಕೆ = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (override_success)
	    ತೆಗೆ_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    ಶೂನ್ಯ
after_updating_screen(ಪೂರ್ಣಾಂಕ may_resize_shell ಬಳಸದ)
{
    updating_screen = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (may_resize_shell)
	gui_may_resize_shell();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪದ_check_channel_closed_recently();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_DROP_FILE
    // If handle_drop() was called while updating_screen was TRUE need to
    // handle the drop now.
    handle_any_postponed_drop();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Update all windows that are editing the current buffer.
 */
    ಶೂನ್ಯ
update_curbuf(ಪೂರ್ಣಾಂಕ type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#ಆದರೆ defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪಠ್ಯ_ಗಮ್ಯ_screenline(win_T *wp, char_u *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ		off = (ಪೂರ್ಣಾಂಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);

    ಆದರೆ (has_mbyte)
    {
	ಪೂರ್ಣಾಂಕ	cells;
	ಪೂರ್ಣಾಂಕ	u8c, u8cc[MAX_MCO];
	ಪೂರ್ಣಾಂಕ	i;
	ಪೂರ್ಣಾಂಕ	idx;
	ಪೂರ್ಣಾಂಕ	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	ಪೂರ್ಣಾಂಕ	prev_c = 0;		// previous Arabic character
	ಪೂರ್ಣಾಂಕ	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	ಆದರೆ (wp->w_p_rl)
	    idx = off;
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    idx = off + ಲಂಬಸಾಲು;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	ಗಾಗಿ (p = ಪಠ್ಯ; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    ಆದರೆ (ಲಂಬಸಾಲು + cells > wp->w_ಅಗಲ)
		ನಿಲ್ಲಿಸು;
# ifdef FEAT_RIGHTLEFT
	    ಆದರೆ (wp->w_p_rl)
		idx = off + wp->w_ಅಗಲ - ಲಂಬಸಾಲು - cells;
# endif
	    Screen_ಸಾಲುಗಳು[idx] = *p;
	    ಆದರೆ (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		ಆದರೆ (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_ಸಾಲುಗಳು_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    prev_c = u8c;
# endif
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
# ifdef FEAT_ARABIC
		    ಆದರೆ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			ಪೂರ್ಣಾಂಕ	pc, pc1, nc;
			ಪೂರ್ಣಾಂಕ	pcc[MAX_MCO];
			ಪೂರ್ಣಾಂಕ	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ಆದರೆ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			ಇಲ್ಲದಿದ್ದರೆ
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
		    ಇಲ್ಲದಿದ್ದರೆ
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_ಸಾಲುಗಳು_UC[idx] = u8c;
		    ಗಾಗಿ (i = 0; i < Screen_mco; ++i)
		    {
			Screen_ಸಾಲುಗಳು_C[i][idx] = u8cc[i];
			ಆದರೆ (u8cc[i] == 0)
			    ನಿಲ್ಲಿಸು;
		    }
		}
		ಆದರೆ (cells > 1)
		    Screen_ಸಾಲುಗಳು[idx + 1] = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cells > 1)
		// double-width character
		Screen_ಸಾಲುಗಳು[idx + 1] = p[1];
	    ಲಂಬಸಾಲು += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಪಠ್ಯ);
	ಪೂರ್ಣಾಂಕ n = wp->w_ಅಗಲ - ಲಂಬಸಾಲು;

	ಆದರೆ (len > n)
	    len = n;
	ಆದರೆ (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    ಆದರೆ (wp->w_p_rl)
		mch_memmove(current_Screen_ಸಾಲು, ಪಠ್ಯ, len);
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		mch_memmove(current_Screen_ಸಾಲು + ಲಂಬಸಾಲು, ಪಠ್ಯ, len);
	    ಲಂಬಸಾಲು += len;
	}
    }
    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
/*
 * Draw the window toolbar.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_idx = 0;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		off = (ಪೂರ್ಣಾಂಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
    bool	override_success =
	ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ಪೂರ್ಣಾಂಕ		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    ಪೂರ್ಣಾಂಕ		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_ವಸ್ತುಗಳು);
    FOR_ALL_ಮಗು_MENUS(wp->w_winbar, menu)
	++ಅಂಶ_ಎಣಿಕೆ;
    wp->w_winbar_ವಸ್ತುಗಳು = ALLOC_CLEAR_MULT(winbar_ಅಂಶ_T, ಅಂಶ_ಎಣಿಕೆ + 1);

    // TODO: use fewer spaces if there is not enough room
    ಗಾಗಿ (menu = wp->w_winbar->children;
			  menu != ಖಾಲಿ && ಲಂಬಸಾಲು < wp->w_ಅಗಲ; menu = menu->ಮುಂದಿನ)
    {
	space_ಗಮ್ಯ_screenline(off + ಲಂಬಸಾಲು, fill_attr);
	ಆದರೆ (++ಲಂಬಸಾಲು >= wp->w_ಅಗಲ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ಲಂಬಸಾಲು > 1)
	{
	    space_ಗಮ್ಯ_screenline(off + ಲಂಬಸಾಲು, fill_attr);
	    ಆದರೆ (++ಲಂಬಸಾಲು >= wp->w_ಅಗಲ)
		ನಿಲ್ಲಿಸು;
	}

	wp->w_winbar_ವಸ್ತುಗಳು[ಅಂಶ_idx].wb_startcol = ಲಂಬಸಾಲು;
	space_ಗಮ್ಯ_screenline(off + ಲಂಬಸಾಲು, button_attr);
	ಆದರೆ (++ಲಂಬಸಾಲು >= wp->w_ಅಗಲ)
	    ನಿಲ್ಲಿಸು;

	ಮುಂದಿನ_ಲಂಬಸಾಲು = ಪಠ್ಯ_ಗಮ್ಯ_screenline(wp, menu->name, ಲಂಬಸಾಲು);
	ಆಗ (ಲಂಬಸಾಲು < ಮುಂದಿನ_ಲಂಬಸಾಲು)
	{
	    ScreenAttrs[off + ಲಂಬಸಾಲು] = button_attr;
	    ++ಲಂಬಸಾಲು;
	}
	wp->w_winbar_ವಸ್ತುಗಳು[ಅಂಶ_idx].wb_endcol = ಲಂಬಸಾಲು;
	wp->w_winbar_ವಸ್ತುಗಳು[ಅಂಶ_idx].wb_menu = menu;
	++ಅಂಶ_idx;

	ಆದರೆ (ಲಂಬಸಾಲು >= wp->w_ಅಗಲ)
	    ನಿಲ್ಲಿಸು;
	space_ಗಮ್ಯ_screenline(off + ಲಂಬಸಾಲು, button_attr);
	++ಲಂಬಸಾಲು;
    }
    ಆಗ (ಲಂಬಸಾಲು < wp->w_ಅಗಲ)
    {
	space_ಗಮ್ಯ_screenline(off + ಲಂಬಸಾಲು, fill_attr);
	++ಲಂಬಸಾಲು;
    }
    wp->w_winbar_ವಸ್ತುಗಳು[ಅಂಶ_idx].wb_menu = ಖಾಲಿ; // end marker

    screen_ಸಾಲು(wp, wp->w_winrow, wp->w_wincol, wp->w_ಅಗಲ,
							  wp->w_ಅಗಲ, -1, 0);

    ಆದರೆ (override_success)
	ತೆಗೆ_highlight_overrides();
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_ಪಠ್ಯ_attr(
    ಪೂರ್ಣಾಂಕ		off,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		attr)
{
    ಪೂರ್ಣಾಂಕ		i;

    mch_memmove(Screen_ಸಾಲುಗಳು + off, buf, (size_t)len);
    ಆದರೆ (enc_utf8)
	vim_memset(Screen_ಸಾಲುಗಳು_UC + off, 0, ಗಾತ್ರ(u8char_T) * (size_t)len);
    ಗಾಗಿ (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fold_ಸಾಲು(
    win_T	*wp,
    ದೀರ್ಘ	fold_ಎಣಿಕೆ,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*ತುದಿ, *bot;
    linenr_T	lnume = lnum + fold_ಎಣಿಕೆ - 1;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		fdc;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		txtcol;
    ಪೂರ್ಣಾಂಕ		off = (ಪೂರ್ಣಾಂಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);
    ಪೂರ್ಣಾಂಕ		ri;

    // Build the fold line:
    // 1. Add the cmdwin_type for the command-line window
    // 2. Add the 'foldcolumn'
    // 3. Add the 'number' or 'relativenumber' column
    // 4. Compose the text
    // 5. Add the text
    // 6. set highlighting for the Visual area an other text
    ಲಂಬಸಾಲು = 0;

    // 1. Add the cmdwin_type for the command-line window
    // Ignores 'rightleft', this window is never right-left.
    ಆದರೆ (wp == cmdwin_win)
    {
	Screen_ಸಾಲುಗಳು[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	ಆದರೆ (enc_utf8)
	    Screen_ಸಾಲುಗಳು_UC[off] = 0;
	++ಲಂಬಸಾಲು;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    ಮಾಡು { \
	ಆದರೆ (wp->w_p_rl) \
	    ಗಾಗಿ (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_ಅಗಲ - (p) - (l)) + ri] = v; \
	 ಇಲ್ಲದಿದ್ದರೆ \
	    ಗಾಗಿ (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } ಆಗ (0)
# ಇಲ್ಲದಿದ್ದರೆ
#  define RL_MEMSET(p, v, l) \
    ಮಾಡು { \
	ಗಾಗಿ (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } ಆಗ (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, ಲಂಬಸಾಲು);
    ಆದರೆ (fdc > 0)
    {
	char_u	*p;
	ಪೂರ್ಣಾಂಕ	i;
	ಪೂರ್ಣಾಂಕ	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	ಗಾಗಿ (i = 0; i < fdc; i++)
	{
	    ಪೂರ್ಣಾಂಕ		ch;

	    ಆದರೆ (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    ಇಲ್ಲದಿದ್ದರೆ
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    ಆದರೆ (wp->w_p_rl)
		idx = off + wp->w_ಅಗಲ - i - 1 - ಲಂಬಸಾಲು;
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		idx = off + ಲಂಬಸಾಲು + i;
	    ಆದರೆ (enc_utf8)
	    {
		ಆದರೆ (ch >= 0x80)
		{
		    Screen_ಸಾಲುಗಳು_UC[idx] = ch;
		    Screen_ಸಾಲುಗಳು_C[0][idx] = 0;
		    Screen_ಸಾಲುಗಳು[idx] = 0x80;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    Screen_ಸಾಲುಗಳು[idx] = ch;
		    Screen_ಸಾಲುಗಳು_UC[idx] = 0;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		Screen_ಸಾಲುಗಳು[idx] = ch;
	}

	RL_MEMSET(ಲಂಬಸಾಲು, HL_ATTR(HLF_FC), fdc);
	ಲಂಬಸಾಲು += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    //ಪಠ್ಯ
    RL_MEMSET(ಲಂಬಸಾಲು, HL_ATTR(HLF_FL), wp->w_ಅಗಲ - ಲಂಬಸಾಲು);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    ಆದರೆ (signcolumn_on(wp))
    {
	len = wp->w_ಅಗಲ - ಲಂಬಸಾಲು;
	ಆದರೆ (len > 0)
	{
	    ಆದರೆ (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    ಆದರೆ (wp->w_p_rl)
		// the line number isn't reversed
		copy_ಪಠ್ಯ_attr(off + wp->w_ಅಗಲ - len - ಲಂಬಸಾಲು,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
		copy_ಪಠ್ಯ_attr(off + ಲಂಬಸಾಲು, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    ಲಂಬಸಾಲು += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    ಆದರೆ (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_ಅಗಲ - ಲಂಬಸಾಲು;
	ಆದರೆ (len > 0)
	{
	    ಪೂರ್ಣಾಂಕ	    w = ಸಂಖ್ಯೆ_ಅಗಲ(wp);
	    ದೀರ್ಘ    ಅಂಕೆ;
	    ಅಕ್ಷರ    *fmt = "%*ld ";

	    ಆದರೆ (len > w + 1)
		len = w + 1;

	    ಆದರೆ (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		ಅಂಕೆ = (ದೀರ್ಘ)lnum;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// 'relativenumber', don't use negative numbers
		ಅಂಕೆ = labs((ದೀರ್ಘ)get_cursor_rel_lnum(wp, lnum));
		ಆದರೆ (ಅಂಕೆ == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    ಅಂಕೆ = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf), fmt, w, ಅಂಕೆ);
# ifdef FEAT_RIGHTLEFT
	    ಆದರೆ (wp->w_p_rl)
		// the line number isn't reversed
		copy_ಪಠ್ಯ_attr(off + wp->w_ಅಗಲ - len - ಲಂಬಸಾಲು, buf, len,
							     HL_ATTR(HLF_FL));
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		copy_ಪಠ್ಯ_attr(off + ಲಂಬಸಾಲು, buf, len, HL_ATTR(HLF_FL));
	    ಲಂಬಸಾಲು += len;
	}
    }

    // 4. Compose the folded-line string with 'foldtext', if set.
    ಪಠ್ಯ = get_foldtext(wp, lnum, lnume, foldinfo, buf);

    txtcol = ಲಂಬಸಾಲು;	// remember where text starts

    // 5. move the text to current_ScreenLine.  Fill up with "fold" from
    //    'fillchars'.
    //    Right-left text is put in columns 0 - number-col, normal text is put
    //    in columns number-col - window-width.
    ಲಂಬಸಾಲು = ಪಠ್ಯ_ಗಮ್ಯ_screenline(wp, ಪಠ್ಯ, ಲಂಬಸಾಲು);

    // Fill the rest of the line with the fold filler
    ಆಗ (ಲಂಬಸಾಲು < wp->w_ಅಗಲ)
    {
	ಪೂರ್ಣಾಂಕ c = wp->w_fill_chars.fold;
	ಪೂರ್ಣಾಂಕ idx = off + ಲಂಬಸಾಲು;

# ifdef FEAT_RIGHTLEFT
	ಆದರೆ (wp->w_p_rl)
	    idx = off + wp->w_ಅಗಲ - 1 - ಲಂಬಸಾಲು;
# endif

	ಆದರೆ (enc_utf8)
	{
	    ಆದರೆ (c >= 0x80)
	    {
		Screen_ಸಾಲುಗಳು_UC[idx] = c;
		Screen_ಸಾಲುಗಳು_C[0][idx] = 0;
		Screen_ಸಾಲುಗಳು[idx] = 0x80; // avoid storing zero
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		Screen_ಸಾಲುಗಳು_UC[idx] = 0;
		Screen_ಸಾಲುಗಳು[idx] = c;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    Screen_ಸಾಲುಗಳು[idx] = c;
	++ಲಂಬಸಾಲು;
    }

    ಆದರೆ (ಪಠ್ಯ != buf)
	vim_free(ಪಠ್ಯ);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && wp->w_ಬಫರ್ == curwin->w_ಬಫರ್)
    {
	ಆದರೆ (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    ತುದಿ = &curwin->w_cursor;
	    bot = &VIsual;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Visual is before curwin->w_cursor
	    ತುದಿ = &VIsual;
	    bot = &curwin->w_cursor;
	}
	ಆದರೆ (lnum >= ತುದಿ->lnum
		&& lnume <= bot->lnum
		&& (VIsual_ಬಹುಲಕ != 'v'
		    || ((lnum > ತುದಿ->lnum
			    || (lnum == ತುದಿ->lnum
				&& ತುದಿ->ಲಂಬಸಾಲು == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->ಲಂಬಸಾಲು - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_ಬಫರ್, lnume))))))
	{
	    ಆದರೆ (VIsual_ಬಹುಲಕ == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		ಆದರೆ (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_ಅಗಲ)
		{
		    ಆದರೆ (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_ಅಗಲ)
			len = wp->w_old_cursor_lcol;
		    ಇಲ್ಲದಿದ್ದರೆ
			len = wp->w_ಅಗಲ - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (ಪೂರ್ಣಾಂಕ)wp->w_old_cursor_fcol);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_ಅಗಲ - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    ಆದರೆ (wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    {
	ಪೂರ್ಣಾಂಕ i = 0;
	ಪೂರ್ಣಾಂಕ j = wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[i];
	ಪೂರ್ಣಾಂಕ old_txtcol = txtcol;

	ಆಗ (j > -1)
	{
	    txtcol += j;
	    ಆದರೆ (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    ಇಲ್ಲದಿದ್ದರೆ
		txtcol -= wp->w_leftcol;
	    ಆದರೆ (txtcol >= 0 && txtcol < wp->w_ಅಗಲ)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    ಆದರೆ (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	ಆದರೆ (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	ಇಲ್ಲದಿದ್ದರೆ
	    txtcol -= wp->w_leftcol;
	ಆದರೆ (txtcol >= 0 && txtcol < wp->w_ಅಗಲ)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_ಸಾಲು(wp, ಅಡ್ಡಸಾಲು + W_WINROW(wp), wp->w_wincol, wp->w_ಅಗಲ, wp->w_ಅಗಲ,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    ಆದರೆ (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
	curwin->w_cline_ಎತ್ತರ = 1;
	curwin->w_cline_folded = true;
	curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
    }

# ifdef FEAT_CONCEAL
    // When the line was not folded w_wrow may have been set, recompute it.
    ಆದರೆ (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_ಸಾಲು(wp))
	curs_columns(TRUE);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

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
    buf_T	*buf = wp->w_ಬಫರ್;
    ಪೂರ್ಣಾಂಕ		type;
    ಪೂರ್ಣಾಂಕ		ತುದಿ_ಕೊನೆ = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    ಪೂರ್ಣಾಂಕ		mid_ಆರಂಭ = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    ಪೂರ್ಣಾಂಕ		mid_ಕೊನೆ = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    ಪೂರ್ಣಾಂಕ		bot_ಆರಂಭ = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    ಪೂರ್ಣಾಂಕ		scrolled_ಕೆಳಕ್ಕೆ = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    ಪೂರ್ಣಾಂಕ		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //ಸಾಲುಗಳು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಂಕ		ತುದಿ_ಗಮ್ಯ_mod = FALSE;    // redraw above mod_top
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    ಪೂರ್ಣಾಂಕ		idx;		// current index in w_lines[]
    ಪೂರ್ಣಾಂಕ		srow;		// starting row of the current line

    ಪೂರ್ಣಾಂಕ		eof = FALSE;	// if TRUE, we hit the end of the file
    ಪೂರ್ಣಾಂಕ		didline = FALSE; // if TRUE, we finished the last line
    ಪೂರ್ಣಾಂಕ		i;
    ದೀರ್ಘ	j;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		old_wrow = wp->w_wrow;
    ಪೂರ್ಣಾಂಕ		old_wcol = wp->w_wcol;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ದೀರ್ಘ	fold_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_ಸಾಲು 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    ಪೂರ್ಣಾಂಕ		did_update = DID_NONE;
    linenr_T	syntax_ಕೊನೆಯ_parsed = 0;		// last parsed text line
#ಆದರೆ_ಅಂತ್ಯ
    linenr_T	mod_ತುದಿ = 0;
    linenr_T	mod_bot = 0;
#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_ಹುಡುಕು_EXTRA)
    ಪೂರ್ಣಾಂಕ		save_got_int;
#ಆದರೆ_ಅಂತ್ಯ
    bool	override_success;

#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    ಆದರೆ (!did_update_one_ಕಿಟಕಿ)
    {
	did_update_one_ಕಿಟಕಿ = TRUE;
# ifdef FEAT_ಹುಡುಕು_EXTRA
	ಆರಂಭ_ಹುಡುಕು_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	ಆದರೆ (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ && clip_isautosel_ನಕ್ಷತ್ರ())
	    clip_update_selection(&clip_ನಕ್ಷತ್ರ);
	ಆದರೆ (clip_plus.ಲಭ್ಯ && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    ಆದರೆ (wp->w_p_cuc)
	validate_virtcol_win(wp);
#ಆದರೆ_ಅಂತ್ಯ

    type = wp->w_redr_type;

    ಆದರೆ (type == UPD_NOT_VALID)
    {
	wp->w_redr_ಸ್ಥಿತಿ = true;
	wp->w_ಸಾಲುಗಳು_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    ಆದರೆ (wp->w_ಎತ್ತರ + WINBAR_ಎತ್ತರ(wp) == 0
	    || (wp->w_ಚೌಕಟ್ಟು->fr_ಎತ್ತರ == wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ
#ಆದರೆ defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#ಆದರೆ_ಅಂತ್ಯ
	       ))
    {
	wp->w_redr_type = 0;
	ಹಿಂತಿರುಗಿ;
    }

    // Window is zero-width: Only need to draw the separator.
    ಆದರೆ (wp->w_ಅಗಲ == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	ಹಿಂತಿರುಗಿ;
    }

    override_success = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    ಆದರೆ (ಪದ_do_update_ಕಿಟಕಿ(wp))
    {
	ಪದ_update_ಕಿಟಕಿ(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	ಆದರೆ (winbar_ಎತ್ತರ(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	ಆದರೆ (override_success)
	    ತೆಗೆ_highlight_overrides();
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    init_ಹುಡುಕು_hl(wp, &screen_ಹುಡುಕು_hl);
#ಆದರೆ_ಅಂತ್ಯ

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    ಆದರೆ (wp->w_skipcol > 0 && wp->w_ಅಗಲ > win_ಲಂಬಸಾಲು_off(wp))
    {
	ಪೂರ್ಣಾಂಕ w = 0;
	ಪೂರ್ಣಾಂಕ width1 = wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp);
	ಪೂರ್ಣಾಂಕ width2 = width1 + win_ಲಂಬಸಾಲು_off2(wp);
	ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು = width1;

	ಆಗ (w < wp->w_skipcol)
	{
	    ಆದರೆ (w > 0)
		ಕೂಡಿಸು = width2;
	    w += ಕೂಡಿಸು;
	}
	ಆದರೆ (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - ಕೂಡಿಸು;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? ಸಂಖ್ಯೆ_ಅಗಲ(wp) : 0;
    ಆದರೆ (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_ತುದಿ = wp->w_redraw_ತುದಿ;
	ಆದರೆ (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    mod_bot = 0;
	ಆದರೆ (buf->b_mod_ಗಣ)
	{
	    ಆದರೆ (mod_ತುದಿ == 0 || mod_ತುದಿ > buf->b_mod_ತುದಿ)
	    {
		mod_ತುದಿ = buf->b_mod_ತುದಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		ಆದರೆ (syntax_present(wp))
		{
		    mod_ತುದಿ -= buf->b_s.b_syn_sync_linebreaks;
		    ಆದರೆ (mod_ತುದಿ < 1)
			mod_ತುದಿ = 1;
		}
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಆದರೆ (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    ಆದರೆ (screen_ಹುಡುಕು_hl.rm.regprog != ಖಾಲಿ
		    && re_multiline(screen_ಹುಡುಕು_hl.rm.regprog))
		ತುದಿ_ಗಮ್ಯ_mod = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		matchitem_T *cur = wp->w_ಹೊಂದಿಕೆ_ತಲೆ;

		ಆಗ (cur != ಖಾಲಿ)
		{
		    ಆದರೆ (cur->mit_ಹೊಂದಿಕೆ.regprog != ಖಾಲಿ
				       && re_multiline(cur->mit_ಹೊಂದಿಕೆ.regprog))
		    {
			ತುದಿ_ಗಮ್ಯ_mod = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		    cur = cur->mit_ಮುಂದಿನ;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	ಆದರೆ (ಹುಡುಕು_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    ಆದರೆ (mod_ತುದಿ == 0 || mod_ತುದಿ > ಹುಡುಕು_hl_has_cursor_lnum)
		mod_ತುದಿ = ಹುಡುಕು_hl_has_cursor_lnum;
	    ಆದರೆ (mod_bot == 0 || mod_bot < ಹುಡುಕು_hl_has_cursor_lnum + 1)
		mod_bot = ಹುಡುಕು_hl_has_cursor_lnum + 1;
	}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಆದರೆ (mod_ತುದಿ != 0 && hasAnyFolding(wp))
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
	    ಗಾಗಿ (i = 0; i < wp->w_ಸಾಲುಗಳು_valid; ++i)
		ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_valid)
		{
		    ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lastlnum < mod_ತುದಿ)
			lnumt = wp->w_ಸಾಲುಗಳು[i].wl_lastlnum + 1;
		    ಆದರೆ (lnumb == MAXLNUM && wp->w_ಸಾಲುಗಳು[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_ಸಾಲುಗಳು[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			ಆದರೆ (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (ಶೂನ್ಯ)hasFoldingWin(wp, mod_ತುದಿ, &mod_ತುದಿ, ಖಾಲಿ, TRUE, ಖಾಲಿ);
	    ಆದರೆ (mod_ತುದಿ > lnumt)
		mod_ತುದಿ = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (ಶೂನ್ಯ)hasFoldingWin(wp, mod_bot, ಖಾಲಿ, &mod_bot, TRUE, ಖಾಲಿ);
	    ++mod_bot;
	    ಆದರೆ (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#ಆದರೆ_ಅಂತ್ಯ

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	ಆದರೆ (mod_ತುದಿ != 0 && mod_ತುದಿ < wp->w_topline)
	{
	    ಆದರೆ (mod_bot > wp->w_topline)
		mod_ತುದಿ = wp->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (syntax_present(wp))
		ತುದಿ_ಕೊನೆ = 1;
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
    wp->w_redraw_ತುದಿ = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಹುಡುಕು_hl_has_cursor_lnum = 0;
#ಆದರೆ_ಅಂತ್ಯ


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    ಆದರೆ (type == UPD_REDRAW_ತುದಿ)
    {
	j = 0;
	ಗಾಗಿ (i = 0; i < wp->w_ಸಾಲುಗಳು_valid; ++i)
	{
	    j += wp->w_ಸಾಲುಗಳು[i].wl_ಗಾತ್ರ;
	    ಆದರೆ (j >= wp->w_upd_ಅಡ್ಡಸಾಲುಗಳು)
	    {
		ತುದಿ_ಕೊನೆ = j;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (ತುದಿ_ಕೊನೆ == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	ಇಲ್ಲದಿದ್ದರೆ
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    ಆದರೆ (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    ಆದರೆ ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	ಆದರೆ (mod_ತುದಿ != 0
		&& wp->w_topline == mod_ತುದಿ
		&& (!wp->w_ಸಾಲುಗಳು[0].wl_valid
		    || wp->w_topline == wp->w_ಸಾಲುಗಳು[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_ಸಾಲುಗಳು[0].wl_valid
		&& (wp->w_topline < wp->w_ಸಾಲುಗಳು[0].wl_lnum
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    || (wp->w_topline == wp->w_ಸಾಲುಗಳು[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#ಆದರೆ_ಅಂತ್ಯ
		   ))
	{
	    // New topline is above old topline: May scroll down.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		ಗಾಗಿ (ln = wp->w_topline; ln < wp->w_ಸಾಲುಗಳು[0].wl_lnum; ++ln)
		{
		    ++j;
		    ಆದರೆ (j >= wp->w_ಎತ್ತರ - 2)
			ನಿಲ್ಲಿಸು;
		    (ಶೂನ್ಯ)hasFoldingWin(wp, ln, ಖಾಲಿ, &ln, TRUE, ಖಾಲಿ);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		j = wp->w_ಸಾಲುಗಳು[0].wl_lnum - wp->w_topline;
	    ಆದರೆ (j < wp->w_ಎತ್ತರ - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_ಸಾಲುಗಳು[0].wl_lnum - 1,
								wp->w_ಎತ್ತರ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		ಆದರೆ (wp->w_ಸಾಲುಗಳು[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_ಸಾಲುಗಳು[0].wl_lnum)
							  - wp->w_old_topfill;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (i < wp->w_ಎತ್ತರ - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    ಆದರೆ (i > 0)
			check_for_delay(FALSE);
		    ಆದರೆ (win_ins_ಸಾಲುಗಳು(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			ಆದರೆ (wp->w_ಸಾಲುಗಳು_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    ತುದಿ_ಕೊನೆ = i;
			    scrolled_ಕೆಳಕ್ಕೆ = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    ಆದರೆ ((wp->w_ಸಾಲುಗಳು_valid += j) > wp->w_ಎತ್ತರ)
				wp->w_ಸಾಲುಗಳು_valid = wp->w_ಎತ್ತರ;
			    ಗಾಗಿ (idx = wp->w_ಸಾಲುಗಳು_valid; idx - j >= 0; idx--)
				wp->w_ಸಾಲುಗಳು[idx] = wp->w_ಸಾಲುಗಳು[idx - j];
			    ಆಗ (idx >= 0)
				wp->w_ಸಾಲುಗಳು[idx--].wl_valid = FALSE;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			mid_ಆರಂಭ = 0;		// redraw all lines
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    mid_ಆರಂಭ = 0;		// redraw all lines
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		mid_ಆರಂಭ = 0;		// redraw all lines
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    ಅಡ್ಡಸಾಲು = 0;
	    ಗಾಗಿ (i = 0; i < wp->w_ಸಾಲುಗಳು_valid && i < ಅಡ್ಡಸಾಲುಗಳು; i++)
	    {
		ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_valid
			&& wp->w_ಸಾಲುಗಳು[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    ನಿಲ್ಲಿಸು;
		}
		ಅಡ್ಡಸಾಲು += wp->w_ಸಾಲುಗಳು[i].wl_ಗಾತ್ರ;
	    }
	    ಆದರೆ (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//ಸಾಲುಗಳು
		mid_ಆರಂಭ = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		ಆದರೆ (wp->w_ಸಾಲುಗಳು[0].wl_lnum == wp->w_topline)
		    ಅಡ್ಡಸಾಲು += wp->w_old_topfill;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಅಡ್ಡಸಾಲು += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		ಅಡ್ಡಸಾಲು -= wp->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (ಅಡ್ಡಸಾಲು > ಅಡ್ಡಸಾಲುಗಳು)  // just in case
		    ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು;
		ಆದರೆ (ಅಡ್ಡಸಾಲು > 0)
		{
		    check_for_delay(FALSE);
		    ಆದರೆ (win_del_ಸಾಲುಗಳು(wp, 0, ಅಡ್ಡಸಾಲು, FALSE, wp == firstwin, 0)
									 == OK)
			bot_ಆರಂಭ = wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲು;
		    ಇಲ್ಲದಿದ್ದರೆ
			mid_ಆರಂಭ = 0;		// redraw all lines
		}
		ಆದರೆ ((ಅಡ್ಡಸಾಲು == 0 || bot_ಆರಂಭ < 999) && wp->w_ಸಾಲುಗಳು_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_ಆರಂಭ = 0;
		    idx = 0;
		    ಗಾಗಿ (;;)
		    {
			wp->w_ಸಾಲುಗಳು[idx] = wp->w_ಸಾಲುಗಳು[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			ಆದರೆ (ಅಡ್ಡಸಾಲು > 0 && bot_ಆರಂಭ + ಅಡ್ಡಸಾಲು
				 + (ಪೂರ್ಣಾಂಕ)wp->w_ಸಾಲುಗಳು[j].wl_ಗಾತ್ರ > wp->w_ಎತ್ತರ)
			{
			    wp->w_ಸಾಲುಗಳು_valid = idx + 1;
			    ನಿಲ್ಲಿಸು;
			}
			bot_ಆರಂಭ += wp->w_ಸಾಲುಗಳು[idx++].wl_ಗಾತ್ರ;

			// stop at the last valid entry in w_lines[].wl_size
			ಆದರೆ (++j >= wp->w_ಸಾಲುಗಳು_valid)
			{
			    wp->w_ಸಾಲುಗಳು_valid = idx;
			    ನಿಲ್ಲಿಸು;
			}
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    ಆದರೆ (wp->w_p_diff && bot_ಆರಂಭ > 0)
			wp->w_ಸಾಲುಗಳು[0].wl_ಗಾತ್ರ = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	ಆದರೆ (mid_ಆರಂಭ == 0)
	    mid_ಕೊನೆ = wp->w_ಎತ್ತರ;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	ಆದರೆ (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_ಆರಂಭ = 0;
	mid_ಕೊನೆ = wp->w_ಎತ್ತರ;
    }

    ಆದರೆ (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_ಆರಂಭ = 0;
	mid_ಕೊನೆ = wp->w_ಎತ್ತರ;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    ಆದರೆ ((VIsual_ಸಕ್ರಿಯ && buf == curwin->w_ಬಫರ್)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, ಗಮ್ಯ;

	ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	{
	    ಆದರೆ (VIsual_ಬಹುಲಕ != wp->w_old_visual_ಬಹುಲಕ
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		ಆದರೆ (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    ಗಮ್ಯ = VIsual.lnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    from = VIsual.lnum;
		    ಗಮ್ಯ = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		ಆದರೆ (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		ಆದರೆ (wp->w_old_cursor_lnum > ಗಮ್ಯ)
		    ಗಮ್ಯ = wp->w_old_cursor_lnum;
		ಆದರೆ (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		ಆದರೆ (wp->w_old_visual_lnum > ಗಮ್ಯ)
		    ಗಮ್ಯ = wp->w_old_visual_lnum;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		ಆದರೆ (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    ಗಮ್ಯ = wp->w_old_cursor_lnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    from = wp->w_old_cursor_lnum;
		    ಗಮ್ಯ = curwin->w_cursor.lnum;
		    ಆದರೆ (from == 0)	// Visual mode just started
			from = ಗಮ್ಯ;
		}

		ಆದರೆ (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.ಲಂಬಸಾಲು != wp->w_old_visual_ಲಂಬಸಾಲು)
		{
		    ಆದರೆ (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    ಆದರೆ (wp->w_old_visual_lnum > ಗಮ್ಯ)
			ಗಮ್ಯ = wp->w_old_visual_lnum;
		    ಆದರೆ (VIsual.lnum < from)
			from = VIsual.lnum;
		    ಆದರೆ (VIsual.lnum > ಗಮ್ಯ)
			ಗಮ್ಯ = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    ಆದರೆ (VIsual_ಬಹುಲಕ == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_ಕೊನೆ_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		ಆದರೆ (curwin->w_curswant == MAXCOL)
		{
		    ಆದರೆ (get_ve_flags() & VE_ಖಂಡ)
		    {
			pos_T	    pos;
			ಪೂರ್ಣಾಂಕ	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			ಗಾಗಿ (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)ml_get_buf_len(wp->w_ಬಫರ್, pos.lnum);
			    getvvcol(wp, &pos, ಖಾಲಿ, ಖಾಲಿ, &t, 0);
			    ಆದರೆ (toc < t)
				toc = t;
			}
			++toc;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			toc = MAXCOL;
		}

		ಆದರೆ (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    ಆದರೆ (from > VIsual.lnum)
			from = VIsual.lnum;
		    ಆದರೆ (ಗಮ್ಯ < VIsual.lnum)
			ಗಮ್ಯ = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Use the line numbers of the old Visual area.
	    ಆದರೆ (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		ಗಮ್ಯ = wp->w_old_visual_lnum;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		from = wp->w_old_visual_lnum;
		ಗಮ್ಯ = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	ಆದರೆ (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	ಆದರೆ (wp->w_valid & VALID_BOTLINE)
	{
	    from = MIN(from, wp->w_botline);
	    ಗಮ್ಯ = MIN(ಗಮ್ಯ, wp->w_botline);
	}

	// Find the minimal part to be updated.
	// Watch out for scrolling that made entries in w_lines[] invalid.
	// E.g., CTRL-U makes the first half of w_lines[] invalid and sets
	// top_end; need to redraw from top_end to the "to" line.
	// A middle mouse click with a Visual selection may change the text
	// above the Visual area and reset wl_valid, do count these for
	// mid_end (in srow).
	ಆದರೆ (mid_ಆರಂಭ > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    ಆದರೆ (scrolled_ಕೆಳಕ್ಕೆ)
		mid_ಆರಂಭ = ತುದಿ_ಕೊನೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		mid_ಆರಂಭ = 0;
	    ಆಗ (lnum < from && idx < wp->w_ಸಾಲುಗಳು_valid)	// find start
	    {
		ಆದರೆ (wp->w_ಸಾಲುಗಳು[idx].wl_valid)
		    mid_ಆರಂಭ += wp->w_ಸಾಲುಗಳು[idx].wl_ಗಾತ್ರ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!scrolled_ಕೆಳಕ್ಕೆ)
		    srow += wp->w_ಸಾಲುಗಳು[idx].wl_ಗಾತ್ರ;
		++idx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		ಆದರೆ (idx < wp->w_ಸಾಲುಗಳು_valid && wp->w_ಸಾಲುಗಳು[idx].wl_valid)
		    lnum = wp->w_ಸಾಲುಗಳು[idx].wl_lnum;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    ++lnum;
	    }
	    srow += mid_ಆರಂಭ;
	    mid_ಕೊನೆ = wp->w_ಎತ್ತರ;
	    ಗಾಗಿ ( ; idx < wp->w_ಸಾಲುಗಳು_valid; ++idx)		// find end
	    {
		ಆದರೆ (wp->w_ಸಾಲುಗಳು[idx].wl_valid
			&& wp->w_ಸಾಲುಗಳು[idx].wl_lnum >= ಗಮ್ಯ + 1)
		{
		    // Only update until first row of this line
		    mid_ಕೊನೆ = srow;
		    ನಿಲ್ಲಿಸು;
		}
		srow += wp->w_ಸಾಲುಗಳು[idx].wl_ಗಾತ್ರ;
	    }
	}
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && buf == curwin->w_ಬಫರ್)
    {
	wp->w_old_visual_ಬಹುಲಕ = VIsual_ಬಹುಲಕ;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_ಲಂಬಸಾಲು = VIsual.ಲಂಬಸಾಲು;
	wp->w_old_curswant = curwin->w_curswant;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wp->w_old_visual_ಬಹುಲಕ = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_ಲಂಬಸಾಲು = 0;
    }

#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_ಹುಡುಕು_EXTRA)
    // reset got_int, otherwise regexp won't work
    save_got_int = got_int;
    got_int = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SYN_ಸಮಯ_ಎಲ್ಲೆ
    // Set the time limit to 'redrawtime'.
    redrawtime_ಎಲ್ಲೆ_ಗಣ = TRUE;
    init_regexp_timeout(p_rdt);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    win_foldinfo.fi_level = 0;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    // Draw the window toolbar, if there is one.
    // TODO: only when needed.
    ಆದರೆ (winbar_ಎತ್ತರ(wp) > 0)
	redraw_win_toolbar(wp);
#ಆದರೆ_ಅಂತ್ಯ

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_ಕ್ಷೇತ್ರ(spv);
    ಆದರೆ (spell_check_ಕಿಟಕಿ(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_ತುದಿ == 0;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    ಅಡ್ಡಸಾಲು = 0;
    srow = 0;
    ಗಾಗಿ (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	ಆದರೆ (ಅಡ್ಡಸಾಲು == wp->w_ಎತ್ತರ)
	{
	    didline = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	// stop updating when hit the end of the file
	ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    eof = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	// Remember the starting row of the line that is going to be dealt
	// with.  It is used further down when the line doesn't fit.
	srow = ಅಡ್ಡಸಾಲು;

	// Update a line when it is in an area that needs updating, when it
	// has changes or w_lines[idx] is invalid.
	// "bot_start" may be halfway a wrapped line after using
	// win_del_lines(), check if the current line includes it.
	// When syntax folding is being used, the saved syntax states will
	// already have been updated, we can't see where the syntax state is
	// the same again, just update until the end of the window.
	ಆದರೆ (ಅಡ್ಡಸಾಲು < ತುದಿ_ಕೊನೆ
		|| (ಅಡ್ಡಸಾಲು >= mid_ಆರಂಭ && ಅಡ್ಡಸಾಲು < mid_ಕೊನೆ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		|| ತುದಿ_ಗಮ್ಯ_mod
#ಆದರೆ_ಅಂತ್ಯ
		|| idx >= wp->w_ಸಾಲುಗಳು_valid
		|| (ಅಡ್ಡಸಾಲು + wp->w_ಸಾಲುಗಳು[idx].wl_ಗಾತ್ರ > bot_ಆರಂಭ)
		|| (mod_ತುದಿ != 0
		    && (lnum == mod_ತುದಿ
			|| (lnum >= mod_ತುದಿ
			    && (lnum < mod_bot
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_ಸಾಲು
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_ಬದಲಾಯಿತು(lnum)))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_ಹೊಂದಿಕೆ_ತಲೆ != ಖಾಲಿ
						    && buf->b_mod_ಗಣ
						    && buf->b_mod_xlines != 0)
#ಆದರೆ_ಅಂತ್ಯ
				))))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		|| (wp->w_p_cul && lnum == wp->w_cursor.lnum)
		|| lnum == wp->w_ಕೊನೆಯ_cursorline
#ಆದರೆ_ಅಂತ್ಯ
				)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	    ಆದರೆ (lnum == mod_ತುದಿ)
		ತುದಿ_ಗಮ್ಯ_mod = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    ಆದರೆ (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_ತುದಿ && lnum < MAX(mod_bot, mod_ತುದಿ + 1)
		    && (!scrolled_ಕೆಳಕ್ಕೆ || ಅಡ್ಡಸಾಲು >= ತುದಿ_ಕೊನೆ))
	    {
		scrolled_for_mod = TRUE;

		ಪೂರ್ಣಾಂಕ		old_cline_ಎತ್ತರ = 0;
		ಪೂರ್ಣಾಂಕ		old_ಅಡ್ಡಸಾಲುಗಳು = 0;
		ಪೂರ್ಣಾಂಕ		new_ಅಡ್ಡಸಾಲುಗಳು = 0;
		ಪೂರ್ಣಾಂಕ		xtra_ಅಡ್ಡಸಾಲುಗಳು;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		ಗಾಗಿ (i = idx; i < wp->w_ಸಾಲುಗಳು_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_valid
			    && wp->w_ಸಾಲುಗಳು[i].wl_lnum == mod_bot)
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_ಎತ್ತರ = wp->w_ಸಾಲುಗಳು[i].wl_ಗಾತ್ರ;
		    old_ಅಡ್ಡಸಾಲುಗಳು += wp->w_ಸಾಲುಗಳು[i].wl_ಗಾತ್ರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		    ಆದರೆ (wp->w_ಸಾಲುಗಳು[i].wl_valid
			    && wp->w_ಸಾಲುಗಳು[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			ಆಗ (i < wp->w_ಸಾಲುಗಳು_valid
						  && !wp->w_ಸಾಲುಗಳು[i].wl_valid)
			    old_ಅಡ್ಡಸಾಲುಗಳು += wp->w_ಸಾಲುಗಳು[i++].wl_ಗಾತ್ರ;
			ನಿಲ್ಲಿಸು;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		}

		ಆದರೆ (i >= wp->w_ಸಾಲುಗಳು_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_ಆರಂಭ = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    ಗಾಗಿ (l = lnum; l < mod_bot; ++l)
		    {
			ಆದರೆ (dollar_vcol >= 0 && wp == curwin &&
				old_cline_ಎತ್ತರ > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_ಅಡ್ಡಸಾಲುಗಳು += old_cline_ಎತ್ತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hasFoldingWin(wp, l, ಖಾಲಿ, &l, TRUE, ಖಾಲಿ))
			    ++new_ಅಡ್ಡಸಾಲುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
			ಇಲ್ಲದಿದ್ದರೆ
			    new_ಅಡ್ಡಸಾಲುಗಳು += plines_correct_topline(wp, l, TRUE);
			++j;
			ಆದರೆ (new_ಅಡ್ಡಸಾಲುಗಳು > wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲು - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_ಅಡ್ಡಸಾಲುಗಳು = 9999;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    xtra_ಅಡ್ಡಸಾಲುಗಳು = new_ಅಡ್ಡಸಾಲುಗಳು - old_ಅಡ್ಡಸಾಲುಗಳು;
		    ಆದರೆ (xtra_ಅಡ್ಡಸಾಲುಗಳು < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			ಆದರೆ (ಅಡ್ಡಸಾಲು - xtra_ಅಡ್ಡಸಾಲುಗಳು >= wp->w_ಎತ್ತರ - 2)
			    mod_bot = MAXLNUM;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    check_for_delay(FALSE);
			    ಆದರೆ (win_del_ಸಾಲುಗಳು(wp, ಅಡ್ಡಸಾಲು,
					  -xtra_ಅಡ್ಡಸಾಲುಗಳು, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    ಇಲ್ಲದಿದ್ದರೆ
				bot_ಆರಂಭ = wp->w_ಎತ್ತರ + xtra_ಅಡ್ಡಸಾಲುಗಳು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xtra_ಅಡ್ಡಸಾಲುಗಳು > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			ಆದರೆ (ಅಡ್ಡಸಾಲು + xtra_ಅಡ್ಡಸಾಲುಗಳು >= wp->w_ಎತ್ತರ - 2)
			    mod_bot = MAXLNUM;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    check_for_delay(FALSE);
			    ಆದರೆ (win_ins_ಸಾಲುಗಳು(wp, ಅಡ್ಡಸಾಲು + old_ಅಡ್ಡಸಾಲುಗಳು,
					     xtra_ಅಡ್ಡಸಾಲುಗಳು, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ತುದಿ_ಕೊನೆ > ಅಡ್ಡಸಾಲು + old_ಅಡ್ಡಸಾಲುಗಳು)
				// Scrolled the part at the top that requires
				// updating down.
				ತುದಿ_ಕೊನೆ += xtra_ಅಡ್ಡಸಾಲುಗಳು;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    ಆದರೆ (mod_bot != MAXLNUM && i != j)
		    {
			ಆದರೆ (j < i)
			{
			    ಪೂರ್ಣಾಂಕ x = ಅಡ್ಡಸಾಲು + new_ಅಡ್ಡಸಾಲುಗಳು;

			    // move entries in w_lines[] upwards
			    ಗಾಗಿ (;;)
			    {
				// stop at last valid entry in w_lines[]
				ಆದರೆ (i >= wp->w_ಸಾಲುಗಳು_valid)
				{
				    wp->w_ಸಾಲುಗಳು_valid = j;
				    ನಿಲ್ಲಿಸು;
				}
				wp->w_ಸಾಲುಗಳು[j] = wp->w_ಸಾಲುಗಳು[i];
				// stop at a line that won't fit
				ಆದರೆ (x + (ಪೂರ್ಣಾಂಕ)wp->w_ಸಾಲುಗಳು[j].wl_ಗಾತ್ರ
							   > wp->w_ಎತ್ತರ)
				{
				    wp->w_ಸಾಲುಗಳು_valid = j + 1;
				    ನಿಲ್ಲಿಸು;
				}
				x += wp->w_ಸಾಲುಗಳು[j++].wl_ಗಾತ್ರ;
				++i;
			    }
			    ಆದರೆ (bot_ಆರಂಭ > x)
				bot_ಆರಂಭ = x;
			}
			ಇಲ್ಲದಿದ್ದರೆ // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_ಸಾಲುಗಳು_valid += j;
			    ಆದರೆ (wp->w_ಸಾಲುಗಳು_valid > wp->w_ಎತ್ತರ)
				wp->w_ಸಾಲುಗಳು_valid = wp->w_ಎತ್ತರ;
			    ಗಾಗಿ (i = wp->w_ಸಾಲುಗಳು_valid; i - j >= idx; --i)
				wp->w_ಸಾಲುಗಳು[i] = wp->w_ಸಾಲುಗಳು[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    ಆಗ (i >= idx)
			    {
				wp->w_ಸಾಲುಗಳು[i].wl_ಗಾತ್ರ = 0;
				wp->w_ಸಾಲುಗಳು[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_ಎಣಿಕೆ = folded_ಎಣಿಕೆ(wp, lnum, &win_foldinfo);
	    ಆದರೆ (fold_ಎಣಿಕೆ != 0)
	    {
		fold_ಸಾಲು(wp, fold_ಎಣಿಕೆ, &win_foldinfo, lnum, ಅಡ್ಡಸಾಲು);
		++ಅಡ್ಡಸಾಲು;
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
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (idx < wp->w_ಸಾಲುಗಳು_valid
		    && wp->w_ಸಾಲುಗಳು[idx].wl_valid
		    && wp->w_ಸಾಲುಗಳು[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_ಸಾಲುಗಳು[idx].wl_ಗಾತ್ರ > wp->w_ಎತ್ತರ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#ಆದರೆ_ಅಂತ್ಯ
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		ಅಡ್ಡಸಾಲು = wp->w_ಎತ್ತರ + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
		prepare_ಹುಡುಕು_hl(wp, &screen_ಹುಡುಕು_hl, lnum);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		ಆದರೆ (syntax_ಕೊನೆಯ_parsed != 0 && syntax_ಕೊನೆಯ_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_ಕೊನೆ_parsing(wp, syntax_ಕೊನೆಯ_parsed + 1);
#ಆದರೆ_ಅಂತ್ಯ

		// Display one line.
		ಅಡ್ಡಸಾಲು = win_ಸಾಲು(wp, lnum, srow, wp->w_ಎತ್ತರ, 0, &spv);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		wp->w_ಸಾಲುಗಳು[idx].wl_folded = FALSE;
		wp->w_ಸಾಲುಗಳು[idx].wl_lastlnum = lnum;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		did_update = DID_ಸಾಲು;
		syntax_ಕೊನೆಯ_parsed = lnum;
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    wp->w_ಸಾಲುಗಳು[idx].wl_lnum = lnum;
	    wp->w_ಸಾಲುಗಳು[idx].wl_valid = TRUE;

	    ಪೂರ್ಣಾಂಕ is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    ಆದರೆ (ಅಡ್ಡಸಾಲು > wp->w_ಎತ್ತರ || ಅಡ್ಡಸಾಲು + wp->w_winrow >= ಅಡ್ಡಸಾಲುಗಳು)
	    {
		// we may need the size of that too long line later on
		ಆದರೆ (dollar_vcol == -1 || !is_curline)
		    wp->w_ಸಾಲುಗಳು[idx].wl_ಗಾತ್ರ = plines_win(wp, lnum, TRUE);
		++idx;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (dollar_vcol == -1 || !is_curline)
		wp->w_ಸಾಲುಗಳು[idx].wl_ಗಾತ್ರ = ಅಡ್ಡಸಾಲು - srow;
	    ++idx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    lnum += fold_ಎಣಿಕೆ + 1;
#ಇಲ್ಲದಿದ್ದರೆ
	    ++lnum;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    ಆದರೆ ((wp->w_p_nu && mod_ತುದಿ != 0 && lnum >= mod_bot
			&& buf->b_mod_ಗಣ && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_ಕೊನೆಯ_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		fold_ಎಣಿಕೆ = folded_ಎಣಿಕೆ(wp, lnum, &win_foldinfo);
		ಆದರೆ (fold_ಎಣಿಕೆ != 0)
		    fold_ಸಾಲು(wp, fold_ಎಣಿಕೆ, &win_foldinfo, lnum, ಅಡ್ಡಸಾಲು);
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    (ಶೂನ್ಯ)win_ಸಾಲು(wp, lnum, srow, wp->w_ಎತ್ತರ,
					       wp->w_ಸಾಲುಗಳು[idx].wl_ಗಾತ್ರ, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    ಅಡ್ಡಸಾಲು += wp->w_ಸಾಲುಗಳು[idx++].wl_ಗಾತ್ರ;
	    ಆದರೆ (ಅಡ್ಡಸಾಲು > wp->w_ಎತ್ತರ)	// past end of screen
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    lnum = wp->w_ಸಾಲುಗಳು[idx - 1].wl_lastlnum + 1;
#ಇಲ್ಲದಿದ್ದರೆ
	    ++lnum;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    did_update = DID_NONE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    eof = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	ಆದರೆ (idx >= ಅಡ್ಡಸಾಲುಗಳು)
	    ನಿಲ್ಲಿಸು;
    }

    // End of loop over all window lines.

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Now that the window has been redrawn with the old and new cursor line,
    // update w_last_cursorline.
    wp->w_ಕೊನೆಯ_cursorline = wp->w_p_cul ? wp->w_cursor.lnum : 0;
#ಆದರೆ_ಅಂತ್ಯ
    wp->w_ಕೊನೆಯ_cursor_lnum_rnu = wp->w_p_rnu ? wp->w_cursor.lnum : 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VTP
    // Rewrite the character at the end of the screen line.
    // See the version that was fixed.
    ಆದರೆ (use_vtp() && get_conpty_fix_type() < 1)
    {
	ಪೂರ್ಣಾಂಕ k;

	ಗಾಗಿ (k = 0; k < ಅಡ್ಡಸಾಲುಗಳು; ++k)
	    ಆದರೆ (enc_utf8)
		ಆದರೆ ((*mb_off2cells)(ಸಾಲು_ಸ್ಥಾನಾಂತರ[k] + topframe->fr_ಅಗಲ - 2,
					   ಸಾಲು_ಸ್ಥಾನಾಂತರ[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_ಅಗಲ - 2, 1, 2,
			    FALSE, FALSE);
		ಇಲ್ಲದಿದ್ದರೆ
		    screen_draw_rectangle(k, topframe->fr_ಅಗಲ - 1, 1, 1,
			    FALSE, FALSE);
	    ಇಲ್ಲದಿದ್ದರೆ
		screen_char(ಸಾಲು_ಸ್ಥಾನಾಂತರ[k] + topframe->fr_ಅಗಲ - 1, k,
			cmdline_ಅಗಲ - 1);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (idx > wp->w_ಸಾಲುಗಳು_valid)
	wp->w_ಸಾಲುಗಳು_valid = idx;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    ಆದರೆ (syntax_ಕೊನೆಯ_parsed != 0 && syntax_present(wp))
	syntax_ಕೊನೆ_parsing(wp, syntax_ಕೊನೆಯ_parsed + 1);
#ಆದರೆ_ಅಂತ್ಯ

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    wp->w_filler_ಅಡ್ಡಸಾಲುಗಳು = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!eof && !didline)
    {
	ಆದರೆ (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (diff_check_fill(wp, lnum) >= wp->w_ಎತ್ತರ - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_ಅಡ್ಡಸಾಲುಗಳು = wp->w_ಎತ್ತರ - srow;
	}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    ಪೂರ್ಣಾಂಕ		scr_ಅಡ್ಡಸಾಲು = W_WINROW(wp) + wp->w_ಎತ್ತರ - 1;
	    ಪೂರ್ಣಾಂಕ		ಚಿಹ್ನೆ  = wp->w_fill_chars.lastline;
	    ಪೂರ್ಣಾಂಕ		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(ಚಿಹ್ನೆ, &fillbuf[0]);
	    mb_char2bytes(ಚಿಹ್ನೆ, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_ಅಗಲ > 2 ? 2 : wp->w_ಅಗಲ) * charlen,
			    scr_ಅಡ್ಡಸಾಲು, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_ಅಡ್ಡಸಾಲು, scr_ಅಡ್ಡಸಾಲು + 1,
		    (ಪೂರ್ಣಾಂಕ)wp->w_wincol + 2, (ಪೂರ್ಣಾಂಕ)W_ENDCOL(wp),
		    ಚಿಹ್ನೆ, ' ', HL_ATTR(HLF_AT));
	    ಗಣ_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು(wp, srow);
	    wp->w_botline = lnum;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)W_ENDCOL(wp) - 3;
	    ಪೂರ್ಣಾಂಕ ಚಿಹ್ನೆ    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_ಎತ್ತರ - 1,
		    W_WINROW(wp) + wp->w_ಎತ್ತರ,
		    (ಆರಂಭ_ಲಂಬಸಾಲು < wp->w_wincol ? wp->w_wincol : ಆರಂಭ_ಲಂಬಸಾಲು),
		    (ಪೂರ್ಣಾಂಕ)W_ENDCOL(wp),
		    ಚಿಹ್ನೆ, ಚಿಹ್ನೆ, HL_ATTR(HLF_AT));
	    ಗಣ_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು(wp, srow);
	    wp->w_botline = lnum;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    win_draw_ಕೊನೆ(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_ಎತ್ತರ, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	draw_vsep_win(wp, ಅಡ್ಡಸಾಲು);
	ಆದರೆ (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    ಆದರೆ (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		ಆದರೆ (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		ಇಲ್ಲದಿದ್ದರೆ
		    i = wp->w_fill_chars.diff;
		ಆದರೆ (ಅಡ್ಡಸಾಲು + j > wp->w_ಎತ್ತರ)
		    j = wp->w_ಎತ್ತರ - ಅಡ್ಡಸಾಲು;
		win_draw_ಕೊನೆ(wp, i, i, TRUE, ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + (ಪೂರ್ಣಾಂಕ)j, HLF_DED);
		ಅಡ್ಡಸಾಲು += j;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	ಆದರೆ (WIN_IS_POPUP(wp))
	    win_draw_ಕೊನೆ(wp, ' ', ' ', FALSE, ಅಡ್ಡಸಾಲು, wp->w_ಎತ್ತರ, HLF_AT);
	ಇಲ್ಲದಿದ್ದರೆ
	    win_draw_ಕೊನೆ(wp, wp->w_fill_chars.eob, ' ', FALSE,
						   ಅಡ್ಡಸಾಲು, wp->w_ಎತ್ತರ, HLF_EOB);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SYN_ಸಮಯ_ಎಲ್ಲೆ
    disable_regexp_timeout();
    redrawtime_ಎಲ್ಲೆ_ಗಣ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // Reset the type of redrawing required, the window has been updated.
    wp->w_redr_type = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    wp->w_old_topfill = wp->w_topfill;
    wp->w_old_botfill = wp->w_botfill;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (dollar_vcol == -1 || wp != curwin)
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
	ಆದರೆ (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#ಆದರೆ defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    ಪೂರ್ಣಾಂಕ		new_wcol = wp->w_wcol;
#ಆದರೆ_ಅಂತ್ಯ
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#ಆದರೆ defined(FEAT_CONCEAL)
	    ಆದರೆ (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
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
		ಆದರೆ (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		ಇಲ್ಲದಿದ್ದರೆ
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    ಆದರೆ (wp->w_redr_type != 0)
	    {
		// Don't update for changes in buffer again.
		bool b = curbuf->b_mod_ಗಣ;
		curbuf->b_mod_ಗಣ = false;
		j = curbuf->b_mod_xlines;
		curbuf->b_mod_xlines = 0;
		curs_columns(TRUE);
		win_update(curwin);
		curbuf->b_mod_ಗಣ = b;
		curbuf->b_mod_xlines = j;
	    }
	    // Other windows might have w_redr_type raised in update_topline().
	    must_redraw = 0;
	    FOR_ALL_WINDOWS(wwp)
		ಆದರೆ (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_ಹುಡುಕು_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    ಆದರೆ (!got_int)
	got_int = save_got_int;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (override_success)
	ತೆಗೆ_highlight_overrides();
}

#ಆದರೆ defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
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
    ಆದರೆ (gui.ಒಳ_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_ಹುಡುಕು_EXTRA
    ಆರಂಭ_ಹುಡುಕು_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_ಮುಖವಾಡ(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_ಮುಕ್ತಾಯ(ಶೂನ್ಯ)
{
    ಆದರೆ (redraw_cmdline || redraw_ಬಹುಲಕ)
	showmode();

# ifdef FEAT_ಹುಡುಕು_EXTRA
    ಕೊನೆ_ಹುಡುಕು_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    ಆದರೆ (gui.ಒಳ_use)
    {
	ಹೊರಹೋಗು_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_NETBEANS_INTG)
    ಶೂನ್ಯ
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_ಸಾಲು_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    ಆದರೆ (!doit || updating_screen
	    || State == ಬಹುಲಕ_ASKMORE || State == ಬಹುಲಕ_HITRETURN
	    || ಸಂದೇಶ_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	ಹಿಂತಿರುಗಿ;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (wp->w_redr_type != 0)
	    win_update(wp);
	ಆದರೆ (wp->w_redr_ಸ್ಥಿತಿ)
	    win_redr_ಸ್ಥಿತಿ(wp, FALSE);
    }

# ಆದರೆ defined(FEAT_TABPANEL)
    ಆದರೆ (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_ಮುಕ್ತಾಯ();
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    ಶೂನ್ಯ
update_ಕಿಟಕಿ(win_T *wp)
{
    // return if already busy updating
    ಆದರೆ (updating_screen)
	ಹಿಂತಿರುಗಿ;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    ಆದರೆ (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ && clip_isautosel_ನಕ್ಷತ್ರ())
	clip_update_selection(&clip_ನಕ್ಷತ್ರ);
    ಆದರೆ (clip_plus.ಲಭ್ಯ && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    ಆದರೆ (redraw_tabline)
	draw_tabline();

# ಆದರೆ defined(FEAT_TABPANEL)
    ಆದರೆ (redraw_tabpanel)
	draw_tabpanel();
# endif

    ಆದರೆ (wp->w_redr_ಸ್ಥಿತಿ || p_ru
# ifdef FEAT_STL_OPT
	    || *p_stl != NUL || *wp->w_p_stl != NUL
# endif
	    )
	win_redr_ಸ್ಥಿತಿ(wp, FALSE);

# ifdef FEAT_PROP_POPUP
    // Display popup windows on top of everything.
    update_popups(win_update);
# endif

    update_ಮುಕ್ತಾಯ();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Redraw as soon as possible.  When the command line is not scrolled redraw
 * right away and restore what was on the command line.
 * Return a code indicating what happened.
 */
    ಪೂರ್ಣಾಂಕ
redraw_asap(ಪೂರ್ಣಾಂಕ type)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲುಗಳು_2;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = screen_Columns;
    ಪೂರ್ಣಾಂಕ		r;
    ಪೂರ್ಣಾಂಕ		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    ಪೂರ್ಣಾಂಕ		i;
    u8char_T	*screenlineUC = ಖಾಲಿ;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = ಖಾಲಿ;	// copy from ScreenLines2[]

    redraw_later(type);
    ಆದರೆ (ಸಂದೇಶ_scrolled
	    || (State != ಬಹುಲಕ_ಸಾಮಾನ್ಯ && State != ಬಹುಲಕ_ಸಾಮಾನ್ಯ_BUSY)
	    || exiting)
	ಹಿಂತಿರುಗಿ ret;

    // Allocate space to save the text displayed in the command line area.
    ಅಡ್ಡಸಾಲುಗಳು_2 = screen_ಅಡ್ಡಸಾಲುಗಳು - cmdline_ಅಡ್ಡಸಾಲು;
    screenline = LALLOC_MULT(schar_T, ಅಡ್ಡಸಾಲುಗಳು_2 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    screenattr = LALLOC_MULT(sattr_T, ಅಡ್ಡಸಾಲುಗಳು_2 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    ಆದರೆ (screenline == ಖಾಲಿ || screenattr == ಖಾಲಿ)
	ret = 2;
    ಆದರೆ (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, ಅಡ್ಡಸಾಲುಗಳು_2 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	ಆದರೆ (screenlineUC == ಖಾಲಿ)
	    ret = 2;
	ಗಾಗಿ (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, ಅಡ್ಡಸಾಲುಗಳು_2 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	    ಆದರೆ (screenlineC[i] == ಖಾಲಿ)
		ret = 2;
	}
    }
    ಆದರೆ (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, ಅಡ್ಡಸಾಲುಗಳು_2 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	ಆದರೆ (screenline2 == ಖಾಲಿ)
	    ret = 2;
    }

    ಆದರೆ (ret != 2)
    {
	// Save the text displayed in the command line area.
	ಗಾಗಿ (r = 0; r < ಅಡ್ಡಸಾಲುಗಳು_2; ++r)
	{
	    mch_memmove(screenline + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
			Screen_ಸಾಲುಗಳು + ಸಾಲು_ಸ್ಥಾನಾಂತರ[cmdline_ಅಡ್ಡಸಾಲು + r],
			(size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(schar_T));
	    mch_memmove(screenattr + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
			ScreenAttrs + ಸಾಲು_ಸ್ಥಾನಾಂತರ[cmdline_ಅಡ್ಡಸಾಲು + r],
			(size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(sattr_T));
	    ಆದರೆ (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
			    Screen_ಸಾಲುಗಳು_UC + ಸಾಲು_ಸ್ಥಾನಾಂತರ[cmdline_ಅಡ್ಡಸಾಲು + r],
			    (size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(u8char_T));
		ಗಾಗಿ (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
				Screen_ಸಾಲುಗಳು_C[i] + ಸಾಲು_ಸ್ಥಾನಾಂತರ[cmdline_ಅಡ್ಡಸಾಲು + r],
				(size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(u8char_T));
	    }
	    ಆದರೆ (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
			    ScreenLines2 + ಸಾಲು_ಸ್ಥಾನಾಂತರ[cmdline_ಅಡ್ಡಸಾಲು + r],
			    (size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(schar_T));
	}

	update_screen(0);
	ret = 3;

	ಆದರೆ (must_redraw == 0)
	{
	    ಪೂರ್ಣಾಂಕ	off = (ಪೂರ್ಣಾಂಕ)(current_Screen_ಸಾಲು - Screen_ಸಾಲುಗಳು);

	    // Restore the text displayed in the command line area.
	    ಗಾಗಿ (r = 0; r < ಅಡ್ಡಸಾಲುಗಳು_2; ++r)
	    {
		mch_memmove(current_Screen_ಸಾಲು,
			    screenline + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
			    (size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
			    (size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(sattr_T));
		ಆದರೆ (enc_utf8)
		{
		    mch_memmove(Screen_ಸಾಲುಗಳು_UC + off,
				screenlineUC + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
				(size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(u8char_T));
		    ಗಾಗಿ (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_ಸಾಲುಗಳು_C[i] + off,
				    screenlineC[i] + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
				    (size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(u8char_T));
		}
		ಆದರೆ (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
				(size_t)ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * ಗಾತ್ರ(schar_T));
		screen_ಸಾಲು(curwin, cmdline_ಅಡ್ಡಸಾಲು + r, 0, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    ಆದರೆ (enc_utf8)
    {
	vim_free(screenlineUC);
	ಗಾಗಿ (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    ಆದರೆ (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_ಸಂದೇಶ();

    setcursor();

    ಹಿಂತಿರುಗಿ ret;
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
redraw_after_callback(ಪೂರ್ಣಾಂಕ call_update_screen, ಪೂರ್ಣಾಂಕ do_ಸಂದೇಶ_2)
{
    ++redrawing_for_callback;

    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_ENABLE);

    ಆದರೆ (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_ASKMORE
	    || State == ಬಹುಲಕ_SETWSIZE || State == ಬಹುಲಕ_EXTERNCMD
	    || State == ಬಹುಲಕ_CONFIRM || exmode_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (do_ಸಂದೇಶ_2)
	    repeat_ಸಂದೇಶ();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
    {
	ಆದರೆ (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	ಆದರೆ (cmdline_ಅಡ್ಡಸಾಲು > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    ಆದರೆ (ಸಂದೇಶ_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	ಆದರೆ (ಸಂದೇಶ_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    ಸಂದೇಶ_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_DISABLE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	ಹೊರಹೋಗು_flush_cursor(FALSE, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಹೊರಹೋಗು_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    ಶೂನ್ಯ
redraw_later(ಪೂರ್ಣಾಂಕ type)
{
    redraw_win_later(curwin, type);
}

    ಶೂನ್ಯ
redraw_win_later(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		type)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    ಆದರೆ (inside_redraw_on_ಆರಂಭ_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	ಆದರೆ (type >= UPD_NOT_VALID)
	    wp->w_ಸಾಲುಗಳು_valid = 0;
	ಆದರೆ (must_redraw < type)	// must_redraw is the maximum of all windows
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
redraw_all_later(ಪೂರ್ಣಾಂಕ type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    ಗಣ_must_redraw(type);
}

#ಆದರೆ 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    ಶೂನ್ಯ
redraw_all_windows_later(ಪೂರ್ಣಾಂಕ type)
{
    redraw_all_later(type);
# ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set "must_redraw" to "type" unless it already has a higher value
 * or it is currently not allowed.
 */
    ಶೂನ್ಯ
ಗಣ_must_redraw(ಪೂರ್ಣಾಂಕ type)
{
    ಆದರೆ (!redraw_not_allowed &&
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    !inside_redraw_on_ಆರಂಭ_cb &&
#ಆದರೆ_ಅಂತ್ಯ
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    ಶೂನ್ಯ
redraw_curbuf_later(ಪೂರ್ಣಾಂಕ type)
{
    redraw_buf_later(curbuf, type);
}

    ಶೂನ್ಯ
redraw_buf_later(buf_T *buf, ಪೂರ್ಣಾಂಕ type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (wp->w_ಬಫರ್ == buf)
	    redraw_win_later(wp, type);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ (wp->w_ಬಫರ್ == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_ಒಳ_TAB(curtab, wp)
	ಆದರೆ (wp->w_ಬಫರ್ == buf)
	    redraw_win_later(wp, type);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    ಆದರೆ (curwin->w_ಬಫರ್ == buf)
	redraw_win_later(curwin, type);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_SIGNS)
    ಶೂನ್ಯ
redraw_buf_ಸಾಲು_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_ಬಫರ್ == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
    ಶೂನ್ಯ
redraw_buf_and_ಸ್ಥಿತಿ_later(buf_T *buf, ಪೂರ್ಣಾಂಕ type)
{
    win_T	*wp;

    ಆದರೆ (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	ಹಿಂತಿರುಗಿ;
    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (wp->w_ಬಫರ್ == buf)
	{
	    redraw_win_later(wp, type);
	    wp->w_redr_ಸ್ಥಿತಿ = true;
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * mark the ruler for redraw when the last window has no status line and the
 * ruler takes its place in the last screen line; showmode() draws it
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ruler_redraw_lastwin(ಶೂನ್ಯ)
{
    ಆದರೆ (p_ru && lastwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    ಶೂನ್ಯ
ಸ್ಥಿತಿ_redraw_all(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ)
	{
	    wp->w_redr_ಸ್ಥಿತಿ = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    ಶೂನ್ಯ
ಸ್ಥಿತಿ_redraw_curbuf(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ != 0 && wp->w_ಬಫರ್ == curbuf)
	{
	    wp->w_redr_ಸ್ಥಿತಿ = true;
	    redraw_later(UPD_VALID);
	}
    ಆದರೆ (lastwin->w_ಬಫರ್ == curbuf)
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
	ಆದರೆ (wp->w_redr_ಸ್ಥಿತಿ)
	{
	    bool ret = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_ಸ್ಥಿತಿ(wp, FALSE);
	    ಆದರೆ (ret)
		ತೆಗೆ_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    ಆದರೆ (redraw_tabline)
	draw_tabline();

#ಆದರೆ defined(FEAT_TABPANEL)
    ಆದರೆ (redraw_tabpanel)
	draw_tabpanel();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    ಶೂನ್ಯ
win_redraw_ಕೊನೆಯ_ಸ್ಥಿತಿ(ಚೌಕಟ್ಟು_T *frp)
{
    ಆದರೆ (frp->fr_layout == FR_ಎಲೆ)
	frp->fr_win->w_redr_ಸ್ಥಿತಿ = true;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (frp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
    {
	FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, frp->fr_ಮಗು)
	    win_redraw_ಕೊನೆಯ_ಸ್ಥಿತಿ(frp);
    }
    ಇಲ್ಲದಿದ್ದರೆ // frp->fr_layout == FR_COL
    {
	frp = frp->fr_ಮಗು;
	ಆಗ (frp->fr_ಮುಂದಿನ != ಖಾಲಿ)
	    frp = frp->fr_ಮುಂದಿನ;
	win_redraw_ಕೊನೆಯ_ಸ್ಥಿತಿ(frp);
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
    linenr_T	ಮೊದಲ,
    linenr_T	ಕೊನೆಯ)
{
    ಆದರೆ (ಕೊನೆಯ >= wp->w_topline && ಮೊದಲ < wp->w_botline)
    {
	ಆದರೆ (wp->w_redraw_ತುದಿ == 0 || wp->w_redraw_ತುದಿ > ಮೊದಲ)
	    wp->w_redraw_ತುದಿ = ಮೊದಲ;
	ಆದರೆ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < ಕೊನೆಯ)
	    wp->w_redraw_bot = ಕೊನೆಯ;
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
    linenr_T	ಮೊದಲ,
    linenr_T	ಕೊನೆಯ)
{
    ಆದರೆ (ಕೊನೆಯ >= wp->w_topline && ಮೊದಲ < wp->w_botline)
    {
	ಆದರೆ (wp->w_redraw_ತುದಿ == 0 || wp->w_redraw_ತುದಿ > ಮೊದಲ)
	    wp->w_redraw_ತುದಿ = ಮೊದಲ;
	ಆದರೆ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < ಕೊನೆಯ)
	    wp->w_redraw_bot = ಕೊನೆಯ;
	ಆದರೆ (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ bool redraw_cb_ಒಳ_ಪ್ರಗತಿ = false;

    ಶೂನ್ಯ
f_redraw_listener_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		id;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (redraw_cb_ಒಳ_ಪ್ರಗತಿ)
    {
	emsg(_(e_cannot_ಕೂಡಿಸು_redraw_listener_ಒಳ_listener_callback));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (check_for_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    ಆದರೆ (rln == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    ಆದರೆ (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	ಆದರೆ (cb.cb_name == ಖಾಲಿ)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    ಹಿಂತಿರುಗಿ;
	}
	copy_callback(&rln->rl_callbacks.on_ಆರಂಭ, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    ಆದರೆ (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	ಆದರೆ (cb.cb_name == ಖಾಲಿ)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_ಆರಂಭ);
	    vim_free(rln);
	    ಹಿಂತಿರುಗಿ;
	}
	copy_callback(&rln->rl_callbacks.on_ಕೊನೆ, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    ಆದರೆ (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	ಹಿಂತಿರುಗಿ;
    }

    rln->rl_ಮುಂದಿನ = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    rettv->vval.v_ಸಂಖ್ಯೆ = id;

    ಹಿಂತಿರುಗಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_ಆರಂಭ);
    free_callback(&rln->rl_callbacks.on_ಕೊನೆ);

    vim_free(rln);
}


    ಸ್ಥಿರ ಶೂನ್ಯ
redraw_listener_cleanup(ಶೂನ್ಯ)
{
    ಗಾಗಿ (redraw_listener_T *rln = redraw_listeners; rln != ಖಾಲಿ;)
    {
	redraw_listener_T *ಮುಂದಿನ = rln->rl_ಮುಂದಿನ;
	ಆದರೆ (rln->rl_id == 0)
	{
	    ಆದರೆ (redraw_listeners == rln)
		redraw_listeners = rln->rl_ಮುಂದಿನ;
	    redraw_listener_free(rln);
	}
	rln = ಮುಂದಿನ;
    }
}

/*
 * Return the redraw listener struct with the specified id. Returns NULL if not
 * found.
 */
    ಸ್ಥಿರ redraw_listener_T *
get_redraw_listener(ಪೂರ್ಣಾಂಕ id)
{
    ಗಾಗಿ (redraw_listener_T *rln = redraw_listeners; rln != ಖಾಲಿ; rln = rln->rl_ಮುಂದಿನ)
	ಆದರೆ (rln->rl_id == id)
	    ಹಿಂತಿರುಗಿ rln;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಶೂನ್ಯ
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ			id;
    redraw_listener_T	*rln;

    ಆದರೆ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = argvars[0].vval.v_ಸಂಖ್ಯೆ;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
    ಆದರೆ (rln == ಖಾಲಿ)
    {
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	ಹಿಂತಿರುಗಿ;
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
invoke_redraw_listener_ಆರಂಭ_or_ಕೊನೆ(bool ಆರಂಭ)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    ಆದರೆ (ಆರಂಭ)
	inside_redraw_on_ಆರಂಭ_cb = true;

    redraw_cb_ಒಳ_ಪ್ರಗತಿ = true;
    ಗಾಗಿ (redraw_listener_T *rln = redraw_listeners; rln != ಖಾಲಿ; rln = rln->rl_ಮುಂದಿನ)
    {
	ಆದರೆ (rln->rl_id == 0)
	    // Listener has been removed, skip
	    ಮುಂದುವರಿ;
	ಆದರೆ (ಆರಂಭ && rln->rl_callbacks.on_ಆರಂಭ.cb_name != ಖಾಲಿ)
	{
	    call_callback(&rln->rl_callbacks.on_ಆರಂಭ, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rln->rl_callbacks.on_ಕೊನೆ.cb_name != ಖಾಲಿ)
	{
	    call_callback(&rln->rl_callbacks.on_ಕೊನೆ, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_ಒಳ_ಪ್ರಗತಿ = false;

    ಆದರೆ (ಆರಂಭ)
	inside_redraw_on_ಆರಂಭ_cb = false;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
