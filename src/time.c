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
#ಆದರೆ defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
ಸ್ಥಿರ ಅಕ್ಷರ	tz_cache[64];
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_TIMERS(t) \
    ಗಾಗಿ ((t) = ಮೊದಲ_timer; (t) != ಖಾಲಿ; (t) = (t)->tr_ಮುಂದಿನ)

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
    ಸ್ಥಿರ ರಚನೆ tm *
vim_localtime(
    ಸ್ಥಿರಾಂಕ time_t	*timep,		// timestamp for local representation
    ರಚನೆ tm		*ಫಲಿತಾಂಶ ಬಳಸದ)	// pointer to caller return buffer
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    ಅಕ್ಷರ		*tz;		// pointer for TZ environment var

    tz = (ಅಕ್ಷರ *)mch_getenv((char_u *)"TZ");
    ಆದರೆ (tz == ಖಾಲಿ)
	tz = "";
    ಆದರೆ (STRNCMP(tz_cache, tz, ಗಾತ್ರ(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, ಗಾತ್ರ(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    ಹಿಂತಿರುಗಿ localtime_r(timep, ಫಲಿತಾಂಶ);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ localtime(timep);
#ಆದರೆ_ಅಂತ್ಯ	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    ಸಮಯ_T
vim_ಸಮಯ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಹಿಂತಿರುಗಿ ಸಮಯ_for_testing == 0 ? ಸಮಯ(ಖಾಲಿ) : ಸಮಯ_for_testing;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ ಸಮಯ(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    ಅಕ್ಷರ *
get_ctime(time_t thetime, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_newline)
{
    ಸ್ಥಿರ ಅಕ್ಷರ buf[100];  // hopefully enough for every language
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRFTIME
    ರಚನೆ tm	tmval;
    ರಚನೆ tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ಆದರೆ (curtime == ಖಾಲಿ)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), ಗಾತ್ರ(buf) - 2);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// xgettext:no-c-format
	ಆದರೆ (strftime(buf, ಗಾತ್ರ(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      ಗಾತ್ರ(buf) - 2);
	}
# ifdef MSWIN
	ಆದರೆ (enc_codepage >= 0 && (ಪೂರ್ಣಾಂಕ)GetACP() != enc_codepage)
	{
	    char_u	*ಗಮ್ಯ_free = ಖಾಲಿ;
	    ಪೂರ್ಣಾಂಕ		len;

	    acp_ಗಮ್ಯ_enc((char_u *)buf, (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf), &ಗಮ್ಯ_free, &len);
	    ಆದರೆ (ಗಮ್ಯ_free != ಖಾಲಿ)
	    {
		STRNCPY(buf, ಗಮ್ಯ_free, ಗಾತ್ರ(buf) - 2);
		vim_free(ಗಮ್ಯ_free);
	    }
	}
# endif
    }
#ಇಲ್ಲದಿದ್ದರೆ
    STRCPY(buf, "(unknown)");
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಕೂಡಿಸು_newline)
	STRCAT(buf, "\n");
    ಹಿಂತಿರುಗಿ buf;
}

#ಆದರೆ defined(FEAT_EVAL)

# ಆದರೆ defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    ಶೂನ್ಯ
f_localtime(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)ಸಮಯ(ಖಾಲಿ);
}

# ಆದರೆ defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
list2proftime(typval_T *arg, proftime_T *tm)
{
    ದೀರ್ಘ	n1, n2;
    ಪೂರ್ಣಾಂಕ	error = FALSE;

    ಆದರೆ (arg->v_type != VAR_LIST || arg->vval.v_list == ಖಾಲಿ
					     || arg->vval.v_list->lv_len != 2)
	ಹಿಂತಿರುಗಿ FAIL;
    n1 = list_ಕಂಡುಹಿಡಿ_nr(arg->vval.v_list, 0L, &error);
    n2 = list_ಕಂಡುಹಿಡಿ_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->ಮೇಲಿನ_Part = n1;
    tm->ಕೆಳಗಿನ_Part = n2;
#  ಇಲ್ಲದಿದ್ದರೆ
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    ಹಿಂತಿರುಗಿ error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    ಶೂನ್ಯ
f_reltime(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	ಆರಂಭ;
    ದೀರ್ಘ	n1, n2;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_ಆರಂಭ(&res);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ಆದರೆ (list2proftime(&argvars[0], &res) == FAIL)
	{
	    ಆದರೆ (ಒಳ_vim9script())
		emsg(_(e_invalid_ವಾದ));
	    ಹಿಂತಿರುಗಿ;
	}
	profile_ಕೊನೆ(&res);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Two arguments: compute the difference.
	ಆದರೆ (list2proftime(&argvars[0], &ಆರಂಭ) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    ಆದರೆ (ಒಳ_vim9script())
		emsg(_(e_invalid_ವಾದ));
	    ಹಿಂತಿರುಗಿ;
	}
	profile_sub(&res, &ಆರಂಭ);
    }

#  ifdef MSWIN
    n1 = res.ಮೇಲಿನ_Part;
    n2 = res.ಕೆಳಗಿನ_Part;
#  ಇಲ್ಲದಿದ್ದರೆ
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
f_reltimefloat(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    ಆದರೆ (ಒಳ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_vim9script())
	emsg(_(e_invalid_ವಾದ));
# endif
}

/*
 * "reltimestr()" function
 */
    ಶೂನ್ಯ
f_reltimestr(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
# ifdef FEAT_RELTIME
    ಆದರೆ (ಒಳ_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    proftime_T	tm;
    ಆದರೆ (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_ಸಂದೇಶ(&tm));
#  ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿರ ಅಕ್ಷರ buf[50];
	ದೀರ್ಘ usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, ಗಾತ್ರ(buf), "%3ld.%06ld", (ದೀರ್ಘ)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_vim9script())
	emsg(_(e_invalid_ವಾದ));
# endif
}

# ಆದರೆ defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    ಶೂನ್ಯ
f_strftime(typval_T *argvars, typval_T *rettv)
{
    ರಚನೆ tm	tmval;
    ರಚನೆ tm	*curtime;
    time_t	seconds;
    char_u	*p;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
	seconds = ಸಮಯ(ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	seconds = (time_t)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ಆದರೆ (curtime == ಖಾಲಿ)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	ಹಿಂತಿರುಗಿ;
    }

#  ifdef MSWIN
    WCHAR	    ಫಲಿತಾಂಶ_buf[256];
    WCHAR	    *wp;

    wp = enc_ಗಮ್ಯ_utf16(p, ಖಾಲಿ);
    ಆದರೆ (wp != ಖಾಲಿ)
	(ಶೂನ್ಯ)wcsftime(ಫಲಿತಾಂಶ_buf, ARRAY_ಉದ್ದಳತೆ(ಫಲಿತಾಂಶ_buf), wp, curtime);
    ಇಲ್ಲದಿದ್ದರೆ
	ಫಲಿತಾಂಶ_buf[0] = NUL;
    rettv->vval.v_string = utf16_ಗಮ್ಯ_enc(ಫಲಿತಾಂಶ_buf, ಖಾಲಿ);
    vim_free(wp);
#  ಇಲ್ಲದಿದ್ದರೆ
    char_u	    ಫಲಿತಾಂಶ_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_ಸಿದ್ಧತೆ(&conv, p_enc, enc);
    ಆದರೆ (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ || strftime((ಅಕ್ಷರ *)ಫಲಿತಾಂಶ_buf, ಗಾತ್ರ(ಫಲಿತಾಂಶ_buf),
		(ಅಕ್ಷರ *)p, curtime) == 0)
	ಫಲಿತಾಂಶ_buf[0] = NUL;

    ಆದರೆ (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_ಸಿದ್ಧತೆ(&conv, enc, p_enc);
    ಆದರೆ (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, ಫಲಿತಾಂಶ_buf, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = vim_strsave(ಫಲಿತಾಂಶ_buf);

    // Release conversion descriptors
    convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
    vim_free(enc);
#  endif
}
# endif

# ಆದರೆ defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    ಶೂನ್ಯ
f_strptime(typval_T *argvars, typval_T *rettv)
{
    ರಚನೆ tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    CLEAR_ಕ್ಷೇತ್ರ(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_ಸಿದ್ಧತೆ(&conv, p_enc, enc);
    ಆದರೆ (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, ಖಾಲಿ);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    ಆದರೆ (fmt == ಖಾಲಿ
	    || strptime((ಅಕ್ಷರ *)str, (ಅಕ್ಷರ *)fmt, &tmval) == ಖಾಲಿ
	    || (rettv->vval.v_ಸಂಖ್ಯೆ = mktime(&tmval)) == -1)
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    ಆದರೆ (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
    vim_free(enc);
}
# endif

# ಆದರೆ defined(FEAT_TIMERS)
ಸ್ಥಿರ timer_T	*ಮೊದಲ_timer = ಖಾಲಿ;
ಸ್ಥಿರ ದೀರ್ಘ	ಕೊನೆಯ_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    ದೀರ್ಘ
proftime_ಸಮಯ_ಎಡ(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    ಆದರೆ (now->QuadPart > due->QuadPart)
	ಹಿಂತಿರುಗಿ 0;
    QueryPerformanceFrequency(&fr);
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)(((ದ್ವಿಗುಣ)(due->QuadPart - now->QuadPart)
		   / (ದ್ವಿಗುಣ)fr.QuadPart) * 1000);
#  ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (now->tv_sec > due->tv_sec)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೇರಿಸು_timer(timer_T *timer)
{
    timer->tr_ಮುಂದಿನ = ಮೊದಲ_timer;
    timer->tr_prev = ಖಾಲಿ;
    ಆದರೆ (ಮೊದಲ_timer != ಖಾಲಿ)
	ಮೊದಲ_timer->tr_prev = timer;
    ಮೊದಲ_timer = timer;
    did_ಕೂಡಿಸು_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_timer(timer_T *timer)
{
    ಆದರೆ (timer->tr_prev == ಖಾಲಿ)
	ಮೊದಲ_timer = timer->tr_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	timer->tr_prev->tr_ಮುಂದಿನ = timer->tr_ಮುಂದಿನ;
    ಆದರೆ (timer->tr_ಮುಂದಿನ != ಖಾಲಿ)
	timer->tr_ಮುಂದಿನ->tr_prev = timer->tr_prev;
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
create_timer(ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    ದೀರ್ಘ	prev_id = ಕೊನೆಯ_timer_id;

    ಆದರೆ (timer == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (++ಕೊನೆಯ_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	ಕೊನೆಯ_timer_id = 0;
    timer->tr_id = ಕೊನೆಯ_timer_id;
    ಸೇರಿಸು_timer(timer);
    ಆದರೆ (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_ಆರಂಭ(timer);
    ಹಿಂತಿರುಗಿ timer;
}

/*
 * (Re)start a timer.
 */
    ಶೂನ್ಯ
timer_ಆರಂಭ(timer_T *timer)
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
    ಆದರೆ (ch_log_ಸಕ್ರಿಯ())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(ಖಾಲಿ, "invoking timer callback %s",
	       cb->cb_partial != ಖಾಲಿ ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(ಖಾಲಿ, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    ದೀರ್ಘ
check_due_timer(ಶೂನ್ಯ)
{
    timer_T	*timer;
    timer_T	*timer_ಮುಂದಿನ;
    ದೀರ್ಘ	this_due;
    ದೀರ್ಘ	ಮುಂದಿನ_due = -1;
    proftime_T	now;
    ಪೂರ್ಣಾಂಕ		did_one = FALSE;
    ಪೂರ್ಣಾಂಕ		need_update_screen = FALSE;
    ದೀರ್ಘ	current_id = ಕೊನೆಯ_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    ಆದರೆ (exiting || aborting() || debug_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ ಮುಂದಿನ_due;

    profile_ಆರಂಭ(&now);
    ಗಾಗಿ (timer = ಮೊದಲ_timer; timer != ಖಾಲಿ && !got_int; timer = timer_ಮುಂದಿನ)
    {
	timer_ಮುಂದಿನ = timer->tr_ಮುಂದಿನ;

	ಆದರೆ (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    ಮುಂದುವರಿ;
	this_due = proftime_ಸಮಯ_ಎಡ(&timer->tr_due, &now);
	ಆದರೆ (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    ಪೂರ್ಣಾಂಕ save_timer_busy = timer_busy;
	    ಪೂರ್ಣಾಂಕ save_vgetc_busy = vgetc_busy;
	    ಪೂರ್ಣಾಂಕ save_did_emsg = did_emsg;
	    ಪೂರ್ಣಾಂಕ prev_uncaught_emsg = uncaught_emsg;
	    ಪೂರ್ಣಾಂಕ save_called_emsg = called_emsg;
	    ಪೂರ್ಣಾಂಕ save_must_redraw = must_redraw;
	    ಪೂರ್ಣಾಂಕ save_ex_pressedreturn = get_pressedreturn();
	    ಪೂರ್ಣಾಂಕ save_may_garbage_collect = may_garbage_collect;
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
	    timer_ಮುಂದಿನ = timer->tr_ಮುಂದಿನ;
	    did_one = TRUE;
	    timer_busy = save_timer_busy;
	    vgetc_busy = save_vgetc_busy;
	    ಆದರೆ (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_ಎಣಿಕೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		timer->tr_emsg_ಎಣಿಕೆ = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    ಆದರೆ (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    ಗಣ_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    ಆದರೆ (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_ಎಣಿಕೆ < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_ಸಮಯ_ಎಡ(&timer->tr_due, &now);
		ಆದರೆ (this_due < 1)
		    this_due = 1;
		ಆದರೆ (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		this_due = -1;
		ಆದರೆ (timer->tr_keep)
		    timer->tr_paused = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
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
	ಆದರೆ (this_due > 0 && (ಮುಂದಿನ_due == -1 || ಮುಂದಿನ_due > this_due))
	    ಮುಂದಿನ_due = this_due;
    }

    ಆದರೆ (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_ಪದ
    ಆದರೆ (bevalexpr_due_ಗಣ)
    {
	this_due = proftime_ಸಮಯ_ಎಡ(&bevalexpr_due, &now);
	ಆದರೆ (this_due <= 1)
	{
	    bevalexpr_due_ಗಣ = FALSE;
	    ಆದರೆ (balloonEval == ಖಾಲಿ)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloon_Eval_For_ಪದ = TRUE;
	    }
	    ಆದರೆ (balloonEval != ಖಾಲಿ)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		ಹೊರಹೋಗು_flush();
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಂದಿನ_due == -1 || ಮುಂದಿನ_due > this_due)
	    ಮುಂದಿನ_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    ಮುಂದಿನ_due = ಪದ_check_timers(ಮುಂದಿನ_due, &now);
#  endif

    ಹಿಂತಿರುಗಿ current_id != ಕೊನೆಯ_timer_id ? 1 : ಮುಂದಿನ_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    ಸ್ಥಿರ timer_T *
ಕಂಡುಹಿಡಿ_timer(ದೀರ್ಘ id)
{
    timer_T *timer;

    ಆದರೆ (id < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    FOR_ALL_TIMERS(timer)
	ಆದರೆ (timer->tr_id == id)
	    ಹಿಂತಿರುಗಿ timer;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}


/*
 * Stop a timer and delete it.
 */
    ಶೂನ್ಯ
stop_timer(timer_T *timer)
{
    ಆದರೆ (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
stop_all_timers(ಶೂನ್ಯ)
{
    timer_T *timer;
    timer_T *timer_ಮುಂದಿನ;

    ಗಾಗಿ (timer = ಮೊದಲ_timer; timer != ಖಾಲಿ; timer = timer_ಮುಂದಿನ)
    {
	timer_ಮುಂದಿನ = timer->tr_ಮುಂದಿನ;
	stop_timer(timer);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    ದೀರ್ಘ	ಉಳಿದ;
    proftime_T	now;

    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    list_append_dict(list, dict);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "id", timer->tr_id);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "time", (ದೀರ್ಘ)timer->tr_interval);

    profile_ಆರಂಭ(&now);
    ಉಳಿದ = proftime_ಸಮಯ_ಎಡ(&timer->tr_due, &now);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "remaining", (ದೀರ್ಘ)ಉಳಿದ);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "repeat",
	    (ದೀರ್ಘ)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "paused", (ದೀರ್ಘ)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    ಆದರೆ (di != ಖಾಲಿ)
    {
	ಆದರೆ (dict_ಕೂಡಿಸು(dict, di) == FAIL)
	    vim_free(di);
	ಇಲ್ಲದಿದ್ದರೆ
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	ಆದರೆ (timer->tr_id != -1)
	    ಕೂಡಿಸು_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_timer(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    ಗಾಗಿ (timer = ಮೊದಲ_timer; !abort && timer != ಖಾಲಿ; timer = timer->tr_ಮುಂದಿನ)
    {
	ಆದರೆ (timer->tr_callback.cb_partial != ಖಾಲಿ)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    }
    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    ಪೂರ್ಣಾಂಕ
timer_valid(timer_T *timer)
{
    ಆದರೆ (timer == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	ಆದರೆ (t == timer)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

#  ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
timer_free_all(ಶೂನ್ಯ)
{
    ಆಗ (ಮೊದಲ_timer != ಖಾಲಿ)
    {
	timer_T *timer = ಮೊದಲ_timer;
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
    timer_T *timer = ಖಾಲಿ;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = ಕಂಡುಹಿಡಿ_timer((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
	ಆದರೆ (timer != ಖಾಲಿ)
	    ಕೂಡಿಸು_timer_info(rettv, timer);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೂಡಿಸು_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    ಶೂನ್ಯ
f_timer_pause(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    timer_T	*timer = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_ಸಂಖ್ಯೆ)
    {
	emsg(_(e_ಸಂಖ್ಯೆ_expected));
	ಹಿಂತಿರುಗಿ;
    }

    ಪೂರ್ಣಾಂಕ	paused = (ಪೂರ್ಣಾಂಕ)tv_get_bool(&argvars[1]);

    timer = ಕಂಡುಹಿಡಿ_timer((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
    ಆದರೆ (timer != ಖಾಲಿ)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    ಶೂನ್ಯ
f_timer_ಆರಂಭ(typval_T *argvars, typval_T *rettv)
{
    ದೀರ್ಘ	msec;
    timer_T	*timer;
    ಪೂರ್ಣಾಂಕ		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    msec = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	dict = argvars[2].vval.v_dict;
	ಆದರೆ (dict_has_ಕೀಲಿ(dict, "repeat"))
	    repeat = dict_get_ಸಂಖ್ಯೆ(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    ಆದರೆ (callback.cb_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ಒಳ_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_ವಾದ));
	free_callback(&callback);
	ಹಿಂತಿರುಗಿ;
    }

    timer = create_timer(msec, repeat);
    ಆದರೆ (timer == ಖಾಲಿ)
    {
	free_callback(&callback);
	ಹಿಂತಿರುಗಿ;
    }
    ಗಣ_callback(&timer->tr_callback, &callback);
    rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    ಶೂನ್ಯ
f_timer_stop(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    timer_T *timer;

    ಆದರೆ (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    timer = ಕಂಡುಹಿಡಿ_timer((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
    ಆದರೆ (timer != ಖಾಲಿ)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    ಶೂನ್ಯ
f_timer_stopall(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# ಆದರೆ defined(STARTUPTIME)
ಸ್ಥಿರ ರಚನೆ timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gettimeofday(ರಚನೆ timeval *tv, ಅಕ್ಷರ *dummy ಬಳಸದ)
{
    ದೀರ್ಘ t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    ಹಿಂತಿರುಗಿ 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    ಶೂನ್ಯ
ಸಮಯ_ತಳ್ಳು(ಶೂನ್ಯ *tv_rel, ಶೂನ್ಯ *tv_ಆರಂಭ)
{
    *((ರಚನೆ timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, ಖಾಲಿ);
    ((ರಚನೆ timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((ರಚನೆ timeval *)tv_rel)->tv_usec;
    ((ರಚನೆ timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((ರಚನೆ timeval *)tv_rel)->tv_sec;
    ಆದರೆ (((ರಚನೆ timeval *)tv_rel)->tv_usec < 0)
    {
	((ರಚನೆ timeval *)tv_rel)->tv_usec += 1000000;
	--((ರಚನೆ timeval *)tv_rel)->tv_sec;
    }
    *(ರಚನೆ timeval *)tv_ಆರಂಭ = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    ಶೂನ್ಯ
ಸಮಯ_ತೆಗೆ(
    ಶೂನ್ಯ	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((ರಚನೆ timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((ರಚನೆ timeval *)tp)->tv_sec;
    ಆದರೆ (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಮಯ_diff(ರಚನೆ timeval *then, ರಚನೆ timeval *now)
{
    ದೀರ್ಘ	usec;
    ದೀರ್ಘ	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(ಸಮಯ_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    ಶೂನ್ಯ
ಸಮಯ_ಸಂದೇಶ_2(
    ಅಕ್ಷರ	*mesg,
    ಶೂನ್ಯ	*tv_ಆರಂಭ)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    ಸ್ಥಿರ ರಚನೆ timeval	ಆರಂಭ;
    ರಚನೆ timeval		now;

    ಆದರೆ (ಸಮಯ_fd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(mesg, "STARTING") != ಖಾಲಿ)
    {
	gettimeofday(&ಆರಂಭ, ಖಾಲಿ);
	prev_timeval = ಆರಂಭ;
	fprintf(ಸಮಯ_fd, "\n\ntimes in msec\n");
	fprintf(ಸಮಯ_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(ಸಮಯ_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, ಖಾಲಿ);
    ಸಮಯ_diff(&ಆರಂಭ, &now);
    ಆದರೆ (((ರಚನೆ timeval *)tv_ಆರಂಭ) != ಖಾಲಿ)
    {
	fprintf(ಸಮಯ_fd, "  ");
	ಸಮಯ_diff(((ರಚನೆ timeval *)tv_ಆರಂಭ), &now);
    }
    fprintf(ಸಮಯ_fd, "  ");
    ಸಮಯ_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(ಸಮಯ_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

#ಆದರೆ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    ಸಮಯ_T
get8ctime(FILE *fd)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಸಮಯ_T	n = 0;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	ಆದರೆ (c == EOF) ಹಿಂತಿರುಗಿ -1;
	n = (n << 8) + c;
    }
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    ಪೂರ್ಣಾಂಕ
put_ಸಮಯ(FILE *fd, ಸಮಯ_T the_ಸಮಯ)
{
    char_u	buf[8];

    ಸಮಯ_ಗಮ್ಯ_ಬೈಟುಗಳು(the_ಸಮಯ, buf);
    ಹಿಂತಿರುಗಿ fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    ಶೂನ್ಯ
ಸಮಯ_ಗಮ್ಯ_ಬೈಟುಗಳು(ಸಮಯ_T the_ಸಮಯ, char_u *buf)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		bi = 0;
    ಸಮಯ_T	wtime = the_ಸಮಯ;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    ಗಾಗಿ (i = 7; i >= 0; --i)
    {
	ಆದರೆ (i + 1 > (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(ಸಮಯ_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	{
# ಆದರೆ defined(SIZEOF_ಸಮಯ_T) && SIZEOF_ಸಮಯ_T > 4
	    c = (ಪೂರ್ಣಾಂಕ)(wtime >> (i * 8));
# ಇಲ್ಲದಿದ್ದರೆ
	    c = (ಪೂರ್ಣಾಂಕ)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_ಸಮಯ(char_u *buf, size_t buflen, time_t tt)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRFTIME
    ರಚನೆ tm	tmval;
    ರಚನೆ tm	*curtime;
    size_t	n;

    ಆದರೆ (vim_ಸಮಯ() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	ಆದರೆ (vim_ಸಮಯ() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((ಅಕ್ಷರ *)buf, buflen, "%H:%M:%S", curtime);
	ಇಲ್ಲದಿದ್ದರೆ
	    // longer ago
	    n = strftime((ಅಕ್ಷರ *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	ಆದರೆ (n == 0)
	    buf[0] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ದೀರ್ಘ seconds = (ದೀರ್ಘ)(vim_ಸಮಯ() - tt);

	vim_snprintf((ಅಕ್ಷರ *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
