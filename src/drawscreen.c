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

#अन्तर्भाव "vim.h"

स्थिर शून्य win_update(win_T *wp);
#ifdef FEAT_STL_OPT
स्थिर शून्य redraw_custom_statusline(win_T *wp);
#endif
स्थिर शून्य borrow_stl_vsep_hl(शून्य);
#यदि defined(FEAT_खोज_EXTRA) || defined(FEAT_CLIPBOARD)
स्थिर पूर्णाङ्क  did_update_one_window;
#endif

#ifdef FEAT_EVAL
स्थिर शून्य redraw_listener_cleanup(शून्य);
स्थिर शून्य invoke_redraw_listener_start_or_end(bool start);
#endif

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    पूर्णाङ्क
update_screen(पूर्णाङ्क type_arg)
{
    पूर्णाङ्क		type = type_arg;
    win_T	*wp;
    स्थिर पूर्णाङ्क	did_intro = FALSE;
#ifdef FEAT_GUI
    पूर्णाङ्क		did_one = FALSE;
    पूर्णाङ्क		did_undraw = FALSE;
    पूर्णाङ्क		gui_cursor_col = 0;
    पूर्णाङ्क		gui_cursor_row = 0;
#endif
    पूर्णाङ्क		no_update = FALSE;
    पूर्णाङ्क		save_pum_will_redraw = pum_will_redraw;
#ifdef FEAT_PROP_POPUP
    पूर्णाङ्क		did_redraw_window = FALSE;
#endif
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    यदि (!screen_valid(TRUE))
	फर्काऊ FAIL;

    यदि (type == UPD_VALID_NO_UPDATE)
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
    यदि (need_diff_redraw)
	diff_redraw(TRUE);
#endif

    यदि (must_redraw)
    {
	यदि (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    यदि (curwin->w_पङ्क्तिहरू_valid == 0 && type < UPD_NOT_VALID
#ifdef FEAT_TERMINAL
	    && !term_do_update_window(curwin)
#endif
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    यदि (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	यदि (type > UPD_INVERTED_ALL)
	    curwin->w_पङ्क्तिहरू_valid = 0;	// don't use w_lines[].wl_size now
	फर्काऊ FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    पूर्णाङ्क hid_cursor = !sync_output_active();
    यदि (hid_cursor)
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
    यदि (no_update)
	++no_win_do_पङ्क्तिहरू_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    यदि (सन्देश_scrolled)
    {
	clear_cmdline = TRUE;
	यदि (type != UPD_CLEAR)
	{
	    यदि (सन्देश_scrolled > Rows - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    अन्यथा
	    {
		check_for_delay(FALSE);
		यदि (screen_ins_पङ्क्तिहरू(0, 0, सन्देश_scrolled, (पूर्णाङ्क)Rows, 0, रिक्त)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    यदि (wp->w_winrow < सन्देश_scrolled)
		    {
			यदि (W_WINROW(wp) + wp->w_height > सन्देश_scrolled
				&& wp->w_redr_type < UPD_REDRAW_शीर्ष
				&& wp->w_पङ्क्तिहरू_valid > 0
				&& wp->w_topline == wp->w_पङ्क्तिहरू[0].wl_lnum)
			{
			    wp->w_upd_rows = सन्देश_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_शीर्ष;
			}
			अन्यथा
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    यदि (W_WINROW(wp) + wp->w_height
					 + wp->w_status_height <= सन्देश_scrolled)
				wp->w_redr_status = true;
			}
		    }
		}
		यदि (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#यदि defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#endif
	}
	सन्देश_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    यदि (need_highlight_changed)
	highlight_changed();

    यदि (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    यदि (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    यदि (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? सङ्ख्या_width(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#endif

#ifdef FEAT_EVAL
    invoke_redraw_listener_start_or_end(true);
#endif

    // Only start redrawing if there is really something to do.
    यदि (type == UPD_INVERTED)
	update_curswant();
    यदि (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_पङ्क्तिहरू[0].wl_valid
#ifdef FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#endif
		    && curwin->w_topline == curwin->w_पङ्क्तिहरू[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_active
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_mode == VIsual_mode
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    यदि (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#यदि defined(FEAT_TABPANEL)
    यदि (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#endif

#ifdef FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	यदि (wp->w_buffer->b_mod_set)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    लागि (wwp = firstwin; wwp != wp; wwp = wwp->w_next)
		यदि (wwp->w_buffer == wp->w_buffer)
		    रोक;
	    यदि (wwp == wp && syntax_present(wp))
		syn_थाक_apply_changes(wp->w_buffer);
	}
    }
#endif

    यदि (pum_redraw_in_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#यदि defined(FEAT_खोज_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_window = FALSE;
#endif
#ifdef FEAT_खोज_EXTRA
    screen_खोज_hl.rm.regprog = रिक्त;
#endif
    FOR_ALL_WINDOWS(wp)
    {
	override_success = थप_highlight_overrides(wp->w_hl, wp->w_hl_len);

	यदि (wp->w_redr_type != 0)
	{
	    cursor_off();
#ifdef FEAT_PROP_POPUP
	    did_redraw_window = TRUE;
#endif
#ifdef FEAT_GUI
	    यदि (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		यदि (gui.in_use && (wp == curwin || cursor_is_sleeping()))
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
	यदि (wp->w_redr_status)
	{
	    cursor_off();
	    win_redr_status(wp, TRUE); // any popup menu will be redrawn below
	}

	यदि (override_success)
	    झिक_highlight_overrides();

    }
#यदि defined(FEAT_खोज_EXTRA)
    end_खोज_hl();
#endif

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    यदि (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    यदि (wp->w_vsep_width > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_buffer->b_mod_set = false;

#ifdef FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    यदि (did_redraw_window || popup_need_redraw())
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
    यदि (clear_cmdline || redraw_cmdline || redraw_mode)
	showmode();

    यदि (no_update)
	--no_win_do_पङ्क्तिहरू_ins;

    // May put up an introductory message when not editing a file
    यदि (!did_intro)
	maybe_intro_message();
    did_intro = TRUE;

#ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    यदि (gui.in_use)
    {
	यदि (did_undraw && !gui_mch_is_blink_off())
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
	अन्यथा
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
    यदि (hid_cursor)
	cursor_on();

    फर्काऊ OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    पूर्णाङ्क
statusline_row(win_T *wp)
{
#यदि defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    यदि (wp->w_frame->fr_height == wp->w_status_height && !popup_is_popup(wp))
	फर्काऊ wp->w_winrow;
#endif
    फर्काऊ W_WINROW(wp) + wp->w_height;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    शून्य
win_redr_status(win_T *wp, पूर्णाङ्क ignore_pum UNUSED)
{
    पूर्णाङ्क		row;
    पूर्णाङ्क		fillchar;
    पूर्णाङ्क		attr;
    पूर्णाङ्क		i;
    स्थिर पूर्णाङ्क  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    यदि (busy)
	फर्काऊ;
    busy = TRUE;

    row = statusline_row(wp);

    wp->w_redr_status = false;
    यदि (wp->w_status_height == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    अन्यथा यदि (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_status = true;
    }
#ifdef FEAT_STL_OPT
    अन्यथा यदि (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#endif
    अन्यथा
    {
	char_u	*p;
	पूर्णाङ्क	plen;
	पूर्णाङ्क	NameBufflen;
	पूर्णाङ्क	this_ru_col;
	पूर्णाङ्क	n;			// scratch value

	fillchar = fillchar_status(&attr, wp);

	get_trans_bufname(wp->w_buffer);
	p = NameBuff;
	plen = (पूर्णाङ्क)STRLEN(p);

	यदि ((bt_help(wp->w_buffer)
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
	यदि (bt_help(wp->w_buffer))
	    plen += vim_snprintf((वर्ण *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#ifdef FEAT_QUICKFIX
	यदि (wp->w_p_pvw)
	    plen += vim_snprintf((वर्ण *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#endif
	यदि (bufIsChanged(wp->w_buffer) && !bt_terminal(wp->w_buffer))
	    plen += vim_snprintf((वर्ण *)p + plen, MAXPATHL - plen, "%s", "[+]");
	यदि (wp->w_buffer->b_p_ro)
	    plen += vim_snprintf((वर्ण *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_col = ru_col - (cmdline_width - wp->w_width);
	n = (wp->w_width + 1) / 2;
	यदि (this_ru_col < n)
	    this_ru_col = n;
	यदि (this_ru_col <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	अन्यथा यदि (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    लागि (i = 0; p[i] != NUL && plen >= this_ru_col - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    यदि (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	अन्यथा यदि (plen > this_ru_col - 1)
	{
	    p += plen - (this_ru_col - 1);
	    *p = '<';
	    plen = this_ru_col - 1;
	}

	screen_puts(p, row, wp->w_wincol, attr);
	screen_fill(row, row + 1, plen + wp->w_wincol,
		    this_ru_col + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	लागि (i = 1; i < wp->w_status_height; i++)
	    screen_fill(row + i, row + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	यदि ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_col - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, row, (पूर्णाङ्क)(this_ru_col - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	यदि (p_sc && *p_sloc == 's')
	{
	    n = this_ru_col - plen - 2;		    // perform the calculation here so we only do it once
	    पूर्णाङ्क	width = MIN(10, n);

	    यदि (width > 0)
		screen_puts_len(showcmd_buf, width, row,
				wp->w_wincol + this_ru_col - width - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    यदि (wp->w_vsep_width != 0 && wp->w_status_height != 0 && redrawing())
    {
	लागि (i = 0; i < wp->w_status_height; i++)
	{
	    पूर्णाङ्क r = row + i;
	    यदि (stl_connected(wp))
		fillchar = fillchar_status(&attr, wp);
	    अन्यथा
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
    स्थिर शून्य
borrow_stl_vsep_hl(शून्य)
{
    win_T   *बायाँ = रिक्त;
    win_T   *दायाँ = रिक्त;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    यदि (!redrawing() || Screen_पङ्क्तिहरू == रिक्त)
	फर्काऊ;

    FOR_ALL_WINDOWS(बायाँ)
    {
	यदि (बायाँ->w_status_height == 0 || बायाँ->w_vsep_width == 0)
	    जारी;
	// A window of zero width has no status line cell to borrow from.
	यदि (बायाँ->w_width == 0)
	    जारी;
	यदि (!stl_connected(बायाँ))
	    जारी;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = रिक्त;
	पूर्णाङ्क start = 0;
	पूर्णाङ्क end = 0;

	FOR_ALL_WINDOWS(दायाँ)
	{
	    यदि (दायाँ == बायाँ || दायाँ->w_status_height == 0)
		जारी;
	    यदि (दायाँ->w_wincol != W_ENDCOL(बायाँ) + 1)
		जारी;
	    पूर्णाङ्क l_stl_row = W_WINROW(बायाँ) + बायाँ->w_height;
	    पूर्णाङ्क r_stl_row = W_WINROW(दायाँ) + दायाँ->w_height;

	    start = l_stl_row > r_stl_row ? l_stl_row : r_stl_row;
	    end = l_stl_row + बायाँ->w_status_height
				< r_stl_row + दायाँ->w_status_height
		? l_stl_row + बायाँ->w_status_height
		: r_stl_row + दायाँ->w_status_height;
	    यदि (start < end)
	    {
		neighbour = दायाँ;
		रोक;
	    }
	}
	यदि (neighbour == रिक्त)
	    जारी;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	पूर्णाङ्क	hl;
	यदि (बायाँ != curwin && neighbour != curwin
		&& fillchar_status(&hl, बायाँ) != ' ')
	    जारी;

	// Source: prefer curwin's side; otherwise left window's right edge.
	पूर्णाङ्क dst_col = W_ENDCOL(बायाँ);
	पूर्णाङ्क src_col = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(बायाँ) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	यदि (dst_col >= screen_Columns || src_col >= screen_Columns)
	    जारी;
	यदि (end > screen_Rows)
	    end = screen_Rows;

	लागि (पूर्णाङ्क r = start; r < end; r++)
	{
	    अचिन्हित	dst_off = पङ्क्ति_Offset[r] + dst_col;
	    sattr_T	attr = ScreenAttrs[पङ्क्ति_Offset[r] + src_col];

	    यदि (ScreenAttrs[dst_off] != attr)
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
    स्थिर शून्य
redraw_custom_statusline(win_T *wp)
{
    स्थिर पूर्णाङ्क	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    यदि (entered)
	फर्काऊ;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#endif

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    शून्य
showruler(पूर्णाङ्क always)
{
    यदि (!always && !redrawing())
	फर्काऊ;
    यदि (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_status = true;
	फर्काऊ;
    }
#यदि defined(FEAT_STL_OPT)
    यदि ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_status_height)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    अन्यथा
#endif
	win_redr_ruler(curwin, always, FALSE);

    यदि (need_maketitle
#ifdef FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_IN_ICON))
	    || (p_title && (stl_syntax & STL_IN_TITLE))
#endif
       )
	maketitle();

    // Redraw the tab pages line if needed.
    यदि (redraw_tabline)
	draw_tabline();

#यदि defined(FEAT_TABPANEL)
    यदि (redraw_tabpanel)
	draw_tabpanel();
#endif
}

    शून्य
win_redr_ruler(win_T *wp, पूर्णाङ्क always, पूर्णाङ्क ignore_pum)
{
    पूर्णाङ्क	empty_पङ्क्ति = FALSE;

    // If 'ruler' off don't do anything
    यदि (!p_ru)
	फर्काऊ;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    यदि (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_पङ्क्ति_गन्ती)
	फर्काऊ;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    यदि (wp == lastwin && lastwin->w_status_height == 0)
	यदि (edit_submode != रिक्त)
	    फर्काऊ;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    यदि (!ignore_pum && pum_visible())
	फर्काऊ;

#ifdef FEAT_STL_OPT
    यदि (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	फर्काऊ;
    }
#endif

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    यदि ((State & MODE_घुसाउ) == 0
		&& *ml_get_buf(wp->w_buffer, wp->w_cursor.lnum, FALSE) == NUL)
	empty_पङ्क्ति = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    यदि (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.col != wp->w_ru_cursor.col
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_buffer->b_ml.ml_पङ्क्ति_गन्ती != wp->w_ru_पङ्क्ति_गन्ती
#ifdef FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#endif
	    || empty_पङ्क्ति != wp->w_ru_empty)
    {
	पूर्णाङ्क	row;
	पूर्णाङ्क	fillchar;
	पूर्णाङ्क	attr;
	पूर्णाङ्क	off;
	पूर्णाङ्क	width;
	colnr_T	virtcol;
#define RULER_BUF_LEN 70
	char_u	buffer[RULER_BUF_LEN];
	पूर्णाङ्क	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	पूर्णाङ्क	rel_poslen;
	पूर्णाङ्क	this_ru_col;
	पूर्णाङ्क	n1;			    // scratch value
	पूर्णाङ्क	n2;			    // scratch value
	bool	override_success =
	    थप_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	यदि (wp->w_status_height)
	{
	    row = statusline_row(wp);
	    fillchar = fillchar_status(&attr, wp);
	    off = wp->w_wincol;
	    width = wp->w_width;
	}
	अन्यथा
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_col_off;
	    width = cmdline_width;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	यदि (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, रिक्त, &virtcol, रिक्त, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((वर्ण *)buffer, RULER_BUF_LEN, _("%ld,"),
		(wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		    ? 0L
		    : (दीर्घ)(wp->w_cursor.lnum));
	bufferlen += col_print(buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			empty_पङ्क्ति ? 0 : (पूर्णाङ्क)wp->w_cursor.col + 1,
			(पूर्णाङ्क)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	यदि (wp->w_status_height == 0)	// can't use last char of screen
	    ++n1;

	this_ru_col = ru_col - (cmdline_width - width);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (width + 1) / 2;
	यदि (this_ru_col < n2)
	    this_ru_col = n2;
	यदि (this_ru_col + n1 < width)
	{
	    // need at least space for rel_pos + NUL
	    जबसम्म (this_ru_col + n1 < width
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		यदि (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, buffer + bufferlen);
		अन्यथा
		    buffer[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((वर्ण *)buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	यदि (has_mbyte)
	{
	    लागि (n1 = 0, n2 = 0; buffer[n1] != NUL; n1 += (*mb_ptr2len)(buffer + n1))
	    {
		n2 += (*mb_ptr2cells)(buffer + n1);
		यदि (this_ru_col + n2 > width)
		{
		    bufferlen = n1;
		    buffer[bufferlen] = NUL;
		    रोक;
		}
	    }
	}
	अन्यथा यदि (this_ru_col + bufferlen > width)
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
	wp->w_ru_empty = empty_पङ्क्ति;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_पङ्क्ति_गन्ती = wp->w_buffer->b_ml.ml_पङ्क्ति_गन्ती;
#ifdef FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#endif

	यदि (override_success)
	    झिक_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    शून्य
after_updating_screen(पूर्णाङ्क may_resize_shell UNUSED)
{
    updating_screen = FALSE;
#ifdef FEAT_GUI
    यदि (may_resize_shell)
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
    शून्य
update_curbuf(पूर्णाङ्क type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#यदि defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    स्थिर पूर्णाङ्क
text_to_screenline(win_T *wp, char_u *text, पूर्णाङ्क col)
{
    पूर्णाङ्क		off = (पूर्णाङ्क)(current_Screen_पङ्क्ति - Screen_पङ्क्तिहरू);

    यदि (has_mbyte)
    {
	पूर्णाङ्क	cells;
	पूर्णाङ्क	u8c, u8cc[MAX_MCO];
	पूर्णाङ्क	i;
	पूर्णाङ्क	idx;
	पूर्णाङ्क	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	पूर्णाङ्क	prev_c = 0;		// previous Arabic character
	पूर्णाङ्क	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	यदि (wp->w_p_rl)
	    idx = off;
	अन्यथा
# endif
	    idx = off + col;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	लागि (p = text; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    यदि (col + cells > wp->w_width)
		रोक;
# ifdef FEAT_RIGHTLEFT
	    यदि (wp->w_p_rl)
		idx = off + wp->w_width - col - cells;
# endif
	    Screen_पङ्क्तिहरू[idx] = *p;
	    यदि (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		यदि (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_पङ्क्तिहरू_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    prev_c = u8c;
# endif
		}
		अन्यथा
		{
# ifdef FEAT_ARABIC
		    यदि (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			पूर्णाङ्क	pc, pc1, nc;
			पूर्णाङ्क	pcc[MAX_MCO];
			पूर्णाङ्क	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			यदि (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			अन्यथा
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			Screen_पङ्क्तिहरू[idx] = firstbyte;
		    }
		    अन्यथा
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_पङ्क्तिहरू_UC[idx] = u8c;
		    लागि (i = 0; i < Screen_mco; ++i)
		    {
			Screen_पङ्क्तिहरू_C[i][idx] = u8cc[i];
			यदि (u8cc[i] == 0)
			    रोक;
		    }
		}
		यदि (cells > 1)
		    Screen_पङ्क्तिहरू[idx + 1] = 0;
	    }
	    अन्यथा यदि (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    अन्यथा यदि (cells > 1)
		// double-width character
		Screen_पङ्क्तिहरू[idx + 1] = p[1];
	    col += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    अन्यथा
    {
	पूर्णाङ्क len = (पूर्णाङ्क)STRLEN(text);
	पूर्णाङ्क n = wp->w_width - col;

	यदि (len > n)
	    len = n;
	यदि (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    यदि (wp->w_p_rl)
		mch_memmove(current_Screen_पङ्क्ति, text, len);
	    अन्यथा
# endif
		mch_memmove(current_Screen_पङ्क्ति + col, text, len);
	    col += len;
	}
    }
    फर्काऊ col;
}
#endif

#ifdef FEAT_MENU
/*
 * Draw the window toolbar.
 */
    स्थिर शून्य
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    पूर्णाङ्क		item_idx = 0;
    पूर्णाङ्क		item_गन्ती = 0;
    पूर्णाङ्क		col = 0;
    पूर्णाङ्क		next_col;
    पूर्णाङ्क		off = (पूर्णाङ्क)(current_Screen_पङ्क्ति - Screen_पङ्क्तिहरू);
    bool	override_success =
	थप_highlight_overrides(wp->w_hl, wp->w_hl_len);
    पूर्णाङ्क		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    पूर्णाङ्क		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_वस्तुहरू);
    FOR_ALL_CHILD_MENUS(wp->w_winbar, menu)
	++item_गन्ती;
    wp->w_winbar_वस्तुहरू = ALLOC_CLEAR_MULT(winbar_item_T, item_गन्ती + 1);

    // TODO: use fewer spaces if there is not enough room
    लागि (menu = wp->w_winbar->children;
			  menu != रिक्त && col < wp->w_width; menu = menu->next)
    {
	space_to_screenline(off + col, fill_attr);
	यदि (++col >= wp->w_width)
	    रोक;
	यदि (col > 1)
	{
	    space_to_screenline(off + col, fill_attr);
	    यदि (++col >= wp->w_width)
		रोक;
	}

	wp->w_winbar_वस्तुहरू[item_idx].wb_startcol = col;
	space_to_screenline(off + col, button_attr);
	यदि (++col >= wp->w_width)
	    रोक;

	next_col = text_to_screenline(wp, menu->name, col);
	जबसम्म (col < next_col)
	{
	    ScreenAttrs[off + col] = button_attr;
	    ++col;
	}
	wp->w_winbar_वस्तुहरू[item_idx].wb_endcol = col;
	wp->w_winbar_वस्तुहरू[item_idx].wb_menu = menu;
	++item_idx;

	यदि (col >= wp->w_width)
	    रोक;
	space_to_screenline(off + col, button_attr);
	++col;
    }
    जबसम्म (col < wp->w_width)
    {
	space_to_screenline(off + col, fill_attr);
	++col;
    }
    wp->w_winbar_वस्तुहरू[item_idx].wb_menu = रिक्त; // end marker

    screen_पङ्क्ति(wp, wp->w_winrow, wp->w_wincol, wp->w_width,
							  wp->w_width, -1, 0);

    यदि (override_success)
	झिक_highlight_overrides();
}
#endif

#यदि defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    स्थिर शून्य
copy_text_attr(
    पूर्णाङ्क		off,
    char_u	*buf,
    पूर्णाङ्क		len,
    पूर्णाङ्क		attr)
{
    पूर्णाङ्क		i;

    mch_memmove(Screen_पङ्क्तिहरू + off, buf, (माप_प्रकार)len);
    यदि (enc_utf8)
	vim_memset(Screen_पङ्क्तिहरू_UC + off, 0, परिमाण(u8char_T) * (माप_प्रकार)len);
    लागि (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    स्थिर शून्य
fold_पङ्क्ति(
    win_T	*wp,
    दीर्घ	fold_गन्ती,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    पूर्णाङ्क		row)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*शीर्ष, *bot;
    linenr_T	lnume = lnum + fold_गन्ती - 1;
    पूर्णाङ्क		len;
    char_u	*text;
    पूर्णाङ्क		fdc;
    पूर्णाङ्क		col;
    पूर्णाङ्क		txtcol;
    पूर्णाङ्क		off = (पूर्णाङ्क)(current_Screen_पङ्क्ति - Screen_पङ्क्तिहरू);
    पूर्णाङ्क		ri;

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
    यदि (wp == cmdwin_win)
    {
	Screen_पङ्क्तिहरू[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	यदि (enc_utf8)
	    Screen_पङ्क्तिहरू_UC[off] = 0;
	++col;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    गर { \
	यदि (wp->w_p_rl) \
	    लागि (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_width - (p) - (l)) + ri] = v; \
	 अन्यथा \
	    लागि (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } जबसम्म (0)
# अन्यथा
#  define RL_MEMSET(p, v, l) \
    गर { \
	लागि (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } जबसम्म (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, col);
    यदि (fdc > 0)
    {
	char_u	*p;
	पूर्णाङ्क	i;
	पूर्णाङ्क	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	लागि (i = 0; i < fdc; i++)
	{
	    पूर्णाङ्क		ch;

	    यदि (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    अन्यथा
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    यदि (wp->w_p_rl)
		idx = off + wp->w_width - i - 1 - col;
	    अन्यथा
# endif
		idx = off + col + i;
	    यदि (enc_utf8)
	    {
		यदि (ch >= 0x80)
		{
		    Screen_पङ्क्तिहरू_UC[idx] = ch;
		    Screen_पङ्क्तिहरू_C[0][idx] = 0;
		    Screen_पङ्क्तिहरू[idx] = 0x80;
		}
		अन्यथा
		{
		    Screen_पङ्क्तिहरू[idx] = ch;
		    Screen_पङ्क्तिहरू_UC[idx] = 0;
		}
	    }
	    अन्यथा
		Screen_पङ्क्तिहरू[idx] = ch;
	}

	RL_MEMSET(col, HL_ATTR(HLF_FC), fdc);
	col += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    // text
    RL_MEMSET(col, HL_ATTR(HLF_FL), wp->w_width - col);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    यदि (signcolumn_on(wp))
    {
	len = wp->w_width - col;
	यदि (len > 0)
	{
	    यदि (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    यदि (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    अन्यथा
#  endif
		copy_text_attr(off + col, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    col += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    यदि (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_width - col;
	यदि (len > 0)
	{
	    पूर्णाङ्क	    w = सङ्ख्या_width(wp);
	    दीर्घ    अङ्क;
	    वर्ण    *fmt = "%*ld ";

	    यदि (len > w + 1)
		len = w + 1;

	    यदि (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		अङ्क = (दीर्घ)lnum;
	    अन्यथा
	    {
		// 'relativenumber', don't use negative numbers
		अङ्क = द_असल((दीर्घ)get_cursor_rel_lnum(wp, lnum));
		यदि (अङ्क == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    अङ्क = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((वर्ण *)buf, परिमाण(buf), fmt, w, अङ्क);
# ifdef FEAT_RIGHTLEFT
	    यदि (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col, buf, len,
							     HL_ATTR(HLF_FL));
	    अन्यथा
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
    जबसम्म (col < wp->w_width)
    {
	पूर्णाङ्क c = wp->w_fill_chars.fold;
	पूर्णाङ्क idx = off + col;

# ifdef FEAT_RIGHTLEFT
	यदि (wp->w_p_rl)
	    idx = off + wp->w_width - 1 - col;
# endif

	यदि (enc_utf8)
	{
	    यदि (c >= 0x80)
	    {
		Screen_पङ्क्तिहरू_UC[idx] = c;
		Screen_पङ्क्तिहरू_C[0][idx] = 0;
		Screen_पङ्क्तिहरू[idx] = 0x80; // avoid storing zero
	    }
	    अन्यथा
	    {
		Screen_पङ्क्तिहरू_UC[idx] = 0;
		Screen_पङ्क्तिहरू[idx] = c;
	    }
	}
	अन्यथा
	    Screen_पङ्क्तिहरू[idx] = c;
	++col;
    }

    यदि (text != buf)
	vim_free(text);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    यदि (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
	यदि (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    शीर्ष = &curwin->w_cursor;
	    bot = &VIsual;
	}
	अन्यथा
	{
	    // Visual is before curwin->w_cursor
	    शीर्ष = &VIsual;
	    bot = &curwin->w_cursor;
	}
	यदि (lnum >= शीर्ष->lnum
		&& lnume <= bot->lnum
		&& (VIsual_mode != 'v'
		    || ((lnum > शीर्ष->lnum
			    || (lnum == शीर्ष->lnum
				&& शीर्ष->col == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->col - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_buffer, lnume))))))
	{
	    यदि (VIsual_mode == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		यदि (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_width)
		{
		    यदि (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_width)
			len = wp->w_old_cursor_lcol;
		    अन्यथा
			len = wp->w_width - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (पूर्णाङ्क)wp->w_old_cursor_fcol);
		}
	    }
	    अन्यथा
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_width - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    यदि (wp->w_p_cc_cols)
    {
	पूर्णाङ्क i = 0;
	पूर्णाङ्क j = wp->w_p_cc_cols[i];
	पूर्णाङ्क old_txtcol = txtcol;

	जबसम्म (j > -1)
	{
	    txtcol += j;
	    यदि (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    अन्यथा
		txtcol -= wp->w_leftcol;
	    यदि (txtcol >= 0 && txtcol < wp->w_width)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_cols[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    यदि (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	यदि (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	अन्यथा
	    txtcol -= wp->w_leftcol;
	यदि (txtcol >= 0 && txtcol < wp->w_width)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_पङ्क्ति(wp, row + W_WINROW(wp), wp->w_wincol, wp->w_width, wp->w_width,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    यदि (wp == curwin
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
    यदि (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_पङ्क्ति(wp))
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
    स्थिर शून्य
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_buffer;
    पूर्णाङ्क		type;
    पूर्णाङ्क		शीर्ष_end = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    पूर्णाङ्क		mid_start = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    पूर्णाङ्क		mid_end = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    पूर्णाङ्क		bot_start = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    पूर्णाङ्क		scrolled_down = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    पूर्णाङ्क		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //पङ्क्तिहरू
#ifdef FEAT_खोज_EXTRA
    पूर्णाङ्क		शीर्ष_to_mod = FALSE;    // redraw above mod_top
#endif

    पूर्णाङ्क		row;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    पूर्णाङ्क		idx;		// current index in w_lines[]
    पूर्णाङ्क		srow;		// starting row of the current line

    पूर्णाङ्क		eof = FALSE;	// if TRUE, we hit the end of the file
    पूर्णाङ्क		didline = FALSE; // if TRUE, we finished the last line
    पूर्णाङ्क		i;
    दीर्घ	j;
    स्थिर पूर्णाङ्क	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#ifdef FEAT_CONCEAL
    पूर्णाङ्क		old_wrow = wp->w_wrow;
    पूर्णाङ्क		old_wcol = wp->w_wcol;
#endif
#ifdef FEAT_FOLDING
    दीर्घ	fold_गन्ती;
#endif
#ifdef FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_पङ्क्ति 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    पूर्णाङ्क		did_update = DID_NONE;
    linenr_T	syntax_last_parsed = 0;		// last parsed text line
#endif
    linenr_T	mod_शीर्ष = 0;
    linenr_T	mod_bot = 0;
#यदि defined(FEAT_SYN_HL) || defined(FEAT_खोज_EXTRA)
    पूर्णाङ्क		save_got_int;
#endif
    bool	override_success;

#यदि defined(FEAT_खोज_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    यदि (!did_update_one_window)
    {
	did_update_one_window = TRUE;
# ifdef FEAT_खोज_EXTRA
	start_खोज_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	यदि (clip_star.available && clip_isautosel_star())
	    clip_update_selection(&clip_star);
	यदि (clip_plus.available && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#endif

#ifdef FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    यदि (wp->w_p_cuc)
	validate_virtcol_win(wp);
#endif

    type = wp->w_redr_type;

    यदि (type == UPD_NOT_VALID)
    {
	wp->w_redr_status = true;
	wp->w_पङ्क्तिहरू_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    यदि (wp->w_height + WINBAR_HEIGHT(wp) == 0
	    || (wp->w_frame->fr_height == wp->w_status_height
#यदि defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#endif
	       ))
    {
	wp->w_redr_type = 0;
	फर्काऊ;
    }

    // Window is zero-width: Only need to draw the separator.
    यदि (wp->w_width == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	फर्काऊ;
    }

    override_success = थप_highlight_overrides(wp->w_hl, wp->w_hl_len);


#ifdef FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    यदि (term_do_update_window(wp))
    {
	term_update_window(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	यदि (winbar_height(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	यदि (override_success)
	    झिक_highlight_overrides();
	फर्काऊ;
    }
#endif

#ifdef FEAT_खोज_EXTRA
    init_खोज_hl(wp, &screen_खोज_hl);
#endif

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    यदि (wp->w_skipcol > 0 && wp->w_width > win_col_off(wp))
    {
	पूर्णाङ्क w = 0;
	पूर्णाङ्क width1 = wp->w_width - win_col_off(wp);
	पूर्णाङ्क width2 = width1 + win_col_off2(wp);
	पूर्णाङ्क add = width1;

	जबसम्म (w < wp->w_skipcol)
	{
	    यदि (w > 0)
		add = width2;
	    w += add;
	}
	यदि (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - add;
    }

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? सङ्ख्या_width(wp) : 0;
    यदि (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    अन्यथा
#endif

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_शीर्ष = wp->w_redraw_शीर्ष;
	यदि (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	अन्यथा
	    mod_bot = 0;
	यदि (buf->b_mod_set)
	{
	    यदि (mod_शीर्ष == 0 || mod_शीर्ष > buf->b_mod_शीर्ष)
	    {
		mod_शीर्ष = buf->b_mod_शीर्ष;
#ifdef FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		यदि (syntax_present(wp))
		{
		    mod_शीर्ष -= buf->b_s.b_syn_sync_linebreaks;
		    यदि (mod_शीर्ष < 1)
			mod_शीर्ष = 1;
		}
#endif
	    }
	    यदि (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ifdef FEAT_खोज_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    यदि (screen_खोज_hl.rm.regprog != रिक्त
		    && re_multiline(screen_खोज_hl.rm.regprog))
		शीर्ष_to_mod = TRUE;
	    अन्यथा
	    {
		matchitem_T *cur = wp->w_match_head;

		जबसम्म (cur != रिक्त)
		{
		    यदि (cur->mit_match.regprog != रिक्त
				       && re_multiline(cur->mit_match.regprog))
		    {
			शीर्ष_to_mod = TRUE;
			रोक;
		    }
		    cur = cur->mit_next;
		}
	    }
#endif
	}

#ifdef FEAT_खोज_EXTRA
	यदि (खोज_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    यदि (mod_शीर्ष == 0 || mod_शीर्ष > खोज_hl_has_cursor_lnum)
		mod_शीर्ष = खोज_hl_has_cursor_lnum;
	    यदि (mod_bot == 0 || mod_bot < खोज_hl_has_cursor_lnum + 1)
		mod_bot = खोज_hl_has_cursor_lnum + 1;
	}
#endif

#ifdef FEAT_FOLDING
	यदि (mod_शीर्ष != 0 && hasAnyFolding(wp))
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
	    लागि (i = 0; i < wp->w_पङ्क्तिहरू_valid; ++i)
		यदि (wp->w_पङ्क्तिहरू[i].wl_valid)
		{
		    यदि (wp->w_पङ्क्तिहरू[i].wl_lastlnum < mod_शीर्ष)
			lnumt = wp->w_पङ्क्तिहरू[i].wl_lastlnum + 1;
		    यदि (lnumb == MAXLNUM && wp->w_पङ्क्तिहरू[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_पङ्क्तिहरू[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			यदि (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (शून्य)hasFoldingWin(wp, mod_शीर्ष, &mod_शीर्ष, रिक्त, TRUE, रिक्त);
	    यदि (mod_शीर्ष > lnumt)
		mod_शीर्ष = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (शून्य)hasFoldingWin(wp, mod_bot, रिक्त, &mod_bot, TRUE, रिक्त);
	    ++mod_bot;
	    यदि (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#endif

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	यदि (mod_शीर्ष != 0 && mod_शीर्ष < wp->w_topline)
	{
	    यदि (mod_bot > wp->w_topline)
		mod_शीर्ष = wp->w_topline;
#ifdef FEAT_SYN_HL
	    अन्यथा यदि (syntax_present(wp))
		शीर्ष_end = 1;
#endif
	}
    }
    wp->w_redraw_शीर्ष = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#ifdef FEAT_खोज_EXTRA
    खोज_hl_has_cursor_lnum = 0;
#endif


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    यदि (type == UPD_REDRAW_शीर्ष)
    {
	j = 0;
	लागि (i = 0; i < wp->w_पङ्क्तिहरू_valid; ++i)
	{
	    j += wp->w_पङ्क्तिहरू[i].wl_size;
	    यदि (j >= wp->w_upd_rows)
	    {
		शीर्ष_end = j;
		रोक;
	    }
	}
	यदि (शीर्ष_end == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	अन्यथा
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    यदि (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    यदि ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#ifdef FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#endif
	    )
    {
	यदि (mod_शीर्ष != 0
		&& wp->w_topline == mod_शीर्ष
		&& (!wp->w_पङ्क्तिहरू[0].wl_valid
		    || wp->w_topline == wp->w_पङ्क्तिहरू[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	अन्यथा यदि (wp->w_पङ्क्तिहरू[0].wl_valid
		&& (wp->w_topline < wp->w_पङ्क्तिहरू[0].wl_lnum
#ifdef FEAT_DIFF
		    || (wp->w_topline == wp->w_पङ्क्तिहरू[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#endif
		   ))
	{
	    // New topline is above old topline: May scroll down.
#ifdef FEAT_FOLDING
	    यदि (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		लागि (ln = wp->w_topline; ln < wp->w_पङ्क्तिहरू[0].wl_lnum; ++ln)
		{
		    ++j;
		    यदि (j >= wp->w_height - 2)
			रोक;
		    (शून्य)hasFoldingWin(wp, ln, रिक्त, &ln, TRUE, रिक्त);
		}
	    }
	    अन्यथा
#endif
		j = wp->w_पङ्क्तिहरू[0].wl_lnum - wp->w_topline;
	    यदि (j < wp->w_height - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_पङ्क्तिहरू[0].wl_lnum - 1,
								wp->w_height);
#ifdef FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		यदि (wp->w_पङ्क्तिहरू[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_पङ्क्तिहरू[0].wl_lnum)
							  - wp->w_old_topfill;
#endif
		यदि (i < wp->w_height - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    यदि (i > 0)
			check_for_delay(FALSE);
		    यदि (win_ins_पङ्क्तिहरू(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			यदि (wp->w_पङ्क्तिहरू_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    शीर्ष_end = i;
			    scrolled_down = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    यदि ((wp->w_पङ्क्तिहरू_valid += j) > wp->w_height)
				wp->w_पङ्क्तिहरू_valid = wp->w_height;
			    लागि (idx = wp->w_पङ्क्तिहरू_valid; idx - j >= 0; idx--)
				wp->w_पङ्क्तिहरू[idx] = wp->w_पङ्क्तिहरू[idx - j];
			    जबसम्म (idx >= 0)
				wp->w_पङ्क्तिहरू[idx--].wl_valid = FALSE;
			}
		    }
		    अन्यथा
			mid_start = 0;		// redraw all lines
		}
		अन्यथा
		    mid_start = 0;		// redraw all lines
	    }
	    अन्यथा
		mid_start = 0;		// redraw all lines
	}
	अन्यथा
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    row = 0;
	    लागि (i = 0; i < wp->w_पङ्क्तिहरू_valid && i < Rows; i++)
	    {
		यदि (wp->w_पङ्क्तिहरू[i].wl_valid
			&& wp->w_पङ्क्तिहरू[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    रोक;
		}
		row += wp->w_पङ्क्तिहरू[i].wl_size;
	    }
	    यदि (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//पङ्क्तिहरू
		mid_start = 0;
	    }
	    अन्यथा
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#ifdef FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		यदि (wp->w_पङ्क्तिहरू[0].wl_lnum == wp->w_topline)
		    row += wp->w_old_topfill;
		अन्यथा
		    row += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		row -= wp->w_topfill;
#endif
		यदि (row > Rows)  // just in case
		    row = Rows;
		यदि (row > 0)
		{
		    check_for_delay(FALSE);
		    यदि (win_del_पङ्क्तिहरू(wp, 0, row, FALSE, wp == firstwin, 0)
									 == OK)
			bot_start = wp->w_height - row;
		    अन्यथा
			mid_start = 0;		// redraw all lines
		}
		यदि ((row == 0 || bot_start < 999) && wp->w_पङ्क्तिहरू_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_start = 0;
		    idx = 0;
		    लागि (;;)
		    {
			wp->w_पङ्क्तिहरू[idx] = wp->w_पङ्क्तिहरू[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			यदि (row > 0 && bot_start + row
				 + (पूर्णाङ्क)wp->w_पङ्क्तिहरू[j].wl_size > wp->w_height)
			{
			    wp->w_पङ्क्तिहरू_valid = idx + 1;
			    रोक;
			}
			bot_start += wp->w_पङ्क्तिहरू[idx++].wl_size;

			// stop at the last valid entry in w_lines[].wl_size
			यदि (++j >= wp->w_पङ्क्तिहरू_valid)
			{
			    wp->w_पङ्क्तिहरू_valid = idx;
			    रोक;
			}
		    }
#ifdef FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    यदि (wp->w_p_diff && bot_start > 0)
			wp->w_पङ्क्तिहरू[0].wl_size = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#endif
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	यदि (mid_start == 0)
	    mid_end = wp->w_height;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	यदि (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    अन्यथा
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
    }

    यदि (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    यदि ((VIsual_active && buf == curwin->w_buffer)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, to;

	यदि (VIsual_active)
	{
	    यदि (VIsual_mode != wp->w_old_visual_mode
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		यदि (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = VIsual.lnum;
		}
		अन्यथा
		{
		    from = VIsual.lnum;
		    to = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		यदि (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		यदि (wp->w_old_cursor_lnum > to)
		    to = wp->w_old_cursor_lnum;
		यदि (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		यदि (wp->w_old_visual_lnum > to)
		    to = wp->w_old_visual_lnum;
	    }
	    अन्यथा
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		यदि (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = wp->w_old_cursor_lnum;
		}
		अन्यथा
		{
		    from = wp->w_old_cursor_lnum;
		    to = curwin->w_cursor.lnum;
		    यदि (from == 0)	// Visual mode just started
			from = to;
		}

		यदि (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.col != wp->w_old_visual_col)
		{
		    यदि (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    यदि (wp->w_old_visual_lnum > to)
			to = wp->w_old_visual_lnum;
		    यदि (VIsual.lnum < from)
			from = VIsual.lnum;
		    यदि (VIsual.lnum > to)
			to = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    यदि (VIsual_mode == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_END_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		यदि (curwin->w_curswant == MAXCOL)
		{
		    यदि (get_ve_flags() & VE_BLOCK)
		    {
			pos_T	    pos;
			पूर्णाङ्क	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			लागि (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.col = (पूर्णाङ्क)ml_get_buf_len(wp->w_buffer, pos.lnum);
			    getvvcol(wp, &pos, रिक्त, रिक्त, &t, 0);
			    यदि (toc < t)
				toc = t;
			}
			++toc;
		    }
		    अन्यथा
			toc = MAXCOL;
		}

		यदि (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    यदि (from > VIsual.lnum)
			from = VIsual.lnum;
		    यदि (to < VIsual.lnum)
			to = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	अन्यथा
	{
	    // Use the line numbers of the old Visual area.
	    यदि (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		to = wp->w_old_visual_lnum;
	    }
	    अन्यथा
	    {
		from = wp->w_old_visual_lnum;
		to = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	यदि (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	यदि (wp->w_valid & VALID_BOTLINE)
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
	यदि (mid_start > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    यदि (scrolled_down)
		mid_start = शीर्ष_end;
	    अन्यथा
		mid_start = 0;
	    जबसम्म (lnum < from && idx < wp->w_पङ्क्तिहरू_valid)	// find start
	    {
		यदि (wp->w_पङ्क्तिहरू[idx].wl_valid)
		    mid_start += wp->w_पङ्क्तिहरू[idx].wl_size;
		अन्यथा यदि (!scrolled_down)
		    srow += wp->w_पङ्क्तिहरू[idx].wl_size;
		++idx;
#ifdef FEAT_FOLDING
		यदि (idx < wp->w_पङ्क्तिहरू_valid && wp->w_पङ्क्तिहरू[idx].wl_valid)
		    lnum = wp->w_पङ्क्तिहरू[idx].wl_lnum;
		अन्यथा
#endif
		    ++lnum;
	    }
	    srow += mid_start;
	    mid_end = wp->w_height;
	    लागि ( ; idx < wp->w_पङ्क्तिहरू_valid; ++idx)		// find end
	    {
		यदि (wp->w_पङ्क्तिहरू[idx].wl_valid
			&& wp->w_पङ्क्तिहरू[idx].wl_lnum >= to + 1)
		{
		    // Only update until first row of this line
		    mid_end = srow;
		    रोक;
		}
		srow += wp->w_पङ्क्तिहरू[idx].wl_size;
	    }
	}
    }

    यदि (VIsual_active && buf == curwin->w_buffer)
    {
	wp->w_old_visual_mode = VIsual_mode;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_col = VIsual.col;
	wp->w_old_curswant = curwin->w_curswant;
    }
    अन्यथा
    {
	wp->w_old_visual_mode = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_col = 0;
    }

#यदि defined(FEAT_SYN_HL) || defined(FEAT_खोज_EXTRA)
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
    यदि (winbar_height(wp) > 0)
	redraw_win_toolbar(wp);
#endif

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#ifdef FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_FIELD(spv);
    यदि (spell_check_window(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_शीर्ष == 0;
    }
#endif

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    row = 0;
    srow = 0;
    लागि (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	यदि (row == wp->w_height)
	{
	    didline = TRUE;
	    रोक;
	}

	// stop updating when hit the end of the file
	यदि (lnum > buf->b_ml.ml_पङ्क्ति_गन्ती)
	{
	    eof = TRUE;
	    रोक;
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
	यदि (row < शीर्ष_end
		|| (row >= mid_start && row < mid_end)
#ifdef FEAT_खोज_EXTRA
		|| शीर्ष_to_mod
#endif
		|| idx >= wp->w_पङ्क्तिहरू_valid
		|| (row + wp->w_पङ्क्तिहरू[idx].wl_size > bot_start)
		|| (mod_शीर्ष != 0
		    && (lnum == mod_शीर्ष
			|| (lnum >= mod_शीर्ष
			    && (lnum < mod_bot
#ifdef FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_पङ्क्ति
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_changed(lnum)))
#endif
#ifdef FEAT_खोज_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_match_head != रिक्त
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
#ifdef FEAT_खोज_EXTRA
	    यदि (lnum == mod_शीर्ष)
		शीर्ष_to_mod = FALSE;
#endif

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    यदि (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_शीर्ष && lnum < MAX(mod_bot, mod_शीर्ष + 1)
		    && (!scrolled_down || row >= शीर्ष_end))
	    {
		scrolled_for_mod = TRUE;

		पूर्णाङ्क		old_cline_height = 0;
		पूर्णाङ्क		old_rows = 0;
		पूर्णाङ्क		new_rows = 0;
		पूर्णाङ्क		xtra_rows;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		लागि (i = idx; i < wp->w_पङ्क्तिहरू_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    यदि (wp->w_पङ्क्तिहरू[i].wl_valid
			    && wp->w_पङ्क्तिहरू[i].wl_lnum == mod_bot)
			रोक;
		    यदि (wp->w_पङ्क्तिहरू[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_height = wp->w_पङ्क्तिहरू[i].wl_size;
		    old_rows += wp->w_पङ्क्तिहरू[i].wl_size;
#ifdef FEAT_FOLDING
		    यदि (wp->w_पङ्क्तिहरू[i].wl_valid
			    && wp->w_पङ्क्तिहरू[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			जबसम्म (i < wp->w_पङ्क्तिहरू_valid
						  && !wp->w_पङ्क्तिहरू[i].wl_valid)
			    old_rows += wp->w_पङ्क्तिहरू[i++].wl_size;
			रोक;
		    }
#endif
		}

		यदि (i >= wp->w_पङ्क्तिहरू_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_start = 0;
		}
		अन्यथा
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    लागि (l = lnum; l < mod_bot; ++l)
		    {
			यदि (dollar_vcol >= 0 && wp == curwin &&
				old_cline_height > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_rows += old_cline_height;
#ifdef FEAT_FOLDING
			अन्यथा यदि (hasFoldingWin(wp, l, रिक्त, &l, TRUE, रिक्त))
			    ++new_rows;
#endif
			अन्यथा
			    new_rows += plines_correct_topline(wp, l, TRUE);
			++j;
			यदि (new_rows > wp->w_height - row - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_rows = 9999;
			    रोक;
			}
		    }
		    xtra_rows = new_rows - old_rows;
		    यदि (xtra_rows < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			यदि (row - xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			अन्यथा
			{
			    check_for_delay(FALSE);
			    यदि (win_del_पङ्क्तिहरू(wp, row,
					  -xtra_rows, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    अन्यथा
				bot_start = wp->w_height + xtra_rows;
			}
		    }
		    अन्यथा यदि (xtra_rows > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			यदि (row + xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			अन्यथा
			{
			    check_for_delay(FALSE);
			    यदि (win_ins_पङ्क्तिहरू(wp, row + old_rows,
					     xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    अन्यथा यदि (शीर्ष_end > row + old_rows)
				// Scrolled the part at the top that requires
				// updating down.
				शीर्ष_end += xtra_rows;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    यदि (mod_bot != MAXLNUM && i != j)
		    {
			यदि (j < i)
			{
			    पूर्णाङ्क x = row + new_rows;

			    // move entries in w_lines[] upwards
			    लागि (;;)
			    {
				// stop at last valid entry in w_lines[]
				यदि (i >= wp->w_पङ्क्तिहरू_valid)
				{
				    wp->w_पङ्क्तिहरू_valid = j;
				    रोक;
				}
				wp->w_पङ्क्तिहरू[j] = wp->w_पङ्क्तिहरू[i];
				// stop at a line that won't fit
				यदि (x + (पूर्णाङ्क)wp->w_पङ्क्तिहरू[j].wl_size
							   > wp->w_height)
				{
				    wp->w_पङ्क्तिहरू_valid = j + 1;
				    रोक;
				}
				x += wp->w_पङ्क्तिहरू[j++].wl_size;
				++i;
			    }
			    यदि (bot_start > x)
				bot_start = x;
			}
			अन्यथा // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_पङ्क्तिहरू_valid += j;
			    यदि (wp->w_पङ्क्तिहरू_valid > wp->w_height)
				wp->w_पङ्क्तिहरू_valid = wp->w_height;
			    लागि (i = wp->w_पङ्क्तिहरू_valid; i - j >= idx; --i)
				wp->w_पङ्क्तिहरू[i] = wp->w_पङ्क्तिहरू[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    जबसम्म (i >= idx)
			    {
				wp->w_पङ्क्तिहरू[i].wl_size = 0;
				wp->w_पङ्क्तिहरू[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ifdef FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_गन्ती = folded_गन्ती(wp, lnum, &win_foldinfo);
	    यदि (fold_गन्ती != 0)
	    {
		fold_पङ्क्ति(wp, fold_गन्ती, &win_foldinfo, lnum, row);
		++row;
		--fold_गन्ती;
		wp->w_पङ्क्तिहरू[idx].wl_folded = TRUE;
		wp->w_पङ्क्तिहरू[idx].wl_lastlnum = lnum + fold_गन्ती;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
# ifdef FEAT_SPELL
		spv.spv_capcol_lnum = 0;
# endif
	    }
	    अन्यथा
#endif
	    यदि (idx < wp->w_पङ्क्तिहरू_valid
		    && wp->w_पङ्क्तिहरू[idx].wl_valid
		    && wp->w_पङ्क्तिहरू[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_पङ्क्तिहरू[idx].wl_size > wp->w_height
#ifdef FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#endif
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		row = wp->w_height + 1;
	    }
	    अन्यथा
	    {
#ifdef FEAT_खोज_EXTRA
		prepare_खोज_hl(wp, &screen_खोज_hl, lnum);
#endif
#ifdef FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		यदि (syntax_last_parsed != 0 && syntax_last_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

		// Display one line.
		row = win_पङ्क्ति(wp, lnum, srow, wp->w_height, 0, &spv);

#ifdef FEAT_FOLDING
		wp->w_पङ्क्तिहरू[idx].wl_folded = FALSE;
		wp->w_पङ्क्तिहरू[idx].wl_lastlnum = lnum;
#endif
#ifdef FEAT_SYN_HL
		did_update = DID_पङ्क्ति;
		syntax_last_parsed = lnum;
#endif
	    }

	    wp->w_पङ्क्तिहरू[idx].wl_lnum = lnum;
	    wp->w_पङ्क्तिहरू[idx].wl_valid = TRUE;

	    पूर्णाङ्क is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    यदि (row > wp->w_height || row + wp->w_winrow >= Rows)
	    {
		// we may need the size of that too long line later on
		यदि (dollar_vcol == -1 || !is_curline)
		    wp->w_पङ्क्तिहरू[idx].wl_size = plines_win(wp, lnum, TRUE);
		++idx;
		रोक;
	    }
	    यदि (dollar_vcol == -1 || !is_curline)
		wp->w_पङ्क्तिहरू[idx].wl_size = row - srow;
	    ++idx;
#ifdef FEAT_FOLDING
	    lnum += fold_गन्ती + 1;
#अन्यथा
	    ++lnum;
#endif
	}
	अन्यथा
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    यदि ((wp->w_p_nu && mod_शीर्ष != 0 && lnum >= mod_bot
			&& buf->b_mod_set && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_last_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#ifdef FEAT_FOLDING
		fold_गन्ती = folded_गन्ती(wp, lnum, &win_foldinfo);
		यदि (fold_गन्ती != 0)
		    fold_पङ्क्ति(wp, fold_गन्ती, &win_foldinfo, lnum, row);
		अन्यथा
#endif
		    (शून्य)win_पङ्क्ति(wp, lnum, srow, wp->w_height,
					       wp->w_पङ्क्तिहरू[idx].wl_size, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    row += wp->w_पङ्क्तिहरू[idx++].wl_size;
	    यदि (row > wp->w_height)	// past end of screen
		रोक;
#ifdef FEAT_FOLDING
	    lnum = wp->w_पङ्क्तिहरू[idx - 1].wl_lastlnum + 1;
#अन्यथा
	    ++lnum;
#endif
#ifdef FEAT_SYN_HL
	    did_update = DID_NONE;
#endif
#ifdef FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#endif
	}

	यदि (lnum > buf->b_ml.ml_पङ्क्ति_गन्ती)
	{
	    eof = TRUE;
	    रोक;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	यदि (idx >= Rows)
	    रोक;
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
    यदि (use_vtp() && get_conpty_fix_type() < 1)
    {
	पूर्णाङ्क k;

	लागि (k = 0; k < Rows; ++k)
	    यदि (enc_utf8)
		यदि ((*mb_off2cells)(पङ्क्ति_Offset[k] + topframe->fr_width - 2,
					   पङ्क्ति_Offset[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_width - 2, 1, 2,
			    FALSE, FALSE);
		अन्यथा
		    screen_draw_rectangle(k, topframe->fr_width - 1, 1, 1,
			    FALSE, FALSE);
	    अन्यथा
		screen_char(पङ्क्ति_Offset[k] + topframe->fr_width - 1, k,
			cmdline_width - 1);
    }
#endif

    यदि (idx > wp->w_पङ्क्तिहरू_valid)
	wp->w_पङ्क्तिहरू_valid = idx;

#ifdef FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    यदि (syntax_last_parsed != 0 && syntax_present(wp))
	syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_empty_rows = 0;
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    यदि (!eof && !didline)
    {
	यदि (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#ifdef FEAT_DIFF
	अन्यथा यदि (diff_check_fill(wp, lnum) >= wp->w_height - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_rows = wp->w_height - srow;
	}
#endif
#ifdef FEAT_PROP_POPUP
	अन्यथा यदि (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#endif
	अन्यथा यदि (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    पूर्णाङ्क		scr_row = W_WINROW(wp) + wp->w_height - 1;
	    पूर्णाङ्क		चिह्न  = wp->w_fill_chars.lastline;
	    पूर्णाङ्क		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(चिह्न, &fillbuf[0]);
	    mb_char2bytes(चिह्न, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_width > 2 ? 2 : wp->w_width) * charlen,
			    scr_row, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_row, scr_row + 1,
		    (पूर्णाङ्क)wp->w_wincol + 2, (पूर्णाङ्क)W_ENDCOL(wp),
		    चिह्न, ' ', HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	अन्यथा यदि (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    पूर्णाङ्क start_col = (पूर्णाङ्क)W_ENDCOL(wp) - 3;
	    पूर्णाङ्क चिह्न    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_height - 1,
		    W_WINROW(wp) + wp->w_height,
		    (start_col < wp->w_wincol ? wp->w_wincol : start_col),
		    (पूर्णाङ्क)W_ENDCOL(wp),
		    चिह्न, चिह्न, HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	अन्यथा
	{
	    win_draw_end(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_height, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    अन्यथा
    {
	draw_vsep_win(wp, row);
	यदि (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_पङ्क्ति_गन्ती + 1;
#ifdef FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    यदि (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		यदि (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		अन्यथा
		    i = wp->w_fill_chars.diff;
		यदि (row + j > wp->w_height)
		    j = wp->w_height - row;
		win_draw_end(wp, i, i, TRUE, row, row + (पूर्णाङ्क)j, HLF_DED);
		row += j;
	    }
#endif
	}
	अन्यथा यदि (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	यदि (WIN_IS_POPUP(wp))
	    win_draw_end(wp, ' ', ' ', FALSE, row, wp->w_height, HLF_AT);
	अन्यथा
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

    यदि (dollar_vcol == -1 || wp != curwin)
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
	यदि (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#यदि defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    पूर्णाङ्क		new_wcol = wp->w_wcol;
#endif
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#यदि defined(FEAT_CONCEAL)
	    यदि (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
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
		यदि (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		अन्यथा
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#endif
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    यदि (wp->w_redr_type != 0)
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
		यदि (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#यदि defined(FEAT_SYN_HL) || defined(FEAT_खोज_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    यदि (!got_int)
	got_int = save_got_int;
#endif

    यदि (override_success)
	झिक_highlight_overrides();
}

#यदि defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    स्थिर शून्य
update_prepare(शून्य)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    यदि (gui.in_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_खोज_EXTRA
    start_खोज_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_mask(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    स्थिर शून्य
update_finish(शून्य)
{
    यदि (redraw_cmdline || redraw_mode)
	showmode();

# ifdef FEAT_खोज_EXTRA
    end_खोज_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    यदि (gui.in_use)
    {
	out_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#endif

#यदि defined(FEAT_NETBEANS_INTG)
    शून्य
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    पूर्णाङ्क		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_पङ्क्ति_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	यदि (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    यदि (!doit || updating_screen
	    || State == MODE_ASKMORE || State == MODE_HITRETURN
	    || सन्देश_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	फर्काऊ;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	यदि (wp->w_redr_type != 0)
	    win_update(wp);
	यदि (wp->w_redr_status)
	    win_redr_status(wp, FALSE);
    }

# यदि defined(FEAT_TABPANEL)
    यदि (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_finish();
}
#endif

#यदि defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    शून्य
updateWindow(win_T *wp)
{
    // return if already busy updating
    यदि (updating_screen)
	फर्काऊ;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    यदि (clip_star.available && clip_isautosel_star())
	clip_update_selection(&clip_star);
    यदि (clip_plus.available && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    यदि (redraw_tabline)
	draw_tabline();

# यदि defined(FEAT_TABPANEL)
    यदि (redraw_tabpanel)
	draw_tabpanel();
# endif

    यदि (wp->w_redr_status || p_ru
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
    पूर्णाङ्क
redraw_asap(पूर्णाङ्क type)
{
    पूर्णाङ्क		rows;
    पूर्णाङ्क		cols = screen_Columns;
    पूर्णाङ्क		r;
    पूर्णाङ्क		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    पूर्णाङ्क		i;
    u8char_T	*screenlineUC = रिक्त;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = रिक्त;	// copy from ScreenLines2[]

    redraw_later(type);
    यदि (सन्देश_scrolled
	    || (State != MODE_NORMAL && State != MODE_NORMAL_BUSY)
	    || exiting)
	फर्काऊ ret;

    // Allocate space to save the text displayed in the command line area.
    rows = screen_Rows - cmdline_row;
    screenline = LALLOC_MULT(schar_T, rows * cols);
    screenattr = LALLOC_MULT(sattr_T, rows * cols);
    यदि (screenline == रिक्त || screenattr == रिक्त)
	ret = 2;
    यदि (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, rows * cols);
	यदि (screenlineUC == रिक्त)
	    ret = 2;
	लागि (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, rows * cols);
	    यदि (screenlineC[i] == रिक्त)
		ret = 2;
	}
    }
    यदि (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, rows * cols);
	यदि (screenline2 == रिक्त)
	    ret = 2;
    }

    यदि (ret != 2)
    {
	// Save the text displayed in the command line area.
	लागि (r = 0; r < rows; ++r)
	{
	    mch_memmove(screenline + r * cols,
			Screen_पङ्क्तिहरू + पङ्क्ति_Offset[cmdline_row + r],
			(माप_प्रकार)cols * परिमाण(schar_T));
	    mch_memmove(screenattr + r * cols,
			ScreenAttrs + पङ्क्ति_Offset[cmdline_row + r],
			(माप_प्रकार)cols * परिमाण(sattr_T));
	    यदि (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * cols,
			    Screen_पङ्क्तिहरू_UC + पङ्क्ति_Offset[cmdline_row + r],
			    (माप_प्रकार)cols * परिमाण(u8char_T));
		लागि (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * cols,
				Screen_पङ्क्तिहरू_C[i] + पङ्क्ति_Offset[cmdline_row + r],
				(माप_प्रकार)cols * परिमाण(u8char_T));
	    }
	    यदि (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * cols,
			    ScreenLines2 + पङ्क्ति_Offset[cmdline_row + r],
			    (माप_प्रकार)cols * परिमाण(schar_T));
	}

	update_screen(0);
	ret = 3;

	यदि (must_redraw == 0)
	{
	    पूर्णाङ्क	off = (पूर्णाङ्क)(current_Screen_पङ्क्ति - Screen_पङ्क्तिहरू);

	    // Restore the text displayed in the command line area.
	    लागि (r = 0; r < rows; ++r)
	    {
		mch_memmove(current_Screen_पङ्क्ति,
			    screenline + r * cols,
			    (माप_प्रकार)cols * परिमाण(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * cols,
			    (माप_प्रकार)cols * परिमाण(sattr_T));
		यदि (enc_utf8)
		{
		    mch_memmove(Screen_पङ्क्तिहरू_UC + off,
				screenlineUC + r * cols,
				(माप_प्रकार)cols * परिमाण(u8char_T));
		    लागि (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_पङ्क्तिहरू_C[i] + off,
				    screenlineC[i] + r * cols,
				    (माप_प्रकार)cols * परिमाण(u8char_T));
		}
		यदि (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * cols,
				(माप_प्रकार)cols * परिमाण(schar_T));
		screen_पङ्क्ति(curwin, cmdline_row + r, 0, cols, cols, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    यदि (enc_utf8)
    {
	vim_free(screenlineUC);
	लागि (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    यदि (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_message();

    setcursor();

    फर्काऊ ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    शून्य
redraw_after_callback(पूर्णाङ्क call_update_screen, पूर्णाङ्क do_message)
{
    ++redrawing_for_callback;

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

    यदि (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_SETWSIZE || State == MODE_EXTERNCMD
	    || State == MODE_CONFIRM || exmode_active)
    {
	यदि (do_message)
	    repeat_message();
    }
    अन्यथा यदि (State & MODE_CMDLINE)
    {
	यदि (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	यदि (cmdline_row > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    यदि (सन्देश_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    अन्यथा यदि (State & (MODE_NORMAL | MODE_घुसाउ | MODE_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	यदि (सन्देश_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    सन्देश_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
#ifdef FEAT_GUI
    यदि (gui.in_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	out_flush_cursor(FALSE, FALSE);
    अन्यथा
#endif
	out_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    शून्य
redraw_later(पूर्णाङ्क type)
{
    redraw_win_later(curwin, type);
}

    शून्य
redraw_win_later(
    win_T	*wp,
    पूर्णाङ्क		type)
{
#ifdef FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    यदि (inside_redraw_on_start_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    अन्यथा
#endif
    यदि (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	यदि (type >= UPD_NOT_VALID)
	    wp->w_पङ्क्तिहरू_valid = 0;
	यदि (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    शून्य
redraw_later_clear(शून्य)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    शून्य
redraw_all_later(पूर्णाङ्क type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    set_must_redraw(type);
}

#यदि 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    शून्य
redraw_all_windows_later(पूर्णाङ्क type)
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
    शून्य
set_must_redraw(पूर्णाङ्क type)
{
    यदि (!redraw_not_allowed &&
#ifdef FEAT_EVAL
	    !inside_redraw_on_start_cb &&
#endif
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    शून्य
redraw_curbuf_later(पूर्णाङ्क type)
{
    redraw_buf_later(curbuf, type);
}

    शून्य
redraw_buf_later(buf_T *buf, पूर्णाङ्क type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	यदि (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    }
#ifdef FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	यदि (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	यदि (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
#endif
#यदि defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    यदि (curwin->w_buffer == buf)
	redraw_win_later(curwin, type);
#endif
}

#यदि defined(FEAT_SIGNS)
    शून्य
redraw_buf_पङ्क्ति_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	यदि (wp->w_buffer == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#endif

#यदि defined(FEAT_JOB_CHANNEL)
    शून्य
redraw_buf_and_status_later(buf_T *buf, पूर्णाङ्क type)
{
    win_T	*wp;

    यदि (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	फर्काऊ;
    FOR_ALL_WINDOWS(wp)
    {
	यदि (wp->w_buffer == buf)
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
    स्थिर शून्य
ruler_redraw_lastwin(शून्य)
{
    यदि (p_ru && lastwin->w_status_height == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    शून्य
status_redraw_all(शून्य)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	यदि (wp->w_status_height)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    शून्य
status_redraw_curbuf(शून्य)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	यदि (wp->w_status_height != 0 && wp->w_buffer == curbuf)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    यदि (lastwin->w_buffer == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    शून्य
redraw_statuslines(शून्य)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	यदि (wp->w_redr_status)
	{
	    bool ret = थप_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_status(wp, FALSE);
	    यदि (ret)
		झिक_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    यदि (redraw_tabline)
	draw_tabline();

#यदि defined(FEAT_TABPANEL)
    यदि (redraw_tabpanel)
	draw_tabpanel();
#endif
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    शून्य
win_redraw_last_status(frame_T *frp)
{
    यदि (frp->fr_layout == FR_LEAF)
	frp->fr_win->w_redr_status = true;
    अन्यथा यदि (frp->fr_layout == FR_ROW)
    {
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    win_redraw_last_status(frp);
    }
    अन्यथा // frp->fr_layout == FR_COL
    {
	frp = frp->fr_child;
	जबसम्म (frp->fr_next != रिक्त)
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
    शून्य
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    शून्य
redraw_win_range_later(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    यदि (last >= wp->w_topline && first < wp->w_botline)
    {
	यदि (wp->w_redraw_शीर्ष == 0 || wp->w_redraw_शीर्ष > first)
	    wp->w_redraw_शीर्ष = first;
	यदि (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    शून्य
redraw_win_range_now(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    यदि (last >= wp->w_topline && first < wp->w_botline)
    {
	यदि (wp->w_redraw_शीर्ष == 0 || wp->w_redraw_शीर्ष > first)
	    wp->w_redraw_शीर्ष = first;
	यदि (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	यदि (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#ifdef FEAT_EVAL
स्थिर bool redraw_cb_in_progress = false;

    शून्य
f_redraw_listener_add(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    स्थिर पूर्णाङ्क		id;

    यदि (check_secure())
	फर्काऊ;

    यदि (redraw_cb_in_progress)
    {
	emsg(_(e_cannot_add_redraw_listener_in_listener_callback));
	फर्काऊ;
    }

    यदि (check_for_dict_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    यदि (rln == रिक्त)
	फर्काऊ;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    यदि (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	यदि (cb.cb_name == रिक्त)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    फर्काऊ;
	}
	copy_callback(&rln->rl_callbacks.on_start, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    यदि (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	यदि (cb.cb_name == रिक्त)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_start);
	    vim_free(rln);
	    फर्काऊ;
	}
	copy_callback(&rln->rl_callbacks.on_end, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    यदि (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	फर्काऊ;
    }

    rln->rl_next = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_सङ्ख्या;
    rettv->vval.v_सङ्ख्या = id;

    फर्काऊ;
}

    स्थिर शून्य
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_start);
    free_callback(&rln->rl_callbacks.on_end);

    vim_free(rln);
}


    स्थिर शून्य
redraw_listener_cleanup(शून्य)
{
    लागि (redraw_listener_T *rln = redraw_listeners; rln != रिक्त;)
    {
	redraw_listener_T *next = rln->rl_next;
	यदि (rln->rl_id == 0)
	{
	    यदि (redraw_listeners == rln)
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
    स्थिर redraw_listener_T *
get_redraw_listener(पूर्णाङ्क id)
{
    लागि (redraw_listener_T *rln = redraw_listeners; rln != रिक्त; rln = rln->rl_next)
	यदि (rln->rl_id == id)
	    फर्काऊ rln;
    फर्काऊ रिक्त;
}

    शून्य
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv UNUSED)
{
    पूर्णाङ्क			id;
    redraw_listener_T	*rln;

    यदि (check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	फर्काऊ;

    id = argvars[0].vval.v_सङ्ख्या;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_सङ्ख्या;
    यदि (rln == रिक्त)
    {
	rettv->vval.v_सङ्ख्या = 0;
	फर्काऊ;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_सङ्ख्या = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    स्थिर शून्य
invoke_redraw_listener_start_or_end(bool start)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    यदि (start)
	inside_redraw_on_start_cb = true;

    redraw_cb_in_progress = true;
    लागि (redraw_listener_T *rln = redraw_listeners; rln != रिक्त; rln = rln->rl_next)
    {
	यदि (rln->rl_id == 0)
	    // Listener has been removed, skip
	    जारी;
	यदि (start && rln->rl_callbacks.on_start.cb_name != रिक्त)
	{
	    call_callback(&rln->rl_callbacks.on_start, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	अन्यथा यदि (rln->rl_callbacks.on_end.cb_name != रिक्त)
	{
	    call_callback(&rln->rl_callbacks.on_end, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_in_progress = false;

    यदि (start)
	inside_redraw_on_start_cb = false;
}
#endif // FEAT_EVAL
