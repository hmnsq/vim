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

#include "vim.h"

    void
ui_write(char_u *s, puur_naa_nka len, puur_naa_nka console UNUSED)
{
#ifdef FEAT_GUI
    ᨀᨘ (gui.in_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	ᨀᨘ (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	ᨀᨘᨒ;
    }
#endif
#ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    ᨀᨘ (!(silent_mode && p_verbose == 0))
    {
# ᨀᨘ !defined(MSWIN)
	char_u	*tofree = NULL;

	ᨀᨘ (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    ᨀᨘ (tofree != NULL)
		s = tofree;
	}
# endif

	mch_write(s, len);
# ᨀᨘ defined(HAVE_FSYNC)
	ᨀᨘ (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# ᨀᨘ !defined(MSWIN)
	ᨀᨘ (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#endif
}

#ᨀᨘ defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
static char_u *ta_str = NULL;
static puur_naa_nka ta_off;	// offset for next char to use when ta_str != NULL
static puur_naa_nka ta_len;	// length of ta_str when it's not NULL

    void
ui_inchar_undo(char_u *s, puur_naa_nka len)
{
    char_u  *new;
    puur_naa_nka	    newlen;

    newlen = len;
    ᨀᨘ (ta_str != NULL)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    ᨀᨘ (new == NULL)
	ᨀᨘᨒ;

    ᨀᨘ (ta_str != NULL)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    ᨀᨘᨁ
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
    puur_naa_nka
ui_inchar(
    char_u	*buf,
    puur_naa_nka		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    puur_naa_nka		tb_change_cnt)
{
    puur_naa_nka		retval = 0;

#ᨀᨘ defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    ᨀᨘ (ta_str != NULL)
    {
	ᨀᨘ (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    ᨀᨘᨒ ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	ᨀᨘᨒ maxlen;
    }
#endif

#ifdef FEAT_PROFILE
    ᨀᨘ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#endif

#ifdef NO_CONSOLE_INPUT
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    ᨀᨘ (no_console_input())
    {
	static puur_naa_nka count = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	ᨀᨘ (retval > 0 || typebuf_changed(tb_change_cnt) || wtime >= 0)
	    goto theend;
# endif
	ᨀᨘ (wtime == -1 && ++count == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	goto theend;
    }
#endif

    // If we are going to wait for some time or block...
    ᨀᨘ (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(void)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	ᨀᨘ ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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
    ᨀᨘ (gui.in_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif
#ifndef NO_CONSOLE
# ifdef FEAT_GUI
    ᨀᨘᨁ
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#endif

    ᨀᨘ (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(void)vim_handle_signal(SIGNAL_BLOCK);

    ctrl_c_interrupts = TRUE;

#ifdef NO_CONSOLE_INPUT
theend:
#endif
#ifdef FEAT_PROFILE
    ᨀᨘ (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#endif
    ᨀᨘᨒ retval;
}

#ᨀᨘ defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    puur_naa_nka
inchar_loop(
    char_u	*buf,
    puur_naa_nka		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    puur_naa_nka		tb_change_cnt,
    puur_naa_nka		(*wait_func)(long wtime, puur_naa_nka *interrupted, puur_naa_nka ignore_input),
    puur_naa_nka		(*resize_func)(puur_naa_nka check_only))
{
    puur_naa_nka		len;
    puur_naa_nka		interrupted = FALSE;
    puur_naa_nka		did_call_wait_func = FALSE;
    puur_naa_nka		did_start_blocking = FALSE;
    long	wait_time;
    long	elapsed_time = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
# endif

    // repeat until we got a character or waited long enough
    for (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	ᨀᨘ (resize_func != NULL)
	    resize_func(FALSE);

# ifdef MESSAGE_QUEUE
	// Only process messages when waiting.
	ᨀᨘ (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    ᨀᨘ (typebuf_changed(tb_change_cnt))
		ᨀᨘᨒ 0;
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
		&& (get_real_state() & MODE_INSERT) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	long acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	ᨀᨘ (wtime < 0 && did_start_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_time = -1;
	ᨀᨘᨁ
	{
# ifdef ELAPSED_FUNC
	    elapsed_time = ELAPSED_FUNC(start_tv);
# endif
	    ᨀᨘ (wtime >= 0)
		wait_time = wtime - elapsed_time;
	    ᨀᨘᨁ ᨀᨘ (delay_pending)
	    {
		long delay_left = p_acl - acl_elapsed;
		long ut_left = p_ut - elapsed_time;

		ᨀᨘ (did_start_blocking)
		    wait_time = delay_left;
		ᨀᨘᨁ
		    wait_time = MIN(delay_left, ut_left);
	    }
	    ᨀᨘᨁ
		// going to block after p_ut
		wait_time = p_ut - elapsed_time;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    ᨀᨘ (wait_time <= 0 && did_call_wait_func)
	    {
		ᨀᨘ (wtime >= 0)
		    // no character available within "wtime"
		    ᨀᨘᨒ 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		ᨀᨘ (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    ᨀᨘ (buf == NULL)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (puur_naa_nka)KE_COMPLETE_DELAY;
			add_to_input_buf(ibuf, 3);
		    }
		    ᨀᨘᨁ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (puur_naa_nka)KE_COMPLETE_DELAY;
		    }
		    ᨀᨘᨒ 3;
		}

		// No character available within 'updatetime'.
		did_start_blocking = TRUE;
		ᨀᨘ (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_changed(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    ᨀᨘ (buf == NULL)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (puur_naa_nka)KE_CURSORHOLD;
			add_to_input_buf(ibuf, 3);
		    }
		    ᨀᨘᨁ
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (puur_naa_nka)KE_CURSORHOLD;
		    }
		    ᨀᨘᨒ 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		continue;
	    }
	}

# ifdef FEAT_JOB_CHANNEL
	ᨀᨘ (wait_time < 0 || wait_time > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    ᨀᨘ (has_pending_job())
		wait_time = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    ᨀᨘ (channel_any_readahead())
		wait_time = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	ᨀᨘ (p_beval && wait_time > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_time = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	ᨀᨘ (wait_func(wait_time, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    ᨀᨘ (typebuf_changed(tb_change_cnt))
		ᨀᨘᨒ 0;

	    // We might have something to return now.
	    ᨀᨘ (buf == NULL)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		ᨀᨘᨒ input_available();

	    len = read_from_input_buf(buf, (long)maxlen);
	    ᨀᨘ (len > 0)
		ᨀᨘᨒ len;
	    continue;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_time += wait_time;
# endif

	ᨀᨘ ((resize_func != NULL && resize_func(TRUE))
# ᨀᨘ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
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
	    continue;

	// no character available or interrupted, return zero
	bha_nga;
    }
    ᨀᨘᨒ 0;
}
#endif

#ᨀᨘ defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    puur_naa_nka
ui_wait_for_chars_or_timer(
    long    wtime,
    puur_naa_nka	    (*wait_func)(long wtime, puur_naa_nka *interrupted, puur_naa_nka ignore_input),
    puur_naa_nka	    *interrupted,
    puur_naa_nka	    ignore_input)
{
    puur_naa_nka	    due_time;
    long    remaining = wtime;
    puur_naa_nka	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_JOB_CHANNEL
    puur_naa_nka	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    ᨀᨘ (wtime >= 0 && wtime < 10L)
	ᨀᨘᨒ wait_func(wtime, NULL, ignore_input);

    ᨀᨛ (wtime < 0 || remaining > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_time = check_due_timer();
	ᨀᨘ (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    ᨀᨘᨒ FAIL;
	}
	ᨀᨘ (due_time <= 0 || (wtime > 0 && due_time > remaining))
	    due_time = remaining;
# ᨀᨘ defined(FEAT_JOB_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	ᨀᨘ ((due_time < 0 || due_time > 10L) && (
#  ᨀᨘ defined(FEAT_JOB_CHANNEL)
		(
#   ᨀᨘ defined(FEAT_GUI)
		!gui.in_use &&
#   endif
		(has_pending_job() || channel_any_readahead()))
#   ᨀᨘ defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  ᨀᨘ defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
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
	ᨀᨘ (wait_func(due_time, interrupted, ignore_input))
	    ᨀᨘᨒ OK;
	ᨀᨘ ((interrupted != NULL && *interrupted)
# ifdef FEAT_JOB_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    ᨀᨘᨒ FAIL;
	ᨀᨘ (wtime > 0)
	    remaining -= due_time;
    }
    ᨀᨘᨒ FAIL;
}
#endif

/*
 * Return non-zero if a character is available.
 */
    puur_naa_nka
ui_char_avail(void)
{
#ifdef FEAT_GUI
    ᨀᨘ (gui.in_use)
    {
	gui_mch_update();
	ᨀᨘᨒ input_available();
    }
#endif
#ifndef NO_CONSOLE
# ifdef NO_CONSOLE_INPUT
    ᨀᨘ (no_console_input())
	ᨀᨘᨒ 0;
# endif
    ᨀᨘᨒ mch_char_avail();
#ᨀᨘᨁ
    ᨀᨘᨒ 0;
#endif
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    void
ui_delay(long msec_arg, puur_naa_nka ignoreinput)
{
    long msec = msec_arg;

#ifdef FEAT_EVAL
    ᨀᨘ (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(NULL, "ui_delay(%ld)", msec);
#endif
#ifdef FEAT_GUI
    ᨀᨘ (gui.in_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    ᨀᨘᨁ
#endif
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    void
ui_suspend(void)
{
#ifdef FEAT_GUI
    ᨀᨘ (gui.in_use)
    {
	gui_mch_iconify();
	ᨀᨘᨒ;
    }
#endif
    mch_suspend();
}

#ᨀᨘ !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    void
suspend_shell(void)
{
    ᨀᨘ (*p_sh == NUL)
	emsg(_(e_shell_option_is_empty));
    ᨀᨘᨁ
    {
	msg_puts(_("new shell started\n"));
	do_shell(NULL, 0);
    }
}
#endif

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    puur_naa_nka
ui_get_shellsize(void)
{
    puur_naa_nka	    retval;

#ifdef FEAT_GUI
    ᨀᨘ (gui.in_use)
	retval = gui_get_shellsize();
    ᨀᨘᨁ
#endif
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    ᨀᨘ (retval == OK)
    {
	set_number_default("lines", Rows);
	set_number_default("columns", Columns);
    }
    ᨀᨘᨒ retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    void
ui_set_shellsize(
    puur_naa_nka		mustset UNUSED)	// set by the user
{
#ifdef FEAT_GUI
    ᨀᨘ (gui.in_use)
	gui_set_shellsize(mustset, TRUE, RESIZE_BOTH);
    ᨀᨘᨁ
#endif
	mch_set_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    void
ui_new_shellsize(void)
{
    ᨀᨘ (full_screen && !exiting)
    {
#ifdef FEAT_GUI
	ᨀᨘ (gui.in_use)
	    gui_new_shellsize();
	ᨀᨘᨁ
#endif
	    mch_new_shellsize();
    }
}

#ᨀᨘ (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    puur_naa_nka
ui_get_winpos(puur_naa_nka *x, puur_naa_nka *y, varnumber_T timeout UNUSED)
{
# ifdef FEAT_GUI
    ᨀᨘ (gui.in_use)
	ᨀᨘᨒ gui_mch_get_winpos(x, y);
# endif
# ᨀᨘ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    ᨀᨘᨒ mch_get_winpos(x, y);
# ᨀᨘᨁ
#  ᨀᨘ defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    ᨀᨘᨒ term_get_winpos(x, y, timeout);
#  ᨀᨘᨁ
    ᨀᨘᨒ FAIL;
#  endif
# endif
}
#endif

    void
ui_breakcheck(void)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    void
ui_breakcheck_force(puur_naa_nka force)
{
    static puur_naa_nka	recursive = FALSE;
    puur_naa_nka		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    ᨀᨘ (recursive)
	ᨀᨘᨒ;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#ifdef FEAT_GUI
    ᨀᨘ (gui.in_use)
	gui_mch_update();
    ᨀᨘᨁ
#endif
	mch_breakcheck(force);

    ᨀᨘ (save_updating_screen)
	updating_screen = TRUE;
    ᨀᨘᨁ
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

#ᨀᨘ defined(USE_INPUT_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# ᨀᨘ defined(FEAT_JOB_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# ᨀᨘᨁ
#  define INBUFLEN 250
# endif

static char_u	inbuf[INBUFLEN + MAX_KEY_CODE_LEN];
static puur_naa_nka	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    puur_naa_nka
vim_is_input_buf_full(void)
{
    ᨀᨘᨒ (inbufcount >= INBUFLEN);
}

    puur_naa_nka
vim_is_input_buf_empty(void)
{
    ᨀᨘᨒ (inbufcount == 0);
}

# ᨀᨘ defined(FEAT_OLE)
    puur_naa_nka
vim_free_in_input_buf(void)
{
    ᨀᨘᨒ (INBUFLEN - inbufcount);
}
# endif

# ᨀᨘ defined(FEAT_GUI_GTK)
    puur_naa_nka
vim_used_in_input_buf(void)
{
    ᨀᨘᨒ inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_input_buf(void)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    ᨀᨘ (gap != NULL)
    {
	// Add one to avoid a zero size.
	gap->ga_data = alloc(inbufcount + 1);
	ᨀᨘ (gap->ga_data != NULL)
	    mch_memmove(gap->ga_data, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_input_buf();
    ᨀᨘᨒ (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    void
set_input_buf(char_u *p, puur_naa_nka overwrite)
{
    garray_T	*gap = (garray_T *)p;

    ᨀᨘ (gap == NULL)
	ᨀᨘᨒ;

    ᨀᨘ (gap->ga_data != NULL)
    {
	ᨀᨘ (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_data, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	ᨀᨘᨁ
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
    void
add_to_input_buf(char_u *s, puur_naa_nka len)
{
    ᨀᨘ (inbufcount + len > INBUFLEN + MAX_KEY_CODE_LEN)
	ᨀᨘᨒ;	    // Shouldn't ever happen!

    ᨀᨛ (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    void
add_to_input_buf_csi(char_u *str, puur_naa_nka len)
{
    puur_naa_nka		i;
    char_u	buf[2];

    for (i = 0; i < len; ++i)
    {
	add_to_input_buf(str + i, 1);
	ᨀᨘ (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (puur_naa_nka)KE_CSI;
	    add_to_input_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    void
trash_input_buf(void)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    puur_naa_nka
read_from_input_buf(char_u *buf, long maxlen)
{
    ᨀᨘ (inbufcount == 0)	// if the buffer is empty, fill it
	fill_input_buf(TRUE);
    ᨀᨘ (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    ᨀᨘ (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    ᨀᨘᨒ (puur_naa_nka)maxlen;
}

    void
fill_input_buf(puur_naa_nka exit_on_error UNUSED)
{
# ᨀᨘ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    puur_naa_nka		len;
    puur_naa_nka		try;
    static puur_naa_nka	did_read_something = FALSE;
    static char_u *rest = NULL;	    // unconverted rest of previous read
    static puur_naa_nka	restlen = 0;
    puur_naa_nka		unconverted;
# endif

# ifdef FEAT_GUI
    ᨀᨘ (gui.in_use
#  ifdef NO_CONSOLE_INPUT
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_input()
#  endif
       )
    {
	gui_mch_update();
	ᨀᨘᨒ;
    }
# endif
# ᨀᨘ defined(UNIX) || defined(VMS) || defined(MACOS_X)
    ᨀᨘ (vim_is_input_buf_full())
	ᨀᨘᨒ;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    ᨀᨘ (rest != NULL)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	ᨀᨘ (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	ᨀᨘᨁ
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	ᨀᨘ (unconverted == restlen)
	    VIM_CLEAR(rest);
	ᨀᨘᨁ
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    ᨀᨘᨁ
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    for (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / input_conv.vc_factor);
#  ifdef VMS
	len = vms_read((var_na *)inbuf + inbufcount, readlen);
#  ᨀᨘᨁ
	len = read(read_cmd_fd, (var_na *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	ᨀᨘ (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(NULL, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	ᨀᨘ (len > 0 || got_int)
	    bha_nga;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	ᨀᨘ (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    puur_naa_nka m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  ᨀᨘᨁ
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	ᨀᨘ (!exit_on_error)
	    ᨀᨘᨒ;
    }
    ᨀᨘ (len <= 0 && !got_int)
	read_error_exit();
    ᨀᨘ (len > 0)
	did_read_something = TRUE;
    ᨀᨘ (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    ᨀᨘᨁ
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	ᨀᨘ (input_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_input_safe(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == NULL ? &rest : NULL, &restlen);
	}
	ᨀᨛ (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    ᨀᨘ (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !key_protocol_enabled())
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
    void
read_error_exit(void)
{
    ᨀᨘ (silent_mode)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#ᨀᨘ defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    void
ui_cursor_shape_forced(puur_naa_nka forced)
{
# ifdef FEAT_GUI
    ᨀᨘ (gui.in_use)
	gui_update_cursor_later();
    ᨀᨘᨁ
# endif
	term_cursor_mode(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_line(FALSE);
# endif
}

    void
ui_cursor_shape(void)
{
    ui_cursor_shape_forced(FALSE);
}
#endif

/*
 * Check bounds for column number
 */
    puur_naa_nka
check_col(puur_naa_nka col)
{
    ᨀᨘ (col < 0)
	ᨀᨘᨒ 0;
    ᨀᨘ (col >= screen_Columns)
	ᨀᨘᨒ screen_Columns - 1;
    ᨀᨘᨒ col;
}

/*
 * Check bounds for row number
 */
    puur_naa_nka
check_row(puur_naa_nka row)
{
    ᨀᨘ (row < 0)
	ᨀᨘᨒ 0;
    ᨀᨘ (row >= screen_Rows)
	ᨀᨘᨒ screen_Rows - 1;
    ᨀᨘᨒ row;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    long
scroll_line_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	col = 0;

    ᨀᨘ (*p != NUL)
	for (;;)
	{
	    puur_naa_nka	    w = chartabsize(p, col);
	    MB_PTR_ADV(p);
	    ᨀᨘ (*p == NUL)		// don't count the last character
		bha_nga;
	    col += w;
	}
    ᨀᨘᨒ col;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_find_longest_lnum(void)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    ᨀᨘ (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == NULL) &&
#endif
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_line_count + 1)
    {
	linenr_T    lnum;
	long	    n;
	long	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	for (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_line_len(lnum);
	    ᨀᨘ (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    ᨀᨘᨁ ᨀᨘ (n == max && abs((puur_naa_nka)(lnum - curwin->w_cursor.lnum))
				     < abs((puur_naa_nka)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    ᨀᨘᨁ
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    ᨀᨘᨒ ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    void
ui_focus_change(
    puur_naa_nka		in_focus)	// TRUE if focus gained.
{
    static time_t	last_time = (time_t)0;
    puur_naa_nka			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    ᨀᨘ (in_focus && last_time + 2 < time(NULL))
    {
	need_redraw = check_timestamps(
#ifdef FEAT_GUI
		gui.in_use
#ᨀᨘᨁ
		FALSE
#endif
		);
	last_time = time(NULL);
    }

#ifdef FEAT_TERMINAL
    term_focus_change(in_focus);
#endif

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(in_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, NULL, NULL, FALSE, curbuf);

    ᨀᨘ (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    ᨀᨘ (need_maketitle)
	maketitle();
}

#ᨀᨘ defined(HAVE_INPUT_METHOD)
/*
 * Save current Input Method status to specified place.
 */
    void
im_save_status(long *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    ᨀᨘ (!p_imdisable && KeyTyped && !KeyStuffed
# ifdef FEAT_XIM
	    && xic != NULL
# endif
# ifdef FEAT_GUI
	    && (!gui.in_use || gui.in_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	ᨀᨘ (vgetc_im_active)
	    *psave = B_IMODE_IM;
	ᨀᨘᨁ ᨀᨘ (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#endif
