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

#ଅନ୍ତର୍ଭାଵ "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#ଯଦି defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
ସ୍ଥିର ଅକ୍ଷର	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    ପାଇଁ ((t) = first_timer; (t) != ଖାଲି; (t) = (t)->tr_next)

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
    ସ୍ଥିର ସଂରଚନା tm *
vim_localtime(
    ଅଚଳ time_t	*timep,		// timestamp for local representation
    ସଂରଚନା tm		*result UNUSED)	// pointer to caller return buffer
{
#ifdef HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    ଅକ୍ଷର		*tz;		// pointer for TZ environment var

    tz = (ଅକ୍ଷର *)mch_getenv((char_u *)"TZ");
    ଯଦି (tz == ଖାଲି)
	tz = "";
    ଯଦି (STRNCMP(tz_cache, tz, ଆକାର(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, ଆକାର(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    ଫେରନ୍ତୁ localtime_r(timep, result);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ localtime(timep);
#endif	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    time_T
vim_time(ଶୂନ୍ୟ)
{
#ifdef FEAT_EVAL
    ଫେରନ୍ତୁ time_for_testing == 0 ? time(ଖାଲି) : time_for_testing;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ time(ଖାଲି);
#endif
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    ଅକ୍ଷର *
get_ctime(time_t thetime, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_newline)
{
    ସ୍ଥିର ଅକ୍ଷର buf[100];  // hopefully enough for every language
#ifdef HAVE_STRFTIME
    ସଂରଚନା tm	tmval;
    ସଂରଚନା tm	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ଯଦି (curtime == ଖାଲି)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), ଆକାର(buf) - 2);
    ଅନ୍ୟଥା
    {
	// xgettext:no-c-format
	ଯଦି (strftime(buf, ଆକାର(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      ଆକାର(buf) - 2);
	}
# ifdef MSWIN
	ଯଦି (enc_codepage >= 0 && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GetACP() != enc_codepage)
	{
	    char_u	*to_free = ଖାଲି;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

	    acp_to_enc((char_u *)buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(buf), &to_free, &len);
	    ଯଦି (to_free != ଖାଲି)
	    {
		STRNCPY(buf, to_free, ଆକାର(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#ଅନ୍ୟଥା
    STRCPY(buf, "(unknown)");
#endif
    ଯଦି (add_newline)
	STRCAT(buf, "\n");
    ଫେରନ୍ତୁ buf;
}

#ଯଦି defined(FEAT_EVAL)

# ଯଦି defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    ଶୂନ୍ୟ
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)time(ଖାଲି);
}

# ଯଦି defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list2proftime(typval_T *arg, proftime_T *tm)
{
    long	n1, n2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	error = FALSE;

    ଯଦି (arg->v_type != VAR_LIST || arg->vval.v_list == ଖାଲି
					     || arg->vval.v_list->lv_len != 2)
	ଫେରନ୍ତୁ FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    tm->ଉପର_Part = n1;
    tm->ତଳ_Part = n2;
#  ଅନ୍ୟଥା
    tm->tv_sec = n1;
    tm->tv_fsec = n2;
#  endif
    ଫେରନ୍ତୁ error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    ଶୂନ୍ୟ
f_reltime(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	start;
    long	n1, n2;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
    {
	ଯଦି (list2proftime(&argvars[0], &res) == FAIL)
	{
	    ଯଦି (in_vim9script())
		emsg(_(e_invalid_argument));
	    ଫେରନ୍ତୁ;
	}
	profile_end(&res);
    }
    ଅନ୍ୟଥା
    {
	// Two arguments: compute the difference.
	ଯଦି (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    ଯଦି (in_vim9script())
		emsg(_(e_invalid_argument));
	    ଫେରନ୍ତୁ;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.ଉପର_Part;
    n2 = res.ତଳ_Part;
#  ଅନ୍ୟଥା
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)n1);
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    ଶୂନ୍ୟ
f_reltimefloat(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	tm;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    ଯଦି (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (list2proftime(&argvars[0], &tm) == OK)
	rettv->vval.v_float = profile_float(&tm);
    ଅନ୍ୟଥା ଯଦି (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

/*
 * "reltimestr()" function
 */
    ଶୂନ୍ୟ
f_reltimestr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;
# ifdef FEAT_RELTIME
    ଯଦି (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    proftime_T	tm;
    ଯଦି (list2proftime(&argvars[0], &tm) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_ସନ୍ଦେଶ(&tm));
#  ଅନ୍ୟଥା
	ସ୍ଥିର ଅକ୍ଷର buf[50];
	long usec = tm.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, ଆକାର(buf), "%3ld.%06ld", (long)tm.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    ଅନ୍ୟଥା ଯଦି (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# ଯଦି defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    ଶୂନ୍ୟ
f_strftime(typval_T *argvars, typval_T *rettv)
{
    ସଂରଚନା tm	tmval;
    ସଂରଚନା tm	*curtime;
    time_t	seconds;
    char_u	*p;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
	seconds = time(ଖାଲି);
    ଅନ୍ୟଥା
	seconds = (time_t)tv_get_ସଂଖ୍ୟା(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    ଯଦି (curtime == ଖାଲି)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	ଫେରନ୍ତୁ;
    }

#  ifdef MSWIN
    WCHAR	    result_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, ଖାଲି);
    ଯଦି (wp != ଖାଲି)
	(ଶୂନ୍ୟ)wcsftime(result_buf, ARRAY_LENGTH(result_buf), wp, curtime);
    ଅନ୍ୟଥା
	result_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(result_buf, ଖାଲି);
    vim_free(wp);
#  ଅନ୍ୟଥା
    char_u	    result_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    ଯଦି (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, ଖାଲି);
    ଯଦି (p == ଖାଲି || strftime((ଅକ୍ଷର *)result_buf, ଆକାର(result_buf),
		(ଅକ୍ଷର *)p, curtime) == 0)
	result_buf[0] = NUL;

    ଯଦି (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    ଯଦି (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, result_buf, ଖାଲି);
    ଅନ୍ୟଥା
	rettv->vval.v_string = vim_strsave(result_buf);

    // Release conversion descriptors
    convert_setup(&conv, ଖାଲି, ଖାଲି);
    vim_free(enc);
#  endif
}
# endif

# ଯଦି defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    ଶୂନ୍ୟ
f_strptime(typval_T *argvars, typval_T *rettv)
{
    ସଂରଚନା tm	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    ଯଦି (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, ଖାଲି);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    ଯଦି (fmt == ଖାଲି
	    || strptime((ଅକ୍ଷର *)str, (ଅକ୍ଷର *)fmt, &tmval) == ଖାଲି
	    || (rettv->vval.v_ସଂଖ୍ୟା = mktime(&tmval)) == -1)
	rettv->vval.v_ସଂଖ୍ୟା = 0;

    ଯଦି (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, ଖାଲି, ଖାଲି);
    vim_free(enc);
}
# endif

# ଯଦି defined(FEAT_TIMERS)
ସ୍ଥିର timer_T	*first_timer = ଖାଲି;
ସ୍ଥିର long	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    long
proftime_time_ବାମ(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    ଯଦି (now->QuadPart > due->QuadPart)
	ଫେରନ୍ତୁ 0;
    QueryPerformanceFrequency(&fr);
    ଫେରନ୍ତୁ (long)(((ଦ୍ୱିଗୁଣ)(due->QuadPart - now->QuadPart)
		   / (ଦ୍ୱିଗୁଣ)fr.QuadPart) * 1000);
#  ଅନ୍ୟଥା
    ଯଦି (now->tv_sec > due->tv_sec)
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ଭର୍ତ୍ତି_timer(timer_T *timer)
{
    timer->tr_next = first_timer;
    timer->tr_prev = ଖାଲି;
    ଯଦି (first_timer != ଖାଲି)
	first_timer->tr_prev = timer;
    first_timer = timer;
    did_add_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
remove_timer(timer_T *timer)
{
    ଯଦି (timer->tr_prev == ଖାଲି)
	first_timer = timer->tr_next;
    ଅନ୍ୟଥା
	timer->tr_prev->tr_next = timer->tr_next;
    ଯଦି (timer->tr_next != ଖାଲି)
	timer->tr_next->tr_prev = timer->tr_prev;
}

    ସ୍ଥିର ଶୂନ୍ୟ
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
create_timer(long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    long	prev_id = last_timer_id;

    ଯଦି (timer == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (++last_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    ଭର୍ତ୍ତି_timer(timer);
    ଯଦି (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    ଫେରନ୍ତୁ timer;
}

/*
 * (Re)start a timer.
 */
    ଶୂନ୍ୟ
timer_start(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    ଯଦି (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(ଖାଲି, "invoking timer callback %s",
	       cb->cb_partial != ଖାଲି ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_ସଂଖ୍ୟା;
    argv[0].vval.v_ସଂଖ୍ୟା = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(ଖାଲି, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    long
check_due_timer(ଶୂନ୍ୟ)
{
    timer_T	*timer;
    timer_T	*timer_next;
    long	this_due;
    long	next_due = -1;
    proftime_T	now;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_one = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_update_screen = FALSE;
    long	current_id = last_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    ଯଦି (exiting || aborting() || debug_mode)
	ଫେରନ୍ତୁ next_due;

    profile_start(&now);
    ପାଇଁ (timer = first_timer; timer != ଖାଲି && !got_int; timer = timer_next)
    {
	timer_next = timer->tr_next;

	ଯଦି (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    ଜାରି;
	this_due = proftime_time_ବାମ(&timer->tr_due, &now);
	ଯଦି (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_timer_busy = timer_busy;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_vgetc_busy = vgetc_busy;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_did_emsg = did_emsg;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_uncaught_emsg = uncaught_emsg;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_called_emsg = called_emsg;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_must_redraw = must_redraw;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ex_pressedreturn = get_pressedreturn();
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_may_garbage_collect = may_garbage_collect;
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
	    ଯଦି (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_ଗଣନା;
	    ଅନ୍ୟଥା
		timer->tr_emsg_ଗଣନା = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    ଯଦି (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    ଯଦି (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_ଗଣନା < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_ବାମ(&timer->tr_due, &now);
		ଯଦି (this_due < 1)
		    this_due = 1;
		ଯଦି (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    ଅନ୍ୟଥା
	    {
		this_due = -1;
		ଯଦି (timer->tr_keep)
		    timer->tr_paused = TRUE;
		ଅନ୍ୟଥା
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
	ଯଦି (this_due > 0 && (next_due == -1 || next_due > this_due))
	    next_due = this_due;
    }

    ଯଦି (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    ଯଦି (bevalexpr_due_set)
    {
	this_due = proftime_time_ବାମ(&bevalexpr_due, &now);
	ଯଦି (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    ଯଦି (balloonEval == ଖାଲି)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    ଯଦି (balloonEval != ଖାଲି)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	ଅନ୍ୟଥା ଯଦି (next_due == -1 || next_due > this_due)
	    next_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    next_due = term_check_timers(next_due, &now);
#  endif

    ଫେରନ୍ତୁ current_id != last_timer_id ? 1 : next_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    ସ୍ଥିର timer_T *
find_timer(long id)
{
    timer_T *timer;

    ଯଦି (id < 0)
	ଫେରନ୍ତୁ ଖାଲି;

    FOR_ALL_TIMERS(timer)
	ଯଦି (timer->tr_id == id)
	    ଫେରନ୍ତୁ timer;
    ଫେରନ୍ତୁ ଖାଲି;
}


/*
 * Stop a timer and delete it.
 */
    ଶୂନ୍ୟ
stop_timer(timer_T *timer)
{
    ଯଦି (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    ଅନ୍ୟଥା
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
stop_all_timers(ଶୂନ୍ୟ)
{
    timer_T *timer;
    timer_T *timer_next;

    ପାଇଁ (timer = first_timer; timer != ଖାଲି; timer = timer_next)
    {
	timer_next = timer->tr_next;
	stop_timer(timer);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
add_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    long	remaining;
    proftime_T	now;

    ଯଦି (dict == ଖାଲି)
	ଫେରନ୍ତୁ;
    list_append_dict(list, dict);

    dict_add_ସଂଖ୍ୟା(dict, "id", timer->tr_id);
    dict_add_ସଂଖ୍ୟା(dict, "time", (long)timer->tr_interval);

    profile_start(&now);
    remaining = proftime_time_ବାମ(&timer->tr_due, &now);
    dict_add_ସଂଖ୍ୟା(dict, "remaining", (long)remaining);

    dict_add_ସଂଖ୍ୟା(dict, "repeat",
	    (long)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_add_ସଂଖ୍ୟା(dict, "paused", (long)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    ଯଦି (di != ଖାଲି)
    {
	ଯଦି (dict_add(dict, di) == FAIL)
	    vim_free(di);
	ଅନ୍ୟଥା
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	ଯଦି (timer->tr_id != -1)
	    add_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_timer(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;
    timer_T	*timer;
    typval_T	tv;

    ପାଇଁ (timer = first_timer; !abort && timer != ଖାଲି; timer = timer->tr_next)
    {
	ଯଦି (timer->tr_callback.cb_partial != ଖାଲି)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	ଅନ୍ୟଥା
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	abort = abort || set_ref_in_item(&tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
    }
    ଫେରନ୍ତୁ abort;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
timer_valid(timer_T *timer)
{
    ଯଦି (timer == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	ଯଦି (t == timer)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

#  ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
timer_free_all(ଶୂନ୍ୟ)
{
    ଯେତେବେଳେ (first_timer != ଖାଲି)
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
    ଶୂନ୍ୟ
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = ଖାଲି;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]));
	ଯଦି (timer != ଖାଲି)
	    add_timer_info(rettv, timer);
    }
    ଅନ୍ୟଥା
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    ଶୂନ୍ୟ
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_ସଂଖ୍ୟା)
    {
	emsg(_(e_ସଂଖ୍ୟା_expected));
	ଫେରନ୍ତୁ;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	paused = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool(&argvars[1]);

    timer = find_timer((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]));
    ଯଦି (timer != ଖାଲି)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    ଶୂନ୍ୟ
f_timer_start(typval_T *argvars, typval_T *rettv)
{
    long	msec;
    timer_T	*timer;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    msec = (long)tv_get_ସଂଖ୍ୟା(&argvars[0]);
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	ଯଦି (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ଫେରନ୍ତୁ;

	dict = argvars[2].vval.v_dict;
	ଯଦି (dict_has_ଚାବି(dict, "repeat"))
	    repeat = dict_get_ସଂଖ୍ୟା(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    ଯଦି (callback.cb_name == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	ଫେରନ୍ତୁ;
    }

    timer = create_timer(msec, repeat);
    ଯଦି (timer == ଖାଲି)
    {
	free_callback(&callback);
	ଫେରନ୍ତୁ;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    ଶୂନ୍ୟ
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    ଯଦି (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    timer = find_timer((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]));
    ଯଦି (timer != ଖାଲି)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    ଶୂନ୍ୟ
f_timer_stopall(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# ଯଦି defined(STARTUPTIME)
ସ୍ଥିର ସଂରଚନା timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gettimeofday(ସଂରଚନା timeval *tv, ଅକ୍ଷର *dummy UNUSED)
{
    long t = clock();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    ଫେରନ୍ତୁ 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    ଶୂନ୍ୟ
time_ଠେଲ(ଶୂନ୍ୟ *tv_rel, ଶୂନ୍ୟ *tv_start)
{
    *((ସଂରଚନା timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, ଖାଲି);
    ((ସଂରଚନା timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((ସଂରଚନା timeval *)tv_rel)->tv_usec;
    ((ସଂରଚନା timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((ସଂରଚନା timeval *)tv_rel)->tv_sec;
    ଯଦି (((ସଂରଚନା timeval *)tv_rel)->tv_usec < 0)
    {
	((ସଂରଚନା timeval *)tv_rel)->tv_usec += 1000000;
	--((ସଂରଚନା timeval *)tv_rel)->tv_sec;
    }
    *(ସଂରଚନା timeval *)tv_start = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    ଶୂନ୍ୟ
time_କାଢ଼(
    ଶୂନ୍ୟ	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((ସଂରଚନା timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((ସଂରଚନା timeval *)tp)->tv_sec;
    ଯଦି (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
time_diff(ସଂରଚନା timeval *then, ସଂରଚନା timeval *now)
{
    long	usec;
    long	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    fprintf(time_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    ଶୂନ୍ୟ
time_ସନ୍ଦେଶ(
    ଅକ୍ଷର	*mesg,
    ଶୂନ୍ୟ	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    ସ୍ଥିର ସଂରଚନା timeval	start;
    ସଂରଚନା timeval		now;

    ଯଦି (time_fd == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (strstr(mesg, "STARTING") != ଖାଲି)
    {
	gettimeofday(&start, ଖାଲି);
	prev_timeval = start;
	fprintf(time_fd, "\n\ntimes in msec\n");
	fprintf(time_fd, " clock   self+sourced   self:  sourced script\n");
	fprintf(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, ଖାଲି);
    time_diff(&start, &now);
    ଯଦି (((ସଂରଚନା timeval *)tv_start) != ଖାଲି)
    {
	fprintf(time_fd, "  ");
	time_diff(((ସଂରଚନା timeval *)tv_start), &now);
    }
    fprintf(time_fd, "  ");
    time_diff(&prev_timeval, &now);
    prev_timeval = now;
    fprintf(time_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#endif // FEAT_EVAL

#ଯଦି defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    time_T
get8ctime(FILE *fd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    time_T	n = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < 8; ++i)
    {
	c = getc(fd);
	ଯଦି (c == EOF) ଫେରନ୍ତୁ -1;
	n = (n << 8) + c;
    }
    ଫେରନ୍ତୁ n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_time(FILE *fd, time_T the_time)
{
    char_u	buf[8];

    time_to_bytes(the_time, buf);
    ଫେରନ୍ତୁ fwrite(buf, (size_t)8, (size_t)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    ଶୂନ୍ୟ
time_to_bytes(time_T the_time, char_u *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bi = 0;
    time_T	wtime = the_time;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    ପାଇଁ (i = 7; i >= 0; --i)
    {
	ଯଦି (i + 1 > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	ଅନ୍ୟଥା
	{
# ଯଦି defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(wtime >> (i * 8));
# ଅନ୍ୟଥା
	    c = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#endif

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    ଶୂନ୍ୟ
add_time(char_u *buf, size_t buflen, time_t tt)
{
#ifdef HAVE_STRFTIME
    ସଂରଚନା tm	tmval;
    ସଂରଚନା tm	*curtime;
    size_t	n;

    ଯଦି (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	ଯଦି (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = strftime((ଅକ୍ଷର *)buf, buflen, "%H:%M:%S", curtime);
	ଅନ୍ୟଥା
	    // longer ago
	    n = strftime((ଅକ୍ଷର *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	ଯଦି (n == 0)
	    buf[0] = NUL;
    }
    ଅନ୍ୟଥା
#endif
    {
	long seconds = (long)(vim_time() - tt);

	vim_snprintf((ଅକ୍ଷର *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
