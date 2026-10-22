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

#ضمن "vim.h"

    فراغ
ui_write(char_u *s, صحيح len, صحيح console اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use && !gui.dying && !gui.starting
# ifndef NO_CONSOLE
	    && !console
# endif
	    )
    {
	gui_write(s, len);
	إذا (p_wd)
	    gui_wait_for_chars(p_wd, typebuf.tb_change_cnt);
	أرجع;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    إذا (!(silent_گهڻائي && p_verbose == 0))
    {
# إذا !defined(MSWIN)
	char_u	*tofree = عدم;

	إذا (output_conv.vc_type != CONV_NONE)
	{
	    // Convert characters from 'encoding' to 'termencoding'.
	    tofree = string_convert(&output_conv, s, &len);
	    إذا (tofree != عدم)
		s = tofree;
	}
# endif

	mch_write(s, len);
# إذا defined(HAVE_FSYNC)
	إذا (console && s[len - 1] == '\n')
	    vim_fsync(1);
# endif

# إذا !defined(MSWIN)
	إذا (output_conv.vc_type != CONV_NONE)
	    vim_free(tofree);
# endif
    }
#جيڪڏهن_پورو
}

#إذا defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * When executing an external program, there may be some typed characters that
 * are not consumed by it.  Give them back to ui_inchar() and they are stored
 * here for the next call.
 */
ساكن char_u *ta_str = عدم;
ساكن صحيح ta_off;	// offset for next char to use when ta_str != NULL
ساكن صحيح ta_len;	// length of ta_str when it's not NULL

    فراغ
ui_inchar_undo(char_u *s, صحيح len)
{
    char_u  *new;
    صحيح	    newlen;

    newlen = len;
    إذا (ta_str != عدم)
	newlen += ta_len - ta_off;
    new = alloc(newlen);
    إذا (new == عدم)
	أرجع;

    إذا (ta_str != عدم)
    {
	mch_memmove(new, ta_str + ta_off, (size_t)(ta_len - ta_off));
	mch_memmove(new + ta_len - ta_off, s, (size_t)len);
	vim_free(ta_str);
    }
    وإلا
	mch_memmove(new, s, (size_t)len);
    ta_str = new;
    ta_len = newlen;
    ta_off = 0;
}
#جيڪڏهن_پورو

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
    صحيح
ui_inchar(
    char_u	*buf,
    صحيح		maxlen,
    ڊگهو	wtime,	    // don't use "time", MIPS cannot handle it
    صحيح		tb_change_cnt)
{
    صحيح		retval = 0;

#إذا defined(FEAT_GUI) && (defined(UNIX) || defined(VMS))
    /*
     * Use the typeahead if there is any.
     */
    إذا (ta_str != عدم)
    {
	إذا (maxlen >= ta_len - ta_off)
	{
	    mch_memmove(buf, ta_str + ta_off, (size_t)ta_len);
	    VIM_CLEAR(ta_str);
	    أرجع ta_len;
	}
	mch_memmove(buf, ta_str + ta_off, (size_t)maxlen);
	ta_off += maxlen;
	أرجع maxlen;
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    إذا (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل NO_CONSOLE_داخلا
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    إذا (no_console_داخلا())
    {
	ساكن صحيح ڳڻپ = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	إذا (retval > 0 || typebuf_بدليو(tb_change_cnt) || wtime >= 0)
	    اذهب theend;
# endif
	إذا (wtime == -1 && ++ڳڻپ == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	اذهب theend;
    }
#جيڪڏهن_پورو

    // If we are going to wait for some time or block...
    إذا (wtime == -1 || wtime > 100L)
    {
	// ... allow signals to kill us.
	(فراغ)vim_handle_signal(SIGNAL_UNBLOCK);

	// ... there is no need for CTRL-C to interrupt something, don't let
	// it set got_int when it was mapped.
	إذا ((mapped_ctrl_c | curbuf->b_mapped_ctrl_c) & get_real_state())
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

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل NO_CONSOLE
# ifdef FEAT_GUI
    وإلا
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#جيڪڏهن_پورو

    إذا (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(فراغ)vim_handle_signal(SIGNAL_بلاڪ);

    ctrl_c_interrupts = TRUE;

#جيڪڏهن_وصف_ٿيل NO_CONSOLE_داخلا
theend:
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    إذا (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#جيڪڏهن_پورو
    أرجع retval;
}

#إذا defined(UNIX) || defined(VMS) || defined(FEAT_GUI)
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
    صحيح
inchar_loop(
    char_u	*buf,
    صحيح		maxlen,
    ڊگهو	wtime,	    // don't use "time", MIPS cannot handle it
    صحيح		tb_change_cnt,
    صحيح		(*wait_func)(ڊگهو wtime, صحيح *interrupted, صحيح ignore_داخلا),
    صحيح		(*resize_func)(صحيح check_only))
{
    صحيح		len;
    صحيح		interrupted = FALSE;
    صحيح		did_call_wait_func = FALSE;
    صحيح		did_شروعات_blocking = FALSE;
    ڊگهو	wait_وقت;
    ڊگهو	elapsed_وقت = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	شروعات_tv;

    ELAPSED_INIT(شروعات_tv);
# endif

    // repeat until we got a character or waited long enough
    لكل (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	إذا (resize_func != عدم)
	    resize_func(FALSE);

# ifdef پيغام_قطار
	// Only process messages when waiting.
	إذا (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    إذا (typebuf_بدليو(tb_change_cnt))
		أرجع 0;
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
		&& !ins_compl_سرگرم()
		&& (get_real_state() & گهڻائي_وجهو) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	ڊگهو acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	إذا (wtime < 0 && did_شروعات_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_وقت = -1;
	وإلا
	{
# ifdef ELAPSED_FUNC
	    elapsed_وقت = ELAPSED_FUNC(شروعات_tv);
# endif
	    إذا (wtime >= 0)
		wait_وقت = wtime - elapsed_وقت;
	    وإلا إذا (delay_pending)
	    {
		ڊگهو delay_کاٻو = p_acl - acl_elapsed;
		ڊگهو ut_کاٻو = p_ut - elapsed_وقت;

		إذا (did_شروعات_blocking)
		    wait_وقت = delay_کاٻو;
		وإلا
		    wait_وقت = MIN(delay_کاٻو, ut_کاٻو);
	    }
	    وإلا
		// going to block after p_ut
		wait_وقت = p_ut - elapsed_وقت;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    إذا (wait_وقت <= 0 && did_call_wait_func)
	    {
		إذا (wtime >= 0)
		    // no character available within "wtime"
		    أرجع 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		إذا (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_بدليو(tb_change_cnt))
		{
		    إذا (buf == عدم)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (صحيح)KE_COMPLETE_DELAY;
			جوڙيو_منزل_داخلا_buf(ibuf, 3);
		    }
		    وإلا
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (صحيح)KE_COMPLETE_DELAY;
		    }
		    أرجع 3;
		}

		// No character available within 'updatetime'.
		did_شروعات_blocking = TRUE;
		إذا (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_بدليو(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    إذا (buf == عدم)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (صحيح)KE_CURSORHOLD;
			جوڙيو_منزل_داخلا_buf(ibuf, 3);
		    }
		    وإلا
		    {
			buf[0] = K_SPECIAL;
			buf[1] = KS_EXTRA;
			buf[2] = (صحيح)KE_CURSORHOLD;
		    }
		    أرجع 3;
		}

		// There is no character available within 'updatetime' seconds:
		// flush all the swap files to disk.  Also done when
		// interrupted by SIGWINCH.
		before_blocking();
		استمر;
	    }
	}

# ifdef FEAT_ڪم_CHANNEL
	إذا (wait_وقت < 0 || wait_وقت > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    إذا (has_pending_ڪم())
		wait_وقت = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    إذا (channel_any_readahead())
		wait_وقت = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	إذا (p_beval && wait_وقت > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_وقت = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	إذا (wait_func(wait_وقت, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    إذا (typebuf_بدليو(tb_change_cnt))
		أرجع 0;

	    // We might have something to return now.
	    إذا (buf == عدم)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		أرجع داخلا_موجود();

	    len = read_from_داخلا_buf(buf, (ڊگهو)maxlen);
	    إذا (len > 0)
		أرجع len;
	    استمر;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_وقت += wait_وقت;
# endif

	إذا ((resize_func != عدم && resize_func(TRUE))
# إذا defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
		||
		(clientserver_طريقو == CLIENTSERVER_طريقو_X11 &&
		 server_انتظار())
# endif
# ifdef پيغام_قطار
		|| interrupted
# endif
		|| wait_وقت > 0
		|| (wtime < 0 && !did_شروعات_blocking))
	    // no character available, but something to be done, keep going
	    استمر;

	// no character available or interrupted, return zero
	اكسر;
    }
    أرجع 0;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    صحيح
ui_wait_for_chars_or_timer(
    ڊگهو    wtime,
    صحيح	    (*wait_func)(ڊگهو wtime, صحيح *interrupted, صحيح ignore_داخلا),
    صحيح	    *interrupted,
    صحيح	    ignore_داخلا)
{
    صحيح	    due_وقت;
    ڊگهو    بچيل = wtime;
    صحيح	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_ڪم_CHANNEL
    صحيح	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    إذا (wtime >= 0 && wtime < 10L)
	أرجع wait_func(wtime, عدم, ignore_داخلا);

    طالما (wtime < 0 || بچيل > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_وقت = check_due_timer();
	إذا (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    أرجع FAIL;
	}
	إذا (due_وقت <= 0 || (wtime > 0 && due_وقت > بچيل))
	    due_وقت = بچيل;
# إذا defined(FEAT_ڪم_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	إذا ((due_وقت < 0 || due_وقت > 10L) && (
#  إذا defined(FEAT_ڪم_CHANNEL)
		(
#   إذا defined(FEAT_GUI)
		!gui.اندريون_use &&
#   endif
		(has_pending_ڪم() || channel_any_readahead()))
#   إذا defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
		||
#   endif
#  endif
#  إذا defined(FEAT_SOUND_CANBERRA) ||  defined(FEAT_SOUND_MACOSX)
		    has_any_sound_callback()
#  endif
		    ))
	{
	    // There is a pending job or channel, should return soon in order
	    // to handle them ASAP.  Do check for input briefly.
	    due_وقت = 10L;
#  ifdef FEAT_ڪم_CHANNEL
	    brief_wait = TRUE;
#  endif
	}
# endif
	إذا (wait_func(due_وقت, interrupted, ignore_داخلا))
	    أرجع OK;
	إذا ((interrupted != عدم && *interrupted)
# ifdef FEAT_ڪم_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    أرجع FAIL;
	إذا (wtime > 0)
	    بچيل -= due_وقت;
    }
    أرجع FAIL;
}
#جيڪڏهن_پورو

/*
 * Return non-zero if a character is available.
 */
    صحيح
ui_char_avail(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
    {
	gui_mch_update();
	أرجع داخلا_موجود();
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل NO_CONSOLE
# ifdef NO_CONSOLE_داخلا
    إذا (no_console_داخلا())
	أرجع 0;
# endif
    أرجع mch_char_avail();
#وإلا
    أرجع 0;
#جيڪڏهن_پورو
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    فراغ
ui_delay(ڊگهو msec_arg, صحيح ignoreinput)
{
    ڊگهو msec = msec_arg;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(عدم, "ui_delay(%ld)", msec);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    وإلا
#جيڪڏهن_پورو
	mch_delay(msec, ignoreinput ? MCH_DELAY_IGNOREINPUT : 0);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    فراغ
ui_suspend(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
    {
	gui_mch_iconify();
	أرجع;
    }
#جيڪڏهن_پورو
    mch_suspend();
}

#إذا !defined(UNIX) || !defined(SIGTSTP)
/*
 * When the OS can't really suspend, call this function to start a shell.
 * This is never called in the GUI.
 */
    فراغ
suspend_shell(فراغ)
{
    إذا (*p_sh == NUL)
	emsg(_(e_shell_option_is_خالي_جايون));
    وإلا
    {
	نياپو_puts(_("new shell started\n"));
	do_shell(عدم, 0);
    }
}
#جيڪڏهن_پورو

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    صحيح
ui_get_shellsize(فراغ)
{
    صحيح	    retval;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
	retval = gui_get_shellsize();
    وإلا
#جيڪڏهن_پورو
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    إذا (retval == OK)
    {
	سيٽ_عدد_default("lines", صفون);
	سيٽ_عدد_default("columns", Columns);
    }
    أرجع retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    فراغ
ui_سيٽ_shellsize(
    صحيح		mustset اڻ_استعمال_ٿيل)	// set by the user
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
	gui_سيٽ_shellsize(mustset, TRUE, RESIZE_BOTH);
    وإلا
#جيڪڏهن_پورو
	mch_سيٽ_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    فراغ
ui_new_shellsize(فراغ)
{
    إذا (ڀريل_screen && !exiting)
    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	إذا (gui.اندريون_use)
	    gui_new_shellsize();
	وإلا
#جيڪڏهن_پورو
	    mch_new_shellsize();
    }
}

#إذا (defined(FEAT_EVAL) || defined(FEAT_TERMINAL)) \
	    && (defined(FEAT_GUI) \
		|| defined(MSWIN) \
		|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)))
/*
 * Get the window position in pixels, if possible.
 * Return FAIL when not possible.
 */
    صحيح
ui_get_winpos(صحيح *x, صحيح *y, varnumber_T timeout اڻ_استعمال_ٿيل)
{
# ifdef FEAT_GUI
    إذا (gui.اندريون_use)
	أرجع gui_mch_get_winpos(x, y);
# endif
# إذا defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    أرجع mch_get_winpos(x, y);
# وإلا
#  إذا defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    أرجع رڪن_get_winpos(x, y, timeout);
#  وإلا
    أرجع FAIL;
#  endif
# endif
}
#جيڪڏهن_پورو

    فراغ
ui_breakcheck(فراغ)
{
    ui_breakcheck_force(FALSE);
}

/*
 * When "force" is true also check when the terminal is not in raw mode.
 * This is useful to read input on channels.
 */
    فراغ
ui_breakcheck_force(صحيح force)
{
    ساكن صحيح	recursive = FALSE;
    صحيح		save_updating_screen = updating_screen;

    // We could be called recursively if stderr is redirected, calling
    // fill_input_buf() calls settmode() when stdin isn't a tty.  settmode()
    // calls vgetorpeek() which calls ui_breakcheck() again.
    إذا (recursive)
	أرجع;
    recursive = TRUE;

    // We do not want gui_resize_shell() to redraw the screen here.
    ++updating_screen;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
	gui_mch_update();
    وإلا
#جيڪڏهن_پورو
	mch_breakcheck(force);

    إذا (save_updating_screen)
	updating_screen = TRUE;
    وإلا
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

#إذا defined(USE_داخلا_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# إذا defined(FEAT_ڪم_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# وإلا
#  define INBUFLEN 250
# endif

ساكن char_u	inbuf[INBUFLEN + MAX_ڪنجي_ڪوڊ_LEN];
ساكن صحيح	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    صحيح
vim_is_داخلا_buf_ڀريل(فراغ)
{
    أرجع (inbufcount >= INBUFLEN);
}

    صحيح
vim_is_داخلا_buf_خالي_جايون(فراغ)
{
    أرجع (inbufcount == 0);
}

# إذا defined(FEAT_OLE)
    صحيح
vim_free_اندريون_داخلا_buf(فراغ)
{
    أرجع (INBUFLEN - inbufcount);
}
# endif

# إذا defined(FEAT_GUI_GTK)
    صحيح
vim_استعمال_ٿيل_اندريون_داخلا_buf(فراغ)
{
    أرجع inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_داخلا_buf(فراغ)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    إذا (gap != عدم)
    {
	// Add one to avoid a zero size.
	gap->ga_معلومات = alloc(inbufcount + 1);
	إذا (gap->ga_معلومات != عدم)
	    mch_memmove(gap->ga_معلومات, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_داخلا_buf();
    أرجع (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    فراغ
سيٽ_داخلا_buf(char_u *p, صحيح overwrite)
{
    garray_T	*gap = (garray_T *)p;

    إذا (gap == عدم)
	أرجع;

    إذا (gap->ga_معلومات != عدم)
    {
	إذا (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_معلومات, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	وإلا
	{
	    mch_memmove(inbuf + gap->ga_len, inbuf, inbufcount);
	    mch_memmove(inbuf, gap->ga_معلومات, gap->ga_len);
	    inbufcount += gap->ga_len;
	}
	vim_free(gap->ga_معلومات);
    }
    vim_free(gap);
}

/*
 * Add the given bytes to the input buffer
 * Special keys start with CSI.  A real CSI must have been translated to
 * CSI KS_EXTRA KE_CSI.  K_SPECIAL doesn't require translation.
 */
    فراغ
جوڙيو_منزل_داخلا_buf(char_u *s, صحيح len)
{
    إذا (inbufcount + len > INBUFLEN + MAX_ڪنجي_ڪوڊ_LEN)
	أرجع;	    // Shouldn't ever happen!

    طالما (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    فراغ
جوڙيو_منزل_داخلا_buf_csi(char_u *str, صحيح len)
{
    صحيح		i;
    char_u	buf[2];

    لكل (i = 0; i < len; ++i)
    {
	جوڙيو_منزل_داخلا_buf(str + i, 1);
	إذا (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (صحيح)KE_CSI;
	    جوڙيو_منزل_داخلا_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    فراغ
trash_داخلا_buf(فراغ)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    صحيح
read_from_داخلا_buf(char_u *buf, ڊگهو maxlen)
{
    إذا (inbufcount == 0)	// if the buffer is empty, fill it
	fill_داخلا_buf(TRUE);
    إذا (maxlen > inbufcount)
	maxlen = inbufcount;
    mch_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    // check "maxlen" to avoid clang warning
    إذا (inbufcount > 0 && maxlen > 0)
	mch_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    أرجع (صحيح)maxlen;
}

    فراغ
fill_داخلا_buf(صحيح exit_on_error اڻ_استعمال_ٿيل)
{
# إذا defined(UNIX) || defined(VMS) || defined(MACOS_X)
    صحيح		len;
    صحيح		try;
    ساكن صحيح	did_read_something = FALSE;
    ساكن char_u *rest = عدم;	    // unconverted rest of previous read
    ساكن صحيح	restlen = 0;
    صحيح		unconverted;
# endif

# ifdef FEAT_GUI
    إذا (gui.اندريون_use
#  ifdef NO_CONSOLE_داخلا
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_داخلا()
#  endif
       )
    {
	gui_mch_update();
	أرجع;
    }
# endif
# إذا defined(UNIX) || defined(VMS) || defined(MACOS_X)
    إذا (vim_is_داخلا_buf_ڀريل())
	أرجع;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    إذا (rest != عدم)
    {
	// Use remainder of previous call, starts with an invalid character
	// that may become valid when reading more.
	إذا (restlen > INBUFLEN - inbufcount)
	    unconverted = INBUFLEN - inbufcount;
	وإلا
	    unconverted = restlen;
	mch_memmove(inbuf + inbufcount, rest, unconverted);
	إذا (unconverted == restlen)
	    VIM_CLEAR(rest);
	وإلا
	{
	    restlen -= unconverted;
	    mch_memmove(rest, rest + unconverted, restlen);
	}
	inbufcount += unconverted;
    }
    وإلا
	unconverted = 0;

    len = 0;	// to avoid gcc warning
    لكل (try = 0; try < 100; ++try)
    {
	size_t readlen = (size_t)((INBUFLEN - inbufcount)
						       / داخلا_conv.vc_ضرب_ڪندڙ);
#  ifdef VMS
	len = vms_read((حرف *)inbuf + inbufcount, readlen);
#  وإلا
	len = اقرأ(read_cmd_fd, (حرف *)inbuf + inbufcount, readlen);
#  endif
#  ifdef FEAT_EVAL
	إذا (len > 0)
	{
	    inbuf[inbufcount + len] = NUL;
	    ch_log(عدم, "raw key input: \"%s\"", inbuf + inbufcount);
	}
#  endif

	إذا (len > 0 || got_int)
	    اكسر;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	إذا (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	{
	    صحيح m = cur_tmode;

	    // We probably set the wrong file descriptor to raw mode.  Switch
	    // back to cooked mode, use another descriptor and set the mode to
	    // what it was.
	    settmode(TMODE_COOK);
#  ifdef HAVE_DUP
	    // Use stderr for stdin, also works for shell commands.
	    close(0);
	    vim_ignored = dup(2);
#  وإلا
	    read_cmd_fd = 2;	// read from stderr instead of stdin
#  endif
	    settmode(m);
	}
	إذا (!exit_on_error)
	    أرجع;
    }
    إذا (len <= 0 && !got_int)
	read_error_exit();
    إذا (len > 0)
	did_read_something = TRUE;
    إذا (got_int)
    {
	// Interrupted, pretend a CTRL-C was typed.
	inbuf[0] = 3;
	inbufcount = 1;
    }
    وإلا
    {
	/*
	 * May perform conversion on the input characters.
	 * Include the unconverted rest of the previous call.
	 * If there is an incomplete char at the end it is kept for the next
	 * time, reading more bytes should make conversion possible.
	 * Don't do this in the unlikely event that the input buffer is too
	 * small ("rest" still contains more bytes).
	 */
	إذا (داخلا_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_داخلا_محفوظ(inbuf + inbufcount,
				     len + unconverted, INBUFLEN - inbufcount,
				       rest == عدم ? &rest : عدم, &restlen);
	}
	طالما (len > 0)
	{
	    // If a CTRL-C was typed, remove it from the buffer and set
	    // got_int.  Also recognize CTRL-C with modifyOtherKeys set, lower
	    // and upper case, in two forms.
	    // If terminal key protocols are in use, we expect to receive
	    // Ctrl_C as an escape sequence, ignore a raw Ctrl_C as this could
	    // be paste data.
	    إذا (ctrl_c_interrupts
			&& ((inbuf[inbufcount] == Ctrl_C && !ڪنجي_protocol_enabled())
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
#جيڪڏهن_پورو // USE_INPUT_BUF

/*
 * Exit because of an input read error.
 */
    فراغ
read_error_exit(فراغ)
{
    إذا (silent_گهڻائي)	// Normal way to exit for "ex -s"
	getout(0);
    STRCPY(IObuff, _("Vim: Error reading input, exiting...\n"));
    preserve_exit();
}

#إذا defined(CURSOR_SHAPE)
/*
 * May update the shape of the cursor.
 */
    فراغ
ui_cursor_shape_forced(صحيح forced)
{
# ifdef FEAT_GUI
    إذا (gui.اندريون_use)
	gui_update_cursor_later();
    وإلا
# endif
	رڪن_cursor_گهڻائي(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_سٽ(FALSE);
# endif
}

    فراغ
ui_cursor_shape(فراغ)
{
    ui_cursor_shape_forced(FALSE);
}
#جيڪڏهن_پورو

/*
 * Check bounds for column number
 */
    صحيح
check_کانو(صحيح کانو)
{
    إذا (کانو < 0)
	أرجع 0;
    إذا (کانو >= screen_Columns)
	أرجع screen_Columns - 1;
    أرجع کانو;
}

/*
 * Check bounds for row number
 */
    صحيح
check_صف(صحيح صف)
{
    إذا (صف < 0)
	أرجع 0;
    إذا (صف >= screen_صفون)
	أرجع screen_صفون - 1;
    أرجع صف;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    ڊگهو
scroll_سٽ_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	کانو = 0;

    إذا (*p != NUL)
	لكل (;;)
	{
	    صحيح	    w = chartabsize(p, کانو);
	    MB_PTR_ADV(p);
	    إذا (*p == NUL)		// don't count the last character
		اكسر;
	    کانو += w;
	}
    أرجع کانو;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_ڳوليو_longest_lnum(فراغ)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    إذا (
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    (!gui.اندريون_use || vim_strchr(p_go, GO_HORSCROLL) == عدم) &&
#جيڪڏهن_پورو
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_سٽ_ڳڻپ + 1)
    {
	linenr_T    lnum;
	ڊگهو	    n;
	ڊگهو	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	لكل (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_سٽ_len(lnum);
	    إذا (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    وإلا إذا (n == max && مطلق_قدر((صحيح)(lnum - curwin->w_cursor.lnum))
				     < مطلق_قدر((صحيح)(ret - curwin->w_cursor.lnum)))
		ret = lnum;
	}
    }
    وإلا
	// Use cursor line only.
	ret = curwin->w_cursor.lnum;

    أرجع ret;
}

/*
 * Called when focus changed.  Used for the GUI or for systems where this can
 * be done in the console (Win32).
 */
    فراغ
ui_focus_change(
    صحيح		اندريون_focus)	// TRUE if focus gained.
{
    ساكن time_t	آخري_وقت = (time_t)0;
    صحيح			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    إذا (اندريون_focus && آخري_وقت + 2 < وقت(عدم))
    {
	need_redraw = check_timestamps(
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		gui.اندريون_use
#وإلا
		FALSE
#جيڪڏهن_پورو
		);
	آخري_وقت = وقت(عدم);
    }

#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    رڪن_focus_change(اندريون_focus);
#جيڪڏهن_پورو

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(اندريون_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, عدم, عدم, FALSE, curbuf);

    إذا (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    إذا (need_maketitle)
	maketitle();
}

#إذا defined(HAVE_داخلا_طريقو)
/*
 * Save current Input Method status to specified place.
 */
    فراغ
im_save_حالت(ڊگهو *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    إذا (!p_imdisable && ڪنجي_Typed && !ڪنجي_Stuffed
# ifdef FEAT_XIM
	    && xic != عدم
# endif
# ifdef FEAT_GUI
	    && (!gui.اندريون_use || gui.اندريون_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	إذا (vgetc_im_سرگرم)
	    *psave = B_IMODE_IM;
	وإلا إذا (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#جيڪڏهن_پورو
