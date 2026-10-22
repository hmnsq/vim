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

#شامل "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#اگر defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
ایستا نویسه	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    برای ((t) = first_timer; (t) != پوچ; (t) = (t)->tr_next)

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
    ایستا ساختار tm *
vim_localtime(
    ثابت time_t	*timep,		// timestamp for local representation
    ساختار tm		*result UNUSED)	// pointer to caller return buffer
{
#ifdef HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    نویسه		*tz;		// pointer for TZ environment var

    tz = (نویسه *)mch_getenv((char_u *)"TZ");
    اگر (tz == پوچ)
	tz = "";
    اگر (STRNCMP(tz_cache, tz, اندازه(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, اندازه(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    بازگشت localtime_r(timep, result);
#وگرنه
    بازگشت localtime(timep);
#endif	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    time_T
vim_time(تهی)
{
#ifdef FEAT_EVAL
    بازگشت time_for_testing == 0 ? time(پوچ) : time_for_testing;
#وگرنه
    بازگشت time(پوچ);
#endif
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    نویسه *
get_ctime(time_t thetime, صحیح add_newline)
{
    ایستا نویسه buf[100];  // hopefully enough for every language
#ifdef HAVE_STRFTIME
    ساختار tm	tmval;
    ساختار tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    اگر (curtime == پوچ)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), اندازه(buf) - 2);
    وگرنه
    {
	// xgettext:no-c-format
	اگر (strftime(buf, اندازه(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      اندازه(buf) - 2);
	}
# ifdef MSWIN
	اگر (enc_codepage >= 0 && (صحیح)GetACP() != enc_codepage)
	{
	    char_u	*to_free = پوچ;
	    صحیح		len;

	    acp_to_enc((char_u *)buf, (صحیح)strlen(buf), &to_free, &len);
	    اگر (to_free != پوچ)
	    {
		STRNCPY(buf, to_free, اندازه(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#وگرنه
    STRCPY(buf, "(unknown)");
#endif
    اگر (add_newline)
	STRCAT(buf, "\n");
    بازگشت buf;
}

#اگر defined(FEAT_EVAL)

# اگر defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    تهی
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = (varnumber_T)time(پوچ);
}

# اگر defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    ایستا صحیح
list2proftime(typval_T *arg, proftime_T *tm)
{
    long	n1, n2;
    صحیح	error = FALSE;

    اگر (arg->v_type != VAR_LIST || arg->vval.v_list == پوچ
					     || arg->vval.v_list->lv_len != 2)
	بازگشت FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->بالا_Part = n1;
    tm->کم_Part = n2;
#  وگرنه
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    بازگشت error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    تهی
f_reltime(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	start;
    long	n1, n2;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	بازگشت;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    وگرنه اگر (argvars[1].v_type == VAR_UNKNOWN)
    {
	اگر (list2proftime(&argvars[0], &res) == FAIL)
	{
	    اگر (in_vim9script())
		emsg(_(e_invalid_argument));
	    بازگشت;
	}
	profile_end(&res);
    }
    وگرنه
    {
	// Two arguments: compute the difference.
	اگر (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    اگر (in_vim9script())
		emsg(_(e_invalid_argument));
	    بازگشت;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.بالا_Part;
    n2 = res.کم_Part;
#  وگرنه
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_عدد(rettv->vval.v_list, (varnumber_T)n1);
    list_append_عدد(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    تهی
f_reltimefloat(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    اگر (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    وگرنه اگر (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

/*
 * "reltimestr()" function
 */
    تهی
f_reltimestr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;
# ifdef FEAT_RELTIME
    اگر (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	بازگشت;

    proftime_T	tm;
    اگر (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_پیام(&tm));
#  وگرنه
	ایستا نویسه buf[50];
	long usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, اندازه(buf), "%3ld.%06ld", (long)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    وگرنه اگر (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# اگر defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    تهی
f_strftime(typval_T *argvars, typval_T *rettv)
{
    ساختار tm	tmval;
    ساختار tm	*curtime;
    time_t	seconds;
    char_u	*p;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type == VAR_UNKNOWN)
	seconds = time(پوچ);
    وگرنه
	seconds = (time_t)tv_get_عدد(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    اگر (curtime == پوچ)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	بازگشت;
    }

#  ifdef MSWIN
    WCHAR	    result_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, پوچ);
    اگر (wp != پوچ)
	(تهی)wcsftime(result_buf, ARRAY_LENGTH(result_buf), wp, curtime);
    وگرنه
	result_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(result_buf, پوچ);
    vim_free(wp);
#  وگرنه
    char_u	    result_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    اگر (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, پوچ);
    اگر (p == پوچ || strftime((نویسه *)result_buf, اندازه(result_buf),
		(نویسه *)p, curtime) == 0)
	result_buf[0] = NUL;

    اگر (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    اگر (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, result_buf, پوچ);
    وگرنه
	rettv->vval.v_string = vim_strsave(result_buf);

    // Release conversion descriptors
    convert_setup(&conv, پوچ, پوچ);
    vim_free(enc);
#  endif
}
# endif

# اگر defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    تهی
f_strptime(typval_T *argvars, typval_T *rettv)
{
    ساختار tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    اگر (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, پوچ);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    اگر (fmt == پوچ
	    || strptime((نویسه *)str, (نویسه *)fmt, &tmval) == پوچ
	    || (rettv->vval.v_عدد = mktime(&tmval)) == -1)
	rettv->vval.v_عدد = 0;

    اگر (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, پوچ, پوچ);
    vim_free(enc);
}
# endif

# اگر defined(FEAT_TIMERS)
ایستا timer_T	*first_timer = پوچ;
ایستا long	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    long
proftime_time_چپ(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    اگر (now->QuadPart > due->QuadPart)
	بازگشت 0;
    QueryPerformanceFrequency(&fr);
    بازگشت (long)(((دوگانه)(due->QuadPart - now->QuadPart)
		   / (دوگانه)fr.QuadPart) * 1000);
#  وگرنه
    اگر (now->tv_sec > due->tv_sec)
	بازگشت 0;
    بازگشت (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    ایستا تهی
درج_timer(timer_T *timer)
{
    timer->tr_next = first_timer;
    timer->tr_prev = پوچ;
    اگر (first_timer != پوچ)
	first_timer->tr_prev = timer;
    first_timer = timer;
    did_add_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    ایستا تهی
remove_timer(timer_T *timer)
{
    اگر (timer->tr_prev == پوچ)
	first_timer = timer->tr_next;
    وگرنه
	timer->tr_prev->tr_next = timer->tr_next;
    اگر (timer->tr_next != پوچ)
	timer->tr_next->tr_prev = timer->tr_prev;
}

    ایستا تهی
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
create_timer(long msec, صحیح repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    long	prev_id = last_timer_id;

    اگر (timer == پوچ)
	بازگشت پوچ;
    اگر (++last_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    درج_timer(timer);
    اگر (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    بازگشت timer;
}

/*
 * (Re)start a timer.
 */
    تهی
timer_start(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    ایستا تهی
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    اگر (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(پوچ, "invoking timer callback %s",
	       cb->cb_partial != پوچ ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(پوچ, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    long
check_due_timer(تهی)
{
    timer_T	*timer;
    timer_T	*timer_next;
    long	this_due;
    long	next_due = -1;
    proftime_T	now;
    صحیح		did_one = FALSE;
    صحیح		need_update_screen = FALSE;
    long	current_id = last_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    اگر (exiting || aborting() || debug_mode)
	بازگشت next_due;

    profile_start(&now);
    برای (timer = first_timer; timer != پوچ && !got_int; timer = timer_next)
    {
	timer_next = timer->tr_next;

	اگر (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    ادامه;
	this_due = proftime_time_چپ(&timer->tr_due, &now);
	اگر (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    صحیح save_timer_busy = timer_busy;
	    صحیح save_vgetc_busy = vgetc_busy;
	    صحیح save_did_emsg = did_emsg;
	    صحیح prev_uncaught_emsg = uncaught_emsg;
	    صحیح save_called_emsg = called_emsg;
	    صحیح save_must_redraw = must_redraw;
	    صحیح save_ex_pressedreturn = get_pressedreturn();
	    صحیح save_may_garbage_collect = may_garbage_collect;
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
	    اگر (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_شمار;
	    وگرنه
		timer->tr_emsg_شمار = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    اگر (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    اگر (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_شمار < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_چپ(&timer->tr_due, &now);
		اگر (this_due < 1)
		    this_due = 1;
		اگر (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    وگرنه
	    {
		this_due = -1;
		اگر (timer->tr_keep)
		    timer->tr_paused = TRUE;
		وگرنه
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
	اگر (this_due > 0 && (next_due == -1 || next_due > this_due))
	    next_due = this_due;
    }

    اگر (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    اگر (bevalexpr_due_set)
    {
	this_due = proftime_time_چپ(&bevalexpr_due, &now);
	اگر (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    اگر (balloonEval == پوچ)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    اگر (balloonEval != پوچ)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	وگرنه اگر (next_due == -1 || next_due > this_due)
	    next_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    next_due = term_check_timers(next_due, &now);
#  endif

    بازگشت current_id != last_timer_id ? 1 : next_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    ایستا timer_T *
find_timer(long id)
{
    timer_T *timer;

    اگر (id < 0)
	بازگشت پوچ;

    FOR_ALL_TIMERS(timer)
	اگر (timer->tr_id == id)
	    بازگشت timer;
    بازگشت پوچ;
}


/*
 * Stop a timer and delete it.
 */
    تهی
stop_timer(timer_T *timer)
{
    اگر (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    وگرنه
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    ایستا تهی
stop_all_timers(تهی)
{
    timer_T *timer;
    timer_T *timer_next;

    برای (timer = first_timer; timer != پوچ; timer = timer_next)
    {
	timer_next = timer->tr_next;
	stop_timer(timer);
    }
}

    ایستا تهی
add_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    long	remaining;
    proftime_T	now;

    اگر (dict == پوچ)
	بازگشت;
    list_append_dict(list, dict);

    dict_add_عدد(dict, "id", timer->tr_id);
    dict_add_عدد(dict, "time", (long)timer->tr_interval);

    profile_start(&now);
    remaining = proftime_time_چپ(&timer->tr_due, &now);
    dict_add_عدد(dict, "remaining", (long)remaining);

    dict_add_عدد(dict, "repeat",
	    (long)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_add_عدد(dict, "paused", (long)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    اگر (di != پوچ)
    {
	اگر (dict_add(dict, di) == FAIL)
	    vim_free(di);
	وگرنه
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    ایستا تهی
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	اگر (timer->tr_id != -1)
	    add_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    صحیح
set_ref_in_timer(صحیح copyID)
{
    صحیح		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    برای (timer = first_timer; !abort && timer != پوچ; timer = timer->tr_next)
    {
	اگر (timer->tr_callback.cb_partial != پوچ)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	وگرنه
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);
    }
    بازگشت abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    صحیح
timer_valid(timer_T *timer)
{
    اگر (timer == پوچ)
	بازگشت FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	اگر (t == timer)
	    بازگشت TRUE;
    بازگشت FALSE;
}

#  اگر defined(EXITFREE)
    تهی
timer_free_all(تهی)
{
    وقتی (first_timer != پوچ)
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
    تهی
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = پوچ;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((صحیح)tv_get_عدد(&argvars[0]));
	اگر (timer != پوچ)
	    add_timer_info(rettv, timer);
    }
    وگرنه
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    تهی
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = پوچ;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[0].v_type != VAR_عدد)
    {
	emsg(_(e_عدد_expected));
	بازگشت;
    }

    صحیح	paused = (صحیح)tv_get_bool(&argvars[1]);

    timer = find_timer((صحیح)tv_get_عدد(&argvars[0]));
    اگر (timer != پوچ)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    تهی
f_timer_start(typval_T *argvars, typval_T *rettv)
{
    long	msec;
    timer_T	*timer;
    صحیح		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_عدد = -1;
    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	بازگشت;

    msec = (long)tv_get_عدد(&argvars[0]);
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	اگر (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    بازگشت;

	dict = argvars[2].vval.v_dict;
	اگر (dict_has_کلید(dict, "repeat"))
	    repeat = dict_get_عدد(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    اگر (callback.cb_name == پوچ)
	بازگشت;
    اگر (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	بازگشت;
    }

    timer = create_timer(msec, repeat);
    اگر (timer == پوچ)
    {
	free_callback(&callback);
	بازگشت;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_عدد = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    تهی
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    اگر (check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    timer = find_timer((صحیح)tv_get_عدد(&argvars[0]));
    اگر (timer != پوچ)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    تهی
f_timer_stopall(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# اگر defined(STARTUPTIME)
ایستا ساختار timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    ایستا صحیح
gettimeofday(ساختار timeval *tv, نویسه *dummy UNUSED)
{
    long t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    بازگشت 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    تهی
time_افزودن(تهی *tv_rel, تهی *tv_start)
{
    *((ساختار timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, پوچ);
    ((ساختار timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((ساختار timeval *)tv_rel)->tv_usec;
    ((ساختار timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((ساختار timeval *)tv_rel)->tv_sec;
    اگر (((ساختار timeval *)tv_rel)->tv_usec < 0)
    {
	((ساختار timeval *)tv_rel)->tv_usec += 1000000;
	--((ساختار timeval *)tv_rel)->tv_sec;
    }
    *(ساختار timeval *)tv_start = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    تهی
time_برداشت(
    تهی	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((ساختار timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((ساختار timeval *)tp)->tv_sec;
    اگر (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    ایستا تهی
time_diff(ساختار timeval *then, ساختار timeval *now)
{
    long	usec;
    long	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(time_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    تهی
time_پیام(
    نویسه	*mesg,
    تهی	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    ایستا ساختار timeval	start;
    ساختار timeval		now;

    اگر (time_fd == پوچ)
	بازگشت;

    اگر (strstr(mesg, "STARTING") != پوچ)
    {
	gettimeofday(&start, پوچ);
	prev_timeval = start;
	fprintf(time_fd, "\n\ntimes in msec\n");
	fprintf(time_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, پوچ);
    time_diff(&start, &now);
    اگر (((ساختار timeval *)tv_start) != پوچ)
    {
	fprintf(time_fd, "  ");
	time_diff(((ساختار timeval *)tv_start), &now);
    }
    fprintf(time_fd, "  ");
    time_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(time_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#endif // FEAT_EVAL

#اگر defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    time_T
get8ctime(FILE *fd)
{
    صحیح		c;
    time_T	n = 0;
    صحیح		i;

    برای (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	اگر (c == EOF) بازگشت -1;
	n = (n << 8) + c;
    }
    بازگشت n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    صحیح
put_time(FILE *fd, time_T the_time)
{
    char_u	buf[8];

    time_to_bytes(the_time, buf);
    بازگشت fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    تهی
time_to_bytes(time_T the_time, char_u *buf)
{
    صحیح		c;
    صحیح		i;
    صحیح		bi = 0;
    time_T	wtime = the_time;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    برای (i = 7; i >= 0; --i)
    {
	اگر (i + 1 > (صحیح)اندازه(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	وگرنه
	{
# اگر defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (صحیح)(wtime >> (i * 8));
# وگرنه
	    c = (صحیح)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#endif

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    تهی
add_time(char_u *buf, size_t buflen, time_t tt)
{
#ifdef HAVE_STRFTIME
    ساختار tm	tmval;
    ساختار tm	*curtime;
    size_t	n;

    اگر (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	اگر (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((نویسه *)buf, buflen, "%H:%M:%S", curtime);
	وگرنه
	    // longer ago
	    n = strftime((نویسه *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	اگر (n == 0)
	    buf[0] = NUL;
    }
    وگرنه
#endif
    {
	long seconds = (long)(vim_time() - tt);

	vim_snprintf((نویسه *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
