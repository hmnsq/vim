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

#ضمن "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#ܐܢ defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
ساكن حرف	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    لكل ((t) = first_timer; (t) != عدم; (t) = (t)->tr_next)

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
    ساكن بنية tm *
vim_localtime(
    ثابت time_t	*timep,		// timestamp for local representation
    بنية tm		*result UNUSED)	// pointer to caller return buffer
{
#ifdef HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    حرف		*tz;		// pointer for TZ environment var

    tz = (حرف *)mch_getenv((char_u *)"TZ");
    ܐܢ (tz == عدم)
	tz = "";
    ܐܢ (STRNCMP(tz_cache, tz, حجم(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, حجم(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    ܦܢܐ localtime_r(timep, result);
#ܐܚܪܢܐ
    ܦܢܐ localtime(timep);
#endif	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    time_T
vim_time(فراغ)
{
#ifdef FEAT_EVAL
    ܦܢܐ time_for_testing == 0 ? time(عدم) : time_for_testing;
#ܐܚܪܢܐ
    ܦܢܐ time(عدم);
#endif
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    حرف *
get_ctime(time_t thetime, صحيح add_newline)
{
    ساكن حرف buf[100];  // hopefully enough for every language
#ifdef HAVE_STRFTIME
    بنية tm	tmval;
    بنية tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ܐܢ (curtime == عدم)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), حجم(buf) - 2);
    ܐܚܪܢܐ
    {
	// xgettext:no-c-format
	ܐܢ (strftime(buf, حجم(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      حجم(buf) - 2);
	}
# ifdef MSWIN
	ܐܢ (enc_codepage >= 0 && (صحيح)GetACP() != enc_codepage)
	{
	    char_u	*to_free = عدم;
	    صحيح		len;

	    acp_to_enc((char_u *)buf, (صحيح)strlen(buf), &to_free, &len);
	    ܐܢ (to_free != عدم)
	    {
		STRNCPY(buf, to_free, حجم(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#ܐܚܪܢܐ
    STRCPY(buf, "(unknown)");
#endif
    ܐܢ (add_newline)
	STRCAT(buf, "\n");
    ܦܢܐ buf;
}

#ܐܢ defined(FEAT_EVAL)

# ܐܢ defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    فراغ
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = (varnumber_T)time(عدم);
}

# ܐܢ defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    ساكن صحيح
list2proftime(typval_T *arg, proftime_T *tm)
{
    long	n1, n2;
    صحيح	error = FALSE;

    ܐܢ (arg->v_type != VAR_LIST || arg->vval.v_list == عدم
					     || arg->vval.v_list->lv_len != 2)
	ܦܢܐ FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->HighPart = n1;
    tm->LowPart = n2;
#  ܐܚܪܢܐ
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    ܦܢܐ error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    فراغ
f_reltime(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	start;
    long	n1, n2;

    ܐܢ (rettv_list_alloc(rettv) == FAIL)
	ܦܢܐ;

    ܐܢ (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	ܦܢܐ;

    ܐܢ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    ܐܚܪܢܐ ܐܢ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ܐܢ (list2proftime(&argvars[0], &res) == FAIL)
	{
	    ܐܢ (in_vim9script())
		emsg(_(e_invalid_argument));
	    ܦܢܐ;
	}
	profile_end(&res);
    }
    ܐܚܪܢܐ
    {
	// Two arguments: compute the difference.
	ܐܢ (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    ܐܢ (in_vim9script())
		emsg(_(e_invalid_argument));
	    ܦܢܐ;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.HighPart;
    n2 = res.LowPart;
#  ܐܚܪܢܐ
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_number(rettv->vval.v_list, (varnumber_T)n1);
    list_append_number(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    فراغ
f_reltimefloat(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    ܐܢ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    ܐܢ (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    ܐܚܪܢܐ ܐܢ (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

/*
 * "reltimestr()" function
 */
    فراغ
f_reltimestr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
# ifdef FEAT_RELTIME
    ܐܢ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    proftime_T	tm;
    ܐܢ (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_msg(&tm));
#  ܐܚܪܢܐ
	ساكن حرف buf[50];
	long usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, حجم(buf), "%3ld.%06ld", (long)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    ܐܚܪܢܐ ܐܢ (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# ܐܢ defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    فراغ
f_strftime(typval_T *argvars, typval_T *rettv)
{
    بنية tm	tmval;
    بنية tm	*curtime;
    time_t	seconds;
    char_u	*p;

    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    ܐܢ (argvars[1].v_type == VAR_UNKNOWN)
	seconds = time(عدم);
    ܐܚܪܢܐ
	seconds = (time_t)tv_get_number(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ܐܢ (curtime == عدم)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	ܦܢܐ;
    }

#  ifdef MSWIN
    WCHAR	    result_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, عدم);
    ܐܢ (wp != عدم)
	(فراغ)wcsftime(result_buf, ARRAY_LENGTH(result_buf), wp, curtime);
    ܐܚܪܢܐ
	result_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(result_buf, عدم);
    vim_free(wp);
#  ܐܚܪܢܐ
    char_u	    result_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    ܐܢ (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, عدم);
    ܐܢ (p == عدم || strftime((حرف *)result_buf, حجم(result_buf),
		(حرف *)p, curtime) == 0)
	result_buf[0] = NUL;

    ܐܢ (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    ܐܢ (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, result_buf, عدم);
    ܐܚܪܢܐ
	rettv->vval.v_string = vim_strsave(result_buf);

    // Release conversion descriptors
    convert_setup(&conv, عدم, عدم);
    vim_free(enc);
#  endif
}
# endif

# ܐܢ defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    فراغ
f_strptime(typval_T *argvars, typval_T *rettv)
{
    بنية tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    ܐܢ (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, عدم);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    ܐܢ (fmt == عدم
	    || strptime((حرف *)str, (حرف *)fmt, &tmval) == عدم
	    || (rettv->vval.v_number = mktime(&tmval)) == -1)
	rettv->vval.v_number = 0;

    ܐܢ (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, عدم, عدم);
    vim_free(enc);
}
# endif

# ܐܢ defined(FEAT_TIMERS)
ساكن timer_T	*first_timer = عدم;
ساكن long	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    long
proftime_time_left(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    ܐܢ (now->QuadPart > due->QuadPart)
	ܦܢܐ 0;
    QueryPerformanceFrequency(&fr);
    ܦܢܐ (long)(((مزدوج)(due->QuadPart - now->QuadPart)
		   / (مزدوج)fr.QuadPart) * 1000);
#  ܐܚܪܢܐ
    ܐܢ (now->tv_sec > due->tv_sec)
	ܦܢܐ 0;
    ܦܢܐ (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    ساكن فراغ
insert_timer(timer_T *timer)
{
    timer->tr_next = first_timer;
    timer->tr_prev = عدم;
    ܐܢ (first_timer != عدم)
	first_timer->tr_prev = timer;
    first_timer = timer;
    did_add_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    ساكن فراغ
remove_timer(timer_T *timer)
{
    ܐܢ (timer->tr_prev == عدم)
	first_timer = timer->tr_next;
    ܐܚܪܢܐ
	timer->tr_prev->tr_next = timer->tr_next;
    ܐܢ (timer->tr_next != عدم)
	timer->tr_next->tr_prev = timer->tr_prev;
}

    ساكن فراغ
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
create_timer(long msec, صحيح repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    long	prev_id = last_timer_id;

    ܐܢ (timer == عدم)
	ܦܢܐ عدم;
    ܐܢ (++last_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    insert_timer(timer);
    ܐܢ (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    ܦܢܐ timer;
}

/*
 * (Re)start a timer.
 */
    فراغ
timer_start(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    ساكن فراغ
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    ܐܢ (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(عدم, "invoking timer callback %s",
	       cb->cb_partial != عدم ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_NUMBER;
    argv[0].vval.v_number = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(عدم, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    long
check_due_timer(فراغ)
{
    timer_T	*timer;
    timer_T	*timer_next;
    long	this_due;
    long	next_due = -1;
    proftime_T	now;
    صحيح		did_one = FALSE;
    صحيح		need_update_screen = FALSE;
    long	current_id = last_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    ܐܢ (exiting || aborting() || debug_mode)
	ܦܢܐ next_due;

    profile_start(&now);
    لكل (timer = first_timer; timer != عدم && !got_int; timer = timer_next)
    {
	timer_next = timer->tr_next;

	ܐܢ (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    استمر;
	this_due = proftime_time_left(&timer->tr_due, &now);
	ܐܢ (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    صحيح save_timer_busy = timer_busy;
	    صحيح save_vgetc_busy = vgetc_busy;
	    صحيح save_did_emsg = did_emsg;
	    صحيح prev_uncaught_emsg = uncaught_emsg;
	    صحيح save_called_emsg = called_emsg;
	    صحيح save_must_redraw = must_redraw;
	    صحيح save_ex_pressedreturn = get_pressedreturn();
	    صحيح save_may_garbage_collect = may_garbage_collect;
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
	    ܐܢ (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_count;
	    ܐܚܪܢܐ
		timer->tr_emsg_count = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    ܐܢ (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    ܐܢ (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_count < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_left(&timer->tr_due, &now);
		ܐܢ (this_due < 1)
		    this_due = 1;
		ܐܢ (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    ܐܚܪܢܐ
	    {
		this_due = -1;
		ܐܢ (timer->tr_keep)
		    timer->tr_paused = TRUE;
		ܐܚܪܢܐ
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
	ܐܢ (this_due > 0 && (next_due == -1 || next_due > this_due))
	    next_due = this_due;
    }

    ܐܢ (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    ܐܢ (bevalexpr_due_set)
    {
	this_due = proftime_time_left(&bevalexpr_due, &now);
	ܐܢ (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    ܐܢ (balloonEval == عدم)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    ܐܢ (balloonEval != عدم)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	ܐܚܪܢܐ ܐܢ (next_due == -1 || next_due > this_due)
	    next_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    next_due = term_check_timers(next_due, &now);
#  endif

    ܦܢܐ current_id != last_timer_id ? 1 : next_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    ساكن timer_T *
find_timer(long id)
{
    timer_T *timer;

    ܐܢ (id < 0)
	ܦܢܐ عدم;

    FOR_ALL_TIMERS(timer)
	ܐܢ (timer->tr_id == id)
	    ܦܢܐ timer;
    ܦܢܐ عدم;
}


/*
 * Stop a timer and delete it.
 */
    فراغ
stop_timer(timer_T *timer)
{
    ܐܢ (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    ܐܚܪܢܐ
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    ساكن فراغ
stop_all_timers(فراغ)
{
    timer_T *timer;
    timer_T *timer_next;

    لكل (timer = first_timer; timer != عدم; timer = timer_next)
    {
	timer_next = timer->tr_next;
	stop_timer(timer);
    }
}

    ساكن فراغ
add_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    long	remaining;
    proftime_T	now;

    ܐܢ (dict == عدم)
	ܦܢܐ;
    list_append_dict(list, dict);

    dict_add_number(dict, "id", timer->tr_id);
    dict_add_number(dict, "time", (long)timer->tr_interval);

    profile_start(&now);
    remaining = proftime_time_left(&timer->tr_due, &now);
    dict_add_number(dict, "remaining", (long)remaining);

    dict_add_number(dict, "repeat",
	    (long)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_add_number(dict, "paused", (long)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    ܐܢ (di != عدم)
    {
	ܐܢ (dict_add(dict, di) == FAIL)
	    vim_free(di);
	ܐܚܪܢܐ
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    ساكن فراغ
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	ܐܢ (timer->tr_id != -1)
	    add_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    صحيح
set_ref_in_timer(صحيح copyID)
{
    صحيح		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    لكل (timer = first_timer; !abort && timer != عدم; timer = timer->tr_next)
    {
	ܐܢ (timer->tr_callback.cb_partial != عدم)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	ܐܚܪܢܐ
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || set_ref_in_item(&tv, copyID, عدم, عدم, عدم);
    }
    ܦܢܐ abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    صحيح
timer_valid(timer_T *timer)
{
    ܐܢ (timer == عدم)
	ܦܢܐ FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	ܐܢ (t == timer)
	    ܦܢܐ TRUE;
    ܦܢܐ FALSE;
}

#  ܐܢ defined(EXITFREE)
    فراغ
timer_free_all(فراغ)
{
    ܟܡܐ (first_timer != عدم)
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
    فراغ
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = عدم;

    ܐܢ (rettv_list_alloc(rettv) == FAIL)
	ܦܢܐ;

    ܐܢ (check_for_opt_number_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    ܐܢ (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((صحيح)tv_get_number(&argvars[0]));
	ܐܢ (timer != عدم)
	    add_timer_info(rettv, timer);
    }
    ܐܚܪܢܐ
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    فراغ
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = عدم;

    ܐܢ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    ܐܢ (argvars[0].v_type != VAR_NUMBER)
    {
	emsg(_(e_number_expected));
	ܦܢܐ;
    }

    صحيح	paused = (صحيح)tv_get_bool(&argvars[1]);

    timer = find_timer((صحيح)tv_get_number(&argvars[0]));
    ܐܢ (timer != عدم)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    فراغ
f_timer_start(typval_T *argvars, typval_T *rettv)
{
    long	msec;
    timer_T	*timer;
    صحيح		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_number = -1;
    ܐܢ (check_secure())
	ܦܢܐ;

    ܐܢ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ܦܢܐ;

    msec = (long)tv_get_number(&argvars[0]);
    ܐܢ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ܐܢ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ܦܢܐ;

	dict = argvars[2].vval.v_dict;
	ܐܢ (dict_has_key(dict, "repeat"))
	    repeat = dict_get_number(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    ܐܢ (callback.cb_name == عدم)
	ܦܢܐ;
    ܐܢ (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	ܦܢܐ;
    }

    timer = create_timer(msec, repeat);
    ܐܢ (timer == عدم)
    {
	free_callback(&callback);
	ܦܢܐ;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_number = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    فراغ
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    ܐܢ (check_for_number_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    timer = find_timer((صحيح)tv_get_number(&argvars[0]));
    ܐܢ (timer != عدم)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    فراغ
f_timer_stopall(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# ܐܢ defined(STARTUPTIME)
ساكن بنية timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    ساكن صحيح
gettimeofday(بنية timeval *tv, حرف *dummy UNUSED)
{
    long t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    ܦܢܐ 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    فراغ
time_push(فراغ *tv_rel, فراغ *tv_start)
{
    *((بنية timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, عدم);
    ((بنية timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((بنية timeval *)tv_rel)->tv_usec;
    ((بنية timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((بنية timeval *)tv_rel)->tv_sec;
    ܐܢ (((بنية timeval *)tv_rel)->tv_usec < 0)
    {
	((بنية timeval *)tv_rel)->tv_usec += 1000000;
	--((بنية timeval *)tv_rel)->tv_sec;
    }
    *(بنية timeval *)tv_start = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    فراغ
time_pop(
    فراغ	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((بنية timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((بنية timeval *)tp)->tv_sec;
    ܐܢ (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    ساكن فراغ
time_diff(بنية timeval *then, بنية timeval *now)
{
    long	usec;
    long	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(time_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    فراغ
time_msg(
    حرف	*mesg,
    فراغ	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    ساكن بنية timeval	start;
    بنية timeval		now;

    ܐܢ (time_fd == عدم)
	ܦܢܐ;

    ܐܢ (strstr(mesg, "STARTING") != عدم)
    {
	gettimeofday(&start, عدم);
	prev_timeval = start;
	fprintf(time_fd, "\n\ntimes in msec\n");
	fprintf(time_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, عدم);
    time_diff(&start, &now);
    ܐܢ (((بنية timeval *)tv_start) != عدم)
    {
	fprintf(time_fd, "  ");
	time_diff(((بنية timeval *)tv_start), &now);
    }
    fprintf(time_fd, "  ");
    time_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(time_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#endif // FEAT_EVAL

#ܐܢ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    time_T
get8ctime(FILE *fd)
{
    صحيح		c;
    time_T	n = 0;
    صحيح		i;

    لكل (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	ܐܢ (c == EOF) ܦܢܐ -1;
	n = (n << 8) + c;
    }
    ܦܢܐ n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    صحيح
put_time(FILE *fd, time_T the_time)
{
    char_u	buf[8];

    time_to_bytes(the_time, buf);
    ܦܢܐ fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    فراغ
time_to_bytes(time_T the_time, char_u *buf)
{
    صحيح		c;
    صحيح		i;
    صحيح		bi = 0;
    time_T	wtime = the_time;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    لكل (i = 7; i >= 0; --i)
    {
	ܐܢ (i + 1 > (صحيح)حجم(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	ܐܚܪܢܐ
	{
# ܐܢ defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (صحيح)(wtime >> (i * 8));
# ܐܚܪܢܐ
	    c = (صحيح)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#endif

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    فراغ
add_time(char_u *buf, size_t buflen, time_t tt)
{
#ifdef HAVE_STRFTIME
    بنية tm	tmval;
    بنية tm	*curtime;
    size_t	n;

    ܐܢ (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	ܐܢ (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((حرف *)buf, buflen, "%H:%M:%S", curtime);
	ܐܚܪܢܐ
	    // longer ago
	    n = strftime((حرف *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	ܐܢ (n == 0)
	    buf[0] = NUL;
    }
    ܐܚܪܢܐ
#endif
    {
	long seconds = (long)(vim_time() - tt);

	vim_snprintf((حرف *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
