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

#অন্তর্ভাব "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#যদি defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
স্থির অক্ষর	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    জন্য ((t) = first_timer; (t) != ফাঁকা; (t) = (t)->tr_পরবর্তী)

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
    স্থির কাঠামো tm *
vim_localtime(
    ধ্রুবক time_t	*timep,		// timestamp for local representation
    কাঠামো tm		*ফলাফল UNUSED)	// pointer to caller return buffer
{
#ifdef HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    অক্ষর		*tz;		// pointer for TZ environment var

    tz = (অক্ষর *)mch_getenv((char_u *)"TZ");
    যদি (tz == ফাঁকা)
	tz = "";
    যদি (STRNCMP(tz_cache, tz, আকার(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, আকার(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    ফেরত localtime_r(timep, ফলাফল);
#অন্যথা
    ফেরত localtime(timep);
#endif	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    time_T
vim_time(শূন্য)
{
#ifdef FEAT_EVAL
    ফেরত time_for_testing == 0 ? time(ফাঁকা) : time_for_testing;
#অন্যথা
    ফেরত time(ফাঁকা);
#endif
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    অক্ষর *
get_ctime(time_t thetime, পূর্ণসংখ্যা add_newline)
{
    স্থির অক্ষর buf[100];  // hopefully enough for every language
#ifdef HAVE_STRFTIME
    কাঠামো tm	tmval;
    কাঠামো tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    যদি (curtime == ফাঁকা)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), আকার(buf) - 2);
    অন্যথা
    {
	// xgettext:no-c-format
	যদি (strftime(buf, আকার(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      আকার(buf) - 2);
	}
# ifdef MSWIN
	যদি (enc_codepage >= 0 && (পূর্ণসংখ্যা)GetACP() != enc_codepage)
	{
	    char_u	*to_free = ফাঁকা;
	    পূর্ণসংখ্যা		len;

	    acp_to_enc((char_u *)buf, (পূর্ণসংখ্যা)strlen(buf), &to_free, &len);
	    যদি (to_free != ফাঁকা)
	    {
		STRNCPY(buf, to_free, আকার(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#অন্যথা
    STRCPY(buf, "(unknown)");
#endif
    যদি (add_newline)
	STRCAT(buf, "\n");
    ফেরত buf;
}

#যদি defined(FEAT_EVAL)

# যদি defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    শূন্য
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = (varnumber_T)time(ফাঁকা);
}

# যদি defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    স্থির পূর্ণসংখ্যা
list2proftime(typval_T *arg, proftime_T *tm)
{
    long	n1, n2;
    পূর্ণসংখ্যা	error = FALSE;

    যদি (arg->v_type != VAR_LIST || arg->vval.v_list == ফাঁকা
					     || arg->vval.v_list->lv_len != 2)
	ফেরত FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->উপরের_Part = n1;
    tm->নিচের_Part = n2;
#  অন্যথা
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    ফেরত error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    শূন্য
f_reltime(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	start;
    long	n1, n2;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	ফেরত;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    অন্যথা যদি (argvars[1].v_type == VAR_UNKNOWN)
    {
	যদি (list2proftime(&argvars[0], &res) == FAIL)
	{
	    যদি (in_vim9script())
		emsg(_(e_invalid_argument));
	    ফেরত;
	}
	profile_end(&res);
    }
    অন্যথা
    {
	// Two arguments: compute the difference.
	যদি (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    যদি (in_vim9script())
		emsg(_(e_invalid_argument));
	    ফেরত;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.উপরের_Part;
    n2 = res.নিচের_Part;
#  অন্যথা
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)n1);
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    শূন্য
f_reltimefloat(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    যদি (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    অন্যথা যদি (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

/*
 * "reltimestr()" function
 */
    শূন্য
f_reltimestr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;
# ifdef FEAT_RELTIME
    যদি (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ফেরত;

    proftime_T	tm;
    যদি (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_বার্তা(&tm));
#  অন্যথা
	স্থির অক্ষর buf[50];
	long usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, আকার(buf), "%3ld.%06ld", (long)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    অন্যথা যদি (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# যদি defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    শূন্য
f_strftime(typval_T *argvars, typval_T *rettv)
{
    কাঠামো tm	tmval;
    কাঠামো tm	*curtime;
    time_t	seconds;
    char_u	*p;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    যদি (argvars[1].v_type == VAR_UNKNOWN)
	seconds = time(ফাঁকা);
    অন্যথা
	seconds = (time_t)tv_get_সংখ্যা(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    যদি (curtime == ফাঁকা)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	ফেরত;
    }

#  ifdef MSWIN
    WCHAR	    ফলাফল_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, ফাঁকা);
    যদি (wp != ফাঁকা)
	(শূন্য)wcsftime(ফলাফল_buf, ARRAY_দৈর্ঘ্য(ফলাফল_buf), wp, curtime);
    অন্যথা
	ফলাফল_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(ফলাফল_buf, ফাঁকা);
    vim_free(wp);
#  অন্যথা
    char_u	    ফলাফল_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    যদি (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, ফাঁকা);
    যদি (p == ফাঁকা || strftime((অক্ষর *)ফলাফল_buf, আকার(ফলাফল_buf),
		(অক্ষর *)p, curtime) == 0)
	ফলাফল_buf[0] = NUL;

    যদি (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    যদি (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, ফলাফল_buf, ফাঁকা);
    অন্যথা
	rettv->vval.v_string = vim_strsave(ফলাফল_buf);

    // Release conversion descriptors
    convert_setup(&conv, ফাঁকা, ফাঁকা);
    vim_free(enc);
#  endif
}
# endif

# যদি defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    শূন্য
f_strptime(typval_T *argvars, typval_T *rettv)
{
    কাঠামো tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    যদি (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, ফাঁকা);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    যদি (fmt == ফাঁকা
	    || strptime((অক্ষর *)str, (অক্ষর *)fmt, &tmval) == ফাঁকা
	    || (rettv->vval.v_সংখ্যা = mktime(&tmval)) == -1)
	rettv->vval.v_সংখ্যা = 0;

    যদি (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, ফাঁকা, ফাঁকা);
    vim_free(enc);
}
# endif

# যদি defined(FEAT_TIMERS)
স্থির timer_T	*first_timer = ফাঁকা;
স্থির long	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    long
proftime_time_বাম(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    যদি (now->QuadPart > due->QuadPart)
	ফেরত 0;
    QueryPerformanceFrequency(&fr);
    ফেরত (long)(((দ্বিগুণ)(due->QuadPart - now->QuadPart)
		   / (দ্বিগুণ)fr.QuadPart) * 1000);
#  অন্যথা
    যদি (now->tv_sec > due->tv_sec)
	ফেরত 0;
    ফেরত (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    স্থির শূন্য
ঢোকাও_timer(timer_T *timer)
{
    timer->tr_পরবর্তী = first_timer;
    timer->tr_পূর্ব = ফাঁকা;
    যদি (first_timer != ফাঁকা)
	first_timer->tr_পূর্ব = timer;
    first_timer = timer;
    did_add_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    স্থির শূন্য
remove_timer(timer_T *timer)
{
    যদি (timer->tr_পূর্ব == ফাঁকা)
	first_timer = timer->tr_পরবর্তী;
    অন্যথা
	timer->tr_পূর্ব->tr_পরবর্তী = timer->tr_পরবর্তী;
    যদি (timer->tr_পরবর্তী != ফাঁকা)
	timer->tr_পরবর্তী->tr_পূর্ব = timer->tr_পূর্ব;
}

    স্থির শূন্য
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
create_timer(long msec, পূর্ণসংখ্যা repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    long	পূর্ব_id = last_timer_id;

    যদি (timer == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (++last_timer_id <= পূর্ব_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    ঢোকাও_timer(timer);
    যদি (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    ফেরত timer;
}

/*
 * (Re)start a timer.
 */
    শূন্য
timer_start(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    স্থির শূন্য
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    যদি (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(ফাঁকা, "invoking timer callback %s",
	       cb->cb_partial != ফাঁকা ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_সংখ্যা;
    argv[0].vval.v_সংখ্যা = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(ফাঁকা, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    long
check_due_timer(শূন্য)
{
    timer_T	*timer;
    timer_T	*timer_পরবর্তী;
    long	this_due;
    long	পরবর্তী_due = -1;
    proftime_T	now;
    পূর্ণসংখ্যা		did_one = FALSE;
    পূর্ণসংখ্যা		need_update_screen = FALSE;
    long	current_id = last_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    যদি (exiting || aborting() || debug_mode)
	ফেরত পরবর্তী_due;

    profile_start(&now);
    জন্য (timer = first_timer; timer != ফাঁকা && !got_int; timer = timer_পরবর্তী)
    {
	timer_পরবর্তী = timer->tr_পরবর্তী;

	যদি (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    চালু;
	this_due = proftime_time_বাম(&timer->tr_due, &now);
	যদি (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    পূর্ণসংখ্যা save_timer_busy = timer_busy;
	    পূর্ণসংখ্যা save_vgetc_busy = vgetc_busy;
	    পূর্ণসংখ্যা save_did_emsg = did_emsg;
	    পূর্ণসংখ্যা পূর্ব_uncaught_emsg = uncaught_emsg;
	    পূর্ণসংখ্যা save_called_emsg = called_emsg;
	    পূর্ণসংখ্যা save_must_redraw = must_redraw;
	    পূর্ণসংখ্যা save_ex_pressedreturn = get_pressedreturn();
	    পূর্ণসংখ্যা save_may_garbage_collect = may_garbage_collect;
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
	    timer_পরবর্তী = timer->tr_পরবর্তী;
	    did_one = TRUE;
	    timer_busy = save_timer_busy;
	    vgetc_busy = save_vgetc_busy;
	    যদি (uncaught_emsg > পূর্ব_uncaught_emsg)
		++timer->tr_emsg_গুনতি;
	    অন্যথা
		timer->tr_emsg_গুনতি = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    যদি (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    যদি (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_গুনতি < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_বাম(&timer->tr_due, &now);
		যদি (this_due < 1)
		    this_due = 1;
		যদি (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    অন্যথা
	    {
		this_due = -1;
		যদি (timer->tr_keep)
		    timer->tr_paused = TRUE;
		অন্যথা
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
	যদি (this_due > 0 && (পরবর্তী_due == -1 || পরবর্তী_due > this_due))
	    পরবর্তী_due = this_due;
    }

    যদি (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    যদি (bevalexpr_due_set)
    {
	this_due = proftime_time_বাম(&bevalexpr_due, &now);
	যদি (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    যদি (balloonEval == ফাঁকা)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    যদি (balloonEval != ফাঁকা)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	অন্যথা যদি (পরবর্তী_due == -1 || পরবর্তী_due > this_due)
	    পরবর্তী_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    পরবর্তী_due = term_check_timers(পরবর্তী_due, &now);
#  endif

    ফেরত current_id != last_timer_id ? 1 : পরবর্তী_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    স্থির timer_T *
find_timer(long id)
{
    timer_T *timer;

    যদি (id < 0)
	ফেরত ফাঁকা;

    FOR_ALL_TIMERS(timer)
	যদি (timer->tr_id == id)
	    ফেরত timer;
    ফেরত ফাঁকা;
}


/*
 * Stop a timer and delete it.
 */
    শূন্য
stop_timer(timer_T *timer)
{
    যদি (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    অন্যথা
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    স্থির শূন্য
stop_all_timers(শূন্য)
{
    timer_T *timer;
    timer_T *timer_পরবর্তী;

    জন্য (timer = first_timer; timer != ফাঁকা; timer = timer_পরবর্তী)
    {
	timer_পরবর্তী = timer->tr_পরবর্তী;
	stop_timer(timer);
    }
}

    স্থির শূন্য
add_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    long	remaining;
    proftime_T	now;

    যদি (dict == ফাঁকা)
	ফেরত;
    list_append_dict(list, dict);

    dict_add_সংখ্যা(dict, "id", timer->tr_id);
    dict_add_সংখ্যা(dict, "time", (long)timer->tr_interval);

    profile_start(&now);
    remaining = proftime_time_বাম(&timer->tr_due, &now);
    dict_add_সংখ্যা(dict, "remaining", (long)remaining);

    dict_add_সংখ্যা(dict, "repeat",
	    (long)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_add_সংখ্যা(dict, "paused", (long)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    যদি (di != ফাঁকা)
    {
	যদি (dict_add(dict, di) == FAIL)
	    vim_free(di);
	অন্যথা
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    স্থির শূন্য
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	যদি (timer->tr_id != -1)
	    add_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    পূর্ণসংখ্যা
set_ref_in_timer(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    জন্য (timer = first_timer; !abort && timer != ফাঁকা; timer = timer->tr_পরবর্তী)
    {
	যদি (timer->tr_callback.cb_partial != ফাঁকা)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	অন্যথা
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || set_ref_in_item(&tv, copyID, ফাঁকা, ফাঁকা, ফাঁকা);
    }
    ফেরত abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    পূর্ণসংখ্যা
timer_valid(timer_T *timer)
{
    যদি (timer == ফাঁকা)
	ফেরত FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	যদি (t == timer)
	    ফেরত TRUE;
    ফেরত FALSE;
}

#  যদি defined(EXITFREE)
    শূন্য
timer_free_all(শূন্য)
{
    যখন (first_timer != ফাঁকা)
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
    শূন্য
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = ফাঁকা;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]));
	যদি (timer != ফাঁকা)
	    add_timer_info(rettv, timer);
    }
    অন্যথা
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    শূন্য
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[0].v_type != VAR_সংখ্যা)
    {
	emsg(_(e_সংখ্যা_expected));
	ফেরত;
    }

    পূর্ণসংখ্যা	paused = (পূর্ণসংখ্যা)tv_get_bool(&argvars[1]);

    timer = find_timer((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]));
    যদি (timer != ফাঁকা)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    শূন্য
f_timer_start(typval_T *argvars, typval_T *rettv)
{
    long	msec;
    timer_T	*timer;
    পূর্ণসংখ্যা		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_সংখ্যা = -1;
    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ফেরত;

    msec = (long)tv_get_সংখ্যা(&argvars[0]);
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	যদি (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ফেরত;

	dict = argvars[2].vval.v_dict;
	যদি (dict_has_চাবি(dict, "repeat"))
	    repeat = dict_get_সংখ্যা(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    যদি (callback.cb_name == ফাঁকা)
	ফেরত;
    যদি (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	ফেরত;
    }

    timer = create_timer(msec, repeat);
    যদি (timer == ফাঁকা)
    {
	free_callback(&callback);
	ফেরত;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_সংখ্যা = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    শূন্য
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    যদি (check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    timer = find_timer((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]));
    যদি (timer != ফাঁকা)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    শূন্য
f_timer_stopall(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# যদি defined(STARTUPTIME)
স্থির কাঠামো timeval	পূর্ব_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    স্থির পূর্ণসংখ্যা
gettimeofday(কাঠামো timeval *tv, অক্ষর *dummy UNUSED)
{
    long t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    ফেরত 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    শূন্য
time_ঠেলো(শূন্য *tv_rel, শূন্য *tv_start)
{
    *((কাঠামো timeval *)tv_rel) = পূর্ব_timeval;
    gettimeofday(&পূর্ব_timeval, ফাঁকা);
    ((কাঠামো timeval *)tv_rel)->tv_usec = পূর্ব_timeval.tv_usec
					- ((কাঠামো timeval *)tv_rel)->tv_usec;
    ((কাঠামো timeval *)tv_rel)->tv_sec = পূর্ব_timeval.tv_sec
					 - ((কাঠামো timeval *)tv_rel)->tv_sec;
    যদি (((কাঠামো timeval *)tv_rel)->tv_usec < 0)
    {
	((কাঠামো timeval *)tv_rel)->tv_usec += 1000000;
	--((কাঠামো timeval *)tv_rel)->tv_sec;
    }
    *(কাঠামো timeval *)tv_start = পূর্ব_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    শূন্য
time_বের_করো(
    শূন্য	*tp)	// actually (struct timeval *)
{
    পূর্ব_timeval.tv_usec -= ((কাঠামো timeval *)tp)->tv_usec;
    পূর্ব_timeval.tv_sec -= ((কাঠামো timeval *)tp)->tv_sec;
    যদি (পূর্ব_timeval.tv_usec < 0)
    {
	পূর্ব_timeval.tv_usec += 1000000;
	--পূর্ব_timeval.tv_sec;
    }
}

    স্থির শূন্য
time_diff(কাঠামো timeval *then, কাঠামো timeval *now)
{
    long	usec;
    long	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(time_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    শূন্য
time_বার্তা(
    অক্ষর	*mesg,
    শূন্য	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    স্থির কাঠামো timeval	start;
    কাঠামো timeval		now;

    যদি (time_fd == ফাঁকা)
	ফেরত;

    যদি (strstr(mesg, "STARTING") != ফাঁকা)
    {
	gettimeofday(&start, ফাঁকা);
	পূর্ব_timeval = start;
	fprintf(time_fd, "\n\ntimes in msec\n");
	fprintf(time_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, ফাঁকা);
    time_diff(&start, &now);
    যদি (((কাঠামো timeval *)tv_start) != ফাঁকা)
    {
	fprintf(time_fd, "  ");
	time_diff(((কাঠামো timeval *)tv_start), &now);
    }
    fprintf(time_fd, "  ");
    time_diff(&পূর্ব_timeval, &now);
    পূর্ব_timeval = now;
    fprintf(time_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#endif // FEAT_EVAL

#যদি defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    time_T
get8ctime(FILE *fd)
{
    পূর্ণসংখ্যা		c;
    time_T	n = 0;
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	যদি (c == EOF) ফেরত -1;
	n = (n << 8) + c;
    }
    ফেরত n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    পূর্ণসংখ্যা
put_time(FILE *fd, time_T the_time)
{
    char_u	buf[8];

    time_to_bytes(the_time, buf);
    ফেরত fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    শূন্য
time_to_bytes(time_T the_time, char_u *buf)
{
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		bi = 0;
    time_T	wtime = the_time;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    জন্য (i = 7; i >= 0; --i)
    {
	যদি (i + 1 > (পূর্ণসংখ্যা)আকার(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	অন্যথা
	{
# যদি defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (পূর্ণসংখ্যা)(wtime >> (i * 8));
# অন্যথা
	    c = (পূর্ণসংখ্যা)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#endif

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    শূন্য
add_time(char_u *buf, size_t buflen, time_t tt)
{
#ifdef HAVE_STRFTIME
    কাঠামো tm	tmval;
    কাঠামো tm	*curtime;
    size_t	n;

    যদি (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	যদি (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((অক্ষর *)buf, buflen, "%H:%M:%S", curtime);
	অন্যথা
	    // longer ago
	    n = strftime((অক্ষর *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	যদি (n == 0)
	    buf[0] = NUL;
    }
    অন্যথা
#endif
    {
	long seconds = (long)(vim_time() - tt);

	vim_snprintf((অক্ষর *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
