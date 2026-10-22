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
#اگر_تعریف_شده FEAT_STL_OPT
ساكن فراغ redraw_custom_statusline(win_T *wp);
#پایان_اگر
ساكن فراغ borrow_stl_vsep_hl(فراغ);
#إذا defined(FEAT_جستجو_EXTRA) || defined(FEAT_CLIPBOARD)
ساكن صحيح  did_update_one_پنجره;
#پایان_اگر

#اگر_تعریف_شده FEAT_EVAL
ساكن فراغ redraw_listener_cleanup(فراغ);
ساكن فراغ invoke_redraw_listener_شروع_or_انتها(bool شروع);
#پایان_اگر

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
#اگر_تعریف_شده FEAT_GUI
    صحيح		did_one = FALSE;
    صحيح		did_undraw = FALSE;
    صحيح		gui_cursor_ستون = 0;
    صحيح		gui_cursor_ردیف = 0;
#پایان_اگر
    صحيح		no_update = FALSE;
    صحيح		save_pum_will_redraw = pum_will_redraw;
#اگر_تعریف_شده FEAT_PROP_POPUP
    صحيح		did_redraw_پنجره = FALSE;
#پایان_اگر
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    إذا (!screen_valid(TRUE))
	أرجع FAIL;

    إذا (type == UPD_VALID_NO_UPDATE)
    {
	no_update = TRUE;
	type = 0;
    }

#اگر_تعریف_شده FEAT_EVAL
    {
	buf_T *buf;

	// Before updating the screen, notify any listeners of changed text.
	FOR_ALL_BUFFERS(buf)
	    invoke_listeners(buf);
    }
#پایان_اگر

#اگر_تعریف_شده FEAT_DIFF
    // May have postponed updating diffs.
    إذا (need_diff_redraw)
	diff_redraw(TRUE);
#پایان_اگر

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
    إذا (curwin->w_سطرها_valid == 0 && type < UPD_NOT_VALID
#اگر_تعریف_شده FEAT_TERMINAL
	    && !جمله_do_update_پنجره(curwin)
#پایان_اگر
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    إذا (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	إذا (type > UPD_INVERTED_ALL)
	    curwin->w_سطرها_valid = 0;	// don't use w_lines[].wl_size now
	أرجع FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    صحيح hid_cursor = !sync_output_فعال();
    إذا (hid_cursor)
	cursor_off();
    جمله_مجموعه_sync_output(جمله_SYNC_OUTPUT_ENABLE);

#اگر_تعریف_شده FEAT_PROP_POPUP
    // Update popup_mask if needed.  This may set w_redraw_top and w_redraw_bot
    // in some windows.
    may_update_popup_ماسک(type);
#پایان_اگر

#اگر_تعریف_شده FEAT_SYN_HL
    ++display_tick;	    // let syntax code know we're in a next round of
			    // display updating
#پایان_اگر
    إذا (no_update)
	++no_win_do_سطرها_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    إذا (پیام_scrolled)
    {
	clear_cmdline = TRUE;
	إذا (type != UPD_CLEAR)
	{
	    إذا (پیام_scrolled > ردیفها - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    وإلا
	    {
		check_for_delay(FALSE);
		إذا (screen_ins_سطرها(0, 0, پیام_scrolled, (صحيح)ردیفها, 0, عدم)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    إذا (wp->w_winrow < پیام_scrolled)
		    {
			إذا (W_WINROW(wp) + wp->w_ارتفاع > پیام_scrolled
				&& wp->w_redr_type < UPD_REDRAW_بالاترین
				&& wp->w_سطرها_valid > 0
				&& wp->w_topline == wp->w_سطرها[0].wl_lnum)
			{
			    wp->w_upd_ردیفها = پیام_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_بالاترین;
			}
			وإلا
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    إذا (W_WINROW(wp) + wp->w_ارتفاع
					 + wp->w_وضعیت_ارتفاع <= پیام_scrolled)
				wp->w_redr_وضعیت = true;
			}
		    }
		}
		إذا (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#إذا defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#پایان_اگر
	}
	پیام_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    إذا (need_highlight_تغییریافته)
	highlight_تغییریافته();

    إذا (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    إذا (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#اگر_تعریف_شده FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    إذا (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? عدد_عرض(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#پایان_اگر

#اگر_تعریف_شده FEAT_EVAL
    invoke_redraw_listener_شروع_or_انتها(true);
#پایان_اگر

    // Only start redrawing if there is really something to do.
    إذا (type == UPD_INVERTED)
	update_curswant();
    إذا (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_سطرها[0].wl_valid
#اگر_تعریف_شده FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#پایان_اگر
		    && curwin->w_topline == curwin->w_سطرها[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_فعال
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_نما == VIsual_نما
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
#پایان_اگر

#اگر_تعریف_شده FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_میانگیر->b_mod_مجموعه)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    لكل (wwp = firstwin; wwp != wp; wwp = wwp->w_بعدی)
		إذا (wwp->w_میانگیر == wp->w_میانگیر)
		    اكسر;
	    إذا (wwp == wp && syntax_present(wp))
		syn_پشته_apply_changes(wp->w_میانگیر);
	}
    }
#پایان_اگر

    إذا (pum_redraw_درون_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#إذا defined(FEAT_جستجو_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_پنجره = FALSE;
#پایان_اگر
#اگر_تعریف_شده FEAT_جستجو_EXTRA
    screen_جستجو_hl.rm.regprog = عدم;
#پایان_اگر
    FOR_ALL_WINDOWS(wp)
    {
	override_success = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

	إذا (wp->w_redr_type != 0)
	{
	    cursor_off();
#اگر_تعریف_شده FEAT_PROP_POPUP
	    did_redraw_پنجره = TRUE;
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI
	    إذا (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		إذا (gui.درون_use && (wp == curwin || cursor_is_sleeping()))
		{
		    gui_cursor_ستون = gui.cursor_ستون;
		    gui_cursor_ردیف = gui.cursor_ردیف;
		    gui_undraw_cursor();
		    did_undraw = TRUE;
		}
	    }
#پایان_اگر
	    win_update(wp);
	}

	// redraw status line after the window to minimize cursor movement
	إذا (wp->w_redr_وضعیت)
	{
	    cursor_off();
	    win_redr_وضعیت(wp, TRUE); // any popup menu will be redrawn below
	}

	إذا (override_success)
	    برداشت_highlight_overrides();

    }
#إذا defined(FEAT_جستجو_EXTRA)
    انتها_جستجو_hl();
#پایان_اگر

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    إذا (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_vsep_عرض > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_میانگیر->b_mod_مجموعه = false;

#اگر_تعریف_شده FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    إذا (did_redraw_پنجره || popup_need_redraw())
	update_popups(win_update);
#پایان_اگر

#اگر_تعریف_شده FEAT_TERMINAL
    FOR_ALL_WINDOWS(wp)
	// If this window contains a terminal, after redrawing all windows, the
	// dirty row range can be reset.
	جمله_did_update_پنجره(wp);
#پایان_اگر

    after_updating_screen(TRUE);

    // Clear or redraw the command line.  Done last, because scrolling may
    // mess up the command line.
    إذا (clear_cmdline || redraw_cmdline || redraw_نما)
	showmode();

    إذا (no_update)
	--no_win_do_سطرها_ins;

    // May put up an introductory message when not editing a file
    إذا (!did_intro)
	maybe_intro_پیام();
    did_intro = TRUE;

#اگر_تعریف_شده FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    إذا (gui.درون_use)
    {
	إذا (did_undraw && !gui_mch_is_blink_off())
	{
	    mch_disable_flush();
	    بیرون_flush();	// required before updating the cursor
	    mch_enable_flush();

	    // Put the GUI position where the cursor was, gui_update_cursor()
	    // uses that.
	    gui.ستون = gui_cursor_ستون;
	    gui.ردیف = gui_cursor_ردیف;
	    gui.ستون = mb_fix_ستون(gui.ستون, gui.ردیف);
	    gui_update_cursor(FALSE, FALSE);
	    gui_may_flush();
	    screen_cur_ستون = gui.ستون;
	    screen_cur_ردیف = gui.ردیف;
	}
	وإلا
	    بیرون_flush();
	gui_update_scrollbars(FALSE);
    }
#پایان_اگر

#اگر_تعریف_شده FEAT_IMAGE
    // Make sure to do this last!
    draw_image_placements();
#پایان_اگر

#اگر_تعریف_شده FEAT_EVAL
    invoke_redraw_listener_شروع_or_انتها(false);
    redraw_listener_cleanup();
#پایان_اگر

    جمله_مجموعه_sync_output(جمله_SYNC_OUTPUT_DISABLE);
    إذا (hid_cursor)
	cursor_on();

    أرجع OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    صحيح
statusline_ردیف(win_T *wp)
{
#إذا defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    إذا (wp->w_قاب->fr_ارتفاع == wp->w_وضعیت_ارتفاع && !popup_is_popup(wp))
	أرجع wp->w_winrow;
#پایان_اگر
    أرجع W_WINROW(wp) + wp->w_ارتفاع;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    فراغ
win_redr_وضعیت(win_T *wp, صحيح ignore_pum استفادهنشده)
{
    صحيح		ردیف;
    صحيح		fillchar;
    صحيح		attr;
    صحيح		i;
    ساكن صحيح  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    إذا (busy)
	أرجع;
    busy = TRUE;

    ردیف = statusline_ردیف(wp);

    wp->w_redr_وضعیت = false;
    إذا (wp->w_وضعیت_ارتفاع == 0)
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
	wp->w_redr_وضعیت = true;
    }
#اگر_تعریف_شده FEAT_STL_OPT
    وإلا إذا (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#پایان_اگر
    وإلا
    {
	char_u	*p;
	صحيح	plen;
	صحيح	NameBufflen;
	صحيح	this_ru_ستون;
	صحيح	n;			// scratch value

	fillchar = fillchar_وضعیت(&attr, wp);

	get_trans_bufname(wp->w_میانگیر);
	p = NameBuff;
	plen = (صحيح)STRLEN(p);

	إذا ((bt_help(wp->w_میانگیر)
#اگر_تعریف_شده FEAT_QUICKFIX
		    || wp->w_p_pvw
#پایان_اگر
		    || buf_Is_تغییریافته(wp->w_میانگیر)
		    || wp->w_میانگیر->b_p_ro)
		&& plen < MAXPATHL - 1)
	{
	    *(p + plen++) = ' ';	// replace NUL with space
	    *(p + plen) = NUL;		// NUL terminate the string
	}
	إذا (bt_help(wp->w_میانگیر))
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#اگر_تعریف_شده FEAT_QUICKFIX
	إذا (wp->w_p_pvw)
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#پایان_اگر
	إذا (buf_Is_تغییریافته(wp->w_میانگیر) && !bt_terminal(wp->w_میانگیر))
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", "[+]");
	إذا (wp->w_میانگیر->b_p_ro)
	    plen += vim_snprintf((حرف *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_ستون = ru_ستون - (cmdline_عرض - wp->w_عرض);
	n = (wp->w_عرض + 1) / 2;
	إذا (this_ru_ستون < n)
	    this_ru_ستون = n;
	إذا (this_ru_ستون <= 1)
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
	    لكل (i = 0; p[i] != NUL && plen >= this_ru_ستون - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    إذا (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	وإلا إذا (plen > this_ru_ستون - 1)
	{
	    p += plen - (this_ru_ستون - 1);
	    *p = '<';
	    plen = this_ru_ستون - 1;
	}

	screen_puts(p, ردیف, wp->w_wincol, attr);
	screen_fill(ردیف, ردیف + 1, plen + wp->w_wincol,
		    this_ru_ستون + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	لكل (i = 1; i < wp->w_وضعیت_ارتفاع; i++)
	    screen_fill(ردیف + i, ردیف + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	إذا ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_ستون - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, ردیف, (صحيح)(this_ru_ستون - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	إذا (p_sc && *p_sloc == 's')
	{
	    n = this_ru_ستون - plen - 2;		    // perform the calculation here so we only do it once
	    صحيح	عرض = MIN(10, n);

	    إذا (عرض > 0)
		screen_puts_len(showcmd_buf, عرض, ردیف,
				wp->w_wincol + this_ru_ستون - عرض - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    إذا (wp->w_vsep_عرض != 0 && wp->w_وضعیت_ارتفاع != 0 && redrawing())
    {
	لكل (i = 0; i < wp->w_وضعیت_ارتفاع; i++)
	{
	    صحيح r = ردیف + i;
	    إذا (stl_connected(wp))
		fillchar = fillchar_وضعیت(&attr, wp);
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
    win_T   *چپ_2 = عدم;
    win_T   *راست_2 = عدم;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    إذا (!redrawing() || Screen_سطرها == عدم)
	أرجع;

    FOR_ALL_WINDOWS(چپ_2)
    {
	إذا (چپ_2->w_وضعیت_ارتفاع == 0 || چپ_2->w_vsep_عرض == 0)
	    استمر;
	// A window of zero width has no status line cell to borrow from.
	إذا (چپ_2->w_عرض == 0)
	    استمر;
	إذا (!stl_connected(چپ_2))
	    استمر;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = عدم;
	صحيح شروع = 0;
	صحيح انتها = 0;

	FOR_ALL_WINDOWS(راست_2)
	{
	    إذا (راست_2 == چپ_2 || راست_2->w_وضعیت_ارتفاع == 0)
		استمر;
	    إذا (راست_2->w_wincol != W_ENDCOL(چپ_2) + 1)
		استمر;
	    صحيح l_stl_ردیف = W_WINROW(چپ_2) + چپ_2->w_ارتفاع;
	    صحيح r_stl_ردیف = W_WINROW(راست_2) + راست_2->w_ارتفاع;

	    شروع = l_stl_ردیف > r_stl_ردیف ? l_stl_ردیف : r_stl_ردیف;
	    انتها = l_stl_ردیف + چپ_2->w_وضعیت_ارتفاع
				< r_stl_ردیف + راست_2->w_وضعیت_ارتفاع
		? l_stl_ردیف + چپ_2->w_وضعیت_ارتفاع
		: r_stl_ردیف + راست_2->w_وضعیت_ارتفاع;
	    إذا (شروع < انتها)
	    {
		neighbour = راست_2;
		اكسر;
	    }
	}
	إذا (neighbour == عدم)
	    استمر;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	صحيح	hl;
	إذا (چپ_2 != curwin && neighbour != curwin
		&& fillchar_وضعیت(&hl, چپ_2) != ' ')
	    استمر;

	// Source: prefer curwin's side; otherwise left window's right edge.
	صحيح dst_ستون = W_ENDCOL(چپ_2);
	صحيح src_ستون = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(چپ_2) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	إذا (dst_ستون >= screen_Columns || src_ستون >= screen_Columns)
	    استمر;
	إذا (انتها > screen_ردیفها)
	    انتها = screen_ردیفها;

	لكل (صحيح r = شروع; r < انتها; r++)
	{
	    بیعلامت	dst_off = سطر_جابه_جایی[r] + dst_ستون;
	    sattr_T	attr = ScreenAttrs[سطر_جابه_جایی[r] + src_ستون];

	    إذا (ScreenAttrs[dst_off] != attr)
	    {
		ScreenAttrs[dst_off] = attr;
		screen_char(dst_off, r, dst_ستون);
	    }
	}
    }
}

#اگر_تعریف_شده FEAT_STL_OPT
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
#پایان_اگر

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
	curwin->w_redr_وضعیت = true;
	أرجع;
    }
#إذا defined(FEAT_STL_OPT)
    إذا ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_وضعیت_ارتفاع)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    وإلا
#پایان_اگر
	win_redr_ruler(curwin, always, FALSE);

    إذا (need_maketitle
#اگر_تعریف_شده FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_درون_ICON))
	    || (p_title && (stl_syntax & STL_درون_TITLE))
#پایان_اگر
       )
	maketitle();

    // Redraw the tab pages line if needed.
    إذا (redraw_tabline)
	draw_tabline();

#إذا defined(FEAT_TABPANEL)
    إذا (redraw_tabpanel)
	draw_tabpanel();
#پایان_اگر
}

    فراغ
win_redr_ruler(win_T *wp, صحيح always, صحيح ignore_pum)
{
    صحيح	خالی_سطر = FALSE;

    // If 'ruler' off don't do anything
    إذا (!p_ru)
	أرجع;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    إذا (wp->w_cursor.lnum > wp->w_میانگیر->b_ml.ml_سطر_شمار)
	أرجع;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    إذا (wp == lastwin && lastwin->w_وضعیت_ارتفاع == 0)
	إذا (edit_submode != عدم)
	    أرجع;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    إذا (!ignore_pum && pum_visible())
	أرجع;

#اگر_تعریف_شده FEAT_STL_OPT
    إذا (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	أرجع;
    }
#پایان_اگر

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    إذا ((State & نما_درج) == 0
		&& *ml_get_buf(wp->w_میانگیر, wp->w_cursor.lnum, FALSE) == NUL)
	خالی_سطر = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    إذا (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.ستون != wp->w_ru_cursor.ستون
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_میانگیر->b_ml.ml_سطر_شمار != wp->w_ru_سطر_شمار
#اگر_تعریف_شده FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#پایان_اگر
	    || خالی_سطر != wp->w_ru_خالی)
    {
	صحيح	ردیف;
	صحيح	fillchar;
	صحيح	attr;
	صحيح	off;
	صحيح	عرض;
	colnr_T	virtcol;
#تعریف_کن RULER_BUF_LEN 70
	char_u	میانگیر[RULER_BUF_LEN];
	صحيح	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	صحيح	rel_poslen;
	صحيح	this_ru_ستون;
	صحيح	n1;			    // scratch value
	صحيح	n2;			    // scratch value
	bool	override_success =
	    افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	إذا (wp->w_وضعیت_ارتفاع)
	{
	    ردیف = statusline_ردیف(wp);
	    fillchar = fillchar_وضعیت(&attr, wp);
	    off = wp->w_wincol;
	    عرض = wp->w_عرض;
	}
	وإلا
	{
	    ردیف = ردیفها - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_ستون_off;
	    عرض = cmdline_عرض;
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
	bufferlen = vim_snprintf((حرف *)میانگیر, RULER_BUF_LEN, _("%ld,"),
		(wp->w_میانگیر->b_ml.ml_flags & ML_خالی)
		    ? 0L
		    : (دراز)(wp->w_cursor.lnum));
	bufferlen += ستون_print(میانگیر + bufferlen, RULER_BUF_LEN - bufferlen,
			خالی_سطر ? 0 : (صحيح)wp->w_cursor.ستون + 1,
			(صحيح)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	إذا (wp->w_وضعیت_ارتفاع == 0)	// can't use last char of screen
	    ++n1;

	this_ru_ستون = ru_ستون - (cmdline_عرض - عرض);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (عرض + 1) / 2;
	إذا (this_ru_ستون < n2)
	    this_ru_ستون = n2;
	إذا (this_ru_ستون + n1 < عرض)
	{
	    // need at least space for rel_pos + NUL
	    طالما (this_ru_ستون + n1 < عرض
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		إذا (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, میانگیر + bufferlen);
		وإلا
		    میانگیر[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((حرف *)میانگیر + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	إذا (has_mbyte)
	{
	    لكل (n1 = 0, n2 = 0; میانگیر[n1] != NUL; n1 += (*mb_ptr2len)(میانگیر + n1))
	    {
		n2 += (*mb_ptr2cells)(میانگیر + n1);
		إذا (this_ru_ستون + n2 > عرض)
		{
		    bufferlen = n1;
		    میانگیر[bufferlen] = NUL;
		    اكسر;
		}
	    }
	}
	وإلا إذا (this_ru_ستون + bufferlen > عرض)
	{
	    bufferlen = عرض - this_ru_ستون;
	    میانگیر[bufferlen] = NUL;
	}

	screen_puts(میانگیر, ردیف, this_ru_ستون + off, attr);
	n1 = redraw_cmdline;
	screen_fill(ردیف, ردیف + 1,
		this_ru_ستون + off + bufferlen,
		(off + عرض),
		fillchar, fillchar, attr);
	// don't redraw the cmdline because of showing the ruler
	redraw_cmdline = n1;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_خالی = خالی_سطر;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_سطر_شمار = wp->w_میانگیر->b_ml.ml_سطر_شمار;
#اگر_تعریف_شده FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#پایان_اگر

	إذا (override_success)
	    برداشت_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    فراغ
after_updating_screen(صحيح may_resize_shell استفادهنشده)
{
    updating_screen = FALSE;
#اگر_تعریف_شده FEAT_GUI
    إذا (may_resize_shell)
	gui_may_resize_shell();
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
    جمله_check_channel_closed_recently();
#پایان_اگر

#اگر_تعریف_شده HAVE_DROP_FILE
    // If handle_drop() was called while updating_screen was TRUE need to
    // handle the drop now.
    handle_any_postponed_drop();
#پایان_اگر
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
متن_مقصد_screenline(win_T *wp, char_u *متن, صحيح ستون)
{
    صحيح		off = (صحيح)(current_Screen_سطر - Screen_سطرها);

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
	    idx = off + ستون;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	لكل (p = متن; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    إذا (ستون + cells > wp->w_عرض)
		اكسر;
# ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		idx = off + wp->w_عرض - ستون - cells;
# endif
	    Screen_سطرها[idx] = *p;
	    إذا (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		إذا (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_سطرها_UC[idx] = 0;
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
			Screen_سطرها[idx] = firstbyte;
		    }
		    وإلا
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_سطرها_UC[idx] = u8c;
		    لكل (i = 0; i < Screen_mco; ++i)
		    {
			Screen_سطرها_C[i][idx] = u8cc[i];
			إذا (u8cc[i] == 0)
			    اكسر;
		    }
		}
		إذا (cells > 1)
		    Screen_سطرها[idx + 1] = 0;
	    }
	    وإلا إذا (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    وإلا إذا (cells > 1)
		// double-width character
		Screen_سطرها[idx + 1] = p[1];
	    ستون += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    وإلا
    {
	صحيح len = (صحيح)STRLEN(متن);
	صحيح n = wp->w_عرض - ستون;

	إذا (len > n)
	    len = n;
	إذا (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		mch_memmove(current_Screen_سطر, متن, len);
	    وإلا
# endif
		mch_memmove(current_Screen_سطر + ستون, متن, len);
	    ستون += len;
	}
    }
    أرجع ستون;
}
#پایان_اگر

#اگر_تعریف_شده FEAT_MENU
/*
 * Draw the window toolbar.
 */
    ساكن فراغ
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    صحيح		قلم_idx = 0;
    صحيح		قلم_شمار = 0;
    صحيح		ستون = 0;
    صحيح		بعدی_ستون;
    صحيح		off = (صحيح)(current_Screen_سطر - Screen_سطرها);
    bool	override_success =
	افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);
    صحيح		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    صحيح		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_اقلام);
    FOR_ALL_فرزند_MENUS(wp->w_winbar, menu)
	++قلم_شمار;
    wp->w_winbar_اقلام = ALLOC_CLEAR_MULT(winbar_قلم_T, قلم_شمار + 1);

    // TODO: use fewer spaces if there is not enough room
    لكل (menu = wp->w_winbar->children;
			  menu != عدم && ستون < wp->w_عرض; menu = menu->بعدی)
    {
	space_مقصد_screenline(off + ستون, fill_attr);
	إذا (++ستون >= wp->w_عرض)
	    اكسر;
	إذا (ستون > 1)
	{
	    space_مقصد_screenline(off + ستون, fill_attr);
	    إذا (++ستون >= wp->w_عرض)
		اكسر;
	}

	wp->w_winbar_اقلام[قلم_idx].wb_startcol = ستون;
	space_مقصد_screenline(off + ستون, button_attr);
	إذا (++ستون >= wp->w_عرض)
	    اكسر;

	بعدی_ستون = متن_مقصد_screenline(wp, menu->name, ستون);
	طالما (ستون < بعدی_ستون)
	{
	    ScreenAttrs[off + ستون] = button_attr;
	    ++ستون;
	}
	wp->w_winbar_اقلام[قلم_idx].wb_endcol = ستون;
	wp->w_winbar_اقلام[قلم_idx].wb_menu = menu;
	++قلم_idx;

	إذا (ستون >= wp->w_عرض)
	    اكسر;
	space_مقصد_screenline(off + ستون, button_attr);
	++ستون;
    }
    طالما (ستون < wp->w_عرض)
    {
	space_مقصد_screenline(off + ستون, fill_attr);
	++ستون;
    }
    wp->w_winbar_اقلام[قلم_idx].wb_menu = عدم; // end marker

    screen_سطر(wp, wp->w_winrow, wp->w_wincol, wp->w_عرض,
							  wp->w_عرض, -1, 0);

    إذا (override_success)
	برداشت_highlight_overrides();
}
#پایان_اگر

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

    mch_memmove(Screen_سطرها + off, buf, (size_t)len);
    إذا (enc_utf8)
	vim_memset(Screen_سطرها_UC + off, 0, حجم(u8char_T) * (size_t)len);
    لكل (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    ساكن فراغ
fold_سطر(
    win_T	*wp,
    دراز	fold_شمار,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    صحيح		ردیف)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*بالاترین, *bot;
    linenr_T	lnume = lnum + fold_شمار - 1;
    صحيح		len;
    char_u	*متن;
    صحيح		fdc;
    صحيح		ستون;
    صحيح		txtcol;
    صحيح		off = (صحيح)(current_Screen_سطر - Screen_سطرها);
    صحيح		ri;

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
    إذا (wp == cmdwin_win)
    {
	Screen_سطرها[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	إذا (enc_utf8)
	    Screen_سطرها_UC[off] = 0;
	++ستون;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    افعل { \
	إذا (wp->w_p_rl) \
	    لكل (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_عرض - (p) - (l)) + ri] = v; \
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
    fdc = compute_foldcolumn(wp, ستون);
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
		idx = off + wp->w_عرض - i - 1 - ستون;
	    وإلا
# endif
		idx = off + ستون + i;
	    إذا (enc_utf8)
	    {
		إذا (ch >= 0x80)
		{
		    Screen_سطرها_UC[idx] = ch;
		    Screen_سطرها_C[0][idx] = 0;
		    Screen_سطرها[idx] = 0x80;
		}
		وإلا
		{
		    Screen_سطرها[idx] = ch;
		    Screen_سطرها_UC[idx] = 0;
		}
	    }
	    وإلا
		Screen_سطرها[idx] = ch;
	}

	RL_MEMSET(ستون, HL_ATTR(HLF_FC), fdc);
	ستون += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    //متن
    RL_MEMSET(ستون, HL_ATTR(HLF_FL), wp->w_عرض - ستون);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    إذا (signcolumn_on(wp))
    {
	len = wp->w_عرض - ستون;
	إذا (len > 0)
	{
	    إذا (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		// the line number isn't reversed
		copy_متن_attr(off + wp->w_عرض - len - ستون,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    وإلا
#  endif
		copy_متن_attr(off + ستون, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    ستون += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    إذا (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_عرض - ستون;
	إذا (len > 0)
	{
	    صحيح	    w = عدد_عرض(wp);
	    دراز    رقم;
	    حرف    *fmt = "%*ld ";

	    إذا (len > w + 1)
		len = w + 1;

	    إذا (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		رقم = (دراز)lnum;
	    وإلا
	    {
		// 'relativenumber', don't use negative numbers
		رقم = labs((دراز)get_cursor_rel_lnum(wp, lnum));
		إذا (رقم == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    رقم = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((حرف *)buf, حجم(buf), fmt, w, رقم);
# ifdef FEAT_RIGHTLEFT
	    إذا (wp->w_p_rl)
		// the line number isn't reversed
		copy_متن_attr(off + wp->w_عرض - len - ستون, buf, len,
							     HL_ATTR(HLF_FL));
	    وإلا
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
    طالما (ستون < wp->w_عرض)
    {
	صحيح c = wp->w_fill_chars.fold;
	صحيح idx = off + ستون;

# ifdef FEAT_RIGHTLEFT
	إذا (wp->w_p_rl)
	    idx = off + wp->w_عرض - 1 - ستون;
# endif

	إذا (enc_utf8)
	{
	    إذا (c >= 0x80)
	    {
		Screen_سطرها_UC[idx] = c;
		Screen_سطرها_C[0][idx] = 0;
		Screen_سطرها[idx] = 0x80; // avoid storing zero
	    }
	    وإلا
	    {
		Screen_سطرها_UC[idx] = 0;
		Screen_سطرها[idx] = c;
	    }
	}
	وإلا
	    Screen_سطرها[idx] = c;
	++ستون;
    }

    إذا (متن != buf)
	vim_free(متن);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    إذا (VIsual_فعال && wp->w_میانگیر == curwin->w_میانگیر)
    {
	إذا (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    بالاترین = &curwin->w_cursor;
	    bot = &VIsual;
	}
	وإلا
	{
	    // Visual is before curwin->w_cursor
	    بالاترین = &VIsual;
	    bot = &curwin->w_cursor;
	}
	إذا (lnum >= بالاترین->lnum
		&& lnume <= bot->lnum
		&& (VIsual_نما != 'v'
		    || ((lnum > بالاترین->lnum
			    || (lnum == بالاترین->lnum
				&& بالاترین->ستون == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->ستون - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_میانگیر, lnume))))))
	{
	    إذا (VIsual_نما == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		إذا (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_عرض)
		{
		    إذا (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_عرض)
			len = wp->w_old_cursor_lcol;
		    وإلا
			len = wp->w_عرض - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (صحيح)wp->w_old_cursor_fcol);
		}
	    }
	    وإلا
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_عرض - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    إذا (wp->w_p_cc_تعداد_ستون)
    {
	صحيح i = 0;
	صحيح j = wp->w_p_cc_تعداد_ستون[i];
	صحيح old_txtcol = txtcol;

	طالما (j > -1)
	{
	    txtcol += j;
	    إذا (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    وإلا
		txtcol -= wp->w_leftcol;
	    إذا (txtcol >= 0 && txtcol < wp->w_عرض)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_تعداد_ستون[++i];
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
	إذا (txtcol >= 0 && txtcol < wp->w_عرض)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_سطر(wp, ردیف + W_WINROW(wp), wp->w_wincol, wp->w_عرض, wp->w_عرض,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    إذا (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_ردیف = ردیف;
	curwin->w_cline_ارتفاع = 1;
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
#پایان_اگر

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
    buf_T	*buf = wp->w_میانگیر;
    صحيح		type;
    صحيح		بالاترین_انتها = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    صحيح		mid_شروع = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    صحيح		mid_انتها = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    صحيح		bot_شروع = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    صحيح		scrolled_پایین = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    صحيح		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //سطرها
#اگر_تعریف_شده FEAT_جستجو_EXTRA
    صحيح		بالاترین_مقصد_mod = FALSE;    // redraw above mod_top
#پایان_اگر

    صحيح		ردیف;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    صحيح		idx;		// current index in w_lines[]
    صحيح		srow;		// starting row of the current line

    صحيح		eof = FALSE;	// if TRUE, we hit the end of the file
    صحيح		didline = FALSE; // if TRUE, we finished the last line
    صحيح		i;
    دراز	j;
    ساكن صحيح	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#اگر_تعریف_شده FEAT_CONCEAL
    صحيح		old_wrow = wp->w_wrow;
    صحيح		old_wcol = wp->w_wcol;
#پایان_اگر
#اگر_تعریف_شده FEAT_FOLDING
    دراز	fold_شمار;
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_سطر 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    صحيح		did_update = DID_NONE;
    linenr_T	syntax_آخرین_parsed = 0;		// last parsed text line
#پایان_اگر
    linenr_T	mod_بالاترین = 0;
    linenr_T	mod_bot = 0;
#إذا defined(FEAT_SYN_HL) || defined(FEAT_جستجو_EXTRA)
    صحيح		save_got_int;
#پایان_اگر
    bool	override_success;

#إذا defined(FEAT_جستجو_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    إذا (!did_update_one_پنجره)
    {
	did_update_one_پنجره = TRUE;
# ifdef FEAT_جستجو_EXTRA
	شروع_جستجو_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	إذا (clip_ستاره.موجود && clip_isautosel_ستاره())
	    clip_update_selection(&clip_ستاره);
	إذا (clip_plus.موجود && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#پایان_اگر

#اگر_تعریف_شده FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    إذا (wp->w_p_cuc)
	validate_virtcol_win(wp);
#پایان_اگر

    type = wp->w_redr_type;

    إذا (type == UPD_NOT_VALID)
    {
	wp->w_redr_وضعیت = true;
	wp->w_سطرها_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    إذا (wp->w_ارتفاع + WINBAR_ارتفاع(wp) == 0
	    || (wp->w_قاب->fr_ارتفاع == wp->w_وضعیت_ارتفاع
#إذا defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#پایان_اگر
	       ))
    {
	wp->w_redr_type = 0;
	أرجع;
    }

    // Window is zero-width: Only need to draw the separator.
    إذا (wp->w_عرض == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	أرجع;
    }

    override_success = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);


#اگر_تعریف_شده FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    إذا (جمله_do_update_پنجره(wp))
    {
	جمله_update_پنجره(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	إذا (winbar_ارتفاع(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	إذا (override_success)
	    برداشت_highlight_overrides();
	أرجع;
    }
#پایان_اگر

#اگر_تعریف_شده FEAT_جستجو_EXTRA
    init_جستجو_hl(wp, &screen_جستجو_hl);
#پایان_اگر

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    إذا (wp->w_skipcol > 0 && wp->w_عرض > win_ستون_off(wp))
    {
	صحيح w = 0;
	صحيح width1 = wp->w_عرض - win_ستون_off(wp);
	صحيح width2 = width1 + win_ستون_off2(wp);
	صحيح جمع_کن = width1;

	طالما (w < wp->w_skipcol)
	{
	    إذا (w > 0)
		جمع_کن = width2;
	    w += جمع_کن;
	}
	إذا (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - جمع_کن;
    }

#اگر_تعریف_شده FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? عدد_عرض(wp) : 0;
    إذا (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    وإلا
#پایان_اگر

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_بالاترین = wp->w_redraw_بالاترین;
	إذا (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	وإلا
	    mod_bot = 0;
	إذا (buf->b_mod_مجموعه)
	{
	    إذا (mod_بالاترین == 0 || mod_بالاترین > buf->b_mod_بالاترین)
	    {
		mod_بالاترین = buf->b_mod_بالاترین;
#اگر_تعریف_شده FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		إذا (syntax_present(wp))
		{
		    mod_بالاترین -= buf->b_s.b_syn_sync_linebreaks;
		    إذا (mod_بالاترین < 1)
			mod_بالاترین = 1;
		}
#پایان_اگر
	    }
	    إذا (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#اگر_تعریف_شده FEAT_جستجو_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    إذا (screen_جستجو_hl.rm.regprog != عدم
		    && re_multiline(screen_جستجو_hl.rm.regprog))
		بالاترین_مقصد_mod = TRUE;
	    وإلا
	    {
		matchitem_T *cur = wp->w_تطابق_سر;

		طالما (cur != عدم)
		{
		    إذا (cur->mit_تطابق.regprog != عدم
				       && re_multiline(cur->mit_تطابق.regprog))
		    {
			بالاترین_مقصد_mod = TRUE;
			اكسر;
		    }
		    cur = cur->mit_بعدی;
		}
	    }
#پایان_اگر
	}

#اگر_تعریف_شده FEAT_جستجو_EXTRA
	إذا (جستجو_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    إذا (mod_بالاترین == 0 || mod_بالاترین > جستجو_hl_has_cursor_lnum)
		mod_بالاترین = جستجو_hl_has_cursor_lnum;
	    إذا (mod_bot == 0 || mod_bot < جستجو_hl_has_cursor_lnum + 1)
		mod_bot = جستجو_hl_has_cursor_lnum + 1;
	}
#پایان_اگر

#اگر_تعریف_شده FEAT_FOLDING
	إذا (mod_بالاترین != 0 && hasAnyFolding(wp))
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
	    لكل (i = 0; i < wp->w_سطرها_valid; ++i)
		إذا (wp->w_سطرها[i].wl_valid)
		{
		    إذا (wp->w_سطرها[i].wl_lastlnum < mod_بالاترین)
			lnumt = wp->w_سطرها[i].wl_lastlnum + 1;
		    إذا (lnumb == MAXLNUM && wp->w_سطرها[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_سطرها[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			إذا (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (فراغ)hasFoldingWin(wp, mod_بالاترین, &mod_بالاترین, عدم, TRUE, عدم);
	    إذا (mod_بالاترین > lnumt)
		mod_بالاترین = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (فراغ)hasFoldingWin(wp, mod_bot, عدم, &mod_bot, TRUE, عدم);
	    ++mod_bot;
	    إذا (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#پایان_اگر

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	إذا (mod_بالاترین != 0 && mod_بالاترین < wp->w_topline)
	{
	    إذا (mod_bot > wp->w_topline)
		mod_بالاترین = wp->w_topline;
#اگر_تعریف_شده FEAT_SYN_HL
	    وإلا إذا (syntax_present(wp))
		بالاترین_انتها = 1;
#پایان_اگر
	}
    }
    wp->w_redraw_بالاترین = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#اگر_تعریف_شده FEAT_جستجو_EXTRA
    جستجو_hl_has_cursor_lnum = 0;
#پایان_اگر


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    إذا (type == UPD_REDRAW_بالاترین)
    {
	j = 0;
	لكل (i = 0; i < wp->w_سطرها_valid; ++i)
	{
	    j += wp->w_سطرها[i].wl_اندازه;
	    إذا (j >= wp->w_upd_ردیفها)
	    {
		بالاترین_انتها = j;
		اكسر;
	    }
	}
	إذا (بالاترین_انتها == 0)
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
#اگر_تعریف_شده FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#پایان_اگر
	    )
    {
	إذا (mod_بالاترین != 0
		&& wp->w_topline == mod_بالاترین
		&& (!wp->w_سطرها[0].wl_valid
		    || wp->w_topline == wp->w_سطرها[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	وإلا إذا (wp->w_سطرها[0].wl_valid
		&& (wp->w_topline < wp->w_سطرها[0].wl_lnum
#اگر_تعریف_شده FEAT_DIFF
		    || (wp->w_topline == wp->w_سطرها[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#پایان_اگر
		   ))
	{
	    // New topline is above old topline: May scroll down.
#اگر_تعریف_شده FEAT_FOLDING
	    إذا (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		لكل (ln = wp->w_topline; ln < wp->w_سطرها[0].wl_lnum; ++ln)
		{
		    ++j;
		    إذا (j >= wp->w_ارتفاع - 2)
			اكسر;
		    (فراغ)hasFoldingWin(wp, ln, عدم, &ln, TRUE, عدم);
		}
	    }
	    وإلا
#پایان_اگر
		j = wp->w_سطرها[0].wl_lnum - wp->w_topline;
	    إذا (j < wp->w_ارتفاع - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_سطرها[0].wl_lnum - 1,
								wp->w_ارتفاع);
#اگر_تعریف_شده FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		إذا (wp->w_سطرها[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_سطرها[0].wl_lnum)
							  - wp->w_old_topfill;
#پایان_اگر
		إذا (i < wp->w_ارتفاع - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    إذا (i > 0)
			check_for_delay(FALSE);
		    إذا (win_ins_سطرها(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			إذا (wp->w_سطرها_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    بالاترین_انتها = i;
			    scrolled_پایین = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    إذا ((wp->w_سطرها_valid += j) > wp->w_ارتفاع)
				wp->w_سطرها_valid = wp->w_ارتفاع;
			    لكل (idx = wp->w_سطرها_valid; idx - j >= 0; idx--)
				wp->w_سطرها[idx] = wp->w_سطرها[idx - j];
			    طالما (idx >= 0)
				wp->w_سطرها[idx--].wl_valid = FALSE;
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
	    ردیف = 0;
	    لكل (i = 0; i < wp->w_سطرها_valid && i < ردیفها; i++)
	    {
		إذا (wp->w_سطرها[i].wl_valid
			&& wp->w_سطرها[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    اكسر;
		}
		ردیف += wp->w_سطرها[i].wl_اندازه;
	    }
	    إذا (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//سطرها
		mid_شروع = 0;
	    }
	    وإلا
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#اگر_تعریف_شده FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		إذا (wp->w_سطرها[0].wl_lnum == wp->w_topline)
		    ردیف += wp->w_old_topfill;
		وإلا
		    ردیف += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		ردیف -= wp->w_topfill;
#پایان_اگر
		إذا (ردیف > ردیفها)  // just in case
		    ردیف = ردیفها;
		إذا (ردیف > 0)
		{
		    check_for_delay(FALSE);
		    إذا (win_del_سطرها(wp, 0, ردیف, FALSE, wp == firstwin, 0)
									 == OK)
			bot_شروع = wp->w_ارتفاع - ردیف;
		    وإلا
			mid_شروع = 0;		// redraw all lines
		}
		إذا ((ردیف == 0 || bot_شروع < 999) && wp->w_سطرها_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_شروع = 0;
		    idx = 0;
		    لكل (;;)
		    {
			wp->w_سطرها[idx] = wp->w_سطرها[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			إذا (ردیف > 0 && bot_شروع + ردیف
				 + (صحيح)wp->w_سطرها[j].wl_اندازه > wp->w_ارتفاع)
			{
			    wp->w_سطرها_valid = idx + 1;
			    اكسر;
			}
			bot_شروع += wp->w_سطرها[idx++].wl_اندازه;

			// stop at the last valid entry in w_lines[].wl_size
			إذا (++j >= wp->w_سطرها_valid)
			{
			    wp->w_سطرها_valid = idx;
			    اكسر;
			}
		    }
#اگر_تعریف_شده FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    إذا (wp->w_p_diff && bot_شروع > 0)
			wp->w_سطرها[0].wl_اندازه = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#پایان_اگر
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	إذا (mid_شروع == 0)
	    mid_انتها = wp->w_ارتفاع;

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
	mid_انتها = wp->w_ارتفاع;
    }

    إذا (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_شروع = 0;
	mid_انتها = wp->w_ارتفاع;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    إذا ((VIsual_فعال && buf == curwin->w_میانگیر)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, مقصد;

	إذا (VIsual_فعال)
	{
	    إذا (VIsual_نما != wp->w_old_visual_نما
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		إذا (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    مقصد = VIsual.lnum;
		}
		وإلا
		{
		    from = VIsual.lnum;
		    مقصد = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		إذا (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		إذا (wp->w_old_cursor_lnum > مقصد)
		    مقصد = wp->w_old_cursor_lnum;
		إذا (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		إذا (wp->w_old_visual_lnum > مقصد)
		    مقصد = wp->w_old_visual_lnum;
	    }
	    وإلا
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		إذا (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    مقصد = wp->w_old_cursor_lnum;
		}
		وإلا
		{
		    from = wp->w_old_cursor_lnum;
		    مقصد = curwin->w_cursor.lnum;
		    إذا (from == 0)	// Visual mode just started
			from = مقصد;
		}

		إذا (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.ستون != wp->w_old_visual_ستون)
		{
		    إذا (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    إذا (wp->w_old_visual_lnum > مقصد)
			مقصد = wp->w_old_visual_lnum;
		    إذا (VIsual.lnum < from)
			from = VIsual.lnum;
		    إذا (VIsual.lnum > مقصد)
			مقصد = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    إذا (VIsual_نما == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_انتها_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		إذا (curwin->w_curswant == MAXCOL)
		{
		    إذا (get_ve_flags() & VE_بلوک)
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

			    pos.ستون = (صحيح)ml_get_buf_len(wp->w_میانگیر, pos.lnum);
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
		    إذا (مقصد < VIsual.lnum)
			مقصد = VIsual.lnum;
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
		مقصد = wp->w_old_visual_lnum;
	    }
	    وإلا
	    {
		from = wp->w_old_visual_lnum;
		مقصد = wp->w_old_cursor_lnum;
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
	    مقصد = MIN(مقصد, wp->w_botline);
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
	    إذا (scrolled_پایین)
		mid_شروع = بالاترین_انتها;
	    وإلا
		mid_شروع = 0;
	    طالما (lnum < from && idx < wp->w_سطرها_valid)	// find start
	    {
		إذا (wp->w_سطرها[idx].wl_valid)
		    mid_شروع += wp->w_سطرها[idx].wl_اندازه;
		وإلا إذا (!scrolled_پایین)
		    srow += wp->w_سطرها[idx].wl_اندازه;
		++idx;
#اگر_تعریف_شده FEAT_FOLDING
		إذا (idx < wp->w_سطرها_valid && wp->w_سطرها[idx].wl_valid)
		    lnum = wp->w_سطرها[idx].wl_lnum;
		وإلا
#پایان_اگر
		    ++lnum;
	    }
	    srow += mid_شروع;
	    mid_انتها = wp->w_ارتفاع;
	    لكل ( ; idx < wp->w_سطرها_valid; ++idx)		// find end
	    {
		إذا (wp->w_سطرها[idx].wl_valid
			&& wp->w_سطرها[idx].wl_lnum >= مقصد + 1)
		{
		    // Only update until first row of this line
		    mid_انتها = srow;
		    اكسر;
		}
		srow += wp->w_سطرها[idx].wl_اندازه;
	    }
	}
    }

    إذا (VIsual_فعال && buf == curwin->w_میانگیر)
    {
	wp->w_old_visual_نما = VIsual_نما;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_ستون = VIsual.ستون;
	wp->w_old_curswant = curwin->w_curswant;
    }
    وإلا
    {
	wp->w_old_visual_نما = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_ستون = 0;
    }

#إذا defined(FEAT_SYN_HL) || defined(FEAT_جستجو_EXTRA)
    // reset got_int, otherwise regexp won't work
    save_got_int = got_int;
    got_int = 0;
#پایان_اگر
#اگر_تعریف_شده SYN_زمان_حد
    // Set the time limit to 'redrawtime'.
    redrawtime_حد_مجموعه = TRUE;
    init_regexp_timeout(p_rdt);
#پایان_اگر
#اگر_تعریف_شده FEAT_FOLDING
    win_foldinfo.fi_level = 0;
#پایان_اگر

#اگر_تعریف_شده FEAT_MENU
    // Draw the window toolbar, if there is one.
    // TODO: only when needed.
    إذا (winbar_ارتفاع(wp) > 0)
	redraw_win_toolbar(wp);
#پایان_اگر

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#اگر_تعریف_شده FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_فیلد(spv);
    إذا (spell_check_پنجره(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_بالاترین == 0;
    }
#پایان_اگر

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    ردیف = 0;
    srow = 0;
    لكل (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	إذا (ردیف == wp->w_ارتفاع)
	{
	    didline = TRUE;
	    اكسر;
	}

	// stop updating when hit the end of the file
	إذا (lnum > buf->b_ml.ml_سطر_شمار)
	{
	    eof = TRUE;
	    اكسر;
	}

	// Remember the starting row of the line that is going to be dealt
	// with.  It is used further down when the line doesn't fit.
	srow = ردیف;

	// Update a line when it is in an area that needs updating, when it
	// has changes or w_lines[idx] is invalid.
	// "bot_start" may be halfway a wrapped line after using
	// win_del_lines(), check if the current line includes it.
	// When syntax folding is being used, the saved syntax states will
	// already have been updated, we can't see where the syntax state is
	// the same again, just update until the end of the window.
	إذا (ردیف < بالاترین_انتها
		|| (ردیف >= mid_شروع && ردیف < mid_انتها)
#اگر_تعریف_شده FEAT_جستجو_EXTRA
		|| بالاترین_مقصد_mod
#پایان_اگر
		|| idx >= wp->w_سطرها_valid
		|| (ردیف + wp->w_سطرها[idx].wl_اندازه > bot_شروع)
		|| (mod_بالاترین != 0
		    && (lnum == mod_بالاترین
			|| (lnum >= mod_بالاترین
			    && (lnum < mod_bot
#اگر_تعریف_شده FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_سطر
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_تغییریافته(lnum)))
#پایان_اگر
#اگر_تعریف_شده FEAT_جستجو_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_تطابق_سر != عدم
						    && buf->b_mod_مجموعه
						    && buf->b_mod_xlines != 0)
#پایان_اگر
				))))
#اگر_تعریف_شده FEAT_SYN_HL
		|| (wp->w_p_cul && lnum == wp->w_cursor.lnum)
		|| lnum == wp->w_آخرین_cursorline
#پایان_اگر
				)
	{
#اگر_تعریف_شده FEAT_جستجو_EXTRA
	    إذا (lnum == mod_بالاترین)
		بالاترین_مقصد_mod = FALSE;
#پایان_اگر

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    إذا (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_بالاترین && lnum < MAX(mod_bot, mod_بالاترین + 1)
		    && (!scrolled_پایین || ردیف >= بالاترین_انتها))
	    {
		scrolled_for_mod = TRUE;

		صحيح		old_cline_ارتفاع = 0;
		صحيح		old_ردیفها = 0;
		صحيح		new_ردیفها = 0;
		صحيح		xtra_ردیفها;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		لكل (i = idx; i < wp->w_سطرها_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    إذا (wp->w_سطرها[i].wl_valid
			    && wp->w_سطرها[i].wl_lnum == mod_bot)
			اكسر;
		    إذا (wp->w_سطرها[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_ارتفاع = wp->w_سطرها[i].wl_اندازه;
		    old_ردیفها += wp->w_سطرها[i].wl_اندازه;
#اگر_تعریف_شده FEAT_FOLDING
		    إذا (wp->w_سطرها[i].wl_valid
			    && wp->w_سطرها[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			طالما (i < wp->w_سطرها_valid
						  && !wp->w_سطرها[i].wl_valid)
			    old_ردیفها += wp->w_سطرها[i++].wl_اندازه;
			اكسر;
		    }
#پایان_اگر
		}

		إذا (i >= wp->w_سطرها_valid)
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
				old_cline_ارتفاع > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_ردیفها += old_cline_ارتفاع;
#اگر_تعریف_شده FEAT_FOLDING
			وإلا إذا (hasFoldingWin(wp, l, عدم, &l, TRUE, عدم))
			    ++new_ردیفها;
#پایان_اگر
			وإلا
			    new_ردیفها += plines_correct_topline(wp, l, TRUE);
			++j;
			إذا (new_ردیفها > wp->w_ارتفاع - ردیف - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_ردیفها = 9999;
			    اكسر;
			}
		    }
		    xtra_ردیفها = new_ردیفها - old_ردیفها;
		    إذا (xtra_ردیفها < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			إذا (ردیف - xtra_ردیفها >= wp->w_ارتفاع - 2)
			    mod_bot = MAXLNUM;
			وإلا
			{
			    check_for_delay(FALSE);
			    إذا (win_del_سطرها(wp, ردیف,
					  -xtra_ردیفها, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    وإلا
				bot_شروع = wp->w_ارتفاع + xtra_ردیفها;
			}
		    }
		    وإلا إذا (xtra_ردیفها > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			إذا (ردیف + xtra_ردیفها >= wp->w_ارتفاع - 2)
			    mod_bot = MAXLNUM;
			وإلا
			{
			    check_for_delay(FALSE);
			    إذا (win_ins_سطرها(wp, ردیف + old_ردیفها,
					     xtra_ردیفها, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    وإلا إذا (بالاترین_انتها > ردیف + old_ردیفها)
				// Scrolled the part at the top that requires
				// updating down.
				بالاترین_انتها += xtra_ردیفها;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    إذا (mod_bot != MAXLNUM && i != j)
		    {
			إذا (j < i)
			{
			    صحيح x = ردیف + new_ردیفها;

			    // move entries in w_lines[] upwards
			    لكل (;;)
			    {
				// stop at last valid entry in w_lines[]
				إذا (i >= wp->w_سطرها_valid)
				{
				    wp->w_سطرها_valid = j;
				    اكسر;
				}
				wp->w_سطرها[j] = wp->w_سطرها[i];
				// stop at a line that won't fit
				إذا (x + (صحيح)wp->w_سطرها[j].wl_اندازه
							   > wp->w_ارتفاع)
				{
				    wp->w_سطرها_valid = j + 1;
				    اكسر;
				}
				x += wp->w_سطرها[j++].wl_اندازه;
				++i;
			    }
			    إذا (bot_شروع > x)
				bot_شروع = x;
			}
			وإلا // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_سطرها_valid += j;
			    إذا (wp->w_سطرها_valid > wp->w_ارتفاع)
				wp->w_سطرها_valid = wp->w_ارتفاع;
			    لكل (i = wp->w_سطرها_valid; i - j >= idx; --i)
				wp->w_سطرها[i] = wp->w_سطرها[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    طالما (i >= idx)
			    {
				wp->w_سطرها[i].wl_اندازه = 0;
				wp->w_سطرها[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#اگر_تعریف_شده FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_شمار = folded_شمار(wp, lnum, &win_foldinfo);
	    إذا (fold_شمار != 0)
	    {
		fold_سطر(wp, fold_شمار, &win_foldinfo, lnum, ردیف);
		++ردیف;
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
	    وإلا
#پایان_اگر
	    إذا (idx < wp->w_سطرها_valid
		    && wp->w_سطرها[idx].wl_valid
		    && wp->w_سطرها[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_سطرها[idx].wl_اندازه > wp->w_ارتفاع
#اگر_تعریف_شده FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#پایان_اگر
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		ردیف = wp->w_ارتفاع + 1;
	    }
	    وإلا
	    {
#اگر_تعریف_شده FEAT_جستجو_EXTRA
		prepare_جستجو_hl(wp, &screen_جستجو_hl, lnum);
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		إذا (syntax_آخرین_parsed != 0 && syntax_آخرین_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_انتها_parsing(wp, syntax_آخرین_parsed + 1);
#پایان_اگر

		// Display one line.
		ردیف = win_سطر(wp, lnum, srow, wp->w_ارتفاع, 0, &spv);

#اگر_تعریف_شده FEAT_FOLDING
		wp->w_سطرها[idx].wl_folded = FALSE;
		wp->w_سطرها[idx].wl_lastlnum = lnum;
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
		did_update = DID_سطر;
		syntax_آخرین_parsed = lnum;
#پایان_اگر
	    }

	    wp->w_سطرها[idx].wl_lnum = lnum;
	    wp->w_سطرها[idx].wl_valid = TRUE;

	    صحيح is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    إذا (ردیف > wp->w_ارتفاع || ردیف + wp->w_winrow >= ردیفها)
	    {
		// we may need the size of that too long line later on
		إذا (dollar_vcol == -1 || !is_curline)
		    wp->w_سطرها[idx].wl_اندازه = plines_win(wp, lnum, TRUE);
		++idx;
		اكسر;
	    }
	    إذا (dollar_vcol == -1 || !is_curline)
		wp->w_سطرها[idx].wl_اندازه = ردیف - srow;
	    ++idx;
#اگر_تعریف_شده FEAT_FOLDING
	    lnum += fold_شمار + 1;
#وإلا
	    ++lnum;
#پایان_اگر
	}
	وإلا
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    إذا ((wp->w_p_nu && mod_بالاترین != 0 && lnum >= mod_bot
			&& buf->b_mod_مجموعه && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_آخرین_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#اگر_تعریف_شده FEAT_FOLDING
		fold_شمار = folded_شمار(wp, lnum, &win_foldinfo);
		إذا (fold_شمار != 0)
		    fold_سطر(wp, fold_شمار, &win_foldinfo, lnum, ردیف);
		وإلا
#پایان_اگر
		    (فراغ)win_سطر(wp, lnum, srow, wp->w_ارتفاع,
					       wp->w_سطرها[idx].wl_اندازه, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    ردیف += wp->w_سطرها[idx++].wl_اندازه;
	    إذا (ردیف > wp->w_ارتفاع)	// past end of screen
		اكسر;
#اگر_تعریف_شده FEAT_FOLDING
	    lnum = wp->w_سطرها[idx - 1].wl_lastlnum + 1;
#وإلا
	    ++lnum;
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
	    did_update = DID_NONE;
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#پایان_اگر
	}

	إذا (lnum > buf->b_ml.ml_سطر_شمار)
	{
	    eof = TRUE;
	    اكسر;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	إذا (idx >= ردیفها)
	    اكسر;
    }

    // End of loop over all window lines.

#اگر_تعریف_شده FEAT_SYN_HL
    // Now that the window has been redrawn with the old and new cursor line,
    // update w_last_cursorline.
    wp->w_آخرین_cursorline = wp->w_p_cul ? wp->w_cursor.lnum : 0;
#پایان_اگر
    wp->w_آخرین_cursor_lnum_rnu = wp->w_p_rnu ? wp->w_cursor.lnum : 0;

#اگر_تعریف_شده FEAT_VTP
    // Rewrite the character at the end of the screen line.
    // See the version that was fixed.
    إذا (use_vtp() && get_conpty_fix_type() < 1)
    {
	صحيح k;

	لكل (k = 0; k < ردیفها; ++k)
	    إذا (enc_utf8)
		إذا ((*mb_off2cells)(سطر_جابه_جایی[k] + topframe->fr_عرض - 2,
					   سطر_جابه_جایی[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_عرض - 2, 1, 2,
			    FALSE, FALSE);
		وإلا
		    screen_draw_rectangle(k, topframe->fr_عرض - 1, 1, 1,
			    FALSE, FALSE);
	    وإلا
		screen_char(سطر_جابه_جایی[k] + topframe->fr_عرض - 1, k,
			cmdline_عرض - 1);
    }
#پایان_اگر

    إذا (idx > wp->w_سطرها_valid)
	wp->w_سطرها_valid = idx;

#اگر_تعریف_شده FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    إذا (syntax_آخرین_parsed != 0 && syntax_present(wp))
	syntax_انتها_parsing(wp, syntax_آخرین_parsed + 1);
#پایان_اگر

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_خالی_ردیفها = 0;
#اگر_تعریف_شده FEAT_DIFF
    wp->w_filler_ردیفها = 0;
#پایان_اگر
    إذا (!eof && !didline)
    {
	إذا (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#اگر_تعریف_شده FEAT_DIFF
	وإلا إذا (diff_check_fill(wp, lnum) >= wp->w_ارتفاع - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_ردیفها = wp->w_ارتفاع - srow;
	}
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
	وإلا إذا (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#پایان_اگر
	وإلا إذا (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    صحيح		scr_ردیف = W_WINROW(wp) + wp->w_ارتفاع - 1;
	    صحيح		نماد  = wp->w_fill_chars.lastline;
	    صحيح		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(نماد, &fillbuf[0]);
	    mb_char2bytes(نماد, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_عرض > 2 ? 2 : wp->w_عرض) * charlen,
			    scr_ردیف, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_ردیف, scr_ردیف + 1,
		    (صحيح)wp->w_wincol + 2, (صحيح)W_ENDCOL(wp),
		    نماد, ' ', HL_ATTR(HLF_AT));
	    مجموعه_خالی_ردیفها(wp, srow);
	    wp->w_botline = lnum;
	}
	وإلا إذا (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    صحيح شروع_ستون = (صحيح)W_ENDCOL(wp) - 3;
	    صحيح نماد    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_ارتفاع - 1,
		    W_WINROW(wp) + wp->w_ارتفاع,
		    (شروع_ستون < wp->w_wincol ? wp->w_wincol : شروع_ستون),
		    (صحيح)W_ENDCOL(wp),
		    نماد, نماد, HL_ATTR(HLF_AT));
	    مجموعه_خالی_ردیفها(wp, srow);
	    wp->w_botline = lnum;
	}
	وإلا
	{
	    win_draw_انتها(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_ارتفاع, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    وإلا
    {
	draw_vsep_win(wp, ردیف);
	إذا (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_سطر_شمار + 1;
#اگر_تعریف_شده FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    إذا (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		إذا (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		وإلا
		    i = wp->w_fill_chars.diff;
		إذا (ردیف + j > wp->w_ارتفاع)
		    j = wp->w_ارتفاع - ردیف;
		win_draw_انتها(wp, i, i, TRUE, ردیف, ردیف + (صحيح)j, HLF_DED);
		ردیف += j;
	    }
#پایان_اگر
	}
	وإلا إذا (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	إذا (WIN_IS_POPUP(wp))
	    win_draw_انتها(wp, ' ', ' ', FALSE, ردیف, wp->w_ارتفاع, HLF_AT);
	وإلا
	    win_draw_انتها(wp, wp->w_fill_chars.eob, ' ', FALSE,
						   ردیف, wp->w_ارتفاع, HLF_EOB);
    }

#اگر_تعریف_شده SYN_زمان_حد
    disable_regexp_timeout();
    redrawtime_حد_مجموعه = FALSE;
#پایان_اگر

    // Reset the type of redrawing required, the window has been updated.
    wp->w_redr_type = 0;
#اگر_تعریف_شده FEAT_DIFF
    wp->w_old_topfill = wp->w_topfill;
    wp->w_old_botfill = wp->w_botfill;
#پایان_اگر

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
#پایان_اگر
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
#پایان_اگر
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    إذا (wp->w_redr_type != 0)
	    {
		// Don't update for changes in buffer again.
		bool b = curbuf->b_mod_مجموعه;
		curbuf->b_mod_مجموعه = false;
		j = curbuf->b_mod_xlines;
		curbuf->b_mod_xlines = 0;
		curs_columns(TRUE);
		win_update(curwin);
		curbuf->b_mod_مجموعه = b;
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

#إذا defined(FEAT_SYN_HL) || defined(FEAT_جستجو_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    إذا (!got_int)
	got_int = save_got_int;
#پایان_اگر

    إذا (override_success)
	برداشت_highlight_overrides();
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
    إذا (gui.درون_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_جستجو_EXTRA
    شروع_جستجو_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_ماسک(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    ساكن فراغ
update_پایان(فراغ)
{
    إذا (redraw_cmdline || redraw_نما)
	showmode();

# ifdef FEAT_جستجو_EXTRA
    انتها_جستجو_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    إذا (gui.درون_use)
    {
	بیرون_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#پایان_اگر

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
	    || State == نما_ASKMORE || State == نما_HITRETURN
	    || پیام_scrolled
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
	إذا (wp->w_redr_وضعیت)
	    win_redr_وضعیت(wp, FALSE);
    }

# إذا defined(FEAT_TABPANEL)
    إذا (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_پایان();
}
#پایان_اگر

#إذا defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    فراغ
update_پنجره(win_T *wp)
{
    // return if already busy updating
    إذا (updating_screen)
	أرجع;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    إذا (clip_ستاره.موجود && clip_isautosel_ستاره())
	clip_update_selection(&clip_ستاره);
    إذا (clip_plus.موجود && clip_isautosel_plus())
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

    إذا (wp->w_redr_وضعیت || p_ru
# ifdef FEAT_STL_OPT
	    || *p_stl != NUL || *wp->w_p_stl != NUL
# endif
	    )
	win_redr_وضعیت(wp, FALSE);

# ifdef FEAT_PROP_POPUP
    // Display popup windows on top of everything.
    update_popups(win_update);
# endif

    update_پایان();
}
#پایان_اگر

/*
 * Redraw as soon as possible.  When the command line is not scrolled redraw
 * right away and restore what was on the command line.
 * Return a code indicating what happened.
 */
    صحيح
redraw_asap(صحيح type)
{
    صحيح		ردیفها_2;
    صحيح		تعداد_ستون = screen_Columns;
    صحيح		r;
    صحيح		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    صحيح		i;
    u8char_T	*screenlineUC = عدم;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = عدم;	// copy from ScreenLines2[]

    redraw_later(type);
    إذا (پیام_scrolled
	    || (State != نما_نرمال && State != نما_نرمال_BUSY)
	    || exiting)
	أرجع ret;

    // Allocate space to save the text displayed in the command line area.
    ردیفها_2 = screen_ردیفها - cmdline_ردیف;
    screenline = LALLOC_MULT(schar_T, ردیفها_2 * تعداد_ستون);
    screenattr = LALLOC_MULT(sattr_T, ردیفها_2 * تعداد_ستون);
    إذا (screenline == عدم || screenattr == عدم)
	ret = 2;
    إذا (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, ردیفها_2 * تعداد_ستون);
	إذا (screenlineUC == عدم)
	    ret = 2;
	لكل (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, ردیفها_2 * تعداد_ستون);
	    إذا (screenlineC[i] == عدم)
		ret = 2;
	}
    }
    إذا (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, ردیفها_2 * تعداد_ستون);
	إذا (screenline2 == عدم)
	    ret = 2;
    }

    إذا (ret != 2)
    {
	// Save the text displayed in the command line area.
	لكل (r = 0; r < ردیفها_2; ++r)
	{
	    mch_memmove(screenline + r * تعداد_ستون,
			Screen_سطرها + سطر_جابه_جایی[cmdline_ردیف + r],
			(size_t)تعداد_ستون * حجم(schar_T));
	    mch_memmove(screenattr + r * تعداد_ستون,
			ScreenAttrs + سطر_جابه_جایی[cmdline_ردیف + r],
			(size_t)تعداد_ستون * حجم(sattr_T));
	    إذا (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * تعداد_ستون,
			    Screen_سطرها_UC + سطر_جابه_جایی[cmdline_ردیف + r],
			    (size_t)تعداد_ستون * حجم(u8char_T));
		لكل (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * تعداد_ستون,
				Screen_سطرها_C[i] + سطر_جابه_جایی[cmdline_ردیف + r],
				(size_t)تعداد_ستون * حجم(u8char_T));
	    }
	    إذا (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * تعداد_ستون,
			    ScreenLines2 + سطر_جابه_جایی[cmdline_ردیف + r],
			    (size_t)تعداد_ستون * حجم(schar_T));
	}

	update_screen(0);
	ret = 3;

	إذا (must_redraw == 0)
	{
	    صحيح	off = (صحيح)(current_Screen_سطر - Screen_سطرها);

	    // Restore the text displayed in the command line area.
	    لكل (r = 0; r < ردیفها_2; ++r)
	    {
		mch_memmove(current_Screen_سطر,
			    screenline + r * تعداد_ستون,
			    (size_t)تعداد_ستون * حجم(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * تعداد_ستون,
			    (size_t)تعداد_ستون * حجم(sattr_T));
		إذا (enc_utf8)
		{
		    mch_memmove(Screen_سطرها_UC + off,
				screenlineUC + r * تعداد_ستون,
				(size_t)تعداد_ستون * حجم(u8char_T));
		    لكل (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_سطرها_C[i] + off,
				    screenlineC[i] + r * تعداد_ستون,
				    (size_t)تعداد_ستون * حجم(u8char_T));
		}
		إذا (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * تعداد_ستون,
				(size_t)تعداد_ستون * حجم(schar_T));
		screen_سطر(curwin, cmdline_ردیف + r, 0, تعداد_ستون, تعداد_ستون, -1, 0);
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
    maybe_intro_پیام();

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
redraw_after_callback(صحيح call_update_screen, صحيح do_پیام_2)
{
    ++redrawing_for_callback;

    جمله_مجموعه_sync_output(جمله_SYNC_OUTPUT_ENABLE);

    إذا (State == نما_HITRETURN || State == نما_ASKMORE
	    || State == نما_SETWSIZE || State == نما_EXTERNCMD
	    || State == نما_CONFIRM || exmode_فعال)
    {
	إذا (do_پیام_2)
	    repeat_پیام();
    }
    وإلا إذا (State & نما_CMDLINE)
    {
	إذا (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	إذا (cmdline_ردیف > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    إذا (پیام_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    وإلا إذا (State & (نما_نرمال | نما_درج | نما_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	إذا (پیام_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    پیام_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    جمله_مجموعه_sync_output(جمله_SYNC_OUTPUT_DISABLE);
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	بیرون_flush_cursor(FALSE, FALSE);
    وإلا
#پایان_اگر
	بیرون_flush();

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
#اگر_تعریف_شده FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    إذا (inside_redraw_on_شروع_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    وإلا
#پایان_اگر
    إذا (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	إذا (type >= UPD_NOT_VALID)
	    wp->w_سطرها_valid = 0;
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
    مجموعه_must_redraw(type);
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
#پایان_اگر

/*
 * Set "must_redraw" to "type" unless it already has a higher value
 * or it is currently not allowed.
 */
    فراغ
مجموعه_must_redraw(صحيح type)
{
    إذا (!redraw_not_allowed &&
#اگر_تعریف_شده FEAT_EVAL
	    !inside_redraw_on_شروع_cb &&
#پایان_اگر
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
	إذا (wp->w_میانگیر == buf)
	    redraw_win_later(wp, type);
    }
#اگر_تعریف_شده FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_میانگیر == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا (wp->w_میانگیر == buf)
	    redraw_win_later(wp, type);
#پایان_اگر
#إذا defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    إذا (curwin->w_میانگیر == buf)
	redraw_win_later(curwin, type);
#پایان_اگر
}

#إذا defined(FEAT_SIGNS)
    فراغ
redraw_buf_سطر_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_میانگیر == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#پایان_اگر

#إذا defined(FEAT_کار_CHANNEL)
    فراغ
redraw_buf_and_وضعیت_later(buf_T *buf, صحيح type)
{
    win_T	*wp;

    إذا (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	أرجع;
    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_میانگیر == buf)
	{
	    redraw_win_later(wp, type);
	    wp->w_redr_وضعیت = true;
	}
    }
}
#پایان_اگر

/*
 * mark the ruler for redraw when the last window has no status line and the
 * ruler takes its place in the last screen line; showmode() draws it
 */
    ساكن فراغ
ruler_redraw_lastwin(فراغ)
{
    إذا (p_ru && lastwin->w_وضعیت_ارتفاع == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    فراغ
وضعیت_redraw_all(فراغ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_وضعیت_ارتفاع)
	{
	    wp->w_redr_وضعیت = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    فراغ
وضعیت_redraw_curbuf(فراغ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_وضعیت_ارتفاع != 0 && wp->w_میانگیر == curbuf)
	{
	    wp->w_redr_وضعیت = true;
	    redraw_later(UPD_VALID);
	}
    إذا (lastwin->w_میانگیر == curbuf)
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
	إذا (wp->w_redr_وضعیت)
	{
	    bool ret = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_وضعیت(wp, FALSE);
	    إذا (ret)
		برداشت_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    إذا (redraw_tabline)
	draw_tabline();

#إذا defined(FEAT_TABPANEL)
    إذا (redraw_tabpanel)
	draw_tabpanel();
#پایان_اگر
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    فراغ
win_redraw_آخرین_وضعیت(قاب_T *frp)
{
    إذا (frp->fr_layout == FR_برگ)
	frp->fr_win->w_redr_وضعیت = true;
    وإلا إذا (frp->fr_layout == FR_ردیف)
    {
	FOR_ALL_قابها(frp, frp->fr_فرزند)
	    win_redraw_آخرین_وضعیت(frp);
    }
    وإلا // frp->fr_layout == FR_COL
    {
	frp = frp->fr_فرزند;
	طالما (frp->fr_بعدی != عدم)
	    frp = frp->fr_بعدی;
	win_redraw_آخرین_وضعیت(frp);
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
    linenr_T	اولی,
    linenr_T	آخرین)
{
    إذا (آخرین >= wp->w_topline && اولی < wp->w_botline)
    {
	إذا (wp->w_redraw_بالاترین == 0 || wp->w_redraw_بالاترین > اولی)
	    wp->w_redraw_بالاترین = اولی;
	إذا (wp->w_redraw_bot == 0 || wp->w_redraw_bot < آخرین)
	    wp->w_redraw_bot = آخرین;
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
    linenr_T	اولی,
    linenr_T	آخرین)
{
    إذا (آخرین >= wp->w_topline && اولی < wp->w_botline)
    {
	إذا (wp->w_redraw_بالاترین == 0 || wp->w_redraw_بالاترین > اولی)
	    wp->w_redraw_بالاترین = اولی;
	إذا (wp->w_redraw_bot == 0 || wp->w_redraw_bot < آخرین)
	    wp->w_redraw_bot = آخرین;
	إذا (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#اگر_تعریف_شده FEAT_EVAL
ساكن bool redraw_cb_درون_پیشرفت = false;

    فراغ
f_redraw_listener_جمع_کن(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    ساكن صحيح		id;

    إذا (check_secure())
	أرجع;

    إذا (redraw_cb_درون_پیشرفت)
    {
	emsg(_(e_cannot_جمع_کن_redraw_listener_درون_listener_callback));
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
	copy_callback(&rln->rl_callbacks.on_انتها, &cb);
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

    rln->rl_بعدی = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_عدد;
    rettv->vval.v_عدد = id;

    أرجع;
}

    ساكن فراغ
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_شروع);
    free_callback(&rln->rl_callbacks.on_انتها);

    vim_free(rln);
}


    ساكن فراغ
redraw_listener_cleanup(فراغ)
{
    لكل (redraw_listener_T *rln = redraw_listeners; rln != عدم;)
    {
	redraw_listener_T *بعدی = rln->rl_بعدی;
	إذا (rln->rl_id == 0)
	{
	    إذا (redraw_listeners == rln)
		redraw_listeners = rln->rl_بعدی;
	    redraw_listener_free(rln);
	}
	rln = بعدی;
    }
}

/*
 * Return the redraw listener struct with the specified id. Returns NULL if not
 * found.
 */
    ساكن redraw_listener_T *
get_redraw_listener(صحيح id)
{
    لكل (redraw_listener_T *rln = redraw_listeners; rln != عدم; rln = rln->rl_بعدی)
	إذا (rln->rl_id == id)
	    أرجع rln;
    أرجع عدم;
}

    فراغ
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    صحيح			id;
    redraw_listener_T	*rln;

    إذا (check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    id = argvars[0].vval.v_عدد;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_عدد;
    إذا (rln == عدم)
    {
	rettv->vval.v_عدد = 0;
	أرجع;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_عدد = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    ساكن فراغ
invoke_redraw_listener_شروع_or_انتها(bool شروع)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    إذا (شروع)
	inside_redraw_on_شروع_cb = true;

    redraw_cb_درون_پیشرفت = true;
    لكل (redraw_listener_T *rln = redraw_listeners; rln != عدم; rln = rln->rl_بعدی)
    {
	إذا (rln->rl_id == 0)
	    // Listener has been removed, skip
	    استمر;
	إذا (شروع && rln->rl_callbacks.on_شروع.cb_name != عدم)
	{
	    call_callback(&rln->rl_callbacks.on_شروع, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	وإلا إذا (rln->rl_callbacks.on_انتها.cb_name != عدم)
	{
	    call_callback(&rln->rl_callbacks.on_انتها, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_درون_پیشرفت = false;

    إذا (شروع)
	inside_redraw_on_شروع_cb = false;
}
#پایان_اگر // FEAT_EVAL
