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

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#ಯದಿ defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
ಸ್ಥಿರ ವರ್ಣ	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    ಪ್ರತಿ ((t) = first_timer; (t) != ರಿಕ್ತ; (t) = (t)->tr_next)

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
    ಸ್ಥಿರ ಸಂರಚನಾ tm *
vim_localtime(
    ಧ್ರುವ time_t	*timep,		// timestamp for local representation
    ಸಂರಚನಾ tm		*result UNUSED)	// pointer to caller return buffer
{
#ifdef HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    ವರ್ಣ		*tz;		// pointer for TZ environment var

    tz = (ವರ್ಣ *)mch_getenv((char_u *)"TZ");
    ಯದಿ (tz == ರಿಕ್ತ)
	tz = "";
    ಯದಿ (STRNCMP(tz_cache, tz, ಪರಿಮಾಣ(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, ಪರಿಮಾಣ(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    ಪ್ರತ್ಯಾಗಮ localtime_r(timep, result);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ localtime(timep);
#endif	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    time_T
vim_time(ಶೂನ್ಯ)
{
#ifdef FEAT_EVAL
    ಪ್ರತ್ಯಾಗಮ time_for_testing == 0 ? time(ರಿಕ್ತ) : time_for_testing;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ time(ರಿಕ್ತ);
#endif
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    ವರ್ಣ *
get_ctime(time_t thetime, ಪೂರ್ಣಾಙ್ಕ add_newline)
{
    ಸ್ಥಿರ ವರ್ಣ buf[100];  // hopefully enough for every language
#ifdef HAVE_STRFTIME
    ಸಂರಚನಾ tm	tmval;
    ಸಂರಚನಾ tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ಯದಿ (curtime == ರಿಕ್ತ)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), ಪರಿಮಾಣ(buf) - 2);
    ಅನ್ಯಥಾ
    {
	// xgettext:no-c-format
	ಯದಿ (strftime(buf, ಪರಿಮಾಣ(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      ಪರಿಮಾಣ(buf) - 2);
	}
# ifdef MSWIN
	ಯದಿ (enc_codepage >= 0 && (ಪೂರ್ಣಾಙ್ಕ)GetACP() != enc_codepage)
	{
	    char_u	*to_free = ರಿಕ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ		len;

	    acp_to_enc((char_u *)buf, (ಪೂರ್ಣಾಙ್ಕ)strlen(buf), &to_free, &len);
	    ಯದಿ (to_free != ರಿಕ್ತ)
	    {
		STRNCPY(buf, to_free, ಪರಿಮಾಣ(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#ಅನ್ಯಥಾ
    STRCPY(buf, "(unknown)");
#endif
    ಯದಿ (add_newline)
	STRCAT(buf, "\n");
    ಪ್ರತ್ಯಾಗಮ buf;
}

#ಯದಿ defined(FEAT_EVAL)

# ಯದಿ defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    ಶೂನ್ಯ
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)time(ರಿಕ್ತ);
}

# ಯದಿ defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
list2proftime(typval_T *arg, proftime_T *tm)
{
    long	n1, n2;
    ಪೂರ್ಣಾಙ್ಕ	error = FALSE;

    ಯದಿ (arg->v_type != VAR_LIST || arg->vval.v_list == ರಿಕ್ತ
					     || arg->vval.v_list->lv_len != 2)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->ಮೇಲಿನ_Part = n1;
    tm->ಕೆಳಗಿನ_Part = n2;
#  ಅನ್ಯಥಾ
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    ಪ್ರತ್ಯಾಗಮ error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    ಶೂನ್ಯ
f_reltime(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	start;
    long	n1, n2;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ಯದಿ (list2proftime(&argvars[0], &res) == FAIL)
	{
	    ಯದಿ (in_vim9script())
		emsg(_(e_invalid_argument));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	profile_end(&res);
    }
    ಅನ್ಯಥಾ
    {
	// Two arguments: compute the difference.
	ಯದಿ (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    ಯದಿ (in_vim9script())
		emsg(_(e_invalid_argument));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.ಮೇಲಿನ_Part;
    n2 = res.ಕೆಳಗಿನ_Part;
#  ಅನ್ಯಥಾ
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)n1);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    ಶೂನ್ಯ
f_reltimefloat(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    ಯದಿ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    ಅನ್ಯಥಾ ಯದಿ (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

/*
 * "reltimestr()" function
 */
    ಶೂನ್ಯ
f_reltimestr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;
# ifdef FEAT_RELTIME
    ಯದಿ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    proftime_T	tm;
    ಯದಿ (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_ಸಂದೇಶ(&tm));
#  ಅನ್ಯಥಾ
	ಸ್ಥಿರ ವರ್ಣ buf[50];
	long usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, ಪರಿಮಾಣ(buf), "%3ld.%06ld", (long)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    ಅನ್ಯಥಾ ಯದಿ (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# ಯದಿ defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    ಶೂನ್ಯ
f_strftime(typval_T *argvars, typval_T *rettv)
{
    ಸಂರಚನಾ tm	tmval;
    ಸಂರಚನಾ tm	*curtime;
    time_t	seconds;
    char_u	*p;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
	seconds = time(ರಿಕ್ತ);
    ಅನ್ಯಥಾ
	seconds = (time_t)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ಯದಿ (curtime == ರಿಕ್ತ)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	ಪ್ರತ್ಯಾಗಮ;
    }

#  ifdef MSWIN
    WCHAR	    result_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, ರಿಕ್ತ);
    ಯದಿ (wp != ರಿಕ್ತ)
	(ಶೂನ್ಯ)wcsftime(result_buf, ARRAY_LENGTH(result_buf), wp, curtime);
    ಅನ್ಯಥಾ
	result_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(result_buf, ರಿಕ್ತ);
    vim_free(wp);
#  ಅನ್ಯಥಾ
    char_u	    result_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    ಯದಿ (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ || strftime((ವರ್ಣ *)result_buf, ಪರಿಮಾಣ(result_buf),
		(ವರ್ಣ *)p, curtime) == 0)
	result_buf[0] = NUL;

    ಯದಿ (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    ಯದಿ (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, result_buf, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
	rettv->vval.v_string = vim_strsave(result_buf);

    // Release conversion descriptors
    convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);
    vim_free(enc);
#  endif
}
# endif

# ಯದಿ defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    ಶೂನ್ಯ
f_strptime(typval_T *argvars, typval_T *rettv)
{
    ಸಂರಚನಾ tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    ಯದಿ (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, ರಿಕ್ತ);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    ಯದಿ (fmt == ರಿಕ್ತ
	    || strptime((ವರ್ಣ *)str, (ವರ್ಣ *)fmt, &tmval) == ರಿಕ್ತ
	    || (rettv->vval.v_ಸಂಖ್ಯೆ = mktime(&tmval)) == -1)
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಯದಿ (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);
    vim_free(enc);
}
# endif

# ಯದಿ defined(FEAT_TIMERS)
ಸ್ಥಿರ timer_T	*first_timer = ರಿಕ್ತ;
ಸ್ಥಿರ long	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    long
proftime_time_ಎಡ(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    ಯದಿ (now->QuadPart > due->QuadPart)
	ಪ್ರತ್ಯಾಗಮ 0;
    QueryPerformanceFrequency(&fr);
    ಪ್ರತ್ಯಾಗಮ (long)(((ದ್ವಿಗುಣ)(due->QuadPart - now->QuadPart)
		   / (ದ್ವಿಗುಣ)fr.QuadPart) * 1000);
#  ಅನ್ಯಥಾ
    ಯದಿ (now->tv_sec > due->tv_sec)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೇರಿಸು_timer(timer_T *timer)
{
    timer->tr_next = first_timer;
    timer->tr_prev = ರಿಕ್ತ;
    ಯದಿ (first_timer != ರಿಕ್ತ)
	first_timer->tr_prev = timer;
    first_timer = timer;
    did_add_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_timer(timer_T *timer)
{
    ಯದಿ (timer->tr_prev == ರಿಕ್ತ)
	first_timer = timer->tr_next;
    ಅನ್ಯಥಾ
	timer->tr_prev->tr_next = timer->tr_next;
    ಯದಿ (timer->tr_next != ರಿಕ್ತ)
	timer->tr_next->tr_prev = timer->tr_prev;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
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
create_timer(long msec, ಪೂರ್ಣಾಙ್ಕ repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    long	prev_id = last_timer_id;

    ಯದಿ (timer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (++last_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    ಸೇರಿಸು_timer(timer);
    ಯದಿ (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    ಪ್ರತ್ಯಾಗಮ timer;
}

/*
 * (Re)start a timer.
 */
    ಶೂನ್ಯ
timer_start(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    ಯದಿ (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(ರಿಕ್ತ, "invoking timer callback %s",
	       cb->cb_partial != ರಿಕ್ತ ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(ರಿಕ್ತ, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    long
check_due_timer(ಶೂನ್ಯ)
{
    timer_T	*timer;
    timer_T	*timer_next;
    long	this_due;
    long	next_due = -1;
    proftime_T	now;
    ಪೂರ್ಣಾಙ್ಕ		did_one = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		need_update_screen = FALSE;
    long	current_id = last_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    ಯದಿ (exiting || aborting() || debug_mode)
	ಪ್ರತ್ಯಾಗಮ next_due;

    profile_start(&now);
    ಪ್ರತಿ (timer = first_timer; timer != ರಿಕ್ತ && !got_int; timer = timer_next)
    {
	timer_next = timer->tr_next;

	ಯದಿ (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    ಅನುವರ್ತ;
	this_due = proftime_time_ಎಡ(&timer->tr_due, &now);
	ಯದಿ (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    ಪೂರ್ಣಾಙ್ಕ save_timer_busy = timer_busy;
	    ಪೂರ್ಣಾಙ್ಕ save_vgetc_busy = vgetc_busy;
	    ಪೂರ್ಣಾಙ್ಕ save_did_emsg = did_emsg;
	    ಪೂರ್ಣಾಙ್ಕ prev_uncaught_emsg = uncaught_emsg;
	    ಪೂರ್ಣಾಙ್ಕ save_called_emsg = called_emsg;
	    ಪೂರ್ಣಾಙ್ಕ save_must_redraw = must_redraw;
	    ಪೂರ್ಣಾಙ್ಕ save_ex_pressedreturn = get_pressedreturn();
	    ಪೂರ್ಣಾಙ್ಕ save_may_garbage_collect = may_garbage_collect;
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
	    ಯದಿ (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_ಎಣಿಕೆ;
	    ಅನ್ಯಥಾ
		timer->tr_emsg_ಎಣಿಕೆ = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    ಯದಿ (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    ಯದಿ (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_ಎಣಿಕೆ < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_ಎಡ(&timer->tr_due, &now);
		ಯದಿ (this_due < 1)
		    this_due = 1;
		ಯದಿ (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    ಅನ್ಯಥಾ
	    {
		this_due = -1;
		ಯದಿ (timer->tr_keep)
		    timer->tr_paused = TRUE;
		ಅನ್ಯಥಾ
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
	ಯದಿ (this_due > 0 && (next_due == -1 || next_due > this_due))
	    next_due = this_due;
    }

    ಯದಿ (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    ಯದಿ (bevalexpr_due_set)
    {
	this_due = proftime_time_ಎಡ(&bevalexpr_due, &now);
	ಯದಿ (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    ಯದಿ (balloonEval == ರಿಕ್ತ)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    ಯದಿ (balloonEval != ರಿಕ್ತ)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (next_due == -1 || next_due > this_due)
	    next_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    next_due = term_check_timers(next_due, &now);
#  endif

    ಪ್ರತ್ಯಾಗಮ current_id != last_timer_id ? 1 : next_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    ಸ್ಥಿರ timer_T *
find_timer(long id)
{
    timer_T *timer;

    ಯದಿ (id < 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    FOR_ALL_TIMERS(timer)
	ಯದಿ (timer->tr_id == id)
	    ಪ್ರತ್ಯಾಗಮ timer;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}


/*
 * Stop a timer and delete it.
 */
    ಶೂನ್ಯ
stop_timer(timer_T *timer)
{
    ಯದಿ (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    ಅನ್ಯಥಾ
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
stop_all_timers(ಶೂನ್ಯ)
{
    timer_T *timer;
    timer_T *timer_next;

    ಪ್ರತಿ (timer = first_timer; timer != ರಿಕ್ತ; timer = timer_next)
    {
	timer_next = timer->tr_next;
	stop_timer(timer);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
add_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    long	remaining;
    proftime_T	now;

    ಯದಿ (dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    list_append_dict(list, dict);

    dict_add_ಸಂಖ್ಯೆ(dict, "id", timer->tr_id);
    dict_add_ಸಂಖ್ಯೆ(dict, "time", (long)timer->tr_interval);

    profile_start(&now);
    remaining = proftime_time_ಎಡ(&timer->tr_due, &now);
    dict_add_ಸಂಖ್ಯೆ(dict, "remaining", (long)remaining);

    dict_add_ಸಂಖ್ಯೆ(dict, "repeat",
	    (long)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_add_ಸಂಖ್ಯೆ(dict, "paused", (long)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    ಯದಿ (di != ರಿಕ್ತ)
    {
	ಯದಿ (dict_add(dict, di) == FAIL)
	    vim_free(di);
	ಅನ್ಯಥಾ
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	ಯದಿ (timer->tr_id != -1)
	    add_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_timer(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    ಪ್ರತಿ (timer = first_timer; !abort && timer != ರಿಕ್ತ; timer = timer->tr_next)
    {
	ಯದಿ (timer->tr_callback.cb_partial != ರಿಕ್ತ)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	ಅನ್ಯಥಾ
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || set_ref_in_item(&tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    }
    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    ಪೂರ್ಣಾಙ್ಕ
timer_valid(timer_T *timer)
{
    ಯದಿ (timer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	ಯದಿ (t == timer)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#  ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
timer_free_all(ಶೂನ್ಯ)
{
    ಯಾವತ್ (first_timer != ರಿಕ್ತ)
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
    ಶೂನ್ಯ
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = ರಿಕ್ತ;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
	ಯದಿ (timer != ರಿಕ್ತ)
	    add_timer_info(rettv, timer);
    }
    ಅನ್ಯಥಾ
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    ಶೂನ್ಯ
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	emsg(_(e_ಸಂಖ್ಯೆ_expected));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪೂರ್ಣಾಙ್ಕ	paused = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool(&argvars[1]);

    timer = find_timer((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
    ಯದಿ (timer != ರಿಕ್ತ)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    ಶೂನ್ಯ
f_timer_start(typval_T *argvars, typval_T *rettv)
{
    long	msec;
    timer_T	*timer;
    ಪೂರ್ಣಾಙ್ಕ		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    msec = (long)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಯದಿ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	dict = argvars[2].vval.v_dict;
	ಯದಿ (dict_has_ಕೀಲಿ(dict, "repeat"))
	    repeat = dict_get_ಸಂಖ್ಯೆ(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    ಯದಿ (callback.cb_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	ಪ್ರತ್ಯಾಗಮ;
    }

    timer = create_timer(msec, repeat);
    ಯದಿ (timer == ರಿಕ್ತ)
    {
	free_callback(&callback);
	ಪ್ರತ್ಯಾಗಮ;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    ಶೂನ್ಯ
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    ಯದಿ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    timer = find_timer((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
    ಯದಿ (timer != ರಿಕ್ತ)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    ಶೂನ್ಯ
f_timer_stopall(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# ಯದಿ defined(STARTUPTIME)
ಸ್ಥಿರ ಸಂರಚನಾ timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gettimeofday(ಸಂರಚನಾ timeval *tv, ವರ್ಣ *dummy UNUSED)
{
    long t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    ಪ್ರತ್ಯಾಗಮ 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    ಶೂನ್ಯ
time_ತಳ್ಳು(ಶೂನ್ಯ *tv_rel, ಶೂನ್ಯ *tv_start)
{
    *((ಸಂರಚನಾ timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, ರಿಕ್ತ);
    ((ಸಂರಚನಾ timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((ಸಂರಚನಾ timeval *)tv_rel)->tv_usec;
    ((ಸಂರಚನಾ timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((ಸಂರಚನಾ timeval *)tv_rel)->tv_sec;
    ಯದಿ (((ಸಂರಚನಾ timeval *)tv_rel)->tv_usec < 0)
    {
	((ಸಂರಚನಾ timeval *)tv_rel)->tv_usec += 1000000;
	--((ಸಂರಚನಾ timeval *)tv_rel)->tv_sec;
    }
    *(ಸಂರಚನಾ timeval *)tv_start = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    ಶೂನ್ಯ
time_ತೆಗೆ(
    ಶೂನ್ಯ	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((ಸಂರಚನಾ timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((ಸಂರಚನಾ timeval *)tp)->tv_sec;
    ಯದಿ (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
time_diff(ಸಂರಚನಾ timeval *then, ಸಂರಚನಾ timeval *now)
{
    long	usec;
    long	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(time_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    ಶೂನ್ಯ
time_ಸಂದೇಶ(
    ವರ್ಣ	*mesg,
    ಶೂನ್ಯ	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    ಸ್ಥಿರ ಸಂರಚನಾ timeval	start;
    ಸಂರಚನಾ timeval		now;

    ಯದಿ (time_fd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (strstr(mesg, "STARTING") != ರಿಕ್ತ)
    {
	gettimeofday(&start, ರಿಕ್ತ);
	prev_timeval = start;
	fprintf(time_fd, "\n\ntimes in msec\n");
	fprintf(time_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, ರಿಕ್ತ);
    time_diff(&start, &now);
    ಯದಿ (((ಸಂರಚನಾ timeval *)tv_start) != ರಿಕ್ತ)
    {
	fprintf(time_fd, "  ");
	time_diff(((ಸಂರಚನಾ timeval *)tv_start), &now);
    }
    fprintf(time_fd, "  ");
    time_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(time_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#endif // FEAT_EVAL

#ಯದಿ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    time_T
get8ctime(FILE *fd)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    time_T	n = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	ಯದಿ (c == EOF) ಪ್ರತ್ಯಾಗಮ -1;
	n = (n << 8) + c;
    }
    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    ಪೂರ್ಣಾಙ್ಕ
put_time(FILE *fd, time_T the_time)
{
    char_u	buf[8];

    time_to_bytes(the_time, buf);
    ಪ್ರತ್ಯಾಗಮ fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    ಶೂನ್ಯ
time_to_bytes(time_T the_time, char_u *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		bi = 0;
    time_T	wtime = the_time;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    ಪ್ರತಿ (i = 7; i >= 0; --i)
    {
	ಯದಿ (i + 1 > (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	ಅನ್ಯಥಾ
	{
# ಯದಿ defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (ಪೂರ್ಣಾಙ್ಕ)(wtime >> (i * 8));
# ಅನ್ಯಥಾ
	    c = (ಪೂರ್ಣಾಙ್ಕ)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#endif

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    ಶೂನ್ಯ
add_time(char_u *buf, size_t buflen, time_t tt)
{
#ifdef HAVE_STRFTIME
    ಸಂರಚನಾ tm	tmval;
    ಸಂರಚನಾ tm	*curtime;
    size_t	n;

    ಯದಿ (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	ಯದಿ (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((ವರ್ಣ *)buf, buflen, "%H:%M:%S", curtime);
	ಅನ್ಯಥಾ
	    // longer ago
	    n = strftime((ವರ್ಣ *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	ಯದಿ (n == 0)
	    buf[0] = NUL;
    }
    ಅನ್ಯಥಾ
#endif
    {
	long seconds = (long)(vim_time() - tt);

	vim_snprintf((ವರ್ಣ *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
