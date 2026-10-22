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

#अन्तर्भाव "vim.h"

    शून्य
ui_write(char_u *s, पूर्णाङ्क len, पूर्णाङ्क console UNUSED)
{
#ifdef FEAT_GUI
    सचे (gui.in_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	सचे (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	पटिनिवत्त;
    }
#endif
#ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    सचे (!(silent_mode && p_verbose == 0))
    {
# सचे !defined(MSWIN)
	char_u	*tofree = रिक्त;

	सचे (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    सचे (tofree != रिक्त)
		s = tofree;
	}
# endif

	mch_write(s, len);
# सचे defined(HAVE_FSYNC)
	सचे (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# सचे !defined(MSWIN)
	सचे (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#endif
}

#सचे defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
स्थिर char_u *ta_str = रिक्त;
स्थिर पूर्णाङ्क ta_off;	// offset for next char to use when ta_str != NULL
स्थिर पूर्णाङ्क ta_len;	// length of ta_str when it's not NULL

    शून्य
ui_inchar_undo(char_u *s, पूर्णाङ्क len)
{
    char_u  *new;
    पूर्णाङ्क	    newlen;

    newlen = len;
    सचे (ta_str != रिक्त)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    सचे (new == रिक्त)
	पटिनिवत्त;

    सचे (ta_str != रिक्त)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    अञ्ञथा
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
    पूर्णाङ्क
ui_inchar(
    char_u	*buf,
    पूर्णाङ्क		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    पूर्णाङ्क		tb_change_cnt)
{
    पूर्णाङ्क		retval = 0;

#सचे defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    सचे (ta_str != रिक्त)
    {
	सचे (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    पटिनिवत्त ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	पटिनिवत्त maxlen;
    }
#endif

#ifdef FEAT_PROFILE
    सचे (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#endif

#ifdef NO_CONSOLE_निवेशः
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    सचे (no_console_निवेशः())
    {
	स्थिर पूर्णाङ्क गणनम् = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	सचे (retval > 0 || typebuf_changed(tb_change_cnt) || wtime >= 0)
	    गच्छ theend;
# endif
	सचे (wtime == -1 && ++गणनम् == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	गच्छ theend;
    }
#endif

    // If we are going to wait for some time or block...
    सचे (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(शून्य)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	सचे ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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
    सचे (gui.in_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif
#ifndef NO_CONSOLE
# ifdef FEAT_GUI
    अञ्ञथा
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif

    सचे (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(शून्य)vim_handle_signal(SIGNAL_BLOCK);

    ctrl_c_interrupts = TRUE;

#ifdef NO_CONSOLE_निवेशः
theend:
#endif
#ifdef FEAT_PROFILE
    सचे (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#endif
    पटिनिवत्त retval;
}

#सचे defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    पूर्णाङ्क
inchar_loop(
    char_u	*buf,
    पूर्णाङ्क		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    पूर्णाङ्क		tb_change_cnt,
    पूर्णाङ्क		(*wait_func)(long wtime, पूर्णाङ्क *interrupted, पूर्णाङ्क ignore_निवेशः),
    पूर्णाङ्क		(*resize_func)(पूर्णाङ्क check_only))
{
    पूर्णाङ्क		len;
    पूर्णाङ्क		interrupted = FALSE;
    पूर्णाङ्क		did_call_wait_func = FALSE;
    पूर्णाङ्क		did_start_blocking = FALSE;
    long	wait_time;
    long	elapsed_time = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
# endif

    // repeat until we got a character or waited long enough
    अत्थाय (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	सचे (resize_func != रिक्त)
	    resize_func(FALSE);

# ifdef MESSAGE_QUEUE
	// Only process messages when waiting.
	सचे (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    सचे (typebuf_changed(tb_change_cnt))
		पटिनिवत्त 0;
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
		&& (get_real_state() & MODE_निवेशय) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	long acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	सचे (wtime < 0 && did_start_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_time = -1;
	अञ्ञथा
	{
# ifdef ELAPSED_FUNC
	    elapsed_time = ELAPSED_FUNC(start_tv);
# endif
	    सचे (wtime >= 0)
		wait_time = wtime - elapsed_time;
	    अञ्ञथा सचे (delay_pending)
	    {
		long delay_वामः = p_acl - acl_elapsed;
		long ut_वामः = p_ut - elapsed_time;

		सचे (did_start_blocking)
		    wait_time = delay_वामः;
		अञ्ञथा
		    wait_time = MIN(delay_वामः, ut_वामः);
	    }
	    अञ्ञथा
		// going to block after p_ut
		wait_time = p_ut - elapsed_time;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    सचे (wait_time <= 0 && did_call_wait_func)
	    {
		सचे (wtime >= 0)
		    // no character available within "wtime"
		    पटिनिवत्त 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		सचे (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    सचे (buf == रिक्त)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (पूर्णाङ्क)KE_COMPLETE_DELAY;
			add_to_निवेशः_buf(ibuf, 3);
		    }
		    अञ्ञथा
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (पूर्णाङ्क)KE_COMPLETE_DELAY;
		    }
		    पटिनिवत्त 3;
		}

		// No character available within 'updatetime'.
		did_start_blocking = TRUE;
		सचे (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    सचे (buf == रिक्त)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (पूर्णाङ्क)KE_CURSORHOLD;
			add_to_निवेशः_buf(ibuf, 3);
		    }
		    अञ्ञथा
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (पूर्णाङ्क)KE_CURSORHOLD;
		    }
		    पटिनिवत्त 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		अनुवर्त;
	    }
	}

# ifdef FEAT_JOB_CHANNEL
	सचे (wait_time < 0 || wait_time > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    सचे (has_pending_job())
		wait_time = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    सचे (channel_any_readahead())
		wait_time = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	सचे (p_beval && wait_time > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_time = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	सचे (wait_func(wait_time, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    सचे (typebuf_changed(tb_change_cnt))
		पटिनिवत्त 0;

	    // We might have something to return now.
	    सचे (buf == रिक्त)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		पटिनिवत्त निवेशः_available();

	    len = read_from_निवेशः_buf(buf, (long)maxlen);
	    सचे (len > 0)
		पटिनिवत्त len;
	    अनुवर्त;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_time += wait_time;
# endif

	सचे ((resize_func != रिक्त && resize_func(TRUE))
# सचे defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
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
	    अनुवर्त;

	// no character available or interrupted, return zero
	भङ्ग;
    }
    पटिनिवत्त 0;
}
#endif

#सचे defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    पूर्णाङ्क
ui_wait_for_chars_or_timer(
    long    wtime,
    पूर्णाङ्क	    (*wait_func)(long wtime, पूर्णाङ्क *interrupted, पूर्णाङ्क ignore_निवेशः),
    पूर्णाङ्क	    *interrupted,
    पूर्णाङ्क	    ignore_निवेशः)
{
    पूर्णाङ्क	    due_time;
    long    remaining = wtime;
    पूर्णाङ्क	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_JOB_CHANNEL
    पूर्णाङ्क	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    सचे (wtime >= 0 && wtime < 10L)
	पटिनिवत्त wait_func(wtime, रिक्त, ignore_निवेशः);

    याव (wtime < 0 || remaining > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_time = check_due_timer();
	सचे (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    पटिनिवत्त FAIL;
	}
	सचे (due_time <= 0 || (wtime > 0 && due_time > remaining))
	    due_time = remaining;
# सचे defined(FEAT_JOB_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	सचे ((due_time < 0 || due_time > 10L) && (
#  सचे defined(FEAT_JOB_CHANNEL)
		(
#   सचे defined(FEAT_GUI)
		!gui.in_use &&
#   endif
		(has_pending_job() || channel_any_readahead()))
#   सचे defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  सचे defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
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
	सचे (wait_func(due_time, interrupted, ignore_निवेशः))
	    पटिनिवत्त OK;
	सचे ((interrupted != रिक्त && *interrupted)
# ifdef FEAT_JOB_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    पटिनिवत्त FAIL;
	सचे (wtime > 0)
	    remaining -= due_time;
    }
    पटिनिवत्त FAIL;
}
#endif

/*
 * Return non-zero if a character is available.
 */
    पूर्णाङ्क
ui_char_avail(शून्य)
{
#ifdef FEAT_GUI
    सचे (gui.in_use)
    {
	gui_mch_update();
	पटिनिवत्त निवेशः_available();
    }
#endif
#ifndef NO_CONSOLE
# ifdef NO_CONSOLE_निवेशः
    सचे (no_console_निवेशः())
	पटिनिवत्त 0;
# endif
    पटिनिवत्त mch_char_avail();
#अञ्ञथा
    पटिनिवत्त 0;
#endif
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    शून्य
ui_delay(long msec_arg, पूर्णाङ्क ignoreinput)
{
    long msec = msec_arg;

#ifdef FEAT_EVAL
    सचे (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(रिक्त, "ui_delay(%ld)", msec);
#endif
#ifdef FEAT_GUI
    सचे (gui.in_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    अञ्ञथा
#endif
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    शून्य
ui_suspend(शून्य)
{
#ifdef FEAT_GUI
    सचे (gui.in_use)
    {
	gui_mch_iconify();
	पटिनिवत्त;
    }
#endif
    mch_suspend();
}

#सचे !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    शून्य
suspend_shell(शून्य)
{
    सचे (*p_sh == NUL)
	emsg(_(e_shell_option_is_empty));
    अञ्ञथा
    {
	सन्देशः_puts(_("new shell started\n"));
	do_shell(रिक्त, 0);
    }
}
#endif

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    पूर्णाङ्क
ui_get_shellsize(शून्य)
{
    पूर्णाङ्क	    retval;

#ifdef FEAT_GUI
    सचे (gui.in_use)
	retval = gui_get_shellsize();
    अञ्ञथा
#endif
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    सचे (retval == OK)
    {
	set_सङ्ख्या_default("lines", Rows);
	set_सङ्ख्या_default("columns", Columns);
    }
    पटिनिवत्त retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    शून्य
ui_set_shellsize(
    पूर्णाङ्क		mustset UNUSED)	// set by the user
{
#ifdef FEAT_GUI
    सचे (gui.in_use)
	gui_set_shellsize(mustset, TRUE, RESIZE_BOTH);
    अञ्ञथा
#endif
	mch_set_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    शून्य
ui_new_shellsize(शून्य)
{
    सचे (full_screen && !exiting)
    {
#ifdef FEAT_GUI
	सचे (gui.in_use)
	    gui_new_shellsize();
	अञ्ञथा
#endif
	    mch_new_shellsize();
    }
}

#सचे (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    पूर्णाङ्क
ui_get_winpos(पूर्णाङ्क *x, पूर्णाङ्क *y, varnumber_T timeout UNUSED)
{
# ifdef FEAT_GUI
    सचे (gui.in_use)
	पटिनिवत्त gui_mch_get_winpos(x, y);
# endif
# सचे defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    पटिनिवत्त mch_get_winpos(x, y);
# अञ्ञथा
#  सचे defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    पटिनिवत्त term_get_winpos(x, y, timeout);
#  अञ्ञथा
    पटिनिवत्त FAIL;
#  endif
# endif
}
#endif

    शून्य
ui_breakcheck(शून्य)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    शून्य
ui_breakcheck_force(पूर्णाङ्क force)
{
    स्थिर पूर्णाङ्क	recursive = FALSE;
    पूर्णाङ्क		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    सचे (recursive)
	पटिनिवत्त;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ifdef FEAT_GUI
    सचे (gui.in_use)
	gui_mch_update();
    अञ्ञथा
#endif
	mch_breakcheck(force);

    सचे (save_updating_screen)
	updating_screen = TRUE;
    अञ्ञथा
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

#सचे defined(USE_निवेशः_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# सचे defined(FEAT_JOB_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# अञ्ञथा
#  define INBUFLEN 250
# endif

स्थिर char_u	inbuf[INBUFLEN + MAX_कुञ्चिका_CODE_LEN];
स्थिर पूर्णाङ्क	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    पूर्णाङ्क
vim_is_निवेशः_buf_full(शून्य)
{
    पटिनिवत्त (inbufcount >= INBUFLEN);
}

    पूर्णाङ्क
vim_is_निवेशः_buf_empty(शून्य)
{
    पटिनिवत्त (inbufcount == 0);
}

# सचे defined(FEAT_OLE)
    पूर्णाङ्क
vim_free_in_निवेशः_buf(शून्य)
{
    पटिनिवत्त (INBUFLEN - inbufcount);
}
# endif

# सचे defined(FEAT_GUI_GTK)
    पूर्णाङ्क
vim_used_in_निवेशः_buf(शून्य)
{
    पटिनिवत्त inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_निवेशः_buf(शून्य)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    सचे (gap != रिक्त)
    {
	// Add one to avoid a zero size.
	gap->ga_data = alloc(inbufcount + 1);
	सचे (gap->ga_data != रिक्त)
	    mch_memmove(gap->ga_data, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_निवेशः_buf();
    पटिनिवत्त (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    शून्य
set_निवेशः_buf(char_u *p, पूर्णाङ्क overwrite)
{
    garray_T	*gap = (garray_T *)p;

    सचे (gap == रिक्त)
	पटिनिवत्त;

    सचे (gap->ga_data != रिक्त)
    {
	सचे (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_data, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	अञ्ञथा
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
    शून्य
add_to_निवेशः_buf(char_u *s, पूर्णाङ्क len)
{
    सचे (inbufcount + len > INBUFLEN + MAX_कुञ्चिका_CODE_LEN)
	पटिनिवत्त;	    // Shouldn't ever happen!

    याव (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    शून्य
add_to_निवेशः_buf_csi(char_u *str, पूर्णाङ्क len)
{
    पूर्णाङ्क		i;
    char_u	buf[2];

    अत्थाय (i = 0; i < len; ++i)
    {
	add_to_निवेशः_buf(str + i, 1);
	सचे (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (पूर्णाङ्क)KE_CSI;
	    add_to_निवेशः_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    शून्य
trash_निवेशः_buf(शून्य)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    पूर्णाङ्क
read_from_निवेशः_buf(char_u *buf, long maxlen)
{
    सचे (inbufcount == 0)	// if the buffer is empty, fill it
	fill_निवेशः_buf(TRUE);
    सचे (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    सचे (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    पटिनिवत्त (पूर्णाङ्क)maxlen;
}

    शून्य
fill_निवेशः_buf(पूर्णाङ्क exit_on_error UNUSED)
{
# सचे defined(UNIX) || defined(VMS) || defined(MACOS_X)
    पूर्णाङ्क		len;
    पूर्णाङ्क		try;
    स्थिर पूर्णाङ्क	did_read_something = FALSE;
    स्थिर char_u *rest = रिक्त;	    // unconverted rest of previous read
    स्थिर पूर्णाङ्क	restlen = 0;
    पूर्णाङ्क		unconverted;
# endif

# ifdef FEAT_GUI
    सचे (gui.in_use
#  ifdef NO_CONSOLE_निवेशः
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_निवेशः()
#  endif
       )
    {
	gui_mch_update();
	पटिनिवत्त;
    }
# endif
# सचे defined(UNIX) || defined(VMS) || defined(MACOS_X)
    सचे (vim_is_निवेशः_buf_full())
	पटिनिवत्त;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    सचे (rest != रिक्त)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	सचे (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	अञ्ञथा
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	सचे (unconverted == restlen)
	    VIM_CLEAR(rest);
	अञ्ञथा
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    अञ्ञथा
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    अत्थाय (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / निवेशः_conv.vc_factor);
#  ifdef VMS
	len = vms_read((वर्ण *)inbuf + inbufcount, readlen);
#  अञ्ञथा
	len = पठ(read_cmd_fd, (वर्ण *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	सचे (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(रिक्त, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	सचे (len > 0 || got_int)
	    भङ्ग;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	सचे (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    पूर्णाङ्क m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  अञ्ञथा
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	सचे (!exit_on_error)
	    पटिनिवत्त;
    }
    सचे (len <= 0 && !got_int)
	read_error_exit();
    सचे (len > 0)
	did_read_something = TRUE;
    सचे (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    अञ्ञथा
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	सचे (निवेशः_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_निवेशः_safe(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == रिक्त ? &rest : रिक्त, &restlen);
	}
	याव (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    सचे (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !कुञ्चिका_protocol_enabled())
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
    शून्य
read_error_exit(शून्य)
{
    सचे (silent_mode)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#सचे defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    शून्य
ui_cursor_shape_forced(पूर्णाङ्क forced)
{
# ifdef FEAT_GUI
    सचे (gui.in_use)
	gui_update_cursor_later();
    अञ्ञथा
# endif
	term_cursor_mode(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_पङ्क्तिः(FALSE);
# endif
}

    शून्य
ui_cursor_shape(शून्य)
{
    ui_cursor_shape_forced(FALSE);
}
#endif

/*
 * Check bounds for column number
 */
    पूर्णाङ्क
check_col(पूर्णाङ्क col)
{
    सचे (col < 0)
	पटिनिवत्त 0;
    सचे (col >= screen_Columns)
	पटिनिवत्त screen_Columns - 1;
    पटिनिवत्त col;
}

/*
 * Check bounds for row number
 */
    पूर्णाङ्क
check_row(पूर्णाङ्क row)
{
    सचे (row < 0)
	पटिनिवत्त 0;
    सचे (row >= screen_Rows)
	पटिनिवत्त screen_Rows - 1;
    पटिनिवत्त row;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    long
scroll_पङ्क्तिः_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	col = 0;

    सचे (*p != NUL)
	अत्थाय (;;)
	{
	    पूर्णाङ्क	    w = chartabsize(p, col);
	    MB_PTR_ADV(p);
	    सचे (*p == NUL)		// don't count the last character
		भङ्ग;
	    col += w;
	}
    पटिनिवत्त col;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_find_longest_lnum(शून्य)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    सचे (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == रिक्त) &&
#endif
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_पङ्क्तिः_गणनम् + 1)
    {
	linenr_T    lnum;
	long	    n;
	long	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	अत्थाय (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_पङ्क्तिः_len(lnum);
	    सचे (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    अञ्ञथा सचे (n == max && abs((पूर्णाङ्क)(lnum - curwin->w_cursor.lnum))
				     < abs((पूर्णाङ्क)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    अञ्ञथा
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    पटिनिवत्त ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    शून्य
ui_focus_change(
    पूर्णाङ्क		in_focus)	// TRUE if focus gained.
{
    स्थिर time_t	last_time = (time_t)0;
    पूर्णाङ्क			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    सचे (in_focus && last_time + 2 < time(रिक्त))
    {
	need_redraw = check_timestamps(
#ifdef FEAT_GUI
		gui.in_use
#अञ्ञथा
		FALSE
#endif
		);
	last_time = time(रिक्त);
    }

#ifdef FEAT_TERMINAL
    term_focus_change(in_focus);
#endif

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(in_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, रिक्त, रिक्त, FALSE, curbuf);

    सचे (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    सचे (need_maketitle)
	maketitle();
}

#सचे defined(HAVE_निवेशः_METHOD)
/*
 * Save current Input Method status to specified place.
 */
    शून्य
im_save_status(long *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    सचे (!p_imdisable && कुञ्चिका_Typed && !कुञ्चिका_Stuffed
# ifdef FEAT_XIM
	    && xic != रिक्त
# endif
# ifdef FEAT_GUI
	    && (!gui.in_use || gui.in_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	सचे (vgetc_im_active)
	    *psave = B_IMODE_IM;
	अञ्ञथा सचे (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#endif
