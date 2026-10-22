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

#અન્તર્ભાવ "vim.h"

સ્થિર શૂન્ય win_update(win_T *wp);
#ifdef FEAT_STL_OPT
સ્થિર શૂન્ય redraw_custom_statusline(win_T *wp);
#endif
સ્થિર શૂન્ય borrow_stl_vsep_hl(શૂન્ય);
#જો defined(FEAT_શોધ_EXTRA) || defined(FEAT_CLIPBOARD)
સ્થિર પૂર્ણાંક  did_update_one_window;
#endif

#ifdef FEAT_EVAL
સ્થિર શૂન્ય redraw_listener_cleanup(શૂન્ય);
સ્થિર શૂન્ય invoke_redraw_listener_start_or_end(bool start);
#endif

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    પૂર્ણાંક
update_screen(પૂર્ણાંક type_arg)
{
    પૂર્ણાંક		type = type_arg;
    win_T	*wp;
    સ્થિર પૂર્ણાંક	did_intro = FALSE;
#ifdef FEAT_GUI
    પૂર્ણાંક		did_one = FALSE;
    પૂર્ણાંક		did_undraw = FALSE;
    પૂર્ણાંક		gui_cursor_col = 0;
    પૂર્ણાંક		gui_cursor_row = 0;
#endif
    પૂર્ણાંક		no_update = FALSE;
    પૂર્ણાંક		save_pum_will_redraw = pum_will_redraw;
#ifdef FEAT_PROP_POPUP
    પૂર્ણાંક		did_redraw_window = FALSE;
#endif
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    જો (!screen_valid(TRUE))
	પાછા FAIL;

    જો (type == UPD_VALID_NO_UPDATE)
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
    જો (need_diff_redraw)
	diff_redraw(TRUE);
#endif

    જો (must_redraw)
    {
	જો (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    જો (curwin->w_લીટીઓ_valid == 0 && type < UPD_NOT_VALID
#ifdef FEAT_TERMINAL
	    && !term_do_update_window(curwin)
#endif
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    જો (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	જો (type > UPD_INVERTED_ALL)
	    curwin->w_લીટીઓ_valid = 0;	// don't use w_lines[].wl_size now
	પાછા FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    પૂર્ણાંક hid_cursor = !sync_output_active();
    જો (hid_cursor)
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
    જો (no_update)
	++no_win_do_લીટીઓ_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    જો (સંદેશ_scrolled)
    {
	clear_cmdline = TRUE;
	જો (type != UPD_CLEAR)
	{
	    જો (સંદેશ_scrolled > Rows - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    નહીંતર
	    {
		check_for_delay(FALSE);
		જો (screen_ins_લીટીઓ(0, 0, સંદેશ_scrolled, (પૂર્ણાંક)Rows, 0, ખાલી)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    જો (wp->w_winrow < સંદેશ_scrolled)
		    {
			જો (W_WINROW(wp) + wp->w_height > સંદેશ_scrolled
				&& wp->w_redr_type < UPD_REDRAW_ટોચ
				&& wp->w_લીટીઓ_valid > 0
				&& wp->w_topline == wp->w_લીટીઓ[0].wl_lnum)
			{
			    wp->w_upd_rows = સંદેશ_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_ટોચ;
			}
			નહીંતર
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    જો (W_WINROW(wp) + wp->w_height
					 + wp->w_status_height <= સંદેશ_scrolled)
				wp->w_redr_status = true;
			}
		    }
		}
		જો (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#જો defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#endif
	}
	સંદેશ_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    જો (need_highlight_changed)
	highlight_changed();

    જો (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    જો (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    જો (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? સંખ્યા_width(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#endif

#ifdef FEAT_EVAL
    invoke_redraw_listener_start_or_end(true);
#endif

    // Only start redrawing if there is really something to do.
    જો (type == UPD_INVERTED)
	update_curswant();
    જો (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_લીટીઓ[0].wl_valid
#ifdef FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#endif
		    && curwin->w_topline == curwin->w_લીટીઓ[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_active
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_mode == VIsual_mode
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    જો (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#જો defined(FEAT_TABPANEL)
    જો (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#endif

#ifdef FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	જો (wp->w_buffer->b_mod_set)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    માટે (wwp = firstwin; wwp != wp; wwp = wwp->w_next)
		જો (wwp->w_buffer == wp->w_buffer)
		    તોડો;
	    જો (wwp == wp && syntax_present(wp))
		syn_ઢગલો_apply_changes(wp->w_buffer);
	}
    }
#endif

    જો (pum_redraw_in_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#જો defined(FEAT_શોધ_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_window = FALSE;
#endif
#ifdef FEAT_શોધ_EXTRA
    screen_શોધ_hl.rm.regprog = ખાલી;
#endif
    FOR_ALL_WINDOWS(wp)
    {
	override_success = ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);

	જો (wp->w_redr_type != 0)
	{
	    cursor_off();
#ifdef FEAT_PROP_POPUP
	    did_redraw_window = TRUE;
#endif
#ifdef FEAT_GUI
	    જો (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		જો (gui.in_use && (wp == curwin || cursor_is_sleeping()))
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
	જો (wp->w_redr_status)
	{
	    cursor_off();
	    win_redr_status(wp, TRUE); // any popup menu will be redrawn below
	}

	જો (override_success)
	    કાઢો_highlight_overrides();

    }
#જો defined(FEAT_શોધ_EXTRA)
    end_શોધ_hl();
#endif

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    જો (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    જો (wp->w_vsep_width > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_buffer->b_mod_set = false;

#ifdef FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    જો (did_redraw_window || popup_need_redraw())
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
    જો (clear_cmdline || redraw_cmdline || redraw_mode)
	showmode();

    જો (no_update)
	--no_win_do_લીટીઓ_ins;

    // May put up an introductory message when not editing a file
    જો (!did_intro)
	maybe_intro_message();
    did_intro = TRUE;

#ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    જો (gui.in_use)
    {
	જો (did_undraw && !gui_mch_is_blink_off())
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
	નહીંતર
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
    જો (hid_cursor)
	cursor_on();

    પાછા OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    પૂર્ણાંક
statusline_row(win_T *wp)
{
#જો defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    જો (wp->w_frame->fr_height == wp->w_status_height && !popup_is_popup(wp))
	પાછા wp->w_winrow;
#endif
    પાછા W_WINROW(wp) + wp->w_height;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    શૂન્ય
win_redr_status(win_T *wp, પૂર્ણાંક ignore_pum UNUSED)
{
    પૂર્ણાંક		row;
    પૂર્ણાંક		fillchar;
    પૂર્ણાંક		attr;
    પૂર્ણાંક		i;
    સ્થિર પૂર્ણાંક  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    જો (busy)
	પાછા;
    busy = TRUE;

    row = statusline_row(wp);

    wp->w_redr_status = false;
    જો (wp->w_status_height == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    નહીંતર જો (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_status = true;
    }
#ifdef FEAT_STL_OPT
    નહીંતર જો (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#endif
    નહીંતર
    {
	char_u	*p;
	પૂર્ણાંક	plen;
	પૂર્ણાંક	NameBufflen;
	પૂર્ણાંક	this_ru_col;
	પૂર્ણાંક	n;			// scratch value

	fillchar = fillchar_status(&attr, wp);

	get_trans_bufname(wp->w_buffer);
	p = NameBuff;
	plen = (પૂર્ણાંક)STRLEN(p);

	જો ((bt_help(wp->w_buffer)
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
	જો (bt_help(wp->w_buffer))
	    plen += vim_snprintf((અક્ષર *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#ifdef FEAT_QUICKFIX
	જો (wp->w_p_pvw)
	    plen += vim_snprintf((અક્ષર *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#endif
	જો (bufIsChanged(wp->w_buffer) && !bt_terminal(wp->w_buffer))
	    plen += vim_snprintf((અક્ષર *)p + plen, MAXPATHL - plen, "%s", "[+]");
	જો (wp->w_buffer->b_p_ro)
	    plen += vim_snprintf((અક્ષર *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_col = ru_col - (cmdline_width - wp->w_width);
	n = (wp->w_width + 1) / 2;
	જો (this_ru_col < n)
	    this_ru_col = n;
	જો (this_ru_col <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	નહીંતર જો (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    માટે (i = 0; p[i] != NUL && plen >= this_ru_col - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    જો (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	નહીંતર જો (plen > this_ru_col - 1)
	{
	    p += plen - (this_ru_col - 1);
	    *p = '<';
	    plen = this_ru_col - 1;
	}

	screen_puts(p, row, wp->w_wincol, attr);
	screen_fill(row, row + 1, plen + wp->w_wincol,
		    this_ru_col + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	માટે (i = 1; i < wp->w_status_height; i++)
	    screen_fill(row + i, row + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	જો ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_col - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, row, (પૂર્ણાંક)(this_ru_col - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	જો (p_sc && *p_sloc == 's')
	{
	    n = this_ru_col - plen - 2;		    // perform the calculation here so we only do it once
	    પૂર્ણાંક	width = MIN(10, n);

	    જો (width > 0)
		screen_puts_len(showcmd_buf, width, row,
				wp->w_wincol + this_ru_col - width - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    જો (wp->w_vsep_width != 0 && wp->w_status_height != 0 && redrawing())
    {
	માટે (i = 0; i < wp->w_status_height; i++)
	{
	    પૂર્ણાંક r = row + i;
	    જો (stl_connected(wp))
		fillchar = fillchar_status(&attr, wp);
	    નહીંતર
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
    સ્થિર શૂન્ય
borrow_stl_vsep_hl(શૂન્ય)
{
    win_T   *ડાબું = ખાલી;
    win_T   *જમણું = ખાલી;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    જો (!redrawing() || Screen_લીટીઓ == ખાલી)
	પાછા;

    FOR_ALL_WINDOWS(ડાબું)
    {
	જો (ડાબું->w_status_height == 0 || ડાબું->w_vsep_width == 0)
	    ચાલુ;
	// A window of zero width has no status line cell to borrow from.
	જો (ડાબું->w_width == 0)
	    ચાલુ;
	જો (!stl_connected(ડાબું))
	    ચાલુ;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = ખાલી;
	પૂર્ણાંક start = 0;
	પૂર્ણાંક end = 0;

	FOR_ALL_WINDOWS(જમણું)
	{
	    જો (જમણું == ડાબું || જમણું->w_status_height == 0)
		ચાલુ;
	    જો (જમણું->w_wincol != W_ENDCOL(ડાબું) + 1)
		ચાલુ;
	    પૂર્ણાંક l_stl_row = W_WINROW(ડાબું) + ડાબું->w_height;
	    પૂર્ણાંક r_stl_row = W_WINROW(જમણું) + જમણું->w_height;

	    start = l_stl_row > r_stl_row ? l_stl_row : r_stl_row;
	    end = l_stl_row + ડાબું->w_status_height
				< r_stl_row + જમણું->w_status_height
		? l_stl_row + ડાબું->w_status_height
		: r_stl_row + જમણું->w_status_height;
	    જો (start < end)
	    {
		neighbour = જમણું;
		તોડો;
	    }
	}
	જો (neighbour == ખાલી)
	    ચાલુ;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	પૂર્ણાંક	hl;
	જો (ડાબું != curwin && neighbour != curwin
		&& fillchar_status(&hl, ડાબું) != ' ')
	    ચાલુ;

	// Source: prefer curwin's side; otherwise left window's right edge.
	પૂર્ણાંક dst_col = W_ENDCOL(ડાબું);
	પૂર્ણાંક src_col = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(ડાબું) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	જો (dst_col >= screen_Columns || src_col >= screen_Columns)
	    ચાલુ;
	જો (end > screen_Rows)
	    end = screen_Rows;

	માટે (પૂર્ણાંક r = start; r < end; r++)
	{
	    unsigned	dst_off = લીટી_Offset[r] + dst_col;
	    sattr_T	attr = ScreenAttrs[લીટી_Offset[r] + src_col];

	    જો (ScreenAttrs[dst_off] != attr)
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
    સ્થિર શૂન્ય
redraw_custom_statusline(win_T *wp)
{
    સ્થિર પૂર્ણાંક	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    જો (entered)
	પાછા;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#endif

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    શૂન્ય
showruler(પૂર્ણાંક always)
{
    જો (!always && !redrawing())
	પાછા;
    જો (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_status = true;
	પાછા;
    }
#જો defined(FEAT_STL_OPT)
    જો ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_status_height)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    નહીંતર
#endif
	win_redr_ruler(curwin, always, FALSE);

    જો (need_maketitle
#ifdef FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_IN_ICON))
	    || (p_title && (stl_syntax & STL_IN_TITLE))
#endif
       )
	maketitle();

    // Redraw the tab pages line if needed.
    જો (redraw_tabline)
	draw_tabline();

#જો defined(FEAT_TABPANEL)
    જો (redraw_tabpanel)
	draw_tabpanel();
#endif
}

    શૂન્ય
win_redr_ruler(win_T *wp, પૂર્ણાંક always, પૂર્ણાંક ignore_pum)
{
    પૂર્ણાંક	empty_લીટી = FALSE;

    // If 'ruler' off don't do anything
    જો (!p_ru)
	પાછા;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    જો (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
	પાછા;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    જો (wp == lastwin && lastwin->w_status_height == 0)
	જો (edit_submode != ખાલી)
	    પાછા;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    જો (!ignore_pum && pum_visible())
	પાછા;

#ifdef FEAT_STL_OPT
    જો (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	પાછા;
    }
#endif

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    જો ((State & MODE_ઉમેરો) == 0
		&& *ml_get_buf(wp->w_buffer, wp->w_cursor.lnum, FALSE) == NUL)
	empty_લીટી = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    જો (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.col != wp->w_ru_cursor.col
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_buffer->b_ml.ml_લીટી_ગણતરી != wp->w_ru_લીટી_ગણતરી
#ifdef FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#endif
	    || empty_લીટી != wp->w_ru_empty)
    {
	પૂર્ણાંક	row;
	પૂર્ણાંક	fillchar;
	પૂર્ણાંક	attr;
	પૂર્ણાંક	off;
	પૂર્ણાંક	width;
	colnr_T	virtcol;
#define RULER_BUF_LEN 70
	char_u	buffer[RULER_BUF_LEN];
	પૂર્ણાંક	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	પૂર્ણાંક	rel_poslen;
	પૂર્ણાંક	this_ru_col;
	પૂર્ણાંક	n1;			    // scratch value
	પૂર્ણાંક	n2;			    // scratch value
	bool	override_success =
	    ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	જો (wp->w_status_height)
	{
	    row = statusline_row(wp);
	    fillchar = fillchar_status(&attr, wp);
	    off = wp->w_wincol;
	    width = wp->w_width;
	}
	નહીંતર
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_col_off;
	    width = cmdline_width;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	જો (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, ખાલી, &virtcol, ખાલી, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((અક્ષર *)buffer, RULER_BUF_LEN, _("%ld,"),
		(wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		    ? 0L
		    : (long)(wp->w_cursor.lnum));
	bufferlen += col_print(buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			empty_લીટી ? 0 : (પૂર્ણાંક)wp->w_cursor.col + 1,
			(પૂર્ણાંક)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	જો (wp->w_status_height == 0)	// can't use last char of screen
	    ++n1;

	this_ru_col = ru_col - (cmdline_width - width);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (width + 1) / 2;
	જો (this_ru_col < n2)
	    this_ru_col = n2;
	જો (this_ru_col + n1 < width)
	{
	    // need at least space for rel_pos + NUL
	    જ્યારે (this_ru_col + n1 < width
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		જો (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, buffer + bufferlen);
		નહીંતર
		    buffer[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((અક્ષર *)buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	જો (has_mbyte)
	{
	    માટે (n1 = 0, n2 = 0; buffer[n1] != NUL; n1 += (*mb_ptr2len)(buffer + n1))
	    {
		n2 += (*mb_ptr2cells)(buffer + n1);
		જો (this_ru_col + n2 > width)
		{
		    bufferlen = n1;
		    buffer[bufferlen] = NUL;
		    તોડો;
		}
	    }
	}
	નહીંતર જો (this_ru_col + bufferlen > width)
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
	wp->w_ru_empty = empty_લીટી;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_લીટી_ગણતરી = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
#ifdef FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#endif

	જો (override_success)
	    કાઢો_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    શૂન્ય
after_updating_screen(પૂર્ણાંક may_resize_shell UNUSED)
{
    updating_screen = FALSE;
#ifdef FEAT_GUI
    જો (may_resize_shell)
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
    શૂન્ય
update_curbuf(પૂર્ણાંક type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#જો defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    સ્થિર પૂર્ણાંક
text_to_screenline(win_T *wp, char_u *text, પૂર્ણાંક col)
{
    પૂર્ણાંક		off = (પૂર્ણાંક)(current_Screen_લીટી - Screen_લીટીઓ);

    જો (has_mbyte)
    {
	પૂર્ણાંક	cells;
	પૂર્ણાંક	u8c, u8cc[MAX_MCO];
	પૂર્ણાંક	i;
	પૂર્ણાંક	idx;
	પૂર્ણાંક	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	પૂર્ણાંક	prev_c = 0;		// previous Arabic character
	પૂર્ણાંક	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	જો (wp->w_p_rl)
	    idx = off;
	નહીંતર
# endif
	    idx = off + col;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	માટે (p = text; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    જો (col + cells > wp->w_width)
		તોડો;
# ifdef FEAT_RIGHTLEFT
	    જો (wp->w_p_rl)
		idx = off + wp->w_width - col - cells;
# endif
	    Screen_લીટીઓ[idx] = *p;
	    જો (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		જો (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_લીટીઓ_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    prev_c = u8c;
# endif
		}
		નહીંતર
		{
# ifdef FEAT_ARABIC
		    જો (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			પૂર્ણાંક	pc, pc1, nc;
			પૂર્ણાંક	pcc[MAX_MCO];
			પૂર્ણાંક	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			જો (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			નહીંતર
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			Screen_લીટીઓ[idx] = firstbyte;
		    }
		    નહીંતર
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_લીટીઓ_UC[idx] = u8c;
		    માટે (i = 0; i < Screen_mco; ++i)
		    {
			Screen_લીટીઓ_C[i][idx] = u8cc[i];
			જો (u8cc[i] == 0)
			    તોડો;
		    }
		}
		જો (cells > 1)
		    Screen_લીટીઓ[idx + 1] = 0;
	    }
	    નહીંતર જો (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    નહીંતર જો (cells > 1)
		// double-width character
		Screen_લીટીઓ[idx + 1] = p[1];
	    col += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    નહીંતર
    {
	પૂર્ણાંક len = (પૂર્ણાંક)STRLEN(text);
	પૂર્ણાંક n = wp->w_width - col;

	જો (len > n)
	    len = n;
	જો (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    જો (wp->w_p_rl)
		mch_memmove(current_Screen_લીટી, text, len);
	    નહીંતર
# endif
		mch_memmove(current_Screen_લીટી + col, text, len);
	    col += len;
	}
    }
    પાછા col;
}
#endif

#ifdef FEAT_MENU
/*
 * Draw the window toolbar.
 */
    સ્થિર શૂન્ય
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    પૂર્ણાંક		item_idx = 0;
    પૂર્ણાંક		item_ગણતરી = 0;
    પૂર્ણાંક		col = 0;
    પૂર્ણાંક		next_col;
    પૂર્ણાંક		off = (પૂર્ણાંક)(current_Screen_લીટી - Screen_લીટીઓ);
    bool	override_success =
	ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);
    પૂર્ણાંક		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    પૂર્ણાંક		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_વસ્તુઓ);
    FOR_ALL_CHILD_MENUS(wp->w_winbar, menu)
	++item_ગણતરી;
    wp->w_winbar_વસ્તુઓ = ALLOC_CLEAR_MULT(winbar_item_T, item_ગણતરી + 1);

    // TODO: use fewer spaces if there is not enough room
    માટે (menu = wp->w_winbar->children;
			  menu != ખાલી && col < wp->w_width; menu = menu->next)
    {
	space_to_screenline(off + col, fill_attr);
	જો (++col >= wp->w_width)
	    તોડો;
	જો (col > 1)
	{
	    space_to_screenline(off + col, fill_attr);
	    જો (++col >= wp->w_width)
		તોડો;
	}

	wp->w_winbar_વસ્તુઓ[item_idx].wb_startcol = col;
	space_to_screenline(off + col, button_attr);
	જો (++col >= wp->w_width)
	    તોડો;

	next_col = text_to_screenline(wp, menu->name, col);
	જ્યારે (col < next_col)
	{
	    ScreenAttrs[off + col] = button_attr;
	    ++col;
	}
	wp->w_winbar_વસ્તુઓ[item_idx].wb_endcol = col;
	wp->w_winbar_વસ્તુઓ[item_idx].wb_menu = menu;
	++item_idx;

	જો (col >= wp->w_width)
	    તોડો;
	space_to_screenline(off + col, button_attr);
	++col;
    }
    જ્યારે (col < wp->w_width)
    {
	space_to_screenline(off + col, fill_attr);
	++col;
    }
    wp->w_winbar_વસ્તુઓ[item_idx].wb_menu = ખાલી; // end marker

    screen_લીટી(wp, wp->w_winrow, wp->w_wincol, wp->w_width,
							  wp->w_width, -1, 0);

    જો (override_success)
	કાઢો_highlight_overrides();
}
#endif

#જો defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    સ્થિર શૂન્ય
copy_text_attr(
    પૂર્ણાંક		off,
    char_u	*buf,
    પૂર્ણાંક		len,
    પૂર્ણાંક		attr)
{
    પૂર્ણાંક		i;

    mch_memmove(Screen_લીટીઓ + off, buf, (size_t)len);
    જો (enc_utf8)
	vim_memset(Screen_લીટીઓ_UC + off, 0, કદ(u8char_T) * (size_t)len);
    માટે (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    સ્થિર શૂન્ય
fold_લીટી(
    win_T	*wp,
    long	fold_ગણતરી,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    પૂર્ણાંક		row)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*ટોચ, *bot;
    linenr_T	lnume = lnum + fold_ગણતરી - 1;
    પૂર્ણાંક		len;
    char_u	*text;
    પૂર્ણાંક		fdc;
    પૂર્ણાંક		col;
    પૂર્ણાંક		txtcol;
    પૂર્ણાંક		off = (પૂર્ણાંક)(current_Screen_લીટી - Screen_લીટીઓ);
    પૂર્ણાંક		ri;

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
    જો (wp == cmdwin_win)
    {
	Screen_લીટીઓ[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	જો (enc_utf8)
	    Screen_લીટીઓ_UC[off] = 0;
	++col;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    કરો { \
	જો (wp->w_p_rl) \
	    માટે (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_width - (p) - (l)) + ri] = v; \
	 નહીંતર \
	    માટે (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } જ્યારે (0)
# નહીંતર
#  define RL_MEMSET(p, v, l) \
    કરો { \
	માટે (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } જ્યારે (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, col);
    જો (fdc > 0)
    {
	char_u	*p;
	પૂર્ણાંક	i;
	પૂર્ણાંક	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	માટે (i = 0; i < fdc; i++)
	{
	    પૂર્ણાંક		ch;

	    જો (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    નહીંતર
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    જો (wp->w_p_rl)
		idx = off + wp->w_width - i - 1 - col;
	    નહીંતર
# endif
		idx = off + col + i;
	    જો (enc_utf8)
	    {
		જો (ch >= 0x80)
		{
		    Screen_લીટીઓ_UC[idx] = ch;
		    Screen_લીટીઓ_C[0][idx] = 0;
		    Screen_લીટીઓ[idx] = 0x80;
		}
		નહીંતર
		{
		    Screen_લીટીઓ[idx] = ch;
		    Screen_લીટીઓ_UC[idx] = 0;
		}
	    }
	    નહીંતર
		Screen_લીટીઓ[idx] = ch;
	}

	RL_MEMSET(col, HL_ATTR(HLF_FC), fdc);
	col += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    // text
    RL_MEMSET(col, HL_ATTR(HLF_FL), wp->w_width - col);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    જો (signcolumn_on(wp))
    {
	len = wp->w_width - col;
	જો (len > 0)
	{
	    જો (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    જો (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    નહીંતર
#  endif
		copy_text_attr(off + col, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    col += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    જો (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_width - col;
	જો (len > 0)
	{
	    પૂર્ણાંક	    w = સંખ્યા_width(wp);
	    long    અંક;
	    અક્ષર    *fmt = "%*ld ";

	    જો (len > w + 1)
		len = w + 1;

	    જો (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		અંક = (long)lnum;
	    નહીંતર
	    {
		// 'relativenumber', don't use negative numbers
		અંક = labs((long)get_cursor_rel_lnum(wp, lnum));
		જો (અંક == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    અંક = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((અક્ષર *)buf, કદ(buf), fmt, w, અંક);
# ifdef FEAT_RIGHTLEFT
	    જો (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col, buf, len,
							     HL_ATTR(HLF_FL));
	    નહીંતર
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
    જ્યારે (col < wp->w_width)
    {
	પૂર્ણાંક c = wp->w_fill_chars.fold;
	પૂર્ણાંક idx = off + col;

# ifdef FEAT_RIGHTLEFT
	જો (wp->w_p_rl)
	    idx = off + wp->w_width - 1 - col;
# endif

	જો (enc_utf8)
	{
	    જો (c >= 0x80)
	    {
		Screen_લીટીઓ_UC[idx] = c;
		Screen_લીટીઓ_C[0][idx] = 0;
		Screen_લીટીઓ[idx] = 0x80; // avoid storing zero
	    }
	    નહીંતર
	    {
		Screen_લીટીઓ_UC[idx] = 0;
		Screen_લીટીઓ[idx] = c;
	    }
	}
	નહીંતર
	    Screen_લીટીઓ[idx] = c;
	++col;
    }

    જો (text != buf)
	vim_free(text);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    જો (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
	જો (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    ટોચ = &curwin->w_cursor;
	    bot = &VIsual;
	}
	નહીંતર
	{
	    // Visual is before curwin->w_cursor
	    ટોચ = &VIsual;
	    bot = &curwin->w_cursor;
	}
	જો (lnum >= ટોચ->lnum
		&& lnume <= bot->lnum
		&& (VIsual_mode != 'v'
		    || ((lnum > ટોચ->lnum
			    || (lnum == ટોચ->lnum
				&& ટોચ->col == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->col - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_buffer, lnume))))))
	{
	    જો (VIsual_mode == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		જો (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_width)
		{
		    જો (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_width)
			len = wp->w_old_cursor_lcol;
		    નહીંતર
			len = wp->w_width - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (પૂર્ણાંક)wp->w_old_cursor_fcol);
		}
	    }
	    નહીંતર
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_width - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    જો (wp->w_p_cc_cols)
    {
	પૂર્ણાંક i = 0;
	પૂર્ણાંક j = wp->w_p_cc_cols[i];
	પૂર્ણાંક old_txtcol = txtcol;

	જ્યારે (j > -1)
	{
	    txtcol += j;
	    જો (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    નહીંતર
		txtcol -= wp->w_leftcol;
	    જો (txtcol >= 0 && txtcol < wp->w_width)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_cols[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    જો (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	જો (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	નહીંતર
	    txtcol -= wp->w_leftcol;
	જો (txtcol >= 0 && txtcol < wp->w_width)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_લીટી(wp, row + W_WINROW(wp), wp->w_wincol, wp->w_width, wp->w_width,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    જો (wp == curwin
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
    જો (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_લીટી(wp))
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
    સ્થિર શૂન્ય
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_buffer;
    પૂર્ણાંક		type;
    પૂર્ણાંક		ટોચ_end = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    પૂર્ણાંક		mid_start = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    પૂર્ણાંક		mid_end = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    પૂર્ણાંક		bot_start = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    પૂર્ણાંક		scrolled_down = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    પૂર્ણાંક		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //લીટીઓ
#ifdef FEAT_શોધ_EXTRA
    પૂર્ણાંક		ટોચ_to_mod = FALSE;    // redraw above mod_top
#endif

    પૂર્ણાંક		row;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    પૂર્ણાંક		idx;		// current index in w_lines[]
    પૂર્ણાંક		srow;		// starting row of the current line

    પૂર્ણાંક		eof = FALSE;	// if TRUE, we hit the end of the file
    પૂર્ણાંક		didline = FALSE; // if TRUE, we finished the last line
    પૂર્ણાંક		i;
    long	j;
    સ્થિર પૂર્ણાંક	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#ifdef FEAT_CONCEAL
    પૂર્ણાંક		old_wrow = wp->w_wrow;
    પૂર્ણાંક		old_wcol = wp->w_wcol;
#endif
#ifdef FEAT_FOLDING
    long	fold_ગણતરી;
#endif
#ifdef FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_લીટી 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    પૂર્ણાંક		did_update = DID_NONE;
    linenr_T	syntax_last_parsed = 0;		// last parsed text line
#endif
    linenr_T	mod_ટોચ = 0;
    linenr_T	mod_bot = 0;
#જો defined(FEAT_SYN_HL) || defined(FEAT_શોધ_EXTRA)
    પૂર્ણાંક		save_got_int;
#endif
    bool	override_success;

#જો defined(FEAT_શોધ_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    જો (!did_update_one_window)
    {
	did_update_one_window = TRUE;
# ifdef FEAT_શોધ_EXTRA
	start_શોધ_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	જો (clip_star.available && clip_isautosel_star())
	    clip_update_selection(&clip_star);
	જો (clip_plus.available && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#endif

#ifdef FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    જો (wp->w_p_cuc)
	validate_virtcol_win(wp);
#endif

    type = wp->w_redr_type;

    જો (type == UPD_NOT_VALID)
    {
	wp->w_redr_status = true;
	wp->w_લીટીઓ_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    જો (wp->w_height + WINBAR_HEIGHT(wp) == 0
	    || (wp->w_frame->fr_height == wp->w_status_height
#જો defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#endif
	       ))
    {
	wp->w_redr_type = 0;
	પાછા;
    }

    // Window is zero-width: Only need to draw the separator.
    જો (wp->w_width == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	પાછા;
    }

    override_success = ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);


#ifdef FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    જો (term_do_update_window(wp))
    {
	term_update_window(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	જો (winbar_height(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	જો (override_success)
	    કાઢો_highlight_overrides();
	પાછા;
    }
#endif

#ifdef FEAT_શોધ_EXTRA
    init_શોધ_hl(wp, &screen_શોધ_hl);
#endif

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    જો (wp->w_skipcol > 0 && wp->w_width > win_col_off(wp))
    {
	પૂર્ણાંક w = 0;
	પૂર્ણાંક width1 = wp->w_width - win_col_off(wp);
	પૂર્ણાંક width2 = width1 + win_col_off2(wp);
	પૂર્ણાંક add = width1;

	જ્યારે (w < wp->w_skipcol)
	{
	    જો (w > 0)
		add = width2;
	    w += add;
	}
	જો (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - add;
    }

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? સંખ્યા_width(wp) : 0;
    જો (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    નહીંતર
#endif

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_ટોચ = wp->w_redraw_ટોચ;
	જો (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	નહીંતર
	    mod_bot = 0;
	જો (buf->b_mod_set)
	{
	    જો (mod_ટોચ == 0 || mod_ટોચ > buf->b_mod_ટોચ)
	    {
		mod_ટોચ = buf->b_mod_ટોચ;
#ifdef FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		જો (syntax_present(wp))
		{
		    mod_ટોચ -= buf->b_s.b_syn_sync_linebreaks;
		    જો (mod_ટોચ < 1)
			mod_ટોચ = 1;
		}
#endif
	    }
	    જો (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ifdef FEAT_શોધ_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    જો (screen_શોધ_hl.rm.regprog != ખાલી
		    && re_multiline(screen_શોધ_hl.rm.regprog))
		ટોચ_to_mod = TRUE;
	    નહીંતર
	    {
		matchitem_T *cur = wp->w_match_head;

		જ્યારે (cur != ખાલી)
		{
		    જો (cur->mit_match.regprog != ખાલી
				       && re_multiline(cur->mit_match.regprog))
		    {
			ટોચ_to_mod = TRUE;
			તોડો;
		    }
		    cur = cur->mit_next;
		}
	    }
#endif
	}

#ifdef FEAT_શોધ_EXTRA
	જો (શોધ_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    જો (mod_ટોચ == 0 || mod_ટોચ > શોધ_hl_has_cursor_lnum)
		mod_ટોચ = શોધ_hl_has_cursor_lnum;
	    જો (mod_bot == 0 || mod_bot < શોધ_hl_has_cursor_lnum + 1)
		mod_bot = શોધ_hl_has_cursor_lnum + 1;
	}
#endif

#ifdef FEAT_FOLDING
	જો (mod_ટોચ != 0 && hasAnyFolding(wp))
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
	    માટે (i = 0; i < wp->w_લીટીઓ_valid; ++i)
		જો (wp->w_લીટીઓ[i].wl_valid)
		{
		    જો (wp->w_લીટીઓ[i].wl_lastlnum < mod_ટોચ)
			lnumt = wp->w_લીટીઓ[i].wl_lastlnum + 1;
		    જો (lnumb == MAXLNUM && wp->w_લીટીઓ[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_લીટીઓ[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			જો (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (શૂન્ય)hasFoldingWin(wp, mod_ટોચ, &mod_ટોચ, ખાલી, TRUE, ખાલી);
	    જો (mod_ટોચ > lnumt)
		mod_ટોચ = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (શૂન્ય)hasFoldingWin(wp, mod_bot, ખાલી, &mod_bot, TRUE, ખાલી);
	    ++mod_bot;
	    જો (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#endif

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	જો (mod_ટોચ != 0 && mod_ટોચ < wp->w_topline)
	{
	    જો (mod_bot > wp->w_topline)
		mod_ટોચ = wp->w_topline;
#ifdef FEAT_SYN_HL
	    નહીંતર જો (syntax_present(wp))
		ટોચ_end = 1;
#endif
	}
    }
    wp->w_redraw_ટોચ = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#ifdef FEAT_શોધ_EXTRA
    શોધ_hl_has_cursor_lnum = 0;
#endif


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    જો (type == UPD_REDRAW_ટોચ)
    {
	j = 0;
	માટે (i = 0; i < wp->w_લીટીઓ_valid; ++i)
	{
	    j += wp->w_લીટીઓ[i].wl_size;
	    જો (j >= wp->w_upd_rows)
	    {
		ટોચ_end = j;
		તોડો;
	    }
	}
	જો (ટોચ_end == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	નહીંતર
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    જો (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    જો ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#ifdef FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#endif
	    )
    {
	જો (mod_ટોચ != 0
		&& wp->w_topline == mod_ટોચ
		&& (!wp->w_લીટીઓ[0].wl_valid
		    || wp->w_topline == wp->w_લીટીઓ[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	નહીંતર જો (wp->w_લીટીઓ[0].wl_valid
		&& (wp->w_topline < wp->w_લીટીઓ[0].wl_lnum
#ifdef FEAT_DIFF
		    || (wp->w_topline == wp->w_લીટીઓ[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#endif
		   ))
	{
	    // New topline is above old topline: May scroll down.
#ifdef FEAT_FOLDING
	    જો (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		માટે (ln = wp->w_topline; ln < wp->w_લીટીઓ[0].wl_lnum; ++ln)
		{
		    ++j;
		    જો (j >= wp->w_height - 2)
			તોડો;
		    (શૂન્ય)hasFoldingWin(wp, ln, ખાલી, &ln, TRUE, ખાલી);
		}
	    }
	    નહીંતર
#endif
		j = wp->w_લીટીઓ[0].wl_lnum - wp->w_topline;
	    જો (j < wp->w_height - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_લીટીઓ[0].wl_lnum - 1,
								wp->w_height);
#ifdef FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		જો (wp->w_લીટીઓ[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_લીટીઓ[0].wl_lnum)
							  - wp->w_old_topfill;
#endif
		જો (i < wp->w_height - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    જો (i > 0)
			check_for_delay(FALSE);
		    જો (win_ins_લીટીઓ(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			જો (wp->w_લીટીઓ_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    ટોચ_end = i;
			    scrolled_down = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    જો ((wp->w_લીટીઓ_valid += j) > wp->w_height)
				wp->w_લીટીઓ_valid = wp->w_height;
			    માટે (idx = wp->w_લીટીઓ_valid; idx - j >= 0; idx--)
				wp->w_લીટીઓ[idx] = wp->w_લીટીઓ[idx - j];
			    જ્યારે (idx >= 0)
				wp->w_લીટીઓ[idx--].wl_valid = FALSE;
			}
		    }
		    નહીંતર
			mid_start = 0;		// redraw all lines
		}
		નહીંતર
		    mid_start = 0;		// redraw all lines
	    }
	    નહીંતર
		mid_start = 0;		// redraw all lines
	}
	નહીંતર
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    row = 0;
	    માટે (i = 0; i < wp->w_લીટીઓ_valid && i < Rows; i++)
	    {
		જો (wp->w_લીટીઓ[i].wl_valid
			&& wp->w_લીટીઓ[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    તોડો;
		}
		row += wp->w_લીટીઓ[i].wl_size;
	    }
	    જો (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//લીટીઓ
		mid_start = 0;
	    }
	    નહીંતર
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#ifdef FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		જો (wp->w_લીટીઓ[0].wl_lnum == wp->w_topline)
		    row += wp->w_old_topfill;
		નહીંતર
		    row += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		row -= wp->w_topfill;
#endif
		જો (row > Rows)  // just in case
		    row = Rows;
		જો (row > 0)
		{
		    check_for_delay(FALSE);
		    જો (win_del_લીટીઓ(wp, 0, row, FALSE, wp == firstwin, 0)
									 == OK)
			bot_start = wp->w_height - row;
		    નહીંતર
			mid_start = 0;		// redraw all lines
		}
		જો ((row == 0 || bot_start < 999) && wp->w_લીટીઓ_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_start = 0;
		    idx = 0;
		    માટે (;;)
		    {
			wp->w_લીટીઓ[idx] = wp->w_લીટીઓ[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			જો (row > 0 && bot_start + row
				 + (પૂર્ણાંક)wp->w_લીટીઓ[j].wl_size > wp->w_height)
			{
			    wp->w_લીટીઓ_valid = idx + 1;
			    તોડો;
			}
			bot_start += wp->w_લીટીઓ[idx++].wl_size;

			// stop at the last valid entry in w_lines[].wl_size
			જો (++j >= wp->w_લીટીઓ_valid)
			{
			    wp->w_લીટીઓ_valid = idx;
			    તોડો;
			}
		    }
#ifdef FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    જો (wp->w_p_diff && bot_start > 0)
			wp->w_લીટીઓ[0].wl_size = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#endif
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	જો (mid_start == 0)
	    mid_end = wp->w_height;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	જો (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    નહીંતર
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
    }

    જો (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    જો ((VIsual_active && buf == curwin->w_buffer)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, to;

	જો (VIsual_active)
	{
	    જો (VIsual_mode != wp->w_old_visual_mode
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		જો (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = VIsual.lnum;
		}
		નહીંતર
		{
		    from = VIsual.lnum;
		    to = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		જો (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		જો (wp->w_old_cursor_lnum > to)
		    to = wp->w_old_cursor_lnum;
		જો (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		જો (wp->w_old_visual_lnum > to)
		    to = wp->w_old_visual_lnum;
	    }
	    નહીંતર
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		જો (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = wp->w_old_cursor_lnum;
		}
		નહીંતર
		{
		    from = wp->w_old_cursor_lnum;
		    to = curwin->w_cursor.lnum;
		    જો (from == 0)	// Visual mode just started
			from = to;
		}

		જો (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.col != wp->w_old_visual_col)
		{
		    જો (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    જો (wp->w_old_visual_lnum > to)
			to = wp->w_old_visual_lnum;
		    જો (VIsual.lnum < from)
			from = VIsual.lnum;
		    જો (VIsual.lnum > to)
			to = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    જો (VIsual_mode == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_END_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		જો (curwin->w_curswant == MAXCOL)
		{
		    જો (get_ve_flags() & VE_BLOCK)
		    {
			pos_T	    pos;
			પૂર્ણાંક	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			માટે (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.col = (પૂર્ણાંક)ml_get_buf_len(wp->w_buffer, pos.lnum);
			    getvvcol(wp, &pos, ખાલી, ખાલી, &t, 0);
			    જો (toc < t)
				toc = t;
			}
			++toc;
		    }
		    નહીંતર
			toc = MAXCOL;
		}

		જો (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    જો (from > VIsual.lnum)
			from = VIsual.lnum;
		    જો (to < VIsual.lnum)
			to = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	નહીંતર
	{
	    // Use the line numbers of the old Visual area.
	    જો (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		to = wp->w_old_visual_lnum;
	    }
	    નહીંતર
	    {
		from = wp->w_old_visual_lnum;
		to = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	જો (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	જો (wp->w_valid & VALID_BOTLINE)
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
	જો (mid_start > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    જો (scrolled_down)
		mid_start = ટોચ_end;
	    નહીંતર
		mid_start = 0;
	    જ્યારે (lnum < from && idx < wp->w_લીટીઓ_valid)	// find start
	    {
		જો (wp->w_લીટીઓ[idx].wl_valid)
		    mid_start += wp->w_લીટીઓ[idx].wl_size;
		નહીંતર જો (!scrolled_down)
		    srow += wp->w_લીટીઓ[idx].wl_size;
		++idx;
#ifdef FEAT_FOLDING
		જો (idx < wp->w_લીટીઓ_valid && wp->w_લીટીઓ[idx].wl_valid)
		    lnum = wp->w_લીટીઓ[idx].wl_lnum;
		નહીંતર
#endif
		    ++lnum;
	    }
	    srow += mid_start;
	    mid_end = wp->w_height;
	    માટે ( ; idx < wp->w_લીટીઓ_valid; ++idx)		// find end
	    {
		જો (wp->w_લીટીઓ[idx].wl_valid
			&& wp->w_લીટીઓ[idx].wl_lnum >= to + 1)
		{
		    // Only update until first row of this line
		    mid_end = srow;
		    તોડો;
		}
		srow += wp->w_લીટીઓ[idx].wl_size;
	    }
	}
    }

    જો (VIsual_active && buf == curwin->w_buffer)
    {
	wp->w_old_visual_mode = VIsual_mode;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_col = VIsual.col;
	wp->w_old_curswant = curwin->w_curswant;
    }
    નહીંતર
    {
	wp->w_old_visual_mode = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_col = 0;
    }

#જો defined(FEAT_SYN_HL) || defined(FEAT_શોધ_EXTRA)
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
    જો (winbar_height(wp) > 0)
	redraw_win_toolbar(wp);
#endif

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#ifdef FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_FIELD(spv);
    જો (spell_check_window(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_ટોચ == 0;
    }
#endif

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    row = 0;
    srow = 0;
    માટે (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	જો (row == wp->w_height)
	{
	    didline = TRUE;
	    તોડો;
	}

	// stop updating when hit the end of the file
	જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
	{
	    eof = TRUE;
	    તોડો;
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
	જો (row < ટોચ_end
		|| (row >= mid_start && row < mid_end)
#ifdef FEAT_શોધ_EXTRA
		|| ટોચ_to_mod
#endif
		|| idx >= wp->w_લીટીઓ_valid
		|| (row + wp->w_લીટીઓ[idx].wl_size > bot_start)
		|| (mod_ટોચ != 0
		    && (lnum == mod_ટોચ
			|| (lnum >= mod_ટોચ
			    && (lnum < mod_bot
#ifdef FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_લીટી
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_changed(lnum)))
#endif
#ifdef FEAT_શોધ_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_match_head != ખાલી
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
#ifdef FEAT_શોધ_EXTRA
	    જો (lnum == mod_ટોચ)
		ટોચ_to_mod = FALSE;
#endif

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    જો (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_ટોચ && lnum < MAX(mod_bot, mod_ટોચ + 1)
		    && (!scrolled_down || row >= ટોચ_end))
	    {
		scrolled_for_mod = TRUE;

		પૂર્ણાંક		old_cline_height = 0;
		પૂર્ણાંક		old_rows = 0;
		પૂર્ણાંક		new_rows = 0;
		પૂર્ણાંક		xtra_rows;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		માટે (i = idx; i < wp->w_લીટીઓ_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    જો (wp->w_લીટીઓ[i].wl_valid
			    && wp->w_લીટીઓ[i].wl_lnum == mod_bot)
			તોડો;
		    જો (wp->w_લીટીઓ[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_height = wp->w_લીટીઓ[i].wl_size;
		    old_rows += wp->w_લીટીઓ[i].wl_size;
#ifdef FEAT_FOLDING
		    જો (wp->w_લીટીઓ[i].wl_valid
			    && wp->w_લીટીઓ[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			જ્યારે (i < wp->w_લીટીઓ_valid
						  && !wp->w_લીટીઓ[i].wl_valid)
			    old_rows += wp->w_લીટીઓ[i++].wl_size;
			તોડો;
		    }
#endif
		}

		જો (i >= wp->w_લીટીઓ_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_start = 0;
		}
		નહીંતર
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    માટે (l = lnum; l < mod_bot; ++l)
		    {
			જો (dollar_vcol >= 0 && wp == curwin &&
				old_cline_height > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_rows += old_cline_height;
#ifdef FEAT_FOLDING
			નહીંતર જો (hasFoldingWin(wp, l, ખાલી, &l, TRUE, ખાલી))
			    ++new_rows;
#endif
			નહીંતર
			    new_rows += plines_correct_topline(wp, l, TRUE);
			++j;
			જો (new_rows > wp->w_height - row - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_rows = 9999;
			    તોડો;
			}
		    }
		    xtra_rows = new_rows - old_rows;
		    જો (xtra_rows < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			જો (row - xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			નહીંતર
			{
			    check_for_delay(FALSE);
			    જો (win_del_લીટીઓ(wp, row,
					  -xtra_rows, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    નહીંતર
				bot_start = wp->w_height + xtra_rows;
			}
		    }
		    નહીંતર જો (xtra_rows > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			જો (row + xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			નહીંતર
			{
			    check_for_delay(FALSE);
			    જો (win_ins_લીટીઓ(wp, row + old_rows,
					     xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    નહીંતર જો (ટોચ_end > row + old_rows)
				// Scrolled the part at the top that requires
				// updating down.
				ટોચ_end += xtra_rows;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    જો (mod_bot != MAXLNUM && i != j)
		    {
			જો (j < i)
			{
			    પૂર્ણાંક x = row + new_rows;

			    // move entries in w_lines[] upwards
			    માટે (;;)
			    {
				// stop at last valid entry in w_lines[]
				જો (i >= wp->w_લીટીઓ_valid)
				{
				    wp->w_લીટીઓ_valid = j;
				    તોડો;
				}
				wp->w_લીટીઓ[j] = wp->w_લીટીઓ[i];
				// stop at a line that won't fit
				જો (x + (પૂર્ણાંક)wp->w_લીટીઓ[j].wl_size
							   > wp->w_height)
				{
				    wp->w_લીટીઓ_valid = j + 1;
				    તોડો;
				}
				x += wp->w_લીટીઓ[j++].wl_size;
				++i;
			    }
			    જો (bot_start > x)
				bot_start = x;
			}
			નહીંતર // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_લીટીઓ_valid += j;
			    જો (wp->w_લીટીઓ_valid > wp->w_height)
				wp->w_લીટીઓ_valid = wp->w_height;
			    માટે (i = wp->w_લીટીઓ_valid; i - j >= idx; --i)
				wp->w_લીટીઓ[i] = wp->w_લીટીઓ[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    જ્યારે (i >= idx)
			    {
				wp->w_લીટીઓ[i].wl_size = 0;
				wp->w_લીટીઓ[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ifdef FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_ગણતરી = folded_ગણતરી(wp, lnum, &win_foldinfo);
	    જો (fold_ગણતરી != 0)
	    {
		fold_લીટી(wp, fold_ગણતરી, &win_foldinfo, lnum, row);
		++row;
		--fold_ગણતરી;
		wp->w_લીટીઓ[idx].wl_folded = TRUE;
		wp->w_લીટીઓ[idx].wl_lastlnum = lnum + fold_ગણતરી;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
# ifdef FEAT_SPELL
		spv.spv_capcol_lnum = 0;
# endif
	    }
	    નહીંતર
#endif
	    જો (idx < wp->w_લીટીઓ_valid
		    && wp->w_લીટીઓ[idx].wl_valid
		    && wp->w_લીટીઓ[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_લીટીઓ[idx].wl_size > wp->w_height
#ifdef FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#endif
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		row = wp->w_height + 1;
	    }
	    નહીંતર
	    {
#ifdef FEAT_શોધ_EXTRA
		prepare_શોધ_hl(wp, &screen_શોધ_hl, lnum);
#endif
#ifdef FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		જો (syntax_last_parsed != 0 && syntax_last_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

		// Display one line.
		row = win_લીટી(wp, lnum, srow, wp->w_height, 0, &spv);

#ifdef FEAT_FOLDING
		wp->w_લીટીઓ[idx].wl_folded = FALSE;
		wp->w_લીટીઓ[idx].wl_lastlnum = lnum;
#endif
#ifdef FEAT_SYN_HL
		did_update = DID_લીટી;
		syntax_last_parsed = lnum;
#endif
	    }

	    wp->w_લીટીઓ[idx].wl_lnum = lnum;
	    wp->w_લીટીઓ[idx].wl_valid = TRUE;

	    પૂર્ણાંક is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    જો (row > wp->w_height || row + wp->w_winrow >= Rows)
	    {
		// we may need the size of that too long line later on
		જો (dollar_vcol == -1 || !is_curline)
		    wp->w_લીટીઓ[idx].wl_size = plines_win(wp, lnum, TRUE);
		++idx;
		તોડો;
	    }
	    જો (dollar_vcol == -1 || !is_curline)
		wp->w_લીટીઓ[idx].wl_size = row - srow;
	    ++idx;
#ifdef FEAT_FOLDING
	    lnum += fold_ગણતરી + 1;
#નહીંતર
	    ++lnum;
#endif
	}
	નહીંતર
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    જો ((wp->w_p_nu && mod_ટોચ != 0 && lnum >= mod_bot
			&& buf->b_mod_set && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_last_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#ifdef FEAT_FOLDING
		fold_ગણતરી = folded_ગણતરી(wp, lnum, &win_foldinfo);
		જો (fold_ગણતરી != 0)
		    fold_લીટી(wp, fold_ગણતરી, &win_foldinfo, lnum, row);
		નહીંતર
#endif
		    (શૂન્ય)win_લીટી(wp, lnum, srow, wp->w_height,
					       wp->w_લીટીઓ[idx].wl_size, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    row += wp->w_લીટીઓ[idx++].wl_size;
	    જો (row > wp->w_height)	// past end of screen
		તોડો;
#ifdef FEAT_FOLDING
	    lnum = wp->w_લીટીઓ[idx - 1].wl_lastlnum + 1;
#નહીંતર
	    ++lnum;
#endif
#ifdef FEAT_SYN_HL
	    did_update = DID_NONE;
#endif
#ifdef FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#endif
	}

	જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
	{
	    eof = TRUE;
	    તોડો;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	જો (idx >= Rows)
	    તોડો;
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
    જો (use_vtp() && get_conpty_fix_type() < 1)
    {
	પૂર્ણાંક k;

	માટે (k = 0; k < Rows; ++k)
	    જો (enc_utf8)
		જો ((*mb_off2cells)(લીટી_Offset[k] + topframe->fr_width - 2,
					   લીટી_Offset[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_width - 2, 1, 2,
			    FALSE, FALSE);
		નહીંતર
		    screen_draw_rectangle(k, topframe->fr_width - 1, 1, 1,
			    FALSE, FALSE);
	    નહીંતર
		screen_char(લીટી_Offset[k] + topframe->fr_width - 1, k,
			cmdline_width - 1);
    }
#endif

    જો (idx > wp->w_લીટીઓ_valid)
	wp->w_લીટીઓ_valid = idx;

#ifdef FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    જો (syntax_last_parsed != 0 && syntax_present(wp))
	syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_empty_rows = 0;
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    જો (!eof && !didline)
    {
	જો (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#ifdef FEAT_DIFF
	નહીંતર જો (diff_check_fill(wp, lnum) >= wp->w_height - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_rows = wp->w_height - srow;
	}
#endif
#ifdef FEAT_PROP_POPUP
	નહીંતર જો (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#endif
	નહીંતર જો (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    પૂર્ણાંક		scr_row = W_WINROW(wp) + wp->w_height - 1;
	    પૂર્ણાંક		પ્રતીક  = wp->w_fill_chars.lastline;
	    પૂર્ણાંક		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(પ્રતીક, &fillbuf[0]);
	    mb_char2bytes(પ્રતીક, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_width > 2 ? 2 : wp->w_width) * charlen,
			    scr_row, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_row, scr_row + 1,
		    (પૂર્ણાંક)wp->w_wincol + 2, (પૂર્ણાંક)W_ENDCOL(wp),
		    પ્રતીક, ' ', HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	નહીંતર જો (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    પૂર્ણાંક start_col = (પૂર્ણાંક)W_ENDCOL(wp) - 3;
	    પૂર્ણાંક પ્રતીક    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_height - 1,
		    W_WINROW(wp) + wp->w_height,
		    (start_col < wp->w_wincol ? wp->w_wincol : start_col),
		    (પૂર્ણાંક)W_ENDCOL(wp),
		    પ્રતીક, પ્રતીક, HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	નહીંતર
	{
	    win_draw_end(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_height, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    નહીંતર
    {
	draw_vsep_win(wp, row);
	જો (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_લીટી_ગણતરી + 1;
#ifdef FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    જો (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		જો (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		નહીંતર
		    i = wp->w_fill_chars.diff;
		જો (row + j > wp->w_height)
		    j = wp->w_height - row;
		win_draw_end(wp, i, i, TRUE, row, row + (પૂર્ણાંક)j, HLF_DED);
		row += j;
	    }
#endif
	}
	નહીંતર જો (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	જો (WIN_IS_POPUP(wp))
	    win_draw_end(wp, ' ', ' ', FALSE, row, wp->w_height, HLF_AT);
	નહીંતર
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

    જો (dollar_vcol == -1 || wp != curwin)
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
	જો (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#જો defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    પૂર્ણાંક		new_wcol = wp->w_wcol;
#endif
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#જો defined(FEAT_CONCEAL)
	    જો (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
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
		જો (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		નહીંતર
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#endif
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    જો (wp->w_redr_type != 0)
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
		જો (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#જો defined(FEAT_SYN_HL) || defined(FEAT_શોધ_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    જો (!got_int)
	got_int = save_got_int;
#endif

    જો (override_success)
	કાઢો_highlight_overrides();
}

#જો defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    સ્થિર શૂન્ય
update_prepare(શૂન્ય)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    જો (gui.in_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_શોધ_EXTRA
    start_શોધ_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_mask(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    સ્થિર શૂન્ય
update_finish(શૂન્ય)
{
    જો (redraw_cmdline || redraw_mode)
	showmode();

# ifdef FEAT_શોધ_EXTRA
    end_શોધ_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    જો (gui.in_use)
    {
	out_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#endif

#જો defined(FEAT_NETBEANS_INTG)
    શૂન્ય
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    પૂર્ણાંક		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_લીટી_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	જો (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    જો (!doit || updating_screen
	    || State == MODE_ASKMORE || State == MODE_HITRETURN
	    || સંદેશ_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	પાછા;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	જો (wp->w_redr_type != 0)
	    win_update(wp);
	જો (wp->w_redr_status)
	    win_redr_status(wp, FALSE);
    }

# જો defined(FEAT_TABPANEL)
    જો (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_finish();
}
#endif

#જો defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    શૂન્ય
updateWindow(win_T *wp)
{
    // return if already busy updating
    જો (updating_screen)
	પાછા;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    જો (clip_star.available && clip_isautosel_star())
	clip_update_selection(&clip_star);
    જો (clip_plus.available && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    જો (redraw_tabline)
	draw_tabline();

# જો defined(FEAT_TABPANEL)
    જો (redraw_tabpanel)
	draw_tabpanel();
# endif

    જો (wp->w_redr_status || p_ru
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
    પૂર્ણાંક
redraw_asap(પૂર્ણાંક type)
{
    પૂર્ણાંક		rows;
    પૂર્ણાંક		cols = screen_Columns;
    પૂર્ણાંક		r;
    પૂર્ણાંક		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    પૂર્ણાંક		i;
    u8char_T	*screenlineUC = ખાલી;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = ખાલી;	// copy from ScreenLines2[]

    redraw_later(type);
    જો (સંદેશ_scrolled
	    || (State != MODE_NORMAL && State != MODE_NORMAL_BUSY)
	    || exiting)
	પાછા ret;

    // Allocate space to save the text displayed in the command line area.
    rows = screen_Rows - cmdline_row;
    screenline = LALLOC_MULT(schar_T, rows * cols);
    screenattr = LALLOC_MULT(sattr_T, rows * cols);
    જો (screenline == ખાલી || screenattr == ખાલી)
	ret = 2;
    જો (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, rows * cols);
	જો (screenlineUC == ખાલી)
	    ret = 2;
	માટે (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, rows * cols);
	    જો (screenlineC[i] == ખાલી)
		ret = 2;
	}
    }
    જો (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, rows * cols);
	જો (screenline2 == ખાલી)
	    ret = 2;
    }

    જો (ret != 2)
    {
	// Save the text displayed in the command line area.
	માટે (r = 0; r < rows; ++r)
	{
	    mch_memmove(screenline + r * cols,
			Screen_લીટીઓ + લીટી_Offset[cmdline_row + r],
			(size_t)cols * કદ(schar_T));
	    mch_memmove(screenattr + r * cols,
			ScreenAttrs + લીટી_Offset[cmdline_row + r],
			(size_t)cols * કદ(sattr_T));
	    જો (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * cols,
			    Screen_લીટીઓ_UC + લીટી_Offset[cmdline_row + r],
			    (size_t)cols * કદ(u8char_T));
		માટે (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * cols,
				Screen_લીટીઓ_C[i] + લીટી_Offset[cmdline_row + r],
				(size_t)cols * કદ(u8char_T));
	    }
	    જો (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * cols,
			    ScreenLines2 + લીટી_Offset[cmdline_row + r],
			    (size_t)cols * કદ(schar_T));
	}

	update_screen(0);
	ret = 3;

	જો (must_redraw == 0)
	{
	    પૂર્ણાંક	off = (પૂર્ણાંક)(current_Screen_લીટી - Screen_લીટીઓ);

	    // Restore the text displayed in the command line area.
	    માટે (r = 0; r < rows; ++r)
	    {
		mch_memmove(current_Screen_લીટી,
			    screenline + r * cols,
			    (size_t)cols * કદ(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * cols,
			    (size_t)cols * કદ(sattr_T));
		જો (enc_utf8)
		{
		    mch_memmove(Screen_લીટીઓ_UC + off,
				screenlineUC + r * cols,
				(size_t)cols * કદ(u8char_T));
		    માટે (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_લીટીઓ_C[i] + off,
				    screenlineC[i] + r * cols,
				    (size_t)cols * કદ(u8char_T));
		}
		જો (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * cols,
				(size_t)cols * કદ(schar_T));
		screen_લીટી(curwin, cmdline_row + r, 0, cols, cols, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    જો (enc_utf8)
    {
	vim_free(screenlineUC);
	માટે (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    જો (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_message();

    setcursor();

    પાછા ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    શૂન્ય
redraw_after_callback(પૂર્ણાંક call_update_screen, પૂર્ણાંક do_message)
{
    ++redrawing_for_callback;

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

    જો (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_SETWSIZE || State == MODE_EXTERNCMD
	    || State == MODE_CONFIRM || exmode_active)
    {
	જો (do_message)
	    repeat_message();
    }
    નહીંતર જો (State & MODE_CMDLINE)
    {
	જો (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	જો (cmdline_row > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    જો (સંદેશ_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    નહીંતર જો (State & (MODE_NORMAL | MODE_ઉમેરો | MODE_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	જો (સંદેશ_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    સંદેશ_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
#ifdef FEAT_GUI
    જો (gui.in_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	out_flush_cursor(FALSE, FALSE);
    નહીંતર
#endif
	out_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    શૂન્ય
redraw_later(પૂર્ણાંક type)
{
    redraw_win_later(curwin, type);
}

    શૂન્ય
redraw_win_later(
    win_T	*wp,
    પૂર્ણાંક		type)
{
#ifdef FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    જો (inside_redraw_on_start_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    નહીંતર
#endif
    જો (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	જો (type >= UPD_NOT_VALID)
	    wp->w_લીટીઓ_valid = 0;
	જો (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    શૂન્ય
redraw_later_clear(શૂન્ય)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    શૂન્ય
redraw_all_later(પૂર્ણાંક type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    set_must_redraw(type);
}

#જો 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    શૂન્ય
redraw_all_windows_later(પૂર્ણાંક type)
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
    શૂન્ય
set_must_redraw(પૂર્ણાંક type)
{
    જો (!redraw_not_allowed &&
#ifdef FEAT_EVAL
	    !inside_redraw_on_start_cb &&
#endif
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    શૂન્ય
redraw_curbuf_later(પૂર્ણાંક type)
{
    redraw_buf_later(curbuf, type);
}

    શૂન્ય
redraw_buf_later(buf_T *buf, પૂર્ણાંક type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	જો (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    }
#ifdef FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	જો (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	જો (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
#endif
#જો defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    જો (curwin->w_buffer == buf)
	redraw_win_later(curwin, type);
#endif
}

#જો defined(FEAT_SIGNS)
    શૂન્ય
redraw_buf_લીટી_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	જો (wp->w_buffer == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#endif

#જો defined(FEAT_JOB_CHANNEL)
    શૂન્ય
redraw_buf_and_status_later(buf_T *buf, પૂર્ણાંક type)
{
    win_T	*wp;

    જો (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	પાછા;
    FOR_ALL_WINDOWS(wp)
    {
	જો (wp->w_buffer == buf)
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
    સ્થિર શૂન્ય
ruler_redraw_lastwin(શૂન્ય)
{
    જો (p_ru && lastwin->w_status_height == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    શૂન્ય
status_redraw_all(શૂન્ય)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	જો (wp->w_status_height)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    શૂન્ય
status_redraw_curbuf(શૂન્ય)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	જો (wp->w_status_height != 0 && wp->w_buffer == curbuf)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    જો (lastwin->w_buffer == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    શૂન્ય
redraw_statuslines(શૂન્ય)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	જો (wp->w_redr_status)
	{
	    bool ret = ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_status(wp, FALSE);
	    જો (ret)
		કાઢો_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    જો (redraw_tabline)
	draw_tabline();

#જો defined(FEAT_TABPANEL)
    જો (redraw_tabpanel)
	draw_tabpanel();
#endif
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    શૂન્ય
win_redraw_last_status(frame_T *frp)
{
    જો (frp->fr_layout == FR_LEAF)
	frp->fr_win->w_redr_status = true;
    નહીંતર જો (frp->fr_layout == FR_ROW)
    {
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    win_redraw_last_status(frp);
    }
    નહીંતર // frp->fr_layout == FR_COL
    {
	frp = frp->fr_child;
	જ્યારે (frp->fr_next != ખાલી)
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
    શૂન્ય
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    શૂન્ય
redraw_win_range_later(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    જો (last >= wp->w_topline && first < wp->w_botline)
    {
	જો (wp->w_redraw_ટોચ == 0 || wp->w_redraw_ટોચ > first)
	    wp->w_redraw_ટોચ = first;
	જો (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    શૂન્ય
redraw_win_range_now(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    જો (last >= wp->w_topline && first < wp->w_botline)
    {
	જો (wp->w_redraw_ટોચ == 0 || wp->w_redraw_ટોચ > first)
	    wp->w_redraw_ટોચ = first;
	જો (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	જો (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#ifdef FEAT_EVAL
સ્થિર bool redraw_cb_in_progress = false;

    શૂન્ય
f_redraw_listener_add(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    સ્થિર પૂર્ણાંક		id;

    જો (check_secure())
	પાછા;

    જો (redraw_cb_in_progress)
    {
	emsg(_(e_cannot_add_redraw_listener_in_listener_callback));
	પાછા;
    }

    જો (check_for_dict_arg(argvars, 0) == FAIL)
	પાછા;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    જો (rln == ખાલી)
	પાછા;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    જો (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	જો (cb.cb_name == ખાલી)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    પાછા;
	}
	copy_callback(&rln->rl_callbacks.on_start, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    જો (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	જો (cb.cb_name == ખાલી)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_start);
	    vim_free(rln);
	    પાછા;
	}
	copy_callback(&rln->rl_callbacks.on_end, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    જો (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	પાછા;
    }

    rln->rl_next = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_સંખ્યા;
    rettv->vval.v_સંખ્યા = id;

    પાછા;
}

    સ્થિર શૂન્ય
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_start);
    free_callback(&rln->rl_callbacks.on_end);

    vim_free(rln);
}


    સ્થિર શૂન્ય
redraw_listener_cleanup(શૂન્ય)
{
    માટે (redraw_listener_T *rln = redraw_listeners; rln != ખાલી;)
    {
	redraw_listener_T *next = rln->rl_next;
	જો (rln->rl_id == 0)
	{
	    જો (redraw_listeners == rln)
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
    સ્થિર redraw_listener_T *
get_redraw_listener(પૂર્ણાંક id)
{
    માટે (redraw_listener_T *rln = redraw_listeners; rln != ખાલી; rln = rln->rl_next)
	જો (rln->rl_id == id)
	    પાછા rln;
    પાછા ખાલી;
}

    શૂન્ય
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv UNUSED)
{
    પૂર્ણાંક			id;
    redraw_listener_T	*rln;

    જો (check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    id = argvars[0].vval.v_સંખ્યા;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_સંખ્યા;
    જો (rln == ખાલી)
    {
	rettv->vval.v_સંખ્યા = 0;
	પાછા;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_સંખ્યા = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    સ્થિર શૂન્ય
invoke_redraw_listener_start_or_end(bool start)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    જો (start)
	inside_redraw_on_start_cb = true;

    redraw_cb_in_progress = true;
    માટે (redraw_listener_T *rln = redraw_listeners; rln != ખાલી; rln = rln->rl_next)
    {
	જો (rln->rl_id == 0)
	    // Listener has been removed, skip
	    ચાલુ;
	જો (start && rln->rl_callbacks.on_start.cb_name != ખાલી)
	{
	    call_callback(&rln->rl_callbacks.on_start, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	નહીંતર જો (rln->rl_callbacks.on_end.cb_name != ખાલી)
	{
	    call_callback(&rln->rl_callbacks.on_end, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_in_progress = false;

    જો (start)
	inside_redraw_on_start_cb = false;
}
#endif // FEAT_EVAL
