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
#यदि defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
स्थिर वर्ण	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    प्रति ((t) = first_timer; (t) != रिक्त; (t) = (t)->tr_next)

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
    यदि (tz == रिक्त)
	tz = "";
    यदि (STRNCMP(tz_cache, tz, परिमाण(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, परिमाण(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    प्रत्यागम localtime_r(timep, result);
#अन्यथा
    प्रत्यागम localtime(timep);
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
    प्रत्यागम time_for_testing == 0 ? time(रिक्त) : time_for_testing;
#अन्यथा
    प्रत्यागम time(रिक्त);
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
    यदि (curtime == रिक्त)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), परिमाण(buf) - 2);
    अन्यथा
    {
	// xgettext:no-c-format
	यदि (strftime(buf, परिमाण(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
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
	यदि (enc_codepage >= 0 && (पूर्णाङ्क)GetACP() != enc_codepage)
	{
	    char_u	*to_free = रिक्त;
	    पूर्णाङ्क		len;

	    acp_to_enc((char_u *)buf, (पूर्णाङ्क)strlen(buf), &to_free, &len);
	    यदि (to_free != रिक्त)
	    {
		STRNCPY(buf, to_free, परिमाण(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#अन्यथा
    STRCPY(buf, "(unknown)");
#endif
    यदि (add_newline)
	STRCAT(buf, "\n");
    प्रत्यागम buf;
}

#यदि defined(FEAT_EVAL)

# यदि defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    शून्य
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_संख्या = (varnumber_T)time(रिक्त);
}

# यदि defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    स्थिर पूर्णाङ्क
list2proftime(typval_T *arg, proftime_T *tm)
{
    long	n1, n2;
    पूर्णाङ्क	error = FALSE;

    यदि (arg->v_type != VAR_LIST || arg->vval.v_list == रिक्त
					     || arg->vval.v_list->lv_len != 2)
	प्रत्यागम FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->वरचा_Part = n1;
    tm->खालचा_Part = n2;
#  अन्यथा
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    प्रत्यागम error ? FAIL : OK;
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

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    अन्यथा यदि (argvars[1].v_type == VAR_UNKNOWN)
    {
	यदि (list2proftime(&argvars[0], &res) == FAIL)
	{
	    यदि (in_vim9script())
		emsg(_(e_invalid_argument));
	    प्रत्यागम;
	}
	profile_end(&res);
    }
    अन्यथा
    {
	// Two arguments: compute the difference.
	यदि (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    यदि (in_vim9script())
		emsg(_(e_invalid_argument));
	    प्रत्यागम;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.वरचा_Part;
    n2 = res.खालचा_Part;
#  अन्यथा
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)n1);
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)n2);
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
    यदि (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    अन्यथा यदि (in_vim9script())
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
    यदि (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    proftime_T	tm;
    यदि (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_संदेश(&tm));
#  अन्यथा
	स्थिर वर्ण buf[50];
	long usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, परिमाण(buf), "%3ld.%06ld", (long)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    अन्यथा यदि (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# यदि defined(HAVE_STRFTIME)
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

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    यदि (argvars[1].v_type == VAR_UNKNOWN)
	seconds = time(रिक्त);
    अन्यथा
	seconds = (time_t)tv_get_संख्या(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    यदि (curtime == रिक्त)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	प्रत्यागम;
    }

#  ifdef MSWIN
    WCHAR	    result_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, रिक्त);
    यदि (wp != रिक्त)
	(शून्य)wcsftime(result_buf, ARRAY_LENGTH(result_buf), wp, curtime);
    अन्यथा
	result_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(result_buf, रिक्त);
    vim_free(wp);
#  अन्यथा
    char_u	    result_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    यदि (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, रिक्त);
    यदि (p == रिक्त || strftime((वर्ण *)result_buf, परिमाण(result_buf),
		(वर्ण *)p, curtime) == 0)
	result_buf[0] = NUL;

    यदि (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    यदि (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, result_buf, रिक्त);
    अन्यथा
	rettv->vval.v_string = vim_strsave(result_buf);

    // Release conversion descriptors
    convert_setup(&conv, रिक्त, रिक्त);
    vim_free(enc);
#  endif
}
# endif

# यदि defined(HAVE_STRPTIME)
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

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    यदि (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, रिक्त);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    यदि (fmt == रिक्त
	    || strptime((वर्ण *)str, (वर्ण *)fmt, &tmval) == रिक्त
	    || (rettv->vval.v_संख्या = mktime(&tmval)) == -1)
	rettv->vval.v_संख्या = 0;

    यदि (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, रिक्त, रिक्त);
    vim_free(enc);
}
# endif

# यदि defined(FEAT_TIMERS)
स्थिर timer_T	*first_timer = रिक्त;
स्थिर long	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    long
proftime_time_डावा(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    यदि (now->QuadPart > due->QuadPart)
	प्रत्यागम 0;
    QueryPerformanceFrequency(&fr);
    प्रत्यागम (long)(((द्विगुण)(due->QuadPart - now->QuadPart)
		   / (द्विगुण)fr.QuadPart) * 1000);
#  अन्यथा
    यदि (now->tv_sec > due->tv_sec)
	प्रत्यागम 0;
    प्रत्यागम (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    स्थिर शून्य
घाला_timer(timer_T *timer)
{
    timer->tr_next = first_timer;
    timer->tr_prev = रिक्त;
    यदि (first_timer != रिक्त)
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
    यदि (timer->tr_prev == रिक्त)
	first_timer = timer->tr_next;
    अन्यथा
	timer->tr_prev->tr_next = timer->tr_next;
    यदि (timer->tr_next != रिक्त)
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

    यदि (timer == रिक्त)
	प्रत्यागम रिक्त;
    यदि (++last_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    घाला_timer(timer);
    यदि (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    प्रत्यागम timer;
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
    यदि (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(रिक्त, "invoking timer callback %s",
	       cb->cb_partial != रिक्त ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_संख्या;
    argv[0].vval.v_संख्या = (varnumber_T)timer->tr_id;
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
    यदि (exiting || aborting() || debug_mode)
	प्रत्यागम next_due;

    profile_start(&now);
    प्रति (timer = first_timer; timer != रिक्त && !got_int; timer = timer_next)
    {
	timer_next = timer->tr_next;

	यदि (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    अनुवर्त;
	this_due = proftime_time_डावा(&timer->tr_due, &now);
	यदि (this_due <= 1)
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
	    यदि (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_मोजणी;
	    अन्यथा
		timer->tr_emsg_मोजणी = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    यदि (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    यदि (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_मोजणी < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_डावा(&timer->tr_due, &now);
		यदि (this_due < 1)
		    this_due = 1;
		यदि (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    अन्यथा
	    {
		this_due = -1;
		यदि (timer->tr_keep)
		    timer->tr_paused = TRUE;
		अन्यथा
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
	यदि (this_due > 0 && (next_due == -1 || next_due > this_due))
	    next_due = this_due;
    }

    यदि (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    यदि (bevalexpr_due_set)
    {
	this_due = proftime_time_डावा(&bevalexpr_due, &now);
	यदि (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    यदि (balloonEval == रिक्त)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    यदि (balloonEval != रिक्त)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	अन्यथा यदि (next_due == -1 || next_due > this_due)
	    next_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    next_due = term_check_timers(next_due, &now);
#  endif

    प्रत्यागम current_id != last_timer_id ? 1 : next_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    स्थिर timer_T *
find_timer(long id)
{
    timer_T *timer;

    यदि (id < 0)
	प्रत्यागम रिक्त;

    FOR_ALL_TIMERS(timer)
	यदि (timer->tr_id == id)
	    प्रत्यागम timer;
    प्रत्यागम रिक्त;
}


/*
 * Stop a timer and delete it.
 */
    शून्य
stop_timer(timer_T *timer)
{
    यदि (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    अन्यथा
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

    प्रति (timer = first_timer; timer != रिक्त; timer = timer_next)
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

    यदि (dict == रिक्त)
	प्रत्यागम;
    list_append_dict(list, dict);

    dict_add_संख्या(dict, "id", timer->tr_id);
    dict_add_संख्या(dict, "time", (long)timer->tr_interval);

    profile_start(&now);
    remaining = proftime_time_डावा(&timer->tr_due, &now);
    dict_add_संख्या(dict, "remaining", (long)remaining);

    dict_add_संख्या(dict, "repeat",
	    (long)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_add_संख्या(dict, "paused", (long)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    यदि (di != रिक्त)
    {
	यदि (dict_add(dict, di) == FAIL)
	    vim_free(di);
	अन्यथा
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    स्थिर शून्य
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	यदि (timer->tr_id != -1)
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

    प्रति (timer = first_timer; !abort && timer != रिक्त; timer = timer->tr_next)
    {
	यदि (timer->tr_callback.cb_partial != रिक्त)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	अन्यथा
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    }
    प्रत्यागम abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    पूर्णाङ्क
timer_valid(timer_T *timer)
{
    यदि (timer == रिक्त)
	प्रत्यागम FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	यदि (t == timer)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

#  यदि defined(EXITFREE)
    शून्य
timer_free_all(शून्य)
{
    यावत् (first_timer != रिक्त)
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

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((पूर्णाङ्क)tv_get_संख्या(&argvars[0]));
	यदि (timer != रिक्त)
	    add_timer_info(rettv, timer);
    }
    अन्यथा
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    शून्य
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = रिक्त;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_संख्या)
    {
	emsg(_(e_संख्या_expected));
	प्रत्यागम;
    }

    पूर्णाङ्क	paused = (पूर्णाङ्क)tv_get_bool(&argvars[1]);

    timer = find_timer((पूर्णाङ्क)tv_get_संख्या(&argvars[0]));
    यदि (timer != रिक्त)
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

    rettv->vval.v_संख्या = -1;
    यदि (check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    msec = (long)tv_get_संख्या(&argvars[0]);
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	यदि (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    प्रत्यागम;

	dict = argvars[2].vval.v_dict;
	यदि (dict_has_किल्ली(dict, "repeat"))
	    repeat = dict_get_संख्या(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    यदि (callback.cb_name == रिक्त)
	प्रत्यागम;
    यदि (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	प्रत्यागम;
    }

    timer = create_timer(msec, repeat);
    यदि (timer == रिक्त)
    {
	free_callback(&callback);
	प्रत्यागम;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_संख्या = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    शून्य
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    यदि (check_for_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    timer = find_timer((पूर्णाङ्क)tv_get_संख्या(&argvars[0]));
    यदि (timer != रिक्त)
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

# यदि defined(STARTUPTIME)
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
    प्रत्यागम 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    शून्य
time_ढकला(शून्य *tv_rel, शून्य *tv_start)
{
    *((संरचना timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, रिक्त);
    ((संरचना timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((संरचना timeval *)tv_rel)->tv_usec;
    ((संरचना timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((संरचना timeval *)tv_rel)->tv_sec;
    यदि (((संरचना timeval *)tv_rel)->tv_usec < 0)
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
time_काढा(
    शून्य	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((संरचना timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((संरचना timeval *)tp)->tv_sec;
    यदि (prev_timeval.tv_usec < 0)
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
time_संदेश(
    वर्ण	*mesg,
    शून्य	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    स्थिर संरचना timeval	start;
    संरचना timeval		now;

    यदि (time_fd == रिक्त)
	प्रत्यागम;

    यदि (strstr(mesg, "STARTING") != रिक्त)
    {
	gettimeofday(&start, रिक्त);
	prev_timeval = start;
	fprintf(time_fd, "\n\ntimes in msec\n");
	fprintf(time_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, रिक्त);
    time_diff(&start, &now);
    यदि (((संरचना timeval *)tv_start) != रिक्त)
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

#यदि defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
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

    प्रति (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	यदि (c == EOF) प्रत्यागम -1;
	n = (n << 8) + c;
    }
    प्रत्यागम n;
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
    प्रत्यागम fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
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
    प्रति (i = 7; i >= 0; --i)
    {
	यदि (i + 1 > (पूर्णाङ्क)परिमाण(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	अन्यथा
	{
# यदि defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (पूर्णाङ्क)(wtime >> (i * 8));
# अन्यथा
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

    यदि (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	यदि (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((वर्ण *)buf, buflen, "%H:%M:%S", curtime);
	अन्यथा
	    // longer ago
	    n = strftime((वर्ण *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	यदि (n == 0)
	    buf[0] = NUL;
    }
    अन्यथा
#endif
    {
	long seconds = (long)(vim_time() - tt);

	vim_snprintf((वर्ण *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
