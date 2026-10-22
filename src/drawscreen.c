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

#అన్తర్భావ "vim.h"

స్థిర శూన్య win_update(win_T *wp);
#నిర్వచితమైతే FEAT_STL_OPT
స్థిర శూన్య redraw_custom_statusline(win_T *wp);
#అయితే_ముగింపు
స్థిర శూన్య borrow_stl_vsep_hl(శూన్య);
#ఒకవేళ defined(FEAT_శోధన_EXTRA) || defined(FEAT_CLIPBOARD)
స్థిర పూర్ణాంకం  did_update_one_కిటికీ;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
స్థిర శూన్య redraw_listener_cleanup(శూన్య);
స్థిర శూన్య invoke_redraw_listener_ప్రారంభం_or_చివర(bool ప్రారంభం);
#అయితే_ముగింపు

/*
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 * Return OK when the screen was updated, FAIL if it was not done.
 */
    పూర్ణాంకం
update_screen(పూర్ణాంకం type_arg)
{
    పూర్ణాంకం		type = type_arg;
    win_T	*wp;
    స్థిర పూర్ణాంకం	did_intro = FALSE;
#నిర్వచితమైతే FEAT_GUI
    పూర్ణాంకం		did_one = FALSE;
    పూర్ణాంకం		did_undraw = FALSE;
    పూర్ణాంకం		gui_cursor_నిలువు = 0;
    పూర్ణాంకం		gui_cursor_అడ్డువరుస = 0;
#అయితే_ముగింపు
    పూర్ణాంకం		no_update = FALSE;
    పూర్ణాంకం		save_pum_will_redraw = pum_will_redraw;
#నిర్వచితమైతే FEAT_PROP_POPUP
    పూర్ణాంకం		did_redraw_కిటికీ = FALSE;
#అయితే_ముగింపు
    bool	override_success;

    // Don't do anything if the screen structures are (not yet) valid.
    ఒకవేళ (!screen_valid(TRUE))
	తిరిగి FAIL;

    ఒకవేళ (type == UPD_VALID_NO_UPDATE)
    {
	no_update = TRUE;
	type = 0;
    }

#నిర్వచితమైతే FEAT_EVAL
    {
	buf_T *buf;

	// Before updating the screen, notify any listeners of changed text.
	FOR_ALL_BUFFERS(buf)
	    invoke_listeners(buf);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_DIFF
    // May have postponed updating diffs.
    ఒకవేళ (need_diff_redraw)
	diff_redraw(TRUE);
#అయితే_ముగింపు

    ఒకవేళ (must_redraw)
    {
	ఒకవేళ (type < must_redraw)	    // use maximal type
	    type = must_redraw;

	// must_redraw is reset here, so that when we run into some weird
	// reason to redraw while busy redrawing (e.g., asynchronous
	// scrolling), or update_topline() in win_update() will cause a
	// scroll, the screen will be redrawn later or in win_update().
	must_redraw = 0;
    }

    // May need to update w_lines[].
    ఒకవేళ (curwin->w_పంక్తులు_valid == 0 && type < UPD_NOT_VALID
#నిర్వచితమైతే FEAT_TERMINAL
	    && !పదం_do_update_కిటికీ(curwin)
#అయితే_ముగింపు
		)
	type = UPD_NOT_VALID;

    // Postpone the redrawing when it's not needed and when being called
    // recursively.
    ఒకవేళ (!redrawing() || updating_screen)
    {
	redraw_later(type);		// remember type for next time
	must_redraw = type;
	ఒకవేళ (type > UPD_INVERTED_ALL)
	    curwin->w_పంక్తులు_valid = 0;	// don't use w_lines[].wl_size now
	తిరిగి FAIL;
    }
    updating_screen = TRUE;

    // Hide the cursor while redrawing when sync output is not active, to
    // avoid visible cursor flicker on terminals like Windows ConPTY.
    పూర్ణాంకం hid_cursor = !sync_output_చురుకైన();
    ఒకవేళ (hid_cursor)
	cursor_off();
    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_ENABLE);

#నిర్వచితమైతే FEAT_PROP_POPUP
    // Update popup_mask if needed.  This may set w_redraw_top and w_redraw_bot
    // in some windows.
    may_update_popup_ముసుగు(type);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYN_HL
    ++display_tick;	    // let syntax code know we're in a next round of
			    // display updating
#అయితే_ముగింపు
    ఒకవేళ (no_update)
	++no_win_do_పంక్తులు_ins;

    // if the screen was scrolled up when displaying a message, scroll it down
    ఒకవేళ (సందేశం_scrolled)
    {
	clear_cmdline = TRUE;
	ఒకవేళ (type != UPD_CLEAR)
	{
	    ఒకవేళ (సందేశం_scrolled > అడ్డువరుసలు - 5)	    // redrawing is faster
	    {
		type = UPD_NOT_VALID;
		redraw_as_cleared();
	    }
	    లేకపోతే
	    {
		check_for_delay(FALSE);
		ఒకవేళ (screen_ins_పంక్తులు(0, 0, సందేశం_scrolled, (పూర్ణాంకం)అడ్డువరుసలు, 0, ఖాళీ)
								       == FAIL)
		{
		    type = UPD_NOT_VALID;
		    redraw_as_cleared();
		}
		FOR_ALL_WINDOWS(wp)
		{
		    ఒకవేళ (wp->w_winrow < సందేశం_scrolled)
		    {
			ఒకవేళ (W_WINROW(wp) + wp->w_ఎత్తు > సందేశం_scrolled
				&& wp->w_redr_type < UPD_REDRAW_శిఖరం
				&& wp->w_పంక్తులు_valid > 0
				&& wp->w_topline == wp->w_పంక్తులు[0].wl_lnum)
			{
			    wp->w_upd_అడ్డువరుసలు = సందేశం_scrolled - W_WINROW(wp);
			    wp->w_redr_type = UPD_REDRAW_శిఖరం;
			}
			లేకపోతే
			{
			    wp->w_redr_type = UPD_NOT_VALID;
			    ఒకవేళ (W_WINROW(wp) + wp->w_ఎత్తు
					 + wp->w_స్థితి_ఎత్తు <= సందేశం_scrolled)
				wp->w_redr_స్థితి = true;
			}
		    }
		}
		ఒకవేళ (!no_update)
		    redraw_cmdline = TRUE;
		redraw_tabline = TRUE;
	    }
#ఒకవేళ defined(FEAT_TABPANEL)
	    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
	}
	సందేశం_scrolled = 0;
	need_wait_return = FALSE;
    }

    // reset cmdline_row now (may have been changed temporarily)
    compute_cmdrow();

    // Check for changed highlighting
    ఒకవేళ (need_highlight_మారింది)
	highlight_మారింది();

    ఒకవేళ (type == UPD_CLEAR)		// first clear screen
    {
	screenclear();		// will reset clear_cmdline
	type = UPD_NOT_VALID;
	// must_redraw may be set indirectly, avoid another redraw later
	must_redraw = 0;
    }

    ఒకవేళ (clear_cmdline)		// going to clear cmdline (done below)
	check_for_delay(FALSE);

#నిర్వచితమైతే FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    ఒకవేళ (curwin->w_redr_type < UPD_NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? సంఖ్య_వెడల్పు(curwin) : 0))
	curwin->w_redr_type = UPD_NOT_VALID;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    invoke_redraw_listener_ప్రారంభం_or_చివర(true);
#అయితే_ముగింపు

    // Only start redrawing if there is really something to do.
    ఒకవేళ (type == UPD_INVERTED)
	update_curswant();
    ఒకవేళ (curwin->w_redr_type < type
	    && !((type == UPD_VALID
		    && curwin->w_పంక్తులు[0].wl_valid
#నిర్వచితమైతే FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#అయితే_ముగింపు
		    && curwin->w_topline == curwin->w_పంక్తులు[0].wl_lnum)
		|| (type == UPD_INVERTED
		    && VIsual_చురుకైన
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_బహుళకం == VIsual_బహుళకం
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

    // Redraw the tab pages line if needed.
    ఒకవేళ (redraw_tabline || type >= UPD_NOT_VALID)
	draw_tabline();

#ఒకవేళ defined(FEAT_TABPANEL)
    ఒకవేళ (redraw_tabpanel || type >= UPD_NOT_VALID)
	draw_tabpanel();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYN_HL
    // Correct stored syntax highlighting info for changes in each displayed
    // buffer.  Each buffer must only be done once.
    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (wp->w_బఫరు->b_mod_సమితి)
	{
	    win_T	*wwp;

	    // Check if we already did this buffer.
	    కొరకు (wwp = firstwin; wwp != wp; wwp = wwp->w_తదుపరి)
		ఒకవేళ (wwp->w_బఫరు == wp->w_బఫరు)
		    ఆపు;
	    ఒకవేళ (wwp == wp && syntax_present(wp))
		syn_దొంతర_apply_changes(wp->w_బఫరు);
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (pum_redraw_లోపల_same_position())
	// Avoid flicker if the popup menu is going to be redrawn in the same
	// position.
	pum_will_redraw = TRUE;

    // Go from top to bottom through the windows, redrawing the ones that need
    // it.
#ఒకవేళ defined(FEAT_శోధన_EXTRA) || defined(FEAT_CLIPBOARD)
    did_update_one_కిటికీ = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    screen_శోధన_hl.rm.regprog = ఖాళీ;
#అయితే_ముగింపు
    FOR_ALL_WINDOWS(wp)
    {
	override_success = నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ఒకవేళ (wp->w_redr_type != 0)
	{
	    cursor_off();
#నిర్వచితమైతే FEAT_PROP_POPUP
	    did_redraw_కిటికీ = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
	    ఒకవేళ (!did_one)
	    {
		did_one = TRUE;

		// Remove the cursor before starting to do anything, because
		// scrolling may make it difficult to redraw the text under
		// it.
		// Also remove the cursor if it needs to be hidden due to an
		// ongoing cursor-less sleep.
		ఒకవేళ (gui.లోపల_use && (wp == curwin || cursor_is_sleeping()))
		{
		    gui_cursor_నిలువు = gui.cursor_నిలువు;
		    gui_cursor_అడ్డువరుస = gui.cursor_అడ్డువరుస;
		    gui_undraw_cursor();
		    did_undraw = TRUE;
		}
	    }
#అయితే_ముగింపు
	    win_update(wp);
	}

	// redraw status line after the window to minimize cursor movement
	ఒకవేళ (wp->w_redr_స్థితి)
	{
	    cursor_off();
	    win_redr_స్థితి(wp, TRUE); // any popup menu will be redrawn below
	}

	ఒకవేళ (override_success)
	    తీయి_highlight_overrides();

    }
#ఒకవేళ defined(FEAT_శోధన_EXTRA)
    చివర_శోధన_hl();
#అయితే_ముగింపు

    // May need to redraw the popup menu.
    pum_will_redraw = save_pum_will_redraw;
    pum_may_redraw();

    // Redraw vertical separators to update VertSplit/VertSplitNC highlights
    // when the current window has changed.
    ఒకవేళ (redraw_vseps)
    {
	redraw_vseps = FALSE;
	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (wp->w_vsep_వెడల్పు > 0)
		draw_vsep_win(wp, 0);
    }

    borrow_stl_vsep_hl();

    // Reset b_mod_set flags.  Going through all windows is probably faster
    // than going through all buffers (there could be many buffers).
    FOR_ALL_WINDOWS(wp)
	wp->w_బఫరు->b_mod_సమితి = false;

#నిర్వచితమైతే FEAT_PROP_POPUP
    // Display popup windows on top of the windows and command line.
    ఒకవేళ (did_redraw_కిటికీ || popup_need_redraw())
	update_popups(win_update);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMINAL
    FOR_ALL_WINDOWS(wp)
	// If this window contains a terminal, after redrawing all windows, the
	// dirty row range can be reset.
	పదం_did_update_కిటికీ(wp);
#అయితే_ముగింపు

    after_updating_screen(TRUE);

    // Clear or redraw the command line.  Done last, because scrolling may
    // mess up the command line.
    ఒకవేళ (clear_cmdline || redraw_cmdline || redraw_బహుళకం)
	showmode();

    ఒకవేళ (no_update)
	--no_win_do_పంక్తులు_ins;

    // May put up an introductory message when not editing a file
    ఒకవేళ (!did_intro)
	maybe_intro_సందేశం();
    did_intro = TRUE;

#నిర్వచితమైతే FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    ఒకవేళ (gui.లోపల_use)
    {
	ఒకవేళ (did_undraw && !gui_mch_is_blink_off())
	{
	    mch_disable_flush();
	    బహిర్గతం_flush();	// required before updating the cursor
	    mch_enable_flush();

	    // Put the GUI position where the cursor was, gui_update_cursor()
	    // uses that.
	    gui.నిలువు = gui_cursor_నిలువు;
	    gui.అడ్డువరుస = gui_cursor_అడ్డువరుస;
	    gui.నిలువు = mb_fix_నిలువు(gui.నిలువు, gui.అడ్డువరుస);
	    gui_update_cursor(FALSE, FALSE);
	    gui_may_flush();
	    screen_cur_నిలువు = gui.నిలువు;
	    screen_cur_అడ్డువరుస = gui.అడ్డువరుస;
	}
	లేకపోతే
	    బహిర్గతం_flush();
	gui_update_scrollbars(FALSE);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_IMAGE
    // Make sure to do this last!
    draw_image_placements();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    invoke_redraw_listener_ప్రారంభం_or_చివర(false);
    redraw_listener_cleanup();
#అయితే_ముగింపు

    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_DISABLE);
    ఒకవేళ (hid_cursor)
	cursor_on();

    తిరిగి OK;
}

/*
 * Return the row for drawing the statusline and the ruler of window "wp".
 */
    పూర్ణాంకం
statusline_అడ్డువరుస(win_T *wp)
{
#ఒకవేళ defined(FEAT_PROP_POPUP)
    // If the window is really zero height the winbar isn't displayed.
    ఒకవేళ (wp->w_చట్రం->fr_ఎత్తు == wp->w_స్థితి_ఎత్తు && !popup_is_popup(wp))
	తిరిగి wp->w_winrow;
#అయితే_ముగింపు
    తిరిగి W_WINROW(wp) + wp->w_ఎత్తు;
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 * If "ignore_pum" is TRUE, also redraw statusline when the popup menu is
 * displayed.
 */
    శూన్య
win_redr_స్థితి(win_T *wp, పూర్ణాంకం ignore_pum వాడని)
{
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		fillchar;
    పూర్ణాంకం		attr;
    పూర్ణాంకం		i;
    స్థిర పూర్ణాంకం  busy = FALSE;

    // It's possible to get here recursively when 'statusline' (indirectly)
    // invokes ":redrawstatus".  Simply ignore the call then.
    ఒకవేళ (busy)
	తిరిగి;
    busy = TRUE;

    అడ్డువరుస = statusline_అడ్డువరుస(wp);

    wp->w_redr_స్థితి = false;
    ఒకవేళ (wp->w_స్థితి_ఎత్తు == 0)
    {
	// no status line, can only be last window
	redraw_cmdline = TRUE;
    }
    లేకపోతే ఒకవేళ (!redrawing()
	    // don't update status line when popup menu is visible and may be
	    // drawn over it, unless it will be redrawn later
	    || (!ignore_pum && pum_visible()))
    {
	// Don't redraw right now, do it later.
	wp->w_redr_స్థితి = true;
    }
#నిర్వచితమైతే FEAT_STL_OPT
    లేకపోతే ఒకవేళ (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	// redraw custom status line
	redraw_custom_statusline(wp);
    }
#అయితే_ముగింపు
    లేకపోతే
    {
	char_u	*p;
	పూర్ణాంకం	plen;
	పూర్ణాంకం	NameBufflen;
	పూర్ణాంకం	this_ru_నిలువు;
	పూర్ణాంకం	n;			// scratch value

	fillchar = fillchar_స్థితి(&attr, wp);

	get_trans_bufname(wp->w_బఫరు);
	p = NameBuff;
	plen = (పూర్ణాంకం)STRLEN(p);

	ఒకవేళ ((bt_help(wp->w_బఫరు)
#నిర్వచితమైతే FEAT_QUICKFIX
		    || wp->w_p_pvw
#అయితే_ముగింపు
		    || buf_Is_మారింది(wp->w_బఫరు)
		    || wp->w_బఫరు->b_p_ro)
		&& plen < MAXPATHL - 1)
	{
	    *(p + plen++) = ' ';	// replace NUL with space
	    *(p + plen) = NUL;		// NUL terminate the string
	}
	ఒకవేళ (bt_help(wp->w_బఫరు))
	    plen += vim_snprintf((అక్షరం *)p + plen, MAXPATHL - plen, "%s", _("[Help]"));
#నిర్వచితమైతే FEAT_QUICKFIX
	ఒకవేళ (wp->w_p_pvw)
	    plen += vim_snprintf((అక్షరం *)p + plen, MAXPATHL - plen, "%s", _("[Preview]"));
#అయితే_ముగింపు
	ఒకవేళ (buf_Is_మారింది(wp->w_బఫరు) && !bt_terminal(wp->w_బఫరు))
	    plen += vim_snprintf((అక్షరం *)p + plen, MAXPATHL - plen, "%s", "[+]");
	ఒకవేళ (wp->w_బఫరు->b_p_ro)
	    plen += vim_snprintf((అక్షరం *)p + plen, MAXPATHL - plen, "%s", _("[RO]"));

	this_ru_నిలువు = ru_నిలువు - (cmdline_వెడల్పు - wp->w_వెడల్పు);
	n = (wp->w_వెడల్పు + 1) / 2;
	ఒకవేళ (this_ru_నిలువు < n)
	    this_ru_నిలువు = n;
	ఒకవేళ (this_ru_నిలువు <= 1)
	{
	    p = (char_u *)"<";		// No room for file name!
	    plen = 1;
	}
	లేకపోతే ఒకవేళ (has_mbyte)
	{
	    // Count total number of display cells.
	    plen = mb_string2cells(p, -1);

	    // Find first character that will fit.
	    // Going from start to end is much faster for DBCS.
	    కొరకు (i = 0; p[i] != NUL && plen >= this_ru_నిలువు - 1;
		    i += (*mb_ptr2len)(p + i))
		plen -= (*mb_ptr2cells)(p + i);
	    ఒకవేళ (i > 0)
	    {
		p = p + i - 1;
		*p = '<';
		++plen;
	    }
	}
	లేకపోతే ఒకవేళ (plen > this_ru_నిలువు - 1)
	{
	    p += plen - (this_ru_నిలువు - 1);
	    *p = '<';
	    plen = this_ru_నిలువు - 1;
	}

	screen_puts(p, అడ్డువరుస, wp->w_wincol, attr);
	screen_fill(అడ్డువరుస, అడ్డువరుస + 1, plen + wp->w_wincol,
		    this_ru_నిలువు + wp->w_wincol, fillchar, fillchar, attr);
	// Fill extra status line rows entirely with fillchar.
	కొరకు (i = 1; i < wp->w_స్థితి_ఎత్తు; i++)
	    screen_fill(అడ్డువరుస + i, అడ్డువరుస + i + 1, wp->w_wincol,
			W_ENDCOL(wp), fillchar, fillchar, attr);
	ఒకవేళ ((NameBufflen = get_keymap_str(wp, (char_u *)"<%s>", NameBuff, MAXPATHL)) > 0
		&& (this_ru_నిలువు - plen) > (NameBufflen + 1))
	    screen_puts(NameBuff, అడ్డువరుస, (పూర్ణాంకం)(this_ru_నిలువు - NameBufflen
						   - 1 + wp->w_wincol), attr);

	win_redr_ruler(wp, TRUE, ignore_pum);

	// Draw the 'showcmd' information if 'showcmdloc' == "statusline".
	ఒకవేళ (p_sc && *p_sloc == 's')
	{
	    n = this_ru_నిలువు - plen - 2;		    // perform the calculation here so we only do it once
	    పూర్ణాంకం	వెడల్పు = MIN(10, n);

	    ఒకవేళ (వెడల్పు > 0)
		screen_puts_len(showcmd_buf, వెడల్పు, అడ్డువరుస,
				wp->w_wincol + this_ru_నిలువు - వెడల్పు - 1, attr);
	    showcmd_update_clear_state();
	}
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    ఒకవేళ (wp->w_vsep_వెడల్పు != 0 && wp->w_స్థితి_ఎత్తు != 0 && redrawing())
    {
	కొరకు (i = 0; i < wp->w_స్థితి_ఎత్తు; i++)
	{
	    పూర్ణాంకం r = అడ్డువరుస + i;
	    ఒకవేళ (stl_connected(wp))
		fillchar = fillchar_స్థితి(&attr, wp);
	    లేకపోతే
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
    స్థిర శూన్య
borrow_stl_vsep_hl(శూన్య)
{
    win_T   *ఎడమ_2 = ఖాళీ;
    win_T   *కుడి_2 = ఖాళీ;

    // In silent Ex mode the screen is not allocated, so LineOffset and
    // ScreenAttrs are NULL; there is nothing to borrow.
    ఒకవేళ (!redrawing() || Screen_పంక్తులు == ఖాళీ)
	తిరిగి;

    FOR_ALL_WINDOWS(ఎడమ_2)
    {
	ఒకవేళ (ఎడమ_2->w_స్థితి_ఎత్తు == 0 || ఎడమ_2->w_vsep_వెడల్పు == 0)
	    కొనసాగు;
	// A window of zero width has no status line cell to borrow from.
	ఒకవేళ (ఎడమ_2->w_వెడల్పు == 0)
	    కొనసాగు;
	ఒకవేళ (!stl_connected(ఎడమ_2))
	    కొనసాగు;

	// Find a right neighbour whose status line rows overlap.
	win_T	*neighbour = ఖాళీ;
	పూర్ణాంకం ప్రారంభం = 0;
	పూర్ణాంకం చివర = 0;

	FOR_ALL_WINDOWS(కుడి_2)
	{
	    ఒకవేళ (కుడి_2 == ఎడమ_2 || కుడి_2->w_స్థితి_ఎత్తు == 0)
		కొనసాగు;
	    ఒకవేళ (కుడి_2->w_wincol != W_ENDCOL(ఎడమ_2) + 1)
		కొనసాగు;
	    పూర్ణాంకం l_stl_అడ్డువరుస = W_WINROW(ఎడమ_2) + ఎడమ_2->w_ఎత్తు;
	    పూర్ణాంకం r_stl_అడ్డువరుస = W_WINROW(కుడి_2) + కుడి_2->w_ఎత్తు;

	    ప్రారంభం = l_stl_అడ్డువరుస > r_stl_అడ్డువరుస ? l_stl_అడ్డువరుస : r_stl_అడ్డువరుస;
	    చివర = l_stl_అడ్డువరుస + ఎడమ_2->w_స్థితి_ఎత్తు
				< r_stl_అడ్డువరుస + కుడి_2->w_స్థితి_ఎత్తు
		? l_stl_అడ్డువరుస + ఎడమ_2->w_స్థితి_ఎత్తు
		: r_stl_అడ్డువరుస + కుడి_2->w_స్థితి_ఎత్తు;
	    ఒకవేళ (ప్రారంభం < చివర)
	    {
		neighbour = కుడి_2;
		ఆపు;
	    }
	}
	ఒకవేళ (neighbour == ఖాళీ)
	    కొనసాగు;

	// For non-current pairs only borrow when the status fillchar is a
	// space; otherwise the visible character would be repainted with a
	// foreign highlight.
	పూర్ణాంకం	hl;
	ఒకవేళ (ఎడమ_2 != curwin && neighbour != curwin
		&& fillchar_స్థితి(&hl, ఎడమ_2) != ' ')
	    కొనసాగు;

	// Source: prefer curwin's side; otherwise left window's right edge.
	పూర్ణాంకం dst_నిలువు = W_ENDCOL(ఎడమ_2);
	పూర్ణాంకం src_నిలువు = (neighbour == curwin)
				? neighbour->w_wincol : W_ENDCOL(ఎడమ_2) - 1;

	// The windows may be laid out for a size the screen does not have yet.
	ఒకవేళ (dst_నిలువు >= screen_Columns || src_నిలువు >= screen_Columns)
	    కొనసాగు;
	ఒకవేళ (చివర > screen_అడ్డువరుసలు)
	    చివర = screen_అడ్డువరుసలు;

	కొరకు (పూర్ణాంకం r = ప్రారంభం; r < చివర; r++)
	{
	    గుర్తులేని	dst_off = పంక్తి_స్థానభ్రంశం[r] + dst_నిలువు;
	    sattr_T	attr = ScreenAttrs[పంక్తి_స్థానభ్రంశం[r] + src_నిలువు];

	    ఒకవేళ (ScreenAttrs[dst_off] != attr)
	    {
		ScreenAttrs[dst_off] = attr;
		screen_char(dst_off, r, dst_నిలువు);
	    }
	}
    }
}

#నిర్వచితమైతే FEAT_STL_OPT
/*
 * Redraw the status line according to 'statusline' and take care of any
 * errors encountered.
 */
    స్థిర శూన్య
redraw_custom_statusline(win_T *wp)
{
    స్థిర పూర్ణాంకం	    entered = FALSE;

    // When called recursively return.  This can happen when the statusline
    // contains an expression that triggers a redraw.
    ఒకవేళ (entered)
	తిరిగి;
    entered = TRUE;

    win_redr_custom(wp, FALSE);
    entered = FALSE;
}
#అయితే_ముగింపు

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    శూన్య
showruler(పూర్ణాంకం always)
{
    ఒకవేళ (!always && !redrawing())
	తిరిగి;
    ఒకవేళ (pum_visible())
    {
	// Don't redraw right now, do it later.
	curwin->w_redr_స్థితి = true;
	తిరిగి;
    }
#ఒకవేళ defined(FEAT_STL_OPT)
    ఒకవేళ ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_స్థితి_ఎత్తు)
    {
	redraw_custom_statusline(curwin);
	borrow_stl_vsep_hl();
    }
    లేకపోతే
#అయితే_ముగింపు
	win_redr_ruler(curwin, always, FALSE);

    ఒకవేళ (need_maketitle
#నిర్వచితమైతే FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_లోపల_ICON))
	    || (p_title && (stl_syntax & STL_లోపల_TITLE))
#అయితే_ముగింపు
       )
	maketitle();

    // Redraw the tab pages line if needed.
    ఒకవేళ (redraw_tabline)
	draw_tabline();

#ఒకవేళ defined(FEAT_TABPANEL)
    ఒకవేళ (redraw_tabpanel)
	draw_tabpanel();
#అయితే_ముగింపు
}

    శూన్య
win_redr_ruler(win_T *wp, పూర్ణాంకం always, పూర్ణాంకం ignore_pum)
{
    పూర్ణాంకం	ఖాళీలు_పంక్తి = FALSE;

    // If 'ruler' off don't do anything
    ఒకవేళ (!p_ru)
	తిరిగి;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    ఒకవేళ (wp->w_cursor.lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి;

    // Don't draw the ruler while doing insert-completion, it might overwrite
    // the (long) mode message.
    ఒకవేళ (wp == lastwin && lastwin->w_స్థితి_ఎత్తు == 0)
	ఒకవేళ (edit_submode != ఖాళీ)
	    తిరిగి;

    // Don't draw the ruler when the popup menu is visible, it may overlap.
    // Except when the popup menu will be redrawn anyway.
    ఒకవేళ (!ignore_pum && pum_visible())
	తిరిగి;

#నిర్వచితమైతే FEAT_STL_OPT
    ఒకవేళ (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	తిరిగి;
    }
#అయితే_ముగింపు

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    ఒకవేళ ((State & బహుళకం_చొప్పించు) == 0
		&& *ml_get_buf(wp->w_బఫరు, wp->w_cursor.lnum, FALSE) == NUL)
	ఖాళీలు_పంక్తి = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    ఒకవేళ (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.నిలువు != wp->w_ru_cursor.నిలువు
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క != wp->w_ru_పంక్తి_లెక్క
#నిర్వచితమైతే FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#అయితే_ముగింపు
	    || ఖాళీలు_పంక్తి != wp->w_ru_ఖాళీలు)
    {
	పూర్ణాంకం	అడ్డువరుస;
	పూర్ణాంకం	fillchar;
	పూర్ణాంకం	attr;
	పూర్ణాంకం	off;
	పూర్ణాంకం	వెడల్పు;
	colnr_T	virtcol;
#నిర్వచించు RULER_BUF_LEN 70
	char_u	బఫరు[RULER_BUF_LEN];
	పూర్ణాంకం	bufferlen;
	char_u	rel_pos[RULER_BUF_LEN];
	పూర్ణాంకం	rel_poslen;
	పూర్ణాంకం	this_ru_నిలువు;
	పూర్ణాంకం	n1;			    // scratch value
	పూర్ణాంకం	n2;			    // scratch value
	bool	override_success =
	    నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);

	cursor_off();
	ఒకవేళ (wp->w_స్థితి_ఎత్తు)
	{
	    అడ్డువరుస = statusline_అడ్డువరుస(wp);
	    fillchar = fillchar_స్థితి(&attr, wp);
	    off = wp->w_wincol;
	    వెడల్పు = wp->w_వెడల్పు;
	}
	లేకపోతే
	{
	    అడ్డువరుస = అడ్డువరుసలు - 1;
	    fillchar = ' ';
	    attr = 0;
	    off = cmdline_నిలువు_off;
	    వెడల్పు = cmdline_వెడల్పు;
	}

	// In list mode virtcol needs to be recomputed
	virtcol = wp->w_virtcol;
	ఒకవేళ (wp->w_p_list && wp->w_lcs_chars.tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, ఖాళీ, &virtcol, ఖాళీ, 0);
	    wp->w_p_list = TRUE;
	}

	// row number, column number is appended
	// l10n: leave as-is unless a space after the comma is preferred
	// l10n: do not add any row/column label, due to the limited space
	bufferlen = vim_snprintf((అక్షరం *)బఫరు, RULER_BUF_LEN, _("%ld,"),
		(wp->w_బఫరు->b_ml.ml_flags & ML_ఖాళీలు)
		    ? 0L
		    : (దీర్ఘ)(wp->w_cursor.lnum));
	bufferlen += నిలువు_print(బఫరు + bufferlen, RULER_BUF_LEN - bufferlen,
			ఖాళీలు_పంక్తి ? 0 : (పూర్ణాంకం)wp->w_cursor.నిలువు + 1,
			(పూర్ణాంకం)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	rel_poslen = get_rel_pos(wp, rel_pos, RULER_BUF_LEN);
	n1 = bufferlen + vim_strsize(rel_pos);
	ఒకవేళ (wp->w_స్థితి_ఎత్తు == 0)	// can't use last char of screen
	    ++n1;

	this_ru_నిలువు = ru_నిలువు - (cmdline_వెడల్పు - వెడల్పు);
	// Never use more than half the window/screen width, leave the other
	// half for the filename.
	n2 = (వెడల్పు + 1) / 2;
	ఒకవేళ (this_ru_నిలువు < n2)
	    this_ru_నిలువు = n2;
	ఒకవేళ (this_ru_నిలువు + n1 < వెడల్పు)
	{
	    // need at least space for rel_pos + NUL
	    అయితే (this_ru_నిలువు + n1 < వెడల్పు
		    && RULER_BUF_LEN > bufferlen + rel_poslen + 1)	// +1 for NUL
	    {
		ఒకవేళ (has_mbyte)
		    bufferlen += (*mb_char2bytes)(fillchar, బఫరు + bufferlen);
		లేకపోతే
		    బఫరు[bufferlen++] = fillchar;
		++n1;
	    }
	    bufferlen += vim_snprintf((అక్షరం *)బఫరు + bufferlen, RULER_BUF_LEN - bufferlen,
			    "%s", rel_pos);
	}
	// Truncate at window boundary.
	ఒకవేళ (has_mbyte)
	{
	    కొరకు (n1 = 0, n2 = 0; బఫరు[n1] != NUL; n1 += (*mb_ptr2len)(బఫరు + n1))
	    {
		n2 += (*mb_ptr2cells)(బఫరు + n1);
		ఒకవేళ (this_ru_నిలువు + n2 > వెడల్పు)
		{
		    bufferlen = n1;
		    బఫరు[bufferlen] = NUL;
		    ఆపు;
		}
	    }
	}
	లేకపోతే ఒకవేళ (this_ru_నిలువు + bufferlen > వెడల్పు)
	{
	    bufferlen = వెడల్పు - this_ru_నిలువు;
	    బఫరు[bufferlen] = NUL;
	}

	screen_puts(బఫరు, అడ్డువరుస, this_ru_నిలువు + off, attr);
	n1 = redraw_cmdline;
	screen_fill(అడ్డువరుస, అడ్డువరుస + 1,
		this_ru_నిలువు + off + bufferlen,
		(off + వెడల్పు),
		fillchar, fillchar, attr);
	// don't redraw the cmdline because of showing the ruler
	redraw_cmdline = n1;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_ఖాళీలు = ఖాళీలు_పంక్తి;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_పంక్తి_లెక్క = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
#నిర్వచితమైతే FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#అయితే_ముగింపు

	ఒకవేళ (override_success)
	    తీయి_highlight_overrides();
    }
}

/*
 * To be called when "updating_screen" was set before and now the postponed
 * side effects may take place.
 */
    శూన్య
after_updating_screen(పూర్ణాంకం may_resize_shell వాడని)
{
    updating_screen = FALSE;
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (may_resize_shell)
	gui_may_resize_shell();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    పదం_check_channel_closed_recently();
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_DROP_FILE
    // If handle_drop() was called while updating_screen was TRUE need to
    // handle the drop now.
    handle_any_postponed_drop();
#అయితే_ముగింపు
}

/*
 * Update all windows that are editing the current buffer.
 */
    శూన్య
update_curbuf(పూర్ణాంకం type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

#ఒకవేళ defined(FEAT_MENU) || defined(FEAT_FOLDING)
/*
 * Copy "text" to ScreenLines using "attr".
 * Returns the next screen column.
 */
    స్థిర పూర్ణాంకం
పాఠ్యం_గమ్యం_screenline(win_T *wp, char_u *పాఠ్యం, పూర్ణాంకం నిలువు)
{
    పూర్ణాంకం		off = (పూర్ణాంకం)(current_Screen_పంక్తి - Screen_పంక్తులు);

    ఒకవేళ (has_mbyte)
    {
	పూర్ణాంకం	cells;
	పూర్ణాంకం	u8c, u8cc[MAX_MCO];
	పూర్ణాంకం	i;
	పూర్ణాంకం	idx;
	పూర్ణాంకం	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	పూర్ణాంకం	prev_c = 0;		// previous Arabic character
	పూర్ణాంకం	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	ఒకవేళ (wp->w_p_rl)
	    idx = off;
	లేకపోతే
# endif
	    idx = off + నిలువు;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	కొరకు (p = పాఠ్యం; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    ఒకవేళ (నిలువు + cells > wp->w_వెడల్పు)
		ఆపు;
# ifdef FEAT_RIGHTLEFT
	    ఒకవేళ (wp->w_p_rl)
		idx = off + wp->w_వెడల్పు - నిలువు - cells;
# endif
	    Screen_పంక్తులు[idx] = *p;
	    ఒకవేళ (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		ఒకవేళ (*p < 0x80 && u8cc[0] == 0)
		{
		    Screen_పంక్తులు_UC[idx] = 0;
# ifdef FEAT_ARABIC
		    prev_c = u8c;
# endif
		}
		లేకపోతే
		{
# ifdef FEAT_ARABIC
		    ఒకవేళ (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			పూర్ణాంకం	pc, pc1, nc;
			పూర్ణాంకం	pcc[MAX_MCO];
			పూర్ణాంకం	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			ఒకవేళ (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			లేకపోతే
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			Screen_పంక్తులు[idx] = firstbyte;
		    }
		    లేకపోతే
			prev_c = u8c;
# endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    Screen_పంక్తులు_UC[idx] = u8c;
		    కొరకు (i = 0; i < Screen_mco; ++i)
		    {
			Screen_పంక్తులు_C[i][idx] = u8cc[i];
			ఒకవేళ (u8cc[i] == 0)
			    ఆపు;
		    }
		}
		ఒకవేళ (cells > 1)
		    Screen_పంక్తులు[idx + 1] = 0;
	    }
	    లేకపోతే ఒకవేళ (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    లేకపోతే ఒకవేళ (cells > 1)
		// double-width character
		Screen_పంక్తులు[idx + 1] = p[1];
	    నిలువు += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    లేకపోతే
    {
	పూర్ణాంకం len = (పూర్ణాంకం)STRLEN(పాఠ్యం);
	పూర్ణాంకం n = wp->w_వెడల్పు - నిలువు;

	ఒకవేళ (len > n)
	    len = n;
	ఒకవేళ (len > 0)
	{
# ifdef FEAT_RIGHTLEFT
	    ఒకవేళ (wp->w_p_rl)
		mch_memmove(current_Screen_పంక్తి, పాఠ్యం, len);
	    లేకపోతే
# endif
		mch_memmove(current_Screen_పంక్తి + నిలువు, పాఠ్యం, len);
	    నిలువు += len;
	}
    }
    తిరిగి నిలువు;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
/*
 * Draw the window toolbar.
 */
    స్థిర శూన్య
redraw_win_toolbar(win_T *wp)
{
    vimmenu_T	*menu;
    పూర్ణాంకం		అంశం_idx = 0;
    పూర్ణాంకం		అంశం_లెక్క = 0;
    పూర్ణాంకం		నిలువు = 0;
    పూర్ణాంకం		తదుపరి_నిలువు;
    పూర్ణాంకం		off = (పూర్ణాంకం)(current_Screen_పంక్తి - Screen_పంక్తులు);
    bool	override_success =
	నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);
    పూర్ణాంకం		fill_attr = syn_name2attr((char_u *)"ToolbarLine");
    పూర్ణాంకం		button_attr = syn_name2attr((char_u *)"ToolbarButton");

    vim_free(wp->w_winbar_వస్తువులు);
    FOR_ALL_బిడ్డ_MENUS(wp->w_winbar, menu)
	++అంశం_లెక్క;
    wp->w_winbar_వస్తువులు = ALLOC_CLEAR_MULT(winbar_అంశం_T, అంశం_లెక్క + 1);

    // TODO: use fewer spaces if there is not enough room
    కొరకు (menu = wp->w_winbar->children;
			  menu != ఖాళీ && నిలువు < wp->w_వెడల్పు; menu = menu->తదుపరి)
    {
	space_గమ్యం_screenline(off + నిలువు, fill_attr);
	ఒకవేళ (++నిలువు >= wp->w_వెడల్పు)
	    ఆపు;
	ఒకవేళ (నిలువు > 1)
	{
	    space_గమ్యం_screenline(off + నిలువు, fill_attr);
	    ఒకవేళ (++నిలువు >= wp->w_వెడల్పు)
		ఆపు;
	}

	wp->w_winbar_వస్తువులు[అంశం_idx].wb_startcol = నిలువు;
	space_గమ్యం_screenline(off + నిలువు, button_attr);
	ఒకవేళ (++నిలువు >= wp->w_వెడల్పు)
	    ఆపు;

	తదుపరి_నిలువు = పాఠ్యం_గమ్యం_screenline(wp, menu->name, నిలువు);
	అయితే (నిలువు < తదుపరి_నిలువు)
	{
	    ScreenAttrs[off + నిలువు] = button_attr;
	    ++నిలువు;
	}
	wp->w_winbar_వస్తువులు[అంశం_idx].wb_endcol = నిలువు;
	wp->w_winbar_వస్తువులు[అంశం_idx].wb_menu = menu;
	++అంశం_idx;

	ఒకవేళ (నిలువు >= wp->w_వెడల్పు)
	    ఆపు;
	space_గమ్యం_screenline(off + నిలువు, button_attr);
	++నిలువు;
    }
    అయితే (నిలువు < wp->w_వెడల్పు)
    {
	space_గమ్యం_screenline(off + నిలువు, fill_attr);
	++నిలువు;
    }
    wp->w_winbar_వస్తువులు[అంశం_idx].wb_menu = ఖాళీ; // end marker

    screen_పంక్తి(wp, wp->w_winrow, wp->w_wincol, wp->w_వెడల్పు,
							  wp->w_వెడల్పు, -1, 0);

    ఒకవేళ (override_success)
	తీయి_highlight_overrides();
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_FOLDING)
/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    స్థిర శూన్య
copy_పాఠ్యం_attr(
    పూర్ణాంకం		off,
    char_u	*buf,
    పూర్ణాంకం		len,
    పూర్ణాంకం		attr)
{
    పూర్ణాంకం		i;

    mch_memmove(Screen_పంక్తులు + off, buf, (size_t)len);
    ఒకవేళ (enc_utf8)
	vim_memset(Screen_పంక్తులు_UC + off, 0, పరిమాణం(u8char_T) * (size_t)len);
    కొరకు (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Display one folded line.
 */
    స్థిర శూన్య
fold_పంక్తి(
    win_T	*wp,
    దీర్ఘ	fold_లెక్క,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    పూర్ణాంకం		అడ్డువరుస)
{
    // Max value of 'foldcolumn' is 12 and maximum number of bytes in a
    // multi-byte character is MAX_MCO.
    char_u	buf[MAX_MCO * 12 + 1];
    pos_T	*శిఖరం, *bot;
    linenr_T	lnume = lnum + fold_లెక్క - 1;
    పూర్ణాంకం		len;
    char_u	*పాఠ్యం;
    పూర్ణాంకం		fdc;
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		txtcol;
    పూర్ణాంకం		off = (పూర్ణాంకం)(current_Screen_పంక్తి - Screen_పంక్తులు);
    పూర్ణాంకం		ri;

    // Build the fold line:
    // 1. Add the cmdwin_type for the command-line window
    // 2. Add the 'foldcolumn'
    // 3. Add the 'number' or 'relativenumber' column
    // 4. Compose the text
    // 5. Add the text
    // 6. set highlighting for the Visual area an other text
    నిలువు = 0;

    // 1. Add the cmdwin_type for the command-line window
    // Ignores 'rightleft', this window is never right-left.
    ఒకవేళ (wp == cmdwin_win)
    {
	Screen_పంక్తులు[off] = cmdwin_type;
	ScreenAttrs[off] = HL_ATTR(HLF_AT);
	ఒకవేళ (enc_utf8)
	    Screen_పంక్తులు_UC[off] = 0;
	++నిలువు;
    }

# ifdef FEAT_RIGHTLEFT
#  define RL_MEMSET(p, v, l) \
    చేయి { \
	ఒకవేళ (wp->w_p_rl) \
	    కొరకు (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (wp->w_వెడల్పు - (p) - (l)) + ri] = v; \
	 లేకపోతే \
	    కొరకు (ri = 0; ri < (l); ++ri) \
	       ScreenAttrs[off + (p) + ri] = v; \
    } అయితే (0)
# లేకపోతే
#  define RL_MEMSET(p, v, l) \
    చేయి { \
	కొరకు (ri = 0; ri < l; ++ri) \
	    ScreenAttrs[off + (p) + ri] = v; \
    } అయితే (0)
# endif

    // 2. Add the 'foldcolumn'
    //    Reduce the width when there is not enough space.
    fdc = compute_foldcolumn(wp, నిలువు);
    ఒకవేళ (fdc > 0)
    {
	char_u	*p;
	పూర్ణాంకం	i;
	పూర్ణాంకం	idx;

	fill_foldcolumn(buf, wp, TRUE, lnum);
	p = buf;
	కొరకు (i = 0; i < fdc; i++)
	{
	    పూర్ణాంకం		ch;

	    ఒకవేళ (has_mbyte)
		ch = mb_ptr2char_adv(&p);
	    లేకపోతే
		ch = *p++;
# ifdef FEAT_RIGHTLEFT
	    ఒకవేళ (wp->w_p_rl)
		idx = off + wp->w_వెడల్పు - i - 1 - నిలువు;
	    లేకపోతే
# endif
		idx = off + నిలువు + i;
	    ఒకవేళ (enc_utf8)
	    {
		ఒకవేళ (ch >= 0x80)
		{
		    Screen_పంక్తులు_UC[idx] = ch;
		    Screen_పంక్తులు_C[0][idx] = 0;
		    Screen_పంక్తులు[idx] = 0x80;
		}
		లేకపోతే
		{
		    Screen_పంక్తులు[idx] = ch;
		    Screen_పంక్తులు_UC[idx] = 0;
		}
	    }
	    లేకపోతే
		Screen_పంక్తులు[idx] = ch;
	}

	RL_MEMSET(నిలువు, HL_ATTR(HLF_FC), fdc);
	నిలువు += fdc;
    }

    // Set all attributes of the 'number' or 'relativenumber' column and the
    //పాఠ్యం
    RL_MEMSET(నిలువు, HL_ATTR(HLF_FL), wp->w_వెడల్పు - నిలువు);

# ifdef FEAT_SIGNS
    // If signs are being displayed, add two spaces.
    ఒకవేళ (signcolumn_on(wp))
    {
	len = wp->w_వెడల్పు - నిలువు;
	ఒకవేళ (len > 0)
	{
	    ఒకవేళ (len > 2)
		len = 2;
#  ifdef FEAT_RIGHTLEFT
	    ఒకవేళ (wp->w_p_rl)
		// the line number isn't reversed
		copy_పాఠ్యం_attr(off + wp->w_వెడల్పు - len - నిలువు,
					(char_u *)"  ", len, HL_ATTR(HLF_FL));
	    లేకపోతే
#  endif
		copy_పాఠ్యం_attr(off + నిలువు, (char_u *)"  ", len, HL_ATTR(HLF_FL));
	    నిలువు += len;
	}
    }
# endif

    // 3. Add the 'number' or 'relativenumber' column
    ఒకవేళ (wp->w_p_nu || wp->w_p_rnu)
    {
	len = wp->w_వెడల్పు - నిలువు;
	ఒకవేళ (len > 0)
	{
	    పూర్ణాంకం	    w = సంఖ్య_వెడల్పు(wp);
	    దీర్ఘ    అంకె;
	    అక్షరం    *fmt = "%*ld ";

	    ఒకవేళ (len > w + 1)
		len = w + 1;

	    ఒకవేళ (wp->w_p_nu && !wp->w_p_rnu)
		// 'number' + 'norelativenumber'
		అంకె = (దీర్ఘ)lnum;
	    లేకపోతే
	    {
		// 'relativenumber', don't use negative numbers
		అంకె = labs((దీర్ఘ)get_cursor_rel_lnum(wp, lnum));
		ఒకవేళ (అంకె == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    // 'number' + 'relativenumber': cursor line shows absolute
		    // line number
		    అంకె = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    vim_snprintf((అక్షరం *)buf, పరిమాణం(buf), fmt, w, అంకె);
# ifdef FEAT_RIGHTLEFT
	    ఒకవేళ (wp->w_p_rl)
		// the line number isn't reversed
		copy_పాఠ్యం_attr(off + wp->w_వెడల్పు - len - నిలువు, buf, len,
							     HL_ATTR(HLF_FL));
	    లేకపోతే
# endif
		copy_పాఠ్యం_attr(off + నిలువు, buf, len, HL_ATTR(HLF_FL));
	    నిలువు += len;
	}
    }

    // 4. Compose the folded-line string with 'foldtext', if set.
    పాఠ్యం = get_foldtext(wp, lnum, lnume, foldinfo, buf);

    txtcol = నిలువు;	// remember where text starts

    // 5. move the text to current_ScreenLine.  Fill up with "fold" from
    //    'fillchars'.
    //    Right-left text is put in columns 0 - number-col, normal text is put
    //    in columns number-col - window-width.
    నిలువు = పాఠ్యం_గమ్యం_screenline(wp, పాఠ్యం, నిలువు);

    // Fill the rest of the line with the fold filler
    అయితే (నిలువు < wp->w_వెడల్పు)
    {
	పూర్ణాంకం c = wp->w_fill_chars.fold;
	పూర్ణాంకం idx = off + నిలువు;

# ifdef FEAT_RIGHTLEFT
	ఒకవేళ (wp->w_p_rl)
	    idx = off + wp->w_వెడల్పు - 1 - నిలువు;
# endif

	ఒకవేళ (enc_utf8)
	{
	    ఒకవేళ (c >= 0x80)
	    {
		Screen_పంక్తులు_UC[idx] = c;
		Screen_పంక్తులు_C[0][idx] = 0;
		Screen_పంక్తులు[idx] = 0x80; // avoid storing zero
	    }
	    లేకపోతే
	    {
		Screen_పంక్తులు_UC[idx] = 0;
		Screen_పంక్తులు[idx] = c;
	    }
	}
	లేకపోతే
	    Screen_పంక్తులు[idx] = c;
	++నిలువు;
    }

    ఒకవేళ (పాఠ్యం != buf)
	vim_free(పాఠ్యం);

    // 6. set highlighting for the Visual area an other text.
    // If all folded lines are in the Visual area, highlight the line.
    ఒకవేళ (VIsual_చురుకైన && wp->w_బఫరు == curwin->w_బఫరు)
    {
	ఒకవేళ (LTOREQ_POS(curwin->w_cursor, VIsual))
	{
	    // Visual is after curwin->w_cursor
	    శిఖరం = &curwin->w_cursor;
	    bot = &VIsual;
	}
	లేకపోతే
	{
	    // Visual is before curwin->w_cursor
	    శిఖరం = &VIsual;
	    bot = &curwin->w_cursor;
	}
	ఒకవేళ (lnum >= శిఖరం->lnum
		&& lnume <= bot->lnum
		&& (VIsual_బహుళకం != 'v'
		    || ((lnum > శిఖరం->lnum
			    || (lnum == శిఖరం->lnum
				&& శిఖరం->నిలువు == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->నిలువు - (*p_sel == 'e'))
				    >= ml_get_buf_len(wp->w_బఫరు, lnume))))))
	{
	    ఒకవేళ (VIsual_బహుళకం == Ctrl_V)
	    {
		// Visual block mode: highlight the chars part of the block
		ఒకవేళ (wp->w_old_cursor_fcol + txtcol < (colnr_T)wp->w_వెడల్పు)
		{
		    ఒకవేళ (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)wp->w_వెడల్పు)
			len = wp->w_old_cursor_lcol;
		    లేకపోతే
			len = wp->w_వెడల్పు - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, HL_ATTR(HLF_V),
					    len - (పూర్ణాంకం)wp->w_old_cursor_fcol);
		}
	    }
	    లేకపోతే
	    {
		// Set all attributes of the text
		RL_MEMSET(txtcol, HL_ATTR(HLF_V), wp->w_వెడల్పు - txtcol);
	    }
	}
    }

# ifdef FEAT_SYN_HL
    // Show colorcolumn in the fold line, but let cursorcolumn override it.
    ఒకవేళ (wp->w_p_cc_నిలువుల_సంఖ్య)
    {
	పూర్ణాంకం i = 0;
	పూర్ణాంకం j = wp->w_p_cc_నిలువుల_సంఖ్య[i];
	పూర్ణాంకం old_txtcol = txtcol;

	అయితే (j > -1)
	{
	    txtcol += j;
	    ఒకవేళ (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    లేకపోతే
		txtcol -= wp->w_leftcol;
	    ఒకవేళ (txtcol >= 0 && txtcol < wp->w_వెడల్పు)
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], HL_ATTR(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_నిలువుల_సంఖ్య[++i];
	}
    }

    // Show 'cursorcolumn' in the fold line.
    ఒకవేళ (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	ఒకవేళ (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	లేకపోతే
	    txtcol -= wp->w_leftcol;
	ఒకవేళ (txtcol >= 0 && txtcol < wp->w_వెడల్పు)
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], HL_ATTR(HLF_CUC));
    }
# endif

    screen_పంక్తి(wp, అడ్డువరుస + W_WINROW(wp), wp->w_wincol, wp->w_వెడల్పు, wp->w_వెడల్పు,
	    -1, 0);

    // Update w_cline_height and w_cline_folded if the cursor line was
    // updated (saves a call to plines() later).
    ఒకవేళ (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_అడ్డువరుస = అడ్డువరుస;
	curwin->w_cline_ఎత్తు = 1;
	curwin->w_cline_folded = true;
	curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
    }

# ifdef FEAT_CONCEAL
    // When the line was not folded w_wrow may have been set, recompute it.
    ఒకవేళ (wp == curwin
	    && wp->w_cursor.lnum >= lnum
	    && wp->w_cursor.lnum <= lnume
	    && conceal_cursor_పంక్తి(wp))
	curs_columns(TRUE);
# endif
}
#అయితే_ముగింపు

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
    స్థిర శూన్య
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_బఫరు;
    పూర్ణాంకం		type;
    పూర్ణాంకం		శిఖరం_చివర = 0;	// Below last row of the top area that needs
				// updating.  0 when no top area updating.
    పూర్ణాంకం		mid_ప్రారంభం = 999;// first row of the mid area that needs
				// updating.  999 when no mid area updating.
    పూర్ణాంకం		mid_చివర = 0;	// Below last row of the mid area that needs
				// updating.  0 when no mid area updating.
    పూర్ణాంకం		bot_ప్రారంభం = 999;// first row of the bot area that needs
				// updating.  999 when no bot area updating
    పూర్ణాంకం		scrolled_కిందికి = FALSE;	// TRUE when scrolled down when
					// w_topline got smaller a bit
    పూర్ణాంకం		scrolled_for_mod = FALSE;   // TRUE after scrolling for changed
					    //పంక్తులు
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    పూర్ణాంకం		శిఖరం_గమ్యం_mod = FALSE;    // redraw above mod_top
#అయితే_ముగింపు

    పూర్ణాంకం		అడ్డువరుస;		// current window row to display
    linenr_T	lnum;		// current buffer lnum to display
    పూర్ణాంకం		idx;		// current index in w_lines[]
    పూర్ణాంకం		srow;		// starting row of the current line

    పూర్ణాంకం		eof = FALSE;	// if TRUE, we hit the end of the file
    పూర్ణాంకం		didline = FALSE; // if TRUE, we finished the last line
    పూర్ణాంకం		i;
    దీర్ఘ	j;
    స్థిర పూర్ణాంకం	recursive = FALSE;	// being called recursively
    linenr_T	old_botline = wp->w_botline;
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		old_wrow = wp->w_wrow;
    పూర్ణాంకం		old_wcol = wp->w_wcol;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
    దీర్ఘ	fold_లెక్క;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    // remember what happened to the previous line, to know if
    // check_visual_highlight() can be used
# define DID_NONE 1	// didn't update a line
# define DID_పంక్తి 2	// updated a normal line
# define DID_FOLD 3	// updated a folded line
    పూర్ణాంకం		did_update = DID_NONE;
    linenr_T	syntax_చివరి_parsed = 0;		// last parsed text line
#అయితే_ముగింపు
    linenr_T	mod_శిఖరం = 0;
    linenr_T	mod_bot = 0;
#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_శోధన_EXTRA)
    పూర్ణాంకం		save_got_int;
#అయితే_ముగింపు
    bool	override_success;

#ఒకవేళ defined(FEAT_శోధన_EXTRA) || defined(FEAT_CLIPBOARD)
    // This needs to be done only for the first window when update_screen() is
    // called.
    ఒకవేళ (!did_update_one_కిటికీ)
    {
	did_update_one_కిటికీ = TRUE;
# ifdef FEAT_శోధన_EXTRA
	ప్రారంభం_శోధన_hl();
# endif
# ifdef FEAT_CLIPBOARD
	// When Visual area changed, may have to update selection.
	ఒకవేళ (clip_నక్షత్రం.అందుబాటు && clip_isautosel_నక్షత్రం())
	    clip_update_selection(&clip_నక్షత్రం);
	ఒకవేళ (clip_plus.అందుబాటు && clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
# endif
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYN_HL
    // 'cursorcolumn' is drawn with w_virtcol, make sure it is up to date.
    // This may set w_redr_type, thus do it before using it below.
    ఒకవేళ (wp->w_p_cuc)
	validate_virtcol_win(wp);
#అయితే_ముగింపు

    type = wp->w_redr_type;

    ఒకవేళ (type == UPD_NOT_VALID)
    {
	wp->w_redr_స్థితి = true;
	wp->w_పంక్తులు_valid = 0;
    }

    // Window frame is zero-height: nothing to draw.
    ఒకవేళ (wp->w_ఎత్తు + WINBAR_ఎత్తు(wp) == 0
	    || (wp->w_చట్రం->fr_ఎత్తు == wp->w_స్థితి_ఎత్తు
#ఒకవేళ defined(FEAT_PROP_POPUP)
		&& !popup_is_popup(wp)
#అయితే_ముగింపు
	       ))
    {
	wp->w_redr_type = 0;
	తిరిగి;
    }

    // Window is zero-width: Only need to draw the separator.
    ఒకవేళ (wp->w_వెడల్పు == 0)
    {
	// draw the vertical separator right of this window
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	తిరిగి;
    }

    override_success = నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);


#నిర్వచితమైతే FEAT_TERMINAL
    // If this window contains a terminal, redraw works completely differently.
    ఒకవేళ (పదం_do_update_కిటికీ(wp))
    {
	పదం_update_కిటికీ(wp);
# ifdef FEAT_MENU
	// Draw the window toolbar, if there is one.
	ఒకవేళ (winbar_ఎత్తు(wp) > 0)
	    redraw_win_toolbar(wp);
# endif
	wp->w_redr_type = 0;
	ఒకవేళ (override_success)
	    తీయి_highlight_overrides();
	తిరిగి;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    init_శోధన_hl(wp, &screen_శోధన_hl);
#అయితే_ముగింపు

    // Make sure skipcol is valid, it depends on various options and the window
    // width.
    ఒకవేళ (wp->w_skipcol > 0 && wp->w_వెడల్పు > win_నిలువు_off(wp))
    {
	పూర్ణాంకం w = 0;
	పూర్ణాంకం width1 = wp->w_వెడల్పు - win_నిలువు_off(wp);
	పూర్ణాంకం width2 = width1 + win_నిలువు_off2(wp);
	పూర్ణాంకం కూడు = width1;

	అయితే (w < wp->w_skipcol)
	{
	    ఒకవేళ (w > 0)
		కూడు = width2;
	    w += కూడు;
	}
	ఒకవేళ (w != wp->w_skipcol)
	    // always round down, the higher value may not be valid
	    wp->w_skipcol = w - కూడు;
    }

#నిర్వచితమైతే FEAT_LINEBREAK
    // Force redraw when width of 'number' or 'relativenumber' column
    // changes.
    i = (wp->w_p_nu || wp->w_p_rnu) ? సంఖ్య_వెడల్పు(wp) : 0;
    ఒకవేళ (wp->w_nrwidth != i)
    {
	type = UPD_NOT_VALID;
	wp->w_nrwidth = i;
    }
    లేకపోతే
#అయితే_ముగింపు

    {
	// Set mod_top to the first line that needs displaying because of
	// changes.  Set mod_bot to the first line after the changes.
	mod_శిఖరం = wp->w_redraw_శిఖరం;
	ఒకవేళ (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	లేకపోతే
	    mod_bot = 0;
	ఒకవేళ (buf->b_mod_సమితి)
	{
	    ఒకవేళ (mod_శిఖరం == 0 || mod_శిఖరం > buf->b_mod_శిఖరం)
	    {
		mod_శిఖరం = buf->b_mod_శిఖరం;
#నిర్వచితమైతే FEAT_SYN_HL
		// Need to redraw lines above the change that may be included
		// in a pattern match.
		ఒకవేళ (syntax_present(wp))
		{
		    mod_శిఖరం -= buf->b_s.b_syn_sync_linebreaks;
		    ఒకవేళ (mod_శిఖరం < 1)
			mod_శిఖరం = 1;
		}
#అయితే_ముగింపు
	    }
	    ఒకవేళ (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#నిర్వచితమైతే FEAT_శోధన_EXTRA
	    // When 'hlsearch' is on and using a multi-line search pattern, a
	    // change in one line may make the Search highlighting in a
	    // previous line invalid.  Simple solution: redraw all visible
	    // lines above the change.
	    // Same for a match pattern.
	    ఒకవేళ (screen_శోధన_hl.rm.regprog != ఖాళీ
		    && re_multiline(screen_శోధన_hl.rm.regprog))
		శిఖరం_గమ్యం_mod = TRUE;
	    లేకపోతే
	    {
		matchitem_T *cur = wp->w_సరిపోలిక_తల;

		అయితే (cur != ఖాళీ)
		{
		    ఒకవేళ (cur->mit_సరిపోలిక.regprog != ఖాళీ
				       && re_multiline(cur->mit_సరిపోలిక.regprog))
		    {
			శిఖరం_గమ్యం_mod = TRUE;
			ఆపు;
		    }
		    cur = cur->mit_తదుపరి;
		}
	    }
#అయితే_ముగింపు
	}

#నిర్వచితమైతే FEAT_శోధన_EXTRA
	ఒకవేళ (శోధన_hl_has_cursor_lnum > 0)
	{
	    // CurSearch was used last time, need to redraw the line with it to
	    // avoid having two matches highlighted with CurSearch.
	    ఒకవేళ (mod_శిఖరం == 0 || mod_శిఖరం > శోధన_hl_has_cursor_lnum)
		mod_శిఖరం = శోధన_hl_has_cursor_lnum;
	    ఒకవేళ (mod_bot == 0 || mod_bot < శోధన_hl_has_cursor_lnum + 1)
		mod_bot = శోధన_hl_has_cursor_lnum + 1;
	}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_FOLDING
	ఒకవేళ (mod_శిఖరం != 0 && hasAnyFolding(wp))
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
	    కొరకు (i = 0; i < wp->w_పంక్తులు_valid; ++i)
		ఒకవేళ (wp->w_పంక్తులు[i].wl_valid)
		{
		    ఒకవేళ (wp->w_పంక్తులు[i].wl_lastlnum < mod_శిఖరం)
			lnumt = wp->w_పంక్తులు[i].wl_lastlnum + 1;
		    ఒకవేళ (lnumb == MAXLNUM && wp->w_పంక్తులు[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_పంక్తులు[i].wl_lnum;
			// When there is a fold column it might need updating
			// in the next line ("J" just above an open fold).
			ఒకవేళ (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (శూన్య)hasFoldingWin(wp, mod_శిఖరం, &mod_శిఖరం, ఖాళీ, TRUE, ఖాళీ);
	    ఒకవేళ (mod_శిఖరం > lnumt)
		mod_శిఖరం = lnumt;

	    // Now do the same for the bottom line (one above mod_bot).
	    --mod_bot;
	    (శూన్య)hasFoldingWin(wp, mod_bot, ఖాళీ, &mod_bot, TRUE, ఖాళీ);
	    ++mod_bot;
	    ఒకవేళ (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#అయితే_ముగింపు

	// When a change starts above w_topline and the end is below
	// w_topline, start redrawing at w_topline.
	// If the end of the change is above w_topline: do like no change was
	// made, but redraw the first line to find changes in syntax.
	ఒకవేళ (mod_శిఖరం != 0 && mod_శిఖరం < wp->w_topline)
	{
	    ఒకవేళ (mod_bot > wp->w_topline)
		mod_శిఖరం = wp->w_topline;
#నిర్వచితమైతే FEAT_SYN_HL
	    లేకపోతే ఒకవేళ (syntax_present(wp))
		శిఖరం_చివర = 1;
#అయితే_ముగింపు
	}
    }
    wp->w_redraw_శిఖరం = 0;	// reset for next time
    wp->w_redraw_bot = 0;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    శోధన_hl_has_cursor_lnum = 0;
#అయితే_ముగింపు


    // When only displaying the lines at the top, set top_end.  Used when
    // window has scrolled down for msg_scrolled.
    ఒకవేళ (type == UPD_REDRAW_శిఖరం)
    {
	j = 0;
	కొరకు (i = 0; i < wp->w_పంక్తులు_valid; ++i)
	{
	    j += wp->w_పంక్తులు[i].wl_కొలత;
	    ఒకవేళ (j >= wp->w_upd_అడ్డువరుసలు)
	    {
		శిఖరం_చివర = j;
		ఆపు;
	    }
	}
	ఒకవేళ (శిఖరం_చివర == 0)
	    // not found (cannot happen?): redraw everything
	    type = UPD_NOT_VALID;
	లేకపోతే
	    // top area defined, the rest is UPD_VALID
	    type = UPD_VALID;
    }

    // Trick: we want to avoid clearing the screen twice.  screenclear() will
    // set "screen_cleared" to TRUE.  The special value MAYBE (which is still
    // non-zero and thus not FALSE) will indicate that screenclear() was not
    // called.
    ఒకవేళ (screen_cleared)
	screen_cleared = MAYBE;

    // If there are no changes on the screen that require a complete redraw,
    // handle three cases:
    // 1: we are off the top of the screen by a few lines: scroll down
    // 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
    // 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
    //    w_lines[] that needs updating.
    ఒకవేళ ((type == UPD_VALID || type == UPD_SOME_VALID
			   || type == UPD_INVERTED || type == UPD_INVERTED_ALL)
#నిర్వచితమైతే FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#అయితే_ముగింపు
	    )
    {
	ఒకవేళ (mod_శిఖరం != 0
		&& wp->w_topline == mod_శిఖరం
		&& (!wp->w_పంక్తులు[0].wl_valid
		    || wp->w_topline == wp->w_పంక్తులు[0].wl_lnum))
	{
	    // w_topline is the first changed line and window is not scrolled,
	    // the scrolling from changed lines will be done further down.
	}
	లేకపోతే ఒకవేళ (wp->w_పంక్తులు[0].wl_valid
		&& (wp->w_topline < wp->w_పంక్తులు[0].wl_lnum
#నిర్వచితమైతే FEAT_DIFF
		    || (wp->w_topline == wp->w_పంక్తులు[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#అయితే_ముగింపు
		   ))
	{
	    // New topline is above old topline: May scroll down.
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (hasAnyFolding(wp))
	    {
		linenr_T ln;

		// count the number of lines we are off, counting a sequence
		// of folded lines as one
		j = 0;
		కొరకు (ln = wp->w_topline; ln < wp->w_పంక్తులు[0].wl_lnum; ++ln)
		{
		    ++j;
		    ఒకవేళ (j >= wp->w_ఎత్తు - 2)
			ఆపు;
		    (శూన్య)hasFoldingWin(wp, ln, ఖాళీ, &ln, TRUE, ఖాళీ);
		}
	    }
	    లేకపోతే
#అయితే_ముగింపు
		j = wp->w_పంక్తులు[0].wl_lnum - wp->w_topline;
	    ఒకవేళ (j < wp->w_ఎత్తు - 2)		// not too far off
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_పంక్తులు[0].wl_lnum - 1,
								wp->w_ఎత్తు);
#నిర్వచితమైతే FEAT_DIFF
		// insert extra lines for previously invisible filler lines
		ఒకవేళ (wp->w_పంక్తులు[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_పంక్తులు[0].wl_lnum)
							  - wp->w_old_topfill;
#అయితే_ముగింపు
		ఒకవేళ (i < wp->w_ఎత్తు - 2)	// less than a screen off
		{
		    // Try to insert the correct number of lines.
		    // If not the last window, delete the lines at the bottom.
		    // win_ins_lines may fail when the terminal can't do it.
		    ఒకవేళ (i > 0)
			check_for_delay(FALSE);
		    ఒకవేళ (win_ins_పంక్తులు(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			ఒకవేళ (wp->w_పంక్తులు_valid != 0)
			{
			    // Need to update rows that are new, stop at the
			    // first one that scrolled down.
			    శిఖరం_చివర = i;
			    scrolled_కిందికి = TRUE;

			    // Move the entries that were scrolled, disable
			    // the entries for the lines to be redrawn.
			    ఒకవేళ ((wp->w_పంక్తులు_valid += j) > wp->w_ఎత్తు)
				wp->w_పంక్తులు_valid = wp->w_ఎత్తు;
			    కొరకు (idx = wp->w_పంక్తులు_valid; idx - j >= 0; idx--)
				wp->w_పంక్తులు[idx] = wp->w_పంక్తులు[idx - j];
			    అయితే (idx >= 0)
				wp->w_పంక్తులు[idx--].wl_valid = FALSE;
			}
		    }
		    లేకపోతే
			mid_ప్రారంభం = 0;		// redraw all lines
		}
		లేకపోతే
		    mid_ప్రారంభం = 0;		// redraw all lines
	    }
	    లేకపోతే
		mid_ప్రారంభం = 0;		// redraw all lines
	}
	లేకపోతే
	{
	    // New topline is at or below old topline: May scroll up.
	    // When topline didn't change, find first entry in w_lines[] that
	    // needs updating.

	    // Try to find wp->w_topline in wp->w_lines[].wl_lnum.  The check
	    // for "Rows" is in case "wl_size" is incorrect somehow.
	    j = -1;
	    అడ్డువరుస = 0;
	    కొరకు (i = 0; i < wp->w_పంక్తులు_valid && i < అడ్డువరుసలు; i++)
	    {
		ఒకవేళ (wp->w_పంక్తులు[i].wl_valid
			&& wp->w_పంక్తులు[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    ఆపు;
		}
		అడ్డువరుస += wp->w_పంక్తులు[i].wl_కొలత;
	    }
	    ఒకవేళ (j == -1)
	    {
		// if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		//పంక్తులు
		mid_ప్రారంభం = 0;
	    }
	    లేకపోతే
	    {
		// Try to delete the correct number of lines.
		// wp->w_topline is at wp->w_lines[i].wl_lnum.
#నిర్వచితమైతే FEAT_DIFF
		// If the topline didn't change, delete old filler lines,
		// otherwise delete filler lines of the new topline...
		ఒకవేళ (wp->w_పంక్తులు[0].wl_lnum == wp->w_topline)
		    అడ్డువరుస += wp->w_old_topfill;
		లేకపోతే
		    అడ్డువరుస += diff_check_fill(wp, wp->w_topline);
		// ... but don't delete new filler lines.
		అడ్డువరుస -= wp->w_topfill;
#అయితే_ముగింపు
		ఒకవేళ (అడ్డువరుస > అడ్డువరుసలు)  // just in case
		    అడ్డువరుస = అడ్డువరుసలు;
		ఒకవేళ (అడ్డువరుస > 0)
		{
		    check_for_delay(FALSE);
		    ఒకవేళ (win_del_పంక్తులు(wp, 0, అడ్డువరుస, FALSE, wp == firstwin, 0)
									 == OK)
			bot_ప్రారంభం = wp->w_ఎత్తు - అడ్డువరుస;
		    లేకపోతే
			mid_ప్రారంభం = 0;		// redraw all lines
		}
		ఒకవేళ ((అడ్డువరుస == 0 || bot_ప్రారంభం < 999) && wp->w_పంక్తులు_valid != 0)
		{
		    // Skip the lines (below the deleted lines) that are still
		    // valid and don't need redrawing.	Copy their info
		    // upwards, to compensate for the deleted lines.  Set
		    // bot_start to the first row that needs redrawing.
		    bot_ప్రారంభం = 0;
		    idx = 0;
		    కొరకు (;;)
		    {
			wp->w_పంక్తులు[idx] = wp->w_పంక్తులు[j];
			// stop at line that didn't fit, unless it is still
			// valid (no lines deleted)
			ఒకవేళ (అడ్డువరుస > 0 && bot_ప్రారంభం + అడ్డువరుస
				 + (పూర్ణాంకం)wp->w_పంక్తులు[j].wl_కొలత > wp->w_ఎత్తు)
			{
			    wp->w_పంక్తులు_valid = idx + 1;
			    ఆపు;
			}
			bot_ప్రారంభం += wp->w_పంక్తులు[idx++].wl_కొలత;

			// stop at the last valid entry in w_lines[].wl_size
			ఒకవేళ (++j >= wp->w_పంక్తులు_valid)
			{
			    wp->w_పంక్తులు_valid = idx;
			    ఆపు;
			}
		    }
#నిర్వచితమైతే FEAT_DIFF
		    // Correct the first entry for filler lines at the top
		    // when it won't get updated below.
		    ఒకవేళ (wp->w_p_diff && bot_ప్రారంభం > 0)
			wp->w_పంక్తులు[0].wl_కొలత = plines_correct_topline(wp,
							  wp->w_topline, TRUE);
#అయితే_ముగింపు
		}
	    }
	}

	// When starting redraw in the first line, redraw all lines.
	ఒకవేళ (mid_ప్రారంభం == 0)
	    mid_చివర = wp->w_ఎత్తు;

	// When win_del_lines() or win_ins_lines() caused the screen to be
	// cleared (only happens for the first window) or when screenclear()
	// was called directly above, "must_redraw" will have been set to
	// UPD_NOT_VALID, need to reset it here to avoid redrawing twice.
	ఒకవేళ (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    లేకపోతే
    {
	// Not UPD_VALID or UPD_INVERTED: redraw all lines.
	mid_ప్రారంభం = 0;
	mid_చివర = wp->w_ఎత్తు;
    }

    ఒకవేళ (type == UPD_SOME_VALID)
    {
	// UPD_SOME_VALID: redraw all lines.
	mid_ప్రారంభం = 0;
	mid_చివర = wp->w_ఎత్తు;
	type = UPD_NOT_VALID;
    }

    // check if we are updating or removing the inverted part
    ఒకవేళ ((VIsual_చురుకైన && buf == curwin->w_బఫరు)
	    || (wp->w_old_cursor_lnum != 0 && type != UPD_NOT_VALID))
    {
	linenr_T    from, గమ్యం;

	ఒకవేళ (VIsual_చురుకైన)
	{
	    ఒకవేళ (VIsual_బహుళకం != wp->w_old_visual_బహుళకం
		|| type == UPD_INVERTED_ALL)
	    {
		// If the type of Visual selection changed, redraw the whole
		// selection.  Also when the ownership of the X selection is
		// gained or lost.
		ఒకవేళ (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    గమ్యం = VIsual.lnum;
		}
		లేకపోతే
		{
		    from = VIsual.lnum;
		    గమ్యం = curwin->w_cursor.lnum;
		}
		// redraw more when the cursor moved as well
		ఒకవేళ (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		ఒకవేళ (wp->w_old_cursor_lnum > గమ్యం)
		    గమ్యం = wp->w_old_cursor_lnum;
		ఒకవేళ (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		ఒకవేళ (wp->w_old_visual_lnum > గమ్యం)
		    గమ్యం = wp->w_old_visual_lnum;
	    }
	    లేకపోతే
	    {
		// Find the line numbers that need to be updated: The lines
		// between the old cursor position and the current cursor
		// position.  Also check if the Visual position changed.
		ఒకవేళ (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    గమ్యం = wp->w_old_cursor_lnum;
		}
		లేకపోతే
		{
		    from = wp->w_old_cursor_lnum;
		    గమ్యం = curwin->w_cursor.lnum;
		    ఒకవేళ (from == 0)	// Visual mode just started
			from = గమ్యం;
		}

		ఒకవేళ (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.నిలువు != wp->w_old_visual_నిలువు)
		{
		    ఒకవేళ (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    ఒకవేళ (wp->w_old_visual_lnum > గమ్యం)
			గమ్యం = wp->w_old_visual_lnum;
		    ఒకవేళ (VIsual.lnum < from)
			from = VIsual.lnum;
		    ఒకవేళ (VIsual.lnum > గమ్యం)
			గమ్యం = VIsual.lnum;
		}
	    }

	    // If in block mode and changed column or curwin->w_curswant:
	    // update all lines.
	    // First compute the actual start and end column.
	    ఒకవేళ (VIsual_బహుళకం == Ctrl_V)
	    {
		colnr_T	    fromc, toc;

		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc,
							 GETVCOL_చివర_EXCL_LBR);
		++toc;
		// Highlight to the end of the line, unless 'virtualedit' has
		// "block".
		ఒకవేళ (curwin->w_curswant == MAXCOL)
		{
		    ఒకవేళ (get_ve_flags() & VE_ఖండం)
		    {
			pos_T	    pos;
			పూర్ణాంకం	    cursor_above =
					   curwin->w_cursor.lnum < VIsual.lnum;

			// Need to find the longest line.
			toc = 0;
			pos.coladd = 0;
			కొరకు (pos.lnum = curwin->w_cursor.lnum; cursor_above
					? pos.lnum <= VIsual.lnum
					: pos.lnum >= VIsual.lnum;
					     pos.lnum += cursor_above ? 1 : -1)
			{
			    colnr_T t;

			    pos.నిలువు = (పూర్ణాంకం)ml_get_buf_len(wp->w_బఫరు, pos.lnum);
			    getvvcol(wp, &pos, ఖాళీ, ఖాళీ, &t, 0);
			    ఒకవేళ (toc < t)
				toc = t;
			}
			++toc;
		    }
		    లేకపోతే
			toc = MAXCOL;
		}

		ఒకవేళ (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    ఒకవేళ (from > VIsual.lnum)
			from = VIsual.lnum;
		    ఒకవేళ (గమ్యం < VIsual.lnum)
			గమ్యం = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	లేకపోతే
	{
	    // Use the line numbers of the old Visual area.
	    ఒకవేళ (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		గమ్యం = wp->w_old_visual_lnum;
	    }
	    లేకపోతే
	    {
		from = wp->w_old_visual_lnum;
		గమ్యం = wp->w_old_cursor_lnum;
	    }
	}

	// There is no need to update lines above the top of the window.
	ఒకవేళ (from < wp->w_topline)
	    from = wp->w_topline;

	// If we know the value of w_botline, use it to restrict the update to
	// the lines that are visible in the window.
	// Note: w_botline may be partially visible or completely invisible.
	ఒకవేళ (wp->w_valid & VALID_BOTLINE)
	{
	    from = MIN(from, wp->w_botline);
	    గమ్యం = MIN(గమ్యం, wp->w_botline);
	}

	// Find the minimal part to be updated.
	// Watch out for scrolling that made entries in w_lines[] invalid.
	// E.g., CTRL-U makes the first half of w_lines[] invalid and sets
	// top_end; need to redraw from top_end to the "to" line.
	// A middle mouse click with a Visual selection may change the text
	// above the Visual area and reset wl_valid, do count these for
	// mid_end (in srow).
	ఒకవేళ (mid_ప్రారంభం > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    ఒకవేళ (scrolled_కిందికి)
		mid_ప్రారంభం = శిఖరం_చివర;
	    లేకపోతే
		mid_ప్రారంభం = 0;
	    అయితే (lnum < from && idx < wp->w_పంక్తులు_valid)	// find start
	    {
		ఒకవేళ (wp->w_పంక్తులు[idx].wl_valid)
		    mid_ప్రారంభం += wp->w_పంక్తులు[idx].wl_కొలత;
		లేకపోతే ఒకవేళ (!scrolled_కిందికి)
		    srow += wp->w_పంక్తులు[idx].wl_కొలత;
		++idx;
#నిర్వచితమైతే FEAT_FOLDING
		ఒకవేళ (idx < wp->w_పంక్తులు_valid && wp->w_పంక్తులు[idx].wl_valid)
		    lnum = wp->w_పంక్తులు[idx].wl_lnum;
		లేకపోతే
#అయితే_ముగింపు
		    ++lnum;
	    }
	    srow += mid_ప్రారంభం;
	    mid_చివర = wp->w_ఎత్తు;
	    కొరకు ( ; idx < wp->w_పంక్తులు_valid; ++idx)		// find end
	    {
		ఒకవేళ (wp->w_పంక్తులు[idx].wl_valid
			&& wp->w_పంక్తులు[idx].wl_lnum >= గమ్యం + 1)
		{
		    // Only update until first row of this line
		    mid_చివర = srow;
		    ఆపు;
		}
		srow += wp->w_పంక్తులు[idx].wl_కొలత;
	    }
	}
    }

    ఒకవేళ (VIsual_చురుకైన && buf == curwin->w_బఫరు)
    {
	wp->w_old_visual_బహుళకం = VIsual_బహుళకం;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_నిలువు = VIsual.నిలువు;
	wp->w_old_curswant = curwin->w_curswant;
    }
    లేకపోతే
    {
	wp->w_old_visual_బహుళకం = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_నిలువు = 0;
    }

#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_శోధన_EXTRA)
    // reset got_int, otherwise regexp won't work
    save_got_int = got_int;
    got_int = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే SYN_సమయం_హద్దు
    // Set the time limit to 'redrawtime'.
    redrawtime_హద్దు_సమితి = TRUE;
    init_regexp_timeout(p_rdt);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
    win_foldinfo.fi_level = 0;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
    // Draw the window toolbar, if there is one.
    // TODO: only when needed.
    ఒకవేళ (winbar_ఎత్తు(wp) > 0)
	redraw_win_toolbar(wp);
#అయితే_ముగింపు

    lnum = wp->w_topline;   // first line shown in window

    spellvars_T spv;
#నిర్వచితమైతే FEAT_SPELL
    // Initialize spell related variables for the first drawn line.
    CLEAR_క్షేత్రం(spv);
    ఒకవేళ (spell_check_కిటికీ(wp))
    {
	spv.spv_has_spell = TRUE;
	spv.spv_unchanged = mod_శిఖరం == 0;
    }
#అయితే_ముగింపు

    // Update all the window rows.
    idx = 0;		// first entry in w_lines[].wl_size
    అడ్డువరుస = 0;
    srow = 0;
    కొరకు (;;)
    {
	// stop updating when reached the end of the window (check for _past_
	// the end of the window is at the end of the loop)
	ఒకవేళ (అడ్డువరుస == wp->w_ఎత్తు)
	{
	    didline = TRUE;
	    ఆపు;
	}

	// stop updating when hit the end of the file
	ఒకవేళ (lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	{
	    eof = TRUE;
	    ఆపు;
	}

	// Remember the starting row of the line that is going to be dealt
	// with.  It is used further down when the line doesn't fit.
	srow = అడ్డువరుస;

	// Update a line when it is in an area that needs updating, when it
	// has changes or w_lines[idx] is invalid.
	// "bot_start" may be halfway a wrapped line after using
	// win_del_lines(), check if the current line includes it.
	// When syntax folding is being used, the saved syntax states will
	// already have been updated, we can't see where the syntax state is
	// the same again, just update until the end of the window.
	ఒకవేళ (అడ్డువరుస < శిఖరం_చివర
		|| (అడ్డువరుస >= mid_ప్రారంభం && అడ్డువరుస < mid_చివర)
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		|| శిఖరం_గమ్యం_mod
#అయితే_ముగింపు
		|| idx >= wp->w_పంక్తులు_valid
		|| (అడ్డువరుస + wp->w_పంక్తులు[idx].wl_కొలత > bot_ప్రారంభం)
		|| (mod_శిఖరం != 0
		    && (lnum == mod_శిఖరం
			|| (lnum >= mod_శిఖరం
			    && (lnum < mod_bot
#నిర్వచితమైతే FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_పంక్తి
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_మారింది(lnum)))
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_శోధన_EXTRA
				// match in fixed position might need redraw
				// if lines were inserted or deleted
				|| (wp->w_సరిపోలిక_తల != ఖాళీ
						    && buf->b_mod_సమితి
						    && buf->b_mod_xlines != 0)
#అయితే_ముగింపు
				))))
#నిర్వచితమైతే FEAT_SYN_HL
		|| (wp->w_p_cul && lnum == wp->w_cursor.lnum)
		|| lnum == wp->w_చివరి_cursorline
#అయితే_ముగింపు
				)
	{
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	    ఒకవేళ (lnum == mod_శిఖరం)
		శిఖరం_గమ్యం_mod = FALSE;
#అయితే_ముగింపు

	    // When at start of changed lines: May scroll following lines
	    // up or down to minimize redrawing.
	    // Don't do this when the change continues until the end.
	    // Don't scroll for changed lines in the top area if that's already
	    // done above, but do scroll for changed lines below the top area.
	    ఒకవేళ (!scrolled_for_mod && mod_bot != MAXLNUM
		    && lnum >= mod_శిఖరం && lnum < MAX(mod_bot, mod_శిఖరం + 1)
		    && (!scrolled_కిందికి || అడ్డువరుస >= శిఖరం_చివర))
	    {
		scrolled_for_mod = TRUE;

		పూర్ణాంకం		old_cline_ఎత్తు = 0;
		పూర్ణాంకం		old_అడ్డువరుసలు = 0;
		పూర్ణాంకం		new_అడ్డువరుసలు = 0;
		పూర్ణాంకం		xtra_అడ్డువరుసలు;
		linenr_T	l;

		// Count the old number of window rows, using w_lines[], which
		// should still contain the sizes for the lines as they are
		// currently displayed.
		కొరకు (i = idx; i < wp->w_పంక్తులు_valid; ++i)
		{
		    // Only valid lines have a meaningful wl_lnum.  Invalid
		    // lines are part of the changed area.
		    ఒకవేళ (wp->w_పంక్తులు[i].wl_valid
			    && wp->w_పంక్తులు[i].wl_lnum == mod_bot)
			ఆపు;
		    ఒకవేళ (wp->w_పంక్తులు[i].wl_lnum == wp->w_cursor.lnum)
			old_cline_ఎత్తు = wp->w_పంక్తులు[i].wl_కొలత;
		    old_అడ్డువరుసలు += wp->w_పంక్తులు[i].wl_కొలత;
#నిర్వచితమైతే FEAT_FOLDING
		    ఒకవేళ (wp->w_పంక్తులు[i].wl_valid
			    && wp->w_పంక్తులు[i].wl_lastlnum + 1 == mod_bot)
		    {
			// Must have found the last valid entry above mod_bot.
			// Add following invalid entries.
			++i;
			అయితే (i < wp->w_పంక్తులు_valid
						  && !wp->w_పంక్తులు[i].wl_valid)
			    old_అడ్డువరుసలు += wp->w_పంక్తులు[i++].wl_కొలత;
			ఆపు;
		    }
#అయితే_ముగింపు
		}

		ఒకవేళ (i >= wp->w_పంక్తులు_valid)
		{
		    // We can't find a valid line below the changed lines,
		    // need to redraw until the end of the window.
		    // Inserting/deleting lines has no use.
		    bot_ప్రారంభం = 0;
		}
		లేకపోతే
		{
		    // Able to count old number of rows: Count new window
		    // rows, and may insert/delete lines
		    j = idx;
		    కొరకు (l = lnum; l < mod_bot; ++l)
		    {
			ఒకవేళ (dollar_vcol >= 0 && wp == curwin &&
				old_cline_ఎత్తు > 0 && l == wp->w_cursor.lnum)
			    // When dollar_vcol >= 0, cursor line isn't fully
			    // redrawn, and its height remains unchanged.
			    new_అడ్డువరుసలు += old_cline_ఎత్తు;
#నిర్వచితమైతే FEAT_FOLDING
			లేకపోతే ఒకవేళ (hasFoldingWin(wp, l, ఖాళీ, &l, TRUE, ఖాళీ))
			    ++new_అడ్డువరుసలు;
#అయితే_ముగింపు
			లేకపోతే
			    new_అడ్డువరుసలు += plines_correct_topline(wp, l, TRUE);
			++j;
			ఒకవేళ (new_అడ్డువరుసలు > wp->w_ఎత్తు - అడ్డువరుస - 2)
			{
			    // it's getting too much, must redraw the rest
			    new_అడ్డువరుసలు = 9999;
			    ఆపు;
			}
		    }
		    xtra_అడ్డువరుసలు = new_అడ్డువరుసలు - old_అడ్డువరుసలు;
		    ఒకవేళ (xtra_అడ్డువరుసలు < 0)
		    {
			// May scroll text up.  If there is not enough
			// remaining text or scrolling fails, must redraw the
			// rest.  If scrolling works, must redraw the text
			// below the scrolled text.
			ఒకవేళ (అడ్డువరుస - xtra_అడ్డువరుసలు >= wp->w_ఎత్తు - 2)
			    mod_bot = MAXLNUM;
			లేకపోతే
			{
			    check_for_delay(FALSE);
			    ఒకవేళ (win_del_పంక్తులు(wp, అడ్డువరుస,
					  -xtra_అడ్డువరుసలు, FALSE, FALSE, 0) == FAIL)
				mod_bot = MAXLNUM;
			    లేకపోతే
				bot_ప్రారంభం = wp->w_ఎత్తు + xtra_అడ్డువరుసలు;
			}
		    }
		    లేకపోతే ఒకవేళ (xtra_అడ్డువరుసలు > 0)
		    {
			// May scroll text down.  If there is not enough
			// remaining text of scrolling fails, must redraw the
			// rest.
			ఒకవేళ (అడ్డువరుస + xtra_అడ్డువరుసలు >= wp->w_ఎత్తు - 2)
			    mod_bot = MAXLNUM;
			లేకపోతే
			{
			    check_for_delay(FALSE);
			    ఒకవేళ (win_ins_పంక్తులు(wp, అడ్డువరుస + old_అడ్డువరుసలు,
					     xtra_అడ్డువరుసలు, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    లేకపోతే ఒకవేళ (శిఖరం_చివర > అడ్డువరుస + old_అడ్డువరుసలు)
				// Scrolled the part at the top that requires
				// updating down.
				శిఖరం_చివర += xtra_అడ్డువరుసలు;
			}
		    }

		    // When not updating the rest, may need to move w_lines[]
		    // entries.
		    ఒకవేళ (mod_bot != MAXLNUM && i != j)
		    {
			ఒకవేళ (j < i)
			{
			    పూర్ణాంకం x = అడ్డువరుస + new_అడ్డువరుసలు;

			    // move entries in w_lines[] upwards
			    కొరకు (;;)
			    {
				// stop at last valid entry in w_lines[]
				ఒకవేళ (i >= wp->w_పంక్తులు_valid)
				{
				    wp->w_పంక్తులు_valid = j;
				    ఆపు;
				}
				wp->w_పంక్తులు[j] = wp->w_పంక్తులు[i];
				// stop at a line that won't fit
				ఒకవేళ (x + (పూర్ణాంకం)wp->w_పంక్తులు[j].wl_కొలత
							   > wp->w_ఎత్తు)
				{
				    wp->w_పంక్తులు_valid = j + 1;
				    ఆపు;
				}
				x += wp->w_పంక్తులు[j++].wl_కొలత;
				++i;
			    }
			    ఒకవేళ (bot_ప్రారంభం > x)
				bot_ప్రారంభం = x;
			}
			లేకపోతే // j > i
			{
			    // move entries in w_lines[] downwards
			    j -= i;
			    wp->w_పంక్తులు_valid += j;
			    ఒకవేళ (wp->w_పంక్తులు_valid > wp->w_ఎత్తు)
				wp->w_పంక్తులు_valid = wp->w_ఎత్తు;
			    కొరకు (i = wp->w_పంక్తులు_valid; i - j >= idx; --i)
				wp->w_పంక్తులు[i] = wp->w_పంక్తులు[i - j];

			    // The w_lines[] entries for inserted lines are
			    // now invalid, but wl_size may be used above.
			    // Reset to zero.
			    అయితే (i >= idx)
			    {
				wp->w_పంక్తులు[i].wl_కొలత = 0;
				wp->w_పంక్తులు[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#నిర్వచితమైతే FEAT_FOLDING
	    // When lines are folded, display one line for all of them.
	    // Otherwise, display normally (can be several display lines when
	    // 'wrap' is on).
	    fold_లెక్క = folded_లెక్క(wp, lnum, &win_foldinfo);
	    ఒకవేళ (fold_లెక్క != 0)
	    {
		fold_పంక్తి(wp, fold_లెక్క, &win_foldinfo, lnum, అడ్డువరుస);
		++అడ్డువరుస;
		--fold_లెక్క;
		wp->w_పంక్తులు[idx].wl_folded = TRUE;
		wp->w_పంక్తులు[idx].wl_lastlnum = lnum + fold_లెక్క;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
# ifdef FEAT_SPELL
		spv.spv_capcol_lnum = 0;
# endif
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (idx < wp->w_పంక్తులు_valid
		    && wp->w_పంక్తులు[idx].wl_valid
		    && wp->w_పంక్తులు[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & (DY_LASTLINE | DY_TRUNCATE))
		    && !WIN_IS_POPUP(wp)
		    && srow + wp->w_పంక్తులు[idx].wl_కొలత > wp->w_ఎత్తు
#నిర్వచితమైతే FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#అయితే_ముగింపు
		    )
	    {
		// This line is not going to fit.  Don't draw anything here,
		// will draw "@  " lines below.
		అడ్డువరుస = wp->w_ఎత్తు + 1;
	    }
	    లేకపోతే
	    {
#నిర్వచితమైతే FEAT_శోధన_EXTRA
		prepare_శోధన_hl(wp, &screen_శోధన_hl, lnum);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
		// Let the syntax stuff know we skipped a few lines.
		ఒకవేళ (syntax_చివరి_parsed != 0 && syntax_చివరి_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_చివర_parsing(wp, syntax_చివరి_parsed + 1);
#అయితే_ముగింపు

		// Display one line.
		అడ్డువరుస = win_పంక్తి(wp, lnum, srow, wp->w_ఎత్తు, 0, &spv);

#నిర్వచితమైతే FEAT_FOLDING
		wp->w_పంక్తులు[idx].wl_folded = FALSE;
		wp->w_పంక్తులు[idx].wl_lastlnum = lnum;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
		did_update = DID_పంక్తి;
		syntax_చివరి_parsed = lnum;
#అయితే_ముగింపు
	    }

	    wp->w_పంక్తులు[idx].wl_lnum = lnum;
	    wp->w_పంక్తులు[idx].wl_valid = TRUE;

	    పూర్ణాంకం is_curline = wp == curwin && lnum == wp->w_cursor.lnum;

	    // Past end of the window or end of the screen. Note that after
	    // resizing wp->w_height may be end up too big. That's a problem
	    // elsewhere, but prevent a crash here.
	    ఒకవేళ (అడ్డువరుస > wp->w_ఎత్తు || అడ్డువరుస + wp->w_winrow >= అడ్డువరుసలు)
	    {
		// we may need the size of that too long line later on
		ఒకవేళ (dollar_vcol == -1 || !is_curline)
		    wp->w_పంక్తులు[idx].wl_కొలత = plines_win(wp, lnum, TRUE);
		++idx;
		ఆపు;
	    }
	    ఒకవేళ (dollar_vcol == -1 || !is_curline)
		wp->w_పంక్తులు[idx].wl_కొలత = అడ్డువరుస - srow;
	    ++idx;
#నిర్వచితమైతే FEAT_FOLDING
	    lnum += fold_లెక్క + 1;
#లేకపోతే
	    ++lnum;
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    // If:
	    // - 'number' is set and below inserted/deleted lines, or
	    // - 'relativenumber' is set and cursor moved vertically,
	    // the text doesn't need to be redrawn, but the number column does.
	    ఒకవేళ ((wp->w_p_nu && mod_శిఖరం != 0 && lnum >= mod_bot
			&& buf->b_mod_సమితి && buf->b_mod_xlines != 0)
		    || (wp->w_p_rnu
			&& wp->w_చివరి_cursor_lnum_rnu != wp->w_cursor.lnum))
	    {
#నిర్వచితమైతే FEAT_FOLDING
		fold_లెక్క = folded_లెక్క(wp, lnum, &win_foldinfo);
		ఒకవేళ (fold_లెక్క != 0)
		    fold_పంక్తి(wp, fold_లెక్క, &win_foldinfo, lnum, అడ్డువరుస);
		లేకపోతే
#అయితే_ముగింపు
		    (శూన్య)win_పంక్తి(wp, lnum, srow, wp->w_ఎత్తు,
					       wp->w_పంక్తులు[idx].wl_కొలత, &spv);
	    }

	    // This line does not need to be drawn, advance to the next one.
	    అడ్డువరుస += wp->w_పంక్తులు[idx++].wl_కొలత;
	    ఒకవేళ (అడ్డువరుస > wp->w_ఎత్తు)	// past end of screen
		ఆపు;
#నిర్వచితమైతే FEAT_FOLDING
	    lnum = wp->w_పంక్తులు[idx - 1].wl_lastlnum + 1;
#లేకపోతే
	    ++lnum;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
	    did_update = DID_NONE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
	    spv.spv_capcol_lnum = 0;
#అయితే_ముగింపు
	}

	ఒకవేళ (lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	{
	    eof = TRUE;
	    ఆపు;
	}

	// Safety check: if any of the wl_size values is wrong we might go over
	// the end of w_lines[].
	ఒకవేళ (idx >= అడ్డువరుసలు)
	    ఆపు;
    }

    // End of loop over all window lines.

#నిర్వచితమైతే FEAT_SYN_HL
    // Now that the window has been redrawn with the old and new cursor line,
    // update w_last_cursorline.
    wp->w_చివరి_cursorline = wp->w_p_cul ? wp->w_cursor.lnum : 0;
#అయితే_ముగింపు
    wp->w_చివరి_cursor_lnum_rnu = wp->w_p_rnu ? wp->w_cursor.lnum : 0;

#నిర్వచితమైతే FEAT_VTP
    // Rewrite the character at the end of the screen line.
    // See the version that was fixed.
    ఒకవేళ (use_vtp() && get_conpty_fix_type() < 1)
    {
	పూర్ణాంకం k;

	కొరకు (k = 0; k < అడ్డువరుసలు; ++k)
	    ఒకవేళ (enc_utf8)
		ఒకవేళ ((*mb_off2cells)(పంక్తి_స్థానభ్రంశం[k] + topframe->fr_వెడల్పు - 2,
					   పంక్తి_స్థానభ్రంశం[k] + screen_Columns) > 1)
		    screen_draw_rectangle(k, topframe->fr_వెడల్పు - 2, 1, 2,
			    FALSE, FALSE);
		లేకపోతే
		    screen_draw_rectangle(k, topframe->fr_వెడల్పు - 1, 1, 1,
			    FALSE, FALSE);
	    లేకపోతే
		screen_char(పంక్తి_స్థానభ్రంశం[k] + topframe->fr_వెడల్పు - 1, k,
			cmdline_వెడల్పు - 1);
    }
#అయితే_ముగింపు

    ఒకవేళ (idx > wp->w_పంక్తులు_valid)
	wp->w_పంక్తులు_valid = idx;

#నిర్వచితమైతే FEAT_SYN_HL
    // Let the syntax stuff know we stop parsing here.
    ఒకవేళ (syntax_చివరి_parsed != 0 && syntax_present(wp))
	syntax_చివర_parsing(wp, syntax_చివరి_parsed + 1);
#అయితే_ముగింపు

    // If we didn't hit the end of the file, and we didn't finish the last
    // line we were working on, then the line didn't fit.
    wp->w_ఖాళీలు_అడ్డువరుసలు = 0;
#నిర్వచితమైతే FEAT_DIFF
    wp->w_filler_అడ్డువరుసలు = 0;
#అయితే_ముగింపు
    ఒకవేళ (!eof && !didline)
    {
	ఒకవేళ (lnum == wp->w_topline)
	{
	    // Single line that does not fit!
	    // Don't overwrite it, it can be edited.
	    wp->w_botline = lnum + 1;
	}
#నిర్వచితమైతే FEAT_DIFF
	లేకపోతే ఒకవేళ (diff_check_fill(wp, lnum) >= wp->w_ఎత్తు - srow)
	{
	    // Window ends in filler lines.
	    wp->w_botline = lnum;
	    wp->w_filler_అడ్డువరుసలు = wp->w_ఎత్తు - srow;
	}
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
	లేకపోతే ఒకవేళ (WIN_IS_POPUP(wp))
	{
	    // popup line that doesn't fit is left as-is
	    wp->w_botline = lnum;
	}
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (dy_flags & DY_TRUNCATE)	// 'display' has "truncate"
	{
	    పూర్ణాంకం		scr_అడ్డువరుస = W_WINROW(wp) + wp->w_ఎత్తు - 1;
	    పూర్ణాంకం		చిహ్నం  = wp->w_fill_chars.lastline;
	    పూర్ణాంకం		charlen;
	    char_u	fillbuf[12];  // 2 characters of 6 bytes

	    charlen = mb_char2bytes(చిహ్నం, &fillbuf[0]);
	    mb_char2bytes(చిహ్నం, &fillbuf[charlen]);

	    // Last line isn't finished: Display "@@@" in the last screen line.
	    screen_puts_len(fillbuf,
			    (wp->w_వెడల్పు > 2 ? 2 : wp->w_వెడల్పు) * charlen,
			    scr_అడ్డువరుస, wp->w_wincol, HL_ATTR(HLF_AT));
	    screen_fill(scr_అడ్డువరుస, scr_అడ్డువరుస + 1,
		    (పూర్ణాంకం)wp->w_wincol + 2, (పూర్ణాంకం)W_ENDCOL(wp),
		    చిహ్నం, ' ', HL_ATTR(HLF_AT));
	    సమితి_ఖాళీలు_అడ్డువరుసలు(wp, srow);
	    wp->w_botline = lnum;
	}
	లేకపోతే ఒకవేళ (dy_flags & DY_LASTLINE)	// 'display' has "lastline"
	{
	    పూర్ణాంకం ప్రారంభం_నిలువు = (పూర్ణాంకం)W_ENDCOL(wp) - 3;
	    పూర్ణాంకం చిహ్నం    = wp->w_fill_chars.lastline;

	    // Last line isn't finished: Display "@@@" at the end.
	    screen_fill(W_WINROW(wp) + wp->w_ఎత్తు - 1,
		    W_WINROW(wp) + wp->w_ఎత్తు,
		    (ప్రారంభం_నిలువు < wp->w_wincol ? wp->w_wincol : ప్రారంభం_నిలువు),
		    (పూర్ణాంకం)W_ENDCOL(wp),
		    చిహ్నం, చిహ్నం, HL_ATTR(HLF_AT));
	    సమితి_ఖాళీలు_అడ్డువరుసలు(wp, srow);
	    wp->w_botline = lnum;
	}
	లేకపోతే
	{
	    win_draw_చివర(wp, wp->w_fill_chars.lastline, ' ', TRUE,
						   srow, wp->w_ఎత్తు, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    లేకపోతే
    {
	draw_vsep_win(wp, అడ్డువరుస);
	ఒకవేళ (eof)		// we hit the end of the file
	{
	    wp->w_botline = buf->b_ml.ml_పంక్తి_లెక్క + 1;
#నిర్వచితమైతే FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    ఒకవేళ (j > 0 && !wp->w_botfill)
	    {
		// Display filler lines at the end of the file.
		ఒకవేళ (char2cells(wp->w_fill_chars.diff) > 1)
		    i = '-';
		లేకపోతే
		    i = wp->w_fill_chars.diff;
		ఒకవేళ (అడ్డువరుస + j > wp->w_ఎత్తు)
		    j = wp->w_ఎత్తు - అడ్డువరుస;
		win_draw_చివర(wp, i, i, TRUE, అడ్డువరుస, అడ్డువరుస + (పూర్ణాంకం)j, HLF_DED);
		అడ్డువరుస += j;
	    }
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (dollar_vcol == -1 || wp != curwin)
	    wp->w_botline = lnum;

	// Make sure the rest of the screen is blank.
	// write the "eob" character from 'fillchars' to rows that aren't part
	// of the file.
	ఒకవేళ (WIN_IS_POPUP(wp))
	    win_draw_చివర(wp, ' ', ' ', FALSE, అడ్డువరుస, wp->w_ఎత్తు, HLF_AT);
	లేకపోతే
	    win_draw_చివర(wp, wp->w_fill_chars.eob, ' ', FALSE,
						   అడ్డువరుస, wp->w_ఎత్తు, HLF_EOB);
    }

#నిర్వచితమైతే SYN_సమయం_హద్దు
    disable_regexp_timeout();
    redrawtime_హద్దు_సమితి = FALSE;
#అయితే_ముగింపు

    // Reset the type of redrawing required, the window has been updated.
    wp->w_redr_type = 0;
#నిర్వచితమైతే FEAT_DIFF
    wp->w_old_topfill = wp->w_topfill;
    wp->w_old_botfill = wp->w_botfill;
#అయితే_ముగింపు

    ఒకవేళ (dollar_vcol == -1 || wp != curwin)
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
	ఒకవేళ (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    win_T	*wwp;
#ఒకవేళ defined(FEAT_CONCEAL)
	    linenr_T	old_topline = wp->w_topline;
	    పూర్ణాంకం		new_wcol = wp->w_wcol;
#అయితే_ముగింపు
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	// may invalidate w_botline again

#ఒకవేళ defined(FEAT_CONCEAL)
	    ఒకవేళ (old_wcol != new_wcol && (wp->w_valid & (VALID_WCOL|VALID_WROW))
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
		ఒకవేళ (wp->w_wcol == old_wcol && wp->w_wrow == old_wrow
					       && old_topline == wp->w_topline)
		    wp->w_wcol = new_wcol;
		లేకపోతే
		    redrawWinline(wp, wp->w_cursor.lnum);
	    }
#అయితే_ముగింపు
	    // New redraw either due to updated topline, wcol fix or reset skipcol.
	    ఒకవేళ (wp->w_redr_type != 0)
	    {
		// Don't update for changes in buffer again.
		bool b = curbuf->b_mod_సమితి;
		curbuf->b_mod_సమితి = false;
		j = curbuf->b_mod_xlines;
		curbuf->b_mod_xlines = 0;
		curs_columns(TRUE);
		win_update(curwin);
		curbuf->b_mod_సమితి = b;
		curbuf->b_mod_xlines = j;
	    }
	    // Other windows might have w_redr_type raised in update_topline().
	    must_redraw = 0;
	    FOR_ALL_WINDOWS(wwp)
		ఒకవేళ (wwp->w_redr_type > must_redraw)
		    must_redraw = wwp->w_redr_type;
	    recursive = FALSE;
	}
    }

#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_శోధన_EXTRA)
    // restore got_int, unless CTRL-C was hit while redrawing
    ఒకవేళ (!got_int)
	got_int = save_got_int;
#అయితే_ముగింపు

    ఒకవేళ (override_success)
	తీయి_highlight_overrides();
}

#ఒకవేళ defined(FEAT_NETBEANS_INTG) || defined(FEAT_GUI)
/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    స్థిర శూన్య
update_prepare(శూన్య)
{
    cursor_off();
    updating_screen = TRUE;
# ifdef FEAT_GUI
    // Remove the cursor before starting to do anything, because scrolling may
    // make it difficult to redraw the text under it.
    ఒకవేళ (gui.లోపల_use)
	gui_undraw_cursor();
# endif
# ifdef FEAT_శోధన_EXTRA
    ప్రారంభం_శోధన_hl();
# endif
# ifdef FEAT_PROP_POPUP
    // Update popup_mask if needed.
    may_update_popup_ముసుగు(must_redraw);
# endif
}

/*
 * Finish updating one or more windows.
 */
    స్థిర శూన్య
update_ముగింపు(శూన్య)
{
    ఒకవేళ (redraw_cmdline || redraw_బహుళకం)
	showmode();

# ifdef FEAT_శోధన_EXTRA
    చివర_శోధన_hl();
# endif

    after_updating_screen(TRUE);

# ifdef FEAT_GUI
    // Redraw the cursor and update the scrollbars when all screen updating is
    // done.
    ఒకవేళ (gui.లోపల_use)
    {
	బహిర్గతం_flush_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_NETBEANS_INTG)
    శూన్య
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    పూర్ణాంకం		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    // update/delete a specific sign
    redraw_buf_పంక్తి_later(buf, lnum);

    // check if it resulted in the need to redraw a window
    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_redr_type != 0)
	    doit = TRUE;

    // Return when there is nothing to do, screen updating is already
    // happening (recursive call), messages on the screen or still starting up.
    ఒకవేళ (!doit || updating_screen
	    || State == బహుళకం_ASKMORE || State == బహుళకం_HITRETURN
	    || సందేశం_scrolled
# ifdef FEAT_GUI
	    || gui.starting
# endif
	    || starting)
	తిరిగి;

    // update all windows that need updating
    update_prepare();

    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (wp->w_redr_type != 0)
	    win_update(wp);
	ఒకవేళ (wp->w_redr_స్థితి)
	    win_redr_స్థితి(wp, FALSE);
    }

# ఒకవేళ defined(FEAT_TABPANEL)
    ఒకవేళ (redraw_tabpanel)
	draw_tabpanel();
# endif

    update_ముగింపు();
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    శూన్య
update_కిటికీ(win_T *wp)
{
    // return if already busy updating
    ఒకవేళ (updating_screen)
	తిరిగి;

    update_prepare();

# ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.
    ఒకవేళ (clip_నక్షత్రం.అందుబాటు && clip_isautosel_నక్షత్రం())
	clip_update_selection(&clip_నక్షత్రం);
    ఒకవేళ (clip_plus.అందుబాటు && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
# endif

    win_update(wp);

    // When the screen was cleared redraw the tab pages line.
    ఒకవేళ (redraw_tabline)
	draw_tabline();

# ఒకవేళ defined(FEAT_TABPANEL)
    ఒకవేళ (redraw_tabpanel)
	draw_tabpanel();
# endif

    ఒకవేళ (wp->w_redr_స్థితి || p_ru
# ifdef FEAT_STL_OPT
	    || *p_stl != NUL || *wp->w_p_stl != NUL
# endif
	    )
	win_redr_స్థితి(wp, FALSE);

# ifdef FEAT_PROP_POPUP
    // Display popup windows on top of everything.
    update_popups(win_update);
# endif

    update_ముగింపు();
}
#అయితే_ముగింపు

/*
 * Redraw as soon as possible.  When the command line is not scrolled redraw
 * right away and restore what was on the command line.
 * Return a code indicating what happened.
 */
    పూర్ణాంకం
redraw_asap(పూర్ణాంకం type)
{
    పూర్ణాంకం		అడ్డువరుసలు_2;
    పూర్ణాంకం		నిలువుల_సంఖ్య = screen_Columns;
    పూర్ణాంకం		r;
    పూర్ణాంకం		ret = 0;
    schar_T	*screenline;	// copy from ScreenLines[]
    sattr_T	*screenattr;	// copy from ScreenAttrs[]
    పూర్ణాంకం		i;
    u8char_T	*screenlineUC = ఖాళీ;	// copy from ScreenLinesUC[]
    u8char_T	*screenlineC[MAX_MCO];	// copy from ScreenLinesC[][]
    schar_T	*screenline2 = ఖాళీ;	// copy from ScreenLines2[]

    redraw_later(type);
    ఒకవేళ (సందేశం_scrolled
	    || (State != బహుళకం_సాధారణ && State != బహుళకం_సాధారణ_BUSY)
	    || exiting)
	తిరిగి ret;

    // Allocate space to save the text displayed in the command line area.
    అడ్డువరుసలు_2 = screen_అడ్డువరుసలు - cmdline_అడ్డువరుస;
    screenline = LALLOC_MULT(schar_T, అడ్డువరుసలు_2 * నిలువుల_సంఖ్య);
    screenattr = LALLOC_MULT(sattr_T, అడ్డువరుసలు_2 * నిలువుల_సంఖ్య);
    ఒకవేళ (screenline == ఖాళీ || screenattr == ఖాళీ)
	ret = 2;
    ఒకవేళ (enc_utf8)
    {
	screenlineUC = LALLOC_MULT(u8char_T, అడ్డువరుసలు_2 * నిలువుల_సంఖ్య);
	ఒకవేళ (screenlineUC == ఖాళీ)
	    ret = 2;
	కొరకు (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = LALLOC_MULT(u8char_T, అడ్డువరుసలు_2 * నిలువుల_సంఖ్య);
	    ఒకవేళ (screenlineC[i] == ఖాళీ)
		ret = 2;
	}
    }
    ఒకవేళ (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = LALLOC_MULT(schar_T, అడ్డువరుసలు_2 * నిలువుల_సంఖ్య);
	ఒకవేళ (screenline2 == ఖాళీ)
	    ret = 2;
    }

    ఒకవేళ (ret != 2)
    {
	// Save the text displayed in the command line area.
	కొరకు (r = 0; r < అడ్డువరుసలు_2; ++r)
	{
	    mch_memmove(screenline + r * నిలువుల_సంఖ్య,
			Screen_పంక్తులు + పంక్తి_స్థానభ్రంశం[cmdline_అడ్డువరుస + r],
			(size_t)నిలువుల_సంఖ్య * పరిమాణం(schar_T));
	    mch_memmove(screenattr + r * నిలువుల_సంఖ్య,
			ScreenAttrs + పంక్తి_స్థానభ్రంశం[cmdline_అడ్డువరుస + r],
			(size_t)నిలువుల_సంఖ్య * పరిమాణం(sattr_T));
	    ఒకవేళ (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * నిలువుల_సంఖ్య,
			    Screen_పంక్తులు_UC + పంక్తి_స్థానభ్రంశం[cmdline_అడ్డువరుస + r],
			    (size_t)నిలువుల_సంఖ్య * పరిమాణం(u8char_T));
		కొరకు (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * నిలువుల_సంఖ్య,
				Screen_పంక్తులు_C[i] + పంక్తి_స్థానభ్రంశం[cmdline_అడ్డువరుస + r],
				(size_t)నిలువుల_సంఖ్య * పరిమాణం(u8char_T));
	    }
	    ఒకవేళ (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * నిలువుల_సంఖ్య,
			    ScreenLines2 + పంక్తి_స్థానభ్రంశం[cmdline_అడ్డువరుస + r],
			    (size_t)నిలువుల_సంఖ్య * పరిమాణం(schar_T));
	}

	update_screen(0);
	ret = 3;

	ఒకవేళ (must_redraw == 0)
	{
	    పూర్ణాంకం	off = (పూర్ణాంకం)(current_Screen_పంక్తి - Screen_పంక్తులు);

	    // Restore the text displayed in the command line area.
	    కొరకు (r = 0; r < అడ్డువరుసలు_2; ++r)
	    {
		mch_memmove(current_Screen_పంక్తి,
			    screenline + r * నిలువుల_సంఖ్య,
			    (size_t)నిలువుల_సంఖ్య * పరిమాణం(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * నిలువుల_సంఖ్య,
			    (size_t)నిలువుల_సంఖ్య * పరిమాణం(sattr_T));
		ఒకవేళ (enc_utf8)
		{
		    mch_memmove(Screen_పంక్తులు_UC + off,
				screenlineUC + r * నిలువుల_సంఖ్య,
				(size_t)నిలువుల_సంఖ్య * పరిమాణం(u8char_T));
		    కొరకు (i = 0; i < p_mco; ++i)
			mch_memmove(Screen_పంక్తులు_C[i] + off,
				    screenlineC[i] + r * నిలువుల_సంఖ్య,
				    (size_t)నిలువుల_సంఖ్య * పరిమాణం(u8char_T));
		}
		ఒకవేళ (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * నిలువుల_సంఖ్య,
				(size_t)నిలువుల_సంఖ్య * పరిమాణం(schar_T));
		screen_పంక్తి(curwin, cmdline_అడ్డువరుస + r, 0, నిలువుల_సంఖ్య, నిలువుల_సంఖ్య, -1, 0);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
    ఒకవేళ (enc_utf8)
    {
	vim_free(screenlineUC);
	కొరకు (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    ఒకవేళ (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);

    // Show the intro message when appropriate.
    maybe_intro_సందేశం();

    setcursor();

    తిరిగి ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 * If "call_update_screen" is FALSE don't call update_screen() when at the
 * command line.
 * If "redraw_message" is TRUE.
 */
    శూన్య
redraw_after_callback(పూర్ణాంకం call_update_screen, పూర్ణాంకం do_సందేశం_2)
{
    ++redrawing_for_callback;

    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_ENABLE);

    ఒకవేళ (State == బహుళకం_HITRETURN || State == బహుళకం_ASKMORE
	    || State == బహుళకం_SETWSIZE || State == బహుళకం_EXTERNCMD
	    || State == బహుళకం_CONFIRM || exmode_చురుకైన)
    {
	ఒకవేళ (do_సందేశం_2)
	    repeat_సందేశం();
    }
    లేకపోతే ఒకవేళ (State & బహుళకం_CMDLINE)
    {
	ఒకవేళ (pum_visible())
	    cmdline_pum_display();

	// Don't redraw when in prompt_for_number().
	ఒకవేళ (cmdline_అడ్డువరుస > 0)
	{
	    // Redrawing only works when the screen didn't scroll. Don't clear
	    // wildmenu entries.
	    ఒకవేళ (సందేశం_scrolled == 0
		    && wild_menu_showing == 0
		    && call_update_screen)
		update_screen(0);

	    // Redraw in the same position, so that the user can continue
	    // editing the command.
	    redrawcmdline_ex(FALSE);
	}
    }
    లేకపోతే ఒకవేళ (State & (బహుళకం_సాధారణ | బహుళకం_చొప్పించు | బహుళకం_TERMINAL))
    {
	update_topline();
	validate_cursor();

	// keep the command line if possible
	update_screen(UPD_VALID_NO_UPDATE);
	setcursor();

	ఒకవేళ (సందేశం_scrolled == 0)
	{
	    // don't want a hit-enter prompt when something else is displayed
	    సందేశం_didany = FALSE;
	    need_wait_return = FALSE;
	}
    }
    cursor_on();
    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_DISABLE);
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use && !gui_mch_is_blink_off())
	// Don't update the cursor when it is blinking and off to avoid
	// flicker.
	బహిర్గతం_flush_cursor(FALSE, FALSE);
    లేకపోతే
#అయితే_ముగింపు
	బహిర్గతం_flush();

    --redrawing_for_callback;
}

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * E.g. if must_redraw is UPD_CLEAR, type UPD_NOT_VALID will do nothing.
 */
    శూన్య
redraw_later(పూర్ణాంకం type)
{
    redraw_win_later(curwin, type);
}

    శూన్య
redraw_win_later(
    win_T	*wp,
    పూర్ణాంకం		type)
{
#నిర్వచితమైతే FEAT_EVAL
    // If inside a redraw_listener_add() callback, then only set the redraw type
    // for the window and not request another one right after.
    ఒకవేళ (inside_redraw_on_ప్రారంభం_cb && wp->w_redr_type < type)
	wp->w_redr_type = type;
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (!exiting && !redraw_not_allowed && wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	ఒకవేళ (type >= UPD_NOT_VALID)
	    wp->w_పంక్తులు_valid = 0;
	ఒకవేళ (must_redraw < type)	// must_redraw is the maximum of all windows
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    శూన్య
redraw_later_clear(శూన్య)
{
    redraw_all_later(UPD_CLEAR);
    reset_screen_attr();
}

/*
 * Mark all windows to be redrawn later.  Except popup windows.
 */
    శూన్య
redraw_all_later(పూర్ణాంకం type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	redraw_win_later(wp, type);
    // This may be needed when switching tabs.
    సమితి_must_redraw(type);
}

#ఒకవేళ 0  // not actually used yet, it probably should
/*
 * Mark all windows, including popup windows, to be redrawn.
 */
    శూన్య
redraw_all_windows_later(పూర్ణాంకం type)
{
    redraw_all_later(type);
# ifdef FEAT_PROP_POPUP
    popup_redraw_all();		// redraw all popup windows
# endif
}
#అయితే_ముగింపు

/*
 * Set "must_redraw" to "type" unless it already has a higher value
 * or it is currently not allowed.
 */
    శూన్య
సమితి_must_redraw(పూర్ణాంకం type)
{
    ఒకవేళ (!redraw_not_allowed &&
#నిర్వచితమైతే FEAT_EVAL
	    !inside_redraw_on_ప్రారంభం_cb &&
#అయితే_ముగింపు
	    must_redraw < type)
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    శూన్య
redraw_curbuf_later(పూర్ణాంకం type)
{
    redraw_buf_later(curbuf, type);
}

    శూన్య
redraw_buf_later(buf_T *buf, పూర్ణాంకం type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (wp->w_బఫరు == buf)
	    redraw_win_later(wp, type);
    }
#నిర్వచితమైతే FEAT_PROP_POPUP
    // popup windows are not in the list of windows
    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ (wp->w_బఫరు == buf)
	    redraw_win_later(wp, type);
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ (wp->w_బఫరు == buf)
	    redraw_win_later(wp, type);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // terminal in popup window is not in list of windows
    ఒకవేళ (curwin->w_బఫరు == buf)
	redraw_win_later(curwin, type);
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_SIGNS)
    శూన్య
redraw_buf_పంక్తి_later(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_బఫరు == buf && lnum >= wp->w_topline
						  && lnum < wp->w_botline)
	    redrawWinline(wp, lnum);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_పని_CHANNEL)
    శూన్య
redraw_buf_and_స్థితి_later(buf_T *buf, పూర్ణాంకం type)
{
    win_T	*wp;

    ఒకవేళ (wild_menu_showing != 0)
	// Don't redraw while the command line completion is displayed, it
	// would disappear.
	తిరిగి;
    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (wp->w_బఫరు == buf)
	{
	    redraw_win_later(wp, type);
	    wp->w_redr_స్థితి = true;
	}
    }
}
#అయితే_ముగింపు

/*
 * mark the ruler for redraw when the last window has no status line and the
 * ruler takes its place in the last screen line; showmode() draws it
 */
    స్థిర శూన్య
ruler_redraw_lastwin(శూన్య)
{
    ఒకవేళ (p_ru && lastwin->w_స్థితి_ఎత్తు == 0)
	redraw_cmdline = TRUE;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    శూన్య
స్థితి_redraw_all(శూన్య)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_స్థితి_ఎత్తు)
	{
	    wp->w_redr_స్థితి = true;
	    redraw_later(UPD_VALID);
	}
    ruler_redraw_lastwin();
}

/*
 * mark all status lines of the current buffer for redraw
 */
    శూన్య
స్థితి_redraw_curbuf(శూన్య)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_స్థితి_ఎత్తు != 0 && wp->w_బఫరు == curbuf)
	{
	    wp->w_redr_స్థితి = true;
	    redraw_later(UPD_VALID);
	}
    ఒకవేళ (lastwin->w_బఫరు == curbuf)
	ruler_redraw_lastwin();
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    శూన్య
redraw_statuslines(శూన్య)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_redr_స్థితి)
	{
	    bool ret = నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);
	    win_redr_స్థితి(wp, FALSE);
	    ఒకవేళ (ret)
		తీయి_highlight_overrides();
	}
    borrow_stl_vsep_hl();
    ఒకవేళ (redraw_tabline)
	draw_tabline();

#ఒకవేళ defined(FEAT_TABPANEL)
    ఒకవేళ (redraw_tabpanel)
	draw_tabpanel();
#అయితే_ముగింపు
}

/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    శూన్య
win_redraw_చివరి_స్థితి(చట్రం_T *frp)
{
    ఒకవేళ (frp->fr_layout == FR_ఆకు)
	frp->fr_win->w_redr_స్థితి = true;
    లేకపోతే ఒకవేళ (frp->fr_layout == FR_అడ్డువరుస)
    {
	FOR_ALL_చట్రాలు(frp, frp->fr_బిడ్డ)
	    win_redraw_చివరి_స్థితి(frp);
    }
    లేకపోతే // frp->fr_layout == FR_COL
    {
	frp = frp->fr_బిడ్డ;
	అయితే (frp->fr_తదుపరి != ఖాళీ)
	    frp = frp->fr_తదుపరి;
	win_redraw_చివరి_స్థితి(frp);
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
    శూన్య
redrawWinline(
    win_T	*wp,
    linenr_T	lnum)
{
    redraw_win_range_later(wp, lnum, lnum);
}

    శూన్య
redraw_win_range_later(
    win_T	*wp,
    linenr_T	మొదటి,
    linenr_T	చివరి)
{
    ఒకవేళ (చివరి >= wp->w_topline && మొదటి < wp->w_botline)
    {
	ఒకవేళ (wp->w_redraw_శిఖరం == 0 || wp->w_redraw_శిఖరం > మొదటి)
	    wp->w_redraw_శిఖరం = మొదటి;
	ఒకవేళ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < చివరి)
	    wp->w_redraw_bot = చివరి;
	redraw_win_later(wp, UPD_VALID);
    }
}

/*
 * Like redraw_win_range_later() but do not raise the global must_redraw.
 * Use this from inside an update_screen() pass (where the redraw will be
 * picked up this cycle), to avoid triggering an extra full redraw cycle.
 */
    శూన్య
redraw_win_range_now(
    win_T	*wp,
    linenr_T	మొదటి,
    linenr_T	చివరి)
{
    ఒకవేళ (చివరి >= wp->w_topline && మొదటి < wp->w_botline)
    {
	ఒకవేళ (wp->w_redraw_శిఖరం == 0 || wp->w_redraw_శిఖరం > మొదటి)
	    wp->w_redraw_శిఖరం = మొదటి;
	ఒకవేళ (wp->w_redraw_bot == 0 || wp->w_redraw_bot < చివరి)
	    wp->w_redraw_bot = చివరి;
	ఒకవేళ (wp->w_redr_type < UPD_VALID)
	    wp->w_redr_type = UPD_VALID;
    }
}

#నిర్వచితమైతే FEAT_EVAL
స్థిర bool redraw_cb_లోపల_పురోగతి = false;

    శూన్య
f_redraw_listener_కూడు(typval_T *argvars, typval_T *rettv)
{
    redraw_listener_T	*rln;
    dict_T		*dict;
    typval_T		tv;
    bool		got_one = false;
    స్థిర పూర్ణాంకం		id;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (redraw_cb_లోపల_పురోగతి)
    {
	emsg(_(e_cannot_కూడు_redraw_listener_లోపల_listener_callback));
	తిరిగి;
    }

    ఒకవేళ (check_for_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    rln = ALLOC_CLEAR_ONE(redraw_listener_T);

    ఒకవేళ (rln == ఖాళీ)
	తిరిగి;
    dict = argvars[0].vval.v_dict;

    /*
     * on_start: called on each screen redraw
     *
     * on_end: called at the end of a redraw cycle
     */
    ఒకవేళ (dict_get_tv(dict, "on_start", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	ఒకవేళ (cb.cb_name == ఖాళీ)
	{
	    clear_tv(&tv);
	    vim_free(rln);
	    తిరిగి;
	}
	copy_callback(&rln->rl_callbacks.on_ప్రారంభం, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    ఒకవేళ (dict_get_tv(dict, "on_end", &tv) == OK)
    {
	callback_T cb = get_callback(&tv);

	ఒకవేళ (cb.cb_name == ఖాళీ)
	{
	    clear_tv(&tv);
	    free_callback(&rln->rl_callbacks.on_ప్రారంభం);
	    vim_free(rln);
	    తిరిగి;
	}
	copy_callback(&rln->rl_callbacks.on_చివర, &cb);
	free_callback(&cb);
	clear_tv(&tv);
	got_one = true;
    }

    ఒకవేళ (!got_one)
    {
	emsg(_(e_no_redraw_listener_callbacks_defined));
	vim_free(rln);
	తిరిగి;
    }

    rln->rl_తదుపరి = redraw_listeners;
    redraw_listeners = rln;
    rln->rl_id = ++id; // Never zero

    rettv->v_type = VAR_సంఖ్య;
    rettv->vval.v_సంఖ్య = id;

    తిరిగి;
}

    స్థిర శూన్య
redraw_listener_free(redraw_listener_T *rln)
{
    free_callback(&rln->rl_callbacks.on_ప్రారంభం);
    free_callback(&rln->rl_callbacks.on_చివర);

    vim_free(rln);
}


    స్థిర శూన్య
redraw_listener_cleanup(శూన్య)
{
    కొరకు (redraw_listener_T *rln = redraw_listeners; rln != ఖాళీ;)
    {
	redraw_listener_T *తదుపరి = rln->rl_తదుపరి;
	ఒకవేళ (rln->rl_id == 0)
	{
	    ఒకవేళ (redraw_listeners == rln)
		redraw_listeners = rln->rl_తదుపరి;
	    redraw_listener_free(rln);
	}
	rln = తదుపరి;
    }
}

/*
 * Return the redraw listener struct with the specified id. Returns NULL if not
 * found.
 */
    స్థిర redraw_listener_T *
get_redraw_listener(పూర్ణాంకం id)
{
    కొరకు (redraw_listener_T *rln = redraw_listeners; rln != ఖాళీ; rln = rln->rl_తదుపరి)
	ఒకవేళ (rln->rl_id == id)
	    తిరిగి rln;
    తిరిగి ఖాళీ;
}

    శూన్య
f_redraw_listener_remove(typval_T *argvars, typval_T *rettv వాడని)
{
    పూర్ణాంకం			id;
    redraw_listener_T	*rln;

    ఒకవేళ (check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = argvars[0].vval.v_సంఖ్య;
    rln = get_redraw_listener(id);

    rettv->v_type = VAR_సంఖ్య;
    ఒకవేళ (rln == ఖాళీ)
    {
	rettv->vval.v_సంఖ్య = 0;
	తిరిగి;
    }

    // We set the id to zero instead of freeing it here, since we still need
    // rl_next from it.
    rln->rl_id = 0;
    rettv->vval.v_సంఖ్య = 1;
}

/*
 * Invoke the on_start callbacks.
 */
    స్థిర శూన్య
invoke_redraw_listener_ప్రారంభం_or_చివర(bool ప్రారంభం)
{
    typval_T argv[1];
    typval_T rettv;

    argv[0].v_type = VAR_UNKNOWN;

    ఒకవేళ (ప్రారంభం)
	inside_redraw_on_ప్రారంభం_cb = true;

    redraw_cb_లోపల_పురోగతి = true;
    కొరకు (redraw_listener_T *rln = redraw_listeners; rln != ఖాళీ; rln = rln->rl_తదుపరి)
    {
	ఒకవేళ (rln->rl_id == 0)
	    // Listener has been removed, skip
	    కొనసాగు;
	ఒకవేళ (ప్రారంభం && rln->rl_callbacks.on_ప్రారంభం.cb_name != ఖాళీ)
	{
	    call_callback(&rln->rl_callbacks.on_ప్రారంభం, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
	లేకపోతే ఒకవేళ (rln->rl_callbacks.on_చివర.cb_name != ఖాళీ)
	{
	    call_callback(&rln->rl_callbacks.on_చివర, -1, &rettv, 0, argv);
	    clear_tv(&rettv);
	}
    }
    redraw_cb_లోపల_పురోగతి = false;

    ఒకవేళ (ప్రారంభం)
	inside_redraw_on_ప్రారంభం_cb = false;
}
#అయితే_ముగింపు // FEAT_EVAL
