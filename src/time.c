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
#إذا defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
ساكن حرف	tz_cache[64];
#پایان_اگر

#تعریف_کن FOR_ALL_TIMERS(t) \
    لكل ((t) = اولی_timer; (t) != عدم; (t) = (t)->tr_بعدی)

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
    بنية tm		*نتیجه استفادهنشده)	// pointer to caller return buffer
{
#اگر_تعریف_شده HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    حرف		*tz;		// pointer for TZ environment var

    tz = (حرف *)mch_getenv((char_u *)"TZ");
    إذا (tz == عدم)
	tz = "";
    إذا (STRNCMP(tz_cache, tz, حجم(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, حجم(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    أرجع localtime_r(timep, نتیجه);
#وإلا
    أرجع localtime(timep);
#پایان_اگر	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    زمان_T
vim_زمان(فراغ)
{
#اگر_تعریف_شده FEAT_EVAL
    أرجع زمان_for_testing == 0 ? زمان(عدم) : زمان_for_testing;
#وإلا
    أرجع زمان(عدم);
#پایان_اگر
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    حرف *
get_ctime(time_t thetime, صحيح جمع_کن_newline)
{
    ساكن حرف buf[100];  // hopefully enough for every language
#اگر_تعریف_شده HAVE_STRFTIME
    بنية tm	tmval;
    بنية tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    إذا (curtime == عدم)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), حجم(buf) - 2);
    وإلا
    {
	// xgettext:no-c-format
	إذا (strftime(buf, حجم(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
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
	إذا (enc_codepage >= 0 && (صحيح)GetACP() != enc_codepage)
	{
	    char_u	*مقصد_free = عدم;
	    صحيح		len;

	    acp_مقصد_enc((char_u *)buf, (صحيح)طول_رشته(buf), &مقصد_free, &len);
	    إذا (مقصد_free != عدم)
	    {
		STRNCPY(buf, مقصد_free, حجم(buf) - 2);
		vim_free(مقصد_free);
	    }
	}
# endif
    }
#وإلا
    STRCPY(buf, "(unknown)");
#پایان_اگر
    إذا (جمع_کن_newline)
	STRCAT(buf, "\n");
    أرجع buf;
}

#إذا defined(FEAT_EVAL)

# إذا defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    فراغ
f_localtime(typval_T *argvars استفادهنشده, typval_T *rettv)
{
    rettv->vval.v_عدد = (varnumber_T)زمان(عدم);
}

# إذا defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    ساكن صحيح
list2proftime(typval_T *arg, proftime_T *tm)
{
    دراز	n1, n2;
    صحيح	error = FALSE;

    إذا (arg->v_type != VAR_LIST || arg->vval.v_list == عدم
					     || arg->vval.v_list->lv_len != 2)
	أرجع FAIL;
    n1 = list_پیدا_کن_nr(arg->vval.v_list, 0L, &error);
    n2 = list_پیدا_کن_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->بالا_Part = n1;
    tm->کم_Part = n2;
#  وإلا
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    أرجع error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    فراغ
f_reltime(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	شروع;
    دراز	n1, n2;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_شروع(&res);
    }
    وإلا إذا (argvars[1].v_type == VAR_UNKNOWN)
    {
	إذا (list2proftime(&argvars[0], &res) == FAIL)
	{
	    إذا (درون_vim9script())
		emsg(_(e_invalid_آرگومان));
	    أرجع;
	}
	profile_انتها(&res);
    }
    وإلا
    {
	// Two arguments: compute the difference.
	إذا (list2proftime(&argvars[0], &شروع) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    إذا (درون_vim9script())
		emsg(_(e_invalid_آرگومان));
	    أرجع;
	}
	profile_sub(&res, &شروع);
    }

#  ifdef MSWIN
    n1 = res.بالا_Part;
    n2 = res.کم_Part;
#  وإلا
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
    فراغ
f_reltimefloat(typval_T *argvars استفادهنشده, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    إذا (درون_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    وإلا إذا (درون_vim9script())
	emsg(_(e_invalid_آرگومان));
# endif
}

/*
 * "reltimestr()" function
 */
    فراغ
f_reltimestr(typval_T *argvars استفادهنشده, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
# ifdef FEAT_RELTIME
    إذا (درون_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	أرجع;

    proftime_T	tm;
    إذا (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_پیام(&tm));
#  وإلا
	ساكن حرف buf[50];
	دراز usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, حجم(buf), "%3ld.%06ld", (دراز)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    وإلا إذا (درون_vim9script())
	emsg(_(e_invalid_آرگومان));
# endif
}

# إذا defined(HAVE_STRFTIME)
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

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    إذا (argvars[1].v_type == VAR_UNKNOWN)
	seconds = زمان(عدم);
    وإلا
	seconds = (time_t)tv_get_عدد(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    إذا (curtime == عدم)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	أرجع;
    }

#  ifdef MSWIN
    WCHAR	    نتیجه_buf[256];
    WCHAR	    *wp;

    wp = enc_مقصد_utf16(p, عدم);
    إذا (wp != عدم)
	(فراغ)wcsftime(نتیجه_buf, ARRAY_درازی(نتیجه_buf), wp, curtime);
    وإلا
	نتیجه_buf[0] = NUL;
    rettv->vval.v_string = utf16_مقصد_enc(نتیجه_buf, عدم);
    vim_free(wp);
#  وإلا
    char_u	    نتیجه_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_راهاندازی(&conv, p_enc, enc);
    إذا (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, عدم);
    إذا (p == عدم || strftime((حرف *)نتیجه_buf, حجم(نتیجه_buf),
		(حرف *)p, curtime) == 0)
	نتیجه_buf[0] = NUL;

    إذا (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_راهاندازی(&conv, enc, p_enc);
    إذا (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, نتیجه_buf, عدم);
    وإلا
	rettv->vval.v_string = vim_strsave(نتیجه_buf);

    // Release conversion descriptors
    convert_راهاندازی(&conv, عدم, عدم);
    vim_free(enc);
#  endif
}
# endif

# إذا defined(HAVE_STRPTIME)
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

    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    CLEAR_فیلد(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_راهاندازی(&conv, p_enc, enc);
    إذا (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, عدم);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    إذا (fmt == عدم
	    || strptime((حرف *)str, (حرف *)fmt, &tmval) == عدم
	    || (rettv->vval.v_عدد = mktime(&tmval)) == -1)
	rettv->vval.v_عدد = 0;

    إذا (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_راهاندازی(&conv, عدم, عدم);
    vim_free(enc);
}
# endif

# إذا defined(FEAT_TIMERS)
ساكن timer_T	*اولی_timer = عدم;
ساكن دراز	آخرین_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    دراز
proftime_زمان_چپ(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    إذا (now->QuadPart > due->QuadPart)
	أرجع 0;
    QueryPerformanceFrequency(&fr);
    أرجع (دراز)(((مزدوج)(due->QuadPart - now->QuadPart)
		   / (مزدوج)fr.QuadPart) * 1000);
#  وإلا
    إذا (now->tv_sec > due->tv_sec)
	أرجع 0;
    أرجع (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    ساكن فراغ
درج_timer(timer_T *timer)
{
    timer->tr_بعدی = اولی_timer;
    timer->tr_prev = عدم;
    إذا (اولی_timer != عدم)
	اولی_timer->tr_prev = timer;
    اولی_timer = timer;
    did_جمع_کن_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    ساكن فراغ
remove_timer(timer_T *timer)
{
    إذا (timer->tr_prev == عدم)
	اولی_timer = timer->tr_بعدی;
    وإلا
	timer->tr_prev->tr_بعدی = timer->tr_بعدی;
    إذا (timer->tr_بعدی != عدم)
	timer->tr_بعدی->tr_prev = timer->tr_prev;
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
create_timer(دراز msec, صحيح repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    دراز	prev_id = آخرین_timer_id;

    إذا (timer == عدم)
	أرجع عدم;
    إذا (++آخرین_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	آخرین_timer_id = 0;
    timer->tr_id = آخرین_timer_id;
    درج_timer(timer);
    إذا (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_شروع(timer);
    أرجع timer;
}

/*
 * (Re)start a timer.
 */
    فراغ
timer_شروع(timer_T *timer)
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
    إذا (ch_log_فعال())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(عدم, "invoking timer callback %s",
	       cb->cb_partial != عدم ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = (varnumber_T)timer->tr_id;
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
    دراز
check_due_timer(فراغ)
{
    timer_T	*timer;
    timer_T	*timer_بعدی;
    دراز	this_due;
    دراز	بعدی_due = -1;
    proftime_T	now;
    صحيح		did_one = FALSE;
    صحيح		need_update_screen = FALSE;
    دراز	current_id = آخرین_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    إذا (exiting || aborting() || debug_نما)
	أرجع بعدی_due;

    profile_شروع(&now);
    لكل (timer = اولی_timer; timer != عدم && !got_int; timer = timer_بعدی)
    {
	timer_بعدی = timer->tr_بعدی;

	إذا (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    استمر;
	this_due = proftime_زمان_چپ(&timer->tr_due, &now);
	إذا (this_due <= 1)
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
	    timer_بعدی = timer->tr_بعدی;
	    did_one = TRUE;
	    timer_busy = save_timer_busy;
	    vgetc_busy = save_vgetc_busy;
	    إذا (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_شمار;
	    وإلا
		timer->tr_emsg_شمار = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    إذا (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    مجموعه_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    إذا (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_شمار < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_زمان_چپ(&timer->tr_due, &now);
		إذا (this_due < 1)
		    this_due = 1;
		إذا (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    وإلا
	    {
		this_due = -1;
		إذا (timer->tr_keep)
		    timer->tr_paused = TRUE;
		وإلا
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
	إذا (this_due > 0 && (بعدی_due == -1 || بعدی_due > this_due))
	    بعدی_due = this_due;
    }

    إذا (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_جمله
    إذا (bevalexpr_due_مجموعه)
    {
	this_due = proftime_زمان_چپ(&bevalexpr_due, &now);
	إذا (this_due <= 1)
	{
	    bevalexpr_due_مجموعه = FALSE;
	    إذا (balloonEval == عدم)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloon_Eval_For_جمله = TRUE;
	    }
	    إذا (balloonEval != عدم)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		بیرون_flush();
	    }
	}
	وإلا إذا (بعدی_due == -1 || بعدی_due > this_due)
	    بعدی_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    بعدی_due = جمله_check_timers(بعدی_due, &now);
#  endif

    أرجع current_id != آخرین_timer_id ? 1 : بعدی_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    ساكن timer_T *
پیدا_کن_timer(دراز id)
{
    timer_T *timer;

    إذا (id < 0)
	أرجع عدم;

    FOR_ALL_TIMERS(timer)
	إذا (timer->tr_id == id)
	    أرجع timer;
    أرجع عدم;
}


/*
 * Stop a timer and delete it.
 */
    فراغ
stop_timer(timer_T *timer)
{
    إذا (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    وإلا
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    ساكن فراغ
stop_all_timers(فراغ)
{
    timer_T *timer;
    timer_T *timer_بعدی;

    لكل (timer = اولی_timer; timer != عدم; timer = timer_بعدی)
    {
	timer_بعدی = timer->tr_بعدی;
	stop_timer(timer);
    }
}

    ساكن فراغ
جمع_کن_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    دراز	مانده;
    proftime_T	now;

    إذا (dict == عدم)
	أرجع;
    list_append_dict(list, dict);

    dict_جمع_کن_عدد(dict, "id", timer->tr_id);
    dict_جمع_کن_عدد(dict, "time", (دراز)timer->tr_interval);

    profile_شروع(&now);
    مانده = proftime_زمان_چپ(&timer->tr_due, &now);
    dict_جمع_کن_عدد(dict, "remaining", (دراز)مانده);

    dict_جمع_کن_عدد(dict, "repeat",
	    (دراز)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_جمع_کن_عدد(dict, "paused", (دراز)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    إذا (di != عدم)
    {
	إذا (dict_جمع_کن(dict, di) == FAIL)
	    vim_free(di);
	وإلا
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    ساكن فراغ
جمع_کن_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	إذا (timer->tr_id != -1)
	    جمع_کن_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    صحيح
مجموعه_ref_درون_timer(صحيح copyID)
{
    صحيح		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    لكل (timer = اولی_timer; !abort && timer != عدم; timer = timer->tr_بعدی)
    {
	إذا (timer->tr_callback.cb_partial != عدم)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	وإلا
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || مجموعه_ref_درون_قلم(&tv, copyID, عدم, عدم, عدم);
    }
    أرجع abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    صحيح
timer_valid(timer_T *timer)
{
    إذا (timer == عدم)
	أرجع FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	إذا (t == timer)
	    أرجع TRUE;
    أرجع FALSE;
}

#  إذا defined(EXITFREE)
    فراغ
timer_free_all(فراغ)
{
    طالما (اولی_timer != عدم)
    {
	timer_T *timer = اولی_timer;
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

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = پیدا_کن_timer((صحيح)tv_get_عدد(&argvars[0]));
	إذا (timer != عدم)
	    جمع_کن_timer_info(rettv, timer);
    }
    وإلا
	جمع_کن_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    فراغ
f_timer_pause(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    timer_T	*timer = عدم;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (argvars[0].v_type != VAR_عدد)
    {
	emsg(_(e_عدد_expected));
	أرجع;
    }

    صحيح	paused = (صحيح)tv_get_bool(&argvars[1]);

    timer = پیدا_کن_timer((صحيح)tv_get_عدد(&argvars[0]));
    إذا (timer != عدم)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    فراغ
f_timer_شروع(typval_T *argvars, typval_T *rettv)
{
    دراز	msec;
    timer_T	*timer;
    صحيح		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_عدد = -1;
    إذا (check_secure())
	أرجع;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    msec = (دراز)tv_get_عدد(&argvars[0]);
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	إذا (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    أرجع;

	dict = argvars[2].vval.v_dict;
	إذا (dict_has_کلید(dict, "repeat"))
	    repeat = dict_get_عدد(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    إذا (callback.cb_name == عدم)
	أرجع;
    إذا (درون_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_آرگومان));
	free_callback(&callback);
	أرجع;
    }

    timer = create_timer(msec, repeat);
    إذا (timer == عدم)
    {
	free_callback(&callback);
	أرجع;
    }
    مجموعه_callback(&timer->tr_callback, &callback);
    rettv->vval.v_عدد = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    فراغ
f_timer_stop(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    timer_T *timer;

    إذا (check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    timer = پیدا_کن_timer((صحيح)tv_get_عدد(&argvars[0]));
    إذا (timer != عدم)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    فراغ
f_timer_stopall(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# إذا defined(STARTUPTIME)
ساكن بنية timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    ساكن صحيح
gettimeofday(بنية timeval *tv, حرف *dummy استفادهنشده)
{
    دراز t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    أرجع 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    فراغ
زمان_افزودن(فراغ *tv_rel, فراغ *tv_شروع)
{
    *((بنية timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, عدم);
    ((بنية timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((بنية timeval *)tv_rel)->tv_usec;
    ((بنية timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((بنية timeval *)tv_rel)->tv_sec;
    إذا (((بنية timeval *)tv_rel)->tv_usec < 0)
    {
	((بنية timeval *)tv_rel)->tv_usec += 1000000;
	--((بنية timeval *)tv_rel)->tv_sec;
    }
    *(بنية timeval *)tv_شروع = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    فراغ
زمان_برداشت(
    فراغ	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((بنية timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((بنية timeval *)tp)->tv_sec;
    إذا (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    ساكن فراغ
زمان_diff(بنية timeval *then, بنية timeval *now)
{
    دراز	usec;
    دراز	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(زمان_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    فراغ
زمان_پیام_2(
    حرف	*mesg,
    فراغ	*tv_شروع)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    ساكن بنية timeval	شروع;
    بنية timeval		now;

    إذا (زمان_fd == عدم)
	أرجع;

    إذا (جستجوی_رشته(mesg, "STARTING") != عدم)
    {
	gettimeofday(&شروع, عدم);
	prev_timeval = شروع;
	fprintf(زمان_fd, "\n\ntimes in msec\n");
	fprintf(زمان_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(زمان_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, عدم);
    زمان_diff(&شروع, &now);
    إذا (((بنية timeval *)tv_شروع) != عدم)
    {
	fprintf(زمان_fd, "  ");
	زمان_diff(((بنية timeval *)tv_شروع), &now);
    }
    fprintf(زمان_fd, "  ");
    زمان_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(زمان_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#پایان_اگر // FEAT_EVAL

#إذا defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    زمان_T
get8ctime(FILE *fd)
{
    صحيح		c;
    زمان_T	n = 0;
    صحيح		i;

    لكل (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	إذا (c == EOF) أرجع -1;
	n = (n << 8) + c;
    }
    أرجع n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    صحيح
put_زمان(FILE *fd, زمان_T the_زمان)
{
    char_u	buf[8];

    زمان_مقصد_بایتها(the_زمان, buf);
    أرجع fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    فراغ
زمان_مقصد_بایتها(زمان_T the_زمان, char_u *buf)
{
    صحيح		c;
    صحيح		i;
    صحيح		bi = 0;
    زمان_T	wtime = the_زمان;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    لكل (i = 7; i >= 0; --i)
    {
	إذا (i + 1 > (صحيح)حجم(زمان_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	وإلا
	{
# إذا defined(SIZEOF_زمان_T) && SIZEOF_زمان_T > 4
	    c = (صحيح)(wtime >> (i * 8));
# وإلا
	    c = (صحيح)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#پایان_اگر

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    فراغ
جمع_کن_زمان(char_u *buf, size_t buflen, time_t tt)
{
#اگر_تعریف_شده HAVE_STRFTIME
    بنية tm	tmval;
    بنية tm	*curtime;
    size_t	n;

    إذا (vim_زمان() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	إذا (vim_زمان() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((حرف *)buf, buflen, "%H:%M:%S", curtime);
	وإلا
	    // longer ago
	    n = strftime((حرف *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	إذا (n == 0)
	    buf[0] = NUL;
    }
    وإلا
#پایان_اگر
    {
	دراز seconds = (دراز)(vim_زمان() - tt);

	vim_snprintf((حرف *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
