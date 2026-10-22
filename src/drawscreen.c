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

#include "vim.h"

static void win_update(win_T *wp);
#ifdef FEAT_STL_OPT
static void redraw_custom_statusline(win_T *wp);
#endif
static void borrow_stl_vsep_hl(void);
#གལ་ཏེ defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
static ཧྲིལ  did_update_one_window;
#endif

#ifdef FEAT_EVAL
static void redraw_listener_cleanup(void);
static void invoke_redraw_listener_start_or_end(bool start);
#endif

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    ཧྲིལ
update_screen(ཧྲིལ type_arg)
{
    ཧྲིལ		type = type_arg;
    win_T	*wp;
    static ཧྲིལ	did_intro = FALSE;
#ifdef FEAT_GUI
    ཧྲིལ		did_one = FALSE;
    ཧྲིལ		did_undraw = FALSE;
    ཧྲིལ		gui_cursor_col = 0;
    ཧྲིལ		gui_cursor_row = 0;
#endif
    ཧྲིལ		no_update = FALSE;
    ཧྲིལ		save_pum_will_redraw = pum_will_redraw;
#ifdef FEAT_PROP_POPUP
    ཧྲིལ		did_redraw_window = FALSE;
#endif
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    གལ་ཏེ (!screen_valid(TRUE))
	ཕྱིར FAIL;

    གལ་ཏེ (type == UPD_VALID_NO_UPDATE)
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
    གལ་ཏེ (need_diff_redraw)
	diff_redraw(TRUE);
#endif

    གལ་ཏེ (must_redraw)
    {
	གལ་ཏེ (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    གལ་ཏེ (curwin->w_lines_valid == 0 && type < UPD_NOT_VALID
#ifdef FEAT_TERMINAL
	    && !term_do_update_window(curwin)
#endif
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    གལ་ཏེ (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	གལ་ཏེ (type > UPD_INVERTED_ALL)
	    curwin->w_lines_valid = 0;	// don't use w_lines[].wl_size now
	ཕྱིར FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    ཧྲིལ hid_cursor = !sync_output_active();
    གལ་ཏེ (hid_cursor)
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
    གལ་ཏེ (no_update)
	++no_win_do_lines_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    གལ་ཏེ (msg_scrolled)
    {
	clear_cmdline = TRUE;
	གལ་ཏེ (type != UPD_CLEAR)
	{
	    གལ་ཏེ (msg_scrolled > Rows - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    དེ་མིན
	    {
		check_for_delay(FALSE);
		གལ་ཏེ (screen_ins_lines(0, 0, msg_scrolled, (ཧྲིལ)Rows, 0, NULL)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    གལ་ཏེ (wp->w_winrow < msg_scrolled)
		    {
			གལ་ཏེ (W_WINROW(wp) + wp->w_height > msg_scrolled
				&& wp->w_redr_type < UPD_REDRAW_TOP
				&& wp->w_lines_valid > 0
				&& wp->w_topline == wp->w_lines[0].wl_lnum)
			{
			    wp->w_upd_rows = msg_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_TOP;
			}
			དེ་མིན
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    གལ་ཏེ (W_WINROW(wp) + wp->w_height
					 + wp->w_status_height <= msg_scrolled)
				wp->w_redr_status = true;
			}
		    }
		}
		གལ་ཏེ (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#གལ་ཏེ defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#endif
	}
	msg_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    གལ་ཏེ (need_highlight_changed)
	highlight_changed();

    གལ་ཏེ (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    གལ་ཏེ (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    གལ་ཏེ (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? number_width(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#endif

#ifdef FEAT_EVAL
    invoke_redraw_listener_start_or_end(true);
#endif

    // Only start redrawing if there is really something to do.
    གལ་ཏེ (type == UPD_INVERTED)
	update_curswant();
    གལ་ཏེ (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_lines[0].wl_valid
#ifdef FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#endif
		    && curwin->w_topline == curwin->w_lines[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_active
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_mode == VIsual_mode
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    གལ་ཏེ (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#གལ་ཏེ defined(FEAT_TABPANEL)
    གལ་ཏེ (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#endif

#ifdef FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	གལ་ཏེ (wp->w_buffer->b_mod_set)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    ཆེད་དུ (wwp = firstwin; wwp != wp; wwp = wwp->w_next)
		གལ་ཏེ (wwp->w_buffer == wp->w_buffer)
		    bha_nga;
	    གལ་ཏེ (wwp == wp && syntax_present(wp))
		syn_stack_apply_changes(wp->w_buffer);
	}
    }
#endif

    གལ་ཏེ (pum_redraw_in_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#གལ་ཏེ defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_window = FALSE;
#endif
#ifdef FEAT_SEARCH_EXTRA
    screen_search_hl.rm.regprog = NULL;
#endif
    FOR_ALL_WINDOWS(wp)
    {
	override_success = push_highlight_overrides(wp->w_hl, wp->w_hl_len);

	གལ་ཏེ (wp->w_redr_type != 0)
	{
	    cursor_off();
#ifdef FEAT_PROP_POPUP
	    did_redraw_window = TRUE;
#endif
#ifdef FEAT_GUI
	    གལ་ཏེ (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		གལ་ཏེ (gui.in_use && (wp == curwin || cursor_is_sleeping()))
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
	གལ་ཏེ (wp->w_redr_status)
	{
	    cursor_off();
	    win_redr_status(wp, TRUE); // any popup menu will be redrawn below
	}

	གལ་ཏེ (override_success)
	    pop_highlight_overrides();

    }
#གལ་ཏེ defined(FEAT_SEARCH_EXTRA)
    end_search_hl();
#endif

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    གལ་ཏེ (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    གལ་ཏེ (wp->w_vsep_width > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_buffer->b_mod_set = false;

#ifdef FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    གལ་ཏེ (did_redraw_window || popup_need_redraw())
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
    གལ་ཏེ (clear_cmdline || redraw_cmdline || redraw_mode)
	showmode();

    གལ་ཏེ (no_update)
	--no_win_do_lines_ins;

    // May put up an introductory message when not editing a file
    གལ་ཏེ (!did_intro)
	maybe_intro_message();
    did_intro = TRUE;

#ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    གལ་ཏེ (gui.in_use)
    {
	གལ་ཏེ (did_undraw && !gui_mch_is_blink_off())
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
	དེ་མིན
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
    གལ་ཏེ (hid_cursor)
	cursor_on();

    ཕྱིར OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    ཧྲིལ
statusline_row(win_T *wp)
{
#གལ་ཏེ defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    གལ་ཏེ (wp->w_frame->fr_height == wp->w_status_height && !popup_is_popup(wp))
	ཕྱིར wp->w_winrow;
#endif
    ཕྱིར W_WINROW(wp) + wp->w_height;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    void
win_redr_status(win_T *wp, ཧྲིལ ignore_pum UNUSED)
{
    ཧྲིལ		row;
    ཧྲིལ		fillchar;
    ཧྲིལ		attr;
    ཧྲིལ		i;
    static ཧྲིལ  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    གལ་ཏེ (busy)
	ཕྱིར;
    busy = TRUE;

    row = statusline_row(wp);

    wp->w_redr_status = false;
    གལ་ཏེ (wp->w_status_height == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    དེ་མིན གལ་ཏེ (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_status = true;
    }
#ifdef FEAT_STL_OPT
    དེ་མིན གལ་ཏེ (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#endif
    དེ་མིན
    {
	char_u	*p;
	ཧྲིལ	plen;
	ཧྲིལ	NameBufflen;
	ཧྲིལ	this_ru_col;
	ཧྲིལ	n;			// scratch value

	fillchar = fillchar_status(&attr, wp);

	get_trans_bufname(wp->w_buffer);
	p = NameBuff;
	plen = (ཧྲིལ)STRLEN(p);

	གལ་ཏེ ((bt_help(wp->w_buffer)
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
	གལ་ཏེ (bt_help(wp->w_buffer))
	    plen += vim_snprintf((var_na *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#ifdef FEAT_QUICKFIX
	གལ་ཏེ (wp->w_p_pvw)
	    plen += vim_snprintf((var_na *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#endif
	གལ་ཏེ (bufIsChanged(wp->w_buffer) && !bt_terminal(wp->w_buffer))
	    plen += vim_snprintf((var_na *)p + plen, MAXPATHL - plen, "%s", "[+]");
	གལ་ཏེ (wp->w_buffer->b_p_ro)
	    plen += vim_snprintf((var_na *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_col = ru_col - (cmdline_width - wp->w_width);
	n = (wp->w_width + 1) / 2;
	གལ་ཏེ (this_ru_col < n)
	    this_ru_col = n;
	གལ་ཏེ (this_ru_col <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	དེ་མིན གལ་ཏེ (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    ཆེད་དུ (i = 0; p[i] != NUL && plen >= this_ru_col - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    གལ་ཏེ (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	དེ་མིན གལ་ཏེ (plen > this_ru_col - 1)
	{
	    p += plen - (this_ru_col - 1);
	    *p = '<';
	    plen = this_ru_col - 1;
	}

	screen_puts(p, row, wp->w_wincol, attr);
	screen_fill(row, row + 1, plen + wp->w_wincol,
		    this_ru_col + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	ཆེད་དུ (i = 1; i < wp->w_status_height; i++)
	    screen_fill(row + i, row + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	གལ་ཏེ ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_col - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, row, (ཧྲིལ)(this_ru_col - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	གལ་ཏེ (p_sc && *p_sloc == 's')
	{
	    n = this_ru_col - plen - 2;		    // perform the calculation here so we only do it once
	    ཧྲིལ	width = MIN(10, n);

	    གལ་ཏེ (width > 0)
		screen_puts_len(showcmd_buf, width, row,
				wp->w_wincol + this_ru_col - width - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    གལ་ཏེ (wp->w_vsep_width != 0 && wp->w_status_height != 0 && redrawing())
    {
	ཆེད་དུ (i = 0; i < wp->w_status_height; i++)
	{
	    ཧྲིལ r = row + i;
	    གལ་ཏེ (stl_connected(wp))
		fillchar = fillchar_status(&attr, wp);
	    དེ་མིན
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
    static void
borrow_stl_vsep_hl(void)
{
    win_T   *left = NULL;
    win_T   *right = NULL;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    གལ་ཏེ (!redrawing() || ScreenLines == NULL)
	ཕྱིར;

    FOR_ALL_WINDOWS(left)
    {
	གལ་ཏེ (left->w_status_height == 0 || left->w_vsep_width == 0)
	    continue;
	// A window of zero width has no status line cell to borrow from.
	གལ་ཏེ (left->w_width == 0)
	    continue;
	གལ་ཏེ (!stl_connected(left))
	    continue;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = NULL;
	ཧྲིལ start = 0;
	ཧྲིལ end = 0;

	FOR_ALL_WINDOWS(right)
	{
	    གལ་ཏེ (right == left || right->w_status_height == 0)
		continue;
	    གལ་ཏེ (right->w_wincol != W_ENDCOL(left) + 1)
		continue;
	    ཧྲིལ l_stl_row = W_WINROW(left) + left->w_height;
	    ཧྲིལ r_stl_row = W_WINROW(right) + right->w_height;

	    start = l_stl_row > r_stl_row ? l_stl_row : r_stl_row;
	    end = l_stl_row + left->w_status_height
				< r_stl_row + right->w_status_height
		? l_stl_row + left->w_status_height
		: r_stl_row + right->w_status_height;
	    གལ་ཏེ (start < end)
	    {
		neighbour = right;
		bha_nga;
	    }
	}
	གལ་ཏེ (neighbour == NULL)
	    continue;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	ཧྲིལ	hl;
	གལ་ཏེ (left != curwin && neighbour != curwin
		&& fillchar_status(&hl, left) != ' ')
	    continue;

	// Source: prefer curwin's side; otherwise left window's right edge.
	ཧྲིལ dst_col = W_ENDCOL(left);
	ཧྲིལ src_col = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(left) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	གལ་ཏེ (dst_col >= screen_Columns || src_col >= screen_Columns)
	    continue;
	གལ་ཏེ (end > screen_Rows)
	    end = screen_Rows;

	ཆེད་དུ (ཧྲིལ r = start; r < end; r++)
	{
	    unsigned	dst_off = LineOffset[r] + dst_col;
	    sattr_T	attr = ScreenAttrs[LineOffset[r] + src_col];

	    གལ་ཏེ (ScreenAttrs[dst_off] != attr)
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
    static void
redraw_custom_statusline(win_T *wp)
{
    static ཧྲིལ	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    གལ་ཏེ (entered)
	ཕྱིར;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#endif

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    void
showruler(ཧྲིལ always)
{
    གལ་ཏེ (!always && !redrawing())
	ཕྱིར;
    གལ་ཏེ (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_status = true;
	ཕྱིར;
    }
#གལ་ཏེ defined(FEAT_STL_OPT)
    གལ་ཏེ ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_status_height)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    དེ་མིན
#endif
	win_redr_ruler(curwin, always, FALSE);

    གལ་ཏེ (need_maketitle
#ifdef FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_IN_ICON))
	    || (p_title && (stl_syntax & STL_IN_TITLE))
#endif
       )
	maketitle();

    // Redraw the tab pages line if needed.
    གལ་ཏེ (redraw_tabline)
	draw_tabline();

#གལ་ཏེ defined(FEAT_TABPANEL)
    གལ་ཏེ (redraw_tabpanel)
	draw_tabpanel();
#endif
}

    void
win_redr_ruler(win_T *wp, ཧྲིལ always, ཧྲིལ ignore_pum)
{
    ཧྲིལ	empty_line = FALSE;

    // If 'ruler' off don't do anything
    གལ་ཏེ (!p_ru)
	ཕྱིར;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    གལ་ཏེ (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_line_count)
	ཕྱིར;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    གལ་ཏེ (wp == lastwin && lastwin->w_status_height == 0)
	གལ་ཏེ (edit_submode != NULL)
	    ཕྱིར;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    གལ་ཏེ (!ignore_pum && pum_visible())
	ཕྱིར;

#ifdef FEAT_STL_OPT
    གལ་ཏེ (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	ཕྱིར;
    }
#endif

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    གལ་ཏེ ((State & MODE_INSERT) == 0
		&& *ml_get_buf(wp->w_buffer, wp->w_cursor.lnum, FALSE) == NUL)
	empty_line = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    གལ་ཏེ (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.col != wp->w_ru_cursor.col
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_buffer->b_ml.ml_line_count != wp->w_ru_line_count
#ifdef FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#endif
	    || empty_line != wp->w_ru_empty)
    {
	ཧྲིལ	row;
	ཧྲིལ	fillchar;
	ཧྲིལ	attr;
	ཧྲིལ	off;
	ཧྲིལ	width;
	colnr_T	virtcol;
#define RULER_BUF_LEN 70
	char_u	buffer[RULER_BUF_LEN];
	ཧྲིལ	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	ཧྲིལ	rel_poslen;
	ཧྲིལ	this_ru_col;
	ཧྲིལ	n1;			    // scratch value
	ཧྲིལ	n2;			    // scratch value
	bool	override_success =
	    push_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	གལ་ཏེ (wp->w_status_height)
	{
	    row = statusline_row(wp);
	    fillchar = fillchar_status(&attr, wp);
	    off = wp->w_wincol;
	    width = wp->w_width;
	}
	དེ་མིན
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_col_off;
	    width = cmdline_width;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	གལ་ཏེ (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, NULL, &virtcol, NULL, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((var_na *)buffer, RULER_BUF_LEN, _("%ld,"),
		(wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		    ? 0L
		    : (long)(wp->w_cursor.lnum));
	bufferlen += col_print(buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			empty_line ? 0 : (ཧྲིལ)wp->w_cursor.col + 1,
			(ཧྲིལ)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	གལ་ཏེ (wp->w_status_height == 0)	// can't use last char of screen
	    ++n1;

	this_ru_col = ru_col - (cmdline_width - width);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (width + 1) / 2;
	གལ་ཏེ (this_ru_col < n2)
	    this_ru_col = n2;
	གལ་ཏེ (this_ru_col + n1 < width)
	{
	    // need at least space for rel_pos + NUL
	    སྐབས (this_ru_col + n1 < width
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		གལ་ཏེ (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, buffer + bufferlen);
		དེ་མིན
		    buffer[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((var_na *)buffer + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	གལ་ཏེ (has_mbyte)
	{
	    ཆེད་དུ (n1 = 0, n2 = 0; buffer[n1] != NUL; n1 += (*mb_ptr2len)(buffer + n1))
	    {
		n2 += (*mb_ptr2cells)(buffer + n1);
		གལ་ཏེ (this_ru_col + n2 > width)
		{
		    bufferlen = n1;
		    buffer[bufferlen] = NUL;
		    bha_nga;
		}
	    }
	}
	དེ་མིན གལ་ཏེ (this_ru_col + bufferlen > width)
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
	wp->w_ru_empty = empty_line;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_line_count = wp->w_buffer->b_ml.ml_line_count;
#ifdef FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#endif

	གལ་ཏེ (override_success)
	    pop_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    void
after_updating_screen(ཧྲིལ may_resize_shell UNUSED)
{
    updating_screen = FALSE;
#ifdef FEAT_GUI
    གལ་ཏེ (may_resize_shell)
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
    void
update_curbuf(ཧྲིལ type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#གལ་ཏེ defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    static ཧྲིལ
text_to_screenline(win_T *wp, char_u *text, ཧྲིལ col)
{
    ཧྲིལ		off = (ཧྲིལ)(current_ScreenLine - ScreenLines);

    གལ་ཏེ (has_mbyte)
    {
	ཧྲིལ	cells;
	ཧྲིལ	u8c, u8cc[MAX_MCO];
	ཧྲིལ	i;
	ཧྲིལ	idx;
	ཧྲིལ	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	ཧྲིལ	prev_c = 0;		// previous Arabic character
	ཧྲིལ	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	གལ་ཏེ (wp->w_p_rl)
	    idx = off;
	དེ་མིན
# endif
	    idx = off + col;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	ཆེད་དུ (p = text; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    གལ་ཏེ (col + cells > wp->w_width)
		bha_nga;
# ifdef FEAT_RIGHTLEFT
	    གལ་ཏེ (wp->w_p_rl)
		idx = off + wp->w_width - col - cells;
# endif
	    ScreenLines[idx] = *p;
	    གལ་ཏེ (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		གལ་ཏེ (*p < 0x80 && u8cc[0] == 0)
		{
		    ScreenLinesUC[idx] = 0;
# ifdef FEAT_ARABIC
		    prev_c = u8c;
# endif
		}
		དེ་མིན
		{
# ifdef FEAT_ARABIC
		    གལ་ཏེ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			ཧྲིལ	pc, pc1, nc;
			ཧྲིལ	pcc[MAX_MCO];
			ཧྲིལ	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			གལ་ཏེ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			དེ་མིན
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			ScreenLines[idx] = firstbyte;
		    }
		    དེ་མིན
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    ScreenLinesUC[idx] = u8c;
		    ཆེད་དུ (i = 0; i < Screen_mco; ++i)
		    {
			ScreenLinesC[i][idx] = u8cc[i];
			གལ་ཏེ (u8cc[i] == 0)
			    bha_nga;
		    }
		}
		གལ་ཏེ (cells > 1)
		    ScreenLines[idx + 1] = 0;
	    }
	    དེ་མིན གལ་ཏེ (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    དེ་མིན གལ་ཏེ (cells > 1)
		// double-width character
		ScreenLines[idx + 1] = p[1];
	    col += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    དེ་མིན
    {
	ཧྲིལ len = (ཧྲིལ)STRLEN(text);
	ཧྲིལ n = wp->w_width - col;

	གལ་ཏེ (len > n)
	    len = n;
	གལ་ཏེ (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    གལ་ཏེ (wp->w_p_rl)
		mch_memmove(current_ScreenLine, text, len);
	    དེ་མིན
# endif
		mch_memmove(current_ScreenLine + col, text, len);
	    col += len;
	}
    }
    ཕྱིར col;
}
#endif

#ifdef FEAT_MENU
/*
 * Draw the window toolbar.
 */
    static void
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    ཧྲིལ		item_idx = 0;
    ཧྲིལ		item_count = 0;
    ཧྲིལ		col = 0;
    ཧྲིལ		next_col;
    ཧྲིལ		off = (ཧྲིལ)(current_ScreenLine - ScreenLines);
    bool	override_success =
	push_highlight_overrides(wp->w_hl, wp->w_hl_len);
    ཧྲིལ		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    ཧྲིལ		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_items);
    FOR_ALL_CHILD_MENUS(wp->w_winbar, menu)
	++item_count;
    wp->w_winbar_items = ALLOC_CLEAR_MULT(winbar_item_T, item_count + 1);

    // TODO: use fewer spaces if there is not enough room
    ཆེད་དུ (menu = wp->w_winbar->children;
			  menu != NULL && col < wp->w_width; menu = menu->next)
    {
	space_to_screenline(off + col, fill_attr);
	གལ་ཏེ (++col >= wp->w_width)
	    bha_nga;
	གལ་ཏེ (col > 1)
	{
	    space_to_screenline(off + col, fill_attr);
	    གལ་ཏེ (++col >= wp->w_width)
		bha_nga;
	}

	wp->w_winbar_items[item_idx].wb_startcol = col;
	space_to_screenline(off + col, button_attr);
	གལ་ཏེ (++col >= wp->w_width)
	    bha_nga;

	next_col = text_to_screenline(wp, menu->name, col);
	སྐབས (col < next_col)
	{
	    ScreenAttrs[off + col] = button_attr;
	    ++col;
	}
	wp->w_winbar_items[item_idx].wb_endcol = col;
	wp->w_winbar_items[item_idx].wb_menu = menu;
	++item_idx;

	གལ་ཏེ (col >= wp->w_width)
	    bha_nga;
	space_to_screenline(off + col, button_attr);
	++col;
    }
    སྐབས (col < wp->w_width)
    {
	space_to_screenline(off + col, fill_attr);
	++col;
    }
    wp->w_winbar_items[item_idx].wb_menu = NULL; // end marker

    screen_line(wp, wp->w_winrow, wp->w_wincol, wp->w_width,
							  wp->w_width, -1, 0);

    གལ་ཏེ (override_success)
	pop_highlight_overrides();
}
#endif

#གལ་ཏེ defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    static void
copy_text_attr(
    ཧྲིལ		off,
    char_u	*buf,
    ཧྲིལ		len,
    ཧྲིལ		attr)
{
    ཧྲིལ		i;

    mch_memmove(ScreenLines + off, buf, (size_t)len);
    གལ་ཏེ (enc_utf8)
	vim_memset(ScreenLinesUC + off, 0, parimaa_na(u8char_T) * (size_t)len);
    ཆེད་དུ (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    static void
fold_line(
    win_T	*wp,
    long	fold_count,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    ཧྲིལ		row)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*top, *bot;
    linenr_T	lnume = lnum + fold_count - 1;
    ཧྲིལ		len;
    char_u	*text;
    ཧྲིལ		fdc;
    ཧྲིལ		col;
    ཧྲིལ		txtcol;
    ཧྲིལ		off = (ཧྲིལ)(current_ScreenLine - ScreenLines);
    ཧྲིལ		ri;

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
    གལ་ཏེ (wp == cmdwin_win)
    {
	ScreenLines[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	གལ་ཏེ (enc_utf8)
	    ScreenLinesUC[off] = 0;
	++col;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    do { \
	གལ་ཏེ (wp->w_p_rl) \
	    ཆེད་དུ (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_width - (p) - (l)) + ri] = v; \
	 དེ་མིན \
	    ཆེད་དུ (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } སྐབས (0)
# དེ་མིན
#  define RL_MEMSET(p, v, l) \
    do { \
	ཆེད་དུ (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } སྐབས (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, col);
    གལ་ཏེ (fdc > 0)
    {
	char_u	*p;
	ཧྲིལ	i;
	ཧྲིལ	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	ཆེད་དུ (i = 0; i < fdc; i++)
	{
	    ཧྲིལ		ch;

	    གལ་ཏེ (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    དེ་མིན
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    གལ་ཏེ (wp->w_p_rl)
		idx = off + wp->w_width - i - 1 - col;
	    དེ་མིན
# endif
		idx = off + col + i;
	    གལ་ཏེ (enc_utf8)
	    {
		གལ་ཏེ (ch >= 0x80)
		{
		    ScreenLinesUC[idx] = ch;
		    ScreenLinesC[0][idx] = 0;
		    ScreenLines[idx] = 0x80;
		}
		དེ་མིན
		{
		    ScreenLines[idx] = ch;
		    ScreenLinesUC[idx] = 0;
		}
	    }
	    དེ་མིན
		ScreenLines[idx] = ch;
	}

	RL_MEMSET(col, HL_ATTR(HLF_FC), fdc);
	col += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    // text
    RL_MEMSET(col, HL_ATTR(HLF_FL), wp->w_width - col);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    གལ་ཏེ (signcolumn_on(wp))
    {
	len = wp->w_width - col;
	གལ་ཏེ (len > 0)
	{
	    གལ་ཏེ (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    གལ་ཏེ (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    དེ་མིན
#  endif
		copy_text_attr(off + col, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    col += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    གལ་ཏེ (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_width - col;
	གལ་ཏེ (len > 0)
	{
	    ཧྲིལ	    w = number_width(wp);
	    long    num;
	    var_na    *fmt = "%*ld ";

	    གལ་ཏེ (len > w + 1)
		len = w + 1;

	    གལ་ཏེ (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		num = (long)lnum;
	    དེ་མིན
	    {
		// 'relativenumber', don't use negative numbers
		num = labs((long)get_cursor_rel_lnum(wp, lnum));
		གལ་ཏེ (num == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    num = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((var_na *)buf, parimaa_na(buf), fmt, w, num);
# ifdef FEAT_RIGHTLEFT
	    གལ་ཏེ (wp->w_p_rl)
		// the line number isn't reversed
		copy_text_attr(off + wp->w_width - len - col, buf, len,
							     HL_ATTR(HLF_FL));
	    དེ་མིན
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
    སྐབས (col < wp->w_width)
    {
	ཧྲིལ c = wp->w_fill_chars.fold;
	ཧྲིལ idx = off + col;

# ifdef FEAT_RIGHTLEFT
	གལ་ཏེ (wp->w_p_rl)
	    idx = off + wp->w_width - 1 - col;
# endif

	གལ་ཏེ (enc_utf8)
	{
	    གལ་ཏེ (c >= 0x80)
	    {
		ScreenLinesUC[idx] = c;
		ScreenLinesC[0][idx] = 0;
		ScreenLines[idx] = 0x80; // avoid storing zero
	    }
	    དེ་མིན
	    {
		ScreenLinesUC[idx] = 0;
		ScreenLines[idx] = c;
	    }
	}
	དེ་མིན
	    ScreenLines[idx] = c;
	++col;
    }

    གལ་ཏེ (text != buf)
	vim_free(text);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    གལ་ཏེ (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
	གལ་ཏེ (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    top = &curwin->w_cursor;
	    bot = &VIsual;
	}
	དེ་མིན
	{
	    // Visual is before curwin->w_cursor
	    top = &VIsual;
	    bot = &curwin->w_cursor;
	}
	གལ་ཏེ (lnum >= top->lnum
		&& lnume <= bot->lnum
		&& (VIsual_mode != 'v'
		    || ((lnum > top->lnum
			    || (lnum == top->lnum
				&& top->col == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->col - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_buffer, lnume))))))
	{
	    གལ་ཏེ (VIsual_mode == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		གལ་ཏེ (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_width)
		{
		    གལ་ཏེ (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_width)
			len = wp->w_old_cursor_lcol;
		    དེ་མིན
			len = wp->w_width - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (ཧྲིལ)wp->w_old_cursor_fcol);
		}
	    }
	    དེ་མིན
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_width - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    གལ་ཏེ (wp->w_p_cc_cols)
    {
	ཧྲིལ i = 0;
	ཧྲིལ j = wp->w_p_cc_cols[i];
	ཧྲིལ old_txtcol = txtcol;

	སྐབས (j > -1)
	{
	    txtcol += j;
	    གལ་ཏེ (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    དེ་མིན
		txtcol -= wp->w_leftcol;
	    གལ་ཏེ (txtcol >= 0 && txtcol < wp->w_width)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_cols[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    གལ་ཏེ (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	གལ་ཏེ (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	དེ་མིན
	    txtcol -= wp->w_leftcol;
	གལ་ཏེ (txtcol >= 0 && txtcol < wp->w_width)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_line(wp, row + W_WINROW(wp), wp->w_wincol, wp->w_width, wp->w_width,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    གལ་ཏེ (wp == curwin
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
    གལ་ཏེ (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_line(wp))
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
    static void
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_buffer;
    ཧྲིལ		type;
    ཧྲིལ		top_end = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    ཧྲིལ		mid_start = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    ཧྲིལ		mid_end = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    ཧྲིལ		bot_start = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    ཧྲིལ		scrolled_down = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    ཧྲིལ		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    // lines
#ifdef FEAT_SEARCH_EXTRA
    ཧྲིལ		top_to_mod = FALSE;    // redraw above mod_top
#endif

    ཧྲིལ		row;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    ཧྲིལ		idx;		// current index in w_lines[]
    ཧྲིལ		srow;		// starting row of the current line

    ཧྲིལ		eof = FALSE;	// if TRUE, we hit the end of the file
    ཧྲིལ		didline = FALSE; // if TRUE, we finished the last line
    ཧྲིལ		i;
    long	j;
    static ཧྲིལ	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#ifdef FEAT_CONCEAL
    ཧྲིལ		old_wrow = wp->w_wrow;
    ཧྲིལ		old_wcol = wp->w_wcol;
#endif
#ifdef FEAT_FOLDING
    long	fold_count;
#endif
#ifdef FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_LINE 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    ཧྲིལ		did_update = DID_NONE;
    linenr_T	syntax_last_parsed = 0;		// last parsed text line
#endif
    linenr_T	mod_top = 0;
    linenr_T	mod_bot = 0;
#གལ་ཏེ defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    ཧྲིལ		save_got_int;
#endif
    bool	override_success;

#གལ་ཏེ defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    གལ་ཏེ (!did_update_one_window)
    {
	did_update_one_window = TRUE;
# ifdef FEAT_SEARCH_EXTRA
	start_search_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	གལ་ཏེ (clip_star.available && clip_isautosel_star())
	    clip_update_selection(&clip_star);
	གལ་ཏེ (clip_plus.available && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#endif

#ifdef FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    གལ་ཏེ (wp->w_p_cuc)
	validate_virtcol_win(wp);
#endif

    type = wp->w_redr_type;

    གལ་ཏེ (type == UPD_NOT_VALID)
    {
	wp->w_redr_status = true;
	wp->w_lines_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    གལ་ཏེ (wp->w_height + WINBAR_HEIGHT(wp) == 0
	    || (wp->w_frame->fr_height == wp->w_status_height
#གལ་ཏེ defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#endif
	       ))
    {
	wp->w_redr_type = 0;
	ཕྱིར;
    }

    // Window is zero-width: Only need to draw the separator.
    གལ་ཏེ (wp->w_width == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	ཕྱིར;
    }

    override_success = push_highlight_overrides(wp->w_hl, wp->w_hl_len);


#ifdef FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    གལ་ཏེ (term_do_update_window(wp))
    {
	term_update_window(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	གལ་ཏེ (winbar_height(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	གལ་ཏེ (override_success)
	    pop_highlight_overrides();
	ཕྱིར;
    }
#endif

#ifdef FEAT_SEARCH_EXTRA
    init_search_hl(wp, &screen_search_hl);
#endif

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    གལ་ཏེ (wp->w_skipcol > 0 && wp->w_width > win_col_off(wp))
    {
	ཧྲིལ w = 0;
	ཧྲིལ width1 = wp->w_width - win_col_off(wp);
	ཧྲིལ width2 = width1 + win_col_off2(wp);
	ཧྲིལ add = width1;

	སྐབས (w < wp->w_skipcol)
	{
	    གལ་ཏེ (w > 0)
		add = width2;
	    w += add;
	}
	གལ་ཏེ (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - add;
    }

#ifdef FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? number_width(wp) : 0;
    གལ་ཏེ (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    དེ་མིན
#endif

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_top = wp->w_redraw_top;
	གལ་ཏེ (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	དེ་མིན
	    mod_bot = 0;
	གལ་ཏེ (buf->b_mod_set)
	{
	    གལ་ཏེ (mod_top == 0 || mod_top > buf->b_mod_top)
	    {
		mod_top = buf->b_mod_top;
#ifdef FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		གལ་ཏེ (syntax_present(wp))
		{
		    mod_top -= buf->b_s.b_syn_sync_linebreaks;
		    གལ་ཏེ (mod_top < 1)
			mod_top = 1;
		}
#endif
	    }
	    གལ་ཏེ (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ifdef FEAT_SEARCH_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    གལ་ཏེ (screen_search_hl.rm.regprog != NULL
		    && re_multiline(screen_search_hl.rm.regprog))
		top_to_mod = TRUE;
	    དེ་མིན
	    {
		matchitem_T *cur = wp->w_match_head;

		སྐབས (cur != NULL)
		{
		    གལ་ཏེ (cur->mit_match.regprog != NULL
				       && re_multiline(cur->mit_match.regprog))
		    {
			top_to_mod = TRUE;
			bha_nga;
		    }
		    cur = cur->mit_next;
		}
	    }
#endif
	}

#ifdef FEAT_SEARCH_EXTRA
	གལ་ཏེ (search_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    གལ་ཏེ (mod_top == 0 || mod_top > search_hl_has_cursor_lnum)
		mod_top = search_hl_has_cursor_lnum;
	    གལ་ཏེ (mod_bot == 0 || mod_bot < search_hl_has_cursor_lnum + 1)
		mod_bot = search_hl_has_cursor_lnum + 1;
	}
#endif

#ifdef FEAT_FOLDING
	གལ་ཏེ (mod_top != 0 && hasAnyFolding(wp))
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
	    ཆེད་དུ (i = 0; i < wp->w_lines_valid; ++i)
		གལ་ཏེ (wp->w_lines[i].wl_valid)
		{
		    གལ་ཏེ (wp->w_lines[i].wl_lastlnum < mod_top)
			lnumt = wp->w_lines[i].wl_lastlnum + 1;
		    གལ་ཏེ (lnumb == MAXLNUM && wp->w_lines[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_lines[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			གལ་ཏེ (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (void)hasFoldingWin(wp, mod_top, &mod_top, NULL, TRUE, NULL);
	    གལ་ཏེ (mod_top > lnumt)
		mod_top = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (void)hasFoldingWin(wp, mod_bot, NULL, &mod_bot, TRUE, NULL);
	    ++mod_bot;
	    གལ་ཏེ (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#endif

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	གལ་ཏེ (mod_top != 0 && mod_top < wp->w_topline)
	{
	    གལ་ཏེ (mod_bot > wp->w_topline)
		mod_top = wp->w_topline;
#ifdef FEAT_SYN_HL
	    དེ་མིན གལ་ཏེ (syntax_present(wp))
		top_end = 1;
#endif
	}
    }
    wp->w_redraw_top = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#ifdef FEAT_SEARCH_EXTRA
    search_hl_has_cursor_lnum = 0;
#endif


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    གལ་ཏེ (type == UPD_REDRAW_TOP)
    {
	j = 0;
	ཆེད་དུ (i = 0; i < wp->w_lines_valid; ++i)
	{
	    j += wp->w_lines[i].wl_size;
	    གལ་ཏེ (j >= wp->w_upd_rows)
	    {
		top_end = j;
		bha_nga;
	    }
	}
	གལ་ཏེ (top_end == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	དེ་མིན
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    གལ་ཏེ (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    གལ་ཏེ ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#ifdef FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#endif
	    )
    {
	གལ་ཏེ (mod_top != 0
		&& wp->w_topline == mod_top
		&& (!wp->w_lines[0].wl_valid
		    || wp->w_topline == wp->w_lines[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	དེ་མིན གལ་ཏེ (wp->w_lines[0].wl_valid
		&& (wp->w_topline < wp->w_lines[0].wl_lnum
#ifdef FEAT_DIFF
		    || (wp->w_topline == wp->w_lines[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#endif
		   ))
	{
	    // New topline is above old topline: May scroll down.
#ifdef FEAT_FOLDING
	    གལ་ཏེ (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		ཆེད་དུ (ln = wp->w_topline; ln < wp->w_lines[0].wl_lnum; ++ln)
		{
		    ++j;
		    གལ་ཏེ (j >= wp->w_height - 2)
			bha_nga;
		    (void)hasFoldingWin(wp, ln, NULL, &ln, TRUE, NULL);
		}
	    }
	    དེ་མིན
#endif
		j = wp->w_lines[0].wl_lnum - wp->w_topline;
	    གལ་ཏེ (j < wp->w_height - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_lines[0].wl_lnum - 1,
								wp->w_height);
#ifdef FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		གལ་ཏེ (wp->w_lines[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_lines[0].wl_lnum)
							  - wp->w_old_topfill;
#endif
		གལ་ཏེ (i < wp->w_height - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    གལ་ཏེ (i > 0)
			check_for_delay(FALSE);
		    གལ་ཏེ (win_ins_lines(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			གལ་ཏེ (wp->w_lines_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    top_end = i;
			    scrolled_down = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    གལ་ཏེ ((wp->w_lines_valid += j) > wp->w_height)
				wp->w_lines_valid = wp->w_height;
			    ཆེད་དུ (idx = wp->w_lines_valid; idx - j >= 0; idx--)
				wp->w_lines[idx] = wp->w_lines[idx - j];
			    སྐབས (idx >= 0)
				wp->w_lines[idx--].wl_valid = FALSE;
			}
		    }
		    དེ་མིན
			mid_start = 0;		// redraw all lines
		}
		དེ་མིན
		    mid_start = 0;		// redraw all lines
	    }
	    དེ་མིན
		mid_start = 0;		// redraw all lines
	}
	དེ་མིན
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    row = 0;
	    ཆེད་དུ (i = 0; i < wp->w_lines_valid && i < Rows; i++)
	    {
		གལ་ཏེ (wp->w_lines[i].wl_valid
			&& wp->w_lines[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    bha_nga;
		}
		row += wp->w_lines[i].wl_size;
	    }
	    གལ་ཏེ (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		// lines
		mid_start = 0;
	    }
	    དེ་མིན
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#ifdef FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		གལ་ཏེ (wp->w_lines[0].wl_lnum == wp->w_topline)
		    row += wp->w_old_topfill;
		དེ་མིན
		    row += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		row -= wp->w_topfill;
#endif
		གལ་ཏེ (row > Rows)  // just in case
		    row = Rows;
		གལ་ཏེ (row > 0)
		{
		    check_for_delay(FALSE);
		    གལ་ཏེ (win_del_lines(wp, 0, row, FALSE, wp == firstwin, 0)
									 == OK)
			bot_start = wp->w_height - row;
		    དེ་མིན
			mid_start = 0;		// redraw all lines
		}
		གལ་ཏེ ((row == 0 || bot_start < 999) && wp->w_lines_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_start = 0;
		    idx = 0;
		    ཆེད་དུ (;;)
		    {
			wp->w_lines[idx] = wp->w_lines[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			གལ་ཏེ (row > 0 && bot_start + row
				 + (ཧྲིལ)wp->w_lines[j].wl_size > wp->w_height)
			{
			    wp->w_lines_valid = idx + 1;
			    bha_nga;
			}
			bot_start += wp->w_lines[idx++].wl_size;

			// stop at the last valid entry in w_lines[].wl_size
			གལ་ཏེ (++j >= wp->w_lines_valid)
			{
			    wp->w_lines_valid = idx;
			    bha_nga;
			}
		    }
#ifdef FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    གལ་ཏེ (wp->w_p_diff && bot_start > 0)
			wp->w_lines[0].wl_size = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#endif
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	གལ་ཏེ (mid_start == 0)
	    mid_end = wp->w_height;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	གལ་ཏེ (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    དེ་མིན
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
    }

    གལ་ཏེ (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_start = 0;
	mid_end = wp->w_height;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    གལ་ཏེ ((VIsual_active && buf == curwin->w_buffer)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, to;

	གལ་ཏེ (VIsual_active)
	{
	    གལ་ཏེ (VIsual_mode != wp->w_old_visual_mode
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		གལ་ཏེ (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = VIsual.lnum;
		}
		དེ་མིན
		{
		    from = VIsual.lnum;
		    to = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		གལ་ཏེ (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		གལ་ཏེ (wp->w_old_cursor_lnum > to)
		    to = wp->w_old_cursor_lnum;
		གལ་ཏེ (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		གལ་ཏེ (wp->w_old_visual_lnum > to)
		    to = wp->w_old_visual_lnum;
	    }
	    དེ་མིན
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		གལ་ཏེ (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = wp->w_old_cursor_lnum;
		}
		དེ་མིན
		{
		    from = wp->w_old_cursor_lnum;
		    to = curwin->w_cursor.lnum;
		    གལ་ཏེ (from == 0)	// Visual mode just started
			from = to;
		}

		གལ་ཏེ (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.col != wp->w_old_visual_col)
		{
		    གལ་ཏེ (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    གལ་ཏེ (wp->w_old_visual_lnum > to)
			to = wp->w_old_visual_lnum;
		    གལ་ཏེ (VIsual.lnum < from)
			from = VIsual.lnum;
		    གལ་ཏེ (VIsual.lnum > to)
			to = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    གལ་ཏེ (VIsual_mode == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_END_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		གལ་ཏེ (curwin->w_curswant == MAXCOL)
		{
		    གལ་ཏེ (get_ve_flags() & VE_BLOCK)
		    {
			pos_T	    pos;
			ཧྲིལ	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			ཆེད་དུ (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.col = (ཧྲིལ)ml_get_buf_len(wp->w_buffer, pos.lnum);
			    getvvcol(wp, &pos, NULL, NULL, &t, 0);
			    གལ་ཏེ (toc < t)
				toc = t;
			}
			++toc;
		    }
		    དེ་མིན
			toc = MAXCOL;
		}

		གལ་ཏེ (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    གལ་ཏེ (from > VIsual.lnum)
			from = VIsual.lnum;
		    གལ་ཏེ (to < VIsual.lnum)
			to = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	དེ་མིན
	{
	    // Use the line numbers of the old Visual area.
	    གལ་ཏེ (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		to = wp->w_old_visual_lnum;
	    }
	    དེ་མིན
	    {
		from = wp->w_old_visual_lnum;
		to = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	གལ་ཏེ (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	གལ་ཏེ (wp->w_valid & VALID_BOTLINE)
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
	གལ་ཏེ (mid_start > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    གལ་ཏེ (scrolled_down)
		mid_start = top_end;
	    དེ་མིན
		mid_start = 0;
	    སྐབས (lnum < from && idx < wp->w_lines_valid)	// find start
	    {
		གལ་ཏེ (wp->w_lines[idx].wl_valid)
		    mid_start += wp->w_lines[idx].wl_size;
		དེ་མིན གལ་ཏེ (!scrolled_down)
		    srow += wp->w_lines[idx].wl_size;
		++idx;
#ifdef FEAT_FOLDING
		གལ་ཏེ (idx < wp->w_lines_valid && wp->w_lines[idx].wl_valid)
		    lnum = wp->w_lines[idx].wl_lnum;
		དེ་མིན
#endif
		    ++lnum;
	    }
	    srow += mid_start;
	    mid_end = wp->w_height;
	    ཆེད་དུ ( ; idx < wp->w_lines_valid; ++idx)		// find end
	    {
		གལ་ཏེ (wp->w_lines[idx].wl_valid
			&& wp->w_lines[idx].wl_lnum >= to + 1)
		{
		    // Only update until first row of this line
		    mid_end = srow;
		    bha_nga;
		}
		srow += wp->w_lines[idx].wl_size;
	    }
	}
    }

    གལ་ཏེ (VIsual_active && buf == curwin->w_buffer)
    {
	wp->w_old_visual_mode = VIsual_mode;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_col = VIsual.col;
	wp->w_old_curswant = curwin->w_curswant;
    }
    དེ་མིན
    {
	wp->w_old_visual_mode = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_col = 0;
    }

#གལ་ཏེ defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
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
    གལ་ཏེ (winbar_height(wp) > 0)
	redraw_win_toolbar(wp);
#endif

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#ifdef FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_FIELD(spv);
    གལ་ཏེ (spell_check_window(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_top == 0;
    }
#endif

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    row = 0;
    srow = 0;
    ཆེད་དུ (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	གལ་ཏེ (row == wp->w_height)
	{
	    didline = TRUE;
	    bha_nga;
	}

	// stop updating when hit the end of the file
	གལ་ཏེ (lnum > buf->b_ml.ml_line_count)
	{
	    eof = TRUE;
	    bha_nga;
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
	གལ་ཏེ (row < top_end
		|| (row >= mid_start && row < mid_end)
#ifdef FEAT_SEARCH_EXTRA
		|| top_to_mod
#endif
		|| idx >= wp->w_lines_valid
		|| (row + wp->w_lines[idx].wl_size > bot_start)
		|| (mod_top != 0
		    && (lnum == mod_top
			|| (lnum >= mod_top
			    && (lnum < mod_bot
#ifdef FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_LINE
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_changed(lnum)))
#endif
#ifdef FEAT_SEARCH_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_match_head != NULL
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
#ifdef FEAT_SEARCH_EXTRA
	    གལ་ཏེ (lnum == mod_top)
		top_to_mod = FALSE;
#endif

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    གལ་ཏེ (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_top && lnum < MAX(mod_bot, mod_top + 1)
		    && (!scrolled_down || row >= top_end))
	    {
		scrolled_for_mod = TRUE;

		ཧྲིལ		old_cline_height = 0;
		ཧྲིལ		old_rows = 0;
		ཧྲིལ		new_rows = 0;
		ཧྲིལ		xtra_rows;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		ཆེད་དུ (i = idx; i < wp->w_lines_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    གལ་ཏེ (wp->w_lines[i].wl_valid
			    && wp->w_lines[i].wl_lnum == mod_bot)
			bha_nga;
		    གལ་ཏེ (wp->w_lines[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_height = wp->w_lines[i].wl_size;
		    old_rows += wp->w_lines[i].wl_size;
#ifdef FEAT_FOLDING
		    གལ་ཏེ (wp->w_lines[i].wl_valid
			    && wp->w_lines[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			སྐབས (i < wp->w_lines_valid
						  && !wp->w_lines[i].wl_valid)
			    old_rows += wp->w_lines[i++].wl_size;
			bha_nga;
		    }
#endif
		}

		གལ་ཏེ (i >= wp->w_lines_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_start = 0;
		}
		དེ་མིན
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    ཆེད་དུ (l = lnum; l < mod_bot; ++l)
		    {
			གལ་ཏེ (dollar_vcol >= 0 && wp == curwin &&
				old_cline_height > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_rows += old_cline_height;
#ifdef FEAT_FOLDING
			དེ་མིན གལ་ཏེ (hasFoldingWin(wp, l, NULL, &l, TRUE, NULL))
			    ++new_rows;
#endif
			དེ་མིན
			    new_rows += plines_correct_topline(wp, l, TRUE);
			++j;
			གལ་ཏེ (new_rows > wp->w_height - row - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_rows = 9999;
			    bha_nga;
			}
		    }
		    xtra_rows = new_rows - old_rows;
		    གལ་ཏེ (xtra_rows < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			གལ་ཏེ (row - xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			དེ་མིན
			{
			    check_for_delay(FALSE);
			    གལ་ཏེ (win_del_lines(wp, row,
					  -xtra_rows, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    དེ་མིན
				bot_start = wp->w_height + xtra_rows;
			}
		    }
		    དེ་མིན གལ་ཏེ (xtra_rows > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			གལ་ཏེ (row + xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			དེ་མིན
			{
			    check_for_delay(FALSE);
			    གལ་ཏེ (win_ins_lines(wp, row + old_rows,
					     xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    དེ་མིན གལ་ཏེ (top_end > row + old_rows)
				// Scrolled the part at the top that requires
				// updating down.
				top_end += xtra_rows;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    གལ་ཏེ (mod_bot != MAXLNUM && i != j)
		    {
			གལ་ཏེ (j < i)
			{
			    ཧྲིལ x = row + new_rows;

			    // move entries in w_lines[] upwards
			    ཆེད་དུ (;;)
			    {
				// stop at last valid entry in w_lines[]
				གལ་ཏེ (i >= wp->w_lines_valid)
				{
				    wp->w_lines_valid = j;
				    bha_nga;
				}
				wp->w_lines[j] = wp->w_lines[i];
				// stop at a line that won't fit
				གལ་ཏེ (x + (ཧྲིལ)wp->w_lines[j].wl_size
							   > wp->w_height)
				{
				    wp->w_lines_valid = j + 1;
				    bha_nga;
				}
				x += wp->w_lines[j++].wl_size;
				++i;
			    }
			    གལ་ཏེ (bot_start > x)
				bot_start = x;
			}
			དེ་མིན // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_lines_valid += j;
			    གལ་ཏེ (wp->w_lines_valid > wp->w_height)
				wp->w_lines_valid = wp->w_height;
			    ཆེད་དུ (i = wp->w_lines_valid; i - j >= idx; --i)
				wp->w_lines[i] = wp->w_lines[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    སྐབས (i >= idx)
			    {
				wp->w_lines[i].wl_size = 0;
				wp->w_lines[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ifdef FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_count = foldedCount(wp, lnum, &win_foldinfo);
	    གལ་ཏེ (fold_count != 0)
	    {
		fold_line(wp, fold_count, &win_foldinfo, lnum, row);
		++row;
		--fold_count;
		wp->w_lines[idx].wl_folded = TRUE;
		wp->w_lines[idx].wl_lastlnum = lnum + fold_count;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
# ifdef FEAT_SPELL
		spv.spv_capcol_lnum = 0;
# endif
	    }
	    དེ་མིན
#endif
	    གལ་ཏེ (idx < wp->w_lines_valid
		    && wp->w_lines[idx].wl_valid
		    && wp->w_lines[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_lines[idx].wl_size > wp->w_height
#ifdef FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#endif
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		row = wp->w_height + 1;
	    }
	    དེ་མིན
	    {
#ifdef FEAT_SEARCH_EXTRA
		prepare_search_hl(wp, &screen_search_hl, lnum);
#endif
#ifdef FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		གལ་ཏེ (syntax_last_parsed != 0 && syntax_last_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

		// Display one line.
		row = win_line(wp, lnum, srow, wp->w_height, 0, &spv);

#ifdef FEAT_FOLDING
		wp->w_lines[idx].wl_folded = FALSE;
		wp->w_lines[idx].wl_lastlnum = lnum;
#endif
#ifdef FEAT_SYN_HL
		did_update = DID_LINE;
		syntax_last_parsed = lnum;
#endif
	    }

	    wp->w_lines[idx].wl_lnum = lnum;
	    wp->w_lines[idx].wl_valid = TRUE;

	    ཧྲིལ is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    གལ་ཏེ (row > wp->w_height || row + wp->w_winrow >= Rows)
	    {
		// we may need the size of that too long line later on
		གལ་ཏེ (dollar_vcol == -1 || !is_curline)
		    wp->w_lines[idx].wl_size = plines_win(wp, lnum, TRUE);
		++idx;
		bha_nga;
	    }
	    གལ་ཏེ (dollar_vcol == -1 || !is_curline)
		wp->w_lines[idx].wl_size = row - srow;
	    ++idx;
#ifdef FEAT_FOLDING
	    lnum += fold_count + 1;
#དེ་མིན
	    ++lnum;
#endif
	}
	དེ་མིན
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    གལ་ཏེ ((wp->w_p_nu && mod_top != 0 && lnum >= mod_bot
			&& buf->b_mod_set && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_last_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#ifdef FEAT_FOLDING
		fold_count = foldedCount(wp, lnum, &win_foldinfo);
		གལ་ཏེ (fold_count != 0)
		    fold_line(wp, fold_count, &win_foldinfo, lnum, row);
		དེ་མིན
#endif
		    (void)win_line(wp, lnum, srow, wp->w_height,
					       wp->w_lines[idx].wl_size, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    row += wp->w_lines[idx++].wl_size;
	    གལ་ཏེ (row > wp->w_height)	// past end of screen
		bha_nga;
#ifdef FEAT_FOLDING
	    lnum = wp->w_lines[idx - 1].wl_lastlnum + 1;
#དེ་མིན
	    ++lnum;
#endif
#ifdef FEAT_SYN_HL
	    did_update = DID_NONE;
#endif
#ifdef FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#endif
	}

	གལ་ཏེ (lnum > buf->b_ml.ml_line_count)
	{
	    eof = TRUE;
	    bha_nga;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	གལ་ཏེ (idx >= Rows)
	    bha_nga;
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
    གལ་ཏེ (use_vtp() && get_conpty_fix_type() < 1)
    {
	ཧྲིལ k;

	ཆེད་དུ (k = 0; k < Rows; ++k)
	    གལ་ཏེ (enc_utf8)
		གལ་ཏེ ((*mb_off2cells)(LineOffset[k] + topframe->fr_width - 2,
					   LineOffset[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_width - 2, 1, 2,
			    FALSE, FALSE);
		དེ་མིན
		    screen_draw_rectangle(k, topframe->fr_width - 1, 1, 1,
			    FALSE, FALSE);
	    དེ་མིན
		screen_char(LineOffset[k] + topframe->fr_width - 1, k,
			cmdline_width - 1);
    }
#endif

    གལ་ཏེ (idx > wp->w_lines_valid)
	wp->w_lines_valid = idx;

#ifdef FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    གལ་ཏེ (syntax_last_parsed != 0 && syntax_present(wp))
	syntax_end_parsing(wp, syntax_last_parsed + 1);
#endif

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_empty_rows = 0;
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    གལ་ཏེ (!eof && !didline)
    {
	གལ་ཏེ (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#ifdef FEAT_DIFF
	དེ་མིན གལ་ཏེ (diff_check_fill(wp, lnum) >= wp->w_height - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_rows = wp->w_height - srow;
	}
#endif
#ifdef FEAT_PROP_POPUP
	དེ་མིན གལ་ཏེ (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#endif
	དེ་མིན གལ་ཏེ (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    ཧྲིལ		scr_row = W_WINROW(wp) + wp->w_height - 1;
	    ཧྲིལ		symbol  = wp->w_fill_chars.lastline;
	    ཧྲིལ		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(symbol, &fillbuf[0]);
	    mb_char2bytes(symbol, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_width > 2 ? 2 : wp->w_width) * charlen,
			    scr_row, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_row, scr_row + 1,
		    (ཧྲིལ)wp->w_wincol + 2, (ཧྲིལ)W_ENDCOL(wp),
		    symbol, ' ', HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	དེ་མིན གལ་ཏེ (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    ཧྲིལ start_col = (ཧྲིལ)W_ENDCOL(wp) - 3;
	    ཧྲིལ symbol    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_height - 1,
		    W_WINROW(wp) + wp->w_height,
		    (start_col < wp->w_wincol ? wp->w_wincol : start_col),
		    (ཧྲིལ)W_ENDCOL(wp),
		    symbol, symbol, HL_ATTR(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	དེ་མིན
	{
	    win_draw_end(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_height, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    དེ་མིན
    {
	draw_vsep_win(wp, row);
	གལ་ཏེ (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_line_count + 1;
#ifdef FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    གལ་ཏེ (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		གལ་ཏེ (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		དེ་མིན
		    i = wp->w_fill_chars.diff;
		གལ་ཏེ (row + j > wp->w_height)
		    j = wp->w_height - row;
		win_draw_end(wp, i, i, TRUE, row, row + (ཧྲིལ)j, HLF_DED);
		row += j;
	    }
#endif
	}
	དེ་མིན གལ་ཏེ (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	གལ་ཏེ (WIN_IS_POPUP(wp))
	    win_draw_end(wp, ' ', ' ', FALSE, row, wp->w_height, HLF_AT);
	དེ་མིན
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

    གལ་ཏེ (dollar_vcol == -1 || wp != curwin)
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
	གལ་ཏེ (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#གལ་ཏེ defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    ཧྲིལ		new_wcol = wp->w_wcol;
#endif
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#གལ་ཏེ defined(FEAT_CONCEAL)
	    གལ་ཏེ (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
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
		གལ་ཏེ (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		དེ་མིན
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#endif
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    གལ་ཏེ (wp->w_redr_type != 0)
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
		གལ་ཏེ (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#གལ་ཏེ defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    གལ་ཏེ (!got_int)
	got_int = save_got_int;
#endif

    གལ་ཏེ (override_success)
	pop_highlight_overrides();
}

#གལ་ཏེ defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    static void
update_prepare(void)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    གལ་ཏེ (gui.in_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_SEARCH_EXTRA
    start_search_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_mask(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    static void
update_finish(void)
{
    གལ་ཏེ (redraw_cmdline || redraw_mode)
	showmode();

# ifdef FEAT_SEARCH_EXTRA
    end_search_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    གལ་ཏེ (gui.in_use)
    {
	out_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#endif

#གལ་ཏེ defined(FEAT_NETBEANS_INTG)
    void
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    ཧྲིལ		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_line_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	གལ་ཏེ (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    གལ་ཏེ (!doit || updating_screen
	    || State == MODE_ASKMORE || State == MODE_HITRETURN
	    || msg_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	ཕྱིར;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	གལ་ཏེ (wp->w_redr_type != 0)
	    win_update(wp);
	གལ་ཏེ (wp->w_redr_status)
	    win_redr_status(wp, FALSE);
    }

# གལ་ཏེ defined(FEAT_TABPANEL)
    གལ་ཏེ (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_finish();
}
#endif

#གལ་ཏེ defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    void
updateWindow(win_T *wp)
{
    // return if already busy updating
    གལ་ཏེ (updating_screen)
	ཕྱིར;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    གལ་ཏེ (clip_star.available && clip_isautosel_star())
	clip_update_selection(&clip_star);
    གལ་ཏེ (clip_plus.available && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    གལ་ཏེ (redraw_tabline)
	draw_tabline();

# གལ་ཏེ defined(FEAT_TABPANEL)
    གལ་ཏེ (redraw_tabpanel)
	draw_tabpanel();
# endif

    གལ་ཏེ (wp->w_redr_status || p_ru
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
    ཧྲིལ
redraw_asap(ཧྲིལ type)
{
    ཧྲིལ		rows;
    ཧྲིལ		cols = screen_Columns;
    ཧྲིལ		r;
    ཧྲིལ		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    ཧྲིལ		i;
    u8char_T	*screenlineUC = NULL;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = NULL;	// copy from ScreenLines2[]

    redraw_later(type);
    གལ་ཏེ (msg_scrolled
	    || (State != MODE_NORMAL && State != MODE_NORMAL_BUSY)
	    || exiting)
	ཕྱིར ret;

    // Allocate space to save the text displayed in the command line area.
    rows = screen_Rows - cmdline_row;
    screenline = LALLOC_MULT(schar_T, rows * cols);
    screenattr = LALLOC_MULT(sattr_T, rows * cols);
    གལ་ཏེ (screenline == NULL || screenattr == NULL)
	ret = 2;
    གལ་ཏེ (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, rows * cols);
	གལ་ཏེ (screenlineUC == NULL)
	    ret = 2;
	ཆེད་དུ (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, rows * cols);
	    གལ་ཏེ (screenlineC[i] == NULL)
		ret = 2;
	}
    }
    གལ་ཏེ (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, rows * cols);
	གལ་ཏེ (screenline2 == NULL)
	    ret = 2;
    }

    གལ་ཏེ (ret != 2)
    {
	// Save the text displayed in the command line area.
	ཆེད་དུ (r = 0; r < rows; ++r)
	{
	    mch_memmove(screenline + r * cols,
			ScreenLines + LineOffset[cmdline_row + r],
			(size_t)cols * parimaa_na(schar_T));
	    mch_memmove(screenattr + r * cols,
			ScreenAttrs + LineOffset[cmdline_row + r],
			(size_t)cols * parimaa_na(sattr_T));
	    གལ་ཏེ (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * cols,
			    ScreenLinesUC + LineOffset[cmdline_row + r],
			    (size_t)cols * parimaa_na(u8char_T));
		ཆེད་དུ (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * cols,
				ScreenLinesC[i] + LineOffset[cmdline_row + r],
				(size_t)cols * parimaa_na(u8char_T));
	    }
	    གལ་ཏེ (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * cols,
			    ScreenLines2 + LineOffset[cmdline_row + r],
			    (size_t)cols * parimaa_na(schar_T));
	}

	update_screen(0);
	ret = 3;

	གལ་ཏེ (must_redraw == 0)
	{
	    ཧྲིལ	off = (ཧྲིལ)(current_ScreenLine - ScreenLines);

	    // Restore the text displayed in the command line area.
	    ཆེད་དུ (r = 0; r < rows; ++r)
	    {
		mch_memmove(current_ScreenLine,
			    screenline + r * cols,
			    (size_t)cols * parimaa_na(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * cols,
			    (size_t)cols * parimaa_na(sattr_T));
		གལ་ཏེ (enc_utf8)
		{
		    mch_memmove(ScreenLinesUC + off,
				screenlineUC + r * cols,
				(size_t)cols * parimaa_na(u8char_T));
		    ཆེད་དུ (i = 0; i < p_mco; ++i)
			mch_memmove(ScreenLinesC[i] + off,
				    screenlineC[i] + r * cols,
				    (size_t)cols * parimaa_na(u8char_T));
		}
		གལ་ཏེ (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * cols,
				(size_t)cols * parimaa_na(schar_T));
		screen_line(curwin, cmdline_row + r, 0, cols, cols, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    གལ་ཏེ (enc_utf8)
    {
	vim_free(screenlineUC);
	ཆེད་དུ (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    གལ་ཏེ (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_message();

    setcursor();

    ཕྱིར ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    void
redraw_after_callback(ཧྲིལ call_update_screen, ཧྲིལ do_message)
{
    ++redrawing_for_callback;

    term_set_sync_output(TERM_SYNC_OUTPUT_ENABLE);

    གལ་ཏེ (State == MODE_HITRETURN || State == MODE_ASKMORE
	    || State == MODE_SETWSIZE || State == MODE_EXTERNCMD
	    || State == MODE_CONFIRM || exmode_active)
    {
	གལ་ཏེ (do_message)
	    repeat_message();
    }
    དེ་མིན གལ་ཏེ (State & MODE_CMDLINE)
    {
	གལ་ཏེ (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	གལ་ཏེ (cmdline_row > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    གལ་ཏེ (msg_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    དེ་མིན གལ་ཏེ (State & (MODE_NORMAL | MODE_INSERT | MODE_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	གལ་ཏེ (msg_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    msg_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    term_set_sync_output(TERM_SYNC_OUTPUT_DISABLE);
#ifdef FEAT_GUI
    གལ་ཏེ (gui.in_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	out_flush_cursor(FALSE, FALSE);
    དེ་མིན
#endif
	out_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    void
redraw_later(ཧྲིལ type)
{
    redraw_win_later(curwin, type);
}

    void
redraw_win_later(
    win_T	*wp,
    ཧྲིལ		type)
{
#ifdef FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    གལ་ཏེ (inside_redraw_on_start_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    དེ་མིན
#endif
    གལ་ཏེ (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	གལ་ཏེ (type >= UPD_NOT_VALID)
	    wp->w_lines_valid = 0;
	གལ་ཏེ (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    void
redraw_later_clear(void)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    void
redraw_all_later(ཧྲིལ type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    set_must_redraw(type);
}

#གལ་ཏེ 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    void
redraw_all_windows_later(ཧྲིལ type)
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
    void
set_must_redraw(ཧྲིལ type)
{
    གལ་ཏེ (!redraw_not_allowed &&
#ifdef FEAT_EVAL
	    !inside_redraw_on_start_cb &&
#endif
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    void
redraw_curbuf_later(ཧྲིལ type)
{
    redraw_buf_later(curbuf, type);
}

    void
redraw_buf_later(buf_T *buf, ཧྲིལ type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	གལ་ཏེ (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    }
#ifdef FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	གལ་ཏེ (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	གལ་ཏེ (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
#endif
#གལ་ཏེ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    གལ་ཏེ (curwin->w_buffer == buf)
	redraw_win_later(curwin, type);
#endif
}

#གལ་ཏེ defined(FEAT_SIGNS)
    void
redraw_buf_line_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	གལ་ཏེ (wp->w_buffer == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#endif

#གལ་ཏེ defined(FEAT_JOB_CHANNEL)
    void
redraw_buf_and_status_later(buf_T *buf, ཧྲིལ type)
{
    win_T	*wp;

    གལ་ཏེ (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	ཕྱིར;
    FOR_ALL_WINDOWS(wp)
    {
	གལ་ཏེ (wp->w_buffer == buf)
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
    static void
ruler_redraw_lastwin(void)
{
    གལ་ཏེ (p_ru && lastwin->w_status_height == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    void
status_redraw_all(void)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	གལ་ཏེ (wp->w_status_height)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    void
status_redraw_curbuf(void)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	གལ་ཏེ (wp->w_status_height != 0 && wp->w_buffer == curbuf)
	{
	    wp->w_redr_status = true;
	    redraw_later(UPD_VALID);
	}
    གལ་ཏེ (lastwin->w_buffer == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    void
redraw_statuslines(void)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	གལ་ཏེ (wp->w_redr_status)
	{
	    bool ret = push_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_status(wp, FALSE);
	    གལ་ཏེ (ret)
		pop_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    གལ་ཏེ (redraw_tabline)
	draw_tabline();

#གལ་ཏེ defined(FEAT_TABPANEL)
    གལ་ཏེ (redraw_tabpanel)
	draw_tabpanel();
#endif
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    void
win_redraw_last_status(frame_T *frp)
{
    གལ་ཏེ (frp->fr_layout == FR_LEAF)
	frp->fr_win->w_redr_status = true;
    དེ་མིན གལ་ཏེ (frp->fr_layout == FR_ROW)
    {
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    win_redraw_last_status(frp);
    }
    དེ་མིན // frp->fr_layout == FR_COL
    {
	frp = frp->fr_child;
	སྐབས (frp->fr_next != NULL)
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
    void
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    void
redraw_win_range_later(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    གལ་ཏེ (last >= wp->w_topline && first < wp->w_botline)
    {
	གལ་ཏེ (wp->w_redraw_top == 0 || wp->w_redraw_top > first)
	    wp->w_redraw_top = first;
	གལ་ཏེ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    void
redraw_win_range_now(
    win_T	*wp,
    linenr_T	first,
    linenr_T	last)
{
    གལ་ཏེ (last >= wp->w_topline && first < wp->w_botline)
    {
	གལ་ཏེ (wp->w_redraw_top == 0 || wp->w_redraw_top > first)
	    wp->w_redraw_top = first;
	གལ་ཏེ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < last)
	    wp->w_redraw_bot = last;
	གལ་ཏེ (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#ifdef FEAT_EVAL
static bool redraw_cb_in_progress = false;

    void
f_redraw_listener_add(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    static ཧྲིལ		id;

    གལ་ཏེ (check_secure())
	ཕྱིར;

    གལ་ཏེ (redraw_cb_in_progress)
    {
	emsg(_(e_cannot_add_redraw_listener_in_listener_callback));
	ཕྱིར;
    }

    གལ་ཏེ (check_for_dict_arg(argvars, 0) == FAIL)
	ཕྱིར;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    གལ་ཏེ (rln == NULL)
	ཕྱིར;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    གལ་ཏེ (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	གལ་ཏེ (cb.cb_name == NULL)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    ཕྱིར;
	}
	copy_callback(&rln->rl_callbacks.on_start, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    གལ་ཏེ (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	གལ་ཏེ (cb.cb_name == NULL)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_start);
	    vim_free(rln);
	    ཕྱིར;
	}
	copy_callback(&rln->rl_callbacks.on_end, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    གལ་ཏེ (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	ཕྱིར;
    }

    rln->rl_next = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_NUMBER;
    rettv->vval.v_number = id;

    ཕྱིར;
}

    static void
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_start);
    free_callback(&rln->rl_callbacks.on_end);

    vim_free(rln);
}


    static void
redraw_listener_cleanup(void)
{
    ཆེད་དུ (redraw_listener_T *rln = redraw_listeners; rln != NULL;)
    {
	redraw_listener_T *next = rln->rl_next;
	གལ་ཏེ (rln->rl_id == 0)
	{
	    གལ་ཏེ (redraw_listeners == rln)
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
    static redraw_listener_T *
get_redraw_listener(ཧྲིལ id)
{
    ཆེད་དུ (redraw_listener_T *rln = redraw_listeners; rln != NULL; rln = rln->rl_next)
	གལ་ཏེ (rln->rl_id == id)
	    ཕྱིར rln;
    ཕྱིར NULL;
}

    void
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv UNUSED)
{
    ཧྲིལ			id;
    redraw_listener_T	*rln;

    གལ་ཏེ (check_for_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    id = argvars[0].vval.v_number;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_NUMBER;
    གལ་ཏེ (rln == NULL)
    {
	rettv->vval.v_number = 0;
	ཕྱིར;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_number = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    static void
invoke_redraw_listener_start_or_end(bool start)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    གལ་ཏེ (start)
	inside_redraw_on_start_cb = true;

    redraw_cb_in_progress = true;
    ཆེད་དུ (redraw_listener_T *rln = redraw_listeners; rln != NULL; rln = rln->rl_next)
    {
	གལ་ཏེ (rln->rl_id == 0)
	    // Listener has been removed, skip
	    continue;
	གལ་ཏེ (start && rln->rl_callbacks.on_start.cb_name != NULL)
	{
	    call_callback(&rln->rl_callbacks.on_start, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	དེ་མིན གལ་ཏེ (rln->rl_callbacks.on_end.cb_name != NULL)
	{
	    call_callback(&rln->rl_callbacks.on_end, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_in_progress = false;

    གལ་ཏེ (start)
	inside_redraw_on_start_cb = false;
}
#endif // FEAT_EVAL
