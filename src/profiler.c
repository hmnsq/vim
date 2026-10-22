/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * profiler.c: Vim script profiler
 */

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)
# إذا defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    فراغ
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  وإلا
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    فراغ
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  وإلا
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    إذا (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    فراغ
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  وإلا
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    إذا (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Return a string that represents the time in "tm".
 * Uses a static buffer!
 */
    حرف *
profile_رسالة(proftime_T *tm)
{
    ساكن حرف buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (مزدوج)tm->QuadPart / (مزدوج)fr.QuadPart);
#  وإلا
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    إرجاع buf;
}

/*
 * Return a float that represents the time in "tm".
 */
    float_T
profile_float(proftime_T *tm)
{
#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    إرجاع (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  وإلا
    إرجاع (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    فراغ
profile_setlimit(long msec, proftime_T *tm)
{
    إذا (msec <= 0)   // no limit
	profile_zero(tm);
    وإلا
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((مزدوج)msec / 1000.0 * (مزدوج)fr.QuadPart);
#  وإلا
	varnumber_T	    fsec;	// this should be 64 bit if possible

	PROF_GET_TIME(tm);
	fsec = (varnumber_T)tm->tv_fsec
		       + (varnumber_T)msec * (varnumber_T)(TV_FSEC_SEC / 1000);
	tm->tv_fsec = fsec % (long)TV_FSEC_SEC;
	tm->tv_sec += fsec / (long)TV_FSEC_SEC;
#  endif
    }
}

/*
 * Return TRUE if the current time is past "tm".
 */
    صحيح
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    إذا (tm->QuadPart == 0)  // timer was not set
	إرجاع FALSE;
    QueryPerformanceCounter(&now);
    إرجاع (now.QuadPart > tm->QuadPart);
#  وإلا
    إذا (tm->tv_sec == 0)    // timer was not set
	إرجاع FALSE;
    PROF_GET_TIME(&now);
    إرجاع (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    فراغ
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  وإلا
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# إذا defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  إذا defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    فراغ
profile_divide(proftime_T *tm, صحيح تعداد_2, proftime_T *tm2)
{
    إذا (تعداد_2 == 0)
	profile_zero(tm2);
    وإلا
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / تعداد_2;
#  وإلا
	مزدوج fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / تعداد_2;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# إذا defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
ساكن proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    فراغ
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  وإلا
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    إذا (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    فراغ
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    إذا (total->QuadPart <= children->QuadPart)
	إرجاع;
#  وإلا
    إذا (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	إرجاع;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    ساكن فراغ
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    فراغ
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    ساكن صحيح
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    إرجاع (tm1->QuadPart == tm2->QuadPart);
#  وإلا
    إرجاع (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    صحيح
profile_cmp(ثابت proftime_T *tm1, ثابت proftime_T *tm2)
{
#  ifdef MSWIN
    إرجاع tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  وإلا
    إذا (tm1->tv_sec == tm2->tv_sec)
	إرجاع tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    إرجاع tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

ساكن char_u	*profile_fname = عدم;
ساكن proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    ساكن فراغ
profile_reset(فراغ)
{
    صحيح		id;
    صحيح		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    لكل (id = 1; id <= script_عناصر.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	إذا (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_تعداد     = 0;
	    profile_zero(&si->sn_pr_total);
	    profile_zero(&si->sn_pr_self);
	    profile_zero(&si->sn_pr_start);
	    profile_zero(&si->sn_pr_children);
	    ga_clear(&si->sn_prl_ga);
	    profile_zero(&si->sn_prl_start);
	    profile_zero(&si->sn_prl_children);
	    profile_zero(&si->sn_prl_wait);
	    si->sn_prl_idx      = -1;
	    si->sn_prl_execed   = 0;
	}
    }

    // Reset functions.
    functbl = func_tbl_get();
    todo = (صحيح)functbl->ht_used;

    FOR_ALL_HASHTAB_عناصر(functbl, hi, todo)
    {
	ufunc_T *fp;
	صحيح	i;

	إذا (HASHITEM_EMPTY(hi))
	    استمر;

	todo--;
	fp = HI2UF(hi);
	إذا (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_تعداد     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    لكل (i = 0; i < fp->uf_أسطر.ga_len; i++)
	    {
		fp->uf_tml_تعداد[i] = 0;
		profile_zero(&fp->uf_tml_total[i]);
		profile_zero(&fp->uf_tml_self[i]);
	    }

	    profile_zero(&fp->uf_tml_start);
	    profile_zero(&fp->uf_tml_children);
	    profile_zero(&fp->uf_tml_wait);
	    fp->uf_tml_idx      = -1;
	    fp->uf_tml_execed   = 0;
	}
    }

    VIM_CLEAR(profile_fname);
}

/*
 * ":profile cmd args"
 */
    فراغ
ex_profile(exarg_T *eap)
{
    char_u	*e;
    صحيح		len;

    e = skiptowhite(eap->arg);
    len = (صحيح)(e - eap->arg);
    e = skipwhite(e);

    إذا (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, عدم);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    وإلا إذا (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    وإلا إذا (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    وإلا إذا (STRCMP(eap->arg, "pause") == 0)
    {
	إذا (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    وإلا إذا (STRCMP(eap->arg, "continue") == 0)
    {
	إذا (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    وإلا إذا (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    وإلا
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
ساكن تعداد
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

ساكن حرف *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			عدم
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, صحيح idx)
{
    بدل (pexpand_what)
    {
    حالة PEXP_SUBCMD:
	إرجاع (char_u *)pexpand_cmds[idx];
    افتراضي:
	إرجاع عدم;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    فراغ
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    إذا (*end_subcmd == NUL)
	إرجاع;

    إذا ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	إرجاع;
    }
    وإلا إذا (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	إرجاع;
    }

    xp->xp_context = EXPAND_NOTHING;
}

ساكن proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    فراغ
prof_inchar_enter(فراغ)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    فراغ
prof_inchar_exit(فراغ)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    صحيح
prof_def_func(فراغ)
{
    إذا (current_sctx.sc_sid > 0)
	إرجاع SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    إرجاع FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    ساكن فراغ
prof_func_سطر(
    FILE	*fd,
    صحيح		تعداد_2,
    proftime_T	*total,
    proftime_T	*self,
    صحيح		prefer_self)	// when equal print only self time
{
    إذا (تعداد_2 > 0)
    {
	fprintf(fd, "%5d ", تعداد_2);
	إذا (prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	وإلا
	    fprintf(fd, "%s ", profile_رسالة(total));
	إذا (!prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	وإلا
	    fprintf(fd, "%s ", profile_رسالة(self));
    }
    وإلا
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    ساكن فراغ
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    صحيح		st_len,
    حرف	*title,
    صحيح		prefer_self)	// when equal print only self time
{
    صحيح		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    لكل (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_سطر(fd, fp->uf_tm_تعداد, &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	إذا (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	وإلا
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    ساكن صحيح
prof_total_cmp(ثابت فراغ *s1, ثابت فراغ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    إرجاع profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    ساكن صحيح
prof_self_cmp(ثابت فراغ *s1, ثابت فراغ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    إرجاع profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    فراغ
func_do_profile(ufunc_T *fp)
{
    صحيح		len = fp->uf_أسطر.ga_len;

    إذا (!fp->uf_prof_initialized)
    {
	إذا (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_تعداد = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	إذا (fp->uf_tml_تعداد == عدم)
	    fp->uf_tml_تعداد = ALLOC_CLEAR_MULT(صحيح, len);
	إذا (fp->uf_tml_total == عدم)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	إذا (fp->uf_tml_self == عدم)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	إذا (fp->uf_tml_تعداد == عدم || fp->uf_tml_total == عدم
						    || fp->uf_tml_self == عدم)
	    إرجاع;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    ساكن فراغ
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    إذا (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	إذا (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    فراغ
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    إذا (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, عدم,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    إذا (fp->uf_profiling || (caller != عدم && caller->uf_profiling))
    {
	++fp->uf_tm_تعداد;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    فراغ
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    إذا (caller != عدم && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    إذا (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    فراغ
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    إذا (fc != عدم && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    فراغ
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    إذا (fc != عدم && fc->fc_func->uf_profiling)
    {
	profile_end(&fc->fc_prof_child);
	profile_sub_wait(tm, &fc->fc_prof_child); // don't count waiting time
	profile_add(&fc->fc_func->uf_tm_children, &fc->fc_prof_child);
	profile_add(&fc->fc_func->uf_tml_children, &fc->fc_prof_child);
    }
    script_prof_restore(tm);
}

/*
 * Called when starting to read a function line.
 * "sourcing_lnum" must be correct!
 * When skipping lines it may not actually be executed, but we won't find out
 * until later and we need to store the time now.
 */
    فراغ
func_سطر_start(فراغ *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    إذا (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_أسطر.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	بينما (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == عدم)
	    --fp->uf_tml_idx;
	fp->uf_tml_execed = FALSE;
	profile_start(&fp->uf_tml_start);
	profile_zero(&fp->uf_tml_children);
	profile_get_wait(&fp->uf_tml_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    فراغ
func_سطر_exec(فراغ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    إذا (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    فراغ
func_سطر_end(فراغ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    إذا (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	إذا (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_تعداد[fp->uf_tml_idx];
	    profile_end(&fp->uf_tml_start);
	    profile_sub_wait(&fp->uf_tml_wait, &fp->uf_tml_start);
	    profile_add(&fp->uf_tml_total[fp->uf_tml_idx], &fp->uf_tml_start);
	    profile_self(&fp->uf_tml_self[fp->uf_tml_idx], &fp->uf_tml_start,
							&fp->uf_tml_children);
	}
	fp->uf_tml_idx = -1;
    }
}

/*
 * Dump the profiling results for all functions in file "fd".
 */
    ساكن فراغ
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    صحيح		todo;
    ufunc_T	*fp;
    صحيح		i;
    ufunc_T	**sorttab;
    صحيح		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (صحيح)functbl->ht_used;
    إذا (todo == 0)
	إرجاع;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_عناصر(functbl, hi, todo)
    {
	إذا (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    إذا (fp->uf_prof_initialized)
	    {
		إذا (sorttab != عدم)
		    sorttab[st_len++] = fp;

		إذا (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		وإلا
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		إذا (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(عدم,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    إذا (p != عدم)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		إذا (fp->uf_tm_تعداد == 1)
		    fprintf(fd, "Called 1 time\n");
		وإلا
		    fprintf(fd, "Called %d times\n", fp->uf_tm_تعداد);
		fprintf(fd, "Total time: %s\n", profile_رسالة(&fp->uf_tm_total));
		fprintf(fd, " Self time: %s\n", profile_رسالة(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		لكل (i = 0; i < fp->uf_أسطر.ga_len; ++i)
		{
		    إذا (FUNCLINE(fp, i) == عدم)
			استمر;
		    prof_func_سطر(fd, fp->uf_tml_تعداد[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    إذا (sorttab != عدم && st_len > 0)
    {
	qsort((فراغ *)sorttab, (size_t)st_len, حجم(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((فراغ *)sorttab, (size_t)st_len, حجم(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    فراغ
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_تعداد = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, حجم(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    فراغ
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	إرجاع;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    إذا (si->sn_prof_on && --si->sn_pr_nest == 0)
    {
	profile_end(&si->sn_pr_child);
	profile_sub_wait(tm, &si->sn_pr_child); // don't count wait time
	profile_add(&si->sn_pr_children, &si->sn_pr_child);
	profile_add(&si->sn_prl_children, &si->sn_pr_child);
    }
}

/*
 * Dump the profiling results for all scripts in file "fd".
 */
    ساكن فراغ
script_dump_profile(FILE *fd)
{
    صحيح		    id;
    scriptitem_T    *si;
    صحيح		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    لكل (id = 1; id <= script_عناصر.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	إذا (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    إذا (si->sn_pr_تعداد == 1)
		fprintf(fd, "Sourced 1 time\n");
	    وإلا
		fprintf(fd, "Sourced %d times\n", si->sn_pr_تعداد);
	    fprintf(fd, "Total time: %s\n", profile_رسالة(&si->sn_pr_total));
	    fprintf(fd, " Self time: %s\n", profile_رسالة(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((حرف *)si->sn_name, "r");
	    إذا (sfd == عدم)
		fprintf(fd, "Cannot open file!\n");
	    وإلا
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		لكل (i = 0; ; ++i)
		{
		    إذا (vim_fgets(IObuff, IOSIZE, sfd))
			اكسر;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    إذا (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			صحيح n = IOSIZE - 2;

			إذا (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    بينما (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			وإلا إذا (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    إذا (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_تعداد > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_تعداد);
			إذا (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			وإلا
			    fprintf(fd, "%s ", profile_رسالة(&pp->sn_prl_total));
			fprintf(fd, "%s ", profile_رسالة(&pp->sn_prl_self));
		    }
		    وإلا
			fprintf(fd, "                            ");
		    fprintf(fd, "%s", IObuff);
		}
		fclose(sfd);
	    }
	    fprintf(fd, "\n");
	}
    }
}

/*
 * Dump the profiling info.
 */
    فراغ
profile_dump(فراغ)
{
    FILE	*fd;

    إذا (profile_fname == عدم)
	إرجاع;

    fd = mch_fopen((حرف *)profile_fname, "w");
    إذا (fd == عدم)
	semsg(_(e_cant_open_file_str), profile_fname);
    وإلا
    {
	script_dump_profile(fd);
	func_dump_profile(fd);
	fclose(fd);
    }
}

/*
 * Called when starting to read a script line.
 * "sourcing_lnum" must be correct!
 * When skipping lines it may not actually be executed, but we won't find out
 * until later and we need to store the time now.
 */
    فراغ
script_سطر_start(فراغ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	إرجاع;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    إذا (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(فراغ)ga_grow(&si->sn_prl_ga,
				  (صحيح)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	بينما (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_تعداد = 0;
	    profile_zero(&pp->sn_prl_total);
	    profile_zero(&pp->sn_prl_self);
	    ++si->sn_prl_ga.ga_len;
	}
	si->sn_prl_execed = FALSE;
	profile_start(&si->sn_prl_start);
	profile_zero(&si->sn_prl_children);
	profile_get_wait(&si->sn_prl_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    فراغ
script_سطر_exec(فراغ)
{
    scriptitem_T    *si;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	إرجاع;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    إذا (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    فراغ
script_سطر_end(فراغ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	إرجاع;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    إذا (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	إذا (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_تعداد;
	    profile_end(&si->sn_prl_start);
	    profile_sub_wait(&si->sn_prl_wait, &si->sn_prl_start);
	    profile_add(&pp->sn_prl_total, &si->sn_prl_start);
	    profile_self(&pp->sn_prl_self, &si->sn_prl_start,
							&si->sn_prl_children);
	}
	si->sn_prl_idx = -1;
    }
}
# endif // FEAT_PROFILE

#endif
