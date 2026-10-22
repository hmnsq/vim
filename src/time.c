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
ساکن حرف	tz_cache[64];
#اگر_ختم

#تعریف_کرو FOR_ALL_TIMERS(t) \
    برائے ((t) = پہلا_timer; (t) != تہی; (t) = (t)->tr_اگلا)

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
    ساکن ساخت tm *
vim_localtime(
    ثابت time_t	*timep,		// timestamp for local representation
    ساخت tm		*نتیجہ غیر_مستعمل)	// pointer to caller return buffer
{
#اگر_تعریف_شدہ HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    حرف		*tz;		// pointer for TZ environment var

    tz = (حرف *)mch_getenv((char_u *)"TZ");
    اگر (tz == تہی)
	tz = "";
    اگر (STRNCMP(tz_cache, tz, حجم(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, حجم(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    واپس localtime_r(timep, نتیجہ);
#ورنہ
    واپس localtime(timep);
#اگر_ختم	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    وقت_T
vim_وقت(خالی)
{
#اگر_تعریف_شدہ FEAT_EVAL
    واپس وقت_for_testing == 0 ? وقت(تہی) : وقت_for_testing;
#ورنہ
    واپس وقت(تہی);
#اگر_ختم
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    حرف *
get_ctime(time_t thetime, صحیح جوڑو_newline)
{
    ساکن حرف buf[100];  // hopefully enough for every language
#اگر_تعریف_شدہ HAVE_STRFTIME
    ساخت tm	tmval;
    ساخت tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    اگر (curtime == تہی)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), حجم(buf) - 2);
    ورنہ
    {
	// xgettext:no-c-format
	اگر (strftime(buf, حجم(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
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
	اگر (enc_codepage >= 0 && (صحیح)GetACP() != enc_codepage)
	{
	    char_u	*مقصد_free = تہی;
	    صحیح		len;

	    acp_مقصد_enc((char_u *)buf, (صحیح)لڑی_لمبائی(buf), &مقصد_free, &len);
	    اگر (مقصد_free != تہی)
	    {
		STRNCPY(buf, مقصد_free, حجم(buf) - 2);
		vim_free(مقصد_free);
	    }
	}
# endif
    }
#ورنہ
    STRCPY(buf, "(unknown)");
#اگر_ختم
    اگر (جوڑو_newline)
	STRCAT(buf, "\n");
    واپس buf;
}

#اگر defined(FEAT_EVAL)

# اگر defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    خالی
f_localtime(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = (varnumber_T)وقت(تہی);
}

# اگر defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    ساکن صحیح
list2proftime(typval_T *arg, proftime_T *tm)
{
    طویل	n1, n2;
    صحیح	error = FALSE;

    اگر (arg->v_type != VAR_LIST || arg->vval.v_list == تہی
					     || arg->vval.v_list->lv_len != 2)
	واپس FAIL;
    n1 = list_ڈھونڈو_nr(arg->vval.v_list, 0L, &error);
    n2 = list_ڈھونڈو_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->بلند_Part = n1;
    tm->کم_Part = n2;
#  ورنہ
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    واپس error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    خالی
f_reltime(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	آغاز;
    طویل	n1, n2;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_آغاز(&res);
    }
    ورنہ اگر (argvars[1].v_type == VAR_UNKNOWN)
    {
	اگر (list2proftime(&argvars[0], &res) == FAIL)
	{
	    اگر (اندر_vim9script())
		emsg(_(e_invalid_دلیل));
	    واپس;
	}
	profile_انتہا(&res);
    }
    ورنہ
    {
	// Two arguments: compute the difference.
	اگر (list2proftime(&argvars[0], &آغاز) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    اگر (اندر_vim9script())
		emsg(_(e_invalid_دلیل));
	    واپس;
	}
	profile_sub(&res, &آغاز);
    }

#  ifdef MSWIN
    n1 = res.بلند_Part;
    n2 = res.کم_Part;
#  ورنہ
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)n1);
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    خالی
f_reltimefloat(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    اگر (اندر_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	واپس;

    اگر (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    ورنہ اگر (اندر_vim9script())
	emsg(_(e_invalid_دلیل));
# endif
}

/*
 * "reltimestr()" function
 */
    خالی
f_reltimestr(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
# ifdef FEAT_RELTIME
    اگر (اندر_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	واپس;

    proftime_T	tm;
    اگر (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_پیغام(&tm));
#  ورنہ
	ساکن حرف buf[50];
	طویل usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, حجم(buf), "%3ld.%06ld", (طویل)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    ورنہ اگر (اندر_vim9script())
	emsg(_(e_invalid_دلیل));
# endif
}

# اگر defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    خالی
f_strftime(typval_T *argvars, typval_T *rettv)
{
    ساخت tm	tmval;
    ساخت tm	*curtime;
    time_t	seconds;
    char_u	*p;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type == VAR_UNKNOWN)
	seconds = وقت(تہی);
    ورنہ
	seconds = (time_t)tv_get_نمبر(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    اگر (curtime == تہی)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	واپس;
    }

#  ifdef MSWIN
    WCHAR	    نتیجہ_buf[256];
    WCHAR	    *wp;

    wp = enc_مقصد_utf16(p, تہی);
    اگر (wp != تہی)
	(خالی)wcsftime(نتیجہ_buf, ARRAY_لمبائی(نتیجہ_buf), wp, curtime);
    ورنہ
	نتیجہ_buf[0] = NUL;
    rettv->vval.v_string = utf16_مقصد_enc(نتیجہ_buf, تہی);
    vim_free(wp);
#  ورنہ
    char_u	    نتیجہ_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_تیاری(&conv, p_enc, enc);
    اگر (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, تہی);
    اگر (p == تہی || strftime((حرف *)نتیجہ_buf, حجم(نتیجہ_buf),
		(حرف *)p, curtime) == 0)
	نتیجہ_buf[0] = NUL;

    اگر (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_تیاری(&conv, enc, p_enc);
    اگر (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, نتیجہ_buf, تہی);
    ورنہ
	rettv->vval.v_string = vim_strsave(نتیجہ_buf);

    // Release conversion descriptors
    convert_تیاری(&conv, تہی, تہی);
    vim_free(enc);
#  endif
}
# endif

# اگر defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    خالی
f_strptime(typval_T *argvars, typval_T *rettv)
{
    ساخت tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    CLEAR_میدان(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_تیاری(&conv, p_enc, enc);
    اگر (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, تہی);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    اگر (fmt == تہی
	    || strptime((حرف *)str, (حرف *)fmt, &tmval) == تہی
	    || (rettv->vval.v_نمبر = mktime(&tmval)) == -1)
	rettv->vval.v_نمبر = 0;

    اگر (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_تیاری(&conv, تہی, تہی);
    vim_free(enc);
}
# endif

# اگر defined(FEAT_TIMERS)
ساکن timer_T	*پہلا_timer = تہی;
ساکن طویل	آخری_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    طویل
proftime_وقت_بایاں(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    اگر (now->QuadPart > due->QuadPart)
	واپس 0;
    QueryPerformanceFrequency(&fr);
    واپس (طویل)(((دوہرا)(due->QuadPart - now->QuadPart)
		   / (دوہرا)fr.QuadPart) * 1000);
#  ورنہ
    اگر (now->tv_sec > due->tv_sec)
	واپس 0;
    واپس (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    ساکن خالی
داخل_کرو_timer(timer_T *timer)
{
    timer->tr_اگلا = پہلا_timer;
    timer->tr_پچھلا = تہی;
    اگر (پہلا_timer != تہی)
	پہلا_timer->tr_پچھلا = timer;
    پہلا_timer = timer;
    did_جوڑو_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    ساکن خالی
remove_timer(timer_T *timer)
{
    اگر (timer->tr_پچھلا == تہی)
	پہلا_timer = timer->tr_اگلا;
    ورنہ
	timer->tr_پچھلا->tr_اگلا = timer->tr_اگلا;
    اگر (timer->tr_اگلا != تہی)
	timer->tr_اگلا->tr_پچھلا = timer->tr_پچھلا;
}

    ساکن خالی
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
create_timer(طویل msec, صحیح repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    طویل	پچھلا_id = آخری_timer_id;

    اگر (timer == تہی)
	واپس تہی;
    اگر (++آخری_timer_id <= پچھلا_id)
	// Overflow!  Might cause duplicates...
	آخری_timer_id = 0;
    timer->tr_id = آخری_timer_id;
    داخل_کرو_timer(timer);
    اگر (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_آغاز(timer);
    واپس timer;
}

/*
 * (Re)start a timer.
 */
    خالی
timer_آغاز(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    ساکن خالی
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    اگر (ch_log_فعال())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(تہی, "invoking timer callback %s",
	       cb->cb_partial != تہی ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_نمبر;
    argv[0].vval.v_نمبر = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(تہی, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    طویل
check_due_timer(خالی)
{
    timer_T	*timer;
    timer_T	*timer_اگلا;
    طویل	this_due;
    طویل	اگلا_due = -1;
    proftime_T	now;
    صحیح		did_one = FALSE;
    صحیح		need_update_screen = FALSE;
    طویل	current_id = آخری_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    اگر (exiting || aborting() || debug_منوال)
	واپس اگلا_due;

    profile_آغاز(&now);
    برائے (timer = پہلا_timer; timer != تہی && !got_int; timer = timer_اگلا)
    {
	timer_اگلا = timer->tr_اگلا;

	اگر (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    جاری;
	this_due = proftime_وقت_بایاں(&timer->tr_due, &now);
	اگر (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    صحیح save_timer_busy = timer_busy;
	    صحیح save_vgetc_busy = vgetc_busy;
	    صحیح save_did_emsg = did_emsg;
	    صحیح پچھلا_uncaught_emsg = uncaught_emsg;
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
	    timer_اگلا = timer->tr_اگلا;
	    did_one = TRUE;
	    timer_busy = save_timer_busy;
	    vgetc_busy = save_vgetc_busy;
	    اگر (uncaught_emsg > پچھلا_uncaught_emsg)
		++timer->tr_emsg_گنتی;
	    ورنہ
		timer->tr_emsg_گنتی = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    اگر (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    سیٹ_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    اگر (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_گنتی < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_وقت_بایاں(&timer->tr_due, &now);
		اگر (this_due < 1)
		    this_due = 1;
		اگر (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    ورنہ
	    {
		this_due = -1;
		اگر (timer->tr_keep)
		    timer->tr_paused = TRUE;
		ورنہ
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
	اگر (this_due > 0 && (اگلا_due == -1 || اگلا_due > this_due))
	    اگلا_due = this_due;
    }

    اگر (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_رکن
    اگر (bevalexpr_due_سیٹ)
    {
	this_due = proftime_وقت_بایاں(&bevalexpr_due, &now);
	اگر (this_due <= 1)
	{
	    bevalexpr_due_سیٹ = FALSE;
	    اگر (balloonEval == تہی)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloon_Eval_For_رکن = TRUE;
	    }
	    اگر (balloonEval != تہی)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		برآمد_flush();
	    }
	}
	ورنہ اگر (اگلا_due == -1 || اگلا_due > this_due)
	    اگلا_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    اگلا_due = رکن_check_timers(اگلا_due, &now);
#  endif

    واپس current_id != آخری_timer_id ? 1 : اگلا_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    ساکن timer_T *
ڈھونڈو_timer(طویل id)
{
    timer_T *timer;

    اگر (id < 0)
	واپس تہی;

    FOR_ALL_TIMERS(timer)
	اگر (timer->tr_id == id)
	    واپس timer;
    واپس تہی;
}


/*
 * Stop a timer and delete it.
 */
    خالی
stop_timer(timer_T *timer)
{
    اگر (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    ورنہ
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    ساکن خالی
stop_all_timers(خالی)
{
    timer_T *timer;
    timer_T *timer_اگلا;

    برائے (timer = پہلا_timer; timer != تہی; timer = timer_اگلا)
    {
	timer_اگلا = timer->tr_اگلا;
	stop_timer(timer);
    }
}

    ساکن خالی
جوڑو_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    طویل	بقایا;
    proftime_T	now;

    اگر (dict == تہی)
	واپس;
    list_append_dict(list, dict);

    dict_جوڑو_نمبر(dict, "id", timer->tr_id);
    dict_جوڑو_نمبر(dict, "time", (طویل)timer->tr_interval);

    profile_آغاز(&now);
    بقایا = proftime_وقت_بایاں(&timer->tr_due, &now);
    dict_جوڑو_نمبر(dict, "remaining", (طویل)بقایا);

    dict_جوڑو_نمبر(dict, "repeat",
	    (طویل)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_جوڑو_نمبر(dict, "paused", (طویل)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    اگر (di != تہی)
    {
	اگر (dict_جوڑو(dict, di) == FAIL)
	    vim_free(di);
	ورنہ
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    ساکن خالی
جوڑو_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	اگر (timer->tr_id != -1)
	    جوڑو_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    صحیح
سیٹ_ref_اندر_timer(صحیح copyID)
{
    صحیح		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    برائے (timer = پہلا_timer; !abort && timer != تہی; timer = timer->tr_اگلا)
    {
	اگر (timer->tr_callback.cb_partial != تہی)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	ورنہ
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);
    }
    واپس abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    صحیح
timer_valid(timer_T *timer)
{
    اگر (timer == تہی)
	واپس FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	اگر (t == timer)
	    واپس TRUE;
    واپس FALSE;
}

#  اگر defined(EXITFREE)
    خالی
timer_free_all(خالی)
{
    جب (پہلا_timer != تہی)
    {
	timer_T *timer = پہلا_timer;
	remove_timer(timer);
	free_timer(timer);
    }
}
#  endif

/*
 * "timer_info([timer])" function
 */
    خالی
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = تہی;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = ڈھونڈو_timer((صحیح)tv_get_نمبر(&argvars[0]));
	اگر (timer != تہی)
	    جوڑو_timer_info(rettv, timer);
    }
    ورنہ
	جوڑو_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    خالی
f_timer_pause(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    timer_T	*timer = تہی;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[0].v_type != VAR_نمبر)
    {
	emsg(_(e_نمبر_expected));
	واپس;
    }

    صحیح	paused = (صحیح)tv_get_bool(&argvars[1]);

    timer = ڈھونڈو_timer((صحیح)tv_get_نمبر(&argvars[0]));
    اگر (timer != تہی)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    خالی
f_timer_آغاز(typval_T *argvars, typval_T *rettv)
{
    طویل	msec;
    timer_T	*timer;
    صحیح		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_نمبر = -1;
    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	واپس;

    msec = (طویل)tv_get_نمبر(&argvars[0]);
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	اگر (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    واپس;

	dict = argvars[2].vval.v_dict;
	اگر (dict_has_کلید(dict, "repeat"))
	    repeat = dict_get_نمبر(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    اگر (callback.cb_name == تہی)
	واپس;
    اگر (اندر_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_دلیل));
	free_callback(&callback);
	واپس;
    }

    timer = create_timer(msec, repeat);
    اگر (timer == تہی)
    {
	free_callback(&callback);
	واپس;
    }
    سیٹ_callback(&timer->tr_callback, &callback);
    rettv->vval.v_نمبر = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    خالی
f_timer_stop(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    timer_T *timer;

    اگر (check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    timer = ڈھونڈو_timer((صحیح)tv_get_نمبر(&argvars[0]));
    اگر (timer != تہی)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    خالی
f_timer_stopall(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# اگر defined(STARTUPTIME)
ساکن ساخت timeval	پچھلا_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    ساکن صحیح
gettimeofday(ساخت timeval *tv, حرف *dummy غیر_مستعمل)
{
    طویل t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    واپس 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    خالی
وقت_ڈالو(خالی *tv_rel, خالی *tv_آغاز)
{
    *((ساخت timeval *)tv_rel) = پچھلا_timeval;
    gettimeofday(&پچھلا_timeval, تہی);
    ((ساخت timeval *)tv_rel)->tv_usec = پچھلا_timeval.tv_usec
					- ((ساخت timeval *)tv_rel)->tv_usec;
    ((ساخت timeval *)tv_rel)->tv_sec = پچھلا_timeval.tv_sec
					 - ((ساخت timeval *)tv_rel)->tv_sec;
    اگر (((ساخت timeval *)tv_rel)->tv_usec < 0)
    {
	((ساخت timeval *)tv_rel)->tv_usec += 1000000;
	--((ساخت timeval *)tv_rel)->tv_sec;
    }
    *(ساخت timeval *)tv_آغاز = پچھلا_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    خالی
وقت_نکالو(
    خالی	*tp)	// actually (struct timeval *)
{
    پچھلا_timeval.tv_usec -= ((ساخت timeval *)tp)->tv_usec;
    پچھلا_timeval.tv_sec -= ((ساخت timeval *)tp)->tv_sec;
    اگر (پچھلا_timeval.tv_usec < 0)
    {
	پچھلا_timeval.tv_usec += 1000000;
	--پچھلا_timeval.tv_sec;
    }
}

    ساکن خالی
وقت_diff(ساخت timeval *then, ساخت timeval *now)
{
    طویل	usec;
    طویل	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(وقت_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    خالی
وقت_پیغام_2(
    حرف	*mesg,
    خالی	*tv_آغاز)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    ساکن ساخت timeval	آغاز;
    ساخت timeval		now;

    اگر (وقت_fd == تہی)
	واپس;

    اگر (لڑی_تلاش(mesg, "STARTING") != تہی)
    {
	gettimeofday(&آغاز, تہی);
	پچھلا_timeval = آغاز;
	fprintf(وقت_fd, "\n\ntimes in msec\n");
	fprintf(وقت_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(وقت_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, تہی);
    وقت_diff(&آغاز, &now);
    اگر (((ساخت timeval *)tv_آغاز) != تہی)
    {
	fprintf(وقت_fd, "  ");
	وقت_diff(((ساخت timeval *)tv_آغاز), &now);
    }
    fprintf(وقت_fd, "  ");
    وقت_diff(&پچھلا_timeval, &now);
    پچھلا_timeval = now;
    fprintf(وقت_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#اگر_ختم // FEAT_EVAL

#اگر defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    وقت_T
get8ctime(FILE *fd)
{
    صحیح		c;
    وقت_T	n = 0;
    صحیح		i;

    برائے (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	اگر (c == EOF) واپس -1;
	n = (n << 8) + c;
    }
    واپس n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    صحیح
put_وقت(FILE *fd, وقت_T the_وقت)
{
    char_u	buf[8];

    وقت_مقصد_بائٹیں(the_وقت, buf);
    واپس fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    خالی
وقت_مقصد_بائٹیں(وقت_T the_وقت, char_u *buf)
{
    صحیح		c;
    صحیح		i;
    صحیح		bi = 0;
    وقت_T	wtime = the_وقت;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    برائے (i = 7; i >= 0; --i)
    {
	اگر (i + 1 > (صحیح)حجم(وقت_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	ورنہ
	{
# اگر defined(SIZEOF_وقت_T) && SIZEOF_وقت_T > 4
	    c = (صحیح)(wtime >> (i * 8));
# ورنہ
	    c = (صحیح)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#اگر_ختم

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    خالی
جوڑو_وقت(char_u *buf, size_t buflen, time_t tt)
{
#اگر_تعریف_شدہ HAVE_STRFTIME
    ساخت tm	tmval;
    ساخت tm	*curtime;
    size_t	n;

    اگر (vim_وقت() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	اگر (vim_وقت() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((حرف *)buf, buflen, "%H:%M:%S", curtime);
	ورنہ
	    // longer ago
	    n = strftime((حرف *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	اگر (n == 0)
	    buf[0] = NUL;
    }
    ورنہ
#اگر_ختم
    {
	طویل seconds = (طویل)(vim_وقت() - tt);

	vim_snprintf((حرف *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
