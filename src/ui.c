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

#ଅନ୍ତର୍ଭାଵ "vim.h"

    ଶୂନ୍ୟ
ui_write(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା console UNUSED)
{
#ifdef FEAT_GUI
    ଯଦି (gui.in_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	ଯଦି (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	ଫେରନ୍ତୁ;
    }
#endif
#ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    ଯଦି (!(silent_mode && p_verbose == 0))
    {
# ଯଦି !defined(MSWIN)
	char_u	*tofree = ଖାଲି;

	ଯଦି (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    ଯଦି (tofree != ଖାଲି)
		s = tofree;
	}
# endif

	mch_write(s, len);
# ଯଦି defined(HAVE_FSYNC)
	ଯଦି (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# ଯଦି !defined(MSWIN)
	ଯଦି (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#endif
}

#ଯଦି defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
ସ୍ଥିର char_u *ta_str = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ta_off;	// offset for next char to use when ta_str != NULL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ta_len;	// length of ta_str when it's not NULL

    ଶୂନ୍ୟ
ui_inchar_undo(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u  *new;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    newlen;

    newlen = len;
    ଯଦି (ta_str != ଖାଲି)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    ଯଦି (new == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (ta_str != ଖାଲି)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ui_inchar(
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tb_change_cnt)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = 0;

#ଯଦି defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    ଯଦି (ta_str != ଖାଲି)
    {
	ଯଦି (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    ଫେରନ୍ତୁ ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	ଫେରନ୍ତୁ maxlen;
    }
#endif

#ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#endif

#ifdef NO_CONSOLE_ନିବେଶ
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    ଯଦି (no_console_ନିବେଶ())
    {
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	ଯଦି (retval > 0 || typebuf_changed(tb_change_cnt) || wtime >= 0)
	    ଯାଅ theend;
# endif
	ଯଦି (wtime == -1 && ++ଗଣନା_2 == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	ଯାଅ theend;
    }
#endif

    // If we are going to wait for some time or block...
    ଯଦି (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(ଶୂନ୍ୟ)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	ଯଦି ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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
    ଯଦି (gui.in_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif
#ifndef NO_CONSOLE
# ifdef FEAT_GUI
    ଅନ୍ୟଥା
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif

    ଯଦି (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(ଶୂନ୍ୟ)vim_handle_signal(SIGNAL_BLOCK);

    ctrl_c_interrupts = TRUE;

#ifdef NO_CONSOLE_ନିବେଶ
theend:
#endif
#ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#endif
    ଫେରନ୍ତୁ retval;
}

#ଯଦି defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
inchar_loop(
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tb_change_cnt,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		(*wait_func)(long wtime, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *interrupted, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_ନିବେଶ),
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		(*resize_func)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_only))
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		interrupted = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_call_wait_func = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_start_blocking = FALSE;
    long	wait_time;
    long	elapsed_time = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
# endif

    // repeat until we got a character or waited long enough
    ପାଇଁ (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	ଯଦି (resize_func != ଖାଲି)
	    resize_func(FALSE);

# ifdef MESSAGE_QUEUE
	// Only process messages when waiting.
	ଯଦି (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    ଯଦି (typebuf_changed(tb_change_cnt))
		ଫେରନ୍ତୁ 0;
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
		&& (get_real_state() & MODE_ଭର୍ତ୍ତି) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	long acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	ଯଦି (wtime < 0 && did_start_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_time = -1;
	ଅନ୍ୟଥା
	{
# ifdef ELAPSED_FUNC
	    elapsed_time = ELAPSED_FUNC(start_tv);
# endif
	    ଯଦି (wtime >= 0)
		wait_time = wtime - elapsed_time;
	    ଅନ୍ୟଥା ଯଦି (delay_pending)
	    {
		long delay_ବାମ = p_acl - acl_elapsed;
		long ut_ବାମ = p_ut - elapsed_time;

		ଯଦି (did_start_blocking)
		    wait_time = delay_ବାମ;
		ଅନ୍ୟଥା
		    wait_time = MIN(delay_ବାମ, ut_ବାମ);
	    }
	    ଅନ୍ୟଥା
		// going to block after p_ut
		wait_time = p_ut - elapsed_time;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    ଯଦି (wait_time <= 0 && did_call_wait_func)
	    {
		ଯଦି (wtime >= 0)
		    // no character available within "wtime"
		    ଫେରନ୍ତୁ 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		ଯଦି (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    ଯଦି (buf == ଖାଲି)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_COMPLETE_DELAY;
			add_to_ନିବେଶ_buf(ibuf, 3);
		    }
		    ଅନ୍ୟଥା
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_COMPLETE_DELAY;
		    }
		    ଫେରନ୍ତୁ 3;
		}

		// No character available within 'updatetime'.
		did_start_blocking = TRUE;
		ଯଦି (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    ଯଦି (buf == ଖାଲି)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CURSORHOLD;
			add_to_ନିବେଶ_buf(ibuf, 3);
		    }
		    ଅନ୍ୟଥା
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CURSORHOLD;
		    }
		    ଫେରନ୍ତୁ 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		ଜାରି;
	    }
	}

# ifdef FEAT_JOB_CHANNEL
	ଯଦି (wait_time < 0 || wait_time > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    ଯଦି (has_pending_job())
		wait_time = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    ଯଦି (channel_any_readahead())
		wait_time = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	ଯଦି (p_beval && wait_time > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_time = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	ଯଦି (wait_func(wait_time, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    ଯଦି (typebuf_changed(tb_change_cnt))
		ଫେରନ୍ତୁ 0;

	    // We might have something to return now.
	    ଯଦି (buf == ଖାଲି)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		ଫେରନ୍ତୁ ନିବେଶ_available();

	    len = read_from_ନିବେଶ_buf(buf, (long)maxlen);
	    ଯଦି (len > 0)
		ଫେରନ୍ତୁ len;
	    ଜାରି;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_time += wait_time;
# endif

	ଯଦି ((resize_func != ଖାଲି && resize_func(TRUE))
# ଯଦି defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
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
	    ଜାରି;

	// no character available or interrupted, return zero
	ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ 0;
}
#endif

#ଯଦି defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ui_wait_for_chars_or_timer(
    long    wtime,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    (*wait_func)(long wtime, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *interrupted, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_ନିବେଶ),
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *interrupted,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ignore_ନିବେଶ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    due_time;
    long    remaining = wtime;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_JOB_CHANNEL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    ଯଦି (wtime >= 0 && wtime < 10L)
	ଫେରନ୍ତୁ wait_func(wtime, ଖାଲି, ignore_ନିବେଶ);

    ଯେତେବେଳେ (wtime < 0 || remaining > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_time = check_due_timer();
	ଯଦି (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (due_time <= 0 || (wtime > 0 && due_time > remaining))
	    due_time = remaining;
# ଯଦି defined(FEAT_JOB_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	ଯଦି ((due_time < 0 || due_time > 10L) && (
#  ଯଦି defined(FEAT_JOB_CHANNEL)
		(
#   ଯଦି defined(FEAT_GUI)
		!gui.in_use &&
#   endif
		(has_pending_job() || channel_any_readahead()))
#   ଯଦି defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  ଯଦି defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
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
	ଯଦି (wait_func(due_time, interrupted, ignore_ନିବେଶ))
	    ଫେରନ୍ତୁ OK;
	ଯଦି ((interrupted != ଖାଲି && *interrupted)
# ifdef FEAT_JOB_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (wtime > 0)
	    remaining -= due_time;
    }
    ଫେରନ୍ତୁ FAIL;
}
#endif

/*
 * Return non-zero if a character is available.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ui_char_avail(ଶୂନ୍ୟ)
{
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	gui_mch_update();
	ଫେରନ୍ତୁ ନିବେଶ_available();
    }
#endif
#ifndef NO_CONSOLE
# ifdef NO_CONSOLE_ନିବେଶ
    ଯଦି (no_console_ନିବେଶ())
	ଫେରନ୍ତୁ 0;
# endif
    ଫେରନ୍ତୁ mch_char_avail();
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ 0;
#endif
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    ଶୂନ୍ୟ
ui_delay(long msec_arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignoreinput)
{
    long msec = msec_arg;

#ifdef FEAT_EVAL
    ଯଦି (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(ଖାଲି, "ui_delay(%ld)", msec);
#endif
#ifdef FEAT_GUI
    ଯଦି (gui.in_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    ଅନ୍ୟଥା
#endif
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    ଶୂନ୍ୟ
ui_suspend(ଶୂନ୍ୟ)
{
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	gui_mch_iconify();
	ଫେରନ୍ତୁ;
    }
#endif
    mch_suspend();
}

#ଯଦି !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    ଶୂନ୍ୟ
suspend_shell(ଶୂନ୍ୟ)
{
    ଯଦି (*p_sh == NUL)
	emsg(_(e_shell_option_is_empty));
    ଅନ୍ୟଥା
    {
	ସନ୍ଦେଶ_puts(_("new shell started\n"));
	do_shell(ଖାଲି, 0);
    }
}
#endif

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ui_get_shellsize(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval;

#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	retval = gui_get_shellsize();
    ଅନ୍ୟଥା
#endif
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    ଯଦି (retval == OK)
    {
	set_ସଂଖ୍ୟା_default("lines", Rows);
	set_ସଂଖ୍ୟା_default("columns", Columns);
    }
    ଫେରନ୍ତୁ retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    ଶୂନ୍ୟ
ui_set_shellsize(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mustset UNUSED)	// set by the user
{
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	gui_set_shellsize(mustset, TRUE, RESIZE_BOTH);
    ଅନ୍ୟଥା
#endif
	mch_set_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    ଶୂନ୍ୟ
ui_new_shellsize(ଶୂନ୍ୟ)
{
    ଯଦି (full_screen && !exiting)
    {
#ifdef FEAT_GUI
	ଯଦି (gui.in_use)
	    gui_new_shellsize();
	ଅନ୍ୟଥା
#endif
	    mch_new_shellsize();
    }
}

#ଯଦି (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ui_get_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y, varnumber_T timeout UNUSED)
{
# ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ gui_mch_get_winpos(x, y);
# endif
# ଯଦି defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    ଫେରନ୍ତୁ mch_get_winpos(x, y);
# ଅନ୍ୟଥା
#  ଯଦି defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    ଫେରନ୍ତୁ term_get_winpos(x, y, timeout);
#  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FAIL;
#  endif
# endif
}
#endif

    ଶୂନ୍ୟ
ui_breakcheck(ଶୂନ୍ୟ)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    ଶୂନ୍ୟ
ui_breakcheck_force(ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    ଯଦି (recursive)
	ଫେରନ୍ତୁ;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	gui_mch_update();
    ଅନ୍ୟଥା
#endif
	mch_breakcheck(force);

    ଯଦି (save_updating_screen)
	updating_screen = TRUE;
    ଅନ୍ୟଥା
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

#ଯଦି defined(USE_ନିବେଶ_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# ଯଦି defined(FEAT_JOB_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# ଅନ୍ୟଥା
#  define INBUFLEN 250
# endif

ସ୍ଥିର char_u	inbuf[INBUFLEN + MAX_ଚାବି_CODE_LEN];
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_is_ନିବେଶ_buf_full(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (inbufcount >= INBUFLEN);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_is_ନିବେଶ_buf_empty(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (inbufcount == 0);
}

# ଯଦି defined(FEAT_OLE)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_free_in_ନିବେଶ_buf(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (INBUFLEN - inbufcount);
}
# endif

# ଯଦି defined(FEAT_GUI_GTK)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_used_in_ନିବେଶ_buf(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_ନିବେଶ_buf(ଶୂନ୍ୟ)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    ଯଦି (gap != ଖାଲି)
    {
	// Add one to avoid a zero size.
	gap->ga_data = alloc(inbufcount + 1);
	ଯଦି (gap->ga_data != ଖାଲି)
	    mch_memmove(gap->ga_data, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_ନିବେଶ_buf();
    ଫେରନ୍ତୁ (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    ଶୂନ୍ୟ
set_ନିବେଶ_buf(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା overwrite)
{
    garray_T	*gap = (garray_T *)p;

    ଯଦି (gap == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (gap->ga_data != ଖାଲି)
    {
	ଯଦି (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_data, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
add_to_ନିବେଶ_buf(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଯଦି (inbufcount + len > INBUFLEN + MAX_ଚାବି_CODE_LEN)
	ଫେରନ୍ତୁ;	    // Shouldn't ever happen!

    ଯେତେବେଳେ (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    ଶୂନ୍ୟ
add_to_ନିବେଶ_buf_csi(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	buf[2];

    ପାଇଁ (i = 0; i < len; ++i)
    {
	add_to_ନିବେଶ_buf(str + i, 1);
	ଯଦି (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
	    add_to_ନିବେଶ_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    ଶୂନ୍ୟ
trash_ନିବେଶ_buf(ଶୂନ୍ୟ)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_from_ନିବେଶ_buf(char_u *buf, long maxlen)
{
    ଯଦି (inbufcount == 0)	// if the buffer is empty, fill it
	fill_ନିବେଶ_buf(TRUE);
    ଯଦି (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    ଯଦି (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)maxlen;
}

    ଶୂନ୍ୟ
fill_ନିବେଶ_buf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା exit_on_error UNUSED)
{
# ଯଦି defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		try;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_read_something = FALSE;
    ସ୍ଥିର char_u *rest = ଖାଲି;	    // unconverted rest of previous read
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	restlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unconverted;
# endif

# ifdef FEAT_GUI
    ଯଦି (gui.in_use
#  ifdef NO_CONSOLE_ନିବେଶ
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_ନିବେଶ()
#  endif
       )
    {
	gui_mch_update();
	ଫେରନ୍ତୁ;
    }
# endif
# ଯଦି defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ଯଦି (vim_is_ନିବେଶ_buf_full())
	ଫେରନ୍ତୁ;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    ଯଦି (rest != ଖାଲି)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	ଯଦି (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	ଅନ୍ୟଥା
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	ଯଦି (unconverted == restlen)
	    VIM_CLEAR(rest);
	ଅନ୍ୟଥା
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    ଅନ୍ୟଥା
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    ପାଇଁ (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / ନିବେଶ_conv.vc_factor);
#  ifdef VMS
	len = vms_read((ଅକ୍ଷର *)inbuf + inbufcount, readlen);
#  ଅନ୍ୟଥା
	len = ପଠ(read_cmd_fd, (ଅକ୍ଷର *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	ଯଦି (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(ଖାଲି, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	ଯଦି (len > 0 || got_int)
	    ଭାଙ୍ଗ;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	ଯଦି (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  ଅନ୍ୟଥା
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	ଯଦି (!exit_on_error)
	    ଫେରନ୍ତୁ;
    }
    ଯଦି (len <= 0 && !got_int)
	read_error_exit();
    ଯଦି (len > 0)
	did_read_something = TRUE;
    ଯଦି (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    ଅନ୍ୟଥା
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	ଯଦି (ନିବେଶ_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_ନିବେଶ_safe(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == ଖାଲି ? &rest : ଖାଲି, &restlen);
	}
	ଯେତେବେଳେ (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    ଯଦି (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !ଚାବି_protocol_enabled())
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
    ଶୂନ୍ୟ
read_error_exit(ଶୂନ୍ୟ)
{
    ଯଦି (silent_mode)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#ଯଦି defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    ଶୂନ୍ୟ
ui_cursor_shape_forced(ପୂର୍ଣ୍ଣସଂଖ୍ୟା forced)
{
# ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	gui_update_cursor_later();
    ଅନ୍ୟଥା
# endif
	term_cursor_mode(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_ଧାଡ଼ି(FALSE);
# endif
}

    ଶୂନ୍ୟ
ui_cursor_shape(ଶୂନ୍ୟ)
{
    ui_cursor_shape_forced(FALSE);
}
#endif

/*
 * Check bounds for column number
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_col(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଯଦି (col < 0)
	ଫେରନ୍ତୁ 0;
    ଯଦି (col >= screen_Columns)
	ଫେରନ୍ତୁ screen_Columns - 1;
    ଫେରନ୍ତୁ col;
}

/*
 * Check bounds for row number
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_row(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row)
{
    ଯଦି (row < 0)
	ଫେରନ୍ତୁ 0;
    ଯଦି (row >= screen_Rows)
	ଫେରନ୍ତୁ screen_Rows - 1;
    ଫେରନ୍ତୁ row;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    long
scroll_ଧାଡ଼ି_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	col = 0;

    ଯଦି (*p != NUL)
	ପାଇଁ (;;)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    w = chartabsize(p, col);
	    MB_PTR_ADV(p);
	    ଯଦି (*p == NUL)		// don't count the last character
		ଭାଙ୍ଗ;
	    col += w;
	}
    ଫେରନ୍ତୁ col;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_find_longest_lnum(ଶୂନ୍ୟ)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    ଯଦି (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == ଖାଲି) &&
#endif
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
    {
	linenr_T    lnum;
	long	    n;
	long	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	ପାଇଁ (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_ଧାଡ଼ି_len(lnum);
	    ଯଦି (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    ଅନ୍ୟଥା ଯଦି (n == max && abs((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(lnum - curwin->w_cursor.lnum))
				     < abs((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    ଅନ୍ୟଥା
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    ଫେରନ୍ତୁ ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    ଶୂନ୍ୟ
ui_focus_change(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_focus)	// TRUE if focus gained.
{
    ସ୍ଥିର time_t	last_time = (time_t)0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    ଯଦି (in_focus && last_time + 2 < time(ଖାଲି))
    {
	need_redraw = check_timestamps(
#ifdef FEAT_GUI
		gui.in_use
#ଅନ୍ୟଥା
		FALSE
#endif
		);
	last_time = time(ଖାଲି);
    }

#ifdef FEAT_TERMINAL
    term_focus_change(in_focus);
#endif

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(in_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, ଖାଲି, ଖାଲି, FALSE, curbuf);

    ଯଦି (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    ଯଦି (need_maketitle)
	maketitle();
}

#ଯଦି defined(HAVE_ନିବେଶ_METHOD)
/*
 * Save current Input Method status to specified place.
 */
    ଶୂନ୍ୟ
im_save_status(long *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    ଯଦି (!p_imdisable && ଚାବି_Typed && !ଚାବି_Stuffed
# ifdef FEAT_XIM
	    && xic != ଖାଲି
# endif
# ifdef FEAT_GUI
	    && (!gui.in_use || gui.in_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	ଯଦି (vgetc_im_active)
	    *psave = B_IMODE_IM;
	ଅନ୍ୟଥା ଯଦି (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#endif
