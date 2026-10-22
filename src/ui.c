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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

    ਖਾਲੀ
ui_write(char_u *s, ਪੂਰਨ len, ਪੂਰਨ console ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	ਜੇ (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    ਜੇ (!(silent_ਬਹੁਲਕ && p_verbose == 0))
    {
# ਜੇ !defined(MSWIN)
	char_u	*tofree = ਨਿਰਾ;

	ਜੇ (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    ਜੇ (tofree != ਨਿਰਾ)
		s = tofree;
	}
# endif

	mch_write(s, len);
# ਜੇ defined(HAVE_FSYNC)
	ਜੇ (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# ਜੇ !defined(MSWIN)
	ਜੇ (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
ਸਥਿਰ char_u *ta_str = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ ta_off;	// offset for next char to use when ta_str != NULL
ਸਥਿਰ ਪੂਰਨ ta_len;	// length of ta_str when it's not NULL

    ਖਾਲੀ
ui_inchar_undo(char_u *s, ਪੂਰਨ len)
{
    char_u  *new;
    ਪੂਰਨ	    newlen;

    newlen = len;
    ਜੇ (ta_str != ਨਿਰਾ)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    ਜੇ (new == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ta_str != ਨਿਰਾ)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    ਨਹੀਂਤਾਂ
	mch_memmove(new, s, (size_t)len);
    ta_str = new;
    ta_len = newlen;
    ta_off = 0;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
ui_inchar(
    char_u	*buf,
    ਪੂਰਨ		maxlen,
    ਲੰਮਾ	wtime,	    // don't use "time", MIPS cannot handle it
    ਪੂਰਨ		tb_change_cnt)
{
    ਪੂਰਨ		retval = 0;

#ਜੇ defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    ਜੇ (ta_str != ਨਿਰਾ)
    {
	ਜੇ (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    ਵਾਪਸ ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	ਵਾਪਸ maxlen;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NO_CONSOLE_ਨਿਵੇਸ਼
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    ਜੇ (no_console_ਨਿਵੇਸ਼())
    {
	ਸਥਿਰ ਪੂਰਨ ਗਿਣਤੀ_2 = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	ਜੇ (retval > 0 || typebuf_ਬਦਲਿਆ(tb_change_cnt) || wtime >= 0)
	    ਜਾਓ theend;
# endif
	ਜੇ (wtime == -1 && ++ਗਿਣਤੀ_2 == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	ਜਾਓ theend;
    }
#ਜੇ_ਅੰਤ

    // If we are going to wait for some time or block...
    ਜੇ (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(ਖਾਲੀ)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	ਜੇ ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NO_CONSOLE
# ifdef FEAT_GUI
    ਨਹੀਂਤਾਂ
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#ਜੇ_ਅੰਤ

    ਜੇ (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(ਖਾਲੀ)vim_handle_signal(SIGNAL_ਖੰਡ);

    ctrl_c_interrupts = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NO_CONSOLE_ਨਿਵੇਸ਼
theend:
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#ਜੇ_ਅੰਤ
    ਵਾਪਸ retval;
}

#ਜੇ defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    ਪੂਰਨ
inchar_loop(
    char_u	*buf,
    ਪੂਰਨ		maxlen,
    ਲੰਮਾ	wtime,	    // don't use "time", MIPS cannot handle it
    ਪੂਰਨ		tb_change_cnt,
    ਪੂਰਨ		(*wait_func)(ਲੰਮਾ wtime, ਪੂਰਨ *interrupted, ਪੂਰਨ ignore_ਨਿਵੇਸ਼),
    ਪੂਰਨ		(*resize_func)(ਪੂਰਨ check_only))
{
    ਪੂਰਨ		len;
    ਪੂਰਨ		interrupted = FALSE;
    ਪੂਰਨ		did_call_wait_func = FALSE;
    ਪੂਰਨ		did_ਸ਼ੁਰੂ_blocking = FALSE;
    ਲੰਮਾ	wait_ਸਮਾਂ;
    ਲੰਮਾ	elapsed_ਸਮਾਂ = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	ਸ਼ੁਰੂ_tv;

    ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
# endif

    // repeat until we got a character or waited long enough
    ਲਈ (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	ਜੇ (resize_func != ਨਿਰਾ)
	    resize_func(FALSE);

# ifdef ਸੁਨੇਹਾ_ਕਤਾਰ
	// Only process messages when waiting.
	ਜੇ (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    ਜੇ (typebuf_ਬਦਲਿਆ(tb_change_cnt))
		ਵਾਪਸ 0;
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
		&& !ins_compl_ਸਰਗਰਮ()
		&& (get_real_state() & ਬਹੁਲਕ_ਪਾਓ) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	ਲੰਮਾ acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	ਜੇ (wtime < 0 && did_ਸ਼ੁਰੂ_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_ਸਮਾਂ = -1;
	ਨਹੀਂਤਾਂ
	{
# ifdef ELAPSED_FUNC
	    elapsed_ਸਮਾਂ = ELAPSED_FUNC(ਸ਼ੁਰੂ_tv);
# endif
	    ਜੇ (wtime >= 0)
		wait_ਸਮਾਂ = wtime - elapsed_ਸਮਾਂ;
	    ਨਹੀਂਤਾਂ ਜੇ (delay_pending)
	    {
		ਲੰਮਾ delay_ਖੱਬਾ = p_acl - acl_elapsed;
		ਲੰਮਾ ut_ਖੱਬਾ = p_ut - elapsed_ਸਮਾਂ;

		ਜੇ (did_ਸ਼ੁਰੂ_blocking)
		    wait_ਸਮਾਂ = delay_ਖੱਬਾ;
		ਨਹੀਂਤਾਂ
		    wait_ਸਮਾਂ = MIN(delay_ਖੱਬਾ, ut_ਖੱਬਾ);
	    }
	    ਨਹੀਂਤਾਂ
		// going to block after p_ut
		wait_ਸਮਾਂ = p_ut - elapsed_ਸਮਾਂ;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    ਜੇ (wait_ਸਮਾਂ <= 0 && did_call_wait_func)
	    {
		ਜੇ (wtime >= 0)
		    // no character available within "wtime"
		    ਵਾਪਸ 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		ਜੇ (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_ਬਦਲਿਆ(tb_change_cnt))
		{
		    ਜੇ (buf == ਨਿਰਾ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (ਪੂਰਨ)KE_COMPLETE_DELAY;
			ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(ibuf, 3);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (ਪੂਰਨ)KE_COMPLETE_DELAY;
		    }
		    ਵਾਪਸ 3;
		}

		// No character available within 'updatetime'.
		did_ਸ਼ੁਰੂ_blocking = TRUE;
		ਜੇ (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_ਬਦਲਿਆ(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    ਜੇ (buf == ਨਿਰਾ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (ਪੂਰਨ)KE_CURSORHOLD;
			ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(ibuf, 3);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (ਪੂਰਨ)KE_CURSORHOLD;
		    }
		    ਵਾਪਸ 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		ਜਾਰੀ;
	    }
	}

# ifdef FEAT_ਕੰਮ_CHANNEL
	ਜੇ (wait_ਸਮਾਂ < 0 || wait_ਸਮਾਂ > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    ਜੇ (has_pending_ਕੰਮ())
		wait_ਸਮਾਂ = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    ਜੇ (channel_any_readahead())
		wait_ਸਮਾਂ = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	ਜੇ (p_beval && wait_ਸਮਾਂ > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_ਸਮਾਂ = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	ਜੇ (wait_func(wait_ਸਮਾਂ, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    ਜੇ (typebuf_ਬਦਲਿਆ(tb_change_cnt))
		ਵਾਪਸ 0;

	    // We might have something to return now.
	    ਜੇ (buf == ਨਿਰਾ)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		ਵਾਪਸ ਨਿਵੇਸ਼_ਉਪਲਬਧ();

	    len = read_from_ਨਿਵੇਸ਼_buf(buf, (ਲੰਮਾ)maxlen);
	    ਜੇ (len > 0)
		ਵਾਪਸ len;
	    ਜਾਰੀ;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_ਸਮਾਂ += wait_ਸਮਾਂ;
# endif

	ਜੇ ((resize_func != ਨਿਰਾ && resize_func(TRUE))
# ਜੇ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
		||
		(clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11 &&
		 server_ਉਡੀਕ())
# endif
# ifdef ਸੁਨੇਹਾ_ਕਤਾਰ
		|| interrupted
# endif
		|| wait_ਸਮਾਂ > 0
		|| (wtime < 0 && !did_ਸ਼ੁਰੂ_blocking))
	    // no character available, but something to be done, keep going
	    ਜਾਰੀ;

	// no character available or interrupted, return zero
	ਰੋਕੋ;
    }
    ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    ਪੂਰਨ
ui_wait_for_chars_or_timer(
    ਲੰਮਾ    wtime,
    ਪੂਰਨ	    (*wait_func)(ਲੰਮਾ wtime, ਪੂਰਨ *interrupted, ਪੂਰਨ ignore_ਨਿਵੇਸ਼),
    ਪੂਰਨ	    *interrupted,
    ਪੂਰਨ	    ignore_ਨਿਵੇਸ਼)
{
    ਪੂਰਨ	    due_ਸਮਾਂ;
    ਲੰਮਾ    ਬਚਿਆ = wtime;
    ਪੂਰਨ	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_ਕੰਮ_CHANNEL
    ਪੂਰਨ	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    ਜੇ (wtime >= 0 && wtime < 10L)
	ਵਾਪਸ wait_func(wtime, ਨਿਰਾ, ignore_ਨਿਵੇਸ਼);

    ਜਦੋਂ (wtime < 0 || ਬਚਿਆ > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_ਸਮਾਂ = check_due_timer();
	ਜੇ (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (due_ਸਮਾਂ <= 0 || (wtime > 0 && due_ਸਮਾਂ > ਬਚਿਆ))
	    due_ਸਮਾਂ = ਬਚਿਆ;
# ਜੇ defined(FEAT_ਕੰਮ_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	ਜੇ ((due_ਸਮਾਂ < 0 || due_ਸਮਾਂ > 10L) && (
#  ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
		(
#   ਜੇ defined(FEAT_GUI)
		!gui.ਵਿੱਚ_use &&
#   endif
		(has_pending_ਕੰਮ() || channel_any_readahead()))
#   ਜੇ defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  ਜੇ defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
		    has_any_sound_callback()
#  endif
		    ))
	{
	    // There is a pending job or channel, should return soon in order
	    // to handle them ASAP.  Do check for input briefly.
	    due_ਸਮਾਂ = 10L;
#  ifdef FEAT_ਕੰਮ_CHANNEL
	    brief_wait = TRUE;
#  endif
	}
# endif
	ਜੇ (wait_func(due_ਸਮਾਂ, interrupted, ignore_ਨਿਵੇਸ਼))
	    ਵਾਪਸ OK;
	ਜੇ ((interrupted != ਨਿਰਾ && *interrupted)
# ifdef FEAT_ਕੰਮ_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    ਵਾਪਸ FAIL;
	ਜੇ (wtime > 0)
	    ਬਚਿਆ -= due_ਸਮਾਂ;
    }
    ਵਾਪਸ FAIL;
}
#ਜੇ_ਅੰਤ

/*
 * Return non-zero if a character is available.
 */
    ਪੂਰਨ
ui_char_avail(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	gui_mch_update();
	ਵਾਪਸ ਨਿਵੇਸ਼_ਉਪਲਬਧ();
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NO_CONSOLE
# ifdef NO_CONSOLE_ਨਿਵੇਸ਼
    ਜੇ (no_console_ਨਿਵੇਸ਼())
	ਵਾਪਸ 0;
# endif
    ਵਾਪਸ mch_char_avail();
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    ਖਾਲੀ
ui_delay(ਲੰਮਾ msec_arg, ਪੂਰਨ ignoreinput)
{
    ਲੰਮਾ msec = msec_arg;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(ਨਿਰਾ, "ui_delay(%ld)", msec);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    ਖਾਲੀ
ui_suspend(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	gui_mch_iconify();
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    mch_suspend();
}

#ਜੇ !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    ਖਾਲੀ
suspend_shell(ਖਾਲੀ)
{
    ਜੇ (*p_sh == NUL)
	emsg(_(e_shell_option_is_ਖਾਲੀ_ਥਾਵਾਂ));
    ਨਹੀਂਤਾਂ
    {
	ਸੁਨੇਹਾ_puts(_("new shell started\n"));
	do_shell(ਨਿਰਾ, 0);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    ਪੂਰਨ
ui_get_shellsize(ਖਾਲੀ)
{
    ਪੂਰਨ	    retval;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	retval = gui_get_shellsize();
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    ਜੇ (retval == OK)
    {
	ਸੈੱਟ_ਸੰਖਿਆ_default("lines", ਪੰਕਤੀਆਂ);
	ਸੈੱਟ_ਸੰਖਿਆ_default("columns", Columns);
    }
    ਵਾਪਸ retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    ਖਾਲੀ
ui_ਸੈੱਟ_shellsize(
    ਪੂਰਨ		mustset ਅਣਵਰਤਿਆ)	// set by the user
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_ਸੈੱਟ_shellsize(mustset, TRUE, RESIZE_BOTH);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	mch_ਸੈੱਟ_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    ਖਾਲੀ
ui_new_shellsize(ਖਾਲੀ)
{
    ਜੇ (ਭਰਿਆ_screen && !exiting)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	    gui_new_shellsize();
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    mch_new_shellsize();
    }
}

#ਜੇ (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    ਪੂਰਨ
ui_get_winpos(ਪੂਰਨ *x, ਪੂਰਨ *y, varnumber_T timeout ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ gui_mch_get_winpos(x, y);
# endif
# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    ਵਾਪਸ mch_get_winpos(x, y);
# ਨਹੀਂਤਾਂ
#  ਜੇ defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    ਵਾਪਸ ਪਦ_get_winpos(x, y, timeout);
#  ਨਹੀਂਤਾਂ
    ਵਾਪਸ FAIL;
#  endif
# endif
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
ui_breakcheck(ਖਾਲੀ)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    ਖਾਲੀ
ui_breakcheck_force(ਪੂਰਨ force)
{
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;
    ਪੂਰਨ		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    ਜੇ (recursive)
	ਵਾਪਸ;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_mch_update();
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	mch_breakcheck(force);

    ਜੇ (save_updating_screen)
	updating_screen = TRUE;
    ਨਹੀਂਤਾਂ
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

#ਜੇ defined(USE_ਨਿਵੇਸ਼_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# ਜੇ defined(FEAT_ਕੰਮ_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# ਨਹੀਂਤਾਂ
#  define INBUFLEN 250
# endif

ਸਥਿਰ char_u	inbuf[INBUFLEN + MAX_ਕੁੰਜੀ_ਕੋਡ_LEN];
ਸਥਿਰ ਪੂਰਨ	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    ਪੂਰਨ
vim_is_ਨਿਵੇਸ਼_buf_ਭਰਿਆ(ਖਾਲੀ)
{
    ਵਾਪਸ (inbufcount >= INBUFLEN);
}

    ਪੂਰਨ
vim_is_ਨਿਵੇਸ਼_buf_ਖਾਲੀ_ਥਾਵਾਂ(ਖਾਲੀ)
{
    ਵਾਪਸ (inbufcount == 0);
}

# ਜੇ defined(FEAT_OLE)
    ਪੂਰਨ
vim_free_ਵਿੱਚ_ਨਿਵੇਸ਼_buf(ਖਾਲੀ)
{
    ਵਾਪਸ (INBUFLEN - inbufcount);
}
# endif

# ਜੇ defined(FEAT_GUI_GTK)
    ਪੂਰਨ
vim_ਵਰਤਿਆ_ਵਿੱਚ_ਨਿਵੇਸ਼_buf(ਖਾਲੀ)
{
    ਵਾਪਸ inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_ਨਿਵੇਸ਼_buf(ਖਾਲੀ)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    ਜੇ (gap != ਨਿਰਾ)
    {
	// Add one to avoid a zero size.
	gap->ga_ਅੰਕੜੇ = alloc(inbufcount + 1);
	ਜੇ (gap->ga_ਅੰਕੜੇ != ਨਿਰਾ)
	    mch_memmove(gap->ga_ਅੰਕੜੇ, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_ਨਿਵੇਸ਼_buf();
    ਵਾਪਸ (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    ਖਾਲੀ
ਸੈੱਟ_ਨਿਵੇਸ਼_buf(char_u *p, ਪੂਰਨ overwrite)
{
    garray_T	*gap = (garray_T *)p;

    ਜੇ (gap == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (gap->ga_ਅੰਕੜੇ != ਨਿਰਾ)
    {
	ਜੇ (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_ਅੰਕੜੇ, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	ਨਹੀਂਤਾਂ
	{
	    mch_memmove(inbuf + gap->ga_len, inbuf, inbufcount);
	    mch_memmove(inbuf, gap->ga_ਅੰਕੜੇ, gap->ga_len);
	    inbufcount += gap->ga_len;
	}
	vim_free(gap->ga_ਅੰਕੜੇ);
    }
    vim_free(gap);
}

/*
 * Add the given bytes to the input buffer
 * Special keys start with CSI.  A real CSI must have been translated to
 * CSI KS_EXTRA KE_CSI.  K_SPECIAL doesn't require translation.
 */
    ਖਾਲੀ
ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(char_u *s, ਪੂਰਨ len)
{
    ਜੇ (inbufcount + len > INBUFLEN + MAX_ਕੁੰਜੀ_ਕੋਡ_LEN)
	ਵਾਪਸ;	    // Shouldn't ever happen!

    ਜਦੋਂ (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    ਖਾਲੀ
ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(char_u *str, ਪੂਰਨ len)
{
    ਪੂਰਨ		i;
    char_u	buf[2];

    ਲਈ (i = 0; i < len; ++i)
    {
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(str + i, 1);
	ਜੇ (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (ਪੂਰਨ)KE_CSI;
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    ਖਾਲੀ
trash_ਨਿਵੇਸ਼_buf(ਖਾਲੀ)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    ਪੂਰਨ
read_from_ਨਿਵੇਸ਼_buf(char_u *buf, ਲੰਮਾ maxlen)
{
    ਜੇ (inbufcount == 0)	// if the buffer is empty, fill it
	fill_ਨਿਵੇਸ਼_buf(TRUE);
    ਜੇ (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    ਜੇ (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    ਵਾਪਸ (ਪੂਰਨ)maxlen;
}

    ਖਾਲੀ
fill_ਨਿਵੇਸ਼_buf(ਪੂਰਨ exit_on_error ਅਣਵਰਤਿਆ)
{
# ਜੇ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ਪੂਰਨ		len;
    ਪੂਰਨ		try;
    ਸਥਿਰ ਪੂਰਨ	did_read_something = FALSE;
    ਸਥਿਰ char_u *rest = ਨਿਰਾ;	    // unconverted rest of previous read
    ਸਥਿਰ ਪੂਰਨ	restlen = 0;
    ਪੂਰਨ		unconverted;
# endif

# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use
#  ifdef NO_CONSOLE_ਨਿਵੇਸ਼
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_ਨਿਵੇਸ਼()
#  endif
       )
    {
	gui_mch_update();
	ਵਾਪਸ;
    }
# endif
# ਜੇ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ਜੇ (vim_is_ਨਿਵੇਸ਼_buf_ਭਰਿਆ())
	ਵਾਪਸ;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    ਜੇ (rest != ਨਿਰਾ)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	ਜੇ (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	ਨਹੀਂਤਾਂ
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	ਜੇ (unconverted == restlen)
	    VIM_CLEAR(rest);
	ਨਹੀਂਤਾਂ
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    ਨਹੀਂਤਾਂ
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    ਲਈ (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / ਨਿਵੇਸ਼_conv.vc_ਗੁਣਕ);
#  ifdef VMS
	len = vms_read((ਅੱਖਰ *)inbuf + inbufcount, readlen);
#  ਨਹੀਂਤਾਂ
	len = ਪਠ(read_cmd_fd, (ਅੱਖਰ *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	ਜੇ (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(ਨਿਰਾ, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	ਜੇ (len > 0 || got_int)
	    ਰੋਕੋ;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	ਜੇ (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    ਪੂਰਨ m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  ਨਹੀਂਤਾਂ
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	ਜੇ (!exit_on_error)
	    ਵਾਪਸ;
    }
    ਜੇ (len <= 0 && !got_int)
	read_error_exit();
    ਜੇ (len > 0)
	did_read_something = TRUE;
    ਜੇ (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_ਨਿਵੇਸ਼_ਸੁਰੱਖਿਅਤ(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == ਨਿਰਾ ? &rest : ਨਿਰਾ, &restlen);
	}
	ਜਦੋਂ (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    ਜੇ (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !ਕੁੰਜੀ_protocol_enabled())
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
#ਜੇ_ਅੰਤ // USE_INPUT_BUF

/*
 * Exit because of an input read error.
 */
    ਖਾਲੀ
read_error_exit(ਖਾਲੀ)
{
    ਜੇ (silent_ਬਹੁਲਕ)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#ਜੇ defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    ਖਾਲੀ
ui_cursor_shape_forced(ਪੂਰਨ forced)
{
# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_update_cursor_later();
    ਨਹੀਂਤਾਂ
# endif
	ਪਦ_cursor_ਬਹੁਲਕ(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_ਸਤਰ(FALSE);
# endif
}

    ਖਾਲੀ
ui_cursor_shape(ਖਾਲੀ)
{
    ui_cursor_shape_forced(FALSE);
}
#ਜੇ_ਅੰਤ

/*
 * Check bounds for column number
 */
    ਪੂਰਨ
check_ਖਾਨਾ(ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (ਖਾਨਾ < 0)
	ਵਾਪਸ 0;
    ਜੇ (ਖਾਨਾ >= screen_Columns)
	ਵਾਪਸ screen_Columns - 1;
    ਵਾਪਸ ਖਾਨਾ;
}

/*
 * Check bounds for row number
 */
    ਪੂਰਨ
check_ਪੰਕਤੀ(ਪੂਰਨ ਪੰਕਤੀ)
{
    ਜੇ (ਪੰਕਤੀ < 0)
	ਵਾਪਸ 0;
    ਜੇ (ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ)
	ਵਾਪਸ screen_ਪੰਕਤੀਆਂ - 1;
    ਵਾਪਸ ਪੰਕਤੀ;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    ਲੰਮਾ
scroll_ਸਤਰ_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	ਖਾਨਾ = 0;

    ਜੇ (*p != NUL)
	ਲਈ (;;)
	{
	    ਪੂਰਨ	    w = chartabsize(p, ਖਾਨਾ);
	    MB_PTR_ADV(p);
	    ਜੇ (*p == NUL)		// don't count the last character
		ਰੋਕੋ;
	    ਖਾਨਾ += w;
	}
    ਵਾਪਸ ਖਾਨਾ;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_ਲੱਭੋ_longest_lnum(ਖਾਲੀ)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    (!gui.ਵਿੱਚ_use || vim_strchr(p_go, GO_HORSCROLL) == ਨਿਰਾ) &&
#ਜੇ_ਅੰਤ
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
    {
	linenr_T    lnum;
	ਲੰਮਾ	    n;
	ਲੰਮਾ	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	ਲਈ (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_ਸਤਰ_len(lnum);
	    ਜੇ (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (n == max && ਨਿਰਪੇਖ((ਪੂਰਨ)(lnum - curwin->w_cursor.lnum))
				     < ਨਿਰਪੇਖ((ਪੂਰਨ)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    ਨਹੀਂਤਾਂ
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    ਵਾਪਸ ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    ਖਾਲੀ
ui_focus_change(
    ਪੂਰਨ		ਵਿੱਚ_focus)	// TRUE if focus gained.
{
    ਸਥਿਰ time_t	ਆਖਰੀ_ਸਮਾਂ = (time_t)0;
    ਪੂਰਨ			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    ਜੇ (ਵਿੱਚ_focus && ਆਖਰੀ_ਸਮਾਂ + 2 < ਸਮਾਂ(ਨਿਰਾ))
    {
	need_redraw = check_timestamps(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		gui.ਵਿੱਚ_use
#ਨਹੀਂਤਾਂ
		FALSE
#ਜੇ_ਅੰਤ
		);
	ਆਖਰੀ_ਸਮਾਂ = ਸਮਾਂ(ਨਿਰਾ);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪਦ_focus_change(ਵਿੱਚ_focus);
#ਜੇ_ਅੰਤ

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(ਵਿੱਚ_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);

    ਜੇ (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    ਜੇ (need_maketitle)
	maketitle();
}

#ਜੇ defined(HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ)
/*
 * Save current Input Method status to specified place.
 */
    ਖਾਲੀ
im_save_ਸਥਿਤੀ(ਲੰਮਾ *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    ਜੇ (!p_imdisable && ਕੁੰਜੀ_Typed && !ਕੁੰਜੀ_Stuffed
# ifdef FEAT_XIM
	    && xic != ਨਿਰਾ
# endif
# ifdef FEAT_GUI
	    && (!gui.ਵਿੱਚ_use || gui.ਵਿੱਚ_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	ਜੇ (vgetc_im_ਸਰਗਰਮ)
	    *psave = B_IMODE_IM;
	ਨਹੀਂਤਾਂ ਜੇ (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#ਜੇ_ਅੰਤ
