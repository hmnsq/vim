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
ui_write(char_u *s, صحيح len, صحيح console استفادهنشده)
{
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use && !gui.dying && !gui.starting
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
#پایان_اگر
#اگر_تعریف_نشده NO_CONSOLE
    // Don't output anything in silent mode ("ex -s") unless 'verbose' set
    إذا (!(silent_نما && p_verbose == 0))
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
#پایان_اگر
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
#پایان_اگر

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
    دراز	wtime,	    // don't use "time", MIPS cannot handle it
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
#پایان_اگر

#اگر_تعریف_شده FEAT_PROFILE
    إذا (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_enter();
#پایان_اگر

#اگر_تعریف_شده NO_CONSOLE_درون_داد
    // Don't wait for character input when the window hasn't been opened yet.
    // Do try reading, this works when redirecting stdin from a file.
    // Must return something, otherwise we'll loop forever.  If we run into
    // this very often we probably got stuck, exit Vim.
    إذا (no_console_درون_داد())
    {
	ساكن صحيح شمار = 0;

# ifndef NO_CONSOLE
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
	إذا (retval > 0 || typebuf_تغییریافته(tb_change_cnt) || wtime >= 0)
	    اذهب theend;
# endif
	إذا (wtime == -1 && ++شمار == 1000)
	    read_error_exit();
	buf[0] = CAR;
	retval = 1;
	اذهب theend;
    }
#پایان_اگر

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

#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
	retval = gui_inchar(buf, maxlen, wtime, tb_change_cnt);
#پایان_اگر
#اگر_تعریف_نشده NO_CONSOLE
# ifdef FEAT_GUI
    وإلا
# endif
	retval = mch_inchar(buf, maxlen, wtime, tb_change_cnt);
#پایان_اگر

    إذا (wtime == -1 || wtime > 100L)
	// block SIGHUP et al.
	(فراغ)vim_handle_signal(SIGNAL_بلوک);

    ctrl_c_interrupts = TRUE;

#اگر_تعریف_شده NO_CONSOLE_درون_داد
theend:
#پایان_اگر
#اگر_تعریف_شده FEAT_PROFILE
    إذا (do_profiling == PROF_YES && wtime != 0)
	prof_inchar_exit();
#پایان_اگر
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
    دراز	wtime,	    // don't use "time", MIPS cannot handle it
    صحيح		tb_change_cnt,
    صحيح		(*wait_func)(دراز wtime, صحيح *interrupted, صحيح ignore_درون_داد),
    صحيح		(*resize_func)(صحيح check_only))
{
    صحيح		len;
    صحيح		interrupted = FALSE;
    صحيح		did_call_wait_func = FALSE;
    صحيح		did_شروع_blocking = FALSE;
    دراز	wait_زمان;
    دراز	elapsed_زمان = 0;
# ifdef ELAPSED_FUNC
    elapsed_T	شروع_tv;

    ELAPSED_INIT(شروع_tv);
# endif

    // repeat until we got a character or waited long enough
    لكل (;;)
    {
	// Check if window changed size while we were busy, perhaps the ":set
	// columns=99" command was used.
	إذا (resize_func != عدم)
	    resize_func(FALSE);

# ifdef پیام_صف
	// Only process messages when waiting.
	إذا (wtime != 0)
	{
	    parse_queued_messages();
	    // If input was put directly in typeahead buffer bail out here.
	    إذا (typebuf_تغییریافته(tb_change_cnt))
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
		&& !ins_compl_فعال()
		&& (get_real_state() & نما_درج) != 0;
	// Measure the delay from when it was armed (the keystroke), so a
	// CursorHold returning in between does not push the popup back.
	دراز acl_elapsed = delay_pending ? ins_compl_autocomplete_elapsed() : 0;

	إذا (wtime < 0 && did_شروع_blocking && !delay_pending)
	    // blocking and already waited for p_ut
	    wait_زمان = -1;
	وإلا
	{
# ifdef ELAPSED_FUNC
	    elapsed_زمان = ELAPSED_FUNC(شروع_tv);
# endif
	    إذا (wtime >= 0)
		wait_زمان = wtime - elapsed_زمان;
	    وإلا إذا (delay_pending)
	    {
		دراز delay_چپ = p_acl - acl_elapsed;
		دراز ut_چپ = p_ut - elapsed_زمان;

		إذا (did_شروع_blocking)
		    wait_زمان = delay_چپ;
		وإلا
		    wait_زمان = MIN(delay_چپ, ut_چپ);
	    }
	    وإلا
		// going to block after p_ut
		wait_زمان = p_ut - elapsed_زمان;

	    // If the waiting time is now zero or less, we timed out.  However,
	    // loop at least once to check for characters and events.  Matters
	    // when "wtime" is zero.
	    إذا (wait_زمان <= 0 && did_call_wait_func)
	    {
		إذا (wtime >= 0)
		    // no character available within "wtime"
		    أرجع 0;

		// The 'autocompletedelay' expired: trigger the popup.  When
		// 'updatetime' is shorter, fall through to CursorHold instead.
		إذا (delay_pending && acl_elapsed >= p_acl && maxlen >= 3
					    && !typebuf_تغییریافته(tb_change_cnt))
		{
		    إذا (buf == عدم)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (صحيح)KE_COMPLETE_DELAY;
			جمع_کن_مقصد_درون_داد_buf(ibuf, 3);
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
		did_شروع_blocking = TRUE;
		إذا (trigger_cursorhold() && maxlen >= 3
					    && !typebuf_تغییریافته(tb_change_cnt))
		{
		    // Put K_CURSORHOLD in the input buffer or return it.
		    إذا (buf == عدم)
		    {
			char_u	ibuf[3];

			ibuf[0] = CSI;
			ibuf[1] = KS_EXTRA;
			ibuf[2] = (صحيح)KE_CURSORHOLD;
			جمع_کن_مقصد_درون_داد_buf(ibuf, 3);
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

# ifdef FEAT_کار_CHANNEL
	إذا (wait_زمان < 0 || wait_زمان > 100L)
	{
	    // Checking if a job ended requires polling.  Do this at least
	    // every 100 msec.
	    إذا (has_pending_کار())
		wait_زمان = 100L;

	    // If there is readahead then parse_queued_messages() timed out and
	    // we should call it again soon.
	    إذا (channel_any_readahead())
		wait_زمان = 10L;
	}
# endif
# ifdef FEAT_BEVAL_GUI
	إذا (p_beval && wait_زمان > 100L)
	    // The 'balloonexpr' may indirectly invoke a callback while waiting
	    // for a character, need to check often.
	    wait_زمان = 100L;
# endif

	// Wait for a character to be typed or another event, such as the winch
	// signal or an event on the monitored file descriptors.
	did_call_wait_func = TRUE;
	إذا (wait_func(wait_زمان, &interrupted, FALSE))
	{
	    // If input was put directly in typeahead buffer bail out here.
	    إذا (typebuf_تغییریافته(tb_change_cnt))
		أرجع 0;

	    // We might have something to return now.
	    إذا (buf == عدم)
		// "buf" is NULL, we were just waiting, not actually getting
		// input.
		أرجع درون_داد_موجود();

	    len = read_from_درون_داد_buf(buf, (دراز)maxlen);
	    إذا (len > 0)
		أرجع len;
	    استمر;
	}
	// Timed out or interrupted with no character available.

# ifndef ELAPSED_FUNC
	// estimate the elapsed time
	elapsed_زمان += wait_زمان;
# endif

	إذا ((resize_func != عدم && resize_func(TRUE))
# إذا defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
		||
		(clientserver_روش == CLIENTSERVER_روش_X11 &&
		 server_انتظار())
# endif
# ifdef پیام_صف
		|| interrupted
# endif
		|| wait_زمان > 0
		|| (wtime < 0 && !did_شروع_blocking))
	    // no character available, but something to be done, keep going
	    استمر;

	// no character available or interrupted, return zero
	اكسر;
    }
    أرجع 0;
}
#پایان_اگر

#إذا defined(FEAT_TIMERS)
/*
 * Wait for a timer to fire or "wait_func" to return non-zero.
 * Returns OK when something was read.
 * Returns FAIL when it timed out or was interrupted.
 */
    صحيح
ui_wait_for_chars_or_timer(
    دراز    wtime,
    صحيح	    (*wait_func)(دراز wtime, صحيح *interrupted, صحيح ignore_درون_داد),
    صحيح	    *interrupted,
    صحيح	    ignore_درون_داد)
{
    صحيح	    due_زمان;
    دراز    مانده = wtime;
    صحيح	    tb_change_cnt = typebuf.tb_change_cnt;
# ifdef FEAT_کار_CHANNEL
    صحيح	    brief_wait = FALSE;
# endif

    // When waiting very briefly don't trigger timers.
    إذا (wtime >= 0 && wtime < 10L)
	أرجع wait_func(wtime, عدم, ignore_درون_داد);

    طالما (wtime < 0 || مانده > 0)
    {
	// Trigger timers and then get the time in wtime until the next one is
	// due.  Wait up to that time.
	due_زمان = check_due_timer();
	إذا (typebuf.tb_change_cnt != tb_change_cnt)
	{
	    // timer may have used feedkeys()
	    أرجع FAIL;
	}
	إذا (due_زمان <= 0 || (wtime > 0 && due_زمان > مانده))
	    due_زمان = مانده;
# إذا defined(FEAT_کار_CHANNEL) || defined(FEAT_SOUND_CANBERRA) || defined(FEAT_SOUND_MACOSX)
	إذا ((due_زمان < 0 || due_زمان > 10L) && (
#  إذا defined(FEAT_کار_CHANNEL)
		(
#   إذا defined(FEAT_GUI)
		!gui.درون_use &&
#   endif
		(has_pending_کار() || channel_any_readahead()))
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
	    due_زمان = 10L;
#  ifdef FEAT_کار_CHANNEL
	    brief_wait = TRUE;
#  endif
	}
# endif
	إذا (wait_func(due_زمان, interrupted, ignore_درون_داد))
	    أرجع OK;
	إذا ((interrupted != عدم && *interrupted)
# ifdef FEAT_کار_CHANNEL
		|| brief_wait
# endif
		)
	    // Nothing available, but need to return so that side effects get
	    // handled, such as handling a message on a channel.
	    أرجع FAIL;
	إذا (wtime > 0)
	    مانده -= due_زمان;
    }
    أرجع FAIL;
}
#پایان_اگر

/*
 * Return non-zero if a character is available.
 */
    صحيح
ui_char_avail(فراغ)
{
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
    {
	gui_mch_update();
	أرجع درون_داد_موجود();
    }
#پایان_اگر
#اگر_تعریف_نشده NO_CONSOLE
# ifdef NO_CONSOLE_درون_داد
    إذا (no_console_درون_داد())
	أرجع 0;
# endif
    أرجع mch_char_avail();
#وإلا
    أرجع 0;
#پایان_اگر
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    فراغ
ui_delay(دراز msec_arg, صحيح ignoreinput)
{
    دراز msec = msec_arg;

#اگر_تعریف_شده FEAT_EVAL
    إذا (ui_delay_for_testing > 0)
	msec = ui_delay_for_testing;
    ch_log(عدم, "ui_delay(%ld)", msec);
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use && !ignoreinput)
	gui_wait_for_chars(msec, typebuf.tb_change_cnt);
    وإلا
#پایان_اگر
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
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
    {
	gui_mch_iconify();
	أرجع;
    }
#پایان_اگر
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
	emsg(_(e_shell_option_is_خالی));
    وإلا
    {
	پیام_puts(_("new shell started\n"));
	do_shell(عدم, 0);
    }
}
#پایان_اگر

/*
 * Try to get the current Vim shell size.  Put the result in Rows and Columns.
 * Use the new sizes as defaults for 'columns' and 'lines'.
 * Return OK when size could be determined, FAIL otherwise.
 */
    صحيح
ui_get_shellsize(فراغ)
{
    صحيح	    retval;

#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
	retval = gui_get_shellsize();
    وإلا
#پایان_اگر
	retval = mch_get_shellsize();

    check_shellsize();

    // adjust the default for 'lines' and 'columns'
    إذا (retval == OK)
    {
	مجموعه_عدد_default("lines", ردیفها);
	مجموعه_عدد_default("columns", Columns);
    }
    أرجع retval;
}

/*
 * Set the size of the Vim shell according to Rows and Columns, if possible.
 * The gui_set_shellsize() or mch_set_shellsize() function will try to set the
 * new size.  If this is not possible, it will adjust Rows and Columns.
 */
    فراغ
ui_مجموعه_shellsize(
    صحيح		mustset استفادهنشده)	// set by the user
{
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
	gui_مجموعه_shellsize(mustset, TRUE, RESIZE_BOTH);
    وإلا
#پایان_اگر
	mch_مجموعه_shellsize();
}

/*
 * Called when Rows and/or Columns changed.  Adjust scroll region and mouse
 * region.
 */
    فراغ
ui_new_shellsize(فراغ)
{
    إذا (پر_screen && !exiting)
    {
#اگر_تعریف_شده FEAT_GUI
	إذا (gui.درون_use)
	    gui_new_shellsize();
	وإلا
#پایان_اگر
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
ui_get_winpos(صحيح *x, صحيح *y, varnumber_T timeout استفادهنشده)
{
# ifdef FEAT_GUI
    إذا (gui.درون_use)
	أرجع gui_mch_get_winpos(x, y);
# endif
# إذا defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
    أرجع mch_get_winpos(x, y);
# وإلا
#  إذا defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)
    أرجع جمله_get_winpos(x, y, timeout);
#  وإلا
    أرجع FAIL;
#  endif
# endif
}
#پایان_اگر

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

#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
	gui_mch_update();
    وإلا
#پایان_اگر
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

#إذا defined(USE_درون_داد_BUF)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
# إذا defined(FEAT_کار_CHANNEL) || defined(FEAT_CLIENTSERVER)
   /*
    * NetBeans stuffs debugger commands into the input buffer.
    * This requires a larger buffer...
    * (Madsen) Go with this for remote input as well ...
    */
#  define INBUFLEN 4096
# وإلا
#  define INBUFLEN 250
# endif

ساكن char_u	inbuf[INBUFLEN + MAX_کلید_کد_LEN];
ساكن صحيح	inbufcount = 0;	    // number of chars in inbuf[]

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    صحيح
vim_is_درون_داد_buf_پر(فراغ)
{
    أرجع (inbufcount >= INBUFLEN);
}

    صحيح
vim_is_درون_داد_buf_خالی(فراغ)
{
    أرجع (inbufcount == 0);
}

# إذا defined(FEAT_OLE)
    صحيح
vim_free_درون_درون_داد_buf(فراغ)
{
    أرجع (INBUFLEN - inbufcount);
}
# endif

# إذا defined(FEAT_GUI_GTK)
    صحيح
vim_استفاده_شده_درون_درون_داد_buf(فراغ)
{
    أرجع inbufcount;
}
# endif

/*
 * Return the current contents of the input buffer and make it empty.
 * The returned pointer must be passed to set_input_buf() later.
 */
    char_u *
get_درون_داد_buf(فراغ)
{
    garray_T	*gap;

    // We use a growarray to store the data pointer and the length.
    gap = ALLOC_ONE(garray_T);
    إذا (gap != عدم)
    {
	// Add one to avoid a zero size.
	gap->ga_داده = alloc(inbufcount + 1);
	إذا (gap->ga_داده != عدم)
	    mch_memmove(gap->ga_داده, inbuf, (size_t)inbufcount);
	gap->ga_len = inbufcount;
    }
    trash_درون_داد_buf();
    أرجع (char_u *)gap;
}

/*
 * Restore the input buffer with a pointer returned from get_input_buf().
 * The allocated memory is freed, this only works once!
 * When "overwrite" is FALSE input typed later is kept.
 */
    فراغ
مجموعه_درون_داد_buf(char_u *p, صحيح overwrite)
{
    garray_T	*gap = (garray_T *)p;

    إذا (gap == عدم)
	أرجع;

    إذا (gap->ga_داده != عدم)
    {
	إذا (overwrite || inbufcount + gap->ga_len >= INBUFLEN)
	{
	    mch_memmove(inbuf, gap->ga_داده, gap->ga_len);
	    inbufcount = gap->ga_len;
	}
	وإلا
	{
	    mch_memmove(inbuf + gap->ga_len, inbuf, inbufcount);
	    mch_memmove(inbuf, gap->ga_داده, gap->ga_len);
	    inbufcount += gap->ga_len;
	}
	vim_free(gap->ga_داده);
    }
    vim_free(gap);
}

/*
 * Add the given bytes to the input buffer
 * Special keys start with CSI.  A real CSI must have been translated to
 * CSI KS_EXTRA KE_CSI.  K_SPECIAL doesn't require translation.
 */
    فراغ
جمع_کن_مقصد_درون_داد_buf(char_u *s, صحيح len)
{
    إذا (inbufcount + len > INBUFLEN + MAX_کلید_کد_LEN)
	أرجع;	    // Shouldn't ever happen!

    طالما (len--)
	inbuf[inbufcount++] = *s++;
}

/*
 * Add "str[len]" to the input buffer while escaping CSI bytes.
 */
    فراغ
جمع_کن_مقصد_درون_داد_buf_csi(char_u *str, صحيح len)
{
    صحيح		i;
    char_u	buf[2];

    لكل (i = 0; i < len; ++i)
    {
	جمع_کن_مقصد_درون_داد_buf(str + i, 1);
	إذا (str[i] == CSI)
	{
	    // Turn CSI into K_CSI.
	    buf[0] = KS_EXTRA;
	    buf[1] = (صحيح)KE_CSI;
	    جمع_کن_مقصد_درون_داد_buf(buf, 2);
	}
    }
}

/*
 * Remove everything from the input buffer.  Called when ^C is found.
 */
    فراغ
trash_درون_داد_buf(فراغ)
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 */
    صحيح
read_from_درون_داد_buf(char_u *buf, دراز maxlen)
{
    إذا (inbufcount == 0)	// if the buffer is empty, fill it
	fill_درون_داد_buf(TRUE);
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
fill_درون_داد_buf(صحيح exit_on_error استفادهنشده)
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
    إذا (gui.درون_use
#  ifdef NO_CONSOLE_درون_داد
    // Don't use the GUI input when the window hasn't been opened yet.
    // We get here from ui_inchar() when we should try reading from stdin.
	    && !no_console_درون_داد()
#  endif
       )
    {
	gui_mch_update();
	أرجع;
    }
# endif
# إذا defined(UNIX) || defined(VMS) || defined(MACOS_X)
    إذا (vim_is_درون_داد_buf_پر())
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
						       / درون_داد_conv.vc_ضریب);
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
	إذا (درون_داد_conv.vc_type != CONV_NONE)
	{
	    inbufcount -= unconverted;
	    len = convert_درون_داد_امن(inbuf + inbufcount,
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
#پایان_اگر // USE_INPUT_BUF

/*
 * Exit because of an input read error.
 */
    فراغ
read_error_exit(فراغ)
{
    إذا (silent_نما)	// Normal way to exit for "ex -s"
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
    إذا (gui.درون_use)
	gui_update_cursor_later();
    وإلا
# endif
	جمله_cursor_نما(forced);

# ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
# endif

# ifdef FEAT_CONCEAL
    conceal_check_cursor_سطر(FALSE);
# endif
}

    فراغ
ui_cursor_shape(فراغ)
{
    ui_cursor_shape_forced(FALSE);
}
#پایان_اگر

/*
 * Check bounds for column number
 */
    صحيح
check_ستون(صحيح ستون)
{
    إذا (ستون < 0)
	أرجع 0;
    إذا (ستون >= screen_Columns)
	أرجع screen_Columns - 1;
    أرجع ستون;
}

/*
 * Check bounds for row number
 */
    صحيح
check_ردیف(صحيح ردیف)
{
    إذا (ردیف < 0)
	أرجع 0;
    إذا (ردیف >= screen_ردیفها)
	أرجع screen_ردیفها - 1;
    أرجع ردیف;
}

/*
 * Return length of line "lnum" in screen cells for horizontal scrolling.
 */
    دراز
scroll_سطر_len(linenr_T lnum)
{
    char_u	*p = ml_get(lnum);
    colnr_T	ستون = 0;

    إذا (*p != NUL)
	لكل (;;)
	{
	    صحيح	    w = chartabsize(p, ستون);
	    MB_PTR_ADV(p);
	    إذا (*p == NUL)		// don't count the last character
		اكسر;
	    ستون += w;
	}
    أرجع ستون;
}

/*
 * Find the longest visible line number.  This is used for horizontal
 * scrolling.  If this is not possible (or not desired, by setting 'h' in
 * "guioptions") then the current line number is returned.
 */
    linenr_T
ui_پیدا_کن_longest_lnum(فراغ)
{
    linenr_T ret = 0;

    // Calculate maximum for horizontal scrollbar.  Check for reasonable
    // line numbers, topline and botline can be invalid when displaying is
    // postponed.
    إذا (
#اگر_تعریف_شده FEAT_GUI
	    (!gui.درون_use || vim_strchr(p_go, GO_HORSCROLL) == عدم) &&
#پایان_اگر
	    curwin->w_topline <= curwin->w_cursor.lnum
	    && curwin->w_botline > curwin->w_cursor.lnum
	    && curwin->w_botline <= curbuf->b_ml.ml_سطر_شمار + 1)
    {
	linenr_T    lnum;
	دراز	    n;
	دراز	    max = 0;

	// Use maximum of all visible lines.  Remember the lnum of the
	// longest line, closest to the cursor line.  Used when scrolling
	// below.
	لكل (lnum = curwin->w_topline; lnum < curwin->w_botline; ++lnum)
	{
	    n = scroll_سطر_len(lnum);
	    إذا (n > max)
	    {
		max = n;
		ret = lnum;
	    }
	    وإلا إذا (n == max && قدر_مطلق((صحيح)(lnum - curwin->w_cursor.lnum))
				     < قدر_مطلق((صحيح)(ret - curwin->w_cursor.lnum)))
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
    صحيح		درون_focus)	// TRUE if focus gained.
{
    ساكن time_t	آخرین_زمان = (time_t)0;
    صحيح			need_redraw = FALSE;

    // When activated: Check if any file was modified outside of Vim.
    // Only do this when not done within the last two seconds (could get
    // several events in a row).
    إذا (درون_focus && آخرین_زمان + 2 < زمان(عدم))
    {
	need_redraw = check_timestamps(
#اگر_تعریف_شده FEAT_GUI
		gui.درون_use
#وإلا
		FALSE
#پایان_اگر
		);
	آخرین_زمان = زمان(عدم);
    }

#اگر_تعریف_شده FEAT_TERMINAL
    جمله_focus_change(درون_focus);
#پایان_اگر

    /*
     * Fire the focus gained/lost autocommand.
     */
    need_redraw |= apply_autocmds(درون_focus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, عدم, عدم, FALSE, curbuf);

    إذا (need_redraw)
	redraw_after_callback(TRUE, TRUE);

    // File may have been changed from 'readonly' to 'noreadonly'
    إذا (need_maketitle)
	maketitle();
}

#إذا defined(HAVE_درون_داد_روش)
/*
 * Save current Input Method status to specified place.
 */
    فراغ
im_save_وضعیت(دراز *psave)
{
    // Don't save when 'imdisable' is set or "xic" is NULL, IM is always
    // disabled then (but might start later).
    // Also don't save when inside a mapping, vgetc_im_active has not been set
    // then.
    // And don't save when the keys were stuffed (e.g., for a "." command).
    // And don't save when the GUI is running but our window doesn't have
    // input focus (e.g., when a find dialog is open).
    إذا (!p_imdisable && کلید_Typed && !کلید_Stuffed
# ifdef FEAT_XIM
	    && xic != عدم
# endif
# ifdef FEAT_GUI
	    && (!gui.درون_use || gui.درون_focus)
# endif
	)
    {
	// Do save when IM is on, or IM is off and saved status is on.
	إذا (vgetc_im_فعال)
	    *psave = B_IMODE_IM;
	وإلا إذا (*psave == B_IMODE_IM)
	    *psave = B_IMODE_NONE;
    }
}
#پایان_اگر
