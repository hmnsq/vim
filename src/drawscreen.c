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

#شامل "vim.h"

ایستا تهی win_update(win_T *wp);
#ifdef FEAT_STL_OPT
ایستا تهی redraw_custom_statusline(win_T *wp);
#endif
ایستا تهی borrow_stl_vsep_hl(تهی);
#اگر defined(FEAT_جستجو_EXTRA) || defined(FEAT_CLIPBOARD)
ایستا صحیح  did_update_one_window;
#endif

#ifdef FEAT_EVAL
ایستا تهی redraw_listener_cleanup(تهی);
ایستا تهی invoke_redraw_listener_start_or_end(bool start);
#endif

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    صحیح
update_screen(صحیح type_arg)
{
    صحیح		type = type_arg;
    win_T	*wp;
    ایستا صحیح	did_intro = FALSE;
#ifdef FEAT_GUI
    صحیح		did_one = FALSE;
    صحیح		did_undraw = FALSE;
    صحیح		gui_cursor_col = 0;
    صحیح		gui_cursor_row = 0;
#endif
    صحیح		no_update = FALSE;
    صحیح		save_pum_will_redraw = pum_will_redraw;
#ifdef FEAT_PROP_POPUP
    صحیح		did_redraw_window = FALSE;
#endif
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    اگر (!screen_valid(TRUE))
	بازگشت FAIL;

    اگر (type == UPD_VALID_NO_UPDATE)
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
    اگر (need_diff_redraw)
	diff_redraw(TRUE);
#endif

    اگر (must_redraw)
    {
	اگر (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    اگر (curwin->w_سطرها_valid == 0 && type < UPD_NOT_VALID
#ifdef FEAT_TERMINAL
	    && !term_do_update_window(curwin)
#endif
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    اگر (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	اگر (type > UPD_INVERTED_ALL)
	    curwin->w_سطرها_valid = 0;	// don't use w_lines[].wl_size now
	بازگشت FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    صحیح hid_cursor = !sync_output_active();
    اگر (hid_cursor)
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
    اگر (no_update)
	++no_win_do_سطرها_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    اگر (پیام_scrolled)
    {
	clear_cmdline = TRUE;
	اگر (type != UPD_CLEAR)
	{
	    اگر (پیام_scrolled > Rows - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    وگرنه
	    {
		check_for_delay(FALSE);
		اگر (screen_ins_سطرها(0, 0, پیام_scrolled, (صحیح)Rows, 0, پوچ)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    اگر (wp->w_winrow < پیام_scrolled)
		    {
			اگر (W_WINROW(wp) + wp->w_height > پیام_scrolled
				&& wp->w_redr_type < UPD_REDRAW_بالاترین
				&& wp->w_سطرها_valid > 0
				&& wp->w_topline == wp->w_سطرها[0].wl_lnum)
			{
			    wp->w_upd_rows = پیام_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_بالاترین;
			}
			وگرنه
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    اگر (W_WINROW(wp) + wp->w_height
					 + wp->w_status_height <= پیام_scrolled)
				wp->w_redr_status = true;
			}
		    }
		}
		اگر (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#اگر defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#endif
	}
	پیام_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    اگر (need_highlight_changed)
	highlight_changed();

    اگر (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    اگر (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    اگر (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? عدد_width(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#endif

#ifdef FEAT_EVAL
    invoke_redraw_listener_start_or_end(true);
#endif

    // Only start redrawing if there is really something to do.
    اگر (type == UPD_INVERTED)
	update_curswant();
    اگر (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_سطرها[0].wl_valid
#ifdef FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#endif
		    && curwin->w_topline == curwin->w_سطرها[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_active
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_mode == VIsual_mode
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    اگر (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#اگر defined(FEAT_TABPANEL)
    اگر (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#endif

#ifdef FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_buffer->b_mod_set)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    برای (wwp = firstwin; wwp != wp; wwp = wwp->w_next)
		اگر (wwp->w_buffer == wp->w_buffer)
		    بشکن;
	    اگر (wwp == wp && syntax_present(wp))
		syn_پشته_apply_changes(wp->w_buffer);
	}
    }
#endif

    اگر (pum_redraw_in_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#اگر defined(FEAT_جستجو_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_window = FALSE;
#endif
#ifdef FEAT_جستجو_EXTRA
    screen_جستجو_hl.rm.regprog = پوچ;
#endif
    FOR_ALL_WINDOWS(wp)
    {
	override_success = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

	اگر (wp->w_redr_type != 0)
	{
	    cursor_off();
#ifdef FEAT_PROP_POPUP
	    did_redraw_window = TRUE;
#endif
#ifdef FEAT_GUI
	    اگر (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		اگر (gui.in_use && (wp == curwin || cursor_is_sleeping()))
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
	اگر (wp->w_redr_status)
	{
	    cursor_off();
	    win_redr_status(wp, TRUE); // any popup menu will be redrawn below
	}

	اگر (override_success)
	    برداشت_highlight_overrides();

    }
#اگر defined(FEAT_جستجو_EXTRA)
    end_جستجو_hl();
#endif

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    اگر (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_vsep_width > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_buffer->b_mod_set = false;

#ifdef FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    اگر (did_redraw_window || popup_need_redraw())
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
    اگر (clear_cmdline || redraw_cmdline || redraw_mode)
	showmode();

    اگر (no_update)
	--no_win_do_سطرها_ins;

    // May put up an introductory message when not editing a file
    اگر (!did_intro)
	maybe_intro_message();
    did_intro = TRUE;

#ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    اگر (gui.in_use)
    {
	اگر (did_undraw && !gui_mch_is_blink_off())
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
	وگرنه
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
    اگر (hid_cursor)
	cursor_on();

    بازگشت OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    صحیح
statusline_row(win_T *wp)
{
#اگر defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    اگر (wp->w_frame->fr_height == wp->w_status_height && !popup_is_popup(wp))
	بازگشت wp->w_winrow;
#endif
    بازگشت W_WINROW(wp) + wp->w_height;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    تهی
win_redr_status(win_T *wp, صحیح ignore_pum UNUSED)
{
    صحیح		row;
    صحیح		fillchar;
    صحیح		attr;
    صحیح		i;
    ایستا صحیح  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    اگر (busy)
	بازگشت;
    busy = TRUE;

    row = statusline_row(wp);

    wp->w_redr_status = false;
    اگر (wp->w_status_height == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    وگرنه اگر (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_status = true;
    }
#ifdef FEAT_STL_OPT
    وگرنه اگر (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#endif
    وگرنه
    {
	char_u	*p;
	صحیح	plen;
	صحیح	NameBufflen;
	صحیح	this_ru_col;
	صحیح	n;			// scratch value

	fillchar = fillchar_status(&attr, wp);

	get_trans_bufname(wp->w_buffer);
	p = NameBuff;
	plen = (صحیح)STRLEN(p);

	اگر ((bt_help(wp->w_buffer)
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
	اگر (bt_help(wp->w_buffer))
	    plen += vim_snprintf((نویسه *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#ifdef FEAT_QUICKFIX
	اگر (wp->w_p_pvw)
	    plen += vim_snprintf((نویسه *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#endif
	اگر (bufIsChanged(wp->w_buffer) && !bt_terminal(wp->w_buffer))
	    plen += vim_snprintf((نویسه *)p + plen, MAXPATHL - plen, "%s", "[+]");
	اگر (wp->w_buffer->b_p_ro)
	    plen += vim_snprintf((نویسه *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_col = ru_col - (cmdline_width - wp->w_width);
	n = (wp->w_width + 1) / 2;
	اگر (this_ru_col < n)
	    this_ru_col = n;
	اگر (this_ru_col <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	وگرنه اگر (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    برای (i = 0; p[i] != NUL && plen >= this_ru_col - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    اگر (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	وگرنه اگر (plen > this_ru_col - 1)
	{
	    p += plen - (this_ru_col - 1);
	    *p = '<';
	    plen = this_ru_col - 1;
	}

	screen_puts(p, row, wp->w_wincol, attr);
	screen_fill(row, row + 1, plen + wp->w_wincol,
		    this_ru_col + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	برای (i = 1; i < wp->w_status_height; i++)
	    screen_fill(row + i, row + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	اگر ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_col - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, row, (صحیح)(this_ru_col - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	اگر (p_sc && *p_sloc == 's')
	{
	    n = this_ru_col - plen - 2;		    // perform the calculation here so we only do it once
	    صحیح	width = MIN(10, n);

	    اگر (width > 0)
		screen_puts_len(showcmd_buf, width, row,
				wp->w_wincol + this_ru_col - width - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    اگر (wp->w_vsep_width != 0 && wp->w_status_height != 0 && redrawing())
    {
	برای (i = 0; i < wp->w_status_height; i++)
	{
	    صحیح r = row + i;
	    اگر (stl_connected(wp))
		fillchar = fillchar_status(&attr, wp);
	    وگرنه
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
    ایستا تهی
borrow_stl_vsep_hl(تهی)
{
    win_T   *چپ = پوچ;
    win_T   *راست = پوچ;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    اگر (!redrawing() || Screen_سطرها == پوچ)
	بازگشت;

    FOR_ALL_WINDOWS(چپ)
    {
	اگر (چپ->w_status_height == 0 || چپ->w_vsep_width == 0)
	    ادامه;
	// A window of zero width has no status line cell to borrow from.
	اگر (چپ->w_width == 0)
	    ادامه;
	اگر (!stl_connected(چپ))
	    ادامه;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = پوچ;
	صحیح start = 0;
	صحیح end = 0;

	FOR_ALL_WINDOWS(راست)
	{
	    اگر (راست == چپ || راست->w_status_height == 0)
		ادامه;
	    اگر (راست->w_wincol != W_ENDCOL(چپ) + 1)
		ادامه;
	    صحیح l_stl_row = W_WINROW(چپ) + چپ->w_height;
	    صحیح r_stl_row = W_WINROW(راست) + راست->w_height;

	    start = l_stl_row > r_stl_row ? l_stl_row : r_stl_row;
	    end = l_stl_row + چپ->w_status_height
				< r_stl_row + راست->w_status_height
		? l_stl_row + چپ->w_status_height
		: r_stl_row + راست->w_status_height;
	    اگر (start < end)
	    {
		neighbour = راست;
		بشکن;
	    }
	}
	اگر (neighbour == پوچ)
	    ادامه;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	صحیح	hl;
	اگر (چپ != curwin && neighbour != curwin
		&& fillchar_status(&hl, چپ) != ' ')
	    ادامه;

	// Source: prefer curwin's side; otherwise left window's right edge.
	صحیح dst_col = W_ENDCOL(چپ);
	صحیح src_col = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(چپ) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	اگر (dst_col >= screen_Columns || src_col >= screen_Columns)
	    ادامه;
	اگر (end > screen_Rows)
	    end = screen_Rows;

	برای (صحیح r = start; r < end; r++)
	{
	    unsigned	dst_off = سطر_Offset[r] + dst_col;
	    sattr_T	attr = ScreenAttrs[سطر_Offset[r] + src_col];

	    اگر (ScreenAttrs[dst_off] != attr)
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
    ایستا تهی
redraw_custom_statusline(win_T *wp)
{
    ایستا صحیح	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    اگر (entered)
	بازگشت;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#endif

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    تهی
showruler(صحیح always)
{
    اگر (!always && !redrawing())
	بازگشت;
    اگر (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_status = true;
	بازگشت;
    }
#اگر defined(FEAT_STL_OPT)
    اگر ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_status_height)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    وگرنه
#endif
	win_redr_ruler(curwin, always, FALSE);

    اگر (need_maketitle
#ifdef FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_IN_ICON))
	    || (p_title && (stl_syntax & STL_IN_TITLE))
#endif
       )
	maketitle();

    // Redraw the tab pages line if needed.
    اگر (redraw_tabline)
	draw_tabline();

#اگر defined(FEAT_TABPANEL)
    اگر (redraw_tabpanel)
	draw_tabpanel();
#endif
}

    تهی
win_redr_ruler(win_T *wp, صحیح always, صحیح ignore_pum)
{
    صحیح	empty_سطر = FALSE;

    // If 'ruler' off don't do anything
    اگر (!p_ru)
	بازگشت;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    اگر (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_سطر_شمار)
	بازگشت;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    اگر (wp == lastwin && lastwin->w_status_height == 0)
	اگر (edit_submode != پوچ)
	    بازگشت;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    اگر (!ignore_pum && pum_visible())
	بازگشت;

#ifdef FEAT_STL_OPT
    اگر (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	بازگشت;
    }
#endif

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    اگر ((State & MODE_درج) == 0
		&& *ml_get_buf(wp->w_buffer, wp->w_cursor.lnum, FALSE) == NUL)
	empty_سطر = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    اگر (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.col != wp->w_ru_cursor.col
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_buffer->b_ml.ml_سطر_شمار != wp->w_ru_سطر_شمار
#ifdef FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#endif
	    || empty_سطر != wp->w_ru_empty)
    {
	صحیح	row;
	صحیح	fillchar;
	صحیح	attr;
	صحیح	off;
	صحیح	width;
	colnr_T	virtcol;
#define RULER_BUF_LEN 70
	char_u	buffer[RULER_BUF_LEN];
	صحیح	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	صحیح	rel_poslen;
	صحیح	this_ru_col;
	صحیح	n1;			    // scratch value
	صحیح	n2;			    // scratch value
	bool	override_success =
	    افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	اگر (wp->w_status_height)
	{
	    row = statusline_row(wp);
	    fillchar = fillchar_status(&attr, wp);
	    off = wp->w_wincol;
	    width = wp->w_width;
	}
	وگرنه
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_col_off;
	    width = cmdline_width;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	اگر (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, پوچ, &virtcol, پوچ, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((نویسه *)buffer, RULER_BUF_LEN, _("%ld,"),
		(wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		    ? 0L
		    : (long)(wp->w_cursor.lnum));
	bufferlen += col_print(buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			empty_سطر ? 0 : (صحیح)wp->w_cursor.col + 1,
			(صحیح)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	اگر (wp->w_status_height == 0)	// can't use last char of screen
	    ++n1;

	this_ru_col = ru_col - (cmdline_width - width);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (width + 1) / 2;
	اگر (this_ru_col < n2)
	    this_ru_col = n2;
	اگر (this_ru_col + n1 < width)
	{
	    // need at least space for rel_pos + NUL
	    وقتی (this_ru_col + n1 < width
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		اگر (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, buffer + bufferlen);
		وگرنه
		    buffer[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((نویسه *)buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	اگر (has_mbyte)
	{
	    برای (n1 = 0, n2 = 0; buffer[n1] != NUL; n1 += (*mb_ptr2len)(buffer + n1))
	    {
		n2 += (*mb_ptr2cells)(buffer + n1);
		اگر (this_ru_col + n2 > width)
		{
		    bufferlen = n1;
		    buffer[bufferlen] = NUL;
		    بشکن;
		}
	    }
	}
	وگرنه اگر (this_ru_col + bufferlen > width)
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
	wp->w_ru_empty = empty_سطر;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_سطر_شمار = wp->w_buffer->b_ml.ml_سطر_شمار;
#ifdef FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#endif

	اگر (override_success)
	    برداشت_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    تهی
after_updating_screen(صحیح may_resize_shell UNUSED)
{
    updating_screen = FALSE;
#ifdef FEAT_GUI
    اگر (may_resize_shell)
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
    تهی
update_curbuf(صحیح type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#اگر defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    ایستا صحیح
text_to_screenline(win_T *wp, char_u *text, صحیح col)
{
    صحیح		off = (صحیح)(current_Screen_سطر - Screen_سطرها);

    اگر (has_mbyte)
    {
	صحیح	cells;
	صحیح	u8c, u8cc[MAX_MCO];
	صحیح	i;
	صحیح	idx;
	صحیح	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	صحیح	prev_c = 0;		// previous Arabic character
	صحیح	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	اگر (wp->w_p_rl)
	    idx = off;
	وگرنه
# endif
	    idx = off + col;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	برای (p = text; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    اگر (col + cells > wp->w_width)
		بشکن;
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		idx = off + wp->w_width - col - cells;
# endif
	    Screen_سطرها[idx] = *p;
	    اگر (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		اگر (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_سطرها_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    prev_c = u8c;
# endif
		}
		وگرنه
		{
# ifdef FEAT_ARABIC
		    اگر (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			صحیح	pc, pc1, nc;
			صحیح	pcc[MAX_MCO];
			صحیح	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			اگر (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			وگرنه
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			Screen_سطرها[idx] = firstbyte;
		    }
		    وگرنه
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_سطرها_UC[idx] = u8c;
		    برای (i = 0; i < Screen_mco; ++i)
		    {
			Screen_سطرها_C[i][idx] = u8cc[i];
			اگر (u8cc[i] == 0)
			    بشکن;
		    }
		}
		اگر (cells > 1)
		    Screen_سطرها[idx + 1] = 0;
	    }
	    وگرنه اگر (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    وگرنه اگر (cells > 1)
		// double-width character
		Screen_سطرها[idx + 1] = p[1];
	    col += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    وگرنه
    {
	صحیح len = (صحیح)STRLEN(text);
	صحیح n = wp->w_width - col;

	اگر (len > n)
	    len = n;
	اگر (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		mch_memmove(current_Screen_سطر, text, len);
	    وگرنه
# endif
		mch_memmove(current_Screen_سطر + col, text, len);
	    col += len;
	}
    }
    بازگشت col;
}
#endif

#ifdef FEAT_MENU
/*
 * Draw the window toolbar.
 */
    ایستا تهی
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    صحیح		item_idx = 0;
    صحیح		item_شمار = 0;
    صحیح		col = 0;
    صحیح		next_col;
    صحیح		off = (صحیح)(current_Screen_سطر - Screen_سطرها);
    bool	override_success =
	افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);
    صحیح		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    صحیح		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_اقلام);
    FOR_ALL_CHILD_MENUS(wp->w_winbar, menu)
	++item_شمار;
    wp->w_winbar_اقلام = ALLOC_CLEAR_MULT(winbar_item_T, item_شمار + 1);

    // TODO: use fewer spaces if there is not enough room
    برای (menu = wp->w_winbar->children;
			  menu != پوچ && col < wp->w_width; menu = menu->next)
    {
	space_to_screenline(off + col, fill_attr);
	اگر (++col >= wp->w_width)
	    بشکن;
	اگر (col > 1)
	{
	    space_to_screenline(off + col, fill_attr);
	    اگر (++col >= wp->w_width)
		بشکن;
	}

	wp->w_winbar_اقلام[item_idx].wb_startcol = col;
	space_to_screenline(off + col, button_attr);
	اگر (++col >= wp->w_width)
	    بشکن;

	next_col = text_to_screenline(wp, menu->name, col);
	وقتی (col < next_col)
	{
	    ScreenAttrs[off + col] = button_attr;
	    ++col;
	}
	wp->w_winbar_اقلام[item_idx].wb_endcol = col;
	wp->w_winbar_اقلام[item_idx].wb_menu = menu;
	++item_idx;

	اگر (col >= wp->w_width)
	    بشکن;
	space_to_screenline(off + col, button_attr);
	++col;
    }
    وقتی (col < wp->w_width)
    {
	space_to_screenline(off + col, fill_attr);
	++col;
    }
    wp->w_winbar_اقلام[item_idx].wb_menu = پوچ; // end marker

    screen_سطر(wp, wp->w_winrow, wp->w_wincol, wp->w_width,
							  wp->w_width, -1, 0);

    اگر (override_success)
	برداشت_highlight_overrides();
}
#endif

#اگر defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    ایستا تهی
copy_text_attr(
    صحیح		off,
    char_u	*buf,
    صحیح		len,
    صحیح		attr)
{
    صحیح		i;

    mch_memmove(Screen_سطرها + off, buf, (size_t)len);
    اگر (enc_utf8)
	vim_memset(Screen_سطرها_UC + off, 0, اندازه(u8char_T) * (size_t)len);
    برای (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    ایستا تهی
fold_سطر(
    win_T	*wp,
    long	fold_شمار,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    صحیح		row)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*بالاترین, *bot;
    linenr_T	lnume = lnum + fold_شمار - 1;
    صحیح		len;
    char_u	*text;
    صحیح		fdc;
    صحیح		col;
    صحیح		txtcol;
    صحیح		off = (صحیح)(current_Screen_سطر - Screen_سطرها);
    صحیح		ri;

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
    اگر (wp == cmdwin_win)
    {
	Screen_سطرها[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	اگر (enc_utf8)
	    Screen_سطرها_UC[off] = 0;
	++col;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    انجامبده { \
	اگر (wp->w_p_rl) \
	    برای (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_width - (p) - (l)) + ri] = v; \
	 وگرنه \
	    برای (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } وقتی (0)
# وگرنه
#  define RL_MEMSET(p, v, l) \
    انجامبده { \
	برای (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } وقتی (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, col);
    اگر (fdc > 0)
    {
	char_u	*p;
	صحیح	i;
	صحیح	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	برای (i = 0; i < fdc; i++)
	{
	    صحیح		ch;

	    اگر (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    وگرنه
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		idx = off + wp->w_width - i - 1 - col;
	    وگرنه
# endif
		idx = off + col + i;
	    اگر (enc_utf8)
	    {
		اگر (ch >= 0x80)
		{
		    Screen_سطرها_UC[idx] = ch;
		    Screen_سطرها_C[0][idx] = 0;
		    Screen_سطرها[idx] = 0x80;
		}
		وگرنه
		{
		    Screen_سطرها[idx] = ch;
		    Screen_سطرها_UC[idx] = 0;
		}
	    }
	    وگرنه
		Screen_سطرها[idx] = ch;
	}

	RL_MEMSET(col, HL_ATTR(HLF_FC), fdc);
	col += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    // text
    RL_MEMSET(col, HL_ATTR(HLF_FL), wp->w_width - col);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    اگر (signcolumn_on(wp))
    {
	len = wp->w_width - col;
	اگر (len > 0)
	{
	    اگر (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    وگرنه
#  endif
		copy_text_attr(off + col, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    col += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    اگر (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_width - col;
	اگر (len > 0)
	{
	    صحیح	    w = عدد_width(wp);
	    long    رقم;
	    نویسه    *fmt = "%*ld ";

	    اگر (len > w + 1)
		len = w + 1;

	    اگر (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		رقم = (long)lnum;
	    وگرنه
	    {
		// 'relativenumber', don't use negative numbers
		رقم = labs((long)get_cursor_rel_lnum(wp, lnum));
		اگر (رقم == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    رقم = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((نویسه *)buf, اندازه(buf), fmt, w, رقم);
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col, buf, len,
							     HL_ATTR(HLF_FL));
	    وگرنه
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
    وقتی (col < wp->w_width)
    {
	صحیح c = wp->w_fill_chars.fold;
	صحیح idx = off + col;

# ifdef FEAT_RIGHTLEFT
	اگر (wp->w_p_rl)
	    idx = off + wp->w_width - 1 - col;
# endif

	اگر (enc_utf8)
	{
	    اگر (c >= 0x80)
	    {
		Screen_سطرها_UC[idx] = c;
		Screen_سطرها_C[0][idx] = 0;
		Screen_سطرها[idx] = 0x80; // avoid storing zero
	    }
	    وگرنه
	    {
		Screen_سطرها_UC[idx] = 0;
		Screen_سطرها[idx] = c;
	    }
	}
	وگرنه
	    Screen_سطرها[idx] = c;
	++col;
    }

    اگر (text != buf)
	vim_free(text);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    اگر (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
	اگر (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    بالاترین = &curwin->w_cursor;
	    bot = &VIsual;
	}
	وگرنه
	{
	    // Visual is before curwin->w_cursor
	    بالاترین = &VIsual;
	    bot = &curwin->w_cursor;
	}
	اگر (lnum >= بالاترین->lnum
		&& lnume <= bot->lnum
		&& (VIsual_mode != 'v'
		    || ((lnum > بالاترین->lnum
			    || (lnum == بالاترین->lnum
				&& بالاترین->col == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->col - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_buffer, lnume))))))
	{
	    اگر (VIsual_mode == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		اگر (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_width)
		{
		    اگر (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_width)
			len = wp->w_old_cursor_lcol;
		    وگرنه
			len = wp->w_width - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (صحیح)wp->w_old_cursor_fcol);
		}
	    }
	    وگرنه
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_width - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    اگر (wp->w_p_cc_cols)
    {
	صحیح i = 0;
	صحیح j = wp->w_p_cc_cols[i];
	صحیح old_txtcol = txtcol;

	وقتی (j > -1)
	{
	    txtcol += j;
	    اگر (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    وگرنه
		txtcol -= wp->w_leftcol;
	    اگر (txtcol >= 0 && txtcol < wp->w_width)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_cols[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    اگر (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	اگر (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	وگرنه
	    txtcol -= wp->w_leftcol;
	اگر (txtcol >= 0 && txtcol < wp->w_width)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_سطر(wp, row + W_WINROW(wp), wp->w_wincol, wp->w_width, wp->w_width,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    اگر (wp == curwin
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
    اگر (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_سطر(wp))
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
    ایستا تهی
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_buffer;
    صحیح		type;
    صحیح		بالاترین_end = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    صحیح		mid_start = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    صحیح		mid_end = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    صحیح		bot_start = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    صحیح		scrolled_down = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    صحیح		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //سطرها
#ifdef FEAT_جستجو_EXTRA
    صحیح		بالاترین_to_mod = FALSE;    // redraw above mod_top
#endif

    صحیح		row;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    صحیح		idx;		// current index in w_lines[]
    صحیح		srow;		// starting row of the current line

    صحیح		eof = FALSE;	// if TRUE, we hit the end of the file
    صحیح		didline = FALSE; // if TRUE, we finished the last line
    صحیح		i;
    long	j;
    ایستا صحیح	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#ifdef FEAT_CONCEAL
    صحیح		old_wrow = wp->w_wrow;
    صحیح		old_wcol = wp->w_wcol;
#endif
#ifdef FEAT_FOLDING
    long	fold_شمار;
#endif
#ifdef FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_سطر 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    صحیح		did_update = DID_NONE;
    linenr_T	syntax_last_parsed = 0;		// last parsed text line
#endif
    linenr_T	mod_بالاترین = 0;
    linenr_T	mod_bot = 0;
#اگر defined(FEAT_SYN_HL) || defined(FEAT_جستجو_EXTRA)
    صحیح		save_got_int;
#endif
    bool	override_success;

#اگر defined(FEAT_جستجو_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    اگر (!did_update_one_window)
    {
	did_update_one_window = TRUE;
# ifdef FEAT_جستجو_EXTRA
	start_جستجو_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	اگر (clip_star.available && clip_isautosel_star())
	    clip_update_selection(&clip_star);
	اگر (clip_plus.available && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#endif

#ifdef FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    اگر (wp->w_p_cuc)
	validate_virtcol_win(wp);
#endif

    type = wp->w_redr_type;

    اگر (type == UPD_NOT_VALID)
    {
	wp->w_redr_status = true;
	wp->w_سطرها_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    اگر (wp->w_height + WINBAR_HEIGHT(wp) == 0
	    || (wp->w_frame->fr_height == wp->w_status_height
#اگر defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#endif
	       ))
    {
	wp->w_redr_type = 0;
	بازگشت;
    }

    // Window is zero-width: Only need to draw the separator.
    اگر (wp->w_width == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	بازگشت;
    }

    override_success = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);


#ifdef FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    اگر (term_do_update_window(wp))
    {
	term_update_window(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	اگر (winbar_height(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	اگر (override_success)
	    برداشت_highlight_overrides();
	بازگشت;
    }
#endif

#ifdef FEAT_جستجو_EXTRA
    init_جستجو_hl(wp, &screen_جستجو_hl);
#endif

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    اگر (wp->w_skipcol > 0 && wp->w_width > win_col_off(wp))
    {
	صحیح w = 0;
	صحیح width1 = wp->w_width - win_col_off(wp);
	صحیح width2 = width1 + win_col_off2(wp);
	صحیح add = width1;

	وقتی (w < wp->w_skipcol)
	{
	    اگر (w > 0)
		add = width2;
	    w += add;
	}
	اگر (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - add;
    }

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? عدد_width(wp) : 0;
    اگر (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    وگرنه
#endif

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_بالاترین = wp->w_redraw_بالاترین;
	اگر (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	وگرنه
	    mod_bot = 0;
	اگر (buf->b_mod_set)
	{
	    اگر (mod_بالاترین == 0 || mod_بالاترین > buf->b_mod_بالاترین)
	    {
		mod_بالاترین = buf->b_mod_بالاترین;
#ifdef FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		اگر (syntax_present(wp))
		{
		    mod_بالاترین -= buf->b_s.b_syn_sync_linebreaks;
		    اگر (mod_بالاترین < 1)
			mod_بالاترین = 1;
		}
#endif
	    }
	    اگر (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ifdef FEAT_جستجو_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    اگر (screen_جستجو_hl.rm.regprog != پوچ
		    && re_multiline(screen_جستجو_hl.rm.regprog))
		بالاترین_to_mod = TRUE;
	    وگرنه
	    {
		matchitem_T *cur = wp->w_match_head;

		وقتی (cur != پوچ)
		{
		    اگر (cur->mit_match.regprog != پوچ
				       && re_multiline(cur->mit_match.regprog))
		    {
			بالاترین_to_mod = TRUE;
			بشکن;
		    }
		    cur = cur->mit_next;
		}
	    }
#endif
	}

#ifdef FEAT_جستجو_EXTRA
	اگر (جستجو_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    اگر (mod_بالاترین == 0 || mod_بالاترین > جستجو_hl_has_cursor_lnum)
		mod_بالاترین = جستجو_hl_has_cursor_lnum;
	    اگر (mod_bot == 0 || mod_bot < جستجو_hl_has_cursor_lnum + 1)
		mod_bot = جستجو_hl_has_cursor_lnum + 1;
	}
#endif

#ifdef FEAT_FOLDING
	اگر (mod_بالاترین != 0 && hasAnyFolding(wp))
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
	    برای (i = 0; i < wp->w_سطرها_valid; ++i)
		اگر (wp->w_سطرها[i].wl_valid)
		{
		    اگر (wp->w_سطرها[i].wl_lastlnum < mod_بالاترین)
			lnumt = wp->w_سطرها[i].wl_lastlnum + 1;
		    اگر (lnumb == MAXLNUM && wp->w_سطرها[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_سطرها[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			اگر (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (تهی)hasFoldingWin(wp, mod_بالاترین, &mod_بالاترین, پوچ, TRUE, پوچ);
	    اگر (mod_بالاترین > lnumt)
		mod_بالاترین = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (تهی)hasFoldingWin(wp, mod_bot, پوچ, &mod_bot, TRUE, پوچ);
	    ++mod_bot;
	    اگر (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#endif

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	اگر (mod_بالاترین != 0 && mod_بالاترین < wp->w_topline)
	{
	    اگر (mod_bot > wp->w_topline)
		mod_بالاترین = wp->w_topline;
#ifdef FEAT_SYN_HL
	    وگرنه اگر (syntax_present(wp))
		بالاترین_end = 1;
#endif
	}
    }
    wp->w_redraw_بالاترین = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#ifdef FEAT_جستجو_EXTRA
    جستجو_hl_has_cursor_lnum = 0;
#endif


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    اگر (type == UPD_REDRAW_بالاترین)
    {
	j = 0;
	برای (i = 0; i < wp->w_سطرها_valid; ++i)
	{
	    j += wp->w_سطرها[i].wl_size;
	    اگر (j >= wp->w_upd_rows)
	    {
		بالاترین_end = j;
		بشکن;
	    }
	}
	اگر (بالاترین_end == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	وگرنه
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    اگر (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    اگر ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#ifdef FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#endif
	    )
    {
	اگر (mod_بالاترین != 0
		&& wp->w_topline == mod_بالاترین
		&& (!wp->w_سطرها[0].wl_valid
		    || wp->w_topline == wp->w_سطرها[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	وگرنه اگر (wp->w_سطرها[0].wl_valid
		&& (wp->w_topline < wp->w_سطرها[0].wl_lnum
#ifdef FEAT_DIFF
		    || (wp->w_topline == wp->w_سطرها[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#endif
		   ))
	{
	    // New topline is above old topline: May scroll down.
#ifdef FEAT_FOLDING
	    اگر (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		برای (ln = wp->w_topline; ln < wp->w_سطرها[0].wl_lnum; ++ln)
		{
		    ++j;
		    اگر (j >= wp->w_height - 2)
			بشکن;
		    (تهی)hasFoldingWin(wp, ln, پوچ, &ln, TRUE, پوچ);
		}
	    }
	    وگرنه
#endif
		j = wp->w_سطرها[0].wl_lnum - wp->w_topline;
	    اگر (j < wp->w_height - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_سطرها[0].wl_lnum - 1,
								wp->w_height);
#ifdef FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		اگر (wp->w_سطرها[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_سطرها[0].wl_lnum)
							  - wp->w_old_topfill;
#endif
		اگر (i < wp->w_height - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    اگر (i > 0)
			check_for_delay(FALSE);
		    اگر (win_ins_سطرها(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			اگر (wp->w_سطرها_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    بالاترین_end = i;
			    scrolled_down = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    اگر ((wp->w_سطرها_valid += j) > wp->w_height)
				wp->w_سطرها_valid = wp->w_height;
			    برای (idx = wp->w_سطرها_valid; idx - j >= 0; idx--)
				wp->w_سطرها[idx] = wp->w_سطرها[idx - j];
			    وقتی (idx >= 0)
				wp->w_سطرها[idx--].wl_valid = FALSE;
			}
		    }
		    وگرنه
			mid_start = 0;		// redraw all lines
		}
		وگرنه
		    mid_start = 0;		// redraw all lines
	    }
	    وگرنه
		mid_start = 0;		// redraw all lines
	}
	وگرنه
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    row = 0;
	    برای (i = 0; i < wp->w_سطرها_valid && i < Rows; i++)
	    {
		اگر (wp->w_سطرها[i].wl_valid
			&& wp->w_سطرها[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    بشکن;
		}
		row += wp->w_سطرها[i].wl_size;
	    }
	    اگر (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//سطرها
		mid_start = 0;
	    }
	    وگرنه
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#ifdef FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		اگر (wp->w_سطرها[0].wl_lnum == wp->w_topline)
		    row += wp->w_old_topfill;
		وگرنه
		    row += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		row -= wp->w_topfill;
#endif
		اگر (row > Rows)  // just in case
		    row = Rows;
		اگر (row > 0)
		{
		    check_for_delay(FALSE);
		    اگر (win_del_سطرها(wp, 0, row, FALSE, wp == firstwin, 0)
									 == OK)
			bot_start = wp->w_height - row;
		    وگرنه
			mid_start = 0;		// redraw all lines
		}
		اگر ((row == 0 || bot_start < 999) && wp->w_سطرها_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_start = 0;
		    idx = 0;
		    برای (;;)
		    {
			wp->w_سطرها[idx] = wp->w_سطرها[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			اگر (row > 0 && bot_start + row
				 + (صحیح)wp->w_سطرها[j].wl_size > wp->w_height)
			{
			    wp->w_سطرها_valid = idx + 1;
			    بشکن;
			}
			bot_start += wp->w_سطرها[idx++].wl_size;

			// stop at the last valid entry in w_lines[].wl_size
			اگر (++j >= wp->w_سطرها_valid)
			{
			    wp->w_سطرها_valid = idx;
			    بشکن;
			}
		    }
#ifdef FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    اگر (wp->w_p_diff && bot_start > 0)
			wp->w_سطرها[0].wl_size = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#endif
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	اگر (mid_start == 0)
	    mid_end = wp->w_height;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	اگر (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    وگرنه
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
    }

    اگر (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    اگر ((VIsual_active && buf == curwin->w_buffer)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, to;

	اگر (VIsual_active)
	{
	    اگر (VIsual_mode != wp->w_old_visual_mode
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		اگر (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = VIsual.lnum;
		}
		وگرنه
		{
		    from = VIsual.lnum;
		    to = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		اگر (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		اگر (wp->w_old_cursor_lnum > to)
		    to = wp->w_old_cursor_lnum;
		اگر (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		اگر (wp->w_old_visual_lnum > to)
		    to = wp->w_old_visual_lnum;
	    }
	    وگرنه
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		اگر (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = wp->w_old_cursor_lnum;
		}
		وگرنه
		{
		    from = wp->w_old_cursor_lnum;
		    to = curwin->w_cursor.lnum;
		    اگر (from == 0)	// Visual mode just started
			from = to;
		}

		اگر (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.col != wp->w_old_visual_col)
		{
		    اگر (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    اگر (wp->w_old_visual_lnum > to)
			to = wp->w_old_visual_lnum;
		    اگر (VIsual.lnum < from)
			from = VIsual.lnum;
		    اگر (VIsual.lnum > to)
			to = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    اگر (VIsual_mode == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_END_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		اگر (curwin->w_curswant == MAXCOL)
		{
		    اگر (get_ve_flags() & VE_BLOCK)
		    {
			pos_T	    pos;
			صحیح	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			برای (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.col = (صحیح)ml_get_buf_len(wp->w_buffer, pos.lnum);
			    getvvcol(wp, &pos, پوچ, پوچ, &t, 0);
			    اگر (toc < t)
				toc = t;
			}
			++toc;
		    }
		    وگرنه
			toc = MAXCOL;
		}

		اگر (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    اگر (from > VIsual.lnum)
			from = VIsual.lnum;
		    اگر (to < VIsual.lnum)
			to = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	وگرنه
	{
	    // Use the line numbers of the old Visual area.
	    اگر (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		to = wp->w_old_visual_lnum;
	    }
	    وگرنه
	    {
		from = wp->w_old_visual_lnum;
		to = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	اگر (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	اگر (wp->w_valid & VALID_BOTLINE)
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
	اگر (mid_start > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    اگر (scrolled_down)
		mid_start = بالاترین_end;
	    وگرنه
		mid_start = 0;
	    وقتی (lnum < from && idx < wp->w_سطرها_valid)	// find start
	    {
		اگر (wp->w_سطرها[idx].wl_valid)
		    mid_start += wp->w_سطرها[idx].wl_size;
		وگرنه اگر (!scrolled_down)
		    srow += wp->w_سطرها[idx].wl_size;
		++idx;
#ifdef FEAT_FOLDING
		اگر (idx < wp->w_سطرها_valid && wp->w_سطرها[idx].wl_valid)
		    lnum = wp->w_سطرها[idx].wl_lnum;
		وگرنه
#endif
		    ++lnum;
	    }
	    srow += mid_start;
	    mid_end = wp->w_height;
	    برای ( ; idx < wp->w_سطرها_valid; ++idx)		// find end
	    {
		اگر (wp->w_سطرها[idx].wl_valid
			&& wp->w_سطرها[idx].wl_lnum >= to + 1)
		{
		    // Only update until first row of this line
		    mid_end = srow;
		    بشکن;
		}
		srow += wp->w_سطرها[idx].wl_size;
	    }
	}
    }

    اگر (VIsual_active && buf == curwin->w_buffer)
    {
	wp->w_old_visual_mode = VIsual_mode;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_col = VIsual.col;
	wp->w_old_curswant = curwin->w_curswant;
    }
    وگرنه
    {
	wp->w_old_visual_mode = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_col = 0;
    }

#اگر defined(FEAT_SYN_HL) || defined(FEAT_جستجو_EXTRA)
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
    اگر (winbar_height(wp) > 0)
	redraw_win_toolbar(wp);
#endif

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#ifdef FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_FIELD(spv);
    اگر (spell_check_window(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_بالاترین == 0;
    }
#endif

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    row = 0;
    srow = 0;
    برای (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	اگر (row == wp->w_height)
	{
	    didline = TRUE;
	    بشکن;
	}

	// stop updating when hit the end of the file
	اگر (lnum > buf->b_ml.ml_سطر_شمار)
	{
	    eof = TRUE;
	    بشکن;
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
	اگر (row < بالاترین_end
		|| (row >= mid_start && row < mid_end)
#ifdef FEAT_جستجو_EXTRA
		|| بالاترین_to_mod
#endif
		|| idx >= wp->w_سطرها_valid
		|| (row + wp->w_سطرها[idx].wl_size > bot_start)
		|| (mod_بالاترین != 0
		    && (lnum == mod_بالاترین
			|| (lnum >= mod_بالاترین
			    && (lnum < mod_bot
#ifdef FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_سطر
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_changed(lnum)))
#endif
#ifdef FEAT_جستجو_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_match_head != پوچ
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
#ifdef FEAT_جستجو_EXTRA
	    اگر (lnum == mod_بالاترین)
		بالاترین_to_mod = FALSE;
#endif

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    اگر (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_بالاترین && lnum < MAX(mod_bot, mod_بالاترین + 1)
		    && (!scrolled_down || row >= بالاترین_end))
	    {
		scrolled_for_mod = TRUE;

		صحیح		old_cline_height = 0;
		صحیح		old_rows = 0;
		صحیح		new_rows = 0;
		صحیح		xtra_rows;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		برای (i = idx; i < wp->w_سطرها_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    اگر (wp->w_سطرها[i].wl_valid
			    && wp->w_سطرها[i].wl_lnum == mod_bot)
			بشکن;
		    اگر (wp->w_سطرها[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_height = wp->w_سطرها[i].wl_size;
		    old_rows += wp->w_سطرها[i].wl_size;
#ifdef FEAT_FOLDING
		    اگر (wp->w_سطرها[i].wl_valid
			    && wp->w_سطرها[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			وقتی (i < wp->w_سطرها_valid
						  && !wp->w_سطرها[i].wl_valid)
			    old_rows += wp->w_سطرها[i++].wl_size;
			بشکن;
		    }
#endif
		}

		اگر (i >= wp->w_سطرها_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_start = 0;
		}
		وگرنه
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    برای (l = lnum; l < mod_bot; ++l)
		    {
			اگر (dollar_vcol >= 0 && wp == curwin &&
				old_cline_height > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_rows += old_cline_height;
#ifdef FEAT_FOLDING
			وگرنه اگر (hasFoldingWin(wp, l, پوچ, &l, TRUE, پوچ))
			    ++new_rows;
#endif
			وگرنه
			    new_rows += plines_correct_topline(wp, l, TRUE);
			++j;
			اگر (new_rows > wp->w_height - row - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_rows = 9999;
			    بشکن;
			}
		    }
		    xtra_rows = new_rows - old_rows;
		    اگر (xtra_rows < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			اگر (row - xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			وگرنه
			{
			    check_for_delay(FALSE);
			    اگر (win_del_سطرها(wp, row,
					  -xtra_rows, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    وگرنه
				bot_start = wp->w_height + xtra_rows;
			}
		    }
		    وگرنه اگر (xtra_rows > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			اگر (row + xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			وگرنه
			{
			    check_for_delay(FALSE);
			    اگر (win_ins_سطرها(wp, row + old_rows,
					     xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    وگرنه اگر (بالاترین_end > row + old_rows)
				// Scrolled the part at the top that requires
				// updating down.
				بالاترین_end += xtra_rows;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    اگر (mod_bot != MAXLNUM && i != j)
		    {
			اگر (j < i)
			{
			    صحیح x = row + new_rows;

			    // move entries in w_lines[] upwards
			    برای (;;)
			    {
				// stop at last valid entry in w_lines[]
				اگر (i >= wp->w_سطرها_valid)
				{
				    wp->w_سطرها_valid = j;
				    بشکن;
				}
				wp->w_سطرها[j] = wp->w_سطرها[i];
				// stop at a line that won't fit
				اگر (x + (صحیح)wp->w_سطرها[j].wl_size
							   > wp->w_height)
				{
				    wp->w_سطرها_valid = j + 1;
				    بشکن;
				}
				x += wp->w_سطرها[j++].wl_size;
				++i;
			    }
			    اگر (bot_start > x)
				bot_start = x;
			}
			وگرنه // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_سطرها_valid += j;
			    اگر (wp->w_سطرها_valid > wp->w_height)
				wp->w_سطرها_valid = wp->w_height;
			    برای (i = wp->w_سطرها_valid; i - j >= idx; --i)
				wp->w_سطرها[i] = wp->w_سطرها[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    وقتی (i >= idx)
			    {
				wp->w_سطرها[i].wl_size = 0;
				wp->w_سطرها[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ifdef FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_شمار = folded_شمار(wp, lnum, &win_foldinfo);
	    اگر (fold_شمار != 0)
	    {
		fold_سطر(wp, fold_شمار, &win_foldinfo, lnum, row);
		++row;
		--fold_شمار;
		wp->w_سطرها[idx].wl_folded = TRUE;
		wp->w_سطرها[idx].wl_lastlnum = lnum + fold_شمار;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
# ifdef FEAT_SPELL
		spv.spv_capcol_lnum = 0;
# endif
	    }
	    وگرنه
#endif
	    اگر (idx < wp->w_سطرها_valid
		    && wp->w_سطرها[idx].wl_valid
		    && wp->w_سطرها[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_سطرها[idx].wl_size > wp->w_height
#ifdef FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#endif
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		row = wp->w_height + 1;
	    }
	    وگرنه
	    {
#ifdef FEAT_جستجو_EXTRA
		prepare_جستجو_hl(wp, &screen_جستجو_hl, lnum);
#endif
#ifdef FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		اگر (syntax_last_parsed != 0 && syntax_last_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

		// Display one line.
		row = win_سطر(wp, lnum, srow, wp->w_height, 0, &spv);

#ifdef FEAT_FOLDING
		wp->w_سطرها[idx].wl_folded = FALSE;
		wp->w_سطرها[idx].wl_lastlnum = lnum;
#endif
#ifdef FEAT_SYN_HL
		did_update = DID_سطر;
		syntax_last_parsed = lnum;
#endif
	    }

	    wp->w_سطرها[idx].wl_lnum = lnum;
	    wp->w_سطرها[idx].wl_valid = TRUE;

	    صحیح is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    اگر (row > wp->w_height || row + wp->w_winrow >= Rows)
	    {
		// we may need the size of that too long line later on
		اگر (dollar_vcol == -1 || !is_curline)
		    wp->w_سطرها[idx].wl_size = plines_win(wp, lnum, TRUE);
		++idx;
		بشکن;
	    }
	    اگر (dollar_vcol == -1 || !is_curline)
		wp->w_سطرها[idx].wl_size = row - srow;
	    ++idx;
#ifdef FEAT_FOLDING
	    lnum += fold_شمار + 1;
#وگرنه
	    ++lnum;
#endif
	}
	وگرنه
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    اگر ((wp->w_p_nu && mod_بالاترین != 0 && lnum >= mod_bot
			&& buf->b_mod_set && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_last_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#ifdef FEAT_FOLDING
		fold_شمار = folded_شمار(wp, lnum, &win_foldinfo);
		اگر (fold_شمار != 0)
		    fold_سطر(wp, fold_شمار, &win_foldinfo, lnum, row);
		وگرنه
#endif
		    (تهی)win_سطر(wp, lnum, srow, wp->w_height,
					       wp->w_سطرها[idx].wl_size, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    row += wp->w_سطرها[idx++].wl_size;
	    اگر (row > wp->w_height)	// past end of screen
		بشکن;
#ifdef FEAT_FOLDING
	    lnum = wp->w_سطرها[idx - 1].wl_lastlnum + 1;
#وگرنه
	    ++lnum;
#endif
#ifdef FEAT_SYN_HL
	    did_update = DID_NONE;
#endif
#ifdef FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#endif
	}

	اگر (lnum > buf->b_ml.ml_سطر_شمار)
	{
	    eof = TRUE;
	    بشکن;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	اگر (idx >= Rows)
	    بشکن;
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
    اگر (use_vtp() && get_conpty_fix_type() < 1)
    {
	صحیح k;

	برای (k = 0; k < Rows; ++k)
	    اگر (enc_utf8)
		اگر ((*mb_off2cells)(سطر_Offset[k] + topframe->fr_width - 2,
					   سطر_Offset[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_width - 2, 1, 2,
			    FALSE, FALSE);
		وگرنه
		    screen_draw_rectangle(k, topframe->fr_width - 1, 1, 1,
			    FALSE, FALSE);
	    وگرنه
		screen_char(سطر_Offset[k] + topframe->fr_width - 1, k,
			cmdline_width - 1);
    }
#endif

    اگر (idx > wp->w_سطرها_valid)
	wp->w_سطرها_valid = idx;

#ifdef FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    اگر (syntax_last_parsed != 0 && syntax_present(wp))
	syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_empty_rows = 0;
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    اگر (!eof && !didline)
    {
	اگر (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#ifdef FEAT_DIFF
	وگرنه اگر (diff_check_fill(wp, lnum) >= wp->w_height - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_rows = wp->w_height - srow;
	}
#endif
#ifdef FEAT_PROP_POPUP
	وگرنه اگر (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#endif
	وگرنه اگر (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    صحیح		scr_row = W_WINROW(wp) + wp->w_height - 1;
	    صحیح		نماد  = wp->w_fill_chars.lastline;
	    صحیح		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(نماد, &fillbuf[0]);
	    mb_char2bytes(نماد, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_width > 2 ? 2 : wp->w_width) * charlen,
			    scr_row, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_row, scr_row + 1,
		    (صحیح)wp->w_wincol + 2, (صحیح)W_ENDCOL(wp),
		    نماد, ' ', HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	وگرنه اگر (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    صحیح start_col = (صحیح)W_ENDCOL(wp) - 3;
	    صحیح نماد    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_height - 1,
		    W_WINROW(wp) + wp->w_height,
		    (start_col < wp->w_wincol ? wp->w_wincol : start_col),
		    (صحیح)W_ENDCOL(wp),
		    نماد, نماد, HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	وگرنه
	{
	    win_draw_end(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_height, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    وگرنه
    {
	draw_vsep_win(wp, row);
	اگر (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_سطر_شمار + 1;
#ifdef FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    اگر (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		اگر (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		وگرنه
		    i = wp->w_fill_chars.diff;
		اگر (row + j > wp->w_height)
		    j = wp->w_height - row;
		win_draw_end(wp, i, i, TRUE, row, row + (صحیح)j, HLF_DED);
		row += j;
	    }
#endif
	}
	وگرنه اگر (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	اگر (WIN_IS_POPUP(wp))
	    win_draw_end(wp, ' ', ' ', FALSE, row, wp->w_height, HLF_AT);
	وگرنه
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

    اگر (dollar_vcol == -1 || wp != curwin)
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
	اگر (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#اگر defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    صحیح		new_wcol = wp->w_wcol;
#endif
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#اگر defined(FEAT_CONCEAL)
	    اگر (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
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
		اگر (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		وگرنه
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#endif
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    اگر (wp->w_redr_type != 0)
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
		اگر (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#اگر defined(FEAT_SYN_HL) || defined(FEAT_جستجو_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    اگر (!got_int)
	got_int = save_got_int;
#endif

    اگر (override_success)
	برداشت_highlight_overrides();
}

#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    ایستا تهی
update_prepare(تهی)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    اگر (gui.in_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_جستجو_EXTRA
    start_جستجو_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_mask(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    ایستا تهی
update_finish(تهی)
{
    اگر (redraw_cmdline || redraw_mode)
	showmode();

# ifdef FEAT_جستجو_EXTRA
    end_جستجو_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    اگر (gui.in_use)
    {
	out_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#endif

#اگر defined(FEAT_NETBEANS_INTG)
    تهی
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    صحیح		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_سطر_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    اگر (!doit || updating_screen
	    || State == MODE_ASKMORE || State == MODE_HITRETURN
	    || پیام_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	بازگشت;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_redr_type != 0)
	    win_update(wp);
	اگر (wp->w_redr_status)
	    win_redr_status(wp, FALSE);
    }

# اگر defined(FEAT_TABPANEL)
    اگر (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_finish();
}
#endif

#اگر defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    تهی
updateWindow(win_T *wp)
{
    // return if already busy updating
    اگر (updating_screen)
	بازگشت;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    اگر (clip_star.available && clip_isautosel_star())
	clip_update_selection(&clip_star);
    اگر (clip_plus.available && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    اگر (redraw_tabline)
	draw_tabline();

# اگر defined(FEAT_TABPANEL)
    اگر (redraw_tabpanel)
	draw_tabpanel();
# endif

    اگر (wp->w_redr_status || p_ru
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
    صحیح
redraw_asap(صحیح type)
{
    صحیح		rows;
    صحیح		cols = screen_Columns;
    صحیح		r;
    صحیح		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    صحیح		i;
    u8char_T	*screenlineUC = پوچ;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = پوچ;	// copy from ScreenLines2[]

    redraw_later(type);
    اگر (پیام_scrolled
	    || (State != MODE_NORMAL && State != MODE_NORMAL_BUSY)
	    || exiting)
	بازگشت ret;

    // Allocate space to save the text displayed in the command line area.
    rows = screen_Rows - cmdline_row;
    screenline = LALLOC_MULT(schar_T, rows * cols);
    screenattr = LALLOC_MULT(sattr_T, rows * cols);
    اگر (screenline == پوچ || screenattr == پوچ)
	ret = 2;
    اگر (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, rows * cols);
	اگر (screenlineUC == پوچ)
	    ret = 2;
	برای (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, rows * cols);
	    اگر (screenlineC[i] == پوچ)
		ret = 2;
	}
    }
    اگر (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, rows * cols);
	اگر (screenline2 == پوچ)
	    ret = 2;
    }

    اگر (ret != 2)
    {
	// Save the text displayed in the command line area.
	برای (r = 0; r < rows; ++r)
	{
	    mch_memmove(screenline + r * cols,
			Screen_سطرها + سطر_Offset[cmdline_row + r],
			(size_t)cols * اندازه(schar_T));
	    mch_memmove(screenattr + r * cols,
			ScreenAttrs + سطر_Offset[cmdline_row + r],
			(size_t)cols * اندازه(sattr_T));
	    اگر (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * cols,
			    Screen_سطرها_UC + سطر_Offset[cmdline_row + r],
			    (size_t)cols * اندازه(u8char_T));
		برای (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * cols,
				Screen_سطرها_C[i] + سطر_Offset[cmdline_row + r],
				(size_t)cols * اندازه(u8char_T));
	    }
	    اگر (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * cols,
			    ScreenLines2 + سطر_Offset[cmdline_row + r],
			    (size_t)cols * اندازه(schar_T));
	}

	update_screen(0);
	ret = 3;

	اگر (must_redraw == 0)
	{
	    صحیح	off = (صحیح)(current_Screen_سطر - Screen_سطرها);

	    // Restore the text displayed in the command line area.
	    برای (r = 0; r < rows; ++r)
	    {
		mch_memmove(current_Screen_سطر,
			    screenline + r * cols,
			    (size_t)cols * اندازه(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * cols,
			    (size_t)cols * اندازه(sattr_T));
		اگر (enc_utf8)
		{
		    mch_memmove(Screen_سطرها_UC + off,
				screenlineUC + r * cols,
				(size_t)cols * اندازه(u8char_T));
		    برای (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_سطرها_C[i] + off,
				    screenlineC[i] + r * cols,
				    (size_t)cols * اندازه(u8char_T));
		}
		اگر (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * cols,
				(size_t)cols * اندازه(schar_T));
		screen_سطر(curwin, cmdline_row + r, 0, cols, cols, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    اگر (enc_utf8)
    {
	vim_free(screenlineUC);
	برای (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    اگر (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_message();

    setcursor();

    بازگشت ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    تهی
redraw_after_callback(صحیح call_update_screen, صحیح do_message)
{
    ++redrawing_for_callback;

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

    اگر (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_SETWSIZE || State == MODE_EXTERNCMD
	    || State == MODE_CONFIRM || exmode_active)
    {
	اگر (do_message)
	    repeat_message();
    }
    وگرنه اگر (State & MODE_CMDLINE)
    {
	اگر (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	اگر (cmdline_row > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    اگر (پیام_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    وگرنه اگر (State & (MODE_NORMAL | MODE_درج | MODE_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	اگر (پیام_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    پیام_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
#ifdef FEAT_GUI
    اگر (gui.in_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	out_flush_cursor(FALSE, FALSE);
    وگرنه
#endif
	out_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    تهی
redraw_later(صحیح type)
{
    redraw_win_later(curwin, type);
}

    تهی
redraw_win_later(
    win_T	*wp,
    صحیح		type)
{
#ifdef FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    اگر (inside_redraw_on_start_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    وگرنه
#endif
    اگر (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	اگر (type >= UPD_NOT_VALID)
	    wp->w_سطرها_valid = 0;
	اگر (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    تهی
redraw_later_clear(تهی)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    تهی
redraw_all_later(صحیح type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    set_must_redraw(type);
}

#اگر 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    تهی
redraw_all_windows_later(صحیح type)
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
    تهی
set_must_redraw(صحیح type)
{
    اگر (!redraw_not_allowed &&
#ifdef FEAT_EVAL
	    !inside_redraw_on_start_cb &&
#endif
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    تهی
redraw_curbuf_later(صحیح type)
{
    redraw_buf_later(curbuf, type);
}

    تهی
redraw_buf_later(buf_T *buf, صحیح type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    }
#ifdef FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	اگر (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
#endif
#اگر defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    اگر (curwin->w_buffer == buf)
	redraw_win_later(curwin, type);
#endif
}

#اگر defined(FEAT_SIGNS)
    تهی
redraw_buf_سطر_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_buffer == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#endif

#اگر defined(FEAT_JOB_CHANNEL)
    تهی
redraw_buf_and_status_later(buf_T *buf, صحیح type)
{
    win_T	*wp;

    اگر (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	بازگشت;
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_buffer == buf)
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
    ایستا تهی
ruler_redraw_lastwin(تهی)
{
    اگر (p_ru && lastwin->w_status_height == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    تهی
status_redraw_all(تهی)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_status_height)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    تهی
status_redraw_curbuf(تهی)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_status_height != 0 && wp->w_buffer == curbuf)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    اگر (lastwin->w_buffer == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    تهی
redraw_statuslines(تهی)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_redr_status)
	{
	    bool ret = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_status(wp, FALSE);
	    اگر (ret)
		برداشت_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    اگر (redraw_tabline)
	draw_tabline();

#اگر defined(FEAT_TABPANEL)
    اگر (redraw_tabpanel)
	draw_tabpanel();
#endif
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    تهی
win_redraw_last_status(frame_T *frp)
{
    اگر (frp->fr_layout == FR_LEAF)
	frp->fr_win->w_redr_status = true;
    وگرنه اگر (frp->fr_layout == FR_ROW)
    {
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    win_redraw_last_status(frp);
    }
    وگرنه // frp->fr_layout == FR_COL
    {
	frp = frp->fr_child;
	وقتی (frp->fr_next != پوچ)
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
    تهی
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    تهی
redraw_win_range_later(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    اگر (last >= wp->w_topline && first < wp->w_botline)
    {
	اگر (wp->w_redraw_بالاترین == 0 || wp->w_redraw_بالاترین > first)
	    wp->w_redraw_بالاترین = first;
	اگر (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    تهی
redraw_win_range_now(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    اگر (last >= wp->w_topline && first < wp->w_botline)
    {
	اگر (wp->w_redraw_بالاترین == 0 || wp->w_redraw_بالاترین > first)
	    wp->w_redraw_بالاترین = first;
	اگر (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	اگر (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#ifdef FEAT_EVAL
ایستا bool redraw_cb_in_progress = false;

    تهی
f_redraw_listener_add(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    ایستا صحیح		id;

    اگر (check_secure())
	بازگشت;

    اگر (redraw_cb_in_progress)
    {
	emsg(_(e_cannot_add_redraw_listener_in_listener_callback));
	بازگشت;
    }

    اگر (check_for_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    اگر (rln == پوچ)
	بازگشت;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    اگر (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	اگر (cb.cb_name == پوچ)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    بازگشت;
	}
	copy_callback(&rln->rl_callbacks.on_start, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    اگر (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	اگر (cb.cb_name == پوچ)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_start);
	    vim_free(rln);
	    بازگشت;
	}
	copy_callback(&rln->rl_callbacks.on_end, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    اگر (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	بازگشت;
    }

    rln->rl_next = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_عدد;
    rettv->vval.v_عدد = id;

    بازگشت;
}

    ایستا تهی
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_start);
    free_callback(&rln->rl_callbacks.on_end);

    vim_free(rln);
}


    ایستا تهی
redraw_listener_cleanup(تهی)
{
    برای (redraw_listener_T *rln = redraw_listeners; rln != پوچ;)
    {
	redraw_listener_T *next = rln->rl_next;
	اگر (rln->rl_id == 0)
	{
	    اگر (redraw_listeners == rln)
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
    ایستا redraw_listener_T *
get_redraw_listener(صحیح id)
{
    برای (redraw_listener_T *rln = redraw_listeners; rln != پوچ; rln = rln->rl_next)
	اگر (rln->rl_id == id)
	    بازگشت rln;
    بازگشت پوچ;
}

    تهی
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv UNUSED)
{
    صحیح			id;
    redraw_listener_T	*rln;

    اگر (check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    id = argvars[0].vval.v_عدد;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_عدد;
    اگر (rln == پوچ)
    {
	rettv->vval.v_عدد = 0;
	بازگشت;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_عدد = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    ایستا تهی
invoke_redraw_listener_start_or_end(bool start)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    اگر (start)
	inside_redraw_on_start_cb = true;

    redraw_cb_in_progress = true;
    برای (redraw_listener_T *rln = redraw_listeners; rln != پوچ; rln = rln->rl_next)
    {
	اگر (rln->rl_id == 0)
	    // Listener has been removed, skip
	    ادامه;
	اگر (start && rln->rl_callbacks.on_start.cb_name != پوچ)
	{
	    call_callback(&rln->rl_callbacks.on_start, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	وگرنه اگر (rln->rl_callbacks.on_end.cb_name != پوچ)
	{
	    call_callback(&rln->rl_callbacks.on_end, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_in_progress = false;

    اگر (start)
	inside_redraw_on_start_cb = false;
}
#endif // FEAT_EVAL
