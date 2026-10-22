/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * time.c: functions related to time and timers
 */

#अन्तर्भाव "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#सचे defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
स्थिर वर्ण	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    अत्थाय ((t) = first_timer; (t) != रिक्त; (t) = (t)->tr_next)

/*
 * Call either localtime(3) or localtime_r(3) from POSIX libc time.h, with the
 * latter version preferred for reentrancy.
 *
 * If we use localtime_r(3) and we have tzset(3) available, check to see if the
 * environment variable TZ has changed since the last run, and call tzset(3) to
 * update the global timezone variables if it has.  This is because the POSIX
 * standard doesn't require localtime_r(3) implementations to do that as it
 * does with localtime(3), and we don't want to call tzset(3) every time.
 */
    स्थिर संरचना tm *
vim_localtime(
    ध्रुव time_t	*timep,		// timestamp for local representation
    संरचना tm		*result UNUSED)	// pointer to caller return buffer
{
#ifdef HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    वर्ण		*tz;		// pointer for TZ environment var

    tz = (वर्ण *)mch_getenv((char_u *)"TZ");
    सचे (tz == रिक्त)
	tz = "";
    सचे (STRNCMP(tz_cache, tz, परिमाण(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, परिमाण(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    पटिनिवत्त localtime_r(timep, result);
#अञ्ञथा
    पटिनिवत्त localtime(timep);
#endif	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    time_T
vim_time(शून्य)
{
#ifdef FEAT_EVAL
    पटिनिवत्त time_for_testing == 0 ? time(रिक्त) : time_for_testing;
#अञ्ञथा
    पटिनिवत्त time(रिक्त);
#endif
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    वर्ण *
get_ctime(time_t thetime, पूर्णाङ्क add_newline)
{
    स्थिर वर्ण buf[100];  // hopefully enough for every language
#ifdef HAVE_STRFTIME
    संरचना tm	tmval;
    संरचना tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    सचे (curtime == रिक्त)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), परिमाण(buf) - 2);
    अञ्ञथा
    {
	// xgettext:no-c-format
	सचे (strftime(buf, परिमाण(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      परिमाण(buf) - 2);
	}
# ifdef MSWIN
	सचे (enc_codepage >= 0 && (पूर्णाङ्क)GetACP() != enc_codepage)
	{
	    char_u	*to_free = रिक्त;
	    पूर्णाङ्क		len;

	    acp_to_enc((char_u *)buf, (पूर्णाङ्क)strlen(buf), &to_free, &len);
	    सचे (to_free != रिक्त)
	    {
		STRNCPY(buf, to_free, परिमाण(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#अञ्ञथा
    STRCPY(buf, "(unknown)");
#endif
    सचे (add_newline)
	STRCAT(buf, "\n");
    पटिनिवत्त buf;
}

#सचे defined(FEAT_EVAL)

# सचे defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    शून्य
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = (varnumber_T)time(रिक्त);
}

# सचे defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    स्थिर पूर्णाङ्क
list2proftime(typval_T *arg, proftime_T *tm)
{
    long	n1, n2;
    पूर्णाङ्क	error = FALSE;

    सचे (arg->v_type != VAR_LIST || arg->vval.v_list == रिक्त
					     || arg->vval.v_list->lv_len != 2)
	पटिनिवत्त FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->उच्चः_Part = n1;
    tm->नीचः_Part = n2;
#  अञ्ञथा
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    पटिनिवत्त error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    शून्य
f_reltime(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	start;
    long	n1, n2;

    सचे (rettv_list_alloc(rettv) == FAIL)
	पटिनिवत्त;

    सचे (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	पटिनिवत्त;

    सचे (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    अञ्ञथा सचे (argvars[1].v_type == VAR_UNKNOWN)
    {
	सचे (list2proftime(&argvars[0], &res) == FAIL)
	{
	    सचे (in_vim9script())
		emsg(_(e_invalid_argument));
	    पटिनिवत्त;
	}
	profile_end(&res);
    }
    अञ्ञथा
    {
	// Two arguments: compute the difference.
	सचे (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    सचे (in_vim9script())
		emsg(_(e_invalid_argument));
	    पटिनिवत्त;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.उच्चः_Part;
    n2 = res.नीचः_Part;
#  अञ्ञथा
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_सङ्ख्या(rettv->vval.v_list, (varnumber_T)n1);
    list_append_सङ्ख्या(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    शून्य
f_reltimefloat(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    सचे (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    अञ्ञथा सचे (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

/*
 * "reltimestr()" function
 */
    शून्य
f_reltimestr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
# ifdef FEAT_RELTIME
    सचे (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    proftime_T	tm;
    सचे (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_सन्देशः(&tm));
#  अञ्ञथा
	स्थिर वर्ण buf[50];
	long usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, परिमाण(buf), "%3ld.%06ld", (long)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    अञ्ञथा सचे (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# सचे defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    शून्य
f_strftime(typval_T *argvars, typval_T *rettv)
{
    संरचना tm	tmval;
    संरचना tm	*curtime;
    time_t	seconds;
    char_u	*p;

    सचे (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    सचे (argvars[1].v_type == VAR_UNKNOWN)
	seconds = time(रिक्त);
    अञ्ञथा
	seconds = (time_t)tv_get_सङ्ख्या(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    सचे (curtime == रिक्त)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	पटिनिवत्त;
    }

#  ifdef MSWIN
    WCHAR	    result_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, रिक्त);
    सचे (wp != रिक्त)
	(शून्य)wcsftime(result_buf, ARRAY_LENGTH(result_buf), wp, curtime);
    अञ्ञथा
	result_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(result_buf, रिक्त);
    vim_free(wp);
#  अञ्ञथा
    char_u	    result_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    सचे (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, रिक्त);
    सचे (p == रिक्त || strftime((वर्ण *)result_buf, परिमाण(result_buf),
		(वर्ण *)p, curtime) == 0)
	result_buf[0] = NUL;

    सचे (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    सचे (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, result_buf, रिक्त);
    अञ्ञथा
	rettv->vval.v_string = vim_strsave(result_buf);

    // Release conversion descriptors
    convert_setup(&conv, रिक्त, रिक्त);
    vim_free(enc);
#  endif
}
# endif

# सचे defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    शून्य
f_strptime(typval_T *argvars, typval_T *rettv)
{
    संरचना tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    सचे (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    सचे (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, रिक्त);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    सचे (fmt == रिक्त
	    || strptime((वर्ण *)str, (वर्ण *)fmt, &tmval) == रिक्त
	    || (rettv->vval.v_सङ्ख्या = mktime(&tmval)) == -1)
	rettv->vval.v_सङ्ख्या = 0;

    सचे (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, रिक्त, रिक्त);
    vim_free(enc);
}
# endif

# सचे defined(FEAT_TIMERS)
स्थिर timer_T	*first_timer = रिक्त;
स्थिर long	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    long
proftime_time_वामः(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    सचे (now->QuadPart > due->QuadPart)
	पटिनिवत्त 0;
    QueryPerformanceFrequency(&fr);
    पटिनिवत्त (long)(((द्विगुण)(due->QuadPart - now->QuadPart)
		   / (द्विगुण)fr.QuadPart) * 1000);
#  अञ्ञथा
    सचे (now->tv_sec > due->tv_sec)
	पटिनिवत्त 0;
    पटिनिवत्त (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    स्थिर शून्य
निवेशय_timer(timer_T *timer)
{
    timer->tr_next = first_timer;
    timer->tr_prev = रिक्त;
    सचे (first_timer != रिक्त)
	first_timer->tr_prev = timer;
    first_timer = timer;
    did_add_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    स्थिर शून्य
remove_timer(timer_T *timer)
{
    सचे (timer->tr_prev == रिक्त)
	first_timer = timer->tr_next;
    अञ्ञथा
	timer->tr_prev->tr_next = timer->tr_next;
    सचे (timer->tr_next != रिक्त)
	timer->tr_next->tr_prev = timer->tr_prev;
}

    स्थिर शून्य
free_timer(timer_T *timer)
{
    free_callback(&timer->tr_callback);
    vim_free(timer);
}

/*
 * Create a timer and return it.  NULL if out of memory.
 * Caller should set the callback.
 */
    timer_T *
create_timer(long msec, पूर्णाङ्क repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    long	prev_id = last_timer_id;

    सचे (timer == रिक्त)
	पटिनिवत्त रिक्त;
    सचे (++last_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    निवेशय_timer(timer);
    सचे (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    पटिनिवत्त timer;
}

/*
 * (Re)start a timer.
 */
    शून्य
timer_start(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    स्थिर शून्य
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    सचे (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(रिक्त, "invoking timer callback %s",
	       cb->cb_partial != रिक्त ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_सङ्ख्या;
    argv[0].vval.v_सङ्ख्या = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(रिक्त, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    long
check_due_timer(शून्य)
{
    timer_T	*timer;
    timer_T	*timer_next;
    long	this_due;
    long	next_due = -1;
    proftime_T	now;
    पूर्णाङ्क		did_one = FALSE;
    पूर्णाङ्क		need_update_screen = FALSE;
    long	current_id = last_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    सचे (exiting || aborting() || debug_mode)
	पटिनिवत्त next_due;

    profile_start(&now);
    अत्थाय (timer = first_timer; timer != रिक्त && !got_int; timer = timer_next)
    {
	timer_next = timer->tr_next;

	सचे (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    अनुवर्त;
	this_due = proftime_time_वामः(&timer->tr_due, &now);
	सचे (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    पूर्णाङ्क save_timer_busy = timer_busy;
	    पूर्णाङ्क save_vgetc_busy = vgetc_busy;
	    पूर्णाङ्क save_did_emsg = did_emsg;
	    पूर्णाङ्क prev_uncaught_emsg = uncaught_emsg;
	    पूर्णाङ्क save_called_emsg = called_emsg;
	    पूर्णाङ्क save_must_redraw = must_redraw;
	    पूर्णाङ्क save_ex_pressedreturn = get_pressedreturn();
	    पूर्णाङ्क save_may_garbage_collect = may_garbage_collect;
	    vimvars_save_T	vvsave;
	    exception_state_T	estate;

	    exception_state_save(&estate);

	    // Create a scope for running the timer callback, ignoring most of
	    // the current scope, such as being inside a try/catch.
	    timer_busy = timer_busy > 0 || vgetc_busy > 0;
	    vgetc_busy = 0;
	    called_emsg = 0;
	    did_emsg = FALSE;
	    must_redraw = 0;
	    may_garbage_collect = FALSE;
	    exception_state_clear();
	    save_vimvars(&vvsave);

	    // Invoke the callback.
	    timer->tr_firing = TRUE;
	    timer_callback(timer);
	    timer->tr_firing = FALSE;

	    // Restore stuff.
	    timer_next = timer->tr_next;
	    did_one = TRUE;
	    timer_busy = save_timer_busy;
	    vgetc_busy = save_vgetc_busy;
	    सचे (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_गणनम्;
	    अञ्ञथा
		timer->tr_emsg_गणनम् = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    सचे (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    सचे (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_गणनम् < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_वामः(&timer->tr_due, &now);
		सचे (this_due < 1)
		    this_due = 1;
		सचे (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    अञ्ञथा
	    {
		this_due = -1;
		सचे (timer->tr_keep)
		    timer->tr_paused = TRUE;
		अञ्ञथा
		{
		    remove_timer(timer);
		    free_timer(timer);
		}
	    }

#  ifdef FEAT_IMAGE
	    // Make sure to update any image placements. Without this, image
	    // placements will not be updated on the screen properly in a
	    // hit-enter prompt.
	    draw_image_placements();
#  endif
	}
	सचे (this_due > 0 && (next_due == -1 || next_due > this_due))
	    next_due = this_due;
    }

    सचे (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    सचे (bevalexpr_due_set)
    {
	this_due = proftime_time_वामः(&bevalexpr_due, &now);
	सचे (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    सचे (balloonEval == रिक्त)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    सचे (balloonEval != रिक्त)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	अञ्ञथा सचे (next_due == -1 || next_due > this_due)
	    next_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    next_due = term_check_timers(next_due, &now);
#  endif

    पटिनिवत्त current_id != last_timer_id ? 1 : next_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    स्थिर timer_T *
find_timer(long id)
{
    timer_T *timer;

    सचे (id < 0)
	पटिनिवत्त रिक्त;

    FOR_ALL_TIMERS(timer)
	सचे (timer->tr_id == id)
	    पटिनिवत्त timer;
    पटिनिवत्त रिक्त;
}


/*
 * Stop a timer and delete it.
 */
    शून्य
stop_timer(timer_T *timer)
{
    सचे (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    अञ्ञथा
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    स्थिर शून्य
stop_all_timers(शून्य)
{
    timer_T *timer;
    timer_T *timer_next;

    अत्थाय (timer = first_timer; timer != रिक्त; timer = timer_next)
    {
	timer_next = timer->tr_next;
	stop_timer(timer);
    }
}

    स्थिर शून्य
add_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    long	remaining;
    proftime_T	now;

    सचे (dict == रिक्त)
	पटिनिवत्त;
    list_append_dict(list, dict);

    dict_add_सङ्ख्या(dict, "id", timer->tr_id);
    dict_add_सङ्ख्या(dict, "time", (long)timer->tr_interval);

    profile_start(&now);
    remaining = proftime_time_वामः(&timer->tr_due, &now);
    dict_add_सङ्ख्या(dict, "remaining", (long)remaining);

    dict_add_सङ्ख्या(dict, "repeat",
	    (long)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_add_सङ्ख्या(dict, "paused", (long)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    सचे (di != रिक्त)
    {
	सचे (dict_add(dict, di) == FAIL)
	    vim_free(di);
	अञ्ञथा
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    स्थिर शून्य
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	सचे (timer->tr_id != -1)
	    add_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    पूर्णाङ्क
set_ref_in_timer(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    अत्थाय (timer = first_timer; !abort && timer != रिक्त; timer = timer->tr_next)
    {
	सचे (timer->tr_callback.cb_partial != रिक्त)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	अञ्ञथा
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    }
    पटिनिवत्त abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    पूर्णाङ्क
timer_valid(timer_T *timer)
{
    सचे (timer == रिक्त)
	पटिनिवत्त FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	सचे (t == timer)
	    पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

#  सचे defined(EXITFREE)
    शून्य
timer_free_all(शून्य)
{
    याव (first_timer != रिक्त)
    {
	timer_T *timer = first_timer;
	remove_timer(timer);
	free_timer(timer);
    }
}
#  endif

/*
 * "timer_info([timer])" function
 */
    शून्य
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = रिक्त;

    सचे (rettv_list_alloc(rettv) == FAIL)
	पटिनिवत्त;

    सचे (check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]));
	सचे (timer != रिक्त)
	    add_timer_info(rettv, timer);
    }
    अञ्ञथा
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    शून्य
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = रिक्त;

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    सचे (argvars[0].v_type != VAR_सङ्ख्या)
    {
	emsg(_(e_सङ्ख्या_expected));
	पटिनिवत्त;
    }

    पूर्णाङ्क	paused = (पूर्णाङ्क)tv_get_bool(&argvars[1]);

    timer = find_timer((पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]));
    सचे (timer != रिक्त)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    शून्य
f_timer_start(typval_T *argvars, typval_T *rettv)
{
    long	msec;
    timer_T	*timer;
    पूर्णाङ्क		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_सङ्ख्या = -1;
    सचे (check_secure())
	पटिनिवत्त;

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	पटिनिवत्त;

    msec = (long)tv_get_सङ्ख्या(&argvars[0]);
    सचे (argvars[2].v_type != VAR_UNKNOWN)
    {
	सचे (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    पटिनिवत्त;

	dict = argvars[2].vval.v_dict;
	सचे (dict_has_कुञ्चिका(dict, "repeat"))
	    repeat = dict_get_सङ्ख्या(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    सचे (callback.cb_name == रिक्त)
	पटिनिवत्त;
    सचे (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	पटिनिवत्त;
    }

    timer = create_timer(msec, repeat);
    सचे (timer == रिक्त)
    {
	free_callback(&callback);
	पटिनिवत्त;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_सङ्ख्या = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    शून्य
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    सचे (check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    timer = find_timer((पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]));
    सचे (timer != रिक्त)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    शून्य
f_timer_stopall(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# सचे defined(STARTUPTIME)
स्थिर संरचना timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    स्थिर पूर्णाङ्क
gettimeofday(संरचना timeval *tv, वर्ण *dummy UNUSED)
{
    long t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    पटिनिवत्त 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    शून्य
time_प्रक्षिप(शून्य *tv_rel, शून्य *tv_start)
{
    *((संरचना timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, रिक्त);
    ((संरचना timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((संरचना timeval *)tv_rel)->tv_usec;
    ((संरचना timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((संरचना timeval *)tv_rel)->tv_sec;
    सचे (((संरचना timeval *)tv_rel)->tv_usec < 0)
    {
	((संरचना timeval *)tv_rel)->tv_usec += 1000000;
	--((संरचना timeval *)tv_rel)->tv_sec;
    }
    *(संरचना timeval *)tv_start = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    शून्य
time_निष्कासय(
    शून्य	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((संरचना timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((संरचना timeval *)tp)->tv_sec;
    सचे (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    स्थिर शून्य
time_diff(संरचना timeval *then, संरचना timeval *now)
{
    long	usec;
    long	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(time_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    शून्य
time_सन्देशः(
    वर्ण	*mesg,
    शून्य	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    स्थिर संरचना timeval	start;
    संरचना timeval		now;

    सचे (time_fd == रिक्त)
	पटिनिवत्त;

    सचे (strstr(mesg, "STARTING") != रिक्त)
    {
	gettimeofday(&start, रिक्त);
	prev_timeval = start;
	fprintf(time_fd, "\n\ntimes in msec\n");
	fprintf(time_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, रिक्त);
    time_diff(&start, &now);
    सचे (((संरचना timeval *)tv_start) != रिक्त)
    {
	fprintf(time_fd, "  ");
	time_diff(((संरचना timeval *)tv_start), &now);
    }
    fprintf(time_fd, "  ");
    time_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(time_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#endif // FEAT_EVAL

#सचे defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    time_T
get8ctime(FILE *fd)
{
    पूर्णाङ्क		c;
    time_T	n = 0;
    पूर्णाङ्क		i;

    अत्थाय (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	सचे (c == EOF) पटिनिवत्त -1;
	n = (n << 8) + c;
    }
    पटिनिवत्त n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    पूर्णाङ्क
put_time(FILE *fd, time_T the_time)
{
    char_u	buf[8];

    time_to_bytes(the_time, buf);
    पटिनिवत्त fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    शून्य
time_to_bytes(time_T the_time, char_u *buf)
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		i;
    पूर्णाङ्क		bi = 0;
    time_T	wtime = the_time;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    अत्थाय (i = 7; i >= 0; --i)
    {
	सचे (i + 1 > (पूर्णाङ्क)परिमाण(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	अञ्ञथा
	{
# सचे defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (पूर्णाङ्क)(wtime >> (i * 8));
# अञ्ञथा
	    c = (पूर्णाङ्क)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#endif

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    शून्य
add_time(char_u *buf, size_t buflen, time_t tt)
{
#ifdef HAVE_STRFTIME
    संरचना tm	tmval;
    संरचना tm	*curtime;
    size_t	n;

    सचे (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	सचे (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((वर्ण *)buf, buflen, "%H:%M:%S", curtime);
	अञ्ञथा
	    // longer ago
	    n = strftime((वर्ण *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	सचे (n == 0)
	    buf[0] = NUL;
    }
    अञ्ञथा
#endif
    {
	long seconds = (long)(vim_time() - tt);

	vim_snprintf((वर्ण *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
