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

#ضمن "vim.h"

ساكن فراغ win_update(win_T *wp);
#جے_تعریف_شدہ FEAT_STL_OPT
ساكن فراغ redraw_custom_statusline(win_T *wp);
#جے_انت
ساكن فراغ borrow_stl_vsep_hl(فراغ);
#إذا defined(FEAT_تلاش_EXTRA) || defined(FEAT_CLIPBOARD)
ساكن صحيح  did_update_one_کھڑکی;
#جے_انت

#جے_تعریف_شدہ FEAT_EVAL
ساكن فراغ redraw_listener_cleanup(فراغ);
ساكن فراغ invoke_redraw_listener_شروع_or_سرا(bool شروع);
#جے_انت

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    صحيح
update_screen(صحيح type_arg)
{
    صحيح		type = type_arg;
    win_T	*wp;
    ساكن صحيح	did_intro = FALSE;
#جے_تعریف_شدہ FEAT_GUI
    صحيح		did_one = FALSE;
    صحيح		did_undraw = FALSE;
    صحيح		gui_cursor_خانہ = 0;
    صحيح		gui_cursor_صف = 0;
#جے_انت
    صحيح		no_update = FALSE;
    صحيح		save_pum_will_redraw = pum_will_redraw;
#جے_تعریف_شدہ FEAT_PROP_POPUP
    صحيح		did_redraw_کھڑکی = FALSE;
#جے_انت
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    إذا (!screen_valid(TRUE))
	أرجع FAIL;

    إذا (type == UPD_VALID_NO_UPDATE)
    {
	no_update = TRUE;
	type = 0;
    }

#جے_تعریف_شدہ FEAT_EVAL
    {
	buf_T *buf;

	// Before updating the screen, notify any listeners of changed text.
	FOR_ALL_BUFFERS(buf)
	    invoke_listeners(buf);
    }
#جے_انت

#جے_تعریف_شدہ FEAT_DIFF
    // May have postponed updating diffs.
    إذا (need_diff_redraw)
	diff_redraw(TRUE);
#جے_انت

    إذا (must_redraw)
    {
	إذا (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    إذا (curwin->w_سطریں_valid == 0 && type < UPD_NOT_VALID
#جے_تعریف_شدہ FEAT_TERMINAL
	    && !رکن_do_update_کھڑکی(curwin)
#جے_انت
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    إذا (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	إذا (type > UPD_INVERTED_ALL)
	    curwin->w_سطریں_valid = 0;	// don't use w_lines[].wl_size now
	أرجع FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    صحيح hid_cursor = !sync_output_سرگرم();
    إذا (hid_cursor)
	cursor_off();
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_ENABLE);

#جے_تعریف_شدہ FEAT_PROP_POPUP
    // Update popup_mask if needed.  This may set w_redraw_top and w_redraw_bot
    // in some windows.
    may_update_popup_نقاب(type);
#جے_انت

#جے_تعریف_شدہ FEAT_SYN_HL
    ++display_tick;	    // let syntax code know we're in a next round of
			    // display updating
#جے_انت
    إذا (no_update)
	++no_win_do_سطریں_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    إذا (پیغام_scrolled)
    {
	clear_cmdline = TRUE;
	إذا (type != UPD_CLEAR)
	{
	    إذا (پیغام_scrolled > صفاں - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    وإلا
	    {
		check_for_delay(FALSE);
		إذا (screen_ins_سطریں(0, 0, پیغام_scrolled, (صحيح)صفاں, 0, عدم)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    إذا (wp->w_winrow < پیغام_scrolled)
		    {
			إذا (W_WINROW(wp) + wp->w_اچائی > پیغام_scrolled
				&& wp->w_redr_type < UPD_REDRAW_چوٹی
				&& wp->w_سطریں_valid > 0
				&& wp->w_topline == wp->w_سطریں[0].wl_lnum)
			{
			    wp->w_upd_صفاں = پیغام_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_چوٹی;
			}
			وإلا
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    إذا (W_WINROW(wp) + wp->w_اچائی
					 + wp->w_حالت_اچائی <= پیغام_scrolled)
				wp->w_redr_حالت = true;
			}
		    }
		}
		إذا (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#إذا defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#جے_انت
	}
	پیغام_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    إذا (need_highlight_بدلیا)
	highlight_بدلیا();

    إذا (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    إذا (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#جے_تعریف_شدہ FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    إذا (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? نمبر_چوڑائی(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#جے_انت

#جے_تعریف_شدہ FEAT_EVAL
    invoke_redraw_listener_شروع_or_سرا(true);
#جے_انت

    // Only start redrawing if there is really something to do.
    إذا (type == UPD_INVERTED)
	update_curswant();
    إذا (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_سطریں[0].wl_valid
#جے_تعریف_شدہ FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#جے_انت
		    && curwin->w_topline == curwin->w_سطریں[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_سرگرم
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_منوال == VIsual_منوال
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    إذا (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#إذا defined(FEAT_TABPANEL)
    إذا (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#جے_انت

#جے_تعریف_شدہ FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_بفر->b_mod_سیٹ)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    لكل (wwp = firstwin; wwp != wp; wwp = wwp->w_اگلا)
		إذا (wwp->w_بفر == wp->w_بفر)
		    اكسر;
	    إذا (wwp == wp && syntax_present(wp))
		syn_ڈھیر_apply_changes(wp->w_بفر);
	}
    }
#جے_انت

    إذا (pum_redraw_وچ_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#إذا defined(FEAT_تلاش_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_کھڑکی = FALSE;
#جے_انت
#جے_تعریف_شدہ FEAT_تلاش_EXTRA
    screen_تلاش_hl.rm.regprog = عدم;
#جے_انت
    FOR_ALL_WINDOWS(wp)
    {
	override_success = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

	إذا (wp->w_redr_type != 0)
	{
	    cursor_off();
#جے_تعریف_شدہ FEAT_PROP_POPUP
	    did_redraw_کھڑکی = TRUE;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
	    إذا (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		إذا (gui.وچ_use && (wp == curwin || cursor_is_sleeping()))
		{
		    gui_cursor_خانہ = gui.cursor_خانہ;
		    gui_cursor_صف = gui.cursor_صف;
		    gui_undraw_cursor();
		    did_undraw = TRUE;
		}
	    }
#جے_انت
	    win_update(wp);
	}

	// redraw status line after the window to minimize cursor movement
	إذا (wp->w_redr_حالت)
	{
	    cursor_off();
	    win_redr_حالت(wp, TRUE); // any popup menu will be redrawn below
	}

	إذا (override_success)
	    نکالو_highlight_overrides();

    }
#إذا defined(FEAT_تلاش_EXTRA)
    سرا_تلاش_hl();
#جے_انت

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    إذا (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_vsep_چوڑائی > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_بفر->b_mod_سیٹ = false;

#جے_تعریف_شدہ FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    إذا (did_redraw_کھڑکی || popup_need_redraw())
	update_popups(win_update);
#جے_انت

#جے_تعریف_شدہ FEAT_TERMINAL
    FOR_ALL_WINDOWS(wp)
	// If this window contains a terminal, after redrawing all windows, the
	// dirty row range can be reset.
	رکن_did_update_کھڑکی(wp);
#جے_انت

    after_updating_screen(TRUE);

    // Clear or redraw the command line.  Done last, because scrolling may
    // mess up the command line.
    إذا (clear_cmdline || redraw_cmdline || redraw_منوال)
	showmode();

    إذا (no_update)
	--no_win_do_سطریں_ins;

    // May put up an introductory message when not editing a file
    إذا (!did_intro)
	maybe_intro_سنیہا();
    did_intro = TRUE;

#جے_تعریف_شدہ FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    إذا (gui.وچ_use)
    {
	إذا (did_undraw && !gui_mch_is_blink_off())
	{
	    mch_disable_flush();
	    باہرلا_flush();	// required before updating the cursor
	    mch_enable_flush();

	    // Put the GUI position where the cursor was, gui_update_cursor()
	    // uses that.
	    gui.خانہ = gui_cursor_خانہ;
	    gui.صف = gui_cursor_صف;
	    gui.خانہ = mb_fix_خانہ(gui.خانہ, gui.صف);
	    gui_update_cursor(FALSE, FALSE);
	    gui_may_flush();
	    screen_cur_خانہ = gui.خانہ;
	    screen_cur_صف = gui.صف;
	}
	وإلا
	    باہرلا_flush();
	gui_update_scrollbars(FALSE);
    }
#جے_انت

#جے_تعریف_شدہ FEAT_IMAGE
    // Make sure to do this last!
    draw_image_placements();
#جے_انت

#جے_تعریف_شدہ FEAT_EVAL
    invoke_redraw_listener_شروع_or_سرا(false);
    redraw_listener_cleanup();
#جے_انت

    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_DISABLE);
    إذا (hid_cursor)
	cursor_on();

    أرجع OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    صحيح
statusline_صف(win_T *wp)
{
#إذا defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    إذا (wp->w_چوکھٹا->fr_اچائی == wp->w_حالت_اچائی && !popup_is_popup(wp))
	أرجع wp->w_winrow;
#جے_انت
    أرجع W_WINROW(wp) + wp->w_اچائی;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    فراغ
win_redr_حالت(win_T *wp, صحيح ignore_pum بن_ورتیا)
{
    صحيح		صف;
    صحيح		fillchar;
    صحيح		attr;
    صحيح		i;
    ساكن صحيح  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    إذا (busy)
	أرجع;
    busy = TRUE;

    صف = statusline_صف(wp);

    wp->w_redr_حالت = false;
    إذا (wp->w_حالت_اچائی == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    وإلا إذا (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_حالت = true;
    }
#جے_تعریف_شدہ FEAT_STL_OPT
    وإلا إذا (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#جے_انت
    وإلا
    {
	char_u	*p;
	صحيح	plen;
	صحيح	NameBufflen;
	صحيح	this_ru_خانہ;
	صحيح	n;			// scratch value

	fillchar = fillchar_حالت(&attr, wp);

	get_trans_bufname(wp->w_بفر);
	p = NameBuff;
	plen = (صحيح)STRLEN(p);

	إذا ((bt_help(wp->w_بفر)
#جے_تعریف_شدہ FEAT_QUICKFIX
		    || wp->w_p_pvw
#جے_انت
		    || buf_Is_بدلیا(wp->w_بفر)
		    || wp->w_بفر->b_p_ro)
		&& plen < MAXPATHL - 1)
	{
	    *(p + plen++) = ' ';	// replace NUL with space
	    *(p + plen) = NUL;		// NUL terminate the string
	}
	إذا (bt_help(wp->w_بفر))
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#جے_تعریف_شدہ FEAT_QUICKFIX
	إذا (wp->w_p_pvw)
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#جے_انت
	إذا (buf_Is_بدلیا(wp->w_بفر) && !bt_terminal(wp->w_بفر))
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", "[+]");
	إذا (wp->w_بفر->b_p_ro)
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_خانہ = ru_خانہ - (cmdline_چوڑائی - wp->w_چوڑائی);
	n = (wp->w_چوڑائی + 1) / 2;
	إذا (this_ru_خانہ < n)
	    this_ru_خانہ = n;
	إذا (this_ru_خانہ <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	وإلا إذا (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    لكل (i = 0; p[i] != NUL && plen >= this_ru_خانہ - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    إذا (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	وإلا إذا (plen > this_ru_خانہ - 1)
	{
	    p += plen - (this_ru_خانہ - 1);
	    *p = '<';
	    plen = this_ru_خانہ - 1;
	}

	screen_puts(p, صف, wp->w_wincol, attr);
	screen_fill(صف, صف + 1, plen + wp->w_wincol,
		    this_ru_خانہ + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	لكل (i = 1; i < wp->w_حالت_اچائی; i++)
	    screen_fill(صف + i, صف + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	إذا ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_خانہ - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, صف, (صحيح)(this_ru_خانہ - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	إذا (p_sc && *p_sloc == 's')
	{
	    n = this_ru_خانہ - plen - 2;		    // perform the calculation here so we only do it once
	    صحيح	چوڑائی = MIN(10, n);

	    إذا (چوڑائی > 0)
		screen_puts_len(showcmd_buf, چوڑائی, صف,
				wp->w_wincol + this_ru_خانہ - چوڑائی - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    إذا (wp->w_vsep_چوڑائی != 0 && wp->w_حالت_اچائی != 0 && redrawing())
    {
	لكل (i = 0; i < wp->w_حالت_اچائی; i++)
	{
	    صحيح r = صف + i;
	    إذا (stl_connected(wp))
		fillchar = fillchar_حالت(&attr, wp);
	    وإلا
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
    ساكن فراغ
borrow_stl_vsep_hl(فراغ)
{
    win_T   *بایاں = عدم;
    win_T   *دایاں = عدم;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    إذا (!redrawing() || Screen_سطریں == عدم)
	أرجع;

    FOR_ALL_WINDOWS(بایاں)
    {
	إذا (بایاں->w_حالت_اچائی == 0 || بایاں->w_vsep_چوڑائی == 0)
	    استمر;
	// A window of zero width has no status line cell to borrow from.
	إذا (بایاں->w_چوڑائی == 0)
	    استمر;
	إذا (!stl_connected(بایاں))
	    استمر;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = عدم;
	صحيح شروع = 0;
	صحيح سرا = 0;

	FOR_ALL_WINDOWS(دایاں)
	{
	    إذا (دایاں == بایاں || دایاں->w_حالت_اچائی == 0)
		استمر;
	    إذا (دایاں->w_wincol != W_ENDCOL(بایاں) + 1)
		استمر;
	    صحيح l_stl_صف = W_WINROW(بایاں) + بایاں->w_اچائی;
	    صحيح r_stl_صف = W_WINROW(دایاں) + دایاں->w_اچائی;

	    شروع = l_stl_صف > r_stl_صف ? l_stl_صف : r_stl_صف;
	    سرا = l_stl_صف + بایاں->w_حالت_اچائی
				< r_stl_صف + دایاں->w_حالت_اچائی
		? l_stl_صف + بایاں->w_حالت_اچائی
		: r_stl_صف + دایاں->w_حالت_اچائی;
	    إذا (شروع < سرا)
	    {
		neighbour = دایاں;
		اكسر;
	    }
	}
	إذا (neighbour == عدم)
	    استمر;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	صحيح	hl;
	إذا (بایاں != curwin && neighbour != curwin
		&& fillchar_حالت(&hl, بایاں) != ' ')
	    استمر;

	// Source: prefer curwin's side; otherwise left window's right edge.
	صحيح dst_خانہ = W_ENDCOL(بایاں);
	صحيح src_خانہ = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(بایاں) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	إذا (dst_خانہ >= screen_Columns || src_خانہ >= screen_Columns)
	    استمر;
	إذا (سرا > screen_صفاں)
	    سرا = screen_صفاں;

	لكل (صحيح r = شروع; r < سرا; r++)
	{
	    بنانشانی	dst_off = سطر_وتھ[r] + dst_خانہ;
	    sattr_T	attr = ScreenAttrs[سطر_وتھ[r] + src_خانہ];

	    إذا (ScreenAttrs[dst_off] != attr)
	    {
		ScreenAttrs[dst_off] = attr;
		screen_char(dst_off, r, dst_خانہ);
	    }
	}
    }
}

#جے_تعریف_شدہ FEAT_STL_OPT
/*
 * Redraw the status line according to 'statusline' and take care of any
 * errors encountered.
 */
    ساكن فراغ
redraw_custom_statusline(win_T *wp)
{
    ساكن صحيح	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    إذا (entered)
	أرجع;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#جے_انت

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    فراغ
showruler(صحيح always)
{
    إذا (!always && !redrawing())
	أرجع;
    إذا (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_حالت = true;
	أرجع;
    }
#إذا defined(FEAT_STL_OPT)
    إذا ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_حالت_اچائی)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    وإلا
#جے_انت
	win_redr_ruler(curwin, always, FALSE);

    إذا (need_maketitle
#جے_تعریف_شدہ FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_وچ_ICON))
	    || (p_title && (stl_syntax & STL_وچ_TITLE))
#جے_انت
       )
	maketitle();

    // Redraw the tab pages line if needed.
    إذا (redraw_tabline)
	draw_tabline();

#إذا defined(FEAT_TABPANEL)
    إذا (redraw_tabpanel)
	draw_tabpanel();
#جے_انت
}

    فراغ
win_redr_ruler(win_T *wp, صحيح always, صحيح ignore_pum)
{
    صحيح	خالی_تھاواں_سطر = FALSE;

    // If 'ruler' off don't do anything
    إذا (!p_ru)
	أرجع;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    إذا (wp->w_cursor.lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
	أرجع;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    إذا (wp == lastwin && lastwin->w_حالت_اچائی == 0)
	إذا (edit_submode != عدم)
	    أرجع;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    إذا (!ignore_pum && pum_visible())
	أرجع;

#جے_تعریف_شدہ FEAT_STL_OPT
    إذا (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	أرجع;
    }
#جے_انت

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    إذا ((State & منوال_داخل_کرو) == 0
		&& *ml_get_buf(wp->w_بفر, wp->w_cursor.lnum, FALSE) == NUL)
	خالی_تھاواں_سطر = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    إذا (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.خانہ != wp->w_ru_cursor.خانہ
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_بفر->b_ml.ml_سطر_گنتی != wp->w_ru_سطر_گنتی
#جے_تعریف_شدہ FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#جے_انت
	    || خالی_تھاواں_سطر != wp->w_ru_خالی_تھاواں)
    {
	صحيح	صف;
	صحيح	fillchar;
	صحيح	attr;
	صحيح	off;
	صحيح	چوڑائی;
	colnr_T	virtcol;
#تعریف_کرو RULER_BUF_LEN 70
	char_u	بفر[RULER_BUF_LEN];
	صحيح	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	صحيح	rel_poslen;
	صحيح	this_ru_خانہ;
	صحيح	n1;			    // scratch value
	صحيح	n2;			    // scratch value
	bool	override_success =
	    ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	إذا (wp->w_حالت_اچائی)
	{
	    صف = statusline_صف(wp);
	    fillchar = fillchar_حالت(&attr, wp);
	    off = wp->w_wincol;
	    چوڑائی = wp->w_چوڑائی;
	}
	وإلا
	{
	    صف = صفاں - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_خانہ_off;
	    چوڑائی = cmdline_چوڑائی;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	إذا (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, عدم, &virtcol, عدم, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((حرف *)بفر, RULER_BUF_LEN, _("%ld,"),
		(wp->w_بفر->b_ml.ml_flags & ML_خالی_تھاواں)
		    ? 0L
		    : (لما)(wp->w_cursor.lnum));
	bufferlen += خانہ_print(بفر + bufferlen, RULER_BUF_LEN - bufferlen,
			خالی_تھاواں_سطر ? 0 : (صحيح)wp->w_cursor.خانہ + 1,
			(صحيح)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	إذا (wp->w_حالت_اچائی == 0)	// can't use last char of screen
	    ++n1;

	this_ru_خانہ = ru_خانہ - (cmdline_چوڑائی - چوڑائی);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (چوڑائی + 1) / 2;
	إذا (this_ru_خانہ < n2)
	    this_ru_خانہ = n2;
	إذا (this_ru_خانہ + n1 < چوڑائی)
	{
	    // need at least space for rel_pos + NUL
	    طالما (this_ru_خانہ + n1 < چوڑائی
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		إذا (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, بفر + bufferlen);
		وإلا
		    بفر[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((حرف *)بفر + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	إذا (has_mbyte)
	{
	    لكل (n1 = 0, n2 = 0; بفر[n1] != NUL; n1 += (*mb_ptr2len)(بفر + n1))
	    {
		n2 += (*mb_ptr2cells)(بفر + n1);
		إذا (this_ru_خانہ + n2 > چوڑائی)
		{
		    bufferlen = n1;
		    بفر[bufferlen] = NUL;
		    اكسر;
		}
	    }
	}
	وإلا إذا (this_ru_خانہ + bufferlen > چوڑائی)
	{
	    bufferlen = چوڑائی - this_ru_خانہ;
	    بفر[bufferlen] = NUL;
	}

	screen_puts(بفر, صف, this_ru_خانہ + off, attr);
	n1 = redraw_cmdline;
	screen_fill(صف, صف + 1,
		this_ru_خانہ + off + bufferlen,
		(off + چوڑائی),
		fillchar, fillchar, attr);
	// don't redraw the cmdline because of showing the ruler
	redraw_cmdline = n1;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_خالی_تھاواں = خالی_تھاواں_سطر;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_سطر_گنتی = wp->w_بفر->b_ml.ml_سطر_گنتی;
#جے_تعریف_شدہ FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#جے_انت

	إذا (override_success)
	    نکالو_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    فراغ
after_updating_screen(صحيح may_resize_shell بن_ورتیا)
{
    updating_screen = FALSE;
#جے_تعریف_شدہ FEAT_GUI
    إذا (may_resize_shell)
	gui_may_resize_shell();
#جے_انت
#جے_تعریف_شدہ FEAT_TERMINAL
    رکن_check_channel_closed_recently();
#جے_انت

#جے_تعریف_شدہ HAVE_DROP_FILE
    // If handle_drop() was called while updating_screen was TRUE need to
    // handle the drop now.
    handle_any_postponed_drop();
#جے_انت
}

/*
 * Update all windows that are editing the current buffer.
 */
    فراغ
update_curbuf(صحيح type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#إذا defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    ساكن صحيح
متن_منزل_screenline(win_T *wp, char_u *متن, صحيح خانہ)
{
    صحيح		off = (صحيح)(current_Screen_سطر - Screen_سطریں);

    إذا (has_mbyte)
    {
	صحيح	cells;
	صحيح	u8c, u8cc[MAX_MCO];
	صحيح	i;
	صحيح	idx;
	صحيح	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	صحيح	prev_c = 0;		// previous Arabic character
	صحيح	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	إذا (wp->w_p_rl)
	    idx = off;
	وإلا
# endif
	    idx = off + خانہ;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	لكل (p = متن; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    إذا (خانہ + cells > wp->w_چوڑائی)
		اكسر;
# ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		idx = off + wp->w_چوڑائی - خانہ - cells;
# endif
	    Screen_سطریں[idx] = *p;
	    إذا (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		إذا (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_سطریں_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    prev_c = u8c;
# endif
		}
		وإلا
		{
# ifdef FEAT_ARABIC
		    إذا (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			صحيح	pc, pc1, nc;
			صحيح	pcc[MAX_MCO];
			صحيح	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			إذا (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			وإلا
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			Screen_سطریں[idx] = firstbyte;
		    }
		    وإلا
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_سطریں_UC[idx] = u8c;
		    لكل (i = 0; i < Screen_mco; ++i)
		    {
			Screen_سطریں_C[i][idx] = u8cc[i];
			إذا (u8cc[i] == 0)
			    اكسر;
		    }
		}
		إذا (cells > 1)
		    Screen_سطریں[idx + 1] = 0;
	    }
	    وإلا إذا (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    وإلا إذا (cells > 1)
		// double-width character
		Screen_سطریں[idx + 1] = p[1];
	    خانہ += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    وإلا
    {
	صحيح len = (صحيح)STRLEN(متن);
	صحيح n = wp->w_چوڑائی - خانہ;

	إذا (len > n)
	    len = n;
	إذا (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		mch_memmove(current_Screen_سطر, متن, len);
	    وإلا
# endif
		mch_memmove(current_Screen_سطر + خانہ, متن, len);
	    خانہ += len;
	}
    }
    أرجع خانہ;
}
#جے_انت

#جے_تعریف_شدہ FEAT_MENU
/*
 * Draw the window toolbar.
 */
    ساكن فراغ
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    صحيح		شے_idx = 0;
    صحيح		شے_گنتی = 0;
    صحيح		خانہ = 0;
    صحيح		اگلا_خانہ;
    صحيح		off = (صحيح)(current_Screen_سطر - Screen_سطریں);
    bool	override_success =
	ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);
    صحيح		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    صحيح		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_اشیاء);
    FOR_ALL_بچہ_MENUS(wp->w_winbar, menu)
	++شے_گنتی;
    wp->w_winbar_اشیاء = ALLOC_CLEAR_MULT(winbar_شے_T, شے_گنتی + 1);

    // TODO: use fewer spaces if there is not enough room
    لكل (menu = wp->w_winbar->children;
			  menu != عدم && خانہ < wp->w_چوڑائی; menu = menu->اگلا)
    {
	space_منزل_screenline(off + خانہ, fill_attr);
	إذا (++خانہ >= wp->w_چوڑائی)
	    اكسر;
	إذا (خانہ > 1)
	{
	    space_منزل_screenline(off + خانہ, fill_attr);
	    إذا (++خانہ >= wp->w_چوڑائی)
		اكسر;
	}

	wp->w_winbar_اشیاء[شے_idx].wb_startcol = خانہ;
	space_منزل_screenline(off + خانہ, button_attr);
	إذا (++خانہ >= wp->w_چوڑائی)
	    اكسر;

	اگلا_خانہ = متن_منزل_screenline(wp, menu->name, خانہ);
	طالما (خانہ < اگلا_خانہ)
	{
	    ScreenAttrs[off + خانہ] = button_attr;
	    ++خانہ;
	}
	wp->w_winbar_اشیاء[شے_idx].wb_endcol = خانہ;
	wp->w_winbar_اشیاء[شے_idx].wb_menu = menu;
	++شے_idx;

	إذا (خانہ >= wp->w_چوڑائی)
	    اكسر;
	space_منزل_screenline(off + خانہ, button_attr);
	++خانہ;
    }
    طالما (خانہ < wp->w_چوڑائی)
    {
	space_منزل_screenline(off + خانہ, fill_attr);
	++خانہ;
    }
    wp->w_winbar_اشیاء[شے_idx].wb_menu = عدم; // end marker

    screen_سطر(wp, wp->w_winrow, wp->w_wincol, wp->w_چوڑائی,
							  wp->w_چوڑائی, -1, 0);

    إذا (override_success)
	نکالو_highlight_overrides();
}
#جے_انت

#إذا defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    ساكن فراغ
copy_متن_attr(
    صحيح		off,
    char_u	*buf,
    صحيح		len,
    صحيح		attr)
{
    صحيح		i;

    mch_memmove(Screen_سطریں + off, buf, (size_t)len);
    إذا (enc_utf8)
	vim_memset(Screen_سطریں_UC + off, 0, حجم(u8char_T) * (size_t)len);
    لكل (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    ساكن فراغ
fold_سطر(
    win_T	*wp,
    لما	fold_گنتی,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    صحيح		صف)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*چوٹی, *bot;
    linenr_T	lnume = lnum + fold_گنتی - 1;
    صحيح		len;
    char_u	*متن;
    صحيح		fdc;
    صحيح		خانہ;
    صحيح		txtcol;
    صحيح		off = (صحيح)(current_Screen_سطر - Screen_سطریں);
    صحيح		ri;

    // Build the fold line:
    // 1. Add the cmdwin_type for the command-line window
    // 2. Add the 'foldcolumn'
    // 3. Add the 'number' or 'relativenumber' column
    // 4. Compose the text
    // 5. Add the text
    // 6. set highlighting for the Visual area an other text
    خانہ = 0;

    // 1. Add the cmdwin_type for the command-line window
    // Ignores 'rightleft', this window is never right-left.
    إذا (wp == cmdwin_win)
    {
	Screen_سطریں[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	إذا (enc_utf8)
	    Screen_سطریں_UC[off] = 0;
	++خانہ;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    افعل { \
	إذا (wp->w_p_rl) \
	    لكل (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_چوڑائی - (p) - (l)) + ri] = v; \
	 وإلا \
	    لكل (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } طالما (0)
# وإلا
#  define RL_MEMSET(p, v, l) \
    افعل { \
	لكل (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } طالما (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, خانہ);
    إذا (fdc > 0)
    {
	char_u	*p;
	صحيح	i;
	صحيح	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	لكل (i = 0; i < fdc; i++)
	{
	    صحيح		ch;

	    إذا (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    وإلا
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		idx = off + wp->w_چوڑائی - i - 1 - خانہ;
	    وإلا
# endif
		idx = off + خانہ + i;
	    إذا (enc_utf8)
	    {
		إذا (ch >= 0x80)
		{
		    Screen_سطریں_UC[idx] = ch;
		    Screen_سطریں_C[0][idx] = 0;
		    Screen_سطریں[idx] = 0x80;
		}
		وإلا
		{
		    Screen_سطریں[idx] = ch;
		    Screen_سطریں_UC[idx] = 0;
		}
	    }
	    وإلا
		Screen_سطریں[idx] = ch;
	}

	RL_MEMSET(خانہ, HL_ATTR(HLF_FC), fdc);
	خانہ += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    //متن
    RL_MEMSET(خانہ, HL_ATTR(HLF_FL), wp->w_چوڑائی - خانہ);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    إذا (signcolumn_on(wp))
    {
	len = wp->w_چوڑائی - خانہ;
	إذا (len > 0)
	{
	    إذا (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		// the line number isn't reversed
		copy_متن_attr(off + wp->w_چوڑائی - len - خانہ,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    وإلا
#  endif
		copy_متن_attr(off + خانہ, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    خانہ += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    إذا (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_چوڑائی - خانہ;
	إذا (len > 0)
	{
	    صحيح	    w = نمبر_چوڑائی(wp);
	    لما    عدد;
	    حرف    *fmt = "%*ld ";

	    إذا (len > w + 1)
		len = w + 1;

	    إذا (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		عدد = (لما)lnum;
	    وإلا
	    {
		// 'relativenumber', don't use negative numbers
		عدد = labs((لما)get_cursor_rel_lnum(wp, lnum));
		إذا (عدد == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    عدد = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((حرف *)buf, حجم(buf), fmt, w, عدد);
# ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		// the line number isn't reversed
		copy_متن_attr(off + wp->w_چوڑائی - len - خانہ, buf, len,
							     HL_ATTR(HLF_FL));
	    وإلا
# endif
		copy_متن_attr(off + خانہ, buf, len, HL_ATTR(HLF_FL));
	    خانہ += len;
	}
    }

    // 4. Compose the folded-line string with 'foldtext', if set.
    متن = get_foldtext(wp, lnum, lnume, foldinfo, buf);

    txtcol = خانہ;	// remember where text starts

    // 5. move the text to current_ScreenLine.  Fill up with "fold" from
    //    'fillchars'.
    //    Right-left text is put in columns 0 - number-col, normal text is put
    //    in columns number-col - window-width.
    خانہ = متن_منزل_screenline(wp, متن, خانہ);

    // Fill the rest of the line with the fold filler
    طالما (خانہ < wp->w_چوڑائی)
    {
	صحيح c = wp->w_fill_chars.fold;
	صحيح idx = off + خانہ;

# ifdef FEAT_RIGHTLEFT
	إذا (wp->w_p_rl)
	    idx = off + wp->w_چوڑائی - 1 - خانہ;
# endif

	إذا (enc_utf8)
	{
	    إذا (c >= 0x80)
	    {
		Screen_سطریں_UC[idx] = c;
		Screen_سطریں_C[0][idx] = 0;
		Screen_سطریں[idx] = 0x80; // avoid storing zero
	    }
	    وإلا
	    {
		Screen_سطریں_UC[idx] = 0;
		Screen_سطریں[idx] = c;
	    }
	}
	وإلا
	    Screen_سطریں[idx] = c;
	++خانہ;
    }

    إذا (متن != buf)
	vim_free(متن);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    إذا (VIsual_سرگرم && wp->w_بفر == curwin->w_بفر)
    {
	إذا (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    چوٹی = &curwin->w_cursor;
	    bot = &VIsual;
	}
	وإلا
	{
	    // Visual is before curwin->w_cursor
	    چوٹی = &VIsual;
	    bot = &curwin->w_cursor;
	}
	إذا (lnum >= چوٹی->lnum
		&& lnume <= bot->lnum
		&& (VIsual_منوال != 'v'
		    || ((lnum > چوٹی->lnum
			    || (lnum == چوٹی->lnum
				&& چوٹی->خانہ == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->خانہ - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_بفر, lnume))))))
	{
	    إذا (VIsual_منوال == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		إذا (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_چوڑائی)
		{
		    إذا (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_چوڑائی)
			len = wp->w_old_cursor_lcol;
		    وإلا
			len = wp->w_چوڑائی - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (صحيح)wp->w_old_cursor_fcol);
		}
	    }
	    وإلا
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_چوڑائی - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    إذا (wp->w_p_cc_خانے_گنتی)
    {
	صحيح i = 0;
	صحيح j = wp->w_p_cc_خانے_گنتی[i];
	صحيح old_txtcol = txtcol;

	طالما (j > -1)
	{
	    txtcol += j;
	    إذا (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    وإلا
		txtcol -= wp->w_leftcol;
	    إذا (txtcol >= 0 && txtcol < wp->w_چوڑائی)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_خانے_گنتی[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    إذا (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	إذا (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	وإلا
	    txtcol -= wp->w_leftcol;
	إذا (txtcol >= 0 && txtcol < wp->w_چوڑائی)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_سطر(wp, صف + W_WINROW(wp), wp->w_wincol, wp->w_چوڑائی, wp->w_چوڑائی,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    إذا (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_صف = صف;
	curwin->w_cline_اچائی = 1;
	curwin->w_cline_folded = true;
	curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
    }

# ifdef FEAT_CONCEAL
    // When the line was not folded w_wrow may have been set, recompute it.
    إذا (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_سطر(wp))
	curs_columns(TRUE);
# endif
}
#جے_انت

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
    ساكن فراغ
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_بفر;
    صحيح		type;
    صحيح		چوٹی_سرا = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    صحيح		mid_شروع = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    صحيح		mid_سرا = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    صحيح		bot_شروع = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    صحيح		scrolled_تھلے = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    صحيح		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //سطریں
#جے_تعریف_شدہ FEAT_تلاش_EXTRA
    صحيح		چوٹی_منزل_mod = FALSE;    // redraw above mod_top
#جے_انت

    صحيح		صف;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    صحيح		idx;		// current index in w_lines[]
    صحيح		srow;		// starting row of the current line

    صحيح		eof = FALSE;	// if TRUE, we hit the end of the file
    صحيح		didline = FALSE; // if TRUE, we finished the last line
    صحيح		i;
    لما	j;
    ساكن صحيح	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#جے_تعریف_شدہ FEAT_CONCEAL
    صحيح		old_wrow = wp->w_wrow;
    صحيح		old_wcol = wp->w_wcol;
#جے_انت
#جے_تعریف_شدہ FEAT_FOLDING
    لما	fold_گنتی;
#جے_انت
#جے_تعریف_شدہ FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_سطر 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    صحيح		did_update = DID_NONE;
    linenr_T	syntax_آخری_parsed = 0;		// last parsed text line
#جے_انت
    linenr_T	mod_چوٹی = 0;
    linenr_T	mod_bot = 0;
#إذا defined(FEAT_SYN_HL) || defined(FEAT_تلاش_EXTRA)
    صحيح		save_got_int;
#جے_انت
    bool	override_success;

#إذا defined(FEAT_تلاش_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    إذا (!did_update_one_کھڑکی)
    {
	did_update_one_کھڑکی = TRUE;
# ifdef FEAT_تلاش_EXTRA
	شروع_تلاش_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	إذا (clip_تارا.دستیاب && clip_isautosel_تارا())
	    clip_update_selection(&clip_تارا);
	إذا (clip_plus.دستیاب && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#جے_انت

#جے_تعریف_شدہ FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    إذا (wp->w_p_cuc)
	validate_virtcol_win(wp);
#جے_انت

    type = wp->w_redr_type;

    إذا (type == UPD_NOT_VALID)
    {
	wp->w_redr_حالت = true;
	wp->w_سطریں_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    إذا (wp->w_اچائی + WINBAR_اچائی(wp) == 0
	    || (wp->w_چوکھٹا->fr_اچائی == wp->w_حالت_اچائی
#إذا defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#جے_انت
	       ))
    {
	wp->w_redr_type = 0;
	أرجع;
    }

    // Window is zero-width: Only need to draw the separator.
    إذا (wp->w_چوڑائی == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	أرجع;
    }

    override_success = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);


#جے_تعریف_شدہ FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    إذا (رکن_do_update_کھڑکی(wp))
    {
	رکن_update_کھڑکی(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	إذا (winbar_اچائی(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	إذا (override_success)
	    نکالو_highlight_overrides();
	أرجع;
    }
#جے_انت

#جے_تعریف_شدہ FEAT_تلاش_EXTRA
    init_تلاش_hl(wp, &screen_تلاش_hl);
#جے_انت

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    إذا (wp->w_skipcol > 0 && wp->w_چوڑائی > win_خانہ_off(wp))
    {
	صحيح w = 0;
	صحيح width1 = wp->w_چوڑائی - win_خانہ_off(wp);
	صحيح width2 = width1 + win_خانہ_off2(wp);
	صحيح جوڑو = width1;

	طالما (w < wp->w_skipcol)
	{
	    إذا (w > 0)
		جوڑو = width2;
	    w += جوڑو;
	}
	إذا (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - جوڑو;
    }

#جے_تعریف_شدہ FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? نمبر_چوڑائی(wp) : 0;
    إذا (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    وإلا
#جے_انت

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_چوٹی = wp->w_redraw_چوٹی;
	إذا (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	وإلا
	    mod_bot = 0;
	إذا (buf->b_mod_سیٹ)
	{
	    إذا (mod_چوٹی == 0 || mod_چوٹی > buf->b_mod_چوٹی)
	    {
		mod_چوٹی = buf->b_mod_چوٹی;
#جے_تعریف_شدہ FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		إذا (syntax_present(wp))
		{
		    mod_چوٹی -= buf->b_s.b_syn_sync_linebreaks;
		    إذا (mod_چوٹی < 1)
			mod_چوٹی = 1;
		}
#جے_انت
	    }
	    إذا (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#جے_تعریف_شدہ FEAT_تلاش_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    إذا (screen_تلاش_hl.rm.regprog != عدم
		    && re_multiline(screen_تلاش_hl.rm.regprog))
		چوٹی_منزل_mod = TRUE;
	    وإلا
	    {
		matchitem_T *cur = wp->w_میل_سر;

		طالما (cur != عدم)
		{
		    إذا (cur->mit_میل.regprog != عدم
				       && re_multiline(cur->mit_میل.regprog))
		    {
			چوٹی_منزل_mod = TRUE;
			اكسر;
		    }
		    cur = cur->mit_اگلا;
		}
	    }
#جے_انت
	}

#جے_تعریف_شدہ FEAT_تلاش_EXTRA
	إذا (تلاش_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    إذا (mod_چوٹی == 0 || mod_چوٹی > تلاش_hl_has_cursor_lnum)
		mod_چوٹی = تلاش_hl_has_cursor_lnum;
	    إذا (mod_bot == 0 || mod_bot < تلاش_hl_has_cursor_lnum + 1)
		mod_bot = تلاش_hl_has_cursor_lnum + 1;
	}
#جے_انت

#جے_تعریف_شدہ FEAT_FOLDING
	إذا (mod_چوٹی != 0 && hasAnyFolding(wp))
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
	    لكل (i = 0; i < wp->w_سطریں_valid; ++i)
		إذا (wp->w_سطریں[i].wl_valid)
		{
		    إذا (wp->w_سطریں[i].wl_lastlnum < mod_چوٹی)
			lnumt = wp->w_سطریں[i].wl_lastlnum + 1;
		    إذا (lnumb == MAXLNUM && wp->w_سطریں[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_سطریں[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			إذا (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (فراغ)hasFoldingWin(wp, mod_چوٹی, &mod_چوٹی, عدم, TRUE, عدم);
	    إذا (mod_چوٹی > lnumt)
		mod_چوٹی = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (فراغ)hasFoldingWin(wp, mod_bot, عدم, &mod_bot, TRUE, عدم);
	    ++mod_bot;
	    إذا (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#جے_انت

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	إذا (mod_چوٹی != 0 && mod_چوٹی < wp->w_topline)
	{
	    إذا (mod_bot > wp->w_topline)
		mod_چوٹی = wp->w_topline;
#جے_تعریف_شدہ FEAT_SYN_HL
	    وإلا إذا (syntax_present(wp))
		چوٹی_سرا = 1;
#جے_انت
	}
    }
    wp->w_redraw_چوٹی = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#جے_تعریف_شدہ FEAT_تلاش_EXTRA
    تلاش_hl_has_cursor_lnum = 0;
#جے_انت


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    إذا (type == UPD_REDRAW_چوٹی)
    {
	j = 0;
	لكل (i = 0; i < wp->w_سطریں_valid; ++i)
	{
	    j += wp->w_سطریں[i].wl_ناپ;
	    إذا (j >= wp->w_upd_صفاں)
	    {
		چوٹی_سرا = j;
		اكسر;
	    }
	}
	إذا (چوٹی_سرا == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	وإلا
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    إذا (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    إذا ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#جے_تعریف_شدہ FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#جے_انت
	    )
    {
	إذا (mod_چوٹی != 0
		&& wp->w_topline == mod_چوٹی
		&& (!wp->w_سطریں[0].wl_valid
		    || wp->w_topline == wp->w_سطریں[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	وإلا إذا (wp->w_سطریں[0].wl_valid
		&& (wp->w_topline < wp->w_سطریں[0].wl_lnum
#جے_تعریف_شدہ FEAT_DIFF
		    || (wp->w_topline == wp->w_سطریں[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#جے_انت
		   ))
	{
	    // New topline is above old topline: May scroll down.
#جے_تعریف_شدہ FEAT_FOLDING
	    إذا (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		لكل (ln = wp->w_topline; ln < wp->w_سطریں[0].wl_lnum; ++ln)
		{
		    ++j;
		    إذا (j >= wp->w_اچائی - 2)
			اكسر;
		    (فراغ)hasFoldingWin(wp, ln, عدم, &ln, TRUE, عدم);
		}
	    }
	    وإلا
#جے_انت
		j = wp->w_سطریں[0].wl_lnum - wp->w_topline;
	    إذا (j < wp->w_اچائی - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_سطریں[0].wl_lnum - 1,
								wp->w_اچائی);
#جے_تعریف_شدہ FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		إذا (wp->w_سطریں[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_سطریں[0].wl_lnum)
							  - wp->w_old_topfill;
#جے_انت
		إذا (i < wp->w_اچائی - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    إذا (i > 0)
			check_for_delay(FALSE);
		    إذا (win_ins_سطریں(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			إذا (wp->w_سطریں_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    چوٹی_سرا = i;
			    scrolled_تھلے = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    إذا ((wp->w_سطریں_valid += j) > wp->w_اچائی)
				wp->w_سطریں_valid = wp->w_اچائی;
			    لكل (idx = wp->w_سطریں_valid; idx - j >= 0; idx--)
				wp->w_سطریں[idx] = wp->w_سطریں[idx - j];
			    طالما (idx >= 0)
				wp->w_سطریں[idx--].wl_valid = FALSE;
			}
		    }
		    وإلا
			mid_شروع = 0;		// redraw all lines
		}
		وإلا
		    mid_شروع = 0;		// redraw all lines
	    }
	    وإلا
		mid_شروع = 0;		// redraw all lines
	}
	وإلا
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    صف = 0;
	    لكل (i = 0; i < wp->w_سطریں_valid && i < صفاں; i++)
	    {
		إذا (wp->w_سطریں[i].wl_valid
			&& wp->w_سطریں[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    اكسر;
		}
		صف += wp->w_سطریں[i].wl_ناپ;
	    }
	    إذا (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//سطریں
		mid_شروع = 0;
	    }
	    وإلا
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#جے_تعریف_شدہ FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		إذا (wp->w_سطریں[0].wl_lnum == wp->w_topline)
		    صف += wp->w_old_topfill;
		وإلا
		    صف += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		صف -= wp->w_topfill;
#جے_انت
		إذا (صف > صفاں)  // just in case
		    صف = صفاں;
		إذا (صف > 0)
		{
		    check_for_delay(FALSE);
		    إذا (win_del_سطریں(wp, 0, صف, FALSE, wp == firstwin, 0)
									 == OK)
			bot_شروع = wp->w_اچائی - صف;
		    وإلا
			mid_شروع = 0;		// redraw all lines
		}
		إذا ((صف == 0 || bot_شروع < 999) && wp->w_سطریں_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_شروع = 0;
		    idx = 0;
		    لكل (;;)
		    {
			wp->w_سطریں[idx] = wp->w_سطریں[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			إذا (صف > 0 && bot_شروع + صف
				 + (صحيح)wp->w_سطریں[j].wl_ناپ > wp->w_اچائی)
			{
			    wp->w_سطریں_valid = idx + 1;
			    اكسر;
			}
			bot_شروع += wp->w_سطریں[idx++].wl_ناپ;

			// stop at the last valid entry in w_lines[].wl_size
			إذا (++j >= wp->w_سطریں_valid)
			{
			    wp->w_سطریں_valid = idx;
			    اكسر;
			}
		    }
#جے_تعریف_شدہ FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    إذا (wp->w_p_diff && bot_شروع > 0)
			wp->w_سطریں[0].wl_ناپ = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#جے_انت
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	إذا (mid_شروع == 0)
	    mid_سرا = wp->w_اچائی;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	إذا (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    وإلا
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_شروع = 0;
	mid_سرا = wp->w_اچائی;
    }

    إذا (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_شروع = 0;
	mid_سرا = wp->w_اچائی;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    إذا ((VIsual_سرگرم && buf == curwin->w_بفر)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, منزل;

	إذا (VIsual_سرگرم)
	{
	    إذا (VIsual_منوال != wp->w_old_visual_منوال
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		إذا (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    منزل = VIsual.lnum;
		}
		وإلا
		{
		    from = VIsual.lnum;
		    منزل = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		إذا (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		إذا (wp->w_old_cursor_lnum > منزل)
		    منزل = wp->w_old_cursor_lnum;
		إذا (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		إذا (wp->w_old_visual_lnum > منزل)
		    منزل = wp->w_old_visual_lnum;
	    }
	    وإلا
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		إذا (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    منزل = wp->w_old_cursor_lnum;
		}
		وإلا
		{
		    from = wp->w_old_cursor_lnum;
		    منزل = curwin->w_cursor.lnum;
		    إذا (from == 0)	// Visual mode just started
			from = منزل;
		}

		إذا (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.خانہ != wp->w_old_visual_خانہ)
		{
		    إذا (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    إذا (wp->w_old_visual_lnum > منزل)
			منزل = wp->w_old_visual_lnum;
		    إذا (VIsual.lnum < from)
			from = VIsual.lnum;
		    إذا (VIsual.lnum > منزل)
			منزل = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    إذا (VIsual_منوال == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_سرا_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		إذا (curwin->w_curswant == MAXCOL)
		{
		    إذا (get_ve_flags() & VE_بلاک)
		    {
			pos_T	    pos;
			صحيح	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			لكل (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.خانہ = (صحيح)ml_get_buf_len(wp->w_بفر, pos.lnum);
			    getvvcol(wp, &pos, عدم, عدم, &t, 0);
			    إذا (toc < t)
				toc = t;
			}
			++toc;
		    }
		    وإلا
			toc = MAXCOL;
		}

		إذا (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    إذا (from > VIsual.lnum)
			from = VIsual.lnum;
		    إذا (منزل < VIsual.lnum)
			منزل = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	وإلا
	{
	    // Use the line numbers of the old Visual area.
	    إذا (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		منزل = wp->w_old_visual_lnum;
	    }
	    وإلا
	    {
		from = wp->w_old_visual_lnum;
		منزل = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	إذا (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	إذا (wp->w_valid & VALID_BOTLINE)
	{
	    from = MIN(from, wp->w_botline);
	    منزل = MIN(منزل, wp->w_botline);
	}

	// Find the minimal part to be updated.
	// Watch out for scrolling that made entries in w_lines[] invalid.
	// E.g., CTRL-U makes the first half of w_lines[] invalid and sets
	// top_end; need to redraw from top_end to the "to" line.
	// A middle mouse click with a Visual selection may change the text
	// above the Visual area and reset wl_valid, do count these for
	// mid_end (in srow).
	إذا (mid_شروع > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    إذا (scrolled_تھلے)
		mid_شروع = چوٹی_سرا;
	    وإلا
		mid_شروع = 0;
	    طالما (lnum < from && idx < wp->w_سطریں_valid)	// find start
	    {
		إذا (wp->w_سطریں[idx].wl_valid)
		    mid_شروع += wp->w_سطریں[idx].wl_ناپ;
		وإلا إذا (!scrolled_تھلے)
		    srow += wp->w_سطریں[idx].wl_ناپ;
		++idx;
#جے_تعریف_شدہ FEAT_FOLDING
		إذا (idx < wp->w_سطریں_valid && wp->w_سطریں[idx].wl_valid)
		    lnum = wp->w_سطریں[idx].wl_lnum;
		وإلا
#جے_انت
		    ++lnum;
	    }
	    srow += mid_شروع;
	    mid_سرا = wp->w_اچائی;
	    لكل ( ; idx < wp->w_سطریں_valid; ++idx)		// find end
	    {
		إذا (wp->w_سطریں[idx].wl_valid
			&& wp->w_سطریں[idx].wl_lnum >= منزل + 1)
		{
		    // Only update until first row of this line
		    mid_سرا = srow;
		    اكسر;
		}
		srow += wp->w_سطریں[idx].wl_ناپ;
	    }
	}
    }

    إذا (VIsual_سرگرم && buf == curwin->w_بفر)
    {
	wp->w_old_visual_منوال = VIsual_منوال;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_خانہ = VIsual.خانہ;
	wp->w_old_curswant = curwin->w_curswant;
    }
    وإلا
    {
	wp->w_old_visual_منوال = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_خانہ = 0;
    }

#إذا defined(FEAT_SYN_HL) || defined(FEAT_تلاش_EXTRA)
    // reset got_int, otherwise regexp won't work
    save_got_int = got_int;
    got_int = 0;
#جے_انت
#جے_تعریف_شدہ SYN_ویلا_حد_بندی
    // Set the time limit to 'redrawtime'.
    redrawtime_حد_بندی_سیٹ = TRUE;
    init_regexp_timeout(p_rdt);
#جے_انت
#جے_تعریف_شدہ FEAT_FOLDING
    win_foldinfo.fi_level = 0;
#جے_انت

#جے_تعریف_شدہ FEAT_MENU
    // Draw the window toolbar, if there is one.
    // TODO: only when needed.
    إذا (winbar_اچائی(wp) > 0)
	redraw_win_toolbar(wp);
#جے_انت

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#جے_تعریف_شدہ FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_میدان(spv);
    إذا (spell_check_کھڑکی(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_چوٹی == 0;
    }
#جے_انت

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    صف = 0;
    srow = 0;
    لكل (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	إذا (صف == wp->w_اچائی)
	{
	    didline = TRUE;
	    اكسر;
	}

	// stop updating when hit the end of the file
	إذا (lnum > buf->b_ml.ml_سطر_گنتی)
	{
	    eof = TRUE;
	    اكسر;
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
	إذا (صف < چوٹی_سرا
		|| (صف >= mid_شروع && صف < mid_سرا)
#جے_تعریف_شدہ FEAT_تلاش_EXTRA
		|| چوٹی_منزل_mod
#جے_انت
		|| idx >= wp->w_سطریں_valid
		|| (صف + wp->w_سطریں[idx].wl_ناپ > bot_شروع)
		|| (mod_چوٹی != 0
		    && (lnum == mod_چوٹی
			|| (lnum >= mod_چوٹی
			    && (lnum < mod_bot
#جے_تعریف_شدہ FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_سطر
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_بدلیا(lnum)))
#جے_انت
#جے_تعریف_شدہ FEAT_تلاش_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_میل_سر != عدم
						    && buf->b_mod_سیٹ
						    && buf->b_mod_xlines != 0)
#جے_انت
				))))
#جے_تعریف_شدہ FEAT_SYN_HL
		|| (wp->w_p_cul && lnum == wp->w_cursor.lnum)
		|| lnum == wp->w_آخری_cursorline
#جے_انت
				)
	{
#جے_تعریف_شدہ FEAT_تلاش_EXTRA
	    إذا (lnum == mod_چوٹی)
		چوٹی_منزل_mod = FALSE;
#جے_انت

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    إذا (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_چوٹی && lnum < MAX(mod_bot, mod_چوٹی + 1)
		    && (!scrolled_تھلے || صف >= چوٹی_سرا))
	    {
		scrolled_for_mod = TRUE;

		صحيح		old_cline_اچائی = 0;
		صحيح		old_صفاں = 0;
		صحيح		new_صفاں = 0;
		صحيح		xtra_صفاں;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		لكل (i = idx; i < wp->w_سطریں_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    إذا (wp->w_سطریں[i].wl_valid
			    && wp->w_سطریں[i].wl_lnum == mod_bot)
			اكسر;
		    إذا (wp->w_سطریں[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_اچائی = wp->w_سطریں[i].wl_ناپ;
		    old_صفاں += wp->w_سطریں[i].wl_ناپ;
#جے_تعریف_شدہ FEAT_FOLDING
		    إذا (wp->w_سطریں[i].wl_valid
			    && wp->w_سطریں[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			طالما (i < wp->w_سطریں_valid
						  && !wp->w_سطریں[i].wl_valid)
			    old_صفاں += wp->w_سطریں[i++].wl_ناپ;
			اكسر;
		    }
#جے_انت
		}

		إذا (i >= wp->w_سطریں_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_شروع = 0;
		}
		وإلا
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    لكل (l = lnum; l < mod_bot; ++l)
		    {
			إذا (dollar_vcol >= 0 && wp == curwin &&
				old_cline_اچائی > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_صفاں += old_cline_اچائی;
#جے_تعریف_شدہ FEAT_FOLDING
			وإلا إذا (hasFoldingWin(wp, l, عدم, &l, TRUE, عدم))
			    ++new_صفاں;
#جے_انت
			وإلا
			    new_صفاں += plines_correct_topline(wp, l, TRUE);
			++j;
			إذا (new_صفاں > wp->w_اچائی - صف - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_صفاں = 9999;
			    اكسر;
			}
		    }
		    xtra_صفاں = new_صفاں - old_صفاں;
		    إذا (xtra_صفاں < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			إذا (صف - xtra_صفاں >= wp->w_اچائی - 2)
			    mod_bot = MAXLNUM;
			وإلا
			{
			    check_for_delay(FALSE);
			    إذا (win_del_سطریں(wp, صف,
					  -xtra_صفاں, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    وإلا
				bot_شروع = wp->w_اچائی + xtra_صفاں;
			}
		    }
		    وإلا إذا (xtra_صفاں > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			إذا (صف + xtra_صفاں >= wp->w_اچائی - 2)
			    mod_bot = MAXLNUM;
			وإلا
			{
			    check_for_delay(FALSE);
			    إذا (win_ins_سطریں(wp, صف + old_صفاں,
					     xtra_صفاں, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    وإلا إذا (چوٹی_سرا > صف + old_صفاں)
				// Scrolled the part at the top that requires
				// updating down.
				چوٹی_سرا += xtra_صفاں;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    إذا (mod_bot != MAXLNUM && i != j)
		    {
			إذا (j < i)
			{
			    صحيح x = صف + new_صفاں;

			    // move entries in w_lines[] upwards
			    لكل (;;)
			    {
				// stop at last valid entry in w_lines[]
				إذا (i >= wp->w_سطریں_valid)
				{
				    wp->w_سطریں_valid = j;
				    اكسر;
				}
				wp->w_سطریں[j] = wp->w_سطریں[i];
				// stop at a line that won't fit
				إذا (x + (صحيح)wp->w_سطریں[j].wl_ناپ
							   > wp->w_اچائی)
				{
				    wp->w_سطریں_valid = j + 1;
				    اكسر;
				}
				x += wp->w_سطریں[j++].wl_ناپ;
				++i;
			    }
			    إذا (bot_شروع > x)
				bot_شروع = x;
			}
			وإلا // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_سطریں_valid += j;
			    إذا (wp->w_سطریں_valid > wp->w_اچائی)
				wp->w_سطریں_valid = wp->w_اچائی;
			    لكل (i = wp->w_سطریں_valid; i - j >= idx; --i)
				wp->w_سطریں[i] = wp->w_سطریں[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    طالما (i >= idx)
			    {
				wp->w_سطریں[i].wl_ناپ = 0;
				wp->w_سطریں[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#جے_تعریف_شدہ FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_گنتی = folded_گنتی(wp, lnum, &win_foldinfo);
	    إذا (fold_گنتی != 0)
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
	    وإلا
#جے_انت
	    إذا (idx < wp->w_سطریں_valid
		    && wp->w_سطریں[idx].wl_valid
		    && wp->w_سطریں[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_سطریں[idx].wl_ناپ > wp->w_اچائی
#جے_تعریف_شدہ FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#جے_انت
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		صف = wp->w_اچائی + 1;
	    }
	    وإلا
	    {
#جے_تعریف_شدہ FEAT_تلاش_EXTRA
		prepare_تلاش_hl(wp, &screen_تلاش_hl, lnum);
#جے_انت
#جے_تعریف_شدہ FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		إذا (syntax_آخری_parsed != 0 && syntax_آخری_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_سرا_parsing(wp, syntax_آخری_parsed + 1);
#جے_انت

		// Display one line.
		صف = win_سطر(wp, lnum, srow, wp->w_اچائی, 0, &spv);

#جے_تعریف_شدہ FEAT_FOLDING
		wp->w_سطریں[idx].wl_folded = FALSE;
		wp->w_سطریں[idx].wl_lastlnum = lnum;
#جے_انت
#جے_تعریف_شدہ FEAT_SYN_HL
		did_update = DID_سطر;
		syntax_آخری_parsed = lnum;
#جے_انت
	    }

	    wp->w_سطریں[idx].wl_lnum = lnum;
	    wp->w_سطریں[idx].wl_valid = TRUE;

	    صحيح is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    إذا (صف > wp->w_اچائی || صف + wp->w_winrow >= صفاں)
	    {
		// we may need the size of that too long line later on
		إذا (dollar_vcol == -1 || !is_curline)
		    wp->w_سطریں[idx].wl_ناپ = plines_win(wp, lnum, TRUE);
		++idx;
		اكسر;
	    }
	    إذا (dollar_vcol == -1 || !is_curline)
		wp->w_سطریں[idx].wl_ناپ = صف - srow;
	    ++idx;
#جے_تعریف_شدہ FEAT_FOLDING
	    lnum += fold_گنتی + 1;
#وإلا
	    ++lnum;
#جے_انت
	}
	وإلا
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    إذا ((wp->w_p_nu && mod_چوٹی != 0 && lnum >= mod_bot
			&& buf->b_mod_سیٹ && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_آخری_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#جے_تعریف_شدہ FEAT_FOLDING
		fold_گنتی = folded_گنتی(wp, lnum, &win_foldinfo);
		إذا (fold_گنتی != 0)
		    fold_سطر(wp, fold_گنتی, &win_foldinfo, lnum, صف);
		وإلا
#جے_انت
		    (فراغ)win_سطر(wp, lnum, srow, wp->w_اچائی,
					       wp->w_سطریں[idx].wl_ناپ, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    صف += wp->w_سطریں[idx++].wl_ناپ;
	    إذا (صف > wp->w_اچائی)	// past end of screen
		اكسر;
#جے_تعریف_شدہ FEAT_FOLDING
	    lnum = wp->w_سطریں[idx - 1].wl_lastlnum + 1;
#وإلا
	    ++lnum;
#جے_انت
#جے_تعریف_شدہ FEAT_SYN_HL
	    did_update = DID_NONE;
#جے_انت
#جے_تعریف_شدہ FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#جے_انت
	}

	إذا (lnum > buf->b_ml.ml_سطر_گنتی)
	{
	    eof = TRUE;
	    اكسر;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	إذا (idx >= صفاں)
	    اكسر;
    }

    // End of loop over all window lines.

#جے_تعریف_شدہ FEAT_SYN_HL
    // Now that the window has been redrawn with the old and new cursor line,
    // update w_last_cursorline.
    wp->w_آخری_cursorline = wp->w_p_cul ? wp->w_cursor.lnum : 0;
#جے_انت
    wp->w_آخری_cursor_lnum_rnu = wp->w_p_rnu ? wp->w_cursor.lnum : 0;

#جے_تعریف_شدہ FEAT_VTP
    // Rewrite the character at the end of the screen line.
    // See the version that was fixed.
    إذا (use_vtp() && get_conpty_fix_type() < 1)
    {
	صحيح k;

	لكل (k = 0; k < صفاں; ++k)
	    إذا (enc_utf8)
		إذا ((*mb_off2cells)(سطر_وتھ[k] + topframe->fr_چوڑائی - 2,
					   سطر_وتھ[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_چوڑائی - 2, 1, 2,
			    FALSE, FALSE);
		وإلا
		    screen_draw_rectangle(k, topframe->fr_چوڑائی - 1, 1, 1,
			    FALSE, FALSE);
	    وإلا
		screen_char(سطر_وتھ[k] + topframe->fr_چوڑائی - 1, k,
			cmdline_چوڑائی - 1);
    }
#جے_انت

    إذا (idx > wp->w_سطریں_valid)
	wp->w_سطریں_valid = idx;

#جے_تعریف_شدہ FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    إذا (syntax_آخری_parsed != 0 && syntax_present(wp))
	syntax_سرا_parsing(wp, syntax_آخری_parsed + 1);
#جے_انت

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_خالی_تھاواں_صفاں = 0;
#جے_تعریف_شدہ FEAT_DIFF
    wp->w_filler_صفاں = 0;
#جے_انت
    إذا (!eof && !didline)
    {
	إذا (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#جے_تعریف_شدہ FEAT_DIFF
	وإلا إذا (diff_check_fill(wp, lnum) >= wp->w_اچائی - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_صفاں = wp->w_اچائی - srow;
	}
#جے_انت
#جے_تعریف_شدہ FEAT_PROP_POPUP
	وإلا إذا (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#جے_انت
	وإلا إذا (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    صحيح		scr_صف = W_WINROW(wp) + wp->w_اچائی - 1;
	    صحيح		علامت  = wp->w_fill_chars.lastline;
	    صحيح		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(علامت, &fillbuf[0]);
	    mb_char2bytes(علامت, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_چوڑائی > 2 ? 2 : wp->w_چوڑائی) * charlen,
			    scr_صف, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_صف, scr_صف + 1,
		    (صحيح)wp->w_wincol + 2, (صحيح)W_ENDCOL(wp),
		    علامت, ' ', HL_ATTR(HLF_AT));
	    سیٹ_خالی_تھاواں_صفاں(wp, srow);
	    wp->w_botline = lnum;
	}
	وإلا إذا (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    صحيح شروع_خانہ = (صحيح)W_ENDCOL(wp) - 3;
	    صحيح علامت    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_اچائی - 1,
		    W_WINROW(wp) + wp->w_اچائی,
		    (شروع_خانہ < wp->w_wincol ? wp->w_wincol : شروع_خانہ),
		    (صحيح)W_ENDCOL(wp),
		    علامت, علامت, HL_ATTR(HLF_AT));
	    سیٹ_خالی_تھاواں_صفاں(wp, srow);
	    wp->w_botline = lnum;
	}
	وإلا
	{
	    win_draw_سرا(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_اچائی, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    وإلا
    {
	draw_vsep_win(wp, صف);
	إذا (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_سطر_گنتی + 1;
#جے_تعریف_شدہ FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    إذا (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		إذا (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		وإلا
		    i = wp->w_fill_chars.diff;
		إذا (صف + j > wp->w_اچائی)
		    j = wp->w_اچائی - صف;
		win_draw_سرا(wp, i, i, TRUE, صف, صف + (صحيح)j, HLF_DED);
		صف += j;
	    }
#جے_انت
	}
	وإلا إذا (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	إذا (WIN_IS_POPUP(wp))
	    win_draw_سرا(wp, ' ', ' ', FALSE, صف, wp->w_اچائی, HLF_AT);
	وإلا
	    win_draw_سرا(wp, wp->w_fill_chars.eob, ' ', FALSE,
						   صف, wp->w_اچائی, HLF_EOB);
    }

#جے_تعریف_شدہ SYN_ویلا_حد_بندی
    disable_regexp_timeout();
    redrawtime_حد_بندی_سیٹ = FALSE;
#جے_انت

    // Reset the type of redrawing required, the window has been updated.
    wp->w_redr_type = 0;
#جے_تعریف_شدہ FEAT_DIFF
    wp->w_old_topfill = wp->w_topfill;
    wp->w_old_botfill = wp->w_botfill;
#جے_انت

    إذا (dollar_vcol == -1 || wp != curwin)
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
	إذا (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#إذا defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    صحيح		new_wcol = wp->w_wcol;
#جے_انت
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#إذا defined(FEAT_CONCEAL)
	    إذا (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
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
		إذا (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		وإلا
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#جے_انت
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    إذا (wp->w_redr_type != 0)
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
		إذا (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#إذا defined(FEAT_SYN_HL) || defined(FEAT_تلاش_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    إذا (!got_int)
	got_int = save_got_int;
#جے_انت

    إذا (override_success)
	نکالو_highlight_overrides();
}

#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    ساكن فراغ
update_prepare(فراغ)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    إذا (gui.وچ_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_تلاش_EXTRA
    شروع_تلاش_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_نقاب(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    ساكن فراغ
update_انت(فراغ)
{
    إذا (redraw_cmdline || redraw_منوال)
	showmode();

# ifdef FEAT_تلاش_EXTRA
    سرا_تلاش_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    إذا (gui.وچ_use)
    {
	باہرلا_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#جے_انت

#إذا defined(FEAT_NETBEANS_INTG)
    فراغ
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    صحيح		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_سطر_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    إذا (!doit || updating_screen
	    || State == منوال_ASKMORE || State == منوال_HITRETURN
	    || پیغام_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	أرجع;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_redr_type != 0)
	    win_update(wp);
	إذا (wp->w_redr_حالت)
	    win_redr_حالت(wp, FALSE);
    }

# إذا defined(FEAT_TABPANEL)
    إذا (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_انت();
}
#جے_انت

#إذا defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    فراغ
update_کھڑکی(win_T *wp)
{
    // return if already busy updating
    إذا (updating_screen)
	أرجع;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    إذا (clip_تارا.دستیاب && clip_isautosel_تارا())
	clip_update_selection(&clip_تارا);
    إذا (clip_plus.دستیاب && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    إذا (redraw_tabline)
	draw_tabline();

# إذا defined(FEAT_TABPANEL)
    إذا (redraw_tabpanel)
	draw_tabpanel();
# endif

    إذا (wp->w_redr_حالت || p_ru
# ifdef FEAT_STL_OPT
	    || *p_stl != NUL || *wp->w_p_stl != NUL
# endif
	    )
	win_redr_حالت(wp, FALSE);

# ifdef FEAT_PROP_POPUP
    // Display popup windows on top of everything.
    update_popups(win_update);
# endif

    update_انت();
}
#جے_انت

/*
 * Redraw as soon as possible.  When the command line is not scrolled redraw
 * right away and restore what was on the command line.
 * Return a code indicating what happened.
 */
    صحيح
redraw_asap(صحيح type)
{
    صحيح		صفاں_2;
    صحيح		خانے_گنتی = screen_Columns;
    صحيح		r;
    صحيح		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    صحيح		i;
    u8char_T	*screenlineUC = عدم;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = عدم;	// copy from ScreenLines2[]

    redraw_later(type);
    إذا (پیغام_scrolled
	    || (State != منوال_عام && State != منوال_عام_BUSY)
	    || exiting)
	أرجع ret;

    // Allocate space to save the text displayed in the command line area.
    صفاں_2 = screen_صفاں - cmdline_صف;
    screenline = LALLOC_MULT(schar_T, صفاں_2 * خانے_گنتی);
    screenattr = LALLOC_MULT(sattr_T, صفاں_2 * خانے_گنتی);
    إذا (screenline == عدم || screenattr == عدم)
	ret = 2;
    إذا (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, صفاں_2 * خانے_گنتی);
	إذا (screenlineUC == عدم)
	    ret = 2;
	لكل (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, صفاں_2 * خانے_گنتی);
	    إذا (screenlineC[i] == عدم)
		ret = 2;
	}
    }
    إذا (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, صفاں_2 * خانے_گنتی);
	إذا (screenline2 == عدم)
	    ret = 2;
    }

    إذا (ret != 2)
    {
	// Save the text displayed in the command line area.
	لكل (r = 0; r < صفاں_2; ++r)
	{
	    mch_memmove(screenline + r * خانے_گنتی,
			Screen_سطریں + سطر_وتھ[cmdline_صف + r],
			(size_t)خانے_گنتی * حجم(schar_T));
	    mch_memmove(screenattr + r * خانے_گنتی,
			ScreenAttrs + سطر_وتھ[cmdline_صف + r],
			(size_t)خانے_گنتی * حجم(sattr_T));
	    إذا (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * خانے_گنتی,
			    Screen_سطریں_UC + سطر_وتھ[cmdline_صف + r],
			    (size_t)خانے_گنتی * حجم(u8char_T));
		لكل (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * خانے_گنتی,
				Screen_سطریں_C[i] + سطر_وتھ[cmdline_صف + r],
				(size_t)خانے_گنتی * حجم(u8char_T));
	    }
	    إذا (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * خانے_گنتی,
			    ScreenLines2 + سطر_وتھ[cmdline_صف + r],
			    (size_t)خانے_گنتی * حجم(schar_T));
	}

	update_screen(0);
	ret = 3;

	إذا (must_redraw == 0)
	{
	    صحيح	off = (صحيح)(current_Screen_سطر - Screen_سطریں);

	    // Restore the text displayed in the command line area.
	    لكل (r = 0; r < صفاں_2; ++r)
	    {
		mch_memmove(current_Screen_سطر,
			    screenline + r * خانے_گنتی,
			    (size_t)خانے_گنتی * حجم(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * خانے_گنتی,
			    (size_t)خانے_گنتی * حجم(sattr_T));
		إذا (enc_utf8)
		{
		    mch_memmove(Screen_سطریں_UC + off,
				screenlineUC + r * خانے_گنتی,
				(size_t)خانے_گنتی * حجم(u8char_T));
		    لكل (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_سطریں_C[i] + off,
				    screenlineC[i] + r * خانے_گنتی,
				    (size_t)خانے_گنتی * حجم(u8char_T));
		}
		إذا (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * خانے_گنتی,
				(size_t)خانے_گنتی * حجم(schar_T));
		screen_سطر(curwin, cmdline_صف + r, 0, خانے_گنتی, خانے_گنتی, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    إذا (enc_utf8)
    {
	vim_free(screenlineUC);
	لكل (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    إذا (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_سنیہا();

    setcursor();

    أرجع ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    فراغ
redraw_after_callback(صحيح call_update_screen, صحيح do_سنیہا)
{
    ++redrawing_for_callback;

    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_ENABLE);

    إذا (State == منوال_HITRETURN || State == منوال_ASKMORE
	    || State == منوال_SETWSIZE || State == منوال_EXTERNCMD
	    || State == منوال_CONFIRM || exmode_سرگرم)
    {
	إذا (do_سنیہا)
	    repeat_سنیہا();
    }
    وإلا إذا (State & منوال_CMDLINE)
    {
	إذا (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	إذا (cmdline_صف > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    إذا (پیغام_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    وإلا إذا (State & (منوال_عام | منوال_داخل_کرو | منوال_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	إذا (پیغام_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    پیغام_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_DISABLE);
#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	باہرلا_flush_cursor(FALSE, FALSE);
    وإلا
#جے_انت
	باہرلا_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    فراغ
redraw_later(صحيح type)
{
    redraw_win_later(curwin, type);
}

    فراغ
redraw_win_later(
    win_T	*wp,
    صحيح		type)
{
#جے_تعریف_شدہ FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    إذا (inside_redraw_on_شروع_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    وإلا
#جے_انت
    إذا (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	إذا (type >= UPD_NOT_VALID)
	    wp->w_سطریں_valid = 0;
	إذا (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    فراغ
redraw_later_clear(فراغ)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    فراغ
redraw_all_later(صحيح type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    سیٹ_must_redraw(type);
}

#إذا 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    فراغ
redraw_all_windows_later(صحيح type)
{
    redraw_all_later(type);
# ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
# endif
}
#جے_انت

/*
 * Set "must_redraw" to "type" unless it already has a higher value
 * or it is currently not allowed.
 */
    فراغ
سیٹ_must_redraw(صحيح type)
{
    إذا (!redraw_not_allowed &&
#جے_تعریف_شدہ FEAT_EVAL
	    !inside_redraw_on_شروع_cb &&
#جے_انت
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    فراغ
redraw_curbuf_later(صحيح type)
{
    redraw_buf_later(curbuf, type);
}

    فراغ
redraw_buf_later(buf_T *buf, صحيح type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_بفر == buf)
	    redraw_win_later(wp, type);
    }
#جے_تعریف_شدہ FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_بفر == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_وچ_TAB(curtab, wp)
	إذا (wp->w_بفر == buf)
	    redraw_win_later(wp, type);
#جے_انت
#إذا defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    إذا (curwin->w_بفر == buf)
	redraw_win_later(curwin, type);
#جے_انت
}

#إذا defined(FEAT_SIGNS)
    فراغ
redraw_buf_سطر_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_بفر == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#جے_انت

#إذا defined(FEAT_کم_CHANNEL)
    فراغ
redraw_buf_and_حالت_later(buf_T *buf, صحيح type)
{
    win_T	*wp;

    إذا (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	أرجع;
    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_بفر == buf)
	{
	    redraw_win_later(wp, type);
	    wp->w_redr_حالت = true;
	}
    }
}
#جے_انت

/*
 * mark the ruler for redraw when the last window has no status line and the
 * ruler takes its place in the last screen line; showmode() draws it
 */
    ساكن فراغ
ruler_redraw_lastwin(فراغ)
{
    إذا (p_ru && lastwin->w_حالت_اچائی == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    فراغ
حالت_redraw_all(فراغ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_حالت_اچائی)
	{
	    wp->w_redr_حالت = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    فراغ
حالت_redraw_curbuf(فراغ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_حالت_اچائی != 0 && wp->w_بفر == curbuf)
	{
	    wp->w_redr_حالت = true;
	    redraw_later(UPD_VALID);
	}
    إذا (lastwin->w_بفر == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    فراغ
redraw_statuslines(فراغ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_redr_حالت)
	{
	    bool ret = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_حالت(wp, FALSE);
	    إذا (ret)
		نکالو_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    إذا (redraw_tabline)
	draw_tabline();

#إذا defined(FEAT_TABPANEL)
    إذا (redraw_tabpanel)
	draw_tabpanel();
#جے_انت
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    فراغ
win_redraw_آخری_حالت(چوکھٹا_T *frp)
{
    إذا (frp->fr_layout == FR_پتی)
	frp->fr_win->w_redr_حالت = true;
    وإلا إذا (frp->fr_layout == FR_صف)
    {
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    win_redraw_آخری_حالت(frp);
    }
    وإلا // frp->fr_layout == FR_COL
    {
	frp = frp->fr_بچہ;
	طالما (frp->fr_اگلا != عدم)
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
    فراغ
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    فراغ
redraw_win_range_later(
    win_T	*wp,
    linenr_T	پہلا,
    linenr_T	آخری)
{
    إذا (آخری >= wp->w_topline && پہلا < wp->w_botline)
    {
	إذا (wp->w_redraw_چوٹی == 0 || wp->w_redraw_چوٹی > پہلا)
	    wp->w_redraw_چوٹی = پہلا;
	إذا (wp->w_redraw_bot == 0 || wp->w_redraw_bot < آخری)
	    wp->w_redraw_bot = آخری;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    فراغ
redraw_win_range_now(
    win_T	*wp,
    linenr_T	پہلا,
    linenr_T	آخری)
{
    إذا (آخری >= wp->w_topline && پہلا < wp->w_botline)
    {
	إذا (wp->w_redraw_چوٹی == 0 || wp->w_redraw_چوٹی > پہلا)
	    wp->w_redraw_چوٹی = پہلا;
	إذا (wp->w_redraw_bot == 0 || wp->w_redraw_bot < آخری)
	    wp->w_redraw_bot = آخری;
	إذا (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#جے_تعریف_شدہ FEAT_EVAL
ساكن bool redraw_cb_وچ_ترقی = false;

    فراغ
f_redraw_listener_جوڑو(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    ساكن صحيح		id;

    إذا (check_secure())
	أرجع;

    إذا (redraw_cb_وچ_ترقی)
    {
	emsg(_(e_cannot_جوڑو_redraw_listener_وچ_listener_callback));
	أرجع;
    }

    إذا (check_for_dict_arg(argvars, 0) == FAIL)
	أرجع;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    إذا (rln == عدم)
	أرجع;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    إذا (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	إذا (cb.cb_name == عدم)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    أرجع;
	}
	copy_callback(&rln->rl_callbacks.on_شروع, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    إذا (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	إذا (cb.cb_name == عدم)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_شروع);
	    vim_free(rln);
	    أرجع;
	}
	copy_callback(&rln->rl_callbacks.on_سرا, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    إذا (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	أرجع;
    }

    rln->rl_اگلا = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_نمبر;
    rettv->vval.v_نمبر = id;

    أرجع;
}

    ساكن فراغ
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_شروع);
    free_callback(&rln->rl_callbacks.on_سرا);

    vim_free(rln);
}


    ساكن فراغ
redraw_listener_cleanup(فراغ)
{
    لكل (redraw_listener_T *rln = redraw_listeners; rln != عدم;)
    {
	redraw_listener_T *اگلا = rln->rl_اگلا;
	إذا (rln->rl_id == 0)
	{
	    إذا (redraw_listeners == rln)
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
    ساكن redraw_listener_T *
get_redraw_listener(صحيح id)
{
    لكل (redraw_listener_T *rln = redraw_listeners; rln != عدم; rln = rln->rl_اگلا)
	إذا (rln->rl_id == id)
	    أرجع rln;
    أرجع عدم;
}

    فراغ
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    صحيح			id;
    redraw_listener_T	*rln;

    إذا (check_for_نمبر_arg(argvars, 0) == FAIL)
	أرجع;

    id = argvars[0].vval.v_نمبر;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_نمبر;
    إذا (rln == عدم)
    {
	rettv->vval.v_نمبر = 0;
	أرجع;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_نمبر = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    ساكن فراغ
invoke_redraw_listener_شروع_or_سرا(bool شروع)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    إذا (شروع)
	inside_redraw_on_شروع_cb = true;

    redraw_cb_وچ_ترقی = true;
    لكل (redraw_listener_T *rln = redraw_listeners; rln != عدم; rln = rln->rl_اگلا)
    {
	إذا (rln->rl_id == 0)
	    // Listener has been removed, skip
	    استمر;
	إذا (شروع && rln->rl_callbacks.on_شروع.cb_name != عدم)
	{
	    call_callback(&rln->rl_callbacks.on_شروع, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	وإلا إذا (rln->rl_callbacks.on_سرا.cb_name != عدم)
	{
	    call_callback(&rln->rl_callbacks.on_سرا, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_وچ_ترقی = false;

    إذا (شروع)
	inside_redraw_on_شروع_cb = false;
}
#جے_انت // FEAT_EVAL
