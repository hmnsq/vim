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

#ಅನ್ತರ್ಭಾವ "vim.h"

    ಶೂನ್ಯ
ui_write(char_u *s, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ console UNUSED)
{
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	ಯದಿ (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
#ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    ಯದಿ (!(silent_mode && p_verbose == 0))
    {
# ಯದಿ !defined(MSWIN)
	char_u	*tofree = ರಿಕ್ತ;

	ಯದಿ (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    ಯದಿ (tofree != ರಿಕ್ತ)
		s = tofree;
	}
# endif

	mch_write(s, len);
# ಯದಿ defined(HAVE_FSYNC)
	ಯದಿ (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# ಯದಿ !defined(MSWIN)
	ಯದಿ (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#endif
}

#ಯದಿ defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
ಸ್ಥಿರ char_u *ta_str = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ta_off;	// offset for next char to use when ta_str != NULL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ta_len;	// length of ta_str when it's not NULL

    ಶೂನ್ಯ
ui_inchar_undo(char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u  *new;
    ಪೂರ್ಣಾಙ್ಕ	    newlen;

    newlen = len;
    ಯದಿ (ta_str != ರಿಕ್ತ)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    ಯದಿ (new == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (ta_str != ರಿಕ್ತ)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ
ui_inchar(
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    ಪೂರ್ಣಾಙ್ಕ		tb_change_cnt)
{
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;

#ಯದಿ defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    ಯದಿ (ta_str != ರಿಕ್ತ)
    {
	ಯದಿ (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    ಪ್ರತ್ಯಾಗಮ ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	ಪ್ರತ್ಯಾಗಮ maxlen;
    }
#endif

#ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#endif

#ifdef NO_CONSOLE_ಒಳಹರಿವು
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    ಯದಿ (no_console_ಒಳಹರಿವು())
    {
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	ಯದಿ (retval > 0 || typebuf_changed(tb_change_cnt) || wtime >= 0)
	    ಗಚ್ಛ theend;
# endif
	ಯದಿ (wtime == -1 && ++ಎಣಿಕೆ == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	ಗಚ್ಛ theend;
    }
#endif

    // If we are going to wait for some time or block...
    ಯದಿ (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(ಶೂನ್ಯ)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	ಯದಿ ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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
    ಯದಿ (gui.in_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif
#ifndef NO_CONSOLE
# ifdef FEAT_GUI
    ಅನ್ಯಥಾ
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif

    ಯದಿ (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(ಶೂನ್ಯ)vim_handle_signal(SIGNAL_BLOCK);

    ctrl_c_interrupts = TRUE;

#ifdef NO_CONSOLE_ಒಳಹರಿವು
theend:
#endif
#ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#endif
    ಪ್ರತ್ಯಾಗಮ retval;
}

#ಯದಿ defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    ಪೂರ್ಣಾಙ್ಕ
inchar_loop(
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    ಪೂರ್ಣಾಙ್ಕ		tb_change_cnt,
    ಪೂರ್ಣಾಙ್ಕ		(*wait_func)(long wtime, ಪೂರ್ಣಾಙ್ಕ *interrupted, ಪೂರ್ಣಾಙ್ಕ ignore_ಒಳಹರಿವು),
    ಪೂರ್ಣಾಙ್ಕ		(*resize_func)(ಪೂರ್ಣಾಙ್ಕ check_only))
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		interrupted = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		did_call_wait_func = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		did_start_blocking = FALSE;
    long	wait_time;
    long	elapsed_time = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
# endif

    // repeat until we got a character or waited long enough
    ಪ್ರತಿ (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	ಯದಿ (resize_func != ರಿಕ್ತ)
	    resize_func(FALSE);

# ifdef MESSAGE_QUEUE
	// Only process messages when waiting.
	ಯದಿ (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    ಯದಿ (typebuf_changed(tb_change_cnt))
		ಪ್ರತ್ಯಾಗಮ 0;
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
		&& (get_real_state() & MODE_ಸೇರಿಸು) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	long acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	ಯದಿ (wtime < 0 && did_start_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_time = -1;
	ಅನ್ಯಥಾ
	{
# ifdef ELAPSED_FUNC
	    elapsed_time = ELAPSED_FUNC(start_tv);
# endif
	    ಯದಿ (wtime >= 0)
		wait_time = wtime - elapsed_time;
	    ಅನ್ಯಥಾ ಯದಿ (delay_pending)
	    {
		long delay_ಎಡ = p_acl - acl_elapsed;
		long ut_ಎಡ = p_ut - elapsed_time;

		ಯದಿ (did_start_blocking)
		    wait_time = delay_ಎಡ;
		ಅನ್ಯಥಾ
		    wait_time = MIN(delay_ಎಡ, ut_ಎಡ);
	    }
	    ಅನ್ಯಥಾ
		// going to block after p_ut
		wait_time = p_ut - elapsed_time;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    ಯದಿ (wait_time <= 0 && did_call_wait_func)
	    {
		ಯದಿ (wtime >= 0)
		    // no character available within "wtime"
		    ಪ್ರತ್ಯಾಗಮ 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		ಯದಿ (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    ಯದಿ (buf == ರಿಕ್ತ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_COMPLETE_DELAY;
			add_to_ಒಳಹರಿವು_buf(ibuf, 3);
		    }
		    ಅನ್ಯಥಾ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_COMPLETE_DELAY;
		    }
		    ಪ್ರತ್ಯಾಗಮ 3;
		}

		// No character available within 'updatetime'.
		did_start_blocking = TRUE;
		ಯದಿ (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    ಯದಿ (buf == ರಿಕ್ತ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_CURSORHOLD;
			add_to_ಒಳಹರಿವು_buf(ibuf, 3);
		    }
		    ಅನ್ಯಥಾ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_CURSORHOLD;
		    }
		    ಪ್ರತ್ಯಾಗಮ 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		ಅನುವರ್ತ;
	    }
	}

# ifdef FEAT_JOB_CHANNEL
	ಯದಿ (wait_time < 0 || wait_time > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    ಯದಿ (has_pending_job())
		wait_time = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    ಯದಿ (channel_any_readahead())
		wait_time = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	ಯದಿ (p_beval && wait_time > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_time = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	ಯದಿ (wait_func(wait_time, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    ಯದಿ (typebuf_changed(tb_change_cnt))
		ಪ್ರತ್ಯಾಗಮ 0;

	    // We might have something to return now.
	    ಯದಿ (buf == ರಿಕ್ತ)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		ಪ್ರತ್ಯಾಗಮ ಒಳಹರಿವು_available();

	    len = read_from_ಒಳಹರಿವು_buf(buf, (long)maxlen);
	    ಯದಿ (len > 0)
		ಪ್ರತ್ಯಾಗಮ len;
	    ಅನುವರ್ತ;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_time += wait_time;
# endif

	ಯದಿ ((resize_func != ರಿಕ್ತ && resize_func(TRUE))
# ಯದಿ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
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
	    ಅನುವರ್ತ;

	// no character available or interrupted, return zero
	ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

#ಯದಿ defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    ಪೂರ್ಣಾಙ್ಕ
ui_wait_for_chars_or_timer(
    long    wtime,
    ಪೂರ್ಣಾಙ್ಕ	    (*wait_func)(long wtime, ಪೂರ್ಣಾಙ್ಕ *interrupted, ಪೂರ್ಣಾಙ್ಕ ignore_ಒಳಹರಿವು),
    ಪೂರ್ಣಾಙ್ಕ	    *interrupted,
    ಪೂರ್ಣಾಙ್ಕ	    ignore_ಒಳಹರಿವು)
{
    ಪೂರ್ಣಾಙ್ಕ	    due_time;
    long    remaining = wtime;
    ಪೂರ್ಣಾಙ್ಕ	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_JOB_CHANNEL
    ಪೂರ್ಣಾಙ್ಕ	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    ಯದಿ (wtime >= 0 && wtime < 10L)
	ಪ್ರತ್ಯಾಗಮ wait_func(wtime, ರಿಕ್ತ, ignore_ಒಳಹರಿವು);

    ಯಾವತ್ (wtime < 0 || remaining > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_time = check_due_timer();
	ಯದಿ (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (due_time <= 0 || (wtime > 0 && due_time > remaining))
	    due_time = remaining;
# ಯದಿ defined(FEAT_JOB_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	ಯದಿ ((due_time < 0 || due_time > 10L) && (
#  ಯದಿ defined(FEAT_JOB_CHANNEL)
		(
#   ಯದಿ defined(FEAT_GUI)
		!gui.in_use &&
#   endif
		(has_pending_job() || channel_any_readahead()))
#   ಯದಿ defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  ಯದಿ defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
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
	ಯದಿ (wait_func(due_time, interrupted, ignore_ಒಳಹರಿವು))
	    ಪ್ರತ್ಯಾಗಮ OK;
	ಯದಿ ((interrupted != ರಿಕ್ತ && *interrupted)
# ifdef FEAT_JOB_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (wtime > 0)
	    remaining -= due_time;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}
#endif

/*
 * Return non-zero if a character is available.
 */
    ಪೂರ್ಣಾಙ್ಕ
ui_char_avail(ಶೂನ್ಯ)
{
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	gui_mch_update();
	ಪ್ರತ್ಯಾಗಮ ಒಳಹರಿವು_available();
    }
#endif
#ifndef NO_CONSOLE
# ifdef NO_CONSOLE_ಒಳಹರಿವು
    ಯದಿ (no_console_ಒಳಹರಿವು())
	ಪ್ರತ್ಯಾಗಮ 0;
# endif
    ಪ್ರತ್ಯಾಗಮ mch_char_avail();
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    ಶೂನ್ಯ
ui_delay(long msec_arg, ಪೂರ್ಣಾಙ್ಕ ignoreinput)
{
    long msec = msec_arg;

#ifdef FEAT_EVAL
    ಯದಿ (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(ರಿಕ್ತ, "ui_delay(%ld)", msec);
#endif
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    ಅನ್ಯಥಾ
#endif
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    ಶೂನ್ಯ
ui_suspend(ಶೂನ್ಯ)
{
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	gui_mch_iconify();
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    mch_suspend();
}

#ಯದಿ !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    ಶೂನ್ಯ
suspend_shell(ಶೂನ್ಯ)
{
    ಯದಿ (*p_sh == NUL)
	emsg(_(e_shell_option_is_empty));
    ಅನ್ಯಥಾ
    {
	ಸಂದೇಶ_puts(_("new shell started\n"));
	do_shell(ರಿಕ್ತ, 0);
    }
}
#endif

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
ui_get_shellsize(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    retval;

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	retval = gui_get_shellsize();
    ಅನ್ಯಥಾ
#endif
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    ಯದಿ (retval == OK)
    {
	set_ಸಂಖ್ಯೆ_default("lines", Rows);
	set_ಸಂಖ್ಯೆ_default("columns", Columns);
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    ಶೂನ್ಯ
ui_set_shellsize(
    ಪೂರ್ಣಾಙ್ಕ		mustset UNUSED)	// set by the user
{
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	gui_set_shellsize(mustset, TRUE, RESIZE_BOTH);
    ಅನ್ಯಥಾ
#endif
	mch_set_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    ಶೂನ್ಯ
ui_new_shellsize(ಶೂನ್ಯ)
{
    ಯದಿ (full_screen && !exiting)
    {
#ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	    gui_new_shellsize();
	ಅನ್ಯಥಾ
#endif
	    mch_new_shellsize();
    }
}

#ಯದಿ (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    ಪೂರ್ಣಾಙ್ಕ
ui_get_winpos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y, varnumber_T timeout UNUSED)
{
# ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ gui_mch_get_winpos(x, y);
# endif
# ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    ಪ್ರತ್ಯಾಗಮ mch_get_winpos(x, y);
# ಅನ್ಯಥಾ
#  ಯದಿ defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    ಪ್ರತ್ಯಾಗಮ term_get_winpos(x, y, timeout);
#  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FAIL;
#  endif
# endif
}
#endif

    ಶೂನ್ಯ
ui_breakcheck(ಶೂನ್ಯ)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    ಶೂನ್ಯ
ui_breakcheck_force(ಪೂರ್ಣಾಙ್ಕ force)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	gui_mch_update();
    ಅನ್ಯಥಾ
#endif
	mch_breakcheck(force);

    ಯದಿ (save_updating_screen)
	updating_screen = TRUE;
    ಅನ್ಯಥಾ
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

#ಯದಿ defined(USE_ಒಳಹರಿವು_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# ಯದಿ defined(FEAT_JOB_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# ಅನ್ಯಥಾ
#  define INBUFLEN 250
# endif

ಸ್ಥಿರ char_u	inbuf[INBUFLEN + MAX_ಕೀಲಿ_CODE_LEN];
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    ಪೂರ್ಣಾಙ್ಕ
vim_is_ಒಳಹರಿವು_buf_full(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (inbufcount >= INBUFLEN);
}

    ಪೂರ್ಣಾಙ್ಕ
vim_is_ಒಳಹರಿವು_buf_empty(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (inbufcount == 0);
}

# ಯದಿ defined(FEAT_OLE)
    ಪೂರ್ಣಾಙ್ಕ
vim_free_in_ಒಳಹರಿವು_buf(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (INBUFLEN - inbufcount);
}
# endif

# ಯದಿ defined(FEAT_GUI_GTK)
    ಪೂರ್ಣಾಙ್ಕ
vim_used_in_ಒಳಹರಿವು_buf(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_ಒಳಹರಿವು_buf(ಶೂನ್ಯ)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    ಯದಿ (gap != ರಿಕ್ತ)
    {
	// Add one to avoid a zero size.
	gap->ga_data = alloc(inbufcount + 1);
	ಯದಿ (gap->ga_data != ರಿಕ್ತ)
	    mch_memmove(gap->ga_data, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_ಒಳಹರಿವು_buf();
    ಪ್ರತ್ಯಾಗಮ (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    ಶೂನ್ಯ
set_ಒಳಹರಿವು_buf(char_u *p, ಪೂರ್ಣಾಙ್ಕ overwrite)
{
    garray_T	*gap = (garray_T *)p;

    ಯದಿ (gap == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (gap->ga_data != ರಿಕ್ತ)
    {
	ಯದಿ (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_data, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
add_to_ಒಳಹರಿವು_buf(char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಯದಿ (inbufcount + len > INBUFLEN + MAX_ಕೀಲಿ_CODE_LEN)
	ಪ್ರತ್ಯಾಗಮ;	    // Shouldn't ever happen!

    ಯಾವತ್ (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    ಶೂನ್ಯ
add_to_ಒಳಹರಿವು_buf_csi(char_u *str, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	buf[2];

    ಪ್ರತಿ (i = 0; i < len; ++i)
    {
	add_to_ಒಳಹರಿವು_buf(str + i, 1);
	ಯದಿ (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
	    add_to_ಒಳಹರಿವು_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    ಶೂನ್ಯ
trash_ಒಳಹರಿವು_buf(ಶೂನ್ಯ)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    ಪೂರ್ಣಾಙ್ಕ
read_from_ಒಳಹರಿವು_buf(char_u *buf, long maxlen)
{
    ಯದಿ (inbufcount == 0)	// if the buffer is empty, fill it
	fill_ಒಳಹರಿವು_buf(TRUE);
    ಯದಿ (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    ಯದಿ (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)maxlen;
}

    ಶೂನ್ಯ
fill_ಒಳಹರಿವು_buf(ಪೂರ್ಣಾಙ್ಕ exit_on_error UNUSED)
{
# ಯದಿ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		try;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_read_something = FALSE;
    ಸ್ಥಿರ char_u *rest = ರಿಕ್ತ;	    // unconverted rest of previous read
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	restlen = 0;
    ಪೂರ್ಣಾಙ್ಕ		unconverted;
# endif

# ifdef FEAT_GUI
    ಯದಿ (gui.in_use
#  ifdef NO_CONSOLE_ಒಳಹರಿವು
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_ಒಳಹರಿವು()
#  endif
       )
    {
	gui_mch_update();
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif
# ಯದಿ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ಯದಿ (vim_is_ಒಳಹರಿವು_buf_full())
	ಪ್ರತ್ಯಾಗಮ;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    ಯದಿ (rest != ರಿಕ್ತ)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	ಯದಿ (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	ಅನ್ಯಥಾ
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	ಯದಿ (unconverted == restlen)
	    VIM_CLEAR(rest);
	ಅನ್ಯಥಾ
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    ಅನ್ಯಥಾ
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    ಪ್ರತಿ (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / ಒಳಹರಿವು_conv.vc_factor);
#  ifdef VMS
	len = vms_read((ವರ್ಣ *)inbuf + inbufcount, readlen);
#  ಅನ್ಯಥಾ
	len = ಪಠ(read_cmd_fd, (ವರ್ಣ *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	ಯದಿ (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(ರಿಕ್ತ, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	ಯದಿ (len > 0 || got_int)
	    ಭಙ್ಗ;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	ಯದಿ (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  ಅನ್ಯಥಾ
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	ಯದಿ (!exit_on_error)
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (len <= 0 && !got_int)
	read_error_exit();
    ಯದಿ (len > 0)
	did_read_something = TRUE;
    ಯದಿ (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	ಯದಿ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_ಒಳಹರಿವು_safe(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == ರಿಕ್ತ ? &rest : ರಿಕ್ತ, &restlen);
	}
	ಯಾವತ್ (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    ಯದಿ (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !ಕೀಲಿ_protocol_enabled())
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
    ಶೂನ್ಯ
read_error_exit(ಶೂನ್ಯ)
{
    ಯದಿ (silent_mode)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#ಯದಿ defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    ಶೂನ್ಯ
ui_cursor_shape_forced(ಪೂರ್ಣಾಙ್ಕ forced)
{
# ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	gui_update_cursor_later();
    ಅನ್ಯಥಾ
# endif
	term_cursor_mode(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_ಸಾಲು(FALSE);
# endif
}

    ಶೂನ್ಯ
ui_cursor_shape(ಶೂನ್ಯ)
{
    ui_cursor_shape_forced(FALSE);
}
#endif

/*
 * Check bounds for column number
 */
    ಪೂರ್ಣಾಙ್ಕ
check_col(ಪೂರ್ಣಾಙ್ಕ col)
{
    ಯದಿ (col < 0)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (col >= screen_Columns)
	ಪ್ರತ್ಯಾಗಮ screen_Columns - 1;
    ಪ್ರತ್ಯಾಗಮ col;
}

/*
 * Check bounds for row number
 */
    ಪೂರ್ಣಾಙ್ಕ
check_row(ಪೂರ್ಣಾಙ್ಕ row)
{
    ಯದಿ (row < 0)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (row >= screen_Rows)
	ಪ್ರತ್ಯಾಗಮ screen_Rows - 1;
    ಪ್ರತ್ಯಾಗಮ row;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    long
scroll_ಸಾಲು_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	col = 0;

    ಯದಿ (*p != NUL)
	ಪ್ರತಿ (;;)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    w = chartabsize(p, col);
	    MB_PTR_ADV(p);
	    ಯದಿ (*p == NUL)		// don't count the last character
		ಭಙ್ಗ;
	    col += w;
	}
    ಪ್ರತ್ಯಾಗಮ col;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_find_longest_lnum(ಶೂನ್ಯ)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    ಯದಿ (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == ರಿಕ್ತ) &&
#endif
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
    {
	linenr_T    lnum;
	long	    n;
	long	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	ಪ್ರತಿ (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_ಸಾಲು_len(lnum);
	    ಯದಿ (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (n == max && abs((ಪೂರ್ಣಾಙ್ಕ)(lnum - curwin->w_cursor.lnum))
				     < abs((ಪೂರ್ಣಾಙ್ಕ)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    ಅನ್ಯಥಾ
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    ಶೂನ್ಯ
ui_focus_change(
    ಪೂರ್ಣಾಙ್ಕ		in_focus)	// TRUE if focus gained.
{
    ಸ್ಥಿರ time_t	last_time = (time_t)0;
    ಪೂರ್ಣಾಙ್ಕ			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    ಯದಿ (in_focus && last_time + 2 < time(ರಿಕ್ತ))
    {
	need_redraw = check_timestamps(
#ifdef FEAT_GUI
		gui.in_use
#ಅನ್ಯಥಾ
		FALSE
#endif
		);
	last_time = time(ರಿಕ್ತ);
    }

#ifdef FEAT_TERMINAL
    term_focus_change(in_focus);
#endif

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(in_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);

    ಯದಿ (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    ಯದಿ (need_maketitle)
	maketitle();
}

#ಯದಿ defined(HAVE_ಒಳಹರಿವು_METHOD)
/*
 * Save current Input Method status to specified place.
 */
    ಶೂನ್ಯ
im_save_status(long *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    ಯದಿ (!p_imdisable && ಕೀಲಿ_Typed && !ಕೀಲಿ_Stuffed
# ifdef FEAT_XIM
	    && xic != ರಿಕ್ತ
# endif
# ifdef FEAT_GUI
	    && (!gui.in_use || gui.in_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	ಯದಿ (vgetc_im_active)
	    *psave = B_IMODE_IM;
	ಅನ್ಯಥಾ ಯದಿ (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#endif
