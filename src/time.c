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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#ਜੇ defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
ਸਥਿਰ ਅੱਖਰ	tz_cache[64];
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_TIMERS(t) \
    ਲਈ ((t) = ਪਹਿਲਾ_timer; (t) != ਨਿਰਾ; (t) = (t)->tr_ਅਗਲਾ)

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
    ਸਥਿਰ ਬਣਤਰ tm *
vim_localtime(
    ਅਚਲ time_t	*timep,		// timestamp for local representation
    ਬਣਤਰ tm		*ਨਤੀਜਾ ਅਣਵਰਤਿਆ)	// pointer to caller return buffer
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    ਅੱਖਰ		*tz;		// pointer for TZ environment var

    tz = (ਅੱਖਰ *)mch_getenv((char_u *)"TZ");
    ਜੇ (tz == ਨਿਰਾ)
	tz = "";
    ਜੇ (STRNCMP(tz_cache, tz, ਆਕਾਰ(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, ਆਕਾਰ(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    ਵਾਪਸ localtime_r(timep, ਨਤੀਜਾ);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ localtime(timep);
#ਜੇ_ਅੰਤ	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    ਸਮਾਂ_T
vim_ਸਮਾਂ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਵਾਪਸ ਸਮਾਂ_for_testing == 0 ? ਸਮਾਂ(ਨਿਰਾ) : ਸਮਾਂ_for_testing;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ ਸਮਾਂ(ਨਿਰਾ);
#ਜੇ_ਅੰਤ
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    ਅੱਖਰ *
get_ctime(time_t thetime, ਪੂਰਨ ਜੋੜੋ_newline)
{
    ਸਥਿਰ ਅੱਖਰ buf[100];  // hopefully enough for every language
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRFTIME
    ਬਣਤਰ tm	tmval;
    ਬਣਤਰ tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ਜੇ (curtime == ਨਿਰਾ)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), ਆਕਾਰ(buf) - 2);
    ਨਹੀਂਤਾਂ
    {
	// xgettext:no-c-format
	ਜੇ (strftime(buf, ਆਕਾਰ(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      ਆਕਾਰ(buf) - 2);
	}
# ifdef MSWIN
	ਜੇ (enc_codepage >= 0 && (ਪੂਰਨ)GetACP() != enc_codepage)
	{
	    char_u	*ਮੰਜ਼ਿਲ_free = ਨਿਰਾ;
	    ਪੂਰਨ		len;

	    acp_ਮੰਜ਼ਿਲ_enc((char_u *)buf, (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(buf), &ਮੰਜ਼ਿਲ_free, &len);
	    ਜੇ (ਮੰਜ਼ਿਲ_free != ਨਿਰਾ)
	    {
		STRNCPY(buf, ਮੰਜ਼ਿਲ_free, ਆਕਾਰ(buf) - 2);
		vim_free(ਮੰਜ਼ਿਲ_free);
	    }
	}
# endif
    }
#ਨਹੀਂਤਾਂ
    STRCPY(buf, "(unknown)");
#ਜੇ_ਅੰਤ
    ਜੇ (ਜੋੜੋ_newline)
	STRCAT(buf, "\n");
    ਵਾਪਸ buf;
}

#ਜੇ defined(FEAT_EVAL)

# ਜੇ defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    ਖਾਲੀ
f_localtime(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)ਸਮਾਂ(ਨਿਰਾ);
}

# ਜੇ defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    ਸਥਿਰ ਪੂਰਨ
list2proftime(typval_T *arg, proftime_T *tm)
{
    ਲੰਮਾ	n1, n2;
    ਪੂਰਨ	error = FALSE;

    ਜੇ (arg->v_type != VAR_LIST || arg->vval.v_list == ਨਿਰਾ
					     || arg->vval.v_list->lv_len != 2)
	ਵਾਪਸ FAIL;
    n1 = list_ਲੱਭੋ_nr(arg->vval.v_list, 0L, &error);
    n2 = list_ਲੱਭੋ_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->ਉੱਚਾ_Part = n1;
    tm->ਨੀਵਾਂ_Part = n2;
#  ਨਹੀਂਤਾਂ
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    ਵਾਪਸ error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    ਖਾਲੀ
f_reltime(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	ਸ਼ੁਰੂ;
    ਲੰਮਾ	n1, n2;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_ਸ਼ੁਰੂ(&res);
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ਜੇ (list2proftime(&argvars[0], &res) == FAIL)
	{
	    ਜੇ (ਵਿੱਚ_vim9script())
		emsg(_(e_invalid_ਦਲੀਲ));
	    ਵਾਪਸ;
	}
	profile_ਸਿਰਾ(&res);
    }
    ਨਹੀਂਤਾਂ
    {
	// Two arguments: compute the difference.
	ਜੇ (list2proftime(&argvars[0], &ਸ਼ੁਰੂ) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    ਜੇ (ਵਿੱਚ_vim9script())
		emsg(_(e_invalid_ਦਲੀਲ));
	    ਵਾਪਸ;
	}
	profile_sub(&res, &ਸ਼ੁਰੂ);
    }

#  ifdef MSWIN
    n1 = res.ਉੱਚਾ_Part;
    n2 = res.ਨੀਵਾਂ_Part;
#  ਨਹੀਂਤਾਂ
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)n1);
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    ਖਾਲੀ
f_reltimefloat(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_vim9script())
	emsg(_(e_invalid_ਦਲੀਲ));
# endif
}

/*
 * "reltimestr()" function
 */
    ਖਾਲੀ
f_reltimestr(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;
# ifdef FEAT_RELTIME
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    proftime_T	tm;
    ਜੇ (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_ਸੁਨੇਹਾ(&tm));
#  ਨਹੀਂਤਾਂ
	ਸਥਿਰ ਅੱਖਰ buf[50];
	ਲੰਮਾ usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, ਆਕਾਰ(buf), "%3ld.%06ld", (ਲੰਮਾ)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_vim9script())
	emsg(_(e_invalid_ਦਲੀਲ));
# endif
}

# ਜੇ defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    ਖਾਲੀ
f_strftime(typval_T *argvars, typval_T *rettv)
{
    ਬਣਤਰ tm	tmval;
    ਬਣਤਰ tm	*curtime;
    time_t	seconds;
    char_u	*p;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
	seconds = ਸਮਾਂ(ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	seconds = (time_t)tv_get_ਸੰਖਿਆ(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ਜੇ (curtime == ਨਿਰਾ)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	ਵਾਪਸ;
    }

#  ifdef MSWIN
    WCHAR	    ਨਤੀਜਾ_buf[256];
    WCHAR	    *wp;

    wp = enc_ਮੰਜ਼ਿਲ_utf16(p, ਨਿਰਾ);
    ਜੇ (wp != ਨਿਰਾ)
	(ਖਾਲੀ)wcsftime(ਨਤੀਜਾ_buf, ARRAY_ਲੰਮਾਈ(ਨਤੀਜਾ_buf), wp, curtime);
    ਨਹੀਂਤਾਂ
	ਨਤੀਜਾ_buf[0] = NUL;
    rettv->vval.v_string = utf16_ਮੰਜ਼ਿਲ_enc(ਨਤੀਜਾ_buf, ਨਿਰਾ);
    vim_free(wp);
#  ਨਹੀਂਤਾਂ
    char_u	    ਨਤੀਜਾ_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_ਤਿਆਰੀ(&conv, p_enc, enc);
    ਜੇ (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ || strftime((ਅੱਖਰ *)ਨਤੀਜਾ_buf, ਆਕਾਰ(ਨਤੀਜਾ_buf),
		(ਅੱਖਰ *)p, curtime) == 0)
	ਨਤੀਜਾ_buf[0] = NUL;

    ਜੇ (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_ਤਿਆਰੀ(&conv, enc, p_enc);
    ਜੇ (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, ਨਤੀਜਾ_buf, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = vim_strsave(ਨਤੀਜਾ_buf);

    // Release conversion descriptors
    convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
    vim_free(enc);
#  endif
}
# endif

# ਜੇ defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    ਖਾਲੀ
f_strptime(typval_T *argvars, typval_T *rettv)
{
    ਬਣਤਰ tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    CLEAR_ਖੇਤਰ(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_ਤਿਆਰੀ(&conv, p_enc, enc);
    ਜੇ (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, ਨਿਰਾ);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    ਜੇ (fmt == ਨਿਰਾ
	    || strptime((ਅੱਖਰ *)str, (ਅੱਖਰ *)fmt, &tmval) == ਨਿਰਾ
	    || (rettv->vval.v_ਸੰਖਿਆ = mktime(&tmval)) == -1)
	rettv->vval.v_ਸੰਖਿਆ = 0;

    ਜੇ (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
    vim_free(enc);
}
# endif

# ਜੇ defined(FEAT_TIMERS)
ਸਥਿਰ timer_T	*ਪਹਿਲਾ_timer = ਨਿਰਾ;
ਸਥਿਰ ਲੰਮਾ	ਆਖਰੀ_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    ਲੰਮਾ
proftime_ਸਮਾਂ_ਖੱਬਾ(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    ਜੇ (now->QuadPart > due->QuadPart)
	ਵਾਪਸ 0;
    QueryPerformanceFrequency(&fr);
    ਵਾਪਸ (ਲੰਮਾ)(((ਦੁੱਗਣਾ)(due->QuadPart - now->QuadPart)
		   / (ਦੁੱਗਣਾ)fr.QuadPart) * 1000);
#  ਨਹੀਂਤਾਂ
    ਜੇ (now->tv_sec > due->tv_sec)
	ਵਾਪਸ 0;
    ਵਾਪਸ (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਾਓ_timer(timer_T *timer)
{
    timer->tr_ਅਗਲਾ = ਪਹਿਲਾ_timer;
    timer->tr_prev = ਨਿਰਾ;
    ਜੇ (ਪਹਿਲਾ_timer != ਨਿਰਾ)
	ਪਹਿਲਾ_timer->tr_prev = timer;
    ਪਹਿਲਾ_timer = timer;
    did_ਜੋੜੋ_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    ਸਥਿਰ ਖਾਲੀ
remove_timer(timer_T *timer)
{
    ਜੇ (timer->tr_prev == ਨਿਰਾ)
	ਪਹਿਲਾ_timer = timer->tr_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	timer->tr_prev->tr_ਅਗਲਾ = timer->tr_ਅਗਲਾ;
    ਜੇ (timer->tr_ਅਗਲਾ != ਨਿਰਾ)
	timer->tr_ਅਗਲਾ->tr_prev = timer->tr_prev;
}

    ਸਥਿਰ ਖਾਲੀ
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
create_timer(ਲੰਮਾ msec, ਪੂਰਨ repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    ਲੰਮਾ	prev_id = ਆਖਰੀ_timer_id;

    ਜੇ (timer == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (++ਆਖਰੀ_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	ਆਖਰੀ_timer_id = 0;
    timer->tr_id = ਆਖਰੀ_timer_id;
    ਪਾਓ_timer(timer);
    ਜੇ (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_ਸ਼ੁਰੂ(timer);
    ਵਾਪਸ timer;
}

/*
 * (Re)start a timer.
 */
    ਖਾਲੀ
timer_ਸ਼ੁਰੂ(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    ਸਥਿਰ ਖਾਲੀ
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    ਜੇ (ch_log_ਸਰਗਰਮ())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(ਨਿਰਾ, "invoking timer callback %s",
	       cb->cb_partial != ਨਿਰਾ ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_ਸੰਖਿਆ;
    argv[0].vval.v_ਸੰਖਿਆ = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(ਨਿਰਾ, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    ਲੰਮਾ
check_due_timer(ਖਾਲੀ)
{
    timer_T	*timer;
    timer_T	*timer_ਅਗਲਾ;
    ਲੰਮਾ	this_due;
    ਲੰਮਾ	ਅਗਲਾ_due = -1;
    proftime_T	now;
    ਪੂਰਨ		did_one = FALSE;
    ਪੂਰਨ		need_update_screen = FALSE;
    ਲੰਮਾ	current_id = ਆਖਰੀ_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    ਜੇ (exiting || aborting() || debug_ਬਹੁਲਕ)
	ਵਾਪਸ ਅਗਲਾ_due;

    profile_ਸ਼ੁਰੂ(&now);
    ਲਈ (timer = ਪਹਿਲਾ_timer; timer != ਨਿਰਾ && !got_int; timer = timer_ਅਗਲਾ)
    {
	timer_ਅਗਲਾ = timer->tr_ਅਗਲਾ;

	ਜੇ (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    ਜਾਰੀ;
	this_due = proftime_ਸਮਾਂ_ਖੱਬਾ(&timer->tr_due, &now);
	ਜੇ (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    ਪੂਰਨ save_timer_busy = timer_busy;
	    ਪੂਰਨ save_vgetc_busy = vgetc_busy;
	    ਪੂਰਨ save_did_emsg = did_emsg;
	    ਪੂਰਨ prev_uncaught_emsg = uncaught_emsg;
	    ਪੂਰਨ save_called_emsg = called_emsg;
	    ਪੂਰਨ save_must_redraw = must_redraw;
	    ਪੂਰਨ save_ex_pressedreturn = get_pressedreturn();
	    ਪੂਰਨ save_may_garbage_collect = may_garbage_collect;
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
	    timer_ਅਗਲਾ = timer->tr_ਅਗਲਾ;
	    did_one = TRUE;
	    timer_busy = save_timer_busy;
	    vgetc_busy = save_vgetc_busy;
	    ਜੇ (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_ਗਿਣਤੀ;
	    ਨਹੀਂਤਾਂ
		timer->tr_emsg_ਗਿਣਤੀ = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    ਜੇ (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    ਸੈੱਟ_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    ਜੇ (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_ਗਿਣਤੀ < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_ਸਮਾਂ_ਖੱਬਾ(&timer->tr_due, &now);
		ਜੇ (this_due < 1)
		    this_due = 1;
		ਜੇ (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		this_due = -1;
		ਜੇ (timer->tr_keep)
		    timer->tr_paused = TRUE;
		ਨਹੀਂਤਾਂ
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
	ਜੇ (this_due > 0 && (ਅਗਲਾ_due == -1 || ਅਗਲਾ_due > this_due))
	    ਅਗਲਾ_due = this_due;
    }

    ਜੇ (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_ਪਦ
    ਜੇ (bevalexpr_due_ਸੈੱਟ)
    {
	this_due = proftime_ਸਮਾਂ_ਖੱਬਾ(&bevalexpr_due, &now);
	ਜੇ (this_due <= 1)
	{
	    bevalexpr_due_ਸੈੱਟ = FALSE;
	    ਜੇ (balloonEval == ਨਿਰਾ)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloon_Eval_For_ਪਦ = TRUE;
	    }
	    ਜੇ (balloonEval != ਨਿਰਾ)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		ਬਾਹਰਲਾ_flush();
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਅਗਲਾ_due == -1 || ਅਗਲਾ_due > this_due)
	    ਅਗਲਾ_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    ਅਗਲਾ_due = ਪਦ_check_timers(ਅਗਲਾ_due, &now);
#  endif

    ਵਾਪਸ current_id != ਆਖਰੀ_timer_id ? 1 : ਅਗਲਾ_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    ਸਥਿਰ timer_T *
ਲੱਭੋ_timer(ਲੰਮਾ id)
{
    timer_T *timer;

    ਜੇ (id < 0)
	ਵਾਪਸ ਨਿਰਾ;

    FOR_ALL_TIMERS(timer)
	ਜੇ (timer->tr_id == id)
	    ਵਾਪਸ timer;
    ਵਾਪਸ ਨਿਰਾ;
}


/*
 * Stop a timer and delete it.
 */
    ਖਾਲੀ
stop_timer(timer_T *timer)
{
    ਜੇ (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    ਨਹੀਂਤਾਂ
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    ਸਥਿਰ ਖਾਲੀ
stop_all_timers(ਖਾਲੀ)
{
    timer_T *timer;
    timer_T *timer_ਅਗਲਾ;

    ਲਈ (timer = ਪਹਿਲਾ_timer; timer != ਨਿਰਾ; timer = timer_ਅਗਲਾ)
    {
	timer_ਅਗਲਾ = timer->tr_ਅਗਲਾ;
	stop_timer(timer);
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    ਲੰਮਾ	ਬਚਿਆ;
    proftime_T	now;

    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ;
    list_append_dict(list, dict);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "id", timer->tr_id);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "time", (ਲੰਮਾ)timer->tr_interval);

    profile_ਸ਼ੁਰੂ(&now);
    ਬਚਿਆ = proftime_ਸਮਾਂ_ਖੱਬਾ(&timer->tr_due, &now);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "remaining", (ਲੰਮਾ)ਬਚਿਆ);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "repeat",
	    (ਲੰਮਾ)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "paused", (ਲੰਮਾ)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    ਜੇ (di != ਨਿਰਾ)
    {
	ਜੇ (dict_ਜੋੜੋ(dict, di) == FAIL)
	    vim_free(di);
	ਨਹੀਂਤਾਂ
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	ਜੇ (timer->tr_id != -1)
	    ਜੋੜੋ_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_timer(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    ਲਈ (timer = ਪਹਿਲਾ_timer; !abort && timer != ਨਿਰਾ; timer = timer->tr_ਅਗਲਾ)
    {
	ਜੇ (timer->tr_callback.cb_partial != ਨਿਰਾ)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	ਨਹੀਂਤਾਂ
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    }
    ਵਾਪਸ abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    ਪੂਰਨ
timer_valid(timer_T *timer)
{
    ਜੇ (timer == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	ਜੇ (t == timer)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

#  ਜੇ defined(EXITFREE)
    ਖਾਲੀ
timer_free_all(ਖਾਲੀ)
{
    ਜਦੋਂ (ਪਹਿਲਾ_timer != ਨਿਰਾ)
    {
	timer_T *timer = ਪਹਿਲਾ_timer;
	remove_timer(timer);
	free_timer(timer);
    }
}
#  endif

/*
 * "timer_info([timer])" function
 */
    ਖਾਲੀ
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = ਨਿਰਾ;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = ਲੱਭੋ_timer((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]));
	ਜੇ (timer != ਨਿਰਾ)
	    ਜੋੜੋ_timer_info(rettv, timer);
    }
    ਨਹੀਂਤਾਂ
	ਜੋੜੋ_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    ਖਾਲੀ
f_timer_pause(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    timer_T	*timer = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_ਸੰਖਿਆ)
    {
	emsg(_(e_ਸੰਖਿਆ_expected));
	ਵਾਪਸ;
    }

    ਪੂਰਨ	paused = (ਪੂਰਨ)tv_get_bool(&argvars[1]);

    timer = ਲੱਭੋ_timer((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]));
    ਜੇ (timer != ਨਿਰਾ)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    ਖਾਲੀ
f_timer_ਸ਼ੁਰੂ(typval_T *argvars, typval_T *rettv)
{
    ਲੰਮਾ	msec;
    timer_T	*timer;
    ਪੂਰਨ		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    msec = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ(&argvars[0]);
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ਜੇ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ਵਾਪਸ;

	dict = argvars[2].vval.v_dict;
	ਜੇ (dict_has_ਕੁੰਜੀ(dict, "repeat"))
	    repeat = dict_get_ਸੰਖਿਆ(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    ਜੇ (callback.cb_name == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (ਵਿੱਚ_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_ਦਲੀਲ));
	free_callback(&callback);
	ਵਾਪਸ;
    }

    timer = create_timer(msec, repeat);
    ਜੇ (timer == ਨਿਰਾ)
    {
	free_callback(&callback);
	ਵਾਪਸ;
    }
    ਸੈੱਟ_callback(&timer->tr_callback, &callback);
    rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    ਖਾਲੀ
f_timer_stop(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    timer_T *timer;

    ਜੇ (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    timer = ਲੱਭੋ_timer((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]));
    ਜੇ (timer != ਨਿਰਾ)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    ਖਾਲੀ
f_timer_stopall(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# ਜੇ defined(STARTUPTIME)
ਸਥਿਰ ਬਣਤਰ timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    ਸਥਿਰ ਪੂਰਨ
gettimeofday(ਬਣਤਰ timeval *tv, ਅੱਖਰ *dummy ਅਣਵਰਤਿਆ)
{
    ਲੰਮਾ t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    ਵਾਪਸ 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    ਖਾਲੀ
ਸਮਾਂ_ਧੱਕੋ(ਖਾਲੀ *tv_rel, ਖਾਲੀ *tv_ਸ਼ੁਰੂ)
{
    *((ਬਣਤਰ timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, ਨਿਰਾ);
    ((ਬਣਤਰ timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((ਬਣਤਰ timeval *)tv_rel)->tv_usec;
    ((ਬਣਤਰ timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((ਬਣਤਰ timeval *)tv_rel)->tv_sec;
    ਜੇ (((ਬਣਤਰ timeval *)tv_rel)->tv_usec < 0)
    {
	((ਬਣਤਰ timeval *)tv_rel)->tv_usec += 1000000;
	--((ਬਣਤਰ timeval *)tv_rel)->tv_sec;
    }
    *(ਬਣਤਰ timeval *)tv_ਸ਼ੁਰੂ = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    ਖਾਲੀ
ਸਮਾਂ_ਕੱਢੋ(
    ਖਾਲੀ	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((ਬਣਤਰ timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((ਬਣਤਰ timeval *)tp)->tv_sec;
    ਜੇ (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਸਮਾਂ_diff(ਬਣਤਰ timeval *then, ਬਣਤਰ timeval *now)
{
    ਲੰਮਾ	usec;
    ਲੰਮਾ	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(ਸਮਾਂ_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    ਖਾਲੀ
ਸਮਾਂ_ਸੁਨੇਹਾ_2(
    ਅੱਖਰ	*mesg,
    ਖਾਲੀ	*tv_ਸ਼ੁਰੂ)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    ਸਥਿਰ ਬਣਤਰ timeval	ਸ਼ੁਰੂ;
    ਬਣਤਰ timeval		now;

    ਜੇ (ਸਮਾਂ_fd == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ਲੜੀ_ਖੋਜ(mesg, "STARTING") != ਨਿਰਾ)
    {
	gettimeofday(&ਸ਼ੁਰੂ, ਨਿਰਾ);
	prev_timeval = ਸ਼ੁਰੂ;
	fprintf(ਸਮਾਂ_fd, "\n\ntimes in msec\n");
	fprintf(ਸਮਾਂ_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(ਸਮਾਂ_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, ਨਿਰਾ);
    ਸਮਾਂ_diff(&ਸ਼ੁਰੂ, &now);
    ਜੇ (((ਬਣਤਰ timeval *)tv_ਸ਼ੁਰੂ) != ਨਿਰਾ)
    {
	fprintf(ਸਮਾਂ_fd, "  ");
	ਸਮਾਂ_diff(((ਬਣਤਰ timeval *)tv_ਸ਼ੁਰੂ), &now);
    }
    fprintf(ਸਮਾਂ_fd, "  ");
    ਸਮਾਂ_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(ਸਮਾਂ_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#ਜੇ_ਅੰਤ // FEAT_EVAL

#ਜੇ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    ਸਮਾਂ_T
get8ctime(FILE *fd)
{
    ਪੂਰਨ		c;
    ਸਮਾਂ_T	n = 0;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	ਜੇ (c == EOF) ਵਾਪਸ -1;
	n = (n << 8) + c;
    }
    ਵਾਪਸ n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    ਪੂਰਨ
put_ਸਮਾਂ(FILE *fd, ਸਮਾਂ_T the_ਸਮਾਂ)
{
    char_u	buf[8];

    ਸਮਾਂ_ਮੰਜ਼ਿਲ_ਬਾਈਟਾਂ(the_ਸਮਾਂ, buf);
    ਵਾਪਸ fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    ਖਾਲੀ
ਸਮਾਂ_ਮੰਜ਼ਿਲ_ਬਾਈਟਾਂ(ਸਮਾਂ_T the_ਸਮਾਂ, char_u *buf)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		i;
    ਪੂਰਨ		bi = 0;
    ਸਮਾਂ_T	wtime = the_ਸਮਾਂ;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    ਲਈ (i = 7; i >= 0; --i)
    {
	ਜੇ (i + 1 > (ਪੂਰਨ)ਆਕਾਰ(ਸਮਾਂ_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	ਨਹੀਂਤਾਂ
	{
# ਜੇ defined(SIZEOF_ਸਮਾਂ_T) && SIZEOF_ਸਮਾਂ_T > 4
	    c = (ਪੂਰਨ)(wtime >> (i * 8));
# ਨਹੀਂਤਾਂ
	    c = (ਪੂਰਨ)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#ਜੇ_ਅੰਤ

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    ਖਾਲੀ
ਜੋੜੋ_ਸਮਾਂ(char_u *buf, size_t buflen, time_t tt)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRFTIME
    ਬਣਤਰ tm	tmval;
    ਬਣਤਰ tm	*curtime;
    size_t	n;

    ਜੇ (vim_ਸਮਾਂ() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	ਜੇ (vim_ਸਮਾਂ() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((ਅੱਖਰ *)buf, buflen, "%H:%M:%S", curtime);
	ਨਹੀਂਤਾਂ
	    // longer ago
	    n = strftime((ਅੱਖਰ *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	ਜੇ (n == 0)
	    buf[0] = NUL;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਲੰਮਾ seconds = (ਲੰਮਾ)(vim_ਸਮਾਂ() - tt);

	vim_snprintf((ਅੱਖਰ *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
