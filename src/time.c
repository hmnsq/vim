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

#अन्तर्भाव "vim.h"

/*
 * Cache of the current timezone name as retrieved from TZ, or an empty string
 * where unset, up to 64 octets long including trailing null byte.
 */
#जर defined(HAVE_LOCALTIME_R) && defined(HAVE_TZSET)
स्थिर अक्षर	tz_cache[64];
#endif

#define FOR_ALL_TIMERS(t) \
    साठी ((t) = first_timer; (t) != रिक्त; (t) = (t)->tr_next)

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
    स्थिर संरचना पंचांग *
vim_localtime(
    स्थिरांक समय_प्रकार	*timep,		// timestamp for local representation
    संरचना पंचांग		*result UNUSED)	// pointer to caller return buffer
{
#ifdef HAVE_LOCALTIME_R
# ifdef HAVE_TZSET
    अक्षर		*tz;		// pointer for TZ environment var

    tz = (अक्षर *)mch_getenv((char_u *)"TZ");
    जर (tz == रिक्त)
	tz = "";
    जर (STRNCMP(tz_cache, tz, आकार(tz_cache) - 1) != 0)
    {
	tzset();
	vim_strncpy((char_u *)tz_cache, (char_u *)tz, आकार(tz_cache) - 1);
    }
# endif	// HAVE_TZSET
    परत localtime_r(timep, result);
#अन्यथा
    परत स_स्थानीय(timep);
#endif	// HAVE_LOCALTIME_R
}

/*
 * Return the current time in seconds.  Calls time(), unless test_settime()
 * was used.
 */
    time_T
vim_time(शून्य)
{
#ifdef FEAT_EVAL
    परत time_for_testing == 0 ? समय(रिक्त) : time_for_testing;
#अन्यथा
    परत समय(रिक्त);
#endif
}

/*
 * Replacement for ctime(), which is not safe to use.
 * Requires strftime(), otherwise returns "(unknown)".
 * If "thetime" is invalid returns "(invalid)".  Never returns NULL.
 * When "add_newline" is TRUE add a newline like ctime() does.
 * Uses a static buffer.
 */
    अक्षर *
get_ctime(समय_प्रकार thetime, पूर्णांक add_newline)
{
    स्थिर अक्षर buf[100];  // hopefully enough for every language
#ifdef HAVE_STRFTIME
    संरचना पंचांग	tmval;
    संरचना पंचांग	*curtime;

    curtime = vim_localtime(&thetime, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    जर (curtime == रिक्त)
	vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"), आकार(buf) - 2);
    अन्यथा
    {
	// xgettext:no-c-format
	जर (स_माला(buf, आकार(buf) - 2, _("%a %b %d %H:%M:%S %Y"), curtime)
									  == 0)
	{
	    // Quoting "man strftime":
	    // > If the length of the result string (including the terminating
	    // > null byte) would exceed max bytes, then strftime() returns 0,
	    // > and the contents of the array are undefined.
	    vim_strncpy((char_u *)buf, (char_u *)_("(Invalid)"),
							      आकार(buf) - 2);
	}
# ifdef MSWIN
	जर (enc_codepage >= 0 && (पूर्णांक)GetACP() != enc_codepage)
	{
	    char_u	*to_free = रिक्त;
	    पूर्णांक		len;

	    acp_to_enc((char_u *)buf, (पूर्णांक)म_माप(buf), &to_free, &len);
	    जर (to_free != रिक्त)
	    {
		STRNCPY(buf, to_free, आकार(buf) - 2);
		vim_free(to_free);
	    }
	}
# endif
    }
#अन्यथा
    STRCPY(buf, "(unknown)");
#endif
    जर (add_newline)
	STRCAT(buf, "\n");
    परत buf;
}

#जर defined(FEAT_EVAL)

# जर defined(MACOS_X)
#  include <time.h>	// for time_t
# endif

/*
 * "localtime()" function
 */
    शून्य
f_localtime(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_संख्या = (varnumber_T)समय(रिक्त);
}

# जर defined(FEAT_RELTIME)
/*
 * Convert a List to proftime_T.
 * Return FAIL when there is something wrong.
 */
    स्थिर पूर्णांक
list2proftime(typval_T *arg, proftime_T *पंचांग)
{
    दीर्घ	n1, n2;
    पूर्णांक	error = FALSE;

    जर (arg->v_type != VAR_LIST || arg->vval.v_list == रिक्त
					     || arg->vval.v_list->lv_len != 2)
	परत FAIL;
    n1 = list_find_nr(arg->vval.v_list, 0L, &error);
    n2 = list_find_nr(arg->vval.v_list, 1L, &error);
#  ifdef MSWIN
    पंचांग->वरचा_Part = n1;
    पंचांग->खालचा_Part = n2;
#  अन्यथा
    पंचांग->tv_sec = n1;
    पंचांग->tv_fsec = n2;
#  endif
    परत error ? FAIL : OK;
}
# endif // FEAT_RELTIME

/*
 * "reltime()" function
 */
    शून्य
f_reltime(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_RELTIME
    proftime_T	res;
    proftime_T	start;
    दीर्घ	n1, n2;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script()
	    && (check_for_opt_list_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_list_arg(argvars, 1) == FAIL)))
	परत;

    जर (argvars[0].v_type == VAR_UNKNOWN)
    {
	// No arguments: get current time.
	profile_start(&res);
    }
    अन्यथा जर (argvars[1].v_type == VAR_UNKNOWN)
    {
	जर (list2proftime(&argvars[0], &res) == FAIL)
	{
	    जर (in_vim9script())
		emsg(_(e_invalid_argument));
	    परत;
	}
	profile_end(&res);
    }
    अन्यथा
    {
	// Two arguments: compute the difference.
	जर (list2proftime(&argvars[0], &start) == FAIL
		|| list2proftime(&argvars[1], &res) == FAIL)
	{
	    जर (in_vim9script())
		emsg(_(e_invalid_argument));
	    परत;
	}
	profile_sub(&res, &start);
    }

#  ifdef MSWIN
    n1 = res.वरचा_Part;
    n2 = res.खालचा_Part;
#  अन्यथा
    n1 = res.tv_sec;
    n2 = res.tv_fsec;
#  endif
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)n1);
    list_append_संख्या(rettv->vval.v_list, (varnumber_T)n2);
# endif
}

/*
 * "reltimefloat()" function
 */
    शून्य
f_reltimefloat(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_RELTIME
    proftime_T	पंचांग;
# endif

    rettv->v_type = VAR_FLOAT;
    rettv->vval.v_float = 0;
# ifdef FEAT_RELTIME
    जर (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	परत;

    जर (list2proftime(&argvars[0], &पंचांग) == OK)
	rettv->vval.v_float = profile_float(&पंचांग);
    अन्यथा जर (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

/*
 * "reltimestr()" function
 */
    शून्य
f_reltimestr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
# ifdef FEAT_RELTIME
    जर (in_vim9script() && check_for_list_arg(argvars, 0) == FAIL)
	परत;

    proftime_T	पंचांग;
    जर (list2proftime(&argvars[0], &पंचांग) == OK)
    {
#  ifdef MSWIN
	rettv->vval.v_string = vim_strsave((char_u *)profile_संदेश(&पंचांग));
#  अन्यथा
	स्थिर अक्षर buf[50];
	दीर्घ usec = पंचांग.tv_fsec / (TV_FSEC_SEC / 1000000);
	vim_snprintf(buf, आकार(buf), "%3ld.%06ld", (दीर्घ)पंचांग.tv_sec, usec);
	rettv->vval.v_string = vim_strsave((char_u *)buf);
#  endif
    }
    अन्यथा जर (in_vim9script())
	emsg(_(e_invalid_argument));
# endif
}

# जर defined(HAVE_STRFTIME)
/*
 * "strftime({format}[, {time}])" function
 */
    शून्य
f_strftime(typval_T *argvars, typval_T *rettv)
{
    संरचना पंचांग	tmval;
    संरचना पंचांग	*curtime;
    समय_प्रकार	seconds;
    char_u	*p;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	परत;

    rettv->v_type = VAR_STRING;

    p = tv_get_string(&argvars[0]);
    जर (argvars[1].v_type == VAR_UNKNOWN)
	seconds = समय(रिक्त);
    अन्यथा
	seconds = (समय_प्रकार)tv_get_संख्या(&argvars[1]);
    curtime = vim_localtime(&seconds, &tmval);
    // MSVC returns NULL for an invalid value of seconds.
    जर (curtime == रिक्त)
    {
	rettv->vval.v_string = vim_strsave((char_u *)_("(Invalid)"));
	परत;
    }

#  ifdef MSWIN
    WCHAR	    result_buf[256];
    WCHAR	    *wp;

    wp = enc_to_utf16(p, रिक्त);
    जर (wp != रिक्त)
	(शून्य)wcsftime(result_buf, ARRAY_LENGTH(result_buf), wp, curtime);
    अन्यथा
	result_buf[0] = NUL;
    rettv->vval.v_string = utf16_to_enc(result_buf, रिक्त);
    vim_free(wp);
#  अन्यथा
    char_u	    result_buf[256];
    vimconv_T   conv;
    char_u	    *enc;

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    जर (conv.vc_type != CONV_NONE)
	p = string_convert(&conv, p, रिक्त);
    जर (p == रिक्त || स_माला((अक्षर *)result_buf, आकार(result_buf),
		(अक्षर *)p, curtime) == 0)
	result_buf[0] = NUL;

    जर (conv.vc_type != CONV_NONE)
	vim_free(p);
    convert_setup(&conv, enc, p_enc);
    जर (conv.vc_type != CONV_NONE)
	rettv->vval.v_string = string_convert(&conv, result_buf, रिक्त);
    अन्यथा
	rettv->vval.v_string = vim_strsave(result_buf);

    // Release conversion descriptors
    convert_setup(&conv, रिक्त, रिक्त);
    vim_free(enc);
#  endif
}
# endif

# जर defined(HAVE_STRPTIME)
/*
 * "strptime({format}, {timestring})" function
 */
    शून्य
f_strptime(typval_T *argvars, typval_T *rettv)
{
    संरचना पंचांग	tmval;
    char_u	*fmt;
    char_u	*str;
    vimconv_T   conv;
    char_u	*enc;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    CLEAR_FIELD(tmval);
    tmval.tm_isdst = -1;
    fmt = tv_get_string(&argvars[0]);
    str = tv_get_string(&argvars[1]);

    conv.vc_type = CONV_NONE;
    enc = enc_locale();
    convert_setup(&conv, p_enc, enc);
    जर (conv.vc_type != CONV_NONE)
	fmt = string_convert(&conv, fmt, रिक्त);
#  ifdef HAVE_TZSET
    // Pick up any change to $TZ so mktime() uses the requested timezone.
    tzset();
#  endif
    जर (fmt == रिक्त
	    || strptime((अक्षर *)str, (अक्षर *)fmt, &tmval) == रिक्त
	    || (rettv->vval.v_संख्या = स_गढ़ो(&tmval)) == -1)
	rettv->vval.v_संख्या = 0;

    जर (conv.vc_type != CONV_NONE)
	vim_free(fmt);
    convert_setup(&conv, रिक्त, रिक्त);
    vim_free(enc);
}
# endif

# जर defined(FEAT_TIMERS)
स्थिर timer_T	*first_timer = रिक्त;
स्थिर दीर्घ	last_timer_id = 0;

/*
 * Return time left, in "msec", until "due".  Negative if past "due".
 */
    दीर्घ
proftime_time_डावा(proftime_T *due, proftime_T *now)
{
#  ifdef MSWIN
    LARGE_INTEGER fr;

    जर (now->QuadPart > due->QuadPart)
	परत 0;
    QueryPerformanceFrequency(&fr);
    परत (दीर्घ)(((द्विगुण)(due->QuadPart - now->QuadPart)
		   / (द्विगुण)fr.QuadPart) * 1000);
#  अन्यथा
    जर (now->tv_sec > due->tv_sec)
	परत 0;
    परत (due->tv_sec - now->tv_sec) * 1000
	+ (due->tv_fsec - now->tv_fsec) / (TV_FSEC_SEC / 1000);
#  endif
}

/*
 * Insert a timer in the list of timers.
 */
    स्थिर शून्य
घाला_timer(timer_T *timer)
{
    timer->tr_next = first_timer;
    timer->tr_prev = रिक्त;
    जर (first_timer != रिक्त)
	first_timer->tr_prev = timer;
    first_timer = timer;
    did_add_timer = TRUE;
}

/*
 * Take a timer out of the list of timers.
 */
    स्थिर शून्य
remove_timer(timer_T *timer)
{
    जर (timer->tr_prev == रिक्त)
	first_timer = timer->tr_next;
    अन्यथा
	timer->tr_prev->tr_next = timer->tr_next;
    जर (timer->tr_next != रिक्त)
	timer->tr_next->tr_prev = timer->tr_prev;
}

    स्थिर शून्य
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
create_timer(दीर्घ msec, पूर्णांक repeat)
{
    timer_T	*timer = ALLOC_CLEAR_ONE(timer_T);
    दीर्घ	prev_id = last_timer_id;

    जर (timer == रिक्त)
	परत रिक्त;
    जर (++last_timer_id <= prev_id)
	// Overflow!  Might cause duplicates...
	last_timer_id = 0;
    timer->tr_id = last_timer_id;
    घाला_timer(timer);
    जर (repeat != 0)
	timer->tr_repeat = repeat - 1;
    timer->tr_interval = msec;

    timer_start(timer);
    परत timer;
}

/*
 * (Re)start a timer.
 */
    शून्य
timer_start(timer_T *timer)
{
    profile_setlimit(timer->tr_interval, &timer->tr_due);
    timer->tr_paused = FALSE;
}

/*
 * Invoke the callback of "timer".
 */
    स्थिर शून्य
timer_callback(timer_T *timer)
{
    typval_T	rettv;
    typval_T	argv[2];

#  ifdef FEAT_EVAL
    जर (ch_log_active())
    {
	callback_T *cb = &timer->tr_callback;

	ch_log(रिक्त, "invoking timer callback %s",
	       cb->cb_partial != रिक्त ? cb->cb_partial->pt_name : cb->cb_name);
    }
#  endif

    argv[0].v_type = VAR_संख्या;
    argv[0].vval.v_संख्या = (varnumber_T)timer->tr_id;
    argv[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_UNKNOWN;
    call_callback(&timer->tr_callback, -1, &rettv, 1, argv);
    clear_tv(&rettv);

#  ifdef FEAT_EVAL
    ch_log(रिक्त, "timer callback finished");
#  endif
}

/*
 * Call timers that are due.
 * Return the time in msec until the next timer is due.
 * Returns -1 if there are no pending timers.
 */
    दीर्घ
check_due_timer(शून्य)
{
    timer_T	*timer;
    timer_T	*timer_next;
    दीर्घ	this_due;
    दीर्घ	next_due = -1;
    proftime_T	now;
    पूर्णांक		did_one = FALSE;
    पूर्णांक		need_update_screen = FALSE;
    दीर्घ	current_id = last_timer_id;

    // Don't run any timers while exiting, dealing with an error or at the
    // debug prompt.
    जर (exiting || aborting() || debug_mode)
	परत next_due;

    profile_start(&now);
    साठी (timer = first_timer; timer != रिक्त && !got_int; timer = timer_next)
    {
	timer_next = timer->tr_next;

	जर (timer->tr_id == -1 || timer->tr_firing || timer->tr_paused)
	    सुरूठेवा;
	this_due = proftime_time_डावा(&timer->tr_due, &now);
	जर (this_due <= 1)
	{
	    // Save and restore a lot of flags, because the timer fires while
	    // waiting for a character, which might be halfway a command.
	    पूर्णांक save_timer_busy = timer_busy;
	    पूर्णांक save_vgetc_busy = vgetc_busy;
	    पूर्णांक save_did_emsg = did_emsg;
	    पूर्णांक prev_uncaught_emsg = uncaught_emsg;
	    पूर्णांक save_called_emsg = called_emsg;
	    पूर्णांक save_must_redraw = must_redraw;
	    पूर्णांक save_ex_pressedreturn = get_pressedreturn();
	    पूर्णांक save_may_garbage_collect = may_garbage_collect;
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
	    जर (uncaught_emsg > prev_uncaught_emsg)
		++timer->tr_emsg_मोजणी;
	    अन्यथा
		timer->tr_emsg_मोजणी = 0;
	    did_emsg = save_did_emsg;
	    called_emsg = save_called_emsg;
	    exception_state_restore(&estate);
	    restore_vimvars(&vvsave);
	    जर (must_redraw != 0)
		need_update_screen = TRUE;
	    must_redraw = must_redraw > save_must_redraw
					      ? must_redraw : save_must_redraw;
	    set_pressedreturn(save_ex_pressedreturn);
	    may_garbage_collect = save_may_garbage_collect;

	    // Only fire the timer again if it repeats and stop_timer() wasn't
	    // called while inside the callback (tr_id == -1).
	    जर (timer->tr_repeat != 0 && timer->tr_id != -1
		    && timer->tr_emsg_मोजणी < 3)
	    {
		profile_setlimit(timer->tr_interval, &timer->tr_due);
		this_due = proftime_time_डावा(&timer->tr_due, &now);
		जर (this_due < 1)
		    this_due = 1;
		जर (timer->tr_repeat > 0)
		    --timer->tr_repeat;
	    }
	    अन्यथा
	    {
		this_due = -1;
		जर (timer->tr_keep)
		    timer->tr_paused = TRUE;
		अन्यथा
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
	जर (this_due > 0 && (next_due == -1 || next_due > this_due))
	    next_due = this_due;
    }

    जर (did_one)
	redraw_after_callback(need_update_screen, FALSE);

#  ifdef FEAT_BEVAL_TERM
    जर (bevalexpr_due_set)
    {
	this_due = proftime_time_डावा(&bevalexpr_due, &now);
	जर (this_due <= 1)
	{
	    bevalexpr_due_set = FALSE;
	    जर (balloonEval == रिक्त)
	    {
		balloonEval = ALLOC_CLEAR_ONE(BalloonEval);
		balloonEvalForTerm = TRUE;
	    }
	    जर (balloonEval != रिक्त)
	    {
		general_beval_cb(balloonEval, 0);
		setcursor();
		out_flush();
	    }
	}
	अन्यथा जर (next_due == -1 || next_due > this_due)
	    next_due = this_due;
    }
#  endif
#  ifdef FEAT_TERMINAL
    // Some terminal windows may need their buffer updated.
    next_due = term_check_timers(next_due, &now);
#  endif

    परत current_id != last_timer_id ? 1 : next_due;
}

/*
 * Find a timer by ID.  Returns NULL if not found;
 */
    स्थिर timer_T *
find_timer(दीर्घ id)
{
    timer_T *timer;

    जर (id < 0)
	परत रिक्त;

    FOR_ALL_TIMERS(timer)
	जर (timer->tr_id == id)
	    परत timer;
    परत रिक्त;
}


/*
 * Stop a timer and delete it.
 */
    शून्य
stop_timer(timer_T *timer)
{
    जर (timer->tr_firing)
	// Free the timer after the callback returns.
	timer->tr_id = -1;
    अन्यथा
    {
	remove_timer(timer);
	free_timer(timer);
    }
}

    स्थिर शून्य
stop_all_timers(शून्य)
{
    timer_T *timer;
    timer_T *timer_next;

    साठी (timer = first_timer; timer != रिक्त; timer = timer_next)
    {
	timer_next = timer->tr_next;
	stop_timer(timer);
    }
}

    स्थिर शून्य
add_timer_info(typval_T *rettv, timer_T *timer)
{
    list_T	*list = rettv->vval.v_list;
    dict_T	*dict = dict_alloc();
    dictitem_T	*di;
    दीर्घ	remaining;
    proftime_T	now;

    जर (dict == रिक्त)
	परत;
    list_append_dict(list, dict);

    dict_add_संख्या(dict, "id", timer->tr_id);
    dict_add_संख्या(dict, "time", (दीर्घ)timer->tr_interval);

    profile_start(&now);
    remaining = proftime_time_डावा(&timer->tr_due, &now);
    dict_add_संख्या(dict, "remaining", (दीर्घ)remaining);

    dict_add_संख्या(dict, "repeat",
	    (दीर्घ)(timer->tr_repeat < 0 ? -1
			     : timer->tr_repeat + (timer->tr_firing ? 0 : 1)));
    dict_add_संख्या(dict, "paused", (दीर्घ)(timer->tr_paused));

    di = dictitem_alloc((char_u *)"callback");
    जर (di != रिक्त)
    {
	जर (dict_add(dict, di) == FAIL)
	    vim_free(di);
	अन्यथा
	    put_callback(&timer->tr_callback, &di->di_tv);
    }
}

    स्थिर शून्य
add_timer_info_all(typval_T *rettv)
{
    timer_T *timer;

    FOR_ALL_TIMERS(timer)
	जर (timer->tr_id != -1)
	    add_timer_info(rettv, timer);
}

/*
 * Mark references in partials of timers.
 */
    पूर्णांक
set_ref_in_timer(पूर्णांक copyID)
{
    पूर्णांक		पात = FALSE;
    timer_T	*timer;
    typval_T	tv;

    साठी (timer = first_timer; !पात && timer != रिक्त; timer = timer->tr_next)
    {
	जर (timer->tr_callback.cb_partial != रिक्त)
	{
	    tv.v_type = VAR_PARTIAL;
	    tv.vval.v_partial = timer->tr_callback.cb_partial;
	}
	अन्यथा
	{
	    tv.v_type = VAR_FUNC;
	    tv.vval.v_string = timer->tr_callback.cb_name;
	}
	पात = पात || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    }
    परत पात;
}

/*
 * Return TRUE if "timer" exists in the list of timers.
 */
    पूर्णांक
timer_valid(timer_T *timer)
{
    जर (timer == रिक्त)
	परत FALSE;

    timer_T *t;
    FOR_ALL_TIMERS(t)
	जर (t == timer)
	    परत TRUE;
    परत FALSE;
}

#  जर defined(EXITFREE)
    शून्य
timer_free_all(शून्य)
{
    पर्यंत (first_timer != रिक्त)
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
    शून्य
f_timer_info(typval_T *argvars, typval_T *rettv)
{
    timer_T *timer = रिक्त;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	परत;

    जर (argvars[0].v_type != VAR_UNKNOWN)
    {
	timer = find_timer((पूर्णांक)tv_get_संख्या(&argvars[0]));
	जर (timer != रिक्त)
	    add_timer_info(rettv, timer);
    }
    अन्यथा
	add_timer_info_all(rettv);
}

/*
 * "timer_pause(timer, paused)" function
 */
    शून्य
f_timer_pause(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T	*timer = रिक्त;

    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_bool_arg(argvars, 1) == FAIL))
	परत;

    जर (argvars[0].v_type != VAR_संख्या)
    {
	emsg(_(e_संख्या_expected));
	परत;
    }

    पूर्णांक	paused = (पूर्णांक)tv_get_bool(&argvars[1]);

    timer = find_timer((पूर्णांक)tv_get_संख्या(&argvars[0]));
    जर (timer != रिक्त)
	timer->tr_paused = paused;
}

/*
 * "timer_start(time, callback [, options])" function
 */
    शून्य
f_timer_start(typval_T *argvars, typval_T *rettv)
{
    दीर्घ	msec;
    timer_T	*timer;
    पूर्णांक		repeat = 0;
    callback_T	callback;
    dict_T	*dict;

    rettv->vval.v_संख्या = -1;
    जर (check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	परत;

    msec = (दीर्घ)tv_get_संख्या(&argvars[0]);
    जर (argvars[2].v_type != VAR_UNKNOWN)
    {
	जर (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    परत;

	dict = argvars[2].vval.v_dict;
	जर (dict_has_किल्ली(dict, "repeat"))
	    repeat = dict_get_संख्या(dict, "repeat");
    }

    callback = get_callback(&argvars[1]);
    जर (callback.cb_name == रिक्त)
	परत;
    जर (in_vim9script() && *callback.cb_name == NUL)
    {
	// empty callback is not useful for a timer
	emsg(_(e_invalid_callback_argument));
	free_callback(&callback);
	परत;
    }

    timer = create_timer(msec, repeat);
    जर (timer == रिक्त)
    {
	free_callback(&callback);
	परत;
    }
    set_callback(&timer->tr_callback, &callback);
    rettv->vval.v_संख्या = (varnumber_T)timer->tr_id;
}

/*
 * "timer_stop(timer)" function
 */
    शून्य
f_timer_stop(typval_T *argvars, typval_T *rettv UNUSED)
{
    timer_T *timer;

    जर (check_for_संख्या_arg(argvars, 0) == FAIL)
	परत;

    timer = find_timer((पूर्णांक)tv_get_संख्या(&argvars[0]));
    जर (timer != रिक्त)
	stop_timer(timer);
}

/*
 * "timer_stopall()" function
 */
    शून्य
f_timer_stopall(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    stop_all_timers();
}

# endif // FEAT_TIMERS

# जर defined(STARTUPTIME)
स्थिर संरचना timeval	prev_timeval;

#  ifdef MSWIN
/*
 * Windows doesn't have gettimeofday(), although it does have struct timeval.
 */
    स्थिर पूर्णांक
gettimeofday(संरचना timeval *tv, अक्षर *dummy UNUSED)
{
    दीर्घ t = घड़ी();
    tv->tv_sec = t / CLOCKS_PER_SEC;
    tv->tv_usec = (t - tv->tv_sec * CLOCKS_PER_SEC) * 1000000 / CLOCKS_PER_SEC;
    परत 0;
}
#  endif

/*
 * Save the previous time before doing something that could nest.
 * set "*tv_rel" to the time elapsed so far.
 */
    शून्य
time_ढकला(शून्य *tv_rel, शून्य *tv_start)
{
    *((संरचना timeval *)tv_rel) = prev_timeval;
    gettimeofday(&prev_timeval, रिक्त);
    ((संरचना timeval *)tv_rel)->tv_usec = prev_timeval.tv_usec
					- ((संरचना timeval *)tv_rel)->tv_usec;
    ((संरचना timeval *)tv_rel)->tv_sec = prev_timeval.tv_sec
					 - ((संरचना timeval *)tv_rel)->tv_sec;
    जर (((संरचना timeval *)tv_rel)->tv_usec < 0)
    {
	((संरचना timeval *)tv_rel)->tv_usec += 1000000;
	--((संरचना timeval *)tv_rel)->tv_sec;
    }
    *(संरचना timeval *)tv_start = prev_timeval;
}

/*
 * Compute the previous time after doing something that could nest.
 * Subtract "*tp" from prev_timeval;
 * Note: The arguments are (void *) to avoid trouble with systems that don't
 * have struct timeval.
 */
    शून्य
time_काढा(
    शून्य	*tp)	// actually (struct timeval *)
{
    prev_timeval.tv_usec -= ((संरचना timeval *)tp)->tv_usec;
    prev_timeval.tv_sec -= ((संरचना timeval *)tp)->tv_sec;
    जर (prev_timeval.tv_usec < 0)
    {
	prev_timeval.tv_usec += 1000000;
	--prev_timeval.tv_sec;
    }
}

    स्थिर शून्य
time_diff(संरचना timeval *then, संरचना timeval *now)
{
    दीर्घ	usec;
    दीर्घ	msec;

    usec = now->tv_usec - then->tv_usec;
    msec = (now->tv_sec - then->tv_sec) * 1000L + usec / 1000L,
    usec = usec % 1000L;
    ख_लिखो(time_fd, "%03ld.%03ld", msec, usec >= 0 ? usec : usec + 1000L);
}

    शून्य
time_संदेश(
    अक्षर	*mesg,
    शून्य	*tv_start)  // only for do_source: start time; actually
			    // (struct timeval *)
{
    स्थिर संरचना timeval	start;
    संरचना timeval		now;

    जर (time_fd == रिक्त)
	परत;

    जर (म_माला(mesg, "STARTING") != रिक्त)
    {
	gettimeofday(&start, रिक्त);
	prev_timeval = start;
	ख_लिखो(time_fd, "\n\ntimes in msec\n");
	ख_लिखो(time_fd, " clock   self+sourced   self:  sourced script\n");
	ख_लिखो(time_fd, " clock   elapsed:              other lines\n\n");
    }
    gettimeofday(&now, रिक्त);
    time_diff(&start, &now);
    जर (((संरचना timeval *)tv_start) != रिक्त)
    {
	ख_लिखो(time_fd, "  ");
	time_diff(((संरचना timeval *)tv_start), &now);
    }
    ख_लिखो(time_fd, "  ");
    time_diff(&prev_timeval, &now);
    prev_timeval = now;
    ख_लिखो(time_fd, ": %s\n", mesg);
}
# endif	// STARTUPTIME
#endif // FEAT_EVAL

#जर defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 8 bytes from "fd" and turn them into a time_T, MSB first.
 * Returns -1 when encountering EOF.
 */
    time_T
get8ctime(खाता *fd)
{
    पूर्णांक		c;
    time_T	n = 0;
    पूर्णांक		i;

    साठी (i = 0; i < 8; ++i)
    {
	c = अ_लो(fd);
	जर (c == खातापूर्ण) परत -1;
	n = (n << 8) + c;
    }
    परत n;
}

/*
 * Write time_T to file "fd" in 8 bytes.
 * Returns FAIL when the write failed.
 */
    पूर्णांक
put_time(खाता *fd, time_T the_time)
{
    char_u	buf[8];

    time_to_bytes(the_time, buf);
    परत ख_डालो(buf, (माप_प्रकार)8, (माप_प्रकार)1, fd) == 1 ? OK : FAIL;
}

/*
 * Write time_T to "buf[8]".
 */
    शून्य
time_to_bytes(time_T the_time, char_u *buf)
{
    पूर्णांक		c;
    पूर्णांक		i;
    पूर्णांक		bi = 0;
    time_T	wtime = the_time;

    // time_T can be up to 8 bytes in size, more than long_u, thus we
    // can't use put_bytes() here.
    // Another problem is that ">>" may do an arithmetic shift that keeps the
    // sign.  This happens for large values of wtime.  A cast to long_u may
    // truncate if time_T is 8 bytes.  So only use a cast when it is 4 bytes,
    // it's safe to assume that long_u is 4 bytes or more and when using 8
    // bytes the top bit won't be set.
    साठी (i = 7; i >= 0; --i)
    {
	जर (i + 1 > (पूर्णांक)आकार(time_T))
	    // ">>" doesn't work well when shifting more bits than avail
	    buf[bi++] = 0;
	अन्यथा
	{
# जर defined(SIZEOF_TIME_T) && SIZEOF_TIME_T > 4
	    c = (पूर्णांक)(wtime >> (i * 8));
# अन्यथा
	    c = (पूर्णांक)((long_u)wtime >> (i * 8));
# endif
	    buf[bi++] = c;
	}
    }
}

#endif

/*
 * Put timestamp "tt" in "buf[buflen]" in a nice format.
 */
    शून्य
add_time(char_u *buf, माप_प्रकार buflen, समय_प्रकार tt)
{
#ifdef HAVE_STRFTIME
    संरचना पंचांग	tmval;
    संरचना पंचांग	*curtime;
    माप_प्रकार	n;

    जर (vim_time() - tt >= 100)
    {
	curtime = vim_localtime(&tt, &tmval);
	जर (vim_time() - tt < (60L * 60L * 12L))
	    // within 12 hours
	    n = स_माला((अक्षर *)buf, buflen, "%H:%M:%S", curtime);
	अन्यथा
	    // longer ago
	    n = स_माला((अक्षर *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
	जर (n == 0)
	    buf[0] = NUL;
    }
    अन्यथा
#endif
    {
	दीर्घ seconds = (दीर्घ)(vim_time() - tt);

	vim_snprintf((अक्षर *)buf, buflen,
		NGETTEXT("%ld second ago", "%ld seconds ago", seconds),
		seconds);
    }
}
