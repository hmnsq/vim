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

ساکن خالی win_update(win_T *wp);
#اگر_تعریف_شدہ FEAT_STL_OPT
ساکن خالی redraw_custom_statusline(win_T *wp);
#اگر_ختم
ساکن خالی borrow_stl_vsep_hl(خالی);
#اگر defined(FEAT_تلاش_EXTRA) || defined(FEAT_CLIPBOARD)
ساکن صحیح  did_update_one_کھڑکی;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
ساکن خالی redraw_listener_cleanup(خالی);
ساکن خالی invoke_redraw_listener_آغاز_or_انتہا(bool آغاز);
#اگر_ختم

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
    ساکن صحیح	did_intro = FALSE;
#اگر_تعریف_شدہ FEAT_GUI
    صحیح		did_one = FALSE;
    صحیح		did_undraw = FALSE;
    صحیح		gui_cursor_ستون = 0;
    صحیح		gui_cursor_صف = 0;
#اگر_ختم
    صحیح		no_update = FALSE;
    صحیح		save_pum_will_redraw = pum_will_redraw;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    صحیح		did_redraw_کھڑکی = FALSE;
#اگر_ختم
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    اگر (!screen_valid(TRUE))
	واپس FAIL;

    اگر (type == UPD_VALID_NO_UPDATE)
    {
	no_update = TRUE;
	type = 0;
    }

#اگر_تعریف_شدہ FEAT_EVAL
    {
	buf_T *buf;

	// Before updating the screen, notify any listeners of changed text.
	FOR_ALL_BUFFERS(buf)
	    invoke_listeners(buf);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_DIFF
    // May have postponed updating diffs.
    اگر (need_diff_redraw)
	diff_redraw(TRUE);
#اگر_ختم

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
    اگر (curwin->w_سطریں_valid == 0 && type < UPD_NOT_VALID
#اگر_تعریف_شدہ FEAT_TERMINAL
	    && !رکن_do_update_کھڑکی(curwin)
#اگر_ختم
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    اگر (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	اگر (type > UPD_INVERTED_ALL)
	    curwin->w_سطریں_valid = 0;	// don't use w_lines[].wl_size now
	واپس FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    صحیح hid_cursor = !sync_output_فعال();
    اگر (hid_cursor)
	cursor_off();
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_ENABLE);

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // Update popup_mask if needed.  This may set w_redraw_top and w_redraw_bot
    // in some windows.
    may_update_popup_نقاب(type);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYN_HL
    ++display_tick;	    // let syntax code know we're in a next round of
			    // display updating
#اگر_ختم
    اگر (no_update)
	++no_win_do_سطریں_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    اگر (پیغام_scrolled)
    {
	clear_cmdline = TRUE;
	اگر (type != UPD_CLEAR)
	{
	    اگر (پیغام_scrolled > صفیں - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    ورنہ
	    {
		check_for_delay(FALSE);
		اگر (screen_ins_سطریں(0, 0, پیغام_scrolled, (صحیح)صفیں, 0, تہی)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    اگر (wp->w_winrow < پیغام_scrolled)
		    {
			اگر (W_WINROW(wp) + wp->w_اونچائی > پیغام_scrolled
				&& wp->w_redr_type < UPD_REDRAW_چوٹی
				&& wp->w_سطریں_valid > 0
				&& wp->w_topline == wp->w_سطریں[0].wl_lnum)
			{
			    wp->w_upd_صفیں = پیغام_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_چوٹی;
			}
			ورنہ
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    اگر (W_WINROW(wp) + wp->w_اونچائی
					 + wp->w_حالت_اونچائی <= پیغام_scrolled)
				wp->w_redr_حالت = true;
			}
		    }
		}
		اگر (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#اگر defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#اگر_ختم
	}
	پیغام_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    اگر (need_highlight_بدلا)
	highlight_بدلا();

    اگر (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    اگر (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#اگر_تعریف_شدہ FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    اگر (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? نمبر_چوڑائی(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    invoke_redraw_listener_آغاز_or_انتہا(true);
#اگر_ختم

    // Only start redrawing if there is really something to do.
    اگر (type == UPD_INVERTED)
	update_curswant();
    اگر (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_سطریں[0].wl_valid
#اگر_تعریف_شدہ FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#اگر_ختم
		    && curwin->w_topline == curwin->w_سطریں[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_فعال
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_منوال == VIsual_منوال
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
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_بفر->b_mod_سیٹ)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    برائے (wwp = firstwin; wwp != wp; wwp = wwp->w_اگلا)
		اگر (wwp->w_بفر == wp->w_بفر)
		    توڑو;
	    اگر (wwp == wp && syntax_present(wp))
		syn_ڈھیر_apply_changes(wp->w_بفر);
	}
    }
#اگر_ختم

    اگر (pum_redraw_اندر_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#اگر defined(FEAT_تلاش_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_کھڑکی = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    screen_تلاش_hl.rm.regprog = تہی;
#اگر_ختم
    FOR_ALL_WINDOWS(wp)
    {
	override_success = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

	اگر (wp->w_redr_type != 0)
	{
	    cursor_off();
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    did_redraw_کھڑکی = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
	    اگر (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		اگر (gui.اندر_use && (wp == curwin || cursor_is_sleeping()))
		{
		    gui_cursor_ستون = gui.cursor_ستون;
		    gui_cursor_صف = gui.cursor_صف;
		    gui_undraw_cursor();
		    did_undraw = TRUE;
		}
	    }
#اگر_ختم
	    win_update(wp);
	}

	// redraw status line after the window to minimize cursor movement
	اگر (wp->w_redr_حالت)
	{
	    cursor_off();
	    win_redr_حالت(wp, TRUE); // any popup menu will be redrawn below
	}

	اگر (override_success)
	    نکالو_highlight_overrides();

    }
#اگر defined(FEAT_تلاش_EXTRA)
    انتہا_تلاش_hl();
#اگر_ختم

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    اگر (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_vsep_چوڑائی > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_بفر->b_mod_سیٹ = false;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    اگر (did_redraw_کھڑکی || popup_need_redraw())
	update_popups(win_update);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMINAL
    FOR_ALL_WINDOWS(wp)
	// If this window contains a terminal, after redrawing all windows, the
	// dirty row range can be reset.
	رکن_did_update_کھڑکی(wp);
#اگر_ختم

    after_updating_screen(TRUE);

    // Clear or redraw the command line.  Done last, because scrolling may
    // mess up the command line.
    اگر (clear_cmdline || redraw_cmdline || redraw_منوال)
	showmode();

    اگر (no_update)
	--no_win_do_سطریں_ins;

    // May put up an introductory message when not editing a file
    اگر (!did_intro)
	maybe_intro_پیغام();
    did_intro = TRUE;

#اگر_تعریف_شدہ FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    اگر (gui.اندر_use)
    {
	اگر (did_undraw && !gui_mch_is_blink_off())
	{
	    mch_disable_flush();
	    برآمد_flush();	// required before updating the cursor
	    mch_enable_flush();

	    // Put the GUI position where the cursor was, gui_update_cursor()
	    // uses that.
	    gui.ستون = gui_cursor_ستون;
	    gui.صف = gui_cursor_صف;
	    gui.ستون = mb_fix_ستون(gui.ستون, gui.صف);
	    gui_update_cursor(FALSE, FALSE);
	    gui_may_flush();
	    screen_cur_ستون = gui.ستون;
	    screen_cur_صف = gui.صف;
	}
	ورنہ
	    برآمد_flush();
	gui_update_scrollbars(FALSE);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_IMAGE
    // Make sure to do this last!
    draw_image_placements();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    invoke_redraw_listener_آغاز_or_انتہا(false);
    redraw_listener_cleanup();
#اگر_ختم

    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_DISABLE);
    اگر (hid_cursor)
	cursor_on();

    واپس OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    صحیح
statusline_صف(win_T *wp)
{
#اگر defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    اگر (wp->w_چوکھٹا->fr_اونچائی == wp->w_حالت_اونچائی && !popup_is_popup(wp))
	واپس wp->w_winrow;
#اگر_ختم
    واپس W_WINROW(wp) + wp->w_اونچائی;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    خالی
win_redr_حالت(win_T *wp, صحیح ignore_pum غیر_مستعمل)
{
    صحیح		صف;
    صحیح		fillchar;
    صحیح		attr;
    صحیح		i;
    ساکن صحیح  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    اگر (busy)
	واپس;
    busy = TRUE;

    صف = statusline_صف(wp);

    wp->w_redr_حالت = false;
    اگر (wp->w_حالت_اونچائی == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    ورنہ اگر (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_حالت = true;
    }
#اگر_تعریف_شدہ FEAT_STL_OPT
    ورنہ اگر (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#اگر_ختم
    ورنہ
    {
	char_u	*p;
	صحیح	plen;
	صحیح	NameBufflen;
	صحیح	this_ru_ستون;
	صحیح	n;			// scratch value

	fillchar = fillchar_حالت(&attr, wp);

	get_trans_bufname(wp->w_بفر);
	p = NameBuff;
	plen = (صحیح)STRLEN(p);

	اگر ((bt_help(wp->w_بفر)
#اگر_تعریف_شدہ FEAT_QUICKFIX
		    || wp->w_p_pvw
#اگر_ختم
		    || buf_Is_بدلا(wp->w_بفر)
		    || wp->w_بفر->b_p_ro)
		&& plen < MAXPATHL - 1)
	{
	    *(p + plen++) = ' ';	// replace NUL with space
	    *(p + plen) = NUL;		// NUL terminate the string
	}
	اگر (bt_help(wp->w_بفر))
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#اگر_تعریف_شدہ FEAT_QUICKFIX
	اگر (wp->w_p_pvw)
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#اگر_ختم
	اگر (buf_Is_بدلا(wp->w_بفر) && !bt_terminal(wp->w_بفر))
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", "[+]");
	اگر (wp->w_بفر->b_p_ro)
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_ستون = ru_ستون - (cmdline_چوڑائی - wp->w_چوڑائی);
	n = (wp->w_چوڑائی + 1) / 2;
	اگر (this_ru_ستون < n)
	    this_ru_ستون = n;
	اگر (this_ru_ستون <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	ورنہ اگر (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    برائے (i = 0; p[i] != NUL && plen >= this_ru_ستون - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    اگر (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	ورنہ اگر (plen > this_ru_ستون - 1)
	{
	    p += plen - (this_ru_ستون - 1);
	    *p = '<';
	    plen = this_ru_ستون - 1;
	}

	screen_puts(p, صف, wp->w_wincol, attr);
	screen_fill(صف, صف + 1, plen + wp->w_wincol,
		    this_ru_ستون + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	برائے (i = 1; i < wp->w_حالت_اونچائی; i++)
	    screen_fill(صف + i, صف + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	اگر ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_ستون - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, صف, (صحیح)(this_ru_ستون - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	اگر (p_sc && *p_sloc == 's')
	{
	    n = this_ru_ستون - plen - 2;		    // perform the calculation here so we only do it once
	    صحیح	چوڑائی = MIN(10, n);

	    اگر (چوڑائی > 0)
		screen_puts_len(showcmd_buf, چوڑائی, صف,
				wp->w_wincol + this_ru_ستون - چوڑائی - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    اگر (wp->w_vsep_چوڑائی != 0 && wp->w_حالت_اونچائی != 0 && redrawing())
    {
	برائے (i = 0; i < wp->w_حالت_اونچائی; i++)
	{
	    صحیح r = صف + i;
	    اگر (stl_connected(wp))
		fillchar = fillchar_حالت(&attr, wp);
	    ورنہ
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
    ساکن خالی
borrow_stl_vsep_hl(خالی)
{
    win_T   *بایاں = تہی;
    win_T   *دایاں = تہی;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    اگر (!redrawing() || Screen_سطریں == تہی)
	واپس;

    FOR_ALL_WINDOWS(بایاں)
    {
	اگر (بایاں->w_حالت_اونچائی == 0 || بایاں->w_vsep_چوڑائی == 0)
	    جاری;
	// A window of zero width has no status line cell to borrow from.
	اگر (بایاں->w_چوڑائی == 0)
	    جاری;
	اگر (!stl_connected(بایاں))
	    جاری;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = تہی;
	صحیح آغاز = 0;
	صحیح انتہا = 0;

	FOR_ALL_WINDOWS(دایاں)
	{
	    اگر (دایاں == بایاں || دایاں->w_حالت_اونچائی == 0)
		جاری;
	    اگر (دایاں->w_wincol != W_ENDCOL(بایاں) + 1)
		جاری;
	    صحیح l_stl_صف = W_WINROW(بایاں) + بایاں->w_اونچائی;
	    صحیح r_stl_صف = W_WINROW(دایاں) + دایاں->w_اونچائی;

	    آغاز = l_stl_صف > r_stl_صف ? l_stl_صف : r_stl_صف;
	    انتہا = l_stl_صف + بایاں->w_حالت_اونچائی
				< r_stl_صف + دایاں->w_حالت_اونچائی
		? l_stl_صف + بایاں->w_حالت_اونچائی
		: r_stl_صف + دایاں->w_حالت_اونچائی;
	    اگر (آغاز < انتہا)
	    {
		neighbour = دایاں;
		توڑو;
	    }
	}
	اگر (neighbour == تہی)
	    جاری;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	صحیح	hl;
	اگر (بایاں != curwin && neighbour != curwin
		&& fillchar_حالت(&hl, بایاں) != ' ')
	    جاری;

	// Source: prefer curwin's side; otherwise left window's right edge.
	صحیح dst_ستون = W_ENDCOL(بایاں);
	صحیح src_ستون = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(بایاں) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	اگر (dst_ستون >= screen_Columns || src_ستون >= screen_Columns)
	    جاری;
	اگر (انتہا > screen_صفیں)
	    انتہا = screen_صفیں;

	برائے (صحیح r = آغاز; r < انتہا; r++)
	{
	    غیرعلامتی	dst_off = سطر_انحراف[r] + dst_ستون;
	    sattr_T	attr = ScreenAttrs[سطر_انحراف[r] + src_ستون];

	    اگر (ScreenAttrs[dst_off] != attr)
	    {
		ScreenAttrs[dst_off] = attr;
		screen_char(dst_off, r, dst_ستون);
	    }
	}
    }
}

#اگر_تعریف_شدہ FEAT_STL_OPT
/*
 * Redraw the status line according to 'statusline' and take care of any
 * errors encountered.
 */
    ساکن خالی
redraw_custom_statusline(win_T *wp)
{
    ساکن صحیح	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    اگر (entered)
	واپس;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#اگر_ختم

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    خالی
showruler(صحیح always)
{
    اگر (!always && !redrawing())
	واپس;
    اگر (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_حالت = true;
	واپس;
    }
#اگر defined(FEAT_STL_OPT)
    اگر ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_حالت_اونچائی)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    ورنہ
#اگر_ختم
	win_redr_ruler(curwin, always, FALSE);

    اگر (need_maketitle
#اگر_تعریف_شدہ FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_اندر_ICON))
	    || (p_title && (stl_syntax & STL_اندر_TITLE))
#اگر_ختم
       )
	maketitle();

    // Redraw the tab pages line if needed.
    اگر (redraw_tabline)
	draw_tabline();

#اگر defined(FEAT_TABPANEL)
    اگر (redraw_tabpanel)
	draw_tabpanel();
#اگر_ختم
}

    خالی
win_redr_ruler(win_T *wp, صحیح always, صحیح ignore_pum)
{
    صحیح	خالی_خانے_سطر = FALSE;

    // If 'ruler' off don't do anything
    اگر (!p_ru)
	واپس;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    اگر (wp->w_cursor.lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
	واپس;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    اگر (wp == lastwin && lastwin->w_حالت_اونچائی == 0)
	اگر (edit_submode != تہی)
	    واپس;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    اگر (!ignore_pum && pum_visible())
	واپس;

#اگر_تعریف_شدہ FEAT_STL_OPT
    اگر (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	واپس;
    }
#اگر_ختم

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    اگر ((State & منوال_داخل_کرو) == 0
		&& *ml_get_buf(wp->w_بفر, wp->w_cursor.lnum, FALSE) == NUL)
	خالی_خانے_سطر = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    اگر (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.ستون != wp->w_ru_cursor.ستون
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_بفر->b_ml.ml_سطر_گنتی != wp->w_ru_سطر_گنتی
#اگر_تعریف_شدہ FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#اگر_ختم
	    || خالی_خانے_سطر != wp->w_ru_خالی_خانے)
    {
	صحیح	صف;
	صحیح	fillchar;
	صحیح	attr;
	صحیح	off;
	صحیح	چوڑائی;
	colnr_T	virtcol;
#تعریف_کرو RULER_BUF_LEN 70
	char_u	بفر[RULER_BUF_LEN];
	صحیح	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	صحیح	rel_poslen;
	صحیح	this_ru_ستون;
	صحیح	n1;			    // scratch value
	صحیح	n2;			    // scratch value
	bool	override_success =
	    ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	اگر (wp->w_حالت_اونچائی)
	{
	    صف = statusline_صف(wp);
	    fillchar = fillchar_حالت(&attr, wp);
	    off = wp->w_wincol;
	    چوڑائی = wp->w_چوڑائی;
	}
	ورنہ
	{
	    صف = صفیں - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_ستون_off;
	    چوڑائی = cmdline_چوڑائی;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	اگر (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, تہی, &virtcol, تہی, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((حرف *)بفر, RULER_BUF_LEN, _("%ld,"),
		(wp->w_بفر->b_ml.ml_flags & ML_خالی_خانے)
		    ? 0L
		    : (طویل)(wp->w_cursor.lnum));
	bufferlen += ستون_print(بفر + bufferlen, RULER_BUF_LEN - bufferlen,
			خالی_خانے_سطر ? 0 : (صحیح)wp->w_cursor.ستون + 1,
			(صحیح)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	اگر (wp->w_حالت_اونچائی == 0)	// can't use last char of screen
	    ++n1;

	this_ru_ستون = ru_ستون - (cmdline_چوڑائی - چوڑائی);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (چوڑائی + 1) / 2;
	اگر (this_ru_ستون < n2)
	    this_ru_ستون = n2;
	اگر (this_ru_ستون + n1 < چوڑائی)
	{
	    // need at least space for rel_pos + NUL
	    جب (this_ru_ستون + n1 < چوڑائی
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		اگر (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, بفر + bufferlen);
		ورنہ
		    بفر[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((حرف *)بفر + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	اگر (has_mbyte)
	{
	    برائے (n1 = 0, n2 = 0; بفر[n1] != NUL; n1 += (*mb_ptr2len)(بفر + n1))
	    {
		n2 += (*mb_ptr2cells)(بفر + n1);
		اگر (this_ru_ستون + n2 > چوڑائی)
		{
		    bufferlen = n1;
		    بفر[bufferlen] = NUL;
		    توڑو;
		}
	    }
	}
	ورنہ اگر (this_ru_ستون + bufferlen > چوڑائی)
	{
	    bufferlen = چوڑائی - this_ru_ستون;
	    بفر[bufferlen] = NUL;
	}

	screen_puts(بفر, صف, this_ru_ستون + off, attr);
	n1 = redraw_cmdline;
	screen_fill(صف, صف + 1,
		this_ru_ستون + off + bufferlen,
		(off + چوڑائی),
		fillchar, fillchar, attr);
	// don't redraw the cmdline because of showing the ruler
	redraw_cmdline = n1;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_خالی_خانے = خالی_خانے_سطر;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_سطر_گنتی = wp->w_بفر->b_ml.ml_سطر_گنتی;
#اگر_تعریف_شدہ FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#اگر_ختم

	اگر (override_success)
	    نکالو_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    خالی
after_updating_screen(صحیح may_resize_shell غیر_مستعمل)
{
    updating_screen = FALSE;
#اگر_تعریف_شدہ FEAT_GUI
    اگر (may_resize_shell)
	gui_may_resize_shell();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    رکن_check_channel_closed_recently();
#اگر_ختم

#اگر_تعریف_شدہ HAVE_DROP_FILE
    // If handle_drop() was called while updating_screen was TRUE need to
    // handle the drop now.
    handle_any_postponed_drop();
#اگر_ختم
}

/*
 * Update all windows that are editing the current buffer.
 */
    خالی
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
    ساکن صحیح
متن_مقصد_screenline(win_T *wp, char_u *متن, صحیح ستون)
{
    صحیح		off = (صحیح)(current_Screen_سطر - Screen_سطریں);

    اگر (has_mbyte)
    {
	صحیح	cells;
	صحیح	u8c, u8cc[MAX_MCO];
	صحیح	i;
	صحیح	idx;
	صحیح	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	صحیح	پچھلا_c = 0;		// previous Arabic character
	صحیح	پچھلا_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	اگر (wp->w_p_rl)
	    idx = off;
	ورنہ
# endif
	    idx = off + ستون;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	برائے (p = متن; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    اگر (ستون + cells > wp->w_چوڑائی)
		توڑو;
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		idx = off + wp->w_چوڑائی - ستون - cells;
# endif
	    Screen_سطریں[idx] = *p;
	    اگر (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		اگر (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_سطریں_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    پچھلا_c = u8c;
# endif
		}
		ورنہ
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
			    pc = پچھلا_c;
			    pc1 = پچھلا_c1;
			    nc = utf_ptr2char(p + c_len);
			    پچھلا_c1 = u8cc[0];
			}
			ورنہ
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = پچھلا_c;
			    pc1 = pcc[0];
			}
			پچھلا_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			Screen_سطریں[idx] = firstbyte;
		    }
		    ورنہ
			پچھلا_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_سطریں_UC[idx] = u8c;
		    برائے (i = 0; i < Screen_mco; ++i)
		    {
			Screen_سطریں_C[i][idx] = u8cc[i];
			اگر (u8cc[i] == 0)
			    توڑو;
		    }
		}
		اگر (cells > 1)
		    Screen_سطریں[idx + 1] = 0;
	    }
	    ورنہ اگر (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    ورنہ اگر (cells > 1)
		// double-width character
		Screen_سطریں[idx + 1] = p[1];
	    ستون += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    ورنہ
    {
	صحیح len = (صحیح)STRLEN(متن);
	صحیح n = wp->w_چوڑائی - ستون;

	اگر (len > n)
	    len = n;
	اگر (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		mch_memmove(current_Screen_سطر, متن, len);
	    ورنہ
# endif
		mch_memmove(current_Screen_سطر + ستون, متن, len);
	    ستون += len;
	}
    }
    واپس ستون;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MENU
/*
 * Draw the window toolbar.
 */
    ساکن خالی
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    صحیح		شے_idx = 0;
    صحیح		شے_گنتی = 0;
    صحیح		ستون = 0;
    صحیح		اگلا_ستون;
    صحیح		off = (صحیح)(current_Screen_سطر - Screen_سطریں);
    bool	override_success =
	ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);
    صحیح		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    صحیح		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_اشیاء);
    FOR_ALL_بچہ_MENUS(wp->w_winbar, menu)
	++شے_گنتی;
    wp->w_winbar_اشیاء = ALLOC_CLEAR_MULT(winbar_شے_T, شے_گنتی + 1);

    // TODO: use fewer spaces if there is not enough room
    برائے (menu = wp->w_winbar->children;
			  menu != تہی && ستون < wp->w_چوڑائی; menu = menu->اگلا)
    {
	space_مقصد_screenline(off + ستون, fill_attr);
	اگر (++ستون >= wp->w_چوڑائی)
	    توڑو;
	اگر (ستون > 1)
	{
	    space_مقصد_screenline(off + ستون, fill_attr);
	    اگر (++ستون >= wp->w_چوڑائی)
		توڑو;
	}

	wp->w_winbar_اشیاء[شے_idx].wb_startcol = ستون;
	space_مقصد_screenline(off + ستون, button_attr);
	اگر (++ستون >= wp->w_چوڑائی)
	    توڑو;

	اگلا_ستون = متن_مقصد_screenline(wp, menu->name, ستون);
	جب (ستون < اگلا_ستون)
	{
	    ScreenAttrs[off + ستون] = button_attr;
	    ++ستون;
	}
	wp->w_winbar_اشیاء[شے_idx].wb_endcol = ستون;
	wp->w_winbar_اشیاء[شے_idx].wb_menu = menu;
	++شے_idx;

	اگر (ستون >= wp->w_چوڑائی)
	    توڑو;
	space_مقصد_screenline(off + ستون, button_attr);
	++ستون;
    }
    جب (ستون < wp->w_چوڑائی)
    {
	space_مقصد_screenline(off + ستون, fill_attr);
	++ستون;
    }
    wp->w_winbar_اشیاء[شے_idx].wb_menu = تہی; // end marker

    screen_سطر(wp, wp->w_winrow, wp->w_wincol, wp->w_چوڑائی,
							  wp->w_چوڑائی, -1, 0);

    اگر (override_success)
	نکالو_highlight_overrides();
}
#اگر_ختم

#اگر defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    ساکن خالی
copy_متن_attr(
    صحیح		off,
    char_u	*buf,
    صحیح		len,
    صحیح		attr)
{
    صحیح		i;

    mch_memmove(Screen_سطریں + off, buf, (size_t)len);
    اگر (enc_utf8)
	vim_memset(Screen_سطریں_UC + off, 0, حجم(u8char_T) * (size_t)len);
    برائے (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    ساکن خالی
fold_سطر(
    win_T	*wp,
    طویل	fold_گنتی,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    صحیح		صف)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*چوٹی, *bot;
    linenr_T	lnume = lnum + fold_گنتی - 1;
    صحیح		len;
    char_u	*متن;
    صحیح		fdc;
    صحیح		ستون;
    صحیح		txtcol;
    صحیح		off = (صحیح)(current_Screen_سطر - Screen_سطریں);
    صحیح		ri;

    // Build the fold line:
    // 1. Add the cmdwin_type for the command-line window
    // 2. Add the 'foldcolumn'
    // 3. Add the 'number' or 'relativenumber' column
    // 4. Compose the text
    // 5. Add the text
    // 6. set highlighting for the Visual area an other text
    ستون = 0;

    // 1. Add the cmdwin_type for the command-line window
    // Ignores 'rightleft', this window is never right-left.
    اگر (wp == cmdwin_win)
    {
	Screen_سطریں[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	اگر (enc_utf8)
	    Screen_سطریں_UC[off] = 0;
	++ستون;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    کرو { \
	اگر (wp->w_p_rl) \
	    برائے (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_چوڑائی - (p) - (l)) + ri] = v; \
	 ورنہ \
	    برائے (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } جب (0)
# ورنہ
#  define RL_MEMSET(p, v, l) \
    کرو { \
	برائے (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } جب (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, ستون);
    اگر (fdc > 0)
    {
	char_u	*p;
	صحیح	i;
	صحیح	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	برائے (i = 0; i < fdc; i++)
	{
	    صحیح		ch;

	    اگر (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    ورنہ
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		idx = off + wp->w_چوڑائی - i - 1 - ستون;
	    ورنہ
# endif
		idx = off + ستون + i;
	    اگر (enc_utf8)
	    {
		اگر (ch >= 0x80)
		{
		    Screen_سطریں_UC[idx] = ch;
		    Screen_سطریں_C[0][idx] = 0;
		    Screen_سطریں[idx] = 0x80;
		}
		ورنہ
		{
		    Screen_سطریں[idx] = ch;
		    Screen_سطریں_UC[idx] = 0;
		}
	    }
	    ورنہ
		Screen_سطریں[idx] = ch;
	}

	RL_MEMSET(ستون, HL_ATTR(HLF_FC), fdc);
	ستون += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    //متن
    RL_MEMSET(ستون, HL_ATTR(HLF_FL), wp->w_چوڑائی - ستون);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    اگر (signcolumn_on(wp))
    {
	len = wp->w_چوڑائی - ستون;
	اگر (len > 0)
	{
	    اگر (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		// the line number isn't reversed
		copy_متن_attr(off + wp->w_چوڑائی - len - ستون,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    ورنہ
#  endif
		copy_متن_attr(off + ستون, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    ستون += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    اگر (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_چوڑائی - ستون;
	اگر (len > 0)
	{
	    صحیح	    w = نمبر_چوڑائی(wp);
	    طویل    عدد;
	    حرف    *fmt = "%*ld ";

	    اگر (len > w + 1)
		len = w + 1;

	    اگر (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		عدد = (طویل)lnum;
	    ورنہ
	    {
		// 'relativenumber', don't use negative numbers
		عدد = labs((طویل)get_cursor_rel_lnum(wp, lnum));
		اگر (عدد == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    عدد = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((حرف *)buf, حجم(buf), fmt, w, عدد);
# ifdef FEAT_RIGHTLEFT
	    اگر (wp->w_p_rl)
		// the line number isn't reversed
		copy_متن_attr(off + wp->w_چوڑائی - len - ستون, buf, len,
							     HL_ATTR(HLF_FL));
	    ورنہ
# endif
		copy_متن_attr(off + ستون, buf, len, HL_ATTR(HLF_FL));
	    ستون += len;
	}
    }

    // 4. Compose the folded-line string with 'foldtext', if set.
    متن = get_foldtext(wp, lnum, lnume, foldinfo, buf);

    txtcol = ستون;	// remember where text starts

    // 5. move the text to current_ScreenLine.  Fill up with "fold" from
    //    'fillchars'.
    //    Right-left text is put in columns 0 - number-col, normal text is put
    //    in columns number-col - window-width.
    ستون = متن_مقصد_screenline(wp, متن, ستون);

    // Fill the rest of the line with the fold filler
    جب (ستون < wp->w_چوڑائی)
    {
	صحیح c = wp->w_fill_chars.fold;
	صحیح idx = off + ستون;

# ifdef FEAT_RIGHTLEFT
	اگر (wp->w_p_rl)
	    idx = off + wp->w_چوڑائی - 1 - ستون;
# endif

	اگر (enc_utf8)
	{
	    اگر (c >= 0x80)
	    {
		Screen_سطریں_UC[idx] = c;
		Screen_سطریں_C[0][idx] = 0;
		Screen_سطریں[idx] = 0x80; // avoid storing zero
	    }
	    ورنہ
	    {
		Screen_سطریں_UC[idx] = 0;
		Screen_سطریں[idx] = c;
	    }
	}
	ورنہ
	    Screen_سطریں[idx] = c;
	++ستون;
    }

    اگر (متن != buf)
	vim_free(متن);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    اگر (VIsual_فعال && wp->w_بفر == curwin->w_بفر)
    {
	اگر (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    چوٹی = &curwin->w_cursor;
	    bot = &VIsual;
	}
	ورنہ
	{
	    // Visual is before curwin->w_cursor
	    چوٹی = &VIsual;
	    bot = &curwin->w_cursor;
	}
	اگر (lnum >= چوٹی->lnum
		&& lnume <= bot->lnum
		&& (VIsual_منوال != 'v'
		    || ((lnum > چوٹی->lnum
			    || (lnum == چوٹی->lnum
				&& چوٹی->ستون == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->ستون - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_بفر, lnume))))))
	{
	    اگر (VIsual_منوال == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		اگر (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_چوڑائی)
		{
		    اگر (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_چوڑائی)
			len = wp->w_old_cursor_lcol;
		    ورنہ
			len = wp->w_چوڑائی - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (صحیح)wp->w_old_cursor_fcol);
		}
	    }
	    ورنہ
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_چوڑائی - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    اگر (wp->w_p_cc_ستون_تعداد)
    {
	صحیح i = 0;
	صحیح j = wp->w_p_cc_ستون_تعداد[i];
	صحیح old_txtcol = txtcol;

	جب (j > -1)
	{
	    txtcol += j;
	    اگر (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    ورنہ
		txtcol -= wp->w_leftcol;
	    اگر (txtcol >= 0 && txtcol < wp->w_چوڑائی)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_ستون_تعداد[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    اگر (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	اگر (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	ورنہ
	    txtcol -= wp->w_leftcol;
	اگر (txtcol >= 0 && txtcol < wp->w_چوڑائی)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_سطر(wp, صف + W_WINROW(wp), wp->w_wincol, wp->w_چوڑائی, wp->w_چوڑائی,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    اگر (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_صف = صف;
	curwin->w_cline_اونچائی = 1;
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
#اگر_ختم

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
    ساکن خالی
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_بفر;
    صحیح		type;
    صحیح		چوٹی_انتہا = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    صحیح		mid_آغاز = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    صحیح		mid_انتہا = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    صحیح		bot_آغاز = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    صحیح		scrolled_نیچے_کو = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    صحیح		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //سطریں
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    صحیح		چوٹی_مقصد_mod = FALSE;    // redraw above mod_top
#اگر_ختم

    صحیح		صف;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    صحیح		idx;		// current index in w_lines[]
    صحیح		srow;		// starting row of the current line

    صحیح		eof = FALSE;	// if TRUE, we hit the end of the file
    صحیح		didline = FALSE; // if TRUE, we finished the last line
    صحیح		i;
    طویل	j;
    ساکن صحیح	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		old_wrow = wp->w_wrow;
    صحیح		old_wcol = wp->w_wcol;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
    طویل	fold_گنتی;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_سطر 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    صحیح		did_update = DID_NONE;
    linenr_T	syntax_آخری_parsed = 0;		// last parsed text line
#اگر_ختم
    linenr_T	mod_چوٹی = 0;
    linenr_T	mod_bot = 0;
#اگر defined(FEAT_SYN_HL) || defined(FEAT_تلاش_EXTRA)
    صحیح		save_got_int;
#اگر_ختم
    bool	override_success;

#اگر defined(FEAT_تلاش_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    اگر (!did_update_one_کھڑکی)
    {
	did_update_one_کھڑکی = TRUE;
# ifdef FEAT_تلاش_EXTRA
	آغاز_تلاش_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	اگر (clip_ستارہ.دستیاب && clip_isautosel_ستارہ())
	    clip_update_selection(&clip_ستارہ);
	اگر (clip_plus.دستیاب && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    اگر (wp->w_p_cuc)
	validate_virtcol_win(wp);
#اگر_ختم

    type = wp->w_redr_type;

    اگر (type == UPD_NOT_VALID)
    {
	wp->w_redr_حالت = true;
	wp->w_سطریں_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    اگر (wp->w_اونچائی + WINBAR_اونچائی(wp) == 0
	    || (wp->w_چوکھٹا->fr_اونچائی == wp->w_حالت_اونچائی
#اگر defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#اگر_ختم
	       ))
    {
	wp->w_redr_type = 0;
	واپس;
    }

    // Window is zero-width: Only need to draw the separator.
    اگر (wp->w_چوڑائی == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	واپس;
    }

    override_success = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);


#اگر_تعریف_شدہ FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    اگر (رکن_do_update_کھڑکی(wp))
    {
	رکن_update_کھڑکی(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	اگر (winbar_اونچائی(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	اگر (override_success)
	    نکالو_highlight_overrides();
	واپس;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    init_تلاش_hl(wp, &screen_تلاش_hl);
#اگر_ختم

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    اگر (wp->w_skipcol > 0 && wp->w_چوڑائی > win_ستون_off(wp))
    {
	صحیح w = 0;
	صحیح width1 = wp->w_چوڑائی - win_ستون_off(wp);
	صحیح width2 = width1 + win_ستون_off2(wp);
	صحیح جوڑو = width1;

	جب (w < wp->w_skipcol)
	{
	    اگر (w > 0)
		جوڑو = width2;
	    w += جوڑو;
	}
	اگر (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - جوڑو;
    }

#اگر_تعریف_شدہ FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? نمبر_چوڑائی(wp) : 0;
    اگر (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    ورنہ
#اگر_ختم

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_چوٹی = wp->w_redraw_چوٹی;
	اگر (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	ورنہ
	    mod_bot = 0;
	اگر (buf->b_mod_سیٹ)
	{
	    اگر (mod_چوٹی == 0 || mod_چوٹی > buf->b_mod_چوٹی)
	    {
		mod_چوٹی = buf->b_mod_چوٹی;
#اگر_تعریف_شدہ FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		اگر (syntax_present(wp))
		{
		    mod_چوٹی -= buf->b_s.b_syn_sync_linebreaks;
		    اگر (mod_چوٹی < 1)
			mod_چوٹی = 1;
		}
#اگر_ختم
	    }
	    اگر (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    اگر (screen_تلاش_hl.rm.regprog != تہی
		    && re_multiline(screen_تلاش_hl.rm.regprog))
		چوٹی_مقصد_mod = TRUE;
	    ورنہ
	    {
		matchitem_T *cur = wp->w_مطابقت_سر;

		جب (cur != تہی)
		{
		    اگر (cur->mit_مطابقت.regprog != تہی
				       && re_multiline(cur->mit_مطابقت.regprog))
		    {
			چوٹی_مقصد_mod = TRUE;
			توڑو;
		    }
		    cur = cur->mit_اگلا;
		}
	    }
#اگر_ختم
	}

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	اگر (تلاش_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    اگر (mod_چوٹی == 0 || mod_چوٹی > تلاش_hl_has_cursor_lnum)
		mod_چوٹی = تلاش_hl_has_cursor_lnum;
	    اگر (mod_bot == 0 || mod_bot < تلاش_hl_has_cursor_lnum + 1)
		mod_bot = تلاش_hl_has_cursor_lnum + 1;
	}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_FOLDING
	اگر (mod_چوٹی != 0 && hasAnyFolding(wp))
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
	    برائے (i = 0; i < wp->w_سطریں_valid; ++i)
		اگر (wp->w_سطریں[i].wl_valid)
		{
		    اگر (wp->w_سطریں[i].wl_lastlnum < mod_چوٹی)
			lnumt = wp->w_سطریں[i].wl_lastlnum + 1;
		    اگر (lnumb == MAXLNUM && wp->w_سطریں[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_سطریں[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			اگر (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (خالی)hasFoldingWin(wp, mod_چوٹی, &mod_چوٹی, تہی, TRUE, تہی);
	    اگر (mod_چوٹی > lnumt)
		mod_چوٹی = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (خالی)hasFoldingWin(wp, mod_bot, تہی, &mod_bot, TRUE, تہی);
	    ++mod_bot;
	    اگر (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#اگر_ختم

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	اگر (mod_چوٹی != 0 && mod_چوٹی < wp->w_topline)
	{
	    اگر (mod_bot > wp->w_topline)
		mod_چوٹی = wp->w_topline;
#اگر_تعریف_شدہ FEAT_SYN_HL
	    ورنہ اگر (syntax_present(wp))
		چوٹی_انتہا = 1;
#اگر_ختم
	}
    }
    wp->w_redraw_چوٹی = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    تلاش_hl_has_cursor_lnum = 0;
#اگر_ختم


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    اگر (type == UPD_REDRAW_چوٹی)
    {
	j = 0;
	برائے (i = 0; i < wp->w_سطریں_valid; ++i)
	{
	    j += wp->w_سطریں[i].wl_سائز;
	    اگر (j >= wp->w_upd_صفیں)
	    {
		چوٹی_انتہا = j;
		توڑو;
	    }
	}
	اگر (چوٹی_انتہا == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	ورنہ
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
#اگر_تعریف_شدہ FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#اگر_ختم
	    )
    {
	اگر (mod_چوٹی != 0
		&& wp->w_topline == mod_چوٹی
		&& (!wp->w_سطریں[0].wl_valid
		    || wp->w_topline == wp->w_سطریں[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	ورنہ اگر (wp->w_سطریں[0].wl_valid
		&& (wp->w_topline < wp->w_سطریں[0].wl_lnum
#اگر_تعریف_شدہ FEAT_DIFF
		    || (wp->w_topline == wp->w_سطریں[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#اگر_ختم
		   ))
	{
	    // New topline is above old topline: May scroll down.
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		برائے (ln = wp->w_topline; ln < wp->w_سطریں[0].wl_lnum; ++ln)
		{
		    ++j;
		    اگر (j >= wp->w_اونچائی - 2)
			توڑو;
		    (خالی)hasFoldingWin(wp, ln, تہی, &ln, TRUE, تہی);
		}
	    }
	    ورنہ
#اگر_ختم
		j = wp->w_سطریں[0].wl_lnum - wp->w_topline;
	    اگر (j < wp->w_اونچائی - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_سطریں[0].wl_lnum - 1,
								wp->w_اونچائی);
#اگر_تعریف_شدہ FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		اگر (wp->w_سطریں[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_سطریں[0].wl_lnum)
							  - wp->w_old_topfill;
#اگر_ختم
		اگر (i < wp->w_اونچائی - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    اگر (i > 0)
			check_for_delay(FALSE);
		    اگر (win_ins_سطریں(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			اگر (wp->w_سطریں_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    چوٹی_انتہا = i;
			    scrolled_نیچے_کو = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    اگر ((wp->w_سطریں_valid += j) > wp->w_اونچائی)
				wp->w_سطریں_valid = wp->w_اونچائی;
			    برائے (idx = wp->w_سطریں_valid; idx - j >= 0; idx--)
				wp->w_سطریں[idx] = wp->w_سطریں[idx - j];
			    جب (idx >= 0)
				wp->w_سطریں[idx--].wl_valid = FALSE;
			}
		    }
		    ورنہ
			mid_آغاز = 0;		// redraw all lines
		}
		ورنہ
		    mid_آغاز = 0;		// redraw all lines
	    }
	    ورنہ
		mid_آغاز = 0;		// redraw all lines
	}
	ورنہ
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    صف = 0;
	    برائے (i = 0; i < wp->w_سطریں_valid && i < صفیں; i++)
	    {
		اگر (wp->w_سطریں[i].wl_valid
			&& wp->w_سطریں[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    توڑو;
		}
		صف += wp->w_سطریں[i].wl_سائز;
	    }
	    اگر (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//سطریں
		mid_آغاز = 0;
	    }
	    ورنہ
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#اگر_تعریف_شدہ FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		اگر (wp->w_سطریں[0].wl_lnum == wp->w_topline)
		    صف += wp->w_old_topfill;
		ورنہ
		    صف += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		صف -= wp->w_topfill;
#اگر_ختم
		اگر (صف > صفیں)  // just in case
		    صف = صفیں;
		اگر (صف > 0)
		{
		    check_for_delay(FALSE);
		    اگر (win_del_سطریں(wp, 0, صف, FALSE, wp == firstwin, 0)
									 == OK)
			bot_آغاز = wp->w_اونچائی - صف;
		    ورنہ
			mid_آغاز = 0;		// redraw all lines
		}
		اگر ((صف == 0 || bot_آغاز < 999) && wp->w_سطریں_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_آغاز = 0;
		    idx = 0;
		    برائے (;;)
		    {
			wp->w_سطریں[idx] = wp->w_سطریں[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			اگر (صف > 0 && bot_آغاز + صف
				 + (صحیح)wp->w_سطریں[j].wl_سائز > wp->w_اونچائی)
			{
			    wp->w_سطریں_valid = idx + 1;
			    توڑو;
			}
			bot_آغاز += wp->w_سطریں[idx++].wl_سائز;

			// stop at the last valid entry in w_lines[].wl_size
			اگر (++j >= wp->w_سطریں_valid)
			{
			    wp->w_سطریں_valid = idx;
			    توڑو;
			}
		    }
#اگر_تعریف_شدہ FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    اگر (wp->w_p_diff && bot_آغاز > 0)
			wp->w_سطریں[0].wl_سائز = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#اگر_ختم
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	اگر (mid_آغاز == 0)
	    mid_انتہا = wp->w_اونچائی;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	اگر (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    ورنہ
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_آغاز = 0;
	mid_انتہا = wp->w_اونچائی;
    }

    اگر (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_آغاز = 0;
	mid_انتہا = wp->w_اونچائی;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    اگر ((VIsual_فعال && buf == curwin->w_بفر)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, مقصد;

	اگر (VIsual_فعال)
	{
	    اگر (VIsual_منوال != wp->w_old_visual_منوال
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		اگر (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    مقصد = VIsual.lnum;
		}
		ورنہ
		{
		    from = VIsual.lnum;
		    مقصد = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		اگر (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		اگر (wp->w_old_cursor_lnum > مقصد)
		    مقصد = wp->w_old_cursor_lnum;
		اگر (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		اگر (wp->w_old_visual_lnum > مقصد)
		    مقصد = wp->w_old_visual_lnum;
	    }
	    ورنہ
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		اگر (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    مقصد = wp->w_old_cursor_lnum;
		}
		ورنہ
		{
		    from = wp->w_old_cursor_lnum;
		    مقصد = curwin->w_cursor.lnum;
		    اگر (from == 0)	// Visual mode just started
			from = مقصد;
		}

		اگر (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.ستون != wp->w_old_visual_ستون)
		{
		    اگر (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    اگر (wp->w_old_visual_lnum > مقصد)
			مقصد = wp->w_old_visual_lnum;
		    اگر (VIsual.lnum < from)
			from = VIsual.lnum;
		    اگر (VIsual.lnum > مقصد)
			مقصد = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    اگر (VIsual_منوال == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_انتہا_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		اگر (curwin->w_curswant == MAXCOL)
		{
		    اگر (get_ve_flags() & VE_بلاک)
		    {
			pos_T	    pos;
			صحیح	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			برائے (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.ستون = (صحیح)ml_get_buf_len(wp->w_بفر, pos.lnum);
			    getvvcol(wp, &pos, تہی, تہی, &t, 0);
			    اگر (toc < t)
				toc = t;
			}
			++toc;
		    }
		    ورنہ
			toc = MAXCOL;
		}

		اگر (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    اگر (from > VIsual.lnum)
			from = VIsual.lnum;
		    اگر (مقصد < VIsual.lnum)
			مقصد = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	ورنہ
	{
	    // Use the line numbers of the old Visual area.
	    اگر (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		مقصد = wp->w_old_visual_lnum;
	    }
	    ورنہ
	    {
		from = wp->w_old_visual_lnum;
		مقصد = wp->w_old_cursor_lnum;
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
	    مقصد = MIN(مقصد, wp->w_botline);
	}

	// Find the minimal part to be updated.
	// Watch out for scrolling that made entries in w_lines[] invalid.
	// E.g., CTRL-U makes the first half of w_lines[] invalid and sets
	// top_end; need to redraw from top_end to the "to" line.
	// A middle mouse click with a Visual selection may change the text
	// above the Visual area and reset wl_valid, do count these for
	// mid_end (in srow).
	اگر (mid_آغاز > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    اگر (scrolled_نیچے_کو)
		mid_آغاز = چوٹی_انتہا;
	    ورنہ
		mid_آغاز = 0;
	    جب (lnum < from && idx < wp->w_سطریں_valid)	// find start
	    {
		اگر (wp->w_سطریں[idx].wl_valid)
		    mid_آغاز += wp->w_سطریں[idx].wl_سائز;
		ورنہ اگر (!scrolled_نیچے_کو)
		    srow += wp->w_سطریں[idx].wl_سائز;
		++idx;
#اگر_تعریف_شدہ FEAT_FOLDING
		اگر (idx < wp->w_سطریں_valid && wp->w_سطریں[idx].wl_valid)
		    lnum = wp->w_سطریں[idx].wl_lnum;
		ورنہ
#اگر_ختم
		    ++lnum;
	    }
	    srow += mid_آغاز;
	    mid_انتہا = wp->w_اونچائی;
	    برائے ( ; idx < wp->w_سطریں_valid; ++idx)		// find end
	    {
		اگر (wp->w_سطریں[idx].wl_valid
			&& wp->w_سطریں[idx].wl_lnum >= مقصد + 1)
		{
		    // Only update until first row of this line
		    mid_انتہا = srow;
		    توڑو;
		}
		srow += wp->w_سطریں[idx].wl_سائز;
	    }
	}
    }

    اگر (VIsual_فعال && buf == curwin->w_بفر)
    {
	wp->w_old_visual_منوال = VIsual_منوال;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_ستون = VIsual.ستون;
	wp->w_old_curswant = curwin->w_curswant;
    }
    ورنہ
    {
	wp->w_old_visual_منوال = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_ستون = 0;
    }

#اگر defined(FEAT_SYN_HL) || defined(FEAT_تلاش_EXTRA)
    // reset got_int, otherwise regexp won't work
    save_got_int = got_int;
    got_int = 0;
#اگر_ختم
#اگر_تعریف_شدہ SYN_وقت_حد_بندی
    // Set the time limit to 'redrawtime'.
    redrawtime_حد_بندی_سیٹ = TRUE;
    init_regexp_timeout(p_rdt);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
    win_foldinfo.fi_level = 0;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MENU
    // Draw the window toolbar, if there is one.
    // TODO: only when needed.
    اگر (winbar_اونچائی(wp) > 0)
	redraw_win_toolbar(wp);
#اگر_ختم

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#اگر_تعریف_شدہ FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_میدان(spv);
    اگر (spell_check_کھڑکی(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_چوٹی == 0;
    }
#اگر_ختم

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    صف = 0;
    srow = 0;
    برائے (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	اگر (صف == wp->w_اونچائی)
	{
	    didline = TRUE;
	    توڑو;
	}

	// stop updating when hit the end of the file
	اگر (lnum > buf->b_ml.ml_سطر_گنتی)
	{
	    eof = TRUE;
	    توڑو;
	}

	// Remember the starting row of the line that is going to be dealt
	// with.  It is used further down when the line doesn't fit.
	srow = صف;

	// Update a line when it is in an area that needs updating, when it
	// has changes or w_lines[idx] is invalid.
	// "bot_start" may be halfway a wrapped line after using
	// win_del_lines(), check if the current line includes it.
	// When syntax folding is being used, the saved syntax states will
	// already have been updated, we can't see where the syntax state is
	// the same again, just update until the end of the window.
	اگر (صف < چوٹی_انتہا
		|| (صف >= mid_آغاز && صف < mid_انتہا)
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		|| چوٹی_مقصد_mod
#اگر_ختم
		|| idx >= wp->w_سطریں_valid
		|| (صف + wp->w_سطریں[idx].wl_سائز > bot_آغاز)
		|| (mod_چوٹی != 0
		    && (lnum == mod_چوٹی
			|| (lnum >= mod_چوٹی
			    && (lnum < mod_bot
#اگر_تعریف_شدہ FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_سطر
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_بدلا(lnum)))
#اگر_ختم
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_مطابقت_سر != تہی
						    && buf->b_mod_سیٹ
						    && buf->b_mod_xlines != 0)
#اگر_ختم
				))))
#اگر_تعریف_شدہ FEAT_SYN_HL
		|| (wp->w_p_cul && lnum == wp->w_cursor.lnum)
		|| lnum == wp->w_آخری_cursorline
#اگر_ختم
				)
	{
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	    اگر (lnum == mod_چوٹی)
		چوٹی_مقصد_mod = FALSE;
#اگر_ختم

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    اگر (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_چوٹی && lnum < MAX(mod_bot, mod_چوٹی + 1)
		    && (!scrolled_نیچے_کو || صف >= چوٹی_انتہا))
	    {
		scrolled_for_mod = TRUE;

		صحیح		old_cline_اونچائی = 0;
		صحیح		old_صفیں = 0;
		صحیح		new_صفیں = 0;
		صحیح		xtra_صفیں;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		برائے (i = idx; i < wp->w_سطریں_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    اگر (wp->w_سطریں[i].wl_valid
			    && wp->w_سطریں[i].wl_lnum == mod_bot)
			توڑو;
		    اگر (wp->w_سطریں[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_اونچائی = wp->w_سطریں[i].wl_سائز;
		    old_صفیں += wp->w_سطریں[i].wl_سائز;
#اگر_تعریف_شدہ FEAT_FOLDING
		    اگر (wp->w_سطریں[i].wl_valid
			    && wp->w_سطریں[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			جب (i < wp->w_سطریں_valid
						  && !wp->w_سطریں[i].wl_valid)
			    old_صفیں += wp->w_سطریں[i++].wl_سائز;
			توڑو;
		    }
#اگر_ختم
		}

		اگر (i >= wp->w_سطریں_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_آغاز = 0;
		}
		ورنہ
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    برائے (l = lnum; l < mod_bot; ++l)
		    {
			اگر (dollar_vcol >= 0 && wp == curwin &&
				old_cline_اونچائی > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_صفیں += old_cline_اونچائی;
#اگر_تعریف_شدہ FEAT_FOLDING
			ورنہ اگر (hasFoldingWin(wp, l, تہی, &l, TRUE, تہی))
			    ++new_صفیں;
#اگر_ختم
			ورنہ
			    new_صفیں += plines_correct_topline(wp, l, TRUE);
			++j;
			اگر (new_صفیں > wp->w_اونچائی - صف - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_صفیں = 9999;
			    توڑو;
			}
		    }
		    xtra_صفیں = new_صفیں - old_صفیں;
		    اگر (xtra_صفیں < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			اگر (صف - xtra_صفیں >= wp->w_اونچائی - 2)
			    mod_bot = MAXLNUM;
			ورنہ
			{
			    check_for_delay(FALSE);
			    اگر (win_del_سطریں(wp, صف,
					  -xtra_صفیں, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    ورنہ
				bot_آغاز = wp->w_اونچائی + xtra_صفیں;
			}
		    }
		    ورنہ اگر (xtra_صفیں > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			اگر (صف + xtra_صفیں >= wp->w_اونچائی - 2)
			    mod_bot = MAXLNUM;
			ورنہ
			{
			    check_for_delay(FALSE);
			    اگر (win_ins_سطریں(wp, صف + old_صفیں,
					     xtra_صفیں, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    ورنہ اگر (چوٹی_انتہا > صف + old_صفیں)
				// Scrolled the part at the top that requires
				// updating down.
				چوٹی_انتہا += xtra_صفیں;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    اگر (mod_bot != MAXLNUM && i != j)
		    {
			اگر (j < i)
			{
			    صحیح x = صف + new_صفیں;

			    // move entries in w_lines[] upwards
			    برائے (;;)
			    {
				// stop at last valid entry in w_lines[]
				اگر (i >= wp->w_سطریں_valid)
				{
				    wp->w_سطریں_valid = j;
				    توڑو;
				}
				wp->w_سطریں[j] = wp->w_سطریں[i];
				// stop at a line that won't fit
				اگر (x + (صحیح)wp->w_سطریں[j].wl_سائز
							   > wp->w_اونچائی)
				{
				    wp->w_سطریں_valid = j + 1;
				    توڑو;
				}
				x += wp->w_سطریں[j++].wl_سائز;
				++i;
			    }
			    اگر (bot_آغاز > x)
				bot_آغاز = x;
			}
			ورنہ // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_سطریں_valid += j;
			    اگر (wp->w_سطریں_valid > wp->w_اونچائی)
				wp->w_سطریں_valid = wp->w_اونچائی;
			    برائے (i = wp->w_سطریں_valid; i - j >= idx; --i)
				wp->w_سطریں[i] = wp->w_سطریں[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    جب (i >= idx)
			    {
				wp->w_سطریں[i].wl_سائز = 0;
				wp->w_سطریں[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#اگر_تعریف_شدہ FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_گنتی = folded_گنتی(wp, lnum, &win_foldinfo);
	    اگر (fold_گنتی != 0)
	    {
		fold_سطر(wp, fold_گنتی, &win_foldinfo, lnum, صف);
		++صف;
		--fold_گنتی;
		wp->w_سطریں[idx].wl_folded = TRUE;
		wp->w_سطریں[idx].wl_lastlnum = lnum + fold_گنتی;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
# ifdef FEAT_SPELL
		spv.spv_capcol_lnum = 0;
# endif
	    }
	    ورنہ
#اگر_ختم
	    اگر (idx < wp->w_سطریں_valid
		    && wp->w_سطریں[idx].wl_valid
		    && wp->w_سطریں[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_سطریں[idx].wl_سائز > wp->w_اونچائی
#اگر_تعریف_شدہ FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#اگر_ختم
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		صف = wp->w_اونچائی + 1;
	    }
	    ورنہ
	    {
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		prepare_تلاش_hl(wp, &screen_تلاش_hl, lnum);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		اگر (syntax_آخری_parsed != 0 && syntax_آخری_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_انتہا_parsing(wp, syntax_آخری_parsed + 1);
#اگر_ختم

		// Display one line.
		صف = win_سطر(wp, lnum, srow, wp->w_اونچائی, 0, &spv);

#اگر_تعریف_شدہ FEAT_FOLDING
		wp->w_سطریں[idx].wl_folded = FALSE;
		wp->w_سطریں[idx].wl_lastlnum = lnum;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
		did_update = DID_سطر;
		syntax_آخری_parsed = lnum;
#اگر_ختم
	    }

	    wp->w_سطریں[idx].wl_lnum = lnum;
	    wp->w_سطریں[idx].wl_valid = TRUE;

	    صحیح is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    اگر (صف > wp->w_اونچائی || صف + wp->w_winrow >= صفیں)
	    {
		// we may need the size of that too long line later on
		اگر (dollar_vcol == -1 || !is_curline)
		    wp->w_سطریں[idx].wl_سائز = plines_win(wp, lnum, TRUE);
		++idx;
		توڑو;
	    }
	    اگر (dollar_vcol == -1 || !is_curline)
		wp->w_سطریں[idx].wl_سائز = صف - srow;
	    ++idx;
#اگر_تعریف_شدہ FEAT_FOLDING
	    lnum += fold_گنتی + 1;
#ورنہ
	    ++lnum;
#اگر_ختم
	}
	ورنہ
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    اگر ((wp->w_p_nu && mod_چوٹی != 0 && lnum >= mod_bot
			&& buf->b_mod_سیٹ && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_آخری_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#اگر_تعریف_شدہ FEAT_FOLDING
		fold_گنتی = folded_گنتی(wp, lnum, &win_foldinfo);
		اگر (fold_گنتی != 0)
		    fold_سطر(wp, fold_گنتی, &win_foldinfo, lnum, صف);
		ورنہ
#اگر_ختم
		    (خالی)win_سطر(wp, lnum, srow, wp->w_اونچائی,
					       wp->w_سطریں[idx].wl_سائز, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    صف += wp->w_سطریں[idx++].wl_سائز;
	    اگر (صف > wp->w_اونچائی)	// past end of screen
		توڑو;
#اگر_تعریف_شدہ FEAT_FOLDING
	    lnum = wp->w_سطریں[idx - 1].wl_lastlnum + 1;
#ورنہ
	    ++lnum;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
	    did_update = DID_NONE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#اگر_ختم
	}

	اگر (lnum > buf->b_ml.ml_سطر_گنتی)
	{
	    eof = TRUE;
	    توڑو;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	اگر (idx >= صفیں)
	    توڑو;
    }

    // End of loop over all window lines.

#اگر_تعریف_شدہ FEAT_SYN_HL
    // Now that the window has been redrawn with the old and new cursor line,
    // update w_last_cursorline.
    wp->w_آخری_cursorline = wp->w_p_cul ? wp->w_cursor.lnum : 0;
#اگر_ختم
    wp->w_آخری_cursor_lnum_rnu = wp->w_p_rnu ? wp->w_cursor.lnum : 0;

#اگر_تعریف_شدہ FEAT_VTP
    // Rewrite the character at the end of the screen line.
    // See the version that was fixed.
    اگر (use_vtp() && get_conpty_fix_type() < 1)
    {
	صحیح k;

	برائے (k = 0; k < صفیں; ++k)
	    اگر (enc_utf8)
		اگر ((*mb_off2cells)(سطر_انحراف[k] + topframe->fr_چوڑائی - 2,
					   سطر_انحراف[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_چوڑائی - 2, 1, 2,
			    FALSE, FALSE);
		ورنہ
		    screen_draw_rectangle(k, topframe->fr_چوڑائی - 1, 1, 1,
			    FALSE, FALSE);
	    ورنہ
		screen_char(سطر_انحراف[k] + topframe->fr_چوڑائی - 1, k,
			cmdline_چوڑائی - 1);
    }
#اگر_ختم

    اگر (idx > wp->w_سطریں_valid)
	wp->w_سطریں_valid = idx;

#اگر_تعریف_شدہ FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    اگر (syntax_آخری_parsed != 0 && syntax_present(wp))
	syntax_انتہا_parsing(wp, syntax_آخری_parsed + 1);
#اگر_ختم

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_خالی_خانے_صفیں = 0;
#اگر_تعریف_شدہ FEAT_DIFF
    wp->w_filler_صفیں = 0;
#اگر_ختم
    اگر (!eof && !didline)
    {
	اگر (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#اگر_تعریف_شدہ FEAT_DIFF
	ورنہ اگر (diff_check_fill(wp, lnum) >= wp->w_اونچائی - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_صفیں = wp->w_اونچائی - srow;
	}
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	ورنہ اگر (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#اگر_ختم
	ورنہ اگر (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    صحیح		scr_صف = W_WINROW(wp) + wp->w_اونچائی - 1;
	    صحیح		علامت  = wp->w_fill_chars.lastline;
	    صحیح		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(علامت, &fillbuf[0]);
	    mb_char2bytes(علامت, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_چوڑائی > 2 ? 2 : wp->w_چوڑائی) * charlen,
			    scr_صف, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_صف, scr_صف + 1,
		    (صحیح)wp->w_wincol + 2, (صحیح)W_ENDCOL(wp),
		    علامت, ' ', HL_ATTR(HLF_AT));
	    سیٹ_خالی_خانے_صفیں(wp, srow);
	    wp->w_botline = lnum;
	}
	ورنہ اگر (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    صحیح آغاز_ستون = (صحیح)W_ENDCOL(wp) - 3;
	    صحیح علامت    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_اونچائی - 1,
		    W_WINROW(wp) + wp->w_اونچائی,
		    (آغاز_ستون < wp->w_wincol ? wp->w_wincol : آغاز_ستون),
		    (صحیح)W_ENDCOL(wp),
		    علامت, علامت, HL_ATTR(HLF_AT));
	    سیٹ_خالی_خانے_صفیں(wp, srow);
	    wp->w_botline = lnum;
	}
	ورنہ
	{
	    win_draw_انتہا(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_اونچائی, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    ورنہ
    {
	draw_vsep_win(wp, صف);
	اگر (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_سطر_گنتی + 1;
#اگر_تعریف_شدہ FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    اگر (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		اگر (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		ورنہ
		    i = wp->w_fill_chars.diff;
		اگر (صف + j > wp->w_اونچائی)
		    j = wp->w_اونچائی - صف;
		win_draw_انتہا(wp, i, i, TRUE, صف, صف + (صحیح)j, HLF_DED);
		صف += j;
	    }
#اگر_ختم
	}
	ورنہ اگر (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	اگر (WIN_IS_POPUP(wp))
	    win_draw_انتہا(wp, ' ', ' ', FALSE, صف, wp->w_اونچائی, HLF_AT);
	ورنہ
	    win_draw_انتہا(wp, wp->w_fill_chars.eob, ' ', FALSE,
						   صف, wp->w_اونچائی, HLF_EOB);
    }

#اگر_تعریف_شدہ SYN_وقت_حد_بندی
    disable_regexp_timeout();
    redrawtime_حد_بندی_سیٹ = FALSE;
#اگر_ختم

    // Reset the type of redrawing required, the window has been updated.
    wp->w_redr_type = 0;
#اگر_تعریف_شدہ FEAT_DIFF
    wp->w_old_topfill = wp->w_topfill;
    wp->w_old_botfill = wp->w_botfill;
#اگر_ختم

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
#اگر_ختم
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
		ورنہ
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#اگر_ختم
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    اگر (wp->w_redr_type != 0)
	    {
		// Don't update for changes in buffer again.
		bool b = curbuf->b_mod_سیٹ;
		curbuf->b_mod_سیٹ = false;
		j = curbuf->b_mod_xlines;
		curbuf->b_mod_xlines = 0;
		curs_columns(TRUE);
		win_update(curwin);
		curbuf->b_mod_سیٹ = b;
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

#اگر defined(FEAT_SYN_HL) || defined(FEAT_تلاش_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    اگر (!got_int)
	got_int = save_got_int;
#اگر_ختم

    اگر (override_success)
	نکالو_highlight_overrides();
}

#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    ساکن خالی
update_prepare(خالی)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    اگر (gui.اندر_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_تلاش_EXTRA
    آغاز_تلاش_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_نقاب(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    ساکن خالی
update_اختتام(خالی)
{
    اگر (redraw_cmdline || redraw_منوال)
	showmode();

# ifdef FEAT_تلاش_EXTRA
    انتہا_تلاش_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    اگر (gui.اندر_use)
    {
	برآمد_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#اگر_ختم

#اگر defined(FEAT_NETBEANS_INTG)
    خالی
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
	    || State == منوال_ASKMORE || State == منوال_HITRETURN
	    || پیغام_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	واپس;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_redr_type != 0)
	    win_update(wp);
	اگر (wp->w_redr_حالت)
	    win_redr_حالت(wp, FALSE);
    }

# اگر defined(FEAT_TABPANEL)
    اگر (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_اختتام();
}
#اگر_ختم

#اگر defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    خالی
update_کھڑکی(win_T *wp)
{
    // return if already busy updating
    اگر (updating_screen)
	واپس;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    اگر (clip_ستارہ.دستیاب && clip_isautosel_ستارہ())
	clip_update_selection(&clip_ستارہ);
    اگر (clip_plus.دستیاب && clip_isautosel_plus())
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

    اگر (wp->w_redr_حالت || p_ru
# ifdef FEAT_STL_OPT
	    || *p_stl != NUL || *wp->w_p_stl != NUL
# endif
	    )
	win_redr_حالت(wp, FALSE);

# ifdef FEAT_PROP_POPUP
    // Display popup windows on top of everything.
    update_popups(win_update);
# endif

    update_اختتام();
}
#اگر_ختم

/*
 * Redraw as soon as possible.  When the command line is not scrolled redraw
 * right away and restore what was on the command line.
 * Return a code indicating what happened.
 */
    صحیح
redraw_asap(صحیح type)
{
    صحیح		صفیں_2;
    صحیح		ستون_تعداد = screen_Columns;
    صحیح		r;
    صحیح		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    صحیح		i;
    u8char_T	*screenlineUC = تہی;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = تہی;	// copy from ScreenLines2[]

    redraw_later(type);
    اگر (پیغام_scrolled
	    || (State != منوال_معمول && State != منوال_معمول_BUSY)
	    || exiting)
	واپس ret;

    // Allocate space to save the text displayed in the command line area.
    صفیں_2 = screen_صفیں - cmdline_صف;
    screenline = LALLOC_MULT(schar_T, صفیں_2 * ستون_تعداد);
    screenattr = LALLOC_MULT(sattr_T, صفیں_2 * ستون_تعداد);
    اگر (screenline == تہی || screenattr == تہی)
	ret = 2;
    اگر (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, صفیں_2 * ستون_تعداد);
	اگر (screenlineUC == تہی)
	    ret = 2;
	برائے (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, صفیں_2 * ستون_تعداد);
	    اگر (screenlineC[i] == تہی)
		ret = 2;
	}
    }
    اگر (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, صفیں_2 * ستون_تعداد);
	اگر (screenline2 == تہی)
	    ret = 2;
    }

    اگر (ret != 2)
    {
	// Save the text displayed in the command line area.
	برائے (r = 0; r < صفیں_2; ++r)
	{
	    mch_memmove(screenline + r * ستون_تعداد,
			Screen_سطریں + سطر_انحراف[cmdline_صف + r],
			(size_t)ستون_تعداد * حجم(schar_T));
	    mch_memmove(screenattr + r * ستون_تعداد,
			ScreenAttrs + سطر_انحراف[cmdline_صف + r],
			(size_t)ستون_تعداد * حجم(sattr_T));
	    اگر (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * ستون_تعداد,
			    Screen_سطریں_UC + سطر_انحراف[cmdline_صف + r],
			    (size_t)ستون_تعداد * حجم(u8char_T));
		برائے (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * ستون_تعداد,
				Screen_سطریں_C[i] + سطر_انحراف[cmdline_صف + r],
				(size_t)ستون_تعداد * حجم(u8char_T));
	    }
	    اگر (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * ستون_تعداد,
			    ScreenLines2 + سطر_انحراف[cmdline_صف + r],
			    (size_t)ستون_تعداد * حجم(schar_T));
	}

	update_screen(0);
	ret = 3;

	اگر (must_redraw == 0)
	{
	    صحیح	off = (صحیح)(current_Screen_سطر - Screen_سطریں);

	    // Restore the text displayed in the command line area.
	    برائے (r = 0; r < صفیں_2; ++r)
	    {
		mch_memmove(current_Screen_سطر,
			    screenline + r * ستون_تعداد,
			    (size_t)ستون_تعداد * حجم(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * ستون_تعداد,
			    (size_t)ستون_تعداد * حجم(sattr_T));
		اگر (enc_utf8)
		{
		    mch_memmove(Screen_سطریں_UC + off,
				screenlineUC + r * ستون_تعداد,
				(size_t)ستون_تعداد * حجم(u8char_T));
		    برائے (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_سطریں_C[i] + off,
				    screenlineC[i] + r * ستون_تعداد,
				    (size_t)ستون_تعداد * حجم(u8char_T));
		}
		اگر (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * ستون_تعداد,
				(size_t)ستون_تعداد * حجم(schar_T));
		screen_سطر(curwin, cmdline_صف + r, 0, ستون_تعداد, ستون_تعداد, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    اگر (enc_utf8)
    {
	vim_free(screenlineUC);
	برائے (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    اگر (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_پیغام();

    setcursor();

    واپس ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    خالی
redraw_after_callback(صحیح call_update_screen, صحیح do_پیغام_2)
{
    ++redrawing_for_callback;

    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_ENABLE);

    اگر (State == منوال_HITRETURN || State == منوال_ASKMORE
	    || State == منوال_SETWSIZE || State == منوال_EXTERNCMD
	    || State == منوال_CONFIRM || exmode_فعال)
    {
	اگر (do_پیغام_2)
	    repeat_پیغام();
    }
    ورنہ اگر (State & منوال_CMDLINE)
    {
	اگر (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	اگر (cmdline_صف > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    اگر (پیغام_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    ورنہ اگر (State & (منوال_معمول | منوال_داخل_کرو | منوال_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	اگر (پیغام_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    پیغام_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_DISABLE);
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	برآمد_flush_cursor(FALSE, FALSE);
    ورنہ
#اگر_ختم
	برآمد_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    خالی
redraw_later(صحیح type)
{
    redraw_win_later(curwin, type);
}

    خالی
redraw_win_later(
    win_T	*wp,
    صحیح		type)
{
#اگر_تعریف_شدہ FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    اگر (inside_redraw_on_آغاز_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    ورنہ
#اگر_ختم
    اگر (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	اگر (type >= UPD_NOT_VALID)
	    wp->w_سطریں_valid = 0;
	اگر (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    خالی
redraw_later_clear(خالی)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    خالی
redraw_all_later(صحیح type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    سیٹ_must_redraw(type);
}

#اگر 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    خالی
redraw_all_windows_later(صحیح type)
{
    redraw_all_later(type);
# ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
# endif
}
#اگر_ختم

/*
 * Set "must_redraw" to "type" unless it already has a higher value
 * or it is currently not allowed.
 */
    خالی
سیٹ_must_redraw(صحیح type)
{
    اگر (!redraw_not_allowed &&
#اگر_تعریف_شدہ FEAT_EVAL
	    !inside_redraw_on_آغاز_cb &&
#اگر_ختم
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    خالی
redraw_curbuf_later(صحیح type)
{
    redraw_buf_later(curbuf, type);
}

    خالی
redraw_buf_later(buf_T *buf, صحیح type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_بفر == buf)
	    redraw_win_later(wp, type);
    }
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_بفر == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر (wp->w_بفر == buf)
	    redraw_win_later(wp, type);
#اگر_ختم
#اگر defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    اگر (curwin->w_بفر == buf)
	redraw_win_later(curwin, type);
#اگر_ختم
}

#اگر defined(FEAT_SIGNS)
    خالی
redraw_buf_سطر_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_بفر == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#اگر_ختم

#اگر defined(FEAT_کام_CHANNEL)
    خالی
redraw_buf_and_حالت_later(buf_T *buf, صحیح type)
{
    win_T	*wp;

    اگر (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	واپس;
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_بفر == buf)
	{
	    redraw_win_later(wp, type);
	    wp->w_redr_حالت = true;
	}
    }
}
#اگر_ختم

/*
 * mark the ruler for redraw when the last window has no status line and the
 * ruler takes its place in the last screen line; showmode() draws it
 */
    ساکن خالی
ruler_redraw_lastwin(خالی)
{
    اگر (p_ru && lastwin->w_حالت_اونچائی == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    خالی
حالت_redraw_all(خالی)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_حالت_اونچائی)
	{
	    wp->w_redr_حالت = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    خالی
حالت_redraw_curbuf(خالی)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_حالت_اونچائی != 0 && wp->w_بفر == curbuf)
	{
	    wp->w_redr_حالت = true;
	    redraw_later(UPD_VALID);
	}
    اگر (lastwin->w_بفر == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    خالی
redraw_statuslines(خالی)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_redr_حالت)
	{
	    bool ret = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_حالت(wp, FALSE);
	    اگر (ret)
		نکالو_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    اگر (redraw_tabline)
	draw_tabline();

#اگر defined(FEAT_TABPANEL)
    اگر (redraw_tabpanel)
	draw_tabpanel();
#اگر_ختم
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    خالی
win_redraw_آخری_حالت(چوکھٹا_T *frp)
{
    اگر (frp->fr_layout == FR_برگ)
	frp->fr_win->w_redr_حالت = true;
    ورنہ اگر (frp->fr_layout == FR_صف)
    {
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    win_redraw_آخری_حالت(frp);
    }
    ورنہ // frp->fr_layout == FR_COL
    {
	frp = frp->fr_بچہ;
	جب (frp->fr_اگلا != تہی)
	    frp = frp->fr_اگلا;
	win_redraw_آخری_حالت(frp);
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
    خالی
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    خالی
redraw_win_range_later(
    win_T	*wp,
    linenr_T	پہلا,
    linenr_T	آخری)
{
    اگر (آخری >= wp->w_topline && پہلا < wp->w_botline)
    {
	اگر (wp->w_redraw_چوٹی == 0 || wp->w_redraw_چوٹی > پہلا)
	    wp->w_redraw_چوٹی = پہلا;
	اگر (wp->w_redraw_bot == 0 || wp->w_redraw_bot < آخری)
	    wp->w_redraw_bot = آخری;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    خالی
redraw_win_range_now(
    win_T	*wp,
    linenr_T	پہلا,
    linenr_T	آخری)
{
    اگر (آخری >= wp->w_topline && پہلا < wp->w_botline)
    {
	اگر (wp->w_redraw_چوٹی == 0 || wp->w_redraw_چوٹی > پہلا)
	    wp->w_redraw_چوٹی = پہلا;
	اگر (wp->w_redraw_bot == 0 || wp->w_redraw_bot < آخری)
	    wp->w_redraw_bot = آخری;
	اگر (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#اگر_تعریف_شدہ FEAT_EVAL
ساکن bool redraw_cb_اندر_پیش_رفت = false;

    خالی
f_redraw_listener_جوڑو(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    ساکن صحیح		id;

    اگر (check_secure())
	واپس;

    اگر (redraw_cb_اندر_پیش_رفت)
    {
	emsg(_(e_cannot_جوڑو_redraw_listener_اندر_listener_callback));
	واپس;
    }

    اگر (check_for_dict_arg(argvars, 0) == FAIL)
	واپس;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    اگر (rln == تہی)
	واپس;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    اگر (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	اگر (cb.cb_name == تہی)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    واپس;
	}
	copy_callback(&rln->rl_callbacks.on_آغاز, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    اگر (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	اگر (cb.cb_name == تہی)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_آغاز);
	    vim_free(rln);
	    واپس;
	}
	copy_callback(&rln->rl_callbacks.on_انتہا, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    اگر (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	واپس;
    }

    rln->rl_اگلا = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_نمبر;
    rettv->vval.v_نمبر = id;

    واپس;
}

    ساکن خالی
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_آغاز);
    free_callback(&rln->rl_callbacks.on_انتہا);

    vim_free(rln);
}


    ساکن خالی
redraw_listener_cleanup(خالی)
{
    برائے (redraw_listener_T *rln = redraw_listeners; rln != تہی;)
    {
	redraw_listener_T *اگلا = rln->rl_اگلا;
	اگر (rln->rl_id == 0)
	{
	    اگر (redraw_listeners == rln)
		redraw_listeners = rln->rl_اگلا;
	    redraw_listener_free(rln);
	}
	rln = اگلا;
    }
}

/*
 * Return the redraw listener struct with the specified id. Returns NULL if not
 * found.
 */
    ساکن redraw_listener_T *
get_redraw_listener(صحیح id)
{
    برائے (redraw_listener_T *rln = redraw_listeners; rln != تہی; rln = rln->rl_اگلا)
	اگر (rln->rl_id == id)
	    واپس rln;
    واپس تہی;
}

    خالی
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    صحیح			id;
    redraw_listener_T	*rln;

    اگر (check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = argvars[0].vval.v_نمبر;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_نمبر;
    اگر (rln == تہی)
    {
	rettv->vval.v_نمبر = 0;
	واپس;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_نمبر = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    ساکن خالی
invoke_redraw_listener_آغاز_or_انتہا(bool آغاز)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    اگر (آغاز)
	inside_redraw_on_آغاز_cb = true;

    redraw_cb_اندر_پیش_رفت = true;
    برائے (redraw_listener_T *rln = redraw_listeners; rln != تہی; rln = rln->rl_اگلا)
    {
	اگر (rln->rl_id == 0)
	    // Listener has been removed, skip
	    جاری;
	اگر (آغاز && rln->rl_callbacks.on_آغاز.cb_name != تہی)
	{
	    call_callback(&rln->rl_callbacks.on_آغاز, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	ورنہ اگر (rln->rl_callbacks.on_انتہا.cb_name != تہی)
	{
	    call_callback(&rln->rl_callbacks.on_انتہا, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_اندر_پیش_رفت = false;

    اگر (آغاز)
	inside_redraw_on_آغاز_cb = false;
}
#اگر_ختم // FEAT_EVAL
