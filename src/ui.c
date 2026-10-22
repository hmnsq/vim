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

#شامل "vim.h"

    تهی
ui_write(char_u *s, صحیح len, صحیح console UNUSED)
{
#ifdef FEAT_GUI
    اگر (gui.in_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	اگر (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	بازگشت;
    }
#endif
#ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    اگر (!(silent_mode && p_verbose == 0))
    {
# اگر !defined(MSWIN)
	char_u	*tofree = پوچ;

	اگر (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    اگر (tofree != پوچ)
		s = tofree;
	}
# endif

	mch_write(s, len);
# اگر defined(HAVE_FSYNC)
	اگر (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# اگر !defined(MSWIN)
	اگر (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#endif
}

#اگر defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
ایستا char_u *ta_str = پوچ;
ایستا صحیح ta_off;	// offset for next char to use when ta_str != NULL
ایستا صحیح ta_len;	// length of ta_str when it's not NULL

    تهی
ui_inchar_undo(char_u *s, صحیح len)
{
    char_u  *new;
    صحیح	    newlen;

    newlen = len;
    اگر (ta_str != پوچ)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    اگر (new == پوچ)
	بازگشت;

    اگر (ta_str != پوچ)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    وگرنه
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
    صحیح
ui_inchar(
    char_u	*buf,
    صحیح		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    صحیح		tb_change_cnt)
{
    صحیح		retval = 0;

#اگر defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    اگر (ta_str != پوچ)
    {
	اگر (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    بازگشت ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	بازگشت maxlen;
    }
#endif

#ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#endif

#ifdef NO_CONSOLE_درون_داد
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    اگر (no_console_درون_داد())
    {
	ایستا صحیح شمار = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	اگر (retval > 0 || typebuf_changed(tb_change_cnt) || wtime >= 0)
	    برو theend;
# endif
	اگر (wtime == -1 && ++شمار == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	برو theend;
    }
#endif

    // If we are going to wait for some time or block...
    اگر (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(تهی)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	اگر ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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
    اگر (gui.in_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif
#ifndef NO_CONSOLE
# ifdef FEAT_GUI
    وگرنه
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif

    اگر (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(تهی)vim_handle_signal(SIGNAL_BLOCK);

    ctrl_c_interrupts = TRUE;

#ifdef NO_CONSOLE_درون_داد
theend:
#endif
#ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#endif
    بازگشت retval;
}

#اگر defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    صحیح
inchar_loop(
    char_u	*buf,
    صحیح		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    صحیح		tb_change_cnt,
    صحیح		(*wait_func)(long wtime, صحیح *interrupted, صحیح ignore_درون_داد),
    صحیح		(*resize_func)(صحیح check_only))
{
    صحیح		len;
    صحیح		interrupted = FALSE;
    صحیح		did_call_wait_func = FALSE;
    صحیح		did_start_blocking = FALSE;
    long	wait_time;
    long	elapsed_time = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
# endif

    // repeat until we got a character or waited long enough
    برای (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	اگر (resize_func != پوچ)
	    resize_func(FALSE);

# ifdef MESSAGE_QUEUE
	// Only process messages when waiting.
	اگر (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    اگر (typebuf_changed(tb_change_cnt))
		بازگشت 0;
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
		&& (get_real_state() & MODE_درج) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	long acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	اگر (wtime < 0 && did_start_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_time = -1;
	وگرنه
	{
# ifdef ELAPSED_FUNC
	    elapsed_time = ELAPSED_FUNC(start_tv);
# endif
	    اگر (wtime >= 0)
		wait_time = wtime - elapsed_time;
	    وگرنه اگر (delay_pending)
	    {
		long delay_چپ = p_acl - acl_elapsed;
		long ut_چپ = p_ut - elapsed_time;

		اگر (did_start_blocking)
		    wait_time = delay_چپ;
		وگرنه
		    wait_time = MIN(delay_چپ, ut_چپ);
	    }
	    وگرنه
		// going to block after p_ut
		wait_time = p_ut - elapsed_time;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    اگر (wait_time <= 0 && did_call_wait_func)
	    {
		اگر (wtime >= 0)
		    // no character available within "wtime"
		    بازگشت 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		اگر (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    اگر (buf == پوچ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (صحیح)KE_COMPLETE_DELAY;
			add_to_درون_داد_buf(ibuf, 3);
		    }
		    وگرنه
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (صحیح)KE_COMPLETE_DELAY;
		    }
		    بازگشت 3;
		}

		// No character available within 'updatetime'.
		did_start_blocking = TRUE;
		اگر (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    اگر (buf == پوچ)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (صحیح)KE_CURSORHOLD;
			add_to_درون_داد_buf(ibuf, 3);
		    }
		    وگرنه
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (صحیح)KE_CURSORHOLD;
		    }
		    بازگشت 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		ادامه;
	    }
	}

# ifdef FEAT_JOB_CHANNEL
	اگر (wait_time < 0 || wait_time > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    اگر (has_pending_job())
		wait_time = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    اگر (channel_any_readahead())
		wait_time = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	اگر (p_beval && wait_time > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_time = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	اگر (wait_func(wait_time, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    اگر (typebuf_changed(tb_change_cnt))
		بازگشت 0;

	    // We might have something to return now.
	    اگر (buf == پوچ)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		بازگشت درون_داد_available();

	    len = read_from_درون_داد_buf(buf, (long)maxlen);
	    اگر (len > 0)
		بازگشت len;
	    ادامه;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_time += wait_time;
# endif

	اگر ((resize_func != پوچ && resize_func(TRUE))
# اگر defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
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
	    ادامه;

	// no character available or interrupted, return zero
	بشکن;
    }
    بازگشت 0;
}
#endif

#اگر defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    صحیح
ui_wait_for_chars_or_timer(
    long    wtime,
    صحیح	    (*wait_func)(long wtime, صحیح *interrupted, صحیح ignore_درون_داد),
    صحیح	    *interrupted,
    صحیح	    ignore_درون_داد)
{
    صحیح	    due_time;
    long    remaining = wtime;
    صحیح	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_JOB_CHANNEL
    صحیح	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    اگر (wtime >= 0 && wtime < 10L)
	بازگشت wait_func(wtime, پوچ, ignore_درون_داد);

    وقتی (wtime < 0 || remaining > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_time = check_due_timer();
	اگر (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    بازگشت FAIL;
	}
	اگر (due_time <= 0 || (wtime > 0 && due_time > remaining))
	    due_time = remaining;
# اگر defined(FEAT_JOB_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	اگر ((due_time < 0 || due_time > 10L) && (
#  اگر defined(FEAT_JOB_CHANNEL)
		(
#   اگر defined(FEAT_GUI)
		!gui.in_use &&
#   endif
		(has_pending_job() || channel_any_readahead()))
#   اگر defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  اگر defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
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
	اگر (wait_func(due_time, interrupted, ignore_درون_داد))
	    بازگشت OK;
	اگر ((interrupted != پوچ && *interrupted)
# ifdef FEAT_JOB_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    بازگشت FAIL;
	اگر (wtime > 0)
	    remaining -= due_time;
    }
    بازگشت FAIL;
}
#endif

/*
 * Return non-zero if a character is available.
 */
    صحیح
ui_char_avail(تهی)
{
#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	gui_mch_update();
	بازگشت درون_داد_available();
    }
#endif
#ifndef NO_CONSOLE
# ifdef NO_CONSOLE_درون_داد
    اگر (no_console_درون_داد())
	بازگشت 0;
# endif
    بازگشت mch_char_avail();
#وگرنه
    بازگشت 0;
#endif
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    تهی
ui_delay(long msec_arg, صحیح ignoreinput)
{
    long msec = msec_arg;

#ifdef FEAT_EVAL
    اگر (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(پوچ, "ui_delay(%ld)", msec);
#endif
#ifdef FEAT_GUI
    اگر (gui.in_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    وگرنه
#endif
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    تهی
ui_suspend(تهی)
{
#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	gui_mch_iconify();
	بازگشت;
    }
#endif
    mch_suspend();
}

#اگر !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    تهی
suspend_shell(تهی)
{
    اگر (*p_sh == NUL)
	emsg(_(e_shell_option_is_empty));
    وگرنه
    {
	پیام_puts(_("new shell started\n"));
	do_shell(پوچ, 0);
    }
}
#endif

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    صحیح
ui_get_shellsize(تهی)
{
    صحیح	    retval;

#ifdef FEAT_GUI
    اگر (gui.in_use)
	retval = gui_get_shellsize();
    وگرنه
#endif
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    اگر (retval == OK)
    {
	set_عدد_default("lines", Rows);
	set_عدد_default("columns", Columns);
    }
    بازگشت retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    تهی
ui_set_shellsize(
    صحیح		mustset UNUSED)	// set by the user
{
#ifdef FEAT_GUI
    اگر (gui.in_use)
	gui_set_shellsize(mustset, TRUE, RESIZE_BOTH);
    وگرنه
#endif
	mch_set_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    تهی
ui_new_shellsize(تهی)
{
    اگر (full_screen && !exiting)
    {
#ifdef FEAT_GUI
	اگر (gui.in_use)
	    gui_new_shellsize();
	وگرنه
#endif
	    mch_new_shellsize();
    }
}

#اگر (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    صحیح
ui_get_winpos(صحیح *x, صحیح *y, varnumber_T timeout UNUSED)
{
# ifdef FEAT_GUI
    اگر (gui.in_use)
	بازگشت gui_mch_get_winpos(x, y);
# endif
# اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    بازگشت mch_get_winpos(x, y);
# وگرنه
#  اگر defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    بازگشت term_get_winpos(x, y, timeout);
#  وگرنه
    بازگشت FAIL;
#  endif
# endif
}
#endif

    تهی
ui_breakcheck(تهی)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    تهی
ui_breakcheck_force(صحیح force)
{
    ایستا صحیح	recursive = FALSE;
    صحیح		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    اگر (recursive)
	بازگشت;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ifdef FEAT_GUI
    اگر (gui.in_use)
	gui_mch_update();
    وگرنه
#endif
	mch_breakcheck(force);

    اگر (save_updating_screen)
	updating_screen = TRUE;
    وگرنه
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

#اگر defined(USE_درون_داد_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# اگر defined(FEAT_JOB_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# وگرنه
#  define INBUFLEN 250
# endif

ایستا char_u	inbuf[INBUFLEN + MAX_کلید_CODE_LEN];
ایستا صحیح	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    صحیح
vim_is_درون_داد_buf_full(تهی)
{
    بازگشت (inbufcount >= INBUFLEN);
}

    صحیح
vim_is_درون_داد_buf_empty(تهی)
{
    بازگشت (inbufcount == 0);
}

# اگر defined(FEAT_OLE)
    صحیح
vim_free_in_درون_داد_buf(تهی)
{
    بازگشت (INBUFLEN - inbufcount);
}
# endif

# اگر defined(FEAT_GUI_GTK)
    صحیح
vim_used_in_درون_داد_buf(تهی)
{
    بازگشت inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_درون_داد_buf(تهی)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    اگر (gap != پوچ)
    {
	// Add one to avoid a zero size.
	gap->ga_data = alloc(inbufcount + 1);
	اگر (gap->ga_data != پوچ)
	    mch_memmove(gap->ga_data, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_درون_داد_buf();
    بازگشت (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    تهی
set_درون_داد_buf(char_u *p, صحیح overwrite)
{
    garray_T	*gap = (garray_T *)p;

    اگر (gap == پوچ)
	بازگشت;

    اگر (gap->ga_data != پوچ)
    {
	اگر (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_data, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	وگرنه
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
    تهی
add_to_درون_داد_buf(char_u *s, صحیح len)
{
    اگر (inbufcount + len > INBUFLEN + MAX_کلید_CODE_LEN)
	بازگشت;	    // Shouldn't ever happen!

    وقتی (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    تهی
add_to_درون_داد_buf_csi(char_u *str, صحیح len)
{
    صحیح		i;
    char_u	buf[2];

    برای (i = 0; i < len; ++i)
    {
	add_to_درون_داد_buf(str + i, 1);
	اگر (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (صحیح)KE_CSI;
	    add_to_درون_داد_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    تهی
trash_درون_داد_buf(تهی)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    صحیح
read_from_درون_داد_buf(char_u *buf, long maxlen)
{
    اگر (inbufcount == 0)	// if the buffer is empty, fill it
	fill_درون_داد_buf(TRUE);
    اگر (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    اگر (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    بازگشت (صحیح)maxlen;
}

    تهی
fill_درون_داد_buf(صحیح exit_on_error UNUSED)
{
# اگر defined(UNIX) || defined(VMS) || defined(MACOS_X)
    صحیح		len;
    صحیح		try;
    ایستا صحیح	did_read_something = FALSE;
    ایستا char_u *rest = پوچ;	    // unconverted rest of previous read
    ایستا صحیح	restlen = 0;
    صحیح		unconverted;
# endif

# ifdef FEAT_GUI
    اگر (gui.in_use
#  ifdef NO_CONSOLE_درون_داد
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_درون_داد()
#  endif
       )
    {
	gui_mch_update();
	بازگشت;
    }
# endif
# اگر defined(UNIX) || defined(VMS) || defined(MACOS_X)
    اگر (vim_is_درون_داد_buf_full())
	بازگشت;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    اگر (rest != پوچ)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	اگر (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	وگرنه
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	اگر (unconverted == restlen)
	    VIM_CLEAR(rest);
	وگرنه
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    وگرنه
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    برای (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / درون_داد_conv.vc_factor);
#  ifdef VMS
	len = vms_read((نویسه *)inbuf + inbufcount, readlen);
#  وگرنه
	len = اقرأ(read_cmd_fd, (نویسه *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	اگر (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(پوچ, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	اگر (len > 0 || got_int)
	    بشکن;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	اگر (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    صحیح m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  وگرنه
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	اگر (!exit_on_error)
	    بازگشت;
    }
    اگر (len <= 0 && !got_int)
	read_error_exit();
    اگر (len > 0)
	did_read_something = TRUE;
    اگر (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    وگرنه
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	اگر (درون_داد_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_درون_داد_safe(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == پوچ ? &rest : پوچ, &restlen);
	}
	وقتی (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    اگر (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !کلید_protocol_enabled())
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
    تهی
read_error_exit(تهی)
{
    اگر (silent_mode)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#اگر defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    تهی
ui_cursor_shape_forced(صحیح forced)
{
# ifdef FEAT_GUI
    اگر (gui.in_use)
	gui_update_cursor_later();
    وگرنه
# endif
	term_cursor_mode(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_سطر(FALSE);
# endif
}

    تهی
ui_cursor_shape(تهی)
{
    ui_cursor_shape_forced(FALSE);
}
#endif

/*
 * Check bounds for column number
 */
    صحیح
check_col(صحیح col)
{
    اگر (col < 0)
	بازگشت 0;
    اگر (col >= screen_Columns)
	بازگشت screen_Columns - 1;
    بازگشت col;
}

/*
 * Check bounds for row number
 */
    صحیح
check_row(صحیح row)
{
    اگر (row < 0)
	بازگشت 0;
    اگر (row >= screen_Rows)
	بازگشت screen_Rows - 1;
    بازگشت row;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    long
scroll_سطر_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	col = 0;

    اگر (*p != NUL)
	برای (;;)
	{
	    صحیح	    w = chartabsize(p, col);
	    MB_PTR_ADV(p);
	    اگر (*p == NUL)		// don't count the last character
		بشکن;
	    col += w;
	}
    بازگشت col;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_find_longest_lnum(تهی)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    اگر (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == پوچ) &&
#endif
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_سطر_شمار + 1)
    {
	linenr_T    lnum;
	long	    n;
	long	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	برای (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_سطر_len(lnum);
	    اگر (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    وگرنه اگر (n == max && abs((صحیح)(lnum - curwin->w_cursor.lnum))
				     < abs((صحیح)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    وگرنه
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    بازگشت ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    تهی
ui_focus_change(
    صحیح		in_focus)	// TRUE if focus gained.
{
    ایستا time_t	last_time = (time_t)0;
    صحیح			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    اگر (in_focus && last_time + 2 < time(پوچ))
    {
	need_redraw = check_timestamps(
#ifdef FEAT_GUI
		gui.in_use
#وگرنه
		FALSE
#endif
		);
	last_time = time(پوچ);
    }

#ifdef FEAT_TERMINAL
    term_focus_change(in_focus);
#endif

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(in_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, پوچ, پوچ, FALSE, curbuf);

    اگر (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    اگر (need_maketitle)
	maketitle();
}

#اگر defined(HAVE_درون_داد_METHOD)
/*
 * Save current Input Method status to specified place.
 */
    تهی
im_save_status(long *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    اگر (!p_imdisable && کلید_Typed && !کلید_Stuffed
# ifdef FEAT_XIM
	    && xic != پوچ
# endif
# ifdef FEAT_GUI
	    && (!gui.in_use || gui.in_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	اگر (vgetc_im_active)
	    *psave = B_IMODE_IM;
	وگرنه اگر (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#endif
