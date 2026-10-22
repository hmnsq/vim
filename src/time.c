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

#અન્તર્ભાવ "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#જો defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
સ્થિર અક્ષર	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    માટે ((t) = first_timer; (t) != ખાલી; (t) = (t)->tr_next)

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
    સ્થિર સંરચના tm *
vim_localtime(
    અચલ time_t	*timep,		// timestamp for local representation
    સંરચના tm		*result UNUSED)	// pointer to caller return buffer
{
#ifdef HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    અક્ષર		*tz;		// pointer for TZ environment var

    tz = (અક્ષર *)mch_getenv((char_u *)"TZ");
    જો (tz == ખાલી)
	tz = "";
    જો (STRNCMP(tz_cache, tz, કદ(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, કદ(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    પાછા localtime_r(timep, result);
#નહીંતર
    પાછા localtime(timep);
#endif	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    time_T
vim_time(શૂન્ય)
{
#ifdef FEAT_EVAL
    પાછા time_for_testing == 0 ? time(ખાલી) : time_for_testing;
#નહીંતર
    પાછા time(ખાલી);
#endif
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    અક્ષર *
get_ctime(time_t thetime, પૂર્ણાંક add_newline)
{
    સ્થિર અક્ષર buf[100];  // hopefully enough for every language
#ifdef HAVE_STRFTIME
    સંરચના tm	tmval;
    સંરચના tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    જો (curtime == ખાલી)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), કદ(buf) - 2);
    નહીંતર
    {
	// xgettext:no-c-format
	જો (strftime(buf, કદ(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      કદ(buf) - 2);
	}
# ifdef MSWIN
	જો (enc_codepage >= 0 && (પૂર્ણાંક)GetACP() != enc_codepage)
	{
	    char_u	*to_free = ખાલી;
	    પૂર્ણાંક		len;

	    acp_to_enc((char_u *)buf, (પૂર્ણાંક)strlen(buf), &to_free, &len);
	    જો (to_free != ખાલી)
	    {
		STRNCPY(buf, to_free, કદ(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#નહીંતર
    STRCPY(buf, "(unknown)");
#endif
    જો (add_newline)
	STRCAT(buf, "\n");
    પાછા buf;
}

#જો defined(FEAT_EVAL)

# જો defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    શૂન્ય
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = (varnumber_T)time(ખાલી);
}

# જો defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    સ્થિર પૂર્ણાંક
list2proftime(typval_T *arg, proftime_T *tm)
{
    long	n1, n2;
    પૂર્ણાંક	error = FALSE;

    જો (arg->v_type != VAR_LIST || arg->vval.v_list == ખાલી
					     || arg->vval.v_list->lv_len != 2)
	પાછા FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->ઉપલું_Part = n1;
    tm->નીચલું_Part = n2;
#  નહીંતર
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    પાછા error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    શૂન્ય
f_reltime(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	start;
    long	n1, n2;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	પાછા;

    જો (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    નહીંતર જો (argvars[1].v_type == VAR_UNKNOWN)
    {
	જો (list2proftime(&argvars[0], &res) == FAIL)
	{
	    જો (in_vim9script())
		emsg(_(e_invalid_argument));
	    પાછા;
	}
	profile_end(&res);
    }
    નહીંતર
    {
	// Two arguments: compute the difference.
	જો (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    જો (in_vim9script())
		emsg(_(e_invalid_argument));
	    પાછા;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.ઉપલું_Part;
    n2 = res.નીચલું_Part;
#  નહીંતર
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)n1);
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    શૂન્ય
f_reltimefloat(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    જો (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	પાછા;

    જો (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    નહીંતર જો (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

/*
 * "reltimestr()" function
 */
    શૂન્ય
f_reltimestr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;
# ifdef FEAT_RELTIME
    જો (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	પાછા;

    proftime_T	tm;
    જો (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_સંદેશ(&tm));
#  નહીંતર
	સ્થિર અક્ષર buf[50];
	long usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, કદ(buf), "%3ld.%06ld", (long)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    નહીંતર જો (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# જો defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    શૂન્ય
f_strftime(typval_T *argvars, typval_T *rettv)
{
    સંરચના tm	tmval;
    સંરચના tm	*curtime;
    time_t	seconds;
    char_u	*p;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    જો (argvars[1].v_type == VAR_UNKNOWN)
	seconds = time(ખાલી);
    નહીંતર
	seconds = (time_t)tv_get_સંખ્યા(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    જો (curtime == ખાલી)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	પાછા;
    }

#  ifdef MSWIN
    WCHAR	    result_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, ખાલી);
    જો (wp != ખાલી)
	(શૂન્ય)wcsftime(result_buf, ARRAY_LENGTH(result_buf), wp, curtime);
    નહીંતર
	result_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(result_buf, ખાલી);
    vim_free(wp);
#  નહીંતર
    char_u	    result_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    જો (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, ખાલી);
    જો (p == ખાલી || strftime((અક્ષર *)result_buf, કદ(result_buf),
		(અક્ષર *)p, curtime) == 0)
	result_buf[0] = NUL;

    જો (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    જો (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, result_buf, ખાલી);
    નહીંતર
	rettv->vval.v_string = vim_strsave(result_buf);

    // Release conversion descriptors
    convert_setup(&conv, ખાલી, ખાલી);
    vim_free(enc);
#  endif
}
# endif

# જો defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    શૂન્ય
f_strptime(typval_T *argvars, typval_T *rettv)
{
    સંરચના tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    જો (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, ખાલી);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    જો (fmt == ખાલી
	    || strptime((અક્ષર *)str, (અક્ષર *)fmt, &tmval) == ખાલી
	    || (rettv->vval.v_સંખ્યા = mktime(&tmval)) == -1)
	rettv->vval.v_સંખ્યા = 0;

    જો (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, ખાલી, ખાલી);
    vim_free(enc);
}
# endif

# જો defined(FEAT_TIMERS)
સ્થિર timer_T	*first_timer = ખાલી;
સ્થિર long	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    long
proftime_time_ડાબું(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    જો (now->QuadPart > due->QuadPart)
	પાછા 0;
    QueryPerformanceFrequency(&fr);
    પાછા (long)(((દ્વિગુણ)(due->QuadPart - now->QuadPart)
		   / (દ્વિગુણ)fr.QuadPart) * 1000);
#  નહીંતર
    જો (now->tv_sec > due->tv_sec)
	પાછા 0;
    પાછા (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    સ્થિર શૂન્ય
ઉમેરો_timer(timer_T *timer)
{
    timer->tr_next = first_timer;
    timer->tr_prev = ખાલી;
    જો (first_timer != ખાલી)
	first_timer->tr_prev = timer;
    first_timer = timer;
    did_add_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    સ્થિર શૂન્ય
remove_timer(timer_T *timer)
{
    જો (timer->tr_prev == ખાલી)
	first_timer = timer->tr_next;
    નહીંતર
	timer->tr_prev->tr_next = timer->tr_next;
    જો (timer->tr_next != ખાલી)
	timer->tr_next->tr_prev = timer->tr_prev;
}

    સ્થિર શૂન્ય
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
create_timer(long msec, પૂર્ણાંક repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    long	prev_id = last_timer_id;

    જો (timer == ખાલી)
	પાછા ખાલી;
    જો (++last_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    ઉમેરો_timer(timer);
    જો (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    પાછા timer;
}

/*
 * (Re)start a timer.
 */
    શૂન્ય
timer_start(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    સ્થિર શૂન્ય
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    જો (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(ખાલી, "invoking timer callback %s",
	       cb->cb_partial != ખાલી ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_સંખ્યા;
    argv[0].vval.v_સંખ્યા = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(ખાલી, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    long
check_due_timer(શૂન્ય)
{
    timer_T	*timer;
    timer_T	*timer_next;
    long	this_due;
    long	next_due = -1;
    proftime_T	now;
    પૂર્ણાંક		did_one = FALSE;
    પૂર્ણાંક		need_update_screen = FALSE;
    long	current_id = last_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    જો (exiting || aborting() || debug_mode)
	પાછા next_due;

    profile_start(&now);
    માટે (timer = first_timer; timer != ખાલી && !got_int; timer = timer_next)
    {
	timer_next = timer->tr_next;

	જો (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    ચાલુ;
	this_due = proftime_time_ડાબું(&timer->tr_due, &now);
	જો (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    પૂર્ણાંક save_timer_busy = timer_busy;
	    પૂર્ણાંક save_vgetc_busy = vgetc_busy;
	    પૂર્ણાંક save_did_emsg = did_emsg;
	    પૂર્ણાંક prev_uncaught_emsg = uncaught_emsg;
	    પૂર્ણાંક save_called_emsg = called_emsg;
	    પૂર્ણાંક save_must_redraw = must_redraw;
	    પૂર્ણાંક save_ex_pressedreturn = get_pressedreturn();
	    પૂર્ણાંક save_may_garbage_collect = may_garbage_collect;
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
	    જો (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_ગણતરી;
	    નહીંતર
		timer->tr_emsg_ગણતરી = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    જો (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    જો (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_ગણતરી < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_ડાબું(&timer->tr_due, &now);
		જો (this_due < 1)
		    this_due = 1;
		જો (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    નહીંતર
	    {
		this_due = -1;
		જો (timer->tr_keep)
		    timer->tr_paused = TRUE;
		નહીંતર
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
	જો (this_due > 0 && (next_due == -1 || next_due > this_due))
	    next_due = this_due;
    }

    જો (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    જો (bevalexpr_due_set)
    {
	this_due = proftime_time_ડાબું(&bevalexpr_due, &now);
	જો (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    જો (balloonEval == ખાલી)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    જો (balloonEval != ખાલી)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	નહીંતર જો (next_due == -1 || next_due > this_due)
	    next_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    next_due = term_check_timers(next_due, &now);
#  endif

    પાછા current_id != last_timer_id ? 1 : next_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    સ્થિર timer_T *
find_timer(long id)
{
    timer_T *timer;

    જો (id < 0)
	પાછા ખાલી;

    FOR_ALL_TIMERS(timer)
	જો (timer->tr_id == id)
	    પાછા timer;
    પાછા ખાલી;
}


/*
 * Stop a timer and delete it.
 */
    શૂન્ય
stop_timer(timer_T *timer)
{
    જો (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    નહીંતર
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    સ્થિર શૂન્ય
stop_all_timers(શૂન્ય)
{
    timer_T *timer;
    timer_T *timer_next;

    માટે (timer = first_timer; timer != ખાલી; timer = timer_next)
    {
	timer_next = timer->tr_next;
	stop_timer(timer);
    }
}

    સ્થિર શૂન્ય
add_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    long	remaining;
    proftime_T	now;

    જો (dict == ખાલી)
	પાછા;
    list_append_dict(list, dict);

    dict_add_સંખ્યા(dict, "id", timer->tr_id);
    dict_add_સંખ્યા(dict, "time", (long)timer->tr_interval);

    profile_start(&now);
    remaining = proftime_time_ડાબું(&timer->tr_due, &now);
    dict_add_સંખ્યા(dict, "remaining", (long)remaining);

    dict_add_સંખ્યા(dict, "repeat",
	    (long)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_add_સંખ્યા(dict, "paused", (long)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    જો (di != ખાલી)
    {
	જો (dict_add(dict, di) == FAIL)
	    vim_free(di);
	નહીંતર
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    સ્થિર શૂન્ય
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	જો (timer->tr_id != -1)
	    add_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    પૂર્ણાંક
set_ref_in_timer(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    માટે (timer = first_timer; !abort && timer != ખાલી; timer = timer->tr_next)
    {
	જો (timer->tr_callback.cb_partial != ખાલી)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	નહીંતર
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);
    }
    પાછા abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    પૂર્ણાંક
timer_valid(timer_T *timer)
{
    જો (timer == ખાલી)
	પાછા FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	જો (t == timer)
	    પાછા TRUE;
    પાછા FALSE;
}

#  જો defined(EXITFREE)
    શૂન્ય
timer_free_all(શૂન્ય)
{
    જ્યારે (first_timer != ખાલી)
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
    શૂન્ય
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = ખાલી;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]));
	જો (timer != ખાલી)
	    add_timer_info(rettv, timer);
    }
    નહીંતર
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    શૂન્ય
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = ખાલી;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[0].v_type != VAR_સંખ્યા)
    {
	emsg(_(e_સંખ્યા_expected));
	પાછા;
    }

    પૂર્ણાંક	paused = (પૂર્ણાંક)tv_get_bool(&argvars[1]);

    timer = find_timer((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]));
    જો (timer != ખાલી)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    શૂન્ય
f_timer_start(typval_T *argvars, typval_T *rettv)
{
    long	msec;
    timer_T	*timer;
    પૂર્ણાંક		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_સંખ્યા = -1;
    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	પાછા;

    msec = (long)tv_get_સંખ્યા(&argvars[0]);
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	જો (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    પાછા;

	dict = argvars[2].vval.v_dict;
	જો (dict_has_ચાવી(dict, "repeat"))
	    repeat = dict_get_સંખ્યા(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    જો (callback.cb_name == ખાલી)
	પાછા;
    જો (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	પાછા;
    }

    timer = create_timer(msec, repeat);
    જો (timer == ખાલી)
    {
	free_callback(&callback);
	પાછા;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_સંખ્યા = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    શૂન્ય
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    જો (check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    timer = find_timer((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]));
    જો (timer != ખાલી)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    શૂન્ય
f_timer_stopall(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# જો defined(STARTUPTIME)
સ્થિર સંરચના timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    સ્થિર પૂર્ણાંક
gettimeofday(સંરચના timeval *tv, અક્ષર *dummy UNUSED)
{
    long t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    પાછા 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    શૂન્ય
time_ધકેલો(શૂન્ય *tv_rel, શૂન્ય *tv_start)
{
    *((સંરચના timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, ખાલી);
    ((સંરચના timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((સંરચના timeval *)tv_rel)->tv_usec;
    ((સંરચના timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((સંરચના timeval *)tv_rel)->tv_sec;
    જો (((સંરચના timeval *)tv_rel)->tv_usec < 0)
    {
	((સંરચના timeval *)tv_rel)->tv_usec += 1000000;
	--((સંરચના timeval *)tv_rel)->tv_sec;
    }
    *(સંરચના timeval *)tv_start = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    શૂન્ય
time_કાઢો(
    શૂન્ય	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((સંરચના timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((સંરચના timeval *)tp)->tv_sec;
    જો (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    સ્થિર શૂન્ય
time_diff(સંરચના timeval *then, સંરચના timeval *now)
{
    long	usec;
    long	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(time_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    શૂન્ય
time_સંદેશ(
    અક્ષર	*mesg,
    શૂન્ય	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    સ્થિર સંરચના timeval	start;
    સંરચના timeval		now;

    જો (time_fd == ખાલી)
	પાછા;

    જો (strstr(mesg, "STARTING") != ખાલી)
    {
	gettimeofday(&start, ખાલી);
	prev_timeval = start;
	fprintf(time_fd, "\n\ntimes in msec\n");
	fprintf(time_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, ખાલી);
    time_diff(&start, &now);
    જો (((સંરચના timeval *)tv_start) != ખાલી)
    {
	fprintf(time_fd, "  ");
	time_diff(((સંરચના timeval *)tv_start), &now);
    }
    fprintf(time_fd, "  ");
    time_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(time_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#endif // FEAT_EVAL

#જો defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    time_T
get8ctime(FILE *fd)
{
    પૂર્ણાંક		c;
    time_T	n = 0;
    પૂર્ણાંક		i;

    માટે (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	જો (c == EOF) પાછા -1;
	n = (n << 8) + c;
    }
    પાછા n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    પૂર્ણાંક
put_time(FILE *fd, time_T the_time)
{
    char_u	buf[8];

    time_to_bytes(the_time, buf);
    પાછા fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    શૂન્ય
time_to_bytes(time_T the_time, char_u *buf)
{
    પૂર્ણાંક		c;
    પૂર્ણાંક		i;
    પૂર્ણાંક		bi = 0;
    time_T	wtime = the_time;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    માટે (i = 7; i >= 0; --i)
    {
	જો (i + 1 > (પૂર્ણાંક)કદ(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	નહીંતર
	{
# જો defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (પૂર્ણાંક)(wtime >> (i * 8));
# નહીંતર
	    c = (પૂર્ણાંક)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#endif

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    શૂન્ય
add_time(char_u *buf, size_t buflen, time_t tt)
{
#ifdef HAVE_STRFTIME
    સંરચના tm	tmval;
    સંરચના tm	*curtime;
    size_t	n;

    જો (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	જો (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((અક્ષર *)buf, buflen, "%H:%M:%S", curtime);
	નહીંતર
	    // longer ago
	    n = strftime((અક્ષર *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	જો (n == 0)
	    buf[0] = NUL;
    }
    નહીંતર
#endif
    {
	long seconds = (long)(vim_time() - tt);

	vim_snprintf((અક્ષર *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
