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
ui_write(char_u *s, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ console ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	ಆದರೆ (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    ಆದರೆ (!(silent_ಬಹುಲಕ && p_verbose == 0))
    {
# ಆದರೆ !defined(MSWIN)
	char_u	*tofree = ಖಾಲಿ;

	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    ಆದರೆ (tofree != ಖಾಲಿ)
		s = tofree;
	}
# endif

	mch_write(s, len);
# ಆದರೆ defined(HAVE_FSYNC)
	ಆದರೆ (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# ಆದರೆ !defined(MSWIN)
	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
ಸ್ಥಿರ char_u *ta_str = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ta_off;	// offset for next char to use when ta_str != NULL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ta_len;	// length of ta_str when it's not NULL

    ಶೂನ್ಯ
ui_inchar_undo(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
    char_u  *new;
    ಪೂರ್ಣಾಂಕ	    newlen;

    newlen = len;
    ಆದರೆ (ta_str != ಖಾಲಿ)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    ಆದರೆ (new == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ta_str != ಖಾಲಿ)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	mch_memmove(new, s, (size_t)len);
    ta_str = new;
    ta_len = newlen;
    ta_off = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
ui_inchar(
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		maxlen,
    ದೀರ್ಘ	wtime,	    // don't use "time", MIPS cannot handle it
    ಪೂರ್ಣಾಂಕ		tb_change_cnt)
{
    ಪೂರ್ಣಾಂಕ		retval = 0;

#ಆದರೆ defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    ಆದರೆ (ta_str != ಖಾಲಿ)
    {
	ಆದರೆ (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    ಹಿಂತಿರುಗಿ ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	ಹಿಂತಿರುಗಿ maxlen;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NO_CONSOLE_ಒಳಹರಿವು
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    ಆದರೆ (no_console_ಒಳಹರಿವು())
    {
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	ಆದರೆ (retval > 0 || typebuf_ಬದಲಾಯಿತು(tb_change_cnt) || wtime >= 0)
	    ಹೋಗು theend;
# endif
	ಆದರೆ (wtime == -1 && ++ಎಣಿಕೆ_2 == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	ಹೋಗು theend;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // If we are going to wait for some time or block...
    ಆದರೆ (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(ಶೂನ್ಯ)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	ಆದರೆ ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NO_CONSOLE
# ifdef FEAT_GUI
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(ಶೂನ್ಯ)vim_handle_signal(SIGNAL_ಖಂಡ);

    ctrl_c_interrupts = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NO_CONSOLE_ಒಳಹರಿವು
theend:
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ retval;
}

#ಆದರೆ defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    ಪೂರ್ಣಾಂಕ
inchar_loop(
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		maxlen,
    ದೀರ್ಘ	wtime,	    // don't use "time", MIPS cannot handle it
    ಪೂರ್ಣಾಂಕ		tb_change_cnt,
    ಪೂರ್ಣಾಂಕ		(*wait_func)(ದೀರ್ಘ wtime, ಪೂರ್ಣಾಂಕ *interrupted, ಪೂರ್ಣಾಂಕ ignore_ಒಳಹರಿವು),
    ಪೂರ್ಣಾಂಕ		(*resize_func)(ಪೂರ್ಣಾಂಕ check_only))
{
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		interrupted = FALSE;
    ಪೂರ್ಣಾಂಕ		did_call_wait_func = FALSE;
    ಪೂರ್ಣಾಂಕ		did_ಆರಂಭ_blocking = FALSE;
    ದೀರ್ಘ	wait_ಸಮಯ;
    ದೀರ್ಘ	elapsed_ಸಮಯ = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	ಆರಂಭ_tv;

    ELAPSED_INIT(ಆರಂಭ_tv);
# endif

    // repeat until we got a character or waited long enough
    ಗಾಗಿ (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	ಆದರೆ (resize_func != ಖಾಲಿ)
	    resize_func(FALSE);

# ifdef ಸಂದೇಶ_ಸರತಿ
	// Only process messages when waiting.
	ಆದರೆ (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    ಆದರೆ (typebuf_ಬದಲಾಯಿತು(tb_change_cnt))
		ಹಿಂತಿರುಗಿ 0;
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
		&& !ins_compl_ಸಕ್ರಿಯ()
		&& (get_real_state() & ಬಹುಲಕ_ಸೇರಿಸು) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	ದೀರ್ಘ acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	ಆದರೆ (wtime < 0 && did_ಆರಂಭ_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_ಸಮಯ = -1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
# ifdef ELAPSED_FUNC
	    elapsed_ಸಮಯ = ELAPSED_FUNC(ಆರಂಭ_tv);
# endif
	    ಆದರೆ (wtime >= 0)
		wait_ಸಮಯ = wtime - elapsed_ಸಮಯ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (delay_pending)
	    {
		ದೀರ್ಘ delay_ಎಡ = p_acl - acl_elapsed;
		ದೀರ್ಘ ut_ಎಡ = p_ut - elapsed_ಸಮಯ;

		ಆದರೆ (did_ಆರಂಭ_blocking)
		    wait_ಸಮಯ = delay_ಎಡ;
		ಇಲ್ಲದಿದ್ದರೆ
		    wait_ಸಮಯ = MIN(delay_ಎಡ, ut_ಎಡ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// going to block after p_ut
		wait_ಸಮಯ = p_ut - elapsed_ಸಮಯ;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    ಆದರೆ (wait_ಸಮಯ <= 0 && did_call_wait_func)
	    {
		ಆದರೆ (wtime >= 0)
		    // no character available within "wtime"
		    ಹಿಂತಿರುಗಿ 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		ಆದರೆ (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_ಬದಲಾಯಿತು(tb_change_cnt))
		{
		    ಆದರೆ (buf == ಖಾಲಿ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (ಪೂರ್ಣಾಂಕ)KE_COMPLETE_DELAY;
			ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(ibuf, 3);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (ಪೂರ್ಣಾಂಕ)KE_COMPLETE_DELAY;
		    }
		    ಹಿಂತಿರುಗಿ 3;
		}

		// No character available within 'updatetime'.
		did_ಆರಂಭ_blocking = TRUE;
		ಆದರೆ (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_ಬದಲಾಯಿತು(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    ಆದರೆ (buf == ಖಾಲಿ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (ಪೂರ್ಣಾಂಕ)KE_CURSORHOLD;
			ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(ibuf, 3);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (ಪೂರ್ಣಾಂಕ)KE_CURSORHOLD;
		    }
		    ಹಿಂತಿರುಗಿ 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		ಮುಂದುವರಿ;
	    }
	}

# ifdef FEAT_ಕೆಲಸ_CHANNEL
	ಆದರೆ (wait_ಸಮಯ < 0 || wait_ಸಮಯ > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    ಆದರೆ (has_pending_ಕೆಲಸ())
		wait_ಸಮಯ = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    ಆದರೆ (channel_any_readahead())
		wait_ಸಮಯ = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	ಆದರೆ (p_beval && wait_ಸಮಯ > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_ಸಮಯ = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	ಆದರೆ (wait_func(wait_ಸಮಯ, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    ಆದರೆ (typebuf_ಬದಲಾಯಿತು(tb_change_cnt))
		ಹಿಂತಿರುಗಿ 0;

	    // We might have something to return now.
	    ಆದರೆ (buf == ಖಾಲಿ)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		ಹಿಂತಿರುಗಿ ಒಳಹರಿವು_ಲಭ್ಯ();

	    len = read_from_ಒಳಹರಿವು_buf(buf, (ದೀರ್ಘ)maxlen);
	    ಆದರೆ (len > 0)
		ಹಿಂತಿರುಗಿ len;
	    ಮುಂದುವರಿ;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_ಸಮಯ += wait_ಸಮಯ;
# endif

	ಆದರೆ ((resize_func != ಖಾಲಿ && resize_func(TRUE))
# ಆದರೆ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
		||
		(clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11 &&
		 server_ಕಾಯುವಿಕೆ())
# endif
# ifdef ಸಂದೇಶ_ಸರತಿ
		|| interrupted
# endif
		|| wait_ಸಮಯ > 0
		|| (wtime < 0 && !did_ಆರಂಭ_blocking))
	    // no character available, but something to be done, keep going
	    ಮುಂದುವರಿ;

	// no character available or interrupted, return zero
	ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    ಪೂರ್ಣಾಂಕ
ui_wait_for_chars_or_timer(
    ದೀರ್ಘ    wtime,
    ಪೂರ್ಣಾಂಕ	    (*wait_func)(ದೀರ್ಘ wtime, ಪೂರ್ಣಾಂಕ *interrupted, ಪೂರ್ಣಾಂಕ ignore_ಒಳಹರಿವು),
    ಪೂರ್ಣಾಂಕ	    *interrupted,
    ಪೂರ್ಣಾಂಕ	    ignore_ಒಳಹರಿವು)
{
    ಪೂರ್ಣಾಂಕ	    due_ಸಮಯ;
    ದೀರ್ಘ    ಉಳಿದ = wtime;
    ಪೂರ್ಣಾಂಕ	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_ಕೆಲಸ_CHANNEL
    ಪೂರ್ಣಾಂಕ	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    ಆದರೆ (wtime >= 0 && wtime < 10L)
	ಹಿಂತಿರುಗಿ wait_func(wtime, ಖಾಲಿ, ignore_ಒಳಹರಿವು);

    ಆಗ (wtime < 0 || ಉಳಿದ > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_ಸಮಯ = check_due_timer();
	ಆದರೆ (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (due_ಸಮಯ <= 0 || (wtime > 0 && due_ಸಮಯ > ಉಳಿದ))
	    due_ಸಮಯ = ಉಳಿದ;
# ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	ಆದರೆ ((due_ಸಮಯ < 0 || due_ಸಮಯ > 10L) && (
#  ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
		(
#   ಆದರೆ defined(FEAT_GUI)
		!gui.ಒಳ_use &&
#   endif
		(has_pending_ಕೆಲಸ() || channel_any_readahead()))
#   ಆದರೆ defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  ಆದರೆ defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
		    has_any_sound_callback()
#  endif
		    ))
	{
	    // There is a pending job or channel, should return soon in order
	    // to handle them ASAP.  Do check for input briefly.
	    due_ಸಮಯ = 10L;
#  ifdef FEAT_ಕೆಲಸ_CHANNEL
	    brief_wait = TRUE;
#  endif
	}
# endif
	ಆದರೆ (wait_func(due_ಸಮಯ, interrupted, ignore_ಒಳಹರಿವು))
	    ಹಿಂತಿರುಗಿ OK;
	ಆದರೆ ((interrupted != ಖಾಲಿ && *interrupted)
# ifdef FEAT_ಕೆಲಸ_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (wtime > 0)
	    ಉಳಿದ -= due_ಸಮಯ;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return non-zero if a character is available.
 */
    ಪೂರ್ಣಾಂಕ
ui_char_avail(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	gui_mch_update();
	ಹಿಂತಿರುಗಿ ಒಳಹರಿವು_ಲಭ್ಯ();
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NO_CONSOLE
# ifdef NO_CONSOLE_ಒಳಹರಿವು
    ಆದರೆ (no_console_ಒಳಹರಿವು())
	ಹಿಂತಿರುಗಿ 0;
# endif
    ಹಿಂತಿರುಗಿ mch_char_avail();
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    ಶೂನ್ಯ
ui_delay(ದೀರ್ಘ msec_arg, ಪೂರ್ಣಾಂಕ ignoreinput)
{
    ದೀರ್ಘ msec = msec_arg;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(ಖಾಲಿ, "ui_delay(%ld)", msec);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	gui_mch_iconify();
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    mch_suspend();
}

#ಆದರೆ !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    ಶೂನ್ಯ
suspend_shell(ಶೂನ್ಯ)
{
    ಆದರೆ (*p_sh == NUL)
	emsg(_(e_shell_option_is_ಖಾಲಿಗಳು));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಂದೇಶ_puts(_("new shell started\n"));
	do_shell(ಖಾಲಿ, 0);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
ui_get_shellsize(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    retval;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	retval = gui_get_shellsize();
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    ಆದರೆ (retval == OK)
    {
	ಗಣ_ಸಂಖ್ಯೆ_default("lines", ಅಡ್ಡಸಾಲುಗಳು);
	ಗಣ_ಸಂಖ್ಯೆ_default("columns", Columns);
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    ಶೂನ್ಯ
ui_ಗಣ_shellsize(
    ಪೂರ್ಣಾಂಕ		mustset ಬಳಸದ)	// set by the user
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	gui_ಗಣ_shellsize(mustset, TRUE, RESIZE_BOTH);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	mch_ಗಣ_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    ಶೂನ್ಯ
ui_new_shellsize(ಶೂನ್ಯ)
{
    ಆದರೆ (ತುಂಬಿದ_screen && !exiting)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	    gui_new_shellsize();
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    mch_new_shellsize();
    }
}

#ಆದರೆ (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    ಪೂರ್ಣಾಂಕ
ui_get_winpos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y, varnumber_T timeout ಬಳಸದ)
{
# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ gui_mch_get_winpos(x, y);
# endif
# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    ಹಿಂತಿರುಗಿ mch_get_winpos(x, y);
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    ಹಿಂತಿರುಗಿ ಪದ_get_winpos(x, y, timeout);
#  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FAIL;
#  endif
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

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
ui_breakcheck_force(ಪೂರ್ಣಾಂಕ force)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;
    ಪೂರ್ಣಾಂಕ		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	gui_mch_update();
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	mch_breakcheck(force);

    ಆದರೆ (save_updating_screen)
	updating_screen = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
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

#ಆದರೆ defined(USE_ಒಳಹರಿವು_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# ಇಲ್ಲದಿದ್ದರೆ
#  define INBUFLEN 250
# endif

ಸ್ಥಿರ char_u	inbuf[INBUFLEN + MAX_ಕೀಲಿ_ಸಂಕೇತ_LEN];
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    ಪೂರ್ಣಾಂಕ
vim_is_ಒಳಹರಿವು_buf_ತುಂಬಿದ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (inbufcount >= INBUFLEN);
}

    ಪೂರ್ಣಾಂಕ
vim_is_ಒಳಹರಿವು_buf_ಖಾಲಿಗಳು(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (inbufcount == 0);
}

# ಆದರೆ defined(FEAT_OLE)
    ಪೂರ್ಣಾಂಕ
vim_free_ಒಳ_ಒಳಹರಿವು_buf(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (INBUFLEN - inbufcount);
}
# endif

# ಆದರೆ defined(FEAT_GUI_GTK)
    ಪೂರ್ಣಾಂಕ
vim_ಬಳಸಿದ_ಒಳ_ಒಳಹರಿವು_buf(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ inbufcount;
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
    ಆದರೆ (gap != ಖಾಲಿ)
    {
	// Add one to avoid a zero size.
	gap->ga_ದತ್ತಾಂಶ = alloc(inbufcount + 1);
	ಆದರೆ (gap->ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	    mch_memmove(gap->ga_ದತ್ತಾಂಶ, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_ಒಳಹರಿವು_buf();
    ಹಿಂತಿರುಗಿ (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    ಶೂನ್ಯ
ಗಣ_ಒಳಹರಿವು_buf(char_u *p, ಪೂರ್ಣಾಂಕ overwrite)
{
    garray_T	*gap = (garray_T *)p;

    ಆದರೆ (gap == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (gap->ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	ಆದರೆ (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_ದತ್ತಾಂಶ, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    mch_memmove(inbuf + gap->ga_len, inbuf, inbufcount);
	    mch_memmove(inbuf, gap->ga_ದತ್ತಾಂಶ, gap->ga_len);
	    inbufcount += gap->ga_len;
	}
	vim_free(gap->ga_ದತ್ತಾಂಶ);
    }
    vim_free(gap);
}

/*
 * Add the given bytes to the input buffer
 * Special keys start with CSI.  A real CSI must have been translated to
 * CSI KS_EXTRA KE_CSI.  K_SPECIAL doesn't require translation.
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
    ಆದರೆ (inbufcount + len > INBUFLEN + MAX_ಕೀಲಿ_ಸಂಕೇತ_LEN)
	ಹಿಂತಿರುಗಿ;	    // Shouldn't ever happen!

    ಆಗ (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(char_u *str, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	buf[2];

    ಗಾಗಿ (i = 0; i < len; ++i)
    {
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(str + i, 1);
	ಆದರೆ (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (ಪೂರ್ಣಾಂಕ)KE_CSI;
	    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(buf, 2);
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
    ಪೂರ್ಣಾಂಕ
read_from_ಒಳಹರಿವು_buf(char_u *buf, ದೀರ್ಘ maxlen)
{
    ಆದರೆ (inbufcount == 0)	// if the buffer is empty, fill it
	fill_ಒಳಹರಿವು_buf(TRUE);
    ಆದರೆ (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    ಆದರೆ (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)maxlen;
}

    ಶೂನ್ಯ
fill_ಒಳಹರಿವು_buf(ಪೂರ್ಣಾಂಕ exit_on_error ಬಳಸದ)
{
# ಆದರೆ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		try;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_read_something = FALSE;
    ಸ್ಥಿರ char_u *rest = ಖಾಲಿ;	    // unconverted rest of previous read
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	restlen = 0;
    ಪೂರ್ಣಾಂಕ		unconverted;
# endif

# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use
#  ifdef NO_CONSOLE_ಒಳಹರಿವು
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_ಒಳಹರಿವು()
#  endif
       )
    {
	gui_mch_update();
	ಹಿಂತಿರುಗಿ;
    }
# endif
# ಆದರೆ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ಆದರೆ (vim_is_ಒಳಹರಿವು_buf_ತುಂಬಿದ())
	ಹಿಂತಿರುಗಿ;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    ಆದರೆ (rest != ಖಾಲಿ)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	ಆದರೆ (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	ಇಲ್ಲದಿದ್ದರೆ
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	ಆದರೆ (unconverted == restlen)
	    VIM_CLEAR(rest);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    ಗಾಗಿ (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / ಒಳಹರಿವು_conv.vc_ಗುಣಕ);
#  ifdef VMS
	len = vms_read((ಅಕ್ಷರ *)inbuf + inbufcount, readlen);
#  ಇಲ್ಲದಿದ್ದರೆ
	len = ಪಠ(read_cmd_fd, (ಅಕ್ಷರ *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	ಆದರೆ (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(ಖಾಲಿ, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	ಆದರೆ (len > 0 || got_int)
	    ನಿಲ್ಲಿಸು;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	ಆದರೆ (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    ಪೂರ್ಣಾಂಕ m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  ಇಲ್ಲದಿದ್ದರೆ
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	ಆದರೆ (!exit_on_error)
	    ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (len <= 0 && !got_int)
	read_error_exit();
    ಆದರೆ (len > 0)
	did_read_something = TRUE;
    ಆದರೆ (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_ಒಳಹರಿವು_ಸುರಕ್ಷಿತ(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == ಖಾಲಿ ? &rest : ಖಾಲಿ, &restlen);
	}
	ಆಗ (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    ಆದರೆ (ctrl_c_interrupts
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
#ಆದರೆ_ಅಂತ್ಯ // USE_INPUT_BUF

/*
 * Exit because of an input read error.
 */
    ಶೂನ್ಯ
read_error_exit(ಶೂನ್ಯ)
{
    ಆದರೆ (silent_ಬಹುಲಕ)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#ಆದರೆ defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    ಶೂನ್ಯ
ui_cursor_shape_forced(ಪೂರ್ಣಾಂಕ forced)
{
# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	gui_update_cursor_later();
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	ಪದ_cursor_ಬಹುಲಕ(forced);

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
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check bounds for column number
 */
    ಪೂರ್ಣಾಂಕ
check_ಲಂಬಸಾಲು(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (ಲಂಬಸಾಲು < 0)
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (ಲಂಬಸಾಲು >= screen_Columns)
	ಹಿಂತಿರುಗಿ screen_Columns - 1;
    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
}

/*
 * Check bounds for row number
 */
    ಪೂರ್ಣಾಂಕ
check_ಅಡ್ಡಸಾಲು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0)
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು)
	ಹಿಂತಿರುಗಿ screen_ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲು;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    ದೀರ್ಘ
scroll_ಸಾಲು_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	ಲಂಬಸಾಲು = 0;

    ಆದರೆ (*p != NUL)
	ಗಾಗಿ (;;)
	{
	    ಪೂರ್ಣಾಂಕ	    w = chartabsize(p, ಲಂಬಸಾಲು);
	    MB_PTR_ADV(p);
	    ಆದರೆ (*p == NUL)		// don't count the last character
		ನಿಲ್ಲಿಸು;
	    ಲಂಬಸಾಲು += w;
	}
    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_ಕಂಡುಹಿಡಿ_longest_lnum(ಶೂನ್ಯ)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    (!gui.ಒಳ_use || vim_strchr(p_go, GO_HORSCROLL) == ಖಾಲಿ) &&
#ಆದರೆ_ಅಂತ್ಯ
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
    {
	linenr_T    lnum;
	ದೀರ್ಘ	    n;
	ದೀರ್ಘ	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	ಗಾಗಿ (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_ಸಾಲು_len(lnum);
	    ಆದರೆ (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n == max && ನಿರಪೇಕ್ಷ((ಪೂರ್ಣಾಂಕ)(lnum - curwin->w_cursor.lnum))
				     < ನಿರಪೇಕ್ಷ((ಪೂರ್ಣಾಂಕ)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    ಶೂನ್ಯ
ui_focus_change(
    ಪೂರ್ಣಾಂಕ		ಒಳ_focus)	// TRUE if focus gained.
{
    ಸ್ಥಿರ time_t	ಕೊನೆಯ_ಸಮಯ = (time_t)0;
    ಪೂರ್ಣಾಂಕ			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    ಆದರೆ (ಒಳ_focus && ಕೊನೆಯ_ಸಮಯ + 2 < ಸಮಯ(ಖಾಲಿ))
    {
	need_redraw = check_timestamps(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		gui.ಒಳ_use
#ಇಲ್ಲದಿದ್ದರೆ
		FALSE
#ಆದರೆ_ಅಂತ್ಯ
		);
	ಕೊನೆಯ_ಸಮಯ = ಸಮಯ(ಖಾಲಿ);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪದ_focus_change(ಒಳ_focus);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(ಒಳ_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);

    ಆದರೆ (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    ಆದರೆ (need_maketitle)
	maketitle();
}

#ಆದರೆ defined(HAVE_ಒಳಹರಿವು_ವಿಧಾನ)
/*
 * Save current Input Method status to specified place.
 */
    ಶೂನ್ಯ
im_save_ಸ್ಥಿತಿ(ದೀರ್ಘ *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    ಆದರೆ (!p_imdisable && ಕೀಲಿ_Typed && !ಕೀಲಿ_Stuffed
# ifdef FEAT_XIM
	    && xic != ಖಾಲಿ
# endif
# ifdef FEAT_GUI
	    && (!gui.ಒಳ_use || gui.ಒಳ_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	ಆದರೆ (vgetc_im_ಸಕ್ರಿಯ)
	    *psave = B_IMODE_IM;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#ಆದರೆ_ಅಂತ್ಯ
