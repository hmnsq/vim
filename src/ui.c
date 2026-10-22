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
ui_write(char_u *s, पूर्णांक len, पूर्णांक console UNUSED)
{
#ifdef FEAT_GUI
    जर (gui.in_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	जर (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	परत;
    }
#endif
#ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    जर (!(silent_mode && p_verbose == 0))
    {
# जर !defined(MSWIN)
	char_u	*tofree = रिक्त;

	जर (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    जर (tofree != रिक्त)
		s = tofree;
	}
# endif

	mch_write(s, len);
# जर defined(HAVE_FSYNC)
	जर (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# जर !defined(MSWIN)
	जर (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#endif
}

#जर defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
स्थिर char_u *ta_str = रिक्त;
स्थिर पूर्णांक ta_off;	// offset for next char to use when ta_str != NULL
स्थिर पूर्णांक ta_len;	// length of ta_str when it's not NULL

    शून्य
ui_inchar_undo(char_u *s, पूर्णांक len)
{
    char_u  *new;
    पूर्णांक	    newlen;

    newlen = len;
    जर (ta_str != रिक्त)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    जर (new == रिक्त)
	परत;

    जर (ta_str != रिक्त)
    {
	mch_memmove(new, ta_str + ta_off, (माप_प्रकार)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (माप_प्रकार)len);
	vim_free(ta_str);
    }
    अन्यथा
	mch_memmove(new, s, (माप_प्रकार)len);
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
    पूर्णांक
ui_inchar(
    char_u	*buf,
    पूर्णांक		maxlen,
    दीर्घ	wtime,	    // don't use "time", MIPS cannot handle it
    पूर्णांक		tb_change_cnt)
{
    पूर्णांक		retval = 0;

#जर defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    जर (ta_str != रिक्त)
    {
	जर (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (माप_प्रकार)ta_len);
	    VIM_CLEAR(ta_str);
	    परत ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (माप_प्रकार)maxlen);
	ta_off += maxlen;
	परत maxlen;
    }
#endif

#ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#endif

#ifdef NO_CONSOLE_आदान
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    जर (no_console_आदान())
    {
	स्थिर पूर्णांक मोजणी = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	जर (retval > 0 || typebuf_changed(tb_change_cnt) || wtime >= 0)
	    जा theend;
# endif
	जर (wtime == -1 && ++मोजणी == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	जा theend;
    }
#endif

    // If we are going to wait for some time or block...
    जर (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(शून्य)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	जर ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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
    जर (gui.in_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif
#ifndef NO_CONSOLE
# ifdef FEAT_GUI
    अन्यथा
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif

    जर (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(शून्य)vim_handle_signal(SIGNAL_BLOCK);

    ctrl_c_interrupts = TRUE;

#ifdef NO_CONSOLE_आदान
theend:
#endif
#ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#endif
    परत retval;
}

#जर defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    पूर्णांक
inchar_loop(
    char_u	*buf,
    पूर्णांक		maxlen,
    दीर्घ	wtime,	    // don't use "time", MIPS cannot handle it
    पूर्णांक		tb_change_cnt,
    पूर्णांक		(*wait_func)(दीर्घ wtime, पूर्णांक *interrupted, पूर्णांक ignore_आदान),
    पूर्णांक		(*resize_func)(पूर्णांक check_only))
{
    पूर्णांक		len;
    पूर्णांक		interrupted = FALSE;
    पूर्णांक		did_call_wait_func = FALSE;
    पूर्णांक		did_start_blocking = FALSE;
    दीर्घ	wait_time;
    दीर्घ	elapsed_time = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
# endif

    // repeat until we got a character or waited long enough
    साठी (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	जर (resize_func != रिक्त)
	    resize_func(FALSE);

# ifdef MESSAGE_QUEUE
	// Only process messages when waiting.
	जर (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    जर (typebuf_changed(tb_change_cnt))
		परत 0;
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
		&& (get_real_state() & MODE_घाला) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	दीर्घ acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	जर (wtime < 0 && did_start_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_time = -1;
	अन्यथा
	{
# ifdef ELAPSED_FUNC
	    elapsed_time = ELAPSED_FUNC(start_tv);
# endif
	    जर (wtime >= 0)
		wait_time = wtime - elapsed_time;
	    अन्यथा जर (delay_pending)
	    {
		दीर्घ delay_डावा = p_acl - acl_elapsed;
		दीर्घ ut_डावा = p_ut - elapsed_time;

		जर (did_start_blocking)
		    wait_time = delay_डावा;
		अन्यथा
		    wait_time = MIN(delay_डावा, ut_डावा);
	    }
	    अन्यथा
		// going to block after p_ut
		wait_time = p_ut - elapsed_time;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    जर (wait_time <= 0 && did_call_wait_func)
	    {
		जर (wtime >= 0)
		    // no character available within "wtime"
		    परत 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		जर (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    जर (buf == रिक्त)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (पूर्णांक)KE_COMPLETE_DELAY;
			add_to_आदान_buf(ibuf, 3);
		    }
		    अन्यथा
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (पूर्णांक)KE_COMPLETE_DELAY;
		    }
		    परत 3;
		}

		// No character available within 'updatetime'.
		did_start_blocking = TRUE;
		जर (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    जर (buf == रिक्त)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (पूर्णांक)KE_CURSORHOLD;
			add_to_आदान_buf(ibuf, 3);
		    }
		    अन्यथा
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (पूर्णांक)KE_CURSORHOLD;
		    }
		    परत 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		सुरूठेवा;
	    }
	}

# ifdef FEAT_JOB_CHANNEL
	जर (wait_time < 0 || wait_time > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    जर (has_pending_job())
		wait_time = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    जर (channel_any_readahead())
		wait_time = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	जर (p_beval && wait_time > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_time = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	जर (wait_func(wait_time, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    जर (typebuf_changed(tb_change_cnt))
		परत 0;

	    // We might have something to return now.
	    जर (buf == रिक्त)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		परत आदान_available();

	    len = read_from_आदान_buf(buf, (दीर्घ)maxlen);
	    जर (len > 0)
		परत len;
	    सुरूठेवा;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_time += wait_time;
# endif

	जर ((resize_func != रिक्त && resize_func(TRUE))
# जर defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
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
	    सुरूठेवा;

	// no character available or interrupted, return zero
	थांबा;
    }
    परत 0;
}
#endif

#जर defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    पूर्णांक
ui_wait_for_chars_or_timer(
    दीर्घ    wtime,
    पूर्णांक	    (*wait_func)(दीर्घ wtime, पूर्णांक *interrupted, पूर्णांक ignore_आदान),
    पूर्णांक	    *interrupted,
    पूर्णांक	    ignore_आदान)
{
    पूर्णांक	    due_time;
    दीर्घ    remaining = wtime;
    पूर्णांक	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_JOB_CHANNEL
    पूर्णांक	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    जर (wtime >= 0 && wtime < 10L)
	परत wait_func(wtime, रिक्त, ignore_आदान);

    पर्यंत (wtime < 0 || remaining > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_time = check_due_timer();
	जर (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    परत FAIL;
	}
	जर (due_time <= 0 || (wtime > 0 && due_time > remaining))
	    due_time = remaining;
# जर defined(FEAT_JOB_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	जर ((due_time < 0 || due_time > 10L) && (
#  जर defined(FEAT_JOB_CHANNEL)
		(
#   जर defined(FEAT_GUI)
		!gui.in_use &&
#   endif
		(has_pending_job() || channel_any_readahead()))
#   जर defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  जर defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
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
	जर (wait_func(due_time, interrupted, ignore_आदान))
	    परत OK;
	जर ((interrupted != रिक्त && *interrupted)
# ifdef FEAT_JOB_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    परत FAIL;
	जर (wtime > 0)
	    remaining -= due_time;
    }
    परत FAIL;
}
#endif

/*
 * Return non-zero if a character is available.
 */
    पूर्णांक
ui_char_avail(शून्य)
{
#ifdef FEAT_GUI
    जर (gui.in_use)
    {
	gui_mch_update();
	परत आदान_available();
    }
#endif
#ifndef NO_CONSOLE
# ifdef NO_CONSOLE_आदान
    जर (no_console_आदान())
	परत 0;
# endif
    परत mch_char_avail();
#अन्यथा
    परत 0;
#endif
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    शून्य
ui_delay(दीर्घ msec_arg, पूर्णांक ignoreinput)
{
    दीर्घ msec = msec_arg;

#ifdef FEAT_EVAL
    जर (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(रिक्त, "ui_delay(%ld)", msec);
#endif
#ifdef FEAT_GUI
    जर (gui.in_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    अन्यथा
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
    जर (gui.in_use)
    {
	gui_mch_iconify();
	परत;
    }
#endif
    mch_suspend();
}

#जर !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    शून्य
suspend_shell(शून्य)
{
    जर (*p_sh == NUL)
	emsg(_(e_shell_option_is_empty));
    अन्यथा
    {
	संदेश_puts(_("new shell started\n"));
	do_shell(रिक्त, 0);
    }
}
#endif

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    पूर्णांक
ui_get_shellsize(शून्य)
{
    पूर्णांक	    retval;

#ifdef FEAT_GUI
    जर (gui.in_use)
	retval = gui_get_shellsize();
    अन्यथा
#endif
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    जर (retval == OK)
    {
	set_संख्या_default("lines", Rows);
	set_संख्या_default("columns", Columns);
    }
    परत retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    शून्य
ui_set_shellsize(
    पूर्णांक		mustset UNUSED)	// set by the user
{
#ifdef FEAT_GUI
    जर (gui.in_use)
	gui_set_shellsize(mustset, TRUE, RESIZE_BOTH);
    अन्यथा
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
    जर (full_screen && !exiting)
    {
#ifdef FEAT_GUI
	जर (gui.in_use)
	    gui_new_shellsize();
	अन्यथा
#endif
	    mch_new_shellsize();
    }
}

#जर (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    पूर्णांक
ui_get_winpos(पूर्णांक *x, पूर्णांक *y, varnumber_T timeout UNUSED)
{
# ifdef FEAT_GUI
    जर (gui.in_use)
	परत gui_mch_get_winpos(x, y);
# endif
# जर defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    परत mch_get_winpos(x, y);
# अन्यथा
#  जर defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    परत term_get_winpos(x, y, timeout);
#  अन्यथा
    परत FAIL;
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
ui_breakcheck_force(पूर्णांक force)
{
    स्थिर पूर्णांक	recursive = FALSE;
    पूर्णांक		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    जर (recursive)
	परत;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ifdef FEAT_GUI
    जर (gui.in_use)
	gui_mch_update();
    अन्यथा
#endif
	mch_breakcheck(force);

    जर (save_updating_screen)
	updating_screen = TRUE;
    अन्यथा
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

#जर defined(USE_आदान_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# जर defined(FEAT_JOB_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# अन्यथा
#  define INBUFLEN 250
# endif

स्थिर char_u	inbuf[INBUFLEN + MAX_किल्ली_CODE_LEN];
स्थिर पूर्णांक	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    पूर्णांक
vim_is_आदान_buf_full(शून्य)
{
    परत (inbufcount >= INBUFLEN);
}

    पूर्णांक
vim_is_आदान_buf_empty(शून्य)
{
    परत (inbufcount == 0);
}

# जर defined(FEAT_OLE)
    पूर्णांक
vim_free_in_आदान_buf(शून्य)
{
    परत (INBUFLEN - inbufcount);
}
# endif

# जर defined(FEAT_GUI_GTK)
    पूर्णांक
vim_used_in_आदान_buf(शून्य)
{
    परत inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_आदान_buf(शून्य)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    जर (gap != रिक्त)
    {
	// Add one to avoid a zero size.
	gap->ga_data = alloc(inbufcount + 1);
	जर (gap->ga_data != रिक्त)
	    mch_memmove(gap->ga_data, inbuf, (माप_प्रकार)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_आदान_buf();
    परत (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    शून्य
set_आदान_buf(char_u *p, पूर्णांक overwrite)
{
    garray_T	*gap = (garray_T *)p;

    जर (gap == रिक्त)
	परत;

    जर (gap->ga_data != रिक्त)
    {
	जर (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_data, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	अन्यथा
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
add_to_आदान_buf(char_u *s, पूर्णांक len)
{
    जर (inbufcount + len > INBUFLEN + MAX_किल्ली_CODE_LEN)
	परत;	    // Shouldn't ever happen!

    पर्यंत (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    शून्य
add_to_आदान_buf_csi(char_u *str, पूर्णांक len)
{
    पूर्णांक		i;
    char_u	buf[2];

    साठी (i = 0; i < len; ++i)
    {
	add_to_आदान_buf(str + i, 1);
	जर (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (पूर्णांक)KE_CSI;
	    add_to_आदान_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    शून्य
trash_आदान_buf(शून्य)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    पूर्णांक
read_from_आदान_buf(char_u *buf, दीर्घ maxlen)
{
    जर (inbufcount == 0)	// if the buffer is empty, fill it
	fill_आदान_buf(TRUE);
    जर (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (माप_प्रकार)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    जर (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (माप_प्रकार)inbufcount);
    परत (पूर्णांक)maxlen;
}

    शून्य
fill_आदान_buf(पूर्णांक exit_on_error UNUSED)
{
# जर defined(UNIX) || defined(VMS) || defined(MACOS_X)
    पूर्णांक		len;
    पूर्णांक		try;
    स्थिर पूर्णांक	did_read_something = FALSE;
    स्थिर char_u *rest = रिक्त;	    // unconverted rest of previous read
    स्थिर पूर्णांक	restlen = 0;
    पूर्णांक		unconverted;
# endif

# ifdef FEAT_GUI
    जर (gui.in_use
#  ifdef NO_CONSOLE_आदान
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_आदान()
#  endif
       )
    {
	gui_mch_update();
	परत;
    }
# endif
# जर defined(UNIX) || defined(VMS) || defined(MACOS_X)
    जर (vim_is_आदान_buf_full())
	परत;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    जर (rest != रिक्त)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	जर (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	अन्यथा
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	जर (unconverted == restlen)
	    VIM_CLEAR(rest);
	अन्यथा
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    अन्यथा
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    साठी (try = 0; try < 100; ++try)
    {
	माप_प्रकार readlen = (माप_प्रकार)((INBUFLEN - inbufcount)
						       / आदान_conv.vc_factor);
#  ifdef VMS
	len = vms_read((अक्षर *)inbuf + inbufcount, readlen);
#  अन्यथा
	len = पठ(read_cmd_fd, (अक्षर *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	जर (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(रिक्त, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	जर (len > 0 || got_int)
	    थांबा;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	जर (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    पूर्णांक m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    बंद(0);
	    vim_ignored = dup(2);
#  अन्यथा
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	जर (!exit_on_error)
	    परत;
    }
    जर (len <= 0 && !got_int)
	read_error_exit();
    जर (len > 0)
	did_read_something = TRUE;
    जर (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    अन्यथा
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	जर (आदान_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_आदान_safe(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == रिक्त ? &rest : रिक्त, &restlen);
	}
	पर्यंत (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    जर (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !किल्ली_protocol_enabled())
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
		mch_memmove(inbuf, inbuf + inbufcount, (माप_प्रकार)(len));
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
    जर (silent_mode)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#जर defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    शून्य
ui_cursor_shape_forced(पूर्णांक forced)
{
# ifdef FEAT_GUI
    जर (gui.in_use)
	gui_update_cursor_later();
    अन्यथा
# endif
	term_cursor_mode(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_ओळ(FALSE);
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
    पूर्णांक
check_col(पूर्णांक col)
{
    जर (col < 0)
	परत 0;
    जर (col >= screen_Columns)
	परत screen_Columns - 1;
    परत col;
}

/*
 * Check bounds for row number
 */
    पूर्णांक
check_row(पूर्णांक row)
{
    जर (row < 0)
	परत 0;
    जर (row >= screen_Rows)
	परत screen_Rows - 1;
    परत row;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    दीर्घ
scroll_ओळ_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	col = 0;

    जर (*p != NUL)
	साठी (;;)
	{
	    पूर्णांक	    w = chartabsize(p, col);
	    MB_PTR_ADV(p);
	    जर (*p == NUL)		// don't count the last character
		थांबा;
	    col += w;
	}
    परत col;
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
    जर (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == रिक्त) &&
#endif
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_ओळ_मोजणी + 1)
    {
	linenr_T    lnum;
	दीर्घ	    n;
	दीर्घ	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	साठी (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_ओळ_len(lnum);
	    जर (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    अन्यथा जर (n == max && असल((पूर्णांक)(lnum - curwin->w_cursor.lnum))
				     < असल((पूर्णांक)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    अन्यथा
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    परत ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    शून्य
ui_focus_change(
    पूर्णांक		in_focus)	// TRUE if focus gained.
{
    स्थिर समय_प्रकार	last_time = (समय_प्रकार)0;
    पूर्णांक			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    जर (in_focus && last_time + 2 < समय(रिक्त))
    {
	need_redraw = check_timestamps(
#ifdef FEAT_GUI
		gui.in_use
#अन्यथा
		FALSE
#endif
		);
	last_time = समय(रिक्त);
    }

#ifdef FEAT_TERMINAL
    term_focus_change(in_focus);
#endif

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(in_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, रिक्त, रिक्त, FALSE, curbuf);

    जर (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    जर (need_maketitle)
	maketitle();
}

#जर defined(HAVE_आदान_METHOD)
/*
 * Save current Input Method status to specified place.
 */
    शून्य
im_save_status(दीर्घ *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    जर (!p_imdisable && किल्ली_Typed && !किल्ली_Stuffed
# ifdef FEAT_XIM
	    && xic != रिक्त
# endif
# ifdef FEAT_GUI
	    && (!gui.in_use || gui.in_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	जर (vgetc_im_active)
	    *psave = B_IMODE_IM;
	अन्यथा जर (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#endif
