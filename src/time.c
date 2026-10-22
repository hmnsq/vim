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

#అన్తర్భావ "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#ఒకవేళ defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
స్థిర అక్షరం	tz_cache[64];
#అయితే_ముగింపు

#నిర్వచించు FOR_ALL_TIMERS(t) \
    కొరకు ((t) = మొదటి_timer; (t) != ఖాళీ; (t) = (t)->tr_తదుపరి)

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
    స్థిర నిర్మాణం tm *
vim_localtime(
    స్థిరరాశి time_t	*timep,		// timestamp for local representation
    నిర్మాణం tm		*ఫలితం వాడని)	// pointer to caller return buffer
{
#నిర్వచితమైతే HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    అక్షరం		*tz;		// pointer for TZ environment var

    tz = (అక్షరం *)mch_getenv((char_u *)"TZ");
    ఒకవేళ (tz == ఖాళీ)
	tz = "";
    ఒకవేళ (STRNCMP(tz_cache, tz, పరిమాణం(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, పరిమాణం(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    తిరిగి localtime_r(timep, ఫలితం);
#లేకపోతే
    తిరిగి localtime(timep);
#అయితే_ముగింపు	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    సమయం_T
vim_సమయం(శూన్య)
{
#నిర్వచితమైతే FEAT_EVAL
    తిరిగి సమయం_for_testing == 0 ? సమయం(ఖాళీ) : సమయం_for_testing;
#లేకపోతే
    తిరిగి సమయం(ఖాళీ);
#అయితే_ముగింపు
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    అక్షరం *
get_ctime(time_t thetime, పూర్ణాంకం కూడు_newline)
{
    స్థిర అక్షరం buf[100];  // hopefully enough for every language
#నిర్వచితమైతే HAVE_STRFTIME
    నిర్మాణం tm	tmval;
    నిర్మాణం tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ఒకవేళ (curtime == ఖాళీ)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), పరిమాణం(buf) - 2);
    లేకపోతే
    {
	// xgettext:no-c-format
	ఒకవేళ (strftime(buf, పరిమాణం(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      పరిమాణం(buf) - 2);
	}
# ifdef MSWIN
	ఒకవేళ (enc_codepage >= 0 && (పూర్ణాంకం)GetACP() != enc_codepage)
	{
	    char_u	*గమ్యం_free = ఖాళీ;
	    పూర్ణాంకం		len;

	    acp_గమ్యం_enc((char_u *)buf, (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(buf), &గమ్యం_free, &len);
	    ఒకవేళ (గమ్యం_free != ఖాళీ)
	    {
		STRNCPY(buf, గమ్యం_free, పరిమాణం(buf) - 2);
		vim_free(గమ్యం_free);
	    }
	}
# endif
    }
#లేకపోతే
    STRCPY(buf, "(unknown)");
#అయితే_ముగింపు
    ఒకవేళ (కూడు_newline)
	STRCAT(buf, "\n");
    తిరిగి buf;
}

#ఒకవేళ defined(FEAT_EVAL)

# ఒకవేళ defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    శూన్య
f_localtime(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = (varnumber_T)సమయం(ఖాళీ);
}

# ఒకవేళ defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    స్థిర పూర్ణాంకం
list2proftime(typval_T *arg, proftime_T *tm)
{
    దీర్ఘ	n1, n2;
    పూర్ణాంకం	error = FALSE;

    ఒకవేళ (arg->v_type != VAR_LIST || arg->vval.v_list == ఖాళీ
					     || arg->vval.v_list->lv_len != 2)
	తిరిగి FAIL;
    n1 = list_కనుగొను_nr(arg->vval.v_list, 0L, &error);
    n2 = list_కనుగొను_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->ఎగువ_Part = n1;
    tm->దిగువ_Part = n2;
#  లేకపోతే
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    తిరిగి error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    శూన్య
f_reltime(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	ప్రారంభం;
    దీర్ఘ	n1, n2;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_ప్రారంభం(&res);
    }
    లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ఒకవేళ (list2proftime(&argvars[0], &res) == FAIL)
	{
	    ఒకవేళ (లోపల_vim9script())
		emsg(_(e_invalid_వాదం));
	    తిరిగి;
	}
	profile_చివర(&res);
    }
    లేకపోతే
    {
	// Two arguments: compute the difference.
	ఒకవేళ (list2proftime(&argvars[0], &ప్రారంభం) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    ఒకవేళ (లోపల_vim9script())
		emsg(_(e_invalid_వాదం));
	    తిరిగి;
	}
	profile_sub(&res, &ప్రారంభం);
    }

#  ifdef MSWIN
    n1 = res.ఎగువ_Part;
    n2 = res.దిగువ_Part;
#  లేకపోతే
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)n1);
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    శూన్య
f_reltimefloat(typval_T *argvars వాడని, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    ఒకవేళ (లోపల_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    లేకపోతే ఒకవేళ (లోపల_vim9script())
	emsg(_(e_invalid_వాదం));
# endif
}

/*
 * "reltimestr()" function
 */
    శూన్య
f_reltimestr(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
# ifdef FEAT_RELTIME
    ఒకవేళ (లోపల_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    proftime_T	tm;
    ఒకవేళ (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_సందేశం(&tm));
#  లేకపోతే
	స్థిర అక్షరం buf[50];
	దీర్ఘ usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, పరిమాణం(buf), "%3ld.%06ld", (దీర్ఘ)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    లేకపోతే ఒకవేళ (లోపల_vim9script())
	emsg(_(e_invalid_వాదం));
# endif
}

# ఒకవేళ defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    శూన్య
f_strftime(typval_T *argvars, typval_T *rettv)
{
    నిర్మాణం tm	tmval;
    నిర్మాణం tm	*curtime;
    time_t	seconds;
    char_u	*p;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
	seconds = సమయం(ఖాళీ);
    లేకపోతే
	seconds = (time_t)tv_get_సంఖ్య(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ఒకవేళ (curtime == ఖాళీ)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	తిరిగి;
    }

#  ifdef MSWIN
    WCHAR	    ఫలితం_buf[256];
    WCHAR	    *wp;

    wp = enc_గమ్యం_utf16(p, ఖాళీ);
    ఒకవేళ (wp != ఖాళీ)
	(శూన్య)wcsftime(ఫలితం_buf, ARRAY_నిడివి(ఫలితం_buf), wp, curtime);
    లేకపోతే
	ఫలితం_buf[0] = NUL;
    rettv->vval.v_string = utf16_గమ్యం_enc(ఫలితం_buf, ఖాళీ);
    vim_free(wp);
#  లేకపోతే
    char_u	    ఫలితం_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_అమరిక(&conv, p_enc, enc);
    ఒకవేళ (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, ఖాళీ);
    ఒకవేళ (p == ఖాళీ || strftime((అక్షరం *)ఫలితం_buf, పరిమాణం(ఫలితం_buf),
		(అక్షరం *)p, curtime) == 0)
	ఫలితం_buf[0] = NUL;

    ఒకవేళ (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_అమరిక(&conv, enc, p_enc);
    ఒకవేళ (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, ఫలితం_buf, ఖాళీ);
    లేకపోతే
	rettv->vval.v_string = vim_strsave(ఫలితం_buf);

    // Release conversion descriptors
    convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
    vim_free(enc);
#  endif
}
# endif

# ఒకవేళ defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    శూన్య
f_strptime(typval_T *argvars, typval_T *rettv)
{
    నిర్మాణం tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    CLEAR_క్షేత్రం(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_అమరిక(&conv, p_enc, enc);
    ఒకవేళ (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, ఖాళీ);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    ఒకవేళ (fmt == ఖాళీ
	    || strptime((అక్షరం *)str, (అక్షరం *)fmt, &tmval) == ఖాళీ
	    || (rettv->vval.v_సంఖ్య = mktime(&tmval)) == -1)
	rettv->vval.v_సంఖ్య = 0;

    ఒకవేళ (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
    vim_free(enc);
}
# endif

# ఒకవేళ defined(FEAT_TIMERS)
స్థిర timer_T	*మొదటి_timer = ఖాళీ;
స్థిర దీర్ఘ	చివరి_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    దీర్ఘ
proftime_సమయం_ఎడమ(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    ఒకవేళ (now->QuadPart > due->QuadPart)
	తిరిగి 0;
    QueryPerformanceFrequency(&fr);
    తిరిగి (దీర్ఘ)(((ద్విగుణ)(due->QuadPart - now->QuadPart)
		   / (ద్విగుణ)fr.QuadPart) * 1000);
#  లేకపోతే
    ఒకవేళ (now->tv_sec > due->tv_sec)
	తిరిగి 0;
    తిరిగి (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    స్థిర శూన్య
చొప్పించు_timer(timer_T *timer)
{
    timer->tr_తదుపరి = మొదటి_timer;
    timer->tr_prev = ఖాళీ;
    ఒకవేళ (మొదటి_timer != ఖాళీ)
	మొదటి_timer->tr_prev = timer;
    మొదటి_timer = timer;
    did_కూడు_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    స్థిర శూన్య
remove_timer(timer_T *timer)
{
    ఒకవేళ (timer->tr_prev == ఖాళీ)
	మొదటి_timer = timer->tr_తదుపరి;
    లేకపోతే
	timer->tr_prev->tr_తదుపరి = timer->tr_తదుపరి;
    ఒకవేళ (timer->tr_తదుపరి != ఖాళీ)
	timer->tr_తదుపరి->tr_prev = timer->tr_prev;
}

    స్థిర శూన్య
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
create_timer(దీర్ఘ msec, పూర్ణాంకం repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    దీర్ఘ	prev_id = చివరి_timer_id;

    ఒకవేళ (timer == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (++చివరి_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	చివరి_timer_id = 0;
    timer->tr_id = చివరి_timer_id;
    చొప్పించు_timer(timer);
    ఒకవేళ (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_ప్రారంభం(timer);
    తిరిగి timer;
}

/*
 * (Re)start a timer.
 */
    శూన్య
timer_ప్రారంభం(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    స్థిర శూన్య
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    ఒకవేళ (ch_log_చురుకైన())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(ఖాళీ, "invoking timer callback %s",
	       cb->cb_partial != ఖాళీ ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_సంఖ్య;
    argv[0].vval.v_సంఖ్య = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(ఖాళీ, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    దీర్ఘ
check_due_timer(శూన్య)
{
    timer_T	*timer;
    timer_T	*timer_తదుపరి;
    దీర్ఘ	this_due;
    దీర్ఘ	తదుపరి_due = -1;
    proftime_T	now;
    పూర్ణాంకం		did_one = FALSE;
    పూర్ణాంకం		need_update_screen = FALSE;
    దీర్ఘ	current_id = చివరి_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    ఒకవేళ (exiting || aborting() || debug_బహుళకం)
	తిరిగి తదుపరి_due;

    profile_ప్రారంభం(&now);
    కొరకు (timer = మొదటి_timer; timer != ఖాళీ && !got_int; timer = timer_తదుపరి)
    {
	timer_తదుపరి = timer->tr_తదుపరి;

	ఒకవేళ (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    కొనసాగు;
	this_due = proftime_సమయం_ఎడమ(&timer->tr_due, &now);
	ఒకవేళ (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    పూర్ణాంకం save_timer_busy = timer_busy;
	    పూర్ణాంకం save_vgetc_busy = vgetc_busy;
	    పూర్ణాంకం save_did_emsg = did_emsg;
	    పూర్ణాంకం prev_uncaught_emsg = uncaught_emsg;
	    పూర్ణాంకం save_called_emsg = called_emsg;
	    పూర్ణాంకం save_must_redraw = must_redraw;
	    పూర్ణాంకం save_ex_pressedreturn = get_pressedreturn();
	    పూర్ణాంకం save_may_garbage_collect = may_garbage_collect;
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
	    timer_తదుపరి = timer->tr_తదుపరి;
	    did_one = TRUE;
	    timer_busy = save_timer_busy;
	    vgetc_busy = save_vgetc_busy;
	    ఒకవేళ (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_లెక్క;
	    లేకపోతే
		timer->tr_emsg_లెక్క = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    ఒకవేళ (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    సమితి_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    ఒకవేళ (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_లెక్క < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_సమయం_ఎడమ(&timer->tr_due, &now);
		ఒకవేళ (this_due < 1)
		    this_due = 1;
		ఒకవేళ (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    లేకపోతే
	    {
		this_due = -1;
		ఒకవేళ (timer->tr_keep)
		    timer->tr_paused = TRUE;
		లేకపోతే
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
	ఒకవేళ (this_due > 0 && (తదుపరి_due == -1 || తదుపరి_due > this_due))
	    తదుపరి_due = this_due;
    }

    ఒకవేళ (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_పదం
    ఒకవేళ (bevalexpr_due_సమితి)
    {
	this_due = proftime_సమయం_ఎడమ(&bevalexpr_due, &now);
	ఒకవేళ (this_due <= 1)
	{
	    bevalexpr_due_సమితి = FALSE;
	    ఒకవేళ (balloonEval == ఖాళీ)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloon_Eval_For_పదం = TRUE;
	    }
	    ఒకవేళ (balloonEval != ఖాళీ)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		బహిర్గతం_flush();
	    }
	}
	లేకపోతే ఒకవేళ (తదుపరి_due == -1 || తదుపరి_due > this_due)
	    తదుపరి_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    తదుపరి_due = పదం_check_timers(తదుపరి_due, &now);
#  endif

    తిరిగి current_id != చివరి_timer_id ? 1 : తదుపరి_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    స్థిర timer_T *
కనుగొను_timer(దీర్ఘ id)
{
    timer_T *timer;

    ఒకవేళ (id < 0)
	తిరిగి ఖాళీ;

    FOR_ALL_TIMERS(timer)
	ఒకవేళ (timer->tr_id == id)
	    తిరిగి timer;
    తిరిగి ఖాళీ;
}


/*
 * Stop a timer and delete it.
 */
    శూన్య
stop_timer(timer_T *timer)
{
    ఒకవేళ (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    లేకపోతే
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    స్థిర శూన్య
stop_all_timers(శూన్య)
{
    timer_T *timer;
    timer_T *timer_తదుపరి;

    కొరకు (timer = మొదటి_timer; timer != ఖాళీ; timer = timer_తదుపరి)
    {
	timer_తదుపరి = timer->tr_తదుపరి;
	stop_timer(timer);
    }
}

    స్థిర శూన్య
కూడు_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    దీర్ఘ	మిగిలిన;
    proftime_T	now;

    ఒకవేళ (dict == ఖాళీ)
	తిరిగి;
    list_append_dict(list, dict);

    dict_కూడు_సంఖ్య(dict, "id", timer->tr_id);
    dict_కూడు_సంఖ్య(dict, "time", (దీర్ఘ)timer->tr_interval);

    profile_ప్రారంభం(&now);
    మిగిలిన = proftime_సమయం_ఎడమ(&timer->tr_due, &now);
    dict_కూడు_సంఖ్య(dict, "remaining", (దీర్ఘ)మిగిలిన);

    dict_కూడు_సంఖ్య(dict, "repeat",
	    (దీర్ఘ)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_కూడు_సంఖ్య(dict, "paused", (దీర్ఘ)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    ఒకవేళ (di != ఖాళీ)
    {
	ఒకవేళ (dict_కూడు(dict, di) == FAIL)
	    vim_free(di);
	లేకపోతే
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    స్థిర శూన్య
కూడు_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	ఒకవేళ (timer->tr_id != -1)
	    కూడు_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_timer(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    కొరకు (timer = మొదటి_timer; !abort && timer != ఖాళీ; timer = timer->tr_తదుపరి)
    {
	ఒకవేళ (timer->tr_callback.cb_partial != ఖాళీ)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	లేకపోతే
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
    }
    తిరిగి abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    పూర్ణాంకం
timer_valid(timer_T *timer)
{
    ఒకవేళ (timer == ఖాళీ)
	తిరిగి FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	ఒకవేళ (t == timer)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

#  ఒకవేళ defined(EXITFREE)
    శూన్య
timer_free_all(శూన్య)
{
    అయితే (మొదటి_timer != ఖాళీ)
    {
	timer_T *timer = మొదటి_timer;
	remove_timer(timer);
	free_timer(timer);
    }
}
#  endif

/*
 * "timer_info([timer])" function
 */
    శూన్య
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = ఖాళీ;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = కనుగొను_timer((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]));
	ఒకవేళ (timer != ఖాళీ)
	    కూడు_timer_info(rettv, timer);
    }
    లేకపోతే
	కూడు_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    శూన్య
f_timer_pause(typval_T *argvars, typval_T *rettv వాడని)
{
    timer_T	*timer = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_సంఖ్య)
    {
	emsg(_(e_సంఖ్య_expected));
	తిరిగి;
    }

    పూర్ణాంకం	paused = (పూర్ణాంకం)tv_get_bool(&argvars[1]);

    timer = కనుగొను_timer((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]));
    ఒకవేళ (timer != ఖాళీ)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    శూన్య
f_timer_ప్రారంభం(typval_T *argvars, typval_T *rettv)
{
    దీర్ఘ	msec;
    timer_T	*timer;
    పూర్ణాంకం		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	తిరిగి;

    msec = (దీర్ఘ)tv_get_సంఖ్య(&argvars[0]);
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    తిరిగి;

	dict = argvars[2].vval.v_dict;
	ఒకవేళ (dict_has_కీలకం(dict, "repeat"))
	    repeat = dict_get_సంఖ్య(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    ఒకవేళ (callback.cb_name == ఖాళీ)
	తిరిగి;
    ఒకవేళ (లోపల_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_వాదం));
	free_callback(&callback);
	తిరిగి;
    }

    timer = create_timer(msec, repeat);
    ఒకవేళ (timer == ఖాళీ)
    {
	free_callback(&callback);
	తిరిగి;
    }
    సమితి_callback(&timer->tr_callback, &callback);
    rettv->vval.v_సంఖ్య = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    శూన్య
f_timer_stop(typval_T *argvars, typval_T *rettv వాడని)
{
    timer_T *timer;

    ఒకవేళ (check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    timer = కనుగొను_timer((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]));
    ఒకవేళ (timer != ఖాళీ)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    శూన్య
f_timer_stopall(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# ఒకవేళ defined(STARTUPTIME)
స్థిర నిర్మాణం timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    స్థిర పూర్ణాంకం
gettimeofday(నిర్మాణం timeval *tv, అక్షరం *dummy వాడని)
{
    దీర్ఘ t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    తిరిగి 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    శూన్య
సమయం_నెట్టు(శూన్య *tv_rel, శూన్య *tv_ప్రారంభం)
{
    *((నిర్మాణం timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, ఖాళీ);
    ((నిర్మాణం timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((నిర్మాణం timeval *)tv_rel)->tv_usec;
    ((నిర్మాణం timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((నిర్మాణం timeval *)tv_rel)->tv_sec;
    ఒకవేళ (((నిర్మాణం timeval *)tv_rel)->tv_usec < 0)
    {
	((నిర్మాణం timeval *)tv_rel)->tv_usec += 1000000;
	--((నిర్మాణం timeval *)tv_rel)->tv_sec;
    }
    *(నిర్మాణం timeval *)tv_ప్రారంభం = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    శూన్య
సమయం_తీయి(
    శూన్య	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((నిర్మాణం timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((నిర్మాణం timeval *)tp)->tv_sec;
    ఒకవేళ (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    స్థిర శూన్య
సమయం_diff(నిర్మాణం timeval *then, నిర్మాణం timeval *now)
{
    దీర్ఘ	usec;
    దీర్ఘ	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(సమయం_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    శూన్య
సమయం_సందేశం_2(
    అక్షరం	*mesg,
    శూన్య	*tv_ప్రారంభం)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    స్థిర నిర్మాణం timeval	ప్రారంభం;
    నిర్మాణం timeval		now;

    ఒకవేళ (సమయం_fd == ఖాళీ)
	తిరిగి;

    ఒకవేళ (అక్షరశ్రేణి_శోధన(mesg, "STARTING") != ఖాళీ)
    {
	gettimeofday(&ప్రారంభం, ఖాళీ);
	prev_timeval = ప్రారంభం;
	fprintf(సమయం_fd, "\n\ntimes in msec\n");
	fprintf(సమయం_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(సమయం_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, ఖాళీ);
    సమయం_diff(&ప్రారంభం, &now);
    ఒకవేళ (((నిర్మాణం timeval *)tv_ప్రారంభం) != ఖాళీ)
    {
	fprintf(సమయం_fd, "  ");
	సమయం_diff(((నిర్మాణం timeval *)tv_ప్రారంభం), &now);
    }
    fprintf(సమయం_fd, "  ");
    సమయం_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(సమయం_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#అయితే_ముగింపు // FEAT_EVAL

#ఒకవేళ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    సమయం_T
get8ctime(FILE *fd)
{
    పూర్ణాంకం		c;
    సమయం_T	n = 0;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	ఒకవేళ (c == EOF) తిరిగి -1;
	n = (n << 8) + c;
    }
    తిరిగి n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    పూర్ణాంకం
put_సమయం(FILE *fd, సమయం_T the_సమయం)
{
    char_u	buf[8];

    సమయం_గమ్యం_బైట్లు(the_సమయం, buf);
    తిరిగి fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    శూన్య
సమయం_గమ్యం_బైట్లు(సమయం_T the_సమయం, char_u *buf)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		i;
    పూర్ణాంకం		bi = 0;
    సమయం_T	wtime = the_సమయం;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    కొరకు (i = 7; i >= 0; --i)
    {
	ఒకవేళ (i + 1 > (పూర్ణాంకం)పరిమాణం(సమయం_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	లేకపోతే
	{
# ఒకవేళ defined(SIZEOF_సమయం_T) && SIZEOF_సమయం_T > 4
	    c = (పూర్ణాంకం)(wtime >> (i * 8));
# లేకపోతే
	    c = (పూర్ణాంకం)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#అయితే_ముగింపు

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    శూన్య
కూడు_సమయం(char_u *buf, size_t buflen, time_t tt)
{
#నిర్వచితమైతే HAVE_STRFTIME
    నిర్మాణం tm	tmval;
    నిర్మాణం tm	*curtime;
    size_t	n;

    ఒకవేళ (vim_సమయం() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	ఒకవేళ (vim_సమయం() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((అక్షరం *)buf, buflen, "%H:%M:%S", curtime);
	లేకపోతే
	    // longer ago
	    n = strftime((అక్షరం *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	ఒకవేళ (n == 0)
	    buf[0] = NUL;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	దీర్ఘ seconds = (దీర్ఘ)(vim_సమయం() - tt);

	vim_snprintf((అక్షరం *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
