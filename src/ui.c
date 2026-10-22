/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * ui.c: functions that handle the user interface.
 * 1. Keyboard input stuff, and a bit of windowing stuff.  These are called
 *    before the machine specific stuff (mch_*) so that we can call the GUI
 *    stuff instead if the GUI is running.
 * 2. Input buffer stuff.
 */

#అన్తర్భావ "vim.h"

    శూన్య
ui_write(char_u *s, పూర్ణాంకం len, పూర్ణాంకం console వాడని)
{
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	ఒకవేళ (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	తిరిగి;
    }
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    ఒకవేళ (!(silent_బహుళకం && p_verbose == 0))
    {
# ఒకవేళ !defined(MSWIN)
	char_u	*tofree = ఖాళీ;

	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    ఒకవేళ (tofree != ఖాళీ)
		s = tofree;
	}
# endif

	mch_write(s, len);
# ఒకవేళ defined(HAVE_FSYNC)
	ఒకవేళ (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# ఒకవేళ !defined(MSWIN)
	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#అయితే_ముగింపు
}

#ఒకవేళ defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
స్థిర char_u *ta_str = ఖాళీ;
స్థిర పూర్ణాంకం ta_off;	// offset for next char to use when ta_str != NULL
స్థిర పూర్ణాంకం ta_len;	// length of ta_str when it's not NULL

    శూన్య
ui_inchar_undo(char_u *s, పూర్ణాంకం len)
{
    char_u  *new;
    పూర్ణాంకం	    newlen;

    newlen = len;
    ఒకవేళ (ta_str != ఖాళీ)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    ఒకవేళ (new == ఖాళీ)
	తిరిగి;

    ఒకవేళ (ta_str != ఖాళీ)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    లేకపోతే
	mch_memmove(new, s, (size_t)len);
    ta_str = new;
    ta_len = newlen;
    ta_off = 0;
}
#అయితే_ముగింపు

/*
 * ui_inchar(): low level input function.
 * Get characters from the keyboard.
 * Return the number of characters that are available.
 * If "wtime" == 0 do not wait for characters.
 * If "wtime" == -1 wait forever for characters.
 * If "wtime" > 0 wait "wtime" milliseconds for a character.
 *
 * "tb_change_cnt" is the value of typebuf.tb_change_cnt if "buf" points into
 * it.  When typebuf.tb_change_cnt changes (e.g., when a message is received
 * from a remote client) "buf" can no longer be used.  "tb_change_cnt" is NULL
 * otherwise.
 */
    పూర్ణాంకం
ui_inchar(
    char_u	*buf,
    పూర్ణాంకం		maxlen,
    దీర్ఘ	wtime,	    // don't use "time", MIPS cannot handle it
    పూర్ణాంకం		tb_change_cnt)
{
    పూర్ణాంకం		retval = 0;

#ఒకవేళ defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    ఒకవేళ (ta_str != ఖాళీ)
    {
	ఒకవేళ (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    తిరిగి ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	తిరిగి maxlen;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#అయితే_ముగింపు

#నిర్వచితమైతే NO_CONSOLE_ఆదానం
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    ఒకవేళ (no_console_ఆదానం())
    {
	స్థిర పూర్ణాంకం లెక్క = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	ఒకవేళ (retval > 0 || typebuf_మారింది(tb_change_cnt) || wtime >= 0)
	    వెళ్ళు theend;
# endif
	ఒకవేళ (wtime == -1 && ++లెక్క == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	వెళ్ళు theend;
    }
#అయితే_ముగింపు

    // If we are going to wait for some time or block...
    ఒకవేళ (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(శూన్య)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	ఒకవేళ ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
	    ctrl_c_interrupts = FALSE;
    }

    /*
     * Here we call gui_inchar() or mch_inchar(), the GUI or machine-dependent
     * input function.  The functionality they implement is like this:
     *
     * while (not timed out)
     * {
     *    handle-resize;
     *    parse-queued-messages;
     *    if (waited for 'updatetime')
     *       trigger-cursorhold;
     *    ui_wait_for_chars_or_timer()
     *    if (character available)
     *      break;
     * }
     *
     * ui_wait_for_chars_or_timer() does:
     *
     * while (not timed out)
     * {
     *     if (any-timer-triggered)
     *	      invoke-timer-callback;
     *     wait-for-character();
     *     if (character available)
     *	      break;
     * }
     *
     * wait-for-character() does:
     * while (not timed out)
     * {
     *     Wait for event;
     *     if (something on channel)
     *	      read/write channel;
     *	   else if (resized)
     *	      handle_resize();
     *	   else if (system event)
     *	      deal-with-system-event;
     *	   else if (character available)
     *	      break;
     * }
     *
     */

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే NO_CONSOLE
# ifdef FEAT_GUI
    లేకపోతే
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#అయితే_ముగింపు

    ఒకవేళ (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(శూన్య)vim_handle_signal(SIGNAL_ఖండం);

    ctrl_c_interrupts = TRUE;

#నిర్వచితమైతే NO_CONSOLE_ఆదానం
theend:
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#అయితే_ముగింపు
    తిరిగి retval;
}

#ఒకవేళ defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
/*
 * Common code for mch_inchar() and gui_inchar(): Wait for a while or
 * indefinitely until characters are available, dealing with timers and
 * messages on channels.
 *
 * "buf" may be NULL if the available characters are not to be returned, only
 * check if they are available.
 *
 * Return the number of characters that are available.
 * If "wtime" == 0 do not wait for characters.
 * If "wtime" == n wait a short time for characters.
 * If "wtime" == -1 wait forever for characters.
 */
    పూర్ణాంకం
inchar_loop(
    char_u	*buf,
    పూర్ణాంకం		maxlen,
    దీర్ఘ	wtime,	    // don't use "time", MIPS cannot handle it
    పూర్ణాంకం		tb_change_cnt,
    పూర్ణాంకం		(*wait_func)(దీర్ఘ wtime, పూర్ణాంకం *interrupted, పూర్ణాంకం ignore_ఆదానం),
    పూర్ణాంకం		(*resize_func)(పూర్ణాంకం check_only))
{
    పూర్ణాంకం		len;
    పూర్ణాంకం		interrupted = FALSE;
    పూర్ణాంకం		did_call_wait_func = FALSE;
    పూర్ణాంకం		did_ప్రారంభం_blocking = FALSE;
    దీర్ఘ	wait_సమయం;
    దీర్ఘ	elapsed_సమయం = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	ప్రారంభం_tv;

    ELAPSED_INIT(ప్రారంభం_tv);
# endif

    // repeat until we got a character or waited long enough
    కొరకు (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	ఒకవేళ (resize_func != ఖాళీ)
	    resize_func(FALSE);

# ifdef సందేశం_వరుస
	// Only process messages when waiting.
	ఒకవేళ (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    ఒకవేళ (typebuf_మారింది(tb_change_cnt))
		తిరిగి 0;
	}
# endif
	// When an autocomplete is pending, wake at the sooner of
	// 'autocompletedelay' and 'updatetime' so the delay does not postpone
	// CursorHold.  Once CursorHold has fired, only the delay is left.
	// Gate the injection like trigger_cursorhold() so the deferred key
	// cannot fire while outside Insert mode, but do still fire the key
	// when recording a register (gotchars_add_byte() will ignore it).
	bool delay_pending = ins_compl_autocomplete_pending() && p_acl > 0
		&& typebuf.tb_len == 0
		&& !ins_compl_చురుకైన()
		&& (get_real_state() & బహుళకం_చొప్పించు) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	దీర్ఘ acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	ఒకవేళ (wtime < 0 && did_ప్రారంభం_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_సమయం = -1;
	లేకపోతే
	{
# ifdef ELAPSED_FUNC
	    elapsed_సమయం = ELAPSED_FUNC(ప్రారంభం_tv);
# endif
	    ఒకవేళ (wtime >= 0)
		wait_సమయం = wtime - elapsed_సమయం;
	    లేకపోతే ఒకవేళ (delay_pending)
	    {
		దీర్ఘ delay_ఎడమ = p_acl - acl_elapsed;
		దీర్ఘ ut_ఎడమ = p_ut - elapsed_సమయం;

		ఒకవేళ (did_ప్రారంభం_blocking)
		    wait_సమయం = delay_ఎడమ;
		లేకపోతే
		    wait_సమయం = MIN(delay_ఎడమ, ut_ఎడమ);
	    }
	    లేకపోతే
		// going to block after p_ut
		wait_సమయం = p_ut - elapsed_సమయం;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    ఒకవేళ (wait_సమయం <= 0 && did_call_wait_func)
	    {
		ఒకవేళ (wtime >= 0)
		    // no character available within "wtime"
		    తిరిగి 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		ఒకవేళ (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_మారింది(tb_change_cnt))
		{
		    ఒకవేళ (buf == ఖాళీ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (పూర్ణాంకం)KE_COMPLETE_DELAY;
			కూడు_గమ్యం_ఆదానం_buf(ibuf, 3);
		    }
		    లేకపోతే
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (పూర్ణాంకం)KE_COMPLETE_DELAY;
		    }
		    తిరిగి 3;
		}

		// No character available within 'updatetime'.
		did_ప్రారంభం_blocking = TRUE;
		ఒకవేళ (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_మారింది(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    ఒకవేళ (buf == ఖాళీ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (పూర్ణాంకం)KE_CURSORHOLD;
			కూడు_గమ్యం_ఆదానం_buf(ibuf, 3);
		    }
		    లేకపోతే
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (పూర్ణాంకం)KE_CURSORHOLD;
		    }
		    తిరిగి 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		కొనసాగు;
	    }
	}

# ifdef FEAT_పని_CHANNEL
	ఒకవేళ (wait_సమయం < 0 || wait_సమయం > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    ఒకవేళ (has_pending_పని())
		wait_సమయం = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    ఒకవేళ (channel_any_readahead())
		wait_సమయం = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	ఒకవేళ (p_beval && wait_సమయం > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_సమయం = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	ఒకవేళ (wait_func(wait_సమయం, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    ఒకవేళ (typebuf_మారింది(tb_change_cnt))
		తిరిగి 0;

	    // We might have something to return now.
	    ఒకవేళ (buf == ఖాళీ)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		తిరిగి ఆదానం_అందుబాటు();

	    len = read_from_ఆదానం_buf(buf, (దీర్ఘ)maxlen);
	    ఒకవేళ (len > 0)
		తిరిగి len;
	    కొనసాగు;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_సమయం += wait_సమయం;
# endif

	ఒకవేళ ((resize_func != ఖాళీ && resize_func(TRUE))
# ఒకవేళ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
		||
		(clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11 &&
		 server_నిరీక్షణ())
# endif
# ifdef సందేశం_వరుస
		|| interrupted
# endif
		|| wait_సమయం > 0
		|| (wtime < 0 && !did_ప్రారంభం_blocking))
	    // no character available, but something to be done, keep going
	    కొనసాగు;

	// no character available or interrupted, return zero
	ఆపు;
    }
    తిరిగి 0;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    పూర్ణాంకం
ui_wait_for_chars_or_timer(
    దీర్ఘ    wtime,
    పూర్ణాంకం	    (*wait_func)(దీర్ఘ wtime, పూర్ణాంకం *interrupted, పూర్ణాంకం ignore_ఆదానం),
    పూర్ణాంకం	    *interrupted,
    పూర్ణాంకం	    ignore_ఆదానం)
{
    పూర్ణాంకం	    due_సమయం;
    దీర్ఘ    మిగిలిన = wtime;
    పూర్ణాంకం	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_పని_CHANNEL
    పూర్ణాంకం	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    ఒకవేళ (wtime >= 0 && wtime < 10L)
	తిరిగి wait_func(wtime, ఖాళీ, ignore_ఆదానం);

    అయితే (wtime < 0 || మిగిలిన > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_సమయం = check_due_timer();
	ఒకవేళ (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    తిరిగి FAIL;
	}
	ఒకవేళ (due_సమయం <= 0 || (wtime > 0 && due_సమయం > మిగిలిన))
	    due_సమయం = మిగిలిన;
# ఒకవేళ defined(FEAT_పని_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	ఒకవేళ ((due_సమయం < 0 || due_సమయం > 10L) && (
#  ఒకవేళ defined(FEAT_పని_CHANNEL)
		(
#   ఒకవేళ defined(FEAT_GUI)
		!gui.లోపల_use &&
#   endif
		(has_pending_పని() || channel_any_readahead()))
#   ఒకవేళ defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  ఒకవేళ defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
		    has_any_sound_callback()
#  endif
		    ))
	{
	    // There is a pending job or channel, should return soon in order
	    // to handle them ASAP.  Do check for input briefly.
	    due_సమయం = 10L;
#  ifdef FEAT_పని_CHANNEL
	    brief_wait = TRUE;
#  endif
	}
# endif
	ఒకవేళ (wait_func(due_సమయం, interrupted, ignore_ఆదానం))
	    తిరిగి OK;
	ఒకవేళ ((interrupted != ఖాళీ && *interrupted)
# ifdef FEAT_పని_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    తిరిగి FAIL;
	ఒకవేళ (wtime > 0)
	    మిగిలిన -= due_సమయం;
    }
    తిరిగి FAIL;
}
#అయితే_ముగింపు

/*
 * Return non-zero if a character is available.
 */
    పూర్ణాంకం
ui_char_avail(శూన్య)
{
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	gui_mch_update();
	తిరిగి ఆదానం_అందుబాటు();
    }
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే NO_CONSOLE
# ifdef NO_CONSOLE_ఆదానం
    ఒకవేళ (no_console_ఆదానం())
	తిరిగి 0;
# endif
    తిరిగి mch_char_avail();
#లేకపోతే
    తిరిగి 0;
#అయితే_ముగింపు
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    శూన్య
ui_delay(దీర్ఘ msec_arg, పూర్ణాంకం ignoreinput)
{
    దీర్ఘ msec = msec_arg;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(ఖాళీ, "ui_delay(%ld)", msec);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    లేకపోతే
#అయితే_ముగింపు
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    శూన్య
ui_suspend(శూన్య)
{
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	gui_mch_iconify();
	తిరిగి;
    }
#అయితే_ముగింపు
    mch_suspend();
}

#ఒకవేళ !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    శూన్య
suspend_shell(శూన్య)
{
    ఒకవేళ (*p_sh == NUL)
	emsg(_(e_shell_option_is_ఖాళీలు));
    లేకపోతే
    {
	సందేశం_puts(_("new shell started\n"));
	do_shell(ఖాళీ, 0);
    }
}
#అయితే_ముగింపు

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    పూర్ణాంకం
ui_get_shellsize(శూన్య)
{
    పూర్ణాంకం	    retval;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	retval = gui_get_shellsize();
    లేకపోతే
#అయితే_ముగింపు
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    ఒకవేళ (retval == OK)
    {
	సమితి_సంఖ్య_default("lines", అడ్డువరుసలు);
	సమితి_సంఖ్య_default("columns", Columns);
    }
    తిరిగి retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    శూన్య
ui_సమితి_shellsize(
    పూర్ణాంకం		mustset వాడని)	// set by the user
{
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	gui_సమితి_shellsize(mustset, TRUE, RESIZE_BOTH);
    లేకపోతే
#అయితే_ముగింపు
	mch_సమితి_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    శూన్య
ui_new_shellsize(శూన్య)
{
    ఒకవేళ (నిండిన_screen && !exiting)
    {
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	    gui_new_shellsize();
	లేకపోతే
#అయితే_ముగింపు
	    mch_new_shellsize();
    }
}

#ఒకవేళ (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    పూర్ణాంకం
ui_get_winpos(పూర్ణాంకం *x, పూర్ణాంకం *y, varnumber_T timeout వాడని)
{
# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	తిరిగి gui_mch_get_winpos(x, y);
# endif
# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    తిరిగి mch_get_winpos(x, y);
# లేకపోతే
#  ఒకవేళ defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    తిరిగి పదం_get_winpos(x, y, timeout);
#  లేకపోతే
    తిరిగి FAIL;
#  endif
# endif
}
#అయితే_ముగింపు

    శూన్య
ui_breakcheck(శూన్య)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    శూన్య
ui_breakcheck_force(పూర్ణాంకం force)
{
    స్థిర పూర్ణాంకం	recursive = FALSE;
    పూర్ణాంకం		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    ఒకవేళ (recursive)
	తిరిగి;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	gui_mch_update();
    లేకపోతే
#అయితే_ముగింపు
	mch_breakcheck(force);

    ఒకవేళ (save_updating_screen)
	updating_screen = TRUE;
    లేకపోతే
	after_updating_screen(FALSE);

    recursive = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// Functions that handle the input buffer.
// This is used for any GUI version, and the unix terminal version.
//
// For Unix, the input characters are buffered to be able to check for a
// CTRL-C.  This should be done with signals, but I don't know how to do that
// in a portable way for a tty in RAW mode.
//
// For the client-server code in the console the received keys are put in the
// input buffer.

#ఒకవేళ defined(USE_ఆదానం_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# ఒకవేళ defined(FEAT_పని_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# లేకపోతే
#  define INBUFLEN 250
# endif

స్థిర char_u	inbuf[INBUFLEN + MAX_కీలకం_సంకేతం_LEN];
స్థిర పూర్ణాంకం	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    పూర్ణాంకం
vim_is_ఆదానం_buf_నిండిన(శూన్య)
{
    తిరిగి (inbufcount >= INBUFLEN);
}

    పూర్ణాంకం
vim_is_ఆదానం_buf_ఖాళీలు(శూన్య)
{
    తిరిగి (inbufcount == 0);
}

# ఒకవేళ defined(FEAT_OLE)
    పూర్ణాంకం
vim_free_లోపల_ఆదానం_buf(శూన్య)
{
    తిరిగి (INBUFLEN - inbufcount);
}
# endif

# ఒకవేళ defined(FEAT_GUI_GTK)
    పూర్ణాంకం
vim_వాడిన_లోపల_ఆదానం_buf(శూన్య)
{
    తిరిగి inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_ఆదానం_buf(శూన్య)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    ఒకవేళ (gap != ఖాళీ)
    {
	// Add one to avoid a zero size.
	gap->ga_దత్తాంశం = alloc(inbufcount + 1);
	ఒకవేళ (gap->ga_దత్తాంశం != ఖాళీ)
	    mch_memmove(gap->ga_దత్తాంశం, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_ఆదానం_buf();
    తిరిగి (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    శూన్య
సమితి_ఆదానం_buf(char_u *p, పూర్ణాంకం overwrite)
{
    garray_T	*gap = (garray_T *)p;

    ఒకవేళ (gap == ఖాళీ)
	తిరిగి;

    ఒకవేళ (gap->ga_దత్తాంశం != ఖాళీ)
    {
	ఒకవేళ (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_దత్తాంశం, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	లేకపోతే
	{
	    mch_memmove(inbuf + gap->ga_len, inbuf, inbufcount);
	    mch_memmove(inbuf, gap->ga_దత్తాంశం, gap->ga_len);
	    inbufcount += gap->ga_len;
	}
	vim_free(gap->ga_దత్తాంశం);
    }
    vim_free(gap);
}

/*
 * Add the given bytes to the input buffer
 * Special keys start with CSI.  A real CSI must have been translated to
 * CSI KS_EXTRA KE_CSI.  K_SPECIAL doesn't require translation.
 */
    శూన్య
కూడు_గమ్యం_ఆదానం_buf(char_u *s, పూర్ణాంకం len)
{
    ఒకవేళ (inbufcount + len > INBUFLEN + MAX_కీలకం_సంకేతం_LEN)
	తిరిగి;	    // Shouldn't ever happen!

    అయితే (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    శూన్య
కూడు_గమ్యం_ఆదానం_buf_csi(char_u *str, పూర్ణాంకం len)
{
    పూర్ణాంకం		i;
    char_u	buf[2];

    కొరకు (i = 0; i < len; ++i)
    {
	కూడు_గమ్యం_ఆదానం_buf(str + i, 1);
	ఒకవేళ (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (పూర్ణాంకం)KE_CSI;
	    కూడు_గమ్యం_ఆదానం_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    శూన్య
trash_ఆదానం_buf(శూన్య)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    పూర్ణాంకం
read_from_ఆదానం_buf(char_u *buf, దీర్ఘ maxlen)
{
    ఒకవేళ (inbufcount == 0)	// if the buffer is empty, fill it
	fill_ఆదానం_buf(TRUE);
    ఒకవేళ (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    ఒకవేళ (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    తిరిగి (పూర్ణాంకం)maxlen;
}

    శూన్య
fill_ఆదానం_buf(పూర్ణాంకం exit_on_error వాడని)
{
# ఒకవేళ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    పూర్ణాంకం		len;
    పూర్ణాంకం		try;
    స్థిర పూర్ణాంకం	did_read_something = FALSE;
    స్థిర char_u *rest = ఖాళీ;	    // unconverted rest of previous read
    స్థిర పూర్ణాంకం	restlen = 0;
    పూర్ణాంకం		unconverted;
# endif

# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use
#  ifdef NO_CONSOLE_ఆదానం
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_ఆదానం()
#  endif
       )
    {
	gui_mch_update();
	తిరిగి;
    }
# endif
# ఒకవేళ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ఒకవేళ (vim_is_ఆదానం_buf_నిండిన())
	తిరిగి;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    ఒకవేళ (rest != ఖాళీ)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	ఒకవేళ (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	లేకపోతే
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	ఒకవేళ (unconverted == restlen)
	    VIM_CLEAR(rest);
	లేకపోతే
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    లేకపోతే
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    కొరకు (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / ఆదానం_conv.vc_గుణకం);
#  ifdef VMS
	len = vms_read((అక్షరం *)inbuf + inbufcount, readlen);
#  లేకపోతే
	len = పఠ(read_cmd_fd, (అక్షరం *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	ఒకవేళ (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(ఖాళీ, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	ఒకవేళ (len > 0 || got_int)
	    ఆపు;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	ఒకవేళ (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    పూర్ణాంకం m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  లేకపోతే
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	ఒకవేళ (!exit_on_error)
	    తిరిగి;
    }
    ఒకవేళ (len <= 0 && !got_int)
	read_error_exit();
    ఒకవేళ (len > 0)
	did_read_something = TRUE;
    ఒకవేళ (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    లేకపోతే
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_ఆదానం_సురక్షిత(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == ఖాళీ ? &rest : ఖాళీ, &restlen);
	}
	అయితే (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    ఒకవేళ (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !కీలకం_protocol_enabled())
			|| (len >= 10 && STRNCMP(inbuf + inbufcount,
						   "\033[27;5;99~", 10) == 0)
			|| (len >= 10 && STRNCMP(inbuf + inbufcount,
						   "\033[27;5;67~", 10) == 0)
			|| (len >= 7 && STRNCMP(inbuf + inbufcount,
						       "\033[99;5u", 7) == 0)
			|| (len >= 7 && STRNCMP(inbuf + inbufcount,
						       "\033[67;5u", 7) == 0)))
	    {
		// remove everything typed before the CTRL-C
		mch_memmove(inbuf, inbuf + inbufcount, (size_t)(len));
		inbufcount = 0;
		got_int = TRUE;
	    }
	    --len;
	    ++inbufcount;
	}
    }
# endif // UNIX || VMS || MACOS_X
}
#అయితే_ముగింపు // USE_INPUT_BUF

/*
 * Exit because of an input read error.
 */
    శూన్య
read_error_exit(శూన్య)
{
    ఒకవేళ (silent_బహుళకం)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#ఒకవేళ defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    శూన్య
ui_cursor_shape_forced(పూర్ణాంకం forced)
{
# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	gui_update_cursor_later();
    లేకపోతే
# endif
	పదం_cursor_బహుళకం(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_పంక్తి(FALSE);
# endif
}

    శూన్య
ui_cursor_shape(శూన్య)
{
    ui_cursor_shape_forced(FALSE);
}
#అయితే_ముగింపు

/*
 * Check bounds for column number
 */
    పూర్ణాంకం
check_నిలువు(పూర్ణాంకం నిలువు)
{
    ఒకవేళ (నిలువు < 0)
	తిరిగి 0;
    ఒకవేళ (నిలువు >= screen_Columns)
	తిరిగి screen_Columns - 1;
    తిరిగి నిలువు;
}

/*
 * Check bounds for row number
 */
    పూర్ణాంకం
check_అడ్డువరుస(పూర్ణాంకం అడ్డువరుస)
{
    ఒకవేళ (అడ్డువరుస < 0)
	తిరిగి 0;
    ఒకవేళ (అడ్డువరుస >= screen_అడ్డువరుసలు)
	తిరిగి screen_అడ్డువరుసలు - 1;
    తిరిగి అడ్డువరుస;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    దీర్ఘ
scroll_పంక్తి_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	నిలువు = 0;

    ఒకవేళ (*p != NUL)
	కొరకు (;;)
	{
	    పూర్ణాంకం	    w = chartabsize(p, నిలువు);
	    MB_PTR_ADV(p);
	    ఒకవేళ (*p == NUL)		// don't count the last character
		ఆపు;
	    నిలువు += w;
	}
    తిరిగి నిలువు;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_కనుగొను_longest_lnum(శూన్య)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    ఒకవేళ (
#నిర్వచితమైతే FEAT_GUI
	    (!gui.లోపల_use || vim_strchr(p_go, GO_HORSCROLL) == ఖాళీ) &&
#అయితే_ముగింపు
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_పంక్తి_లెక్క + 1)
    {
	linenr_T    lnum;
	దీర్ఘ	    n;
	దీర్ఘ	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	కొరకు (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_పంక్తి_len(lnum);
	    ఒకవేళ (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    లేకపోతే ఒకవేళ (n == max && నిరపేక్ష((పూర్ణాంకం)(lnum - curwin->w_cursor.lnum))
				     < నిరపేక్ష((పూర్ణాంకం)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    లేకపోతే
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    తిరిగి ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    శూన్య
ui_focus_change(
    పూర్ణాంకం		లోపల_focus)	// TRUE if focus gained.
{
    స్థిర time_t	చివరి_సమయం = (time_t)0;
    పూర్ణాంకం			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    ఒకవేళ (లోపల_focus && చివరి_సమయం + 2 < సమయం(ఖాళీ))
    {
	need_redraw = check_timestamps(
#నిర్వచితమైతే FEAT_GUI
		gui.లోపల_use
#లేకపోతే
		FALSE
#అయితే_ముగింపు
		);
	చివరి_సమయం = సమయం(ఖాళీ);
    }

#నిర్వచితమైతే FEAT_TERMINAL
    పదం_focus_change(లోపల_focus);
#అయితే_ముగింపు

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(లోపల_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, ఖాళీ, ఖాళీ, FALSE, curbuf);

    ఒకవేళ (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    ఒకవేళ (need_maketitle)
	maketitle();
}

#ఒకవేళ defined(HAVE_ఆదానం_పద్ధతి)
/*
 * Save current Input Method status to specified place.
 */
    శూన్య
im_save_స్థితి(దీర్ఘ *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    ఒకవేళ (!p_imdisable && కీలకం_Typed && !కీలకం_Stuffed
# ifdef FEAT_XIM
	    && xic != ఖాళీ
# endif
# ifdef FEAT_GUI
	    && (!gui.లోపల_use || gui.లోపల_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	ఒకవేళ (vgetc_im_చురుకైన)
	    *psave = B_IMODE_IM;
	లేకపోతే ఒకవేళ (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#అయితే_ముగింపు
