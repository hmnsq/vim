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

#include "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#ប្រសិនបើ defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
static var_na	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    សម្រាប់ ((t) = first_timer; (t) != NULL; (t) = (t)->tr_next)

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
    static sa_nracanaa tm *
vim_localtime(
    const time_t	*timep,		// timestamp for local representation
    sa_nracanaa tm		*result UNUSED)	// pointer to caller return buffer
{
#ifdef HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    var_na		*tz;		// pointer for TZ environment var

    tz = (var_na *)mch_getenv((char_u *)"TZ");
    ប្រសិនបើ (tz == NULL)
	tz = "";
    ប្រសិនបើ (STRNCMP(tz_cache, tz, parimaa_na(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, parimaa_na(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    ត្រឡប់ localtime_r(timep, result);
#បើមិនដូច្នេះ
    ត្រឡប់ localtime(timep);
#endif	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    time_T
vim_time(void)
{
#ifdef FEAT_EVAL
    ត្រឡប់ time_for_testing == 0 ? time(NULL) : time_for_testing;
#បើមិនដូច្នេះ
    ត្រឡប់ time(NULL);
#endif
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    var_na *
get_ctime(time_t thetime, ចំនួនគត់ add_newline)
{
    static var_na buf[100];  // hopefully enough for every language
#ifdef HAVE_STRFTIME
    sa_nracanaa tm	tmval;
    sa_nracanaa tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ប្រសិនបើ (curtime == NULL)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), parimaa_na(buf) - 2);
    បើមិនដូច្នេះ
    {
	// xgettext:no-c-format
	ប្រសិនបើ (strftime(buf, parimaa_na(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      parimaa_na(buf) - 2);
	}
# ifdef MSWIN
	ប្រសិនបើ (enc_codepage >= 0 && (ចំនួនគត់)GetACP() != enc_codepage)
	{
	    char_u	*to_free = NULL;
	    ចំនួនគត់		len;

	    acp_to_enc((char_u *)buf, (ចំនួនគត់)strlen(buf), &to_free, &len);
	    ប្រសិនបើ (to_free != NULL)
	    {
		STRNCPY(buf, to_free, parimaa_na(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#បើមិនដូច្នេះ
    STRCPY(buf, "(unknown)");
#endif
    ប្រសិនបើ (add_newline)
	STRCAT(buf, "\n");
    ត្រឡប់ buf;
}

#ប្រសិនបើ defined(FEAT_EVAL)

# ប្រសិនបើ defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    void
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = (varnumber_T)time(NULL);
}

# ប្រសិនបើ defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    static ចំនួនគត់
list2proftime(typval_T *arg, proftime_T *tm)
{
    long	n1, n2;
    ចំនួនគត់	error = FALSE;

    ប្រសិនបើ (arg->v_type != VAR_LIST || arg->vval.v_list == NULL
					     || arg->vval.v_list->lv_len != 2)
	ត្រឡប់ FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->HighPart = n1;
    tm->LowPart = n2;
#  បើមិនដូច្នេះ
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    ត្រឡប់ error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    void
f_reltime(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	start;
    long	n1, n2;

    ប្រសិនបើ (rettv_list_alloc(rettv) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	ត្រឡប់;

    ប្រសិនបើ (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ប្រសិនបើ (list2proftime(&argvars[0], &res) == FAIL)
	{
	    ប្រសិនបើ (in_vim9script())
		emsg(_(e_invalid_argument));
	    ត្រឡប់;
	}
	profile_end(&res);
    }
    បើមិនដូច្នេះ
    {
	// Two arguments: compute the difference.
	ប្រសិនបើ (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    ប្រសិនបើ (in_vim9script())
		emsg(_(e_invalid_argument));
	    ត្រឡប់;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.HighPart;
    n2 = res.LowPart;
#  បើមិនដូច្នេះ
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
    void
f_reltimefloat(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    ប្រសិនបើ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    បើមិនដូច្នេះ ប្រសិនបើ (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

/*
 * "reltimestr()" function
 */
    void
f_reltimestr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
# ifdef FEAT_RELTIME
    ប្រសិនបើ (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    proftime_T	tm;
    ប្រសិនបើ (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_msg(&tm));
#  បើមិនដូច្នេះ
	static var_na buf[50];
	long usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, parimaa_na(buf), "%3ld.%06ld", (long)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    បើមិនដូច្នេះ ប្រសិនបើ (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# ប្រសិនបើ defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    void
f_strftime(typval_T *argvars, typval_T *rettv)
{
    sa_nracanaa tm	tmval;
    sa_nracanaa tm	*curtime;
    time_t	seconds;
    char_u	*p;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    ប្រសិនបើ (argvars[1].v_type == VAR_UNKNOWN)
	seconds = time(NULL);
    បើមិនដូច្នេះ
	seconds = (time_t)tv_get_number(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ប្រសិនបើ (curtime == NULL)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	ត្រឡប់;
    }

#  ifdef MSWIN
    WCHAR	    result_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, NULL);
    ប្រសិនបើ (wp != NULL)
	(void)wcsftime(result_buf, ARRAY_LENGTH(result_buf), wp, curtime);
    បើមិនដូច្នេះ
	result_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(result_buf, NULL);
    vim_free(wp);
#  បើមិនដូច្នេះ
    char_u	    result_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    ប្រសិនបើ (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, NULL);
    ប្រសិនបើ (p == NULL || strftime((var_na *)result_buf, parimaa_na(result_buf),
		(var_na *)p, curtime) == 0)
	result_buf[0] = NUL;

    ប្រសិនបើ (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    ប្រសិនបើ (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, result_buf, NULL);
    បើមិនដូច្នេះ
	rettv->vval.v_string = vim_strsave(result_buf);

    // Release conversion descriptors
    convert_setup(&conv, NULL, NULL);
    vim_free(enc);
#  endif
}
# endif

# ប្រសិនបើ defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    void
f_strptime(typval_T *argvars, typval_T *rettv)
{
    sa_nracanaa tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    ប្រសិនបើ (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, NULL);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    ប្រសិនបើ (fmt == NULL
	    || strptime((var_na *)str, (var_na *)fmt, &tmval) == NULL
	    || (rettv->vval.v_number = mktime(&tmval)) == -1)
	rettv->vval.v_number = 0;

    ប្រសិនបើ (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, NULL, NULL);
    vim_free(enc);
}
# endif

# ប្រសិនបើ defined(FEAT_TIMERS)
static timer_T	*first_timer = NULL;
static long	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    long
proftime_time_left(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    ប្រសិនបើ (now->QuadPart > due->QuadPart)
	ត្រឡប់ 0;
    QueryPerformanceFrequency(&fr);
    ត្រឡប់ (long)(((dviguu_na)(due->QuadPart - now->QuadPart)
		   / (dviguu_na)fr.QuadPart) * 1000);
#  បើមិនដូច្នេះ
    ប្រសិនបើ (now->tv_sec > due->tv_sec)
	ត្រឡប់ 0;
    ត្រឡប់ (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    static void
insert_timer(timer_T *timer)
{
    timer->tr_next = first_timer;
    timer->tr_prev = NULL;
    ប្រសិនបើ (first_timer != NULL)
	first_timer->tr_prev = timer;
    first_timer = timer;
    did_add_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    static void
remove_timer(timer_T *timer)
{
    ប្រសិនបើ (timer->tr_prev == NULL)
	first_timer = timer->tr_next;
    បើមិនដូច្នេះ
	timer->tr_prev->tr_next = timer->tr_next;
    ប្រសិនបើ (timer->tr_next != NULL)
	timer->tr_next->tr_prev = timer->tr_prev;
}

    static void
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
create_timer(long msec, ចំនួនគត់ repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    long	prev_id = last_timer_id;

    ប្រសិនបើ (timer == NULL)
	ត្រឡប់ NULL;
    ប្រសិនបើ (++last_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    insert_timer(timer);
    ប្រសិនបើ (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    ត្រឡប់ timer;
}

/*
 * (Re)start a timer.
 */
    void
timer_start(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    static void
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    ប្រសិនបើ (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(NULL, "invoking timer callback %s",
	       cb->cb_partial != NULL ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_NUMBER;
    argv[0].vval.v_number = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(NULL, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    long
check_due_timer(void)
{
    timer_T	*timer;
    timer_T	*timer_next;
    long	this_due;
    long	next_due = -1;
    proftime_T	now;
    ចំនួនគត់		did_one = FALSE;
    ចំនួនគត់		need_update_screen = FALSE;
    long	current_id = last_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    ប្រសិនបើ (exiting || aborting() || debug_mode)
	ត្រឡប់ next_due;

    profile_start(&now);
    សម្រាប់ (timer = first_timer; timer != NULL && !got_int; timer = timer_next)
    {
	timer_next = timer->tr_next;

	ប្រសិនបើ (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    continue;
	this_due = proftime_time_left(&timer->tr_due, &now);
	ប្រសិនបើ (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    ចំនួនគត់ save_timer_busy = timer_busy;
	    ចំនួនគត់ save_vgetc_busy = vgetc_busy;
	    ចំនួនគត់ save_did_emsg = did_emsg;
	    ចំនួនគត់ prev_uncaught_emsg = uncaught_emsg;
	    ចំនួនគត់ save_called_emsg = called_emsg;
	    ចំនួនគត់ save_must_redraw = must_redraw;
	    ចំនួនគត់ save_ex_pressedreturn = get_pressedreturn();
	    ចំនួនគត់ save_may_garbage_collect = may_garbage_collect;
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
	    ប្រសិនបើ (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_count;
	    បើមិនដូច្នេះ
		timer->tr_emsg_count = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    ប្រសិនបើ (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    ប្រសិនបើ (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_count < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_left(&timer->tr_due, &now);
		ប្រសិនបើ (this_due < 1)
		    this_due = 1;
		ប្រសិនបើ (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    បើមិនដូច្នេះ
	    {
		this_due = -1;
		ប្រសិនបើ (timer->tr_keep)
		    timer->tr_paused = TRUE;
		បើមិនដូច្នេះ
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
	ប្រសិនបើ (this_due > 0 && (next_due == -1 || next_due > this_due))
	    next_due = this_due;
    }

    ប្រសិនបើ (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    ប្រសិនបើ (bevalexpr_due_set)
    {
	this_due = proftime_time_left(&bevalexpr_due, &now);
	ប្រសិនបើ (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    ប្រសិនបើ (balloonEval == NULL)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    ប្រសិនបើ (balloonEval != NULL)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	បើមិនដូច្នេះ ប្រសិនបើ (next_due == -1 || next_due > this_due)
	    next_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    next_due = term_check_timers(next_due, &now);
#  endif

    ត្រឡប់ current_id != last_timer_id ? 1 : next_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    static timer_T *
find_timer(long id)
{
    timer_T *timer;

    ប្រសិនបើ (id < 0)
	ត្រឡប់ NULL;

    FOR_ALL_TIMERS(timer)
	ប្រសិនបើ (timer->tr_id == id)
	    ត្រឡប់ timer;
    ត្រឡប់ NULL;
}


/*
 * Stop a timer and delete it.
 */
    void
stop_timer(timer_T *timer)
{
    ប្រសិនបើ (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    បើមិនដូច្នេះ
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    static void
stop_all_timers(void)
{
    timer_T *timer;
    timer_T *timer_next;

    សម្រាប់ (timer = first_timer; timer != NULL; timer = timer_next)
    {
	timer_next = timer->tr_next;
	stop_timer(timer);
    }
}

    static void
add_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    long	remaining;
    proftime_T	now;

    ប្រសិនបើ (dict == NULL)
	ត្រឡប់;
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
    ប្រសិនបើ (di != NULL)
    {
	ប្រសិនបើ (dict_add(dict, di) == FAIL)
	    vim_free(di);
	បើមិនដូច្នេះ
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    static void
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	ប្រសិនបើ (timer->tr_id != -1)
	    add_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    ចំនួនគត់
set_ref_in_timer(ចំនួនគត់ copyID)
{
    ចំនួនគត់		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    សម្រាប់ (timer = first_timer; !abort && timer != NULL; timer = timer->tr_next)
    {
	ប្រសិនបើ (timer->tr_callback.cb_partial != NULL)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	បើមិនដូច្នេះ
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    }
    ត្រឡប់ abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    ចំនួនគត់
timer_valid(timer_T *timer)
{
    ប្រសិនបើ (timer == NULL)
	ត្រឡប់ FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	ប្រសិនបើ (t == timer)
	    ត្រឡប់ TRUE;
    ត្រឡប់ FALSE;
}

#  ប្រសិនបើ defined(EXITFREE)
    void
timer_free_all(void)
{
    ខណៈពេល (first_timer != NULL)
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
    void
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = NULL;

    ប្រសិនបើ (rettv_list_alloc(rettv) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (check_for_opt_number_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((ចំនួនគត់)tv_get_number(&argvars[0]));
	ប្រសិនបើ (timer != NULL)
	    add_timer_info(rettv, timer);
    }
    បើមិនដូច្នេះ
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    void
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = NULL;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    ប្រសិនបើ (argvars[0].v_type != VAR_NUMBER)
    {
	emsg(_(e_number_expected));
	ត្រឡប់;
    }

    ចំនួនគត់	paused = (ចំនួនគត់)tv_get_bool(&argvars[1]);

    timer = find_timer((ចំនួនគត់)tv_get_number(&argvars[0]));
    ប្រសិនបើ (timer != NULL)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    void
f_timer_start(typval_T *argvars, typval_T *rettv)
{
    long	msec;
    timer_T	*timer;
    ចំនួនគត់		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_number = -1;
    ប្រសិនបើ (check_secure())
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ត្រឡប់;

    msec = (long)tv_get_number(&argvars[0]);
    ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ប្រសិនបើ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ត្រឡប់;

	dict = argvars[2].vval.v_dict;
	ប្រសិនបើ (dict_has_key(dict, "repeat"))
	    repeat = dict_get_number(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    ប្រសិនបើ (callback.cb_name == NULL)
	ត្រឡប់;
    ប្រសិនបើ (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	ត្រឡប់;
    }

    timer = create_timer(msec, repeat);
    ប្រសិនបើ (timer == NULL)
    {
	free_callback(&callback);
	ត្រឡប់;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_number = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    void
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    ប្រសិនបើ (check_for_number_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    timer = find_timer((ចំនួនគត់)tv_get_number(&argvars[0]));
    ប្រសិនបើ (timer != NULL)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    void
f_timer_stopall(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# ប្រសិនបើ defined(STARTUPTIME)
static sa_nracanaa timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    static ចំនួនគត់
gettimeofday(sa_nracanaa timeval *tv, var_na *dummy UNUSED)
{
    long t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    ត្រឡប់ 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    void
time_push(void *tv_rel, void *tv_start)
{
    *((sa_nracanaa timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, NULL);
    ((sa_nracanaa timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((sa_nracanaa timeval *)tv_rel)->tv_usec;
    ((sa_nracanaa timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((sa_nracanaa timeval *)tv_rel)->tv_sec;
    ប្រសិនបើ (((sa_nracanaa timeval *)tv_rel)->tv_usec < 0)
    {
	((sa_nracanaa timeval *)tv_rel)->tv_usec += 1000000;
	--((sa_nracanaa timeval *)tv_rel)->tv_sec;
    }
    *(sa_nracanaa timeval *)tv_start = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    void
time_pop(
    void	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((sa_nracanaa timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((sa_nracanaa timeval *)tp)->tv_sec;
    ប្រសិនបើ (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    static void
time_diff(sa_nracanaa timeval *then, sa_nracanaa timeval *now)
{
    long	usec;
    long	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(time_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    void
time_msg(
    var_na	*mesg,
    void	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    static sa_nracanaa timeval	start;
    sa_nracanaa timeval		now;

    ប្រសិនបើ (time_fd == NULL)
	ត្រឡប់;

    ប្រសិនបើ (strstr(mesg, "STARTING") != NULL)
    {
	gettimeofday(&start, NULL);
	prev_timeval = start;
	fprintf(time_fd, "\n\ntimes in msec\n");
	fprintf(time_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, NULL);
    time_diff(&start, &now);
    ប្រសិនបើ (((sa_nracanaa timeval *)tv_start) != NULL)
    {
	fprintf(time_fd, "  ");
	time_diff(((sa_nracanaa timeval *)tv_start), &now);
    }
    fprintf(time_fd, "  ");
    time_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(time_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#endif // FEAT_EVAL

#ប្រសិនបើ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    time_T
get8ctime(FILE *fd)
{
    ចំនួនគត់		c;
    time_T	n = 0;
    ចំនួនគត់		i;

    សម្រាប់ (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	ប្រសិនបើ (c == EOF) ត្រឡប់ -1;
	n = (n << 8) + c;
    }
    ត្រឡប់ n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    ចំនួនគត់
put_time(FILE *fd, time_T the_time)
{
    char_u	buf[8];

    time_to_bytes(the_time, buf);
    ត្រឡប់ fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    void
time_to_bytes(time_T the_time, char_u *buf)
{
    ចំនួនគត់		c;
    ចំនួនគត់		i;
    ចំនួនគត់		bi = 0;
    time_T	wtime = the_time;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    សម្រាប់ (i = 7; i >= 0; --i)
    {
	ប្រសិនបើ (i + 1 > (ចំនួនគត់)parimaa_na(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	បើមិនដូច្នេះ
	{
# ប្រសិនបើ defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (ចំនួនគត់)(wtime >> (i * 8));
# បើមិនដូច្នេះ
	    c = (ចំនួនគត់)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#endif

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    void
add_time(char_u *buf, size_t buflen, time_t tt)
{
#ifdef HAVE_STRFTIME
    sa_nracanaa tm	tmval;
    sa_nracanaa tm	*curtime;
    size_t	n;

    ប្រសិនបើ (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	ប្រសិនបើ (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((var_na *)buf, buflen, "%H:%M:%S", curtime);
	បើមិនដូច្នេះ
	    // longer ago
	    n = strftime((var_na *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	ប្រសិនបើ (n == 0)
	    buf[0] = NUL;
    }
    បើមិនដូច្នេះ
#endif
    {
	long seconds = (long)(vim_time() - tt);

	vim_snprintf((var_na *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
