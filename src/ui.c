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

#അന്തര്ഭാവ "vim.h"

    ശൂന്യം
ui_write(char_u *s, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ console UNUSED)
{
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	എങ്കിൽ (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	തിരികെ;
    }
#endif
#ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    എങ്കിൽ (!(silent_mode && p_verbose == 0))
    {
# എങ്കിൽ !defined(MSWIN)
	char_u	*tofree = ശൂന്യ;

	എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    എങ്കിൽ (tofree != ശൂന്യ)
		s = tofree;
	}
# endif

	mch_write(s, len);
# എങ്കിൽ defined(HAVE_FSYNC)
	എങ്കിൽ (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# എങ്കിൽ !defined(MSWIN)
	എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#endif
}

#എങ്കിൽ defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
സ്ഥിതം char_u *ta_str = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ ta_off;	// offset for next char to use when ta_str != NULL
സ്ഥിതം പൂർണ്ണസംഖ്യ ta_len;	// length of ta_str when it's not NULL

    ശൂന്യം
ui_inchar_undo(char_u *s, പൂർണ്ണസംഖ്യ len)
{
    char_u  *new;
    പൂർണ്ണസംഖ്യ	    newlen;

    newlen = len;
    എങ്കിൽ (ta_str != ശൂന്യ)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    എങ്കിൽ (new == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (ta_str != ശൂന്യ)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    അല്ലെങ്കിൽ
	mch_memmove(new, s, (size_t)len);
    ta_str = new;
    ta_len = newlen;
    ta_off = 0;
}
#endif

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
    പൂർണ്ണസംഖ്യ
ui_inchar(
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    പൂർണ്ണസംഖ്യ		tb_change_cnt)
{
    പൂർണ്ണസംഖ്യ		retval = 0;

#എങ്കിൽ defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    എങ്കിൽ (ta_str != ശൂന്യ)
    {
	എങ്കിൽ (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    തിരികെ ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	തിരികെ maxlen;
    }
#endif

#ifdef FEAT_PROFILE
    എങ്കിൽ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#endif

#ifdef NO_CONSOLE_നിവേശം
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    എങ്കിൽ (no_console_നിവേശം())
    {
	സ്ഥിതം പൂർണ്ണസംഖ്യ എണ്ണം = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	എങ്കിൽ (retval > 0 || typebuf_changed(tb_change_cnt) || wtime >= 0)
	    പോകുക theend;
# endif
	എങ്കിൽ (wtime == -1 && ++എണ്ണം == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	പോകുക theend;
    }
#endif

    // If we are going to wait for some time or block...
    എങ്കിൽ (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(ശൂന്യം)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	എങ്കിൽ ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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

#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif
#ifndef NO_CONSOLE
# ifdef FEAT_GUI
    അല്ലെങ്കിൽ
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif

    എങ്കിൽ (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(ശൂന്യം)vim_handle_signal(SIGNAL_BLOCK);

    ctrl_c_interrupts = TRUE;

#ifdef NO_CONSOLE_നിവേശം
theend:
#endif
#ifdef FEAT_PROFILE
    എങ്കിൽ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#endif
    തിരികെ retval;
}

#എങ്കിൽ defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    പൂർണ്ണസംഖ്യ
inchar_loop(
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    പൂർണ്ണസംഖ്യ		tb_change_cnt,
    പൂർണ്ണസംഖ്യ		(*wait_func)(long wtime, പൂർണ്ണസംഖ്യ *interrupted, പൂർണ്ണസംഖ്യ ignore_നിവേശം),
    പൂർണ്ണസംഖ്യ		(*resize_func)(പൂർണ്ണസംഖ്യ check_only))
{
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		interrupted = FALSE;
    പൂർണ്ണസംഖ്യ		did_call_wait_func = FALSE;
    പൂർണ്ണസംഖ്യ		did_start_blocking = FALSE;
    long	wait_time;
    long	elapsed_time = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
# endif

    // repeat until we got a character or waited long enough
    വേണ്ടി (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	എങ്കിൽ (resize_func != ശൂന്യ)
	    resize_func(FALSE);

# ifdef MESSAGE_QUEUE
	// Only process messages when waiting.
	എങ്കിൽ (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    എങ്കിൽ (typebuf_changed(tb_change_cnt))
		തിരികെ 0;
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
		&& !ins_compl_active()
		&& (get_real_state() & MODE_തിരുകുക) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	long acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	എങ്കിൽ (wtime < 0 && did_start_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_time = -1;
	അല്ലെങ്കിൽ
	{
# ifdef ELAPSED_FUNC
	    elapsed_time = ELAPSED_FUNC(start_tv);
# endif
	    എങ്കിൽ (wtime >= 0)
		wait_time = wtime - elapsed_time;
	    അല്ലെങ്കിൽ എങ്കിൽ (delay_pending)
	    {
		long delay_ഇടത് = p_acl - acl_elapsed;
		long ut_ഇടത് = p_ut - elapsed_time;

		എങ്കിൽ (did_start_blocking)
		    wait_time = delay_ഇടത്;
		അല്ലെങ്കിൽ
		    wait_time = MIN(delay_ഇടത്, ut_ഇടത്);
	    }
	    അല്ലെങ്കിൽ
		// going to block after p_ut
		wait_time = p_ut - elapsed_time;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    എങ്കിൽ (wait_time <= 0 && did_call_wait_func)
	    {
		എങ്കിൽ (wtime >= 0)
		    // no character available within "wtime"
		    തിരികെ 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		എങ്കിൽ (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    എങ്കിൽ (buf == ശൂന്യ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (പൂർണ്ണസംഖ്യ)KE_COMPLETE_DELAY;
			add_to_നിവേശം_buf(ibuf, 3);
		    }
		    അല്ലെങ്കിൽ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (പൂർണ്ണസംഖ്യ)KE_COMPLETE_DELAY;
		    }
		    തിരികെ 3;
		}

		// No character available within 'updatetime'.
		did_start_blocking = TRUE;
		എങ്കിൽ (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    എങ്കിൽ (buf == ശൂന്യ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (പൂർണ്ണസംഖ്യ)KE_CURSORHOLD;
			add_to_നിവേശം_buf(ibuf, 3);
		    }
		    അല്ലെങ്കിൽ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (പൂർണ്ണസംഖ്യ)KE_CURSORHOLD;
		    }
		    തിരികെ 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		തുടരുക;
	    }
	}

# ifdef FEAT_JOB_CHANNEL
	എങ്കിൽ (wait_time < 0 || wait_time > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    എങ്കിൽ (has_pending_job())
		wait_time = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    എങ്കിൽ (channel_any_readahead())
		wait_time = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	എങ്കിൽ (p_beval && wait_time > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_time = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	എങ്കിൽ (wait_func(wait_time, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    എങ്കിൽ (typebuf_changed(tb_change_cnt))
		തിരികെ 0;

	    // We might have something to return now.
	    എങ്കിൽ (buf == ശൂന്യ)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		തിരികെ നിവേശം_available();

	    len = read_from_നിവേശം_buf(buf, (long)maxlen);
	    എങ്കിൽ (len > 0)
		തിരികെ len;
	    തുടരുക;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_time += wait_time;
# endif

	എങ്കിൽ ((resize_func != ശൂന്യ && resize_func(TRUE))
# എങ്കിൽ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
		||
		(clientserver_method == CLIENTSERVER_METHOD_X11 &&
		 server_waiting())
# endif
# ifdef MESSAGE_QUEUE
		|| interrupted
# endif
		|| wait_time > 0
		|| (wtime < 0 && !did_start_blocking))
	    // no character available, but something to be done, keep going
	    തുടരുക;

	// no character available or interrupted, return zero
	നിർത്തുക;
    }
    തിരികെ 0;
}
#endif

#എങ്കിൽ defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    പൂർണ്ണസംഖ്യ
ui_wait_for_chars_or_timer(
    long    wtime,
    പൂർണ്ണസംഖ്യ	    (*wait_func)(long wtime, പൂർണ്ണസംഖ്യ *interrupted, പൂർണ്ണസംഖ്യ ignore_നിവേശം),
    പൂർണ്ണസംഖ്യ	    *interrupted,
    പൂർണ്ണസംഖ്യ	    ignore_നിവേശം)
{
    പൂർണ്ണസംഖ്യ	    due_time;
    long    remaining = wtime;
    പൂർണ്ണസംഖ്യ	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_JOB_CHANNEL
    പൂർണ്ണസംഖ്യ	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    എങ്കിൽ (wtime >= 0 && wtime < 10L)
	തിരികെ wait_func(wtime, ശൂന്യ, ignore_നിവേശം);

    ആകുമ്പോൾ (wtime < 0 || remaining > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_time = check_due_timer();
	എങ്കിൽ (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    തിരികെ FAIL;
	}
	എങ്കിൽ (due_time <= 0 || (wtime > 0 && due_time > remaining))
	    due_time = remaining;
# എങ്കിൽ defined(FEAT_JOB_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	എങ്കിൽ ((due_time < 0 || due_time > 10L) && (
#  എങ്കിൽ defined(FEAT_JOB_CHANNEL)
		(
#   എങ്കിൽ defined(FEAT_GUI)
		!gui.in_use &&
#   endif
		(has_pending_job() || channel_any_readahead()))
#   എങ്കിൽ defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  എങ്കിൽ defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
		    has_any_sound_callback()
#  endif
		    ))
	{
	    // There is a pending job or channel, should return soon in order
	    // to handle them ASAP.  Do check for input briefly.
	    due_time = 10L;
#  ifdef FEAT_JOB_CHANNEL
	    brief_wait = TRUE;
#  endif
	}
# endif
	എങ്കിൽ (wait_func(due_time, interrupted, ignore_നിവേശം))
	    തിരികെ OK;
	എങ്കിൽ ((interrupted != ശൂന്യ && *interrupted)
# ifdef FEAT_JOB_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    തിരികെ FAIL;
	എങ്കിൽ (wtime > 0)
	    remaining -= due_time;
    }
    തിരികെ FAIL;
}
#endif

/*
 * Return non-zero if a character is available.
 */
    പൂർണ്ണസംഖ്യ
ui_char_avail(ശൂന്യം)
{
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	gui_mch_update();
	തിരികെ നിവേശം_available();
    }
#endif
#ifndef NO_CONSOLE
# ifdef NO_CONSOLE_നിവേശം
    എങ്കിൽ (no_console_നിവേശം())
	തിരികെ 0;
# endif
    തിരികെ mch_char_avail();
#അല്ലെങ്കിൽ
    തിരികെ 0;
#endif
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    ശൂന്യം
ui_delay(long msec_arg, പൂർണ്ണസംഖ്യ ignoreinput)
{
    long msec = msec_arg;

#ifdef FEAT_EVAL
    എങ്കിൽ (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(ശൂന്യ, "ui_delay(%ld)", msec);
#endif
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    അല്ലെങ്കിൽ
#endif
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    ശൂന്യം
ui_suspend(ശൂന്യം)
{
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	gui_mch_iconify();
	തിരികെ;
    }
#endif
    mch_suspend();
}

#എങ്കിൽ !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    ശൂന്യം
suspend_shell(ശൂന്യം)
{
    എങ്കിൽ (*p_sh == NUL)
	emsg(_(e_shell_option_is_empty));
    അല്ലെങ്കിൽ
    {
	സന്ദേശം_puts(_("new shell started\n"));
	do_shell(ശൂന്യ, 0);
    }
}
#endif

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
ui_get_shellsize(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    retval;

#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	retval = gui_get_shellsize();
    അല്ലെങ്കിൽ
#endif
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    എങ്കിൽ (retval == OK)
    {
	set_സംഖ്യ_default("lines", Rows);
	set_സംഖ്യ_default("columns", Columns);
    }
    തിരികെ retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    ശൂന്യം
ui_set_shellsize(
    പൂർണ്ണസംഖ്യ		mustset UNUSED)	// set by the user
{
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	gui_set_shellsize(mustset, TRUE, RESIZE_BOTH);
    അല്ലെങ്കിൽ
#endif
	mch_set_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    ശൂന്യം
ui_new_shellsize(ശൂന്യം)
{
    എങ്കിൽ (full_screen && !exiting)
    {
#ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use)
	    gui_new_shellsize();
	അല്ലെങ്കിൽ
#endif
	    mch_new_shellsize();
    }
}

#എങ്കിൽ (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    പൂർണ്ണസംഖ്യ
ui_get_winpos(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y, varnumber_T timeout UNUSED)
{
# ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	തിരികെ gui_mch_get_winpos(x, y);
# endif
# എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    തിരികെ mch_get_winpos(x, y);
# അല്ലെങ്കിൽ
#  എങ്കിൽ defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    തിരികെ term_get_winpos(x, y, timeout);
#  അല്ലെങ്കിൽ
    തിരികെ FAIL;
#  endif
# endif
}
#endif

    ശൂന്യം
ui_breakcheck(ശൂന്യം)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    ശൂന്യം
ui_breakcheck_force(പൂർണ്ണസംഖ്യ force)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;
    പൂർണ്ണസംഖ്യ		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    എങ്കിൽ (recursive)
	തിരികെ;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	gui_mch_update();
    അല്ലെങ്കിൽ
#endif
	mch_breakcheck(force);

    എങ്കിൽ (save_updating_screen)
	updating_screen = TRUE;
    അല്ലെങ്കിൽ
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

#എങ്കിൽ defined(USE_നിവേശം_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# എങ്കിൽ defined(FEAT_JOB_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# അല്ലെങ്കിൽ
#  define INBUFLEN 250
# endif

സ്ഥിതം char_u	inbuf[INBUFLEN + MAX_താക്കോൽ_CODE_LEN];
സ്ഥിതം പൂർണ്ണസംഖ്യ	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    പൂർണ്ണസംഖ്യ
vim_is_നിവേശം_buf_full(ശൂന്യം)
{
    തിരികെ (inbufcount >= INBUFLEN);
}

    പൂർണ്ണസംഖ്യ
vim_is_നിവേശം_buf_empty(ശൂന്യം)
{
    തിരികെ (inbufcount == 0);
}

# എങ്കിൽ defined(FEAT_OLE)
    പൂർണ്ണസംഖ്യ
vim_free_in_നിവേശം_buf(ശൂന്യം)
{
    തിരികെ (INBUFLEN - inbufcount);
}
# endif

# എങ്കിൽ defined(FEAT_GUI_GTK)
    പൂർണ്ണസംഖ്യ
vim_used_in_നിവേശം_buf(ശൂന്യം)
{
    തിരികെ inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_നിവേശം_buf(ശൂന്യം)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    എങ്കിൽ (gap != ശൂന്യ)
    {
	// Add one to avoid a zero size.
	gap->ga_data = alloc(inbufcount + 1);
	എങ്കിൽ (gap->ga_data != ശൂന്യ)
	    mch_memmove(gap->ga_data, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_നിവേശം_buf();
    തിരികെ (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    ശൂന്യം
set_നിവേശം_buf(char_u *p, പൂർണ്ണസംഖ്യ overwrite)
{
    garray_T	*gap = (garray_T *)p;

    എങ്കിൽ (gap == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (gap->ga_data != ശൂന്യ)
    {
	എങ്കിൽ (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_data, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	അല്ലെങ്കിൽ
	{
	    mch_memmove(inbuf + gap->ga_len, inbuf, inbufcount);
	    mch_memmove(inbuf, gap->ga_data, gap->ga_len);
	    inbufcount += gap->ga_len;
	}
	vim_free(gap->ga_data);
    }
    vim_free(gap);
}

/*
 * Add the given bytes to the input buffer
 * Special keys start with CSI.  A real CSI must have been translated to
 * CSI KS_EXTRA KE_CSI.  K_SPECIAL doesn't require translation.
 */
    ശൂന്യം
add_to_നിവേശം_buf(char_u *s, പൂർണ്ണസംഖ്യ len)
{
    എങ്കിൽ (inbufcount + len > INBUFLEN + MAX_താക്കോൽ_CODE_LEN)
	തിരികെ;	    // Shouldn't ever happen!

    ആകുമ്പോൾ (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    ശൂന്യം
add_to_നിവേശം_buf_csi(char_u *str, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	buf[2];

    വേണ്ടി (i = 0; i < len; ++i)
    {
	add_to_നിവേശം_buf(str + i, 1);
	എങ്കിൽ (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (പൂർണ്ണസംഖ്യ)KE_CSI;
	    add_to_നിവേശം_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    ശൂന്യം
trash_നിവേശം_buf(ശൂന്യം)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    പൂർണ്ണസംഖ്യ
read_from_നിവേശം_buf(char_u *buf, long maxlen)
{
    എങ്കിൽ (inbufcount == 0)	// if the buffer is empty, fill it
	fill_നിവേശം_buf(TRUE);
    എങ്കിൽ (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    എങ്കിൽ (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    തിരികെ (പൂർണ്ണസംഖ്യ)maxlen;
}

    ശൂന്യം
fill_നിവേശം_buf(പൂർണ്ണസംഖ്യ exit_on_error UNUSED)
{
# എങ്കിൽ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		try;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	did_read_something = FALSE;
    സ്ഥിതം char_u *rest = ശൂന്യ;	    // unconverted rest of previous read
    സ്ഥിതം പൂർണ്ണസംഖ്യ	restlen = 0;
    പൂർണ്ണസംഖ്യ		unconverted;
# endif

# ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use
#  ifdef NO_CONSOLE_നിവേശം
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_നിവേശം()
#  endif
       )
    {
	gui_mch_update();
	തിരികെ;
    }
# endif
# എങ്കിൽ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    എങ്കിൽ (vim_is_നിവേശം_buf_full())
	തിരികെ;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    എങ്കിൽ (rest != ശൂന്യ)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	എങ്കിൽ (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	അല്ലെങ്കിൽ
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	എങ്കിൽ (unconverted == restlen)
	    VIM_CLEAR(rest);
	അല്ലെങ്കിൽ
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    അല്ലെങ്കിൽ
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    വേണ്ടി (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / നിവേശം_conv.vc_factor);
#  ifdef VMS
	len = vms_read((അക്ഷരം *)inbuf + inbufcount, readlen);
#  അല്ലെങ്കിൽ
	len = പഠ(read_cmd_fd, (അക്ഷരം *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	എങ്കിൽ (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(ശൂന്യ, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	എങ്കിൽ (len > 0 || got_int)
	    നിർത്തുക;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	എങ്കിൽ (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    പൂർണ്ണസംഖ്യ m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  അല്ലെങ്കിൽ
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	എങ്കിൽ (!exit_on_error)
	    തിരികെ;
    }
    എങ്കിൽ (len <= 0 && !got_int)
	read_error_exit();
    എങ്കിൽ (len > 0)
	did_read_something = TRUE;
    എങ്കിൽ (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	എങ്കിൽ (നിവേശം_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_നിവേശം_safe(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == ശൂന്യ ? &rest : ശൂന്യ, &restlen);
	}
	ആകുമ്പോൾ (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    എങ്കിൽ (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !താക്കോൽ_protocol_enabled())
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
#endif // USE_INPUT_BUF

/*
 * Exit because of an input read error.
 */
    ശൂന്യം
read_error_exit(ശൂന്യം)
{
    എങ്കിൽ (silent_mode)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#എങ്കിൽ defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    ശൂന്യം
ui_cursor_shape_forced(പൂർണ്ണസംഖ്യ forced)
{
# ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	gui_update_cursor_later();
    അല്ലെങ്കിൽ
# endif
	term_cursor_mode(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_വരി(FALSE);
# endif
}

    ശൂന്യം
ui_cursor_shape(ശൂന്യം)
{
    ui_cursor_shape_forced(FALSE);
}
#endif

/*
 * Check bounds for column number
 */
    പൂർണ്ണസംഖ്യ
check_col(പൂർണ്ണസംഖ്യ col)
{
    എങ്കിൽ (col < 0)
	തിരികെ 0;
    എങ്കിൽ (col >= screen_Columns)
	തിരികെ screen_Columns - 1;
    തിരികെ col;
}

/*
 * Check bounds for row number
 */
    പൂർണ്ണസംഖ്യ
check_row(പൂർണ്ണസംഖ്യ row)
{
    എങ്കിൽ (row < 0)
	തിരികെ 0;
    എങ്കിൽ (row >= screen_Rows)
	തിരികെ screen_Rows - 1;
    തിരികെ row;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    long
scroll_വരി_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	col = 0;

    എങ്കിൽ (*p != NUL)
	വേണ്ടി (;;)
	{
	    പൂർണ്ണസംഖ്യ	    w = chartabsize(p, col);
	    MB_PTR_ADV(p);
	    എങ്കിൽ (*p == NUL)		// don't count the last character
		നിർത്തുക;
	    col += w;
	}
    തിരികെ col;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_find_longest_lnum(ശൂന്യം)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    എങ്കിൽ (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == ശൂന്യ) &&
#endif
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_വരി_എണ്ണം + 1)
    {
	linenr_T    lnum;
	long	    n;
	long	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	വേണ്ടി (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_വരി_len(lnum);
	    എങ്കിൽ (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (n == max && abs((പൂർണ്ണസംഖ്യ)(lnum - curwin->w_cursor.lnum))
				     < abs((പൂർണ്ണസംഖ്യ)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    അല്ലെങ്കിൽ
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    തിരികെ ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    ശൂന്യം
ui_focus_change(
    പൂർണ്ണസംഖ്യ		in_focus)	// TRUE if focus gained.
{
    സ്ഥിതം time_t	last_time = (time_t)0;
    പൂർണ്ണസംഖ്യ			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    എങ്കിൽ (in_focus && last_time + 2 < time(ശൂന്യ))
    {
	need_redraw = check_timestamps(
#ifdef FEAT_GUI
		gui.in_use
#അല്ലെങ്കിൽ
		FALSE
#endif
		);
	last_time = time(ശൂന്യ);
    }

#ifdef FEAT_TERMINAL
    term_focus_change(in_focus);
#endif

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(in_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);

    എങ്കിൽ (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    എങ്കിൽ (need_maketitle)
	maketitle();
}

#എങ്കിൽ defined(HAVE_നിവേശം_METHOD)
/*
 * Save current Input Method status to specified place.
 */
    ശൂന്യം
im_save_status(long *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    എങ്കിൽ (!p_imdisable && താക്കോൽ_Typed && !താക്കോൽ_Stuffed
# ifdef FEAT_XIM
	    && xic != ശൂന്യ
# endif
# ifdef FEAT_GUI
	    && (!gui.in_use || gui.in_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	എങ്കിൽ (vgetc_im_active)
	    *psave = B_IMODE_IM;
	അല്ലെങ്കിൽ എങ്കിൽ (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#endif
