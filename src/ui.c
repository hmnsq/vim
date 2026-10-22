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

#অন্তর্ভাব "vim.h"

    শূন্য
ui_write(char_u *s, পূর্ণাঙ্ক len, পূর্ণাঙ্ক console UNUSED)
{
#ifdef FEAT_GUI
    যদি (gui.in_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	যদি (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	প্রত্যাগম;
    }
#endif
#ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    যদি (!(silent_mode && p_verbose == 0))
    {
# যদি !defined(MSWIN)
	char_u	*tofree = রিক্ত;

	যদি (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    যদি (tofree != রিক্ত)
		s = tofree;
	}
# endif

	mch_write(s, len);
# যদি defined(HAVE_FSYNC)
	যদি (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# যদি !defined(MSWIN)
	যদি (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#endif
}

#যদি defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
স্থির char_u *ta_str = রিক্ত;
স্থির পূর্ণাঙ্ক ta_off;	// offset for next char to use when ta_str != NULL
স্থির পূর্ণাঙ্ক ta_len;	// length of ta_str when it's not NULL

    শূন্য
ui_inchar_undo(char_u *s, পূর্ণাঙ্ক len)
{
    char_u  *new;
    পূর্ণাঙ্ক	    newlen;

    newlen = len;
    যদি (ta_str != রিক্ত)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    যদি (new == রিক্ত)
	প্রত্যাগম;

    যদি (ta_str != রিক্ত)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    অন্যথা
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
    পূর্ণাঙ্ক
ui_inchar(
    char_u	*buf,
    পূর্ণাঙ্ক		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    পূর্ণাঙ্ক		tb_change_cnt)
{
    পূর্ণাঙ্ক		retval = 0;

#যদি defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    যদি (ta_str != রিক্ত)
    {
	যদি (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    প্রত্যাগম ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	প্রত্যাগম maxlen;
    }
#endif

#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#endif

#ifdef NO_CONSOLE_প্রবেশ
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    যদি (no_console_প্রবেশ())
    {
	স্থির পূর্ণাঙ্ক গুনতি = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	যদি (retval > 0 || typebuf_changed(tb_change_cnt) || wtime >= 0)
	    গচ্ছ theend;
# endif
	যদি (wtime == -1 && ++গুনতি == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	গচ্ছ theend;
    }
#endif

    // If we are going to wait for some time or block...
    যদি (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(শূন্য)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	যদি ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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
    যদি (gui.in_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif
#ifndef NO_CONSOLE
# ifdef FEAT_GUI
    অন্যথা
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif

    যদি (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(শূন্য)vim_handle_signal(SIGNAL_BLOCK);

    ctrl_c_interrupts = TRUE;

#ifdef NO_CONSOLE_প্রবেশ
theend:
#endif
#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#endif
    প্রত্যাগম retval;
}

#যদি defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    পূর্ণাঙ্ক
inchar_loop(
    char_u	*buf,
    পূর্ণাঙ্ক		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    পূর্ণাঙ্ক		tb_change_cnt,
    পূর্ণাঙ্ক		(*wait_func)(long wtime, পূর্ণাঙ্ক *interrupted, পূর্ণাঙ্ক ignore_প্রবেশ),
    পূর্ণাঙ্ক		(*resize_func)(পূর্ণাঙ্ক check_only))
{
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		interrupted = FALSE;
    পূর্ণাঙ্ক		did_call_wait_func = FALSE;
    পূর্ণাঙ্ক		did_start_blocking = FALSE;
    long	wait_time;
    long	elapsed_time = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
# endif

    // repeat until we got a character or waited long enough
    প্রতি (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	যদি (resize_func != রিক্ত)
	    resize_func(FALSE);

# ifdef MESSAGE_QUEUE
	// Only process messages when waiting.
	যদি (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    যদি (typebuf_changed(tb_change_cnt))
		প্রত্যাগম 0;
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
		&& (get_real_state() & MODE_ঢোকাও) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	long acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	যদি (wtime < 0 && did_start_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_time = -1;
	অন্যথা
	{
# ifdef ELAPSED_FUNC
	    elapsed_time = ELAPSED_FUNC(start_tv);
# endif
	    যদি (wtime >= 0)
		wait_time = wtime - elapsed_time;
	    অন্যথা যদি (delay_pending)
	    {
		long delay_বাম = p_acl - acl_elapsed;
		long ut_বাম = p_ut - elapsed_time;

		যদি (did_start_blocking)
		    wait_time = delay_বাম;
		অন্যথা
		    wait_time = MIN(delay_বাম, ut_বাম);
	    }
	    অন্যথা
		// going to block after p_ut
		wait_time = p_ut - elapsed_time;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    যদি (wait_time <= 0 && did_call_wait_func)
	    {
		যদি (wtime >= 0)
		    // no character available within "wtime"
		    প্রত্যাগম 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		যদি (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    যদি (buf == রিক্ত)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (পূর্ণাঙ্ক)KE_COMPLETE_DELAY;
			add_to_প্রবেশ_buf(ibuf, 3);
		    }
		    অন্যথা
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (পূর্ণাঙ্ক)KE_COMPLETE_DELAY;
		    }
		    প্রত্যাগম 3;
		}

		// No character available within 'updatetime'.
		did_start_blocking = TRUE;
		যদি (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    যদি (buf == রিক্ত)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (পূর্ণাঙ্ক)KE_CURSORHOLD;
			add_to_প্রবেশ_buf(ibuf, 3);
		    }
		    অন্যথা
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (পূর্ণাঙ্ক)KE_CURSORHOLD;
		    }
		    প্রত্যাগম 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		অনুবর্ত;
	    }
	}

# ifdef FEAT_JOB_CHANNEL
	যদি (wait_time < 0 || wait_time > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    যদি (has_pending_job())
		wait_time = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    যদি (channel_any_readahead())
		wait_time = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	যদি (p_beval && wait_time > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_time = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	যদি (wait_func(wait_time, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    যদি (typebuf_changed(tb_change_cnt))
		প্রত্যাগম 0;

	    // We might have something to return now.
	    যদি (buf == রিক্ত)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		প্রত্যাগম প্রবেশ_available();

	    len = read_from_প্রবেশ_buf(buf, (long)maxlen);
	    যদি (len > 0)
		প্রত্যাগম len;
	    অনুবর্ত;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_time += wait_time;
# endif

	যদি ((resize_func != রিক্ত && resize_func(TRUE))
# যদি defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
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
	    অনুবর্ত;

	// no character available or interrupted, return zero
	ভঙ্গ;
    }
    প্রত্যাগম 0;
}
#endif

#যদি defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    পূর্ণাঙ্ক
ui_wait_for_chars_or_timer(
    long    wtime,
    পূর্ণাঙ্ক	    (*wait_func)(long wtime, পূর্ণাঙ্ক *interrupted, পূর্ণাঙ্ক ignore_প্রবেশ),
    পূর্ণাঙ্ক	    *interrupted,
    পূর্ণাঙ্ক	    ignore_প্রবেশ)
{
    পূর্ণাঙ্ক	    due_time;
    long    remaining = wtime;
    পূর্ণাঙ্ক	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_JOB_CHANNEL
    পূর্ণাঙ্ক	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    যদি (wtime >= 0 && wtime < 10L)
	প্রত্যাগম wait_func(wtime, রিক্ত, ignore_প্রবেশ);

    যাবত্ (wtime < 0 || remaining > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_time = check_due_timer();
	যদি (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    প্রত্যাগম FAIL;
	}
	যদি (due_time <= 0 || (wtime > 0 && due_time > remaining))
	    due_time = remaining;
# যদি defined(FEAT_JOB_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	যদি ((due_time < 0 || due_time > 10L) && (
#  যদি defined(FEAT_JOB_CHANNEL)
		(
#   যদি defined(FEAT_GUI)
		!gui.in_use &&
#   endif
		(has_pending_job() || channel_any_readahead()))
#   যদি defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  যদি defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
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
	যদি (wait_func(due_time, interrupted, ignore_প্রবেশ))
	    প্রত্যাগম OK;
	যদি ((interrupted != রিক্ত && *interrupted)
# ifdef FEAT_JOB_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    প্রত্যাগম FAIL;
	যদি (wtime > 0)
	    remaining -= due_time;
    }
    প্রত্যাগম FAIL;
}
#endif

/*
 * Return non-zero if a character is available.
 */
    পূর্ণাঙ্ক
ui_char_avail(শূন্য)
{
#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	gui_mch_update();
	প্রত্যাগম প্রবেশ_available();
    }
#endif
#ifndef NO_CONSOLE
# ifdef NO_CONSOLE_প্রবেশ
    যদি (no_console_প্রবেশ())
	প্রত্যাগম 0;
# endif
    প্রত্যাগম mch_char_avail();
#অন্যথা
    প্রত্যাগম 0;
#endif
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    শূন্য
ui_delay(long msec_arg, পূর্ণাঙ্ক ignoreinput)
{
    long msec = msec_arg;

#ifdef FEAT_EVAL
    যদি (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(রিক্ত, "ui_delay(%ld)", msec);
#endif
#ifdef FEAT_GUI
    যদি (gui.in_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    অন্যথা
#endif
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    শূন্য
ui_suspend(শূন্য)
{
#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	gui_mch_iconify();
	প্রত্যাগম;
    }
#endif
    mch_suspend();
}

#যদি !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    শূন্য
suspend_shell(শূন্য)
{
    যদি (*p_sh == NUL)
	emsg(_(e_shell_option_is_empty));
    অন্যথা
    {
	বার্তা_puts(_("new shell started\n"));
	do_shell(রিক্ত, 0);
    }
}
#endif

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    পূর্ণাঙ্ক
ui_get_shellsize(শূন্য)
{
    পূর্ণাঙ্ক	    retval;

#ifdef FEAT_GUI
    যদি (gui.in_use)
	retval = gui_get_shellsize();
    অন্যথা
#endif
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    যদি (retval == OK)
    {
	set_সংখ্যা_default("lines", Rows);
	set_সংখ্যা_default("columns", Columns);
    }
    প্রত্যাগম retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    শূন্য
ui_set_shellsize(
    পূর্ণাঙ্ক		mustset UNUSED)	// set by the user
{
#ifdef FEAT_GUI
    যদি (gui.in_use)
	gui_set_shellsize(mustset, TRUE, RESIZE_BOTH);
    অন্যথা
#endif
	mch_set_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    শূন্য
ui_new_shellsize(শূন্য)
{
    যদি (full_screen && !exiting)
    {
#ifdef FEAT_GUI
	যদি (gui.in_use)
	    gui_new_shellsize();
	অন্যথা
#endif
	    mch_new_shellsize();
    }
}

#যদি (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    পূর্ণাঙ্ক
ui_get_winpos(পূর্ণাঙ্ক *x, পূর্ণাঙ্ক *y, varnumber_T timeout UNUSED)
{
# ifdef FEAT_GUI
    যদি (gui.in_use)
	প্রত্যাগম gui_mch_get_winpos(x, y);
# endif
# যদি defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    প্রত্যাগম mch_get_winpos(x, y);
# অন্যথা
#  যদি defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    প্রত্যাগম term_get_winpos(x, y, timeout);
#  অন্যথা
    প্রত্যাগম FAIL;
#  endif
# endif
}
#endif

    শূন্য
ui_breakcheck(শূন্য)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    শূন্য
ui_breakcheck_force(পূর্ণাঙ্ক force)
{
    স্থির পূর্ণাঙ্ক	recursive = FALSE;
    পূর্ণাঙ্ক		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    যদি (recursive)
	প্রত্যাগম;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ifdef FEAT_GUI
    যদি (gui.in_use)
	gui_mch_update();
    অন্যথা
#endif
	mch_breakcheck(force);

    যদি (save_updating_screen)
	updating_screen = TRUE;
    অন্যথা
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

#যদি defined(USE_প্রবেশ_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# যদি defined(FEAT_JOB_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# অন্যথা
#  define INBUFLEN 250
# endif

স্থির char_u	inbuf[INBUFLEN + MAX_চাবি_CODE_LEN];
স্থির পূর্ণাঙ্ক	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    পূর্ণাঙ্ক
vim_is_প্রবেশ_buf_full(শূন্য)
{
    প্রত্যাগম (inbufcount >= INBUFLEN);
}

    পূর্ণাঙ্ক
vim_is_প্রবেশ_buf_empty(শূন্য)
{
    প্রত্যাগম (inbufcount == 0);
}

# যদি defined(FEAT_OLE)
    পূর্ণাঙ্ক
vim_free_in_প্রবেশ_buf(শূন্য)
{
    প্রত্যাগম (INBUFLEN - inbufcount);
}
# endif

# যদি defined(FEAT_GUI_GTK)
    পূর্ণাঙ্ক
vim_used_in_প্রবেশ_buf(শূন্য)
{
    প্রত্যাগম inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_প্রবেশ_buf(শূন্য)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    যদি (gap != রিক্ত)
    {
	// Add one to avoid a zero size.
	gap->ga_data = alloc(inbufcount + 1);
	যদি (gap->ga_data != রিক্ত)
	    mch_memmove(gap->ga_data, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_প্রবেশ_buf();
    প্রত্যাগম (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    শূন্য
set_প্রবেশ_buf(char_u *p, পূর্ণাঙ্ক overwrite)
{
    garray_T	*gap = (garray_T *)p;

    যদি (gap == রিক্ত)
	প্রত্যাগম;

    যদি (gap->ga_data != রিক্ত)
    {
	যদি (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_data, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	অন্যথা
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
    শূন্য
add_to_প্রবেশ_buf(char_u *s, পূর্ণাঙ্ক len)
{
    যদি (inbufcount + len > INBUFLEN + MAX_চাবি_CODE_LEN)
	প্রত্যাগম;	    // Shouldn't ever happen!

    যাবত্ (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    শূন্য
add_to_প্রবেশ_buf_csi(char_u *str, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক		i;
    char_u	buf[2];

    প্রতি (i = 0; i < len; ++i)
    {
	add_to_প্রবেশ_buf(str + i, 1);
	যদি (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (পূর্ণাঙ্ক)KE_CSI;
	    add_to_প্রবেশ_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    শূন্য
trash_প্রবেশ_buf(শূন্য)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    পূর্ণাঙ্ক
read_from_প্রবেশ_buf(char_u *buf, long maxlen)
{
    যদি (inbufcount == 0)	// if the buffer is empty, fill it
	fill_প্রবেশ_buf(TRUE);
    যদি (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    যদি (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    প্রত্যাগম (পূর্ণাঙ্ক)maxlen;
}

    শূন্য
fill_প্রবেশ_buf(পূর্ণাঙ্ক exit_on_error UNUSED)
{
# যদি defined(UNIX) || defined(VMS) || defined(MACOS_X)
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		try;
    স্থির পূর্ণাঙ্ক	did_read_something = FALSE;
    স্থির char_u *rest = রিক্ত;	    // unconverted rest of previous read
    স্থির পূর্ণাঙ্ক	restlen = 0;
    পূর্ণাঙ্ক		unconverted;
# endif

# ifdef FEAT_GUI
    যদি (gui.in_use
#  ifdef NO_CONSOLE_প্রবেশ
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_প্রবেশ()
#  endif
       )
    {
	gui_mch_update();
	প্রত্যাগম;
    }
# endif
# যদি defined(UNIX) || defined(VMS) || defined(MACOS_X)
    যদি (vim_is_প্রবেশ_buf_full())
	প্রত্যাগম;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    যদি (rest != রিক্ত)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	যদি (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	অন্যথা
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	যদি (unconverted == restlen)
	    VIM_CLEAR(rest);
	অন্যথা
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    অন্যথা
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    প্রতি (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / প্রবেশ_conv.vc_factor);
#  ifdef VMS
	len = vms_read((বর্ণ *)inbuf + inbufcount, readlen);
#  অন্যথা
	len = পঠ(read_cmd_fd, (বর্ণ *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	যদি (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(রিক্ত, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	যদি (len > 0 || got_int)
	    ভঙ্গ;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	যদি (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    পূর্ণাঙ্ক m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  অন্যথা
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	যদি (!exit_on_error)
	    প্রত্যাগম;
    }
    যদি (len <= 0 && !got_int)
	read_error_exit();
    যদি (len > 0)
	did_read_something = TRUE;
    যদি (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    অন্যথা
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	যদি (প্রবেশ_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_প্রবেশ_safe(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == রিক্ত ? &rest : রিক্ত, &restlen);
	}
	যাবত্ (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    যদি (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !চাবি_protocol_enabled())
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
    শূন্য
read_error_exit(শূন্য)
{
    যদি (silent_mode)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#যদি defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    শূন্য
ui_cursor_shape_forced(পূর্ণাঙ্ক forced)
{
# ifdef FEAT_GUI
    যদি (gui.in_use)
	gui_update_cursor_later();
    অন্যথা
# endif
	term_cursor_mode(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_সারি(FALSE);
# endif
}

    শূন্য
ui_cursor_shape(শূন্য)
{
    ui_cursor_shape_forced(FALSE);
}
#endif

/*
 * Check bounds for column number
 */
    পূর্ণাঙ্ক
check_col(পূর্ণাঙ্ক col)
{
    যদি (col < 0)
	প্রত্যাগম 0;
    যদি (col >= screen_Columns)
	প্রত্যাগম screen_Columns - 1;
    প্রত্যাগম col;
}

/*
 * Check bounds for row number
 */
    পূর্ণাঙ্ক
check_row(পূর্ণাঙ্ক row)
{
    যদি (row < 0)
	প্রত্যাগম 0;
    যদি (row >= screen_Rows)
	প্রত্যাগম screen_Rows - 1;
    প্রত্যাগম row;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    long
scroll_সারি_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	col = 0;

    যদি (*p != NUL)
	প্রতি (;;)
	{
	    পূর্ণাঙ্ক	    w = chartabsize(p, col);
	    MB_PTR_ADV(p);
	    যদি (*p == NUL)		// don't count the last character
		ভঙ্গ;
	    col += w;
	}
    প্রত্যাগম col;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_find_longest_lnum(শূন্য)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    যদি (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == রিক্ত) &&
#endif
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_সারি_গুনতি + 1)
    {
	linenr_T    lnum;
	long	    n;
	long	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	প্রতি (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_সারি_len(lnum);
	    যদি (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    অন্যথা যদি (n == max && abs((পূর্ণাঙ্ক)(lnum - curwin->w_cursor.lnum))
				     < abs((পূর্ণাঙ্ক)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    অন্যথা
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    প্রত্যাগম ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    শূন্য
ui_focus_change(
    পূর্ণাঙ্ক		in_focus)	// TRUE if focus gained.
{
    স্থির time_t	last_time = (time_t)0;
    পূর্ণাঙ্ক			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    যদি (in_focus && last_time + 2 < time(রিক্ত))
    {
	need_redraw = check_timestamps(
#ifdef FEAT_GUI
		gui.in_use
#অন্যথা
		FALSE
#endif
		);
	last_time = time(রিক্ত);
    }

#ifdef FEAT_TERMINAL
    term_focus_change(in_focus);
#endif

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(in_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, রিক্ত, রিক্ত, FALSE, curbuf);

    যদি (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    যদি (need_maketitle)
	maketitle();
}

#যদি defined(HAVE_প্রবেশ_METHOD)
/*
 * Save current Input Method status to specified place.
 */
    শূন্য
im_save_status(long *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    যদি (!p_imdisable && চাবি_Typed && !চাবি_Stuffed
# ifdef FEAT_XIM
	    && xic != রিক্ত
# endif
# ifdef FEAT_GUI
	    && (!gui.in_use || gui.in_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	যদি (vgetc_im_active)
	    *psave = B_IMODE_IM;
	অন্যথা যদি (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#endif
